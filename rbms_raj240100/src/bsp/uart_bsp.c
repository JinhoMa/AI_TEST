/******************************************************************************
* DISCLAIMER                                                                    *
* This software is supplied by Renesas Electronics Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.
*
* This software is owned by Renesas Electronics Corp. and is protected under 
* all applicable laws, including copyright laws.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.
*
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORP. NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/

/*""FILE COMMENT""*******************************************************
* System Name	: RAJ240090/100 Stack Firmware
* File Name		: uart.c
* Version		: 0.01
* Contents		: UART functions
* Customer		: Renesas Electronics Corp.
* Model			: 
* Order			: 
* CPU			: RAJ240090/100
* Compiler		: CCRL
* OS			: None
* Programmer	: Yasuo Usuda
* Note			: 
************************************************************************
* Copyright,2012-2012 RENESAS ELECTRONICS CORPORATION
*                     AND RENESAS SOLUTIONS CORPORATION
************************************************************************
* History		: 2012.05.01 Ver 0.01
* 				: 륷딮띿맟 New create
* 
*""FILE COMMENT END""**************************************************/

#define _UART



/***********************************************************************
** Include files
***********************************************************************/
#include "BMS.h"
#include "define.h"								// union/define definition
#include "afe.h"
#include "ram.h"
#include "mode.h"
#include "main.h"

#pragma	interrupt Uart_int(vect=INTSR1)			// UART 1 reception transfer end
#pragma	interrupt Uart_error_int(vect=INTSRE1)
#pragma interrupt _int_UART_TxD(vect=INTST1)

#define UART_TMPSIZE  0x40

// - Prototype definition -
void Uart_err_unsupportcmd(void);				// UART ERROR UNSUPPORTED COMMAND function
void Uart_err_timeout(void);					// UART ERROR TIMEOUT function
void Uart_err_timeout_master(void);				// UART ERROR TIMEOUT(Master IC) function
void Uart_hw_err(BYTE factor);					// UART H/W ERROR function

BYTE uart_err;
// - variable -
BOOL	buartstop;								// receive stop flag
int uart_txleft_count;
int uart_txrear;
BYTE uart_acnt = 0;

// - External variable -
extern const BYTE	aver;


/* PRS00 serial clock select register setting value */
const BYTE	auart_clk[4] =	{	0x0001,			// PRS00 : 2MHz(4MHz/2)
								0x0001,			// PRS00 : 2MHz(4MHz/2)
								0x0002,			// PRS00 : 2MHz(8MHz/4)
								0x0004			// PRS00 : 2MHz(32MHz/16)
							};

extern BYTE	acmode;								// Flash operation mode setting


/*""FUNC COMMENT""***************************************************
* ID : 1.0
* module outline: UART communication reset function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration	: void Uart_reset(void)
*-------------------------------------------------------------------
* Function		: Execute UART communication reset function
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function : 
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2019.03.15 Ver 0.01
* 				: New create 
*""FUNC COMMENT END""**********************************************/
void Uart_reset(BYTE err)
{
    if(err != 0 ){
        uart_err = err;
        armod = UART_IDLE;						// IDLE
        buarttout_req = 0;						// Stop the timeout measurement
        auarttout = 0;							// Clear timeout counter
        buartstop = 0;							// receive process enabled
        uartdata_cnt = 0;
        uart_acnt=0;

    }else{
        armod = UART_IDLE;						// IDLE
        buarttout_req = 0;						// Stop the timeout measurement
        auarttout = 0;							// Clear timeout counter
        buartstop = 0;							// receive process enabled
        uartdata_cnt = 0;
        uart_acnt=0;
    }  // Receive counter clear
}

/*""FUNC COMMENT""***************************************************
* ID : 1.0
* module outline: UART intialize function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration	: void Uart_BspInit(void)
*-------------------------------------------------------------------
* Function		: Execute UART intialize function
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function : 
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2019.03.15 Ver 0.01
* 				: New create 
*""FUNC COMMENT END""**********************************************/
void Uart_BspInit(void)
{
	Uart_reset(0);
											// - UART related register -
	SAU0EN = 0;								// UART disable
	SAU0EN = 1;								// UART enable
	SPS0 = UART_SPS0_115200;				// PRS00 : 2MHz
	ST0 |= 0x0003;					// disable UART0 receive and transmit
	STMK1 = 1;								// disable INTST1 interrupt
	STIF1 = 0;								// clear INTST1 interrupt flag
	SRMK1 = 1;								// disable INTSR1 interrupt
	SRIF1 = 0;								// clear INTSR1 interrupt flag
	SREMK1 = 1;								// disable INTSRE1 interrupt
	SREIF1 = 0;								// clear INTSRE1 interrupt flag
	/* Set INTST1 low priority */
	STPR11 = 1;
	STPR01 = 1;
	/* Set INTSR1 high priority */ 
     // CAN전송 Timer0 인터럽트보다 높게 설정하지 않으면 Overrun Error가 발생함. 
	SRPR11 = 0;
	SRPR01 = 0;
	/* Set INTSRE0 low priority */
	SREPR11 = 1;
	SREPR01 = 1;

	// Transmision
	SMR02 = 0x0022;				// operation clock CK0 set by PRS register
											// falling edge is detected as the start bit
											// UART mode
											// transfer end interrupt
	SCR02 = 0x8097;				// transmission only
											// no parity
											// LSB
											// 1 stop bit
											// 8-bit data length
	SDR02 = UART_BPS_115200;					// baudrate 9600bps
	NFEN1_bit.no0 = 1;						// noise filter on
	
	// Reception
	SIR03L = 0x0007;					// Clear trigger of framing error,overrun error,parity error
	SMR03 = 0x0122;				// operation clock CK0 set by PRS register
											// falling edge is detected as the start bit
											// valid edge of RXD pin
											// UART mode
											// transfer end interrupt
	SCR03 = 0x4097;				// reception only
											// no parity
											// LSB
											// 1 stop bit
											// 8-bit data length
	SDR03 = UART_BPS_115200;					// baudrate 9600bps
	
	SO0 |= 0b00000100;						// SO02:1
	SOL0 |= 0b00000000;						// output level normal
	SOE0L_bit.no2 = 1;

//	POM1_bit.no5 = 1;						// TXD1 pin open drain output
	POM1_bit.no5 = 0;						// TXD1 pin Normal mode output

	/* Set RxD1 pin */
	PM1 |= 0x10;
	/* Set TxD1 pin */
	P1 |= 0x20;
	PM1 &= 0xDF;

	STIF1 = 0;								// clear INTST1 interrupt flag
	SRIF1 = 0;								// clear INTSR1 interrupt flag
	SREIF1 = 0;								// clear INTSRE1 interrupt flag
	SRMK1 = 0;                              // Enable INTSR1 interrupt
	SREMK1 = 0;                             // Disable  INTSRE1 interrupt 
	SS0L_bit.no2 = 1;						// enable UART transmit
	SS0L_bit.no3 = 1;						// enable UART receive
}

/*""FUNC COMMENT""***************************************************
* ID : 1.0
* module outline: UART communication stop function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration	: void Uart_stop(void)
*-------------------------------------------------------------------
* Function		: Execute UART communication stop function
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function : 
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2019.03.15 Ver 0.01
* 				: New create 
*""FUNC COMMENT END""**********************************************/
void Uart_stop(void)
{
											// - UART related register -
	SAU0EN = 0;								// UART disable
}

void _int_UART_TxD(void)
{
	if ( uart_txleft_count)
	{
	   TXD1 = uart_txdata[uart_txrear];
           uart_txrear = (uart_txrear+1)%UART_BUF_SIZE;
           uart_txleft_count--;
	}
	else
	{
		STMK1=1; //interrupt disable
	}
}
 


/*""FUNC COMMENT""***************************************************
* ID : 1.0
* module outline: UART send function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration	: void Uart_snd(char mode, unsigned char *buf, unsigned char len)
*-------------------------------------------------------------------
* Function		: UART send function
*-------------------------------------------------------------------
* Argument		: char mode				Mode
*				: unsigned char *buf	Address of send data
*				: unsigned char len		Send data length
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function : 
*-------------------------------------------------------------------
* Caution		: mode=0	Send with STX,ETX
*				: mode=1	Send without STX,ETX
*-------------------------------------------------------------------
* History		: 2019.03.15 Ver 0.01
* 				: New create 
*""FUNC COMMENT END""**********************************************/
void Uart_BspSend(char mode, unsigned char *buf, unsigned char len)
{
    BYTE	acnt;
    STMK1 = 1;					// disable INTST1 interrupt
    for( acnt=0; acnt<len; acnt++ ){
        uart_txdata[uart_txfront] =  (BYTE)(*(buf+acnt));
        uart_txfront = (uart_txfront+1)%UART_BUF_SIZE;
        uart_txleft_count++;
    }
    TXD1= uart_txdata[uart_txrear];
    uart_txrear = (uart_txrear+1)%UART_BUF_SIZE;
    uart_txleft_count--;
    STMK1 = 0;					// enable INTST1 interrupt
}

/*""FUNC COMMENT""***************************************************
* ID : 1.0
* module outline: UART ERROR TIMEOUT function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration	: void Uart_err_timeout_master(void)
*-------------------------------------------------------------------
* Function		: UART ERROR TIMEOUT function갃
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function : 
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* * History		: 2019.03.15 Ver 0.01
* 				: New create 
*""FUNC COMMENT END""**********************************************/
void Uart_err_timeout_master(void)
{
	Uart_reset(UARTRST_SLVTIMEOUT);							// UART timeout Initialize function
}

BYTE  BUF[UART_TMPSIZE];

/*""FUNC COMMENT""***************************************************
* ID : 1.0
* module outline: UART receive interrupt function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration	: void Uart_rcv_int(void)
*-------------------------------------------------------------------
* Function		: UART receive interrupt function
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function : 
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2019.03.15 Ver 0.01
* 				: New create 
*""FUNC COMMENT END""**********************************************/
BYTE CAL_CRC;

void Uart_int(void)
{
#if 1
    volatile unsigned char err_type;
   
    //if( !(SSR03L & 0b00100000) ){			// there is no received data ?
    //    return;
    //}
    err_type = SSR03L & 0b00000111;

    if( err_type != 0  ){				// If there is receive error,
        uart_err_count++;
        SIR03L = 0b00000111;				// Clear error
    }

    bspRcvData = RXD1;						// Read data from receive buffer register
    HAL_UART_RxCpltCallback(NULL);
#else
    BYTE auartdat,i;
    volatile unsigned char err_type;
   
    //if( !(SSR03L & 0b00100000) ){			// there is no received data ?
    //    return;
    //}
    err_type = SSR03L & 0b00000111;

    if( err_type != 0  ){				// If there is receive error,
        uart_err_count++;
        SIR03L = 0b00000111;				// Clear error
    }

    auartdat = RXD1;						// Read data from receive buffer register
    uart_rxdata[uart_rxfront] = auartdat;
    uart_rxfront =(uart_rxfront+1)%UART_BUF_SIZE;
    if( buartstop == 1){					// Exit function after reading from receive buffer register in reception disabled.
        return;								//
    }

    // Master IC receive process

    switch( armod ){
        case UART_IDLE:							// IDLE
            if( auartdat != STX ){				// Confirm that received data is STX
                return;
            }
            armod = UART_LENGTH;				// Length check is next
            buarttout_req = 1;					// Start timeout measurement
            auarttout = 0;						// Clear timeout counter
            uartdata_cnt = 0;							// Receive counter clear
            break;

        case UART_LENGTH:
            uartdata_cnt = auartdat;					// Set Data Lenght(Except for STX)
            armod = UART_COMCHK;				// Command check is next
            if(f_SlvMC == ON){
                CAL_CRC = auartdat^0xFF;
                if( uartdata_cnt != (sizeof(_MEAS)+3) ){
                    Uart_reset(UARTRST_DATARCVERR);
                }
            }
            break;

        case UART_COMCHK:						// Judgemnet Code check
            arcmd = auartdat;					// Set the command
            switch( arcmd ){
                case UART_COM_SUCCESS:
                case UART_UNSPT_CMD:
                case UART_RCVTO:
                case UART_HW_ERR:
                case UART_TEST_MOD:
                    armod = UART_DATACHK;			// Data check is next
                    CAL_CRC ^= auartdat;
                    uartdata_cnt--;
                    break;
                case UART_SLV_ASYNCSND:
                    armod = UART_DATACHK;
                    uartdata_cnt--;
                    break;
                default:
                Uart_reset(UARTRST_DATARCVERR);					// UART reset
                return;
            }
            break;
        case UART_DATACHK:

            switch (arcmd){
                case UART_COM_SUCCESS:			// Between Master IC and Slave IC communication succeeds
                    // voltage and temperature
                    if ( f_SlvMC == ON) {
                        uartdata_cnt--;
                        if(uartdata_cnt > 2){
                            BUF[acnt] = auartdat; //ameasdat[acnt+1] = auartdat;
                            CAL_CRC^= auartdat;
                            acnt++;
                        } else {					// 
                            BUF[acnt] = auartdat; 
                            CAL_CRC^=auartdat;
                            armod = UART_CRCCHK;			// ETX reception is next
                            acnt = 0;
                            f_SlvMC = OFF;
                        }
                    } else {
                        // Operation mode
                        if ( f_SlvOPM == ON) {
                            aslvmode = auartdat;		// 
                            armod = UART_END;			// ETX reception is next
                        } else if( f_SlvCal == ON){
                            if(auartdat == UART_CAL_LVOLALL ){
                                f_SlvADC= OFF;
                                aslvmode = auartdat;		// 
                                armod = UART_END;			// ETX reception is next
                            }else if(auartdat == UART_CAL_LVOLALL ){
                                f_SlvADC = OFF;
                                aslvmode = auartdat;		// 
                                armod = UART_END;			// ETX reception is next
                            }else{		// Response Code Error
                                f_SlvADC = OFF;
                                armod = UART_END;
                            }

                        }else
                        {
                            if ( auartdat == UART_CAL_START){
                                f_SlvCal = ON;     
                            }

                            if( auartdat == UART_CMD_PDN){
                                f_SlvPDN = ON;
                            }
                            // FET control
                            if( f_SlvFETC == ON) {
                                uartdata_cnt--;
                                armod = UART_END;				// ETX reception is next
                            } else {
                                // Cell balancing
                                if( f_SlvCB == ON){
                                    uartdata_cnt--;
                                    armod = UART_END;				// ETX reception is next
                                } else {

                                }
                            }
                         }
                    }
                break;

                case UART_UNSPT_CMD:			// Slave IC mode number
                    uartdata_cnt--;
                    //auart_unsupport = auartdat;
                    t_com47 = UART_UNSPT_CMD;
                    armod = UART_END;			// ETX reception is next
                    break;

                case UART_RCVTO:				// Control C-FET and D-FET
                    uartdata_cnt--;
                    if ( auartdat == 0x00) {
                        t_com47 = UART_RCVTO;
                        armod = UART_END;	// ETX reception is next
                    }
                    break;

                case UART_HW_ERR:
                    uartdata_cnt--;
                    t_com47 = auartdat;
                    armod = UART_END;			// ETX reception is next
                    break;
                case TEST_MODE_SUCESS:
                    uartdata_cnt--;
#if 0      // test              
                    t_com6 = auartdat;
#endif
                    armod = UART_END;			// ETX reception is next
                    break;
                case UART_SLV_ASYNCSND:
                    armod = UART_END;			// ETX reception is next        
                    aslvmode = auartdat;        // 
                    uartdata_cnt--;
                    break;
            }
            break;
        case UART_CRCCHK:
            if( CAL_CRC != auartdat){
               Uart_reset(UARTRST_DATARCVERR);
               return;
            }
            for(i=0;i<MEASSIZE;i++){
                smeas.uc[i]=BUF[i];;
            }
            armod = UART_END;
            break;
        case UART_END:
            if( auartdat != ETX ){				// Confirm that the received data is ETX or not,
                Uart_reset(UARTRST_DATARCVERR);					// if received data isn't ETX, call UART reset function
                return;
            }
            uart_rxdata[rxfront]=0x7F;
            rxfront = (rxfront+1)%BUF_SIZE;
            uart_rxdata[rxfront]=0x7F;
            rxfront = (rxfront+1)%BUF_SIZE;

            switch( arcmd ){

                case UART_COM_SUCCESS:
                    f_SlvOPM = OFF;
                    f_SlvFETC = OFF;
                    f_SlvCB = OFF;
                    //LED_ON;
                    break;
                case UART_UNSPT_CMD:
                    if(f_SlvFETC) f_SlvFETC =0;
                    break;
                case UART_RCVTO:
                case UART_HW_ERR:
                    //  LED_OFF ;
                    break;
                case UART_SLV_ASYNCSND:
                    f_SlvAysncSnd = ON;
                    break;
                 default:
                    Uart_reset(UARTRST_DATARCVERR);
                return;
            }
            Uart_reset(0);
            break;
        default:
            Uart_reset(UARTRST_DATARCVERR);						// Call UART reset function
        return;
    }
#endif
}

/*""FUNC COMMENT""***************************************************
* ID : 1.0
* module outline: UART error interrupt function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration	: void Uart_error_int(void)
*-------------------------------------------------------------------
* Function		: UART error interrupt function
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function : 
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2012.05.01 Ver 0.01
* 				: New create
* 
*""FUNC COMMENT END""**********************************************/
void Uart_error_int(void)
{
	BYTE	auart_err;
	
	SREIF1 = 0;
	
	if( SSR03L & 0b00000111 ){				// If there is receive error,
		auart_err = SSR03L;
		auart_err = auart_err & 0x07;
		SIR03L = 0b00000111;				// Clear error
		switch ( auart_err ) {
			case UART_HW_ERR_PRT:
				Uart_reset(UARTRST_HWERR);
				break;
		
			case UART_HW_ERR_FRM:
				Uart_reset(UARTRST_HWERR);
				break;
			
			case UART_HW_ERR_OVR:
				Uart_reset(UARTRST_HWERR);
				break;
			
			default:
				break;
		}
	}
}

BYTE  Uart1_Data;
BYTE  Uart_Poll (void){
    //if( !(SSR03L & 0b00100000) ){			// there is no received data ?
   //     return NULL;
   // }
      
    Uart1_Data = RXD1;						// Read data from receive buffer register
    return Uart1_Data;
}

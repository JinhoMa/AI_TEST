/******************************************************************************
* DISCLAIMER																	*
* This software is supplied by Renesas Electronics Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.
*
* This software is owned by Renesas Electronics Corp. and is protected under 
* all applicable laws, including copyright laws.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.	ALL SUCH WARRANTIES ARE EXPRESSLY 
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
* System Name	: RAJ240090 Sample Code
* File Name		: can_user.c
* Version		: 1.00
* Contents		: CAN interrupt functions
* Programmer	: Yasuo Usuda
* Note			: 
************************************************************************
* Copyright(C) 2016 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History			: 2016.02.21 Ver 1.00
 *					:  New Create
 *					: 
*""FILE COMMENT END""**************************************************/
#include "BMS.h"
#include "iodefine.h"
#include "define.h"
#include "dataflash.h"
#include "ram.h"
#include "kousei.h"
#include "afe.h"
#include "can_ex.h"
#include <string.h>
#include "main.h"

#define _CAN

//MAIL BOX를 찾은후 TX 데이터를 WRITE 할려고 할 이미 데이터가 바뀌었음. 
//TX 를 하기전에 Interrupt를 DISABLE 한다.
#define CASE_1      

//MAIL BOX를 Reserved 한다. 
// MB1 SOC
// MB2 ST
// MB3,MB4 는 나머지 
#define CASE_2


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: CAN Receive interrupt
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void _IntCAN_Receive( void )
 *--------------------------------------------------------------------------
 * Function			: CAN Receive interrupt function
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
void can_receive( void )
{
    //BYTE	dlc;														// DLC 
    //RETURN_CODE	rRet;														// Parameter Check Result 
    /* CAN Receive End */
    RFSTS0 &= 0xfff7;														// Clear Interrupt Flag 
    f_nocan  = OFF;
    /* Get Data */
    CAN_GetMsg();										// Get message for CAN
    /* Parameter Check */
    Can_ParseMsg();
    /* Parameter Error ? */
}


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: CAN Global error interrupt
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void _IntCAN_GError( void )
 *--------------------------------------------------------------------------
 * Function			: CAN Global error interrupt function
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
void can_gerror( void )
{
	/* Global Error */
	
	BYTE gStatus = GERFLL;
	
	if ( (gStatus & 0x01) == 0x01 ) {
		/* DLC Error */
		
		GERFLL &= ~0x01;													// Clear Error Flag
	}
	
	if ( (gStatus & 0x02) == 0x02 ) {
		/* FIFO message lost error */
		
		RFSTS0 &= 0xfffb;													// Clear Error Flag (RFMLT bit)
	}
	
	if ( (gStatus & 0x04) == 0x04 ) {
		/* Send history buffer over flow */
		
		THLSTS0 &= 0xfffb;													// Clear Error Flag (THLELT bit)
	}
	
	canerrFlag = TRUE;
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: CAN send complete interrupt
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void _IntCAN_Send( void )
 *--------------------------------------------------------------------------
 * Function			: CAN Global error interrupt function
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
void can_send( void )
{
	/* CAN Send End */
	/*
	TMSTS0 &= 0xf9;															// Clear Interrupt Flag
	TMSTS1 &= 0xf9;															// Clear Interrupt Flag
	TMSTS2 &= 0xf9;															// Clear Interrupt Flag
	TMSTS3 &= 0xf9;															// Clear Interrupt Flag
	*/
	canTxrqFlag = FALSE;
}


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: CAN Channel error interrupt
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void _IntCAN_CError( void )
 *--------------------------------------------------------------------------
 * Function			: CAN Channel error interrupt function
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/

void can_cerror( void )
{
/* Channel Error */
	
	_UWORD cStatus = C0ERFLL;
	
	if ( (cStatus & 0x0001) == 0x0001 ) {
		/* protocol error */
		C0ERFLL &= ~0x0001;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x0002) == 0x0002 ) {
		/* error warning */
		C0ERFLL &= ~0x0002;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x0004) == 0x0004 ) {
		/* error passive */
		C0ERFLL &= ~0x0004;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x0008) == 0x0008 ) {
		/* start bus off */
		C0ERFLL &= ~0x0008;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x0010) == 0x0010 ) {
		/* come back from bus off */
		C0ERFLL &= ~0x0010;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x0020) == 0x0020 ) {
		/* over load */
		C0ERFLL &= ~0x0020;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x0040) == 0x0040 ) {
		/* dominant lock */
		C0ERFLL &= ~0x0040;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x0080) == 0x0080 ) {
		/* arbitration lost */
		C0ERFLL &= ~0x0080;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x0100) == 0x0100 ) {
		/* Stuff error */
		C0ERFLL &= ~0x0100;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x0200) == 0x0200 ) {
		/* form error */
		C0ERFLL &= ~0x0200;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x0400) == 0x0400 ) {
		/* ACK error */
		C0ERFLL &= ~0x0400;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x0800) == 0x0800 ) {
		/* CRC error */
		C0ERFLL &= ~0x0800;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x1000) == 0x1000 ) {
		/* recessive bit error */
		C0ERFLL &= ~0x1000;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x2000) == 0x2000 ) {
		/* dominant bit error */
		C0ERFLL &= ~0x2000;													// Clear Error Flag
	}
	
	if ( (cStatus & 0x4000) == 0x4000 ) {
		/* ACK delimiter error */
		C0ERFLL &= ~0x4000;													// Clear Error Flag
	}
	
	canerrFlag = TRUE;
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: CAN Initialize
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void CAN_BspInit(void)
 *--------------------------------------------------------------------------
 * Function			: Check Initialize function
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
void CAN_BspInit( void )
{
	BYTE wait;										// Counter
	
	/* Page4. Note1. ==== CAN Module is Enabled ==== */
	CAN0EN = 0x1;									// Enable CAN module
        /* Page4. Note 1 ==== CAN RAM initialization ==== */
	while( (GSTS & 0x0008) == 0x0008 );				// Wait CAN RAM Init
        /* Page4. Note2. ==== global mode switch (stop->reset) ==== */
	GCTRL  &= 0xfffb;								// transition Reset mode
        /* Page4. Note3. ==== channel mode switch ==== */
	C0CTRL &= 0xfffb;
	
	
	/* Port Setting */
	POM0_bit.no3 = 0;										// CMOS output
	
	PM0_bit.no3 = 0;										// CTXD0:Output
	PM0_bit.no4 = 1;										// CRXD0:Input
	
	P0_bit.no3 = 1;										// CTXD0 Hi
	//P1_bit.no3 = 0;										// CAN transceiver(Low active)
	
        /* ==== global function setting ==== */
	GCFGH = 0x0001;									// Clock division value setting. Set 1 because don't use. 
	
	GCFGL = 0x0000;									// Clock source is fCLK. Don't check DLC, ID Priority
	
	C0CFGH = 0x14;									// 8Tq(SS:1,TSEG1:5,TSEG2:2,SJW:1 = 75.00%)
      // TSEG1     0x0003 //4TQ
      //           0x0004 // 5TQ
      //           0x0005 : 6TQ 
      
//	C0CFGH = 0x23;									// 8Tq(SS:1,TSEG1:3,TSEG2:4,SJW:1 = 50.00%)

      // TSEG2   0x0000 // 
      //          0x0010 // 2TQ
      //          0x0020 // 3TQ
      //          0x0030 // 4TQ
      //        
       // SJW 0x0000 // 1TQ
       //       0x0100 // 2TQ
       //       0x0200 // 3TQ
       //       0x0300 // 4TQ
	
	C0CFGL = 0x00;									// Division ratio = 1 : Tq number = 8Tq */
													// Bit rate = fCAN/(Division ratio x Tq number of 1bit time)
													//          = 4MHz/(1 x 8)
													//          = 500Kbps
	/* Receive rules setting */
	GAFLCFG  = 0x0002;							// Receive rules.(=used buffer number)
	
	GRWCR &= 0xfffe;								// Change window to 0

/******************************************************************************************
 *   Entry 1 0x110~0x116 (VCU)
 ******************************************************************************************/
	//Standard ID 사용시 , GAFLID 의 bit10~0 만 설정 하고 15-11 은 모두 0으로 
	// 
    GAFLIDL0 = 0x3C2;//MASK_DEVID & 0x07ff;	     // CAN ID for Receive  
	GAFLIDH0 = 0x0000;								// Standerd ID, for Data Frame, Receive message from other node
	GAFLML0 = 0x7FF;//0xFe00;	   //하위 5bit 체크 안함. 0x000~0x1FF	
	GAFLMH0  = 0xc000;								// Compare IDE bit & RTR bit

	// rxid =   (rx ^ GAFLML0 )
	GAFLPL0  = 0x0001;								// Used Receive FIFO Buffer 0. Normal Receive buffer doesn't use.
	GAFLPH0  = 0x0000; //0x2001;	
/******************************************************************************************
 *   Entry 2 0x501~0x503 (CHARGER)
 ******************************************************************************************/
	//Standard ID 사용시 , GAFLID 의 bit10~0 만 설정 하고 15-11 은 모두 0으로 
	// 
       GAFLIDL1 = 0x6F0;//MASK_DEVID & 0x07ff;	     // CAN ID for Receive  
	GAFLIDH1 = 0x0000;								// Standerd ID, for Data Frame, Receive message from other node
	GAFLML1 = 0x7FF;//0xFe00;	   //하위 5bit 체크 안함. 0x000~0x1FF	
	GAFLMH1  = 0xc000;								// Compare IDE bit & RTR bit

	// rxid =   (rx ^ GAFLML0 )
	GAFLPL1  = 0x0001;								// Used Receive FIFO Buffer 0. Normal Receive buffer doesn't use.
	GAFLPH1  = 0x0000; //0x2001;	


	GRWCR |= 0x0001;								// Change window to 1
	
	/* Buffer setting */
	RMNB = 0x0000;									// Receive buffer doesn't use.
	
	RFCC0 = 0x1100;									// Interrupt happen each 1 message receive. Until four Message.
	
	/* buffer setting */
	TMIDL0 = SEND_ID & 0x07ff;							// CAN ID for Send
	TMIDH0 = 0x0000;								// Standerd ID, Data Frame, Send history buffer doesn't use. 
	TMPTR0 = 0x0000;								// DLC Value sets 0 Byte. rebel is 0.
	
	CAN0TRMPR1 = 0;									// CAN0 Interrupt priority is Level 1
	CAN0TRMPR0 = 1;
	
	/* Interrupt setting */
	RFCC0 |= 0x0002;								// Enable Receive FIFO Interrupt 
	TMIEC |= 0x0001;								// Enable Send Buffer 0 Interrupt
	
	GCTRL  |= 0x0300;								// Enable FIFO Message lost interrupt & DLC error interrupt
	C0CTRL |= 0xff00;								// Enable All Error Interrupt
	
	/* CAN0 Error(CAN channel error) */
	CAN0ERRIF = 0;									// Clear CAN channel error interrupt flag
	CAN0ERRMK = 0;									// Enable CAN channel error interrupt

	/* CAN0 Send Finish(CAN Send) */
        /* ==== CAN0 Transmit Interrupt Setting */
	CAN0TRMIF = 0;									// Clear CAN channel send interrupt flag
	CAN0TRMMK = 0;									// Enable CAN channel send interrupt
	
	/* CAN Receive FIFO */
	CANGRFRIF = 0;									// Clear CAN Global receive FIFO interrupt flag
	CANGRFRMK = 0;									// Enable CAN Global receive FIFO interrupt
	
	/* CAN Global Error */
    /* ==== CAN Global FIFO Receive Interrupt Setting */
	CANGERRIF = 0;									// Clear CAN Global error interrupt flag
	CANGERRMK = 0;		/*CAN Global FIFO Interrupt Mask Enable*/// Enable CAN Global error interrupt
	
	/* Transition to Global mode */
	GCTRL &= 0xfffc;
	
	/* Wait transition time( > 10 clock) */
	for ( wait = 0; wait < 10; wait++ ) {
		NOP();
	}
	
	/* Enable Receive FIFO Buffer */
	RFCC0 |= 0x0001;
	
	/* Transition to channel communication mode */
	C0CTRL &= 0xfffc;
	
	return;
}

struct CanR {
    unsigned short *TMIDLx;
    unsigned short *TMPTRx ;
    BYTE *TMDFxL;
    BYTE *TMSTSx;
    BYTE *TMCx;
} canR[]={
        { (unsigned short *)&TMIDL0, (unsigned short *)&TMPTR0, (BYTE *)&TMDF00L, (BYTE *)&TMSTS0, (BYTE *)&TMC0 },      // 0
        { (unsigned short *)&TMIDL1, (unsigned short *)&TMPTR1, (BYTE *)&TMDF01L, (BYTE *)&TMSTS1, (BYTE *)&TMC1 },      // 1
        { (unsigned short *)&TMIDL2, (unsigned short *)&TMPTR2, (BYTE *)&TMDF02L, (BYTE *)&TMSTS2, (BYTE *)&TMC2 },      // 0
        { (unsigned short *)&TMIDL3, (unsigned short *)&TMPTR3, (BYTE *)&TMDF03L, (BYTE *)&TMSTS3, (BYTE *)&TMC3 }      // 1            
};

CAN_RESULT CAN_Send(void)
{
#if 0
    struct CanR *can;
    BYTE try=0;
    CAN_RESULT cRet = CAN_RESULT_OK;
    BYTE mailbox = CAN_TX_NOMAILBOX;
    BYTE	cnt;
    BYTE idx;
    BYTE data[8];
    WORD TxCanID ;
    while ( (GRWCR & 0x0001) != 0x0001 ) {				// Window 0 ?
       // cRet = CAN_RESULT_ERROR;
        GRWCR |= 0x0001;								// Change window to 1
    } ;;
    {
retry:	    
       // while((TMSTS0 & 0x06) != 0){
       // *((BYTE *)&TMSTS0) &= 0xf9;						// Clear TMTRF bit
	//  }
#if defined(CASE_2)
        if(TxMsg.id == CANTX_OEM_BMS0_ESTSOC_ID || TxMsg.id == CANTX_OEM_BMS1_ESTSOC_ID){
            if( (TMSTS0 & 0x06) == 0){
                DI();
                for ( cnt = 0; cnt < 8; cnt++ ) {
                    data[cnt] = TxMsg.data[cnt];
                }
                TxCanID = TxMsg.id;
                EI();
                mailbox = CAN_TX_MAILBOX1;
            }else{
		 *((BYTE *)&TMSTS0) &= 0xf9;
	    }
        }else if( TxMsg.id == CANTX_OEM_BMS0_ST_ID || TxMsg.id == CANTX_OEM_BMS1_ST_ID){
            if( (TMSTS1 & 0x06) == 0){
                mailbox = CAN_TX_MAILBOX2;
                DI();
                for ( cnt = 0; cnt < 8; cnt++ ) {
                    data[cnt] = TxMsg.data[cnt];
                }
                TxCanID = TxMsg.id;
                EI();
            }else{
        	*((BYTE *)&TMSTS1) &= 0xf9;
	    }
        }else{
            if( (TMSTS2 & 0x06) == 0){
                mailbox = CAN_TX_MAILBOX3;
                DI();
                for ( cnt = 0; cnt < 8; cnt++ ) {
                    data[cnt] = TxMsg.data[cnt];
                }
                TxCanID = TxMsg.id;
                EI();
            }else{
        	*((BYTE *)&TMSTS2) &= 0xf9;		    
		if( (TMSTS3 & 0x06) == 0){
                mailbox = CAN_TX_MAILBOX4;
                DI();
                for ( cnt = 0; cnt < 8; cnt++ ) {
                    data[cnt] = TxMsg.data[cnt];
                }
                TxCanID = TxMsg.id;
                EI();
		}else{
                *((BYTE *)&TMSTS3) &= 0xf9;		    
		}
            }
        }
#else
        if( (TMSTS0 & 0x06) == 0 )
        {
            mailbox = CAN_TX_MAILBOX1;
        }
        else{
            *((BYTE *)&TMSTS0) &= 0xf9;
            if( (TMSTS1 & 0x06) == 0)
            {
                mailbox = CAN_TX_MAILBOX2;
            }else
            {
                *((BYTE *)&TMSTS1) &= 0xf9;    
                if( (TMSTS2 & 0x06) == 0)
                {
                    mailbox = CAN_TX_MAILBOX3;
                }else
                {
                    *((BYTE *)&TMSTS2) &= 0xf9;
                    if( (TMSTS3 & 0x06) == 0x0)
                    {
                        mailbox = CAN_TX_MAILBOX4;
                    }else
                    {
                    *((BYTE *)&TMSTS3) &= 0xf9;    
                        mailbox = CAN_TX_NOMAILBOX;    
                    }
                }
            }
        }
#endif

        if( mailbox == CAN_TX_NOMAILBOX)
        {  
            try++;
            if(try > 20){
                return CAN_RESULT_BUSY;
            }
            goto retry;
        } else {
            idx =(BYTE) mailbox-1;
        }
        can = &canR[idx];
        /* Data Id Set*/
        *can->TMIDLx = TxCanID;
        /* Data Length Set */
        *can->TMPTRx &= 0x0fff;					// Clear length
        //*can->TMPTRx |= ( ((_UWORD)TxMsg.dlc << 12) & 0xf000 );
        *can->TMPTRx |= ( ((_UWORD)8 << 12) & 0xf000 );
        /* Data Set */
        for ( cnt = 0; cnt < 8; cnt++ ) 
        {
            *(can->TMDFxL+cnt) = data[cnt];			// Set Data
        }
            // Transmit Request
            *(can->TMCx) |= 0x01;						// Send request
            //*((BYTE *)&TMSTS0+idx) &= 0xf9;						// Clear TMTRF bit

    }
    return cRet;
}

CAN_RESULT CAN_SendThr(BYTE fifo)
{
    struct CanR *can;
    BYTE try=0;
    CAN_RESULT cRet = CAN_RESULT_OK;
    BYTE mailbox = CAN_TX_NOMAILBOX;
    BYTE	cnt;
    BYTE idx;
    BYTE data[8];
    WORD TxCanID ;
    while ( (GRWCR & 0x0001) != 0x0001 ) {				// Window 0 ?
       // cRet = CAN_RESULT_ERROR;
        GRWCR |= 0x0001;								// Change window to 1
    } ;
    DI();
    for ( cnt = 0; cnt < 8; cnt++ ) {
        data[cnt] = TxMsg.data[cnt];
    }
    TxCanID = TxMsg.id;
    EI();
    
    {
retry:	    
       // while((TMSTS0 & 0x06) != 0){
       // *((BYTE *)&TMSTS0) &= 0xf9;						// Clear TMTRF bit
	//  }
        switch(fifo)
        {
            case 1:
                if( (TMSTS0 & 0x06) == 0){
                    mailbox = CAN_TX_MAILBOX1;
                }else{
                    *((BYTE *)&TMSTS0) &= 0xf9;
                }
                break;
            case 2:
                if( (TMSTS1 & 0x06) == 0){
                    mailbox = CAN_TX_MAILBOX2;
                }else{
                    *((BYTE *)&TMSTS1) &= 0xf9;
                }
                break;
            case 3:
                if( (TMSTS2 & 0x06) == 0){
                    mailbox = CAN_TX_MAILBOX3;
                }else{
                    *((BYTE *)&TMSTS2) &= 0xf9;
                }
                break;
            case 4:
                if( (TMSTS3 & 0x06) == 0){
                    mailbox = CAN_TX_MAILBOX4;
                }else{
                    *((BYTE *)&TMSTS3) &= 0xf9;
                }
                break;
        }
   
        if( mailbox == CAN_TX_NOMAILBOX)
        {  
            try++;
            if(try > 20){
                return CAN_RESULT_BUSY;
            }
            goto retry;
        } else {
            idx =(BYTE) mailbox-1;
        }

        can = &canR[idx];
        /* Data Id Set*/
        *can->TMIDLx = TxCanID;
        /* Data Length Set */
        *can->TMPTRx &= 0x0fff;					// Clear length
        //*can->TMPTRx |= ( ((_UWORD)TxMsg.dlc << 12) & 0xf000 );
        *can->TMPTRx |= ( ((_UWORD)8 << 12) & 0xf000 );
        /* Data Set */
        for ( cnt = 0; cnt < 8; cnt++ ) 
        {
            *(can->TMDFxL+cnt) = data[cnt];			// Set Data
        }
        // Transmit Request
        *(can->TMCx) |= 0x01;						// Send request
        //*((BYTE *)&TMSTS0+idx) &= 0xf9;						// Clear TMTRF bit

    }
    return cRet;
#endif    
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: CAN received data function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: CAN_RESULT CAN_GetMsg( BYTE* rxData )
 *--------------------------------------------------------------------------
 * Function			: Get received data for CAN
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: BYTE *rxData:CAN receive data
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
CAN_LENGTH CAN_GetMsg( void)
{
	int	cRet = 0;
	BYTE		cnt;
	BYTE*		adr8;

	if ( (RFSTS0 & 0x0001) == 0x0000 ) {			// UnReceived data is exist ?
            if(RFIDL0 != 0){
            
                CanRxMsg.id = RFIDL0 & 0x7FF;
                /* Get Length */
                cRet = (CAN_LENGTH)( ( (RFPTR0 & 0xf000) >> 12 ) & 0x000f );

                /* Get Data */
                adr8 = (BYTE*)&RFDF00;

                for ( cnt = 0; cnt < cRet; cnt++ ) {
                    CanRxMsg.data[cnt] = adr8[cnt];
                }

                RFPCTR0 = 0x00ff;							// Read pointer move
           }
				
	} else {										// UnReceived data is not exist
		
	}
	return (CAN_LENGTH)cRet;
}
#if 0
void Can_SendRespAckDataBlock(void)
{
    TxMsg.id = CANTX_BMS0_RSP_ID;
    TxMsg.dlc = 8;
    CAN_Send();
}

void Can_SendResponse(BYTE ack, BYTE rspd1, BYTE rspd2)
{
    TxMsg.id = CANTX_BMS0_RSP_ID;
    TxMsg.dlc = 8;
    MsgSetClr(TxMsg.data);
    if(ack==ACK ){
        TxMsg.data[4]=rspd1;
        TxMsg.data[5]=rspd2;
        MsgSetAck(TxMsg.data);
    }else{
        MsgSetNak(TxMsg.data);
        TxMsg.data[4]=rspd1;
        TxMsg.data[5]=rspd2;
    }
    CAN_Send();

}
#endif


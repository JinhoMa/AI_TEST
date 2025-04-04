/*******************************************************************************
* Copyright(C) 2008, 2017 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*******************************************************************************
*******************************************************************************
* DISCLAIMER
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
* http://www.renesas.com/disclaimer
*""FILE COMMENT""***************************************************************
* System Name	: RAJ240090/100 sample program
* File Name		: main.c
* Contents		: main function source file
* Device		: RAJ240090/100
* Compiler		: CC-RL (V.4.01)
* Note			: 
*""FILE COMMENT END""**********************************************************/

/***********************************************************************
** Include files
***********************************************************************/
#include "define.h"																			// union/define definition
#include "afe.h"																			// AFE register(Window 0)
#include "afe1.h"																			// AFE register(Window 1)
#include "afe2.h"																			// AFE register(Window 2)
#include "kousei.h"																			// Initial calibration
#include "mcu.h"																			// mcu.c
#include "ram.h"																			// ram.c
#include "smbus.h"																			// smbus.c
#include "smbus_ex.h"																		// smbus_ex.c
#include "dataflash.h"																		// dataflash.c
#include "iodefine.h"
#include "datcalc_spl.h"																	// datcalc_spl.c
#include "datcalc.h"																		// datcalc.c
#include "mode.h"																			// mode.c
#include "cell.h"																			// cell.c
#include "dataflash.h"
#include <string.h> 
#include <stdio.h>
#include "spi.h"
#include "bms.h"
#include "BMSHal.h"
#include "main.h"

//HUNATE HEADER FILE
AD_Data guage;
extern void Uart_BspInit(void);	
int kcount=1;
/***********************************************************************
** Announcement of internal function prototype
***********************************************************************/
void main(void);
void STcal(void);


void Calc_SOH(void);

unsigned char FslBootSwap(void );

#define WUD_EN 1

void SlowMain(void);
#ifdef Clock_SW
void OCO2LOCO(void);
void LOCO2OCO(void);
#endif

void monit_output(void);


void UpdateDF_Chk(void);		// check whether need to update dataflash



#if 1
BYTE Global_f_curr_int;
#endif

BYTE adc_msrcount;
BYTE pwrdn_timeoutcount;

void OtherEvent_Disable(void);

typedef unsigned char                       fsl_u08;
#define   FSL_OK                    0x00      /* no problems                                    */

void Calc_Init(void);

typedef struct
{                                                       /* ---------------------------------------------- */
        fsl_u08         fsl_flash_voltage_u08;          /* 1, setting of flash voltage mode               */
        fsl_u08         fsl_frequency_u08;              /* 1, CPU frequency (MHz)                         */
        fsl_u08         fsl_auto_status_check_u08;      /* 1, setting of status check mode                */
} fsl_descriptor_t; 

fsl_u08 samFslStart( void );
extern fsl_u08 __far  FSL_InvertBootFlag(void);
extern fsl_u08 __far  FSL_Init(const __far fsl_descriptor_t* descriptor_pstr);
extern void __far  FSL_Open(void);
extern void __far  FSL_PrepareFunctions(void);
extern void __far  FSL_PrepareExtFunctions(void);


// Protection Voltage
#define	D_FULLCHGCV		 u_fo_ram_ext.d_FULLCHGCV	// [2byte] Fullcharge Judgement Cell voltage

#define OEM_WKUPOFF_TIMEOUT     1000 /* 1sec */
BYTE ff_wkupOfftimeout;

BYTE ff_cantimeoutstart;

#pragma address build_info = 0x2000

BYTE     d_MF_DATE[6];                        //  0xF1CF8    [6byte]     Manufacture Date

#if 0
const _BUILD_INFO build_info={
     _DATE_,
    0x0,
    _TIME_,
    0x0,
    {'v','0'+SW_MAJOR,'0'+SW_MINOR1,'0'+SW_MINOR2},
};  
#endif

#if 0
void CellBal_chk(void);
typedef union {									// DWORD or WORD[2] or BYTE[4]
	long	l;
	unsigned short	us[2];
	unsigned char	uc[sizeof(unsigned long)];
} _SLONG2CHAR;

_SLONG2CHAR tcurr;													// Current integrated value(4byte)
#define f_cb_start             DEF_BIT5(&((_SHORT2CHAR *)(&tpacksts))->uc[1])		     // Cell balancing ?? ?? 

BYTE	acmode;

BYTE InitFixed_Chk(void);

typedef unsigned long	DWORD;

void Mode_Change(BYTE mode);

static BYTE c1s ;
unsigned long use_mAh ;
DWORD c1s_sum_tabsc ;
WORD				FCC;										// 0x10:FullChargeCapacity() mAh
#define CYCLE_COUNT       u_fo_ram_ext.d_CycleCnt

#define WD_RESET()   WDTE = 0xAC

#define CMODE	(*(__far BYTE*)0x0000C2)>>6		// CMODE (Option byte)

void InitCVT(void);

extern unsigned char hunate;

void samFslEnd( void );


void fw_upgrade(void);

#if 0
extern __far fsl_descriptor_t fsl_descriptor_pstr ;
#endif

void Uart_BspInit(void);
void Uart_BspSend(char mode, unsigned char *buf, unsigned char length);

int countA=0;
int countB=0;
int countC=0; // Intention for 12?? ??? flag

DWORD			com_BVol;										// Battery Voltage ( 10mV)



#define Timer0_BspStart()   TS0L_bit.no0 = 1
#define Timer0_BspStop()   TT0L_bit.no0 = 1
#define Timer1_BspStart()   TS0L_bit.no1 = 1
#define Timer1_BspStop()   TT0L_bit.no1 = 1
#define Timer2_BspStart()   TS0L_bit.no2 = 1
#define Timer2_BspStop()   TT0L_bit.no2 = 1
#define Timer2_BspStopWait()   while( TE0L_bit.no2 == ON )     // Wait until timer stop

#define TickTimer_Start     Timer1_BspStart
#define SlvToutStart()        TS0L_bit.no2 = 1
#define SlvToutStop()         TT0L_bit.no2 = 1
#define SlvStopWait()         while( TE0L_bit.no2 == ON )
extern Time sysTimer;
#endif
void Timer0_BspInit(void);
void Timer1_BspInit(void);
void Timer2_BspInit(void);



void UPDATE_MODE(void);
#define f_fault                     DEF_BIT6(&((_SHORT2CHAR *)(&tpacksts))->uc[1])		     // 0 - NORMAL, 1- FAIL ( FET OFF ?? ??)
#define f_failure               DEF_BIT7(&((_SHORT2CHAR *)(&tpacksts))->uc[1])		     // 0 - NORMAL, 1- Permanent FAIL ( FET OFF ?? ??)
BYTE	comFlag;				// CAN Flag
//#define DEF_BIT3(pdata)	((volatile _STC_BIT*)(pdata))->bit3

BYTE can_log_out;

int len=0;
int vcu_data;
int chg_data;
extern unsigned char rxfront;
extern unsigned char txfront;
extern unsigned int txleft_count;
extern unsigned char txrear;
int bsp_test_count=0;
int flag_for_uart=0;


#pragma	inline_asm	FUD_Mode
static void FUD_Mode(void)
{
	BR !!0x01CC00;
}

#pragma	inline_asm jumpToBootCode


	
/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: SERIAL initialize Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Serial_init(void)
 *--------------------------------------------------------------------------
 * Function			: Serial communication initialize.
 *					: (Unit1 channel1 : CSI21)
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
 *""FUNC COMMENT END""*****************************************************/
void Serial_init(void)
{
	CS = 0;
	CS_DIR = 0;																			// PM7_2(CS) = OUTPUT
	
	PER2_bit.no7 = 1;																			// C2C enabled
	C2CCNT = 0x81;																		// C2C mode (4bit parallel communication)
																						// C2C communication enabled
	
}



#ifdef	WUD_EN
/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: AFE Wakeup discharge current setting Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void AFE_DWUC(void)
 *--------------------------------------------------------------------------
* Function			: AFE wakeup current setting(Discharge).
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
 *""FUNC COMMENT END""*****************************************************/
void AFE_DWUC_EN(void)
{
	AFE_WR(AFE_WUDRSTRT,DWUDRSTRT);														// DWU interrupt request flag clear
	
	AFE_WR(AFE_WUDTIME,0x0F);															// Wakeup current detection time = 3.91[ms]* 16 = 62.56[ms]
	AFE_WR(AFE_DWUDCON1,0x04);															// Wakeup current detection voltage:1.25[mV] * 4 = 5.0[mV]
	AFE_WR(AFE_DWUDCON2,DWUFETEN | DWUGAIN_10 | DWUACALEN | DWUPERIODIC |  DWUEN);		// FET enable, Gain:10 times, DWU auto calib:enabled, DWU detectiton enable
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: AFE Wakeup charge current setting Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void AFE_CWUC(void)
 *--------------------------------------------------------------------------
* Function			: AFE wakeup current setting(Charge).
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
 *""FUNC COMMENT END""*****************************************************/
void AFE_CWUC_EN(void)
{
	AFE_WR(AFE_WUDRSTRT,CWUDRSTRT);														// CWU interrupt request flag clear
	
	AFE_WR(AFE_WUDTIME,0x0F);															// Wakeup current detection time = 3.91[ms]* 16 = 62.56[ms]
	AFE_WR(AFE_CWUDCON1,0x04);															// Wakeup current detection voltage:1.25[mV] * 4 = 5.0[mV]
	AFE_WR(AFE_CWUDCON2,CWUFETEN | CWUGAIN_10 | CWUACALEN | CWUPERIODIC | CWUEN);		// FET enable, Gain:10 times, DWU auto calib:enabled, DWU detectiton enable
}


#endif
extern BYTE  Uart_Poll (void);

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Initial processing 
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void hdwinit(void)
 *--------------------------------------------------------------------------
 * Function			: Hardware initialize 
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
 * Caution			: This function is called from startup routine.
 *					: 
 *--------------------------------------------------------------------------
 *""FUNC COMMENT END""*****************************************************/
void hdwinit(void)
{
	volatile BYTE acnt;
	
	DI();																				// Interrupt disable
    
    
  	PM0 = 0b00000010;																	// P10-P11 output Low
		 //	|||||||+--- P00 : 0 OUTPUT  ( Watchdog SUB1O)
		 //	||||||+---- P01 : 1 INPUT   ( Watchdog SUB1I)
		 //	|||||+----- P02 : 0 OUTPUT    (No function)
		 //	||||+------ P03 : 0 OUTPUT  (No function)
		 //	|||+------- P04 : 0 OUTPUT  (No function)
		 //	||+-------- P05 : 0 OUTPUT  (MAIN MCU RESET)
		 //	|+--------- P06 : 0 OUTPUT  (Active Balancing)
		 //	+---------- P07 : 0 OUTPUT   (LED )


	PM1 = 0b10011000;																	// P10-P11 output Low
		 //	|||||||+--- P10 : 0 OUTPUT ( MCU_POWER_5V )
		 //	||||||+---- P11 : 0 OUTPUT ( RAJ_POWER_3.3V)
		 //	|||||+----- P12 : 0 OUTPUT    (Ext Power 12V)
		 //	||||+------ P13 : 1 INPUT    (EX_5V)
		 //	|||+------- P14 : 1 INPUT    (RXD1)
		 //	||+-------- P15 : 0 OUTPUT    (TXD1)
		 //	|+--------- P16 : 0 OUTPUT  (No function)
		 //	+---------- P17 : 1 INPUT

	PM2 = 0b11110000;
		 //	|||||||+--- P20 : 0 OUTPUT  (No function)
		 //	||||||+---- P21 : 1 INPUT  (AN2)
		 //	|||||+----- P22 : 0 OUTPUT (No function)
		 //	||||+------ P23 : 0 OUTPUT (No function)
		 //	|||+------- P24 : 1 INPUT(No function)
		 //	||+-------- P25 : 1 INPUT(No function)
		 //	|+--------- P26 : 1 INPUT(No function)
		 //	+---------- P27 : 1 INPUT
	PM6 = 0b11110011;
	     //	|||||||+--- P60 : 1 INPUT
		 //	||||||+---- P61 : 1 INPUT
		 //	|||||+----- P62 : 0 OUTPUT(No function)
		 //	||||+------ P63 : 0 OUTPUT(No function)
		 //	|||+------- P64 : 1 INPUT(No function)
		 //	||+-------- P65 : 1 INPUT(No function)
		 //	|+--------- P66 : 1 INPUT(No function)
		 //	+---------- P67 : 1 INPUT

	CMC = 0b00000000;																	// CMC register
	
//		TIMER INITIALIZE
	TAU0EN = 1;																			// Timer array unit0 supplies input clock
	
	//Gpio_BspInit();
    
//	IIC INITIALIZE
	//SMBus_init();
//  SERIAL(for AFE) INITIALIZE
    IO_LEDOn();
    MAINMCU_RESET_OFF;
    IO_MAINMCU_POWEROn();
    IO_RAJ_2ND_POWEROn();
    IO_ExtPowerOn();
    Timer0_BspInit();
    Timer1_BspInit();
    Timer2_BspInit();
	Serial_init();
    Uart_BspInit();	
	//SPI_init();
	ADC_Bsp_init();
    // HVP 초기화
    AFE_HVP_INIT();
    IO_CONNECT_FUSE();
}



/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: main processing 
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void main(void)
 *--------------------------------------------------------------------------
 * Function			: Execute main processing 
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
 *""FUNC COMMENT END""*****************************************************/

 unsigned char message[8] = "Hello";
 unsigned char length_1 = sizeof(message)-1;
 
 int k;
 uint32_t tid;
 unsigned int cellcount;

void monit_output(void)
{
       Printf((unsigned char* )"\n");
       DPrintf(tick_timer,10,8); Printf((unsigned char* )",");
       // CELL 1~4
       DPrintf(t_com09,  10,0); Printf((unsigned char* )","); 
       
       DPrintf(tvolt[0],  10,0);  Printf((unsigned char* )","); 
       DPrintf(tvolt[1],  10,0);Printf((unsigned char* )",");
       DPrintf(tvolt[2],  10,0);Printf((unsigned char* )","); 
       DPrintf(tvolt[3],  10,0);Printf((unsigned char* )","); 
#if 0       
        DPrintf(tminv,  10,0);  Printf((unsigned char* )","); 
        DPrintf(tmaxv,  10,0);  Printf((unsigned char* )","); 
#endif       
       // avtemp
       DPrintf((_SWORD)t_com08,  10,3);Printf((unsigned char* )",");        
       // min temp
       DPrintf(minTemp,  10,3);Printf((unsigned char* )",");        
       // max temp
       DPrintf(maxTemp,  10,3);Printf((unsigned char* )",");        
       // Current
       DPrintf(t_com0a,  10,5 ); Printf((unsigned char* )","); 
       // SOC
       DPrintf(t_com0d,  10,3);Printf((unsigned char* )","); 
       // SOC
       DPrintf(uSOC,  10,3);Printf((unsigned char* )","); 
       // MODE
       Printf((unsigned char* )"0x");DPrintf(amode,  16,4 ); Printf((unsigned char* )","); 
       // PACK STATE
       Printf((unsigned char* )"0x"); DPrintf(tpacksts&0xFF,  16,4 ); Printf((unsigned char* )","); 
        // Battery Status
       Printf((unsigned char* )"0x"); DPrintf(tcom16.us,  16,4 ); Printf((unsigned char* )","); 
       // Battery Protection Event
       Printf((unsigned char* )"0x"); DPrintf(Bevt.us[0],  16,4 ); Printf((unsigned char* )","); 
#if 1
        //Cellbalancing
       Printf((unsigned char* )"0x"); DPrintf(acb_w,  16,2 ); Printf((unsigned char* )","); 
       // Cycle Count
       DPrintf(t_com17,  10,0 ); Printf((unsigned char* )","); 
       // remain CAP 
       DPrintf(t_com10c,  10,0 ); Printf((unsigned char* )","); 
      // Full Charge
       DPrintf(t_com0fc,  10,0 ); Printf((unsigned char* )","); 
      // remain CAP 
      DPrintf(t_com10c_h,  10,0 ); Printf((unsigned char* )","); 
      // Average Current
       DPrintf(t_com0fc_h,  10,0 ); Printf((unsigned char* )",");
       // Run to Empty 
      // DPrintf(t_com11c,  10,0 ); Printf((unsigned char* )","); 
      //  AvgTime to Empty
      // DPrintf(t_com12c,  10,0 ); Printf((unsigned char* )","); 
      //  AvgTime to Empty
       DPrintf(lrc_w,  10,0 ); Printf((unsigned char* )","); 
      //  AvgTime to Empty
       DPrintf(lrc_w_hunate,  10,0 ); Printf((unsigned char* )","); 
      /*
       if(baup_lrc_w == 0){baup_lrc_w = lrc_w_hunate; }
       else{
            if( OCV_ST == ON){
                 baup_lrc_w = lrc_w_hunate;   
            }else{
                baup_lrc_w += (long)t_com0a*50;
            }
       }
       DPrintf(baup_lrc_w, 10,0 ); Printf((unsigned char* )","); 
       */
      //  AvgTime to Empty
      // itoa(lrccr_w, pBuf, 10,0 ); Printf((unsigned char* )","); 
       
#endif
}


extern _ST_FLASH_OWN DVCal;
void PowerDown_Chk(void);

 void main(void)
{
	DI();
    BMIC_BspInit();	
    IO_FETAllOff();
    hSysInit();
#if 1
    BMIC_BspStart();
    // IO_FETAllOn(); //! MCU가 RAJ로 제어신호를 보내는 구조이므로, RAJ 부팅시에는 FET OFF 상태태
    app_main();
#else
   k = hSerOpen("COM1", 38400, NULL);
   tid = hTimCreate();
   hTimStart(tid);

    while(1){      
    hSerWrite(k,(uint8_t *) "ABCDEF", sizeof("ABCDEF"));
    hTimDelay(1000);


    }
#endif
}
 

void UpdateDF_Chk(void)
{
    if( f_dfe == ON )						// DataFlash erasing ?
    {
        DF_Erase_Confirm();				// Confirm DataFlash erase
    } else {								// not erasing				// #20120124_1
        if( f_dfreq == ON )						// Is there DF update request ?
        {
            if( DF_Write() )					// succeed to update DF ?
            {
                f_dfreq = OFF;					// Clear DF update request
                tdfup_cnt = 0;					// Clear DF update counter
                while( f_dfe == ON )			// Erase wait if DF erasing
                {
                    DF_Erase_Confirm();			// Confirm DataFlash erase
                }
            }
        }			
    }
}
 WORD GetADVin(void)
 {
 	WORD			tcellad;														// Cell ad value
	BYTE			Cell_low,Cell_high;

    AFE_WR(AFE_ADMSEL0,av_tbl[0] | ADATOFF);								// Select offset and cell voltage
    AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME8);							// Select AD conver time and wait time
    AFE_WR(AFE_ADCON0,ADC_EN);												// AD enabled
    AFE_WR(AFE_ADCON1,ADC_BTMODE1);	
    while( PIF9 == 0 ) ;													// AD complete ?
    PIF9 = 0;																// Clear Interrupt request flag(MCU)

    AFE_WR(AFE_AFIF0,INT_ADIR_W);											// Clear interrupt request flag(AFE)
    AFE_RD(AFE_ADRL1,Cell_low);
    AFE_RD(AFE_ADRH1,Cell_high);
	tcellad = ((WORD)Cell_high << 8) + Cell_low;
    
    return tcellad;
 }
 
int16_t afecon_readBatVol(void)
{
    return guage.ad_batvol;
}

int16_t afecon_readCurrent(void)
{
    return guage.ad_cur;
}

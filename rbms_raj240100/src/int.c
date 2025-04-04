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
* File Name		: int.c
* Contents		: interrupt function source file
* Device		: RAJ240090/100
* Compiler		: CC-RL (V.4.01)
* Note			: 
*""FILE COMMENT END""**********************************************************/
/***********************************************************************
** Include files
***********************************************************************/
#include "iodefine.h"
#include "define.h"										// union/define definition
#include "afe.h"										// AFE definition
#include "ram.h"										// ram.c
#include "smbus.h"										// smbus.c
#include "smbus_ex.h"									// smbus_ex.c
#include "mode.h"										// mode.c

#pragma interrupt _int_AFE_CC(vect=INTP8)				// AFE Current integration interrupt
#pragma	interrupt _int_AFE_HVP(vect=INTP10)				// AFE HVP interrupt
#pragma interrupt _int_AFE_WU(vect=INTP11)				// AFE Wakeup detect interrupt
#pragma interrupt _int_AFE_ANL(vect=INTP12)				// AFE ANL interrupt
#pragma interrupt _int_AFE_TM(vect=INTP13)				// AFE interval timer interrupt
#pragma interrupt _int_AD_ANI2(vect=INTAD)				// ANI1 AD interrupt

#if 0		//duplicate error with main.c
BOOL		buarttout_req;				// UART timeout measurement request
WORD		auarttout;					// Timeout counter
BYTE cantout_req;   // timer?? can ?? timeout ?? req
static WORD tick_10ms;
//BYTE can_log_out;
BYTE     pcTool;
#define f_eeprom       DEF_BIT1(&pcTool)     // EEPROM Access
BYTE can_wrtopctool;   // pc tool?? read ??? ?? write ?? ?? ON
#endif

unsigned char tenmilsec=0;
unsigned char ilmilsec=0;
unsigned char afecctest=0;


void _int_AD_ANI2(void)
{	
	f_adc_int=1;
	
}

// Current integration interrupt
void _int_AFE_CC(void)
{
	
	#if 1		//2023-06-15
	afecctest=(afecctest+1)%255;  //Intend to check _int_AFE_CC is operating
	#endif	
	f_curr_int = ON;									// Current integration interruption generation flag ON
	
}

// HVP interrupt
void _int_AFE_HVP(void)
{
	BYTE	aintflg_in;
	BYTE	asys_con;			
	
	AFE_RD(AFE_AFIF2,aintflg_in);				

	if( aintflg_in & INT_HVP0IR)						// AFE HVP0 interrupt
	{
		AFE_WR(AFE_AFIF2,INT_HVP0IR_W); 				// AFE HVP0 interrupt request flag 

		
		if(EGP1_bit.no2 == 0 && EGN1_bit.no2 == 1)		// Falling edge ?
		{
		
			AFE_RD(AFE_PHV & 0x01,asys_con);			// No chattering 

			if(asys_con == LOW)						
			{
				f_sys_in = ON;							// SYS_CONNECT = ON
				f_slow = OFF;							// Clear slow flag
				anosmb_cnt = 0;							// Clear no SMBus counter
				f_nosmb = OFF;							// Clear no SMBus flag
				EGN1_bit.no2 = 0;
				EGP1_bit.no2 = 1;						// Change
			}
		} else {										// Rising edge ?
			
			AFE_RD(AFE_PHV & 0x01,asys_con);			// No chattering 

			if(asys_con == HI)						
			{
				f_sys_in = OFF;							// SYS_CONNECT = OFF
				
				EGN1_bit.no2 = 1;
				EGP1_bit.no2 = 0;						// Change
			}
		}
	}
}


// Interval timer interrupt
void _int_AFE_TM(void)
{
	BYTE	aintflg_in;
	
	AFE_RD(AFE_AFIF3,aintflg_in);
	
	if( aintflg_in & INT_AFETRAIR)						// AFE timer A interrupt
	{
		AFE_WR(AFE_AFIF3,INT_AFETRAIR_W);				// AFE timer A interrupt request flag
		if( f_slow == ON ||  f_fcslp == ON)
		{
			asleep_cnt1++;								// Increment slow mode 1sec counter
			if( asleep_cnt1 == 10 )						// 10sec has passed ?
			{
				asleep_cnt1 = 0;						// Clear the counter
				asleep_cnt2++;							// Increment 10sec counter
				if (asleep_cnt2 == 12)					// 2min has passed ?
				{
					asleep_cnt2 = 0;					// Clear the counter
					f_slp2min = ON; 					// Set sleep 2min passed flag
				} else {								// Not passed 2min
					f_slpcalc = ON; 					// Set calc request on sleep flag
				}
			}
		}
		
		acccv_cnt++;									// Increment the counter
		if( acccv_cnt == 30 )							// 30sec has passed ?
		{
			acccv_cnt = 0;								// Clear the counter
		}		
		
		// - SMBus no communication time check -
		if( f_nosmb == OFF )								// Not detected no SMBus com ?
		{
			anosmb_cnt++;									// Increment the counter
			if( anosmb_cnt == 60 )							// 60sec has passed ?
			{
				f_nosmb = ON;								// Set No SMBus com flag
				anosmb_cnt = 0;								// Clear the counter
			}
		}
		
	}
	
	if( aintflg_in & INT_AFETRBIR)						// AFE timer B interrupt
	{
		AFE_WR(AFE_AFIF3,INT_AFETRBIR_W);				// AFE timer B interrupt request flag
		
		// TO DO: Add your control code here	
	}
	
}

void _int_AFE_WU(void)
{
	BYTE	aintflg_in;
	BYTE	arstflg = 0;
	
	AFE_RD(AFE_AFIF0,aintflg_in);
	
	if( aintflg_in & INT_DWUIR )						// Wakeup detect ?
	{
		AFE_WR(AFE_AFIF0,INT_DWUIR_W);					// Discharge Wakeup interrupt request flag clear
		arstflg |= DWUDRSTRT;
		// TO DO: Add your control code here
	}
	
	if( aintflg_in & INT_CWUIR )						// Wakeup detect ?
	{
		AFE_WR(AFE_AFIF0,INT_CWUIR_W);					// Charge Wakeup interrupt request flag clear
		arstflg |= CWUDRSTRT;
		// TO DO: Add your control code here
	}
	
	if( aintflg_in & INT_CHGIR )						// PON detect ?
	{
		f_fcslp = OFF;
		f_slow = OFF;
		anosmb_cnt = 0;									// Clear no SMBus counter
		f_nosmb = OFF;									// Clear no SMBus flag
		AFE_WR(AFE_AFIF0,INT_CHGIR_W);					// PON interrupt request flag clear
		// TO DO: Add your control code here
	}
	
	// set WU restart
	AFE_WR(AFE_WUDRSTRT,arstflg);
	
}

// The setting for over-current Å¶called from interruption
void OverCurrentSetting(void)
{
	AFE_WR(AFE_FCON,afet);
	// TO DO: Add your control code here
}

// CD/Wakeup/WDT detect interrupt
void AFE_CD(void)
{
	BYTE	aintflg_in;
	BYTE	arstflg = 0;
	
	f_slow = OFF;
	
	AFE_RD(AFE_AFIF1,aintflg_in);						// Read interrupt request flag
	
	if( aintflg_in & INT_SCDIR )						// SC detect ?
	{
		AFE_WR(AFE_AFIF1,INT_SCDIR_W);					// Short Current  interrupt request flag clear
		f_short_alarm = ON;								// Clear SC Alarm
		amode = M_SHC;									// ShortCurrent mode
		afet = FET_C;									// FET [C=ON ,D=OFF]
		OverCurrentSetting();
		arstflg |= SCRSTRT;
	}
	if( aintflg_in & INT_DOC1DIR )						// DOC detect ?
	{
		AFE_WR(AFE_AFIF1,INT_DOC1DIR_W);				// Discharge Over Current interrupt request flag clear
		amode = M_ODC;									// OverDischargeCurrent mode
		f_odc_alarm = ON;								// Set ODC Alarm
		afet = FET_C;									// FET [C=ON ,D=OFF]
		OverCurrentSetting();
		arstflg |= DOC1RSTRT;
	}
	if( aintflg_in & INT_COCDIR )						// COC detect ?
	{
		AFE_WR(AFE_AFIF1,INT_COCDIR_W); 				// Charge Over Current interrupt request flag clear
		amode = M_OCC;									// OverChargeCurrent
		f_occ_alarm = ON;								// Set OCC Alarm
		afet = FET_D;									// FET [C=OFF,D=ON]
		OverCurrentSetting();
		arstflg |= COCRSTRT;
	}

	// WDT detect
	if( aintflg_in & INT_WDTIR )						// WDT detect ?
	{		
		AFE_WR(AFE_AFIF1,INT_WDTIR_W);					// WDT interrupt request flag clear
		// TO DO: Add your control code here
	}

	// Set CD restart
	AFE_WR(AFE_OCDRSTRT,arstflg);
	
}

void _int_AFE_ANL(void)
{
	AFE_CD();
}


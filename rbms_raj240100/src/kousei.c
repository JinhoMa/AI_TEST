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
* File Name		: kousei.c
* Contents		: Initial calibration functions
* Device		: RAJ240090/100
* Compiler		: CC-RL (V.4.01)
* Note			: 
*""FILE COMMENT END""**********************************************************/
#define _KOUSEI


/***********************************************************************
** Include files
***********************************************************************/
#include "iodefine.h"
#include "define.h"																	// union/define definition
#include "dataflash.h"																// DataFlash data definition
#include "afe.h"																	// AFE definition
#include "kousei.h"																	// Initial calibration
#include "ram.h"																	// ram.c
#include "smbus.h"																	// smbus.c
#include "smbus_ex.h"																// smbus_ex.c
#include "mcu.h"																	// mcu.c
#include "cell.h"

#include <string.h>

/***********************************************************************
** Announcement of internal function prototype
***********************************************************************/
void GetCurrCalib(void);															// Acquision process for current calibration value 
WORD GetVAD(BYTE aid);																// Acquision process for voltage AD value
void FlashWrite_Word(BYTE *apadr, BYTE *apdata);		// DataFlash Word Data Write Process(2bytes)
static void FlashWrite_DWord(BYTE *apadr, BYTE *apdata);	// DataFlash Word Data Write Process(4bytes)
void Kousei(void);
void GetAD_Calib(void);																//	Acquision process for AD internal reference value
void CalibSkip(void);																// Calibration skip process

#define AD_UNDERFLOW	(0x8000)

#if UNDER_V0_50
_ST_FLASH_OWN DVCal ={
        //  ===== Initial Calibration Data =====
        8290,8288,8291,8289,0xffff,                         // [10byte] AD converted value for calibration voltage LV
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,   // [22byte] reserved
        16584,16578,16571,16568,0xffff,                         // [10byte] AD converted value for calibration voltage HV
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,                         // [22byte] reserved
        2000, 2000, 2000, 2000, 0xffff,                         // [10byte] Calibration voltage LV [1mV]
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,                         // [22byte] reserved
        4000, 4000, 4000, 4000, 0xffff,                         // [10byte] Calibration voltage value HV [1mV]
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,                         // [22byte] reserved
        6950,                           // [2byte] AD converted value for VIN12 calibration voltage LV
        13900,                          // [2byte] AD converted value for VIN12 calibration voltage HV
        8000,                           // [2byte] VIN12 calibration voltage value LV [1mV]
        16000,                          // [2byte] VIN12 calibration voltage value HV [1mV]
    
        131146,                             // [4byte] Current counter value for initial calibration 0A
        154400,                             // [4byte] Discharge current counter value for initial calibration
        4000,                           // [2byte] Reference current value for initial calibration
        2967,                           // [2byte] Initial calibration value for temperature
        2980,                           // [2byte] Initial calibration reference value for temperature
        
    };
#else
_ST_FLASH_OWN DVCal ={
	//  ===== Initial Calibration Data =====
	9890,9887,9887,9889,9890,							// [10byte] AD converted value for calibration voltage LV
	9887,9878,8098,8097,8100,0xff,0xff,0xff,0xff,0xff,0xff,   // [22byte] reserved
	19781,19774,19775,19776,19775,							// [10byte] AD converted value for calibration voltage HV
	19770,19765,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 						// [22byte] reserved
	2000, 2000, 2000, 2000, 2000,							// [10byte] Calibration voltage LV [1mV]
	2000,2000,2000,2000,2000,0xff,0xff,0xff,0xff,0xff,0xff, 						// [22byte] reserved
	4000, 4000, 4000, 4000, 4000,							// [10byte] Calibration voltage value HV [1mV]
	4000,4000,4000,4000,4000,0xff,0xff,0xff,0xff,0xff,0xff, 						// [22byte] reserved
	6950,							// [2byte] AD converted value for VIN12 calibration voltage LV
	13900,							// [2byte] AD converted value for VIN12 calibration voltage HV
	14000,							// [2byte] VIN12 calibration voltage value LV [1mV]
	28000,							// [2byte] VIN12 calibration voltage value HV [1mV]

	131115,								// [4byte] Current counter value for initial calibration 0A
	153868,								// [4byte] Discharge current counter value for initial calibration
	4000,							// [2byte] Reference current value for initial calibration
	2967,							// [2byte] Initial calibration value for temperature
	2980,							// [2byte] Initial calibration reference value for temperature
	
};

#endif


const __near WORD tcalib_def[sizeof(u_fo_ram)] = 

		{cellL_ad,cellL_ad,cellL_ad,cellL_ad,cellL_ad,cellL_ad,cellL_ad,cellL_ad,cellL_ad,cellL_ad,
																					// Cell voltage(Low side) AD value[20bytes]
		rsv_calib,rsv_calib,rsv_calib,rsv_calib,rsv_calib,rsv_calib,				// reserved[6bytes]
		cellH_ad,cellH_ad,cellH_ad,cellH_ad,cellH_ad,cellH_ad,cellH_ad,cellH_ad,cellH_ad,cellH_ad,
																					// Cell voltage(High side) AD value[20bytes]
		rsv_calib,rsv_calib,rsv_calib,rsv_calib,rsv_calib,rsv_calib,				// reserved[6bytes]
		cellL_ref,cellL_ref,cellL_ref,cellL_ref,cellL_ref,cellL_ref,cellL_ref,cellL_ref,cellL_ref,cellL_ref,
																					// Cell voltage(Low side) reference value[20bytes]
		rsv_calib,rsv_calib,rsv_calib,rsv_calib,rsv_calib,rsv_calib,				// reserved[6bytes]
		cellH_ref,cellH_ref,cellH_ref,cellH_ref,cellH_ref,cellH_ref,cellH_ref,cellH_ref,cellH_ref,cellH_ref,
																					// Cell voltage(high side) reference value[20bytes]
		rsv_calib,rsv_calib,rsv_calib,rsv_calib,rsv_calib,rsv_calib,				// reserved[6bytes]
		packL_ad,																	// PACK voltage(Low side) AD value[2bytes]
		packH_ad,																	// PACK voltage(High side) AD value[2bytes]
		packL_ref,																	// PACK voltage(Low side) reference value[2bytes]
		packH_ref,																	// PACK voltage(High side) reference value[2bytes]
		curr_ad0_low,curr_ad0_high,													// Current counter value for initial calibration 0A[4byte]
		curr_ad_low,curr_ad_high,													// Current counter value for initial calibration [4byte]
		curr_ref,																	// Reference current value for initial calibration[2byte] 
		temp_ad,																	// temperature AD value[2bytes]
		temp_ref,																	// temperature ref value[2bytes]
		tcvL_ad,																	// Total cell voltage(Low side) AD value[2bytes]
		tcvH_ad,																	// Total cell voltage(High side) AD value[2bytes]
		tcvL_ref,																	// Total cell voltage(Low side) reference value[2bytes]
		tcvH_ref,																	// Total cell voltage(High side) reference value[2bytes]
		mcubg_ad,																	// MCU BG AD value[2bytes]
		creg2_ad																	// CREG2 AD value[2bytes]
		};


		
#pragma	inline_asm	FUD_Mode
static void FUD_Mode(void)
{
	BR !!0x01CC00;
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: FlashROM Word Data Write Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void FlashWrite_Word(BYTE *apadr, BYTE *apdata)
 *--------------------------------------------------------------------------
 * Function			: Word data is written into the designated sector in FlashROM.
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: BYTE *apadr : Pointer of write address
 *					: BYTE *apdata: Data line to be written
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
void FlashWrite_Word(BYTE *apadr, BYTE *apdata)
{
	FlashWrite( apadr, apdata, 2 );													// Write data of 2bytes at DataFlash area
	DFLEN = 1;																		// Data flash access enable
	Waittimer_us(10);																// wait 10us
	memcpy( &u_fo_ram, &u_fo_rom, sizeof(u_fo_rom) );								// DataFlash own data -> RAM copy
	DFLEN = 0;																		// Data flash access disable
}



/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: FlashROM Word Data Write Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void FlashWrite_Word(BYTE *apadr, BYTE *apdata)
 *--------------------------------------------------------------------------
 * Function			: DWord data is written into the designated sector in FlashROM.
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: BYTE *apadr : Pointer of write address
 *					: BYTE *apdata: Data line to be written
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
void FlashWrite_DWord(BYTE *apadr, BYTE *apdata)
{
	FlashWrite( apadr, apdata, 4 );													// Write data of 4bytes at DataFlash area
	DFLEN = 1;																		// Data flash access enable
	Waittimer_us(10);																// wait 10us
	memcpy( &u_fo_ram, &u_fo_rom, sizeof(u_fo_rom) );								// DataFlash own data -> RAM copy
	DFLEN = 0;																		// Data flash access disable
}


#if 1


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Calibration Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Kousei(void)
 *--------------------------------------------------------------------------
 * Function			: Calibrated after calibration request is received and this function is called 
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
void Kousei(void)
{
	WORD	tad;
	BYTE	al1;
	BYTE	acell_L,acell_H;

	switch( acalib_id )																// Calibration item ?
	{
	case CALIB_LV1:																	// LVx
	case CALIB_LV2:
	case CALIB_LV3:
	case CALIB_LV4:
	case CALIB_LV5:
	case CALIB_LV6:
	case CALIB_LV7:
	case CALIB_LV8:
	case CALIB_LV9:
	case CALIB_LV10:
		if( D_LV[acalib_id] == 0xFFFF )												// The DataFlash area is vacant ?
		{
			tad = GetVAD(acalib_id);												// The acquisition of voltage AD value 
																					// Write to DataFlash
			FlashWrite_Word((BYTE*)&D_R_LV[acalib_id], (BYTE*)&tad);
			if( D_LV[acalib_id] == tad )											// Verify OK ?
			{
				acalib_sts = CALIB_OK;												// Calibration completes
			} else {																// Verify NG
				acalib_sts = CALIB_NG;												// Calibration error
			}
		} else {																	// Not vacant
			acalib_sts = CALIB_NG;													// Calibration error
		}
		break;

	case CALIB_HV1:																	// HVx
		if( F_CALIB1P == ON )
		{
			if( D_0A == 0xFFFFFFFF )			// The DataFlash area is vacant ?
			{
				GetCurrCalib();					// The acquisition of current calibration value
												// Write to Flash
				FlashWrite_DWord((BYTE*)&D_R_0A, (BYTE*)&(cc_curr.us[0]));
				if( D_0A != lcurr_ad )			// Verify OK ?
				{
					acalib_sts = CALIB_NG;		// Calibration error
					break;
				}
			} else {							// Not vacant
				acalib_sts = CALIB_NG;			// Calibration error
				break;
			}
		}
	case CALIB_HV2:
	case CALIB_HV3:
	case CALIB_HV4:
	case CALIB_HV5:
	case CALIB_HV6:
	case CALIB_HV7:
	case CALIB_HV8:
	case CALIB_HV9:
	case CALIB_HV10:
		if( D_HV[acalib_id-0x20] == 0xFFFF )										// The DataFlash area is vacant ?
		{
			tad = GetVAD(acalib_id-0x20);											// The acquisition of voltage AD value
																					// Write to Flash
			FlashWrite_Word((BYTE*)&D_R_HV[acalib_id-0x20], (BYTE*)&tad);
			if( D_HV[acalib_id-0x20] == tad )										// Verify OK ?
			{
				acalib_sts = CALIB_OK;												// Calibration completes
			} else {																// Verify NG
				acalib_sts = CALIB_NG;												// Calibration error
			}
		} else {																	// Not vacant
			acalib_sts = CALIB_NG;													// Calibration error
		}
		break;

	case CALIB_VIN12_LV:															// Pack voltage
		if( D_V12LV == 0xFFFF )														// The DataFlash area is vacant	 ?
		{
			AFE_WR(AFE_ADMSEL3,ADATPACK | ADATOFF2);								// Select offset and Pack voltage
			AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME8);							// Select AD conver time and wait time
			AFE_WR(AFE_ADCON0,ADC_EN);												// AD enabled
			AFE_WR(AFE_ADCON1,ADC_HVPMODE);											// Start AD conversion
	
			while( PIF9 == 0 ) ;													// AD complete ?
			PIF9 = 0;																// Clear Interrupt request flag(MCU)
		
			AFE_WR(AFE_AFIF0,INT_ADIR_W);											// Clear interrupt request flag(AFE)
		
			AFE_RD(AFE_ADRL1,acell_L);												// get AD value
			AFE_RD(AFE_ADRH1,acell_H);												// get AD value
			tad = ((WORD)acell_H << 8) + acell_L;
			
			if( tad >= AD_UNDERFLOW)
			{
				acalib_sts = CALIB_NG;												// Calibration error
			} else {
																					// Write to Flash
				FlashWrite_Word((BYTE*)&D_R_V12LV, (BYTE*)&tad);
				if( D_V12LV == tad )												// Verify OK ?
				{
					acalib_sts = CALIB_OK;											// Calibration completes
				} else {															// Verify NG
					acalib_sts = CALIB_NG;											// Calibration error
				}
			}
		} else {																	// Not vacant
			acalib_sts = CALIB_NG;													// Calibration error
		}
		break;

	case CALIB_VIN12_HV:
		if( D_V12HV == 0xFFFF )														// The DataFlash area is vacant	 ?
		{
			AFE_WR(AFE_ADMSEL3,ADATPACK | ADATOFF2);								// Select offset and Pack voltage
			AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME8);							// Select AD conver time and wait time
			AFE_WR(AFE_ADCON0,ADC_EN);												// AD enabled
			AFE_WR(AFE_ADCON1,ADC_HVPMODE);											// Start AD conversion
	
			while( PIF9 == 0 ) ;													// AD complete ?
			PIF9 = 0;																// Clear Interrupt request flag(MCU)
		
			AFE_WR(AFE_AFIF0,INT_ADIR_W);											// Clear interrupt request flag(AFE)
		
			AFE_RD(AFE_ADRL1,acell_L);												// get AD value
			AFE_RD(AFE_ADRH1,acell_H);												// get AD value
			tad = ((WORD)acell_H << 8) + acell_L;
			
			if( tad >= AD_UNDERFLOW)
			{
				acalib_sts = CALIB_NG;												// Calibration error
			}else {
						
																					// Write to Flash
				FlashWrite_Word((BYTE*)&D_R_V12HV, (BYTE*)&tad);
				if( D_V12HV == tad )												// Verify OK ?
				{
					acalib_sts = CALIB_OK;											// Calibration completes
				} else {															// Verify NG
					acalib_sts = CALIB_NG;											// Calibration error
				}
			}
		} else {																	// Not vacant
			acalib_sts = CALIB_NG;													// Calibration error
		}
		break;

	case CALIB_LVALL:																// LVall
																						
		for( al1=0; al1<D_SERIES; al1++ )											// Check loop of vacant DataFlash
		{
			if( D_LV[al1] != 0xFFFF )												// The FlashROM area is vacant	?
			{
				acalib_sts = CALIB_NG;												// Calibration error
				return;
			}
		}

		for( al1=0; al1<D_SERIES; al1++ )											// Calibration loop
		{
			tad = GetVAD(al1);														// The acquisition of AD value
																					// Write to Flash
			FlashWrite_Word((BYTE*)&D_R_LV[al1], (BYTE*)&tad);
			if( D_LV[al1] != tad )													// Verify NG ?
			{
				acalib_sts = CALIB_NG;												// Calibration error
				return;
			}
		}
		acalib_sts = CALIB_OK;														// Calibration complets
		break;

	case CALIB_HVALL:																// HVall
		
		for( al1=0; al1<D_SERIES; al1++ )											// Check loop of vacant DataFlash
		{
			if( D_HV[al1] != 0xFFFF )												// The DataFlash area is vacant ?
			{
				acalib_sts = CALIB_NG;												// Calibration error
				return;
			}
		}
		
		if( F_CALIB1P == ON )
		{
			if( D_0A == 0xFFFFFFFF )			// The DataFlash area is vacant ?
			{
				GetCurrCalib();					// The acquisition of current calibration value
												// Write to Flash
				FlashWrite_DWord((BYTE*)&D_R_0A, (BYTE*)&(cc_curr.us[0]));
				if( D_0A != lcurr_ad )			// Verify OK ?
				{
					acalib_sts = CALIB_NG;		// Calibration error
					break;
				}
			} else {							// Not vacant
				acalib_sts = CALIB_NG;			// Calibration error
				break;
			}
		}

		for( al1=0; al1<D_SERIES; al1++ )											// Calibration loop
		{
			tad = GetVAD(al1);														// The acquisition of AD value
																					// Write to Flash
			FlashWrite_Word((BYTE*)&D_R_HV[al1], (BYTE*)&tad);
			if( D_HV[al1] != tad )													// Verify NG ?
			{
				acalib_sts = CALIB_NG;												// Calibration error
				return;
			}
		}
		acalib_sts = CALIB_OK;														// Calibration complets
		break;

	case CALIB_0A:																	// Current 0A
		if( D_0A == 0xFFFFFFFF )													// The DataFlash area is vacant ?
		{
			GetCurrCalib();															// The acquisition of current calibration value
																					// Write to Flash
			FlashWrite_DWord((BYTE*)&D_R_0A, (BYTE*)&(cc_curr.us[0]));
			if( D_0A == lcurr_ad )													// Verify OK ?
			{
				acalib_sts = CALIB_OK;												// Calibration complets
			} else {																// Verify NG 
				acalib_sts = CALIB_NG;												// Calibration error
			}
		} else {																	// Not vacant
			acalib_sts = CALIB_NG;													// Calibration error
		}

		break;

	case CALIB_CURR:																// Current calibration value
		if( D_CURR == 0xFFFFFFFF )													// The DataFlash area is vacant ?
		{
			GetCurrCalib();															// The acquisition of current calibration value
																					// Write to Flash
			FlashWrite_DWord((BYTE*)&D_R_CURR, (BYTE*)&(cc_curr.us[0]));
			if( D_CURR == lcurr_ad )												// Verify OK ?
			{
				acalib_sts = CALIB_OK;												// Calibration complets
			} else {																// Verify NG
				acalib_sts = CALIB_NG;												// Calibration error
			}
		} else {																	// Not vacant
			acalib_sts = CALIB_NG;													// Calibration error
		}
		break;

	case CALIB_TEMP:																// Temperature
		if( D_CALTH == 0xFFFF )														// The DataFlash area is vacant ?
		{
			ReadTemp();																// The acquisition of temperature value
																					// Write to Flash
			FlashWrite_Word((BYTE*)&D_R_CALTH, (BYTE*)&t_com08);
			if( D_CALTH == t_com08 )												// Verify OK ?
			{
				acalib_sts = CALIB_OK;												// Calibration complets
			} else {																// Verify NG
				acalib_sts = CALIB_NG;												// Calibration error
			}
		} else {																	// Not vacant
			acalib_sts = CALIB_NG;													// Calibration error
		}
		break;
	
#ifdef	TCV_EN
	
	case CALIB_TCLV:																// Total Cell Voltage(Low)
		if( D_TCLV == 0xFFFF )														// The DataFlash area is vacant	 ?
		{
			AFE_WR(AFE_ADMSEL3,ADATVIN10 | ADATOFF2);								// Select offset and Total Cell Voltage
			AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME8);							// Select AD conver time and wait time
			AFE_WR(AFE_ADCON0,ADC_EN);												// AD enabled
			AFE_WR(AFE_ADCON1,ADC_HVPMODE);											// Start AD conversion
	
			while( PIF9 == 0 ) ;													// AD complete ?
			PIF9 = 0;																// Clear Interrupt request flag(MCU)
			
			AFE_WR(AFE_AFIF0,INT_ADIR_W);											// Clear interrupt request flag(AFE)
			
			AFE_RD(AFE_ADRL3,acell_L);												// get AD value
			AFE_RD(AFE_ADRH3,acell_H);												// get AD value
			tad = ((WORD)acell_H << 8) + acell_L;
			
			if( tad >= AD_UNDERFLOW)
			{
				acalib_sts = CALIB_NG;												// Calibration error
			} else {
																					// Write to Flash
				FlashWrite_Word((BYTE*)&D_R_TCLV, (BYTE*)&tad);
				if( D_TCLV == tad )													// Verify OK ?
				{
					acalib_sts = CALIB_OK;											// Calibration completes
				} else {															// Verify NG
					acalib_sts = CALIB_NG;											// Calibration error
				}
			}
		} else {																	// Not vacant
			acalib_sts = CALIB_NG;													// Calibration error
		}
		break;
	
	case CALIB_TCHV:																// Total Cell Voltage(High)
		if( D_TCHV == 0xFFFF )														// The DataFlash area is vacant	 ?
		{
			AFE_WR(AFE_ADMSEL3,ADATVIN10 | ADATOFF2);								// Select offset and Total Cell Voltage
			AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME8);							// Select AD conver time and wait time
			AFE_WR(AFE_ADCON0,ADC_EN);												// AD enabled
			AFE_WR(AFE_ADCON1,ADC_HVPMODE);											// Start AD conversion
	
			while( PIF9 == 0 ) ;													// AD complete ?
			PIF9 = 0;																// Clear Interrupt request flag(MCU)
			
			AFE_WR(AFE_AFIF0,INT_ADIR_W);											// Clear interrupt request flag(AFE)
			
			AFE_RD(AFE_ADRL3,acell_L);												// get AD value
			AFE_RD(AFE_ADRH3,acell_H);												// get AD value
			tad = ((WORD)acell_H << 8) + acell_L;
					
			if( tad >= AD_UNDERFLOW)
			{
				acalib_sts = CALIB_NG;												// Calibration error
			}else {
							
																					// Write to Flash
				FlashWrite_Word((BYTE*)&D_R_TCHV, (BYTE*)&tad);
				if( D_TCHV == tad )													// Verify OK ?
				{
					acalib_sts = CALIB_OK;											// Calibration completes
				} else {															// Verify NG
					acalib_sts = CALIB_NG;											// Calibration error
				}
			}
		} else {																	// Not vacant
			acalib_sts = CALIB_NG;													// Calibration error
		}
		break;
	
#endif
	
#ifdef	MCUBG_EN
	
	case CALIB_MCUBG:																// MCU BG Voltage
		if( D_MCUBG == 0xFFFF )														// The DataFlash area is vacant	 ?
		{
			// ADC(MCU) setting
			ADCEN = ON;																// AD(MCU) enabled
			ADM2 = ADM2_IRV;														// AD converter reference voltage source select(Internal reference voltage(1.45[V]))
					
			AFE_WR(AFE_ADMSEL5,ADATMCUREF | ADATOFF4);								// Select offset and MCUBG voltage
			AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME8);							// Select AD conver time and wait time
			AFE_WR(AFE_ADCON0,ADC_EN);												// AD enabled
			AFE_WR(AFE_ADCON1,ADC_SFUNCMODE);										// Start AD conversion
	
			while( PIF9 == 0 ) ;													// AD complete ?
			PIF9 = 0;																// Clear Interrupt request flag(MCU)
			
			AFE_WR(AFE_AFIF0,INT_ADIR_W);											// Clear interrupt request flag(AFE)
			
			AFE_RD(AFE_ADRL4,acell_L);												// get AD value
			AFE_RD(AFE_ADRH4,acell_H);												// get AD value
			tad = ((WORD)acell_H << 8) + acell_L;
					
			if( tad >= AD_UNDERFLOW)
			{
				acalib_sts = CALIB_NG;												// Calibration error
			} else {
																					// Write to Flash
				FlashWrite_Word((BYTE*)&D_R_MCUBG, (BYTE*)&tad);
			if( D_MCUBG == tad )													// Verify OK ?
			{
				acalib_sts = CALIB_OK;												// Calibration completes
			} else {																// Verify NG
				acalib_sts = CALIB_NG;												// Calibration error
			}
			}
					
				ADCEN = OFF;														// AD(MCU) disable
					
		} else {																	// Not vacant
					acalib_sts = CALIB_NG;											// Calibration error
			}
			break;

#endif
	
#ifdef	CREG2_EN
	
	case CALIB_CREG2:																// CREG2 Voltage
		if( D_CREG2 == 0xFFFF )														// The DataFlash area is vacant	 ?
		{
			AFE_WR(AFE_ADMSEL5,ADATCREG2 | ADATOFF4);								// Select offset and MCUBG voltage
			AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME8);							// Select AD conver time and wait time
			AFE_WR(AFE_ADCON0,ADC_EN);												// AD enabled
			AFE_WR(AFE_ADCON1,ADC_SFUNCMODE);										// Start AD conversion
	
			while( PIF9 == 0 ) ;													// AD complete ?
			PIF9 = 0;																// Clear Interrupt request flag(MCU)
			
			AFE_WR(AFE_AFIF0,INT_ADIR_W);											// Clear interrupt request flag(AFE)
			
			AFE_RD(AFE_ADRL6,acell_L);												// get AD value
			AFE_RD(AFE_ADRH6,acell_H);												// get AD value
			tad = ((WORD)acell_H << 8) + acell_L;
		
			if( tad >= AD_UNDERFLOW)
			{
				acalib_sts = CALIB_NG;												// Calibration error
			} else {
							
																					// Write to Flash
				FlashWrite_Word((BYTE*)&D_R_CREG2, (BYTE*)&tad);
				if( D_CREG2 == tad )												// Verify OK ?
				{
					acalib_sts = CALIB_OK;											// Calibration completes
				} else {															// Verify NG
					acalib_sts = CALIB_NG;											// Calibration error
				}
			}
		} else {																	// Not vacant
			acalib_sts = CALIB_NG;													// Calibration error
		}
		break;

#endif


	}


}

#endif


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Initial Calibration Data Check Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		:  BYTE InitCalib_Chk(void)
 *--------------------------------------------------------------------------
 * Function			: Check whether there are some initial calibration data or not
 *					: Even if there is just one vacant data, FALSE is returned.
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: TRUE=There are all necessary data.
 *					: FALSE=There are some vacant data.
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
BYTE InitCalib_Chk(void)
{
	BYTE	al1;

	DFLEN = 1;																		// Data flash access enable
	Waittimer_us(10);																// wait 10us
	memcpy( &u_fo_ram, &DVCal, sizeof(DVCal) );								// DataFlash own data -> RAM copy
	DFLEN = 0;																		// Data flash access disable

	#if 1
	D_CURRREF=0x0fa1; //2023-06-07 changed 0x0fa0 to 0x0fa1
	#endif
	
	
	for( al1 = 0; al1<D_SERIES; al1++ )												// LV,HV
	{
		if( F_CALIB1P == OFF )					// Cell Voltage Calib is 2Point ?
		{
			if( D_LV[al1] == 0xFFFF )													// LVx is vacant ?
			{
				return FALSE;
			}
		
			if( D_LVREF[al1] == 0xFFFF )												// LVx reference value is vacant ?
			{
				return FALSE;
			}
		}
		if( D_HV[al1] == 0xFFFF )													// HVx is vacant ?
		{
			return FALSE;
		}
		
		if( D_HVREF[al1] == 0xFFFF )												// HVx reference value is vacant?
		{
			return FALSE;
		}
	}

	if( D_0A == 0xFFFFFFFF )														// 0 Curr AD value is vacant ?
	{
		return FALSE;
	}
	if( D_CURR == 0xFFFFFFFF )														// Curr AD value is vacant ?
	{
		return FALSE;
	}

	if( D_CURRREF == 0xFFFF )														// Curr reference value is vacant ?
	{
		return FALSE;
	}
	
	if( F_CALIB1P == OFF )					// Cell Voltage Calib is 2Point ?
	{
	
		if( D_V12LV == 0xFFFF)															// PACK voltage low value is vacant ?
		{
			return FALSE;
		}
	}
	if( D_V12HV == 0xFFFF)															// PACK voltage high value is vacant ?
	{
		return FALSE;
	}
	if( F_CALIB1P == OFF )					// Cell Voltage Calib is 2Point ?
	{
		if( D_V12LVREF == 0xFFFF)														// PACK voltage low reference value is vacant ?
		{
			return FALSE;
		}
	}
	
	if( D_V12HVREF == 0xFFFF)														// PACK voltage high reference value is vacant ?
	{
		return FALSE;
	}
	
#ifdef	TCV_EN
	if( F_CALIB1P == OFF )					// Cell Voltage Calib is 2Point ?
	{
		if( D_TCLV == 0xFFFF)															// Total Cell voltage low value is vacant ?
		{
			return FALSE;
		}
	}
	
	if( D_TCHV == 0xFFFF)															// Total Cell voltage high value is vacant ?
	{
		return FALSE;
	}
	
	if( F_CALIB1P == OFF )					// Cell Voltage Calib is 2Point ?
	{
		if( D_TCLVREF == 0xFFFF)														// Total Cell voltage low reference value is vacant ?
		{
			return FALSE;
		}
	}
	
	if( D_TCHVREF == 0xFFFF)														// Total Cell voltage high reference value is vacant ?
	{
		return FALSE;
	}
	
#endif
	
#ifdef	MCUBG_EN
	
	if( D_MCUBG == 0xFFFF)															// MCU BG voltage value is vacant ?
	{
		return FALSE;
	}
	
#endif

#ifdef	CREG2_EN
	
	if( D_CREG2 == 0xFFFF)															// CREG2 voltage value is vacant ?
	{
		return FALSE;
	}
	
#endif

	return TRUE;
}



/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Acquisition Process for Current Calibration
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void GetCurrCalib(void)
 *--------------------------------------------------------------------------
 * Function			: Acquisition of current calibration value
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
void GetCurrCalib(void)
{
	// When it receives command for current calib it gets current intergration data in second cycle.
	// Because the command reception and the current integration period are asynchronous
	
	
	f_curr_int = OFF;																// Clear CC detect flag
	
	// First CC interrupt wait
	while( f_curr_int == OFF );														// Wait for current integration completion
	f_curr_int = OFF;																// Clear CC detect flag
	
	
	// Second CC interrupt wait
	while( f_curr_int == OFF );														// Wait for current integration completion
	f_curr_int = OFF;																// Clear CC detect flag
	
	AFE_RD(AFE_CCRL,cc_curr.uc[0]);													// Get result (L)
	AFE_RD(AFE_CCRM,cc_curr.uc[1]);													// Get result (M)
	AFE_RD(AFE_CCRH,cc_curr.uc[2]);													// Get result (H)
}



/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Acquisition Process for voltage AD value
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: WORD GetVAD(BYTE aid)
 *--------------------------------------------------------------------------
 * Function			: Acquisition of voltage AD value
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: BYTE aid : CELL number(0=Cell1, 1=Cell2, ...)
 *--------------------------------------------------------------------------
 * Return			: AD value (AD value-offset value)
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

WORD GetVAD(BYTE aid)
{
	WORD			tcellad;														// Cell ad value
//	long			*tmp;
	BYTE			Cell_low,Cell_high;
	
	if(aid < 10)
	{
	
		if(aid < 5)																	// cell 1-5
		{
			AFE_WR(AFE_ADMSEL0,av_tbl[aid] | ADATOFF);								// Select offset and cell voltage
			AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME8);							// Select AD conver time and wait time
			AFE_WR(AFE_ADCON0,ADC_EN);												// AD enabled
			AFE_WR(AFE_ADCON1,ADC_BTMODE1);											// Start AD conversion
				
		} else {																	// cell 6-10
			AFE_WR(AFE_ADMSEL1,av_tbl[aid-5] | ADATOFF);							// Select offset and cell voltage
			AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME8);							// Select AD conver time and wait time
			AFE_WR(AFE_ADCON0,ADC_EN);												// AD enabled
			AFE_WR(AFE_ADCON1,ADC_BTMODE2);											// Start AD conversion
			aid -= 5;
		}
			while( PIF9 == 0 ) ;													// AD complete ?
			PIF9 = 0;																// Clear Interrupt request flag(MCU)
		
			AFE_WR(AFE_AFIF0,INT_ADIR_W);											// Clear interrupt request flag(AFE)
		
			switch(aid)
			{
				case 0:																// Cell 1,6
					AFE_RD(AFE_ADRL1,Cell_low);
					AFE_RD(AFE_ADRH1,Cell_high);
					break;
						
				case 1:																// Cell 2,7
					AFE_RD(AFE_ADRL2,Cell_low);
					AFE_RD(AFE_ADRH2,Cell_high);
					break;
						
				case 2:																// Cell 3,8
					AFE_RD(AFE_ADRL3,Cell_low);
					AFE_RD(AFE_ADRH3,Cell_high);
					break;
						
				case 3:																// Cell 4,9
					AFE_RD(AFE_ADRL4,Cell_low);
					AFE_RD(AFE_ADRH4,Cell_high);
					break;
						
				case 4:																// Cell 5,10
					AFE_RD(AFE_ADRL5,Cell_low);
					AFE_RD(AFE_ADRH5,Cell_high);
					break;
						
				default:
				break;
			}

		tcellad = ((WORD)Cell_high << 8) + Cell_low;
		
	} else																			// Error
	{
		tcellad = AD_UNDERFLOW;
	}
		return (tcellad);															// return cell AD value 
}
/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Initial Calibration Main
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void KouseiMain(void)
 *--------------------------------------------------------------------------
 * Function			: Initial calibration main process
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
void KouseiMain(void)
{
	do
	{
		if( f_calib == ON )						// Receive calibration command ?
		{
#if DEBUG_USE && CAL_LOG		
                   Printf("[CAL] cmd\n");
#endif
			Kousei();							// Calibration process
			f_calib = OFF;						// Clear calibration flag
		}
		
		BootJump_Chk();							// SMBus boot jump check
		
		if(f_adccalib == ON)
		{
			f_adccalib = OFF;
			GetAD_Calib();
		}
		
		if(f_skipcalib == ON)
		{
			f_skipcalib = OFF;
			CalibSkip();
			return;
		}
		
		Timeout_Chk();							// SMBus timeout check

	} while(1);
}



/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Initialized Process of initial Calibration Data
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Init_CalibDat(void)
 *--------------------------------------------------------------------------
 * Function			: Initial caluculation of initial calibration data
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
void Init_CalibDat(void)
{
	BYTE	al1;

	for(al1=0; al1<D_SERIES; al1++ )
	{
		// - Voltage -
																					// VrefH - VrefL
		trefhv_lv[al1] = D_HVREF[al1] - D_LVREF[al1];
		thv_lv[al1] = D_HV[al1] - D_LV[al1];										// VH - VL
	}

	// - Current -

	lccaldef = D_CURR - D_0A;														// Discharge calibration value - 0A calibration value

	// - Temperature -
																					// Calibration data is vacant ?
	if( D_CALTH!=0xFFFF && D_CALTHREF!=0xFFFF )
	{
		tthoff = D_CALTHREF - D_CALTH;												// Caluculation of offset
	} else {
		tthoff = 0;																	// Offset=0
	}

	// - Pack voltage -
	trefhl_vin12 = D_V12HVREF - D_V12LVREF;											// Pack VrefH - VrefL
	thlvin12 = D_V12HV - D_V12LV;													// Pack VH - VL
	
#ifdef	TCV_EN
	// Total Cell Voltage
	trefhl_tcv = D_TCHVREF - D_TCLVREF;												// Total Cell Voltage VrefH - VrefL
	thltcv = D_TCHV - D_TCLV;														// Total Cell Voltage VH - VL
#endif
	
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Get AD value in initial Calibration
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void GetAD_Calib(void)
 *--------------------------------------------------------------------------
 * Function			: Get AD value in initial calibration
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
void GetAD_Calib(void)
{
	char i;
	BYTE	acell_L,acell_H;
	BYTE	atemp_L,atemp_H;
	
	for(i = 0;i < D_SERIES; i++)
	{
		if (i == 0)		//2016.03.28
		{
			if(f_curr_int == ON)
			{
				AFE_RD(AFE_CCRL,cc_curr.uc[0]);							// Get result (L)
				AFE_RD(AFE_CCRM,cc_curr.uc[1]);							// Get result (M)
				AFE_RD(AFE_CCRH,cc_curr.uc[2]);							// Get result (H)

			}
			//while( PIF9 == 0 ) ;										// AD complete ?
			//PIF9 = 0;													// Clear Interrupt request flag(MCU)

			//AFE_WR(AFE_AFIF0,INT_ADIR_W);								// Clear interrupt request flag(AFE)

		}
	
		ad_cell[i] = GetVAD(i);
	}
	
	// AN0,AN1 AD value
	AFE_WR(AFE_ADMSEL4,ADATOFF3 | ADATAN0 | ADATAN1 | ADATVREGpullAN0pull | ADATVREGpullAN1pull | ADATVREGpull);			
																		// Select offset and AN0, AN1, puli-up enabled
	AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME1);						// Select AD conver time and wait time
	AFE_WR(AFE_ADCON0,ADC_EN);											// AD enabled
	AFE_WR(AFE_ADCON1,ADC_THERMMODE);									// Start AD conversion (Thermistor mode)
	
	while( PIF9 == 0 ) ;												// AD complete ?
	PIF9 = 0;															// Clear Interrupt request flag(MCU)
		
	AFE_WR(AFE_AFIF0,INT_ADIR_W);										// Clear interrupt request flag(AFE)
	
	// AN0
	
	AFE_RD(AFE_ADRL1,atemp_L);											// get AD value
	AFE_RD(AFE_ADRH1,atemp_H);											// get AD value
	
	ad_therm = ((WORD)atemp_H << 8) + atemp_L; 
	
	// AN1
	AFE_RD(AFE_ADRL2,atemp_L);											// get AD value
	AFE_RD(AFE_ADRH2,atemp_H);											// get AD value
	
	ad_therm2 = ((WORD)atemp_H << 8) + atemp_L; 
	
	// Pack voltage & Total Cell voltage
#ifdef	TCV_EN
	AFE_WR(AFE_ADMSEL3,ADATPACK | ADATVIN10 | ADATOFF2);				// Select offset and Pack voltage and Total cell voltage
#else
	AFE_WR(AFE_ADMSEL3,ADATPACK | ADATOFF2);							// Select offset and Pack voltage
#endif
	AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME1);						// Select AD conver time and wait time
	AFE_WR(AFE_ADCON0,ADC_EN);											// AD enabled
	AFE_WR(AFE_ADCON1,ADC_HVPMODE);										// Start AD conversion
	
	while( PIF9 == 0 ) ;												// AD complete ?
	PIF9 = 0;															// Clear Interrupt request flag(MCU)
		
	AFE_WR(AFE_AFIF0,INT_ADIR_W);										// Clear interrupt request flag(AFE)
	
	
	AFE_RD(AFE_ADRL1,acell_L);											// get AD value
	AFE_RD(AFE_ADRH1,acell_H);											// get AD value
	ad_pack = ((WORD)acell_H << 8) + acell_L;															
	
	if( ad_pack >= AD_UNDERFLOW)										// AD value underflow ?
	{
		ad_pack = 0;
	}

#ifdef	TCV_EN
	
	AFE_RD(AFE_ADRL3,acell_L);											// get AD value
	AFE_RD(AFE_ADRH3,acell_H);											// get AD value
	ad_tcv = ((WORD)acell_H << 8) + acell_L;
	
	if( ad_tcv >= AD_UNDERFLOW)											// AD value underflow ?
	{
		ad_tcv = 0;
	}
	
#endif

#ifdef	MCUBG_EN
	
	// ADC(MCU) setting
	ADCEN = ON;															// AD(MCU) enabled
	ADM2 = ADM2_IRV;													// AD converter reference voltage source select(Internal reference voltage(1.45[V]))

	// MCUBG voltage
	AFE_WR(AFE_ADMSEL5,ADATMCUREF | ADATOFF4);							// Select offset and MCUBG voltage
	AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME1);						// Select AD conver time and wait time
	AFE_WR(AFE_ADCON0,ADC_EN);											// AD enabled
	AFE_WR(AFE_ADCON1,ADC_SFUNCMODE);									// Start AD conversion
	
	while( PIF9 == 0 ) ;												// AD complete ?
	PIF9 = 0;															// Clear Interrupt request flag(MCU)
		
	AFE_WR(AFE_AFIF0,INT_ADIR_W);										// Clear interrupt request flag(AFE)

	AFE_RD(AFE_ADRL4,acell_L);											// get AD value
	AFE_RD(AFE_ADRH4,acell_H);											// get AD value
	ad_tmcubg = ((WORD)acell_H << 8) + acell_L;
			
	if( ad_tmcubg >= AD_UNDERFLOW)										// AD value underflow ?
	{
		ad_tmcubg = 0;
	}
	
	ADCEN = OFF;														// AD(MCU) disable
	
#endif
	
#ifdef	CREG2_EN
	// CREG2 voltage
	AFE_WR(AFE_ADMSEL5,ADATCREG2 | ADATOFF4);							// Select offset and CREG2 voltage
	AFE_WR(AFE_ADCON2,ADATWAITTIME1 | ADATTIME1);						// Select AD conver time and wait time
	AFE_WR(AFE_ADCON0,ADC_EN);											// AD enabled
	AFE_WR(AFE_ADCON1,ADC_SFUNCMODE);									// Start AD conversion
	
	while( PIF9 == 0 ) ;												// AD complete ?
	PIF9 = 0;															// Clear Interrupt request flag(MCU)
		
	AFE_WR(AFE_AFIF0,INT_ADIR_W);										// Clear interrupt request flag(AFE)
		
	AFE_RD(AFE_ADRL6,acell_L);											// get AD value
	AFE_RD(AFE_ADRH6,acell_H);											// get AD value
	ad_tcreg2 = ((WORD)acell_H << 8) + acell_L;
			
	if( ad_tcreg2 >= AD_UNDERFLOW)										// AD value underflow ?
	{
		ad_tcreg2 = 0;
	}
	
#endif
}
/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Calibration skip process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void CalibSkip(void)
 *--------------------------------------------------------------------------
 * Function			: Calibration skip in calibration
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
void CalibSkip(void)
{
	BYTE acnt;
	WORD *twork;
	
	twork = (WORD *)&u_fo_ram;
	
	for(acnt = 0; acnt < sizeof(u_fo_ram); acnt++)
	{
		twork[acnt] = tcalib_def[acnt];
	}
}

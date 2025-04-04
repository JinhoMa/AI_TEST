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
* File Name		: mcu_auto.c
* Contents		: mcu function source file
* Device		: RAJ240090/100
* Compiler		: CC-RL (V.4.01)
* Note			: 
*""FILE COMMENT END""**********************************************************/
#define _MCU

/***********************************************************************
** Include files
***********************************************************************/
#include "define.h"														// union/define definition
#include "afe.h"														// AFE window 0
#include "afe1.h"														// AFE window 1
#include "dataflash.h"													// Dataflash.c
#include "kousei.h"														// Initial calibration
#include "mcu.h"														// mcu.c
#include "ram.h"														// ram.c
#include "iodefine.h"
#include "cell.h"
#include "mode.h"
#include "main.h"

#define AD_UNDERFLOW	(0x8000)


#define B_TH25_AN0_REG			*(__far WORD*)0x01C840						// Data of AN0 Internal Pullup resister 25degC value
#define B_TH25_AN1_REG			*(__far WORD*)0x01C842						// Data of AN1 Internal Pullup resister 25degC value

const BYTE		acond_tbl[10]											// Conditioning table
						= {COND_V1^COND_OFF, COND_V2^COND_OFF
							, COND_V3^COND_OFF, COND_V4^COND_OFF, COND_V5^COND_OFF
							, COND_V6^COND_OFF, COND_V7^COND_OFF, COND_V8^COND_OFF
							, COND_V9^COND_OFF, COND_V10^COND_OFF};

int output_index = 0;  // ?? ??? ???

unsigned char Vbat_buffer[20];
unsigned char hunate=10;
int cnt;
WORD adc_buffer;

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Function of calculate VIN12 voltage
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void CalcPackVolt(WORD tad)
 *--------------------------------------------------------------------------
 * Function			: 
 *					: Calculate VIN12 voltage value.
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: WORD tad	: AD value - offset value
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
void CalcPackVolt(WORD tad)
{
	long	lvalue;

	if( D_V12LV != 0xFFFF )												// InitCalib value is valid ?
	{
		if(F_CALIB1P == OFF)
		{
			// VIN12Volt = (AD - VL) * (VrefH - VrefL) / (VH - VL) + VrefL
			// *:Vx = ärê≥ADíl, Vrefx = ärê≥äÓèÄìdà≥íl
			// *:Vx = InitCalib AD value, Vrefx = InitCalib ref. voltage value
			lvalue = ((long)tad - D_V12LV) * trefhl_vin12 / thlvin12 + D_V12LVREF;
		} else {
			lvalue = (long)tad * D_V12HVREF / D_V12HV;
		}
		
		if( lvalue > 0 )												// åãâ ÇÕê≥êî ?
		{																// Positive value ?
			tvin12v = (WORD)lvalue;										// Set the result
		} else {														// ïâêî negative value
			tvin12v = 0;												// Set to 0
		}
	} else {															// InitCalib value is Invalid
		tvin12v = 0;													// Set to 0
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Function of calculate Total Cell voltage
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void CalcTotalCellVolt(WORD tad)
 *--------------------------------------------------------------------------
 * Function			: 
 *					: Calculate Total Cell voltage value.
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: WORD tad	: AD value - offset value
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
static void CalcTotalCellVolt(WORD tad)
{
	long	lvalue;

	if( D_TCLV != 0xFFFF )												// InitCalib value is valid ?
	{
		if(F_CALIB1P == OFF)
		{
			// VIN12Volt = (AD - VL) * (VrefH - VrefL) / (VH - VL) + VrefL
			// *:Vx = ärê≥ADíl, Vrefx = ärê≥äÓèÄìdà≥íl
			// *:Vx = InitCalib AD value, Vrefx = InitCalib ref. voltage value
			lvalue = ((long)tad - D_TCLV) * trefhl_tcv / thltcv + D_TCLVREF;
		} else {
			lvalue = (long)tad * D_TCHVREF / D_TCHV;
		}
		
		if( lvalue > 0 )												// åãâ ÇÕê≥êî ?
		{																// Positive value ?
			ttcv = (WORD)lvalue;										// Set the result
		} else {														// ïâêî negative value
			ttcv = 0;													// Set to 0
		}
	} else {															// InitCalib value is Invalid
		ttcv = 0;														// Set to 0
	}
}



/*""FUNC COMMENT""**********************************************************
 * ID				: 
 * module outline	: Current and voltage and temperature measurement
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void CVT_Func(void)
 *--------------------------------------------------------------------------
 * Function			: Current and voltage and temperature measurement 
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
void CVT_Func(void)
{
	char i;
	unsigned short tvtmp = 0;
	BYTE	acell_L,acell_H;
	BYTE	aimp_L,aimp_H;
	
	BYTE av_tbl_tmp_L = 0;
	BYTE av_tbl_tmp_H = 0;
	BYTE	amxidx;														// MaxV index
	BYTE	amnidx;														// MinV index
	BYTE	al1;
	
	
	
	
	
    if( acb_w != 0 )											        // Is there cell balance control ?
	{
		AFE_WR(AFE_COND0,COND_OFF);									        // Cell Balancing OFF(Cell 1 - Cell 8)   
	}
	
	if( acb_w1 != 0 )											        // Is there cell balance control ?
	{
		AFE_WR(AFE_COND1,COND_OFF);							            // Cell Balancing OFF(Cell 9 - Cell 10)
	}
    
    
	
	
	for(i = 0; i < D_SERIES; i++)
	{
		if(i < 5)
		{
			av_tbl_tmp_L += av_tbl[i];									// Select measurement cell(for ADC_BTMODE1)
		} else {
			av_tbl_tmp_H += av_tbl[i];									// Select measurement cell(for ADC_BTMODE2)
		}
	}
	for(i=0;i<D_SERIES;i++){
		ad_cell[i]=0xFFFF;
	}
	for(i = 0;i < D_SERIES; i++)
	{
		if (i == 0)		//2016.03.28
		{
			AFE_WR(AFE_ADMSEL0,av_tbl_tmp_L | ADATOFF); 
			AFE_WR(AFE_ADCON2,ADATTIME1 | ADATWAITTIME1);				// Select AD conver time and wait time
			AFE_WR(AFE_ADCON0,ADC_EN);									// AD enabled
			AFE_WR(AFE_ADCON1,ADC_BTMODE1 | ADC_ADIMPSEL);				// Start AD conversion and imp
			if(f_curr_int == ON)
			{
				AFE_RD(AFE_CCRL,cc_curr.uc[0]);							// Get result (L)
				AFE_RD(AFE_CCRM,cc_curr.uc[1]);							// Get result (M)
				AFE_RD(AFE_CCRH,cc_curr.uc[2]);							// Get result (H)
                guage.ad_cur = cc_curr.ul;
				//CalcCurr();												// Calculate current

			}
			#if 1
			while( PIF9 == 0 ) ;										// AD complete ?
			PIF9 = 0;													// Clear Interrupt request flag(MCU)
			#endif
			
			AFE_WR(AFE_AFIF0,INT_ADIR_W);								// Clear interrupt request flag(AFE)

		} else {
			if (i ==5)													
			{
				AFE_WR(AFE_ADMSEL1,av_tbl_tmp_H | ADATOFF); 			// Select offset and cell voltage
				AFE_WR(AFE_ADCON2,ADATTIME1 | ADATWAITTIME1);			// Select AD conver time and wait time
				AFE_WR(AFE_ADCON0,ADC_EN);								// AD enabled
				AFE_WR(AFE_ADCON1,ADC_BTMODE2 | ADC_ADIMPSEL);			// Start AD conversion and imp

				while( PIF9 == 0 ) ;									// AD complete ?
				PIF9 = 0;												// Clear Interrupt request flag(MCU)

				AFE_WR(AFE_AFIF0,INT_ADIR_W);							// Clear interrupt request flag(AFE)
			}
		}

		switch(i)
		{
			case 0:														// Cell 1
			case 5:														// Cell 6
				AFE_RD(AFE_ADRL1,acell_L);
				AFE_RD(AFE_ADRH1,acell_H);
				AFE_RD(AFE_IMPRL1,aimp_L);
				AFE_RD(AFE_IMPRH1,aimp_H);
			break;

			case 1:														// Cell 2
			case 6:														// Cell 7
				AFE_RD(AFE_ADRL2,acell_L);
				AFE_RD(AFE_ADRH2,acell_H);
				AFE_RD(AFE_IMPRL2,aimp_L);
				AFE_RD(AFE_IMPRH2,aimp_H);
			break;

			case 2:														// Cell 3
			case 7:														// Cell 8
				AFE_RD(AFE_ADRL3,acell_L);
				AFE_RD(AFE_ADRH3,acell_H);
				AFE_RD(AFE_IMPRL3,aimp_L);
				AFE_RD(AFE_IMPRH3,aimp_H);
			break;

			case 3:														// Cell 4
			case 8:														// Cell 9
				AFE_RD(AFE_ADRL4,acell_L);
				AFE_RD(AFE_ADRH4,acell_H);
				AFE_RD(AFE_IMPRL4,aimp_L);
				AFE_RD(AFE_IMPRH4,aimp_H);
			break;

			case 4:														// Cell 5
			case 9:														// Cell 10
				AFE_RD(AFE_ADRL5,acell_L);
				AFE_RD(AFE_ADRH5,acell_H);
				AFE_RD(AFE_IMPRL5,aimp_L);
				AFE_RD(AFE_IMPRH5,aimp_H);
			break;
		}

		ad_cell[i] = ((WORD)acell_H << 8) + acell_L;					// Cell AD value
		guage.ad_cvol[i] = ad_cell[i];
		ad_imp[i] = ((WORD)aimp_H << 8) + aimp_L;

		if(ad_cell[i] >= AD_UNDERFLOW)
		{
			ad_div[i] = 0;
		} else {
			ad_div[i] = ad_cell[i];
		}

		//CalcVolt(i,ad_div[i]);											// Calculate voltage value
		//tvtmp += tvolt[i];												// Add voltage value
	}

	ReadTemp();

#if 1
	// Pack voltage & Total Cell voltage
#ifdef	TCV_EN
	AFE_WR(AFE_ADMSEL3,ADATPACK | ADATVIN10 | ADATOFF2);				// Select offset and Pack voltage and Total cell voltage
#else
	AFE_WR(AFE_ADMSEL3,ADATPACK | ADATOFF2);							// Select offset and Pack voltage
#endif
	AFE_WR(AFE_ADCON2,ADATTIME1 | ADATWAITTIME1);						// Select AD conver time and wait time
	AFE_WR(AFE_ADCON0,ADC_EN);											// AD enabled
	AFE_WR(AFE_ADCON1,ADC_HVPMODE);										// Start AD conversion
	
	#if 1
	while( PIF9 == 0 ) ;												// AD complete ?
	PIF9 = 0;															// Clear Interrupt request flag(MCU)
	#endif	
	
	AFE_WR(AFE_AFIF0,INT_ADIR_W);										// Clear interrupt request flag(AFE)
	
	
	AFE_RD(AFE_ADRL1,acell_L);											// get AD value
	AFE_RD(AFE_ADRH1,acell_H);											// get AD value
	ad_pack = ((WORD)acell_H << 8) + acell_L;															
	
	if( ad_pack >= AD_UNDERFLOW)										// AD value underflow ?
	{
		ad_pack = 0;
	}
	guage.ad_packvol  = ad_pack;

	//CalcPackVolt(ad_pack);												// Calc Pack Voltage

#ifdef	TCV_EN
	
	AFE_RD(AFE_ADRL3,acell_L);											// get AD value
	AFE_RD(AFE_ADRH3,acell_H);											// get AD value
	ad_tcv = ((WORD)acell_H << 8) + acell_L;
	
	if( ad_tcv >= AD_UNDERFLOW)											// AD value underflow ?
	{
		ad_tcv = 0;
	}
    guage.ad_batvol = ad_tcv;
	
	//CalcTotalCellVolt(ad_tcv);											// Calc Total cell Voltage
#endif

#ifdef	MCUBG_EN
	
	// ADC(MCU) setting
	ADCEN = ON;															// AD(MCU) enabled
	ADM2 = ADM2_IRV;													// AD converter reference voltage source select(Internal reference voltage(1.45[V]))

	// MCUBG voltage
	AFE_WR(AFE_ADMSEL5,ADATMCUREF | ADATOFF4);							// Select offset and MCUBG voltage
	AFE_WR(AFE_ADCON2,ADATTIME1 | ADATWAITTIME1);						// Select AD conver time and wait time
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

#endif

#if 0
#ifdef	CREG2_EN
	// CREG2 voltage
	AFE_WR(AFE_ADMSEL5,ADATCREG2 | ADATOFF4);							// Select offset and CREG2 voltage
	AFE_WR(AFE_ADCON2,ADATTIME1 | ADATWAITTIME1);						// Select AD conver time and wait time
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

#endif



}

/*""FUNC COMMENT""***************************************************
* module outline	: WakeUp calibration processing
*-------------------------------------------------------------------
* Include			: 
*-------------------------------------------------------------------
* Declaration		: void WU_CALIB_D(void)
*-------------------------------------------------------------------
* Function			: WakeUp calibration processing
*					: 
*-------------------------------------------------------------------
* Return			: None
*-------------------------------------------------------------------
* Input				: None
* Output			: None
*-------------------------------------------------------------------
* Used function		: 
*-------------------------------------------------------------------
* Caution			: 
*					: 
*""FUNC COMMENT END""**********************************************/
void WU_CALIB_D(void)
{
	BYTE awork = 0x70;							// DWUDCON1 value. 140mV. 950p
	BYTE awork2 = 0x70;							// bitdata value
	BYTE	ocdsds_in;
	
	DI();										// Interrupt disable
	
	AFE_WR(AFE_DWUDCON2,DWUFETEN | DWUGAIN_10 | DWUACALEN | DWUPERIODIC |  DWUEN);		// FET enable, Gain:10 times, DWU auto calib:enabled, DWU detectiton enable
		
	AFE_WR(AFE_DWUDCON1,awork);	//140mV

	AFE_WR(AFE_DWUDCON2,DWUFETEN | DWUGAIN_10 | DWUACALEN | DWUPERIODIC |  DWUEN);		// FET enable, Gain:10 times, DWU auto calib:enabled, DWU detectiton enable
	
	EI();										// Interrupt enable

	do
	{
		Waittimer_us(250);
		Waittimer_us(250);
		Waittimer_us(250);
		Waittimer_us(250);// Wait 1ms in total
		DI();									// Interrupt disable

		AFE_WR(AFE_DWUDCON2,DWUEN)
		EI();									// Interrupt enable

		AFE_RD(AFE_OCDSTS,ocdsds_in); 		

		if(( ocdsds_in & DWUDFLG ) == OFF )		// Detect WakeUp current ?
		{
			awork -= awork2;					// Adjust WDTCON1 set value
		}
		if( awork2 == 1 )						// bitdata is smallest ?
		{
			break;
		}
		awork2 = awork2 >> 1;					// Set next bit
		awork = awork + awork2;

		DI();									// Disable interrupt

		AFE_WR(AFE_DWUDCON1,awork)
		
		AFE_WR(AFE_DWUDCON2,DWUFETEN | DWUGAIN_10 | DWUACALEN | DWUPERIODIC |  DWUEN);		// FET enable, Gain:10 times, DWU auto calib:enabled, DWU detectiton enable		//Discharge wakeup current detection enabled
									//Enabled auto calibration (once after the start of operation)

		AFE_WR(AFE_WUDRSTRT, DWUDRSTRT);					//Charge current detection circuit restart

		EI();									// Enable interrupt
	}
	while(1);
												// Decide comparator voltage
	if( awork > (0x74 - WU_CURR_05))			// detection current is added ?
	{
		awork = 0x74;							// Set up maximum value for wakeUp comparator voltage
	} else {
		awork += WU_CURR_05;					// Add the detecting voltage 
	}

	DI();										// Interruption prohibition
	AFE_WR(AFE_DWUDCON2,0b00000000);

	AFE_WR(AFE_DWUDCON1,awork);

	AFE_WR(AFE_WUDRSTRT,DWUDRSTRT);

	AFE_WR(AFE_AFIF0,INT_DWUIR_W);			// Discharge wakeup current detection interrupt request
					

	EI();										// Interruption permission
}

/*""FUNC COMMENT""***************************************************
* module outline	: WakeUp calibration processing
*-------------------------------------------------------------------
* Include			: 
*-------------------------------------------------------------------
* Declaration		: void WU_CALIB_C(void)
*-------------------------------------------------------------------
* Function			: WakeUp calibration processing
*					: 
*-------------------------------------------------------------------
* Return			: None
*-------------------------------------------------------------------
* Input				: None
* Output			: None
*-------------------------------------------------------------------
* Used function		: 
*-------------------------------------------------------------------
* Caution			: 
*					: 
*""FUNC COMMENT END""**********************************************/
void WU_CALIB_C(void)
{
	const unsigned char WUDCON_MAX = 0x70;
	unsigned char awork = 0x70;					// CWUDCON1 value. -140mV. 952p
	unsigned char awork2 = 0x6F;				// bitdata value
	BYTE	ocdsds_in;

	DI();										// Interruption prohibition


	AFE_WR(AFE_CWUDCON2,CWUFETEN | CWUGAIN_10 | CWUACALEN | CWUPERIODIC | CWUEN);		// FET enable, Gain:10 times, DWU auto calib:enabled, DWU detectiton enable

	
	AFE_WR(AFE_CWUDCON1,awork)

	AFE_WR(AFE_CWUDCON2,CWUEN);

	EI();										// Interruption permission

	do
	{
		Waittimer_us(250);
		Waittimer_us(250);
		Waittimer_us(250);
		Waittimer_us(250); // Wait 1ms in total
		DI();									// Interrupt disable
		AFE_WR(AFE_CWUDCON2,0b00000000);
		EI();									// Interrupt enable

		AFE_RD(AFE_OCDSTS,ocdsds_in);

		if(( ocdsds_in & CWUDFLG ) == OFF )		// Detect WakeUp current ?
		{
			awork |= awork2;					// Adjust WDTCON1 set value
		}
		if( awork2 == 1 )						// bitdata is smallest ?
		{
			break;
		}
		awork2 = awork2>>1;						// Set next bit
		awork = awork - awork2;
		if( awork >= WUDCON_MAX )				// register setting max over
		{
			awork = WUDCON_MAX;
			break;
		}

		DI();									// Disable interrupt

		AFE_WR(AFE_CWUDCON1,awork);

		AFE_WR(AFE_CWUDCON2,CWUFETEN | CWUGAIN_10 | CWUACALEN | CWUPERIODIC | CWUEN);		// FET enable, Gain:10 times, DWU auto calib:enabled, DWU detectiton enable


		AFE_WR(AFE_OCDRSTRT,COCRSTRT);

		EI();									// Enable interrupt
	}
	while(1);
												// Decide comparator voltage
	if( awork <= WU_CURR_05 )
	{
		awork = 0;
	} else {
		awork -= WU_CURR_05;					// Add the detecting voltage 
	}

	DI();										// Interruption prohibition

	AFE_WR(AFE_CWUDCON2,0b00000000);

	AFE_WR(AFE_CWUDCON1,awork);

	AFE_WR(AFE_OCDRSTRT,COCRSTRT);
	
	AFE_WR(AFE_AFIF0,INT_CWUIR_W);

	EI();										// Interruption permission
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Temperature value calculation processing
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Calc_Temp(void)
 *--------------------------------------------------------------------------
 * Function			: Temperature value calculation by AD value
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
void Calc_Temp(void)
{
	BYTE	aidx;														// refer index table
	WORD	tr1 = B_TH25_AN0_REG;										// internal pull-up resistance
	long	lr2;														// thermistor resistance

		// Temp = (AD25degC - ad) * ((100+273) - (250+273)) * 10 / (AD25degC - AD100degC) + ((25 + 273) * 10)
	// Temp = (AD25degC - ad) * (1000 - 250) / (AD25degC - AD100degC)+ 2980

	if( tadvregpull <= ad_therm )										// AN0 AD value > VREG pull-up AD value
	{
		return;
	}

	
	// Calc resistance of thermistor from AD voltage and internal pull-up resistance
	lr2 = ((long)tr1 * (ad_therm - tadvss)) / (tadvregpull - ad_therm);

	for( aidx=0; aidx<6 && lr2<((long)THERM_TBL[aidx+1]*10); aidx++ );

	// Temp = (ThTbl[0]-r2)*(Thidx[1]-Thidx[0])/(ThTbl[0]-ThTbl[1])+Thidx[0]
	// Temp = (ThTbl[0] - r2) * 200 / (ThTbl[0] - ThTbl[1]) + Thidx (+ offset)
	t_com08 = (WORD)(((long)THERM_TBL[aidx]*10 - lr2) * 200 / ((long)THERM_TBL[aidx]*10 - (long)THERM_TBL[aidx+1]*10) + ttempidx[aidx])+ tthoff;

	// Temperature(base) = t_com08 / 10 - 273
	if( t_com08 <= 1450 )						// -128Åé or less ?
	{
		adegree = -128;
		//adegree = 0;
	} else {
		if( t_com08 >= 4000 )					// 127Åé or more ?
		{
			adegree = 127;
			//adegree = 255;
		} else {								// -127ÅéÅ`126Åé
			adegree = (_SBYTE)((t_com08/10) - 273);
			//adegree = (BYTE)(t_com08/10 - 145);
		}
	}
	
}


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Temperature value calculation processing
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Calc_Temp(void)
 *--------------------------------------------------------------------------
 * Function			: Temperature value calculation by AD value
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
void Calc_2ndTemp(void)
{
																		
	BYTE	aidx;														// refer index table
	WORD	tr1 = B_TH25_AN1_REG;										// internal pull-up resistance
	long	lr2;														// thermistor resistance

	// Temp = (AD25degC - ad) * ((100+273) - (250+273)) * 10 / (AD25degC - AD100degC) + ((25 + 273) * 10)
	// Temp = (AD25degC - ad) * (1000 - 250) / (AD25degC - AD100degC)+ 2980

	if( tadvregpull2 <= ad_therm2 )										// AN1 AD value > VREG pull-up AD value
	{
		return;
	}
	
	// Calc resistance of thermistor from AD voltage and internal pull-up resistance
	lr2 = ((long)tr1 * (ad_therm2 - tadvss)) / (tadvregpull2 - ad_therm2);

	for( aidx=0; aidx<6 && lr2<((long)THERM_TBL[aidx+1]*10); aidx++ );

	// Temp = (ThTbl[0]-r2)*(Thidx[1]-Thidx[0])/(ThTbl[0]-ThTbl[1])+Thidx[0]
	// Temp = (ThTbl[0] - r2) * 200 / (ThTbl[0] - ThTbl[1]) + Thidx (+ offset)
	t_com44 = (WORD)(((long)THERM_TBL[aidx]*10 - lr2) * 200 / ((long)THERM_TBL[aidx]*10 - (long)THERM_TBL[aidx+1]*10) + ttempidx[aidx]) + tthoff;
	
	// Temperature(base) = t_com08 / 10 - 273
	if( t_com44 <= 1450 )						// -128Åé or less ?
	{
		adegree2 = -128;
		//adegree2 = 0;
	} else {
		if( t_com44 >= 4000 )					// 127Åé or more ?
		{
			adegree2 = 127;
			//adegree2 = 255;
		} else {								// -127ÅéÅ`126Åé
			adegree2 = (_SBYTE)((t_com44/10) - 273);
			//adegree2 = (BYTE)(t_com44/10 - 145);
		}
	}

}



/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Temperature value acquisition processing
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void ReadTemp(void)
 *--------------------------------------------------------------------------
 * Function			: Measure temperature AD and  calculation
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
void ReadTemp(void)
{
	BYTE	atemp_L,atemp_H;
	BYTE	apull_L,apull_H;
	
	
	#if 0
	AFE_WR(AFE_ANPULL, 0x7); //added on 2023-06-09
	#endif
	
	AFE_WR(AFE_ADMSEL4,ADATOFF3 | ADATAN0 | ADATAN1 |ADATAN2 | ADATVREGpullAN0pull | ADATVREGpullAN1pull | ADATVREGpull);			
																		// Select offset and AN0, AN1, puli-up enabled
	AFE_WR(AFE_ADCON2,ADATTIME1 | ADATWAITTIME1);						// Select AD conver time and wait time
	AFE_WR(AFE_ADCON0,ADC_EN);											// AD enabled
	AFE_WR(AFE_ADCON1,ADC_THERMMODE);									// Start AD conversion (Thermistor mode)
	
	#if 1
	while( PIF9 == 0 ) ;												// AD complete ?
	PIF9 = 0;															// Clear Interrupt request flag(MCU)
	#endif	
	
	AFE_WR(AFE_AFIF0,INT_ADIR_W);										// Clear interrupt request flag(AFE)
	
	// AN0
	
	AFE_RD(AFE_ADRL1,atemp_L);											// get AD value
	AFE_RD(AFE_ADRH1,atemp_H);											// get AD value
	
	ad_therm = ((WORD)atemp_H << 8) + atemp_L; 
	
	AFE_RD(AFE_ADRL4,apull_L);											// get AD value
	AFE_RD(AFE_ADRH4,apull_H);											// get AD value
	
	tadvregpull = ((WORD)apull_H << 8) + apull_L; 

	// AN1
	AFE_RD(AFE_ADRL2,atemp_L);											// get AD value
	AFE_RD(AFE_ADRH2,atemp_H);											// get AD value
	
	ad_therm2 = ((WORD)atemp_H << 8) + atemp_L; 

	// AN2
	AFE_RD(AFE_ADRL3,atemp_L);											// get AD value
	AFE_RD(AFE_ADRH3,atemp_H);											// get AD value
	
	ad_therm3 = ((WORD)atemp_H << 8) + atemp_L; 
	
	AFE_RD(AFE_ADRL5,apull_L);											// get AD value
	AFE_RD(AFE_ADRH5,apull_H);											// get AD value
	
	tadvregpull2 = ((WORD)apull_H << 8) + apull_L; 
    guage.ad_temp[0]=ad_therm;
    guage.ad_temp[1]=ad_therm2;
    guage.ad_temp[2]=ad_therm3;
#if 0
	Calc_Temp();														// Temperature calculation(AN0)
	if(F_2NDTHE == ON)
	{
		Calc_2ndTemp();													// Temperature calculation(AN1)
	}
#endif    
}




/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Voltage Calculation
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void CalcVolt(BYTE acell, WORD tad)
 *--------------------------------------------------------------------------
 * Function			: Voltage Calculation
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: BYTE acell	: cell Number
 *					: WORD tad	: AD Value - offset Value
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
void CalcVolt(BYTE acell, WORD tad)
{
	if(F_CALIB1P == OFF)
	{
	// Voltage = (AD - VL) * (VrefH - VrefL) / (VH - VL) + VrefL
	// *:Vx = Calibration AD value, Vrefx = Calibration reference voltage value
		long ttmp1 = ((long)tad - D_LV[acell]) * ( D_HVREF[acell] - D_LVREF[acell] );
		WORD ttmp2 = D_HV[acell] - D_LV[acell];
		WORD ttmp3 = (WORD)( ttmp1 % ttmp2 );
		ttmp1 = ttmp1 / ttmp2;
	
		if( ttmp1 < 0 && ttmp3 != 0 )
		{
			ttmp1 -= 1;
		}
		tvolt[acell] = (WORD)(ttmp1 + D_LVREF[acell]);
	}else
	{
		WORD tvolt_w = (WORD)((long)tad * D_HVREF[acell] / D_HV[acell]);
		if( tvolt_w >= 0x8000 )						// under flow ?
		{
			tvolt_w = 0;							// Set to 0
		}
		tvolt[acell] = tvolt_w;						// Save the result
	}
}



/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Current calculation function 
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void CalcCurr(void)
 *--------------------------------------------------------------------------
 * Function			: Calculate current value
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
void CalcCurr(void)
{
	// Current = (AD - 0ACalibration value)*(Calibration reference value - 0A)/(discharge calibration value - 0Acalibration value)+0A
	//		   = (0Aeference value-AD) * |Calibration reference value| / (discharge calibration value - 0Aeference value)
	// (calibration reference value is negative number for the discharge. Calculate using the positive number)
	// Current = (AD - 0ACalib)*(Calib ref - 0A)/(DisChg calib - 0A calib)+0A
	//		   = (0ACalib - AD) * |Calib ref| / (DisChg calib - 0A calib)
	// (Calib ref is minus number due to discharge. It calculates by plus 
	//	number.)
	long lad_curr = (long)D_0A - lcurr_ad;
	long lad_calib = D_CURR - D_0A;
	t_com0a = (short)((double)lad_curr * D_CURRREF / lad_calib);

	tabsc = ABS(t_com0a);
	
	if( t_com0a < 0 )							// Current is minus number ?
	{
		if( tabsc >= D_DCDET )					// Discharge detection current or more ?
												// Dischg detect curr or more ?
		{
			f_discharge = ON;					// Set Discharging flag
		} else {								// Smaller than Dischg det.
			f_discharge = OFF;					// Clear Discharging flag
		}
		f_charge = OFF;							// Clear Charging flag
	} else {									// Plus number
		if( tabsc >= D_CCDET )					// Charge detection current or more ?
												// Chg detect curr or more ?
		{
			f_charge = ON;						// Set Charging flag
		} else {								// Smaller than Chg det.
			f_charge = OFF;						// Clear Charging flag
		}
		f_discharge = OFF;						// Clear Discharging flag
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: PowerDown function 
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void PowerDown(void)
 *--------------------------------------------------------------------------
 * Function			: PowerDown function 
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
void PowerDown(void)
{
	DI();
	amode = M_DOWN;
	AFE_WR(AFE_FCON,FET_OFF);											// FET [C=OFF,D=OFF]
	// PowerDown command
	AFE_WR(AFE_PDCNT,0xAA);
	AFE_WR(AFE_PDCNT,0x55);
	AFE_WR(AFE_PDCNT,0xFF);
	AFE_WR(AFE_PDCNT,0x00);
	EI();
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Cell Balancing function 
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void CB_Chk(void)
 *--------------------------------------------------------------------------
 * Function			: Cell Balancing function 
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
void CB_Chk(void)
{
	BYTE	al1 = 0;						
	BYTE	awork = 0,awork1 = 0;

	if( f_discharge == OFF ) 									// Charging or rest
	{		
		if( tmaxv < D_CBENVH && tminv >= D_CBENVL )				// High voltage range
		{
			if( (tmaxv - tminv) >= D_CBONV )
			{
				for( al1=0; al1<D_SERIES; al1++ )				// Imbalance checking loop
				{
					if( (tvolt[al1] - tminv) >= D_CBONV && al1 < 8)
					{
						awork |= acond_tbl[al1];				// Set to cell balance list
					}
					
					if( (tvolt[al1] - tminv) >= D_CBONV && al1 >= 8)
					{
						awork1 |= acond_tbl[al1];
					}
				}
			}
			if( acb_w != 0 )									// If cell balancing
			{
				for( al1=0; al1<D_SERIES; al1++ )				// imbalance check loop
				{
					if(al1 > 7)									// break for-loop (in case of Cell 9 or more) 
					{
						break;
					}
					
					if( (acb_w & acond_tbl[al1]) != 0 )			// Is it controlled cell ?
					{
						if( (tvolt[al1] - tminv) < D_CBOFFV )	// Is the voltage less than "Cell balance OFF voltage" ?
						{
							acb_w &= ~acond_tbl[al1];			// Clear it from cell balance list
						}
					}
					
				}
			}
			
			if( acb_w1 != 0)									// If cell balancing(Cell 9 - Cell 10)
			{
				for( al1 = 8; al1 < D_SERIES; al1++)			// imbalance check loop(Cell 9 - Cell 10)
				{
					if( (acb_w1 & acond_tbl[al1]) != 0 )		// Is it controlled cell ?
					{
						if( (tvolt[al1] - tminv) < D_CBOFFV )	// Is the voltage less than "Cell balance OFF voltage" ?
						{
							acb_w1 &= ~acond_tbl[al1];		// Clear it from cell balance list
						}
					}
				}
			}
			
		}else {													// not meet cell imbalance condition?
			acb_w = 0;
			acb_w1 = 0;
		}
	} else {													// discharging?
		acb_w = 0;
		acb_w1 = 0;
	}
	
	acb_w |= awork; 											// Merge control and start list
	acb_w1 |= awork1; 											// Merge control and start list

	a_com46[0] = acb_w;											// CellBalancing Status(Cell 1 -8)
	a_com46[1] = acb_w1 & 0x03;									// CellBalancing Status(Cell 9 -10)

	AFE_WR(AFE_WINCHG,WINCHG_RM0);								

	AFE_WR(AFE_COND0,acb_w);									// Cell Balancing (Cell 1 - Cell 8)
	AFE_WR(AFE_COND1,(acb_w1 & 0x03));							// Cell Balancing (Cell 9 - Cell 10)

	
}


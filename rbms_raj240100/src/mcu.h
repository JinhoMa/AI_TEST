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
* System Name	: RAJ240090 Sample Code
* File Name		: mcu.h
* Version		: 1.00
* Contents		: 
* Programmer	: Yasuo Usuda
* Note			: 
************************************************************************
* Copyright(C) 2016 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History			: 2016.02.15 Ver 1.00
 *					:  New Create
 *					: 
*""FILE COMMENT END""**************************************************/

#ifndef _MCU_H
#define _MCU_H

#ifdef	_MCU
#define		GLOBAL
#else
#define		GLOBAL	extern
#endif	/* _MCU */




/***********************************************************************
Global functions
***********************************************************************/
GLOBAL void CVT_Func(void);
GLOBAL void PowerDown(void);
GLOBAL void CB_Chk(void);
GLOBAL void WU_CALIB_D(void);
GLOBAL void WU_CALIB_C(void);

void CalcPackVolt(WORD tad);
void CalcCurr(void);
void Calc_ADV(void);
void Calc_Temp(void);
void ReadTemp(void);
void WaitTimer(BYTE);
void Calc_AD_Hall(void);


static void CalcVolt(BYTE acell, WORD tad);

GLOBAL BYTE			amcuflg;					// Flags for MCU
#define f_charge	DEF_BIT0(&amcuflg)			// Charging flag
#define f_discharge	DEF_BIT1(&amcuflg)			// Discharging flag


/***********************************************************************
Define Definition
***********************************************************************/
#define WU_CURR_05	20							// WakeUp current(5m ohm)


#define		ADM2_IRV		0x80

#undef		GLOBAL

#endif


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
* System Name	: RAJ240045 sample code
* File Name		: datcalc_spl.h
* Contents		: SMBus data calculating function header file
* Device		: RAJ240045
* Compiler		: CC-RL (V.4.01)
* Note			: 
*""FILE COMMENT END""**********************************************************/

#ifndef _DATCALC_SPL_H
#define _DATCALC_SPL_H

#ifdef	_DATCALC_SPL
#define		GLOBAL
#else
#define		GLOBAL	extern
#endif	/* _DATCALC_SPL */


/***********************************************************************
Global functions
***********************************************************************/
GLOBAL void Calc_RC(void);						// RemainingCapacity() calc
GLOBAL void Make_RC(void);						// RC calc from integration


void MEDF_Count(WORD tdata);					// Counting capacity for MaxError() and DataFlash update
void Calc_CC(void);								// CycleCount() calculation
void Calc_FCC_P(void);							// Calc FCC[10mWh]
void Calc_ATTF(void);							// AverageTimeToFull() calc
void FCC_Limit_Chk(void);						// FCC Limit check function
void Usage_Chk(void);							// BatteryUsageCondition func.
void Clear_BUC_Exp(void);						// BUC/Exp clear function
void Init_Cap(void);							// InitCap setting by voltage
void Make_Relearning(BYTE acp);
void Calc_CPVolt(void);
void Calc_CP1RelRC(WORD trc);
void Calc_HoseiRC(WORD	trc);
void CC_Count(WORD tdata);


// - Grobal variable -

GLOBAL WORD		toldrc;						// last RemCap value
GLOBAL WORD		tdfup_cnt;					// DataFlash update counter 
GLOBAL WORD		tccup;						// CycleCount increment cap.

GLOBAL BYTE		arelearn_cnt;					// Relearn counter
GLOBAL BYTE		arcflg2;					// Flags for RC

GLOBAL WORD		tcom10c_w;					// FCC temporary save area
GLOBAL DWORD	lfc_hold_cnt;						// Full charge hold counter
GLOBAL DWORD	lfcd_hold_cnt;						// FC discharge hold counter
GLOBAL WORD		tcph_v;						// CP_H
GLOBAL WORD		tcpl_v;						// CP_L
GLOBAL BYTE		arcflg;						// Flags for RC
GLOBAL BYTE		ausage_cnt;					// Usage condition time counter
GLOBAL WORD		toldcap;					// old Cap
		
// - bit / union variable definition -
									// Flags for RC
#define f_fcdet		DEF_BIT0(&arcflg)				// Full charge detection flag
#define f_fcdis		DEF_BIT1(&arcflg)				// Discharge detect after FC
#define f_studied	DEF_BIT2(&arcflg)				// relearned flag
#define f_lscurr	DEF_BIT3(&arcflg)				// Current detection at slow
#define f_1stcc		DEF_BIT4(&arcflg)				// 1st CC flag

#define f_0chg		DEF_BIT5(&arcflg)				// 0% voltage detected flag for Reserved Capacity function 
#define f_rcsame	DEF_BIT6(&arcflg)				// Correct/Relearn RC same flag

#define f_fcdetrh	DEF_BIT0(&arcflg2)				// Full charge detection flag	for RSOC hold
												
#define f_fcdisrh	DEF_BIT1(&arcflg2)			// Discharge detect after FC for RSOC hold
#define f_keep100	DEF_BIT2(&arcflg2)			// 100% keeping flag							
#define f_aft100	DEF_BIT3(&arcflg2)			// Smoothing function flag for fter keeping 100% 

#undef		GLOBAL

#endif

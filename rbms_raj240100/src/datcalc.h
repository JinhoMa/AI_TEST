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
* System Name	: RAJ240080 sample code
* File Name		: datcalc.h
* Contents		: SMBus data calculating function header file
* Device		: RAJ240080
* Compiler		: CC-RL (V.1.04)
* Note			: 
*""FILE COMMENT END""**********************************************************/

#ifndef _DATCALC_H
#define _DATCALC_H

#ifdef	_DATCALC
#define		GLOBAL
#else
#define		GLOBAL	extern
#endif	/* _DATCALC */


/***********************************************************************
Global functions
***********************************************************************/
GLOBAL void Calc_RSOC(void);					// RelativeStateOfCharge() calc
GLOBAL void Calc_AC(void);						// AverageCurrent()
GLOBAL void Calc_ASOC(void);					// AbsoluteStateOfCharge() calc
GLOBAL void Calc_RTTE(void);					// RunTimeToEmpty() calc
GLOBAL void Calc_ATTE(void);					// AverageTimeToEmpty() calc

/***********************************************************************
Global Hunate 
***********************************************************************/
GLOBAL   void Calc_OCV(void);			// Cell OCV Calc

GLOBAL long		l_move_w;						// AveC moving ave. add value
GLOBAL long		t_add_w;						// AveC add counter

#undef		GLOBAL

#endif

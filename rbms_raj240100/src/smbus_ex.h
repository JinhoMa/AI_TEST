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
* File Name		: smbus_ex.h
* Version		: 1.00
* Contents		: SMBus extra functions custom header file
* Programmer	: Yasuo Usuda
* Note			: 
************************************************************************
* Copyright(C) 2016 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History			: 2016.02.15 Ver 1.00
 *					:  New Create
 *					: 
*""FILE COMMENT END""**************************************************/
#ifndef _SMBUS_EX_H
#define _SMBUS_EX_H

#ifdef	_SMBUS_EX
#define		GLOBAL
#else
#define		GLOBAL	extern
#endif	/* _SMBUS_EX */


/***********************************************************************
Global functions
***********************************************************************/
GLOBAL void StoreRcvData(void);						// processing saving SMBus receive data
GLOBAL BYTE ComExist_Chk(void);						// Command check
GLOBAL void SetSMBusRead(void);						// processing read data setting
GLOBAL void BootJump_Chk(void);					// boot jump check


void Calc_AtRate05(void);
void Calc_AtRate06(void);
void Calc_AtRate07(void);

GLOBAL BYTE aextflg;							// Flags for ExtraFunction

#define f_comjib	DEF_BIT0(&aextflg)			// Force PowerDown flag
#define	f_fetctl	DEF_BIT1(&aextflg)			// FET control flag(for test)
#define	f_fcslp		DEF_BIT2(&aextflg)			// Force Sleep flag
#define	f_fettst	DEF_BIT3(&aextflg)			// FET test mode
#define f_slp2min	DEF_BIT4(&aextflg)			// 2 min passed flag
#define f_slpcalc	DEF_BIT5(&aextflg)			// calc req in sleep mode flag


#undef		GLOBAL


#endif


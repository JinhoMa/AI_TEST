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
* File Name		: afe2.h
* Version		: 1.00
* Contents		: afe function header file
* Programmer	: Yasuo Usuda
* Note			: 
************************************************************************
* Copyright(C) 2016 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History			: 2016.02.15 Ver 1.00
 *					:  New Create
 *					: 
*""FILE COMMENT END""**************************************************/

#ifndef _AFE_H_2
#define _AFE_H_2 

/************************************WINDOW 2*************************/
#define AFE_AOCOT0	AFEREG04							//(F0504)
#define AFE_AOCOT1	AFEREG05							//(F0505)
#define AFE_AOCOT2	AFEREG06							//(F0506)
#define AFE_ALOCOT0	AFEREG07							//(F0507)
#define AFE_ALOCOT1	AFEREG08							//(F0508)
#define AFE_BT0		AFEREG0A							//(F050A)
#define AFE_BT1		AFEREG0B							//(F050B)
#define AFE_RT0		AFEREG0C							//(F050C)
#define AFE_RT1		AFEREG0D							//(F050D)
#define AFE_BUFT0	AFEREG0E							//(F050E)
#define AFE_BUFT1	AFEREG0F							//(F050F)
#define AFE_REG2T	AFEREG10							//(F0510)
#define AFE_FCPT0	AFEREG11							//(F0511)

// Trimming data for AFE
#define B_AFE_FET0		*(__far BYTE*)0x01C86D			// B_AFE_FET0
#define B_AFE_VREG2_5V	*(__far BYTE*)0x01C86C			// AFE-VREG2(5.0V)
#define B_AFE_VREG2_3V	*(__far BYTE*)0x01C86B			// AFE-VREG2(3.3V)
#define B_AFE_IVT1		*(__far BYTE*)0x01C86A			// AFE-IV TRIM1
#define B_AFE_IVT0		*(__far BYTE*)0x01C869			// AFE-IV TRIM0
#define B_AFE_VREGAD	*(__far BYTE*)0x01C867			// AFE-VREGAD(RT0)
#define B_AFE_VREGCC	*(__far BYTE*)0x01C868			// AFE-VREGCC(RT1)
#define B_AFE_BT1		*(__far BYTE*)0x01C866			// AFE-BT1
#define B_AFE_BT0		*(__far BYTE*)0x01C865			// AFE-BT0
#define B_AFE_LOCO1		*(__far BYTE*)0x01C864			// AFE-LOCO1
#define B_AFE_LOCO0		*(__far BYTE*)0x01C863			// AFE-LOCO0
#define B_AFE_OCO2		*(__far BYTE*)0x01C862			// AFE-OCO2
#define B_AFE_OCO1		*(__far BYTE*)0x01C861			// AFE-OCO1
#define B_AFE_OCO0		*(__far BYTE*)0x01C860			// AFE-OCO0

#endif

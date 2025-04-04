/*******************************************************************************
* Copyright(C) 2008, 2019 Renesas Electronics Corporation
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
* System Name	: RAJ240075 sample program
* File Name		: flashrom.h
* Contents		: flashrom function header file
* Device		: RAJ240075
* Compiler		: CC-RL
* Note			: 
*""FILE COMMENT END""**********************************************************/

#ifndef _FLASHROM_H
#define _FLASHROM_H

#ifdef	_FLASHROM
#define		GLOBAL
#else
#define		GLOBAL	extern
#endif	/* _FLASHROM */

/*
// - Control flag definition -
typedef struct
{
	BYTE DUMMY0		:1;		// bit  0 : Reserved
	BYTE DUMMY1		:1;		// bit  1 : Reserved
	BYTE DUMMY2		:1;		// bit  2 : Reserved
	BYTE f_sysconen	:1;		// bit  3 : SYS-connect enabled flag
	BYTE f_calib1p	:1;		// bit  4 : Initial Calibration 1Point mode flag
							//         (1=use, 0=not use)
	BYTE DUMMY5		:1;		// bit  5 : Resreved
	BYTE DUMMY6		:1;		// bit  6 : Reserved
	BYTE DUMMY7		:1;		// bit  7 : Resreved
	BYTE DUMMY8		:1;		// bit  8 : Resreved
	BYTE DUMMY9		:1;		// bit  9 : Resreved
	BYTE DUMMY10	:1;		// bit 10 : Resreved
	BYTE DUMMY11	:1;		// bit 11 : Resreved
	BYTE DUMMY12	:1;		// bit 12 : Resreved
	BYTE DUMMY13	:1;		// bit 13 : Resreved
	BYTE DUMMY14	:1;		// bit 14 : Resreved
	BYTE DUMMY15	:1;		// bit 15 : Resreved
}_ST_CTLFLG3;

typedef struct
{
	BYTE DUMMY0		:1;		// bit  0 : Resreved
	BYTE f_shipen	:1;		// bit  1 : Shipping mode using flag
	BYTE f_uvpuse	:1;		// bit  2 : UVP condition using flag 
	BYTE DUMMY3		:1;		// bit  3 : Reserved
	BYTE DUMMY4		:1;		// bit  4 : Resreved
	BYTE DUMMY5		:1;		// bit  5 : Reserved
	BYTE DUMMY6		:1;		// bit  6 : Reserved
	BYTE DUMMY7		:1;		// bit  7 : Reserved
	BYTE DUMMY8		:1;		// bit  8 : Reserved
	BYTE DUMMY9		:1;		// bit  9 : Resreved
	BYTE DUMMY10	:1;		// bit 10 : Resreved
	BYTE DUMMY11	:1;		// bit 11 : Resreved
	BYTE DUMMY12	:1;		// bit 12 : Resreved
	BYTE DUMMY13	:1;		// bit 13 : Resreved
	BYTE DUMMY14	:1;		// bit 14 : Resreved
	BYTE DUMMY15	:1;		// bit 15 : Resreved
}_ST_CTLFLG4;

typedef struct
{
	BYTE DUMMY0		:1;		// bit  0 : Resreved
	BYTE DUMMY1		:1;		// bit  1 : Resreved
	BYTE f_ct_con	:1;		// bit  2 : C-FET ON at ChgTerm (1=ON, 0=OFF)
	BYTE DUMMY3		:1;		// bit  3 : Resreved
	BYTE f_fdext	:1;		// bit  4 : FD additional flag
							//          (1=Use, 0=Not use)
	BYTE f_maxer	:1;		// bit  5 : MaxError function (1=Use, 0=Not use)
	BYTE DUMMY6		:1;		// Bit  6 : Resreved
	BYTE DUMMY7		:1;		// bit  7 : Resreved
	BYTE DUMMY8		:1;		// bit  8 : Reserved
	BYTE DUMMY9		:1;		// bit  9 : Resreved
	BYTE f_initcap	:1;		// bit 10 : InitCap table using flag
							//          (1=Use, 0=Not use)
	BYTE DUMMY11	:1;		// bit 11 : Resreved
	BYTE DUMMY12	:1;		// bit 12 : Resreved
	BYTE DUMMY13	:1;		// bit 13 : Resreved
	BYTE DUMMY14	:1;		// bit 14 : Resreved
	BYTE DUMMY15	:1;		// bit 15 : Resreved
}_ST_CTLFLG5;

typedef struct							// Over Current Release conditions flag
{
	BYTE f_r1		:1;					// bit  0 : 30 second has passed (1=ON, 0=OFF)
	BYTE f_r2		:1;					// bit  1 : detect OffState (1=ON, 0=OFF)
	BYTE f_r3		:1;					// bit  2 : detect opposite current (1=ON, 0=OFF)
	BYTE f_r4		:1;					// bit  3 : SYS-IN=OFF (1=ON, 0=OFF)
	BYTE f_r5		:1;					// bit  4 : AverageCurrent (1=ON, 0=OFF)
	BYTE f_r6		:1;					// bit  5 : AverageCurrent setting time
										//          (1=ON, 0=OFF)
	BYTE f_r7		:1;					// bit  6 : OCP Latch
	BYTE f_fet		:1;					// bit  7 : DFET(ODC)/CFET(OCC) condition
										//          (1=ON, 0=OFF)
}_ST_OCRFLG;
typedef union {
	_ST_OCRFLG	f;
	BYTE		a;
} _OCRFLG;

*/

/***********************************************************************
Global functions
***********************************************************************/
//GLOBAL void FlashWrite(BYTE *apadr, BYTE *apdata, BYTE abytecnt);
__callt BYTE DFlashWrite(__far BYTE *apadr, BYTE *apdata, WORD abytecnt);
//GLOBAL void FlashErase(void);
void DFlashErase(BYTE aerase_no);
BYTE FlashErase_Chk(void);
//void Waittimer_us(BYTE ainval);

//	Data flash memory clock
#define		DFLASH_8M		8
#define		DFLASH_VALUE	DFLASH_8M


#undef		GLOBAL

#endif


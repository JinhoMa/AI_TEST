/******************************************************************************
* DISCLAIMER																	*
* This software is supplied by Renesas Electronics Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.
*
* This software is owned by Renesas Electronics Corp. and is protected under 
* all applicable laws, including copyright laws.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.	ALL SUCH WARRANTIES ARE EXPRESSLY 
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
* File Name		: smbus.h
* Version		: 1.00
* Contents		: SMBus functions header file
* Programmer	: Yasuo Usuda
* Note			: 
************************************************************************
* Copyright(C) 2016 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History			: 2016.02.15 Ver 1.00
 *					:  New Create
 *					: 
*""FILE COMMENT END""**************************************************/
#ifndef _SMBUS_H
#define _SMBUS_H

#ifdef	_SMBUS
#define			GLOBAL
#else
#define			GLOBAL	extern
#endif
/* _SMBUS */


/***********************************************************************
Global functions
***********************************************************************/
GLOBAL void SMBus_init(void);											// SMBus initialize
GLOBAL void Timeout_Chk(void);											// SMBus timeout check
GLOBAL void SMB_Timeout(void);											// SMBus timeout

/***********************************************************************
Global functions
***********************************************************************/
GLOBAL BYTE		anosmb_cnt;												// No communication time counter
GLOBAL BYTE		aslvto_cnt;												// Slave timeout counter
GLOBAL BYTE		asclto_cnt;												// SCL communication timeout counter
GLOBAL BYTE		acccv_cnt;												// CC/CV time counter

GLOBAL BYTE		acomflg1;												// Flags for SMBus (1)
#define f_slvto_req				DEF_BIT0(&acomflg1)						// Slave communication timeout check request flag
#define f_sclto_req				DEF_BIT1(&acomflg1)						// SCL timeout check request flag
#define f_slv_to				DEF_BIT2(&acomflg1)						// Slave timeout flag
#define f_scl_to				DEF_BIT3(&acomflg1)						// SCL timeout flag

GLOBAL BYTE		acomflg2;												// Flags for SMBus (2)
#define f_nosmb					DEF_BIT0(&acomflg2)						// No communication flag
#define f_recv					DEF_BIT1(&acomflg2)						// Data received flag
#define f_wwpec					DEF_BIT2(&acomflg2)						// PEC sent flag
#define f_comerr				DEF_BIT3(&acomflg2)						// Communication error flag
#define f_flashup				DEF_BIT4(&acomflg2)						// Flash update mode move flag


GLOBAL _ST_SMB ssmbdata;												// SMBus communication information

GLOBAL BYTE		arcv_cmd;												// Receive command
GLOBAL BYTE		arem_cc;												// Number of communication data
GLOBAL BYTE		asmbuf[40];												// Slave receive buffer

GLOBAL BYTE		aiic_sadr;												// Slave address
#define			ADDR_ERR				0xA5
GLOBAL BYTE		aerr_cnt;												// SMBus error check counter
#define			NO_RESET_CNT			5								// No reset count(SDA low edge count)
#define			SDA_MAX_CNT				200								// SDA max count(MAX:255)


#undef			GLOBAL


#endif


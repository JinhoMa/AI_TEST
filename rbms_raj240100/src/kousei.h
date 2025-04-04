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
* File Name				: kousei.h
* Version				: 1.00
* Contents				: Initial calibration functions header file
* Programmer			: Yasuo Usuda
* Note					: 
************************************************************************
* Copyright(C) 2016 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History						: 2016.02.15 Ver 1.00
 *								:  New Create
 *								: 
*""FILE COMMENT END""**************************************************/
#ifndef _KOUSEI_H
#define _KOUSEI_H



#ifdef	_KOUSEI
#define			GLOBAL
#else
#define			GLOBAL	extern
#endif	/* _KOUSEI */


/***********************************************************************
Global define
***********************************************************************/
GLOBAL _SHORT2CHAR		tcalib;									// Calibration related data
#define t_com58			tcalib.us								// SMBus interface
#define acalib_id		tcalib.uc[0]							// Calibration item
#define acalib_sts		tcalib.uc[1]							// Calibration status
GLOBAL BYTE				acalib_time;							// Calibration passage time
GLOBAL BYTE				acalibflg;
#define f_calibmode		DEF_BIT0(&acalibflg)					// Flag during calibration mode
#define f_calib			DEF_BIT1(&acalibflg)					// Flag for calibration requirement
#define f_adccalib		DEF_BIT2(&acalibflg)					// Flag for AD conversion requirement in calibration
#define f_skipcalib		DEF_BIT3(&acalibflg)					// Flag for calibration skip
GLOBAL WORD				trefhv_lv[10];							// Calibration data:VrefH - VrefL
GLOBAL WORD				thv_lv[10];								// Calibration data:VH - VL
GLOBAL _SWORD			tthoff;									// Calibration data:Temepreatureoffset
GLOBAL DWORD			lccaldef;								// Calibration data:Discharge calibration - 0A calibration
GLOBAL WORD				trefhl_vin12;							// Calibration data:VIN12 VrefH - L
GLOBAL WORD				thlvin12;								// Calibration data:VIN12 VH - VL
GLOBAL WORD				tadvss;									// ADVSS AD value
GLOBAL WORD				tadvcc;									// ADVCC AD value

#ifdef	TCV_EN
GLOBAL WORD				trefhl_tcv;								// Calibration data:Total Cell Voltage VrefH - L
GLOBAL WORD				thltcv;									// Calibration data:Total Cell Voltage VH - VL
#endif

// Calibration default value
#define	cellL_ad	0x26AC
#define	cellH_ad	0x4d3a
#define	cellL_ref	0x07d0
#define	cellH_ref	0x0fa0
#define	packL_ad	0x1e79
#define	packH_ad	0x3d0c
#define	packL_ref	0x3e80
#define	packH_ref	0x7d00
#define	rsv_calib	0xFFFF

#define	curr_ad0		0x0001ffd7
#define	curr_ad0_low	curr_ad0 & 0x0000FFFF
#define	curr_ad0_high  (curr_ad0 >> 16) & 0x0000FFFF

#define	curr_ad			0x00020239
#define	curr_ad_low		curr_ad & 0x0000FFFF
#define	curr_ad_high  	(curr_ad >> 16) & 0x0000FFFF

#define	curr_ref		0x01f4
#define	temp_ad			0x0af5
#define	temp_ref		0x0ba4
#define	tcvL_ad			0x1ec5
#define	tcvH_ad			0x3da9
#define	tcvL_ref		0x3e80
#define	tcvH_ref		0x7d00
#define	mcubg_ad		0x1c75
#define	creg2_ad		0x401b



/***********************************************************************
Global functions
***********************************************************************/
GLOBAL BYTE InitCalib_Chk(void);								// Check process on initial calibration data
GLOBAL void KouseiMain(void);									// Calibration Main process
GLOBAL void Init_CalibDat(void);								// Calc initial calibration data



/***********************************************************************
Define Definition
***********************************************************************/
#define CALIB_REQ				0x01							// Calibration requirement
#define CALIB_OK				0x08							// Calibration OK
#define CALIB_NG				0x04							// Calibration NG

																// Calibration item number
#define CALIB_LV1				0x00
#define CALIB_LV2				0x01
#define CALIB_LV3				0x02
#define CALIB_LV4				0x03
#define CALIB_LV5				0x04
#define CALIB_LV6				0x05
#define CALIB_LV7				0x06
#define CALIB_LV8				0x07
#define CALIB_LV9				0x08
#define CALIB_LV10				0x09
#define CALIB_VIN12_LV			0x0a
#define CALIB_VIN12_HV			0x0b
#define CALIB_LVALL				0x0c
#define CALIB_HVALL				0x0d
#define CALIB_0A				0x0e
#define CALIB_CURR				0x0f
#define CALIB_TEMP				0x10
#define CALIB_HV1				0x20
#define CALIB_HV2				0x21
#define CALIB_HV3				0x22
#define CALIB_HV4				0x23
#define CALIB_HV5				0x24
#define CALIB_HV6				0x25
#define CALIB_HV7				0x26
#define CALIB_HV8				0x27
#define CALIB_HV9				0x28
#define CALIB_HV10				0x29

#ifdef	MCUBG_EN
#define CALIB_MCUBG				0x2a
#endif

#ifdef	CREG2_EN
#define CALIB_CREG2				0x2b
#endif

#ifdef	TCV_EN
#define CALIB_TCLV				0x2c
#define CALIB_TCHV				0x2d
#endif


#undef			GLOBAL

#endif


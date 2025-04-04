/***********************************************************************************************************************
 DISCLAIMER
 This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
 No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 applicable laws, including copyright laws. 
 THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
 LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
 INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
 ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
 of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
 following link:
 http://www.renesas.com/disclaimer

 Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : stl_adc.h
* Version      : 3.00
* Device(s)    : RL78 (Test device used RL78/G14)
* Tool-Chain   : RL78 CC-RL V1.08.00
* Note         :
***********************************************************************************************************************/

#ifndef STL_ADC_H
#define STL_ADC_H

/**********************************************************************************
Revision History
DD-MMM-YYYY REV-UID Description
* 27.09.2019 V3.00 Extened Version
***********************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

#define OVERALL_ERROR_LSB_UNIT  (7)                                             /* AD Converter Overall error */
#define VSS_RANGE_MAX           (0 + OVERALL_ERROR_LSB_UNIT)                    /* Vss Range Max */
#define AD_RESOLUTION_HEX       (0x03FF)                                        /* AD Converter Resolution */
#define VDD_RANGE_MIN           (AD_RESOLUTION_HEX - OVERALL_ERROR_LSB_UNIT)    /* Vdd Range Min */
#define VDD                     (3.3)                                           /* Vdd */
#define VBGR_MIN                (1.38)                                          /* Vbgr Min */
#define VBGR_MAX                (1.5)                                           /* Vbgr Max */
#define VBGR_RANGE_MIN          ((VBGR_MIN / (VDD / AD_RESOLUTION_HEX)) - OVERALL_ERROR_LSB_UNIT)   /* Vdd Range Min */
#define VBGR_RANGE_MAX          ((VBGR_MAX / (VDD / AD_RESOLUTION_HEX)) + OVERALL_ERROR_LSB_UNIT)   /* Vdd Range Max */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void stl_ADC_Create(void);
char stl_ADC_Check_TestVoltage(void);
#endif

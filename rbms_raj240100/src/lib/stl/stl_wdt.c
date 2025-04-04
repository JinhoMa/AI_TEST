/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : stl_wdt.c
* Version      : 3.00
* Device(s)    : RL78 (Test device used RL78/G14)
* Tool-Chain   : RL78 CC-RL V1.08.00
* OS           : None
* H/W Platform : RL78/G14 Target Board
* Note         : 
***********************************************************************************************************************/

/**********************************************************************************
Revision History
DD-MMM-YYYY REV-UID Description
* 27.09.2019 V3.00 Extened Version
***********************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "iodefine.h" 
#ifdef WDT_Test_enabled
#include "stl_wdt.h"


/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/


/***********************************************************************************
Function Name : stl_wdt_Kick

Description   :	Kick (refesh) the watch dog count
				Must get called before count overflows.
				NOTE: The first time this is called it starts the counter.
Parameters    : 
Return value  : 
***********************************************************************************/
void stl_wdt_Kick(void)
{
	/* Watchdog timer refresh */
	WDTE = 0xAC;
}
#endif

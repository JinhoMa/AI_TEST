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
* File Name    : stl_adc.c
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
#include "stl.h"
#include "stl_adc.h"

#define NOP     __nop

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
static uint8_t testVoltageIndex;

/***********************************************************************************************************************
* Function Name: stl_ADC_Create
* Description  : This function initializes the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void stl_ADC_Create(void)
{
    ADCEN = 1U;  /* supply AD clock */
    ADM0 = 0x00U;  /* disable AD conversion and clear ADM0 register */
    ADMK = 1U;  /* disable INTAD interrupt */
    ADIF = 0U;  /* clear INTAD interrupt flag */
    /* The reset status of ADPC is analog input, so it's unnecessary to set. */
    ADM0 = 0x00;
    ADM1 = 0x20;
    ADM2 = 0x00;
    ADUL = 0xFF;
    ADLL = 0x00;
    ADS = 0x81;

    ADCE = 1U;  /* enable AD comparator */

    testVoltageIndex = 0;
}

/******************************************************************************
* Function Name: stl_ADC_Check_TestVoltage
* Description  : This function changes the voltage which does AD test.
* Arguments    : nothig
* Return Value : 0x00: OK
*                0x01: NG
******************************************************************************/
char stl_ADC_Check_TestVoltage(void)
{
uint16_t low_range;
uint16_t high_range;
uint16_t result_buffer;
int8_t  result = 0;

    switch (testVoltageIndex)
    {
        case 0:     /* Vss */
            /* Set AD test voltage */
            ADTES = 0x02;
            low_range = 0x0000;
            high_range = VSS_RANGE_MAX;
            testVoltageIndex++;
            break;
            
        case 1:     /* Vdd */
            /* Set AD test voltage */
            ADTES = 0x03;
            low_range = VDD_RANGE_MIN;
            high_range = AD_RESOLUTION_HEX;
            testVoltageIndex++;
            break;
            
        case 2:     /* Vbgr */
        default:
           /* Set analog input channel */
            ADS = 0x81;
            /* Set AD test voltage */
            ADTES = 0x00;
            low_range = (uint16_t)VBGR_RANGE_MIN;
            high_range = (uint16_t)VBGR_RANGE_MAX;
            testVoltageIndex = 0;
            break;
    }
    
    /* Starts the AD converter */
    ADCS = 1U;

  	NOP(); NOP(); NOP(); NOP(); NOP(); NOP();

	/*Wait for conversion to finish */
	while(0 == ADIF){NOP();}

    ADIF = 0U;  /* clear INTAD interrupt flag */

    /* Get AD conversion result */
    result_buffer = (uint16_t)(ADCR >> 6U);

    /* Check AD data */
    if (!((low_range <= result_buffer) && (result_buffer <= high_range)))
    {
        result = 1;
    }

    return result;
}

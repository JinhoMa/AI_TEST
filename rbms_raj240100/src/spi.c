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
* System Name	: RAJ240090/100 sample program
* File Name	: SPI.c
* Contents	: SPI functions
* Device	: RAJ240090/100
* Compiler	: CC-RL (V.4.01)
* Note		: 
*""FILE COMMENT END""**********************************************************/
#define _SPI

/***********************************************************************
** Include files
***********************************************************************/
#include "define.h"								// union/define definition
#include "spi.h"								// SPI.c


#pragma interrupt _int_SPI_CSI00(vect=INTCSI00)

// - Internal function prototype -
static void SPI_csi00_callback_error(BYTE err_type);
static void SPI_csi00_callback_receiveend(void);
static void SPI_csi00_callback_sendend(void);

/*""FUNC COMMENT""**********************************************************
 * ID			: 1.0
 * module outline	: SPI interrupt function
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: _int_SPI_csi00()
 *--------------------------------------------------------------------------
 * Function		: SPI interrupt function.
 *--------------------------------------------------------------------------
 * Argument		: None
 *--------------------------------------------------------------------------
 * Return		: None
 *--------------------------------------------------------------------------
 * Input		: None
 *			: 
 * Output		: None
 *			: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *			: 
 *--------------------------------------------------------------------------
 * Caution		: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void _int_SPI_CSI00(void)
{
    volatile BYTE err_type;
    volatile BYTE sio_dummy;

    err_type = (BYTE)(SSR00 & 0x0001);
    SIR00 = (WORD)err_type;

    if (1U == err_type)
    {
        SPI_csi00_callback_error(err_type);    /* overrun error occurs */
    }
    else
    {
        if (g_csi00_tx_count > 0U)
        {
            if (0U != gp_csi00_rx_address)
            {
                *gp_csi00_rx_address = SIO00;
                gp_csi00_rx_address++;
            }
            else
            {
                sio_dummy = SIO00;
            }

            if (0U != gp_csi00_tx_address)
            {
                SIO00 = *gp_csi00_tx_address;
                gp_csi00_tx_address++;
            }
            else
            {
                SIO00 = 0xFFU;
            }

            g_csi00_tx_count--;
        }
        else 
        {
            if (0U == g_csi00_tx_count)
            {
                if (0U != gp_csi00_rx_address)
                {
                    *gp_csi00_rx_address = SIO00;
                }
                else
                {
                    sio_dummy = SIO00;
                }
            }

            SPI_csi00_callback_sendend();		/* complete send */
            SPI_csi00_callback_receiveend();	/* complete receive */
        }
    }
}

/*""FUNC COMMENT""**********************************************************
 * module outline	: SPI callback function
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: void SPI_csi00_callback_receiveend(void)
 *--------------------------------------------------------------------------
 * Function		: Function of SPI receiveend.
 *--------------------------------------------------------------------------
 * Argument		: None
 *--------------------------------------------------------------------------
 * Return		: None
 *--------------------------------------------------------------------------
 * Input		: None
 *			: 
 * Output		: None
 *			: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *			: 
 *--------------------------------------------------------------------------
 * Caution		: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void SPI_csi00_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/*""FUNC COMMENT""**********************************************************
 * module outline	: SPI callback error function
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: void SPI_csi00_callback_error(void)
 *--------------------------------------------------------------------------
 * Function		: Function of SPI callback error.
 *--------------------------------------------------------------------------
 * Argument		: None
 *--------------------------------------------------------------------------
 * Return		: None
 *--------------------------------------------------------------------------
 * Input		: None
 *			: 
 * Output		: None
 *			: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *			: 
 *--------------------------------------------------------------------------
 * Caution		: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void SPI_csi00_callback_error(BYTE err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/*""FUNC COMMENT""**********************************************************
 * module outline	: SPI callback function
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: void SPI_csi00_callback_sendend(void)
 *--------------------------------------------------------------------------
 * Function		: Function of SPI sendend.
 *--------------------------------------------------------------------------
 * Argument		: None
 *--------------------------------------------------------------------------
 * Return		: None
 *--------------------------------------------------------------------------
 * Input		: None
 *			: 
 * Output		: None
 *			: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *			: 
 *--------------------------------------------------------------------------
 * Caution		: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void SPI_csi00_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/*""FUNC COMMENT""**********************************************************
 * ID			: 1.0
 * module outline	: SPI initialize function
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: void SPI_init(void)
 *--------------------------------------------------------------------------
 * Function		: Function of SPI initialization.
 *--------------------------------------------------------------------------
 * Argument		: None
 *--------------------------------------------------------------------------
 * Return		: None
 *--------------------------------------------------------------------------
 * Input		: None
 *			: 
 * Output		: None
 *			: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *			: 
 *--------------------------------------------------------------------------
 * Caution		: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void SPI_init(void)
{
    // Refer User Manual 512page.
    
    SAU0EN |= 1;			// 
    SPS0= SPI_SPS0_1000000;	//1000kbps 
    SMR00 = 0x0020;		// Operation clock CKm0 set by the SPSm register. 
    				// Divided operation clock fmck specified by the CKSmn bit
				// Only software trigger is valid(selected for CSI)
				// Falling edge is detected as the start bit. CSI mode.
				// Transfer end interrupt
				
    SCR00 = 0xC087;		// Transmission/reception, data and clock phase
    				// disable generation of error interrupt INTSREx(INTSRx is generated)
				// No parity bit. Input/output data with LSB first. No stop bit.
				// 8bit data length.    

    SDR00 = 0x0000;		// fmck/2 = 500kbps

    SO0 |= 0x0100;		/* CSI00 clock initial level */
    SO0 &= ~0x0001;		/* CSI00 SO initial level */

    SOE0 |= 0x0001;		/* enable CSI00 output */

#if 0
    PM1 |= 0x02U;		/* Set SI00(P11) pin */
    P1 |= 0x04U;
    PM1 &= 0xFBU;		/* Set SO00(P12) pin */
    P1 |= 0x01U;
    PM1 &= 0xFEU;		/* Set SCK00(P10) pin */
#endif
    
    PM1_bit.no0=0;		// OUTPUT(ADC_CLK) for NTC
    PM1_bit.no1=1;		// INPUT(ADC_MISO) for NTC
    PM1_bit.no2=0;		// OUTPUT(ADC_MOSI) for NTC
    PM1_bit.no6=0;		// OUTPUT(CS)
    
    P1_bit.no0=1;		// for NTC clock, MOSI,CS pin setting.
    P1_bit.no2=0;    
    CS_OFF;    
    
    ST0 |= 0x0001;		/* disable CSI00 */
    CSIMK00 = 1U;		/* disable INTCSI00 interrupt */
    CSIIF00 = 0U;		/* clear INTCSI00 interrupt flag */
    CSIPR100 = 1U;		/* Set INTCSI00 low priority */
    CSIPR000 = 1U;
    SIR00 = 0x0003;		// clear trigger of framing error, parity error, overrun error.
    
    SS0 |= 0x0001;		/* enable CSI00 */
    CSIIF00 = 0U;		/* clear INTCSI00 interrupt flag */
    CSIMK00 = 0U;		/* enable INTCSI00 */
}

/*""FUNC COMMENT""**********************************************************
 * ID			: 1.0
 * module outline	: SPI start function
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: void SPI_Start(void)
 *--------------------------------------------------------------------------
 * Function		: Function of SPI start.
 *--------------------------------------------------------------------------
 * Argument		: None
 *--------------------------------------------------------------------------
 * Return		: None
 *--------------------------------------------------------------------------
 * Input		: None
 *			: 
 * Output		: None
 *			: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *			: 
 *--------------------------------------------------------------------------
 * Caution		: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void SPI_Start(void)
{
    SO0 |= 0x0100;		/* CSI00 clock initial level */
    SO0 &= ~0x0001;		/* CSI00 SO initial level */
    SOE0 |= 0x0001;		/* enable CSI00 output */
    SS0 |= 0x0001;		/* enable CSI00 */
    CSIIF00 = 0U;		/* clear INTCSI00 interrupt flag */
    CSIMK00 = 0U;		/* enable INTCSI00 */
}

/*""FUNC COMMENT""**********************************************************
 * ID			: 1.0
 * module outline	: SPI start function
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: void SPI_Stop(void)
 *--------------------------------------------------------------------------
 * Function		: Function of SPI stop.
 *--------------------------------------------------------------------------
 * Argument		: None
 *--------------------------------------------------------------------------
 * Return		: None
 *--------------------------------------------------------------------------
 * Input		: None
 *			: 
 * Output		: None
 *			: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *			: 
 *--------------------------------------------------------------------------
 * Caution		: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void SPI_Stop(void)
{
    CSIMK00 = 1U;		/* disable INTCSI00 interrupt */
    ST0 |= 0x0001;		/* disable CSI00 */
    SOE0 &= ~0x0001;	/* disable CSI00 output */
    CSIIF00 = 0U;		/* clear INTCSI00 interrupt flag */
}

void SpiGetData(DWORD addr, WORD size)
{
	BYTE	temp;
	WORD	i;

	CS_ON;
	SpiBegin();

	temp = 0x03;							/* command */
	SpiSendByte(temp);
	temp = (BYTE)((addr >> 16) & 0xff);		/* a23 - a16(byte2) */
	SpiSendByte(temp);
	temp = (BYTE)((addr >> 8) & 0xff);		/* a15 - a8(byte3) */
	SpiSendByte(temp);
	temp = (BYTE)(addr & 0xff);				/* a7 - a0(byte4) */
	SpiSendByte(temp);

	SpiEnd();
	SCR00 |= 0xC000;						/* Transmit&receive enable */
	SpiBegin();
	
	for(i=0; i<size; i++)
	{
		temp = 0xff;
		SIO00 = temp;						/* Writes dummy data to SDR in order to receive data. */
		
		while(!CSIIF00);					/* Loop for receive completion */
		CSIIF00 = 0;						/* clear INTCSI00 interrupt flag */
			
		if((SSR00 & 0x0001) != 1U)			/* SAU OVERRUN ERROR ? */
		{
			Read_buf[i] = SIO00;			/* Get data from receive buffer */
		}
		else
		{
			SIR00 = 0x0001;					/* error flag(OVF) clear */

			/* error process */
		}
	}
	
	SpiEnd();
	CS_OFF;
	
	SCR00 &= 0x8007U;	/* Transmit enable & receive disable */
}

BYTE SpiSendByte(BYTE obyte)
{
	SIO00 = obyte;
	
	while(!CSIIF00);	/* CSIIF00 : Transmission preparation completion check */
	CSIIF00 = 0;		/* clear INTCSI00 interrupt flag */
	
	return obyte;
}

void SpiBegin(void)
{
	SOE0 |= 0x0001;		/* enable CSI00 output */
	SS0 |= 0x0001;		/* enable CSI00 */
	CSIIF00 = 0;		/* clear INTCSI00 interrupt flag */
}

void SpiEnd(void)
{
	ST0 |= 0x0001;		/* disable CSI00 */
	SOE0 &= ~0x0001;	/* disable CSI00 output */
	CSIIF00 = 0;		/* clear INTCSI00 interrupt flag */
}
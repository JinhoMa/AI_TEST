/******************************************************************************
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
* System Name	: RAJ240090/100 Sample Code
* File Name	: spi.h
* Version	: 1.00
* Contents	: SPI functions header file
* Programmer	: SJ Kwon
* Note		: 
************************************************************************
* Copyright(C) 2016, 2019 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History	: 2020.02.26 Ver 1.00
 *		: New Create
 *		: 
*""FILE COMMENT END""**************************************************/
#ifndef _SPI_H
#define _SPI_H

#ifdef	_SPI
#define			GLOBAL
#else
#define			GLOBAL	extern
#endif
/* _SP */

/***********************************************************************
Global functions
***********************************************************************/
GLOBAL void SPI_init(void);					// SPI initialize
GLOBAL void SPI_Start(void);				// SPI start
GLOBAL void SPI_Stop(void);					// SPI stop
GLOBAL void SpiGetData(DWORD data, WORD size);
GLOBAL void SpiBegin(void);
GLOBAL void SpiEnd(void);
GLOBAL BYTE SpiSendByte(BYTE obyte);

GLOBAL BYTE * gp_csi00_rx_address;			/* csi00 receive buffer address */
GLOBAL WORD  g_csi00_rx_length;				/* csi00 receive data length */
GLOBAL WORD  g_csi00_rx_count;				/* csi00 receive data count */
GLOBAL BYTE * gp_csi00_tx_address;			/* csi00 send buffer address */
GLOBAL WORD  g_csi00_send_length;			/* csi00 send data length */
GLOBAL WORD  g_csi00_tx_count;				/* csi00 send data count */

#define	BUF_SIZE		(10)
#define CS_ON			(P1_bit.no6 = 0)
#define CS_OFF			(P1_bit.no6 = 1)

// SPI SPS register
#define SPI_SPS0_1000000		0x0033

GLOBAL BYTE	Read_buf[BUF_SIZE];

#undef			GLOBAL

#endif

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
* System Name	: RAJ240XXX UartBoot
* File Name		: uart.h
* Version		: 0.01
* Contents		: UART functions header file
* Customer		: 
* Model			: 
* Order			: 
* CPU			: RAJ240XXX
* Compiler		: CA78K0R (V.1.30)
* OS			: None
* Programmer	: Hiroaki Marutani
* Note			: 
************************************************************************
* Copyright(C) 2012 (2012-2012) RENESAS ELECTRONICS CORPORATION
*                AND RENESAS SOLUTIONS CORPORATION
************************************************************************
* History		: 2012.05.01 Ver 0.01
* 				: New Create
*""FILE COMMENT END""**************************************************/

#ifndef _UART_H
#define _UART_H

#ifdef	_UART
#define		GLOBAL
#else
#define		GLOBAL	extern
#endif	/* _UART */


/***********************************************************************
	Global function prototypes
***********************************************************************/
//Uart BSP Function Prototype
GLOBAL void Uart_BspInit(void);				// UART initialize
GLOBAL void Uart_BspSend(char mode, unsigned char *buf, unsigned char len);	// UART send function
GLOBAL void Uart_int(void);					// UART receive function
GLOBAL void Uart_error_int(void);			// UART error function
GLOBAL void Uart_stop(void);				// UART communication stop function
GLOBAL void Uart_err(void);					// UART ERROR function
GLOBAL void Uart_reset(BYTE);				// UART communication reset function
GLOBAL void Uart_err_timeout(void);			// UART timeout
GLOBAL void Uart_err_timeout_master(void);	// UART ERROR TIMEOUT(Master IC) function
GLOBAL void Uart_err_unsupportcmd(void);	// UART unsupported command



/***********************************************************************
	Global variable
***********************************************************************/
GLOBAL BOOL	buartstop;						// receive stop flag

/***********************************************************************
	Constant definitions
***********************************************************************/
// armod value
#define UART_IDLE			0x00			// State that waiting for STX code
#define	UART_LENGTH			0x01			// State that waiting for Length code
#define UART_COMCHK			0x02			// State that waiting for commnad code
#define UART_DATACHK		        0x03			// State that waiting for Data
#define UART_CRCCHK                     0x04
#define UART_END			0x06			// State that waiting for ETX code

// Control code
#define	STX					0x02
#define	ETX					0x03

// Command code
/***************************************************
 *           |                        |   LSB    | MSB  | CRC     *
 *  size(5)  | UART_CMD_CB     |   LSB    | MSB  | CRC    *
 *           |                        |   LSB    | MSB  | CRC    *
 *           |                        |   LSB    | MSB  | CRC    *
***************************************************/

#define	UART_CMD_CB		0x33			// Cell Balancing Command
/***************************************************
 *           |                        | UART_CAL_LVOLALL | CRC    *
 * size(4)   | UART_CMD_CALIB | UART_CAL_HVOLALL | CRC    *
 *           | UART_CMD_CALIB | UART_CAL_START    | CRC    *
 *           |                        | UART_CAL_STOP     | CRC    *
 ***************************************************/
#define  UART_CMD_CALIB     0x44           // Slave Calibration Command

/***************************************************
 *    Slave 에서 FET를 제어하는 경우 필요한 Command
 *  size(4)  | UART_CMD_FET | FET_C |  CRC      *
 *           |                     | FET_D | CRC    *
 ***************************************************/
#define	UART_CMD_FET		0x55			// Control C-FET and D-FET
/***************************************************
 *  size(5)  | UART_CMD_HT  |  HEAT1    | on-1,off-0 | CRC    *
 *           | UART_CMD_HT  |  HEAT2    | on-1,off-0 | CRC    *
 *           | UART_CMD_HT  |  HEAT3    | on-1,off-0 | CRC    *
 ***************************************************/
#define	UART_CMD_HT           0x66			// Control  HEAT SW
/***************************************************
 *  size(4)  | UART_CMD_OPM | 0    | CRC    *
 ***************************************************/
#define	UART_CMD_OPM		0x99			// Slave IC mode number
/***************************************************
 * size(4)    | UART_CMD_MC | UART_MC_VTMES | CRC    *
 ***************************************************/
#define	UART_CMD_MC		0xAA			// Measure voltage and temperature or Enter power down state
/***************************************************
 * size(4)    | UART_CMD_PDN|  0    | CRC    *
 ***************************************************/
#define  UART_CMD_PDN       0xBB    // Command PwrDown 
#define  UART_TEST_MOD      0x77

// Data related Command code
#define	UART_MC_VTMES			0x11			 // Measure voltage and temperature
#define	UART_MC_PD				0x12			 // Enter power down state
#define	UART_OPM_DATA			0x00			 // Data of Operation mode check
#define  UART_CAL_START                  0x21            // Slave Calibration Start
#define  UART_CAL_STOP                   0x22            // Slave Clabiration Stop
#define  UART_CAL_LVOLALL              0x23        // Calibration low voltage  All 실행
#define  UART_CAL_HVOLALL             0x24        // Calibration High voltage All 실행

// Judgment Code
// ACK PACKET
/***************************************************
 * size(4)    |  ACK DATA  |  0    | CRC    *
 ***************************************************/
#define	UART_COM_SUCCESS	0x00			// Between Master IC and Slave IC communication succeeds
#define	UART_UNSPT_CMD		0x01			// Master IC sends wrong command to Slave IC
#define	UART_RCVTO			0x02			// Slave IC occurs UART communication timeout
#define	UART_HW_ERR			0x03			// Some UART H/W error occurs 
#define  TEST_MODE_SUCESS        0x04
 #define  UART_SLV_ASYNCSND     0xFF

// Data related Judgment Code
#define	UART_RCVTO_DATA		0x00			// Data of Slave IC occurs UART communication timeout
#define	UART_HW_ERR_OVR		0x01			// Over-run Error
#define	UART_HW_ERR_PRT		0x02			// Parity Error
#define	UART_HW_ERR_FRM		0x04			// Framing Error

// UART SPS register
#define UART_SPS0_9600          0x0022
#define UART_SPS0_19200         0x0011
#define UART_SPS0_38400         0x0000
#define UART_SPS0_115200        0x0000

// UART serial data register (SDR)
#define	UART_BPS_9600		0xCE00			//  9600bps  = 0xCE00(103D)
#define	UART_BPS_19200		0xCE00			//  9600bps  = 0xCE00(103D)
#define  UART_BPS_38400      0xCE00
#define  UART_BPS_115200     0x4400

#define UARTRST_NOERR     0
#define UARTRST_SLVTIMEOUT   1
#define UARTRST_DATARCVERR  2
#define UARTRST_HWERR   3

// Slave IC mode  
#define	SLV_NORMAL			0x00			// Normal (Bootint state)
#define SLV_FWUPGRADE                   0x11      // Firmware Upgrade
#define	SLV_CALIB			0x20			// Calibration
#define	SLV_FIXEDERR		        0x99			// Fixed data Error

#define COMBUF_MAX	8						// Maximum Communication receive buffer
GLOBAL BYTE	acombuf[COMBUF_MAX];			// Communication receive buffer
GLOBAL BYTE		aslvmode;
GLOBAL BOOL		buarttout_req;				// UART timeout measurement request
GLOBAL WORD		auarttout;					// Timeout counter
GLOBAL BYTE		arcmd;						// Received command
GLOBAL BYTE		armod;						// Communication mode
GLOBAL BYTE		uartdata_cnt;						// data counter

GLOBAL WORD		auartrcvtout;				// UART communication timeout
GLOBAL BYTE     bspRcvData;                 // when  connect to HAL, use this buffer 

#undef		GLOBAL


#endif


/*******************************************************************************
* Copyright(C) 2008, 2017 Renesas Electronics Corporation
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
* File Name		: smbus.c
* Contents		: SMBus functions
* Device		: RAJ240090/100
* Compiler		: CC-RL (V.4.01)
* Note			: 
*""FILE COMMENT END""**********************************************************/
#define _SMBUS

/***********************************************************************
** Include files
***********************************************************************/
#include "define.h"														// union/define definition
#include "ram.h"														// ram.c
#include "smbus.h"														// smbus.c
#include "smbus_ex.h"													// smbus_ex.c
#include "mode.h"														// mode.c
#include "cell.h"

#pragma interrupt _int_SMBus(vect=INTIICA0)		
#pragma interrupt _int_SCLSDA(vect=INTP4)


// - Internal function prototype -
static void ComChk(void);												// Command judgment
static void SAd_Chk(void);												// SlaveAddress check
static void R_W_Chk(void);												// Read/Write judgment
static void ReadRs_SAd(void);											// Read com SlaveAddress judgment
static void Read_Com(void);												// ReadWord/Block data send
static void Write_Com(void);											// WriteWord/Block data receive
static void Write_Blk_Chk(void);										// BlockWrite data number check
static void ComErr(void);												// Error function
static void ComErrNak(void);											// Error function with NAK
static void AckSend(void);													// ACK send function



// - Internal variable -
void (*REM_MOD_TBL[])(void) = {
																		// Slave func. function table
	SAd_Chk,															// SlaveAddress check
	ComChk,																// Command judgment
	R_W_Chk,															// Read/Write judgment
	ReadRs_SAd,															// ReadWord SlaveAddress judgment
	Read_Com,															// Read com data send
	Write_Com,															// Write com data receive
	ReadRs_SAd,															// BlockRead SlaveAddress judgment
	Write_Blk_Chk,														// BlockWrite data number check
	ComErr																// Error function
	};


static volatile BYTE	apec;											// PEC

// - Define definition -

// - CRC8 table -
const unsigned char CRC8TBL[] = {
	0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,
	0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,
	0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,
	0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,
	0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,
	0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,
	0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,
	0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,
	0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,
	0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,
	0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,
	0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,
	0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,
	0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,
	0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,
	0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,
	0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,
	0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,
	0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,
	0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,
	0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,
	0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,
	0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,
	0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,
	0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,
	0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63,
	0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,
	0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,
	0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,
	0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83,
	0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,
	0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3
};

#define CRC8_Calc(a)	apec = CRC8TBL[a^apec]							// PEC calculation macro


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: IIC interrupt function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: int SMB_INT()
 *--------------------------------------------------------------------------
 * Function			: IIC Bus interrupt function.
 *					: Function of slave function.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: StoreRcvData, SAd_Chk
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void _int_SMBus(void)
{
	WUP0 = 0;															// Wakeup disable
	NOP();																// wait 5clock
	NOP();
	NOP();
	NOP();
	NOP();

	if( SPD0 == 1														// Stop condition ?
		|| (STD0 == 1
			&& (ssmbdata.arcv_mod == R_WORD || ssmbdata.arcv_mod == W_WORD)) )
	{
		if( aiic_sadr == ADDR_ERR)										// Slave address mismatch ?
		{
			if( aerr_cnt < NO_RESET_CNT )								// Only slave address check ?
			{
				f_slvto_req = OFF;										// Clear slave timeout check req.
				f_sclto_req = OFF;										// Clear SCL timeout check req.
				anosmb_cnt = 0;											// Clear no comm time counter
				f_nosmb = OFF;											// Clear no comm flag
				f_wwpec = OFF;											// Clear PEC sent flag
				SMB_Timeout();											// SMBus timeout
				return;
			}
		}
		aiic_sadr = ADDR_ERR;											// Set slave address initial value
		aerr_cnt = 0;													// Clear SMBus error check counter

		if( f_slvto_req == ON )											// Start condition is received ?
		{
			WTIM0 = 0;													// 8bit communication
			if( f_comerr == OFF )										// No communication error ?
			{
				if( f_recv == ON )										// Data received ?
				{
					StoreRcvData();										// Store received data function
				}
			}

			ssmbdata.arcv_mod = SADCHK;									// Receive mode: Slave adr check
			f_recv = OFF;												// Clear received flag
			f_comerr = OFF;												// Clear comm. error flag
			f_wwpec = OFF;												// Clear PEC sent flag

			if( (COI0 == ON || EXC0 == ON)								// スレーブアドレス一致 ?
				&& ACKD0 == ON
				&& CLD0 == LOW )
			{
				IICAIF0 = 0;											// 割り込み要求クリア
				SAd_Chk();												// Slave address受信処理

			} else {
				
				if(SPD0 == 1)
				{
				TT0L_bit.no1 = 1;												// Stop timer TM01(1msec timer)
				ssmbdata.acrem = 0xFF;									// Init number of remain data
				f_slvto_req = OFF;										// Clear slave timeout check req.
				PIF4 = 0;												// Interrupt flag clear
				PMK4 = 0;												// SCL/SDA interrupt enable
				anosmb_cnt = 0;											// Clear no comm time counter
				f_nosmb = OFF;											// Clear no comm flag
				f_sclto_req = OFF;										// Clear SCL timeout check req.
				}
			}
		} else {														// Start condition is not receive
			ssmbdata.arcv_mod = SADCHK;									// Receive mode: Slave adr check
			if( (COI0 == ON || EXC0 == ON)								// スレーブアドレス一致 ?
				&& ACKD0 == ON
				&& CLD0 == LOW )
			{
				IICAIF0 = 0;											// 割り込み要求クリア
				SAd_Chk();												// Slave address受信処理
			} else {
				PIF4 = 0;												// Interrupt flag clear
				PMK4 = 0;												// SCL/SDA interrupt enable
			}
		}
	} else {															// Not stop condition

																		// 通信モードに対応した処理へ
																		// Jump to function of Receive mode
			REM_MOD_TBL[ssmbdata.arcv_mod]();
			anosmb_cnt = 0;												// Clear no comm time counter
			f_nosmb = OFF;												// Clear no comm flag
			f_sclto_req = OFF;											// Clear SCL timeout check req.
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Slave address receive function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void SAd_Chk()
 *--------------------------------------------------------------------------
 * Function			: Function of receiving slave address after start
*					: condition.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: CRC8_Calc, ComErrNak
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void SAd_Chk(void)
{
	BYTE	aiic_dat;													// Receive data

	aiic_dat = IICA0;													// Store received data
	aiic_sadr = aiic_dat;												// Set slave address
	if( COI0 == ON )													// Slave address match ?
	{
		PMK4 = 1;														// SCL/SDA interrupt disable
		f_fcslp = OFF;													// Clear force sleep flag
		f_slow = OFF;													// Clear slow flag
																		// No general call
																		// & R/W bit is Write ?
		if( EXC0 == 0 && TRC0 == 0 )
		{
			apec = 0;													// Initialize PEC
			CRC8_Calc(aiic_dat);										// Calculate PEC
			ssmbdata.arcv_mod = COMCHK;									// Receive mode: Command check
			WTIM0 = 0;													// 8bit communication
			WREL0 = 1;													// wait release
		} else {
			ComErrNak();												// Error with Nak
		}
	} else {															// Slave address mismatch
		ComErrNak();													// Error with Nak
	}

	TT0L_bit.no1 = 1;															// Stop timer TM01(1msec timer)
	while( TE0L_bit.no1 == ON ) ;												// Wait until timer stop
	TS0L_bit.no1 = 1;															// Start timer TM01(1msec timer)
	aslvto_cnt = 0;														// Clear slave timeout counter
	f_slv_to = OFF;														// Clear slave timeout flag
	f_slvto_req = ON;													// Set slave timeout check req.
	f_sclto_req = OFF;													// Clear SCL timeout check req.
	asclto_cnt = 0;														// Clear SCL timeout counter
	f_scl_to = OFF;														// Clear SCL timeout flag
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Command check function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void ComChk()
 *--------------------------------------------------------------------------
 * Function			: Function of receiving command.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: CRC8_Calc, ComExist_Chk, ComErrNak
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void ComChk(void)
{
	if( EXC0 == 0 )														// 
	{																	// No general call ?
		arcv_cmd = IICA0;												// Store received data
		CRC8_Calc(arcv_cmd);											// Calculate PEC
		if( ComExist_Chk() )											// Valid command ?
		{
			AckSend();													// Send ACK
			
		} else {														// Undefine command
			ComErrNak();												// Error with Nak
		}
	} else {
		ComErrNak();													// Error with Nak
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Read/Write judgment function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void R_W_Chk()
 *--------------------------------------------------------------------------
 * Function			: Judge the communication is Read or Write when
 *					: receiving the command that support both Read
 *					: and Write
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: CRC8_Calc, SetSMBusRead, AckSendR, AckSend, ComErrNak
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void R_W_Chk(void)
{
	BYTE	aiic_dat;													// Receive data

	aiic_dat = IICA0;													// Store received data
	CRC8_Calc(aiic_dat);												// Calculate PEC
	if( TRC0 == 1 )														// Rd?
	{
		if( COI0 == 1 && EXC0 == 0 )									// No general call ?
		{																// & Slave address match ?
			SetSMBusRead();												// Set Read data to SMBus send buf.
																		// Calculate PEC
			CRC8_Calc(asmbuf[ssmbdata.acrem]);
			WTIM0 = 1;													// 9bit communication
			IICA0 = asmbuf[ssmbdata.acrem];								// Send data

			ssmbdata.arcv_mod = R_WORD;									// Receive mode: Send Read data
		} else {
			ComErrNak();												// Error with Nak
		}
	} else {															// Slave address mismatch
		if( ssmbdata.acrem != 0x01 )									// BlockWrite communication ?
		{																// ☆データ数1のBlockWriteがある場合は方法を別途検討する!
																		// It needs to use other way if there is BlockWrite 
																		// with 1byte data.
			ssmbdata.acrem = IICA0;										// Store as number of remain data
			arem_cc = ssmbdata.acrem;									// Update number of com. data
			arem_cc--;
		} else {														// WriteWord communication
			asmbuf[ssmbdata.acrem] = IICA0;								// Store received data
		}
		ssmbdata.acrem--;												// Decrement number of remain data
		ssmbdata.arcv_mod = W_WORD;										// Receive mode: Receive write data
		AckSend();														// Send ACK
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Re-start condition check function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void ReadRs_SAd()
 *--------------------------------------------------------------------------
 * Function			: Function of receiving slave address after
*					: re-start condition.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: CRC8_Calc, SetSMBusRead, AckSendR, ComErrNak
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void ReadRs_SAd(void)
{
	BYTE	aiic_dat;													// Receive data

	aiic_dat = IICA0;													// Store received data
	CRC8_Calc(aiic_dat);												// Calculate PEC
																		// Slave address match ?
																		//	& No general call ?
																		//	& Rd ?
	if( COI0==1 && EXC0==0 && TRC0==1 )
	{
		SetSMBusRead();													// Set Read data to SMBus send buf.
		CRC8_Calc(asmbuf[ssmbdata.acrem]);								// Calculate PEC
		WTIM0 = 1;														// 9bit communication
		IICA0 = asmbuf[ssmbdata.acrem];									// Send data
		ssmbdata.arcv_mod = R_WORD;										// Receive mode: Send read data
	} else {
		ComErrNak();													// Error with Nak
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: ReadWord / ReadBlock data send function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Read_Com()
 *--------------------------------------------------------------------------
 * Function			: Send ReadWord / ReadBlock data.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: CRC8_Calc, ComErr
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void Read_Com(void)
{
	if( EXC0 == 0 )														// No general call ?
	{
		if( ssmbdata.acrem != 0xFF )									// Not complete to send yet ?
		{
			ssmbdata.acrem--;											// Decrement number of remain data
			if( ssmbdata.acrem == 0xFF )								// Send complete ?
			{
				if( ACKD0 == 0 )										// NAK ?
				{
					WTIM0 = 0;											// 8bit communication
					ACKE0 = 0;											// ACK disable
					WREL0 = 1;											// wait release

				} else {												// ACK
					IICA0 = apec;										// Send PEC

				}
			} else {													// Not complete to send yet
				if( ACKD0 == 1 )										// ACK = LOW ?
				{
																		// Calculate PEC
					CRC8_Calc(asmbuf[ssmbdata.acrem]);
																		// Output data
					IICA0 = asmbuf[ssmbdata.acrem];
				} else {												// NAK
					ComErr();											// Error function
				}
			}
		} else {														// Already completed to send
			if( ACKD0 == 0 )											// NAK ?
			{
				WTIM0 = 0;												// 8bit communication
				ACKE0 = 0;												// ACK disable
				WREL0 = 1;												// wait release
			} else {
				ComErr();												// Error function
			}
		}
	} else {
		ComErr();														// Error function
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Function of communication error
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void ComErr()
 *--------------------------------------------------------------------------
 * Function			: Function of when communication error is occurred.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void ComErr(void)
{
	ssmbdata.arcv_mod = COMDAT_ERR;										// Receive mode: Error
	f_comerr = ON;														// Set communication error flag
	LREL0 = 1;															// Exit from communications
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Function of communication error with NAK
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void ComErrNak()
 *--------------------------------------------------------------------------
 * Function			: Function of when communication error with NAK is occurred.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void ComErrNak(void)
{
	ACKE0 = 0;															// NACK
	WREL0 = 1;															// wait release(送信)

	ssmbdata.arcv_mod = COMDAT_ERR;										// Receive mode: Error
	f_comerr = ON;														// Set communication error flag
}


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: WriteWord / BlockWrite data receive function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Write_Com()
 *--------------------------------------------------------------------------
 * Function			: Function of receiving WriteWord / BlockWrite data.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: CRC8_Calc, AckSend, ComErrNak, ComErr
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void Write_Com(void)
{
	if( EXC0 == 0 )														// No general call ?
	{																	// 
		if( ssmbdata.acrem != 0xFF )									// Not complete to receive yet ?
		{
			asmbuf[ssmbdata.acrem] = IICA0;								// Receive data
																		// Calculate PEC
			CRC8_Calc(asmbuf[ssmbdata.acrem]);
			ssmbdata.acrem--;											// Decrement number of remain data
			if( ssmbdata.acrem == 0xFF )								// Complete to receive ?
			{
				f_recv = ON;											// Set received flag
			}
			AckSend();													// Send ACK
		} else {														// Already completed to receive
																		// $FFで来ると言うことはPECである
																		// It means PEC is received.
			if( f_wwpec == OFF )										// PEC is not send yet ?
			{
				if( apec == IICA0 )										// PEC is correct ?
				{
					AckSend();											// Send ACK

				} else {												// PEC error
					f_comerr = ON;										// Set communication error flag
					ACKE0 = 0;											// NACK
					WREL0 = 1;											// wait release(送信)
																		// Send NAK
				}
				f_wwpec = ON;											// Set PEC sent flag
			} else {													// Already PEC is sent
				ComErrNak();											// Error with Nak
			}
		}
	} else {
		f_recv = OFF;													// Clear received flag
		ComErr();														// Error function
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: BlockWrite data length check function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Write_Blk_Chk()
 *--------------------------------------------------------------------------
 * Function			: Check data length of BlockWrite data.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: CRC8_Calc, ComErr, AckSend
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: This function is not used in the sample code
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void Write_Blk_Chk(void)
{
	BYTE	aiic_dat;													// Receive data

																		// Slave address mismatch ?
	if( TRC0 == 0 )														// Wr?
	{
		aiic_dat = IICA0;												// Store received data
		if( ssmbdata.acrem == aiic_dat )								// Data length is same as spec ?
		{
			CRC8_Calc(aiic_dat);										// Calculate PEC
			ssmbdata.acrem--;											// Decrement number of remain data
			arem_cc = ssmbdata.acrem;									// Set comm data length
			ssmbdata.arcv_mod = W_WORD;									// Receive mode: Receive write data
			AckSend();													// Send ACK
		} else {														// data length mismatch
			ComErrNak();												// Error with Nak
		}
	} else {
		ComErrNak();													// Error with Nak
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: ACK send function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void AckSend()
 *--------------------------------------------------------------------------
 * Function			: Send ACK.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void AckSend(void)
{
	ACKE0 = 1;															// ACK
	WREL0 = 1;															// wait release(送信)
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Timeout function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: BYTE SMB_Timeout()
 *--------------------------------------------------------------------------
 * Function			: Function of SMBUs timeout.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void SMB_Timeout(void)
{
	ssmbdata.arcv_mod = SADCHK;											// Receive mode: Slave adr check
	ssmbdata.acrem = 0xFF;												// Init number of remain data
	f_recv = OFF;														// Clear received flag
	f_comerr = OFF;														// Clear comm. error flag

	TT0L_bit.no1 = 1;															// Stop timer TM01(1msec timer)
	PM6 |= 0x03;
	SMBus_init();														// SMBus initialize
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Timeout check function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: BYTE Timeout_Chk()
 *--------------------------------------------------------------------------
 * Function			: Check SMBus timeout.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void Timeout_Chk(void)
{
	if( f_slv_to == ON )												// Slave timeout happened ?
	{
	f_slv_to = OFF;														// Clear slave timeout flag
	f_slvto_req = OFF;													// Clear slave  timeout check req.
	} else {
		if( f_scl_to == ON )											// SCL timeout happened ?
		{
			f_scl_to = OFF;												// Clear SCL timeout flag
			f_sclto_req = OFF;											// Clear SCL timeout check req.
		} else {														// No timeout
			return;
		}
	}
	DI();
	SMB_Timeout();														// Timeout function
	EI();
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: SCL/SDA interrupt function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void _int_SCLSDA(void)
 *--------------------------------------------------------------------------
 * Function			: Function of SCL/SDA interrupt.
*					: Start to check SMBus low hold.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void _int_SCLSDA(void)
{
	WUP0 = 0;															// Wakeup disable
	f_fcslp = OFF;														// Clear force sleep flag
	TT0L_bit.no1 = 1;															// Stop timer TM01(1msec timer)
	while( TE0L_bit.no1 == ON ) ;												// Wait until timer stop
	TS0L_bit.no1 = 1;															// Start timer TM01(1msec timer)
	aoffs_cnt = 0;														// Clear OffState counter
	asclto_cnt = 0;														// Clear SCL timeout counter
	f_scl_to = OFF;														// Clear SCL timeout flag
	f_sclto_req = ON;													// Set SCL timeout check req.

	if( aerr_cnt < SDA_MAX_CNT )
	{
		aerr_cnt++;														// Increment SMBus error check counter
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: SMBus initialize function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void SMBus_init(void)
 *--------------------------------------------------------------------------
 * Function			: Function of SMBus initialization.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *-------------------------------------------------------------------------- 
 *""FUNC COMMENT END""*****************************************************/
void SMBus_init(void)
{
	aiic_sadr = ADDR_ERR;												// Set slave address initial value
	aerr_cnt = 0;														// Clear SMBus error check counter

	IICA0EN = 1;														// SMBus serial I/F SFR read/Write enabled
	IICE0 = 0;															// SMBus disable
	IICAMK0 = 1;														// SMBus interrupt disable
	IICAIF0 = 0;														// SMBus interrupt request flag clear
	IICAPR00 = 0;														// SMBus interrupt Priority High
	IICAPR10 = 0;
	P6 &= 0xFC;
	PM6 |= 0x03;
	SMC0 = 0;															// Normal mode:100Kbps
	IICWL0 = 0x26;														// IICA LowLevel width : 4.7[uS]*8[MHz]=37.6-> 38=0x26
	IICWH0 = 0x2B;														// IICA HighLevelwidth : 5.3[uS]*8[MHz]=42.4-> 43=0x2B
	IICCTL01 = 0x00;													// Address match wake -up function disable in the STOP mode state / fCLK
	if(SMB_addr != 0xFF)
	{
		SVA0 = SMB_addr;													// SMBus slave address
	} else {
		SVA0 = 0x16;
	}
	IICCTL00 = 0x14;													// SMbus disable, interrupt enabled for stop condition,ACK enabled
	STCEN0 = 0;
	IICRSV0 = 1;
	IICAIF0 = 0;														// SMBus interrupt request flag clear
	IICAMK0 = 0;														// SMBus interrupt enabled
	IICE0 = 1;															// SMBus enabled
	LREL0 = 1;															// Exit from communications
	PM6 &= 0xFC;

	//	SCL/SDA interrupt
	EGP0_bit.no4 = 0;															// fall edge
	EGN0_bit.no4 = 1;
	PPR04 = 0;															// Interrupt top priority
	PPR14 = 0;
	PIF4 = 0;															// Interrupt flag clear
	PMK4 = 0;															// Interrupt enable
}




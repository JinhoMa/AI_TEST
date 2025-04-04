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
* File Name		: flashrom.c
* Contents		: flash memory control functions source file
* Device		: RAJ240075
* Compiler		: CC-RL
* Note			: Use "Flash Library Type04"
*""FILE COMMENT END""**********************************************************/

#define _FLASHROM





/***********************************************************************
** Include files
***********************************************************************/
#include "define.h"								// union/define definition
#include "flashrom.h"							// FlashROM data definition
#include "pfdl.h"							// for Flash Data Library Type04 at UIF
#include "pfdl_types.h"							// for Flash Data Library Type04
#include "dataflash.h"
#include "iodefine.h"
#include "fsl.h"

pfdl_request_t		st_req;

//	Initialize data for PFDL
pfdl_descriptor_t	pfdl_descriptor_pstr = { 0x08, 0x01 };	// fCLK / Width voltage mode

#if 0
//	Initialize data for FSL
__far fsl_descriptor_t fsl_descriptor_pstr
				= { 0x01, DFLASH_VALUE, 0x01 };
				//    |          |        +---- Status check internal mode
				//    |          +------------- Clock(Selected by compile option)
				//    +------------------------ Wide voltage mode
#endif
				
				
/*""FUNC COMMENT""**********************************************************
 * ID			: 1.0
 * module outline	: FlashROM Data Write Process
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: void FlashWrite(BYTE *apadr, BYTE *apdata, BYTE anum)
 *--------------------------------------------------------------------------
 * Function		: Data is written into the designated sector in FlashROM.
 *			: 
 *--------------------------------------------------------------------------
 * Argument		: BYTE *apadr	: Pointer of write address
 *			: BYTE *apdata	: Data line to be written
 *			: BYTE abytecnt : Byte count of write data
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
int flash_ret;
                
void FlashEraseFixedBlock(void)
{
    pfdl_status_t           aret;                                   // Return code
    //pfdl_request_t          st_req;
    PFDL_Open( &pfdl_descriptor_pstr );                             // Flash data library open

    st_req.index_u16 = (pfdl_u16)3;                                 // Set block number of erase
    st_req.command_enu = PFDL_CMD_ERASE_BLOCK;                      // Erase command
    aret = PFDL_Execute( &st_req );                                 // Erase execute
    while( aret == PFDL_BUSY )                                      // Erase ?
    {
        aret = PFDL_Handler();                                      // Ststus check
    }
    PFDL_Close();                                                   // Flash data library close
}
                
 void FlashWrite(BYTE *apadr, BYTE *apdata, WORD abytecnt)
{
	pfdl_status_t			aret;						// Return code
	//pfdl_request_t			st_req;

	PFDL_Open( &pfdl_descriptor_pstr );						// Flash data library open

	st_req.index_u16 = (pfdl_u16)apadr;						// Set top address of write data
	st_req.data_pu08 = apdata;							// Set address of source data
	st_req.bytecount_u16 = abytecnt;						// Set wite byte count

	/* Blank check */
	st_req.command_enu = PFDL_CMD_BLANKCHECK_BYTES;					// Blank check command
	aret = PFDL_Execute( &st_req );							// Blank check execute
	while( aret == PFDL_BUSY )							// Blank checking ?
	{
	aret = PFDL_Handler();								// Ststus check
	}

	if( aret == PFDL_OK )								// Blank check OK?
	{
		/* Write */
		st_req.command_enu = PFDL_CMD_WRITE_BYTES;				// Data write command
		aret = PFDL_Execute( &st_req );						// Write execute
		while( aret == PFDL_BUSY )						// Writing ?
		{
				aret = PFDL_Handler();					// Status check
		}

		if( aret == PFDL_OK )							// Write OK?
		{
			/* Verify */
			st_req.command_enu = PFDL_CMD_IVERIFY_BYTES;			// Verify command
			aret = PFDL_Execute( &st_req );					// Verify execute
			while( aret == PFDL_BUSY )					// Verifing ?
			{
				aret = PFDL_Handler();					// Status check
			}
		}
        flash_ret = 1;
	}else{
        flash_ret = 0;
    }

	PFDL_Close();									// Flash data library close
}


/*""FUNC COMMENT""**********************************************************
 * ID			: 1.0
 * module outline	: FlashROM Data Erase Process
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: void FlashErase(void)
 *--------------------------------------------------------------------------
 * Function		: Data is erased into the designated sector in DataFlash.
 *			: 
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
void FlashErase(void)
{
	pfdl_status_t			aret;						// Return code
	//pfdl_request_t			st_req;

	PFDL_Open( &pfdl_descriptor_pstr );						// Flash data library open

	st_req.index_u16 = (pfdl_u16)0;							// Set block number of erase
	st_req.command_enu = PFDL_CMD_ERASE_BLOCK;					// Erase command
	aret = PFDL_Execute( &st_req );							// Erase execute
	while( aret == PFDL_BUSY )							// Erase ?
	{
		aret = PFDL_Handler();							// Ststus check
	}
	st_req.index_u16 = (pfdl_u16)1;							// Set block number of erase
	st_req.command_enu = PFDL_CMD_ERASE_BLOCK;					// Erase command
	aret = PFDL_Execute( &st_req );							// Erase execute
	while( aret == PFDL_BUSY )							// Erase ?
	{
		aret = PFDL_Handler();							// Ststus check
	}
	st_req.index_u16 = (pfdl_u16)2;							// Set block number of erase
	st_req.command_enu = PFDL_CMD_ERASE_BLOCK;					// Erase command
	aret = PFDL_Execute( &st_req );							// Erase execute
	while( aret == PFDL_BUSY )							// Erase ?
	{
		aret = PFDL_Handler();							// Ststus check
	}
	st_req.index_u16 = (pfdl_u16)3;							// Set block number of erase
	st_req.command_enu = PFDL_CMD_ERASE_BLOCK;					// Erase command
	aret = PFDL_Execute( &st_req );							// Erase execute
	while( aret == PFDL_BUSY )							// Erase ?
	{
		aret = PFDL_Handler();							// Ststus check
	}

	PFDL_Close();									// Flash data library close
}
				
/*""FUNC COMMENT""**********************************************************
 * ID			: 1.0
 * module outline	: wait timer function
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: void Waittimer_us(BYTE ainval)
 *--------------------------------------------------------------------------
 * Function		: Measure 10us by using timer UNIT0 Channel3.
 *			: 
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
void Waittimer_us(BYTE ainval)
{
	/* Stop channel3 */
	TT0L_bit.no3 = 1;								// Stop TM03
	
	/* Mask channel3 interrupt */
	TMMK03 = 1;														// disable INTTM03 interrupt
	TMIF03 = 0;														// clear INTTM03 interrupt flag
	/* Set INTTM03 low priority */
	TMPR103 = 1;									// TM03 interrupt priority low
	TMPR003 = 1;
	/* Channel0 used as interval timer */
	TMR03 = 0x8000;									// Select CKm0
	TDR03 = ainval;									// 1MHz * ainval[us] = ainval[us]

	TS0L_bit.no3 = 1;								// Start TM03

	while( TMIF03 == 0 ) ;								// wait ainval[us]
	TMIF03 = 0;														// clear INTTM03 interrupt flag

	TT0L_bit.no3 = 1;								// Stop TM03
}

/*""FUNC COMMENT""**********************************************************
 * ID			: 1.0
 * module outline	: FlashROM Data Write Process
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: void FlashWrite(BYTE *apadr, BYTE *apdata, BYTE anum)
 *--------------------------------------------------------------------------
 * Function		: Data is written into the designated sector in FlashROM.
 *			: 
 *--------------------------------------------------------------------------
 * Argument		: BYTE *apadr	: Pointer of write address
 *			: BYTE *apdata	: Data line to be written
 *			: BYTE abytecnt : Byte count of write data
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
 * History		: 2016.02.15 Ver 1.00
 *			:  New Create
 *			: 
 *""FUNC COMMENT END""*****************************************************/
__callt BYTE DFlashWrite(__far BYTE *apadr, BYTE *apdata, WORD abytecnt)
{
	pfdl_status_t			aret;						// Return code
	pfdl_request_t			st_req;

	//DI();															// Disable interrupt

	PFDL_Open( &pfdl_descriptor_pstr );						// Flash data library open

	st_req.index_u16 = (pfdl_u16)((pfdl_u32)apadr);					// Set top address of write data
	st_req.data_pu08 = apdata;							// Set address of source data
	st_req.bytecount_u16 = abytecnt;						// Set wite byte count

	/* Blank check */
	st_req.command_enu = PFDL_CMD_BLANKCHECK_BYTES;					// Blank check command
	aret = PFDL_Execute( &st_req );							// Blank check execute
	while( aret == PFDL_BUSY )							// Blank checking ?
	{
	aret = PFDL_Handler();								// Ststus check
	}

	if( aret == PFDL_OK )								// Blank check OK?
	{
		/* Write */
		st_req.command_enu = PFDL_CMD_WRITE_BYTES;				// Data write command
		aret = PFDL_Execute( &st_req );						// Write execute
		while( aret == PFDL_BUSY )						// Writing ?
		{
				aret = PFDL_Handler();					// Status check
		}

		if( aret == PFDL_OK )							// Write OK?
		{
			/* Verify */
			st_req.command_enu = PFDL_CMD_IVERIFY_BYTES;			// Verify command
			aret = PFDL_Execute( &st_req );					// Verify execute
			while( aret == PFDL_BUSY )					// Verifing ?
			{
				aret = PFDL_Handler();					// Status check
			}
		}
	}

	PFDL_Close();									// Flash data library close
	
	return aret;
}

/*""FUNC COMMENT""**********************************************************
 * ID			: 1.0
 * module outline	: FlashROM Data Erase Process
 *--------------------------------------------------------------------------
 * Include		: 
 *--------------------------------------------------------------------------
 * Declaration		: void FlashErase(BYTE aerase_no)
 *--------------------------------------------------------------------------
 * Function		: Data is erased into the designated sector in FlashROM.
 *			: 
 *--------------------------------------------------------------------------
 * Argument		: BYTE aerase_no   : Block number of erase
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
 * History		: 2012.04.01 Ver 0.01
 *			:  New Create
 *			: 
 *""FUNC COMMENT END""*****************************************************/
void DFlashErase(BYTE aerase_no)
{
	//DI();										// Disable interrupt

	PFDL_Open( &pfdl_descriptor_pstr );						// Flash data library open

	st_req.index_u16 = (pfdl_u16)aerase_no;						// Set block number of erase
	st_req.command_enu = PFDL_CMD_ERASE_BLOCK;					// Erase command
	PFDL_Execute( &st_req );							// Erase execute
	
}

BYTE FlashErase_Chk(void)
{
	DFLEN = 1;									// Data flash access enable
	Waittimer_us(10);								// Data flash access wait
	if( PFDL_Handler() == PFDL_BUSY )						// Erasing ?
	{
		return (0);
	}

	PFDL_Close();									// Flash data library close

	return (1);									// Complete
}

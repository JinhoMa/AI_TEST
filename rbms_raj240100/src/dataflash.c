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
* File Name		: dataflashrom.c
* Contents		: Flash memory control functions
* Device		: RAJ240090/100
* Compiler		: CC-RL (V.4.01)
* Note			: 
*""FILE COMMENT END""**********************************************************/
#define _DATAFLASH


/***********************************************************************
** Include files
***********************************************************************/
#include "define.h"													// union/define definition
#include "dataflash.h"												// DataFlash data definition
#include "pfdl.h"													// for Flash Data Library Type04
#include "pfdl_types.h"												// for Flash Data Library Type04
#include "iodefine.h"
#include "ram.h"
#include "cell.h"
#include "flashrom.h"

//const pfdl_descriptor_t pfdl_descriptor_pstr = { 0x08, 0x00 };		// 8MHz / Full speed mode
extern pfdl_descriptor_t	pfdl_descriptor_pstr;	// 8MHz / Width voltage mode



void FlashEraseCalBlock(void)
{
	pfdl_status_t			aret;									// Return code
	pfdl_request_t			st_req;

	PFDL_Open( &pfdl_descriptor_pstr );								// Flash data library open

	st_req.index_u16 = (pfdl_u16)2;									// Set block number of erase
	st_req.command_enu = PFDL_CMD_ERASE_BLOCK;						// Erase command
	aret = PFDL_Execute( &st_req );									// Erase execute
	while( aret == PFDL_BUSY )										// Erase ?
	{
		aret = PFDL_Handler();										// Ststus check
	}
	PFDL_Close();													// Flash data library close
}


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: SUM Calculation
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: WORD Calc_SUM(DWORD tstart, DWORD tend)
 *--------------------------------------------------------------------------
 * Function			: Calc sum value of specified area
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: WORD tstart	: Start address
*					: WORD tend		: End Address
 *--------------------------------------------------------------------------
 * Return			: SUM
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
WORD Calc_SUM(DWORD nstart, DWORD nend)
{
	__far WORD	*lpt;
	WORD		lsum;

																	// SUM calculation loop
	lpt = (__far WORD *)nstart;
	lsum = 0;
	do
	{
		lsum += *lpt;
		lpt++;
	} while( (DWORD)lpt <= (DWORD)nend );

	return lsum;
}
/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Calculate checksum
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Calc_Checksum(void)
 *--------------------------------------------------------------------------
 * Function			: Calculate checksum of sample program and Fixed data.
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 * Output			: None
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 *""FUNC COMMENT END""*****************************************************/
#define MAIN1_START		0x00000
#define MAIN1_END		0x0D7FF
#define MAIN2_START		0x0D800
#define MAIN2_END		0x0DBFF
void Calc_Checksum(void)
{
	
												// Calc checksum of Main1
	lsum_main1 = Calc_SUM(MAIN1_START, MAIN1_END);

												// Calc checksum of Main2
	lsum_main2 = Calc_SUM(MAIN2_START, MAIN2_END);
}

#if 1
/*""FUNC COMMENT""***************************************************
* ID : 1.0
* module outline	: DataFlash BlockA erase function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration		: void DF_Erase_A(void)
*-------------------------------------------------------------------
* Function		: Erase Block A of DataFlash.
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function 	: 
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2012.08.01 Ver 0.01
* 			
*""FUNC COMMENT END""**********************************************/
void DF_Erase_A(void)
{
	f_dfe = ON;								// Set DataFlash erasing flag
//	DI();									// Disable interrupt
	DFlashErase(0);								// Erase data flash block0
//	EI();									// Enable interrupt
}

/*""FUNC COMMENT""***************************************************
* ID			: 1.0
* module outline	: DataFlash BlockB erase function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration		: void DF_Erase_B(void)
*-------------------------------------------------------------------
* Function		: Erase BlockB of DataFlash.
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function 	: 
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2012.08.01 Ver 0.01
* 			
*""FUNC COMMENT END""**********************************************/
void DF_Erase_B(void)
{
	f_dfe = ON;								// Set DataFlash erasing flag
//	DI();									// Disable interrupt
	DFlashErase(1);								// Erase data flash block1
//	EI();									// Enable interrupt
}

/*""FUNC COMMENT""***************************************************
* ID			: 1.0
* module outline	: DataFlash confirm to erase function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration		: void DF_Erase_Confirm(void)
*-------------------------------------------------------------------
* Function		: Confirm to complete erasing DataFlash.
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function 	: 
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2012.08.01 Ver 0.01
* 			
*""FUNC COMMENT END""**********************************************/
__callt void DF_Erase_Confirm(void)
{
	if( FlashErase_Chk() == 1 )
	{
		f_dfe = OFF;							// Clear DataFlash erasing flag
	}
}

#endif

/*""FUNC COMMENT""***************************************************
* ID			: 1.0
* module outline	: DataFlash read index search function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration		: void DFR_Index(void)
*-------------------------------------------------------------------
* Function		: Search latest data area index of DataFlash.
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: apdfadr	: Latest data address
*-------------------------------------------------------------------
* Used function 	: 
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2012.08.01 Ver 0.01
* 			
*""FUNC COMMENT END""**********************************************/
void DFR_Index(void)
{
	BYTE	aidx1=0xFF,aidx2=0xFF;
	BYTE	aidx_w;

	apdfadr = (__far BYTE*)BLOCK_B;						// Search from BlockB
	if( *apdfadr == 0xFF )							// BlockB is not used ?
	{
		apdfadr = (__far BYTE*)BLOCK_A;					// Search BlockA
		if( *apdfadr != 0xFF )						// BlockA is used ?
		{
										// Search blank index
			for( aidx_w=0; aidx_w<DFBLOCK && *apdfadr!=0xFF;aidx_w++,apdfadr++ )
			{
				if( *apdfadr == W_OK )				// Valid data ?
				{
					aidx1 = aidx_w;				// Set as DFindex
				}
			}
			if( aidx1 == 0xFF )					// Not exist valid data ?
			{
				apdfadr = (BYTE*)0xFFFF;			// Set address to 0xFFFF
			} else {						// Valid data exist
										// Calculate data address
				apdfadr = (__far BYTE*)(BLOCK_A_ST + ((WORD)aidx1 * DF_LEN));
			}
		} else {							// BlockA also not used
			apdfadr = (BYTE*)0xFFFF;				// Set address to 0xFFFF
		}
	} else {								// BlockB is used
										// Search blank index
		for( aidx_w=0; aidx_w<DFBLOCK && *apdfadr!=0xFF; aidx_w++,apdfadr++ )
		{
			if( *apdfadr == W_OK )					// Valid data ?
			{
				aidx2 = aidx_w;					// Set as DFindex
			}
		}
		if( *(__far BYTE*)BLOCK_A == 0xFF )				// BlockA is not used ?
		{
			if( aidx2 == 0xFF )					// Not exist valid data ?
			{
				apdfadr = (BYTE*)0xFFFF;			// Set address to 0xFFFF
			} else {						// Exist valid data
				apdfadr = (__far BYTE*)(BLOCK_B_ST + ((WORD)aidx2 * DF_LEN));
			}
		} else {							// BlockA also used
			apdfadr = (__far BYTE*)BLOCK_A;				// Search BlockA too
			for( aidx_w=0; aidx_w<DFBLOCK && *apdfadr!=0xFF;aidx_w++,apdfadr++ )
			{
				if( *apdfadr == W_OK )				// Valid data ?
				{
					aidx1 = aidx_w;				// Tentative save as DFindex
				}
			}
			if( aidx2 == 0xFF )					// Not exist valid data on BlockB ?
			{
				if( aidx1 == 0xFF )				// BlockA also not exist ?
				{
					apdfadr = (BYTE*)0xFFFF;		// Set address to 0xFFFF
				} else {					// BlockA has valid data
										// Calc data address by BlockA
					apdfadr = (__far BYTE*)(BLOCK_A_ST + ((WORD)aidx1 * DF_LEN));
				}
			} else {						// BlockB is valid
				if( aidx1 == 0xFF )				// Not exist valid data on BlockA ?
				{
										// Calc data address by BlockB
					apdfadr = (__far BYTE*)(BLOCK_B_ST + ((WORD)aidx2 * DF_LEN));
				} else {					// Both block are valid
					if( aidx1 < aidx2 )			// BlockA is less than B ?
					{
										// Use BlockA
						apdfadr = (__far BYTE*)(BLOCK_A_ST + ((WORD)aidx1 * DF_LEN));
					} else {				// Same or B is less than A
										// Use BlockB
						apdfadr = (__far BYTE*)(BLOCK_B_ST + ((WORD)aidx2 * DF_LEN));
					}
				}
			}
		}
	}
}

/*""FUNC COMMENT""***************************************************
* ID : 1.0
* module outline	: DataFlash read function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration		: void DF_Read(void)
*-------------------------------------------------------------------
* Function		: Read DataFlash
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function 	: DFR_Index()
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2012.01.08 Ver 0.01
* 			
*""FUNC COMMENT END""**********************************************/
WORD SOH;
void DF_Read(void)
{
    BYTE	al1;
    BYTE       ocv_st;
    DFLEN = 1;								// Data flash access enable
    Waittimer_us(10);							// Data flash access wait

    DFR_Index();								// Search read area
    //0xF1000 or 0xF1400
    if( apdfadr != (BYTE*)0xFFFF )						// DataFlash is not empty ?
    {
        // Data read loop
        for( al1=0; al1<DFSIZE; al1++,apdfadr++ )
        {
            adfdat[al1] = *apdfadr;
        }

        //t_com10c = (WORD)((long)t_com10p * 10000 / twh_w);
        // 100% - 40%*cyclecount / 250 cycle 
        SOH = 10000 - (long)4000*t_com17/250;
        t_com10c = (long)D_COM18C*SOH/10000;
        //hunate full cap은 db에서 읽어온다. 
        t_com10c_h = FCC_ADJ;
        if( t_com10c_h == 0 || t_com10c_h == 0xFFFF ){
            t_com10c_h = D_COM18C;
        }
        //t_com10c = (WORD)((long)t_com10p * 10000 / twh_w);
        //data에 잔존용량이없는경우 강제 OCV 로 설정 
        if( lrc_w == 0){
            OCV_ST = ON;
        }
        ocv_st = OCV_ST;
        // OCV State 는 Clear 하지 않는다. 
        t_com16 =0;  OCV_ST = ocv_st;
        tpacksts = 0;
    } else {								// DataFlash is empty
        // Data 가 Backup 이 안된경우.. 최초 일때
        // SOC 는 50%로 해서 용량을 계산 한다.  lrc_w = 57600000
        for( al1=0; al1<DFSIZE; al1++ )					// Set default data
        {
            adfdat[al1] = 0;
        }

        t_com10p = D_COM18P;						// FCC(10mWh) = DC
        t_com10c_h = t_com10c = D_COM18C;					// set initial FCC
        lrc_w = (long)t_com10c * INIT_RSOC * 144;
        lrc_w_hunate =  (long)t_com10c_h * INIT_RSOC * 1800;
        FCC_ADJ = t_com10c_h;
        OCV_ST = ON;  // SOC 를 초기화 한다. 
    }
    DFLEN = 0;								// Data flash access disable
}

void DF_Update_Set(void)
{
    f_dfreq =ON;
}

/*""FUNC COMMENT""***************************************************
* ID			: 1.0
* module outline	: DataFlash write function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration		: BYTE DF_Write(void)
*-------------------------------------------------------------------
* Function		: Write to DataFlash
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: TRUE, FALSE
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function		: DFW_Index(), DF_Erase_B(), DF_Erase_A()
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2012.08.01 Ver 0.01
* 			
*""FUNC COMMENT END""**********************************************/
__callt BYTE DF_Write(void)
{
	BYTE	al1;
	BYTE	*apwork1, *apwork2;
	BYTE	aret;

	if( f_dfe == ON )							// DataFlash Erasing ?
	{
		return FALSE;							// Abort
	}

	if( f_dfstop == ON )							// Stop update ?
	{
		return TRUE;							// return
	}
	DFW_Index();								// Search write area
	if( adfidx >= DFBLOCK )							// Index NG ?
	{
		return FALSE;							// Abort
	}

	DFLEN = 1;								// Data flash access enable
	Waittimer_us(10);
										// Make write address
	apwork1 = (BYTE*)(apdfadr + DF_ST_OFF + ((WORD)adfidx * DF_LEN));
	apwork2 = apwork1;
	for( al1=0; al1<DF_LEN; al1++ )						// Write area empty check
	{
		if( *apwork1 != 0xFF )						// Not empty ?
		{
			DF_IndexW(AREA_NG);					// Index NG
										// Write index(area NG)
			return FALSE;						// Abort
		}
		apwork1++;							// To next data
	}

										// Write data
	aret = DFlashWrite( apwork2, adfdat, DFSIZE );
	if( aret == PFDL_OK )							// OK ?
	{
		DF_IndexW(W_OK);						// Index OK
										// Write index(Write OK)
	} else {								// NG
		DF_IndexW(W_NG);						// Write index (Write NG)
	}

	if( f_dfe_breq == ON )							// BlockB Erase request exist ?
	{
		DF_Erase_B();							// BlockB erase
		f_dfe_breq = OFF;						// Clear BlockB erase request
	} else {
		if( f_dfe_areq == ON )						// BlockA Erase request exist ?
		{
			DF_Erase_A();						// BlockA erase
			f_dfe_areq = OFF;					// Clear BlockA erase request
		}
	}

	DFLEN = 0;								// Data flash access disable
	return TRUE;
}

/*""FUNC COMMENT""***************************************************
* ID : 1.0
* module outline	: DataFlash search writing index function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration		: void DFW_Index(void)
*-------------------------------------------------------------------
* Function		: It searches data area index to write next.
*-------------------------------------------------------------------
* Argument		: None
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* 			: apdfadr	: op address of block that data exist
*			: adfidx	: Index of data area to write next
*-------------------------------------------------------------------
* Used function 	: DF_Erase_A(), DF_Erase_B(), DF_Erase_Confirm()
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2012.08.01 Ver 0.01
* 			
*""FUNC COMMENT END""**********************************************/
void DFW_Index(void)
{
	BYTE	aidxA, aidxB;

	DFLEN = 1;								// Data flash access enable
	Waittimer_us(10);							// Data flash access wait

	apdfadr = (__far BYTE*)BLOCK_A;						// Search BlockA
	for( aidxA=0; aidxA<DFBLOCK && apdfadr[aidxA]!=0xFF; aidxA++ );
	apdfadr = (__far BYTE*)BLOCK_B;						// Search BlockB
	for( aidxB=0; aidxB<DFBLOCK && apdfadr[aidxB]!=0xFF; aidxB++ );
	if( aidxB == 0 )							// Block B is empty ?
	{
		if( aidxA >= DFBLOCK )						// Block A is full ?
		{
			apdfadr = (__far BYTE*)BLOCK_B;				// Use BlockB
			adfidx = 0;
		} else {							// Block A is not full
			apdfadr = (__far BYTE*)BLOCK_A;				// Use BlockA
			adfidx = aidxA;
		}
	} else {								// Block B is not empty
		if( aidxA == 0 )						// Block A is empty ?
		{
			if( aidxB >= DFBLOCK )					// Block B is full ?
			{
				apdfadr = (__far BYTE*)BLOCK_A;			// Use BlockA
				adfidx = 0;
			} else {						// Block B is not full
				apdfadr = (__far BYTE*)BLOCK_B;			// Use BlockB
				adfidx = aidxB;
			}
		} else {							// Block A & B are not empty
			if( aidxA < aidxB )					// Number of A less than B ?
			{
				apdfadr = (__far BYTE*)BLOCK_A;			// Use BlockA
				adfidx = aidxA;
				if( aidxA >= (DFBLOCK - 3) )			// Block A is last 3 area ?
				{
						f_dfe_breq = ON;		// Set BlockB erase request
										// DF erase times is not max ?
						if( tdferase_cnt != 65535 )
						{
							tdferase_cnt++;		// Increment DF erase times
						}
				}
			} else {						// B equals A or less
				apdfadr = (__far BYTE*)BLOCK_B;			// Use BlockB
				if( aidxB == DFBLOCK )				// Block B is full ?
				{						// (It means some error.)
						DF_Erase_B();			// Erase Block B
						while( f_dfe == ON )		// Wait for erase Block B
						{
							DF_Erase_Confirm();	// Confirm erase DataFlash
						}
						adfidx = 0;			// Index is first.
				} else {
					adfidx = aidxB;
					if( aidxB >= (DFBLOCK - 3) )		// Block B is last 3 area ?
					{
							f_dfe_areq = ON;	// Set BlockB erase request
					}
				}
			}
		}
	}

	DFLEN = 0;								// Data flash access disable
}

/*""FUNC COMMENT""***************************************************
* ID			: 1.0
* module outline	: DataFlash index writing function
*-------------------------------------------------------------------
* Include		: 
*-------------------------------------------------------------------
* Declaration		: void DF_IndexW(BYTE adata)
*-------------------------------------------------------------------
* Function		: Write index of DataFlash
*-------------------------------------------------------------------
* Argument		: BYTE adata : Write data
*-------------------------------------------------------------------
* Return		: None
*-------------------------------------------------------------------
* Input			: None
* Output		: None
*-------------------------------------------------------------------
* Used function 	: 
*-------------------------------------------------------------------
* Caution		: 
*-------------------------------------------------------------------
* History		: 2012.08.01 Ver 0.01
* 			
*""FUNC COMMENT END""**********************************************/
void DF_IndexW(BYTE adata)
{
	DFLEN = 1;								// Data flash access enable
	Waittimer_us(10);							// Data flash access wait
	
	if( apdfadr[adfidx] == 0xFF )						// index is empty ?
	{
//		DI();								// Disable interrupt
		DFlashWrite( &apdfadr[adfidx], &adata, 1 );
//		EI();								// Enable interrupt
	}
	DFLEN = 0;								// Data flash access disable
}



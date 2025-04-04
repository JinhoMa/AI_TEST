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
* File Name		: can.h
* Version		: 1.00
* Contents		: CAN header file
* Programmer	: Yasuo Usuda
* Note			: 
************************************************************************
* Copyright(C) 2016 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History			: 2016.02.21 Ver 1.00
 *					:  New Create
 *					: 
*""FILE COMMENT END""**************************************************/
#ifndef _CAN_H_
#define _CAN_H_


#ifdef	_CAN
#define			GLOBAL
#else
#define			GLOBAL	extern
#endif	/* _CAN */


#define  MASTER_ID         0x1
#define  MASK_MSTID      0x1FF   
#define  SLAVE_ID         0x2
#define  MASK_SLVID      0x2FF   

#define  DEVICEID           MASTER_ID
#define  MASK_DEVID     MASK_MSTID

#define	RCV_ID	    0x00aa									// CAN Receive ID
#define	SEND_ID	    0x0701									// Master BMIC CAN SEND ID


typedef enum {
	CAN_RESULT_OK,
	CAN_RESULT_ERROR,
	CAN_RESULT_BUSFOFF,
	CAN_RESULT_BUSY,
}	CAN_RESULT;

typedef enum {
	CAN_LENGTH_0,
	CAN_LENGTH_1,
	CAN_LENGTH_2,
	CAN_LENGTH_3,
	CAN_LENGTH_4,
	CAN_LENGTH_5,
	CAN_LENGTH_6,
	CAN_LENGTH_7,
	CAN_LENGTH_8,
	CAN_LENGTH_ERROR
}	CAN_LENGTH;

typedef enum {
	RETURN_OK,
	RW_ERROR,											// Read or Write command error
	DLC_ERROR,											// DLC error
	COM_ERROR											// Command code error
}	RETURN_CODE;

typedef enum{
    CAN_TX_NOMAILBOX,
    CAN_TX_MAILBOX1,
    CAN_TX_MAILBOX2,
    CAN_TX_MAILBOX3,
    CAN_TX_MAILBOX4
} CAN_TX_MAILBOX;


/* Function Prototype */

// BSP Function Prototype
GLOBAL void CAN_BspInit( void );

CAN_RESULT CAN_SendMsg(void);
CAN_LENGTH CAN_GetMsg(void );
RETURN_CODE cCheckParamDLC2( void );
RETURN_CODE cCheckParamDLC( void );
void StoreRcvData(void);								// processing saving CAN receive data
void SetCANRead(void);								// processing read data setting
CAN_RESULT CAN_Send(void);
void Can_SendResponse(BYTE , BYTE , BYTE );
void Can_SendRespAckDataBlock(void);
RETURN_CODE Can_ParseMsg(void);

extern DWORD fw_download_cnt;

/* Interrupt Handler */
void can_receive(void);
void can_gerror(void);
void can_send(void);
void can_cerror(void);

void __far  fwcan_receive(void);
#undef	GLOBAL

#endif /* _CAN_H_ */

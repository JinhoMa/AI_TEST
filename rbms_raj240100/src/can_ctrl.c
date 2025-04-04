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
* File Name		: can_ctrl.c
* Version		: 1.00
* Contents		: CAN extra functions
* Programmer	: Yasuo Usuda
* Note			: 
************************************************************************
* Copyright(C) 2016 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History			: 2016.02.21 Ver 1.00
 *					:  New Create
 *					: 
*""FILE COMMENT END""**************************************************/

/**************************************************************************************************
 *
 *              +----------+                             +---------+
 *              + host        +                             + bms       +
 *              +----+----+                              +----+----+
 *                      |                                             |   
 *                      |                                             |
 *                     +------ --- LOGOUT(ID=0x700)--->  | [ data[0]=ON,OFF]  , [ACK 없음]
 *                     +                                             |
 *                     +---------  CALREQ(ID=0x701)----->|  [data[0]=ON] , [ACK 있음]
 *                     |                                              |
 *   [data[0]='O'    | <-------- ACK(ID=0x600) -----  --+
 *   data[1]='K']     |---------> CALMODE(ID=0x0702)-->| [data[0]=id, data[1]=st]
 *
                     | <--------- ACK(ID=0x600)---------|
 *                     |---------- CALREQ(ID=0x702) ---->| [data[0]=OFF]
 *
                     | <--------- ACK(ID=0x600)---------| 이전상태가 LOG OUT 중이라면 다시 LOG OUT 실행한다.
 *
 *
 **/



#include "define.h"
#include "dataflash.h"
#include "ram.h"
#include "kousei.h"
#include "mcu.h"
#include "afe.h"
#include "cell.h"
#include "can.h"
#include "can_ex.h"
#include "main.h"
#include <stdint.h>


#if 0
#include "diag.h"
#include <string.h>
#include "mode.h"
#include "OEM_define.h"
/* External Function */
extern void fw_ChageMode(void);

/* Internal Function */
static void Write_CalRef(void);
static void Read_CalRef(void);

BYTE rspd1;     //Response data1
BYTE rspd2;     //Repsonse data2
BYTE fixedblock_access ;
BYTE calblock_access ;

static _SHORT2CHAR		tcom57;						// calibration reference value interface
#define t_com57	tcom57.us
#define a_com57	tcom57.uc

#define EVEN_MASK 0xAA
#define ODD_MASK 0x55


#pragma	inline_asm	SW_RESET


// Charging Voltage/Current
const __near WORD	tcv = 33600;
const __near WORD	tcc = 3000;
const __near BYTE	ads = 8;
void SW_RESET(void)
{
	.DB2 0x0FFFF;
}

typedef struct _DIAG {
    WORD cmd;
    void (*func)(BYTE , BYTE);
} DIAG;


const  DIAG  diag_table[]={
    { CHNG_TO_DIAG ,       dg_changeToDiagMode },
    { CHNG_TO_NORL,       dg_changeToNormalMode},      
    { CALIB_ON,              dg_calib_on      },
    { CALIB_OFF,              dg_calib_off      },
    {CALIB_MST_LCELLALL,   dg_calib_MstLCVolAll },
    {CALIB_MST_HCELLALL,  dg_clib_MstHCVolAll},
    {CALIB_SLV_LCELLALL,    dg_calib_SlvLCVolAll },
    {CALIB_SLV_HCELLALL,   dg_clib_SlvHCVolAll},
    { CALIB_L_CUR,           dg_calib_0ACurrent },
    { CALIB_H_CUR,          dg_calib_HiCurrent },
    { RD_L_CURREF,          dg_readLowCurRef },
    { RD_H_CURREF,         dg_readHighCurRef },
    { RD_L_CELLALLREF,     dg_readLowCVolAllRef },
    { RD_H_CELLALLREF,    dg_readHighCVolAllRef },
    { WR_H_CURREF,               dg_writeHighCurRef },
    { TEST_CB_ODDON,      dg_test_cb_oddon },
    { TEST_CB_EVENON,     dg_test_cb_evenon },
    { TEST_CB_OFF,           dg_test_cb_off },
    { TEST_CB_SEPON,           dg_test_cb_sepon },
    { TEST_HEAT_EN_ON,        dg_test_heat_en_on },
    { TEST_HEAT_EN_OFF,        dg_test_heat_en_off },
    { TEST_HEAT1_ON,        dg_test_heat1_on },
    { TEST_HEAT1_OFF,        dg_test_heat1_off },
    { TEST_HEAT2_ON,        dg_test_heat2_on },
    { TEST_HEAT2_OFF,        dg_test_heat2_off },
    { TEST_HEAT3_ON,        dg_test_heat3_on },
    { TEST_HEAT3_OFF,        dg_test_heat3_off },
    { TEST_DFET_ON,         dg_test_dfet_on },
    { TEST_DFET_OFF,        dg_test_dfet_off },
    { TEST_PFET_ON,         dg_test_pfet_on },
    { TEST_PFET_OFF,        dg_test_pfet_off },
    { TEST_CFET_ON,         dg_test_cfet_on },
    { TEST_CFET_OFF,        dg_test_cfet_off },
    { TEST_DFET_ON,         dg_test_dfet_on },
    { TEST_DFET_OFF,        dg_test_dfet_off },
    { TEST_PFET_ON,         dg_test_pfet_on },
    { TEST_PFET_OFF,        dg_test_pfet_off },
    { TEST_SLV_PWRDN,       dg_test_slave_pwrdn },
    { TEST_SLV_RESET_ON,        dg_test_slave_reset_on },
    { TEST_SLV_RESET_OFF,        dg_test_slave_reset_off },
    { DIAG_SW_RESET,        dg_sw_reset},
    { RD_BUILD_DATE,        dg_rd_builddate},
    { RD_BUILD_TIME,        dg_rd_buildtime},
};

void dg_rd_builddate(BYTE p1, BYTE p2)
{
    extern const _BUILD_INFO build_info;
    TxMsg.dlc = 8;
    TxMsg.data[0] = 'o';
    TxMsg.data[1] = 'k';
    TxMsg.data[2] = build_info.date[0];
    TxMsg.data[3] = build_info.date[1];
    TxMsg.data[4] = build_info.date[2];
    TxMsg.data[5] = build_info.date[3];
    TxMsg.data[6] = build_info.date[4];
    TxMsg.data[7] = build_info.date[5];
    Can_SendRespAckDataBlock();
}
void dg_rd_buildtime(BYTE p1, BYTE p2)
{
    extern const _BUILD_INFO build_info;
    TxMsg.dlc = 8;
    TxMsg.data[0] = 'o';
    TxMsg.data[1] = 'k';
    TxMsg.data[2] = build_info.time[0];
    TxMsg.data[3] = build_info.time[1];
    TxMsg.data[4] = build_info.time[2];
    TxMsg.data[5] = build_info.time[3];
    TxMsg.data[6] = build_info.time[4];
    TxMsg.data[7] = build_info.time[5];
    Can_SendRespAckDataBlock();
}


void dg_test_heat_en_on(BYTE p1, BYTE p2)
{
    IO_OEMHeatMainOn();
    f_heat_En = 1;
    Can_SendResponse(ACK,0,0);
}
void dg_test_heat_en_off(BYTE p1, BYTE p2)
{
    IO_OEMHeatMainOff();
    f_heat_En = 0;
    Can_SendResponse(ACK,0,0);
}

void dg_test_heat1_on(BYTE p1, BYTE p2)
{
    f_heat1= ON;
    acombuf[0] = 4;
    acombuf[1] = UART_CMD_HT;
    acombuf[2] = heat_sw;
    Uart_BspSend(0,acombuf,3);
    Can_SendResponse(ACK,0,0);
}
void dg_test_heat1_off(BYTE p1, BYTE p2)
{
    f_heat1= OFF;
    acombuf[0] = 4;
    acombuf[1] = UART_CMD_HT;
    acombuf[2] = heat_sw;
    Uart_BspSend(0,acombuf,3);
    Can_SendResponse(ACK,0,0);

}
void dg_test_heat2_on(BYTE p1, BYTE p2)
{
    f_heat2= ON;
    acombuf[0] = 4;
    acombuf[1] = UART_CMD_HT;
    acombuf[2] = heat_sw;
    Uart_BspSend(0,acombuf,3);
    Can_SendResponse(ACK,0,0);

}
void dg_test_heat2_off(BYTE p1, BYTE p2)
{
    f_heat2= OFF;
    acombuf[0] = 4;
    acombuf[1] = UART_CMD_HT;
    acombuf[2] = heat_sw;
    Uart_BspSend(0,acombuf,3);
    Can_SendResponse(ACK,0,0);

}
void dg_test_heat3_on(BYTE p1, BYTE p2)
{
    f_heat3= ON;
    acombuf[0] = 4;
    acombuf[1] = UART_CMD_HT;
    acombuf[2] = heat_sw;
    Uart_BspSend(0,acombuf,3);
    Can_SendResponse(ACK,0,0);

}
void dg_test_heat3_off(BYTE p1, BYTE p2)
{
    f_heat3= OFF;
    acombuf[0] = 4;
    acombuf[1] = UART_CMD_HT;
    acombuf[2] = heat_sw;
    Uart_BspSend(0,acombuf,3);
    Can_SendResponse(ACK,0,0);

}

void dg_test_cfet_on(BYTE p1, BYTE p2)
{
    IO_CFETOn();

    f_cfet = ON;                            // Set PackStatus:CFET
    Can_SendResponse(ACK,0,0);    
}
void dg_test_cfet_off(BYTE p1, BYTE p2)
{
    IO_CFETOff();
    f_cfet = OFF;                            // Set PackStatus:CFET
    Can_SendResponse(ACK,0,0);    
}

void dg_test_dfet_on(BYTE p1, BYTE p2)
{
    IO_DFETOn();
    f_dfet = ON;                            // Set PackStatus:CFET
    Can_SendResponse(ACK,0,0);    
}
void dg_test_dfet_off(BYTE p1, BYTE p2)
{
    IO_DFETOff();
    f_dfet = OFF;                            // Set PackStatus:CFET
    Can_SendResponse(ACK,0,0);    
}

void dg_test_pfet_on(BYTE p1, BYTE p2)
{
    IO_PFETOn();
    f_pfet = ON;                            // Set PackStatus:CFET
    Can_SendResponse(ACK,0,0);    
}
void dg_test_pfet_off(BYTE p1, BYTE p2)
{
    IO_PFETOff();
    f_pfet = OFF;                            // Set PackStatus:CFET
    Can_SendResponse(ACK,0,0);    
}

void dg_test_slave_pwrdn(BYTE p1, BYTE p2)
{
    IO_SlvWakeupOff();
    acombuf[0] = 4;
    acombuf[1] = UART_CMD_PDN;
    acombuf[2] = 0;
    Uart_BspSend(0,acombuf,3);
    Can_SendResponse(ACK,0,0);    
}
void dg_test_slave_reset_on(BYTE p1, BYTE p2)
{
    IO_SlvHwResetOn();
    Can_SendResponse(ACK,0,0);    
}

void dg_test_slave_reset_off(BYTE p1, BYTE p2)
{
    IO_SlvHwResetOff();
    Can_SendResponse(ACK,0,0);    
}


void dg_test_cb_oddon(BYTE p1, BYTE p2)
{
    BYTE bit;
    bit = ODD_MASK & 0x7F;
    AFE_WR(AFE_WINCHG,WINCHG_RM0);                              
    AFE_WR(AFE_COND0, bit);               // Cell Balancing (Cell 1 - Cell 8)
    AFE_WR(AFE_COND1, 0 );                          // Cell Balancing (Cell 9 - Cell 10)
    //
    acombuf[0] = 5;
    acombuf[1] = UART_CMD_CB;
    acombuf[2] = bit ;
    acombuf[3] = 0;
    Uart_BspSend(0,acombuf,4);
    f_SlvCB = ON; 
    tcom46.us[0] = (bit<<8)|bit ;
    Can_SendResponse(ACK,0,0);
}

void dg_test_cb_evenon(BYTE p1, BYTE p2)
{
    BYTE bit;
    bit = EVEN_MASK & 0x7F;
    AFE_WR(AFE_WINCHG,WINCHG_RM0);                              
    AFE_WR(AFE_COND0, bit);               // Cell Balancing (Cell 1 - Cell 8)
    AFE_WR(AFE_COND1, 0 );                          // Cell Balancing (Cell 9 - Cell 10)
    //
    acombuf[0] = 5;
    acombuf[1] = UART_CMD_CB;
    acombuf[2] = bit ;
    acombuf[3] = 0;
    Uart_BspSend(0,acombuf,4);
    tcom46.us[0] = (bit<<8)|bit ;
    f_SlvCB = ON; 
    Can_SendResponse(ACK,0,0);
}
void dg_test_cb_off(BYTE p1, BYTE p2)
{
    CB_OFF();
    tcom46.us[0] = 0;
    Can_SendResponse(ACK,0,0);
}

void dg_test_cb_sepon(BYTE p1, BYTE p2)
{
    BYTE mst_cb = RxMsg.data[2];
    BYTE slv_cb = RxMsg.data[3];
    AFE_WR(AFE_WINCHG,WINCHG_RM0);                              
    AFE_WR(AFE_COND0, mst_cb);               // Cell Balancing (Cell 1 - Cell 8)
    AFE_WR(AFE_COND1, 0 );                          // Cell Balancing (Cell 9 - Cell 10)
    //
    acombuf[0] = 5;
    acombuf[1] = UART_CMD_CB;
    acombuf[2] = slv_cb ;
    acombuf[3] = 0;
    Uart_BspSend(0,acombuf,4);
    f_SlvCB = ON; 
    tcom46.us[0] =(mst_cb<<8) | slv_cb;
    Can_SendResponse(ACK,0,0);
}

void dg_sw_reset(BYTE p1, BYTE p2)
{
    SW_RESET();
}

void dg_calib_MstLCVolAll(BYTE p1, BYTE p2)
{
    f_calib=ON; f_ersblcalib=ON;
    acalib_id = CALIB_MST_LVALL;     
    D_LV[1] =0xFFFF;

}
void dg_clib_MstHCVolAll(BYTE p1, BYTE p2)
{
    f_calib=ON; f_ersblcalib=ON;
    acalib_id = CALIB_MST_HVALL;     
    D_HV[1] =0xFFFF;
}
void dg_calib_SlvLCVolAll(BYTE p1, BYTE p2)
{
    f_calib=ON; f_ersblcalib=ON;
    acalib_id = CALIB_SLV_LVALL;     
}
void dg_clib_SlvHCVolAll(BYTE p1, BYTE p2)
{
    f_calib=ON; f_ersblcalib=ON;
    acalib_id = CALIB_SLV_HVALL;     
}

void dg_readLowCVolAllRef(BYTE p1, BYTE p2)
{
    TxMsg.data[0]='o';TxMsg.data[1]='k';
    TxMsg.data[2]=D_LVREF[1]>>8;
    TxMsg.data[3]=D_LVREF[1];
    Can_SendRespAckDataBlock(); 

}
void dg_readHighCVolAllRef(BYTE p1, BYTE p2)
{
    TxMsg.data[0]='o';TxMsg.data[1]='k';
    TxMsg.data[2]=D_HVREF[1]>>8;
    TxMsg.data[3]=D_HVREF[1];
    Can_SendRespAckDataBlock(); 

}

void dg_calib_HiCurrent(BYTE p1, BYTE p2)
{
    f_calib=ON; f_ersblcalib=ON;
    acalib_id = CALIB_CURR;     
    D_CURR =0xFFFFFFFF;
}

void dg_calib_0ACurrent(BYTE p1, BYTE p2)
{
    f_calib=ON; f_ersblcalib=ON;
    acalib_id = CALIB_0A;     
    D_0A =0xFFFFFFFF;
}

void dg_readLowCurRef(BYTE p1, BYTE p2)
{
    TxMsg.data[0]='o';TxMsg.data[1]='k';
    TxMsg.data[2]=0;
    TxMsg.data[3]=0;
    Can_SendRespAckDataBlock(); 
}

void dg_readHighCurRef(BYTE p1, BYTE p2)
{
    TxMsg.data[0]='o';TxMsg.data[1]='k';
    TxMsg.data[2]=D_CURRREF>>8;
    TxMsg.data[3]=D_CURRREF;
    Can_SendRespAckDataBlock(); 

}
void dg_writeHighCurRef(BYTE p1, BYTE p2)
{

    D_CURRREF = (p1<<8) | p2;
    FlashEraseCalBlock();
    FlashWrite_CalBlock();    
    TxMsg.data[0]='o';TxMsg.data[1]='k';
    TxMsg.data[2]=0;
    TxMsg.data[3]=0;
    Can_SendRespAckDataBlock(); 
}
void dg_calib_on(BYTE p1, BYTE p2)
{
    if(f_diag == ON){
        f_calibcanmode = ON;
        f_caliboffcanmode = OFF;
      //  can_log_out = 0;
        Can_SendResponse(ACK,0,0);
    }else{
        Can_SendResponse(NAK,0,0);
    }
}
void dg_calib_off(BYTE p1, BYTE p2)
{
    if(f_diag == ON){
        f_caliboffcanmode = ON;
    }else{
        Can_SendResponse(NAK,0,0);
    }
}
void dg_changeToDiagMode(BYTE p1, BYTE p2)
{
    WORD seed;
    WORD key;
    DWORD tmp;
    seed=RxMsg.data[2]<<8 |RxMsg.data[3] ;
    key= RxMsg.data[4]<<8 |RxMsg.data[5];
    tmp = 0x9164*seed + 0x1665;
    tmp &= 0xFFFF;
    if( tmp != key){
        Can_SendResponse(NAK,0,0);    
	return;
    }
    Can_SendResponse(ACK, 0, 0);
    f_diag = ON;

}

void dg_changeToNormalMode(BYTE p1, BYTE p2)
{
    Can_SendResponse(ACK, 0, 0);
    f_diag = OFF;
}
void dg_AuthPCTool(void)
{
    WORD seed;
    WORD key;
    DWORD tmp;
    if( RxMsg.data[0] == 1)
    { // Unlock
        seed=RxMsg.data[1]<<8 |RxMsg.data[2] ;
        key= RxMsg.data[3]<<8 |RxMsg.data[4];
        tmp = 0x9164*seed + 0x1665;
        tmp &= 0xFFFF;
        if( tmp != key ||  RxMsg.data[5] != 1){
            Can_SendResponse(NAK,0,0);    
    	  return;
        }
        fixedblock_access=calblock_access = 0;
        Can_SendResponse(ACK, 0, 0);
        f_eeprom = ON;
    }else if( RxMsg.data[0] == 2)
    { //lock
        if(f_eeprom != ON){
            Can_SendResponse(ACK, 0, 0);
            return;
        }
        if( fixedblock_access ){
            // EEPROM BLOCK FLASH WRITE
            FlashEraseFixedBlock();
            FlashWrite_FixedBlock();
        }
        if(calblock_access){
            FlashEraseCalBlock();
            FlashWrite_CalBlock();
        }
        Can_SendResponse(ACK, 0, 0);        
        f_eeprom = OFF;
    }else {
        Can_SendResponse(NAK,0,0);    
        f_eeprom = OFF;
    }    
}

static BYTE WR_LEN;
static BYTE off_set;
static BYTE cur_cnt;
static BYTE *cur_addr;
void Can_Write_PCTool(void)
{
     BYTE i;
    TxMsg.dlc = 8;
    TxMsg.data[0] = 'o';
    TxMsg.data[1] = 'k';
    TxMsg.data[2] = WR_LEN;
    if( off_set >= WR_LEN ){
        can_wrtopctool = OFF;
        return;
    }
    TxMsg.data[3] = off_set/4+1; 
    if( WR_LEN-off_set < 4){
           for(i=0; i< WR_LEN-off_set ;i++){
           TxMsg.data[4+i] = *((BYTE *)cur_addr+off_set+i);
           }
    }else{
           for(i=0; i<4 ;i++){
           TxMsg.data[4+i] = *((BYTE *)cur_addr+off_set+i);
           }
    }
    off_set +=4;
    CAN_Send();

}

void dg_DataFlashAccess(void)
{
    BYTE i;
    BYTE *paddr;
    BYTE mod = RxMsg.data[0];
    WORD address = RxMsg.data[1]<<8 |RxMsg.data[2];
    BYTE len = RxMsg.data[3];
    if(f_eeprom == OFF){
        Can_SendResponse(NAK,0,0);
        return;
    }
    if( address >= 0x1C00){
        fixedblock_access = 1;
        address -= 0x1C00;
        paddr = (BYTE *)&u_fo_ram_ext+address;
    } else{
        calblock_access = 1;
        address -= 0x1800;
        paddr = (BYTE *)&u_fo_ram+address;
    }
    
    if( mod == 1){
    //RD
        cur_addr = paddr;
        WR_LEN = len; 
        off_set = 0; cur_cnt = 0;
        can_wrtopctool = ON;
    }
    if(mod == 2){
    //WR
        for(i=0;i<len;i++){
           *((BYTE *)paddr+i) = RxMsg.data[4+i];
        }   
        Can_SendResponse(ACK,0,0);
    }

}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: CAN send data function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: CAN_RESULT CAN_SendMsg(void)
 *--------------------------------------------------------------------------
 * Function			: Send buffer data for CAN
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: CAN_RESULT_OK,CAN_RESULT_ERROR
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
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
int vcu_data;
int chg_data;


char checksum(void)
{
   char ch;
   ch = BITWISEXOR(RxMsg.data[0], RxMsg.data[1]);
   ch = BITWISEXOR(ch, RxMsg.data[2]);
   ch = BITWISEXOR(ch, RxMsg.data[3]);
   ch = BITWISEXOR(ch, RxMsg.data[4]);
   ch = BITWISEXOR(ch, RxMsg.data[5]);
   ch = BITWISEXOR(ch, RxMsg.data[6]);
   return ch;
}



/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: CAN received data storage processing
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void StoreRcvData(void)
 *--------------------------------------------------------------------------
 * Function			: Store received data for CAN Write command to each command
 *					: 
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
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
#if 0
void StoreRcvData()
{
	BYTE	*apwork;								// Work pointer
	_SBYTE	al1;									// Loop counter
	BYTE	cnt;

	switch( tmpRxBuf[1] )							// What's Received command ?
	{
		
		// Extension command to append here
		case 0x00:									// ExtraFunction()
			//Ext_Func();
			break;

		case 0x58:									// Initial calibration
			if( f_calibmode == ON )					// Now initial calibration mode ?
			{
				acalib_id = acanbuf[1];				// Store received data
				acalib_sts = acanbuf[0];
				if( acalib_sts == 0x01 )			// Make initial calibration ?
				{
					f_calib = ON;					// Set InitCalib req. flag
				}
			}
			break;

		case 0x57:									// Write InitCalib ref. value
			if( f_calibmode == ON )					// Now InitCalib mode ?
			{
				//Write_CalRef();						// InitCalib ref. writing
			}
			break;

			
		case 0x5a:
			if( acanbuf[0] == 0x23 && acanbuf[1] == 0x45 )
			{
				//f_flashup;							// Set Flash update mode req.
			}
			break;
			
		case 0x5e:
			if( acanbuf[0] == 0x12 && acanbuf[1] == 0x34 )
			{
				FlashErase();						// Set Flash erase mode.
			}
			break;
	
		default:									// Other
			apwork = (BYTE *)scandata.aadr;			// Set data address to pointer
			for( al1=can_dl; al1!=(_SBYTE)0xFF; al1-- )
			{
				*apwork = acanbuf[al1];				// Store received data
				apwork++;							// To next pointer
			}
			break;
	}
}
#endif
/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: CAN Read data setting
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void SetCANRead(void)
 *--------------------------------------------------------------------------
 * Function			: Set Read data to SMBus send buffer
 *					: 
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
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
void SetCANRead(void)
{
	BYTE	*apwork;								// work pointer
	_SBYTE	al1;									// loop counter

	apwork = (BYTE *)scandata.aadr;					// Set data address to pointer
	switch( tmpRxBuf[1] )							
													// Check command needs calc.
	{
		case 0x57:									// InitCalib ref. value
			Read_CalRef();
			break;
	
	}

	if( scandata.acrem == CAN_LENGTH_4) 			// 4byte Read data?
	{
		for( al1=0; al1 < scandata.acrem; al1++)
		{
			acanbuf[al1] = *apwork;					// set data to send buffer
			apwork++;
		}
	} else {										// 2byte Read data
		for( al1=0; al1 < scandata.acrem; al1++)
		{
			acanbuf[al1] = *apwork;					// Set data to send buffer
			apwork++;
		}
	}
	
	atx_dlc = scandata.acrem;
}




/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: ExtraFunction processing
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Ext_Func(void)
 *--------------------------------------------------------------------------
 * Function			: Exectute this function when received command is 0x40.
 *					: 
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
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
#if 0
 void Ext_Func(void)
{
	BYTE	al1;
	
	a_com00[0] = acanbuf[1];						// Store received data
	a_com00[1] = acanbuf[0];

	
		// Force PowerDown
		if( f_comjib == ON )						// Force PowerDown flag = ON ?
		{
			if( t_com00 == 0x6789 )					// Data is correct ?
			{
				PowerDown();						// PowerDown function
			} else {								// Data is wrong
				f_comjib = OFF;						// Clear the flag
			}
		} else {									// Force PowerDown flag = OFF
			if( t_com00 == 0x2345 )					// PowerDown req. command ?
			{
				f_comjib = ON;						// Set force PowerDown flag
			}
		}


		// - FET test function -
		if(f_fettst == ON && a_com00[1] == 0xFF )	// & high byte is 0xFF ?
		{
			afet = a_com00[0] & 0x03;				// 븉뾴Bit귩mask
			AFE_WR(AFE_FCON,afet);
		}
		

		if( a_com00[1] == 0x12 )					// 0x12** command ?
		{
			switch( a_com00[0] )					// ** is ...
			{			
			case 0x33:								// S/W reset
				__asm("DW 0FFFFh");					// 떗맕Reset(뼟믦?뼺쀟렳뛱걂
				break;
			case 0x34:								// Force Sleep
				f_fcslp = ON;						// Set force sleep flag
				break;
			

			// - FET test function -
			case 0x35:								// FET control
				
				f_fettst = ON;						// FET test mode ON
				AFE_WR(AFE_FCON,FET_OFF);
				break;
				
			case 0x36:
			
				f_fettst = OFF;						// FET test mode OFF
				break;
			}
		}
}

#endif

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Calibration referenece value read processing
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Read_CalRef(void)
 *--------------------------------------------------------------------------
 * Function			: 
*					: Set InitCalib reference value of command specified
*					:  by "acalib_id"(Received by 0x58 command) to "t_com57"
 *					: 
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
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
void Read_CalRef(void)
{
	if( acalib_id < 0x0e )							// Voltage-related ?
	{
		if( acalib_id == 0x0c )						// LVall ?
		{
			t_com57 = D_LVREF[1];					// Set LV2
		} else {
			if( acalib_id == 0x0d )					// HVall ?
			{
				t_com57 = D_HVREF[1];				// Set HV2
			} else {
				if( acalib_id == 0x0a )				// LVIN12 ?
				{
					t_com57 = D_V12LVREF;			// Set LVIN12
				} else {
					if( acalib_id == 0x0b )			// HVIN12 ?
					{
						t_com57 = D_V12HVREF;		// Set HVIN12
					} else {
													// Specified individual
						t_com57 = D_LVREF[acalib_id];
					}
				}
			}
		}
	} else {
		if( acalib_id == 0x0f )						// Current ?
		{
			t_com57 = D_CURRREF;
		} else {
			if( acalib_id == 0x10 )					// Temperature ?
			{
				t_com57 = D_CALTHREF;
			} else {												// Other(undefined)
				if( acalib_id < 0x2a)
				t_com57 = D_HVREF[acalib_id -0x20];					// HV 
				else{
					if(acalib_id == 0x2c)
					{
						t_com57 = D_TCLVREF;
					}else if(acalib_id == 0x2d)
					{
						t_com57 = D_TCHVREF;
					}else{
						t_com57 = 0xFFFF;
					}
				}
			}
		}
	}
}



/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: InitCalib reference value write function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void WriteFlash_Calref(BYTE *apadr)
 *--------------------------------------------------------------------------
 * Function			: Write InitCalib reference value.
 *					: This function is called from Write_CalRef().
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: WORD *tpadr : Pointer to write address
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
 * History			: 2016.02.21 Ver 1.00
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
 #if 1
static void WriteFlash_CalRef( BYTE *apadr)
{
	FlashWrite( apadr, &a_com57[0], 2 );			// Write data of 2bytes at DataFlash area
	DFLEN = 1;										// Data flash access enable
	Waittimer_us(10);								// wait 10us
	memcpy( &u_fo_ram, &u_fo_rom, sizeof(u_fo_rom) );	// DataFlash own data -> RAM copy
	DFLEN = 0;										// Data flash access disable
}
#endif

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: InitCalib reference value write function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Write_Calref(void)
 *--------------------------------------------------------------------------
 * Function			: Function when 0x59 command is received.
 *					: Write received data to InitCalib reference value
 *					:  of command specified by "acalib_id"(Received by
 *					:  0x58 command).
 *					: 
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
 * Used function	: WriteFlash_Calref
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 * History			: 2010.09.27 Ver 0.01
 *					:  New create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
void Write_CalRef(void)
{
	BYTE	al1;

	a_com57[0] = acanbuf[1];						// Store received data
	a_com57[1] = acanbuf[0];

	if( acalib_id < 0x0c )							// LVx / TotalLV / HV ?
	{
		if( acalib_id == 0x0a )						// LVIN12 ?
		{
			WriteFlash_CalRef((BYTE*)&D_R_V12LVREF);
		} else {
			if( acalib_id == 0x0b )					// HVIN12 ?
			{
				WriteFlash_CalRef((BYTE*)&D_R_V12HVREF);
			} else {
				WriteFlash_CalRef((BYTE*)&D_R_LVREF[acalib_id]);
			}
		}
	} else {
		if( acalib_id == 0x0c )						// LV all ?
		{
			for( al1=0; al1<D_SERIES; al1++ )	
													// Write it continues number of cell
			{
				WriteFlash_CalRef((BYTE*)&D_R_LVREF[al1]);
			}
		} else {
			if( acalib_id == 0x0d )					// HV all ?
			{
												
													// Write it continues number of cell
				for( al1=0; al1<D_SERIES; al1++ )
				{
					WriteFlash_CalRef((BYTE*)&D_R_HVREF[al1]);
				}
			} else {
				if( acalib_id == 0x0f )				// Current ?
				{
					WriteFlash_CalRef((BYTE*)&D_R_CURRREF);
				} else {
					if( acalib_id == 0x10 )			// Temperature ?
					{
						WriteFlash_CalRef((BYTE*)&D_R_CALTHREF);
					}else if(acalib_id < 0x2a){
						WriteFlash_CalRef((BYTE*)&D_R_HVREF[acalib_id - 0x20]);
													//HVx
					}else{							// acalib_id >= 2a
						
						if( acalib_id == 0x2c)
						{
							WriteFlash_CalRef((BYTE*)&D_R_TCLVREF);
						}
						
						if( acalib_id == 0x2d)
						{
							WriteFlash_CalRef((BYTE*)&D_R_TCHVREF);
						}
					}
				}
			}
		}
	}
}
#endif

RETURN_CODE Can_ParseMsg(void)
{
    BYTE i;
    uint32_t temp;
    switch(CanRxMsg.id)
    {
        case CANRX_CUR_SENSOR_ID:
            temp = 0x00000000;
            temp |= ((DWORD)(CanRxMsg.data[0])<<24);
            temp |= ((DWORD)(CanRxMsg.data[1])<<16);
            temp |= ((DWORD)(CanRxMsg.data[2])<<8);
            temp |= ((DWORD)CanRxMsg.data[3]);
            temp = temp^0x80000000;
            ir.model.can_cur = temp;
        break;
    
    }
}


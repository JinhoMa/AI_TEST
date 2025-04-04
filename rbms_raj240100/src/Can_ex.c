/**
    ******************************************************************************
    *     @file         Can_ex.c
    *     @version   
    *     @author    
    *     @date      2019/10/10 
    *     @brief     
    ******************************************************************************
    */
#include "define.h"													// union/define definition
#include "cell.h"
#include "dataflash.h"												// DataFlash data definition
#include "kousei.h"													// Initial calibration
#include "ram.h"													// ram.c
#include "afe.h"
#include "mcu.h"
#include "can.h"
#include "can_ex.h"
#include "main.h"

#define PROTEC 0xFF    
BYTE dummy = 0xFF;
extern DWORD slv_rsptime;
extern BYTE uart_err;

const CanExTable clog[] ={
    { CANTX_BMS0_LOG1 , (BYTE*)&tminv,(BYTE*)&tminv+1 , (BYTE*)&tmaxv, (BYTE*)&tmaxv+1 , (BYTE*)&minCV_n,(BYTE*)&maxCV_n, (BYTE *)&com_BVol,  (BYTE *)&com_BVol+1  },                    // 01    minv, maxv, min_n, max_n
    { CANTX_BMS0_LOG2 , (BYTE*)&minTemp,(BYTE*)&maxTemp , (BYTE*)&minT_n, (BYTE*)&maxT_n, (BYTE*)&com_Cur10mA , (BYTE*)&com_Cur10mA+1, (BYTE *)&tcom46.us[0], (BYTE *)&tcom46.us[0]+1 },                    // 01    minv, maxv, min_n, max_n
    { CANTX_BMS0_LOG3 , (BYTE*)&tcell11 ,(BYTE*)&tcell11+1 ,(BYTE*)&tcell12 ,(BYTE*)&tcell12+1, (BYTE*)&tcell13 ,(BYTE*)&tcell13+1, (BYTE*)&tcell14 ,(BYTE*)&tcell14+1 },
    { CANTX_BMS0_LOG4 , (BYTE*)&tcell15 ,(BYTE*)&tcell15+1 ,(BYTE*)&tcell16 ,(BYTE*)&tcell16+1, (BYTE*)&tcell17,(BYTE*)&tcell17+1,(BYTE*)&tvolt[0] ,(BYTE*)&tvolt[0]+1},
    { CANTX_BMS0_LOG5 , (BYTE*)&tvolt[1] ,(BYTE*)&tvolt[1]+1 ,(BYTE*)&tvolt[2] ,(BYTE*)&tvolt[2]+1, (BYTE*)&tvolt[3] ,(BYTE*)&tvolt[3]+1, (BYTE*)&tvolt[4] ,(BYTE*)&tvolt[4]+1 },
    { CANTX_BMS0_LOG6 , (BYTE*)&tvolt[5] ,(BYTE*)&tvolt[5]+1 ,(BYTE*)&tvolt[6] ,(BYTE*)&tvolt[6]+1, (BYTE*)&smeas.smeasdat.cur, (BYTE*)&smeas.smeasdat.cur+1,(BYTE*)&smeas.smeasdat.cur+2,(BYTE*)&smeas.smeasdat.cur+3 },
    { CANTX_BMS0_LOG7 , (BYTE*)&ExTemp[0],(BYTE*)&ExTemp[1],(BYTE*)&ExTemp[2],(BYTE*)&InTemp[0],(BYTE*)&InTemp[1],(BYTE*)&InTemp[2] ,(BYTE *)&u_fo_ram_ext.d_CTLFLG,(BYTE *)&u_fo_ram_ext.d_CTLFLG+1 },
    { CANTX_BMS0_LOG8 , (BYTE*)&amode,(BYTE*)&acause, (BYTE *)&state,(BYTE *)&state+1,(BYTE *)&tpacksts,(BYTE *)&tpacksts+1,(BYTE *)&Bevt.uc[0],(BYTE *)&Bevt.uc[1]},
    { CANTX_BMS0_LOG9 , (BYTE*)&RSoC, (BYTE *)&RCC, (BYTE *)&RCC+1,(BYTE *)&Bevt.uc[2],(BYTE *)&uart_err,(BYTE *)&dummy,(BYTE *)&tvin12v,(BYTE *)&tvin12v+1},

#if 0
    { 0x382  , 2, (BYTE*)&minTemp,  (BYTE*)&maxTemp , (BYTE*)&minT_n.us ,   (BYTE*)&maxT_n.us },               // 02    minT, maxT, minT_n, maxT_n
    { 0x383  , 2, (BYTE *)&tcurr.uc[0],   (BYTE *)&tcurr.uc[2]  , (BYTE*)&minBdTemp, (BYTE*)&maxBdTemp },      //03     Current, minBdTemp, maxBdTemp
    { 0x384  , 2, (BYTE*)&t_com0d,   (BYTE*)&t_com0fc,  (BYTE*)&t_com10c,     NULL },                          //04 RSOC, RemainCap, FullCap
    { 0x385  , 2, (BYTE*)&t_com14,    (BYTE*)&t_com15,    (BYTE*)&t_com16,    (BYTE*)&Bevt.us[0] },                  //05 ChCurrent, ChVoltage, BtStatus, EvAlarm
    { 0x386  , 2, (BYTE*)&CONFLG,   (BYTE*)&tuartsts,   (BYTE*)&pcTool,     (BYTE*)&psuCon.us},                  //06 ConfigFlag, Slave State, PcTool, PsuCon
    { 0x387  , 2, (BYTE*)&(tvolt[0]), (BYTE*)&(tvolt[1]), (BYTE*)&(tvolt[2]), (BYTE*)&(tvolt[3]) },                   // 07 cell1 Cell2,Cell3,Cell4
    { 0x388  , 2, (BYTE*)&(tvolt[4]), (BYTE*)&(tvolt[5]), (BYTE*)&(tvolt[6]), (BYTE*)&(tvolt[7]) },                   // 08 cell5 Cell6,Cell7,Cell8
    { 0x389  , 2, (BYTE*)&tcell11, (BYTE*)&tcell12, (BYTE*)&tcell13, (BYTE*)&tcell14 },                                  // 09 cell9 cell10  cell11 cell12 
    { 0x38A  , 2, (BYTE*)&tcell15,  (BYTE*)&tcell16,  (BYTE*)&tcell17,  (BYTE*)&tcell18 },                               // 0A cell13 cell14, cell15,cell16
    { 0x38B  , 2, (BYTE*)&state ,    (BYTE*)&tpacksts,  (BYTE*)&t_com46, (BYTE*)&CellBalancing_OnSec},      // 0B state, packstatus, cellbalst, cbwtime
    { 0x38C  , 4, (BYTE*)&t_com09,          (BYTE*)&tmode.us,                         (BYTE*) NULL,                        (BYTE*) PROTEC }            // 0C Total Cell Voltage
#endif
};    

BYTE canout_idx;
BYTE can_count;


void Can_ExInit(void)
{
    can_count = (sizeof(clog) / sizeof(CanExTable));
    
}
void Can_Ext_Output(void)
{
    BYTE i,size;
    BYTE *temp;
    TxMsg.id = clog[canout_idx].id;
    TxMsg.dlc       = 8U;
    for(i=0;i<8; i++)
    {
         TxMsg.data[i]= *clog[canout_idx].d[i];
    }
    canout_idx = (canout_idx+1) % can_count ;
    CAN_Send();
}




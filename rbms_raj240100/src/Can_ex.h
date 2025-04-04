#ifndef __CAN_EX_H__
#define __CAN_EX_H__


#ifndef _CAN_H_
#include "can.h"
#endif

#ifndef __BMS_H__
#include "BMS.h"
#endif
/**
    ******************************************************************************
    *     @file         Can_ex.h
    *     @version   
    *     @author    
    *     @date      2019/10/10 
    *     @brief     
    ******************************************************************************
    */
#define CAN_ID_TEMP     8
#define CAN_ID_VOL       9



typedef struct {
    WORD id;
    BYTE *d[8];
} CanExTable;

#if  defined(IC_MASTER) && (IC_MASTER==1)
#define CANTX_BMS0_RSP_ID                          0x380
#define CANRX_BMS0_LOGOUT_ID                    0x382
#define CANRX_BMS0_SCURTY_ID                    0x383
#define CANRX_BMS0_CONFIG_ID                    0x384
#define CANRX_BMS0_BINARYRCV_ID               0x386
#define CANRX_BMS0_RCVBLKSTR_ID               0x387
#define CANRX_BMS0_RCVBLKEND_ID              0x388
#define CANTX_BMS0_LOG1                             0x391
#define CANTX_BMS0_LOG2                             0x392
#define CANTX_BMS0_LOG3                             0x393
#define CANTX_BMS0_LOG4                             0x394
#define CANTX_BMS0_LOG5                             0x395
#define CANTX_BMS0_LOG6                             0x396
#define CANTX_BMS0_LOG7                             0x397
#define CANTX_BMS0_LOG8                             0x398
#define CANTX_BMS0_LOG9                             0x399

#define CANRX_BMS0_DIAG_ID                        0x39F
#endif
#if defined(IC_SLAVE) && (IC_SLAVE==1)
#define CANTX_BMS0_RSP_ID                          0x381
#define CANRX_BMS0_BINARYRCV_ID               0x38B
#define CANRX_BMS0_RCVBLKSTR_ID               0x38C
#define CANRX_BMS0_RCVBLKEND_ID              0x38D
#define CANRX_BMS0_SCURTY_ID                    0x38F
#define CANRX_BMS0_CONFIG_ID                    0x390
#endif
#define CANRX_BMS0_MSTDOWNLOAD_START_ID    0x385
#define CANRX_BMS0_MSTDOWNLOAD_STOP_ID     0x389
#define CANRX_BMS0_SLVDOWNLOAD_START_ID    0x38A
#define CANRX_BMS0_SLVDOWNLOAD_STOP_ID     0x38E


#define CANRX_BMS0_CALMODE_ID      0x701
#define CANRX_BMS0_RDREF_ID           0x702
#define CANRX_BMS0_WRREF_ID           0x703
#define CANRX_BMS0_UPDATECAL_ID    0x704

#define CANRX_CUR_SENSOR_ID        0x3C2

typedef struct{
    BYTE cmd;
} CanRx_Log;


typedef struct{
    BYTE cmd;
} CanRx_CALREQ;

typedef struct{
    BYTE id;
    BYTE st;
} CANRx_CALMD;

typedef union{
    CanRx_Log logcmd;
    CanRx_CALREQ calreq;
    CANRx_CALMD  calmd;
    BYTE  data[8];
} CanRx_Msg;

#define MsgSetClr(x)    x[2]=x[3]=x[4]=x[5]=x[6]=x[7]=0xFF;
#define MsgSetAck(x)   x[0]='o';x[1]='k';
#define MsgSetNak(x)   x[0]='e';x[1]='r';x[2]='r';

#define MsgSetRsp(x1,x2)   rspd1=x1; rspd2=x2
#define ACK 1
#define NAK 0

#define INVALID_CODE  1

void Can_ExInit(void);
void Can_Ext_Output(void);

#endif //__CAN_EX_H__

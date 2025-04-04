/**
 ******************************************************************************
 *     @file         CAN_DiagComProc.c
 *     @version   
 *     @author    
 *     @date      2024/09/24 
 *     @brief     
 ******************************************************************************
*/
#include "main.h"
#include "obshal.h"
#include "BMS.h"
#include "BMS_OS.h"
#include "CAN_DiagCom.h"
#include <stdlib.h>
#include <string.h>

#define MAX_MSG_SIZE  10

uint32_t diag_can_fd;

STR_CanGw CanGW;
STR_CAN_MSG CanMsg[MAX_MSG_SIZE];

/* +------------------------------------------------------------------------
   +             Can_DiagMasterProc.c
   +                   +
   +             Can_DiagRackProc.c
   +                   +
   +             Can_DiagComProc.c
*/

static void startDiagMode(void)
{
    //자동 CellBalancing 기능을 끈다.
    //AutoBalancingMode = FALSE;
}

static void stopDiagMode(void)
{
    //자동 CellBalancing 기능을 끈다.
    //AutoBalancingMode = TRUE;
}

PUBLIC void responsePositiveMessage(uint8_t  LocalID , uint8_t SubCMD, uint8_t *data)
{
    hCanMessage message;

    message.id = DIAG_PRT_RSP;
	message.len  =8;
	message.data[0] = LocalID+0x40;
	message.data[1] = SubCMD;
	message.data[2] = *data++;
	message.data[3] = *data++;
	message.data[4] = *data++;
	message.data[5] = *data++;
	message.data[6] = *data++;
	message.data[7] = *data;

    while ( hCanWrite(diag_can_fd, &message, 1) == E_INPROGRESS){
        hTimDelay(200);
    }

}

PUBLIC void responseNegtiveMessage(uint8_t LocalID, uint8_t Reason)
{
    hCanMessage message;

    message.id = DIAG_PRT_RSP;
	message.len  =8;
	message.data[0] =  NEGATIVE_RESPONSE_LID;
	message.data[1]	= LocalID + 0x40;
	message.data[2] =  Reason;
	message.data[3] =  0xFF;
	message.data[4] =  0xFF;
	message.data[5] =  0xFF;
	message.data[6] =  0xFF;
	message.data[7] =  0xFF;

    while ( hCanWrite(diag_can_fd, &message, 1) == E_INPROGRESS){
        hTimDelay(100);
    }

}

PUBLIC void can_com_initInterface(void)
{
    int i;
    for(i=0;i<MAX_MSG_SIZE;i++)
    {
        CleanUpMsg(&CanMsg[i]);
    }
    diag_can_fd = hCanGetHandle("CANB");
}
PUBLIC void* can_com_AllocMessage(uint8_t who)
{
    int i;
    for(i=0;i<MAX_MSG_SIZE;i++)
    {
        if(IsFreeMsg(&CanMsg[i])){
            ReserveMsg(&CanMsg[i], who);
            return &CanMsg[i];
        }
    }
    return NULL;
}


PUBLIC void can_com_initSession(void)
{
 ////////////////////////////////////////////////////////////////////check////////////////////////////////////////////////////////////
	CanGW.AccessStatus=DENYACCESS;//SUCESSACCESS; //DENYACCESS
 ////////////////////////////////////////////////////////////////////check////////////////////////////////////////////////////////////
 	CanGW.MBMSMode=NORMAL_MODE;

 	CanGW.Security.SecurityKey=0;
 	CanGW.Security.SeedInfo =0x1234;
 	//-- Must not Initialize
 	//LineTest.Security.DummyValue=0;
 	can_com_initInterface();
}

PUBLIC void can_com_modechange_handler(uint8_t LocalID, uint8_t SubCMD)
{
    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    switch(SubCMD){
 	    case NORMAL_MODE:
     	    if(CanGW.MBMSMode != NORMAL_MODE){
    		    PushJob(CANB_SCHEDULER_IDX,INIT_LINETEST, CANBSCH_TIME, NULL);
            }
            stopDiagMode();
            responsePositiveMessage(LocalID, SubCMD, data );
        break;
        case DIAG_MODE:
 	    if( CanGW.MBMSMode != DIAG_MODE){
            CanGW.MBMSMode=DIAG_MODE;
            startDiagMode();
            responsePositiveMessage(LocalID, SubCMD, data );
        }else{
            responseNegtiveMessage(LocalID,0);
        }
        break;
        default:
        responseNegtiveMessage(LocalID,0);
        break;
   }
}
PUBLIC void can_com_securityacess_handler(uint8_t *msg)
{
    uint8_t lid, sid;
    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    lid = msg[0];
    sid = msg[1];
    
	if(CanGW.MBMSMode !=DIAG_MODE)
	{
	    responseNegtiveMessage(lid, 0 );
        return;
    }
    if(sid == REQ_SEED){
        srand((int32_t)hTimCurrentTime());
        CanGW.Security.SeedInfo = rand()%10+1;
        data[0] = (uint8_t)( (CanGW.Security.SeedInfo>>8)&0xFF);
        data[1] = (uint8_t)( CanGW.Security.SeedInfo&0xFF);
        responsePositiveMessage(lid,sid, data);
	}else if( sid==SND_KEY)
	{
        CanGW.Security.SecurityKey=(uint16_t)(((CanGW.Security.SeedInfo * KEY_A)+KEY_C) & KEY_E);//Calculation Key
		if( ( (uint8_t)(CanGW.Security.SecurityKey >>8) == msg[2] ) && ( (uint8_t)CanGW.Security.SecurityKey == 
		msg[3]) ){
			CanGW.AccessStatus=SUCESSACCESS;
            memset(data,0xFF,6);
			responsePositiveMessage(lid, sid, data);
        }else{
			CanGW.AccessStatus=DENYACCESS;
            PushJob(CANB_SCHEDULER_IDX,INIT_LINETEST, CANBSCH_TIME, NULL);
			responseNegtiveMessage(lid, 0);
		}
    }
}
__WEAK PUBLIC void can_Diag_ReadCellNum(void){
    uint8_t data[6];
    data[0]=TRAY_CELL_NUMS;
    data[1]=0xFF; 
    data[2]=0xFF;
    data[3]=0xFF;
    data[4]=0xFF;
    data[5]=0xFF;
    responsePositiveMessage(HANDLETRAY, CELL_NUM, data);    
}
__WEAK PUBLIC void can_Diag_ReadBlockNum(void){
    uint8_t data[6];
    data[0]=BMIC_DEVICE_COUNT;
    data[1]=0xFF; 
    data[2]=0xFF;
    data[3]=0xFF;
    data[4]=0xFF;
    data[5]=0xFF;
    responsePositiveMessage(HANDLETRAY, BLOCK_NUM, data);
}
__WEAK PUBLIC void can_Diag_ReadMDTmpNum(void)
{
    uint8_t data[6];
    data[0]=MODULE_TEMP_SENSORNUM;
    data[1]=0xFF; 
    data[2]=0xFF;
    data[3]=0xFF;
    data[4]=0xFF;
    data[5]=0xFF;
    responsePositiveMessage(HANDLETRAY, MDTMP_NUM, data);
}

__WEAK PUBLIC void can_Diag_ReadBDTmpNum(void)
{
    uint8_t data[6];
    data[0]=MODULE_BDTEMP_SENSORNUM;
    data[1]=0xFF; 
    data[2]=0xFF;
    data[3]=0xFF;
    data[4]=0xFF;
    data[5]=0xFF;
    responsePositiveMessage(HANDLETRAY, BDTMP_NUM, data);
}

#if 0
__WEAK int32_t can_rbms_logout(void){
    return 0;
}
__WEAK void can_Diag_ReadDTCInfomation(void){
    responseNegtiveMessage(DTCINFO, N_INVALID_ID);
}
__WEAK void can_Diag_ClearDTCInfomation(uint8_t resp){
    responseNegtiveMessage(DTCINFO, N_INVALID_ID);
}
__WEAK  PUBLIC void can_Diag_SuccessFactoryReset(void){
    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    responsePositiveMessage(FACTRESET, 0x1, data);
    hSysWait(2000);
    HAL_NVIC_SystemReset(); // Factory Reset 후에 Rebooting 한다.
}

#endif

PUBLIC int32_t can_com_logout(void)
{
    int32_t ret = 0;
    ret = can_rbms_logout();
    return ret;
}

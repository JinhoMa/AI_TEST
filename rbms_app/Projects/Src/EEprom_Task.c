/**
    ******************************************************************************
    *     @file         EEprom_Task.c
    *     @version   
    *     @author    
    *     @date      2017/09/12 
    *     @brief     EEPROM 데이터 READ,WRITE 처리 TASK
    ******************************************************************************
    */

#include "obshal.h"
#include "hos.h"
#include "bms_os.h"
#include "bms.h"
//#include "Sk_Config.h"
//#include "EEPROM_TASK.h"
#include "AppQueue.h"
//#include "CAN_LineTest.h"
#include "EEprom_Control.h"
//#include "Can_Communication.h"
//#include "BMIC_ProcessData.h"
#include "EEprom_ProcessData.h"


static uint32_t *EEprom_Buf;
static void DoJob(int );
void EEprom_Task(void* arg)
{
	int job;
    eeprom_run = 1;
	while(1)
	{
		SEM_pend(EEPROM_SEM, SYS_FOREVER);
		job = PopJob(EEPROM_SCHEDULER_IDX, (void **)&EEprom_Buf );
        DoJob(job);
        FreeMsg(EEprom_Buf);
		//if(job) HandlingEEPROM();
	}

}

void DoJob(int JobIdx)
{
    switch(JobIdx)
    {
        case FACTORYREST_FROM_DIAGMODE:
            EEP_FactoryResetEEPRom(0);
            PushJob(CANB_SCHEDULER_IDX, FACTORYRESET_DIAG_SUCCESS_FROM_EEP, CANBSCH_TIME, NULL);
            break;
        case WRITEBYTE_FROM_DIAGMODE:
            EEP_WriteByteFromDiag(EEprom_Buf);
            break;
        case READBYTE_FROM_DIAGMODE:
            EEP_ReadByteFromDiag(EEprom_Buf);
            break;
        case READMODEL_FROM_DIAGMODE:
            EEP_ReadModelFromDiag(EEprom_Buf);
            break;
        case READVER_FROM_DIAGMODE:
            EEP_ReadVerFromDiag(EEprom_Buf);
            break;
        case RTCSET_FROM_LINETEST:
            EEP_RTCSet();
        case UPDATE_USEDATA_SEC:
            EEP_UpdateUseData();
            break;
        case CLEAR_FAULT_RECORD:
            EEP_ClearFaultRecord();
            break;
#if 1  // PC TOOL
        case ULOCKWRITE_FROM_PCTOOL:
            EEP_ChangeAccessFromPCTool(1);
            break;
        case LOCKWRITE_FROM_PCTOOL:
            EEP_ChangeAccessFromPCTool(0);
            break;
        case READBYTE_FROM_PCTOOL:
            EEP_ReadByteFromPCTool(EEprom_Buf);
            break;
        case WRITEBYTE_FROM_PCTOOL:
            EEP_WriteByteFromPCTool(EEprom_Buf);
            break;
        case FACTORYREST_FROM_PCTOOL:
            EEP_FactoryResetEEPRom(0);
            PushJob(CANB_SCHEDULER_IDX, FACTORYRESET_PC_SUCCESS_FROM_EEP, CANBSCH_TIME, NULL);
            break;
//        case STARTINGSOH_FROM_PCTOOL:
//            EEP_BuringSOH();
//            break;
        case ERASEALL_FROM_PCTOOL:
            EEP_EraseAll();
            PushJob(CANB_SCHEDULER_IDX, ERASEALL_SUCCESS_FROM_EEP, CANBSCH_TIME, NULL);
            break;
//        case RTCSET_FROM_PCTOOL:
//        case UPDATE_EVTLOG:
//            EEP_UpdateEventLog();
//            break;
#endif        
    }
}

















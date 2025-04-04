/**
    ******************************************************************************
    *     @file         CAN_Transmit_Task.c
    *     @version   
    *     @author    
    *     @date      2017/09/12 
    *     @brief     CAN 데이터 처리 , PC 또는 검사장비 에서 보내는 메시지 처리 및, EEPROM TASK통신
   *                       또는 BPU 제어신호 처리 TASK와 통신  
    ******************************************************************************
    */


#include "obshal.h"
#include "hos.h"
#include "bms_os.h"
#include "bms.h"
#include "AppQueue.h"
//#include "Sk_Config.h"
//#include "CAN_RAckBMS.h"
//#include "CAN_LineTest.h"
//#include "CAN_PcInter.h"
//#include "Can_Communication.h"
#include "Can_Transmit_Task.h"
#include "Can_DiagCom.h"
#include "CAN_PCTool.h"
//#include "Operating_Condition.h"
//#include "BMIC_ProcessData.h"

//#include "diag_protocol.h"
//#include "TRAY_SYS.h"

static void DoJob(int JobIdx);
static uint32_t *CanTrans_buf;


//-------------3ms Task For CAN_B Response
void CanB_Scheduler_Task(void *arg)
{
    int job;
    canb_run = 1;
    can_com_initSession();
	while(1)
	{
		SEM_pend(CANB_SCHEDULER_SEM, SYS_FOREVER);
		job = PopJob(CANB_SCHEDULER_IDX, (void **)&CanTrans_buf);
        DoJob(job);
        FreeMsg(CanTrans_buf);
	}

}

static void DoJob(int JobIdx)
{
    int ret;
    switch(JobIdx)
    {
/****************************************************************************
 *          CANOE TOOL or EOL Equ. Event
 ****************************************************************************/ 
        case DIAG_LOG_OUT:
            ret = can_com_logout();
            if(ret)
                PushJob(CANB_SCHEDULER_IDX, DIAG_LOG_OUT, CANBSCH_TIME , 0);  
            break;
        case INIT_LINETEST:
        	can_com_initSession();
        	break;
        case READ_DTC_FROM_DIAGMODE:
    	    can_Diag_ReadDTCInfomation();
    	    break;        
        case CLR_DTC_FROM_DIAGMODE:
        	can_Diag_ClearDTCInfomation(1);
        	break;
        case FACTORYRESET_DIAG_SUCCESS_FROM_EEP:
            can_Diag_SuccessFactoryReset();
            break;        
        case READ_CELLNUM_FROM_DIAGMODE:
            can_Diag_ReadCellNum();
            break;
        case READ_BLOCKNUM_FROM_DIAGMODE:
            can_Diag_ReadBlockNum();
            break;
        case READ_MDTMPNUM_FROM_DIAGMODE:
            can_Diag_ReadMDTmpNum();
            break;
        case READ_BDTMPNUM_FROM_DIAGMODE:
            can_Diag_ReadBDTmpNum();
            break;
        case READ_VER_SUCCESS_FROM_EEP:
            can_Diag_SuccessVerInfoEEPROM(CanTrans_buf);
            break;
        case READ_RACK_TRAYCELLVOL_FROM_DIAGMODE:
            can_Diag_RackReadTrayCellVol();
            break;
        case WRBYTE_DIAG_SUCCESS_FROM_EEP:
            can_Diag_SuccessWriteBytesEEPROM(CanTrans_buf);
            break;
        case RDBYTE_DIAG_SUCCESS_FROM_EEP:
            can_Diag_SuccessReadBytesEEPROM(CanTrans_buf);
            break;
        case READ_MODEL_SUCCESS_FROM_EEP:
            can_Diag_SuccessModelInfoEEPROM(CanTrans_buf);
            break;
        case READ_RACK_TRAYTEMP_FROM_DIAGMODE:
            can_Diag_RackReadTrayTmp();
            break;
        
/****************************************************************************
 *          PC Tool Event
 ****************************************************************************/ 
        case RET_WRITEBYTE_FROM_EEP:
            PCInter_ReturnWriteBytes(CanTrans_buf);
            break;
        case RET_READBYTE_FROM_EEP:
            PCInter_ReturnReadBytes(CanTrans_buf);
            break;
        case SEND_SEGDATA_TO_PCTOOL:
            PCInter_SendSegmentDataToPC();
            break;
        case FACTORYRESET_PC_SUCCESS_FROM_EEP:
            PCInter_SuccessFactoryReset();
            break;
        case ERASEALL_SUCCESS_FROM_EEP:
            PCInter_SuccessEraseAll();
            break;
#if 0


    case WRTEST_SUCCESS_FROM_NET:
        Diag_ReturnWREthenet(CanTrans_buf);
        break;
    case RDTEST_SUCCESS_FROM_NET:
        Diag_ReturnRDEthernet(CanTrans_buf);
        break;
    case READ_RACK_TRAYCNT_FROM_DIAGMODE:
        Diag_RackReadTrayNum();
        break;
    case READ_BPU_BPUTMP_FROM_DIAGMODE:
        Diag_BPUReadBpuTmp();
        break;
    case READ_RACK_CELLDEV_FROM_DIAGMODE:
        Diag_RackReadCellDev();
        break;
    case CTRL_BPU_P_RLYST_FROM_DIAGMODE:
        Diag_BPUCtrlRlyPStatus();
        break;
    case CTRL_BPU_N_RLYST_FROM_DIAGMODE:
        Diag_BPUCtrlRlyNStatus();
        break;

    case RETURN_RELAYCMD_FROM_R10MS:
        Diag_RackReturnRelyCmd(CanTrans_buf);
        break;
    case GET_INTLOCKST_FROM_DIAGMODE:
        Diag_GetIntlockState();
        break;
    case SEND_EVTLOGDATA_TO_PCTOOL:
        PCInter_SendEvtLogDataToPC();
        break;
#endif
    default:
        break;
    }
}


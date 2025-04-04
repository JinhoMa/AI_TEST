/**
 ******************************************************************************
 *     @file         Rack_Task.c
 *     @version
 *     @author
 *     @date      2024/09/19
 *     @brief
 ******************************************************************************
*/
#include "obshal.h"
#include "BMS.h"
#include "BMS_OS.h"
#include "master.h"
#include "SYS_ERR_CHK.h"
#include "Can_DiagCom.h"
#include "Buzzer_Control.h"
#include "GET_EXT_TEMP.h"
#include "sm.h"
#include "cellbalancing.h"
//#include "R20MS_ProcessData.h"

static uint32_t *RBMS_Buf;
static int timeout ;
static int req;
static uint8_t stop_bit;

static void DoJob(int JobIdx);
static void init_RackTask(void)
{
    PushJob(RBMS_SCHEDULER_IDX, RBMS_RELAYCONTROL, RBMS_EVT_TIME + 2000, NULL);  //부팅후 100ms
   // PushJob(RBMS_SCHEDULER_IDX, RBMS_SEND_CANGWDATA, RBMS_EVT_TIME*5, NULL );
    PushJob(RBMS_SCHEDULER_IDX, RBMS_UPDATE_HOST_DATA, RBMS_EVT_TIME, NULL);
    PushJob(RBMS_SCHEDULER_IDX, RBMS_SEND_REQTRAY, RBMS_EVT_TIME, NULL);
    PushJob(RBMS_SCHEDULER_IDX, RBMS_DIAG_UPDATE, RBMS_EVT_TIME + 4000, NULL);
    PushJob(RBMS_SCHEDULER_IDX, RBMS_SOC, 3000, NULL);
    PushJob(RBMS_SCHEDULER_IDX, RBMS_BALANCING_CHK, 500, NULL);
    PushJob(RBMS_SCHEDULER_IDX, RBMS_WATCHDOG_CHK, 500, NULL);
		PushJob(RBMS_SCHEDULER_IDX, RBMS_GET_TEMP, 100, NULL);
    buzzerInit();   //& Buzzer 테스트용 코드
}

//나중에 CAN으로 데이터를 받게 되면 여기서 처리하도록 한다.
// 지금은 BMIC Task에서 Req하게 되면 업데이트 하도록 구현되어 있다.
void R20ms_RspTrayData(void)
{
    static uint64_t prv;
    timeout = 0;
    prv = hTimCurrentTime();
    hLogProc(LOG_RBMS, "%d : rsp()\n", prv);
    DTC_RBMS_Alarm(DTC_TBMS_RX_F, 0, 0);
    RackSys_UpdateModuleData();
}

void R20ms_ReqTrayData(void)
{
    static uint64_t prv;
    if (!req)
    {
        req = 1;
        timeout = 1;
        prv = hTimCurrentTime();
        hLogProc(LOG_RBMS, "%d : req send() \n", prv);
        PushJob(RBMS_SCHEDULER_IDX, RBMS_COMM_CHECK, RBMS_EVT_TIME + COMM_ERR_TIMEOUT, NULL);
    }
}

void R20ms_CommErrCheck(void)
{
    uint64_t prv;
    if (timeout)
    {
        prv = hTimCurrentTime();
        hLogProc(LOG_RBMS, "%d : comm error\n", prv);
        DTC_RBMS_Alarm(DTC_TBMS_RX_F, 1, 0);
    }
    req = 0;
}
static void makePeriodicCall(int job)
{
    if (stop_bit == 1)
    {
        return;
    }
    PushJob(RBMS_SCHEDULER_IDX, job, RBMS_EVT_TIME, NULL);
}

void Rack_Task(void *arg)
{
    int job;
    int count = 0;
    init_RackTask();
    rbms_run = 1;
    while (1)
    {
        SEM_pend(RBMS_SEM, SYS_FOREVER);
        job = PopJob(RBMS_SCHEDULER_IDX, (void **)&RBMS_Buf);
        DoJob(job);
        FreeMsg(RBMS_Buf);
        if (count++ > 200)
        {
            count = 0;
            //RackSys_PrintOut();
        }
    }

}

static void DoJob(int JobIdx)
{
    //static int buzzer_test = 0;
    static int wd_t10ms = 0;
    //
    if (wd_t10ms++ > 30)
    {
        BSP_WDGPIN_TOGGLE();
        wd_t10ms = 0;
    }

    switch (JobIdx)
    {
        case RBMS_RELAYCONTROL:
            if (diag_relay_mode == 0)
            {
                RELAY_CONTROL(RLY_CTL.RELAY_MODE);
            }
            makePeriodicCall(JobIdx);
            break;
        case RBMS_SYS_POWERDOWN:
            //EEPROM 부분 동작 중이면..중지
            stop_bit = 1;
            PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_CMD_POWERDOWN, 0, NULL);
            break;
        case RBMS_UPDATE_HOST_DATA:
            RackSys_UpdateStringModel();
            Get_PackVoltage();
            makePeriodicCall(JobIdx);
            break;
        case RBMS_SEND_REQTRAY:
            R20ms_ReqTrayData();
            makePeriodicCall(JobIdx);
            break;
        case RBMS_RECV_RSPTRAY:
            R20ms_RspTrayData();
            break;
        case RBMS_COMM_CHECK:
            R20ms_CommErrCheck();
            break;
        case RBMS_DIAG_UPDATE:
            Diag_Update();
            makePeriodicCall(JobIdx);
            break;
        case RBMS_SOC:
            CAL_SOC();
            RackSt_UpdateStAndPeriodSave(0);
            makePeriodicCall(JobIdx);
            break;
        case RBMS_BLOWOFF_FUSE:
            PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_CMD_BLOW_FUSE, 0, NULL);
            break;
        case RBMS_FULLCHARGE:
            //SOC Recal
            //PackState Update
            RackSt_SaveCurrentStAtOnce(F_FULLC);
            CAL_ReCalSOC();
            break;
        case RBMS_FULLDIS:
            RackSt_SaveCurrentStAtOnce(F_FULLD);
            CAL_ReCalSOC();
            break;
        case RBMS_BALANCING_CHK:
            runAutoBalancingProcess();
            PushJob(RBMS_SCHEDULER_IDX, RBMS_BALANCING_CHK, 500, NULL);
            break;
        case RBMS_WATCHDOG_CHK:
            sm_watchdog_monitor();
            PushJob(RBMS_SCHEDULER_IDX, RBMS_WATCHDOG_CHK, 500, NULL);
            break;
        case RBMS_GET_TEMP:
            GET_EXT_TEMPERATURE();
            PushJob(RBMS_SCHEDULER_IDX, RBMS_GET_TEMP, 100, NULL);
            break;
    }

}


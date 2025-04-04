/**
 ******************************************************************************
 *     @file         BMIC_Task.c
 *     @version
 *     @author
 *     @date      2024/09/18
 *     @brief
 ******************************************************************************
*/
#include "main.h"
#include "obshal.h"
#include "hos.h"
#include "BMS.h"
#include "BMS_OS.h"
#include "BMIC_ProcessData.h"
#include "oMbMaster.h"
#include "AppQueue.h"
#include "SubmicProcessData.h"
#include "cellbalancing.h"
#include <stdio.h>

/*== MACRO ===================================================================*/
/*== GLOBAL VARIABLE =========================================================*/
int32_t mb_fd1;
uint8_t submic_run;
uint8_t bmic_pwrdown;
uint8_t bmic_balsw_on;
uint8_t bmic_blowoff_fuse;

/*== STATIC VARIABLE =========================================================*/
static uint32_t *Bmic_Buf;
static uint32_t *Client_Buf;
static int expire_count;

/*== FUNCTION DECLARATION ====================================================*/
static void DoJob(int JobIdx);
static void DoSubJob(int JobIdx);

/*== FUNCTION DEFINITION =====================================================*/

void SUBMIC_TASK(void *arg);


PUBLIC void Variable_Init(void)
{
    uint8_t id;

    if (BMIC_DEVICE_COUNT > TRAY_DEFAULT_BMIC_COUNT)
    {
        tray_fault |= TRAY_ERR_EEPCONFIG;
        return;
    }
    if (TRAY_NUMS > TBMS_BUF_SIZE )
    {
        tray_fault |= TRAY_ERR_EEPCONFIG;
        return;
    }
    if (MODULE_TEMP_SENSORNUM > TRAY_DEFAULT_TEMP_SENSORNUM)
    {
        tray_fault |= TRAY_ERR_EEPCONFIG;
        return;
    }
    if (MODULE_BDTEMP_SENSORNUM > TRAY_DEFAULT_BDTEMP_SENSORNUM)
    {
        tray_fault |= TRAY_ERR_EEPCONFIG;
        return;
    }
    if (TRAY_CELL_NUMS > TRAY_DEFAULT_CELL_NUMS)
    {
        tray_fault |= TRAY_ERR_EEPCONFIG;
        return;
    }

	 //-----------------BMIC_ProcessData.c
	Init_Tray();

	 //-----------------Fan.c
	 //Init_Fan();
	for (id = 0; id < TRAY_NUMS; id++)
    {
	 //-----------------ExternalTempSensing.c
	    Init_TempSensor(id);
	 //-----------------CellBalancing.c
    }
    Init_Balancing();

	 //-----CAN_LineTest.c
	 //Init_LineTest();
     //Init_CanPCInterface();

}

void SpecialCommandCheck(void)
{
	if (BalancingCMD == BALANCING_START_CMD)
	{
		PushJob(BMIC_SCHDEULER_IDX, CELL_BALANCING_START_EVENT, BMICSCH_TIME, NULL);
	}
    if (BalancingCMD == BALANCING_STOP_CMD)
    {
		PushJob(BMIC_SCHDEULER_IDX, CELL_BALANCING_STOP_EVENT, BMICSCH_TIME, NULL);
    }
}

PUBLIC void Init_BMIC()
{
#if 0
    hOSThrdAttr_t attr;
    hOsSetThreadAttr(attr, "SUBMIC", H_OSTHRD_PRI_NORMAL, 0);
    hOsThreadCreate(&attr, SUBMIC_TASK, NULL);
#endif
    submic_run = 0;
    SUBMIC_Scheduler.priorty = (int32_t)H_OSTHRD_PRI_NORMAL;
    hOsSetThreadName((char*) SUBMIC_Scheduler, "BM_SCH");
    hOsCreateThread(&SUBMIC_Scheduler, (void *)&SUBMIC_TASK, NULL);
    mb_fd1 = oMbMasterOpen(OMB_RTU, "COM3", 115200, NULL);
    while (!submic_run)
    {
        hSysWait(1);
    }
    PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_INIT_VAR, 0, NULL);
}

uint64_t Evt_time =0 ;
//--- Pop BMIC job from BMIC scheduler
PUBLIC void BMIC_Task(void *arg)
{
    int job;
    Init_BMIC();
    bmic_run = 1;
    while (1)
    {
        SEM_pend(BMIC_SEM, SYS_FOREVER);
        job = PopJob(BMIC_SCHDEULER_IDX, (void **)&Bmic_Buf);
        //set_time();
        DoJob(job);
        //chk_time(job);
        FreeMsg(Bmic_Buf);
        if (tray_fault)
        {
            PushJob(BMIC_SCHDEULER_IDX, INIT_DAISYCHAIN, BMICSCH_TIME, NULL);
        }
        else
        {
            if (Evt_time == 0 )
            {
                Evt_time = hTimCurrentTime() + 100;
            }
            else
            {
                if (hTimCurrentTime() > Evt_time)
                {
                    PushJob(BMIC_SCHDEULER_IDX, EACH_VOLTAGE_READ_ALL, BMICSCH_TIME, NULL);
                    PushJob(BMIC_SCHDEULER_IDX, MODULE_BLOCKVOLTAGE_N_TEMPERATURE_READ, BMICSCH_TIME, NULL);
                    PushJob(BMIC_SCHDEULER_IDX, COMM_TRAY_TO_RBMS, BMICSCH_TIME,NULL);
                    PushJob(BMIC_SCHDEULER_IDX, SUMMARY_AND_ALERT_READ_1, BMICSCH_TIME,NULL);
                    Evt_time = 0;
                    if (on_balancing)
                    {
                        PushJob(BMIC_SCHDEULER_IDX, CELL_BALANCING_EVENT, BMICSCH_TIME, NULL);
                    }
                }
            }

        }
        SpecialCommandCheck();
    }
}


void SUBMIC_TASK(void *arg)
{
    int job;
    submic_run = 1;
    while (1)
    {
        SEM_pend(SUBMIC_SEM, SYS_FOREVER);
        job = PopJob(SUBMIC_SCHEDULER_IDX, (void **)&Client_Buf);
        DoSubJob(job);
        FreeMsg(Client_Buf);

    }
}

static void DoJob(int JobIdx)
{
    int i;
    switch (JobIdx)
    {
        case INIT_DAISYCHAIN:
            if (ir.model.mode != ST_RUN)
            {
                hLogProc(LOG_BMIC, "Com Recovery... \n");
                //PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_INIT_DEVICE, 0, NULL);
            }
            else
            {
                tray_fault &= ~(TRAY_ERR_DAISYCHAIN | TRAY_ERR_BMIC | TRAY_ERR_DIAG);
                hLogProc(LOG_BMIC, "Submic Run\n");
            }
            break;
        case INTERNAL_DIAGNOSIS:
            break;
        case BMIC_SCAN_N_FANMODE_N_LEDFAULT:    //6~8ms
            hLogProc(LOG_BMIC, "scan n\n");
            break;
        case EACH_VOLTAGE_READ_ALL:
            hLogProc(LOG_BMIC, "cellvol read \n");
            BMIC_Read_Each_Voltage(0, ALL_CELLDATA, NOMAL_STORAGE);
            break;
        case MODULE_BLOCKVOLTAGE_N_TEMPERATURE_READ:    //5ms
            BMIC_Read_Block_V(0);
            BMIC_Read_Temperature(0);
            hLogProc(LOG_BMIC, "pack & temp \n");
            break;
        case SUMMARY_AND_ALERT_READ_1:  //5ms
            for (i = 0; i < TRAY_NUMS; i++)
            {
                Update_Voltage_MisMatch_Check(i);
            }
            break;
        case VOL_N_TEMP_EEPROM_UPDATE:  //1ms ?댄
            hLogProc(LOG_BMIC, "vol update\n");
            break;
        case CELL_BALANCING_START_EVENT:
            hLogProc(LOG_BMIC, "cell bal start\n");
            BalancingCMD = BALANCING_CMD_CLR;
            on_balancing = 1;
            PushJob(BMIC_SCHDEULER_IDX, CELL_BALANCING_EVENT, 100, NULL);
            break;
        case CELL_BALANCING_EVENT:
            hLogProc(LOG_BMIC, "cell bal on\n");
            // for (i = 0; i < MAX_COMM_NUMS; i++)
            // {
                PerformBalancing(i);
            // }
            // break;
        case CELL_BALANCING_STOP_EVENT:
            hLogProc(LOG_BMIC, "cell bal stop\n");
            BalancingCMD = BALANCING_CMD_CLR;
            on_balancing = 0;
           // for (i = 0; i < MAX_COMM_NUMS; i++)
           // {
           //     StopCellBalancing(i);
           // }
            break;
        case HW_DIAG_EXTERNAL_FET1:
            hLogProc(LOG_BMIC, "hw diag1\n");
            break;

#ifdef _RBMS
        case COMM_TRAY_TO_RBMS:     //1ms 이하
            if ((tray_fault & (TRAY_ERR_DAISYCHAIN | TRAY_ERR_DIAG)) == 0)
            {
                BMIC_ReadDeviceIDFromRBMS();
                BMIC_ReadCellVolFromRBMS();
                BMIC_ReadModTmpFromRBMS();
                BMIC_ReadTrayInterLockFromRBMS();
                BMIC_ReadCellBalancingFromRBMS();
                BMIC_ReadTrayFaultFromRBMS();
                PushJob(RBMS_SCHEDULER_IDX, RBMS_RECV_RSPTRAY, 0, NULL);
            }
            break;
#endif
        case CELL_BALANCING_READ_EVENT:
            //BMIC_BALSW_Read(0);
            PushJob(BMIC_SCHDEULER_IDX, CELL_BALANCING_READ_EVENT, 100, NULL);
            break;
        default:
            break;
    }
}

static void DoSubJob(int JobIdx)
{
    static int count = 0;
    if (JobIdx != NULL)
    {
        count = 0;
    }
    switch (JobIdx)
    {
        case NULL:
            count ++;
            if (count > 3*40 )
            {    //4초 이상
                PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_INIT_DEVICE, 0, NULL) ; // Event 없이 4초 이상 지나면 ...
                printf("error case.................\n");
                count = 0;
            }
            break;
        case SUBMIC_INIT_VAR:
            hLogProc(LOG_BMIC, "sub init variable\n");
            Submic_ClrRegisters();
        case SUBMIC_INIT_DEVICE:
            if (Submic_GetData(0) == TRUE)
            {
                hLogProc(LOG_RENE , "sub init device\n");
                Submic_initDevice(0);
                tray_fault &= ~TRAY_ERR_DAISYCHAIN;
            }
            else
            {
                hLogProc(LOG_RENE , "sub init no reply\n");
                PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_INIT_DEVICE, 500, NULL); //500ms 마다 Device가 응답하는지
                tray_fault |= TRAY_ERR_DAISYCHAIN;
                // 확인
            }
            break;
        case SUBMIC_CONFIG_WRITE:
            hLogProc(LOG_RENE, "sub config\n");
            assert_param(0); // 구현 안되어 있음.
            expire_count=0;
            break;
        case SUBMIC_POLLING:
            if (Submic_GetData(0) == TRUE)
            {
                expire_count = 0;
                Submic_Polling();
                hLogProc(LOG_RENE, "polling ok..... \n");
            }
            else
            {  // RENESAS로 부터 Modbus 통신이 Error 인 경우
                hLogProc(LOG_RENE , "polling err..... \n");
                expire_count++;
                if (expire_count > (int)(SUBMCU_TIMEOUT_TIME / SUBMIC_EVT_TIME))
                {
                    //sm_submcu_timeout();
                    hLogProc(LOG_RENE, "submic reset..... \n");
                    Submic_Reset(0);
                }
                else
                {
                    PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_POLLING, SUBMIC_EVT_TIME, NULL);
                }
            }
            break;
        case SUBMIC_CAL_MSRREQ:
            assert_param(0); // 구현 안되어 있음.
        break;
        case SUBMIC_CMD_POWERDOWN:
            bmic_pwrdown = 1;
            Submic_Send_PowerDownCmd();
            break;
        case SUBMIC_CMD_BLOW_FUSE:
            bmic_blowoff_fuse = 1;
            Submic_Send_FuseOffCmd();
            break;
        case SUBMIC_CMD_CELLBAL:
            Submic_Send_CellBalCmd();
            break;
    }
}


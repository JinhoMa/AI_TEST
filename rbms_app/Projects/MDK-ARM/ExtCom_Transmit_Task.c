/**
 ******************************************************************************
 *     @file         ExtCom_Transmit_Task.c
 *     @version
 *     @author
 *     @date      2024/09/30
 *     @brief
 ******************************************************************************
 */

#include "AppQueue.h"
#include "BMS.h"
#include "BMS_OS.h"
#include "Master.h"
#include "hos.h"
#include "main.h"
#include "obshal.h"
#include "RS485_Data_Processor.h"

/*== MACRO ===================================================================*/
/*== TYPE DEFINITION =========================================================*/

/*== GLOBAL VARIABLE =========================================================*/
/*== STATIC VARIABLE =========================================================*/
static uint32_t *ExtComTrans_buf;

/*== FUNCTION DECLARATION ====================================================*/
static void DoJob(int JobIdx);
static void rs485_send(void);
static void check_pwr_button(void);
static void makePeriodicCall(int job);

/*== FUNCTION DEFINITION =====================================================*/
static void rs485_rd_process(void)
{
    rs485_rdpacket_process();
}


static void rs485_send(void)
{
    rs485_wrpacket_process();
}

static void check_pwr_button(void)
{
    if (ir.model.pon == 1)
    {
        // RS485 command
    }
}
static void makePeriodicCall(int job)
{

    PushJob(EXTCOM_SCHEDULER_IDX, job, EXTCOM_EVT_TIME, NULL);
}

//-------------3ms Task For CAN_B Response
void ExtCom_Scheduler_Task(void *arg)
{
    int job;
    RS485Init();
    PushJob(EXTCOM_SCHEDULER_IDX, EXTCOM_RS485_READ, EXTCOM_EVT_TIME, NULL);
    PushJob(EXTCOM_SCHEDULER_IDX, EXTCOM_PWR_BUTTON, EXTCOM_EVT_TIME, NULL);
    while (1)
    {
        SEM_pend(EXTCOM_SCHEDULER_SEM, SYS_FOREVER);
        job = PopJob(EXTCOM_SCHEDULER_IDX, (void **)&ExtComTrans_buf);
        DoJob(job);
        FreeMsg(ExtComTrans_buf);
    }
}

static void DoJob(int JobIdx)
{
    switch (JobIdx)
    {
        case EXTCOM_RS485_READ:
            rs485_rd_process();
            makePeriodicCall(JobIdx);
            break;
        case EXTCOM_RS485_OUT:
            rs485_send();
            // makePeriodicCall(JobIdx);
            break;
        case EXTCOM_BLE_OUT:
            break;
        case EXTCOM_BLE_READ:
            break;
        case EXTCOM_PWR_BUTTON:
            check_pwr_button();
            makePeriodicCall(JobIdx);
            break;
    }
}

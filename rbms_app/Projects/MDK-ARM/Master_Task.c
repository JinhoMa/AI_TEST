/**
 ******************************************************************************
 *     @file         Master_Task.c
 *     @version
 *     @author
 *     @date      2024/09/23
 *     @brief
 ******************************************************************************
*/
#include "main.h"
#include "obshal.h"
#include "BMS.h"
#include "BMS_OS.h"
#include "master.h"
static void DoJob(int JobIdx);
static uint32_t *MBMS_Buf;

static void init_MasterTask(void)
{
    master_init();
    PushJob(MBMS_SCHEDULER_IDX,MBMS_MODE_CHECK, 5000, 0);
    PushJob(MBMS_SCHEDULER_IDX,MBMS_FAULT_CHECK, 5000, 0);
}
void Master_Task(void *arg)
{
    int job;
    //int count = 0;
    init_MasterTask();
    while (1)
    {
        SEM_pend(MBMS_SEM, SYS_FOREVER);
        job = PopJob(MBMS_SCHEDULER_IDX, (void **)&MBMS_Buf);
        DoJob(job);
        FreeMsg(MBMS_Buf);
    }
}

static void makePeriodicCall(int job)
{
    PushJob(MBMS_SCHEDULER_IDX, job, MMBS_EVT_TIME, NULL);
}


static void DoJob(int JobIdx)
{
    switch (JobIdx)
    {
        case MBMS_MODE_CHECK:
            master_mode_check();
            makePeriodicCall(JobIdx);
            break;
        case MBMS_FAULT_CHECK:
            master_fault_check();
            makePeriodicCall(JobIdx);
            break;
    }
}


/**
    ******************************************************************************
    *     @file         SKQueue.c
    *     @version   
    *     @author    
    *     @date      2017/09/12 
    *     @brief     TASK의 EVENT QUEUE  API
    ******************************************************************************
    */




//#include "Sk_Config.h"
//#include "CAN_LineTest.h"
//#include "EEPROM_TASK.h"
#include "AppQueue.h"

//#include "BMIC_APP.h"
//#include "BMIC_ProcessData.h"
//#include "CellBalancing.h"
//#include "EEprom_Control.h"
//#include "EEprom_ProcessData.h"
//#include "BMIC_API.h"//test
//#include "BMIC_RegisterNCommand.h"
//#include "Can_Transmit_Task.h"
//#include "CAN_RackBMS.h"
//#include "Can_Developer.h"
//#include "Operating_Condition.h"
//#include "BMIC_Diagnosis.h"
//#include "CAN_Simulation.h"

#include <string.h>

// --- BMIC,  Scheduler
//STR_SchedulerModel Scheduler[SCHEDULER_NUMBER];


SchedulerModel Scheduler[SCHEDULER_NUMBER];

#define DEBUG   1

static JOB * allocJob(int sidx)
{
    int i;
    JOB *j = 0;
    for (i = 0; i < JOB_QUEUE_SIZE; i++)
    {
#if DEBUG
        if (sidx == BMIC_SCHDEULER_IDX)
        {
            if (Scheduler[BMIC_SCHDEULER_IDX].job[i].used == NULL)
            {
                j = &Scheduler[BMIC_SCHDEULER_IDX].job[i];
                j->next = NULL;
                j->used = 1;
                Scheduler[sidx].usedCnt++;
                return j;
            }
        }
        else
#endif
        if (Scheduler[sidx].job[i].used == NULL)
        {
            j = &Scheduler[sidx].job[i];
            j->next = NULL;
            j->used = 1;
            Scheduler[sidx].usedCnt++;
            return j;
        }
    }
    return NULL;
}
static void freeJob(int sidx, void *job)
{
    JOB *j ;
    j = (JOB *)job;
    j->used = 0;
    Scheduler[sidx].usedCnt--;
}

static void enQueue(int sidx, void *job, uint64_t time)
{
    JOB *j = job;
    JOB **wp;
    for (wp = &Scheduler[sidx].joblist; *wp != NULL; wp = &((*wp)->next))
    {
        if ((*wp)->sch_time > time)
        {
            break;
        }
    }
    j->next = *wp;
    *wp = j;
}

static void deQueue(int sidx, JOB **j)
{
    JOB** wp;
	for (wp = &Scheduler[sidx].joblist; *wp != NULL; wp = &((*wp)->next))
    {
        *j = *wp;
        *wp = (*wp)->next;
        return;
    }
    *j = NULL;
}

/**
  * @brief Scheduler Queue를 초기화 한다.
  */
void Init_Scheduler(void)
{
    uint16_t index=0;
    uint16_t JobIndex=0;
    for (index = 0; index < SCHEDULER_NUMBER; index++)
    {
        for (JobIndex = 0; JobIndex < JOB_QUEUE_SIZE; JobIndex++)
        {
            Scheduler[index].job[JobIndex].used = 0;
        }
    }
}


PUBLIC int32_t IsFreeMsg(void *msg)
{
    MH *p = (MH *)msg;
    if (p->used == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

PUBLIC void ReserveMsg(void *msg, uint8_t who)
{
    MH *p = (MH *)msg;
    p->who = who;
    p->used = 1;
}

PUBLIC void CleanUpMsg(void *msg)
{
    MH *p = (MH *)msg;
    p->used = 0;
}

PUBLIC void FreeMsg(void *msg)
{
    MH *p = (MH *)msg;
    if (p == NULL) return;
    p->used = 0;
}

void PushJob(uint16_t SchedulerIdx, uint16_t Job, uint32_t tim, void* buf)
{
    JOB *job;
#if 0
    if (Scheduler[BMIC_SCHDEULER_IDX].job[Job].used)
    {
        return;
    }
    hOsLock();
    job = &Scheduler[BMIC_SCHDEULER_IDX].job[Job];
    job->idx = Job;
    job->sch_time = hTimCurrentTime()+tim;
    job->data = buf;
    job->used = 1;
    Scheduler[BMIC_SCHDEULER_IDX].usedCnt++;
    enQueue(BMIC_SCHDEULER_IDX, job, job->sch_time);
    hOsUnlock();
#else
    if (SchedulerIdx == BMIC_SCHDEULER_IDX)
    {
        if (Scheduler[BMIC_SCHDEULER_IDX].job[Job].used)
        {
            return;
        }
        hOsLock();
        job = &Scheduler[BMIC_SCHDEULER_IDX].job[Job];
        job->idx = Job;
        job->sch_time = hTimCurrentTime()+tim;
        job->data = buf;
        job->used = 1;
        Scheduler[BMIC_SCHDEULER_IDX].usedCnt++;
        enQueue(BMIC_SCHDEULER_IDX, job, job->sch_time);
        hOsUnlock();
    }
    else
    {
        hOsLock();
        job = allocJob(SchedulerIdx);
        if (job == NULL)
        {
            hLogProc(LOG_ERROR, "[%d:%d]job alloc NULL\n", SchedulerIdx,Job);
            hOsUnlock();
            return;
        }
        job->idx = Job;
        job->sch_time = hTimCurrentTime()+tim;
        job->data = buf;
        enQueue(SchedulerIdx, job, job->sch_time);
        hOsUnlock();
    }
#endif
}

int32_t PopJob(uint16_t SchedulerIdx, void **buf)
{
    JOB *job, *q;
    int jobIdx;
	uint64_t tim = hTimCurrentTime();
    hOsLock();
#if DEBUG
    if (SchedulerIdx ==BMIC_SCHDEULER_IDX)
    {
        q = Scheduler[BMIC_SCHDEULER_IDX].joblist;
    }
    else
#endif
    {
        q= Scheduler[SchedulerIdx].joblist;
    }
    if (q == NULL || (tim< q->sch_time))
    {
        hOsUnlock();
        return 0;
    }
    deQueue(SchedulerIdx, &job);
    if (job == NULL)
    {
        hOsUnlock();
        return 0;
    }
    jobIdx = job->idx;
    *buf = job->data;
    freeJob(SchedulerIdx, job);
    hOsUnlock();
    return jobIdx;
}





















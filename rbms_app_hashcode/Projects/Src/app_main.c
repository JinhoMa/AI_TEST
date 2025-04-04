/*
 * BMS Application
 */

#include "obshal.h"
//#include "hos.h"
//#include "hserial.h"
#include "bms.h"
#include "BMS_OS.h"
#include "app_main.h"
#include "SYS_ERR_CHK.h"
#include "sm.h"
#include <stdio.h>
/*== MACRO ===================================================================*/
/*== GLOBAL VARIABLE =========================================================*/


uint32_t SystemBaseCounter = 0;
//////////////////////////////////////////////////////////////////////////////////
//                 io resource
/////////////////////////////////////////////////////////////////////////////////
static int32_t timer_id;
static uint8_t start = 0;
const uint16_t _flashId = 0xA55A;

//////////////////////////////////////////////////////////////////////////////////
//                 os  resource
/////////////////////////////////////////////////////////////////////////////////
hOsSemaId_t EEPROM_SEM;
hOsSemaId_t CANB_SCHEDULER_SEM;
hOsSemaId_t EXTCOM_SCHEDULER_SEM;
hOsSemaId_t BMIC_SEM;
hOsSemaId_t SUBMIC_SEM;
hOsSemaId_t RBMS_SEM;
hOsSemaId_t MBMS_SEM;


hOSThread BootTh;
hOSThread BMIC_Scheduler;
hOSThread ExtCom_Scheduler;
hOSThread SUBMIC_Scheduler;
hOSThread EEprom;
hOSThread CanB_Scheduler;
hOSThread RackTh;
hOSThread MasterTh;

hOsThrdId_t boot_tid;
uint8_t eeprom_run ;
uint8_t canb_run ;
uint8_t bmic_run;
uint8_t rbms_run;
uint8_t mbms_run;

/*== STATIC VARIABLE =========================================================*/
const char *frame = "***************************************************************************************\n";
const char *title = "**                     run Application\n";
const char *model = "**                     model name: %s\n";
const char *date  = "**                     compile date : %s\n";


/*== FUNCTION DECLARATION ====================================================*/
void BootThread(void const *argument);
void timerHandler(void);

/*== FUNCTION DEFINITION =====================================================*/
void bootPrint(void)
{
    printf(frame);
    printf(title);
    printf(frame);
}

void SEM_post(hOsSemaId_t sm)
{
    hOsBiSemaSignal(sm);
}

void SEM_pend(hOsSemaId_t sm, int mode)
{
    hOsBiSemaWait(sm, -1);
}

static void initBmsSignal(void)
{
    EEPROM_SEM = hOsBiSemaCreate();
    assert_param( EEPROM_SEM != 0);
    CANB_SCHEDULER_SEM = hOsBiSemaCreate();
    assert_param( CANB_SCHEDULER_SEM != 0);
    EXTCOM_SCHEDULER_SEM = hOsBiSemaCreate();
    assert_param( EXTCOM_SCHEDULER_SEM != 0);
#ifdef _TBMS
    BMIC_SEM = hOsBiSemaCreate();
    assert_param( BMIC_SEM != 0);
    SUBMIC_SEM = hOsBiSemaCreate();
    assert_param( SUBMIC_SEM != 0);
#endif
#ifdef _RBMS
    RBMS_SEM = hOsBiSemaCreate();
    assert_param( RBMS_SEM != 0);
#endif
#ifdef _MBMS
    MBMS_SEM = hOsBiSemaCreate();
    assert_param( MBMS_SEM != 0);
#endif
}

extern UART_HandleTypeDef huart1;
extern void board_test(void);

void app_main(void)
{

    int fd;
    hSysInit();
    hOsInit();
    timer_id = hTimCreate();
    hTimSet(timer_id, 1, timerHandler);
    hTimStart(timer_id);

    //printf port
    fd = hSerOpen((char *)hSysProperty("shell.serial.port"), 115200, NULL);
    if(fd > 0){
        hSerCtrl(fd, SER_SET_DEBUG, 1);
    }
    fd = hSerOpen((char *)hSysProperty("rs485.port"), 9600, NULL);
    assert_param(fd != 0);
    //hSerCtrl(fd, SER_SET_RS485TXE, 1);
    diag_can_fd = hCanOpen("CANB", 500000, 1, 3, 2);
    assert_param(diag_can_fd!=0);
    hEprOpen();
    fd = hAdcOpen("ADC1", 1);
    assert_param(fd != 0);
    hAdcStart(fd);
    board_test();
#if 1
    hOsSetThreadName((char *)BootTh, "boot");
    BootTh.priorty = (int32_t) H_OSTHRD_PRI_LOW;
    boot_tid = hOsCreateThread(&BootTh, (void *)&BootThread , NULL);
    assert_param(boot_tid!=0);
#else
    BootThread(NULL);
#endif
    hOsRun();
}

#ifdef _TBMS
static void initTrayBmsTask(void)
{
    hOsThrdId_t id;

#if 0
    hOSThrdAttr_t attr;
    hOsSetThreadAttr(attr, "BMIC", H_OSTHRD_PRI_NORMAL, 0);
    hOsThreadCreate(&attr, BMIC_Task, NULL);
#else
    BMIC_Scheduler.priorty = (int32_t)H_OSTHRD_PRI_NORMAL;
    hOsSetThreadName((char*) BMIC_Scheduler, "CLNT");
    id = hOsCreateThread(&BMIC_Scheduler, (void *)&BMIC_Task, NULL);
    assert_param(id != 0);
    while (!bmic_run)
    {
        hSysWait(1);
    }
#endif
}
#endif

#ifdef _MBMS
__WEAK void Master_Task(void *arg)
{
    R.StringModel.SetEna = STRING_ENABLE;
    R.StringModel.SetCon = STRING_ENABLE;
    RLY_CTL.RELAY_MODE = ALL_MODE;
}
__WEAK void ExtCom_Scheduler_Task(void *arg)
{

}

#endif

#ifdef _RBMS
void initRBMS(void)
{
    InitRackSys();
    ClearDTCTable();
}

static void initRackBmsTask(void)
{
    hOsThrdId_t id;
    int error = 0;
#ifdef _RBMS
#if 1
    RackTh.priorty = (int32_t)H_OSTHRD_PRI_NORMAL;
    hOsSetThreadName((char*) RackTh, "Rack");
    id = hOsCreateThread(&RackTh, (void *)&Rack_Task, NULL);
    if (id == 0)
    {
        error = 1;
    }
    assert_param(error != 1);
    while (!rbms_run)
    {
        hSysWait(1);
    }
#endif
#endif
#ifdef _MBMS
    MasterTh.priorty = (int32_t) H_OSTHRD_PRI_NORMAL;
    hOsSetThreadName((char*) MasterTh, "MST");
    id = hOsCreateThread(&MasterTh, (void *)&Master_Task, NULL);
    if (id == 0)
    {
        error = 1;
    }
    assert_param(error != 1);

    ExtCom_Scheduler.priorty = (int32_t)H_OSTHRD_PRI_NORMAL;
    hOsSetThreadName((char*)ExtCom_Scheduler, "UART" );
    id = hOsCreateThread(&ExtCom_Scheduler, (void *)&ExtCom_Scheduler_Task, NULL);
    if (id == 0)
    {
        error = 1;
    }
    assert_param(error != 1);

    
#endif
}

#endif


void BootThread(void const *argument)
{
    int error;
    hOsThrdId_t id;
    //initVariable();
    initBmsSignal();

    //board_test();
    hSysWait(500);
    bootPrint();
    sm_init();
    // COMMON TASK
    Init_Scheduler();
    Init_EEPROM();

    EEprom.priorty = (int32_t)H_OSTHRD_PRI_NORMAL;
    hOsSetThreadName((char*)EEprom, "EEPR");
    id = hOsCreateThread(&EEprom, (void *)&EEprom_Task, NULL);
    if (id == 0)
    {
        error = 1;
    }
    while (!eeprom_run)
    {
        hSysWait(1);
    }
    CanB_Scheduler.priorty = (int32_t)H_OSTHRD_PRI_NORMAL;
    hOsSetThreadName((char*)CanB_Scheduler, "CAN_B" );
    id = hOsCreateThread(&CanB_Scheduler, (void *)&CanB_Scheduler_Task, NULL);
    if (id == 0)
    {
        error = 1;
    }
    assert_param(error != 1);
    while (!canb_run)
    {
        hSysWait(1);
    }
#ifdef _TBMS
    Variable_Init();
    initTrayBmsTask();
#endif

#ifdef _RBMS
    initRBMS();
    initRackBmsTask();
#endif
    start = 1;
    hOsSuspendThread();
}

void timerHandler(void)
{
    HAL_IncTick();

  /* Scheduler Base Counter */
    SystemBaseCounter++;
    if (!start)
    {
        return;
    }
#if 1
    if (1 == (SystemBaseCounter % 3))
    {
        SEM_post(CANB_SCHEDULER_SEM);
    }

    if (5 == (SystemBaseCounter % 10))
    {
        SEM_post(EEPROM_SEM);
    }
    if (15 == (SystemBaseCounter % 100))
    {
        SEM_post(EXTCOM_SCHEDULER_SEM);
    }
#ifdef _TBMS
  /* 100ms Task scheduler */
    if (7 == (SystemBaseCounter % 100))
    {
        if (!EEPROMData.access)
        {
            SEM_post(BMIC_SEM);
        }
    }
    if (20 == (SystemBaseCounter % 30))
    {
        if (!EEPROMData.access)
        {
            SEM_post(SUBMIC_SEM);
        }
    }
#endif
#ifdef _RBMS
    if (4 == (SystemBaseCounter % 5))
    {
        if (!EEPROMData.access)
        {
            SEM_post(RBMS_SEM);
        }
    }
#endif
#ifdef _MBMS
    if (2 == (SystemBaseCounter % 10))
    {
        if (!EEPROMData.access)
        {
            SEM_post(MBMS_SEM);
        }
    }
#endif

#else


    if(0 == (SystemBaseCounter % 5))
    {
        if (!EEPROMData.access)
        {
            SEM_post(RBMS_10MS_SEM);
        }
    }
    else if (1 == (SystemBaseCounter % 2))
    {
        if (!EEPROMData.access)
        {
            SEM_post(SCHEDULER_SEM);
        }
    }
    else if(3 == (SystemBaseCounter % 5))
    {
        SEM_post(&NET_SCHEDULER_SEM);
    }

  /* 100ms Task scheduler */
    if (7 == (SystemBaseCounter % 100))
    {
        if (!EEPROMData.access)
        {
            SEM_post(RBMS_100MS_SEM);
        }
    }

    if (50 == (SystemBaseCounter % 100))
    {
        if (!EEPROMData.access)
        {
            SEM_post(RBMS_500MS_SEM);
        }
    }

    if (20 == (SystemBaseCounter % 30))
    {
        if (!EEPROMData.access)
        {
            SEM_post(RBMS_SUBMIC_SEM);
        }
    }

    if (5 == (SystemBaseCounter % 30))
    {
        if (!EEPROMData.access)
        {
            SEM_post(RBMS_SUBMIC2_SEM);
        }
    }

    if (100 == (SystemBaseCounter % 300))
    {
        if (!EEPROMData.access)
        {
        SEM_post(BMIC_PUSH_SEM);
        }
    }
#endif
}


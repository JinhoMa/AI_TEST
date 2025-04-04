/**
   ******************************************************************************
   *     @file         sm.c
   *     @version
   *     @author
   *     @date       2021/09/07
   *     @brief       Functional Safety Mechanism Implement
   ******************************************************************************
   */
#include <stdint.h>
#include "obshal.h"
#include "BMS.h"
#include "main.h"
#include "BMS_OS.h"
#include "SYS_ERR_CHK.h"
#include "sm_inc/stm32xx_STLparam.h"
#include "sm_inc/stm32xx_STLclock.h"

/*== MACRO ===================================================================*/
#if defined ( __CC_ARM   )
uint32_t sm_memmarchtest_addr __attribute__((at(0x20000000)));/* memory marchtest address */
#endif

#define TEST_MEM_STARTADDR        0x20000010
#define TEST_MEM_SIZE             0xFFE0

/*== GLOBAL VARIABLE =========================================================*/
/*== STATIC VARIABLE =========================================================*/
static uint8_t keepalive_event ;

/////////////////////////////////////////////////////////////
// submcu wathdog
// submcu 디버깅중에는 enable을끄도록 한다.
/////////////////////////////////////////////////////////////
static uint8_t submcu_watchdog_enable;
/////////////////////////////////////////////////////////////
// submcu uart 통신 timeout
// submcu 디버깅중에는 enable을끄도록 한다.
/////////////////////////////////////////////////////////////
static uint8_t submcu_comtimeout_enable;
static uint8_t eeprom_check_enable;
/////////////////////////////////////////////////////////////
// pack 전압 과 cell 전압 차이 비교
// 디버깅 환경에서는 끄도록 한다.
/////////////////////////////////////////////////////////////
static uint8_t sm_voltagePlausiblity_enable;

/*== FUNCTION DECLARATION ====================================================*/
static  void sm_eeprom_error(void);

/*== FUNCTION DEFINITION =====================================================*/

uint32_t CtrlFlowCnt;
uint32_t CtrlFlowCntInv;
const uint32_t conAA = 0xAAAAAAAA;
const uint32_t con55 = 0x55555555;
void sm_DoRunTimeChecks(void);
void sm_memory_marchtest(void);
int STL_RunTimeCPUTest(void);

PUBLIC void sm_init(void)
{
    submcu_watchdog_enable = 1;
    submcu_comtimeout_enable = 1;
    eeprom_check_enable = 1;
    sm_voltagePlausiblity_enable = 0;
    sm_memmarchtest_addr = (uint32_t)TEST_MEM_STARTADDR;
}

void FailSafePOR(void)
{
    int i;
    BSP_CFET_DISABLE();
    BSP_DFET_DISABLE();
    while (1)
    {
        hLedOn(1);
        for (i = 0; i < 50000000; i++);
        hLedOff(1);
        for (i = 0; i < 50000000; i++);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)
{
    keepalive_event = 1;
}

//500ms 한번 호출 하도록 구현한다.
PUBLIC void sm_watchdog_monitor(void)
{
    static uint64_t keepalive_time = 0;
    static uint64_t mcutest_time = 0;
    static int first = 0;
    uint64_t t = hTimCurrentTime();
    //3sec 마다 체크
    if ((t - keepalive_time) > 30000)
    {
        if (keepalive_event)
        {
            keepalive_event = 0;
        }
        else
        {
           // 체크 주기 내에 watdog 이 update 되면 relay가 유지 되나 update 되지 않으면 relay를 off 한다.
            if (submcu_watchdog_enable)
            {
                //DTC_RBMS_Alarm(DTC_SM_F, 1, SM_F_SUBMCU_WATCHDOG);
                BSP_RAJ_RESET_ON();
            }
            else
            {
                DTC_RBMS_Alarm(DTC_SM_F, 0, SM_F_SUBMCU_WATCHDOG);
            }
        }
        keepalive_time = t;
    }
    if ((t - mcutest_time) > 60000)
    {
        sm_DoRunTimeChecks();   //1분에 1번 테스트
        mcutest_time = t;
    }
    if (first == 0)
    {
        sm_eeprom_error();
        first = 1;
    }
    sm_memory_marchtest();
}

PUBLIC void sm_submcu_timeout(void)
{
    if (submcu_comtimeout_enable)
    {
        DTC_RBMS_Alarm(DTC_SM_F, 1, SM_F_SUBMCU_TIMEOUT);
    }
    else
    {
        DTC_RBMS_Alarm(DTC_SM_F, 0, SM_F_SUBMCU_TIMEOUT);
    }
}

void sm_DoRunTimeChecks(void)
{
    int ret = STL_RunTimeCPUTest();
    if (ret != STL_SUCCESS)
    {
        FailSafePOR();
    }

}

uint32_t backup_addr;
PUBLIC void sm_memory_marchtest(void)
{
    uint32_t backup ;
    hSysLock();
    backup = *((uint32_t *)sm_memmarchtest_addr);
    backup_addr = sm_memmarchtest_addr;
    *((uint32_t *)sm_memmarchtest_addr) = 0xAAAAAAAA;
    if (*((uint32_t *)sm_memmarchtest_addr) != 0xAAAAAAAA)
    {
        goto failmode;
    }
    *((uint32_t *)sm_memmarchtest_addr) = 0x55555555;
    if (*((uint32_t *)sm_memmarchtest_addr) != 0x55555555)
    {
        goto failmode;
    }
    *((uint32_t *)sm_memmarchtest_addr) = backup;
    hSysUnlock();
    sm_memmarchtest_addr += 4;
    if (sm_memmarchtest_addr >= (uint32_t)(TEST_MEM_STARTADDR + TEST_MEM_SIZE))
    {
        sm_memmarchtest_addr = (uint32_t)TEST_MEM_STARTADDR;
    }
    return;
failmode:
    hSysUnlock();
    FailSafePOR();
}


static  void sm_eeprom_error(void)
{
    if (eeprom_check_enable == 1)
    {
        if (EEPROMData.chksumerr == 1)
        {
            DTC_RBMS_Alarm(DTC_SM_F, 1, SM_F_EEPROM_CHKSUM);
        }
        else
        {
            DTC_RBMS_Alarm(DTC_SM_F, 0, SM_F_EEPROM_CHKSUM);
        }
    }
}

PUBLIC void sm_mcufault_handler(void)
{
    DTC_RBMS_Alarm(DTC_SM_F, 1, SM_F_MCUFAULT);
    FailSafePOR();
}

PUBLIC void sm_voltagePlausiblity_error(void)
{
    if (sm_voltagePlausiblity_enable)
    {
        DTC_RBMS_Alarm(DTC_SM_F, 1, SM_F_VOLPLB_ERROR);
    }
    else
    {
        DTC_RBMS_Alarm(DTC_SM_F, 0, SM_F_VOLPLB_ERROR);
    }
}

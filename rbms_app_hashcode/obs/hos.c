/**
  ******************************************************************************
  * @file    hos.c
  * @author  SKI ESS Team
  * @version V1.0
  * @date    2014.11.5
  * @brief   HAL OS 모듈
  ******************************************************************************
 */

#include <string.h>
#include "obshal.h"
#include "htypes.h"
#include "main.h"


#ifdef __CMSIS_RTOS
#include "cmsis_os.h"
#endif

#ifdef __FREERTOS__
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Semphr.h"
#include "portmacro.h"

#define portVECTACTIVE_MASK					( 0x1FUL )

#endif

/** @addtogroup HAL
  * @{
  */
  
  /**
      * @brief 이 소스는 Embedded RTOS 를 포팅한 소스
      * RTOS의 기능중에서 BMS에 필요한 자원만 사용함.
      */
  /** @addtogroup HAL_Os
    * @{
    */

        /** @defgroup HAL_OS_Private_Defines
          * @{
          */
#define THREAD_NUMS 20
#define SEMAPHORE_NUMS 20
#define THREAD_SIGNAL   0x1000
typedef void (*FUNCP)(void)  ;

        /**
          * @}
          */
          
        /** @defgroup HAL_OS_Private_Structures
          * @{
          */
typedef struct _THDMGR{
    hOSThread *thread;    //MBMS thread id
    osThreadDef_t th;       //Platform OS Thread Param
    uint32_t tid;           //thread id
    uint32_t mid;         // mutext id;
    int32_t use;          // USE Flag
} THDMGR;

typedef struct _SEMGR{
    int *smp;
    uint32_t sb[2];   //Platfrom Semaphore Depend
    int32_t sid;        //Semaphore id
    int32_t use;
    uint8_t cnt;
}SEMGR;
        /**
          * @}
          */
        /** @defgroup HAL_OS_Private_Variables
           * @{
           */
 THDMGR thdmgr[THREAD_NUMS];
 SEMGR  semgr[SEMAPHORE_NUMS];
        /**
          * @}
          */
        /** @defgroup HAL_OS_Private_Variables
           * @{
           */
#if defined(__CMSIS_RTOS) || defined(__FREERTOS__)
osMutexDef(lock);
osSemaphoreDef(test);
#endif

static osMutexId mutex_id;
        /**
          * @}
          */

/**
  * @brief HAL Layer의 OS 모듈을 초기화 한다.
  * @param NONE
  * @retval NONE
  */
void hOsInit(void)
{
    memset(&thdmgr, 0x0, sizeof(thdmgr));
    memset(&semgr,0x0,sizeof(semgr));
#if defined(__CMSIS_RTOS) || defined(__FREERTOS__)
    mutex_id = (osMutexId) osMutexCreate(osMutex(lock));
#endif
 
}

static uint32_t threadAlloc(hOSThread *th)
{
    int i=0;
    for(; i<THREAD_NUMS  ; i++){
        if(thdmgr[i].use == 1 ) continue;
       thdmgr[i].thread = th;
       thdmgr[i].use = TRUE;
       return (uint32_t)&thdmgr[i];
    }
    return NULL;
}
void threadFree(uint32_t resource)
{
     THDMGR *tm = (THDMGR*) resource;
     memset(tm, 0x0, sizeof(THDMGR));
}

static uint32_t semaAlloc(void)
{
    int i=0;
    for(; i<SEMAPHORE_NUMS  ; i++){
        if(semgr[i].use== 1 ) continue;
        semgr[i].use = TRUE;
        semgr[i].smp = (int *)&semgr[i].sb[0];
       return (uint32_t)&semgr[i];
    }
    return NULL;
}

void semaFree(uint32_t resource)
{
    SEMGR *sm = (SEMGR *)resource;
    memset(sm,0x0,sizeof(SEMGR));
    
}


/**
  * @brief thread를 생성한다.
  * thread 생성 후 해당 trhead는 Ready 상태가 된다.
  * @param[in] thread   hOSThread 타입의 식별자, 이 식별자는 반드시 thread의 이름과 Priority 값을 설정되어야 한다.
  * @param[in] starFunc thread가 시작될때 시작 함수 포인터 값
  * @param[in] param 함수가 시작될때 파라메터값을 사용되는 값
  * @retval  thread ID   : 성공
  * @retval  음수 : 에러   
  */
hOsThrdId_t hOsCreateThread(hOSThread *thread,void *startFunc, void *param)
{
    uint32_t tid;
    THDMGR *tm;
    tm =(THDMGR *) threadAlloc(thread);
    if(tm == NULL) return NULL;
    tm->mid = 0;
    tm->th.instances = 1;
    tm->th.pthread=(os_pthread) startFunc;
    
    tm->th.tpriority = ( thread->priorty ==H_OSTHRD_PRI_LOW) ? 
                                osPriorityLow : ( thread->priorty ==H_OSTHRD_PRI_NORMAL) ? osPriorityNormal :
                                ( thread->priorty ==H_OSTHRD_PRI_HIGH) ? osPriorityHigh : osPriorityNormal ;
#if  defined(__FREERTOS__)
    tm->th.stacksize = configMINIMAL_STACK_SIZE;
#else
    tm->th.stacksize = 0;
#endif
    tid = (uint32_t)osThreadCreate(&tm->th,param);
    assert_param(tid>0);
   // xTaskCreate( (TaskFunction_t)startFunc , (const char *)thread->name , configMINIMAL_STACK_SIZE, param, thread->priorty + 1, (TaskHandle_t * )&tid);
    if(tid == NULL)
    {
        threadFree((uint32_t)tm);
        return NULL;
    }
    tm->tid = tid;
    return (hOsThrdId_t)tm;

}

/**
  * @brief thread를 resume한다.
  * thread가 suspend되어 있다면 resume 되고, ready 된 상태에서 호출된다면 무시 된다.
  * @param[in] tid hOsCreateThread() 에 의해서 생성된 thread 식별자
  * @retval  0  : 성공
  * @retval  음수 : 에러 
  */
int32_t  hOsResumeThread(hOsThrdId_t tid)
{
    THDMGR *thdmgr = (THDMGR *)tid;
#if defined(__CMSIS_RTOS) ||  defined(__FREERTOS__)
    int32_t ret;
    ret = osSignalSet((osThreadId) thdmgr->tid,THREAD_SIGNAL);
    return ret;
#endif
}

/**
  * @brief thread를 suspend한다.
  * thread가 ready 상태에서 block상태로 변경된다.
  * @param NONE
  * @retval 성공:0, 에러: 음수
  */
int32_t hOsSuspendThread(void)
{
#if defined(__CMSIS_RTOS) ||  defined(__FREERTOS__)
    osEvent e;
    e = osSignalWait(THREAD_SIGNAL,osWaitForever);
    return e.status;
#endif
}

/**
  * @brief thread가 stop되고 소멸된다.
  * thread는 종료되게 되고 thread가 점유하고 있는 Stack을 해제 되지만 해당 
  * thread내에서 생성한 device resource들은 그대로 유지 되기 때문에 thread를 종료시
  * 할당한 모든 resource를 모두 해제를 하고 호출되어야 한다.
  * @param[in] tid hOsCreateThread()에 의해 생성된 thread 식별자
  * @retval NONE
  */
void hOsStopThread(hOsThrdId_t tid)
{
    THDMGR *thdmgr = (THDMGR *)tid;
#if defined(__CMSIS_RTOS) ||  defined(__FREERTOS__)
    osThreadTerminate((osThreadId)thdmgr->tid);
    threadFree((uint32_t)thdmgr);
#endif
}


/**
  * @brief Binary Semaphore를 생성 한다.
  * binary Semaphore는 token이 1개이기 때문에 해당 Semaphore는 한 쪽에서만 점유하기 때문에
  * n 개에서 접근 하더라도 오직 한 쪽만 사용될수 있다.
  * @param NONE
  * @retval Semaphore 식별자
  */
hOsSemaId_t hOsBiSemaCreate(void)
{
    int32_t sid;
    SEMGR *sm;
   // osSemaphoreDef_t *sp = (osSemaphoreDef_t *) sema ;
    sm = (SEMGR *)semaAlloc();
    sid =(int32_t) osSemaphoreCreate((const osSemaphoreDef_t *)sm, sm->cnt);
    if(sid == NULL) 
    {
        semaFree((uint32_t)sm);
        return NULL;
    }
    sm->sid = sid;
    return (hOsSemaId_t)sm;
}

/**
  * @brief Semaphore token을 주어진 시간 만 큼 기다린다.
  * 이 함수가 호출 된 이후 token을 점유하지 못하는 경우 호출된 thread는 suspend 하게 된다.
  * suspend된 thread는 hOsBiSemaSignal() 를 호출되어 token을 할당 해줄때 까지 기다리게 된다.
  * @param[in] sid hOsBiSemaCreate()에 의해 생성된 식별자
  * @param[in] millsec millsecond 단위의 time 값, 이 값이 -1 인 경우는 영원히 기다리게 된다.
  * @retval  음수 : 에러 
  */
int32_t hOsBiSemaWait(hOsSemaId_t sid ,int32_t millsec)
{
   SEMGR *sm = (SEMGR *)sid;
#if defined(__CMSIS_RTOS) ||  defined(__FREERTOS__)
   int32_t ret  = osSemaphoreWait((osSemaphoreId)sm->sid,millsec);
   if(ret == -1) 
    return E_ERROR;
#endif
   if( sm->cnt == 1) sm->cnt =0;
   return 0;
}

/**
  * @brief Semaphore의 token값을 할당 해 준다.
  * 이 함수가 호출되면 semaphore를 기다리고 있는 thread가 깨어 나게 된다.
  * semaphore를 기다리는 thread가 없다면 token이 할당 되어 있기 때문에
  * 그 이후 hOsBiSemaWait()를 호출 되는 thread가 wait 되지 않는다.
  * @param[in] sid Semaphore 식별자
  * @retval  0  : 성공
  * @retval  음수 : 에러 
  */
int32_t hOsBiSemaSignal(hOsSemaId_t sid)
{
    SEMGR *sm = (SEMGR *)sid;
    if(sm->cnt ==1) return 0;
    sm->cnt ++;
    osSemaphoreRelease((osSemaphoreId)sm->sid);
    /*
    {
    
        BaseType_t xHigherPriorityTaskWoken=0;    
        if( portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK  == 0)
            xSemaphoreGive(sm->sid);
        else    
            xSemaphoreGiveFromISR(sm->sid,&xHigherPriorityTaskWoken);
    }
    */
    return 0;
}

void hOsBisSemaDelete(hOsSemaId_t sid)
{
        SEMGR *sm = (SEMGR *)sid;
#if defined(__CMSIS_RTOS) ||  defined(__FREERTOS__)
        osSemaphoreDelete((osSemaphoreId)sm->sid);
#endif
    semaFree((uint32_t)sm);
}


/**
  * @brief os의 Lock을 호출 한다.
  * 이 함수는 hSysLock처럼 IRQ가 disable되지 않고 OS Level에서 
  * thread간의 context switching 이 일어나지 않는다. 따라서 Interrupt
  * 발생시 인터럽트 루틴을 수행 후 다른 thread로 전환되지 않고 이 함수를 시작으로
  * hOsUnlock() 을 만나는 구간 내에서만 실행 된다.
  * @param NONE
  * @retval NONE
  */
void hOsLock(void)
{
    osMutexWait ( (osMutexId)mutex_id, (unsigned int)-1);
}
/**
  * @brief os Unlock을 호출한다.
  * @param NONE
  * @retval NONE
  */
void hOsUnlock(void)
{
    osMutexRelease((osMutexId) mutex_id);
}

void hOsRun(void)
{
#ifdef __CMSIS_RTOS
    ;
#elif defined(__FREERTOS__)
    vTaskStartScheduler();
	for(;;);
#endif
}

    /**
      * @}
      */

/**
* @}
*/

#ifdef __FREERTOS__
void vApplicationTickHook( void )
{
 
}
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
        ( void ) pcTaskName;
        ( void ) pxTask;

        /* Run time stack overflow checking is performed if
        configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
        function is called if a stack overflow is detected. */
        taskDISABLE_INTERRUPTS();
        for( ;; );
}
void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}

#endif

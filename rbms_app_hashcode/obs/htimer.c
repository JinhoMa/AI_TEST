#include "main.h"
#include <stdint.h>
#define TIMERn 4

struct time_struct
{
	uint8_t used;
	uint32_t  stime;
    uint32_t  ntime;
    void (*func)(void *);
}   tres[TIMERn];

static uint32_t tid_malloc(void)
{
	int i;
	for (i = 0; i < TIMERn; i++)
    {
		if (tres[i].used == 0)
        {
	        tres[i].used = 1;
            return (uint32_t)&tres[i];
		}
	}
	return 0;
}

static void tid_free(uint32_t tid)
{
	struct time_struct *t = (struct time_struct *)tid;
	t->used = 0;
}
extern TIM_HandleTypeDef htim3;

uint32_t uhCapture = 0;

/* Timer handler declaration */
TIM_HandleTypeDef    TimHandle;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfig;

/* Counter Prescaler value */
uint32_t msPrescalerValue = 0;

volatile unsigned int *DWT_CYCCNT = (volatile unsigned int *)0xE0001004;    //address of the register
volatile unsigned int *DWT_CONTROL = (volatile unsigned int *)0xE0001000;   //address of the register
volatile unsigned int *SCB_DEMCR = (volatile unsigned int *)0xE000EDFC;     //address of the register
volatile uint32_t *LAR  = (uint32_t *) 0xE0001FB0;                          // <-- added lock access register

void EnableTiming(void)
{
    *SCB_DEMCR = *SCB_DEMCR | 0x01000000;
    //jae.kim cortex M7 에서 바뀐듯하다. 이 부분을 안해주면 control enable 이 안된다.
    *LAR = 0xC5ACCE55;                // <-- added unlock access to DWT (ITM, etc.)registers , 
    *DWT_CYCCNT = 0; // reset the counter
    *DWT_CONTROL = 0x40000001; // *DWT_CONTROL | 1 ; // enable the counter
}


void hTimInit(void)
{
    	
#if 0

    /* Compute the prescaler value to have TIM3 counter clock equal to 1 MHz */
    // Hz = (SystemCoreClock)/2 /(Preload+1)*(Prescaler+1)
    // 1KHz
    msPrescalerValue = (uint32_t)((SystemCoreClock / 2) / 1000000) - 1;
    TimHandle.Instance = TIM3;
    TimHandle.Init.Period = msPrescalerValue;
    TimHandle.Init.Prescaler = 1000 - 1;
    TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&TimHandle, &sClockSourceConfig);
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&TimHandle, &sMasterConfig);
    EnableTiming();
#endif
}

uint64_t time_tick = 0L;
uint64_t hTimCurrentTime(void)
{
    time_tick = HAL_GetTick();

    return time_tick;
}

uint32_t hTimCreate(void)
{
	return tid_malloc();
}

void hTimSet(uint32_t tid, uint32_t msec, void *cbFunc)
{
	struct time_struct *t = (struct time_struct *)tid;
	t->stime = msec;
    t->ntime = 0;
    t->func = (void (*)(void *))cbFunc;
}

void hTimStart(uint32_t tid)
{
    if (tid == (uint32_t)&tres[0])
    {
        HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
    }
    else if (tid == (uint32_t)&tres[1])
    {
        HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_2);
    }
    else if (tid == (uint32_t)&tres[2])
    {
        HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_3);
    }
    else if (tid == (uint32_t)&tres[3])
    {
        HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_4);
    }
}

void hTimStop(uint32_t tid)
{
    if (tid == (uint32_t)&tres[0])
    {
        HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_1);
    }
    else if (tid == (uint32_t)&tres[1])
    {
        HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_2);
    }
    else if (tid == (uint32_t)&tres[2])
    {
        HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_3);
    }
    else if (tid == (uint32_t)&tres[3])
    {
        HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_4);
    }
}

void hTimClose(uint32_t tid)
{
    tid_free(tid);
}


void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    struct time_struct *t;
    switch (htim->Channel)
    {
        case  HAL_TIM_ACTIVE_CHANNEL_1:
            t = &tres[0];
            break;
        case  HAL_TIM_ACTIVE_CHANNEL_2:
            t = &tres[1];
        break;
        case  HAL_TIM_ACTIVE_CHANNEL_3:
            t = &tres[2];
        break;
        case  HAL_TIM_ACTIVE_CHANNEL_4:
            t = &tres[3];
        break;
        default:
            return;
    }

    if (++t->ntime >= t->stime)
    {
    //on
        (*t->func)(NULL);
        t->ntime = 0;
    }

}

void TimingDelay(unsigned int tick)
{
    uint32_t start, current;

    start = *DWT_CYCCNT;

    do
    {
        current = *DWT_CYCCNT;
        if (current < start)
        {
            if ((0xffffffff-start+current) >= tick)
            {
                break;
            }
        }
        else if ((current - start) >= tick)
        {
            break;
        }
    } while (1);
}


void hTimDelay(uint32_t usec){
#if 1

    __IO int counter = (usec * 2);
    while (counter != 0)
    {
      counter--;
    }
#else
    TimingDelay(usec);

#endif

}

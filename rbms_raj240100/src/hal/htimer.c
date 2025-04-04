#include "BMS.h"
#include "main.h"

#define TIMERn 1

uint32_t t_1ms;
uint8_t t_10ms_1;
uint8_t t_20ms_1;
uint8_t t_300ms_1;
uint8_t t_100ms_1;
uint8_t t_500ms_1;
uint8_t t_200ms_1;


struct time_struct{
	uint8_t used;
	uint32_t  stime;
    uint32_t  ntime;
    void __far (* __far func)();
}  tres[TIMERn];

static  uint32_t tid_malloc(void)
{
	int i;
	for(i=0;i<TIMERn;i++){
		if( tres[i].used ==0) { 
		tres[i].used = 1; return (uint32_t)&tres[i];
		}
	}
	return 0;
}

static void tid_free(uint32_t tid)
{
	struct time_struct *t = (struct time_struct *)tid;
	t->used = 0;
}


void hTimInit(void)
{
    //Current Time 1ms start
}


uint64_t time_tick = 0L;
uint64_t hTimCurrentTime(void)
{
    return time_tick;
}

uint32_t hTimCreate(void)
{
	 return tid_malloc();
}

void hTimSet(uint32_t tid, uint32_t msec,__far void* __far cbFunc)
{
	struct time_struct *t = (struct time_struct *)tid;
	t->stime = msec;
    t->ntime =0 ;
    t->func = cbFunc;
}

void hTimStart(uint32_t tid)
{
	struct time_struct *t = (struct time_struct *)tid;
    if(tid == (uint32_t)&tres[0]){
       t->ntime = 0;
       Timer1_BspStart();
    }
}

void hTimStop(uint32_t tid)
{
	struct time_struct *t = (struct time_struct *)tid;
    if(tid == (uint32_t)&tres[0]){
       Timer1_BspStop();
    }
}

void hTimClose(uint32_t tid){
    tid_free( tid );
}




void hTimDelay(uint32_t msec){
	uint64_t stick = time_tick;
	while(hTimCurrentTime()-stick < msec);
}

void HAL_TIM_ElapsedCallback(void *htim)
{
    struct time_struct *t;
    t = &tres[0];
    if( ++t->ntime >= t->stime){
    //on 
        if(t->func)
            (*t->func)();
        t->ntime = 0;
    }
}
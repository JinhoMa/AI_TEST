#ifndef __TIMER_H__
#define __TIMER_H__


/**
    ******************************************************************************
    *     @file         Timer.h
    *     @version   
    *     @author    
    *     @date      2020/03/02 
    *     @brief     
    ******************************************************************************
    */

#ifdef	_TIME
#define		GLOBAL
#else
#define		GLOBAL	extern
#endif	/* _TIME */


#if 0    
typedef struct _Time{
    DWORD lo;
    DWORD hi;
}Time;
#else
typedef long long  Time;
#endif

GLOBAL long long tick_timer;
// BSP Function Define
#define Timer0_BspStart()   TS0L_bit.no0 = 1
#define Timer0_BspStop()   TT0L_bit.no0 = 1
#define Timer1_BspStart()   TS0L_bit.no1 = 1
#define Timer1_BspStop()   TT0L_bit.no1 = 1
#define Timer2_BspStart()   TS0L_bit.no2 = 1
#define Timer2_BspStop()   TT0L_bit.no2 = 1
#define Timer2_BspStopWait()   while( TE0L_bit.no2 == ON )     // Wait until timer stop

#define TickTimer_Start     Timer1_BspStart
#define SlvToutStart()        TS0L_bit.no2 = 1
#define SlvToutStop()         TT0L_bit.no2 = 1
#define SlvStopWait()         while( TE0L_bit.no2 == ON )
extern Time sysTimer;
void Timer0_BspInit(void);
void Timer1_BspInit(void);
void Timer2_BspInit(void);

void Tim_CurrentTime(Time *t);
DWORD Tim_CurPassTime(Time * t);

#define Tim_GetCurrentTime()  sysTimer


#endif //__TIMER_H__

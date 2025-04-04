

#include "obshal.h"
#include <assert.h>
#include "modbus.h"
#include <string.h>
#include <stdio.h>
#include "port_modbus.h"
#include "main.h"


/* ----------------------- Defines  -----------------------------------------*/
#if MB_ASCII_ENABLED == 1
#define	BUF_SIZE	513     /* must hold a complete ASCII frame. */
#else
#define	BUF_SIZE	256     /* must hold a complete RTU frame. */
#endif
#define TX_START_EVENT 1

/* ----------------------- Static variables ---------------------------------*/
static int32_t mbmaster_uart_fd2;
static uint32_t mtimer;
static int32_t  mbmaster_serSema2;
static int32_t  mbMasterTxStartEvent;
static hOSThread mdbsMaster_th;
static int32_t mbmaster_evtSema2;
static uint32_t mbmaster_event;


static uint8_t    ucBuffer[BUF_SIZE];
static int32_t      TxBufCnt;
static uint32_t     expiredUtime;
static uint64_t     prevtime;
/* ----------------------- Static Function ---------------------------------*/
static void (*mbMasterPortCBTimerExpired2)(void);
static void mbMasterPortThread2(void const * parameter) ;

/* ----------------------- Function prototypes ------------------------------*/


/* ----------------------- Begin implementation -----------------------------*/
__weak PUBLIC void mbMasterCallbackByteReceived2(void){
}
__weak PUBLIC void mbMasterCallbackTrasmitterEmpty2(void){
}

void serialCallbackFromIsr2(int mode)
{
    if(mode == 0){
        mbMasterCallbackByteReceived2();
    }else{
        mbMasterCallbackTrasmitterEmpty2();
    }
}
static void timer_timeout_ind2(void* parameter)
{
    htrace(LOG_MODBUS, "expired=%d\n", (int)(hTimCurrentTime()-prevtime));
    (void) mbMasterPortCBTimerExpired2();
}

static void mbMasterPortEventInit2( void )
{
    mbmaster_event =0;
    mbmaster_evtSema2 = hOsBiSemaCreate();
}



PUBLIC uint32_t mbMasterPortTimerInit2(uint32_t  usec, void *asyncFunc )
{
    /* backup T35 ticks */
    expiredUtime = usec/1000 ;
    mtimer = hTimCreate();
    mbMasterPortCBTimerExpired2 = ( void (*)(void))asyncFunc;
    return TRUE;
}

PUBLIC void mbMasterPortTimerT35Enable2(void )
{
    /* Set current timer mode, don't change it.*/
    MBMasterSetCurTimerMode2(MBMASTER_TMODE_T35);
    hTimStop(mtimer);
    prevtime = hTimCurrentTime();
    hTimSet(mtimer, expiredUtime , &timer_timeout_ind2);
    hTimStart(mtimer);
}

PUBLIC void mbMasterPortTimerConvertDelayEnable2(void)
{

    /* Set current timer mode, don't change it.*/
    MBMasterSetCurTimerMode2(MBMASTER_TMODE_CONVERT_DELAY);
    hTimStop(mtimer);
	  prevtime = hTimCurrentTime();
    hTimSet(mtimer, MBMASTER_DELAY_MS_CONVERT,  &timer_timeout_ind2);
    hTimStart(mtimer);
}

PUBLIC void mbMasterPortTimerRspTimeoutEnable2(void)
{

    /* Set current timer mode, don't change it.*/
    MBMasterSetCurTimerMode2(MBMASTER_TMODE_RESPOND_TIMEOUT);
    hTimStop(mtimer);
	  prevtime = hTimCurrentTime();
    hTimSet(mtimer, MBMASTER_TIMEOUT_MS_RESPOND, &timer_timeout_ind2);
    hTimStart(mtimer);
}

PUBLIC void mbMasterPortTimersDisable2(void)
{
    hTimStop(mtimer);
}

PUBLIC uint32_t mbMasterPortSerialInit2( uint8_t ucPort, uint32_t ulBaudRate, uint8_t ucDataBits,
                   int eParity )
{
    int32_t tid;
    mbmaster_uart_fd2 = E_ERROR ;
    /**
     * set 485 mode receive and transmit control IO
     * @note MODBUS_MASTER_RT_CONTROL_PIN_INDEX need be defined by user
     */
    mbmaster_uart_fd2 = hSerOpen("COM3", ulBaudRate, serialCallbackFromIsr2 );
    mbmaster_serSema2 = hOsBiSemaCreate();
    mbMasterPortEventInit2();
    hOsSetThreadName((char*)mdbsMaster_th, "mbmstprt2");
    mdbsMaster_th.priorty = (int32_t) H_OSTHRD_PRI_NORMAL;
    tid = hOsCreateThread( &mdbsMaster_th,   (void *)&mbMasterPortThread2 ,NULL);   
    hOsResumeThread(tid);
    return mbmaster_uart_fd2;
    
}

PUBLIC void mbMasterPortSerialClose2 ( void )
{
    hSerClose(mbmaster_uart_fd2);
}


PUBLIC void mbMasterPortSerialEnable2( int32_t bEnableRx, int32_t bEnableTx )
{
    /* it is not allowed that both receiver and transmitter are enabled. */
    //assert( !bEnableRx || !bEnableTx );

    if( bEnableRx )
    {
    }
    else
    {
    }
    if( bEnableTx )
    {
        TxBufCnt = 0;
        mbMasterTxStartEvent = TX_START_EVENT;
        hSerFlush(mbmaster_uart_fd2);
        hSerCtrl(mbmaster_uart_fd2, SER_SET_RS485TXE, 1);
        hOsBiSemaSignal(mbmaster_serSema2);        
    }
    else
    {
        mbMasterTxStartEvent = 0;
    }
}

PUBLIC void mbMasterPortSerialSend2(void)
{
    hSerWrite(mbmaster_uart_fd2,&ucBuffer[0], TxBufCnt);
}


PUBLIC uint32_t mbMasterPortSerialPutByte2( char  ucByte )
{
    //assert( TxBufCnt < BUF_SIZE );
    ucBuffer[TxBufCnt] = ucByte;
    TxBufCnt++;
    return TRUE;
}

PUBLIC uint32_t mbMasterPortSerialGetByte2( char * pucByte )
{
    if(hSerAvailable(mbmaster_uart_fd2)){
       hSerRead( mbmaster_uart_fd2,(uint8_t*) pucByte, 1);
       return 1;
    }
    else
       return 0;
}

// false - big endian, true - little endian, 
// big endian 인경우 그대로 사용한다.

PUBLIC uint32_t mbMasterPortGetByteOrder2(void)
{
	return (uint32_t)TRUE;
}


PUBLIC int32_t mbMasterPortEventPost2( MBMasterEventType eEvent )
{
	hSysLock();
	mbmaster_event |= eEvent;
	hSysUnlock();
	//rt_event_send(&xMasterOsEvent, eEvent);
	hOsBiSemaSignal(mbmaster_evtSema2);
    return TRUE;
}

PUBLIC int32_t mbMasterPortEventGet2( MBMasterEventType * eEvent )
{
    *(int *)eEvent =0 ;
    hOsBiSemaWait(mbmaster_evtSema2, -1);
    if (mbmaster_event & EV_MASTER_READY )
    {
        hSysLock();
        mbmaster_event &= ~(EV_MASTER_READY) ;
        hSysUnlock();
        *eEvent = EV_MASTER_READY;
    }else
    if (mbmaster_event & EV_MASTER_FRAME_RECEIVED)
    {
        hSysLock();
        mbmaster_event &= ~(EV_MASTER_FRAME_RECEIVED) ;
        hSysUnlock();
        *eEvent = EV_MASTER_FRAME_RECEIVED;
    }else
    if (mbmaster_event & EV_MASTER_EXECUTE)
    {
        hSysLock();
        mbmaster_event &= ~(EV_MASTER_EXECUTE) ;
        hSysUnlock();
        *eEvent = EV_MASTER_EXECUTE;
    }else
    if (mbmaster_event & EV_MASTER_FRAME_SENT)
    {
        hSysLock();
        mbmaster_event &= ~(EV_MASTER_FRAME_SENT) ;
        hSysUnlock();
        *eEvent = EV_MASTER_FRAME_SENT;
    }else
    if (mbmaster_event & EV_MASTER_ERROR_PROCESS)
    {
        hSysLock();
        mbmaster_event &= ~(EV_MASTER_ERROR_PROCESS) ;
        hSysUnlock();
        *eEvent = EV_MASTER_ERROR_PROCESS;
    }
    return TRUE;
}



static void mbMasterPortThread2(void const * parameter) {
    int timeout=-1;
    while (1)
    {
        if( mbMasterTxStartEvent == TX_START_EVENT ){
            /* execute modbus callback */
            mbMasterCallbackTrasmitterEmpty2();
        }
        else{
            hOsBiSemaWait(mbmaster_serSema2  , timeout);
        }
    }
}



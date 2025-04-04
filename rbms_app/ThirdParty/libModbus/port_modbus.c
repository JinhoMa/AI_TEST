

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
static int32_t mbmaster_uart_fd;
static uint32_t mtimer;
static hOsSemaId_t  mbmaster_serSema;
static int32_t  mbMasterTxStartEvent;
static hOSThread mdbsMaster_th;
static hOsSemaId_t mbmaster_evtSema;
uint32_t mbmaster_event;


static uint8_t    ucBuffer[BUF_SIZE];
static int32_t      TxBufCnt;
static uint32_t     expiredUtime;
static uint64_t     prevtime;
/* ----------------------- Static Function ---------------------------------*/
static void (*mbMasterPortCBTimerExpired)(void);
static void mbMasterPortThread(void const * parameter) ;

/* ----------------------- Function prototypes ------------------------------*/


/* ----------------------- Begin implementation -----------------------------*/
__weak PUBLIC void mbMasterCallbackByteReceived(void){
}
__weak PUBLIC void mbMasterCallbackTrasmitterEmpty(void){
}

void serialCallbackFromIsr(int mode)
{
    if(mode == 0){
        mbMasterCallbackByteReceived();
    }else{
        mbMasterCallbackTrasmitterEmpty();
    }
}
static void timer_timeout_ind(void* parameter)
{
    htrace(LOG_MODBUS, "expired=%d\n", (int)(hTimCurrentTime()-prevtime));
    (void) mbMasterPortCBTimerExpired();
}

static void mbMasterPortEventInit( void )
{
    mbmaster_event =0;
    mbmaster_evtSema = hOsBiSemaCreate();
}



PUBLIC uint32_t mbMasterPortTimerInit(uint32_t  usec, void *asyncFunc )
{
    /* backup T35 ticks */
    expiredUtime = usec/1000 ;
    mtimer = hTimCreate();
    mbMasterPortCBTimerExpired = ( void (*)(void))asyncFunc;
    return TRUE;
}

PUBLIC void mbMasterPortTimerT35Enable(void )
{
    /* Set current timer mode, don't change it.*/
    //htrace(LOG_MODBUS, "T32 en\n");

    MBMasterSetCurTimerMode(MBMASTER_TMODE_T35);
    hTimStop(mtimer);
    prevtime = hTimCurrentTime();
    hTimSet(mtimer, expiredUtime , &timer_timeout_ind);
    hTimStart(mtimer);
}

PUBLIC void mbMasterPortTimerConvertDelayEnable(void)
{
   // htrace(LOG_MODBUS, "Tconv en\n");

    /* Set current timer mode, don't change it.*/
    MBMasterSetCurTimerMode(MBMASTER_TMODE_CONVERT_DELAY);
    hTimStop(mtimer);
	  prevtime = hTimCurrentTime();
    hTimSet(mtimer, MBMASTER_DELAY_MS_CONVERT,  &timer_timeout_ind);
    hTimStart(mtimer);
}

PUBLIC void mbMasterPortTimerRspTimeoutEnable(void)
{

    //htrace(LOG_MODBUS, "Trsp en\n");
    /* Set current timer mode, don't change it.*/
    MBMasterSetCurTimerMode(MBMASTER_TMODE_RESPOND_TIMEOUT);
    hTimStop(mtimer);
	  prevtime = hTimCurrentTime();
    hTimSet(mtimer, MBMASTER_TIMEOUT_MS_RESPOND, &timer_timeout_ind);
    hTimStart(mtimer);
}

PUBLIC void mbMasterPortTimersDisable(void)
{
    //htrace(LOG_MODBUS, "Tdisable \n");
    hTimStop(mtimer);
}

PUBLIC uint32_t mbMasterPortSerialInit(const uint8_t* ucPort, uint32_t ulBaudRate, uint8_t ucDataBits,
                   int eParity )
{
    hOsThrdId_t tid;
    mbmaster_uart_fd = E_ERROR ;
    /**
     * set 485 mode receive and transmit control IO
     * @note MODBUS_MASTER_RT_CONTROL_PIN_INDEX need be defined by user
     */
    mbmaster_uart_fd = hSerOpen(( char *)ucPort, ulBaudRate, serialCallbackFromIsr );
    mbmaster_serSema = hOsBiSemaCreate();
    mbMasterPortEventInit();
    hOsSetThreadName((char*)mdbsMaster_th, "mbmstport");
    mdbsMaster_th.priorty = (int32_t) H_OSTHRD_PRI_NORMAL;
    tid = hOsCreateThread( &mdbsMaster_th,   (void *)&mbMasterPortThread ,NULL);   
    hOsResumeThread(tid);
    return mbmaster_uart_fd;
    
}

PUBLIC void mbMasterPortSerialClose ( void )
{
    hSerClose(mbmaster_uart_fd);
}


PUBLIC void mbMasterPortSerialEnable( int32_t bEnableRx, int32_t bEnableTx )
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
        hSerFlush(mbmaster_uart_fd);
        hSerCtrl(mbmaster_uart_fd, SER_SET_RS485TXE, 1);
        hOsBiSemaSignal(mbmaster_serSema);        
    }
    else
    {
        mbMasterTxStartEvent = 0;
    }
}

PUBLIC void mbMasterPortSerialSend(void)
{
    hSerWrite(mbmaster_uart_fd,&ucBuffer[0], TxBufCnt);
}


PUBLIC uint32_t mbMasterPortSerialPutByte( char  ucByte )
{
    //assert( TxBufCnt < BUF_SIZE );
    hOsLock();
    ucBuffer[TxBufCnt] = ucByte;
    TxBufCnt++;
    hOsUnlock();
    return TRUE;
}

PUBLIC uint32_t mbMasterPortSerialGetByte( char * pucByte )
{
    if(hSerAvailable(mbmaster_uart_fd)){
       hSerRead( mbmaster_uart_fd,(uint8_t*) pucByte, 1);
       return 1;
    }
    else
       return 0;
}

// false - big endian, true - little endian, 
// big endian 인경우 그대로 사용한다.

PUBLIC uint32_t mbMasterPortGetByteOrder(void)
{
	return (uint32_t)TRUE;
}


PUBLIC int32_t mbMasterPortEventPost( MBMasterEventType eEvent )
{
	hSysLock();
	mbmaster_event |= eEvent;
	hSysUnlock();
	//rt_event_send(&xMasterOsEvent, eEvent);
	hOsBiSemaSignal(mbmaster_evtSema);
    hLogProc(LOG_MODBUS, "signal send evtSema(0x%x)\n", mbmaster_event);
    return TRUE;
}

PUBLIC int32_t mbMasterPortEventGet( MBMasterEventType * eEvent )
{
    *(int *)eEvent =0 ;
    if(mbmaster_event == NULL){
        hOsBiSemaWait(mbmaster_evtSema, -1);
    }
    hLogProc(LOG_MODBUS, "wakeup evtSema(0x%x)\n",mbmaster_event );

    if (mbmaster_event & EV_MASTER_READY )
    {
        hOsLock();
        mbmaster_event &= ~(EV_MASTER_READY) ;
        hOsUnlock();
        *eEvent = EV_MASTER_READY;
    }else
    if (mbmaster_event & EV_MASTER_FRAME_RECEIVED)
    {
        hOsLock();
        mbmaster_event &= ~(EV_MASTER_FRAME_RECEIVED) ;
        hOsUnlock();
        *eEvent = EV_MASTER_FRAME_RECEIVED;
    }else
    if (mbmaster_event & EV_MASTER_EXECUTE)
    {
        hOsLock();
        mbmaster_event &= ~(EV_MASTER_EXECUTE) ;
        hOsUnlock();
        *eEvent = EV_MASTER_EXECUTE;
    }else
    if (mbmaster_event & EV_MASTER_FRAME_SENT)
    {
        hOsLock();
        mbmaster_event &= ~(EV_MASTER_FRAME_SENT) ;
        hOsUnlock();
        *eEvent = EV_MASTER_FRAME_SENT;
    }else
    if (mbmaster_event & EV_MASTER_ERROR_PROCESS)
    {
        hOsLock();
        mbmaster_event &= ~(EV_MASTER_ERROR_PROCESS) ;
        hOsUnlock();
        *eEvent = EV_MASTER_ERROR_PROCESS;
    }
    return TRUE;
}



static void mbMasterPortThread(void const * parameter) {
    int timeout=-1;
    while (1)
    {
        if( mbMasterTxStartEvent == TX_START_EVENT ){
            /* execute modbus callback */
            mbMasterCallbackTrasmitterEmpty();
        }
        else{
            hOsBiSemaWait(mbmaster_serSema  , timeout);
        }
    }
}



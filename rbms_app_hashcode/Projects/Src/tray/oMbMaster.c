#include <stdio.h>
#include "obshal.h"
#include "hos.h"
#include "htypes.h"
#include "hlog.h"
#include "modbus.h"
#include "modbus_def.h"
#include "oMbMaster.h"
#include "Port_modbus.h"
#include "main.h"

enum {
 OMB_IDLE,
 OMB_READY,
 OMB_ERROR,
 OMB_DONE,
} OMBSTATE;

typedef struct {
    uint8_t mode;
    const uint8_t *port;
    uint32_t rtubaudrate;
    hOsSemaId_t sema;
    uint16_t *rdata;
    uint16_t rdsize;
    uint16_t *wdata;
    uint16_t wdsize;
    int32_t state;
    int32_t timeout;
    void (*asyncFunc)(int);
} oMbMaster_;

static hOSThread mbmaster_thread;
static hOsThrdId_t mbmaster_tid;
static uint8_t mbmaster1_run;
static void mbMasterTask (void *param);
oMbMaster_ oMbMaster;
uint16_t transmit_ackBuf[10];


PUBLIC int32_t oMbMasterOpen(uint32_t mode, const uint8_t *port, uint32_t baudrate, void *asyncFunc)
{
    hOsSetThreadName((char *)mbmaster_thread, "mbmaster");
    mbmaster_thread.priorty = (int32_t) H_OSTHRD_PRI_NORMAL;
    oMbMaster.mode  = mode;
    oMbMaster.port = port;
    oMbMaster.rtubaudrate = baudrate;
    oMbMaster.asyncFunc = (void(*)(int))asyncFunc;
    if(asyncFunc == NULL){
        oMbMaster.sema = hOsBiSemaCreate();
        assert_param(oMbMaster.sema  != 0);
    }
    mbmaster_tid = hOsCreateThread( &mbmaster_thread,   (void *)&mbMasterTask , NULL);  
    assert_param(mbmaster_tid != 0);
    //hSysWait(100);	
    while(!mbmaster1_run){
        hSysWait(1);
    }
    return (int32_t)&oMbMaster;
}


PUBLIC int32_t oMbMasterClose(int32_t fd)
{
    if( fd ==(int32_t)&oMbMaster){
        hOsStopThread(mbmaster_tid);
        oMbMaster.port = 0; oMbMaster.asyncFunc = NULL;
    }
    return 0;
}

PUBLIC void static oMbMasterSetState(int32_t fd, int32_t state)
{
    if( fd == (int32_t )&oMbMaster){
        hSysLock();
        oMbMaster.state = state;
        hSysUnlock();
    }

}

PUBLIC int32_t oMbMasterGetState(int32_t fd)
{
    int32_t ret;
    if( fd == (int32_t )&oMbMaster){
        hSysLock();
        ret = oMbMaster.state ;
        hSysUnlock();
    }
    return ret;
}
static int32_t readModbusFunc(uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data, int32_t func)
{
    int ret =0,val;
    oMbMaster.rdata = data;
    oMbMaster.rdsize = cnt;
    oMbMasterSetState( (int32_t)&oMbMaster, OMB_READY);
    val = cnt;
    mbMasterRTUSend(slaveAddr, func,  addr, val , NULL );
    mbMasterPortEventPost(EV_MASTER_FRAME_SENT);
    if(oMbMaster.asyncFunc == NULL){
        hOsBiSemaWait(oMbMaster.sema, -1);
        if(oMbMasterGetState((int32_t)&oMbMaster) == OMB_ERROR ){
            ret = E_ERROR;
        }else if(oMbMasterGetState((int32_t)&oMbMaster) == OMB_DONE){
            ret = oMbMaster.rdsize;
        }
        oMbMasterSetState((int32_t)&oMbMaster, OMB_IDLE);
    }
    return ret;

}
static int32_t writeModbusFunc
(
    uint32_t slaveAddr, uint32_t addr, uint8_t *data, uint32_t cnt, int32_t timeout, int32_t func
)
{
    int ret =0, val;
    oMbMaster.rdata = transmit_ackBuf;
    oMbMaster.rdsize = 8;    
    oMbMaster.wdata = NULL;;
    oMbMaster.wdsize = cnt;
    oMbMaster.timeout = timeout;
    oMbMasterSetState((int32_t)&oMbMaster ,OMB_READY);
    val = cnt;
    mbMasterRTUSend(slaveAddr, func,  addr, val, data);
    mbMasterPortEventPost(EV_MASTER_FRAME_SENT);
    if(oMbMaster.asyncFunc == NULL){
        hOsBiSemaWait(oMbMaster.sema, timeout);
        if(oMbMasterGetState( (int32_t)&oMbMaster ) == OMB_ERROR ){
            ret = E_ERROR;
        }else if(oMbMasterGetState((int32_t)&oMbMaster) == OMB_DONE){
            ret = oMbMaster.wdsize;
        }
        oMbMasterSetState((int32_t)&oMbMaster, OMB_IDLE);
    }
    return ret;

}

static int32_t writeModbusSingleFunc
(
    uint32_t slaveAddr, uint32_t addr, uint16_t data, int32_t timeout, int32_t func
)
{
    int ret =0,val;
    oMbMaster.rdata = transmit_ackBuf;
    oMbMaster.rdsize = 8;
    oMbMaster.wdata = NULL;;
    oMbMaster.wdsize = 1;
    oMbMaster.timeout = timeout;
    oMbMasterSetState((int32_t)&oMbMaster, OMB_READY);
    val = data;
    mbMasterRTUSend(slaveAddr, func,  addr, val , NULL);
    mbMasterPortEventPost(EV_MASTER_FRAME_SENT);
    if(oMbMaster.asyncFunc == NULL){
        hOsBiSemaWait(oMbMaster.sema, timeout);
        if(oMbMasterGetState((int32_t)&oMbMaster) == OMB_ERROR ){
            ret = E_ERROR;
        }else if(oMbMasterGetState((int32_t)&oMbMaster) == OMB_DONE){
            ret = oMbMaster.wdsize;
        }
        oMbMasterSetState((int32_t)&oMbMaster, OMB_IDLE);
    }
    return ret;

}


/**
   *********************************************************************************************************************************
   * @brief Function Code 1
   * @param [in] slaveAddr   slave의 국번
   * @param [in] addr             coil 의 시작 어드레스
   * @param [in] cnt                coli 의 읽을 개수
   * @param [out] data           성공적으로 읽은 경우 데이터가 채워지는 버퍼.  16비트 버퍼에 8비트씩 채워진다.
          코일을 16개 읽어 온다면, 첫번째 버퍼 하위 8bit 에 8개가 채워지고, 다음 버퍼에 하위 8bit에 8개가 채워진다.
          만일  open시 Async  모드로 open하였다면  함수가 반환될때 채워지지 않는다.오직  Blocking  함수로 사용하는 경우만 채워진다.
   * @return 에러의 경우 음수가 반환된다. non-blocking 모드의 경우 에러가 없다면 0 이 반환된다. blocking 모드의 경우 
   * 성공한 경우 data 버퍼에 채워진 size가 반환되므로 0 보다 큰값이 반환된다.
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbMasterReadCoils(int32_t fd,  uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data)
{
    int32_t ret;
    if( fd == (int32_t)&oMbMaster){
        ret = readModbusFunc(slaveAddr, addr, cnt, data, FC_READ_COIL_STATUS);
    }
    return ret;

}
/**
   *********************************************************************************************************************************
   * @brief Function Code 2
   * @param [in] slaveAddr   slave의 국번
   * @param [in] addr             Discrete Input 의 시작 어드레스
   * @param [in] cnt                Discrete Input 의 읽을 개수
   * @param [out] data           성공적으로 읽은 경우 데이터가 채워지는 버퍼.  만일  open시 Async  모드로 open하였다면 
   *  함수가 반환될때 채워지지 않는다.오직  Blocking  함수로 사용하는 경우만 채워진다.
   * @return 에러의 경우 음수가 반환된다. non-blocking 모드의 경우 에러가 없다면 0 이 반환된다. blocking 모드의 경우 
   * 성공한 경우 data 버퍼에 채워진 size가 반환되므로 0 보다 큰값이 반환된다.
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbMasterReadDisInputs( int32_t fd, uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data)
{
    int32_t ret;
    if( fd == (int32_t)&oMbMaster){
        ret = readModbusFunc(slaveAddr, addr, cnt, data, FC_READ_INPUT_STATUS);
    }
    return ret;
}
/**
   *********************************************************************************************************************************
   * @brief Function Code 3
   * @param [in] slaveAddr   slave의 국번
   * @param [in] addr             HoldRegister  의 시작 어드레스
   * @param [in] cnt                HoldRegister  의 읽을 개수
   * @param [out] data           성공적으로 읽은 경우 데이터가 채워지는 버퍼.  만일  open시 Async  모드로 open하였다면 
   *  함수가 반환될때 채워지지 않는다.오직  Blocking  함수로 사용하는 경우만 채워진다.
   * @return 에러의 경우 음수가 반환된다. non-blocking 모드의 경우 에러가 없다면 0 이 반환된다. blocking 모드의 경우 
   * 성공한 경우 data 버퍼에 채워진 size가 반환되므로 0 보다 큰값이 반환된다.
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbMasterReadHoldRegs(int32_t fd, uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data)
{
    int32_t ret;
    if( fd == (int32_t)&oMbMaster){
        ret = readModbusFunc(slaveAddr, addr, cnt, data, FC_READ_HOLDING_REGISTERS);
    }
    return ret;
}

/**
   *********************************************************************************************************************************
   * @brief Function Code 4
   * @param [in] slaveAddr   slave의 국번
   * @param [in] addr             Input Register  의 시작 어드레스
   * @param [in] cnt                Input Register  의 읽을 개수
   * @param [out] data           성공적으로 읽은 경우 데이터가 채워지는 버퍼.  만일  open시 Async  모드로 open하였다면 
   *  함수가 반환될때 채워지지 않는다.오직  Blocking  함수로 사용하는 경우만 채워진다.
   * @return 에러의 경우 음수가 반환된다. non-blocking 모드의 경우 에러가 없다면 0 이 반환된다. blocking 모드의 경우 
   * 성공한 경우 data 버퍼에 채워진 size가 반환되므로 0 보다 큰값이 반환된다.
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbMasterReadInputRegs(int32_t fd, uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data )
{
    int32_t ret;
    if( fd == (int32_t)&oMbMaster){
       ret = readModbusFunc(slaveAddr, addr, cnt, data, FC_READ_INPUT_REGISTERS);
    }
    
    return ret;
}
/**
   *********************************************************************************************************************************
   * @brief Function Code 5
   * @param [in] slaveAddr   slave의 국번
   * @param [in] addr             Single Coil  의 어드레스 
   * @param [in] data              Single Coil 이 data, 1 or 0 의 값이다. 
   * @param [in] timeout        blocking 모드인경우 주어진 시간 내에 성공한 경우 깨어 나며, 실패한경우 주어진 시간이 
   모두 지난 다음 깨어 난다. -1인 경우 영원히 기다린다. 이 경우 만일 실패하는 경우 내부 timeout 에 의해서 깨어난다.
   깨어나는 시간은 내부 시스템 설정값에 종속적이다.
   * @return 에러의 경우 음수가 반환된다. non-blocking 모드의 경우 에러가 없다면 0 이 반환된다. blocking 모드의 경우 
   * 성공한 경우 data 버퍼에 채워진 size가 반환되므로 0 보다 큰값이 반환된다.
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbMasterWriteSingleCoil(int32_t fd, uint32_t slaveAddr, uint32_t addr, uint16_t data, int32_t timeout)
{
    int32_t ret;
    if( fd == (int32_t)&oMbMaster){
        ret =writeModbusSingleFunc(slaveAddr, addr, data,  timeout, FC_FORCE_SINGLE_COIL); 
    }
    return ret;
}
/**
   *********************************************************************************************************************************
   * @brief Function Code 6
   * @param [in] slaveAddr   slave의 국번
   * @param [in] addr             HoldingRegister  의 어드레스 
   * @param [in] data             Register의 data, 16비트값이다. 
   * @param [in] timeout        blocking 모드인경우 주어진 시간 내에 성공한 경우 깨어 나며, 실패한경우 주어진 시간이 
   모두 지난 다음 깨어 난다. -1인 경우 영원히 기다린다. 이 경우 만일 실패하는 경우 내부 timeout 에 의해서 깨어난다.
   깨어나는 시간은 내부 시스템 설정값에 종속적이다.
   * @return 에러의 경우 음수가 반환된다. non-blocking 모드의 경우 에러가 없다면 0 이 반환된다. blocking 모드의 경우 
   * 성공한 경우 data 버퍼에 채워진 size가 반환되므로 0 보다 큰값이 반환된다.
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbMasterWriteSingleReg( int32_t fd, uint32_t slaveAddr, uint32_t addr, uint16_t data, int32_t timeout)
{
    int ret;
    if( fd ==(int32_t)&oMbMaster){
        writeModbusSingleFunc(slaveAddr, addr, data, timeout, FC_PRESET_SINGLE_REGISTER); 
    }
    return ret;
}
/**
   *********************************************************************************************************************************
   * @brief Function Code 15
   * @param [in] slaveAddr   slave의 국번
   * @param [in] addr             Multiple  Coil  의 시작 어드레스 
   * @param [in] data             Coil data. 8bit 는 Coil 8개의 묶음이다. Coil주소가 1부터 10개라면 Coil[9-2] , Coil[ 
   11-10] 의 데이터를 가진다. 
   * @param [in] len              Multiple Coil 의 Write 하기 위한 개수
   * @param [in] timeout        blocking 모드인경우 주어진 시간 내에 성공한 경우 깨어 나며, 실패한경우 주어진 시간이 
   모두 지난 다음 깨어 난다. -1인 경우 영원히 기다린다. 이 경우 만일 실패하는 경우 내부 timeout 에 의해서 깨어난다.
   깨어나는 시간은 내부 시스템 설정값에 종속적이다.
   * @return 에러의 경우 음수가 반환된다. non-blocking 모드의 경우 에러가 없다면 0 이 반환된다. blocking 모드의 경우 
   * 성공한 경우 data 버퍼에 채워진 size가 반환되므로 0 보다 큰값이 반환된다.
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbMasterWriteMultiCoils( int32_t fd, uint32_t slaveAddr, uint32_t addr, uint8_t *data, 
uint16_t len, int32_t timeout)
{
    int ret;
    if( fd == (int32_t)&oMbMaster){
        ret =writeModbusFunc(slaveAddr, addr, data, len, timeout, FC_FORCE_MULTIPLE_COILS); 
    }
    return ret;
}
/**
   *********************************************************************************************************************************
   * @brief Function Code 15
   * @param [in] slaveAddr   slave의 국번
   * @param [in] addr             Register  의 시작 어드레스 
   * @param [in] data            16비트의 데이터이다.  시작 주소부터  D8[HI], D8[LO] 형태로 입력된다.
   * @param [in] len              Multiple Coil 의 Write 하기 위한 개수, 이 개수는 data의 byte수가 아니며 실제 
   Register의 개수가 된다. 즉 16비트가 1개이다. 
   * @param [in] timeout        blocking 모드인경우 주어진 시간 내에 성공한 경우 깨어 나며, 실패한경우 주어진 시간이 
   모두 지난 다음 깨어 난다. -1인 경우 영원히 기다린다. 이 경우 만일 실패하는 경우 내부 timeout 에 의해서 깨어난다.
   깨어나는 시간은 내부 시스템 설정값에 종속적이다.
   * @return 에러의 경우 음수가 반환된다. non-blocking 모드의 경우 에러가 없다면 0 이 반환된다. blocking 모드의 경우 
   * 성공한 경우 data 버퍼에 채워진 size가 반환되므로 0 보다 큰값이 반환된다.
   *********************************************************************************************************************************
   **/
PUBLIC int32_t oMbMasterWriteMultiRegs( int32_t fd, uint32_t slaveAddr, uint32_t addr, uint8_t *data, 
uint16_t len, int32_t timeout)
{
    int ret;
    if( fd == (int32_t)&oMbMaster){
        ret =writeModbusFunc(slaveAddr, addr, data, len, timeout, FC_PRESET_MULTIPLE_REGISTERS); 
    }
    return ret;
}

static void mbMasterTask (void *param)
{   
    MBMasterEventType    Event;
    int ret;

    //OMB_TCP 는 Master의 경우 TBD
    if(oMbMaster.mode == OMB_RTU){
        MBMasterInit(oMbMaster.mode, oMbMaster.port, oMbMaster.rtubaudrate);
    }
    MBMasterEnable();
    mbmaster1_run = 1;
    for(;;)
    {
        if( mbMasterPortEventGet(&Event) == TRUE){
            switch(Event)
            {
                case EV_MASTER_FRAME_SENT:
                    hLogProc(LOG_MODBUS, "evt=frame_sent\n");
                    break;
                case EV_MASTER_PROCESS_SUCESS:
                case EV_MASTER_FRAME_RECEIVED:
                    hLogProc(LOG_MODBUS, "evt=frame_rcved\n");
                    ret = mbMasterProcessRcvdData();
                    if(ret < 0){
                        mbMasterSetErrorCode(ret);
                        mbMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                    }else{
                        mbMasterReadValue(oMbMaster.rdata, oMbMaster.rdsize);
                        if(oMbMaster.asyncFunc == NULL){
                            if( oMbMasterGetState((int32_t)&oMbMaster) == OMB_READY){
                                oMbMasterSetState((int32_t)&oMbMaster, OMB_DONE);
                                hOsBiSemaSignal(oMbMaster.sema);
                            }
                        }else{
                            oMbMasterSetState((int32_t)&oMbMaster, OMB_DONE);
                            (void)oMbMaster.asyncFunc(OMB_ASYNC_SUCCESS);
                        }
                    }
                    break;
                case EV_MASTER_ERROR_PROCESS:
                    hLogProc(LOG_MODBUS, "evt=error_process\n");
                    if(oMbMaster.asyncFunc == NULL){
                        if( oMbMasterGetState((int32_t)&oMbMaster) == OMB_READY){
                            oMbMasterSetState((int32_t)&oMbMaster, OMB_ERROR);
                            hOsBiSemaSignal(oMbMaster.sema);
                        }

                    }else{
                        oMbMasterSetState((int32_t)&oMbMaster, OMB_ERROR);
                        (void)oMbMaster.asyncFunc(OMB_ASYNC_ERROR);                    
                    }
                    break;
            }
        }
    }
}



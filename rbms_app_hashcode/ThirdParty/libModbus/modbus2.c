/******************************************************************************/
/* modbus.c                                                                   */
/*                                                                            */
/* Based on "libmodbus-2.0.3" - Unix\Win version                              */
/* Author: Stephane Raimbault <stephane.raimbault@gmail.com>                  */
/*                                                                            */
/* Modifed for embedded system 02.2011 Sergei Maslyakov <rusoil.9@gmail.com>  */
/******************************************************************************/


#include "data_types.h"
#include "crc16.h"
#include "modbus.h"
#include "obshal.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "port_modbus.h"


/******************************************************************************/
/* Example                                                                    */
/******************************************************************************/
/*
   1. OpenMbMaster() - init uart, set slave addr and func
   2. Modbus_make_query() - create modbus query. Input structure see in
      modbus_def.h.
      Func return error(<0) or expected answer datalen.
      Expected answer datalen need for receive and parsing answer.

   3. Send query
   4. Wait answer and parsing
      If you used rtos, send and receive task desirable must have separate.
      Received data bytes need send in function Modbus_receive() and test
      return value.
   5. After receiving data - need run func Modbus_process_answ() - its func
      validate and select payload data from answer. Need test return value for
      error.
   6. CloseMbMaster() - need if you used driver rs485 or for free alloc memory
*/



/******************************************************************************/
/*      Static variables                                                      */
/******************************************************************************/
static __MB_ANSW_READY_DATA * s_pAnsw;
static __MB_QUERY_TOOLS       s_query;
static int32_t error_code;





//==============================================================================
// App: Modbus_change_order_bytes()
//==============================================================================
static void Modbus_change_order_bytes(uint8_t * const _pBuff,
                                      const uint32_t _dataLen) {

    uint8_t swop;
    uint32_t i;
    for ( i = 0; i < _dataLen; i++) {
        swop = _pBuff[i + 1];
        _pBuff[i + 1] = _pBuff[i];
        _pBuff[i] = swop;
        i++;
    };
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Compute_response_length()
//==============================================================================
static uint32_t Compute_response_length(const uint8_t * const _pQuery) {

    uint32_t length;

    switch (_pQuery[1])
    {
        //----------------------------------------------------------------------
        case FC_READ_COIL_STATUS:
        //----------------------------------------------------------------------
        case FC_READ_INPUT_STATUS:
          {
              // Header + nb values (code from force_multiple_coils)
              uint32_t nb = (_pQuery[4] << 8) | _pQuery[5];
              length = 3 + (nb / 8) + ((nb % 8) ? 1 : 0);
          };
        break;

        //----------------------------------------------------------------------
        case FC_READ_HOLDING_REGISTERS:
        //----------------------------------------------------------------------
        case FC_READ_INPUT_REGISTERS:
          // Header + 2 * nb values
          length = 3 + 2 * ((_pQuery[4] << 8) | _pQuery[5]);
        break;

        //----------------------------------------------------------------------
        case FC_FORCE_SINGLE_COIL:
        //----------------------------------------------------------------------
        case FC_PRESET_SINGLE_REGISTER:
        //----------------------------------------------------------------------
        case FC_FORCE_MULTIPLE_COILS:
        //----------------------------------------------------------------------
        case FC_PRESET_MULTIPLE_REGISTERS:
          length = 6;
        break;

        //----------------------------------------------------------------------
        default:
          return MSG_LENGTH_UNDEFINED;
    };

    return (length + CHECKSUM_LENGTH_RTU);
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Build_query_basis_rtu()
//==============================================================================
static uint32_t Build_query_basis_rtu(const uint8_t _slave,
                                      const uint8_t _function,
                                      const uint16_t _startAddr,
                                      const uint16_t _nb,
                                      uint8_t * const _pQuery) {

    _pQuery[0] = _slave;
    _pQuery[1] = _function;
    _pQuery[2] = _startAddr >> 8;
    _pQuery[3] = _startAddr & 0x00ff;
    //big endian 
    _pQuery[4] = _nb >> 8;
    _pQuery[5] = _nb & 0x00ff;

    return PRESET_QUERY_LENGTH_RTU;
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Check_crc16()
//==============================================================================
static int32_t Check_crc16(const uint8_t * const _pMsg,
                            const uint32_t _msgLength) {

    const uint16_t crc_calc = ModbusCrc16(_pMsg, _msgLength - 2);
    const uint16_t crc_received = (_pMsg[_msgLength - 1] << 8)
                                      | _pMsg[_msgLength - 2];

    // Check CRC
    return (crc_calc == crc_received) ? MB_MASTER_ERR_OK : MB_MASTER_ERR_CRC;
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Modbus_add_crc()
//==============================================================================
static uint32_t Modbus_add_crc(uint8_t * const _pQuery,
                               const uint32_t _queryLength) {

    const uint16_t crc = ModbusCrc16(_pQuery, _queryLength);
    _pQuery[_queryLength] = (uint8_t)(crc & 0x00FF);
    _pQuery[_queryLength + 1] = (uint8_t)(crc >> 8);
    return 2;
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Read_io_status()
//==============================================================================
static void Read_io_status(const uint8_t _slave,
                           const uint8_t _function,
                           const uint16_t _startAddr,
                           const uint16_t _nb) {

    s_query.queryLen = Build_query_basis_rtu(_slave,
                                             _function, 
                                             _startAddr,
                                             _nb,
                                             s_query.pQuery);

    s_query.queryLen += Modbus_add_crc(s_query.pQuery, s_query.queryLen);
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Read_registers()
//==============================================================================
static int32_t Read_registers(const uint8_t _slave,
                               const uint8_t _function,
                               const uint16_t _startAddr,
                               const uint16_t _nb) {
    if (_nb > MAX_REGISTERS) {
        return MB_MASTER_ERR_LEN;
    };

    s_query.queryLen = Build_query_basis_rtu(_slave,
                                             _function,
                                             _startAddr,
                                             _nb,
                                             s_query.pQuery);

    s_query.queryLen += Modbus_add_crc(s_query.pQuery, s_query.queryLen);

    return MB_MASTER_ERR_OK;
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Set_single()
//==============================================================================
static void Set_single(const uint8_t _slave,
                       const uint8_t _function,
                       const uint16_t _addr,
                       const uint16_t _value) {

    s_query.queryLen = Build_query_basis_rtu(_slave,
                                             _function, 
                                             _addr,
                                             _value,
                                             s_query.pQuery);

    s_query.queryLen += Modbus_add_crc(s_query.pQuery, s_query.queryLen);
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Read_coil_status()
//==============================================================================
static int32_t Read_coil_status(const uint8_t _slave,
                                 const uint16_t _startAddr,
                                 const uint16_t _nb) {
    if (_nb > MAX_STATUS) {
        return MB_MASTER_ERR_LEN;
    };

    Read_io_status(_slave, FC_READ_COIL_STATUS, _startAddr, _nb);

    return MB_MASTER_ERR_OK;
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Read_input_status()
//==============================================================================
static int32_t Read_input_status(const uint8_t _slave,
                                  const uint16_t _startAddr,
                                  const uint16_t _nb) {
    if (_nb > MAX_STATUS) {
        return MB_MASTER_ERR_LEN;
    };

    Read_io_status(_slave, FC_READ_INPUT_STATUS, _startAddr, _nb);

    return MB_MASTER_ERR_OK;
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Read_holding_registers()
//==============================================================================
static int32_t Read_holding_registers(const uint8_t _slave,
                                       const uint16_t _startAddr,
                                       const uint16_t _nb) {
    if (_nb > MAX_REGISTERS) {
        return MB_MASTER_ERR_LEN;
    };

    return Read_registers(_slave,
                          FC_READ_HOLDING_REGISTERS,
                          _startAddr,
                          _nb);
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Read_input_registers()
//==============================================================================
static int32_t Read_input_registers(const uint8_t _slave,
                                     const uint16_t _startAddr,
                                     const uint16_t _nb) {
    if (_nb > MAX_REGISTERS) {
        return MB_MASTER_ERR_LEN;
    };

    return Read_registers(_slave,
                          FC_READ_INPUT_REGISTERS,
                          _startAddr,
                          _nb);
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Force_single_coil()
//==============================================================================
static void Force_single_coil(const uint8_t _slave,
                              const uint16_t _coilAddr,
                              uint32_t _state) {
    if (_state) {
        _state = 0xFF00;
    };

    Set_single(_slave, FC_FORCE_SINGLE_COIL, _coilAddr, _state);
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Preset_single_register()
//==============================================================================
static void Preset_single_register(const uint8_t _slave,
                                   const uint16_t _regAddr,
                                   const uint16_t _value) {

    Set_single(_slave, FC_PRESET_SINGLE_REGISTER, _regAddr, _value);
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Force_multiple_coils()
//==============================================================================
static int32_t Force_multiple_coils(const uint8_t _slave,
                                     const uint16_t _startAddr,
                                     const uint16_t _nb,
                                     const uint8_t * const _pData) {
    uint32_t byteCount; 
    uint32_t coilCheck = 0;
    uint32_t pos = 0,i;

	if (_nb > MAX_STATUS) {
        return MB_MASTER_ERR_LEN;
    };

    s_query.queryLen = Build_query_basis_rtu(_slave,
                                             FC_FORCE_MULTIPLE_COILS, 
                                             _startAddr,
                                             _nb,
                                             s_query.pQuery);

    byteCount = (_nb / 8) + ((_nb % 8) ? 1 : 0);
    s_query.pQuery[s_query.queryLen++] = byteCount;


    for (i = 0; i < byteCount; i++) {
        int32_t bit;

        bit = 0x01;
        s_query.pQuery[s_query.queryLen] = 0;

        while ((bit & 0xFF) && (coilCheck++ < _nb)) {

            if (_pData[pos++]) {
                s_query.pQuery[s_query.queryLen] |= bit;
            }
            else {
                s_query.pQuery[s_query.queryLen] &=~ bit;
            };
            bit = bit << 1;
        };
        s_query.queryLen++;
    };

    s_query.queryLen += Modbus_add_crc(s_query.pQuery, s_query.queryLen);

    return MB_MASTER_ERR_OK;
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: Preset_multiple_registers()
//==============================================================================
static int32_t Preset_multiple_registers(const uint8_t _slave,
                                          const uint16_t _startAddr,
                                          const uint16_t _nb,
                                          const uint8_t * const _pData) {
    uint32_t i;
	if (_nb > MAX_REGISTERS) {
        return MB_MASTER_ERR_LEN;
    };

    s_query.queryLen = Build_query_basis_rtu(_slave,
                                             FC_PRESET_MULTIPLE_REGISTERS, 
                                             _startAddr,
                                             _nb,
                                             s_query.pQuery);

    s_query.pQuery[s_query.queryLen++] = _nb * 2;

    for ( i = 0; i < (uint32_t)_nb*2; i+=2) {
        
        if( mbMasterPortGetByteOrder() == FALSE){   //case 'bigendian'
        s_query.pQuery[s_query.queryLen++] = _pData[i];
        s_query.pQuery[s_query.queryLen++] = _pData[i + 1];
        }else{                                   //case 'littleendian'
            s_query.pQuery[s_query.queryLen++] = _pData[i+1];
            s_query.pQuery[s_query.queryLen++] = _pData[i];
        }
    };

    s_query.queryLen += Modbus_add_crc(s_query.pQuery, s_query.queryLen);

    return MB_MASTER_ERR_OK;
}
//==============================================================================
//==============================================================================


//==============================================================================
// App: User_func()
//==============================================================================
static int32_t User_func(const uint8_t _slave,
                          const uint8_t _function,
                          const uint16_t _dataLen,
                          const uint8_t * const _pData) {
    uint16_t i;
	// Test
    if (_dataLen > (MAX_REGISTERS * 2)) {
        return MB_MASTER_ERR_LEN;
    };

    if (_function > 127) {
        return MB_MASTER_ERR_FUNC;
    };

    // Make query
    s_query.queryLen = 0;

    s_query.pQuery[s_query.queryLen++] = _slave;
    s_query.pQuery[s_query.queryLen++] = _function;

    for ( i = 0; i < _dataLen; i++) {
        s_query.pQuery[s_query.queryLen++] = _pData[i];
    };

    // Add crc
    s_query.queryLen += Modbus_add_crc(s_query.pQuery, s_query.queryLen);

    return MB_MASTER_ERR_OK;
}
static uint8_t mbData2[MAX_MESSAGE_LENGTH];
static uint8_t mbRdData2[MAX_MESSAGE_LENGTH];
static __MB_ANSW_READY_DATA  mbAnsw2;
static uint32_t mbMasterState = MBMSATER_NOT_INIT ;
static uint32_t mbMasterRcvState;
static uint32_t mbMasterSndState;
//static uint32_t mbMasterTmode;

static void MBMaserCallbackTimerExpired2(void);

/* ----------------------------------------------------------------------------------------
    Static Function
 ----------------------------------------------------------------------------------------*/

static void mbMasterRTUStart2(void)
{
    mbMasterRcvState = STATE_M_RX_INIT;
    mbMasterPortSerialEnable2(TRUE, FALSE);
    mbMasterPortTimerT35Enable2(); 
}
#if 0
static void mbMasterRTUStop2(void)
{
    mbMasterPortSerialEnable2( FALSE, FALSE );
    mbMasterPortTimersDisable2(  );    
}
#endif

PUBLIC void mbMasterRTUSend2(uint32_t slaveAddr, uint32_t func, uint32_t addr, uint32_t val, uint8_t *muldata )
{
    MB_QUERY q;

    if(mbMasterRcvState == STATE_M_RX_IDLE)
    {
        q.addr = addr;
        q.value = val;
        q.pData = muldata;
        mbMasterFlushData2();
        mbMasterSetMode2(slaveAddr, func);
        mbMasterMakeQuery2(&q);    
        mbMasterSndState = STATE_M_TX_XMIT;
        mbMasterPortSerialEnable2(FALSE, TRUE);
    }
}

static void mbMasterRTUTransmit2(void)
{
    int temp;
    switch(mbMasterSndState)
    {
    case STATE_M_TX_IDLE:
        /* enable receiver/disable transmitter. */
        mbMasterPortSerialEnable2( TRUE, FALSE );
        break;
    case STATE_M_TX_XMIT:
        for(temp=0;temp<s_query.queryLen;temp++)
        {
            mbMasterPortSerialPutByte2(*(s_query.pQuery+temp));
        }
        mbMasterPortSerialSend2();
        mbMasterSndState = STATE_M_TX_XFWR;
        break;
    case STATE_M_TX_XFWR:
        mbMasterPortSerialEnable2(TRUE,FALSE);
        mbMasterPortTimerRspTimeoutEnable2();
        break;
        
    }
}

static void mbMasterRTUReceived2()
{
    char data;
    switch(mbMasterRcvState)
    {
    case STATE_M_RX_INIT:
        //while(mbMasterPortSerialGetByte(&data));
        mbMasterPortTimerT35Enable2();
        break;
    case STATE_M_RX_ERROR:
        //error  발생후 수신된 데이터는 모두 버린다.
        //while(mbMasterPortSerialGetByte(&data));
        mbMasterPortTimerT35Enable2();
        break;
    case STATE_M_RX_IDLE: // timer expired 된 경우 에러처리후 state가 IDLE로 간다.
        //error  발생후 수신된 데이터는 모두 버린다.
        mbMasterPortTimersDisable2();
        mbMasterSndState = STATE_M_TX_IDLE;
        mbMasterPortTimerT35Enable2();
        mbMasterRcvState = STATE_M_RX_RCV;
        break;
    case STATE_M_RX_RCV:
        while(mbMasterPortSerialGetByte2(&data))
        {
            mbMasterReceive2(data);
        }
        mbMasterPortTimerT35Enable2();
        break;
    }
}

static void mbMasterRTUExipred2(void)
{
	htrace(LOG_MODBUS,"timeout s(%d)\n",mbMasterRcvState);
	switch (mbMasterRcvState)
	{
		/* Timer t35 expired. Startup phase is finished. */
	case STATE_M_RX_INIT:
		mbMasterPortEventPost2(EV_MASTER_READY);
		break;
    case STATE_M_RX_IDLE:
        mbMasterPortEventPost2( EV_MASTER_ERROR_PROCESS );
        break;
		/* A frame was received and t35 expired. Notify the listener that
		 * a new frame was received. */
	case STATE_M_RX_RCV:
		mbMasterPortEventPost2(EV_MASTER_FRAME_RECEIVED);
		break;

		/* An error occured while receiving the frame. */
	case STATE_M_RX_ERROR:
		//mbMasterSetErrorCode(EV_ERROR_RECEIVE_DATA);
		mbMasterPortEventPost2( EV_MASTER_ERROR_PROCESS );
		break;

		/* Function called in an illegal state. */
	default:
		
		break;
	}
	mbMasterRcvState = STATE_M_RX_IDLE;

	switch (mbMasterSndState)
	{
		/* A frame was send finish and convert delay or respond timeout expired.
		 * If the frame is broadcast,The master will idle,and if the frame is not
		 * broadcast.Notify the listener process error.*/
	case STATE_M_TX_XFWR:
			mbMasterSetErrorCode2(MB_MASTER_ERR_TXTIMEOUT);
			mbMasterPortEventPost2(EV_MASTER_ERROR_PROCESS);
		break;
		/* Function called in an illegal state. */
	default:
		break;
	}
	mbMasterSndState = STATE_M_TX_IDLE;

	mbMasterPortTimersDisable2( );
}
/* ----------------------------------------------------------------------------------------*/

PUBLIC void initmbMasterVariables2(){
	s_query.pQuery = &mbData2[0];
    s_pAnsw = &mbAnsw2;
    s_pAnsw->pAnsw = &mbRdData2[0]; 
    s_pAnsw->answLen = 0;
}


#if 0
/**
    ******************************************************************************
    *    @brief Modbus Device 를 Open 한다.
    *    @param   baudrate    device 통신 전송 속도
    *    @return   성공 - handle 값
                       실패 - 음수 에러 값
     ******************************************************************************
     */
PUBLIC int32_t mbMasterOpenDevice2(uint32_t baudrate)
{
    mbMasterPortSerialInit2( NULL, baudrate, 8, 0);
	mbMasterPortSerialSetTimeout2(20);
    return 0;
}
#endif    
/**
    ******************************************************************************
    *    @brief     새로운 Slave에게 Modbus 통신을 위해서 set 한다.
    *                 slave id  가 업데이트 되며, 원하는 function code를 set 한다.
    *                 이전 설정된 상태는 clear 된다.
    *    @param   slave   Slave의 국번 
    *    @param   func    Function Code 
    *    @return   성공 - handle 값
                       실패 - 음수 에러 값
     ******************************************************************************
 */
PUBLIC int32_t mbMasterSetMode2(uint8_t slave, uint8_t func)
{
    s_query.slave = slave;
    s_query.func = func;
    return 0;
}

/**
    ******************************************************************************
    *    @brief     Modbus Master의 Device 를 Close 한다.
    *    @param   없음
    *    @return   없음
                       
     ******************************************************************************
 */

PUBLIC void mbMasterCloseDevice2(void){
    mbMasterPortSerialClose2();
}

/**
    ******************************************************************************
    *    @brief mbMasterSetMode() 을 한 후에 시작 어드레스와 읽을 사이즈를 와 데이터를 설정 한다.
    *                                        data는 read function의 경우 필요하지 않다.
    *    @param[in]    pQueryData포인터  query 정보를 담는다.
    *    @return    성공- 예상되는 응답 사이즈, 0 인경우 - 사용되지 않는 function code 
    *                  실패 - 에러값 정보
    ******************************************************************************
*/
 PUBLIC int32_t mbMasterMakeQuery2(MB_QUERY* pQueryData) 
 {
    int32_t err = 0;

    switch (s_query.func)
    {
        //----------------------------------------------------------------------
        case FC_READ_COIL_STATUS:
          err = Read_coil_status(s_query.slave,
                                 pQueryData->addr,
                                 pQueryData->value);
        break;

        //----------------------------------------------------------------------
        case FC_READ_INPUT_STATUS:
          err = Read_input_status(s_query.slave,
                                  pQueryData->addr,
                                  pQueryData->value);
        break;

        //----------------------------------------------------------------------
        case FC_READ_HOLDING_REGISTERS:
          err = Read_holding_registers(s_query.slave,
                                       pQueryData->addr,
                                       pQueryData->value);
        break;

        //----------------------------------------------------------------------
        case FC_READ_INPUT_REGISTERS:
          err = Read_input_registers(s_query.slave,
                                     pQueryData->addr,
                                     pQueryData->value);
        break;

        //----------------------------------------------------------------------
        case FC_FORCE_SINGLE_COIL:
          Force_single_coil(s_query.slave,
                            pQueryData->addr,
                            pQueryData->value);
        break;

        //----------------------------------------------------------------------
        case FC_PRESET_SINGLE_REGISTER:
          Preset_single_register(s_query.slave,
                                 pQueryData->addr,
                                 pQueryData->value);
        break;

        //----------------------------------------------------------------------
        case FC_FORCE_MULTIPLE_COILS:
          err = Force_multiple_coils(s_query.slave,
                                     pQueryData->addr,
                                     pQueryData->value,
                                     pQueryData->pData);
        break;

        //----------------------------------------------------------------------
        case FC_PRESET_MULTIPLE_REGISTERS:
          err = Preset_multiple_registers(s_query.slave,
                                          pQueryData->addr,
                                          pQueryData->value,
                                          pQueryData->pData);
        break;

        //----------------------------------------------------------------------
        default:
          err = User_func(s_query.func,
                          s_query.slave,
                          pQueryData->value,
                          pQueryData->pData);
        break;
    };

    if (err < 0) {
        return err;
    };

    s_query.expectedLen = Compute_response_length(s_query.pQuery);
    return s_query.expectedLen;
}

PUBLIC void mbMasterSendData2(void)
{
    int temp ;
    for(temp=0;temp<s_query.queryLen;temp++)
    {
        mbMasterPortSerialPutByte2(*(s_query.pQuery+temp));
    }
}

PUBLIC void mbMasterFlushData2(void)
{
    initmbMasterVariables2();
    s_query.slave = s_query.func=s_query.expectedLen=s_query.queryLen = 0;  
}

 /**
     ******************************************************************************
     *    @brief         반환되는 값이 0이 될때 까지 계속 읽는다.
     *                    TODO: state 가 static 변수이기 떄문에 읽는 도중에 connection 이 끊어질때
     *                    state 가 어긋 날 수 있기 때문에 timeout을 설정하여 clear 하도록 한다. 
     *
     *    @param [in] byte 수신된 1 byte data  
     *    @return    성공- 수신이 완료된 경우 0을 반환, 다음 데이터가 존재하는 경우 지금까지 수신한 
     *                   사이즈가 반환된다.
     *                  실패 - 에러값 정보
     ******************************************************************************
 */
 
PUBLIC int32_t mbMasterReceive2(uint8_t _byte) 
{
    static uint32_t state;
    int32_t len;
    hSysLock();
    len = s_pAnsw->answLen;
    hSysUnlock();
    if ((len > MAX_MESSAGE_LENGTH)
        || (s_query.expectedLen > MAX_MESSAGE_LENGTH)) {
        return MB_MASTER_ERR_LEN;
    };

    // Init start parsing
    if (len == 0) {
        state = eMB_PARS_SLAVE_ADDR;
    };
    hSysLock();
    s_pAnsw->pAnsw[len++] = _byte;
    s_pAnsw->answLen = len;
    hSysUnlock();
    switch (state)
    {
        //----------------------------------------------------------------------
        case eMB_PARS_SLAVE_ADDR:
          state++;
          if (s_pAnsw->pAnsw[0] != s_query.slave) {
              return MB_MASTER_ERR_SLAVE;
          }
          else {
              return len;
          };

        //----------------------------------------------------------------------
        case eMB_PARS_FUNC:
          state++;
          if (s_pAnsw->pAnsw[1] != s_query.func) {
              s_query.expectedLen = 5;

              if (s_pAnsw->pAnsw[1] != (s_query.func | 0x80)) {
                  return MB_MASTER_ERR_FUNC;
              };
          };
          return len;

        //----------------------------------------------------------------------
        case eMB_PARS_DATA:
          if (s_query.expectedLen != len) {
              return len;
          };
        break;

        //----------------------------------------------------------------------
        default:
          return MB_MASTER_ERR_UNDEF;
    };

    return Check_crc16(s_pAnsw->pAnsw, len);
}
 /**
     ******************************************************************************
     *    @brief        수신된 Response 데이터를 처리한다.
     *                     mbMasterReceive() 처리한후 더이상 수신할 데이터가 없는 경우 호출된다.
     *
     *    @param [in] 없음 
     *    @return    성공- 0
     *                  실패 - 에러값 정보
     ******************************************************************************
 */
PUBLIC int32_t mbMasterProcessRcvdData2(void) 
{
    int32_t  len, pos=0 ;
    hSysLock();
    len = s_pAnsw->answLen;
    hSysUnlock();
    // Test at user func (need test crc or not)
    if (len != s_query.expectedLen) {
        //err = Check_crc16(s_pAnsw->pAnsw, s_pAnsw->answLen);

        //if (err != MB_MASTER_ERR_OK) {
        return MB_MASTER_ERR_LEN;
        //};
    };

    // Test on modbus exception
    if (s_query.func == (s_pAnsw->pAnsw[1] | 0x80)) {
        s_pAnsw->errCode = s_pAnsw->pAnsw[2];

        return MB_MASTER_ERR_OK;
    };

    // Calc payload data
    s_pAnsw->errCode = 0;

    switch (s_query.func)
    {
        //----------------------------------------------------------------------
        case FC_READ_COIL_STATUS:
        //----------------------------------------------------------------------
        case FC_READ_INPUT_STATUS:
          if ((len - 5) != ((uint32_t)s_pAnsw->pAnsw[2])) {
              return MB_MASTER_ERR_DATA;
          };
          pos +=3;
          len -=5;
        break;

        //----------------------------------------------------------------------
        case FC_READ_HOLDING_REGISTERS:
        //----------------------------------------------------------------------
        case FC_READ_INPUT_REGISTERS:
          if ((len - 5) != ((uint32_t)s_pAnsw->pAnsw[2])) {
              return MB_MASTER_ERR_DATA;
          };
          pos +=3;
          len -= 5;
          hSysLock();
          if(mbMasterPortGetByteOrder2()== TRUE){
            Modbus_change_order_bytes(s_pAnsw->pAnsw+pos, len);
          }
          hSysUnlock();
        break;

        //----------------------------------------------------------------------
        case FC_FORCE_SINGLE_COIL:
        //----------------------------------------------------------------------
        case FC_FORCE_MULTIPLE_COILS:
        //----------------------------------------------------------------------
        case FC_PRESET_SINGLE_REGISTER:
        //----------------------------------------------------------------------
        case FC_PRESET_MULTIPLE_REGISTERS:
        break;

        //----------------------------------------------------------------------
        default:
          pos +=2;
          len -=4;
        break;
    };
	hSysLock();
    s_pAnsw->pAnsw += pos;
    s_pAnsw->dataLen = len;
    hSysUnlock();
    return MB_MASTER_ERR_OK;
}

int32_t mbMasterReadValue2(uint16_t *buf, int size)
{
    int i, len;
    uint8_t *ptr;
    hSysLock();
    len = s_pAnsw->dataLen;
    hSysUnlock();
    /*
    if(len > size ) {
        htrace(LOG_WARN, "modbus bus copy error\n");
        return 0;
    }
    */
    ptr = (uint8_t *)buf;
    for(i=0;i<len;i++){
        *(ptr +i)= *(s_pAnsw->pAnsw+i);
    }
    return size;
}

void printRcvData2(int addr){
	int temp;
    char buf[128];
    char tbuf[4];
    int len=0;
    buf[0]=0;
 	for(temp=0;temp<s_pAnsw->answLen;temp++){
        //printf("%2x ",*(s_pAnsw->pAnsw+temp));
        sprintf(tbuf,"%2x ",*(s_pAnsw->pAnsw+temp));
        tbuf[3]=0;
        if(len < 128){ 
            strcat(buf,tbuf);
            len +=3;
        }
	}    
    htrace(LOG_MODBUS,"addr[%d] << %s\n",addr, buf);
}

/**
    *  @brief  Modbus Master Module을 초기화 한다.
    *      serial Port를 연다.
    *      통신 채널의 Time를 감시하는 timer를 하나 연다.
    **/   
PUBLIC void  MBMasterInit2( uint16_t eMode, uint32_t ucPort, uint32_t ulBaudRate)
{
    uint32_t utime;
    mbMasterPortSerialInit2(ucPort, ulBaudRate,8,0);
    if(ulBaudRate > 19200){
        utime = 2800;
    }else{
        utime = 50000;
    }
	mbMasterPortTimerInit2(utime, MBMaserCallbackTimerExpired2 );
}

PUBLIC int32_t MBMasterEnable2(void)
{
    
    mbMasterState = MBMASTER_ENABLE;
    mbMasterRTUStart2();
    return mbMasterState;

}

PUBLIC void MBMasterSetCurTimerMode2(uint32_t mode)
{
 //   mbMasterTmode = mode;   

}

static void MBMaserCallbackTimerExpired2(void)
{
    mbMasterRTUExipred2();
}

PUBLIC void mbMasterCallbackTrasmitterEmpty2(void)
{
    mbMasterRTUTransmit2();
}

PUBLIC void mbMasterCallbackByteReceived2(void){
    mbMasterRTUReceived2();
}

PUBLIC void mbMasterSetErrorCode2(int code)
{
    error_code = code;
}

PUBLIC int mbMasterGetErrorCode2(void)
{
    return error_code;
}

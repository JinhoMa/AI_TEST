/******************************************************************************/
/* modbus.h                                                                   */
/*                                                                            */
/* Based on "libmodbus-2.0.3" - Unix\Win version                              */
/* Author: Stephane Raimbault <stephane.raimbault@gmail.com>                  */
/*                                                                            */
/* Modifed for embedded system 02.2011 Sergei Maslyakov <rusoil.9@gmail.com>  */
/******************************************************************************/


#ifndef __MODBUS_H
#define __MODBUS_H

#include "htypes.h"
#include  <stdint.h>

#include "modbus_def.h"

enum MbMasterSate_{
    MBMASTER_ENABLE,
    MBMASTER_DISABLED,
    MBMSATER_NOT_INIT
}  ; 

typedef enum
{
    STATE_M_RX_INIT,              /*!< Receiver is in initial state. */
    STATE_M_RX_IDLE,              /*!< Receiver is in idle state. */
    STATE_M_RX_RCV,               /*!< Frame is beeing received. */
    STATE_M_RX_ERROR,              /*!< If the frame is invalid. */
} MBMasterRcvState;

typedef enum
{
    STATE_M_TX_IDLE,              /*!< Transmitter is in idle state. */
    STATE_M_TX_XMIT,              /*!< Transmitter is in transfer state. */
    STATE_M_TX_XFWR,              /*!< Transmitter is in transfer finish and wait receive state. */
} MBMasterSndState;

typedef enum
{
	MBMASTER_TMODE_T35,                   /*!< Master receive frame T3.5 timeout. */
	MBMASTER_TMODE_RESPOND_TIMEOUT,       /*!< Master wait respond for slave. */
	MBMASTER_TMODE_CONVERT_DELAY          /*!< Master sent broadcast ,then delay sometime.*/
} MBMasterTimerMode;


/**************************** Func declarations *******************************/
PUBLIC void  MBMasterInit( uint16_t eMode, const uint8_t* ucPort, uint32_t ulBaudRate);
PUBLIC void  MBMasterInit2( uint16_t eMode, uint32_t ucPort, uint32_t ulBaudRate);
void mbMasterRTUSend(uint32_t slaveAddr, uint32_t func, uint32_t addr, uint32_t val, uint8_t *muldata );
void mbMasterRTUSend2(uint32_t slaveAddr, uint32_t func, uint32_t addr, uint32_t val, uint8_t *muldata );


PUBLIC void MBMasterSetCurTimerMode(uint32_t mode);
PUBLIC int32_t MBMasterEnable(void);

PUBLIC int32_t mbMasterOpenDevice(uint32_t baudrate);

PUBLIC int32_t mbMasterSetMode(uint8_t slave, uint8_t func);

PUBLIC void mbMasterCloseDevice(void);

PUBLIC int32_t mbMasterMakeQuery(MB_QUERY* _pQueryData);

PUBLIC int32_t mbMasterReceive(uint8_t _byte) ;

PUBLIC int32_t mbMasterProcessRcvdData(void) ;

PUBLIC int32_t mbMasterReadValue(uint16_t *buf, int size);

PUBLIC void mbMasterCallbackByteReceived(void);
PUBLIC void mbMasterCallbackTrasmitterEmpty(void);
PUBLIC void mbMasterFlushData(void);
PUBLIC void mbMasterSendData(void);
PUBLIC void initmbMasterVariables(void);

PUBLIC void mbMasterSetErrorCode(int code);


PUBLIC void MBMasterSetCurTimerMode2(uint32_t mode);
PUBLIC int32_t MBMasterEnable2(void);

PUBLIC int32_t mbMasterOpenDevice2(uint32_t baudrate);

PUBLIC int32_t mbMasterSetMode2(uint8_t slave, uint8_t func);

PUBLIC void mbMasterCloseDevice2(void);

PUBLIC int32_t mbMasterMakeQuery2(MB_QUERY* _pQueryData);

PUBLIC int32_t mbMasterReceive2(uint8_t _byte) ;

PUBLIC int32_t mbMasterProcessRcvdData2(void) ;

PUBLIC int32_t mbMasterReadValue2(uint16_t *buf, int size);

PUBLIC void mbMasterCallbackByteReceived2(void);
PUBLIC void mbMasterCallbackTrasmitterEmpty2(void);
PUBLIC void mbMasterFlushData2(void);
PUBLIC void mbMasterSendData2(void);
PUBLIC void initmbMasterVariables2(void);

PUBLIC void mbMasterSetErrorCode2(int code);

#endif

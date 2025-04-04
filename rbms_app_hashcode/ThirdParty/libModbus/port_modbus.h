#ifndef __PORT_MODBUS_H__
#define __PORT_MODBUS_H__
typedef enum
{
    EV_MASTER_READY                    = 1<<0,  /*!< Startup finished. */
    EV_MASTER_FRAME_RECEIVED           = 1<<1,  /*!< Frame received. */
    EV_MASTER_EXECUTE                  = 1<<2,  /*!< Execute function. */
    EV_MASTER_FRAME_SENT               = 1<<3,  /*!< Frame sent. */
    EV_MASTER_ERROR_PROCESS            = 1<<4,  /*!< Frame error process. */
    EV_MASTER_PROCESS_SUCESS           = 1<<5,  /*!< Request process success. */
    EV_MASTER_ERROR_RESPOND_TIMEOUT    = 1<<6,  /*!< Request respond timeout. */
    EV_MASTER_ERROR_RECEIVE_DATA       = 1<<7,  /*!< Request receive data error. */
    EV_MASTER_ERROR_EXECUTE_FUNCTION   = 1<<8,  /*!< Request execute function error. */
} MBMasterEventType;



/*! \brief If master send a broadcast frame,the master will wait time of convert to delay,
 * then master can send other frame */
#define MBMASTER_DELAY_MS_CONVERT              (200 )
/*! \brief If master send a frame which is not broadcast,the master will wait sometime for slave.
 * And if slave is not respond in this time,the master will process this timeout error.
 * Then master can send other frame */
#define MBMASTER_TIMEOUT_MS_RESPOND            (300 )
/*! \brief The total slaves in Modbus Master system. Default 16.
 * \note : The slave ID must be continuous from 1.*/
#define MBMASTER_TOTAL_SLAVE_NUM               ( 16 )
/**
    ******************************************************************************
    *     @file      port_modbus.h 
    *     @author    
    *     @version   
    *     @date      2015.04.05 
    *     @brief     
    ******************************************************************************
    */
//serial
PUBLIC uint32_t mbMasterPortSerialInit( const uint8_t* ucPort, uint32_t ulBaudRate, uint8_t ucDataBits,
                   int eParity );

PUBLIC void mbMasterPortSerialEnable( int32_t bEnableRx, int32_t bEnableTx );
PUBLIC uint32_t mbMasterPortSerialPutByte( char  ucByte );
PUBLIC uint32_t mbMasterPortSerialGetByte( char * pucByte );
PUBLIC void mbMasterPortSerialSend(void);
PUBLIC void mbMasterPortSerialClose ( void );


//timer
PUBLIC uint32_t mbMasterPortTimerInit(uint32_t  usec, void *asyncFunc );
PUBLIC void mbMasterPortTimerT35Enable(void);
PUBLIC void mbMasterPortTimersDisable(void);
PUBLIC void mbMasterPortTimerRspTimeoutEnable(void);

// Event
int32_t mbMasterPortEventGet( MBMasterEventType * eEvent );
int32_t mbMasterPortEventPost( MBMasterEventType eEvent );
//ETC
PUBLIC uint32_t mbMasterPortGetByteOrder(void);

PUBLIC uint32_t mbMasterPortSerialInit2( uint8_t ucPort, uint32_t ulBaudRate, uint8_t ucDataBits,
                   int eParity );

PUBLIC void mbMasterPortSerialEnable2( int32_t bEnableRx, int32_t bEnableTx );
PUBLIC uint32_t mbMasterPortSerialPutByte2( char  ucByte );
PUBLIC uint32_t mbMasterPortSerialGetByte2( char * pucByte );
PUBLIC void mbMasterPortSerialSend2(void);
PUBLIC void mbMasterPortSerialClose2 ( void );
PUBLIC uint32_t mbMasterPortTimerInit2(uint32_t  usec, void *asyncFunc );
PUBLIC void mbMasterPortTimerT35Enable2(void);
PUBLIC void mbMasterPortTimersDisable2(void);
PUBLIC void mbMasterPortTimerRspTimeoutEnable2(void);
int32_t mbMasterPortEventGet2( MBMasterEventType * eEvent );
int32_t mbMasterPortEventPost2( MBMasterEventType eEvent );
PUBLIC uint32_t mbMasterPortGetByteOrder2(void);

#endif //__PORT_MODBUS_H__

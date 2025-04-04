/******************************************************************************/
/* modbus_def.c                                                               */
/*                                                                            */
/* Based on "libmodbus-2.0.3" - Unix\Win version                              */
/* Author: Stephane Raimbault <stephane.raimbault@gmail.com>                  */
/*                                                                            */
/* Modifed for embedded system 02.2011 Sergei Maslyakov <rusoil.9@gmail.com>  */
/******************************************************************************/

#ifndef __MODBUS_DEF_H
#define __MODBUS_DEF_H


/************************* MB_MASTER constants ********************************/
#define HEADER_LENGTH_RTU               (0)
#define PRESET_QUERY_LENGTH_RTU         (6)
#define PRESET_RESPONSE_LENGTH_RTU      (2)
#define CHECKSUM_LENGTH_RTU             (2)
#define MIN_QUERY_LENGTH                (8)
#define MIN_ANSWER_LENGTH               (5)
#define MAX_MESSAGE_LENGTH              (256)

#define MAX_STATUS                      (512)
#define MAX_REGISTERS                   (32)

#define REPORT_SLAVE_ID_LENGTH          (75)

#define MB_OFF                          (0)
#define MB_ON                           (1)


// Function codes
#define FC_READ_COIL_STATUS             (0x01)
#define FC_READ_INPUT_STATUS            (0x02)
#define FC_READ_HOLDING_REGISTERS       (0x03)
#define FC_READ_INPUT_REGISTERS         (0x04)
#define FC_FORCE_SINGLE_COIL            (0x05)
#define FC_PRESET_SINGLE_REGISTER       (0x06)
#define FC_READ_EXCEPTION_STATUS        (0x07)
#define FC_FORCE_MULTIPLE_COILS         (0x0F)
#define FC_PRESET_MULTIPLE_REGISTERS    (0x10)
#define FC_REPORT_SLAVE_ID              (0x11)


// Protocol exceptions
#define ILLEGAL_FUNCTION                (0x01)
#define ILLEGAL_DATA_ADDRESS            (0x02)
#define ILLEGAL_DATA_VALUE              (0x03)
#define SLAVE_DEVICE_FAILURE            (0x04)
#define SERVER_FAILURE                  (0x04)
#define ACKNOWLEDGE                     (0x05)
#define SLAVE_DEVICE_BUSY               (0x06)
#define NEGATIVE_ACKNOWLEDGE            (0x07)
#define MEMORY_PARITY_ERROR             (0x08)


// Internal using
#define MSG_LENGTH_UNDEFINED            (0)

#define MB_MASTER_ERR_OK                (0)
#define MB_MASTER_ERR_LEN               (-1)
#define MB_MASTER_ERR_SLAVE             (-2)
#define MB_MASTER_ERR_FUNC              (-3)
#define MB_MASTER_ERR_CRC               (-4)
#define MB_MASTER_ERR_UNDEF             (-5)
#define MB_MASTER_ERR_TXTIMEOUT           (-6)
#define MB_MASTER_ERR_DATA              (-7)
#define MB_MASTER_ERR_RXTIMEOUT           (-8)


/********************** Prototypes struct for MB_MASTER ***********************/
typedef enum
{
    eMB_PARS_SLAVE_ADDR = 0,
    eMB_PARS_FUNC,
    eMB_PARS_DATA
} __MB_PARS_ANSW;


typedef struct _MB_QUERY
{
    uint16_t  addr;
    uint16_t  value;
    uint8_t * pData;
} MB_QUERY;


typedef struct
{
    uint8_t * pAnsw;
    uint32_t  answLen;
    uint8_t   errCode;
    uint32_t  dataLen;
} __MB_ANSW_READY_DATA;


typedef struct
{
    __MB_ANSW_READY_DATA answ;
    MB_QUERY     query;
} __MB_QUERY_SEND;


typedef struct
{
    uint8_t   slave;
    uint8_t   func;
    uint32_t  expectedLen;
    uint32_t  queryLen;
    uint8_t * pQuery;
} __MB_QUERY_TOOLS;



#endif

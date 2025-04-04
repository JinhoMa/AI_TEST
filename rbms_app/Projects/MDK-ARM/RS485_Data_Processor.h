/*
 * <모듈 요약>
 * <모듈 상세 설명>
 */

#ifndef __RS485_DATA_PROCESSOR_H__
#define __RS485_DATA_PROCESSOR_H__
#include "obshal.h"
#include "main.h"
#include "BMS.h"
#include "SYS_ERR_CHK.h"

#ifdef __cplusplus
extern "C" {
#endif

/*== MACRO ===================================================================*/
#define COM_ERROR           1       // 통신 에러(Communication Error)
#define OTP                 2       // 고온 에러(Over Temperature)
#define UTP                 3       // 저온 에러(Under Temperature)
#define OCC                 4       // 과충전 전류 에러(Overcharge Current)
#define ODC                 5       // 과방전 전류 에러(Overdischarge Current)
#define OPV                 6       // 과충전 전압 에러(Over Pack Voltage)
#define UPV                 7       // 과방전 전압 에러(Under Pack Voltage)
#define CONT                8       // 컨택터 에러(Contactor Error)
#define OCV                 9       // 과충전 셀 전압 에러(Overcharge Cell Voltage)
#define UCV                 10      // 과방전 셀 전압 에러(Overdischarge Cell Voltage)
#define SM_EEPCHKSUM        11      // EEPROM checksum 에러
#define SM_MCUFAULT         12      // MCU 체크 에러
#define SM_VOLTAGE          13      // 전압 타당성 에러
#define SM_MEM              14      // 메모리 체크 에러
#define SM_SUBMCU_STUCK     15      // SUBMCU(RAJ) Stuck 에러

#define COMMAND_SUCCESS     1
#define COMMAND_FAIL        2

/*== TYPE DEFINITION =========================================================*/

typedef union    // 66 bytes
{
    __packed struct _BMS_Data {
        uint16_t header;
        uint8_t  addr;
        uint8_t  command;
        uint16_t BV;
        uint16_t PV;
        int16_t  C;
        uint16_t P;
        uint8_t  SOC;
        uint16_t CV[16];
        int8_t   ExtT1;
        int8_t   ExtT2;
        int8_t   ExtT3;
        int8_t   ExtT4;
        int8_t   BdT1;
        int8_t   BdT2;
        uint8_t  Random_Seed;
        uint8_t  PackST;
        uint16_t CellBal;
        uint32_t StrEvt1;
        uint32_t StrEvt2;
        uint16_t ERRORCODE;
        uint16_t Capacity;
        uint8_t  Reserved1;
        uint8_t  chksum;
        uint8_t  Reserved2;
   }str;
    uint8_t buffer[sizeof(struct _BMS_Data)];
} Response_BMSData_Packet;
/*== FUNCTION DECLARATION ====================================================*/

PUBLIC void rs485_rdpacket_process(void);
PUBLIC void rs485_wrpacket_process(void);
PUBLIC void RS485Init(void);

#ifdef __cplusplus
}
#endif

#endif // __RS485_DATA_PROCESSOR_H__

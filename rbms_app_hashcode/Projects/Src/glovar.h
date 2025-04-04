#ifndef __GLOVAR_H__
#define __GLOVAR_H__

/**
 ******************************************************************************
 *     @file         glovar.h
 *     @version   
 *     @author    
 *     @date      2024/09/13 
 *     @brief     global 변수는 모두 여기에 define 합니다.
 ******************************************************************************
*/
extern STR_EEPROM_DataModel EEPROMData;
extern uint32_t diag_can_fd;
#ifdef _TBMS
#include "SubmicProcessData.h"
extern InputRegisters ir;
extern HoldRegisters hr;
extern RBMS_COM_TRAY RbmsComTray[TBMS_BUF_SIZE];
#endif
#ifdef _RBMS
#define STREV_COMM_ERROR            0x00000001
#define STREV_OVTEMP_ALARM          0x00000002
#define STREV_OVTEMP_WARN           0x00000004
#define STREV_UNTEMP_ALARM          0x00000008
#define STREV_UNTEMP_WARN           0x00000010
#define STREV_OVCHAR_CUR_ALARM      0x00000020
#define STREV_OVCHAR_CUR_WARN       0x00000040
#define STREV_OVDSCH_CUR_ALARM      0x00000080
#define STREV_OVDSCH_CUR_WARN       0x00000100
#define STREV_OVVOL_ALARM           0x00000200
#define STREV_OVVOL_WARN            0x00000400
#define STREV_UNVOL_ALARM           0x00000800
#define STREV_UNVOL_WARN            0x00001000
#define STREV_UNSOC_ALARM           0x00002000
#define STREV_UNSOC_WARN            0x00004000
#define STREV_OVSOC_ALARM           0x00008000
#define STREV_OVSOC_WARN            0x00010000
#define STREV_VOL_IMBAL_WARN        0x00020000
#define STREV_TEMP_IMBAL_ALARM      0x00040000
#define STREV_TEMP_IMBAL_WARN       0x00080000
#define STREV_CONTACTOR_ERROR       0x00100000
#define STREV_FAN_ERROR             0x00200000
#define STREV_GND_FAULT             0x00400000
#define STREV_OPENDOOR_ERR          0x00800000
#define STREV_RESERVED_1            0x01000000
#define STREV_OTHER_ALARM           0x02000000
#define STREV_OTHER_WARN            0x04000000


#define STREXTEV_OVCVOL_ALARM       0x00000001
#define STREXTEV_OVCVOL_WARN        0x00000002
#define STREXTEV_UNCVOL_ALARM       0x00000004
#define STREXTEV_UNCVOL_WARN        0x00000008
#define STREXTEV_OCD_ALARM          0x00000010

#define STRING_STATUS_RELAY_BIT    0x00000001
#define STRING_STATUS_ENABLE_BIT   0x00000000

typedef union {
    __packed struct _StringModel{
      uint32_t St; // String Status
      uint16_t SoC; // String State of Charge
      uint16_t DoD; // Optional. String Depth of Discharge
      uint32_t NCyc; // Optional String Cycle Count
      uint16_t SoH; // Optional. String State of Health
      int16_t A; // String Current (100mA 단위)
      uint16_t V; // Optional. String Voltage Pack Voltage (100mV 단위)
      uint16_t CellVMax; // Max Cell Voltage
      uint16_t CellVMaxMod; // Optional. Max Cell Voltage Module
      uint16_t CellVMin; // Min Cell Voltage
      uint16_t CellVMinMod; // Min Cell Voltage Module
      uint16_t CellVAvg; // Average Cell Voltage
      int16_t ModTmpMax; // Max Module Temperature
      uint16_t ModTmpMaxMod; // Max Module Temperature Module
      int16_t ModTmpMin; //Min Module Temperature
      uint16_t ModTmpMinMod; //Min Module Temperature
      int16_t ModTmpAvg; // Average Module Temperature
      uint32_t Evt1; // String Event 1
      uint32_t Evt2; // Optional. String Event 2
      uint32_t EvtVnd1; // Optional. Vendor Event Bitfield 1
      uint32_t EvtVnd2; // Optional. Vendor Event Bitfield 2
      uint16_t SetEna; // R/W, Optional. Enable/Disable String
      uint16_t SetCon; // R/W, Optional. Connect/Disconnect String
      uint16_t BatV;   // Battery Voltage (10mV 단위)
      uint16_t alrmRstCommand;
  } StringModel;
  uint16_t buffer[sizeof(struct _StringModel)]; 
} StringRegisters;

extern UNI_REC_D   RTRecDF;
// CURRENT , RC 관점 flag
extern  RACK_ST Rack_St;
extern StringRegisters R;

typedef enum{
    F_OCV = 0,
    F_CHG = 1,
    F_DIS = 2,
    F_FULLC = 3,
    F_FULLD = 4,
    F_PDOWN = 5,
    F_RCCHG = 6,
    F_ERROR = 0xEE,
    F_PFAIL = 0xEF,    
} ENUM_PACKST;

#define f_ocv       DEFS_BIT0(&Rack_St)
#define f_chg       DEFS_BIT1(&Rack_St)
#define f_dis       DEFS_BIT2(&Rack_St)
#define f_fullchg   DEFS_BIT3(&Rack_St)
#define f_fulldis   DEFS_BIT4(&Rack_St)
#define f_pdown     DEFS_BIT5(&Rack_St)
#define f_error     DEFS_BIT6(&Rack_St)
#define f_pfail     DEFS_BIT7(&Rack_St)

#endif

#endif //__GLOVAR_H__

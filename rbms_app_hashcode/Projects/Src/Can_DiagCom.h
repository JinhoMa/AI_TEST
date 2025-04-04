#ifndef __CAN_DIAGCOM_H__
#define __CAN_DIAGCOM_H__

#include "AppQueue.h"

/**
 ******************************************************************************
 *     @file         Can_DiagCom.h
 *     @version   
 *     @author    
 *     @date      2024/09/24 
 *     @brief     
 ******************************************************************************
*/
#define DIAG_PRT_REQ 0x7EF
#define DIAG_PRT_RSP  0x7F0
#define PLUSRESPONSE_ID 0x40
#define NEGATIVE_RESPONSE_LID 0x7F

#define N_NOACESS_MODE   0x01
#define N_INVALID_ID     0x02
#define N_CHECK_STATE  0x03
#define N_NOTSUPPORT    0x04
#define N_KEY_MISMATCH  0x27

#define LOGSTOP     0x00
#define LOGOUT      0x01

#define CHANGEMODE  0x10
    #define NORMAL_MODE 1
    #define DIAG_MODE   2
    
#define SECACCESS   0x27
    #define REQ_SEED    1
    #define SND_KEY     2
    #define KEY_A 0x9164
    #define KEY_C 0x1665
    #define KEY_E 0x0000FFFF
    #define SUCESSACCESS 0x01
    #define DENYACCESS 0x02
#define HANDLEDTC   0x19
    #define READ_DTC    1
    #define CLR_DTC     2
#define CELLBALANCE 0x20
    #define ENABLE_CB   0x1
    #define DISABLE_CB  0x2
    #define ON_ODD_CB   0x3
    #define ON_EVEN_CB 0x4
    #define ON_CB_OFF 0x5
    
    
#define FACTRESET   0x24
    #define FACT_RESET  1
#define HANDLETRAY   0x22
    #define CELL_NUM      1
    #define BLOCK_NUM     2
    #define MDTMP_NUM     3
    #define BDTMP_NUM     4
/////////////////////////////////////////////////////////////////
///   RACK
/////////////////////////////////////////////////////////////////
#define CTRLCMD     0x30
    #define DIAG_RELAY_ON    0x1
    #define DIAG_RELAY_OFF   0x2
#define READRACKSYS    0x40
    #define READ_CUR        0x01
    #define READ_PACKVOL    0x02
#define READTRAYS    0x42
    #define TRAY_CNT    0x01
    #define CELL_VOL    0xE0
    #define TRAY_TMP    0xE2
    #define INTLOCK     0xE3
    #define CELL_DEV    0xE4
#define TESTEEP     0x44
    #define WR_EEP  0x01
    #define RD_EEP  0x02    
#define TESTCONF   0x46
    #define READMODEL   0x01
    #define SWVER       0x02
    #define CHKSUM      0x03
    
#define ALMRST  0x60
    #define ALM_CLR 0x1

#define RELAY_CTRL  0xA0
    #define RELAY_MODE_ON    0xEE
    #define RELAY_CFET_ON    0x01
    #define RELAY_CFET_OFF   0x02
    #define RELAY_DFET_ON    0x11
    #define RELAY_DFET_OFF   0x12
    #define RELAY_MODE_OFF   0xFF
    
#define CALBAT      0xB0
#define CALPCS      0xB1
#define CALCCUR     0xB2
#define CAL_CV      0xB3
    #define CAL_CV_L 1
    #define CAL_CV_H 2
#define CALDCUR     0xB6
    #define CAL_REF_LO  1
    #define CAL_REF_MID 2
    #define CAL_REF_HI  3

#define CUR_SIMUL   0xC0
    #define CUR_SIMUL_ON  0x01
    #define CUR_SIMUL_OFF 0x00


#define FUSE_CTRL 0xF0
   #define FUSE_MODE_ON    0xEE
   #define FUSE_OFF   0x01
  

    
#define DTCINFO 0x19

#define READ_DTCINFO 0x01
#define CLEAR_DTCINFO 0x02
#define SUCESS_CLEARDTC     0x03
#define SUCESS_FACTRESET    0x04

typedef struct{
    uint16_t addr;
    uint8_t  len;
    uint8_t  value[4];
} EepModel;

typedef struct{
    uint8_t len;
    EepModel data[8];
} Eeprom;

typedef struct{
    uint8_t mn[16]; //model name
    uint8_t mv[8];  //model version
} HwModel;


typedef struct{
    uint8_t ver[2];
    uint8_t build[3];
} SwModel;

typedef union{
    Eeprom  eep;
    SwModel sm;
    HwModel hm;    
    uint16_t code;
} TestData; 

typedef struct
{
    MH mh;          //message header
    uint8_t lid ;
    uint8_t sid;
    TestData test;
} STR_CAN_MSG;
    
typedef struct
{
	uint16_t SeedInfo;
	uint16_t SecurityKey;
	uint16_t DummyValue;
}STR_Security;

typedef struct 
{
    uint8_t cur_sim:1;
    uint8_t pv_sim:1;
    uint8_t tmp_sim:1;
    int sim_data;
}STR_Simul;
    
typedef struct
{

	uint32_t MBMSMode;
	uint32_t AccessStatus;
	STR_Security Security;
    STR_Simul Simul;
	//UNI_LineTestCANBReadModel Read;
	//UNI_LineTestCANBInspectionModel Inspection;

}STR_CanGw;


extern STR_CanGw CanGW;
extern uint8_t diag_relay_mode; // RELAY 단위 제어 테스트 모드 
extern uint8_t diag_fuse_mode;  // FUSE 단위 제어 테스트 모드
PUBLIC void* can_com_AllocMessage(uint8_t who);
PUBLIC void can_com_initSession(void);
PUBLIC int32_t can_com_logout(void);
PUBLIC void can_com_securityacess_handler(uint8_t *msg);
PUBLIC void can_com_modechange_handler(uint8_t LocalID, uint8_t SubCMD);
PUBLIC void can_com_initInterface(void);
PUBLIC void responseNegtiveMessage(uint8_t LocalID, uint8_t Reason);
PUBLIC void responsePositiveMessage(uint8_t  LocalID , uint8_t SubCMD, uint8_t *data);

PUBLIC void can_Diag_ReadDTCInfomation(void);
PUBLIC void can_Diag_ClearDTCInfomation(uint8_t);    
PUBLIC void can_Diag_SuccessFactoryReset(void);
PUBLIC void can_Diag_ReadCellNum(void);
PUBLIC void can_Diag_ReadBlockNum(void);
PUBLIC void can_Diag_ReadMDTmpNum(void);
PUBLIC void can_Diag_ReadBDTmpNum(void);
PUBLIC void can_Diag_SuccessVerInfoEEPROM(void *buf);
PUBLIC void can_Diag_RackReadTrayCellVol(void);
PUBLIC void can_Diag_SuccessWriteBytesEEPROM(void *buf);
PUBLIC void can_Diag_SuccessReadBytesEEPROM(void *buf);
PUBLIC void can_Diag_SuccessModelInfoEEPROM(void *buf);
PUBLIC void can_Diag_RackReadTrayTmp(void);
PUBLIC int32_t can_rbms_logout(void);

#endif //__CAN_DIAGCOM_H__

#ifndef __SUBMICPROCESSDATA_H__
#define __SUBMICPROCESSDATA_H__
/**
    ******************************************************************************
    *     @file         SubmicProcessData.h
    *     @version   
    *     @author    
    *     @date      2017/10/13 
    *     @brief     
    ******************************************************************************
    */
#include "EEprom_ProcessData.h"
    
#define INPUTREG_SIZE   18
#define WRITEREG_SIZE   7
#define RDONLY_SIZE     0
#define HOLDREG_SIZE    WRITEREG_SIZE+RDONLY_SIZE
    
#define HOLD_ADDR_CMD       0
#define HOLD_ADDR_CFET     1
#define HOLD_ADDR_DFET     2
#define HOLD_ADDR_CBAL     3
#define HOLD_ADDR_FUSE     4
#define HOLD_ADDR_RSV1     5
#define HOLD_ADDR_RSV2     6
    
typedef enum{
    ST_RESET= 0,
    ST_RSV1= 1,
    ST_RSV2 =2,
    ST_RUN=3,
} MODE;

typedef enum{
    C_RUN = 2,
    C_RESET = 3,
	C_PDWN =0xFF
} CMD;

typedef enum{
    INIT = 0,
    RUN,
	ERR =0xFF
} SUBMIC_STATE;


typedef union {
    __packed struct{
        uint16_t mode;    //reset:(0), ready(1), config(2), run(3)
        uint16_t cell[8];
        uint16_t pon;
        uint16_t ad_batvol;        
        uint16_t ad_packvol;
        uint16_t temp1;
        uint16_t temp2;
        uint16_t temp3;
        uint32_t ad_cur;
        uint16_t version;
        uint16_t CfetState; // 현재 Cfet 상태(On : Off)
        uint16_t DfetState; // 현재 Dfet 상태(On : Off)
    } model;
    int16_t buffer[INPUTREG_SIZE]; 
} InputRegisters;


typedef union {
    __packed struct{
        uint16_t cmd;     //n/a(0), config(1), start(2), reset(3), powrdown(0xff)
        uint16_t Cfet;    // Charge FET
        uint16_t Dfet;    // Discharge FET
        uint16_t cbal_on;  // Cell balancing on/off
        uint16_t fuse_off;
        uint16_t testMode;  // testMode가 1이면 디버깅을 진행중임임
        uint16_t rsrv4;
    } model;       
    int16_t buffer[HOLDREG_SIZE]; 
} HoldRegisters;

typedef struct{
    int16_t ad_cvol[10];
    int16_t ad_temp[2];
    int16_t ad_batvol;
    int16_t ad_pcsvol;
    int16_t ad_gfd_p;
    int16_t ad_gfd_n;
    uint32_t ad_cur;
} AD_Data;




PUBLIC void Submic_ClrRegisters(void);
PUBLIC void Submic_initDevice(uint8_t ch);
PUBLIC uint8_t Submic_GetData(uint8_t ch);
PUBLIC void Submic_Polling(void);
PUBLIC void Submic_Reset(uint8_t ch);
PUBLIC void Submic_Send_PowerDownCmd(void);
PUBLIC void Submic_Send_FuseOffCmd(void);
PUBLIC void Submic_Send_CellBalCmd(void);


#endif //__SUBMICPROCESSDATA_H__

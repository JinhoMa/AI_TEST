#ifndef __MAIN_H__
#define __MAIN_H__

/**
    ******************************************************************************
    *     @file         main.h
    *     @version   
    *     @author    
    *     @date      2023/08/03 
    *     @brief     
    ******************************************************************************
    */
// BMS header 파일이 제일 상단에 위치
// BSP 별 header file include 
#include "BMS.h"
#include <stdint.h>
#include "define.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"


#define MAJOR_VER       1
#define MINOR1_VER      2
#define MINOR2_VER      0

/********************************************************************************
   *    BMS COM Layer define through Modbus (serial)
********************************************************************************/

typedef enum{
    M_RESET= 0,
    M_READY= 1,
    M_CONFIG =2,
    M_RUN=3,
	M_CAL_BATVOL=0xF1,
	M_CAL_PCSVOL=0xF2,
	M_CAL_CUR=0xF3,
	M_CAL_GFD_P=0xF4,
	M_CAL_GFD_N=0xF5,
	M_PDOWN =0xFF
} MODE;
typedef enum{
    C_CONFIG = 1,
    C_RUN = 2,
    C_RESET = 3,
	C_BATVOL = 0xF1,
	C_PCSVOL = 0xF2,
	C_CUR = 0xF3,
	C_GFD_P = 0xF4,
	C_GFD_N = 0xF5
} CMD;
typedef enum{
    INIT = 0,
    CALIB_WAIT,
    CONFIG_WAIT,
    CONFIG_WRITE,
    RUN_WAIT,
    RUN,
	CAL_BATVOL,
	CAL_PCSVOL,
	CAL_CUR,
	CAL_GFD_P,
	CAL_GFD_N,
	FUSE_OFF,
	PWR_OFF,
	ERR =0xFF
} STATE;


#define INPUTREG_SIZE   17
#define WRITEREG_SIZE   7
#define RDONLY_SIZE     0
#define HOLDREG_SIZE    WRITEREG_SIZE+RDONLY_SIZE


typedef union {
    struct{
        uint16_t mode;    //reset:(0), ready(1), config(2), run(3), powerdown(0xff)
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
    struct{
        uint16_t cmd;     //n/a(0), config(1), start(2), reset(3)
        uint16_t Cfet;    // Charge FET
        uint16_t Dfet;    // Discharge FET
        uint16_t cbal_on;
        uint16_t fuse_off;
        uint16_t testMode;  // testMode가 1이면 디버깅을 진행중임임
        uint16_t rsrv4;
    } model;       
    int16_t buffer[HOLDREG_SIZE]; 
} HoldRegisters;

typedef struct{
    int16_t ad_cvol[10];
    int16_t ad_temp[3];
    int16_t ad_packvol;
    int16_t ad_batvol;
    uint32_t ad_cur;
} AD_Data;

/********************************************************************************
   *     Hal Layer Configuration
********************************************************************************/
// HAL Serial Device Configuration
#define COMn    1   // UART USE 개수
#define COM1    0   // HAL COM1 Port index
#define COM2    1   //HAL COM2 Port Index

/********************************************************************************
   *     HVP Include
********************************************************************************/
extern void AFE_HVP_INIT(void);
extern void AFE_HVP0_Hi(void);
extern void AFE_HVP0_Lo(void);
extern void AFE_HVP1_Hi(void);
extern void AFE_HVP1_Lo(void);


// HAL GPIO Device Configuration

#define MAINMCU_RESET_ON          P0_bit.no5 = 1                // MCU Reset Out Pin  - ON
#define MAINMCU_RESET_OFF         P0_bit.no5 = 0                // MCU Reset Out Pin - OFF
#define KEEPALIVE_SIG_TOGLE       P0_bit.no0 = ( P0_bit.no0)? 0 : 1     // RAJ KEEP ALIVE OUT PIN 
#define READ_MAINMCU_WDPIN        P0_bit.no1                    // MCU KEEPALIVE IN PIN
#define IO_MAINMCU_POWEROn()      P1_bit.no0 = 1
#define IO_RAJ_2ND_POWEROn()      P1_bit.no1 = 1
#define IO_ExtPowerOn()           P1_bit.no2 = 1  
#define IO_ExtPowerOff()          P1_bit.no2 = 0
#define IO_LEDOn()                    P0_bit.no7 = 1
#define IO_LEDOff()                    P0_bit.no7 = 0



// RAJ80 ?? 諛痍?
#define CHASSIS_GND_ENABLE          P5_bit.no0 = 1
#define CHASSIS_GND_DISABLE         P5_bit.no0 = 0
#define PCS_SENS_ENABLE            P5_bit.no1= 1
#define PCS_SENS_DISABLE           P5_bit.no1 =0
//#define BAT_SENS_ENABLE            P1_bit.no0 = 1
//#define BAT_SENS_DISABLE           P1_bit.no2 = 0


#define IO_CFETOn()               afet |= FET_C ; Set_FET();
#define IO_CFETOff()              afet &= ~FET_C; Set_FET();
#define IO_DFETOn()               afet |= FET_D ; Set_FET();
#define IO_DFETOff()              afet &= ~FET_D; Set_FET();
#define IO_CELLBALOn()            P0_bit.no6 = 0
#define IO_CELLBALOff()            P0_bit.no6 = 1
#define IO_BLOW_FUSE()          AFE_HVP0_Hi(); AFE_HVP1_Hi();
#define IO_CONNECT_FUSE()           AFE_HVP0_Lo(); AFE_HVP1_Lo();
#define IO_FETAllOn()             afet |=(FET_C|FET_D); Set_FET();
#define IO_FETAllOff()            afet= FET_OFF; Set_FET();

#define RAJ_FETOFF()            IO_FETAllOff(); hr.model.Cfet = SW_OFF; hr.model.Dfet = SW_OFF;

extern InputRegisters ir;
extern HoldRegisters hr;
extern AD_Data guage;
extern int32_t mainmcu_wcdog_event;  // low에서 high 로 감지 될때 마다
extern uint8_t mainmcu_wd_start;  // Low 에서 high로 최초 1번 감지시 
extern uint8_t from_mcuwdpin_prvstate;

int16_t afecon_readBatVol(void);
int16_t afecon_readCurrent(void);
void BMIC_BspInit(void);
void app_main(void);

#undef D_SERIES
#define D_SERIES 8
#define D_TEMPS 2

#endif //__MAIN_H__


#ifndef __RACK_SYS_H__
#define __RACK_SYS_H__

/**
    ******************************************************************************
    *     @file         RACK_SYS.h
    *     @version   
    *     @author    
    *     @date      2017/02/16 
    *     @brief     
    ******************************************************************************
    */
#include "AppQueue.h"

typedef struct
{
    uint32_t eeprom:1 ; // eeprom
    uint32_t net:1 ;    // network     
    uint32_t idsw:1 ;   // ID 스위치 
    uint32_t relay:1 ;  // Relay
    uint32_t cur:1 ;    // Current Sensing
    uint32_t gfd:1 ;    //GFD 측정
    uint32_t vol:1 ;    //Voltage 측정
    uint32_t led:1 ;    //LED 
    uint32_t emo:1 ;   //비상 relay 차단 switch
    uint32_t bmic:1 ;
    
} RACK_INI ;
extern RACK_INI rack_ini;
//////////////////////////////////////////////////////////////////////////////////
//  0. RACK SYSTEM INFORMATION
//////////////////////////////////////////////////////////////////////////////////
 



//jae.kim
//일단 이렇게 구현하고 나중에 DTC 구현되면 합친다.
extern uint16_t RACK_SYSFAULT;      //RACK단위 System Fault 정보 
extern uint16_t SW_Reset_Command;   //Master에서 Alarm Reset 명령 
extern uint16_t pushDiagDataEvent;
void InitRackSys(void);

//////////////////////////////////////////////////////////////////////////////////
// 1. PACK VOLTAGE
//////////////////////////////////////////////////////////////////////////////////
#if defined(KC_RELAY_FAULT_TEST)
 #define PACK_V_AVG_CNT 1
#else
#define    PACK_V_AVG_CNT    5
#endif

typedef struct
{
  //Uint16  RESULT_BUF[PACK_V_AVG_CNT];
  uint16_t  AVG_CHK_CNT;
  uint32_t  AVG_TEMP_BUF;
  float   PACK_V_OFFSET_ERR;
  uint16_t  Pack_V_Sense_First;
  float   PACK_V;
  float  PACK_V_GFD;
}_PACK_V;

extern _PACK_V PACK_V;
extern _PACK_V PCS_V;
void PACK_VOLTAGE(void);
void PCS_VOLTAGE(void);

//////////////////////////////////////////////////////////////////////////////////
// 2. RELAY CONTROL
//////////////////////////////////////////////////////////////////////////////////

#define    PRE_CHG_RELAY_ON_TIME      450        // 4.5SEC: 10mSec Base time
#define    RELAY_OVERLAP_TIME        50        // 0.5SEC: 10mSec base time
#define    MAIN_PRA_P_ON_TIME        450        // 4.5SEC: 10mSec base time
#define    RLY_P_STATUS          0x01
#define    RLY_N_STATUS          0x02
#define    RLY_ST_CNT_NUM                  // 100mSec: 10mSec Base time
#define    RLY_ST_CHK_TIME          10000      // 1Sec : 100uSec Base time

#define    BANK_TRAY_CONTACT_REQUSET_EN  0x80      // Relay Contactor Request Enable
#define    BANK_TRAY_CONTACT_ALL_REQUEST  0x40      // All Tray Relay Contactor Request
#define    BANK_TRAY_CELL_BALAN_EN      0x80      // Cell Balancing Enable
#define    BANK_TRAY_CELL_AUTOBALAN_ON  0x40
#define    BANK_TRAY_CELL_BALAN_DISABLE  0x00      // Cell Balancing Disable


typedef enum
{
    RLY_2CH = 1, // P, N rely
    RLY_3CH = 2, // P, N, Pre
    FET_2CH = 3, // C FET, D FET
    FET_3CH = 4, // C FET, D FET, PRE FET
    FET_2CH_N = 5, // C FET, D FET,  GND FET
    FET_3CH_N =6, // C FET, D FET, PRE FET, GND FET 
}_RLY_TYPE;

typedef struct
{
   _RLY_TYPE   type;
   uint32_t RELAY_MODE;
   uint8_t st;
}_RLY_CTL;

typedef enum
{
    RELAY_RESET = 0,
    RELAY_NORMAL = 1,
    RELAY_FORCE_OFF = 2,    
    CHG_ONLY_MODE =3,
    DIS_ONLY_MODE = 4,
    ALL_MODE = 5,
} _RLY_MODE;
    
typedef enum {
  STRING_NONE = 0,
  STRING_ENABLE = 1,
  STRING_DISABLE = 2
}StrEnaCommand;

#define    RELAY_OFF                  2  

extern _RLY_CTL RLY_CTL;
PUBLIC void processRelayOffFromDTC(void);
void RELAY_CONTROL(uint32_t MODE);
PUBLIC int32_t getRelayOnOffStatus(void);
PUBLIC void Get_PackVoltage(void);


#define MAIN_RELAY_N_CLOSE   hRlyCtrl(3, 1)
#define MAIN_RELAY_N_OPEN    hRlyCtrl(3, 0)
#define MAIN_RELAY_P_CLOSE   hRlyCtrl(1, 1)
#define MAIN_RELAY_P_OPEN    hRlyCtrl(1, 0)
#define OPEN_ALL_RELAY       hRlyOpen()

#define HEATFET_ON()          hFetEnable(1)
#define HEATFET_OFF()         hFetDisable(1)
#define CFET_ON()             hFetEnable(2)
#define CFET_OFF()            hFetDisable(2)
#define DFET_ON()             hFetEnable(3)
#define DFET_OFF()            hFetDisable(3)

////////////////////////////////////////////////////
//3. EXT TEMPERATURE
///////////////////////////////////////////////////
#define      TEMP_AVG_CNT    5
typedef struct
{
  uint16_t  AVG_CHK_CNT;
  uint32_t  AVG_TEMP_BUF;
  int16_t   REAL_TEMP_VALUE;
}_EXT_TEMP;
extern _EXT_TEMP FET_TEMP;
extern _EXT_TEMP BD_TEMP;
void GET_EXT_TEMPERATURE(void);

////////////////////////////////////////////////////
//4. GFD CHECK
///////////////////////////////////////////////////
#define    GFD_AVG_CNT        5

typedef struct
{
  uint16_t  AVG_CHK_CNT;
  uint32_t  AVG_TEMP_BUF;
  uint16_t  *pCUR_RESULT_BUF;
  uint32_t  REAL_GFD_VALUE;
  uint16_t  F_GFD_FAIL;
  int32_t  RAW_VAL;
}_GFD;

extern _GFD GFD_P;
extern _GFD GFD_N;

void GET_GFD_VALUE(uint16_t type);
void GfdControl(void);

//////////////////////////////////////////////////////////////////////////////////
// 5. LED
//////////////////////////////////////////////////////////////////////////////////
//LED Definition.
#define    PWR_LED_ON_CNT_CHK   100    // 10ms base time...
#define PWR_LED_ON          hLedOn(LED1)
#define PWR_LED_OFF         hLedOff(LED1)
#define PWR_LED_TOGGLE       hLedBlink(LED1)
#define LNK_LED_ON          hLedOn(LED2)
#define LNK_LED_OFF         hLedOff(LED2)
#define LNK_LED_TOGGLE       hLedBlink(LED2)
#define FLT_LED_ON          hLedOn(LED3)
#define FLT_LED_OFF         hLedOff(LED3)
#define FLT_LED_TOGGLE       hLedBlink(LED3)

typedef struct
{
  uint16_t PWR_LED_ON_CNT;
  uint16_t LINK_LED_ON_CNT;
  uint16_t FAULT_LED_ON_CNT;

}_LED_CONTROL;
extern _LED_CONTROL LED_CONTROL;
void LED_CTL(void);

//////////////////////////////////////////////////////////////////////////////////
// 6. ANALOG INPUT 
//////////////////////////////////////////////////////////////////////////////////

#define    EXT_TEMP_ADC_CH10       hAdcReadData(hAdcGetHandle("ADC1"), TEMP_ADC_CH10)
#define    EXT_TEMP_ADC_CH11       hAdcReadData(hAdcGetHandle("ADC1"), TEMP_ADC_CH11)

//////////////////////////////////////////////////////////////////////////////////
// 7. CURRENT CTL
//////////////////////////////////////////////////////////////////////////////////

#define CUR_AVG_CNT 3

typedef struct {
//  float curDiff; 
  float avgCurrentA; // 단위 A 소수점 있음.
//  float avgRefCurrent;
//  uint8_t ocd_on;
}_CUR_CTL;
extern _CUR_CTL CUR_CTL;

void GetExtCurrSensorSignal(void);

//////////////////////////////////////////////////////////////////////////////////
// 8. DTC VALUE
//////////////////////////////////////////////////////////////////////////////////

void Diag_Update(void);
#define DIAG_PUSH_SIZE  50
//////////////////////////////////////////////////////////////////////////////////
// 9. SOC
//////////////////////////////////////////////////////////////////////////////////
typedef struct
{
  // Capacity
  uint32_t Total_Capacity;
  uint32_t Cur_Capacity;
  
  // Input Signal
  int32_t  BATCURR;
  int32_t  BATCURR_CAN;

  // Output Signal
  uint16_t  SOC;    //1000%
  uint16_t  SOH;    //1000%
  uint16_t  SOC_VALIDITY;
  uint16_t  OCV;
  uint16_t  OCV_VALIDITY;
  uint16_t  SOC_I;
  uint16_t  SOC_V;
  uint16_t  RESET_TIME;

  uint16_t  INIT_SOC_FLAG;
  uint64_t  SOC_TIME_TICK;
  uint16_t  UPDATE_EN_FLAG;

  uint16_t  VTMS_CELL_V_AVG[TBMS_BUF_SIZE];
  uint16_t  VTMS_CELL_V_MIN[TBMS_BUF_SIZE];
  uint16_t  VTMS_CELL_V_MAX[TBMS_BUF_SIZE];
}_SOC_CTL;

//#define    UPDATE_SOC_CNT    5    // 0.2SEC :100mSec base time
#define    UPDATE_SOC_CYCLE_TIME    200    // 200ms

extern _SOC_CTL SOC_CTL;
// SKI Input Signal
extern int16_t  ModuleTemp;
extern int16_t  ModuleTemp_min;
extern int16_t  ModuleTemp_max;
extern int16_t  AmbientTemp;
extern uint32_t Pack10mVolt;
extern uint32_t PcsVolt;
extern uint32_t  CellVolt;
extern uint32_t  CellVolt_min;
extern uint32_t  CellVolt_max;
extern uint32_t  CellVolt_dev;
extern int32_t  BatCurr;
extern int32_t  BatCurr_CAN;
extern uint16_t  TempSenseValid;
extern uint16_t  VoltSenseValid;
extern uint16_t  CurrSenseValid;
extern uint32_t BatSum10mVolt;
extern uint32_t BatReal10mVolt;

// SKI Output Signal
extern uint16_t  SOC;
extern uint16_t  SOC_validity;
extern uint16_t  OCV;
extern uint16_t  OCV_validity;
extern uint16_t  SOCi;
extern uint16_t  SOCv;
extern uint16_t  RestTime;

extern uint16_t TEMP_DEV;
PUBLIC void CAL_SOC(void);
PUBLIC void CAL_ReCalSOC(void);
void SK_APP_SOC_Init (void);
void SK_APP_SOC_Update(void);
void TEMP_CAL_INPUT_SOC(void);
void CELL_VOL_INPUT_SOC(void);

//////////////////////////////////////////////////////////////////////////////////
// 10. RACK FAULT DEFINE
//////////////////////////////////////////////////////////////////////////////////
#define RACK_ERR_COMM_TRAY          0x1

//////////////////////////////////////////////////////////////////////////////////
// 11. GateWay COMMUNICATION
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// 12. RTC Clock
//////////////////////////////////////////////////////////////////////////////////
struct RTC_Time{
    uint8_t year;
    uint8_t mon;
    uint8_t day;
    uint8_t hr;
    uint8_t min;
    uint8_t sec;
} __attribute__((packed));

extern struct RTC_Time RtcTime;
extern struct RTC_Time RtcSTime;

//////////////////////////////////////////////////////////////////////////////////
// 13. Tray Interlock
//////////////////////////////////////////////////////////////////////////////////
extern uint8_t trayUnlock;

//////////////////////////////////////////////////////////////////////////////////
// 14. RaskSt
//////////////////////////////////////////////////////////////////////////////////
#define SOC_TBL_SIZE     21
#define REC_D_SEC_N      16
#define REC_D_SIZE      16

typedef unsigned short  RACK_ST ; //Rack Status

typedef union 
{
     struct {
        uint32_t time;
        int16_t max_temp;
        int16_t min_temp;
        uint16_t max_cv;
        uint16_t min_cv;
        RACK_ST rs;    //Pack Status
        uint16_t rsv1;
        uint16_t pv;
        uint16_t rsv3;
        int32_t rc;
        uint32_t evt1;
        uint32_t evt2;        
    } __attribute__((packed))  str;
    uint16_t buffer[REC_D_SIZE];
} __attribute__((packed)) UNI_REC_D;
PUBLIC void RackSt_SaveCurrentStAtOnce( uint32_t u);
PUBLIC void RackSt_UpdateStAndPeriodSave( uint32_t u);

//////////////////////////////////////////////////////////////////////////////////
// 15. Cellbalancing
//////////////////////////////////////////////////////////////////////////////////
extern uint8_t BalancingCMD ;  //Balancing Command [ start, stop, 수행 후 에 clear 된다.
extern uint8_t AutoBalancingMode ;  // 셀 편차가 20mV 이상 생기면 자동으로 Balancing Command가 Start 되도록 
extern uint8_t on_balancing;   //balancing 중이면 1, balancing이 끝나는 경우 0이 된다. 
extern uint32_t ocv_config_time;    // OCV CHECK 설정 시간 
extern uint8_t on_bgbalancing;  // background balancing 진행중. 
extern uint8_t bmic_balsw_on;

//////////////////////////////////////////////////////////////////////////////////
// 16. SOH
//////////////////////////////////////////////////////////////////////////////////
PUBLIC void CAL_SOH(void);

//////////////////////////////////////////////////////////////////////////////////
// 17. FAN
//////////////////////////////////////////////////////////////////////////////////
extern     int32_t fan_on;
//////////////////////////////////////////////////////////////////////////////////
// 18. TRAY
//////////////////////////////////////////////////////////////////////////////////

PUBLIC void RackSys_StringEvent(uint32_t evt, uint8_t on);
PUBLIC void RackSys_StringExtEvent(uint32_t evt, uint8_t on);
PUBLIC void RackSys_UpdateModuleData(void);
PUBLIC void RackSys_UpdateStringModel(void);
PUBLIC uint16_t RackSys_GetCurDiagCode(void);
PUBLIC uint8_t RackSys_PopDiagCode(void *buf);
PUBLIC void RackSys_PushDiagCode(uint16_t code);
PUBLIC void RackSys_ClrDiagCode(void);
PUBLIC uint32_t RackSys_Status(void);
PUBLIC void RasckSys_SetFailReason(uint16_t reasoncode);
#endif //__RACK_SYS_H__

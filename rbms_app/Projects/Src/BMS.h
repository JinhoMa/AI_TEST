#ifndef __BMS_H__
#define __BMS_H__

/**
    ******************************************************************************
    *     @file         BMS.h
    *     @version   
    *     @author    
    *     @date      2017/02/16 
    *     @brief     
    ******************************************************************************
    */
#include "main.h"

#define _RBMS  
#define _TBMS
#define _MBMS

#define D_DV      1       // Designed Verification 
#define D_PV      0       // Product Validation ( 시험성적서 직전 )

/**
  *     @brief 아래 define중에서 어떤 버전으로 빌드할지 결정한다.
  *             반드시 1개만 선택되어야 한다.
**/
#if defined(_TBMS)
#define P100_8S         1
#endif

#define CELLBAL_39_OHM	0
#define CELLBAL_10_OHM	1

#ifdef _TBMS
#include "TRAY_SYS.h"
#if( P100_8S == 1)
#define   TRAY_MODEL_NAME   "1P8S"
#endif
#else
#define   TRAY_MODEL_NAME   "DEFAULT"
#endif


//////////////////////////////////////////////////////////////////////////////////
// Interface between RACKBMS and TRAYBMS
//////////////////////////////////////////////////////////////////////////////////
#define COMM_ERR_TIMEOUT        2000    //2sec


//#ifndef TRAY_CELL_NUMS
// #define TRAY_CELL_NUMS          1          //Tray내에 Cell 채널 개수
//#endif

//#ifndef MODULE_TEMP_SENSORNUM
// #define MODULE_TEMP_SENSORNUM   2           //Tray의 온도 센서 개수
//#endif

#ifndef TBMS_BUF_SIZE
#define    TBMS_BUF_SIZE      1   // BUFFER를 위한 DEFINE , 실제 사이즈와 틀림
#endif


#ifdef _RBMS
#include "RACK_SYS.h"
#define    BOARD_ID        1   // default 가  1
#define   RACK_MODEL_NAME    "P100_8SBL_230A"
#else
#define   RACK_MODEL_NAME    "DEFAULT"
#endif


#define   RACK_MODEL_VER     TRAY_MODEL_NAME     //"1P8S"
#define   RACK_SER_NUM       "00001"
#define   TRAY_MODEL_VER    "23-01"
//#define   TRAY_NUMS               13      //EEPROM 으로 변경

#define    SW_VER_MAJOR    0x01    // VER- O.X.X
#define    SW_VER_MINOR    0x00    // VER- X.O.X
#define    SW_VER_MINOR2   0x00    // VER- X.X.O

#define   NETIP1    192
#define   NETIP2    168
#define   NETIP3    100
#define   NETIP4    100
#define   GWIP4     254

typedef struct
{
	unsigned char bit0			:1;	
	unsigned char bit1			:1;
	unsigned char bit2			:1;
	unsigned char bit3			:1;
	unsigned char bit4			:1;
	unsigned char bit5			:1;
	unsigned char bit6			:1;
	unsigned char bit7			:1;
}_STC_BIT;

#define DEF_BIT7(pdata)	((volatile _STC_BIT*)(pdata))->bit7
#define DEF_BIT6(pdata)	((volatile _STC_BIT*)(pdata))->bit6
#define DEF_BIT5(pdata)	((volatile _STC_BIT*)(pdata))->bit5
#define DEF_BIT4(pdata)	((volatile _STC_BIT*)(pdata))->bit4
#define DEF_BIT3(pdata)	((volatile _STC_BIT*)(pdata))->bit3
#define DEF_BIT2(pdata)	((volatile _STC_BIT*)(pdata))->bit2
#define DEF_BIT1(pdata)	((volatile _STC_BIT*)(pdata))->bit1
#define DEF_BIT0(pdata)	((volatile _STC_BIT*)(pdata))->bit0

typedef struct
{
	unsigned char bit0			:1;	
	unsigned char bit1			:1;
	unsigned char bit2			:1;
	unsigned char bit3			:1;	
	unsigned char bit4			:1;
	unsigned char bit5			:1;
	unsigned char bit6			:1;
	unsigned char bit7			:1;
	unsigned char bit8			:1;
	unsigned char bit9			:1;
	unsigned char bit10			:1;
	unsigned char bit11			:1;
	unsigned char bit12			:1;
	unsigned char bit13			:1;
	unsigned char bit14			:1;
	unsigned char bit15			:1;
}_STS_BIT;

#define DEFS_BIT15(pdata)	((volatile _STS_BIT*)(pdata))->bit15
#define DEFS_BIT14(pdata)	((volatile _STS_BIT*)(pdata))->bit14
#define DEFS_BIT13(pdata)	((volatile _STS_BIT*)(pdata))->bit13
#define DEFS_BIT12(pdata)	((volatile _STS_BIT*)(pdata))->bit12
#define DEFS_BIT11(pdata)	((volatile _STS_BIT*)(pdata))->bit11
#define DEFS_BIT10(pdata)	((volatile _STS_BIT*)(pdata))->bit10
#define DEFS_BIT9(pdata)	((volatile _STS_BIT*)(pdata))->bit9
#define DEFS_BIT8(pdata)	((volatile _STS_BIT*)(pdata))->bit8
#define DEFS_BIT7(pdata)	((volatile _STS_BIT*)(pdata))->bit7
#define DEFS_BIT6(pdata)	((volatile _STS_BIT*)(pdata))->bit6
#define DEFS_BIT5(pdata)	((volatile _STS_BIT*)(pdata))->bit5
#define DEFS_BIT4(pdata)	((volatile _STS_BIT*)(pdata))->bit4
#define DEFS_BIT3(pdata)	((volatile _STS_BIT*)(pdata))->bit3
#define DEFS_BIT2(pdata)	((volatile _STS_BIT*)(pdata))->bit2
#define DEFS_BIT1(pdata)	((volatile _STS_BIT*)(pdata))->bit1
#define DEFS_BIT0(pdata)	((volatile _STS_BIT*)(pdata))->bit0

typedef struct
{
	uint8_t LSB:8;
	uint8_t MSB:8;     // 7:0
}LSBMSB;

typedef union
{
	uint16_t all;
	LSBMSB byte;
}UNI_Uint16Data;

typedef union
{
	int16_t all;
	LSBMSB byte;
}UNI_int16Data;

typedef struct
{
    uint8_t num;
    uint16_t Vol;
}CellVol;

typedef struct
{
    //MH mh;
    uint8_t id;
    CellVol maxCell;
    CellVol minCell;
    int32_t sum_sw_vol;
    int32_t sum_hw_vol;
    uint16_t cellvol[CELL_BUF_SIZE];
    int16_t tmp[TMP_BUF_SIZE];
    int16_t brdtmp[BDTMP_BUF_SIZE];
    uint8_t cellbal[CELL_BUF_SIZE];
    uint16_t fault;
    uint8_t unlock;        //E850 부터 추가 
    uint8_t on_balancing;
}RBMS_COM_TRAY;

typedef struct
{
    uint16_t value;
    uint8_t num;
    uint8_t tray;    
} MOST;
#define MAX MOST
#define MIN MOST
typedef struct{
    uint8_t BmicAuxUse[4];
    uint8_t pad2[4];
} __attribute__((packed)) BMIC_2AUX;

/***************************************************
  | aux1 | aux2 | aux3 | aux4 |
  | aux5 | aux6 | aux7 | aux8 |
  | aux9 | aux10 | N/A | N/A |
***************************************************/ 
typedef struct{
    uint8_t BmicAuxUse1[4];
    uint8_t BmicAuxUse2[4];
    uint8_t BmicAuxUse3[4];
}__attribute__((packed)) BMIC_10AUX;

typedef union{
    uint8_t all[12];
    BMIC_2AUX    aux2;
    BMIC_10AUX   aux10;
}__attribute__((packed)) BMIC_AUX;

typedef struct
{
    uint8_t MN[16]; //Model Name
    uint8_t MV[8];  //Model Version
    uint8_t SN[8];  //Serial Number
    uint8_t ID ;   //RBMS ID
    uint8_t TrMN[16];       //TrayBMS Model Name
    uint8_t TrMV[8];        //TrayBMS Model Version
    uint8_t TrCnt;  //TrayBMS Count
    uint8_t TrCellCnt;   //TrayBMS Cell Count;
    uint8_t TrBmicCnt;  //TrayBMS BMIC Count    - Tray 당 BMIC 개수 
    uint8_t TrTmpCnt;   //TrayBMS Module Temperature Count - Tray당 모듈 온도 센서 개수   
    uint8_t TrBdTmpCnt;     //TrayBMS Board Tmeperature Count - TBMS 보드 온도 센서 개수
    uint8_t BmicCellEnaCnt;    //BMIC 1개당 Enable되는 Cell Count;
    uint16_t BmicCellMask[4];           //BMIC 1개당 Mask 되는 것, 
    uint16_t pad1[4];             //reserved space - BMIC  Device 가 늘어날 고려  
    //uint8_t  BmicAuxUse[4];         //BMIC 1개당 Aux  Use 정보
    //uint8_t pad2[4];
    BMIC_AUX u;
}__attribute__((packed)) EEP_COM_BASE;

typedef struct
{
    uint8_t SWMAJ_MIN;      //SW Major Version << 4 & SW Minor Version
    uint8_t SWMIN2;      //SW Minor2 Version
    uint8_t SWBLDYR;    //SW Build Year
    uint8_t SWBLDMON;   //SW Build Month
    uint8_t SWBLDDAY;   //SW Build Day
}__attribute__((packed)) EEP_COM_BLDMGR;

typedef struct
{
    uint8_t NIP1;       //Network IP 1st
    uint8_t NIP2;       //Network IP 2nd
    uint8_t NIP3;       //Network IP 3th
    uint8_t NIP4;       //Network IP 4th
    uint8_t GIP4;      //Gateway IP 4th
}__attribute__((packed)) EEP_COM_NETMGR;

typedef struct
{
    int16_t deprecated1;      //Over Cell Voltage Protect Value 
    int16_t OCVF;       //Over Cell Voltage Fault value
    int16_t OCVW;       //Over Cell Voltage Warn Value
    int16_t deprecated2;      //Under Cell Voltage Protect Value
    int16_t UCVF;       //Under Cell Voltage Fault Value
    int16_t UCVW;       //Under Cell Voltage Warn Value
    int16_t CVDF;       //Cell Voltage Deviation
    int16_t CVDW;       //Over Temperature Protect Value
    int16_t OTF;       //Over Temperature Fault Value
    int16_t OTW;       //Over Temperature Warn Value
    int16_t UTF;       //Under Temperature Fault Value
    int16_t UTW;       //Under Temperature Warn Value
    int16_t OSOCF;     //Over SOC Fault Value
    int16_t OSOCW;     //Over SOC Warn Value
    int16_t USOCF;     //Under SOC Fault Value
    int16_t USOCW;     //Under SOC Warn Value
    int16_t deprecated3;      //Over Battery Voltage Protect Value
    int16_t OBVF;      //Over Battery Voltage Fault Value
    int16_t OBVW;      //Over Battery Voltage Warning Value
    int16_t deprecated4;      //Under Battery Voltage Protect Value
    int16_t UBVF;      //Under Battery Voltage Fault Value
    int16_t UBVW;      //Under Battery Voltage Warning Value
    int16_t deprecated5;      //Over Charge Current Protect Value
    int16_t OCCF;      //Over Charge Current Fault Value
    int16_t OCCW;      //Over Charge Current Warning Value
    int16_t deprecated6;      //Over DisCharge Current Protect Value
    int16_t ODCF;      //Over DisCharge Current Fault Value
    int16_t ODCW;      //Over DisCharge Current Warning Value
    int16_t deprecated7;       //Over Current Detection Value
    int16_t deprecated8;        //Module Temp Deviation Protect Value
    int16_t TDF;        //Module Temp Deviation Fault Value
    int16_t TDW;        //Module Temp Deviation Fault Value
    int16_t deprecated9;      //Current Correction Value
    
}__attribute__((packed)) EEP_COM_BMICONF;

typedef struct
{
    int16_t SOC_5P;      //SOC 5%
    int16_t DCHGSCV;     //Discharge Stop Cell Voltage
    int16_t CWCV;        //Charging Wait Cell Voltage
    int16_t PDWNCV;       //Power Down Cell Voltage
    int16_t FCCV;       //Full Charge Cell Voltage
    int16_t FCTV;       //Full Charge Tapper Voltage
    int16_t FCWJCV;      //Full Charge Wakeup Judgement Cell Voltage
    int16_t FCJC;       //Full Charge Judgment Current
    int16_t DCHGJC;     //Dicharge Judement Current
    int16_t CHGJC;       //Charge Jdugement Current
    int16_t reserve1; 
    int16_t reserve2;
}__attribute__((packed)) EEP_MG_CONF;

typedef struct
{
    int16_t SOCTBL[SOC_TBL_SIZE];
    uint16_t cell_100mA;  // 셀당 용량
    uint32_t total_rc;    // total 용량 (100mA*sec) = 셀용량*3600 
    uint32_t total_eng;    // total energy
}__attribute__((packed)) EEP_SOC_CONF;


// 32 byte * 16 = 512 byte

typedef struct
{
    uint8_t   last_idx;
    uint8_t   flag;     //SOC가 계산되어 기록 된 상태 ON
    uint8_t   SOH;  // % 단위
    uint8_t   SOC;
    UNI_REC_D   sector[REC_D_SEC_N];
}__attribute__((packed)) EEP_REC_DATA;

typedef struct
{
    uint16_t dtc;
    uint16_t sub_code;
}__attribute__((packed)) EEP_COM_EVTLOG; 


typedef struct
{
    uint8_t t[6];
    uint16_t v;

}__attribute__((packed)) EEP_DIAG_DATA;

typedef struct
{
    uint16_t OVCNT[16];
}__attribute__((packed)) EEP_TRAY_OVCNT;

typedef struct
{
    uint16_t UVCNT[16];
}__attribute__((packed)) EEP_TRAY_UVCNT;

typedef struct
{
    uint16_t MAXVOL[16];
}__attribute__((packed)) EEP_TRAY_MAXVOL;

typedef struct
{
    uint16_t MINVOL[16];
}__attribute__((packed)) EEP_TRAY_MINVOL;

typedef struct
{
    uint16_t M1OTCNT;
    uint16_t M1UTCNT;
    uint16_t M2OTCNT;
    uint16_t M2UTCNT;
    uint16_t M1BDOTCNT;
    uint16_t M1BDUTCNT;
    uint16_t M2BDOTCNT;
    uint16_t M2BDUTCNT;
    uint16_t M1MAXTMP;
    uint16_t M2MAXTMP;
    uint16_t M1BDMAXTMP;
    uint16_t M2BDMAXTMP;
    uint16_t M1MINTMP;
    uint16_t M2MINTMP;
    uint16_t M1BDMINTMP;
    uint16_t M2BDMINTMP;
}__attribute__((packed)) EEP_TRAY_TEMP;


typedef struct
{
    EEP_TRAY_OVCNT ovcnt;
    EEP_TRAY_UVCNT uvcnt;
    EEP_TRAY_MAXVOL maxvol;
    EEP_TRAY_MINVOL minvol;
    EEP_TRAY_TEMP temp;
}__attribute__((packed)) EEP_TRAY_DATA;

#define CAL_BVC_BUFSIZE     3
#define CAL_PVC_BUFSIZE     3
#define CAL_CUR_BUFSIZE        3
#define CAL_GFDP_BUFSIZE          3
#define CAL_GFDN_BUFSIZE          3
#define CAL_BVC_DSIZE       2
#define CAL_PVC_DSIZE       2
#define CAL_CUR_DSIZE       4
#define CAL_GFDP_DSIZE       2
#define CAL_GFDN_DSIZE      2
#define CAL_CELLREFV_BUFSIZE 2


typedef struct
{
    uint16_t BREFV[CAL_BVC_BUFSIZE];
    uint16_t BVOL[CAL_BVC_BUFSIZE];
    uint16_t PREFV[CAL_PVC_BUFSIZE];
    uint16_t PVOL[CAL_PVC_BUFSIZE];
    uint16_t REFC[CAL_CUR_BUFSIZE];
    uint32_t CCUR[CAL_CUR_BUFSIZE];
    uint32_t DCUR[CAL_CUR_BUFSIZE];
    uint16_t GFDP[CAL_GFDP_BUFSIZE];
    uint16_t GFDN[CAL_GFDN_BUFSIZE];
    uint16_t CV_LVREF;
    uint16_t CV_HVREF;
    uint16_t D_LV[10];
    uint16_t D_HV[10];
}__attribute__((packed)) EEP_COM_CAL;

typedef struct 
{
    uint16_t YER;
    uint16_t MON;
    uint16_t DAY;
#if ( SOH_NEWLIB_REPLACE==1)
    uint16_t SOH;    
#endif
}__attribute__((packed)) EEP_COM_SOH;

#if ( ETH_ENABLE_CDMS == 1)    
typedef struct
{
   uint16_t CCT;
   uint16_t CAID;
}__attribute__((packed)) EEP_COM_CANCNF;
#endif

typedef struct
{
	uint16_t IsRunning:1;
	uint32_t JobIdx;
	uint16_t Size;
	uint16_t memIdx;
    uint8_t access;
    uint8_t chksumerr;
    EEP_COM_BASE base;
    EEP_COM_BLDMGR  bldmgr;
    EEP_COM_NETMGR  netmgr;
    EEP_COM_BMICONF bmiconf;
    EEP_COM_EVTLOG      evlog;
    uint16_t com_chksum;
#if ( ETH_ENABLE_CDMS == 1)    
    EEP_COM_CANCNF cancnf;
#endif
    EEP_COM_CAL     cal;
    uint16_t cal_chksum;
    EEP_COM_SOH    soh;
    EEP_MG_CONF mgconf;
    EEP_SOC_CONF soccnf;
    EEP_REC_DATA recdf;
//    uint8_t         diagcnt;
//    EEP_DIAG_DATA   diagdata[DIAG_PUSH_SIZE];
//    EEP_TRAY_DATA tray[TBMS_BUF_SIZE];
}STR_EEPROM_DataModel;



#include "glovar.h"

#define TRAY_NUMS               EEPROMData.base.TrCnt
#define BMIC_DEVICE_COUNT       EEPROMData.base.TrBmicCnt
#define MODULE_TEMP_SENSORNUM   EEPROMData.base.TrTmpCnt
#define MODULE_BDTEMP_SENSORNUM EEPROMData.base.TrBdTmpCnt
#define TRAY_CELL_NUMS          EEPROMData.base.TrCellCnt
#define CELLMASK(x)              EEPROMData.base.BmicCellMask[x]
///////////////////////////////////////////////////////////////   Cell Enable Check ///////////////////////////////////////////////////////////
/**
*    @brief  
*                 BMIC 와 모듈간에 실제 연결되는 CELL 채널수 
*                예) 4P15S 인 경우 BMIC 2개에 8개, 7개 연결되는 경우 값은 8 이 된다.
*/
#define CELL_ENABLE_COUNT       EEPROMData.base.BmicCellEnaCnt

//////////////////////////////////////////////////////////////////////////////////
// Porting Function Call
//////////////////////////////////////////////////////////////////////////////////
#define DelayUs hTimDelay

#define TRAY_DEFAULT_CELL_NUMS              1
#define TRAY_DEFAULT_TEMP_SENSORNUM         1
#define TRAY_DEFAULT_BDTEMP_SENSORNUM       1 
#define TRAY_DEFAULT_BMIC_COUNT             1
#define BMIC_DEFAULT_CELLENA_COUNT          1

#if( P100_8S == 1)
#undef TRAY_DEFAULT_TEMP_SENSORNUM
#undef TRAY_DEFAULT_BDTEMP_SENSORNUM
#define TRAY_DEFAULT_TEMP_SENSORNUM         3  // AUX 를 통해서 4개 사용
#define TRAY_DEFAULT_BDTEMP_SENSORNUM       1  //BMIC 1 개당 ITEMP를 사용한다. TRAY_DEFAULT_BMIC_COUNT와 동일하게

#undef TRAY_DEFAULT_BMIC_COUNT
#undef TRAY_DEFAULT_CELL_NUMS
#undef TRAY_DEFAULT_NUMS
#undef BMIC_DEFAULT_CELLENA_COUNT
#define TRAY_DEFAULT_BMIC_COUNT             1
#define TRAY_DEFAULT_CELL_NUMS              8
#define TRAY_DEFAULT_NUMS                   1
#define BMIC_DEFAULT_CELLENA_COUNT          10

#endif

#if (P100_8S == 1)
#define DEFAULT_OVER_CELL_V_FAULT_DETECT        3600 // mV  
#define DEFAULT_OVER_CELL_V_WARNING_DETECT      3400 // mV
#define DEFAULT_UNDER_CELL_V_FAULT_DETECT       2800 //mV
#define DEFAULT_UNDER_CELL_V_WARNING_DETECT     3100   // mV
#define DEFAULT_CELL_DEVIATION_V_FAULT_DETECT   600
#define DEFAULT_CELL_DEVIATION_V_WARNING_DETECT 300    // mV
#define DEFAULT_OVER_CELL_TEMP_FAULT_DETECT  	5800//  factor 0.01
#define DEFAULT_OVER_CELL_TEMP_WARNING_DETECT   5300 //  factor 0.01
#define DEFAULT_UNDER_CELL_TEMP_FAULT_DETECT    (500) //  factor 0..01
#define DEFAULT_UNDER_CELL_TEMP_WARNING_DETECT  (1000) //  factor 0..01
#define DEFAULT_OVER_SOC_FAULT_DETECT           10000//b9300 // factor 0.01
#define DEFAULT_OVER_SOC_WARNING_DETECT         10000//9200// factor 0.01
#define DEFAULT_UNDER_SOC_FAULT_DETECT          0   //factor 0.01
#define DEFAULT_UNDER_SOC_WARNING_DETECT        0 //  factor 0..01
#define DEFAULT_OVER_BAT_VOL_FAULT_DETECT       3.6*10*TRAY_DEFAULT_NUMS*TRAY_DEFAULT_CELL_NUMS     // 28.8 V (factor 0.1)
#define DEFAULT_OVER_BAT_VOL_WARNING_DETECT     3.45*10*TRAY_DEFAULT_NUMS*TRAY_DEFAULT_CELL_NUMS    // 27.6 V (factor 0.1)
#define DEFAULT_UNDER_BAT_VOL_FAULT_DETECT      2.6*10*TRAY_DEFAULT_NUMS*TRAY_DEFAULT_CELL_NUMS     // 20.8 V (factor 0.1)
#define DEFAULT_UNDER_BAT_VOL_WARNING_DETECT    2.85*10*TRAY_DEFAULT_NUMS*TRAY_DEFAULT_CELL_NUMS    // 22.8 V (factor 0.1)
#define DEFAULT_OVER_CHARGE_CURRENT_FAULT_DETECT ((int16_t)10000) // 10mA
#define DEFAULT_OVER_CHARGE_CURRENT_WARN_DETECT ((int16_t)9000) 
#define DEFAULT_OVER_DSCHARGE_CURRENT_FAULT_DETECT ((int16_t)-20000) // 10mA
#define DEFAULT_OVER_DSCHARGE_CURRENT_WARN_DETECT  ((int16_t)-18000) // 10mA
#define DEFAULT_MODULE_TEMP_DEV_FAULT_DETECT    1500    //factor 0.01
#define DEFAULT_MODULE_TEMP_DEV_WARNING_DETECT  1000    //factor 0.01
#define DEFAULT_CELL_CAPACITY                   3040        //100mA  fator 0.1
#define DEFAULT_TOTAL_RC                        DEFAULT_CELL_CAPACITY*3600 //100mA * sec 
#define DEFAULT_TOTAL_ENERGY                    3.2*DEFAULT_CELL_CAPACITY*8/10  // (A*PACKV)

// Power Bank Management 
#define DEFAULT_SOC_5PERCENT_CELL_V                 3160        //mV
#define DEFAULT_DSCHARGE_STOP_CELL_V                3000        //mV
#define DEFAULT_CHARGE_WAIT_CELL_V                  3160        //mV
#define DEFAULT_POWEDOWN_CELL_V                     2500        //mV
#define DEFAULT_FULLCHARGE_JUGE_CELL_V              3500        //mV
#define DEFAULT_FULLCHARGE_TAPPER_CELL_V            100          //mV
#define DEFAULT_FULLCHARGE_WAKEUPJUGE_CELL_V        3450        //mV
#define DEFAULT_FULLCHARGE_JUGE_CURRENT             4         //10mA
#define DEFAULT_DISCHARGE_JUGE_CURREN               ((int16_t) -1)  //10mA
#define DEFAULT_CHARGE_JUGE_CURRENT                 ((int16_t)1)  //10mA

#else
//error "need define"
#define DEFAULT_OVER_CELL_V_FAULT_DETECT              0
#define DEFAULT_OVER_CELL_V_WARNING_DETECT            0
#define DEFAULT_UNDER_CELL_V_FAULT_DETECT             0
#define DEFAULT_UNDER_CELL_V_WARNING_DETECT           0
#define DEFAULT_CELL_DEVIATION_V_FAULT_DETECT         0
#define DEFAULT_CELL_DEVIATION_V_WARNING_DETECT       0
#define DEFAULT_OVER_CELL_TEMP_FAULT_DETECT           0
#define DEFAULT_OVER_CELL_TEMP_WARNING_DETECT         0
#define DEFAULT_UNDER_CELL_TEMP_FAULT_DETECT          0
#define DEFAULT_UNDER_CELL_TEMP_WARNING_DETECT        0
#define DEFAULT_OVER_SOC_FAULT_DETECT                 0
#define DEFAULT_OVER_SOC_WARNING_DETECT               0
#define DEFAULT_UNDER_SOC_FAULT_DETECT                0
#define DEFAULT_UNDER_SOC_WARNING_DETECT              0
#define DEFAULT_OVER_BAT_VOL_FAULT_DETECT             0
#define DEFAULT_OVER_BAT_VOL_WARNING_DETECT           0
#define DEFAULT_UNDER_BAT_VOL_FAULT_DETECT            0
#define DEFAULT_UNDER_BAT_VOL_WARNING_DETECT          0
#define DEFAULT_OVER_CHARGE_CURRENT_FAULT_DETECT      0
#define DEFAULT_OVER_CHARGE_CURRENT_WARN_DETECT       0
#define DEFAULT_OVER_DSCHARGE_CURRENT_FAULT_DETECT    0
#define DEFAULT_OVER_DSCHARGE_CURRENT_WARN_DETECT     0
#define DEFAULT_MODULE_TEMP_DEV_FAULT_DETECT          0
#define DEFAULT_MODULE_TEMP_DEV_WARNING_DETECT        0

#endif

#define OVER_SOC_WARNING_DETECT         EEPROMData.bmiconf.OSOCW 
#define OVER_SOC_FAULT_DETECT 	 		EEPROMData.bmiconf.OSOCF
#define OVER_SOC_WARNING_WTIME         5000         //5s
#define OVER_SOC_FAULT_WTIME           5000         //5s
#define OVER_SOC_WARNING_RELEASE_TIME   30000       //30s
#define OVER_SOC_FAULT_RELEASE_TIME     30000       //30s

#define UNDER_SOC_WARNING_DETECT        EEPROMData.bmiconf.USOCW   
#define UNDER_SOC_FAULT_DETECT 	 	    EEPROMData.bmiconf.USOCF
#define UNDER_SOC_WARNING_WTIME         5000        //5s
#define UNDER_SOC_FAULT_WTIME           5000        //5s
#define UNDER_SOC_WARNING_RELEASE_TIME  30000       //30s
#define UNDER_SOC_FAULT_RELEASE_TIME    30000       //30s


#define OVER_CELL_V_WARNING_DETECT      EEPROMData.bmiconf.OCVW
#define OVER_CELL_V_FAULT_DETECT        EEPROMData.bmiconf.OCVF
// DTC 정책 변경으로 인해서 deprecated
/* 
#define OVER_CELL_V_PROTECT_DETECT      EEPROMData.bmiconf.OCVP   	// 4200 // mV
#define UNDER_CELL_V_PROTECT_DETECT     EEPROMData.bmiconf.UCVP
#define OVER_CELL_TEMP_PROTECT_DETECT 	   EEPROMData.bmiconf.OTP
#define OVER_CHARGE_CURRENT_PROTECT_DETECT    EEPROMData.bmiconf.OCCP
#define OVER_DSCHARGE_CURRENT_PROTECT_DETECT   EEPROMData.bmiconf.ODCP
#define MODULE_TEMP_DEV_PROTECT_DETECT        EEPROMData.bmiconf.TDP
#define OVER_BAT_VOL_PROTECT_DETECT     EEPROMData.bmiconf.OBVP        //(8190-15)    // 4.2 factor 0.1
#define UNDER_BAT_VOL_PROTECT_DETECT     EEPROMData.bmiconf.UBVP    //(5690)    // 2.7 factor 0.1
#define OCD_PROTECT       EEPROMData.bmiconf.OCD
*/
#define OVER_CELL_V_WARNING_WTIME           3000  	//3s
#define OVER_CELL_V_FAULT_WTIME             3000    //3s
#define OVER_CELL_V_PROTECT_WTIME             1000    //1s <-- jae.kim 설변품 부터 적용 
#define OVER_CELL_V_WARNING_RELEASE_TIME        3000   //30s
#define OVER_CELL_V_FAULT_RELEASE_TIME           3000   //30s


#define UNDER_CELL_V_WARNING_DETECT     EEPROMData.bmiconf.UCVW
#define UNDER_CELL_V_FAULT_DETECT      	EEPROMData.bmiconf.UCVF  // 2900 //mV

#define UNDER_CELL_V_WARNING_WTIME         	3000  	//3s
#define UNDER_CELL_V_FAULT_WTIME           	3000  	//3s
#define UNDER_CELL_V_PROTECT_WTIME           1000    //1s <-- jae.kim 설변품 부터 적용 
#define UNDER_CELL_V_WARNING_RELEASE_TIME  	3000  	//3s
#define UNDER_CELL_V_FAULT_RELEASE_TIME    	3000  	//3s

#define OVER_CELL_TEMP_WARNING_DETECT      EEPROMData.bmiconf.OTW
#define OVER_CELL_TEMP_FAULT_DETECT 	   EEPROMData.bmiconf.OTF
#define OVER_CELL_TEMP_WARNING_WTIME        5000   // 5s
#define OVER_CELL_TEMP_FAULT_WTIME          5000  //2s
#define OVER_CELL_TEMP_PROTECT_WTIME          2000  //2s
#define OVER_CELL_TEMP_WARNING_RELEASE_TIME    3000    //3s
#define OVER_CELL_TEMP_FAULT_RELEASE_TIME    3000    //3s


#define UNDER_CELL_TEMP_WARNING_DETECT      EEPROMData.bmiconf.UTW
#define UNDER_CELL_TEMP_FAULT_DETECT        EEPROMData.bmiconf.UTF
#define UNDER_CELL_TEMP_WARNING_WTIME         5000  // 5s
#define UNDER_CELL_TEMP_FAULT_WTIME           5000 // 5s
#define UNDER_CELL_TEMP_WARNING_RELEASE_TIME  3000    //3s
#define UNDER_CELL_TEMP_FAULT_RELEASE_TIME    3000    //3s


#define OVER_CHARGE_CURRENT_WARN_DETECT       EEPROMData.bmiconf.OCCW
#define OVER_CHARGE_CURRENT_FAULT_DETECT      EEPROMData.bmiconf.OCCF
#define OVER_CHARGE_CURRENT_WARNING_WTIME       3000    //3s 
#define OVER_CHARGE_CURRENT_FAULT_WTIME         3000    //3s
#define OVER_CHARGE_CURRENT_PROTECT_WTIME          1000    //1s <-- jae.kim 설변품 부터 적용 
#define OVER_CHARGE_CURRENT_WARNING_RELEASE_TIME       3000    //3s 
#define OVER_CHARGE_CURRENT_FAULT_RELEASE_TIME         3000    //3s

#define OVER_DSCHARGE_CURRENT_WARN_DETECT      EEPROMData.bmiconf.ODCW
#define OVER_DSCHARGE_CURRENT_FAULT_DETECT     EEPROMData.bmiconf.ODCF
#define OVER_DSCHARGE_CURRENT_WARNING_WTIME       3000    //3s 
#define OVER_DSCHARGE_CURRENT_FAULT_WTIME         3000    //3s
#define OVER_DSCHARGE_CURRENT_PROTECT_WTIME          1000    //1s <-- jae.kim 설변품 부터 적용 
#define OVER_DSCHARGE_CURRENT_WARNING_RELEASE_TIME       3000    //30s 
#define OVER_DSCHARGE_CURRENT_FAULT_RELEASE_TIME        3000    //30s 


#define CELL_DEVIATION_DETECT_UPPER_CONDITION_SOC    9900    // factor 0.01
#define CELL_DEVIATION_DETECT_LOWER_CONDITION_SOC   1000    // factor 0.01
#define CELL_DEVIATION_DETECT_CURRENT_CONDITION      5000    // mA
#define CELL_DEVIATION_DETECT_VOLTAGE           600 //EEPROMData.bmiconf.CVD 
#define CELL_DEVIATION_RELEASE_VOLTAGE               500    //  mV
#define CELL_DEVIATION_FAULT_WTIME                   10000   // 10s
#define CELL_DEVIATION_FAULT_RELEASE_TIME            3000   // 3s

#define MODULE_TEMP_DEV_WARNING_DETECT        EEPROMData.bmiconf.TDW
#define MODULE_TEMP_DEV_FAULT_DETECT          EEPROMData.bmiconf.TDF
#define MODULE_TEMP_DEV_WARNING_WTIME         5000    //5s
#define MODULE_TEMP_DEV_FAULT_WTIME           5000    //5s
#define MODULE_TEMP_DEV_PROTECT_WTIME         5000      //5s

#define MODULE_TEMP_DEV_WARNING_RELEASE_TIME        3000    //3s
#define MODULE_TEMP_DEV_FAULT_RELEASE_TIME          3000    //3s

#define OVER_BAT_VOL_WARNING_DETECT     EEPROMData.bmiconf.OBVW
#define OVER_BAT_VOL_FAULT_DETECT       EEPROMData.bmiconf.OBVF        //(8190-15)    // 4.2  factor 0.1

#define OVER_BAT_VOL_WARNING_WTIME      3000    //3s
#define OVER_BAT_VOL_FAULT_WTIME      3000    //3s
#define OVER_BAT_VOL_PROTECT_WTIME       1000    //1s <-- jae.kim 설변품 부터 적용 
#define OVER_BAT_VOL_WARNING_RELEASE_TIME      3000    //3s
#define OVER_BAT_VOL_FAULT_RELEASE_TIME      3000    //3s


#define UNDER_BAT_VOL_WARNING_DETECT     EEPROMData.bmiconf.UBVW
#define UNDER_BAT_VOL_FAULT_DETECT       EEPROMData.bmiconf.UBVF    //(5690)    // 3.0  factor 0.1

#define UNDER_BAT_VOL_WARNING_WTIME      3000    //3s
#define UNDER_BAT_VOL_FAULT_WTIME      3000    //3s
#define UNDER_BAT_VOL_PROTECT_WTIME       1000    //1s <-- jae.kim 설변품 부터 적용 
#define UNDER_BAT_VOL_WARNING_RELEASE_TIME      3000    //3s
#define UNDER_BAT_VOL_FAULT_RELEASE_TIME      3000    //3s

#define ISOL_WARNING_REGISTANCE 1000000 //1K옴 
#define ISOL_WARNING_RELEASE_TIME 3000    //3s
#define ISOL_FAULT_REGISTANCE   300000  //500k옴
#define ISOL_FAULT_WTIME        1000    //1S
#define ISOL_FAULT_RELEASE_TIME 3000    //3s

#define OCD_PROTECT_WTIME   100

#define RELAY_WARNING_WTIME 5000    //5sec
#define RELAY_FAULT_WTIME 3000    // 3sec (+10sec) total 30sec
#define RELAY_WARNING_RELEASE_TIME 3000
#define RELAY_FAULT_RELEASE_TIEM 3000

#define SUBMCU_TIMEOUT_TIME     5000

#define TRAY_COMM_FAULT_TIME  30000
#define MASTER_COMM_FAULT_TIME 30000

#endif //__BMS_H__

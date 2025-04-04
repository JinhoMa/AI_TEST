/*
 * SYS_ERR_CHK.h
 *
 *  Created on: 2014. 4. 23.
 *      Author: si14294
 */

#ifndef SYS_ERR_CHK_NEW_H_
#define SYS_ERR_CHK_NEW_H_

enum DtcTypeTag {WARNING=0, FAULT, PROTECT };
enum DtcOccurenceTag {NotDeteced=0, Detected}; /* Error Detection Flag */
enum DtcRelaseCondTag {NoRelease=0, Release, NoReleaseHold };  /* Release Function Active Flag */
enum eachErrorTag {eachNormal=0, eachWarning, eachWarningFault, eachFault};
enum DtcActionTag{ Alarm= 0, RelayOpen=1};

struct DTC_Memory
{
  uint32_t DtcNumber;
  uint16_t DtcType;
  uint16_t Occurence;
  uint16_t ReleaseCond;
  uint16_t Action;
};

typedef struct
{
  uint16_t Set;
  //uint16_t Release;
  uint64_t Start;
}STR_ErrorTime;

typedef struct
{
  uint16_t Warning:1;
  uint16_t Fault:1;
}STR_Error;

typedef union
{
  uint16_t all:3;
}UNION_Eror;

typedef struct
{
  UNION_Eror Status;
  STR_ErrorTime time;
}STR_ErrorModel;

typedef struct
{
    STR_ErrorModel DtcErr;
    uint16_t reason_code;
} STR_SMErrorModel;

typedef struct
{
    uint16_t code;
    struct RTC_Time t;
} STR_DiagData;

/* DTC Entry Index Definition */
#define IDC_BAT_OV_W            0
#define IDC_BAT_OV_F             1
#define IDC_BAT_UV_W            2
#define IDC_BAT_UV_F             3
#define IDX_CELL_OV_W           4
#define IDX_CELL_OV_F            5
#define IDX_CELL_UV_W           6
#define IDX_CELL_UV_F            7
#define IDX_CELL_OT_W           8
#define IDX_CELL_OT_F            9
#define IDX_CELL_UT_W           10
#define IDX_CELL_UT_F            11
#define IDC_MODTEMP_DEV_W         12
#define IDC_MODTEMP_DEV_F         13
#define IDX_CELL_DEVIATION_W         14
#define IDX_CELL_DEVIATION_F         15
#define IDX_ISOL_W         		  			  16
#define IDX_ISOL_F         		  			  17
#define IDX_TBMS_RX_W           			  18
#define IDX_TBMS_RX_F           			  19
#define IDX_MBMS_RX_W           			  20
#define IDX_MBMS_RX_F           			  21
#define IDX_RELAY_W             	 		  22
#define IDX_RELAY_F             	 		  23
#define IDX_FUSE_W             	 		  24
#define IDX_FUSE_F             	 		  25
#define IDX_OV_CHRG_CURR_W        26
#define IDX_OV_CHRG_CURR_F         27
#define IDX_OV_DSCHRG_CURR_W      28
#define IDX_OV_DSCHRG_CURR_F      29
#define IDX_OVER_SOC_W		      30
#define IDX_OVER_SOC_F		  	 31
#define IDX_UNDER_SOC_W		32
#define IDX_UNDER_SOC_F		      33
#define IDX_SM_W                               34
#define IDX_SM_F                                35

//#define IDX_HUGE_CHARGE_CURRENT_F	  3
//#define IDX_HUGE_DISCHARGE_CURRENT_F  4


#define NUM_DTC                  (IDX_SM_F+1)

/* DTC Code Definition */
// Warning: even, Fault: Odd
/**
   * RACK Over Voltage
   */
#define DTC_BAT_OV_W                0x0100
#define DTC_BAT_OV_F                 0x0101
/**
   * RACK Under Voltage
   */
#define DTC_BAT_UV_W                0x0200
#define DTC_BAT_UV_F                 0x0201
/**
   * Cell Over Voltage
   */
#define DTC_CELL_OV_W               0x0300
#define DTC_CELL_OV_F                0x0301
/**
   * Cell Under Voltage
   */
#define DTC_CELL_UV_W               0x0400
#define DTC_CELL_UV_F                0x0401
/**
   * Module Over Temperature
   */
#define DTC_MOD_OT_W               0x0500
#define DTC_MOD_OT_F                0x0501
/**
   * Module Under Temperature
   */
#define DTC_MOD_UT_W               0x0600
#define DTC_MOD_UT_F                0x0601
/**
   * Temperature Deviation
   */
#define DTC_TEMP_DEVIATION_W    0x0700
#define DTC_TEMP_DEVIATION_F     0x0701
/**
   * CellVoltage Deviation
   */
#define DTC_CELL_DEVIATION_W     0x0800
#define DTC_CELL_DEVIATION_F      0x0801
/**
   * Isolation Failure
   */
#define DTC_ISOL_W                    0x0900
#define DTC_ISOL_F                     0x0901
/**
   * TrayBMS Communication ERROR
   */
#define DTC_TBMS_RX_W              0x0A00
#define DTC_TBMS_RX_F               0x0A01
/**
   * MasterBMS Communication ERROR
   */
#define DTC_MBMS_RX_W             0x0B00
#define DTC_MBMS_RX_F              0x0B01
/**
   * Relay Fault
   */
#define DTC_RELAY_W                 0x0D00
#define DTC_RELAY_F                  0x0D01
/**
   * Fuse Fault
   */
#define DTC_FUSE_W                  0x0E00
#define DTC_FUSE_F                   0x0E01
/**
   * Over Charge Current
   */
#define DTC_OV_CHRG_CURR_W    0x0F00
#define DTC_OV_CHRG_CURR_F     0x0F01
/**
   * Over DisCharge Current
   */
#define DTC_OV_DSCHRG_CURR_W 0x1000
#define DTC_OV_DSCHRG_CURR_F  0x1000
/**
   * Over SOC Fault
   */

#define DTC_OVER_SOC_W	           0x1100
#define DTC_OVER_SOC_F             0x1101
/**
   * Under SOC Fault
   */

#define DTC_UNDER_SOC_W          0x1200
#define DTC_UNDER_SOC_F           0x1201

/**
   * SM SUBMCU WATCHDOG
   */
#define DTC_SM_W                         0x1300
#define DTC_SM_F                          0x1301


#define SM_F_SUBMCU_WATCHDOG            0x0001
#define SM_F_SUBMCU_TIMEOUT             0x0002
#define SM_F_EEPROM_CHKSUM              0x0004
#define SM_F_VOLPLB_ERROR               0x0008
#define SM_F_MCUFAULT                   0x0010

   
extern uint16_t rub_RelayErrCode;
extern uint16_t Relay_state_ctrl;
extern STR_SMErrorModel DTC_SM;


void TBMS_STATUS(void);
void _Diag_Update_Huge_ChargeCurrent();
void _Diag_Update_Huge_DischargeCurrent();
void _Diag_Update_Bat_Over_Cell_Volt_WNF(void);      //--Added By Choi
void _Diag_Update_Bat_Under_Cell_Volt_WNF(void);	 //--Added By Choi
void _Diag_Update_Over_Cell_Temp_WNF(void);      //--Added By Choi
void _Diag_Update_Under_Cell_Temp_WNF(void);      //--Added By Choi
void _Diag_Update_Bat_Over_Charge_Current_WNF1(void);
void _Diag_Update_Bat_Over_Discharge_Current_WNF1(void);
void _Diag_Update_Cell_Deviation();
void _System_Isolation_Failure(void);
void _System_Relay_Faiure(void);
void _Diag_Update_MBMS_Rx_Status(void);
void _Diag_Update_Under_SOC_WNF(void);
void _Diag_Update_Over_SOC_WNF(void);
void Diag_ErrorReset();
uint16_t ConvertDTCTbleToErrTble(uint16_t dtc, uint16_t subcode);

/*
void _Diag_Update_Bat_Under_Total_Volt_WNF(void);
//void _Diag_Update_Bat_Under_Total_Volt_W(void);
//void _Diag_Update_Bat_Under_Total_Volt_F(void);
void _Diag_Update_Bat_Over_Total_Volt_WNF(void);
//void _Diag_Update_Bat_Over_Total_Volt_W(void);
//void _Diag_Update_Bat_Over_Total_Volt_F(void);

//void _Diag_Update_Bat_Over_Charge_Current_W1(void);
//void _Diag_Update_Bat_Over_Charge_Current_F1(void);
void _Diag_Update_Bat_Over_Charge_Current_WNF2(void);
//void _Diag_Update_Bat_Over_Charge_Current_W2(void);
//void _Diag_Update_Bat_Over_Charge_Current_F2(void);

//void _Diag_Update_Bat_Over_Discharge_Current_W1(void);
//void _Diag_Update_Bat_Over_Discharge_Current_F1(void);
void _Diag_Update_Bat_Over_Discharge_Current_WNF2(void);
//void _Diag_Update_Bat_Over_Discharge_Current_W2(void);
//void _Diag_Update_Bat_Over_Discharge_Current_F2(void);
void _Diag_Update_Bat_Under_SOC(void);
void _Diag_Update_Bat_Over_SOC(void);

void _System_CurrSense_Failure(void);
*/

PUBLIC uint32_t DTC_RBMS_Status(void);
PUBLIC int32_t DTC_RBMS_COM_ERR(void);
PUBLIC void DTC_RBMS_Alarm(uint32_t code, uint8_t set, uint8_t reason);


PUBLIC void ClearDTCTable();

#endif /* SYS_ERR_CHK_NEW_H_ */

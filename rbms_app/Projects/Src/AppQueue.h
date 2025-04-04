/*
 * Queue.h
 *
 *  Created on: 2013. 9. 9.
 *      Author: iw00066
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "obshal.h"				// Peripheral address definitions
#include "hos.h"
#include "hlog.h" //log
#include "BMS.h"

#define EEPROM_SCHEDULER_IDX 0
#define CANB_SCHEDULER_IDX 1
#define EXTCOM_SCHEDULER_IDX 2
#define BMIC_SCHDEULER_IDX 3
#define SUBMIC_SCHEDULER_IDX 4
// #define CANA_SCHEDULER_IDX 2
#define RBMS_SCHEDULER_IDX 5
#define MBMS_SCHEDULER_IDX 6
#define LAST_IDX    10


//---- Scheduler Parameter
#define SCHEDULER_NUMBER LAST_IDX

#define JOB_QUEUE_SIZE 100

#define EEPROM 1

//----ExtCom Job Index
#define EXTCOM_RS485_READ   1
#define EXTCOM_RS485_OUT    2
#define EXTCOM_BLE_OUT      3
#define EXTCOM_BLE_READ     4
#define EXTCOM_PWR_BUTTON   5

//----BMIC Job Index
#define INIT_DAISYCHAIN     1
#define INTERNAL_DIAGNOSIS      2
#define COMMFAULT_DIAGNOSIS     3
#define BMIC_SCAN_N_FANMODE_N_LEDFAULT 4

#define EACH_VOLTAGE_READ_ODD 5
#define EACH_VOLTAGE_READ_EVEN 6
#define MODULE_VOLTAGE_READ  7
#define MODULE_BLOCKVOLTAGE_N_TEMPERATURE_READ 8
#define SUMMARY_AND_ALERT_READ_1 9
#define SUMMARY_AND_ALERT_READ_2 10
#define VOL_N_TEMP_EEPROM_UPDATE 11
#define EACH_VOLTAGE_READ_ALL   12
#define CELL_BALANCING_START_EVENT  13
#define CELL_BALANCING_EVENT 14
#define CELL_BALANCING_STOP_EVENT 15


#define HW_DIAG_EXTERNAL_FET1 16
#define HW_DIAG_EXTERNAL_FET2 17
#define HW_DIAG_CNPIN1 18
#define HW_DIAG_CNPIN2 19
#define HW_DIAG_CBCONTROL 20

#define DAISYCHAIN_FAULT 21
#define CLEARSTATUS_FAULT 22
#define INIT_THRESHOLD_FAULT 23
#define ALERTEN_BALANCINGDN_FAULT 25
#define ALERT_READ_FAULT 26

//향후 CAN 통신으로 바뀔것을 대비해서 ,RBMS에서 직접 메모리접근안하고
//통신으로 받는다.
#define COMM_TRAY_TO_RBMS 27
#define CELL_BALANCING_READ_EVENT 28


//---- EEPROM Job Index
#define FACTORYREST_FROM_DIAGMODE 1
#define ULOCKWRITE_FROM_PCTOOL   2
#define LOCKWRITE_FROM_PCTOOL   3
#define READBYTE_FROM_PCTOOL    4
#define WRITEBYTE_FROM_PCTOOL   5
#define FACTORYREST_FROM_PCTOOL 6
#define WRITEBYTE_FROM_DIAGMODE 7
#define READBYTE_FROM_DIAGMODE  8
#define READMODEL_FROM_DIAGMODE 9
#define READVER_FROM_DIAGMODE   10
#define RTCSET_FROM_PCTOOL      11
#define UPDATE_USEDATA_PER           12
#define UPDATE_USEDATA_SEC           13
#define BACKGROUND_DEVLOPMODE    14
#define RTCSET_FROM_LINETEST    15
#define STARTINGSOH_FROM_PCTOOL  16
#define ERASEALL_FROM_PCTOOL        17
#define CLEAR_FAULT_RECORD            18
#define UPDATE_SOHTABLE_FROM_SOH     19

//----CANA Job Index

#define MEASURING_START_TX 100
#define MBMS_NUMBER_TX 101
#define FAULT_STATUS_TX 102
#define CELL_BALANCING_TX 103
#define LINETEST_TX 104
#define DEVELOPER_CANA_TX 105
//----CANB Job Index

#define INIT_LINETEST           1
#define READ_DTC_FROM_DIAGMODE  2
#define CLR_DTC_FROM_DIAGMODE   3
#define CLR_DTC_FROM_BMICDIAG   4
#define READ_CELLNUM_FROM_DIAGMODE  5
#define READ_BLOCKNUM_FROM_DIAGMODE 6
#define READ_MDTMPNUM_FROM_DIAGMODE   7
#define READ_BDTMPNUM_FROM_DIAGMODE     8       
#define GET_INTLOCKST_FROM_DIAGMODE    9  
#define READ_TMP_FROM_DIAGMODE      10      //Deprecated
#define FACTORYRESET_DIAG_SUCCESS_FROM_EEP 11
#define RET_READBYTE_FROM_EEP         12
#define RET_WRITEBYTE_FROM_EEP          13
#define SEND_SEGDATA_TO_PCTOOL          14
#define FACTORYRESET_PC_SUCCESS_FROM_EEP 15
#define WRTEST_SUCCESS_FROM_NET         16
#define RDTEST_SUCCESS_FROM_NET            17
#define WRBYTE_DIAG_SUCCESS_FROM_EEP    18
#define RDBYTE_DIAG_SUCCESS_FROM_EEP     19
#define READ_RACK_TRAYCNT_FROM_DIAGMODE      20
#define READ_RACK_TRAYCELLVOL_FROM_DIAGMODE 21
#define READ_RACK_TRAYTEMP_FROM_DIAGMODE 22
#define READ_BPU_BPUTMP_FROM_DIAGMODE  23
#define READ_RACK_CELLDEV_FROM_DIAGMODE 24
#define CTRL_BPU_P_RLYST_FROM_DIAGMODE  25
#define CTRL_BPU_N_RLYST_FROM_DIAGMODE  26
#define READ_MODEL_SUCCESS_FROM_EEP     27
#define READ_VER_SUCCESS_FROM_EEP       28
#define RETURN_RELAYCMD_FROM_R10MS      29
#define SEND_EVTLOGDATA_TO_PCTOOL       30
#define ERASEALL_SUCCESS_FROM_EEP           31

#define DIAG_LOG_OUT                 100

//RBMS_10MS_SCHEDULER_IDX
#define R10MS_CHK_PACKVOLTAGE   1
#define R10MS_GET_COMMANDFROMMSG        2
#define R10MS_DO_RELAYCONTROL     3
#define R10MS_DO_GFDCONTROL      4
//#define R10MS_READ_CURRENTSENSOR  5
//#define R10MS_DIAG_UPDATE         6
#define R10MS_LED_UPDATE         7
#define R10MS_RELAYCMD_FROM_DIAGMOD    8

//RBMS_SCHEDULER_IDX
#define RBMS_RELAYCONTROL      1
#define RBMS_SYS_POWERDOWN      2
#define RBMS_UPDATE_HOST_DATA    3
#define RBMS_SEND_REQTRAY   4    //1,2,3,4 합쳐야 함.
#define RBMS_RECV_RSPTRAY    5
#define RBMS_COMM_CHECK      6
#define RBMS_DIAG_UPDATE         7
#define RBMS_BLOWOFF_FUSE        8
#define RBMS_SOC                  9
#define RBMS_FULLCHARGE 10
#define RBMS_FULLDIS    11
#define RBMS_BALANCING_CHK   12
#define RBMS_WATCHDOG_CHK   13
#define RBMS_GET_TEMP     14      //& 보드 온도 측정

//RBMS_100MS_SCHEDULER_IDX
#define R100MS_TEMPCAL_SOC             1
#define R100MS_CELLVOL_SOC            2
#define R100MS_TOTALCAL_SOC           3

//RBMS_500MS_SCHEDULER_IDX
#define R500MS_GET_TEMP     1
#define R500MS_RUN_AUTOBAL             2
#define R500MS_GET_SOH  3
#define R500MS_SM_MONITOR      4

//NETWORK_SCHEDULER_IDX
#define NET_INIT_DEVICE          1
#define NET_CHK_PHYCONNECT       2
#define NET_CHK_IP           3
#define NET_END_CONFIG           4
#define NET_POLL_STATUS          5
// MESA_SERVER_IDX (SUBJOB OF NETWORK_SCHEDULER_IDX)
#define NET_APP_STATUS          6
#define NET_WRTEST_FROM_DIAGMODE    7
#define NET_RDTEST_FROM_DIAGMODE    8
#define NET_APP2_STATUS          9


//SUBMIC_SCHEDULER_IDX
#define SUBMIC_INIT_VAR         1
#define SUBMIC_INIT_DEVICE      2
#define SUBMIC_CONFIG_WRITE     3
#define SUBMIC_CONFIG_READ      4
#define SUBMIC_POLLING          5
#define SUBMIC_CAL_MSRREQ       6
#define SUBMIC_CAL_SAVECAL       7
#define SUBMIC_CMD_POWERDOWN     8
#define SUBMIC_CMD_BLOW_FUSE   9
#define SUBMIC_CMD_CELLBAL     10

//MBMS_SCHEDULER_IDX
#define MBMS_MODE_CHECK      1
#define MBMS_FAULT_CHECK     2

extern void SEM_post(hOsSemaId_t sm);
extern void SEM_pend(hOsSemaId_t sm, int mode);
void Init_Scheduler(void);
int32_t PopJob(uint16_t SchedulerIdx, void **);
void PushJob(uint16_t SchedulerIdx,uint16_t Job, uint32_t t, void *);
void FreeMsg(void *);
void CleanUpMsg(void *);
int32_t IsFreeMsg(void *);
void ReserveMsg(void *, uint8_t);
/*
typedef struct
{
	uint16_t strt_indx;
	uint16_t end_indx;
	uint16_t current_Q_sz;
	uint16_t curJob;
	uint16_t overJob;
	uint16_t jobArr[JOB_QUEUE_SIZE];
}STR_SchedulerModel;
*/
typedef struct _JOB{
    uint8_t idx;
    uint8_t used;
    uint64_t sch_time;
    void *data;
    struct _JOB *next;
} JOB;

typedef struct {
    JOB job[JOB_QUEUE_SIZE];
    JOB *joblist;
    int usedCnt;
}SchedulerModel;

typedef struct _MH{
    uint8_t used;
    uint8_t who;
} MH;

extern SchedulerModel Scheduler[SCHEDULER_NUMBER];

#endif /* QUEUE_H_ */












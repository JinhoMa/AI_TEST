
#ifndef BMIC_PROCESSDATA_H_
#define BMIC_PROCESSDATA_H_
#include "BMS.h"

#define ALL_CELLDATA 0
#define ODD_CELLDATA 1
#define EVEN_CELLDATA 2

#define NOMAL_STORAGE 0
#define DIAG_STORAGE 1

#define TEMP_SAMPLE_COUNT  3
#define CELL_SAMPLE_COUNT  3
#define CELLDEVIATION 300 						// mV

//--
typedef struct{
    uint8_t lsb;
    uint8_t msb;
}Byte;



//17823
//--------------------------------------
typedef struct
{
	uint16_t Balancing:1;
	uint16_t Vol_Simulation:1;
	uint16_t Temp_Simulation:1;
	uint16_t Fan_Simulation:1;
	uint16_t Fan_Simulation_TestRun:2;

	uint16_t BMIC_Fault:1;
}STR_Mode;



typedef union
{
	uint16_t all:7;
	STR_Mode bit;
}UNI_ModeModel;


typedef struct
{
	uint16_t  rsvd1:1; //Uint16 BalancingCMD:1;
	uint16_t ResetDiagnosisCMD:1;
	uint16_t CheckBalancingPathCMD:1;
	uint16_t CheckCNCMD:1;
}STR_SpecialStatusRX;

typedef union
{
	uint16_t all:4;
	STR_SpecialStatusRX bit;
}UNI_SpecialStatusRXModel;


typedef struct
{
	uint16_t V_Validation_F:1;
	uint16_t T_Validation_F:1;
	uint16_t  rsvd1:2;  //	Uint16 Balancing:2;
	uint16_t BMIC_Init_Cplt:1;
	uint16_t CnPinOpenCheck_Cplt:1;
	uint16_t BalancingPathCheck_Cplt:1;
	uint16_t FanOnOff:1;
}STR_SpecialStatusTx;

typedef union
{
	uint16_t all:8;
	STR_SpecialStatusTx bit;
}UNI_SpecialStatusTxModel;

typedef struct
{
	UNI_SpecialStatusTxModel Tx;
	UNI_SpecialStatusRXModel Rx;
}STR_SpecialStatusModel;


typedef struct
{
	uint16_t  rsvd1:8;    // 7:0 reserved
	uint16_t CBTIMER :4;
	uint16_t CBPDIV :3;
	uint16_t rsvd2 :1;  // 15 reserved
}STR_WatchdogReg;

typedef union
{
	uint16_t all;
	STR_WatchdogReg bit;
}UNI_WatchdoggModel;



typedef struct
{
	uint16_t ALRTGNDL1 :1;
	uint16_t ALRTVDDL1 :1;
	uint16_t ALRTGNDL3 :1;
	uint16_t ALRTHVUV :1;
	uint16_t ALRTTEMP :1;
	uint16_t ALRTBALSW :1;
	uint16_t ALRTGNDL2 :1;
	uint16_t ALRTVDDL2 :1;
	uint16_t ALRTVDDL3 :1;
	uint16_t ALRTCOMMSEL2 :1;
	uint16_t ALRTCOMMSEU2 :1;
	uint16_t ALRTCOMMSEL :1;
	uint16_t ALRTCOMMSEU :1;
	uint16_t Reserved :1;
	uint16_t ALRTOSC2 :1;
	uint16_t ALRTOSC :1;
}STR_FMEA_Status;


typedef union
{
	uint16_t all;
	STR_FMEA_Status bit;
}UNI_FMEAReg_FatultModel;


typedef struct
{
	uint16_t FMEAReg_SetCount;
	uint16_t FMEAReg_ClearCount;

}STR_FMEARegAlertCNT;



typedef struct
{
	STR_FMEARegAlertCNT Count;
	uint16_t Warning_FMEA;
	uint16_t Fault_FMEA;
}STR_FMEAReg_AlertModel;

typedef struct
{
	UNI_FMEAReg_FatultModel Reg;
	STR_FMEAReg_AlertModel Alert;

}STR_FMEAReg_FatultModel;


//--------------------------------------

typedef struct
{
	uint16_t Reference: 1;
	uint16_t Vaa: 1;
	uint16_t LSAmp: 1;
	uint16_t ADCZeroScale: 1;
	uint16_t ADCFullScale: 1;
	uint16_t DieTemp: 1;
}STR_InternalREF_Fautlt;

typedef union
{
	uint16_t all;
	STR_InternalREF_Fautlt bit;
}UNI_InternalREF_FatultModel;




typedef struct
{
	uint16_t Reference_SetCount;
	uint16_t Reference_ClearCount;
	uint16_t Vaa_SetCount;
	uint16_t Vaa_ClearCount;
	uint16_t LSAmp_SetCount;
	uint16_t LSAmp_ClearCount;
	uint16_t ADCZeroScale_SetCount;
	uint16_t ADCZeroScale_ClearCount;
	uint16_t ADCFullScale_SetCount;
	uint16_t ADCFullScale_ClearCount;
	uint16_t DieTemp_SetCount;
	uint16_t DieTemp_ClearCount;
}STR_InternalREFAlertCNT;

typedef union
{
	uint16_t all:6;
	STR_InternalREF_Fautlt bit;
}UNI_InternalREFWaringModel;

typedef union
{
	uint16_t all:6;
	STR_InternalREF_Fautlt bit;
}UNI_InternalREFFaultgModel;

typedef struct
{
	STR_InternalREFAlertCNT Count;
	UNI_InternalREFWaringModel Warning;
	UNI_InternalREFFaultgModel Fault;
}STR_InternalREF_AlertModel;

typedef struct
{
	UNI_InternalREF_FatultModel Reg;
	STR_InternalREF_AlertModel Alert;

}STR_InternalREF_FatultModel;



//--------------------------------------

typedef struct
{
	uint16_t ALRTFMEA :1;
	uint16_t ALRTFMEA2 :1;
	uint16_t ALRTPARUP :1;
	uint16_t Reserved_1 :1;
	uint16_t ALRTMANUP:1;
	uint16_t Reserved_2 :1;
	uint16_t Reserved_3 :1;
	uint16_t ALRTPEC :1;
	uint16_t ALRTTHOT :1;
	uint16_t ALRTTCOLD :1;
	uint16_t ALRTMSMTCH :1;
	uint16_t ALRTSHDNLRT :1;
	uint16_t ALRTSHDLL :1;
	uint16_t ALRTUV :1;
	uint16_t ALRTOV :1;
	uint16_t ALRTRST :1;
}STR_StatusReg;

typedef union
{
	uint16_t all;
	STR_StatusReg bit;
}UNI_StatusRegModel;

typedef struct
{
	uint16_t SCAN :1;
	uint16_t DATAMOVE :1;
	uint16_t SCANMODE :1;
	uint16_t ZERO :1;
	uint16_t OVRSAMPL0:1;
	uint16_t OVRSAMPL1 :1;
	uint16_t OVRSAMPL2 :1;
	uint16_t POLRTY :1;
	uint16_t BALSWDIAG0 :1;
	uint16_t BALSWDIAG1 :1;
	uint16_t BALSWDIAG2 :1;
	uint16_t RSVED1 :1;
	uint16_t RSVED2 :1;
	uint16_t DATARDY :1;
	uint16_t SCANTIMEOUT :1;
	uint16_t SCANDONE :1;
}STR_ScanCtlReg;

typedef union
{
    uint16_t all;
    Byte byte;    
    STR_ScanCtlReg bit;
}STR_ScanCtrl;

typedef struct
{
    uint8_t reg[6];
} CFGB_C;

typedef struct
{
	UNI_StatusRegModel StatusReg;
	STR_FMEAReg_FatultModel FMEA;
	STR_InternalREF_FatultModel InternalREF;
	UNI_WatchdoggModel WatchdogReg;
    STR_ScanCtrl ScanCtrl;
}STR_17823_RegisterModel;

typedef struct{
    uint16_t gpio:1;
    uint16_t pwr:1;
    uint16_t clk:1;
    uint16_t cmp:1;
    uint16_t ow:1;
} STR_FMEA_Status_;

typedef union
{
	uint16_t all;
	STR_FMEA_Status_ bit;
} FMEA_FatultModel;


typedef struct{
    uint16_t oscchk:1;
    uint16_t tmodchk:1;
    uint16_t thsd:1;
    uint16_t sleep:1;
    uint16_t spiflt:1;
    uint16_t comp:1;
    uint16_t vde:1;
    uint16_t vdel:1;
    uint16_t smed:1;
    uint16_t sed:1;
    uint16_t cmed:1;
    uint16_t ced:1;
    uint16_t vduv:1;
    uint16_t vdov:1;
    uint16_t vauv:1;
    uint16_t vaov:1;
} STR_Status;

typedef union
{
	uint16_t all;
	STR_Status bit;
} FMEA_FaultModel;

typedef struct fmea_reg_t{
    uint16_t csflt;
    uint16_t ct;
    FMEA_FaultModel   fltchk;
}  FMEA_REG_T;

typedef struct
{
//	UNI_StatusRegModel StatusReg;
    
	FMEA_REG_T FMEA;
	STR_InternalREF_FatultModel InternalREF;
	UNI_WatchdoggModel WatchdogReg;
    STR_ScanCtrl ScanCtrl;
    CFGB_C       rd_cfgb;
}STR_6830_RegisterModel;


typedef struct
{
	uint16_t Cell1:1;
	uint16_t Cell2:1;
	uint16_t Cell3:1;
	uint16_t Cell4:1;
	uint16_t Cell5:1;
	uint16_t Cell6:1;
	uint16_t Cell7:1;

}STR_ExternalFet;

typedef union
{
	uint16_t all:7;
	STR_ExternalFet bit;
}STR_ExternalFetModel;



typedef struct
{
	uint16_t CnPinFault;
	STR_ExternalFetModel ExtFetFault;
}STR_HW_DiagnosisModel;



//--------------------------------------



typedef struct
{
	uint16_t OV:1;
	uint16_t UV:1;
}STR_AbnormalVoltage;

typedef union
{
	uint16_t all:2;
	STR_AbnormalVoltage bit;
}UNI_CellStatus;

typedef struct
{
	UNI_CellStatus Summary;
	UNI_CellStatus BySW;
	UNI_CellStatus ByBMIC;
}STR_CellVoltageStatusModel;

typedef struct
{
	uint16_t Balancing:1;
	STR_CellVoltageStatusModel VoltageStatus;
}STR_CellStatusModel;

typedef struct
{
	uint16_t OV_SetCount;
	uint16_t OV_ClearCount;
	uint16_t UV_SetCount;
	uint16_t UV_ClearCount;
}STR_VoltageAlertCNT;

typedef union
{
	uint16_t all:2;
	STR_AbnormalVoltage bit;
}UNI_VoltageWaringModel;

typedef union
{
	uint16_t all:2;
	STR_AbnormalVoltage bit;
}UNI_VoltageFaultgModel;

typedef struct
{
	STR_VoltageAlertCNT Count;
	UNI_VoltageWaringModel Warning;
	UNI_VoltageFaultgModel Fault;
	uint16_t VoltageStatus_Conversion;
}STR_VoltageAlertModel;

typedef struct
{
	UNI_Uint16Data CellVoltage;
	UNI_Uint16Data BufferCellVoltage;
	STR_CellStatusModel Status;
	STR_VoltageAlertModel Alert;
	uint16_t OnBalancing;
	uint16_t ZeroVoltFlag;
    UNI_Uint16Data SampleVolt[CELL_SAMPLE_COUNT];
    uint8_t samplePos;
    uint8_t used;
}STR_CellModel;





//------------------------------------------
typedef struct
{
	uint16_t CellNumber;
	UNI_Uint16Data Voltage;
}STR_SpecificCellModel;


typedef struct
{
	uint16_t Fault;
	UNI_Uint16Data Value;
}STR_ModuleCellDeviation;


typedef struct
{
	uint16_t MisMatch_SetCount;
	uint16_t MisMatch_ClearCount;

}STR_VoltageMisMatchAlertCNT;

typedef struct
{
	STR_VoltageMisMatchAlertCNT Count;
	uint16_t Warning_MisMatch;
	uint16_t Fault_MisMatch;
}STR_VoltageMisMatchAlertModel;

typedef struct
{
	STR_VoltageMisMatchAlertModel Alert;
	uint16_t Fault;
	UNI_Uint16Data Value;
}STR_TrayCellDeviation;


typedef struct
{
	UNI_Uint16Data CellSumBySW;
    UNI_Uint16Data CellSumMV;
	UNI_Uint16Data BlockByBMIC;
	UNI_Uint16Data TotalByBMIC;
}STR_VoltageSummaryModel;

typedef struct
{
	uint32_t DeviceID;

	STR_VoltageSummaryModel ModuleVoltage;
	STR_SpecificCellModel MinCell;
	STR_SpecificCellModel MaxCell;
	STR_ModuleCellDeviation VoltageMismatch;
    union {
	STR_17823_RegisterModel Register_17823;
        STR_6830_RegisterModel Register_6830;
    } u;
	STR_CellModel Cells[BMIC_MAX_CH];
	STR_HW_DiagnosisModel HWDiag;
    TrayAuxModel aux[BMIC_AUX_NUM];
    uint8_t usedCellNum;

}STR_ModuleModel;

typedef struct
{
	uint16_t OT:1;
	uint16_t UT:1;
}STR_AbnormalTemperature;

typedef union
{
	uint16_t all:2;
	STR_AbnormalTemperature bit;
}UNI_TemperatureStatus;

typedef struct
{
	uint16_t SHORT: 1;
	uint16_t OPEN: 1;
}STR_TempSensorStatus;
typedef union
{
	uint16_t all:2;
	STR_TempSensorStatus bit;
}UNI_TempSensorStatus;

typedef struct
{
	UNI_TempSensorStatus SensorStatus;
	UNI_TemperatureStatus Summary;
	UNI_TemperatureStatus BySW;
//	UNI_TemperatureStatus ByBMIC;
}STR_TemperatureStatusModel;


typedef struct
{
	UNI_int16Data Temperature;	// `C*100
	STR_TemperatureStatusModel Status;
    uint16_t SampleTemp[TEMP_SAMPLE_COUNT];
    uint16_t samplePos;
}STR_TempSensorModel;

typedef struct
{
	uint32_t MbmsID;
     uint8_t bmic_fault;
     uint8_t TempCnt;        // temperature count
     uint8_t BdTempCnt;      //Board temperature count
	UNI_ModeModel Mode;
	STR_VoltageSummaryModel TrayVoltage;
	STR_SpecificCellModel MinCell;
	STR_SpecificCellModel MaxCell;
	STR_TrayCellDeviation VoltageMismatch;
	STR_ModuleModel Module[SLAVEDEV_BUF_SIZE];
    STR_TempSensorModel Module_TempSensor[TMP_BUF_SIZE];
    STR_TempSensorModel Board_TempSensor[BDTMP_BUF_SIZE];   
    InterlockModel unlock;
    uint8_t maxUsedCnt;
}STR_TrayModel;




extern uint8_t tray_fault;
extern uint8_t tray_fault_num;
extern uint8_t tray_mod_fault_num;
extern STR_TrayModel Tray[TBMS_BUF_SIZE];




void Init_Tray();
void Init_Alert_Tray(uint8_t);
void Init_Module(uint8_t);
void Init_Temperature(uint8_t);

void Init_Alert_Module(uint8_t, uint16_t DeviceNum);
void Init_Cells(uint8_t, uint16_t i);
void Init_Alert_Cells(uint8_t, uint16_t DeviceNum, uint16_t CellNum);
void Init_TempSensor(uint8_t id);

void Update_Voltage_MisMatch_Check(uint8_t);

void BMIC_Read_Temperature(uint8_t);
void BMIC_ComFaultCheck(void);

void CheckCellStatusbySW(void);
void CheckEachCellStausbySW(uint8_t, uint16_t DeviceNum, uint16_t CellNum);
void TempFaultCheck_bySW();

void SaveDiagReg(uint8_t, uint16_t Varity);
void SaveBalancingWatchdog_17823Reg(uint8_t);
void CellSumBySW();

void InternalDiagnosis_Two_SetNRead(void);
void CellNTemp_AlertNStatus_Check(void);

uint16_t MakeProperBitScale(uint16_t idx,uint16_t VarityOfSource);
uint16_t CheckMaskInformation(uint8_t CellNum);
void BMIC_ReadTrayFaultFromRBMS(void);
void BMIC_ReadCellBalancingFromRBMS(void);
void BMIC_ReadModTmpFromRBMS(void);
void BMIC_ReadCellVolFromRBMS(void);
void BMIC_ReadDeviceIDFromRBMS(void);
void BMIC_ReadTrayInterLockFromRBMS(void);
void BMIC_Read_Each_Voltage(uint8_t ch, uint16_t Value, uint16_t Storage_Location );
void BMIC_Read_Block_V(uint8_t ch);
uint8_t BMIC_CommFaultCheck(uint8_t ch);
uint8_t BMIC_ScanDoneCheck(uint8_t ch);
void SaveScanCtrlReg(uint8_t ch);

void BMIC_Read_Temp(uint8_t ch);
void SaveModuleAuxData(uint8_t ch, uint8_t aux_id);
void ConvertModuleAuxData(uint8_t ch);



#endif




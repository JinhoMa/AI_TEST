/**
    ******************************************************************************
    *     @file         RACK_SYS.c
    *     @version   
    *     @author    
    *     @date      2017/02/16 
    *     @brief     
    ******************************************************************************
    */
#include "obshal.h"
#include "main.h"
#include "AppQueue.h"
#include "BMS.h"
#include "Master.h"
#include "eeprom_processdata.h"
#include "SYS_ERR_CHK.h"
//#include "RTC_CTL.h"
#include "tray_sys.h"

#include <string.h>

RACK_INI rack_ini;

typedef struct{
    uint16_t vol;
    uint8_t tray;
    uint8_t cell;
} CELLV;

typedef struct{
    int16_t tmp;
    uint8_t tray;
    uint8_t mod;
} MODTEMP;

#if ( SOH_NEWLIB_REPLACE==1)
uint8_t MSG_CELL_SOC;
uint16_t MSG_CELL_SOH;
uint16_t MSG_SOC;
uint16_t MSG_SOH;
#endif

StringRegisters R;
RBMS_COM_TRAY RbmsComTray[TBMS_BUF_SIZE];
MODTEMP MODTMP_SORT[TBMS_BUF_SIZE * TMP_BUF_SIZE];   //Module 온도 Sort
MODTEMP MODBDTMP_SORT[TBMS_BUF_SIZE * BDTMP_BUF_SIZE]; //Module내에 Board 온도 Sort
uint16_t RACK_SYSFAULT;
uint16_t SW_Reset_Command;      //Master에서 Alarm Reset Command
uint8_t trayUnlock;
//extern StringRegisters R;
int16_t AVG_BDTEMP;
uint16_t TEMP_DEV;
uint16_t BPUTEMP;
uint32_t CellVolt;
uint32_t CellVolt_dev;
int16_t ModuleTemp;
MOST MaxCell[TBMS_BUF_SIZE] ;
MOST MinCell[TBMS_BUF_SIZE] ;
MOST MaxTemp ;
MOST MinTemp;
uint32_t BatSum10mVolt;   //내부 Battery 전압,  SW SUM을 할지 AD값으로 읽어할지는 내부 결정에 따라 정한다.
uint32_t BatReal10mVolt;
uint32_t BAT_100MVOL;
int32_t BatCurr_CAN=0;  //mA단위
uint32_t Pack10mVolt;
RACK_ST Rack_St;        // Power Bank Status


void InitRackSys(void)
{
    uint32_t i,j;
	//-----------------EEprom_Control.c
    memset(&rack_ini, 0x0, sizeof(uint32_t));
    if( EEPROMData.base.ID == 0 ){
        rack_ini.idsw = 0;
    }else
        rack_ini.idsw = 1;
    PWR_LED_ON;
    RLY_CTL.type = FET_2CH;  // RELAY TYPE regist
    RLY_CTL.RELAY_MODE = 1;  // Normal Mode
	/*
    //1. Relay 초기화
    //2. Coil Current 초기화
    //3. GFD 초기화
    GFD_P_DISABLE;
    GFD_N_DISABLE;
    //4. LED 초기화
    PWR_LED_ON;

    CTL_BUFFER1_ON;
    //Watich Dog 설정     
	*/
	for(i=0;i<TRAY_NUMS;i++){
        for(j=0;j<TRAY_CELL_NUMS;j++){
            RbmsComTray[i].cellvol[j]=INIT_CELLVOLTAGE;
        }
        RbmsComTray[i].maxCell.Vol = RbmsComTray[i].minCell.Vol = INIT_CELLVOLTAGE;
        RbmsComTray[i].maxCell.num = RbmsComTray[i].minCell.num = 1;
    }
	for(i=0;i<TRAY_NUMS;i++){
        for(j=0;j<MODULE_TEMP_SENSORNUM;j++){
            RbmsComTray[i].tmp[j]=2000;
            RbmsComTray[i].brdtmp[j]=2000;
        }
    }
    MaxCell[0].value =INIT_CELLVOLTAGE;
    MinCell[0].value = INIT_CELLVOLTAGE;
    MaxTemp.value  =MinTemp.value  =2000;

}

PUBLIC void RackSys_PrintOut(void)
{
    hLogProc(LOG_RBMS, "maxV[%d] = %4.4d\n", R.StringModel.CellVMaxMod, R.StringModel.CellVMax );
    hLogProc(LOG_RBMS, "main[%d] = %4.4d\n", R.StringModel.CellVMinMod, R.StringModel.CellVMin);
}

PUBLIC void RackSys_UpdateComModel(void)
{
#ifdef _TBMS
#endif

}

PUBLIC void RackSys_UpdateStringModel(void)
{
    //uint8_t i;
    R.StringModel.CellVMax = MaxCell[0].value;
    R.StringModel.CellVMaxMod = MaxCell[0].tray;
    R.StringModel.CellVMin = MinCell[0].value;
    R.StringModel.CellVMinMod = MinCell[0].tray;
    R.StringModel.CellVAvg = CellVolt;
    R.StringModel.ModTmpMax = MaxTemp.value;
    R.StringModel.ModTmpMaxMod= MaxTemp.tray;
    R.StringModel.ModTmpMin = MinTemp.value;
    R.StringModel.ModTmpMinMod= MinTemp.tray;
    R.StringModel.ModTmpAvg = ModuleTemp;
    R.StringModel.SoC = SOC_CTL.SOC  ;
    R.StringModel.SoH = 1000 ;
    R.StringModel.A = CUR_CTL.avgCurrentA *10 ;
    R.StringModel.BatV = (uint16_t)BatSum10mVolt;   
    if( R.StringModel.alrmRstCommand ) SW_Reset_Command= 1;
    else SW_Reset_Command= 0;
   
    
}

/**
   *      @brief 이 함수는 Event 를 담당하는 부분에서 비동기적으로 호출되어  업데이트 된다.
   *          tray의 comm error 의 경우 tray와 통신 파트에서 호출된다.
   *         temp, soc, vol, cur 등은 dtc 체크 루틴에서 호출된다.
    *     @[in] eve mesa의 string event 규격에 맞게 셋된다.
    *
   * */                 
PUBLIC void RackSys_StringEvent(uint32_t evt, uint8_t on)
{
     R.StringModel.Evt1 = (on)? (R.StringModel.Evt1 | evt) :
        (R.StringModel.Evt1 & ~(evt)) ;
}

PUBLIC void RackSys_StringExtEvent(uint32_t evt, uint8_t on)
{
    R.StringModel.Evt2 = (on)? (R.StringModel.Evt2 | evt) :
       (R.StringModel.Evt2 & ~(evt)) ;
}

PUBLIC uint32_t RackSys_GetStringEvent(void)
{
    return R.StringModel.Evt1 ;
}
PUBLIC uint32_t RackSys_GetStringExtEvent(void)
{
    return R.StringModel.Evt2 ;
}

PUBLIC void RackSys_UpdateModuleData(void)
{
    int i,j,n,nz,c,d;
    MOST swap;
    uint32_t tmp=0;
    uint32_t tmp2=0;
    uint32_t vol;
    // R.MESAStringModel.CommonModel.Mn
//---- search Max Cell
//---- search Min Cell
//---- AVG  Vol    
    //Rack단위로 Max Cell값과 Min Cell값을 찾는다.
    n=0;
    for(i=0; i< TRAY_NUMS ;i++){
       //Min-Max Cell 편차 확인
        if(RbmsComTray[i].id==0) continue;
        MaxCell[i].value = RbmsComTray[i].maxCell.Vol;
        MaxCell[i].tray = i+1;
        MaxCell[i].num = RbmsComTray[i].maxCell.num;
        MinCell[i].value = RbmsComTray[i].minCell.Vol;
        MinCell[i].tray = i+1;
        MinCell[i].num = RbmsComTray[i].minCell.num;
        n++; 
        tmp += RbmsComTray[i].sum_sw_vol;
    }
    CellVolt = (uint16_t)(tmp/n/TRAY_CELL_NUMS);
    //Rack의 전체 Cell값을 Sorting한다.
    n=TRAY_NUMS;
    for (c = 0 ; c < ( n - 1 ); c++)
    {
      for (d = 0 ; d < n - c - 1; d++)
      {
        if ( MaxCell[d].value < MaxCell[d+1].value ) /* For decreasing order use  */
        {
          //swap       = CELLV_SORT[d].vol;
          memcpy(&swap, &MaxCell[d], sizeof(MOST));
          memcpy(&MaxCell[d], &MaxCell[d+1], sizeof(MOST));
          memcpy(&MaxCell[d+1], &swap, sizeof(MOST));
        }
        if ( MinCell[d].value > MinCell[d+1].value ) /* For increasing order use  */
        {
          //swap       = CELLV_SORT[d].vol;
          memcpy(&swap, &MinCell[d], sizeof(MOST));
          memcpy(&MinCell[d], &MinCell[d+1], sizeof(MOST));
          memcpy(&MinCell[d+1], &swap, sizeof(MOST));
        }
        
      }
    }
    //Cell 편차가 오차 미만이면 더 이상 진행 않는다.
    CellVolt_dev= MaxCell[0].value -MinCell[0].value;
    //dbg_print("cell dev=%d", CELLV_SORT[TRAY_NUMS*TRAY_CELL_NUMS-1].vol -CELLV_SORT[0].vol);
    // abnormal case.

//---- serach Max Temp
//---- serarch Min Temp
//---- AVG Temp
    tmp=0;
    n=0;
    for(i=0; i< TRAY_NUMS ;i++){
        if(RbmsComTray[i].id==0) 
            continue;
        for(j=0;j<MODULE_TEMP_SENSORNUM;j++){
            MODTMP_SORT[i*MODULE_TEMP_SENSORNUM+j].tmp = RbmsComTray[i].tmp[j];
            MODTMP_SORT[i*MODULE_TEMP_SENSORNUM+j].mod = j;
            MODTMP_SORT[i*MODULE_TEMP_SENSORNUM+j].tray = i;
            tmp+=RbmsComTray[i].tmp[j];
            
        }
        for(j=0;j<MODULE_BDTEMP_SENSORNUM;j++){
            MODBDTMP_SORT[i*MODULE_BDTEMP_SENSORNUM+j].tmp = RbmsComTray[i].brdtmp[j];
            MODBDTMP_SORT[i*MODULE_BDTEMP_SENSORNUM+j].mod = j;
            MODBDTMP_SORT[i*MODULE_BDTEMP_SENSORNUM+j].tray = i;
            tmp2+=RbmsComTray[i].brdtmp[j];
        }
        n++;
    }
    ModuleTemp = tmp/n/MODULE_TEMP_SENSORNUM;
    AVG_BDTEMP = tmp2/n/MODULE_TEMP_SENSORNUM;
    nz = n;   
    n= nz*MODULE_TEMP_SENSORNUM;
    for (c = 0 ; c < ( n - 1 ); c++)
    {
      for (d = 0 ; d < n - c - 1; d++)
      {
        if ( MODTMP_SORT[d].tmp < MODTMP_SORT[d+1].tmp ) /* For decreasing order use  */
        {
          //swap       = CELLV_SORT[d].vol;
          memcpy(&swap, &MODTMP_SORT[d], sizeof(MODTEMP));
          memcpy(&MODTMP_SORT[d], &MODTMP_SORT[d+1], sizeof(MODTEMP));
          memcpy(&MODTMP_SORT[d+1], &swap, sizeof(MODTEMP));
        }
      }
    }
   n= nz*MODULE_BDTEMP_SENSORNUM;
    for (c = 0 ; c < ( n - 1 ); c++)
    {
      for (d = 0 ; d < n - c - 1; d++)
      {
        if ( MODBDTMP_SORT[d].tmp < MODBDTMP_SORT[d+1].tmp ) /* For decreasing order use  */
        {
          //swap       = CELLV_SORT[d].vol;
          memcpy(&swap, &MODBDTMP_SORT[d], sizeof(MODTEMP));
          memcpy(&MODBDTMP_SORT[d], &MODBDTMP_SORT[d+1], sizeof(MODTEMP));
          memcpy(&MODBDTMP_SORT[d+1], &swap, sizeof(MODTEMP));
        }
        
      }
    }
    TEMP_DEV= MODTMP_SORT[0].tmp -MODTMP_SORT[n-1].tmp;
    MaxTemp.value = MODTMP_SORT[0].tmp;
    MaxTemp.tray =  MODTMP_SORT[0].tray;
    MaxTemp.num = MODTMP_SORT[0].mod;
    MinTemp.value = MODTMP_SORT[n-1].tmp;
    MinTemp.tray =  MODTMP_SORT[n-1].tray;
    MinTemp.num = MODTMP_SORT[n-1].mod;

    vol=0;
    for(i=0; i<TRAY_NUMS;i++){
        vol += RbmsComTray[i].sum_sw_vol;
    }
    BatSum10mVolt = vol/10;
    j =1;
    for(i=0; i<TRAY_NUMS;i++){
        j &= RbmsComTray[i].unlock ;
    }
    trayUnlock = j;
}

PUBLIC uint16_t RackSys_GetCurDiagCode(void)
{
#if 0
    int idx ;
    if(EEPROMData.diagcnt == 0) return 0;
    idx = EEPROMData.diagcnt-1;
    return EEPROMData.diagdata[idx].v;
#else
    return 0;
#endif
}

PUBLIC uint8_t RackSys_PopDiagCode(void *buf)
{
#if 0
    int idx ;
    if(EEPROMData.diagcnt == 0) return 0;
    idx = --EEPROMData.diagcnt;
    memcpy( buf, (const void *)&EEPROMData.diagdata[idx], sizeof(EEP_DIAG_DATA));
    return 1;
#else
    return 0;
#endif
}

PUBLIC void RackSys_PushDiagCode(uint16_t code)
{
#if 0
    struct RTC_Time rtc;
    int idx = EEPROMData.diagcnt;
    RTC_GetTime(&rtc);
    pushDiagDataEvent =1;
    memcpy( (void *)&EEPROMData.diagdata[idx].t, (const void *)&rtc, sizeof(struct RTC_Time));
    EEPROMData.diagdata[idx].v = code;
    if(idx == DIAG_PUSH_SIZE) 
        return;
    //FULL 나면 마지막 버퍼에서 업데이트 
    EEPROMData.diagcnt ++;
#endif
}

PUBLIC void RackSys_ClrDiagCode(void)
{
#if 0
    EEPROMData.diagcnt = 0;
#endif
}

PUBLIC uint32_t RackSys_Status(void)
{
#if 0
    uint32_t dtc_st = DTC_RBMS_Status();
    //dtc
    if ( dtc_st != eachNormal)
        return dtc_st;
    //rbms hw diag
    
    //tbms hw diag
#endif
    return 0;
}

PUBLIC void RasckSys_SetFailReason(uint16_t reasoncode)
{
   // R.StringModel.ConFail = reasoncode;
}

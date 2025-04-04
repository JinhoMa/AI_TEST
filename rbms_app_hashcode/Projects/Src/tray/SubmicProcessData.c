/**
    ******************************************************************************
    *     @file         SubmicProcessData.c
    *     @version   
    *     @author    
    *     @date      2017/10/13 
    *     @brief     
    ******************************************************************************
    */
#include "main.h"
#include "obshal.h"
#include "bms.h"
#include "BMS_OS.h"
#include <stdint.h>
#include "string.h"
#include <stdio.h>
#include "submicProcessData.h"
#include "oMbMaster.h"
#include "AppQueue.h"
//#include "GW_Interface.h"

#define CVREF_H    EEPROMData.cal.CV_HVREF
#define CVREF_L    EEPROMData.cal.CV_LVREF
#define CV_LD       EEPROMData.cal.D_LV
#define CV_HD       EEPROMData.cal.D_HV

HoldRegisters hr;
InputRegisters ir;
        
 uint8_t Cal_Msr_On = 0;
 uint8_t Cal_SelIdx = 0;

uint16_t AD_BVOL[CAL_BVC_BUFSIZE];   //Bat  VOL 측정 전압
uint16_t AD_PVOL[CAL_PVC_BUFSIZE];   //PCS VOL 측정 전압
uint32_t AD_CCur[CAL_CUR_BUFSIZE];    //전류 측정 전압 
uint32_t AD_DCur[CAL_CUR_BUFSIZE];    //전류 측정 전압 
uint16_t CellV[8];

float test_bat_vol;
float test_pcs_vol;
float test_current;

uint32_t Modbus_Ch1_Rcv_Cnt;
uint32_t Modbus_Ch1_RcvErr_Cnt;
int32_t hr_prev_cmd;

float bat_delta_lo;
float bat_delta_hi;
float pcs_delta_lo;
float pcs_delta_hi;
float ccur_delta_hi;
float ccur_delta_lo;
float dcur_delta_hi;
float dcur_delta_lo;
float gfdp_delta_hi;
float gfdp_delta_lo;
float gfdn_delta_hi;
float gfdn_delta_lo;

extern uint8_t bmic_balsw_on;
extern uint8_t bmic_blowoff_fuse;


extern void PACK_V_Update(float vol);
extern void PCS_V_Update(float vol);
extern void CUR_Update(float c);
extern void GFDN_VOLUpdate(float g);
extern void GFDP_VOLUpdate(float g);

extern int32_t mb_fd1;
__WEAK void CUR_Update(float c)
{
}

__WEAK void PACK_V_Update(float v){

}

#if ( P100_8S == 1)   
static void processVol(void)
{
    uint16_t tad = ir.model.ad_packvol;
    uint16_t D_LPV ;
    uint16_t ref_0; // ref_1; // ref_2
    int i;
    ref_0 = EEPROMData.cal.BREFV[0];
   // ref_1 = EEPROMData.cal.BREFV[1];
   // ref_2 = EEPROMData.cal.BREFV[2];
    for(i=0;i < CAL_BVC_BUFSIZE; i++){
        if( tad <= AD_BVOL[i] || (AD_BVOL[i]==0) ){
            break;
        }
    }
    if(i==0){ //
        if(ref_0==0){
            test_bat_vol = 0;
        }else{
            D_LPV = AD_BVOL[0];
            test_bat_vol = (tad-D_LPV)*bat_delta_lo+ ref_0;       
        }
    }else{
        D_LPV = AD_BVOL[0];
        test_bat_vol = (tad-D_LPV)*bat_delta_lo + ref_0;        
    }

    tad = ir.model.ad_packvol;
    ref_0 = EEPROMData.cal.PREFV[0];
   // ref_1 = EEPROMData.cal.PVOL[1];
   // ref_2 = EEPROMData.cal.BREFV[2];
    for(i=0;i < CAL_BVC_BUFSIZE; i++){
        if( tad <= AD_PVOL[i] || (AD_PVOL[i]==0) ){
            break;
        }
    }
    if(i==0){ // 
        if(ref_0==0){
            test_pcs_vol = 0;
        }else{
            D_LPV = AD_PVOL[0];
            test_pcs_vol = (tad-D_LPV)*pcs_delta_lo+ ref_0;       
        }
    }else{
        D_LPV = AD_PVOL[0];
        test_pcs_vol = (tad-D_LPV)*pcs_delta_lo + ref_0;        
    }
    
    
    PACK_V_Update(test_bat_vol);
}

static void processCurrent(void)
{
    int32_t tad =  ir.model.ad_cur;
    int32_t D_LC ;
    //int i;
    uint16_t ref_0; //ref_1, ref_2;
    ref_0 = EEPROMData.cal.REFC[0];
    //ref_1 = EEPROMData.cal.REFC[1];
    //ref_2 = EEPROMData.cal.REFC[2];
    if(tad < AD_CCur[0]){
        goto ChargerCal;
    }
    D_LC = AD_DCur[0] ;
    test_current = (D_LC-tad)*dcur_delta_lo - ref_0 ;        
    CUR_Update(test_current);
    return;
 ChargerCal:
    D_LC = AD_CCur[0];
    if(tad == 0)  tad = AD_CCur[0];
    test_current = (tad-D_LC)*ccur_delta_lo + ref_0 ;       
    CUR_Update(test_current);
    
 }
#if 0
static void processCV(void)
{
    int cid;
    uint16_t tad, ttmp2, ttmp3;
    int32_t ttmp1;
    for(cid=0;cid< 8; cid++)
    {
        tad = ir.model.cell[cid];
	// Voltage = (AD - VL) * (VrefH - VrefL) / (VH - VL) + VrefL
	// *:Vx = Calibration AD value, Vrefx = Calibration reference voltage value
		ttmp1 = ((long)tad - CV_LD[cid] ) * ( CVREF_H - CVREF_L );
		ttmp2 = CV_HD[cid] - CV_LD[cid];
		ttmp3 = (uint16_t)( ttmp1 % ttmp2 );
		ttmp1 = ttmp1 / ttmp2;
		if( ttmp1 < 0 && ttmp3 != 0 )
		{
			ttmp1 -= 1;
		}
		CellV[cid] = (ttmp1 + CVREF_L );
    }
}
#endif

#endif


float VPs_Mul100;
float VPpre_Mul100;
float VNs_Mul100;
float VNpre_Mul100;

void Submic_ClrRegisters(void)
{
    int i;
    int32_t temp;
    float REF_H, REF_L;
    memset(&hr, 0x0, sizeof(HoldRegisters));
    memset(&ir, 0x0, sizeof(InputRegisters));
    for(i=0;i<3;i++){
        AD_BVOL[i]=EEPROMData.cal.BVOL[i];
        AD_CCur[i]=EEPROMData.cal.CCUR[i];
        AD_DCur[i]=EEPROMData.cal.DCUR[i];
        AD_PVOL[i]=EEPROMData.cal.PVOL[i];
    }
    
    REF_H = EEPROMData.cal.BREFV[1];
    REF_L = EEPROMData.cal.BREFV[0];
    temp = AD_BVOL[1]-AD_BVOL[0];
    if(!temp) {
        bat_delta_lo =0;
    }else{
        bat_delta_lo = (REF_H-REF_L)/(float)temp;
    }
    REF_H = EEPROMData.cal.BREFV[2];
    REF_L = EEPROMData.cal.BREFV[1];
    temp = AD_BVOL[2]-AD_BVOL[1];
    if(!temp || AD_BVOL[2] == 0) {
        bat_delta_hi =0;
    }else{
        bat_delta_hi = (REF_H-REF_L)/(float)temp;
    }
    REF_H = EEPROMData.cal.PREFV[1];
    REF_L = EEPROMData.cal.PREFV[0];
    temp = AD_PVOL[1]-AD_PVOL[0];
    if(!temp) {
        pcs_delta_lo =0;
    }else{
        pcs_delta_lo = (REF_H-REF_L)/(float)temp;
    }
    REF_H = EEPROMData.cal.PREFV[2];
    REF_L = EEPROMData.cal.PREFV[1];
    temp = AD_PVOL[2]-AD_PVOL[1];
    if(!temp || AD_PVOL[2] == 0) {
        pcs_delta_hi =0;
    }else{
        pcs_delta_hi = (REF_H-REF_L)/(float)temp;
    }

    REF_H = EEPROMData.cal.REFC[1];
    REF_L = EEPROMData.cal.REFC[0];
    temp = AD_CCur[1]-AD_CCur[0];
    if(!temp) {
        ccur_delta_lo =0;
    }else{
        ccur_delta_lo = (REF_H-REF_L)/(float)temp;
    }
    REF_H = EEPROMData.cal.REFC[2];
    REF_L = EEPROMData.cal.REFC[1];
    temp = AD_CCur[2]-AD_CCur[1];
    if(!temp || AD_CCur[2] == 0) {
        ccur_delta_hi =0;
    }else{
        ccur_delta_hi = (REF_H-REF_L)/(float)temp;
    }

    REF_H = EEPROMData.cal.REFC[1];
    REF_L = EEPROMData.cal.REFC[0];
    temp = AD_DCur[1]-AD_DCur[0];
    if(!temp) {
        dcur_delta_lo =0;
    }else{
        dcur_delta_lo = (REF_H-REF_L)/(float)temp;
    }
    REF_H = EEPROMData.cal.REFC[2];
    REF_L = EEPROMData.cal.REFC[1];
    temp = AD_DCur[2]-AD_DCur[1];
    if(!temp || AD_DCur[2] == 0) {
        dcur_delta_hi =0;
    }else{
        dcur_delta_hi = (REF_H-REF_L)/(float)temp;
    }
   
}

PUBLIC void Submic_initDevice(uint8_t ch)
{
    int32_t st;
    //uint16_t ver;
    {
        hr.model.Cfet = 1;
        hr.model.Dfet = 1;
        hr.model.cbal_on = (bmic_balsw_on)? 1: 2;
        hr.model.fuse_off = 2;
    }
    if( ch == 0){
        st = ir.model.mode;
        if(st == ST_RESET){
            //device가 ready가 될때 까지 기다린다.
            PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_INIT_DEVICE, SUBMIC_EVT_TIME, NULL ); 
        }else if(st == ST_RUN){
            hr.model.cmd = C_RUN,
            // run 중에 있을때는 sw Reset 을 시킨다. 
            PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_POLLING, SUBMIC_EVT_TIME, NULL ); 
        }else {
            hr.model.cmd = C_RESET,
            // run 중에 있을때는 sw Reset 을 시킨다. 
            oMbMasterWriteSingleReg(mb_fd1, 1, HOLD_ADDR_CMD , hr.model.cmd, -1);
            PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_INIT_DEVICE, SUBMIC_EVT_TIME, NULL ); 
        }
    }
}

PUBLIC uint8_t Submic_GetData(uint8_t ch)
{
    if( ch == 0){
       if( oMbMasterReadInputRegs( mb_fd1, 1, 0, INPUTREG_SIZE, (uint16_t *)&ir) > 0){
        Modbus_Ch1_Rcv_Cnt++;
        return TRUE;
       }{
        Modbus_Ch1_RcvErr_Cnt++;
        return FALSE;
       }
    }
    return FALSE;
}



PUBLIC void Submic_Reset(uint8_t ch)
{
    if( ch == 0) {
        PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_INIT_VAR, SUBMIC_EVT_TIME, NULL ); 
    }
}
PUBLIC void Submic_Send_PowerDownCmd(void)
{
    hr.model.cmd =hr_prev_cmd = C_PDWN;
    oMbMasterWriteSingleReg( mb_fd1, 1, HOLD_ADDR_CMD, hr.model.cmd, -1);    
}

PUBLIC void Submic_Send_FuseOffCmd(void)
{
    hr.model.fuse_off = 1 ;
    oMbMasterWriteSingleReg( mb_fd1, 1, HOLD_ADDR_CMD, hr.model.cmd, -1);    
}

PUBLIC void Submic_Send_CellBalCmd(void)
{
    if(bmic_balsw_on == 1){
        hr.model.cbal_on = 1;
    }else{
        hr.model.cbal_on = 2;
    }
    oMbMasterWriteSingleReg( mb_fd1, 1, HOLD_ADDR_CMD, hr.model.cmd, -1);    
}

PUBLIC void Submic_Polling(void)
{
    static uint64_t t=0 ;
    int32_t st = ir.model.mode;
    static int abnormal_cnt=0;
    if( st == ST_RESET)
    {
        //RENESAS가 Command 수신후 처리가 늦거나. 내부 문제로 STATE가 바뀐경우 
        printf("abnormal case(%d) \n",st);
        if(abnormal_cnt++ > 3){
            abnormal_cnt =0;
            Submic_Reset(0);
            return;
        }
    }else 
    if( st == ST_RUN )
    {
        if ( hTimCurrentTime() -t > 200){
            t= hTimCurrentTime();
            //printf("poll.\n");
            hLedBlink(LED1);
        }
    
        abnormal_cnt =0;
        if(hr_prev_cmd != hr.model.cmd){
            hr_prev_cmd = hr.model.cmd;
        }else{
            //processCV();
            //processVol();
            //processGFD();
            processVol();
            processCurrent();
        }
        oMbMasterWriteMultiRegs( mb_fd1, 1, HOLD_ADDR_CMD,(uint8_t *)hr.buffer, sizeof(hr),-1);
    }
    PushJob(SUBMIC_SCHEDULER_IDX, SUBMIC_POLLING, SUBMIC_EVT_TIME, NULL ); 

}


void Submic_ReadSoftwareVersion(uint8_t* b1, uint8_t size1, uint8_t* b2, uint8_t size2)
{
    if(size1 < 3 || size2<3 ) return;
    b1[0] = (uint8_t)((ir.model.version>>12)&0xF); 
    b1[1] = (uint8_t)((ir.model.version>>8)&0xF);
    b1[2] = (uint8_t)((ir.model.version>>4)&0xF);

}



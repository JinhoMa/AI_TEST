/**
 ******************************************************************************
 *     @file         Soc.c
 *     @version   
 *     @author    
 *     @date      2024/09/19 
 *     @brief     
 ******************************************************************************
*/
#include "obshal.h"
#include "main.h"
#include "BMS.h"
#include "master.h"
#include <string.h>
_SOC_CTL SOC_CTL;
uint16_t Soc_Tbl[SOC_TBL_SIZE];
uint16_t cal_soc, prv_soc, rec_soc ; //factor 0.1
uint64_t prv_tag_time;
uint64_t df_update_time;
uint64_t ocv_time;
int32_t total_rc;   //100mA*sec
int32_t total_eng;
int32_t soh_rc;
int32_t now_rc;
UNI_REC_D   RTRecDF;  // RealTime Record DataFormat
void CanSocSimulationHandler(int fd, uint8_t *data)
{

}
//1초 마다 용량 계산
// RC 가 100mA 단위 이므로 . mA 값을 100으로 나눈다.
void CAL_RC(void)
{
    float delta_rc;
    int32_t rc_5percent = soh_rc*5/100;
    delta_rc = CUR_CTL.avgCurrentA*10.0f;
    if( now_rc < rc_5percent )
    {
        now_rc +=  ( delta_rc /2 );
        if(now_rc <= 0)
        {
            now_rc = 0;
        }
    }else{
        now_rc += delta_rc;
        now_rc = ( now_rc > soh_rc) ? soh_rc : now_rc;
    }
}

static void PackState_ComCheck(void)
{
    if( CUR_CTL.avgCurrentA == 0)
    {
        f_chg = 0; f_dis = 0;
    }else if( CUR_CTL.avgCurrentA > 0)
    {
        f_chg = 1;
        f_ocv = 0;
        ocv_time = 0;
        f_fulldis = 0;

    }else{ // 방전
        f_dis = 1;
        f_ocv = 0;
        f_fullchg = 0;
        ocv_time = 0;
    }
    {
        RTRecDF.str.time =(uint32_t)hTimCurrentTime();
        RTRecDF.str.max_temp = R.StringModel.ModTmpMax;
        RTRecDF.str.min_temp = R.StringModel.ModTmpMin;
        RTRecDF.str.max_cv = R.StringModel.CellVMax;
        RTRecDF.str.min_cv = R.StringModel.CellVMin;
        RTRecDF.str.pv = R.StringModel.BatV;
        RTRecDF.str.rc = now_rc;
        RTRecDF.str.evt1 = R.StringModel.Evt1;
        RTRecDF.str.evt2 = R.StringModel.Evt2;    
        RTRecDF.str.rs = Rack_St;
    }
}
__WEAK  PUBLIC void RackSt_SaveCurrentStAtOnce( uint32_t u){
    switch(u){
        case F_FULLC:
            f_fullchg = 1;
            now_rc = soh_rc ;
            cal_soc =100;
            break;
        case F_FULLD:
            f_fulldis = 1;
            now_rc = 0 ;
            cal_soc = 0;
            break;
        case F_RCCHG:
            total_rc = EEPROMData.soccnf.total_rc;
            soh_rc = total_rc * EEPROMData.recdf.SOH / 100 ;
            now_rc = total_rc*cal_soc/100;    
            break;
    }
    PackState_ComCheck();
    df_update_time = 0;
    hLogProc(LOG_SOC,"Full Soc(%d)\n",cal_soc);
    EEPROMData.recdf.flag = 1;
    PushJob( EEPROM_SCHEDULER_IDX, UPDATE_USEDATA_SEC, 0, 0 );
}

__WEAK  PUBLIC void RackSt_UpdateStAndPeriodSave( uint32_t u){
    uint64_t cur_time;
    PackState_ComCheck();
    cur_time = hTimCurrentTime();
    if(prv_tag_time == 0){
        prv_tag_time = cur_time;
    }
    if(df_update_time == 0){
        df_update_time = cur_time; //  다음 업데이트 계산 시간
    }
    if( ocv_time == 0){
        ocv_time = cur_time; // 다음 업데이트 계산 시간
    }
    if( (int32_t)(cur_time- prv_tag_time ) > 1000  ){ //1sec
        prv_tag_time = cur_time; // 다음 계산 시간.
        CAL_RC();
    }
    if(f_ocv == 0)
    {
        if( (int32_t)(cur_time - ocv_time) >= 25*60000 ) // 25분
        {
            ocv_time = cur_time;
            f_ocv = 1;
        }
    }
    if( (int32_t)(cur_time- df_update_time) > 60000 ){  // 10분다.
        df_update_time = 0;
        if( rec_soc != cal_soc){
            rec_soc = cal_soc;
            // SOC 가 변경 된 경우만 업데이트 하도록 한다.
            EEPROMData.recdf.flag = 1;
            PushJob( EEPROM_SCHEDULER_IDX, UPDATE_USEDATA_SEC, 0, 0 );
        }
    }
}

/*****************************************************************
 * EEPROM 에 마지막 Save 된 데이터를 읽어 오는데
 * 사용자가 셀 용량 설정을 업데이트 하거나, Save 된 데이터가 오류가 읽는경우
 * 처음부터 새롭게 읽도록 구현해야 한다.
*****************************************************************/ 
static void InitRc(void)
{
    uint8_t idx = EEPROMData.recdf.last_idx;
    total_rc = EEPROMData.soccnf.total_rc;
    soh_rc = total_rc * EEPROMData.recdf.SOH / 100 ;
    memcpy(&RTRecDF, &EEPROMData.recdf.sector[idx], sizeof(UNI_REC_D));
    now_rc = RTRecDF.str.rc;    
}
static uint16_t getSOC(void){
    uint16_t y1, y2, y0,i;
    uint16_t x0;
    y0 = CellVolt;
    for(i=0;i<SOC_TBL_SIZE;i++){
      if( y0  <= Soc_Tbl[i]){
        break;
      }
    }
    if(i== 0){
        cal_soc = 0;
        EEPROMData.recdf.SOC = 0;
        return 0;
    }
    y1 = Soc_Tbl[i-1];
    y2 = Soc_Tbl[i];
    x0 = (uint32_t)((y0-y1)*5)/(y2-y1)+(i-1)*5;
    return x0;
}

void InitSOC(void)
{
    //EEPROM에 저장된 SOC 가 없으면( 0으로 기록) OCV 로 가정하고 읽어온다.
   int i;
   //EEPROM 에 저장된 SOC 를 읽어 온다.
   InitRc();
   memcpy(Soc_Tbl, &EEPROMData.soccnf, sizeof(EEP_SOC_CONF) );
   // SOC가 업데이트 되었다면 읽어온다.
   if( EEPROMData.recdf.flag == 1 && EEPROMData.recdf.SOC != 0 ){
       cal_soc =  EEPROMData.recdf.SOC;
       now_rc  = soh_rc * cal_soc / 100; 
   }else{
       cal_soc =  getSOC();
       //EEPROM에 저장된 RC 값
       if(now_rc == 0){
           now_rc  = soh_rc * cal_soc / 100; 
       }
       EEPROMData.recdf.SOC = cal_soc;
   }
   hLogProc(LOG_SOC,"init Soc(%d)\n",cal_soc);
}

//LFP 는 완충인경우 RECAL 을 하도록 한다.
__WEAK  PUBLIC void CAL_ReCalSOC(void)
{
    // Recalibration 구현 필요.
    EEPROMData.recdf.SOC = cal_soc;  //업데이트 한다.
}

__WEAK PUBLIC void CAL_SOC(void){
    uint16_t t;
    if( SOC_CTL.INIT_SOC_FLAG == FALSE){
        InitSOC();
        SOC_CTL.INIT_SOC_FLAG = TRUE;
        SOC_CTL.SOC = cal_soc*10;
        SOC_CTL.SOH = (EEPROMData.recdf.SOH)? (EEPROMData.recdf.SOH)*10 : 1000;
        rec_soc = prv_soc = cal_soc;
        if(EEPROMData.recdf.flag == 0){
            
        }
        return;
    }else{
        if(f_ocv == 1 ){
            SOC_CTL.OCV= 1;
            t = getSOC();
            if( cal_soc > t){
                if((cal_soc-t) > 10){
                    cal_soc = t;
                    SOC_CTL.SOC = cal_soc * 10;
                    now_rc =soh_rc*cal_soc/100;     
                    CAL_ReCalSOC();
                }
            }else{
                if((t-cal_soc) > 10){
                    cal_soc = t;
                    SOC_CTL.SOC = cal_soc * 10;
                    now_rc =soh_rc*cal_soc/100;     
                    CAL_ReCalSOC();
                }
            }   
        }else{
            SOC_CTL.OCV= 0;
            cal_soc = (uint32_t )( now_rc*100/soh_rc);
            SOC_CTL.SOC = cal_soc * 10;
            if(prv_soc != cal_soc){
                prv_soc = cal_soc;
                EEPROMData.recdf.SOC = cal_soc;  //업데이트 한다.
            }
        }
        hLogProc(LOG_SOC,"Soc(%4f)\n",cal_soc);
    }
}

PUBLIC void CAL_SOH(void){
}

/**
 ******************************************************************************
 *     @file         CUR_PACKV.c
 *     @version   
 *     @author    
 *     @date      2024/09/19 
 *     @brief     
 ******************************************************************************
*/
#include "obshal.h"
#include "bms.h"
#include "rack_sys.h"
#include "master.h"
#include "Can_DiagCom.h"


float currentPack[CUR_AVG_CNT];
float pvol;
uint8_t cur_samplepos ;
static uint8_t slotFull = 0;

_CUR_CTL CUR_CTL;
_PACK_V PACK_V;

static float average(float values[], uint8_t n){
  int i;
  float sum = 0L;
  for (i = 0; i < n; i++){
    sum += values[i];
  }
  return sum / n;
}

/**********************************************************************
 *      @brief 최종 Pack 전압을 계산해서 주는 곳이다. 테스트시는 Cell 전압 SUM 인 BatVolt 으로 준다.
**********************************************************************/
static uint32_t caliberationPackVoltage(float real_vol)
{
    BatReal10mVolt = real_vol;
#if 1
    return BatReal10mVolt;
#else
   return BatSum10mVolt;
#endif
}
void processPackV(void)
{

}
/**********************************************************************
 *      @brief  이 함수는 RACKBMS code 가 링크 될때 Override 된다.
**********************************************************************/
PUBLIC void PACK_V_Update(float v)
{
    static float max = 0x0f; 
    static float min = 0x0f; 
    static float cur = 0x0f;
    pvol = v;

    if (pvol > max){
        max = pvol;
    }

    if (pvol < min){
        min = pvol;
    }

    if (PACK_V.Pack_V_Sense_First)
    {
        float devMax, devMin;
        float half = cur / 2;
        devMax = cur + half;
        devMin = cur - half;
        if (pvol < devMin || pvol > devMax)
        {
            cur = pvol;
            return;
        }
    }
    cur = pvol*100;
    PACK_V.AVG_TEMP_BUF += cur;
    PACK_V.AVG_CHK_CNT++;
    if((uint16_t)PACK_V_AVG_CNT == PACK_V.AVG_CHK_CNT)
    {
        hLogProc(LOG_VOL, "min = %8.5f, cur = %8.5f, max = %8.5f\n", min, cur, max);
        min = max = cur;
        PACK_V.AVG_CHK_CNT = 0;
        PACK_V.PACK_V  = (PACK_V.AVG_TEMP_BUF / PACK_V_AVG_CNT);
        PACK_V.AVG_TEMP_BUF = 0;
        Pack10mVolt = caliberationPackVoltage(PACK_V.PACK_V);
        R.StringModel.V = Pack10mVolt/10;
        if(PACK_V.Pack_V_Sense_First==0)
        {
            PACK_V.Pack_V_Sense_First = 1;
        }
        else
        {
        }
    }
    
}
PUBLIC void Get_PackVoltage(void)
{
    Pack10mVolt = caliberationPackVoltage(PACK_V.PACK_V);
    R.StringModel.V = Pack10mVolt/10;
}


/**********************************************************************
 *      @brief  이 함수는 RACKBMS code 가 링크 될때 Override 된다.
**********************************************************************/
PUBLIC void CUR_Update(float c)
{
    float avgCurrentA;
    currentPack[cur_samplepos] = c;
    cur_samplepos = cur_samplepos + 1;
    if (slotFull){
      avgCurrentA = average(currentPack,  CUR_AVG_CNT );
    }else {
      if (cur_samplepos == CUR_AVG_CNT){
        slotFull = 1;
      }
      avgCurrentA = average(currentPack,  cur_samplepos );
    }
    cur_samplepos = cur_samplepos % CUR_AVG_CNT;
    if(avgCurrentA < 0.1F && avgCurrentA > -0.1F ){
        CUR_CTL.avgCurrentA  =0;
    }else{
        CUR_CTL.avgCurrentA = avgCurrentA;
    }
    if( CanGW.MBMSMode == DIAG_MODE  && CanGW.Simul.cur_sim == 1 )
    {
        CUR_CTL.avgCurrentA = CanGW.Simul.sim_data/1000;
    }
    BatCurr_CAN = CUR_CTL.avgCurrentA*1000;

}

PUBLIC void Get_Current(void){
  R.StringModel.A  = CUR_CTL.avgCurrentA * 10;
}

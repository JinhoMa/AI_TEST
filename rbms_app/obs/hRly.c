/**
    ******************************************************************************
    *     @file         hRly.c
    *     @version   
    *     @author    
    *     @date      2017/03/12 
    *     @brief     E850에서 RELAY 구동 방식 LEVEL 신호에서 Pulse 로 동작하게 된다. 
    *             Pulse 신호는 TC7WH123FK 에 5번 PIN Q 신호의 Output Level 신호를 연장시키는 역할은 한다.
    *            Out Pulse 시간값 (twOUT = 1.0 * R * C ) 이고  100K * 1UF = 0.1 초 정도가 된다.
    *          1.41 초내 Pulse가 한번 더 발생하는 경우 Output Level 시간은 그 만큼 연장이 된다.
    *          12/20일  C값: 2.2uF으로 변경. = 0.22초 가 된다. 
    *           기존 주기를 50ms 에서 100ms 로 변경하도록 한다.
    ******************************************************************************
    */

#include "obshal.h"
#include "main.h"

#define HALF_PERIOD_TIME    50
typedef struct _BHRly{
    uint8_t use;
    uint8_t on;
    uint8_t high;
   // int8_t tick10ms;
}BHRly;

BHRly rlyHandle[3];
int32_t rlytid;
/**
   *  LOW->ON->59MS(H)->50MS(L)->OFF->LOW 
   * period = 100ms
   */
void rly_50ms_cbfunc(void)
{
    if(rlyHandle[0].use)
    {
        if(rlyHandle[0].on ==1)
        {
            if(!rlyHandle[0].high){
                HAL_GPIO_WritePin( MARLY_P_GPIO_PORT, MARLY_P_PIN, GPIO_PIN_SET );//GPIO LEVEL HIGH
                rlyHandle[0].high=1;
            }else{ 
                HAL_GPIO_WritePin( MARLY_P_GPIO_PORT, MARLY_P_PIN, GPIO_PIN_RESET );//GPIO LEVEL LOW
                rlyHandle[0].high=0;
            }
        }
    }
    if(rlyHandle[1].use)
    {
        if(rlyHandle[1].on ==1)
        {
            if( !rlyHandle[1].high){
                HAL_GPIO_WritePin( PRERLY_GPIO_PORT, PRERLY_PIN, GPIO_PIN_SET );  //GPIO LEVEL HIGH
                rlyHandle[1].high=1;

            }else{
                HAL_GPIO_WritePin( PRERLY_GPIO_PORT, PRERLY_PIN, GPIO_PIN_RESET ); //GPIO LEVEL LOW
                rlyHandle[1].high=0;

            }
        }
    }
    if(rlyHandle[2].use)
    {
        if(rlyHandle[2].on ==1)
        {
            if(!rlyHandle[2].high){
                HAL_GPIO_WritePin( MARLY_N_GPIO_PORT, MARLY_N_PIN, GPIO_PIN_SET );//GPIO LEVEL HIGH
                rlyHandle[2].high=1;
            }else{
                HAL_GPIO_WritePin( MARLY_N_GPIO_PORT, MARLY_N_PIN, GPIO_PIN_RESET );//GPIO LEVEL LOW
                rlyHandle[2].high=0;
            }
        }
    }
}

void hRlyInit(void)
{
}

void hRlyOpen(void)
{
    HAL_RLY_Init();
    rlytid = hTimCreate();
    hTimSet(rlytid, HALF_PERIOD_TIME, rly_50ms_cbfunc);   
    rlyHandle[0].use = rlyHandle[1].use =rlyHandle[2].use =1;
    rlyHandle[0].on = rlyHandle[1].on =rlyHandle[2].on =0;
    hTimStart(rlytid);
}
/**
   *    @brief Relay를 제어한다.
   *   @[in] ch   1-MAIN P relay, 2- PRE relay, 3-MAIN N Relay
   *    @[in]  on - 1: ON,  0: OFF
   **/
void hRlyCtrl(uint8_t ch, uint8_t on)
{
    if(ch == 1){
        if(on) { rlyHandle[0].on = 1;  }
        else { rlyHandle[0].on = 0; }
    }else if(ch==2){
        if(on) { rlyHandle[1].on = 1;  }
        else { rlyHandle[1].on = 0; }
    }else if(ch==3){
        if(on) { rlyHandle[2].on = 1;  }
        else { rlyHandle[2].on = 0; }
    }
}

void hRlyClose(void)
{
    hTimClose(rlytid);
}

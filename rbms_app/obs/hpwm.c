/**
    ******************************************************************************
    *     @file      hpwm.c 
    *     @author    
    *     @version   
    *     @date      2015.10.13 
    *     @brief     
    ******************************************************************************
    *       Copyright(C) 2014~2015  OBSKorea Corp. All rights reserved.
    *       DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER 
    *       Use is subject to license terms
    ******************************************************************************
    */
#include "obshal.h"
#include "main.h"
#include <string.h>

/* Timer handler declariation*/
TIM_HandleTypeDef   PwmTimHandle;

typedef struct _BHPwm{
    TIM_HandleTypeDef handle;
    int32_t duty;
    int32_t freq;
    uint32_t ch;
    uint8_t use;
    int8_t rw ; //1 //1: write mode, 0: read mode
}BHPwm;

#define DEFAULT_DUTY_VALUE      5000
#define PWM_CHANNEL_SIZE    3
#define PWM_OUT_CH1      0
#define PWM_OUT_CH2      1
#define PWM_OUT_CH3      2

static BHPwm  pwm_ch[PWM_CHANNEL_SIZE];

extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);

void hPwmInit(void)
{
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef    sConfigOC;
    int msPrescalerValue;

    //1Hz
    /* Compute the prescaler value to have TIM3 counter clock equal to 1KHz(1ms Period) */
    //Hz = (SystemCoreClock)/2 /(Preload+1)*(Prescaler+1)
    //Prescaler = (SystemCoreClock)/2 /( Hz* (Prescaler+1) - 1;
    // = (102000)/(1000) -1 ;

    // 1초  설정 
    msPrescalerValue                    = (uint32_t)((SystemCoreClock /2) / 1000000) -1 ;
    PwmTimHandle.Instance               = TIM4;
    PwmTimHandle.Init.Period            = 10000-1;
    PwmTimHandle.Init.Prescaler         = msPrescalerValue*100;
    PwmTimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    PwmTimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    PwmTimHandle.Init.RepetitionCounter = 0;
    if (HAL_TIM_PWM_Init(&PwmTimHandle) != HAL_OK)
    {
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&PwmTimHandle, &sMasterConfig) ;
    
    sConfigOC.OCMode= TIM_OCMODE_PWM1 ;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH ;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE ;
    HAL_TIM_PWM_ConfigChannel(&PwmTimHandle, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&PwmTimHandle, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&PwmTimHandle, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_MspPostInit(&PwmTimHandle);
}


int32_t hPwmOpen(char *port )
{
    int out=0;
    uint32_t ch_num=0;
    uint8_t idx;
    if(strncmp (port, "PWO1",4)==0){
        idx = PWM_OUT_CH1; out=1; ch_num =TIM_CHANNEL_1;
    }else if(strncmp(port,"PWO2",4) == 0){
        idx =PWM_OUT_CH2; out=1; ch_num =TIM_CHANNEL_2;
    }else if(strncmp(port,"PWO3",4) == 0){
        idx =PWM_OUT_CH3; out=1; ch_num =TIM_CHANNEL_3;
    }else{
        return E_ERROR;
    }    

    memcpy(&pwm_ch[idx].handle, (void *)&PwmTimHandle, sizeof(TIM_HandleTypeDef));
    pwm_ch[idx].use =1;
    pwm_ch[idx].rw = out;
    pwm_ch[idx].ch =ch_num;
    
    return (int32_t)&pwm_ch[idx];
}

int32_t  hPwmCtrl(int32_t fd, int32_t opt, int32_t param)
{
    int32_t err = -1;
    BHPwm *pwm = (BHPwm*)fd;
    TIM_OC_InitTypeDef    sConfigOC;
    switch(opt)
    {
        case PWM_DUTYSET:
            err = 0;
            sConfigOC.OCMode= TIM_OCMODE_PWM1 ;
            sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH ;
            sConfigOC.OCFastMode = TIM_OCFAST_DISABLE ;
            sConfigOC.Pulse =  param*(pwm->handle.Init.Period+1) /10000;
            HAL_TIM_PWM_ConfigChannel(&PwmTimHandle, &sConfigOC, pwm->ch);
            break;
        case PWM_DUTYUPDATE:
            err = E_NOTSUP;
            break;
    }
    return err;
}

int32_t hPwmStart(int32_t fd)
{
    BHPwm *pwm = (BHPwm*)fd;
    HAL_TIM_PWM_Start(( TIM_HandleTypeDef*)&pwm->handle, pwm->ch);
    //HAL_TIM_PWM_Start_IT((TIM_HandleTypeDef*)&pwm->handle, pwm->ch);
    return 0;
}

int32_t hPwmRead(int32_t fd)
{
    return E_NOTSUP;
}

int32_t hPwmStop(int32_t fd)
{
    BHPwm *pwm = (BHPwm*)fd;
    HAL_TIM_PWM_Stop((TIM_HandleTypeDef*)&pwm->handle, pwm->ch);
    //HAL_TIM_PWM_Stop_IT((TIM_HandleTypeDef*)&pwm->handle, pwm->ch);
    return 0;
}

int32_t hPwmGetHandle(char *port)
{
    int32_t idx;
    if(strncmp (port, "PWO1",4)==0){
        idx = 0;
    }else if(strncmp(port,"PWO2",4) == 0){
        idx =1;
    }else if(strncmp(port,"PWO3",4) == 0){
        idx =2;
    }else{
        return E_ERROR;
    }    
    return (int32_t)&pwm_ch[idx];
}

void hPwmClose(int32_t fd)
{
    BHPwm *pwm = (BHPwm*)fd;
    hPwmStop(fd);
    if(pwm->rw){
        HAL_TIM_PWM_DeInit(&pwm->handle);
    }else{
  //      HAL_TIM_IC_DeInit(&pwm->handle);    
    }
    memset(pwm,0x0,sizeof(BHPwm));
}

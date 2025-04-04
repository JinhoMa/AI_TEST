/**
    ******************************************************************************
    *     @file         hExt.c
    *     @version   
    *     @author    
    *     @date      2017/03/12 
    *     @brief     
    ******************************************************************************
    */
#include "obshal.h"
#include "main.h"

void hExtOpen(void)
{
    HAL_EXT_Init();
}

void hExtEnable(uint8_t ch)
{
}

void hExtDisable(uint8_t ch)
{
}

uint8_t hExtGetState(uint8_t ch) 
{
  uint8_t hi = 0;
  switch(ch){
    case HEXT_4:    //RELAY_PWR_SW
    hi = (int)HAL_GPIO_ReadPin(RLY_PWR_SW_GPIO_PORT, RLY_PWR_SW_PIN );
    break;
  }
  return hi;
}

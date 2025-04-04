/**
    ******************************************************************************
    *     @file         hled.c
    *     @version   
    *     @author    
    *     @date      2017/03/28 
    *     @brief     
    ******************************************************************************
    */
#include "obshal.h"
#include "main.h"

uint8_t ledPin[LEDn];

int32_t hLedOpen(void)
{
    return  0;
}

void hLedOn(int16_t led)
{
    switch(led)
    {
        case LED1:  //power
            ledPin[0]=GPIO_PIN_SET;
            HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, (GPIO_PinState)ledPin[0]);
            break;
/*            
        case LED2:  //LNK
            ledPin[1]=GPIO_PIN_RESET;
            HAL_GPIO_WritePin(LNK_LED_GPIO_PORT, LNK_LED_PIN, (GPIO_PinState)ledPin[1]);
            break;
        case LED3:  //FLT
            ledPin[2]=GPIO_PIN_RESET;
            HAL_GPIO_WritePin(FLT_LED_GPIO_PORT, FLT_LED_PIN, (GPIO_PinState)ledPin[2]);
            break;
*/            
    }
}

void hLedOff(int16_t led)
{
    switch(led)
    {
        case LED1:  //power
            ledPin[0]=GPIO_PIN_RESET;
            HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, (GPIO_PinState)ledPin[0]);
            break;
/*            
        case LED2:  //LNK
            ledPin[1]=GPIO_PIN_SET;
            HAL_GPIO_WritePin(LNK_LED_GPIO_PORT, LNK_LED_PIN, (GPIO_PinState)ledPin[1]);
            break;
        case LED3:  //FLT
            ledPin[2]=GPIO_PIN_SET;
            HAL_GPIO_WritePin(FLT_LED_GPIO_PORT, FLT_LED_PIN, (GPIO_PinState)ledPin[2]);
            break;
*/            
    }
}

void hLedBlink(int16_t led)
{
    switch(led)
    {
        case LED1:  //power
					ledPin[0]  = (ledPin[0])? 0 : 1;
            HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, (GPIO_PinState)ledPin[0]);
            break;
/*            
            
        case LED2:  //LNK
            ledPin[1]  = (ledPin[1])? 0 : 1;
            HAL_GPIO_WritePin(LNK_LED_GPIO_PORT, LNK_LED_PIN, (GPIO_PinState)ledPin[1]);
            break;
        case LED3:  //FLT
            ledPin[2]  = (ledPin[2])? 0 : 1;
            HAL_GPIO_WritePin(FLT_LED_GPIO_PORT, FLT_LED_PIN, (GPIO_PinState)ledPin[2]);
            break;
*/            
    }
}

void hLedClose(void) {}

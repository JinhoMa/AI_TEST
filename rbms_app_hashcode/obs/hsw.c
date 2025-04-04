/**
    ******************************************************************************
    *     @file         hsw.c
    *     @version   
    *     @author    
    *     @date      2017/01/14 
    *     @brief     
    ******************************************************************************
    */
#include "obshal.h"
#include "main.h"
#include <stdint.h>

extern void HAL_SW_MspInit(void);

//bms number 
uint32_t bms_num;

void hSwOpen(void){
    HAL_SW_Init();
}

uint32_t hSwRead(void){
    //default return
	return (1);
}

void hSwClose(void){
}


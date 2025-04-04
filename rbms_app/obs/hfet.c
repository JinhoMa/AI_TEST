/**
 ******************************************************************************
 *     @file         hfet.c
 *     @version   
 *     @author    
 *     @date      2024/09/13 
 *     @brief     
 ******************************************************************************
*/
#include "main.h"
#include "obshal.h"
#include "glovar.h"

/*== MACRO ===================================================================*/
/*== GLOBAL VARIABLE =========================================================*/
/*== STATIC VARIABLE =========================================================*/

 /**
  *  함수명 : hFetOpen
  *  @brief:
  *  @param: 1- HEAT_FET, 2- C_FET 3-D_FET
  *  @return:
  **/
int hFetOpen(uint8_t port)
{
    return 0;
}

/**
  @brief Fet 동작을 활성화 한다.
   활성화 시킨 후 FET ON/OFF 제어를 한다.
**/
void hFetEnable(uint32_t fd)
{
    switch(fd){
        case HEAT_FET:
            BSP_HEAT_FET_ENABLE();
            break;
        case C_FET:
            BSP_CFET_ENABLE();    
            break;
        case D_FET:
            BSP_DFET_ENABLE();    
            break;
    
    }
}
/**
  @brief Fet 동작을 비활성화 한다.
   비활성화 하는 순간 FET 는 도무 OFF 된다.
**/
void hFetDisable(uint32_t fd)
{
    switch(fd){
        case HEAT_FET:
            BSP_HEAT_FET_DISABLE();
            break;
        case C_FET:
            BSP_CFET_DISABLE();    
            break;
        case D_FET:
            BSP_DFET_DISABLE();    
            break;
    
    }
}

uint8_t hFetState(uint32_t fd)
{
    uint8_t ret=0;
    switch(fd){
        case HEAT_FET:
            ret = BSP_HEAT_ST();
            break;
        case C_FET:
            ret = (BSP_CFET_ST() == 1) ? 1 : 0;
            break;
        case D_FET:
            ret = (BSP_DFET_ST() == 1) ? 1 : 0;
            break;
    }
    return ret;
}

uint8_t hRajFetState(uint32_t fd)
{
    uint8_t ret = 0;
    switch (fd)
    {
        case C_FET:
            ret = (BSP_RAJ_CFET_ST() == 1) ? 1 : 0;
            break;
        case D_FET:
            ret = (BSP_RAJ_DFET_ST() == 1) ? 1 : 0;
            break;
    }
    return ret;
}

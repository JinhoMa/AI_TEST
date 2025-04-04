/**
    ******************************************************************************
    *     @file      hfan.c 
    *     @author    SKI ESS Team
    *     @version   V1.0
    *     @date      2014.11.05 
    *     @brief     HAL Fan Module Source
    ******************************************************************************
    */
#include "obshal.h"
#include "main.h"

/** @addtogroup HAL
  * @{
  */
  
  /**
      * @brief 이 소스는 Fan Device를 포팅한 모듈이다.
      * Fan은 PWM Out을 사용하여 RPM을 조절 하게 된다. 
      * hFanPowerOn() 를 호출 하게 되면 Fan에 전압이 인가되어 모터가 구동될 수 있는 상태가 되며
      * hPwmStart() 호출 하는 순간 부터 PWM 의 Duty 비에 따라서 모터의 속도가 바뀌게 된다.
      */
  /** @addtogroup HAL_Fan
    * @{
    */
    
         /** @defgroup HAL_Fan_Private_Variables
                * @{
                */

int fan1_pwmout_fd;
int fan2_pwmout_fd;
int fan1_pwmin_fd;
int fan2_pwmin_fd;
        /**
             * @}
             */

/**
  * @brief Fan 모듈을 open 한다.
  * FAN이 PWM 모듈을 사용하는 경우 Open 함수 내에서 pwm 모듈을 open 하게 된다.
  * @param NONE
  * @retval NONE
  */

void hFanOpen(void)
{
    HAL_FAN_Init();
}

/**
  * @brief FAN의 Power 를 ON 한다.
  * @param[in] fan FAN의 식별값 :  0 or 1 
  * @retval NONE
  */
void hFanPowerOn(uint8_t fan)
{
    HAL_GPIO_WritePin( FAN_OUT_GPIO_PORT, FAN_OUT_PIN, GPIO_PIN_SET ); 
}

/**
  * @brief FAN의 Power를 OFF 한다.
  * @param[in] fan FAN의 식별값:  0 or 1
  * @retval NONE
  */
void hFanPowerOff(uint8_t fan)
{
    HAL_GPIO_WritePin( FAN_OUT_GPIO_PORT, FAN_OUT_PIN, GPIO_PIN_RESET ); 
}

/**
  * @brief FAN 모듈을 Close 한다.
  * @param NONE
  * @retval NONE
  */
void hFanClose(void)
{
}

/**
  * deprecated 함수 
  */
void hFanGpioRead(int *fan1_pw, int *fan2_pw, int *fan1_pw_st, int *fan2_pw_st)
{
}

    /**
      * @}
      */
// End of HAL Group
/**
  * @}
  */


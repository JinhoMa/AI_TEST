/**
  ******************************************************************************
  * @file    stm32f7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx.h"
#include "stm32f7xx_it.h"
#include  "main.h"
#include "obshal.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_spi2_rx;
extern TIM_HandleTypeDef    TimHandle;
extern TIM_HandleTypeDef PwmTimHandle;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

/******************************************************************************/
/*            Cortex-M7 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
#if 0
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
 // osSystickHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}
#endif

/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TimHandle);
}

/**
* @brief This function handles DMA1 stream3 global interrupt.
*/
void DMA1_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

  /* USER CODE END DMA1_Stream3_IRQn 0 */
  //HAL_DMA_IRQHandler(&hdma_spi2_rx);
  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

  /* USER CODE END DMA1_Stream3_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void EVAL_COM1_IRQHandler(void)
{
   UART_HandleTypeDef *handle= (UART_HandleTypeDef *) hSerGetHandle("COM1");
   HAL_UART_IRQHandler(handle);    
}

void COM2_IRQHandler(void)
{
   UART_HandleTypeDef *handle= (UART_HandleTypeDef *) hSerGetHandle("COM2");
   HAL_UART_IRQHandler(handle);    
}

void COM3_IRQHandler(void)
{
   UART_HandleTypeDef *handle= (UART_HandleTypeDef *) hSerGetHandle("COM3");
   HAL_UART_IRQHandler(handle);    
}

void CAN1_TX_IRQHandler(void)
{
   CAN_HandleTypeDef *handle = (CAN_HandleTypeDef *) hCanGetHandleFromID(CAN1_ADDRESS);
   HAL_CAN_IRQHandler(handle);
}

void CAN1_RX0_IRQHandler(void)
{
    CAN_HandleTypeDef *handle = (CAN_HandleTypeDef *) hCanGetHandleFromID(CAN1_ADDRESS);
    HAL_CAN_IRQHandler(handle);
}

void CAN1_RX1_IRQHandler(void)
{
    CAN_HandleTypeDef *handle = (CAN_HandleTypeDef *) hCanGetHandleFromID(CAN1_ADDRESS);
    HAL_CAN_IRQHandler(handle);
}

void CAN1_SCE_IRQHandler(void)
{
    CAN_HandleTypeDef *handle = (CAN_HandleTypeDef *) hCanGetHandleFromID(CAN1_ADDRESS);
    HAL_CAN_IRQHandler(handle);
}

/**
* @brief This function handles DMA2 stream0 global interrupt.
*/
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
   HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

void DMA2_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc2);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}


/**
  * @brief  This function handles ADC interrupt request.
  * @param  None
  * @retval None
  */
void ADC_IRQHandler(void)
{
  ADC_HandleTypeDef *handle = (ADC_HandleTypeDef *) hAdcGetHandle("ADC1");
  HAL_ADC_IRQHandler(handle);
}

void I2C1_EV_IRQHandler(void)
{
    I2C_HandleTypeDef *handle = (I2C_HandleTypeDef *) hI2cGetHandle("I2C1");
    HAL_I2C_EV_IRQHandler(handle);
}

void I2C1_ER_IRQHandler(void)
{
    I2C_HandleTypeDef *handle = (I2C_HandleTypeDef *) hI2cGetHandle("I2C1");
    HAL_I2C_ER_IRQHandler(handle);
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEEPALIVE_IN_PIN);
}


__weak void sm_mcufault_handler(void)
{
    while(1);
}


void HardFault_Handler(void)
{
    sm_mcufault_handler();
}
void MemManage_Handler(void)
{
    sm_mcufault_handler();
}
void BusFault_Handler(void)
{
    sm_mcufault_handler();
}
void UsageFault_Handler(void)
{
    sm_mcufault_handler();
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

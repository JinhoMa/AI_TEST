/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
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

#if  defined(STM32F107xC)
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#endif
#include  "main.h"
#include "obshal.h"

#if BSP_CONFIG_RTOS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

#if BSP_CONFIG_RTOS_RTX
/* Defined in RTX library */
#define SVC_Handler  disabled_SVC_Handler
#define PendSV_Handler  disabled_PendSV_Handler
#define SysTick_Handler  disabled_SysTick_Handler
#elif BSP_CONFIG_RTOS_FREERTOS
/* Defined in FreeRTOSConfig.h */
#define SVC_Handler  disabled_SVC_Handler
#define PendSV_Handler  disabled_PendSV_Handler
#endif

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern CAN_HandleTypeDef hcan2;
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

/******************************************************************************/
/*            Cortex-M7 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/


/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
__weak void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
}

/**
* @brief This function handles System tick timer.
*/

void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
 // osSystickHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */
#if BSP_CONFIG_RTOS_FREERTOS
#if (INCLUDE_xTaskGetSchedulerState == 1)
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif /* INCLUDE_xTaskGetSchedulerState */
  xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1)
  }
#endif
#endif
  /* USER CODE END SysTick_IRQn 1 */
}


/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/
/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief  This function handles EXTI interrupt request.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
  /* EXTI line interrupt detected */
  if(__HAL_GPIO_EXTI_GET_IT(WDG_MISUBO_Pin) != RESET)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(WDG_MISUBO_Pin);
      HAL_GPIO_EXTI_Callback(WDG_MISUBO_Pin);
  }
}

/* USER CODE BEGIN 1 */
/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
   UART_HandleTypeDef *handle= (UART_HandleTypeDef *)hSerGetHandleFromIsr(USART1);

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(handle);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
   UART_HandleTypeDef *handle= (UART_HandleTypeDef *)hSerGetHandleFromIsr(USART2);

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(handle);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
   UART_HandleTypeDef *handle= (UART_HandleTypeDef *)hSerGetHandleFromIsr(USART3);

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(handle);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles CAN2 TX interrupts.
  */
void CAN2_TX_IRQHandler(void)
{
  /* USER CODE BEGIN CAN2_TX_IRQn 0 */
  CAN_HandleTypeDef *handle= (CAN_HandleTypeDef *) hCanGetHandleFromIsr(CAN2);
  /* USER CODE END CAN2_TX_IRQn 0 */
  HAL_CAN_IRQHandler(handle);
  /* USER CODE BEGIN CAN2_TX_IRQn 1 */

  /* USER CODE END CAN2_TX_IRQn 1 */
}

/**
  * @brief This function handles CAN2 RX0 interrupts.
  */
void CAN2_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN CAN2_RX0_IRQn 0 */
  CAN_HandleTypeDef *handle= (CAN_HandleTypeDef *) hCanGetHandleFromIsr(CAN2);
  /* USER CODE END CAN2_RX0_IRQn 0 */
  HAL_CAN_IRQHandler(handle);
  /* USER CODE BEGIN CAN2_RX0_IRQn 1 */

  /* USER CODE END CAN2_RX0_IRQn 1 */
}

/**
  * @brief This function handles CAN2 RX1 interrupt.
  */
void CAN2_RX1_IRQHandler(void)
{
  /* USER CODE BEGIN CAN2_RX1_IRQn 0 */
  CAN_HandleTypeDef *handle= (CAN_HandleTypeDef *) hCanGetHandleFromIsr(CAN2);
  /* USER CODE END CAN2_RX1_IRQn 0 */
  HAL_CAN_IRQHandler(handle);
  /* USER CODE BEGIN CAN2_RX1_IRQn 1 */

  /* USER CODE END CAN2_RX1_IRQn 1 */
}

/**
  * @brief This function handles CAN2 SCE interrupt.
  */
void CAN2_SCE_IRQHandler(void)
{
  /* USER CODE BEGIN CAN2_SCE_IRQn 0 */
  CAN_HandleTypeDef *handle= (CAN_HandleTypeDef *) hCanGetHandleFromIsr(CAN2);
  /* USER CODE END CAN2_SCE_IRQn 0 */
  HAL_CAN_IRQHandler(handle);
  /* USER CODE BEGIN CAN2_SCE_IRQn 1 */

  /* USER CODE END CAN2_SCE_IRQn 1 */
}

void ADC1_2_IRQHandler(void)
{
  ADC_HandleTypeDef *handle = (ADC_HandleTypeDef *) hAdcGetHandle("ADC1");
  HAL_ADC_IRQHandler(handle);
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

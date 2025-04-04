/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_config.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_0
#define LED_GPIO_Port GPIOA
#define USART1_TX_485_Pin GPIO_PIN_9
#define USART1_TX_485_GPIO_Port GPIOA
#define UART1_RX_485_Pin GPIO_PIN_10
#define UART1_RX_485_GPIO_Port GPIOA
#define USART2_TX_232_Pin GPIO_PIN_2
#define USART2_TX_232_GPIO_Port GPIOA
#define USART2_RX_232_Pin GPIO_PIN_3
#define USART2_RX_232_GPIO_Port GPIOA
#define CAN2_RX_Pin       GPIO_PIN_5
#define CAN2_RX_GPIO_Port GPIOB
#define CAN2_TX_Pin       GPIO_PIN_6
#define CAN2_TX_GPIO_Port GPIOB
#define SPI1_NSS_BLE_Pin GPIO_PIN_4
#define SPI1_NSS_BLE_GPIO_Port GPIOA
#define SPI1_SCK_BLE_Pin GPIO_PIN_5
#define SPI1_SCK_BLE_GPIO_Port GPIOA
#define SPI1_MISO_BLE_Pin GPIO_PIN_6
#define SPI1_MISO_BLE_GPIO_Port GPIOA
#define SPI1_MOSI_BLE_Pin GPIO_PIN_7
#define SPI1_MOSI_BLE_GPIO_Port GPIOA
#define BLE_RST_IN_Pin GPIO_PIN_4
#define BLE_RST_IN_GPIO_Port GPIOC
#define CFET_EN_Pin GPIO_PIN_5
#define CFET_EN_GPIO_Port GPIOC
#define DFET_EN_Pin GPIO_PIN_0
#define DFET_EN_GPIO_Port GPIOB
#define HEAT_CONTROL_Pin GPIO_PIN_1
#define HEAT_CONTROL_GPIO_Port GPIOB
#define USART3_RAJ_TX_Pin GPIO_PIN_10
#define USART3_RAJ_TX_GPIO_Port GPIOB
#define USART3_RAJ_RX_Pin GPIO_PIN_11
#define USART3_RAJ_RX_GPIO_Port GPIOB
#define SPI2_NSS_EEPROM_Pin GPIO_PIN_12
#define SPI2_NSS_EEPROM_GPIO_Port GPIOB
#define SPI2_SCK_EEPROM_Pin GPIO_PIN_13
#define SPI2_SCK_EEPROM_GPIO_Port GPIOB
#define SPI2_MISO_EEPROM_Pin GPIO_PIN_14
#define SPI2_MISO_EEPROM_GPIO_Port GPIOB
#define SPI2_MOSI_EEPROM_Pin GPIO_PIN_15
#define SPI2_MOSI_EEPROM_GPIO_Port GPIOB
#define RESET_RAJ_Pin GPIO_PIN_6
#define RESET_RAJ_GPIO_Port GPIOC
#define BUZ_CLK_Pin GPIO_PIN_7
#define BUZ_CLK_GPIO_Port GPIOC
#define WDG_MOSUBI_Pin GPIO_PIN_8
#define WDG_MOSUBI_GPIO_Port GPIOC
#define WDG_MISUBO_Pin GPIO_PIN_9
#define WDG_MISUBO_GPIO_Port GPIOC
#define TRANS_EN_485_Pin GPIO_PIN_1
#define TRANS_EN_485_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
#define RRRR 0
/////////////////////////////////////////////////////////////////////////////////////
//  hled.c
/////////////////////////////////////////////////////////////////////////////////////
#define LED1_PIN             LED_Pin
#define LED1_GPIO_PORT       LED_GPIO_Port
#define LED2_PIN
#define LED2_GPIO_PORT
#define LED3_PIN
#define LED3_GPIO_PORT
#define LEDn        1
#define LED1        1       // Power & LINK LED
#define LED2        2       // NO
#define LED3        3       // NO


/////////////////////////////////////////////////////////////////////////////////////
//  hcan.c
/////////////////////////////////////////////////////////////////////////////////////
#define CANxn       1
/* User can use this section to tailor CANx instance used and associated   resources */
/* Definition for CANx clock resources */
#define CANxB                           CAN2
/* Definition for CANx Pins */
/* Definition for CANx's NVIC */
/////////////////////////////////////////////////////////////////////////////////////
//  hserial.c
/////////////////////////////////////////////////////////////////////////////////////
#define COMxn    3
  #define RS232_UART   USART1  // RS232
  #define RAJ_UART     USART3  // RAJ COM
  #define RS485_UART   USART2  // RS485

#define COM1_Instance    RS232_UART
#define COM2_Instance    RS485_UART
#define COM3_Instance    RAJ_UART

#define BSP_COM_RS485TX_ENABLE()  HAL_GPIO_WritePin(TRANS_EN_485_GPIO_Port, TRANS_EN_485_Pin, GPIO_PIN_SET)
#define BSP_COM_RS485TX_DISABLE() HAL_GPIO_WritePin(TRANS_EN_485_GPIO_Port, TRANS_EN_485_Pin, GPIO_PIN_RESET)
/////////////////////////////////////////////////////////////////////////////////////
//  hspi.c
/////////////////////////////////////////////////////////////////////////////////////
#define BLE_SPI         SPI1
#define EEP_SPI         SPI2
/////////////////////////////////////////////////////////////////////////////////////
//  hadc.c
/////////////////////////////////////////////////////////////////////////////////////
#define BDTEMP_ADC      ADC1
/////////////////////////////////////////////////////////////////////////////////////
//  hfet.c
/////////////////////////////////////////////////////////////////////////////////////
#define BSP_DFET_ENABLE()         hr.model.Dfet = 1
#define BSP_DFET_DISABLE()        hr.model.Dfet = 2
#define BSP_CFET_ENABLE()         hr.model.Cfet = 1
#define BSP_CFET_DISABLE()        hr.model.Cfet = 2
#define BSP_DFET_ST()             hr.model.Dfet
#define BSP_CFET_ST()             hr.model.Cfet
#define BSP_RAJ_DFET_ST()         ir.model.DfetState
#define BSP_RAJ_CFET_ST()         ir.model.CfetState

#define BSP_HEAT_FET_ENABLE()     HAL_GPIO_WritePin(HEAT_CONTROL_GPIO_Port, HEAT_CONTROL_Pin, GPIO_PIN_SET)
#define BSP_HEAT_FET_DISABLE()    HAL_GPIO_WritePin(HEAT_CONTROL_GPIO_Port, HEAT_CONTROL_Pin, GPIO_PIN_RESET)
#define BSP_HEAT_ST()             HAL_GPIO_ReadPin(HEAT_CONTROL_GPIO_Port, HEAT_CONTROL_Pin)

/////////////////////////////////////////////////////////////////////////////////////
//  heeprom.c
/////////////////////////////////////////////////////////////////////////////////////
#define BSP_EEP_CS_ENABLE()       HAL_GPIO_WritePin(SPI2_NSS_EEPROM_GPIO_Port, SPI2_NSS_EEPROM_Pin, GPIO_PIN_RESET)
#define BSP_EEP_CS_DISABLE()      HAL_GPIO_WritePin(SPI2_NSS_EEPROM_GPIO_Port, SPI2_NSS_EEPROM_Pin, GPIO_PIN_SET)


/////////////////////////////////////////////////////////////////////////////////////
//  buzzer
/////////////////////////////////////////////////////////////////////////////////////
#define BUZ_ON()                  HAL_GPIO_WritePin(BUZ_CLK_GPIO_Port, BUZ_CLK_Pin, GPIO_PIN_SET)
#define BUZ_OFF()                 HAL_GPIO_WritePin(BUZ_CLK_GPIO_Port, BUZ_CLK_Pin, GPIO_PIN_RESET)
/////////////////////////////////////////////////////////////////////////////////////
//  Watchdog
/////////////////////////////////////////////////////////////////////////////////////
#define BSP_WDGPIN_TOGGLE()       HAL_GPIO_TogglePin(WDG_MOSUBI_GPIO_Port, WDG_MOSUBI_Pin)

#define BSP_RAJ_RESET_ON()        HAL_GPIO_WritePin(RESET_RAJ_GPIO_Port, RESET_RAJ_Pin, GPIO_PIN_SET)   //! RAJ STUCK 코드 구현 테스트
#define BSP_RAJ_RESET_OFF()       HAL_GPIO_WritePin(RESET_RAJ_GPIO_Port, RESET_RAJ_Pin, GPIO_PIN_RESET)     //! RAJ STUCK 코드 구현 테스트

/////////////////////////////////////////////////////////////////////////////////////
//  hadc.c
/////////////////////////////////////////////////////////////////////////////////////
#define TEMP_ADC_CH10 0
#define TEMP_ADC_CH11 1

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

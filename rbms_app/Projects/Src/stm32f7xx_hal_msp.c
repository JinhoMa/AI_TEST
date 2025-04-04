﻿/**
  ******************************************************************************
  * File Name          : stm32f7xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
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
#include "main.h"

extern DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);

  /* USER CODE BEGIN MspInit 1 */
  //GPIO All Clock used at System Should be Enabled here
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOE_CLK_ENABLE();


  /* USER CODE END MspInit 1 */
}

/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIM3 Peripheral clock enable */
  TIMx_CLK_ENABLE();


  /*##-2- Configure I/Os #####################################################*/
  /* Configure PB.04 (TIM3_Channel1), PB.05 (TIM3_Channel2), PB.00 (TIM3_Channel3),
     PB.01 (TIM3_Channel4) in output, pull-up, alternate function mode
  */


  /*##-3- Enable the TIM2 global Interrupt & set priority ####################*/
  HAL_NVIC_SetPriority(TIM3_IRQn, 15, 1);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
}
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN TIM4_MspPostInit 0 */
    __HAL_RCC_TIM4_CLK_ENABLE();
    /*##-3- Enable the TIM2 global Interrupt & set priority ####################*/
      HAL_NVIC_SetPriority(TIM4_IRQn, 15, 1);
      HAL_NVIC_EnableIRQ(TIM4_IRQn);        
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(htim->Instance==TIM4)
    {
        /* USER CODE END TIM4_MspPostInit 0 */
        /**TIM4 GPIO Configuration    
        PD12     ------> TIM4_CH1
        PD13     ------> TIM4_CH2
        PD14     ------> TIM4_CH3 
                */
        GPIO_InitStruct.Pin = MARLY_P_PIN|PRERLY_PIN|MARLY_N_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        /* USER CODE BEGIN TIM4_MspPostInit 1 */

        /* USER CODE END TIM4_MspPostInit 1 */
    }

}



void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */
  
    /**I2C1 GPIO Configuration    
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = RTC_SDA_PIN |RTC_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(RTC_GPIO_PORT, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */
  HAL_NVIC_SetPriority(I2C1_EV_IRQn, 11, 0);
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
  HAL_NVIC_SetPriority(I2C1_ER_IRQn, 11, 0);
  HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{

  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

  }
  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
} 

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hadc->Instance == RBMS_ADC )
    {
        /* Peripheral clock enable */
        ADC_CLK_ENABLE();
        //ADC_GPIO_CLK_ENABLE();
        //__GPIOC_CLK_ENABLE();
        GPIO_InitStruct.Pin = TEMP_ADC_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
#if 0
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __ADC1_CLK_DISABLE();
  
    /**ADC1 GPIO Configuration    
    PA0/WKUP     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    PA4     ------> ADC1_IN4
    PA5     ------> ADC1_IN5
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7 
    */
    HAL_GPIO_DeInit(GPIOA, TEMP_ADC_Pin|HVDC_ADC_Pin|MAIN_RLYCUR_P_ADC_Pin|PRE_RLYCUR_ADC_Pin 
                          |MAIN_RLYCUR_N_ADC_Pin|CUR_SEN_ADC_Pin);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
  else if(hadc->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspDeInit 0 */

  /* USER CODE END ADC2_MspDeInit 0 */
    /* Peripheral clock disable */
    __ADC2_CLK_DISABLE();
  
    /**ADC2 GPIO Configuration    
    PA2     ------> ADC2_IN2 
    */
    HAL_GPIO_DeInit(GFD__ADC_GPIO_Port, GFD__ADC_Pin);

  /* USER CODE BEGIN ADC2_MspDeInit 1 */

  /* USER CODE END ADC2_MspDeInit 1 */
  }
  else if(hadc->Instance==ADC3)
  {
  /* USER CODE BEGIN ADC3_MspDeInit 0 */

  /* USER CODE END ADC3_MspDeInit 0 */
    /* Peripheral clock disable */
    __ADC3_CLK_DISABLE();
  
    /**ADC3 GPIO Configuration    
    PA3     ------> ADC3_IN3 
    */
    HAL_GPIO_DeInit(GFD__ADCA3_GPIO_Port, GFD__ADCA3_Pin);

  /* USER CODE BEGIN ADC3_MspDeInit 1 */

  /* USER CODE END ADC3_MspDeInit 1 */
  }
#endif
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(hcan->Instance==CAN1)
  {
      /*##-1- Enable peripherals and GPIO Clocks #################################*/
      /* Enable GPIO TX/RX clock */
      CANx1_GPIO_CLK_ENABLE();

      //CANx1_REMAP_ENABLE();
      /* Enable CAN  clock */
      CANx1_CLK_ENABLE();

      /*##-2- Configure peripheral GPIO ##########################################*/
      GPIO_InitStruct.Pin       = CANx1_TX_PIN|CANx1_RX_PIN;
      GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull      = GPIO_NOPULL; //GPIO_PULLUP;
      GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;

      HAL_GPIO_Init(CANx1_TX_GPIO_PORT, &GPIO_InitStruct);
  
      HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 11, 0);
      HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
      HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 11, 0);
      HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
#if 0
  if(hcan->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __CAN1_CLK_DISABLE();
  
    /**CAN1 GPIO Configuration    
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX 
    */
    HAL_GPIO_DeInit(GPIOA, CAN1_RXD_Pin|CAN1_TXD_Pin);

  }
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
#endif
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(hspi->Instance==BMIC_SPI)
  {
      /* USER CODE BEGIN SPI1_MspInit 0 */
      
      /* USER CODE END SPI1_MspInit 0 */
        /* Peripheral clock enable */
        
        BMIC_CLK_ENABLE();
        //BMIC_GPIO_CLK_ENABLE();
        /**SPI1 GPIO Configuration    
        PA5     ------> SPI1_SCK
        PA6     ------> SPI1_MISO
        PA7     ------> SPI1_MOSI 
        */
        GPIO_InitStruct.Pin = BMIC_SCK_PIN|BMIC_MOSI_PIN|BMIC_MISO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(BMIC_GPIO_PORT, &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = BMIC_CS1_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = 0;
        HAL_GPIO_Init(BMIC_CS1_GPIO_PORT, &GPIO_InitStruct);
        /*
        GPIO_InitStruct.Pin = BMIC_CS2_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = 0;
        HAL_GPIO_Init(BMIC_CS2_GPIO_PORT, &GPIO_InitStruct);
        */
      
      /* USER CODE BEGIN SPI1_MspInit 1 */
      
      /* USER CODE END SPI1_MspInit 1 */

  }  
  if(hspi->Instance==ETH_SPI)
  {
      /* USER CODE BEGIN SPI2_MspInit 0 */
      
      /* USER CODE END SPI2_MspInit 0 */
        /* Peripheral clock enable */
        ETH_CLK_ENABLE();
        GPIO_InitStruct.Pin = ETH_CLK_PIN|ETH_MOSI_PIN|ETH_MISO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(ETH_SPI_GPIO_PORT, &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = ETH_CS_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = 0;
        HAL_GPIO_Init(ETH_CS_GPIO_PORT, &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = ETH_RESET_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = 0;
        HAL_GPIO_Init(ETH_RESET_GPIO_PORT, &GPIO_InitStruct);
      
      /* USER CODE BEGIN SPI2_MspInit 1 */
      
      /* USER CODE END SPI2_MspInit 1 */

  }  
  
  if(hspi->Instance==EEP_SPI)
  {
        EEP_CLK_ENABLE();
        EEP_CS_GPIO_CLK_ENABLE();
        GPIO_InitStruct.Pin = EEP_SCK_PIN|EEP_MOSI_PIN|EEP_MISO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
        HAL_GPIO_Init(EEP_GPIO_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = EEP_CS_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = 0;
        HAL_GPIO_Init(EEP_CS_GPIO_PORT, &GPIO_InitStruct);
        BSP_EEP_CS_DISABLE();

  }  

#if 0
  if(hspi->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspInit 0 */

  /* USER CODE END SPI2_MspInit 0 */
    /* Peripheral clock enable */
    __SPI2_CLK_ENABLE();
  
    /**SPI2 GPIO Configuration    
    PB12     ------> SPI2_NSS
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI 
    */
    GPIO_InitStruct.Pin = M_BMIC_STE_A_Pin|M_BMIC_CLK_Pin|M_BMIC_SOMI_Pin|M_BMIC_SIMO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_spi2_rx.Instance = DMA1_Stream3;
    hdma_spi2_rx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi2_rx.Init.Mode = DMA_NORMAL;
    hdma_spi2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_spi2_rx);

    __HAL_LINKDMA(hspi,hdmarx,hdma_spi2_rx);

  /* USER CODE BEGIN SPI2_MspInit 1 */

  /* USER CODE END SPI2_MspInit 1 */
  }
  else if(hspi->Instance==SPI3)
  {
  /* USER CODE BEGIN SPI3_MspInit 0 */

  /* USER CODE END SPI3_MspInit 0 */
    /* Peripheral clock enable */
    __SPI3_CLK_ENABLE();
  
    /**SPI3 GPIO Configuration    
    PC10     ------> SPI3_SCK
    PC11     ------> SPI3_MISO
    PC12     ------> SPI3_MOSI 
    */
    GPIO_InitStruct.Pin = EEP_SCK_Pin|EEP_MISO_Pin|EEP_MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI3_MspInit 1 */

  /* USER CODE END SPI3_MspInit 1 */
  }
  else if(hspi->Instance==SPI4)
  {
  /* USER CODE BEGIN SPI4_MspInit 0 */

  /* USER CODE END SPI4_MspInit 0 */
    /* Peripheral clock enable */
    __SPI4_CLK_ENABLE();
  
    /**SPI4 GPIO Configuration    
    PE2     ------> SPI4_SCK
    PE5     ------> SPI4_MISO
    PE6     ------> SPI4_MOSI 
    */
    GPIO_InitStruct.Pin = ETH_SCK_Pin|ETH_SOMI_Pin|ETH_SIMO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI4_MspInit 1 */

  /* USER CODE END SPI4_MspInit 1 */
  }
#endif
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
#if 0
  if(hspi->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspDeInit 0 */

  /* USER CODE END SPI2_MspDeInit 0 */
    /* Peripheral clock disable */
    __SPI2_CLK_DISABLE();
  
    /**SPI2 GPIO Configuration    
    PB12     ------> SPI2_NSS
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI 
    */
    HAL_GPIO_DeInit(GPIOB, M_BMIC_STE_A_Pin|M_BMIC_CLK_Pin|M_BMIC_SOMI_Pin|M_BMIC_SIMO_Pin);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(hspi->hdmarx);
  /* USER CODE BEGIN SPI2_MspDeInit 1 */

  /* USER CODE END SPI2_MspDeInit 1 */
  }
  else if(hspi->Instance==SPI3)
  {
  /* USER CODE BEGIN SPI3_MspDeInit 0 */

  /* USER CODE END SPI3_MspDeInit 0 */
    /* Peripheral clock disable */
    __SPI3_CLK_DISABLE();
  
    /**SPI3 GPIO Configuration    
    PC10     ------> SPI3_SCK
    PC11     ------> SPI3_MISO
    PC12     ------> SPI3_MOSI 
    */
    HAL_GPIO_DeInit(GPIOC, EEP_SCK_Pin|EEP_MISO_Pin|EEP_MOSI_Pin);

  /* USER CODE BEGIN SPI3_MspDeInit 1 */

  /* USER CODE END SPI3_MspDeInit 1 */
  }
  else if(hspi->Instance==SPI4)
  {
  /* USER CODE BEGIN SPI4_MspDeInit 0 */

  /* USER CODE END SPI4_MspDeInit 0 */
    /* Peripheral clock disable */
    __SPI4_CLK_DISABLE();
  
    /**SPI4 GPIO Configuration    
    PE2     ------> SPI4_SCK
    PE5     ------> SPI4_MISO
    PE6     ------> SPI4_MOSI 
    */
    HAL_GPIO_DeInit(GPIOE, ETH_SCK_Pin|ETH_SOMI_Pin|ETH_SIMO_Pin);

  /* USER CODE BEGIN SPI4_MspDeInit 1 */

  /* USER CODE END SPI4_MspDeInit 1 */
  }
#endif
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(huart->Instance==EVAL_COM1 )
    {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        //EVAL_COM1_TX_CLK_ENABLE();
        //EVAL_COM1_RX_CLK_ENABLE();

        /* Select SysClk as source of UART4 clocks */
        EVAL_COM1_SYSCLOCK();

        /* Enable USARTx clock */
        EVAL_COM1_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin       = EVAL_COM1_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = EVAL_COM1_AF;

        HAL_GPIO_Init(EVAL_COM1_TX_GPIO_PORT, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = EVAL_COM1_RX_PIN;
        GPIO_InitStruct.Alternate = EVAL_COM1_AF;

        HAL_GPIO_Init(EVAL_COM1_RX_GPIO_PORT, &GPIO_InitStruct);
        HAL_NVIC_SetPriority(EVAL_COM1_IRQn, 10, 0);
        HAL_NVIC_EnableIRQ(EVAL_COM1_IRQn);

    }else
    if(huart->Instance==EVAL_COM2 )
    {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        //EVAL_COM1_TX_CLK_ENABLE();
        //EVAL_COM1_RX_CLK_ENABLE();

        /* Select SysClk as source of UART4 clocks */
        COM2_SYSCLOCK();

        /* Enable USARTx clock */
        COM2_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin       = COM2_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = COM2_AF;

        HAL_GPIO_Init(COM2_TX_GPIO_PORT, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = COM2_RX_PIN;
        GPIO_InitStruct.Alternate = COM2_AF;

        HAL_GPIO_Init(COM2_RX_GPIO_PORT, &GPIO_InitStruct);
        HAL_NVIC_SetPriority(COM2_IRQn, 10, 0);
        HAL_NVIC_EnableIRQ(COM2_IRQn);

    }else
    if(huart->Instance==EVAL_COM3 )
    {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        //EVAL_COM1_TX_CLK_ENABLE();
        //EVAL_COM1_RX_CLK_ENABLE();

        /* Select SysClk as source of UART4 clocks */
        COM3_SYSCLOCK();

        /* Enable USARTx clock */
        COM3_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin       = COM3_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = COM3_AF;

        HAL_GPIO_Init(COM3_TX_GPIO_PORT, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = COM3_RX_PIN;
        GPIO_InitStruct.Alternate = COM3_AF;

        HAL_GPIO_Init(COM3_RX_GPIO_PORT, &GPIO_InitStruct);
        HAL_NVIC_SetPriority(COM3_IRQn, 10, 0);
        HAL_NVIC_EnableIRQ(COM3_IRQn);

    }
    
#if 0
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
#endif

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
#if 0
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

  }
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
#endif
}
void HAL_SW_Init(void)
{

}

void HAL_RLY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin= MARLY_P_PIN | MARLY_N_PIN |  PRERLY_PIN ;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed= GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MARLY_P_GPIO_PORT, &GPIO_InitStruct);

}

void HAL_EXT_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    //RELAY POWER ON/OFF
    GPIO_InitStruct.Pin =  RLY_PWR_SW_PIN ;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed= GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RLY_PWR_SW_GPIO_PORT, &GPIO_InitStruct);

}


void HAL_BMIC_Init(void)
{

}

void HAL_RENESAS_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
  
    //SUB MCU#1 OUT
    GPIO_InitStruct.Pin =  KEEPALIVE_OUT_PIN ;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed= GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEEPALIVE_OUT_GPIO_PORT, &GPIO_InitStruct);

    //INPUT FROM SUB MCU#1 
    GPIO_InitStruct.Pin =  KEEPALIVE_IN_PIN ;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed= GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEEPALIVE_IN_GPIO_PORT, &GPIO_InitStruct);
    /* PD3 is connected to EXTI_Line3 */ 
    HAL_NVIC_SetPriority(EXTI3_IRQn, 11, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    

}

void HAL_RENESAS2_Init(void)
{
    return;
}

void HAL_LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin= PWR_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed= GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(PWR_LED_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin= LNK_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed= GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LNK_LED_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin= FLT_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed= GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FLT_LED_GPIO_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(PWR_LED_GPIO_PORT, PWR_LED_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LNK_LED_GPIO_PORT, LNK_LED_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(FLT_LED_GPIO_PORT, FLT_LED_PIN, GPIO_PIN_SET);

}

void HAL_OCD_Init(void)
{
    return;
}

void HAL_FAN_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin= FAN_OUT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed= GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FAN_OUT_GPIO_PORT, &GPIO_InitStruct);
}

/* USER CODE BEGIN 1 */
void BSP_COM_RS485TX_ENABLE(USART_TypeDef *base)
{
/*
    if(base == EVAL_COM2){
        GPIO_SetBits(EVAL_COM2_CTS_GPIO_PORT, EVAL_COM2_CTS_PIN);
    }else if(base == EVAL_COM3){
        GPIO_SetBits(EVAL_COM3_TXE_GPIO_PORT, EVAL_COM3_TXE_PIN);
    }else if(base == EVAL_COM4){
        GPIO_SetBits(EVAL_COM4_TXE_GPIO_PORT, EVAL_COM4_TXE_PIN);
    }
 */
}
void BSP_COM_RS485TX_DISABLE(USART_TypeDef *base)
{
/*
    if(base == EVAL_COM2){
        GPIO_ResetBits(EVAL_COM2_CTS_GPIO_PORT, EVAL_COM2_CTS_PIN);
    }else if(base == EVAL_COM3){
        GPIO_ResetBits(EVAL_COM3_TXE_GPIO_PORT, EVAL_COM3_TXE_PIN);
    }else if(base == EVAL_COM4){
        GPIO_ResetBits(EVAL_COM4_TXE_GPIO_PORT, EVAL_COM4_TXE_PIN);
    }
*/
}
void BSP_CAN_ENABLE(void)
{
    return;
}
void BSP_CAN_DISABLE(void)
{
    return;
}
void BSP_ETH_RESET_ENABLE(void)
{
    HAL_GPIO_WritePin( ETH_RESET_GPIO_PORT, ETH_RESET_PIN, GPIO_PIN_RESET );
}
void BSP_ETH_RESET_DISABLE(void)
{
    HAL_GPIO_WritePin( ETH_RESET_GPIO_PORT, ETH_RESET_PIN, GPIO_PIN_SET );
}
void BSP_ETH_CS_ENABLE(void)
{
    HAL_GPIO_WritePin( ETH_CS_GPIO_PORT, ETH_CS_PIN, GPIO_PIN_RESET );
}
void BSP_ETH_CS_DISABLE(void)
{
    HAL_GPIO_WritePin( ETH_CS_GPIO_PORT, ETH_CS_PIN, GPIO_PIN_SET );
}

void BSP_EEP_CS_ENABLE(void)
{
    HAL_GPIO_WritePin( EEP_CS_GPIO_PORT, EEP_CS_PIN, GPIO_PIN_RESET );
}
void BSP_EEP_CS_DISABLE(void)
{
    HAL_GPIO_WritePin( EEP_CS_GPIO_PORT, EEP_CS_PIN, GPIO_PIN_SET );
}
void BSP_RLY_POS_OUT(void)
{
    HAL_GPIO_WritePin( MARLY_P_GPIO_PORT, MARLY_P_PIN, GPIO_PIN_SET );//GPIO LEVEL HIGH
}
void BSP_RLY_PRE_OUT(void)
{
    HAL_GPIO_WritePin( PRERLY_GPIO_PORT, PRERLY_PIN, GPIO_PIN_SET );  //GPIO LEVEL HIGH
}
void BSP_RLY_NEG_OUT(void)
{
    HAL_GPIO_WritePin( MARLY_N_GPIO_PORT, MARLY_N_PIN, GPIO_PIN_SET );//GPIO LEVEL HIGH
}
void BSP_RLY_POS_OFF(void)
{
    HAL_GPIO_WritePin( MARLY_P_GPIO_PORT, MARLY_P_PIN, GPIO_PIN_RESET );//GPIO LEVEL LOW
}
void BSP_RLY_PRE_OFF(void)
{
    HAL_GPIO_WritePin( PRERLY_GPIO_PORT, PRERLY_PIN, GPIO_PIN_RESET ); //GPIO LEVEL LOW
}
void BSP_RLY_NEG_OFF(void)
{
    HAL_GPIO_WritePin( MARLY_N_GPIO_PORT, MARLY_N_PIN, GPIO_PIN_RESET );//GPIO LEVEL LOW
}
void BSP_FAN_OUT(void)
{
    HAL_GPIO_WritePin( FAN_OUT_GPIO_PORT, FAN_OUT_PIN, GPIO_PIN_SET ); 
}
void BSP_FAN_OFF(void)
{
    HAL_GPIO_WritePin( FAN_OUT_GPIO_PORT, FAN_OUT_PIN, GPIO_PIN_RESET ); 
}

void BSP_WDGPIN_TOGLE(void)
{
    HAL_GPIO_TogglePin(KEEPALIVE_OUT_GPIO_PORT, KEEPALIVE_OUT_PIN);
    
}
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

#include "obsHal.h"
#include "htypes.h"
#include "main.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cmsis_os.h"

int syslock=0;


//////////////////////////////////////////////////////////////////////////////////
//                 io resource
/////////////////////////////////////////////////////////////////////////////////

 /* SysTick_IRQn interrupt configuration */

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 9
  *            PLL2_M                          = 8
  *            PREDIV1                          = /5
  *            PREDIV2                          = /5
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
#if 0
static void SystemClock_Config(void)
{
 #if 0
RCC_OscInitTypeDef RCC_OscInitStruct = {0};
RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

/** Configure the main internal regulator output voltage
*/
__HAL_RCC_PWR_CLK_ENABLE();
__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
/** Initializes the RCC Oscillators according to the specified parameters
* in the RCC_OscInitTypeDef structure.
*/
RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
RCC_OscInitStruct.HSEState = RCC_HSE_ON;
RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
RCC_OscInitStruct.PLL.PLLM = 4;
RCC_OscInitStruct.PLL.PLLN = 216;
RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
RCC_OscInitStruct.PLL.PLLQ = 2;
if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
{
  //Error_Handler();
}
/** Activate the Over-Drive mode
*/
if (HAL_PWREx_EnableOverDrive() != HAL_OK)
{
  //Error_Handler();
}
/** Initializes the CPU, AHB and APB buses clocks
*/
RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
{
  //Error_Handler();
}
PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                            |RCC_PERIPHCLK_I2C1;
PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
{
  //Error_Handler();
}
/** Enables the Clock Security System
*/
HAL_RCC_EnableCSS();
HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

/* SysTick_IRQn interrupt configuration */
HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);


#endif
}
#endif

// key ���� ���ؼ� ���ε� Port String �� �д´�.
// Key="shell.serial.port", -> "COM1" or "COM2"
char *hSysProperty(char *key){
    if(strcmp(key,"shell.serial.port") == 0){
        return "COM1";
    }
    if(strcmp(key,"rs485.port")==0){
        return "COM2";
    }
    if(strcmp(key,"eeprom.spi.port")==0){
        return "SPI2";
    }
    if(strcmp(key,"ble.spi.port")==0){
        return "SPI1";
    }
            
    return "";
}

/**
  * @brief System 초기화 한다.
  *  이 함수내에서 사용되는 HAL Module들을 초기화 하게 된다.
  *  System이 부팅  되는 과정에서 최초 1회 호출 하게 된다.
  * @param NONE
  * @retval NONE
  */
void hSysInit(void)
{
#if (__CORTEX_M == 7U)
    SCB_EnableICache();
    SCB_EnableDCache();
#endif
syslock = 0;
}

void HAL_Delay(__IO uint32_t Delay)
{
#if defined(__CMSIS_RTOS) || defined(__FREERTOS__)
    osDelay(Delay);
#else
    error "...."
#endif

//#if defined(	__FREERTOS__)
//    vTaskDelay(Delay);
//#endif
}

void hSysWait(uint32_t msec)
{
    HAL_Delay(msec);
}

void hSysLock(void)
{
    __disable_irq();
    syslock++;
}


void hSysUnlock(void)
{
   --syslock;
   if(syslock ==0)
   __enable_irq();
}

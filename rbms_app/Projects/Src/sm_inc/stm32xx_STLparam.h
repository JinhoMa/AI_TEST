﻿/**
  ******************************************************************************
  * @file    stm32xx_STLparam.h 
  * @author  MCD Application Team
  * @version V2.3.0
  * @date    28-Jun-2019
  * @brief   This file contains the parameters to be customized for
  *                      the final application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SELFTEST_PARAM_H
#define __SELFTEST_PARAM_H

/* Includes ------------------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
typedef enum
{
  STL_SUCCESS = 1,
  STL_ERROR = !STL_SUCCESS
} STLErrorStatus;

/* Exported constants --------------------------------------------------------*/

/* This is for having self-diagnostic messages reported on a PC via UART */
//#define STL_VERBOSE_POR       /* During Power-on phase only */
//#define STL_VERBOSE           /* During main program execution  */

/* uncoment next lines to configure watchdogs & resets for debug purposes */
#define USE_INDEPENDENT_WDOG
#define USE_WINDOW_WDOG

/* comment out next line to remove any eval board control */
#define STL_EVAL_MODE
/* comment out next line to remove any control of eval board LCD display  */
/* #define STL_EVAL_LCD */
/* uncomment next line to add user defined control of uxiliary VLM and NVM gpios */
/* #define STL_USER_AUX_MODE */

/* comment out next line to force micro reset at fail mode */
#define NO_RESET_AT_FAIL_MODE

/* comment out next line when CRC unit is not configurable */
#define CRC_UNIT_CONFIGURABLE

/* comment out next line when there is no window feature at IWDG */
#define IWDG_FEATURES_BY_WINDOW_OPTION

/* comment out next line when HSE clock is not used - clock test then runs based on HSI */
/* #define HSE_CLOCK_APPLIED */

/* comment out next line for STL dubug if CRC result is not finaly stored at flash, 
   startup calculation result is then applied at run time - not applicable for ARM V6 compiler */  
/* #define DEBUG_CRC_CALCULATION */

/* system clock & HSE when HSE is applied as PLL source */
#if !defined  (HSE_VALUE) 
  #define HSE_VALUE    ((uint32_t)8000000U) /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */
#define SYSTCLK_AT_RUN_HSE (uint32_t)(110000000uL)
/* system clock when HSI is applied as PLL source (HSE is not used) */
#define SYSTCLK_AT_STARTUP (uint32_t)(110000000uL)
#define SYSTCLK_AT_RUN_HSI (uint32_t)(110000000uL)

/* Reserved area for RAM buffer, incl overlap for test purposes */
/* Don't change this parameter as it is related to physical technology used! */
#define RT_RAM_BLOCKSIZE   (uint32_t)6u 
/* Min overlap to cover coupling fault from one tested row to the other */
#define RT_RAM_BLOCK_OVERLAP  (uint32_t)1u
                                           
/* These are the direct and inverted data (pattern) used during the RAM
test, performed using March C- Algorithm */
#define BCKGRND     ((uint32_t)0x00000000uL)
#define INV_BCKGRND ((uint32_t)0xFFFFFFFFuL)
/* uncoment next line to use March-X test instead of March-C */
/* #define USE_MARCHX_RAM_TEST */

/* This is to provide a time base longer than the SysTick for the main */
/* For instance thi scan be used to signalize refresh the LSI watchdog and window watchdog at main */
#define SYSTICK_10ms_TB  ((uint32_t)10uL) /* 10*1ms */

/* Value of the Internal LSI oscillator in Hz */
  #define LSI_Freq    ((uint32_t)32000uL)

  /* HSE frequency above this limit considered as harmonics */
  #define HSE_LimitHigh(fcy) ((uint32_t)(((fcy)/LSI_Freq)*8u*5u)/4u) /* (HSEValue + 25%) */

  /* HSE frequency below this limit considered as sub-harmonics*/
  #define HSE_LimitLow(fcy) ((uint32_t)(((fcy)/LSI_Freq)*8u*3u)/4u)  /* (HSEValue - 25%) */

  /* here you can define HSI frequency limits  */
  #define HSI_LimitHigh(fcy) ((uint32_t)(((fcy)/LSI_Freq)*8u*6u)/5u) /* (HSIValue + 20%) */
  #define HSI_LimitLow(fcy) ((uint32_t)(((fcy)/LSI_Freq)*8u*4u)/5u)  /* (HSIValue - 20%) */

/* define the maximum U32 */
#define U32_MAX    ((uint32_t)4294967295uL)

/* make control of DBG clock required by STL for debug mode inefficient - e.g. case of STM32L4 */
#ifndef __HAL_RCC_DBGMCU_CLK_ENABLE
#define __HAL_RCC_DBGMCU_CLK_ENABLE()
#endif /* __HAL_RCC_DBGMCU_CLK_ENABLE */

/* -------------------------------------------------------------------------- */
/* ------------------ CONTROL FLOW TAGS and CHECKPOINTS --------------------- */
/* -------------------------------------------------------------------------- */
/* RAM_TEST_CALLEE is only needed for CtrlFlowCntInv when exiting routine */
/* This is because the RAM test routines destroys the control flow counters */
#define RAM_TEST_CALLEE         ((uint32_t)0xFFFFFFFFuL)

#define CPU_TEST_CALLER         ((uint32_t)2)
#define CPU_TEST_CALLEE         ((uint32_t)3) /* Do not modify: hard coded in assembly file */
#define WDG_TEST_CALLER         ((uint32_t)5)
#define CRC32_INIT_CALLER       ((uint32_t)7)
#define CRC32_TEST_CALLER       ((uint32_t)11)
#define CRC32_TEST_CALLEE       ((uint32_t)13)
#define CRC_TEST_CALLER         ((uint32_t)17)
#define CRC_STORE_CALLER        ((uint32_t)19)
#define CLOCK_TEST_CALLER       ((uint32_t)23)
#define CLOCK_TEST_CALLEE       ((uint32_t)29)
#define LSI_INIT_CALLEE         ((uint32_t)31)
#define HSE_INIT_CALLEE         ((uint32_t)37)
#define RTC_INIT_CALLEE         ((uint32_t)41)
#define SYSTICK_INIT_CALLEE     ((uint32_t)43)
#define CLOCK_SWITCH_CALLEE     ((uint32_t)47)
#define STACK_OVERFLOW_TEST     ((uint32_t)53)
#define STACK_OVERFLOW_CALLEE   ((uint32_t)59)
#define CLOCKPERIOD_TEST_CALLEE ((uint32_t)61)
#define FLASH_TEST_CALLER       ((uint32_t)63)
#define CRC32_RUN_TEST_CALLEE   ((uint32_t)65)

#define CHECKPOINT1 ((uint32_t)CPU_TEST_CALLER + \
                          CPU_TEST_CALLEE + \
                          WDG_TEST_CALLER + \
                          CRC32_TEST_CALLER + \
                          CRC_TEST_CALLER)

#define CHECKPOINT2 ((uint32_t)CRC_STORE_CALLER +\
                          CLOCK_TEST_CALLER + \
                          CLOCK_TEST_CALLEE + \
                          STACK_OVERFLOW_TEST)

/* This is for run-time tests with 32-bit CRC */
#define DELTA_MAIN  ((uint32_t)CPU_TEST_CALLER + \
                          CPU_TEST_CALLEE + \
                          STACK_OVERFLOW_TEST + \
                          STACK_OVERFLOW_CALLEE + \
                          CLOCK_TEST_CALLER + \
                          CLOCKPERIOD_TEST_CALLEE + \
                          FLASH_TEST_CALLER + \
                          CRC32_RUN_TEST_CALLEE)

#define LAST_DELTA_MAIN ((uint32_t) DELTA_MAIN + CRC32_INIT_CALLER)
#define FULL_FLASH_CHECKED ((uint32_t)DELTA_MAIN * STEPS_NUMBER + LAST_DELTA_MAIN)

#define MEASPERIOD_ISR_CALLER   ((uint32_t)2)
#define MEASPERIOD_ISR_CALLEE   ((uint32_t)3)
#define CLOCKPERIOD_ISR_CALLEE  ((uint32_t)5)
#define RAM_MARCHC_ISR_CALLER   ((uint32_t)7)
#define RAM_MARCHC_ISR_CALLEE   ((uint32_t)11)

/* This is for March C tests */
#define DELTA_ISR  (uint32_t)(RAM_MARCHC_ISR_CALLER + \
                           RAM_MARCHC_ISR_CALLEE)

#define CLASS_B_ROWS (((uint32_t)CLASS_B_END - (uint32_t)CLASS_B_START) / (RT_RAM_BLOCKSIZE - (2u * RT_RAM_BLOCK_OVERLAP)))
/* +2 below is for last block & buffer self-test itself */
#define RAM_TEST_COMPLETED ((uint32_t)((uint32_t)DELTA_ISR * (uint32_t)((CLASS_B_ROWS / 4u) + 2u)))

/* Exported macro ------------------------------------------------------------*/
#define init_control_flow() CtrlFlowCntInv = ~(CtrlFlowCnt = 0uL)
#define control_flow_call(a) CtrlFlowCnt += (a)
#define control_flow_resume(a) CtrlFlowCntInv -= (a)

/* Exported functions ------------------------------------------------------- */
#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
/* This is the IAR compiler entry point, usually executed right after reset */
  extern void __iar_program_start( void );
  extern uint32_t __checksum;
/* The 4 variables here above are set by IAR linker */
  extern uint32_t __ICFEDIT_region_ROM_start__;
  extern uint32_t __ICFEDIT_region_ROM_end__;
  extern uint32_t __ICFEDIT_region_RAM_start__;
  extern uint32_t __ICFEDIT_region_RAM_end__;
  extern uint32_t __ICFEDIT_region_CLASSB_start__;
  extern uint32_t __ICFEDIT_region_CLASSB_end__;  
  extern uint32_t __ICFEDIT_region_SRAM2_start__;

  /* Constants necessary for Flash CRC calculation  (last block - 64 bytes - separated for CRC) */
  #define ROM_START (uint8_t *)&__ICFEDIT_region_ROM_start__ 
  #define ROM_END   (uint8_t *)((uint8_t *)(&__checksum) - 1) 
  #define ROM_SIZE  (uint32_t)(ROM_END - ROM_START + 1u)

  #define STEPS_NUMBER          ((uint32_t)ROM_SIZE/64u)
  #define ROM_SIZEinWORDS       (uint32_t) (ROM_SIZE/4u)
  #define FLASH_BLOCK_WORDS     (uint32_t)((ROM_SIZEinWORDS) / STEPS_NUMBER)
 
  /* Constants necessary for Full March tests */
  #define RAM_START (uint32_t *)&__ICFEDIT_region_RAM_start__ 
  #define RAM_END   (uint32_t *)&__ICFEDIT_region_RAM_end__  
  #define RAM_BCKUP (uint32_t *)&__ICFEDIT_region_SRAM2_start__   
  
  /* Constants necessary for Transparent March tests */
  #define CLASS_B_START ((uint32_t *)(&__ICFEDIT_region_CLASSB_start__))
  #define CLASS_B_END ((uint32_t *)(&__ICFEDIT_region_CLASSB_end__))
  
  #define GotoCompilerStartUp() { __iar_program_start(); }

  #define REF_CRC32 __checksum
#
#endif  /* __IAR_SYSTEMS_ICC__ */

#if defined(__ARMCC_VERSION) || defined(__CC_ARM) /* KEIL Compiler V6 and V5 */

  /* This is the KEIL compiler entry point, usually executed right after reset */
  extern void Reset_Handler( void ); 
  extern const uint32_t __Check_Sum;
  extern void __main( void );
	
  /* Constants necessary for Flash CRC calculation (ROM_SIZE in byte) */
	/* byte-aligned addresses */
  #define ROM_START ((uint32_t *)0x08000000uL)	
  #define ROM_END   ((uint32_t *)&__Check_Sum)
  #define ROM_SIZE  ((uint32_t)ROM_END - (uint32_t)ROM_START)

  #define ROM_SIZEinWORDS (uint32_t) (ROM_SIZE/4u)
  #define STEPS_NUMBER          ((uint32_t)ROM_SIZE/64u)
  #define FLASH_BLOCK_WORDS     (uint32_t)((ROM_SIZEinWORDS)/STEPS_NUMBER)

  #define REF_CRC32 __Check_Sum
	
  /* Constants necessary for execution initial March test */
  #define RAM_START ((uint32_t *)0x20000000uL)
  #define RAM_END   ((uint32_t *)0x2002FEFFuL)
  #define RAM_BCKUP ((uint32_t *)0x20007F00uL)
	//#define RAM_BCKUP ((uint32_t *)0x40002500uL)  //MLE BACKUP are

  /* Constants necessary for execution of transparent run time March tests */
  #define CLASS_B_START ((uint32_t *)0x20000030uL)
  #define CLASS_B_END   ((uint32_t *)0x2000007BuL)
  #define CRC_FLAG	CRC->IDR
  #define GotoCompilerStartUp()  { CRC_FLAG = 0xAAu; __main(); } /* entry to init C before main() */

#else  /* GCC Compiler */
  
 #ifdef __GNUC__ /* GCC Compiler */

  /* This is the GCC compiler entry point, usually executed right after reset */
  extern void Reset_Handler( void );
  extern const uint32_t _Check_Sum;
  extern void __main( void );

  /* Constants necessary for Flash CRC calculation (ROM_SIZE in byte) */
	/* byte-aligned addresses */
  #define ROM_START ((uint32_t *)0x08000000uL)
  #define ROM_END   ((uint32_t *)&_Check_Sum)
  #define ROM_SIZE  ((uint32_t)ROM_END - (uint32_t)ROM_START)

  #define ROM_SIZEinWORDS (uint32_t) (ROM_SIZE/4u)
  #define STEPS_NUMBER          ((uint32_t)ROM_SIZE/64u)
  #define FLASH_BLOCK_WORDS     (uint32_t)((ROM_SIZEinWORDS)/STEPS_NUMBER)

  #define REF_CRC32 _Check_Sum

  /* Constants necessary for execution initial March test */
  #define RAM_START ((uint32_t *)0x20000000uL)
  #define RAM_END   ((uint32_t *)0x2002FF7FuL)
  #define RAM_BCKUP ((uint32_t *)0x2002FF80uL)

  /* Constants necessary for execution of transparent run time March tests */
  #define CLASS_B_START ((uint32_t *)0x20000030uL)
  #define CLASS_B_END   ((uint32_t *)0x2000007BuL)
  #define CRC_FLAG	CRC->IDR
  #define GotoCompilerStartUp()  { Startup_Copy_Handler(); SystemInit(); __libc_init_array(); main(); }    /* entry to  main() */
  #ifndef __HAL_RCC_DBGMCU_CLK_ENABLE
   #define __HAL_RCC_DBGMCU_CLK_ENABLE()  do { \
                                          } while(0)
  #endif
 #endif  /* __GNUC__ */
#endif /* MDK_ARM */
	
/* Exported functions ------------------------------------------------------- */
void FailSafePOR(void);
#if defined STL_EVAL_MODE
  void Eval_Board_HW_Init(void);
#endif
#if defined(STL_VERBOSE) || defined(STL_VERBOSE_POR)
  void USART_Configuration(void);
	void USART_Reconfiguration(void);
#endif /* STL_VERBOSE */
  
STLErrorStatus control_flow_check_point(uint32_t chck);

#endif /* __SELFTEST_PARAM_H */
/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

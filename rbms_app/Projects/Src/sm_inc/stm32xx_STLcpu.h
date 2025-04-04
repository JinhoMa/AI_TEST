/**
  ******************************************************************************
  * @file    stm32xx_STLcpu.h 
  * @author  MCD Application Team
  * @version V2.4.0
  * @date    28-Sept-2020
  * @brief   This file contains start-up CPU test function prototype
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 STMicroelectronics</center></h2>
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
#ifndef __CORTEXM3_CPU_TEST_H
#define __CORTEXM3_CPU_TEST_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Note the STLErrorStatus definition has to match with return of the
   following assembly routines */
STLErrorStatus STL_StartUpCPUTest(void);
STLErrorStatus STL_RunTimeCPUTest(void);

#endif /* __CORTEXM3_CPU_TEST_H */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

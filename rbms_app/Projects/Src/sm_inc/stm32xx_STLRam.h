/**
  ******************************************************************************
  * @file    stm32xx_STLRam.h 
  * @author  MCD Application Team
  * @version V2.4.0
  * @date    28-Sept-2020
  * @brief   This file contains prototype of the RAM functional test
  *          to be done at start-up.
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
#ifndef __STL_RAM_H
#define __STL_RAM_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//#define MARCH_STEP_SUCCESS ((uint32_t)0x00000001uL)
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
STLErrorStatus STL_FullRamMarchC(uint32_t *beg, uint32_t *end, uint32_t pat, uint32_t *bckup);
STLErrorStatus STL_TranspRamMarchCXStep(uint32_t *beg, uint32_t *buf, uint32_t pat);
void STL_TranspMarchInit(void);
ClassBTestStatus STL_TranspMarch(void);

#endif /* __STL_RAM_H */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

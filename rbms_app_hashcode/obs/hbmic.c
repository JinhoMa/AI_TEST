/**
    ******************************************************************************
    *     @file      hbmic.c 
    *     @author   SKI ESS Team 
    *     @version   V1.0
    *     @date      2014.11.05 
    *     @brief     HAL BMIC Module Source
    ******************************************************************************
    */
#include "obshal.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

#define delay1us()   { int j;for(j=0;j<100;j++); }
/** @addtogroup HAL
  *  @{
  */
  
  /**
      * @brief 이 소스는 BMIC Device를 포팅한 모듈이다.
      * BMIC칩은 CPU와 여러가지 통신 방식으로 통신을 할 수 있다.
      * MAXIM 칩의 경우 SPI 통신을 사용하게 된다.
      * 따라서, hBmicOpen() 시 내부적으로 SPI 모듈을 Open하게 된다.
      * H/W 구성 특성 에 따라서 CS Pin 과 ShutDown Pin을 조합 해서 사용하게 된다.
      * BMIC 칩에 대한  초기화 과정과 BMIC 구동 Configuration 들은 Application Layer에서 
      * 결정 하게 된다.
      */
  /** @addtogroup HAL_Bmic
    *   @{
    */
static uint32_t bmic_fd;    
/**
  * @brief BMIC 모듈을 Open한다.
  * BMIC 통신이 SPI 인 경우 이 함수 호출 시 BMIC와 연결된 SPI 모듈이 Open 된다.
  * @param NONE
  * @retval NONE
  */
void hBmicOpen(void)
{
    char *port = hSysProperty("bmic.spi.port");
    assert_param(strcmp(port,"")!=0);
    bmic_fd = hSpiOpen(port, NULL);
    assert_param(bmic_fd>0);
    HAL_BMIC_Init(); 
    return ;
}

/**
  * @brief BMIC shutdown pin을 Reset 한다.
  * 만일 BMIC shutdown pin이 없는 경우 포팅 과정에서 무시한다.
  * @param NONE
  * @retval NONE
  */
void hBmicShutDown(uint8_t ch)
{
    return;
}
/**
  * @brief BMIC shutdown pin을 set 한다.
  * 만일 BMIC shutdown pin이 없는 경우 포팅 과정에서 무시한다.
  * @param NONE
  * @retval NONE  
  */
void hBmicTurnOn(uint8_t ch)
{
    return;
}
/**
  * @brief BMIC CS를 Enable 한다.
  * @param NONE
  * @retval NONE  
  */
void hBmicEnable(uint8_t ch)
{
//SPI_NSS 설정 
    if(ch==0){
        HAL_GPIO_WritePin(BMIC_CS1_GPIO_PORT, BMIC_CS1_PIN, GPIO_PIN_RESET);
    }else{
        HAL_GPIO_WritePin(BMIC_CS2_GPIO_PORT, BMIC_CS2_PIN, GPIO_PIN_RESET);
    }
    delay1us();
}
/**
  * @brief BMIC CS를 Disable 한다.
  * @param NONE
  * @retval NONE  
  */
void hBmicDisable(uint8_t ch)
{
    if(ch==0){
        HAL_GPIO_WritePin(BMIC_CS1_GPIO_PORT, BMIC_CS1_PIN, GPIO_PIN_SET);
    }else{
        HAL_GPIO_WritePin(BMIC_CS2_GPIO_PORT, BMIC_CS2_PIN, GPIO_PIN_SET);
    }

    delay1us();
}
uint8_t hBmicWriteBuf(uint8_t ch, uint8_t* data, uint8_t len  )
{
    return hSpiSendBuf(bmic_fd, data , len);
}

uint8_t hBmicWrite(uint8_t data)
{
   
    return  hSpiSendByte(bmic_fd, data);
}

uint32_t hBmicRead(uint32_t addr)
{
    uint8_t data;
    hSpiSendByte(bmic_fd, addr);
    data = hSpiReceiveByte(bmic_fd);
	return data;
}

uint32_t hBmicReadBuf(uint8_t ch, uint8_t *data, uint8_t len)
{

    return  hSpiReceiveBuf(bmic_fd, data, len);
}


/**
  * @brief BMIC 모듈을 Close한다.
  * @param NONE
  * @retval NONE
  */
void hBmicClose(void)
{
}
/**
  * @brief BMIC 의 Shutdown PIN의 상태를 read 한다.
  * @param[out] shutdown  PIN 정보, 0 or 1값이다.
  * @retval NONE
  */
void hBmicGpioRead(uint8_t  ch, int *shutdown)
{
    return;
}



    /**
      * @}
      */
// End of HAL Group
/**
  * @}
  */


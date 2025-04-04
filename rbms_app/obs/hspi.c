/**
    ******************************************************************************
    *     @file      hspi.c 
    *     @author SKI ESS Team   
    *     @version   V1.0
    *     @date      2014.11.05 
    *     @brief     HAL SPI Module Source
    ******************************************************************************
    */
#include "obshal.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

typedef struct _HSpi{
  SPI_HandleTypeDef handle;
  uint8_t(*send)(uint8_t);
  uint8_t(*recv)(void);
} HSpi;

HSpi spi_fd[3];

  

/** @addtogroup HAL
  *  @{
  */
  
  /**
      * @brief 이 소스는 SPI Device를 포팅한 모듈이다.
      *  SPI 모듈은 Application Layer에서 직접 사용되지 않고 
      *  SPI1 은 BMIC 모듈에서 Open 되고, SPI2는 EEPROM 모듈에서 Open 된다.
      */
  /** @addtogroup HAL_Spi
    *   @{
    */

/**
    * @brief SPI 모듈을 초기화 한다.
    * @param NONE
    * @retval NONE
    */
void hSpiInit(void){
    
}


/**
  * @brief SPI 포트를 주어진 전송속도로 Open한다.
  * @param[in] port SPI포트의 문자열 이름, "SPI1", "SPI2",..
  * @param[in] baudrate 전송속도 값 - Default: NULL
  * @retval 0이상값: SPI 디바이스 식별자
  * @retval 음수: 에러정보
  */
int32_t hSpiOpen(char *port, int32_t baudrate){
    HSpi *spi;
    if(strcmp(port,"SPI1")==0){
        spi = &spi_fd[0];
        spi->handle.Instance = BLE_SPI;
        spi->send = NULL;
        spi->recv = NULL;
        spi->handle.Init.Mode               = SPI_MODE_MASTER; 
        spi->handle.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_64; //SPI_BAUDRATEPRESCALER_256; 
        spi->handle.Init.Direction          = SPI_DIRECTION_2LINES; 
        spi->handle.Init.DataSize           = SPI_DATASIZE_8BIT; 
        spi->handle.Init.CLKPolarity = SPI_POLARITY_LOW;
        spi->handle.Init.CLKPhase = SPI_PHASE_1EDGE;
        spi->handle.Init.FirstBit           = SPI_FIRSTBIT_MSB; 
        spi->handle.Init.NSS                = SPI_NSS_SOFT; 
        spi->handle.Init.TIMode             = SPI_TIMODE_DISABLED; 
        spi->handle.Init.CRCPolynomial      = 7; 
        spi->handle.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLED; 
       // spi->handle.Init.NSSPMode = SPI_NSS_PULSE_DISABLED;
             
        if (HAL_SPI_Init((SPI_HandleTypeDef *)spi) != HAL_OK)
        {
            printf ("ERROR: Error in initialising SPI2 \n");
        }
        __HAL_SPI_ENABLE((SPI_HandleTypeDef *)spi);

        return (int32_t)spi;
        }
     else if (strcmp(port, "SPI2") == 0)
    {
        spi = &spi_fd[2];
        spi->handle.Instance= EEP_SPI;
        spi->send = NULL;
        spi->recv = NULL;
        spi->handle.Init.Mode               = SPI_MODE_MASTER; 
        spi->handle.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_2; //SPI_BAUDRATEPRESCALER_256; 
        spi->handle.Init.Direction          = SPI_DIRECTION_2LINES; 
        spi->handle.Init.DataSize           = SPI_DATASIZE_8BIT; 
        spi->handle.Init.CLKPolarity = SPI_POLARITY_HIGH;
        spi->handle.Init.CLKPhase = SPI_PHASE_2EDGE;
        spi->handle.Init.FirstBit           = SPI_FIRSTBIT_MSB; 
        spi->handle.Init.NSS                = SPI_NSS_SOFT; 
        spi->handle.Init.TIMode             = SPI_TIMODE_DISABLED; 
        spi->handle.Init.CRCPolynomial      = 7; 
        spi->handle.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLED; 
       // spi->handle.Init.NSSPMode = SPI_NSS_PULSE_DISABLED;
             
        if (HAL_SPI_Init((SPI_HandleTypeDef *)spi) != HAL_OK)
        {
            printf ("ERROR: Error in initialising SPI3 \n");
        }
        __HAL_SPI_ENABLE((SPI_HandleTypeDef *)spi);

        return (int32_t)spi;
    }
    return -1;
}

int32_t hSpiGetHandle(char *port){
    if( strcmp(port, "SPI1")==0){
        return (uint32_t)&spi_fd[0];
    }else if(strcmp(port, "SPI2")==0){
        return (uint32_t)&spi_fd[1];
    }else if(strcmp(port,"SPI3")==0){
        return (uint32_t)&spi_fd[2];
    }
    return 0;
}

#if 0

uint8_t hSpiSendBuf(int32_t fd, uint8_t *data, uint8_t len){
    HSpi *spi = (HSpi*)fd;
	uint8_t da;
    uint8_t ret;
    if(spi->send != NULL)
    {
        //da= spi->send(data);
    }
    else
    {
        ret = HAL_SPI_TransmitReceive((SPI_HandleTypeDef *)spi, data, data, len, 1); 
        if(ret != HAL_OK){
            da = 0xff;
        }
    }

    return da;
}

uint8_t hSpiSendByte(int32_t fd, uint8_t data)
{
    HSpi *spi = (HSpi*)fd;
	uint8_t da;
    uint8_t ret;
    if(spi->send != NULL)
    {
        da= spi->send(data);
    }
    else
    {
        ret = HAL_SPI_Transmit((SPI_HandleTypeDef *)spi, &data,1, 1);	 
        if(ret != HAL_OK){
            da = 0xff;
        }
    }

    return da;
}
uint8_t hSpiReceiveBuf(int32_t fd, uint8_t* buf, uint8_t size)
{
    HSpi *spi = (HSpi*)fd;
	uint8_t data=0;
    uint8_t ret;
    if(spi->recv != NULL)
    {
        ret = spi->recv();
    }
    else
    {
        ret = HAL_SPI_Receive((SPI_HandleTypeDef *)spi, buf,size, 10);	
        if(ret != HAL_OK){
            ret = 0xff;
        }

    }
    return ret;
}

uint8_t hSpiReceiveByte(int32_t fd)
{
    HSpi *spi = (HSpi*)fd;
	uint8_t data=0;
    uint8_t da=0;
    uint8_t ret;
    if(spi->recv != NULL)
    {
        da= spi->recv();
    }
    else
    {
        ret = HAL_SPI_Receive((SPI_HandleTypeDef *)spi, &da,1, 1);	
        if(ret != HAL_OK){
            da = 0xff;
        }

    }
    return da;
}

#else
uint8_t hSpiSendBuf(int32_t fd, uint8_t *data, uint8_t len){
    HSpi *spi = (HSpi*)fd;
	uint8_t da= HAL_OK;
    uint8_t ret;
    if(spi->send != NULL)
    {
        //da= spi->send(data);
    }
    else
    {
        ret = HAL_SPI_TransmitReceive((SPI_HandleTypeDef *)spi, data, data, len, 10); 
        if(ret != HAL_OK){
            da = ret;
        }
    }

    return da;
}
uint8_t hSpiReceiveBuf(int32_t fd, uint8_t* buf, uint8_t size)
{
    HSpi *spi = (HSpi*)fd;
    uint8_t ret;
    if(spi->recv != NULL)
    {
        ret = spi->recv();
    }
    else
    {
        ret = HAL_SPI_Receive((SPI_HandleTypeDef *)spi, buf,size, 10);	
        if(ret != HAL_OK){
            ret = 0xff;
        }

    }
    return ret;
}
uint8_t hSpiReceiveByte(int32_t fd)
{
    HSpi *spi = (HSpi*)fd;
	uint8_t data=0;
    uint8_t da=0;
    uint8_t ret;
    if(spi->recv != NULL)
    {
        da= spi->recv();
    }
    else
    {
        ret = HAL_SPI_TransmitReceive((SPI_HandleTypeDef *)spi, &data, &da,1, 5);	
        if(ret != HAL_OK){
            da = 0xff;
        }

    }
    return da;
}

uint8_t hSpiSendByte(int32_t fd, uint8_t data)
{
    HSpi *spi = (HSpi*)fd;
	uint8_t da = 0;
    int ret;
    if(spi->send != NULL)
    {
        da= spi->send(data);
    }
    else
    {
        ret = HAL_SPI_TransmitReceive((SPI_HandleTypeDef *)spi, &data, &da,1, 5);	 
        if(ret != HAL_OK){
            return ret;
        }
    }

    return 0;
}


#endif
#if 0
uint8_t hSpi1SendByte(uint8_t data)
{  
  	SPI_TypeDef *SPIx = ETH_SPI;
    uint8_t cnt=0;
    while ((SPIx->SR & SPI_FLAG_TXE) == (uint16_t)RESET);
//    __disable_irq();
    SPIx->DR = data;
    while ((SPIx->SR & SPI_FLAG_RXNE) == (uint16_t)RESET){
        if(++cnt > 10){
//            __enable_irq();
            printf("---->\n");
            return 0;
        }
    }
    data = (uint8_t)SPIx->DR ;
//    __enable_irq();
    return data;
}

/**
  * @brief SPI 1포트로 data를 read 한다.
  * @retval data 반환 되는 data 값
  */
uint8_t hSpi1ReceiveByte(void)
{
	__IO uint8_t data = 0;
    uint8_t cnt=0;
    SPI_TypeDef *SPIx = ETH_SPI;
    while ((SPIx->SR & SPI_FLAG_TXE) == (uint16_t)RESET);
//    __disable_irq();
    SPIx->DR = data;
    while ((SPIx->SR & SPI_FLAG_RXNE) == (uint16_t)RESET ){
        if(++cnt > 10){
//            __enable_irq();
            printf("****>\n");
            return 0;
        }
    }
    data = (uint8_t)SPIx->DR ;
//    __enable_irq();
    return data;
}

/**
  * @brief SPI 2포트로 data를 write 한다.
  * @param[in] data write할 데이터
  * @retval data write 후 반환 되는 상태 값
  */
uint8_t hSpi2SendByte(uint8_t data)
{
    SPI_TypeDef *SPIx = BMIC_SPI;
    uint8_t cnt=0;
    while ((SPIx->SR & SPI_FLAG_TXE) == (uint16_t)RESET);
    __disable_irq();
    SPIx->DR = data;
    while ((SPIx->SR & SPI_FLAG_RXNE) == (uint16_t)RESET){
        if(++cnt > 10){
            __enable_irq();
            printf("---->\n");
            return 0;
        }
    }
    data = (uint8_t)SPIx->DR ;
    __enable_irq();
    return data;

}

/**
  * @brief SPI 2포트로 data를 read 한다.
  * @retval data 반환 되는 data 값
  */
uint8_t hSpi2ReceiveByte(void)
{
    __IO uint8_t data = 0;
    uint8_t cnt=0;
    SPI_TypeDef *SPIx = BMIC_SPI;
    while ((SPIx->SR & SPI_FLAG_TXE) == (uint16_t)RESET);
    __disable_irq();
    SPIx->DR = data;
    while ((SPIx->SR & SPI_FLAG_RXNE) == (uint16_t)RESET ){
        if(++cnt > 10){
            __enable_irq();
            printf("****>\n");
            return 0;
        }
    }
    data = (uint8_t)SPIx->DR ;
    __enable_irq();
    return data;

}
#endif


#include "obshal.h"
#include "main.h"
#include <string.h>

#define S390_DEVCODE            0x6     /* 0110 */


#define S390_ADDR               (S390_DEVCODE << 4)




typedef struct _HI2C{
    I2C_HandleTypeDef handle;
    uint8_t rcvcnt;
    uint8_t *buf;
} H_I2C;

H_I2C  i2c;
/* I2C TIMING Register define when I2C clock source is PCLK1 (51Mhz) */
/* I2C TIMING is ( Frequency 500KHz, rise time 50us, fall time- = 30us) */
#define I2C_TIMING      0x20C145FF//0xC0110F14

uint8_t i2cbuf[8];
extern uint8_t RtcRead;
/** @addtogroup HAL
  *  @{
  */
  
  /**
      * @brief 이 소스는 I2C Device를 포팅한 모듈이다.
      *  I2C 모듈은 Application Layer에서 직접 사용되지 않고 
      *  I2C2 은 BMIC 모듈에서 Open 된다.
      */
  /** @addtogroup HAL_Spi
    *   @{
    */

/**
    * @brief I2C 모듈을 초기화 한다.
    * @param NONE
    * @retval NONE
    */
void hI2cInit(void){
}

/**
  * @brief I2C 포트를 주어진 전송속도로 Open한다.
  * @param[in] port I2C포트의 문자열 이름, "I2C1", "I2C2",..
  * @param[in] baudrate 전송속도 값 - Default: NULL
  * @retval 0이상값: I2C 디바이스 식별자
  * @retval 음수: 에러정보
  */
int32_t hI2cOpen(char *port, int32_t baudrate){
    
    if(strcmp(port,"I2C1")==0){
        i2c.handle.Instance             = I2C1;
        i2c.handle.Init.Timing          = I2C_TIMING;
        i2c.handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
        i2c.handle.Init.OwnAddress1     = 0x00;
        i2c.handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
        i2c.handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
        i2c.handle.Init.OwnAddress2     = 0x00;
        i2c.handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
        i2c.handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
        i2c.handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
        
        if (HAL_I2C_Init(&i2c.handle) != HAL_OK)
        {
          /* Initialization Error */
        }
        HAL_I2CEx_ConfigAnalogFilter(&i2c.handle, I2C_ANALOGFILTER_ENABLE);
        return (int32_t) &i2c;

    }
    return -1;
}

int32_t  hI2cReOpen(int32_t fd){
    int32_t ret;
    ret = HAL_I2C_DeInit(&i2c.handle) ;
    if(ret != HAL_OK){
        return -1;
    }
    i2c.handle.Instance             = I2C1;
    i2c.handle.Init.Timing          = I2C_TIMING;
    i2c.handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2c.handle.Init.OwnAddress1     = 0x00;
    i2c.handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    i2c.handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2c.handle.Init.OwnAddress2     = 0x00;
    i2c.handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    i2c.handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2c.handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    
    if (HAL_I2C_Init(&i2c.handle) != HAL_OK)
    {
      /* Initialization Error */
    }
    HAL_I2CEx_ConfigAnalogFilter(&i2c.handle, I2C_ANALOGFILTER_ENABLE);
    return (int32_t) &i2c;
}

int32_t hI2cGetHandle(char *data)
{
    return (int32_t) &i2c;
}

const uint8_t nibbletab[] = {
        /* 0x0 0000 -> 0000 */  0x0,
        /* 0x1 0001 -> 1000 */  0x8,
        /* 0x2 0010 -> 0100 */  0x4,
        /* 0x3 0011 -> 1100 */  0xc,
        /* 0x4 0100 -> 0010 */  0x2,
        /* 0x5 0101 -> 1010 */  0xa,
        /* 0x6 0110 -> 0110 */  0x6,
        /* 0x7 0111 -> 1110 */  0xe,
        /* 0x8 1000 -> 0001 */  0x1,
        /* 0x9 1001 -> 1001 */  0x9,
        /* 0xa 1010 -> 0101 */  0x5,
        /* 0xb 1011 -> 1101 */  0xd,
        /* 0xc 1100 -> 0011 */  0x3,
        /* 0xd 1101 -> 1011 */  0xb,
         /* 0xe 1110 -> 0111 */ 0x7,
        /* 0xf 1111 -> 1111 */  0xf, 
};

 
static uint8_t bitreverse(uint8_t x)
{
    return (nibbletab[x & 0xf] << 4) | nibbletab[x >> 4];
}
 

/**
  * @brief I2C 1포트로 data를 write 한다.
  * @param[in] data write할 데이터
  * @retval data write 후 반환 되는 상태 값
  */
int32_t  hI2cWriteBlock(int32_t fd, uint8_t addr, uint8_t *data, uint16_t size)
{
    int i;
    HAL_StatusTypeDef st;
    uint16_t devaddr = S390_ADDR | addr | 0x0 ;
    for(i=0; i< size; i++){
        *(data+i) = bitreverse(*(data+i));
    }
    st = HAL_I2C_Master_Transmit((I2C_HandleTypeDef*) &i2c.handle, devaddr , data , size,  1);
    if(st == HAL_OK)
      return size;
    else 
      return -1;
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    int i;
    if(  hi2c->XferSize == 0){
        for (i = 0; i <  7; ++i){
            *(i2c.buf+i) = bitreverse(*(i2cbuf+i));
            RtcRead = 1;
        }
    }
}

void  HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    RtcRead = 0;
    //printf("ic2 error\n");
}

int32_t hI2cReadNonBlock(int fd, uint8_t addr, uint8_t *data, uint16_t size)
{
    int i;
    HAL_StatusTypeDef st;
    uint16_t devaddr = S390_ADDR | addr | 0x1 ;
    i2c.buf = data;
    st =HAL_I2C_Master_Receive_IT((I2C_HandleTypeDef *)&i2c.handle, devaddr , i2cbuf, size);
    if(st == HAL_OK){
        return size;
    }
    else{
        return -1;
    }

}


/**
  * @brief I2C 1포트로 data를 read 한다.
  * @retval data 반환 되는 data 값
  */
int32_t hI2cReadBlock(int fd, uint8_t addr, uint8_t *data, uint16_t size)
{
    int i;
    HAL_StatusTypeDef st;
    uint16_t devaddr = S390_ADDR | addr | 0x1 ;
    st= HAL_I2C_Master_Receive((I2C_HandleTypeDef *)&i2c.handle, devaddr , data, size, 1);
    if(st == HAL_OK){
        for (i = 0; i < size; ++i){
            *(data+i) = bitreverse(*(data+i));
        }
        return size;
    }
    else{
        return -1;
}
}



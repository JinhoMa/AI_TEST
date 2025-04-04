/**
    ******************************************************************************
    *     @file      heeprom.c 
    *     @author    SKI ESS Team
    *     @version   V1.0
    *     @date      2014.11.05 
    *     @brief     HAL EEPROM Module Source
    ******************************************************************************
    */
#include "obshal.h"
#include "htypes.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

#define EEPROM_WREN_INSTRUCTION_CMD 0x06 //0b00000110  // Write Enable
#define EEPROM_WRDI_INSTRUCTION_CMD 0x04 //0b00000100  // Write Disable
#define EEPROM_RDSR_INSTRUCTION_CMD 0x05 //0b00000101  // Read Status Register
#define EEPROM_WRSR_INSTRUCTION_CMD 0x01 //0b00000001  // Write Status Register
#define EEPROM_READ_INSTRUCTION_CMD 0x03 //0b00000011  // Read from Memory Array
#define EEPROM_WRITE_INSTRUCTION_CMD 0x02 //0b00000010 // Write to Memory Array
#define M95M01PAGESIZE   256

#define EEP_CS_ENABLE()  {   BSP_EEP_CS_ENABLE();  hTimDelay(5); }
#define EEP_CS_DISABLE() {   BSP_EEP_CS_DISABLE();  }

#define wrEnable()    { EEP_CS_ENABLE(); hSpiSendByte(eep_fd, EEPROM_WREN_INSTRUCTION_CMD ) ;  EEP_CS_DISABLE(); }        
#define wrDisable()   { EEP_CS_ENABLE(); hSpiSendByte(eep_fd, EEPROM_WRDI_INSTRUCTION_CMD ) ;  EEP_CS_DISABLE(); }

/** @addtogroup HAL
  *  @{
  */
  
  /**
      * @brief 이 소스는 EEPROM Device를 포팅한 모듈이다.
      * EEPROM 과 CPU간 통신 방식 중 SPI 통신을 통해서 EEPROM과 연결되어 있다.
      * EEPROM은 비휘발성 메모리 이기 문에 전원이 꺼지더라도 저장된 데이타는 사라지지 않는다.
      * BMS Application에서 비휘발성 데이터는 EEPROM을 사용하게 된다.
      */
  /** @addtogroup HAL_Eeprom
    *   @{
    */
int32_t eep_fd;    

/**
  * @brief EEPROM 모듈을 Open 한다.
  * EEPROM 모듈내에서 통신을 SPI 통신 하는 경우 해당 포트의 SPI모듈이 Open 된다.
  * @param NONE
  * @retval NONE
  */
void hEprOpen(void)
{
    char *port = hSysProperty("eeprom.spi.port");
    if(strcmp(port,"")==0) return;
    eep_fd = hSpiOpen(port, NULL);
    return ;
}

/**
  * @brief EEPROM CS 가 Enable 된다.
  * @param NONE
  * @retval NONE
  */
void hEprEnable(void)
{
   // EEP_CS_ENABLE();
   //     hSpiSendByte(eep_fd, EEPROM_WREN_INSTRUCTION_CMD ) ; 
  //  EEP_CS_DISABLE();
}

/**
  * @brief EEPROM CS가 Disable 된다.
  * @param NONE
  * @retval NONE
  */
void hEprDisable(void)
{
   // EEP_CS_ENABLE();
   //     hSpiSendByte(eep_fd, EEPROM_WRDI_INSTRUCTION_CMD ) ; 
   // EEP_CS_DISABLE();
}


static void waitEprStanbyState(void)
{
    uint8_t st;
    
    /*!< Select the EEPROM: Chip Select low */
    EEP_CS_ENABLE();
    
    /*!< Send "Read Status Register" instruction */
    hSpiSendByte(eep_fd, EEPROM_RDSR_INSTRUCTION_CMD);
    
    /*!< Loop as long as the memory is busy with a write cycle */
    do
    {
      /*!< Send a dummy byte to generate the clock needed by the EEPROM
      and put the value of the status register in EEPROM Status variable */
      st = hSpiReceiveByte(eep_fd);
    
    }
    while ((st & 0x01) == SET); /* Write in progress */
    
    /*!< Deselect the EEPROM: Chip Select high */
    EEP_CS_DISABLE();

}
/**
  * @brief EEPROM 의 해당 주소에 데이터를 write 한다.
  * @param[in] addr Write하고자 하는 주소
  * @param[in] data 1byte 데이터
  * @retval NONE
  */
void hEprWriteByte(uint32_t addr, uint8_t data)
{
	uint8_t addrH, addrM, addrL;

	/* Parse address */
	addrH = (uint8_t)((addr & 0xFF0000) >> 16);
	addrM = (uint8_t)((addr & 0xFF00) >> 8);
	addrL = (uint8_t)(addr  & 0x00FF);
    wrEnable();
    EEP_CS_ENABLE();
        hSpiSendByte(eep_fd, EEPROM_WRITE_INSTRUCTION_CMD ) ;         
        hSpiSendByte(eep_fd, addrH);
        hSpiSendByte(eep_fd, addrM);
        hSpiSendByte(eep_fd, addrL);
        hSpiSendByte(eep_fd, data);
    EEP_CS_DISABLE();
    waitEprStanbyState();
    wrDisable();
}
/**
  * @brief EEPROM의 해당 주소의 데이터를 read한다.
  * @param[in] addr EEPROM의 주소
  * @retval 0이상 값 
  */
uint8_t hEprReadByte(uint32_t addr)
{
	uint8_t addrH, addrM, addrL;
    uint8_t data=0;
	/* Parse address */
	addrH = (uint8_t)((addr & 0xFF0000) >> 16);
	addrM = (uint8_t)((addr & 0xFF00) >> 8);
	addrL = (uint8_t)(addr  & 0x00FF);

    EEP_CS_ENABLE();
        hSpiSendByte(eep_fd, EEPROM_READ_INSTRUCTION_CMD ) ; 
        hSpiSendByte(eep_fd, addrH);
        hSpiSendByte(eep_fd, addrM);
        hSpiSendByte(eep_fd, addrL);
        data = hSpiReceiveByte(eep_fd);
        //data = hSpiReceiveByte(eep_fd);
    EEP_CS_DISABLE();
	return data;
}

static void writeEprPage(uint32_t addr, uint8_t *data, uint32_t size)
{
	uint8_t addrH, addrM, addrL;
    uint8_t ret;
	/* Parse address */
	addrH = (uint8_t)((addr & 0xFF0000) >> 16);
	addrM = (uint8_t)((addr & 0xFF00) >> 8);
	addrL = (uint8_t)(addr  & 0x00FF);
    wrEnable();
    EEP_CS_ENABLE();
        ret = hSpiSendByte(eep_fd, EEPROM_WRITE_INSTRUCTION_CMD ) ;         
        if(ret != 0) printf("ret1(%d)\n", ret);
        ret = hSpiSendByte(eep_fd, addrH);
        if(ret != 0) printf("ret2(%d)\n", ret);
        ret = hSpiSendByte(eep_fd, addrM);
        if(ret != 0) printf("ret3(%d)\n", ret);
        ret = hSpiSendByte(eep_fd, addrL);
        if(ret != 0) printf("ret4(%d)\n", ret);
        while(size--){
            ret = hSpiSendByte(eep_fd, *(data++));
            if(ret != 0) printf("ret5(%d)\n", ret);
        }
        
    EEP_CS_DISABLE();
    waitEprStanbyState();
    wrDisable();

}

void hEprWrite(uint32_t addr, uint8_t *data, uint32_t size)
{

    uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
    uint16_t num;
    Addr = addr % M95M01PAGESIZE;
    count = M95M01PAGESIZE - Addr;
    NumOfPage =  size / M95M01PAGESIZE;
    NumOfSingle = size % M95M01PAGESIZE;

    if (Addr == 0) /*!< addr is M95M01PAGESIZE aligned  */
    {
      if (NumOfPage == 0) /*!< size < M95M01PAGESIZE */
      {
        num = size;      
        writeEprPage(addr, data, num);
      }
      else /*!< size > M95M01PAGESIZE */
      {
        while (NumOfPage--)
        {
          num = M95M01PAGESIZE;
          writeEprPage(addr, data, num);
          addr +=  M95M01PAGESIZE;
          data += M95M01PAGESIZE;
        }
        
        num = NumOfSingle;
        writeEprPage(addr, data, num);
      }
    }
    else /*!< addr is not M95M01PAGESIZE aligned  */
    {
      if (NumOfPage == 0) /*!< size < M95M01PAGESIZE */
      {
        if (NumOfSingle > count) /*!< (size + addr) > M95M01PAGESIZE */
        {
          temp = NumOfSingle - count;
          num = count;
          writeEprPage(addr, data, num);
          addr +=  count;
          data += count;
          
          num = temp;
          writeEprPage(addr, data, num);
        }
        else
        {
          num = size; 
          writeEprPage(addr, data, num);
        }
      }
      else /*!< size > M95M01PAGESIZE */
      {
        size -= count;
        NumOfPage =  size / M95M01PAGESIZE;
        NumOfSingle = size % M95M01PAGESIZE;

        num = count;
          
          writeEprPage(addr, data, num);
        addr +=  count;
        data += count;

        while (NumOfPage--)
        {
          num = M95M01PAGESIZE;
          
          writeEprPage(addr, data, num);
          addr +=  M95M01PAGESIZE;
          data += M95M01PAGESIZE;
        }

        if (NumOfSingle != 0)
        {
          num = NumOfSingle;
          
          writeEprPage(addr, data, num);
        }
      }
    }
}

void hEprRead(uint32_t addr, uint8_t *data, uint32_t size)
{
	uint8_t addrH, addrM, addrL;
	/* Parse address */
	addrH = (uint8_t)((addr & 0xFF0000) >> 16);
	addrM = (uint8_t)((addr & 0xFF00) >> 8);
	addrL = (uint8_t)(addr  & 0x00FF);

    EEP_CS_ENABLE();
        hSpiSendByte(eep_fd, EEPROM_READ_INSTRUCTION_CMD ) ; 
        hSpiSendByte(eep_fd, addrH);
        hSpiSendByte(eep_fd, addrM);
        hSpiSendByte(eep_fd, addrL);
        while(size--){
            *(data++) = hSpiReceiveByte(eep_fd);
        }
        //data = hSpiReceiveByte(eep_fd);
    EEP_CS_DISABLE();
}

/**
  * @brief EEPROM 모듈을 Close한다.
  * @param NONE
  * @retval NONE
  */
void hEprClose(void)
{
}

/**
  * deprecated 함수, only shell use
  */
void hEprGpioRead(int *hold, int *wp)
{
}
/**
  * deprecated 함수, only shell use
  */
void hEprGpioWrite(int *hold , int *wp)
{
}


    /**
      * @}
      */
// End of HAL Group
/**
  * @}
  */


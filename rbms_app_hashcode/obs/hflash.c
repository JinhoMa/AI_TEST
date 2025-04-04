/**
    ******************************************************************************
    *     @file      hflash.c 
    *     @author    SKI ESS Team
    *     @version   V1.0
    *     @date      2014.11.05 
    *     @brief     HAL Flash Module Source
    ******************************************************************************
    */
#include "obsHal.h"
#include "main.h"
#include <stdio.h>

#if defined(__CC_ARM)
#pragma arm section code="$RAM_BASE"
#elif defined(__ICCARM__)
#endif

/** @addtogroup HAL
  *  @{
  */
  
  /**
      * @brief 이 소스는 Flash Device를 포팅한 모듈이다.
      * 이 소스는 RAM 영역에 놓여지게 된다.
      * Program을 하기 전에 반드시 Erase 한 후에 Prgram 해야 한다.
      * Verify 기능은 옵션 기능이다.
      * STM32F10x 의  Flash 영역에 따라서 Bank 개수가 틀리기 때문에
      * Flash Size가 바뀌는 경우에서는 새롭게 구현해야 한다.
      */
  /** @addtogroup HAL_Flash
    *   @{
    */
    
         /** @defgroup HAL_Flash_Private_Types
                * @{
                */

struct CpuInternalFlashTable {
    unsigned int sector;
    unsigned int address;
    int size;
} ;
   /**
             * @}
             */

        /** @defgroup HAL_Flash_Private_Defines
               * @{
               */


#define THISCPU         0xFF

#define SECTOR0_SIZE    32*1024
#define SECTOR1_SIZE    SECTOR0_SIZE
#define SECTOR2_SIZE    SECTOR0_SIZE
#define SECTOR3_SIZE    SECTOR0_SIZE
#define SECTOR4_SIZE    128*1024
#define SECTOR5_SIZE    256*1024
//...

#define SECTOR0_BASE        FLASH_BASE
#define SECTOR1_BASE        SECTOR0_BASE+SECTOR0_SIZE
#define SECTOR2_BASE        SECTOR1_BASE+SECTOR1_SIZE
#define SECTOR3_BASE        SECTOR2_BASE+SECTOR2_SIZE
#define SECTOR4_BASE        SECTOR3_BASE+SECTOR3_SIZE
#define SECTOR5_BASE        SECTOR4_BASE+SECTOR4_SIZE
//..

#define SECTORn     6



         /**
              * @}
              */
       /** @defgroup HAL_Flash_Private_Variables
           * @{
           */

static struct CpuInternalFlashTable sector_db[SECTORn+1]=
{
    { 0, SECTOR0_BASE, SECTOR0_SIZE},
    { 1, SECTOR1_BASE, SECTOR1_SIZE},
    { 2, SECTOR2_BASE, SECTOR2_SIZE},
    { 3, SECTOR3_BASE, SECTOR3_SIZE},
    { 4, SECTOR4_BASE, SECTOR4_SIZE},
    { 5, SECTOR5_BASE, SECTOR5_SIZE},
    { 6, SECTOR5_BASE+SECTOR5_SIZE, 0}
           
};
          /**
                * @}
                */
      /** @defgroup HAL_Flash_Private_Functions
          * @{
          */

          /**
                * @}
                */

/**
  * @brief Flash Module을 초기화 한다.
  * @param NONE
  * @retval NONE
  */
void hFlashInit(void)
{
	#if 0
    __HAL_FLASH_INSTRUCTION_CACHE_RESET();
    __HAL_FLASH_DATA_CACHE_RESET();
	#endif
}

static int get_sector(unsigned int address)
{
    int i;
    if(address >= (sector_db[ SECTORn-1 ].address +sector_db[ SECTORn-1 ].size)) return -1;
    if(address < SECTOR0_BASE) return -1;
    for( i=0; i<SECTORn;i++)
    {
        if(sector_db[i].address <= address && address < sector_db[i+1].address)
        {
            return sector_db[i].sector;
        }
    }
		return -1;
}

/**
  * @brief Flash가 Program 가능한 상태로 변경된다.
  * 이 함수는 Flash의 같은 Bank 내에서 호출 될 수 없다.
  * @param NONE
  * @retval NONE
  */
void hFlashWriteEnable(void)
{
    __disable_irq();
    HAL_FLASH_Unlock();
}
/**
  * @brief Flash가 Code 수행 모드로 변경된다.
  * @param NONE
  * @retval NONE
  */
void hFlashWriteDisable(void)
{
    HAL_FLASH_Lock();
    __enable_irq();
}
/**
  * @brief flash의 특정 주소에 해당 버퍼의 데이터를 Program 하게 된다.
  * Program 하기 전에 주어진 주소 부터 Program 할 size만큼 Erase되어야 하며,
  * 이 함수가 호출 되기 전에 hFlashWriteEnable() 가 호출 되어야한다.
  * @param[in] address program 하기 위한 시작 번지
  * @param[in] buf program 하기 위한 데이터의 버퍼 포인터
  * @param[in] len 버퍼의 사이즈
  * @retval 0보다 큰값 : Program 한 데이터의 사이즈
  * @retval 음수 : 에러정보
  */ 
int32_t hFlashProgram(uint32_t address, uint8_t * buf, int32_t len)
{
    int i;
    unsigned short temp;

    if(address%2) return E_INVALID;
    
    for( i=0;i<len; i +=2)
    {
        if ((i+1)==len){
            temp =  0xFF00 |*(buf+i);
        }else{
            temp = ((unsigned short )*(buf+i+1)<<8) | *(buf+i);
        }
        if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address+i, (uint64_t) temp ) != HAL_OK){
            return E_ERROR;
        }
    }
		return 0;
}

/**
  * @brief 특정 주소의 Bank를 시작으로 주어진 Size만큼 Erase 한다.
  * @param[in] addr Erase 하기 위한 시작 번지
  * @param[in] size Erase 할 Size
  * @retval Erase한 사이즈 
  * @retval  음수: 에러정보 
  */
int32_t hFlashErase(uint32_t addr, int32_t size)
{
    int first_sector, end_sector;
    unsigned int SectorError;
	  //int mass_mode;
    FLASH_EraseInitTypeDef erase;
    if ( (first_sector = get_sector(addr)) < 0)
        return E_INVALID;
    if( (end_sector = get_sector(addr+size-1)) < 0)
        return E_INVALID;
    if(size == -1){ //erase all
        //mass_mode = TRUE;
    }
    
    /* Fill EraseInit structure*/
    erase.TypeErase = FLASH_TYPEERASE_SECTORS ;
    erase.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
    erase.Sector= first_sector;
    erase.NbSectors = end_sector - first_sector + 1 ;

    if (HAL_FLASHEx_Erase(&erase, &SectorError) != HAL_OK )
    {
        return E_ERROR;
    }
    hLogProc(LOG_FLASH, "erase sector [ 0x%x ~0x%x ]\n", sector_db[first_sector].address, (sector_db[end_sector].address+sector_db[end_sector].size));
    return 0;
}

/**
  * @brief 특정 주소 부터 데이터 버퍼의 담겨져 있는 데이터와 동일 한지 검증한다.
  * @param[in] address 검증 할 주소의 시작 번지
  * @param[in] buf 주소 영역과 비교할 데이터의 버퍼 포인터
  * @param[in] len 데이터의 버퍼 사이즈
  * @retval  0: 성공 
  * @retval  음수: 에러정보 
  */
int32_t hFlashVerify(uint32_t address, uint8_t * buf, int32_t len)
{
    int  i;
    uint32_t *p = (uint32_t*) address;
    for(i=0; i<len; i++)
    {
       if(  *(buf+i) != *(p+i) )
       {
        return E_ERROR;
       }
    }
    return 0;
}

    /**
      * @}
      */
// End of HAL Group
/**
  * @}
  */



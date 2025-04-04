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
      * @brief �� �ҽ��� Flash Device�� ������ ����̴�.
      * �� �ҽ��� RAM ������ �������� �ȴ�.
      * Program�� �ϱ� ���� �ݵ�� Erase �� �Ŀ� Prgram �ؾ� �Ѵ�.
      * Verify ����� �ɼ� ����̴�.
      * STM32F10x ��  Flash ������ ���� Bank ������ Ʋ���� ������
      * Flash Size�� �ٲ�� ��쿡���� ���Ӱ� �����ؾ� �Ѵ�.
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

static FLASH_EraseInitTypeDef EraseInitStruct;
static struct CpuInternalFlashTable sector_db[SECTORn]=
{
    { 0, SECTOR0_BASE, SECTOR0_SIZE},
    { 1, SECTOR1_BASE, SECTOR1_SIZE},
    { 2, SECTOR2_BASE, SECTOR2_SIZE},
    { 3, SECTOR3_BASE, SECTOR3_SIZE},
    { 4, SECTOR4_BASE, SECTOR4_SIZE},
    { 5, SECTOR5_BASE, SECTOR5_SIZE},
           
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
  * @brief Flash Module�� �ʱ�ȭ �Ѵ�.
  * @param NONE
  * @retval NONE
  */
void hFlashInit(void)
{
    int i;
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

static int get_pageAddr(unsigned int sector){
    int i;
    for(i=0;i<SECTORn;i++){
    if(sector_db[i].sector == sector){
        return sector_db[i].address;
    }
    }
    return -1;
}

/**
  * @brief Flash�� Program ������ ���·� ����ȴ�.
  * �� �Լ��� Flash�� ���� Bank ������ ȣ�� �� �� ����.
  * @param NONE
  * @retval NONE
  */
void hFlashWriteEnable(void)
{
    __disable_irq();
    HAL_FLASH_Unlock();
}
/**
  * @brief Flash�� Code ���� ���� ����ȴ�.
  * @param NONE
  * @retval NONE
  */
void hFlashWriteDisable(void)
{
    HAL_FLASH_Lock();
    __enable_irq();
}
/**
  * @brief flash�� Ư�� �ּҿ� �ش� ������ �����͸� Program �ϰ� �ȴ�.
  * Program �ϱ� ���� �־��� �ּ� ���� Program �� size��ŭ Erase�Ǿ�� �ϸ�,
  * �� �Լ��� ȣ�� �Ǳ� ���� hFlashWriteEnable() �� ȣ�� �Ǿ���Ѵ�.
  * @param[in] address program �ϱ� ���� ���� ����
  * @param[in] buf program �ϱ� ���� �������� ���� ������
  * @param[in] len ������ ������
  * @retval 0���� ū�� : Program �� �������� ������
  * @retval ���� : ��������
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
  * @brief Ư�� �ּ��� Bank�� �������� �־��� Size��ŭ Erase �Ѵ�.
  * @param[in] addr Erase �ϱ� ���� ���� ����
  * @param[in] size Erase �� Size
  * @retval Erase�� ������ 
  * @retval  ����: �������� 
  */
int32_t hFlashErase(uint32_t addr, int32_t size)
{
    int first_sector, end_sector;
    int mass_mode = 0;
    unsigned int SectorError;
    FLASH_EraseInitTypeDef erase;
    if ( (first_sector = get_sector(addr)) < 0)
        return E_INVALID;
    if( (end_sector = get_sector(addr+size)) < 0)
        return E_INVALID;
    if(size == -1){ //erase all
        mass_mode = TRUE;
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
    printf("erase sector [ 0x%x ~0x%x ]\n", sector_db[first_sector].address, (sector_db[end_sector].address+sector_db[end_sector].size));
    return 0;
}

/**
  * @brief Ư�� �ּ� ���� ������ ������ ����� �ִ� �����Ϳ� ���� ���� �����Ѵ�.
  * @param[in] address ���� �� �ּ��� ���� ����
  * @param[in] buf �ּ� ������ ���� �������� ���� ������
  * @param[in] len �������� ���� ������
  * @retval  0: ���� 
  * @retval  ����: �������� 
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



/**
    ******************************************************************************
    *     @file         EEprom_Control.c
    *     @version   
    *     @author    
    *     @date      2017/09/12 
    *     @brief     EEPROM Byte 또는 버퍼 단위 READ,WRITE API
    ******************************************************************************
    */

#include "obshal.h"
//#include "hos.h"
//#include "heeprom.h"
#include "AppQueue.h"
#include "EEprom_Control.h"
#include "EEprom_ProcessData.h"



//--- Per 1Byte
PUBLIC void EEprom_WriteByte(uint32_t startAddr, uint8_t Txdata)
{
    if(startAddr == EEP_COM_BASE_TRBMICNT){
        //printf("c");
    }
    hEprEnable();
    hEprWriteByte(startAddr, Txdata);
    hEprDisable();
}

PUBLIC uint8_t EEprom_ReadByte(uint32_t startAddr)//, unsigned int *data)
{
	return  hEprReadByte(startAddr);
}

//--- Per 1Byte
PUBLIC void EEprom_WriteBuf(uint32_t startAddr, uint8_t *Txdata, uint32_t size)
{
    hOsLock();
        hEprEnable();
        hEprWrite(startAddr, Txdata,size);
        hEprDisable();
    hOsUnlock();
}

PUBLIC void EEprom_ReadBuf(uint32_t startAddr, uint8_t *Rxdata, uint32_t size)//, unsigned int *data)
{
    hOsLock();
	  hEprRead(startAddr, Rxdata, size);
    hOsUnlock();
}











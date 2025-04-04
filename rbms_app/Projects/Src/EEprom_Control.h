#ifndef __EEPROM_CONTROL_H__
#define __EEPROM_CONTROL_H__

/**
 ******************************************************************************
 *     @file         EEprom_Control.h
 *     @version   
 *     @author    
 *     @date      2024/09/15 
 *     @brief     
 ******************************************************************************
*/



#define WREN 0x06 //0b00000110  // Write Enable
#define WRDI 0x04 //0b00000100  // Write Disable
#define RDSR 0x05 //0b00000101  // Read Status Register
#define WRSR 0x01 //0b00000001  // Write Status Register
#define READ 0x03 //0b00000011  // Read from Memory Array
#define WRITE 0x02 //0b00000010 // Write to Memory Array


#define SPICNT_VALUE 4

void EEprom_WriteByte(uint32_t startAddr, uint8_t Txdata);
uint8_t EEprom_ReadByte(uint32_t startAddr);
void EEprom_WriteBuf(uint32_t startAddr, uint8_t *Txdata, uint32_t size);
void EEprom_ReadBuf(uint32_t startAddr, uint8_t *Rxdata, uint32_t size);






#endif //__EEPROM_CONTROL_H__

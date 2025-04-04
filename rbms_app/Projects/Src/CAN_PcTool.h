#ifndef __CAN_PCTOOL_H__
#define __CAN_PCTOOL_H__

/**
 ******************************************************************************
 *     @file         CAN_PcTool.h
 *     @version   
 *     @author    
 *     @date      2024/11/07 
 *     @brief     
 ******************************************************************************
*/

typedef struct
{
    MH mh;          //message header
    uint8_t rw;     //1-read, 2-write
    uint16_t addr;   //start address
    uint8_t size;       // data size;
    uint8_t idx;
    uint8_t data[16];   //data buffer
    uint8_t fail;      // when it fails
} PCINTER_MSG;
    

PUBLIC void Init_CanPCInterface(void);
PUBLIC void* PCInter_AllocMessage(uint8_t);
PUBLIC void PCInter_FreeMessage(void *msg);
PUBLIC void CanPCToolEventHandler(uint32_t fd, uint16_t id, uint8_t *data);
PUBLIC void PCInter_ReturnReadBytes(void *buf);
PUBLIC void PCInter_ReturnWriteBytes(void *buf);
PUBLIC void PCInter_SendSegmentDataToPC(void);
PUBLIC void PCInter_SuccessFactoryReset(void);
PUBLIC void PCInter_SendEvtLogDataToPC(void);
PUBLIC void PCInter_SuccessEraseAll(void);

#endif //__CAN_PCTOOL_H__

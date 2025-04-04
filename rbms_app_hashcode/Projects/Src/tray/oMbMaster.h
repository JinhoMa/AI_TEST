#ifndef __OMBMASTER_H__
#define __OMBMASTER_H__

/**
    ******************************************************************************
    *     @file         oMbMaster.h
    *     @version   
    *     @author    
    *     @date      2016/05/13 
    *     @brief     
    ******************************************************************************
    */

enum {
    OMB_RTU,
    OMB_TCP
};

enum {
    OMB_ASYNC_ERROR,
    OMB_ASYNC_SUCCESS
};

int32_t oMbMasterOpen( uint32_t mode, const uint8_t* port, uint32_t baudrate, void *asyncFunc); 
int32_t oMbMasterClose(int32_t fd );
int32_t oMbMasterReadCoils(int32_t fd, uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data);
int32_t oMbMasterReadDisInputs(int32_t fd, uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data);
int32_t oMbMasterReadHoldRegs(int32_t fd, uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data);
int32_t oMbMasterReadInputRegs(int32_t fd, uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data);
int32_t oMbMasterWriteSingleCoil(int32_t fd, uint32_t slaveAddr, uint32_t addr, uint16_t data, int32_t timeout);
int32_t oMbMasterWriteSingleReg(int32_t fd, uint32_t slaveAddr, uint32_t addr, uint16_t data, int32_t timeout);
int32_t oMbMasterWriteMultiCoils(int32_t fd, uint32_t slaveAddr, uint32_t addr, uint8_t *data, uint16_t len, int32_t timeout);
int32_t oMbMasterWriteMultiRegs(int32_t fd, uint32_t slaveAddr, uint32_t addr, uint8_t *data, uint16_t len, int32_t timeout);
#endif //__OMBMASTER_H__

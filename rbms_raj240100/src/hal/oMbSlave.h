#ifndef __OMBSLAVE_H__
#define __OMBSLAVE_H__

/**
    ******************************************************************************
    *     @file         oMbSlave.h
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

int32_t oMbSlaveOpen(uint32_t mode, uint8_t address, uint32_t port, uint32_t baudrate); 
void oMbSlaveStart(int32_t sid);
void oMbSlaveClose(int32_t sid);
void oMbSlavePoll(int32_t sid);

int32_t oMbMasterClose(void);
int32_t oMbMasterReadCoils(uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data);
int32_t oMbMasterReadDisInputs(uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data);
int32_t oMbMasterReadHoldRegs(uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data);
int32_t oMbMasterReadInputRegs(uint32_t slaveAddr, uint32_t addr, uint32_t cnt, uint16_t *data);
int32_t oMbMasterWriteSingleCoil(uint32_t slaveAddr, uint32_t addr, uint16_t data, int32_t timeout);
int32_t oMbMasterWriteSingleReg(uint32_t slaveAddr, uint32_t addr, uint16_t data, int32_t timeout);
int32_t oMbMasterWriteMultiCoils(uint32_t slaveAddr, uint32_t addr, uint8_t *data, uint16_t len, int32_t timeout);
int32_t oMbMasterWriteMultiRegs(uint32_t slaveAddr, uint32_t addr, uint8_t *data, uint16_t len, int32_t timeout);
#endif //__OMBSLAVE_H__

#ifndef __HLOG_H
#define __HLOG_H

/*
  *  User Level 31~8 bit,  System Level 7~0 bit
  * ----------------------------------------------------
  *                                        Default:|WARN|ERROR|ABORT|
  * ----------------------------------------------------  
  */

#define LOG_MSG3	1<<23
#define LOG_MSG2	1<<22
#define LOG_MSG1	1<<21
#define LOG_SOC     1<<19
#define LOG_MBMS    1<<18
#define LOG_MODBUS_TCP 1<<17
#define LOG_RENE    1<<16
#define LOG_CBAL    1<<15
#define LOG_CELL    1<<14
#define LOG_DTC     1<<13
#define LOG_RLY     1<<12
#define LOG_RBMS    1<<11
#define LOG_ETH     1<<10
#define LOG_MODBUS  1<<9
#define LOG_BMIC    1<<8
#define LOG_FLASH   1<<7
#define LOG_VOL     1<<6
#define LOG_CUR     1<<5
#define LOG_DIAG    1<<4
#define LOG_PCTOOL  1<<3
#define LOG_WARN    1<<2
#define LOG_ERROR	1<<1
#define LOG_ABORT	1


#define LOG_DEFAULT  ( LOG_ERROR |LOG_MSG1 | LOG_MBMS | LOG_DTC | LOG_CBAL)//G_MSG3| LOG_MSG2| LOG_MSG1| LOG_ERROR | LOG_ABORT | LOG_WARN  )


#endif

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
#define LOG_ETH 1<<10
#define LOG_MODBUS  1<<9
#define LOG_BMIC    1<<8
#define LOG_WARN    1<<2
#define LOG_ERROR	1<<1
#define LOG_ABORT	1

#define LOG_DEFAULT  (LOG_MSG3| LOG_MSG2|LOG_MSG1| LOG_ERROR | LOG_ABORT | LOG_WARN   )

#endif

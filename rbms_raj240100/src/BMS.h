#ifndef __BMS_H__
#define __BMS_H__

/**
   ******************************************************************************
   *     @file         BMS.h
   *     @version   
   *     @author    
   *     @date       2021/12/10 
   *     @brief     
   ******************************************************************************
   */

#define MODEL_NAME           "ESC100"


#define SW_MAJOR                 1
#define SW_MINOR1                0
#define SW_MINOR2                0

#define BOARD_NAME         "HU20MB00011B-14S"


#define USE_AFETM             0    // AFE ? Telerant Time Interval ==> Use AFE CC intterupt (250ms)
#define USE_MCUTM           0     // AFE ? Tolerant Time Interval ==>use  MCU 1msec interrupt   ( 1ms)
#define DEBUG_USE             0    // UART? ??? ??? ??? ?? ?? 
#define USE_JTAGDEBUG      0    // JTAG DEBUGGING? ?? ??? SLEEP ? ???? ?? ?? 
#define CAL_ROM           0     // CAL ?? Flash? ?? ?? ROM DATA ? ?? ?? . ?? CAL MOT ??? ???? ???.
#define BMS_LOG           0       // BMS ?? Log Message ? ON ??. (UART ?? ??)
#define CAL_LOG            0     // CAL ?? Log Message ? ON ??. (UART ????) 




#define BD_VER_1      1
#define BD_VER_2      2
#define BOARD   BD_VER_2
//#define CUR_SIMUL
//#define TEMP_SIMUL

typedef struct {
      char date[6];
      char pad1;
      char time[6];
      char pad2;
      char ver[6];
} _BUILD_INFO;
typedef struct _BUILD_INFO FW_BUILD_INFO;




#endif //__BMS_H__

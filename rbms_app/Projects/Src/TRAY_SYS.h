#ifndef __TRAY_SYS_H__
#define __TRAY_SYS_H__

/**
    ******************************************************************************
    *     @file         TRAY_SYS.h
    *     @version   
    *     @author    
    *     @date      2017/02/18 
    *     @brief     
    ******************************************************************************
    */


///////////////////////////////////////////////////////////////   1. SW Version Check ///////////////////////////////////////////////////////////
#define SW_MAJOR 1
#define SW_MINOR 0
#define SW_PATCH 0
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
*    @brief  
*                 BMIC 와 모듈간에 실제 연결될때 BMIC DEVICE 개수 
*                예) 4P15S 인 경우 BMIC 2개 사용된다. 
*/
#define MAX_COMM_NUMS           1           //RACK BMS와 Tray간 연결되는 채널 수 COM DEVICE 와 실제 DaisyChain 인터페이스 개수 ( 단일화 or 이중화)

#define SEN_ALG_3MEANS           1
#define SEN_ALG_9MOVAVG          0

#define SLAVEDEV_BUF_SIZE    1   //Buffer를 위한 DEFINE, 실제 사이즈와 틀림 , Tray당 최대 BMIC (Module) 개수 

#define BMIC_MAX_CH        10
#define BMIC_AUX_NUM       4
#define CELL_BUF_SIZE       BMIC_MAX_CH*SLAVEDEV_BUF_SIZE   // Compile 시 지정되는 Tray당 CELL 개수 사이즈 
#define TMP_BUF_SIZE        3    // Compile 시 지정되는 Tray당 온도 개수 사이즈 
#define BDTMP_BUF_SIZE      1    // Compile 시 지정되는 Tray당 보드온도 개수 사이즈 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TRAY_ERR_BMIC           0x1
#define TRAY_ERR_DAISYCHAIN     0x2
#define TRAY_ERR_DIAG           0x4
#define TRAY_ERR_EEPCONFIG      0x8

#define TRAY_DIAG_ERR_ALERTOUT_SHORT           1
#define TRAY_DIAG_ERR_CELLPIN_OPEN                2
#define TRAY_DIAG_ERR_DIETEMP                       3
#define TRAY_DIAG_ERR_VAA                             4
#define TRAY_DIAG_ERR_ALTREF                         5
#define TRAY_DIAG_ERR_COMPACC                      6
#define TRAY_DIAG_ERR_CELLGAIN_CAL                 7
#define TRAY_DIAG_ERR_OFFSET_CAL                    8
#define TRAY_DIAG_ERR_THRMOFFSET_CAL            9
#define TRAY_DIAG_ERR_LSAMPOFFSET                10
#define TRAY_DIAG_ERR_ZEROSCALE_ADC             11
#define TRAY_DIAG_ERR_FULLSCALE_ADC               12
#define TRAY_DIAG_ERR_DAC1_4SCALE                 13
#define TRAY_DIAG_ERR_DAC3_4SCALE                 14
#define TRAY_DIAG_ERR_BALSW_SHORT                15
#define TRAY_DIAG_ERR_BALSW_OPEN                  16
#define TRAY_DIAG_ERR_SENWIRE_OPEN                17


#if     (TRAY_DIAG_EQUMENT==1)
#define SCAN_OVERSAMPLEs        0      // No-OverSample : 0, 4,16 OverSample :16 
#else
#define SCAN_OVERSAMPLEs        16      // No-OverSample : 0, 4,16 OverSample :16 
#endif

//#define BMIC_DEFAULT_7CELLMASK          0x007F
#define BMIC_DEFAULT_8CELLMASK          0x00FF 
#define BMIC_DEFAULT_9CELLMASK         0x07FC
#define BMIC_DEFAULT_10CELLMASK        0x03FF
#define BMIC_DEFAULT_11CELLMASK         0x07FF
#define BMIC_DEFAULT_12CELLMASK         0x0FFF
#define BMIC_DEFAULT_16CELLMASK         0xFFFF

#if ( P100_8S == 1)
#define INIT_CELLVOLTAGE 3200
#endif
#define INIT_TEMPERATURE 2500

typedef enum{
    NO_USE      = 0,
    AUX_TEMP    = 1,    // Module Temperature Input
    AUX_BDTEMP  = 2,    // Board Temperature
    AUX_GPIO = 3     // External Input(Interlock) or Output( Control)
} BmicAuxEnum;


typedef struct{
    uint8_t type;
    uint16_t data;
}  __attribute__((packed)) TrayAuxModel;

typedef struct{
    uint16_t data;
}  __attribute__((packed)) InterlockModel;


PUBLIC void Variable_Init(void);


#endif //__TRAY_SYS_H__

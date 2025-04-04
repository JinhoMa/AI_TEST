/**
    ******************************************************************************
    *     @file         EEprom_ProcessData.c
    *     @version   
    *     @author    
    *     @date      2017/09/12 
    *     @brief     EEPROM 디바이스 접근 및 CONFIG 관련 데이터 처리 
    ******************************************************************************
    */
#include <stdint.h>
#include "obshal.h"
//#include "hos.h"
//#include "htypes.h"
//#include "heeprom.h"
//#include "htimer.h"
#include "BMS.h"
#include "BMS_OS.h"
#include "AppQueue.h"
#include "EEprom_ProcessData.h"
#include "EEprom_Control.h"
//#include "Sk_Config.h"
//#include "CAN_LineTest.h"
//#include "Operating_Condition.h"
//#include "BMIC_ProcessData.h"
//#include "CellBalancing.h"
//#include "ExternalTempSensing.h"
//#include "RTC_CTL.h"
//#include "sm.h"
#include <string.h>
#include <stdlib.h>
#include "CAN_DiagCom.h"
#include "CAN_PCTool.h"


STR_EEPROM_DataModel EEPROMData;

static void readEEPRomDataAllFromDevice(void);

/**
   *  @brief 초기화 할때 주의 할점
   */
EEP_COM_BASE ComBase=
{   
    RACK_MODEL_NAME,    //MN
    RACK_MODEL_VER,     //MV
    RACK_SER_NUM,       //SN
    1,                  //ID
    TRAY_MODEL_NAME,    //TrMN
    TRAY_MODEL_VER,     //TrMV
    TRAY_DEFAULT_NUMS,              //TrCnt
    TRAY_DEFAULT_CELL_NUMS,         //TrCellCnt
    TRAY_DEFAULT_BMIC_COUNT,        //TrBMICCnt
    TRAY_DEFAULT_TEMP_SENSORNUM,    //TrTmpCnt
    TRAY_DEFAULT_BDTEMP_SENSORNUM,  //TrBdTmpCnt
    BMIC_DEFAULT_CELLENA_COUNT,     //BMIC 1개당 Enable 된 Cell Count
    {   
#if( P100_8S == 1)
        BMIC_DEFAULT_8CELLMASK,
        0,
        0,
        0
#else
        0,
        0,
        0,
        0        
        
#endif
    },
    {0,0,0,0},
    {
#if (P100_8S == 1)
      (((AUX_TEMP<<6)&0xC0)|((AUX_TEMP<<4)&0x30)|((AUX_TEMP<<2)&0x0C)| (NO_USE&0x3))    , NO_USE,   NO_USE,  NO_USE,
      0,0,0,0,
      0,0,0,0
#else
      0,
      0,
      0,
      0, 
      0,0,0,0,0,0,0,0
#endif
    },

    
};

EEP_COM_BLDMGR  ComBldMgr=
{
    ((SW_VER_MAJOR<<4)&0xF0)|(SW_VER_MINOR&0xF), SW_VER_MINOR2, 0 , 0, 0 
};
EEP_COM_NETMGR  ComNetMgr=
{
    NETIP1, NETIP2, NETIP3, NETIP4, GWIP4
};
EEP_COM_BMICONF ComBmicConf=
{
    //Short Type Value muste be converted for little endian type
    0xFFFF,
    DEFAULT_OVER_CELL_V_FAULT_DETECT,
    DEFAULT_OVER_CELL_V_WARNING_DETECT,
    0xFFFF,
    DEFAULT_UNDER_CELL_V_FAULT_DETECT,
    DEFAULT_UNDER_CELL_V_WARNING_DETECT,
    DEFAULT_CELL_DEVIATION_V_FAULT_DETECT,
    DEFAULT_CELL_DEVIATION_V_WARNING_DETECT,
    DEFAULT_OVER_CELL_TEMP_FAULT_DETECT,
    DEFAULT_OVER_CELL_TEMP_WARNING_DETECT,
    DEFAULT_UNDER_CELL_TEMP_FAULT_DETECT,
    DEFAULT_UNDER_CELL_TEMP_WARNING_DETECT,
    DEFAULT_OVER_SOC_FAULT_DETECT,
    DEFAULT_OVER_SOC_WARNING_DETECT,
    DEFAULT_UNDER_SOC_FAULT_DETECT,
    DEFAULT_UNDER_SOC_WARNING_DETECT,
    0xFFFF,
    DEFAULT_OVER_BAT_VOL_FAULT_DETECT,
    DEFAULT_OVER_BAT_VOL_WARNING_DETECT,
    0xFFFF,
    DEFAULT_UNDER_BAT_VOL_FAULT_DETECT,
    DEFAULT_UNDER_BAT_VOL_WARNING_DETECT,
    0xFFFF,
    DEFAULT_OVER_CHARGE_CURRENT_FAULT_DETECT,
    DEFAULT_OVER_CHARGE_CURRENT_WARN_DETECT,
    0xFFFF,
    DEFAULT_OVER_DSCHARGE_CURRENT_FAULT_DETECT,
    DEFAULT_OVER_DSCHARGE_CURRENT_WARN_DETECT,
    0xFFFF,
    0xFFFF,
    DEFAULT_MODULE_TEMP_DEV_FAULT_DETECT,
    DEFAULT_MODULE_TEMP_DEV_WARNING_DETECT,
    0
    
};

EEP_COM_EVTLOG ComEvtLog =
{
    0,0
};

EEP_COM_CAL  ComCal =
{
   {16,    32 ,    0 }, //REFERENCE PACK ?? 
   {7937,  15900,    0 }, //   {4811,  9973,  0 },
   {16,     32,   0 }, //REFERENCE PCS ?? 
   {7609, 15460,   0 },
   
   {0,         3,     0 }, //REFERENCE Current Value
   {130982, 130227,   0 }, // Charge Current Measure
   {130983, 132030,   0 }, // DisCharge Current Measure
   
   {3327, 17000, 0},
   {3327, 17000, 0},

   2000, 4000 ,  //CV_LVREF, CV_HVREF
   { 9887  , 9881, 9891, 9891, 9891, 9891,  9892,9884,9890,9890     },
   { 19788 ,19782,19786,19784,19784, 19781,19785,19777,19781,19781 }
};

EEP_COM_SOH  ComSoh =
{
#if(SOH_NEWLIB_REPLACE==1)
    0,0,0,1000
#else
   0, 0, 0
#endif   
};

#if ( ETH_ENABLE_CDMS == 1)    

EEP_COM_CANCNF ComCancnf =
{
    1, 0
};
#endif

EEP_MG_CONF  MgConf =
{
    DEFAULT_SOC_5PERCENT_CELL_V,
    DEFAULT_DSCHARGE_STOP_CELL_V,
    DEFAULT_CHARGE_WAIT_CELL_V,
    DEFAULT_POWEDOWN_CELL_V,
    DEFAULT_FULLCHARGE_JUGE_CELL_V,
    DEFAULT_FULLCHARGE_TAPPER_CELL_V,
    DEFAULT_FULLCHARGE_WAKEUPJUGE_CELL_V,
    DEFAULT_FULLCHARGE_JUGE_CURRENT,
    DEFAULT_DISCHARGE_JUGE_CURREN,
    DEFAULT_CHARGE_JUGE_CURRENT,
    0x0,
    0x0
};

EEP_SOC_CONF SocConf =
{
    { //  0%    5%   10%   15%   20%   25%   30%   35%   40%   45%   50%   55%   60%   65%   70%  75%    80%   85%   90%   95%   100%
        2552, 3128, 3208, 3226, 3253, 3272, 3288, 3295, 3298, 3300, 3302, 3305, 3309, 3325, 3335, 3338, 3341, 3342, 3345, 3348, 3360
    },
    DEFAULT_CELL_CAPACITY,
    DEFAULT_TOTAL_RC,
    DEFAULT_TOTAL_ENERGY,
};

EEP_REC_DATA RecDF =
{
    0,
    0,    //%
    100,
    0,
    {
        0,0,0,0,0,0,0,0,0,0,0,11,        // idx 0
        1,0,0,0,0,0,0,0,0,0,0,11,        // idx 1
        2,0,0,0,0,0,0,0,0,0,0,11,        // idx 2
        3,0,0,0,0,0,0,0,0,0,0,11,        // idx 3
        4,0,0,0,0,0,0,0,0,0,0,11,        // idx 4
        5,0,0,0,0,0,0,0,0,0,0,11,        // idx 5
        6,0,0,0,0,0,0,0,0,0,0,11,        // idx 6
        7,0,0,0,0,0,0,0,0,0,0,11,        // idx 7
        8,0,0,0,0,0,0,0,0,0,0,11,        // idx 8
        9,0,0,0,0,0,0,0,0,0,0,11,        // idx 9
        10,0,0,0,0,0,0,0,0,0,0,11,        // idx 10
        11,0,0,0,0,0,0,0,0,0,0,11,        // idx 11
        12,0,0,0,0,0,0,0,0,0,0,11,        // idx 12
        13,0,0,0,0,0,0,0,0,0,0,11,        // idx 13
        14,0,0,0,0,0,0,0,0,0,0,11,        // idx 14
        15,0,0,0,0,0,0,0,0,0,0,11,        // idx 15
    },
};

static uint8_t getBuildYear(const char *time)
{
    uint16_t t_yearnum;
    t_yearnum = atoi(time + strlen(time)-4);
    t_yearnum -= 2000;
    return (uint8_t)(t_yearnum&0xFF);

}
static uint8_t getBuildMonth(const char *time)
{
   int i;
    static const char *month_names[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    for(i=0;i<12;i++){
    	if(strncmp(time,month_names[i],3)==0){
    		break;
    	}
    }    
    if(i==12) return 0;
    return i+1;
}
static uint8_t getBuildDay(const char *time)
{
    return (uint8_t)atoi(time+4) ;    

}

extern const uint8_t tableCRCHi[256];
extern const uint8_t tableCRCLo[256];
static uint16_t eepRomCrc16(const uint8_t * const _pBuff, uint32_t _len)
{
    uint8_t CrcHi = 0xFF;
    uint8_t CrcLo = 0xFF;
    uint32_t bInd = 0;
    uint32_t wInd;

    while(_len--) {
        wInd = CrcLo ^ _pBuff[bInd++];
        CrcLo = (uint8_t)(CrcHi ^ tableCRCHi[wInd]);
        CrcHi = tableCRCLo[wInd];
    };
    return ((uint16_t)(CrcHi << 8 | CrcLo));
}

static void updateCommonBlockChkSum(void)
{
    uint16_t crc;
    crc = eepRomCrc16((const uint8_t *)&EEPROMData.base, sizeof(EEP_COM_BASE));
    crc +=eepRomCrc16((const uint8_t *)&EEPROMData.bldmgr,sizeof(EEP_COM_BLDMGR));
    crc +=eepRomCrc16((const uint8_t *)&EEPROMData.netmgr, sizeof(EEP_COM_NETMGR));
    crc +=eepRomCrc16((const uint8_t *)&EEPROMData.bmiconf,sizeof(EEP_COM_BMICONF));
    EEPROMData.com_chksum = crc;
    EEprom_WriteByte(EEP_COM_CHKSUM, (crc>>8)&0xFF);
    EEprom_WriteByte(EEP_COM_CHKSUM+1, (crc&0xFF));
}

static void updateCalibBlockChkSum(void)
{
    uint16_t crc;
    crc = eepRomCrc16((const uint8_t *)&EEPROMData.cal , sizeof(EEP_COM_CAL));
    EEprom_WriteByte(EEP_CAL_CHKSUM, (crc>>8)&0xFF);
    EEprom_WriteByte(EEP_CAL_CHKSUM+1, (crc&0xFF));
    EEPROMData.cal_chksum = crc;
}

static int memMachTest(void)
{
    uint16_t crc;
    uint16_t rc_crc;
    //COMMON BLOCK CHKSUM
    crc = eepRomCrc16((const uint8_t *)&EEPROMData.base, sizeof(EEP_COM_BASE));
    crc +=eepRomCrc16((const uint8_t *)&EEPROMData.bldmgr,sizeof(EEP_COM_BLDMGR));
    crc +=eepRomCrc16((const uint8_t *)&EEPROMData.netmgr, sizeof(EEP_COM_NETMGR));
    crc +=eepRomCrc16((const uint8_t *)&EEPROMData.bmiconf,sizeof(EEP_COM_BMICONF));
    rc_crc = EEprom_ReadByte(EEP_COM_CHKSUM);
    rc_crc = (rc_crc<<8)&0xFF00;
    rc_crc += EEprom_ReadByte(EEP_COM_CHKSUM+1);
    if( crc != rc_crc )
        return FALSE;
    EEPROMData.com_chksum = crc;
    //CALIBRATION BLOCK CHKSUM
    crc = eepRomCrc16((const uint8_t *)&EEPROMData.cal , sizeof(EEP_COM_CAL));
    rc_crc = EEprom_ReadByte(EEP_CAL_CHKSUM);
    rc_crc = (rc_crc<<8)&0xFF00;
    rc_crc += EEprom_ReadByte(EEP_CAL_CHKSUM+1);
    if( crc != rc_crc )
        return FALSE;
    EEPROMData.cal_chksum = crc;
    return TRUE;
}


/**
   * @brief EEPROM 에 저장될 데이터를 초기화 한다.
   */
void Init_EEPROM()
{
    EEPROMData.IsRunning=0;
    EEPROMData.JobIdx=0;
    EEPROMData.Size=0;
    EEPROMData.memIdx=0;
    readEEPRomDataAllFromDevice();
    if( EEPROMData.base.TrCnt == 0x0 || EEPROMData.base.TrCnt ==0xFF){  //EEPROM이 초기화 되지 않은 경우 
        EEP_FactoryResetEEPRom(1);
    }
    if( memMachTest() == FALSE){
        EEPROMData.chksumerr = 1;
    }
    //rack_ini.eeprom = 1;
}

static void updateEEPRomFactoryDataAll(void)
{

    memcpy((char *)&EEPROMData.base, (const char *)&ComBase, sizeof(EEP_COM_BASE));
   // EEPROMData.base.ID = hSwRead();
    memcpy((char *)&EEPROMData.bldmgr, (const char *)&ComBldMgr, sizeof(EEP_COM_BLDMGR));
    EEPROMData.bldmgr.SWBLDYR = getBuildYear(__DATE__);
    EEPROMData.bldmgr.SWBLDMON = getBuildMonth(__DATE__);
    EEPROMData.bldmgr.SWBLDDAY = getBuildDay(__DATE__);
    memcpy((char *)&EEPROMData.netmgr, (const char *)&ComNetMgr, sizeof(EEP_COM_NETMGR));    
    EEPROMData.netmgr.NIP4 = NETIP4 + EEPROMData.base.ID;
    EEPROMData.netmgr.GIP4 = GWIP4;
    memcpy((char *)&EEPROMData.bmiconf, (const char *)&ComBmicConf, sizeof(EEP_COM_BMICONF));  

    memcpy((char *)&EEPROMData.mgconf, (const char *)&MgConf, sizeof(EEP_MG_CONF));  
    memcpy((char *)&EEPROMData.soccnf, (const char *)&SocConf, sizeof(EEP_SOC_CONF));  
    memcpy((char *)&EEPROMData.recdf, (const char *)&RecDF, sizeof(EEP_REC_DATA));   
    /*
    for(i=0;i<TRAY_NUMS;i++){
        memset( (char *)&EEPROMData.tray[i], 0x0, sizeof(EEP_TRAY_DATA));
    }
    memcpy((char *)&EEPROMData.evlog, (const char *)&ComEvtLog, sizeof(EEP_COM_EVTLOG));
    */
}


/**
    *       @brief comEEPROM 
    *              PC Tool 을 통해서 Item별 Read와 Write 하기 위해서 구현 함.
    *               PC TOOL을 통해서 Data을 넘겨 줄때 데이터가 type정보가 없기 때문에 무조건 Big Endian Data로 넘어 오게 된다.
    *               역으로 여기서 셋 된 값도 PC에서 읽을 떄 Data의 Type을 모르기 때문에 comEEPRom 에서 필드값 별로 byte 
    *               ordering를 잘 처리하도록 한다.
    *       @[in] id       0 - reserved,  1- PC
    *       @[in] addr  address 주소
    *      @[in] rw        Read 와 Write 모드를 나타냄
    *      @[in] data    데이터의 버퍼 포인터
    *      @[in] len       데이터의 길이
    *      @[out]   -1     READ ONLY 에 WRITE모드로 접근
    *                  -24     ACCESS 권한이 없음
                        0이상  operation한 size 
 **/
PUBLIC int32_t comEEPRom(uint8_t id, uint32_t addr, uint8_t rw, uint8_t *data, uint8_t len)
{
    //uint8_t readin;
    int i,offset;
    uint8_t *ptr;

    if(id ==1  && EEPROMData.access != 1){
        return E_ACCESS;
    }
    switch(addr)
    {
        case EEP_COM_BASE_MN:
            //if(rw == COM_EEP_WRITE_MODE) return -1;
            strncpy((char *)data, (const char *)EEPROMData.base.MN, len);
            return len;
        case EEP_COM_BASE_MV:
            if(rw == COM_EEP_WRITE_MODE) return -1;
            strncpy((char *)data, (const char *)EEPROMData.base.MV, len);
            return len;
        case EEP_COM_BASE_SN:
            if(rw == COM_EEP_WRITE_MODE){
                strncpy((char *)EEPROMData.base.SN, (const char *)data, len);
                for(i=0;i<len;i++){
                    EEprom_WriteByte(EEP_COM_BASE_SN+i,EEPROMData.base.SN[i]);
                }
                updateCommonBlockChkSum();
            }else{
                strncpy((char *)data, (const char *)EEPROMData.base.SN, len);
            }
            return len;
        case EEP_COM_BASE_ID:
            if(rw == COM_EEP_WRITE_MODE){
                EEPROMData.base.ID = *data;
                EEprom_WriteByte( addr, EEPROMData.base.ID);
                EEPROMData.netmgr.NIP4 = NETIP4 + EEPROMData.base.ID;
                addr = EEP_COM_NETMGR_NIP4;
                EEprom_WriteByte( addr, EEPROMData.netmgr.NIP4);
                updateCommonBlockChkSum();                
            }else{
                *data = EEPROMData.base.ID;
            }
            return 1;
        case EEP_COM_BASE_TRCNT:
        case EEP_COM_BASE_TRCELLCNT:
        case EEP_COM_BASE_TRBMICNT:
        case EEP_COM_BASE_TRTMPCNT:
        case EEP_COM_BASE_TRBDTMPCNT:
        case EEP_COM_BASE_BMICCELLENACNT:
        case EEP_COM_BASE_BMICAUXUSE0:
        case EEP_COM_BASE_BMICAUXUSE1:
        case EEP_COM_BASE_BMICAUXUSE2:
        case EEP_COM_BASE_BMICAUXUSE3:
            ptr =(uint8_t *)&EEPROMData.base;
            if(rw == COM_EEP_WRITE_MODE){
                for(i=0;i<len;i++){
                    *(ptr+addr+i) = *(data+i);
                    EEprom_WriteByte(addr, *data);
                }
                updateCommonBlockChkSum();                
            }else{
                for(i=0;i<len;i++)
                {
                    *(data+i) = *(ptr+addr+i);
                }
            }
            return 1;
        case EEP_COM_BASE_BMICCELLMASK0:
        case EEP_COM_BASE_BMICCELLMASK1:
        case EEP_COM_BASE_BMICCELLMASK2:
        case EEP_COM_BASE_BMICCELLMASK3:
            ptr =(uint8_t *)&EEPROMData.base;
            offset =addr ;
            if(rw == COM_EEP_WRITE_MODE){
                //16bit 는 little endian를 변환                
                *(ptr+offset) = *(data+1);
                EEprom_WriteByte(addr, *(data+1));
                *(ptr+offset+1) = *(data);
                EEprom_WriteByte(addr+1, *(data));
                updateCommonBlockChkSum();                
            }else{
                *data = *(ptr+offset+1);
                *(data+1) = *(ptr+offset);
            }
            return 1;
        case EEP_COM_BASE_TRMN:
            if(rw == COM_EEP_WRITE_MODE) return -1;
            strncpy((char *)data, (const char*)EEPROMData.base.TrMN, len);
            return len;
        case EEP_COM_BASE_TRMV:
            if(rw == COM_EEP_WRITE_MODE) return -1;
            strncpy((char *)data, (const char *)EEPROMData.base.TrMV, len);
            return len;
        case EEP_COM_BLDMGR_SW_MAJOR:
        case EEP_COM_BLDMGR_SW_MINOR:
        case EEP_COM_BLDMGR_BLDYR:
        case EEP_COM_BLDMGR_BLDMON:
        case EEP_COM_BLDMGR_BLDDAY:
            if(rw == COM_EEP_WRITE_MODE) return -1;
            ptr = (uint8_t *)&EEPROMData.bldmgr;
            offset = addr - EEP_COM_BLDMGR_ADDR;
            if(rw == COM_EEP_WRITE_MODE){
                *(ptr+offset) = *data;
                EEprom_WriteByte(addr, *data);
            }else
                *data = *(ptr+offset);
            
            return 1;
        case EEP_COM_NETMGR_NIP1:
        case EEP_COM_NETMGR_NIP2:
        case EEP_COM_NETMGR_NIP3:
        case EEP_COM_NETMGR_NIP4:
        case EEP_COM_NETMGR_GIP4:
            ptr =(uint8_t *)&EEPROMData.netmgr;
            offset =addr - EEP_COM_NETMGR_ADDR;
            if(rw == COM_EEP_WRITE_MODE){
                *(ptr+offset) = *data;
                EEprom_WriteByte(addr, *data);
                updateCommonBlockChkSum();
            }else
                *data = *(ptr+offset);
            return 1;
        case EEP_COM_BMICONF_OCVP:
        case EEP_COM_BMICONF_OCVF:
        case EEP_COM_BMICONF_OCVW:
        case EEP_COM_BMICONF_UCVP:
        case EEP_COM_BMICONF_UCVF:
        case EEP_COM_BMICONF_UCVW:
        case EEP_COM_BMICONF_CVD:
        case EEP_COM_BMICONF_OTP:
        case EEP_COM_BMICONF_OTF:
        case EEP_COM_BMICONF_OTW:
        case EEP_COM_BMICONF_UTF:
        case EEP_COM_BMICONF_UTW:
        case EEP_COM_BMICONF_OSOCF:
        case EEP_COM_BMICONF_OSOCW:
        case EEP_COM_BMICONF_USOCF:
        case EEP_COM_BMICONF_USOCW:
        case EEP_COM_BMICONF_OBVP:
        case EEP_COM_BMICONF_OBVF:
        case EEP_COM_BMICONF_OBVW:
        case EEP_COM_BMICONF_UBVP:
        case EEP_COM_BMICONF_UBVF:
        case EEP_COM_BMICONF_UBVW:
        case EEP_COM_BMICONF_OCCP:
        case EEP_COM_BMICONF_OCCF:
        case EEP_COM_BMICONF_OCCW:
        case EEP_COM_BMICONF_ODCP:
        case EEP_COM_BMICONF_ODCF:
        case EEP_COM_BMICONF_ODCW:
        case EEP_COM_BMICONF_OCD:
        case EEP_COM_BMICONF_TDP:
        case EEP_COM_BMICONF_TDF:
        case EEP_COM_BMICONF_TDW:
        case EEP_COM_BMICONF_CCOR: 
            ptr =(uint8_t *)&EEPROMData.bmiconf;
            offset =addr - EEP_COM_BMICONF_ADDR;
            if(rw == COM_EEP_WRITE_MODE){
                //16bit 는 little endian를 변환                
                *(ptr+offset) = *(data+1);
                EEprom_WriteByte(addr, *(data+1));
                *(ptr+offset+1) = *(data);
                EEprom_WriteByte(addr+1, *(data));
                updateCommonBlockChkSum();                
            }else{
                *data = *(ptr+offset+1);
                *(data+1) = *(ptr+offset);
            }
            return 2;
        case EEP_COM_CHKSUM:
            if(rw == COM_EEP_WRITE_MODE){
                return -1;
            }
            ptr = (uint8_t *)&EEPROMData.com_chksum;
            *data = *(ptr);
            *(data+1) = *(ptr+1);
            return 2;
        case EEP_COM_EVTLOG_DTC:
        case EEP_COM_EVTLOG_SUBCODE:
            //PC TOOL을 사용하지 않고 MCU내부적으로 16비트 업데이트는 little endian으로 값을 준다. 
            ptr = (uint8_t *)&EEPROMData.evlog.dtc;
            offset = addr - EEP_COM_EVTLOG_ADDR;
            if(rw == COM_EEP_WRITE_MODE){
                *(ptr+offset) = *(data);
                EEprom_WriteByte(addr, *(data));
                *(ptr+offset+1) = *(data+1);
                EEprom_WriteByte(addr+1, *(data+1));
            }else{
                *data = *(ptr+offset);
                *(data+1) = *(ptr+offset+1);
            }
            break;
        case EEP_CAL_BREFV1:
        case EEP_CAL_BREFV2:
        case EEP_CAL_BREFV3:
        case EEP_CAL_PREFV1:
        case EEP_CAL_PREFV2:
        case EEP_CAL_PREFV3:
        case EEP_CAL_REFC1:
        case EEP_CAL_REFC2:
        case EEP_CAL_REFC3:
        case EEP_CAL_BVC1:
        case EEP_CAL_BVC2:
        case EEP_CAL_BVC3:
        case EEP_CAL_PVC1:
        case EEP_CAL_PVC2:
        case EEP_CAL_PVC3:
        case EEP_CAL_GFDP1:
        case EEP_CAL_GFDP2:
        case EEP_CAL_GFDP3:
        case EEP_CAL_GFDN1:
        case EEP_CAL_GFDN2:
        case EEP_CAL_GFDN3:
        case EEP_CAL_LVREF:
        case EEP_CAL_HVREF:
        case EEP_CAL_LVARRAY:
        case EEP_CAL_LVARRAY+2:
        case EEP_CAL_LVARRAY+4:
        case EEP_CAL_LVARRAY+6:
        case EEP_CAL_LVARRAY+8:
        case EEP_CAL_LVARRAY+10:
        case EEP_CAL_LVARRAY+12:
        case EEP_CAL_LVARRAY+14:
        case EEP_CAL_LVARRAY+16:
        case EEP_CAL_LVARRAY+18:
        case EEP_CAL_HVARRAY:
        case EEP_CAL_HVARRAY+2:
        case EEP_CAL_HVARRAY+4:
        case EEP_CAL_HVARRAY+6:
        case EEP_CAL_HVARRAY+8:
        case EEP_CAL_HVARRAY+10:
        case EEP_CAL_HVARRAY+12:
        case EEP_CAL_HVARRAY+14:
        case EEP_CAL_HVARRAY+16:
        case EEP_CAL_HVARRAY+18:
            
            ptr =(uint8_t *)&EEPROMData.cal;
            offset =addr - EEP_COM_CAL_ADDR;
            if(rw == COM_EEP_WRITE_MODE){
                //16bit 는 little endian를 변환                
                *(ptr+offset) = *(data+1);
                EEprom_WriteByte(addr, *(data+1));
                *(ptr+offset+1) = *(data);
                EEprom_WriteByte(addr+1, *(data));
                updateCalibBlockChkSum();                
            }else{
                *data = *(ptr+offset+1);
                *(data+1) = *(ptr+offset);
            }
            return 2;
         case EEP_CAL_CC1:
         case EEP_CAL_CC2:
         case EEP_CAL_CC3:
         case EEP_CAL_DC1:
         case EEP_CAL_DC2:
         case EEP_CAL_DC3:
             ptr =(uint8_t *)&EEPROMData.cal;
             offset =addr - EEP_COM_CAL_ADDR;
             if(rw == COM_EEP_WRITE_MODE){
                 //16bit 는 little endian를 변환                
                 *(ptr+offset) = *(data+3);
                 EEprom_WriteByte(addr    , *(data+3));
                 *(ptr+offset+1) = *(data+2);
                 EEprom_WriteByte(addr+1 , *(data+2));
                 *(ptr+offset+2) = *(data+1);
                 EEprom_WriteByte(addr+2 , *(data+1));
                 *(ptr+offset+3) = *(data);
                 EEprom_WriteByte(addr+3 , *(data));
                 updateCalibBlockChkSum();                                 
             }else{
                 *data    = *(ptr+offset+3);
                 *(data+1) = *(ptr+offset+2);
                 *(data+2) = *(ptr+offset+1);
                 *(data+3) = *(ptr+offset);
             }
            
            return 4;   
         case EEP_CAL_CHKSUM:
            if(rw == COM_EEP_WRITE_MODE){
                return -1;
            }
            ptr = (uint8_t *)&EEPROMData.cal_chksum;
            *data = *(ptr);
            *(data+1) = *(ptr+1);
            return 2;
        /*    
         case EEP_DIAG_PUSHCNT:
            if(rw == COM_EEP_WRITE_MODE){
                EEprom_WriteByte(addr, *(data));
                EEPROMData.diagcnt = *data;
            }else{
                *data = EEPROMData.diagcnt;
            }
            return 1;
         */
        case EEP_MG_SOC5P:
        case EEP_MG_DCHGSCV:
        case EEP_MG_CWCV:
        case EEP_MG_PDWNCV:
        case EEP_MG_FCCV:
        case EEP_MG_FCTV:
        case EEP_MG_FCWJCV:
        case EEP_MG_FCJC:
        case EEP_MG_DCHGJC:
        case EEP_MG_CHGJC:
            ptr =(uint8_t *)&EEPROMData.mgconf;
            offset =addr - EEP_MG_CONF_ADDR;
            if(rw == COM_EEP_WRITE_MODE){
                //16bit 는 little endian를 변환                
                *(ptr+offset) = *(data+1);
                EEprom_WriteByte(addr, *(data+1));
                *(ptr+offset+1) = *(data);
                EEprom_WriteByte(addr+1, *(data));
            }else{
                *data = *(ptr+offset+1);
                *(data+1) = *(ptr+offset);
            }
            return 2;
        case EEP_SOC_000PER:
        case EEP_SOC_005PER:
        case EEP_SOC_010PER:
        case EEP_SOC_015PER:
        case EEP_SOC_020PER:
        case EEP_SOC_025PER:
        case EEP_SOC_030PER:
        case EEP_SOC_035PER:
        case EEP_SOC_040PER:
        case EEP_SOC_045PER:
        case EEP_SOC_050PER:
        case EEP_SOC_055PER:
        case EEP_SOC_060PER:
        case EEP_SOC_065PER:
        case EEP_SOC_070PER:
        case EEP_SOC_075PER:
        case EEP_SOC_080PER:
        case EEP_SOC_085PER:
        case EEP_SOC_090PER:
        case EEP_SOC_095PER:
        case EEP_SOC_100PER:
        case EEP_SOC_CELLCAP:
            ptr =(uint8_t *)&EEPROMData.soccnf;
            offset =addr - EEP_SOCTBLE_ADDR;
            if(rw == COM_EEP_WRITE_MODE){
                //16bit 는 little endian를 변환                
                *(ptr+offset) = *(data+1);
                EEprom_WriteByte(addr, *(data+1));
                *(ptr+offset+1) = *(data);
                EEprom_WriteByte(addr+1, *(data));
            }else{
                *data = *(ptr+offset+1);
                *(data+1) = *(ptr+offset);
            }
            return 2;
        case EEP_SOC_TOTALCAP:
        case EEP_SOC_TOTALENG:
             ptr =(uint8_t *)&EEPROMData.soccnf;
             offset =addr - EEP_SOCTBLE_ADDR;
             if(rw == COM_EEP_WRITE_MODE){
                 //16bit 는 little endian를 변환                
                 *(ptr+offset) = *(data+3);
                 EEprom_WriteByte(addr    , *(data+3));
                 *(ptr+offset+1) = *(data+2);
                 EEprom_WriteByte(addr+1 , *(data+2));
                 *(ptr+offset+2) = *(data+1);
                 EEprom_WriteByte(addr+2 , *(data+1));
                 *(ptr+offset+3) = *(data);
                 EEprom_WriteByte(addr+3 , *(data));
                 updateCalibBlockChkSum();                                 
             }else{
                 *data    = *(ptr+offset+3);
                 *(data+1) = *(ptr+offset+2);
                 *(data+2) = *(ptr+offset+1);
                 *(data+3) = *(ptr+offset);
             }
            
            return 4;        
         case EEP_SOH_DAY:
         case EEP_SOH_MON:
         case EEP_SOH_YEAR:
#if ( SOH_NEWLIB_REPLACE == 1)
         case EEP_MSG_SOH:
#endif
            
             ptr =(uint8_t *)&EEPROMData.soh;
             offset =addr - EEP_COM_SOH_ADDR;
            if(rw == COM_EEP_WRITE_MODE){
                //16bit 는 little endian를 변환                
                *(ptr+offset) = *(data+1);
                EEprom_WriteByte(addr, *(data+1));
                *(ptr+offset+1) = *(data);
                EEprom_WriteByte(addr+1, *(data));
            }else{
                *data = *(ptr+offset+1);
                *(data+1) = *(ptr+offset);
            }
             return 2;

         case EEP_RECDATA_BASE_ADDR:
            ptr = (uint8_t *)&EEPROMData.recdf;
            offset = 0;
            if(rw == COM_EEP_WRITE_MODE){
                *(ptr+offset) = *(data);
                EEprom_WriteByte(addr, *(data));
                *(ptr+offset+1) = *(data+1);
                EEprom_WriteByte(addr+1, *(data+1));
            }else{
                *data = *(ptr+offset);
                *(data+1) = *(ptr+offset+1);
            }            
            break;
         default:
            if(addr >= EEP_DIAG_DATA1 && addr <= EEP_DIAG_DATAEND)
            {
                if(rw == COM_EEP_WRITE_MODE){
                    for(i=0;i<len;i++){
                        EEprom_WriteByte(addr+i, *(data+i));
                    }
                }else{
                    for(i=0;i<len;i++){
                        *(data+i) = EEprom_ReadByte(addr+i);
                    }
                }
                return len;
            }
            if(addr >= EEP_RECDATA0_BASE_ADDR && addr <= EEP_RECDATA0_BASE_ADDR+REC_D_SEC_N*sizeof(UNI_REC_D))
            {
                if(rw == COM_EEP_WRITE_MODE){
                    for(i=0;i<len;i++){
                        EEprom_WriteByte(addr+i, *(data+i));
                    }
                }else{
                    for(i=0;i<len;i++){
                       *(data+i) = EEprom_ReadByte(addr+i);
                    }
                }
            }
            *data = 0;
            break;
    }
    return 0;
}

static void readEEPRomDataAllFromDevice(void)
{
    EEprom_ReadBuf(EEP_COM_BASE_ADDR, (uint8_t*)&EEPROMData.base, sizeof(EEP_COM_BASE));
    EEprom_ReadBuf(EEP_COM_BLDMGR_ADDR, (uint8_t*)&EEPROMData.bldmgr, sizeof(EEP_COM_BLDMGR));
    EEprom_ReadBuf(EEP_COM_NETMGR_ADDR, (uint8_t*)&EEPROMData.netmgr, sizeof(EEP_COM_NETMGR));
    EEprom_ReadBuf(EEP_COM_BMICONF_ADDR, (uint8_t*)&EEPROMData.bmiconf, sizeof(EEP_COM_BMICONF));

    EEprom_ReadBuf(EEP_MG_CONF_ADDR, (uint8_t*)&EEPROMData.mgconf,sizeof(EEP_MG_CONF));   
    EEprom_ReadBuf(EEP_SOCTBLE_ADDR, (uint8_t*)&EEPROMData.soccnf,sizeof(EEP_SOC_CONF));   
    EEprom_ReadBuf(EEP_RECDATA_BASE_ADDR, (uint8_t*)&EEPROMData.recdf,sizeof(EEP_REC_DATA));   
    EEprom_ReadBuf(EEP_COM_CAL_ADDR, (uint8_t*)&EEPROMData.cal, sizeof(EEP_COM_CAL)); 
    EEprom_ReadBuf(EEP_COM_EVTLOG_ADDR, (uint8_t*)&EEPROMData.evlog, sizeof(EEP_COM_EVTLOG));
    /*
    EEprom_ReadBuf(EEP_COM_CAL_ADDR, (uint8_t*)&EEPROMData.cal, sizeof(EEP_COM_CAL));
    EEprom_ReadBuf(EEP_COM_SOH_ADDR, (uint8_t*)&EEPROMData.soh, sizeof(EEP_COM_SOH));
    EEprom_ReadBuf(EEP_DIAG_BASE_ADDR, (uint8_t*)&EEPROMData.diagcnt, 1);
    for(j=0;j<TRAY_NUMS;j++){
        addr = EEP_TRAY1_BASE_ADDR+ 200*j;
        ptr = (uint8_t *)&EEPROMData.tray[j];
        EEprom_ReadBuf(addr, ptr, (EEP_TRAY1_END_ADDR-EEP_TRAY1_BASE_ADDR) );
    }
    */
       
}

void WritetoEEPROM(uint32_t StartAddress, uint16_t size, int16_t Value)
{
	uint16_t i=0;
	uint16_t WriteData=0;
	UNI_int16Data Data;

	Data.all=Value;
	for(i=0; i < size ; i++)
	{
		if(i%2 ==0)
		{
			WriteData=Data.byte.LSB;
		}else
		{
			WriteData=Data.byte.MSB;
		}
		EEprom_WriteByte((uint32_t)(StartAddress+i),(uint8_t)WriteData);
		DelayUs(EEPROM_WRITE_DELAY);
	}
}



PUBLIC void EEP_FactoryResetEEPRom(uint8_t first)
{
    EEP_COM_BASE tmp;
    updateEEPRomFactoryDataAll();
    EEprom_WriteBuf(EEP_COM_BASE_ADDR, (uint8_t*)&EEPROMData.base,sizeof(EEP_COM_BASE));
    EEprom_ReadBuf(EEP_COM_BASE_ADDR, (uint8_t *)&tmp, sizeof(EEP_COM_BASE));
    EEprom_WriteBuf(EEP_COM_BLDMGR_ADDR,(uint8_t*) &EEPROMData.bldmgr,sizeof(EEP_COM_BLDMGR));
    EEprom_WriteBuf(EEP_COM_NETMGR_ADDR, (uint8_t*)&EEPROMData.netmgr,sizeof(EEP_COM_NETMGR));
    EEprom_WriteBuf(EEP_COM_BMICONF_ADDR, (uint8_t*)&EEPROMData.bmiconf,sizeof(EEP_COM_BMICONF));
    updateCommonBlockChkSum();
    EEprom_WriteBuf(EEP_MG_CONF_ADDR, (uint8_t*)&EEPROMData.mgconf,sizeof(EEP_MG_CONF));  
    EEprom_WriteBuf(EEP_SOCTBLE_ADDR, (uint8_t*)&EEPROMData.soccnf,sizeof(EEP_SOC_CONF));  

    EEprom_WriteBuf(EEP_RECDATA_BASE_ADDR, (uint8_t*)&EEPROMData.recdf,sizeof(EEP_REC_DATA));  
    /*    
    EEprom_WriteBuf(EEP_COM_EVTLOG_ADDR, (uint8_t*)&EEPROMData.evlog,sizeof(EEP_COM_EVTLOG));
    */
    if( first == 1) { // Calibration 까지 초기화 한다. 
        memcpy((char *)&EEPROMData.cal, (const char *)&ComCal, sizeof(EEP_COM_CAL)); 
        EEprom_WriteBuf(EEP_COM_CAL_ADDR, (uint8_t *)&EEPROMData.cal, sizeof(EEP_COM_CAL));
        //EEprom_WriteBuf(EEP_COM_SOH_ADDR, (uint8_t *)&EEPROMData.soh, sizeof(EEP_COM_SOH));
        updateCalibBlockChkSum();
    }
}

PUBLIC void EEP_ChangeAccessFromPCTool(uint8_t access)
{
    EEPROMData.access = access;
}

PUBLIC void EEP_ReadByteFromPCTool(uint32_t *buf)
{
    uint8_t data[16];
    int ret;
    PCINTER_MSG* p = (PCINTER_MSG*)buf;
    PCINTER_MSG* s;
    if(p== NULL) return;
    ret = comEEPRom(1, p->addr ,COM_EEP_READ_MODE , data, p->size);
    s = PCInter_AllocMessage(EEPROM_SCHEDULER_IDX);
    if(!s) return;
    if(ret < 0){
        s->fail = 1;
    }else{
        s->fail=0;
        s->addr = p->addr;
        memcpy(s->data, data, p->size);
        s->size = p->size;
    }    
    PushJob(CANB_SCHEDULER_IDX, RET_READBYTE_FROM_EEP, CANBSCH_TIME, s);
}
PUBLIC void EEP_WriteByteFromPCTool(uint32_t *buf)
{
    uint8_t data[16];
    int ret;
    PCINTER_MSG* p = (PCINTER_MSG*)buf;
    PCINTER_MSG* s;
    if(p== NULL) return;
    memcpy(data, p->data, p->size);
    ret = comEEPRom(1, p->addr ,COM_EEP_WRITE_MODE , data, p->size);
    s = PCInter_AllocMessage(EEPROM_SCHEDULER_IDX);
    if(!s) return;
    if(ret < 0){
        s->fail = 1;
    }else{
        s->fail=0;
        s->addr = p->addr;
        s->size = p->size;
        s->idx = p->idx;
    }    
    PushJob(CANB_SCHEDULER_IDX, RET_WRITEBYTE_FROM_EEP, CANBSCH_TIME, s);

}

#if 1
PUBLIC void EEP_WriteByteFromDiag(void *buf)
{
    int i;
    STR_CAN_MSG *r = (STR_CAN_MSG *)buf;
    STR_CAN_MSG *s;
    if(r==NULL) return;
    EEPROMData.access = 1;
    for(i=0; i< r->test.eep.len; i++){
        if(r->test.eep.data[i].len > 4) 
            continue;
        comEEPRom(1, r->test.eep.data[i].addr, COM_EEP_WRITE_MODE, r->test.eep.data[i].value, r->test.eep.data[i].len);
    }
    EEPROMData.access = 0;
    s = (STR_CAN_MSG *)can_com_AllocMessage(EEPROM_SCHEDULER_IDX);
    if(!s) return;
    s->lid = r->lid;
    s->sid = r->sid;
    PushJob(CANB_SCHEDULER_IDX, WRBYTE_DIAG_SUCCESS_FROM_EEP, 0, s);
}

PUBLIC void EEP_ReadByteFromDiag(void *buf)
{
    int i;
    STR_CAN_MSG *r = (STR_CAN_MSG *)buf;
    STR_CAN_MSG *s= (STR_CAN_MSG *)can_com_AllocMessage(EEPROM_SCHEDULER_IDX);
    if(!s) return;
    s->lid = r->lid;
    s->sid = r->sid;
    s->test.eep.len = r->test.eep.len;
    EEPROMData.access = 1;
    for(i=0;i< r->test.eep.len;i++){
        if(r->test.eep.data[i].len > 4)
            continue;
        s->test.eep.data[i].addr = r->test.eep.data[i].addr;
        s->test.eep.data[i].len = r->test.eep.data[i].len;
        comEEPRom(1, s->test.eep.data[i].addr, COM_EEP_READ_MODE, s->test.eep.data[i].value, s->test.eep.data[i].len);
    }
    EEPROMData.access = 0;
    PushJob(CANB_SCHEDULER_IDX, RDBYTE_DIAG_SUCCESS_FROM_EEP, 0, s);
}

PUBLIC void EEP_ReadModelFromDiag(void *buf)
{
    STR_CAN_MSG *r = (STR_CAN_MSG *)buf;
    STR_CAN_MSG *s= (STR_CAN_MSG *)can_com_AllocMessage(EEPROM_SCHEDULER_IDX);
    if(!s) return;
    s->lid = r->lid;
    s->sid = r->sid;
    EEPROMData.access = 1;
    comEEPRom(1, EEP_COM_BASE_MN ,COM_EEP_READ_MODE , s->test.hm.mn, 16);
    comEEPRom(1, EEP_COM_BASE_MV ,COM_EEP_READ_MODE , s->test.hm.mv, 8);
    EEPROMData.access = 0;
    PushJob(CANB_SCHEDULER_IDX, READ_MODEL_SUCCESS_FROM_EEP, 0, s);    
}

PUBLIC void EEP_ReadVerFromDiag(void *buf)
{
    STR_CAN_MSG *r = (STR_CAN_MSG *)buf;
    STR_CAN_MSG *s= (STR_CAN_MSG *)can_com_AllocMessage(EEPROM_SCHEDULER_IDX);
    if(!s) return;
    s->lid = r->lid;
    s->sid = r->sid;
    EEPROMData.access = 1;
    comEEPRom(1, EEP_COM_BLDMGR_SW_MAJOR ,COM_EEP_READ_MODE , &s->test.sm.ver[0],   1);
    comEEPRom(1, EEP_COM_BLDMGR_SW_MINOR ,COM_EEP_READ_MODE , &s->test.sm.ver[1],   1);
    comEEPRom(1, EEP_COM_BLDMGR_BLDYR   ,COM_EEP_READ_MODE  , &s->test.sm.build[0], 1);
    comEEPRom(1, EEP_COM_BLDMGR_BLDMON  ,COM_EEP_READ_MODE  , &s->test.sm.build[1], 1);
    comEEPRom(1, EEP_COM_BLDMGR_BLDDAY  ,COM_EEP_READ_MODE  , &s->test.sm.build[2], 1);
    EEPROMData.access = 0;
    PushJob(CANB_SCHEDULER_IDX, READ_VER_SUCCESS_FROM_EEP, 0, s);    
}

#endif
void EEP_RTCSet(void)
{
#if 0
    RTC_SetTime();
    RTC_UpdateTime();
#endif    
}

void EEP_BuringSOH(void)
{


}

void EEP_UpdateUseData(void)
{
    uint8_t idx;
    int len;
    uint32_t eep_addr;
    uint8_t *data;
    EEPROMData.access = 1;
        EEPROMData.recdf.last_idx = (EEPROMData.recdf.last_idx+1)% REC_D_SEC_N ; 
        idx = EEPROMData.recdf.last_idx;
        len = sizeof(UNI_REC_D);
        //memcpy(&EEPROMData.recdf.sector[idx],&RTRecDF, len );
        eep_addr = EEP_RECDATA0_BASE_ADDR+ REC_D_SIZE*2*idx;
        data = (uint8_t *)&RTRecDF;
        EEprom_WriteBuf(eep_addr, data , REC_D_SIZE*2);
        EEprom_ReadBuf(eep_addr, (uint8_t *)&EEPROMData.recdf.sector[idx], len );
        len = 4;
        eep_addr = EEP_RECDATA_BASE_ADDR;
        data = (uint8_t *)&EEPROMData.recdf;
        EEprom_WriteBuf(eep_addr, data, len );
    EEPROMData.access = 0;
    
}

void EEP_EraseAll(void)
{
    EEP_FactoryResetEEPRom(1);    
}

void EEP_ClearFaultRecord(void)
{
    EEPROMData.evlog.dtc=0;
    EEPROMData.evlog.sub_code=0;    
    EEPROMData.access = 1;
    comEEPRom(1, EEP_COM_EVTLOG_ADDR , COM_EEP_WRITE_MODE , (uint8_t *)&EEPROMData.evlog,sizeof(EEPROMData.evlog));
    EEPROMData.access = 0;
}


/*
 * EEPROM_ProcessData.h
 *
 *  Created on: 2014. 2. 4.
 *      Author: si14294
 */

#ifndef EEPROM_PROCESSDATA_H_
#define EEPROM_PROCESSDATA_H_
#include "BMS.h"
//#include "Common_HeaderFile.h"
//---------------------------------- Delay & Command Index ---------------------------------------
#define EEPROM_WRITE_DELAY 5000 // us
#define EEPROM_READ_DELAY 5     // us



#define COM_EEP_READ_MODE           1
#define COM_EEP_WRITE_MODE          2



#define EEP_COM_BASE_ADDR    0
    #define EEP_COM_BASE_MN             ( EEP_COM_BASE_ADDR )
    #define EEP_COM_BASE_MV             ( EEP_COM_BASE_MN+16 )
    #define EEP_COM_BASE_SN             ( EEP_COM_BASE_MV+8 )
    #define EEP_COM_BASE_ID             ( EEP_COM_BASE_SN+8 )
    #define EEP_COM_BASE_TRMN           ( EEP_COM_BASE_ID+1 )
    #define EEP_COM_BASE_TRMV           ( EEP_COM_BASE_TRMN+16 )
    #define EEP_COM_BASE_TRCNT          ( EEP_COM_BASE_TRMV+8 )
    #define EEP_COM_BASE_TRCELLCNT      ( EEP_COM_BASE_TRCNT+1 )
    #define EEP_COM_BASE_TRBMICNT       ( EEP_COM_BASE_TRCELLCNT+1 )
    #define EEP_COM_BASE_TRTMPCNT       ( EEP_COM_BASE_TRBMICNT+1 )
    #define EEP_COM_BASE_TRBDTMPCNT     ( EEP_COM_BASE_TRTMPCNT+1 )
    #define EEP_COM_BASE_BMICCELLENACNT ( EEP_COM_BASE_TRBDTMPCNT+1 )
    #define EEP_COM_BASE_BMICCELLMASK0   ( EEP_COM_BASE_BMICCELLENACNT+1 )
    #define EEP_COM_BASE_BMICCELLMASK1  ( EEP_COM_BASE_BMICCELLMASK0+2 )
    #define EEP_COM_BASE_BMICCELLMASK2  ( EEP_COM_BASE_BMICCELLMASK1+2 )
    #define EEP_COM_BASE_BMICCELLMASK3  ( EEP_COM_BASE_BMICCELLMASK2+2 )
    #define EEP_COM_BASE_PAD1           ( EEP_COM_BASE_BMICCELLMASK3+2 )
    #define EEP_COM_BASE_BMICAUXUSE0    ( EEP_COM_BASE_PAD1+8 )
    #define EEP_COM_BASE_BMICAUXUSE1    ( EEP_COM_BASE_BMICAUXUSE0+1 )
    #define EEP_COM_BASE_BMICAUXUSE2    ( EEP_COM_BASE_BMICAUXUSE1+1 )
    #define EEP_COM_BASE_BMICAUXUSE3    ( EEP_COM_BASE_BMICAUXUSE2+1 )
    
    
#define EEP_COM_BLDMGR_ADDR  (EEP_COM_BASE_ADDR+sizeof(EEP_COM_BASE))
    #define EEP_COM_BLDMGR_SW_MAJOR ( EEP_COM_BLDMGR_ADDR )
    #define EEP_COM_BLDMGR_SW_MINOR ( EEP_COM_BLDMGR_SW_MAJOR+1 )
    #define EEP_COM_BLDMGR_BLDYR   ( EEP_COM_BLDMGR_SW_MINOR+1 )
    #define EEP_COM_BLDMGR_BLDMON   ( EEP_COM_BLDMGR_BLDYR+1 )
    #define EEP_COM_BLDMGR_BLDDAY   ( EEP_COM_BLDMGR_BLDMON+1 )
#define EEP_COM_NETMGR_ADDR  (EEP_COM_BLDMGR_ADDR+sizeof(EEP_COM_BLDMGR))
    #define EEP_COM_NETMGR_NIP1     ( EEP_COM_NETMGR_ADDR )
    #define EEP_COM_NETMGR_NIP2     ( EEP_COM_NETMGR_NIP1+1 )
    #define EEP_COM_NETMGR_NIP3     ( EEP_COM_NETMGR_NIP2+1 )
    #define EEP_COM_NETMGR_NIP4     ( EEP_COM_NETMGR_NIP3+1 )
    #define EEP_COM_NETMGR_GIP4     ( EEP_COM_NETMGR_NIP4+1 )
#define EEP_COM_BMICONF_ADDR    (EEP_COM_NETMGR_ADDR+sizeof(EEP_COM_NETMGR))
    #define EEP_COM_BMICONF_OCVP    ( EEP_COM_BMICONF_ADDR )        // 101 deprecated1
    #define EEP_COM_BMICONF_OCVF    ( EEP_COM_BMICONF_OCVP+2 )      // 103 OCVF
    #define EEP_COM_BMICONF_OCVW    (  EEP_COM_BMICONF_OCVF+2 )     // 105 OCVW
    #define EEP_COM_BMICONF_UCVP    (  EEP_COM_BMICONF_OCVW+2 )     // 107 deprecated2
    #define EEP_COM_BMICONF_UCVF    (  EEP_COM_BMICONF_UCVP+2 )     // 109 UCVF
    #define EEP_COM_BMICONF_UCVW    (  EEP_COM_BMICONF_UCVF+2 )     // 111 UCVW
    #define EEP_COM_BMICONF_CVD     ( EEP_COM_BMICONF_UCVW+2 )      // 113 CVDF
    #define EEP_COM_BMICONF_OTP     ( EEP_COM_BMICONF_CVD+2 )       // 115 CVDW
    #define EEP_COM_BMICONF_OTF     ( EEP_COM_BMICONF_OTP+2 )       // 117 OTF
    #define EEP_COM_BMICONF_OTW     ( EEP_COM_BMICONF_OTF+2 )       // 119 OTW
    #define EEP_COM_BMICONF_UTF     ( EEP_COM_BMICONF_OTW+2 )       // 121 UTF
    #define EEP_COM_BMICONF_UTW     ( EEP_COM_BMICONF_UTF+2 )       // 123 UTW
    #define EEP_COM_BMICONF_OSOCF   ( EEP_COM_BMICONF_UTW+2 )       // 125 OSOCF
    #define EEP_COM_BMICONF_OSOCW   ( EEP_COM_BMICONF_OSOCF+2 )     // 127 OSOCW
    #define EEP_COM_BMICONF_USOCF   ( EEP_COM_BMICONF_OSOCW+2 )     // 129 USOCF
    #define EEP_COM_BMICONF_USOCW   ( EEP_COM_BMICONF_USOCF+2 )     // 131 USOCW
    #define EEP_COM_BMICONF_OBVP    ( EEP_COM_BMICONF_USOCW+2 )     // 133 deprecated3
    #define EEP_COM_BMICONF_OBVF    ( EEP_COM_BMICONF_OBVP+2 )      // 135 OBVF
    #define EEP_COM_BMICONF_OBVW    ( EEP_COM_BMICONF_OBVF+2 )      // 137 OBVW
    #define EEP_COM_BMICONF_UBVP    ( EEP_COM_BMICONF_OBVW+2 )      // 139 deprecated4
    #define EEP_COM_BMICONF_UBVF    ( EEP_COM_BMICONF_UBVP+2 )      // 141 UBVF
    #define EEP_COM_BMICONF_UBVW    ( EEP_COM_BMICONF_UBVF+2 )      // 143 UBVW
    #define EEP_COM_BMICONF_OCCP    ( EEP_COM_BMICONF_UBVW+2 )      // 145 deprecated5
    #define EEP_COM_BMICONF_OCCF    ( EEP_COM_BMICONF_OCCP+2 )      // 147 OCCF
    #define EEP_COM_BMICONF_OCCW    ( EEP_COM_BMICONF_OCCF+2 )      // 149 OCCW
    #define EEP_COM_BMICONF_ODCP    ( EEP_COM_BMICONF_OCCW+2 )      // 151 deprecated6
    #define EEP_COM_BMICONF_ODCF    ( EEP_COM_BMICONF_ODCP+2 )      // 153 ODCF
    #define EEP_COM_BMICONF_ODCW    ( EEP_COM_BMICONF_ODCF+2 )      // 155 ODCW
    #define EEP_COM_BMICONF_OCD     ( EEP_COM_BMICONF_ODCW+2 )      // 157 deprecated7
    #define EEP_COM_BMICONF_TDP     ( EEP_COM_BMICONF_OCD+2 )       // 159 deprecated8
    #define EEP_COM_BMICONF_TDF     ( EEP_COM_BMICONF_TDP+2 )       // 161 TDF
    #define EEP_COM_BMICONF_TDW     ( EEP_COM_BMICONF_TDF+2 )       // 163 TDW
    #define EEP_COM_BMICONF_CCOR    ( EEP_COM_BMICONF_TDW+2 )       // 165 deprecated9

#define  EEP_COM_CHKSUM         ( EEP_COM_BMICONF_ADDR+sizeof(EEP_COM_BMICONF)) //167
#define EEP_COM_EVTLOG_ADDR     ( EEP_COM_CHKSUM+2 )                            //141
    #define EEP_COM_EVTLOG_DTC         ( EEP_COM_EVTLOG_ADDR )
    #define EEP_COM_EVTLOG_SUBCODE   ( EEP_COM_EVTLOG_DTC+2 )

#if ( ETH_ENABLE_CDMS == 1)    
#define EEP_COM_CANCNF_ADDR (EEP_COM_EVTLOG_RP+2)
    #define EEP_COM_CANCNF_CCT (EEP_COM_CANCNF_ADDR)
    #define EEP_COM_CANCNF_CAID (EEP_COM_CANCNF_CCT+2)
#endif


#define EEP_COM_CAL_ADDR    300   
    #define EEP_CAL_BREFV1      ( EEP_COM_CAL_ADDR )    // 300
    #define EEP_CAL_BREFV2      ( EEP_CAL_BREFV1+2 )    // 302
    #define EEP_CAL_BREFV3      ( EEP_CAL_BREFV2+2 )    // 304
    #define EEP_CAL_BVC1        ( EEP_CAL_BREFV3+2 )    // 306
    #define EEP_CAL_BVC2        ( EEP_CAL_BVC1+2 )      // 308
    #define EEP_CAL_BVC3        ( EEP_CAL_BVC2+2 )      // 310
    #define EEP_CAL_PREFV1      ( EEP_CAL_BVC3+2 )      // 312
    #define EEP_CAL_PREFV2      ( EEP_CAL_PREFV1+2 )    // 314
    #define EEP_CAL_PREFV3      ( EEP_CAL_PREFV2+2 )    // 316
    #define EEP_CAL_PVC1        ( EEP_CAL_PREFV3+2 )    // 318
    #define EEP_CAL_PVC2        ( EEP_CAL_PVC1+2 )      // 320
    #define EEP_CAL_PVC3        ( EEP_CAL_PVC2+2 )      // 322
    #define EEP_CAL_REFC1       ( EEP_CAL_PVC3+2 )      // 324
    #define EEP_CAL_REFC2       ( EEP_CAL_REFC1+2 )     // 326
    #define EEP_CAL_REFC3       ( EEP_CAL_REFC2+2 )     // 328
    #define EEP_CAL_CC1         ( EEP_CAL_REFC3+2 )     // 330
    #define EEP_CAL_CC2         ( EEP_CAL_CC1+4 )       // 334
    #define EEP_CAL_CC3         ( EEP_CAL_CC2+4 )       // 338
    #define EEP_CAL_DC1         ( EEP_CAL_CC3+4 )       // 342
    #define EEP_CAL_DC2         ( EEP_CAL_DC1+4 )       // 346
    #define EEP_CAL_DC3         ( EEP_CAL_DC2+4 )       // 350
    #define EEP_CAL_GFDP1          ( EEP_CAL_DC3+4 )    // 354
    #define EEP_CAL_GFDP2          ( EEP_CAL_GFDP1+2 )  // 356
    #define EEP_CAL_GFDP3          ( EEP_CAL_GFDP2+2 )  // 358
    #define EEP_CAL_GFDN1          ( EEP_CAL_GFDP3+2 )  // 360
    #define EEP_CAL_GFDN2          ( EEP_CAL_GFDN1+2 )  // 362
    #define EEP_CAL_GFDN3          ( EEP_CAL_GFDN2+2 )  // 364
    #define EEP_CAL_LVREF          ( EEP_CAL_GFDN3+2 )  // 366
    #define EEP_CAL_HVREF          ( EEP_CAL_LVREF+2 )  // 368
    #define EEP_CAL_LVARRAY        ( EEP_CAL_HVREF+2 )  // 370
    #define EEP_CAL_HVARRAY        ( EEP_CAL_LVARRAY+20 )  //390
    #define EEP_CAL_END            EEP_CAL_HVARRAY+20   //410


#define EEP_CAL_CHKSUM         450
#if (EEP_CAL_END >= EEP_CAL_CHKSUM )
    #error "Address overflow"
#endif

#define EEP_COM_SOH_ADDR    460
    #define EEP_SOH_YEAR        ( EEP_COM_SOH_ADDR )
    #define EEP_SOH_MON        ( EEP_SOH_YEAR + 2)
    #define EEP_SOH_DAY         ( EEP_SOH_MON + 2)
    #define EEP_SOH_SOH         ( EEP_SOH_DAY + 2)
#if ( SOH_NEWLIB_REPLACE == 1)
    #define EEP_MSG_SOH        ( EEP_SOH_DAY+2)
#endif


#define EEP_MG_CONF_ADDR    500
    #define EEP_MG_SOC5P    ( EEP_MG_CONF_ADDR )    //2
    #define EEP_MG_DCHGSCV  ( EEP_MG_SOC5P+2 )      //4
    #define EEP_MG_CWCV     ( EEP_MG_DCHGSCV+2 )    //6
    #define EEP_MG_PDWNCV   ( EEP_MG_CWCV+2 )       //8
    #define EEP_MG_FCCV     ( EEP_MG_PDWNCV+2 )     //10
    #define EEP_MG_FCTV     ( EEP_MG_FCCV+2 )       //12
    #define EEP_MG_FCWJCV    ( EEP_MG_FCTV+2 )      //14
    #define EEP_MG_FCJC     ( EEP_MG_FCWJCV+2 )     //16
    #define EEP_MG_DCHGJC   ( EEP_MG_FCJC+2 )       //18
    #define EEP_MG_CHGJC    ( EEP_MG_DCHGJC+2 )     //20

#define EEP_SOCTBLE_ADDR   550
    #define EEP_SOC_000PER      ( EEP_SOCTBLE_ADDR )
    #define EEP_SOC_005PER      ( EEP_SOC_000PER+2 )
    #define EEP_SOC_010PER      ( EEP_SOC_005PER+2 )
    #define EEP_SOC_015PER      ( EEP_SOC_010PER+2 )
    #define EEP_SOC_020PER      ( EEP_SOC_015PER+2 )
    #define EEP_SOC_025PER      ( EEP_SOC_020PER+2 )
    #define EEP_SOC_030PER      ( EEP_SOC_025PER+2 )
    #define EEP_SOC_035PER      ( EEP_SOC_030PER+2 )
    #define EEP_SOC_040PER      ( EEP_SOC_035PER+2 )
    #define EEP_SOC_045PER      ( EEP_SOC_040PER+2 )
    #define EEP_SOC_050PER      ( EEP_SOC_045PER+2 )
    #define EEP_SOC_055PER      ( EEP_SOC_050PER+2 )
    #define EEP_SOC_060PER      ( EEP_SOC_055PER+2 )
    #define EEP_SOC_065PER      ( EEP_SOC_060PER+2 )
    #define EEP_SOC_070PER      ( EEP_SOC_065PER+2 )
    #define EEP_SOC_075PER      ( EEP_SOC_070PER+2 )
    #define EEP_SOC_080PER      ( EEP_SOC_075PER+2 )
    #define EEP_SOC_085PER      ( EEP_SOC_080PER+2 )
    #define EEP_SOC_090PER      ( EEP_SOC_085PER+2 )
    #define EEP_SOC_095PER      ( EEP_SOC_090PER+2 )
    #define EEP_SOC_100PER      ( EEP_SOC_095PER+2 )
    #define EEP_SOC_CELLCAP     ( EEP_SOC_100PER+2 )
    #define EEP_SOC_TOTALCAP    ( EEP_SOC_CELLCAP+2 )
    #define EEP_SOC_TOTALENG    ( EEP_SOC_TOTALCAP+4 )

    
#define EEP_DIAG_BASE_ADDR      1000
    #define EEP_DIAG_PUSHCNT    ( EEP_DIAG_BASE_ADDR )
    #define EEP_DIAG_DATA1      ( EEP_DIAG_PUSHCNT+2)
    #define EEP_DIAG_DATAEND    ( EEP_DIAG_BASE_ADDR+400 )  //50개 

#define EEP_RECDATA_BASE_ADDR     2000
   #define  EEP_RECDATA_LASTIDX    ( EEP_RECDATA_BASE_ADDR )
   #define  EEP_RECDATA_FLAG        ( EEP_RECDATA_LASTIDX + 1 )
   #define  EEP_RECDATA_SOH        ( EEP_RECDATA_FLAG + 1 )
   #define  EEP_RECDATA_SOC        ( EEP_RECDATA_SOH + 1 )
   #define  EEP_RECDATA0_BASE_ADDR ( EEP_RECDATA_SOC + 1 )


extern STR_EEPROM_DataModel EEPROMData;

void EEP_FactoryResetEEPRom(uint8_t);
void WritetoEEPROM(uint32_t StartAddres, uint16_t Size, int16_t Vaule);
extern void Init_EEPROM(void);
extern void Read_All_EEPROM_Data(void);
void Read_EEPROM_Voltage_Data(void);
extern void EEP_ChangeAccessFromPCTool(uint8_t access);
extern void EEP_ReadByteFromPCTool(uint32_t *buf);
extern void EEP_WriteByteFromPCTool(uint32_t *buf);
extern void EEP_WriteByteFromDiag(void *buf);
extern void EEP_ReadByteFromDiag(void *buf);
extern void EEP_ReadModelFromDiag(void *buf);
extern void EEP_ReadVerFromDiag(void *buf);

extern void EEP_RTCSet(void);
extern void EEP_BuringSOH(void);
extern void EEP_EraseAll(void);
extern void EEP_ClearFaultRecord(void);
extern void EEP_UpdateUseData(void);
extern void EEP_ClearEventLog(void);
void EEP_WriteByteFromR10MS(void *buf);
void EEP_BackGroundDevDataUpdate(void);


extern void CellNTemp_EEpromUpdate_Check(void);
	void Update_OVnUVCount_toEEPROM(uint16_t DeviceNum, uint16_t CellNum);
	void Update_VolatgeValue_toEEPROM(uint16_t DeviceNum, uint16_t CellNum);
	void Update_ModuleOTnUTCount_toEEPROM(uint16_t DeviceNum);
	void Update_ModuleTemperatureValue_toEEPROM(uint16_t DeviceNum);
	void Update_BoardOTnUTCount_toEEPROM(uint16_t DeviceNum);
	void Update_BoardTemperatureValue_toEEPROM(uint16_t DeviceNum);

#endif /* EEPROM_PROCESSDATA_H_ */

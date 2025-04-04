/**
    ******************************************************************************
    *     @file         SYS_ERR_CHK.c
    *     @version
    *     @author
    *     @date      2017/09/12
    *     @brief     DTC 조건 검색 및 WARNING, FAULT, PROTECT 결정
    ******************************************************************************
    */

#include "obshal.h"
#include "BMS.h"
#include "SYS_ERR_CHK.h"
#include "eeprom_processdata.h"
#include "eeprom_Control.h"
#include "rack_sys.h"
#include "master.h"
#include "sm.h"

STR_ErrorModel DTC_OVER_BATVOL;
STR_ErrorModel DTC_UNDER_BATVOL;
STR_ErrorModel DTC_EachCell_OV[TBMS_BUF_SIZE][CELL_BUF_SIZE];
STR_ErrorModel DTC_EachCell_UV[TBMS_BUF_SIZE][CELL_BUF_SIZE];
STR_ErrorModel DTC_EachTemp_OT[TBMS_BUF_SIZE][TMP_BUF_SIZE];
STR_ErrorModel DTC_EachTemp_UT[TBMS_BUF_SIZE][TMP_BUF_SIZE];
STR_ErrorModel DTC_Temp_Deviation;
STR_ErrorModel DTC_CellV_Deviation;
STR_ErrorModel DTC_ISOL_ERR[2];
STR_ErrorModel DTC_TBMS_COM_ERR;
STR_ErrorModel DTC_MBMS_COM_ERR;
STR_ErrorModel DTC_RELAY_ERR;
STR_ErrorModel DTC_FUSE_ERR;
STR_ErrorModel DTC_Curr_OVCHG1;
STR_ErrorModel DTC_Curr_OVDCHG1;
STR_ErrorModel DTC_OVER_SOC;
STR_ErrorModel DTC_UNDER_SOC;

//Safety Mech. DTC
STR_SMErrorModel DTC_SM;


extern uint16_t TEMP_DEV;
extern void RelayControl_initialize(void);

struct DTC_Memory DTC_Mem[NUM_DTC] =
{
    {DTC_BAT_OV_W,          WARNING, NotDeteced,  Release,   Alarm},            // Battery Over Voltage
    {DTC_BAT_OV_F,          FAULT,   NotDeteced,  NoRelease, RelayOpen},        // Battery Over Voltage
    {DTC_BAT_UV_W,          WARNING, NotDeteced,  Release,   Alarm},            // Battery Under Voltage
    {DTC_BAT_UV_F,          FAULT,   NotDeteced,  NoRelease, RelayOpen},        // Battery Under Voltage
    {DTC_CELL_OV_W,         WARNING, NotDeteced,  Release,   Alarm},            // Cell OV Waring
    {DTC_CELL_OV_F,         FAULT,   NotDeteced,  NoRelease, RelayOpen},        // Cell OV Fault
    {DTC_CELL_UV_W,         WARNING, NotDeteced,  Release,   Alarm},            // Cell UV Waring
    {DTC_CELL_UV_F,         FAULT,   NotDeteced,  NoRelease, RelayOpen},        // Cell UV Fault
    {DTC_MOD_OT_W,          WARNING, NotDeteced,  Release,   Alarm},            // Cell OT Waring
    {DTC_MOD_OT_F,          FAULT,   NotDeteced,  NoRelease, RelayOpen},        // Cell OT Fault
    {DTC_MOD_UT_W,          WARNING, NotDeteced,  Release,   Alarm},            // Cell UT Waring
    {DTC_MOD_UT_F,          FAULT,   NotDeteced,  Release,   Alarm},            // Cell UT Fault
    {DTC_TEMP_DEVIATION_W,  WARNING, NotDeteced,  Release,   Alarm},            // Moduel Temp Deviation Warning
    {DTC_TEMP_DEVIATION_F,  FAULT,   NotDeteced,  NoRelease, Alarm},            // Moduel Temp Deviation Fault
    {DTC_CELL_DEVIATION_W,  WARNING, NotDeteced,  Release,   Alarm},            // Cell Deviation Warning
    {DTC_CELL_DEVIATION_F,  FAULT,   NotDeteced,  NoRelease, Alarm},            // Cell Deviation Fault
    {DTC_ISOL_W,            WARNING, NotDeteced,  Release,   Alarm},            // GFD, Isolation Failure
    {DTC_ISOL_F,            FAULT,   NotDeteced,  NoRelease, RelayOpen},        // GFD, Isolation Failure
    {DTC_TBMS_RX_W,         WARNING, NotDeteced,  Release,   Alarm},            // TrayBMS RX Status
    {DTC_TBMS_RX_F,         FAULT,   NotDeteced,  Release,   RelayOpen},        // TrayBMS RX Status
    {DTC_MBMS_RX_W,         WARNING, NotDeteced,  Release,   Alarm},            // Master BMS RX Status
    {DTC_MBMS_RX_F,         FAULT,   NotDeteced,  Release,   RelayOpen},        // Master BMS RX Status
    {DTC_RELAY_W,           WARNING, NotDeteced,  Release,   Alarm},            // Relay Fault
    {DTC_RELAY_F,           FAULT,   NotDeteced,  NoRelease, RelayOpen},        // Relay Fault
    {DTC_FUSE_W,            WARNING, NotDeteced,  Release,   Alarm},            // Fuse Fault
    {DTC_FUSE_F,            FAULT,   NotDeteced,  NoReleaseHold,  RelayOpen},   // Fuse Fault
    {DTC_OV_CHRG_CURR_W,    WARNING, NotDeteced,  Release,   Alarm},            // Over Charge Current Warning 1
    {DTC_OV_CHRG_CURR_F,    FAULT,   NotDeteced,  NoRelease,   RelayOpen},        // Over Charge Current Fault 1
    {DTC_OV_DSCHRG_CURR_W,  WARNING, NotDeteced,  Release,   Alarm},            // Over Discharge Current Warning 1
    {DTC_OV_DSCHRG_CURR_F,  FAULT,   NotDeteced,  NoRelease,   RelayOpen},        // Over Discharge Current Fault 1
    {DTC_OVER_SOC_W,        WARNING, NotDeteced,  Release,   Alarm},            // Over SOC Warning
    {DTC_OVER_SOC_F,        FAULT,   NotDeteced,  Release,   Alarm},            // Over Soc Fault
    {DTC_UNDER_SOC_W,       WARNING, NotDeteced,  Release,   Alarm},            // Under SOC Warning
    {DTC_UNDER_SOC_F,       FAULT,   NotDeteced,  Release,   Alarm} ,           // Under SOC Fault
    {DTC_SM_W,              WARNING, NotDeteced,  Release,   Alarm} ,           // Safety Mechanism Warning
    {DTC_SM_F,              FAULT,   NotDeteced,  NoRelease, RelayOpen}         // Safety Mechanism Fault

};

PUBLIC void ClearDTCTable(void)
{
    uint16_t mBmsIdx,cellIdx,TempIdx = 0;

    DTC_OVER_BATVOL.Status.all = 0;
    DTC_OVER_BATVOL.time.Set = 0;

    DTC_UNDER_BATVOL.Status.all = 0;
    DTC_UNDER_BATVOL.time.Set = 0;

    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS; mBmsIdx++)
    {
        for (cellIdx = 0; cellIdx < TRAY_CELL_NUMS; cellIdx++)
        {
            DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all = eachNormal;
            DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = 0;

            DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all = eachNormal;
            DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = 0;
        }

        for(TempIdx = 0; TempIdx < MODULE_TEMP_SENSORNUM; TempIdx++)
        {
            DTC_EachTemp_OT[mBmsIdx][TempIdx].Status.all = eachNormal;
            DTC_EachTemp_OT[mBmsIdx][TempIdx].time.Set = 0;

            DTC_EachTemp_UT[mBmsIdx][TempIdx].Status.all = eachNormal;
            DTC_EachTemp_UT[mBmsIdx][TempIdx].time.Set = 0;
        }
    }
    DTC_Temp_Deviation.Status.all = 0;
    DTC_Temp_Deviation.time.Set = 0;

    DTC_CellV_Deviation.Status.all = 0;
    DTC_CellV_Deviation.time.Set = 0;

    DTC_ISOL_ERR[0].Status.all = 0;
    DTC_ISOL_ERR[0].time.Set = 0;
    DTC_ISOL_ERR[1].Status.all = 0;
    DTC_ISOL_ERR[1].time.Set = 0;

    DTC_TBMS_COM_ERR.Status.all = 0;
    DTC_TBMS_COM_ERR.time.Set = 0;
    DTC_MBMS_COM_ERR.Status.all = 0;
    DTC_MBMS_COM_ERR.time.Set = 0;
    DTC_RELAY_ERR.Status.all = 0;
    DTC_RELAY_ERR.time.Set = 0;
    DTC_FUSE_ERR.Status.all = 0;
    DTC_FUSE_ERR.time.Set = 0;
    DTC_Curr_OVCHG1.Status.all = 0;
    DTC_Curr_OVCHG1.time.Set = 0;
    DTC_Curr_OVDCHG1.Status.all = 0;
    DTC_Curr_OVDCHG1.time.Set = 0;

    DTC_OVER_SOC.Status.all = 0;
    DTC_OVER_SOC.time.Set = 0;
    DTC_UNDER_SOC.Status.all = 0;
    DTC_UNDER_SOC.time.Set = 0;
    DTC_SM.DtcErr.Status.all = 0;
}
#if 0
void _Diag_Update_Huge_Current()
{
    uint32_t DT;
    uint32_t ocd_detect = CUR_CTL.ocd_on;

    if (DTC_Curr_Huge.Status.all == eachNormal)
    {
        if (ocd_detect)
        {
            hLogProc(LOG_DTC, "ocd p+ \n");
            if (!DTC_Curr_Huge.time.Set)
            {
                DTC_Curr_Huge.time.Set = OCD_PROTECT_WTIME;
            	DTC_Curr_Huge.time.Start = hTimCurrentTime();
            }
            else
            {
                DT = (uint32_t)(hTimCurrentTime() - DTC_Curr_Huge.time.Start);
                if (DT >= DTC_Curr_Huge.time.Set)
                {
                    hLogProc(LOG_DTC, "ocd -->protect \n");
                    DTC_Curr_Huge.Status.all = Protect;
                    DTC_Curr_Huge.time.Set = 0;
                }
            }
        }
        else
        {
            DTC_Curr_Huge.time.Set = 0;
            RackSys_StringExtEvent(STREXTEV_OCD_ALARM, 0);
        }
    }

    if (DTC_Curr_Huge.Status.all == Protect)    // Protected  status  구현
    {
        //Relay off
        processRelayOffFromDTC();
        RackSys_StringExtEvent(STREXTEV_OCD_ALARM, 1);
        RackSys_PushDiagCode(DTC_Mem[IDX_OCD_P].DtcNumber);
        DTC_Curr_Huge.Status.all = eachNormal;
    }

}

void _Diag_Update_Over_SOC_WNF(void)
{
    uint32_t DT;
    static uint16_t prevStatusSummary = 0;
    uint32_t soc = SOC_CTL.SOC;
    // 1. Check whether Over discharge Current 1 detected.
    if (DTC_OVER_SOC.Status.all == eachNormal)
    {// --
        if (soc >= OVER_SOC_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "osoc w+ \n");
            if (!DTC_OVER_SOC.time.Set)
            {
                DTC_OVER_SOC.time.Set = OVER_SOC_WARNING_WTIME;
                DTC_OVER_SOC.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_SOC.time.Start);
            if (DT >= DTC_OVER_SOC.time.Set)
            {
                hLogProc(LOG_DTC, "osoc -->warning \n");
                DTC_OVER_SOC.Status.all = eachWarning;
                DTC_OVER_SOC.time.Set = 0;
            }

        }
        else
        {
            DTC_OVER_SOC.time.Set = 0;
        }
    }
    else if (DTC_OVER_SOC.Status.all == eachWarning)
    {//--
        if (soc >= OVER_SOC_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "osoc f+ \n");
            if (!DTC_OVER_SOC.time.Set)
            {
                DTC_OVER_SOC.time.Set = OVER_SOC_FAULT_WTIME;
                DTC_OVER_SOC.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_SOC.time.Start);
            if (DT >= DTC_OVER_SOC.time.Set)
            {
                hLogProc(LOG_DTC, "osoc -->fault \n");
                DTC_OVER_SOC.Status.all = eachFault;
                DTC_OVER_SOC.time.Set = 0;
            }
        }
        else if (soc < OVER_SOC_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "osoc n+ \n");
            if (!DTC_OVER_SOC.time.Set)
            {
                DTC_OVER_SOC.time.Set = OVER_SOC_WARNING_RELEASE_TIME;
                DTC_OVER_SOC.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_SOC.time.Start);
            if (DT >= DTC_OVER_SOC.time.Set)
            {
                hLogProc(LOG_DTC, "osoc n+ \n");
                DTC_OVER_SOC.Status.all = eachNormal;
                DTC_OVER_SOC.time.Set = 0;
            }

        }
        else
        {
            DTC_OVER_SOC.time.Set = 0;
        }

    }
    else if (DTC_OVER_SOC.Status.all == eachFault)
    {//--
        if (soc < OVER_SOC_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "osoc w+ \n");
            if (!DTC_OVER_SOC.time.Set)
            {
                DTC_OVER_SOC.time.Set = OVER_SOC_FAULT_RELEASE_TIME;
                DTC_OVER_SOC.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_SOC.time.Start);
            if (DT >= DTC_OVER_SOC.time.Set)
            {
                hLogProc(LOG_DTC, "osoc -->warning \n");
                DTC_OVER_SOC.Status.all = eachWarning;
                DTC_OVER_SOC.time.Set = 0;
            }
        }
        else
        {
            DTC_OVER_SOC.time.Set = 0;
        }
    }
    else
    {
    /* To avoid QAC Warnings */
    }

    if (DTC_OVER_SOC.Status.all == eachNormal)
    {
        DTC_Mem[IDX_OVER_SOC_W].Occurence = NotDeteced;

        if (DTC_Mem[IDX_OVER_SOC_F].ReleaseCond == Release)
        {
    	    DTC_Mem[IDX_OVER_SOC_F].Occurence = NotDeteced;
        }
        else
        {
        }
    }
    else if (DTC_OVER_SOC.Status.all == eachWarning)
    {
        DTC_Mem[IDX_OVER_SOC_W].Occurence = Detected;
        if (DTC_Mem[IDX_OVER_SOC_F].ReleaseCond == Release)
        {
    	    DTC_Mem[IDX_OVER_SOC_F].Occurence = NotDeteced;
        }
    }
    else if (DTC_OVER_SOC.Status.all == eachFault)
    {
        if (prevStatusSummary == eachWarning)
        {
            RackSys_PushDiagCode(DTC_Mem[IDX_OVER_SOC_F].DtcNumber);
        }
        DTC_Mem[IDX_OVER_SOC_W].Occurence = NotDeteced;
        DTC_Mem[IDX_OVER_SOC_F].Occurence = Detected;
    }

    prevStatusSummary = DTC_OVER_SOC.Status.all;

    //4. Set Gateway Data
    if (DTC_Mem[IDX_OVER_SOC_W].Occurence == Detected) {
        RackSys_StringEvent(STREV_OVSOC_WARN, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVSOC_WARN, 0);
    }
    if (DTC_Mem[IDX_OVER_SOC_F].Occurence == Detected) {
        RackSys_StringEvent(STREV_OVSOC_ALARM, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVSOC_ALARM, 0);
    }
}

#endif
__WEAK uint16_t ConvertDTCTbleToErrTble(uint16_t dtc, uint16_t subcode)
{
    switch(dtc){
        case DTC_TBMS_RX_F:
            return 0xE001;
        case DTC_MOD_OT_F:
            return 0xE002;
        case DTC_MOD_UT_F:
            return 0xE003;
        case DTC_OV_CHRG_CURR_F:
            return 0xE004;
        case DTC_OV_DSCHRG_CURR_F:
            return 0xE005;
        case DTC_BAT_OV_F:
            return 0xE006;
        case DTC_BAT_UV_F:
            return 0xE007;
        case DTC_RELAY_F:
            return 0xE008;
        case DTC_CELL_OV_F:
            return 0xE009;
        case DTC_CELL_UV_F:
            return 0xE010;
        case DTC_SM_F:
            switch(subcode){
                case SM_F_EEPROM_CHKSUM:
                    return 0xE011;
                case SM_F_MCUFAULT:
                    return 0xE012;
                case SM_F_VOLPLB_ERROR:
                    return 0xE013;
                case SM_F_SUBMCU_TIMEOUT:
                case SM_F_SUBMCU_WATCHDOG:
                    return 0xE014;
            }
            return 0xE0FF;
    }
    return 0xE000;
}

void _System_Relay_Faiure(void)
{
    static uint16_t prevStatusSummary = 0;
    if (DTC_RELAY_ERR.Status.all == eachFault)
    {
        if (prevStatusSummary == eachNormal)
        {
            RackSys_PushDiagCode(DTC_Mem[IDX_RELAY_F].DtcNumber);
        }
        DTC_Mem[IDX_RELAY_F].Occurence = Detected;
    }
    else if (DTC_RELAY_ERR.Status.all == eachNormal)
    {
        if (DTC_Mem[IDX_RELAY_F].ReleaseCond == Release)
        {
            DTC_Mem[IDX_RELAY_F].Occurence = NotDeteced;
        }

    }
    prevStatusSummary = DTC_RELAY_ERR.Status.all;

    if (DTC_Mem[IDX_RELAY_F].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_CONTACTOR_ERROR, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_CONTACTOR_ERROR, 0);
    }
}

void _Diag_Update_Bat_Over_Volt_WNF(void)
{
    uint32_t DT;
    static uint16_t prevStatusSummary = 0;
    uint32_t vol = BatSum10mVolt/10;

    //jae.kim
    //Relay가 연결되어 있지 않은 상태에서는 체크 하지 않는다.
    if (getRelayOnOffStatus() == 0)
    {
        DTC_OVER_BATVOL.Status.all = eachNormal;
        goto UpdateDTC;
    }

    if (vol > OVER_BAT_VOL_FAULT_DETECT)
    {
        if (DTC_OVER_BATVOL.Status.all != eachWarningFault && DTC_OVER_BATVOL.Status.all != eachFault)
        {
            hLogProc(LOG_DTC, "ob f+ \n");
            if (!DTC_OVER_BATVOL.time.Set)
            {
                DTC_OVER_BATVOL.time.Set = OVER_BAT_VOL_FAULT_WTIME;
                DTC_OVER_BATVOL.time.Start = hTimCurrentTime();
            }
            DTC_OVER_BATVOL.Status.all = eachWarningFault;
        }
    }
    else
    {    //warning or normal
        if (DTC_OVER_BATVOL.Status.all == eachWarningFault)
        {
            hLogProc(LOG_DTC, "ob -->warning \n");
            DTC_OVER_BATVOL.Status.all = eachWarning;
            DTC_OVER_BATVOL.time.Set = 0;
        }
    }
    // 1. Check whether Over Charge Current 1 detected.
    if (DTC_OVER_BATVOL.Status.all == eachNormal)
    {// -- Current Status is Normal
        if (vol > OVER_BAT_VOL_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "ob w+ \n");
            if (!DTC_OVER_BATVOL.time.Set){
                DTC_OVER_BATVOL.time.Set = OVER_BAT_VOL_WARNING_WTIME;
                DTC_OVER_BATVOL.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_BATVOL.time.Start);
            if (DT >= DTC_OVER_BATVOL.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "ob -->warning \n");
                DTC_OVER_BATVOL.Status.all = eachWarning;
                DTC_OVER_BATVOL.time.Set = 0;
            }
        }
        else
        {
            DTC_OVER_BATVOL.time.Set = 0;
        }
    }
    else if (DTC_OVER_BATVOL.Status.all == eachWarning)
    {//-- Current Status is Warning
    /*
        if (vol > OVER_BAT_VOL_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "ob f+ \n");
            if (!DTC_OVER_BATVOL.time.Set)
            {
                DTC_OVER_BATVOL.time.Set = OVER_BAT_VOL_FAULT_WTIME;
                DTC_OVER_BATVOL.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_BATVOL.time.Start);
            if (DT >= DTC_OVER_BATVOL.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "ob -->fault \n");
                DTC_OVER_BATVOL.Status.all = eachFault;
                DTC_OVER_BATVOL.time.Set = 0;
            }
        }
        else */
        if (vol <= OVER_BAT_VOL_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "ob n+ \n");
            if (!DTC_OVER_BATVOL.time.Set)
            {
                DTC_OVER_BATVOL.time.Set = OVER_BAT_VOL_WARNING_RELEASE_TIME;
                DTC_OVER_BATVOL.time.Start = hTimCurrentTime();
            }
        
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_BATVOL.time.Start);
            if (DT >= DTC_OVER_BATVOL.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "ob -->normal \n");
                DTC_OVER_BATVOL.Status.all = eachNormal;
                DTC_OVER_BATVOL.time.Set = 0;
            }
        }
        else
        {
            DTC_OVER_BATVOL.time.Set = 0;
        }
    }
    else if (DTC_OVER_BATVOL.Status.all == eachWarningFault)
    {//-- Current Status is Fault
        if (vol <= OVER_BAT_VOL_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "ob w+ \n");
            if (!DTC_OVER_BATVOL.time.Set)
            {
                DTC_OVER_BATVOL.time.Set = OVER_BAT_VOL_FAULT_RELEASE_TIME;
                DTC_OVER_BATVOL.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_BATVOL.time.Start);
            if (DT >= DTC_OVER_BATVOL.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "ob -->warning \n");
                DTC_OVER_BATVOL.Status.all = eachWarning;
                DTC_OVER_BATVOL.time.Set = 0;
            }
        }
        else
        {
            hLogProc(LOG_DTC, "ob f+ \n");
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_BATVOL.time.Start);
            if (DT >= DTC_OVER_BATVOL.time.Set)
            {
                hLogProc(LOG_DTC, "ob -->fault \n");
                DTC_OVER_BATVOL.Status.all = eachFault;
                DTC_OVER_BATVOL.time.Set = 0;
            }
        }
    }
    else
    {
      /* To avoid QAC Warnings */
    }
UpdateDTC:
    //3. Update DTC
    switch (DTC_OVER_BATVOL.Status.all)
    {
        case eachNormal:
            if ( DTC_Mem[IDC_BAT_OV_W].ReleaseCond == Release)
            {
                DTC_Mem[IDC_BAT_OV_W].Occurence = 0;
            }
            if (DTC_Mem[IDC_BAT_OV_F].ReleaseCond == Release)
            {
                DTC_Mem[IDC_BAT_OV_F].Occurence = 0;
            }
            break;
        case eachWarning:
            DTC_Mem[IDC_BAT_OV_W].Occurence = Detected;
            if (DTC_Mem[IDC_BAT_OV_F].ReleaseCond == Release)
            {
                DTC_Mem[IDC_BAT_OV_F].Occurence = 0;
            }
            break;
        case eachFault:
            DTC_Mem[IDC_BAT_OV_F].Occurence = Detected;
            if (DTC_Mem[IDC_BAT_OV_W].ReleaseCond == Release)
            {
                DTC_Mem[IDC_BAT_OV_W].Occurence = 0;
            }
        //Relay Off
        if (prevStatusSummary == eachWarningFault)
        {
            hLogProc(LOG_DTC, "push ov err code\n");
            RackSys_PushDiagCode(DTC_Mem[IDC_BAT_OV_F].DtcNumber);
        }
        if (DTC_Mem[IDC_BAT_OV_F].Action == RelayOpen)
        {
            hLogProc(LOG_DTC, "relay open\n");
            processRelayOffFromDTC();
            if (!EEPROMData.evlog.dtc)
            {
                EEprom_WriteBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&DTC_Mem[IDC_BAT_OV_F].DtcNumber, 2);
                EEprom_ReadBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&EEPROMData.evlog, 2);
            }

        }
        break;
    }
    prevStatusSummary = DTC_OVER_BATVOL.Status.all;

    if (DTC_Mem[IDC_BAT_OV_W].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_OVVOL_WARN, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVVOL_WARN, 0);
    }
    if (DTC_Mem[IDC_BAT_OV_F].Occurence == Detected)
    {
        hLogProc(LOG_DTC, "Cur OV Fault on\n");
        RackSys_StringEvent(STREV_OVVOL_ALARM, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVVOL_ALARM, 0);
    }
}


void _Diag_Update_Bat_Under_Volt_WNF(void)
{
    uint32_t DT;
    static uint16_t prevStatusSummary = 0;
    uint32_t vol = BatSum10mVolt / 10;
    static int relay_cnt = 0;
    //jae.kim
    //Relay가 연결되어 있지 않은 상태에서는 체크 하지 않는다.
    if (getRelayOnOffStatus() == 0)
    {
        DTC_UNDER_BATVOL.Status.all = eachNormal;
        relay_cnt = 0;
        goto UpdateDTC;
    }
    //jae.kim
    // Pack 전압 읽는 순간에는 relay가 open 이다가 err check시 relay가 붙는 경우 방지
    if (++relay_cnt < 2)
    {
        return;
    }
    else
    {
        relay_cnt = 2;
    }

    if (vol < UNDER_BAT_VOL_FAULT_DETECT)
    {
        if (DTC_UNDER_BATVOL.Status.all != eachWarningFault && DTC_UNDER_BATVOL.Status.all != eachFault)
        {
            hLogProc(LOG_DTC, "ub f+ \n");
            if (!DTC_UNDER_BATVOL.time.Set)
            {
                DTC_UNDER_BATVOL.time.Set = UNDER_BAT_VOL_FAULT_WTIME;
                DTC_UNDER_BATVOL.time.Start = hTimCurrentTime();
            }
            DTC_UNDER_BATVOL.Status.all = eachWarningFault;
        }
    }
    else
    { //warning or normal
        if (DTC_UNDER_BATVOL.Status.all == eachWarningFault)
        {
            hLogProc(LOG_DTC, "ub -->warning \n");
            DTC_UNDER_BATVOL.Status.all = eachWarning;
            DTC_UNDER_BATVOL.time.Set = 0;
        }
    }

    // 1. Check whether Over Charge Current 1 detected.
    if (DTC_UNDER_BATVOL.Status.all == eachNormal)
    {// -- Current Status is Normal
        if (vol < UNDER_BAT_VOL_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "ub w+ \n");
            if (!DTC_UNDER_BATVOL.time.Set)
            {
                DTC_UNDER_BATVOL.time.Set = UNDER_BAT_VOL_WARNING_WTIME;
                DTC_UNDER_BATVOL.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_UNDER_BATVOL.time.Start);
            if (DT >= DTC_UNDER_BATVOL.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "ub -->warning \n");
                DTC_UNDER_BATVOL.Status.all = eachWarning;
                DTC_UNDER_BATVOL.time.Set = 0;
            }
        }
        else
        {
            DTC_UNDER_BATVOL.time.Set = 0;
        }
    }
    else if (DTC_UNDER_BATVOL.Status.all == eachWarning)
    {//-- Current Status is Warning
    /*
        if (vol < UNDER_BAT_VOL_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "ub f+ \n");
            if (!DTC_UNDER_BATVOL.time.Set)
            {
                DTC_UNDER_BATVOL.time.Set = UNDER_BAT_VOL_FAULT_WTIME;
                DTC_UNDER_BATVOL.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_UNDER_BATVOL.time.Start);
            if (DT >= DTC_UNDER_BATVOL.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "ub -->fault \n");
                DTC_UNDER_BATVOL.Status.all = eachFault;
                DTC_UNDER_BATVOL.time.Set = 0;
            }
      }else */
        if (vol >= UNDER_BAT_VOL_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "ub n+ \n");
            if (!DTC_UNDER_BATVOL.time.Set)
            {
                DTC_UNDER_BATVOL.time.Set = UNDER_BAT_VOL_WARNING_RELEASE_TIME;
                DTC_UNDER_BATVOL.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_UNDER_BATVOL.time.Start);
            if (DT >= DTC_UNDER_BATVOL.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "ub -->normal \n");
                DTC_UNDER_BATVOL.Status.all = eachNormal;
                DTC_UNDER_BATVOL.time.Set = 0;
            }
        }
        else
        {
            DTC_UNDER_BATVOL.time.Set = 0;
        }
    }
    else if (DTC_UNDER_BATVOL.Status.all == eachWarningFault)
    {//-- Current Status is Fault
        if (vol >= UNDER_BAT_VOL_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "ub w+ \n");
            if (!DTC_UNDER_BATVOL.time.Set)
            {
                DTC_UNDER_BATVOL.time.Set = UNDER_BAT_VOL_FAULT_RELEASE_TIME;
                DTC_UNDER_BATVOL.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_UNDER_BATVOL.time.Start);
            if (DT >= DTC_UNDER_BATVOL.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "ub -->warning \n");
                DTC_UNDER_BATVOL.Status.all = eachWarning;
                DTC_UNDER_BATVOL.time.Set = 0;
            }
        }
        else
        {
            hLogProc(LOG_DTC, "ub f+ \n");
            DT = (uint32_t)(hTimCurrentTime() - DTC_UNDER_BATVOL.time.Start);
            if (DT >= DTC_UNDER_BATVOL.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "ub -->fault \n");
                DTC_UNDER_BATVOL.Status.all = eachFault;
                DTC_UNDER_BATVOL.time.Set = 0;
            }
        }
    }
    else
    {
      /* To avoid QAC Warnings */
    }
UpdateDTC:
    //3. Update DTC
    switch (DTC_UNDER_BATVOL.Status.all)
    {
        case eachNormal:
            if (DTC_Mem[IDC_BAT_UV_W].ReleaseCond == Release)
            {
                DTC_Mem[IDC_BAT_UV_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDC_BAT_UV_F].ReleaseCond == Release)
            {
                DTC_Mem[IDC_BAT_UV_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDC_BAT_UV_W].Occurence = Detected;
            if (DTC_Mem[IDC_BAT_UV_F].ReleaseCond == Release)
            {
                DTC_Mem[IDC_BAT_UV_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            if (prevStatusSummary == eachWarningFault)
            {
                hLogProc(LOG_DTC, "push uv err code\n");
                RackSys_PushDiagCode(DTC_Mem[IDC_BAT_UV_F].DtcNumber);
            }
            if (DTC_Mem[IDC_BAT_UV_W].ReleaseCond == Release)
            {
                DTC_Mem[IDC_BAT_UV_W].Occurence = NotDeteced;
            }
            DTC_Mem[IDC_BAT_UV_F].Occurence = Detected;
            if (DTC_Mem[IDC_BAT_UV_F].Action == RelayOpen)
            {
                hLogProc(LOG_DTC, "relay open\n");
                processRelayOffFromDTC();
                if (!EEPROMData.evlog.dtc)
                {
                    EEprom_WriteBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&DTC_Mem[IDC_BAT_UV_F].DtcNumber, 2);
                    EEprom_ReadBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&EEPROMData.evlog, 2);
                }
            }
            break;
    }

    prevStatusSummary = DTC_UNDER_BATVOL.Status.all;
    if (DTC_Mem[IDC_BAT_UV_W].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_UNVOL_WARN, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_UNVOL_WARN, 0);
    }
    if (DTC_Mem[IDC_BAT_UV_F].Occurence == Detected)
    {
        hLogProc(LOG_DTC, "Cur UV Fault on\n");

        RackSys_StringEvent(STREV_UNVOL_ALARM, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_UNVOL_ALARM, 0);
    }
}

void _Diag_Update_Over_Cell_Volt_WNF(void)
{
    uint16_t mBmsIdx= 0;
    uint16_t cellIdx= 0;
    uint16_t StatusSummary= 0;
    static uint16_t prevStatusSummary= 0;
    uint32_t WarningMbmsIdx=0xFF;
    uint32_t FaultMbmsIdx=0xFF;
    uint32_t DT;


  //-- 1. Whether Check All Cell Voltage
    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS; mBmsIdx++)
    {
        for (cellIdx = 0; cellIdx < TRAY_CELL_NUMS; cellIdx++)
        {
            //protect 구간은 Normal, Warning,Fault State 와 무관하게 체크한다.
            if (RbmsComTray[mBmsIdx].cellvol[cellIdx] > OVER_CELL_V_FAULT_DETECT)
            {
                if (DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all != eachWarningFault && DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all != eachFault)
                {
                    hLogProc(LOG_DTC, "ov f++\n");
                    if (!DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set)
                    {
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set= OVER_CELL_V_FAULT_WTIME;
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Start = hTimCurrentTime();
                    }
                    DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all = eachWarningFault;
                }
            }
            else  //warning or normal
            {
                if (DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all == eachWarningFault)
                {
                    hLogProc(LOG_DTC, "ov -->warning\n");
                    DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all = eachWarning;
                    DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = 0;
                }
            }

            if (DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all == eachNormal)
            {//-- Current Status is Normal
                if (RbmsComTray[mBmsIdx].cellvol[cellIdx] > OVER_CELL_V_WARNING_DETECT)
                {
                    hLogProc(LOG_DTC, "ov w+ \n");
                    if (!DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set)
                    {
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = OVER_CELL_V_WARNING_WTIME;
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Start = hTimCurrentTime();
                    }
                    DT =  (uint32_t)(hTimCurrentTime() - DTC_EachCell_OV[mBmsIdx][cellIdx].time.Start);
                    if (DT >= DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set)
                    {//-- Go to Warning Status
                        hLogProc(LOG_DTC, "ov -->warn\n");
                        DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all = eachWarning;
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = 0;
                    }
                }
                else
                {
                    DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = 0;
                }
            }
            else if (DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all == eachWarning)
            {//-- Current Status is Warning
                /*
                if (RbmsComTray[mBmsIdx].cellvol[cellIdx] > OVER_CELL_V_FAULT_DETECT)
                {
                    hLogProc(LOG_DTC, "ov f+ \n");
                    if (!DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set)
                    {
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set= OVER_CELL_V_FAULT_WTIME;
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Start = hTimCurrentTime();
                    }
                    DT = (uint32_t)(hTimCurrentTime() - DTC_EachCell_OV[mBmsIdx][cellIdx].time.Start);
                    if (DT >=  DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set)
                    {  //-- Go to Fault Status
                        hLogProc(LOG_DTC, "ov --> fault\n");
                        DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all = eachFault;
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = 0;
                    }
                }
                else
                {
                //-- Keep the Warning Status
                }
                }
                else
                */
                if (RbmsComTray[mBmsIdx].cellvol[cellIdx] <= OVER_CELL_V_WARNING_DETECT)
                {
                    hLogProc(LOG_DTC, "ov n++ \n");
                    if (!DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set)
                    {
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = OVER_CELL_V_WARNING_RELEASE_TIME;
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Start = hTimCurrentTime();
                    }
                    DT = (uint32_t)(hTimCurrentTime() - DTC_EachCell_OV[mBmsIdx][cellIdx].time.Start);
                    if (DT >= DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set)
                    {
                        hLogProc(LOG_DTC, "ov -->normal\n");
                        DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all = eachNormal;
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = 0;
                    }
                    else
                    {
                        //--Keep the Warning Status
                    }
                }
                else
                {
                    DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = 0;
                }
            }
            else if (DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all == eachWarningFault || DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all == eachFault)
            {//-- Current Status is Fault
                if (RbmsComTray[mBmsIdx].cellvol[cellIdx] <= OVER_CELL_V_FAULT_DETECT)
                {
                    hLogProc(LOG_DTC, "ov w++ \n");
                    if (!DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set)
                    {
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = OVER_CELL_V_FAULT_RELEASE_TIME;
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Start = hTimCurrentTime();
                    }
                    DT = (uint32_t)(hTimCurrentTime() - DTC_EachCell_OV[mBmsIdx][cellIdx].time.Start);
                    if (DT >= DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set)
                    {
                        hLogProc(LOG_DTC, "ov -->warn\n");
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = 0;
                        DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all = eachWarning;
                    }
                }
                else
                {
                    hLogProc(LOG_DTC, "ob f+ \n");
                    DT = (uint32_t)(hTimCurrentTime() - DTC_EachCell_OV[mBmsIdx][cellIdx].time.Start);
                    if (DT >=  DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set)
                    {  //-- Go to Fault Status
                        hLogProc(LOG_DTC, "ov -->fault\n");
                        DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all = eachFault;
                        DTC_EachCell_OV[mBmsIdx][cellIdx].time.Set = 0;
                    }
                }
            }
        }
    }


    WarningMbmsIdx = 0;

  //-- find warning
    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS; mBmsIdx++)
    {
        for (cellIdx = 0; cellIdx < TRAY_CELL_NUMS; cellIdx++)
        {
            if (DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all == eachWarning)
            {
                StatusSummary = eachWarning;
                WarningMbmsIdx |= (1<< mBmsIdx);
                FaultMbmsIdx = 0;
                break;
            }
        }
    }
    FaultMbmsIdx = 0;

  // -- find fault
    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS; mBmsIdx++)
    {
        for (cellIdx = 0; cellIdx < TRAY_CELL_NUMS; cellIdx++)
        {
            if (DTC_EachCell_OV[mBmsIdx][cellIdx].Status.all == eachFault)
            {
                StatusSummary = eachFault;
                FaultMbmsIdx |= (1<<mBmsIdx);
                break;
            }
        }
    }



  // protect, fault, warning이 동시에 발생 하였다면, protect > fault > warning 순으로 알린다.
  // 상위 alarm이 없어야 다음 알림을 할수 있다.

  //3. Update DTC
    switch (StatusSummary)
    {
        case eachNormal:
            if (DTC_Mem[IDX_CELL_OV_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_OV_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_CELL_OV_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_OV_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDX_CELL_OV_W].Occurence = Detected;
            if (DTC_Mem[IDX_CELL_OV_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_OV_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            if (prevStatusSummary == eachWarningFault)
            {
                hLogProc(LOG_DTC, "push ov err code\n");
                RackSys_PushDiagCode(DTC_Mem[IDX_CELL_OV_F].DtcNumber);
            }
            DTC_Mem[IDX_CELL_OV_F].Occurence = Detected;
            if (DTC_Mem[IDX_CELL_OV_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_OV_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_CELL_OV_F].Action == RelayOpen)
            {
                hLogProc(LOG_DTC, "relay open\n");
                processRelayOffFromDTC();
                if (!EEPROMData.evlog.dtc)
                {
                    EEprom_WriteBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&DTC_Mem[IDX_CELL_OV_F].DtcNumber, 2);
                    EEprom_ReadBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&EEPROMData.evlog, 2);
                }
            }
            break;
    }
    prevStatusSummary = StatusSummary;
    //4. Set Gateway Data
    if (DTC_Mem[IDX_CELL_OV_W].Occurence == Detected)
    {
        RackSys_StringExtEvent(STREXTEV_OVCVOL_WARN, 1);
    }
    else
    {
        RackSys_StringExtEvent(STREXTEV_OVCVOL_WARN, 0);
    }
    if (DTC_Mem[IDX_CELL_OV_F].Occurence == Detected)
    {
        hLogProc(LOG_DTC, "Cur OCV Fault on\n");
        RackSys_StringExtEvent(STREXTEV_OVCVOL_ALARM, 1);
    }
    else
    {
        RackSys_StringExtEvent(STREXTEV_OVCVOL_ALARM, 0);
    }
}//--End of Function


void _Diag_Update_Under_Cell_Volt_WNF(void)
{
    uint16_t mBmsIdx = 0;
    uint16_t cellIdx = 0;
    uint16_t StatusSummary = 0;
    static uint16_t prevStatusSummary = 0;
    uint32_t DT;
    static uint8_t cell_cnt = 0;

    if (cell_cnt < 10)
    {   // moving average 에 의해서 cell sampling이 시간이 걸림
        cell_cnt++;
        return;
    }
    else
    {
    //traybms 통신 에러상태 고려 필요.
    }

  //-- 1. Whether Check All Cell Voltage
    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS; mBmsIdx++)
    {
        for (cellIdx = 0; cellIdx < TRAY_CELL_NUMS; cellIdx++)
        {
            if (RbmsComTray[mBmsIdx].cellvol[cellIdx] < UNDER_CELL_V_FAULT_DETECT)
            {
                if (DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all != eachWarningFault && DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all != eachFault)
                {
                    hLogProc(LOG_DTC, "uv f+\n");
                    if (!DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set)
                    {
                        DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = UNDER_CELL_V_PROTECT_WTIME;
                        DTC_EachCell_UV[mBmsIdx][cellIdx].time.Start = hTimCurrentTime();
                    }
                    DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all = eachWarningFault;
                }
            }
            else    //Warning or normal
            {
                if (DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all == eachWarningFault)
                {
                    hLogProc(LOG_DTC, "uv -->warning\n");
                    DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all = eachWarning;
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = 0;
                }
            }

        if (DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all == eachNormal)
        {//-- Current Status is Normal
            if (RbmsComTray[mBmsIdx].cellvol[cellIdx] < UNDER_CELL_V_WARNING_DETECT)
            {
                hLogProc(LOG_DTC, "uv w+\n");
                if (!DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set)
                {
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = UNDER_CELL_V_WARNING_WTIME;
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Start = hTimCurrentTime();
                }
                DT = hTimCurrentTime() - DTC_EachCell_UV[mBmsIdx][cellIdx].time.Start;
                if (DT >= DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set)
                {//-- Go to Warning Status
                    hLogProc(LOG_DTC, "uv -->warning\n");
                    DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all = eachWarning;
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = 0;
                }
            }
            else
            {
                DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = 0;
            }
        }
        else if (DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all == eachWarning)
        {//-- Current Status is Warning
        /*
            if (RbmsComTray[mBmsIdx].cellvol[cellIdx] < UNDER_CELL_V_FAULT_DETECT)
            {
                hLogProc(LOG_DTC, "uv f+\n");
                if (!DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set)
                {
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = UNDER_CELL_V_FAULT_WTIME;
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Start = hTimCurrentTime();
                }

                DT = hTimCurrentTime() - DTC_EachCell_UV[mBmsIdx][cellIdx].time.Start;
                if (DT >= DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set)
                {  //-- Go to Fault Status
                    hLogProc(LOG_DTC, "uv -->fault\n");
                    DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all = eachFault;
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = 0;
                }
                else
                {
                //-- Keep the Warning Status
                }
            }
            else */
            if (RbmsComTray[mBmsIdx].cellvol[cellIdx] >= UNDER_CELL_V_WARNING_DETECT)
            {
                hLogProc(LOG_DTC, "uv n+\n");
                if (!DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set)
                {
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = UNDER_CELL_V_WARNING_RELEASE_TIME;
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Start = hTimCurrentTime();
                }

                DT = hTimCurrentTime() - DTC_EachCell_UV[mBmsIdx][cellIdx].time.Start;
                if (DT >= DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set)
                {
                    hLogProc(LOG_DTC, "uv -->normal\n");
                    DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all = eachNormal;
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = 0;
                }
            }
            else
            {
                DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = 0;
                //--Keep the Warning Status
            }
        }
        else if (DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all == eachFault || DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all == eachWarningFault)
        {//-- Current Status is Fault
            if (RbmsComTray[mBmsIdx].cellvol[cellIdx] >=  UNDER_CELL_V_FAULT_DETECT)
            {
                hLogProc(LOG_DTC, "uv w+\n");
                if (!DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set)
                {
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = UNDER_CELL_V_FAULT_RELEASE_TIME;
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Start = hTimCurrentTime();
                }

                DT = hTimCurrentTime() - DTC_EachCell_UV[mBmsIdx][cellIdx].time.Start;
                if (DT >= DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set)
                {
                    hLogProc(LOG_DTC, "uv -->warning\n");
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = 0;
                    DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all = eachWarning;
                }
            }
            else
            {
                hLogProc(LOG_DTC, "uv f+\n");
                DT = hTimCurrentTime() - DTC_EachCell_UV[mBmsIdx][cellIdx].time.Start;
                if (DT >= DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set)
                {
                    hLogProc(LOG_DTC, "uv -->fault\n");
                    DTC_EachCell_UV[mBmsIdx][cellIdx].time.Set = 0;
                    DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all = eachFault;
                }
            }
        }
        }
    }

    //-- find warning
    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS; mBmsIdx++)
    {
        for (cellIdx = 0; cellIdx < TRAY_CELL_NUMS; cellIdx++)
        {
        if (DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all == eachWarning)
        {
            StatusSummary = eachWarning;
            break;
        }
        }
    }
    // -- find fault
    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS; mBmsIdx++)
    {
        for (cellIdx = 0; cellIdx < TRAY_CELL_NUMS; cellIdx++)
        {
            if (DTC_EachCell_UV[mBmsIdx][cellIdx].Status.all == eachFault)
            {
                StatusSummary = eachFault;
                break;
            }
        }
    }


  //3. Update DTC
    switch (StatusSummary)
    {
        case eachNormal:
            if (DTC_Mem[IDX_CELL_UV_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_UV_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_CELL_UV_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_UV_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDX_CELL_UV_W].Occurence = Detected;
            if (DTC_Mem[IDX_CELL_UV_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_UV_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            if (prevStatusSummary == eachWarningFault)
            {
                hLogProc(LOG_DTC, "push uv err code\n");
                RackSys_PushDiagCode(DTC_Mem[IDX_CELL_UV_F].DtcNumber);
            }
            DTC_Mem[IDX_CELL_UV_F].Occurence = Detected;
            if (DTC_Mem[IDX_CELL_UV_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_UV_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_CELL_UV_F].Action == RelayOpen)
            {
                hLogProc(LOG_DTC, "relay open\n");
                processRelayOffFromDTC();
                if (!EEPROMData.evlog.dtc)
                {
                    EEprom_WriteBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&DTC_Mem[IDX_CELL_UV_F].DtcNumber, 2);
                    EEprom_ReadBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&EEPROMData.evlog, 2);
                }
            }
            break;
    }
    prevStatusSummary = StatusSummary;

    //4. Set Gateway Data
    if (DTC_Mem[IDX_CELL_UV_W].Occurence == Detected)
    {
        RackSys_StringExtEvent(STREXTEV_UNCVOL_WARN, 1);
    }
    else
    {
        RackSys_StringExtEvent(STREXTEV_UNCVOL_WARN, 0);
    }
    if (DTC_Mem[IDX_CELL_UV_F].Occurence == Detected)
    {
        hLogProc(LOG_DTC, "Cur UCV Fault on\n");
        RackSys_StringExtEvent(STREXTEV_UNCVOL_ALARM, 1);
    }
    else
    {
        RackSys_StringExtEvent(STREXTEV_UNCVOL_ALARM, 0);
    }
}


void _Diag_Update_Over_Mod_Temp_WNF(void)
{
    uint16_t mBmsIdx = 0;
    uint16_t temperatureIdx = 0;
    uint16_t StatusSummary = 0;
    static uint16_t prevStatusSummary = 0;
    int16_t *temperature;
    STR_ErrorModel* TmpOT;
    uint32_t DT;
    //-- 1. Whether Check All Temperature
    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS; mBmsIdx++)
    {
        for (temperatureIdx = 0; temperatureIdx < MODULE_TEMP_SENSORNUM; temperatureIdx++)
        {
            temperature = &RbmsComTray[mBmsIdx].tmp[temperatureIdx];
            TmpOT = &DTC_EachTemp_OT[mBmsIdx][temperatureIdx];

            if (*temperature > OVER_CELL_TEMP_FAULT_DETECT)
            {
                if (TmpOT->Status.all != eachWarningFault && TmpOT->Status.all != eachFault)
                {
                    hLogProc(LOG_DTC, "ot f+ \n");
                    if (!TmpOT->time.Set)
                    {
                        TmpOT->time.Set = OVER_CELL_TEMP_FAULT_WTIME;
                        TmpOT->time.Start = hTimCurrentTime();
                    }
                    TmpOT->Status.all = eachWarningFault;
                }
            }
            else   //Warning or Normal
            {
                if (TmpOT->Status.all == eachWarningFault)
                {
                    hLogProc(LOG_DTC, "ot w+ \n");
                    TmpOT->Status.all = eachWarning;
                    TmpOT->time.Set = 0;
                }
            }
            if (TmpOT->Status.all == eachNormal)
            {//-- Current Status is Normal
                if (*temperature > OVER_CELL_TEMP_WARNING_DETECT)
                {
                    hLogProc(LOG_DTC, "ot w+ \n");
                    if (!TmpOT->time.Set)
                    {
                        TmpOT->time.Set = OVER_CELL_TEMP_WARNING_WTIME;
                        TmpOT->time.Start = hTimCurrentTime();
                    }
                    DT = hTimCurrentTime() - TmpOT->time.Start;
                    if (DT >= TmpOT->time.Set)
                    {//-- Go to Warning Status
                        hLogProc(LOG_DTC, "ot -->warning \n");
                        TmpOT->Status.all = eachWarning;
                        TmpOT->time.Set = 0;
                    }
                }
                else
                {
                    TmpOT->time.Set = 0;
                }
            }
            else if (TmpOT->Status.all == eachWarning)
            {//-- Current Status is Warning
                /*
                if (*temperature > OVER_CELL_TEMP_FAULT_DETECT)
                {
                    hLogProc(LOG_DTC, "ot f+ \n");
                    if (!TmpOT->time.Set)
                    {
                        TmpOT->time.Set = OVER_CELL_TEMP_FAULT_WTIME;
                        TmpOT->time.Start = hTimCurrentTime();
                    }
                    DT = hTimCurrentTime() - TmpOT->time.Start;
                    if (DT >= TmpOT->time.Set)
                    {//-- Go to Fault Status
                        hLogProc(LOG_DTC, "ot -->fault \n");
                        TmpOT->Status.all = eachFault;
                        TmpOT->time.Set = 0;

                    }
                }else */
                if (*temperature <= OVER_CELL_TEMP_WARNING_DETECT)
                {
                    hLogProc(LOG_DTC, "ot n+ \n");
                    if (!TmpOT->time.Set)
                    {
                        TmpOT->time.Set = OVER_CELL_TEMP_WARNING_RELEASE_TIME;
                        TmpOT->time.Start = hTimCurrentTime();
                    }
                    DT = hTimCurrentTime() - TmpOT->time.Start;
                    if (DT >= TmpOT->time.Set)
                    {//-- Go to Warning Status
                        hLogProc(LOG_DTC, "ot -->normal \n");
                        TmpOT->Status.all = eachNormal;
                        TmpOT->time.Set = 0;
                    }
                }
                else
                {
                        TmpOT->time.Set = 0;
                }

            }
            else if (TmpOT->Status.all == eachFault || TmpOT->Status.all == eachWarningFault)
            {//-- Current Status is Fault
                if (*temperature <= OVER_CELL_TEMP_FAULT_DETECT)
                {
                    hLogProc(LOG_DTC, "ot w+ \n");
                    if (!TmpOT->time.Set)
                    {
                        TmpOT->time.Set = OVER_CELL_TEMP_WARNING_WTIME;
                        TmpOT->time.Start = hTimCurrentTime();
                    }
                    DT = hTimCurrentTime() - TmpOT->time.Start;
                    if (DT >= TmpOT->time.Set)
                    {//-- Go to Warning Status
                        hLogProc(LOG_DTC, "ot -->warning\n");
                        TmpOT->Status.all = eachWarning;
                        TmpOT->time.Set = 0;
                    }
                }
                else
                {
                    DT = hTimCurrentTime() - TmpOT->time.Start;
                    if (DT >= TmpOT->time.Set)
                    {//-- Go to Fault Status
                        hLogProc(LOG_DTC, "ot -->fault \n");
                        TmpOT->Status.all = eachFault;
                        TmpOT->time.Set = 0;
                    }
                }
            }
        }
    }

   // WarningMbmsIdx = 0;

    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS;  mBmsIdx++)
    {
        for (temperatureIdx = 0; temperatureIdx < MODULE_TEMP_SENSORNUM; temperatureIdx++)
        {
            if (DTC_EachTemp_OT[mBmsIdx][temperatureIdx].Status.all == eachWarning)
            {
                StatusSummary =  eachWarning;
            // WarningMbmsIdx = (1<<mBmsIdx);
                break;
            }
        }
    }
   // FaultMbmsIdx = 0;

    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS;  mBmsIdx++)
    {
        for (temperatureIdx = 0; temperatureIdx < MODULE_TEMP_SENSORNUM; temperatureIdx++)
        {
            if (DTC_EachTemp_OT[mBmsIdx][temperatureIdx].Status.all == eachFault)
            {
                StatusSummary = eachFault;
            // FaultMbmsIdx  = (1<<mBmsIdx);
                break;
            }
        }
    }

  //3. Update DTC
    switch (StatusSummary)
    {
        case eachNormal:
            if (DTC_Mem[IDX_CELL_OT_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_OT_F].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_CELL_OT_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_OT_W].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDX_CELL_OT_W].Occurence = Detected;
            if (DTC_Mem[IDX_CELL_OT_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_OT_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            DTC_Mem[IDX_CELL_OT_F].Occurence = Detected;
            if (DTC_Mem[IDX_CELL_OT_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_OT_W].Occurence = NotDeteced;
            }
            if (prevStatusSummary == eachWarningFault)
            {
                hLogProc(LOG_DTC, "push ot err code\n");
                RackSys_PushDiagCode(DTC_Mem[IDX_CELL_OT_F].DtcNumber);
            }
            if (DTC_Mem[IDX_CELL_OT_F].Action == RelayOpen)
            {
                hLogProc(LOG_DTC, "relay open\n");
                processRelayOffFromDTC();
                if (!EEPROMData.evlog.dtc)
                {
                    EEprom_WriteBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&DTC_Mem[IDX_CELL_OT_F].DtcNumber, 2);
                    EEprom_ReadBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&EEPROMData.evlog, 2);
                }
            }
            break;
    }
    prevStatusSummary = StatusSummary;

  //4. Set Gateway Data
    if (DTC_Mem[IDX_CELL_OT_W].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_OVTEMP_WARN, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVTEMP_WARN, 0);
    }
    if (DTC_Mem[IDX_CELL_OT_F].Occurence == Detected)
    {
        hLogProc(LOG_DTC, "Cur OT Fault on\n");
        RackSys_StringEvent(STREV_OVTEMP_ALARM, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVTEMP_ALARM, 0);
    }
}


void _Diag_Update_Under_Mod_Temp_WNF(void)
{
    uint16_t mBmsIdx = 0;
    uint16_t temperatureIdx = 0;
    uint16_t StatusSummary = 0;
    static uint16_t prevStatusSummary = 0;
    int16_t *temperature;
    STR_ErrorModel* TmpUT;
    uint32_t DT;
    //-- 1. Whether Check All Temperature
    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS; mBmsIdx++)
    {
        for (temperatureIdx = 0; temperatureIdx < MODULE_TEMP_SENSORNUM; temperatureIdx++)
        {
            temperature = &RbmsComTray[mBmsIdx].tmp[temperatureIdx];
            TmpUT = &DTC_EachTemp_UT[mBmsIdx][temperatureIdx];

            if (*temperature < UNDER_CELL_TEMP_WARNING_DETECT)
            {
                if (TmpUT->Status.all != eachWarningFault && TmpUT->Status.all != eachFault)
                {
                    hLogProc(LOG_DTC, "ut f+ \n");
                    if (TmpUT->time.Set)
                    {
                        TmpUT->time.Set = UNDER_CELL_TEMP_FAULT_WTIME;
                        TmpUT->time.Start = hTimCurrentTime();
                    }
                    TmpUT->Status.all=eachWarningFault;
                }
            }
            else   //Warning or Normal
            {
                if (TmpUT->Status.all == eachWarningFault)
                {
                    hLogProc(LOG_DTC, "ut w+ \n");
                    TmpUT->Status.all = eachWarning;
                    TmpUT->time.Set = 0;
                }
            }

            if (TmpUT->Status.all == eachNormal)
            {//-- Current Status is Normal
                if (*temperature < UNDER_CELL_TEMP_WARNING_DETECT)
                {
                    hLogProc(LOG_DTC, "ut w+ \n");
                    if (!TmpUT->time.Set)
                    {
                        TmpUT->time.Set= UNDER_CELL_TEMP_WARNING_WTIME;
                        TmpUT->time.Start = hTimCurrentTime();
                    }
                    DT = hTimCurrentTime() - TmpUT->time.Start;
                    if (DT >= TmpUT->time.Set)
                    {//-- Go to Warning Status
                        hLogProc(LOG_DTC, "ut -->warning \n");
                        TmpUT->Status.all = eachWarning;
                        TmpUT->time.Set = 0;
                    }
                }
                else
                {
                    TmpUT->time.Set = 0;
                }
            }
            else if (TmpUT->Status.all == eachWarning)
            {//-- Current Status is Warning
                /* if (*temperature < UNDER_CELL_TEMP_FAULT_DETECT)
                {
                    hLogProc(LOG_DTC, "ut f+ \n");
                    if (!TmpUT->time.Set)
                    {
                        TmpUT->time.Set= UNDER_CELL_TEMP_FAULT_WTIME;
                        TmpUT->time.Start = hTimCurrentTime();
                    }
                    TmpUT->Status.all=eachWarningFault;

                    DT = hTimCurrentTime() - TmpUT->time.Start;
                    if (DT >= TmpUT->time.Set)
                    {//-- Go to Fault Status
                        hLogProc(LOG_DTC, "ut -->fault \n");
                        TmpUT->Status.all = eachFault;
                        TmpUT->time.Set = 0;
                    }
                }
                else */
                if (*temperature >= UNDER_CELL_TEMP_WARNING_DETECT)
                {
                    hLogProc(LOG_DTC, "ut n+ \n");
                    if (!TmpUT->time.Set)
                    {
                        TmpUT->time.Set= UNDER_CELL_TEMP_WARNING_RELEASE_TIME;
                        TmpUT->time.Start = hTimCurrentTime();
                    }
                    DT = hTimCurrentTime() - TmpUT->time.Start;
                    if (DT >= TmpUT->time.Set)
                    {//-- Go to Warning Status
                        hLogProc(LOG_DTC, "ot -->normal \n");
                        TmpUT->Status.all = eachNormal;
                        TmpUT->time.Set = 0;
                    }
                }
                else
                {
                    TmpUT->time.Set = 0;
                }
            }
            else if (TmpUT->Status.all == eachFault || TmpUT->Status.all == eachWarningFault)
            {//-- Current Status is Fault
                if (*temperature < UNDER_CELL_TEMP_FAULT_DETECT)
                {
                    DT = hTimCurrentTime() - TmpUT->time.Start;
                    if (DT >= TmpUT->time.Set)
                    {
                        hLogProc(LOG_DTC, "ut -->fault \n");
                        TmpUT->Status.all = eachFault;
                        TmpUT->time.Set = 0;
                    }

                }
                else
                {
                    hLogProc(LOG_DTC, "ut w+ \n");
                    if (!TmpUT->time.Set)
                    {
                        TmpUT->time.Set= UNDER_CELL_TEMP_WARNING_RELEASE_TIME;
                        TmpUT->time.Start = hTimCurrentTime();
                    }
                    DT = hTimCurrentTime() - TmpUT->time.Start;
                    if (DT >= TmpUT->time.Set)
                    {//-- Go to Warning Status
                        hLogProc(LOG_DTC, "ut -->warning \n");
                        TmpUT->Status.all = eachWarning;
                        TmpUT->time.Set = 0;
                    }
                }
            }
        }
    }

    for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS;  mBmsIdx++)
    {
        for (temperatureIdx = 0; temperatureIdx < MODULE_TEMP_SENSORNUM; temperatureIdx++)
        {
            if (DTC_EachTemp_UT[mBmsIdx][temperatureIdx].Status.all == eachWarning)
            {
                StatusSummary = eachWarning;
                break;
            }
        }
        }

        for (mBmsIdx = 0; mBmsIdx < TRAY_NUMS;  mBmsIdx++)
        {
        for (temperatureIdx = 0; temperatureIdx < MODULE_TEMP_SENSORNUM; temperatureIdx++)
        {
            if (DTC_EachTemp_UT[mBmsIdx][temperatureIdx].Status.all == eachFault)
            {
                StatusSummary = eachFault;
                break;
            }
        }
    }


    //3. Update DTC
    switch (StatusSummary)
    {
        case eachNormal:
            if (DTC_Mem[IDX_CELL_UT_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_UT_F].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_CELL_UT_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_UT_W].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDX_CELL_UT_W].Occurence = Detected;
            if (DTC_Mem[IDX_CELL_UT_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_UT_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            if (prevStatusSummary == eachWarningFault)
            {
                hLogProc(LOG_DTC, "push ut err code\n");
                RackSys_PushDiagCode(DTC_Mem[IDX_CELL_UT_F].DtcNumber);
            }
            DTC_Mem[IDX_CELL_UT_F].Occurence = Detected;
            if (DTC_Mem[IDX_CELL_UT_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_UT_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_CELL_UT_F].Action == RelayOpen){
                hLogProc(LOG_DTC, "relay open\n");
                processRelayOffFromDTC();
            }
            break;
    }

    prevStatusSummary = StatusSummary;
    //4. Set Gateway Data
    if (DTC_Mem[IDX_CELL_UT_W].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_UNTEMP_WARN, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_UNTEMP_WARN, 0);
    }
    if (DTC_Mem[IDX_CELL_UT_F].Occurence == Detected)
    {
        hLogProc(LOG_DTC, "Cur UT Fault on\n");
        RackSys_StringEvent(STREV_UNTEMP_ALARM, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_UNTEMP_ALARM, 0);
    }

}


void _Diag_Update_Temp_Deviation(void)
{
    uint32_t DT;
    static uint16_t prevStatusSummary = 0;
    if (TEMP_DEV > MODULE_TEMP_DEV_FAULT_DETECT)
    {
        if (DTC_Temp_Deviation.Status.all != eachWarningFault && DTC_Temp_Deviation.Status.all != eachFault)
        {
            hLogProc(LOG_DTC, "td f+ \n");
            if (!DTC_Temp_Deviation.time.Set)
            {
                DTC_Temp_Deviation.time.Set = MODULE_TEMP_DEV_FAULT_WTIME;
                DTC_Temp_Deviation.time.Start = hTimCurrentTime();
            }
            DTC_Temp_Deviation.Status.all  = eachWarningFault;
        }
    }
    else
    { // Warning or Normal
        if (DTC_Temp_Deviation.Status.all == eachWarningFault)
        {
            DTC_Temp_Deviation.Status.all = eachWarning;
            DTC_Temp_Deviation.time.Set = 0;
        }
    }

    if (DTC_Temp_Deviation.Status.all == eachNormal)
    {
        if (TEMP_DEV > MODULE_TEMP_DEV_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "td w+ \n");
            if (!DTC_Temp_Deviation.time.Set)
            {
                DTC_Temp_Deviation.time.Set = MODULE_TEMP_DEV_WARNING_WTIME;
                DTC_Temp_Deviation.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_Temp_Deviation.time.Start);
            if (DT >= DTC_Temp_Deviation.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "td -->warning \n");
                DTC_Temp_Deviation.Status.all = eachWarning;
                DTC_Temp_Deviation.time.Set = 0;
            }
        }
        else
        {
            DTC_Temp_Deviation.time.Set = 0;
        }
    }
    else if (DTC_Temp_Deviation.Status.all == eachWarning)
    {
        /*
        if (TEMP_DEV > MODULE_TEMP_DEV_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "td f+ \n");
            if (!DTC_Temp_Deviation.time.Set)
            {
                DTC_Temp_Deviation.time.Set = MODULE_TEMP_DEV_FAULT_WTIME;
                DTC_Temp_Deviation.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_Temp_Deviation.time.Start);
            if (DT >= DTC_Temp_Deviation.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "td -->fault \n");
                DTC_Temp_Deviation.Status.all = eachFault;
                DTC_Temp_Deviation.time.Set = 0;
            }
        }else */
        if (TEMP_DEV <= MODULE_TEMP_DEV_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "td n+ \n");
            if (!DTC_Temp_Deviation.time.Set)
            {
                DTC_Temp_Deviation.time.Set = MODULE_TEMP_DEV_WARNING_RELEASE_TIME;
                DTC_Temp_Deviation.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_Temp_Deviation.time.Start);
            if (DT >= DTC_Temp_Deviation.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "td -->normal \n");
                DTC_Temp_Deviation.Status.all = eachNormal;
                DTC_Temp_Deviation.time.Set = 0;
            }
        }
        else
        {
            DTC_Temp_Deviation.time.Set = 0;
        }
    }
    else if (DTC_Temp_Deviation.Status.all == eachWarningFault || DTC_Temp_Deviation.Status.all == eachFault)
    {
        if (TEMP_DEV <= MODULE_TEMP_DEV_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "td w+ \n");
            if (!DTC_Temp_Deviation.time.Set)
            {
                DTC_Temp_Deviation.time.Set = MODULE_TEMP_DEV_FAULT_RELEASE_TIME;
                DTC_Temp_Deviation.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_Temp_Deviation.time.Start);
            if (DT >= DTC_Temp_Deviation.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "td -->warning \n");
                DTC_Temp_Deviation.Status.all = eachWarning;
                DTC_Temp_Deviation.time.Set = 0;
            }
        }
        else
        {
            hLogProc(LOG_DTC, "td f+ \n");
            DT = (uint32_t)(hTimCurrentTime() - DTC_Temp_Deviation.time.Start);
            if (DT >= DTC_Temp_Deviation.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "td -->fault \n");
                DTC_Temp_Deviation.Status.all = eachFault;
                DTC_Temp_Deviation.time.Set = 0;
            }
        }
    }
    //3. Update DTC
    switch (DTC_Temp_Deviation.Status.all)
    {
        case eachNormal:
            if (DTC_Mem[IDC_MODTEMP_DEV_W].ReleaseCond == Release)
            {
                DTC_Mem[IDC_MODTEMP_DEV_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDC_MODTEMP_DEV_F].ReleaseCond == Release)
            {
                DTC_Mem[IDC_MODTEMP_DEV_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDC_MODTEMP_DEV_W].Occurence = Detected;
            if (DTC_Mem[IDC_MODTEMP_DEV_F].ReleaseCond == Release)
            {
                DTC_Mem[IDC_MODTEMP_DEV_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            if (prevStatusSummary == eachWarningFault)
            {
                RackSys_PushDiagCode(DTC_Mem[IDC_MODTEMP_DEV_F].DtcNumber);
            }
            DTC_Mem[IDC_MODTEMP_DEV_F].Occurence = Detected;
            if (DTC_Mem[IDC_MODTEMP_DEV_W].ReleaseCond == Release)
            {
                DTC_Mem[IDC_MODTEMP_DEV_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDC_MODTEMP_DEV_F].Action == RelayOpen)
            {
                processRelayOffFromDTC();
            }

        break;
    }
    prevStatusSummary = DTC_Temp_Deviation.Status.all;
    //4. Set Gateway Data
    if (DTC_Mem[IDC_MODTEMP_DEV_W].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_TEMP_IMBAL_WARN, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_TEMP_IMBAL_WARN, 0);
    }
    if (DTC_Mem[IDC_MODTEMP_DEV_F].Occurence == Detected)
    {
        hLogProc(LOG_DTC, "TEMP DEV Fault on\n");
        RackSys_StringEvent(STREV_TEMP_IMBAL_ALARM, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_TEMP_IMBAL_ALARM, 0);
    }
}

void _Diag_Update_Cell_Deviation(void)
{
    uint16_t BalancingCplt= 1;
    static uint16_t prevStatusSummary = 0;
    uint32_t DT;
    //jae.kim
    //셀밸런싱을 구현 후 적용해야 함.
    //BalancingCplt=isAllBalCompleteFlagTrue();
    if ((SOC_CTL.SOC < CELL_DEVIATION_DETECT_UPPER_CONDITION_SOC)
        && (SOC_CTL.SOC >CELL_DEVIATION_DETECT_LOWER_CONDITION_SOC)
        && (BatCurr_CAN < CELL_DEVIATION_DETECT_CURRENT_CONDITION)
        && (BatCurr_CAN > (CELL_DEVIATION_DETECT_CURRENT_CONDITION * -1))
        && BalancingCplt == 1)
        {
        if (DTC_CellV_Deviation.Status.all == eachNormal)
        {
            if (CellVolt_dev > CELL_DEVIATION_DETECT_VOLTAGE)
            {

                hLogProc(LOG_DTC, "vd f+ \n");
                if (!DTC_CellV_Deviation.time.Set)
                {
                    DTC_CellV_Deviation.time.Set = CELL_DEVIATION_FAULT_WTIME;
                    DTC_CellV_Deviation.time.Start = hTimCurrentTime();
                }
                DT = (uint32_t)(hTimCurrentTime() - DTC_CellV_Deviation.time.Start);
                if (DT >= DTC_CellV_Deviation.time.Set)
                {// -- Go to Warning Status
                    hLogProc(LOG_DTC, "vd -->fault \n");
                    DTC_CellV_Deviation.Status.all = eachFault;
                    DTC_CellV_Deviation.time.Set = 0;
                }
            }
            else
            {
                DTC_CellV_Deviation.time.Set = 0;
            }
        }
        else
        {
            if (CellVolt_dev <= CELL_DEVIATION_DETECT_VOLTAGE)
            {

                hLogProc(LOG_DTC, "vd n+ \n");
                if (!DTC_CellV_Deviation.time.Set)
                {
                    DTC_CellV_Deviation.time.Set = CELL_DEVIATION_FAULT_RELEASE_TIME;
                    DTC_CellV_Deviation.time.Start = hTimCurrentTime();
                }
                DT = (uint32_t)(hTimCurrentTime() - DTC_CellV_Deviation.time.Start);
                if (DT >= DTC_CellV_Deviation.time.Set)
                {// -- Go to Warning Status
                    hLogProc(LOG_DTC, "vd -->normal \n");
                    DTC_CellV_Deviation.Status.all = eachNormal;
                    DTC_CellV_Deviation.time.Set = 0;
                }
            }
            else
            {
                DTC_CellV_Deviation.time.Set = 0;
            }
        }
    }
    else
    {
        DTC_CellV_Deviation.time.Set = 0;
        DTC_CellV_Deviation.Status.all = eachNormal;
    }
    switch ( DTC_CellV_Deviation.Status.all)
    {
        case eachNormal:
            if (DTC_Mem[IDX_CELL_DEVIATION_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_CELL_DEVIATION_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            if (prevStatusSummary == eachNormal)
            {
                RackSys_PushDiagCode(DTC_Mem[IDX_CELL_DEVIATION_F].DtcNumber);
            }
            DTC_Mem[IDX_CELL_DEVIATION_F].Occurence = Detected;
            if (DTC_Mem[IDX_CELL_DEVIATION_F].ReleaseCond == Release)
            {
            }
            break;
    }
    prevStatusSummary = DTC_CellV_Deviation.Status.all;

    if (DTC_CellV_Deviation.Status.all == eachFault)
    {
        hLogProc(LOG_DTC, "CV DEV Fault on\n");
        RackSys_StringEvent(STREV_VOL_IMBAL_WARN, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_VOL_IMBAL_WARN, 0);
    }

}


void _System_Isolation_Failure(void)
{
#if 0
    uint32_t DT;
    uint32_t status;
    static uint16_t prevStatusSummary = 0;
    uint32_t p_reg, n_reg;
    p_reg = GFD_P.REAL_GFD_VALUE;
    n_reg = GFD_N.REAL_GFD_VALUE;

    if (p_reg < ISOL_FAULT_REGISTANCE)
    {
        if (DTC_ISOL_ERR[0].Status.all != eachWarningFault && DTC_ISOL_ERR[0].Status.all != eachFault)
        {
            hLogProc(LOG_DTC, "is_p f+ \n");
            if (!DTC_ISOL_ERR[0].time.Set){
                DTC_ISOL_ERR[0].time.Set = ISOL_FAULT_WTIME;
                DTC_ISOL_ERR[0].time.Start = hTimCurrentTime();
            }
            DTC_ISOL_ERR[0].Status.all  = eachWarningFault;
        }
    }
    else
    {  //Warning or Normal
        if (DTC_ISOL_ERR[0].Status.all == eachWarningFault)
        {
            DTC_ISOL_ERR[0].Status.all = eachWarning;
            DTC_ISOL_ERR[0].time.Set = 0;
        }
    }

    if (n_reg < ISOL_FAULT_REGISTANCE)
    {
        if (DTC_ISOL_ERR[1].Status.all != eachWarningFault && DTC_ISOL_ERR[1].Status.all != eachFault)
        {
            hLogProc(LOG_DTC, "is_n f+ \n");
            if (!DTC_ISOL_ERR[1].time.Set){
                DTC_ISOL_ERR[1].time.Set = ISOL_FAULT_WTIME;
                DTC_ISOL_ERR[1].time.Start = hTimCurrentTime();
            }
            DTC_ISOL_ERR[1].Status.all  = eachWarningFault;

            DT = (uint32_t)(hTimCurrentTime() - DTC_ISOL_ERR[1].time.Start);
            if (DT >= DTC_ISOL_ERR[1].time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "is_n -->fault \n");
                DTC_ISOL_ERR[1].Status.all = eachFault;
                DTC_ISOL_ERR[1].time.Set = 0;
            }
        }
    }
    else
    {
        if (DTC_ISOL_ERR[1].Status.all == eachWarningFault)
        {
            DTC_ISOL_ERR[1].Status.all = eachWarning;
            DTC_ISOL_ERR[1].time.Set = 0;
        }
    }

    if (DTC_ISOL_ERR[0].Status.all == eachNormal)
    {
        if (p_reg < ISOL_WARNING_REGISTANCE)
        {
            hLogProc(LOG_DTC, "is_p w+ \n");
            if (!DTC_ISOL_ERR[0].time.Set)
            {
                DTC_ISOL_ERR[0].time.Set = ISOL_FAULT_WTIME;
                DTC_ISOL_ERR[0].time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_ISOL_ERR[0].time.Start);
            if (DT >= DTC_ISOL_ERR[0].time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "is_p -->warning \n");
                DTC_ISOL_ERR[0].Status.all = eachWarning;
                DTC_ISOL_ERR[0].time.Set = 0;
                GFD_P.F_GFD_FAIL = 1;
            }
        }
        else
        {
            DTC_ISOL_ERR[0].time.Set = 0;
        }
    }
    else if (DTC_ISOL_ERR[0].Status.all == eachWarning)
    {
        if (p_reg >= ISOL_WARNING_REGISTANCE)
        {
            hLogProc(LOG_DTC, "is_p n+ \n");
            if (!DTC_ISOL_ERR[0].time.Set)
            {
                DTC_ISOL_ERR[0].time.Set = ISOL_WARNING_RELEASE_TIME;
                DTC_ISOL_ERR[0].time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_ISOL_ERR[0].time.Start);
            if (DT >= DTC_ISOL_ERR[0].time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "is_p -->normal \n");
                DTC_ISOL_ERR[0].Status.all = eachNormal;
                DTC_ISOL_ERR[0].time.Set = 0;
                GFD_P.F_GFD_FAIL = 0;
            }
        }
        else
        {
            DTC_ISOL_ERR[0].time.Set = 0;
        }
    }
    else if ( DTC_ISOL_ERR[0].Status.all == eachWarningFault || DTC_ISOL_ERR[0].Status.all == eachFault)
    {
        if (p_reg >= ISOL_FAULT_REGISTANCE)
        {
            hLogProc(LOG_DTC, "is_p w+ \n");
            if (!DTC_ISOL_ERR[0].time.Set)
            {
                DTC_ISOL_ERR[0].time.Set = ISOL_FAULT_RELEASE_TIME;
                DTC_ISOL_ERR[0].time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_ISOL_ERR[0].time.Start);
            if (DT >= DTC_ISOL_ERR[0].time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "is_p -->warning \n");
                DTC_ISOL_ERR[0].Status.all = eachWarning;
                DTC_ISOL_ERR[0].time.Set = 0;
                GFD_P.F_GFD_FAIL = 0;
            }
        }
        else
        {
            DT = (uint32_t)(hTimCurrentTime() - DTC_ISOL_ERR[0].time.Start);
            if (DT >= DTC_ISOL_ERR[0].time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "is_p -->fault \n");
                DTC_ISOL_ERR[0].Status.all = eachFault;
                GFD_P.F_GFD_FAIL = 1;
                DTC_ISOL_ERR[0].time.Set = 0;
            }
        }
    }
    if (DTC_ISOL_ERR[1].Status.all == eachNormal)
    {
        if (n_reg < ISOL_WARNING_REGISTANCE)
        {
            hLogProc(LOG_DTC, "is_n w+ \n");
            if (!DTC_ISOL_ERR[1].time.Set)
            {
                DTC_ISOL_ERR[1].time.Set = ISOL_FAULT_WTIME;
                DTC_ISOL_ERR[1].time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_ISOL_ERR[1].time.Start);
            if (DT >= DTC_ISOL_ERR[1].time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "is_n -->warning \n");
                DTC_ISOL_ERR[1].Status.all = eachWarning;
                DTC_ISOL_ERR[1].time.Set = 0;
            }
        }else
        {
            DTC_ISOL_ERR[1].time.Set = 0;
        }
    }
    else if (DTC_ISOL_ERR[1].Status.all == eachWarning)
    {
        if (n_reg >= ISOL_WARNING_REGISTANCE)
        {
            hLogProc(LOG_DTC, "is_p n+ \n");
            if (!DTC_ISOL_ERR[1].time.Set)
            {
                DTC_ISOL_ERR[1].time.Set = ISOL_WARNING_RELEASE_TIME;
                DTC_ISOL_ERR[1].time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_ISOL_ERR[1].time.Start);
            if (DT >= DTC_ISOL_ERR[1].time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "is_n -->normal \n");
                DTC_ISOL_ERR[1].Status.all = eachNormal;
                DTC_ISOL_ERR[1].time.Set = 0;
                GFD_N.F_GFD_FAIL = 0;
            }
        }
        else
        {
            DTC_ISOL_ERR[1].time.Set = 0;
        }
    }
    else if ( DTC_ISOL_ERR[1].Status.all == eachWarningFault || DTC_ISOL_ERR[1].Status.all == eachFault)
    {
        if (n_reg >= ISOL_FAULT_REGISTANCE)
        {
            hLogProc(LOG_DTC, "is_n w+ \n");
            if (!DTC_ISOL_ERR[1].time.Set){
                DTC_ISOL_ERR[1].time.Set = ISOL_FAULT_RELEASE_TIME;
                DTC_ISOL_ERR[1].time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_ISOL_ERR[1].time.Start);
            if (DT >= DTC_ISOL_ERR[1].time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "is_n -->warning \n");
                DTC_ISOL_ERR[1].Status.all = eachWarning;
                DTC_ISOL_ERR[1].time.Set = 0;
                GFD_N.F_GFD_FAIL = 0;
            }
        }
        else
        {
            DT = (uint32_t)(hTimCurrentTime() - DTC_ISOL_ERR[1].time.Start);
            if (DT >= DTC_ISOL_ERR[1].time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "is_n -->fault \n");
                DTC_ISOL_ERR[1].Status.all = eachFault;
                GFD_N.F_GFD_FAIL = 1;
                DTC_ISOL_ERR[1].time.Set = 0;
            }
        }
    }

    if ( DTC_ISOL_ERR[0].Status.all == eachNormal && DTC_ISOL_ERR[1].Status.all == eachNormal)
    {
        status = eachNormal;
    }
    else if (DTC_ISOL_ERR[0].Status.all == eachWarning && DTC_ISOL_ERR[1].Status.all == eachWarning)
    {
        status = eachWarning;
    }
    else if (DTC_ISOL_ERR[0].Status.all == eachFault || DTC_ISOL_ERR[1].Status.all == eachFault)
    {
        status = eachFault;
    }


    //3. Update DTC
    switch (status)
    {
        case eachNormal:
        if (DTC_Mem[IDX_ISOL_W].ReleaseCond == Release)
        {
            DTC_Mem[IDX_ISOL_W].Occurence = NotDeteced;
        }
        if (DTC_Mem[IDX_ISOL_F].ReleaseCond == Release)
        {
            DTC_Mem[IDX_ISOL_F].Occurence = NotDeteced;
        }
            break;
        case eachWarning:
            DTC_Mem[IDX_ISOL_W].Occurence = Detected;
            if (DTC_Mem[IDX_ISOL_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_ISOL_F].Occurence = NotDeteced;
            }
        break;
        case eachFault:
            DTC_Mem[IDX_ISOL_F].Occurence = Detected;
            if (DTC_Mem[IDX_ISOL_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_ISOL_W].Occurence = NotDeteced;
            }
            if (prevStatusSummary == eachWarningFault)
            {
                RackSys_PushDiagCode(DTC_Mem[IDX_ISOL_F].DtcNumber);
            }
            if (DTC_Mem[IDX_ISOL_F].Action == RelayOpen)
            {
                processRelayOffFromDTC();
            }
            break;
    }

    prevStatusSummary = status;
    //4. Set Gateway Data
    if (DTC_Mem[IDX_ISOL_F].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_GND_FAULT, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_GND_FAULT, 0);
    }
#endif
}

void _Diag_Update_Communicaton(void)
{
    static uint16_t prevStatusSummary = 0;
    uint32_t DT;

    switch (DTC_TBMS_COM_ERR.Status.all)
    {
        case eachNormal:
            DTC_TBMS_COM_ERR.time.Set = 0;
            if (DTC_Mem[IDX_TBMS_RX_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_TBMS_RX_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_TBMS_RX_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_TBMS_RX_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_TBMS_COM_ERR.time.Set = 0;
            DTC_Mem[IDX_TBMS_RX_W].Occurence = Detected;
            if (DTC_Mem[IDX_TBMS_RX_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_TBMS_RX_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            DTC_Mem[IDX_TBMS_RX_F].Occurence = Detected;
            if (DTC_Mem[IDX_TBMS_RX_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_TBMS_RX_W].Occurence = NotDeteced;
            }
            if (prevStatusSummary == eachWarning)
            {
                RackSys_PushDiagCode(DTC_Mem[IDX_TBMS_RX_F].DtcNumber);
            }
            if (!DTC_TBMS_COM_ERR.time.Set)
            {
                DTC_TBMS_COM_ERR.time.Set = TRAY_COMM_FAULT_TIME;
                DTC_TBMS_COM_ERR.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_TBMS_COM_ERR.time.Start);
            if (DT >= DTC_TBMS_COM_ERR.time.Set)
            {
                DTC_TBMS_COM_ERR.time.Set = 0;
            }
            else
            {
                break;
            }
            if (DTC_Mem[IDX_TBMS_RX_F].Action == RelayOpen)
            {
                processRelayOffFromDTC();
            }
            break;
    }

    prevStatusSummary = DTC_TBMS_COM_ERR.Status.all;

    //4. Set Gateway Data
    if (DTC_Mem[IDX_TBMS_RX_F].Occurence == Detected)
    {
        hLogProc(LOG_DTC, "Com Err on\n");
        RackSys_StringEvent(STREV_COMM_ERROR, 1);
    }
    else
    {
        if (DTC_Mem[IDX_MBMS_RX_F].Occurence != Detected)
        {
            RackSys_StringEvent(STREV_COMM_ERROR, 0);
        }
    }
}

void _Diag_Update_MBMS_Communicaton(void)
{
    static uint16_t prevStatusSummary = 0;
    switch (DTC_MBMS_COM_ERR.Status.all)
    {
        case eachNormal:
            if (DTC_Mem[IDX_MBMS_RX_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_MBMS_RX_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_MBMS_RX_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_MBMS_RX_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDX_MBMS_RX_W].Occurence = Detected;
            if (DTC_Mem[IDX_MBMS_RX_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_MBMS_RX_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            DTC_Mem[IDX_MBMS_RX_F].Occurence = Detected;
            if (DTC_Mem[IDX_MBMS_RX_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_MBMS_RX_W].Occurence = NotDeteced;
            }
            if (prevStatusSummary == eachWarning)
            {
                RackSys_PushDiagCode(DTC_Mem[IDX_MBMS_RX_F].DtcNumber);
            }
            if (DTC_Mem[IDX_MBMS_RX_F].Action == RelayOpen)
            {
                processRelayOffFromDTC();
            }
            break;
    }

    prevStatusSummary = DTC_MBMS_COM_ERR.Status.all;

    //4. Set Gateway Data
    if (DTC_Mem[IDX_MBMS_RX_F].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_COMM_ERROR, 1);
    }
    else
    {
        if (DTC_Mem[IDX_TBMS_RX_F].Occurence != Detected)
        {
            RackSys_StringEvent(STREV_COMM_ERROR, 0);
        }
    }
}


void _Diag_Update_Over_Charge_Current_WNF1(void)
{
    uint32_t DT;
    static uint16_t prevStatusSummary = 0;
    int32_t current_10mA = BatCurr_CAN / 10;

    if (current_10mA > OVER_CHARGE_CURRENT_FAULT_DETECT)
    {
        if ( DTC_Curr_OVCHG1.Status.all != eachWarningFault && DTC_Curr_OVCHG1.Status.all != eachFault)
        {
            hLogProc(LOG_DTC, "oc f+ \n");
            if (!DTC_Curr_OVCHG1.time.Set)
            {
                DTC_Curr_OVCHG1.time.Set = OVER_CHARGE_CURRENT_FAULT_WTIME;
                DTC_Curr_OVCHG1.time.Start = hTimCurrentTime();
            }
            DTC_Curr_OVCHG1.Status.all  = eachWarningFault;
        }
    }
    else
    { //Warning or Normal
        if ( DTC_Curr_OVCHG1.Status.all == eachWarningFault)
        {
            hLogProc(LOG_DTC, "oc -->warning \n");
            DTC_Curr_OVCHG1.Status.all = eachWarning;
            DTC_Curr_OVCHG1.time.Set = 0;
        }
    }
    // 1. Check whether Over Charge Current 1 detected.
    if (DTC_Curr_OVCHG1.Status.all == eachNormal)
    {// -- Current Status is Normal
        if (current_10mA > OVER_CHARGE_CURRENT_WARN_DETECT)
        {
            hLogProc(LOG_DTC, "oc w+ \n");
            if (!DTC_Curr_OVCHG1.time.Set)
            {
                DTC_Curr_OVCHG1.time.Set = OVER_CHARGE_CURRENT_WARNING_WTIME;
                DTC_Curr_OVCHG1.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_Curr_OVCHG1.time.Start);
            if (DT >= DTC_Curr_OVCHG1.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "oc -->warning \n");
                DTC_Curr_OVCHG1.Status.all = eachWarning;
                DTC_Curr_OVCHG1.time.Set = 0;
            }
        }
        else
        {
            DTC_Curr_OVCHG1.time.Set = 0;
        }
    }
    else if (DTC_Curr_OVCHG1.Status.all == eachWarning)
    {//-- Current Status is Warning
        /*
        if (current > OVER_CHARGE_CURRENT_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "oc f+ \n");
            if (!DTC_Curr_OVCHG1.time.Set)
            {
                DTC_Curr_OVCHG1.time.Set = OVER_CHARGE_CURRENT_FAULT_WTIME;
                DTC_Curr_OVCHG1.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_Curr_OVCHG1.time.Start);
            if (DT >= DTC_Curr_OVCHG1.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "oc -->fault \n");
                DTC_Curr_OVCHG1.Status.all = eachFault;
                DTC_Curr_OVCHG1.time.Set = 0;
            }
        }
        else*/
        if (current_10mA < OVER_CHARGE_CURRENT_WARN_DETECT)
        {
            hLogProc(LOG_DTC, "oc n+ \n");
            if (!DTC_Curr_OVCHG1.time.Set)
            {
                DTC_Curr_OVCHG1.time.Set = OVER_CHARGE_CURRENT_WARNING_RELEASE_TIME;
                DTC_Curr_OVCHG1.time.Start = hTimCurrentTime();
            }

            DT = (uint32_t)(hTimCurrentTime() - DTC_Curr_OVCHG1.time.Start);
            if (DT >= DTC_Curr_OVCHG1.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "oc -->normal \n");
                DTC_Curr_OVCHG1.Status.all = eachNormal;
                DTC_Curr_OVCHG1.time.Set = 0;
            }
        }
        else
        {
            DTC_Curr_OVCHG1.time.Set = 0;
        }
    }
    else if (DTC_Curr_OVCHG1.Status.all == eachFault || DTC_Curr_OVCHG1.Status.all == eachWarningFault)
    {//-- Current Status is Fault
        if ( current_10mA <= OVER_CHARGE_CURRENT_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "oc w+ \n");
            if (!DTC_Curr_OVCHG1.time.Set)
            {
                DTC_Curr_OVCHG1.time.Set = OVER_CHARGE_CURRENT_FAULT_RELEASE_TIME;
                DTC_Curr_OVCHG1.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_Curr_OVCHG1.time.Start);
            if (DT >= DTC_Curr_OVCHG1.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "oc -->warning \n");
                DTC_Curr_OVCHG1.Status.all = eachWarning;
                DTC_Curr_OVCHG1.time.Set = 0;
            }
        }
        else
        {
            DT = (uint32_t)(hTimCurrentTime() - DTC_Curr_OVCHG1.time.Start);
            if (DT >= DTC_Curr_OVCHG1.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "oc -->fault \n");
                DTC_Curr_OVCHG1.Status.all = eachFault;
                DTC_Curr_OVCHG1.time.Set = 0;
            }
        }
    }
    else
    {
    /* To avoid QAC Warnings */
    }

    //3. Update DTC
    switch ( DTC_Curr_OVCHG1.Status.all)
    {
        case eachNormal:
            if (DTC_Mem[IDX_OV_CHRG_CURR_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OV_CHRG_CURR_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_OV_CHRG_CURR_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OV_CHRG_CURR_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDX_OV_CHRG_CURR_W].Occurence = Detected;
            if (DTC_Mem[IDX_OV_CHRG_CURR_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OV_CHRG_CURR_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            if (prevStatusSummary == eachWarningFault)
            {
                RackSys_PushDiagCode(DTC_Mem[IDX_OV_CHRG_CURR_F].DtcNumber);
            }
            DTC_Mem[IDX_OV_CHRG_CURR_F].Occurence = Detected;
            if (DTC_Mem[IDX_OV_CHRG_CURR_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OV_CHRG_CURR_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_OV_CHRG_CURR_F].Action == RelayOpen)
            {
                processRelayOffFromDTC();
                if (!EEPROMData.evlog.dtc)
                {
                    EEprom_WriteBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&DTC_Mem[IDX_OV_CHRG_CURR_F].DtcNumber, 2);
                    EEprom_ReadBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&EEPROMData.evlog, 2);
                }                
            }
            break;
    }

    prevStatusSummary = DTC_Curr_OVCHG1.Status.all;
    //4. Set Gateway Data
    if (DTC_Mem[IDX_OV_CHRG_CURR_W].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_OVCHAR_CUR_WARN, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVCHAR_CUR_WARN, 0);
    }
    if (DTC_Mem[IDX_OV_CHRG_CURR_F].Occurence == Detected)
    {
        hLogProc(LOG_DTC, "Cur OC Fault on\n");
        RackSys_StringEvent(STREV_OVCHAR_CUR_ALARM, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVCHAR_CUR_ALARM, 0);
    }
}

void _Diag_Update_Over_Discharge_Current_WNF1(void)
{
    uint32_t DT;
    static uint16_t prevStatusSummary = 0;
    int32_t current_10mA = BatCurr_CAN / 10;
    if (current_10mA < OVER_DSCHARGE_CURRENT_FAULT_DETECT)
    {
        if (DTC_Curr_OVDCHG1.Status.all != eachWarningFault && DTC_Curr_OVDCHG1.Status.all != eachFault)
        {
            hLogProc(LOG_DTC, "uc f+ \n");
            if (!DTC_Curr_OVDCHG1.time.Set)
            {
                DTC_Curr_OVDCHG1.time.Set = OVER_DSCHARGE_CURRENT_FAULT_WTIME;
                DTC_Curr_OVDCHG1.time.Start = hTimCurrentTime();
            }
            DTC_Curr_OVDCHG1.Status.all = eachWarningFault;
        }
    }
    else
    {
        if (DTC_Curr_OVDCHG1.Status.all == eachWarningFault)
        {
            hLogProc(LOG_DTC, "uc -->warning \n");
            DTC_Curr_OVDCHG1.Status.all = eachWarning;
            DTC_Curr_OVDCHG1.time.Set = 0;
        }
    }

    // 1. Check whether Over Charge Current 1 detected.
    if (DTC_Curr_OVDCHG1.Status.all == eachNormal)
    {// -- Current Status is Normal
        if (current_10mA < OVER_DSCHARGE_CURRENT_WARN_DETECT)
        {
            hLogProc(LOG_DTC, "uc w+ \n");
            if (!DTC_Curr_OVDCHG1.time.Set)
            {
                DTC_Curr_OVDCHG1.time.Set = OVER_DSCHARGE_CURRENT_WARNING_WTIME;
                DTC_Curr_OVDCHG1.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_Curr_OVDCHG1.time.Start);
            if (DT >= DTC_Curr_OVDCHG1.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "uc -->warning \n");
                DTC_Curr_OVDCHG1.Status.all = eachWarning;
                DTC_Curr_OVDCHG1.time.Set = 0;
            }
        }
        else
        {
            DTC_Curr_OVDCHG1.time.Set = 0;
        }
    }
    else if (DTC_Curr_OVDCHG1.Status.all == eachWarning)
    {//-- Current Status is Warning
        /*
        if (current < OVER_DSCHARGE_CURRENT_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "uc f+ \n");
            if (!DTC_Curr_OVDCHG1.time.Set){
                DTC_Curr_OVDCHG1.time.Set = OVER_DSCHARGE_CURRENT_FAULT_WTIME;
                DTC_Curr_OVDCHG1.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_Curr_OVDCHG1.time.Start);
            if (DT >= DTC_Curr_OVDCHG1.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "uc -->fault \n");
                DTC_Curr_OVDCHG1.Status.all = eachFault;
                DTC_Curr_OVDCHG1.time.Set = 0;
            }
        }
        else */
        if (current_10mA >= OVER_DSCHARGE_CURRENT_WARN_DETECT)
        {
            hLogProc(LOG_DTC, "uc n+ \n");
            if (!DTC_Curr_OVDCHG1.time.Set)
            {
                DTC_Curr_OVDCHG1.time.Set = OVER_DSCHARGE_CURRENT_WARNING_RELEASE_TIME;
                DTC_Curr_OVDCHG1.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_Curr_OVDCHG1.time.Start);
            if (DT >= DTC_Curr_OVDCHG1.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "uc -->normal \n");
                DTC_Curr_OVDCHG1.Status.all = eachNormal;
                DTC_Curr_OVDCHG1.time.Set = 0;
            }
        }
        else
        {
            DTC_Curr_OVDCHG1.time.Set = 0;
        }
    }
    else if (DTC_Curr_OVDCHG1.Status.all == eachFault  || DTC_Curr_OVDCHG1.Status.all == eachWarningFault)
    {//-- Current Status is Fault
        if (current_10mA >= OVER_DSCHARGE_CURRENT_FAULT_DETECT)
        {
            hLogProc(LOG_DTC, "uc f- \n");
            if (!DTC_Curr_OVDCHG1.time.Set)
            {
                DTC_Curr_OVDCHG1.time.Set = OVER_DSCHARGE_CURRENT_FAULT_RELEASE_TIME;
                DTC_Curr_OVDCHG1.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_Curr_OVDCHG1.time.Start);
            if (DT >= DTC_Curr_OVDCHG1.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "uc -->warning \n");
                DTC_Curr_OVDCHG1.Status.all = eachWarning;
                DTC_Curr_OVDCHG1.time.Set = 0;
            }
        }
        else
        {
            DT = (uint32_t)(hTimCurrentTime() - DTC_Curr_OVDCHG1.time.Start);
            if (DT >= DTC_Curr_OVDCHG1.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "uc -->fault \n");
                DTC_Curr_OVDCHG1.Status.all = eachFault;
                DTC_Curr_OVDCHG1.time.Set = 0;
            }
        }
    }
    else
    {
      /* To avoid QAC Warnings */
    }
    //3. Update DTC
    switch ( DTC_Curr_OVDCHG1.Status.all)
    {
        case eachNormal:
            if (DTC_Mem[IDX_OV_DSCHRG_CURR_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OV_DSCHRG_CURR_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_OV_DSCHRG_CURR_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OV_DSCHRG_CURR_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDX_OV_DSCHRG_CURR_W].Occurence = Detected;
            if (DTC_Mem[IDX_OV_DSCHRG_CURR_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OV_DSCHRG_CURR_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            if (prevStatusSummary == eachWarningFault)
            {
                RackSys_PushDiagCode(DTC_Mem[IDX_OV_DSCHRG_CURR_F].DtcNumber);
            }
            DTC_Mem[IDX_OV_DSCHRG_CURR_F].Occurence = Detected;
            if (DTC_Mem[IDX_OV_DSCHRG_CURR_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OV_DSCHRG_CURR_W].Occurence = NotDeteced;
            }
            if ( DTC_Mem[IDX_OV_DSCHRG_CURR_F].Action == RelayOpen)
            {
                processRelayOffFromDTC();
                if (!EEPROMData.evlog.dtc)
                {
                    EEprom_WriteBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&DTC_Mem[IDX_OV_DSCHRG_CURR_F].DtcNumber, 2);
                    EEprom_ReadBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&EEPROMData.evlog, 2);
                }                
            }
            break;
    }

    prevStatusSummary = DTC_Curr_OVDCHG1.Status.all;
    //4. Set Gateway Data
    if (DTC_Mem[IDX_OV_DSCHRG_CURR_W].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_OVDSCH_CUR_WARN, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVDSCH_CUR_WARN, 0);
    }
    if (DTC_Mem[IDX_OV_DSCHRG_CURR_F].Occurence == Detected)
    {
        hLogProc(LOG_DTC, "Cur ODC Fault on\n");
        RackSys_StringEvent(STREV_OVDSCH_CUR_ALARM, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVDSCH_CUR_ALARM, 0);
    }
}

void _Diag_Update_Under_SOC_WNF(void)
{
    uint32_t DT;
    static uint16_t prevStatusSummary = 0;
    uint32_t soc = SOC_CTL.SOC;

    if (soc <= UNDER_SOC_FAULT_DETECT && R.StringModel.CellVMin <= 2800)
    {
        if (DTC_UNDER_SOC.Status.all != eachWarningFault && DTC_UNDER_SOC.Status.all != eachFault)
        {
            hLogProc(LOG_DTC, "usoc f+ \n");
            if (!DTC_UNDER_SOC.time.Set)
            {
                DTC_UNDER_SOC.time.Set = UNDER_SOC_FAULT_WTIME;
                DTC_UNDER_SOC.time.Start = hTimCurrentTime();
            }
            DTC_UNDER_SOC.Status.all = eachWarningFault;
        }
    }
    else
    {
        if (DTC_UNDER_SOC.Status.all == eachWarningFault)
        {
            hLogProc(LOG_DTC, "usoc -->warning \n");
            DTC_UNDER_SOC.Status.all = eachWarning;
            DTC_UNDER_SOC.time.Set = 0;
        }

    }
    // 1. Check whether Over Charge Current 1 detected.
    if (DTC_UNDER_SOC.Status.all == eachNormal)
    {// -- Current Status is Normal
        if (soc <= UNDER_SOC_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "usoc w+ \n");
            if (!DTC_UNDER_SOC.time.Set){
                DTC_UNDER_SOC.time.Set = UNDER_SOC_WARNING_WTIME;
                DTC_UNDER_SOC.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_UNDER_SOC.time.Start);
            if (DT >= DTC_UNDER_SOC.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "usoc -->warning \n");
                DTC_UNDER_SOC.Status.all = eachWarning;
                DTC_UNDER_SOC.time.Set = 0;
            }
        }
        else
        {
            DTC_UNDER_SOC.time.Set = 0;
        }
    }
    else if (DTC_UNDER_SOC.Status.all == eachWarning)
    {//-- Current Status is Warning

        if (soc > UNDER_SOC_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "usoc n+ \n");
            if (!DTC_UNDER_SOC.time.Set){
                DTC_UNDER_SOC.time.Set = UNDER_SOC_WARNING_RELEASE_TIME;
                DTC_UNDER_SOC.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_UNDER_SOC.time.Start);
            if (DT >= DTC_UNDER_SOC.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "uc -->normal \n");
                DTC_UNDER_SOC.Status.all = eachNormal;
                DTC_UNDER_SOC.time.Set = 0;
            }
        }
        else
        {
            DTC_UNDER_SOC.time.Set = 0;
        }
    }
    else if (DTC_UNDER_SOC.Status.all == eachFault  || DTC_UNDER_SOC.Status.all == eachWarningFault)
    {//-- Current Status is Fault
        if (soc > UNDER_SOC_FAULT_DETECT || R.StringModel.CellVMin > 2800)
        {
            hLogProc(LOG_DTC, "usoc f- \n");
            if (!DTC_UNDER_SOC.time.Set)
            {
                DTC_UNDER_SOC.time.Set = UNDER_SOC_FAULT_RELEASE_TIME;
                DTC_UNDER_SOC.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_UNDER_SOC.time.Start);
            if (DT >= DTC_UNDER_SOC.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "usoc -->warning \n");
                DTC_UNDER_SOC.Status.all = eachWarning;
                DTC_UNDER_SOC.time.Set = 0;
            }
        }
        else
        {
            DT = (uint32_t)(hTimCurrentTime() - DTC_UNDER_SOC.time.Start);
            if (DT >= DTC_UNDER_SOC.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "uc -->fault \n");
                DTC_UNDER_SOC.Status.all = eachFault;
                DTC_UNDER_SOC.time.Set = 0;
            }
        }
    }
    else
    {
      /* To avoid QAC Warnings */
    }

        //3. Update DTC
    switch ( DTC_UNDER_SOC.Status.all)
    {
        case eachNormal:
            if (DTC_Mem[IDX_UNDER_SOC_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_UNDER_SOC_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_UNDER_SOC_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_UNDER_SOC_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDX_UNDER_SOC_W].Occurence = Detected;
            if (DTC_Mem[IDX_UNDER_SOC_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_UNDER_SOC_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            if (prevStatusSummary == eachWarningFault)
            {
                RackSys_PushDiagCode(DTC_Mem[IDX_UNDER_SOC_W].DtcNumber);
            }
            DTC_Mem[IDX_UNDER_SOC_F].Occurence = Detected;
            if (DTC_Mem[IDX_UNDER_SOC_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_UNDER_SOC_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_UNDER_SOC_F].Action == RelayOpen)
            {
                processRelayOffFromDTC();
            }
            break;
    }
    prevStatusSummary = DTC_UNDER_SOC.Status.all;
    //4. Set Gateway Data
    if (DTC_Mem[IDX_UNDER_SOC_W].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_UNSOC_WARN, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_UNSOC_WARN, 0);
    }
    if (DTC_Mem[IDX_UNDER_SOC_F].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_UNSOC_ALARM, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_UNSOC_ALARM, 0);
    }
}

void _Diag_Update_Over_SOC_WNF(void)
{
    uint32_t DT;
    static uint16_t prevStatusSummary = 0;
    uint32_t soc = SOC_CTL.SOC;

    if (soc >= OVER_SOC_FAULT_DETECT && R.StringModel.CellVMin >= 4200)
    {
        if (DTC_OVER_SOC.Status.all != eachWarningFault && DTC_OVER_SOC.Status.all != eachFault)
        {
            hLogProc(LOG_DTC, "osoc f+ \n");
            if (!DTC_OVER_SOC.time.Set)
            {
                DTC_OVER_SOC.time.Set = OVER_SOC_FAULT_WTIME;
                DTC_OVER_SOC.time.Start = hTimCurrentTime();
            }
            DTC_OVER_SOC.Status.all = eachWarningFault;
        }
    }
    else
    {
        if (DTC_OVER_SOC.Status.all == eachWarningFault)
        {
            hLogProc(LOG_DTC, "usoc -->warning \n");
            DTC_OVER_SOC.Status.all = eachWarning;
            DTC_OVER_SOC.time.Set = 0;
        }

    }
    // 1. Check whether Over Charge Current 1 detected.
    if (DTC_OVER_SOC.Status.all == eachNormal)
    {// -- Current Status is Normal
        if (soc >= OVER_SOC_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "osoc w+ \n");
            if (!DTC_OVER_SOC.time.Set){
                DTC_OVER_SOC.time.Set = OVER_SOC_WARNING_WTIME;
                DTC_OVER_SOC.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_SOC.time.Start);
            if (DT >= DTC_OVER_SOC.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "osoc -->warning \n");
                DTC_OVER_SOC.Status.all = eachWarning;
                DTC_OVER_SOC.time.Set = 0;
            }
        }
        else
        {
            DTC_OVER_SOC.time.Set = 0;
        }
    }
    else if (DTC_OVER_SOC.Status.all == eachWarning)
    {//-- Current Status is Warning

        if (soc < OVER_SOC_WARNING_DETECT)
        {
            hLogProc(LOG_DTC, "osoc n+ \n");
            if (!DTC_OVER_SOC.time.Set)
            {
                DTC_OVER_SOC.time.Set = OVER_SOC_WARNING_RELEASE_TIME;
                DTC_OVER_SOC.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_SOC.time.Start);
            if (DT >= DTC_OVER_SOC.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "osoc -->normal \n");
                DTC_OVER_SOC.Status.all = eachNormal;
                DTC_OVER_SOC.time.Set = 0;
            }
        }
        else
        {
            DTC_OVER_SOC.time.Set = 0;
        }
    }
    else if (DTC_OVER_SOC.Status.all == eachFault  || DTC_OVER_SOC.Status.all == eachWarningFault)
    {//-- Current Status is Fault
        if (soc < OVER_SOC_FAULT_DETECT || R.StringModel.CellVMin < 4200)
        {
            hLogProc(LOG_DTC, "usoc f- \n");
            if (!DTC_OVER_SOC.time.Set)
            {
                DTC_OVER_SOC.time.Set = OVER_SOC_FAULT_RELEASE_TIME;
                DTC_OVER_SOC.time.Start = hTimCurrentTime();
            }
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_SOC.time.Start);
            if (DT >= DTC_OVER_SOC.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "osoc -->warning \n");
                DTC_OVER_SOC.Status.all = eachWarning;
                DTC_OVER_SOC.time.Set = 0;
            }
        }
        else
        {
            DT = (uint32_t)(hTimCurrentTime() - DTC_OVER_SOC.time.Start);
            if (DT >= DTC_OVER_SOC.time.Set)
            {// -- Go to Warning Status
                hLogProc(LOG_DTC, "uc -->fault \n");
                DTC_OVER_SOC.Status.all = eachFault;
                DTC_OVER_SOC.time.Set = 0;
            }
        }
    }
    else
    {
      /* To avoid QAC Warnings */
    }

        //3. Update DTC
    switch ( DTC_OVER_SOC.Status.all)
    {
        case eachNormal:
            if (DTC_Mem[IDX_OVER_SOC_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OVER_SOC_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_OVER_SOC_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OVER_SOC_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDX_OVER_SOC_W].Occurence = Detected;
            if (DTC_Mem[IDX_OVER_SOC_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OVER_SOC_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            if (prevStatusSummary == eachWarningFault){
                RackSys_PushDiagCode(DTC_Mem[IDX_OVER_SOC_W].DtcNumber);
            }
            DTC_Mem[IDX_OVER_SOC_F].Occurence = Detected;
            if (DTC_Mem[IDX_OVER_SOC_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_OVER_SOC_W].Occurence = NotDeteced;
            }
            if ( DTC_Mem[IDX_OVER_SOC_F].Action == RelayOpen)
            {
                processRelayOffFromDTC();
            }
            break;
    }
    prevStatusSummary = DTC_OVER_SOC.Status.all;
    //4. Set Gateway Data
    if (DTC_Mem[IDX_OVER_SOC_W].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_OVSOC_WARN, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVSOC_WARN, 0);
    }

    if (DTC_Mem[IDX_OVER_SOC_F].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_OVSOC_ALARM, 1);
    }
    else
    {
        RackSys_StringEvent(STREV_OVSOC_ALARM, 0);
    }

}
static uint32_t volplaus_cnt;

//25ms 마다 불린다 가정.
void _Diag_Update_VoltPlausibility(void)
{

    //cell voltage sum - BatVolt,
    int32_t tmp;
    if (BatReal10mVolt > BatSum10mVolt)
    {
        tmp = BatReal10mVolt - BatSum10mVolt;
        if ((tmp > BatSum10mVolt * 0.1) && hFetState(D_FET))
        {
            volplaus_cnt++;
            if (volplaus_cnt > 120)
            { //3S 이상
                sm_voltagePlausiblity_error();
            }
        }
        else
        {
            volplaus_cnt = 0;
        }
    }
    else
    {
        tmp = BatSum10mVolt - BatReal10mVolt;
        if ((tmp > BatSum10mVolt * 0.1) && hFetState(D_FET))
        {
            volplaus_cnt++;
            if (volplaus_cnt > 120)
            {    //3S 이상
                sm_voltagePlausiblity_error();
            }
        }
        else
        {
            volplaus_cnt = 0;
        }
    }
}

void _Diag_Update_SM(void)
{
    //static uint16_t prevStatusSummary = 0;
    switch (DTC_SM.DtcErr.Status.all)
    {
        case eachNormal:
            if (DTC_Mem[IDX_SM_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_SM_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_SM_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_SM_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDX_SM_W].Occurence = Detected;
            if (DTC_Mem[IDX_SM_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_SM_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            DTC_Mem[IDX_SM_F].Occurence = Detected;
            if (DTC_Mem[IDX_SM_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_SM_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_SM_F].Action == RelayOpen)
            {
                processRelayOffFromDTC();
                if (!EEPROMData.evlog.dtc)
                {
                    EEprom_WriteBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&DTC_Mem[IDX_SM_F].DtcNumber, 2);
                    EEprom_ReadBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&EEPROMData.evlog.dtc, 2);
                    EEprom_WriteBuf(EEP_COM_EVTLOG_SUBCODE, (uint8_t*)&DTC_SM.reason_code, 2);
                    EEprom_ReadBuf(EEP_COM_EVTLOG_SUBCODE, (uint8_t*)&EEPROMData.evlog.sub_code, 2);
                }

            }
            break;
    }

    //prevStatusSummary = DTC_TBMS_COM_ERR.Status.all;

    //4. Set Gateway Data
    if (DTC_Mem[IDX_SM_F].Occurence == Detected)
    {
        RackSys_StringEvent(STREV_OTHER_ALARM, 1);
        RasckSys_SetFailReason(DTC_SM.reason_code);
    }
    else
    {
        RackSys_StringEvent(STREV_OTHER_ALARM, 0);
    }
}

void _System_Relay_Failure(void)
{
    uint32_t DT;
    uint32_t status;
    static uint16_t prevStatusSummary = 0;
    int16_t current = BatCurr_CAN;

    if (getRelayOnOffStatus() == 0)
    {
        if (current >=  700 || current <= -700)    //700mA 기준
        {
            hLogProc(LOG_DTC, "relay error detect s(%d) t(%d)\n", DTC_RELAY_ERR.Status.all, hTimCurrentTime() - DTC_RELAY_ERR.time.Start);
            switch (DTC_RELAY_ERR.Status.all)
            {
                case eachNormal:
                    if (DTC_RELAY_ERR.time.Set)
                    {
                        DT = hTimCurrentTime() - DTC_RELAY_ERR.time.Start;
                        if (DT >= DTC_RELAY_ERR.time.Set)
                        {
                            DTC_RELAY_ERR.time.Set = 0;
                            DTC_RELAY_ERR.Status.all = eachWarning;
                        }
                    }
                    else
                    {
                        DTC_RELAY_ERR.time.Set = RELAY_WARNING_WTIME;
                        DTC_RELAY_ERR.time.Start = hTimCurrentTime();
                    }
                    break;
                case eachWarning:
                        DTC_RELAY_ERR.time.Set = RELAY_FAULT_WTIME;
                        DTC_RELAY_ERR.time.Start = hTimCurrentTime();
                        DTC_RELAY_ERR.Status.all = eachWarningFault;
                    break;
                case eachWarningFault:
                    if (DTC_RELAY_ERR.time.Set)
                    {
                        DT = hTimCurrentTime() - DTC_RELAY_ERR.time.Start;
                        if (DT >= DTC_RELAY_ERR.time.Set)
                        {
                            DTC_RELAY_ERR.time.Set = 0;
                            DTC_RELAY_ERR.Status.all = eachFault;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        else
        {
            switch (DTC_RELAY_ERR.Status.all)
            {
                case eachWarningFault:
                    DTC_RELAY_ERR.time.Set = 0;
                    DTC_RELAY_ERR.Status.all = eachWarning;
                    break;
                case eachWarning:
                    if (!DTC_RELAY_ERR.time.Set)
                    {
                        DTC_RELAY_ERR.time.Set = RELAY_WARNING_RELEASE_TIME;
                        DTC_RELAY_ERR.time.Start = hTimCurrentTime();
                    }
                    else
                    {
                        DT = hTimCurrentTime() - DTC_RELAY_ERR.time.Start;
                        if (DT >= DTC_RELAY_ERR.time.Set)
                        {
                            DTC_RELAY_ERR.time.Set = 0;
                            DTC_RELAY_ERR.Status.all = eachNormal;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        DTC_RELAY_ERR.time.Set = 0;
        if (DTC_RELAY_ERR.Status.all != eachFault)
        {
            DTC_RELAY_ERR.Status.all = eachNormal;
        }
    }

    //3. Update DTC
    switch (DTC_RELAY_ERR.Status.all)
    {
        case eachNormal:
            if (DTC_Mem[IDX_RELAY_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_RELAY_W].Occurence = NotDeteced;
            }
            if (DTC_Mem[IDX_RELAY_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_RELAY_F].Occurence = NotDeteced;
            }
            break;
        case eachWarning:
            DTC_Mem[IDX_RELAY_W].Occurence = Detected;
            if (DTC_Mem[IDX_RELAY_F].ReleaseCond == Release)
            {
                DTC_Mem[IDX_RELAY_F].Occurence = NotDeteced;
            }
            break;
        case eachFault:
            DTC_Mem[IDX_RELAY_F].Occurence = Detected;
            if (DTC_Mem[IDX_RELAY_W].ReleaseCond == Release)
            {
                DTC_Mem[IDX_RELAY_W].Occurence = NotDeteced;
            }
            if (prevStatusSummary == eachWarningFault)
            {
                RackSys_PushDiagCode(DTC_Mem[IDX_RELAY_F].DtcNumber);
            }
            if (!EEPROMData.evlog.dtc)
            {
                EEprom_WriteBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&DTC_Mem[IDX_RELAY_F].DtcNumber, 2);
                EEprom_ReadBuf(EEP_COM_EVTLOG_DTC, (uint8_t*)&EEPROMData.evlog,2);
            }
            break;
    }

    prevStatusSummary = status;

    //4. Set Gateway Data
        if (DTC_Mem[IDX_RELAY_F].Occurence == Detected)
        {
            processRelayOffFromDTC();
            hLogProc(LOG_DTC, "Relay Fail on\n");
            RackSys_StringEvent(STREV_CONTACTOR_ERROR, 1);
        }
        else
        {
            RackSys_StringEvent(STREV_CONTACTOR_ERROR, 0);
        }
}


static void do_diag_update(void)
{
    static uint16_t diagFuncCallCnt = 0;

    _System_Isolation_Failure();    //GFD 체크는 10ms 마다 하도록 수정
    _System_Relay_Failure();

    if ((diagFuncCallCnt % 2) == 0)
    {
      //  _Diag_Update_Huge_Current();
        _Diag_Update_Communicaton();
        _Diag_Update_MBMS_Communicaton();
    }

    if ((diagFuncCallCnt % 5) == 0)
    {
        RackSys_UpdateModuleData();
        _Diag_Update_Bat_Over_Volt_WNF();
        _Diag_Update_Bat_Under_Volt_WNF();
        _Diag_Update_Over_Cell_Volt_WNF();
        _Diag_Update_Under_Cell_Volt_WNF();
        _Diag_Update_Over_Mod_Temp_WNF();
        _Diag_Update_Under_Mod_Temp_WNF();
        _Diag_Update_Cell_Deviation();
#if defined(KC_TEST) && defined(KC_TEMP_TEST)
#else
        _Diag_Update_Temp_Deviation();
#endif
        _Diag_Update_VoltPlausibility();

    }
    else if ((diagFuncCallCnt % 5) == 1)
    {
//#if defined(KBIA_TEST) && defined(KBIA_EFF_CAP_TEST)
//#else
        _Diag_Update_Over_Charge_Current_WNF1();
        _Diag_Update_Over_Discharge_Current_WNF1();
//#endif

    }
    else if ((diagFuncCallCnt % 5) == 2)
    {
      // _System_Relay_Faiure();
        _Diag_Update_SM();
    }
    else if ((diagFuncCallCnt % 5) == 3)
    {
        _Diag_Update_Under_SOC_WNF();
        _Diag_Update_Over_SOC_WNF();
    }
    else
    {
    /* To avoid QAC Warnings */
    }
    diagFuncCallCnt++;
    /* SYSTEM READY & FAULT INDICATOR */
    TBMS_STATUS();
}

#define DIAG_UPDATE   1
#define ON_RESETCMD   2
#define OFF_RESETCMD   3

static uint32_t dtc_state;
static void restore_racksys_stringevent(uint32_t dtcid, uint32_t reason)
{

    uint32_t ret = 0;
    uint8_t ext = 0;
    switch (dtcid)
    {
        case IDC_BAT_OV_F:
            ret = STREV_OVVOL_ALARM;
            break;
        case IDC_BAT_UV_F:
            ret = STREV_UNVOL_ALARM;
            break;
        case IDX_CELL_OV_F:
            ret = STREXTEV_OVCVOL_ALARM;
            ext = 1;
            break;
        case IDX_CELL_UV_F:
            ret = STREXTEV_UNCVOL_ALARM;
            ext = 1;
            break;
        case IDX_SM_F:
            ret = STREV_OTHER_ALARM;
            break;
        case IDX_RELAY_F:
            ret = STREV_CONTACTOR_ERROR;
            break;
        case IDX_CELL_OT_F:
            ret = STREV_OVTEMP_ALARM;
            break;
        case IDX_CELL_UT_F:
            ret = STREV_UNTEMP_ALARM;
            break;
        case IDC_MODTEMP_DEV_F:
            ret = STREV_UNTEMP_ALARM;
            break;
        case IDX_OV_CHRG_CURR_F:
            ret = STREV_OVCHAR_CUR_ALARM;
            break;
        case IDX_OV_DSCHRG_CURR_F:
            ret = STREV_OVDSCH_CUR_ALARM;
            break;
        case IDX_FUSE_F:
            break;
        case IDX_CELL_DEVIATION_F:
        case IDX_ISOL_F:
        case IDX_TBMS_RX_F:
        case IDX_MBMS_RX_F:
        case IDX_OVER_SOC_F:
        case IDX_UNDER_SOC_F:
            break;
    }
    if (ext == 1)
    {
        RackSys_StringExtEvent(ret, 1);
    }
    else
    {
        RackSys_StringEvent(ret, 1);
    }
    RasckSys_SetFailReason(reason);
}

static void restore_before_event(void)
{
    int32_t idxDTCmem;
    uint32_t dtc_number = EEPROMData.evlog.dtc;
    uint32_t reasoncode = EEPROMData.evlog.sub_code;
    for (idxDTCmem = 0; idxDTCmem < NUM_DTC; idxDTCmem++)
    {
        if (DTC_Mem[idxDTCmem].DtcNumber == dtc_number)
        {
            DTC_Mem[idxDTCmem].DtcType = FAULT;
            if (DTC_Mem[idxDTCmem].ReleaseCond != Release)
            {
                DTC_Mem[idxDTCmem].Occurence = Detected;
                RLY_CTL.RELAY_MODE = RELAY_OFF;
                restore_racksys_stringevent(idxDTCmem,reasoncode);
            }
            else
            {
                DTC_Mem[idxDTCmem].Occurence = NotDeteced;
                RLY_CTL.RELAY_MODE = 0;
            }
        }
    }
}

// 부팅 후 4초 뒤에 최초로 불린다.
void Diag_Update(void)
{
    static uint8_t ResetProcessActive = 0;
    static uint64_t tim;
    switch (dtc_state)
    {
        case 0:
            dtc_state = DIAG_UPDATE;
            break;
        case DIAG_UPDATE:
            if (EEPROMData.evlog.dtc)
            {
                restore_before_event();
            }
            else
            {
                do_diag_update();
            }
            if (SW_Reset_Command)
            {
                dtc_state = ON_RESETCMD;
            }
            break;
        case ON_RESETCMD:
            if (ResetProcessActive)
            {
                if (SW_Reset_Command == 0)
                {
                    dtc_state = OFF_RESETCMD;
                    PushJob(EEPROM_SCHEDULER_IDX, CLEAR_FAULT_RECORD, 0, NULL);
                }
                break;
            }
            ResetProcessActive = 1;
            tim = hTimCurrentTime();
            Diag_ErrorReset();
            break;
        case OFF_RESETCMD:
            ResetProcessActive = 0;
            if ((hTimCurrentTime() - tim) > 3000)
            {
                dtc_state = DIAG_UPDATE;
            }
            break;
    }
}


void TBMS_STATUS(void)
{
    uint16_t idxDTCmem;
    //uint16_t DtcNum= 0;
    uint16_t OccureDtcType = 0;
    //  uint16_t Fault= 0;

    for (idxDTCmem = 0; idxDTCmem < NUM_DTC; idxDTCmem++)
    {
        if (DTC_Mem[idxDTCmem].Occurence == Detected)
        {//-- Fault priority is higher than warning
            if (DTC_Mem[idxDTCmem].DtcType == FAULT)
            {
                OccureDtcType=OccureDtcType | eachFault;
            }
            else if (DTC_Mem[idxDTCmem].DtcType == WARNING)
            {
                OccureDtcType=OccureDtcType | eachWarning;
            }
        }
    }

    if (OccureDtcType == eachFault || OccureDtcType == eachWarningFault)
    {
        for(idxDTCmem = 0;idxDTCmem<NUM_DTC; idxDTCmem++)
        {
            if (DTC_Mem[idxDTCmem].Occurence==Detected && DTC_Mem[idxDTCmem].DtcType == FAULT)
            {
                //DtcNum=DTC_Mem[idxDTCmem].DtcNumber;
                break;
            }
        }
    }
    else if (OccureDtcType == eachWarning)
    {
        for(idxDTCmem = 0;idxDTCmem<NUM_DTC; idxDTCmem++)
        {
            if (DTC_Mem[idxDTCmem].Occurence==Detected && DTC_Mem[idxDTCmem].DtcType == WARNING)
            {
                //DtcNum=DTC_Mem[idxDTCmem].DtcNumber;
                break;
            }
        }
    }
    else
    {

    }
}

void _System_CurrSense_Failure(void)
{
  //Uint16 idxCurrFailNum= 0;
}


void Diag_ErrorReset(void)
{
    uint16_t idxDtcRst;
    ClearDTCTable();
    RLY_CTL.RELAY_MODE = 0;
    /* DTC Memory Reset */
    for (idxDtcRst = 0; idxDtcRst < NUM_DTC; idxDtcRst++)
    {
        /* Clear Error Flag */
        DTC_Mem[idxDtcRst].Occurence = NotDeteced;
    }
    /* Isolation Failure Reset */
    //GFD_N.F_GFD_FAIL = 0;
    //GFD_P.F_GFD_FAIL = 0;
}

PUBLIC void DTC_RBMS_Alarm(uint32_t code, uint8_t set, uint8_t reason)
{
    switch (code)
    {
        case DTC_TBMS_RX_F:
            DTC_TBMS_COM_ERR.Status.all = (set) ? eachFault: eachNormal;
            break;
        case DTC_RELAY_F:
            DTC_RELAY_ERR.Status.all = (set) ? eachFault: eachNormal;
            break;
        case DTC_MBMS_RX_F:
            DTC_MBMS_COM_ERR.Status.all = (set) ? eachFault: eachNormal;
            break;
        case DTC_SM_F:
            if (set)
            {
                DTC_SM.DtcErr.Status.all = eachFault;
                DTC_SM.reason_code |= reason;
            }
            else
            {
                DTC_SM.DtcErr.Status.all = eachNormal;
                DTC_SM.reason_code &= ~(reason);
            }
            break;
    }
}

PUBLIC uint32_t DTC_RBMS_Status(void)
{
    uint16_t idxDTCmem;
//    uint16_t DtcNum = 0;
    uint16_t OccureDtcType = 0;
    for (idxDTCmem = 0; idxDTCmem < NUM_DTC; idxDTCmem++)
    {
        if (DTC_Mem[idxDTCmem].Occurence == Detected)
        {//-- Fault priority is higher than warning
            if (DTC_Mem[idxDTCmem].DtcType == FAULT)
            {
                OccureDtcType |= eachFault;
            }
            else if (DTC_Mem[idxDTCmem].DtcType == WARNING)
            {
                OccureDtcType |= eachWarning;
            }
        }
    }
    return OccureDtcType;
}

/**
    *   @brief  Fault 발생 시 1, Fault가 발생하지 않는 경우 0
     **/
PUBLIC int32_t DTC_RBMS_COM_ERR(void)
{
    int ret = 0;
    if (DTC_Mem[IDX_TBMS_RX_F].Occurence == Detected || DTC_Mem[IDX_MBMS_RX_F].Occurence == Detected)
    {
        ret = 1;
    }
    else
    {
        ret  = 0;
    }
    return ret;
}

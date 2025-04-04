/*******************************************************************************
* Copyright(C) 2008, 2017 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*******************************************************************************
*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.
*
* This software is owned by Renesas Electronics Corp. and is protected under 
* all applicable laws, including copyright laws.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.
*
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORP. NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
* http://www.renesas.com/disclaimer

*""FILE COMMENT""***************************************************************
* System Name	: RAJ240090/100 sample program
* File Name		: mode.c
* Contents		: mode function source file
* Device		: RAJ240090/100
* Compiler		: CC-RL (V.4.01)
* Note			: 
*""FILE COMMENT END""**********************************************************/


#define _MODE_C

/***********************************************************************
** Include files
***********************************************************************/
#include "BMS.h"
#include "define.h"								// union/define definition
#include "dataflash.h"							// DataFlash data definition
#include "ram.h"								// ram.c
#include "datcalc_spl.h"						// datcalc_spl.c
#include "mcu.h"								// mcu.c
#include "afe.h"								// afe.c
#include "smbus.h"								// smbus.c
#include "smbus_ex.h"							// smbus_lnv.c

#include "mode.h"
#include "cell.h"

/***********************************************************************
** Pragma directive
***********************************************************************/

/***********************************************************************
** Announcement of internal function prototype
***********************************************************************/
static void FullCharge(void);					// Full Charge Processing
static void Set_ODC(void);						// Set OverDischargeCurrent
static void Chgwait_Chk(void);					// Charge wait Check
static void MainMode_Chk(void);					// MainMode Check
void Set_CCCV(void);							// Set CCCV


// - Internal variable -
//static _SHORT2CHAR	_tfc_cnt;					// Full Charge check count
//static BYTE		alow_cnt;						// Discharge stop count
//static BYTE		afcdc_cnt;						// Discharge detection count(FC)
//static BYTE		aocc_cnt1,aocc_cnt2;			// OverChgCurr count
//static BYTE		aodc_cnt1,aodc_cnt2;			// OverDisCurr count
//static BYTE		aoccrel_cnt;					// OverChgCurr release count
//static BYTE		aodcrel_cnt;					// OverDisCurr release count
//static BYTE		ascrel_cnt;						// ShortCurr release count
//static BYTE		aov_cnt;						// Over voltage release count

WORD			tcom14_new;						// CC/CV data update
WORD			tcom15_new;						// CC/CV data update


#if USE_AFETM
#define OCCREL_DETECT       aoccrel_cnt++;
#define OCCREL_RELEASE     aoccrel_cnt=0;
#define ODCREL_DETECT       aodcrel_cnt++;
#define ODCREL_RELEASE     aodcrel_cnt=0;
#define OCC1_DETECT           aocc_cnt1++;                             // OverChgCurr release count + 1
#define OCC1_RELEASE         aocc_cnt1=0;                            // OverChgCurr release count + 1
#define OCC2_DETECT           aocc_cnt2++;                            // OverChgCurr release count + 1
#define OCC2_RELEASE         aocc_cnt2=0;                            // OverChgCurr release count + 1
#define OCC3_DETECT         aocc_cnt3++                         // Permanent Charging Current
#define OCC3_RELEASE       aocc_cnt3=0;                            // OverChgCurr release count + 1
#define ODC1_DETECT            aodc_cnt1++;                            // OverDisChgCurr release count + 1
#define ODC1_RELEASE         aodc_cnt1=0;                         // OverDisChgCurr release count + 1
#define ODC2_DETECT           aodc_cnt2++;                              // OverDisChgCurr release count + 1
#define ODC2_RELEASE         aodc_cnt2=0;                         // OverDisChgCurr release count + 1
#define OCV_DETECT              ocv_p_cnt ++;
#define OCV_RELEASE            ocv_p_cnt =0;
#define UCV_DETECT              ucv_p_cnt++;
#define UCV_RELEASE           ucv_p_cnt =0;
#define UCVPF_DETECT         ucv_pf_cnt++;
#define UCVPD_DETECT        ucv_pd_cnt++;
#define UCVPD_RELEASE     ucv_pd_cnt=0;
#define OCT_DETECT          octp_cnt++;
#define OCT_RELEASE        octp_cnt =0;
#define ODT_DETECT          odtp_cnt++;
#define ODT_RELEASE        odtp_cnt=0;
#define UCT_DETECT           uctp_cnt++;
#define UCT_RELEASE        uctp_cnt=0; 
#define UDT_DETECT          udtp_cnt++;
#define UDT_RELEASE        udtp_cnt=0;
#define CVIMB_DETECT      cvimb_pf_cnt ++;
#define CVIMB_RELEASE    cvimb_pf_cnt=0; 

#endif

#if USE_MCUTM
#define OCCREL_DETECT    TTI.b.occrel=ON;
#define OCCREL_RELEASE  TTI.b.occrel=OFF; aoccrel_cnt=0;
#define ODCREL_DETECT    TTI.b.odcrel=ON;
#define ODCREL_RELEASE  TTI.b.odcrel=OFF; aodcrel_cnt=0;
#define OCC1_DETECT         TTI.b.occdet1= ON;                             // OverChgCurr release count + 1
#define OCC1_RELEASE       TTI.b.occdet1= OFF;  aocc_cnt1=0;                            // OverChgCurr release count + 1
#define OCC2_DETECT         TTI.b.occdet2= ON;                             // OverChgCurr release count + 1
#define OCC2_RELEASE       TTI.b.occdet2= OFF;  aocc_cnt2=0;                            // OverChgCurr release count + 1
#define OCC3_DETECT         TTI.b.occdet3= ON;                             // Permanent Charging Current
#define OCC3_RELEASE       TTI.b.occdet3= OFF;  aocc_cnt3=0;                            // OverChgCurr release count + 1
#define ODC1_DETECT         TTI.b.odcdet1= ON;                              // OverDisChgCurr release count + 1
#define ODC1_RELEASE       TTI.b.odcdet1= OFF;  aodc_cnt1=0;                         // OverDisChgCurr release count + 1
#define ODC2_DETECT         TTI.b.odcdet2= ON;                              // OverDisChgCurr release count + 1
#define ODC2_RELEASE       TTI.b.odcdet2= OFF;  aodc_cnt2=0;                         // OverDisChgCurr release count + 1
#define OCV_DETECT            TTI.b.ocvdet=ON; 
#define OCV_RELEASE         TTI.b.ocvdet=OFF; ocv_p_cnt =0;
#define UCV_DETECT           TTI.b.ucvdet=ON;
#define UCV_RELEASE         TTI.b.ucvdet=OFF; ucv_p_cnt =0;
#define UCVPF_DETECT       TTI.b.ucvpfdet = ON; 
#define UCVPD_DETECT       TTI.b.ucvpddet= ON; 
#define UCVPD_RELEASE     TTI.b.ucvpddet= OFF; ucv_pd_cnt=0;
#define OCT_DETECT            TTI.b.octdet=ON; 
#define OCT_RELEASE         TTI.b.octdet=OFF; octp_cnt =0;
#define ODT_DETECT            TTI.b.odtdet=ON; 
#define ODT_RELEASE         TTI.b.odtdet=OFF; odtp_cnt=0;
#define UCT_DETECT            TTI.b.uctdet=ON; 
#define UCT_RELEASE         TTI.b.uctdet=OFF; uctp_cnt=0; 
#define UDT_DETECT           TTI.b.udtdet=ON; 
#define UDT_RELEASE         TTI.b.udtdet=OFF; udtp_cnt=0;
#define CVIMB_DETECT      TTI.b.cvimbdet = ON;
#define CVIMB_RELEASE    TTI.b.cvimbdet = OFF; cvimb_pf_cnt=0; 
#endif


/*""FUNC COMMENT""***************************************************
* ID				: 1.0
* module outline	: Mode Check Processing Function
*-------------------------------------------------------------------
* Include			: 
*-------------------------------------------------------------------
* Declaration		: void Mode_Chk(void)
*-------------------------------------------------------------------
* Function			: Judge mode transition from each mode.
*					: 
*-------------------------------------------------------------------
* Return			: None
*-------------------------------------------------------------------
* Input				: None
* Output			: None
*-------------------------------------------------------------------
* Used function		: 
* 					: 
*-------------------------------------------------------------------
* Caution			: 
*					: 
*""FUNC COMMENT END""**********************************************/
void Mode_Chk(void)
{
#if 0
    unsigned long tt;

#if 0   //?? ??? Fuse ??. 
    if( f_sys_in == ON)     //Power Down Check
    {
        DIAGCODE = 0xFF;
        amode = M_PF_ON;	
    }
#endif
	// - Judgement process of each mode -
    switch( amode )
    {
        case M_WAKE:								// WakeUp
            Chgwait_Chk();							// Charge Wait Check
            if( amode == M_WAKE )					// WakeUp ?
            {
                if( t_com09 >= ((WORD)NORMAL_CV*D_SERIES))				// Full charge determination voltage or more ?
                {
                    amode = M_TERM;					// Mode to Charge Terminate
                    afet = FET_D;					// FET control [C=OFF,D=ON]
                    Set_FET();
                    FullCharge();					// Full Charge Processing
                } else {							// Not Full Charge
                    amode = M_DIS;					// Mode to Discharging
                    afet = FET_CD;					// FET control [C=ON ,D=ON]
                    Set_FET();
                }
            }
        break;

        case M_DIS:									// Discharge

            if( f_charge == ON )					// Charging current detection ?
            {
#if 0                
                if((adegree >= COTH) || (adegree < COTL))		// Rechargeable temperature range ?
                {
                    amode = M_COH;								// Mode to Charge OverHeat
                    f_over_tmp =ON;								// Set OTA Alarm
                    f_crelearn = OFF;                           // Charge relearning flag = OFF
                    relcap_c = 0;					        	// Reset relearn capacity
                    afet = FET_D;								// FET control [C=OFF,D=ON]
                    Set_FET();
                }else {
#endif                    

                    amode = M_CHG;					// Mode to Charging
                    afet = FET_CD;					// FET control [C=ON ,D=ON]
                    Set_FET();
#if 0                    
                }
#endif                
                    alow_cnt = 0;						// Clear Discharge stop count
            } else {								// Charging current undetected
                // - Discharging -> ChargeWait -
                if( tminv < DISSTOPV )				// Less than discharge stop voltage ?
                {
                    alow_cnt++;						// Discharge stop count + 1
                    if( alow_cnt == 4 )				// Discharge stop count = 4 ?
                    {
                        alow_cnt = 0;				// Clear Discharge stop count
                        lrc_w = 0;					// Clear Correction capacity
                        f_fulldis = ON;	
                        amode = M_CWIT;				// Mode to ChargeWait
                        afet = FET_C;				// FET control [C=ON ,D=OFF]
                        Set_FET();
                    }
                } else {							// Discharge stop voltage or more
                    alow_cnt = 0;					// Clear Discharge stop count
                }
            }
            MainMode_Chk();							// MainMode Check
        break;

        case M_CHG:									// Charging
            // - Charging -> Discharging -
            if( f_discharge == ON )					// Discharge current detection ?
            {
                amode = M_DIS;						// Mode to Discharging
                afet = FET_CD;						// FET control [C=ON ,D=ON]
                Set_FET();
                MainMode_Chk();						// MainMode Check
                break;
            } else {								// Not detect discharge
                Chgwait_Chk();
            }
#if 0
            // - Charging -> Charge OverHeat -
            if((adegree >= COTH) || (adegree < COTL))		// Rechargeable temperature range ?
            {
                amode = M_COH;								// Mode to Charge OverHeat
                f_over_tmp =ON;								// Set OTA Alarm
                f_crelearn = OFF;							// Charge relearning flag = OFF
                relcap_c = 0;					        	// Reset relearn capacity
                afet = FET_D;								// FET control [C=OFF,D=ON]
                Set_FET();
            }
#endif
            // - Charging -> ChargeTerminate -
            if( f_over_chg == ON )					// Full charge detection ?
            {
                amode = M_TERM;						// Mode to ChargeTerminate
                afet = FET_D;						// FET control [C=OFF,D=ON]
                Set_FET();
                MainMode_Chk();						// MainMode Check
                break;
            }

            MainMode_Chk();							// MainMode Check
            break;

        case M_TERM:								// Charge terminate

            if( f_discharge == ON )					// Discharge current detection ?
            {
                afcdc_cnt++;						// Discharge detection count(FC) + 1
                if( afcdc_cnt == 8 )				// Discharge detection count(FC) = 8 ?
                {
                    afcdc_cnt = 0;					// Clear Discharge detection count(FC)
                    amode = M_DIS;					// Mode to Discharging
                    afet = FET_CD;					// FET control [C=ON ,D=ON]
                    Set_FET();
                }
            } else {								// Discharge undetected
                afcdc_cnt = 0;						// Clear Discharge detection count(FC)
            }

            MainMode_Chk();							// MainMode Check
            break;

        case M_CWIT:								// Charge wait
            if( f_charge == ON && tminv >= DISSTOPV)	// Charging current detection & tminv is more than discharge stop voltage ?
            {
                amode = M_CHG;						// Mode to Charging
                afet = FET_CD;						// FET control [C=ON ,D=ON]
                Set_FET();
            }
            Slow_Chk();								// Slow mode check

            break;
        /*
        case M_OH:									// Over heat release temp
        if((adegree < (OTH - TH_hys))
        && (adegree >= (OTL + TH_hys)))
        {
        amode = M_WAKE;					// Mode to WakeUp
        } 
        break;
        */
#if 0        
        case M_COH:									// Over heat release temp
            if(f_discharge == ON || (adegree < (COTH - CTH_hys))
            && (adegree >= (COTL + CTH_hys)))
            {
                amode = M_WAKE;					// Mode to WakeUp
                f_over_tmp =OFF;				// Clear OTA Alarm
            }

            if((adegree >= DOTH) || (adegree < DOTL))		// Check temperature range
            {
                amode = M_DOH;								// Mode to Charge OverHeat
                afet = FET_OFF;								// FET control [C=OFF,D=OFF]
                Set_FET();
            }
            if(F_2NDTHE == ON)
            {
                if(adegree2 >= D_2NDTHM)		//  check temperature range
                {
                    amode = M_FOH;							// Mode to 2nd TH OverHeat
                    afet = FET_OFF;							// FET control [C=OFF,D=OFF]
                    Set_FET();
                }
            }
            break;

        case M_DOH:									// Over heat release temp
            if((adegree < (DOTH - DTH_hys))
            && (adegree >= (DOTL + DTH_hys)))
            {
                amode = M_COH;								// Mode to Charge OverHeat
                afet = FET_D;								// FET control [C=OFF,D=ON]
                Set_FET();
            } 
            if(F_2NDTHE == ON)
            {
                if(adegree2 >= D_2NDTHM)					// Rechargeable temperature range ?
                {
                    amode = M_FOH;							// Mode to 2nd TH OverHeat
                    afet = FET_OFF;							// FET control [C=OFF,D=OFF]
                    Set_FET();
                }
            }

        break;
#endif


        case M_OCC:									// Over charge current
            if( f_discharge == ON )					// Discharge current detection ?
            {
                OCCREL_RELEASE;					// Clear OverChgCurr release count
               // f_occ_alarm -> f_occ_p
                f_occ_p = OFF;					// Clear OCC Alarm
                amode = M_WAKE;						// Mode to WakeUp
            }
            if( tabsc < 100 )						// Curr less than 100mA ?
            {
                OCCREL_DETECT;						// OverChgCurr release count + 1
                if( aoccrel_cnt >= 120 )				// 30s elapsed ?
                {
                    OCCREL_RELEASE;				// Clear OverChgCurr release count
                    // f_occ_alarm -> f_occ_p
                    f_occ_p = OFF;				// Clear OCC Alarm
                    amode = M_WAKE;					// Mode to WakeUp
                }
            }
            break;

        case M_ODC:									// Over discharge current
            if( tabsc < 100 )						// Curr less than 100mA ?
            {
                ODCREL_DETECT						// OverDisCurr release count + 1
                if( aodcrel_cnt >= 120 )				// 30s elapsed ?
                {
                    ODCREL_RELEASE			// Clear OverDisCurr release count
                    //f_odc_alarm = OFF;				// Clear ODC Alarm
                    f_odc_p = OFF;
                    amode = M_WAKE;					// Mode to WakeUp
                }
            }

            if( f_charge == ON )					// Charging current detection ?
            {
                ODCREL_RELEASE					// Clear OverDisCurr release count
               // f_odc_alarm = OFF;					// Clear ODC Alarm
               f_odc_p = OFF;
                amode = M_WAKE;						// Mode to WakeUp
            }
            break;

        case M_SHC:									// Short current
            if( tabsc < 100 )						// Curr less than 100mA ?
            {
                ascrel_cnt++;						// Shortcurr release count + 1
                if( ascrel_cnt >= 40 )				// 10s elapsed ?
                {
                    ascrel_cnt = 0;					// Clear Shortcurr release count
                    f_short_alarm = OFF;			// Clear SC Alarm 
                    f_sdc_p = OFF;                    
                    amode = M_WAKE;					// Mode to WakeUp
                }
            }
            if( f_charge == ON )					// Charging current detection ?
            {
                ascrel_cnt = 0;						// Clear Shortcurr release count
                f_sdc_p = OFF;
                f_short_alarm = OFF;				// Clear SC Alarm
                amode = M_WAKE;						// Mode to WakeUp
            }
            break;

        case M_OV:
            if( tmaxv < (WORD)(CHGPV-D_CVP_TV) || f_discharge == ON)
            {
                f_ocv_p = OFF;					// Clear OV Alarm
                amode = M_WAKE;						// Move to WakeUp
            }

        break;

        case M_FOH:									// 2nd Th over heat
            // Discharge over heat mode check -

            break;
        case M_SWRST:
            if( swrst_cnt++ >= 4)
            {
                swrst_cnt = 0;
                SWreset();
            }
            break;
        case M_DOWN:
                PowerDown();
            break;
        case M_PF_ON:
	    #if 0
            FuseCutOff_On();
            #endif
	    //Fuse Blown
            for(tt=0;tt<2000;tt++){ //2sec
                Waittimer_us(250);	 //1ms
		Waittimer_us(250);
		Waittimer_us(250);
		Waittimer_us(250);
            }
	    #if 0
            FuseCutOff_Off();
	    #endif
            DF_Update_Set();
            amode = M_DOWN;
            return;
        case M_FMODE:
            
            break;
        default:
            break;
    }
	
	aafe = afet;
	
	if( f_cfctl == ON )							// C-FET=ON ?
	{
		f_cfet = ON;							// Set PackStatus:CFET
	} else {									// C-FET=OFF
		f_cfet = OFF;							// Clear PackStatus:CFET
	}

	if( f_dfctl == ON )							// D-FET=ON ?
	{
		f_dfet = ON;							// Set PackStatus:DFET
	} else {									// D-FET=OFF
		f_dfet = OFF;							// Clear PackStatus:DFET
	}
	
	Set_CCCV();									// Set CC & CV by different condition								// Set CC & CV by different condition
#endif
}


/*""FUNC COMMENT""***************************************************
* ID				: 1.0
* module outline	: Full Charge Processing Function
*-------------------------------------------------------------------
* Include			: 
*-------------------------------------------------------------------
* Declaration		: void FullCharge(void)
*-------------------------------------------------------------------
* Function			: Function of when full charge is judged.
*					: 
*-------------------------------------------------------------------
* Return			: None
*-------------------------------------------------------------------
* Input				: None
* Output			: None
*-------------------------------------------------------------------
* Used function		: 
* 					: 
*-------------------------------------------------------------------
* Caution			: 
*					: 
*""FUNC COMMENT END""**********************************************/
static void FullCharge(void)
{
	
	f_over_chg = ON;							// Set OVER_CHARGED_ALARM
	f_fullchg = ON;								// Set FULLY_CHARGED bit
	lrc_w = (long)t_com10c * 14400;				// Correction capacity calculation
	t_com0fc = t_com10c;						// Update the RemainingCapacity()
	t_com0d = 100;								// RSOC = 100%
	t_com14 = 0;								// ChargingCurrent() = 0
	t_com15 = 0;								// ChargingVotlage() = 0
}

/*""FUNC COMMENT""***************************************************
* ID				: 1.0
* module outline	: Charge Wait Check Processing Function
*-------------------------------------------------------------------
* Include			: 
*-------------------------------------------------------------------
* Declaration		: void Chgwait_Chk(void)
*-------------------------------------------------------------------
* Function			: Check Charge Wait.
*					: 
*-------------------------------------------------------------------
* Return			: None
*-------------------------------------------------------------------
* Input				: None
* Output			: None
*-------------------------------------------------------------------
* Used function		: 
* 					: 
*-------------------------------------------------------------------
* Caution			: 
*					: 
*""FUNC COMMENT END""**********************************************/
static void Chgwait_Chk(void)
{
	if( tminv < DISSTOPV)						// Discharge stop voltage less than voltage ?
	{
		amode = M_CWIT;								// Mode to Charge Wait
		afet = FET_C;								// FET control [C=ON,D=OFF]
		Set_FET();
		lrc_w = 0;									// Clear Correction capacity
	}
}


/*""FUNC COMMENT""***************************************************
* ID				: 1.0
* module outline	: OverDischargeCurrent Set Processing Function
*-------------------------------------------------------------------
* Include			: 
*-------------------------------------------------------------------
* Declaration		: void Set_ODC(void)
*-------------------------------------------------------------------
* Function			: Set to OverDischargeCurrent.
*					: 
*-------------------------------------------------------------------
* Return			: None
*-------------------------------------------------------------------
* Input				: None
* Output			: None
*-------------------------------------------------------------------
* Used function		: 
* 					: 
*-------------------------------------------------------------------
* Caution			: 
*					: 
*""FUNC COMMENT END""**********************************************/
static void Set_ODC(void)
{
	amode = M_ODC;								// Mode to OverDischargeCurrent
	f_odc_alarm = ON;							// Set ODC Alarm
	afet = FET_C;								// FET control [C=ON ,D=OFF]
	Set_FET();
}


/*""FUNC COMMENT""***************************************************
* ID				: 1.0
* module outline	: MainMode Check Processing Function
*-------------------------------------------------------------------
* Include			: 
*-------------------------------------------------------------------
* Declaration		: void MainMode_Chk(void)
*-------------------------------------------------------------------
* Function			: Judge mode transition from main mode to abnormal mode.
*					: 
*-------------------------------------------------------------------
* Return			: None
*-------------------------------------------------------------------
* Input				: None
* Output			: None
*-------------------------------------------------------------------
* Used function		: 
* 					: 
*-------------------------------------------------------------------
* Caution			: 
*					: 
*""FUNC COMMENT END""**********************************************/
static void MainMode_Chk(void)
{
	// - Main -> Sleep
	Slow_Chk();									// Slow mode check
	if( f_slow == ON )							// Become slow mode ?
	{
		amode = M_ALON;							// Stand Alone mode
		//afet = FET_D;							// FET [C=OFF,D=ON]
		//Set_FET();
	}
#if 0
	// - Main -> Discharge Over Heat -
	if((adegree >= DOTH) || (adegree < DOTL))		// Check temperature range
	{
		amode = M_DOH;								// Mode to Charge OverHeat
		f_over_tmp =ON;								// Set OTA Alarm
		afet = FET_OFF;								// FET control [C=OFF,D=OFF]
		Set_FET();
	}

	// - Main -> OverChargeCurrent 1-
	if( f_charge == ON )						// Charging current detection ?
	{
		if( tabsc >= OVERCH1 )					// OverChargeCurrent detection ?
		{
			aocc_cnt1++;							// OverChgCurr release count + 1
			if( aocc_cnt1 == OCC1JT )				// OCC determination time elapsed ?
			{
				aocc_cnt1 = 0;					// Clear OverChgCurr release count
				amode = M_OCC;					// Mode to OverChargeCurrent
				f_occ_alarm = ON;				// Set OCC Alarm
				afet = FET_D;					// FET control [C=OFF,D=ON]
				Set_FET();
			}
		} else {								// OCC undetected
			aocc_cnt1 = 0;						// Clear OverChgCurr release count
		}
	} else {
		aocc_cnt1 = 0;							// Clear OverChgCurr release count
	}
	
	// - Main -> OverChargeCurrent 2-
	if( f_charge == ON )						// Charging current detection ?
	{
		if( tabsc >= OVERCH2 )					// OverChargeCurrent detection ?
		{
			aocc_cnt2++;						// OverChgCurr release count + 1
			if( aocc_cnt2 == OCC2JT )			// OCC determination time elapsed ?
			{
				aocc_cnt2 = 0;					// Clear OverChgCurr release count
				amode = M_OCC;					// Mode to OverChargeCurrent
				f_occ_alarm = ON;				// Set OCC Alarm
				afet = FET_D;					// FET control [C=OFF,D=ON]
				Set_FET();
			}
		} else {								// OCC undetected
			aocc_cnt2 = 0;						// Clear OverChgCurr release count
		}
	} else {
		aocc_cnt2 = 0;							// Clear OverChgCurr release count
	}
	
	// - Main -> OverDischargeCurrent 1-
	if( f_discharge == ON )						// Discharge current detection ?
	{
		if( tabsc >= OVERDI1 )					// OverDischargeCurrent more ?
		{
			aodc_cnt1++;						// OverDisCurr release count + 1
			if( aodc_cnt1 == ODC1JT )			// ODC determination time elapsed ?
			{
				aodc_cnt1 = 0;					// Clear OverDisCurr release count
				Set_ODC();						// Set OverDischargeCurrent
			}
		} else {								// ODC undetected
			aodc_cnt1 = 0;						// Clear OverDisCurr release count
		}
	}
	
	// - Main -> OverDischargeCurrent 2-
	if( f_discharge == ON )						// Discharge current detection ?
	{
		if( tabsc >= OVERDI2 )					// OverDischargeCurrent more ?
		{
			aodc_cnt2++;						// OverDisCurr release count + 1
			if( aodc_cnt2 == ODC2JT )			// ODC determination time elapsed ?
			{
				aodc_cnt2 = 0;					// Clear OverDisCurr release count
				Set_ODC();						// Set OverDischargeCurrent
			}
		} else {								// ODC undetected
			aodc_cnt2 = 0;						// Clear OverDisCurr release count
		}
	}
	
	// - Main -> OverVoltage -
	if( f_charge == ON )						// Charge current detection ?
	{
		if( tmaxv >= CHGPV )					// OverVoltage more ?
		{
			aov_cnt++;							// OverVoltage release count + 1
			if( aov_cnt == CHGPVJT )			// OV determination time elapsed ?
			{
				aov_cnt = 0;					// Clear OverVoltage release count
				amode = M_OV;					// Mode to OverVoltage
				f_ov_alarm = ON;				// Set OV Alarm
				afet = FET_D;					// FET control [C=OFF,D=ON]
				Set_FET();
				
			}
		} else {									// OV undetected
			aov_cnt = 0;							// Clear OverVoltage release count
		}
	}
	
	if(F_2NDTHE == ON)
	{
	
		// - Main -> 2nd TH Over Heat -
		if((adegree2 >= D_2NDTHM))						// Check  temperature range
		{
			amode = M_FOH;								// Mode to 2nd TH OverHeat
			f_over_tmp =ON;								// Set OTA Alarm
			afet = FET_OFF;								// FET control [C=OFF,D=OFF]
			Set_FET();
		}
	}
#endif
}

/*""FUNC COMMENT""***************************************************
* ID				: 1.0
* module outline	: FET Set Processing Function
*-------------------------------------------------------------------
* Include			: 
*-------------------------------------------------------------------
* Declaration		: void Set_FET(void)
*-------------------------------------------------------------------
* Function			: Set to FET.
*					: 
*-------------------------------------------------------------------
* Return			: None
*-------------------------------------------------------------------
* Input				: None
* Output			: None
*-------------------------------------------------------------------
* Used function		: 
* 					: 
*-------------------------------------------------------------------
* Caution			: 
*					: 
*""FUNC COMMENT END""**********************************************/
void Set_FET(void)
{
	AFE_WR(AFE_FCON,afet);
}

void Vol_chk(void)
{

}

/*""FUNC COMMENT""***************************************************
* ID				: 1.0
* module outline	: ALARM Check Processing Function
*-------------------------------------------------------------------
* Include			: 
*-------------------------------------------------------------------
* Declaration		: void Alarm_Chk(void)
*-------------------------------------------------------------------
* Function			: Judge the Alarm of BatteryStatus() except
*					:  depending on mode.
*-------------------------------------------------------------------
* Return			: None
*-------------------------------------------------------------------
* Input				: None
* Output			: None
*-------------------------------------------------------------------
* Used function		: 
* 					: 
*-------------------------------------------------------------------
* Caution			: 
*					: 
*""FUNC COMMENT END""**********************************************/
void Alarm_Chk(void)
{

}

/*""FUNC COMMENT""***************************************************
* ID				: 1.0
* module outline	: Slow mode check function
*-------------------------------------------------------------------
* Include			: 
*-------------------------------------------------------------------
* Declaration		: void Slow_Chk(void)
*-------------------------------------------------------------------
* Function			: Check the condition of slow mode.
*					:
*-------------------------------------------------------------------
* Return			: None
*-------------------------------------------------------------------
* Input				: None
* Output			: None
*-------------------------------------------------------------------
* Used function		: 
* 					: 
*-------------------------------------------------------------------
* Caution			: 
*					: 
*""FUNC COMMENT END""**********************************************/
void Slow_Chk(void)
{
}

void Cur_chk(void)
{
 

}

void Temp_chk(void)
{
 
  
}

void CVImb_chk(void)
{

}

void Dtc_Chk(void)
{
 
 

}


/*""FUNC COMMENT""***************************************************
* ID				: 1.0
* module outline	: Set CC & CV by different condition
*-------------------------------------------------------------------
* Include			: 
*-------------------------------------------------------------------
* Declaration		: void Set_CCCV(void)
*-------------------------------------------------------------------
* Function			: Set CC & CV by different condition.
*					:
*-------------------------------------------------------------------
* Return			: None
*-------------------------------------------------------------------
* Input				: None
* Output			: None
*-------------------------------------------------------------------
* Used function		: 
* 					: 
*-------------------------------------------------------------------
* Caution			: 
*					: 
*""FUNC COMMENT END""**********************************************/
void Set_CCCV(void)
{
	
	tcom14_new = t_com14;						// CC/CV backup
	tcom15_new = t_com15;
	

	if( f_cfctl == ON )							// C-FET=ON ?
	{

		if( f_fullchg == OFF )					// FULLY_CHARGED = OFF ?
		{

			tcom15_new = NORMAL_CV * D_SERIES;	// Set NormalCV * Serial number of cell

			// - CC setting -
			if( amode == M_CWIT )				// Charge Wait mode ?
			{
				tcom14_new = CHGWAIT_CC;		// ChargingCurrent(Charge Wait)
			} else {							// Not Charge Wait mode ?
				tcom14_new = NORMAL_CC;			// ChargingCurrent(Normal)
			}
		} else {								// FULLY_CHARGED = ON
			tcom14_new = 0;						// CC/CV = 0
			tcom15_new = 0;
		}

	}else										// C-FET=OFF
	{
		tcom15_new = 0;							// CV = 0
		tcom14_new = 0;							// CC = 0		
	}

	t_com14 = tcom14_new;
	t_com15 = tcom15_new;
	
}

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
* File Name		: datcalc_spl.c
* Contents		: Data calculation function source file
* Device		: RAJ240090/100
* Compiler		: CC-RL (V.4.01)
* Note			: 
*""FILE COMMENT END""**********************************************************/

#define _DATCALC_SPL

#include "define.h"								// union/define definition
#include "datcalc_spl.h"						// datcalc_spl.c
#include "dataflash.h"							// DataFlash data definition
#include "ram.h"								// ram.c
#include "smbus_ex.h"							// smbus_ex.c
#include "mcu.h"								// mcu.c
#include "afe.h"								// afe.c
#include "cell.h"								// cell.c
#include "mode.h"

/***********************************************************************
** Announcement of internal function prototype
***********************************************************************/
static void Calc_NormalRC(WORD	trc);

void Calc_OCV(void)
{
    //1시간 단위로 전압 변화 측정
    //전압 측정 변화가 없는 경우 OCV 로 간주
    if (f_discharge == ON || f_charge == ON) {
         OCV_ST = OFF; 
         ocv_watch = OFF;
         ocv_count_1sec = 0;
         return;
    }else{
        //OCV 상태이면 timer를 동작시키지 않는다. 
        if( OCV_ST == ON)
            return;
        ocv_watch = ON;
    }
    
    // jae.kim 지금은 테스틀 위해서 1분만 적용. 실제 OCV 추정시간은 10~15분정도로 예상 
    if( ocv_count_1sec >= 60*15  ){ 
        //count 가 더이상 되지 않도록 off 한다. 
        ocv_watch = OFF;
        OCV_ST = ON; 
            }
    
}


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: RemainingCapacity() Calculation Function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		:  void Make_RC(void)
 *--------------------------------------------------------------------------
 * Function			: Calculate RemainingCapacity() from RC integration work.
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: lrc_w		: RC integration work
 *					: 
 * Output			: t_com0fc	: RemainingCapacity() [mAh]
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 *""FUNC COMMENT END""*****************************************************/
void Make_RC(void)
{
	WORD	twork;
        short twork_h;
	long	lwork;

	twork = (WORD)(lrc_w / (60*60*4));			// RC = RC_W / (60min*60sec*4)
	t_com0fc = twork;
        twork_h = (short)((lrc_w_hunate/(60*60)/50));
        twork_h = (twork_h<0) ? 0: twork_h;
        t_com0fc_h = twork_h;
	if( t_com0fc == 0 )				// RC = 0 ?
	{
		t_com0c = 0;				// Clear MaxError
	}
			
	lwork = (long)t_com0fc * twh_w / 10000;
	if( lwork < 65535 )							// not over flow
	{
		t_com0fp = (WORD)lwork;
	} else {
		t_com0fp = 65535;
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Function of capacity calculation while
 * 					: discharging and voltage is higher than CP_H.
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Calc_NormalRC(WORD trc)
 *--------------------------------------------------------------------------
 * Function			: It calculates capacity while discharging and
 *					:  the voltage is higher than CP_H.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: lrc_w		: RC integration work
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 *""FUNC COMMENT END""*****************************************************/
static void Calc_NormalRC(WORD	trc)
{
	long	lwork;

        lrc_w_hunate -= (long)trc*cvt_ms/20; //Hunate는  증분에 대한 가중치를 계산하지 않는다.
    // 잔존 용량이 음수 이면  0 로 
    if(lrc_w_hunate < 0 ) lrc_w_hunate = 0;
    
	if( f_study_d == ON )					// Now discharge relearning ?
	{
		lrcdr_w += trc;						// Integrate discharge rel.cap
	}
	lwork = (long)t_com10c * D_CP_H * 144;	// Calculate CP_H capacity
	if( lrc_w < lwork )						// Correction cap < CP_H cap ?
	{
		trc /= 2;							// Correction coefficient = 0.5
	}
	lrc_w -= trc;							// Substruct correction value
	lwork = (long)t_com10c * D_CP_L * 144;	// Calculate CP_L capacity
	if( lrc_w < lwork )						// Correction cap < CP_L cap ?
	{
		lrc_w = lwork;						// Correction cap hold to CP_H cap
	}
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: RemainingCapacity() calculation function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Calc_RC(void)
 *--------------------------------------------------------------------------
 * Function			: It calculates RemainingCapacity()
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: t_com0fc	: RemainingCapacity() [mAh]
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: Calc_NormalRC(), Make_RC()
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 *""FUNC COMMENT END""*****************************************************/
void Calc_RC(void)
{
	static BYTE	a0p_cnt;
        static WORD        old_SOC;
	WORD	twork;
	
	if( f_fullchg == ON )						// After detecting FC ?
	{
		if( t_com0a <= 0 )						// Not charging ?
		{
			if( tabsc > D_DCDET )				// Not discharging ?
			{
				f_fullchg = OFF;				// Full charge detection flag clear
			}
		}
	}

	if( t_com0a > 0 )							// Charging ?
	{
		if( f_studied == ON )						// Studied flag = ON ?
		{
			f_studied = OFF;					// Clear studied flag
			t_com10c = tcom10c_w;				// Store relearn value to FCC
#if DEBUG_USE && BMS_LOG			
			Printf((__far const unsigned char *)"Designed Cap changed!!\n");
#endif                                
			Calc_FCC_P();						// Calculate FCC[10mWh]
		}

		if( f_charge == ON )						// Charging ?
		{
			if( f_over_chg == OFF )					// No OVER_CHARGED_ALARM ?
			{
				f_study_d = OFF;				// Clear discharge relearn flag
			}
			
			if( f_cp_h == ON					// CP_H detected ?
				&& t_com0d > D_CP_H )				// & RSOC > CP_H ?
			{
				f_cp_h = OFF;					// Clear CP_H detection flag
			}
				
			if( f_cp_l == ON					// CP_L detected ?
				&& t_com0d > D_CP_L )				// & RSOC > CP_L ?
			{
				f_cp_l = OFF;					// Clear CP_L detection flag
			}
		}
			
		if( f_study_c == ON )						// Now charge relarning ?
		{
			lrccr_w += tabsc;					// Calculate chg relearn cap.
			lrc_w_hunate += (long)(tabsc)*cvt_ms/20;
			if( f_rcsame == ON )					// Correction cap = chg relearn cap ?
			{
				lrc_w = lrccr_w;				// Set same value to RC
			} 
			else
			{							// Not same
				if( lrc_w < lrccr_w )				// Chg relearn cap is bigger ?
				{
					lrc_w += (tabsc << 1);			// Correction coefficient = 2
					if( lrc_w >= lrccr_w )			// Inverted ?
					{
						f_rcsame = ON;			// Set same RC flag
					}
				} 
				else
				{						// Correction cap is bigger
					lrc_w += (tabsc >> 1);			// Correction coefficient = 0.5
					if( lrc_w < lrccr_w )			// Inverted ?
					{
						f_rcsame = ON;			// Set same RC flag
					}
				}
			}
		} 
		else 
		{								// Not charge relarning
			lrc_w += tabsc;						// Calc RC without correction
			lrc_w_hunate += (long)(tabsc)*cvt_ms/20;
										// CPx detected ?
			if( f_cp_h == ON || f_cp_l == ON )
			{
				lrccr_w += tabsc;				// Integrate chg rel.cap.
			}
		}
		
	} 
	else 
	{  // Discahrging or No current ( 방전, 대기 )
	        //Pack 전압으로 변경함.
		if( t_com09  <  (D_0PVOLT*4) )	// lower than 0% voltage ?
		{
			if( a0p_cnt >= 4 || ++a0p_cnt == 4 )			// Detect more than 4 times ?
			{
				if( F_CSDIS == OFF )				// Charge relearn not disable ?
				{
					f_study_c = ON;				// Set charge relearn flag
				}
				
				f_cp_h = ON;					// Set CP_H detection flag
				f_cp_l = ON;					// Set CP_L detection flag
				lrc_w = 0;					// Correction capacity = 0
				lrccr_w = 0;					// Charge relearn capacity = 0
				f_fulldis = ON;				// Set FD flag

				if( f_ampn == ON )				// Discharging ?
				{
					f_term_dis = ON;			// TERMINATE_DISCHARGE_ALARM=ON
				}
				t_com0fc = 0;					// RC[mAh] = 0
				t_com0fp = 0;					// RC[10mWh] = 0
				// (Process to prevent fluctuation when jumping CC.)
				toldrc = 0;					// Set last RC to 0
				return;
			}
		} 
		else 
		{								// Higher than 0% voltage
			a0p_cnt = 0;						// Clear 0% detection counter
		}
		
        	// Normal RC calculation

		if( tabsc <= D_RCCMIN )					// Current < not count curr ?
		{
		        //대기 (자연방전) 
		    if( OCV_ST == ON){
                    lrc_w =(long)( t_com0d) * t_com10c*144;
                    lrc_w_hunate = (long)( uSOC) * t_com10c_h * 180; // 20ms 단위로 계산  --> uSOC를 0.1% 까지 정수화 해서 *1800 ->*180 으로 변경 
                    }else{
                        //내부 소비 전류를 차감한다. 
                        twork = tabsc;	
                        Calc_NormalRC(tabsc);
                    }
                    if( t_com09 < (WORD)FULLCHGHV*D_SERIES ){ 
                        f_fullchg=OFF;
                    }
                    if( old_SOC/10 != uSOC/10 ){ //대기상태에서 SOC가 업데이트 될때 마다. DataFlash를 업데이트 한다. 
                        old_SOC = uSOC;
                    DF_Update_Set();
			}
		} 
		else 
		{       //방전 (Power 감지) 
			//Calc_CPVolt();					// Calculate CP_x voltage
			twork = tabsc;					// Make data for RC integration
                    Calc_NormalRC(twork);
                        //DPrintf(twork,10,0 );Printf(","); 
		}
        
       }

    Make_RC();									// Calculate RemainingCapacity

    if(uSOC <= D_LOW_SOC){
        f_low_soc = ON;
    }else{
        f_low_soc = OFF;
    }										// Calculate RemainingCapacity
}

///*""FUNC COMMENT""***************************************************
//* ID			: 1.0
//* module outline	: CycleCount() calculation function
//*-------------------------------------------------------------------
//* Include		: 
//*-------------------------------------------------------------------
//* Declaration	: void Calc_CC(void)
//*-------------------------------------------------------------------
//* Function		: Calculate CycleCount()
//*-------------------------------------------------------------------
//* Argument		: None
//*-------------------------------------------------------------------
//* Return		: None
//*-------------------------------------------------------------------
//* Input			: None
//* Output		: t_com17	: CycleCount()
//*-------------------------------------------------------------------
//* 			: CC_Count(), MEDF_Count()
//* Used function 	: 
//*-------------------------------------------------------------------
//* Caution		: 
//*-------------------------------------------------------------------
//* History		: 2012.08.01 Ver 0.01
//* 			: New create
//* 
//*""FUNC COMMENT END""**********************************************/
void Calc_CC(void)
{
	WORD	twork;

	if( D_CCPER < 0 )						// CCCC counts discharging ?
	{
		if( t_com0a < 0 )					// Now discharging ?
		{
			if( t_com0fc < toldrc )				// RC reduces ?
			{
				twork = toldrc - t_com0fc;
				CC_Count(twork);			// Counting CycleCount
				MEDF_Count(twork);			// Counting MaxError/DF
			}
		} 
		else
		{							// Not discharging
			if( t_com0fc > toldrc )				// RC increasing ?
			{
				MEDF_Count(t_com0fc-toldrc);		// Counting MaxError/DF
			}
		}
	} 
	else
	{								// CC counts charging
		if( t_com0a > 0 )					// Now charging ?
		{
			if( t_com0fc > toldrc )				// RC increasing ?
			{
				twork = t_com0fc - toldrc;
				CC_Count(twork);			// Counting CycleCount
				MEDF_Count(twork);			// Counting MaxError/DF
			}
		} 
		else
		{							// Not charging 
			if( t_com0fc < toldrc )				// RC reducing ?
			{
				MEDF_Count(toldrc-t_com0fc);		// Counting MaxError/DF
			}
		}
	}
	
	toldrc = t_com0fc;						   // Update last RC
}

void Calc_FCC_P(void)
{
	// t_com10p = FCC[mAH] * DesignVoltage
	//			= FCC[mAH] * DesignVoltage[mV] / 10000
	t_com10p = (WORD)((long)t_com10c * D_COM19 / 10000);
}


void MEDF_Count(WORD tdata)
{
	tdfup_cnt += tdata;						// Add DF update counter
	if( tdfup_cnt >= 200 )						// reached 500mAh ?
	{
		DF_Update_Set();					// Set DataFlash update req.
		tdfup_cnt = 0;						// Clear the counter
	}
}

void CC_Count(WORD tdata)
{
	tcc_cnt += tdata;						// Add CC counter
	if( tcc_cnt >= tccup )						// reached CC count value ?
	{
		t_com17++;						// Increment CycleCount
		trek_cnt++;						// Increment degradation cnt
		tcc_cnt = 0;						// Clear the counter
	}
}
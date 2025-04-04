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
* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/*""FILE COMMENT""*******************************************************
* System Name	: RAJ240090 Sample Code
* File Name		: cell.h
* Version		: 1.00
* Contents		: 
* Programmer		: Yasuo Usuda
* Note			: 
*""FILE COMMENT END""**********************************************************/

#ifndef _CELL_H
#define _CELL_H

#ifdef	_CELL
#define		GLOBAL
#else
#define		GLOBAL	extern
#endif	/* _CELL */

typedef struct
{
	BYTE f_cvol	    :1;		// bit  0 : CellVoltage ( minV, maxV )
	BYTE f_cur        :1;		// bit  1 : Current 
	BYTE f_tmp    	:1;		// bit  2 : temperature ( min temp, max temp )
	BYTE DUMMY3	:1;		// bit  3 : Resreved
	BYTE DUMMY4		:1;		// bit  4 : Resreved
	BYTE DUMMY5		:1;		// bit  5 : Resreved
	BYTE DUMMY6		:1;		// bit  6 : Resreved
	BYTE DUMMY7		:1;		// bit  7 : Resreved
} _ST_SIMFLG_BIT ;

typedef union
{
    BYTE byte;
     _ST_SIMFLG_BIT b;
}_ST_SIMFLG;

// - Control flag definition -
typedef struct
{
	BYTE f_2ndthe	:1;		// bit  0 : 2nd Th using flag
	BYTE f_cben		:1;		// bit  1 : Cell balancing enable
	BYTE f_calib1p	:1;		// bit  2 : Calibration 1 point flag
	BYTE f_sysconen	:1;		// bit  3 : SYS-connect using flag
	BYTE DUMMY4		:1;		// bit  4 : Resreved
	BYTE DUMMY5		:1;		// bit  5 : Resreved
	BYTE DUMMY6		:1;		// bit  6 : Resreved
	BYTE DUMMY7		:1;		// bit  7 : Resreved
	BYTE DUMMY8		:1;		// bit  8 : Resreved
	BYTE DUMMY9		:1;		// bit  9 : Resreved
	BYTE f_imbchk       	:1;		        // bit 10 : HUNATE, Cell imbalance Protection Check
	BYTE f_bmschk	:1;		        // bit 11 : HUNATE, BMS Diag Protection Check
	BYTE f_cvolchk   	:1;		        // bit 12 : HUNATE, Cell Voltage Protection Check
	BYTE f_curchk   	:1;		        // bit 13 : HUNATE, Current Protection Check
	BYTE f_tmpchk	:1;		        // bit 14 : HUNATE, Temperature Protection Check
	BYTE f_comchk	        :1;		// bit 15 : HUNATE,  SMBUS COM CHK
} _ST_CTLFLG1_BIT ;
// - Control flag definition -
typedef union
{
	WORD w;
	 _ST_CTLFLG1_BIT b;
}_ST_CTLFLG1;



typedef struct
{
	BYTE f_dspt		:1;		// bit  0 : Discharge relearning point
							//         (1=CP_H, 0=CP_L)
	BYTE f_csdis	:1;		// bit  1 : Disable charge relearning
							//         (1=Disable, 0=Enable)
	BYTE f_dsdis	:1;		// bit  2 : Disable discharge relearning
							//         (1=Disable, 0=Enable)
	BYTE f_cpldrop	:1;		// bit  3 : Correct RC flag
							//          (1=Use,0=Not use)
	BYTE f_fccup	:1;		// bit  4 : Fcc update point flag
							//          (0=when detect CP)
							//          (1=when detect charging)
//	BYTE f_rclmt	:1;		// bit  5 : RemainingCapacity limit flag
//							//          (1=RC never become RC > FCC,0=RC>FCC)
	BYTE DUMMY5		:1;		// bit  5 : Resreved
	BYTE DUMMY6		:1;		// bit  6 : Resreved
	BYTE DUMMY7		:1;		// bit  7 : Resreved
	BYTE DUMMY8		:1;		// bit  8 : Resreved
	BYTE DUMMY9		:1;		// bit  9 : Resreved
	BYTE DUMMY10	:1;		// bit 10 :	Resreved
	BYTE DUMMY11	:1;		// bit 11 : Resreved
	BYTE DUMMY12	:1;		// bit 12 : Resreved
	BYTE DUMMY13	:1;		// bit 13 : Reserved
	//BYTE f_2ndthe	:1;		// bit 14 : 2nd Th using flag
	//BYTE f_3rdthe	:1;		// bit 15 : 3rd Th using flag
	BYTE DUMMY14	:1;		// bit 12 : Resreved
	BYTE DUMMY15	:1;		// bit 13 : Reserved
							//         (On: Use, Off: Not use)
}_ST_CTLFLG2_BIT;

typedef union
{
	WORD w;
	 _ST_CTLFLG2_BIT b;
}_ST_CTLFLG2;

// - FlashROM fixed data definition -
typedef struct
{
	// Voltage
	WORD	d_NORMAL_CV;		// 0xD800 [2byte] Charging voltage
								// 0xD801
	WORD	d_NORMAL_CC;		// 0xD802 [2byte] Charging current
								// 0xD803
	WORD	d_CHGWAIT_CC;		// 0xD804 [2byte] Charging current at Charge Wait
								// 0xD805
	WORD	d_FULLCHGHV;		// 0xD806 [2byte] Fullcharge detection voltage
								// 0xD807
	WORD	d_FULLCHG_TV;		// 0xD808 [2byte] Fullcharge taper voltage
								// 0xD809
	WORD	d_FULLCHG_CURR;		// 0xD80A [2byte] Fullcharge judgement current
								// 0xD80B 
	WORD	d_FULLCHG_T;		// 0xD80C [2byte] Fullcharge judgement time
								// 0xD80D
	WORD	d_DISSTOPV;			// 0xD80E [2byte] Discharge stop voltage
								// 0xD80F
	WORD	d_PDVOLT;			// 0xD810 [2byte] Power Down voltage
								// 0xD811
	WORD	d_0PVOLT;			// 0xD812 [2byte] 0% voltage
								// 0xD813
	// Current
	WORD	d_DCDET;			// 0xD814 [2byte] Discharge detection current
								// 0xD815
	WORD	d_CCDET;			// 0xD816 [2byte] Charge detection current
								// 0xD817
	WORD	d_SLPCURR;			// 0xD818 [2byte] Sleep Current
								// 0xD819
	WORD	d_MSKCURR;			// 0xD81A [2byte] Mask Current
								// 0xD81B
	// Temperature
	// Discharge over temp
	_SBYTE	d_DOTH;				// 0xD81C [1byte] Discharge over temperature (H)
	_SBYTE	d_DOTL;				// 0xD81D [1byte] Discharge over temperature (L)
	_SBYTE	d_DTH_hys;			// 0xD81E [1byte] Discharge over temperature release hysteresis
	BYTE	d_DOTJT;			        // 0xD81F [1byte] Discharge Over temperature Judgment time
	// Charge over temp
	_SBYTE	d_COTH;				// 0xD820 [1byte] Charge Over temperature (H)
	_SBYTE	d_COTL;				// 0xD821 [1byte] Charge Over temperature (L)
	_SBYTE	d_CTH_hys;			// 0xD822 [1byte] Charge Over temperature release hysteresis
	_SBYTE	d_COTJT;			// 0xD823 [1byte] Charge Over temperature Judgment time
	// Charge over temp
	BYTE	d_2NDTHM;			// 0xD824 [1byte] 2nd Thermistor over temperature
	BYTE	d_2NDTHM_hys;		// 0xD825 [1byte] 2nd Thermistor release temperature
	
	
	WORD	d_THERM_TBL[8];		// 0xD826 [16byte] Thermistor table
								//    |
								// 0xD835
	WORD	d_ttempidx[8];		// 0xD836 [16byte] Thermistor characteristic temperature index table
								//    |
								// 0xD845
	WORD	d_2NDTHERM_TBL[8];	// 0xD846 [16byte] 2nd Thermistor table
								//    |
								// 0xD855
	
	WORD	d_2ndttempidx[8];	// 0xD856 [16byte] 2nd Thermistor characteristic temperature index table
								//    |
								// 0xD865
	
	// Device info
	BYTE	d_SERIES;			// 0xD866 [1byte] CELL series numbers
	BYTE	d_rsv_03;			// 0xD867 [1byte] Reserved(Padding)
	BYTE	d_MF_NAME[32];		// 0xD868	// [32byte] 0x20:ManufacutereName
								//    |		//
								// 0xD887	//
	BYTE	d_Dev_NAME[32];		// 0xD888	// [32byte] 0x21:DeviceName
								//    |		//
								// 0xD8A7	//			
	BYTE	d_Dev_chem[4];		// 0xD8A8	// [4byte] 0x22:DeviceChemistry
								// 0xD8A9	//
								// 0xD8AA	//
								// 0xD8AB	//
	BYTE	d_SMB_addr;			// 0xD8AC [1byte] SMBus slave address
	BYTE	d_rsv_04;			// 0xD8AD [1byte] Reserved(Padding)
	_ST_CTLFLG1	d_CTLFLG1;		// 0xD8AE	// [2byte] Control flag 1
								// 0xD8AF	//
	
	// Capacity
	BYTE	d_INIT_RSOC;		// 0xD8B0 [1byte] Initial RelativeStateOfCharge
	BYTE	d_LOW_SOC  ;			// 0xD8B1 [1byte] LOW SOC
	WORD	d_rsv_05a;			// 0xD8B2 [2byte] Reserved
								// 0xD8B3
	WORD	d_rsv_05b;			// 0xD8B4 [2byte] Reserved
								// 0xD8B5
	BYTE	d_rsv_05c;			// 0xD8B6 [1byte] Reserved
	BYTE	d_rsv_06;			// 0xD8B7 [1byte] Reserved(Padding)
	
	BYTE	d_rsv_06a[6];		// 0xD8B8	// [6byte] Reserved
								// 0xD8B9	//
								// 0xD8BA	//
								// 0xD8BB	//
								// 0xD8BC	//
								// 0xD8BD	//
	WORD	d_rsv_06b[6];		// 0xD8BE	// [12byte] Reserved
								//    |		//
								// 0xD8C9	//
	WORD	d_CBONV;			// 0xD8CA [2byte] Cell balancing ON voltage
								// 0xD8CB
	WORD	d_CBOFFV;			// 0xD8CC [2byte] Cell balancing OFF voltage
								// 0xD8CD
	WORD	d_CBENVH;			// 0xD8CE [2byte] Cell balancing enable voltage upper limit
								// 0xD8CF
	WORD	d_CBENVL;			// 0xD8D0 [2byte] Cell balancing enable voltage lower limit
								// 0xD8D1
	// Protection 
	WORD	d_OVERCH1;			// 0xD8D2 [2byte] Charge over current 1
								// 0xD8D3
	BYTE        d_OCC1JT;			// 0xD8D4 [1byte] Charge over current 1 judgement time
	BYTE        d_rsv_07;			// 0xD8D5 [1byte] Reserved(Padding)
	WORD      d_OVERCH2;			// 0xD8D6 [2byte] Charge over current 2
								// 0xD8D7
	BYTE        d_OCC2JT;			// 0xD8D8 [1byte] Charge over current 2 judgement time
	BYTE        d_rsv_08;			// 0xD8D9 [1byte] Reserved(Padding)
	WORD      d_OVERDI1;			// 0xD8DA [2byte] Discharge over current 1
								// 0xD8DB
	BYTE        d_ODC1JT;			// 0xD8DC [1byte] Discharge over current 1 judgement time
	BYTE        d_rsv_09;			// 0xD8DD [1byte] Reserved(Padding)
	WORD      d_OVERDI2;			// 0xD8DE [2byte] Discharge over current 2
								// 0xD8DF
	BYTE        d_ODC2JT;			// 0xD8E0 [1byte] Discharge over current 2 judgement time
	BYTE        d_rsv_0A;			// 0xD8E1 [1byte] Reserved(Padding)
	
	WORD      d_CHGPV;			// 0xD8E2	// [2byte] Charge protection voltage
								// 0xD8E3	//
	BYTE        d_CHGPVJT;			// 0xD8E4	// [1byte] Charge protection voltage judgment time
	
	//BYTE	d_rsv_27;			// 0xD8E5	// [1byte] Reserved
	_SBYTE      d_CCPER;			// 0xD8E5	// [1byte] CycleCount counting percent
	WORD        d_COM18P;			// 0xD8E6	// [2byte]      DesignCapacity [10mWh]
								// 0xD8E7	//	
	WORD        d_COM18C;			// 0xD8E8	// [2byte] 0x18:DesignCapacity [mAh]
								// 0xD8E9 	//
	WORD        d_COM19;			// 0xD8EA	// [2byte] 0x19:DesignVoltage [mV]
								// 0xD8EB
	BYTE        d_rsv_27[20];		// 0xD8EC	// [1byte] Reserved	
								//   |
								// 0xD8FF 	//
	
	WORD        d_RLSTPT;			// 0xD900	// [2byte] Relearn stop time [hour]
								// 0xD901	//
	_ST_CTLFLG2 d_CTLFLG2;		// 0xD902	// [2byte] Control flag 2
								// 0xD903	//
	BYTE        d_CP_H;				// 0xD904	// [1byte] Correction point(H) [%]
	BYTE        d_CP_L;				// 0xD905	// [1byte] Correction point(L) [%]
	WORD        d_rsv_27a;			// 0xD906	// [2byte] Reserved
								// 0xD907	//
	BYTE        d_RCCMIN;			// 0xD908	// [1byte] RemCap counting minimum current(mA)
	BYTE        d_DLOGC;			// 0xD909	// [1byte] Consumption current of discharge current
		
	BYTE        d_rsv_28[6];		// 0xD90A
								//   |
								// 0xD900F
							
	WORD        CPH_tbl[20];		// 0xD910	// [40byte] Correction point High Voltage table
								//    |		//
								// 0xD937	//
	WORD        CPL_tbl[20];		// 0xD938	// [40byte] Correction point Low Voltage table
								//    |		//
								// 0xD95F	//
	
	BYTE        d_CPHCOE;			// 0xD960	// [1byte] CPH Coefficient
	BYTE        d_CPLCOE;			// 0xD961	// [1byte] CPL Coefficient
	
	BYTE        d_CRATE[4];			// 0xD962	// [4byte] CP table C-rate [0.01C]
								// 0xD963	//
								// 0xD964	//
								// 0xD965	//
	_SBYTE      d_TRATE[5];			// 0xD966	// [5byte] CP table T-rate [-128??]
								// 0xD967	//
								// 0xD968	//
								// 0xD969	//
								// 0xD96A	//
	
	BYTE        d_rsv_29;			// 0xD96B	// reserve
	
	BYTE        d_rsv_30[18];		// 0xD96C	// [18byte] reserve
								//    |		//
								// 0xD97D	//

	WORD        INREG_cycle[6];		// 0xD97E	// [6byte] cycle count for Internal impedance table
								//    |		//
								// 0xD989	//
	BYTE        INREG_tbl[6];		// 0xD98A	// [6byte] Internal impedance table	
								//    |		//
								// 0xD98F	//	
	
	WORD        d_PCC;		                // 0xD990 [2byte] Permanent Charge Current [mA]
								// 0xD991 	//
	WORD       d_SDC ;		                // 0xD992  [2byte] Short Current [mA]
								// 0xD993 	//
	WORD        d_PFVOLT;		        // 0xD994 //[2byte] PermanentFail Voltage
								// 0xD995	//
						
	WORD        d_FULLV;			// 0xD996	// [2byte] Full charge judgment voltage [mV]
								// 0xD997	//
	WORD        d_RCLMT;			// 0xD998	// [2byte] FCC variance limit
								// 0xD999
	BYTE        d_CVP_TV;			// 0xD99A	// [1byte]  Cell Voltage Protection Tapper Voltage
	_ST_SIMFLG        d_simulFlg;			// 0xD99B	// [1byte] 데이터값을 Debugger에서 설정하면서 테스트 
	BYTE        d_REKKA;			// 0xD99C	// [1byte] Deterioration capacity [10uA]
	_SBYTE      d_STUDYT;			// 0xD99D	// [1byte] Relearning temp. limit [-128??]
	
	BYTE        d_MFD[30];			// 0xD99E	// [32byte] ManufacturerData
								// |
								// 0xD9BB	
        WORD        d_CHGPV2;                   // 0xD9BC  // [2byte] 	Charge Protection Voltage2					
        WORD        d_DISPV;                       // 0xD9BE // [2byte] Discharge Protection voltage
        BYTE        d_DISPVJT;                      // 0xD9C0 // [1byte] Discharge Proection voltage judgment time
	BYTE        d_PDVJT;			        // 0xD9C1   // [1byte] Power Down Judgement time
	WORD     d_EOL;                             //0xD9C2        [42byte]   EOL COUNT - 250 cycle / SOH 60%
	WORD      d_rsv_hunate[20];            //0xD9C4   [40byte]  reserve
	WORD      d_ChkSum;                      // 0xD9EC [2byte] CheckSum;
}_ST_FIXED_DATA;

#pragma address u_fd = 0x00D800
//#pragma address u_fd = 0x0F1C00
GLOBAL _ST_FIXED_DATA __far u_fd;
GLOBAL _ST_FIXED_DATA  u_fd_ram;

// Voltage,current,temperature
#define    NORMAL_CV            u_fd_ram.d_NORMAL_CV	// [2byte] Charging voltage
#define    NORMAL_CC            u_fd_ram.d_NORMAL_CC	// [2byte] Charging current
#define    CHGWAIT_CC          u_fd_ram.d_CHGWAIT_CC	// [2byte] Charging current at Charge Wait
#define    FULLCHGHV             u_fd_ram.d_FULLCHGHV	// [2byte] Fullcharge detection voltage
#define    FULLCHG_TV           u_fd_ram.d_FULLCHG_TV	// [2byte] Fullcharge taper voltage
#define    FULLCHG_CURR	u_fd_ram.d_FULLCHG_CURR // [2byte] Fullcharge judgement current
#define    FULLCHG_T             u_fd_ram.d_FULLCHG_T	// [2byte] Fullcharge judgement time
#define    DISSTOPV               u_fd_ram.d_DISSTOPV		// [2byte] Discharge stop voltage
#define    D_PDVOLT		u_fd_ram.d_PDVOLT		// [2byte] Power Down voltage
#define    D_0PVOLT		u_fd_ram.d_0PVOLT		// [2byte] 0% voltage
#define    D_DCDET			u_fd_ram.d_DCDET		// [2byte] Discharge detection current
#define    D_CCDET			u_fd_ram.d_CCDET		// [2byte] Charge detection current
#define    D_SLPCURR		u_fd_ram.d_SLPCURR		// [2byte] Sleep Current
#define    D_MSKCURR		u_fd_ram.d_MSKCURR		// [2byte] Mask Current
#define    DOTH			u_fd_ram.d_DOTH			// [1byte] Discharge over temperature (H)
#define    DOTL			u_fd_ram.d_DOTL			// [1byte] Discharge over temperature (L)
#define    DTH_hys			u_fd_ram.d_DTH_hys		// [1byte] Discharge over temperature release hysteresis
#define    COTH			u_fd_ram.d_COTH			// [1byte] Charge over temperature (H)
#define    COTL			u_fd_ram.d_COTL			// [1byte] Charge over temperature (L)
#define    CTH_hys			u_fd_ram.d_CTH_hys		// [1byte] Charge over temperature release hysteresis
#define    D_2NDTHM		u_fd_ram.d_2NDTHM		// [1byte] 2nd Thermistor over temperature
#define    D_2NDTHM_hys	u_fd_ram.d_2NDTHM_hys	// [1byte] 2nd Thermistor release temperature
#define    THERM_TBL		u_fd_ram.d_THERM_TBL	// [16byte] Thermistor table
#define    ttempidx		        u_fd_ram.d_ttempidx		// [16byte] Thermistor characteristic temperature index table
#define    D_2NDTHERM_TBL	u_fd_ram.d_2NDTHERM_TBL	// [16byte] 2nd Thermistor table
#define    D_2ndttempidx		u_fd_ram.d_2ndttempidx// [16byte] 2nd Thermistor characteristic temperature index table

// Device info
#define    D_SERIES		u_fd_ram.d_SERIES				// [1byte] CELL series numbers
#define    MF_NAME			u_fd_ram.d_MF_NAME				// [32byte] ManufacutereName
#define    Dev_NAME		u_fd_ram.d_Dev_NAME				// [32byte] DeviceName
#define    Dev_chem		u_fd_ram.d_Dev_chem				// [4byte]  DeviceChemistry
#define    SMB_addr		u_fd_ram.d_SMB_addr				// [1byte] SMBus slave address

#define    CONFLG			u_fd_ram.d_CTLFLG1				// [2byte] Control flag 1
#define    F_2NDTHE		u_fd_ram.d_CTLFLG1.b.f_2ndthe		// bit 0
#define    F_CBEN			u_fd_ram.d_CTLFLG1.b.f_cben		// bit 1
#define    F_CALIB1P		u_fd_ram.d_CTLFLG1.b.f_calib1p	// bit 2
#define    F_SYSCONEN		u_fd_ram.d_CTLFLG1.b.f_sysconen	// bit 4

// Capacity
#define    INIT_RSOC		u_fd_ram.d_INIT_RSOC			// [1byte] Initial RelativeStateOfCharge

// Protection
#define    D_CBONV			u_fd_ram.d_CBONV				// [2byte] Cell balancing ON voltage
#define    D_CBOFFV		u_fd_ram.d_CBOFFV				// [2byte] Cell balancing OFF voltage
#define    D_CBENVH		u_fd_ram.d_CBENVH				// [2byte] Cell balancing enable voltage upper limit
#define    D_CBENVL		u_fd_ram.d_CBENVL				// [2byte] Cell balancing enable voltage lower limit

#define    OVERCH1			u_fd_ram.d_OVERCH1				// [2byte] Charge over current 1
#define    OCC1JT			u_fd_ram.d_OCC1JT				// [1byte] Charge over current 1 judgement time
#define    OVERCH2			u_fd_ram.d_OVERCH2				// [2byte] Charge over current 2
#define    OCC2JT			u_fd_ram.d_OCC2JT				// [1byte] Charge over current 2 judgement time

#define    OVERDI1			u_fd_ram.d_OVERDI1				// [2byte] Discharge over current 1
#define    ODC1JT			u_fd_ram.d_ODC1JT				// [1byte] Discharge over current 1 judgement time
#define    OVERDI2			u_fd_ram.d_OVERDI2				// [2byte] Discharge over current 2
#define    ODC2JT			u_fd_ram.d_ODC2JT				// [1byte] Discharge over current 2 judgement time
#define    CHGPV			u_fd_ram.d_CHGPV				// [2byte] Charge protection voltage
#define    CHGPVJT			u_fd_ram.d_CHGPVJT				// [1byte] Charge protection voltage judgment time

#define    D_CCPER			u_fd_ram.d_CCPER				// [1byte] CycleCount counting percent
#define    D_COM18P		u_fd_ram.d_COM18P				// [2byte]      DesignCapacity [10mWh]
#define    D_COM18C		u_fd_ram.d_COM18C				// [2byte] 0x18:DesignCapacity [mAh]
#define    D_COM19			u_fd_ram.d_COM19				// [2byte] 0x19:DesignVoltage [mV]

#define    D_RCCMIN		u_fd_ram.d_RCCMIN				// [1byte] RemCap counting minimum current(mA)
#define    D_DLOGC			u_fd_ram.d_DLOGC				// [1byte] Consumption current of discharge current
	
#define    D_RLSTPT		u_fd_ram.d_RLSTPT				// [2byte] Relearn stop time [hour]
		
#define    D_TCLFLG2		u_fd_ram.d_CTLFLG2				// [2byte] Control flag 2
#define    F_DSPT			u_fd_ram.d_CTLFLG2.b.f_dspt			// bit 0
#define    F_CSDIS			u_fd_ram.d_CTLFLG2.b.f_csdis			// bit 1
#define    F_DSDIS			u_fd_ram.d_CTLFLG2.b.f_dsdis			// bit 2
#define    F_CPLDROP		u_fd_ram.d_CTLFLG2.b.f_cpldrop		// bit 3
#define    F_FCCUP			u_fd_ram.d_CTLFLG2.b.f_fccup			// bit 4
//#define F_RCLMT			u_fd.d_CTLFLG2.f_rclmt			// bit 5
//#define F_3RDTHE		u_fd.d_CTLFLG2.f_3rdthe			// bit 15

#define    D_CP_H			u_fd_ram.d_CP_H					// [1byte] Correction point(H) [%]
#define    D_CP_L			u_fd_ram.d_CP_L					// [1byte] Correction point(L) [%]

#define    CPH_TBL			u_fd_ram.CPH_tbl				// [40byte] Correction point High Voltage table
#define    CPL_TBL			u_fd_ram.CPL_tbl				// [40byte] Correction point Low Voltage table
#define    INREG_CYCLE		u_fd_ram.INREG_cycle			// [6byte] Internal impedance table
#define    INREG_TBL		u_fd_ram.INREG_tbl				// [6byte] Internal impedance table

#define    D_FULLV			u_fd_ram.d_FULLV				// [2byte] Full charge judgment voltage [mV]

#define    D_RCLMT			u_fd_ram.d_RCLMT				// [2byte] FCC variance limit

#define    D_REKKA			u_fd_ram.d_REKKA				// [1byte] Deterioration capacity [10uA]

#define    D_CPHCOE		u_fd_ram.d_CPHCOE				// [1byte] CPH Coefficient
#define    D_CPLCOE		u_fd_ram.d_CPLCOE				// [1byte] CPL Coefficient
#define    D_CRATE			u_fd_ram.d_CRATE				// [4byte] CP table C-rate [0.1C]
#define    D_TRATE			u_fd_ram.d_TRATE				// [5byte] CP table T-rate [-128??]
#define    D_STUDYT		u_fd_ram.d_STUDYT				// [1byte] Relearning temp. limit [-128??]
#define    D_COM23			u_fd_ram.d_MFD					// [32byte] ManufacturerData
/***********************************************************************
HUNATE define
***********************************************************************/
#define    SOCLUT_ROWSIZE          5
#define    SOCLUT_COLSIZE           21                
#define    D_LOW_SOC               u_fd_ram.d_LOW_SOC
#define    D_PFVOLT                     u_fd_ram.d_PFVOLT
#define    D_SOCTBL                     u_fd_ram.d_SOCTBL
#define    D_DIPV                          u_fd_ram.d_DISPV
#define    D_DIPVJT                     u_fd_ram.d_DISPVJT
#define    D_PDVJT                      u_fd_ram.d_PDVJT
#define    D_DOTJT                      u_fd_ram.d_DOTJT
#define    D_COTJT                      u_fd_ram.d_COTJT
#define    D_SDC                          u_fd_ram.d_SDC
#define    D_PCC                          u_fd_ram.d_PCC
#define    D_EOL                          u_fd_ram.d_EOL
#define    D_CVP_TV                      u_fd_ram.d_CVP_TV
#define    D_CHGPV2                      u_fd_ram.d_CHGPV2
#define F_CtlComChk		u_fd_ram.d_CTLFLG1.b.f_comchk		// bit 15
#define    F_CtlTmpChk		u_fd_ram.d_CTLFLG1.b.f_tmpchk	                // bit 14
#define    F_CtlCurChk		u_fd_ram.d_CTLFLG1.b.f_curchk		                // bit 13
#define    F_CtlCVChk		        u_fd_ram.d_CTLFLG1.b.f_cvolchk		    // bit 12
#define    F_CtlImbChk              u_fd_ram.d_CTLFLG1.b.f_imbchk          //bit 10

#define F_SimCvol                  u_fd_ram.d_simulFlg.b.f_cvol
#define F_SimCur                    u_fd_ram.d_simulFlg.b.f_cur
#define F_SimTmp                  u_fd_ram.d_simulFlg.b.f_tmp


/***********************************************************************
Global functions
***********************************************************************/
GLOBAL BYTE InitFixed_Chk(void);								// Check process on fixed data

#undef		GLOBAL

#endif

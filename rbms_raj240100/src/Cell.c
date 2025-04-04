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
* File Name		: cell.c
* Contents		: cell function source file
* Device		: RAJ240090
* Compiler		: CC-RL (V.4.01)
* Note			: 
*""FILE COMMENT END""**********************************************************/

#define _CELL

/***********************************************************************
** Include files
***********************************************************************/
#include "define.h"								// union/define definition
#include "bms.h"
#include "cell.h"								// cell.c
#include "dataflash.h"
#include <string.h>
#include "build.h"



#pragma address build_info = 0x2000
const _BUILD_INFO build_info={
     _DATE_,
    0x0,
    _TIME_,
    0x0,
    {'v','0'+SW_MAJOR,'0'+SW_MINOR1,'0'+SW_MINOR2},
};            


const _ST_FLASH_CUSTOM  default_FlshData={
    4150 ,               // 0xF1C00 [2byte] Fullcharge Judgement Cell voltage
    4200 ,               // 0xF1C02 [2byte] Over Protection Cell Voltage
    4150 ,              // 0xF1C04 [2byte] Over Warning Cell Voltage
    3000 ,             // 0xF1C06 [2byte] Fulldischarge Judgement Cell Voltage
    2650,              // 0xF1C08 [2byte] Power Down Cell Voltage
    2800,              //  0xF1C0A [2byte] Under Protection Cell Voltage
    3000,              // 0xF1C0C [2byte] Under Warning Cell Voltage
    5810,                 // 0xF1C0E [2byte] Charge Stop Pack Voltage       [10mv]
    0xFF ,              // 0xF1C10 [2byte]  Charge Request Pack Voltage [10mv] ==> UPS
    4200 ,               // 0xF1C12 [2byte] Discharge Stop Pack Voltage   [10mv]
    4150 ,               // 0xF1C14 [2byte]  Over Voltage Protection Release Cell Voltaga ==> Trigger 된후 release 판단
    3000 ,                // 0xF1C16 [2byte]  Under Voltage Protection Release Cell Voltaga ==> Trigger 된후 release 판단   
    4100 ,                    // 0xF1C18  [2byte]  Over Voltage Warning Release Cell Votage
    3100 ,                   // 0xF1C18  [2byte]  Under Voltage Warning Release Cell Voltage
    40 ,                       // 0xF1C0C [2byte]   Mask Current 0A  
    100,                        // 0xF1C1E  [2byte]  Discharge detection current
    100 ,                        // 0xF1C20  [2byte]  Charge detection current
    1680 ,                         // 0xF1C22  [2byte]  FullCharge Judgement Current
    11074,                      //  0xF1C24  [2byte]  Charge Limit Current
    (DWORD)11000UL ,      //  0xF191A   [4byte]      Charge over current 1 --> Charge Limited Current
    (DWORD)30000UL ,      // 0xF191E    [4byte]      Charge over current 2  --> Charge Protection Current
    (DWORD)80000UL ,     //  0xF1922   [4byte]     Discharge over current 1 --> Dishcarge Limited Current
    (DWORD)95000UL ,    //   0xF1926   [4byte]    Discharge over current 2  --> Discharge Protection Current
    (DWORD)70000UL ,      //  0xF1C36  [4byte]  OEM Discharge Limited Current 1
    (DWORD)35000UL ,      //  0xF1C3A  [4byte]  OEM Discharge Limited Current 2  ( SOC 17%, TEMP 50)
    (DWORD)20000UL ,     //  0xF1C3E  [4byte]  OEM Discharge Limited Current 3   ( SOC 7%, TEMP 55) 
    0xffff,                    // 0xF1C42  [2byte]  
    0xffff,                    // 0xF1C44  [2byte]  
    65,                      // 0xF1C46  [1byte]  Discharge over temperature (H)
    (char)-20,                      // 0xF1C47  [1byte]  Discharge over temperature (L)
    5,                      // 0xF1C48 [1byte]   Discharge over temperature release hysteresis
    55,                           // 0xF1C49 [1bye]     Charge over temperature (H)
    (char)0,                            // 0xF1C4A  [1byte]    Charge over temperature (L)
    5,                           // 0xF1C4B    [1byte]   Charge over temperature release hysteresis
    0xffff,                    // 0xF1C4C  [2byte]  
    0xffff,                    // 0xF1C4E  [2byte]  
    8,                            //  0xF1C50  [1byte]  Over Charge Current Protection Reaction Time [0.25s] 
    120,                            //  0xF1C51  [1byte]  Over Charge Current Protection Release   Time [0.25s] 
    8,                           //  0xF1C52  [1byte]  Over DisCharge Current Protection Reaction Time [0.25s]  
    120,                              //  0xF1C53  [1byte]  Over DisCharge Current Protection Release Time [0.25s]
    8,                           //  0xF1C54  [1byte]  Over Cell Protection Voltage Reaction Time [0.25s]  
    0xFF,                            //  0xF1C55  [1byte]  Over Cell Protection Voltage Release Time [0.25s] 
    8,                            //  0xF1C56  [1byte]  Under Cell Protection Voltage Reaction Time [0.25s] 
    0xFF,                             //  0xF1C57  [1byte]  Under Cell Protection Voltage Release Time [0.25s]
    8,                             //  0xF1C58  [1byte]  Over Temperature protection Reaction Time [0.25s]
    0xFF,                             //  0xF1C59  [1byte]  Over Temperature protection Release Time [0.25s]
    8,                             //  0xF1C5A  [1byte]  Under Temperature protection Reaction Time [0.25s]
    0xFF,                             //  0xF1C5B  [1byte]  Under Temperature protection Release Time [0.25s]
    7,                           // 0xF1C5C   [1byte]   Series number of Cell  on Master BMIC
    7,                          // 0xF1C5D    [1byte]   Series number of Cell on 2ND BMIC  
    0x7F03, //0x6802,                   // 0xF1C5E   [2byte] Control flag 1 
    33600 ,                              // 0xF1C60   [2byte]    Initial FullChargeCapacity [10mA]
    0xFFFF,                     //0xF1C62 [2byte]
    2 ,                            // 0xF1C64    [1byte]   Internal Temperaure Install Number
    3 ,                         // 0xF1C65    [1byte]    External Temperature Install Number
    0,      5,     10,   15,    20,    25 ,   30,    35,      40,    45,   50,    55,    60,    65,    70,    75,     80,     85,    90,    95,  100,    0,                    // 0xF1C64    [22byte]     Initial capacity table 
    3000, 3123, 3272, 3395, 3452, 3499 ,3554, 3603,  3642, 3676, 3718, 3763,3813, 3877, 3912, 3953, 4012, 4063, 4080, 4096, 4150,    0,                   // 0xF1C7A    [22*2byte]   Initial voltage table
    10 ,                               //  0xF1CA8    [2byte]    Cell balancing ON voltage
    5 ,                              //  0xF1CAA    [2byte]    Cell balancing OFF voltage
    4080 ,                              // 0xF1CAC    [2byte]     Cell balancing enable voltage(H)
    3272 ,                              // 0xF1CAE   [2byte]     Cell balancing enable voltage(L) 
    0xFF,                         // 0xF1CB0   [2byte]
    "CURO" ,                   // 0xF1CB2 [22byte]  Manufacture Name
    "ESC100" ,                        // 0xF1CC8  [32byte]  Manufacture Model
    "v123" ,                        // 0xF1CE8  [4byte]   FirmWare Version "ver1.2.3" -->"v123"
    "12345678" ,                //  0xF1CEC    [8byte]    Custom Part Number
    "EVT" ,                        //  0xF1CF4     [4byte]     Build Revision
    "" ,                    //  0xF1CF8    [6byte]     Manufacture Date
    0xFF,                              // 0xF1CFE   [2byte]  reserved
    0xFF ,                             // 0xF1D00   [2byte]  reserved
    100 ,                              // 0xF1D02   [1byte]     End of Life (0~100)
    100 ,                             //   0xF1D03   [1byte]     UART Communication Timeout
    0,                                 //  0xF1D4   [4byte]    1 Cycle Count increase when SOHmAh = FCC*2
    0,                                 //  0xF1D08     [2byte]      cycle count
    120,                              // 0xF1D0A   [1byte]     OCV WAIT Minute TIME
    0,0,0,                            // 0xF1D0B   [3byte]     reserved
    4250,                              //0xF1D0E      [2byte]    Over  Permanent FAIL Cell Voltage
    2000,                            //0xF1D10      [2byte]     Under Permanent FAIL Cell Voltage
    34000,                           //0xF1D12       [4byte]    Over Charge Permenant FAIL Current
    100000,                          //0xF1D16       [4byte]    Over Discharge Permenant FAIL Current
    60,                                //0xF1D1A        [1byte]     Over Charge Permenant FAIL Temperature
    -5,                                //0xF1D1B        [1byte]     Under Charger Permenant FAIL Temperauture
    68,                               //0xF1D1C       [1byte]     Over Discharge Permenant FAIL Temperature
    -25,                              //0xF1D1D       [1byte]     Under Discharge Permenant FAIL Temperature
       0,                               //0xF1D1E       [2byte]    Data CheckSUM
};

#if 0
#pragma address default_fixData = 0x0D800

const _ST_FIXED_DATA default_fixData ={
	4150,		// 0xD800 [2byte] Charging voltage
				// 0xD801
	5500,		// 0xD802 [2byte] Charging current - Limit ?? ??? ?? ???. 
				// 0xD803
	200,        // 0xD804 [2byte] Charging current at Charge Wait
				// 0xD805
	4050,		// 0xD806 [2byte] Fullcharge detection voltage - Wakeup? Full ???? ??.
	                        // ?? CELL ?? X D_SERIES ? ?? PACK???? ????.
				// 0xD807
	25,	        // 0xD808 [2byte] Fullcharge taper voltage
				// 0xD809
	400,        // 0xD80A [2byte] Fullcharge judgement current
				// 0xD80B 
	8,	        // 0xD80C [2byte] Fullcharge judgement time
				// 0xD80D
	2750,		// 0xD80E [2byte] Discharge stop voltage  // CUT-OFF ??? 2.65->2.75? ?? 
				// 0xD80F
	2500,		// 0xD810 [2byte] Power Down voltage
				// 0xD811
	2750,		// 0xD812 [2byte] 0% voltage
				// 0xD813
	// Current
	100,			// 0xD814 [2byte] Discharge detection current
				// 0xD815
	100,			// 0xD816 [2byte] Charge detection current
				// 0xD817
	50,			// 0xD818 [2byte] Sleep Current
				// 0xD819
	50,			// 0xD81A [2byte] Mask Current
				// 0xD81B
	// Temperature
	// Discharge over temp
	60,	       	// 0xD81C [1byte] Discharge over temperature (H)
	-20,		// 0xD81D [1byte] Discharge over temperature (L)
	10,			// 0xD81E [1byte] Discharge over temperature release hysteresis
	12,			// 0xD81F [1byte] Discharge Over temperature Judgment time
	// Charge over temp
	55,			// 0xD820 [1byte] Charge Over temperature (H)
	-5,			// 0xD821 [1byte] Charge Over temperature (L)
	5,			// 0xD822 [1byte] Charge Over temperature release hysteresis
	12,			// 0xD823 [1byte]  Charge Over temperature Judgment time
	// Charge over temp
	80,	        // 0xD824 [1byte] 2nd Thermistor over temperature
	10,	        // 0xD825 [1byte] 2nd Thermistor release temperature
	
	
	0,0,0,0,0,0,0,0,  // 0xD826 [16byte] Thermistor table
				      //    |
				      // 0xD835
	0,0,0,0,0,0,0,0,  // 0xD836 [16byte] Thermistor characteristic temperature index table
				      //    |
				      // 0xD845
	0,0,0,0,0,0,0,0,  // 0xD846 [16byte] 2nd Thermistor table
				      //    |
				      // 0xD855
	
	0,0,0,0,0,0,0,0,  // 0xD856 [16byte] 2nd Thermistor characteristic temperature index table
				      //    |
				      // 0xD865
	
	// Device info
	4,			// 0xD866 [1byte] CELL series numbers
	0,			// 0xD867 [1byte] Reserved(Padding)
	"renesas",	// 0xD868	// [32byte] 0x20:ManufacutereName
				//    |		//
				// 0xD887	//
	"raj240045",// 0xD888	// [32byte] 0x21:DeviceName
			    //    |		//
				// 0xD8A7	//			
	1,2,3,4,	// 0xD8A8	// [4byte] 0x22:DeviceChemistry
				// 0xD8A9	//
				// 0xD8AA	//
				// 0xD8AB	//
	0x16,		// 0xD8AC [1byte] SMBus slave address
	0xFF,		// 0xD8AD [1byte] Reserved(Padding)
	(WORD)0xFC0A,	// 0xD8AE	// [2byte] Control flag 1
		//    |||||||+- 8bit : 0
		//    ||||||+-- 9bit : 0
		//    |||||+--- 10bit : 1  //Cell imbalance Protection Check
		//    ||||+---- 11bit : 1  // BMS Diag Protection Check
		//    |||+----- 12bit : 1  // Cell Voltage Protection Check
		//    ||+------ 13bit : 1  //Current Protection Check
		//    |+------- 14bit : 1   //Temperature Protection Check
		//    +-------- 15bit : 1  //SMBUS COM CHK
				// 0xD8AF	//
	
	// Capacity
	50,		    // 0xD8B0 [1byte] Initial RelativeStateOfCharge
	5,			// 0xD8B1 [1byte] Low SOC 
	0,		    // 0xD8B2[2byte] Reserved
				// 0xD8B3
	0,	        // 0xD8B4 [2byte] Reserved
				// 0xD8B5
	0,			// 0xD8B6 [1byte] Reserved
	0,			// 0xD8B7 [1byte] Reserved(Padding)
	
	0,0,0,0,0,0,// 0xD8B8	// [6byte] Reserved
				// 0xD8B9	//
				// 0xD8BA	//
				// 0xD8BB	//
				// 0xD8BC	//
				// 0xD8BD	//
	0,0,0,0,0,0,// 0xD8BE	// [12byte] Reserved
				//    |		//
				// 0xD8C9	//
	30,			// 0xD8CA [2byte] Cell balancing ON voltage
				// 0xD8CB
	10,			// 0xD8CC [2byte] Cell balancing OFF voltage
				// 0xD8CD
	4140,		// 0xD8CE [2byte] Cell balancing enable voltage upper limit
				// 0xD8CF
	3500,	    // 0xD8D0 [2byte] Cell balancing enable voltage lower limit
				// 0xD8D1
	// Protection 
	7000,		// 0xD8D2 [2byte] Charge over current 1
				// 0xD8D3
	16,			// 0xD8D4 [1byte] Charge over current 1 judgement time
	0,			// 0xD8D5 [1byte] Reserved(Padding)
	9000,		// 0xD8D6 [2byte] Charge over current 2
				// 0xD8D7
	8,			// 0xD8D8 [1byte] Charge over current 2 judgement time
	0,			// 0xD8D9 [1byte] Reserved(Padding)
	11000,		// 0xD8DA [2byte] Discharge over current 1
				// 0xD8DB
	16,			// 0xD8DC [1byte] Discharge over current 1 judgement time
	0,			// 0xD8DD [1byte] Reserved(Padding)
	13000,		// 0xD8DE [2byte] Discharge over current 2
				// 0xD8DF
	8,			// 0xD8E0 [1byte] Discharge over current 2 judgement time
	0,			// 0xD8E1 [1byte] Reserved(Padding)
	
	4200,	        // 0xD8E2	[2byte] Charge protection voltage
				// 0xD8E3	//
	8,			// 0xD8E4	 [1byte] Charge protection voltage judgment time
	
	100,	        // 0xD8E5	// [1byte] CycleCount Counting percent
        2880,        // 0xD8E6  [2byte]    DesignCapacity [10mWh]
                // 0xD8E7
        8000,       // 0xD8E8  [2byte]  0x18:DesignCapacity [mAh]
                // 0xD8E9 [1byte] resserved (Padding)
        3600,       //  0xD8EA	// [2byte] 0x19:DesignVoltage [mV]
                // 0xD8EB
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,// 0xD8EC	// [1byte] Reserved	
				//   |
				// 0xD8FF 	//
	
	1,			// 0xD900	// [2byte] Relearn stop time [hour]
				// 0xD901	//
	(WORD)0,	      	// 0xD902	// [2byte] Control flag 2
				// 0xD903	//
	10,			// 0xD904	// [1byte] Correction point(H) [%]
	5,	       	// 0xD905	// [1byte] Correction point(L) [%]
	0, 			// 0xD906	// [2byte] Reserved
				// 0xD907	//
	100,		// 0xD908	// [1byte] RemCap counting minimum current(mA)
	100,		// 0xD909	// [1byte] Consumption current of discharge current
        0,0,0,0,0,0,// 0xD90A
				//   |
			    // 0xD900F
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0xD910	// [40byte] Correction point High Voltage table
								 //    |		//
								 // 0xD937	//
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0xD938	// [40byte] Correction point Low Voltage table
								 //    |		//
								 // 0xD95F	//
	
	7,                    	// 0xD960	// [1byte] CPH Coefficient
	6,			// 0xD961	// [1byte] CPL Coefficient
	10,20,50,80,// 0xD962	// [4byte] CP table C-rate [0.01C]
				// 0xD963	//
				// 0xD964	//
				// 0xD965	//
	0,10,25,45,60,// 0xD966	// [5byte] CP table T-rate [-128??]
				// 0xD967	//
				// 0xD968	//
				// 0xD969	//
				// 0xD96A	//
	
	0,			// 0xD96B	// reserve
	
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 0xD96C	// [18byte] reserve
								//    |		//
								// 0xD97D	//

	50,100,150,200,250,300,	// 0xD97E	// [6byte] cycle count for Internal impedance table
						//    |		//
					        // 0xD989	//
	20,30,50,80,100,150,		// 0xD98A	// [6byte] Internal impedance table	
						//    |		//
					        // 0xD98F	//	
	
	24000,                 // 0xD990 [2byte] Permanent Charging Current [mA]
				  // 0xD991 	//
	25000,		   // 0xD992  [2byte] Short Current [mA] - H/W ??? 25A ? ?? ?? ? AD? 3266
			         // 0xD993 	//
	1700,                   // 0xD994  //[2byte] PermanentFail Voltage
						
	4125,		 // 0xD996	// [2byte] Full charge judgment voltage [mV]
		                 // 0xD997	//
	200,		         // 0xD998	// [2byte] FCC variance limit
				 // 0xD999
	100,			 // 0xD99A	//[1byte]  Cell Voltage Protection Tapper Voltage
	0x0, //0x07,			 // 0xD99B	// [1byte] ????? Debugger?? ????? ??? 
	10,			 // 0xD99C	// [1byte] Deterioration capacity [10uA]
	10,			 // 0xD99D	// [1byte] Relearning temp. limit [-128??]
	
	"0123",             // 0xD99E	// [30byte] ManufacturerData
                                 // |
				 // 0xD9BB	
	4300,                 // 0xD9BC // [2byte] 	Charge Protection Voltage2			
        2600,                 // 0xD9BE // [2byte] Discharge Protection voltage
        8,                      // 0xD9C0 // [1byte] Discharge Proection voltage judgment time
	4,			 // 0xD9C1  [1ybte] Power Down voltage judgment time
	250,                    // 0xD9C2 [2byte] EOL - 250 count / SOH 60%
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0xD9C4 [40byte] reserve
	0,                      // 0xD9EC [2byte] CheckSum;
};
/************************************************************************************************************
 *                              SOC LOOK-UP TABLE
 * 
 ************************************************************************************************************/
const _SBYTE SOC_TMP[SOCLUT_ROWSIZE]=
{
    -10,
    0,
    10,
    30,
    45,
};

const WORD SOC_TABLE[SOCLUT_ROWSIZE][SOCLUT_COLSIZE]=
{
//      0%     5%    10%    15%    20%    25%    30%    35%    40%    45%    50%    55%    60%    65%   70%   75%   80%   85%   90%   95%  100%    
//-10?
    3113,3367,3430,3470,3524,3573,3602,3627,3655,3690,3731,3776,3831,3877,3910,3946,3996,4044,4071,4086,4127,
//0?
    3077,3297,3405,3456,3506,3563,3590,3610,3654,3691,3735,3786,3841,3884,3921,3966,4023,4066,4080,4100,4164,
//10?
    2919,3195,3350,3432,3479,3540,3585,3616,3646,3682,3727,3784,3840,3886,3922,3970,4029,4069,4085,4112,4174,
//30?
    2818,3004,3293,3400,3445,3504,3560,3599,3630,3662,3700,3748,3813,3866,3901,3938,3991,4046,4076,4094,4143,
//45?
    2657,3101,3285,3396,3457,3509,3563,3604,3644,3680,3725,3775,3838,3886,3920,3966,4027,4071,4084,4109,4189,
};


#endif


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: Fixed Data Check Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		:  BYTE InitFixed_Chk(void)
 *--------------------------------------------------------------------------
 * Function			: Check whether there are some fixed data or not
 *					: Even if there is just one vacant data, FALSE is returned.
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: TRUE=There are all necessary data.
 *					: FALSE=There are some vacant data.
 *--------------------------------------------------------------------------
 * Input			: None
 *					: 
 * Output			: None
 *					: 
 *--------------------------------------------------------------------------
 * Used function	: 
 *					: 
 *--------------------------------------------------------------------------
 * Caution			: 
 *--------------------------------------------------------------------------
 * History			: 2016.02.15 Ver 1.00
 *					:  New Create
 *					: 
 *""FUNC COMMENT END""*****************************************************/
BYTE InitFixed_Chk(void)
{
#if 0    
    BYTE	al1;
    WORD p=0;
    int i;
    DFLEN = 1;																		// Data flash access enable
    Waittimer_us(10);																// wait 10us
    memcpy( &u_fo_ram_ext, &u_fo_rom_ext, sizeof(u_fo_rom_ext));
    DFLEN = 0;																		// Data flash access disable

	// Voltage,Current,Temperature
	if( NORMAL_CV == 0xFFFF )								// NORMAL_CV is vacant ?
	{
		return FALSE;
	}
	
	if( NORMAL_CC == 0xFFFF )								// NORMAL_CC is vacant ?
	{
		return FALSE;
	}
	
	if( CHGWAIT_CC == 0xFFFF )								// CHGWAIT_CC is vacant ?
	{
		return FALSE;
	}
	
	if( FULLCHGHV == 0xFFFF )								// FULLCHGHV is vacant ?
	{
		return FALSE;
	}
	
	if( FULLCHG_TV == 0xFFFF )								// FULLCHG_TV is vacant ?
	{
		return FALSE;
	}
	
	if( FULLCHG_CURR == 0xFFFF )							// FULLCHG_CURR is vacant ?
	{
		return FALSE;
	}
	
	if( FULLCHG_T == 0xFFFF )								// FULLCHG_T is vacant ?
	{
		return FALSE;
	}
	
	if( DISSTOPV == 0xFFFF )								// DISSTOPV is vacant ?
	{
		return FALSE;
	}
	
	if( D_PDVOLT == 0xFFFF )								// D_PDVOLT is vacant ?
	{
		return FALSE;
	}
	
	if( D_0PVOLT == 0xFFFF )								// D_0PVOLT is vacant ?
	{
		return FALSE;
	}
	
	if( D_DCDET == 0xFFFF )									// D_DCDET is vacant ?
	{
		return FALSE;
	}
	
	if( D_CCDET == 0xFFFF )									// D_CCDET is vacant ?
	{
		return FALSE;
	}
	
	if( D_SLPCURR == 0xFFFF )								// D_SLPCURR is vacant ?
	{
		return FALSE;
	}
	
	if( D_MSKCURR == 0xFFFF )								// D_MSKCURR is vacant ?
	{
		return FALSE;
	}
	
	if( DOTH == 0xFF )										// DOTH is vacant ?
	{
		return FALSE;
	}
	
	if( DOTL == 0xFF )										// DOTH is vacant ?
	{
		return FALSE;
	}
	
	if( DTH_hys == 0xFF )									// DTH_hys is vacant ?
	{
		return FALSE;
	}
	
	if( COTH == 0xFF )										// COTH is vacant ?
	{
		return FALSE;
	}
	
	if( COTL == 0xFF )										// COTH is vacant ?
	{
		return FALSE;
	}
	
	if( CTH_hys == 0xFF )									// CTH_hys is vacant ?
	{
		return FALSE;
	}
	
	if( D_2NDTHM == 0xFF )									// D_2NDTHM is vacant ?
	{
		return FALSE;
	}
	
	if( D_2NDTHM_hys == 0xFF )								// D_2NDTHM_hys is vacant ?
	{
		return FALSE;
	}
	
	for(al1 = 0; al1 < sizeof(THERM_TBL)/sizeof(THERM_TBL[0]); al1++)			// THERM_TBL are vacant ?
	{
	if(~THERM_TBL[al1] == 0)
		{
			return FALSE;
		}
	}
	
	for(al1 = 0; al1 < sizeof(ttempidx)/sizeof(ttempidx[0]); al1++)				// ttempidx are vacant ?
	{
	if(~ttempidx[al1] == 0)
		{
			return FALSE;
		}
	}
	
	for(al1 = 0; al1 < sizeof(D_2NDTHERM_TBL)/sizeof(D_2NDTHERM_TBL[0]); al1++)		// D_2NDTHERM_TBL are vacant ?
	{
	if(~D_2NDTHERM_TBL[al1] == 0)
		{
			return FALSE;
		}
	}
	
	for(al1 = 0; al1 < sizeof(D_2ndttempidx)/sizeof(D_2ndttempidx[0]); al1++)		// D_2ndttempidx are vacant ?
	{
	if(~D_2ndttempidx[al1] == 0)
		{
			return FALSE;
		}
	}
	
	// Device info
	if( D_SERIES == 0xFF )									// D_SERIES value is vacant ?
	{
		return FALSE;
	}
	
	for(al1 = 0; al1 < sizeof(MF_NAME); al1++)				// MF_NAME are vacant ?
	{
		if(MF_NAME[al1] == 0xFF)
		{
			return FALSE;
		}
	}
	
	for(al1 = 0; al1 < sizeof(Dev_NAME); al1++)				// Dev_NAME are vacant ?
	{
		if(Dev_NAME[al1] == 0xFF)
		{
			return FALSE;
		}
	}
	
	for(al1 = 0; al1 < sizeof(Dev_chem); al1++)				// Dev_chem are vacant ?
	{
		if(Dev_chem[al1] == 0xFF)
		{
			return FALSE;
		}
	}
	
	if( SMB_addr == 0xFF)									// SMB_addr value is vacant ?
	{
		return FALSE;
	}

	// Since All control flag sets 1 CONFLG data is 0xFFFF. Skip CONFLG data check.    
//	if( memcmp((__near _ST_CTLFLG1 *)&tmp, (__near _ST_CTLFLG1 *)&CONFLG, sizeof(_ST_CTLFLG1)) != 0)		// CONFLG value is vacant ?
//	{
//		return FALSE;
//	}
	
	
	// Capacity
	if( INIT_RSOC == 0xFF )									// INIT_RSOC is vacant ?
	{
		return FALSE;
	}
	
	// Protection
	if( D_CBONV == 0xFFFF )									// D_CBONV is vacant ?
	{
		return FALSE;
	}
	
	if( D_CBOFFV == 0xFFFF )								// D_CBOFFV is vacant ?
	{
		return FALSE;
	}
	
	if( D_CBENVH == 0xFFFF )								// D_CBENVH is vacant ?
	{
		return FALSE;
	}
	
	if( D_CBENVL == 0xFFFF )								// D_CBENVL is vacant ?
	{
		return FALSE;
	}
	
	if( OVERCH1 == 0xFFFF )									// OVERCH1 is vacant ?
	{
		return FALSE;
	}
	
	if( OCC1JT == 0xFF )									// OCC1JT is vacant ?
	{
		return FALSE;
	}
	
	if( OVERCH2 == 0xFFFF )									// OVERCH2 is vacant ?
	{
		return FALSE;
	}
	
	if( OCC2JT == 0xFF )									// OCC2JT is vacant ?
	{
		return FALSE;
	}

	if( OVERDI1 == 0xFFFF )									// OVERDI1 is vacant ?
	{
		return FALSE;
	}
	
	if( ODC1JT == 0xFF )									// ODC1JT is vacant ?
	{
		return FALSE;
	}
	
	if( OVERDI2 == 0xFFFF )									// OVERDI2 is vacant ?
	{
		return FALSE;
	}
	
	if( ODC2JT == 0xFF )									// ODC2JT is vacant ?
	{
		return FALSE;
	}
	
	if( CHGPV == 0xFFFF )									// CHGPV is vacant ?
	{
		return FALSE;
	}
	
	if( CHGPVJT == 0xFF )									// CHGPVJT is vacant ?
	{
		return FALSE;
	}
	
	if( D_CCPER == 0xFF )									// D_CCPER is vacant ?
	{
		return FALSE;
	}	
	
	if( D_COM18P == 0xFFFF )								// D_COM18P is vacant ?
	{
		return FALSE;
	}
	
	if( D_COM18C == 0xFFFF )								// D_COM18C is vacant ?
	{
		return FALSE;
	}
	
	if( D_COM19 == 0xFFFF )									// D_COM19 is vacant ?
	{
		return FALSE;
	}
	
	if( D_RLSTPT == 0xFFFF )								// D_RLSTPT is vacant ?
	{
		return FALSE;
	}

	if( D_CP_H == 0xFF )									// D_CP_H is vacant ?
	{
		return FALSE;
	}	
		
	if( D_CP_L == 0xFF )									// D_CP_L is vacant ?
	{
		return FALSE;
	}
	
	if( D_RCCMIN == 0xFF )									// D_RCCMIN is vacant ?
	{
		return FALSE;
	}	
		
	if( D_DLOGC == 0xFF )									// D_DLOGC is vacant ?
	{
		return FALSE;
	}
	
	for(al1 = 0; al1 < sizeof(CPH_TBL)/sizeof(CPH_TBL[0]); al1++)						// CPH_TBL are vacant ?
	{
		if(~CPH_TBL[al1] == 0)
		{
			return FALSE;
		}
	}
	
	for(al1 = 0; al1 < sizeof(CPL_TBL)/sizeof(CPL_TBL[0]); al1++)						// CPL_TBL are vacant ?
	{
		if(~CPL_TBL[al1] == 0)
		{
			return FALSE;
		}
	}

	if( D_CPHCOE == 0xFF )								// D_CPHCOE is vacant ?
	{
		return FALSE;
	}

	if( D_CPLCOE == 0xFF )								// D_CPLCOE is vacant ?
	{
		return FALSE;
	}

	for(al1 = 0; al1 < sizeof(D_CRATE); al1++)			// D_CRATE are vacant ?
	{
		if(INREG_TBL[al1] == 0xFF)
		{
			return FALSE;
		}
	}	
	
	for(al1 = 0; al1 < sizeof(D_TRATE); al1++)			// D_TRATE are vacant ?
	{
		if(INREG_TBL[al1] == 0xFF)
		{
			return FALSE;
		}
	}	
	
	for(al1 = 0; al1 < sizeof(INREG_CYCLE)/sizeof(INREG_CYCLE[0]); al1++)						// INREG_CYCLE are vacant ?
	{
		if(~INREG_CYCLE[al1] == 0)
		{
			return FALSE;
		}
	}
		
	for(al1 = 0; al1 < sizeof(INREG_TBL); al1++)			// INREG_TBL are vacant ?
	{
		if(INREG_TBL[al1] == 0xFF)
		{
			return FALSE;
		}
	}	
	
	if( D_FULLV == 0xFFFF )									// D_FULLV is vacant ?
	{
		return FALSE;
	}
	
	if( D_RCLMT == 0xFFFF )									// D_RCLMT is vacant ?
	{
		return FALSE;
	}
		
	if( D_REKKA == 0xFF )									// D_REKKA is vacant ?
	{
		return FALSE;
	}
		
	if( D_STUDYT == 0xFF )									// D_STUDYT is vacant ?
	{
		return FALSE;
	}


	for(al1 = 0; al1 < sizeof(D_COM23); al1++)			// D_COM23 are vacant ?
	{
		if(INREG_TBL[al1] == 0xFF)
		{
			return FALSE;
		}
	}	    
    
    
#if 0

    if( D_FULLCHGCV == 0xFFFF){
        return FALSE;
    }
    
    // Voltage,Current,Temperature
	if( NORMAL_CV == 0xFFFF )								// NORMAL_CV is vacant ?
	{
		return FALSE;
	}
	
	if( NORMAL_CC == 0xFFFF )								// NORMAL_CC is vacant ?
	{
		return FALSE;
	}
	
	if( CHGWAIT_CC == 0xFFFF )								// CHGWAIT_CC is vacant ?
	{
		return FALSE;
	}
	
	if( FULLCHGHV == 0xFFFF )								// FULLCHGHV is vacant ?
	{
		return FALSE;
	}
	
	if( FULLCHG_TV == 0xFFFF )								// FULLCHG_TV is vacant ?
	{
		return FALSE;
	}
	
	if( FULLCHG_CURR == 0xFFFF )							// FULLCHG_CURR is vacant ?
	{
		return FALSE;
	}
	
	if( FULLCHG_T == 0xFFFF )								// FULLCHG_T is vacant ?
	{
		return FALSE;
	}
	
	if( DISSTOPV == 0xFFFF )								// DISSTOPV is vacant ?
	{
		return FALSE;
	}
	
	if( D_PDVOLT == 0xFFFF )								// D_PDVOLT is vacant ?
	{
		return FALSE;
	}
	
	if( D_0PVOLT == 0xFFFF )								// D_0PVOLT is vacant ?
	{
		return FALSE;
	}
	
	if( D_DCDET == 0xFFFF )									// D_DCDET is vacant ?
	{
		return FALSE;
	}
	
	if( D_CCDET == 0xFFFF )									// D_CCDET is vacant ?
	{
		return FALSE;
	}
	
	if( D_SLPCURR == 0xFFFF )								// D_SLPCURR is vacant ?
	{
		return FALSE;
	}
	
	if( D_MSKCURR == 0xFFFF )								// D_MSKCURR is vacant ?
	{
		return FALSE;
	}
	
	if( DOTH == 0xFF )										// DOTH is vacant ?
	{
		return FALSE;
	}
	
	if( DOTL == 0xFF )										// DOTH is vacant ?
	{
		return FALSE;
	}
	
	if( DTH_hys == 0xFF )									// DTH_hys is vacant ?
	{
		return FALSE;
	}
	
	if( COTH == 0xFF )										// COTH is vacant ?
	{
		return FALSE;
	}
	
	if( COTL == 0xFF )										// COTH is vacant ?
	{
		return FALSE;
	}
	
	if( CTH_hys == 0xFF )									// CTH_hys is vacant ?
	{
		return FALSE;
	}
	
	if( D_2NDTHM == 0xFF )									// D_2NDTHM is vacant ?
	{
		return FALSE;
	}
	
	if( D_2NDTHM_hys == 0xFF )								// D_2NDTHM_hys is vacant ?
	{
		return FALSE;
	}
    
    for(i=0;i<sizeof(u_fo_rom_ext)-2;i++){
       p +=*((BYTE *)&u_fo_ram_ext+i);
    }
    if( u_fo_ram_ext.d_ChkSum != p){
        return FALSE;
    }
 
   //BUILD DATE 비교
    if( strncmp( build_info.date , u_fo_ram_ext.d_MF_DATE, 6) != 0){
        for(i=0;i<6;i++){
            u_fo_ram_ext.d_MF_DATE[i] = build_info.date[i];
        }
        FlashEraseFixedBlock();
        FlashWrite_FixedBlock();
    }
 
    /*

   
    if( D_OPCV== 0xFFFF){
        return FALSE;
    }
    
    if( D_FULLDISCV == 0xFFFF){
        return FALSE;
    }

    if( D_CHGSTOPPV == 0xFFFF){
        return FALSE;
    }

    if( D_DISTOPV == 0xFFFF){
        return FALSE;
    }

    if( D_PDNCV == 0xFFFF){
        return FALSE;
    }

    if( D_SERIES == 0xFF){
        return FALSE;
    }
    
    
    if( DOTH == 0xFF || DOTL == 0xFF || DTH_hys == 0xFF ){
        return FALSE;
    }
    if( COTH == 0xFF || COTL == 0xFF || CTH_hys == 0xFF ){
        return FALSE;
    }
    if( D_SERIES== 0xFF || D_SERIES_2ND == 0xFF){
        return FALSE;
    }
    */
    
#endif


#endif
    return TRUE;											// there are All necessary data
	
}

void RestoreFixedData(void)
{
#if 0
    memcpy( &u_fo_ram_ext, &default_FlshData, sizeof(default_FlshData));

    FlashEraseFixedBlock();
    FlashWrite_FixedBlock();
#endif    
}

void FlashWrite_FixedBlock(void)
{
#if 0    
    int i;
    WORD p=0;   
    for(i=0;i< (sizeof(u_fo_ram_ext)-2); i++){
        p  += *((BYTE *)&u_fo_ram_ext+i);
    }
    u_fo_ram_ext.d_ChkSum = p;
    
    FlashWrite( (BYTE *)&u_fo_rom_ext, (BYTE *)&u_fo_ram_ext, sizeof(u_fo_ram_ext) );      
    // Write data of 4bytes at DataFlash area
    
    DFLEN = 1;                                                                      // Data flash access enable
    Waittimer_us(10);                                                               // wait 10us
    memcpy( (BYTE *)&u_fo_ram_ext,(BYTE *) &u_fo_rom_ext, sizeof(u_fo_rom_ext) );                               // DataFlash own data -> RAM copy
    DFLEN = 0;                                                                      // Data flash access disable
#endif    
}


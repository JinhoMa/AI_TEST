/******************************************************************************
* DISCLAIMER																	*
* This software is supplied by Renesas Electronics Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.
*
* This software is owned by Renesas Electronics Corp. and is protected under 
* all applicable laws, including copyright laws.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.	ALL SUCH WARRANTIES ARE EXPRESSLY 
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
* File Name		: ram.h
* Version		: 1.00
* Contents		: 
* Programmer	: Yasuo Usuda
* Note			: 
************************************************************************
* Copyright(C) 2016 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History			: 2016.02.15 Ver 1.00
 *					:  New Create
 *					: 
*""FILE COMMENT END""**************************************************/
#ifndef _RAM_H
#define _RAM_H


#ifdef	_RAM
#define			GLOBAL
#else
#define			GLOBAL	extern
#endif	/* _RAM */

/***********************************************************************
Global Config Define ( HUNATE )
***********************************************************************/

GLOBAL WORD t_com01c;							// 0x01:RemainingCapacityAlarm() mAh
GLOBAL WORD t_com01p;							// 0x01:RemainingCapacityAlarm() 10mwh
GLOBAL WORD t_com02;							// 0x02:remainingtimealarm()	

GLOBAL WORD				t_com03;				// 0x03:BatteryMode()
#define f_capmode		DEFS_BIT15(&t_com03)	// CAPACITY_MODE
#define f_relearn		DEFS_BIT7(&t_com03)		// RELEARN_FLAG (unknownbit,same as r1.12)

GLOBAL short			t_com0b;			// 0x0B:AverageCurrent()
GLOBAL WORD			t_com0c;			// 0x0C:MaxError()
GLOBAL WORD			t_com0e;				// 0x0E:AbsoluteStateOfCharge()
GLOBAL WORD			t_com13;				// 0x13:AverageTimeToFull()


GLOBAL short			t_com04c;				// 0x04:AtRate() mAh
GLOBAL short			t_com04p;				// 0x04:AtRate() 10mWh
GLOBAL WORD				t_com05;				// 0x05:AtRateTimeToFull()
GLOBAL WORD				t_com06;				// 0x06:AtRateTimeToEmpty()
GLOBAL WORD				t_com07;				// 0x07:AtRateOK()

GLOBAL WORD				t_com0fp;				// 0x0F:RemainingCapacity() 10mWh

GLOBAL WORD				t_com11c;				// 0x11:RunTimeToEmpty() mAh
GLOBAL WORD				t_com11p;				// 0x11:RunTimeToEmpty() 10mWh
GLOBAL WORD				t_com12c;				// 0x12:AverageTimeToEmpty() mAh
GLOBAL WORD				t_com12p;				// 0x12:AverageTimeToEmpty() 10mWh

GLOBAL long			lrccr_w;			// Charge relearn capacity
GLOBAL long			lrcdr_w;			// Discharge relearn capacity
GLOBAL WORD			tcom17_w;

GLOBAL WORD				t_com0fc_h;				// 0x0F:RemainingCapacity() mAh

void DPrintf(long long value,int base, int align);

GLOBAL signed char       InTemp[2];
GLOBAL signed char       minTemp;
GLOBAL signed char       maxTemp;
GLOBAL long long old_rctime;
GLOBAL WORD cvt_ms;
GLOBAL long long tick_timer;
GLOBAL long long smbustick_timer;
GLOBAL WORD       fixblk_chksum;
GLOBAL WORD        ocv_count_1sec;      // OCV measuring time count
GLOBAL WORD         longkey_1msec;          // LONG KEY Event time count
GLOBAL     BYTE       ocv_watch;

GLOBAL BYTE             ucv_p_cnt ; //Under Cell Voltage Protection Count
GLOBAL BYTE             ocv_p_cnt  ; //Over Cell Voltage Protection Count
GLOBAL BYTE             ucv_pd_cnt ;  // Under Cell Voltage Power Down Count
GLOBAL BYTE             ucv_pf_cnt;
GLOBAL BYTE                octp_cnt;               // Over Charge Temperature count
GLOBAL BYTE                uctp_cnt;               // Under Charge Temperature count
GLOBAL BYTE                odtp_cnt;               // Over Discarge Temperature count
GLOBAL BYTE                udtp_cnt;               // Under Disharge Temperature count
GLOBAL  _SHORT2CHAR	_tfc_cnt;					// Full Charge check count
GLOBAL BYTE		alow_cnt;						// Discharge stop count
GLOBAL BYTE		afcdc_cnt;						// Discharge detection count(FC)
GLOBAL BYTE		aocc_cnt1,aocc_cnt2,aocc_cnt3;			// OverChgCurr count
GLOBAL  BYTE		aodc_cnt1,aodc_cnt2;			// OverDisCurr count
GLOBAL BYTE		aoccrel_cnt;					// OverChgCurr release count
GLOBAL BYTE		aodcrel_cnt;					// OverDisCurr release count
GLOBAL BYTE		ascrel_cnt;						// ShortCurr release count
GLOBAL BYTE            cvimb_pf_cnt;        //Cell Voltage Imbalance Permanent Fail count
GLOBAL BYTE       swrst_cnt;        // SW Reset Count
GLOBAL BYTE       pwdn_cnt;        // Power Down Count

void DF_Update_Set(void);



typedef struct{
    WORD dS;    //?? ?? 
    WORD soc1;   //START SOC
    WORD soc2;   // END SOC
    BYTE   sign;
    BYTE   pad;
} _DFSOHDAT;

typedef struct
{
	//rename df_t_com10c -> df_t_com10p
	//WORD		df_t_com10c;			// 0x10:FullChargeCapacity() mAh
	WORD		df_t_com10p;			// 0x10:FullChargeCapacity() 10mWh
	WORD		df_t_com17;				// 0x17:CycleCount()
	long		df_lrc_w;				// RC correction capacity
	WORD		df_tcc_cnt;				// CycleCount RC counter
	WORD		df_trek_cnt;			// Degradation counter
	WORD		df_tpacksts;			// PackStatus
	_SHORT2CHAR	df_t_com16;				// BatteryStatus
	WORD		df_t_com08;				// 0x08:Temperature()
	WORD		df_t_com09;				// 0x09:Voltage()
	short		df_t_com0a;				// 0x0A:Current()
	WORD		df_tminv;				// Minimum cell voltage
	WORD		df_tmaxv;				// Maximum cell voltage
	WORD		df_tdferase_cnt;		// Flexible data erase counter
				
	WORD		Reserved1;				// Reserved
	BYTE		Reserved2;				// Reserved
	BYTE             df_errcode;                        // ERROR CODE <- Permanent Fail ? ?? ERRCODE
	WORD		df_t_com10c_h;			//Full Charge Capacity <- Aging Caculated 
	_SHORT2CHAR	df_tmode;				// 0x41:OperationStatus()
	long          df_lrc_w_hunate;	        //?? ??? ?? ???? ?? 
    	BYTE                 df_sohidx;
        BYTE                 pad;
        WORD               now_soh;
	_DFSOHDAT      df_soh[5];		// reserve 	
}_DFDAT;

#define DFSIZE		sizeof(_DFDAT)				// DataFlash size

typedef union
{
	_DFDAT	sdfdat;						// Flexible data
	BYTE	uc[DFSIZE];					// Byte alignment
}_DF;
GLOBAL _DF	sdf;

#define adfdat			sdf.uc

#define tdferase_cnt	sdf.sdfdat.df_tdferase_cnt // counter that records DataFlash erease times


void DF_Update_Set(void);


GLOBAL WORD				t_com10c;				// 0x10:FullChargeCapacity() mAh
//#define t_com10p		sdf.sdfdat.df_t_com10c 	// 0x10:FullChargeCapacity() 10mWh 
#define t_com10p		sdf.sdfdat.df_t_com10p 	// 0x10:FullChargeCapacity() 10mWh

#define t_com17			sdf.sdfdat.df_t_com17 	// 0x17:CycleCount()
#define tcc_cnt			sdf.sdfdat.df_tcc_cnt 	// counter used to calculate 0x17:CycleCount()
#define trek_cnt		sdf.sdfdat.df_trek_cnt  // derioation counter

//Using Vriable in Library
// SOC,SOH
GLOBAL WORD         t_com10c_h;				// 0x10:FullChargeCapacity() mAh
GLOBAL  WORD old_uSOC;
GLOBAL  long  old_lrc_w_hunate;
GLOBAL  long caps;
GLOBAL WORD	       uSOC;	// User SOC




#define ERRCODE_LV       0xE1          // LOW Voltage PF
#define ERRCODE_OC      0xE2          // OCC PF
#define ERRCODE_CVIMB   0xE3          // CELL IMB PF



GLOBAL BYTE       swrst_cnt;        // SW Reset Count
GLOBAL BYTE       pwdn_cnt;        // Power Down Count

GLOBAL _LONG2CHAR  Bevt;        // Battery Protection Detection Flag
GLOBAL WORD         mcu_cmd;    //MCU Command
GLOBAL BYTE           mcu_event;    // MCU ? ?? Command ? ?????? ?? 

typedef enum{
    MCU_CMD_RECHARGE = 0x02,
    MCU_CMD_HWRESET = 0x03,
    MCU_CMD_SLEEP = 0x4,
    MCU_CMD_PWRDOWN = 0xFF,    
} MCU_CMD;



/***********************************************************************
Global define
***********************************************************************/
// smbus data
GLOBAL _SHORT2CHAR		tcom00;											// 0x00:ex_func
#define t_com00			tcom00.us
#define a_com00			tcom00.uc
#define t_com08			sdf.sdfdat.df_t_com08		// 0x08:Temperature()
#define t_com09			sdf.sdfdat.df_t_com09		// 0x09:Voltage()
#define t_com0a			sdf.sdfdat.df_t_com0a		// 0x0A:Current()
#define f_ampn			DEFS_BIT15(&t_com0a)		// Current direction
GLOBAL WORD				t_com0d;										// 0x0D:RelativeStateOfCharge()
GLOBAL WORD				t_com0fc;										// 0x0F:RemainingCapacity() mAh
GLOBAL WORD				t_com10c;										// 0x10:FullChargeCapacity() mAh
GLOBAL WORD				t_com14;										// 0x14:ChargingCurrent() mA
GLOBAL WORD				t_com15;										// 0x15:ChargingVoltage() mV
GLOBAL _SHORT2CHAR		tcom16;											// 0x16:BatteryStatus
#define t_com16			tcom16.us
#define a_com16			tcom16.uc

#define f_ocv			DEF_BIT4(&a_com16[1])							// OCV Status
#define f_low_soc   		DEF_BIT5(&a_com16[1])       // low SOC Alarm


#define f_term_dis		DEF_BIT3(&a_com16[1])	// TERMINATE_DISCHARGE_ALARM
#define f_rem_cap		DEF_BIT1(&a_com16[1])	// REMAINING_CAPACITY_ALARM
#define f_rem_tim		DEF_BIT0(&a_com16[1])	// REMAINING_TIME_ALARM
#define f_over_tmp		DEF_BIT6(&a_com16[0])							// OVER_TEMP_ALARM
#define f_odc_alarm		DEF_BIT5(&a_com16[0])							// Over Discharge Current Alarm
#define f_occ_alarm		DEF_BIT4(&a_com16[0])							// Over Charge Current Alarm
#define f_short_alarm		DEF_BIT3(&a_com16[0])							// Short Current Alarm
#define f_dischg		DEF_BIT2(&a_com16[0])							// DISCHARGING
#define f_fullchg		DEF_BIT1(&a_com16[0])							// FULLY_CHARGED
#define f_fulldis		DEF_BIT0(&a_com16[0])							// FULLY_DISCHARGED

#define OCV_ST                               f_ocv


GLOBAL WORD				t_com44;	// 0x44:2nd Thermistor()									// 0x44:2nd Thermistor()
#define tpacksts		sdf.sdfdat.df_tpacksts	// 0x45:PackStatus
#define f_dfet			DEFS_BIT0(&tpacksts)		// DFET	1=ON, 0=OFF
#define f_cfet			DEFS_BIT1(&tpacksts)		// CFET	1=ON, 0=OFF
#define f_sys_in		DEFS_BIT2(&tpacksts)		// 	BI	1=Batt-in, 0=Batt-out 
#define f_ccp			DEFS_BIT3(&tpacksts)		// Capacity correction point flag	1=detect, 0=not detect
#define f_rl			DEFS_BIT5(&tpacksts)		// 	RL	1=relearning, 0=not rel.
#define f_cp_h			DEFS_BIT6(&tpacksts)		// 	CPH	1=detect, 0=not detect
#define f_cp_l			DEFS_BIT7(&tpacksts)		// 	CPL	1=detect, 0=not detect
#define f_uvp			DEFS_BIT9(&tpacksts)		// UVP detection flag	


GLOBAL _SHORT2CHAR		tcom46;											// 0x46:CellBalancingStatus
#define t_com46			tcom46.us
#define a_com46			tcom46.uc
GLOBAL WORD				t_com5a;										// 0x5a:Flash update mode
GLOBAL WORD				t_com5d;										// 0x5d:Software Reset()
GLOBAL WORD				t_com5e;										// 0x5e:Flash erase mode

GLOBAL _SHORT2CHAR		tad_curr;
GLOBAL _LONG2CHAR		cc_curr;										// result of current multiplication
#define lcurr_ad		cc_curr.ul

GLOBAL long long old_rctime;


GLOBAL BYTE			amodeflg1;
#define f_over_chg	DEF_BIT0(&amodeflg1)								// OVER_CHARGED_ALARM

// system counter
GLOBAL BYTE				aoffs_cnt;										// OffStateCounter
GLOBAL BYTE				asleep_cnt1,asleep_cnt2;						// Sleep counter


GLOBAL WORD				tvin12v;										// vin12 voltage

#ifdef	TCV_EN
GLOBAL WORD				ttcv;											// Total cell voltage
#endif

GLOBAL BYTE 			afet;											// FET status
GLOBAL BYTE				afet_real;										// FET status register read
GLOBAL BYTE				aafe;											// FET setting
#define f_cfctl			DEF_BIT1(&aafe)									// C-FET
#define f_dfctl			DEF_BIT0(&aafe)									// D-FET


// AD value
GLOBAL WORD				ad_cell[10];									// cell
GLOBAL WORD				ad_div[10];										// Difference
GLOBAL WORD				ad_imp[10];										// Impedance measurement
GLOBAL DWORD			ad_therm;										// thermister
GLOBAL DWORD			ad_therm2;										// thermister2
GLOBAL DWORD			ad_therm3;										// thermister3
GLOBAL WORD				tethad;											// simple thermistor AD value
GLOBAL WORD				tadvregpull;									// VREG pullup AD value (AN0)
GLOBAL WORD				tadvregpull2;									// VREG pullup AD value (AN1)
GLOBAL WORD				ad_pack;
GLOBAL WORD			ad_vin12[2];					// VIN12
// PACK voltage
#ifdef	TCV_EN
GLOBAL WORD				ad_tcv;											// Total Cell Voltage
#endif

#ifdef	MCUBG_EN
GLOBAL WORD				ad_tmcubg;										// MCU BG Voltage
#endif

#ifdef	CREG2_EN
GLOBAL WORD				ad_tcreg2;										// CREG2 Voltage
#endif


//function flags
volatile GLOBAL BYTE f_curr_int;

GLOBAL char afunction_flag_datas;
volatile GLOBAL BYTE f_curr_int;
#define f_boot_jump	DEF_BIT1(&afunction_flag_datas)

volatile GLOBAL BYTE f_adc_int;

/***********************************************************************
   UART Device Memory define
***********************************************************************/
#define UART_BUF_SIZE 128
GLOBAL BYTE     uart_txdata[UART_BUF_SIZE];
GLOBAL BYTE     uart_rxdata[UART_BUF_SIZE];
GLOBAL int     uart_txfront;
GLOBAL int     uart_rxfront;
GLOBAL int     uart_err_count;

void Printf(__far const unsigned char *buf);


/***********************************************************************
   CAN Device Memory define
***********************************************************************/
GLOBAL BYTE canFlag;
#define	canTxEndFlag		DEF_BIT0(&canFlag)							// CAN Send End Flag
#define  canRxEndFlag		DEF_BIT1(&canFlag)							// CAN Receive End Flag
//#define	extFlag			DEF_BIT2(&canFlag)							// External Interrupt Flag
#define  f_nocan				DEF_BIT3(&canFlag)							// No communication flag
#define	canTxrqFlag			DEF_BIT4(&canFlag)							// CAN Send request flag
#define  canerrFlag			DEF_BIT5(&canFlag)
struct CanMsg{
    WORD id;
    BYTE data[8];
    BYTE dlc;
};

GLOBAL struct CanMsg CanTxMsg;
GLOBAL struct CanMsg CanRxMsg;

/***********************************************************************
   Timer Device Memory define
***********************************************************************/




void FuseCutOff_On(void);
void FuseCutOff_Off(void);

GLOBAL BYTE				a_com52;
GLOBAL BYTE				asout1[3],asout2[3];

GLOBAL WORD			flag4mode;					// create a new flag for f_ov_alarm
#define f_ov_alarm	DEF_BIT7(&flag4mode)		// OVER VOLTAGE use bit7  --> Deprecated by Hunate,

GLOBAL WORD			tcom15_w;			// ChargingVoltage par single cell, useless in this FW
GLOBAL WORD			twh_w;				// temp-variable used for 10mWh-calculation, actually it is DesignCapacity *10x%


// Voltage measurement relation
GLOBAL WORD				tvolt[10];										// single cell Voltage
GLOBAL BYTE				av_tbl[10];
GLOBAL WORD				tminv;											// Minimum cell voltage
GLOBAL WORD				tmaxv;											// Maximum cell voltage
// Measurement Result
GLOBAL unsigned short tvt_ad;											// thermistor AD value
GLOBAL unsigned short tadv;												// thermistor voltage
GLOBAL _SBYTE	adegree,adegree2;											// Temperature[Åé]

// Measurement data
GLOBAL WORD				tabsc;											// Current absolute value

GLOBAL _SHORT2CHAR 		tmode;
#define amode	tmode.uc[0]												// Mode number

GLOBAL long		lrc_w;													// Charging learning RC integrated value

GLOBAL long		relcap_c;                       // charging relearn capacity

GLOBAL BYTE		asys_in;												// SYS-connect

GLOBAL 	BYTE  acb_w,acb_w1;												// Cell balance select flag

void SWreset(void);


typedef union {
	WORD	lsum[2];													// Checksum
} _CHECKSUM;
GLOBAL _CHECKSUM	ssum;
#define lsum_main1		ssum.lsum[0]									// FlashROM 1(FW) checksum
#define lsum_main2		ssum.lsum[1]	// FlashROM 2(Fixed data) checksum

#define lrc_w_hunate			sdf.sdfdat.df_lrc_w_hunate	// RC correction capacity, for 0x0F:RemainingCapacity() calculation
#define FCC_ADJ                           sdf.sdfdat.df_t_com10c_h
#define DIAGCODE                        sdf.sdfdat.df_errcode

// - TTI(tolerant timer interval ) Set Flag
typedef struct
{
	BYTE scdet	                :1;		// bit  0 : Short Cicruit time flag
	BYTE odcrel		:1;		// bit  1 : Over Discharge Current release
	BYTE occrel         	:1;		// bit  2 : Over Charge Current release
	BYTE odcdet1	        :1;		// bit  3 : Over Discharge Current1 Detection
	BYTE odcdet2	        :1;		// bit  4 : Over Discharge Current2 Detection
	BYTE occdet1		:1;		// bit  5 : Over Charge Current1 Detection
	BYTE occdet2		:1;		// bit  6 : Over Charge Current2 Detection
	BYTE occdet3		:1;		// bit  7 : Over Charge Current2 Detection
	BYTE udtdet	        :1;		// bit  8 : Under Discharge temperature Detection
	BYTE odtdet             :1;		// bit  9: Over Discharge temperature Detection
	BYTE uctdet		:1;		// bit  10 : Under Charge temperture Detection
	BYTE octdet             :1;		// bit  11 : Over Charge temperature Detection
	BYTE ucvdet	        :1;	         // bit 12 : Under Cell Voltage Detection
	BYTE ocvdet	        :1;		 // bit 13 : Over Cell Voltage Detection
	BYTE ucvpddet   	:1;		 // bit 14 : Under Cell Power Down Voltage Detection
	BYTE ucvpfdet   	:1;		//bit15: Under Cell Permenant Fail Detection 
	BYTE cvimbdet          :1;           // bit16: Cell Voltage Imbalance Detection
	BYTE DUM1            :1;          // bit17: DUM 
	BYTE DUM2            :1;          // bit18: DUM 
	BYTE DUM3            :1;          // bit18: DUM 
	BYTE DUM4            :1;          // bit20: DUM 
	BYTE DUM5            :1;          // bit21: DUM 
	BYTE DUM6            :1;          // bit22: DUM 
	BYTE DUM7            :1;          // bit23: DUM 
	BYTE DUM8            :1;          // bit24: DUM 
	BYTE DUM9            :1;          // bit25: DUM 
	BYTE DUM10            :1;          // bit26: DUM 
	BYTE DUM11            :1;          // bit27: DUM 
	BYTE DUM12            :1;          // bit28: DUM 
	BYTE DUM13            :1;          // bit29: DUM 
	BYTE DUM14            :1;          // bit30: DUM 
	BYTE DUM15            :1;          // bit31: DUM 
} TTI_FLG_BIT ;

typedef union
{
    DWORD l;
    TTI_FLG_BIT b;
} FS_TTI_FLG;

GLOBAL FS_TTI_FLG  TTI;

//FAULT
#define f_ocv_p             DEF_BIT0(&Bevt.uc[0])
#define f_ucv_p             DEF_BIT1(&Bevt.uc[0])
#define f_occ_p             DEF_BIT2(&Bevt.uc[0])
#define f_odc_p             DEF_BIT3(&Bevt.uc[0])
#define f_octp_p            DEF_BIT4(&Bevt.uc[0])
#define f_uctp_p            DEF_BIT5(&Bevt.uc[0])
#define f_odtp_p            DEF_BIT6(&Bevt.uc[0])
#define f_udtp_p           DEF_BIT7(&Bevt.uc[0])

#define f_other_p         DEF_BIT0(&Bevt.uc[1])
#define f_pd_p              DEF_BIT1(&Bevt.uc[1])
#define f_sdc_p            DEF_BIT2(&Bevt.uc[1])

//PF
#define f_ucv_pf              DEF_BIT0(&Bevt.uc[2])
#define f_cvimb_pf          DEF_BIT1(&Bevt.uc[2])
#define f_other_pf           DEF_BIT2(&Bevt.uc[2])
#define f_ocv_pf              DEF_BIT3(&Bevt.uc[2])
#define f_occ_pf              DEF_BIT4(&Bevt.uc[2])
#define f_odc_pf              DEF_BIT5(&Bevt.uc[2])
#define f_otp_pf              DEF_BIT6(&Bevt.uc[2])
#define f_utp_pf              DEF_BIT7(&Bevt.uc[2])

//WARNING
#define f_ocv_w                DEF_BIT0(&Bevt.uc[3])
#define f_ucv_w                DEF_BIT1(&Bevt.uc[3])
#define f_occ_w                DEF_BIT2(&Bevt.uc[3])
#define f_odc_w                DEF_BIT3(&Bevt.uc[3])
#define f_otp_w                DEF_BIT4(&Bevt.uc[3])
#define f_utp_w                DEF_BIT5(&Bevt.uc[3])
#define f_tmpimb_w         DEF_BIT6(&Bevt.uc[3])



#undef	GLOBAL

#endif

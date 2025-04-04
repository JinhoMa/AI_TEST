/******************************************************************************
* DISCLAIMER                                                                    *
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
* File Name		: Dataflash.h
* Version		: 1.00
* Contents		: Flash memory control functions header file
* Programmer		: Yasuo Usuda
* Note			: 
************************************************************************
* Copyright(C) 2016 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History			: 2016.02.15 Ver 1.00
 *					:  New Create
 *					: 
*""FILE COMMENT END""**************************************************/
#ifndef _DATAFLASH_H
#define _DATAFLASH_H


#ifdef	_DATAFLASH
#define		GLOBAL
#else
#define		GLOBAL	extern
#endif	/* _DATAFLASH */


__callt BYTE DF_Write(void);					// Write DataFlash
void DF_Read(void);								// Read DataFlash
__callt void DF_Erase_Confirm(void);			// Confirm erase DataFlash
void DF_Erase_RMA(void);						// Erase DataFlash RMA
void DFW_Index(void);
void DF_IndexW(BYTE adata);
void DF_Erase_A(void);
void DF_Erase_B(void);
__callt void DF_Erase_Confirm(void);
void DFR_Index(void);


GLOBAL BYTE adfflg;								// Flags for DataFlashGLOBAL __far BYTE	*apdfadr;					// DataFlash address pointer
GLOBAL __far BYTE	*apdfadr;					// DataFlash address pointer
GLOBAL BYTE	adfidx;								// DataFlash index

#define f_dfreq			DEF_BIT0(&adfflg)		// DataFlash update request
#define f_dfe			DEF_BIT1(&adfflg)		// DataFlash erasing flag
#define f_dffail		DEF_BIT2(&adfflg)		// DataFlash failure flag
#define f_dfe_areq		DEF_BIT3(&adfflg)		// BlockA erase request flag
#define f_dfe_breq		DEF_BIT4(&adfflg)		// BlockB erase request flag
#define f_dfe_dreq		DEF_BIT5(&adfflg)		// BlockD erase request flag
#define f_dfe_rmareq	DEF_BIT6(&adfflg)		// Block RMA erase request flag
#define f_dfstop		DEF_BIT7(&adfflg)		// DataFlash stop update flag

// - Define definition -
#define	BLOCK_A		0x0F1000					// Block A Erase address
#define BLOCK_B		0x0F1400					// Block B Erase address
#define BLOCK_D		0x0F1C00					// Block D Erase address
#define BLOCK_A_ST	0x0F1040					// Block A Data start address
#define BLOCK_B_ST	0x0F1440					// Block B Data start address
#define DF_ST_OFF	0x0040						// DataFlash data start offset

#define DFBLOCK		10						// DNumber of area of DataFlash
#define DF_LEN		96						// Unit of DataFlash area

#define W_OK		0xA0						// Write OK
#define W_NG		0x07						// Write NG
#define AREA_NG		0x08						// ?Data exist before write


// - DataFlash data definition -
typedef struct
{
	//  ===== Initial Calibration Data =====
	WORD	d_LV[10];							// [20byte] AD converted value for calibration voltage LV
	WORD	Reserved1[6];						// [12byte] reserved
	WORD	d_HV[10];							// [20byte] AD converted value for calibration voltage HV
	WORD	Reserved2[6];						// [12byte] reserved
	WORD	d_LVREF[10];						// [20byte] Calibration voltage LV [1mV]
	WORD	Reserved3[6];						// [12byte] reserved
	WORD	d_HVREF[10];						// [20byte] Calibration voltage value HV [1mV]
	WORD	Reserved4[6];						// [12byte] reserved
	WORD	d_V12LV;							// [2byte] AD converted value for VIN12 calibration voltage LV
	WORD	d_V12HV;							// [2byte] AD converted value for VIN12 calibration voltage HV
	WORD	d_V12LVREF;							// [2byte] VIN12 calibration voltage value LV [1mV]
	WORD	d_V12HVREF;							// [2byte] VIN12 calibration voltage value HV [1mV]

	DWORD	d_0A;								// [4byte] Current counter value for initial calibration 0A
	DWORD	d_CURR;								// [4byte] Discharge current counter value for initial calibration
	WORD	d_CURRREF;							// [2byte] Reference current value for initial calibration
	WORD	d_CALTH;							// [2byte] Initial calibration value for temperature
	WORD	d_CALTHREF;							// [2byte] Initial calibration reference value for temperature
	WORD	d_TCLV;								// [2byte] AD converted value for Total cell voltage calibration voltage LV
	WORD	d_TCHV;								// [2byte] AD converted value for Total cell voltage calibration voltage HV
	WORD	d_TCLVREF;							// [2byte] Total cell voltage calibration voltage value LV [1mV]
	WORD	d_TCHVREF;							// [2byte] Total cell voltage calibration voltage value HV [1mV]
	WORD	d_MCUBG;							// [2byte] Initial calibration value for MCU BG
	WORD	d_CREG2;							// [2byte] Initial calibration value for CREG2

	//  ===== SMBus command =====
	BYTE	d_COM1B[4];			// [2byte] 0x1B:ManufactureDate
	BYTE	d_COM1C[8];			// [2byte] 0x1C:SerialNumber


} _ST_FLASH_OWN;
//__directmap _ST_FLASH_OWN u_fo_rom = { 0x0F1800 };		// FlashROM own data the top of the area:DataFlashMemory Block2
#pragma address u_fo_rom = 0x0F1800
GLOBAL  _ST_FLASH_OWN u_fo_rom;				// FlashROM own data the top of the area
GLOBAL _ST_FLASH_OWN u_fo_ram;					// FlashROM own data the top of the area:RAM for DataFlashMemory Block2
typedef struct{
    WORD   d_FULLCHGCV;		             // 0xF1C00 [2byte] Fullcharge Judgement Cell voltage
    WORD   d_OPCV;                                  // 0xF1C02 [2byte] Over Protection Cell Voltage
    WORD   d_OWCV;                                 // 0xF1C04 [2byte] Over Warning Cell Voltage
    WORD   d_FULLDISCV;                          // 0xF1C06 [2byte] Fulldischarge Judgement Cell Voltage
    WORD   d_PDCV;                                  // 0xF1C08 [2byte] Power Down Cell Voltage
    WORD   d_UPCV;                                  //  0xF1C0A [2byte] Under Protection Cell Voltage
    WORD   d_UWCV;                                  // 0xF1C0C [2byte] Under Warning Cell Voltage
    WORD   d_CHGSTOPPV;                         // 0xF1C0E [2byte] Charge Stop Pack Voltage       [10mv]
    WORD   d_CHGREQPV;                           // 0xF1C10 [2byte]  Charge Request Pack Voltage [10mv] ==> UPS
    WORD   d_DISSTOPPV;                          // 0xF1C12 [2byte] Discharge Stop Pack Voltage   [10mv]
    WORD   d_OVPRLSCV;                           // 0xF1C14 [2byte]  Over Voltage Protection Release Cell Voltaga ==> Trigger 된후 release 판단 
    WORD   d_UVPRLSCV;                           // 0xF1C16 [2byte]  Under Voltage Protection Release Cell Voltaga ==> Trigger 된후 release 판단 
    WORD   d_OVWRLSCV;                          // 0xF1C18  [2byte]  Over Voltage Warning Release Cell Voltaga ==> Trigger 된후 release 판단 
    WORD   d_UVWRLSCV;                           // 0xF1C1A  [2byte] Under Voltage Warning Release Cell Voltaga ==> Trigger 된후 release 판단  
    WORD   d_MSKCURR;                             // 0xF1C0C [2byte]   Mask Current 0A  
    WORD   d_DCDET;                                 // 0xF1C1E  [2byte]  Discharge detection current
    WORD   d_CCDET;                                 // 0xF1C20  [2byte]  Charge detection current
    WORD   d_FULLCHGCUR;                        // 0xF1C22  [2byte]  FullCharge Judgement Current
    WORD   d_CHGLMTCUR;                         //  0xF1C24  [2byte]  Charge Limit Current
    DWORD  d_OVERCH1;                            //  0xF1C26   [4byte]  Charge over current 1
    DWORD  d_OVERCH2;                            //  0xF1C2A  [4byte]  Charge over current 2 (Protection)
    DWORD  d_OVERDI1;                             //  0xF1C2E  [4byte]  Discharge over current 1
    DWORD  d_OVERDI2;                             //  0xF1C32  [4byte]  Discharge over current 2 (Protection)
    DWORD  d_ODLC1;                                //  0xF1C36  [4byte]  OEM Discharge Limited Current 1
    DWORD  d_ODLC2;                                //  0xF1C3A  [4byte]  OEM Discharge Limited Current 2
    DWORD  d_ODLC3;                                //  0xF1C3E  [4byte]  OEM Discharge Limited Current 3
    WORD   d_rsved3;                                // 0xF1C42  [2byte]  
    WORD   d_rsved4;                                // 0xF1C44  [2byte]        
    BYTE    d_DOTH;                                 // 0xF1C46  [1byte]  Discharge over temperature (H)
    char     d_DOTL;                                 // 0xF1C47  [1byte]  Discharge over temperature (L)
    BYTE    d_DTH_hys;                           // 0xF1C48 [1byte]        Discharge over temperature release hysteresis
    BYTE    d_COTH;                                // 0xF1C49 [1bye]     Charge over temperature (H)
    char      d_COTL;                                // 0xF1C4A  [1byte]    Charge over temperature (L)
    BYTE     d_CTH_hys;                           // 0xF1C4B    [1byte]   Charge over temperature release hysteresis
    WORD   d_rsved5;                              // 0xF1C4C  [2byte]  
    WORD   d_rsved6;                              // 0xF1C4E  [2byte]        
    BYTE    d_OCCPRACTIM;                    //  0xF1C50  [1byte]  Over Charge Current Protection Reaction Time [0.25s]
    BYTE    d_OCCPRLSTIM;                     //  0xF1C51  [1byte]  Over Charge Current Protection Release   Time [0.25s]
    BYTE    d_ODCPRACTIM;                    //  0xF1C52  [1byte]  Over DisCharge Current Protection Reaction Time [0.25s]
    BYTE    d_ODCPRLSTIM;                     //  0xF1C53  [1byte]  Over DisCharge Current Protection Release Time [0.25s]
    BYTE    d_OVPRACTIM;                      //  0xF1C54  [1byte]  Over Cell Protection Voltage Reaction Time [0.25s]
    BYTE    d_OVPRLSTIM;                      //  0xF1C55  [1byte]  Over Cell Protection Voltage Release Time [0.25s]
    BYTE    d_UVPRACTIM;                      //  0xF1C56  [1byte]  Under Cell Protection Voltage Reaction Time [0.25s]
    BYTE    d_UVPRLSTIM;                      //  0xF1C57  [1byte]  Under Cell Protection Voltage Release Time [0.25s]
    BYTE    d_OTRACTIM;                       //  0xF1C58  [1byte]  Over Temperature protection Reaction Time [0.25s]
    BYTE    d_OTRLSTIM;                       //  0xF1C59  [1byte]  Over Temperature protection Release Time [0.25s]
    BYTE    d_UTRACTIM;                       //  0xF1C5A  [1byte]  Under Temperature protection Reaction Time [0.25s]
    BYTE    d_UTRLSTIM;                       //  0xF1C5B  [1byte]  Under Temperature protection Release Time [0.25s]
    BYTE     d_SERIES;                            // 0xF1C5C   [1byte]   Series number of Cell
    BYTE     d_SERIES_2ND;                    // 0xF1C5D    [1byte]   Series number of Cell on 2ND BMIC
    WORD    d_CTLFLG;                            // 0xF1C5E   [2byte] Control flag 1
    WORD    d_INIT_FCC;                         // 0xF1C60   [2byte]    Initial FullChargeCapacity
    WORD   d_rsved10;                           // 0xF1C62     [2byte]
    BYTE     d_IN_TEMPn;                        // 0xF1C64    [1byte]   Internal Temperaure Install Number
    BYTE     d_EXT_TEMPn;                      // 0xF1C65    [1byte]    External Temperature Install Number
    BYTE     d_INITCAP_TBL[22];              // 0xF1C66    [6byte]     Initial capacity table 
    WORD    d_INITV_TBL[22];                   //  0xF1C7C    [12byte]   Initial voltage table
    WORD   d_CBONV;                               //  0xF1CA8    [2byte]    Cell balancing ON voltage
    WORD   d_CBOFFV;                              //  0xF1CAA    [2byte]    Cell balancing OFF voltage
    WORD   d_CBENVH;                              // 0xF1CAC    [2byte]     Cell balancing enable voltage(H)
    WORD   d_CBENVL;                              //  0xF1CAE   [2byte]     Cell balancing enable voltage(L)
    WORD   d_rsved7;                                // 0xF1CB0   [2byte]  
    BYTE     d_MF_NAME[22];                   // 0xF1CB2  [22byte]  Manufacture Name
    BYTE     d_MF_MODL[32];                   // 0xF1CC8   [32byte]  Manufacture Model
    BYTE     d_FW_VER[4];                        // 0xF1CE8  [4byte]   FirmWare Version "ver1.2.3" -->"v123"
    BYTE     d_CST_PART[8];                       //  0xF1CEC    [8byte]    Custom Part Number
    BYTE     d_BLD_REV[4];                        //  0xF1CF4     [4byte]     Build Revision
    BYTE     d_MF_DATE[6];                        //  0xF1CF8    [6byte]     Manufacture Date
    WORD    d_rsved8;                               //  0xF1CFE     [2byte]    
    WORD    d_rsved9;                               //  0xF1D00     [2byte]    
    BYTE     d_EOL;                                   // 0xF1D02    [1byte]     End of Life (0~100) ==> SOH
    BYTE     d_UARTMOUT;                         // 0xF1D03   [1byte]     Uart Commnunication Timeout [ms]
    DWORD   d_SOHmAh;                           //  0xF1D04   [4byte]    1 Cycle Count increase when SOHmAh = FCC*2
    WORD     d_CycleCnt;                           // 0xF1D08   [2byte]      cycle COunt
    BYTE      d_OCVTIME;                           // 0xF1D0A   [1byte]     OCV WAIT Minute TIME
    BYTE      d_PFENTRY[3];                         // 0xF1D0B   [3byte]     Permenant Fault Entry
    WORD    d_OPFCV;                            //0xF1D0E      [2byte]    Over  Permanent FAIL Cell Voltage
    WORD    d_UPFCV;                            //0xF1D10      [2byte]     Under Permanent FAIL Cell Voltage
    DWORD  d_OCPFC;                            //0xF1D12       [4byte]    Over Charge Permenant FAIL Current
    DWORD  d_ODPFC;                            //0xF1D16       [4byte]    Over Discharge Permenant FAIL Current
    BYTE     d_OCPFT;                           //0xF1D1A        [1byte]     Over Charge Permenant FAIL Temperature
    BYTE     d_UCPFT;                           //0xF1D1B        [1byte]     Under Charger Permenant FAIL Temperauture
    BYTE     d_ODPFT;                           //0xF1D1C       [1byte]     Over Discharge Permenant FAIL Temperature
    BYTE     d_UDPFT;                           //0xF1D1D       [1byte]     Under Discharge Permenant FAIL Temperature
    WORD    d_ChkSum;                        //0xF1D1E        [2byte]     Checksum
   
} _ST_FLASH_CUSTOM;

// ROM area
#define D_R_LV			u_fo_rom.d_LV			// [10byte] AD converted value for calibration voltage LV
#define D_R_HV			u_fo_rom.d_HV			// [10byte] AD converted value for calibration voltage HV
#define D_R_LVREF		u_fo_rom.d_LVREF		// [10byte] Calibration voltage LV [1mV]
#define D_R_HVREF		u_fo_rom.d_HVREF		// [10byte] Calibration voltage HV [1mV]
#define D_R_V12LV		u_fo_rom.d_V12LV		// [2byte] AD converted value for VIN12 calibration voltage LV
#define D_R_V12HV		u_fo_rom.d_V12HV		// [2byte] AD converted value for VIN12 calibration voltage HV
#define D_R_V12LVREF	u_fo_rom.d_V12LVREF		// [2byte] VIN12 calibration voltage value LV [1mV]
#define D_R_V12HVREF	u_fo_rom.d_V12HVREF		// [2byte] VIN12 calibration voltage value HV [1mV]

#define D_R_0A			u_fo_rom.d_0A			// [4byte] Current counter value for initial calibration 0A
#define D_R_CURR		u_fo_rom.d_CURR			// [4byte] Discharge current counter value for initial calibration
#define D_R_CURRREF		u_fo_rom.d_CURRREF		// [2byte] Reference current value for initial calibration
#define D_R_CALTH		u_fo_rom.d_CALTH		// [2byte] Initial calibration value for temperature
#define D_R_CALTHREF	u_fo_rom.d_CALTHREF		// [2byte] Initial calibration reference value for temperature

#define D_R_COM1B         u_fo_rom.d_COM1B
#define D_R_COM1C         u_fo_rom.d_COM1C
#define D_R_TCLV		u_fo_rom.d_TCLV			// [2byte] AD converted value for Total cell voltage calibration voltage LV
#define D_R_TCHV		u_fo_rom.d_TCHV			// [2byte] AD converted value for Total cell voltage calibration voltage HV
#define D_R_TCLVREF		u_fo_rom.d_TCLVREF		// [2byte] Total cell voltage calibration voltage value LV [1mV]
#define D_R_TCHVREF		u_fo_rom.d_TCHVREF		// [2byte] Total cell voltage calibration voltage value HV [1mV]
#define D_R_MCUBG		u_fo_rom.d_MCUBG		// [2byte] Initial calibration value for MCU BG
#define D_R_CREG2		u_fo_rom.d_CREG2		// [2byte] Initial calibration value for CREG2


// RAM area
#define D_LV			u_fo_ram.d_LV			// [10byte] AD converted value for calibration voltage LV
#define D_HV			u_fo_ram.d_HV			// [10byte] AD converted value for calibration voltage HV
#define D_LVREF			u_fo_ram.d_LVREF		// [10byte] Calibration voltage LV [1mV]
#define D_HVREF			u_fo_ram.d_HVREF		// [10byte] Calibration voltage HV [1mV]
#define D_V12LV			u_fo_ram.d_V12LV		// [2byte] AD converted value for VIN12 calibration voltage LV
#define D_V12HV			u_fo_ram.d_V12HV		// [2byte] AD converted value for VIN12 calibration voltage HV
#define D_V12LVREF		u_fo_ram.d_V12LVREF		// [2byte] VIN12 calibration voltage value LV [1mV]
#define D_V12HVREF		u_fo_ram.d_V12HVREF		// [2byte] VIN12 calibration voltage value HV [1mV]

#define D_0A			u_fo_ram.d_0A			// [4byte] Current counter value for initial calibration 0A
#define D_CURR			u_fo_ram.d_CURR			// [4byte] Discharge current counter value for initial calibration
#define D_CURRREF		u_fo_ram.d_CURRREF		// [2byte] Reference current value for initial calibration
#define D_CALTH			u_fo_ram.d_CALTH		// [2byte] Initial calibration value for temperature
#define D_CALTHREF		u_fo_ram.d_CALTHREF		// [2byte] Initial calibration reference value for temperature

//  ===== SMBus command =====
#define D_COM1B			u_fo_ram.d_COM1B	// [4byte] 0x1B:ManufactureDate
#define D_COM1C			u_fo_ram.d_COM1C	// [8byte] 0x1C:SerialNumber
#define D_TCLV		u_fo_ram.d_TCLV				// [2byte] AD converted value for Total cell voltage calibration voltage LV
#define D_TCHV		u_fo_ram.d_TCHV				// [2byte] AD converted value for Total cell voltage calibration voltage HV
#define D_TCLVREF		u_fo_ram.d_TCLVREF		// [2byte] Total cell voltage calibration voltage value LV [1mV]
#define D_TCHVREF		u_fo_ram.d_TCHVREF		// [2byte] Total cell voltage calibration voltage value HV [1mV]
#define D_MCUBG		u_fo_ram.d_MCUBG			// [2byte] Initial calibration value for MCU BG
#define D_CREG2		      u_fo_ram.d_CREG2			// [2byte] Initial calibration value for CREG2


/***********************************************************************
Global functions
***********************************************************************/
GLOBAL void FlashWrite(BYTE *apadr, BYTE *apdata, WORD abytecnt);
GLOBAL void FlashErase(void);
void Waittimer_us(BYTE ainval);
void Calc_Checksum(void);
void FlashEraseCalBlock(void);
void FlashWrite_CalBlock(void);
void FlashEraseFixedBlock(void);
void FlashWrite_FixedBlock(void);
void RestoreFixedData(void);
#undef		GLOBAL

#endif


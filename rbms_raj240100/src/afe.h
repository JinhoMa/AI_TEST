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
* File Name		: afe.h
* Version		: 1.00
* Contents		: afe function source file
* Programmer	: Yasuo Usuda
* Note			: 
************************************************************************
* Copyright(C) 2016 RENESAS ELECTRONICS CORPORATION
************************************************************************
 * History			: 2016.02.15 Ver 1.00
 *					:  New Create
 *					: 
*""FILE COMMENT END""**************************************************/
#ifndef _AFE_H_
#define _AFE_H_ 

#define	CS		P7_bit.no2						// CS port

#define	CS_DIR		PM7_bit.no2						// CS direction
static BYTE	lastpsw = 0;

#define AFE_WR(n,m)	lastpsw = __get_psw(); DI(); CS = 1;n=m;CS = 0; __set_psw(lastpsw);
#define AFE_RD(n,m)	lastpsw = __get_psw(); DI(); CS = 1;m=n;CS = 0; __set_psw(lastpsw);



/************************************WINDOW 0*************************/
#define AFE_PCODEL	AFEREG00					//(F0500)
#define AFE_PCODEH	AFEREG01					//(F0501)
#define AFE_RECODE	AFEREG02					//(F0502)
#define AFE_WINCHG	AFEREG03					//(F0503)
#define AFE_PDCNT	AFEREG04					//(F0504)
#define AFE_AFESWRST	AFEREG05				//(F0505)
#define AFE_AMOCON	AFEREG06					//(F0506)
#define AFE_PCON	AFEREG07					//(F0507)
#define AFE_AWDTCON	AFEREG08					//(F0508)
#define AFE_AWDTRF	AFEREG09					//(F0509)
#define AFE_VRG2LVD	AFEREG0B					//(F050B)
#define AFE_FCON	AFEREG0C					//(F050C)
#define AFE_DFOCON	AFEREG0D					//(F050D)
#define AFE_CFOCON	AFEREG0E					//(F050E)
#define AFE_FMON	AFEREG0F					//(F050F)
#define AFE_PWMCON	AFEREG10					//(F0510)
#define AFE_PWMPRE	AFEREG11					//(F0511)
#define AFE_PWMTIML	AFEREG12					//(F0512)
#define AFE_PWMTIMH	AFEREG13					//(F0513)
#define AFE_PWMFETSEL	AFEREG14				//(F0514)
#define AFE_AFMK0	AFEREG18					//(F0518)
#define AFE_AFMK1	AFEREG19					//(F0519)
#define AFE_AFMK2	AFEREG1A					//(F051A)
#define AFE_AFMK3	AFEREG1B					//(F051B)
#define AFE_AFIF0	AFEREG1C					//(F051C)
#define AFE_AFIF1	AFEREG1D					//(F051D)
#define AFE_AFIF2	AFEREG1E					//(F051E)
#define AFE_AFIF3	AFEREG1F					//(F051F)
#define AFE_ADCON0	AFEREG20					//(F0520)
#define AFE_ADCON1	AFEREG21					//(F0521)
#define AFE_ADCON2	AFEREG22					//(F0522)
#define AFE_ADMSEL0	AFEREG23					//(F0523)
#define AFE_ADMSEL1	AFEREG24					//(F0524)
#define AFE_ADMSEL3	AFEREG26					//(F0526)
#define AFE_ADMSEL4	AFEREG27					//(F0527)
#define AFE_ADMSEL5	AFEREG28					//(F0528)
#define AFE_ADSTS	AFEREG29					//(F0529)
#define AFE_ADORL	AFEREG2A					//(F052A)
#define AFE_ADORH	AFEREG2B					//(F052B)
#define AFE_ADRL1	AFEREG2C					//(F052C)
#define AFE_ADRH1	AFEREG2D					//(F052D)
#define AFE_ADRL2	AFEREG2E					//(F052E)
#define AFE_ADRH2	AFEREG2F					//(F052F)
#define AFE_ADRL3	AFEREG30					//(F0530)
#define AFE_ADRH3	AFEREG31					//(F0531)
#define AFE_ADRL4	AFEREG32					//(F0532)
#define AFE_ADRH4	AFEREG33					//(F0533)
#define AFE_ADRL5	AFEREG34					//(F0534)
#define AFE_ADRH5	AFEREG35					//(F0535)
#define AFE_ADRL6	AFEREG36					//(F0536)
#define AFE_ADRH6	AFEREG37					//(F0537)
#define AFE_IMPRL0	AFEREG3A					//(F053A)
#define AFE_IMPRH0	AFEREG3B					//(F053B)
#define AFE_IMPRL1	AFEREG3C					//(F053C)
#define AFE_IMPRH1	AFEREG3D					//(F053D)
#define AFE_IMPRL2	AFEREG3E					//(F053E)
#define AFE_IMPRH2	AFEREG3F					//(F053F)
#define AFE_IMPRL3	AFEREG40					//(F0540)
#define AFE_IMPRH3	AFEREG41					//(F0541)
#define AFE_IMPRL4	AFEREG42					//(F0542)
#define AFE_IMPRH4	AFEREG43					//(F0543)
#define AFE_IMPRL5	AFEREG44					//(F0544)
#define AFE_IMPRH5	AFEREG45					//(F0545)
#define AFE_IMPRL6	AFEREG46					//(F0546)
#define AFE_IMPRH6	AFEREG47					//(F0547)
#define AFE_COND0	AFEREG4C					//(F054C)
#define AFE_COND1	AFEREG4D					//(F054D)
#define AFE_COND2	AFEREG4E					//(F054E)
#define AFE_CCCON	AFEREG50					//(F0550)
#define AFE_CCRL	AFEREG51					//(F0551)
#define AFE_CCRM	AFEREG52					//(F0552)
#define AFE_CCRH	AFEREG53					//(F0553)
#define AFE_IMPCON0	AFEREG54					//(F0554)
#define AFE_PMHV	AFEREG58					//(F0558)
#define AFE_PHV	AFEREG59						//(F0559)
#define AFE_POMHV	AFEREG5A					//(F055A)
#define AFE_PMSHV	AFEREG5B					//(F055B)
#define AFE_HVIODIN	AFEREG5C					//(F055C)
#define AFE_ANPULL	AFEREG5D					//(F055D)
#define AFE_EXBPULL	AFEREG5E					//(F055E)
#define AFE_SCDCON	AFEREG60					//(F0560)
#define AFE_DOC1DCON	AFEREG61				//(F0561)
#define AFE_DOC2DCON	AFEREG62				//(F0562)
#define AFE_COCDCON	AFEREG63					//(F0563)
#define AFE_SCDTIME	AFEREG64					//(F0564)
#define AFE_DOC1DTIME1	AFEREG65				//(F0565)
#define AFE_DOC1DTIME2	AFEREG66				//(F0566)
#define AFE_DOC2DTIME1	AFEREG67				//(F0567)
#define AFE_DOC2DTIME2	AFEREG68				//(F0568)
#define AFE_COCDTIME	AFEREG69				//(F0569)
#define AFE_OCDSTS	AFEREG6A					//(F056A)
#define AFE_OCDRSTRT	AFEREG6B				//(F056B)
#define AFE_DWUDCON1	AFEREG6E				//(F056E)
#define AFE_DWUDCON2	AFEREG6F				//(F056F)
#define AFE_CWUDCON1	AFEREG70				//(F0570)
#define AFE_CWUDCON2	AFEREG71				//(F0571)
#define AFE_DBPTCON1	AFEREG72				//(F0572)
#define AFE_DBPTCON2	AFEREG73				//(F0573)
#define AFE_WUDTIME	AFEREG74					//(F0574)
#define AFE_WUDSTS	AFEREG75					//(F0575)
#define AFE_WUDRSTRT	AFEREG76				//(F0576)
#define AFE_ATMACR	AFEREG78					//(F0578)
#define AFE_ATMAR	AFEREG79					//(F0579)
#define AFE_ATMBCR	AFEREG7C					//(F057C)
#define AFE_ATMBR	AFEREG7D					//(F057D)
#define AFE_ATMBPRE1	AFEREG7E				//(F057E)
#define AFE_TMMCON	AFEREG7F					//(F057F)


// AFE parameter

									// - (F0503) WINCHG -
#define		WINCHG_RM0		0b00000000			// Select Register map 0
#define		WINCHG_RM1		0b00000001			// Select Register map 1
#define		WINCHG_TR		0b10000000			// Select trimming register

									// - (F0506) AMOCON -
#define		MCLKSEL_AOCO	0b00000000			// AOCO select
#define		MCLKSEL_ALOCO	0b00000001			// ALOCO select
#define		MCLKSTS_AOCO	0b00000000			// AOCO operation
#define		MCLKSTS_ALOCO	0b00010000			// ALOCO operation


									// - (F0507) PCON -
#define		BUFCEN1_EN		0b00000001			// BUFC1 enable
#define		BUFCEN1_DIS		0b00000000			// BUFC1 disable
#define		BUFCEN2_EN		0b00000010			// BUFC2 enable
#define		BUFCEN2_DIS		0b00000000			// BUFC2 disable
#define		BUFCEN3_EN		0b00000100			// BUFC3 enable
#define		BUFCEN3_DIS		0b00000000			// BUFC3 disable
#define		BUFCEN4_EN		0b00001000			// BUFC4 enable
#define		BUFCEN4_DIS		0b00000000			// BUFC4 disable
#define		VCPC_EN			0b00100000			// VCPC enable
#define		VCPC_DIS		0b00000000			// VCPC disable
#define		REG1C_EN		0b01000000			// REG1C enable
#define		REG1C_DIS		0b00000000			// REG1C disable


									// - (F0508) AWDTCON -
#define		WDT_64S			0b00000000			// Detect time 64sec
#define		WDT_32S			0b00000001			// Detect time 32sec
#define		WDT_16S			0b00000010			// Detect time 16sec
#define		WDT_8S			0b00000011			// Detect time 8sec
#define		WDT_4S			0b00000100			// Detect time 4sec
#define		WDT_2S			0b00000101			// Detect time 2sec
#define		WDT_STOP		0b00000111			// Stop
#define		WDT_MTRSEL_EN	0b01000000			// Reset output enable
#define		WDT_MTRSEL_DIS	0b00000000			// Reset output disable

									// - (F050B) VRG2LVD -
#define		VRG2LV_EN		0b00000001			// VRG2LV enable
#define		VRG2LV_DIS		0b00000000			// VRG2LV disable
#define		VRG2LVMD_INT	0b00000000			// VRG2LVMD:interrupt
#define		VRG2LVMD_RST	0b00000010			// VRG2LVMD:Reset
#define		VRG2LF_D		0b00010000			// VRG2LV detection
#define		VRG2LF_ND		0b00000000			// VRG2LV non detection


									// - (F050C) FCON -
#define		FET_OFF			0b00000000			// C=OFF, D=OFF
#define		FET_CD			0b00000011			// C=ON,  D=ON
#define		FET_C			0b00000010			// C=ON,  D=OFF
#define		FET_D			0b00000001			// C=OFF, D=ON

									// - (F050D) DFOCON -
#define		DFET_Hi			0b00000001			// DFET High Side
#define		DFET_Lo			0b00000000			// DFET Low Side
#define		DFCLP_EN		0b00000000			// DFET Clamp enabled
#define		DFCLP_DIS		0b00000010			// DFET Clamp disabled
#define		DFOFFBST_EN		0b00001000			// DFET OFF Boost enabled
#define		DFOFFBST_DIS	0b00000000			// DFET OFF Boost disabled
#define		DFOFFBSTC_EN	0b00010000			// PACK voltage output during OFF boost
#define		DFOFFBSTC_DIS	0b00000000			// (PACK - 10) voltage output during OFF boost

									// - (F050E) CFOCON -
#define		CFET_Hi			0b00000001			// CFET High Side
#define		CFET_Lo			0b00000000			// CFET Low Side
#define		CFCLP_EN		0b00000000			// CFET Clamp enabled
#define		CFCLP_DIS		0b00000010			// CFET Clamp disabled
#define		CFOFFBST_EN		0b00001000			// CFET OFF Boost enabled
#define		CFOFFBST_DIS	0b00000000			// CFET OFF Boost disabled
#define		CFOFFBSTC_EN	0b00010000			// VBAT voltage output during OFF boost
#define		CFOFFBSTC_DIS	0b00000000			// (VBAT - 10) voltage output during OFF boost

									// - (F050F) FMON -

#define		DFMON2_OFF		0b00000000			// Not DFET force OFF control for overcurrent
#define		DFMON2_ON		0b00000100			// DFET force OFF control for overcurrent
#define		DFMON3_OFF		0b00000000			// Not DFET force OFF control for wakeup
#define		DFMON3_ON		0b00001000			// DFET force OFF control for wakeup
#define		CFMON2_OFF		0b00000000			// Not DFET force OFF control for overcurrent
#define		CFMON2_ON		0b01000000			// DFET force OFF control for overcurrent
#define		CFMON3_OFF		0b00000000			// Not DFET force OFF control for wakeup
#define		CFMON3_ON		0b10000000			// DFET force OFF control for wakeup

									// - (F0510) PWMCON -
#define		PWM_EN			0b00000001			// PWM enable
#define		PWM_DIS			0b00000000			// PWM disable
#define		PWM_f1			0b00000000			// PWM count source(f1)
#define		PWM_f1half		0b00000010			// PWM count source(f1/2)
#define		PWM_8bit		0b00000000			// PWM mode 8bit resolution
#define		PWM_10bit		0b00100000			// PWM mode 10bit resolution

									// - (F0514) PWMFETSEL -
#define		PWM_CFET		0b00000000			// Select CFET for PWM
#define		PWM_DFET		0b00000001			// Select DFET for PWM

									// - (F0518) AFE Interrupt control 0 -
#define		INT_ADIE		0b00000001			// AD interrupt enable
#define		INT_CCIE		0b00000010			// CC interrupt enable
#define		INT_CHGIE		0b00010000			// CHG interrupt enable
#define		INT_DWUIE		0b00100000			// DWU interrupt enable
#define		INT_CWUIE		0b01000000			// CWU interrupt enable
#define		INT_DBPTIE		0b10000000			// DBPT interrupt enable

									// - (F0519) AFE Interrupt control 1 -
#define		INT_SCDIE		0b00000001			// SCD interrupt enable
#define		INT_DOC1DIE		0b00000010			// DOC1D interrupt enable
#define		INT_DOC2DIE		0b00000100			// DOC2D interrupt enable
#define		INT_COCDIE		0b00001000			// COCD interrupt enable
#define		INT_WDTIE		0b00010000			// WDT interrupt enable
#define		INT_VRG2IE		0b01000000			// VREG2 voltage drop interrupt enable



									// - (F051A) AFE Interrupt control 2 -
#define		INT_HVP0IE		0b00000001			// HVP0 interrupt enable
#define		INT_HVP1IE		0b00000010			// HVP1 interrupt enable
#define		INT_HVP2IE		0b00000100			// HVP2 interrupt enable

									// - (F051B) AFE Interrupt control 3 -
#define		INT_AFETRAIE	0b00000001			// AFE timer A interrupt enable
#define		INT_AFETRBIE	0b00000010			// AFE timer B interrupt enable

									// - (F051C) AFE Interrupt request 0 -
#define		INT_ADIR		0b00000001			// AD interrupt request
#define		INT_CCIR		0b00000010			// CC interrupt request
#define		INT_CCFSTFLG	0b00000100			// First CC interrupt request
#define		INT_CHGIR		0b00010000			// CHG interrupt request
#define		INT_DWUIR		0b00100000			// Discharge Wakeup interrupt request
#define		INT_CWUIR		0b01000000			// Charge Wakeup interrupt request
#define		INT_DBPTIR		0b10000000			// DBPT interrupt request
#define		INT_ADIR_W		(BYTE)~(INT_ADIR)	// AD interrupt request(for write command)
#define		INT_CCIR_W		(BYTE)~(INT_CCIR)	// CC interrupt request(for write command)
#define		INT_CCFSTFLG_W	(BYTE)~(INT_CCFSTFLG)	// First CC interrupt request(for write command)
#define		INT_CHGIR_W		(BYTE)~(INT_CHGIR)	// CHG interrupt request(for write command)
#define		INT_DWUIR_W		(BYTE)~(INT_DWUIR)	// Discharge Wakeup interrupt request(for write command)
#define		INT_CWUIR_W		(BYTE)~(INT_CWUIR)	// Charge Wakeup interrupt request(for write command)
#define		INT_DBPTIR_W	(BYTE)~(INT_DBPTIR)	// DBPT interrupt request(for write command)

									// - (F051D) AFE Interrupt request 1 -
#define		INT_SCDIR		0b00000001			// SCD interrupt request
#define		INT_DOC1DIR		0b00000010			// DOC1D interrupt request
#define		INT_COCDIR		0b00001000			// COCD interrupt request
#define		INT_WDTIR		0b00010000			// WDT interrupt request
#define		INT_OVADIR		0b00100000			// Over Voltage interrupt for AD request
#define		INT_VRG2IR		0b01000000			// VREG2 voltage drop interrupt request


#define		INT_SCDIR_W		(BYTE)~(INT_SCDIR)	// SCD interrupt request(for write command)
#define		INT_DOC1DIR_W	(BYTE)~(INT_DOC1DIR)// DOC1D interrupt request(for write command)
#define		INT_COCDIR_W	(BYTE)~(INT_COCDIR)	// COCD interrupt request(for write command)
#define		INT_WDTIR_W		(BYTE)~(INT_WDTIR)	// WDT interrupt request(for write command)
#define		INT_OVADIR_W	(BYTE)~(INT_OVADIR)	// Over Voltage interrupt for AD request(for write command)
#define		INT_VRG2IR_W	(BYTE)~(INT_VRG2IR)	// VREG2 voltage drop interrupt request(for write command)


									// - (F051E) AFE Interrupt request 2 -
#define		INT_HVP0IR		0b00000001			// HVP0 interrupt request
#define		INT_HVP1IR		0b00000010			// HVP1 interrupt request
#define		INT_HVP2IR		0b00000100			// HVP2 interrupt request

#define		INT_HVP0IR_W	(BYTE)~(INT_HVP0IR)	// HVP0 interrupt request(for write command)
#define		INT_HVP1IR_W	(BYTE)~(INT_HVP1IR)	// HVP1 interrupt request(for write command)
#define		INT_HVP2IR_W	(BYTE)~(INT_HVP2IR)	// HVP2 interrupt request(for write command)

									// - (F051F) AFE Interrupt request 3 -
#define		INT_AFETRAIR	0b00000001			// AFE timer A interrupt request
#define		INT_AFETRBIR	0b00000010			// AFE timer B interrupt request

#define		INT_AFETRAIR_W	(BYTE)~(INT_AFETRAIR)// AFE timer A interrupt request(for write command)
#define		INT_AFETRBIR_W	(BYTE)~(INT_AFETRBIR)// AFE timer B interrupt request(for write command)

									// - (F0520) ADC control 0 -

#define		ADC_EN			0b00000001			// ADC enable
#define		ADC_DIS			0b00000000			// ADC disable

									// - (F0521) ADC control 1 -
#define		ADC_FSP			0b00000000			// ADC force stop
#define		ADC_BTMODE1		0b00000001			// ADC Battery measurement mode 1
#define		ADC_BTMODE2		0b00000010			// ADC Battery measurement mode 2
#define		ADC_HVPMODE		0b00000100			// ADC HVP measurement mode
#define		ADC_THERMMODE	0b00000101			// ADC Themistor measurement mode
#define		ADC_SFUNCMODE	0b00000110			// ADC safety function mode
#define		ADC_ADIMPSEL	0b10000000			// IMP measurement enabled

									// - (F0522) ADC control 2 -

#define		ADATTIME1		0b00000000			// AD auto mode AD time(1msec)
#define		ADATTIME2		0b00000001			// AD auto mode AD time(2msec)
#define		ADATTIME4		0b00000010			// AD auto mode AD time(4msec)
#define		ADATTIME8		0b00000011			// AD auto mode AD time(8msec)
#define		ADATWAITTIME1	0b00000000			// AD auto mode AD Wait time(80usec)
#define		ADATWAITTIME2	0b00010000			// AD auto mode AD Wait time(120usec)
#define		ADATWAITTIME3	0b00100000			// AD auto mode AD Wait time(160usec)
#define		ADATWAITTIME4	0b00110000			// AD auto mode AD Wait time(200usec)
#define		ADATWAITTIME5	0b01000000			// AD auto mode AD Wait time(250usec)
#define		ADATWAITTIME6	0b01010000			// AD auto mode AD Wait time(500usec)
#define		ADATWAITTIME7	0b01100000			// AD auto mode AD Wait time(750usec)
#define		ADATWAITTIME8	0b01110000			// AD auto mode AD Wait time(1000usec)

									// - (F0523) ADC mesurement item select 0 -
#define		ADATOFF			0b00000001			// AD auto mode offset
#define		ADATCELL1		0b00000010			// AD auto mode cell1
#define		ADATCELL2		0b00000100			// AD auto mode cell2
#define		ADATCELL3		0b00001000			// AD auto mode cell3
#define		ADATCELL4		0b00010000			// AD auto mode cell4
#define		ADATCELL5		0b00100000			// AD auto mode cell5
#define		ADATALLCELL3	0b00001111			// AD auto mode cell1-3
#define		ADATALLCELL4	0b00011111			// AD auto mode cell1-4
#define		ADATALLCELL5	0b00111111			// AD auto mode cell1-5

									// - (F0524) ADC mesurement item select 1 -
#define		ADATOFF1		0b00000001			// AD auto mode offset
#define		ADATCELL6		0b00000010			// AD auto mode cell6
#define		ADATCELL7		0b00000100			// AD auto mode cell7
#define		ADATCELL8		0b00001000			// AD auto mode cell8
#define		ADATCELL9		0b00010000			// AD auto mode cell9
#define		ADATCELL10		0b00100000			// AD auto mode cell10
#define		ADATALLCELL7	0b00000111			// AD auto mode cell6-7
#define		ADATALLCELL8	0b00001111			// AD auto mode cell6-8
#define		ADATALLCELL9	0b00011111			// AD auto mode cell6-9
#define		ADATALLCELL10	0b00111111			// AD auto mode cell6-10

									// - (F0526) ADC mesurement item select 3 -
#define		ADATOFF2		0b00000001			// AD auto mode offset
#define		ADATPACK		0b00000010			// AD auto mode Pack voltage
#define		ADATPON			0b00000100			// AD auto mode PON voltage
#define		ADATVIN10		0b00001000			// AD auto mode VIN10(cell total voltage)

									// - (F0527) ADC mesurement item select 4 -
#define		ADATOFF3		0b00000001			// AD auto mode offset
#define		ADATAN0			0b00000010			// AD auto mode AN0
#define		ADATAN1			0b00000100			// AD auto mode AN1
#define		ADATAN2			0b00001000			// AD auto mode AN2
#define		ADATVREGpullAN0pull	0b00010000		// AD auto mode AN0 with pullup
#define		ADATVREGpullAN1pull	0b00100000		// AD auto mode AN1 with pullup
#define		ADATVREGpullAN2pull	0b01000000		// AD auto mode AN2 with pullup
#define		ADATVREGpull		0b10000000		// AD auto mode VREG pullup

									// - (F0528) ADC mesurement item select 5 -
#define		ADATOFF4		0b00000001			// AD auto mode offset
#define		ADATADREF		0b00000010			// AD auto mode AD reference voltage
#define		ADATCCREF		0b00000100			// AD auto mode CC reference voltage
#define		ADATADCCREF		0b00001000			// AD auto mode AD/CC reference voltage
#define		ADATMCUREF		0b00010000			// AD auto mode MCU reference voltage
#define		ADATETHERM		0b00100000			// AD auto mode simple thermistor
#define		ADATCREG2		0b01000000			// AD auto mode CREG2 voltage

									// - (F0529) ADC Status -
#define		ADOPFLG			0b00000001			// AD operation flag
#define		DADOFLG			0b00000010			// cell voltage measurement in DBPT flag
#define		ADSPFLG			0b00000100			// AD measurement stop flag
#define		ADRSTFLG		0b10000000			// MCU reference voltage measurement flag

									// - (F054C) Cell conditioning control 0 -
#define		CONDS0			0b00000001			// VIN1-VIN0 Switch ON
#define		CONDS1			0b00000010			// VIN2-VIN1 Switch ON
#define		CONDS2			0b00000100			// VIN3-VIN2 Switch ON
#define		CONDS3			0b00001000			// VIN4-VIN3 Switch ON
#define		CONDS4			0b00010000			// VIN5-VIN4 Switch ON
#define		CONDS5			0b00100000			// VIN6-VIN5 Switch ON
#define		CONDS6			0b01000000			// VIN7-VIN6 Switch ON
#define		CONDS7			0b10000000			// VIN8-VIN7 Switch ON

									// - (F054D) Cell conditioning control 1 -
#define		CONDS8			0b00000001			// VIN9-VIN8 Switch ON
#define		CONDS9			0b00000010			// VIN10-VIN9 Switch ON

									// - (F0550) CC control 0 -
#define		CCSHORT			0b00000001			// ISENS1-ISENS0 short
#define		CCDR_200		0b00000010			// Dynamic range 200mV
#define		CCDR_100		0b00000000			// Dynamic range 100mV
#define		CCDR_50			0b00000100			// Dynamic range 50mV
#define		CC_STOP			0b00000000			// CC stop
#define		CC_START		0b11000100			// CC start

									// - (F0554) CC control 0 for IMP measurement-
#define		IMPSEL			0b00000001			// select secondary filter

									// - (F0560) SCD control-
#define		SCEN			0b10000000			// Discharge short current detection enabled
#define		SCFETEN			0b01000000			// Discharge FET control enabled
#define		SCCMPV			0b00001010			// Discharge short current setting (12.5[mV] * 10 + 25[mV]= 150[mV] )

									// - (F0561) DOC 1 control-
#define		DOC1EN			0b10000000			// Discharge over current detection enabled
#define		DOC1FETEN		0b01000000			// Discharge FET control enabled
#define		DOC1CMPV		0b00100010			// Discharge over current setting (2.5[mV] * (n-1) + 15[mV] = 100[mV] )(n = 22)

									// - (F0562) DOC 2 control-
#define		DOC2EN			0b10000000			// Discharge over current detection enabled
#define		DOC2FETEN		0b01000000			// Discharge FET control enabled
#define		DOC2CMPV		0b00100010			// Discharge over current setting (2.5[mV] * (n-1) + 15[mV] = 100[mV] )(n = 22)


									// - (F0563) COCD control-
#define		COCFETEN		0b10000000			// Charge over current detection enabled
#define		COCEN			0b01000000			// Charge FET control enabled
#define		COCCMPV			0b00011100			// Charge over current setting (-5 [mV] * (n - 1CH) - 60[mV]= - 60[mV] )(n = 1C)


									// - (F0564) SCDTIME control-
#define		SCITIM			0b00000101			// Discharge short current detection time (61[us] * 5 = 305[us])

									// - (F0565) DOCTIME control-
#define		DOC1ITIM		0b01100100			// Discharge over current detection time (0.488[ms] * 100 = 48.8[ms])

									// - (F0566) COCTIME control-
#define		COCITIM			0b11111111			// Charge over current detection time (61[us] * 255 = 15555[us])


									// - (F056A) OCDSTS
#define		SCRFLG		0b00000001			// SCR FLAG
#define		DOC1FLG		0b00000010			// DOC1 FLAG
#define		DOC2FLG		0b00000100			// DOC2 FLAG
#define		COCRFLG		0b00001000			// COC FLAG
#define		DOC3FLG		0b00010000			// DOC3 FLAG
#define		DOC4FLG		0b00100000			// DOC4 FLAG


									// - (F056B) CD restart
#define		SCRSTRT			0b00000001			// SCR restart
#define		DOC1RSTRT		0b00000010			// DOC1 restart
#define		DOC2RSTRT		0b00000100			// DOC2 restart
#define		COCRSTRT		0b00001000			// COC restart
#define		DOC3RSTRT		0b00010000			// DOC3 restart
#define		DOC4RSTRT		0b00100000			// DOC4 restart

									// - (F056F) Discharge Wakeup current detection control 2 -
#define		DWUCALEN		0b00000001			// Discharge wakeup calibration enable
#define		DWUACALEN		0b00000010			// Discharge wakeup Auto calibration enable
#define		DWUPERIODIC		0b00000100			// Discharge wakeup Auto calibration(periodic)
#define		DWUGAIN_2		0b00000000			// Input gain 2
#define		DWUGAIN_5		0b00001000			// Input gain 5
#define		DWUGAIN_10		0b00010000			// Input gain 10
#define		DWUGAIN_20		0b00011000			// Input gain 20
#define		DWUEN			0b10000000			// Discharge wakeup detect enable
#define		DWUFETEN		0b01000000			// Discharge FET control enable
#define		DWUDIS			0b00000000			// Discharge wakeup detect disable

									// - (F0571) Charge Wakeup current detection control 2 -
#define		CWUCALEN		0b00000001			// Charge wakeup calibration enable
#define		CWUACALEN		0b00000010			// Charge wakeup Auto calibration enable
#define		CWUPERIODIC		0b00000100			// Charge wakeup Auto calibration(periodic)
#define		CWUGAIN_2		0b00000000			// Input gain 2
#define		CWUGAIN_5		0b00001000			// Input gain 5
#define		CWUGAIN_10		0b00010000			// Input gain 10
#define		CWUGAIN_20		0b00011000			// Input gain 20
#define		CWUEN			0b10000000			// Charge wakeup detect enable
#define		CWUFETEN		0b01000000			// Charge FET control enable
#define		CWUDIS			0b00000000			// Charge wakeup detect disable

									// - (F0573) Current detection control 2 for DBPT -
#define		DBPTCALEN		0b00000001			// Current calibration for DBPT enable
#define		DBPTACALEN		0b00000010			// DBPT Auto calibration enable
#define		DBPTEN			0b10000000			// Current detect for DBPT enable

									// - (F0575) Wakeup current detect status -
#define		DWUDFLG			0b00000001			// DWUD flag
#define		CWUDFLG			0b00000010			// CWUD flag
#define		DBPTFLG			0b00000100			// DBPT flag

									// - (F0576) Wakeup current detect restart -
#define		DWUDRSTRT		0b00000001			// DWUD restart
#define		CWUDRSTRT		0b00000010			// CWUD restart
#define		DBPTRSTRT		0b00000100			// DBPT restart

									// - (F0578) AFE timer A control -
#define		AFETRASTART_ST	0b00000001			// AFE timer A count start
#define		AFETRASTART_SP	0b00000000			// AFE timer A count stop
#define		AFETRASTOP		0b00000100			// AFE timer A count force stop
#define		AFETRACLKSEL_32K 	0b00000000		// AFE timer A source 32kHz
#define		AFETRACLKSEL_16K 	0b00001000		// AFE timer A source 16kHz

									// - (F0578) AFE timer B control -
#define		AFETRBSTART_ST	0b00000001			// AFE timer B count start
#define		AFETRBSTART_SP	0b00000000			// AFE timer B count stop
#define		AFETRBSTOP		0b00000100			// AFE timer B count force stop
#define		AFETRBCLKSEL_32K 	0b00001000		// AFE timer B source 32kHz
#define		AFETRBCLKSEL_16K 	0b00000000		// AFE timer B source 16kHz

#endif

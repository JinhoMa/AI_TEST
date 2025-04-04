/**
    ******************************************************************************
    *     @file         BMIC_Bsp.c
    *     @version   
    *     @author    
    *     @date      2020/03/03 
    *     @brief     
    ******************************************************************************
    */

#include "iodefine.h"
#include "define.h"
#include "ram.h"
#include "afe.h"
#include "afe1.h"																			// AFE register(Window 1)
#include "afe2.h"																			// AFE register(Window 2)
#include "kousei.h"																			// Initial calibration
#include "mcu.h"																			// mcu.c
#include "Dataflash.h"

void LOCO2OCO(void);
/*""FUNC COMMENT""**********************************************************
 * ID               : 1.0
 * module outline   : AFE initialize Process
 *--------------------------------------------------------------------------
 * Include          : 
 *--------------------------------------------------------------------------
 * Declaration      : void AFE_init(void)
 *--------------------------------------------------------------------------
 * Function         : AFE initialize.
 *--------------------------------------------------------------------------
 * Argument         : None
 *--------------------------------------------------------------------------
 * Return           : None
 *--------------------------------------------------------------------------
 * Input            : None
 *                  : 
 * Output           : None
 *                  : 
 *--------------------------------------------------------------------------
 * Used function    : 
 *                  : 
 *--------------------------------------------------------------------------
 * Caution          : 
 *--------------------------------------------------------------------------
 *""FUNC COMMENT END""*****************************************************/
void AFE_init(void)
{   
    AFE_WR(AFE_WINCHG,WINCHG_RM0);                                                      // AFE Window 0
    
    // AFE interrupt request flag clear
    AFE_WR(AFE_AFIF1,INT_WDTIR_W);                                                      // AFE WDT interrupt request flag clear
    
    /* AFE WDT stop */
    AFE_WR(AFE_AWDTCON,WDT_STOP);                                                       // AFE WDT stop
    
    AFE_WR(AFE_WINCHG,WINCHG_RM0);                                                      // AFE Window 0
    
    LOCO2OCO();                                                                         // Switch LOCO to OCO
                                                                                        // for countermeausre (Reset in Low Power Mode)
    
}

/*""FUNC COMMENT""**********************************************************
 * ID               : 1.0
 * module outline   : 1sec interval timer initial processing for AFE timer
 *--------------------------------------------------------------------------
 * Include          : 
 *--------------------------------------------------------------------------
 * Declaration      : void AFE_Timer1sec_init(void)
 *--------------------------------------------------------------------------
* Function          : 1sec timer initialize. (AFE timer)
 *                  : 
 *--------------------------------------------------------------------------
 * Argument         : None
 *--------------------------------------------------------------------------
 * Return           : None
 *--------------------------------------------------------------------------
 * Input            : None
 *                  : 
 * Output           : None
 *                  : 
 *--------------------------------------------------------------------------
 * Used function    : 
 *                  : 
 *--------------------------------------------------------------------------
 * Caution          : 
 *--------------------------------------------------------------------------
 *""FUNC COMMENT END""*****************************************************/
void AFE_Timer1sec_init(void)
{
    BYTE tmp;
    
    AFE_RD(AFE_AFMK3,tmp);
    AFE_WR(AFE_AFMK3,tmp & ~INT_AFETRAIE);                                              // AFE timer interrupt disable
    AFE_WR(AFE_AFIF3,INT_AFETRAIR_W);                                                   // Clear INT_AFETRAIR flag(AFE timer)
    AFE_WR(AFE_ATMACR,AFETRASTART_SP | AFETRACLKSEL_16K);                               // AFE timer stop / count source 16kHz / Timer mode
    AFE_WR(AFE_ATMAR,3);                                                                // 0.25[s] * (3+1) = 1[s]
    AFE_WR(AFE_ATMACR,AFETRASTART_ST | AFETRACLKSEL_16K)                                // AFE timer start
    
}

/*""FUNC COMMENT""**********************************************************
 * ID               : 1.0
 * module outline   : AFE trimming setting Process
 *--------------------------------------------------------------------------
 * Include          : 
 *--------------------------------------------------------------------------
 * Declaration      : void AFE_Trimming_setting(void)
 *--------------------------------------------------------------------------
 * Function         : AFE initialize.
 *--------------------------------------------------------------------------
 * Argument         : None
 *--------------------------------------------------------------------------
 * Return           : None
 *--------------------------------------------------------------------------
 * Input            : None
 *                  : 
 * Output           : None
 *                  : 
 *--------------------------------------------------------------------------
 * Used function    : 
 *                  : 
 *--------------------------------------------------------------------------
 * Caution          : 
 *--------------------------------------------------------------------------
 *""FUNC COMMENT END""*****************************************************/
void AFE_Trimming_setting(void)
{
    /* AFE Window Change */
    AFE_WR(AFE_WINCHG,WINCHG_TR)                                                        // AFE Window 2
    
    AFE_WR(AFE_AOCOT0,B_AFE_OCO0);
    AFE_WR(AFE_AOCOT1,B_AFE_OCO1);
    AFE_WR(AFE_AOCOT2,B_AFE_OCO2);
    AFE_WR(AFE_ALOCOT0,B_AFE_LOCO0);
    AFE_WR(AFE_ALOCOT1,B_AFE_LOCO1);
    AFE_WR(AFE_BT0,B_AFE_BT0);
    AFE_WR(AFE_BT1,B_AFE_BT1);
    AFE_WR(AFE_RT0,B_AFE_VREGAD);
    AFE_WR(AFE_RT1,B_AFE_VREGCC);
    AFE_WR(AFE_BUFT0,B_AFE_IVT0);
    AFE_WR(AFE_BUFT1,B_AFE_IVT1);
    //VREG2 = 3.3V(Selectable)
    AFE_WR(AFE_REG2T,B_AFE_VREG2_3V);
    // VREG2 = 5.0V(Selectable)
    //AFE_WR(AFE_REG2T,B_AFE_VREG2_5V);
    AFE_WR(AFE_FCPT0,B_AFE_FET0);
            
    AFE_WR(AFE_WINCHG,WINCHG_RM0);                                                      // AFE Window 0
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: AFE RESET Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void AFE_RESET(void)
 *--------------------------------------------------------------------------
 * Function			: AFE initialize.
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
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
 *""FUNC COMMENT END""*****************************************************/
void AFE_RESET(void)
{
	
    AFE_WR(AFE_WINCHG,WINCHG_RM0);	// AFE Window 0
	
    AFE_WR(AFE_AFESWRST,0xAA);
    AFE_WR(AFE_AFESWRST,0x55);
    AFE_WR(AFE_AFESWRST,0xFF);
    AFE_WR(AFE_AFESWRST,0x00);
								
    Waittimer_us(200);                       // Add Wait 200us (it needs more than 130usec wait)
			
    AFE_WR(AFE_WINCHG,WINCHG_RM0);	// AFE Window 0
}

#ifdef	OCDCALIB_EN
/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: AFE Over current detection Calibration Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void AFE_OCDCalib_D(void)
 *--------------------------------------------------------------------------
* Function			: AFE Over current detection calibration(Discharge).
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
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
 *""FUNC COMMENT END""*****************************************************/
void AFE_OCDCalib_D(void)
{
	BYTE atmp = 0;																		// OCDSTS register read
	BYTE atmp_1;
	BYTE awork = 0;																		// work for calibration value
	
	AFE_WR(AFE_WINCHG,WINCHG_RM0);														// AFE Window 0
	AFE_WR(AFE_DOC1DCON,0x00);															// 0[mV] setting
	AFE_WR(AFE_DOC1DTIME1,0x00);														// detection time = 0.488[ms]*(n + 1) = 0.488[ms] (n = 0)
	
	AFE_WR(AFE_WINCHG,WINCHG_RM1);														// AFE Window 1
	AFE_WR(AFE_DOC1CAL,DOC1CAL_EN);														// DOC1 calibration enabled
	AFE_WR(AFE_WINCHG,WINCHG_RM0);														// AFE Window 0
	AFE_WR(AFE_DOC1DCON,0x80);															// DOC1 detection curcuit enabled
	
	Waittimer_us(250);																	// wait 250us
		
	do{
	
		AFE_WR(AFE_WINCHG,WINCHG_RM1);													// AFE Window 1
		AFE_WR(AFE_DOC1CAL,DOC1CAL_EN | awork);											// DOC1 calibration enabled & calibration value
		AFE_WR(AFE_WINCHG,WINCHG_RM0);													// AFE Window 0
	
		AFE_WR(AFE_OCDRSTRT,DOC1RSTRT);													// Set CD restart
		
		Waittimer_us(250);																// wait 250us
		Waittimer_us(250);																// wait 250us
		Waittimer_us(200);																// wait 200us
		
		AFE_RD(AFE_OCDSTS,atmp);
		atmp_1 = atmp & DOC1FLG;
		
		if(awork < 0x18)
		{
			if(atmp_1 != DOC1FLG)
			{
				awork++;
			}else
			{
				break;
			}
		}else
		{
			awork = 0x18;
			break;
		}
	}while(1);	
	
	awork -= 1;
	AFE_WR(AFE_WINCHG,WINCHG_RM1);														// AFE Window 1
	AFE_WR(AFE_DOC1CAL,awork | DOC1CAL_DIS);											// DOC1CAL calibration value setting & DOC1 calibration disabled
	AFE_WR(AFE_WINCHG,WINCHG_RM0);														// AFE Window 0
	AFE_RD(AFE_DOC1DCON,atmp);															// DOC1DCON register read
	AFE_WR(AFE_DOC1DCON,atmp | 0x00);													// DOC1 detection curcuit disabled
	AFE_WR(AFE_OCDRSTRT,DOC1RSTRT);														// Discharge detection flag clear
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: AFE Over current detection Calibration Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void AFE_OCDCalib_C(void)
 *--------------------------------------------------------------------------
* Function			: AFE Over current detection calibration(Charge).
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
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
 *""FUNC COMMENT END""*****************************************************/
void AFE_OCDCalib_C(void)
{
	BYTE atmp = 0;																		// OCDSTS register read
	BYTE atmp_1;
	BYTE awork = 0x18;																	// work for calibration value
	
	AFE_WR(AFE_WINCHG,WINCHG_RM0);														// AFE Window 0
	AFE_WR(AFE_COCDCON,0x00);															// 0[mV] setting
	AFE_WR(AFE_COCDTIME,0x02);															// detectiom time 61[us] * 2 = 122[us]
	
	AFE_WR(AFE_WINCHG,WINCHG_RM1);														// AFE Window 1
	AFE_WR(AFE_COCCAL,COCCAL_EN | awork);												// COC calibration enabled
	AFE_WR(AFE_WINCHG,WINCHG_RM0);														// AFE Window 0
	AFE_WR(AFE_COCDCON,0x80);															// COC detection curcuit enabled
	
	Waittimer_us(250);																	// wait 250us
	
	do{	
		AFE_WR(AFE_WINCHG,WINCHG_RM1);													// AFE Window 1
		AFE_WR(AFE_COCCAL,COCCAL_EN | awork);											// COC calibration enabled & calibration value
		AFE_WR(AFE_WINCHG,WINCHG_RM0);													// AFE Window 0
		
		AFE_WR(AFE_OCDRSTRT,COCRSTRT);													// Set CD restart
		
		Waittimer_us(200);																// wait 200us
	
		AFE_RD(AFE_OCDSTS,atmp);
		atmp_1 = atmp & COCRFLG;
		
		
		if(awork > 0x00)
		{
			if(atmp_1 != COCRFLG)															
			{
				awork--;
			}else
			{
				break;
			}
		}else
		{
			awork = 0x00;
			break;
		}
	}while(1);	
	
	awork += 1;
	AFE_WR(AFE_WINCHG,WINCHG_RM1);														// AFE Window 1
	AFE_WR(AFE_COCCAL,awork | COCCAL_DIS);												// COCCAL calibration value setting & COC calibration disabled
	AFE_WR(AFE_WINCHG,WINCHG_RM0);														// AFE Window 0
	AFE_RD(AFE_COCDCON,atmp);															// COCDCON register read
	AFE_WR(AFE_COCDCON,atmp | 0x00);													// COC detection curcuit disabled
	AFE_WR(AFE_OCDRSTRT,COCRSTRT);														// Charge detection flag clear
}


#endif

#ifdef	WUD_EN
/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: AFE Wakeup discharge current setting Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void AFE_DWUC(void)
 *--------------------------------------------------------------------------
* Function			: AFE wakeup current setting(Discharge).
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
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
 *""FUNC COMMENT END""*****************************************************/
void AFE_DWUC_EN(void)
{
	AFE_WR(AFE_WUDRSTRT,DWUDRSTRT);														// DWU interrupt request flag clear
	
	AFE_WR(AFE_WUDTIME,0x0F);															// Wakeup current detection time = 3.91[ms]* 16 = 62.56[ms]
	AFE_WR(AFE_DWUDCON1,0x04);															// Wakeup current detection voltage:1.25[mV] * 4 = 5.0[mV]
	AFE_WR(AFE_DWUDCON2,DWUFETEN | DWUGAIN_10 | DWUACALEN | DWUPERIODIC |  DWUEN);		// FET enable, Gain:10 times, DWU auto calib:enabled, DWU detectiton enable
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: AFE Wakeup charge current setting Process
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void AFE_CWUC(void)
 *--------------------------------------------------------------------------
* Function			: AFE wakeup current setting(Charge).
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
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
 *""FUNC COMMENT END""*****************************************************/
void AFE_CWUC_EN(void)
{
	AFE_WR(AFE_WUDRSTRT,CWUDRSTRT);														// CWU interrupt request flag clear
	
	AFE_WR(AFE_WUDTIME,0x0F);															// Wakeup current detection time = 3.91[ms]* 16 = 62.56[ms]
	AFE_WR(AFE_CWUDCON1,0x04);															// Wakeup current detection voltage:1.25[mV] * 4 = 5.0[mV]
	AFE_WR(AFE_CWUDCON2,CWUFETEN | CWUGAIN_10 | CWUACALEN | CWUPERIODIC | CWUEN);		// FET enable, Gain:10 times, DWU auto calib:enabled, DWU detectiton enable
}


#endif
void BMIC_BspStart(void)
{
    AFE_WR(AFE_CCCON,CC_START);
}

void BMIC_BspInit(void)
{
        AFE_Trimming_setting();                                                             // AFE trimming setting(this function calls before AFE reset)
        //AFE_RESET();                                                                      // AFE Reset function(Default: Not execute this function)
        AFE_init();                                                                         // AFE initialize
        AFE_Timer1sec_init();                                                               // AFE timer initialize(1sec)
#ifdef	OCDCALIB_EN
        AFE_OCDCalib_D();
        AFE_OCDCalib_C();
#endif
        //AD stop
        AFE_WR(AFE_ADCON0,ADC_DIS);
        // CC stop
        AFE_WR(AFE_CCCON,CC_STOP);
        AFE_WR(AFE_OCDRSTRT,SCRSTRT);
        // Discharge short current detection time
        AFE_WR(AFE_SCDTIME,SCITIM);                                                         // detection time 61us * 5 = 305us
        // Discharge short current detection 
        AFE_WR(AFE_SCDCON,SCEN | SCFETEN | SCCMPV);                                         // FET enabled, 150mV/1mohm = 150A setting
#ifdef	DOC2D_EN	
        // Discharge over current 2 detection time
        AFE_WR(AFE_DOC2DTIME1,DOC1ITIM);                                                    // detection time 0.488ms* 100 = 48.8ms
        // Discharge over current 2 detection 
        AFE_WR(AFE_DOC2DCON,DOC2EN | DOC2FETEN | DOC2CMPV);                                 // FET enabled, 100mV/1mohm = 100A setting
#elif	defined DOC3D_EN
        AFE_WR(AFE_WINCHG,WINCHG_RM1);                                                      // AFE Window 1
        // Discharge over current 3 detection time
        AFE_WR(AFE_DOC3DTIME1,DOC1ITIM);                                                    // detection time 0.488ms* 100 = 48.8ms
        // Discharge over current 3 detection 
        AFE_WR(AFE_DOC3DCON,DOC3EN | DOC3FETEN | DOC3CMPV);                                 // FET enabled, 100mV/1mohm = 100A setting
        AFE_WR(AFE_WINCHG,WINCHG_RM0);                                                      // AFE Window 0
#elif defined DOC4D_EN
        AFE_WR(AFE_WINCHG,WINCHG_RM1);                                                      // AFE Window 1
        // Discharge over current 4 detection time
        AFE_WR(AFE_DOC4DTIME1,DOC1ITIM);                                                    // detection time 0.488ms* 100 = 48.8ms
        // Discharge over current 4 detection 
        AFE_WR(AFE_DOC4DCON,DOC4EN | DOC4FETEN | DOC4CMPV);                                 // FET enabled, 100mV/1mohm = 100A setting
        AFE_WR(AFE_WINCHG,WINCHG_RM0);                                                      // AFE Window 0
#else
        // Discharge over current 1 detection time
        AFE_WR(AFE_DOC1DTIME1,DOC1ITIM);                                                    // detection time 0.488ms* 100 = 48.8ms
        // Discharge over current 1 detection 
        AFE_WR(AFE_DOC1DCON,DOC1EN | DOC1FETEN | DOC1CMPV);                                 // FET enabled, 100mV/1mohm = 100A setting
#endif
        // Charge over current detection time
        AFE_WR(AFE_COCDTIME,COCITIM);                                                       // detectiom time 61[us] * 255 = 15564[us]
        // Charge over current detection 
        AFE_WR(AFE_COCDCON,COCFETEN | COCEN | COCCMPV);                                     // FET enabled, -60mV/1mohm = -60A setting
        // AFE interrupt request flag clear
        AFE_WR(AFE_AFIF0,INT_ADIR_W & INT_CCIR_W & INT_CCFSTFLG_W & INT_CHGIR_W ); 
        // AFE interrupt request flag clear
        AFE_WR(AFE_AFIF1,INT_SCDIR_W & INT_DOC1DIR_W & INT_COCDIR_W & INT_WDTIR_W); 
        // AFE interrupt request flag clear
        AFE_WR(AFE_AFIF3,INT_AFETRAIR_W); 
#ifdef	WUD_EN
        // AFE Interrupt control
        AFE_WR(AFE_AFMK0,INT_ADIE | INT_CCIE | INT_CHGIE | INT_DWUIE | INT_CWUIE);  
#else
        // AFE Interrupt control
        AFE_WR(AFE_AFMK0,INT_ADIE | INT_CCIE | INT_CHGIE ); 
#endif
        // ANL Interrupt control 
        AFE_WR(AFE_AFMK1,INT_SCDIE | INT_DOC1DIE | INT_COCDIE | INT_WDTIE);                 // SCD,DOC,COC,WDT interrupt enabled
        // AFE timer Interrupt control
        AFE_WR(AFE_AFMK3,INT_AFETRAIE);                                                     // AFE timer A interrupt enabled
        AFE_WR(AFE_COND0,COND_OFF);                                                         // Cell Balancing OFF(Cell 1 - Cell 8) 
        AFE_WR(AFE_COND1,COND_OFF);                                                         // Cell Balancing OFF(Cell 9 - Cell 10)
    
        // AFE timer interrupt enable(INTP13)
        EGP1_bit.no5 = 0;                                                                   // Falling edge
        EGN1_bit.no5 = 1;
        PPR013 = 0;                                                                         // Interrupt top priority
        PPR113 = 0;
        PIF13 = 0;                                                                          // Interrupt flag clear
        PMK13 = 0;                                                                          // Interrupt enable
    
        // ANL interrupt enable(INTP12)
        EGP1_bit.no4 = 0;                                                                   // Falling edge
        EGN1_bit.no4 = 1;
        PPR012 = 0;                                                                         // Interrupt top priority
        PPR112 = 0;
        PIF12 = 0;                                                                          // Interrupt flag clear
        PMK12 = 0;                                                                          // Interrupt enable
    
        // WU interrupt enable(INTP11)
        EGP1_bit.no3 = 0;                                                                   // Falling edge
        EGN1_bit.no3 = 1;
        PPR011 = 0;                                                                         // Interrupt top priority
        PPR111 = 0;
        PIF11 = 0;                                                                          // Interrupt flag clear
        PMK11 = 0;                                                                          // Interrupt enable
    
        // HVP interrupt enable(INTP10)
        EGP1_bit.no2 = 0;                                                                   // Falling edge
        EGN1_bit.no2 = 1;
        PPR010 = 0;                                                                         // Interrupt top priority
        PPR110 = 0;
        PIF10 = 0;                                                                          // Interrupt flag clear
    
        // AD interrupt enable(INTP9)
        EGP1_bit.no1 = 0;                                                                   // Falling edge
        EGN1_bit.no1 = 1;
        PPR09 = 1;                                                                          // Interrupt top priority
        PPR19 = 1;
        PIF9 = 0;                                                                           // Interrupt flag clear
        PMK9 = 1;                                                                           // Interrupt disable
    
        // CC interrupt enable(INTP8)
        EGP1_bit.no0 = 0;                                                                   // Falling edge
        EGN1_bit.no0 = 1;
        PPR08 = 0;                                                                          // Interrupt low priority
        PPR18 = 0;
        PIF8 = 0;
        PMK8 = 0;                                                                           // Interrupt enable

}
#ifdef	Clock_SW
/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: LOCO switch function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void OCO2LOCO()
 *--------------------------------------------------------------------------
 * Function			: Clock switch(OCO -> LOCO).
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
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
 *""FUNC COMMENT END""*****************************************************/
void OCO2LOCO(void)
{
	
	BYTE atmp;																			// AMOCON read
	
	DI();																				// Interrupt disable
	
	AFE_WR(AFE_WINCHG,WINCHG_RM0);														// Change WINDOW0
	AFE_WR(AFE_AMOCON,MCLKSEL_ALOCO);													// ALOCO select
	
	do{
	
		AFE_RD(AFE_AMOCON,atmp);														// MCLKSTS bit read
		atmp = atmp & 0x10;																// Bit Mask
		
	}while(atmp != MCLKSTS_ALOCO);														// ALOCO select wait
	
	EI();																				// Interrupt enable
	
}

/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: OCO switch function
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void LOCO2OCO()
 *--------------------------------------------------------------------------
 * Function			: Clock switch(LOCO -> OCO).
 *					: 
 *--------------------------------------------------------------------------
 * Argument			: None
 *--------------------------------------------------------------------------
 * Return			: None
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
 *""FUNC COMMENT END""*****************************************************/
void LOCO2OCO(void)
{
	
	BYTE atmp;																			// AMOCON read
	
	DI();																				// Interrupt disable
	
	
	AFE_WR(AFE_WINCHG,WINCHG_RM0);														// Change WINDOW0
	AFE_WR(AFE_AMOCON,MCLKSEL_AOCO);													// AOCO select
	
	do{
	
		AFE_RD(AFE_AMOCON,atmp);														// MCLKSTS bit read
		atmp = atmp & 0x10;																// Mask 4bit
		
	}while(atmp != MCLKSTS_AOCO);														// AOCO select wait
	
	EI();																				// Interrupt enable
}
#endif

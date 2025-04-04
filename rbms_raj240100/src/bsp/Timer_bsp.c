/**
    ******************************************************************************
    *     @file         Timer_bsp.c
    *     @version   
    *     @author    
    *     @date      2020/03/02 
    *     @brief     
    ******************************************************************************
    */
#include "iodefine.h"
#include "define.h"
#include "ram.h"
#include "smbus.h"										// smbus.c
#include "smbus_ex.h"									// smbus_ex.c
#include "mode.h"										// mode.c
#include "cell.h"
#include "can_ex.h"
#include "BMSHal.h"
#include "main.h"
#include "dataflash.h"

#define _TIME

extern     void Can_Write_PCTool(void);
extern  void HAL_TIM_ElapsedCallback(void *htim); 

#pragma interrupt _int_Timer0_1msec(vect=INTTM00)		// 500usec interval timer interrupt
#pragma interrupt _int_Timer1_1msec(vect=INTTM01)			// 1msec interval timer interrupt
#pragma interrupt _int_Timer2_1msec_uart(vect=INTTM02)	// 1msec interval timer interrupt

//jae.kim
//���� 1ms ������ ���ͷ�Ʈ �߻��ϵ��� ���� 
//Sleep Mode�� �Ҹ� ���� �� ���̱� ���ؼ��� Timer �ð� ���� �ʿ� 
void Timer0_BspInit(void)
{
    /* Stop channel0 */
    TT0L_bit.no0 = 1;   // Stop Timer0 interrupt 
    TPS0 |= 0b0000000000000011;                                                         // CKm1 : Clock=1MHz(8MHz/8)
            //            |||+- PRSm00 : 1
            //            ||+-- PRSm01 : 1
            //            |+--- PRSm02 : 0
            //            +---- PRSm03 : 0

    /* Mask channel0 interrupt */
    TMMK00 = 1;                                                                         // disable INTTM00 interrupt
    TMIF00 = 0;                                                                         // clear INTTM00 interrupt flag
    /* Set INTTM00 top priority */
    // Uart ���� �켱������ ���� �Ѵ�.
    TMPR100 = 1;                                                                        // TM00 Interrupt Low priority
    TMPR000 = 1;
    /* Channel0 used as interval timer */
    TMR00 = 0x8000;                                                                 // Select CKm0
    TDR00 = 1000;                                                                        // 1MHz * 1000 = 1msec

    TMIF00 = 0;                                                                     // clear INTTM00 interrupt flag
    TMMK00 = 0;                                                                     // enable INTTM00 interrupt
    TS0L_bit.no0 = 1;                                                                   // Start TM00
}
   


/*""FUNC COMMENT""**********************************************************
 * ID               : 1.0
 * module outline   : 1msec interval timer initial processing 
 *--------------------------------------------------------------------------
 * Include          : 
 *--------------------------------------------------------------------------
 * Declaration      : void Timer1msec_init(void)
 *--------------------------------------------------------------------------
* Function          : 1msec timer initialize. (Timer array unit0 channel1)
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
// Modbus timer �� dynamic start, stop �� ���� �Ѵ�.

void Timer1_BspInit(void)
{
    /* Stop channel1 */
    TT0L_bit.no1 = 1;                                                                   // Stop TM01

    TPS0 |= 0b0000000000110000;                                                         // CKm1 : Clock=1MHz(8MHz/8)
            //        |||+- PRSm10 : 1
            //        ||+-- PRSm11 : 1
            //        |+--- PRSm12 : 0
            //        +---- PRSm13 : 0

    /* Mask channel1 interrupt */
    TMMK01 = 1;                                                                         // disable INTTM01 interrupt
    TMIF01 = 0;                                                                         // clear INTTM01 interrupt flag
    /* Set INTTM01 top priority */
    // Uart ���� �켱������ ���� �Ѵ�.
    TMPR101 = 1;                                                                        // TM01 Interrupt top priority
    TMPR001 = 1;
    /* Channel1 used as interval timer */
    TMR01 = 0x8000;                                                                     // Select CKm1
    TDR01 = 1000;                                                                       // 1MHz * 1000 = 1msec

    TMIF01 = 0;                                                                         // clear INTTM01 interrupt flag
    TMMK01 = 0;                                                                         // enable INTTM01 interrupt
    
}


/*""FUNC COMMENT""**********************************************************
 * ID				: 1.0
 * module outline	: 1msec interval timer initial processing 
 *--------------------------------------------------------------------------
 * Include			: 
 *--------------------------------------------------------------------------
 * Declaration		: void Timer1msec_init(void)
 *--------------------------------------------------------------------------
* Function			: 1msec timer initialize. (Timer array unit0 channel1)
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
void Timer2_BspInit(void)
{
	/* Stop channel1 */
	TT0L_bit.no2 = 1;																	// Stop TM02

	TPS0 |= 0b0000000000110000;														// CKm1 : Clock=1MHz(8MHz/8)
			//		  |||+- PRSm10 : 1
			//		  ||+-- PRSm11 : 1
			//		  |+--- PRSm12 : 0
			//		  +---- PRSm13 : 0

	/* Mask channel1 interrupt */
	TMMK02 = 1;																			// disable INTTM02 interrupt
	TMIF02 = 0;																			// clear INTTM02 interrupt flag
	/* Set INTTM01 top priority */
	TMPR102 = 0;																		// TM02 Interrupt top priority
	TMPR002 = 0;
	/* Channel1 used as interval timer */
	TMR02 = 0x8000;																		// Select CKm1
	TDR02 = 1000;																		// 1MHz * 1000 = 1msec

	TMIF02 = 0;																			// clear INTTM02 interrupt flag
	TMMK02 = 0;																			// enable INTTM02 interrupt
	
}


//static WORD tick_10ms;
//static WORD can_timeoutcnt;
extern uint64_t time_tick;
extern uint32_t t_1ms;
extern uint8_t t_10ms_1;
extern uint8_t t_20ms_1;
extern uint8_t t_100ms_1;
extern uint8_t t_200ms_1;
extern uint8_t t_300ms_1;
extern uint8_t t_500ms_1;

// 1sec interval timer (Timer array unit0 channel0)
void _int_Timer0_1msec(void)
{
    /*********************************************************************************************
    **
    **   tick time 
    **
    **/
    
        /* Start user code. Do not edit comment generated here */
        time_tick++;
        t_1ms++;
        if(t_1ms % 10 == 0){
            t_10ms_1=1;
        }
        if( ((t_1ms+5)% 20) == 0){
            t_20ms_1=1;
        }
        if( ((t_1ms+50)%300) == 0){
            t_300ms_1 = 1;
            //KEEPALIVE SIG  toggle
            KEEPALIVE_SIG_TOGLE;
        }
        if( t_1ms %100 == 0){
            t_100ms_1 = 1;
        }
        if ((t_1ms+200) % 500 == 0){
            t_500ms_1=1;
        }
        if (t_1ms % 200 == 0)
        {
            t_200ms_1 = 1;
        }

        if(READ_MAINMCU_WDPIN  ==1 ){
            mainmcu_wd_start = 1;
            if( from_mcuwdpin_prvstate ==0){
                mainmcu_wcdog_event =1;
                from_mcuwdpin_prvstate = 1;
            }
        }else{
            from_mcuwdpin_prvstate =0;
        }
}


	
// 1msec interval timer (Timer array unit0 channel2)
void _int_Timer2_1msec_uart(void)
{

}

// 1msec interval timer (Timer array unit0 channel1)
void _int_Timer1_1msec(void)
{
    HAL_TIM_ElapsedCallback(NULL);
}


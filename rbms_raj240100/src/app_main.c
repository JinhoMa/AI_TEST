
/* Start user code for include. Do not edit comment generated here */
#include "BMShal.h"
#include "oMbSlave.h"
#include <stdio.h>
/* End user code. Do not edit comment generated here */
#include "define.h"								// union/define definition
#include "ram.h"
#include "afe.h"														// AFE window 0
#include "afe1.h"														// AFE window 1
#include "mode.h"
#include "main.h"
#include "mcu.h"
#include "dataflash.h"

HoldRegisters hr;
InputRegisters ir;
uint8_t state;
//char AppName[16];


/*******************************************************************
 * STL
*******************************************************************/
#if 0
#include "stl.h"
BYTE g_resultBool;
uint16_t __far *g_pMC_StAdr;        /* Start Address pointer */
uint16_t g_RAM_MarchC_lgth;        /* Address range variable */
#define  START_ADDR 0xfe301
#define  END_ADDR   0xffee0
unsigned char RamTestSampleBuffer[64];
#endif

BYTE PON;


extern uint8_t t_10ms_1;
extern uint8_t t_20ms_1;
extern uint8_t t_300ms_1;
extern uint8_t t_100ms_1;
extern uint8_t t_500ms_1;
extern uint8_t t_200ms_1;

extern void mbSlavePortPoll(void);
extern void oMbSlaveRegHoldingRegMemory(int32_t sid, uint16_t *ptr, uint32_t size);
extern void oMbSlaveRegInputRegMemory(int32_t sid, uint16_t *ptr, uint32_t size);
void KeppAlive_Signal_Toggle(void);
void processHostCommand(void);

void Task_COM_Run(void);
void Task_MCUMon_Run(void);
void Task_COM_Run(void);
void Task_KeepAlive_Run(void);
void Task_UartMon_Run(void);
void Task_CVT_Run(void);
void Task_Stl_Run(void);
//static void runGuageVolCur(void);
//static void runGuageGFD(void);
int32_t rcv_cnt =0;
uint8_t from_mcuwdpin_prvstate =0;
int32_t mainmcu_wcdog_event=0;  // low에서 high 로 감지 될때 마다
int32_t mainmcu_uart_wcdog_event = 0; // low에서 high 로 감지 될때 마다
int32_t mainmcu_wdevent_wait;
uint8_t mainmcu_wd_start =0;  // Low 에서 high로 최초 1번 감지시 
uint16_t pon_hold_cnt=0;
uint16_t pwr_off_cnt=0;
uint16_t fuse_off_cnt = 0;
uint16_t Ext_Power_Cnt= 0;
uint8_t ext_power_on =0;

void AsyncUartHandler(uint8_t mode)
{
	rcv_cnt++;
}
void app_main(void)
{
  //int32_t fd = hSerOpen("COM1", 115200, AsyncUartHandler);
    int32_t fd;
    ext_power_on  =1;
    //sprintf(AppName, "%s", "BMIC1");
    fd= oMbSlaveOpen((uint32_t)OMB_RTU,1,COM1,115200);
    oMbSlaveRegHoldingRegMemory(fd, (uint16_t*)&hr, sizeof(HoldRegisters));
    oMbSlaveRegInputRegMemory(fd,  (uint16_t*)&ir, sizeof(InputRegisters));
    ir.model.version = ((0x10 | MAJOR_VER) << 8);
    ir.model.version |= (MINOR1_VER << 4);
    
    oMbSlaveStart(fd);
    for(;;){
        oMbSlavePoll(fd);
        mbSlavePortPoll();
        if( t_10ms_1 )
        {
            t_10ms_1=0;
            Task_COM_Run();
        }
        if( t_20ms_1)
        {
            t_20ms_1=0;
            Task_CVT_Run();
        }
        if( t_300ms_1)
        {
            t_300ms_1 = 0;
            Task_MCUMon_Run();
        }
        if( t_100ms_1)
        {
            t_100ms_1 = 0;
            Task_KeepAlive_Run();
        }
        if (t_200ms_1)
        {
            t_200ms_1 = 0;
            Task_UartMon_Run();

        }
        if( t_500ms_1)
        {
            t_500ms_1 = 0;
	    /*
            if(start_time < 20){
                start_time++;
                continue;
            }*/
            // Task_Stl_Run();
        }
    }
}
static uint8_t getNextState(uint8_t cmd){
    int i;
    for(i = 0 ; i < 5 ; i++){
        switch(cmd){
            case C_BATVOL:
                return CAL_BATVOL;
            case C_PCSVOL:
                return CAL_PCSVOL;
            case C_CUR:
                return CAL_CUR;
            case C_GFD_P:
                return CAL_GFD_P;
            case C_GFD_N:
                return CAL_GFD_N;
            case C_CONFIG:
                return CONFIG_WRITE;
            case C_RUN:
                return RUN_WAIT;
            case C_RESET:
                return INIT;
        }
    }
    return INIT;
}

void resetValue(void)
{

}
void processHRCommand(void){
    if(hr.model.cbal_on == SW_ON)
    {
        IO_CELLBALOn();
    }
    else if( hr.model.cbal_on == SW_OFF)
    {
        IO_CELLBALOff();
    }
    if(hr.model.fuse_off == SW_ON)
    {
        IO_BLOW_FUSE();
    }
    else if(hr.model.fuse_off == SW_OFF)
    {
        IO_CONNECT_FUSE();
    }

    if(hr.model.Cfet == SW_ON )
    {
        IO_CFETOn();
    }
    else if(hr.model.Cfet == SW_OFF )
    {
        IO_CFETOff();
    }
    
    if( hr.model.Dfet == SW_ON)
    {
        IO_DFETOn();
    }
    else if(hr.model.Dfet == SW_OFF)
    {
        IO_DFETOff();
    }
}

void processHostCommand(void)
{
    WORD tt;
    BYTE window;
    switch(state)
    {
        case INIT:
            state = CONFIG_WAIT;
            ir.model.mode = M_READY;
            state = CONFIG_WAIT;
            break;
        case CONFIG_WAIT:
            if( hr.model.cmd == C_CONFIG)
            {
                state = CONFIG_WRITE;
            }
			state = RUN;
            break;
        case CONFIG_WRITE:
            ir.model.mode = M_CONFIG;
            state = RUN_WAIT;
            break;
		case CAL_BATVOL:
            ir.model.mode = M_CAL_BATVOL;
            ir.model.ad_batvol = afecon_readBatVol();
            state = getNextState(hr.model.cmd);
			break;
	    case CAL_PCSVOL:
            ir.model.mode = M_CAL_PCSVOL;
           // ir.model.ad_pcsvol = afecon_readPcsVol();
            state = getNextState(hr.model.cmd);
		    break;
		case CAL_CUR:
            ir.model.mode = M_CAL_CUR;
            ir.model.ad_cur = afecon_readCurrent();
            state = getNextState(hr.model.cmd);
		    break;
		case CAL_GFD_P:
            ir.model.mode = M_CAL_GFD_P;
           // ir.model.ad_gfd_p = afecon_readGFD_P();
            state = getNextState(hr.model.cmd);
		    break;
		case CAL_GFD_N:
            ir.model.mode = M_CAL_GFD_N;
           // ir.model.ad_gfd_n = afecon_readGFD_N();
            state = getNextState(hr.model.cmd);
		    break;
        case RUN_WAIT:
            if(hr.model.cmd == C_RUN)
            {
                state = RUN;
                ir.model.mode = M_RUN;
            }else if( hr.model.cmd != C_CONFIG){
                state = getNextState(hr.model.cmd);
            }
            break;
        case RUN:
            /******************************************************
            * PON 입려 신호 Monitoring
            ******************************************************/
            AFE_RD(AFE_WINCHG, window);
      	    AFE_WR(AFE_WINCHG,WINCHG_RM1);														// AFE Window 1
            AFE_RD(AFE_PONMON, PON);

            ir.model.pon = PON;
            if( PON )
            {
                pon_hold_cnt++; //one increament per 10ms
                if( pon_hold_cnt >= 500 )  //5 sec
                {
                    ir.model.mode = M_PDOWN;
                    IO_LEDOff();
                    IO_ExtPowerOff();
                    state = PWR_OFF;
                    return;
                }
                if( pon_hold_cnt >= 20)
                {
                    ext_power_on = ON;
					IO_ExtPowerOn();
                }
            }else{
                pon_hold_cnt =0;
            }

            if(ext_power_on == ON){
                // HMI 전원은 RAJ에서 직접 차단하지 않고 HMI에서 자체적으로 제어하는걸로
                // Ext_Power_Cnt++;
                // if( Ext_Power_Cnt >= 3000 ) //30sec
                // {
                //     IO_ExtPowerOff();
                //     ext_power_on = OFF;
                // }
            }else{
                Ext_Power_Cnt= 0;
            }

            /******************************************************
            * FET 신호 Monitoring
            ******************************************************/
            BYTE fet_status;
       	    AFE_WR(AFE_WINCHG,WINCHG_RM0);														// AFE Window 0
            AFE_RD(AFE_FCON, fet_status);                // FET 상태 레지스터 읽기
            
            // CfetState, DfetState 업데이트
            ir.model.CfetState = (fet_status & FET_C) ? 1 : 0;  // C-FET 상태
            ir.model.DfetState = (fet_status & FET_D) ? 1 : 0;  // D-FET 상태
            
            ir.model.mode = M_RUN;

            if(hr.model.cmd == C_RUN){ 
                
            }else
            {
                state = getNextState(hr.model.cmd);
            }
            processHRCommand();
            if(hr.model.fuse_off == 1){
				state = FUSE_OFF;
			}
             break;
        case FUSE_OFF:
            fuse_off_cnt++;
            break;
        case PWR_OFF:
            if(++pwr_off_cnt < 50 ){ //500msec  동안 MCU가 power down 처리 하도록 대기
                return;
            }
            PowerDown();
            for(tt=0;tt<1000;tt++)
            {   //2sec
                Waittimer_us(250); 
                Waittimer_us(250);
                Waittimer_us(250);
                Waittimer_us(250);
            }
            break;
        default:
            ir.model.mode = ERR;
            break;
    }
}



void KeppAlive_Signal_Toggle(void)
{
   KEEPALIVE_SIG_TOGLE;
}

void OVD_OCD_Signal_Set(void)
{
   // P6.3 = 1;
}

void OVD_OCD_Signal_Reset(void)
{
   // P6.3 = 0;
}

void Task_COM_Run(void)
{
    processHostCommand();   
   // runGuageVolCur();   
}

#define WD_MCU_RESET    //! 평소 테스트시에는 편의성을 위해 주석처리하기기
void Task_MCUMon_Run(void)
{
    if(mainmcu_wcdog_event == 1 ) 
    {
        mainmcu_wcdog_event =0;
        mainmcu_wdevent_wait=0;
    }else{
        if(mainmcu_wd_start != 1){
            return;
        }
        if( mainmcu_wdevent_wait++ > 100)
        {
            if (hr.model.testMode == 0) // TEST MODE는 디버깅을 위한 모드이기 때문에 디버깅 편의를 위해 RESET 기능을 방지한다.
            {
                // IO_FETAllOff();         //! FET 제어 주체 변경(MCU -> RAJ)으로 인해 추가됨.
                // hr.model.Cfet = SW_OFF; //! FET 제어 주체 변경(MCU -> RAJ)으로 인해 추가됨.
                // hr.model.Dfet = SW_OFF; //! FET 제어 주체 변경(MCU -> RAJ)으로 인해 추가됨.
                RAJ_FETOFF();
#if defined(WD_MCU_RESET)
                MAINMCU_RESET_ON;
                hTimDelay(100);
                MAINMCU_RESET_OFF;
                hTimDelay(1000);
#endif
            }
            mainmcu_wd_start = 0;
            mainmcu_wcdog_event =0;
            mainmcu_wdevent_wait =0;
        }
    }
}
void Task_KeepAlive_Run(void)
{
    KeppAlive_Signal_Toggle();
}

//! FET 제어 주체 변경(MCU -> RAJ)으로 인해 추가됨.
void Task_UartMon_Run(void)
{
    // if (mainmcu_uart_wcdog_event == 1)
    // {
    //     mainmcu_uart_wcdog_event = 0;
    // }
}


void Task_CVT_Run(void)
{
    BYTE i;
#if 0
    /******************************************************
     * Battery 전압 측정
    ******************************************************/
    BAT_SENS_ENABLE;
    guage.ad_batvol = GetADVin();
    /******************************************************
     * Current 측정
    ******************************************************/
    ADC_Start();
    guage.ad_cur = ADC_Read();
    ADC_Stop();
#else
    if(f_curr_int == ON){
        CVT_Func();
        f_curr_int = OFF;
    }
    // Shunt 측정 방식으로 바뀌어야 한다.
    
#endif
    // Cell 전압도 측정해야 한다.
    for(i=0;i<D_SERIES;i++){
        ir.model.cell[i]=guage.ad_cvol[i];
    }
    ir.model.temp1 = guage.ad_temp[0];
    ir.model.temp2 = guage.ad_temp[1];
    ir.model.temp3 = guage.ad_temp[2];
    ir.model.ad_batvol = guage.ad_batvol;
    ir.model.ad_packvol = guage.ad_packvol;
    ir.model.ad_cur = guage.ad_cur;
}


#if 0
DWORD stl_offset;
void Task_Stl_Run(void)
{
    static BYTE step= 5;
    uint16_t mem_backup;
    DI();
    switch(step){
        case 0:
            g_resultBool = stl_RL78_registertest(); 
            step=1;
            break;
        case 1:
            g_resultBool = stl_RL78_registertest_cs(); 
            step=2;
            break;   
        case 2:
            g_resultBool = stl_RL78_registertest_psw(); 
            step=3;            
            break;
        case 3:
            g_resultBool = stl_RL78_registertest_stack(); 
            step=4;            
            break;
        case 4:
            g_resultBool = stl_RL78_registertest_es(); 
            step=5;            
            break;
        case 5:
            //해당 코드는 JTAG 물리는 경우 정상 실행되고 뽑으면 죽기 때문에 해결될때 까지 실행 하지 않는다.
            //jae.kim 2023.10.24
            //Start：0x000fe300
            //END 0x000ffee0
#if 1	    
            g_pMC_StAdr = (unsigned char __far *) START_ADDR+stl_offset;   /* Set RAM start address */
            /* Execute March C test */
            mem_backup = *g_pMC_StAdr;
            if((uint32_t)g_pMC_StAdr >= (uint32_t)END_ADDR ){
                g_pMC_StAdr = (unsigned char __far *)START_ADDR; stl_offset = 0;
            }
            g_resultBool = stl_RL78_march_c((unsigned char *)g_pMC_StAdr, 1);
            stl_offset +=1;
            *g_pMC_StAdr = mem_backup;
#else
    g_pMC_StAdr = (unsigned char __far *)RamTestSampleBuffer;  /* Set RAM start address */
    g_RAM_MarchC_lgth = sizeof(RamTestSampleBuffer);           /* Set RAM test length (= end adress - start address */

    /* Execute March C test */
    g_resultBool = stl_RL78_march_c((unsigned char *)g_pMC_StAdr, (unsigned short)g_RAM_MarchC_lgth);
#endif
            step=5;
            break;
    }
    EI();
}

#endif
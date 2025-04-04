/**
 ******************************************************************************
 *     @file         Cellbalancing.c
 *     @version   
 *     @author    
 *     @date      2024/09/20 
 *     @brief     
 ******************************************************************************
*/
#include "main.h"
#include "obshal.h"
#include "bms.h"
#include "BMS_OS.h"
#include "cellbalancing.h"
#include "master.h"
#include <stdint.h>

//-----Balancing Start_Voltage, Deviation
#define MIN_BALANCING_TARGET_V 3208			// mV (10%)
#define VMAXBAL 3348	    // mV (95%)
#define BALANCING_COLTROL_RANGE 15				// mV
#define V_MAX_HYST 255          // mV
#define OCV_ALLOW_mA   1000

#define OCV_MS_TIME 1000*60*1 //20      //default 20minute

#define AUTOBAL_DOIT_CELLDEV   10 // 
#define AUTOBAL_STOP_CELLDEV    5 //

#define BACKGROUND_DO_CELLDEV 300
#define AUTOBAL_PROHIBIT_CELLMin    3000
#define AUTOBAL_PROHIBIT_CELLMax    4000
#define AUTOBAL_1mV_TAKETIME     1000*60*60  //1hour 에 1mv 간주 
#define TARGETV_INIT 5000
#define DFAULT_ONBALTIMESEC 60 //600    	// 10 min
#define DEFAULT_RESTTIMESEC 30     // 30s
#define DEFAULT_MEASURETIMESEC 30 	// 30s
#define DEFAULT_ACTIONRESTMS 500   	// 500ms
uint32_t FaultData1, FaultData2;

uint32_t endit_mV=AUTOBAL_STOP_CELLDEV;

GLOBAL STR_BalancingModel Balancing;
GLOBAL uint8_t BalancingCMD ;  //Balancing Command [ start, stop, 수행 후 에 clear 된다.
GLOBAL uint8_t AutoBalancingMode ;  // 셀 편차가 20mV 이상 생기면 자동으로 Balancing Command가 Start 되도록 
uint8_t on_balancing;   //balancing 중이면 1, balancing이 끝나는 경우 0이 된다. 
uint32_t ocv_config_time;    // OCV CHECK 설정 시간 
uint32_t measure_celldev = 0;
uint64_t bal_start_time;
uint8_t AutoCellBalancingOperated;
uint64_t tag_time;
uint32_t autobal_st_time;
uint32_t doit_mV = AUTOBAL_DOIT_CELLDEV;


uint16_t TargetV_Check(uint16_t T_Voltage);

PUBLIC void Init_Balancing(void)
{
	//uint16_t i,j;
    ocv_config_time = OCV_MS_TIME;
	Balancing.TargetV.all=TARGETV_INIT;
	Balancing.BalancingStatus=BALANCING_COMPLETE;
	Balancing.BalancingHystrisis=AUTOBAL_STOP_CELLDEV;
	Balancing.T_OnBalTime = DFAULT_ONBALTIMESEC* 1000;
	Balancing.T_RestTime=DEFAULT_RESTTIMESEC *1000;
	Balancing.T_MeasureTime=DEFAULT_MEASURETIMESEC *1000;
	Balancing.T_ActionRestTime=DEFAULT_ACTIONRESTMS;

    FaultData1 = STREV_OVTEMP_ALARM|STREV_UNTEMP_ALARM|STREV_OVCHAR_CUR_ALARM|
STREV_OVDSCH_CUR_ALARM|STREV_OVVOL_ALARM|STREV_UNVOL_ALARM|
                 STREV_UNSOC_ALARM|STREV_OVSOC_ALARM|STREV_TEMP_IMBAL_ALARM|
STREV_CONTACTOR_ERROR|STREV_GND_FAULT ;
    FaultData2 = STREXTEV_OVCVOL_ALARM | STREXTEV_UNCVOL_ALARM | 
STREXTEV_OCD_ALARM ;
    AutoBalancingMode = TRUE;
}


PUBLIC void PerformBalancing(uint8_t ch)
{

	uint16_t MaxV;
	uint16_t BalTargetV;
	uint16_t MaxVMinusTargetV;
	uint16_t BlockVOK;
    uint32_t tms;
    //int i,j, temp_stop;
    uint8_t id;
    //uint16_t mask;
    uint8_t offset = 0; // channel 1개만 현재 구현 .
    //uint8_t n = TRAY_NUMS ;
    MaxV=R.StringModel.CellVMax;
    if(MaxV  < MIN_BALANCING_TARGET_V ) MaxV =MIN_BALANCING_TARGET_V;

    BalTargetV=TargetV_Check(Balancing.TargetV.all);	    						// Target V Check
    BlockVOK=1;

    //--- Deviation Calculation & Check
    if(Balancing.BalancingStatus == CELLBALANCING)
    {
        MaxVMinusTargetV = 5000;
    }else
    {
        MaxVMinusTargetV=(MaxV>BalTargetV)? MaxV -BalTargetV : BalTargetV - MaxV;
    }


    //--- Status Decision Logic
    if( BlockVOK && (MaxVMinusTargetV > endit_mV ) &&( MaxV > BalTargetV))
    {

        switch(Balancing.BalancingStatus)
        {
            case BALANCING_COMPLETE:
            	Balancing.tagtime = hTimCurrentTime();
            	Balancing.BalancingStatus = ACTION_REST;
                hLogProc(LOG_CBAL, "trya bal status -> rest\n");
                break;
            case ACTION_REST:
            	tms = (uint32_t)(hTimCurrentTime()-Balancing.tagtime);
            	if(tms >= Balancing.T_ActionRestTime)     // ms
            	{
            		Balancing.tagtime = hTimCurrentTime();
            		Balancing.balancingOnSubStage=0;
            		Balancing.BalancingStatus = CELLBALANCING;
                    hLogProc(LOG_CBAL, "traybal status -> cellbal\n");
            	}
                break;
            case CELLBALANCING:
            	if(Balancing.balancingOnSubStage==CELL_SELECT_STAGE)   // cell select , Watchdog Enable
            	{
            	    hLogProc(LOG_CBAL, "tray[%d] watch enable\n", offset+id);
                    bmic_balsw_on = 1;
                    PushJob( SUBMIC_SCHEDULER_IDX, SUBMIC_CMD_CELLBAL, 0, 0);
                    Balancing.balancingOnSubStage=B_WATCHDOG_CHECK_STAGE;
                    hLogProc(LOG_CBAL, "tray bal status -> watchdog check\n");
            	}  else if(Balancing.balancingOnSubStage==B_WATCHDOG_CHECK_STAGE)  // time and watchdog check
            	{

                    tms = (uint32_t)(hTimCurrentTime()-Balancing.tagtime);
                    if( (tms >= Balancing.T_OnBalTime)  )
                    {
                        
                        hLogProc(LOG_CBAL, "stop balancing\n" );
                        bmic_balsw_on = 0;
                        PushJob( SUBMIC_SCHEDULER_IDX, SUBMIC_CMD_CELLBAL, 0, 0);
                        //Tray[id].Module[i].Cells[j].OnBalancing=0;
                        //BMIC_BAL_StopCellBalancingAll(ch);
                        Balancing.tagtime=hTimCurrentTime();
                        Balancing.balancingOnSubStage=STEADY_STAGE;
                        hLogProc(LOG_CBAL, "tray bal status ->steady stage\n");

                    }else{
            			//read watchdog
            			//BMIC_Read_Balancing_WatchDogTimer(ch);
                        //jae.kim
                        //보드 내부 온도도 감안해서 너무 뜨거워 지면 Cellbalancing을 멈춘다. 
                        // 온도 감시에 의한 Watdog timer 를 멈추게 하는 부분 구현 필요.
                    }

            	}
                
            	else if(Balancing.balancingOnSubStage==STEADY_STAGE)
            	{
            		
                    tms = (uint32_t)(hTimCurrentTime()-Balancing.tagtime);
                    if(tms >= 500 )
                    {
                        Balancing.tagtime = hTimCurrentTime();
                        Balancing.BalancingStatus = REST;
                    }

            	}
                break;
            case REST:
                tms = (uint32_t)(hTimCurrentTime()-Balancing.tagtime);
                if(tms < 100)
                {

                    if(Balancing.balancingOnSubStage==B_WATCHDOG_CHECK_STAGE) //Watchdog  Operation
                    {
                        Balancing.balancingOnSubStage=CELL_SELECT_STAGE;
                    }
                    else
                    {
                        Balancing.balancingOnSubStage=CELL_SELECT_STAGE;
                    }
                }
                
                if(tms >= Balancing.T_RestTime)//30s
                {
                    hLogProc(LOG_CBAL, "tray rest end\n");
                    Balancing.tagtime = hTimCurrentTime();
                    Balancing.BalancingStatus = MEASUREMENT;
                    hLogProc(LOG_CBAL, "tray bal status -> mesurement\n");
                }
                
                break;
            case MEASUREMENT:
                tms = (uint32_t)(hTimCurrentTime()-Balancing.tagtime);
                if(tms >= Balancing.T_MeasureTime )//30s
                {
                    hLogProc(LOG_CBAL, "tray  measure end\n" );
                    Balancing.tagtime = hTimCurrentTime();
                    Balancing.BalancingStatus = ACTION_REST;
                    hLogProc(LOG_CBAL, "tray bal status -> Action Rest\n");
                }

                break;
        }

    }
        /* Balancing 명령은 받았으나 Target Balance Voltage 보다 Max Cell값이 작은 경우 아래 구간을 수행 한다. */
    else
    {
        if(Balancing.BalancingStatus == CELLBALANCING)
        {
            hLogProc(LOG_CBAL, "tray[%d] stop balancing\n", offset+id);
            
        }
        //AutoCellBal time이 존재한다면 지워라.
        Balancing.BalancingStatus=BALANCING_COMPLETE;
    }
    //}

}

PUBLIC void StopCellBalancing(uint8_t ch){
    //uint8_t id,i,j;
    //uint8_t offset = 0; // channel 1개만 현재 구현 .
    //uint8_t n = TRAY_NUMS ;

    hLogProc(LOG_CBAL, "stop balancing\n");

    hLogProc(LOG_CBAL, "bal init\n");
    Init_Balancing();
}

//---- Target V Validation
uint16_t TargetV_Check(uint16_t T_Voltage)
{
	uint16_t TempTarget;

	if( MIN_BALANCING_TARGET_V <= T_Voltage &&  T_Voltage <= VMAXBAL)
	{
		TempTarget=T_Voltage;// Do Nothing
	}else if(MIN_BALANCING_TARGET_V > T_Voltage )
	{
		TempTarget=MIN_BALANCING_TARGET_V;
	}else
	{
		TempTarget=VMAXBAL;
	}
	return TempTarget;
}
static void clearTimeRecord(void)
{
       autobal_st_time = 0;
       tag_time = 0;
 }

static void doStoppingCellBal(void)
{
    BalancingCMD = BALANCING_STOP_CMD;
    bal_start_time = 0;
}

PUBLIC void runAutoBalancingProcess(void)
{
    //uint32_t  i;
        // Balancing 중에 있다면 
    if( on_balancing == TRUE ) {
        if( AutoCellBalancingOperated == TRUE)  //Auto Balancing 중이냐?
        {
            if( BatCurr_CAN > OCV_ALLOW_mA  || BatCurr_CAN < (-1*OCV_ALLOW_mA) ){
                //cellbal 중에 전압을 측정하면 왜곡된 셀편차가 나오기 문에
                //최조 측정시 셀편차를 기준으로 체크
                if( measure_celldev >= BACKGROUND_DO_CELLDEV ){
                    //startBGBalancingProcess();
                 }
                 doStoppingCellBal();
            }
            if ( (R.StringModel.Evt1 & FaultData1) || (R.StringModel.Evt2 & FaultData2))
            {
                doStoppingCellBal();
            }
            clearTimeRecord();
        }else{
            // Manual 방식으로 Balancing 중인 경우 OCV time 체크 안한다. 
            clearTimeRecord();
        }
        return;
    }

    // 밸런싱이 일어나지 않을 여기로 들어온다.

    if( tag_time == 0){
        tag_time = hTimCurrentTime();
        return;
    }

    if(AutoCellBalancingOperated == FALSE){
        autobal_st_time = hTimCurrentTime()-tag_time;
    }
    
    if(autobal_st_time > ocv_config_time ) {  //20 minute
        if( R.StringModel.CellVMax < AUTOBAL_PROHIBIT_CELLMax && R.StringModel.CellVMin > AUTOBAL_PROHIBIT_CELLMin  ){
            measure_celldev =R.StringModel.CellVMax - R.StringModel.CellVMin;

            if( measure_celldev >= doit_mV ){
                AutoCellBalancingOperated = TRUE;
                BalancingCMD = BALANCING_START_CMD;
                Balancing.TargetV.all = R.StringModel.CellVMin;
                bal_start_time = hTimCurrentTime();
            }else{  //Cell 편차가 미흡하면 ocv_time을 reset한다.
                tag_time = 0;
            }
        }
    }else{
        if( BatCurr_CAN > OCV_ALLOW_mA  || BatCurr_CAN < (-1*OCV_ALLOW_mA) ){
            clearTimeRecord();
        }
    }
}



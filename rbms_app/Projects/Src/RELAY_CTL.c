/**
 ******************************************************************************
 *     @file         RELAY_CTL.c
 *     @version   
 *     @author    
 *     @date      2024/09/19 
 *     @brief     
 ******************************************************************************
*/
#include "main.h"
#include "obshal.h"
#include "bms.h"
#include "rack_sys.h"
#include "master.h"

#define RLY_N_DELAY 0U


typedef enum _retCodes{ 
  Okay = 0, 
  Repeat, 
  Open, 
  Fail
}retCodes;
  
typedef enum {
  RLY_NONE = 0,
  RLY_CONNECT = 1,
  RLY_DISCONNECT = 2
}RlyConCommand;


typedef enum _RlyStateCodes{
  RLY2_OPEN, 
  RLY2_STARTED, 
  RLY2_N_WAIT ,
  RLY2_N_HOLD, 
  RLY2_P_WAIT, 
  RLY2_P_HOLD, 
  RLY2_CLOSED, 
  RLY2_N_OPEN_WAIT, 
  RLY2_P_OPEN_WAIT, 
  RLY2_FAULT
} RlyStateCodes;

typedef enum _FetStateCodes{
  FET_ALL_OFF           = 0, 
  FET_CFET_ON_DFET_OFF  = 0x01, 
  FET_CFET_OFF_DFET_ON  = 0x02,
  FET_ALL_ON            = 0x04,
  FET_FAULT             = 0x80
} FetStateCodes;

_RLY_CTL RLY_CTL;
RlyConCommand conCommand;
StrEnaCommand enaCommand;
//static uint64_t tClose= 0;
//static uint64_t tStart = 0;

PUBLIC void RELAY_CONTROL( uint32_t  mode)
{
    uint8_t enabled;
//    uint8_t ret;
    conCommand = (RlyConCommand)R.StringModel.SetCon;
    enaCommand = (StrEnaCommand)R.StringModel.SetEna;
    if( mode == RELAY_FORCE_OFF){
        hLogProc(LOG_RLY, "rly force off \n");
        CFET_OFF();
        DFET_OFF();
        R.StringModel.St &= ~(1 << STRING_STATUS_ENABLE_BIT);
        R.StringModel.St &= ~(1 << STRING_STATUS_RELAY_BIT);
        RLY_CTL.st = FET_ALL_OFF;
        return;
    }
    
    enabled = (R.StringModel.St & (1 << STRING_STATUS_ENABLE_BIT))  != 0;
    if (!enabled){
        // Enable 안된 상태에서 Enable 신호가 오면... Flag 만 Set 한다.
        // only accpet enable command
        if (enaCommand == STRING_ENABLE){
          R.StringModel.St |= (1 << STRING_STATUS_ENABLE_BIT);
        }
        return;
    }
    // Disable 명령이 오면, RELAY가 Open 상태이거나, Fault 상태에서는 St 만 업데이트 한다.
    if (enaCommand == STRING_DISABLE){
        if( RLY_CTL.type == RLY_2CH ||  RLY_CTL.type == RLY_3CH )
        {
            if (RLY_CTL.st == RLY2_OPEN || RLY_CTL.st == RLY2_FAULT){
                R.StringModel.St &= ~(1 << STRING_STATUS_ENABLE_BIT);
                R.StringModel.St &= ~(1 << STRING_STATUS_RELAY_BIT);
                return;
            }
        }else if( RLY_CTL.type == FET_2CH ||  RLY_CTL.type == FET_3CH )
        {
            if(RLY_CTL.st == FET_ALL_OFF || RLY_CTL.st == FET_FAULT ){
                R.StringModel.St &= ~(1 << STRING_STATUS_ENABLE_BIT);
                R.StringModel.St &= ~(1 << STRING_STATUS_RELAY_BIT);
                return;
            }
        }
    } 
    
    // 이곳으로 실행 된것은 RLY가 ON 되어 있는 상태
    /***********************************************************
    FET type 구현 
    ***********************************************************/   
    if( RLY_CTL.type == FET_2CH ||  RLY_CTL.type == FET_3CH)
    {
        switch(mode)
        {
            case CHG_ONLY_MODE:
                if(conCommand == STRING_DISABLE || enaCommand == STRING_DISABLE){
                    hLogProc(LOG_RLY, "rly off \n");
                    CFET_OFF();
                    DFET_OFF();
                    R.StringModel.St &= ~(1 << STRING_STATUS_RELAY_BIT);
                    RLY_CTL.st = FET_ALL_OFF;  
                }
                else if( conCommand == STRING_ENABLE){
                    hLogProc(LOG_RLY, "charg rly on \n");
                    CFET_ON();
                    DFET_OFF();
                    RLY_CTL.st = FET_CFET_ON_DFET_OFF;
                    R.StringModel.St |= (1<<STRING_STATUS_RELAY_BIT);
                }
                break;
            case DIS_ONLY_MODE:
                if(conCommand == STRING_DISABLE || enaCommand == STRING_DISABLE){
                    hLogProc(LOG_RLY, "rly off \n");
                    CFET_OFF();
                    DFET_OFF();
                    R.StringModel.St &= ~(1 << STRING_STATUS_RELAY_BIT);
                    RLY_CTL.st = FET_ALL_OFF;  
                }
                else if( conCommand == STRING_ENABLE){
                    hLogProc(LOG_RLY, "dicharge rly on \n");
                    CFET_OFF();
                    DFET_ON();
                    RLY_CTL.st = FET_CFET_ON_DFET_OFF;
                    R.StringModel.St |= (1<<STRING_STATUS_RELAY_BIT);
                }
                break;
            case ALL_MODE:
            case RELAY_NORMAL:
                if(conCommand == STRING_DISABLE || enaCommand == STRING_DISABLE ){
                    hLogProc(LOG_RLY, "rly off \n");
                    CFET_OFF();
                    DFET_OFF();
                    R.StringModel.St &= ~(1 << STRING_STATUS_RELAY_BIT);
                    RLY_CTL.st = FET_ALL_OFF;  
                }else if ( conCommand == STRING_ENABLE){
                    hLogProc(LOG_RLY, "all rly on \n");
                        CFET_ON();
                        DFET_ON();
                        RLY_CTL.st = FET_ALL_ON;
                        R.StringModel.St |= (1<<STRING_STATUS_RELAY_BIT);
                }
                break;
        }
    }
}
/**
   *    @brief Relay가 On 되어 있는 상태면 1, Relay가 Off된 상태이면 0
   **/
PUBLIC int32_t getRelayOnOffStatus(void)
{
    if( R.StringModel.St & (1 << STRING_STATUS_RELAY_BIT) )
    {
        return 1;
    }else
        return 0;
}
   

PUBLIC void processRelayOffFromDTC(void)
{
    R.StringModel.SetEna = 1;
    R.StringModel.SetCon = 2;
}


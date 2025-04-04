/**
    ******************************************************************************
    *     @file         hcan.c 
    *     @author    SKI ESS Team
    *     @version   V1.0
    *     @date       2014.11.05 
    *     @brief       HAL CAN Module Source
    ******************************************************************************
    */
#include "obshal.h"
#include "main.h"
#include <string.h>
#include <stdint.h>

/** @addtogroup HAL
  * @{
  */
  
  /**
      * @brief 이 소스는 Can  Device를 포팅한 모듈이다.
      * Can 모듈은 포트가 최대 2개이며, CAN 2.0B 버전을 default 로 하여 포팅 되어 있다.
      * TX는 인터럽트를 사용하지 않고 Blocking 함수 이지만, Device의 Mail-box가 full 인 경우
      * 즉각 리턴하기 때문에 내부적으로 대기 하는 시간은 없다.
      * RX의 경우는 비동기적인 인터럽트 방식을 지원한다. RX 인터럽트가 발생한 경우 
      * Call-back 함수를 호출하게 되고 이 함수내에서 수신 데이터를 처리 해 주도록 구현 되어 있다.
      */
  /** @addtogroup HAL_Can
    * @{
    */
    
         /** @defgroup HAL_Can_Private_Types
                * @{
                */

#define CAN_RX_BUF_SIZE         20        
#define CAN_INITIALIZATION             1
#define CAN_NORMAL                     2
#define CAN_SLEEP                      0
#define CAN_SILENT                     3
#define CAN_LOOPBACK                    4
#define CAN_SILENT_AND_LOOPBACK         5
#define CAN_DEBUG                       6

typedef struct _BHCan{
    CAN_HandleTypeDef CanHandle;
    CAN_TxHeaderTypeDef txHead;
    CAN_RxHeaderTypeDef rxHead;
    uint8_t txData[8];
    int32_t baudrate;
    int32_t debug; //Debug port
    int32_t rx_count;
    hCanMessage rxBuf[CAN_RX_BUF_SIZE];
    int32_t rx_front ;
    int32_t rx_rear ;
}BHCan;

        /**
              * @}
              */

        /** @defgroup HAL_Can_Private_Defines
           * @{
           */

        /**
              * @}
              */
        
        /** @defgroup HAL_Can_Private_Variables
               * @{
               */
BHCan can_fd[3];
        /**
            * @}
            */


/**
  * @brief CAN Module을 초기화 한다.
  * @param NONE
  * @retval NONE
  */
void hCanInit(void){
}

__weak void Cana_RxSwi(void)
{
}

__weak void Canb_RxSwi(hCanMessage  *msg)
{
   
}

/***
   baudrate =  (can clock (normal =36Mhz) / (prescale * (1+BS1+BS2))
*/

static int32_t  set_baudrate(  BHCan *can, int32_t baudrate , int sjw, int seg1,int seg2   )
{
    uint32_t PCLK1;
    switch(sjw){
        case 1:
            can->CanHandle.Init.SyncJumpWidth = CAN_SJW_1TQ; //CAN_SJW_1TQ; 
            break;
        case 2:
            can->CanHandle.Init.SyncJumpWidth = CAN_SJW_2TQ; //CAN_SJW_1TQ; 
            break;
        case 3:
            can->CanHandle.Init.SyncJumpWidth = CAN_SJW_3TQ; //CAN_SJW_1TQ; 
            break;
        case 4:
            can->CanHandle.Init.SyncJumpWidth = CAN_SJW_4TQ; //CAN_SJW_1TQ; 
            break;
        default:
            break;
    }
    switch(seg1){
        case 1:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_1TQ;//CAN_BS1_3TQ;
            break;
        case 2:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_2TQ;//CAN_BS1_3TQ;
            break;
        case 3:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_3TQ;//CAN_BS1_3TQ;
            break;
        case 4:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_4TQ;//CAN_BS1_3TQ;
            break;
        case 5:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_5TQ;//CAN_BS1_3TQ;
            break;
        case 6:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_6TQ;//CAN_BS1_3TQ;
            break;
        case 7:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_7TQ;//CAN_BS1_3TQ;
            break;
        case 8:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_8TQ;//CAN_BS1_3TQ;
            break;
        case 9:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_9TQ;//CAN_BS1_3TQ;
            break;
        case 10:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_10TQ;//CAN_BS1_3TQ;
            break;
        case 11:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_11TQ;//CAN_BS1_3TQ;
            break;
        case 12:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_12TQ;//CAN_BS1_3TQ;
            break;
        case 13:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_13TQ;//CAN_BS1_3TQ;
            break;
        case 14:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_14TQ;//CAN_BS1_3TQ;
            break;
        case 15:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_15TQ;//CAN_BS1_3TQ;
            break;
        case 16:
            can->CanHandle.Init.TimeSeg1 = CAN_BS1_16TQ;//CAN_BS1_3TQ;
            break;
        default:
            break;
    }
    switch(seg2){
        case 1:
            can->CanHandle.Init.TimeSeg2 = CAN_BS2_1TQ;//CAN_BS1_3TQ;
            break;
        case 2:
            can->CanHandle.Init.TimeSeg2 = CAN_BS2_2TQ;//CAN_BS1_3TQ;
            break;
        case 3:
            can->CanHandle.Init.TimeSeg2 = CAN_BS2_3TQ;//CAN_BS1_3TQ;
            break;
        case 4:
            can->CanHandle.Init.TimeSeg2 = CAN_BS2_4TQ;//CAN_BS1_3TQ;
            break;
        case 5:
            can->CanHandle.Init.TimeSeg2 = CAN_BS2_5TQ;//CAN_BS1_3TQ;
            break;
        case 6:
            can->CanHandle.Init.TimeSeg2 = CAN_BS2_6TQ;//CAN_BS1_3TQ;
            break;
        case 7:
            can->CanHandle.Init.TimeSeg2 = CAN_BS2_7TQ;//CAN_BS1_3TQ;
            break;
        case 8:
            can->CanHandle.Init.TimeSeg2 = CAN_BS2_8TQ;//CAN_BS1_3TQ;
            break;
        default:
            break;
    }    
   // can->CanHandle.Init.Prescaler = 27;
    PCLK1 = HAL_RCC_GetPCLK1Freq();
    can->CanHandle.Init.Prescaler  =PCLK1/baudrate/(1+seg1+seg2); //6 = 1+3T+2T  
    switch(baudrate){
        case 1000000:
            break;
        case 500000:
        case 250000:
        case 125000:
            break;
        default:
            return E_ERROR;
    }
    return 0;
}

/**
  * @brief CAN의 포트를 주어진 전송속도로 Open 한다.
  * @param[in] port CAN 포트 문자열 이름, "CANA", "CANB", "CANC", ..
  * @param[in] baudrate 전송속도 -  250000(250kbps), 500000(500kbps), 1000000 (1Mbps)
  * @retval  0 이상값:  CAN 포트의 식별자 
  * @retval  음수: 에러정보 
  */
int32_t hCanOpen(char *port, int32_t baudrate, int32_t sjw, int32_t seg1, int32_t seg2)
{
  
    CAN_FilterTypeDef  sFilterConfig;
    BHCan *can;

    if(strncmp (port, "CANB",4)==0){
        can = &can_fd[0];
        memset( can, 0x0, sizeof(struct _BHCan) );    
        //##-1- Configure the CAN peripheral #######################################
        can->CanHandle.Instance = CANxB;
        can->CanHandle.Init.TimeTriggeredMode = DISABLE;
        can->CanHandle.Init.AutoBusOff = ENABLE ;  
        can->CanHandle.Init.AutoWakeUp = DISABLE;   
        can->CanHandle.Init.AutoRetransmission = ENABLE;
        can->CanHandle.Init.ReceiveFifoLocked = DISABLE;
        can->CanHandle.Init.TransmitFifoPriority = DISABLE;
        can->CanHandle.Init.Mode = CAN_MODE_NORMAL;
        //HAL_CAN_MspInit(&can->CanHandle);
        if( set_baudrate(can, baudrate,sjw,seg1,seg2) < 0) {
            return E_ERROR;
        }
        if( HAL_CAN_Init(&can->CanHandle ) != HAL_OK){
            return E_ERROR;
        }        

			//	HAL_CAN_ConfigFilter( &can->CanHandle, &sFilterConfig);
				

				sFilterConfig.FilterBank = 0;
				sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
				sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
				sFilterConfig.FilterIdHigh = 0;
				sFilterConfig.FilterIdLow = 0;
				sFilterConfig.FilterMaskIdHigh =  0;  
				sFilterConfig.FilterMaskIdLow = 0;
				sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;  //FIFO 0???ъ슜?쒕떎.
				sFilterConfig.FilterActivation = ENABLE;
				sFilterConfig.SlaveStartFilterBank = 0;		
				HAL_CAN_ConfigFilter( &can->CanHandle, &sFilterConfig);
					
	
        //##-2- Configure the CAN Filter ###########################################
        /** ?ν썑 14~27踰덇퉴吏 14媛??ъ슜媛?ν븯?? */
/*    
        sFilterConfig.FilterBank = 14;  //14踰??섎굹留??ъ슜?쒕떎.
        sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
        sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
        sFilterConfig.FilterIdHigh = 0x0000;
        sFilterConfig.FilterIdLow = 0x0000;
        sFilterConfig.FilterMaskIdHigh = 0x0000;
        sFilterConfig.FilterMaskIdLow = 0x0000;
        sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1; //FIFO1???ъ슜?쒕떎.
        sFilterConfig.FilterActivation = ENABLE;

        HAL_CAN_ConfigFilter(&can->CanHandle, &sFilterConfig);
*/

        //##-3- Configure Transmission process #####################################
        can->rxHead.StdId = 0x321;
        can->rxHead.ExtId = 0x01;
        can->rxHead.RTR = CAN_RTR_DATA;
        can->rxHead.IDE = CAN_ID_STD;
        can->rxHead.DLC = 2;
        HAL_CAN_ActivateNotification( &can->CanHandle, CAN_IT_RX_FIFO0_MSG_PENDING ) ;
    }
    else
    {
        return E_BADID;
    }
    HAL_CAN_Start( &can->CanHandle);
    return (int32_t)can;  
}

int32_t hCanAvailable(int32_t fd)
{
    BHCan * can=(BHCan *)fd;
	  if(can == NULL) return -1;
    return can->rx_count;  
}

/**
  * @brief CAN 포트의 설정을 변경한다.
  * hCanCtrlOpt type의 옵션을 제공한다.
  * @param[in] fd CAN 포트의 식별자
  * @param[in] opt 옵션값
  * @param[in] param 옵션의 파라메터 값이다. 
  *         CAN_SET_BAUDRATE 의 경우 250000(250kbps), 500000(500kbps), 1000000 (1Mbps)
  * @retval 0: 성공
  * @retval 음수 : 에러정보
  */
int32_t hCanCtrl(int32_t fd, int32_t opt, int32_t param)
{
    BHCan * can=(BHCan *)fd;
    switch(opt){
          case CAN_SET_BAUDRATE:
            if( set_baudrate(can, param,(int)NULL,(int)NULL,(int)NULL) < 0) return E_ERROR;
            if( HAL_CAN_Init( (CAN_HandleTypeDef *)can) != HAL_OK){
                return E_ERROR;
            }        
            break;
          default:
            break;
    }
    return E_NOTSUP;
}

/**
  * @brief CAN 포트로 주어진 버퍼를 전송한다.
  * 버퍼는 hCanMessage type 이다. 디바이스의 mailbox 가 full 이 된경우
  * E_INPROGRESS 에러값을 반환 할 것이다.
  * @param[in] fd CAN 식별자 
  * @param[in] buf 전송 할 데이터의 버퍼 포인터
  * @param[in] len 버퍼의 사이즈
  * @retval 0이상값 : 전송 된 버퍼의 사이즈
  * @retval 음수: 에러정보
  */
int32_t hCanWrite(int32_t fd, void *buf, int32_t len)
{
    BHCan * can=(BHCan *)fd;
    hCanMessage *msg = (hCanMessage *) buf;    
    HAL_StatusTypeDef  status;
    int i;
    uint32_t mailbox;
    if(can == NULL || msg->len > 8 || msg->len ==0 ) return E_ERROR;
    for(i=0;i<8 ;i++){
        if(i< msg->len){
            can->txData[i] = msg->data[i];
        }else{
            can->txData[i]=0xff;
        }
    }
		
    if(msg->id > 0x7FF){
			can->txHead.ExtId = msg->id;
			can->txHead.IDE = CAN_ID_EXT;
			can->txHead.StdId = 0;
		}
		else{
			can->txHead.StdId = msg->id;
			can->txHead.IDE = CAN_ID_STD;
			can->txHead.ExtId  = 0;
		}
		
    can->txHead.DLC = msg->len;
    mailbox=0;
    status = HAL_CAN_AddTxMessage(&can->CanHandle, &can->txHead, can->txData, &mailbox);
    if(status != HAL_OK){
        if(!mailbox ){ //Mail Box FUll return value;
            return E_INPROGRESS;
        }else{
            return E_ERROR;
        }
    }
    return len;
}

/**
  * @brief CAN 포트로 데이터를 수신한다.
  * 이 함수는 non-blocking 함수이다. 만일 CAN 포트에 버퍼에 수신된 데이터가 없는 경우
  * 즉각 반환되며, 리턴값은 0 이다.
  * 버퍼는 hCanMessage type 단위로 할당 해야 된다.
  * @param[in] fd CAN 포트 식별자
  * @param[out] buf 버퍼의 포인터
  * @param[in] len 버퍼의 사이즈
  * @retval 0이상값: 수신 데이터의 사이즈 
  * @retval 음수: 에러정보
  */
int32_t hCanRead(int32_t fd, void *buf, int32_t len)
{
    int32_t readin =0;
    BHCan *can = (BHCan *)fd;;
    hCanMessage *msg = (hCanMessage *) buf;
    if(can->CanHandle.Instance == CANxB)
    {
        while(len){
            __disable_irq();
            if(can->rx_count){
                __enable_irq();
                memcpy( msg ,&can->rxBuf[can->rx_rear] ,sizeof( hCanMessage));
                can->rx_rear++;
                can->rx_rear = can->rx_rear % CAN_RX_BUF_SIZE;
                msg++;   readin++;    len--;
                __disable_irq();
                can->rx_count--;
                __enable_irq();            
            }else{
                __enable_irq();
                return readin;
            }
        }
        return readin;
    }
    return -1;    
}

/**
  * @brief CAN 포트를 Close 한다.
  * @param [in] fd CAN 포트 식별자
  * @retval NONE
  */
void hCanClose(int32_t fd)
{
	BHCan * can=(BHCan *)fd;
	HAL_CAN_DeInit(&can->CanHandle);
}

 /**
  * @brief CAN 포트의 식별자 값을 얻어온다.
  * @param [in] port index, ex 0, 1
  * @retval 0 이상값 : 식별자 
  * @retval 음수 : 에러정보
  */ 
uint32_t hCanGetHandleFromID(uint8_t id){
  if ( id > 1 ){
     return 0;
  }
  return (uint32_t)&can_fd[id];
}
/**
  * @brief CAN 포트의 식별자 값을 얻어온다.
  * @param [in] port 포트의 문자열 이름 "CANA","CANB",..
  * @retval 0 이상값 : 식별자 
  */ 

uint32_t hCanGetHandle(char *port)
{
#if 1
    if(strncmp (port, "CANA",4)==0){
        return NULL;
    }else if(strncmp (port, "CANB",4)==0){
        return (uint32_t )&can_fd[0] ;
    }
#endif
    return(uint32_t) &can_fd[0];
}

// CAN 수가 2개 이상이라면 Instance 를 찾아서 mapping 하도록 한다.
// CANxn 이 1개 인경우는 can_fd[0] 로 fix
// Instance 가 일치 하지 않는 경우 에러 
uint32_t hCanGetHandleFromIsr(void *Instance){
    assert_param(Instance == CANxB);
    return (uint32_t)&can_fd[0];
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_RxCpltCallback could be implemented in the user file
   */
  // int i; 
   hCanMessage msg_buf;
   CAN_RxHeaderTypeDef rxHead;
   BHCan *can;
   if(hcan->Instance == CANxB)
   {
       rxHead.ExtId = rxHead.StdId = 0;
       HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHead, msg_buf.data) ;
       if( rxHead.StdId ){
           msg_buf.id = rxHead.StdId;
           msg_buf.len = rxHead.DLC;
           Canb_RxSwi(&msg_buf);
       }else{
           can= (BHCan *)hCanGetHandleFromIsr(hcan->Instance);
           memcpy(can->rxBuf[can->rx_front].data, msg_buf.data, 8);
           can->rxBuf[can->rx_front].id = rxHead.ExtId;	 		
           can->rxBuf[can->rx_front].len = rxHead.DLC;
           can->rx_front++;
           can->rx_front = can->rx_front % CAN_RX_BUF_SIZE;
           __disable_irq();
           can->rx_count++;
           __enable_irq();
       }
       hcan->State = HAL_CAN_STATE_READY;
       HAL_CAN_ActivateNotification( &can->CanHandle, CAN_IT_RX_FIFO0_MSG_PENDING ) ;
    }

}

    /**
      * @}
      */
// End of HAL Group
/**
  * @}
  */


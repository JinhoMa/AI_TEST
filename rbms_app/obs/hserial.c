#include "obshal.h"
#include "main.h"
#include <string.h>
#include <assert.h>
#define  USART_RX_BUF_SIZE 512
#define USART_TX_BUF_SIZE   (512)

#define MODE_UART       1
#define MODE_USART      2
//#define DBG_ITM


typedef struct _BHSerial{
    UART_HandleTypeDef handle;
    int baudrate;
    uint8_t mode;
    uint8_t rxInterrupt; //RX Interrupt Mode check 
    uint8_t debug; //Debug port
    uint8_t parity; //party 0-no partiy, 1-odd, 2-even
    uint8_t stopbit; //stop 0, 1, 2 bit
    uint8_t used;
    uint8_t *rxBuf;
    uint8_t *txBuf;
    uint32_t txFront;
    uint32_t txRear;
    uint32_t txLeft;
    uint32_t rxFront;
    uint32_t rxCount;
    uint32_t rxRear;
    void (*func)(int);
    
}BHSerial;

uint8_t rx_buf[COMxn][USART_RX_BUF_SIZE];
static uint8_t tx_buf[COMxn][USART_TX_BUF_SIZE];
static uint8_t rx_temp_buf[COMxn][2];
BHSerial serial_fd[COMxn];

static uint32_t debug_port_handle = 0;
static uint32_t shell_port_handle = 0;

void hSerInit(void){
}

int32_t hSerOpen(char *port, int32_t baudrate, void *asyncFunc )
{
    BHSerial *ser;
    if(strncmp (port, "COM1",4)==0){
        ser = &serial_fd[COMFD_1];
        memset(ser,0x0,sizeof(struct _BHSerial));
        ser->handle.Instance = COM1_Instance;
        ser->handle.Init.BaudRate = 115200;
        ser->handle.Init.WordLength = UART_WORDLENGTH_8B;			
        ser->handle.Init.StopBits = UART_STOPBITS_1;
        ser->handle.Init.Parity= UART_PARITY_NONE;
        ser->handle.Init.HwFlowCtl =UART_HWCONTROL_NONE;
        ser->handle.Init.Mode =UART_MODE_TX_RX;
        ser->handle.Init.OverSampling = UART_OVERSAMPLING_16;
        if( HAL_UART_Init(&ser->handle) != HAL_OK)
        {
        	/* Initialization Error */
        	return -1;
        }
			
        ser->rxBuf = &rx_buf[COMFD_1][0];
        ser->txBuf = &tx_buf[COMFD_1][0];
        ser->rxInterrupt = 1;
        ser->txFront=ser->txRear=ser->rxFront=ser->rxRear=ser->txLeft = 
        ser->rxCount =0;
        ser->mode = MODE_UART;
        ser->func =(void (*)(int) )asyncFunc;
        HAL_UART_Receive_IT(&ser->handle, &rx_temp_buf[COMFD_1][0] , 1 );
 
    }else if(strncmp( port, "COM2", 4) == 0){
        ser = &serial_fd[COMFD_2];
        memset(ser,0x0,sizeof(struct _BHSerial));
        ser->handle.Instance =COM2_Instance ;
        ser->handle.Init.BaudRate = baudrate;
        ser->handle.Init.WordLength = UART_WORDLENGTH_8B;			
        ser->handle.Init.StopBits = UART_STOPBITS_1;
        ser->handle.Init.Parity= UART_PARITY_NONE;
        ser->handle.Init.HwFlowCtl =UART_HWCONTROL_NONE;
        ser->handle.Init.Mode =UART_MODE_TX_RX;
        ser->handle.Init.OverSampling = UART_OVERSAMPLING_16;
        if( HAL_UART_Init(&ser->handle) != HAL_OK)
        {
        	/* Initialization Error */
        	return -1;
        }
			
        ser->rxBuf = &rx_buf[COMFD_2][0];
        ser->txBuf = &tx_buf[COMFD_2][0];
        ser->rxInterrupt = 1;
        ser->txFront=ser->txRear=ser->rxFront=ser->rxRear=ser->txLeft = 
        ser->rxCount =0;
        ser->mode = MODE_UART;
        ser->func =(void (*)(int) )asyncFunc;
        HAL_UART_Receive_IT(&ser->handle, &rx_temp_buf[COMFD_2][0] , 1 );
    }else{
        ser = &serial_fd[COMFD_3];
        memset(ser,0x0,sizeof(struct _BHSerial));
        ser->handle.Instance = COM3_Instance;
        ser->handle.Init.BaudRate = baudrate;
        ser->handle.Init.WordLength = UART_WORDLENGTH_8B;			
        ser->handle.Init.StopBits = UART_STOPBITS_1;
        ser->handle.Init.Parity= UART_PARITY_NONE;
        ser->handle.Init.HwFlowCtl =UART_HWCONTROL_NONE;
        ser->handle.Init.Mode =UART_MODE_TX_RX;
        ser->handle.Init.OverSampling = UART_OVERSAMPLING_16;
        if( HAL_UART_Init(&ser->handle) != HAL_OK)
        {
        	/* Initialization Error */
        	return -1;
        }
			
        ser->rxBuf = &rx_buf[COMFD_3][0];
        ser->txBuf = &tx_buf[COMFD_3][0];
        ser->rxInterrupt = 1;
        ser->txFront=ser->txRear=ser->rxFront=ser->rxRear=ser->txLeft = 
        ser->rxCount =0;
        ser->mode = MODE_UART;
        ser->func =(void (*)(int) )asyncFunc;
        HAL_UART_Receive_IT(&ser->handle, &rx_temp_buf[COMFD_3][0] , 1 );
    }
    
    if( strcmp(port, hSysProperty("shell.serial.port"))==0){ 
        shell_port_handle =(uint32_t) ser;
    }
    ser->used = TRUE;
    return (int32_t)ser;
}

int32_t hSerCtrl(int32_t fd, int32_t opt, int32_t param)
{
    BHSerial *ser = (BHSerial *)fd;

    switch(opt)
    {
        case SER_SET_DEBUG:
            ser->debug = param;
            if(param) debug_port_handle = fd;
            else debug_port_handle= 0;
            break;
        case SER_SET_RS485TXE:
            if(fd  == hSerGetHandle("COM2") ){
                if(param) BSP_COM_RS485TX_ENABLE();
                else  BSP_COM_RS485TX_DISABLE();
            }
            break;
        default:
            return E_NOTSUP;
    }
    return 0;
}

int32_t hSerWrite(int32_t fd, uint8_t *buf, int32_t len)
{
    int i ;
    int empty,wrout;
    BHSerial *ser = (BHSerial *)fd;
    // buffer가 st code상 round-robine 이 되지 않기 때문에  buffer 
    // 끝 까지 data를 채운후 intterupt 를 통해서 다 send 한 후 다시 시작 한다.
    if(ser->txLeft && !ser->txFront){
        return 0;
    }
    hSysLock();
    empty = USART_TX_BUF_SIZE - ser->txFront;
    hSysUnlock();
#if 1
    if(len > empty) return E_NOMEMORY;
    wrout =  ( len > empty )  ? empty : len;
    for(i=0; i<wrout ;i++){
        *(ser->txBuf+ser->txFront) = *(buf+i);
        ++ser->txFront;
        ser->txFront  = (ser->txFront) % USART_TX_BUF_SIZE;
        hSysLock();
        ser->txLeft ++;
        hSysUnlock();
        if(!ser->txFront)
            break;
    }
    if( (ser->handle.gState == HAL_UART_STATE_BUSY_TX )|| 
			  (ser->handle.gState == HAL_UART_STATE_BUSY_TX_RX))
		{ 
        return i;
    }
    if(ser->txLeft >= USART_TX_BUF_SIZE){   //when error
        ser->txRear =0;
        ser->txLeft=0;
        ser->txFront=0;
        return 0;
	}
    HAL_UART_Transmit_IT(&ser->handle, ser->txBuf+ser->txRear, ser->txLeft );

#else
    HAL_UART_Transmit(&ser->handle, buf, len, -1);
#endif
    return len;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	BHSerial *ser;
    if(huart->Instance == COM1_Instance){
        ser = (BHSerial *)hSerGetHandle("COM1");
        hSysLock();
        ser->txLeft -= ser->handle.TxXferSize;
        hSysUnlock();
        if(!ser->txLeft){
            if(ser->func) (*ser->func)(1);
            hSysLock();
            ser->txFront = ser->txRear = 0;
            hSysUnlock();
            if( ser->handle.gState == HAL_UART_STATE_READY){
                //jae.kim
                //HAL에서 TX 후에 state가 READY가 되는 경우 interrupt handle에서 수신 처리가 되지 않는다.
                HAL_UART_Receive_IT(&ser->handle, &rx_temp_buf[COMFD_1][0] , 1 );
            }
        }else{
            hSysLock();
            ser->txRear += ser->handle.TxXferSize;
            hSysUnlock();
            if(ser->txLeft > USART_TX_BUF_SIZE){
                ser->txLeft = 0;
                ser->txRear = 0;
                ser->txFront = 0;
                return;
            }
            HAL_UART_Transmit_IT(&ser->handle, ser->txBuf+ser->txRear, ser->txLeft );
        }
    }else
    if(huart->Instance == COM2_Instance){
        ser = (BHSerial *)hSerGetHandle("COM2");
        hSysLock();
        ser->txLeft -= ser->handle.TxXferSize;
        hSysUnlock();
        if(!ser->txLeft){
            if(ser->func) (*ser->func)(1);
            hSysLock();
            ser->txFront = ser->txRear = 0;
            hSysUnlock();
            if( ser->handle.gState == HAL_UART_STATE_READY){
                //jae.kim
                //HAL에서 TX 후에 state가 READY가 되는 경우 interrupt handle에서 수신 처리가 되지 않는다.
                HAL_UART_Receive_IT(&ser->handle, &rx_temp_buf[COMFD_2][0] , 1 );
            }
        }else{
            hSysLock();
            ser->txRear += ser->handle.TxXferSize;
            hSysUnlock();
            if(ser->txLeft > USART_TX_BUF_SIZE){
                ser->txLeft = 0;
                ser->txRear = 0;
                ser->txFront = 0;
                return;
            }
            HAL_UART_Transmit_IT(&ser->handle, ser->txBuf+ser->txRear, ser->txLeft );
        }
    }else
    if(huart->Instance == COM3_Instance){
        ser = (BHSerial *)hSerGetHandle("COM3");
        hSysLock();
        ser->txLeft -= ser->handle.TxXferSize;
        hSysUnlock();
        if(!ser->txLeft){
            if(ser->func) (*ser->func)(1);
            hSysLock();
            ser->txFront = ser->txRear = 0;
            hSysUnlock();
            if( ser->handle.gState == HAL_UART_STATE_READY){
                //jae.kim
                //HAL에서 TX 후에 state가 READY가 되는 경우 interrupt handle에서 수신 처리가 되지 않는다.
                HAL_UART_Receive_IT(&ser->handle, &rx_temp_buf[COMFD_3][0] , 1 );
            }
        }else{
            hSysLock();
            ser->txRear += ser->handle.TxXferSize;
            hSysUnlock();
            if(ser->txLeft > USART_TX_BUF_SIZE){
                ser->txLeft = 0;
                ser->txRear = 0;
                ser->txFront = 0;
                return;
            }
            HAL_UART_Transmit_IT(&ser->handle, ser->txBuf+ser->txRear, ser->txLeft );
        }
    }

}


#define min(x,y) x>y ? y: x;
int32_t hSerRead(int32_t fd, uint8_t *buf, int32_t len)
{
    int readin=0;   
    BHSerial *ser = (BHSerial *)fd;
    if( (readin = hSerAvailable(fd))==0){
        return 0;
    }
    len = min(len, readin);
    readin=0;
    hSysLock();
    while(len){
        *(buf++)=ser->rxBuf[ser->rxRear];
        ser->rxRear = (ser->rxRear+1)% USART_RX_BUF_SIZE;
        ser->rxCount--;
        len--;
        readin++;
    }
    hSysUnlock();
    return readin;

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == COM1_Instance){
        BHSerial *ser = (BHSerial *)hSerGetHandle("COM1");
        *(ser->rxBuf + ser->rxFront) = rx_temp_buf[COMFD_1][0];
        ser->rxFront = (ser->rxFront +1 )% USART_RX_BUF_SIZE;
        hSysLock();
        ser->rxCount ++;
        hSysUnlock();
        HAL_UART_Receive_IT(&ser->handle, &rx_temp_buf[COMFD_1][0] , 1 );
        if(ser->func) (*ser->func)(0);
    }else
    if(huart->Instance == COM2_Instance){
        BHSerial *ser = (BHSerial *)hSerGetHandle("COM2");
        *(ser->rxBuf + ser->rxFront) = rx_temp_buf[COMFD_2][0];
        ser->rxFront = (ser->rxFront +1 )% USART_RX_BUF_SIZE;
        hSysLock();
        ser->rxCount ++;
        hSysUnlock();
        HAL_UART_Receive_IT(&ser->handle, &rx_temp_buf[COMFD_2][0] , 1 );
        if(ser->func) (*ser->func)(0);
    }else
    if(huart->Instance == COM3_Instance){
        BHSerial *ser = (BHSerial *)hSerGetHandle("COM3");
        *(ser->rxBuf + ser->rxFront) = rx_temp_buf[COMFD_3][0];
        ser->rxFront = (ser->rxFront +1 )% USART_RX_BUF_SIZE;
        hSysLock();
        ser->rxCount ++;
        hSysUnlock();
        HAL_UART_Receive_IT(&ser->handle, &rx_temp_buf[COMFD_3][0] , 1 );
        if(ser->func) (*ser->func)(0);
    }
    
}


int32_t hSerAvailable(int32_t fd){
    BHSerial *ser = (BHSerial *)fd;
    return ser->rxCount;
}

void hSerFlush(int32_t fd){
    BHSerial *ser = (BHSerial *)fd;
    hSysLock();
    ser->rxRear = ser->rxFront = ser->rxCount=0;
    hSysUnlock();
}

void hSerClose(int32_t fd)
{
    BHSerial *ser = (BHSerial *)fd;
    HAL_UART_DeInit(&ser->handle);
    ser->used = 0;
}


uint32_t hSerGetHandle(char *port)
{
    if(strncmp (port, "COM1",4)==0){
        if(!serial_fd[COMFD_1].used) return 0;
        return (uint32_t )&serial_fd[COMFD_1] ;
    }
    else if(strncmp (port, "COM2",4)==0){
        if(!serial_fd[COMFD_2].used) return 0;        
        return (uint32_t )&serial_fd[COMFD_2] ;
    }
    else if(strncmp (port, "COM3",4)==0){
        if(!serial_fd[COMFD_3].used) return 0;        
        return (uint32_t )&serial_fd[COMFD_3] ;
    }
   
    return NULL;
}


uint32_t hSerGetHandleFromIsr(void *Instance)
{
    int i;
    UART_HandleTypeDef *handle;
    
    for(i=0;i<COMxn;i++){
        handle = (UART_HandleTypeDef *)&serial_fd[i];
        if(handle->Instance  == (USART_TypeDef*)Instance){
            return (uint32_t)handle;
        }
    }
    return 0;
}

#ifdef __GNUC__
#define PRINTF_WRITE	__io_putchar
#endif
//=======================================================================
void PRINTF_WRITE(int c)
{
#ifdef DBG_ITM
	ITM_SendChar(c);
#else
    if(debug_port_handle){
#if 1        
       hSerWrite(debug_port_handle,(uint8_t *)&c, 1);
#else
        BHSerial *ser;
        ser = (BHSerial *)debug_port_handle;
       while( (ser->handle.Instance->SR & UART_FLAG_TXE) == 0 && cnt < 1000){
        cnt++;
       }
       if(cnt != 1000){ 
           ser->handle.Instance->DR = (uint16_t)(c & 0x01FFU);
           return;
       }
       cnt = 0;
#endif
   }
#endif
}
int32_t GETCHAR_READ()
{
    int32_t ret;
    uint8_t BUF;
    if(debug_port_handle)
    ret  =     hSerRead(debug_port_handle,(uint8_t *)&BUF,1);
    if(ret > 0) return (int32_t) BUF;
		return -1;
}


#ifdef __DBG_ITM
volatile int ITM_RxBuffer = ITM_RXBUFFER_EMPTY;  /*  CMSIS Debug Input        */
#endif


/*----------------------------------------------------------------------------
  getchar() 함수 porting 
 *----------------------------------------------------------------------------*/
int SER_GetChar (void) {

#ifdef __DBG_ITM
  while (ITM_CheckChar() != 1) __NOP();
  return (ITM_ReceiveChar());
#else
 return GETCHAR_READ();
#endif
}

/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
 static  uint8_t BUF[3];

int SER_ReadByte (void) {

#ifdef __DBG_ITM
  if (ITM_CheckChar() != 1) {
    return 0;
  }
  return (ITM_ReceiveChar());
#else
    int32_t ret;
    //do
    //{
        ret = hSerRead(shell_port_handle,&BUF[0],1);
    if(ret ==0) return 0;
    return BUF[0];

#endif
}


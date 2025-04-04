#include "main.h"
#include "define.h"
#include "bmshal.h"
#include <string.h>
#include <assert.h>

#define  USART_RX_BUF_SIZE 256
//#define USART_TX_BUF_SIZE   (512)

#define MODE_UART       1
#define MODE_USART      2

typedef struct _BHSerial{
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

uint8_t rx_buf[COMn][USART_RX_BUF_SIZE];
BHSerial __far serial_fd[COMn];

uint32_t debug_port_handle = 0;


void hSerInit(void){
}

int32_t hSerOpen(char *port, int32_t baudrate, __far void  *asyncFunc )
{
    BHSerial __far *ser;
    if(strncmp (port, "COM1",4)==0){
        ser = (BHSerial __far *)&serial_fd[COM1];
        memset((void *)ser,0x0,sizeof(struct _BHSerial));
        ser->rxBuf = &rx_buf[COM1][0];
        ser->txBuf = NULL;
        ser->rxInterrupt = 1;
        ser->txFront=ser->txRear=ser->rxFront=ser->rxRear=ser->txLeft = 
        ser->rxCount =0;
        ser->mode = MODE_UART;
        ser->func =(void (*)(int) )asyncFunc;

    }else{
        return -1;
    }
    if( strcmp(port, hSysProperty("debug.serial.port"))==0){ 
        debug_port_handle =(uint32_t) ser;
    }
    ser->used = TRUE;
    return (int32_t)ser;
}

int32_t hSerCtrl(int32_t fd, int32_t opt, int32_t param)
{
    BHSerial __far*ser = (BHSerial __far *)fd;

    switch(opt)
    {
        case SER_SET_DEBUG:
            ser->debug = param;
            if(param) debug_port_handle = fd;
            break;
        case SER_SET_RS485TXE:
            break;
        default:
            return E_NOTSUP;
    }
    return 0;
}

int32_t hSerWrite(int32_t fd, uint8_t *buf, int32_t len)
{
    BHSerial __far *ser = (BHSerial __far*)fd;
    // buffer가 st code상 round-robine 이 되지 않기 때문에  buffer 
    // 끝 까지 data를 채운후 intterupt 를 통해서 다 send 한 후 다시 시작 한다.
    if(ser->txLeft && !ser->txFront){
        return 0;
    }
    Uart_BspSend(0,buf, len );
    return len;
}


#define min(x,y) x>y ? y: x;
int32_t hSerRead(int32_t fd, uint8_t *buf, int32_t len)
{
    int readin=0;   
    BHSerial __far*ser = (BHSerial __far*)fd;
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


void HAL_UART_RxCpltCallback(void *huart)
{
    BHSerial __far*ser = (BHSerial __far*)hSerGetHandle("COM1");
    ser->rxBuf[ser->rxFront] = bspRcvData;
    ser->rxFront = (ser->rxFront+1)% USART_RX_BUF_SIZE;
    hSysLock();
    ser->rxCount ++;
    hSysUnlock();
    if(ser->func){
        (*ser->func)(0);
    }
}


int32_t hSerAvailable(int32_t fd){
    BHSerial __far*ser = (BHSerial __far*)fd;
    return ser->rxCount;
}

void hSerFlush(int32_t fd){
    BHSerial __far *ser = (BHSerial __far*)fd;
    hSysLock();
    ser->rxRear = ser->rxFront = ser->rxCount=0;
    hSysUnlock();
}

void hSerClose(int32_t fd)
{
    BHSerial __far*ser = (BHSerial __far*)fd;
    Uart_stop();
    ser->used = 0;
}


uint32_t hSerGetHandle(char *port)
{
    if(strncmp (port, "COM1",4)==0){
        if(!serial_fd[COM1].used) return 0;
        return (uint32_t )&serial_fd[COM1] ;
    }
   
    return NULL;
}

int putchar(int __near ch){
	int32_t ret;
	ret = hSerWrite((int32_t)&serial_fd[COM1],(uint8_t *) &ch, 1);
	return ret;
}

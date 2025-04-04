/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "main.h"

#define RTOS 0

#if MB_ASCII_ENABLED == 1
#define	BUF_SIZE	513     /* must hold a complete ASCII frame. */
#else
#define	BUF_SIZE	256     /* must hold a complete RTU frame. */
#endif
#define TX_START_EVENT 1

static void mbSlavePortThread(void const * parameter) ;
/* ----------------------- Static variables ---------------------------------*/
static uint8_t    ucBuffer[2][BUF_SIZE];
static int32_t      TxBufCnt[2];
static uint32_t     expiredUtime;
//static uint64_t     prevtime;


static int32_t mb_serSema;
static int32_t mb_uart_fd[2];
static hOSThread mdbsSlave_th;
static int mbTxStartEvent[2];
/* ----------------------- Start implementation -----------------------------*/
void com1CallbackFromIsr(int mode)
{
    if(mode == 0){
        pxMBFrameCBByteReceived(COM1);
    }else{
        //mbMasterCallbackTrasmitterEmpty();
    }
}


BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
        eMBParity eParity)
{
    int32_t tid;
    mb_uart_fd[ucPORT] = E_ERROR ;
    assert_param( COM1<=ucPORT );
    /**
     * set 485 mode receive and transmit control IO
     * @note MODBUS_MASTER_RT_CONTROL_PIN_INDEX need be defined by user
     */
    switch(ucPORT)
    {
        case COM1:
            mb_uart_fd[COM1] = hSerOpen("COM1", ulBaudRate, com1CallbackFromIsr );
            break;

    }
    if(!mb_serSema){      
    }
    return TRUE;

}

void vMBPortSerialEnable(ULONG port , BOOL xRxEnable, BOOL xTxEnable)
{
    assert_param( COM1<=port );
    if(xRxEnable)
    {
    }
    else
    {
    }
		
    if( xTxEnable )
    {
        TxBufCnt[port] = 0;
        mbTxStartEvent[port] = TX_START_EVENT;
        hSerFlush(mb_uart_fd[port]);
        //hSerCtrl(mb_uart_fd[port], SER_SET_RS485TXE, 1);

        mb_serSema=1;
    }
    else
    {
        mbTxStartEvent[port] = 0;
    }

}

void vMBPortClose(ULONG port)
{
    hSerClose(mb_uart_fd[port]);
}

BOOL xMBPortSerialPutByte(ULONG port, CHAR ucByte)
{
    ucBuffer[port][TxBufCnt[port]] = ucByte;
    TxBufCnt[port]++;
    return TRUE;

}

BOOL xMBPortSerialSend(ULONG port)
{
    hSerWrite(mb_uart_fd[port],&ucBuffer[port][0], TxBufCnt[port]);
    return TRUE;
}

BOOL xMBPortSerialGetByte(ULONG port, CHAR * pucByte)
{
    hSerRead(mb_uart_fd[port], pucByte, 1);
    return TRUE;
}

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(ULONG port)
{
    pxMBFrameCBTransmitterEmpty(port);
}


void mbSlavePortPoll(void)
{
    if( mbTxStartEvent[COM1] == TX_START_EVENT ){
        /* execute modbus callback */
        prvvUARTTxReadyISR(COM1);
    }
}



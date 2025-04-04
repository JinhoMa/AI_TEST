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
 * File: $Id: porttimer.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static int32_t stimer[2];
static void prvvTIMERExpiredISR(ULONG);
static void com1_timer_timeout_ind(void);

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit(ULONG port, USHORT usTim1Timerout50us)
{
    int ms;
    assert_param( COM1<=port );
    ms = (usTim1Timerout50us < 20)? 1: ((usTim1Timerout50us <40)? 2: usTim1Timerout50us/20);
    if(port == COM1){
        stimer[0] = hTimCreate();
    	hTimSet(stimer[0], ms, com1_timer_timeout_ind);
    }

    return TRUE;
}

void vMBPortTimersEnable(ULONG port)
{
    assert_param( COM1<=port  );
    hTimStart(stimer[port]);
}

void vMBPortTimersDisable(ULONG port)
{
    assert_param( COM1<=port  );
    hTimStop(stimer[port]);
}
static void prvvTIMERExpiredISR(ULONG port){
    pxMBPortCBTimerExpired(port);
}

static void com1_timer_timeout_ind()
{
    prvvTIMERExpiredISR(COM1);
}



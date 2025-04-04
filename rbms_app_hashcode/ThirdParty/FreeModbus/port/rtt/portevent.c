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
 * File: $Id: portevent.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "obshal.h"

struct  MBPort_{
    LONG used;
    LONG port;
    ULONG event;
    LONG  mb_sema;
}  ;

#define RTOS    0

/* ----------------------- Variables ----------------------------------------*/
static struct MBPort_ mbPort[MBSLAVEn];

/* ----------------------- Start implementation -----------------------------*/
LONG xMBPortGetEvent(ULONG port)
{
    int i;
    for(i=0;i<MBSLAVEn;i++)
    {
        if(mbPort[i].port == port){
            return (LONG)&mbPort[i];
        }
    }
    return E_ERROR;
}

LONG 
xMBPortEventInit(ULONG port)
{
    int i;
    for(i=0;i<MBSLAVEn;i++){
        if( mbPort[i].used==0){
            mbPort[i].used= 1;
            break;
        }
    }
    if(i == MBSLAVEn) {
        htrace( LOG_ABORT, "event init error\n");
        return 0;
    }
    mbPort[i].event =0;
    mbPort[i].port = port;
#if RTOS    
    mbPort[i].mb_sema = hOsBiSemaCreate();
#else
    mbPort[i].mb_sema = 0;
#endif
    return (LONG)&mbPort[i];
}

BOOL
xMBPortEventPost( LONG id, eMBEventType eEvent )
{
    struct MBPort_ *p = (struct MBPort_*)id;
	hSysLock();
	p->event |= eEvent;
	hSysUnlock();
#if RTOS    
	hOsBiSemaSignal( p->mb_sema );
#else
    p->mb_sema = 1;
#endif
	return TRUE;
}

BOOL
xMBPortEventGet(LONG id, eMBEventType * eEvent )
{
    struct MBPort_ *p = (struct MBPort_*)id;
#if RTOS    
    hOsBiSemaWait(p->mb_sema , -1);
#else
    if(p->mb_sema == 0) return FALSE;
    p->mb_sema =0;
#endif
    hSysLock();
    if (p->event & EV_READY ){
        p->event  &= ~EV_READY;
        *eEvent = EV_READY;
    }else	if(p->event  & EV_FRAME_RECEIVED){
        p->event  &= ~EV_FRAME_RECEIVED;
        *eEvent = EV_FRAME_RECEIVED;
    }else if(p->event  & EV_EXECUTE){
        p->event  &= ~EV_EXECUTE;
        *eEvent = EV_EXECUTE;
    }else  if(p->event  & EV_FRAME_SENT){
        p->event  &= ~EV_FRAME_SENT;
        *eEvent = EV_FRAME_SENT;
    }
    hSysUnlock();
    return TRUE;
}

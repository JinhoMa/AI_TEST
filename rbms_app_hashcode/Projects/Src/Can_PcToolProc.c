/**
 ******************************************************************************
 *     @file         Can_PcToolProc.c
 *     @version   
 *     @author    
 *     @date      2024/11/07 
 *     @brief     
 ******************************************************************************
*/

#include <string.h>
#include "obshal.h"
#include "main.h"
#include "BMS.h"
#include "BMS_OS.h"
#include "CAN_PCTool.h"
#include "Cellbalancing.h"
//#include "BMIC_APP.h"
//#include "BMIC_ProcessData.h"


#define DOWNSTART_ID  0x7F1
#define DOWNING_ID    0x7F2
#define SECUR_ID     0x7F3
#define CONFIG_ID    0x7F4
#define RESET_ID     0x7F5
#define DOWNEND_ID    0x7FF
#define RET_ID       0x700

#define ACK_OK     "ok"
#define ACK_ERROR        "err"
#define KEY_A 0x9164
#define KEY_C 0x1665
#define KEY_E 0x0000FFFF
#define MAX_MSG_SIZE 30
    
static uint32_t pc_can_fd;
PCINTER_MSG PCInter[MAX_MSG_SIZE];
static uint8_t rd_total = 0;
static uint8_t wr_total = 0;
static uint8_t  pcinter_onReadBytes =0;
static uint8_t  rd_cnt = 0;
static uint8_t  wr_cnt =0;
int32_t pcinter_addr =0;
uint8_t pcinter_sendingBuf[16];
uint8_t pcinter_readBuf[16];
static void download_ack(const char *data, uint32_t value)
{
    hCanMessage msg;
    msg.id = 0x700;
    strncpy((char *)msg.data, data, strlen(data));
    msg.data[4]= (uint8_t)(value>>24);
    msg.data[5]= (uint8_t)(value>>16);
    msg.data[6]= (uint8_t)(value>>8);
    msg.data[7]= (uint8_t)value;
    msg.len = 8;
    hCanWrite(pc_can_fd, &msg, 1);
}

static void changeEEPRomAccessMode( uint8_t mode , uint16_t seed, uint16_t key)
{
    uint16_t CalKey;
    if( mode==1)
    { //unlock process
        CalKey=(uint16_t)((( seed * KEY_A)+KEY_C) & KEY_E);
        if(CalKey == key){
            pcinter_onReadBytes= 0; rd_total=rd_cnt= 0;
            PushJob(EEPROM_SCHEDULER_IDX, ULOCKWRITE_FROM_PCTOOL, EEPROMSCH_TIME, NULL);
            download_ack(ACK_OK, 0);
        }else{
            download_ack(ACK_ERROR, 0);
        }        
    }else
    {  //lock process
        PushJob(EEPROM_SCHEDULER_IDX, LOCKWRITE_FROM_PCTOOL, EEPROMSCH_TIME, NULL);
        download_ack(ACK_OK,0 );
    }
}
static void sendMessageToEEPRom(uint8_t *data)
{
    PCINTER_MSG *p;
    uint8_t rw = data[0];
    uint16_t addr = data[1]<<8|data[2];
    uint8_t size = data[3]&0x1F;
    uint8_t index= data[3]>>5;
    if (rw == 1){   //read
        p = (PCINTER_MSG *) PCInter_AllocMessage(CANB_SCHEDULER_IDX);
        if(p == NULL) return;
        p->addr = addr;
        p->size = size;
        if(size==0 || size > 16 || pcinter_onReadBytes ) { download_ack(ACK_ERROR,0); return; }
        pcinter_onReadBytes=1;
        
        //hLogProc(LOG_PCTOOL, "try read\n");
        PushJob(EEPROM_SCHEDULER_IDX, READBYTE_FROM_PCTOOL, EEPROMSCH_TIME, p);
    }else if(rw == 2){  //write
        wr_total = size;
        if(index ==0){
            wr_cnt =0;
        }
        if(wr_total <= (index+1)*4){ //LAST PACKET
            p = (PCINTER_MSG *) PCInter_AllocMessage(CANB_SCHEDULER_IDX);
            if(p == NULL) return;
            memcpy(&pcinter_sendingBuf[wr_cnt], &data[4], wr_total-wr_cnt);
            p->addr = addr;
            p->size = wr_total;
            p->idx = 0;
            memcpy(p->data, pcinter_sendingBuf, wr_total);;
            //hLogProc(LOG_PCTOOL, "try write\n");
            PushJob(EEPROM_SCHEDULER_IDX, WRITEBYTE_FROM_PCTOOL, EEPROMSCH_TIME, p);        
        }else{
            memcpy(&pcinter_sendingBuf[wr_cnt], &data[4], 4);
            wr_cnt += 4;
        }
    }
}

static void sendFactoryReset(void)
{
    PushJob(EEPROM_SCHEDULER_IDX, FACTORYREST_FROM_PCTOOL, EEPROMSCH_TIME, NULL);
}

static void sendEventLogData(void)
{
    PushJob(CANB_SCHEDULER_IDX, SEND_EVTLOGDATA_TO_PCTOOL, CANBSCH_TIME, NULL);
}

static void sendEraseAll(void)
{
    PushJob(EEPROM_SCHEDULER_IDX, ERASEALL_FROM_PCTOOL, EEPROMSCH_TIME, NULL);
}

#define KEY_A 0x9164
#define KEY_C 0x1665
#define KEY_E 0x0000FFFF 


PUBLIC void Init_CanPCToolInterface(void)
{
    int i;
    for(i=0;i<MAX_MSG_SIZE;i++)
    {
        CleanUpMsg(&PCInter[i]);
    }
}

PUBLIC void* PCInter_AllocMessage(uint8_t who)
{
    int i;
    for(i=0;i<MAX_MSG_SIZE;i++)
    {
        if(IsFreeMsg(&PCInter[i])){
            ReserveMsg(&PCInter[i], who);
            return &PCInter[i];
        }
    }
    return NULL;
}
PUBLIC void PCInter_SendSegmentDataToPC(void){
    int i;
    hCanMessage msg;
    uint8_t tmp, readin, prev;
    if(rd_cnt >= rd_total){
        pcinter_onReadBytes = 0;
        rd_cnt = 0; rd_total = 0;
        return;
    }
    memset(msg.data, 0xFF, 8);
    msg.id = RET_ID;
	msg.len =8;
    msg.data[0]=1;      //read
    msg.data[1]= (pcinter_addr>>8) &0xFF;
    msg.data[2]= pcinter_addr& 0xFF;
    tmp = rd_total;
    readin= ((rd_total-rd_cnt)>=4)? 4 : rd_total-rd_cnt ;
    prev = rd_cnt;
    rd_cnt += readin;
    tmp |= ((rd_cnt-1)/4) <<5;
    msg.data[3]= tmp;
    for(i=0;i<readin;i++){
        msg.data[4+i]=pcinter_sendingBuf[prev+i];
    }
    hLogProc(LOG_PCTOOL, "segment byte \n");
    
    hCanWrite(pc_can_fd, &msg, 1);
    PushJob(CANB_SCHEDULER_IDX, SEND_SEGDATA_TO_PCTOOL, CANBSCH_TIME, NULL);
}

PUBLIC void PCInter_ReturnReadBytes(void *buf)
{
    PCINTER_MSG *p = (PCINTER_MSG *)buf;
    if(p->fail) {
        hLogProc(LOG_PCTOOL, "return rd byte error\n");
        download_ack(ACK_ERROR, 0);
        return;
    }
    rd_total = p->size;
    memcpy(pcinter_sendingBuf, p->data, p->size);
    pcinter_addr = p->addr;
    rd_cnt =0;
    PCInter_SendSegmentDataToPC();
}

PUBLIC void PCInter_ReturnWriteBytes(void *buf)
{
    PCINTER_MSG *p = (PCINTER_MSG *)buf;
    hCanMessage msg;
    if(p->fail) {
         hLogProc(LOG_PCTOOL, "return byte error\n");
        download_ack(ACK_ERROR, 0);
        wr_total =wr_cnt =0;
        return;
	}
    hLogProc(LOG_PCTOOL, "return byte \n");
	memset(msg.data,0xFF,8);
	msg.id = RET_ID;
	msg.len =8;
	msg.data[0]=2;
	msg.data[1]=((p->addr)>>8) &0xFF;
	msg.data[2]= p->addr & 0xFF;
	msg.data[3]= wr_total ;    
	hCanWrite(pc_can_fd, &msg, 1);
}

PUBLIC void PCInter_SuccessFactoryReset(void)
{
    hLogProc(LOG_PCTOOL, "factory sucess\n");
    download_ack(ACK_OK, 0);
    hSysWait(1000);
    HAL_NVIC_SystemReset();
    hSysWait(1000);
}

PUBLIC void PCInter_SuccessEraseAll(void)
{
    download_ack(ACK_OK, 0);
    hSysWait(1000);
    HAL_NVIC_SystemReset();
    hSysWait(1000);
}

PUBLIC void CanPCToolEventHandler(uint32_t fd, uint16_t id, uint8_t *data)
{
    uint8_t msg[8] ;
    memcpy(msg, data, 8);
    pc_can_fd = fd;
    switch(id)
    {
        case 0x7F1:
            break;
        case 0x7F3:
            //hLogProc(LOG_PCTOOL, "PC change access\n");
            changeEEPRomAccessMode(data[0], (data[1]<<8 | data[2]), (data[3]<<8 | data[4]));
            break;
        case 0x7F4:
            //hLogProc(LOG_PCTOOL, "PC message\n");
            sendMessageToEEPRom(data);
            break;
        case 0x7F5:
            //hLogProc(LOG_PCTOOL, "factory reset\n");
            sendFactoryReset();
            break;
        case 0x7F6:
            //sendRTCSet(data);
            break;
        case 0x7F7:
            sendEventLogData();
            break;
        case 0x7F8:
            //erase all
            sendEraseAll();
            break;
        case 0x7F9:
            //soh 시작시간 
            //burningStartingSOH(data);
            break;
        case 0x7FA:
            //processCellBalThrPCTool(data);
            break;
    }

}


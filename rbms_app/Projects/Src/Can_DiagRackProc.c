/**
 ******************************************************************************
 *     @file         Can_DiagRackProc.c
 *     @version   
 *     @author    
 *     @date      2024/09/24 
 *     @brief     
 ******************************************************************************
*/
#include "main.h"
#include "obshal.h"
#include "BMS.h"
#include "BMS_OS.h"
#include "Master.h"
#include "CAN_DiagCom.h"
#include "CAN_PCTool.h"
#include "SYS_ERR_CHK.h"
#include "RACK_SYS.h"
#include "cellbalancing.h"
#include <string.h>

static uint8_t log_out;
extern MOST MaxCell[TBMS_BUF_SIZE] ;
extern MOST MinCell[TBMS_BUF_SIZE] ;
extern MOST MaxTemp;
extern MOST MinTemp;
extern STR_SMErrorModel DTC_SM;

uint8_t diag_relay_mode; // RELAY 단위 제어 테스트 모드 
uint8_t diag_fuse_mode;  // FUSE 단위 제어 테스트 모드

__WEAK PUBLIC void can_mbms_logout(void){
}


PUBLIC int32_t can_rbms_logout(void){
    static uint8_t mux=0  ;
    static int8_t trays = 0;
    hCanMessage msg;
    static uint64_t tag_time=0;
    static uint8_t cellcnt = 0;
    uint32_t base_id = 0x101;
    uint32_t vol1, vol2, tmp, i,j;

    if(!log_out) return 0;
#if defined(_RBMS)
    //Step1. Cell Voltage Out
    switch(mux){
        case 0:
            if( (hTimCurrentTime() -tag_time) < 100L) //100ms 
            {
                break;
            }
            tag_time = hTimCurrentTime();
            cellcnt=0;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            while( cellcnt < TRAY_CELL_NUMS )
            {
                msg.id = base_id + 0x20*(trays) + mux;
                msg.len = 8;
                if(cellcnt < TRAY_CELL_NUMS){
                    msg.data[0]=  RbmsComTray[trays].cellvol[cellcnt]>>8;
                    msg.data[1] = RbmsComTray[trays].cellvol[cellcnt];
                }else{
                    memset(msg.data, 0xff, 8);
                    goto can_write;
                }
                ++cellcnt;
                if(cellcnt < TRAY_CELL_NUMS){
                    msg.data[2] =  RbmsComTray[trays].cellvol[cellcnt]>>8;
                    msg.data[3] =  RbmsComTray[trays].cellvol[cellcnt];
                }else{
                    memset(&msg.data[2], 0xff, 6);
                    goto can_write;
                }
                ++cellcnt;
                if(cellcnt < TRAY_CELL_NUMS){
                    msg.data[4] =  RbmsComTray[trays].cellvol[cellcnt]>>8;
                    msg.data[5] =  RbmsComTray[trays].cellvol[cellcnt];
                }else{
                    msg.data[4]=msg.data[5]= msg.data[6]=msg.data[7]= 0xFF;
                    goto can_write;
                }
                ++cellcnt;
                if(cellcnt < TRAY_CELL_NUMS){
                    msg.data[6] =  RbmsComTray[trays].cellvol[cellcnt]>>8;
                    msg.data[7] =  RbmsComTray[trays].cellvol[cellcnt];
                }else{
                    msg.data[6]=msg.data[7]= 0xff;
                }
				++cellcnt;
can_write:                
                while ( hCanWrite(diag_can_fd, &msg, 1) == E_INPROGRESS){
                    hSysWait(1);
                }
                mux++;
                //4개 can write 후 쉬도록 한다. 
                if( mux %3 == 0){
                    return 1;
                }
            }
            mux=15;
            break;
        case 15:
            msg.id = base_id + 0x20*trays + mux++;
            msg.len = 8;
            //Step2. after CELL VOL, Temperature
            msg.data[0]=RbmsComTray[trays].tmp[0]>>8;
            msg.data[1]=RbmsComTray[trays].tmp[0];
            msg.data[2]=RbmsComTray[trays].tmp[1]>>8;
            msg.data[3]=RbmsComTray[trays].tmp[1];
            msg.data[4]=RbmsComTray[trays].tmp[2]>>8;
            msg.data[5]=RbmsComTray[trays].tmp[2];
            hCanWrite(diag_can_fd, &msg, 1);

            msg.id = base_id + 0x20*trays + mux++;
            msg.len = 8;
            //Step2. after CELL VOL, Temperature
            msg.data[0]=RbmsComTray[trays].brdtmp[0]>>8;
            msg.data[1]=RbmsComTray[trays].brdtmp[0];
            msg.data[2]=RbmsComTray[trays].brdtmp[1]>>8;
            msg.data[3]=RbmsComTray[trays].brdtmp[1];
            msg.data[4]=0xFF;
            msg.data[5]=0xFF;
            msg.data[6]=0xFF;
            msg.data[7]=0xFF;
            hCanWrite(diag_can_fd, &msg, 1);
            
            msg.id = base_id + 0x20*trays + mux++;
            msg.len = 8;
            tmp = 0;
            j= 0;
            msg.data[0]=msg.data[1]=msg.data[2]=msg.data[3]=msg.data[4]=msg.data[5]=msg.data[6]=msg.data[7]=0;
            for(i=0;i<TRAY_CELL_NUMS;i++){
                if (RbmsComTray[trays].cellbal[i]){
                    tmp |= (1<<(7- (i%8)));
                }
                if( i % 8 >= 7){
                    msg.data[j++]=tmp;
                    tmp = 0;
                }
            }
            hCanWrite(diag_can_fd, &msg, 1);
            
            break;

        case 18:            
            if(trays  == TRAY_NUMS-1){  // after last tray log 
                msg.id = 0x4F0;
                msg.len = 8;
                vol1 =vol2 = 0;
                for(i=0; i<TRAY_NUMS;i++){
                    vol1 += RbmsComTray[i].sum_sw_vol;
                }
                vol1 = vol1/10;
                vol2 = 0; // PcsVolt;
                msg.data[0]= vol1 >> 8;
                msg.data[1]= vol1;
                msg.data[2]= vol2 >> 8;
                msg.data[3]= vol2 ;
                vol1=  (uint32_t)Pack10mVolt;
                msg.data[4]= vol1 >> 8;
                msg.data[5]= vol1 ;
                tmp =  (BatCurr_CAN/10); 
                msg.data[6]= tmp>>8;
                msg.data[7]= tmp;
                hCanWrite(diag_can_fd, &msg, 1);
                                
                msg.id = 0x4F1;
                msg.len = 8;
                msg.data[0]= R.StringModel.Evt1 >> 24;
                msg.data[1]= R.StringModel.Evt1 >>16;
                msg.data[2]= R.StringModel.Evt1 >> 8;
                msg.data[3]= R.StringModel.Evt1 ;
                msg.data[4]= SOC_CTL.SOC >> 8;
                msg.data[5]= SOC_CTL.SOC ;
                msg.data[6]= 0; //EXT_TEMP.REAL_TEMP_VALUE>>8;
                msg.data[7]= 0; //EXT_TEMP.REAL_TEMP_VALUE;
                hCanWrite(diag_can_fd, &msg, 1);
                msg.id = 0x4F2;
                msg.len = 8;

            }
            mux++;
            break;            
        case 19:
                if(trays  == TRAY_NUMS-1){
                    msg.id = 0x4F3;
                    msg.len = 8;
                    msg.data[0]= MaxCell[0].value >>8;
                    msg.data[1]= MaxCell[0].value;
                    msg.data[2]= MaxCell[0].tray;
                    msg.data[3]= MaxCell[0].num ;
                    msg.data[4]= MinCell[0].value >>8;
                    msg.data[5]= MinCell[0].value;
                    msg.data[6]= MinCell[0].tray;
                    msg.data[7]= MinCell[0].num;
                    hCanWrite(diag_can_fd, &msg, 1);
                        
                    msg.id = 0x4F4;
                    msg.len = 8;
                    msg.data[0]= MaxTemp.value >> 8;
                    msg.data[1]= MaxTemp.value ;
                    msg.data[2]= MaxTemp.tray;
                    msg.data[3]= MaxTemp.num ;
                    msg.data[4]= MinTemp.value >> 8;
                    msg.data[5]= MinTemp.value;
                    msg.data[6]= MinTemp.tray;
                    msg.data[7]= MinTemp.num;
                    hCanWrite(diag_can_fd, &msg, 1);
                    // Add Vendor Alarm StrEvt2 Field
                    msg.id = 0x4F5;
                    msg.len = 8;
                    msg.data[0]= R.StringModel.Evt2 >> 24;
                    msg.data[1]= R.StringModel.Evt2 >>16;
                    msg.data[2]= R.StringModel.Evt2 >> 8;
                    msg.data[3]= R.StringModel.Evt2 ;
                    msg.data[4]= DTC_SM.reason_code>>8;
                    msg.data[5]= DTC_SM.reason_code;
                    msg.data[6]= 0;
                    msg.data[7]= 0;
                    hCanWrite(diag_can_fd, &msg, 1);
            
                    
                }
                        
                mux ++;
                break;
        case 20:
            msg.id = 0x4F6;
            msg.len = 8;
            msg.data[0]= R.StringModel.SetEna ;
            msg.data[1]= R.StringModel.SetCon ;
            msg.data[2]= hFetState(2);  //C_FET
            msg.data[3]= hFetState(3);  //D_FET
            msg.data[4]= hFetState(1);  //HEAT_FET
            msg.data[5]= diag_fuse_mode;
            msg.data[6]= hRajFetState(2);
            msg.data[7]= hRajFetState(3);
            hCanWrite(diag_can_fd, &msg, 1);
            can_mbms_logout();
            /*
            msg.data[4] = FET_TEMP.REAL_TEMP_VALUE >> 8;
            msg.data[5] = FET_TEMP.REAL_TEMP_VALUE;
            msg.data[6] = BD_TEMP.REAL_TEMP_VALUE >> 8;
            msg.data[7] = BD_TEMP.REAL_TEMP_VALUE;
            */

            mux ++;
            break;
        case 21:
            mux++;
            break;
        case 22:
            trays = (++trays) % TRAY_NUMS;
            mux= 0;
            break;
        default:
            mux=0;
            trays = (++trays) % TRAY_NUMS;
            break;
            
    }
#endif						
    return 1;    
}
PUBLIC void can_Diag_ReadDTCInfomation(void)
{

    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    data[0] = R.StringModel.Evt2;
    data[1] = (R.StringModel.Evt1 >> 16);
    data[2] = (R.StringModel.Evt1 >> 8);
    data[3] = R.StringModel.Evt1 ;
    responsePositiveMessage(DTCINFO, READ_DTCINFO, data);

}
PUBLIC void can_Diag_ClearDTCInfomation(uint8_t resp )
{
    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    R.StringModel.alrmRstCommand=1;
    hSysWait(1000);    
    R.StringModel.alrmRstCommand=0;
    responsePositiveMessage(DTCINFO, CLEAR_DTCINFO, data);
}

PUBLIC void can_Diag_SuccessFactoryReset(void)
{
    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    responsePositiveMessage(FACTRESET, 0x1, data);
    hSysWait(2000);
    HAL_NVIC_SystemReset(); // Factory Reset 후에 Rebooting 한다.
}

PUBLIC void can_Diag_SuccessWriteBytesEEPROM(void *buf)
{
    uint8_t lid,sid;
    STR_CAN_MSG *r = (STR_CAN_MSG *)buf;
    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    if(!r) return;
    lid = r->lid;
    sid = r->sid;
    responsePositiveMessage(lid,sid,data);
}
PUBLIC void can_Diag_SuccessReadBytesEEPROM(void *buf)
{
    uint8_t lid,sid;
    STR_CAN_MSG *r = (STR_CAN_MSG *)buf;
    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    if(!r) return;
    lid = r->lid;
    sid = r->sid;
    data[0]= r->test.eep.data[0].value[0];
    data[1]= r->test.eep.data[0].value[1];
    data[2]= r->test.eep.data[0].value[2];
    data[3]= r->test.eep.data[0].value[3];
    responsePositiveMessage(lid,sid,data);
    data[0]= r->test.eep.data[1].value[0];
    data[1]= r->test.eep.data[1].value[1];
    data[2]= r->test.eep.data[1].value[2];
    data[3]= r->test.eep.data[1].value[3];
    responsePositiveMessage(lid,sid,data);
    data[0]= r->test.eep.data[2].value[0];
    data[1]= r->test.eep.data[2].value[1];
    data[2]= r->test.eep.data[2].value[2];
    data[3]= r->test.eep.data[2].value[3];
    responsePositiveMessage(lid,sid,data);
    data[0]= r->test.eep.data[3].value[0];
    data[1]= r->test.eep.data[3].value[1];
    data[2]= r->test.eep.data[3].value[2];
    data[3]= r->test.eep.data[3].value[3];
    responsePositiveMessage(lid,sid,data);
}

PUBLIC void can_Diag_SuccessModelInfoEEPROM(void *buf)
{
    uint8_t i;
    STR_CAN_MSG *r = (STR_CAN_MSG *)buf;
    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    if(!r) return;
    data[0]=0;
    for(i=0;i<5;i++){
        data[i+1]=r->test.hm.mn[i];
    }
    responsePositiveMessage(TESTCONF, READMODEL, data);
    data[0]=1;
    for(i=0;i<5;i++){
        data[i+1]=r->test.hm.mn[i+5];
    }
    responsePositiveMessage(TESTCONF, READMODEL, data);
    data[0]=2;
    for(i=0;i<5;i++){
        data[i+1]=r->test.hm.mn[i+10];
    }
    responsePositiveMessage(TESTCONF, READMODEL, data);
    data[0]=3; data[1]=r->test.hm.mn[15];data[2]=data[3]=data[4]=data[5]=0xFF;
    responsePositiveMessage(TESTCONF, READMODEL, data);
    data[0]=4; 
    for(i=0;i<5;i++){
        data[i+1]=r->test.hm.mv[i];
    }
    responsePositiveMessage(TESTCONF, READMODEL, data);
    data[0]=5;data[1]=r->test.hm.mv[5];data[2]=r->test.hm.mv[6];data[3]=r->test.hm.mv[7];data[4]=data[5]=0xff;
    responsePositiveMessage(TESTCONF, READMODEL, data);
    
}
PUBLIC void can_Diag_SuccessVerInfoEEPROM(void *buf)
{
    STR_CAN_MSG *r = (STR_CAN_MSG *)buf;
    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    if(!r) return;
    data[0] = 0;
    data[1]=r->test.sm.ver[0];
    data[2]=r->test.sm.ver[1];
    data[3]=r->test.sm.build[0];
    data[4]=r->test.sm.build[1];
    data[5]=r->test.sm.build[2];
    responsePositiveMessage(TESTCONF, SWVER, data);    
}

PUBLIC void can_Diag_RackReadTrayCellVol(void)
{
#ifdef _RBMS
    int i,j,cnt;
    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    for(i=0;i<TRAY_NUMS;i++){
        cnt =(i<<4)& 0xF0;
        for(j=0;j<TRAY_CELL_NUMS; j+=2){
            data[0]=cnt++;
            data[1]=RbmsComTray[i].cellvol[j]>>8;
            data[2]=RbmsComTray[i].cellvol[j]&0xFF;
            if(j+1 >= TRAY_CELL_NUMS){
                data[3]=0xFF;
                data[4]=0xFF;
            }else{
                data[3]=RbmsComTray[i].cellvol[j+1]>>8;
                data[4]=RbmsComTray[i].cellvol[j+1]&0xFF;
            }
            responsePositiveMessage(READTRAYS, CELL_VOL, data);
        }
        hSysWait(1);
    }
#endif
}
PUBLIC void can_Diag_RackReadTrayTmp(void)
{
#ifdef _RBMS
    int i,cnt;
    uint8_t data[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    for(i=0;i<TRAY_NUMS;i++){
        cnt =(i<<4)& 0xF0;
        data[0]=cnt++;
        data[1]=RbmsComTray[i].tmp[0]>>8;
        data[2]=RbmsComTray[i].tmp[0]&0xFF;
        data[3]=RbmsComTray[i].tmp[1]>>8;
        data[4]=RbmsComTray[i].tmp[1]&0xFF;
        responsePositiveMessage(READTRAYS, TRAY_TMP, data);
        data[0]=cnt++;
        data[1]=RbmsComTray[i].tmp[2]>>8;
        data[2]=RbmsComTray[i].tmp[2]&0xFF;
        data[3]=0xFF;
        data[4]=0xFF;
        responsePositiveMessage(READTRAYS, TRAY_TMP, data);
        data[0]=cnt++;
        data[1]=RbmsComTray[i].brdtmp[0]>>8;
        data[2]=RbmsComTray[i].brdtmp[0]&0xFF;
        data[3]=RbmsComTray[i].brdtmp[1]>>8;
        data[4]=RbmsComTray[i].brdtmp[1]&0xFF;
        responsePositiveMessage(READTRAYS, TRAY_TMP, data);
    }
#endif
}

static void can_rack_dtcinformation_handler( uint8_t *msg)
{
    uint8_t lid = msg[0];
    uint8_t sid = msg[1];
 	if(CanGW.AccessStatus == SUCESSACCESS)
 	{
 		switch(sid)
 		{
 			case 1: //READ DTC
 			    PushJob( CANB_SCHEDULER_IDX, READ_DTC_FROM_DIAGMODE, CANBSCH_TIME, NULL);
 				//Diag_ReadDTCInfomation();
 			  break;

 			case 2: //CELAR DTC
     			PushJob( CANB_SCHEDULER_IDX, CLR_DTC_FROM_DIAGMODE, CANBSCH_TIME, NULL);
                //Diag_ClearDTCInfomation(1);
 			  break;

 			default:
 				responseNegtiveMessage(lid, N_INVALID_ID);
 			break;
 		}
 	}
 	else
 	{
 		responseNegtiveMessage(lid, N_NOACESS_MODE );
 	}
}

static void can_rack_factoryreset_handler( uint8_t *msg)
{
    uint8_t lid = msg[0];
    uint8_t sid = msg[1];
    if( CanGW.AccessStatus != SUCESSACCESS )
    {
 		responseNegtiveMessage(lid, N_NOACESS_MODE );    
        return;
    }
    if(sid ==1){
        PushJob( EEPROM_SCHEDULER_IDX, FACTORYREST_FROM_DIAGMODE, EEPROMSCH_TIME, NULL);
    }else{
        responseNegtiveMessage(lid, N_INVALID_ID);
    }
}

static void can_trayreaddata_handler(uint8_t *msg)
{

    uint8_t lid = msg[0];
    uint8_t sid = msg[1];
    
 	if(CanGW.AccessStatus != SUCESSACCESS)
 	{
 		responseNegtiveMessage(lid, N_NOACESS_MODE );    
 	    return;
    }
 	    
	switch(sid)
	{

        case CELL_NUM:
            PushJob(CANB_SCHEDULER_IDX, READ_CELLNUM_FROM_DIAGMODE, CANBSCH_TIME, NULL);
            break;
        case BLOCK_NUM:
            PushJob(CANB_SCHEDULER_IDX, READ_BLOCKNUM_FROM_DIAGMODE, CANBSCH_TIME, NULL);
            break;
        case MDTMP_NUM:
            PushJob(CANB_SCHEDULER_IDX, READ_MDTMPNUM_FROM_DIAGMODE, CANBSCH_TIME, NULL);            
            break;
        case BDTMP_NUM:
            PushJob(CANB_SCHEDULER_IDX, READ_BDTMPNUM_FROM_DIAGMODE, CANBSCH_TIME, NULL);    
            break;
        default:
            responseNegtiveMessage(lid, N_INVALID_ID);
            break;
	}

}

static void can_rack_ctrlcmd_handler(uint8_t *msg)
{
    uint8_t lid = msg[0];
    uint8_t sid = msg[1];
    if( CanGW.AccessStatus != SUCESSACCESS )
    {
 		responseNegtiveMessage(lid, N_NOACESS_MODE );    
        return;
    }
    if(sid ==DIAG_RELAY_ON){
        R.StringModel.SetEna = 1;
        R.StringModel.SetCon = 1;
    }else if(sid == DIAG_RELAY_OFF){
        R.StringModel.SetEna = 2;
        R.StringModel.SetCon = 2;
    }else{
        responseNegtiveMessage(lid, N_INVALID_ID);
    }

}

static void can_rack_readsys_handler(uint8_t *msg)
{
	uint8_t lid = msg[0];
    uint8_t sid = msg[1];
    uint8_t DataByte[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    int16_t cur;
    uint32_t vol;
    if( CanGW.AccessStatus != SUCESSACCESS )
    {
 		responseNegtiveMessage(lid, N_NOACESS_MODE );    
        return;
    }
    switch(sid){
        case READ_CUR:
            cur =  (int16_t)(BatCurr_CAN/10); 
            DataByte[0]= (cur>>8);
            DataByte[1]= cur&0xFF;
            responsePositiveMessage(lid, sid, DataByte);
            break;
        case READ_PACKVOL:
            vol = R.StringModel.BatV;
            DataByte[0]= (vol>>8);
            DataByte[1]= vol&0xFF;            
            vol = R.StringModel.V;
            DataByte[2]= (vol>>8);
            DataByte[3]= vol&0xFF;            
            responsePositiveMessage(lid, sid, DataByte);
            break;
        default:
            responseNegtiveMessage(lid, N_INVALID_ID);
            break;
    }
}
void can_rack_readtrays_handler(uint8_t *msg)
{
    uint8_t lid = msg[0];
    uint8_t sid = msg[1];
    if( CanGW.AccessStatus != SUCESSACCESS )
    {
 		responseNegtiveMessage(lid, N_NOACESS_MODE );    
        return;
    }
    switch(sid){
        case TRAY_CNT:
        //PushJob(CANB_SCHEDULER_IDX, READ_RACK_TRAYCNT_FROM_DIAGMODE, 0, 0);
        break;
        case CELL_VOL:
        PushJob(CANB_SCHEDULER_IDX, READ_RACK_TRAYCELLVOL_FROM_DIAGMODE, 0, 0);
        break;
        case TRAY_TMP:
        PushJob(CANB_SCHEDULER_IDX, READ_RACK_TRAYTEMP_FROM_DIAGMODE, 0, 0);
        break;
        case CELL_DEV:
        //PushJob(CANB_SCHEDULER_IDX, READ_RACK_CELLDEV_FROM_DIAGMODE, 0, 0);
        break;
        case INTLOCK:
        //PushJob(CANB_SCHEDULER_IDX, GET_INTLOCKST_FROM_DIAGMODE, 0, 0);
        break;
        default:
        responseNegtiveMessage(lid, N_INVALID_ID);
        break;
    }

}

void can_rack_testconf_handler(uint8_t *msg)
{
    uint8_t lid = msg[0];
    uint8_t sid = msg[1];
    STR_CAN_MSG *p;
    if( CanGW.AccessStatus != SUCESSACCESS )
    {
 		responseNegtiveMessage(lid, N_NOACESS_MODE );    
        return;
    }
    switch(sid){
        case READMODEL:
            p =(STR_CAN_MSG *)can_com_AllocMessage(CANB_SCHEDULER_IDX);
            if(!p) return;
            p->sid = sid;
            p->lid = lid;
            PushJob( EEPROM_SCHEDULER_IDX, READMODEL_FROM_DIAGMODE, 0, p);
            break;
        case SWVER:
            p =(STR_CAN_MSG *) can_com_AllocMessage(CANB_SCHEDULER_IDX);
            if(!p) return;
            p->sid = sid;
            p->lid = lid;
            PushJob( EEPROM_SCHEDULER_IDX, READVER_FROM_DIAGMODE, 0, p);
            break;
        case CHKSUM:
           // responsePositiveMessage(lid,sid, data);
            break;
        default:
            responseNegtiveMessage(lid, N_INVALID_ID);            
            break;
    }
}


static void can_rack_alarmreset_handler(uint8_t *msg)
{
    //DIAGMODE_MSG *p;
    uint8_t lid = msg[0];
    uint8_t sid = msg[1];
    //uint8_t data[6]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    if( CanGW.AccessStatus != SUCESSACCESS )
    {
 		responseNegtiveMessage(lid, N_NOACESS_MODE );    
        return;
    }
    switch(sid){
        case ALM_CLR:
            PushJob( EEPROM_SCHEDULER_IDX,  CLEAR_FAULT_RECORD , 0, 0);            
            Diag_ErrorReset();
            DTC_SM.reason_code=0;
            responseNegtiveMessage(lid, 0);
            break;
        default:
            responseNegtiveMessage(lid, N_INVALID_ID);
        return;;
    }
}

__WEAK void can_rack_eepromaccess_handler(uint8_t *msg)
{
 
}

static void can_rack_fusecontrol_handler(uint8_t *msg)
{
   uint8_t lid = msg[0];
   uint8_t sid = msg[1];
    if( CanGW.AccessStatus != SUCESSACCESS )
    {
 		responseNegtiveMessage(lid, N_NOACESS_MODE );    
        return;
    }
    switch(sid){
        case FUSE_MODE_ON:
            diag_fuse_mode = 1;
            break;
        case FUSE_OFF:
            if(diag_fuse_mode == 0)
                break;
            PushJob(RBMS_SCHEDULER_IDX, RBMS_BLOWOFF_FUSE, 0, 0 );
            break;
        default:
            responseNegtiveMessage(lid, N_INVALID_ID);                
    }
    responseNegtiveMessage(lid, 0);    
}

static void can_rack_fetcontrol_handler(uint8_t *msg)
{
   uint8_t lid = msg[0];
   uint8_t sid = msg[1];

    if( CanGW.AccessStatus != SUCESSACCESS )
    {
 		responseNegtiveMessage(lid, N_NOACESS_MODE );    
        return;
    }

    switch(sid){
        case RELAY_MODE_ON:
            diag_relay_mode = 1;
            break;
        case RELAY_MODE_OFF:
            diag_relay_mode = 0;
            break;
        case RELAY_CFET_ON:
            if(diag_relay_mode == 0)
                break;
            CFET_ON();
            break;
        case RELAY_CFET_OFF:
            if(diag_relay_mode == 0)
                break;
            CFET_OFF();
            break;
        case RELAY_DFET_ON:
            if(diag_relay_mode == 0)
                break;
            DFET_ON();
            break;
        case RELAY_DFET_OFF:
            if(diag_relay_mode == 0)
                break;
            DFET_OFF();
            break;
        default:
            responseNegtiveMessage(lid, N_INVALID_ID);                
            return;
    }
    responseNegtiveMessage(lid, 0);
}

void can_rack_cellbaancing_handler(uint8_t *msg)
{
   uint8_t lid = msg[0];
   uint8_t sid = msg[1];

    if( CanGW.AccessStatus != SUCESSACCESS )
    {
 		responseNegtiveMessage(lid, N_NOACESS_MODE );    
        return;
    }

    switch(sid){
        case ENABLE_CB:
            BalancingCMD = BALANCING_STOP_CMD;
            bmic_balsw_on =1;
            PushJob( SUBMIC_SCHEDULER_IDX, SUBMIC_CMD_CELLBAL, 0, 0);
            break;
        case DISABLE_CB:
            BalancingCMD = BALANCING_STOP_CMD;
            bmic_balsw_on=0;
            PushJob( SUBMIC_SCHEDULER_IDX, SUBMIC_CMD_CELLBAL, 0, 0);
            break;
    }
    responseNegtiveMessage(lid, 0);
    
}

__WEAK void can_rack_calibration_handler(uint8_t *msg)
{
   uint8_t lid = msg[0];
   responseNegtiveMessage(lid, N_NOTSUPPORT);
}

__WEAK void can_rack_cursimulation_handler(uint8_t *msg)
{
   uint8_t lid = msg[0];
   uint8_t sid = msg[1];  
   int32_t data;
   if( CanGW.AccessStatus != SUCESSACCESS ){
 		responseNegtiveMessage(lid, N_NOACESS_MODE );    
        return;
    }

   if(sid == CUR_SIMUL_ON)
   {
      CanGW.Simul.cur_sim = 1;
      data = (int32_t)(int16_t)((msg[2] << 8) | msg[3]);
      CanGW.Simul.sim_data = data;
   }else{
      CanGW.Simul.cur_sim = 0;
   }
   responseNegtiveMessage(lid, 0);
}

__WEAK void can_debug_mode_handler(uint8_t *msg)
{
    uint8_t lid = msg[0];
    uint8_t sid = msg[1];
    int32_t data;
    if (CanGW.AccessStatus != SUCESSACCESS)
    {
        responseNegtiveMessage(lid, N_NOACESS_MODE);
        return;
    }
    
}

PUBLIC void can_rbms_eventhandler(uint32_t fd, uint8_t *data)
{
    uint8_t msg[8] ;
    memcpy(msg, data, 8);
    diag_can_fd = fd;
    switch(msg[0])
    {
        case CHANGEMODE:
            can_com_modechange_handler( msg[0], msg[1] );
            break;
        case SECACCESS:
            can_com_securityacess_handler(data);
            break;
        case HANDLEDTC:
            can_rack_dtcinformation_handler(data);
            break;
        case FACTRESET:
            can_rack_factoryreset_handler(data);
            break;
        case HANDLETRAY:
            can_trayreaddata_handler(data);
            break;
        case CTRLCMD:
            can_rack_ctrlcmd_handler(data);
            break;
        case READRACKSYS:
            can_rack_readsys_handler(data);
            break;
        case ALMRST:
            can_rack_alarmreset_handler(data);
            break;
        case FUSE_CTRL:
            can_rack_fusecontrol_handler(data);
            break;
        case RELAY_CTRL:
            can_rack_fetcontrol_handler(data);
            break;
        case CELLBALANCE:
            can_rack_cellbaancing_handler(data);
            break;
        case READTRAYS:
            can_rack_readtrays_handler(data);
            break;
        case TESTCONF:
            can_rack_testconf_handler(data);
            break;
        case CALBAT:
        case CALPCS:
        case CALCCUR:
        case CALDCUR:
        case CAL_CV:
            can_rack_calibration_handler(data);
            break;
        case CUR_SIMUL:
            can_rack_cursimulation_handler(data);
            break;
        case LOGOUT:
            log_out=1;
            PushJob(CANB_SCHEDULER_IDX, DIAG_LOG_OUT, 0 , 0);  
            break;
        case LOGSTOP:
            log_out= 0;
            break;
        case DEBUGSTART:
            
            break;
        case DEBUGSTOP:
            break;
        default:
            responseNegtiveMessage(msg[0], N_INVALID_ID);
            break;
    }
}

void Canb_RxSwi(hCanMessage    *msg)
{
    uint16_t MIV;

    int fd;
    fd = hCanGetHandle("CANB");
    MIV = msg->id;

    switch(MIV)
    {
        case 0x7EF:
            //hLedBlink(LED1);
            can_rbms_eventhandler(fd, msg->data);
            break;
        case 0x7F1:
        case 0x7F2:
            //download_app(fd,  msg->data);
            break;
        case 0x7F3:
        case 0x7F4:
        case 0x7F5:
        case 0x7F6:
        case 0x7F7:
        case 0x7F8:
        case 0x7F9:
        case 0x7FA:
            CanPCToolEventHandler(fd, MIV,  msg->data);
            break;
        case 0x7FB:
            //CanSocSimulationHandler(fd,  msg->data);
            break;
    }

}

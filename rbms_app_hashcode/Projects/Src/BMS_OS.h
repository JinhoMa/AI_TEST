#ifndef __BMS_OS_H
#define __BMS_OS_H

#define SYS_FOREVER -1

extern uint8_t eeprom_run ;
extern uint8_t canb_run ;




extern hOsSemaId_t EEPROM_SEM;
extern hOsSemaId_t CANB_SCHEDULER_SEM;
extern hOsSemaId_t EXTCOM_SCHEDULER_SEM;
extern void ExtCom_Scheduler_Task(void *arg);

#ifdef _TBMS
extern uint8_t bmic_run ;
extern uint8_t submic_run;

extern hOsSemaId_t BMIC_SEM;
extern hOsSemaId_t SUBMIC_SEM;
extern hOSThread SUBMIC_Scheduler;
extern void BMIC_Task(void *arg);
#endif
#ifdef _RBMS
extern uint8_t rbms_run ;
extern hOsSemaId_t RBMS_SEM;
extern void Rack_Task(void *arg);
#endif
#ifdef _MBMS
extern uint8_t mbms_run ;
extern hOsSemaId_t MBMS_SEM;
extern void Master_Task(void *arg);
#endif
#define RBMS_EVT_TIME  5
#define MMBS_EVT_TIME  25
#define NET_EVT_TIME    30
#define R10MS_EVT_TIME  10
#define R100MS_EVT_TIME  100
#define R500MS_EVT_TIME  500
#define EXTCOM_EVT_TIME  100

#define BMICPUSH_TIME   200
#define BMICSCH_TIME    100
#define CANBSCH_TIME    3    
#define EEPROMSCH_TIME  15
#define SUBMIC_EVT_TIME    200

#define NET_APP_TIME R100MS_EVT_TIME
#endif


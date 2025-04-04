
#ifndef Can_Transmit_Task_H_
#define Can_Transmit_Task_H_

//#include "Operating_Condition.h"

#define MBMS_CAN_TX_INTERVAL 0
#define UPS_MEASURING_START_INDEX  6
#define FR_MEASURING_START_INDEX  7
#define FAULT_STATUS_INDEX  2




void CAN_TX_Measuring_Start(void);
void CAN_TX_MBMS_Number(void);
void CAN_TX_Fault_Status(void);
void CAN_TX_Cell_Balancing(void);


extern void CanA_Scheduler_Task(void);
extern void CanB_Scheduler_Task(void*);






#endif

#ifndef __CELLBALANCING_H__
#define __CELLBALANCING_H__

/**
 ******************************************************************************
 *     @file         Cellbalancing.h
 *     @version   
 *     @author    
 *     @date      2024/10/17 
 *     @brief     
 ******************************************************************************
*/

#define CELLBALANCING	0x00
	#define CELL_SELECT_STAGE 0
	#define B_WATCHDOG_CHECK_STAGE   1
	#define STEADY_STAGE 2
#define REST			0x01
#define MEASUREMENT		0x02
#define ACTION_REST		0x03
#define BALANCING_COMPLETE 0x04

#define BALANCING_CMD_CLR 0x00
#define BALANCING_START_CMD 0x01
#define BALANCING_STOP_CMD   0x02

typedef struct
{
	uint16_t Cell_0:1;
	uint16_t Cell_1:1;
	uint16_t Cell_2:1;
	uint16_t Cell_3:1;
	uint16_t Cell_4:1;
	uint16_t Cell_5:1;
	uint16_t Cell_6:1;
    uint16_t Cell_7:1;
    uint16_t Cell_8:1;
    uint16_t Cell_9:1;
    uint16_t Cell_10:1;
    uint16_t Cell_11:1;
    uint16_t Cell_12:1;
    uint16_t Cell_13:1;
    uint16_t Cell_14:1;
    uint16_t Cell_15:1;
}STR_BalancingCellModel;

typedef union
{
	uint16_t all;
	STR_BalancingCellModel bit;
}UNI_BalancingCellPerDevice;


typedef struct
{
	UNI_Uint16Data TargetV;
	uint16_t BalancingStatus;
	uint16_t BalancingHystrisis;
	//UNI_BalancingCellPerDevice TargetBalCell[TBMS_BUF_SIZE][SLAVEDEV_BUF_SIZE];
	uint32_t T_OnBalTime; 			//T1, Min
	uint32_t T_RestTime;			//T2, sec
	uint32_t T_MeasureTime;		//T3, sec
	uint32_t T_ActionRestTime;	//T4, ms
	uint64_t tagtime;
    uint8_t balancingOnSubStage;

}STR_BalancingModel;

extern STR_BalancingModel Balancing;

PUBLIC void Init_Balancing(void);
PUBLIC void runAutoBalancingProcess(void);
PUBLIC void PerformBalancing(uint8_t ch);

#endif //__CELLBALANCING_H__

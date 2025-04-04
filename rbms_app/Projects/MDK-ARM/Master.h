#ifndef __MASTER_H__
#define __MASTER_H__

/**
 ******************************************************************************
 *     @file         Master.h
 *     @version   
 *     @author    
 *     @date      2024/09/19 
 *     @brief     
 ******************************************************************************
*/



// STATE
#define M_INIT			0x00					// Init
#define M_WAKE			0x01					// Wake Up
#define M_DIS			0x02					// Discharging
#define M_CHG			0x03					// Charging
#define M_TERM			0x05					// Charge Terminate
#define M_CWIT			0x06					// Charge Wait
#define M_PREC			0x07					// Pre Charge
#define M_ALON			0x09					// Sleep
#define M_SWRST         0x17                                    // Software Reset
#define M_DOWN			0x1E					// Power Down
#define M_FMODE         0xEE              		//  FAIL MODE
#define M_PFAIL         0xFF



PUBLIC void master_init(void);
PUBLIC void master_mode_check(void);
PUBLIC void master_fault_check(void);

#endif //__MASTER_H__

#ifndef __SM_H__
#define __SM_H__

/**
   ******************************************************************************
   *     @file         sm.h
   *     @version   
   *     @author    
   *     @date       2021/09/15 
   *     @brief     
   ******************************************************************************
   */

PUBLIC void sm_init(void);
PUBLIC void sm_watchdog_monitor(void);
PUBLIC void sm_submcu_timeout(void);
PUBLIC void sm_memory_marchtest(void);
PUBLIC void sm_mcufault_handler(void);
PUBLIC void sm_voltagePlausiblity_error(void);

#endif //__SM_H__

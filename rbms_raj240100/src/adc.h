#ifndef __ADC_H__
#define __ADC_H__

/**
 ******************************************************************************
 *     @file         adc.h
 *     @version   
 *     @author    
 *     @date      2023/09/18 
 *     @brief     
 ******************************************************************************
*/
GLOBAL void ADC_Bsp_init(void);
GLOBAL void ADC_Start(void);
GLOBAL void ADC_Stop(void);
GLOBAL WORD  ADC_Read(void) ;
#endif //__ADC_H__

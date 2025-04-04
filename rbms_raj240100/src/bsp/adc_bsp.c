
/*""FILE COMMENT""*******************************************************
* System Name	: 
* File Name		: 
* Version		: 
* Contents		: 
* Programmer	: 
* Note			: 
************************************************************************

*""FILE COMMENT END""**************************************************/
#include "BMS.h"
#include "iodefine.h"
#include "define.h"
#include "dataflash.h"
#include "ram.h"
#include "kousei.h"
#include "afe.h"
#include <string.h>
#include "main.h"


GLOBAL void ADC_Bsp_init(void)
{    
    ADCEN = 1;		/* supply AD clock */
    ADM0 = 0x00;	/* disable AD conversion and clear ADM0 register */
    ADMK = 1;		/* disable INTAD interrupt */
    ADIF = 0;		/* clear INTAD interrupt flag */
    /* The reset status of ADPC is analog input, so it's unnecessary to set. */
    /* Set ANI0 - ANI2 pin as analog input */
    PM2 |= 0x07;
    ADM0 = 0x14;
    ADM1 = 0x00;	/* software trigger, continuous mode */
    ADM2 = 0x60;	/* AD resolution is 10bit */          
    ADUL = 0xFF;
    ADLL = 0x00;
    ADS = 0x02;		/* AD input channel select (ANI2) */
    ADCE = 1;		/* enable AD comparator */
}

GLOBAL void ADC_Start(void)
{
    ADIF = 0U;  /* clear INTAD interrupt flag */   
    ADMK = 0U;  /* enable INTAD interrupt */   
    ADCS = 1U;  /* enable AD conversion */
    f_adc_int= 0;
}

GLOBAL void ADC_Stop(void)
{
    //ADCS = 0U;  /* disable AD conversion */   
    //ADMK = 1U;  /* disable INTAD interrupt */   
    ADIF = 0U;  /* clear INTAD interrupt flag */  
}

GLOBAL WORD  ADC_Read(void)   
{   
    while(!f_adc_int);
    return  (WORD)(ADCR >> 6U)&0x3FF;   //10bit AD
}  

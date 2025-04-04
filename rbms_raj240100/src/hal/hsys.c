#include "bmsHal.h"
#include "htypes.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>



const char *frame = "***************************************************************************************\n";
const char *title = "**                     run Application\n";
const char *model = "**                     model name: %s\n";
const char *date  = "**                     compile date : %s\n";

//////////////////////////////////////////////////////////////////////////////////
//                 io resource
/////////////////////////////////////////////////////////////////////////////////
int32_t can_fd;

void assert_failed(uint8_t *file, uint32_t line)
{
	 printf("abort: %s line %d\n", file, line); 
     while(1);
}


void bootPrint(void)
{
	
    printf(frame);
    printf(title);
    printf(frame);
	
}


 
// key ���� ���ؼ� ���ε� Port String �� �д´�.
// Key="shell.serial.port", -> "COM1" or "COM2"
char *hSysProperty(char *key){
    if(strcmp(key,"debug.serial.port") == 0){
        return "COM1";
    }
    return "";
}

/**
  * @brief System �ʱ�ȭ �Ѵ�.
  *  �� �Լ������� ���Ǵ� HAL Module���� �ʱ�ȭ �ϰ� �ȴ�.
  *  System�� ����  �Ǵ� �������� ���� 1ȸ ȣ�� �ϰ� �ȴ�.
  * @param NONE
  * @retval NONE
  */
void hSysInit(void)
{

    hTimInit();
    //serial �ʱ�ȭ 
    hSerInit();
}


void hSysWait(uint32_t msec)
{
   
}

int syslock=0;
void hSysLock(void)
{
    //__disable_irq();
   // EI();
    syslock++;
}


void hSysUnlock(void)
{
   --syslock;
   //if(syslock ==0)
   //DI();// __enable_irq();
}

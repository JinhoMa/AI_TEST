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


 
// key 값에 의해서 매핑된 Port String 을 읽는다.
// Key="shell.serial.port", -> "COM1" or "COM2"
char *hSysProperty(char *key){
    if(strcmp(key,"debug.serial.port") == 0){
        return "COM1";
    }
    return "";
}

/**
  * @brief System 초기화 한다.
  *  이 함수내에서 사용되는 HAL Module들을 초기화 하게 된다.
  *  System이 부팅  되는 과정에서 최초 1회 호출 하게 된다.
  * @param NONE
  * @retval NONE
  */
void hSysInit(void)
{

    hTimInit();
    //serial 초기화 
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

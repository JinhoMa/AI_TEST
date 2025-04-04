#include "define.h"								// union/define definition
#include "afe.h"								// afe.c
#include "kousei.h"								// Initial calibration
#include "mcu.h"								// mcu.c
#include "ram.h"								// ram.c

#define BUF_SIZE        256
unsigned char pBuf[BUF_SIZE];
int buf_size;
void PutChar(unsigned char buf);
void PrintpBuf(void);
 
  int txleft_count;
  int txrear;
  int logrxfront , logtxfront;

int strlen_(__far BYTE *buf){

    int t=0;
    while(*buf++ != 0){
        t++;
    }
    return t;
}  

void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

char* reverse(char *buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}


  
void R_UART0_Send(void)
 {
    unsigned char i=0;
   //    DI();
    for(i=0;i<buf_size;i++){
      while (SSR00 & 0x0040){}; 
      TXD0 = pBuf[i];
    }
    //   EI();
 }  


  void Printf(__far const unsigned char *buf){
    int i;
    for(i=0;buf[i]!=0;i++){
        pBuf[i] = buf[i];    
    }
    buf_size = i;
    pBuf[buf_size+1]=0x0;
    
    R_UART0_Send();
   //Uart_BspSend(0, buf, buf_size);
}
  
unsigned char* itoa(long long value, __far unsigned char* buffer, int base, int align)
{
    long long n;
    int i;
        
    // �߸��� �Է�
    if (base < 2 || base > 32) {
        return (unsigned char *)buffer;
    }
 
    // ������ ���밪�� ���
    n = (value < 0) ? -value: value;
 
    i = 0;
    while (n)
    {
        int r = n % base;
 
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }
        
        n = n / base;
    }
    // ���ڰ� 0�� ���
    if (i == 0) {
        buffer[i++] = '0';
    }
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }
    align -= i;
    if( base == 10){
        while(align-- > 0){
            buffer[i++]=' ';
        }
    }else{
        while(align-- > 0){
            buffer[i++]='0';
        }
    } 
    // ���� 10�̰� ���� �����̸� ��� ���ڿ�
    // ���̳ʽ� ��ȣ(-)�� �տ� �ɴϴ�.
    // �ٸ� ������ ����ϸ� ���� �׻� ��ȣ ���� ������ ���ֵ˴ϴ�.

 
    buffer[i] = '\0'; // null ���� ���ڿ�
 
    // ���ڿ��� �ݴ�� �Ͽ� ��ȯ
    return (unsigned char *)reverse((char *)buffer, 0, i - 1);
}

  void DPrintf(long long value,int base, int align)
{
    itoa(value, pBuf, base, align);
    PrintpBuf();
}

void PrintpBuf(void)
{
    buf_size = strlen_(pBuf);
    R_UART0_Send();
    pBuf[0]=0;
//    Uart_BspSend(0, pBuf, buf_size);
}




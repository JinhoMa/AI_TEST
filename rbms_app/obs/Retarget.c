/*----------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#ifdef __ICCARM__
#endif
#ifdef __CC_ARM
 #include <rt_misc.h>
 #pragma import(__use_no_semihosting_swi)
#endif
//#include "obshal.h"


extern int PRINTF_WRITE (int c);
extern int GETCHAR_READ (void);


#ifdef __CC_ARM 
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;


int fputc(int c, FILE *f) {
  return (PRINTF_WRITE(c));
}


int fgetc(FILE *f) {
  return (GETCHAR_READ());
}


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}
#endif
#ifdef __ICCARM__
int putchar(int c)
{
  PRINTF_WRITE(c);
  return c;
}
#endif

void _ttywrch(int c) {
  PRINTF_WRITE(c);
}


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}

#include "obshal.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "main.h"

extern void PRINTF_WRITE(int);

#define MAXDIGITS 20
/* Minimum and maximum values a `signed int' can hold.  */
#define INT_MAX   2147483647
#define isdigit(c)	((unsigned) ((c) - '0') <  (unsigned) 10)

#define PRINT PRINTF_WRITE

char debugMsg[512];
static uint32_t logLevel= LOG_DEFAULT;

struct _Log{
    uint32_t l;
    const char *msg;
} Log[]={
    { LOG_ABORT,    "abort "     },
    { LOG_ERROR,    "error "     },
    { LOG_WARN,     "warn  "      },
    { LOG_BMIC,     "bmic  "        },
    { LOG_MODBUS,   "modbus"      },
    { LOG_RENE,     "raj   "      },
    { LOG_RBMS,     "rbms  "       },
    { LOG_ETH,      "eth   "      },
    { LOG_MSG1,     "msg1  "      },
    { LOG_MSG2,     "msg2  "      },
    { LOG_MSG3,     "msg3  "      },
    { LOG_DTC,      "dtc   "       },
    { LOG_MBMS,     "mbms  "       },

};




typedef void (*LogHandler)(uint32_t level, char *fmt,  va_list  );

static void uart_handler(uint32_t level, char *fmt, va_list );

LogHandler logHandler = uart_handler;


void vprintk(char *fmt, va_list argp){
    int c;
    enum { LEFT, RIGHT } adjust;
    enum { LONG, INT } intsize;
    int fill;
    int width, max, len, base;
    static char X2C_tab[]= "0123456789ABCDEF";
    static char x2c_tab[]= "0123456789abcdef";
    char *x2c;
    char *p;
    long long i;
    unsigned long long u;
    char temp[8 * sizeof(long long) / 3 + 2];

    while ((c= *fmt++) != 0) {
        if (c != '%') {
            /* Ordinary character. */
            PRINT(c);
            continue;
        }

        /* Format specifier of the form:
         *	%[adjust][fill][width][.max]keys
         */
        c= *fmt++;

        adjust= RIGHT;
        if (c == '-') {
            adjust= LEFT;
            c= *fmt++;
        }

        fill= ' ';
        if (c == '0') {
            fill= '0';
            c= *fmt++;
        }

        width= 0;
        if (c == '*') {
            /* Width is specified as an argument, e.g. %*d. */
            width= va_arg(argp, int);
            c= *fmt++;
        } else
            if (isdigit(c)) {
                /* A number tells the width, e.g. %10d. */
                do {
                    width= width * 10 + (c - '0');
                } while (isdigit(c= *fmt++));
            }

        max= INT_MAX;
        if (c == '.') {
            /* Max field length coming up. */
            if ((c= *fmt++) == '*') {
                max= va_arg(argp, int);
                c= *fmt++;
            } else
                if (isdigit(c)) {
                    max= 0;
                    do {
                        max= max * 10 + (c - '0');
                    } while (isdigit(c= *fmt++));
                }
        }

        /* Set a few flags to the default. */
        x2c= x2c_tab;
        i= 0;
        base= 10;
        intsize= INT;
        if (c == 'l' || c == 'L') {
            /* "Long" key, e.g. %ld. */
            intsize= LONG;
            c= *fmt++;
        }
        if (c == 0) break;

        switch (c) {
            /* Decimal.  Note that %D is treated as %ld. */
            case 'D':
                intsize= LONG;
            case 'd':
                i= intsize == LONG ? va_arg(argp, long long)
                    : va_arg(argp, int);
                u= i < 0 ? -i : i;
                goto int2ascii;

                /* Octal. */
            case 'O':
                intsize= LONG;
            case 'o':
                base= 010;
                goto getint;

                /* Hexadecimal.  %X prints upper case A-F, not %lx. */
            case 'X':
                x2c= X2C_tab;
				intsize= LONG;
            case 'x':
                base= 0x10;
                goto getint;

                /* Unsigned decimal. */
            case 'U':
                intsize= LONG;
            case 'u':
getint:
                u= intsize == LONG ? va_arg(argp, unsigned long long)
                    : va_arg(argp, unsigned int);
int2ascii:
                p= temp + sizeof(temp)-1;
                *p= 0;
                do {
                    *--p= x2c[u % base];
                } while ((u /= base) > 0);
                goto string_length;

                /* A character. */
            case 'c':
                p= temp;
                *p= va_arg(argp, int);
                len= 1;
                goto string_print;

                /* Simply a percent. */
            case '%':
                p= temp;
                *p= '%';
                len= 1;
                goto string_print;

                /* A string.  The other cases will join in here. */
            case 's':
                p= va_arg(argp, char *);

string_length:
                for (len= 0; p[len] != 0 && len < max; len++) {}

string_print:
                width -= len;
                if (i < 0) width--;
                if (fill == '0' && i < 0) PRINT('-');
                if (adjust == RIGHT) {
                    while (width > 0) { PRINT(fill); width--; }
                }
                if (fill == ' ' && i < 0) PRINT('-');
                while (len > 0) { PRINT((unsigned char) *p++); len--; }
                while (width > 0) { PRINT(fill); width--; }
                break;

                /* Unrecognized format key, echo it back. */
            default:
                PRINT('%');
                PRINT(c);
        }
    }
}

void printk(char *fmt, ...)
{
    int i=0;
    va_list argp;
    va_start(argp, fmt);
    vsprintf(debugMsg,fmt, argp);
    /* Mark the end with a null (should be something else, like -1). */
    //	putChar(0);
    va_end(argp);
    while(debugMsg[i]) PRINT(debugMsg[i++]);
}

uint32_t hLogGetLevel(void)
{
    return logLevel;
}

void hLogSetLevel(uint32_t l){
    logLevel = l;
}

static void uart_handler(uint32_t level, char *fmt, va_list args)
{
    //printk("%d :", level);
    int i, find=0;
    for(i=0;i<sizeof(Log)/sizeof(struct _Log);i++){
        if(Log[i].l == level ){
            if(strstr(fmt,"\n")){
                printf("[%s:%8d] >> ", Log[i].msg, (uint32_t)hTimCurrentTime() );
            }
            find=1;
        }
    }
    if(!find) printf("[0x%8x] >>", level);
    vprintf(fmt,args);
}

void hLogProc(uint32_t level, char *fmt, ...)
{
 
    va_list args;
    if ( (level &logLevel) && logHandler) {
        va_start(args, fmt);
        logHandler(level, fmt, args);
        va_end(args);
    }

}


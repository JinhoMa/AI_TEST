/******************************************************************************/
/* data_types.h                                                               */
/******************************************************************************/

#ifndef __DATA_TYPES_H
#define __DATA_TYPES_H

#include <stdint.h>

/******************************************************************************/
/*                            Data types                                      */
/******************************************************************************/

#define TRUE_T                  (1)
#define FALSE_T                 (0)


/******************************************************************************/
/*                               Macros                                       */
/******************************************************************************/
#define BIT_SET(slovo, bit)     ((slovo) |= (1 << (bit)))
#define BIT_CLEAR(slovo, bit)   ((slovo) &= ~( 1 << (bit)))
#define BIT_TEST(slovo, bit)    ((slovo) & ( 1 << (bit)))
#define BIT_TOG(slovo, bit)     ((slovo) ^= (bit))
#define MAX(a, b)               (((a) > (b)) ? (a):(b))
#define MIN(a, b)               (((a) < (b)) ? (a):(b))
#define ARRAYSIZE(a)            (sizeof(a) / sizeof(a[0]))


typedef union
UNION_DWORD
{
  //  UNION_DWORD() : data32(0) {}

    uint32_t  data32;
    int32_t   sdata32;
    float dataF;
    uint16_t  data16[2];
    uint8_t   data8[4];
} __UNION_DWORD;

#ifndef PUBLIC
#define PUBLIC
#endif

#endif

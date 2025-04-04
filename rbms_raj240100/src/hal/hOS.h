#ifndef __hOS_H
#define __hOS_H

typedef struct __hOSThread{
    int8_t  name[8];
    int32_t priorty;
} hOSThread;

typedef enum _hOSTHREAD_PRIORITY{
    H_OSTHRD_PRI_LOW =0,
    H_OSTHRD_PRI_NORMAL = 1,
    H_OSTHRD_PRI_HIGH = 2
} _hOSTHREAD_PRIORITY;


#define hOsSetThreadName(x,y )   \
    sprintf(x.name,"%s",y) 


#endif // __hOS_H


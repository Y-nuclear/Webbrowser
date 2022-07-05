#ifndef _STACKWEB_H_
#define _STACKWEB_H_
typedef struct stackNodeWeb *STKW;
typedef struct stackNodeWeb StackW;

STKW PushW(STKW S,char* a);

STKW PopW(STKW S);

char* TopW(STKW S);

void PrintStackW(STKW S);

#endif

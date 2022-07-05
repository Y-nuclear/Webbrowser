#ifndef _STACK_H_
#define _STACK_H_
typedef struct stackNode *STK;
typedef struct stackNode Stack;

STK Push(STK S,int a);

STK Pop(STK S);

int Top(STK S);

void PrintStack(STK S);

#endif

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
struct stackNode{
	int type;
	struct stackNode* next;
};

void PrintStack(STK S)
{
	if(S==NULL)return;
	//printf("%d ",S->type);
	PrintStack(S->next);
}
int Top(STK S)
{
	return S->type;
}

STK Push(STK S,int a)
{
	STK header=(STK)malloc(sizeof(Stack));
	header->type=a;
	header->next=S;
	return header;
}

STK Pop(STK S)
{
	STK p=S->next;
	free(S);
	return p;
}

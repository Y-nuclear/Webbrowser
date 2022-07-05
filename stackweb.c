#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stackw.h"

void PrintStackW(LNW S)
{
	if(S==NULL)return;
	//printf("%d ",S->type);
	PrintStack(S->past);
}
char* TopW(LNW S)
{
	return S->Web_site;
}

LNW AddW(LNW S,char* a)
{
	LNW header=(LNW)malloc(sizeof(ListW));
	strcpy(header->Web_site,a);
	header->next=NULL;
	if(S==NULL)
		{
			header->past=NULL;
			return header;
		}
	header->past=S;
	FreeW(S->next);
	S->next=header;
	header->next=NULL;
	return header;
}

void FreeW(LNW S)
{
	LNW p=S;
	while(S!=NULL)
	{
		S=S->next;
		free(p);
		p=S;
	}
}

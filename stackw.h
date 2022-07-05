#ifndef _STACKWEB_H_
#define _STACKWEB_H_
typedef struct ListNodeWeb *LNW;
typedef struct ListNodeWeb ListW;

LNW AddW(LNW S,char* a);

void FreeW(LNW S);

char* TopW(LNW S);

void PrintStackW(LNW S);
struct ListNodeWeb{
	char Web_site[100];
	struct ListNodeWeb* next;
	struct ListNodeWeb* past;
};
char *DNSServer(char *Domain);
int NetConnect(char *Website);
#endif

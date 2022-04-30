#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HTML_TEXT 0
#define HTML_BODY 1
#define HTML_A 2
#define HTML_P 3
#define HTML_B 4
#define HTML_HTML 5
static char *TypeName[20]={
"body",
"a",
"p",
"b",
"html"
};

typedef struct TokenAttribute{
	char *Name;
	char *Tail;
	struct TokenAttribute *next;
}TokenAttr,*TKA;

typedef struct Token{
	int type;
	char *Text;
	TKA Attr;
	int endflag;
}Newtoken,*NK;

typedef struct TokenList{
	NK token;
	struct TokenList* next;
	struct TokenList* past;
}Newtokenlist,*NTK;

NK GetToken(char *s);
NK GetText(char *s);
int GetType(char *s);
TKA GetAttr(char *s);
void Print(NTK T)
{
	if(T==NULL)return;
	if(T->token!=NULL)
	if(T->token->Text!=NULL)
	printf("%s\n",T->token->Text);
	else if(T->token->Text==NULL)
	printf("%d\n",T->token->type);
	else if(T->token==NULL)
	printf("NULL\n");
	Print(T->next);
}
int main()
{
	NTK Header=(NTK)malloc(sizeof(Newtokenlist)),p=Header;
	FILE *fp=fopen("HTML.html","r");
	char c=fgetc(fp);
	while(c!=EOF)
	{
		if(c==' '||c=='\n'||c=='>')
		{
			c=fgetc(fp);
			 continue;
		}
			NTK NewNode=(NTK)malloc(sizeof(Newtokenlist));
			p->next=NewNode;
			NewNode->past=p;
			NewNode->next=NULL;
			p=p->next;
			char s[100]={0};
			int i=0;
			if(c=='<')
			{
				do
				{
					s[i++]=c;
				}while((c!='>')&&(c=fgetc(fp))!=EOF);
				NewNode->token=GetToken(s);
			}
			else
			{
				do
				{
					s[i++]=c;
				}while((c=fgetc(fp))!=EOF&&c!=' '&&c!='\n'&&c!='<');
				NewNode->token=GetText(s);
			}
			
	}
	Print(Header->next);
}
NK GetToken(char *s)
{
	NK NewTokenNode=(NK)malloc(sizeof(Newtoken));
	NewTokenNode->endflag=0;
	int i=1,j=0;
	if(s[i]=='/')
	{
		NewTokenNode->endflag=1;
		i++;
	}
	char Name[20]={0};
	while(s[i]!=' '&&s[i]!='\n'&&s[i]!=0&&s[i]!='>')
	{
		Name[j++]=s[i++];
	}
	Name[j]=0;
	NewTokenNode->type=GetType(Name);
	NewTokenNode->Text=NULL;
	NewTokenNode->Attr=(TKA)GetAttr(&s[i]);
	return NewTokenNode;
}
TKA GetAttr(char *s)
{
	TKA NewTAttr=(TKA)malloc(sizeof(TokenAttr));
	if(s[0]==0||s[0]=='>')
	return NULL;
	char AttrName[20]={0},AttrTail[20]={0};
	int i=0,flag=0,namecount=0,tailcount=0;
	while(s[i]==' '||s[i]=='\n')
	{
		i++;
	}
	while(s[i]!=' '&&s[i]!='\n'&&s[i]!=0)
	{
		if(s[i]=='=')flag=1;
		if(!flag)AttrName[namecount++]=s[i++];
		else if(flag)AttrTail[tailcount++]=s[i++];
	}
	AttrName[namecount]=AttrTail[tailcount]=0;
	NewTAttr->Name=(char *)malloc(sizeof(char)*(strlen(AttrName)+1));
	NewTAttr->Tail=(char *)malloc(sizeof(char)*(strlen(AttrTail)+1));
	NewTAttr->next=GetAttr(&s[i]);
	return NewTAttr;
}
int GetType(char *s)
{
	int i=0;
	while(TypeName[i]!=NULL)
	{
		if(strcmp(s,TypeName[i])==0)return i+1;
		i++;
	}
	return -1;
}

NK GetText(char *s)
{
	NK NewTokenNode=(NK)malloc(sizeof(Newtoken));
	NewTokenNode->type=HTML_TEXT;
	NewTokenNode->Text=(char *)malloc(sizeof(char)*(strlen(s)+1));
	strcpy(NewTokenNode->Text,s);
	NewTokenNode->Attr=NULL;
	return NewTokenNode;
}

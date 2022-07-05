#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "html.h"

static char *TypeName[100]={
"text",       
"!DOCTYPE",
"body",            
"a",
"p",
"b",
"html",
"meta",
"title",
"script",
"li",
"small",
"strong",
"span",
"abbr",
"br",
"caption",
"em",
"code",
"div",
"h1",
"h2",
"h3",
"h4",
"h5",
"h6",
"hr",
"big",
"i",
"tt",
"ol",
"ul",
"mark",
"nav",
"var",
"cite",
"wbr",
"table",
"tr",
"th",
"td",
"thead",
"tbody",
"tfoot",
"ins",
"del",
};


void PrintAttr(TKA Attr)
{
	if(Attr==NULL)return ;
	printf("%s = %s \n",Attr->Name,Attr->Tail);
	PrintAttr(Attr->next); 
}
void Print(NTK T)
{
	if(T==NULL)return;
	if(T->token!=NULL)
	if(T->token->Text!=NULL)
	printf("%s\n",T->token->Text);
	else if(T->token->Text==NULL)
	{
		printf("%d\n",T->token->type);
		if(T->token->Attr!=NULL)
		{
			PrintAttr(T->token->Attr);
			}	
	}
	
	else if(T->token==NULL)
	printf("NULL\n");
	Print(T->next);
}
NTK DomTree(FILE *fp)
{
	NTK Header=(NTK)malloc(sizeof(Newtokenlist)),p=Header;
	Header->next=NULL;
	char c=fgetc(fp);
	while(c!=EOF)
	{
		if(c==' '||c=='\n'||c=='>'||c=='\t'||c=='\r')
		{
			c=fgetc(fp);
			 continue;
		}
			NTK NewNode=(NTK)malloc(sizeof(Newtokenlist));
			p->next=NewNode;
			NewNode->past=p;
			NewNode->next=NULL;
			p=p->next;
			char s[2048]={0};
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
				}while((c=fgetc(fp))!=EOF&&c!=' '&&c!='\n'&&c!='<'&&c!='\r');
				NewNode->token=GetText(s);
			}
			
	}
	return Header->next;
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
	while(s[i]!=' '&&s[i]!='\n'&&s[i]!=0&&s[i]!='>'&&s[i]!='/')
	{
		Name[j++]=s[i++];
	}
	Name[j]=0;
	if(!strcmp(Name,"br")||!strcmp(Name,"hr"))
		NewTokenNode->endflag=1;
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
	char AttrName[1024]={0},AttrTail[1024]={0};
	int i=0,flag=0,namecount=0,tailcount=0;
	while(s[i]==' '||s[i]=='\n')
	{
		i++;
	}
	while(s[i]!=' '&&s[i]!='\n'&&s[i]!=0&&s[i]!='>')
	{
		if(s[i]=='='&&flag==0)
		{
			flag=1;	
			i++;
			continue;
		}
		if(!flag)AttrName[namecount++]=s[i++];
		else if(flag)AttrTail[tailcount++]=s[i++];
	}
	AttrName[namecount]=AttrTail[tailcount]=0;
	NewTAttr->Name=(char *)malloc(sizeof(char)*(strlen(AttrName)+1));
	strcpy(NewTAttr->Name,AttrName);
	NewTAttr->Tail=(char *)malloc(sizeof(char)*(strlen(AttrTail)+1));
	strcpy(NewTAttr->Tail,AttrTail); 
	NewTAttr->next=GetAttr(&s[i]);
	return NewTAttr;
}
int GetType(char *s)
{
	int i=0;
	while(TypeName[i]!=NULL)
	{
		if(strcmp(s,TypeName[i])==0)return i;
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
void FreeDomTree(NTK Header)
{
	if(Header==NULL)
		return;
	else
	{
		FreeDomTree(Header->next);
		free(Header);
	}
}

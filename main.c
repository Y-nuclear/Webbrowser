#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <windows.h>
#include <olectl.h>
#include <stdio.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>

#include "html.h"
#include "stack.h"
#include "stackw.h"

#define BT_LEFT 1002
#define BT_RIGHT 1003
#define BT_ENTER 1004
#define BT_REFRESH 1005
#define BT_PERSON 1010
#define BT_NEW 1012
#define BT_HELP 1013
#define BT_ABOUT 1014 
#define EDITBOX_HTML 1006
#define EDITBOX_SEARCH 1007

#define HSCROLL_S1 1008

#define STATIC_SITUATION 1009
#define STATIC_RESULT 1011 

#define OUT_WIDTH 0.9
struct stackNode{
	int type;
	struct stackNode* next;
};

static HINSTANCE hInst;
/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
BOOL DispalyHTML(HWND Hwnd);
void Printf(int subtype,char *s,NTK p,HDC hdc);
void Execute(NTK p,HDC hdc);
void Result_Paint(HWND hWnd,int Flag);
static STK S=NULL;//�洢��ǩ���������ջ 
static LNW List_Web=NULL;//�洢��ҳ��Ϣ��ջ 
static char s[2048]={0};//�洢�������ı����� 
static int ListNum=0;//�����б�ı�� 
static int Width,Height;//�ܴ��ڵĿ�͸� 
static int cux,cuy;//�������ڵ�λ�� 
static int BackForm=0;//���Ի���ʽ 
static char Website[2048]={0};//�洢��ַ 
static SCROLLINFO Scroll;//������ 
void Reverse(char *s);
int WINAPI WinMain_1(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
void Result_Paint(HWND hWnd,int Flag)
{
	RECT Windows;
	GetClientRect(hWnd, &Windows);//��ȡ��ǰ���ڵĴ�С 
	Width=Windows.right-Windows.left;
	Height=Windows.bottom-Windows.top;
	cux=Width*5/100;
	cuy=0;
	HFONT hFont=CreateFont(
            -20/*�߶�*/, -10/*���*/, 0, 0, 400 /*һ�����ֵ��Ϊ400*/,
            FALSE/*����б��*/, FALSE/*�����»���*/, FALSE/*����ɾ����*/,
            DEFAULT_CHARSET,  //ʹ��Ĭ���ַ���
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  //Ĭ���������
            FF_DONTCARE,  //��ָ��������*/
            TEXT("΢���ź�")  //������
        );
	HDC hdc;
	PAINTSTRUCT ps;
	RECT Text;
	hdc = BeginPaint(hWnd, &ps);
	SelectObject(hdc,hFont);
	SetBkMode(hdc,  //�豸�������
    TRANSPARENT  //����ģʽ
	);
	//if(Flag)
	//{
		FILE *fp=fopen("HTML.html","r");
		NTK header=DomTree(fp);
		//Print(header);
		while(header!=NULL)
		{
			
			Execute(header,hdc);
			header=header->next;				
		}
	//}
	EndPaint(hWnd, &ps);
	
}

void Execute(NTK p,HDC hdc)
{
	//�������Ⱦ 
	if (p->token->type==-1)//δ����ı�ǩ 
	{
		if(p->token->endflag==1)
		{
			memset(s,0,2048);
		} 
		return;
	}
	else if(p->token->type==0)//�����ı� 
	{
		strcat(s,p->token->Text);
		strcat(s," ");
		return; 
	}
	else//����ǰ��ǩ 
	{
		if(p->token->endflag==1)
		{
			Printf(p->token->type,s,p,hdc);
			memset(s,0,2048);
		}
		else if(p->token->endflag==0)
		{
			if(S!=NULL&&p->token->type!=HTML_LIST)
			Printf(Top(S),s,p,hdc);
			memset(s,0,2048);
		}
	}
	
	//����һ��ջ�洢��ǩ��� 
	if (p->token->type!=0&&p->token->type!=HTML_LIST)
	{
		if(S==NULL||Top(S)!=p->token->type)
		S=Push(S,p->token->type);
		else if(p->token->endflag==0)
		S=Push(S,p->token->type);
		else
		S=Pop(S);      
	}

}


/*����ִ����������ĺ���*/
void Printf(int subtype,char *s,NTK p,HDC hdc){
	char Listnum[1024]={0};
	static HFONT hfont,hFont_default;
	static int onex,oney;
	COLORREF color;
	TEXTMETRICW NowT;
	static RECT text; 
	switch(subtype){
		case HTML_HTML:
			if(s[0]==0)break;
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			break;
		case HTML_BODY:
			if(s[0]==0)break;
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			break;
		
		//���峬�ı�����.
		case HTML_A:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 400,
            FALSE, TRUE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�")  
        	);
        	SetTextColor(hdc,RGB(0,255,255));
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			SetTextColor(hdc,RGB(0,0,0));
			break;
		
		//�������.
		case HTML_P:
			if(s[0]==0)break;
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			break;
		
		//�����ı�����.
		case HTML_B:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 700,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�")  
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		
		//�����������.
		case HTML_TITLE:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18*2, -9*2, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY, 
            FF_DONTCARE,  
            TEXT("΢���ź�") 
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		
		//�����б��������Ŀ
		case HTML_LIST:
			if(Top(S)==HTML_OL)
			{
				sprintf(Listnum,"%d",++ListNum);
				strcat(Listnum,".");
				strcat(Listnum,s);
				GetTextMetricsW(hdc,&NowT);
				SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
				DrawText(hdc,Listnum,strlen(Listnum),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
				DrawText(hdc,Listnum,strlen(Listnum),&text,DT_LEFT |DT_WORDBREAK);
				cuy+=text.bottom-text.top;
			}
			else
			{
				strcat(Listnum,"��");
				strcat(Listnum,s);
				GetTextMetricsW(hdc,&NowT);
				SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
				DrawText(hdc,Listnum,strlen(Listnum),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
				DrawText(hdc,Listnum,strlen(Listnum),&text,DT_LEFT |DT_WORDBREAK);
				cuy+=text.bottom-text.top;
			}
			break;

		case HTML_SMALL:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18/2, -9/2, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�")  
			        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		
		//�����ı�ǿ�������ּӺ�
		case HTML_STRONG:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�")  
        	);
        	SetTextColor(hdc,RGB(255,0,0));
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			SetTextColor(hdc,RGB(0,0,0));
			break;
		
		//������д������ƶ��������ʱ�����ȫ��
		case HTML_ABBR:

			break;
		
		//����򵥵����У�ʵ���Ͼ��ǻس�
		case HTML_BR:
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			break;
		
		//������ı���(Ҫ���Ͱ�����ͨ���������)
		case HTML_CAPTION:break;
			
		//������������ǿ�������ݣ����ּӴּӺ�
		case HTML_EM:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 700 ,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�")  
        	);
        	SetTextColor(hdc,RGB(255,0,0));
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			SetTextColor(hdc,RGB(0,0,0));
			break;
		case HTML_DIV:
			break;
		
		/*�������ݶ���html���⼶�𣬼���ԽС����Խ��*/
		case HTML_H1:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18*4, -9*4, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�") 
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		case HTML_H2:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18*3, -9*3, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�") 
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		case HTML_H3:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18*2, -9*2, 0, 0, 400 ,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�") 
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		case HTML_H4:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18*1, -9*1, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�") 
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		case HTML_H5:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18*3/4, -9*3/4, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, 
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY, 
            FF_DONTCARE,  
            TEXT("΢���ź�") 
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		case HTML_H6:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18*2/3, -9*2/3, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�")        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		
		//�ڵ�ǰ��������������һ�е���һ�л�һ���ָ���
		case HTML_HR:
			GetTextMetricsW(hdc,&NowT);
			if(s[0]!=0)
			{
				SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
				DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
				DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
				cuy+=(text.bottom-text.top);
			}	
			cuy+=NowT.tmHeight/2;	
			MoveToEx(hdc,cux,cuy,NULL);
			LineTo(hdc,cux+Width*9/10,cuy); 
			cuy+=NowT.tmHeight/2;
			break;
		
		//����������
		case HTML_BIG:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18*2, -9*2, 0, 0, 400 ,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�") 
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		
		//б��
		case HTML_I:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 400 ,
            TRUE, FALSE, FALSE,
            DEFAULT_CHARSET, 
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY, 
            FF_DONTCARE,  
            TEXT("΢���ź�")  
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		
		//�����б�����ÿ��ǰ���ϱ��
		case HTML_OL:
		if(p->token->type==HTML_OL&&p->token->endflag==1)
			ListNum=0;
		break;
			
		//�����б�ÿ��ǰ������ϱ��
		case HTML_UL:break;
		
		//���мǺŵ��ı����ı����������òʱ�Ϳ�ϣ������ʼ�������
		case HTML_MARK:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 400 ,
            TRUE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�") 
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			color=SetBkColor(hdc,RGB(255,125,0)); 
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			SetBkColor(hdc,color);
		break;
		
		//���õ��ı���������������ʾ
		case HTML_CITE:break;
		
		//��ʼ�Ʊ�
		case HTML_TABLE:
			if(p->token->type==HTML_TH||p->token->type==HTML_TD)
			break;
			onex=cux;
			oney=cuy;
		break;
		
		//�������е�һ��
		case HTML_TR:
			if(p->token->type==HTML_TR&&p->token->endflag==1)
			{
				GetTextMetricsW(hdc,&NowT);
				cuy+=NowT.tmHeight;
			}
			
			break;
		
		//�����ͷ
		case HTML_TH:
			GetTextMetricsW(hdc,&NowT);
			strcat(s," | ");
			TextOut(hdc,onex,oney,s,strlen(s));
			onex+=strlen(s)*NowT.tmAveCharWidth;
			break;
		
		//�������ݵ�Ԫ��
		case HTML_TD:
			GetTextMetricsW(hdc,&NowT);
			strcat(s," | ");
			TextOut(hdc,onex,oney,s,strlen(s));
			onex+=strlen(s)*NowT.tmAveCharWidth;
			break;
		
		//���嵥Ԫ���еı�ͷ
		case HTML_THEAD:break;
		
		//���嵥Ԫ���м䲿��
		case HTML_TBODY:break;
		
		//���嵥Ԫ��β��
		case HTML_TFOOT:break;
		
		//���ı��¼����»���
		case HTML_INS:
			hfont = CreateFont(
            -18, -9, 0, 0, 400 ,
            FALSE, TRUE, FALSE,
            DEFAULT_CHARSET,
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,
            FF_DONTCARE,  
            TEXT("΢���ź�")
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		
		//���ı����ɾ����
		case HTML_DEL:
			hfont = CreateFont(
            -18*2, -9*2, 0, 0, 400 ,
            FALSE, FALSE, TRUE,
            DEFAULT_CHARSET, 
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY, 
            FF_DONTCARE,  
            TEXT("΢���ź�") 
        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		default:break;
		
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	HWND hWnd;
	MSG Msg; 
	FILE *File,*HTMLFile;
	char Middle[100]={0};
	hInst=hInstance;
	File=fopen("file.txt","r");
    HTMLFile=fopen("HTML.html","w");
    while(fgets(Middle,100,File)!=NULL)
    {
       	fputs(Middle,HTMLFile);
    	memset(Middle,0,100);
    }
    fputs(Middle,HTMLFile);	
	
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; 
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); 
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); 

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "����ע��ʧ��","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","��ҳ�����",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		GetSystemMetrics(SM_CXSCREEN), 
		GetSystemMetrics(SM_CYSCREEN), 
		NULL,NULL,hInstance,NULL);
	if(hWnd == NULL) {
		MessageBox(NULL, "���ڴ���ʧ��!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	

	while(GetMessage(&Msg, NULL, 0, 0) > 0) { 
		TranslateMessage(&Msg); 
		DispatchMessage(&Msg); 
	}
	return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND Hwnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	int i,j,k;//ѭ��ʹ�ñ���
    int wmId, wmEvent,EnterFlag=0;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT Windows,ResultPiant,Text;
    TCHAR HTMLEditText[]="HTML������ʾ��";
    TCHAR ResultPaint[]="�����ʾ��";

	GetClientRect(Hwnd, &Windows);//��ȡ��ǰ���ڵĴ�С 
	Width=Windows.right-Windows.left;
	Height=Windows.bottom-Windows.top;
    static HFONT hFont,HFontSearch;
    static HWND ButtonLeft,ButtonRight,ButtonEnter,ButtonRefresh,
		ButtonPerson,ButtonNew,ButtonAbout,ButtonHelp;//��ť 
    static HWND HTMLEdit,SearchEdit;//�ɱ༭�ı��� 
    static HWND hScroll;//������ 
    static HWND Situation,BackGround,Result;//״̬�� 
    static HPEN hpen,hpen_Default;//����
 	static HBRUSH hbrush,hbrush_Default;//��ˢ   
    static int cxSize, cySize,yPos;
    static int iScrollWidth;
    static HMENU Hmenu,Hmenu_1;//�˵� 
    FILE *File,*HTMLFile;
    
    COLORREF Color_Ref[7]={
	RGB(255,0,0),
	RGB(255,125,0),
	RGB(255,255,0),
	RGB(0,255,0),
	RGB(0,0,255),
	RGB(0,255,255),
	RGB(255,0,255)
	};//��ɫ�洢���� 
    
    char Middle[100]={0};

    switch (message) {
    case  WM_CREATE:
    	iScrollWidth = GetSystemMetrics(SM_CXVSCROLL);
        GetClientRect(Hwnd, &Windows);
        cxSize = Windows.right;
        cySize = Windows.bottom;
        Scroll.cbSize = sizeof(Scroll);
        Scroll.fMask = SIF_ALL;
        Scroll.nMin = 0;
        Scroll.nMax = 100;
        Scroll.nPage = 10;
        Scroll.nPos = 0;
        
        //�����߼�����
        hFont = CreateFont(
            -18, -9, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("΢���ź�")
        );
        
        HFontSearch = CreateFont(
            -18*3/2, -9*3/2, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("����")  
	        );
        SelectObject(hdc,hFont);
        SearchEdit = CreateWindow(
            TEXT("edit"),  //��̬�ı��������
            TEXT("Http://"),  //�ؼ����ı�
            WS_CHILD /*�Ӵ���*/ | WS_VISIBLE /*����ʱ��ʾ*/ | WS_BORDER /*���߿�*/|ES_AUTOHSCROLL,
            Width*3/10 /*X����*/, Height*70/1000 /*Y����*/, Width*4/10 /*���*/, Height*50/1000/*�߶�*/,
			Hwnd/*�����ھ��*/,
            (HMENU)EDITBOX_SEARCH,  //Ϊ�ؼ�ָ��һ��Ψһ��ʶ��
            hInst,  //��ǰʵ�����
            NULL
        );
        //����HTML�ı��� 
        HTMLEdit = CreateWindow(
            TEXT("edit"),  
            TEXT(""),  
            WS_CHILD| WS_VISIBLE| WS_BORDER |ES_MULTILINE,
            Width/20 , Height/4, Width*2/5, Height*6/10, Hwnd,
            (HMENU)EDITBOX_HTML,  
            hInst,  
            NULL
        );
        //״̬�� 
        Situation = CreateWindow(
            TEXT("static"),  
            TEXT("�������"), 
            WS_CHILD | WS_VISIBLE | WS_BORDER ,
            Width/20 , Height*88/100, Width*1/5, Height*4/100, Hwnd,
            (HMENU)STATIC_SITUATION,  
            hInst,  
            NULL
        );
        //������ť�ؼ�
        //���˰�ť 
        ButtonLeft = CreateWindow(
            TEXT("button"),  //��ť�ؼ�������
            TEXT("����"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 
            Width*5/100 , Height*5/100 , Width*8/100 , Height*75/1000,
            Hwnd, (HMENU)BT_LEFT , hInst, NULL
        );
		//ǰ����ť 
        ButtonRight = CreateWindow(
            TEXT("button"),  
            TEXT("ǰ��"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON,
            Width*18/100 , Height*5/100 , Width*8/100, Height*75/1000,
            Hwnd, (HMENU)BT_RIGHT , hInst, NULL
        );
        //���� 
        ButtonEnter = CreateWindow(
            TEXT("button"),  
            TEXT("����"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 
            Width*7/10 , Height*7/100 , Width*4/100 , Height*5/100,
            Hwnd, (HMENU)BT_ENTER , hInst, NULL
        );
        //ˢ�°�ť 
        ButtonRefresh = CreateWindow(
            TEXT("button"),  
            TEXT("ˢ��"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 
            Width*75/100 , Height*6/100 , Width*7/100 , Height*60/1000,
            Hwnd, (HMENU)BT_REFRESH , hInst, NULL
        );
        //���Ի���ť 
        ButtonPerson = CreateWindow(
            TEXT("button"),  
            TEXT("���Ի�"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 
            Width*83/100 , Height*6/100 , Width*7/100 , Height*6/100,
            Hwnd, (HMENU)BT_PERSON , hInst, NULL
        );
        
        ButtonNew = CreateWindow(
            TEXT("button"),  
            TEXT("�´���"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON,
            Width*91/100 , Height*6/100 , Width*7/100 , Height*6/100,
            Hwnd, (HMENU)BT_NEW , hInst, NULL
        );
        ButtonHelp = CreateWindow(
            TEXT("button"),  
            TEXT(""),
            WS_CHILD, 
            Width*91/100 , Height*6/100 , Width*7/100, Height*6/100,
            Hwnd, (HMENU)BT_HELP , hInst, NULL
        );
        ButtonAbout = CreateWindow(
            TEXT("button"),  
            TEXT(""),
            WS_CHILD , 
            Width*91/100 , Height*6/100 , Width*7/100 , Height*6/100,
            Hwnd, (HMENU)BT_ABOUT , hInst, NULL
        );
        //�����ʾλ��
		Result = CreateWindow(
            TEXT("static"),  
            TEXT(""),  
            WS_CHILD  | WS_VISIBLE  | WS_BORDER ,
            Width*50/100, Height*25/100, Width*2/5, Height, Hwnd,
            (HMENU)STATIC_RESULT,  
            hInst, 
            NULL
        );
        Hmenu = CreateMenu();
        Hmenu_1 = CreateMenu();
        AppendMenu(Hmenu_1,MF_STRING,BT_NEW,TEXT("�½���Ctrl+N��"));
		AppendMenu(Hmenu_1,MF_STRING,BT_RIGHT,TEXT("ǰ����Ctrl+R��"));
		AppendMenu(Hmenu_1,MF_STRING,BT_LEFT,TEXT("���ˣ�Ctrl+L��"));
		AppendMenu(Hmenu_1,MF_STRING,BT_REFRESH,TEXT("ˢ�£�Ctrl+F��"));
		AppendMenu(Hmenu_1,MF_STRING,BT_PERSON,TEXT("���Ի���Ctrl+P��"));
        AppendMenu(Hmenu,MF_POPUP,(unsigned int)Hmenu_1, TEXT("�˵�"));
        AppendMenu(Hmenu, MF_STRING,BT_HELP, TEXT("������Ctrl+H��"));
        AppendMenu(Hmenu, MF_STRING,BT_ABOUT, TEXT("�йأ�Ctrl+A��"));
        SetMenu(Hwnd,Hmenu);
        
        hScroll= CreateWindowEx( 
        0,                   
        TEXT("SCROLLBAR"),       
        (PTSTR) NULL,             
        WS_OVERLAPPEDWINDOW | WS_VSCROLL, 
        CW_USEDEFAULT,         
        CW_USEDEFAULT,         
        CW_USEDEFAULT,         
        CW_USEDEFAULT,         
        (HWND) Result,           
        (HMENU) NULL,          
        hInst,              
        (PVOID) NULL           
    );
            if (!hScroll) MessageBox(Hwnd, TEXT("Failed"), TEXT("create scroll control"), MB_OK);
        SetScrollInfo(hScroll, SB_CTL, &Scroll, TRUE);
        SetScrollInfo(Hwnd, SB_VERT, &Scroll, TRUE);
		 
        SendMessage(ButtonEnter, WM_SETFONT, (WPARAM)HFontSearch, 1);
        SendMessage(ButtonLeft, WM_SETFONT, (WPARAM)HFontSearch, 1);
        SendMessage(ButtonRight, WM_SETFONT, (WPARAM)HFontSearch, 1);
        SendMessage(ButtonRefresh, WM_SETFONT, (WPARAM)HFontSearch, 1);
        SendMessage(ButtonPerson, WM_SETFONT, (WPARAM)HFontSearch, 1);
        SendMessage(ButtonNew, WM_SETFONT, (WPARAM)HFontSearch, 1);
  		SendMessage(SearchEdit, WM_SETFONT, (WPARAM)HFontSearch, 0);//���ð�ť���� 
  		DispalyHTML(HTMLEdit);
  		
        break;
    case WM_COMMAND://���� 
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        switch (wmId) {
        case BT_LEFT:  //���°�ť
            if(List_Web==NULL)
        	{
        		SetWindowText(Situation,TEXT("û�����������ҳ")); 
        	}
        	else if(List_Web->past==NULL)
        	{
        		SetWindowText(Situation,TEXT("ǰ��û��·��")); 
        	}
        	else if(List_Web->past!=NULL)
        	{
			List_Web=List_Web->past;             
        	strcpy(Website,List_Web->Web_site);
        	SetWindowText(SearchEdit,Website);
        	if(Website[0]!=0&&Website[0]=='H'&&Website[1]!=0&&Website[1]=='t'
			&&Website[2]!=0&&Website[2]=='t'&&Website[3]!=0&&Website[3]=='p'
			&&Website[4]!=0&&Website[4]==':'&&Website[5]!=0&&Website[5]=='/')
			{
				SetWindowText(Situation,TEXT("������..."));
				NetConnect(Website);
			}
        	else if(Website[0]!=0)
        	{
        		SetWindowText(Situation,TEXT("�ļ�����..."));
        		File=fopen(Website,"r");
        		HTMLFile=fopen("HTML.html","w");
        		while(fgets(Middle,100,File)!=NULL)
        		{
        			fputs(Middle,HTMLFile);
        			memset(Middle,0,100);
        		}
        		fputs(Middle,HTMLFile);	
        	}
        	RedrawWindow(Result,NULL,NULL,RDW_INTERNALPAINT|RDW_UPDATENOW );
        	InvalidateRect(Result,NULL,TRUE);
        	UpdateWindow(Result);
        	RedrawWindow(Hwnd,NULL,NULL,RDW_INTERNALPAINT|RDW_UPDATENOW );
        	InvalidateRect(Hwnd,NULL,TRUE);
        	UpdateWindow(Hwnd);
        	DispalyHTML(HTMLEdit);
        	SetWindowText(Situation,TEXT("�������"));
        	}
            break;
        case BT_RIGHT:
        	if(List_Web==NULL)
        	{
        		SetWindowText(Situation,TEXT("û�����������ҳ")); 
        	}
        	else if(List_Web->next==NULL)
        	{
        		SetWindowText(Situation,TEXT("����û��·��")); 
        	}
        	else if(List_Web->next!=NULL)
        	{
			List_Web=List_Web->next;             
        	strcpy(Website,List_Web->Web_site);
        	SetWindowText(SearchEdit,Website);
        	if(Website[0]!=0&&Website[0]=='H'&&Website[1]!=0&&Website[1]=='t'
			&&Website[2]!=0&&Website[2]=='t'&&Website[3]!=0&&Website[3]=='p'
			&&Website[4]!=0&&Website[4]==':'&&Website[5]!=0&&Website[5]=='/')
			{
				SetWindowText(Situation,TEXT("������..."));
				NetConnect(Website);
			}
        	else if(Website[0]!=0)
        	{
        		SetWindowText(Situation,TEXT("�ļ�����..."));
        		File=fopen(Website,"r");
        		HTMLFile=fopen("HTML.html","w");
        		while(fgets(Middle,100,File)!=NULL)
        		{
        			fputs(Middle,HTMLFile);
        			memset(Middle,0,100);
        		}
        		fputs(Middle,HTMLFile);	
        	}
        	RedrawWindow(Result,NULL,NULL,RDW_INTERNALPAINT|RDW_UPDATENOW );
        	InvalidateRect(Result,NULL,TRUE);
        	UpdateWindow(Result);
        	RedrawWindow(Hwnd,NULL,NULL,RDW_INTERNALPAINT|RDW_UPDATENOW );
        	InvalidateRect(Hwnd,NULL,TRUE);
        	UpdateWindow(Hwnd);
        	DispalyHTML(HTMLEdit);
        	SetWindowText(Situation,TEXT("�������"));
        	}
            break;
        case BT_ENTER:
        	GetWindowText(SearchEdit,Website,2048);
        	
        	if(Website[0]!=0&&Website[0]=='H'&&Website[1]!=0&&Website[1]=='t'
			&&Website[2]!=0&&Website[2]=='t'&&Website[3]!=0&&Website[3]=='p'
			&&Website[4]!=0&&Website[4]==':'&&Website[5]!=0&&Website[5]=='/')
			{
				SetWindowText(Situation,TEXT("������..."));
				NetConnect(Website);
				//MessageBox(Hwnd,Website,TEXT("��ҳ��"),MB_OK);
			}
        	else if(Website[0]!=0&&Website[0]!='H')
        	{
        		SetWindowText(Situation,TEXT("�ļ�����..."));
        		File=fopen(Website,"r");
        		HTMLFile=fopen("HTML.html","w");
        		while(fgets(Middle,100,File)!=NULL)
        		{
        			fputs(Middle,HTMLFile);
        			memset(Middle,0,100);
        		}
        		fputs(Middle,HTMLFile);
        		fclose(HTMLFile);
        		fclose(File);
        	}
        	if(Website[0]!=0)
        	{
        		List_Web=AddW(List_Web,Website);
        	}
        	RedrawWindow(Result,NULL,NULL,RDW_INTERNALPAINT|RDW_UPDATENOW );
        	InvalidateRect(Result,NULL,TRUE);
        	UpdateWindow(Result);
        	RedrawWindow(Hwnd,NULL,NULL,RDW_INTERNALPAINT|RDW_UPDATENOW );
        	InvalidateRect(Hwnd,NULL,TRUE);
        	UpdateWindow(Hwnd);

        	DispalyHTML(HTMLEdit);
        	SetWindowText(Situation,TEXT("�������"));
        	break; 
        case BT_REFRESH:
        	SendMessage(Hwnd,WM_PAINT,wParam,lParam);
        	DispalyHTML(HTMLEdit);
        	UpdateWindow(Result);
        	SetWindowText(Situation,TEXT("ˢ�����"));
        	break;
        case BT_PERSON:
        	if(BackForm>=5)
        		BackForm=0;
        	else
        		BackForm++;
        	RedrawWindow(Hwnd,NULL,NULL,RDW_INTERNALPAINT|RDW_UPDATENOW );
        	InvalidateRect(Hwnd,NULL,TRUE);
        	UpdateWindow(Hwnd);
        	break;
        case BT_NEW:
			ShellExecute(0,"open", "Project.exe", 0, 0, 1); 
			break;
		case BT_ABOUT:
			MessageBox(Hwnd,TEXT("�����򴴽�һ���������������ʵ����ҳ��ǰ�������ˡ�\n������ҳ��ˢ�¡���ת�ȹ��ܡ�\n��ǰ�汾Ϊ2.1.1��\n�汾�������ڣ�1/6/2022"),TEXT("�й�"),MB_OK); 
		break;
		case BT_HELP:
			MessageBox(Hwnd,TEXT("1��ǰ�����������ַ������ʾ��һ����ַ������һ����ַΪ�գ�������ʾԭ��ҳ�����ݣ�״̬����ʾ��ǰ��û��·����\n2�����ˣ��������ַ������ʾ��һ����ַ������һ����ַΪ�գ�������ʾԭ��ҳ�����ݣ�״̬����ʾ������û��·����\n3�����루��ת��������ַ���������ʽ��http://)��ȷ����ַ����������Կ����ʾ������ʾ����ҳ�����ݡ�\n4��ˢ�£�����������ͼ��ҳ�����ˢ�£�����ַ����ʾ�иĶ���û�е�����룬��ˢ�º���ʾ�Ķ�ǰ���ݡ�\n5����ҳ���������ʾ��ҳ��\n6�����Ի���������л�������ɫ����\n6��˵��������󣬻���ҳ�����ĵ�����ǩ��˵���������ܡ�\n7���йأ�����󣬻���ҳ�����ĵ�����ǩ����ʾ�й���Ϣ��"),TEXT("����"),MB_OK); 
        default:
            //���������Ϣ���� DefWindowProc ����
            return DefWindowProc(Hwnd, message, wParam, lParam);
        }
        break;
        
    case WM_VSCROLL://������Ϣ���� 
        Scroll.cbSize = sizeof (Scroll);
        Scroll.fMask  = SIF_ALL;
        GetScrollInfo (Hwnd, SB_VERT, &Scroll);
		yPos=Scroll.nPos;
        switch (LOWORD (wParam))
        {
        case SB_TOP:
            Scroll.nPos = Scroll.nMin;
            break;
        case SB_BOTTOM:
            Scroll.nPos = Scroll.nMax;
            break;
        case SB_LINEUP:
            Scroll.nPos -= 1;
            break;
        case SB_LINEDOWN:
            Scroll.nPos += 1;
            break;
        case SB_PAGEUP:
            Scroll.nPos -= Scroll.nPage;
            break;
        case SB_PAGEDOWN:
            Scroll.nPos += Scroll.nPage;
            break;
        case SB_THUMBTRACK:
            Scroll.nPos = Scroll.nTrackPos;
            break;
              
        default:
            break; 
        }
        Scroll.fMask = SIF_POS;
        SetScrollInfo (Hwnd, SB_VERT, &Scroll, TRUE);
        GetScrollInfo (Hwnd, SB_VERT, &Scroll);
    	if(Scroll.nPos-yPos)
    	{
    		ScrollWindow(Hwnd,0,cuy*(-Scroll.nPos+yPos)/(Scroll.nMax-Scroll.nMin),NULL,NULL);
        	InvalidateRect(Hwnd,NULL,TRUE);
        	UpdateWindow(Hwnd);	
    	}
        break;
    case WM_KEYDOWN://��ݼ������� 
            {
            int iState = GetKeyState(VK_CONTROL);
                if (iState<0)
                    {
                    	switch(wParam)
                    	{
                    		case TEXT('N'):SendMessage(Hwnd,WM_COMMAND,(WPARAM)BT_NEW,0);break;
                    		case TEXT('P'):SendMessage(Hwnd,WM_COMMAND,(WPARAM)BT_PERSON,0);break;
                    		case TEXT('F'):SendMessage(Hwnd,WM_COMMAND,(WPARAM)BT_REFRESH,0);break;
                    		case TEXT('L'):SendMessage(Hwnd,WM_COMMAND,(WPARAM)BT_LEFT,0);break;
                    		case TEXT('R'):SendMessage(Hwnd,WM_COMMAND,(WPARAM)BT_RIGHT,0);break;
                    		case TEXT('E'):SendMessage(Hwnd,WM_COMMAND,(WPARAM)BT_ENTER,0);break;
                    		case TEXT('A'):SendMessage(Hwnd,WM_COMMAND,(WPARAM)BT_ABOUT,0);break;
                    		case TEXT('H'):SendMessage(Hwnd,WM_COMMAND,(WPARAM)BT_HELP,0);break;
                    		//case TEXT('C'):SendMessage(Hwnd,WM_COMMAND,(WPARAM)(unsigned int)Hmenu_1,0);break;
                    		default:break;
                    	}
                        
                    }

                
            }
    case WM_SIZE:

        return 0;
    case WM_PAINT:
		hdc = BeginPaint(Hwnd, &ps);
		SetBkMode(hdc,  //�豸�������
    	TRANSPARENT  //����ģʽ
		);
		SelectObject(hdc,hFont);
    	EndPaint(Hwnd,&ps);
        Result_Paint(Result,EnterFlag);
        break;
        
    case WM_ERASEBKGND: 
    	hdc = (HDC) wParam;
    	switch(BackForm)
    	{
    		case 0:
    			hbrush = CreateSolidBrush(RGB(96,96,96));	
    			hbrush_Default=(HBRUSH)SelectObject(hdc,hbrush);
    			for(i=0;i<3;i++)
    			{
    				for(j=0;j<4;j++)
    				{
    				hbrush = CreateSolidBrush(RGB(48+(i+1)*(j+1)*16,48+(i+1)*(j+1)*16,48+(i+1)*(j+1)*16));
    				SelectObject(hdc,hbrush);
    				Rectangle(hdc,0+j*Width/4,0+i*Height/3,Width/4+j*Width/4+1,Height/3+i*Height/3+1);
    				DeleteObject(hbrush);	
    				}
				} 
    			SelectObject(hdc,hpen_Default);
			break;
			case 1:
    			hbrush = CreateSolidBrush(RGB(96,96,96));	
    			hbrush_Default=(HBRUSH)SelectObject(hdc,hbrush);
    			for(i=0;i<3;i++)
    			{
    				for(j=0;j<4;j++)
    				{
    				hbrush = CreateSolidBrush(RGB(24+(i+1)*(j+1)*8,48+(i+1)*(j+1)*16,36+(i+1)*(j+1)*12));
    				SelectObject(hdc,hbrush);
    				Rectangle(hdc,0+j*Width/4,0+i*Height/3,Width/4+j*Width/4+1,Height/3+i*Height/3+1);
    				DeleteObject(hbrush);	
    				}
				} 
    			SelectObject(hdc,hpen_Default);
			break;
			case 2:
    			hbrush = CreateSolidBrush(RGB(96,96,96));	
    			hbrush_Default=(HBRUSH)SelectObject(hdc,hbrush);
    			for(i=0;i<3;i++)
    			{
    				for(j=0;j<4;j++)
    				{
    				hbrush = CreateSolidBrush(RGB(48+(i+1)*(j+1)*16,24+(i+1)*(j+1)*8,36+(i+1)*(j+1)*12));
    				SelectObject(hdc,hbrush);
    				Rectangle(hdc,0+j*Width/4,0+i*Height/3,Width/4+j*Width/4+1,Height/3+i*Height/3+1);
    				DeleteObject(hbrush);	
    				}
				} 
    			SelectObject(hdc,hpen_Default);
			break;
			case 3:
    			hbrush = CreateSolidBrush(RGB(96,96,96));	
    			hbrush_Default=(HBRUSH)SelectObject(hdc,hbrush);
    			for(i=0;i<3;i++)
    			{
    				for(j=0;j<4;j++)
    				{
    				hbrush = CreateSolidBrush(RGB(48+(i+1)*(j+1)*16,36+(i+1)*(j+1)*12,24+(i+1)*(j+1)*8));
    				SelectObject(hdc,hbrush);
    				Rectangle(hdc,0+j*Width/4,0+i*Height/3,Width/4+j*Width/4+1,Height/3+i*Height/3+1);
    				DeleteObject(hbrush);	
    				}
				} 
    			SelectObject(hdc,hpen_Default);
			break;
			case 4:
    			hbrush = CreateSolidBrush(RGB(96,96,96));	
    			hbrush_Default=(HBRUSH)SelectObject(hdc,hbrush);
    			for(i=0;i<3;i++)
    			{
    				for(j=0;j<4;j++)
    				{
    				hbrush = CreateSolidBrush(RGB(36+(i+1)*(j+1)*12,24+(i+1)*(j+1)*8,48+(i+1)*(j+1)*16));
    				SelectObject(hdc,hbrush);
    				Rectangle(hdc,0+j*Width/4,0+i*Height/3,Width/4+j*Width/4+1,Height/3+i*Height/3+1);
    				DeleteObject(hbrush);	
    				}
				} 
    			SelectObject(hdc,hpen_Default);
			break;
			case 5:
    			hbrush = CreateSolidBrush(RGB(96,96,96));	
    			hbrush_Default=(HBRUSH)SelectObject(hdc,hbrush);
    			for(i=0;i<3;i++)
    			{
    				for(j=0;j<4;j++)
    				{
    				hbrush = CreateSolidBrush(RGB(24+(i+1)*(j+1)*8,36+(i+1)*(j+1)*12,48+(i+1)*(j+1)*16));
    				SelectObject(hdc,hbrush);
    				Rectangle(hdc,0+j*Width/4,0+i*Height/3,Width/4+j*Width/4+1,Height/3+i*Height/3+1);
    				DeleteObject(hbrush);	
    				}
				} 
    			SelectObject(hdc,hpen_Default);
			break;
    		default:break;
    	}
  	break;
	//case WM_CTLCOLORSTATIC:
    //        return (INT_PTR)CreateSolidBrush(RGB(0x41, 0x96, 0x4F)); 
    case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(Hwnd, message, wParam, lParam);
    }
    return 0;
}

BOOL DispalyHTML(HWND Hwnd)
{
	char a[2048*10]="";
	FILE *WEB;
	int i=0;
	WEB = fopen("HTML.html","r+");
	char HTMLText[80]={0};	
	//���� 
	while(fgets(HTMLText,80,WEB)!=NULL){
		if(strcmp(HTMLText,"\n")==0)
			continue;
	if(HTMLText[strlen(HTMLText)-1]=='\n'||HTMLText[strlen(HTMLText)-1]=='\r'||HTMLText[strlen(HTMLText)-1]=='\t')
	HTMLText[strlen(HTMLText)-1]=0;
	strcat(a,HTMLText);
	strcat(a,"\r\n");
	i++; 
	}
	SetWindowText(Hwnd, TEXT(a));
}

void Reverse(char *s)
{
	int i=0,j=strlen(s)-1;
	while(i<j)
	{
		char c=s[i];
		s[i]=s[j];
		s[j]=c;
		j--;
		i++;
	}
}

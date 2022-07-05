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
static STK S=NULL;//存储标签处理情况的栈 
static LNW List_Web=NULL;//存储网页信息的栈 
static char s[2048]={0};//存储待处理文本内容 
static int ListNum=0;//有序列表的编号 
static int Width,Height;//总窗口的宽和高 
static int cux,cuy;//画笔所在的位置 
static int BackForm=0;//个性化格式 
static char Website[2048]={0};//存储网址 
static SCROLLINFO Scroll;//滚动条 
void Reverse(char *s);
int WINAPI WinMain_1(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
void Result_Paint(HWND hWnd,int Flag)
{
	RECT Windows;
	GetClientRect(hWnd, &Windows);//获取当前窗口的大小 
	Width=Windows.right-Windows.left;
	Height=Windows.bottom-Windows.top;
	cux=Width*5/100;
	cuy=0;
	HFONT hFont=CreateFont(
            -20/*高度*/, -10/*宽度*/, 0, 0, 400 /*一般这个值设为400*/,
            FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
            DEFAULT_CHARSET,  //使用默认字符集
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  //默认输出质量
            FF_DONTCARE,  //不指定字体族*/
            TEXT("微软雅黑")  //字体名
        );
	HDC hdc;
	PAINTSTRUCT ps;
	RECT Text;
	hdc = BeginPaint(hWnd, &ps);
	SelectObject(hdc,hFont);
	SetBkMode(hdc,  //设备环境句柄
    TRANSPARENT  //背景模式
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
	//处理和渲染 
	if (p->token->type==-1)//未定义的标签 
	{
		if(p->token->endflag==1)
		{
			memset(s,0,2048);
		} 
		return;
	}
	else if(p->token->type==0)//处理文本 
	{
		strcat(s,p->token->Text);
		strcat(s," ");
		return; 
	}
	else//处理当前标签 
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
	
	//建立一个栈存储标签情况 
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


/*这是执行最终输出的函数*/
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
		
		//定义超文本链接.
		case HTML_A:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 400,
            FALSE, TRUE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("微软雅黑")  
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
		
		//定义段落.
		case HTML_P:
			if(s[0]==0)break;
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			break;
		
		//定义文本粗体.
		case HTML_B:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 700,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("微软雅黑")  
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
		
		//定义标题字体.
		case HTML_TITLE:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18*2, -9*2, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY, 
            FF_DONTCARE,  
            TEXT("微软雅黑") 
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
		
		//定义列表的主题项目
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
				strcat(Listnum,"·");
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
            TEXT("微软雅黑")  
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
		
		//定义文本强调，文字加红
		case HTML_STRONG:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("微软雅黑")  
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
		
		//定义缩写，光标移动到上面的时候出现全程
		case HTML_ABBR:

			break;
		
		//定义简单的折行，实际上就是回车
		case HTML_BR:
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			break;
		
		//定义表格的标题(要不就按照普通标题输出吧)
		case HTML_CAPTION:break;
			
		//定义语气更加强调的内容，文字加粗加红
		case HTML_EM:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 700 ,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("微软雅黑")  
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
		
		/*以下内容定义html标题级别，级别越小标题越大*/
		case HTML_H1:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18*4, -9*4, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("微软雅黑") 
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
            TEXT("微软雅黑") 
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
            TEXT("微软雅黑") 
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
            TEXT("微软雅黑") 
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
            TEXT("微软雅黑") 
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
            TEXT("微软雅黑")        	);
        	hFont_default=SelectObject(hdc,hfont); 
			GetTextMetricsW(hdc,&NowT);
			SetRect(&text,cux,cuy,cux+Width*9/10,cuy+NowT.tmHeight);
			DrawText(hdc,s,strlen(s),&text,DT_CALCRECT |DT_LEFT |DT_WORDBREAK);
			DrawText(hdc,s,strlen(s),&text,DT_LEFT |DT_WORDBREAK);
			cuy+=text.bottom-text.top;
			SelectObject(hdc,hFont_default);
			DeleteObject(hfont);
			break;
		
		//在当前有内容输出的最后一行的下一行画一条分割线
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
		
		//定义大号字体
		case HTML_BIG:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18*2, -9*2, 0, 0, 400 ,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("微软雅黑") 
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
		
		//斜体
		case HTML_I:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 400 ,
            TRUE, FALSE, FALSE,
            DEFAULT_CHARSET, 
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY, 
            FF_DONTCARE,  
            TEXT("微软雅黑")  
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
		
		//有序列表，即在每行前加上标号
		case HTML_OL:
		if(p->token->type==HTML_OL&&p->token->endflag==1)
			ListNum=0;
		break;
			
		//无序列表，每行前无需加上标号
		case HTML_UL:break;
		
		//标有记号的文本（文本所在区域用彩笔涂上，像做笔记那样）
		case HTML_MARK:
			if(s[0]==0)break;
			hfont = CreateFont(
            -18, -9, 0, 0, 400 ,
            TRUE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("微软雅黑") 
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
		
		//引用的文本，以特殊字体显示
		case HTML_CITE:break;
		
		//开始制表
		case HTML_TABLE:
			if(p->token->type==HTML_TH||p->token->type==HTML_TD)
			break;
			onex=cux;
			oney=cuy;
		break;
		
		//定义表格中的一行
		case HTML_TR:
			if(p->token->type==HTML_TR&&p->token->endflag==1)
			{
				GetTextMetricsW(hdc,&NowT);
				cuy+=NowT.tmHeight;
			}
			
			break;
		
		//定义表头
		case HTML_TH:
			GetTextMetricsW(hdc,&NowT);
			strcat(s," | ");
			TextOut(hdc,onex,oney,s,strlen(s));
			onex+=strlen(s)*NowT.tmAveCharWidth;
			break;
		
		//定义内容单元格
		case HTML_TD:
			GetTextMetricsW(hdc,&NowT);
			strcat(s," | ");
			TextOut(hdc,onex,oney,s,strlen(s));
			onex+=strlen(s)*NowT.tmAveCharWidth;
			break;
		
		//定义单元格中的表头
		case HTML_THEAD:break;
		
		//定义单元格中间部分
		case HTML_TBODY:break;
		
		//定义单元格尾部
		case HTML_TFOOT:break;
		
		//在文本下加上下划线
		case HTML_INS:
			hfont = CreateFont(
            -18, -9, 0, 0, 400 ,
            FALSE, TRUE, FALSE,
            DEFAULT_CHARSET,
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,
            FF_DONTCARE,  
            TEXT("微软雅黑")
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
		
		//在文本间加删除线
		case HTML_DEL:
			hfont = CreateFont(
            -18*2, -9*2, 0, 0, 400 ,
            FALSE, FALSE, TRUE,
            DEFAULT_CHARSET, 
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY, 
            FF_DONTCARE,  
            TEXT("微软雅黑") 
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
		MessageBox(NULL, "窗口注册失败","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","网页浏览器",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		GetSystemMetrics(SM_CXSCREEN), 
		GetSystemMetrics(SM_CYSCREEN), 
		NULL,NULL,hInstance,NULL);
	if(hWnd == NULL) {
		MessageBox(NULL, "窗口创建失败!","Error!",MB_ICONEXCLAMATION|MB_OK);
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
	int i,j,k;//循环使用变量
    int wmId, wmEvent,EnterFlag=0;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT Windows,ResultPiant,Text;
    TCHAR HTMLEditText[]="HTML代码显示栏";
    TCHAR ResultPaint[]="结果显示栏";

	GetClientRect(Hwnd, &Windows);//获取当前窗口的大小 
	Width=Windows.right-Windows.left;
	Height=Windows.bottom-Windows.top;
    static HFONT hFont,HFontSearch;
    static HWND ButtonLeft,ButtonRight,ButtonEnter,ButtonRefresh,
		ButtonPerson,ButtonNew,ButtonAbout,ButtonHelp;//按钮 
    static HWND HTMLEdit,SearchEdit;//可编辑文本框 
    static HWND hScroll;//滚动条 
    static HWND Situation,BackGround,Result;//状态栏 
    static HPEN hpen,hpen_Default;//画笔
 	static HBRUSH hbrush,hbrush_Default;//画刷   
    static int cxSize, cySize,yPos;
    static int iScrollWidth;
    static HMENU Hmenu,Hmenu_1;//菜单 
    FILE *File,*HTMLFile;
    
    COLORREF Color_Ref[7]={
	RGB(255,0,0),
	RGB(255,125,0),
	RGB(255,255,0),
	RGB(0,255,0),
	RGB(0,0,255),
	RGB(0,255,255),
	RGB(255,0,255)
	};//颜色存储数组 
    
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
        
        //创建逻辑字体
        hFont = CreateFont(
            -18, -9, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("微软雅黑")
        );
        
        HFontSearch = CreateFont(
            -18*3/2, -9*3/2, 0, 0, 400,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,  
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
            DEFAULT_QUALITY,  
            FF_DONTCARE,  
            TEXT("宋体")  
	        );
        SelectObject(hdc,hFont);
        SearchEdit = CreateWindow(
            TEXT("edit"),  //静态文本框的类名
            TEXT("Http://"),  //控件的文本
            WS_CHILD /*子窗口*/ | WS_VISIBLE /*创建时显示*/ | WS_BORDER /*带边框*/|ES_AUTOHSCROLL,
            Width*3/10 /*X坐标*/, Height*70/1000 /*Y坐标*/, Width*4/10 /*宽度*/, Height*50/1000/*高度*/,
			Hwnd/*父窗口句柄*/,
            (HMENU)EDITBOX_SEARCH,  //为控件指定一个唯一标识符
            hInst,  //当前实例句柄
            NULL
        );
        //创建HTML文本框 
        HTMLEdit = CreateWindow(
            TEXT("edit"),  
            TEXT(""),  
            WS_CHILD| WS_VISIBLE| WS_BORDER |ES_MULTILINE,
            Width/20 , Height/4, Width*2/5, Height*6/10, Hwnd,
            (HMENU)EDITBOX_HTML,  
            hInst,  
            NULL
        );
        //状态栏 
        Situation = CreateWindow(
            TEXT("static"),  
            TEXT("浏览器打开"), 
            WS_CHILD | WS_VISIBLE | WS_BORDER ,
            Width/20 , Height*88/100, Width*1/5, Height*4/100, Hwnd,
            (HMENU)STATIC_SITUATION,  
            hInst,  
            NULL
        );
        //创建按钮控件
        //后退按钮 
        ButtonLeft = CreateWindow(
            TEXT("button"),  //按钮控件的类名
            TEXT("后退"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 
            Width*5/100 , Height*5/100 , Width*8/100 , Height*75/1000,
            Hwnd, (HMENU)BT_LEFT , hInst, NULL
        );
		//前进按钮 
        ButtonRight = CreateWindow(
            TEXT("button"),  
            TEXT("前进"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON,
            Width*18/100 , Height*5/100 , Width*8/100, Height*75/1000,
            Hwnd, (HMENU)BT_RIGHT , hInst, NULL
        );
        //进入 
        ButtonEnter = CreateWindow(
            TEXT("button"),  
            TEXT("进入"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 
            Width*7/10 , Height*7/100 , Width*4/100 , Height*5/100,
            Hwnd, (HMENU)BT_ENTER , hInst, NULL
        );
        //刷新按钮 
        ButtonRefresh = CreateWindow(
            TEXT("button"),  
            TEXT("刷新"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 
            Width*75/100 , Height*6/100 , Width*7/100 , Height*60/1000,
            Hwnd, (HMENU)BT_REFRESH , hInst, NULL
        );
        //个性化按钮 
        ButtonPerson = CreateWindow(
            TEXT("button"),  
            TEXT("个性化"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 
            Width*83/100 , Height*6/100 , Width*7/100 , Height*6/100,
            Hwnd, (HMENU)BT_PERSON , hInst, NULL
        );
        
        ButtonNew = CreateWindow(
            TEXT("button"),  
            TEXT("新窗口"),
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
        //结果显示位置
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
        AppendMenu(Hmenu_1,MF_STRING,BT_NEW,TEXT("新建（Ctrl+N）"));
		AppendMenu(Hmenu_1,MF_STRING,BT_RIGHT,TEXT("前进（Ctrl+R）"));
		AppendMenu(Hmenu_1,MF_STRING,BT_LEFT,TEXT("后退（Ctrl+L）"));
		AppendMenu(Hmenu_1,MF_STRING,BT_REFRESH,TEXT("刷新（Ctrl+F）"));
		AppendMenu(Hmenu_1,MF_STRING,BT_PERSON,TEXT("个性化（Ctrl+P）"));
        AppendMenu(Hmenu,MF_POPUP,(unsigned int)Hmenu_1, TEXT("菜单"));
        AppendMenu(Hmenu, MF_STRING,BT_HELP, TEXT("帮助（Ctrl+H）"));
        AppendMenu(Hmenu, MF_STRING,BT_ABOUT, TEXT("有关（Ctrl+A）"));
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
  		SendMessage(SearchEdit, WM_SETFONT, (WPARAM)HFontSearch, 0);//设置按钮字体 
  		DispalyHTML(HTMLEdit);
  		
        break;
    case WM_COMMAND://命令 
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        switch (wmId) {
        case BT_LEFT:  //按下按钮
            if(List_Web==NULL)
        	{
        		SetWindowText(Situation,TEXT("没有浏览过的网页")); 
        	}
        	else if(List_Web->past==NULL)
        	{
        		SetWindowText(Situation,TEXT("前面没有路了")); 
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
				SetWindowText(Situation,TEXT("连接中..."));
				NetConnect(Website);
			}
        	else if(Website[0]!=0)
        	{
        		SetWindowText(Situation,TEXT("文件打开中..."));
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
        	SetWindowText(Situation,TEXT("解析完成"));
        	}
            break;
        case BT_RIGHT:
        	if(List_Web==NULL)
        	{
        		SetWindowText(Situation,TEXT("没有浏览过的网页")); 
        	}
        	else if(List_Web->next==NULL)
        	{
        		SetWindowText(Situation,TEXT("后面没有路了")); 
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
				SetWindowText(Situation,TEXT("连接中..."));
				NetConnect(Website);
			}
        	else if(Website[0]!=0)
        	{
        		SetWindowText(Situation,TEXT("文件打开中..."));
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
        	SetWindowText(Situation,TEXT("解析完成"));
        	}
            break;
        case BT_ENTER:
        	GetWindowText(SearchEdit,Website,2048);
        	
        	if(Website[0]!=0&&Website[0]=='H'&&Website[1]!=0&&Website[1]=='t'
			&&Website[2]!=0&&Website[2]=='t'&&Website[3]!=0&&Website[3]=='p'
			&&Website[4]!=0&&Website[4]==':'&&Website[5]!=0&&Website[5]=='/')
			{
				SetWindowText(Situation,TEXT("连接中..."));
				NetConnect(Website);
				//MessageBox(Hwnd,Website,TEXT("网页名"),MB_OK);
			}
        	else if(Website[0]!=0&&Website[0]!='H')
        	{
        		SetWindowText(Situation,TEXT("文件打开中..."));
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
        	SetWindowText(Situation,TEXT("解析完成"));
        	break; 
        case BT_REFRESH:
        	SendMessage(Hwnd,WM_PAINT,wParam,lParam);
        	DispalyHTML(HTMLEdit);
        	UpdateWindow(Result);
        	SetWindowText(Situation,TEXT("刷新完成"));
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
			MessageBox(Hwnd,TEXT("本程序创建一个简单网络浏览器，实现网页的前进、后退、\n进入主页、刷新、跳转等功能。\n当前版本为2.1.1，\n版本创建日期：1/6/2022"),TEXT("有关"),MB_OK); 
		break;
		case BT_HELP:
			MessageBox(Hwnd,TEXT("1、前进：点击后，网址框内显示下一个网址，若下一个网址为空，则仍显示原网页的内容，状态栏显示“前面没有路”。\n2、后退：点击后，网址框内显示上一个网址，若上一个网址为空，则仍显示原网页的内容，状态栏显示“后面没有路”。\n3、进入（跳转）：在网址框内输入格式（http://)正确的网址并点击后，语言框和显示框中显示该网页的内容。\n4、刷新：点击后对整个图形页面进行刷新，若网址框显示有改动但没有点击进入，则刷新后显示改动前内容。\n5、主页：点击后显示主页。\n6、个性化：点击后切换界面主色调。\n6、说明：点击后，会在页面中心弹出标签，说明上述功能。\n7、有关：点击后，会在页面中心弹出标签，显示有关信息。"),TEXT("帮助"),MB_OK); 
        default:
            //不处理的消息交给 DefWindowProc 处理。
            return DefWindowProc(Hwnd, message, wParam, lParam);
        }
        break;
        
    case WM_VSCROLL://滚轮消息处理 
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
    case WM_KEYDOWN://快捷键的设置 
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
		SetBkMode(hdc,  //设备环境句柄
    	TRANSPARENT  //背景模式
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
	//输入 
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

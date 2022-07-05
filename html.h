#ifndef _HTML_H_
#define _HTML_H_

#define HTML_TEXT 0     //text类型.
#define HTML_DOCTYPE 1 //开始标志.
#define HTML_BODY 2
#define HTML_A 3        //定义超文本链接.
#define HTML_P 4       //定义段落p.在前后各加一个换行符号.
#define HTML_B 5         //定义文本粗体.
#define HTML_HTML 6       //定义html文档.
#define HTML_META 7    //定义关于HTML文档的元信息.
#define HTML_TITLE 8   //定义标题.
#define HTML_SCRIPT 9  //定义客户端脚本.
#define HTML_LIST 10   //定义列表的主题项目.即这一行要输出什么.(缩进,加回车即可)
#define HTML_SMALL 11  //显示小号字符.
#define HTML_STRONG 12  //定义文本强调,按文本粗体显示.
#define HTML_SPAN 13   //定义文档中的节. https://www.w3school.com.cn/tags/tag_span.asp
#define HTML_ABBR 14   //定义缩写.光标移动到上面的时候出现全称.
#define HTML_BR 15     //定义简单的折行.实际上就是回车.
#define HTML_CAPTION 16 //表格的标题,但是按照普通标题输出.
#define HTML_EM 17     //定义为语气更强的强调内容(红色输出?).
#define HTML_CODE 18    //计算机源代码文本.先按文本处理.
#define HTML_DIV 19    //定义文档中的块级元素,按折行处理.



#define HTML_H1 20
#define HTML_H2 21
#define HTML_H3 22
#define HTML_H4 23
#define HTML_H5 24
#define HTML_H6 25

#define HTML_HR 26     
#define HTML_BIG 27    //定义大号字体.
#define HTML_I 28      //斜体.先按照普通文本处理.
#define HTML_TT 29      //字符样式定义.先按普通文本处理.
#define HTML_OL 30     //定义有序列表,在li元素前加上标号.
#define HTML_UL 31     //定义无序列表,直接输出li即可.
#define HTML_MARK 32   //定义标有记号的文本.(用彩笔标出文本,像做笔记那样).
#define HTML_NAV 33    //定义导航链接的标签.解释时直接无视.
#define HTML_VAR 34     //一种格式的斜体字.先按照普通文本输出.
#define HTML_CITE 35   //引用文本,以特殊字体展示文本.
#define HTML_WBR 36    //解释时直接无视.
#define HTML_TABLE 37  
#define HTML_TR 38     
#define HTML_TH 39     
#define HTML_TD 40     
#define HTML_THEAD 41  //定义单元格中的表头.
#define HTML_TBODY 42  //定义单元格中间部分.
#define HTML_TFOOT 43  //定义单元格尾部. 
#define HTML_INS 44     //在文本下加下划线.
#define HTML_DEL 45     //在文本间加删除线.
typedef struct TokenAttribute TokenAttr;
typedef struct TokenAttribute *TKA;
typedef struct Token Newtoken;
typedef struct Token *NK;
typedef struct TokenList Newtokenlist;
typedef struct TokenList *NTK;

NTK DomTree(FILE *fp);
NK GetToken(char *s);
NK GetText(char *s);
int GetType(char *s);
TKA GetAttr(char *s);
void FreeDomTree(NTK Header);
void PrintAttr(TKA Attr);
void Print(NTK T);
struct TokenAttribute{
	char *Name;
	char *Tail;
	struct TokenAttribute *next;
};

struct Token{
	int type;
	char *Text;
	TKA Attr;
	int endflag;
};

struct TokenList{
	NK token;
	struct TokenList* next;
	struct TokenList* past;
};
#endif


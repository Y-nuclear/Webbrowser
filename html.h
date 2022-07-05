#ifndef _HTML_H_
#define _HTML_H_

#define HTML_TEXT 0     //text����.
#define HTML_DOCTYPE 1 //��ʼ��־.
#define HTML_BODY 2
#define HTML_A 3        //���峬�ı�����.
#define HTML_P 4       //�������p.��ǰ�����һ�����з���.
#define HTML_B 5         //�����ı�����.
#define HTML_HTML 6       //����html�ĵ�.
#define HTML_META 7    //�������HTML�ĵ���Ԫ��Ϣ.
#define HTML_TITLE 8   //�������.
#define HTML_SCRIPT 9  //����ͻ��˽ű�.
#define HTML_LIST 10   //�����б��������Ŀ.����һ��Ҫ���ʲô.(����,�ӻس�����)
#define HTML_SMALL 11  //��ʾС���ַ�.
#define HTML_STRONG 12  //�����ı�ǿ��,���ı�������ʾ.
#define HTML_SPAN 13   //�����ĵ��еĽ�. https://www.w3school.com.cn/tags/tag_span.asp
#define HTML_ABBR 14   //������д.����ƶ��������ʱ�����ȫ��.
#define HTML_BR 15     //����򵥵�����.ʵ���Ͼ��ǻس�.
#define HTML_CAPTION 16 //���ı���,���ǰ�����ͨ�������.
#define HTML_EM 17     //����Ϊ������ǿ��ǿ������(��ɫ���?).
#define HTML_CODE 18    //�����Դ�����ı�.�Ȱ��ı�����.
#define HTML_DIV 19    //�����ĵ��еĿ鼶Ԫ��,�����д���.



#define HTML_H1 20
#define HTML_H2 21
#define HTML_H3 22
#define HTML_H4 23
#define HTML_H5 24
#define HTML_H6 25

#define HTML_HR 26     
#define HTML_BIG 27    //����������.
#define HTML_I 28      //б��.�Ȱ�����ͨ�ı�����.
#define HTML_TT 29      //�ַ���ʽ����.�Ȱ���ͨ�ı�����.
#define HTML_OL 30     //���������б�,��liԪ��ǰ���ϱ��.
#define HTML_UL 31     //���������б�,ֱ�����li����.
#define HTML_MARK 32   //������мǺŵ��ı�.(�òʱʱ���ı�,�����ʼ�����).
#define HTML_NAV 33    //���嵼�����ӵı�ǩ.����ʱֱ������.
#define HTML_VAR 34     //һ�ָ�ʽ��б����.�Ȱ�����ͨ�ı����.
#define HTML_CITE 35   //�����ı�,����������չʾ�ı�.
#define HTML_WBR 36    //����ʱֱ������.
#define HTML_TABLE 37  
#define HTML_TR 38     
#define HTML_TH 39     
#define HTML_TD 40     
#define HTML_THEAD 41  //���嵥Ԫ���еı�ͷ.
#define HTML_TBODY 42  //���嵥Ԫ���м䲿��.
#define HTML_TFOOT 43  //���嵥Ԫ��β��. 
#define HTML_INS 44     //���ı��¼��»���.
#define HTML_DEL 45     //���ı����ɾ����.
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


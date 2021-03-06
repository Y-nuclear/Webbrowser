#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <time.h>
#pragma comment(lib,"ws2_32.lib")

#define SERVER_PORT 80                //服务器端口号 
#define BUF_SIZE 2048
#define DOMAIN_SIZE 100
#define IP_SIZE 100
#include "stackw.h"
int NetConnect(char *Website) {
	int i=0,j=0;;
	while(Website[i]!=0&&Website[i]!='w')
	{
		i++;
	}
    int rval = 0;
    char sendbuf[BUF_SIZE];
    char revbuf[BUF_SIZE];
    char nameUrl[100]={0},*s;
    //解析IP地址 
    while(Website[i]!=0)
    {
    	nameUrl[j++]=Website[i++];
    }
    nameUrl[j]=0;
	s=DNSServer(nameUrl);
    WORD wVersionrequested;
    WSADATA wsaData;
    SOCKET ClientSock;
    struct sockaddr_in ServerAddr;

    wVersionrequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionrequested, &wsaData) != 0) {
        system("pause");
        exit(1);
    }
    ClientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ClientSock == INVALID_SOCKET) {
        system("pause");
        exit(1);
    }

    /* 配置服务器IP、端口信息 */
    memset(&ServerAddr, 0, sizeof(struct sockaddr));    //每一个字节都用0来填充 
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(SERVER_PORT);
    ServerAddr.sin_addr.s_addr = inet_addr(s);

    /* 向服务器发起请求建立连接 */
    rval = connect(ClientSock, (SOCKADDR*)& ServerAddr, sizeof(struct sockaddr));
    if (rval == SOCKET_ERROR) {
        system("pause");
        exit(1);
    }

    strcpy(sendbuf,"GET / HTTP/1.1\n\n");
	rval = send(ClientSock, sendbuf, strlen(sendbuf),0);
    FILE *fp=fopen("output.txt","w+");
    while(1){
    char recvbuf[1025] = {0};
    rval = recv(ClientSock, recvbuf, sizeof(recvbuf)-1, 0);
	if (rval <= 0) {
            closesocket(ClientSock);     //关闭套接字 
    		WSACleanup(); //停止Winsock 
            break;
        }
    fprintf(fp,"%s",recvbuf);
    }
	fclose(fp);
    closesocket(ClientSock);     //关闭套接字 
    WSACleanup(); //停止Winsock 

	fp=fopen("output.txt","r");
	FILE *fp2=fopen("HTML.html","w");
	char h[100]={0};
	i=0;
	while(fgets(h,100,fp)!=NULL)
	{
		if(h[0]=='<')break;
	}
	do
	{
		fputs(h,fp2);
		
	}while(fgets(h,100,fp)!=NULL);
	fclose(fp);
	fclose(fp2);
    return 0;
}
//DNS域名解析
 char *DNSServer(char *Domain)
{
	char name[DOMAIN_SIZE+10]={"ping "};
	strcat(name,Domain);
	FILE *fp=popen(name,"r");
	char *s=(char *)malloc(sizeof(char)*IP_SIZE),c;
    int i=0;
    while((c=fgetc(fp))!=EOF)
    {
    	if(c=='[')
    	{
    		while((c=fgetc(fp))!=']')
    		{
    			s[i++]=c;
			}
			s[i]=0;
			break;
		}
	}
	return s;
}

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <time.h>
#pragma comment(lib,"ws2_32.lib")

#define SERVER_PORT 80                //�������˿ں� 
#define BUF_SIZE 1024
#define DOMAIN_SIZE 100
#define IP_SIZE 100
char *DNSServer(char *Domain);


int main(int argc, char* argv[]) {
    int rval = 0;
    char sendbuf[BUF_SIZE];
    char revbuf[BUF_SIZE];
    char nameUrl[100]="www.bilibili.com",*s;
    //����IP��ַ
	s=DNSServer(nameUrl);
    WORD wVersionrequested;
    WSADATA wsaData;
    SOCKET ClientSock;
    struct sockaddr_in ServerAddr;
    printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * \n");
    /* ����Winsock */
    wVersionrequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionrequested, &wsaData) != 0) {
        printf("Failed to load Winsock!\n");
        system("pause");
        return -1;
    }
    printf("Succeed to load Winsock!\n");

    /* �����׽��� */
    ClientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ClientSock == INVALID_SOCKET) {
        printf("Failed to create socket!\n");
        system("pause");
        exit(1);
    }
    printf("Succeed to create socket!\n");

    /* ���÷�����IP���˿���Ϣ */
    memset(&ServerAddr, 0, sizeof(struct sockaddr));    //ÿһ���ֽڶ���0����� 
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(SERVER_PORT);
    ServerAddr.sin_addr.s_addr = inet_addr(s);

    /* ��������������������� */
    rval = connect(ClientSock, (SOCKADDR*)& ServerAddr, sizeof(struct sockaddr));
    if (rval == SOCKET_ERROR) {
        printf("Failed to create connection with server!\n");
        system("pause");
        exit(1);
    }
    printf("Succeed to create connection with server!\n");

    printf("\nNow you can have a task!\n");
    printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * \n\n");
    strcpy(sendbuf,"GET / HTTP/1.1\n\n");
	rval = send(ClientSock, sendbuf, strlen(sendbuf),0);
    FILE *fp=fopen("output.txt","w+");
    while(1){
    char recvbuf[1025] = {0};
    rval = recv(ClientSock, recvbuf, sizeof(recvbuf)-1, 0);
	if (rval <= 0) {
            closesocket(ClientSock);     //�ر��׽��� 
    		WSACleanup(); //ֹͣWinsock 
            break;
        }
    fprintf(fp,"%s",recvbuf);
    printf("%s",recvbuf);
    }
	fclose(fp);
    closesocket(ClientSock);     //�ر��׽��� 
    WSACleanup(); //ֹͣWinsock 

	fp=fopen("output.txt","r");
	FILE *fp2=fopen("HTML.html","w");
	char h[100]={0};
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
//DNS��������
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

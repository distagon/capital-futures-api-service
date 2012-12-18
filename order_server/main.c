#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include "orderapi.h"

#ifdef DEBUG
#define _D(x)	printf x
#else
#define _D(x)
#endif

#define	_L(x)	printf x



extern char  RunCommand(int __cs); //declare on command.c

extern char LoginId[32];	//export to command.c
extern char Password[128];	//export to command.c
extern SOCKET ClientSocket;	//export to command.c
char LoginId[32];
char Password[128];
SOCKET ClientSocket=-1;

static int __defaultport=1677;

int main(int argc,char** argv);
int main(int argc,char** argv)
{


	if(argc < 3)
	{
		printf("usage: order_server LoginID Password [port_number]\n"
				"\tHi,you have not assign Capital Future LoginID and Password.\n");
		return 0;
	}

	_L(("info:\tOrder server start\n"));
	strcpy(LoginId, argv[1]);
	strcpy(Password,argv[2]);
	if(argc > 3)
	{
		__defaultport=atoi(argv[3]);
	}

	_L(("info:\tLogin ID %s\n",LoginId));
	_L(("info:\tPassword %s\n",Password));
	_L(("info:\tconnect port %d\n",__defaultport));

	//init Windows socket
	_L(("info:\tcreate network interface\n"));
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		_D(("WSAStartup() failed\n"));
		return 0;
	}


	SOCKET  __ls; //listen socket
	__ls=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (__ls==INVALID_SOCKET)
	{
		_D(("socket() Failed\n"));
		WSACleanup();
		return 0;
	}

	struct sockaddr_in sin;
	sin.sin_family=AF_INET;
	sin.sin_port=htons(__defaultport);
	sin.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	if (bind(__ls,(struct sockaddr*)&sin,sizeof(sin))==SOCKET_ERROR)
	{
		_D(("bind() Failed\n"));
		WSACleanup();
		return 0;
	}

	if (listen(__ls,1)==SOCKET_ERROR)
	{
		_D(("listen() Failed\n"));
		WSACleanup();
		return 0;
	}


	_L(("info:\twait connect\n"));
	ClientSocket=accept(__ls,NULL,NULL);
	if (ClientSocket==INVALID_SOCKET)
	{
		_D(("accept() fail\n"));
		closesocket(__ls);
		WSACleanup();
		return 0;
	}

	_L(("info:\tconnect create.ready to read command\n"));
	while(RunCommand(ClientSocket) == 1)
	{
		_D(("Finish one command\n"));
	}

	closesocket(ClientSocket);
	closesocket(__ls);
	WSACleanup();
	_L(("info:\tTerminate\n"));
	return 0;
}


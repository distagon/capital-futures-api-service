#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include "orderapi.h"

static int __defaultport=1677;

extern char  RunCommand(int __cs); //declare on command.c

int main(int argc,char** argv);
int main(int argc,char** argv) {


    if(argc == 1) {
        printf("usage: order_server [port_number]\n"
        "\t port_number is server bind port,client will connect with this port"
        "\n default port is 1677\n");
    }
    else __defaultport=atoi(argv[1]);



    //init Windows socket
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup() failed\n");
        return 0;
    }


    SOCKET  __ls; //listen socket
    __ls=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (__ls==INVALID_SOCKET)
    {
        printf("socket() Failed\n");
        WSACleanup();
        return 0;
    }

    struct sockaddr_in sin;
    sin.sin_family=AF_INET;
    sin.sin_port=htons(__defaultport);
    sin.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    if (bind(__ls,(struct sockaddr*)&sin,sizeof(sin))==SOCKET_ERROR)
    {
        printf("bind() Failed\n");
        WSACleanup();
        return 0;
    }

    if (listen(__ls,1)==SOCKET_ERROR)
    {
        printf("listen() Failed\n");
        WSACleanup();
        return 0;
    }

    SOCKET __cs; //Client Socket
    __cs=accept(__ls,NULL,NULL);
    if (__cs==INVALID_SOCKET) {
        printf("accept() fail\n");
        closesocket(__ls);
        WSACleanup();
        return 0;
    }

    while(RunCommand(__cs) == 1) {
        printf("Finish one command\n");
    }

    closesocket(__cs);
    closesocket(__ls);
    WSACleanup();
    return 0;
}


#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include "orderapi.h"

int main(void);
int main(void) {

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
    sin.sin_port=htons(1677);
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

    char buffer;
    int  _r;
    while(1) {
        _r=recv(__cs,&buffer,1,0);
        if(_r <= 0) break;

        buffer++;
        send(__cs,&buffer,1,0);
    }

    closesocket(__cs);
    closesocket(__ls);
    WSACleanup();
    return 0;
}


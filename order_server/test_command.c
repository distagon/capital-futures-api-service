#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

int main(int argc, char **argv)
{

    WSADATA              wsaData;

    SOCKET               SendingSocket;

    // Server/receiver address

    SOCKADDR_IN          ServerAddr, ThisSenderInfo;

    // Server/receiver port to connect to

    unsigned int         Port = 1677;

    int        RetCode;

    // Be careful with the array bound, provide some checking mechanism...
    char sendbuf[1024];
    int BytesSent, nlen;



    // Initialize Winsock version 2.2
    WSAStartup(MAKEWORD(2,2), &wsaData);
    printf("Client: Winsock DLL status is %s.\n", wsaData.szSystemStatus);



    // Create a new socket to make a client connection.
    // AF_INET = 2, The Internet Protocol version 4 (IPv4) address family, TCP protocol
    SendingSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(SendingSocket == INVALID_SOCKET)
    {
        printf("Client: socket() failed! Error code: %ld\n", WSAGetLastError());
        // Do the clean up
        WSACleanup();
        // Exit with error
        return -1;
    }
    else
        printf("Client: socket() is OK!\n");    



    // Set up a SOCKADDR_IN structure that will be used to connect

    // to a listening server on port 5150. For demonstration

    // purposes, let's assume our server's IP address is 127.0.0.1 or localhost



    // IPv4
    ServerAddr.sin_family = AF_INET;
    // Port no.
    ServerAddr.sin_port = htons(Port);
    // The IP address
    ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");



    // Make a connection to the server with socket SendingSocket.

    RetCode = connect(SendingSocket, (SOCKADDR *) &ServerAddr, sizeof(ServerAddr));

    if(RetCode != 0)
    {
        printf("Client: connect() failed! Error code: %ld\n", WSAGetLastError());
        // Close the socket
        closesocket(SendingSocket);
        // Do the clean up
        WSACleanup();
        // Exit with error
        return -1;
    }
    else
    {
        printf("Client: connect() is OK, got connected...\n");
        printf("Client: Ready for sending and/or receiving data...\n");
    }



    // At this point you can start sending or receiving data on

    // the socket SendingSocket.



    // Some info on the receiver side...

    getsockname(SendingSocket, (SOCKADDR *)&ServerAddr, (int *)sizeof(ServerAddr));

    printf("Client: Receiver IP(s) used: %s\n", inet_ntoa(ServerAddr.sin_addr));

    printf("Client: Receiver port used: %d\n", htons(ServerAddr.sin_port));



    // Sends some data to server/receiver...
    sendbuf[0]=2;
    sendbuf[1]=3;
    BytesSent = send(SendingSocket, sendbuf,2, 0);
    if(BytesSent == SOCKET_ERROR)
        printf("Client: send() error %ld.\n", WSAGetLastError());
    else
    {
        printf("Client: send() is OK - bytes sent: %ld\n", BytesSent);
    }



    if( shutdown(SendingSocket, SD_SEND) != 0)
        printf("Client: Well, there is something wrong with the shutdown(). The error code: %ld\n", WSAGetLastError());
    else
        printf("Client: shutdown() looks OK...\n");


    // When you are finished sending and receiving data on socket SendingSocket,

    // you should close the socket using the closesocket API. We will

    // describe socket closure later in the chapter.

    if(closesocket(SendingSocket) != 0)
        printf("Client: Cannot close \"SendingSocket\" socket. Error code: %ld\n", WSAGetLastError());
    else
        printf("Client: Closing \"SendingSocket\" socket...\n");


    // When your application is finished handling the connection, call WSACleanup.
    if(WSACleanup() != 0)
        printf("Client: WSACleanup() failed!...\n");
    else
        printf("Client: WSACleanup() is OK...\n");
    return 0;

}


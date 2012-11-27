#include <windows.h> 
#include <winsock2.h> 
#include <stdio.h>
#include "quoteapi.h"

const char      g_szClassName[] = "quoteview";
unsigned int    _Tid;
HANDLE          __ref_get_data_thread = NULL;
int             __last_trade_id = 0;
short int       __market,__stock;
int             __client_s=-1;
#define WM_SOCKET WM_USER+101


DWORD WINAPI __get_tick_thread(LPVOID lpArg);
DWORD WINAPI __get_tick_thread(LPVOID lpArg) {
    TTick   data;
    int __local_nptr = 0;
    char buffer[16];
    
    while(1) {
        while(__local_nptr < __last_trade_id) {
            QL_GetTick(__market,__stock,__local_nptr,&data);
            //printf("Get data. tick price is %d\n",data.m_nClose);
            sprintf(buffer,"%d\n",data.m_nClose);
            send(__client_s,buffer,strlen(buffer),0);
            __local_nptr++;
        }
        SuspendThread(__ref_get_data_thread);
    }
    return 0;
}

void CALLBACK TimerProc(HWND hwnd, UINT message, UINT timerID, DWORD time) ;
void CALLBACK TimerProc(HWND hwnd, UINT message, UINT timerID, DWORD time) {
    QL_Request("""");//this line will compile fail. that is tip
    KillTimer(NULL,_Tid);
}


void    __stdcall ConnectN ( int nKind, int nCode );
void    __stdcall ConnectN ( int nKind, int nCode ) {
    printf("Connect callback notify. nKind = %d , nCode = %d\n",nKind,nCode);
    _Tid = SetTimer(NULL,0,1000,TimerProc);
    __ref_get_data_thread = CreateThread( NULL, 0,__get_tick_thread, NULL,CREATE_SUSPENDED, NULL);
}


void    __stdcall TickN    ( short sMarketNo, short sStockidx, int nPtr);
void    __stdcall TickN    ( short sMarketNo, short sStockidx, int nPtr){
    printf("Tick callback notify. sMarketNo = %d,sStockidx = %d,nPtr = %d\n",sMarketNo,sStockidx,nPtr);
    __last_trade_id = nPtr;
    __market    = sMarketNo;
    __stock     = sStockidx;
    ResumeThread(__ref_get_data_thread);
}


void SocketHaveData(HWND hwnd,int _socket,LPARAM _l);
void SocketHaveData(HWND hwnd,int _socket,LPARAM _l) {

    SOCKET _sn;
    char buffer[256];
    int bi;


    if (WSAGETSELECTERROR(_l)) 
    { 
        closesocket(_socket);
        return;
    } 

    switch (WSAGETSELECTEVENT(_l)) 
    { 
        case FD_ACCEPT:
            _sn=accept(_socket,NULL,NULL);
            if (_socket==INVALID_SOCKET) printf("accept() fail\n");
            if(__client_s == -1) {
                __client_s = _sn;
            }
            WSAAsyncSelect(_sn,hwnd,WM_SOCKET,FD_READ|FD_CLOSE);
            break;

        case FD_READ:
            bi=recv(_socket,buffer,256,0);
            if ( (bi==SOCKET_ERROR&& WSAGetLastError() == WSAECONNRESET) || bi==0 )
            {
                printf("recv() fail\n");
                closesocket(_socket);
            }
            else if (bi>0)
            {
                if(buffer[0] == '1') {
                    __ref_get_data_thread = NULL;
                    QL_LoginServer("","","");
                    QL_AddCallBack((long)ConnectN,(long)TickN);
                    QL_ConnectDataBase();
                }
                else if (buffer[0] == '2') {
                    printf("kill Capital API\n");
                    QL_Bye();
                }
                else if(buffer[0] == '3') {
                    printf("Call DestoryWindow()\n");
                    DestroyWindow(hwnd);
                }
            }
            break;

        case FD_CLOSE:
            closesocket(_socket);
            break; 
    }
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam) 
{ 
    switch(uMsg) 
    { 
        case WM_SOCKET:
            SocketHaveData(hwnd,wParam,lParam);
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default: 
            return DefWindowProc(hwnd,uMsg,wParam,lParam); 
    }
    return 0;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;



    //<Window Class>--------------------------------------------
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        printf("Window Registration Failed! Job aborted\n");
        return 0;
    }
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"quoteview","The title of my window",
            WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,120,40,NULL, NULL, hInstance, NULL);

    if (hwnd==NULL) 
    { 
        printf("Window Creation Failed! Job aborted\n");
        return 0;
    } 
    //</Window Class>--------------------------------------------
    



    //<create server socket>
    WSADATA wsaData;
    WORD wVersionRequested=MAKEWORD(2,2);
    if (WSAStartup(wVersionRequested,&wsaData)!=0)
    { 
        printf("WSAStartup() Failed\n");
        return 0; 
    } 
    SOCKET s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (s==INVALID_SOCKET)
    {
        printf("socket() Failed\n");
        return 0;
    }

    struct sockaddr_in sin; 
    sin.sin_family=AF_INET; 
    sin.sin_port=htons(1111); 
    sin.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    if (bind(s,(struct sockaddr*)&sin,sizeof(sin))==SOCKET_ERROR) 
    { 
        printf("bind() Failed\n");
        return 0;
    } 

    if (listen(s,3)==SOCKET_ERROR)
    { 
        printf("listen() Failed\n");
        return 0;
    } 

    WSAAsyncSelect(s,hwnd,WM_SOCKET,FD_ACCEPT|FD_CLOSE);
    //</create server socket>



    ShowWindow(hwnd,SW_SHOWNORMAL);
    UpdateWindow(hwnd);
    while (GetMessage(&Msg,0,0,0)) 
    { 
        TranslateMessage(&Msg); 
        DispatchMessage(&Msg); 
    } 
    closesocket(s);
    WSACleanup();
    return 1;
}

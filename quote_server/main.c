#include <windows.h> 
#include <winsock2.h> 
#include <stdio.h>

const char      g_szClassName[] = "quoteview";
unsigned int    _Tid;
HANDLE          __ref_get_data_thread = NULL;
int             __last_trade_id = 0;
short int       __market,__stock;
int             __client_s=-1;
int             DefaultPort=3396;
#define WM_SOCKET WM_USER+101

extern int RunCommand(char data); //define in command.c

extern char LoginID[16];
extern char Password[16];
char LoginID[16];
char Password[16];



int __parse_argv(void);
int __parse_argv(void) {
    LPWSTR *Wargv;
    int argc;
    char __dp[16];

    Wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if          ( Wargv == NULL ) {
        printf("CommandLineToArgvW failed\n");
        return 0;
    } else if   (argc < 3) {
        printf("command argement error\n");
        LocalFree(Wargv);
        return 0;
    } else {
        WideCharToMultiByte(437, 0, Wargv[1], -1, LoginID, 16, NULL, NULL);
        printf("Login ID %s\n",LoginID);
        WideCharToMultiByte(437, 0, Wargv[2], -1, Password,16, NULL, NULL);
        printf("Password %s\n",Password);

        if (argc >= 4) {
            WideCharToMultiByte(437, 0, Wargv[3], -1, __dp,16, NULL, NULL);
            DefaultPort = atoi(__dp);
        }
        printf("default port %d\n",DefaultPort);
    }
    LocalFree(Wargv);
    return 1;
}




void SocketHaveData(HWND hwnd,int _socket,LPARAM _l);
void SocketHaveData(HWND hwnd,int _socket,LPARAM _l) {

    char buffer;
    int bi;


    if (WSAGETSELECTERROR(_l)) {
        closesocket(_socket);
        return;
    }

    switch (WSAGETSELECTEVENT(_l))  {

        case FD_ACCEPT:
            if(__client_s == -1) {
                __client_s=accept(_socket,NULL,NULL);
                if (__client_s==INVALID_SOCKET) {
                    printf("accept() fail\n");
                    __client_s = -1;
                } else WSAAsyncSelect(__client_s,hwnd,WM_SOCKET,FD_READ|FD_CLOSE);
            }
            break;



        case FD_READ:
            bi=recv(_socket,&buffer,1,0);
            if ( bi==SOCKET_ERROR || bi==0 )
            {
                printf("recv() fail\n");
                closesocket(_socket);
            } else bi = RunCommand(buffer);
            
            if(bi == 0) {
                closesocket(_socket);
                DestroyWindow(hwnd);
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

    if (__parse_argv() != 1) return 0;

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
    sin.sin_port=htons(DefaultPort);
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



    ShowWindow(hwnd,SW_SHOW);
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

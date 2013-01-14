#include <windows.h> 
#include <winsock2.h> 
#include <stdio.h>

#ifdef DEBUG
#define _D(x)	printf x
#else
#define _D(x)
#endif

#define	_L(x)	printf x

#define WM_SOCKET WM_USER+101

extern void SocketHaveData(HWND hwnd,int _socket,LPARAM _l); //define from command.c
extern char LoginID[16];	//export to command.c
extern char Password[16];	//export to command.c
extern SOCKET ACtlS;		//export to command.c
extern SOCKET ADataS;		//export to command.c
char LoginID[16];
char Password[16];
SOCKET ACtlS = -1;
SOCKET ADataS = -1;

static const char __szClassName[] = "quoteview";
static int __ctl_port=3396;
static int __data_port=3050;

static SOCKET NewSocket(int port);
SOCKET NewSocket(int port)
{
	WSADATA wsaData;
	WORD wVersionRequested=MAKEWORD(2,2);
	SOCKET _s;

	if (WSAStartup(wVersionRequested,&wsaData)!=0)
	{
		_D(("WWSAStartup() Failed! Job aborted\n"));
		return -1;
	}

	_s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (_s==INVALID_SOCKET)
	{
		_D(("socket() Failed! Job aborted\n"));
		return -1;
	}

	struct sockaddr_in sin;
	sin.sin_family=AF_INET;
	sin.sin_port=htons(port);
	sin.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	if (bind(_s,(struct sockaddr*)&sin,sizeof(sin))==SOCKET_ERROR)
	{
		_D(("bind() Failed! Job aborted\n"));
		closesocket(_s);
		return -1;
	}

	if (listen(_s,3)==SOCKET_ERROR)
	{
		_D(("Listen() Failed! Job aborted\n"));
		closesocket(_s);
		return -1;
	}

	return _s;
}

int __parse_argv(void);
int __parse_argv(void)
{
	LPWSTR	*__wargv;
	int	__argc;


	__wargv = CommandLineToArgvW(GetCommandLineW(), &__argc);
	if        (__wargv == NULL)
	{
		_D(("CommandLineToArgvW failed\n"));
		return 0;
	}

	else if   (__argc < 3)
	{
		_L(("info:\tcommand line argement is not assign\n"));
		LocalFree(__wargv);
		return 0;
	}

	else
	{
		WideCharToMultiByte(437, 0, __wargv[1], -1, LoginID, 16, NULL, NULL);
		_L(("info:\tLogin ID %s\n",LoginID));
		WideCharToMultiByte(437, 0, __wargv[2], -1, Password,16, NULL, NULL);
		_L(("info:\tPassword %s\n",Password));
	}

	LocalFree(__wargv);
	return 1;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if        (uMsg == WM_SOCKET)           {SocketHaveData(hwnd,wParam,lParam);return 0;}
	else if   (uMsg == WM_DESTROY)          {_L(("info:\tTerminate\n"));PostQuitMessage(0);return 0;}

	else      return                        DefWindowProc(hwnd,uMsg,wParam,lParam);
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;


	_L(("info:\tQuote server start\n"));

	if (__parse_argv() != 1)
		return 0;

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
	wc.lpszClassName = __szClassName;
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		_D(("RegisterClassEx() fail\n"));
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE
			,"quoteview"
			,"The title of my window"
			,WS_OVERLAPPEDWINDOW
			,CW_USEDEFAULT
			,CW_USEDEFAULT
			,120
			,40
			,NULL
			,NULL
			,hInstance
			,NULL);
	if (hwnd==NULL)
	{
		_D(("Window Creation Failed! Job aborted\n"));
		return 0;
	}
	//</Window Class>--------------------------------------------




	//<create server socket>
	ACtlS = NewSocket(__ctl_port);
	if (ACtlS == -1)
	{
		_D(("Create control socket failed! Job aborted\n"));
		return 0;
	}
	WSAAsyncSelect(ACtlS,hwnd,WM_SOCKET,FD_ACCEPT|FD_CLOSE);
	_L(("info:\tListen %d port(Control)\n",__ctl_port));

	ADataS = NewSocket(__data_port);
	if (ADataS == -1)
	{
		_D(("Create data socket failed! Job aborted\n"));
		return 0;
	}
	WSAAsyncSelect(ADataS,hwnd,WM_SOCKET,FD_ACCEPT|FD_CLOSE);
	_L(("info:\tListen %d port(Data)\n",__data_port));
	//</create server socket>



	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);
	while (GetMessage(&Msg,0,0,0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	closesocket(ACtlS);
	closesocket(ADataS);
	WSACleanup();
	return 1;
}

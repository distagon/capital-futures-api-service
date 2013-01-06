#include <windows.h>
#include <stdio.h>
#include <winsock2.h>
#include "quoteapi.h"

/*command

login
struct Login {
    char command:=2;
}

logout
struct Logout {
    char command:=3;
}

exit
struct Exit {
    char command:=4;
}

watch Commodity name
struct Watch {
    char command:=5;
    char watch[10];
}

pull price
struct Pull {
    char command:=6;
    int  index; -> little_endian
}
*/

/*logic

server(this)                                client
        <-----------------------------------Login
report login result >>>>>>>>>>>>>>>>>>>>>>>>

        <-----------------------------------Watch
report watch result >>>>>>>>>>>>>>>>>>>>>>>>

        <-----------------------------------Pull
report price >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


        <-----------------------------------Pull
report price >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

        <-----------------------------------Pull
report price >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

.
.
.
.
.
.
.
        <-----------------------------------Logout
NULL report

        <-----------------------------------Exit
Server exit
*/


#ifdef DEBUG
#define _D(x)	printf x
#else
#define _D(x)
#endif

#define _L(x)	printf x

#define _START		1
#define _FILLING	2
#define WM_SOCKET WM_USER+101


extern char	LoginID[16]; //define in main.c
extern char	Password[16];//define in main.c


static int _state = _START;
static int _cmdsize = 0;
static int _fillcur = 0;
static char _cmdbuf[64];
static int _last_i = 0;
static short int __market;
static short int __stock;
static SOCKET ClientSocket = -1;
static int _curr_index = -1;
static char _thread_stop = 0;
static HANDLE _ref_thread = NULL;


void __send_result_OK();
void __send_result_OK()
{
	char __b[2];

	if (ClientSocket == -1)
		return;

	__b[0] = 1;__b[1]=0;
	send(ClientSocket,__b,2,0);
}


void __send_result_Error(char number);
void __send_result_Error(char number)
{
	char __b[2];

	if (ClientSocket == -1)
		return;

	__b[0] = 1;__b[1]=number;
	send(ClientSocket,__b,2,0);
}


void __send_Tick(void* data,char size);
void __send_Tick(void* data,char size)
{
	if (ClientSocket == -1)
		return;

	send(ClientSocket,&size,1,0);
	send(ClientSocket,data,size,0);
}

static void __stdcall ConnectN ( int nKind, int nCode );
void	    __stdcall ConnectN ( int nKind, int nCode )
{
	_D(("Connect callback notify. nKind = %d , nCode = %d\n",nKind,nCode));
	//send login ok result to client
	__send_result_OK();
}


static void __stdcall TickN    ( short sMarketNo, short sStockidx, int nPtr);
void	    __stdcall TickN    ( short sMarketNo, short sStockidx, int nPtr)
{
	static char _f = 0;


	_D(("Tick callback notify. sMarketNo = %d,sStockidx = %d,nPtr = %d\n",sMarketNo,sStockidx,nPtr));
	if (_f == 0)
	{
		//when called first, this mean main code calling EnteryMonitor
		//so,we must init some variable , and send result of QL_Watch to client.
		__market  = sMarketNo;
		__stock   = sStockidx;
		__send_result_OK();
		_f = 1;
	}

	_last_i = nPtr;
	if (_ref_thread != NULL)
		ResumeThread(_ref_thread);
}

static DWORD WINAPI _tick_thread(LPVOID _d);
DWORD WINAPI _tick_thread(LPVOID _d)
{
	TTick data;


	while(1)
	{
		while(_curr_index < _last_i)
		{
			QL_GetTick(__market,__stock,_curr_index,&data);
			_D(("%d,%d - %d\n",_last_i,_curr_index,data.m_nClose));
			__send_Tick(&data,sizeof(TTick));
			_curr_index++;
		}
		SuspendThread(_ref_thread);
	}
	return 0;
}

static inline char _accept_socket(HWND hwnd,int _socket);
static inline char _imp_command(HWND hwnd);
static inline char _what_command(void);
static inline char _evLogin(void);
static inline char _evLogout(void);
static inline char _evWatch(void);
static inline char _evPull(void);


void SocketHaveData(HWND hwnd,int _socket,LPARAM _l);
void SocketHaveData(HWND hwnd,int _socket,LPARAM _l)
{
	if (WSAGETSELECTERROR(_l))
	{
		closesocket(_socket);
		return;
	}

	switch (WSAGETSELECTEVENT(_l))
	{
		case FD_ACCEPT:
			_accept_socket(hwnd,_socket);
			break;

		case FD_READ:
			_imp_command(hwnd);
			break;
	}
}

char _accept_socket(HWND hwnd,int _socket)
{
	SOCKET _i;


	_i=accept(_socket,NULL,NULL);
	if (_i==INVALID_SOCKET)
	{
		_D(("accept() fail\n"));
		return -1;
	}
	else
	{
		if (ClientSocket == -1)
		{
			ClientSocket = _i;
			WSAAsyncSelect(ClientSocket,hwnd,WM_SOCKET,FD_READ);
		}
		else
			closesocket(_i); //we support one pipe only
	}
	return 1;
}

char _imp_command(HWND hwnd)
{
	int _i;
	char _r;
	char _b;

	_i=recv(ClientSocket,&_b,1,0);
	if	(_i <= 0)
	{
		_D(("recv %d,incorrent\n",_i));
		closesocket(ClientSocket);
		PostMessage(hwnd,WM_CLOSE,0,0);
		_r = -1;
	}
	else if	(_state == _START)
	{
		_cmdsize = _b;
		_fillcur = 0;
		_state = _FILLING;
		_r = 1;
	}
	else if	(_state == _FILLING)
	{
		_cmdbuf[_fillcur] = _b;
		_fillcur ++;
		if (_fillcur == _cmdsize)
		{
			/*parse command*/
			if (_what_command() == 0)
			{
				closesocket(ClientSocket);
				PostMessage(hwnd,WM_CLOSE,0,0);
			}
			_state = _START;
		}
		_r = 1;
	}

	return _r;
}

char _what_command(void)
{
	if        (_cmdbuf[0] == 4)    {return 0;} //Exit command
	else if   (_cmdbuf[0] == 2)    {_evLogin();return 1;}
	else if   (_cmdbuf[0] == 3)    {_evLogout();return 1;}
	else if   (_cmdbuf[0] == 5)    {_evWatch();return 1;}
	else if   (_cmdbuf[0] == 6)    {_evPull();return 1;}
	else      return 1;
}

char _evLogin(void)
{
	char _r;

	_L(("info:\tstart login command\n"));

	_r = QL_LoginServer(LoginID,Password);
	if (_r == -1)
	{
		_L(("info:\tlogin server is fail\n"));
		__send_result_Error(-1);
		return -1;
	}

	_r = QL_AddCallBack((long)ConnectN,(long)TickN);
	if (_r == -1)
	{
		_L(("info:\tRegister call back function is fail\n"));
		__send_result_Error(-1);
		return -1;
	}


	_r = QL_ConnectDataBase();
	if (_r == -1)
	{
		_L(("info:\tjoin data base is fail\n"));
		__send_result_Error(-1);
		return -1;
	}

	_L(("info:\tLogin process OK\n"));
	//callback function will send request to Client

	return 1;
}

char _evLogout(void)
{
	_L(("info:\tstart logout command\n"));

	QL_Bye();

	_L(("info:\tlogout process OK,send result\n"));
	__send_result_OK();
	return 1;
}

char _evWatch(void)
{
	char _r;

	_L(("info:\tstart Watch command\n"));
	_D(("Watch is %s\n",_cmdbuf+1));

	_r = QL_Request(_cmdbuf+1);
	_D(("QL_Request return %d\n",_r));

	if (_r == 1)
	{
		_L(("info:\tWatch process OK\n"));
		//callback function will send request to Client
	}
	else
	{
		_L(("info:\tWatch process is fail\n"));
		__send_result_Error(-1);
	}
	return _r;
}

char _evPull(void)
{
	int index;


	_D(("start Pull command\n"));

	if (_ref_thread != NULL)
		__send_result_Error(-1);

	index = *((UINT32*)(_cmdbuf+1));
	_D(("index is %d\n",index));
	if(index < 0)
		__send_result_Error(-1);

	_curr_index = index;
	_ref_thread = CreateThread(NULL, 0,_tick_thread, NULL,0, NULL);

	_D(("Pull process OK,send result\n"));
	return 1;
}

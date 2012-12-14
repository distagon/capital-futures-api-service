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

#define _START		1
#define _FILLING	2

static int          _state    = _START;
static int          _cmdsize  = 0;
static int          _fillcur  = 0;
static char         _cmdbuf   [64];
static char         _resultb  [4];
static int          _last_i   = 0;
static short int    __market;
static short int    __stock;


extern char	LoginID[16]; //define in main.c
extern char	Password[16];//define in main.c
extern int	ClientSocket;//define in main.c

static void __stdcall ConnectN ( int nKind, int nCode );
void	    __stdcall ConnectN ( int nKind, int nCode )
{
	printf("Connect callback notify. nKind = %d , nCode = %d\n",nKind,nCode);
}


static void __stdcall TickN    ( short sMarketNo, short sStockidx, int nPtr);
void	    __stdcall TickN    ( short sMarketNo, short sStockidx, int nPtr)
{
	_D(("Tick callback notify. sMarketNo = %d,sStockidx = %d,nPtr = %d\n",sMarketNo,sStockidx,nPtr));
	__market  = sMarketNo;
	__stock   = sStockidx;
	_last_i   = nPtr;
}

char _evLogin(void);
char _evLogout(void);
char _evWatch(void);
char _evPull(void);


static int WhatCommand(void);
static int WhatCommand(void)
{
	if        (_cmdbuf[0] == 4)    {return 0;} //Exit command
	else if   (_cmdbuf[0] == 2)    {_evLogin();return 1;}
	else if   (_cmdbuf[0] == 3)    {_evLogout();return 1;}
	else if   (_cmdbuf[0] == 5)    {_evWatch();return 1;}
	else if   (_cmdbuf[0] == 6)    {_evPull();return 1;}
	else      return 1;
}

extern int RunCommand(char data);
int RunCommand(char data)
{
	if	( _state == _START)
	{
		_cmdsize = data;
		_fillcur = 0;
		_state = _FILLING;
	}

	else if	(_state == _FILLING)
	{
		_cmdbuf[_fillcur] = data;
		_fillcur ++;
		if( _fillcur == _cmdsize)
		{
			/*parse command*/
			if( WhatCommand() == 0 )
				return 0;
			_state = _START;
		}
	}

	return 1;
}










void __send_result_OK();
void __send_result_OK()
{
	char __b[2];

	if(ClientSocket == -1)
		return;

	__b[0] = 1;__b[1]=0;
	send(ClientSocket,__b,2,0);
}


void __send_result_Error(char number);
void __send_result_Error(char number)
{
	char __b[2];

	if(ClientSocket == -1)
		return;

	__b[0] = 1;__b[1]=number;
	send(ClientSocket,__b,2,0);
}


void __send_Tick(void* data,char size);
void __send_Tick(void* data,char size)
{
	if(ClientSocket == -1)
		return;

	send(ClientSocket,&size,1,0);
	send(ClientSocket,data,size,0);
}


char _evLogin(void)
{
	_D(("_evLogin\n"));
	QL_LoginServer(LoginID,Password);
	QL_AddCallBack((long)ConnectN,(long)TickN);
	QL_ConnectDataBase();

	__send_result_OK();
	return 1;
}

char _evLogout(void)
{
	_D(("_evLogout\n"));
	QL_Bye();

	__send_result_OK();
	return 1;
}

char _evWatch(void)
{
	_D(("_evWatch\n"));
	_D(("Watch is %s\n",_cmdbuf+1));
	QL_Request(_cmdbuf+1);

	__send_result_OK();
	return 1;
}

char _evPull(void)
{
	TTick   data;
	int     index;

	_D(("_evPull\n"));

	index = *((UINT32*)(_cmdbuf+1));
	if(index <= _last_i)
	{
		QL_GetTick(__market,__stock,index,&data);
		_D(("index %d data. tick price is %d\n",index,data.m_nClose));
		__send_Tick(&data,(char)sizeof(TTick));
	}

	return 1;
}


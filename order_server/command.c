#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include "orderapi.h"

#ifdef DEBUG
#define _D(x)	printf x
#else
#define _D(x)
#endif

#define	_L(x)	printf x


extern char LoginId[32];    //declare on main.c,LoginID
extern char Password[128];  //declare on main.c,Login Password

/*
   command list
	1:login
	2:set account
	3:logout
	4:exit
	5:commit market order & push directly
	6:commit price order
	7:push all order

command report
	a.login report
	b.order report
	c.account list report


command encode:
	1:login
	char:size|char:command->1

	2:set account
	char:size|char:command->2|char:value

	3:logout
	char:size|char:command->3

	4:exit
	char:size|char:command->4

	5:commit market order & push directly
	char:size|char:command->5|Cstr:name|char:amount|char:daily flag|char:Buy/Short Flag

	6:commit price order
	char:size|char:command->6|Cstr:name|Cstr:price|char:amount|char:daily flag|char:Buy/Short Flag

	7:push all order
	char:size|char:command->7

 */

char __login(void);
char __logout(void);
char __account(char index);
char __push_market_order(char* cmdbuf);
char __commit_order(char* cmdbuf);
char __push_order(char* cmdbuf);

char  __run_command_parse(int socket,void* cmd,int cmdsize);
char  __run_command_parse(int socket,void* cmd,int cmdsize)
{
	char* yes = cmd;

	if	(yes[0] == 1)	{return __login();}
	else if	(yes[0] == 2)	{return __account(yes[1]);}
	else if	(yes[0] == 3)	{return __logout();}
	else if	(yes[0] == 4)	{return -1;}
	else if	(yes[0] == 5)	{return __push_market_order(yes+1);}
	else if	(yes[0] == 6)	{return __commit_order(yes+1);}
	else if	(yes[0] == 7)	{return __push_order(yes+1);}

	else 			{_D(("Unknow command type\n")); return 1;}

}

char  RunCommand(int __cs);
char  RunCommand(int __cs)
{
	/*
	   recv to get one byte (size)
	   recv to get data of size bytes into command buffer
	   now , I get full command data
	   run parse;
	   loop
	 */

	char __size;
	int _r;
	char __cmdb[1024];

	_r = recv (__cs,&__size,1,0);
	if(_r != 1)
		return -1;

	_D(("command.c:size is %d\n",__size));

	_r = recv(__cs,__cmdb,__size-1,0);
	if(_r != __size-1)
		return -1;

	_D(("command.c:read command %d bytes\n",_r));

	return __run_command_parse(__cs,__cmdb,__size-1);
}













char __parse_order_market(char* yes);
char __parse_order_market(char* yes)
{
	char __n[16];
	char* _index;

	_index = yes;
	strcpy(__n,_index);
	_index += (strlen(__n) + 1);
	printf("parse [%s][%d][%d][%d]\n",__n,_index[0],_index[1],_index[2]);
	//OL_OrderMarket(__n,_index[0],_index[1],_index[2]);
	return 1;
}

char __parse_order_price(char* yes);
char __parse_order_price(char* yes)
{
	char __n[16];
	char __p[64];

	char* _index;
	_index = yes;
	strcpy(__n,_index);
	_index += (strlen(__n) + 1);

	strcpy(__p,_index);
	_index += (strlen(__p) + 1);

	printf("parse [%s][%s][%d][%d][%d]\n",__n,__p,_index[0],_index[1],_index[2]);
	//OL_OrderPrice(__n,__p,_index[0],_index[1],_index[2]);
	return 1;
}

char __login(void)
{
	char _r;
	_L(("info:\tstart login command\n"));
	_r = OL_LoginServer(LoginId,Password);
	_D(("login process return %d\n",_r));
	
	if( _r == 1)
	{
		_L(("info:\tlogin process OK\n"));
	}
	else
	{
		_L(("info:\tlogin process fail\n"));
	}
	
	return _r;
}

char __logout(void)
{

	_L(("info:\tstart logout command\n"));	
	OL_Bye();

	_L(("info:\tlogout process OK\n"));
	return 1;
}

char __account(char index)
{
	char _r;
	
	_L(("info:\tstart select account command\n"));
	_D(("account is %d\n",index));
	_r = OL_SetTradeAccount(index);
	_D(("OL_SetTradeAccount return is %d\n",_r));

	if( _r == 1)
	{
		_L(("info:\tselect account process OK\n"));
	}
	else
	{
		_L(("info:\tselect account process fail\n"));
	}
	
	return _r;
}

char __push_market_order(char* cmdbuf)
{
	char _r;

	_L(("info:\tpush Market order\n"));	
	_r = __parse_order_market(cmdbuf);
	_D(("return is %d\n",_r));
	

	if( _r == 1)
	{
		_L(("info:\tpush Market order OK\n"));
	}
	else
	{
		_L(("info:\tpush Market order fail\n"));
	}
	
	return _r;
}

char __commit_order(char* cmdbuf)
{
	char _r;
	_L(("info:\tstart commit a order\n"));
	_r = __parse_order_price(cmdbuf);
	_D(("return is %d\n",_r));

	if( _r == 1)
	{
		_L(("info:\tcommit order OK\n"));
	}
	else
	{
		_L(("info:\tcommit order fail\n"));
	}

	return _r;
}

char __push_order(char* cmdbuf)
{
	return 1;
}

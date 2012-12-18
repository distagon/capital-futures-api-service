#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include "orderapi.h"

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

char  __run_command_parse(int socket,void* cmd,int cmdsize);
char  __run_command_parse(int socket,void* cmd,int cmdsize)
{

	char* yes = cmd;
	int _r;

	_r = 1;
	switch (yes[0]) {
		case 1:
			printf("%s Login ...\n",LoginId);
			_r = OL_LoginServer(LoginId,Password);
			printf("return %d\n",_r);
			_r = 1;
			break;

		case 2:
			printf("Select a account\n");
			OL_SetTradeAccount(yes[1]);
			break;

		case 3:
			printf("Logout\n");
			OL_Bye();
			break;

		case 4:
			printf("exit\n");
			_r = -1;
			break;

		case 5:
			printf("push market order\n");
			_r = __parse_order_market(yes+1);
			break;

		case 6:
			printf("commit price order\n");
			_r = __parse_order_price(yes+1);
			break;

		case 7:
			printf("push order\n");
			break;

		default:
			printf("command.c: unknow , unsupport , unimplement command\n");
			break;
	}
	return _r;
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

	char size;
	int _r;
	char __cmdb[1024];

	_r = recv (__cs,&size,1,0);
	if(_r != 1)
		return -1;

	printf("command.c:size is %d\n",size);

	_r = recv(__cs,__cmdb,size-1,0);
	if(_r != size-1)
		return -1;

	printf("command.c:read command %d bytes\n",_r);

	return __run_command_parse(__cs,__cmdb,size-1);
}


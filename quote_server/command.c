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



#define _Start      1
#define _FILLING    2

static int  _state = _Start;
static int  _cmdsize = 0;
static int  _fillcur = 0;
static char cmdbuf[64];
static int  __last_trade_id = 0;
static short int __market,__stock;

extern char LoginID[16]; //define in main.c
extern char Password[16];//define in main.c

static void __stdcall ConnectN ( int nKind, int nCode );
void        __stdcall ConnectN ( int nKind, int nCode ) {
    printf("Connect callback notify. nKind = %d , nCode = %d\n",nKind,nCode);
}


static void __stdcall TickN    ( short sMarketNo, short sStockidx, int nPtr);
void        __stdcall TickN    ( short sMarketNo, short sStockidx, int nPtr){
    printf("Tick callback notify. sMarketNo = %d,sStockidx = %d,nPtr = %d\n",sMarketNo,sStockidx,nPtr);
    __market        = sMarketNo;
    __stock         = sStockidx;
    __last_trade_id = nPtr;
}



static int WhatCommand(void);
static int WhatCommand(void) {
    TTick   data;
    int     index;

    switch (cmdbuf[0]) {
        case 1:
        case 2:
            printf("bp1\n");
            QL_LoginServer(LoginID,Password);
            QL_AddCallBack((long)ConnectN,(long)TickN);
            QL_ConnectDataBase();
            break;

        case 3:
            QL_Bye();
            break;

        case 4:
            return 0;
            
        case 5:
            printf("bp2,watch is %s\n",cmdbuf+1);
            QL_Request(cmdbuf+1);
            break;

        case 6:
            index = *((UINT32*)(cmdbuf+1));
            if(index <= __last_trade_id) {
                QL_GetTick(__market,__stock,index,&data);
                printf("index %d data. tick price is %d\n",index,data.m_nClose);
            }
            break;
    }
    
    return 1;
}

extern int RunCommand(char data);
int RunCommand(char data) {

    if( _state == _Start) {
        _cmdsize = data;
        _fillcur = 0;
        _state = _FILLING;
    }

    else if (_state == _FILLING) {
        cmdbuf[_fillcur] = data;
        _fillcur ++;
        if( _fillcur == _cmdsize) {
            /*parse command*/
            if( WhatCommand() == 0 ) return 0;
            _state = _Start;
        }
    }

    return 1;
}

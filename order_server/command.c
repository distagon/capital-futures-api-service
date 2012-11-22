#include <windows.h>
#include <winsock2.h>
#include <stdio.h>


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
    char:size|char:command->5|char:Buy/Short Flag|char:amount

    6:commit price order
    char:size|char:command->6|Cstr:price|char:Buy/Short Flag|char:amount

    7:push all order
    char:size|char:command->7

*/

char  __run_command_parse(int socket,void* cmd,int cmdsize);
char  __run_command_parse(int socket,void* cmd,int cmdsize) {

    char* yes = cmd;
    int _r;

    _r = 1;
    switch (yes[0]) {
        case 1:
        printf("Login\n");
        break;

        case 2:
        printf("Select a account\n");
        break;

        case 3:
        printf("Logout\n");
        break;

        case 4:
        printf("exit\n");
        _r = -1;
        break;

        case 5:
        printf("push market order\n");
        break;

        case 6:
        printf("commit price order\n");
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
char  RunCommand(int __cs) {
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
    if(_r != 1) {
        return -1;
    }
    printf("command.c:size is %d\n",size);

    _r = recv(__cs,__cmdb,size-1,0);
    if(_r != size-1) {
        return -1;
    }
    printf("command.c:read command %d bytes\n",_r);

    return __run_command_parse(__cs,__cmdb,size-1);
}


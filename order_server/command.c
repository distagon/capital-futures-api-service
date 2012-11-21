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

*/


char  RunCommand(int __cs);
char  RunCommand(int __cs) {
    /*
    recv to get one byte (size)
    recv to get data of size bytes into command buffer
    now , I get full command data
    run parse;
    loop
    */
}


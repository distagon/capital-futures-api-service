/*
    command list

    1:login
    2:set account
    3:order
    4:logout
    5:exit

    command report
    a.login report
    b.order report
    c.account list report

    encode
    1:login
    char:size|char:command:=1|Cstr:loginID|Cstr:password

    2:set account
    char:size|char:command:=2|integer:set

    3:order
    4:logout
    char:size|char:command:=4

    5:exit
    char:size|char:command:=5
*/


extern char  RunCommand(void* datab,int size);
char  RunCommand(void* datab,int size) {
    /*
    if state is start
        get command length
    
        if size < command legth
        then
            not full
            copy data into command buffer
            state become wait left
        else
            full 
            copy into command buffer
            run command
            if size = legth
                clear command buffer
            else 
                recall myself and use rest data
            endif
        endif
        
    else state is wait
        if size < rest command legth
        then
            still not full ! shit
            copy data into command buffer
            state still wait left
        else
            full
            copy into command buffer
            run command
            if size = legth left
                clear command buffer
            else 
                recall myself and use rest data
            endif
    endif
    */
}


# Introduction #

此文件是將下單server 的溝通二進位格式給予規格化


# Details #

command list
  1. login  通知login server 去login 後台
  1. set account
  1. logout
  1. exit
  1. commit market order & push directly
  1. commit price order
  1. push all order

command report
  1. ogin report
  1. rder report
  1. ccount list report


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
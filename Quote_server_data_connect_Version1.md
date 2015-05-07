# Introduction #

此篇文章的目的是要將報價server 的溝通介面逕行規格化.


# Details #

Version 1 :
此版本server 接受以下命令
Login -> 通知server 要login期貨商
返回 -> 0 : 成功 . -1 : 失敗

Logout -> 通知server 要logout期貨商
返回 -> 0 : 成功 . -1 : 失敗

Exit ->  通知server 結束整個server 程式
此命令不返回

Watch -> 通知server 跟期貨商註冊想查看的商品名稱
返回 -> 0 : 成功 . -1 : 失敗

Pull -> 通知server 把報價資料丟回來
返回 -> 0 : 成功 . -1 : 失敗

以下是 hand shake
server      client
> <-----Login
OK|Fail----->

> <-----Watch
OK|Fail----->

> <-----Pull
one data----->

> <-----Pull
one data----->

.
.
.
.
.
.
.


> <-----Logout
OK|Fail----->

> <-----Exit
(server program terminate)


以下是命令的詳細二進位格式
Login
byte:1{command size}|byte:1{2}
return
byte:1{return size}|byte:1{1:OK;-1:Fail}


Logout
byte:1{command size}|byte:1{3}
return
byte:1{return size}|byte:1{1:OK;-1:Fail}


Exit
byte:1{command size}|byte:1{4}
(no return)

Watch
byte:1{command size}|byte:10{a C char array,Null terminal}
return
byte:1{return size}|byte:1{1:OK;-1:Fail}



Pull
byte:1{command size}|byte:4{32 bit integer}
return
if OK -> byte:1{return size}|byte:value{tick data}
if Fail -> byte:1{return size}|byte:1{-1:Fail}
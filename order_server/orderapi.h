#ifndef __ORDER_LIBRARY__
#define __ORDER_LIBRARY__

typedef struct {
    char Market[4];
    char BranchName[16];
    char BranchID[8];
    char Account[16];
    char IdentityNumber[16];
} UserAccount;

#define TYPE_ROD    0 //Rest of Day
#define TYPE_IOC    1 //Immediate-or-Cancel
#define TYPE_FOK    2 //Fill-or-Kill

#define TYPE_BUY    0 //buy
#define TYPE_SELL   1 //sell

#define TYPE_DAILY  1 //Daily trade
#define TYPE_OPPEN  0 //not Daily


char OL_LoginServer(char* username , char* password);
UserAccount* OL_GetUserAccount(int* number);
char OL_SetTradeAccount(int index);
char OL_OrderMarket(char* Stockname,unsigned char amount,char DailyFlag,char BSFlag);
char OL_OrderPrice (char* Stockname,char* price,unsigned char amount,char DailyFlag,char BSFlag);
void OL_Bye(void);

#endif


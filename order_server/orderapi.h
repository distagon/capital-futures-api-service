#ifndef __ORDER_LIBRARY__
#define __ORDER_LIBRARY__

typedef struct {
    char Market[4];
    char BranchName[16];
    char BranchID[8];
    char Account[16];
    char IdentityNumber[16];
} UserAccount;

char OL_LoginServer(char* username , char* password);
UserAccount* OL_GetUserAccount(int* number);
char OL_SetTradeAccount(int index);
char OL_Order(char* Stockname);
void OL_Bye(void);

#endif


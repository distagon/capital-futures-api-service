#ifndef __QUOTE_LIBRARY__
#define __QUOTE_LIBRARY__

typedef struct STOCK
{
    short m_sStockidx; 
    short m_sDecimal; 
    short m_sTypeNo; 
    char m_cMarketNo;
    char m_caStockNo[20]; 
    char m_caName[10]; 
    int m_nOpen; 
    int m_nHigh; 
    int m_nLow; 
    int m_nClose; 
    int m_nTickQty; 
    int m_nRef; 
    int m_nBid; 
    int m_nBc;
    int m_nAsk; 
    int m_nAc; 
    int m_nTBc; 
    int m_nTAc; 
    int m_nFutureOI; 
    int m_nTQty; 
    int m_nYQty; 
    int m_nUp; 
    int m_nDown;
} TStock;

typedef struct TICK
{
    int m_nPtr;
    int m_nTime; 
    int m_nBid; 
    int m_nAsk; 
    int m_nClose; 
    int m_nQty; 
} TTick;


extern char QL_LoginServer(char* username, char* password);
extern char QL_AddCallBack(long connect,long ticks);
extern char QL_ConnectDataBase(void);
extern char QL_Request(char* stockname);
extern char QL_GetTick(short sMarketNo, short sStockidx,int nPtr, TTick* pTick);
extern void QL_Bye(void);

#define SK_SUCCESS      0
#define SK_FAIL         -1
#define SK_ERROR_STRING_LENGTH_NOT_ENOUGH   -2
#define SK_ERROR_SERVER_NOT_CONNECTED       -3
#define SK_ERROR_INITIALIZE_FAIL            -4
#define SK_ERROR_ACCOUNT_NOT_EXIST          1
#define SK_ERROR_ACCOUNT_MARKET_NOT_MATCH   2
#define SK_ERROR_PERIOD_OUT_OF_RANGE        3
#define SK_ERROR_FLAG_OUT_OF_RANGE          4
#define SK_ERROR_BUYSELL_OUT_OF_RANGE       5
#define SK_ERROR_ORDER_SERVER_INVALID       6
#define SK_ERROR_PERMISSION_DENIED          7
#define SK_KLINE_DATA_TYPE_NOT_FOUND        8
#define SK_ERROR_PERMISSION_TIMEOUT         9

#define SK_SUBJECT_CONNECTION_CONNECTED     100
#define SK_SUBJECT_CONNECTION_DISCONNECT    101
#define SK_SUBJECT_QUOTE_PAGE_EXCEED        200
#define SK_SUBJECT_QUOTE_PAGE_INCORRECT     201
#define SK_SUBJECT_TICK_PAGE_EXCEED         210
#define SK_SUBJECT_TICK_PAGE_INCORRECT      211
#define SK_SUBJECT_TICK_STOCK_NOT_FOUND     212
#define SK_SUBJECT_BEST5_DATA_NOT_FOUND     213
#define SK_SUBJECT_QUOTEREQUEST_NOT_FOUND   214
#define SK_SUBJECT_SERVER_TIME_NOT_FOUND    215


extern int __servererr;
extern char QL_ERR;

#define QL_R_OK     0
#define QL_R_FAIL   0x100 + 1;

#endif


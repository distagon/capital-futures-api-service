#include <stdio.h>
#include <windows.h>
#include "quoteapi.h"

//DLL function . use LoadLibrary / GetProcAddress to get function address
static int  __stdcall (*__pointer_SKQuoteLib_GetVersion)      (char* lpszVersion,int* pnSize);
static int  __stdcall (*__pointer_SKQuoteLib_Initialize)      (char* lpszLoginID,char* lpszPassword);
static int  __stdcall (*__pointer_SKQuoteLib_EnterMonitor)    (void);
static int  __stdcall (*__pointer_SKQuoteLib_LeaveMonitor)    (void);
static int  __stdcall (*__pointer_SKQuoteLib_RequestStocks)   (short* psPageNo,char* pStockNos);
static int  __stdcall (*__pointer_SKQuoteLib_RequestTicks)    (short* psPageNo,char* pStockNo);
static int  __stdcall (*__pointer_SKQuoteLib_AttachConnectionCallBack)    (long lCallBack);
static int  __stdcall (*__pointer_SKQuoteLib_AttachQuoteCallBack)         (long lCallBack);
static int  __stdcall (*__pointer_SKQuoteLib_AttachTicksCallBack)         (long lCallBack);
static int  __stdcall (*__pointer_SKQuoteLib_AttachTicksGetCallBack)      (long lCallBack);
static int  __stdcall (*__pointer_SKQuoteLib_GetTick)                     (short sMarketNo, short sStockidx,int nPtr, TTick* pTick);
/*
int __stdcall SKQuoteLib_GetVersion( [out] char* lpszVersion, [in,out] int* pnSize) 
int __stdcall SKQuoteLib_Initialize( [in]char* lpszLoginID, [in]char* lpszPassword) 
int __stdcall SKQuoteLib_EnterMonitor() 
int __stdcall SKQuoteLib_LeaveMonitor()
int __stdcall SKQuoteLib_RequestStocks( [in,out] short* psPageNo, [in]TCHAR* pStockNos) 
int __stdcall SKQuoteLib_RequestTicks( [in,out]short* psPageNo, [in]TCHAR* pStockNo)
int __stdcall SKQuoteLib_GetStockByIndex([in]short sMarketNo, [in]short sIndex,[in,out] TStock* pStock)
int __stdcall SKQuoteLib_GetStockByNo([in] const TCHAR* lpszStockNo,[in,out] TStock* pStock) 
int __stdcall SKQuoteLib_GetTick([in]short sMarketNo, [in]short sStockidx, [in]int nPtr, [in,out]TTick* pTick)
int __stdcall SKQuoteLib_GetBest5( [in]short sMarketNo, [in]short sStockidx, [in,out]TBest5* pBest5)
int __stdcall SKQuoteLib_GetStrikePrices() 
int __stdcall SKQuoteLib_GetKLine([in,out] char* caStockNo, [in] int KLineType )
int __stdcall SKQuoteLib_GetMarketTot( [in] int index, [in] char cMarketNo , [out] short sPrt , [out] long* lTime, [out] long* lTotv,[out] long* lTots,[out] long* lTotc ) 
int __stdcall SKQuoteLib_GetMarketBuySell( [in] int index, [in] char cMarketNo , [out] short sPrt , [out] long* lTime, [out] long* lBc, [out] long* lSc, [out] long* lBs, [out] long* lSs )
int __stdcall SKQuoteLib_GetMarketHighLow( [in] int index, [in] char cMarketNo , [out] short sPrt , [out] long* lTime, [out] short* sUp, [out] short* sDown, [out] short* sHigh, [out] short* sLow,  [out ]short* sNoChange )
int __stdcall SKQuoteLib_Delta( [in] int nCallPut, [in] double S, [in] double K, [in] double R, [in]double T, [in] double sigma, [out] double* dDelta ) 
int __stdcall SKQuoteLib_Gamma( [in] double S, [in] double K, [in] double R, [in] double T,[in]double sigma , [out] double* dGamma) 
int __stdcall SKQuoteLib_Theta( [in] int nCallPut, [in] double S, [in] double K, [in] double R,  [in]double T, [in] double sigma , [out] double* dTheta)
int __stdcall SKQuoteLib_Vega( [in] double S, [in] double K, [in] double R, [in] double T, [in]double sigma , [out] double* dVega)
int __stdcall SKQuoteLib_Rho([in] int nCallPut, [in] double S, [in] double K, [in] double R, [in]double T, [in]double sigma , [out] double* dRho)
int __stdcall SKQuoteLib_AttachConnectionCallBack([in]long lCallBack)
int __stdcall SKQuoteLib_AttachQuoteCallBack([in]long lCallBack) 
int __stdcall SKQuoteLib_AttachTicksCallBack([in] long lCallBack) 
int __stdcall SKQuoteLib_AttachTicksGetCallBack([in] long lCallBack) 
int __stdcall SKQuoteLib_AttachBest5CallBack([in] long lCallBack)
int __stdcall SKQuoteLib_AttachBest5GetCallBack([in] long lCallBack) 
int __stdcall AttachStrikePricesCallBack([in] long lCallBack)
int __stdcall SKQuoteLib_AttachKLineDataCallBack([in] long lCallBack)
int __stdcall SKQuoteLib_AttachMarketTotCallBack( [in] long lCallBack)
int __stdcall SKQuoteLib_AttachMarketBuySellCallBack( [in] long lCallBack)
int __stdcall SKQuoteLib_AttachMarketHighLowCallBack( [in] long lCallBack)
int __stdcall SKQuoteLib_RequestServerTime( ) 
int __stdcall SKQuoteLib_AttchServerTimeCallBack ( [in] long lCallBack)
int __stdcall SKQuoteLib_GetServerTime ( [in,out]CFormat05* pFormat05) 
int __stdcall SKQuoteLib_QuoteRequest ( ) 
int __stdcall SKQuoteLib_AttachQuoteRequestCallBack( [in] long lCallBack)
int __stdcall SKQuoteLib_GetQuoteRequest( [in] int nIndex, [in,out]CQuoteItem* pQuoteItem)
int __stdcall SKQuoteLib_AttachFutureTradeInfoCallBack ( [in] long CallBack) 
*/

#define SKQuoteLib_GetVersion       (*__pointer_SKQuoteLib_GetVersion)
#define SKQuoteLib_Initialize       (*__pointer_SKQuoteLib_Initialize)
#define SKQuoteLib_EnterMonitor     (*__pointer_SKQuoteLib_EnterMonitor)
#define SKQuoteLib_LeaveMonitor     (*__pointer_SKQuoteLib_LeaveMonitor)
#define SKQuoteLib_RequestStocks    (*__pointer_SKQuoteLib_RequestStocks)
#define SKQuoteLib_RequestTicks     (*__pointer_SKQuoteLib_RequestTicks)
#define SKQuoteLib_GetTick          (*__pointer_SKQuoteLib_GetTick)

#define SKQuoteLib_AttachConnectionCallBack (*__pointer_SKQuoteLib_AttachConnectionCallBack)
#define SKQuoteLib_AttachQuoteCallBack      (*__pointer_SKQuoteLib_AttachQuoteCallBack)
#define SKQuoteLib_AttachTicksCallBack      (*__pointer_SKQuoteLib_AttachTicksCallBack)
#define SKQuoteLib_AttachTicksGetCallBack   (*__pointer_SKQuoteLib_AttachTicksGetCallBack)

////////////////////////////////////////

static HINSTANCE    __lib = NULL;

static char __load_ql(void);
static char __load_ql(void) {
    void* __funcp;

    __lib = LoadLibrary("SKQuoteLib.dll");
    if(__lib == NULL) return -1;

    __funcp = GetProcAddress(__lib,"SKQuoteLib_GetVersion");
    if(__funcp == NULL) return -1;
    __pointer_SKQuoteLib_GetVersion = __funcp;

    __funcp = GetProcAddress(__lib,"SKQuoteLib_Initialize");
    if(__funcp == NULL) return -1;
    __pointer_SKQuoteLib_Initialize = __funcp;

    __funcp = GetProcAddress(__lib,"SKQuoteLib_EnterMonitor");
    if(__funcp == NULL) return -1;
    __pointer_SKQuoteLib_EnterMonitor = __funcp;

    __funcp = GetProcAddress(__lib,"SKQuoteLib_LeaveMonitor");
    if(__funcp == NULL) return -1;
    __pointer_SKQuoteLib_LeaveMonitor = __funcp;

    __funcp = GetProcAddress(__lib,"SKQuoteLib_AttachConnectionCallBack");
    if(__funcp == NULL) return -1;
    __pointer_SKQuoteLib_AttachConnectionCallBack = __funcp;

    __funcp = GetProcAddress(__lib,"SKQuoteLib_AttachQuoteCallBack");
    if(__funcp == NULL) return -1;
    __pointer_SKQuoteLib_AttachQuoteCallBack = __funcp;

    __funcp = GetProcAddress(__lib,"SKQuoteLib_AttachTicksCallBack");
    if(__funcp == NULL) return -1;
    __pointer_SKQuoteLib_AttachTicksCallBack = __funcp;

    __funcp = GetProcAddress(__lib,"SKQuoteLib_AttachTicksGetCallBack");
    if(__funcp == NULL) return -1;
    __pointer_SKQuoteLib_AttachTicksGetCallBack = __funcp;

    __funcp = GetProcAddress(__lib,"SKQuoteLib_RequestTicks");
    if(__funcp == NULL) return -1;
    __pointer_SKQuoteLib_RequestTicks = __funcp;

    __funcp = GetProcAddress(__lib,"SKQuoteLib_RequestStocks");
    if(__funcp == NULL) return -1;
    __pointer_SKQuoteLib_RequestStocks = __funcp;

    __funcp = GetProcAddress(__lib,"SKQuoteLib_GetTick");
    if(__funcp == NULL) return -1;
    __pointer_SKQuoteLib_GetTick = __funcp;

    return 1;
}

static void __free_ql(void);
static void __free_ql(void) {
    if(__lib != NULL) {
        FreeLibrary(__lib);
        __lib = NULL;
    }
}

////////////////////////////////////////




//###################################################
char QL_LoginServer(char* username, char* password) {
    char _r;
    _r = __load_ql();
    
    printf("Load SKQuoteLib.dll %d\n",_r);
    if (_r == -1) return -1;

    _r = SKQuoteLib_Initialize(username,password);
    printf("Login ... %d\n",_r);
    if (_r != 0) return -1;

    return 1;
}


char QL_AddCallBack(long connect,long ticks) {
    printf("add connect callback %d\n",SKQuoteLib_AttachConnectionCallBack ( connect ));
    printf("add get tick callback %d\n",SKQuoteLib_AttachTicksCallBack ( ticks ));
    return 1;
}

char QL_ConnectDataBase(void) {
    printf("Connect Database %d\n",SKQuoteLib_EnterMonitor());
    return 1;
}



char QL_Request(char* stockname) {
    short int __page;
    
    __page = -1;
    printf("Request '%s' , return %d\n",stockname,SKQuoteLib_RequestTicks(&__page,stockname));

    return 1;
}

char QL_GetTick(short sMarketNo, short sStockidx,int nPtr, TTick* pTick) {
    SKQuoteLib_GetTick(sMarketNo,sStockidx,nPtr,pTick);
    return 1;
}

void QL_Bye(void) {
    printf("disconnect database %d\n",SKQuoteLib_LeaveMonitor() );
    printf("Release API Library\n");
    __free_ql();
}
//###################################################


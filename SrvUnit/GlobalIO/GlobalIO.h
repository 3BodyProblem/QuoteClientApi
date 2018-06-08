#ifndef _GLOBALIO_DEFINE_H
#define _GLOBALIO_DEFINE_H

#include "../DataClient/DataClient.h"
#include "../OptionIO/MOptionIO.h"
#include "../MLogUnit/MLogUnit.h"

#include "../SubDll/MDllMgr.h"
#include "../SubDll/MVPlat.h"
#include "../SubDll/MDataProc.h"
#include "../SubDll/MDataIO.h"


extern QuoteClientSpi *		Global_pSpi;
extern bool					Global_bInit;
extern int32_t				Global_MajorVer;
extern int32_t				Global_MinorVer;
extern HANDLE				Global_Handle;

extern MOptionIO			Global_Option;
extern MDataClient			Global_Client;
extern MLogUnit				Global_LogUnit;					
extern MDllMgr				Global_DllMgr;
extern MVPlatIO				Global_MVPlatIO;
extern MDataProc			Global_DataProc;
extern MDataIO				Global_DataIO;
extern MPrimeClient			Global_PrimeClient;
extern MQueryClient			Global_QueryClient;


extern int32_t			Global_StartWork();
extern void				Global_EndWork();


enum QL4XMarket
{
	QL4X_SHStock = 0,
	QL4X_SZStock = 1,

	QL4X_ZJQH	= 9,
	QL4X_CNF	= 14,

	QL4X_SHOPT	= 27,
	QL4X_ZJOPT	= 28,

	//QL4X_SHL2	 = 3,
	//QL4X_SZL2    = 16,

	QL4X_SZOPT   = 31,

	QL4X_CNFOPT	 = 35,
};


extern void			Global_WriteLog(int32_t type, int32_t level,const char* pszsrc, const char * pszmsg);
extern uint8_t		Global_MarketID2XDFID(int nMarketID);	//nMarket ==>XDFMarket
extern int			Global_XDFID2MarketID(uint8_t cXDFMarket);	//XDFMarket ==> nMarket



#endif
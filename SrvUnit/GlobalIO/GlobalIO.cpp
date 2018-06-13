
#include "StdAfx.h"
#include "GlobalIO.h"


QuoteClientSpi *	Global_pSpi =0;
bool				Global_bInit = false;
int32_t				Global_MajorVer = 5;
int32_t				Global_MinorVer = 16;
HANDLE				Global_Handle=0;

MDataClient			Global_Client;
MLogUnit			Global_LogUnit;	
MOptionIO			Global_Option;
MDllMgr				Global_DllMgr;
MVPlatIO			Global_MVPlatIO;
MDataProc			Global_DataProc;
MDataIO				Global_DataIO;
MPrimeClient		Global_PrimeClient;
MQueryClient		Global_QueryClient;


void		Global_WriteLog(int32_t type, int32_t level,const char* pszsrc, const char * pszmsg)
{
	Global_LogUnit.WriteLog(type, level, pszsrc,pszmsg);
}

uint8_t Global_MarketID2XDFID(int nMarketID)
{
	uint8_t cMarket = 200;
	switch (nMarketID)
	{
	case QL4X_SZOPT:
		cMarket = XDF_SZOPT;
		break;
	case QL4X_SHOPT:		//27
		cMarket = XDF_SHOPT;
		break;
	case QL4X_ZJOPT:		//28
		cMarket = XDF_ZJOPT;
		break;
	case QL4X_ZJQH:	//9
		cMarket = XDF_CF;
		break;
	case QL4X_SHStock://0
		cMarket =XDF_SH;
		break;
	case QL4X_SZStock:	//1
		cMarket = XDF_SZ;
		break;
	case QL4X_CNF:		//14
		cMarket = XDF_CNF;
		break;
	case QL4X_CNFOPT:		//35
		cMarket = XDF_CNFOPT;
		break;
	}

	return cMarket;
}


int			Global_XDFID2MarketID(uint8_t cXDFMarket)
{
	int nMarket = 200;
	switch (cXDFMarket)
	{
	case XDF_SZOPT:
		nMarket = QL4X_SZOPT;
		break;
	case XDF_SHOPT:		
		nMarket = QL4X_SHOPT;	//27
		break;
	case XDF_ZJOPT:		
		nMarket = QL4X_ZJOPT;	//28
		break;
	case XDF_CF:	
		nMarket = QL4X_ZJQH;	//9
		break;
	case XDF_CNF:
		nMarket = QL4X_CNF;		//14
		break;
	case XDF_SH:
		nMarket =QL4X_SHStock;	//0
		break;
	case XDF_SZ:	
		nMarket = QL4X_SZStock;	//1
		break;
	case XDF_CNFOPT:
		nMarket = QL4X_CNFOPT;	//35
		break;
	}

	return nMarket;
}

int32_t		Global_StartWork()
{
	int iret = Global_Option.Instance();
	if (iret <0)
	{
		return -1;
	}
	iret = Global_MVPlatIO.Instance();
	if (iret <0)
	{
		return -2;
	}
	iret = Global_DataProc.Instance();
	if (iret <0)
	{
		return -3;
	}
	iret = Global_LogUnit.Instance();
	if (iret <0)
	{
		return -4;
	}
	if( Global_QueryClient.Initialize() < 0 )
	{
		return -5;
	}
	iret = Global_DllMgr.Instance();
	if (iret <0)
	{
		return -6;
	}

	return 1;
}

void			Global_EndWork()
{
	printf("[XDFClient:Global_EndWork]begin\n");
	Global_pSpi =0;
	MThread::StopAllThread();

	Global_DllMgr.Release();
	Global_DataIO.Release();	
	Global_LogUnit.Release();
	Global_DataProc.Release();
	Global_MVPlatIO.Release();
	Global_Option.Release();
	Global_QueryClient.Release();
	printf("[XDFClient:Global_EndWork]end\n");
}





#pragma warning(disable : 4786)
#include "stdafx.h"
#include <math.h>
#include "DataClient.h"
#include <string>
#include <fstream>
#include <sstream>
#include <shellapi.h>
#include <shlwapi.h>
#include "../GlobalIO/GlobalIO.h"


CTYPENAMEFUNC  QuoteClientApi * STDCALL CreateQuoteApi(const char *pszDebugPath)
{
	if (!Global_bInit)
	{
		Global_bInit = true;
		Global_StartWork();
	}
	return &Global_Client;
}

CTYPENAMEFUNC  QuotePrimeApi * STDCALL CreatePrimeApi()
{
	return &Global_PrimeClient;
}

CTYPENAMEFUNC	QuoteQueryApi* STDCALL CreateQueryApi()
{
	return &Global_QueryClient;
}

CTYPENAMEFUNC  const char * STDCALL GetDllVersion(int &nMajorVersion, int &nMinorVersion)
{
	
	static char szbuf[255]={0};
	_snprintf(szbuf,254,"V%.02d B%.02d", Global_MajorVer,Global_MinorVer);
	nMajorVersion = Global_MajorVer;
	nMinorVersion = Global_MinorVer;
	
	return szbuf;
}

CTYPENAMEFUNC  int 	STDCALL GetSettingInfo(tagQuoteSettingInfo* pArrMarket, int nCount)
{
	Global_Option.Instance();
	int nrealcount = Global_Option.GetKeyFileCount();
	if (0 == pArrMarket)
	{
		return nrealcount;
	}
	
	tagKeyFileInfo oInfo;
	int ncopycount = min(nrealcount, nCount);
	for (int i=0; i<ncopycount; i++)
	{
		Global_Option.GetKeyFileInfo(i, oInfo);
		pArrMarket[i].cMarketID = oInfo.nMarketID;
		strcpy(pArrMarket[i].cMarketChn, oInfo.cMarketChn);
		pArrMarket[i].nStatus = Global_DllMgr.GetMarketStat(oInfo.nMarketID);

		MData4XDll*		p4xDLL = Global_DllMgr.GetMarketDll( pArrMarket[i].cMarketID );
		if( NULL != p4xDLL )
		{
			tagDll_DriverStatusInfo		tagStatusInfo = { 0 };

			p4xDLL->GetDllStatus( &tagStatusInfo );

			unsigned short				nPort = *((unsigned short*)tagStatusInfo.HQSourceAddr);
			::sprintf( pArrMarket[i].cAddress, "%s:%u", tagStatusInfo.HQSourceAddr+2, nPort );
		}
		else
		{
			strcpy(pArrMarket[i].cAddress, oInfo.cAddress);
		}
	}
	return nrealcount;
}

//....................................................................................................................................................................................................................................................................
MDataClient::MDataClient()
{
	
}

MDataClient::~MDataClient()
{

}

int32_t STDCALL	MDataClient::Init()
{
	if (!Global_bInit)
	{
		Global_bInit = true;
		Global_StartWork();
	}

	return 1;
}

void STDCALL MDataClient::Release()
{
	if (Global_bInit)
	{
		Global_bInit=false;
		Global_EndWork();
		Global_pSpi=0;
	}
}

void STDCALL MDataClient::RegisterSpi(QuoteClientSpi * pspi)
{
	Global_pSpi = pspi;
}

int32_t STDCALL	MDataClient::BeginWork()
{
	int iret = Global_DataIO.Instance(); 
	if (iret <0)
	{
		return  -1;
	}
	iret = Global_DllMgr.BeginWork();
	if (iret<0)
	{
		return -2;
	}
	return iret;
}

void STDCALL MDataClient::EndWork()
{
	Global_DllMgr.EndWork();
	Global_DataIO.Release();
}

int	 STDCALL		MDataClient::GetMarketInfo(unsigned char cMarket, char* pszInBuf, int nInBytes)
{
	int iret = Global_DllMgr.GetMarketInfo(cMarket, pszInBuf, nInBytes);
	return iret;
}

int32_t	STDCALL		MDataClient::GetCodeTable(uint8_t cMarket, char* pszInBuf, int32_t nInBytes, int32_t& nCount)
{
	int iret = Global_DllMgr.GetCodeTable(cMarket, pszInBuf, nInBytes, nCount);
	return iret;
}


int32_t STDCALL		MDataClient::GetLastMarketDataAll(uint8_t cMarket, char* pszInBuf, int32_t nInBytes)
{
	int iret = Global_DllMgr.GetLastMarketDataAll(cMarket, pszInBuf, nInBytes);
	return iret;
}

int32_t STDCALL		MDataClient::GetMarketStatus(uint8_t cMarket,int32_t& nStatus, uint32_t& ulTime, int64_t * pI64Send, int64_t * pI64Recv)
{
	int iret = Global_DllMgr.GetMarketStatus(cMarket,nStatus, ulTime, pI64Send, pI64Recv);
	return iret;
}


///< ----------------------------- 60分钟线 --------------------------------------------


Minute60LineStatus::Minute60LineStatus()
{
	m_bCalled0 = false;
	m_bCalled1 = false;
	m_bCalled2 = false;
	m_bCalled3 = false;
	m_cMkID = 0;
	m_nDate1 = 0;
	m_nTime1 = 0;
	m_f103000PricePx1 = 0;			///< 第一笔快照的现价
	m_f103000Amount1 = 0;			///< 第一笔快照的成交金额
	m_n103000Volume1 = 0;			///< 第一笔快照的成交量
	m_n103000NumTrades1 = 0;		///< 成交笔数
	m_f130000PricePx1 = 0;			///< 第一笔快照的现价
	m_f130000Amount1 = 0;			///< 第一笔快照的成交金额
	m_n130000Volume1 = 0;			///< 第一笔快照的成交量
	m_n130000NumTrades1 = 0;		///< 成交笔数
	m_f140000PricePx1 = 0;			///< 第一笔快照的现价
	m_f140000Amount1 = 0;			///< 第一笔快照的成交金额
	m_n140000Volume1 = 0;			///< 第一笔快照的成交量
	m_n140000NumTrades1 = 0;		///< 成交笔数
	m_f150000PricePx1 = 0;			///< 第一笔快照的现价
	m_f150000Amount1 = 0;			///< 第一笔快照的成交金额
	m_n150000Volume1 = 0;			///< 第一笔快照的成交量
	m_n150000NumTrades1 = 0;		///< 成交笔数
	m_fPriceRate = 0;
	::memset( Code, 0, sizeof(Code) );
	::memset( &m_tag1030Line, 0, sizeof(T_Minute_Line) );
	::memset( &m_tag1300Line, 0, sizeof(T_Minute_Line) );
	::memset( &m_tag1400Line, 0, sizeof(T_Minute_Line) );
	::memset( &m_tag1500Line, 0, sizeof(T_Minute_Line) );
}

Minute60LineStatus::Minute60LineStatus( const Minute60LineStatus& refObj )
{
	::memcpy( &m_tag1030Line, &(refObj.m_tag1030Line), sizeof(T_Minute_Line) );
	::memcpy( &m_tag1300Line, &(refObj.m_tag1300Line), sizeof(T_Minute_Line) );
	::memcpy( &m_tag1400Line, &(refObj.m_tag1400Line), sizeof(T_Minute_Line) );
	::memcpy( &m_tag1500Line, &(refObj.m_tag1500Line), sizeof(T_Minute_Line) );

	::memcpy( Code, refObj.Code, sizeof(Code) );
	m_cMkID = refObj.m_cMkID;					///< 市场ID
	m_nDate1 = refObj.m_nDate1;					///< 当前市场日期
	m_nTime1 = refObj.m_nTime1;					///< 当前市场时间
	m_f103000PricePx1 = refObj.m_f103000PricePx1;			///< 第一笔快照的现价
	m_f103000Amount1 = refObj.m_f103000Amount1;			///< 第一笔快照的成交金额
	m_n103000Volume1 = refObj.m_n103000Volume1;			///< 第一笔快照的成交量
	m_n103000NumTrades1 = refObj.m_n103000NumTrades1;		///< 成交笔数
	m_f130000PricePx1 = refObj.m_f130000PricePx1;			///< 第一笔快照的现价
	m_f130000Amount1 = refObj.m_f130000Amount1;			///< 第一笔快照的成交金额
	m_n130000Volume1 = refObj.m_n130000Volume1;			///< 第一笔快照的成交量
	m_n130000NumTrades1 = refObj.m_n130000NumTrades1;		///< 成交笔数
	m_f140000PricePx1 = refObj.m_f140000PricePx1;			///< 第一笔快照的现价
	m_f140000Amount1 = refObj.m_f140000Amount1;			///< 第一笔快照的成交金额
	m_n140000Volume1 = refObj.m_n140000Volume1;			///< 第一笔快照的成交量
	m_n140000NumTrades1 = refObj.m_n140000NumTrades1;		///< 成交笔数
	m_f150000PricePx1 = refObj.m_f150000PricePx1;			///< 第一笔快照的现价
	m_f150000Amount1 = refObj.m_f150000Amount1;			///< 第一笔快照的成交金额
	m_n150000Volume1 = refObj.m_n150000Volume1;			///< 第一笔快照的成交量
	m_n150000NumTrades1 = refObj.m_n150000NumTrades1;		///< 成交笔数
	m_fPriceRate = refObj.m_fPriceRate;			///< 放大比率
	m_bCalled0 = refObj.m_bCalled0;
	m_bCalled1 = refObj.m_bCalled1;
	m_bCalled2 = refObj.m_bCalled2;
	m_bCalled3 = refObj.m_bCalled3;
}

Minute60LineStatus::Minute60LineStatus( const SHSZ_60Minute_Lines& refObj )
{
	::memcpy( &m_tag1030Line, &(refObj.m_tag1030Line), sizeof(T_Minute_Line) );
	::memcpy( &m_tag1300Line, &(refObj.m_tag1300Line), sizeof(T_Minute_Line) );
	::memcpy( &m_tag1400Line, &(refObj.m_tag1400Line), sizeof(T_Minute_Line) );
	::memcpy( &m_tag1500Line, &(refObj.m_tag1500Line), sizeof(T_Minute_Line) );

	::memcpy( Code, refObj.Code, sizeof(Code) );
	m_cMkID = refObj.m_cMkID;					///< 市场ID
	m_nDate1 = refObj.m_nDate1;					///< 当前市场日期
	m_nTime1 = refObj.m_nTime1;					///< 当前市场时间
	m_f103000PricePx1 = refObj.m_f103000PricePx1;			///< 第一笔快照的现价
	m_f103000Amount1 = refObj.m_f103000Amount1;			///< 第一笔快照的成交金额
	m_n103000Volume1 = refObj.m_n103000Volume1;			///< 第一笔快照的成交量
	m_n103000NumTrades1 = refObj.m_n103000NumTrades1;		///< 成交笔数
	m_f130000PricePx1 = refObj.m_f130000PricePx1;			///< 第一笔快照的现价
	m_f130000Amount1 = refObj.m_f130000Amount1;			///< 第一笔快照的成交金额
	m_n130000Volume1 = refObj.m_n130000Volume1;			///< 第一笔快照的成交量
	m_n130000NumTrades1 = refObj.m_n130000NumTrades1;		///< 成交笔数
	m_f140000PricePx1 = refObj.m_f140000PricePx1;			///< 第一笔快照的现价
	m_f140000Amount1 = refObj.m_f140000Amount1;			///< 第一笔快照的成交金额
	m_n140000Volume1 = refObj.m_n140000Volume1;			///< 第一笔快照的成交量
	m_n140000NumTrades1 = refObj.m_n140000NumTrades1;		///< 成交笔数
	m_f150000PricePx1 = refObj.m_f150000PricePx1;			///< 第一笔快照的现价
	m_f150000Amount1 = refObj.m_f150000Amount1;			///< 第一笔快照的成交金额
	m_n150000Volume1 = refObj.m_n150000Volume1;			///< 第一笔快照的成交量
	m_n150000NumTrades1 = refObj.m_n150000NumTrades1;		///< 成交笔数
	m_fPriceRate = refObj.m_fPriceRate;			///< 放大比率
	m_bCalled0 = false;
	m_bCalled1 = false;
	m_bCalled2 = false;
	m_bCalled3 = false;
}

void Minute60LineStatus::SetRefParam( char cMkID, const char* pszCode, unsigned int nDate, unsigned int nTime, double dPriceRate )
{
	m_cMkID = cMkID;					///< 市场ID
	m_nDate1 = nDate;					///< 当前市场日期
	m_nTime1 = nTime;					///< 当前市场时间
	m_fPriceRate = dPriceRate;			///< 放大比率
	::memcpy( Code, pszCode, sizeof Code );
	m_bCalled0 = false;
	m_bCalled1 = false;
	m_bCalled2 = false;
	m_bCalled3 = false;
}

void Minute60LineStatus::DoQueryResponse( unsigned int nPeriodIndex, unsigned int nReqID, bool bIsLast )
{
	unsigned int		nDate= MDateTime::Now().DateToLong();

	if( 0 == nPeriodIndex && m_tag1030Line.Date == nDate && m_bCalled0 == true ) {
		Global_QueryClient.GetHandle()->OnNotifySyncMinuteLine( m_cMkID, &m_tag1030Line, NULL, nReqID, bIsLast );
	} else if( 1 == nPeriodIndex && m_tag1300Line.Date == nDate && m_bCalled1 == true ) {
		Global_QueryClient.GetHandle()->OnNotifySyncMinuteLine( m_cMkID, &m_tag1300Line, NULL, nReqID, bIsLast );
	} else if( 2 == nPeriodIndex && m_tag1400Line.Date == nDate && m_bCalled2 == true ) {
		Global_QueryClient.GetHandle()->OnNotifySyncMinuteLine( m_cMkID, &m_tag1400Line, NULL, nReqID, bIsLast );
	} else if( 3 == nPeriodIndex && m_tag1500Line.Date == nDate && m_bCalled3 == true ) {
		Global_QueryClient.GetHandle()->OnNotifySyncMinuteLine( m_cMkID, &m_tag1500Line, NULL, nReqID, bIsLast );
	}
}

void Minute60LineStatus::DoCallBack( unsigned int nPeriodIndex )
{
	unsigned int		nDate= MDateTime::Now().DateToLong();

	if( 0 == nPeriodIndex && m_tag1030Line.Date == nDate && m_bCalled0 == false ) {
		m_tag1030Line.Time = 103000;
		m_tag1030Line.Amount -= m_f103000Amount1;					///< 成交额一分钟最后笔减去第一笔的amount
		m_tag1030Line.Volume -= m_n103000Volume1;					///< 成交量(股/张/份)一分钟最后笔减去第一笔的volume
		m_tag1030Line.NumTrades -= m_n103000NumTrades1;			///< 成交笔数一分钟最后笔减去第一笔的numtrades
		::memcpy( m_tag1030Line.Code, Code, sizeof(Code) );
		m_bCalled0 = true;
		Global_QueryClient.GetHandle()->OnMarketMinuteLine( m_cMkID, &m_tag1030Line );
	} else if( 1 == nPeriodIndex && m_tag1300Line.Date == nDate && m_bCalled1 == false ) {
		m_tag1300Line.Time = 130000;
		m_tag1300Line.Amount -= m_f130000Amount1;					///< 成交额一分钟最后笔减去第一笔的amount
		m_tag1300Line.Volume -= m_n130000Volume1;					///< 成交量(股/张/份)一分钟最后笔减去第一笔的volume
		m_tag1300Line.NumTrades -= m_n130000NumTrades1;			///< 成交笔数一分钟最后笔减去第一笔的numtrades
		::memcpy( m_tag1300Line.Code, Code, sizeof(Code) );
		m_bCalled1 = true;
		Global_QueryClient.GetHandle()->OnMarketMinuteLine( m_cMkID, &m_tag1300Line );
	} else if( 2 == nPeriodIndex && m_tag1400Line.Date == nDate && m_bCalled2 == false ) {
		m_tag1400Line.Time = 140000;
		m_tag1400Line.Amount -= m_f140000Amount1;					///< 成交额一分钟最后笔减去第一笔的amount
		m_tag1400Line.Volume -= m_n140000Volume1;					///< 成交量(股/张/份)一分钟最后笔减去第一笔的volume
		m_tag1400Line.NumTrades -= m_n140000NumTrades1;			///< 成交笔数一分钟最后笔减去第一笔的numtrades
		::memcpy( m_tag1400Line.Code, Code, sizeof(Code) );
		m_bCalled2 = true;
		Global_QueryClient.GetHandle()->OnMarketMinuteLine( m_cMkID, &m_tag1400Line );
	} else if( 3 == nPeriodIndex && m_tag1500Line.Date == nDate && m_bCalled3 == false ) {
		m_tag1500Line.Time = 150000;
		m_tag1500Line.Amount -= m_f150000Amount1;					///< 成交额一分钟最后笔减去第一笔的amount
		m_tag1500Line.Volume -= m_n150000Volume1;					///< 成交量(股/张/份)一分钟最后笔减去第一笔的volume
		m_tag1500Line.NumTrades -= m_n150000NumTrades1;			///< 成交笔数一分钟最后笔减去第一笔的numtrades
		::memcpy( m_tag1500Line.Code, Code, sizeof(Code) );
		m_bCalled3 = true;
		Global_QueryClient.GetHandle()->OnMarketMinuteLine( m_cMkID, &m_tag1500Line );
	}
}

void Minute60LineStatus::Update( T_Minute_Line& refObj, bool bInit )
{
	if( /*refObj.Time >= 91000 && */refObj.Time < 103000 ) {
		if( refObj.Time >= 93000 && m_nTime1 < 93000 ) {
			m_nTime1 = refObj.Time;								///< 60分钟内第一笔快照的市场时间
		}

		if( m_nTime1 < 93000 || true == bInit ) {
			::memcpy( Code, refObj.Code, sizeof(Code) );
			m_nDate1 = refObj.Date;								///< 60分钟内第一笔快照的市场日期
			m_f103000PricePx1 = refObj.ClosePx;						///< 60分钟内第一笔快照的NowPx/ClosePx
			m_f103000Amount1 = refObj.Amount;							///< 60分钟内第一笔快照的成交金额
			m_n103000Volume1 = refObj.Volume;							///< 60分钟内第一笔快照的成交量
			m_n103000NumTrades1 = refObj.NumTrades;					///< 60分钟内第一笔快照的成交笔数
			m_tag1030Line.Time = refObj.Time;
			m_tag1030Line.OpenPx = refObj.ClosePx;
			m_tag1030Line.HighPx = refObj.ClosePx;
			m_tag1030Line.LowPx = refObj.ClosePx;
		}

		::memcpy( m_tag1030Line.Code, refObj.Code, sizeof(Code) );
		if( false == bInit && refObj.Time >= 93000 ) {
			m_tag1030Line.Date = refObj.Date;
		}
		m_tag1030Line.ClosePx = refObj.ClosePx;
		m_tag1030Line.Amount = refObj.Amount;
		m_tag1030Line.Volume = refObj.Volume;
		m_tag1030Line.NumTrades = refObj.NumTrades;
		m_tag1030Line.Voip = refObj.Voip;
		if( refObj.ClosePx > m_tag1030Line.HighPx ) {
			m_tag1030Line.HighPx = refObj.ClosePx;
		}

		if( refObj.ClosePx < m_tag1030Line.LowPx ) {
			m_tag1030Line.LowPx = refObj.ClosePx;
		}

	}

	if( refObj.Time >= 93000/*103000*/ && refObj.Time < 130000 ) {
		if( refObj.Time >= 103000 && m_nTime1 < 103000 ) {
			DoCallBack( 0 );
			m_nTime1 = refObj.Time;								///< 60分钟内第一笔快照的市场时间
		}

		if( m_nTime1 < 103000 || true == bInit ) {
			::memcpy( Code, refObj.Code, sizeof(Code) );
			m_nDate1 = refObj.Date;								///< 60分钟内第一笔快照的市场日期
			m_f130000PricePx1 = refObj.ClosePx;						///< 60分钟内第一笔快照的NowPx/ClosePx
			m_f130000Amount1 = refObj.Amount;							///< 60分钟内第一笔快照的成交金额
			m_n130000Volume1 = refObj.Volume;							///< 60分钟内第一笔快照的成交量
			m_n130000NumTrades1 = refObj.NumTrades;					///< 60分钟内第一笔快照的成交笔数
			m_tag1300Line.OpenPx = refObj.ClosePx;
			m_tag1300Line.HighPx = refObj.ClosePx;
			m_tag1300Line.LowPx = refObj.ClosePx;
		}

		if( false == bInit && refObj.Time >= 93000 ) {
			m_tag1300Line.Date = refObj.Date;
		}
		m_tag1300Line.ClosePx = refObj.ClosePx;
		m_tag1300Line.Amount = refObj.Amount;
		m_tag1300Line.Volume = refObj.Volume;
		m_tag1300Line.NumTrades = refObj.NumTrades;
		m_tag1300Line.Voip = refObj.Voip;
		if( refObj.ClosePx > m_tag1300Line.HighPx ) {
			m_tag1300Line.HighPx = refObj.ClosePx;
		}

		if( refObj.ClosePx < m_tag1300Line.LowPx ) {
			m_tag1300Line.LowPx = refObj.ClosePx;
		}

	}

	if( refObj.Time >= 103000/*130000*/ && refObj.Time < 140000 ) {
		if( refObj.Time >= 130000 && m_nTime1 < 130000 ) {
			DoCallBack( 1 );
			m_nTime1 = refObj.Time;								///< 60分钟内第一笔快照的市场时间
		}

		if( m_nTime1 < 130000 || true == bInit ) {
			::memcpy( Code, refObj.Code, sizeof(Code) );
			m_nDate1 = refObj.Date;								///< 60分钟内第一笔快照的市场日期
			m_f140000PricePx1 = refObj.ClosePx;						///< 60分钟内第一笔快照的NowPx/ClosePx
			m_f140000Amount1 = refObj.Amount;							///< 60分钟内第一笔快照的成交金额
			m_n140000Volume1 = refObj.Volume;							///< 60分钟内第一笔快照的成交量
			m_n140000NumTrades1 = refObj.NumTrades;					///< 60分钟内第一笔快照的成交笔数
			m_tag1400Line.OpenPx = refObj.ClosePx;
			m_tag1400Line.HighPx = refObj.ClosePx;
			m_tag1400Line.LowPx = refObj.ClosePx;
		}

		::memcpy( m_tag1400Line.Code, refObj.Code, sizeof(Code) );
		if( false == bInit && refObj.Time >= 93000 ) {
			m_tag1400Line.Date = refObj.Date;
		}
		m_tag1400Line.ClosePx = refObj.ClosePx;
		m_tag1400Line.Amount = refObj.Amount;
		m_tag1400Line.Volume = refObj.Volume;
		m_tag1400Line.NumTrades = refObj.NumTrades;
		m_tag1400Line.Voip = refObj.Voip;
		if( refObj.ClosePx > m_tag1400Line.HighPx ) {
			m_tag1400Line.HighPx = refObj.ClosePx;
		}

		if( refObj.ClosePx < m_tag1400Line.LowPx ) {
			m_tag1400Line.LowPx = refObj.ClosePx;
		}

	}

	if( refObj.Time >= 130000/*140000*/ && refObj.Time < 150000 ) {
		if( refObj.Time >= 140000 && m_nTime1 < 140000 ) {
			DoCallBack( 2 );
			m_nTime1 = refObj.Time;								///< 60分钟内第一笔快照的市场时间
		}

		if( m_nTime1 < 140000 || true == bInit ) {
			::memcpy( Code, refObj.Code, sizeof(Code) );
			m_nDate1 = refObj.Date;								///< 60分钟内第一笔快照的市场日期
			m_f150000PricePx1 = refObj.ClosePx;						///< 60分钟内第一笔快照的NowPx/ClosePx
			m_f150000Amount1 = refObj.Amount;							///< 60分钟内第一笔快照的成交金额
			m_n150000Volume1 = refObj.Volume;							///< 60分钟内第一笔快照的成交量
			m_n150000NumTrades1 = refObj.NumTrades;					///< 60分钟内第一笔快照的成交笔数
			m_tag1500Line.OpenPx = refObj.ClosePx;
			m_tag1500Line.HighPx = refObj.ClosePx;
			m_tag1500Line.LowPx = refObj.ClosePx;
		}

		::memcpy( m_tag1500Line.Code, refObj.Code, sizeof(Code) );
		if( false == bInit && refObj.Time >= 93000 ) {
			m_tag1500Line.Date = refObj.Date;
		}
		m_tag1500Line.ClosePx = refObj.ClosePx;
		m_tag1500Line.Amount = refObj.Amount;
		m_tag1500Line.Volume = refObj.Volume;
		m_tag1500Line.NumTrades = refObj.NumTrades;
		m_tag1500Line.Voip = refObj.Voip;
		if( refObj.ClosePx > m_tag1500Line.HighPx ) {
			m_tag1500Line.HighPx = refObj.ClosePx;
		}

		if( refObj.ClosePx < m_tag1500Line.LowPx ) {
			m_tag1500Line.LowPx = refObj.ClosePx;
		}
	}
}

void Minute60LineStatus::Update( unsigned int nDate, unsigned int nMkTime, tagCcComm_IndexData& refData )
{
	T_Minute_Line				tagLine = { 0 };

	tagLine.Date = nDate;
	tagLine.Time = nMkTime;
	::memcpy( tagLine.Code, refData.Code, sizeof(refData.Code) );
	tagLine.OpenPx = refData.Open / m_fPriceRate;
	tagLine.HighPx = refData.High / m_fPriceRate;
	tagLine.LowPx = refData.Low / m_fPriceRate;
	tagLine.ClosePx = refData.Now / m_fPriceRate;
	tagLine.Amount = refData.Amount;
	tagLine.Volume = refData.Volume;

	Update( tagLine );
}

void Minute60LineStatus::Update( unsigned int nDate, unsigned int nMkTime, tagCcComm_StockData5& refData )
{
	T_Minute_Line				tagLine = { 0 };

	tagLine.Date = nDate;
	tagLine.Time = nMkTime;
	::memcpy( tagLine.Code, refData.Code, sizeof(refData.Code) );
	tagLine.OpenPx = refData.Open / m_fPriceRate;
	tagLine.HighPx = refData.High / m_fPriceRate;
	tagLine.LowPx = refData.Low / m_fPriceRate;
	tagLine.ClosePx = refData.Now / m_fPriceRate;
	tagLine.Voip = refData.Voip / m_fPriceRate;
	tagLine.Amount = refData.Amount;
	tagLine.Volume = refData.Volume;
	tagLine.NumTrades = refData.Records;

	Update( tagLine );
}

///< ----------------------------- 1分钟线 --------------------------------------------

Min1Sync::Min1Sync( enum XDFMarket eMkID )
: m_eMarketID( eMkID ), m_bSyned( false )
{
}

bool Min1Sync::Sync()
{
	if( false == m_oSyncThread.rbl_GetRunState() )
	{
		m_bSyned = false;
		if( 0 > m_oSyncThread.StartThread( "QuoteQueryClient::Min1Sync::Sync()", SyncThread, this ) ) {
			Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "Min1Sync::Sync() : failed 2 create minute line sync thread" );
			return false;
		}
	}

	return true;
}

bool Min1Sync::IsLoaded()
{
	return m_bSyned;
}

void* __stdcall Min1Sync::SyncThread( void* pSelf )
{
	SHELLEXECUTEINFO	tagExeInfo = { 0 };
	Min1Sync&			refSync = *((Min1Sync*)pSelf);
	std::string			sCmd = "/C .\\client4shrealmin1.bat";

	Global_LogUnit.WriteLogEx( 0, 0, "QuoteQueryClient", "Min1Sync::SyncThread() : MarketID = %d, enter...................", (int)(refSync.m_eMarketID) );

	unsigned int		nLoopTimes = 60 * Global_Option.GetSyncIntervalMin();
	for( int n = 0; n < nLoopTimes && refSync.m_oSyncThread.GetThreadStopFlag() == false; n++ ) {
		MThread::Sleep( 1000 );
	}

	if( true == refSync.m_oSyncThread.GetThreadStopFlag() ) {
		return NULL;
	}

	Global_LogUnit.WriteLogEx( 0, 0, "QuoteQueryClient", "Min1Sync::SyncThread() : MarketID = %d, sync...................", (int)(refSync.m_eMarketID) );
	if( refSync.m_eMarketID == XDF_SZ ) {
		sCmd = "/C .\\client4szrealmin1.bat";
	}

	refSync.m_bSyned = false;
	tagExeInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	tagExeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	tagExeInfo.hwnd = NULL;
	tagExeInfo.lpVerb = NULL;
	tagExeInfo.lpFile = "cmd";
	tagExeInfo.lpParameters = sCmd.c_str();
	tagExeInfo.lpDirectory = Global_Option.GetSyncExeFolder();
	tagExeInfo.nShow = SW_HIDE;
	tagExeInfo.hInstApp = NULL;

	if( !::ShellExecuteEx( &tagExeInfo ) ) {
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "Min1Sync::SyncThread() : MarketID = %d, Workpath = %s, Terminated!!!", (int)(refSync.m_eMarketID), Global_Option.GetSyncExeFolder() );
		return NULL;
	}

	if( WAIT_TIMEOUT == ::WaitForSingleObject( tagExeInfo.hProcess, 1000 * 60 * 10 ) ) {
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "Min1Sync::SyncThread() : MarketID = %d, Workpath = %s, Terminated (overtime)!!!", (int)(refSync.m_eMarketID), Global_Option.GetSyncExeFolder() );
		return NULL;
	}

	refSync.m_bSyned = true;
	Global_LogUnit.WriteLogEx( 0, 0, "QuoteQueryClient", "Min1Sync::SyncThread() : MarketID = %d, leave...", (int)(refSync.m_eMarketID) );

	return NULL;
}

MinGenerator::MinGenerator()
 : m_nDate( 0 ), m_nMaxLineCount( 241 ), m_pDataCache( NULL ), m_dPriceRate( 0 )
 , m_nWriteSize( -1 ), m_nDataSize( 0 ), m_dAmountBefore930( 0. ), m_nVolumeBefore930( 0 ), m_nNumTradesBefore930( 0 )
 , m_dAmountOfRecover( 0. ), m_nVolumeOfRecover( 0 ), m_nNumTradesOfRecover( 0 )
{
	::memset( m_pszCode, 0, sizeof(m_pszCode) );
}

MinGenerator::MinGenerator( enum XDFMarket eMkID, unsigned int nDate, const std::string& sCode, double dPriceRate, T_DATA& objData, T_DATA* pBufPtr )
: m_nMaxLineCount( 241 ), m_nWriteSize( -1 ), m_nDataSize( 0 ), m_dAmountBefore930( 0. ), m_nVolumeBefore930( 0 ), m_nNumTradesBefore930( 0 )
, m_dAmountOfRecover( 0. ), m_nVolumeOfRecover( 0 ), m_nNumTradesOfRecover( 0 )
{
	m_eMarket = eMkID;
	m_nDate = nDate;
	::strcpy( m_pszCode, sCode.c_str() );
	m_dPriceRate = dPriceRate;
	m_pDataCache = pBufPtr;
	::memset( m_pDataCache, 0, sizeof(T_DATA) * m_nMaxLineCount );
}

MinGenerator& MinGenerator::operator=( const MinGenerator& obj )
{
	m_eMarket = obj.m_eMarket;
	m_nDate = obj.m_nDate;
	::strcpy( m_pszCode, obj.m_pszCode );

	m_dPriceRate = obj.m_dPriceRate;
	m_dAmountBefore930 = obj.m_dAmountBefore930;
	m_nVolumeBefore930 = obj.m_nVolumeBefore930;
	m_nNumTradesBefore930 = obj.m_nNumTradesBefore930;

	m_dAmountOfRecover = obj.m_dAmountOfRecover;
	m_nVolumeOfRecover = obj.m_nVolumeOfRecover;
	m_nNumTradesOfRecover = obj.m_nNumTradesOfRecover;

	m_pDataCache = obj.m_pDataCache;
	m_nWriteSize = obj.m_nWriteSize;
	m_nDataSize = obj.m_nDataSize;

	return *this;
}

int MinGenerator::Initialize()
{
	m_nWriteSize = -1;
	m_nDataSize = 0;

	m_dAmountOfRecover = 0.;
	m_nVolumeOfRecover = 0;
	m_nNumTradesOfRecover = 0;

	return 0;
}

double MinGenerator::GetPxRate()
{
	return m_dPriceRate;
}

static bool s_bCloseMarket = false;

int MinGenerator::Update( T_DATA& objData )
{
	if( NULL == m_pDataCache )
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MinGenerator::Update() : invalid buffer pointer" );
		return -1;
	}

	if( 0 == objData.Time )
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MinGenerator::Update() : invalid snap time" );
		return -2;
	}

	unsigned int		nMKTime = objData.Time;
	unsigned int		nHH = nMKTime / 10000;
	unsigned int		nMM = nMKTime % 10000 / 100;
	int					nDataIndex = -1;

	if( nMKTime >= 150000 ) {
		s_bCloseMarket = true;		///< 如果有商品的市场时间为15:00，则标记为需要集体生成分钟线
	}

	if( nMKTime < 93000 ) {
		m_dAmountBefore930 = objData.Amount;		///< 9:30前的金额
		m_nVolumeBefore930 = objData.Volume;		///< 9:30前的量
		m_nNumTradesBefore930 = objData.NumTrades;	///< 9:30前的笔数
	} else if( nMKTime >= 93000 && nMKTime <= 130000 ) {
		nDataIndex = 0;								///< 需要包含9:30这根线
		if( 9 == nHH ) {
			nDataIndex += (nMM - 30);				///< 9:30~9:59 = 30根
		} else if( 10 == nHH ) {
			nDataIndex += (30 + nMM);				///< 10:00~10:59 = 60根
		} else if( 11 == nHH ) {
			nDataIndex += (90 + nMM);				///< 11:00~11:30 = 31根
		} else if( 13 == nHH ) {
			nDataIndex += 120;						///< 13:01(即13:00)的数据存放到11:30的位置
		}
	} else if( nMKTime > 130000 && nMKTime <= 150000 ) {
		nDataIndex = 120;							///< 上午共121根
		if( 13 == nHH ) {
			nDataIndex += nMM;						///< 13:01~13:59 = 59根
		} else if( 14 == nHH ) {
			nDataIndex += (60 + nMM);				///< 14:00~14:59 = 60根
		} else if( 15 == nHH ) {
			nDataIndex += 120;						///< 15:00~15:00 = 1根
		}
	}

	if( nDataIndex < 0 ) {
		return 0;
	}

	if( nDataIndex >= 241 ) {
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MinGenerator::Update() : out of range" );
		return -4;
	}

	T_DATA*		pData = m_pDataCache + nDataIndex;

	pData->Amount = objData.Amount;
	pData->Volume = objData.Volume;
	pData->NumTrades = objData.NumTrades;
	pData->Voip = objData.Voip;
	pData->ClosePx = objData.ClosePx;
	if( pData->Time == 0 ) {						///< 该1分钟线位置结构的初始化，开、高、低、收都等于收盘价
		pData->Time = objData.Time;
		pData->OpenPx = objData.ClosePx;
		pData->HighPx = objData.ClosePx;
		pData->LowPx = objData.ClosePx;
	} else {										///< 该1分钟线位置的后续更新，调整最高、最低价
		pData->Time = objData.Time;
		if( objData.ClosePx > pData->HighPx ) {
			pData->HighPx = objData.ClosePx;
		}
		if( objData.ClosePx < pData->LowPx ) {
			pData->LowPx = objData.ClosePx;
		}
	}

	if( nDataIndex > m_nDataSize ) {
		m_nDataSize = nDataIndex;
	}

	return 0;
}

bool MinGenerator::AssignMin1( T_DATA& objData )
{
	if( NULL == m_pDataCache )
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MinGenerator::AssignMin1() : invalid buffer pointer" );
		return false;
	}

	if( 0 == objData.Time )
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MinGenerator::AssignMin1() : invalid snap time" );
		return false;
	}

	unsigned int		nNowT = MDateTime::Now().TimeToLong();
	unsigned int		nMKTime = objData.Time;
	unsigned int		nHH = nMKTime / 10000;
	unsigned int		nMM = nMKTime % 10000 / 100;
	int					nDataIndex = -1;

	if( nNowT >= 150000 )	{
		m_nWriteSize = 241;
	} else if( nNowT > 113000 && nNowT < 130000 )	{
		m_nWriteSize = 120;
	}

	if( nMKTime >= 93000 && nMKTime < 130000 ) {
		nDataIndex = 0;								///< 需要包含9:30这根线
		if( 9 == nHH ) {
			nDataIndex += (nMM - 30);				///< 9:30~9:59 = 30根
		} else if( 10 == nHH ) {
			nDataIndex += (30 + nMM);				///< 10:00~10:59 = 60根
		} else if( 11 == nHH ) {
			nDataIndex += (90 + nMM);				///< 11:00~11:30 = 31根
		}
	} else if( nMKTime > 130000 && nMKTime <= 150000 ) {
		nDataIndex = 120;							///< 上午共121根
		if( 13 == nHH ) {
			nDataIndex += nMM;						///< 13:01~13:59 = 59根
		} else if( 14 == nHH ) {
			nDataIndex += (60 + nMM);				///< 14:00~14:59 = 60根
		} else if( 15 == nHH ) {
			nDataIndex += 120;						///< 15:00~15:00 = 1根
		}
	}

	if( nDataIndex > m_nWriteSize ) {				///< 从同步文件恢复只恢复到当前待生成实时min1线的位置
		return false;
	}

	if( nDataIndex >= 241 || nDataIndex < 0 ) {
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MinGenerator::AssignMin1() : out of range, index = %d", nDataIndex );
		return false;
	}

	if( 0 == nDataIndex ) {	///< 9:30这个节点的处理, (原样copy),  直接把server端已经计算好的一分钟线数据，对号入座写入相应的位置
		T_DATA*		pData = m_pDataCache + nDataIndex;

		::memcpy( pData, &objData, sizeof(T_DATA) );
		///< 9:30的1分钟线的量是全量（不是差量）
		m_dAmountOfRecover = objData.Amount;
		m_nVolumeOfRecover = objData.Volume;
		m_nNumTradesOfRecover = objData.NumTrades;
	} else { ///< 9:31分以后的所有节点的处理, 后一分钟线，决定前一线钟线，和前前值之间的差值
		if( 1 == nDataIndex ) {
			///< 为计算第二笔，准备好30分前的最后一笔的价格状态
			m_dAmountBefore930 = m_dAmountOfRecover - objData.Amount;
			m_nVolumeBefore930 = m_nVolumeOfRecover - objData.Volume;
			m_nNumTradesBefore930 = m_nNumTradesOfRecover - objData.NumTrades;
			m_dAmountOfRecover = m_dAmountBefore930 + objData.Amount;
			m_nVolumeOfRecover = m_nVolumeBefore930 + objData.Volume;
			m_nNumTradesOfRecover = m_nNumTradesBefore930 + objData.NumTrades;
		} else {
			m_dAmountOfRecover += objData.Amount;
			m_nVolumeOfRecover += objData.Volume;
			m_nNumTradesOfRecover += objData.NumTrades;
		}

		if( nDataIndex > 1 ) {
			T_DATA*		pPreData = m_pDataCache + (nDataIndex-1);
			///< 把1分线数据的价格值，赋值到前一分钟上(因为当前一分钟价格计算自前一分钟）
			::memcpy( pPreData, &objData, sizeof(T_DATA) );

			pPreData->Amount = m_dAmountOfRecover;
			pPreData->Volume = m_nVolumeOfRecover;
			pPreData->NumTrades = m_nNumTradesOfRecover;

		}
	}

	return true;
}

int MinGenerator::CallBack4Query( int nReqID, unsigned int nBeginTime, unsigned int nEndTime, bool bIsLastCode )
{
	bool					bIsLastCB = false;				///< 是否为最后一笔回调
	unsigned int			nLastLineIndex = 0;				///< 上一笔快照是索引值
	T_MIN_LINE				tagLastLine = { 0 };			///< 上一笔快照的最后情况
	T_MIN_LINE				tagLastLastLine = { 0 };		///< 上上笔快照的最后情况

	if( NULL == m_pDataCache ) {
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MinGenerator::CallBack4Query() : invalid buffer pointer 4 code:%s", m_pszCode );
		return -1;
	}

	///< 从头遍历，直到最后一个收到的时间节点上
	for( int i = 0; i <= m_nWriteSize; i++ )
	{
		T_Minute_Line		tagMinuteLine = { 0 };

		if( true == bIsLastCode && i == m_nWriteSize ) {	bIsLastCB = true;	}
		///< 以m_pDataCache[i].Time大于零为标识，进行"后续查询回调"
		::memcpy( tagMinuteLine.Code, m_pszCode, 6 );
		tagMinuteLine.Date = m_nDate;
		if( i == 0 ) {											///< [ 上午121根分钟线，下午120根 ]
			tagMinuteLine.Time = 93000;							///< 9:30~9:30 = 1根 (i:0)
		} else if( i >= 1 && i <= 29 ) {
			tagMinuteLine.Time = (930 + i) * 100;				///< 9:31~9:59 = 29根 (i:1--29)
		} else if( i >= 30 && i <= 89 ) {
			tagMinuteLine.Time = (1000 + (i-30)) * 100;			///< 10:00~10:59 = 60根 (i:30--89)
		} else if( i >= 90 && i <= 120 ) {
			tagMinuteLine.Time = (1100 + (i-90)) * 100;			///< 11:00~11:30 = 31根 (i:90--120)
		} else if( i > 120 && i <= 179 ) {
			tagMinuteLine.Time = (1300 + (i-120)) * 100;		///< 13:01~13:59 = 59根 (i:121--179)
		} else if( i > 179 && i <= 239 ) {
			tagMinuteLine.Time = (1400 + (i-180)) * 100;		///< 14:00~14:59 = 60根 (i:180--239)
		} else if( i == 240 ) {
			tagMinuteLine.Time = 150000;						///< 15:00~15:00 = 1根 (i:240)
		}

		if( 0 == i ) {	////////////////////////< 第一个节点是9:30，此时只需要将9:30分的第一个快照回调
			tagMinuteLine.Amount = m_pDataCache[i].Amount;
			tagMinuteLine.Volume = m_pDataCache[i].Volume;
			tagMinuteLine.NumTrades = m_pDataCache[i].NumTrades;
			tagMinuteLine.OpenPx = m_pDataCache[i].OpenPx / m_dPriceRate;
			tagMinuteLine.HighPx = m_pDataCache[i].HighPx / m_dPriceRate;
			tagMinuteLine.LowPx = m_pDataCache[i].LowPx / m_dPriceRate;
			tagMinuteLine.ClosePx = m_pDataCache[i].ClosePx / m_dPriceRate;
			tagMinuteLine.Voip = m_pDataCache[i].Voip / m_dPriceRate;
			if( m_pDataCache[i].Time > 0 ) {
				Global_QueryClient.GetHandle()->OnNotifySyncMinuteLine( (char)m_eMarket, &tagMinuteLine, NULL, nReqID, bIsLastCB );
			}
		} else {		////////////////////////< 处理9:30后的分钟线计算与回调	 [1. 前面无成交的情况 2.前面是连续成交的情况]
			if( i - nLastLineIndex > 1 ) {	///< 如果前面n分钟内无成交，则开盘最高最低等于ClosePx
				tagMinuteLine.OpenPx = tagLastLine.ClosePx;
				tagMinuteLine.HighPx = tagLastLine.ClosePx;
				tagMinuteLine.LowPx = tagLastLine.ClosePx;
				tagMinuteLine.ClosePx = tagLastLine.ClosePx;
				tagMinuteLine.Voip = tagLastLine.Voip;
			} else {						///< 最近一分钟内有连续成交
				tagMinuteLine.OpenPx = tagLastLine.OpenPx;
				tagMinuteLine.HighPx = tagLastLine.HighPx;
				tagMinuteLine.LowPx = tagLastLine.LowPx;
				tagMinuteLine.ClosePx = tagLastLine.ClosePx;
				tagMinuteLine.Voip = tagLastLine.Voip;
				tagMinuteLine.Amount = tagLastLine.Amount - tagLastLastLine.Amount;
				tagMinuteLine.Volume = tagLastLine.Volume - tagLastLastLine.Volume;
				tagMinuteLine.NumTrades = tagLastLine.NumTrades - tagLastLastLine.NumTrades;
			}
			if( m_pDataCache[i-1].Time > 0 ) {
				Global_QueryClient.GetHandle()->OnNotifySyncMinuteLine( (char)m_eMarket, &tagMinuteLine, NULL, nReqID, bIsLastCB );
				m_pDataCache[i-1].Time = 0;
			}
		}

		///< 记录： 本次的金额，量等信息，供用于后一笔的差值计算
		if( m_pDataCache[i].Volume > 0 ) {
			if( i == 0 ) {	///< 将9:30前最后一记赋值，用于计算9:31那笔的差值
				tagLastLastLine.Amount = m_dAmountBefore930;
				tagLastLastLine.Volume = m_nVolumeBefore930;
				tagLastLastLine.NumTrades = m_nNumTradesBefore930;
			} else {		///< 用于计算9:31以后的分钟线差值
				::memcpy( &tagLastLastLine, &tagLastLine, sizeof tagLastLine );
			}
			///< 记录上一次快照的最后状态值
			nLastLineIndex = i;
			tagLastLine.Amount = m_pDataCache[i].Amount;
			tagLastLine.Volume = m_pDataCache[i].Volume;
			tagLastLine.NumTrades = m_pDataCache[i].NumTrades;
			tagLastLine.OpenPx = m_pDataCache[i].OpenPx / m_dPriceRate;
			tagLastLine.HighPx = m_pDataCache[i].HighPx / m_dPriceRate;
			tagLastLine.LowPx = m_pDataCache[i].LowPx / m_dPriceRate;
			tagLastLine.ClosePx = m_pDataCache[i].ClosePx / m_dPriceRate;
			tagLastLine.Voip = m_pDataCache[i].Voip / m_dPriceRate;
		}
	}

	return 0;
}

void MinGenerator::DumpMinutes()
{
	unsigned int			nLastLineIndex = 0;						///< 上一笔快照是索引值
	T_MIN_LINE				tagLastLine = { 0 };					///< 上一笔快照的最后情况
	T_MIN_LINE				tagLastLastLine = { 0 };				///< 上上笔快照的最后情况

	if( true == s_bCloseMarket ) m_nDataSize = m_nMaxLineCount;		///< 盘后，需要回调全部数据
	if( NULL == m_pDataCache ) {
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MinGenerator::DumpMinutes() : invalid buffer pointer 4 code:%s", m_pszCode );
		return;
	}

	///< 从头遍历，直到最后一个收到的时间节点上
	for( int i = 0; i < m_nDataSize; i++ )
	{
		///< 跳过已经回调过的时间节点(i > m_nWriteSize)，以m_pDataCache[i].Time大于零为标识，进行"后续写入"
		if( i > m_nWriteSize ) {
			T_Minute_Line		tagMinuteLine = { 0 };

			::memcpy( tagMinuteLine.Code, m_pszCode, 6 );
			tagMinuteLine.Date = m_nDate;
			if( i == 0 ) {											///< [ 上午121根分钟线，下午120根 ]
				tagMinuteLine.Time = 93000;							///< 9:30~9:30 = 1根 (i:0)
			} else if( i >= 1 && i <= 29 ) {
				tagMinuteLine.Time = (930 + i) * 100;				///< 9:31~9:59 = 29根 (i:1--29)
			} else if( i >= 30 && i <= 89 ) {
				tagMinuteLine.Time = (1000 + (i-30)) * 100;			///< 10:00~10:59 = 60根 (i:30--89)
			} else if( i >= 90 && i <= 120 ) {
				tagMinuteLine.Time = (1100 + (i-90)) * 100;			///< 11:00~11:30 = 31根 (i:90--120)
			} else if( i > 120 && i <= 179 ) {
				tagMinuteLine.Time = (1300 + (i-120)) * 100;		///< 13:01~13:59 = 59根 (i:121--179)
			} else if( i > 179 && i <= 239 ) {
				tagMinuteLine.Time = (1400 + (i-180)) * 100;		///< 14:00~14:59 = 60根 (i:180--239)
			} else if( i == 240 ) {
				tagMinuteLine.Time = 150000;						///< 15:00~15:00 = 1根 (i:240)
			}

			if( 0 == i ) {	////////////////////////< 第一个节点是9:30，此时只需要将9:30分的第一个快照回调
				tagMinuteLine.Amount = m_pDataCache[i].Amount;
				tagMinuteLine.Volume = m_pDataCache[i].Volume;
				tagMinuteLine.NumTrades = m_pDataCache[i].NumTrades;
				tagMinuteLine.OpenPx = m_pDataCache[i].OpenPx / m_dPriceRate;
				tagMinuteLine.HighPx = m_pDataCache[i].HighPx / m_dPriceRate;
				tagMinuteLine.LowPx = m_pDataCache[i].LowPx / m_dPriceRate;
				tagMinuteLine.ClosePx = m_pDataCache[i].ClosePx / m_dPriceRate;
				tagMinuteLine.Voip = m_pDataCache[i].Voip / m_dPriceRate;
				if( m_pDataCache[i].Time > 0 ) {
					Global_QueryClient.GetHandle()->OnMarketMinuteLine( m_eMarket, &tagMinuteLine );
					m_pDataCache[i].Time = 0;
				}

				m_nWriteSize = i;									///< 更新最新的写盘数据位置
			} else {		////////////////////////< 处理9:30后的分钟线计算与回调	 [1. 前面无成交的情况 2.前面是连续成交的情况]
				if( i - nLastLineIndex > 1 ) {	///< 如果前面n分钟内无成交，则开盘最高最低等于ClosePx
					tagMinuteLine.OpenPx = tagLastLine.ClosePx;
					tagMinuteLine.HighPx = tagLastLine.ClosePx;
					tagMinuteLine.LowPx = tagLastLine.ClosePx;
					tagMinuteLine.ClosePx = tagLastLine.ClosePx;
					tagMinuteLine.Voip = tagLastLine.Voip;
				} else {						///< 最近一分钟内有连续成交
					tagMinuteLine.OpenPx = tagLastLine.OpenPx;
					tagMinuteLine.HighPx = tagLastLine.HighPx;
					tagMinuteLine.LowPx = tagLastLine.LowPx;
					tagMinuteLine.ClosePx = tagLastLine.ClosePx;
					tagMinuteLine.Voip = tagLastLine.Voip;
					tagMinuteLine.Amount = tagLastLine.Amount - tagLastLastLine.Amount;
					tagMinuteLine.Volume = tagLastLine.Volume - tagLastLastLine.Volume;
					tagMinuteLine.NumTrades = tagLastLine.NumTrades - tagLastLastLine.NumTrades;
				}

				if( m_pDataCache[i].Time > 0 ) {
					Global_QueryClient.GetHandle()->OnMarketMinuteLine( m_eMarket, &tagMinuteLine );
					m_pDataCache[i].Time = 0;
				}

				m_nWriteSize = i;										///< 更新最新的写盘数据位置
			}
		}

		///< 记录： 本次的金额，量等信息，供用于后一笔的差值计算
		if( m_pDataCache[i].Volume > 0 ) {
			if( i == 0 ) {	///< 将9:30前最后一记赋值，用于计算9:31那笔的差值
				tagLastLastLine.Amount = m_dAmountBefore930;
				tagLastLastLine.Volume = m_nVolumeBefore930;
				tagLastLastLine.NumTrades = m_nNumTradesBefore930;
			} else {		///< 用于计算9:31以后的分钟线差值
				::memcpy( &tagLastLastLine, &tagLastLine, sizeof tagLastLine );
			}
			///< 记录上一次快照的最后状态值
			nLastLineIndex = i;
			tagLastLine.Amount = m_pDataCache[i].Amount;
			tagLastLine.Volume = m_pDataCache[i].Volume;
			tagLastLine.NumTrades = m_pDataCache[i].NumTrades;
			tagLastLine.OpenPx = m_pDataCache[i].OpenPx / m_dPriceRate;
			tagLastLine.HighPx = m_pDataCache[i].HighPx / m_dPriceRate;
			tagLastLine.LowPx = m_pDataCache[i].LowPx / m_dPriceRate;
			tagLastLine.ClosePx = m_pDataCache[i].ClosePx / m_dPriceRate;
			tagLastLine.Voip = m_pDataCache[i].Voip / m_dPriceRate;
		}
	}
}

SecurityMinCache::SecurityMinCache( enum XDFMarket eMkID, MQueryClient* pQueryClientApi )
 : m_nSecurityCount( 0 ), m_pMinDataTable( NULL ), m_nAlloPos( 0 ), m_eMarketID( eMkID )
 , m_objSyncMin1( eMkID ), m_bSyncDataLoaded( false ), m_pQueryClientApi( pQueryClientApi )
{
	m_vctCode.clear();
	m_objMapMinutes.clear();
}

SecurityMinCache::~SecurityMinCache()
{
}

int SecurityMinCache::Initialize( unsigned int nSecurityCount )
{
	unsigned int	nTotalCount = (1+nSecurityCount) * 241;

	Release();
	if( NULL == (m_pMinDataTable = new MinGenerator::T_DATA[nTotalCount]) )
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "SecurityMinCache::Initialize() : cannot allocate minutes buffer ( count:%u )", nTotalCount );
		return -1;
	}

	m_nAlloPos = 0;
	m_nSecurityCount = nTotalCount;
	::memset( m_pMinDataTable, 0, sizeof(MinGenerator::T_DATA) * nTotalCount );
	m_vctCode.reserve( nSecurityCount + 32 );

	return 0;
}

void SecurityMinCache::Release()
{
	if( NULL != m_pMinDataTable )
	{
		m_oRealCbAndLoadThread.StopThread();

		MLocalSection			section( &m_oLockData );

		delete [] m_pMinDataTable;
		m_pMinDataTable = NULL;
		m_objMapMinutes.clear();
		m_vctCode.clear();
	}

	m_nAlloPos = 0;
	m_nSecurityCount = 0;
	m_sLastValidCode = "";
	s_bCloseMarket = false;
	m_bSyncDataLoaded = false;
}

void SecurityMinCache::ActivateDumper()
{
	if( false == m_oRealCbAndLoadThread.rbl_GetRunState() )
	{
		m_bSyncDataLoaded = false;
		if( 0 > m_oRealCbAndLoadThread.StartThread( "SecurityMinCache::ActivateDumper()", RealCallbackAndLoadThread, this ) ) {
			Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "SecurityMinCache::ActivateDumper() : failed 2 create minute line thread(1)" );
		} else {
			m_objSyncMin1.Sync();
		}
	}
}

void SecurityMinCache::StopDumper()
{
	if( true == m_oRealCbAndLoadThread.rbl_GetRunState() ) {
		Global_LogUnit.WriteLogEx( 0, 0, "QuoteQueryClient", "SecurityMinCache::StopDumper() : MarketID = %d, stopping ...", (int)m_eMarketID );
		m_bSyncDataLoaded = false;
		m_oRealCbAndLoadThread.StopThread();
		Global_LogUnit.WriteLogEx( 0, 0, "QuoteQueryClient", "SecurityMinCache::StopDumper() : MarketID = %d, stopped ...", (int)m_eMarketID );
	}
}

int SecurityMinCache::NewSecurity( enum XDFMarket eMarket, const std::string& sCode, unsigned int nDate, double dPriceRate, MinGenerator::T_DATA& objData )
{
	MLocalSection			section( &m_oLockData );

	if( (m_nAlloPos+1) >= m_nSecurityCount )
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "SecurityMinCache::NewSecurity() : cannot grap any section from buffer ( %u:%u )", m_nAlloPos, m_nSecurityCount );
		return -1;
	}

	if( NULL == m_pMinDataTable )
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "SecurityMinCache::NewSecurity() : invalid buffer pointer" );
		return -2;
	}

	bool					bValidCode = false;
	unsigned int			nCode = ::atoi( sCode.c_str() );

	if( XDF_SH == m_eMarketID ) {
		if( (nCode>=1 && nCode<=999) || (nCode>=600000&&nCode<=609999) || (nCode>=510000&&nCode<=519999) ) {
			bValidCode = true;
		}
	} else if( XDF_SZ == m_eMarketID ) {
		if( (nCode>=399000 && nCode<=399999) || (nCode>=1&&nCode<=9999) || (nCode>=159000&&nCode<=159999) || (nCode>=300000&&nCode<=300999) ) {
			bValidCode = true;
		}
	}

	if( false == bValidCode ) {
		return 0;
	}

	m_objMapMinutes[sCode] = MinGenerator( eMarket, nDate, sCode, dPriceRate, objData, m_pMinDataTable + m_nAlloPos );
	if( 0 != m_objMapMinutes[sCode].Initialize() )
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "SecurityMinCache::NewSecurity() : cannot initialize tick generator class 4 code : %s", sCode.c_str() );
		return -3;
	}

	m_nAlloPos += 241;
	m_vctCode.push_back( sCode );
	m_sLastValidCode = sCode;

	return 0;
}

int SecurityMinCache::UpdateSecurity( const tagCcComm_IndexData& refObj, unsigned int nDate, unsigned int nTime )
{
	std::string					sCode( refObj.Code, 6 );
	MLocalSection				section( &m_oLockData );
	T_MAP_MINUTES::iterator		it = m_objMapMinutes.find( sCode );

	if( it != m_objMapMinutes.end() )
	{
		MinGenerator::T_DATA	objData = { 0 };

		objData.Time = nTime;
		objData.OpenPx = refObj.Open;
		objData.HighPx = refObj.High;
		objData.LowPx = refObj.Low;
		objData.ClosePx = refObj.Now;
		objData.Amount = refObj.Amount;
		objData.Volume = refObj.Volume;

		return it->second.Update( objData );
	}

	return -1;
}

int SecurityMinCache::UpdateSecurity( const tagCcComm_StockData5& refObj, unsigned int nDate, unsigned int nTime )
{
	std::string					sCode( refObj.Code, 6 );
	MLocalSection				section( &m_oLockData );
	T_MAP_MINUTES::iterator		it = m_objMapMinutes.find( sCode );

	if( it != m_objMapMinutes.end() )
	{
		MinGenerator::T_DATA		objData = { 0 };

		objData.Time = nTime;
		objData.OpenPx = refObj.Open;
		objData.HighPx = refObj.High;
		objData.LowPx = refObj.Low;
		objData.ClosePx = refObj.Now;
		objData.Amount = refObj.Amount;
		objData.Volume = refObj.Volume;
		objData.NumTrades = refObj.Records;
		objData.Voip = refObj.Voip;

		return it->second.Update( objData );
	}

	return -1;
}

int SecurityMinCache::QueryRecords( int nReqID, unsigned int nBeginTime, unsigned int nEndTime )
{
	unsigned int		nCallBackNum = 0;					///< 回调实际商品数
	bool				bIsLastCode = false;				///< 最否为最后一个回调的商品
	unsigned int		nCodeNumber = m_vctCode.size();		///< 代码长度

	for( unsigned int n = 0; n < nCodeNumber && false == m_oRealCbAndLoadThread.GetThreadStopFlag(); n++ )
	{
		T_MAP_MINUTES::iterator	it;

		{
			MLocalSection			section( &m_oLockData );
			it = m_objMapMinutes.find( m_vctCode[n] );
			if( it == m_objMapMinutes.end() ) {
				continue;
			}
		}

		if( it->first == m_sLastValidCode ) {
			bIsLastCode = true;
		}

		unsigned int				nCode = ::atoi( it->first.c_str() );

		if( XDF_SH == m_eMarketID ) {
			if( (nCode>=1 && nCode<=999) || (nCode>=600000&&nCode<=609999) || (nCode>=510000&&nCode<=519999) ) {
				it->second.CallBack4Query( nReqID, nBeginTime, nEndTime, bIsLastCode );
				nCallBackNum++;
			}
		} else if( XDF_SZ == m_eMarketID ) {
			if( (nCode>=399000 && nCode<=399999) || (nCode>=1&&nCode<=9999) || (nCode>=159000&&nCode<=159999) || (nCode>=300000&&nCode<=300999) ) {
				it->second.CallBack4Query( nReqID, nBeginTime, nEndTime, bIsLastCode );
				nCallBackNum++;
			}
		}
	}

	return nCallBackNum;
}

bool ParseMin1Line( const char* pszLine, double dPxRate, MinGenerator::T_DATA& refData )
{
	unsigned int	nLastPos = 0;
	unsigned int	nFieldNo = 0;
	unsigned int	nLen = ::strlen( pszLine ) + 1;

	for( int n = 0; n < nLen; n++ ) {
		if( ',' == pszLine[n] || '\n' == pszLine[n] ) {
			std::string	sFieldVal( pszLine + nLastPos, n-nLastPos );

			switch( nFieldNo ) {
			case 1:
				refData.Time = ::atoi( sFieldVal.c_str() );
				if( refData.Time <= 0 ) {
					return false;
				}
				break;
			case 2:
				refData.OpenPx = (int)(::atof( sFieldVal.c_str() ) * dPxRate);
				break;
			case 3:
				refData.HighPx = (int)(::atof( sFieldVal.c_str() ) * dPxRate);
				break;
			case 4:
				refData.LowPx = (int)(::atof( sFieldVal.c_str() ) * dPxRate);
				break;
			case 5:
				refData.ClosePx = (int)(::atof( sFieldVal.c_str() ) * dPxRate);
				break;
			case 7:
				refData.Amount = ::atof( sFieldVal.c_str() );
				break;
			case 8:
				refData.Volume = ::_atoi64( sFieldVal.c_str() );
				break;
			case 10:
				refData.NumTrades = ::_atoi64( sFieldVal.c_str() );
				break;
			case 11:
				refData.Voip = (int)(::atof( sFieldVal.c_str() ) * dPxRate);
				break;
			default:
				break;
			}

			nFieldNo++;
			nLastPos = n + 1;
		}
	}

	return true;
}

bool SecurityMinCache::LoadFromSyncDataFile()
{
	if( false == m_bSyncDataLoaded && true == m_objSyncMin1.IsLoaded() )
	{
		Global_LogUnit.WriteLogEx( 0, 0, "QuoteQueryClient", "SecurityMinCache::LoadFromSyncDataFile() : MarketID = %d, loading...................", (int)m_eMarketID );

		unsigned int			nUpdateCount = 0;
		unsigned int			nYear = MDateTime::Now().DateToLong() / 10000;
		unsigned int			nCodeNumber = m_vctCode.size();

		for( unsigned int n = 0; n < nCodeNumber && false == m_oRealCbAndLoadThread.GetThreadStopFlag(); n++ )
		{
			T_MAP_MINUTES::iterator	it;

			{
				MLocalSection				section( &m_oLockData );
				it = m_objMapMinutes.find( m_vctCode[n] );
				if( it == m_objMapMinutes.end() ) {
					continue;
				}
			}

			bool							bIsEOF = false;
			std::ifstream					objCSV;
			const std::string&				sCode = it->first;
			MinGenerator&					objMinGen = it->second;
			char							pszFileName[128] = { 0 };
			char							pszDataFilePath[1024] = { 0 };

			if( XDF_SH == m_eMarketID ) {
				::sprintf( pszFileName, "SSE\\MIN1_TODAY\\%s\\MIN%s_%u.csv", sCode.c_str(), sCode.c_str(), nYear );
			}	else	{
				::sprintf( pszFileName,"SZSE\\MIN1_TODAY\\%s\\MIN%s_%u.csv", sCode.c_str(), sCode.c_str(), nYear );
			}
			::PathCombine( pszDataFilePath, Global_Option.GetSyncDataFolder(), pszFileName );

			nUpdateCount++;
			objCSV.open( pszDataFilePath, std::ios::binary|std::ios::in );
			if( objCSV.is_open() ) {
				for( int n = 0; false == bIsEOF && n < 300 && false == m_oRealCbAndLoadThread.GetThreadStopFlag(); n++ )
				{
					char					pszLine[215] = { 0  };
					MinGenerator::T_DATA	tagMin1Data = { 0 };

					if( !objCSV.getline( pszLine, sizeof(pszLine) ) )	{
						bIsEOF = true;			///< EOF
					}

					if( n > 0  && ::strlen(pszLine) > 0 ) {
						if( true == ParseMin1Line( pszLine, objMinGen.GetPxRate(), tagMin1Data ) )	{
							objMinGen.AssignMin1( tagMin1Data );
						}
					}

				}

				objCSV.close();
			}

		}

		m_bSyncDataLoaded = true;				///< 标记为已经加载
		Global_LogUnit.WriteLogEx( 0, 0, "QuoteQueryClient", "SecurityMinCache::LoadFromSyncDataFile() : MarketID = %d, loaded %u ...................", (int)m_eMarketID, nUpdateCount );
		return true;
	}

	return false;
}

void* SecurityMinCache::RealCallbackAndLoadThread( void* pSelf )
{
	bool					bNotified = false;						///< 标记： 每次加载或断开重载后，都需要将最新一分钟线，回调一次
	SecurityMinCache&		refData = *(SecurityMinCache*)pSelf;
	Global_LogUnit.WriteLogEx( 0, 0, "QuoteQueryClient", "SecurityMinCache::RealCallbackAndLoadThread() : MarketID = %d, enter...................", (int)(refData.m_eMarketID) );

	while( false == refData.m_oRealCbAndLoadThread.GetThreadStopFlag() )
	{
		MThread::Sleep( 1000 * 3 );

		try
		{
			/////////////// 1分钟实时行情回调 ////////////////////////////////////////////////////////////////////////////////////////////
			unsigned int			nCodeNumber = refData.m_vctCode.size();

			for( unsigned int n = 0; n < nCodeNumber && false == refData.m_oRealCbAndLoadThread.GetThreadStopFlag(); n++ )
			{
				T_MAP_MINUTES::iterator	it;

				{
					MLocalSection			section( &(refData.m_oLockData) );
					it = refData.m_objMapMinutes.find( refData.m_vctCode[n] );
					if( it == refData.m_objMapMinutes.end() ) {
						continue;
					}
				}

				unsigned int		nCode = ::atoi( it->first.c_str() );

				if( XDF_SH == refData.m_eMarketID ) {
					if( (nCode>=1 && nCode<=999) || (nCode>=600000&&nCode<=609999) || (nCode>=510000&&nCode<=519999) ) {
						it->second.DumpMinutes();
					}
				} else if( XDF_SZ == refData.m_eMarketID ) {
					if( (nCode>=399000 && nCode<=399999) || (nCode>=1&&nCode<=9999) || (nCode>=159000&&nCode<=159999) || (nCode>=300000&&nCode<=300999) ) {
						it->second.DumpMinutes();
					}
				}
			}

			//////////// 检查是否有从server端同步今日最新的1分钟线，如果有，则进行一次全时段查询回调(一次连接只需要回调一次) /////////////////
			refData.LoadFromSyncDataFile();												///< 加载同步下来的1分钟线数据
			if ( false == bNotified && true == refData.m_objSyncMin1.IsLoaded() )	{	///< 加载今日同步的1分钟线数据文件
				unsigned int	nBeginTime = 0;											///< 查询开始时间
				unsigned int	nEndTime = 0;											///< 查询结束时间
				unsigned int	nRequestID = 300000000 + (int)(refData.m_eMarketID);	///< 查询请求编号(300000000 + 市场ID)

				switch( refData.m_eMarketID )	{
				case XDF_SH:
				case XDF_SZ:
					nBeginTime = 50101;
					nEndTime = 190101;
					break;
				default:
					Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "SecurityMinCache::RealCallbackAndLoadThread() : invalid marketid..." );
					continue;
				}

				if( 0 == refData.m_pQueryClientApi->QueryMinuteLines( refData.m_eMarketID, nRequestID, 50101, 190101 ) ) {
					bNotified = true;	///< 标记已经回调过了，下次不用再回调
					Global_LogUnit.WriteLogEx( 0, 0, "QuoteQueryClient", "SecurityMinCache::RealCallbackAndLoadThread() : MarketID(%d), Active query callback ...", (int)(refData.m_eMarketID) );
				}
			}
		}
		catch( std::exception& err )
		{
			Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "SecurityMinCache::RealCallbackAndLoadThread() : exception : %s", err.what() );
		}
		catch( ... )
		{
			Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "SecurityMinCache::RealCallbackAndLoadThread() : unknow exception" );
		}
	}

	Global_LogUnit.WriteLogEx( 0, 0, "QuoteQueryClient", "SecurityMinCache::RealCallbackAndLoadThread() : MarketID = %d, misson complete!............", (int)(refData.m_eMarketID) );

	return NULL;
}


//....................................................................................................................................................................................................................................................................

MQueryClient::MQueryClient()
 : m_bIsQuerying( false ), m_pQuoteQuerySpi( NULL ), m_nBeginTime( 0 ), m_nEndTime( 0 )
 , m_nMkDate4SHL1( 0 ), m_nMkDate4SZL1( 0 ), m_nMkTime4SHL1( 0 ), m_nMkTime4SZL1( 0 )
 , m_obj1MinCache4SHL1( XDF_SH, this ), m_obj1MinCache4SZL1( XDF_SZ, this )
{
	Release();
}

QuoteQuerySpi* MQueryClient::GetHandle()
{
	return m_pQuoteQuerySpi;
}

int	MQueryClient::Initialize()
{
	Release();

	if( m_oQueryThread.StartThread( "MQueryClient",QueryThreadFunc, this ) < 0 )
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::Initialize() : cannot active query thread obj." );
		return -1;
	}

	return 0;
}

void MQueryClient::Release()
{
	m_obj1MinCache4SHL1.StopDumper();
	m_obj1MinCache4SZL1.StopDumper();
	m_oQueryThread.StopThread();

	m_nMkDate4SHL1 = 0;
	m_nMkDate4SZL1 = 0;
	m_nMkTime4SHL1 = 0;
	m_nMkTime4SZL1 = 0;
	m_nBeginTime = 0;
	m_nEndTime = 0;
	m_nWareCount4SHL1 = -1;
	m_nWareCount4SZL1 = -1;
}

void MQueryClient::RegisterSpi( QuoteQuerySpi* pSpi, int nMinuteLineType )
{
	m_pQuoteQuerySpi = pSpi;
}

void MQueryClient::SetWareCount4SHL1( int nWareCount )
{
	m_nWareCount4SHL1 = nWareCount;
}

void MQueryClient::SetWareCount4SZL1( int nWareCount )
{
	m_nWareCount4SZL1 = nWareCount;
}

int STDCALL MQueryClient::QueryMinuteLines( enum XDFMarket eMkID, unsigned int nRequestID, unsigned int nBeginTime, unsigned int nEndTime )
{
	if( true == m_bIsQuerying )
	{
		return -1024;						///< 还有在执行中的请求，当前本次请求被忽略
	}

	if( NULL == m_pQuoteQuerySpi )
	{
		return -2;							///< 未注册回调接口
	}

	if( XDF_SH == eMkID )
	{
		m_eReqMkID = eMkID;
		m_bIsQuerying = true;
		m_nRequestID = nRequestID;
		m_nBeginTime = nBeginTime;
		m_nEndTime = nEndTime;

		return 0;
	}
	else if( XDF_SZ == eMkID )
	{
		m_eReqMkID = eMkID;
		m_bIsQuerying = true;
		m_nRequestID = nRequestID;
		m_nBeginTime = nBeginTime;
		m_nEndTime = nEndTime;

		return 0;
	}

	return -100;
}

void* STDCALL MQueryClient::QueryThreadFunc( void* pParam )
{
	MQueryClient*		pSelf = (MQueryClient*)pParam;

	while( !pSelf->m_oQueryThread.GetThreadStopFlag() )
    {
		try
		{
			MThread::Sleep( 1000 );
			if( true == pSelf->m_bIsQuerying )													///< 有分钟线的查询请求
			{
				if( XDF_SH == pSelf->m_eReqMkID && pSelf->m_nWareCount4SHL1 > 0 )
				{
					pSelf->m_obj1MinCache4SHL1.QueryRecords( pSelf->m_nRequestID, pSelf->m_nBeginTime, pSelf->m_nEndTime );
				}
				else if( XDF_SZ == pSelf->m_eReqMkID && pSelf->m_nWareCount4SZL1 > 0 )
				{
					pSelf->m_obj1MinCache4SZL1.QueryRecords( pSelf->m_nRequestID, pSelf->m_nBeginTime, pSelf->m_nEndTime );
				}

				pSelf->m_bIsQuerying = false;
			}
		}
		catch( std::exception& err )
		{
			Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::QueryThreadFunc() : exception : %s", err.what() );
			pSelf->m_pQuoteQuerySpi->OnNotifySyncMinuteLine( (char)(pSelf->m_eReqMkID), NULL, err.what(), pSelf->m_nRequestID, true );
		}
		catch( ... )
        {
			Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::QueryThreadFunc() : unknow exception" );
			pSelf->m_pQuoteQuerySpi->OnNotifySyncMinuteLine( (char)(pSelf->m_eReqMkID), NULL, "unknow exception", pSelf->m_nRequestID, true );
        }
	}

	return NULL;
}

void MQueryClient::OnSHL1Index( unsigned int nMkTime, tagCcComm_IndexData& refData )
{
	if( nMkTime > m_nMkTime4SHL1 ) {
		m_nMkTime4SHL1 = nMkTime;
	}

	if( m_nWareCount4SHL1 <= 0 )
	{
		return;
	}

	m_obj1MinCache4SHL1.UpdateSecurity( refData, m_nMkDate4SHL1, nMkTime );
}

void MQueryClient::OnSHL1Snap( unsigned int nMkTime, tagCcComm_StockData5& refData )
{
	if( nMkTime > m_nMkTime4SHL1 ) {
		m_nMkTime4SHL1 = nMkTime;
	}

	if( m_nWareCount4SHL1 <= 0 )
	{
		return;
	}

	m_obj1MinCache4SHL1.UpdateSecurity( refData, m_nMkDate4SHL1, nMkTime );
}

void MQueryClient::OnSZL1Index( unsigned int nMkTime, tagCcComm_IndexData& refData )
{
	if( nMkTime > m_nMkTime4SZL1 ) {
		m_nMkTime4SZL1 = nMkTime;
	}

	if( m_nWareCount4SZL1 <= 0 )
	{
		return;
	}

	m_obj1MinCache4SZL1.UpdateSecurity( refData, m_nMkDate4SHL1, nMkTime );
}

void MQueryClient::OnSZL1Snap( unsigned int nMkTime, tagCcComm_StockData5& refData )
{
	if( nMkTime > m_nMkTime4SZL1 ) {
		m_nMkTime4SZL1 = nMkTime;
	}

	if( m_nWareCount4SZL1 <= 0 )
	{
		return;
	}

	m_obj1MinCache4SZL1.UpdateSecurity( refData, m_nMkDate4SHL1, nMkTime );
}

void MQueryClient::OnMarketAvailableNotify( unsigned char cMkID, int nStatus )
{
	int		nRet = 0;

	if( XRS_Normal!=nStatus || NULL==m_pQuoteQuerySpi )	///< 只处理某市场状态为“可服务”的情况
	{
		switch( cMkID )
		{
		case XDF_SH:
			m_obj1MinCache4SHL1.StopDumper();			///< 停止回调线程
			break;
		case XDF_SZ:
			m_obj1MinCache4SZL1.StopDumper();			///< 停止回调线程
			break;
		}

		return;
	}

	switch( cMkID )
	{
	case XDF_SH:
		{
			m_nWareCount4SHL1 = -1;						///< 标识为分钟线"不可查询"
			MThread::Sleep( 0 );MThread::Sleep( 1000 );MThread::Sleep( 0 );
			if( (nRet = LoadMinuteLine4SHL1()) > 0 )	///< 加载SHL1市场分钟线状态
			{
				SetWareCount4SHL1( nRet );				///< 设置SHL1市场分钟线“可查询状态”
				m_obj1MinCache4SHL1.ActivateDumper();
			}
		}
		break;
	case XDF_SZ:
		{
			m_nWareCount4SZL1 = -1;						///< 标识为分钟线"不可查询"
			MThread::Sleep( 0 );MThread::Sleep( 1000 );MThread::Sleep( 0 );
			if( (nRet = LoadMinuteLine4SZL1()) > 0 )	///< 加载SZL1市场分钟线状态
			{
				SetWareCount4SZL1( nRet );				///< 设置SZL1市场分钟线“可查询状态”
				m_obj1MinCache4SZL1.ActivateDumper();
			}
		}
		break;
	default:
		break;
	}
}

typedef std::map<std::string,double>	TMAP_PX_RATE;	///< 放大倍数表

int MQueryClient::LoadMinuteLine4SHL1()
{
	TMAP_PX_RATE			mapPxRate;
	int						nKindCount = 0;
	int						nCodeCount = 0;
	char					tempbuf[8192] = { 0 };
	XDFAPI_MarketKindInfo	vctKindInfo[32] = { 0 };
	int						nErrorCode = Global_DllMgr.GetMarketInfo( XDF_SH, tempbuf, sizeof(tempbuf) );

	m_nMkTime4SHL1 = 0;
	m_nMkDate4SHL1 = 0;
	///< -------------- 获取上海的基础信息 --------------------------------------------
	if( nErrorCode <= 0 )
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SHL1() : cannot access marketinfo, errorcode = %d", nErrorCode );
		return -100;
	}

	int m = sizeof(XDFAPI_MsgHead)+sizeof(XDFAPI_MarketKindHead);
	for( int i = 0; m < nErrorCode; )
	{
		XDFAPI_UniMsgHead*	pMsgHead = (XDFAPI_UniMsgHead*)(tempbuf + m);
		char*				pbuf = tempbuf + m +sizeof(XDFAPI_UniMsgHead);
		int					nMsgCount = pMsgHead->MsgCount;

		while( nMsgCount-- > 0 )
		{
			XDFAPI_MarketKindInfo* pInfo = (XDFAPI_MarketKindInfo*)pbuf;
			::memcpy( vctKindInfo + nKindCount++, pInfo, sizeof(XDFAPI_MarketKindInfo) );
			pbuf += sizeof(XDFAPI_MarketKindInfo);
		}

		m += sizeof(XDFAPI_MsgHead) + pMsgHead->MsgLen;
	}

	///< -------------- 获取上海的码表信息 --------------------------------------------
	nErrorCode = Global_DllMgr.GetCodeTable( XDF_SH, NULL, NULL, nCodeCount );						///< 先获取一下商品数量
	if( 0 != m_obj1MinCache4SHL1.Initialize( nCodeCount ) ) {
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SHL1() : cannot initialize min1 buffer, errorcode = %d", nErrorCode );
		return -100;
	}
	if( nErrorCode > 0 && nCodeCount > 0 )
	{
		int					noffset = (sizeof(XDFAPI_NameTableSh) + sizeof(XDFAPI_UniMsgHead)) * nCodeCount;	///< 根据商品数量，分配获取快照表需要的缓存
		char*				pszCodeBuf = new char[noffset];

		if( NULL == pszCodeBuf )
		{
			Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SHL1() : cannot allocate more bufffer 4 code table, size = %d", noffset );
			return -101;
		}

		nErrorCode = Global_DllMgr.GetCodeTable( XDF_SH, pszCodeBuf, noffset, nCodeCount );			///< 获取码表
		for( int m = 0; m < nErrorCode; )
		{
			XDFAPI_UniMsgHead*	pMsgHead = (XDFAPI_UniMsgHead*)(pszCodeBuf+m);
			char*				pbuf = pszCodeBuf+m +sizeof(XDFAPI_UniMsgHead);
			int					MsgCount = pMsgHead->MsgCount;

			for( int i = 0; i < MsgCount; i++ )
			{
				if( abs(pMsgHead->MsgType) == 5 )
				{
					XDFAPI_NameTableSh*		pData = (XDFAPI_NameTableSh*)pbuf;

					mapPxRate[std::string(pData->Code,6)] = ::pow(10.0, int(vctKindInfo[pData->SecKind].PriceRate) );

					pbuf += sizeof(XDFAPI_NameTableSh);
				}
			}

			m += (sizeof(XDFAPI_UniMsgHead) + pMsgHead->MsgLen - sizeof(pMsgHead->MsgCount));
		}

		if( NULL != pszCodeBuf )
		{
			delete []pszCodeBuf;
			pszCodeBuf = NULL;
		}
	}
	else
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SHL1() : cannot fetch nametable, size = %d", nErrorCode );
		return -102;
	}

	///< -------------- 获取上海的快照信息 --------------------------------------------
	nErrorCode = Global_DllMgr.GetCodeTable( XDF_SH, NULL, NULL, nCodeCount );					///< 先获取一下商品数量
	if( nErrorCode > 0 && nCodeCount > 0 )
	{
		int		noffset = (sizeof(XDFAPI_StockData5) + sizeof(XDFAPI_UniMsgHead)) * nCodeCount;	///< 根据商品数量，分配获取快照表需要的缓存
		char*	pszCodeBuf = new char[noffset];

		if( NULL == pszCodeBuf )
		{
			Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SHL1() : cannot allocate more bufffer 4 snap table, size = %d", noffset );
			return -103;
		}

		nErrorCode = Global_DllMgr.GetLastMarketDataAll( XDF_SH, pszCodeBuf, noffset );			///< 获取快照
		for( int m = 0; m < nErrorCode; )
		{
			XDFAPI_UniMsgHead*			pMsgHead = (XDFAPI_UniMsgHead*)(pszCodeBuf+m);
			char*						pbuf = pszCodeBuf+m +sizeof(XDFAPI_UniMsgHead);
			int							MsgCount = pMsgHead->MsgCount;
			XDFAPI_MarketStatusInfo		tagMkInfo;

			tagMkInfo.MarketID = XDF_SH;
			if( Global_DllMgr.GetQuicksSimpleMarketInfo( XDF_SH, &tagMkInfo ) > 0 )
			{
				m_nMkDate4SHL1 = tagMkInfo.MarketDate;
				m_nMkTime4SHL1 = tagMkInfo.MarketTime > m_nMkTime4SHL1 ? tagMkInfo.MarketTime : m_nMkTime4SHL1;
			}

			for( int i = 0; i < MsgCount; i++ )
			{
				MinGenerator::T_DATA		tagLine = { 0 };

				if( abs(pMsgHead->MsgType) == 21 )			///< 指数
				{
					XDFAPI_IndexData*	pData = (XDFAPI_IndexData*)pbuf;
					std::string			sCode( pData->Code, 6 );
					TMAP_PX_RATE::iterator	it = mapPxRate.find( sCode );

					if( mapPxRate.end() == it )
					{
						nCodeCount--;
						Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SHL1() : ignore snap, code = %s", sCode.c_str() );
					}
					else
					{
						m_obj1MinCache4SHL1.NewSecurity( XDF_SH, sCode, tagMkInfo.MarketDate, it->second, tagLine );
					}

					pbuf += sizeof(XDFAPI_IndexData);
				}
				else if( abs(pMsgHead->MsgType) == 22 )		///< 快照
				{
					XDFAPI_StockData5*	pData = (XDFAPI_StockData5*)pbuf;
					std::string			sCode( pData->Code, 6 );
					TMAP_PX_RATE::iterator	it = mapPxRate.find( sCode );

					if( mapPxRate.end() == it )
					{
						nCodeCount--;
						Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SHL1() : ignore snap, code = %s", sCode.c_str() );
					}
					else
					{
						m_obj1MinCache4SHL1.NewSecurity( XDF_SH, sCode, tagMkInfo.MarketDate, it->second, tagLine );
					}

					pbuf += sizeof(XDFAPI_StockData5);
				}
			}

			m += (sizeof(XDFAPI_UniMsgHead) + pMsgHead->MsgLen - sizeof(pMsgHead->MsgCount));
		}

		if( NULL != pszCodeBuf )
		{
			delete []pszCodeBuf;
		}

		return nCodeCount;
	}

	Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SHL1() : cannot fetch snap table, size = %d", nErrorCode );

	return -104;
}

int MQueryClient::LoadMinuteLine4SZL1()
{
	TMAP_PX_RATE			mapPxRate;
	int						nKindCount = 0;
	int						nCodeCount = 0;
	char					tempbuf[8192] = { 0 };
	XDFAPI_MarketKindInfo	vctKindInfo[32] = { 0 };
	int						nErrorCode = Global_DllMgr.GetMarketInfo( XDF_SZ, tempbuf, sizeof(tempbuf) );

	m_nMkTime4SZL1 = 0;
	m_nMkDate4SZL1 = 0;
	///< -------------- 获取深圳的基础信息 --------------------------------------------
	if( nErrorCode <= 0 )
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SZL1() : cannot access marketinfo, errorcode = %d", nErrorCode );
		return -100;
	}

	int m = sizeof(XDFAPI_MsgHead)+sizeof(XDFAPI_MarketKindHead);
	for( int i = 0; m < nErrorCode; )
	{
		XDFAPI_UniMsgHead*	pMsgHead = (XDFAPI_UniMsgHead*)(tempbuf + m);
		char*				pbuf = tempbuf + m +sizeof(XDFAPI_UniMsgHead);
		int					nMsgCount = pMsgHead->MsgCount;

		while( nMsgCount-- > 0 )
		{
			XDFAPI_MarketKindInfo* pInfo = (XDFAPI_MarketKindInfo*)pbuf;
			::memcpy( vctKindInfo + nKindCount++, pInfo, sizeof(XDFAPI_MarketKindInfo) );
			pbuf += sizeof(XDFAPI_MarketKindInfo);
		}

		m += sizeof(XDFAPI_MsgHead) + pMsgHead->MsgLen;
	}

	///< -------------- 获取深圳的码表信息 --------------------------------------------
	nErrorCode = Global_DllMgr.GetCodeTable( XDF_SZ, NULL, NULL, nCodeCount );						///< 先获取一下商品数量
	if( 0 != m_obj1MinCache4SZL1.Initialize( nCodeCount ) ) {
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SZL1() : cannot initialize min1 buffer, errorcode = %d", nErrorCode );
		return -100;
	}
	if( nErrorCode > 0 && nCodeCount > 0 )
	{
		int					noffset = (sizeof(XDFAPI_NameTableSz) + sizeof(XDFAPI_UniMsgHead)) * nCodeCount;	///< 根据商品数量，分配获取快照表需要的缓存
		char*				pszCodeBuf = new char[noffset];

		if( NULL == pszCodeBuf )
		{
			Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SZL1() : cannot allocate more bufffer 4 code table, size = %d", noffset );
			return -101;
		}

		nErrorCode = Global_DllMgr.GetCodeTable( XDF_SZ, pszCodeBuf, noffset, nCodeCount );			///< 获取码表
		for( int m = 0; m < nErrorCode; )
		{
			XDFAPI_UniMsgHead*	pMsgHead = (XDFAPI_UniMsgHead*)(pszCodeBuf+m);
			char*				pbuf = pszCodeBuf+m +sizeof(XDFAPI_UniMsgHead);
			int					MsgCount = pMsgHead->MsgCount;

			for( int i = 0; i < MsgCount; i++ )
			{
				if( abs(pMsgHead->MsgType) == 6 )
				{
					XDFAPI_NameTableSz*		pData = (XDFAPI_NameTableSz*)pbuf;

					mapPxRate[std::string(pData->Code,6)] = ::pow(10.0, int(vctKindInfo[pData->SecKind].PriceRate) );

					pbuf += sizeof(XDFAPI_NameTableSz);
				}
			}

			m += (sizeof(XDFAPI_UniMsgHead) + pMsgHead->MsgLen - sizeof(pMsgHead->MsgCount));
		}

		if( NULL != pszCodeBuf )
		{
			delete []pszCodeBuf;
			pszCodeBuf = NULL;
		}
	}
	else
	{
		Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SZL1() : cannot fetch nametable, size = %d", nErrorCode );
		return -102;
	}

	///< -------------- 获取深圳的快照信息 --------------------------------------------
	nErrorCode = Global_DllMgr.GetCodeTable( XDF_SZ, NULL, NULL, nCodeCount );					///< 先获取一下商品数量
	if( nErrorCode > 0 && nCodeCount > 0 )
	{
		int		noffset = (sizeof(XDFAPI_StockData5) + sizeof(XDFAPI_UniMsgHead)) * nCodeCount;	///< 根据商品数量，分配获取快照表需要的缓存
		char*	pszCodeBuf = new char[noffset];

		if( NULL == pszCodeBuf )
		{
			Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SZL1() : cannot allocate more bufffer 4 snap table, size = %d", noffset );
			return -103;
		}

		nErrorCode = Global_DllMgr.GetLastMarketDataAll( XDF_SZ, pszCodeBuf, noffset );			///< 获取快照
		for( int m = 0; m < nErrorCode; )
		{
			XDFAPI_UniMsgHead*			pMsgHead = (XDFAPI_UniMsgHead*)(pszCodeBuf+m);
			char*						pbuf = pszCodeBuf+m +sizeof(XDFAPI_UniMsgHead);
			int							MsgCount = pMsgHead->MsgCount;
			XDFAPI_MarketStatusInfo		tagMkInfo;

			tagMkInfo.MarketID = XDF_SZ;
			if( Global_DllMgr.GetQuicksSimpleMarketInfo( XDF_SZ, &tagMkInfo ) > 0 )
			{
				m_nMkDate4SZL1 = tagMkInfo.MarketDate;
				m_nMkTime4SZL1 = tagMkInfo.MarketTime > m_nMkTime4SZL1 ? tagMkInfo.MarketTime : m_nMkTime4SZL1;
			}

			for( int i = 0; i < MsgCount; i++ )
			{
				MinGenerator::T_DATA			tagLine = { 0 };

				if( abs(pMsgHead->MsgType) == 21 )			///< 指数
				{
					XDFAPI_IndexData*	pData = (XDFAPI_IndexData*)pbuf;
					std::string			sCode( pData->Code, 6 );
					TMAP_PX_RATE::iterator	it = mapPxRate.find( sCode );

					if( mapPxRate.end() == it )
					{
						nCodeCount--;
						Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SZL1() : ignore snap, code = %s", sCode.c_str() );
					}
					else
					{
						m_obj1MinCache4SZL1.NewSecurity( XDF_SZ, sCode, tagMkInfo.MarketDate, it->second, tagLine );
					}

					pbuf += sizeof(XDFAPI_IndexData);
				}
				else if( abs(pMsgHead->MsgType) == 22 )		///< 快照
				{
					XDFAPI_StockData5*	pData = (XDFAPI_StockData5*)pbuf;
					std::string			sCode( pData->Code, 6 );
					TMAP_PX_RATE::iterator	it = mapPxRate.find( sCode );

					if( mapPxRate.end() == it )
					{
						nCodeCount--;
						Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SZL1() : ignore snap, code = %s", sCode.c_str() );
					}
					else
					{
						m_obj1MinCache4SZL1.NewSecurity( XDF_SZ, sCode, tagMkInfo.MarketDate, it->second, tagLine );
					}

					pbuf += sizeof(XDFAPI_StockData5);
				}
			}

			m += (sizeof(XDFAPI_UniMsgHead) + pMsgHead->MsgLen - sizeof(pMsgHead->MsgCount));
		}

		if( NULL != pszCodeBuf )
		{
			delete []pszCodeBuf;
		}

		return nCodeCount;
	}

	Global_LogUnit.WriteLogEx( 3, 0, "QuoteQueryClient", "MQueryClient::LoadMinuteLine4SZL1() : cannot fetch snap table, size = %d", nErrorCode );

	return -104;
}


//....................................................................................................................................................................................................................................................................
MPrimeClient::MPrimeClient()
{

}

MPrimeClient::~MPrimeClient()
{

}

int		STDCALL		MPrimeClient::ReqFuncData(int FuncNo, void* wParam, void* lParam)
{
	if (FuncNo ==100)			//获取某个市场的市场日期和市场时间(参数:uint8*,   XDFAPI_MarketStatusInfo*)
	{
		uint8_t * pMarket = (uint8_t*)wParam;
		XDFAPI_MarketStatusInfo* pInfo = (XDFAPI_MarketStatusInfo*)lParam;
		if (pMarket && pInfo)
		{
			uint8_t cMarket = *pMarket;
			XDFAPI_MarketStatusInfo oInfo;

			oInfo.MarketID = cMarket;
			int iret = Global_DllMgr.GetSimpleMarketInfo(cMarket, &oInfo);
			if (iret >0)
			{
				*pInfo = oInfo;
				return 1;
			}
		}
	}
	else if (FuncNo ==101)		//获取某个市场的市场日期和市场时间(参数:uint8*,   XDFAPI_MarketStatusInfo*)
	{
		uint8_t * pMarket = (uint8_t*)wParam;
		XDFAPI_MarketStatusInfo* pInfo = (XDFAPI_MarketStatusInfo*)lParam;
		if (pMarket && pInfo)
		{
			uint8_t cMarket = *pMarket;
			XDFAPI_MarketStatusInfo oInfo;

			oInfo.MarketID = cMarket;
			int iret = Global_DllMgr.GetQuicksSimpleMarketInfo(cMarket, &oInfo);
			if (iret >0)
			{
				*pInfo = oInfo;
				return 1;
			}
		}
	}
	else if( FuncNo == 102 )	// 获取沪、深停牌标记
	{
		int						nErrorCode = 0;
		int						nCodeCount = 0;
		XDFAPI_ReqFuncParam*	pParam = (XDFAPI_ReqFuncParam*)wParam;
		MStreamWrite			oMSW( (char*)lParam, pParam->nBufLen );

		switch( pParam->MkID )
		{
		case XDF_SH:
			{
				nErrorCode = Global_DllMgr.GetCodeTable( XDF_SH, NULL, NULL, nCodeCount );		///< 先获取一下商品数量
				if( nErrorCode <= 0 && nCodeCount <= 0 )
				{
					return -2;
				}

				Global_DllMgr.m_oShSEDll.inner_getStopFlagData( nCodeCount, oMSW );
			}
			break;
		case XDF_SZ:
			{
				nErrorCode = Global_DllMgr.GetCodeTable( XDF_SZ, NULL, NULL, nCodeCount );		///< 先获取一下商品数量
				if( nErrorCode <= 0 && nCodeCount <= 0 )
				{
					return -3;
				}

				Global_DllMgr.m_oSzSEDll.inner_getStopFlagData( nCodeCount, oMSW );
			}
			break;
		default:
			return -100;
		}

		oMSW.Detach();
		return oMSW.GetOffset();
	}
	else if( 103 == FuncNo )	// 获取中金期货市场码表(用新的格式)
	{
		int						nCodeCount = 0;
		int						nErrorCode = 0;
		unsigned int			nOutputBufLen = 0;
		char*					pOutputBuf = (char*)lParam;
		XDFAPI_ReqFuncParam*	pParam = (XDFAPI_ReqFuncParam*)wParam;

		if( NULL != lParam ) {
			nOutputBufLen = pParam->nBufLen;
		}

		nErrorCode = Global_DllMgr.GetCodeTableEx( pParam->MkID, pOutputBuf, nOutputBufLen, nCodeCount );		///< 先获取一下商品数量
		if( nErrorCode < 0 )
		{
			return -101;
		}

		return nCodeCount;
	}

	return 0;
}









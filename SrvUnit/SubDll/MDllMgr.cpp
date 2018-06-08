
#include "StdAfx.h"
#include "MDllMgr.h"
#include "../GlobalIO/GlobalIO.h"

#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")

//.............................................................................................................................................................
//.............................................................................................................................................................
//.............................................................................................................................................................
//.............................................................................................................................................................
//.............................................................................................................................................................
//.............................................................................................................................................................
//.............................................................................................................................................................
M4XDllStat::M4XDllStat()
{

}
M4XDllStat::~M4XDllStat()
{

}
int		M4XDllStat::BeginWork()
{
	inner_FreeMarket();
	
	int nDllCount = Global_Option.GetKeyFileCount();
	if (nDllCount<=0)
	{
		return -1;
	}
	
	char szModulePath[255]={0};
	Global_Option.GetModulePath(szModulePath);
	
	int i=0;
	tagKeyFileInfo oInfo;
	for (i=0; i< nDllCount; i++)
	{
		memset(&oInfo, 0, sizeof(tagKeyFileInfo));
		Global_Option.GetKeyFileInfo(i, oInfo);
		inner_StartOneMarket(oInfo.nMarketID,szModulePath, oInfo.szDll, 50+i);
		if (oInfo.nSleep >0)
		{
			Sleep(oInfo.nSleep);
		}
		
	}
	
	return 1;
}

void	M4XDllStat::EndWork()
{
	int i=0;
	for (i=0; i<m_arrStat.size(); i++)
	{
		inner_StopOneMarket(m_arrStat[i].nMarketID);
	}
	inner_FreeMarket();
}

void		M4XDllStat::inner_AddMarket(int nMarketID, int nInnerStat, int nOuterStat)
{
	MLocalSection lock(&m_Section);
	
	tagOneMarketStat oStat;
	oStat.nMarketID = nMarketID;
	oStat.nInnerStat = nInnerStat;
	oStat.nOuterStat = nOuterStat;
	m_arrStat.push_back(oStat);
}
void		M4XDllStat::inner_FreeMarket()
{
	MLocalSection lock(&m_Section);
	m_arrStat.clear();
}

int			M4XDllStat::inner_StartOneMarket(int nMarketID, const char* pszModulePath, const char* pszDllPath, int UnitNo)
{
	char szFile[255]={0};
	if (::PathIsRelative(pszDllPath))
	{
		PathCombine(szFile, pszModulePath, pszDllPath);
	}
	else
	{
		strcpy(szFile, pszDllPath);
	}
	
	int iret =0;
	Base4XDll *pDll = GetMarketDll(nMarketID);
	if (pDll)
	{
		iret = pDll->Instance(szFile, UnitNo, nMarketID);
	}

	int nInnerStat = (iret>0)?(1):(0) ;
	inner_AddMarket(nMarketID, nInnerStat, 0);
	
	return iret;
}

void	M4XDllStat::inner_StopOneMarket(int nMarketID)
{
	Base4XDll * pDll = GetMarketDll(nMarketID);
	if (pDll)
	{
		pDll->Release();
	}
}

Base4XDll*		M4XDllStat::GetMarketDll(int nMarketId)
{
	Base4XDll* pDll =0;
	switch (nMarketId)
	{
	case QL4X_SHStock:
		pDll = &m_oShSe;
		break;
	case QL4X_SZStock:
		pDll = &m_oSzSe;
		break;
	case QL4X_SHOPT:
		pDll = &m_oShOpt;
		break;
	case QL4X_SZOPT:
		pDll = &m_oSzOpt;
		break;
	case QL4X_ZJQH:
		pDll = &m_oZjqh;
		break;
	case QL4X_ZJOPT:
		pDll = &m_oZjOpt;
		break;
	case QL4X_CNF:
		pDll = &m_oCnf;
		break;
	case QL4X_CNFOPT:
		pDll = &m_oCnfOpt;
		break;
	}
	return pDll;
}

int	M4XDllStat::GetMarketStat(int nMarketID)
{
	MLocalSection lock(&m_Section);
	int nsize = m_arrStat.size();
	for (int i=0; i<nsize; i++)
	{
		if (m_arrStat[i].nMarketID == nMarketID)
		{
			return m_arrStat[i].nOuterStat;
		}
	}
	return 0;
}

void M4XDllStat::CheckMarket()
{
	int nsize = m_arrStat.size();
	int nMarketID=0;
	int nNewStat =0;
	
	for (int i=0; i<nsize; i++)
	{
		nMarketID = m_arrStat[i].nMarketID;
		if (m_arrStat[i].nInnerStat >0)	//启动成功
		{
			unsigned char uMarket = Global_MarketID2XDFID(nMarketID);
			Base4XDll * pDll = GetMarketDll(nMarketID);
			if (pDll)
			{
				nNewStat  = inner_CheckOneMarket(pDll);
				if (m_arrStat[i].nOuterStat != nNewStat)
				{
					m_arrStat[i].nOuterStat = nNewStat;
					if (Global_pSpi)
					{
						Global_QueryClient.OnMarketAvailableNotify( uMarket, nNewStat );		///< 在某市场服务变可用时，加载相应的行情数据
						Global_pSpi->XDF_OnRspStatusChanged(uMarket, nNewStat);
					}
				}
			}
			
		}
		
	}
}

int	M4XDllStat::inner_CheckOneMarket(Base4XDll* pObj)
{
	if (0 == pObj)
	{
		return XRS_None;
	}
	if (pObj->IsDllWorking())
	{
		tagCcComm_MarketInfoHead			markethead;
		char								tempbuf[8192];
		tagCcComm_MarketInfoDetail * pMarketdetail = (tagCcComm_MarketInfoDetail *)tempbuf;
		
		int iret = pObj->GetMarketInfo( &markethead, pMarketdetail, sizeof(tempbuf)/sizeof(tagCcComm_MarketInfoDetail) );
		if (iret <=0)
		{
			return XRS_Unknow;
		}
		if (markethead.MarketStatus <= 0)
		{
			return XRS_Init;
		}
		return XRS_Normal;
	}
	else
	{
		return XRS_None;
	}
}

//.................................................................................................................................................................................................................................................................................................
//.................................................................................................................................................................................................................................................................................................
//.................................................................................................................................................................................................................................................................................................
//.................................................................................................................................................................................................................................................................................................
//.................................................................................................................................................................................................................................................................................................

MDllMgr::MDllMgr()
{
	m_bWorking =false;
}

MDllMgr::~MDllMgr()
{

}

int		MDllMgr::Instance()
{
	Release();
	
	//MData4XDll 和Base4XDll的绑定
	int arrMarket[]=
	{ 
		QL4X_SHStock, QL4X_SZStock, QL4X_SHOPT, QL4X_ZJQH, QL4X_ZJOPT, QL4X_CNF, QL4X_SZOPT, QL4X_CNFOPT
	};
	for (int i=0; i< sizeof(arrMarket)/sizeof(int); ++i)
	{
		Base4XDll * pDll = m_oWorkStat.GetMarketDll(arrMarket[i]);
		MData4XDll* pDataDll = GetMarketDll(arrMarket[i]);
		if (pDataDll && pDll)
		{
			pDataDll->Bind4XDll(pDll);
		}
	}

	int iret = m_threadClient.StartThread("TaskThread",TaskThreadFunc, this);
	if (iret <0)
	{
		return -2;
	}
	
	m_oCounter.SetCurTickCount();

	return 1;
}

void	MDllMgr::Release()
{
	EndWork();		//最后再清一遍
	
	m_threadClient.StopThread(15000);
	m_bWorking =false;
}


int		MDllMgr::BeginWork()
{
	int iret = m_oWorkStat.BeginWork();		///< 加载、初始化各市场的DLL

	//此处有Reset动作
	int arrMarket[]=
	{ 
		QL4X_SHStock, QL4X_SZStock, QL4X_SHOPT, QL4X_ZJQH, QL4X_ZJOPT, QL4X_CNF, QL4X_SZOPT, QL4X_CNFOPT
	};
	for (int i=0; i< sizeof(arrMarket)/sizeof(int); ++i)
	{
		MData4XDll* pDataDll = GetMarketDll(arrMarket[i]);
		if (pDataDll )
		{
			pDataDll->Reset();
		}
	}
	m_bWorking =true;

	return iret;
}

void	MDllMgr::EndWork()
{
	m_bWorking =false;
	m_oWorkStat.EndWork();
}

MData4XDll*	MDllMgr::GetMarketDll(int nMarketId)
{
	MData4XDll* pDll =0;
	switch (nMarketId)
	{
	case QL4X_SHStock:
		pDll = &m_oShSEDll;
		break;
	case QL4X_SZStock:
		pDll = &m_oSzSEDll;
		break;
	case QL4X_SHOPT:
		pDll = &m_oShOptDll;
		break;
	case QL4X_SZOPT:
		pDll = &m_oSzOptDll;
		break;
	case QL4X_ZJQH:
		pDll = &m_oZjqhDll;
		break;
	case QL4X_ZJOPT:
		pDll = &m_oZjOptDll;
		break;
	case QL4X_CNF:
		pDll = &m_oCnfDll;
		break;
	case QL4X_CNFOPT:
		pDll = &m_oCnfOptDll;
		break;
	}
	return pDll;
}
/*
int		MDllMgr::GetStopFlagTable(uint8_t cMarket, char* pszInBuf, int32_t nInBytes, int32_t& nCount)
{
	int nMarket = Global_XDFID2MarketID(cMarket);
	MData4XDll* pDll = GetMarketDll(nMarket);
	if (pDll)
	{
		return pDll->outer_GetCodeTable(pszInBuf, nInBytes, nCount);
	}
	return 0;
}
*/
int		MDllMgr::GetCodeTable(uint8_t cMarket, char* pszInBuf, int32_t nInBytes, int32_t& nCount)
{
	int nMarket = Global_XDFID2MarketID(cMarket);
	MData4XDll* pDll = GetMarketDll(nMarket);
	if (pDll)
	{
		return pDll->outer_GetCodeTable(pszInBuf, nInBytes, nCount);
	}
	return 0;
}

int		MDllMgr::GetLastMarketDataAll(uint8_t cMarket, char* pszInBuf, int32_t nInBytes)
{
	int nMarket = Global_XDFID2MarketID(cMarket);
	MData4XDll* pDll = GetMarketDll(nMarket);
	if (pDll)
	{
		return pDll->outer_GetLastMarketDataAll(pszInBuf, nInBytes);
	}
	return 0;
}

int	MDllMgr::GetMarketInfo(uint8_t cMarket, char* pszInBuf, int32_t nInBytes)
{
	int nMarket = Global_XDFID2MarketID(cMarket);
	MData4XDll* pDll = GetMarketDll(nMarket);

	if (pDll)
	{
		return pDll->outer_GetMarketInfo(pszInBuf, nInBytes);
	}
	return -1;
}

int	MDllMgr::GetSimpleMarketInfo(uint8_t cMarket, XDFAPI_MarketStatusInfo* pInfo)
{
	int nMarket = Global_XDFID2MarketID(cMarket);
	MData4XDll* pDll = GetMarketDll(nMarket);
	if (pDll)
	{
		return pDll->outer_GetSimpleMarketInfo(pInfo);
	}

	return -1;
}

int	MDllMgr::GetQuicksSimpleMarketInfo(uint8_t cMarket, XDFAPI_MarketStatusInfo* pInfo)
{
	int nMarket = Global_XDFID2MarketID(cMarket);
	MData4XDll* pDll = GetMarketDll(nMarket);
	if (pDll)
	{
		return pDll->outer_GetQuickSimpleMarketInfo(pInfo);
	}

	return -1;
}

int		MDllMgr::GetMarketStatus(uint8_t cMarket,int32_t& nStatus, uint32_t& ulTime, int64_t * pI64Send, int64_t * pI64Recv)
{
	int nMarket = Global_XDFID2MarketID(cMarket);
	MData4XDll* pDll = GetMarketDll(nMarket);
	if (pDll)
	{
		pDll->outer_GetStatus(ulTime, pI64Send, pI64Recv);
		nStatus = m_oWorkStat.GetMarketStat(nMarket);
		return 1;
	}
	
	return 0;
}

int		MDllMgr::GetMarketStat(unsigned char MarketId)
{
	return m_oWorkStat.GetMarketStat(MarketId);
}

bool	MDllMgr::outer_ExpectData(unsigned char MarketId,unsigned char MainType, unsigned char ChildType)
{
	MData4XDll* pDll = GetMarketDll(MarketId);
	if (pDll)
	{
		return pDll->outer_ExpectData(MainType, ChildType, MarketId);
	}
	return false;
}

void	MDllMgr::outer_OnPushData(unsigned char MarketId,unsigned char ChildType, const char* pszbuf, unsigned short bufsize)
{
	MData4XDll* pDll = GetMarketDll(MarketId);
	if (pDll)
	{
		pDll->outer_OnPushData(ChildType, pszbuf, bufsize);
	}
}

void	MDllMgr::outer_CloseSub(unsigned char MarketId)
{
	
}

void* STDCALL MDllMgr::TaskThreadFunc(void *pParam)
{
	MDllMgr * pSelf = (MDllMgr*)pParam;
	MCounter	cCounter;
	cCounter.SetCurTickCount();
	while (!pSelf->m_threadClient.GetThreadStopFlag())
    {
		try
		{
			//每隔1秒钟扫描一次缓冲区
			if (cCounter.GetDuration() >1000)
			{
				if (pSelf->m_bWorking)
				{
					pSelf->m_oWorkStat.CheckMarket();
				}
				Global_LogUnit.outer_CheckLog();
				cCounter.SetCurTickCount();
			}
			else
			{
				MThread::Sleep(100);	
			}
			
		}
		catch (...)
        {
            // 处理异常代码
            //Global_WriteLog(ERR, 0,"<TaskThreadFunc>线程出现未知异常");
        }
	}
	
	return 0;
}


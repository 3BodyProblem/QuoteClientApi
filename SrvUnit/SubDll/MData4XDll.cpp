#include "StdAfx.h"
#include "MData4XDll.h"
#include "../GlobalIO/GlobalIO.h"


MData4XDll::MData4XDll()
{
	m_p4XDll =0;
	m_uTime =0;
	m_i64Send =0;
	m_i64Recv =0;
}

MData4XDll::~MData4XDll()
{
}

void	MData4XDll::Bind4XDll(Base4XDll* pDll)
{
	m_p4XDll = pDll;
}

void MData4XDll::Reset()
{
	m_uTime =0;
	m_i64Send =0;
	m_i64Recv =0;
}

void MData4XDll::GetDllStatus( tagDll_DriverStatusInfo* pInfo )
{
	if( NULL == pInfo )
	{
		return;
	}

	if( NULL != m_p4XDll )
	{
		m_p4XDll->GetDllStatus( pInfo );
	}
}

void	MData4XDll::outer_GetStatus(uint32_t& ulTime, int64_t * pI64Send, int64_t * pI64Recv)
{
	*pI64Recv = m_i64Recv;
	*pI64Send = m_i64Send;
	ulTime = m_uTime;
}

int		MData4XDll::outer_GetLastMarketDataAll(char* pszInBuf, int32_t nInBytes)
{
	unsigned int MsgType =0;
	int sizeComStockData=0;
	int sizeXDFStockData=0;
	inner_GetStockInfo(MsgType, sizeComStockData, sizeXDFStockData);
	
	tagCcComm_MarketInfoHead			markethead;
	char								tempbuf[81920];
	tagCcComm_MarketInfoDetail * pMarketdetail = (tagCcComm_MarketInfoDetail *)tempbuf;

	int iret = m_p4XDll->GetMarketInfo( &markethead, pMarketdetail, sizeof(tempbuf)/sizeof(tagCcComm_MarketInfoDetail) );
	if (iret <0)
	{
		return iret;
	}

	MStreamWrite oMSW(pszInBuf, nInBytes);
	char	namebuf[2048]={0};

	int nCount = markethead.WareCount;
	int m=0;

	while( m < nCount)
	{
		memset(tempbuf, 0, sizeof(tempbuf));
		void * pData = (void *)tempbuf;
		iret = inner_GetStock(m, pData, sizeof(tempbuf)/sizeComStockData);
		if (iret <0)
		{
			break;
		}
		for (int i=0; i<iret; i++)
		{
			pData= (void*)(tempbuf+sizeComStockData*i);
			inner_TransStcok(pData, namebuf);
			oMSW.PutMsg(MsgType, namebuf, sizeXDFStockData);
		}
		
		m += iret;
	}

	oMSW.Detach();
	
	return oMSW.GetOffset();
}

int		MData4XDll::outer_GetSimpleMarketInfo(void* pInfo)
{
	tagCcComm_MarketInfoHead			markethead;
	char								tempbuf[81920];
	tagCcComm_MarketInfoDetail * pMarketdetail = (tagCcComm_MarketInfoDetail *)tempbuf;
	int iret = m_p4XDll->GetMarketInfo( &markethead, pMarketdetail, sizeof(tempbuf)/sizeof(tagCcComm_MarketInfoDetail) );
	if (iret <0)
	{
		return iret;
	}

	XDFAPI_MarketStatusInfo * pMInfo = (XDFAPI_MarketStatusInfo*)pInfo;
	pMInfo->MarketDate = markethead.Date;
	pMInfo->MarketTime = markethead.Time;
	return 1;
}

int		MData4XDll::outer_GetQuickSimpleMarketInfo(void* pInfo)
{
	tagCcComm_MarketInfoHead			markethead;
	char								tempbuf[81920];
	tagCcComm_MarketInfoDetail * pMarketdetail = (tagCcComm_MarketInfoDetail *)tempbuf;
	int iret = m_p4XDll->GetQuickMarketInfo( &markethead, pMarketdetail, sizeof(tempbuf)/sizeof(tagCcComm_MarketInfoDetail) );
	if (iret <0)
	{
		return iret;
	}

	XDFAPI_MarketStatusInfo * pMInfo = (XDFAPI_MarketStatusInfo*)pInfo;
	pMInfo->MarketDate = markethead.Date;
	pMInfo->MarketTime = markethead.Time;
	return 1;
}

int		MData4XDll::outer_GetMarketInfo(char* pszInBuf, int32_t nInBytes)
{
	tagCcComm_MarketInfoHead			markethead;
	char								tempbuf[81920];
	tagCcComm_MarketInfoDetail * pMarketdetail = (tagCcComm_MarketInfoDetail *)tempbuf;
	int iret = m_p4XDll->GetMarketInfo( &markethead, pMarketdetail, sizeof(tempbuf)/sizeof(tagCcComm_MarketInfoDetail) );
	if (iret <0)
	{
		return iret;
	}

	MStreamWrite oMSW(pszInBuf, nInBytes);

	XDFAPI_MarketKindHead oHead;
	oHead.WareCount = markethead.WareCount;
	oHead.KindCount = markethead.KindCount;

	oMSW.PutSingleMsg(100, (char*)&oHead, sizeof(oHead));
	int m =0;
	while(m < markethead.KindCount)
	{
		XDFAPI_MarketKindInfo oInfo;
		memset(&oInfo, 0, sizeof(oInfo));
		oInfo.Serial = m;
		memcpy(oInfo.KindName, pMarketdetail->KindName, 8);
		oInfo.PriceRate = pMarketdetail->PriceRate;
		oInfo.LotSize = pMarketdetail->LotSize;
		oInfo.WareCount = pMarketdetail->WareCount;
		oMSW.PutMsg(101, (char*)&oInfo, sizeof(oInfo) );

		pMarketdetail++;
		m++;
	}

	oMSW.Detach();
	return oMSW.GetOffset();
}

int MData4XDll::outer_GetCodeTableEx( char* pszInBuf, int32_t nInBytes, int32_t& nCount )
{
	unsigned int MsgType =0;
	int sizeComNameTable=0;
	int sizeXDFNameTable=0;
	inner_GetNameTableInfoEx(MsgType, sizeComNameTable, sizeXDFNameTable);

	if( 0 == sizeComNameTable && 0 == sizeXDFNameTable ) {
		return 0;
	}

	tagCcComm_MarketInfoHead			markethead;
	char								tempbuf[81920];
	tagCcComm_MarketInfoDetail * pMarketdetail = (tagCcComm_MarketInfoDetail *)tempbuf;
	int iret = m_p4XDll->GetMarketInfo( &markethead, pMarketdetail, sizeof(tempbuf)/sizeof(tagCcComm_MarketInfoDetail) );
	if (iret <0)
	{
		return iret;
	}

	nCount = markethead.WareCount;
	if (0 == pszInBuf || 0 == nInBytes)		//通过nCount返回码表个数
	{
		return 1;
	}

	MStreamWrite oMSW(pszInBuf, nInBytes);
	char	namebuf[2048]={0};

	int m=0;
	while (m < nCount)
	{
		memset(tempbuf, 0, sizeof(tempbuf));
		void * pNameTb = (void *)tempbuf;
		iret = inner_GetNameTable(m, pNameTb, sizeof(tempbuf)/sizeComNameTable);
		if (iret <0)
		{
			return -1;
		}
		for (int i=0; i<iret; i++)
		{
			pNameTb= (void*)(tempbuf+sizeComNameTable*i);
			inner_TransNameTableEx(pNameTb, namebuf);
			oMSW.PutMsg(MsgType, namebuf, sizeXDFNameTable);
		}

		m += iret;
	}
	oMSW.Detach();
	return oMSW.GetOffset();
}

int		MData4XDll::outer_GetCodeTable(char* pszInBuf, int32_t nInBytes, int32_t& nCount)
{
	unsigned int MsgType =0;
	int sizeComNameTable=0;
	int sizeXDFNameTable=0;
	inner_GetNameTableInfo(MsgType, sizeComNameTable, sizeXDFNameTable);
	
	tagCcComm_MarketInfoHead			markethead;
	char								tempbuf[81920];
	tagCcComm_MarketInfoDetail * pMarketdetail = (tagCcComm_MarketInfoDetail *)tempbuf;
	int iret = m_p4XDll->GetMarketInfo( &markethead, pMarketdetail, sizeof(tempbuf)/sizeof(tagCcComm_MarketInfoDetail) );
	if (iret <0)
	{
		return iret;
	}

	nCount = markethead.WareCount;
	if (0 == pszInBuf || 0 == nInBytes)		//通过nCount返回码表个数
	{
		return 1;
	}
	
	MStreamWrite oMSW(pszInBuf, nInBytes);
	char	namebuf[2048]={0};

	int m=0;
	while (m < nCount)
	{
		memset(tempbuf, 0, sizeof(tempbuf));
		void * pNameTb = (void *)tempbuf;
		iret = inner_GetNameTable(m, pNameTb, sizeof(tempbuf)/sizeComNameTable);
		if (iret <0)
		{
			return -1;
		}
		for (int i=0; i<iret; i++)
		{
			pNameTb= (void*)(tempbuf+sizeComNameTable*i);
			inner_TransNameTable(pNameTb, namebuf);
			oMSW.PutMsg(MsgType, namebuf, sizeXDFNameTable);
		}

		m += iret;
	}
	oMSW.Detach();
	return oMSW.GetOffset();
}

//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
MStreamWrite::MStreamWrite(char* pszinbuf, int ninbytes)
{
	m_pInBuf = pszinbuf;
	m_nInBytes = ninbytes;

	m_nLastType =0;
	m_nMsgCount =0;
	m_nOffset =0;
	m_bError =false;
	m_pUniHead =0;
}

MStreamWrite::MStreamWrite()
{
	m_pInBuf =0;
	m_nInBytes =0;
	m_nLastType =0;
	m_nMsgCount =0;
	m_nOffset =0;
	m_bError =false;
	m_pUniHead =0;
}

MStreamWrite::~MStreamWrite()
{
	
}

void	MStreamWrite::Attach(char* pszinbuf, int ninbytes)
{
	m_pInBuf = pszinbuf;
	m_nInBytes = ninbytes;
	
	m_nLastType =0;
	m_nMsgCount =0;
	m_nOffset =0;
	m_bError =false;
	m_pUniHead =0;
}

bool	MStreamWrite::IsError()
{
	return m_bError;
}
int		MStreamWrite::GetOffset()
{
	return m_nOffset;
}
int		MStreamWrite::GetCount()
{
	return m_nMsgCount;
}

void	MStreamWrite::Detach()
{
	//do nothing now!!
}

void	MStreamWrite::PutSingleMsg(unsigned int ntype, char* pStruct, int nStructSize)
{
	if (m_bError)		//错误已出现
		return;
	
	int noffset = nStructSize + sizeof(XDFAPI_MsgHead);
	if (m_nInBytes - m_nOffset < noffset)	//容量超了
	{
		m_bError =true;
		return;
	}
	
	char* pChar = m_pInBuf+m_nOffset;
	XDFAPI_MsgHead* pHead = (XDFAPI_MsgHead*)(pChar);
	pHead->MsgType = ntype;
	pHead->MsgLen = nStructSize;
	memcpy(pChar+sizeof(XDFAPI_MsgHead), pStruct, nStructSize);

	m_nMsgCount += 1;
	m_nLastType =0;
	m_nOffset += noffset;
	m_pUniHead =0;
}

void	MStreamWrite::PutMsg(unsigned int ntype, char* pStruct, int nStructSize)
{
	if (m_bError)	//错误已出现
		return;
	
	//[1]
	int realoffset = nStructSize;
	bool breuse =false;
	//检查是否m_pUniHead为0
	//检查MsgLen是否越界
	//检查类型和LastType是否一致
	if (0 == m_pUniHead || m_pUniHead->MsgLen >64000 || m_nLastType!= ntype)
	{
		realoffset += sizeof(XDFAPI_UniMsgHead);
		breuse =false;
	}
	else
	{
		breuse =true;
	}

	//[2]容量检查
	if (m_nInBytes - m_nOffset < realoffset) //容量超了
	{
		m_bError =true;
		return;
	}

	if (!breuse)
	{
		char* pChar = m_pInBuf+m_nOffset;
		m_pUniHead = (XDFAPI_UniMsgHead*)pChar;
		m_pUniHead->MsgType = (-1)*ntype;
		m_pUniHead->MsgCount=1;
		m_pUniHead->MsgLen = nStructSize + sizeof(int16_t);
		memcpy(pChar+sizeof(XDFAPI_UniMsgHead), pStruct, nStructSize);
		
	}
	else
	{
		m_pUniHead->MsgCount += 1;
		m_pUniHead->MsgLen += nStructSize;
		char* pChar = m_pInBuf+m_nOffset;
		memcpy(pChar, pStruct, nStructSize);
	}

	m_nLastType = ntype;
	m_nMsgCount += 1;
	m_nOffset += realoffset;
}





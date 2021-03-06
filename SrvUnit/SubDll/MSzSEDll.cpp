
#include "StdAfx.h"
#include "MSzSEDll.h"
#include "../GlobalIO/GlobalIO.h"

//[2, 2]码表
//[2, 6]指数
//[2, 7]个股
//[2, 50]	//0 -- 市场状态  1 -- 指数快照  2 -- 个股快照	3 -- 前缀变化 
//[2, 9]	//0 -- 停牌标志

//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
MSzSEDll::MSzSEDll()
{
	
}

MSzSEDll::~MSzSEDll()
{
	
}

//除了个股，还有指数
int		MSzSEDll::outer_GetLastMarketDataAll(char* pszInBuf, int32_t nInBytes)
{
	tagCcComm_MarketInfoHead			markethead;
	char								tempbuf[81920];
	tagCcComm_MarketInfoDetail * pMarketdetail = (tagCcComm_MarketInfoDetail *)tempbuf;
	
	int iret = m_p4XDll->GetMarketInfo( &markethead, pMarketdetail, sizeof(tempbuf)/sizeof(tagCcComm_MarketInfoDetail) );
	if (iret <0)
	{
		return iret;
	}
	int nCount = markethead.WareCount;
	int nIndexCount = pMarketdetail->WareCount;
	int nStockCount = nCount - nIndexCount;
	
	MStreamWrite oMSW(pszInBuf, nInBytes);
	iret = inner_getLastIndexData( nIndexCount, oMSW);
	if (iret <0)
	{
		oMSW.Detach();
		return oMSW.GetOffset();
	}
	
	iret = inner_getLastStockData( nStockCount, oMSW);
	if (iret >0)
	{
		oMSW.Detach();
		return oMSW.GetOffset();
	}
	
	oMSW.Detach();
	return -2;
}

//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
//返回偏移
int		MSzSEDll::inner_getLastIndexData(int nIndexCount, MStreamWrite& oMSW)
{
	unsigned int MsgType =0;
	int sizeComIndexData=0;
	int sizeXDFIndexData=0;
	inner_GetIndexInfo(MsgType, sizeComIndexData, sizeXDFIndexData);
	
	int m=0;
	int iret =0;
	char		tempbuf[81920];
	char		namebuf[2048];
	
	while (m < nIndexCount)
	{
		memset(tempbuf, 0, sizeof(tempbuf));
		void * pData = (void *)tempbuf;
		iret = inner_GetIndex(m, pData, sizeof(tempbuf)/sizeComIndexData);
		if (iret <0)
		{
			break;
		}
		for (int i=0; i<iret; i++)
		{
			pData= (void*)(tempbuf+sizeComIndexData*i);
			inner_TransIndexData(pData, namebuf);
			oMSW.PutMsg(MsgType, namebuf, sizeXDFIndexData );
		}
		
		m += iret;
	}
	
	return 1;
}

//返回偏移
int		MSzSEDll::inner_getLastStockData(int nStockCount, MStreamWrite& oMSW)
{
	unsigned int MsgType =0;
	int sizeComStockData=0;
	int sizeXDFStockData=0;
	inner_GetStockInfo(MsgType, sizeComStockData, sizeXDFStockData);

	int m=0;
	int iret = 0;
	char		tempbuf[81920];
	char		namebuf[2048];
	
	while (m < nStockCount)
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
	
	return (1);
}

int	MSzSEDll::inner_getStopFlagData(int nStockCount, MStreamWrite& oMSW)
{
	unsigned int MsgType =0;
	int sizeComStockData=0;
	int sizeXDFStockData=0;
	inner_GetStopFlag(MsgType, sizeComStockData, sizeXDFStockData);

	int m=0;
	int iret = 0;
	char		tempbuf[81920];
	char		namebuf[2048];

	while (m < nStockCount)
	{
		memset(tempbuf, 0, sizeof(tempbuf));
		void * pData = (void *)tempbuf;
		iret = inner_GetNameTableEx(m, pData, sizeof(tempbuf)/sizeComStockData);
		if (iret <0)
		{
			break;
		}
		for (int i=0; i<iret; i++)
		{
			pData= (void*)(tempbuf+sizeComStockData*i);
			inner_TransStopFlag(pData, namebuf);
			oMSW.PutMsg(MsgType, namebuf, sizeXDFStockData);
		}

		m += iret;
	}

	return (1);
}


//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
void	MSzSEDll::inner_GetStopFlag(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData)
{
	MsgType = 23;
	sizeComStockData = sizeof(tagCcComm_NameTableEx);
	sizeXDFStockData = sizeof(XDFAPI_StopFlag);
}
void	MSzSEDll::inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData)
{
	MsgType = 22;
	sizeComStockData = sizeof(tagCcComm_StockData5);
	sizeXDFStockData = sizeof(XDFAPI_StockData5);
}
void	MSzSEDll::inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable)
{
	MsgType = 6;
	sizeComNameTable = sizeof(tagCcComm_ShNameTable);
	sizeXDFNameTable = sizeof(XDFAPI_NameTableSz);
}
void	MSzSEDll::inner_GetIndexInfo(unsigned int& MsgType, int& sizeComIndexData, int& sizeXDFIndexData)
{
	MsgType = 21;
	sizeComIndexData = sizeof(tagCcComm_IndexData);
	sizeXDFIndexData = sizeof(XDFAPI_IndexData);
}

//获取码表[2,2]
int		MSzSEDll::inner_GetNameTable(unsigned short dwStartNo, void * pValue, int nCount)
{
	assert( pValue != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 2, dwStartNo, pValue, nCount*sizeof(tagCcComm_ShNameTable), sizeof(tagCcComm_ShNameTable) );
}
//获取扩展码表[2,8]
int		MSzSEDll::inner_GetNameTableEx(unsigned short dwStartNo, void * pValue, int nCount)
{
	assert( pValue != NULL && nCount > 0 );

	return m_p4XDll->GetData( 2, 8, dwStartNo, pValue, nCount*sizeof(tagCcComm_NameTableEx), sizeof(tagCcComm_NameTableEx) );
}
//获取个股行情[2,7]
int		MSzSEDll::inner_GetStock(unsigned short dwStartNo, void * pValue, int nCount)
{
	assert( pValue != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 7, dwStartNo, pValue, nCount*sizeof(tagCcComm_StockData5), sizeof(tagCcComm_StockData5) );
}
//获取指数行情[2,6]
int		MSzSEDll::inner_GetIndex(unsigned short dwStartNo, void * pValue, int nCount)
{
	assert( pValue != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 6, dwStartNo, pValue, nCount*sizeof(tagCcComm_IndexData), sizeof(tagCcComm_IndexData) );
}

int		MSzSEDll::inner_TransIndexData(void* pValue, char* poutbuf)
{
	tagCcComm_IndexData * pIndexTb = (tagCcComm_IndexData*)pValue;
	
	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_IndexData);
	XDFAPI_IndexData* pTable = (XDFAPI_IndexData*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_IndexData));
	memcpy(pTable->Code,pIndexTb->Code,6);
	pTable->High = pIndexTb->High;
	pTable->Open = pIndexTb->Open;
	pTable->Low = pIndexTb->Low;
	pTable->PreClose = pIndexTb->Close;
	pTable->Now = pIndexTb->Now;
	pTable->Amount = pIndexTb->Amount;
	pTable->Volume = pIndexTb->Volume;
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset;
}

int		MSzSEDll::inner_TransStopFlag(void* pValue, char* poutbuf)
{
	char					tempbuf[128] = { 0 };
	int						noffset = sizeof(XDFAPI_StopFlag);
	XDFAPI_StopFlag*		pTable = (XDFAPI_StopFlag*)(tempbuf);
	tagCcComm_NameTableEx*	pData = (tagCcComm_NameTableEx*)pValue;

	memcpy(pTable->Code, pData->Code,6);
	pTable->SFlag = pData->SFlag;
	memcpy(poutbuf, tempbuf, noffset);

	return noffset ;
}

int		MSzSEDll::inner_TransNameTable(void* pValue, char* poutbuf)
{
	tagCcComm_ShNameTable * pNameTb = (tagCcComm_ShNameTable*)pValue;
	
	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_NameTableSz);
	XDFAPI_NameTableSz* pTable = (XDFAPI_NameTableSz*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_NameTableSz));
	pTable->Market = XDF_SZ;
	memcpy(pTable->Code, pNameTb->Code, 6);
	memcpy(pTable->Name, pNameTb->Name, 8);
	pTable->SecKind = pNameTb->Type;
	memcpy(pTable->PreName, pNameTb->PreName,4);
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}

int		MSzSEDll::inner_TransStcok(void* pValue, char* poutbuf)
{
	tagCcComm_StockData5* pData = (tagCcComm_StockData5*)pValue;
	
	char tempbuf[1000]={0};
	int noffset = sizeof(XDFAPI_StockData5);
	XDFAPI_StockData5* pTable = (XDFAPI_StockData5*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_StockData5));
	memcpy(pTable->Code, pData->Code,6);
	pTable->High = pData->High;
	pTable->Open = pData->Open;
	pTable->Low = pData->Low;
	pTable->PreClose = pData->Close;
	pTable->Now = pData->Now;
	pTable->Amount = pData->Amount;
	pTable->Volume = pData->Volume;
	pTable->Records = pData->Records;
	pTable->HighLimit = pData->HighLimit;
	pTable->LowLimit = pData->LowLimit;
	pTable->Voip = pData->Voip;

	for (int i=0; i<5; i++)
	{
		pTable->Buy[i].Price = pData->Buy[i].Price;
		pTable->Buy[i].Volume= pData->Buy[i].Volume;
		pTable->Sell[i].Price= pData->Sell[i].Price;
		pTable->Sell[i].Volume=pData->Sell[i].Volume;
	}
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset;
}

bool	MSzSEDll::inner_CompareStock(void* pData, char* pCode)
{
	tagCcComm_StockData5* pStock = (tagCcComm_StockData5*)pData;
	if (_strnicmp(pStock->Code, pCode, 6) ==0)
	{
		return true;
	}
	return false;
}

bool	MSzSEDll::inner_IndexCompare(void* pData, char* pCode)
{
	tagCcComm_IndexData* pIndex = (tagCcComm_IndexData*)pData;
	if (_strnicmp(pIndex->Code, pCode, 6) ==0)
	{
		return true;
	}
	return false;
}

//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................
//.................................................................................................................................................................

bool	MSzSEDll::outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID)
{
	//期望市场值是1
	if (MarketID == 1 && MainType ==2)
	{
		if ( ChildType == 50 || ChildType ==9)
		{
			return true;
		}
	}
	return false;
}

void	MSzSEDll::outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize)
{
	char				tempbuf[2048]={ 0 };
	char				outbuf[81920]={ 0 } ;
	MStreamWrite		oMSW( outbuf, 81920 );
	tagCcComm_TimeInfo*	pTime = (tagCcComm_TimeInfo*)pszbuf;

	//[1]//更新市场时间,更新iRecv
	m_i64Recv += bufsize;

	if( pTime->CurTime != m_uTime )
	{
		XDFAPI_MarketStatusInfo		tagStatus = { 0 };

		tagStatus.MarketID = XDF_SH;
		tagStatus.MarketTime = pTime->CurTime;
		oMSW.PutSingleMsg(1, (char*)&tagStatus, sizeof(XDFAPI_MarketStatusInfo));
	}

	m_uTime = pTime->CurTime;

	int nLen = bufsize;
	char * pbuf = (char *)pszbuf;
	pbuf += sizeof(tagCcComm_TimeInfo);
	nLen -= sizeof(tagCcComm_TimeInfo);
	
	if (ChildType ==9)
	{
		unsigned char cType = 0;
		while(nLen >0)
		{
			cType = *pbuf;
			pbuf += sizeof(char);
			nLen -= sizeof(char);

			switch (cType)
			{
			case 0:
				{
					tagCcComm_StopFlag* pFlag = (tagCcComm_StopFlag *)pbuf;
					inner_TransStopFlag( pFlag, tempbuf);
					oMSW.PutSingleMsg(23, tempbuf, sizeof(XDFAPI_StopFlag));
					
					pbuf += sizeof(tagCcComm_StopFlag);
					nLen -= sizeof(tagCcComm_StopFlag);
				}
				break;
			default:
				{
					//return;	//如果出现未知类型,请检查上述代码,或DLL版本等信息
					nLen =0;	//强制退出while()
				}
				break;
			}
		}
	}
	else if (ChildType ==50)
	{
		unsigned char cType = 0;
		while(nLen >0)
		{
			cType = *pbuf;
			pbuf += sizeof(char);
			nLen -= sizeof(char);
			
			switch (cType)
			{
			case 0:
				{
					tagCcComm_MarketStatusInfo * pMarketInfo = (tagCcComm_MarketStatusInfo *)pbuf;
					inner_TransMarketStatus(pMarketInfo, tempbuf);
					oMSW.PutSingleMsg(1, tempbuf, sizeof(XDFAPI_MarketStatusInfo));

					pbuf += sizeof(tagCcComm_MarketStatusInfo);
					nLen -= sizeof(tagCcComm_MarketStatusInfo);
				}
				break;
			case 1:
				{
					tagCcComm_IndexData * pIndex = (tagCcComm_IndexData*)pbuf;
					inner_TransIndexData( pIndex, tempbuf);
					oMSW.PutMsg(21, tempbuf, sizeof(XDFAPI_IndexData));
					Global_QueryClient.OnSZL1Index( m_uTime, *pIndex );

					pbuf += sizeof(tagCcComm_IndexData);
					nLen -= sizeof(tagCcComm_IndexData);
				}
				break;
			case 2:
				{
					tagCcComm_StockData5* pData = (tagCcComm_StockData5*)pbuf;
					inner_TransStcok(pData, tempbuf);
					oMSW.PutMsg(22, tempbuf, sizeof(XDFAPI_StockData5) );
					Global_QueryClient.OnSZL1Snap( m_uTime, *pData );

					pbuf += sizeof(tagCcComm_StockData5);
					nLen -= sizeof(tagCcComm_StockData5);
				}
				break;
			case 3:
				{
					tagCcComm_SzPerNameChg * pData = (tagCcComm_SzPerNameChg*)pbuf;
					inner_TransPreNameChg(pData, tempbuf);
					oMSW.PutMsg(24, tempbuf, sizeof(XDFAPI_PreNameChg));
					
					pbuf += sizeof(tagCcComm_SzPerNameChg);
					nLen -= sizeof(tagCcComm_SzPerNameChg);
				}
				break;
			default:
				{
					//此处可以输出日志或其他提示
					//return;	//如果出现未知类型,请检查上述代码,或DLL版本等信息
					nLen =0;	//强制退出while()
				}
				break;
			}
			
		}
	}

	oMSW.Detach();

	//[3]放入池子(PkgHead+PkgBuf)
	XDFAPI_PkgHead pkghead;
	memset(&pkghead, 0, sizeof(XDFAPI_PkgHead));
	pkghead.MarketID = XDF_SZ;
	pkghead.PkgSize = oMSW.GetOffset() ;
	
	Global_DataIO.PutData( &pkghead, outbuf, oMSW.GetOffset() );
}

int		MSzSEDll::inner_TransMarketStatus(void* pData,  char* poutbuf)
{
	tagCcComm_MarketStatusInfo* pStatus = (tagCcComm_MarketStatusInfo*)pData;
	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_MarketStatusInfo);
	
	XDFAPI_MarketStatusInfo* pTable = (XDFAPI_MarketStatusInfo*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_MarketStatusInfo));
	pTable->MarketDate = pStatus->MarketDate;
	pTable->MarketTime = pStatus->MarketTime;
	pTable->MarketID = XDF_SZ;
	pTable->MarketStatus = pStatus->MarketStatus;
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}

//MsgType=24
int	MSzSEDll::inner_TransPreNameChg(void* pData, char* poutbuf)
{
	tagCcComm_SzPerNameChg* pChg = (tagCcComm_SzPerNameChg*)pData;
	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_PreNameChg);

	XDFAPI_PreNameChg* pTable = (XDFAPI_PreNameChg*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_PreNameChg));
	memcpy(pTable->Code, pChg->Code,6);
	memcpy(pTable->PreName, pChg->PreName,4);
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}
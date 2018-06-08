#include "StdAfx.h"
#include "MSZL2Dll.h"
#include "../GlobalIO/GlobalIO.h"

//[2, 2]码表
//[2, 6]指数
//[2, 13]个股
//[2, 59]	//0 -市场信息  1 -指数 2 -个股
MSZL2Dll::MSZL2Dll()
{

}

MSZL2Dll::~MSZL2Dll()
{

}

int		MSZL2Dll::outer_GetLastMarketDataAll(char* pszInBuf, int32_t nInBytes)	//除了个股，还有指数
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

int		MSZL2Dll::inner_getLastStockData(int nStockCount, MStreamWrite& oMSW)
{
	unsigned int MsgType =0;
	int sizeComStockData=0;
	int sizeXDFStockData=0;
	inner_GetStockInfo(MsgType, sizeComStockData, sizeXDFStockData);
	
	int m=0;
	int iret = 0;
	int i=0;
	char		tempbuf[81920];
	char		namebuf[2048];
	
	while (m < nStockCount)
	{
		memset(tempbuf, 0, sizeof(tempbuf));
		void * pData = (void *)tempbuf;
		iret = inner_GetStock(m, pData, 81920/sizeComStockData);
		if (iret <0)
		{
			break;
		}
		for (i=0; i<iret; i++)
		{
			pData= (void*)(tempbuf+sizeComStockData*i);
			inner_TransStcok(pData, namebuf);
			oMSW.PutMsg(MsgType, namebuf, sizeXDFStockData);
		}
		
		m += iret;
	}
	
	return (1);
}

int		MSZL2Dll::inner_getLastIndexData(int nIndexCount, MStreamWrite& oMSW)
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


void	MSZL2Dll::inner_GetIndexInfo(unsigned int& MsgType, int& sizeComIndexData, int& sizeXDFIndexData)
{
	MsgType = 21;
	sizeComIndexData = sizeof(tagCcComm_IndexData);
	sizeXDFIndexData = sizeof(XDFAPI_IndexData);
}
void	MSZL2Dll::inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData)
{
	MsgType = 30;
	sizeComStockData = sizeof(tagCcComm_StockData10);
	sizeXDFStockData = sizeof(XDFAPI_StockData10);
}
void	MSZL2Dll::inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable)
{
	MsgType = 5;
	sizeComNameTable = sizeof(tagCcComm_ShNameTable);
	sizeXDFNameTable = sizeof(XDFAPI_NameTableSh);
}

int		MSZL2Dll::inner_GetNameTable(unsigned short dwStartNo, void * pValue, int nCount)
{
	assert( pValue != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 2, dwStartNo, pValue, nCount*sizeof(tagCcComm_ShNameTable), sizeof(tagCcComm_ShNameTable) );
}

int		MSZL2Dll::inner_GetStock(unsigned short dwStartNo, void * pValue, int nCount)
{
	assert( pValue != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 13, dwStartNo, pValue, nCount*sizeof(tagCcComm_StockData10), sizeof(tagCcComm_StockData10) );
}

int		MSZL2Dll::inner_GetIndex(unsigned short dwStartNo, void * pValue, int nCount)
{
	assert( pValue != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 6, dwStartNo, pValue, nCount*sizeof(tagCcComm_IndexData), sizeof(tagCcComm_IndexData) );
}

int		MSZL2Dll::inner_TransIndexData(void* pData, char* poutbuf)
{
	tagCcComm_IndexData * pIndexTb = (tagCcComm_IndexData*)pData;
	
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
	return noffset ;
}

int		MSZL2Dll::inner_TransNameTable( void * pData, char* poutbuf)
{
	tagCcComm_ShNameTable * pNameTb = (tagCcComm_ShNameTable*)pData;
	
	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_NameTableSh);
	XDFAPI_NameTableSh* pTable = (XDFAPI_NameTableSh*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_NameTableSh));
	pTable->Market = XDF_SZL2;
	memcpy(pTable->Code, pNameTb->Code, 6);
	memcpy(pTable->Name, pNameTb->Name, 8);
	pTable->SecKind = pNameTb->Type;
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}

int		MSZL2Dll::inner_TransStcok(void* pValue, char* poutbuf)
{
	tagCcComm_StockData10* pData = (tagCcComm_StockData10*)pValue;
	
	char tempbuf[1000]={0};
	int noffset = sizeof(XDFAPI_StockData10);
	XDFAPI_StockData10* pTable = (XDFAPI_StockData10*)(tempbuf);
	
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
	
	for (int i=0; i<10; i++)
	{
		pTable->Buy[i].Price = pData->Buy[i].Price;
		pTable->Buy[i].Volume= pData->Buy[i].Volume;
		pTable->Sell[i].Price= pData->Sell[i].Price;
		pTable->Sell[i].Volume=pData->Sell[i].Volume;
	}
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset;
}

bool	MSZL2Dll::inner_CompareStock(void* pData, char* pCode)
{
	tagCcComm_StockData10* pStock = (tagCcComm_StockData10*)pData;
	if (_strnicmp(pStock->Code, pCode, 6) ==0)
	{
		return true;
	}
	return false;
}

bool	MSZL2Dll::inner_IndexCompare(void* pData, char* pCode)
{
	tagCcComm_IndexData* pIndex = (tagCcComm_IndexData*)pData;
	if (_strnicmp(pIndex->Code, pCode, 6) ==0)
	{
		return true;
	}
	return false;
}


int		MSZL2Dll::inner_TransMarketStatus(void* pData, char* poutbuf)
{
	tagCcComm_MarketStatusInfo* pStatus = (tagCcComm_MarketStatusInfo*)pData;
	
	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_MarketStatusInfo);
	XDFAPI_MarketStatusInfo* pTable = (XDFAPI_MarketStatusInfo*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_MarketStatusInfo));
	pTable->MarketDate = pStatus->MarketDate;
	pTable->MarketTime = pStatus->MarketTime;
	pTable->MarketID = XDF_SZL2;
	pTable->MarketStatus = pStatus->MarketStatus;
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}


bool	MSZL2Dll::outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID)
{
	if (MarketID == XDF_SZL2 && MainType ==2)
	{
		if (ChildType == 59)
		{
			return true;
		}
	}
	return false;
}

//[2，59]  市场信息-0   指数数据-1   商品快照-2
void	MSZL2Dll::outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize)
{
	//[1]//更新市场时间,更新iRecv
	m_i64Recv += bufsize;
	
	tagCcComm_TimeInfo* pTime = (tagCcComm_TimeInfo*)pszbuf;
	m_uTime = pTime->CurTime;
	
	//[2]转换格式
	char tempbuf[2048]={0};
	char outbuf[81920]={0};
	MStreamWrite oMSW(outbuf, 81920);
	
	int nLen = bufsize;
	char * pbuf = (char *)pszbuf;
	pbuf += sizeof(tagCcComm_TimeInfo);
	nLen -= sizeof(tagCcComm_TimeInfo);
	
	if (ChildType == 59)
	{
		unsigned char cType = 0;
		while(nLen >0)
		{
			cType = *pbuf;
			pbuf += sizeof(char);
			nLen -= sizeof(char);
			
			switch (cType)
			{
			case 0:		//市场状态
				{
					tagCcComm_MarketStatusInfo * pMarketInfo = (tagCcComm_MarketStatusInfo *)pbuf;
					inner_TransMarketStatus(pMarketInfo, tempbuf);
					oMSW.PutSingleMsg(1, tempbuf, sizeof(XDFAPI_MarketStatusInfo));
					
					pbuf += sizeof(tagCcComm_MarketStatusInfo);
					nLen -= sizeof(tagCcComm_MarketStatusInfo);
				}
				break;
			case 1:		//指数快照
				{
					tagCcComm_IndexData * pIndex = (tagCcComm_IndexData*)pbuf;
					inner_TransIndexData(pIndex, tempbuf);
					oMSW.PutMsg(21, tempbuf, sizeof(XDFAPI_IndexData) );
					
					pbuf += sizeof(tagCcComm_IndexData);
					nLen -= sizeof(tagCcComm_IndexData);
				}
				break;
			case 2:	//个股快照
				{
					tagCcComm_StockData10* pData = (tagCcComm_StockData10*)pbuf;
					inner_TransStcok(pData, tempbuf);
					oMSW.PutMsg(30, tempbuf,sizeof(XDFAPI_StockData10) );
				
					pbuf += sizeof(tagCcComm_StockData10);
					nLen -= sizeof(tagCcComm_StockData10);
				}
				break;
			case 3:
				{
					pbuf += sizeof(tagCcComm_SzL2TransData);
					nLen -= sizeof(tagCcComm_SzL2TransData);
				}
				break;
			case 4:
				{
					pbuf += sizeof(tagCcComm_SzL2OderData);
					nLen -= sizeof(tagCcComm_SzL2OderData);
				}
				break;
			case 5:
				{
					int ndetail = sizeof(tagCcComm_OrderVolDetail);
					int nemit = sizeof(tagCcComm_OrderQueueHeadSzL2);

					tagCcComm_OrderQueueHeadSzL2 * pPrice = (tagCcComm_OrderQueueHeadSzL2*)pbuf;
					int nmincount = min( pPrice->VolCount, 50);
					int nminoffset = nemit + ndetail*nmincount;
					
					pbuf += nminoffset;
					nLen -= nminoffset;
				}
				break;
			case 6:
				{
					pbuf += sizeof(tagCcComm_SzPerNameChg);
					nLen -= sizeof(tagCcComm_SzPerNameChg);
				}
				break;
			case 7:
				{
					pbuf += sizeof(tagCcComm_Gzlx);
					nLen -= sizeof(tagCcComm_Gzlx);
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
	pkghead.MarketID = XDF_SZL2;
	pkghead.PkgSize = oMSW.GetOffset();
	
	Global_DataIO.PutData( &pkghead, outbuf, oMSW.GetOffset() );
	
}

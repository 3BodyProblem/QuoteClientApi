


#include "StdAfx.h"
#include "MCnfOptDll.h"
#include "../GlobalIO/GlobalIO.h"

MCnfOptDll::MCnfOptDll()
{

}

MCnfOptDll::~MCnfOptDll()
{

}

void	MCnfOptDll::inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData)
{
	MsgType = 34;
	sizeComStockData = sizeof(tagCcComm_CsfexFutOptStockData);
	sizeXDFStockData = sizeof(XDFAPI_CNFutOptData);
}

void	MCnfOptDll::inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable)
{
	MsgType = 11;
	sizeComNameTable = sizeof(tagCcComm_CsfexFutOptNameTable);
	sizeXDFNameTable = sizeof(XDFAPI_NameTableCnfOpt);
}

bool	MCnfOptDll::inner_CompareStock(void* pData, char* pCode)
{
	tagCcComm_CsfexFutOptStockData* pStock = (tagCcComm_CsfexFutOptStockData*)pData;
	if (_strnicmp(pStock->Code, pCode, 20) ==0)
	{
		return true;
	}
	return false;
}

//获取码表[2,201]
int		MCnfOptDll::inner_GetNameTable(unsigned short dwStartNo, void * pData, int nCount)
{
	assert( pData != NULL && nCount > 0 );

	return m_p4XDll->GetData( 2, 105, dwStartNo, pData, nCount*sizeof(tagCcComm_CsfexFutOptNameTable), sizeof(tagCcComm_CsfexFutOptNameTable) );
}

//获取个股行情[2,202]
int		MCnfOptDll::inner_GetStock(unsigned short dwStartNo, void * pData, int nCount)
{
	assert( pData != NULL && nCount > 0 );

	return m_p4XDll->GetData( 2, 202, dwStartNo, pData, nCount*sizeof(tagCcComm_CsfexFutOptStockData), sizeof(tagCcComm_CsfexFutOptStockData) );
}

bool	MCnfOptDll::outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID)
{
	//期望市场值是35
	if (MarketID == 35)
	{
		if (MainType ==2 && ChildType ==204 )
		{
			return true;
		}
	}
	return false;
}


void	MCnfOptDll::outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize)
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
				tagCcComm_CsfexFutOptStockData* pData = (tagCcComm_CsfexFutOptStockData*)pbuf;
				inner_TransStcok(pData, tempbuf);
				oMSW.PutMsg(34, tempbuf, sizeof(XDFAPI_CNFutOptData) );

				pbuf += sizeof(tagCcComm_CsfexFutOptStockData);
				nLen -= sizeof(tagCcComm_CsfexFutOptStockData);
			}
			break;
		case 2:
			{
				XDFAPI_CNFutOptMkInquiry*	pInquiry = (XDFAPI_CNFutOptMkInquiry*)tempbuf;
				tagCcComm_CnfOptMkInquiry*	pData = (tagCcComm_CnfOptMkInquiry*)pbuf;

				::memcpy( pInquiry->Code, pData->Code, sizeof(pInquiry->Code) );
				::memcpy( pInquiry->ForQuoteSysID, pData->ForQuoteSysID, sizeof(pInquiry->ForQuoteSysID) );
				oMSW.PutMsg(36, tempbuf, sizeof(XDFAPI_CNFutOptMkInquiry) );

				pbuf += sizeof(tagCcComm_CnfOptMkInquiry);
				nLen -= sizeof(tagCcComm_CnfOptMkInquiry);
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

	oMSW.Detach();

	//[3]放入池子(PkgHead+PkgBuf)
	XDFAPI_PkgHead pkghead;
	memset(&pkghead, 0, sizeof(XDFAPI_PkgHead));
	pkghead.MarketID = XDF_CNFOPT;
	pkghead.PkgSize = oMSW.GetOffset();

	Global_DataIO.PutData( &pkghead, outbuf, oMSW.GetOffset());
}



int		MCnfOptDll::inner_TransNameTable( void * pData, char* poutbuf)
{
	tagCcComm_CsfexFutOptNameTable* pNameTb = (tagCcComm_CsfexFutOptNameTable*)pData;

	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_NameTableCnfOpt);
	XDFAPI_NameTableCnfOpt* pTable = (XDFAPI_NameTableCnfOpt*)(tempbuf);

	memset(pTable, 0, sizeof(XDFAPI_NameTableCnfOpt));
	pTable->Market = XDF_CNFOPT;
	pTable->SecKind = pNameTb->Type;
	memcpy(pTable->Code,pNameTb->Code, 20);
	memcpy(pTable->Name, pNameTb->Name, 40);
	pTable->LotFactor = pNameTb->LotFactor;

	pTable->LeavesQty = pNameTb->LeavesQty;
	pTable->ObjectMId = pNameTb->ObjectMId;
	memcpy(pTable->UnderlyingCode,pNameTb->UnderlyingCode, sizeof(pTable->UnderlyingCode));
	pTable->PriceLimitType = pNameTb->PriceLimitType;
	pTable->UpLimit = pNameTb->UpLimit;
	pTable->DownLimit = pNameTb->DownLimit;
	pTable->LotSize = pNameTb->LotSize;
	pTable->ContractMult = pNameTb->ContractMult;
	pTable->XqPrice = pNameTb->XqPrice;
	pTable->StartDate = pNameTb->StartDate;
	pTable->EndDate = pNameTb->EndDate;
	pTable->XqDate = pNameTb->XqDate;
	pTable->DeliveryDate = pNameTb->DeliveryDate;
	pTable->ExpireDate = pNameTb->ExpireDate;
	pTable->TypePeriodIndx = pNameTb->TypePeriodIndx;
	pTable->EarlyNightFlag = pNameTb->EarlyNightFlag;

	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}

int		MCnfOptDll::inner_TransStcok(void* pValue, char* poutbuf)
{
	tagCcComm_CsfexFutOptStockData* pData = (tagCcComm_CsfexFutOptStockData*)pValue;

	char tempbuf[1000]={0};
	int noffset = sizeof(XDFAPI_CNFutOptData);
	XDFAPI_CNFutOptData* pTable = (XDFAPI_CNFutOptData*)(tempbuf);

	pTable->Date = pData->Date;
	pTable->DataTimeStamp = pData->DataTimeStamp;
	memcpy(pTable->Code, pData->Code,20);
	pTable->High = pData->High;						//最高价格[* 放大倍数]
	pTable->Open = pData->Open;						//开盘价格[* 放大倍数]
	pTable->Low = pData->Low;						//最低价格[* 放大倍数]
	pTable->PreClose = pData->PreClose;				//昨收价格[* 放大倍数]
	pTable->PreSettlePrice = pData->PreSettlePrice;	//昨日结算价格[* 放大倍数]

	pTable->Now		 = pData->Now;                    //最新价格[* 放大倍数]
	pTable->Close	 = pData->Close;                  //今日收盘价格[* 放大倍数]
	pTable->SettlePrice	 = pData->SettlePrice;        //今日结算价格[* 放大倍数]
	pTable->UpperPrice	 = pData->UpperPrice;         //涨停价格[* 放大倍数]
	pTable->LowerPrice	 = pData->LowerPrice;         //跌停价格[* 放大倍数]
	pTable->Amount		 = pData->Amount;             //总成交金额[元]
	pTable->Volume		 = pData->Volume;             //总成交量[股]
	pTable->PreOpenInterest = pData->PreOpenInterest; //昨日持仓量[股]
	pTable->OpenInterest = pData->OpenInterest;       //持仓量[股]

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

int		MCnfOptDll::inner_TransMarketStatus(void* pData, char* poutbuf)
{
	tagCcComm_MarketStatusInfo* pStatus = (tagCcComm_MarketStatusInfo*)pData;

	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_MarketStatusInfo);
	XDFAPI_MarketStatusInfo* pTable = (XDFAPI_MarketStatusInfo*)(tempbuf);

	memset(pTable, 0, sizeof(XDFAPI_MarketStatusInfo));
	pTable->MarketDate = pStatus->MarketDate;
	pTable->MarketTime = pStatus->MarketTime;
	pTable->MarketID = XDF_CNFOPT;
	pTable->MarketStatus = pStatus->MarketStatus;

	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}














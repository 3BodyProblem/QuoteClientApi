
#include "StdAfx.h"
#include "MCnfDll.h"
#include "../GlobalIO/GlobalIO.h"

MCnfDll::MCnfDll()
{

}

MCnfDll::~MCnfDll()
{

}

void	MCnfDll::inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData)
{
	MsgType = 26;
	sizeComStockData = sizeof(tagCcComm_CsfexFutOptStockData);
	sizeXDFStockData = sizeof(XDFAPI_CNFutureData);
}

void	MCnfDll::inner_GetNameTableInfoEx(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable)
{
	MsgType = 8;
	sizeComNameTable = sizeof(tagCcComm_CsfexFutOptNameTable);
	sizeXDFNameTable = sizeof(XDFAPI_NameTableCnfEx);
}

void	MCnfDll::inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable)
{
	MsgType = 7;
	sizeComNameTable = sizeof(tagCcComm_CsfexFutOptNameTable);
	sizeXDFNameTable = sizeof(XDFAPI_NameTableCnf);
}

bool	MCnfDll::inner_CompareStock(void* pData, char* pCode)
{
	tagCcComm_CsfexFutOptNameTable* pStock = (tagCcComm_CsfexFutOptNameTable*)pData;
	if (_strnicmp(pStock->Code, pCode, 6) ==0)
	{
		return true;
	}
	return false;
}

//获取码表[2,99]
int		MCnfDll::inner_GetNameTable(unsigned short dwStartNo, void * pData, int nCount)
{
	assert( pData != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 105, dwStartNo, pData, nCount*sizeof(tagCcComm_CsfexFutOptNameTable), sizeof(tagCcComm_CsfexFutOptNameTable) );
}

//获取个股行情[2,101]
int		MCnfDll::inner_GetStock(unsigned short dwStartNo, void * pData, int nCount)
{
	assert( pData != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 202, dwStartNo, pData, nCount*sizeof(tagCcComm_CsfexFutOptStockData), sizeof(tagCcComm_CsfexFutOptStockData) );
}

bool	MCnfDll::outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID)
{
	//期望市场值是14
	if (MarketID == 14)
	{
		if (MainType ==2 && ChildType ==204 )
		{
			return true;
		}
	}
	return false;
}

void	MCnfDll::outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize)
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
				oMSW.PutMsg(26, tempbuf, sizeof(XDFAPI_CNFutureData) );

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
	pkghead.MarketID = XDF_CNF;
	pkghead.PkgSize = oMSW.GetOffset();

	Global_DataIO.PutData( &pkghead, outbuf, oMSW.GetOffset());
}

int		MCnfDll::inner_TransNameTableEx( void * pData, char* poutbuf)
{
	char							tempbuf[200] = { 0 };
	tagCcComm_CsfexFutOptNameTable* pNameTb = (tagCcComm_CsfexFutOptNameTable*)pData;
	XDFAPI_NameTableCnfEx*			pTable = (XDFAPI_NameTableCnfEx*)(tempbuf);

	pTable->Market = XDF_CNF;
	pTable->SecKind = pNameTb->Type;
	memcpy(pTable->Code,pNameTb->Code, 20);
	memcpy(pTable->Name, pNameTb->Name, 40);
	pTable->LotFactor = pNameTb->LotFactor;

	pTable->LeavesQty = pNameTb->LeavesQty;				// 未平仓合约数 = 昨持仓 单位是(张)
	pTable->ObjectMId = pNameTb->ObjectMId;				// 上海期货 0  大连期货 1  郑州期货 2 上海期权 3  大连期权 4  郑州期权 5
	::memcpy( pTable->UnderlyingCode, pNameTb->UnderlyingCode, sizeof(pNameTb->UnderlyingCode) );// 标的证券代码
	pTable->PriceLimitType = pNameTb->PriceLimitType;		// 涨跌幅限制类型(N 有涨跌幅)(R 无涨跌幅)
	pTable->UpLimit = pNameTb->UpLimit;					// 当日期权涨停价格(精确到厘) //[*放大倍数]
	pTable->DownLimit = pNameTb->DownLimit;				// 当日期权跌停价格(精确到厘) //[*放大倍数]
	pTable->LotSize = pNameTb->LotSize;					// 一手等于几张合约
	pTable->ContractMult = pNameTb->ContractMult;		// 合约乘数
	pTable->XqPrice = pNameTb->XqPrice;					// 行权价格(精确到厘) //[*放大倍数] 
	pTable->StartDate = pNameTb->StartDate;				// 首个交易日(YYYYMMDD)
	pTable->EndDate = pNameTb->EndDate;					// 最后交易日(YYYYMMDD)
	pTable->XqDate = pNameTb->XqDate;					// 行权日(YYYYMM)
	pTable->DeliveryDate = pNameTb->DeliveryDate;		// 交割日(YYYYMMDD)
	pTable->ExpireDate = pNameTb->ExpireDate;			// 到期日(YYYYMMDD)

	::memcpy( poutbuf, tempbuf, sizeof(XDFAPI_NameTableCnfEx) );

	return sizeof(XDFAPI_NameTableCnfEx);
}

int		MCnfDll::inner_TransNameTable( void * pData, char* poutbuf)
{
	tagCcComm_CsfexFutOptNameTable* pNameTb = (tagCcComm_CsfexFutOptNameTable*)pData;
	
	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_NameTableCnf);
	XDFAPI_NameTableCnf* pTable = (XDFAPI_NameTableCnf*)(tempbuf);

	memset(pTable, 0, sizeof(XDFAPI_NameTableCnf));
	pTable->Market = XDF_CNF;
	pTable->SecKind = pNameTb->Type;
	memcpy(pTable->Code,pNameTb->Code, 20);
	memcpy(pTable->Name, pNameTb->Name, 40);
	pTable->LotFactor = pNameTb->LotFactor;

	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}

int		MCnfDll::inner_TransStcok(void* pValue, char* poutbuf)
{
	tagCcComm_CsfexFutOptStockData* pData = (tagCcComm_CsfexFutOptStockData*)pValue;

	char tempbuf[1000]={0};
	int noffset = sizeof(XDFAPI_CNFutureData);
	XDFAPI_CNFutureData* pTable = (XDFAPI_CNFutureData*)(tempbuf);

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

int		MCnfDll::inner_TransMarketStatus(void* pData, char* poutbuf)
{
	tagCcComm_MarketStatusInfo* pStatus = (tagCcComm_MarketStatusInfo*)pData;
	
	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_MarketStatusInfo);
	XDFAPI_MarketStatusInfo* pTable = (XDFAPI_MarketStatusInfo*)(tempbuf);

	memset(pTable, 0, sizeof(XDFAPI_MarketStatusInfo));
	pTable->MarketDate = pStatus->MarketDate;
	pTable->MarketTime = pStatus->MarketTime;
	pTable->MarketID = XDF_CNF;
	pTable->MarketStatus = pStatus->MarketStatus;
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}
















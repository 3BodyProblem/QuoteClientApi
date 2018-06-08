#include "StdAfx.h"
#include "MZjqhDll.h"
#include "../GlobalIO/GlobalIO.h"

MZjqhDll::MZjqhDll()
{

}

MZjqhDll::~MZjqhDll()
{
	
}

void	MZjqhDll::inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData)
{
	MsgType = 20;
	sizeComStockData = sizeof(tagCcComm_CffexStockDataEx);
	sizeXDFStockData = sizeof(XDFAPI_CffexData);
}

void	MZjqhDll::inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable)
{
	MsgType = 4;
	sizeComNameTable = sizeof(tagCcComm_CffexNameTable);
	sizeXDFNameTable = sizeof(XDFAPI_NameTableZjqh);
}

bool	MZjqhDll::inner_CompareStock(void* pData, char* pCode)
{
	tagCcComm_CffexStockDataEx* pStock = (tagCcComm_CffexStockDataEx*)pData;
	if (_strnicmp(pStock->Code, pCode, 6) ==0)
	{
		return true;
	}
	return false;
}

//获取码表[2,100]
int		MZjqhDll::inner_GetNameTable(unsigned short dwStartNo, void * pData, int nCount)
{
	assert( pData != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 100, dwStartNo, pData, nCount*sizeof(tagCcComm_CffexNameTable), sizeof(tagCcComm_CffexNameTable) );
}

//获取个股行情[2,102]
int		MZjqhDll::inner_GetStock(unsigned short dwStartNo, void * pData, int nCount)
{
	assert( pData != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 102, dwStartNo, pData, nCount*sizeof(tagCcComm_CffexStockDataEx), sizeof(tagCcComm_CffexStockDataEx) );
}

bool	MZjqhDll::outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID)
{
	//期望市场值是9
	if (MarketID == 9)
	{
		if (MainType ==2 && ChildType ==105 )
		{
			return true;
		}
	}
	return false;
}
void	MZjqhDll::outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize)
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
		case 2:
			{
				tagCcComm_CffexStockDataEx* pData = (tagCcComm_CffexStockDataEx*)pbuf;
				inner_TransStcok(pData, tempbuf);
				oMSW.PutMsg(20, tempbuf, sizeof(XDFAPI_CffexData));
/*
if( strncmp( pData->Code, "IH1805", 6 ) == 0 ) {
	Global_LogUnit.WriteLogEx( 0, 0, "BARRY", "IH1805, Now=%d, BuyPrice0=%u, BuyVol0=%I64d, SellPrice0=%u, SellVol0=%I64d\n", pData->Now, pData->Buy[0].Price, pData->Buy[0].Volume, pData->Sell[0].Price, pData->Sell[0].Volume );
}
*/
				pbuf += sizeof(tagCcComm_CffexStockDataEx);
				nLen -= sizeof(tagCcComm_CffexStockDataEx);
			}
			break;
		case 3:
			{
				XDFAPI_CFFutOptMkInquiry*	pInquiry = (XDFAPI_CFFutOptMkInquiry*)tempbuf;
				tagCcComm_CnfOptMkInquiry*	pData = (tagCcComm_CnfOptMkInquiry*)pbuf;

				::memcpy( pInquiry->Code, pData->Code, sizeof(pInquiry->Code) );
				::memcpy( pInquiry->ForQuoteSysID, pData->ForQuoteSysID, sizeof(pInquiry->ForQuoteSysID) );
				oMSW.PutMsg(37, tempbuf, sizeof(XDFAPI_CFFutOptMkInquiry) );

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
	pkghead.MarketID = XDF_CF;
	pkghead.PkgSize = oMSW.GetOffset();
	
	Global_DataIO.PutData( &pkghead, outbuf, oMSW.GetOffset() );
}



int		MZjqhDll::inner_TransNameTable( void * pData, char* poutbuf)
{
	tagCcComm_CffexNameTable* pNameTb = (tagCcComm_CffexNameTable*)pData;
	
	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_NameTableZjqh);

	XDFAPI_NameTableZjqh* pTable = (XDFAPI_NameTableZjqh*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_NameTableZjqh));
	pTable->Market = XDF_CF;
	memcpy(pTable->Code, pNameTb->Code, 6);
	pTable->SecKind = pNameTb->Type;
	pTable->ContractMult = pNameTb->ContractMult;	//合约乘数
	pTable->ExFlag = pNameTb->ExFlag;
	strncpy(pTable->Name, pNameTb->Name, 8);
	pTable->ObjectMId = pNameTb->ObjectMId;
	strncpy(pTable->ObjectCode, pNameTb->ObjectCode,6);
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}

int		MZjqhDll::inner_TransStcok(void* pValue, char* poutbuf)
{
	tagCcComm_CffexStockDataEx* pData = (tagCcComm_CffexStockDataEx*)pValue;

	char tempbuf[1000]={0};
	int noffset = sizeof(XDFAPI_CffexData);

	XDFAPI_CffexData* pTable = (XDFAPI_CffexData*)(tempbuf);
	
	pTable->DataTimeStamp = pData->DataTimeStamp;
	memcpy(pTable->Code, pData->Code,6);
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

int		MZjqhDll::inner_TransMarketStatus(void* pData, char* poutbuf)
{
	tagCcComm_MarketStatusInfo* pStatus = (tagCcComm_MarketStatusInfo*)pData;

	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_MarketStatusInfo);
	
	XDFAPI_MarketStatusInfo* pTable = (XDFAPI_MarketStatusInfo*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_MarketStatusInfo));
	pTable->MarketDate = pStatus->MarketDate;
	pTable->MarketTime = pStatus->MarketTime;
	pTable->MarketID = XDF_CF;
	pTable->MarketStatus = pStatus->MarketStatus;

	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}




#include "StdAfx.h"
#include "MShOptDll.h"
#include "../GlobalIO/GlobalIO.h"


MShOptDll::MShOptDll()
{

}

MShOptDll::~MShOptDll()
{

}
void	MShOptDll::inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData)
{
	MsgType = 15;
	sizeComStockData = sizeof(tagCcComm_ShOptionData);
	sizeXDFStockData = sizeof(XDFAPI_ShOptData);
}
void	MShOptDll::inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable)
{
	MsgType = 2;
	sizeComNameTable = sizeof(tagCcComm_ShOptNameTable);
	sizeXDFNameTable = sizeof(XDFAPI_NameTableShOpt);
}

//获取码表[2,80]
int		MShOptDll::inner_GetNameTable( unsigned short dwStartNo, void * pValue, int nCount )
{
	assert( pValue != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 80, dwStartNo, pValue, nCount*sizeof(tagCcComm_ShOptNameTable), sizeof(tagCcComm_ShOptNameTable) );
}

//获取个股行情[2,82]
int		MShOptDll::inner_GetStock( unsigned short dwStartNo, void * pValue, int nCount )
{
	assert( pValue != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 82, dwStartNo, pValue, nCount*sizeof(tagCcComm_ShOptionData), sizeof(tagCcComm_ShOptionData) );
}

int 	MShOptDll::inner_TransNameTable(void * pData, char* poutbuf)
{
	tagCcComm_ShOptNameTable * pNameTb = (tagCcComm_ShOptNameTable*)pData;

	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_NameTableShOpt);

	XDFAPI_NameTableShOpt* pTable = (XDFAPI_NameTableShOpt*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_NameTableShOpt));
	pTable->Market = XDF_SHOPT;
	memcpy(pTable->Code, pNameTb->SecurityID, 8);
	memcpy(pTable->Name, pNameTb->SecuritySymbol,20);
	pTable->SecKind = pNameTb->Type;
	memcpy(pTable->ContractID, pNameTb->ContractID, 19);

	memcpy( &(pTable->OptionType), &(pNameTb->OptionType),  sizeof(XDFAPI_NameTableShOpt)-49 );

	memcpy(poutbuf, tempbuf, noffset);	
	return noffset;
}

int		MShOptDll::inner_TransStcok(void* pValue, char* poutbuf)
{
	tagCcComm_ShOptionData* pData = (tagCcComm_ShOptionData*)pValue;

	char tempbuf[1000]={0};
	int noffset = sizeof(XDFAPI_ShOptData);
	XDFAPI_ShOptData* pTable = (XDFAPI_ShOptData*)(tempbuf);

	memcpy(pTable->Code, pData->SecurityID,8);
	pTable->Time = pData->DataTimeStamp;
	pTable->PreSettlePx = pData->PreSettlePx;
	pTable->SettlePrice = pData->SettlePrice;
	pTable->OpenPx = pData->OpenPx;
	pTable->HighPx = pData->HighPx;
	pTable->LowPx = pData->LowPx;
	pTable->Now = pData->Now;
	pTable->Volume = pData->Volume;
	pTable->Amount = pData->Amount;
	memcpy(pTable->TradingPhaseCode, pData->TradingPhaseCode,4);
	pTable->AuctionPrice = pData->AuctionPrice;
	pTable->AuctionQty = pData->AuctionQty;
	pTable->Position = pData->Position;
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

int		MShOptDll::inner_TransMarketStatus(void* pData, char* poutbuf)
{
	tagCcComm_ShOptMarketStatus* pStatus = (tagCcComm_ShOptMarketStatus*)pData;

	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_ShOptMarketStatus);

	XDFAPI_ShOptMarketStatus* pTable = (XDFAPI_ShOptMarketStatus*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_ShOptMarketStatus));
	pTable->MarketDate = pStatus->MarketDate;
	pTable->MarketTime = pStatus->MarketTime;
	memcpy(pTable->TradingPhaseCode, pStatus->TradingPhaseCode,8);
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}

bool	MShOptDll::inner_CompareStock(void* pData, char* pCode)
{
	tagCcComm_ShOptionData* pStock = (tagCcComm_ShOptionData*)pData;
	if (_strnicmp(pStock->SecurityID, pCode, 8) ==0)
	{
		return true;
	}
	return false;
}

//.............................................................................................................................................................
//.............................................................................................................................................................
//.............................................................................................................................................................
//.............................................................................................................................................................
//.............................................................................................................................................................
//.............................................................................................................................................................
//.............................................................................................................................................................

bool	MShOptDll::outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID)
{
	//期望市场值是27
	if (MarketID == 27)
	{
		if (MainType ==2 && ChildType == 83 )
		{
			return true;
		}
	}
	return false;
}

void	MShOptDll::outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize)
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
				tagCcComm_ShOptMarketStatus * pMarketInfo = (tagCcComm_ShOptMarketStatus *)pbuf;
				inner_TransMarketStatus(pMarketInfo, tempbuf);
				oMSW.PutSingleMsg(14, tempbuf, sizeof(XDFAPI_ShOptMarketStatus));

				pbuf += sizeof(tagCcComm_ShOptMarketStatus);
				nLen -= sizeof(tagCcComm_ShOptMarketStatus);
			}
			break;
		case 1:
			{
				tagCcComm_ShOptionData* pData = (tagCcComm_ShOptionData*)pbuf;
				inner_TransStcok(pData, tempbuf);
				oMSW.PutMsg(15, tempbuf, sizeof(XDFAPI_ShOptData));

				pbuf += sizeof(tagCcComm_ShOptionData);
				nLen -= sizeof(tagCcComm_ShOptionData);
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
	pkghead.MarketID = XDF_SHOPT;
	pkghead.PkgSize = oMSW.GetOffset();
	
	Global_DataIO.PutData( &pkghead, outbuf, oMSW.GetOffset() );
}


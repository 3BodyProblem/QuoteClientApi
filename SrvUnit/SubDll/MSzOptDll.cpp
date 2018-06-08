
#include "StdAfx.h"
#include "MSzOptDll.h"
#include "../GlobalIO/GlobalIO.h"


MSzOptDll::MSzOptDll()
{

}

MSzOptDll::~MSzOptDll()
{

}

bool	MSzOptDll::outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID)
{
	//期望市场值是31
	if (MarketID == 31)
	{
		if (MainType ==2 && ChildType == 115 )
		{
			return true;
		}
	}
	return false;
}

void	MSzOptDll::outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize)
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
				tagCcComm_SzOptMarketStatus * pMarketInfo = (tagCcComm_SzOptMarketStatus *)pbuf;
				inner_TransMarketStatus(pMarketInfo, tempbuf);
				oMSW.PutSingleMsg(1, tempbuf, sizeof(XDFAPI_MarketStatusInfo));

				pbuf += sizeof(tagCcComm_SzOptMarketStatus);
				nLen -= sizeof(tagCcComm_SzOptMarketStatus);
			}
			break;
		case 1:
			{
				tagCcComm_SzOptionData* pData = (tagCcComm_SzOptionData*)pbuf;
				inner_TransStcok(pData, tempbuf);
				oMSW.PutMsg(35, tempbuf, sizeof(XDFAPI_SzOptData));

				pbuf += sizeof(tagCcComm_SzOptionData);
				nLen -= sizeof(tagCcComm_SzOptionData);
			}
			break;
		case 2:
			{
				pbuf += sizeof(tagCcComm_SzOptPerNameChg);
				nLen -= sizeof(tagCcComm_SzOptPerNameChg);
			}
			break;
		case 3:
			{
				
				XDFAPI_SzOptMkInquiry*		pInquiry = (XDFAPI_SzOptMkInquiry*)tempbuf;
				tagCcComm_SzOptMkInquiry*	pData = (tagCcComm_SzOptMkInquiry*)pbuf;

				::memcpy( pInquiry->Code, pData->Code, sizeof(pInquiry->Code) );
				pInquiry->InquiryStatus = pData->InquiryStatus;
				oMSW.PutMsg(38, tempbuf, sizeof(XDFAPI_SzOptMkInquiry) );


				pbuf += sizeof(tagCcComm_SzOptMkInquiry);
				nLen -= sizeof(tagCcComm_SzOptMkInquiry);
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
	pkghead.MarketID = XDF_SZOPT;
	pkghead.PkgSize = oMSW.GetOffset();

	Global_DataIO.PutData( &pkghead, outbuf, oMSW.GetOffset() );
}

void	MSzOptDll::inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData)
{
	MsgType = 35;
	sizeComStockData = sizeof(tagCcComm_SzOptionData);
	sizeXDFStockData = sizeof(XDFAPI_SzOptData);
}
void	MSzOptDll::inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable)
{
	MsgType = 9;
	sizeComNameTable = sizeof(tagCcComm_SzOptNameTable);
	sizeXDFNameTable = sizeof(XDFAPI_NameTableSzOpt);
}

int		MSzOptDll::inner_GetNameTable(unsigned short dwStartNo, void * pValue, int nCount)
{
	assert( pValue != NULL && nCount > 0 );

	return m_p4XDll->GetData( 2, 112, dwStartNo, pValue, nCount*sizeof(tagCcComm_SzOptNameTable), sizeof(tagCcComm_SzOptNameTable) );
}
int		MSzOptDll::inner_GetStock(unsigned short dwStartNo, void * pValue, int nCount)
{
	assert( pValue != NULL && nCount > 0 );

	return m_p4XDll->GetData( 2, 113, dwStartNo, pValue, nCount*sizeof(tagCcComm_SzOptionData), sizeof(tagCcComm_SzOptionData) );
}

int		MSzOptDll::inner_TransNameTable( void * pData, char* poutbuf)
{
	tagCcComm_SzOptNameTable * pNameTb = (tagCcComm_SzOptNameTable*)pData;

	char tempbuf[400]={0};
	int noffset = sizeof(XDFAPI_NameTableSzOpt);

	XDFAPI_NameTableSzOpt* pTable = (XDFAPI_NameTableSzOpt*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_NameTableSzOpt));
	pTable->Market = XDF_SZOPT;
	pTable->SecKind = pNameTb->Type;
	memcpy(pTable->Code, pNameTb->SecurityID, 8);
	memcpy(pTable->Name, pNameTb->SecuritySymbol,20);
	memcpy(pTable->ContractID, pNameTb->ContractID, 20);
	pTable->OptionType = pNameTb->OptionType;
	pTable->CallOrPut = pNameTb->CallOrPut;
	pTable->PreClosePx = pNameTb->PreClosePx;
	pTable->PreSettlePx = pNameTb->PreSettlePx;
	pTable->LeavesQty = pNameTb->LeavesQty;
	memcpy(pTable->UnderlyingCode, pNameTb->UnderlyingCode, 6);
	pTable->UpLimit = pNameTb->UpLimit;
	pTable->DownLimit = pNameTb->DownLimit;
	pTable->LotSize = pNameTb->LotSize;
	pTable->ContractUnit = pNameTb->ContractUnit;
	pTable->XqPrice = pNameTb->XqPrice;
	pTable->StartDate = pNameTb->StartDate;
	pTable->EndDate = pNameTb->EndDate;
	pTable->XqDate = pNameTb->XqDate;

	pTable->DeliveryDate = pNameTb->ExpireDate;  //交割日， pNameTb 里是交割月份，这里直接把到期日填入交割日
	pTable->ExpireDate = pNameTb->ExpireDate;
	pTable->MarginUnit = pNameTb->MarginUnit;

	memcpy(poutbuf, tempbuf, noffset);	

	return noffset;
}

int		MSzOptDll::inner_TransStcok(void* pValue, char* poutbuf)
{
	tagCcComm_SzOptionData* pData = (tagCcComm_SzOptionData*)pValue;

	char tempbuf[1000]={0};
	int noffset = sizeof(XDFAPI_SzOptData);
	XDFAPI_SzOptData* pTable = (XDFAPI_SzOptData*)(tempbuf);

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

bool	MSzOptDll::inner_CompareStock(void* pData, char* pCode)
{
	tagCcComm_SzOptionData* pStock = (tagCcComm_SzOptionData*)pData;
	if (_strnicmp(pStock->SecurityID, pCode, 8) ==0)
	{
		return true;
	}
	return false;
}


int		MSzOptDll::inner_TransMarketStatus(void* pData, char* poutbuf)
{
	tagCcComm_MarketStatusInfo* pStatus = (tagCcComm_MarketStatusInfo*)pData;

	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_MarketStatusInfo);
	XDFAPI_MarketStatusInfo* pTable = (XDFAPI_MarketStatusInfo*)(tempbuf);

	memset(pTable, 0, sizeof(XDFAPI_MarketStatusInfo));
	pTable->MarketDate = pStatus->MarketDate;
	pTable->MarketTime = pStatus->MarketTime;
	pTable->MarketID = XDF_SZOPT;
	pTable->MarketStatus = pStatus->MarketStatus;

	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}



























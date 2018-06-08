


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

//��ȡ���[2,201]
int		MCnfOptDll::inner_GetNameTable(unsigned short dwStartNo, void * pData, int nCount)
{
	assert( pData != NULL && nCount > 0 );

	return m_p4XDll->GetData( 2, 105, dwStartNo, pData, nCount*sizeof(tagCcComm_CsfexFutOptNameTable), sizeof(tagCcComm_CsfexFutOptNameTable) );
}

//��ȡ��������[2,202]
int		MCnfOptDll::inner_GetStock(unsigned short dwStartNo, void * pData, int nCount)
{
	assert( pData != NULL && nCount > 0 );

	return m_p4XDll->GetData( 2, 202, dwStartNo, pData, nCount*sizeof(tagCcComm_CsfexFutOptStockData), sizeof(tagCcComm_CsfexFutOptStockData) );
}

bool	MCnfOptDll::outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID)
{
	//�����г�ֵ��35
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
	//[1]//�����г�ʱ��,����iRecv
	m_i64Recv += bufsize;

	tagCcComm_TimeInfo* pTime = (tagCcComm_TimeInfo*)pszbuf;
	m_uTime = pTime->CurTime;

	//[2]ת����ʽ
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
				//�˴����������־��������ʾ
				//return;	//�������δ֪����,������������,��DLL�汾����Ϣ
				nLen =0;	//ǿ���˳�while()
			}
			break;
		}
	}

	oMSW.Detach();

	//[3]�������(PkgHead+PkgBuf)
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
	pTable->High = pData->High;						//��߼۸�[* �Ŵ���]
	pTable->Open = pData->Open;						//���̼۸�[* �Ŵ���]
	pTable->Low = pData->Low;						//��ͼ۸�[* �Ŵ���]
	pTable->PreClose = pData->PreClose;				//���ռ۸�[* �Ŵ���]
	pTable->PreSettlePrice = pData->PreSettlePrice;	//���ս���۸�[* �Ŵ���]

	pTable->Now		 = pData->Now;                    //���¼۸�[* �Ŵ���]
	pTable->Close	 = pData->Close;                  //�������̼۸�[* �Ŵ���]
	pTable->SettlePrice	 = pData->SettlePrice;        //���ս���۸�[* �Ŵ���]
	pTable->UpperPrice	 = pData->UpperPrice;         //��ͣ�۸�[* �Ŵ���]
	pTable->LowerPrice	 = pData->LowerPrice;         //��ͣ�۸�[* �Ŵ���]
	pTable->Amount		 = pData->Amount;             //�ܳɽ����[Ԫ]
	pTable->Volume		 = pData->Volume;             //�ܳɽ���[��]
	pTable->PreOpenInterest = pData->PreOpenInterest; //���ճֲ���[��]
	pTable->OpenInterest = pData->OpenInterest;       //�ֲ���[��]

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














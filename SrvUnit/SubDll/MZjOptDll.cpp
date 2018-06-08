
#include "StdAfx.h"
#include "MZjOptDll.h"
#include "../GlobalIO/GlobalIO.h"

MZjOptDll::MZjOptDll()
{

}

MZjOptDll::~MZjOptDll()
{
	
}

void	MZjOptDll::inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData)
{
	MsgType = 18;
	sizeComStockData = sizeof(tagCcComm_CffexOptStockData);
	sizeXDFStockData = sizeof(XDFAPI_ZjOptData);
}

void	MZjOptDll::inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable)
{
	MsgType = 3;
	sizeComNameTable = sizeof(tagCcComm_CffexOptNameTable);
	sizeXDFNameTable = sizeof(XDFAPI_NameTableZjOpt);
}

//��ȡ���[2,108]
int		MZjOptDll::inner_GetNameTable(unsigned short dwStartNo, void * pValue, int nCount)
{
	assert( pValue != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 108, dwStartNo, pValue, nCount*sizeof(tagCcComm_CffexOptNameTable), sizeof(tagCcComm_CffexOptNameTable) );
}
//��ȡ��������[2,109]
int		MZjOptDll::inner_GetStock(unsigned short dwStartNo, void * pValue, int nCount)
{
	assert( pValue != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 109, dwStartNo, pValue, nCount*sizeof(tagCcComm_CffexOptStockData), sizeof(tagCcComm_CffexOptStockData) );
}

bool	MZjOptDll::inner_CompareStock(void* pData, char* pCode)
{
	tagCcComm_CffexOptStockData* pStock = (tagCcComm_CffexOptStockData*)pData;
	if (_strnicmp(pStock->Code, pCode, 32) ==0)
	{
		return true;
	}
	return false;
}

bool	MZjOptDll::outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID)
{
	//�����г�ֵ��28
	if (MarketID == 28)
	{
		if (MainType ==2 && ChildType ==110 )
		{
			return true;
		}
	}
	return false;
}



void	MZjOptDll::outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize)
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
		case 2:
			{
				tagCcComm_CffexOptStockData* pData = (tagCcComm_CffexOptStockData*)pbuf;
				inner_TransStcok(pData, tempbuf);
				oMSW.PutMsg(18, tempbuf, sizeof(XDFAPI_ZjOptData) );

				pbuf += sizeof(tagCcComm_CffexOptStockData);
				nLen -= sizeof(tagCcComm_CffexOptStockData);
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
	pkghead.MarketID = XDF_ZJOPT;
	pkghead.PkgSize = oMSW.GetOffset();
	
	Global_DataIO.PutData( &pkghead, outbuf, oMSW.GetOffset() );
}


int		MZjOptDll::inner_TransNameTable( void * pData, char* poutbuf)
{
	tagCcComm_CffexOptNameTable * pNameTb = (tagCcComm_CffexOptNameTable*)pData;

	char tempbuf[400]={0};
	int noffset = sizeof(XDFAPI_NameTableZjOpt);
	
	XDFAPI_NameTableZjOpt* pTable = (XDFAPI_NameTableZjOpt*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_NameTableZjOpt));
	pTable->Market = XDF_ZJOPT;
	memcpy(pTable->Code, pNameTb->Code, 32);
	pTable->SecKind = pNameTb->Type;
	
	pTable->ContractMult = pNameTb->ContractMult;	//��Լ����
	pTable->ContractUnit = pNameTb->ContractUnit;	//��Լ��λ
	pTable->StartDate = pNameTb->StartDate;			//�׸�������(YYYYMMDD)
	pTable->EndDate = pNameTb->EndDate;				//�������(YYYYMMDD)
	pTable->XqDate = pNameTb->XqDate;				//��Ȩ��(YYYYMMDD)
	pTable->DeliveryDate = pNameTb->DeliveryDate;	//������(YYYYMMDD)
	pTable->ExpireDate = pNameTb->ExpireDate;		//������(YYYYMMDD)
	pTable->ObjectMId = pNameTb->ObjectMId;
	strncpy(pTable->ObjectCode, pNameTb->ObjectCode,6);
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}

int		MZjOptDll::inner_TransStcok(void* pValue, char* poutbuf)
{
	tagCcComm_CffexOptStockData* pData = (tagCcComm_CffexOptStockData*)pValue;

	char tempbuf[1000]={0};
	int noffset = sizeof(XDFAPI_ZjOptData);
	XDFAPI_ZjOptData* pTable = (XDFAPI_ZjOptData*)(tempbuf);
	
	pTable->DataTimeStamp = pData->DataTimeStamp;
	memcpy(pTable->Code, pData->Code,32);
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
	return noffset ;
}

int		MZjOptDll::inner_TransMarketStatus(void* pData, char* poutbuf)
{
	tagCcComm_MarketStatusInfo* pStatus = (tagCcComm_MarketStatusInfo*)pData;

	char tempbuf[200]={0};
	int noffset = sizeof(XDFAPI_MarketStatusInfo);

	XDFAPI_MarketStatusInfo* pTable = (XDFAPI_MarketStatusInfo*)(tempbuf);
	memset(pTable, 0, sizeof(XDFAPI_MarketStatusInfo));
	pTable->MarketDate = pStatus->MarketDate;
	pTable->MarketTime = pStatus->MarketTime;
	pTable->MarketID = XDF_ZJOPT;
	pTable->MarketStatus = pStatus->MarketStatus;
	
	memcpy(poutbuf, tempbuf, noffset);	
	return noffset ;
}




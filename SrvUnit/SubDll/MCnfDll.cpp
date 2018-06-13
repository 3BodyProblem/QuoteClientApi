
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

//��ȡ���[2,99]
int		MCnfDll::inner_GetNameTable(unsigned short dwStartNo, void * pData, int nCount)
{
	assert( pData != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 105, dwStartNo, pData, nCount*sizeof(tagCcComm_CsfexFutOptNameTable), sizeof(tagCcComm_CsfexFutOptNameTable) );
}

//��ȡ��������[2,101]
int		MCnfDll::inner_GetStock(unsigned short dwStartNo, void * pData, int nCount)
{
	assert( pData != NULL && nCount > 0 );
	
	return m_p4XDll->GetData( 2, 202, dwStartNo, pData, nCount*sizeof(tagCcComm_CsfexFutOptStockData), sizeof(tagCcComm_CsfexFutOptStockData) );
}

bool	MCnfDll::outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID)
{
	//�����г�ֵ��14
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

	pTable->LeavesQty = pNameTb->LeavesQty;				// δƽ�ֺ�Լ�� = ��ֲ� ��λ��(��)
	pTable->ObjectMId = pNameTb->ObjectMId;				// �Ϻ��ڻ� 0  �����ڻ� 1  ֣���ڻ� 2 �Ϻ���Ȩ 3  ������Ȩ 4  ֣����Ȩ 5
	::memcpy( pTable->UnderlyingCode, pNameTb->UnderlyingCode, sizeof(pNameTb->UnderlyingCode) );// ���֤ȯ����
	pTable->PriceLimitType = pNameTb->PriceLimitType;		// �ǵ�����������(N ���ǵ���)(R ���ǵ���)
	pTable->UpLimit = pNameTb->UpLimit;					// ������Ȩ��ͣ�۸�(��ȷ����) //[*�Ŵ���]
	pTable->DownLimit = pNameTb->DownLimit;				// ������Ȩ��ͣ�۸�(��ȷ����) //[*�Ŵ���]
	pTable->LotSize = pNameTb->LotSize;					// һ�ֵ��ڼ��ź�Լ
	pTable->ContractMult = pNameTb->ContractMult;		// ��Լ����
	pTable->XqPrice = pNameTb->XqPrice;					// ��Ȩ�۸�(��ȷ����) //[*�Ŵ���] 
	pTable->StartDate = pNameTb->StartDate;				// �׸�������(YYYYMMDD)
	pTable->EndDate = pNameTb->EndDate;					// �������(YYYYMMDD)
	pTable->XqDate = pNameTb->XqDate;					// ��Ȩ��(YYYYMM)
	pTable->DeliveryDate = pNameTb->DeliveryDate;		// ������(YYYYMMDD)
	pTable->ExpireDate = pNameTb->ExpireDate;			// ������(YYYYMMDD)

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
















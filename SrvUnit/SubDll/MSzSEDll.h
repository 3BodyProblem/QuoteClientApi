
#ifndef MSZSEDLL_DEFINE_H
#define MSZSEDLL_DEFINE_H

//代码基本和MSHSEDll相同
#include "MData4XDll.h"

//深圳证券
class MSzSEDll : public MData4XDll
{
public:
	MSzSEDll();
	virtual ~MSzSEDll();

public:
	int		outer_GetLastMarketDataAll(char* pszInBuf, int32_t nInBytes);	//除了个股，还有指数
	int		inner_getStopFlagData(int nStockCount, MStreamWrite& oMSW);

protected:
	int		inner_getLastStockData(int nStockCount, MStreamWrite& oMSW);
	int		inner_getLastIndexData(int nIndexCount, MStreamWrite& oMSW);

public:
	bool	outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID);
	void	outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize);
	
protected:
	void	inner_GetIndexInfo(unsigned int& MsgType, int& sizeComIndexData, int& sizeXDFIndexData);
	void	inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData);
	void	inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable);
	void	inner_GetStopFlag(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData);

	int		inner_GetNameTableEx(unsigned short dwStartNo, void * pValue, int nCount);//获取扩展码表[2,8]
	int		inner_GetNameTable(unsigned short dwStartNo, void * pData, int nCount);//获取码表[2,2]
	int		inner_GetStock(unsigned short dwStartNo, void * pData, int nCount);//获取个股行情[2,7]
	int		inner_GetIndex(unsigned short dwStartNo, void * pData, int nCount);//获取指数行情[2,6]
	
	int		inner_TransStopFlag(void* pValue, char* poutbuf);
	int		inner_TransIndexData(void* pValue, char* poutbuf);
	int		inner_TransNameTable(void* pValue, char* poutbuf);
	int		inner_TransStcok(void* pValue, char* poutbuf); 
	
	bool	inner_CompareStock(void* pData, char* pCode);
	bool	inner_IndexCompare(void* pData, char* pCode);
	
	int		inner_TransMarketStatus(void* pData, char* poutbuf);
	int		inner_TransPreNameChg(void* pData, char* poutbuf);

};






#endif
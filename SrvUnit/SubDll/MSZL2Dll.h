
#ifndef _MSZL2DLL_DEFINE_H
#define _MSZL2DLL_DEFINE_H


#include "MData4XDll.h"
//深圳L2

class MSZL2Dll: public MData4XDll
{
public:
	MSZL2Dll();
	virtual ~MSZL2Dll();

public:
	int		outer_GetLastMarketDataAll(char* pszInBuf, int32_t nInBytes);	//除了个股，还有指数
	
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
	
	int		inner_GetNameTable(unsigned short dwStartNo, void * pData, int nCount);
	int		inner_GetStock(unsigned short dwStartNo, void * pData, int nCount);
	int		inner_GetIndex(unsigned short dwStartNo, void * pData, int nCount);
	
	int		inner_TransIndexData(void* pValue, char* poutbuf);
	int		inner_TransNameTable( void * pData, char* poutbuf);
	int		inner_TransStcok(void* pValue, char* poutbuf); 
	
	bool	inner_CompareStock(void* pData, char* pCode);
	bool	inner_IndexCompare(void* pData, char* pCode);
	
	int		inner_TransMarketStatus(void* pData, char* poutbuf);


};



#endif


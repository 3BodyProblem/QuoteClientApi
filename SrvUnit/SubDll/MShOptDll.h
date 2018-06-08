
#ifndef _MSHOPTDLL_DEFINE_H
#define _MSHOPTDLL_DEFINE_H

#include "MData4XDll.h"

//上海期权

class MShOptDll : public MData4XDll
{
public:
	MShOptDll();
	virtual ~MShOptDll();

public:
	bool	outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID);
	void	outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize);

protected:
	void	inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData);
	void	inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable);
	
	int		inner_GetNameTable(unsigned short dwStartNo, void * pData, int nCount);//获取码表[2,80]
	int		inner_GetStock(unsigned short dwStartNo, void * pData, int nCount);//获取个股行情[2,82]
	
	int		inner_TransNameTable( void * pData, char* poutbuf);
	int		inner_TransStcok(void* pValue, char* poutbuf); 
	bool	inner_CompareStock(void* pData, char* pCode);

protected:
	int		inner_TransMarketStatus(void* pData, char* poutbuf);

};




#endif
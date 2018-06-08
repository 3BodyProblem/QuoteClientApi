#ifndef _MData4XDLL_DEFINE_H
#define _MData4XDLL_DEFINE_H


#include "Base4XDll.h"


class MStreamWrite
{
public:
	MStreamWrite();
	virtual ~MStreamWrite();
	MStreamWrite(char* pszinbuf, int ninbytes);

public:
	void			Attach(char* pszinbuf, int ninbytes);
	void			Detach();	//处理MsgHead,Last缓冲等
	//负责处理(1)超过ninbytes, (2)超过ushort的MsgLen复合类型等处理,
	//复合类型，是否要缓冲一个，还有，如果超出65535的表述范围，要另外分一个,LastType=0xFFFF?


	void			PutSingleMsg(unsigned int ntype, char* pStruct, int nStructSize);	//独立消息，不可合并
	void			PutMsg(unsigned int ntype, char* pStruct, int nStructSize);

	bool			IsError();
	int				GetOffset();
	int				GetCount();

protected:

protected:
	char*			m_pInBuf;
	int				m_nInBytes;
	
	XDFAPI_UniMsgHead*	m_pUniHead;

	unsigned int	m_nLastType;
	int				m_nMsgCount;
	int				m_nOffset;
	bool			m_bError;

private:

};


/**
 * @class		MData4XDll
 * @brief		各市场回调派生类接口类
 */
class MData4XDll
{
public:
	MData4XDll();
	virtual ~MData4XDll();

public:
	/**
	 * @brief		绑定到一个对应的市场传输的管理对象
	 */
	void			Bind4XDll(Base4XDll* pDll);

	/**
	 * @brief		重置状态
	 */
	void			Reset();

	/**
	 *
	 * @brief		获取模块状态信息
	 * @param[out]	pInfo				状态个自
	 */
	virtual void	GetDllStatus( tagDll_DriverStatusInfo* pInfo );

public:
	/**
	 * @brief		市场推送协议有效性检查
	 * @return		true		有效协议
					false		无效协议
	 */
	virtual bool	outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID ) = 0;

	/**
	 * @brief		市场推送行情回调
	 */
	virtual void	outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize) = 0;

	/**
	 * @brief		取得统计信息
	 */
	virtual void	outer_GetStatus(uint32_t& ulTime, int64_t * pI64Send, int64_t * pI64Recv);

	/**
	 * @brief		取得全部的快照信息
	 */
	virtual int		outer_GetLastMarketDataAll(char* pszInBuf, int32_t nInBytes);

	/**
	 * @brief		取得全部的码表信息
	 */
	virtual int		outer_GetCodeTable(char* pszInBuf, int32_t nInBytes, int32_t& nCount);

	/**
	 * @brief		取得全部的市场信息
	 */
	virtual int		outer_GetMarketInfo(char* pszInBuf, int32_t nInBytes);

	/**
	 * @brief		取得市场日期和时间
	 */
	virtual int		outer_GetSimpleMarketInfo(void* pInfo);

	/**
	 * @brief		取得市场日期和时间(无阻塞查询)
	 */
	virtual int		outer_GetQuickSimpleMarketInfo(void* pInfo);

protected:
	/**
	 * @brief		接口：取得各对应市场的快照大小信息
	 */
	virtual void	inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData) = 0;

	/**
	 * @brief		接口：取得各对应市场的码表大小信息
	 */
	virtual void	inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable) = 0;

	/**
	 * @brief		接口：取得各对应市场的码表内存块
	 */
	virtual int		inner_GetNameTable(unsigned short dwStartNo, void * pData, int nCount) = 0;

	/**
	 * @brief		接口：取得各对应市场的快照内存块
	 */
	virtual int		inner_GetStock(unsigned short dwStartNo, void * pData, int nCount) = 0;

	/**
	 * @brief		接口：转换传输的码表结构至qwin的码表结构
	 */
	virtual int		inner_TransNameTable( void * pNameTb, char* poutbuf) = 0;

	/**
	 * @brief		接口：转换传输的快照结构至qwin的快照结构
	 */
	virtual int		inner_TransStcok(void* pData, char* poutbuf) = 0; 

	/**
	 * @brief		接口：比较两个快照是否是同一个商品的
	 */
	virtual bool	inner_CompareStock(void* pData, char* pCode) =0;

protected:
	Base4XDll*		m_p4XDll;
	unsigned int	m_uTime;
	__int64			m_i64Send;
	__int64			m_i64Recv;
};


#endif







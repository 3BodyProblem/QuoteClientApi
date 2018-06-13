
#ifndef _MDLLMGR_DEFINE_H
#define _MDLLMGR_DEFINE_H


#include	"MEngine.hpp"

#include	"MShOptDll.h"
#include	"MZjOptDll.h"
#include	"MZjqhDll.h"
#include	"MShSEDll.h"
#include	"MSzSEDll.h"
#include	"MCnfDll.h"
#include	"MSHL2Dll.h"
#include	"MSZL2Dll.h"
#include	"MSzOptDll.h"
#include	"MCnfOptDll.h"

#include	"../DataClient/DataClient.h"

#include <vector>
using namespace std;


//某个市场的状态
typedef struct  
{
	int		nMarketID;
	int		nInnerStat;	//启动状态(启动成功/失败) 0=失败  1= 成功
	int		nOuterStat; //运行状态(不可用/可用) //	0 =不可用   XRS_Unknow=未知  XRS_Init=未就绪  5=可用
}tagOneMarketStat;


/**
 * @class		各市场传输Dll状态管理类
 * @class		M4XDllStat
 */
class M4XDllStat
{
public:
	M4XDllStat();
	virtual ~M4XDllStat();
	
public:
	/**
	 * @brief		启动一个市场(调用inner_StartOneMarket())
	 */
	int				BeginWork();

	/**
	 * @brief		停止所有市场的传输服务，并清空资源
	 */
	void			EndWork();

	/**
	 * @brief		取得驱动的服务状态
	 */
	int				GetMarketStat(int nMarketID);

	/**
	 * @brief		周期性检查各市场状态，并通知回调这种变化
	 */
	void			CheckMarket();

	/**
	 * @brief		根据市场ID返回对应的市场引用
	 */
	Base4XDll*		GetMarketDll(int nMarketId);

protected:
	/**
	 * @brief		根据参数，马上加载Dll & 启动一个市场
					&
					记录一个市场的活动状态(tagOneMarketStat)
	 * @param[in]	nMarketID			市场ID
	 * @param[in]	UnitNo				服务单元号
	 */
	int				inner_StartOneMarket(int nMarketID, const char* pszModulePath, const char* pszDllPath, int UnitNo);

	/**
	 * @brief		停止指定的市场传输驱动
	 * @param[in]	nMarketID			市场ID
	 */
	void			inner_StopOneMarket(int nMarketID);

	/**
	 * @brief		添加一个新的市场状态到维护列表
	 * @param[in]	nMarketID			市场ID
	 * @param[in]	nInnerStat			驱动的启动状态
	 * @param[in]	nOuterStat			驱动的服务状态
	 */
	void			inner_AddMarket(int nMarketID, int nInnerStat, int nOuterStat);

	/**
	 * @brief		清理各市场资源状态
	 */
	void			inner_FreeMarket();

	/**
	 * @brief		取得驱动运行状态(XDFRunStat)
	 */
	int				inner_CheckOneMarket(Base4XDll* pObj);

protected:
	vector<tagOneMarketStat>	m_arrStat;			///< 各大市场的状态维护列表		
	MCriticalSection			m_Section;

	Base4XDll					m_oShSe;			///< 上海市场传输驱动DLL调用封装
	Base4XDll					m_oSzSe;			///< 深圳市场传输驱动DLL调用封装
	Base4XDll					m_oShOpt;			///< 上海期权市场传输驱动DLL调用封装
	Base4XDll					m_oZjqh;			///< 中金期货市场传输驱动DLL调用封装
	Base4XDll					m_oZjOpt;			///< 中金期权市场传输驱动DLL调用封装
	Base4XDll					m_oCnf;				///< 市场传输驱动DLL调用封装
	Base4XDll					m_oSzOpt;			///< 深圳期权市场传输驱动DLL调用封装
	Base4XDll					m_oCnfOpt;			///< 市场传输驱动DLL调用封装
};


/**
 * @brief		数据管理及全局调用, 管理主键为市场ID
 * @class		MDllMgr
 */
class MDllMgr
{
	friend class	MPrimeClient;
public:
	MDllMgr();
	virtual ~MDllMgr();

public:
	/**
	 * @brief		初始化各资源
	 */
	int				Instance();

	/**
	 * @brief		释放资源
	 */
	void			Release();

	/**
	 * @brief		通过状态管理对象M4XDllStat，启动加载各市场模块
	 * @return		> 0				成功
					< 0				失败
	 */
	int				BeginWork();

	/**
	 * @brief		停止所有市场的传输服务，并清空资源
	 */
	void			EndWork();

public:	
	int				GetCodeTable(uint8_t cMarket, char* pszInBuf, int32_t nInBytes, int32_t& nCount);
	int				GetCodeTableEx(uint8_t cMarket, char* pszInBuf, int32_t nInBytes, int32_t& nCount);
	int				GetMarketInfo(uint8_t cMarket, char* pszInBuf, int32_t nInBytes);
	int				GetLastMarketDataAll(uint8_t cMarket, char* pszInBuf, int32_t nInBytes);
	int				GetMarketStatus(uint8_t cMarket,int32_t& nStatus, uint32_t& ulTime, int64_t * pI64Send, int64_t * pI64Recv);
	int				GetMarketStat(unsigned char MarketId);

	/**
	 * @brief		根据市场id取对应的日期、时间、市场状态
	 */
	int				GetSimpleMarketInfo(uint8_t cMarket, XDFAPI_MarketStatusInfo* pInfo);

	/**
	 * @brief		根据市场id取对应的日期、时间、市场状态(无阻塞查询)
	 */
	int				GetQuicksSimpleMarketInfo(uint8_t cMarket, XDFAPI_MarketStatusInfo* pInfo);

public:
	/**
	 * @brief		检查进来的协议是不是合法的推送协议
	 */
	bool			outer_ExpectData(unsigned char MarketId,unsigned char MainType, unsigned char ChildType);

	/**
	 * @brief		推送回调
	 */
	void			outer_OnPushData(unsigned char MarketId,unsigned char ChildType, const char* pszbuf, unsigned short bufsize);

	void			outer_CloseSub(unsigned char MarketId);

public:
	MData4XDll*		GetMarketDll(int nMarketId);
	
protected:
	static void* STDCALL	TaskThreadFunc(void *pParam);	//处理状态和日志
	
protected:
	
	MShOptDll					m_oShOptDll;		///< 上海期权
	MZjOptDll					m_oZjOptDll;		///< 中金期权
	MZjqhDll					m_oZjqhDll;			///< 中金期货
	MShSEDll					m_oShSEDll;			///< 上海证券市场
	MSzSEDll					m_oSzSEDll;			///< 深圳证券市场
	MCnfDll						m_oCnfDll;			///< 商品期货(老版本的)
	MSzOptDll					m_oSzOptDll;		///< 深圳期权
	MCnfOptDll					m_oCnfOptDll;		///< 商品期权(新版本的)

	MThread						m_threadClient;		///< 状态监控线程
	M4XDllStat					m_oWorkStat;		///< 传输平台(DLL)管理模块
	bool						m_bWorking;

	MCounter					m_oCounter;
private:
	
	
};


#endif



























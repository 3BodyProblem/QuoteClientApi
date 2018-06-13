#ifndef _DATACLIENT_DEFINE_H
#define _DATACLIENT_DEFINE_H


#pragma warning(disable : 4786)
#include <map>
#include <vector>
#include <string>
#include "MEngine.hpp"
#include "../MPublic/MInterface.hpp"
#include <windows.h>


/**
 * @class	MDataClient
 * @brief	api接口的实现
 */
class MDataClient : public QuoteClientApi
{
public:
	MDataClient();
	virtual ~MDataClient();

public:
	/**
	 * @brief			只作模块初始化，不启动各市场传输
	 */
	int STDCALL			Init();

	/**
	 * @brief			释放资源
	 */
	void STDCALL		Release();

	/**
	 * @brief			注册回调实例
	 */
	void STDCALL		RegisterSpi(QuoteClientSpi * pspi);

	/**
	 * @brief			启动各市场传输驱动
	 */
	int STDCALL			BeginWork();

	/**
	 * @brief			关闭各市场传输驱动
	 */
	void STDCALL		EndWork();

	int	 STDCALL		GetMarketInfo(unsigned char cMarket, char* pszInBuf, int nInBytes);

	int	STDCALL			GetCodeTable(unsigned char cMarket, char* pszInBuf, int nInBytes, int& nCount);
	
	int STDCALL			GetLastMarketDataAll(unsigned char cMarket, char* pszInBuf, int nInBytes);
	
	int STDCALL			GetMarketStatus(unsigned char cMarket,int& nStatus, unsigned int& ulTime, __int64 * pI64Send, __int64 * pI64Recv);
	

protected:

private:

};


///< --------------------- 60 分钟线 --------------------------------------------------
/**
 * @class						SHSZ_60Minute_Lines
 * @brief						对应沪深某商品的一天内所有60分钟线
 */
typedef struct
{
	///< 计算中的60分钟线(保存最后一次快照)，或已经计算完成的60分钟线
	T_Minute_Line				m_tag1030Line;				///< 第一根线
	T_Minute_Line				m_tag1300Line;				///< 第二根线
	T_Minute_Line				m_tag1400Line;				///< 第三根线
	T_Minute_Line				m_tag1500Line;				///< 第四根线
	///< 计算参考信息
	char						Code[16];					///< 商品代码
	char						m_cMkID;					///< 市场ID
	double						m_fPriceRate;				///< 放大比率
	///< 计算参数(通常是第一笔的快照信息)
	unsigned int				m_nDate1;					///< 第一笔快照的市场日期
	unsigned int				m_nTime1;					///< 第一笔快照的市场时间
	double						m_f103000PricePx1;			///< 第一笔快照的现价
	double						m_f103000Amount1;			///< 第一笔快照的成交金额
	unsigned __int64			m_n103000Volume1;			///< 第一笔快照的成交量
	unsigned int				m_n103000NumTrades1;		///< 成交笔数
	double						m_f130000PricePx1;			///< 第一笔快照的现价
	double						m_f130000Amount1;			///< 第一笔快照的成交金额
	unsigned __int64			m_n130000Volume1;			///< 第一笔快照的成交量
	unsigned int				m_n130000NumTrades1;		///< 成交笔数
	double						m_f140000PricePx1;			///< 第一笔快照的现价
	double						m_f140000Amount1;			///< 第一笔快照的成交金额
	unsigned __int64			m_n140000Volume1;			///< 第一笔快照的成交量
	unsigned int				m_n140000NumTrades1;		///< 成交笔数
	double						m_f150000PricePx1;			///< 第一笔快照的现价
	double						m_f150000Amount1;			///< 第一笔快照的成交金额
	unsigned __int64			m_n150000Volume1;			///< 第一笔快照的成交量
	unsigned int				m_n150000NumTrades1;		///< 成交笔数
} SHSZ_60Minute_Lines;

/**
 * @class						Minute60LineStatus
 * @brief						分钟线状态
 */
class Minute60LineStatus : public SHSZ_60Minute_Lines
{
public:
	Minute60LineStatus();
	Minute60LineStatus( const Minute60LineStatus& refObj );
	Minute60LineStatus( const SHSZ_60Minute_Lines& refObj );

	/**
	 * @brief					更新分钟线
	 * @param[in]				cMkID						市场ID
	 * @param[in]				nDate						市场日期
	 * @param[in]				nTime						市场时间
	 * @param[in]				dPriceRate					放大倍数
	 */
	void						SetRefParam( char cMkID, const char* pszCode, unsigned int nDate, unsigned int nTime, double dPriceRate );

	/**
	 * @brief					更新实时数据
	 * @param[in]				refObj						新分钟线数据
	 * @param[in]				bInit						是否为初始化状态
	 * @note					考虑了，新数据和旧数据不在同一分钟内的情况
	 */
	void						Update( T_Minute_Line& refObj, bool bInit = false );

	/**
	 * @brief					更新指数快照
	 * @param[in]				nMkTime						市场时间
	 * @param[in]				refObj						新分钟线数据
	 * @note					考虑了，新数据和旧数据不在同一分钟内的情况
	 */
	void						Update( unsigned int nDate, unsigned int nMkTime, tagCcComm_IndexData& refData );

	/**
	 * @brief					更新股票快照
	 * @param[in]				nMkTime						市场时间
	 * @param[in]				refObj						新分钟线数据
	 * @note					考虑了，新数据和旧数据不在同一分钟内的情况
	 */
	void						Update( unsigned int nDate, unsigned int nMkTime, tagCcComm_StockData5& refData );

	/**
	 * @brief					激活分钟线的回调机制
	 * @param[in]				nPeriodIndex				时段周期索引
	 */
	void						DoCallBack( unsigned int nPeriodIndex );

	/**
	 * @brief					激活分钟线的查询响应机制
	 * @param[in]				nPeriodIndex				时段周期索引
	 */
	void						DoQueryResponse( unsigned int nPeriodIndex, unsigned int nReqID, bool bIsLast );

protected:
	bool						m_bCalled0;					///< 第一时段已经回调过了的标识
	bool						m_bCalled1;					///< 第一时段已经回调过了的标识
	bool						m_bCalled2;					///< 第一时段已经回调过了的标识
	bool						m_bCalled3;					///< 第一时段已经回调过了的标识
};


///< 1分钟线计算并落盘类 ///////////////////////////////////////////////////////////////////////////////////////////


/**
 * @class						Min1Sync
 * @brief						沪、深一分钟线同步类
 * @author						barry
 */
class Min1Sync
{
public:
	Min1Sync( enum XDFMarket eMkID );

	/**
	 * @brief					异步同步1分钟线数据
	 * @return					true					启动同步线程,成功
	 */
	bool						Sync();

	/**
	 * @brief					1分钟线数据是否已经下载完成
	 * @return					true					下载成功
	 */
	bool						IsLoaded();

protected:
	static void*	__stdcall	SyncThread( void* pSelf );

protected:
	bool						m_bSyned;				///< 是否已经同步成功
	MThread						m_oSyncThread;			///< 分钟线落盘数据
	enum XDFMarket				m_eMarketID;			///< 市场编号
};


/**
 * @class						T_MIN_LINE
 * @brief						分钟线
 */
typedef struct
{
	unsigned int				Date;					///< YYYYMMDD（如20170705）
	unsigned int				Time;					///< HHMMSSmmm（如093005000)
	double						OpenPx;					///< 开盘价一分钟内的第一笔的nowpx
	double						HighPx;					///< 最高价一分钟内的 最高的highpx
	double						LowPx;					///< 最低价一分钟内的 最低的lowpx
	double						ClosePx;				///< 收盘价一分钟内最后一笔的Nowpx
	double						SettlePx;				///< 结算价一分钟内最后一笔的settlepx
	double						Amount;					///< 成交额一分钟最后笔减去第一笔的amount
	unsigned __int64			Volume;					///< 成交量(股/张/份)一分钟最后笔减去第一笔的volume
	unsigned __int64			OpenInterest;			///< 持仓量(股/张/份)一分钟最后一笔
	unsigned __int64			NumTrades;				///< 成交笔数一分钟最后笔减去第一笔的numtrades
	double						Voip;					///< 基金模净、权证溢价一分钟的最后一笔voip
} T_MIN_LINE;

/**
 * @class						MinGenerator
 * @brief						商品1分钟线生产者
 * @author						barry
 */
class MinGenerator
{
public:
	typedef struct
	{
		unsigned int			Time;					///< HHMMSSmmm（如093005000)
		unsigned int			OpenPx;
		unsigned int			HighPx;
		unsigned int			LowPx;
		unsigned int			ClosePx;
		unsigned int			Voip;
		double					Amount;
		unsigned __int64		Volume;
		unsigned __int64		NumTrades;
	} T_DATA;											///< 商品快照
	const unsigned int			m_nMaxLineCount;		///< 最多支持的分钟线数量(241根)
public:
	MinGenerator();
	MinGenerator( enum XDFMarket eMkID, unsigned int nDate, const std::string& sCode, double dPriceRate, T_DATA& objData, T_DATA* pBufPtr );
	MinGenerator&				operator=( const MinGenerator& obj );

	/**
	 * @brief					初始化
	 * @return					0						成功
	 */
	int							Initialize();

	/**
	 * @brief					将行情中的每分钟的第一笔快照，更新到对应的241条分钟线数据槽中
	 * @param[in]				objData					行情快照
	 * @return					0						成功
	 */
	int							Update( T_DATA& objData );

	/**
	 * @brief					用1分钟线给对应的缓存块直接赋值(用于从1分钟同步文件加载)
	 * @param[in]				objData					行情快照
	 * @return					true					成功
	 */
	bool						AssignMin1( T_DATA& objData );

	/**
	 * @brief					获取放大倍数
	 */
	double						GetPxRate();

	/**
	 * @brief					生成分钟线并存盘
	 */
	void						DumpMinutes();

	/**
	 * @brief					查询今日的分1钟线记录并回调
	 * @param[in]				nReqID			请求ID
	 * @param[in]				nBeginTime		查询记录的开始时间
	 * @param[in]				nEndTime		查询记录的结束时间
	 * @param[in]				bIsLastCode		最后一条个商品Code
	 * @return					查询条数
	 */
	int							CallBack4Query( int nReqID, unsigned int nBeginTime, unsigned int nEndTime, bool bIsLastCode );

protected:
	double						m_dAmountBefore930;		///< 9:30前的金额
	unsigned __int64			m_nVolumeBefore930;		///< 9:30前的量
	unsigned __int64			m_nNumTradesBefore930;	///< 9:30前的笔数
protected:
	double						m_dPriceRate;			///< 放大倍数
	enum XDFMarket				m_eMarket;				///< 市场编号
	unsigned int				m_nDate;				///< YYYYMMDD（如20170705）
	char						m_pszCode[9];			///< 商品代码
protected:
	T_DATA*						m_pDataCache;			///< 241根1分钟缓存
	int							m_nWriteSize;			///< 写入分钟线的长度
	int							m_nDataSize;			///< 数据长度
};


/**
 * @class						SecurityMinCache
 * @brief						所有商品分钟线的缓存
 * @author						barry
 */
class SecurityMinCache
{
public:
	typedef std::map<std::string,MinGenerator>	T_MAP_MINUTES;
public:
	SecurityMinCache( enum XDFMarket eMkID );
	~SecurityMinCache();

	/**
	 * @brief					分钟线缓存初始化
	 * @param[in]				nSecurityCount			市场中的商品数量
	 * @return					0						成功
	 */
	int							Initialize( unsigned int nSecurityCount );

	/**
	 * @brief					释放资源
	 */
	void						Release();

public:
	/**
	 * @brief					激活分钟线落盘线程
	 */
	void						ActivateDumper();

	/**
	 * @brief					更新商品的参数信息
	 * @param[in]				eMarket			市场ID
	 * @param[in]				sCode			商品代码
	 * @param[in]				nDate			行情日期
	 * @param[in]				dPriceRate		放大倍率
	 * @param[in]				objData			行情价格
	 * @return					==0				成功
	 */
	int							NewSecurity( enum XDFMarket eMarket, const std::string& sCode, unsigned int nDate, double dPriceRate, MinGenerator::T_DATA& objData );

	/**
	 * @brief					更新商品信息
	 * @param[in]				objData			行情价格
	 * @return					==0				成功
	 */
	int							UpdateSecurity( const tagCcComm_IndexData& refObj, unsigned int nDate, unsigned int nTime );
	int							UpdateSecurity( const tagCcComm_StockData5& refObj, unsigned int nDate, unsigned int nTime );

	/**
	 * @brief					查询今日的分1钟线记录并回调
	 * @param[in]				nReqID			请求ID
	 * @param[in]				nBeginTime		查询记录的开始时间
	 * @param[in]				nEndTime		查询记录的结束时间
	 * @return					查询条数
	 */
	int							QueryRecords( int nReqID, unsigned int nBeginTime, unsigned int nEndTime );

protected:
	/**
	 * @brief					从同步数据文件，加载今日的1分钟线数据
	 */
	void						LoadFromSyncDataFile();

protected:
	static void*	__stdcall	RealCallbackAndLoadThread( void* pSelf );

protected:
	enum XDFMarket				m_eMarketID;			///< 市场编号
	bool						m_bSyncDataLoaded;		///< 是否已经加载同步数据
	Min1Sync					m_objSyncMin1;			///< 1分钟线同步对象
	MThread						m_oRealCbAndLoadThread;	///< 分钟线实时回调和今日内历史数据加载线程
	std::string					m_sLastValidCode;		///< 最后一个代码，用于计算是否已经最后一个查询回调商品
protected:
	unsigned int				m_nAlloPos;				///< 缓存已经分配的位置
	unsigned int				m_nSecurityCount;		///< 商品数量
	MinGenerator::T_DATA*		m_pMinDataTable;		///< 分钟线缓存
	T_MAP_MINUTES				m_objMapMinutes;		///< 分钟线Map
	std::vector<std::string>	m_vctCode;				///< CodeInTicks
	MCriticalSection			m_oLockData;			///< 分钟线数据锁
};


/**
 * @brief						当日行情数据查询API
 * @class						MQueryClient
 * @author						barry
 * @date						2018/3/28
 */
class MQueryClient : public QuoteQueryApi
{
	typedef	std::map<std::string,Minute60LineStatus>	TMAP_60MINU_LINE;

public:///< 对外方法
	MQueryClient();

	/**
	 * @brief					初始化
	 * @return					==0				成功
	 */
	int	STDCALL					Initialize();

	/**
	 * @brief					释放资源
	 */
	void STDCALL				Release();

	/**
	 * @brief					注册查询回调接口
	 * @param[in]				pSpi			查询回调对象指针
	 */
	void STDCALL				RegisterSpi( QuoteQuerySpi* pSpi, int nMinuteLineType );

	/**
	 * @brief					请求分钟线
	 * @param[in]				nRequestID		请求ID
	 * @param[in]				nBeginTime		查询内容的开始时间
	 * @param[in]				nEndTime		查询内容的结束时间
	 * @return					==0				请求成功
								-1				还有在执行中的请求，当前本次请求被忽略
								-2				其他错误
	 */
	int STDCALL					QueryMinuteLines( enum XDFMarket eMkID, unsigned int nRequestID, unsigned int nBeginTime, unsigned int nEndTime );

	/**
	 * @brief					获取回调对象指针
	 */
	QuoteQuerySpi*				GetHandle();

public:///< 行情数据回调、通知
	/**
	 * @brief					市场可用状态的通知事件
	 * @param[in]				cMkID			市场ID
	 * @param[in]				nStatus			可服务状态
	 */
	void						OnMarketAvailableNotify( unsigned char cMkID, int nStatus );

	/**
	 * @brief					实时行情分钟线更新回调
	 */
	void						OnSHL1Index( unsigned int nMkTime, tagCcComm_IndexData& refData );
	void						OnSHL1Snap( unsigned int nMkTime, tagCcComm_StockData5& refData );
	void						OnSZL1Index( unsigned int nMkTime, tagCcComm_IndexData& refData );
	void						OnSZL1Snap( unsigned int nMkTime, tagCcComm_StockData5& refData );

protected:
	/**
	 * @brief					加载上海Lv1的分钟线数据
	 * @return					返回加载的商品数
	 */
	int							LoadMinuteLine4SHL1();

	/**
	 * @brief					加载深圳Lv1的分钟线数据
	 * @return					返回加载的商品数
	 */
	int							LoadMinuteLine4SZL1();

	/**
	 * @brief					设置上海Lv1市场状态是否为可用（标识可用商品数量）
	 * @param[in]				nWareCount		可用商品数量
	 */
	void						SetWareCount4SHL1( int nWareCount );

	/**
	 * @brief					设置深圳Lv1市场状态是否为可用（标识可用商品数量）
	 * @param[in]				nWareCount		可用商品数量
	 */
	void						SetWareCount4SZL1( int nWareCount );

protected:
	/**
	 * @brief					查询处理数据派发
	 */
	static void* STDCALL		QueryThreadFunc( void* pParam );

private:
	MThread						m_oQueryThread;					///< 查询响应线程
	MThread						m_oCallBackThread;				///< 查询响应线程
	QuoteQuerySpi*				m_pQuoteQuerySpi;				///< 查询响应回调
	bool						m_bIsQuerying;					///< 任务查询中
	enum XDFMarket				m_eReqMkID;						///< 请求的市场
	unsigned int				m_nRequestID;					///< 请求编号
	unsigned int				m_nBeginTime;					///< 要查询分钟线的开始时间
	unsigned int				m_nEndTime;						///< 要查询分钟线的结束时间

private:
	int							m_nWareCount4SHL1;				///< 上海Lv1初始化成功标识
	unsigned int				m_nMkTime4SHL1;					///< 上海Lv1市场时间
	unsigned int				m_nMkDate4SHL1;					///< 上海Lv1市场日期
	int							m_nWareCount4SZL1;				///< 深圳Lv1初始化成功标识
	unsigned int				m_nMkTime4SZL1;					///< 深圳Lv1市场时间
	unsigned int				m_nMkDate4SZL1;					///< 上海Lv1市场日期
	SecurityMinCache			m_obj1MinCache4SHL1;			///< 上证L1分钟线缓存
	SecurityMinCache			m_obj1MinCache4SZL1;			///< 深证L1分钟线缓存
};


/**
 * @class		MPrimeClient
 * @brief		数据查询类
 */
class MPrimeClient : public QuotePrimeApi
{
public:
	MPrimeClient();
	virtual ~MPrimeClient();

public:
	/**
	 * @brief			请求查询数据
	 * @param[in]		FuncNo			100:同步方式查询某市场信息XDFAPI_MarketStatusInfo
										101:异步方式查询某市场信息XDFAPI_MarketStatusInfo
										102:获取沪、深的停牌标记
										103:获取中金市场码表 (pParam, 为空，则return码表中的商品数量)
	 */
	int		STDCALL		ReqFuncData( int FuncNo, void* wParam, void* lParam );

};






#endif












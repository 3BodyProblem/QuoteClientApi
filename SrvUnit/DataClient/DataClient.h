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
 * @brief	api�ӿڵ�ʵ��
 */
class MDataClient : public QuoteClientApi
{
public:
	MDataClient();
	virtual ~MDataClient();

public:
	/**
	 * @brief			ֻ��ģ���ʼ�������������г�����
	 */
	int STDCALL			Init();

	/**
	 * @brief			�ͷ���Դ
	 */
	void STDCALL		Release();

	/**
	 * @brief			ע��ص�ʵ��
	 */
	void STDCALL		RegisterSpi(QuoteClientSpi * pspi);

	/**
	 * @brief			�������г���������
	 */
	int STDCALL			BeginWork();

	/**
	 * @brief			�رո��г���������
	 */
	void STDCALL		EndWork();

	int	 STDCALL		GetMarketInfo(unsigned char cMarket, char* pszInBuf, int nInBytes);

	int	STDCALL			GetCodeTable(unsigned char cMarket, char* pszInBuf, int nInBytes, int& nCount);
	
	int STDCALL			GetLastMarketDataAll(unsigned char cMarket, char* pszInBuf, int nInBytes);
	
	int STDCALL			GetMarketStatus(unsigned char cMarket,int& nStatus, unsigned int& ulTime, __int64 * pI64Send, __int64 * pI64Recv);
	

protected:

private:

};


///< --------------------- 60 ������ --------------------------------------------------
/**
 * @class						SHSZ_60Minute_Lines
 * @brief						��Ӧ����ĳ��Ʒ��һ��������60������
 */
typedef struct
{
	///< �����е�60������(�������һ�ο���)�����Ѿ�������ɵ�60������
	T_Minute_Line				m_tag1030Line;				///< ��һ����
	T_Minute_Line				m_tag1300Line;				///< �ڶ�����
	T_Minute_Line				m_tag1400Line;				///< ��������
	T_Minute_Line				m_tag1500Line;				///< ���ĸ���
	///< ����ο���Ϣ
	char						Code[16];					///< ��Ʒ����
	char						m_cMkID;					///< �г�ID
	double						m_fPriceRate;				///< �Ŵ����
	///< �������(ͨ���ǵ�һ�ʵĿ�����Ϣ)
	unsigned int				m_nDate1;					///< ��һ�ʿ��յ��г�����
	unsigned int				m_nTime1;					///< ��һ�ʿ��յ��г�ʱ��
	double						m_f103000PricePx1;			///< ��һ�ʿ��յ��ּ�
	double						m_f103000Amount1;			///< ��һ�ʿ��յĳɽ����
	unsigned __int64			m_n103000Volume1;			///< ��һ�ʿ��յĳɽ���
	unsigned int				m_n103000NumTrades1;		///< �ɽ�����
	double						m_f130000PricePx1;			///< ��һ�ʿ��յ��ּ�
	double						m_f130000Amount1;			///< ��һ�ʿ��յĳɽ����
	unsigned __int64			m_n130000Volume1;			///< ��һ�ʿ��յĳɽ���
	unsigned int				m_n130000NumTrades1;		///< �ɽ�����
	double						m_f140000PricePx1;			///< ��һ�ʿ��յ��ּ�
	double						m_f140000Amount1;			///< ��һ�ʿ��յĳɽ����
	unsigned __int64			m_n140000Volume1;			///< ��һ�ʿ��յĳɽ���
	unsigned int				m_n140000NumTrades1;		///< �ɽ�����
	double						m_f150000PricePx1;			///< ��һ�ʿ��յ��ּ�
	double						m_f150000Amount1;			///< ��һ�ʿ��յĳɽ����
	unsigned __int64			m_n150000Volume1;			///< ��һ�ʿ��յĳɽ���
	unsigned int				m_n150000NumTrades1;		///< �ɽ�����
} SHSZ_60Minute_Lines;

/**
 * @class						Minute60LineStatus
 * @brief						������״̬
 */
class Minute60LineStatus : public SHSZ_60Minute_Lines
{
public:
	Minute60LineStatus();
	Minute60LineStatus( const Minute60LineStatus& refObj );
	Minute60LineStatus( const SHSZ_60Minute_Lines& refObj );

	/**
	 * @brief					���·�����
	 * @param[in]				cMkID						�г�ID
	 * @param[in]				nDate						�г�����
	 * @param[in]				nTime						�г�ʱ��
	 * @param[in]				dPriceRate					�Ŵ���
	 */
	void						SetRefParam( char cMkID, const char* pszCode, unsigned int nDate, unsigned int nTime, double dPriceRate );

	/**
	 * @brief					����ʵʱ����
	 * @param[in]				refObj						�·���������
	 * @param[in]				bInit						�Ƿ�Ϊ��ʼ��״̬
	 * @note					�����ˣ������ݺ;����ݲ���ͬһ�����ڵ����
	 */
	void						Update( T_Minute_Line& refObj, bool bInit = false );

	/**
	 * @brief					����ָ������
	 * @param[in]				nMkTime						�г�ʱ��
	 * @param[in]				refObj						�·���������
	 * @note					�����ˣ������ݺ;����ݲ���ͬһ�����ڵ����
	 */
	void						Update( unsigned int nDate, unsigned int nMkTime, tagCcComm_IndexData& refData );

	/**
	 * @brief					���¹�Ʊ����
	 * @param[in]				nMkTime						�г�ʱ��
	 * @param[in]				refObj						�·���������
	 * @note					�����ˣ������ݺ;����ݲ���ͬһ�����ڵ����
	 */
	void						Update( unsigned int nDate, unsigned int nMkTime, tagCcComm_StockData5& refData );

	/**
	 * @brief					��������ߵĻص�����
	 * @param[in]				nPeriodIndex				ʱ����������
	 */
	void						DoCallBack( unsigned int nPeriodIndex );

	/**
	 * @brief					��������ߵĲ�ѯ��Ӧ����
	 * @param[in]				nPeriodIndex				ʱ����������
	 */
	void						DoQueryResponse( unsigned int nPeriodIndex, unsigned int nReqID, bool bIsLast );

protected:
	bool						m_bCalled0;					///< ��һʱ���Ѿ��ص����˵ı�ʶ
	bool						m_bCalled1;					///< ��һʱ���Ѿ��ص����˵ı�ʶ
	bool						m_bCalled2;					///< ��һʱ���Ѿ��ص����˵ı�ʶ
	bool						m_bCalled3;					///< ��һʱ���Ѿ��ص����˵ı�ʶ
};


///< 1�����߼��㲢������ ///////////////////////////////////////////////////////////////////////////////////////////


/**
 * @class						Min1Sync
 * @brief						������һ������ͬ����
 * @author						barry
 */
class Min1Sync
{
public:
	Min1Sync( enum XDFMarket eMkID );

	/**
	 * @brief					�첽ͬ��1����������
	 * @return					true					����ͬ���߳�,�ɹ�
	 */
	bool						Sync();

	/**
	 * @brief					1�����������Ƿ��Ѿ��������
	 * @return					true					���سɹ�
	 */
	bool						IsLoaded();

protected:
	static void*	__stdcall	SyncThread( void* pSelf );

protected:
	bool						m_bSyned;				///< �Ƿ��Ѿ�ͬ���ɹ�
	MThread						m_oSyncThread;			///< ��������������
	enum XDFMarket				m_eMarketID;			///< �г����
};


/**
 * @class						T_MIN_LINE
 * @brief						������
 */
typedef struct
{
	unsigned int				Date;					///< YYYYMMDD����20170705��
	unsigned int				Time;					///< HHMMSSmmm����093005000)
	double						OpenPx;					///< ���̼�һ�����ڵĵ�һ�ʵ�nowpx
	double						HighPx;					///< ��߼�һ�����ڵ� ��ߵ�highpx
	double						LowPx;					///< ��ͼ�һ�����ڵ� ��͵�lowpx
	double						ClosePx;				///< ���̼�һ���������һ�ʵ�Nowpx
	double						SettlePx;				///< �����һ���������һ�ʵ�settlepx
	double						Amount;					///< �ɽ���һ�������ʼ�ȥ��һ�ʵ�amount
	unsigned __int64			Volume;					///< �ɽ���(��/��/��)һ�������ʼ�ȥ��һ�ʵ�volume
	unsigned __int64			OpenInterest;			///< �ֲ���(��/��/��)һ�������һ��
	unsigned __int64			NumTrades;				///< �ɽ�����һ�������ʼ�ȥ��һ�ʵ�numtrades
	double						Voip;					///< ����ģ����Ȩ֤���һ���ӵ����һ��voip
} T_MIN_LINE;

/**
 * @class						MinGenerator
 * @brief						��Ʒ1������������
 * @author						barry
 */
class MinGenerator
{
public:
	typedef struct
	{
		unsigned int			Time;					///< HHMMSSmmm����093005000)
		unsigned int			OpenPx;
		unsigned int			HighPx;
		unsigned int			LowPx;
		unsigned int			ClosePx;
		unsigned int			Voip;
		double					Amount;
		unsigned __int64		Volume;
		unsigned __int64		NumTrades;
	} T_DATA;											///< ��Ʒ����
	const unsigned int			m_nMaxLineCount;		///< ���֧�ֵķ���������(241��)
public:
	MinGenerator();
	MinGenerator( enum XDFMarket eMkID, unsigned int nDate, const std::string& sCode, double dPriceRate, T_DATA& objData, T_DATA* pBufPtr );
	MinGenerator&				operator=( const MinGenerator& obj );

	/**
	 * @brief					��ʼ��
	 * @return					0						�ɹ�
	 */
	int							Initialize();

	/**
	 * @brief					�������е�ÿ���ӵĵ�һ�ʿ��գ����µ���Ӧ��241�����������ݲ���
	 * @param[in]				objData					�������
	 * @return					0						�ɹ�
	 */
	int							Update( T_DATA& objData );

	/**
	 * @brief					��1�����߸���Ӧ�Ļ����ֱ�Ӹ�ֵ(���ڴ�1����ͬ���ļ�����)
	 * @param[in]				objData					�������
	 * @return					true					�ɹ�
	 */
	bool						AssignMin1( T_DATA& objData );

	/**
	 * @brief					��ȡ�Ŵ���
	 */
	double						GetPxRate();

	/**
	 * @brief					���ɷ����߲�����
	 */
	void						DumpMinutes();

	/**
	 * @brief					��ѯ���յķ�1���߼�¼���ص�
	 * @param[in]				nReqID			����ID
	 * @param[in]				nBeginTime		��ѯ��¼�Ŀ�ʼʱ��
	 * @param[in]				nEndTime		��ѯ��¼�Ľ���ʱ��
	 * @param[in]				bIsLastCode		���һ������ƷCode
	 * @return					��ѯ����
	 */
	int							CallBack4Query( int nReqID, unsigned int nBeginTime, unsigned int nEndTime, bool bIsLastCode );

protected:
	double						m_dAmountBefore930;		///< 9:30ǰ�Ľ��
	unsigned __int64			m_nVolumeBefore930;		///< 9:30ǰ����
	unsigned __int64			m_nNumTradesBefore930;	///< 9:30ǰ�ı���
protected:
	double						m_dPriceRate;			///< �Ŵ���
	enum XDFMarket				m_eMarket;				///< �г����
	unsigned int				m_nDate;				///< YYYYMMDD����20170705��
	char						m_pszCode[9];			///< ��Ʒ����
protected:
	T_DATA*						m_pDataCache;			///< 241��1���ӻ���
	int							m_nWriteSize;			///< д������ߵĳ���
	int							m_nDataSize;			///< ���ݳ���
};


/**
 * @class						SecurityMinCache
 * @brief						������Ʒ�����ߵĻ���
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
	 * @brief					�����߻����ʼ��
	 * @param[in]				nSecurityCount			�г��е���Ʒ����
	 * @return					0						�ɹ�
	 */
	int							Initialize( unsigned int nSecurityCount );

	/**
	 * @brief					�ͷ���Դ
	 */
	void						Release();

public:
	/**
	 * @brief					��������������߳�
	 */
	void						ActivateDumper();

	/**
	 * @brief					������Ʒ�Ĳ�����Ϣ
	 * @param[in]				eMarket			�г�ID
	 * @param[in]				sCode			��Ʒ����
	 * @param[in]				nDate			��������
	 * @param[in]				dPriceRate		�Ŵ���
	 * @param[in]				objData			����۸�
	 * @return					==0				�ɹ�
	 */
	int							NewSecurity( enum XDFMarket eMarket, const std::string& sCode, unsigned int nDate, double dPriceRate, MinGenerator::T_DATA& objData );

	/**
	 * @brief					������Ʒ��Ϣ
	 * @param[in]				objData			����۸�
	 * @return					==0				�ɹ�
	 */
	int							UpdateSecurity( const tagCcComm_IndexData& refObj, unsigned int nDate, unsigned int nTime );
	int							UpdateSecurity( const tagCcComm_StockData5& refObj, unsigned int nDate, unsigned int nTime );

	/**
	 * @brief					��ѯ���յķ�1���߼�¼���ص�
	 * @param[in]				nReqID			����ID
	 * @param[in]				nBeginTime		��ѯ��¼�Ŀ�ʼʱ��
	 * @param[in]				nEndTime		��ѯ��¼�Ľ���ʱ��
	 * @return					��ѯ����
	 */
	int							QueryRecords( int nReqID, unsigned int nBeginTime, unsigned int nEndTime );

protected:
	/**
	 * @brief					��ͬ�������ļ������ؽ��յ�1����������
	 */
	void						LoadFromSyncDataFile();

protected:
	static void*	__stdcall	RealCallbackAndLoadThread( void* pSelf );

protected:
	enum XDFMarket				m_eMarketID;			///< �г����
	bool						m_bSyncDataLoaded;		///< �Ƿ��Ѿ�����ͬ������
	Min1Sync					m_objSyncMin1;			///< 1������ͬ������
	MThread						m_oRealCbAndLoadThread;	///< ������ʵʱ�ص��ͽ�������ʷ���ݼ����߳�
	std::string					m_sLastValidCode;		///< ���һ�����룬���ڼ����Ƿ��Ѿ����һ����ѯ�ص���Ʒ
protected:
	unsigned int				m_nAlloPos;				///< �����Ѿ������λ��
	unsigned int				m_nSecurityCount;		///< ��Ʒ����
	MinGenerator::T_DATA*		m_pMinDataTable;		///< �����߻���
	T_MAP_MINUTES				m_objMapMinutes;		///< ������Map
	std::vector<std::string>	m_vctCode;				///< CodeInTicks
	MCriticalSection			m_oLockData;			///< ������������
};


/**
 * @brief						�����������ݲ�ѯAPI
 * @class						MQueryClient
 * @author						barry
 * @date						2018/3/28
 */
class MQueryClient : public QuoteQueryApi
{
	typedef	std::map<std::string,Minute60LineStatus>	TMAP_60MINU_LINE;

public:///< ���ⷽ��
	MQueryClient();

	/**
	 * @brief					��ʼ��
	 * @return					==0				�ɹ�
	 */
	int	STDCALL					Initialize();

	/**
	 * @brief					�ͷ���Դ
	 */
	void STDCALL				Release();

	/**
	 * @brief					ע���ѯ�ص��ӿ�
	 * @param[in]				pSpi			��ѯ�ص�����ָ��
	 */
	void STDCALL				RegisterSpi( QuoteQuerySpi* pSpi, int nMinuteLineType );

	/**
	 * @brief					���������
	 * @param[in]				nRequestID		����ID
	 * @param[in]				nBeginTime		��ѯ���ݵĿ�ʼʱ��
	 * @param[in]				nEndTime		��ѯ���ݵĽ���ʱ��
	 * @return					==0				����ɹ�
								-1				������ִ���е����󣬵�ǰ�������󱻺���
								-2				��������
	 */
	int STDCALL					QueryMinuteLines( enum XDFMarket eMkID, unsigned int nRequestID, unsigned int nBeginTime, unsigned int nEndTime );

	/**
	 * @brief					��ȡ�ص�����ָ��
	 */
	QuoteQuerySpi*				GetHandle();

public:///< �������ݻص���֪ͨ
	/**
	 * @brief					�г�����״̬��֪ͨ�¼�
	 * @param[in]				cMkID			�г�ID
	 * @param[in]				nStatus			�ɷ���״̬
	 */
	void						OnMarketAvailableNotify( unsigned char cMkID, int nStatus );

	/**
	 * @brief					ʵʱ��������߸��»ص�
	 */
	void						OnSHL1Index( unsigned int nMkTime, tagCcComm_IndexData& refData );
	void						OnSHL1Snap( unsigned int nMkTime, tagCcComm_StockData5& refData );
	void						OnSZL1Index( unsigned int nMkTime, tagCcComm_IndexData& refData );
	void						OnSZL1Snap( unsigned int nMkTime, tagCcComm_StockData5& refData );

protected:
	/**
	 * @brief					�����Ϻ�Lv1�ķ���������
	 * @return					���ؼ��ص���Ʒ��
	 */
	int							LoadMinuteLine4SHL1();

	/**
	 * @brief					��������Lv1�ķ���������
	 * @return					���ؼ��ص���Ʒ��
	 */
	int							LoadMinuteLine4SZL1();

	/**
	 * @brief					�����Ϻ�Lv1�г�״̬�Ƿ�Ϊ���ã���ʶ������Ʒ������
	 * @param[in]				nWareCount		������Ʒ����
	 */
	void						SetWareCount4SHL1( int nWareCount );

	/**
	 * @brief					��������Lv1�г�״̬�Ƿ�Ϊ���ã���ʶ������Ʒ������
	 * @param[in]				nWareCount		������Ʒ����
	 */
	void						SetWareCount4SZL1( int nWareCount );

protected:
	/**
	 * @brief					��ѯ���������ɷ�
	 */
	static void* STDCALL		QueryThreadFunc( void* pParam );

private:
	MThread						m_oQueryThread;					///< ��ѯ��Ӧ�߳�
	MThread						m_oCallBackThread;				///< ��ѯ��Ӧ�߳�
	QuoteQuerySpi*				m_pQuoteQuerySpi;				///< ��ѯ��Ӧ�ص�
	bool						m_bIsQuerying;					///< �����ѯ��
	enum XDFMarket				m_eReqMkID;						///< ������г�
	unsigned int				m_nRequestID;					///< ������
	unsigned int				m_nBeginTime;					///< Ҫ��ѯ�����ߵĿ�ʼʱ��
	unsigned int				m_nEndTime;						///< Ҫ��ѯ�����ߵĽ���ʱ��

private:
	int							m_nWareCount4SHL1;				///< �Ϻ�Lv1��ʼ���ɹ���ʶ
	unsigned int				m_nMkTime4SHL1;					///< �Ϻ�Lv1�г�ʱ��
	unsigned int				m_nMkDate4SHL1;					///< �Ϻ�Lv1�г�����
	int							m_nWareCount4SZL1;				///< ����Lv1��ʼ���ɹ���ʶ
	unsigned int				m_nMkTime4SZL1;					///< ����Lv1�г�ʱ��
	unsigned int				m_nMkDate4SZL1;					///< �Ϻ�Lv1�г�����
	SecurityMinCache			m_obj1MinCache4SHL1;			///< ��֤L1�����߻���
	SecurityMinCache			m_obj1MinCache4SZL1;			///< ��֤L1�����߻���
};


/**
 * @class		MPrimeClient
 * @brief		���ݲ�ѯ��
 */
class MPrimeClient : public QuotePrimeApi
{
public:
	MPrimeClient();
	virtual ~MPrimeClient();

public:
	/**
	 * @brief			�����ѯ����
	 * @param[in]		FuncNo			100:ͬ����ʽ��ѯĳ�г���ϢXDFAPI_MarketStatusInfo
										101:�첽��ʽ��ѯĳ�г���ϢXDFAPI_MarketStatusInfo
										102:��ȡ�������ͣ�Ʊ��
										103:��ȡ�н��г���� (pParam, Ϊ�գ���return����е���Ʒ����)
	 */
	int		STDCALL		ReqFuncData( int FuncNo, void* wParam, void* lParam );

};






#endif












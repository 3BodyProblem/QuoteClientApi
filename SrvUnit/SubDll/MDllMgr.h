
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


//ĳ���г���״̬
typedef struct  
{
	int		nMarketID;
	int		nInnerStat;	//����״̬(�����ɹ�/ʧ��) 0=ʧ��  1= �ɹ�
	int		nOuterStat; //����״̬(������/����) //	0 =������   XRS_Unknow=δ֪  XRS_Init=δ����  5=����
}tagOneMarketStat;


/**
 * @class		���г�����Dll״̬������
 * @class		M4XDllStat
 */
class M4XDllStat
{
public:
	M4XDllStat();
	virtual ~M4XDllStat();
	
public:
	/**
	 * @brief		����һ���г�(����inner_StartOneMarket())
	 */
	int				BeginWork();

	/**
	 * @brief		ֹͣ�����г��Ĵ�����񣬲������Դ
	 */
	void			EndWork();

	/**
	 * @brief		ȡ�������ķ���״̬
	 */
	int				GetMarketStat(int nMarketID);

	/**
	 * @brief		�����Լ����г�״̬����֪ͨ�ص����ֱ仯
	 */
	void			CheckMarket();

	/**
	 * @brief		�����г�ID���ض�Ӧ���г�����
	 */
	Base4XDll*		GetMarketDll(int nMarketId);

protected:
	/**
	 * @brief		���ݲ��������ϼ���Dll & ����һ���г�
					&
					��¼һ���г��Ļ״̬(tagOneMarketStat)
	 * @param[in]	nMarketID			�г�ID
	 * @param[in]	UnitNo				����Ԫ��
	 */
	int				inner_StartOneMarket(int nMarketID, const char* pszModulePath, const char* pszDllPath, int UnitNo);

	/**
	 * @brief		ָֹͣ�����г���������
	 * @param[in]	nMarketID			�г�ID
	 */
	void			inner_StopOneMarket(int nMarketID);

	/**
	 * @brief		���һ���µ��г�״̬��ά���б�
	 * @param[in]	nMarketID			�г�ID
	 * @param[in]	nInnerStat			����������״̬
	 * @param[in]	nOuterStat			�����ķ���״̬
	 */
	void			inner_AddMarket(int nMarketID, int nInnerStat, int nOuterStat);

	/**
	 * @brief		������г���Դ״̬
	 */
	void			inner_FreeMarket();

	/**
	 * @brief		ȡ����������״̬(XDFRunStat)
	 */
	int				inner_CheckOneMarket(Base4XDll* pObj);

protected:
	vector<tagOneMarketStat>	m_arrStat;			///< �����г���״̬ά���б�		
	MCriticalSection			m_Section;

	Base4XDll					m_oShSe;			///< �Ϻ��г���������DLL���÷�װ
	Base4XDll					m_oSzSe;			///< �����г���������DLL���÷�װ
	Base4XDll					m_oShOpt;			///< �Ϻ���Ȩ�г���������DLL���÷�װ
	Base4XDll					m_oZjqh;			///< �н��ڻ��г���������DLL���÷�װ
	Base4XDll					m_oZjOpt;			///< �н���Ȩ�г���������DLL���÷�װ
	Base4XDll					m_oCnf;				///< �г���������DLL���÷�װ
	Base4XDll					m_oSzOpt;			///< ������Ȩ�г���������DLL���÷�װ
	Base4XDll					m_oCnfOpt;			///< �г���������DLL���÷�װ
};


/**
 * @brief		���ݹ���ȫ�ֵ���, ��������Ϊ�г�ID
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
	 * @brief		��ʼ������Դ
	 */
	int				Instance();

	/**
	 * @brief		�ͷ���Դ
	 */
	void			Release();

	/**
	 * @brief		ͨ��״̬�������M4XDllStat���������ظ��г�ģ��
	 * @return		> 0				�ɹ�
					< 0				ʧ��
	 */
	int				BeginWork();

	/**
	 * @brief		ֹͣ�����г��Ĵ�����񣬲������Դ
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
	 * @brief		�����г�idȡ��Ӧ�����ڡ�ʱ�䡢�г�״̬
	 */
	int				GetSimpleMarketInfo(uint8_t cMarket, XDFAPI_MarketStatusInfo* pInfo);

	/**
	 * @brief		�����г�idȡ��Ӧ�����ڡ�ʱ�䡢�г�״̬(��������ѯ)
	 */
	int				GetQuicksSimpleMarketInfo(uint8_t cMarket, XDFAPI_MarketStatusInfo* pInfo);

public:
	/**
	 * @brief		��������Э���ǲ��ǺϷ�������Э��
	 */
	bool			outer_ExpectData(unsigned char MarketId,unsigned char MainType, unsigned char ChildType);

	/**
	 * @brief		���ͻص�
	 */
	void			outer_OnPushData(unsigned char MarketId,unsigned char ChildType, const char* pszbuf, unsigned short bufsize);

	void			outer_CloseSub(unsigned char MarketId);

public:
	MData4XDll*		GetMarketDll(int nMarketId);
	
protected:
	static void* STDCALL	TaskThreadFunc(void *pParam);	//����״̬����־
	
protected:
	
	MShOptDll					m_oShOptDll;		///< �Ϻ���Ȩ
	MZjOptDll					m_oZjOptDll;		///< �н���Ȩ
	MZjqhDll					m_oZjqhDll;			///< �н��ڻ�
	MShSEDll					m_oShSEDll;			///< �Ϻ�֤ȯ�г�
	MSzSEDll					m_oSzSEDll;			///< ����֤ȯ�г�
	MCnfDll						m_oCnfDll;			///< ��Ʒ�ڻ�(�ϰ汾��)
	MSzOptDll					m_oSzOptDll;		///< ������Ȩ
	MCnfOptDll					m_oCnfOptDll;		///< ��Ʒ��Ȩ(�°汾��)

	MThread						m_threadClient;		///< ״̬����߳�
	M4XDllStat					m_oWorkStat;		///< ����ƽ̨(DLL)����ģ��
	bool						m_bWorking;

	MCounter					m_oCounter;
private:
	
	
};


#endif



























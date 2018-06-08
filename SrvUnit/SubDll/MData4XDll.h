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
	void			Detach();	//����MsgHead,Last�����
	//������(1)����ninbytes, (2)����ushort��MsgLen�������͵ȴ���,
	//�������ͣ��Ƿ�Ҫ����һ�������У��������65535�ı�����Χ��Ҫ�����һ��,LastType=0xFFFF?


	void			PutSingleMsg(unsigned int ntype, char* pStruct, int nStructSize);	//������Ϣ�����ɺϲ�
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
 * @brief		���г��ص�������ӿ���
 */
class MData4XDll
{
public:
	MData4XDll();
	virtual ~MData4XDll();

public:
	/**
	 * @brief		�󶨵�һ����Ӧ���г�����Ĺ������
	 */
	void			Bind4XDll(Base4XDll* pDll);

	/**
	 * @brief		����״̬
	 */
	void			Reset();

	/**
	 *
	 * @brief		��ȡģ��״̬��Ϣ
	 * @param[out]	pInfo				״̬����
	 */
	virtual void	GetDllStatus( tagDll_DriverStatusInfo* pInfo );

public:
	/**
	 * @brief		�г�����Э����Ч�Լ��
	 * @return		true		��ЧЭ��
					false		��ЧЭ��
	 */
	virtual bool	outer_ExpectData(unsigned char MainType, unsigned char ChildType, unsigned char MarketID ) = 0;

	/**
	 * @brief		�г���������ص�
	 */
	virtual void	outer_OnPushData(unsigned char ChildType, const char* pszbuf, unsigned short bufsize) = 0;

	/**
	 * @brief		ȡ��ͳ����Ϣ
	 */
	virtual void	outer_GetStatus(uint32_t& ulTime, int64_t * pI64Send, int64_t * pI64Recv);

	/**
	 * @brief		ȡ��ȫ���Ŀ�����Ϣ
	 */
	virtual int		outer_GetLastMarketDataAll(char* pszInBuf, int32_t nInBytes);

	/**
	 * @brief		ȡ��ȫ���������Ϣ
	 */
	virtual int		outer_GetCodeTable(char* pszInBuf, int32_t nInBytes, int32_t& nCount);

	/**
	 * @brief		ȡ��ȫ�����г���Ϣ
	 */
	virtual int		outer_GetMarketInfo(char* pszInBuf, int32_t nInBytes);

	/**
	 * @brief		ȡ���г����ں�ʱ��
	 */
	virtual int		outer_GetSimpleMarketInfo(void* pInfo);

	/**
	 * @brief		ȡ���г����ں�ʱ��(��������ѯ)
	 */
	virtual int		outer_GetQuickSimpleMarketInfo(void* pInfo);

protected:
	/**
	 * @brief		�ӿڣ�ȡ�ø���Ӧ�г��Ŀ��մ�С��Ϣ
	 */
	virtual void	inner_GetStockInfo(unsigned int& MsgType, int& sizeComStockData, int& sizeXDFStockData) = 0;

	/**
	 * @brief		�ӿڣ�ȡ�ø���Ӧ�г�������С��Ϣ
	 */
	virtual void	inner_GetNameTableInfo(unsigned int& MsgType, int& sizeComNameTable, int& sizeXDFNameTable) = 0;

	/**
	 * @brief		�ӿڣ�ȡ�ø���Ӧ�г�������ڴ��
	 */
	virtual int		inner_GetNameTable(unsigned short dwStartNo, void * pData, int nCount) = 0;

	/**
	 * @brief		�ӿڣ�ȡ�ø���Ӧ�г��Ŀ����ڴ��
	 */
	virtual int		inner_GetStock(unsigned short dwStartNo, void * pData, int nCount) = 0;

	/**
	 * @brief		�ӿڣ�ת����������ṹ��qwin�����ṹ
	 */
	virtual int		inner_TransNameTable( void * pNameTb, char* poutbuf) = 0;

	/**
	 * @brief		�ӿڣ�ת������Ŀ��սṹ��qwin�Ŀ��սṹ
	 */
	virtual int		inner_TransStcok(void* pData, char* poutbuf) = 0; 

	/**
	 * @brief		�ӿڣ��Ƚ����������Ƿ���ͬһ����Ʒ��
	 */
	virtual bool	inner_CompareStock(void* pData, char* pCode) =0;

protected:
	Base4XDll*		m_p4XDll;
	unsigned int	m_uTime;
	__int64			m_i64Send;
	__int64			m_i64Recv;
};


#endif







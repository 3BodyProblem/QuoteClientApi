
#ifndef _MDATAIO_DEFINE_H
#define _MDATAIO_DEFINE_H

#include "MEngine.hpp"


/**
 * @brief			�������͹���
 */
class MDataIO
{
public:
	MDataIO();
	virtual ~MDataIO();

public:
	int					Instance();
	void				Release();

public:
	int					PutData(XDFAPI_PkgHead* pHead,const char* pszInBuf, int	nInBytes);

protected:
	static void* STDCALL	DataThreadFunc(void *pParam);	//���������ɷ�
	void				inner_CheckData();

protected:
	char*					m_pPkgBuf;
	MWaitEvent				m_oWEvent;
	MThread					m_threadClient;
	MLoopBufferMt<char>		m_PushBuffer;
	MCriticalSection		m_oSection;

	MCounter				m_oCounter;

private:

};









#endif
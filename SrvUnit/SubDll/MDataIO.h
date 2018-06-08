
#ifndef _MDATAIO_DEFINE_H
#define _MDATAIO_DEFINE_H

#include "MEngine.hpp"


/**
 * @brief			数据推送管理
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
	static void* STDCALL	DataThreadFunc(void *pParam);	//处理数据派发
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

#ifndef _MLOGUNIT_DEFINE_H
#define _MLOGUNIT_DEFINE_H

#include "MEngine.hpp"



class MLogUnit
{
public:
	MLogUnit();
	virtual ~MLogUnit();

public:
	int			Instance();
	void		Release();

	void		WriteLog(int32_t type, int32_t level,const char* pszsrc, const char * pszmsg);
	void		WriteLogEx(int32_t type, int32_t level,const char* pszsrc, const char * format, ...);

public:
	void		outer_CheckLog();

protected:


protected:
	MLoopBufferMt<char>		m_LogBuffer;
	MCriticalSection		m_Section;

private:
};








#endif
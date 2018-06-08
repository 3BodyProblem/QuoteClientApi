
#include "StdAfx.h"
#include "MLogUnit.h"
#include "../GlobalIO/GlobalIO.h"

#define MAX_LOGBUF_COUNT	3*1024*1024 //×Ü»º³å´óÐ¡

typedef struct  
{
	int		LogType;
	int		LogLevel;
	char	Log[255];
}tagLogInfo;


MLogUnit::MLogUnit()
{

}
MLogUnit::~MLogUnit()
{

}

int			MLogUnit::Instance()
{
	int  iret = m_LogBuffer.Instance(MAX_LOGBUF_COUNT);
	if (iret <0)
	{
		return -1;
	}

	return 1;
}

void		MLogUnit::Release()
{
	m_LogBuffer.Release();
}

void		MLogUnit::WriteLog(int32_t type, int32_t level,const char* pszsrc, const char * pszmsg)
{
	MLocalSection lock(&m_Section);

	tagLogInfo oInfo;
	memset(&oInfo, 0, sizeof(oInfo));
	oInfo.LogType = type;
	oInfo.LogLevel = level;
	_snprintf(oInfo.Log, 254, "<%s>%s", pszsrc, pszmsg);
	
	m_LogBuffer.PutData((char*)&oInfo, sizeof(oInfo));

}

void		MLogUnit::WriteLogEx(int32_t type, int32_t level,const char* pszsrc, const char * format, ...)
{
	char							tempbuf[255];
	va_list							valist;
	
	va_start(valist,format);
	_vsnprintf(tempbuf,254,format,valist);
	va_end(valist); 
	
	WriteLog(type, level, pszsrc, tempbuf);
}

void		MLogUnit::outer_CheckLog()
{
	int nsize =0;
	int nonelen = sizeof(tagLogInfo);
	tagLogInfo  oInfo;
	do 
	{
		nsize = m_LogBuffer.GetRecordCount();
		if (nsize < nonelen )
		{
			return;
		}

		m_LogBuffer.GetData((char*)&oInfo, nonelen );
		if (Global_pSpi)
		{
			Global_pSpi->XDF_OnRspOutLog(oInfo.LogType, oInfo.LogLevel, oInfo.Log);
		}
	} while (1);

}





#include "StdAfx.h"
#include "Base4XDll.h"
#include "../GlobalIO/GlobalIO.h"


Base4XDll::Base4XDll()
{
	m_pControlIO =0;
	memset(&m_tagDllFunc, 0, sizeof(m_tagDllFunc));
	m_nMarketID =0;

}

Base4XDll::~Base4XDll()
{
	
}

int		Base4XDll::Instance(const char* pszDll, unsigned int nUnitCode, int nMarketID)
{
	int							nRetVal;
	tagDll_DataCenterInput		tagDcInput;
	
	Release();

	m_nMarketID = nMarketID;
	
	MString str= pszDll;
	nRetVal = m_driverdll.LoadDll( str );
	if( nRetVal < 0 )
	{
		return nRetVal;
	}
	
	m_tagDllFunc.GetDllVersion = (tagGetDllVersion *)m_driverdll.GetDllFunction( "GetDllVersion" );
	m_tagDllFunc.GetBaseInfo = (tagGetDriverBaseInfo *)m_driverdll.GetDllFunction( "GetDriverBaseInfo" );
	m_tagDllFunc.BeginWork = (tagBeginDriverWork *)m_driverdll.GetDllFunction( "BeginDriverWork" );
	m_tagDllFunc.EndWork = (tagEndDriverWork *)m_driverdll.GetDllFunction( "EndDriverWork" );
	m_tagDllFunc.GetMarketInfo = (tagGetMarketInfo *)m_driverdll.GetDllFunction( "GetMarketInfo" );
	m_tagDllFunc.GetFrameCount = (tagGetDataFrameCount *)m_driverdll.GetDllFunction( "GetDataFrameCount" );
	m_tagDllFunc.GetFrameData = (tagGetOneDataFrame *)m_driverdll.GetDllFunction( "GetOneDataFrame" );
	m_tagDllFunc.IsWorking = (tagIsDriverWorking *)m_driverdll.GetDllFunction( "IsDriverWorking" );
	m_tagDllFunc.GetStatusInfo = (tagGetDriverStatusInfo *)m_driverdll.GetDllFunction( "GetDriverStatusInfo" );
	
	if( m_tagDllFunc.GetDllVersion == NULL || m_tagDllFunc.GetBaseInfo == NULL || 
		m_tagDllFunc.BeginWork == NULL ||m_tagDllFunc.EndWork == NULL || 
		m_tagDllFunc.GetMarketInfo == NULL || 
		m_tagDllFunc.GetFrameCount == NULL || m_tagDllFunc.GetFrameData == NULL ||
		m_tagDllFunc.IsWorking == NULL || m_tagDllFunc.GetStatusInfo == NULL )
	{
		Release();
		
		return -2;
	}

	memset(&tagDcInput, 0, sizeof(tagDll_DataCenterInput));
	inner_GetDataCenterFunc( tagDcInput );
	tagDcInput.DriverUnitCode = nUnitCode;
	
	tagDll_DriverOutput tagDcOutput;
	memset(&tagDcOutput, 0, sizeof(tagDll_DriverOutput));
	nRetVal = m_tagDllFunc.BeginWork( &tagDcInput, &tagDcOutput );
	if( nRetVal < 0 )
	{
		Release();
		return nRetVal;
	}

	tagDll_DriverBaseInfo BaseInfo;
	memset(&BaseInfo, 0, sizeof(tagDll_DriverBaseInfo));
	m_tagDllFunc.GetBaseInfo(&BaseInfo);
	m_pControlIO = BaseInfo.drvControlClassPtr;

	return 1;
}

void	Base4XDll::Release()
{
	m_pControlIO=NULL;
	
	if( m_tagDllFunc.EndWork )
	{
		m_tagDllFunc.EndWork();
	}
	memset( &m_tagDllFunc, 0, sizeof(m_tagDllFunc) );
	
	m_driverdll.CloseDll();

}

bool	Base4XDll::IsDllWorking()
{
	if (m_tagDllFunc.IsWorking)
	{
		return m_tagDllFunc.IsWorking();
	}
	return false;
}

int		Base4XDll::GetMarketInfo( tagCcComm_MarketInfoHead * pHead, tagCcComm_MarketInfoDetail * pDetail, int nCount )
{
	int time = 0;

	if( NULL == m_tagDllFunc.IsWorking )
	{
		return -100;
	}

	while (!m_tagDllFunc.IsWorking())	//加入这种机制的原因？？？？
	{ 
		if(time >4)
			break;
		time++;
		Sleep(500);
	}

	int						nRetVal = 0;
	tagDll_GetMarketInfoIn	tagIn = {0};
	tagDll_GetMarketInfoOut tagOut = {0};
	
	tagIn.nCount = nCount;
	tagOut.pHead = pHead;
	tagOut.pInfo = pDetail;
	
	if( m_tagDllFunc.GetMarketInfo )
	{
		nRetVal = m_tagDllFunc.GetMarketInfo( &tagIn, &tagOut );
	}
	
	return nRetVal;
}

int		Base4XDll::GetQuickMarketInfo( tagCcComm_MarketInfoHead * pHead, tagCcComm_MarketInfoDetail * pDetail, int nCount )
{
	int time = 0;

	if( NULL == m_tagDllFunc.IsWorking )
	{
		return -100;
	}
/*
	while (!m_tagDllFunc.IsWorking())	//加入这种机制的原因？？？？
	{ 
		if(time >4)
			break;
		time++;
		Sleep(500);
	}
*/
	int						nRetVal = 0;
	tagDll_GetMarketInfoIn	tagIn = {0};
	tagDll_GetMarketInfoOut tagOut = {0};
	
	tagIn.nCount = nCount;
	tagOut.pHead = pHead;
	tagOut.pInfo = pDetail;
	
	if( m_tagDllFunc.GetMarketInfo )
	{
		nRetVal = m_tagDllFunc.GetMarketInfo( &tagIn, &tagOut );
	}
	
	return nRetVal;
}

void		Base4XDll::ShowDllHelp(unsigned long ulCmdSerial)
{
	if (NULL != m_pControlIO)
	{
		char * arr[1]={"help"};
		m_pControlIO->ControlCommand(ulCmdSerial,arr, 1);
	}
}

void		Base4XDll::ExecDllCmd(unsigned long ulCmdSerial, char * argv[], int nArgc)
{
	if (NULL != m_pControlIO)
	{
		m_pControlIO->ControlCommand( ulCmdSerial, argv, nArgc );
	}
}

unsigned long	Base4XDll::GetDllVersion()
{
	if (m_tagDllFunc.GetDllVersion)
	{
		return m_tagDllFunc.GetDllVersion();
	}
	return 0;
}

void		Base4XDll::GetDllStatus(tagDll_DriverStatusInfo * pInfo)
{
	if (m_tagDllFunc.GetStatusInfo)
	{
		m_tagDllFunc.GetStatusInfo(pInfo);
	}
}

//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
void __cdecl Base4XDll::static_OnPushDataFun(unsigned char MainType,unsigned char ChildType,const char * InBuf,unsigned short InSize,unsigned char Marketid,unsigned short UnitNo,bool SendDirectFlag)
{
	if (!Global_DllMgr.outer_ExpectData(Marketid, MainType, ChildType))
	{
		return;
	}

	if (SendDirectFlag)
	{
		char output[16*1024]={0};
		char *pszCurrentPosPt =NULL;
		//解压一下
		int rv = Global_MVPlatIO.RestoreDataFrame(InBuf, InSize, &pszCurrentPosPt, output, 16*1024);
		if (rv <=0)
		{
			printf("Base4XDll::static_OnPushDataFun ============> RestoreDataFrame\n");
			return;
		}
		Global_DllMgr.outer_OnPushData(Marketid, ChildType, pszCurrentPosPt+sizeof(tagComm_FrameHead), rv- sizeof(tagComm_FrameHead) );	
	}
	else
	{
		Global_DllMgr.outer_OnPushData(Marketid, ChildType, InBuf, InSize);
	}
}

void __cdecl Base4XDll::static_WriteLogFun(unsigned char Type,unsigned short SrvUnit,unsigned short LogLevel,const char * Source,const char	* Format,...)
{
	char							tempbuf[8192];
	va_list							valist;
	
	va_start(valist,Format);
	_vsnprintf(tempbuf,8192,Format,valist);
	va_end(valist); 
	
	char szsrc[80]={0};
	_snprintf(szsrc,79,"<%s>", Source);
	Global_WriteLog(Type, 1, szsrc, tempbuf);
}

void __cdecl Base4XDll::static_PrintLnFun(unsigned long CmdSerial,const char * Format,...)
{
	char							tempbuf[8192];
	va_list							valist;
	
	va_start(valist,Format);
	_vsnprintf(tempbuf,8192,Format,valist);
	va_end(valist); 
	
	char szsrc[40]={0};
	_snprintf(szsrc,39,"PrintLnFun[%ld]", CmdSerial);
	Global_WriteLog(0, 1, szsrc, tempbuf);
}

void __cdecl Base4XDll::static_PrintFun(unsigned long CmdSerial,const char * Format,...)
{
	char							tempbuf[8192];
	va_list							valist;
	
	va_start(valist,Format);
	_vsnprintf(tempbuf,8192,Format,valist);
	va_end(valist); 
	
	char szsrc[40]={0};
	_snprintf(szsrc,39,"PrintFun[%ld]", CmdSerial);
	Global_WriteLog(0, 1, szsrc, tempbuf);

}

void __cdecl Base4XDll::static_CloseFun(unsigned char MarketId,unsigned short UnitCode)
{
	char tempbuf[500]={0};
	_snprintf(tempbuf,499, "收到市场[%d]传输的断开连接的通知", MarketId);
	Global_WriteLog(2, 3,"CloseFun", tempbuf);
}

//............................................................................................................................
int  __cdecl Base4XDll::static_PrepareDataFrame(unsigned char cMainType,unsigned char cChildType,const char * lpInBuf,unsigned short sInSize,char * lpOutBuf,unsigned short sOutSize)
{
	return(Global_MVPlatIO.PrepareDataFrame(cMainType,cChildType,lpInBuf,sInSize,lpOutBuf,sOutSize));
}

//............................................................................................................................
int  __cdecl Base4XDll::static_RestoreDataFrame(const char * lpInBuf,unsigned short sInSize,char * * lpOutPtr,char * lpTempBuf,unsigned short sTempSize )
{
	return(Global_MVPlatIO.RestoreDataFrame(lpInBuf,sInSize,lpOutPtr,lpTempBuf,sTempSize));
}

//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................
//..............................................................................................................................................................

void Base4XDll::inner_GetDataCenterFunc(tagDll_DataCenterInput & datacenterinput)
{
	datacenterinput.PlatUnitType = 0x2000;	//伪装自己是传输平台
	datacenterinput.PlatVersion = ((unsigned long)601 << 16) + 51;
	datacenterinput.DriverUnitCode = 0; 
	datacenterinput.SendBufSize = 2621440;
	datacenterinput.FrameSize = 8192;
	datacenterinput.PushDataFunPtr = static_OnPushDataFun;
	datacenterinput.WriteLogFunPtr = static_WriteLogFun;
	datacenterinput.CmdPrintFunPtr = static_PrintFun;
	datacenterinput.CmdPrintlnFunPtr = static_PrintLnFun;
	datacenterinput.CloseFunptr = static_CloseFun;
	datacenterinput.PrepareDataFrame = static_PrepareDataFrame;
	datacenterinput.RestoreDataFrame = static_RestoreDataFrame;

	memcpy(&datacenterinput.ulSvcPlatFlag, "TRSP", 4);
	datacenterinput.ulPlatVer = ((unsigned long)601 << 16) + 51;
	
}

int		Base4XDll::GetData( unsigned char MainType, unsigned char AssiType, unsigned short dwStartNo, void * pOutData, int nSize, int nRecordSize )
{
	tagCcComm_MarketSubApply		apply = {0};
	tagDll_GetDataFrameCountIn		paramcountin = {0};
	tagDll_GetFrameCountOut			paramcountout = {0};
	tagDll_GetOneDataFrameIn		paramdatain = {0};
	tagDll_GetOneDataFrameOut		paramdataout = {0};
	int								nFrameCount;
	int								nGetCount = 0;
	int								nTemp;
	int								nDataSize;
	char							tempbuf[8192];
	
	if( m_tagDllFunc.GetFrameCount == NULL || m_tagDllFunc.GetFrameData == NULL || nRecordSize <= 0 )
	{
		return -1;
	}
	
	apply.MarketID = m_nMarketID;
	nGetCount = 0;
	//	apply.ApplyOffset = dwStartNo;
	
	paramdatain.MainType = paramcountin.MainType = MainType;
	paramdatain.ChildType = paramcountin.ChildType = AssiType;
	paramdatain.ApplyBuf = paramcountin.ApplyBuf = (char *)&apply;
	paramdatain.ApplySize = paramcountin.ApplySize = sizeof(apply);
	
	do
	{
		apply.ApplyOffset = dwStartNo + nGetCount;
		
		paramdatain.DataOutBufSize = sizeof(tempbuf);
		paramdataout.DataOutBuf = tempbuf;
		
		nFrameCount = m_tagDllFunc.GetFrameCount( &paramcountin, &paramcountout );
		if( nFrameCount < 0 )
		{
			return -3;
		}
		
		for( int i = 0; i < nFrameCount; ++i )
		{
			paramdatain.FrameNo = i;
			nDataSize = m_tagDllFunc.GetFrameData( &paramdatain, &paramdataout );
			if( nDataSize <= sizeof(tagCcComm_MarketSubResponse) )
			{
				paramcountout.RequestCompleteFlag = 1;
				break;
			}
			nDataSize -= sizeof(tagCcComm_MarketSubResponse);
			assert( nDataSize % nRecordSize == 0 );
			nTemp = nGetCount*nRecordSize;
			if( nTemp + nDataSize <= nSize )
			{
				memcpy( (char *)pOutData+nTemp, paramdataout.DataOutBuf+sizeof(tagCcComm_MarketSubResponse), nDataSize ); 
				nGetCount += (nDataSize/nRecordSize);
			}
			else
			{
				paramcountout.RequestCompleteFlag = 1;
				break;
			}
		}

	}while( paramcountout.RequestCompleteFlag != 1 );
	
	return nGetCount;
}








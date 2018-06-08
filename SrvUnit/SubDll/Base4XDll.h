
#ifndef _BASE4XDLL_DEFINE_H
#define _BASE4XDLL_DEFINE_H

#include "../MPublic/MInterface.hpp"
#include <fstream>


typedef struct
{
	tagGetDllVersion		*			GetDllVersion;
	tagGetDriverBaseInfo	*			GetBaseInfo;
	tagBeginDriverWork		*			BeginWork;
	tagEndDriverWork		*			EndWork;
	tagGetMarketInfo		*			GetMarketInfo;
	tagGetDataFrameCount	*			GetFrameCount;
	tagGetOneDataFrame		*			GetFrameData;
	tagIsDriverWorking		*			IsWorking;
	tagGetDriverStatusInfo	*			GetStatusInfo;
} tag4XDll_Func;


/**
 * @class		Base4XDll
 * @brief		各市场传输Dll直接管理类
 */
class Base4XDll
{
public:
	Base4XDll();
	virtual ~Base4XDll();

public:
	/**
	 * @brief				加载市场传输驱动，并启动它
	 */
	int						Instance(const char* pszDll,unsigned int nUnitCode, int nMarketID);

	/**
	 * @brief				停止市场传输驱动，并卸载它
	 */
	void					Release();

public://公共函数
	bool					IsDllWorking();
	int						GetMarketInfo( tagCcComm_MarketInfoHead * pHead, tagCcComm_MarketInfoDetail * pDetail, int nCount );
	int						GetQuickMarketInfo( tagCcComm_MarketInfoHead * pHead, tagCcComm_MarketInfoDetail * pDetail, int nCount );
	int						GetData( unsigned char MainType, unsigned char AssiType, unsigned short dwStartNo, void * pOutData, int nSize, int nRecordSize );

public:
	void					ShowDllHelp(unsigned long ulCmdSerial);
	void					ExecDllCmd(unsigned long ulCmdSerial, char * argv[], int nArgc);
	unsigned long			GetDllVersion();
	void					GetDllStatus(tagDll_DriverStatusInfo * pInfo);

protected:
	void					inner_GetDataCenterFunc(tagDll_DataCenterInput & datacenterinput);

public:
	static void __cdecl		static_OnPushDataFun(unsigned char MainType,unsigned char ChildType,const char * InBuf,unsigned short InSize,unsigned char Marketid,unsigned short UnitNo,bool SendDirectFlag);
	static void __cdecl		static_WriteLogFun(unsigned char Type,unsigned short SrvUnit,unsigned short LogLevel,const char * Source,const char	* Format,...);
	static void __cdecl		static_PrintFun(unsigned long CmdSerial,const char * Format,...);
	static void __cdecl		static_PrintLnFun(unsigned long CmdSerial,const char * Format,...);
	static void __cdecl		static_CloseFun(unsigned char MarketId,unsigned short UnitCode);
	static int  __cdecl		static_PrepareDataFrame(unsigned char cMainType,unsigned char cChildType,const char * lpInBuf,unsigned short sInSize,char * lpOutBuf,unsigned short sOutSize);
	static int  __cdecl		static_RestoreDataFrame(const char * lpInBuf,unsigned short sInSize,char * * lpOutPtr,char * lpTempBuf,unsigned short sTempSize );

public:
	int								m_nMarketID;			///< 市场ID
	
protected:
	tag4XDll_Func					m_tagDllFunc;			///< 市场传输驱动dll导出函数
	MDll							m_driverdll;	
	MControlBaseClass	*			m_pControlIO;

};



#endif
















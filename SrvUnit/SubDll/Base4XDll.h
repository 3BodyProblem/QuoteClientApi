
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
 * @brief		���г�����Dllֱ�ӹ�����
 */
class Base4XDll
{
public:
	Base4XDll();
	virtual ~Base4XDll();

public:
	/**
	 * @brief				�����г�������������������
	 */
	int						Instance(const char* pszDll,unsigned int nUnitCode, int nMarketID);

	/**
	 * @brief				ֹͣ�г�������������ж����
	 */
	void					Release();

public://��������
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
	int								m_nMarketID;			///< �г�ID
	
protected:
	tag4XDll_Func					m_tagDllFunc;			///< �г���������dll��������
	MDll							m_driverdll;	
	MControlBaseClass	*			m_pControlIO;

};



#endif
















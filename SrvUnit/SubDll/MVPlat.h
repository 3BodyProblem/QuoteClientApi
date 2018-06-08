//--------------------------------------------------------------------------------------------------------------------------
//                  ;;;,
//        ,;;;;;: ,;;;:.,;..
//         :;;"`,;;;;;;"":;;					单元名称：SrvPlat2006通讯单元
//      ..;;:  .;;,.;;:',;;"					备注信息：
//    ,;;;;;;" :;`',;",;;"						
//   :" ,;:` , `:.;;;;;'
//    ,;;;,;:;;,;;;, ,;							
//   :;;;"` `:;;;;;,;,.;.
//   '""   .;;`.;";:;;;;` ;,					
//       .;"`  ::. '   .,;;;
//      ""     ';;;;;;;;;;`
//--------------------------------------------------------------------------------------------------------------------------
#ifndef __TRANSRVPLAT_MVPLATH__
#define __TRANSRVPLAT_MVPLATH__
//--------------------------------------------------------------------------------------------------------------------------
#include "MEngine.hpp"
//#include "../Interface/MInterface.hpp"
//--------------------------------------------------------------------------------------------------------------------------
//通讯请求返回通用错误定义
#define			COMMERR_NOREQUEST			-1		//没有对应的请求
#define			COMMERR_GETDATA				-2		//获取数据错误
#define			COMMERR_CANNOT				-3		//不能完成相应操作
#define			COMMERR_CERTIFY				-4		//认证失败
#define			COMMERR_INITIALIZE			-5		//认证失败
//..........................................................................................................................
//extern void Global_WriteLog(unsigned char type,unsigned short wSrvUnit,unsigned short logLevel,const char * source,const char * Format,...);
//extern unsigned short Global_DataCenterUnitCode;
//--------------------------------------------------------------------------------------------------------------------------
//..........................................................................................................................
//..........................................................................................................................
//..........................................................................................................................
//..........................................................................................................................
//--------------------------------------------------------------------------------------------------------------------------
class MVPlatIO 
{
public:
	MVPlatIO(void);
	virtual ~MVPlatIO();
public:
	int			Instance(void);
	void		Release(void);
public:
	virtual bool IsValidIPAddr(unsigned long lIPAddr);
	virtual int  SpProcFrame(unsigned char cMarket,unsigned long lLinkNo,unsigned char cMainType,unsigned char cChildType,unsigned short sRequestCode,unsigned char cFrameCount,unsigned char cFrameNo,short sErrorCode,const char * lpInBuf,unsigned short sInSize,char * lpOutBuf,unsigned short sOutSize);
public:
	//准备推送的数据包（包括对数据包金融金融压缩、标准压缩等处理），准备成功后，直接对每一个连接调用SendToBuf进行发送，就不用每次发送都对数据包进行压缩了。
	int			PrepareDataFrame(unsigned char cMainType,unsigned char cChildType,const char * lpInBuf,unsigned short sInSize,char * lpOutBuf,unsigned short sOutSize);
	int			RestoreDataFrame(const char * lpInBuf,unsigned short sInSize,char * * lpOutPtr,char * lpTempBuf,unsigned short sTempSize );
public:
	bool		IsEconomicalCompress(void);
protected:

};
//--------------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------------
////////////////////////This Code AUTO Mount Point By Append////////////

////////////////////////LINUX newline 请在该注释上面加CODE///////////////////

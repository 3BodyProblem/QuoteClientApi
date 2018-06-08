//--------------------------------------------------------------------------------------------------------------------------
//                  ;;;,
//        ,;;;;;: ,;;;:.,;..
//         :;;"`,;;;;;;"":;;					��Ԫ���ƣ�SrvPlat2006ͨѶ��Ԫ
//      ..;;:  .;;,.;;:',;;"					��ע��Ϣ��
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
//ͨѶ���󷵻�ͨ�ô�����
#define			COMMERR_NOREQUEST			-1		//û�ж�Ӧ������
#define			COMMERR_GETDATA				-2		//��ȡ���ݴ���
#define			COMMERR_CANNOT				-3		//���������Ӧ����
#define			COMMERR_CERTIFY				-4		//��֤ʧ��
#define			COMMERR_INITIALIZE			-5		//��֤ʧ��
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
	//׼�����͵����ݰ������������ݰ����ڽ���ѹ������׼ѹ���ȴ�����׼���ɹ���ֱ�Ӷ�ÿһ�����ӵ���SendToBuf���з��ͣ��Ͳ���ÿ�η��Ͷ������ݰ�����ѹ���ˡ�
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

////////////////////////LINUX newline ���ڸ�ע�������CODE///////////////////

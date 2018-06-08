
#include "StdAfx.h"
#include "MOptionIO.h"

extern HANDLE Global_Handle;

MOptionIO::MOptionIO()
{
	m_bInited =false;
	m_nCount =0;
	memset(m_szModulePath, 0, 255);
	memset(m_arrInfo, 0, sizeof(tagKeyFileInfo)*20 );
	
}

MOptionIO::~MOptionIO()
{

}

int		MOptionIO::Instance()
{
	if (m_bInited)
	{
		return 1;
	}
	Release();

	char sziniFile[255]={0};
	char szFile[255]={0};
	int i=0;
	//[1]
	{
		
		GetModuleFileName((HMODULE)Global_Handle, szFile, 254);
		
		int nlen = strlen(szFile);
		
		for (i = nlen-1; i>=0; i--)
		{
			if (szFile[i] == '.')
			{
				strncpy(sziniFile, szFile, i+1);
				strcat(sziniFile,"ini");
			}
			if (szFile[i] == '\\')
			{
				strncpy(m_szModulePath, szFile, i+1);
				break;
			}
		}
	}

	//[2]
	char szNames[8192]={0};
	int iret = GetPrivateProfileSectionNames(szNames, 8191, sziniFile);
	memset(szFile,0, sizeof(szFile));
	int idx=0;
	for (i=0; i<iret; i++)
	{
		if (szNames[i] == '\0')
		{
			inner_loadcontent(szFile, sziniFile);
			memset(szFile,0, sizeof(szFile));
			idx= 0;
		}
		else
		{
			szFile[idx++] = szNames[i];
		}
	}

	
	m_bInited = true;
	return 1;
}

void	MOptionIO::Release()
{
	m_bInited = false;
	memset(m_szModulePath, 0, sizeof(m_szModulePath));
	m_nCount =0;
	memset(m_arrInfo, 0, sizeof(tagKeyFileInfo)*20 );
	
}

int		MOptionIO::GetKeyFileCount()
{
	return m_nCount;
}

void	MOptionIO::GetKeyFileInfo(int idx, tagKeyFileInfo & oInfo)
{
	if (idx>=0 && idx < m_nCount)
	{
		memcpy(&oInfo, &m_arrInfo[idx], sizeof(tagKeyFileInfo) );
	}
	
}

void	MOptionIO::inner_loadcontent(const char* pszSection, const char* pszIniFile)
{
	if (0 == pszSection || 0 == pszIniFile)
	{
		return;
	}

	tagKeyFileInfo oInfo;
	memset(&oInfo, 0, sizeof(tagKeyFileInfo));

	GetPrivateProfileString(pszSection, "keyfile", "", oInfo.szDll, 254, pszIniFile);
	oInfo.nMarketID = GetPrivateProfileInt(pszSection,"marketid", 0, pszIniFile);
	oInfo.nSleep = GetPrivateProfileInt(pszSection,"sleepm", 200, pszIniFile);
	GetPrivateProfileString(pszSection,"marketchn","", oInfo.cMarketChn, 63, pszIniFile);
	if (oInfo.nMarketID < 0 || strlen(oInfo.szDll) <=0)
	{
		return;
	}


	{
		//[1]由dll 去寻找对应的ini
		char szAbsPath[255]={0};
		strcpy(szAbsPath, m_szModulePath);
		strcat(szAbsPath, oInfo.szDll);

		int i=0;
		int nlen = strlen(szAbsPath);
		for (i = nlen-1; i>=0; i--)
		{
			if (szAbsPath[i] == '.' )
			{
				szAbsPath[i] = '\0';
				break;
			}
		}
		strcat(szAbsPath,".ini");
		//[2]读取内容
		int srvcount = GetPrivateProfileInt("SRV","ServerListCount", 0, szAbsPath);
		if (srvcount>0)
		{
			char szIP[128]={0};
			GetPrivateProfileString("Communication","ServerIP_0","", szIP, 127, szAbsPath);
			int nPort = GetPrivateProfileInt("Communication","ServerPort_0", 0, szAbsPath);
			_snprintf(oInfo.cAddress,127, "%s:%d", szIP, nPort);
		}
	}
	
	if (m_nCount < 19)
	{
		memcpy(&m_arrInfo[m_nCount], &oInfo, sizeof(tagKeyFileInfo) );
		++m_nCount;
	}
	
}

void	MOptionIO::GetModulePath(char* pszPath)
{
	strcpy(pszPath, m_szModulePath);
}



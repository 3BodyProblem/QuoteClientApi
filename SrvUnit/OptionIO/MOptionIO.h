
#ifndef _MOPTIONIO_DEFINE_H
#define _MOPTIONIO_DEFINE_H

typedef struct
{
	char			szDll[255];
	int				nMarketID;
	int				nSleep;
	char			cMarketChn[64];
	char			cAddress[128];
}tagKeyFileInfo;


class MOptionIO
{
public:
	MOptionIO();
	virtual ~MOptionIO();

	int			Instance();
	void		Release();

	void		GetModulePath(char* pszPath);

	int			GetKeyFileCount();
	void		GetKeyFileInfo(int idx, tagKeyFileInfo & oInfo);

protected:
	void		inner_loadcontent(const char* pszSection, const char* pszIniFile);

protected:
	char			m_szModulePath[255];

	tagKeyFileInfo	m_arrInfo[20];
	int				m_nCount;
	bool			m_bInited;

	
private:

};




#endif
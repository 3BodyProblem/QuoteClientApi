
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

	/**
	 * @brief		行情数据同步工具所在目录
	 * @return		返回目录路径
	 */
	const char*		GetSyncExeFolder();

	/**
	 * @brief		行情同步数据所在目录
	 * @return		返回目录路径
	 */
	const char*		GetSyncDataFolder();

	/**
	 * @brief		获取实时数据同步延时（分）
	 * @return		延时时间值（分）
	 */
	unsigned int	GetSyncIntervalMin();

protected:
	void		inner_loadcontent(const char* pszSection, const char* pszIniFile);

protected:
	unsigned int	m_nSyncIntervalMin;			///< 同步下载延后时间（分）
	char			m_pszExeFolder[1024];		///< 下载工具所在目录
	char			m_pszDataFolder[1024];		///< 下载数据所在目录
	char			m_szModulePath[255];

	tagKeyFileInfo	m_arrInfo[20];
	int				m_nCount;
	bool			m_bInited;

	
private:

};




#endif
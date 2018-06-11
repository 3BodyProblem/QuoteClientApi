
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
	 * @brief		��������ͬ����������Ŀ¼
	 * @return		����Ŀ¼·��
	 */
	const char*		GetSyncExeFolder();

	/**
	 * @brief		����ͬ����������Ŀ¼
	 * @return		����Ŀ¼·��
	 */
	const char*		GetSyncDataFolder();

	/**
	 * @brief		��ȡʵʱ����ͬ����ʱ���֣�
	 * @return		��ʱʱ��ֵ���֣�
	 */
	unsigned int	GetSyncIntervalMin();

protected:
	void		inner_loadcontent(const char* pszSection, const char* pszIniFile);

protected:
	unsigned int	m_nSyncIntervalMin;			///< ͬ�������Ӻ�ʱ�䣨�֣�
	char			m_pszExeFolder[1024];		///< ���ع�������Ŀ¼
	char			m_pszDataFolder[1024];		///< ������������Ŀ¼
	char			m_szModulePath[255];

	tagKeyFileInfo	m_arrInfo[20];
	int				m_nCount;
	bool			m_bInited;

	
private:

};




#endif
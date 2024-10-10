#pragma once

#include <string>
#include <vector>

typedef std::vector<std::string> ResPathList;

class CDownloadResources
{
public:
	CDownloadResources(void);
	~CDownloadResources(void);
public:
	void Download();
	void AddPath(const char* szSrcPath, const char* szDestPath);
protected:
	bool findAndCopy(const char* szScrPath, const char* szDestPath);
	int countFileNum(const char* szDir);
protected:
	int m_iFileNum;
	int m_iCurIndex;
	ResPathList m_listResPathSrc;
	ResPathList m_listResPathDest;
};

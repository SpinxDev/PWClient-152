#include "StdAfx.h"
#include "DownloadResources.h"
#include "Global.h"
#include <io.h>

CDownloadResources::CDownloadResources(void)
{
}

CDownloadResources::~CDownloadResources(void)
{
	m_listResPathSrc.clear();
	m_listResPathDest.clear();
}

bool CDownloadResources::findAndCopy(const char* szScrPath, const char* szDestPath)
{
	if(!PathIsDirectory(szDestPath) && !CreateDirectory(szDestPath, NULL))
	{
		GF_Log(LOG_ERROR, "%s, 创建文件夹%s失败", __FUNCTION__, szDestPath);
		return false;
	}
	SetFileAttributes(szDestPath, GetFileAttributes(szDestPath) & !FILE_ATTRIBUTE_READONLY);
	char szBuf[MAX_PATH];
	_snprintf_s(szBuf, MAX_PATH, "%s\\*.*", szScrPath);

	CFileFind finder;
	BOOL bFind = finder.FindFile(szBuf);
	while (bFind)
	{
		bFind = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		char szDest[MAX_PATH];
		_snprintf_s(szDest, MAX_PATH, "%s\\%s", szDestPath, finder.GetFileName());

		if (finder.IsDirectory())
		{
			if(!findAndCopy(finder.GetFilePath(), szDest))
				return false;

			continue;
		}		

		SetFileAttributes(szDest, GetFileAttributes(szDest) & !FILE_ATTRIBUTE_READONLY);

		bool bCopy = false;

		if(0 == _access(szDest, 0))
		{
			FILETIME createTime,lassAccessTime,modifyTime;
			FILETIME createTime_n,lassAccessTime_n,modifyTime_n;
			CFile file;
			if(!file.Open(finder.GetFilePath(), CFile::modeRead))
				return false;
			if(!GetFileTime(file.m_hFile, &createTime, &lassAccessTime, &modifyTime))
			{
				file.Close();
				return false;
			}
			file.Close();
			if(!file.Open(szDest, CFile::modeRead))
				return false;
			if(!GetFileTime(file.m_hFile, &createTime_n, &lassAccessTime_n, &modifyTime_n))
			{
				file.Close();
				return false;
			}
			file.Close();
			
			SYSTEMTIME modifySysTime;
			SYSTEMTIME modifySysTime_n;
			if(!FileTimeToSystemTime(&modifyTime, &modifySysTime))
				return false;
			if(!FileTimeToSystemTime(&modifyTime_n, &modifySysTime_n))
				return false;
			CTime modifyCTime(modifySysTime);
			CTime modifyCTime_n(modifySysTime_n);
			if(modifyCTime_n < modifyCTime)
			{
				if(!CopyFile(finder.GetFilePath(), szDest, FALSE))
				{
					GF_Log(LOG_ERROR, "%s, 复制文件%s失败", __FUNCTION__, finder.GetFilePath());
					return false;
				}
				SetFileAttributes(szDest, GetFileAttributes(szDest) & !FILE_ATTRIBUTE_READONLY);
				bCopy = true;
			}
		}
		else
		{
			if(!CopyFile(finder.GetFilePath(), szDest, FALSE))
			{
				GF_Log(LOG_ERROR, "%s, 复制文件%s失败", __FUNCTION__, finder.GetFilePath());
				return false;
			}
			SetFileAttributes(szDest, GetFileAttributes(szDest) & !FILE_ATTRIBUTE_READONLY);
			bCopy = true;
		}

		if(bCopy)
		{
			CString csLog = szDest;
			csLog = "正在下载：" + csLog;
			GF_Log(LOG_NORMAL, csLog);
		}
		++m_iCurIndex;
	}
	return true;
}

void CDownloadResources::Download()
{
	if(m_listResPathDest.size() != m_listResPathSrc.size())
		return;
	for (size_t i=0; i<m_listResPathSrc.size(); ++i)
	{
		m_iFileNum = countFileNum(m_listResPathSrc[i].c_str());
		m_iCurIndex = 0;
		if(0 != m_iFileNum)
		{
			if(!findAndCopy(m_listResPathSrc[i].c_str(), m_listResPathDest[i].c_str()))
			{
				GF_Log(LOG_ERROR, "%s, 更新资源失败", __FUNCTION__);
				return;
			}
		}
	}
	MessageBox(AfxGetMainWnd()->GetSafeHwnd(), "下载资源成功", "完成", MB_OK);
}

int CDownloadResources::countFileNum(const char* szDir)
{
	int iFileCnt = 0;
	char szBuf[MAX_PATH];
	_snprintf_s(szBuf, MAX_PATH, "%s\\*.*", szDir);	
	CFileFind finder;
	BOOL bFind = finder.FindFile(szBuf);
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (finder.IsDots())
			continue;		

		if (finder.IsDirectory())
		{
			char szDest[MAX_PATH];
			_snprintf_s(szDest, MAX_PATH, "%s\\%s", szDir, finder.GetFileName());
			iFileCnt += countFileNum(szDest);
			continue;
		}
		++iFileCnt;
	}
	return iFileCnt;
}

void CDownloadResources::AddPath(const char* szSrcPath, const char* szDestPath)
{
	std::string strSrcPath(szSrcPath);
	m_listResPathSrc.push_back(strSrcPath);

	std::string strDestPath(szDestPath);
	m_listResPathDest.push_back(strDestPath);
}
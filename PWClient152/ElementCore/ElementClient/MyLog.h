// MyLog.h: interface for the MyLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYLOG_H__06FD5BB0_75B7_4661_9913_A4950DC45430__INCLUDED_)
#define AFX_MYLOG_H__06FD5BB0_75B7_4661_9913_A4950DC45430__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class MyLog
{
public:
	~MyLog();

	bool Init(const char *szLogFile, const char *szHead = NULL);
	void Log(const char *szFormat, ...);
	void Close();

	static MyLog & GetInstance();

private:
	MyLog();
	bool IsInited();
	void CheckFileBackup();

	long GetDefaultBakSize();
	long GetMaxBakSize();

private:
	FILE*	m_file;
	char	m_fileName[MAX_PATH];
	long	m_logBakSize;
	int		m_nBakFailure;
};

#endif // !defined(AFX_MYLOG_H__06FD5BB0_75B7_4661_9913_A4950DC45430__INCLUDED_)

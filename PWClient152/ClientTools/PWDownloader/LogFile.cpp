/*
 * FILE: LogFile.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/7/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "LogFile.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static const char* GetTimeStamp(int type)
{
	static char szTime[80];

	SYSTEMTIME st;
	GetLocalTime(&st);
	if( type == 0 )
		_snprintf(szTime, 80, "%02d/%02d/%d %02d:%02d:%02d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
	else
		_snprintf(szTime, 80, "[%02d:%02d:%02d.%03d]", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return szTime;
}


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class CLogFile
//  
///////////////////////////////////////////////////////////////////////////

// 构造函数
CLogFile::CLogFile()
{
	m_pLog = NULL;
}

// 析构函数
CLogFile::~CLogFile()
{
	Close();
}

// 获取实例
CLogFile& CLogFile::GetInstance()
{
	static CLogFile obj;
	return obj;
}

// 创建日志文件
bool CLogFile::CreateLog(const char* szLogFile)
{
	Close();

	m_pLog = fopen("Logs\\PWDownloader.log", "w");
	if( !m_pLog ) return false;

	fprintf(m_pLog, "CPW Client Downloader Error Log\n");
	fprintf(m_pLog, "Created(or opened) on %s\n\n", GetTimeStamp(0));
	return true;
}

// 关闭日志
void CLogFile::Close()
{
	if( m_pLog )
	{
		WriteLog("Log file closed successfully!");

		fclose(m_pLog);
		m_pLog = NULL;
	}
}

// 写入日志
void CLogFile::WriteLog(const char* szMsg)
{
	if( m_pLog && szMsg )
	{
		fprintf(m_pLog, "%s %s\n", GetTimeStamp(1), szMsg);
		fflush(m_pLog);
	}
}

// 写入日志
void LogOutput(const char* szMsg, ...)
{
	static char szText[4096];

	va_list args;
	va_start(args, szMsg);
	_vsnprintf(szText, 4096, szMsg, args);
	va_end(args);

	CLogFile::GetInstance().WriteLog(szText);
}

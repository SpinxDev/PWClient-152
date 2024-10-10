#include "FAudioEngine.h"
#include "FLog.h"
#include <memory.h>
#include <direct.h>
#include <WTypes.h>
#include <time.h>
#include <windows.h> 
using namespace AudioEngine;

Log::Log(void)
{
	memset(m_szLogFilePath, 0, sizeof(char) * MAX_PATH);
}

Log::~Log(void)
{
	Close();
}

bool Log::Open(const char* szFileName, LogLevel logLevel /*= LOG_NORMAL*/, bool bAppend /*= false*/, bool bFlushImmediate /*= false*/, bool bAsyn /*= true*/,  bool bDebuggerOutput /*= false*/)
{
	if(!szFileName)
		return false;
	_mkdir("Logs");
	sprintf_s(m_szLogFilePath, "Logs\\%s", szFileName);
	m_File.open(m_szLogFilePath, std::ios::out);

	m_OutputLogLevel = logLevel;
	m_bDebuggerOutput = bDebuggerOutput;
	m_bFlushImmediate = bFlushImmediate;
	m_bAsyn = bAsyn;
	return true;
}

bool Log::Write(LogLevel logLevel, const char* szFormat, ...)
{
	if(logLevel < m_OutputLogLevel)
		return true;

	memset(m_szLog, 0, sizeof(char) * LOG_LENGTH);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	sprintf(m_szLog, "<%d>[%2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d %3.3d] ", 
		logLevel, sysTime.wDay, sysTime.wMonth, sysTime.wYear,
		sysTime.wHour, sysTime.wMinute,sysTime.wSecond,	sysTime.wMilliseconds);
	va_list argList;
	char szLog[LOG_LENGTH] = {0};
	va_start(argList, szFormat);
	_vsnprintf_s(szLog, LOG_LENGTH*sizeof(char), szFormat, argList);
	va_end(argList);
	if(0 != strcat_s(m_szLog, szLog))
		return false;
	if(0 != strcat_s(m_szLog, "\n"))
		return false;
	m_File<<m_szLog;
	if(m_bFlushImmediate)
		if(!Flush())
			return false;
#ifdef _DEBUG
	if(m_bDebuggerOutput)
		OutputDebugString(m_szLog);
#endif
	return true;
}

bool Log::Close()
{
	m_File.close();
	return true;
}

bool Log::Flush()
{
	m_File.flush();
	return true;
}
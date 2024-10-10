#include "FLogManager.h"

using namespace AudioEngine;

LogManager::LogManager(void)
{
	m_pEngineLog = 0;
}

LogManager::~LogManager(void)
{
	release();
}

bool LogManager::Init()
{
	m_pEngineLog = new Log;
	if(!m_pEngineLog->Open("AudioEngine.log"))
		return false;
	m_pEngineLog->Write(LOG_NORMAL, "Create Log.");
	return true;
}

void LogManager::release()
{
	NameLogMap::const_iterator it = m_mapNameLog.begin();
	for (; it!=m_mapNameLog.end(); ++it)
	{
		Log* pLog = it->second;
		pLog->Flush();
		pLog->Close();
		delete pLog;
	}
	m_mapNameLog.clear();

	m_pEngineLog->Flush();
	m_pEngineLog->Close();
	delete m_pEngineLog;
	m_pEngineLog = 0;
}

bool LogManager::CreateLog(const char* szName, const char* szFileName, LogLevel logLevel /*= LOG_NORMAL*/, bool bAppend /*= false*/, bool bFlushImmediate /*= false*/, bool bAsyn /*= true*/, bool bDebuggerOutput /*= false*/)
{
	if(m_mapNameLog.find(szName) != m_mapNameLog.end())
	{
		m_pEngineLog->Write(LOG_NORMAL, "名字为%s的日志已经存在", szName);
		return false;
	}
	Log* pLog = new Log;
	if(!pLog->Open(szFileName, logLevel, bAppend, bFlushImmediate, bAsyn, bDebuggerOutput))
		return false;
	m_mapNameLog[szName] = pLog;
	return true;
}

bool LogManager::CloseLog(const char* szName)
{
	NameLogMap::iterator it = m_mapNameLog.find(szName);
	if(it == m_mapNameLog.end())
	{
		m_pEngineLog->Write(LOG_NORMAL, "名字为%s的日志不存在", szName);
		return false;
	}
	Log* pLog = it->second;
	pLog->Flush();
	pLog->Close();
	delete pLog;
	m_mapNameLog.erase(it);
	return true;
}

Log* LogManager::GetLog(const char* szName) const
{
	NameLogMap::const_iterator it = m_mapNameLog.find(szName);
	if(it == m_mapNameLog.end())
	{
		m_pEngineLog->Write(LOG_NORMAL, "名字为%s的日志不存在", szName);
		return false;
	}

	return it->second;
}
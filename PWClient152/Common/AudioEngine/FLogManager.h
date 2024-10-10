#pragma once

#include "FExportDecl.h"
#include <map>
#include <string>
#include "FLog.h"

namespace AudioEngine
{
	class Log;

	class _EXPORT_DLL_ LogManager
	{
		typedef std::map<std::string, Log*> NameLogMap;
	public:
		LogManager(void);
		~LogManager(void);

	public:
		bool		Init();		
		bool		CreateLog(const char* szName, const char* szFileName, LogLevel logLevel = LOG_NORMAL, bool bAppend = false, bool bFlushImmediate = false, bool bAsyn = true, bool bDebuggerOutput = false);
		bool		CloseLog(const char* szName);
		Log*		GetEngineLog() const { return m_pEngineLog; }
		Log*		GetLog(const char* szName) const;
	protected:
		void		release();
	protected:
		NameLogMap	m_mapNameLog;
		Log*		m_pEngineLog;
	};
}

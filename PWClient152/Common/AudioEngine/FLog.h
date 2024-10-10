#pragma once

#include "FExportDecl.h"
#include <cstdio>
#include <fstream>

#define LOG_LENGTH	1024
#define MAX_PATH 260

namespace AudioEngine
{
	typedef enum
	{
		LOG_LOWEST		= -2,
		LOG_LOW			= -1,
		LOG_NORMAL		= 0,
		LOG_WARNING		= 1,
		LOG_ERROR		= 2,
		LOG_HIGH		= 3,
		LOG_HIGHEST		= 4
	}LogLevel;

	class _EXPORT_DLL_ Log
	{	
	public:
		Log(void);
		~Log(void);
	public:
		bool		Open(const char* szFileName, LogLevel logLevel = LOG_NORMAL, bool bAppend = false, bool bFlushImmediate = false, bool bAsyn = true, bool bDebuggerOutput = false);
		bool		Write(LogLevel logLevel, const char* szFormat, ...);
		bool		Close();
		bool		Flush();

		void		SetOutputLogLevel(LogLevel logLevel) { m_OutputLogLevel = logLevel; }
		void		EnableFlushImmediate(bool bEnable = true) { m_bFlushImmediate = bEnable; }
		void		EnableDebuggerOutput(bool bEnable = true) { m_bDebuggerOutput = bEnable; }
		void		EnableAsyn(bool bEnable = true) { m_bAsyn = bEnable; }
		const char*	GetLastLog() const { return m_szLog; }
	protected:
		char		m_szLogFilePath[MAX_PATH];
		char		m_szLog[LOG_LENGTH];
		bool		m_bDebuggerOutput;
		bool		m_bFlushImmediate;
		LogLevel	m_OutputLogLevel;
		bool		m_bAsyn;
		std::ofstream	m_File;
	};
}
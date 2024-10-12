// MyLog.cpp: implementation of the MyLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyLog.h"
#include <stdarg.h>
#include <io.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyLog & MyLog::GetInstance()
{
	static MyLog s_instance;
	return s_instance;
}

MyLog::MyLog() : m_file(NULL)
{
	ZeroMemory(m_fileName, sizeof(m_fileName));
	m_logBakSize = GetDefaultBakSize();
	m_nBakFailure = 0;
}

MyLog::~MyLog()
{
	Close();
}

bool MyLog::IsInited()
{
	return m_file != NULL;
}

void MyLog::Close()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = NULL;
		ZeroMemory(m_fileName, sizeof(m_fileName));
	}
}

bool MyLog::Init(const char *szLogFile, const char *szHead)
{
	bool bRet(false);

	Close();

	if (szLogFile && szLogFile[0])
	{
		FILE *file = fopen(szLogFile, "a+");
		if (file)
		{
			m_file = file;
			strncpy(m_fileName, szLogFile, sizeof(m_fileName));
			bRet = true;

			if (szHead && szHead[0])
			{
				Log("***************************************************");
				Log(szHead);
				Log("***************************************************");
			}
			else
			{
				Log("--------------------------------------------------");
			}
		}
	}

	return bRet;
}

void MyLog::Log(const char *szFormat, ...)
{
	if (IsInited())
	{
		char buffer[1024] = {0};

		//	写入时间戳
		SYSTEMTIME st;
		GetLocalTime(&st);
		_snprintf(buffer,
			sizeof(buffer)/sizeof(buffer[0]),
			"[%02d/%02d/%d %02d:%02d:%02d.%03d] ",
			st.wMonth, st.wDay, st.wYear,
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		fwrite(buffer, sizeof(buffer[0]), strlen(buffer), m_file);

		//	写入日志信息
		va_list args;
		va_start(args, szFormat);
		_vsnprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), szFormat, args);
		va_end(args);
		fwrite(buffer, sizeof(buffer[0]), strlen(buffer), m_file);

		//	写入换行符
		const char *szLine = "\n";
		fwrite(szLine, sizeof(char), strlen(szLine), m_file);

		//	将缓冲写入文件
		fflush(m_file);

		//	检查文件大小并做相应备份
		CheckFileBackup();
	}
}

void MyLog::CheckFileBackup()
{
	if (!IsInited())
	{
		//	文件未初始化
		return;
	}
	
	long len = ftell(m_file);
	if (len < m_logBakSize)
	{
		//	文件大小未达到备份要求
		return;
	}

	//	文件已经较大，尝试备份

	//	检查连续备份失败次数
	if (m_nBakFailure > 3)
	{
		//	已经连续 3 次备份失败了，不再尝试
		return;
	}

	bool bReopen = false;
	bool bOK = false;
	char szLogFile[MAX_PATH] = {0};
	while (true)
	{		
		//	尝试关闭当前日志文件
		strncpy(szLogFile, m_fileName, sizeof(szLogFile));
		if (fclose(m_file))
		{
			fprintf(stderr, "Backup %s error (cannot close)\n", m_fileName);
			break;
		}
		m_file = NULL;
		ZeroMemory(m_fileName, sizeof(m_fileName));

		//	以下操作均需要再次打开原文件
		bReopen = true;

		//	检查并删除旧有备份文件
		char szBackupName[MAX_PATH] = {0};
		strcat(szBackupName, szLogFile);
		strcat(szBackupName, ".bak");
		if (0 == _access(szBackupName, 0))
		{
			//	文件存在

			if (remove(szBackupName))
			{
				//	旧有文件无法删除，不能备份
				fprintf(stderr, "Backup %s error (cannot remove last bak file.)\n", szLogFile);
				break;
			}

			//	文件已删除
		}
		//	else 文件不存在，不需要删除

		//	将旧有文件重命名为备份文件
		if (rename(szLogFile, szBackupName))
		{
			//	重命名失败
			fprintf(stderr, "Backup %s error (cannot create new bak file.)\n", szLogFile);
			break;
		}

		//	重命名成功（备份成功）
		bOK = true;

		//	重新打开文件
		break;
	}

	if (bReopen)
	{
		//	打开未备份成功的文件，或打开新文件
		if (!Init(szLogFile))
		{
			//	打开文件失败
			fprintf(stderr, "Backup %s error (Cannot reopen/recreate for logging\n", szLogFile);
		}
	}
	if (bOK)
	{
		//	备份成功
		Log("Backup %s success!", szLogFile);

		//	清除以往失败记录
		m_nBakFailure = 0;

		//	重新设置下次备份检查要求
		m_logBakSize = GetDefaultBakSize();
	}
	else
	{
		//	备份失败
		m_logBakSize *= 2;
		long maxSize = GetMaxBakSize();
		if (m_logBakSize > maxSize)
			m_logBakSize = maxSize;

		m_nBakFailure++;
	}
}

long MyLog::GetDefaultBakSize()
{
	//	第一次备份检查大小
	return 20 * 1024 * 1024;
}

long MyLog::GetMaxBakSize()
{
	//	备份文件的最大检查大小
	return 5 * GetDefaultBakSize();
}
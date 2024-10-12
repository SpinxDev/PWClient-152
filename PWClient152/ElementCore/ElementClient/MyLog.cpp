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

		//	д��ʱ���
		SYSTEMTIME st;
		GetLocalTime(&st);
		_snprintf(buffer,
			sizeof(buffer)/sizeof(buffer[0]),
			"[%02d/%02d/%d %02d:%02d:%02d.%03d] ",
			st.wMonth, st.wDay, st.wYear,
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		fwrite(buffer, sizeof(buffer[0]), strlen(buffer), m_file);

		//	д����־��Ϣ
		va_list args;
		va_start(args, szFormat);
		_vsnprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), szFormat, args);
		va_end(args);
		fwrite(buffer, sizeof(buffer[0]), strlen(buffer), m_file);

		//	д�뻻�з�
		const char *szLine = "\n";
		fwrite(szLine, sizeof(char), strlen(szLine), m_file);

		//	������д���ļ�
		fflush(m_file);

		//	����ļ���С������Ӧ����
		CheckFileBackup();
	}
}

void MyLog::CheckFileBackup()
{
	if (!IsInited())
	{
		//	�ļ�δ��ʼ��
		return;
	}
	
	long len = ftell(m_file);
	if (len < m_logBakSize)
	{
		//	�ļ���Сδ�ﵽ����Ҫ��
		return;
	}

	//	�ļ��Ѿ��ϴ󣬳��Ա���

	//	�����������ʧ�ܴ���
	if (m_nBakFailure > 3)
	{
		//	�Ѿ����� 3 �α���ʧ���ˣ����ٳ���
		return;
	}

	bool bReopen = false;
	bool bOK = false;
	char szLogFile[MAX_PATH] = {0};
	while (true)
	{		
		//	���Թرյ�ǰ��־�ļ�
		strncpy(szLogFile, m_fileName, sizeof(szLogFile));
		if (fclose(m_file))
		{
			fprintf(stderr, "Backup %s error (cannot close)\n", m_fileName);
			break;
		}
		m_file = NULL;
		ZeroMemory(m_fileName, sizeof(m_fileName));

		//	���²�������Ҫ�ٴδ�ԭ�ļ�
		bReopen = true;

		//	��鲢ɾ�����б����ļ�
		char szBackupName[MAX_PATH] = {0};
		strcat(szBackupName, szLogFile);
		strcat(szBackupName, ".bak");
		if (0 == _access(szBackupName, 0))
		{
			//	�ļ�����

			if (remove(szBackupName))
			{
				//	�����ļ��޷�ɾ�������ܱ���
				fprintf(stderr, "Backup %s error (cannot remove last bak file.)\n", szLogFile);
				break;
			}

			//	�ļ���ɾ��
		}
		//	else �ļ������ڣ�����Ҫɾ��

		//	�������ļ�������Ϊ�����ļ�
		if (rename(szLogFile, szBackupName))
		{
			//	������ʧ��
			fprintf(stderr, "Backup %s error (cannot create new bak file.)\n", szLogFile);
			break;
		}

		//	�������ɹ������ݳɹ���
		bOK = true;

		//	���´��ļ�
		break;
	}

	if (bReopen)
	{
		//	��δ���ݳɹ����ļ���������ļ�
		if (!Init(szLogFile))
		{
			//	���ļ�ʧ��
			fprintf(stderr, "Backup %s error (Cannot reopen/recreate for logging\n", szLogFile);
		}
	}
	if (bOK)
	{
		//	���ݳɹ�
		Log("Backup %s success!", szLogFile);

		//	�������ʧ�ܼ�¼
		m_nBakFailure = 0;

		//	���������´α��ݼ��Ҫ��
		m_logBakSize = GetDefaultBakSize();
	}
	else
	{
		//	����ʧ��
		m_logBakSize *= 2;
		long maxSize = GetMaxBakSize();
		if (m_logBakSize > maxSize)
			m_logBakSize = maxSize;

		m_nBakFailure++;
	}
}

long MyLog::GetDefaultBakSize()
{
	//	��һ�α��ݼ���С
	return 20 * 1024 * 1024;
}

long MyLog::GetMaxBakSize()
{
	//	�����ļ���������С
	return 5 * GetDefaultBakSize();
}
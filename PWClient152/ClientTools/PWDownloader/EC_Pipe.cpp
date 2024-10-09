/*
 * FILE: EC_Pipe.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/7/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "EC_Pipe.h"
#include "EC_PWDownload.h"

#include "LogFile.h"


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


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class CECPipe
//  
///////////////////////////////////////////////////////////////////////////

// ���캯��
CECPipe::CECPipe()
{
	m_hWndClient = NULL;
	m_hPipe = NULL;
	m_iPipeDataSize = 0;
	m_iSendedSize = 0;
}

// ��������
CECPipe::~CECPipe()
{
	SendDataQueue::iterator it = m_SendQueue.begin();
	while( it != m_SendQueue.end() )
	{
		delete[] it->pBuf;
		++it;
	}

	if( m_hPipe )
	{
		CloseHandle(m_hPipe);
		m_hPipe = NULL;
	}
}

// ��ʼ���ܵ�
bool CECPipe::Init(HWND hClientWnd)
{
	m_hWndClient = hClientWnd;

	if( !WaitNamedPipe(APP_PIPE_NAME, NMPWAIT_WAIT_FOREVER) )
	{
		LogOutput("CECPipe::Init, Can't find the named pipe instance!");
		return false;
	}

	// ���ӹܵ�
	m_hPipe = CreateFile(APP_PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( INVALID_HANDLE_VALUE == m_hPipe )
	{
		m_hPipe = NULL;
		LogOutput("CECPipe::Init, Open the named pipe failed!");
		return false;
	}

	return true;
}

// ���¹ܵ�
void CECPipe::Tick()
{
	// �����յ�������
	ProcessPipeData();

	// ��������Ͷ���
	ProcessSendData();
}

// ��Ӧ�ܵ����ݵ���
void CECPipe::OnPipeData(int iSize)
{
	assert(m_iPipeDataSize == 0);
	if( iSize > 0 && iSize <= PIPEDATA_MAXSIZE )
		m_iPipeDataSize = iSize;
	else
	{
		assert(0);
		LogOutput("CECPipe::OnPipeData, Invalid pipe data size!");
	}
}

// ��������
bool CECPipe::SendData(void* pBuf, int iSize)
{
	if( !pBuf || !iSize )
		return false;

	if( iSize > PIPEDATA_MAXSIZE )
	{
		LogOutput("CECMCDownload::SendData, The size to send is too large!");
		return false;
	}
	
	// ����ȥ�����ݻ�û�б��Է�����
	// �Է�������������û�д���
	if( m_iSendedSize > 0 || m_iPipeDataSize > 0 )
	{
		LogOutput("CECMCDownload::SendData, the data sended hasn't been processed!");
		return false;
	}

	DWORD dwWrite;
	if( !WriteFile(m_hPipe, pBuf, iSize, &dwWrite, NULL) )
	{
		LogOutput("CECPipe::SendData, Write data to pipe failed!");
		return false;
	}

	m_iSendedSize = iSize;

	if( dwWrite == iSize )
		::PostMessage(m_hWndClient, WM_SENDPIPEDATA, 0, iSize);
	else
	{
		LogOutput("CECPipe::SendData, the size of data written is error!");
		return false;
	}

	return true;
}

// ֪ͨ�ͻ����������
bool CECPipe::SendDownloadOK()
{
	CMD_HEAD cmd;
	cmd.id = OUT_DOWNLOAD_OK;
	return AddSendDataToQueue(&cmd, sizeof(CMD_HEAD));
}

// �ظ��ͻ��˵�ǰ������״̬
void CECPipe::SendTaskInfo(bool bDown, float progress, float dspeed, float uspeed)
{
#pragma pack(1)
	struct DATA
	{
		CMD_HEAD cmd;
		CMD_TASKINFO info;
	} data;
#pragma pack()

	data.cmd.id = OUT_TASKINFO;
	data.info.isdownloading = bDown;
	data.info.progress = progress;
	data.info.dspeed = dspeed;
	data.info.uspeed = uspeed;
	AddSendDataToQueue(&data, sizeof(DATA));
}

// ��Ӵ��������ݵ�����
bool CECPipe::AddSendDataToQueue(void* pBuf, int iSize)
{
	SENDDATA data;
	data.iSize = iSize;
	data.pBuf = new char[iSize];
	memcpy(data.pBuf, pBuf, iSize);
	m_SendQueue.push_back(data);
	return true;
}

// ��������Ͷ���
void CECPipe::ProcessSendData()
{
	if( !m_SendQueue.size() )
		return;

	SENDDATA& data = m_SendQueue.front();
	if( SendData(data.pBuf, data.iSize) )
	{
		OnDataSended(data);
		delete[] data.pBuf;
		m_SendQueue.pop_front();
	}
}

// ��Ӧ���ݵķ���
void CECPipe::OnDataSended(const SENDDATA& data)
{
	int id = ((CMD_HEAD*)data.pBuf)->id;

	// ����ÿ�����ݵ����
	// ...
}

// ���ܵ����ݵĴ�С�Ƿ���ȷ
bool CECPipe::CheckValid(BYTE* pBuf, int iSize)
{
	if( iSize < sizeof(CMD_HEAD) )
		return false;
	
	int iCmd = ((CMD_HEAD*)pBuf)->id;
	int iDataSize = iSize - sizeof(CMD_HEAD);
	switch(iCmd)
	{
	case IN_DOWNLOAD:		return iDataSize == sizeof(CMD_DOWNLOAD);
	case IN_INSTALL:		return iDataSize == 0;
	case IN_QUIT:			return iDataSize == 0;
	case IN_GETTASKINFO:	return iDataSize == 0;
	case IN_GETDOWNLOADOK:	return iDataSize == 0;
	case IN_SWITCHGAME:		return iDataSize == sizeof(bool);
	default:				return false;
	}
}

// �������Ĺܵ�����
void CECPipe::ProcessPipeData()
{
	BYTE byBuf[PIPEDATA_MAXSIZE];

	if( m_iPipeDataSize > 0 )
	{
		DWORD dwRead;
		if( !ReadFile(m_hPipe, byBuf, m_iPipeDataSize, &dwRead, NULL) )
		{
			LogOutput("CECPipe::ProcessPipeData, Read data from pipe failed!");
			return;
		}
		
		if( (int)dwRead == m_iPipeDataSize )
			m_iPipeDataSize = 0;
		else
		{
			LogOutput("CECPipe::ProcessPipeData, pipe data size does not match!");
			return;
		}

		if( !CheckValid(byBuf, dwRead) )
		{
			assert(0);
			LogOutput("CECPipe::ProcessPipeData, Invalid pipe data size!");
			return;
		}

		// ֪ͨ�Է������Ѵ���
		PostMessage(m_hWndClient, WM_SENDPIPEDATA, 2, 0);

		CMD_HEAD* pHead = (CMD_HEAD*)byBuf;
		switch(pHead->id)
		{
		case IN_QUIT:
			CECPWDownload::GetInstance().Quit();
			break;
		case IN_INSTALL:
			CECPWDownload::GetInstance().Install();
			break;
		case IN_GETTASKINFO:
			{
				float progress, dspeed, uspeed;
				CECPWDownload& dl = CECPWDownload::GetInstance();
				dl.GetTaskInfo(progress, dspeed, uspeed);
				bool bDown = dl.IsDownloading();
				SendTaskInfo(bDown, progress, dspeed, uspeed);
			}
			break;
		case IN_GETDOWNLOADOK:
			if( CECPWDownload::GetInstance().IsDownloadOK() )
				SendDownloadOK();
			break;
		case IN_DOWNLOAD:
			if( !CECPWDownload::GetInstance().IsDownloading() )
			{
				CMD_DOWNLOAD* pDL = (CMD_DOWNLOAD*)(byBuf + sizeof(CMD_HEAD));
				CECPWDownload::GetInstance().StartDownload();
				CECPWDownload::GetInstance().SetDownSpeed(pDL->dspeed, pDL->uspeed);
			}
			break;
		case IN_SWITCHGAME:
			{
				bool bIsEnter = *(bool*)(byBuf + sizeof(CMD_HEAD));
				CECPWDownload::GetInstance().OnEnterOrLeaveGame(bIsEnter);
			}
			break;
		default: assert(0);
		}
	}
}

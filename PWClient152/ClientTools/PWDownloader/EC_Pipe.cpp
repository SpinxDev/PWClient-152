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

// 构造函数
CECPipe::CECPipe()
{
	m_hWndClient = NULL;
	m_hPipe = NULL;
	m_iPipeDataSize = 0;
	m_iSendedSize = 0;
}

// 析构函数
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

// 初始化管道
bool CECPipe::Init(HWND hClientWnd)
{
	m_hWndClient = hClientWnd;

	if( !WaitNamedPipe(APP_PIPE_NAME, NMPWAIT_WAIT_FOREVER) )
	{
		LogOutput("CECPipe::Init, Can't find the named pipe instance!");
		return false;
	}

	// 连接管道
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

// 更新管道
void CECPipe::Tick()
{
	// 处理收到的数据
	ProcessPipeData();

	// 处理待发送队列
	ProcessSendData();
}

// 响应管道数据到来
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

// 发送数据
bool CECPipe::SendData(void* pBuf, int iSize)
{
	if( !pBuf || !iSize )
		return false;

	if( iSize > PIPEDATA_MAXSIZE )
	{
		LogOutput("CECMCDownload::SendData, The size to send is too large!");
		return false;
	}
	
	// 发出去的数据还没有被对方处理
	// 对方发过来的数据没有处理
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

// 通知客户端下载完毕
bool CECPipe::SendDownloadOK()
{
	CMD_HEAD cmd;
	cmd.id = OUT_DOWNLOAD_OK;
	return AddSendDataToQueue(&cmd, sizeof(CMD_HEAD));
}

// 回复客户端当前的下载状态
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

// 添加待发送数据到队列
bool CECPipe::AddSendDataToQueue(void* pBuf, int iSize)
{
	SENDDATA data;
	data.iSize = iSize;
	data.pBuf = new char[iSize];
	memcpy(data.pBuf, pBuf, iSize);
	m_SendQueue.push_back(data);
	return true;
}

// 处理待发送队列
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

// 响应数据的发送
void CECPipe::OnDataSended(const SENDDATA& data)
{
	int id = ((CMD_HEAD*)data.pBuf)->id;

	// 处理每种数据的情况
	// ...
}

// 检查管道数据的大小是否正确
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

// 处理到来的管道数据
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

		// 通知对方数据已处理
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

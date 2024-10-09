/*
 * FILE: EC_MCDownload.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/7/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "EC_MCDownload.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Configs.h"
#include "EC_UIManager.h"
#include "EC_LoginUIMan.h"
#include "EC_GameUIMan.h"
#include "EC_MsgDef.h"

#include "DlgInfo.h"

#include <A3DEngine.h>
#include <A3DDevice.h>


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
	#define CLIENT_DOWNLOADER_PATH _AL("..\\patcher\\pwdownloader_d.exe")
#else
	#define CLIENT_DOWNLOADER_PATH _AL("..\\patcher\\pwdownloader.exe")
#endif


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
//  Implement class CECMCDownload
//  
///////////////////////////////////////////////////////////////////////////

// ��ȡ����
CECMCDownload& CECMCDownload::GetInstance()
{
	static CECMCDownload obj;
	return obj;
}

// ���캯��
CECMCDownload::CECMCDownload()
{
	m_hWndDownloader = NULL;
	m_hPipe = NULL;
	m_iPipeDataSize = 0;
	m_iSendedSize = 0;
	memset(&m_TaskInfo, 0, sizeof(TASK_INFO));
	m_bDownloadOK = false;
	m_bQuitSended = false;

	m_cntTick.SetPeriod(500);	// һ����һ�ιܵ�
	m_cntStart.SetPeriod(1800000);	// ÿ���Сʱ��ʾ�������
	m_cntStart.SetCounter(1790000);	// �ս���Ϸʱ10����������

	InitializeCriticalSection(&m_csQueue);
}

// ��������
CECMCDownload::~CECMCDownload()
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

	DeleteCriticalSection(&m_csQueue);
}

// �����������ӽ���
bool CECMCDownload::StartDownloader()
{
	ACString strMiniDLTitle = g_pGame->GetConfigs()->GetMiniDLTitle();
	if( !strMiniDLTitle.IsEmpty() )
	{
		HWND hDLWnd = FindWindow(NULL, strMiniDLTitle);
		if( hDLWnd )
		{
			PostMessage(hDLWnd, WM_COMMAND, IDCANCEL, 0);
			
			while(1)
			{
				HANDLE hMutex = CreateMutexA(NULL, TRUE, "CPW_MiniDownloader");
				if( GetLastError() == ERROR_ALREADY_EXISTS )
				{
					Sleep(100);
					CloseHandle(hMutex);
					continue;
				}
				
				CloseHandle(hMutex);
				break;
			}
		}
	}

	// �ȴ���һ���ͻ����������������ܵ�
	Sleep(1000);

	// �����ܵ�
	m_hPipe = CreateNamedPipe(APP_PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 1024, 1024, 0, NULL);
	if( INVALID_HANDLE_VALUE == m_hPipe )
	{
		m_hPipe = NULL;
		a_LogOutput(1, "CECMCDownload::StartDownloader, Create the named pipe failed!");
		return false;
	}

	OVERLAPPED ov;
	memset(&ov, 0, sizeof(ov));
	ov.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if( !ConnectNamedPipe(m_hPipe, &ov) )
	{
		if( ERROR_IO_PENDING != GetLastError() )
		{
			a_LogOutput(1, "CECMCDownload::StartDownloader, Wait the client connecting failed!");
			CloseHandle(ov.hEvent);
			CloseHandle(m_hPipe);
			m_hPipe = NULL;
			return false;
		}
	}

	// ����������
	STARTUPINFO sui;
	memset(&sui, 0, sizeof(STARTUPINFO));
	sui.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;
	ACHAR szCommandLine[80];
	a_sprintf(szCommandLine, _AL("%d %x"), GetCurrentProcessId(), g_pGame->GetA3DDevice()->GetDeviceWnd());
	ACString strWorkDir = _AL("..\\patcher");
	if( !CreateProcess(CLIENT_DOWNLOADER_PATH, szCommandLine, NULL, NULL, TRUE, 0, NULL, strWorkDir, &sui, &pi) )
	{
		a_LogOutput(1, "CECMCDownload::StartDownloader, Run the downloader application failed!");
		return false;
	}
	else
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	// �ȴ��ӽ�������
	if( WAIT_FAILED == WaitForSingleObject(ov.hEvent, INFINITE) )
	{
		a_LogOutput(1, "CECMCDownload::StartDownloader, Wait the event object failed!");
		CloseHandle(ov.hEvent);
		CloseHandle(m_hPipe);
		m_hPipe = NULL;
		return false;
	}

	CloseHandle(ov.hEvent);
	return true;
}

// ���µ�ǰ״̬
void CECMCDownload::Tick(DWORD dwDeltaTime)
{
	static int n = 0;

	if( !g_pGame->GetConfigs()->IsMiniClient() || !m_hWndDownloader )
		return;

	if( g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME &&
		m_cntStart.IncCounter(dwDeltaTime) )
	{
		if( m_bDownloadOK )
		{
			// ��ʾ�������
			OnDownloadOK();
		}
		else if( !m_TaskInfo.isdownloading )
		{
			// ��ʾ��ʼ����
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			if( pGameUI )
				pGameUI->MessageBox("Game_StartDownload", pGameUI->GetStringFromTable(10712), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
		}

		m_cntStart.Reset();
	}

	if( m_cntTick.IncCounter(dwDeltaTime) )
	{
		// �����յ�������
		ProcessPipeData();

		// ����ǰ������״̬
		if( !m_bDownloadOK && n++ > 5 )
		{
			SendGetTaskInfo();
			n = 0;
		}

		// �����Ͷ���
		ProcessSendData();

		DestroyOldPipe();
		m_cntTick.Reset();
	}
}

void CECMCDownload::DestroyOldPipe()
{
	if( !m_hPipe ) return;

	ACString strMiniDLTitle = g_pGame->GetConfigs()->GetMiniDLTitle();
	if( !strMiniDLTitle.IsEmpty() )
	{
		HWND hDLWnd = FindWindow(NULL, strMiniDLTitle);
		if( !hDLWnd )
		{
			CloseHandle(m_hPipe);
			m_hPipe = NULL;
			m_hWndDownloader = NULL;
		}
	}
}

// ���ӽ��̷�������
bool CECMCDownload::SendData(void* pBuf, int iSize)
{
	if( !m_hWndDownloader || !pBuf || !iSize )
		return false;

	if( iSize > PIPEDATA_MAXSIZE )
	{
		a_LogOutput(1, "CECMCDownload::SendData, The size to send is too large!");
		return false;
	}

	// ����ȥ�����ݻ�û�б��Է�����
	// �Է�������������û�д���
	if( m_iSendedSize > 0 || m_iPipeDataSize > 0 )
	{
		//a_LogOutput(1, "CECMCDownload::SendData, the data sended hasn't been processed!");
		return false;
	}

	DWORD dwWrite;
	if( !WriteFile(m_hPipe, pBuf, iSize, &dwWrite, NULL) )
	{
		a_LogOutput(1, "CECMCDownload::SendData, Write data to pipe failed!");
		return false;
	}

	InterlockedExchange(reinterpret_cast<volatile long*>(&m_iSendedSize), iSize);

	if( (int)dwWrite == iSize )
		::PostMessage(m_hWndDownloader, WM_SENDPIPEDATA, 0, iSize);
	else
	{
		a_LogOutput(1, "CECMCDownload::SendData, the size of data written is error!");
		return false;
	}

	return true;
}

void CECMCDownload::OnSendedDataProcessed()
{
	InterlockedExchange(reinterpret_cast<volatile long*>(&m_iSendedSize), 0);
}

// ��ȡ��ǰ������Ϣ
void CECMCDownload::SendGetTaskInfo()
{
	CMD_HEAD cmd;
	cmd.id = OUT_GETTASKINFO;
	AddSendDataToQueue(&cmd, sizeof(CMD_HEAD));
}

// ֪ͨ�ӽ����˳�
void CECMCDownload::SendQuit()
{
	CMD_HEAD cmd;
	cmd.id = OUT_QUIT;
	AddSendDataToQueue(&cmd, sizeof(cmd));
}

// ֪ͨ�ӽ��̿�ʼ��װ
void CECMCDownload::SendInstall()
{
	CMD_HEAD cmd;
	cmd.id = OUT_INSTALL;
	AddSendDataToQueue(&cmd, sizeof(CMD_HEAD));
}

// ����Ƿ��������
bool CECMCDownload::SendGetDownloadOK()
{
	CMD_HEAD cmd;
	cmd.id = OUT_GETDOWNLOADOK;
	return AddSendDataToQueue(&cmd, sizeof(CMD_HEAD));
}

// ֪ͨ�ӽ��̿�ʼ����
bool CECMCDownload::SendDownload(int dspeed, int uspeed)
{
#pragma pack(1)
	struct 
	{
		CMD_HEAD cmd;
		START_DOWNLOAD info;
	} data;
#pragma pack()

	data.cmd.id = OUT_DOWNLOAD;
	data.info.dspeed = dspeed;
	data.info.uspeed = uspeed;
	return AddSendDataToQueue(&data, sizeof(data));
}

// ֪ͨ�ӽ��̽�����Ϸ����
bool CECMCDownload::SendSwitchGame(bool bEnter)
{
#pragma pack(1)
	struct
	{
		CMD_HEAD cmd;
		bool bIsEnter;
	} data;
#pragma pack()

	data.cmd.id = OUT_SWITCHGAME;
	data.bIsEnter = bEnter;
	return AddSendDataToQueue(&data, sizeof(data));
}

// ��ӷ������ݵ�����
bool CECMCDownload::AddSendDataToQueue(void* pBuf, int iSize, bool bFront)
{
	if( !m_hPipe || !m_hWndDownloader )
		return false;

	SENDDATA data;
	data.iSize = iSize;
	data.pBuf = new char[iSize];
	memcpy(data.pBuf, pBuf, iSize);
	ACSWrapper csa(&m_csQueue);
	if( !bFront )
		m_SendQueue.push_back(data);
	else
		m_SendQueue.push_front(data);
	return true;
}

// �����Ͷ���
void CECMCDownload::ProcessSendData()
{
	ACSWrapper csa(&m_csQueue);
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

// ��Ӧ���ݷ��ͳ�ȥ
void CECMCDownload::OnDataSended(const SENDDATA& data)
{
	int id = ((CMD_HEAD*)data.pBuf)->id;
	if( id == OUT_INSTALL )
	{
		if( g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME )
			g_pGame->GetGameRun()->PostMessage(MSG_EXITGAME, -1, 0);
		else
			::PostMessage(g_pGame->GetGameInit().hWnd, WM_QUIT, 0, 0);
	}
	else if( id == OUT_QUIT )
	{
		m_bQuitSended = true;
	}
}

// �ܵ��﷢������
void CECMCDownload::OnPipeData(int iSize)
{		
	ASSERT(m_iPipeDataSize == 0);
	if( iSize > 0 && iSize <= PIPEDATA_MAXSIZE )
		InterlockedExchange(reinterpret_cast<volatile long*>(&m_iPipeDataSize), iSize);
	else
	{
		ASSERT(0);
		a_LogOutput(1, "CECMCDownload::OnPipeData, Invalid pipe data size!");
	}
}

// ���ܵ����ݴ�С�Ƿ�Ϸ�
bool CECMCDownload::CheckValid(BYTE* pBuf, int iSize)
{
	if( iSize < sizeof(CMD_HEAD) )
		return false;
	
	int iCmd = ((CMD_HEAD*)pBuf)->id;
	int iDataSize = iSize - sizeof(CMD_HEAD);
	switch(iCmd)
	{
	case IN_DOWNLOAD_OK:	return iDataSize == 0;
	case IN_TASKINFO:		return iDataSize == sizeof(TASK_INFO);
	default:				return false;
	}
}

// ����ܵ�����
void CECMCDownload::ProcessPipeData()
{
	BYTE byBuf[PIPEDATA_MAXSIZE];

	if( m_iPipeDataSize > 0 )
	{
		DWORD dwRead;
		if( !ReadFile(m_hPipe, byBuf, m_iPipeDataSize, &dwRead, NULL) )
		{
			a_LogOutput(1, "CECMCDownload::ProcessPipeData, Read data from pipe failed!");
			return;
		}

		if( (int)dwRead == m_iPipeDataSize )
			InterlockedExchange(reinterpret_cast<volatile long*>(&m_iPipeDataSize), 0);
		else
		{
			a_LogOutput(1, "CECMCDownload::ProcessPipeData, pipe data size does not match!");
			return;
		}

		if( !CheckValid(byBuf, dwRead) )
		{
			ASSERT(0);
			a_LogOutput(1, "CECMCDownload::ProcessPipeData, Invalid pipe data size!");
			return;
		}

		// ֪ͨ�Է������Ѵ���
		PostMessage(m_hWndDownloader, WM_SENDPIPEDATA, 2, 0);

		CMD_HEAD* pHead = (CMD_HEAD*)byBuf;
		switch(pHead->id)
		{
		case IN_DOWNLOAD_OK:
			OnDownloadOK();
			break;
		case IN_TASKINFO:
			{
				TASK_INFO* pInfo = (TASK_INFO*)(byBuf + sizeof(CMD_HEAD));
				memcpy(&m_TaskInfo, pInfo, sizeof(TASK_INFO));
			}
			break;
		}
	}
}

// ��Ӧ�������
void CECMCDownload::OnDownloadOK()
{
	CECUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager();
	if( pUIMan->GetCurrentUIMan() == CECUIManager::UIMAN_LOGIN )
	{
		CECLoginUIMan* pLoginUI = pUIMan->GetLoginUIMan();
		if( pLoginUI )
			pLoginUI->MessageBox("Game_DownloadOK", pLoginUI->GetStringFromTable(10160), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else
	{
		CECGameUIMan* pGameUI = pUIMan->GetInGameUIMan();
		if( pGameUI )
			pGameUI->MessageBox("Game_DownloadOK", pGameUI->GetStringFromTable(10704), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
	}

	m_bDownloadOK = true;
	m_cntStart.Reset();
}

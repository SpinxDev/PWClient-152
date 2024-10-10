/*
 * FILE: A3DMultiThreadRender.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2012, 7, 10
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DMultiThreadRender.h"
#include "A3DDevice.h"

A3DDevice* A3DMultiThreadRender::m_pA3DDevice = NULL;

PlayBackThreadProcCallBackFunc A3DMultiThreadRender::m_PlayBackCallBackFunc = A3DMultiThreadRender::PlayBackThreadProcImpl;
HANDLE A3DMultiThreadRender::m_RecordSemaphore = NULL;
HANDLE A3DMultiThreadRender::m_PlayBackSemaphore  = NULL;
HANDLE A3DMultiThreadRender::m_PlayBackThread = NULL;
volatile bool A3DMultiThreadRender::m_bInitedMultiThreadRender = false;
volatile bool A3DMultiThreadRender::m_bMultiThreadRender = false;
HANDLE A3DMultiThreadRender::m_hRTExitEvent = FALSE;
volatile bool A3DMultiThreadRender::m_bMTRendering = false;
volatile bool A3DMultiThreadRender::m_bAllowRecordToOverlapPlayback = true;
CRITICAL_SECTION A3DMultiThreadRender::m_MTCriticalSec;

volatile bool A3DMultiThreadRender::m_bTickEnable = true;
volatile bool _render_reset_flag = false;

bool A3DMultiThreadRender::Init(A3DDevice* pA3DDevice)
{
	if( !pA3DDevice)
		return false;

	m_pA3DDevice = pA3DDevice;

	m_RecordSemaphore = NULL;
	m_PlayBackSemaphore = NULL;
	m_PlayBackThread = NULL;
	m_bInitedMultiThreadRender = false;
	m_bMultiThreadRender = false;
	m_bMTRendering = false;
	m_bAllowRecordToOverlapPlayback = true;
	m_hRTExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	InitializeCriticalSection(&m_MTCriticalSec);
	return true;
}
void A3DMultiThreadRender::Release()
{
	DeleteCriticalSection(&m_MTCriticalSec);
	CloseHandle(m_hRTExitEvent);
}

void A3DMultiThreadRender::SetupMTRenderThreads()
{
	if( m_bMultiThreadRender && !m_bInitedMultiThreadRender)
	{
		//Destroy thread
		DestroyMTRenderThreads();
		SetMultiThreadRender(true); //DestroyMTRenderThreads() ��g_bMultiThreadRender = false;

		//Create thread
		m_RecordSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
		m_PlayBackSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

		m_bInitedMultiThreadRender = true; //FIXME!! Render Thread �߳���lost device
		m_bMTRendering = false;
		
		ResetEvent(m_hRTExitEvent);

		DWORD dwPBThreadID = 0;
		m_PlayBackThread = CreateThread(NULL, 0, PlayBackThreadProc, 0, 0, &dwPBThreadID);
	}
}

void A3DMultiThreadRender::DestroyMTRenderThreads()
{
	SetMultiThreadRender(false); 
	SetEvent(m_hRTExitEvent);	//����ڶ��߳����˳�ʱ��PlayBack�߳�����ѭ����
								//��һ֡�˳����߳�

	if(m_PlayBackThread)
	{
		ReleaseSemaphore(m_PlayBackSemaphore, 1, NULL); // ������߳�->���߳̿����л�ʱ��
		//RecordRenderOperation�߳�û�����ͷ�g_PlayBackSemaphore����PlayBack�߳����޵ȴ�
		WaitForSingleObject(m_PlayBackThread, INFINITE);//����Render thread һ���ʱ���˳�������D3DDevoce->BeginScene()֮��δ����EndScene(),
		//�ٴε���Beginscene()
		CloseHandle(m_PlayBackThread);
		m_PlayBackThread = NULL;
	}

	if( m_RecordSemaphore)
	{
		CloseHandle(m_RecordSemaphore);
		m_RecordSemaphore = NULL;
	}

	if( m_PlayBackSemaphore)
	{
		CloseHandle(m_PlayBackSemaphore);
		m_PlayBackSemaphore = NULL;
	}

	m_bInitedMultiThreadRender = false;
	m_bMTRendering = false;
}

DWORD WINAPI A3DMultiThreadRender::PlayBackThreadProc(LPVOID pParam)
{
	assert(m_PlayBackCallBackFunc);
	return (*m_PlayBackCallBackFunc)(pParam);
}

DWORD WINAPI A3DMultiThreadRender::PlayBackThreadProcImpl(LPVOID pParam)
{
	if (!m_bMTRendering)//First
	{
		ReleaseSemaphore(m_RecordSemaphore, 1, NULL);
		m_bMTRendering = true;
	}

	while(true)
	{
		if(WaitForSingleObject(m_hRTExitEvent, 0) != WAIT_TIMEOUT)//FIXME!!
		{
			return 0;
		}

		WaitForSingleObject(m_PlayBackSemaphore, INFINITE);

		EnterCriticalSection(&m_MTCriticalSec);
		if( m_bAllowRecordToOverlapPlayback) 
		{
			if(m_bTickEnable)
			{
				m_pA3DDevice->SwitchCBRecorder();  
			}
			ReleaseSemaphore(m_RecordSemaphore, 1, NULL);
		}

		//Sleep(40);
		if( m_bMTRendering)
		{
			//FIXME!!Play Back
			//OutputDebugStr("Render\n");	
			m_pA3DDevice->Playback();	
		}

		

		if( !m_bAllowRecordToOverlapPlayback) 
		{
			if(m_bTickEnable)
			{
				m_pA3DDevice->SwitchCBRecorder();  
			}
			ReleaseSemaphore(m_RecordSemaphore, 1, NULL);
		}
		LeaveCriticalSection(&m_MTCriticalSec);
	}
	return 0;
}

void A3DMultiThreadRender::RecordRenderOperation(RecorderParam* pParam)
{
	WaitForSingleObject(m_RecordSemaphore, INFINITE);

	//OutputDebugStr("Tick\n");

	if(pParam->pRender->IsNeedResetDevice() || m_pA3DDevice->IsCBPlayerNeedReset())
	{
		EnterCriticalSection(&m_MTCriticalSec);
		pParam->pRender->BeforeResetDevice();//�ͻ��˹������м����ͷ�D3D��Դ���߳�
		m_pA3DDevice->SetNeedResetFlag(true);
		_render_reset_flag = true;


		//����ǰ��Ҫ�ͷŵ���Դ---begin
		m_pA3DDevice->SetCBPlayerNeedReset(false); 
		m_pA3DDevice->BeginCBRecord(); 
		m_pA3DDevice->CopyRResRec2CBRec();
		m_pA3DDevice->EndCBRecord();	
		m_pA3DDevice->Playback(true);
		//����ǰ��Ҫ�ͷŵ���Դ---end

		m_pA3DDevice->SetCBPlayerNeedReset(true);
		pParam->pRender->ResetDevice();
		m_pA3DDevice->SetCBPlayerNeedReset(false);

		m_pA3DDevice->SetNeedResetFlag(false);

		m_pA3DDevice->InvalidAllCBRecorder();
		pParam->pRender->AfterResetDevice();//�ͻ����������м����ͷ�D3D��Դ���߳�
		LeaveCriticalSection(&m_MTCriticalSec);
	}
	else
	{
		//	Do logic tick
		if( m_bTickEnable)
		{
			pParam->pRender->Tick(pParam->dwTickTime);	
		}

		m_pA3DDevice->BeginCBRecord(); 
		pParam->pRender->Render();
		m_pA3DDevice->CopyRResRec2CBRec();
		m_pA3DDevice->EndCBRecord();
	}

	ReleaseSemaphore(m_PlayBackSemaphore, 1, NULL);

	return;
}

void A3DMultiThreadRender::SetMultiThreadRender(bool bMultiThreadRender)
{
	m_bMultiThreadRender = bMultiThreadRender;
}

void A3DMultiThreadRender::SetTickEnable(bool bTickEnable)
{
	m_bTickEnable = bTickEnable;
}
/*
 * FILE: EC_MCDownload.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/7/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include <windows.h>
#include <AChar.h>
#include <AAssist.h>
#include <list>
#include "EC_Counter.h"


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define PIPEDATA_MAXSIZE 256

//  �������ӽ���֪ͨ�ͻ����йܵ����ݵ���
#define WM_SENDPIPEDATA (WM_USER + 8905)

#define APP_PIPE_NAME _AL("\\\\.\\pipe\\PWDownloader")


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECMCDownload
//	
///////////////////////////////////////////////////////////////////////////

class CECMCDownload
{
public:

	// �����ӽ��̵�����
	enum
	{
		OUT_DOWNLOAD,		// ��ʼ����
		OUT_INSTALL,		// ��ʼ��װ
		OUT_QUIT,			// �˳���Ϸ
		OUT_GETTASKINFO,	// ��ȡ������Ϣ
		OUT_GETDOWNLOADOK,	// ����Ƿ��������
		OUT_SWITCHGAME,		// ������Ϸ

		OUT_NUM,
	};

	// �յ��ӽ��̵�����
	enum
	{
		IN_DOWNLOAD_OK,		// �������
		IN_TASKINFO,		// �յ���ǰ������Ϣ

		IN_NUM,
	};

#pragma pack(1)

	struct CMD_HEAD
	{
		int id;
	};

	struct TASK_INFO
	{
		bool isdownloading;	// �Ƿ���������
		float down_speed;	// �����ٶ�
		float up_speed;		// �ϴ��ٶ�
		float progress;		// ���ؽ���
	};

	struct START_DOWNLOAD
	{
		int dspeed;			// �����ٶ�����
		int uspeed;			// �ϴ��ٶ�����
	};

#pragma pack()

	///////////////////////////////////////////////////////////////////////

	// �������ݶ���
	struct SENDDATA
	{
		char* pBuf;
		int iSize;
	};

	typedef std::list<SENDDATA> SendDataQueue;

public:
	virtual ~CECMCDownload();

	// ����������
	bool StartDownloader();

	// ���µ�ǰ״̬
	void Tick(DWORD dwDeltaTime);

	// ��Ӧ�ܵ���Ϣ�ĵ���
	void OnPipeData(int iSize);

	// ����ȥ�������Ѿ����Է���Ӧ
	void OnSendedDataProcessed();

	// �����������ӽ��̵Ĵ��ھ��
	void SetDownloaderHWnd(HWND hWnd) { m_hWndDownloader = hWnd; }

	// ����������Ϣ
	void SendGetTaskInfo();
	// ֪ͨ�ӽ����˳�
	void SendQuit();
	// ֪ͨ�ӽ��̿�ʼ��װ
	void SendInstall();
	// ����Ƿ��������
	bool SendGetDownloadOK();
	// ֪ͨ��ʼ����
	bool SendDownload(int dspeed, int uspeed);
	// ֪ͨ�ӽ��̽�����Ϸ����
	bool SendSwitchGame(bool bEnter);

	// ��ȡ��ǰ���ؽ���
	float GetCurProgress() const { return m_TaskInfo.progress; }

	// ��ȡ����
	static CECMCDownload& GetInstance();

protected:
	CECCounter m_cntTick;
	HANDLE m_hPipe;

	volatile HWND m_hWndDownloader;	// �ӽ��̵Ĵ��ھ��
	volatile int m_iSendedSize;		// �Ѿ����ͳ�ȥ���ȴ��Է���������ݴ�С
	volatile int m_iPipeDataSize;	// �Ѿ��յ������ݴ�С

	CRITICAL_SECTION m_csQueue;	// ���м���
	SendDataQueue m_SendQueue;	// ���Ͷ���

	TASK_INFO m_TaskInfo;	// ��ǰ�������
	bool m_bDownloadOK;		// �Ƿ��������
	bool m_bQuitSended;		// �Ѿ������˰�װ������
	CECCounter m_cntStart;

protected:
	CECMCDownload();

	// ��ӷ������ݵ�����
	bool AddSendDataToQueue(void* pBuf, int iSize, bool bFront = false);
	// �����Ͷ���
	void ProcessSendData();
	// ���ӽ��̷�������
	bool SendData(void* pBuf, int iSize);
	// ��Ӧ���ݷ���ȥ
	void OnDataSended(const SENDDATA& data);

	// ���ܵ����ݵĴ�С�Ƿ���ȷ
	bool CheckValid(BYTE* pBuf, int iSize);
	// ����ܵ��������
	void ProcessPipeData();

	// ��Ӧ������ɵĲ���
	void OnDownloadOK();

	// ���ٹ��ڵ������ܵ�
	void DestroyOldPipe();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

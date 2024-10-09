/*
 * FILE: EC_Pipe.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/7/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once


#include <stdio.h>
#include <string>
#include <windows.h>
#include <list>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define PIPEDATA_MAXSIZE 256

// �ͻ���֪ͨ���йܵ����ݵ���
#define WM_SENDPIPEDATA (WM_USER + 8905)

// �����ܵ�������
#define APP_PIPE_NAME "\\\\.\\pipe\\PWDownloader"


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

// �����ͻ��˵�����
enum
{
	OUT_DOWNLOAD_OK,	// �������
	OUT_TASKINFO,		// ���͵�ǰ������Ϣ

	OUT_NUM,
};

// �յ��ͻ��˵�����
enum
{
	IN_DOWNLOAD,		// ��ʼ����
	IN_INSTALL,			// ��ʼ��װ
	IN_QUIT,			// �˳���Ϸ
	IN_GETTASKINFO,		// �ͻ�������������Ϣ
	IN_GETDOWNLOADOK,	// �ͻ��˼���Ƿ��������
	IN_SWITCHGAME,		// ��Ӧ������Ϸ

	IN_NUM,
};

#pragma pack(1)

struct CMD_HEAD
{
	int id;
};

struct CMD_TASKINFO
{
	bool isdownloading;
	float progress;
	float dspeed;
	float uspeed;
};

struct CMD_DOWNLOAD
{
	int dspeed;
	int uspeed;
};

#pragma pack()


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  class CECPipe
//  
///////////////////////////////////////////////////////////////////////////

class CECPipe
{
public:

	struct SENDDATA
	{
		char* pBuf;
		int iSize;
	};

	// ���������ݵĶ���
	typedef std::list<SENDDATA> SendDataQueue;

public:
	CECPipe();
	virtual ~CECPipe();

	// ��ʼ���ܵ�
	bool Init(HWND hClientWnd);

	// ���¹ܵ�
	void Tick();

	// ��Ӧ�ܵ����ݵ���
	void OnPipeData(int iSize);

	// ����ȥ�������Ѿ����Է���Ӧ
	void OnSendedDataProcessed() { m_iSendedSize = 0; }

	bool SendDownloadOK();
	void SendTaskInfo(bool bDown, float progress, float dspeed, float uspeed);

protected:
	HWND m_hWndClient;		// �ͻ��˴��ھ��
	HANDLE m_hPipe;			// �ܵ��ľ��
	int m_iPipeDataSize;	// �Ѿ��յ������ݴ�С
	int m_iSendedSize;		// �Ѿ����ͳ�ȥ���ȴ��Է���������ݴ�С

	SendDataQueue m_SendQueue;	// ���Ͷ���

	// ��ӷ������ݵ�����
	bool AddSendDataToQueue(void* pBuf, int iSize);
	// ��������Ͷ���
	void ProcessSendData();
	// �������ݵ��ͻ���
	bool SendData(void* pBuf, int iSize);
	// ��Ӧ���ݵķ���
	void OnDataSended(const SENDDATA& data);

	// ���ܵ����ݵĴ�С�Ƿ���ȷ
	bool CheckValid(BYTE* pBuf, int iSize);

	// ����ܵ��������
	void ProcessPipeData();
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////

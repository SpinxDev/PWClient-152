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

// 客户端通知我有管道数据到来
#define WM_SENDPIPEDATA (WM_USER + 8905)

// 命名管道的名字
#define APP_PIPE_NAME "\\\\.\\pipe\\PWDownloader"


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

// 发给客户端的命令
enum
{
	OUT_DOWNLOAD_OK,	// 下载完成
	OUT_TASKINFO,		// 发送当前下载信息

	OUT_NUM,
};

// 收到客户端的命令
enum
{
	IN_DOWNLOAD,		// 开始下载
	IN_INSTALL,			// 开始安装
	IN_QUIT,			// 退出游戏
	IN_GETTASKINFO,		// 客户端请求下载信息
	IN_GETDOWNLOADOK,	// 客户端检查是否下载完毕
	IN_SWITCHGAME,		// 响应进出游戏

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

	// 待发送数据的队列
	typedef std::list<SENDDATA> SendDataQueue;

public:
	CECPipe();
	virtual ~CECPipe();

	// 初始化管道
	bool Init(HWND hClientWnd);

	// 更新管道
	void Tick();

	// 响应管道数据到来
	void OnPipeData(int iSize);

	// 发出去的数据已经被对方响应
	void OnSendedDataProcessed() { m_iSendedSize = 0; }

	bool SendDownloadOK();
	void SendTaskInfo(bool bDown, float progress, float dspeed, float uspeed);

protected:
	HWND m_hWndClient;		// 客户端窗口句柄
	HANDLE m_hPipe;			// 管道的句柄
	int m_iPipeDataSize;	// 已经收到的数据大小
	int m_iSendedSize;		// 已经发送出去，等待对方处理的数据大小

	SendDataQueue m_SendQueue;	// 发送队列

	// 添加发送数据到队列
	bool AddSendDataToQueue(void* pBuf, int iSize);
	// 处理待发送队列
	void ProcessSendData();
	// 发送数据到客户端
	bool SendData(void* pBuf, int iSize);
	// 响应数据的发送
	void OnDataSended(const SENDDATA& data);

	// 检查管道数据的大小是否正确
	bool CheckValid(BYTE* pBuf, int iSize);

	// 处理管道里的数据
	void ProcessPipeData();
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////

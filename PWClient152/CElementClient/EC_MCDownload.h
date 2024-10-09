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

//  下载器子进程通知客户端有管道数据到来
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

	// 发给子进程的命令
	enum
	{
		OUT_DOWNLOAD,		// 开始下载
		OUT_INSTALL,		// 开始安装
		OUT_QUIT,			// 退出游戏
		OUT_GETTASKINFO,	// 获取下载信息
		OUT_GETDOWNLOADOK,	// 检查是否下载完毕
		OUT_SWITCHGAME,		// 进出游戏

		OUT_NUM,
	};

	// 收到子进程的命令
	enum
	{
		IN_DOWNLOAD_OK,		// 下载完成
		IN_TASKINFO,		// 收到当前下载信息

		IN_NUM,
	};

#pragma pack(1)

	struct CMD_HEAD
	{
		int id;
	};

	struct TASK_INFO
	{
		bool isdownloading;	// 是否正在下载
		float down_speed;	// 下载速度
		float up_speed;		// 上传速度
		float progress;		// 下载进度
	};

	struct START_DOWNLOAD
	{
		int dspeed;			// 下载速度限制
		int uspeed;			// 上传速度限制
	};

#pragma pack()

	///////////////////////////////////////////////////////////////////////

	// 发送数据队列
	struct SENDDATA
	{
		char* pBuf;
		int iSize;
	};

	typedef std::list<SENDDATA> SendDataQueue;

public:
	virtual ~CECMCDownload();

	// 开启下载器
	bool StartDownloader();

	// 更新当前状态
	void Tick(DWORD dwDeltaTime);

	// 响应管道消息的到来
	void OnPipeData(int iSize);

	// 发出去的数据已经被对方响应
	void OnSendedDataProcessed();

	// 设置下载器子进程的窗口句柄
	void SetDownloaderHWnd(HWND hWnd) { m_hWndDownloader = hWnd; }

	// 请求下载信息
	void SendGetTaskInfo();
	// 通知子进程退出
	void SendQuit();
	// 通知子进程开始安装
	void SendInstall();
	// 检查是否下载完毕
	bool SendGetDownloadOK();
	// 通知开始下载
	bool SendDownload(int dspeed, int uspeed);
	// 通知子进程进出游戏世界
	bool SendSwitchGame(bool bEnter);

	// 获取当前下载进度
	float GetCurProgress() const { return m_TaskInfo.progress; }

	// 获取单例
	static CECMCDownload& GetInstance();

protected:
	CECCounter m_cntTick;
	HANDLE m_hPipe;

	volatile HWND m_hWndDownloader;	// 子进程的窗口句柄
	volatile int m_iSendedSize;		// 已经发送出去，等待对方处理的数据大小
	volatile int m_iPipeDataSize;	// 已经收到的数据大小

	CRITICAL_SECTION m_csQueue;	// 队列加锁
	SendDataQueue m_SendQueue;	// 发送队列

	TASK_INFO m_TaskInfo;	// 当前下载情况
	bool m_bDownloadOK;		// 是否下载完毕
	bool m_bQuitSended;		// 已经发送了安装的命令
	CECCounter m_cntStart;

protected:
	CECMCDownload();

	// 添加发送数据到队列
	bool AddSendDataToQueue(void* pBuf, int iSize, bool bFront = false);
	// 处理发送队列
	void ProcessSendData();
	// 向子进程发送数据
	bool SendData(void* pBuf, int iSize);
	// 响应数据发出去
	void OnDataSended(const SENDDATA& data);

	// 检查管道数据的大小是否正确
	bool CheckValid(BYTE* pBuf, int iSize);
	// 处理管道里的数据
	void ProcessPipeData();

	// 响应下载完成的操作
	void OnDownloadOK();

	// 销毁过期的命名管道
	void DestroyOldPipe();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

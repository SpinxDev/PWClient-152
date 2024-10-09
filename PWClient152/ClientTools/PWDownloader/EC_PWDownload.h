/*
 * FILE: EC_PWDownload.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/7/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include <vector.h>
#include <hashmap.h>
#include <string>
#include "i_p2sp_download.h"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECPipe;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  class CECPWDownload
//  
///////////////////////////////////////////////////////////////////////////

class CECPWDownload
{
	// 下载线程
	friend DWORD WINAPI P2SPDownloadThread(LPVOID lpParameter);

	class CScopedLock
	{
	public:
		CScopedLock(CRITICAL_SECTION* cs) : m_cs(cs) { EnterCriticalSection(cs); }
		~CScopedLock() { LeaveCriticalSection(m_cs); }
	private:
		CRITICAL_SECTION* m_cs;
	};

	struct DOWNLOAD_INFO
	{
		std::string md5;
		std::string filename;
		int downspeed;
		int upspeed;
	};

public:
	virtual ~CECPWDownload();

	// 初始化/销毁下载器
	bool Init(HWND hWndClient);
	void Release();

	// 开始下载
	bool StartDownload();

	// 停止下载
	void StopDownload();

	// 响应进出游戏
	void OnEnterOrLeaveGame(bool bEnter);

	// 开始安装
	void Install();

	// 退出程序
	void Quit();

	// 更新逻辑
	void Tick();

	// 获取当前下载状态
	bool GetTaskInfo(float& fProgress, float& fDownSpeed, float& fUpSpeed);

	// 设置下载速度
	bool SetDownSpeed(int downspeed, int upspeed);

	// 获取当前设置的下载速度
	bool GetMaxSpeed(int& downspeed, int& upspeed);

	// 获取管道指针
	CECPipe* GetPipe() { return m_pPipe; }

	// 是否正在下载
	bool IsDownloading() const { return m_hDownloadThread != NULL; }

	// 是否下载完毕
	bool IsDownloadOK() const { return m_bDownloadOK; }

	// 获取错误码
	int GetErrorCode();

	// 获取实例
	static CECPWDownload& GetInstance();

protected:
	HMODULE m_hP2SPDLL;
	std::string m_sServerAddr;			// MD5文件下载服务器
	HANDLE m_hDownloadThread;			// 下载线程
	bool m_bP2SPInitOK;					// P2SP库初始化完毕
	CRITICAL_SECTION m_csInfoLock;		// 下载信息加锁
	TaskInfomationT m_DownloadInfo;		// 当前下载信息
	DOWNLOAD_INFO m_LastDownInfo;		// 上次下载的信息
	CECPipe* m_pPipe;					// 与客户端通信的管道

	volatile bool m_bDownloadOK;		// 下载完成
	volatile bool m_bStopThread;		// 停止下载线程

protected:
	CECPWDownload();

	// 初始化p2sp引擎
	bool p2sp_Start(int gameID);
	
	// 设置获取配置
	bool p2sp_Setting(const SettingItemT* setting);
	bool p2sp_GetSetting(SettingItemT* setting);
	bool p2sp_SetLsp(LSP_SETTING_FLAG flag);

	// 添加一个下载任务
	bool p2sp_AddDownload(const char* filemd5);
	
	// 开始一个下载任务
	bool p2sp_StartDownload(const char* filemd5);
	
	// 停止下载任务
	bool p2sp_StopDownload(const char* filemd5);
	
	// 删除下载任务
	bool p2sp_DeleteDownload(const char* filemd5);
	
	// 获取任务信息
	bool p2sp_GetTaskInformation(TaskInfomationT* info);
	
	// 停止p2sp引擎
	bool p2sp_Stop();

	// 获取当前更新服务器的地址
	bool GetUpdateServer();

	// 获取待下载客户端的MD5
	bool GetFullClientMD5(std::string& strMD5, std::string& strFile);

	// 获取上次下载的信息
	bool GetLastClientDL();

	// 写入未完成的客户端下载信息
	void WriteLastClientDL();

	// 启动P2SP引擎
	bool StartP2SPEngine();
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////

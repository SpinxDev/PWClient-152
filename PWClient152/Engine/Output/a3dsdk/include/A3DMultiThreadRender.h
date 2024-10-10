/*
 * FILE: A3DMultiThreadRender.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2012, 7, 10
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */
#ifndef _A3D_MULTI_THREAD_RENDER_H_
#define _A3D_MULTI_THREAD_RENDER_H_

#include <windows.h>

class A3DDevice;

class IA3DRender
{
public:
	virtual bool IsNeedResetDevice() = 0;
	virtual bool ResetDevice() = 0;

	virtual bool Tick(DWORD dwTickTime) = 0;
	virtual bool Render() = 0;

	virtual bool BeforeResetDevice() = 0; //客户端挂起所有加载释放D3D资源的线程
	virtual bool AfterResetDevice() = 0;  //客户端启动所有加载释放D3D资源的线程
};

typedef DWORD (WINAPI *PlayBackThreadProcCallBackFunc)(LPVOID pParam);

class A3DMultiThreadRender
{
public:
	struct RecorderParam //FIXME!!
	{
		DWORD dwTickTime;
		IA3DRender* pRender;    
	};
public:
	static bool Init(A3DDevice* pA3DDevice);
	static void Release();

	static bool IsMultiThreadRender() { return m_bMultiThreadRender;}
	static void SetMultiThreadRender(bool bMultiThreadRender);

	static void SetupMTRenderThreads();
	static void DestroyMTRenderThreads();

	static DWORD WINAPI PlayBackThreadProc(LPVOID pParam);
	static DWORD WINAPI PlayBackThreadProcImpl(LPVOID pParam);
	static void SetPlayBackThreadProcCallBack(PlayBackThreadProcCallBackFunc pFunc) { m_PlayBackCallBackFunc = pFunc; }
	static void RecordRenderOperation(RecorderParam* pParam); //FIXME!!

	static void SetTickEnable(bool bTickEnable);

protected:
	static A3DDevice* m_pA3DDevice;

	static PlayBackThreadProcCallBackFunc m_PlayBackCallBackFunc;
	static HANDLE m_RecordSemaphore;
	static HANDLE m_PlayBackSemaphore;
	static HANDLE m_PlayBackThread;
	static volatile bool m_bInitedMultiThreadRender;	  
	static volatile bool m_bMultiThreadRender;			  	
	static volatile bool m_bMTRendering;				 
	static volatile bool m_bAllowRecordToOverlapPlayback; 
	static HANDLE m_hRTExitEvent;
	static CRITICAL_SECTION m_MTCriticalSec;

	static volatile bool m_bTickEnable;
};

#endif //_A3D_MULTI_THREAD_RENDER_H_
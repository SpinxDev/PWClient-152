/********************************************************************
	created:	2007/08/23
	author:		liudong
	
	purpose:	interprocess communication (shared memory)
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#include "LuaDebugMsg.h"
#include "LuaDebugIPC.h"
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLuaDebugIPC::CLuaDebugIPC()
{
	m_pData       = NULL;
	m_hMapFile    = NULL;
	m_hEventWrite = NULL;
	m_hEventRead  = NULL;
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
}

CLuaDebugIPC::~CLuaDebugIPC()
{
	Release();
}

bool CLuaDebugIPC::Init()
{
	m_hMapFile = CreateFileMapping((HANDLE)-1,
		                            NULL,
								    PAGE_READWRITE,
									0,
									DEBUGGER_FILEMAP_SIZE,
									DEBUGGER_FILEMAP_NAME);

	if (!m_hMapFile) {
		::MessageBox(NULL,_T("failed to create shared memory"),0,0);
		return false;
	}

	m_pData      = MapViewOfFile(m_hMapFile,FILE_MAP_ALL_ACCESS,0,0,0);
	if (!m_pData) {
		Release();
		::MessageBox(NULL,_T("failed to read data from shared memory"),0,0);
		return false;
	}

	m_hEventWrite= CreateEvent(0,0,1,DEBUGGER_EVENT_WRITE);
	if (!m_hEventWrite) {
		Release();
		::MessageBox(NULL,_T("failed to create event for write"),0,0);
		return false;
	}
	
	m_hEventRead     = CreateEvent(0,0,0,DEBUGGER_EVENT_READ);
	if (!m_hEventRead) {
		Release();
		::MessageBox(NULL,_T("failed to create event for read"),0,0);
		return false;
	}
	return true;
}

void CLuaDebugIPC::Release()
{
	if (m_pData) {
		UnmapViewOfFile(m_pData);
		m_pData       = NULL;
	}
	if (m_hMapFile) {
		CloseHandle(m_hMapFile);
		m_hMapFile    = NULL;
	}
	if (m_hEventWrite) {
		CloseHandle(m_hEventWrite);
		m_hEventWrite = NULL;
	}
	if (m_hEventRead) {
		CloseHandle(m_hEventRead);
		m_hEventRead  = NULL;
	}
}

bool CLuaDebugIPC::Send(LuaDebugMsg& msg,
					 DWORD dwMilliseconds/*=INFINITE*/)
{
	//todo:
    if(WAIT_OBJECT_0!=WaitForSingleObject(m_hEventWrite,dwMilliseconds)){
		//time out
		return false;
    }
	INT64 timeNow64;//now counter of CPU
	QueryPerformanceCounter((LARGE_INTEGER *)&timeNow64);
	unsigned long timeNow;//now time (miliseconds)
	timeNow= (unsigned long)((double)timeNow64/(double)m_frequency*1000.0f);
	msg.header.timeStamp= timeNow;
    
	int* pNow= (int*)m_pData;
    *pNow++ =  msg.header.appId;
	*pNow++ =  msg.header.appSubId;
	*pNow++ =  msg.header.direction;
	*pNow++ =  msg.header.msgType;
	*pNow++ =  msg.header.bodyLength;
	*pNow++ =  msg.header.timeStamp;
	*pNow++ =  msg.header.timeDiscard;
	memcpy(pNow,msg.header.appName,DEBUGGER_TOKEN_LEN*sizeof(_TCHAR));
	char* pChar= (char*)pNow;
	pChar +=  DEBUGGER_TOKEN_LEN*sizeof(_TCHAR);
	pNow= (int*)pChar;
 
	void* pDest      = (void*)pNow;
	memcpy(pDest,msg.data,msg.header.bodyLength); 

	//waiting for read
	SetEvent(m_hEventRead);

	return true;
}

bool CLuaDebugIPC::Receive(LuaDebugMsg& msg,
					DWORD dwMilliseconds/*=INFINITE*/)
{
	//todo:

	if (WAIT_OBJECT_0!=WaitForSingleObject(m_hEventRead,dwMilliseconds)) {
	    return false;
	}

	unsigned long appID    = msg.header.appId;
	unsigned long appSubId = msg.header.appSubId;
	int           direction= msg.header.direction;


	int* pNow= (int*)m_pData;
    msg.header.appId       = *pNow++;
	msg.header.appSubId    = *pNow++;
	msg.header.direction   = *pNow++;
	msg.header.msgType     = *pNow++;
	msg.header.bodyLength  = *pNow++;
	msg.header.timeStamp   = *pNow++;
	msg.header.timeDiscard = *pNow++;
	memcpy(msg.header.appName,pNow,DEBUGGER_TOKEN_LEN*sizeof(_TCHAR));
	char* pChar= (char*)pNow;
	pChar +=  DEBUGGER_TOKEN_LEN*sizeof(_TCHAR);
	pNow= (int*)pChar;


	bool _IsWaitOtherRead= false;
	if (msg.header.direction != direction) {
	    _IsWaitOtherRead = true;
	}else if(0!=appID || 0!=appSubId){
		//app
        if (appID!=msg.header.appId || appSubId!=msg.header.appSubId) {
			_IsWaitOtherRead = true;
        }
	}
	
	if (_IsWaitOtherRead) {
		INT64 timeNow64;//now counter of CPU
		QueryPerformanceCounter((LARGE_INTEGER *)&timeNow64);
		unsigned long timeNow;//now time (miliseconds)
		timeNow= (unsigned long)((double)timeNow64/(double)m_frequency*1000.0f);

		if (timeNow - msg.header.timeStamp < msg.header.timeDiscard) 
		{
			//wait for other app or debugger to receive the msg
			SetEvent(m_hEventRead);

		}else{
			//discard the msg because the message is out of time
			SetEvent(m_hEventWrite);
		}
		return false;
	}
	
	msg.Release();
	msg.data= new char[msg.header.bodyLength];
	void* pSrc      = (void*)pNow;
	memcpy(msg.data,pSrc,msg.header.bodyLength); 

	//shared memory is ready for write
	SetEvent(m_hEventWrite);

	return true;
}

bool CLuaDebugIPC::ResetMemoryState(){
	if (!m_hEventWrite || !m_hEventRead) {
        return false;
	}
	SetEvent(m_hEventWrite);
	ResetEvent(m_hEventRead);
	return true;
}

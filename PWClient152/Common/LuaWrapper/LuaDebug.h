/********************************************************************
	created:	2007/08/16
	author:		kuiwu
	
	purpose:	the communication utilty between app and luadebugger
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#pragma once

#include "LuaDebugIPC.h"
#include "LuaDebugMsg.h"
#include "Debugger.h"
#include <AList2.h>
#include <hashmap.h>
#include <tchar.h>

typedef struct lua_State lua_State;
class CDebugger;
class CLuaState;

class CLuaDebug  
{
public:
	CLuaDebug(int type);
	virtual ~CLuaDebug();
	bool Init();
	void ReportError(const char * szBufName, int line, const char * szDesc);

	void SendLocalVariable(LD_TreeRoot *locals);
	void Break(const _TCHAR *szFile, LDM_Break *brk);
	void ShowBufferLine(LDM_ShowBufferLine *showBufferLine);
	void Wait();

	CLuaState* GetState() { return m_pState; }
	void SetState(CLuaState *pState) { m_pState = pState; }

	CDebugger* GetDebugger() { return m_pDebugger; }
	void SetDebugger(CDebugger *pDebugger) { m_pDebugger = pDebugger; }
	bool  IsInDebug() const {return m_bInDebug;}
	void  SetInDebug(bool bInDebug) { m_bInDebug = bInDebug;}
	
	void Tick();

private:
	CLuaDebugIPC  m_LuaDebugIPC; 
	_TCHAR m_Name[DEBUGGER_TOKEN_LEN];
	int  m_ID;
	int  m_SubID;
	
	APtrList<LuaDebugMsg*> m_plSendMsg;	//发送消息队列
	abase::hash_map<AString, bool> m_buffMap;
	bool m_bInDebug;
	CLuaState *m_pState;
	CDebugger *m_pDebugger;
private:
	bool PrepareDebugger();
	void SendBuff();
	bool _FillLDMBuffer(const _TCHAR * name, LDM_SendBuffer& senBufMsg);
	void ClearSendMsg();
};
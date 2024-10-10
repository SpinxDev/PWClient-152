/********************************************************************
	created:	2007/08/16
	author:		kuiwu
	
	purpose:	the communication utilty between app and luadebugger
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#include <windows.h>
#include "LuaDebug.h"
#include "LuaState.h"
#include "LuaScript.h"
#include "LuaUtil.h"
#include "ScriptValue.h"

CLuaDebug::CLuaDebug(int type)
{
	GetModuleFileName(NULL,m_Name,DEBUGGER_TOKEN_LEN-1);

	m_ID  = GetCurrentProcessId();
	m_SubID = type;

	m_pDebugger = new CDebugger();
	m_pDebugger->SetLuaDebug(this);

	m_bInDebug = true;
}

CLuaDebug::~CLuaDebug()
{
	while (m_plSendMsg.GetCount() != 0) {
		LuaDebugMsg *pToDelete = m_plSendMsg.GetHead();
		m_plSendMsg.RemoveHead();
		delete pToDelete;
	}
	if (m_pDebugger != NULL) {
		delete m_pDebugger;
		m_pDebugger = NULL;
	}
}

bool CLuaDebug::Init()
{
	//app check the process, if it isnot working, start it.
	HWND hWndReceive=::FindWindow(DEBUGGER_WIN_NAME,0);
		
	if (!hWndReceive) {
		int returnInt = WinExec(DEBUGGER_PROCESS_NAME,SW_SHOWNORMAL);
		if (ERROR_FILE_NOT_FOUND == returnInt) {
			::MessageBox(0,_T("Enable lua debug, but cannot find debugger!"),0,0);
			return false;
		}else if( returnInt<31 ){
			_TCHAR errorMsg[MAX_PATH];
			_stprintf(errorMsg,_T("无法启动debugger，error: %d"),returnInt);
			::MessageBox(0,errorMsg,0,0);
			return false;
		}
	}
	if(!m_LuaDebugIPC.Init()){
		return false;
	}

	PrepareDebugger();

	return true;
}

bool CLuaDebug::PrepareDebugger() {
	m_pDebugger->Prepare();
	m_pDebugger->StepInto();
	return true;
}


bool CLuaDebug::_FillLDMBuffer(const _TCHAR * name, LDM_SendBuffer& senBufMsg)
{
	_tcscpy(senBufMsg.name, name);
#ifdef _UNICODE
	AString _name;
	CScriptString temp;
	temp.SetAWString(name);
	temp.RetrieveAString(_name);
#else
#define _name name
#endif
	CLuaScript * lua_script = m_pState->GetScript(_name);
	if (lua_script)
	{
		senBufMsg.bufLen = lua_script->RetrieveBuf(NULL);
		senBufMsg.buf =  new char[senBufMsg.bufLen];
		lua_script->RetrieveBuf(senBufMsg.buf);
		return true;
	}
	return false;
}


void CLuaDebug::ReportError(const char * szBufName, int line, const char * szDesc)
{
	SendBuff();	//编译错误时没有buff
	while (m_plSendMsg.GetCount() != 0) {
			LuaDebugMsg *pToSend = m_plSendMsg.GetHead();
			m_LuaDebugIPC.Send(*pToSend);
			m_plSendMsg.RemoveHead();
			delete pToSend;
	}

	//收发公用的消息对象
	LuaDebugMsg  debugMsg;

	// never change
	debugMsg.header.appId    = m_ID;
	debugMsg.header.appSubId = m_SubID;
	_tcscpy(debugMsg.header.appName,m_Name);

	//format err
	LDM_ReportError   errMsg;
//	if (!errMsg.FromLuaError(err))
//	{
//        return;
//	}
#ifdef _UNICODE
	AWString _szBufName, _szDesc;
	CScriptString temp;
	temp.SetAString(szBufName);
	temp.RetrieveAWString(_szBufName);
	temp.SetAString(szDesc);
	temp.RetrieveAWString(_szDesc);
#else
#define _szBufName szBufName
#define _szDesc szDesc
#endif
	_tcsncpy(errMsg.name, _szBufName, DEBUGGER_TOKEN_LEN);
	errMsg.line = line;
	_tcsncpy(errMsg.info, _szDesc, DEBUGGER_INFO_LEN);

	errMsg.line -= 1; // 1-based to 0-based
	//send error message to debugger
	debugMsg.header.direction= LDM_APP2DEBUGGER;
	errMsg.ToLDM(debugMsg);
    m_LuaDebugIPC.Send(debugMsg);
	
	return; // exit directly

	//wait for debugger message
/*	while (1)
	{
		debugMsg.header.appId = m_ID;
		debugMsg.header.appSubId = m_SubID;
		debugMsg.header.direction= LDM_DEBUGGER2APP;
		if (!m_LuaDebugIPC.Receive(debugMsg))
		{
			Sleep(100);
			continue;
		}

		//reveive a msg from debugger
		switch(debugMsg.header.msgType)
		{
			case LDM_DEBUGGER_EXIT:
			{
				//debugger has exit
				m_bInDebug = false;	// not necessary
				PostQuitMessage(0);
				return;
			}
			case LDM_SEND_BUFFER:
			{
				//modify the lua buff
				LDM_SendBuffer receiveMsg;
				receiveMsg.FromLDM(debugMsg);
				CLuaScript * lua_script = m_pState->GetScript(receiveMsg.name);
				LuaModifyHandler hModify = lua_script->GetModifyHandler();
				if (hModify != NULL) {
					hModify(receiveMsg.name,receiveMsg.buf,receiveMsg.bufLen);
				}
				//return here, in the future this maybe change
				break;
			}
			default:
				break;
		}

	}
*/
	//todo: test to work
	//::PostQuitMessage(0);
}





void CLuaDebug::SendLocalVariable(LD_TreeRoot *locals) {
	LuaDebugMsg          msg;
	msg.header.appId = m_ID;
	msg.header.appSubId = m_SubID;
	msg.header.direction = LDM_APP2DEBUGGER;
	_tcscpy(msg.header.appName, m_Name);
	LDM_SendAllLocalVar treeMsg(locals);
	treeMsg.ToLDM(msg);
	LuaDebugMsg *newMsg = new LuaDebugMsg;
	*newMsg = msg;
	m_plSendMsg.AddTail(newMsg);
}

void CLuaDebug::Break(const _TCHAR *szFile, LDM_Break *brk) {
	SendBuff();	// make break the last mesg

	_tcsncpy(brk->name, szFile, DEBUGGER_TOKEN_LEN);
	
	LuaDebugMsg          msg;
	msg.header.appId = m_ID;
	msg.header.appSubId = m_SubID;
	msg.header.direction = LDM_APP2DEBUGGER;
	_tcscpy(msg.header.appName, m_Name);
	brk->ToLDM(msg);
	
	LuaDebugMsg *newMsg = new LuaDebugMsg;
	*newMsg = msg;
	m_plSendMsg.AddTail(newMsg);
}

void CLuaDebug::SendBuff() {
	abase::hash_map<AString, CLuaScript*> *scripts =  m_pState->GetAllScripts();
	abase::hash_map<AString, CLuaScript *>::iterator it;
	for (it = scripts->begin(); it != scripts->end(); ++it)
	{
		AString astring = it->first;
		if (it->second) {
			if(m_buffMap[astring] != true) {
				m_buffMap[astring] = false;
			}
		}
	}
	//send  buff to debugger
	LuaDebugMsg		sendmsg;
	sendmsg.header.appId = m_ID;
	sendmsg.header.appSubId = m_SubID;
	sendmsg.header.direction = LDM_APP2DEBUGGER;
	_tcscpy(sendmsg.header.appName, m_Name);
	LDM_SendBuffer     senbufMsg;

	abase::hash_map<AString, bool>::iterator it2;
	for (it2 = m_buffMap.begin(); it2 != m_buffMap.end(); ++it2)
	{
		senbufMsg.Release();
		if(it2->second != true) {
#ifdef _UNICODE
			AWString _buffname;
			CScriptString temp;
			temp.SetAString(it2->first);
			temp.RetrieveAWString(_buffname);
#else
#define _buffname it2->first
#endif
			if (_FillLDMBuffer(_buffname, senbufMsg))
			{
				sendmsg.header.direction= LDM_APP2DEBUGGER;
				senbufMsg.ToLDM(sendmsg);
				LuaDebugMsg *newMsg = new LuaDebugMsg;
				*newMsg = sendmsg;
				m_plSendMsg.AddTail(newMsg);
			}
			it2->second = true;
		}
	}
}

void CLuaDebug::ClearSendMsg()
{
	while (m_plSendMsg.GetCount() != 0) {
		LuaDebugMsg *pToDelete = m_plSendMsg.GetHead();
		m_plSendMsg.RemoveHead();
		delete pToDelete;
	}
}

void CLuaDebug::Tick() {	//check breakpoints and send new buff
	SendBuff();

	LuaDebugMsg     recvmsg;
	LuaDebugMsg		sendmsg;

	sendmsg.header.appId = m_ID;
	sendmsg.header.appSubId = m_SubID;
	sendmsg.header.direction = LDM_APP2DEBUGGER;
	_tcscpy(sendmsg.header.appName, m_Name);

	while (m_plSendMsg.GetCount() != 0) {
		LuaDebugMsg *pToSend = m_plSendMsg.GetHead();
		if (!m_LuaDebugIPC.Send(*pToSend, 0)) {
			break;
		}
		m_plSendMsg.RemoveHead();
		delete pToSend;
	}

	recvmsg.header.appId = m_ID;
	recvmsg.header.appSubId = m_SubID;
	recvmsg.header.direction= LDM_DEBUGGER2APP;
	if (!m_LuaDebugIPC.Receive(recvmsg,0))
	{
		return;	//
	}

	switch(recvmsg.header.msgType)
	{
		case LDM_BREAK_POINT: {
			LDM_BreakPoint brkpt;
			brkpt.FromLDM(recvmsg);
#ifdef _UNICODE
			AString _brkptName;
			CScriptString temp;
			temp.SetAWString(brkpt.name);
			temp.RetrieveAString(_brkptName);
#else
#define _brkptName brkpt.name
#endif
			CLuaScript *script = m_pState->GetScript(_brkptName);
			if (script != NULL) {
				script->ToggleBreakPoints(brkpt.isAdd, brkpt.lineNum);
			}
			break;
		}
	}
}

void CLuaDebug::Wait() {
	LuaDebugMsg     recvmsg;
	LuaDebugMsg		sendmsg;

	sendmsg.header.appId = m_ID;
	sendmsg.header.appSubId = m_SubID;
	sendmsg.header.direction = LDM_APP2DEBUGGER;
	_tcscpy(sendmsg.header.appName, m_Name);
	while (1)
	{
		while (m_plSendMsg.GetCount() != 0) {
			LuaDebugMsg *pToSend = m_plSendMsg.GetHead();
			if (!m_LuaDebugIPC.Send(*pToSend, 0)) {
				break;
			}
			m_plSendMsg.RemoveHead();
			delete pToSend;
		}
 
		recvmsg.header.appId = m_ID;
		recvmsg.header.appSubId = m_SubID;
		recvmsg.header.direction= LDM_DEBUGGER2APP;
		if (!m_LuaDebugIPC.Receive(recvmsg,0))
		{
			Sleep(1);
			continue;
		}
		//reveive a msg from debugger
		switch(recvmsg.header.msgType)
		{
			case LDM_STEP_INTO:
			{
				ClearSendMsg();
				m_pDebugger->StepInto();
				return;
			}
			case LDM_STEP_OVER:
			{
				ClearSendMsg();
				m_pDebugger->StepOver();
				return;
			}
			case LDM_STEP_OUT:
			{
				ClearSendMsg();
				m_pDebugger->StepOut();
				return;
			}
			case LDM_RUN_TO_CURSOR:
			{
				ClearSendMsg();
				LDM_RunToCursor runToCursor;
				runToCursor.FromLDM(recvmsg);
				m_pDebugger->RunToCursor(runToCursor.name, runToCursor.line);
				return;
			}
			case LDM_GO:
			{
				ClearSendMsg();
				m_pDebugger->Go();
				return;
			}
			case LDM_REQUEST_WATCH_VAR:
			{
				m_pDebugger->GetWatchRoot()->Clear();
				LDM_RequestWatchVar reqWatchVar;
				reqWatchVar.FromLDM(recvmsg);
				WatchVarNameNode *pCurNode = reqWatchVar.m_pHead;
				while(pCurNode) {
					m_pDebugger->Eval(pCurNode->name);
					pCurNode = pCurNode->pNext;
				}
				m_pDebugger->GetWatchRoot()->ResetTreeNode();
				LDM_SendWatchVar var(m_pDebugger->GetWatchRoot());
				var.ToLDM(sendmsg);
				LuaDebugMsg *pNewMsg = new LuaDebugMsg;
				*pNewMsg = sendmsg;
				m_plSendMsg.AddTail(pNewMsg);
				break;
			}
			case LDM_STACK_CHANGE:
			{
				LDM_StackChange stackchange;
				stackchange.FromLDM(recvmsg);
				m_pDebugger->GotoStackTraceLevel(stackchange.level);
				SendLocalVariable(m_pDebugger->GetLocalVarRoot());
				m_pDebugger->ClearLocalVariables();
				break;
			}
			case LDM_BREAK_POINT:
			{
				LDM_BreakPoint brkpt;
				brkpt.FromLDM(recvmsg);
#ifdef _UNICODE
			AString _brkptName;
			CScriptString temp;
			temp.SetAWString(brkpt.name);
			temp.RetrieveAString(_brkptName);
#else
#define _brkptName brkpt.name
#endif
				CLuaScript *script = m_pState->GetScript(_brkptName);
				if (script != NULL) {
					script->ToggleBreakPoints(brkpt.isAdd, brkpt.lineNum);
				}
				break;
			}
			case LDM_SEND_BUFFER: // modify the script
			{
				LDM_SendBuffer sendbuff;
				sendbuff.FromLDM(recvmsg);
#ifdef _UNICODE
			AString _sendbuffName;
			CScriptString temp;
			temp.SetAWString(sendbuff.name);
			temp.RetrieveAString(_sendbuffName);
#else
#define _sendbuffName sendbuff.name
#endif
				CLuaScript * lua_script = m_pState->GetScript(_sendbuffName);
				LuaModifyHandler hModify = lua_script->GetModifyHandler();
				if (hModify)
				{
					hModify(_sendbuffName, sendbuff.buf, sendbuff.bufLen);
				}
				else
				{
					AString msg;
					msg.Format("cannot save the modified buf %s\n", sendbuff.name);
					LUA_DEBUG_INFO(msg);
				}
				break;
			}
			case LDM_DEBUGGER_EXIT:
			{
				m_bInDebug = false;
				//PostQuitMessage(0);
				return;
			}
		}
	}
}

void CLuaDebug::ShowBufferLine(LDM_ShowBufferLine *showBufferLine)
{
	LuaDebugMsg          msg;
	msg.header.appId = m_ID;
	msg.header.appSubId = m_SubID;
	msg.header.direction = LDM_APP2DEBUGGER;
	_tcscpy(msg.header.appName, m_Name);
	showBufferLine->ToLDM(msg);
	
	LuaDebugMsg *newMsg = new LuaDebugMsg;
	*newMsg = msg;
	m_plSendMsg.AddTail(newMsg);
}
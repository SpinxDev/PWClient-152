/************************************************************************
	created:	   2007-9-6   10:00
	filename: 	   Debugger.h
	author:		   baoshiming
	
	description:   interface for CDebugger class, which provids 
				   debug control operations.
/************************************************************************/
#pragma once

#include "LuaDebugHelper.h"
#include "LuaDebug.h"
#include "ATString.h"
#include "LuaState.h"
#include <tchar.h>

class CLuaDebug;

#define DMOD_NONE					0
#define DMOD_STEP_INTO				1
#define DMOD_STEP_OVER				2
#define DMOD_STEP_OUT				3
#define DMOD_RUN_TO_CURSOR			4

#define DMOD_BREAK					10
#define DMOD_STOP					11

class CDebugger  
{
public:
	CDebugger();
	virtual ~CDebugger();

	// ----- control -----
	BOOL Prepare();
	BOOL Start();
	void Stop();

	// ----- debug mode -----
	void Go();
	void Break();
	void StepInto();
	void StepOver();
	void StepOut();
	void RunToCursor(const _TCHAR *name, int nLine);

	// ----- hooks -----
	void LineHook(const char* szFile, int nLine);
	void FunctionHook(const char* szFile, int nLine, BOOL bCall);
	void DebugBreak(const _TCHAR* szFile, int nLine);
	
	// ----- Stack Trace-----
	void ClearStackTrace();
	int  GetStackTraceLevel();
	void AddStackTrace(const _TCHAR* strDesc, const _TCHAR* strFile, int nLine);
	void GotoStackTraceLevel(int nLevel);

	// ----- variable functions -----
	void StackLevelChanged();
	void ClearLocalVariables();
	LD_TreeNode* AddLocalVariable(LD_NameValue &nameValue);
	LD_TreeNode* AddWatchVariable(LD_NameValue &nameValue);
	void Eval(ATString strCode);

	void SetCurlevel(int nlevel) { m_nStacklevel = nlevel; }
	LD_TreeRoot* GetLocalVarRoot() { return &m_LocalVarRoot; }

	CLuaDebug* GetLuaDebug() { return m_pLuaDebug; }
	void SetLuaDebug(CLuaDebug *pLuaDebug) { m_pLuaDebug = pLuaDebug; }

	LD_TreeRoot* GetLocalRoot() { return &m_LocalVarRoot; }
	LD_TreeRoot* GetWatchRoot() { return &m_WatchVarRoot; }
	LD_TreeRoot* GetGlobalRoot() { return &m_GlobalVarRoot; }

protected:
	int m_nMode;				// debug mode
	int m_nLevel;				// relative level, used by step over and run to cursor
	int m_nLine;				// run to cursor line
	_TCHAR m_szBuffName[256];		// run to cursor buffer name
	
	CLuaDebugHelper m_lua;

	LD_TreeRoot     m_GlobalVarRoot;	//所有全局变量,树的根节点
	LD_TreeRoot     m_LocalVarRoot;		//所有局部变量,树的根节点
	LD_TreeRoot     m_WatchVarRoot;		//所有监视变量,树的根节点

	// ----- Stack -----
	ATString		m_strStack;				// Call Stack description
	int				m_nStacklevel;			// current call stack level
	abase::vector<ATString>		m_buffs;	// call stack buffers
	abase::vector<int>			m_lines;	// call stack lines

	CLuaDebug  *m_pLuaDebug;
};

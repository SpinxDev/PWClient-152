/************************************************************************
	created:	   2007-9-6   9:59
	filename: 	   Debugger.cpp
	author:		   baoshiming
	
	description:   CDebugger implement file  
/************************************************************************/
#include "Debugger.h"
#include "LuaScript.h"
#include "ScriptValue.h"

CDebugger::CDebugger(/*DEBUGGER_TYPE type*/)
{
	m_lua.SetDebugger(this);
	memset(m_szBuffName, 0, sizeof(m_szBuffName));
}

CDebugger::~CDebugger()
{
}

// ----- control  -----
BOOL CDebugger::Prepare()
{
	m_lua.PrepareDebugger();
	m_nMode = DMOD_NONE;
	m_nLine = 0;
	m_nLevel = 0;
	return TRUE;
}

// ----- debug mode -----
void CDebugger::Go()
{
	m_nMode = DMOD_NONE;
}

void CDebugger::Break()
{
	m_nMode = DMOD_BREAK;
}

void CDebugger::StepInto()
{
	m_nMode = DMOD_STEP_INTO;
}

void CDebugger::StepOver()
{
	m_nMode = DMOD_STEP_OVER;
	m_nLevel = 0;
}

void CDebugger::StepOut()
{
	m_nMode = DMOD_STEP_OUT;
	m_nLevel = 0;
}

void CDebugger::RunToCursor(const _TCHAR* szName, int nLine)
{
	m_nMode = DMOD_RUN_TO_CURSOR;
	m_nLine = nLine;
	if (szName) {
		memcpy(m_szBuffName, szName, 256*sizeof(_TCHAR));
	}
}

// ----- hooks -----
void CDebugger::LineHook(const char *szFile, int nLine) // zero-based nLine
{
#ifdef _UNICODE
	CScriptString temp;
	AWString _szFile;
	temp.SetAString(szFile);
	temp.RetrieveAWString(_szFile);
#else
#define _szFile szFile
#endif
	bool hasBkpt = false;
	CLuaScript *pScript = m_pLuaDebug->GetState()->GetScript(szFile);
	if (pScript) {
		hasBkpt = pScript->hasBreakPoint(nLine);
	}
	if ( hasBkpt || m_nMode==DMOD_STEP_INTO || 
		m_nMode==DMOD_BREAK ||
		(m_nMode==DMOD_STEP_OVER && m_nLevel<=0) || 
		(m_nMode==DMOD_STEP_OUT && m_nLevel<0) ||
		(m_nMode==DMOD_RUN_TO_CURSOR && !_tcsncmp(m_szBuffName, _szFile, _tcslen(_szFile)) && m_nLine == nLine) )
	{
		DebugBreak(_szFile, nLine);		
	}
}

void CDebugger::FunctionHook(const char *szFile, int nLine, BOOL bCall)
{
	m_nLevel += (bCall?1:-1);
}

void CDebugger::DebugBreak(const _TCHAR *szFile, int nLine)
{
	m_lua.DrawStackTrace();

	m_nStacklevel = 0;
	m_lua.DrawLocalVariables();

	m_pLuaDebug->SendLocalVariable(&m_LocalVarRoot);
	ClearLocalVariables();

	LDM_Break		ldmBreak;
	ldmBreak.lineNum = nLine;
	const _TCHAR *c_src = m_strStack.GetBuffer(0);
	ldmBreak.stackLen = _tcslen(c_src);
	ldmBreak.callStack = new _TCHAR[ldmBreak.stackLen];
	memcpy(ldmBreak.callStack, c_src, ldmBreak.stackLen*sizeof(_TCHAR));
	m_strStack.ReleaseBuffer();
	m_pLuaDebug->Break(szFile, &ldmBreak);
	
	m_strStack = _T("");

	m_pLuaDebug->Wait();
}
// ----- Stack Trace-----
void CDebugger::ClearStackTrace()
{
	m_nStacklevel = -1;
	m_lines.clear();
	m_buffs.clear();
}

int CDebugger::GetStackTraceLevel()
{
	return m_nStacklevel;
}

void CDebugger::AddStackTrace(const _TCHAR* szDesc, const _TCHAR* szFile, int nLine)
{
	m_strStack += szDesc;
	m_buffs.push_back(szFile);
	m_lines.push_back(nLine);
}

void CDebugger::GotoStackTraceLevel(int nLevel)
{
	m_nStacklevel = nLevel;

	// Inform debugger to go to file line m_files[nLevel] and m_lines[nLevel]
	LDM_ShowBufferLine showBufferLine;
	showBufferLine.line = m_lines[nLevel];
	memcpy(showBufferLine.name, m_buffs[nLevel], DEBUGGER_TOKEN_LEN*sizeof(_TCHAR));
	m_pLuaDebug->ShowBufferLine(&showBufferLine);

	StackLevelChanged();
}

// ----- variable functions -----
void CDebugger::Eval(ATString strCode)
{
	strCode = _T("return ") + strCode;
#ifdef _UNICODE
	CScriptString temp;
	AString astrCode;
	temp.SetAWString(strCode);
	temp.RetrieveAString(astrCode);
	m_lua.Eval(astrCode);
#else
	m_lua.Eval(strCode);
#endif
}

void CDebugger::ClearLocalVariables()
{
	m_LocalVarRoot.Clear();
}

LD_TreeNode* CDebugger::AddLocalVariable(LD_NameValue &nameValue)
{
	LD_TreeNode *node;
	m_LocalVarRoot.AddChild(nameValue, &node);
	return node;
}

LD_TreeNode* CDebugger::AddWatchVariable(LD_NameValue &nameValue)
{
	LD_TreeNode *node;
	m_WatchVarRoot.AddChild(nameValue, &node);
	return node;
}

void CDebugger::StackLevelChanged()
{
	m_lua.DrawLocalVariables();
}

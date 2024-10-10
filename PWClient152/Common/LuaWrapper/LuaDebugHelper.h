/************************************************************************
	created:	   2007-9-6   10:12
	filename: 	   LuaHelper.h
	author:		   baoshiming
	
	description:   interface for the CHelper class, which perform
				   immediate control of lua debug lib.
/************************************************************************/
#pragma once

#include "LD_NameValue.h"
#include "AString.h"
#include "alist2.h"

struct lua_State;
struct lua_Debug;
class CDebugger;

class CLuaDebugHelper  
{
public:
	CLuaDebugHelper();
	virtual ~CLuaDebugHelper();

	// ----- debugger functions -----
	BOOL PrepareDebugger();

	// ----- watches  -----
	void DrawStackTrace();
	void DrawLocalVariables();

	void CoverGlobals(lua_State *L);
	void RestoreGlobals(lua_State *L);
	void Describe(lua_State *L, int nIndex, const char *name);
	BOOL Eval(const char *szCode);

	void SetDebugger(CDebugger *pDebugger) { m_pDebugger = pDebugger; }
	CDebugger* GetDebugger() { return m_pDebugger; }
protected:
	static void hook (lua_State *L, lua_Debug *ar);
	static void line_hook (lua_State *L, lua_Debug *ar);
	static void func_hook (lua_State *L, lua_Debug *ar);

	int LuaAbsIndex(int nIndex, lua_State *L);
	void TableToChild(int nIndex, int nMaxTable, LD_TreeNode *pParent);

protected:
	CDebugger *m_pDebugger;

	lua_State* L;
};

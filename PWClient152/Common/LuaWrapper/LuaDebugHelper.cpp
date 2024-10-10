/************************************************************************
	created:	   2007-9-6   10:14
	filename: 	   LuaHelper.cpp
	author:		   baoshiming
	
	description:   implementation of the CHelper class.  
/************************************************************************/
#include "LuaDebugHelper.h"
#include "Debugger.h"
#include "LuaState.h"
#include "lua.hpp"
#include "LuaScript.h"
#include "LuaUtil.h"
#include "ScriptValue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLuaDebugHelper::CLuaDebugHelper()
{
	L = NULL;
}

CLuaDebugHelper::~CLuaDebugHelper()
{

}

BOOL CLuaDebugHelper::PrepareDebugger()
{
	ASSERT(L==NULL);

	L = m_pDebugger->GetLuaDebug()->GetState()->GetVM();

// 	lua_register(L, "_ERRORMESSAGE", LuaBind::ErrorCallback);

	lua_sethook(L, hook, LUA_MASKLINE|LUA_MASKCALL|LUA_MASKRET, 0);

	return TRUE;
}

BOOL CLuaDebugHelper::Eval(const char* szCode)
{
	CoverGlobals(L);

	int top = lua_gettop(L);	
	int status = luaL_loadbuffer(L, szCode, strlen(szCode), szCode);
	if ( status ) {
		LD_NameValue nameValue;
		sprintf_utf82w(nameValue.m_Name, MAX_PATH, szCode+strlen("return "));
		sprintf_utf82w(nameValue.m_Value, MAX_PATH, luaL_checkstring(L, -1));
		m_pDebugger->AddWatchVariable(nameValue);
	}
	else
	{
		status = lua_pcall(L, 0, LUA_MULTRET, 0);  /* call main */
		if ( status )
		{
			LD_NameValue nameValue;
			const char* szErr = luaL_checkstring(L, -1);
			const char* szErr2 = strstr(szErr, ": ");
			sprintf_utf82w(nameValue.m_Name, MAX_PATH, szCode+strlen("return "));
			sprintf_utf82w(nameValue.m_Value, MAX_PATH, szErr2?(szErr2+2):szErr);
			m_pDebugger->AddWatchVariable(nameValue);
		}
		else
			Describe(L, -1, szCode+strlen("return "));
	}

	lua_settop(L, top);

	RestoreGlobals(L);

	return !status;
}

void CLuaDebugHelper::Describe(lua_State *L, int nIndex, const char *name)
{
	int ntype = lua_type(L, nIndex);
	const char* type = lua_typename(L, ntype);
	LD_NameValue nameValue;
	sprintf_utf82w(nameValue.m_Name, MAX_PATH, name);
	sprintf_utf82w(nameValue.m_Type, MAX_PATH, type);
	AString astr;
	astr.Format("%p", lua_topointer(L, nIndex));
	sprintf_utf82w(nameValue.m_VarAddress, MAX_PATH, astr);
	bool bAlreadyAdd = false;
	switch(ntype)
	{
	case LUA_TNUMBER:
		astr.Format("%f", lua_tonumber(L, nIndex));
		break;
	case LUA_TSTRING:
		astr.Format("%s", lua_tostring(L, nIndex));
		break;
	case LUA_TBOOLEAN:
		astr.Format(lua_toboolean(L, nIndex) != 0 ? "true" : "false");
		break;
	case LUA_TNIL:
		astr.Format("nil");
		break;
	case LUA_TFUNCTION:
		if (lua_iscfunction(L, nIndex) != 0) {
			astr.Format("c function:%p", lua_tocfunction(L, nIndex));
		}
		else {
			astr.Format("lua function:%p", lua_topointer(L, nIndex));
		}
		break;
	case LUA_TTHREAD:
		astr.Format("%s:%p", type, lua_tothread(L, nIndex));
		break;
	case LUA_TUSERDATA:
	case LUA_TLIGHTUSERDATA:
		astr.Format("%s:%p", type, lua_touserdata(L, nIndex));
		break;
	case LUA_TTABLE:
		astr.Format("%p", lua_topointer(L, nIndex));	// set table's value to address
		if(!m_pDebugger->GetLocalRoot()->GetTreeNode(nameValue)) {
			LD_TreeNode * pNode = m_pDebugger->AddWatchVariable(nameValue);
			bAlreadyAdd = true;
			TableToChild(nIndex, 255, pNode);
		}
		else {
			astr.Format("%p", lua_topointer(L, nIndex));
		}
		break;
	default:
		astr.Format("%s:%p", type, lua_topointer(L, nIndex));
		break;
	}
	sprintf_utf82w(nameValue.m_Value, MAX_PATH, astr);
	if (!bAlreadyAdd) {
		m_pDebugger->AddWatchVariable(nameValue);
	}
}

void CLuaDebugHelper::CoverGlobals(lua_State *L)
{
	lua_newtable(L);  // save there globals covered by locals

	int nLevel = m_pDebugger->GetStackTraceLevel();
	lua_Debug ar;
	if ( lua_getstack (L, nLevel, &ar) )
	{
		int i = 1;
        const char *name;
        while ((name = lua_getlocal(L, &ar, i++)) != NULL) { /* SAVE lvalue */
			lua_pushstring(L, name);	/* SAVE lvalue name */						
			lua_pushvalue(L, -1);		/* SAVE lvalue name name */
			lua_pushvalue(L, -1);		/* SAVE lvalue name name name */
			lua_insert(L, -4);			/* SAVE name lvalue name name */
			lua_rawget(L, LUA_GLOBALSINDEX);	/* SAVE name lvalue name gvalue */

			lua_rawset(L, -5);					// save global value in local table
												/* SAVE name lvalue */

			lua_rawset(L, LUA_GLOBALSINDEX);	/* SAVE */
		}
	}
}

void CLuaDebugHelper::RestoreGlobals(lua_State *L)
{
	// there is table of covered globals on top

	lua_pushnil(L);				/* first key */
								/* SAVE nil */
	while (lua_next(L, -2))		/* SAVE key value */
	{
		lua_pushvalue(L, -2);		/* SAVE key value key */
		lua_insert(L, -2);			/* SAVE key key value */

		lua_rawset(L, LUA_GLOBALSINDEX);	// restore global
											/* SAVE key */
	}

	lua_pop(L, 1); // pop table of covered globals;
}

void CLuaDebugHelper::DrawLocalVariables()
{
//	LuaBind::DumpStack(L, __FILE__, __LINE__, "in");
	m_pDebugger->ClearLocalVariables();

	int nLevel = m_pDebugger->GetStackTraceLevel();
	lua_Debug ar;
	if ( lua_getstack (L, nLevel, &ar) ) {
		int i = 1;
		LD_NameValue nameValue;
        const char *name, *type;
		AString value, addr;
        while ((name = lua_getlocal(L, &ar, i++)) != NULL) {
			if (name[0] == '(') {
				lua_pop(L, 1);
				continue;
			}
			int ntype = lua_type(L, -1);
			type = lua_typename(L, ntype);
			addr.Format("%p", lua_topointer(L, -1));
			bool bAlreadyAdd = false;
			switch(ntype) {
				case LUA_TNUMBER: {
					value.Format("%f", lua_tonumber(L, -1));
					break;
				}
				case LUA_TSTRING: {
					value = lua_tostring(L, -1);
					break;
				}
				case LUA_TBOOLEAN: {
					value = lua_toboolean(L, -1) != 0 ? "true" : "false";
					break;
				}
				case LUA_TNIL: {
					value = "nil";
					break;
				}
				case LUA_TFUNCTION: {
					if (lua_iscfunction(L, -1) != 0) {
						value.Format("c function:%p", lua_tocfunction(L, -1));
					}
					else {
						value.Format("lua function:%p", lua_topointer(L, -1));
					}
					break;
				}
				case LUA_TTHREAD: {
					value.Format("%s:%p", type, lua_tothread(L, -1));
					break;
				}
				case LUA_TUSERDATA:
				case LUA_TLIGHTUSERDATA: {
					value.Format("%s:%p", type, lua_touserdata(L, -1));
					break;
				}
				case LUA_TTABLE: {
					sprintf_utf82w(nameValue.m_Name, MAX_PATH, name);
					sprintf_utf82w(nameValue.m_Type, MAX_PATH, type);
					sprintf_utf82w(nameValue.m_Value, MAX_PATH, addr);	// set table's value to address
					sprintf_utf82w(nameValue.m_VarAddress, MAX_PATH, addr);
					if(!m_pDebugger->GetLocalRoot()->GetTreeNode(nameValue)) {
						LD_TreeNode * pNode = m_pDebugger->AddLocalVariable(nameValue);
						bAlreadyAdd = true;
						TableToChild(-1, 255, pNode);
					}
					else {
						value.Format("%p", lua_topointer(L, -1));
					}
					break;
				}
				default: {
					value.Format("%s:%p", type, lua_topointer(L, -1));
					break;
				}
			}
			if (!bAlreadyAdd) {
//#ifdef _UNICODE
//				CScriptString temp;
//				AWString awstr;
//				temp.SetAString(name);
//				temp.RetrieveAWString(awstr);
//				_snwprintf(nameValue.m_Name, MAX_PATH, awstr);
//				temp.SetAString(type);
//				temp.RetrieveAWString(awstr);
//				_snwprintf(nameValue.m_Type, MAX_PATH, awstr);
//				temp.SetAString(value);
//				temp.RetrieveAWString(awstr);
//				_snwprintf(nameValue.m_Value, MAX_PATH, awstr);
//				temp.SetAString(addr);
//				temp.RetrieveAWString(awstr);
//				_snwprintf(nameValue.m_VarAddress, MAX_PATH, awstr);
//#else
				sprintf_utf82w(nameValue.m_Name, MAX_PATH, name);
				sprintf_utf82w(nameValue.m_Type, MAX_PATH, type);
				sprintf_utf82w(nameValue.m_Value, MAX_PATH, value);
				sprintf_utf82w(nameValue.m_VarAddress, MAX_PATH, addr);
//#endif
				m_pDebugger->AddLocalVariable(nameValue);
			}
			lua_pop(L, 1);  /* remove variable value */
        }
	}
	
	m_pDebugger->GetLocalRoot()->ResetTreeNode();

//	LuaBind::DumpStack(L, __FILE__, __LINE__, "out");
}

void CLuaDebugHelper::DrawStackTrace()
{
	m_pDebugger->ClearStackTrace();

	int nLevel = 0;
	lua_Debug ar;
	char szDesc[256];
	while ( lua_getstack (L, nLevel, &ar) )
	{
		lua_getinfo(L, "lnuS", &ar);

		szDesc[0] = '\0';
		if ( ar.name )
			strcat(szDesc, ar.name);
		strcat(szDesc, ",");
		if ( ar.namewhat )
			strcat(szDesc, ar.namewhat);
		strcat(szDesc, ",");
		if ( ar.what )
			strcat(szDesc, ar.what);
		strcat(szDesc, ",");
		if ( ar.short_src )
			strcat(szDesc, ar.short_src);
		strcat(szDesc, "\n");

#ifdef _UNICODE
		CScriptString temp;
		AWString awDesc;
		AWString awSrc;
		temp.SetAString(szDesc);
		temp.RetrieveAWString(awDesc);
		if (ar.source[0] == '@') {
			temp.SetAString(ar.source+1);
			temp.RetrieveAWString(awSrc);
			m_pDebugger->AddStackTrace(awDesc, awSrc, ar.currentline-1);	// 1-based to 0-based
		}
		else {
			temp.SetAString(ar.source);
			temp.RetrieveAWString(awSrc);
			m_pDebugger->AddStackTrace(awDesc, awSrc, ar.currentline-1);
		}
#else
		if (ar.source[0] == '@') {
			m_pDebugger->AddStackTrace(szDesc, ar.source+1, ar.currentline-1);	// 1-based to 0-based
		}
		else {
			m_pDebugger->AddStackTrace(szDesc, ar.source, ar.currentline-1);
		}
#endif

		++nLevel;
	};
}

int CLuaDebugHelper::LuaAbsIndex(int nIndex, lua_State *L) {
	if(nIndex == LUA_GLOBALSINDEX || nIndex == LUA_REGISTRYINDEX) {
		return nIndex;
	}

	if(nIndex < 0)
		return nIndex + lua_gettop(L) + 1;
	return nIndex;
}

void CLuaDebugHelper::TableToChild(int nIndex, int nMaxTable, LD_TreeNode *pParent) {
	ASSERT(pParent);
	LD_NameValue nameValue;

	int count = 0;

	nIndex = LuaAbsIndex(nIndex, L);

	lua_pushnil(L);
    while (lua_next(L, nIndex) != 0) {
		memset(&nameValue, 0, sizeof(nameValue));
		count++;
		if(count > nMaxTable) {
			sprintf_utf82w(nameValue.m_Name, MAX_PATH, "...");
			sprintf_utf82w(nameValue.m_Type, MAX_PATH, "...");
			sprintf_utf82w(nameValue.m_Value, MAX_PATH, "...");
			// leave m_ValueAddr NULL
			pParent->AddChild(nameValue);
			lua_pop(L, 2);
			break;
		}
		AString astr;
		int ntype = lua_type(L, -2);	// key
		switch(ntype) {		
			case LUA_TNUMBER: {
				astr.Format("[%.0f]", lua_tonumber(L, -2));
				break;
			}
			case LUA_TBOOLEAN: {
				astr.Format("%s", lua_toboolean(L, -2) != 0 ? "true" : "false");
				break;
			}
			case LUA_TSTRING: {
				astr.Format("\"%s\"", lua_tostring(L, -2));
				break;
			}
			case LUA_TFUNCTION: {
				if (lua_iscfunction(L, -2) != 0) {
					astr.Format("CFUNC:%p", lua_tocfunction(L, -2));
				}
				else {
					astr.Format("FUNC:%p", lua_topointer(L, -2));
				}
				break;
			}
			case LUA_TTABLE: {
				astr.Format("table:%p", lua_topointer(L, -2));
				break;
			}
			case LUA_TTHREAD: {
				astr.Format("%s:%p", lua_typename(L, ntype), lua_tothread(L, -2));
				break;
			}
			case LUA_TUSERDATA:
			case LUA_TLIGHTUSERDATA: {
				astr.Format("%s:%p", lua_typename(L, ntype), lua_touserdata(L, -2));
				break;
			}
			default:
				astr.Format("%s:%p", lua_typename(L, ntype), lua_topointer(L, -2));
				break;
		}
		sprintf_utf82w(nameValue.m_Name, MAX_PATH, astr);

		bool bAlreadyAdd = false;
		ntype = lua_type(L, -1);	// value
		const char *type = lua_typename(L, ntype);
		sprintf_utf82w(nameValue.m_Type, MAX_PATH, type);
		astr.Format("%p", lua_topointer(L, -1));
		sprintf_utf82w(nameValue.m_VarAddress, MAX_PATH, astr);
		switch(ntype) {
			case LUA_TNUMBER: {
				astr.Format("%f", lua_tonumber(L, -1));
				break;
			}
			case LUA_TBOOLEAN: {
				astr.Format(lua_toboolean(L, -1) != 0 ? "true" : "false");
				break;
			}
			case LUA_TSTRING: {
				astr.Format("\"%s\"", lua_tostring(L, -1));
				break;
			}
			case LUA_TFUNCTION: {
				if (lua_iscfunction(L, -1) != 0) {
					astr.Format("CFUNC:%p", lua_tocfunction(L, -1));
				}
				else {
					astr.Format("FUNC:%p", lua_topointer(L, -1));
				}
				break;
			}
			case LUA_TTHREAD: {
				astr.Format("thread:%p", lua_tothread(L, -1));
				break;
			}
			case LUA_TUSERDATA:
			case LUA_TLIGHTUSERDATA: {
				astr.Format("%s:%p", type, lua_touserdata(L, -1));
				break;
			}
			case LUA_TTABLE: {
				astr.Format("%p", lua_topointer(L, -1));
				sprintf_utf82w(nameValue.m_Value, MAX_PATH, astr);
				if(!pParent->m_pTreeRoot->GetTreeNode(nameValue)) {
					LD_TreeNode *newNode;
					pParent->AddChild(nameValue, &newNode);
					bAlreadyAdd = true;
					TableToChild(-1, nMaxTable, newNode);
				}
				else {
					sprintf_utf82w(nameValue.m_Value, MAX_PATH, astr);
				}
				break;
			}
			default: {
				astr.Format("%s:%p", type, lua_topointer(L, -1));
				break;
			}
		}
		if (!bAlreadyAdd) {
			sprintf_utf82w(nameValue.m_Value, MAX_PATH, astr);
			pParent->AddChild(nameValue);
		}
		lua_pop(L, 1);
	}
}

void CLuaDebugHelper::hook (lua_State *L, lua_Debug *ar)
{
	if(!g_LuaStateMan.GetState(L) || !g_LuaStateMan.GetState(L)->GetDebug()->IsInDebug())
		return;
 	switch(ar->event)
 	{
 		case LUA_HOOKTAILRET:
		case LUA_HOOKRET:
		{
 			func_hook(L,ar);
 			break;
		}
 		case LUA_HOOKCALL:
		{
			func_hook(L,ar);
 			break;
		}
 		case LUA_HOOKLINE:
		{
			line_hook(L,ar);
 			break;
		}
 	}
}

void CLuaDebugHelper::func_hook (lua_State *L, lua_Debug *ar)
{
	lua_getinfo(L, "lnuS", ar);
	
	const char* szSource = ar->source;
	if ( ar->source[0] == '@' )
 	{
 		szSource=ar->source+1;
 	}
//#ifdef _UNICODE
//	CScriptString temp;
//	AWString awstring;
//	temp.SetAString(szSource);
//	temp.RetrieveAWString(awstring);
//	g_LuaStateMan.GetState(L)->GetDebug()->GetDebugger()->		// can't get debugger from m_pDebugger
//		FunctionHook(awstring, ar->currentline, ar->event==LUA_HOOKCALL);
//#else
	g_LuaStateMan.GetState(L)->GetDebug()->GetDebugger()->		// can't get debugger from m_pDebugger
		FunctionHook(szSource, ar->currentline, ar->event==LUA_HOOKCALL);
//#endif
}

void CLuaDebugHelper::line_hook (lua_State *L, lua_Debug *ar)
{
	lua_getinfo(L, "lnuS", ar);
	
	const char* szSource = ar->source;
	if ( ar->source[0] == '@' )
 	{
 		szSource=ar->source+1;
 	}
//#ifdef _UNICODE
//	CScriptString temp;
//	AWString awstring;
//	temp.SetAString(szSource);
//	temp.RetrieveAWString(awstring);
//	g_LuaStateMan.GetState(L)->GetDebug()->GetDebugger()->	// can't get debugger from m_pDebugger
//			LineHook(awstring, ar->currentline-1);
//#else
	g_LuaStateMan.GetState(L)->GetDebug()->GetDebugger()->	// can't get debugger from m_pDebugger
			LineHook(szSource, ar->currentline-1);
//#endif
}

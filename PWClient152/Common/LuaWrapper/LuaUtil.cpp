/********************************************************************
  created:	   20/4/2006  
  filename:	   LuaUtil.cpp
  author:      Wangkuiwu  
  description: 
  Copyright (c) , All Rights Reserved.
*********************************************************************/

#include "LuaUtil.h"
#include "AFileImage.h"

#include "LuaDebug.h"
#include "luastate.h"
//#define _LUA_DEBUG

namespace LuaBind
{

//////////////////////////////////////////////////////////////////////////
//global data
LuaErrHandler    g_LuaErrHandler = NULL;

bool             g_LuaEnableLog  = false;

LuaErrHandler  RegisterLuaErrHandler(LuaErrHandler pHandler)
{
	LuaErrHandler old = g_LuaErrHandler;
	g_LuaErrHandler = pHandler;
	return old;
}

void EnableLog(bool bEnable)
{
	g_LuaEnableLog = bEnable;
}

void OutputDbgInfo(const char * str)
{
	OutputDebugStringA(str);
	if (g_LuaEnableLog)
	{
		a_LogOutput(1, str);
	}
}

void SetField(lua_State * L, const char * szTblName, const char * szField, TypeWrapper<void *>,  void * pValue)
{
	if (!GetTable(L, szTblName))
	{
		return;
	}	
	
	lua_pushstring(L, szField);
	lua_pushlightuserdata(L, pValue);
	lua_settable(L, -3);
	
	//pop the table
	lua_pop(L, 1);
}


void SetField(lua_State *L, const char * szField, TypeWrapper<const char *>, const char * pValue)
{
	lua_pushstring(L, szField);
	lua_pushstring(L, pValue);
	lua_settable(L, -3);
}

bool GetTable(lua_State *L, const char * szTblName)
{
	lua_getglobal(L, szTblName);
	if (lua_type(L, -1) != LUA_TTABLE)
	{
		AString msg;
		msg.Format("LuaGetTable fail to get %s as a table\n", szTblName);
		LUA_DEBUG_INFO(msg);
		lua_pop(L, 1); 
		return  false;
	}

	return true;
}

bool GetField(lua_State * L, const char * szTblName, const char * szField)
{
	if (!GetTable(L, szTblName))
	{
		return false;
	}
	return GetField(L, szField);
}

bool GetField(lua_State * L, const char * szField)
{
	lua_pushstring(L,szField);
	lua_gettable(L, -2);
	return true;
}


bool  RawGetField(lua_State * L, int index)
{
    lua_rawgeti(L, -1, index);
    return true;
}
bool RawGetField(lua_State * L, const char * szField)
{
    lua_pushstring(L, szField);
    lua_rawget(L, -2);
    return true;
}
bool Next(lua_State * L)
{
   return (lua_next(L, -2) != 0);
}

void DumpStack(lua_State * L, int index, const char * szSrcMsg)
{
	AString msg;
	int n = lua_gettop(L);
	
	int tp = lua_type(L, index);
	switch (tp)
	{
     case LUA_TNUMBER: 
		 if (szSrcMsg)
		 {
			 msg.Format("%s %d %s %f\n", szSrcMsg, index, lua_typename(L, tp), lua_tonumber(L, index));
		 }
		 else
		 {
			msg.Format("%d %s %f\n", index, lua_typename(L, tp), lua_tonumber(L, index));
		 }
		 LUA_DEBUG_INFO(msg);
		 break;
     case LUA_TTABLE:
		 {
		    if (szSrcMsg)
			{
				msg.Format("%s %d %s %p\n", szSrcMsg,index, lua_typename(L, tp), lua_topointer(L, index));	 
			}
			else
			{
				msg.Format("%d %s %p\n", index, lua_typename(L, tp), lua_topointer(L, index));
			}

			LUA_DEBUG_INFO(msg);
			
			AString sub_element;
			if (szSrcMsg)
			{
				sub_element.Format("\t%s", szSrcMsg);
			}
			else
			{
				sub_element.Format("\t");
			}

			lua_pushnil(L);
			while (lua_next(L, index) != 0)
			{
				AString str_key,str_val;
				str_key = sub_element + " val";
				DumpStack(L, -1, str_key);
				str_key = sub_element + " key";
				DumpStack(L, -2, str_key);
				lua_pop(L, 1);
			}
			break;
		 }
	 case LUA_TFUNCTION:	 
 		if (szSrcMsg)
		{
			 msg.Format("%s %d %s %p\n", szSrcMsg, index, lua_typename(L, tp), lua_topointer(L, index));
		}
		else
		{
			msg.Format("%d %s %p\n", index, lua_typename(L, tp), lua_topointer(L, index));
		}

		 LUA_DEBUG_INFO(msg);
		 break;
     case LUA_TSTRING: 
 		 if (szSrcMsg)
		 {
			msg.Format("%s %d %s %s\n", szSrcMsg, index, lua_typename(L, tp), lua_tostring(L, index));	 
		 }
		 else
		 {
			msg.Format("%d %s %s\n", index, lua_typename(L, tp), lua_tostring(L, index));
		 }

		 LUA_DEBUG_INFO(msg);
		 break;
     case LUA_TBOOLEAN:
		 
  	     if (szSrcMsg)
		 {
			 msg.Format("%s %d %s %d\n",  szSrcMsg, index, lua_typename(L, tp), lua_toboolean(L, index));
		 }
		 else
		 {
			 msg.Format("%d %s %d\n", index, lua_typename(L, tp), lua_toboolean(L, index));
		 }

		 LUA_DEBUG_INFO(msg);
		 break;
	 case LUA_TUSERDATA:
	 case LUA_TLIGHTUSERDATA:
		 
		 if (szSrcMsg)
		 {
			 msg.Format("%s %d %s %p\n", szSrcMsg, index, lua_typename(L, tp), lua_touserdata(L, index));
		 }
		 else
		 {
			msg.Format("%d %s %p\n", index, lua_typename(L, tp), lua_touserdata(L, index));	
		 }

		 LUA_DEBUG_INFO(msg);
		 break;
	 case LUA_TTHREAD:
		  if (szSrcMsg)
		  {
			  msg.Format("%s %d %s %p\n", szSrcMsg, index, lua_typename(L, tp), lua_tothread(L, index));
		  }
		  else
		  {
			  msg.Format("%d %s %p\n", index, lua_typename(L, tp), lua_tothread(L, index));
		  }

		 LUA_DEBUG_INFO(msg);
		 break;
	 case LUA_TNIL:
		 
 		 if (szSrcMsg)
		 {
			msg.Format("%s %d %s\n", szSrcMsg, index, lua_typename(L, tp));
		 }
		 else
		 {
			msg.Format("%d %s\n", index, lua_typename(L, tp));
		 }
		 LUA_DEBUG_INFO(msg);
		 break;
	 default: 
	    if (szSrcMsg)
		{
			msg.Format("%s %d %s\n", szSrcMsg, index, lua_typename(L, tp));	
		}
		else
		{
			msg.Format("%d %s\n", index, lua_typename(L, tp));
		}
		 LUA_DEBUG_INFO(msg);
		 break;
  }
	
  lua_settop(L, n);
	
}

void DumpStack(lua_State * L ,const char * szFile, int line,  const char * szSrcMsg, int count)
{
	AString msg;
	int n = lua_gettop(L);
	if (szSrcMsg)
	{
		msg.Format("======%s:%d %s stack %d print %d======\n",  szFile, line, szSrcMsg, n, count);
	}
	else
	{
		msg.Format("======%s:%d stack %d print %d======\n",  szFile, line,  n, count);
	}
	
	LUA_DEBUG_INFO(msg);
	int to;
	to = ( count > n  || count < 0) ? (0): (n - count);

	while(n > to)
	{
	  DumpStack(L, n, "");	
	  --n;	
	}
}





void ReportError(lua_State * L, const char * szSrc, const char * szBufName, int line, const char * szDesc)
{
	
	AString msg;
	
	if (L)
	{
	
		if (g_LuaStateMan.GetState(L) && g_LuaStateMan.GetState(L)->IsDebugEnabled()) 
		{
			g_LuaStateMan.GetState(L)->GetDebug()->ReportError(szBufName, line, szDesc);
		}

		msg.Format("%s, %s %s:%d:%s\n", szSrc, LUA_ERROR_HINT, szBufName, line, szDesc);
	}
	else
	{
		msg.Format("%s, error caught by c++\n", szSrc);
	}

	if (g_LuaErrHandler)
	{
		g_LuaErrHandler(msg);
	}
	LUA_DEBUG_INFO(msg);
}


bool Call(lua_State *L, const char* szTblName, const char * szMethod, int nargs, int nresults)
{
	int top = lua_gettop(L);

	bool bIsTableMethod = szTblName ? true : false;

	if (bIsTableMethod) 
	{
		// get a table method
		if (!GetField(L, szTblName, szMethod))
		{
			return false;
		}
		
		if (lua_type(L, -1) != LUA_TFUNCTION)
		{
			AString msg;
			msg.Format("******Call Function Is Nil:   %s:%s******\n", szTblName,szMethod);
			LUA_DEBUG_INFO(msg);
			lua_pop(L, 1); 
			return  false;
		}
	}
	else 
	{
		// get a global method
		lua_getglobal(L, szMethod);
		
		if (lua_type(L, -1) != LUA_TFUNCTION)
		{
			AString msg;
			msg.Format("******Call Function Is Nil:    %s******\n", szMethod);
			LUA_DEBUG_INFO(msg);
			lua_pop(L, 1); 
			return  false;
		}
	}

	if (bIsTableMethod) {
		//stack :  top                        bottom
		//            func  tbl  argn ... arg1
		//expect:     argn ... arg1  tbl  func
		lua_insert(L, -2);   // swap tbl and func,  
		if (nargs > 0)
		{
			lua_insert(L, -2-nargs );  
			lua_insert(L, -2-nargs );
		}
	}
	else {
		//stack :  top                        bottom
		//            func argn ... arg1
		//expect:     argn ... arg1 func
		if (nargs > 0)
		{
			lua_insert(L, -1-nargs );
		}
	}
	
	
	top = lua_gettop(L);
	lua_getglobal(L, "_ERRORMESSAGE");
	int where;	// where is the errfunc
	if (bIsTableMethod) {
		lua_insert(L, -3-nargs);			// argn ... arg1 tbl func errfunc
		where = -3 - nargs + lua_gettop(L) + 1;
		if (lua_pcall(L, nargs+1, nresults, -3-nargs))
		{
	// 		AString msg;
	// 		msg.Format("Call %s:%s", szTblName, szMethod);
	// 		LUA_DEBUG_INFO(msg);
			lua_settop(L, where-1);
			return false;
		}
	}
	else {
		lua_insert(L, -2-nargs);			// argn ... arg1 func errfunc
		where = -2 - nargs + lua_gettop(L) + 1;
		if (lua_pcall(L, nargs, nresults, -2-nargs))
		{
	// 		AString msg;
	// 		msg.Format("Call %s:%s", szTblName, szMethod);
	// 		LUA_DEBUG_INFO(msg);
			lua_settop(L, where-1);
			return false;
		}
	}

	// remove error function
	lua_remove(L, where);

	return true;
	
}

bool FromLuaError(const char * err, int *pLine, char *szInfo)
{
	const char * p0;
	p0 = err;

	//expect [
	while (*p0 != '\0')
	{
		if (*p0 == '[')
		{
			break;
		}
		p0++;
	}
	if (*p0 == '\0')
	{
		return false;
	}

	//expect left "
	while (*p0 != '\0')
	{
		if (*p0 == '\"')
		{
			break;
		}
		p0++;
	}
	if (*p0 == '\0')
	{
		return false;
	}

	p0++; //skip left "

	//expect right "
	while (*p0 != '\0')
	{
		if (*p0 == '\"')
		{
			break;
		}
		p0++;
	}

	if (*p0 == '\0')
	{
		return false;
	}
	
	//expect :
	char line_str[DEBUGGER_TOKEN_LEN];
	while (*p0 != '\0')
	{
		if (*p0 == ':')
		{
			break;
		}
		p0++;
	}
	if (*p0 == '\0')
	{
		return false;
	}
	p0++;  //skip :

	//expect next :
	char *p1 = line_str;
	while (*p0 != '\0')
	{
		if (*p0 == ':')
		{
			break;
		}
		*p1 = *p0;
		p0++;
		p1++;
	}
	*p1 = '\0';
	if (*p0 == '\0')
	{
		return false;
	}
	*pLine = atoi(line_str);
	p0++;  //skip :

	//fill error info
	p1 = szInfo;
	while (*p0 != '\0')
	{
		*p1 = *p0;
		p0++;
		p1++;
	}
	*p1 = '\0';
	
	return true;
}

bool DoFile(lua_State * L, const char* szScript)
{
	int top = lua_gettop(L);
	lua_getglobal(L, "_ERRORMESSAGE");
	if (luaL_loadfile(L, szScript))	//compile error
	{
		char szInfo[DEBUGGER_INFO_LEN];
		int line;
		const char *err = lua_tostring(L, -1);
		FromLuaError(err, &line, szInfo);
		ReportError(L, "DoBuffer compiling", szScript, line, szInfo);
		lua_settop(L, top);		// pop error msg and error function
	}
	if(lua_pcall(L, 0, LUA_MULTRET, -2))		//run-time error
	{
		lua_settop(L, top);
// 		AString msg;
// 		msg.Format("load and run script %s", szScript);
// 		LUA_DEBUG_INFO(msg);
		return false;
	}
	
	// remove error function
	lua_remove(L, top+1);

	return true;
}


bool DoFileImage(lua_State * L,  const char* szScript)
{
	int top = lua_gettop(L);
	lua_getglobal(L, "_ERRORMESSAGE");
	if (LoadFileImage(L, szScript))	//compile error
	{
		char szInfo[DEBUGGER_INFO_LEN];
		int line;
		const char *err = lua_tostring(L, -1);
		FromLuaError(err, &line, szInfo);
		ReportError(L, "DoBuffer compiling", szScript, line, szInfo);
		lua_settop(L, top);		// pop error msg and error function
	}
	if(lua_pcall(L, 0, LUA_MULTRET, -2))		//run-time error
	{
		lua_settop(L, top);
// 		AString msg;
// 		msg.Format("load and run script %s", szScript);
// 		LUA_DEBUG_INFO(msg);
		return false;
	}
	
	// remove error function
	lua_remove(L, top+1);

	return true;
}

int LoadFileImage(lua_State * L, const char * szScript)
{
	AFileImage  fScript;

	if (!fScript.Open(szScript,  AFILE_OPENEXIST | AFILE_TYPE_BINARY | AFILE_TEMPMEMORY))
	{
		AString msg;
		msg.Format("LoadFileImage: can not open script %s\n", szScript);
		LUA_DEBUG_INFO(msg);
		return -1;  //error
	}

	int fSize;
	const char * pBuf;
	fSize  = fScript.GetFileLength();
	pBuf   = (const char *) fScript.GetFileBuffer();

	return luaL_loadbuffer(L, pBuf, fSize, szScript);
}

bool DoBuffer(lua_State * L, const char * pBuf, int nBuf, const char * name)
{
	int top = lua_gettop(L);
	lua_getglobal(L, "_ERRORMESSAGE");
	if (luaL_loadbuffer(L, pBuf, nBuf, name))	//compile error
	{
		char szInfo[DEBUGGER_INFO_LEN];
		int line;
		const char *err = lua_tostring(L, -1);
		FromLuaError(err, &line, szInfo);
		ReportError(L, "DoBuffer compiling", name, line, szInfo);
		lua_settop(L, top);		// pop error msg and error function
		return false;
	}
	if(lua_pcall(L, 0, LUA_MULTRET, -2))		//run-time error
	{
		lua_settop(L, top);
// 		AString msg;
// 		msg.Format("DoBuffer %s", name);
// 		LUA_DEBUG_INFO(msg);
		return false;
	}
	
	// remove error function
	lua_remove(L, top+1);
	
	return true;
}

int  ErrorCallback(lua_State * L)
{
	lua_Debug ar;
	if (!lua_isstring(L, -1))
		return lua_gettop(L);
	AString errMsg = lua_tostring(L, -1);

//	DumpStack(L, __FILE__, __LINE__, "errorcallback", -1);

	if(lua_getstack(L, 1, &ar)) {
		lua_getinfo(L, "Snl", &ar);
		AString msg;
		msg.Format("Error:");
		ReportError(L, msg, ar.source, ar.currentline, errMsg);
		lua_pop(L, 1);	// pop original message
	}
	return 0;
}

void ReleaseTbl(lua_State * L,  const char *szTblName)
{
	//release the lua table
	lua_pushnil(L);
	lua_setglobal(L, szTblName);
	
}



int  luaL_tostringW(lua_State * L, int index, AWString& wstr)
{
	const char * szUtf8 = lua_tostring(L, index);

	int len =MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, NULL, 0);
	wchar_t * szBuf =wstr.GetBuffer(len);
	MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, szBuf, len);
	wstr.ReleaseBuffer(-1);
	return len;
}



bool CheckValue(TypeWrapper<const char *>, lua_State * L, int index, AString& val, const char * szFile , int iLine )
{
	if (!Match(L, index, LUA_TSTRING))
	{
		AString msg;
		if (szFile)
		{
			msg.Format("%s:%d expected %s got %s", szFile, iLine, lua_typename(L, LUA_TSTRING), lua_typename(L, lua_type(L, index)));
		}
		else
		{
			msg.Format("expected %s got %s", lua_typename(L, LUA_TSTRING), lua_typename(L, lua_type(L, index)));
		}
		
		
		LUA_DEBUG_INFO(msg);

		//want an assert???
		//assert(0 && "type mismatch!");

		return false;
	}

	val = lua_tostring(L, index);
	return true;
}

bool CheckValue(TypeWrapper<int>, lua_State * L, int index, int& val, const char * szFile, int iLine )
{
	
	if (!Match(L, index, LUA_TNUMBER))
	{
		AString msg;

		if (szFile)
		{
			msg.Format("%s:%d expected %s got %s", szFile, iLine, lua_typename(L, LUA_TNUMBER), lua_typename(L, lua_type(L, index)));
		}
		else
		{
			msg.Format("expected %s got %s", lua_typename(L, LUA_TNUMBER), lua_typename(L, lua_type(L, index)));
		}
		
		LUA_DEBUG_INFO(msg);

		//want an assert???
		//assert(0 && "type mismatch!");
		return false;
	}
	val = lua_tointeger(L, index);
	return true;
}

bool CheckValue(TypeWrapper<const wchar_t *>, lua_State * L, int index, AWString& val, const char * szFile, int iLine )
{
	if (!Match(L, index, LUA_TSTRING))
	{
		AString msg;
		if (szFile)
		{
			msg.Format("%s:%d expected %s got %s", szFile, iLine, lua_typename(L, LUA_TSTRING), lua_typename(L, lua_type(L, index)));
		}
		else
		{
			msg.Format("expected %s got %s", lua_typename(L, LUA_TSTRING), lua_typename(L, lua_type(L, index)));
		}
		
		LUA_DEBUG_INFO(msg);

		//want an assert???
		//assert(0 && "type mismatch!");

		return false;
	}
	luaL_tostringW(L, index, val);
	return true;
}

bool CheckValue(TypeWrapper<bool>, lua_State * L, int index, bool& val, const char * szFile, int iLine )
{
	if (!Match(L, index, LUA_TBOOLEAN))
	{
		AString msg;
		if (szFile)
		{
			msg.Format("%s:%d expected %s got %s", szFile, iLine, lua_typename(L, LUA_TBOOLEAN), lua_typename(L, lua_type(L, index)));
		}
		else
		{
			msg.Format("expected %s got %s", lua_typename(L, LUA_TBOOLEAN), lua_typename(L, lua_type(L, index)));
		}
		LUA_DEBUG_INFO(msg);
		//want an assert???
		//assert(0 && "type mismatch!");
		return false;
	}
	val = (lua_toboolean(L, index) == 1);
	return true;
}

bool CheckValue(TypeWrapper<double>, lua_State * L, int index, double& val, const char * szFile, int iLine )
{
	if (!Match(L, index, LUA_TNUMBER))
	{
		AString msg;
		if (szFile)
		{
			msg.Format("%s:%d expected %s got %s", szFile, iLine, lua_typename(L, LUA_TNUMBER), lua_typename(L, lua_type(L, index)));
		}
		else
		{
			msg.Format("expected %s got %s", lua_typename(L, LUA_TNUMBER), lua_typename(L, lua_type(L, index)));
		}
		LUA_DEBUG_INFO(msg);
		//want an assert???
		//assert(0 && "type mismatch!");
		return false;
	}
	val = lua_tonumber(L, index);
	return true;
}


bool CheckValue(TypeWrapper<LuaUserData>, lua_State * L, int index, void *&val, const char * szFile, int iLine )
{
	if (!Match(L, index, LUA_TUSERDATA) && !Match(L, index, LUA_TLIGHTUSERDATA))
	{
		AString msg;
		if (szFile)
		{
			msg.Format("%s:%d expected userdata got %s", szFile, iLine, lua_typename(L, lua_type(L, index)));
		}
		else
		{
			msg.Format("expected userdata got %s", lua_typename(L, lua_type(L, index)));
		}
		LUA_DEBUG_INFO(msg);
		//want an assert???
		//assert(0 && "type mismatch!");
		return false;
	}
	val = lua_touserdata(L, index);
	return true;
}




/////////end package
}
/********************************************************************
  created:	   20/4/2006  
  filename:	   LuaUtil.h
  author:      Wangkuiwu  
  description: 
  Copyright (c) , All Rights Reserved.
*********************************************************************/
#pragma  once

#include <lua.hpp>
#include <windows.h>
#include "ALog.h"

class AString;
class AWString;

namespace LuaBind
{

#define REG_API(s) {#s, s}

// #ifdef _DEBUG
// 	#define	 LUA_DEBUG_INFO(s)	 OutputDebugStringA(s);\
// 								 a_LogOutput(1,s)
// #else
// 	#define	 LUA_DEBUG_INFO(s)	 a_LogOutput(1, s)
// #endif

#define	 LUA_DEBUG_INFO(s)	    LuaBind::OutputDbgInfo(s)

#define  SET_GLOBAL_CONST(L, c)   lua_pushinteger(L, c);\
							lua_setglobal(L, #c);

#define   LUA_ERROR_HINT		"From Lua:"

	struct LuaLightUserData
	{
		LuaLightUserData(const void* value) :
			m_value(value)
		{
		}

		const void* m_value;
	};


	struct LuaUserData
	{
		LuaUserData(const void* value) :
			m_value(value)
		{
		}

		const void* m_value;
	};

	struct LuaNil
	{
	};
	
	struct LuaFunc
	{
	};
	
	struct LuaTable
	{
	};
	

	template<class T> struct TypeWrapper {};

	bool CheckValue(TypeWrapper<int>, lua_State * L, int index, int& val, const char * szFile = NULL, int iLine = 0 );
	bool CheckValue(TypeWrapper<const char *>, lua_State * L, int index, AString& val, const char * szFile = NULL, int iLine = 0);
	bool CheckValue(TypeWrapper<const wchar_t *>, lua_State * L, int index, AWString& val, const char * szFile  = NULL, int iLine  = 0 );
	bool CheckValue(TypeWrapper<bool>, lua_State * L, int index, bool& val, const char* szFile = NULL, int iLine = 0);
	bool CheckValue(TypeWrapper<double>, lua_State * L, int index, double& val, const char* szFile = NULL, int iLine = 0);
	bool CheckValue(TypeWrapper<LuaUserData>, lua_State * L, int index, void *&val, const char* szFile = NULL, int iLine = 0);
	

	inline  bool Match(lua_State * L, int index, int type)
	{
		return (lua_type(L, index) == type);
	}
	

	inline void Push(TypeWrapper<LuaLightUserData>, lua_State* L, const void * value)
		{  lua_pushlightuserdata(L, (void*)value);  }
	
	inline void Push(TypeWrapper<const char *>, lua_State* L, const char * value)
		{  lua_pushstring(L, value);  }

	inline void Push(TypeWrapper<int>, lua_State* L, int value)
		{  lua_pushnumber(L, value);  }
	inline void Push(TypeWrapper<double>, lua_State* L, double value)
		{  lua_pushnumber(L, value);  }

	inline void Push(TypeWrapper<bool>, lua_State* L, bool value)
		{  lua_pushboolean(L, value);  }

	inline void Push(TypeWrapper<LuaNil>, lua_State* L)
		{  lua_pushnil(L);  }
	inline void Push(TypeWrapper<__int64>, lua_State * L, __int64 value)
		{  lua_pushnumber(L, (lua_Number)value); 	}

	
	inline void Pop(lua_State * L, int n)
		{ lua_pop(L, n); }
/**
 * \brief get a table from luavm. 
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning  
			the stack is NOT balance. If success, leave the 
			table on the top of the stack.
 * \todo   
 * \author kuiwu 
 * \date 20/4/2006
 * \see 
 */
bool GetTable(lua_State * L,  const char * szTblName);


/**
 * \brief get a field from lua class
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning  
			the stack is NOT balance. If success, leave the 
			table and field on the top of the stack.
 * \todo   
 * \author kuiwu 
 * \date 20/4/2006
 * \see 
 */
bool GetField(lua_State * L,  const char * szTblName, const char * szField);
/**
 * \brief get a field from lua class
 * \param[in]
 * \param[out]
 * \return
 * \note	the caller must make sure the table on the top of stack.
 * \warning 
			the stack is NOT balance. If success, leave the 
			field on the top of the stack.
 * \todo   
 * \author kuiwu 
 * \date 10/5/2006
 * \see 
 */
bool GetField(lua_State * L, const char * szField);
/**
 * \brief rawget(not invoke metamethod) a field from lua array
 * \param[in] 
 * \param[out]
 * \return
 * \note	the caller must make sure the table on the top of stack.
 * \warning 
			the stack is NOT balance. If success, leave the 
			field on the top of the stack.
 * \todo   
 * \author kuiwu 
 * \date 13/7/2006
 * \see 
 */
bool RawGetField(lua_State * L,  int index);
/**
 * \brief rawget(not invoke metamethod) a field from lua table
 * \param[in] 
 * \param[out]
 * \return
 * \note	the caller must make sure the table on the top of stack.
 * \warning 
			the stack is NOT balance. If success, leave the 
			field on the top of the stack.
 * \todo   
 * \author kuiwu 
 * \date 13/7/2006
 * \see  GetField
 */
bool RawGetField(lua_State * L, const char * szField);

/**
 * \brief iterate a table
 * \param[in] 
 * \param[out]
 * \return      true if success, false if no more elements in the table
 * \note	the caller must make sure the table and last key on the top of stack.
 * \warning 
			the stack is NOT balance. If success, leave the 
			key and value on the top of the stack.
 * \todo   
 * \author kuiwu 
 * \date 13/7/2006
 * \see  GetField
 */
bool Next(lua_State * L);
void SetField(lua_State * L, const char * szTblName, const char * szField, TypeWrapper<void *>,  void * pValue);

/**
 * \brief set a  field, assume the table is on the top of stack.
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 16/5/2006
 * \see 
 */
void SetField(lua_State * L,  const char * szField, TypeWrapper<const char *>,  const char * pValue);


void DumpStack(lua_State * L, const char * szFile, int line,  const char * szSrcMsg = NULL, int count = -1);
void DumpStack(lua_State * L, int index, const char * szSrcMsg /* = NULL */);
/**
 * \brief report error 
 * \param[in] L, lua vm,  null if the error msg is szSrc, 
				 otherwise assume error message is on the top of stack..
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 20/4/2006
 * \see 
 */
void ReportError(lua_State * L, const char * szSrc, char * bufName, int line, const char * desc);

bool Call(lua_State * L, const char* szTblName, const char * szMethod, int nargs = 0, int nresults = 0);

bool  DoFile(lua_State * L, const char* szScript);

bool  DoFileImage(lua_State * L, const char* szScript);

int   LoadFileImage(lua_State * L, const char * szScript);

bool  DoBuffer(lua_State * L, const char * pBuf, int nBuf, const char * name);

int   ErrorCallback(lua_State * L);	// Registered by LuaDebugHelper

/**
 * \brief release the given  table (global)
 * \param[in] szTblName: table name, caller must make sure the name is correct.
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 20/4/2006
 * \see 
 */
void ReleaseTbl(lua_State * L, const char *szTblName);

inline bool luaL_checkbool(lua_State * L, int arg)
{
	luaL_checktype(L, arg, LUA_TBOOLEAN);
	return (lua_toboolean(L, arg) != 0);  //disable C4800
}

inline void * luaL_checklightudata(lua_State *L, int arg)
{
	luaL_checktype(L, arg, LUA_TLIGHTUSERDATA);
	return lua_touserdata(L, arg);
}



int   luaL_tostringW(lua_State * L, int index, AWString& wstr);

inline	int   wc2utf8(const wchar_t * szSrc, char * szDest, int nSize)
{
	return WideCharToMultiByte(CP_UTF8, 0, szSrc, -1, szDest, nSize, NULL, NULL);
}

typedef  void (*LuaErrHandler) (const char * szMsg);
LuaErrHandler  RegisterLuaErrHandler(LuaErrHandler pHandler);

void      EnableLog(bool bEnable);
void      OutputDbgInfo(const char * str);

}
/********************************************************************
	created:	2006/09/07
	author:		kuiwu
	
	purpose:	the value used to communicate with script(eg. lua)
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/

#include <assert.h>
#include <AWString.h>
#include <AString.h>
#include <string.h>
#include <windows.h>
#include "ScriptValue.h"

#include "LuaUtil.h"
#include "LuaState.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptString::CScriptString()
:m_pBuf(NULL), m_nLen(0)
{
}


CScriptString::CScriptString(const CScriptString& str)
{
	if (str.m_pBuf == NULL)
	{
		m_pBuf = NULL;
		m_nLen = 0;
	}
	else
	{
		m_nLen = str.m_nLen;
		m_pBuf = new char[m_nLen+1];
		memcpy(m_pBuf, str.m_pBuf, m_nLen+1);
	}

}

CScriptString& CScriptString::operator=(const CScriptString& str)
{
	if (this == &str)
	{
		return *this;
	}

	Release();

	if (str.m_pBuf != NULL)
	{
		m_nLen = str.m_nLen;
		m_pBuf = new char[m_nLen+1];
		memcpy(m_pBuf, str.m_pBuf, m_nLen+1);
	}


	return *this;
	
}

CScriptString& CScriptString::operator=(const char * str)
{
	
	if (m_pBuf == str)
	{
		return *this;
	}
	Release();
	if (str != NULL)
	{
		m_nLen = strlen(str);
		m_pBuf = new char[m_nLen+1];
		strcpy(m_pBuf, str);
	}
	

	return *this;
	
}

void CScriptString::SetUtf8(const char * pBuf, int nLen)
{
	Release();
	assert(pBuf != NULL && nLen >=0);
	m_nLen = nLen;	
	m_pBuf = new char[m_nLen+1];
	
	memcpy(m_pBuf, pBuf, m_nLen);
	m_pBuf[m_nLen] = 0;
}

void CScriptString::SetAString(const char * str)
{
	assert(str);
	AStringToUtf8(str);
}

void CScriptString::SetAWString(const wchar_t * str)
{
	assert(str);
	AWStringToUtf8(str);
}

void CScriptString::RetrieveAString(AString& out) const
{
	Utf8ToAString(out);
}
void CScriptString::RetrieveAWString(AWString& out) const
{
	Utf8ToAWString(out);
}

void CScriptString::AWStringToUtf8(const AWString& in)
{
	Release();
	m_nLen = WideCharToMultiByte(CP_UTF8, 0, in, in.GetLength(), NULL, 0, NULL, NULL);
	m_pBuf = new  char[m_nLen+1];
	WideCharToMultiByte(CP_UTF8, 0, in, -1, m_pBuf, m_nLen+1, NULL, NULL);
	m_pBuf[m_nLen] = '\0';
}

void CScriptString::Utf8ToAWString(AWString& out) const
{
	out.Empty();
	int len =MultiByteToWideChar(CP_UTF8, 0, m_pBuf, m_nLen, NULL, 0);
	wchar_t * szBuf =out.GetBuffer(len+1);
	MultiByteToWideChar(CP_UTF8, 0, m_pBuf, -1, szBuf, len);
	szBuf[len] = L'\0';
	out.ReleaseBuffer();
}

void CScriptString::AStringToUtf8(const AString& in)
{
	AWString bridge;
	int codePage = g_LuaStateMan.GetCodePage();
	int len = MultiByteToWideChar(codePage, 0, in, in.GetLength(), NULL, 0);
	wchar_t * szBuf = bridge.GetBuffer(len+1);
	MultiByteToWideChar(codePage, 0, in, -1, szBuf, len);
	szBuf[len] = L'\0';
	bridge.ReleaseBuffer(-1);
	AWStringToUtf8(bridge);
}
void CScriptString::Utf8ToAString(AString& out) const
{
	out.Empty();
	AWString bridge;
	int codePage = g_LuaStateMan.GetCodePage();
	Utf8ToAWString(bridge);
	int len = WideCharToMultiByte(codePage, 0, bridge, bridge.GetLength(), NULL, 0, NULL, NULL);
	char * szBuf = out.GetBuffer(len+1);
	WideCharToMultiByte(codePage, 0, bridge, -1, szBuf, len, NULL, NULL);
	szBuf[len] = '\0';
	out.ReleaseBuffer();
}

CScriptString::~CScriptString()
{
	Release();
}



void CScriptString::Release()
{
	if (m_pBuf)
	{
		delete[] m_pBuf;
		m_pBuf = NULL;
	}
	m_nLen = 0;
}



CScriptValue::CScriptValue()
:m_Type(SVT_INVALID)
{

}

CScriptValue::~CScriptValue()
{

}

CScriptValue::CScriptValue(double val)
{
	m_Type = SVT_NUMBER;
	m_Number = val;
}
CScriptValue::CScriptValue(bool val)
{
	m_Type = SVT_BOOL;
	m_Bool =  val;
}
CScriptValue::CScriptValue(void * val)
{
	m_Type = SVT_USERDATA;
	m_UserData = val;
}

CScriptValue::CScriptValue(_64BITS_ID val)
{
	m_Type = SVT_NUMBER;
	m_64BitsId = val.val;
}

void CScriptValue::Set64BitsId(__int64 val)
{
	m_Type = SVT_NUMBER;
	m_64BitsId = val;
}

__int64 CScriptValue::Get64BitsId() const
{
	assert(m_Type == SVT_NUMBER);
	return m_64BitsId;
}

CScriptValue::CScriptValue(const CScriptString& val)
:m_String(val)
{
	m_Type = SVT_STRING;
}

CScriptValue::CScriptValue(const CScriptValue& val)
:m_String(val.m_String), m_ArrVal(val.m_ArrVal), m_ArrKey(val.m_ArrKey),
m_Type(val.m_Type)
{
	switch(val.m_Type)
	{
	case SVT_BOOL:
		m_Bool = val.m_Bool;
		break;
	case SVT_NUMBER:
		m_Number = val.m_Number;
		break;
	case SVT_USERDATA:
		m_UserData = val.m_UserData;
		break;
	default:
		break;
	}
}


namespace LuaBind
{


bool GetScriptValue(lua_State* L, int index, CScriptValue& val)
{
	
	int tp = lua_type(L, index);
	switch (tp)
	{
		case LUA_TNUMBER:
			{
				val.m_Type   = CScriptValue::SVT_NUMBER;
				val.m_Number = lua_tonumber(L, index);
			}	
			break;
		case LUA_TBOOLEAN:
			{
				val.m_Type   = CScriptValue::SVT_BOOL;
				val.m_Bool = (lua_toboolean(L, index) == 1);
			}	
			break;
		case LUA_TUSERDATA:
		case LUA_TLIGHTUSERDATA:
			{
				val.m_Type = CScriptValue::SVT_USERDATA;
				val.m_UserData = lua_touserdata(L, index);
			}	
			break;
		case LUA_TSTRING:
			{
				val.m_Type = CScriptValue::SVT_STRING;
				//val.m_String = lua_tostring(L, index);
				unsigned int len;
				const char * szTxt = lua_tolstring(L, index, &len);
				val.m_String.SetUtf8(szTxt, len/*+1*/);
			}	
			break;
		case LUA_TTABLE:
			{
				val.m_Type = CScriptValue::SVT_ARRAY;
				val.m_ArrVal.clear();
				val.m_ArrKey.clear();
				//int n = lua_gettop(L); //save the stack pos	
				lua_pushnil(L);
				while (lua_next(L, index) != 0)
				{
					CScriptValue subVal;
					//note: not use negative index
					//if (GetScriptValue(L, -1, subVal))
					bool bVal = GetScriptValue(L, lua_gettop(L), subVal);
					if (bVal)
					{
						val.m_ArrVal.push_back(subVal);
						if (!GetScriptValue(L, lua_gettop(L)-1, subVal))
						{
							assert(0);
							LUA_DEBUG_INFO("error get key!\n");
							subVal.m_Type = CScriptValue::SVT_INVALID;
						}
						val.m_ArrKey.push_back(subVal);
					}
					lua_pop(L, 1);
				}

				//lua_settop(L, n);   //restore the stack pos
			}	
			break;
		default :
			{
				//char msg[200];
				//sprintf()
				//OutputDebugStringA()
			}
			//useless type
			return false;
	}	
	
	return true;
}

void GetStack(lua_State * L, abase::vector<CScriptValue>& values, int start)
{
	values.clear();

	int n = lua_gettop(L);

	for (int i = start; i <= n; ++i)
	{
		CScriptValue val;
		if (GetScriptValue(L, i, val))
		{
			values.push_back(val);
		}
	}
}


void DumpValue(const CScriptValue& val)
{
	AString  msg;
	switch(val.m_Type)
	{
	case CScriptValue::SVT_BOOL:
		msg.Format("bool %d\n", val.m_Bool);
		LUA_DEBUG_INFO(msg);
		break;
	case CScriptValue::SVT_NUMBER:
		msg.Format("number %f\n", val.m_Number);
		LUA_DEBUG_INFO(msg);
		break;
	case CScriptValue::SVT_USERDATA:
		msg.Format("userdata %p\n", (void *)val.m_UserData);
		LUA_DEBUG_INFO(msg);
		break;
	case CScriptValue::SVT_STRING:
		val.m_String.RetrieveAString(msg);
		LUA_DEBUG_INFO(msg);
		break;
	case CScriptValue::SVT_ARRAY:
		msg.Format("array(table) %d...\n", val.m_ArrVal.size());
		LUA_DEBUG_INFO(msg);
		{
			for (int i = 0; i < (int)val.m_ArrVal.size(); ++i)
			{
				DumpValue(val.m_ArrKey[i]);	
				DumpValue(val.m_ArrVal[i]);
			}
		}
		break;
	default:
		break;
	}

}

void SetScriptValue(lua_State *L, const CScriptValue& val)
{
	switch(val.m_Type)
	{
	case CScriptValue::SVT_BOOL:
		lua_pushboolean(L, val.m_Bool);
		break;
	case CScriptValue::SVT_NUMBER:
		lua_pushnumber(L, val.m_Number);
	    break;
	case CScriptValue::SVT_USERDATA:
		lua_pushlightuserdata(L, (void *)val.m_UserData);
	    break;
	case CScriptValue::SVT_STRING: 
		lua_pushstring(L, val.m_String.GetUtf8());
		break;
	case CScriptValue::SVT_ARRAY:
		{
			//check empty table
			//assert(!val.m_ArrVal.empty());

			lua_newtable(L);
			unsigned int i;
			if (val.m_ArrKey.empty())
			{
				for ( i= 0; i < val.m_ArrVal.size(); i++)
				{
					SetScriptValue(L, val.m_ArrVal[i]);
					lua_rawseti(L, -2, i+1); //lua start from 1
				}
			}
			else
			{
				for (i = 0; i < val.m_ArrVal.size(); i++)
				{
					SetScriptValue(L, val.m_ArrKey[i]);
					SetScriptValue(L, val.m_ArrVal[i]);
					lua_rawset(L, -3);
				}
			}

		
		}
	    break;
	default:
		//unknown type
		assert(0);
	    break;
	}
}

void SetStack(lua_State * L, const abase::vector<CScriptValue>& values)
{
	
	for (int i= 0; i< (int)values.size(); ++i)
	{
		SetScriptValue(L, values[i]);
	}
}

//	Return argument number
// int SetStack(lua_State* L, const char* szFormat, va_list arglist)
// {
// 	int iFormatLen = szFormat ? strlen(szFormat) : 0;
// 	int iNumArg = 0;
// 
// 	for (int i=0; i < iFormatLen; i++)
// 	{
// 		BYTE ch = szFormat[i];
// 
// 		switch (ch)
// 		{
// 		case 'b':
// 
// 			iNumArg++;
// 			lua_pushboolean(L, va_arg(arglist, int));
// 			break;
// 
// 		case 'd':
// 
// 			iNumArg++;
// 			lua_pushnumber(L, (double)va_arg(arglist, int));
// 			break;
// 
// 		case 'f':
// 	
// 			iNumArg++;
// 			lua_pushnumber(L, va_arg(arglist, double));
// 			break;
// 	
// 		case 's':
// 		{
// 			iNumArg++;
// 
// 		#ifdef UNICODE
// 			CScriptString str;
// 			str.SetAWString(va_arg(arglist, const wchar_t*));
// 			lua_pushstring(L, str.GetUtf8());
// 		#else
// 			const char* str = va_arg(arglist, const char*);
// 			lua_pushlstring(L, str, strlen(str));
// 		#endif
// 
// 			break;
// 		}
// 		case 'S':
// 		{
// 			iNumArg++;
// 
// 		#ifdef UNICODE
// 			const char* str = va_arg(arglist, const char*);
// 			lua_pushlstring(L, str, strlen(str));
// 		#else
// 			CScriptString str;
// 			str.SetAWString(va_arg(arglist, const wchar_t*));
// 			lua_pushstring(L, str.GetUtf8());
// 		#endif
// 
// 			break;
// 		}
// 		case 'p':
// 
// 			iNumArg++;
// 			lua_pushlightuserdata(L, va_arg(arglist, void*));
// 			break;
// 
// 		default:
// 
// 			ASSERT(0);
// 			break;
// 		}
// 	}
// 
// 	return iNumArg;
// }

bool Call(lua_State*L, const char * szTblName, const char * szMethod, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	int n = lua_gettop(L); //save the stack pos
	SetStack(L, args);
	if (!Call(L, szTblName, szMethod, args.size(), LUA_MULTRET))
	{
		lua_settop(L, n);   //restore the stack pos
		return false;
	}

	GetStack(L, results, n+1);
	lua_settop(L, n);
	
	return true;
}
#if 0
/*	szFormat string describe following arguments type sequentially. 
	'd', 'f', 'b', 's', 'S', 'p' can appear in szFormat, their meanings
	explained below:

	b: boolean
	d: integer
	f: float or double
	s: ANSI string or Unicode string base on application's type
	S: ANSI string or Unicode string base on application's type, 'S' has
		inverse meaning of 's', just like they behave in printf's format
	p: pointer, user data

	example: 

		Call(L, szTableName, szMethodName, rlts, "dfs", 10, 5.0f, "hello");

		d - 10
		f - 5.0f
		s - "hello"
*/
// bool Call(lua_State* L, const char* szTable, const char* szMethod, 
// 			 abase::vector<CScriptValue>& rlts, const char* szFormat, ...)
// {
// 	//	save the stack pos
// 	int n = lua_gettop(L); 
// 
// 	va_list argList;
// 	va_start(argList, szFormat);
// 	int iNumArg = SetStack(L, szFormat, argList);
// 	va_end(argList);
// 
// 	if (!Call(L, szTable, szMethod, iNumArg, LUA_MULTRET))
// 	{
// 		//	restore the stack pos
// 		lua_settop(L, n);
// 		return false;
// 	}
// 
// 	//	Get returned values
// 	GetStack(L, rlts, n+1);
// 	lua_settop(L, n);
// 	
// 	return true;
// }
#endif

}
/********************************************************************
	created:	2006/08/21
	author:		kuiwu
	
	purpose:	construct a lua table (only function) from  memory
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/
#pragma once

#include "vector.h"
#include <AString.h>
#include <vector.h>
#include "ScriptValue.h"

class CLuaState;

class CLuaMemTbl  
{
public:
	CLuaMemTbl();
	virtual ~CLuaMemTbl();
	bool Init(const char * szTblName, bool bConfig = true);
	void Release();
	bool AddMethod(const char * szName,  const abase::vector<AString>& vArgs, const char * szBody);
	void RemoveMethod(const char * szName);
	bool Call(const char * szMethod, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results);
private:
	CLuaState * m_pState;
	AString        m_szTblName;
	int         m_Ref;
	
	abase::vector<AString>  m_Methods;
};



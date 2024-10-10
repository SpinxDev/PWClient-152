/********************************************************************
	created:	2006/09/14
	author:		kuiwu
	
	purpose:	a lua thread(coroutine) 
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/
#pragma  once

#include <vector.h>

typedef struct lua_State lua_State;
class CLuaState;
class CScriptValue;
class CLuaThread  
{
public:
	enum LT_STATUS
	{
		LT_NOTINIT,
		LT_NOTSTART,
		LT_RUNNING,
		LT_ERROR,
		LT_SLEEP,
		LT_DONE,
	};
	CLuaThread();
	virtual ~CLuaThread();
	bool Init(CLuaState * pMasterState);
	void Release();
	bool BeginThread(const char * szTbl, const char * szMethod, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results);
	bool ResumeThread(const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results);
	bool Call(const char * szTbl, const char * szMethod, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results);
	
	int  GetStatus() const 
	{
		return m_Status;
	}
	lua_State * GetThreadVM() const
	{
		return m_pThreadVM;
	}

	//the methods only used by lua api
	void   Sleep()
	{
		m_Status = LT_SLEEP;
	}
private:

	bool _Resume(int nargs, abase::vector<CScriptValue>& results, int rt_start);

	CLuaState * m_pMasterState;
	lua_State * m_pThreadVM;
	int         m_refThread;
	int         m_Status;
};

void RegisterThreadApi(lua_State * L);

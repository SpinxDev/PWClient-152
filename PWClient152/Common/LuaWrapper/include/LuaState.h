/********************************************************************
	created:	2006/08/07
	author:		kuiwu
	
	purpose:	an independent lua state.
	history:    add the script lists to the class state  [9/10/2007 kuiwu]
				move CLuaDebug object from CLuaStateMan to CLuaState
	
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/
#pragma  once

#include <windows.h>
#include <vector.h>
#include <hashmap.h>
#include <AString.h>
#include <vector.h>



typedef struct lua_State lua_State;
class CScriptValue;
class CLuaScript;
class CLuaDebug;

class CLuaState  
{

public:
	CLuaState();
	virtual ~CLuaState();
	lua_State * GetVM()
	{
		return m_pState;
	}
	void Release();
	bool Init(int debugType);
	// lock and call
	//note: it will be efficient that cache calls in the same os thread using the 
	//      LuaBind::call directly.
	bool LockCall(const char * szTbl, const char * szMethod, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results);
	//call, the caller should handle lock/unlock the state if running in the multithread env. 
	bool Call(const char * szTbl, const char * szMethod, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results);
	//it will be efficient that
	//the caller caches server ops during one lock/unlock pair in the multi-thread environment
	void Lock();
	void Unlock();
	//the caller should  handle lock/unlock the state if running in the multithread env.
// 	CLuaScript * Register(const CLuaScript& prototype);
//	void         UnRegister(const char * szName);

	//just register a script, not fill the script content and execute
	CLuaScript * Register(const char * szName);
	//register, fill the script content and execute
	CLuaScript * RegisterFile(const char * szFileName);
	CLuaScript * RegisterBuffer(const char * szBufName, const char * buf, int len);
	
	void         RegisterLibApi(const char * libName);
	void         ReleaseTbl(const char * tblName);
	/**
	 * \brief unregister a buffer(file)
	 * \param[in]  szName, buffer(file) name
	 * \param[in]  rmTbls, the tables need to be removed when the buffer is unregistered.
	 *					   it will be helpful to notify lua it the table is never used.
	 * \return
	 * \note
	 * \warning
	 * \todo   
	 * \author kuiwu 
	 * \date 26/9/2007
	 * \see 
	 */
	void         UnRegister(const char * szName, abase::vector<AString>* rmTbls = NULL);
	CLuaScript * GetScript(const char * szName);
	abase::hash_map<AString, CLuaScript*> * GetAllScripts() 
	{
		return &m_LuaScripts;
	}
	CLuaDebug * GetDebug() 
	{
		return m_pLuaDebug;
	}
	bool IsDebugEnabled() const;
	void EnableDebug(bool enable);
	
	void Tick();
protected:
	lua_State * m_pState;
	
	CRITICAL_SECTION	m_csVisit;

	abase::hash_map<AString, CLuaScript *> m_LuaScripts;

	CLuaDebug * m_pLuaDebug;

};

class CLuaStateMan
{
public:

	CLuaStateMan()
	{
		m_CodePage = CP_ACP; //todo: which codepage as default???
	};
	~CLuaStateMan(){};

	//static CLuaStateMan * GetInstance();

	bool Init(bool bEnableDebug = false);
	void Release();
	
// 	CLuaState * GetUIState()
// 	{
// 		return m_UIState;
// 	}
	
	//the state  treats lua script as a config file, run once.
	CLuaState * GetConfigState()
	{
		return m_ConfigState;
	}
	//the state commonly used, treats lua script as a logic file.
	CLuaState * GetAIState()
	{
		return m_AIState;
	}
	
	CLuaState * GetState(lua_State * L);

	unsigned int GetCodePage() const
	{
		return m_CodePage;
	}
	void         SetCodePage(unsigned int cp)
	{
		m_CodePage = cp;
	}

	void        Tick(DWORD dwTick);
private:
	//CLuaState * m_UIState;
	CLuaState * m_ConfigState;
	CLuaState * m_AIState;
	
	//static	CLuaStateMan *  m_Instance;

	unsigned int  m_CodePage;

};

extern CLuaStateMan g_LuaStateMan;


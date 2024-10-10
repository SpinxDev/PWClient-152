#ifndef EC_MODELMAN_H_
#define EC_MODELMAN_H_

#include "LuaMemTbl.h"
#include "A3DCombActMan.h"

class CECModelStaticData;
class CECModelMan
{
public:

	CECModelMan() : m_bScriptInit(false) { ::InitializeCriticalSection(&m_csModels); }
	~CECModelMan()
	{
		Release();
		::DeleteCriticalSection(&m_csModels); 
	}

protected:

	A3DGfxSharedObjMan<CECModelStaticData> m_SharedModelMan;
	CLuaMemTbl m_ScriptEventTbl;
	CLuaMemTbl m_ScriptEventTblConfigState;
	bool m_bScriptInit;
	CRITICAL_SECTION m_csModels;

public:

	void Lock() { ::EnterCriticalSection(&m_csModels); }
	void Unlock() { ::LeaveCriticalSection(&m_csModels); }

	CECModelStaticData* LoadModelData(const char* szModelFile, bool bLoadAdditionalSkin, bool bLoadConvexHull);

	CECModelStaticData* GetAndRef(const char* strName);

	void AddModel(const char* strName, CECModelStaticData* pModel);
	CECModelStaticData* ReleaseModel(const char* strName);

	void LogCurrentModels()
	{
		Lock();
		m_SharedModelMan.LogCurrentObjs();
		Unlock();
	}

	bool IsActScriptInit() const { return m_bScriptInit; }
	CLuaMemTbl& GetActScriptTbl() { return m_ScriptEventTbl; }
	CLuaMemTbl& GetActScriptTblConfigState() { return m_ScriptEventTblConfigState; }

	bool Init();
	void Release();
};

CECModelMan* AfxGetECModelMan();

#endif

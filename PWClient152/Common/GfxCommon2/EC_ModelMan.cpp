#include "StdAfx.h"
#include "Windows.h"
#include "EC_Model.h"
#include "EC_ModelMan.h"

inline AString GetRelativePath(const char* szPath)
{
	AString strRelPath;
	af_GetRelativePath(szPath, strRelPath);
	strRelPath.MakeLower();
	return strRelPath;
}

bool CECModelMan::Init()
{
	m_bScriptInit = m_ScriptEventTbl.Init("ECMActScriptTbl", false);

	if (!m_bScriptInit)
		return false;

	m_bScriptInit = m_ScriptEventTblConfigState.Init("ECMActScriptTbl");
	return m_bScriptInit;
}

CECModelStaticData* CECModelMan::GetAndRef(const char* strName)
{
	AString strRelPath = GetRelativePath(strName);
	Lock();
	CECModelStaticData* p = m_SharedModelMan.GetAndRefItem(strRelPath);
	Unlock();
	return p;
}

void CECModelMan::AddModel(const char* strName, CECModelStaticData* pModel)
{
	AString strRelPath = GetRelativePath(strName);
	Lock();
	m_SharedModelMan.AddItem(strRelPath, pModel);
	Unlock();
}

CECModelStaticData* CECModelMan::ReleaseModel(const char* strName)
{
	AString strRelPath = GetRelativePath(strName);
	Lock();
	CECModelStaticData* pData = m_SharedModelMan.RemoveItem(strRelPath);
	Unlock();
	return pData;
}


void CECModelMan::Release()
{
	Lock();
	m_SharedModelMan.Release();
	Unlock();

	if (m_bScriptInit)
	{
		m_ScriptEventTbl.Release();
		m_ScriptEventTblConfigState.Release();
		m_bScriptInit = false;
	}
}

CECModelStaticData* CECModelMan::LoadModelData(const char* szModelFile, bool bLoadAdditionalSkin, bool bLoadConvexHull)
{
	int nLen = strlen(szModelFile);

	if (nLen < 4 || stricmp(szModelFile + nLen - 4, ".ecm") != 0)
		return NULL;

	CECModelStaticData* pMapModel = GetAndRef(szModelFile);

	if (pMapModel)
		return pMapModel;

	pMapModel = new CECModelStaticData;

	if (!pMapModel->LoadData(szModelFile, bLoadAdditionalSkin, bLoadConvexHull))
	{
		delete pMapModel;
		return NULL;
	}

	Lock();
	CECModelStaticData* pMapModelOld = GetAndRef(szModelFile);

	if (pMapModelOld)
	{
		Unlock();
		delete pMapModel;
		return pMapModelOld;
	}

	AddModel(szModelFile, pMapModel);
	Unlock();
	return pMapModel;
}

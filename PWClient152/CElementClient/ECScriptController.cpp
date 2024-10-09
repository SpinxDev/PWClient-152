#include <ABaseDef.h>
#include <ATextTabFile.h>
#include <AString.h>
#include "ECScriptController.h"
#include "ECScript.h"
#include "ECScriptUnitExecutor.h"
#include "ECScriptContext.h"
#include "ECScriptOption.h"
#include "ECScriptUI.h"
#include "ECScriptGlobalResource.h"
#include <AFilePackage.h>
#include <AFileImage.h>
#include "EC_Global.h"

bool CECScriptController::LoadScript(AString strPath)
{
	CECScript *pScript = new CECScript();
	CECScriptUnitExecutor *pExecutor = 
		new CECScriptUnitExecutor(m_pContext);

	// load script
	if (!pScript->LoadFromFile(strPath, pExecutor))
	{
		delete pExecutor;
		delete pScript;
		return false;
	}

	// check if the id is identical
	CScriptArrayIterator iter = m_vecScript.begin();
	for (; iter != m_vecScript.end(); ++iter)
	{
		if ((*iter)->GetID() == pScript->GetID())
		{
			ASSERT(!"script id duplicated");
			delete pScript;
			return false;
		}
	}
	
	// add to list
	m_vecScript.push_back(pScript);

	return true;
}

bool CECScriptController::LoadScriptList(AString strPath, CAStringArray &vecFileList)
{
	AFileImage fileList;
	if (!fileList.Open(strPath, AFILE_NOHEAD | AFILE_TEXT | AFILE_OPENEXIST | AFILE_TEMPMEMORY))
	{
		return false;
	}
	char temp[MAX_PATH];
	DWORD dwTemp;
	while (fileList.ReadLine(temp, MAX_PATH, &dwTemp))
	{
		AString strFile = temp;
		strFile.TrimLeft();
		strFile.TrimRight();
		if (!strFile.IsEmpty())
		{
			vecFileList.push_back(strFile);
		}
	}

	return true;
}


bool CECScriptController::Init(CECScriptContext *pContext, AString strRelativePathOfScript)
{
	ASSERT(pContext);
	m_pContext = pContext;
	

	CAStringArray vecFileList;
	if (!LoadScriptList(strRelativePathOfScript + "list.txt", vecFileList))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECScriptController::Init", __LINE__);
		return true;
	}
	CAStringArray::iterator iter = vecFileList.begin();
	for (; iter != vecFileList.end(); ++iter)
	{
		if (!LoadScript(strRelativePathOfScript + *iter))
		{
			glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECScriptController::Init", __LINE__);
		}
	}

	return true;
}

void CECScriptController::TickRunScript(DWORD dwTickTime)
{
	// run each script 
	CScriptArrayIterator iter = m_vecScript.begin();
	for (; iter != m_vecScript.end(); ++iter)
	{
		CECScript *pScript = *iter;

		// ignore deactivated script
		if (!m_pContext->GetOption()->IsScriptActive(pScript->GetID()) ||
			!m_pContext->GetOption()->IsTypeActive(pScript->GetType()))
			continue;

		// run script
		pScript->Run();
	}
	
}

// add by zhougaomin01305 2012/8/10
void CECScriptController::TickRunScript(DWORD dwTickTime,const abase::vector<int>* pnIDS)
{
	if (!pnIDS) return;
	// run each script 
	CScriptArrayIterator iter = m_vecScript.begin();
	for (; iter != m_vecScript.end(); ++iter)
	{
		CECScript *pScript = *iter;
		
		// force show script help. 
		abase::vector<int>::const_iterator cit,cite;
		for(cit = pnIDS->begin(),cite = pnIDS->end(); cit != cite; ++ cit)
		{
			if (pScript->GetID() == *cit)
			{
				// force run script
				if (m_pContext->GetOption()->IsScriptActive(pScript->GetID()))
					pScript->Run();
				break;
			}
		}
	}
	
}

CECScriptController::~CECScriptController()
{
	// delete scripts
	CScriptArrayIterator iter = m_vecScript.begin();
	for (; iter != m_vecScript.end(); ++iter)
	{
		delete (*iter);
	}
}

CECScriptController::CECScriptController() :
	m_pCurrentScript(NULL),
	m_bNewScriptRegisteredToOption(false)
{

}


void CECScriptController::FinishCurrentScript(CECScript* pNewScript)
{
	if (m_pCurrentScript)
	{
		m_pContext->GetOption()->SetScriptActive(
			m_pCurrentScript->GetID(), false);
		m_pContext->GetOption()->SaveToServer();
	}
	m_pContext->GetGlobalResource()->ReleaseAllResource();
	m_pContext->GetUI()->CloseHelpDialog(NULL == pNewScript);
	m_pCurrentScript = pNewScript;
}

CECScript * CECScriptController::GetScriptFromID(int nScriptID)
{
	CScriptArrayIterator iter = m_vecScript.begin();
	for (; iter != m_vecScript.end(); ++iter)
	{
		CECScript *pScript = *iter;
		if (pScript->GetID() == nScriptID)
			return pScript;
	}
	return NULL;
}

bool CECScriptController::IsScriptLoaded(int nScriptID)
{
	CScriptArrayIterator iter = m_vecScript.begin();
	for (; iter != m_vecScript.end(); ++iter)
	{
		if ((*iter)->GetID() == nScriptID)
			return true;
	}
	return false;
}

void CECScriptController::RegisterNewScriptToOption()
{
	if (m_bNewScriptRegisteredToOption)
		return;

	CScriptArrayIterator iter = m_vecScript.begin();
	for (; iter != m_vecScript.end(); ++iter)
	{
		CECScript *pScript = *iter;

		// register new added script's state 
		CECScriptOption *pOption = m_pContext->GetOption();
		if (!pOption->IsScriptInOption(pScript->GetID()))
		{
			pOption->RegisterScript(pScript->GetID());
		}
	}
	m_bNewScriptRegisteredToOption = true;
}

CScriptArray & CECScriptController::GetScriptArray()
{
	return m_vecScript;
}

void CECScriptController::ForceActiveScript(int nID)
{
	CECScript *pScript = GetScriptFromID(nID);
	ASSERT(pScript);
	m_pContext->GetOption()->SetScriptActive(nID, true);
	pScript->GetExecutor()->SetNextState(new CECScriptUnitExecutorStateAfterPop(
		pScript->GetExecutor()));
}

const CECScript * CECScriptController::GetCurrentScript() {
	return m_pCurrentScript;
}
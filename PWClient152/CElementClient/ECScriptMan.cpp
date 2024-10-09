#include "CodeTemplate.h"
#include "ECScriptMan.h"
#include "ECScriptContext.h"
#include "ECScriptOption.h"
#include "ECScriptCheckState.h"
#include "ECScriptController.h"
#include "ECScriptUI.h"
#include "ECScriptGlobalResource.h"


typedef unsigned char BYTE;

CECScriptMan::CECScriptMan() :
	m_pContext(NULL)
{

}

CECScriptMan::~CECScriptMan()
{
}

void CECScriptMan::Release()
{
	// delete context
	if (m_pContext)
	{
		delete m_pContext->GetCheckState();
		delete m_pContext->GetController();
		delete m_pContext->GetOption();
		delete m_pContext->GetUI();
		delete m_pContext->GetGlobalResource();
		SAFE_DELETE(m_pContext);
	}
}


void CECScriptMan::Tick(DWORD dwTickTime)
{
	if (!m_pContext->GetOption()->IsOptionLoaded())
	{
		if (CanStartLoad())
			m_pContext->GetOption()->LoadFromServer();
	}
	else
	{
		// first time enter, synchrolize server option with 
		// local new script
		m_pContext->GetOption()->ShrinkOldScriptInOption();
		m_pContext->GetController()->RegisterNewScriptToOption();

		const int nIDCountryMap    =  5032;// 国战帮助script id
		const int nIDCountryWarMap1 =  5033;// 战场帮助script id
		const int nIDCountryWarMap2 =  5034;// 战场帮助script id
		const int nIDCountryWarMap3 =  5035;// 战场帮助script id
		abase::vector<int> vIDsForceShow;// 强制显示的帮助id列表
		vIDsForceShow.push_back(nIDCountryMap);
		vIDsForceShow.push_back(nIDCountryWarMap1);
		vIDsForceShow.push_back(nIDCountryWarMap2);
		vIDsForceShow.push_back(nIDCountryWarMap3);

		// run
		if (!m_pContext->GetOption()->IsHelpDisabled())
			m_pContext->GetController()->TickRunScript(dwTickTime);
		else //强制显示帮助 add by zhougaomin01305 2012/8/10
			m_pContext->GetController()->TickRunScript(dwTickTime,&vIDsForceShow);
		
		// save option
		m_pContext->GetOption()->TickSave();
	}
}


bool CECScriptMan::Init()
{
	m_pContext = new CECScriptContext;
	CECScriptOption *pOption = NULL;
	IECScriptCheckState *pCheckState = NULL;
	IECScriptUI *pUI = NULL;
	CECScriptController *pController = NULL;
	IECScriptGlobalResource *pGlobalResource = NULL;

	
	BEGIN_FAKE_WHILE;

	pOption = CreateOption(m_pContext);
	CHECK_BREAK(pOption);
	m_pContext->SetOption(pOption);

	pCheckState = CreateCheckState(m_pContext);
	CHECK_BREAK(pCheckState);
	m_pContext->SetCheckState(pCheckState);

	pUI = CreateUI(m_pContext);
	CHECK_BREAK(pUI);
	m_pContext->SetUI(pUI);

	pController = CreateController(m_pContext);
	CHECK_BREAK(pController);
	m_pContext->SetController(pController);

	pGlobalResource = CreateGlobalResource(m_pContext);
	CHECK_BREAK(pGlobalResource);
	m_pContext->SetGlobalResource(pGlobalResource);

	END_FAKE_WHILE;

	BEGIN_ON_FAIL_FAKE_WHILE;

	SAFE_DELETE(pController);
	SAFE_DELETE(pOption);
	SAFE_DELETE(pCheckState);
	SAFE_DELETE(pUI);
	SAFE_DELETE(pGlobalResource);
	SAFE_DELETE(m_pContext);

	END_ON_FAIL_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

CECScriptContext * CECScriptMan::GetContext()
{
	return m_pContext;
}

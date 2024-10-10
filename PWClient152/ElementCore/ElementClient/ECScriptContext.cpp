#include "ECScriptContext.h"
#include <ABaseDef.h>

CECScriptContext::CECScriptContext() :
	m_pOption(NULL),
	m_pCheckState(NULL),
	m_pUI(NULL),
	m_pController(NULL),
	m_pGlobalResource(NULL)
{

}

CECScriptContext::~CECScriptContext()
{
}

CECScriptOption * CECScriptContext::GetOption()
{
	return m_pOption;
}

IECScriptCheckState * CECScriptContext::GetCheckState()
{
	return m_pCheckState;
}

IECScriptUI * CECScriptContext::GetUI()
{
	return m_pUI;
}

CECScriptController * CECScriptContext::GetController()
{
	return m_pController;
}

IECScriptGlobalResource * CECScriptContext::GetGlobalResource()
{
	return m_pGlobalResource;
}


void CECScriptContext::SetOption(CECScriptOption * pOption)
{
	ASSERT(pOption);
	m_pOption = pOption;
}

void CECScriptContext::SetCheckState(IECScriptCheckState * pCheckState)
{
	ASSERT(pCheckState);
	m_pCheckState = pCheckState;
}

void CECScriptContext::SetUI(IECScriptUI * pUI)
{
	ASSERT(pUI)	;
	m_pUI = pUI;
}

void CECScriptContext::SetController(CECScriptController *pController)
{
	m_pController = pController;
}

void CECScriptContext::SetGlobalResource(IECScriptGlobalResource *pGlobalResource)
{
	m_pGlobalResource = pGlobalResource;
}


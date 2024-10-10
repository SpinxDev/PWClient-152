/********************************************************************
	created:	2005/09/03
	created:	3:9:2005   11:47
	file name:	ECScriptContext.h
	author:		yaojun
	
	purpose:	scripts running context
*********************************************************************/

#pragma once

class CECScriptOption;
class IECScriptCheckState;
class IECScriptUI;
class CECScriptController;
class CECStringTab;
class IECScriptGlobalResource;

class CECScriptContext
{
private:
	CECScriptOption * m_pOption;
	IECScriptCheckState * m_pCheckState;
	IECScriptUI * m_pUI;
	CECScriptController * m_pController;
	IECScriptGlobalResource * m_pGlobalResource;
public:
	CECScriptContext();
	~CECScriptContext();

	CECScriptOption * GetOption();
	IECScriptCheckState * GetCheckState();
	IECScriptUI * GetUI();
	CECScriptController * GetController();
	IECScriptGlobalResource * GetGlobalResource();

	void SetOption(CECScriptOption * pOption);
	void SetCheckState(IECScriptCheckState * pCheckState);
	void SetUI(IECScriptUI * pUI);
	void SetController(CECScriptController *pController);
	void SetGlobalResource(IECScriptGlobalResource *pGlobalResource);
};
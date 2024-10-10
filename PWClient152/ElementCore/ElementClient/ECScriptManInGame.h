/********************************************************************
	created:	2005/09/09
	created:	9:9:2005   14:48
	file name:	ECScriptManInGame.h
	author:		yaojun
	
	purpose:	implement of IECScriptMan for game
*********************************************************************/

#pragma once

#include "ECScriptMan.h"

class CECScriptManInGame : public CECScriptMan
{
protected:
	virtual CECScriptOption * CreateOption(CECScriptContext * pContext);
	virtual IECScriptCheckState * CreateCheckState(CECScriptContext * pContext);
	virtual IECScriptUI * CreateUI(CECScriptContext * pContext);
	virtual CECScriptController * CreateController(CECScriptContext * pContext);
	virtual IECScriptGlobalResource * CreateGlobalResource(CECScriptContext * pContext);
	virtual bool CanStartLoad();
};
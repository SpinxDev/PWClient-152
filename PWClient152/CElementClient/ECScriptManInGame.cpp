#include "ECScriptManInGame.h"
#include "ECScriptOptionInGame.h"
#include "ECScriptCheckStateInGame.h"
#include "ECScriptUIInGame.h"
#include "ECScriptGlobalResourceInGame.h"
#include "ECScriptController.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "CodeTemplate.h"

CECScriptOption * CECScriptManInGame::CreateOption(CECScriptContext * pContext)
{
	CECScriptOptionInGame *pOption = new CECScriptOptionInGame;
	if (!pOption->Init(pContext))
		SAFE_DELETE(pOption);
	return pOption;
}

IECScriptCheckState * CECScriptManInGame::CreateCheckState(CECScriptContext * pContext)
{
	CECScriptCheckStateInGame *pCheckState = new CECScriptCheckStateInGame;
	return pCheckState;
}

IECScriptUI * CECScriptManInGame::CreateUI(CECScriptContext * pContext)
{
	CECScriptUIInGame *pUI = new CECScriptUIInGame;
	if (!pUI->Init(pContext))
		SAFE_DELETE(pUI);
	return pUI;
}

CECScriptController * CECScriptManInGame::CreateController(CECScriptContext * pContext)
{
	CECScriptController *pController = new CECScriptController;
	if (!pController->Init(pContext, "Help\\Script\\"))
		SAFE_DELETE(pController);
	return pController;
}

IECScriptGlobalResource * CECScriptManInGame::CreateGlobalResource(CECScriptContext * pContext)
{
	CECScriptGlobalResourceInGame *pGlobalResource = new CECScriptGlobalResourceInGame;
	return pGlobalResource;
}


bool CECScriptManInGame::CanStartLoad()
{
	return g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->IsUIEnabled();
}

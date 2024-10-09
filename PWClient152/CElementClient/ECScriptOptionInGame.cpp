#include "ECScriptOptionInGame.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "DlgScriptHelp.h"


//bool CECScriptOptionInGame::Init(CECScriptContext *pContext)
//{
//	CECScriptOption::Init(pContext);
//
//	CDlgScriptHelp *pDlg = dynamic_cast<CDlgScriptHelp *>(
//		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_ScriptHelp"));
//	ASSERT(pDlg);
//	pDlg->SetScriptOption(this);
//	return true;
//}
//
//void CECScriptOptionInGame::LoadFromServer()
//{
//	CECScriptOption::LoadFromServer();
//
////	CDlgScriptHelp *pDlg = dynamic_cast<CDlgScriptHelp *>(
////		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_ScriptHelp"));
////	ASSERT(pDlg);
////	pDlg->SetIsAutoPopup(IsAutoPop());
//}

void CECScriptOptionInGame::LoadOption(PONRECEIVEOPTIONS pfnCallBack)
{
	g_pGame->GetGameSession()->LoadHelpStates(pfnCallBack);
}

void CECScriptOptionInGame::SaveOption(const void* pDataBuf, int iSize)
{
	g_pGame->GetGameSession()->SaveHelpStates(pDataBuf, iSize);
}

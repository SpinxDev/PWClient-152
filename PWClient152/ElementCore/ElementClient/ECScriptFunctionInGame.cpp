#include "ECScriptFunctionInGame.h"
#include "ECScriptContext.h"
#include "ECScriptUI.h"
#include "ECScriptGlobalResource.h"
#include "ECScriptCheckState.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_Global.h"
#include "EC_UIManager.h"
#include "AUIDialog.h"

REGISTER_TO_ECSCRIPTFUNCTION_FACTORY("ShowHelpDialog", CECScriptFunctionShowHelpDialog)
REGISTER_TO_ECSCRIPTFUNCTION_FACTORY("SetHelpDialogContent", CECScriptFunctionSetContent)
REGISTER_TO_ECSCRIPTFUNCTION_FACTORY("ShowDialog", CECScriptFunctionShowDialog)
REGISTER_TO_ECSCRIPTFUNCTION_FACTORY("SetFlashDialogItem", CECScriptFunctionSetFlashDialogItem)
REGISTER_TO_ECSCRIPTFUNCTION_FACTORY("SetHasNextTip", CECScriptFunctionSetHasNextTip)
REGISTER_TO_ECSCRIPTFUNCTION_FACTORY("PlayVoice", CECScriptFunctionPlayVoice)
REGISTER_TO_ECSCRIPTFUNCTION_FACTORY("StopVoice", CECScriptFunctionStopVoice)
REGISTER_TO_ECSCRIPTFUNCTION_FACTORY("ResetTimer", CECScriptFucntionResetTimer)
REGISTER_TO_ECSCRIPTFUNCTION_FACTORY("SetHelpDialogImage", CECScriptFunctionSetImage)
REGISTER_TO_ECSCRIPTFUNCTION_FACTORY("SetTipTitleAndContent", CECScriptFunctionSetTipTitleAndContent)
REGISTER_TO_ECSCRIPTFUNCTION_FACTORY("ShowTip", CECScriptFunctionShowTip)


void CECScriptFunctionSetHasNextTip::Execute(CECScriptContext *pContext)
{
	pContext->GetUI()->SetHasNextTip(m_bHasNext);
}

void CECScriptFunctionShowHelpDialog::Execute(CECScriptContext *pContext)
{
	if (m_bShow)
		pContext->GetUI()->ShowHelpDialog();
	else
		pContext->GetUI()->CloseHelpDialog();
}

void CECScriptFunctionSetContent::Execute(CECScriptContext *pContext)
{
	pContext->GetUI()->SetHelpDialogContent(m_strContent);
}

void CECScriptFunctionShowDialog::Execute(CECScriptContext *pContext)
{
	PAUIDIALOG pDlg = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog(m_szDialogName);
	ASSERT(pDlg);
	pDlg->Show(m_bShow);
}

void CECScriptFunctionSetFlashDialogItem::Execute(CECScriptContext *pContext)
{
	PAUIDIALOG pDlg = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog(m_szDialogName);
	ASSERT(pDlg);
	PAUIOBJECT pObj = pDlg->GetDlgItem(m_szItemName);
	ASSERT(pObj);
	pContext->GetGlobalResource()->FlashItem(pObj, m_bFlash);
}

void CECScriptFunctionPlayVoice::Execute(CECScriptContext *pContext)
{
	pContext->GetGlobalResource()->PlayVoice(m_strFileName);
}

void CECScriptFunctionStopVoice::Execute(CECScriptContext *pContext)
{
	pContext->GetGlobalResource()->StopVoice(m_bFadeOut);
}

void CECScriptFucntionResetTimer::Execute(CECScriptContext *pContext)
{
	pContext->GetCheckState()->SetScriptTimerParam(a_GetTime(), m_dwDuration);
}

void CECScriptFunctionSetImage::Execute(CECScriptContext *pContext) {
	pContext->GetUI()->SetHelpDialogImage(m_strImgFile);
}

void CECScriptFunctionSetTipTitleAndContent::Execute(CECScriptContext *pContext) {
	pContext->GetUI()->SetTipDialogTitleAndContent(m_strTitle, m_strContent);
}

void CECScriptFunctionShowTip::Execute(CECScriptContext *pContext) {
	pContext->GetUI()->ShowTip(m_iGfxTime, m_iInTime, m_iStayTime, m_iOutTime);
}
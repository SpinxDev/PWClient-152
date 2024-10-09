#include "ECScriptUIInGame.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "ECScriptContext.h"
#include "ECScriptCheckState.h"
#include "ECScriptOption.h"
#include "ECScriptController.h"
#include "DlgScriptHelp.h"
#include "DlgInfo.h"
#include "AUITextArea.h"
#include "ECScript.h"
#include "ECScriptUnitExecutor.h"
#include "DlgScriptHelpHistory.h"
#include "DlgScriptTip.h"

CECScriptUIInGame::CECScriptUIInGame()
{
}

CECScriptUIInGame::~CECScriptUIInGame()
{
}


bool CECScriptUIInGame::Init(CECScriptContext * pContext)
{
	m_pContext = pContext;

	GetHelpDlg()->SetScriptContext(m_pContext);
	GetInfoDlg()->SetScriptContext(m_pContext);

	return true;
}

CDlgScriptHelp * CECScriptUIInGame::GetHelpDlg()
{
	CDlgScriptHelp *pDlg = dynamic_cast<CDlgScriptHelp *>(
		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_ScriptHelp"));
	ASSERT(pDlg);
	return pDlg;
}

CDlgScriptTip * CECScriptUIInGame::GetTipDlg() {
	CDlgScriptTip *pDlg = dynamic_cast<CDlgScriptTip *>(
		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_ScriptTip"));
	ASSERT(pDlg);
	return pDlg;
}

CDlgInfo * CECScriptUIInGame::GetInfoDlg()
{
	CDlgInfo *pDlg = dynamic_cast<CDlgInfo *>(
		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_Poplist"));
	ASSERT(pDlg);
	return pDlg;
}

CDlgScriptHelpHistory * CECScriptUIInGame::GetHelpHistoryDialog()
{
	CDlgScriptHelpHistory * pDlg = dynamic_cast<CDlgScriptHelpHistory *>(
		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_HelpHistory"));
	ASSERT(pDlg);
	return pDlg;
}


void CECScriptUIInGame::ShowHelpDialog()
{
	GetHelpDlg()->Show(true);
}

void CECScriptUIInGame::CloseHelpDialog(bool bCloseHistory)
{
	GetHelpDlg()->Show(false);
	if(bCloseHistory)
		GetHelpHistoryDialog()->Show(false);
}

void CECScriptUIInGame::SetHasNextTip(bool bHasNext)
{
	GetHelpDlg()->SetHasNextTip(bHasNext);
}

void CECScriptUIInGame::SetHelpDialogContent(AString strContent)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strContent, strContent.GetLength(), NULL, 0);
	ACHAR *strText = (ACHAR*)a_malloctemp(sizeof(ACHAR) * (len + 1));
	MultiByteToWideChar(CP_UTF8, 0, strContent, strContent.GetLength(), strText, len + 1);
	strText[len] = 0;

	GetHelpDlg()->SetTipText(strText);
	a_freetemp(strText);
}

void CECScriptUIInGame::SetHelpDialogImage(AString strFile) {

	int len = MultiByteToWideChar(CP_UTF8, 0, strFile, strFile.GetLength(), NULL, 0);
	ACHAR *strText = (ACHAR*)a_malloctemp(sizeof(ACHAR) * (len + 1));
	MultiByteToWideChar(CP_UTF8, 0, strFile, strFile.GetLength(), strText, len + 1);
	strText[len] = 0;

	AString fileName = AC2AS(strText);
	GetHelpDlg()->SetTipImage(fileName);
	a_freetemp(strText);
}

void CECScriptUIInGame::AddToSpirit(int nScriptID, AString strName)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strName, strName.GetLength(), NULL, 0);
	ACHAR *strText = (ACHAR*)a_malloctemp(sizeof(ACHAR) * (len + 1));
	MultiByteToWideChar(CP_UTF8, 0, strName, strName.GetLength(), strText, len + 1);
	strText[len] = 0;

	g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->AddInformation(
		CDlgInfo::INFO_HELP,"Game_HelpMsg",strText,0x0fffffff,nScriptID,0,0);

	a_freetemp(strText);
}

void CECScriptUIInGame::SetTipDialogTitleAndContent(const AString& strTitle, const AString& strContent) {
	ACString strTitleW;
	ACString strContentW;
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, strTitle, strTitle.GetLength(), NULL, 0);
		ACHAR *strText = (ACHAR*)a_malloctemp(sizeof(ACHAR) * (len + 1));
		MultiByteToWideChar(CP_UTF8, 0, strTitle, strTitle.GetLength(), strText, len + 1);
		strText[len] = 0;
		strTitleW = ACString(strText);
		a_freetemp(strText);
	}
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, strContent, strContent.GetLength(), NULL, 0);
		ACHAR *strText = (ACHAR*)a_malloctemp(sizeof(ACHAR) * (len + 1));
		MultiByteToWideChar(CP_UTF8, 0, strContent, strContent.GetLength(), strText, len + 1);
		strText[len] = 0;
		strContentW = ACString(strText);
		a_freetemp(strText);
	}

	GetTipDlg()->SetTitleAndContent(strTitleW, strContentW);
}

void CECScriptUIInGame::ShowTip(int iGfxTime, int iInTime, int iStayTime, int iOutTime) {
	GetTipDlg()->ShowTip(iGfxTime, iInTime, iStayTime, iOutTime);
}


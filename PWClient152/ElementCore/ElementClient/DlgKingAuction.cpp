// Filename	: DlgKingAuction.cpp
// Creator	: Shi && Wang
// Date		: 2013/1/29

#include "DlgKingAuction.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_UIConfigs.h"
#include "EC_UIManager.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "AUIManager.h"
#include "AIniFile.h"
#include <AFI.h>

#include "kecandidateapply_re.hpp"


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgKingAuction, CDlgBase)

AUI_ON_COMMAND("Btn_Confirm", OnCommand_Confirm)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgKingAuction, CDlgBase)

AUI_ON_EVENT("Edt_Money", WM_KEYUP, OnEvent_MoneyKeyUp)

AUI_END_EVENT_MAP()


CDlgKingAuction::CDlgKingAuction()
{
 	m_pBtnConfirm = NULL;
	m_iMoneyCount = 0;
}

CDlgKingAuction::~CDlgKingAuction()
{
}

bool CDlgKingAuction::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	return true;
}

void CDlgKingAuction::DoDataExchange(bool bSave)
{
	DDX_EditBox(bSave, "Edt_Money", m_iMoneyCount);
	DDX_Control("Btn_Confirm", m_pBtnConfirm);
}

void CDlgKingAuction::OnShowDialog()
{
	m_iMoneyCount = 0;
	UpdateData(false);
	m_pBtnConfirm->Enable(false);
}

void CDlgKingAuction::OnCommand_Confirm(const char * szCommand)
{
	UpdateData();

	if( m_iMoneyCount > GetHostPlayer()->GetYinpiaoTotalAmount() )
	{
		m_pAUIManager->MessageBox("", GetStringFromTable(10306), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}

	AUIDialog* pDlg;
	m_pAUIManager->MessageBox("Game_KingAuction", m_pAUIManager->GetStringFromTable(10307), MB_OKCANCEL, A3DCOLORRGB(255, 255, 255), &pDlg);
	pDlg->SetData(m_iMoneyCount);
}

void CDlgKingAuction::OnKECandidateApply_Re(void* pData)
{
	using namespace GNET;

	ACString errMsg;
	KECandidateApply_Re* p = (KECandidateApply_Re*)pData;
	if( p && p->retcode == 0 )
		m_pAUIManager->MessageBox("", GetStringFromTable(10314), MB_OK, A3DCOLORRGB(255, 255, 255));
	else if( p && GetGameSession()->GetServerError(p->retcode, errMsg) )
		m_pAUIManager->MessageBox("", errMsg, MB_OK, A3DCOLORRGB(255, 255, 255));
}

void CDlgKingAuction::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
}

void CDlgKingAuction::OnEvent_MoneyKeyUp(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	UpdateData();
	m_pBtnConfirm->Enable(m_iMoneyCount > 0);
}

// 检查是否可以暗拍或投票
bool CDlgKingAuction::CheckPlayerCondition(bool bAuction, ACString& strMsg)
{
	bool bOK(true);
	ACString strTemp;
	ACString strColorRed = _AL("^FF0000");
	ACString strColorWhite = _AL("^FFFFFF");
	ACString strNextLine = _AL("\r");
	
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	strMsg = pGameUI->GetStringFromTable(bAuction ? 10311 : 10313);
	
	//	玩家历史最高等级
	const int REQUIRE_LEVEL = CECUIConfig::Instance().GetGameUI().nCrossServerEnterLevel;
	int iLevel = pHost->GetMaxLevelSofar();
	if (iLevel < REQUIRE_LEVEL) bOK = false;
	
	strTemp.Format(pGameUI->GetStringFromTable(9766)
		, (iLevel < REQUIRE_LEVEL) ? strColorRed : strColorWhite
		, REQUIRE_LEVEL);
	
	strMsg += strNextLine;
	strMsg += strTemp;
	
	//	修真要求
	const int REQUIRE_LEVEL2 = CECUIConfig::Instance().GetGameUI().nCrossServerEnterLevel2;
	int iLevel2 = pHost->GetBasicProps().iLevel2;
	if (iLevel2 < REQUIRE_LEVEL2) bOK = false;
	
	strTemp.Format(pGameUI->GetStringFromTable(10135)
		, (iLevel2 < REQUIRE_LEVEL2) ? strColorRed : strColorWhite
		, g_pGame->GetGameRun()->GetLevel2NameForCondition(REQUIRE_LEVEL2));
	
	strMsg += strNextLine;
	strMsg += strTemp;
	return bOK;
}
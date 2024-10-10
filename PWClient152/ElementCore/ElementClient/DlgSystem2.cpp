// File		: DlgSystem2.h
// Creator	: Xiao Zhou
// Date		: 2005/8/15

#include "DlgSystem2.h"
//#include "DlgAction.h"
#include "DlgBBS.h"
#include "DlgArrangeTeam.h"
#include "DlgFriendList.h"
#include "DlgGuildMan.h"
#include "DlgPetList.h"
#include "EC_GameUIMan.h"
#include "DlgExp.h"
#include "DlgWiki.h"
#include "EC_HostPlayer.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "DlgAutoLock.h"
#include "EC_GameSession.h"
#include "EC_OfflineShopCtrl.h"
#include "EC_Configs.h"
#include "EC_Utility.h"
#include "DlgTrade.h"
#include "DlgShop.h"
#include "DlgSkillAction.h"
#include "DlgInventory.h"
#include "DlgOfflineShopList.h"
#include "DlgMeridians.h"
#include "DlgMatchProfile.h"
#include "EC_UIConfigs.h"

#define new DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgSystem2, CDlgBase)

AUI_ON_COMMAND("waction",		OnCommandAction)
AUI_ON_COMMAND("wteam",			OnCommandTeam)
AUI_ON_COMMAND("wfriend",		OnCommandFriend)
AUI_ON_COMMAND("wfaction",		OnCommandFaction)
AUI_ON_COMMAND("wbbs",			OnCommandBBS)
AUI_ON_COMMAND("Btn_Force",		OnCommandForce)
AUI_ON_COMMAND("Btn_QuickMatch",OnCommandMatch)
AUI_END_COMMAND_MAP()

CDlgSystem2::CDlgSystem2()
{
}

CDlgSystem2::~CDlgSystem2()
{
}

void CDlgSystem2::OnCommandPet(const char *szCommand)
{
	CDlgPetList* pDlg = GetGameUIMan()->m_pDlgPetList;
	if( pDlg )
		pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem2::OnCommandAction(const char *szCommand)
{
	/*
	if( GetGameUIMan()->m_pDlgAction )
		GetGameUIMan()->m_pDlgAction->Show(!GetGameUIMan()->m_pDlgAction->IsShow());
		*/
	GetGameUIMan()->m_pDlgSkillAction->TryOpenDialog(true);
}

void CDlgSystem2::OnCommandTeam(const char *szCommand)
{
	CDlgArrangeTeam* pDlg = GetGameUIMan()->m_pDlgArrangeTeam;
	bool bval = GetGameUIMan()->UpdateTeam(true);
	if( !bval )
		AUI_ReportError(__LINE__, __FILE__);
	if( pDlg )
		pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem2::OnCommandFriend(const char *szCommand)
{
	CDlgFriendList* pDlg = GetGameUIMan()->m_pDlgFriendList;
	if( pDlg )
		pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem2::OnCommandFaction(const char * szCommand)
{
	CDlgGuildMan *pDlg = GetGameUIMan()->m_pDlgGuildMan;
	pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem2::OnCommandBBS(const char *szCommand)
{
	CDlgBBS* pDlg = GetGameUIMan()->m_pDlgBBS;
	if( pDlg && pDlg->m_bShowBBS )
		pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem2::OnCommandAddExp(const char *szCommand)
{
	CDlgExp *pDlgAddExp = GetGameUIMan()->m_pDlgAddExp;
	if (pDlgAddExp && pDlgAddExp->IsShow())
	{
		pDlgAddExp->Show(false);
		return;
	}
	CDlgExp *pDlgSetExp = GetGameUIMan()->m_pDlgSetExp;
	if (pDlgSetExp && pDlgSetExp->IsShow())
	{
		pDlgSetExp->Show(false);
		return;
	}
	if (GetHostPlayer()->MultiExp_GetEnhanceTime()>0)
	{
		if (pDlgSetExp)
			pDlgSetExp->Show(true);
		return;
	}
	if (pDlgAddExp)
		pDlgAddExp->Show(true);
}

void CDlgSystem2::OnCommandBroadcast(const char * szCommand)
{
	PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_WebList");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem2::OnCommandForce(const char *szCommand)
{
	PAUIDIALOG pForce = GetGameUIMan()->GetDialog("Win_Force");
	if(pForce) pForce->Show(!pForce->IsShow());
}
void CDlgSystem2::OnCommandAddExp2(const char* szCommand)
{
	if (g_pGame->GetGameRun()->GetHostPlayer()->GetBasicProps().iLevel <= 30)
	{
		GetGameUIMan()->MessageBox("",GetStringFromTable(9633),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	PAUIDIALOG pAddExp2 = GetGameUIMan()->GetDialog("Win_AddExp2");
	if(pAddExp2) pAddExp2->Show(!pAddExp2->IsShow());
}
void CDlgSystem2::OnCommandMatch(const char* szCommand)
{
	if (GetGameUIMan()->GetDialog("Win_MatchHint")->IsShow())
	{
		GetGameUIMan()->GetDialog("Win_MatchHint")->Show(false);	
		return;
	}
	if (GetGameUIMan()->GetDialog("Win_MatchModeSel")->IsShow())
	{
		GetGameUIMan()->GetDialog("Win_MatchModeSel")->Show(false);	
		return;
	}
	GetGameUIMan()->m_pDlgMatchSetting->GetProfile(true, true);
}
//////////////////////////////////////////////////////////////////////////


AUI_BEGIN_COMMAND_MAP(CDlgSystem4, CDlgBase)
AUI_ON_COMMAND("Btn_Pet",			OnCommandPet)
AUI_ON_COMMAND("Btn_Action",		OnCommandAction)
AUI_ON_COMMAND("Btn_OfflineShop",	OnCommandOfflineShop)
AUI_ON_COMMAND("Btn_Broadcast",		OnCommandBroadcast)
AUI_ON_COMMAND("Btn_Wiki",			OnCommandWiki)
AUI_ON_COMMAND("Btn_Robot",			OnCommandAutoRobot)
AUI_END_COMMAND_MAP()

CDlgSystem4::CDlgSystem4()
{
}

CDlgSystem4::~CDlgSystem4()
{
}

void CDlgSystem4::OnCommandPet(const char *szCommand)
{
	CDlgPetList* pDlg = GetGameUIMan()->m_pDlgPetList;
	if( pDlg )
		pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem4::OnCommandAction(const char *szCommand)
{
	/*
	if( GetGameUIMan()->m_pDlgAction )
		GetGameUIMan()->m_pDlgAction->Show(!GetGameUIMan()->m_pDlgAction->IsShow());
	*/
	GetGameUIMan()->m_pDlgSkillAction->TryOpenDialog(true);
}

void CDlgSystem4::OnCommandBroadcast(const char * szCommand)
{
	if( !CECUIConfig::Instance().GetGameUI().bEnablePWService )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10931), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_WebList");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem4::OnCommandWiki(const char* szCommand)
{
	CDlgWiki* pDlg = GetGameUIMan()->m_pDlgWiki;
	pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem4::OnCommandAutoRobot(const char* szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_AutoPolicy");
	if( pDlg ) pDlg->Show(!pDlg->IsShow());
}
void CDlgSystem4::OnCommandOfflineShop(const char* szCommand)
{
	if (GetGameUIMan()->m_pDlgOffShopList->IsShow())
	{
		GetGameUIMan()->m_pDlgOffShopList->Show(false);
		return;
	}
	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	if(!pCtrl || pCtrl->GetNPCSevFlag() !=  COfflineShopCtrl::NPCSEV_NULL) return;

	if( g_pGame->GetConfigs()->IsMiniClient() || glb_IsInMapForMiniClient() )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10713), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	// Get shop info
	GetGameSession()->OffShop_QueryShops(COfflineShopCtrl::OSTM_ALL);
	pCtrl->SetNPCSevFlag(COfflineShopCtrl::NPCSEV_NULL);
}
//////////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgSystem5, CDlgBase)
AUI_ON_COMMAND("Btn_Skill",			OnCommandSkill)
AUI_ON_COMMAND("Btn_AddExp",		OnCommandAddExp)
AUI_ON_COMMAND("Btn_AddExp2",		OnCommandAddExp2)
AUI_ON_COMMAND("Btn_GeneralCard",	OnCommandCard)
AUI_ON_COMMAND("Btn_Meridians",		OnCommandMeridian)
AUI_END_COMMAND_MAP()

void CDlgSystem5::OnCommandSkill(const char *szCommand)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;
	GetGameUIMan()->m_pDlgSkillAction->TryOpenDialog(false);
	/*
	CDlgSkill* pDlg = GetGameUIMan()->m_pDlgSkill;
	if(pDlg)
		pDlg->Show(!pDlg->IsShow());
	*/
}

void CDlgSystem5::OnCommandAddExp2(const char* szCommand)
{
	if (g_pGame->GetGameRun()->GetHostPlayer()->GetBasicProps().iLevel <= 30)
	{
		GetGameUIMan()->MessageBox("",GetStringFromTable(9633),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	PAUIDIALOG pAddExp2 = GetGameUIMan()->GetDialog("Win_AddExp2");
	if(pAddExp2) pAddExp2->Show(!pAddExp2->IsShow());
}
void CDlgSystem5::OnCommandAddExp(const char *szCommand)
{
	CDlgExp *pDlgAddExp = GetGameUIMan()->m_pDlgAddExp;
	if (pDlgAddExp && pDlgAddExp->IsShow())
	{
		pDlgAddExp->Show(false);
		return;
	}
	CDlgExp *pDlgSetExp = GetGameUIMan()->m_pDlgSetExp;
	if (pDlgSetExp && pDlgSetExp->IsShow())
	{
		pDlgSetExp->Show(false);
		return;
	}
	if (GetHostPlayer()->MultiExp_GetEnhanceTime()>0)
	{
		if (pDlgSetExp)
			pDlgSetExp->Show(true);
		return;
	}
	if (pDlgAddExp)
		pDlgAddExp->Show(true);
}

void CDlgSystem5::OnCommandCard(const char* szCommand) // ¿¨ÅÆ
{
	if( g_pGame->GetConfigs()->IsMiniClient() )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10713), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_GeneralCard");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
}
void CDlgSystem5::OnCommandMeridian(const char* szCommand) // ÁéÂö
{
	if (GetGameUIMan()->m_pDlgMeridians->IsShow())
	{
		GetGameUIMan()->m_pDlgMeridians->Show(false);
		return;
	}

	GetGameUIMan()->m_pDlgInventory->OnCommand_meridian(NULL);
}
void CDlgSystem5::ShowNewImg(bool bShow) {
	AUIObject* pObj = GetDlgItem("Img_New");
	if (pObj) {
		pObj->Show(bShow);
	}
}

bool CDlgSystem5::OnInitDialog() {
	ShowNewImg(false);
	return CDlgHorizontalVertical::OnInitDialog();
}
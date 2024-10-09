// Filename	: DlgHost.cpp
// Creator	: Tom Zhou
// Date		: October 11, 2005

#include "AFI.h"
#include "AUICommon.h"
#include "AUICheckBox.h"
#include "AUICTranslate.h"
#include "DlgHost.h"
#include "DlgSetting.h"
#include "DlgTeamContext.h"
#include "DlgHostPet.h"
#include "DlgQuickBarPet.h"
#include "DlgAutoLock.h"
#include "DlgAutoHPMP.h"
#include "EC_Global.h"
#include "EC_Utility.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Team.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrFlySword.h"
#include "EC_IvtrEquipMatter.h"
#include "EC_Skill.h"
#include "EC_PetCorral.h"
#include "EC_Pet.h"
#include "DlgHostELF.h"
#include "DlgSystem2.h"
#include "A2DSprite.h"
#include "A3DDevice.h"
#include "EC_ForceMgr.h"
#include "EC_UIManager.h"
#include "EC_CountryConfig.h"
#include "ExpTypes.h"
#include "EC_ComputerAid.h"
#include "GT/gt_overlay.h"
#include <algorithm>
#include "EC_OfflineShopCtrl.h"
#include "EC_CrossServer.h"
#include "EC_UIConfigs.h"
#include "EC_World.h"
#include "EC_Instance.h"
#include "EC_FixedMsg.h"
#include "DlgPKSetting.h"
#include "EC_TimeSafeChecker.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgHost, CDlgBase)

AUI_ON_COMMAND("autolock",		OnCommand_lock)
AUI_ON_COMMAND("closeHP",		OnCommand_closeHP)
AUI_ON_COMMAND("headhp",		OnCommand_headhp)
AUI_ON_COMMAND("headmp",		OnCommand_headmp)
AUI_ON_COMMAND("headexp",		OnCommand_headexp)
AUI_ON_COMMAND("stop",			OnCommand_stop)
AUI_ON_COMMAND("addexp",        OnCommand_addexp)
AUI_ON_COMMAND("autohpmp",		OnCommand_autohpmp)
AUI_ON_COMMAND("gt",			OnCommand_GT)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Shop",		OnCommand_OfflineShop)
AUI_ON_COMMAND("Btn_Setting",	OnCommand_Setting)
AUI_ON_COMMAND("Combo_Line",	OnCommand_Line)
AUI_ON_COMMAND("Chk_SwitchSafety",	OnCommand_safe)
AUI_ON_COMMAND("Chk_Sight",		OnCommand_sight)
AUI_ON_COMMAND("Chk_Player",	OnCommand_player)
AUI_ON_COMMAND("Chk_Surround",	OnCommand_surround)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgHost, CDlgBase)

AUI_ON_EVENT("Size",		WM_MOUSEMOVE,		OnEventMouseMove)
AUI_ON_EVENT("Size",		WM_LBUTTONDOWN,		OnEventLButtonDown_Size)
AUI_ON_EVENT("Size",		WM_LBUTTONUP,		OnEventLButtonUp_Size)
AUI_ON_EVENT("Size",		WM_LBUTTONDBLCLK,	OnEventLButtonDBlclk_Size)
AUI_ON_EVENT("Size",		WM_RBUTTONDOWN,		OnEventRButtonDown_Size)
AUI_ON_EVENT("Size",		WM_RBUTTONUP,		OnEventRButtonUp_Size)
AUI_ON_EVENT("Size",		WM_RBUTTONDBLCLK,	OnEventRButtonDBlclk_Size)
AUI_ON_EVENT("Size",		WM_MBUTTONDOWN,		OnEventLButtonDown_Size)
AUI_ON_EVENT("Size",		WM_MBUTTONUP,		OnEventLButtonUp_Size)
AUI_ON_EVENT("Size",		WM_MBUTTONDBLCLK,	OnEventLButtonDBlclk_Size)

AUI_END_EVENT_MAP()

CDlgHost::CDlgHost()
{
	m_nMouseOffset = 0;
	m_nMouseOffsetThis = 0;
	m_nLastAP = 0;
	m_bGTWaiting = false;
	m_cntQueryParallelWorld.SetPeriod(20*1000);
	m_cntQueryParallelWorld.Reset(true);
}

CDlgHost::~CDlgHost()
{
}

bool CDlgHost::OnInitDialog()
{
	int i;
	char szItem[20];

	m_pImgSize = (PAUIIMAGEPICTURE)GetDlgItem("Size");
	m_pImgCombatMask = (PAUIIMAGEPICTURE)GetDlgItem("CombatMask");
	m_pTxt_Name = (PAUILABEL)GetDlgItem("Txt_Name");
	m_pTxt_Prof = (PAUILABEL)GetDlgItem("Txt_Prof");
	m_pTXT_LV = (PAUILABEL)GetDlgItem("TXT_LV");
	m_pPrgs_HP = (PAUIPROGRESS)GetDlgItem("Prgs_HP");
	m_pPrgs_MP = (PAUIPROGRESS)GetDlgItem("Prgs_MP");
	m_pPrgs_AP = (PAUIPROGRESS)GetDlgItem("Prgs_AP");
	m_pPrgs_EXP = (PAUIPROGRESS)GetDlgItem("Prgs_EXP");
	m_pPic_MinusExp = (PAUIIMAGEPICTURE)GetDlgItem("Pic_MinusExp");
	m_pTxt_HP = (PAUILABEL)GetDlgItem("Txt_HP");
	m_pTxt_MP = (PAUILABEL)GetDlgItem("Txt_MP");
	m_pTXT_EXP = (PAUILABEL)GetDlgItem("TXT_EXP");
	m_pImg_Leader = (PAUIIMAGEPICTURE)GetDlgItem("Img_Leader");
	m_pCombo_Line = (PAUICOMBOBOX)GetDlgItem("Combo_Line");
	m_pImg_Realm = (PAUIIMAGEPICTURE)GetDlgItem("Img_RealmNum");
	m_pImg_RealmBG = (PAUIIMAGEPICTURE)GetDlgItem("Img_Realm");

	m_pChk_SwitchSafety = (PAUICHECKBOX)GetDlgItem("Chk_SwitchSafety");

	m_pChk_Player = dynamic_cast<PAUICHECKBOX>(GetDlgItem("Chk_Player"));
	m_pChk_Surround = dynamic_cast<PAUICHECKBOX>(GetDlgItem("Chk_Surround"));
	m_pChk_Sight = dynamic_cast<PAUICHECKBOX>(GetDlgItem("Chk_Sight"));

	if( m_pImgCombatMask )
		m_pImgCombatMask->SetTransparent(true);

	for( i = 0; ; i++ )
	{
		sprintf(szItem, "Img_P%don", i + 1);
		if( !GetDlgItem(szItem) ) break;
		m_vecImgAPOn.push_back((PAUIIMAGEPICTURE)GetDlgItem(szItem));

		sprintf(szItem, "Img_P%doff", i + 1);
		if( !GetDlgItem(szItem) ) break;
		m_vecImgAPOff.push_back((PAUIIMAGEPICTURE)GetDlgItem(szItem));
		
		sprintf(szItem, "Img_P%dflash", i + 1);
		if( !GetDlgItem(szItem) ) break;
		m_vecImgAPFlash.push_back((PAUIIMAGEPICTURE)GetDlgItem(szItem));
	}

	AString strName;
	for(i = 0; ; i++ )
	{
		strName.Format("St_%d", i + 1);
		PAUIIMAGEPICTURE pIcon = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( !pIcon ) break;
		strName.Format("St_Txt_%d", i + 1);
		PAUIOBJECT pObj = GetDlgItem(strName);
		pIcon->SetDataPtr(pObj, "AUIObject");
		m_vecImgState.push_back(pIcon);
	}


	if(stricmp(GetName(), "Win_Hpmpxp") == 0)
	{
		if( CECCrossServer::Instance().IsOnSpecialServer() && GetDlgItem("Btn_Shop") )
			GetDlgItem("Btn_Shop")->Show(false);

		if( !CECUIConfig::Instance().GetGameUI().bEnableOptimize && GetDlgItem("Btn_Setting") )
			GetDlgItem("Btn_Setting")->Show(false);
	}

	return true;
}

void CDlgHost::OnCommand_closeHP(const char * szCommand)
{
	bool bCheck = ((PAUICHECKBOX)GetDlgItem("Chk_CloseHP"))->IsChecked();

	GetGameUIMan()->GetDialog("Win_Hpmpxp")->Show(!bCheck);
}

void CDlgHost::OnCommand_headhp(const char * szCommand)
{
	EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();
	if( GetDlgItem("Btn_HeadHP")->GetColor() == A3DCOLORRGB(255, 255, 255) )
	{
		vs.bPlayerHP = true;
		GetDlgItem("Btn_HeadHP")->SetColor(A3DCOLORRGB(255, 203, 74));
	}
	else
	{
		vs.bPlayerHP = false;
		GetDlgItem("Btn_HeadHP")->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	g_pGame->GetConfigs()->SetVideoSettings(vs);
	GetGameUIMan()->m_pDlgSettingCurrent->UpdateView();
}

void CDlgHost::OnCommand_headmp(const char * szCommand)
{
	EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();
	if( GetDlgItem("Btn_HeadMP")->GetColor() == A3DCOLORRGB(255, 255, 255) )
	{
		vs.bPlayerMP = true;
		GetDlgItem("Btn_HeadMP")->SetColor(A3DCOLORRGB(255, 203, 74));
	}
	else
	{
		vs.bPlayerMP = false;
		GetDlgItem("Btn_HeadMP")->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	g_pGame->GetConfigs()->SetVideoSettings(vs);
	GetGameUIMan()->m_pDlgSettingCurrent->UpdateView();
}

void CDlgHost::OnCommand_headexp(const char * szCommand)
{
	EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();
	if( GetDlgItem("Btn_HeadEXP")->GetColor() == A3DCOLORRGB(255, 255, 255) )
	{
		vs.bPlayerEXP = true;
		GetDlgItem("Btn_HeadEXP")->SetColor(A3DCOLORRGB(255, 203, 74));
	}
	else
	{
		vs.bPlayerEXP = false;
		GetDlgItem("Btn_HeadEXP")->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	g_pGame->GetConfigs()->SetVideoSettings(vs);
	GetGameUIMan()->m_pDlgSettingCurrent->UpdateView();
}

void CDlgHost::OnCommand_lock(const char * szCommand)
{
	GetGameUIMan()->GetDialog("Win_AutoLock")->Show(true);
}

void CDlgHost::OnCommand_stop(const char * szCommand)
{
	if( m_szName == "MagicProgress2" )			// Power full.
	{
		CECSkill *pCurSkill = GetHostPlayer()->GetCurSkill();
		pCurSkill->EndCharging();
		GetGameSession()->c2s_CmdContinueAction();
	}
	else if( m_szName == "MagicProgress3" )		// Stop array.
	{
		GetGameSession()->c2s_CmdCancelAction();
	}
}

void CDlgHost::OnCommand_addexp(const char *szCommand)
{
	if (m_szName == "Win_Hpmpxp")
		GetGameUIMan()->m_pDlgSystem2->OnCommandAddExp("");
}

void CDlgHost::OnCommand_autohpmp(const char *szCommand)
{
	if (m_szName == "Win_Hpmpxp")
		GetGameUIMan()->m_pDlgAutoHPMP->SwitchShow();
}
void CDlgHost::OnCommand_OfflineShop(const char* szCommand)
{
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

void CDlgHost::OnCommand_Setting(const char *szCommand)
{
	if( m_szName == "Win_Hpmpxp" )
	{
		PAUIDIALOG pDlgGfx = GetGameUIMan()->GetDialog("Win_OptimizeGfx");
		PAUIDIALOG pDlgMem = GetGameUIMan()->GetDialog("Win_OptimizeMem");
		if( !pDlgMem || !pDlgMem ) return;
		if( pDlgMem->IsShow() ) pDlgMem->Show(false);
		else if( pDlgGfx->IsShow() ) pDlgGfx->Show(false);
		else GetGameUIMan()->m_pDlgOptimizeCurrent->Show(true);
	}
}

void CDlgHost::OnCommand_GT(const char *szCommand)
{
	if (g_pGame->GetConfigs()->IsMiniClient() ||
		(CECCrossServer::Instance().IsOnSpecialServer() || CECCrossServer::Instance().IsToSpecial()) &&
		!CECUIConfig::Instance().GetGameUI().bEnableGTOnSpecialServer)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10713), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (!g_pGame->GetConfigs()->GetEnableGT())
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10931), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	if (m_bGTWaiting) return;

	using namespace overlay;
	if (!GTOverlay::Instance().IsLogin())
	{
		//	获取 gt 登录 ticket，返回时自动登录 GT
		if (!GetGameUIMan()->SSOGetGTTicket())
			return;
		m_bGTWaiting = true;
		a_LogOutput(1, "CDlgHost::OnCommand_GT, GTWaiting open");
		return;
	}

	//	切换 GT 主界面的显示
	GTOverlay::Instance().ClickGTIcon();
}

void CDlgHost::ProcessGTEnter()
{
	//	非 GTWaiting 时，直接忽略
	if (!m_bGTWaiting)	return;
	
	m_bGTWaiting = false;
	a_LogOutput(1, "CDlgHost::ProcessGTEnter, GTWaiting cleared");
}

void CDlgHost::OnCommand_CANCEL(const char * szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgHost::OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( (wParam & MK_LBUTTON) )
	{
		m_nMouseOffsetThis = GET_X_LPARAM(lParam) - m_nMouseLastX;
		if( m_nMouseOffset + m_nMouseOffsetThis > 70 )
			m_nMouseOffsetThis = 70 - m_nMouseOffset;
		else if( m_nMouseOffset + m_nMouseOffsetThis < -70 )
			m_nMouseOffsetThis = -70 - m_nMouseOffset;
	}
}

void CDlgHost::OnEventLButtonDown_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetCaptureObject(pObj);
	ChangeFocus(GetDlgItem("Img_BGclose"));
	m_nMouseLastX = GET_X_LPARAM(lParam);
	m_nMouseOffset += m_nMouseOffsetThis;
	m_nMouseOffsetThis = 0;

}

void CDlgHost::OnEventLButtonUp_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetCaptureObject(NULL);
	ChangeFocus(GetDlgItem("Img_BGclose"));
	GetHostPlayer()->SelectTarget(GetHostPlayer()->GetCharacterID());
}

void CDlgHost::OnEventLButtonDBlclk_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	ChangeFocus(GetDlgItem("Img_BGclose"));
}

void CDlgHost::OnEventRButtonDBlclk_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	ChangeFocus(GetDlgItem("Img_BGclose"));
}

void CDlgHost::OnEventRButtonDown_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	ChangeFocus(GetDlgItem("Img_BGclose"));
}

void CDlgHost::OnEventRButtonUp_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	ChangeFocus(GetDlgItem("Img_BGclose"));
	CECTeam *pTeam = GetHostPlayer()->GetTeam();
	if( !pTeam ) return;

	CDlgTeamContext *pMenu;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	if( pTeam->GetLeaderID() == GetHostPlayer()->GetCharacterID() )
		pMenu = GetGameUIMan()->m_pDlgTeamMe1;
	else
		pMenu = GetGameUIMan()->m_pDlgTeamMe2;
	
	// old : pMenu->SetPos(x, y);
	pMenu->SetPosEx(x, y);

	pMenu->Show(true);
}

bool CDlgHost::Render(void)
{
	if( m_szName == "Win_Hpmpxp" )
		RefreshHostStats();
	else if( m_szName == "Win_HideHP")
		RefreshStatusBar();

	return CDlgBase::Render();
}

void CDlgHost::RefreshHostStats()
{
	//Force status
	RefreshForceStatus();

	PAUIDIALOG pDlg = this;
	bool bCheck = ((PAUICHECKBOX)GetGameUIMan()->GetDialog(
		"Win_HideHP")->GetDlgItem("Chk_CloseHP"))->IsChecked();
	if( pDlg->IsShow() == bCheck ) pDlg->Show(!bCheck);
	if( !pDlg->IsShow() ) return;

	ACHAR szText[80];
	CECHostPlayer *pHost = GetHostPlayer();
	const ROLEEXTPROP& ep = pHost->GetExtendProps();
	const ROLEBASICPROP& bp = pHost->GetBasicProps();

	int i;
	int nLevel = bp.iLevel;
	int nHP = bp.iCurHP;
	int nMaxHP = max(ep.bs.max_hp, 1);
	int nMP = bp.iCurMP;
	int nMaxMP = max(ep.bs.max_mp, 1);
	int nAP = max(bp.iCurAP, 0);
	int nMaxAP = max(ep.max_ap, 1);
	int nEXP = bp.iExp;
	int nMaxEXP = max(pHost->GetLevelUpExp(nLevel), 1);

	if( pHost->IsAllResReady() )
	{
		m_pImgSize->SetRenderCallback(PlayerRenderPortrait,
			(DWORD)pHost, m_nMouseOffset + m_nMouseOffsetThis);
	}
	else
		m_pImgSize->SetRenderCallback(NULL, 0);

	if( pHost->IsFighting() )
	{
		m_pImgCombatMask->Show(true);
		m_pImgCombatMask->FadeInOut(1500);
	}
	else
	{
		m_pImgCombatMask->Show(false);
		m_pImgCombatMask->FadeInOut(0);
	}

	AUI_ConvertChatString(pHost->GetName(), szText);
	m_pTxt_Name->SetText(szText);
	m_pTxt_Name->SetColor(pHost->GetNameColor());
	
	m_pTxt_Prof->SetText(GetGameRun()->GetProfName(pHost->GetProfession()));

	a_sprintf(szText, _AL("%d"), nLevel);
	m_pTXT_LV->SetText(szText);

	int nRCount = GetHostPlayer()->GetReincarnationCount();
	if (nRCount > 0){
		if(!m_pImg_Realm->IsShow())
			m_pImg_Realm->Show(true);
		if(!m_pImg_RealmBG->IsShow())
			m_pImg_RealmBG->Show(true);
		m_pImg_Realm->FixFrame(nRCount-1);
		static const PLAYER_REINCARNATION_CONFIG* pConfig = pHost->GetReincarnationConfig();
		if (pConfig && nRCount < sizeof(pConfig->level)/ sizeof(pConfig->level[0]))
		{
			a_sprintf(szText, GetGameUIMan()->GetStringFromTable(11160), nRCount, pConfig->level[nRCount-1].exp_premote + 1.0f);
			m_pImg_RealmBG->SetHint(szText);
		}		
	}
	else
	{
		if(m_pImg_Realm->IsShow())
			m_pImg_Realm->Show(false);
		m_pImg_RealmBG->SetHint(_AL(""));
		if(m_pImg_RealmBG->IsShow())
			m_pImg_RealmBG->Show(false);
	}

	if( nMaxHP > 0 )
		m_pPrgs_HP->SetProgress(int((double)max(nHP, 0) * AUIPROGRESS_MAX / nMaxHP));
	if( nMaxMP > 0 )
		m_pPrgs_MP->SetProgress(int((double)max(nMP, 0) * AUIPROGRESS_MAX / nMaxMP));
	if( nMaxEXP > 0 )
	{
		m_pPrgs_EXP->SetProgress(int((double)max(nEXP, 0) * AUIPROGRESS_MAX / nMaxEXP));
		a_sprintf(szText, _AL("%d/%d"), max(0, nEXP), nMaxEXP);
		m_pPrgs_EXP->SetHint(szText);
	}
	m_pPic_MinusExp->Show(nEXP < 0 ? true : false);

	a_sprintf(szText, _AL("%d/%d"), nHP, nMaxHP);
	m_pTxt_HP->SetText(szText);
	a_sprintf(szText, _AL("%d/%d"), nMP, nMaxMP);
	m_pTxt_MP->SetText(szText);
	a_sprintf(szText, _AL("%4.1f%c"), (double(max(0, nEXP)) * 1000 / nMaxEXP) / 10.0f, '%');
	m_pTXT_EXP->SetText(szText);

	a_sprintf(szText, _AL("%d/%d"), nEXP, nMaxEXP);
	m_pTXT_EXP->SetHint(szText);
	m_pPic_MinusExp->SetHint(szText);

	int nNumOff = nMaxAP / 100;
	for( i = 0; i < (int)m_vecImgAPOff.size(); i++ )
		m_vecImgAPOff[i]->Show(i < nNumOff ? true : false);

	int nNumOn = nAP / 100;
	DWORD dwTick = GetTickCount();
	for( i = 0; i < (int)m_vecImgAPOn.size(); i++ )
	{
		m_vecImgAPOn[i]->Show(i < nNumOn ? true : false);
		if( m_vecImgAPFlash[i]->IsShow() &&
			CECTimeSafeChecker::ElapsedTime(dwTick, m_vecImgAPFlash[i]->GetData()) > 500 )
			m_vecImgAPFlash[i]->Show(false);
	}
	if( nNumOn < m_nLastAP )
	{
		for( i = nNumOn; i < m_nLastAP; i++ )
		{
			m_vecImgAPFlash[i]->Show(true);
			m_vecImgAPFlash[i]->SetData(dwTick);
		}
	}
	m_nLastAP = nNumOn;

	a_sprintf(szText, _AL("%d/%d"), nAP % 100, 100);
	m_pPrgs_AP->SetProgress(AUIPROGRESS_MAX * (nAP % 100) / 100);
	m_pPrgs_AP->SetHint(szText);

	CECTeam *pTeam = pHost->GetTeam();
	if( pTeam && pTeam->GetLeaderID() == pHost->GetCharacterID() )
		m_pImg_Leader->Show(true);
	else
		m_pImg_Leader->Show(false);
	
	CECIconStateMgr* pMgr = GetGameUIMan()->GetIconStateMgr();
	pMgr->RefreshStateIcon( m_vecImgState.begin(), m_vecImgState.size(), &GetHostPlayer()->GetIconStates(),
							IconLayoutBig(m_vecImgState.size() / 2), false );
	
	((PAUICHECKBOX)GetDlgItem("Chk_AutoLock"))->Check(GetGameUIMan()->m_pDlgAutoLock->IsLocked());
	GetDlgItem("Chk_AutoLock")->SetFlash(GetGameUIMan()->m_pDlgAutoLock->IsLocked());
	
	PAUIOBJECT pObjAutoHPMP = GetDlgItem("Btn_AutoHpMp");
	if (pObjAutoHPMP)
		pObjAutoHPMP->Enable(CEComputerAid::Instance().CanUse());
	
	const CECPlayer::PVPINFO &pvp = GetHostPlayer()->GetPVPInfo();	
	EC_GAME_SETTING gs = g_pGame->GetConfigs()->GetGameSettings();
	if( GetGameRun()->GetPVPMode() )
	{
		bool bRed = GetHostPlayer()->IsPariah() || GetHostPlayer()->IsInvader();
		ACString strText;
		if( bRed && GetHostPlayer()->IsPariah() )
			strText.Format(GetStringFromTable(689), GetHostPlayer()->GetPariahTime() / 3600000 + 1);
		m_pChk_SwitchSafety->Check(gs.bAtk_Player);
		m_pChk_SwitchSafety->SetHint(ACString(GetStringFromTable(823)) + strText);
	}
	else
	{
		if( pvp.bEnable )
		{
			const EC_GAME_SETTING &gs = g_pGame->GetConfigs()->GetGameSettings();
			m_pChk_SwitchSafety->Check(gs.bAtk_Player);
		}
		else
		{
			m_pChk_SwitchSafety->Check(false);
		}
	}
	
	EC_VIDEO_SETTING vs = GetGame()->GetConfigs()->GetVideoSettings();
	if(m_pChk_Player)
		m_pChk_Player->Check(vs.bPlayerHeadText);
	if(m_pChk_Surround)
		m_pChk_Surround->Check(vs.bModelLimit);
}

void CDlgHost::RefreshStatusBar()
{
	bool bActive = true;
	if( GetGameUIMan()->GetActiveDialog() && 
		GetGameUIMan()->GetActiveDialog()->GetFocus() &&
		GetGameUIMan()->GetActiveDialog()->GetFocus()->GetType() == AUIOBJECT_TYPE_EDITBOX )
		bActive = false;
	
	// Flight bar.
	RefreshFlightBar(bActive);

	// Oxigen bar.
	RefreshOxigenBar(bActive);

	// Gather bar.
	RefreshGatherBar(bActive);

	// group these bars, show only one at one time
	bool bShow = false;
	// Power bar.
	bShow |= RefreshPowerBar(bShow, bActive);
	// Array bar.
	bShow |= RefreshArrayBar(bShow, bActive);
	// Incant bar.
	bShow |= RefreshIncantBar(bShow, bActive);
	
	if (GetHostPlayer()->GetCurSkill() && !bShow)
	{
		// 传送等方式导致位置切换、技能（阵技能）仍然在释放状态
		//
		GetGameSession()->c2s_CmdCancelAction();
	}

	// AutoHP bar.
	RefreshAutoHPBar(bActive);

	// AutoMP bar.
	RefreshAutoMPBar(bActive);

	// pet status
	RefreshPetStatus();
	
	//ELF status
	RefreshElfStatus();
}

bool CDlgHost::RefreshFlightBar(bool bActive)
{
	CECHostPlayer *pHost = GetHostPlayer();

	const ROLEBASICPROP& bp = pHost->GetBasicProps();
	const ROLEEXTPROP& ep = pHost->GetExtendProps();
	
	int nMP = bp.iCurMP;
	int nMaxMP = max(ep.bs.max_mp, 1);

	CECInventory *pPack = pHost->GetEquipment();
	CECIvtrItem *pItem = pPack->GetItem(EQUIPIVTR_FLYSWORD);
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Prgs");
	if( pHost->IsFlying() && pHost->GetRushFlyFlag() && pItem )
	{
		int nCurTime = 0, nMaxTime = 1, nPercent;
		
		if( !pDlg->IsShow() )
			pDlg->Show(true, false, bActive);
		
		if( pItem->GetClassID() == CECIvtrItem::ICID_FLYSWORD )
		{
			CECIvtrFlySword *pSword = (CECIvtrFlySword *)pItem;
			
			nCurTime = pSword->GetCurTime();
			nMaxTime = max(pSword->GetMaxTime(), 1);
		}
		else
		{
			CECIvtrWing *pWing = (CECIvtrWing *)pItem;
			int nMPPerSec = max(pWing->GetMPPerSecond(), 1);
			
			nCurTime = nMP / nMPPerSec;
			nMaxTime = max(nMaxMP / nMPPerSec, 1);
		}
		
		ACHAR szText[80];
		a_sprintf(szText, _AL("%d/%d"), nCurTime, nMaxTime);
		pDlg->GetDlgItem("Txt_info")->SetText(szText);
		
		PAUIPROGRESS pProgress = (PAUIPROGRESS)pDlg->GetDlgItem("Prgs");
		pProgress->SetProgress(AUIPROGRESS_MAX * nCurTime / nMaxTime);
		
		nPercent = nCurTime * 100 / nMaxTime;
		if( nPercent <= 2 )
			pDlg->Blink(150);
		else if( nPercent <= 5 )
			pDlg->Blink(250);
		else if( nPercent <= 10 )
			pDlg->Blink(500);
		else
			pDlg->Blink(0);
	}
	else
	{
		pDlg->Blink(0);
		if( pDlg->IsShow() )
			pDlg->Show(false);
	}

	return pDlg->IsShow();
}

bool CDlgHost::RefreshOxigenBar(bool bActive)
{
	CECHostPlayer *pHost = GetHostPlayer();
	const CECHostPlayer::BREATHDATA &bd = pHost->GetBreathData();
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Prgs1");
	if( bd.bDiving )
	{
		int nCurTime = bd.iCurBreath, nMaxTime = max(bd.iMaxBreath, 1), nPercent;
		
		if( !pDlg->IsShow() )
			pDlg->Show(true, false, bActive);
		
		PAUIPROGRESS pProgress = (PAUIPROGRESS)pDlg->GetDlgItem("Prgs");
		pProgress->SetProgress(AUIPROGRESS_MAX * nCurTime / nMaxTime);
		
		nPercent = nCurTime * 100 / nMaxTime;
		if( nPercent <= 2 )
			pDlg->Blink(150);
		else if( nPercent <= 5 )
			pDlg->Blink(250);
		else if( nPercent <= 10 )
			pDlg->Blink(500);
		else
			pDlg->Blink(0);
		
		ACHAR szText[80];
		a_sprintf(szText, _AL("%d%c"), nPercent, '%');
		pDlg->GetDlgItem("Txt_info")->SetText(szText);
	}
	else
	{
		pDlg->Blink(0);
		if( pDlg->IsShow() )
			pDlg->Show(false);
	}

	return pDlg->IsShow();
}

bool CDlgHost::RefreshGatherBar(bool bActive)
{
	CECHostPlayer *pHost = GetHostPlayer();

	CECCounter counter = pHost->GetGatherCnt();
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Prgs2");
	if( pHost->IsGathering() )
	{
		int nCur = counter.GetCounter();
		int nMax = max(counter.GetPeriod(), 1);
		
		if( !pDlg->IsShow() )
			pDlg->Show(true, false, bActive);
		
		PAUIPROGRESS pProgress = (PAUIPROGRESS)pDlg->GetDlgItem("Prgs");
		PAUILABEL pLabel = (PAUILABEL)pDlg->GetDlgItem("1");
		ACString percent;
		percent.Format(_AL("%d%%"), AUIPROGRESS_MAX * nCur / nMax);
		pLabel->SetText(percent);
		pProgress->SetProgress(AUIPROGRESS_MAX * nCur / nMax);
	}
	else
	{
		if( pDlg->IsShow() )
			pDlg->Show(false);
	}

	return pDlg->IsShow();
}

bool CDlgHost::RefreshPowerBar(bool bOtherShown, bool bActive)
{
	CECHostPlayer *pHost = GetHostPlayer();
	CECSkill *pCurSkill = pHost->GetCurSkill();

	bool bShow = false;
	
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("MagicProgress2");
	if( pCurSkill && pCurSkill->IsCharging() && !bOtherShown )
	{
		bShow = true;
		int nCur = pCurSkill->GetChargingCnt();
		int nMax = max(pCurSkill->GetChargingMax(), 1);
		
		if( !pDlg->IsShow() )
			pDlg->Show(true, false, bActive);
		
		PAUIPROGRESS pProgress = (PAUIPROGRESS)pDlg->GetDlgItem("Prgs");
		pProgress->SetProgress(AUIPROGRESS_MAX * nCur / nMax);
		
		ACString strText;
		strText.Format(GetStringFromTable(605), g_pGame->GetSkillDesc()->GetWideString(pCurSkill->GetSkillID() * 10));
		pDlg->GetDlgItem("Txt")->SetText(strText);
	}
	else
	{
		if( pDlg->IsShow() )
			pDlg->Show(false);
	}

	return bShow;
}

bool CDlgHost::RefreshArrayBar(bool bOtherShown, bool bActive)
{
	CECHostPlayer *pHost = GetHostPlayer();
	CECSkill *pCurSkill = pHost->GetCurSkill();

	const ROLEBASICPROP& bp = pHost->GetBasicProps();
	const ROLEEXTPROP& ep = pHost->GetExtendProps();
	
	int nMP = bp.iCurMP;
	int nMaxMP = max(ep.bs.max_mp, 1);

	bool bShow = false;

	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("MagicProgress3");
	if( pHost->IsSpellingDurativeSkill() && pCurSkill && !bOtherShown )
	{
		bShow = true;
		int nCur = nMP;
		int nMax = nMaxMP;
		
		if( !pDlg->IsShow() )
			pDlg->Show(true, false, bActive);
		
		PAUIPROGRESS pProgress = (PAUIPROGRESS)pDlg->GetDlgItem("Prgs");
		pProgress->SetProgress(AUIPROGRESS_MAX * nCur / nMax);
		
		ACString strText;
		strText.Format(GetStringFromTable(606), g_pGame->GetSkillDesc()->GetWideString(pCurSkill->GetSkillID() * 10));
		pDlg->GetDlgItem("Txt")->SetText(strText);
	}
	else
	{
		if( pDlg->IsShow() )
			pDlg->Show(false);
	}

	return bShow;
}

bool CDlgHost::RefreshIncantBar(bool bOtherShown, bool bActive)
{
	CECHostPlayer *pHost = GetHostPlayer();
	CECSkill *pCurSkill = pHost->GetCurSkill();

	bool bShow = false;
	int nCur = 1;
	int nMax = 1;

	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("MagicProgress1");
	if(!bOtherShown)
	{
		DWORD dwCurTime = 0, dwMaxTime = 0;
		if( pHost->GetUsingItemTimeCnt(dwCurTime, dwMaxTime) )
		{
			bShow = true;
			nCur = dwCurTime;
			nMax = dwMaxTime;
			pDlg->GetDlgItem("Txt")->SetText(pHost->IsGatheringMonsterSpirit() ? 
				GetStringFromTable(738) : GetStringFromTable(726));
		}
		else if( pHost->IsOperatingPet() )
		{
			bShow = true;
			nCur = pHost->GetPetOptTime().GetCounter();
			nMax = max(pHost->GetPetOptTime().GetPeriod(), 1);
			switch(pHost->IsOperatingPet())
			{
			case 1:
				pDlg->GetDlgItem("Txt")->SetText(GetStringFromTable(791));
				break;
			case 2:
				pDlg->GetDlgItem("Txt")->SetText(GetStringFromTable(792));
				break;
			case 3:
				pDlg->GetDlgItem("Txt")->SetText(GetStringFromTable(793));
				break;
			case 4:
				pDlg->GetDlgItem("Txt")->SetText(GetStringFromTable(794));
				break;
			}
		}
		else if( pHost->IsSpellingMagic() && pCurSkill )
		{
			bShow = true;
			CECCounter counter = pHost->GetIncantCnt();
			nCur = counter.GetCounter();
			nMax = max(counter.GetPeriod(), 1);
			pDlg->GetDlgItem("Txt")->SetText(g_pGame->GetSkillDesc()->GetWideString(pCurSkill->GetSkillID() * 10));
		}
		else if( pHost->IsCongregating() )
		{
			bShow = true;
			CECCounter counter = pHost->GetCongregateCnt();
			nCur = counter.GetCounter();
			nMax = max(counter.GetPeriod(), 1);

			int conType = pHost->IsCongregating() - 1;
			pDlg->GetDlgItem("Txt")->SetText(GetStringFromTable(5700+conType));
		}
	}

	if(bShow)
	{
		if( !pDlg->IsShow() )
			pDlg->Show(true, false, bActive);
		
		PAUIPROGRESS pProgress = (PAUIPROGRESS)pDlg->GetDlgItem("Prgs");
		pProgress->SetProgress(AUIPROGRESS_MAX * nCur / nMax);
	}
	else
	{
		if( pDlg->IsShow() )
			pDlg->Show(false);
	}

	return bShow;
}

bool CDlgHost::RefreshAutoHPBar(bool bActive)
{
	CECHostPlayer *pHost = GetHostPlayer();

	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_PrgsHP");
	CECIvtrAutoHP* pAutoHPItem = (CECIvtrAutoHP*)pHost->GetEquipment()->GetItem(EQUIPIVTR_AUTOHP);
	if( pAutoHPItem )
	{
		if( !pDlg->IsShow() )
			pDlg->Show(true, false, bActive);
		
		int nCur = pAutoHPItem->GetDBEssence()->cool_time - pAutoHPItem->GetCoolTime();
		int nMax = pAutoHPItem->GetDBEssence()->cool_time;
		PAUIPROGRESS pProgress = (PAUIPROGRESS)pDlg->GetDlgItem("Prgs_HPCD");
		if( nMax == 0 )
			pProgress->SetProgress(AUIPROGRESS_MAX);
		else
			pProgress->SetProgress(AUIPROGRESS_MAX * nCur / nMax);
		AUICTranslate trans;
		if( pAutoHPItem->GetDesc() )
			pProgress->SetHint(trans.Translate(pAutoHPItem->GetDesc()));
		else
			pProgress->SetHint(_AL(""));
		
		nCur = pAutoHPItem->GetEssence().hp_left;
		nMax = pAutoHPItem->GetDBEssence()->total_hp;
		pProgress = (PAUIPROGRESS)pDlg->GetDlgItem("Prgs_HPLeft");
		if( nMax > 0 )
			pProgress->SetProgress(AUIPROGRESS_MAX * nCur / nMax);
		
		ACHAR szText[80];
		PAUILABEL pLabel = (PAUILABEL)pDlg->GetDlgItem("Txt_HPLeft");
		a_sprintf(szText, _AL("%d"), nCur);
		pLabel->SetText(szText);
	}
	else
	{
		if( pDlg->IsShow() )
			pDlg->Show(false);
	}

	return pDlg->IsShow();
}

bool CDlgHost::RefreshAutoMPBar(bool bActive)
{
	CECHostPlayer *pHost = GetHostPlayer();

	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_PrgsMP");
	CECIvtrAutoMP* pAutoMPItem = (CECIvtrAutoMP*)pHost->GetEquipment()->GetItem(EQUIPIVTR_AUTOMP);
	if( pAutoMPItem )
	{
		if( !pDlg->IsShow() )
			pDlg->Show(true, false, bActive);
		
		int nCur = pAutoMPItem->GetDBEssence()->cool_time - pAutoMPItem->GetCoolTime();
		int nMax = pAutoMPItem->GetDBEssence()->cool_time;
		PAUIPROGRESS pProgress = (PAUIPROGRESS)pDlg->GetDlgItem("Prgs_MPCD");
		if( nMax == 0 )
			pProgress->SetProgress(AUIPROGRESS_MAX);
		else
			pProgress->SetProgress(AUIPROGRESS_MAX * nCur / nMax);
		AUICTranslate trans;
		if( pAutoMPItem->GetDesc() )
			pProgress->SetHint(trans.Translate(pAutoMPItem->GetDesc()));
		else
			pProgress->SetHint(_AL(""));
		
		nCur = pAutoMPItem->GetEssence().mp_left;
		nMax = pAutoMPItem->GetDBEssence()->total_mp;
		pProgress = (PAUIPROGRESS)pDlg->GetDlgItem("Prgs_MPLeft");
		if( nMax > 0 )
			pProgress->SetProgress(AUIPROGRESS_MAX * nCur / nMax);
		
		ACHAR szText[80];
		PAUILABEL pLabel = (PAUILABEL)pDlg->GetDlgItem("Txt_MPLeft");
		a_sprintf(szText, _AL("%d"), nCur);
		pLabel->SetText(szText);
	}
	else
	{
		if( pDlg->IsShow() )
			pDlg->Show(false);
	}

	return pDlg->IsShow();
}

void CDlgHost::RefreshPetStatus()
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	CECPetData * pPet = pPetCorral->GetActivePet();
	char szDlg[20];
	bool bPetDlgShow = false;
	CDlgHostPet *pDlgPet = NULL;
	CDlgQuickBarPet *pDlgQuickBarPet = GetGameUIMan()->m_pDlgQuickBarPet;
	if( pPet && pPet->GetClass() >= 0 )
	{
		for(int i = 0; i < GP_PET_CLASS_MAX; i++ )
			if(i != pPet->GetClass() )
			{
				sprintf(szDlg, "Win_HpmpxpPet%d", i);
				pDlgPet = (CDlgHostPet *)GetGameUIMan()->GetDialog(szDlg);
				if( pDlgPet )
					pDlgPet->Show(false);
			}
			sprintf(szDlg, "Win_HpmpxpPet%d", pPet->GetClass());
			pDlgPet = (CDlgHostPet *)GetGameUIMan()->GetDialog(szDlg);
			if( pDlgPet )
			{
				if( GetGameUIMan()->m_pDlgHost->IsShow() )
				{
					if (!pDlgPet->IsShow())
					{
						pDlgPet->Show(true);
					}
				}
				else
					pDlgPet->Show(false);
			}
			if( pPet->GetClass() == GP_PET_CLASS_COMBAT || pPet->GetClass() == GP_PET_CLASS_SUMMON || pPet->GetClass() == GP_PET_CLASS_EVOLUTION )
			{
				if( !pDlgQuickBarPet->IsShow() )
				{
					pDlgQuickBarPet->m_nAutoCastID = 0;
					pDlgQuickBarPet->Show(true);
				}
			}
			else
				pDlgQuickBarPet->Show(false);
			bPetDlgShow = (pDlgPet && pDlgPet->IsShow()) ? true : false;
	}
	else if (pDlgQuickBarPet->IsShow()){
		pDlgQuickBarPet->Show(false);
	}
	
	//	植物宠面板单独处理
	PAUIDIALOG pDlgPlantPet = GetGameUIMan()->GetDialog("Win_PlantPet");
	if (pDlgPlantPet)
	{
		bool bShow = (pPetCorral->GetPlantCount() > 0);
		if (pDlgPlantPet->IsShow() != bShow)
			pDlgPlantPet->Show(bShow);
	}
}

void CDlgHost::RefreshElfStatus()
{
	CECInventory *pInventory = GetHostPlayer()->GetEquipment();
	CECIvtrItem *pELF = pInventory->GetItem(23);
	CDlgHostELF *pDlgELF = (CDlgHostELF *)GetGameUIMan()->GetDialog("Win_ELF");
	CDlgHostELF *pDlgELFZoom = (CDlgHostELF *)GetGameUIMan()->GetDialog("Win_ELFZoom");

	bool bInChariotWar = GetHostPlayer()->GetBattleInfo().IsChariotWar();

	if (pELF != NULL)
	{
		// 战车战场里不显示精灵头像界面
		if (!pDlgELFZoom->IsShow() && !pDlgELF->IsShow() && !bInChariotWar)
		{			
			pDlgELF->Show(true);
		}
	}
	else
	{
		if (pDlgELF->IsShow()){
			pDlgELF->Show(false);
		}
		if (pDlgELFZoom->IsShow()){
			pDlgELFZoom->Show(false);
		}
	}

	// 战车战场里不显示精灵头像界面
	if(bInChariotWar && pDlgELF->IsShow())
			pDlgELF->Show(false);
}

void CDlgHost::RefreshForceStatus()
{
	// get the icon
	PAUIIMAGEPICTURE pImgForce = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Force"));
	if(!pImgForce)
		return;
	
	// check whether we need update
	int forceID = GetHostPlayer()->GetForce();
	int lastForceID = pImgForce->GetData();
	if(forceID != lastForceID)
	{
		const FORCE_CONFIG* pConfig = NULL;
		if(forceID)
		{
			pConfig = CECForceMgr::GetForceData(forceID);
		}

		ACString strHint;
		A2DSprite* pSprite = NULL;
		// try to load the flag icon
		if(pConfig)
		{
			CECForceMgr* pMgr = GetHostPlayer()->GetForceMgr();
			if(pMgr) pSprite = pMgr->GetForceIcon(m_pA3DDevice, pConfig, CECForceMgr::FORCE_ICON_NORMAL);
			strHint = pConfig->name;
		}
		pImgForce->SetCover(pSprite, 0);
		pImgForce->SetData(forceID);
		pImgForce->SetHint(strHint);
	}
}

// =======================================================================
// IconStateDefault
// =======================================================================
#define ICON_SIZE_SMALL 16
#define ICON_SIZE_BIG 32
class IconStateDefault : public IconStateUpdater
{
public:
	IconStateDefault(int id = -1, int priority = PRIORITY_NORMAL)
		:IconStateUpdater(id, priority)
		,m_Width(ICON_SIZE_SMALL) ,m_Height(ICON_SIZE_SMALL)
	{}

	virtual SIZE Update(const S2C::IconState& state, int x, int y, PAUIIMAGEPICTURE pIcon)
	{
		SIZE ret = {0, 0};

		const GNET::TeamState *pTS = GNET::TeamState::Query(state.id);
		if( !pTS ) return ret;

		AString strFile;
		af_GetFileTitle(pTS->GetIcon(), strFile);
		strFile.MakeLower();

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pIcon->SetCover(
			pGameUI->GetIconCover(CECGameUIMan::ICONS_STATE),
			pGameUI->GetIconIndex(CECGameUIMan::ICONS_STATE, strFile) );

		pIcon->SetHint(g_pGame->GetBuffDesc(pTS->GetID()));
		pIcon->Show(true);

		// hide the append object
		PAUIOBJECT pObj = (PAUIOBJECT)pIcon->GetDataPtr("AUIObject");
		if(pObj) pObj->Show(false);

		// clear the clock icon
		AUIClockIcon* pClock = pIcon->GetClockIcon();
		pClock->SetProgressRange(0, 1);
		pClock->SetProgressPos(1);

		pIcon->SetPos(x, y);
		pIcon->SetDefaultSize(m_Width, m_Height);
		return pIcon->GetSize();
	}

protected:
	int m_Width;
	int m_Height;
};
// =======================================================================
// IconStateBigIcon
// =======================================================================
class IconStateBigIcon : public IconStateDefault
{
public:
	IconStateBigIcon(int id)
		:IconStateDefault(id, IconStateUpdater::PRIORITY_BIG)
	{
		m_Width = ICON_SIZE_BIG;
		m_Height = ICON_SIZE_BIG;
	}
};
// =======================================================================
// IconStateBigIcon2IntegersRange 整型最大值最小值效果图标
// =======================================================================
class IconStateBigIcon2IntegersRange : public IconStateBigIcon
{
	DWORD	m_dwColor;
public:
	IconStateBigIcon2IntegersRange(int id, DWORD dwColor)
		: IconStateBigIcon(id)
		, m_dwColor(dwColor)
	{}

	virtual SIZE Update(const S2C::IconState& state, int x, int y, PAUIIMAGEPICTURE pIcon)
	{
		SIZE sz = IconStateBigIcon::Update(state, x, y, pIcon);

		int curVal = state.param[0];
		int maxVal = state.param[1];
		int curShowVal = curVal > 100 ? (curVal / 100) * 100 : curVal;
		
		// show the text on binded object
		PAUIOBJECT pObj = (PAUIOBJECT)pIcon->GetDataPtr("AUIObject");
		PAUILABEL pTxt = dynamic_cast<PAUILABEL>(pObj);
		if(pTxt)
		{
			ACString strNum;
			pTxt->Show(true);
			pTxt->SetText(strNum.Format(_AL("%d"), curShowVal));
			POINT pos = pIcon->GetPos(true);
			pTxt->SetPos(pos.x + sz.cy / 2, pos.y + sz.cy);
		}

		AUIClockIcon* pClock = pIcon->GetClockIcon();
		pClock->SetClockType(AUICLOCK_DOWN);
		pClock->SetProgressRange(0, maxVal + 1);
		pClock->SetProgressPos(min(curVal, maxVal));
		pClock->SetColor(m_dwColor);

		return sz;
	}
};
// =======================================================================
// IconStateBigIcon1Integer 单独整型值效果图标
// =======================================================================
class IconStateBigIcon1Integer : public IconStateBigIcon
{
public:
	IconStateBigIcon1Integer(int id)
		: IconStateBigIcon(id)
	{}
	
	virtual SIZE Update(const S2C::IconState& state, int x, int y, PAUIIMAGEPICTURE pIcon)
	{
		SIZE sz = IconStateBigIcon::Update(state, x, y, pIcon);
				
		// show the text on binded object
		PAUIOBJECT pObj = (PAUIOBJECT)pIcon->GetDataPtr("AUIObject");
		PAUILABEL pTxt = dynamic_cast<PAUILABEL>(pObj);
		if(pTxt)
		{
			ACString strNum;
			pTxt->Show(true);
			pTxt->SetText(strNum.Format(_AL("%d"), state.param[0]));
			POINT pos = pIcon->GetPos(true);
			pTxt->SetPos(pos.x + sz.cy / 2, pos.y + sz.cy);
		}
		
		return sz;
	}
};

// =======================================================================
// CECIconStateMgr
// =======================================================================
CECIconStateMgr::CECIconStateMgr()
{
	// here we register new icon update type
	RegisterUpdater(new IconStateBigIcon2IntegersRange(203, A3DCOLORRGBA(228, 0, 127, 128)));	//	吸收治疗状态效果
	RegisterUpdater(new IconStateBigIcon1Integer(280));											//	霜雷术状态效果
}

CECIconStateMgr::~CECIconStateMgr()
{
	ClearUpdater();
}

// get the updater pointer
IconStateUpdater* CECIconStateMgr::GetUpdater(int id)
{
	if(m_UpdaterMap.empty())
		return NULL;

	abase::hash_map<int, IconStateUpdater*>::iterator itr = m_UpdaterMap.find(id);
	return itr != m_UpdaterMap.end() ? itr->second : NULL;
}

// updater pointer will be stored here
void CECIconStateMgr::RegisterUpdater(IconStateUpdater* pUpdater)
{
	if(pUpdater)
	{
		int id = pUpdater->GetID();
		m_UpdaterMap[id] = pUpdater;
	}
}

// release all updater pointer
void CECIconStateMgr::ClearUpdater()
{
	abase::hash_map<int, IconStateUpdater*>::iterator itr = m_UpdaterMap.begin();
	for(; itr != m_UpdaterMap.end(); ++itr)
	{
		IconStateUpdater* p = itr->second;
		delete p;
	}
	m_UpdaterMap.clear();
}

static bool FindState287(const S2C::IconState &state){
	return state.id == 287;
}

// refresh the state icon by different updater
void CECIconStateMgr::RefreshStateIcon(PAUIIMAGEPICTURE* pIcons, int size, const S2C::IconStates* pStates, 
									   IconLayout& layout, bool defaultOnly)
{
	if(!pIcons || !size )
	{
		ASSERT(false);
		return;
	}

	if(!pStates || pStates->empty())
	{
		// for clear only
		for(int i = 0; i < size; i++ )
		{
			if(!pIcons[i]) continue;
			pIcons[i]->Show(false);
			PAUIOBJECT pObj = (PAUIOBJECT)pIcons[i]->GetDataPtr("AUIObject");
			if(pObj) pObj->Show(false);
		}
	}
	else
	{
		// for normal update
		IconStateDefault defaultUpdater;

		// record the start point
		POINT startPos;
		if(pIcons[0])
		{
			// start from 1st icon
			startPos = pIcons[0]->GetPos(true);
		}
		else
		{
			// should not have void pointer
			ASSERT(false);
			memset(&startPos, 0, sizeof(POINT));
		}
	
		// 删除冰雷球显示 287
		S2C::IconStates sorted = *pStates;
		sorted.erase(std::remove_if(sorted.begin(), sorted.end(), FindState287), sorted.end());

		// sort if necessary
		if(sorted.size() > 1){
			std::sort(sorted.begin(), sorted.end(), IconStateUpdater::Less(*this));
		}
		const S2C::IconStates* pSortedState = &sorted;

		// start to update all icons
		POINT pos = startPos;
		for(int i = 0; i < size; i++ )
		{
			// should not have void pointer
			if(!pIcons[i])
			{
				ASSERT(false);
				continue;
			}

			if(i < (int)pSortedState->size())
			{
				const S2C::IconState& state = (*pSortedState)[i];

				// update by specific updater or default updater
				IconStateUpdater* pUpdater = defaultOnly ? &defaultUpdater : GetUpdater(state.id);
				if(!pUpdater) pUpdater = &defaultUpdater;

				SIZE size = pUpdater->Update(state, pos.x, pos.y, pIcons[i]);
				if(size.cx == 0 || size.cy == 0)
				{
					// should not have invalid state id
					ASSERT(false);
					continue;
				}

				// adjust the layout
				pos = layout(i, startPos, pIcons[i]);
			}
			else
			{
				pIcons[i]->Show(false);
				PAUIOBJECT pObj = (PAUIOBJECT)pIcons[i]->GetDataPtr("AUIObject");
				if(pObj) pObj->Show(false);
			}
		}
	}
}

// layout the icon by vertical
POINT IconLayoutVertical::operator()(int index, POINT start, PAUIIMAGEPICTURE pIcon)
{
	POINT next;

	POINT current = pIcon->GetPos(true);
	SIZE size = pIcon->GetSize();

	if((index + 1) % _limit == 0)
	{
		next.x = current.x + size.cx;
		next.y = start.y;
	}
	else
	{
		next.x = current.x;
		next.y = current.y + size.cy;
	}

	return next;
}
// layout the icon by horizontal
POINT IconLayoutHorizontal::operator()(int index, POINT start, PAUIIMAGEPICTURE pIcon)
{
	POINT next;

	POINT current = pIcon->GetPos(true);
	SIZE size = pIcon->GetSize();

	// normal logic for
	if((index + 1) % _limit == 0)
	{
		next.x = start.x;
		next.y = current.y + size.cy;
	}
	else
	{
		next.x = current.x + size.cx;
		next.y = current.y;
	}

	return next;
}
// layout the big icons with small icon
POINT IconLayoutBig::operator()(int index, POINT start, PAUIIMAGEPICTURE pIcon)
{
	// update the last big icon index

	POINT current = pIcon->GetPos(true);
	SIZE size = pIcon->GetSize();

	SIZE defaultSize = pIcon->GetDefaultSize();
	if (defaultSize.cx == ICON_SIZE_BIG)
	{
		_lastBigIcon = index;
	}

	POINT next;

	if(_lastBigIcon < 0)
	{
		// use original logic
		next = IconLayoutHorizontal::operator()(index, start, pIcon);
	}
	else if(_lastBigIcon == index)
	{
		next.x = current.x + size.cx;
		next.y = current.y;
	}
	else
	{
		const int row = ICON_SIZE_BIG / ICON_SIZE_SMALL;

		// found small icon after the big icons
		int nextIndex = index - _lastBigIcon;
		if(nextIndex % row == 0)
		{
			next.x = current.x + size.cx;
			next.y = start.y;
		}
		else
		{
			next.x = current.x;
			next.y = current.y + size.cy;
		}
	}

	return next;
}

static A3DCOLOR GetParallelWorldLoadColor(float load)
{
	A3DCOLOR clr = 0;
	if (load < 0.5f){
		clr = A3DCOLORRGB(0, 255, 0);
	}else if (load < 0.75f){
		clr = A3DCOLORRGB(255, 255, 0);
	}else{
		clr = A3DCOLORRGB(255, 0, 0);
	}
	return clr;
}

void CDlgHost::UpdateParallelWorldUI()
{
	if (!m_pCombo_Line){
		return;
	}
	m_pCombo_Line->ResetContent();
	CECWorld *pWorld = GetWorld();
	if (pWorld->GetParallelWorldCount() == 0){
		if (m_pCombo_Line->IsShow()){
			m_pCombo_Line->Show(false);
		}
	}else{
		if (!m_pCombo_Line->IsShow()){
			m_pCombo_Line->Show(true);
		}
		CECInstance *pInst = GetGameRun()->GetInstance(pWorld->GetInstanceID());
		ACString strFormat = GetStringFromTable(10702);
		ACString strTemp;
		int i(0);
		for (i = 0; i < pWorld->GetParallelWorldCount(); ++ i)
		{
			int iParallelWorldID = pWorld->GetParallelWorldID(i);
			strTemp.Format(strFormat, pInst->GetName(), iParallelWorldID);
			int index = m_pCombo_Line->AddString(strTemp)-1;
			m_pCombo_Line->SetItemHint(index, strTemp);
			m_pCombo_Line->SetItemData(index, iParallelWorldID);

			float load = pWorld->GetParallelWorldLoad(i);
			m_pCombo_Line->SetItemTextColor(index, GetParallelWorldLoadColor(load));
		}
		for (i = 0; i < m_pCombo_Line->GetCount(); ++ i)
		{
			if (m_pCombo_Line->GetItemData(i) == pWorld->GetCurParallelWorld()){
				m_pCombo_Line->SetCurSel(i);
				break;
			}
		}
	}

	if (pWorld->GetCurParallelWorld() > 0 && pWorld->GetParallelWorldCount() == 0){
		//	没取到分线信息时，马上更新
		m_cntQueryParallelWorld.Reset(true);
	}
}

void CDlgHost::OnCommand_Line(const char *szCommand)
{
	int iCurSel = m_pCombo_Line->GetCurSel();
	if (iCurSel < 0 || iCurSel >= m_pCombo_Line->GetCount()){
		return;
	}
	int iParallelWorldID = (int)m_pCombo_Line->GetItemData(iCurSel);
	CECWorld *pWorld = GetWorld();
	if (iParallelWorldID == pWorld->GetCurParallelWorld()){
		return;
	}
	//	等待成功返回后再修改显示
	for (int i(0); i < m_pCombo_Line->GetCount(); ++ i)
	{
		if (m_pCombo_Line->GetItemData(i) == pWorld->GetCurParallelWorld()){
			m_pCombo_Line->SetCurSel(i);
			break;
		}
	}
	CECHostPlayer *pHostPlayer = GetHostPlayer();
	if (pHostPlayer->GetCoolTime(GP_CT_SWITCH_PARALLEL_WORLD)){
		GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
		return;
	}
	if (!pHostPlayer->CanSwitchParallelWorld()){
		return;
	}
	GetGameSession()->c2s_CmdSwitchParallelWorld(iParallelWorldID);
}

void CDlgHost::OnCommand_safe(const char * szCommand)
{
	CDlgPKSetting *pDlg = GetGameUIMan()->m_pDlgPKSetting;
	pDlg->Show(!pDlg->IsShow());
}

void CDlgHost::OnTick()
{
	if (!m_pCombo_Line){
		return;
	}
	CECWorld *pWorld = GetWorld();
	if (pWorld && pWorld->GetCurParallelWorld() > 0){
		CECHostPlayer *pHostPlayer = GetHostPlayer();
		if (!pHostPlayer->GetCoolTime(GP_CT_QUERY_PARALLEL_WORLD)){
			if (m_cntQueryParallelWorld.IncCounter(GetGame()->GetRealTickTime())){
				m_cntQueryParallelWorld.Reset();
				g_pGame->GetGameSession()->c2s_CmdQueryParallelWorld();
			}
		}
	}
}

void CDlgHost::Resize(A3DRECT rcOld, A3DRECT rcNew){
	//	窗口拖动到最小，然后拖大，经AUInterface界面库操作后，会导致从（0，0）左上角跑到其它位置，且不可拖动，严重影响操作
	//	故临时按以下方法修复
	bool xIsZero = (m_x == 0);
	bool yIsZero = (m_y == 0);
	CDlgBase::Resize(rcOld, rcNew);
	if (xIsZero || yIsZero){
		SetPosEx(xIsZero ? 0 : m_x, yIsZero ? 0 : m_y);
	}
}

void CDlgHost::OnCommand_sight(const char * szCommand)
{
	PAUICHECKBOX pCheck = (PAUICHECKBOX)GetDlgItem("Chk_Sight");
	

	EC_SYSTEM_SETTING m_ss = g_pGame->GetConfigs()->GetSystemSettings();
	EC_VIDEO_SETTING m_vs = g_pGame->GetConfigs()->GetVideoSettings();
	
	static EC_SYSTEM_SETTING m_ssTemp = g_pGame->GetConfigs()->GetSystemSettings();
	static EC_VIDEO_SETTING m_vsTemp = g_pGame->GetConfigs()->GetVideoSettings();

	if( pCheck->IsChecked() )
	{
		m_ssTemp = g_pGame->GetConfigs()->GetSystemSettings();
		m_vsTemp = g_pGame->GetConfigs()->GetVideoSettings();
		
		m_ss.nLevel = 0;
		m_ss.nSight = 0;
		m_ss.nWaterEffect = 0;
		m_ss.nTreeDetail = 0;
		m_ss.nGrassDetail = 0;
		m_ss.nCloudDetail = 0;
		m_ss.bShadow = false;
		m_ss.bMipMapBias = false;
		m_ss.bFullGlow = false;
		m_ss.bSimpleTerrain = true;
		m_ss.bSpaceWarp = false;
		m_ss.bSunFlare = false;
		m_ss.bVSync = false;
		
		m_vs.bShowCustomize = false;
		m_vs.bModelLimit = true;
		m_vs.nDistance = 0;
		
		g_pGame->GetConfigs()->SetSystemSettings(m_ss);
		g_pGame->GetConfigs()->SetVideoSettings(m_vs);
	}
	else
	{
		EC_SYSTEM_SETTING ssTemp = m_ss;
		EC_VIDEO_SETTING vsTemp = m_vs;
		
		ssTemp.nLevel = m_ssTemp.nLevel;
		ssTemp.nSight = m_ssTemp.nSight;
		ssTemp.nWaterEffect = m_ssTemp.nWaterEffect;
		ssTemp.nTreeDetail = m_ssTemp.nTreeDetail;
		ssTemp.nGrassDetail = m_ssTemp.nGrassDetail;
		ssTemp.nCloudDetail = m_ssTemp.nCloudDetail;
		ssTemp.bShadow = m_ssTemp.bShadow;
		ssTemp.bMipMapBias = m_ssTemp.bMipMapBias;
		ssTemp.bFullGlow = m_ssTemp.bFullGlow;
		ssTemp.bSimpleTerrain = m_ssTemp.bSimpleTerrain;
		ssTemp.bSpaceWarp = m_ssTemp.bSpaceWarp;
		ssTemp.bSunFlare = m_ssTemp.bSunFlare;
		ssTemp.bVSync = m_ssTemp.bVSync;
		
		vsTemp.bShowCustomize = m_vsTemp.bShowCustomize;
		vsTemp.bModelLimit = m_vsTemp.bModelLimit;
		vsTemp.nDistance = m_vsTemp.nDistance;
		
		g_pGame->GetConfigs()->SetSystemSettings(ssTemp);
		g_pGame->GetConfigs()->SetVideoSettings(vsTemp);
	}
	GetGameUIMan()->m_pDlgSettingCurrent->UpdateView();
}

void CDlgHost::OnCommand_player(const char * szCommand)
{
	EC_VIDEO_SETTING m_vs = g_pGame->GetConfigs()->GetVideoSettings();
	m_vs.bPlayerHeadText = !m_vs.bPlayerHeadText;
	g_pGame->GetConfigs()->SetVideoSettings(m_vs);
	GetGameUIMan()->m_pDlgSettingCurrent->UpdateView();
}
void CDlgHost::OnCommand_surround(const char * szCommand)
{
	EC_VIDEO_SETTING m_vs = g_pGame->GetConfigs()->GetVideoSettings();
	m_vs.bModelLimit = !m_vs.bModelLimit;
	g_pGame->GetConfigs()->SetVideoSettings(m_vs);
	GetGameUIMan()->m_pDlgSettingCurrent->UpdateView();
}

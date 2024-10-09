// File		: DlgArrangeTeam.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/17

#include "DlgArrangeTeam.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_World.h"
#include "EC_HostPlayer.h"
#include "EC_Team.h"
#include "EC_ElsePlayer.h"
#include "EC_ManPlayer.h"
#include "EC_Faction.h"
#include "EC_Configs.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgArrangeTeam, CDlgBase)

AUI_ON_COMMAND("kick",			OnCommandKick)
AUI_ON_COMMAND("invite",		OnCommandInvite)
AUI_ON_COMMAND("leave",			OnCommandLeave)
AUI_ON_COMMAND("disband",		OnCommandLeave)
AUI_ON_COMMAND("random",		OnCommandRandom)
AUI_ON_COMMAND("free",			OnCommandRandom)
AUI_ON_COMMAND("slogan",		OnCommandSlogan)
AUI_ON_COMMAND("showself",		OnCommandShowSelf)
AUI_ON_COMMAND("lfg",			OnCommandLfg)
AUI_ON_COMMAND("lfm",			OnCommandLfg)
AUI_ON_COMMAND("makeleader",	OnCommandMakeLeader)
AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("refresh",		OnCommandRefresh)
AUI_ON_COMMAND("Chk_AutoAccept",OnCommandAcceptAutoTeam)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgArrangeTeam, CDlgBase)

AUI_ON_EVENT("Lst_Nearbypp",	WM_LBUTTONUP,	OnEventLButtonUp)
AUI_ON_EVENT("Lst_Nearbypp",	WM_RBUTTONUP,	OnEventRButtonUp)

AUI_END_EVENT_MAP()

static const char * CheckList_Prof[NUM_PROFESSION] = {"Chk_ProfWX", "Chk_ProfFS", "Chk_ProfWS", "Chk_ProfYJ", "Chk_ProfYS", "Chk_ProfCK", "Chk_ProfYM", "Chk_ProfYL", "Chk_ProfJL", "Chk_ProfML", "Chk_ProfYY", "Chk_ProfYX"};
static const char * CheckList_Guild[] = {"Chk_GuildIn", "Chk_GuildOut"};
CDlgArrangeTeam::CDlgArrangeTeam()
	:m_nPickupMode(GP_TMPU_RANDOM)
{
	m_pLstCurrentTeamMate = NULL;
	m_pLstNearbypp = NULL;
	m_pTxtName = NULL;
	m_pTxtGroupMsg = NULL;
	m_pChkSlogan = NULL;
	m_pChkShowself = NULL;
	m_pRdoLfgroup = NULL;
	m_pRdoLfmember = NULL;
	m_pRdoRandom = NULL;
	m_pRdoFree = NULL;
	m_pBtnLeave = NULL;
	m_pBtnKick = NULL;
	m_pBtnInvite = NULL;
	m_pBtnDisband = NULL;
	m_pChkShowSlogan = NULL;
	m_pChkHostile = NULL;
	m_pChkAutoTeam = NULL;
}

CDlgArrangeTeam::~CDlgArrangeTeam()
{
}

void CDlgArrangeTeam::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("DEFAULT_Lst_CurrentTmate", m_pLstCurrentTeamMate);
	DDX_Control("Lst_Nearbypp", m_pLstNearbypp);
	DDX_Control("Txt_Name",	m_pTxtName);
	DDX_Control("Txt_GroupMsg", m_pTxtGroupMsg);
	DDX_Control("Chk_Slogan", m_pChkSlogan);
	DDX_Control("Chk_Showself", m_pChkShowself);
	DDX_Control("Rdo_Lfgroup", m_pRdoLfgroup);
	DDX_Control("Rdo_Lfmember", m_pRdoLfmember);
	DDX_Control("Rdo_Random", m_pRdoRandom);
	DDX_Control("Rdo_Free", m_pRdoFree);
	DDX_Control("Btn_Leave", m_pBtnLeave);
	DDX_Control("Btn_Kick", m_pBtnKick);
	DDX_Control("Btn_Invite", m_pBtnInvite);
	DDX_Control("Btn_Disband", m_pBtnDisband);
	DDX_Control("Chk_ShowSlogan", m_pChkShowSlogan);
	DDX_Control("Chk_Hostile", m_pChkHostile);
	DDX_Control("Chk_AutoAccept", m_pChkAutoTeam);
}

bool CDlgArrangeTeam::OnInitDialog()
{
	int i(0);
	for (i =0; i < sizeof(CheckList_Prof)/sizeof(CheckList_Prof[0]); i++)
	{
		PAUIOBJECT pObj = GetDlgItem(CheckList_Prof[i]);
		if (pObj)
		{
			((PAUICHECKBOX)pObj)->Check(true);
		}
	}
	for (i =0; i < sizeof(CheckList_Guild)/sizeof(CheckList_Guild[0]); i++)
	{
		PAUIOBJECT pObj = GetDlgItem(CheckList_Guild[i]);
		if (pObj)
		{
			((PAUICHECKBOX)pObj)->Check(true);
		}
	}
	m_pChkShowSlogan->Check(true);
	return true;
}
void CDlgArrangeTeam::OnCommandKick(const char *szCommand)
{
	CECHostPlayer *pHost = GetHostPlayer();
	CECTeam *pTeam = pHost->GetTeam();

	if( pTeam && m_pLstCurrentTeamMate->GetCurSel() >= 0 &&
		m_pLstCurrentTeamMate->GetCurSel() < m_pLstCurrentTeamMate->GetCount() )
	{
		CECTeamMember *pMember = pTeam->GetMemberByIndex(m_pLstCurrentTeamMate->GetCurSel());
		if( pMember->GetCharacterID() == pHost->GetCharacterID() )
			GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(206), GP_CHAT_MISC);
		else
			GetGameSession()->c2s_CmdTeamKickMember(pMember->GetCharacterID());
	}
}

void CDlgArrangeTeam::OnCommandInvite(const char *szCommand)
{
	ACString strName = m_pTxtName->GetText();
	int idPlayer = GetGameRun()->GetPlayerID(strName);

	if( ISPLAYERID(idPlayer) )
	{
		GetGameSession()->c2s_CmdTeamInvite(idPlayer);
		GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(203), GP_CHAT_MISC);
	}
	else
		GetGameSession()->GetPlayerIDByName(strName, 1);
	//	GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(531), GP_CHAT_MISC);
}

void CDlgArrangeTeam::OnCommandLeave(const char *szCommand)
{
	CECHostPlayer *pHost = GetHostPlayer();
	CECTeam *pTeam = pHost->GetTeam();

	GetGameUIMan()->MessageBox("Game_TeamDisband", GetGameUIMan()->
		GetStringFromTable(235), MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgArrangeTeam::OnCommandRandom(const char *szCommand)
{
	CECHostPlayer *pHost = GetHostPlayer();
	CECTeam *pTeam = pHost->GetTeam();

	if( pTeam )
	{
		GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(207), GP_CHAT_MISC);
		if( m_nPickupMode == GP_TMPU_RANDOM )
			CheckRadioButton(1, 1);
		else
			CheckRadioButton(1, 2);
	}
	else
	{
		if( 0 == stricmp(szCommand, "random") )
			m_nPickupMode = GP_TMPU_RANDOM;
		else
			m_nPickupMode = GP_TMPU_FREEDOM;
		GetGameSession()->c2s_CmdTeamSetPickupFlag(m_nPickupMode);
	}
}

void CDlgArrangeTeam::OnCommandSlogan(const char *szCommand)
{
	CECHostPlayer *pHost = GetGameRun()->GetHostPlayer();
	CECPlayer::TEAMREQ tr = pHost->GetTeamRequire();
	tr.bShowReq = m_pChkSlogan->IsChecked();
	tr.bShowLevel = m_pChkShowself->IsChecked();
	tr.iType = m_pRdoLfgroup->IsChecked() ? 0 : 1;
	pHost->SetTeamRequireText(tr);
	if( m_pChkSlogan->IsChecked() )
		m_pTxtGroupMsg->SetText(pHost->GetTeamReqText());
	else
		m_pTxtGroupMsg->SetText(_AL(""));
}

void CDlgArrangeTeam::OnCommandShowSelf(const char *szCommand)
{
	CECHostPlayer *pHost = GetGameRun()->GetHostPlayer();
	CECPlayer::TEAMREQ tr = pHost->GetTeamRequire();
	tr.bShowReq = m_pChkSlogan->IsChecked();
	tr.bShowLevel = m_pChkShowself->IsChecked();
	tr.iType = m_pRdoLfgroup->IsChecked() ? 0 : 1;
	pHost->SetTeamRequireText(tr);
	if( m_pChkSlogan->IsChecked() )
		m_pTxtGroupMsg->SetText(pHost->GetTeamReqText());
	else
		m_pTxtGroupMsg->SetText(_AL(""));
}

void CDlgArrangeTeam::OnCommandLfg(const char *szCommand)
{
	CECHostPlayer *pHost = GetGameRun()->GetHostPlayer();
	CECPlayer::TEAMREQ tr = pHost->GetTeamRequire();
	tr.bShowReq = m_pChkSlogan->IsChecked();
	tr.bShowLevel = m_pChkShowself->IsChecked();
	tr.iType = m_pRdoLfgroup->IsChecked() ? 0 : 1;
	pHost->SetTeamRequireText(tr);
	if( m_pChkSlogan->IsChecked() )
		m_pTxtGroupMsg->SetText(pHost->GetTeamReqText());
	else
		m_pTxtGroupMsg->SetText(_AL(""));
}

void CDlgArrangeTeam::OnCommandConfirm(const char *szCommand)
{
	CECHostPlayer *pHost = GetGameRun()->GetHostPlayer();
	CECPlayer::TEAMREQ tr = pHost->GetTeamRequire();
	tr.bShowReq = m_pChkSlogan->IsChecked();
	tr.bShowLevel = m_pChkShowself->IsChecked();
	tr.iType = m_pRdoLfgroup->IsChecked() ? 0 : 1;
	pHost->SetTeamRequire(tr, true);
	Show(false);
}

void CDlgArrangeTeam::OnCommandRefresh(const char *szCommand)
{
	UpdateTeam(true);
}

void CDlgArrangeTeam::OnCommandAcceptAutoTeam(const char * szCommand)
{
	EC_GAME_SETTING setting = GetGame()->GetConfigs()->GetGameSettings();
	setting.bAutoTeamForTask = !setting.bAutoTeamForTask;
	GetGame()->GetConfigs()->SetGameSettings(setting);
	m_pChkAutoTeam->Check(setting.bAutoTeamForTask);
}

void CDlgArrangeTeam::OnShowDialog()
{
	CECHostPlayer *pHost = GetGameRun()->GetHostPlayer();
	CECPlayer::TEAMREQ tr = pHost->GetTeamRequire();
	m_pChkSlogan->Check(tr.bShowReq);
	m_pChkShowself->Check(tr.bShowLevel);
	if( tr.iType == 0 )
		m_pRdoLfgroup->Check(true);
	else
		m_pRdoLfmember->Check(true);
	if( m_pChkSlogan->IsChecked() )
		m_pTxtGroupMsg->SetText(pHost->GetTeamReqText());
	else
		m_pTxtGroupMsg->SetText(_AL(""));

	const EC_GAME_SETTING& setting = GetGame()->GetConfigs()->GetGameSettings();
	m_pChkAutoTeam->Check(setting.bAutoTeamForTask);
}

void CDlgArrangeTeam::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	POINT ptPos = pObj->GetPos();
	PAUILISTBOX pList = (PAUILISTBOX)pObj;
	int nCurSel = pList->GetCurSel();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int y = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if( nCurSel >= 0 && nCurSel < pList->GetCount() &&
		pList->GetHitArea(x, y) == AUILISTBOX_RECT_TEXT )
	{
		ACString strName = GetGameRun()->GetPlayerName(pList->GetItemData(nCurSel), true);
		m_pTxtName->SetText(strName);
		GetHostPlayer()->SelectTarget(pList->GetItemData(nCurSel));
	}
}

void CDlgArrangeTeam::OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	POINT ptPos = pObj->GetPos();
	PAUILISTBOX pList = (PAUILISTBOX)pObj;
	int nCurSel = pList->GetCurSel();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int y = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if( nCurSel >= 0 && nCurSel < pList->GetCount() &&
		pList->GetHitArea(x, y) == AUILISTBOX_RECT_TEXT )
	{
		int idPlayer = pList->GetItemData(nCurSel);

		x = GET_X_LPARAM(lParam) - p->X;
		y = GET_Y_LPARAM(lParam) - p->Y;
		GetGameUIMan()->PopupPlayerContextMenu(idPlayer, x, y);
	}
}

bool CDlgArrangeTeam::UpdateTeam(bool bUpdateNear)
{
	int i;
	ACHAR szText[256];
	CECWorld *pWorld = GetWorld();
	CECHostPlayer *pHost = GetGameRun()->GetHostPlayer();

	PAUICHECKBOX pObj1 = (PAUICHECKBOX)GetDlgItem(CheckList_Guild[0]);
	PAUICHECKBOX pObj2 = (PAUICHECKBOX)GetDlgItem(CheckList_Guild[1]);
	if (!pHost->GetFactionID())
	{
		pObj1->Check(true);
		pObj1->Enable(false);
		pObj2->Check(true);
		pObj2->Enable(false);
	}
	else
	{
		pObj1->Enable(true);
		pObj2->Enable(true);
	}
	if( bUpdateNear )
	{
		CECPlayerMan *pPlayerMan = pWorld->GetPlayerMan();
		const APtrArray<CECElsePlayer *> &PlayerNear = pPlayerMan->GetPlayersInMiniMap();

		bool bShowSlogan = m_pChkShowSlogan->IsChecked();
		m_pLstNearbypp->ResetContent();
		for( i = 0; i < PlayerNear.GetSize(); i++ )
		{
			if( bShowSlogan && PlayerNear[i]->GetTeamRequire().bShowReq )
			{
				a_sprintf(szText, _AL("[%s]%s"), PlayerNear[i]->
					GetTeamReqText(), PlayerNear[i]->GetName());
			}
			else
				a_strcpy(szText, PlayerNear[i]->GetName());
			if (CheckShowCondition(PlayerNear[i]))
			{
				m_pLstNearbypp->AddString(szText);
				m_pLstNearbypp->SetItemData(m_pLstNearbypp->GetCount()-1, PlayerNear[i]->GetCharacterID());

				int iState = CheckFactionAlliance(PlayerNear[i]);
				if( iState == 1 ) m_pLstNearbypp->SetItemTextColor(m_pLstNearbypp->GetCount()-1, NAMECOL_ALLIANCE);
				else if( iState == 2 ) m_pLstNearbypp->SetItemTextColor(m_pLstNearbypp->GetCount()-1, NAMECOL_HOSTILE);
			}
		}
		m_pLstNearbypp->SetCurSel(-1);

		CECPlayer::TEAMREQ tr = pHost->GetTeamRequire();
		tr.iLevel = pHost->GetBasicProps().iLevel;
		tr.iProfession = pHost->GetProfession();
		pHost->SetTeamRequire(tr, false);
		CheckRadioButton(2, tr.iType + 1);
	}
	
	CECTeamMember *pMember;
	CECTeam *pTeam = pHost->GetTeam();
	int nNumMembers = pTeam ? pTeam->GetMemberNum() : 0;
	PAUILISTBOX pListMate = m_pLstCurrentTeamMate;
	int nCurSel = pListMate->GetCurSel();
	
	pListMate->ResetContent();
	for( i = 0; i < nNumMembers; i++ )
	{
		pMember = pTeam->GetMemberByIndex(i);
		pListMate->AddString(pMember->GetName());
	}
	if( nCurSel < pListMate->GetCount() )
		pListMate->SetCurSel(nCurSel);

	if( pTeam )
	{
		m_pBtnLeave->Enable(true);
		m_pRdoRandom->Enable(false);
		m_pRdoFree->Enable(false);
		if( pTeam->GetLeaderID() == pHost->GetCharacterID() )
		{
			m_pBtnKick->Enable(true);
			m_pBtnInvite->Enable(true);
			m_pBtnDisband->Enable(true);
		}
		else
		{
			m_pBtnKick->Enable(false);
			m_pBtnInvite->Enable(false);
			m_pBtnDisband->Enable(false);
		}
	}
	else
	{
		m_pBtnKick->Enable(false);
		m_pBtnLeave->Enable(false);
		m_pBtnInvite->Enable(true);
		m_pBtnDisband->Enable(false);
		m_pRdoRandom->Enable(true);
		m_pRdoFree->Enable(true);
	}

	if( m_nPickupMode == GP_TMPU_RANDOM )
		CheckRadioButton(1, 1);
	else
		CheckRadioButton(1, 2);

	return true;
}

void CDlgArrangeTeam::OnCommandMakeLeader(const char * szCommand)
{
	CECTeam *pTeam = GetHostPlayer()->GetTeam();

	if( pTeam && m_pLstCurrentTeamMate->GetCurSel() >= 0 &&
		m_pLstCurrentTeamMate->GetCurSel() < m_pLstCurrentTeamMate->GetCount() )
	{
		CECTeamMember *pMember = pTeam->GetMemberByIndex(m_pLstCurrentTeamMate->GetCurSel());
		if( pMember->GetCharacterID() != GetHostPlayer()->GetCharacterID() )
			GetGameSession()->c2s_CmdTeamChangeLeader(pMember->GetCharacterID());
	}
}

bool CDlgArrangeTeam::CheckShowCondition(const CECElsePlayer* pPlayer)
{
	if (!pPlayer)
	{
		return false;
	}
	int prof = ((CECPlayer *)pPlayer)->GetProfession();
	PAUICHECKBOX pObj = (PAUICHECKBOX)GetDlgItem(CheckList_Prof[prof]);
	if (!pObj->IsChecked())
	{
		return false;
	}
	int faction = ((CECPlayer *)pPlayer)->GetFactionID();
	if (GetHostPlayer()->GetFactionID() == faction)
	{
		pObj = (PAUICHECKBOX)GetDlgItem(CheckList_Guild[0]);
		if (!pObj->IsChecked())
		{
			return false;
		}
	}
	else
	{
		pObj = (PAUICHECKBOX)GetDlgItem(CheckList_Guild[1]);
		if (!pObj->IsChecked())
		{
			return false;
		}
	}

	if (m_pChkHostile->IsChecked() && CheckFactionAlliance(pPlayer) != 2)
		return false;

	return true;
}

int CDlgArrangeTeam::CheckFactionAlliance( const CECElsePlayer* pPlayer )
{
	if( !pPlayer ) return 0;
	CECFactionMan* pFMan = GetGame()->GetFactionMan();
	int faction = ((CECPlayer*)pPlayer)->GetFactionID();
	if( GetHostPlayer()->IsFactionAllianceMember(faction) ) return 1;
	else if( pFMan->IsFactionHostile(faction) ) return 2;
	return 0;
}
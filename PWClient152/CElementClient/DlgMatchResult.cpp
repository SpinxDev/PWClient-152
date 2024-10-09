/*
 * FILE: DlgOfflineShopCreate.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WYD
 *
 * HISTORY: 
 *
 * Copyright (c) 2013, All Rights Reserved.
 */
 
#include "DlgMatchResult.h"
#include "DlgQuickAction.h"
#include "EC_GameSession.h"
#include "AUICTranslate.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "DlgChat.h"
#include "EC_GameRun.h"
#include "DlgMatchProfile.h"
#include "EC_TimeSafeChecker.h"

#include <vector>

#include "playerprofilegetmatchresult_re.hpp"
#include "playerprofilegetprofiledata_re.hpp"

const DWORD COOLDOWN_TIME = 10000;

AUI_BEGIN_COMMAND_MAP(CDlgMatchModeSelect, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Start",		OnCommandDo)
AUI_ON_COMMAND("Btn_FixInfo",	OnCommandInfo)
AUI_ON_COMMAND("Btn_Close",		OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

	
bool CDlgMatchModeSelect::OnInitDialog()
{	
	m_pComList = NULL;
	DDX_Control("Combo_Res",m_pComList);
	m_pComList->AddString(GetGameUIMan()->GetStringFromTable(10650));
	m_pComList->AddString(GetGameUIMan()->GetStringFromTable(10651));
	m_pComList->AddString(GetGameUIMan()->GetStringFromTable(10652));

	m_pComList->SetCurSel(0);

	m_matchtime = 0;
	return CDlgBase::OnInitDialog();
}
void CDlgMatchModeSelect::OnShowDialog()
{
	CheckButtonState();
}
void CDlgMatchModeSelect::OnTick()
{
	CheckButtonState();
}
void CDlgMatchModeSelect::OnCommandDo(const char *szCommand)
{
	// 开始匹配
	CDlgMatchResult* pDlg = (CDlgMatchResult*)GetGameUIMan()->GetDialog("Win_MatchResult");
	pDlg->OnCommandNext(NULL);
	m_matchtime = GetTickCount();
}
void CDlgMatchModeSelect::CheckButtonState()
{
	PAUIOBJECT pBtn = GetDlgItem("Btn_Start");
	bool bEnable = CECTimeSafeChecker::ElapsedTimeFor(m_matchtime) >= COOLDOWN_TIME;
	pBtn->Enable(bEnable);
	pBtn->SetHint(bEnable ? _AL(""):GetGameUIMan()->GetStringFromTable(10061));
}
void CDlgMatchModeSelect::OnCommandInfo(const char *szCommand)
{	
	// 完善信息
	GetGameUIMan()->m_pDlgMatchHabit->Show(true);
}
int CDlgMatchModeSelect::GetMatchMode()
{
	return m_pComList->GetCurSel();
}
//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgMatchResult, CDlgBase)
AUI_ON_COMMAND("Btn_SayHi",		OnCommandSayHi)
AUI_ON_COMMAND("Btn_Chat",		OnCommandChat)
AUI_ON_COMMAND("Btn_Team",		OnCommandTeam)
AUI_ON_COMMAND("Btn_Friend",	OnCommandFriend)
AUI_ON_COMMAND("Btn_Next",		OnCommandNext)

AUI_ON_COMMAND("Btn_Close",		OnCommand_CANCEL)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

void CDlgMatchResult::OnShowDialog()
{
	CheckButtonState();
}
void CDlgMatchResult::CheckButtonState()
{
	PAUIOBJECT pBtn = GetDlgItem("Btn_Next");
	bool bEnable = CECTimeSafeChecker::ElapsedTimeFor(m_matchtime) >= COOLDOWN_TIME;
	pBtn->Enable(bEnable);
	pBtn->SetHint(bEnable ? _AL(""):GetGameUIMan()->GetStringFromTable(10061));
}
bool CDlgMatchResult::OnInitDialog()
{
	CDlgBase::OnInitDialog();

	m_pLst = NULL;
	DDX_Control("List_Result",m_pLst);

	m_matchtime = 0;

	return true;
}
int CDlgMatchResult::GetSelRoleid()
{
	int roleid = m_pLst->GetItemData(m_pLst->GetCurSel());
	if (roleid<=0)
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10656),MB_OK,A3DCOLORRGB(255,255,255));
		return 0;
	}

	return roleid;
}
void CDlgMatchResult::OnCommandSayHi(const char *szCommand)
{
	// 打招呼
	int roleid = GetSelRoleid();
	if(roleid<=0) return;

	const ACHAR *pszName = GetGameRun()->GetPlayerName(roleid, true);
	
	CDlgChat *pChat = GetGameUIMan()->m_pDlgChat;
	pChat->SwitchToPrivateChat(pszName);

	PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(pChat->GetDlgItem("DEFAULT_Txt_Speech"));
	
	ACString strText = pEdit->GetText();
	if( strText.GetLength() <= 0 )
	{
		pChat->ChangeFocus(NULL);
		return;
	}

//	ACString sayword;
	int nEmotion = GetHostPlayer()->GetCurEmotionSet();
	if( 23 < (int)GetGameUIMan()->m_vecEmotion[nEmotion]->size() )
	{
		strText += GetGameUIMan()->GetStringFromTable(10653);
		pEdit->SetText(strText);
		pEdit->AppendItem(enumEIEmotion, 0, L"W", MarshalEmotionInfo(nEmotion, 3));
		strText = pEdit->GetText();
		strText += GetGameUIMan()->GetStringFromTable(10659);
		pEdit->SetText(strText);
		pEdit->AppendItem(enumEIEmotion, 0, L"W", MarshalEmotionInfo(nEmotion, 22));
	}
	else
	{
		strText += GetGameUIMan()->GetStringFromTable(10653);
		strText += GetGameUIMan()->GetStringFromTable(10659);
		pEdit->SetText(strText);
	}

	pChat->OnCommand_speak(NULL);

	pEdit->SetText(_AL(""));

	pChat->ChangeFocus(NULL);
}
void CDlgMatchResult::OnCommandChat(const char *szCommand)
{
	int roleid = GetSelRoleid();
	if(roleid<=0) return;
	
	GetGameUIMan()->m_pDlgQuickAction->SetData(roleid);
	GetGameUIMan()->m_pDlgQuickAction->OnCommand_Whisper(NULL);
}
void CDlgMatchResult::OnCommandTeam(const char *szCommand)
{
	int roleid = GetSelRoleid();
	if(roleid<=0) return;
	
	GetGameUIMan()->m_pDlgQuickAction->SetData(roleid);
	GetGameUIMan()->m_pDlgQuickAction->OnCommand_InviteGroup(NULL);

}
void CDlgMatchResult::OnCommandFriend(const char *szCommand)
{
	int roleid = GetSelRoleid();
	if(roleid<=0) return;
	
	GetGameUIMan()->m_pDlgQuickAction->SetData(roleid);
	GetGameUIMan()->m_pDlgQuickAction->OnCommand_AddFriend(NULL);
}

void CDlgMatchResult::OnCommandNext(const char *szCommand)
{
	// 换一批
	int mode = 0;
	CDlgMatchModeSelect* pDlg = (CDlgMatchModeSelect*)GetGameUIMan()->GetDialog("Win_MatchModeSel");
	if(pDlg)
	{
		pDlg->ResetCounter(GetTickCount());
		mode = pDlg->GetMatchMode();
	}

	m_matchtime = GetTickCount();

	GetGameSession()->match_FindPlayer(mode);	
}
void CDlgMatchResult::OnTick()
{
	int i=0;
	for (i=0;i<m_pLst->GetCount();i++)
	{
		int flag = m_pLst->GetItemData(i,1);
		int roleid = m_pLst->GetItemData(i,0);
		
		if (flag == MATCH_NONAME)
		{
			ACHAR szText[512] = {0};
				
			const ACHAR* szName = g_pGame->GetGameRun()->GetPlayerName(roleid, false);				
			
			if( szName )		
			{						
				m_pLst->SetItemData(i,MATCH_HASNAME,1);

				ACString name = szName;
				ACString genderCol = m_pLst->GetItemData(i,5) == GENDER_FEMALE ? _AL("^e066ff"):_AL("^20b2aa");
				ACString whiteCol = _AL("^ffffff");

				name = genderCol + name;
				name += whiteCol;
				
				a_sprintf(szText,_AL("%s\t%d%%\t%d\t%s"), name, m_pLst->GetItemData(i,2), m_pLst->GetItemData(i,3), GetGameRun()->GetProfName(m_pLst->GetItemData(i,4)));
				m_pLst->SetText(i,szText);
			}				
		}		
	}

	CheckButtonState();
}
void CDlgMatchResult::OnMatchResultRe(GNET::Protocol* pProtocl)
{
	m_pLst->ResetContent();

	PlayerProfileGetMatchResult_Re* p = (PlayerProfileGetMatchResult_Re*)pProtocl;

	int i=0;
	
	std::vector<int> rolevec;

	for (i=0;i<(int)p->result.size();i++)
	{
		m_pLst->AddString(_AL(""));
		m_pLst->SetItemData(i, p->result[i].roleid,0);

		ACHAR szText[512] = {0};
		
		ACString name;		
		const ACHAR* szName = g_pGame->GetGameRun()->GetPlayerName(p->result[i].roleid, false);	
		

		if( szName )		
		{	
			name = szName;			
			m_pLst->SetItemData(i,MATCH_HASNAME,1);
		}	
		else
		{
			name = _AL("--");
			m_pLst->SetItemData(i,MATCH_NONAME,1);

			rolevec.push_back(p->result[i].roleid);
		}
		int sim = (int)(p->result[i].similarity * 100);

		ACString genderCol = p->result[i].gender == GENDER_FEMALE ? _AL("^e066ff"):_AL("^20b2aa");
		ACString whiteCol = _AL("^ffffff");
		
		name = genderCol + name;
		name += whiteCol;

		a_sprintf(szText,_AL("%s\t%d%%\t%d\t%s"), name, sim, p->result[i].level, GetGameRun()->GetProfName(p->result[i].occupation));
		m_pLst->SetText(i,szText);

		m_pLst->SetItemData(i,sim,2);
		m_pLst->SetItemData(i,p->result[i].level,3);
		m_pLst->SetItemData(i,p->result[i].occupation,4);
		m_pLst->SetItemData(i,p->result[i].gender,5);
	}

	if(rolevec.size()>0)
		g_pGame->GetGameSession()->CacheGetPlayerBriefInfo(rolevec.size(),rolevec.begin(), 2);

	GetDlgItem("Btn_SayHi")->Enable(i>0);
	GetDlgItem("Btn_Chat")->Enable(i>0);
	GetDlgItem("Btn_Team")->Enable(i>0);
	GetDlgItem("Btn_Friend")->Enable(i>0);
}

///////////////////////////////////////////////////////////////////////////
//
// 领奖
//
//////////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgMatchAward, CDlgBase)
AUI_ON_COMMAND("btn_getreward",	OnCommandAward)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

void CDlgMatchAward::OnShowDialog()
{
	GetDlgItem("Lab_reward")->SetText(GetGameUIMan()->GetStringFromTable(m_bOneAward ? 10654:10655));
}

void CDlgMatchAward::OnCommandAward(const char *szCommand)
{
	// 领奖
}


///////////////////////////////////////////////////////////////////////////
//
// 
//
//////////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgMatchInfoHint, CDlgBase)
AUI_ON_COMMAND("Btn_FixInfo",	OnCommandInfo)
AUI_END_COMMAND_MAP()

void CDlgMatchInfoHint::OnCommandInfo(const char *szCommand)
{
	// 完善信息
	GetGameUIMan()->m_pDlgMatchHabit->Show(true);
	Show(false);
}
void CDlgMatchInfoHint::OnServerNotify(GNET::Protocol* pProtocol)
{
	switch(pProtocol->GetType())
	{
	case PROTOCOL_PLAYERPROFILEGETPROFILEDATA_RE: // 获取资料结果
		{
			PlayerProfileGetProfileData_Re* p = (PlayerProfileGetProfileData_Re*)pProtocol;
			int ret = p->retcode;

			if(ret < 0)
			{
				GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10657),MB_OK,A3DCOLORRGB(255,255,255));
				return;
			}
			
			GetGameUIMan()->m_pDlgMatchHabit->SetMaskTimeSpan(p->data.game_time_mask);
			GetGameUIMan()->m_pDlgMatchHabit->SetMaskGamePlay(p->data.game_interest_mask);				
			GetGameUIMan()->m_pDlgMatchHobby->SetMaskHobby(p->data.personal_interest_mask);
			GetGameUIMan()->m_pDlgMatchAge->SetAgeHoro(p->data.age,p->data.zodiac);

			unsigned short mask = p->data.match_option_mask;
			GetGameUIMan()->m_pDlgMatchSetting->SetMask(ret == 0 ? 0xff : mask); // 初始 全选
			GetGameUIMan()->m_pDlgMatchSetting->SetServerMask(mask);

			if (GetGameUIMan()->m_pDlgMatchSetting->ShowUIOnGetProfileReturn()){
				bool bCompleteInfo = GetGameUIMan()->m_pDlgMatchHabit->IsCompletedProfile() && 
					GetGameUIMan()->m_pDlgMatchHobby->IsCompletedProfile() && 
					GetGameUIMan()->m_pDlgMatchAge->IsCompletedProfile() && 
					GetGameUIMan()->m_pDlgMatchSetting->IsCompletedProfile();
				
				if (bCompleteInfo)
				{
					ASSERT( ret > 0);
					GetGameUIMan()->GetDialog("Win_MatchModeSel")->Show(true);		
					if(GetGameUIMan()->GetDialog("Win_MatchHint")->IsShow())
						GetGameUIMan()->GetDialog("Win_MatchHint")->Show(false);
				}
				else
				{
					GetGameUIMan()->GetDialog("Win_MatchHint")->Show(true);	
					if (GetGameUIMan()->GetDialog("Win_MatchModeSel")->IsShow())
						GetGameUIMan()->GetDialog("Win_MatchModeSel")->Show(false);
				}
			}
		}
		break;
	case PROTOCOL_PLAYERPROFILEGETMATCHRESULT_RE: // 匹配结果
		{
			PlayerProfileGetMatchResult_Re* p = (PlayerProfileGetMatchResult_Re*)pProtocol;
			
			CDlgMatchResult* pDlg = dynamic_cast<CDlgMatchResult*>(GetGameUIMan()->GetDialog("Win_MatchResult"));

			if(pDlg)			
				pDlg->OnMatchResultRe(pProtocol);			

			if(p->result.size()<1)
			{
				GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10658),MB_OK,A3DCOLORRGB(255,255,255));
				return;
			}

		
			if(pDlg && !pDlg->IsShow())
				pDlg->Show(true);
		}
		break;
	default:
		ASSERT(FALSE);
		a_LogOutput(1,"CDlgMatchInfoHint::OnServerNotify: error protocol %d", pProtocol->GetType());
		break;
	}
}
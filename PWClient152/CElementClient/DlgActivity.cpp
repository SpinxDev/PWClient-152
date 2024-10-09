// Filename	: DlgActivity.h
// Creator	: Xu Wenbin
// Date		: 2009/09/28

#include "AFI.h"
#include "DlgActivity.h"
#include "DlgSignIn.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_World.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_HostPlayer.h"
#include "EC_AutoTeam.h"
#include "EC_Resource.h"
#include "EC_Manager.h"
#include "EC_FixedMsg.h"
#include "EC_CrossServer.h"
#include "EC_UIConfigs.h"
#include "EC_Configs.h"
#include "EC_Utility.h"
#include <AWIniFile.h>
#include "CSplit.h"
#include "AUICTranslate.h"
#include "TaskTemplMan.h"
#include "elementdataman.h"

#include "autoteamplayerleave.hpp"

#include <A2DSprite.h>

#define new A_DEBUG_NEW

static const ACHAR* ACTIVITY_COLOR_NORMAL = _AL("^ffffff");
static const ACHAR* ACTIVITY_COLOR_RECOMMEND = _AL("^e8a200");

AUI_BEGIN_COMMAND_MAP(CDlgActivity, CDlgBase)
AUI_ON_COMMAND("Btn_Daily", OnCommand_MainType)
AUI_ON_COMMAND("Btn_Instance", OnCommand_MainType)
AUI_ON_COMMAND("Btn_Holiday", OnCommand_MainType)
AUI_ON_COMMAND("Btn_Sign", OnCommand_SignIn)
AUI_ON_COMMAND("Btn_Record", OnCommand_Record)
AUI_ON_COMMAND("Btn_QuickTeam", OnCommand_QuickTeam)
AUI_ON_COMMAND("Btn_Cancel", OnCommand_CancelTeam)
AUI_ON_COMMAND("Btn_GotoNPC", OnCommand_GotoNPC)
AUI_ON_COMMAND("Btn_Level", OnCommand_SortList)
AUI_ON_COMMAND("Chk_Level", OnCommand_CheckShowAll)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgActivity, CDlgBase)
AUI_ON_EVENT("List_Result", WM_LBUTTONDOWN, OnEventLButtonDown_List)
AUI_END_EVENT_MAP()

CDlgActivity::CDlgActivity()
{
	m_pLbl_Date = NULL;
	m_pLst_Activity = NULL;
	m_pImg_Activity = NULL;
	m_pTxt_Desc = NULL;
	memset(m_pImgAward, 0, sizeof(m_pImgAward));
	m_pBtn_Daily = NULL;
	m_pBtn_Instance = NULL;
	m_pBtn_Holiday = NULL;
	m_pBtn_QuickTeam = NULL;
	m_pBtn_Cancel = NULL;
	m_pBtn_GotoNPC = NULL;
	m_pChk_ShowAllActivty = NULL;
	m_bSortByDesc = false;

	m_iMainType = ACTIVITY_DAILY;
	m_year = m_month = m_day = m_sign_update_day = 0;
	m_SingInCounter.SetPeriod(5000);
}

bool CDlgActivity::OnInitDialog()
{
	DDX_Control("Txt_Date", m_pLbl_Date);
	DDX_Control("List_Result", m_pLst_Activity);
	DDX_Control("Img_1", m_pImg_Activity);
	DDX_Control("Txt_1", m_pTxt_Desc);
	DDX_Control("Btn_Daily", m_pBtn_Daily);
	DDX_Control("Btn_Instance", m_pBtn_Instance);
	DDX_Control("Btn_Holiday", m_pBtn_Holiday);
	DDX_Control("Btn_QuickTeam", m_pBtn_QuickTeam);
	DDX_Control("Btn_Cancel", m_pBtn_Cancel);
	DDX_Control("Btn_GotoNPC", m_pBtn_GotoNPC);
	DDX_Control("Chk_Level", m_pChk_ShowAllActivty);
	for( int i=0;i<ACTIVITY_AWARD_NUM;i++ )
	{
		char szName[40];
		sprintf(szName, "Img_Item%d", i);
		DDX_Control(szName, m_pImgAward[i]);
	}

	OnCommand_SortList(NULL);
	EnableSignIn(false);
	if (CECCrossServer::Instance().IsOnSpecialServer()) {
		PAUIOBJECT pObj = GetDlgItem("Btn_Record");
		if (pObj) pObj->Enable(false);
	}
	m_SingInCounter.SetPeriod(5000);
	return true;
}

void CDlgActivity::OnShowDialog()
{
	OnCommand_MainType("Btn_Daily");
}

void CDlgActivity::OnTick()
{
	// Get current time
	int week;
	int hour, minute, second;
	GetTime(m_year, m_month, m_day, hour, minute, second, week);

	// Update title UI
	ACString strText;
	strText.Format(GetStringFromTable(8010), m_year, m_month, m_day, hour, minute);
	ACString strWeek = GetStringFromTable(!week ? 8025 : 8018 + week);
	m_pLbl_Date->SetText(strText + _AL(" ") + strWeek);

	// 更新按钮状态
	UpdateButtonState();

	// 过了24点时请求更新签到信息
	if (m_day != m_sign_update_day) {
		if (m_SingInCounter.IncCounter(GetGame()->GetRealTickTime())){
			m_SingInCounter.Reset();
			GetGameSession()->c2s_CmdRefreshSignIn();
		}
	}
}

void CDlgActivity::UpdateButtonState()
{
	bool bCanGotoNPC = false;
	bool bCanDoAutoTeam = false;
	CECAutoTeam* pAutoTeam = GetHostPlayer()->GetAutoTeam();
	int iSelActivity = 0;

	while(1)
	{
		int iCurSel = m_pLst_Activity->GetCurSel();
		if( iCurSel < 0 || iCurSel >= m_pLst_Activity->GetCount() )
			break;

		Activity* pAct = (Activity*)m_pLst_Activity->GetItemDataPtr(iCurSel);
		if( !pAct ) break;
		iSelActivity = pAct->p->id;

		if( CECAutoTeamConfig::Instance().CanJumpToGoal(pAct) )
			bCanGotoNPC = true;

		if( !CECAutoTeamConfig::Instance().CanDoAutoTeam(pAct) ) break;

		if( pAutoTeam->IsMatchingForActivity() )
			break;

		if( GetHostPlayer()->GetTeam() )
			break;

		bCanDoAutoTeam = true;
		break;
	}

	m_pBtn_QuickTeam->Enable(bCanDoAutoTeam);
	m_pBtn_GotoNPC->Enable(bCanGotoNPC);
	m_pBtn_GotoNPC->SetHint(!CECAutoTeamConfig::Instance().IsHaveTransmitItem() ? GetStringFromTable(10866) : _AL(""));
	m_pBtn_Cancel->Enable(pAutoTeam->IsMatchingForActivity() && CECAutoTeamConfig::Instance().IsInEnabledMap() && pAutoTeam->GetCurGoal() == iSelActivity);
}

void CDlgActivity::GetTime(int &year, int &month, int &day, int &hour, int &minute, int &second, int &week)
{
	tm newTime = GetGame()->GetServerLocalTime();
	year = 1900 + newTime.tm_year;
	month = 1 + newTime.tm_mon;
	day = newTime.tm_mday;
	hour = newTime.tm_hour;
	minute = newTime.tm_min;
	second = newTime.tm_sec;
	week = newTime.tm_wday;
}

void CDlgActivity::UpdateActivityList(bool bUpdateStatus)
{
	ACString strLine;
	int iCurSel = m_pLst_Activity->GetCurSel();
	m_pLst_Activity->ResetContent();

	CECAutoTeamConfig & config = CECAutoTeamConfig::Instance();
	for( size_t i=0;i<config.GetCount();i++ )
	{
		const Activity* pAct = config.Get(i);
		if( pAct->IsDisable() || pAct->p->type != m_iMainType )
			continue;

		if (!IsShowAllActivity() && !CECAutoTeamConfig::Instance().IsFitLevelRankRealm(pAct)) {
			continue;
		}

		strLine.Format(_AL("%s\t%s\t%s\t%s"), pAct->p->name, GetRequireLevelStr(pAct), GetRequireCountStr(pAct), GetRequireTimeStr(pAct, false));
		if( GetHostPlayer()->GetAutoTeam()->GetType() == CECAutoTeam::TYPE_ACTIVITY &&
			GetHostPlayer()->GetAutoTeam()->GetCurGoal() == pAct->p->id )
			strLine += GetStringFromTable(8047);

		if( pAct->IsRecommend() )
			strLine = ACTIVITY_COLOR_RECOMMEND + strLine;
		else
			strLine = ACTIVITY_COLOR_NORMAL + strLine;
		int n = m_pLst_Activity->AddString(strLine);
		m_pLst_Activity->SetItemData(n-1, pAct->p->id);
		m_pLst_Activity->SetItemDataPtr(n-1, (void *)pAct);
//		m_pLst_Activity->SetItemTextColor(n-1, pAct->IsRecommend() ? A3DCOLORRGB(255, 0, 0) : A3DCOLORRGB(255, 255, 255));
	}

	m_pLst_Activity->SetCurSel(bUpdateStatus ? iCurSel : 0);
	UpdateActivityContent();
}

void CDlgActivity::UpdateActivityContent()
{
	ScopedAUIControlSpriteModify _dummy(m_pImg_Activity);

	int i;
	AUICTranslate trans;
	AUIOBJECT_SETPROPERTY prop;
	strcpy(prop.fn, "");
	m_pImg_Activity->SetProperty("Image File", &prop);
	m_pTxt_Desc->SetText(_AL(""));
	for( i=0;i<ACTIVITY_AWARD_NUM;i++ )
	{
		m_pImgAward[i]->ClearCover();
		m_pImgAward[i]->SetHint(_AL(""));
	}

	int iCurSel = m_pLst_Activity->GetCurSel();
	if( iCurSel < 0 || iCurSel >= m_pLst_Activity->GetCount() )
		return;
	Activity* pAct = (Activity*)m_pLst_Activity->GetItemDataPtr(iCurSel);
	if( !pAct ) return;

	strcpy(prop.fn, pAct->icon);
	m_pImg_Activity->SetProperty("Image File", &prop);
	if (A2DSprite *pSprite = m_pImg_Activity->GetImage()){
		pSprite->SetLinearFilter(true);
	}

	ACString strContent;
//	strContent += GetStringFromTable(8045) + GetRequireTimeStr(pAct, true) + _AL("\r");
	strContent += ACString(trans.Translate(pAct->p->desc));
	m_pTxt_Desc->SetText(strContent);

	for( i=0;i<ACTIVITY_AWARD_NUM;i++ )
	{
		if( !pAct->p->award[i] )
			continue;

		CECIvtrItem* pItem = CECIvtrItem::CreateItem(pAct->p->award[i], 0, 1);
		if( !pItem ) continue;
		pItem->GetDetailDataFromLocal();

		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImgAward[i]->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
		m_pImgAward[i]->SetHint(trans.Translate(pItem->GetDesc()));
		delete pItem;
	}
}

void CDlgActivity::OnEventLButtonDown_List(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	UpdateActivityContent();
}

void CDlgActivity::OnCommand_MainType(const char* szCommand)
{
	PAUISTILLIMAGEBUTTON buttons[] =
	{ m_pBtn_Daily, m_pBtn_Instance, m_pBtn_Holiday };

	for( int i=0;i<sizeof(buttons)/sizeof(PAUISTILLIMAGEBUTTON);i++ )
	{
		if( !strcmp(buttons[i]->GetName(), szCommand) )
		{
			m_iMainType = i;
			buttons[i]->SetPushed(true);
		}
		else
		{
			buttons[i]->SetPushed(false);
		}
	}

	UpdateActivityList();
}

void CDlgActivity::OnCommand_QuickTeam(const char* szCommand)
{
	if( GetHostPlayer()->GetTeam() ||
		GetHostPlayer()->GetAutoTeam()->IsMatchingForActivity() )
		return;

	int iCurSel = m_pLst_Activity->GetCurSel();
	if( iCurSel < 0 || iCurSel >= m_pLst_Activity->GetCount() )
		return;

	Activity* pAct = (Activity*)m_pLst_Activity->GetItemDataPtr(iCurSel);
	if( !pAct ) return;
	if( !CECAutoTeamConfig::Instance().CanDoAutoTeam(pAct) ) return;

	if( GetHostPlayer()->GetCoolTime(GP_CT_AUTOTEAM) )
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9203), GP_CHAT_MISC);
		return;
	}

	GetHostPlayer()->GetAutoTeam()->DoAutoTeam(CECAutoTeam::TYPE_ACTIVITY, pAct->p->id);
}

void CDlgActivity::OnCommand_CancelTeam(const char* szCommand)
{
	GetHostPlayer()->GetAutoTeam()->Cancel(CECAutoTeam::TYPE_ACTIVITY);
}

void CDlgActivity::OnCommand_GotoNPC(const char* szCommand)
{
	int iCurSel = m_pLst_Activity->GetCurSel();
	if( iCurSel < 0 || iCurSel >= m_pLst_Activity->GetCount() )
		return;

	Activity* pAct = (Activity*)m_pLst_Activity->GetItemDataPtr(iCurSel);
	if( !pAct ) return;
	if( !CECAutoTeamConfig::Instance().CanJumpToGoal(pAct) ) return;

	if( GetHostPlayer()->GetCoolTime(GP_CT_AUTOTEAM) )
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9203), GP_CHAT_MISC);
		return;
	}

	int goal_id = (int)m_pLst_Activity->GetItemData(iCurSel);
	GetGameSession()->c2s_CmdAutoTeamJumpToGoal(goal_id);
}

void CDlgActivity::OnCommand_SignIn(const char *szCommand)
{
	if( g_pGame->GetConfigs()->IsMiniClient() || glb_IsInMapForMiniClient() )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10713), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	GetGameSession()->c2s_CmdDaylySignin();
}

void CDlgActivity::OnCommand_Record(const char *szCommand)
{
	if( g_pGame->GetConfigs()->IsMiniClient() || glb_IsInMapForMiniClient() )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10713), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Signin");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
}

void CDlgActivity::OnCommand_CheckShowAll(const char* szCommand) {
	UpdateActivityList();
}

void CDlgActivity::EnableSignIn(bool bEnable)
{
	PAUIOBJECT pObj = GetDlgItem("Btn_Sign");
	if (pObj) pObj->Enable(bEnable);
}

bool CDlgActivity::IsHasActivity()
{
	// Check whether there's activity at current time
	// Called when dialog is not shown
	//
	
	int year, month, day;
	int hour, minute, second;
	int week;
	GetTime(year, month, day, hour, minute, second, week);
	
	CECAutoTeamConfig & config = CECAutoTeamConfig::Instance();
	for (size_t i = 0; i < config.GetCount(); ++ i)
	{
		const Activity &a = *config.Get(i);
		if (a.IsHappenOn(year, month, day) &&
			a.IsHappenIn(hour, minute))
			return true;
	}
	
	return false;
}

ACString CDlgActivity::GetRequireLevelStr(const Activity* pAct)
{
	ACString str;
	if( pAct->p->require_level[0] == 0 && pAct->p->require_level[1] == 0 )
		str = GetStringFromTable(8016);
	else if( pAct->p->require_level[0] == 0 )
		str.Format(GetStringFromTable(8013), pAct->p->require_level[1]);
	else if( pAct->p->require_level[1] == 0 )
		str.Format(GetStringFromTable(8012), pAct->p->require_level[0]);
	else if( pAct->p->require_level[0] == pAct->p->require_level[1] )
		str.Format(_AL("%d"), pAct->p->require_level[0]);
	else
		str.Format(GetStringFromTable(8011), pAct->p->require_level[0], pAct->p->require_level[1]);
	return str;
}

ACString CDlgActivity::GetRequireCountStr(const Activity* pAct)
{
	ACString str;
	if( pAct->p->require_num[0] == 0 && pAct->p->require_num[1] == 0 )
		str = GetStringFromTable(8016);
	else if( pAct->p->require_num[0] == 0 )
		str.Format(GetStringFromTable(8015), pAct->p->require_num[1]);
	else if( pAct->p->require_num[1] == 0 )
		str.Format(GetStringFromTable(8014), pAct->p->require_num[0]);
	else if( pAct->p->require_num[0] == pAct->p->require_num[1] )
		str.Format(_AL("%d"), pAct->p->require_num[0]);
	else
		str.Format(GetStringFromTable(8011), pAct->p->require_num[0], pAct->p->require_num[1]);
	return str;
}

ACString CDlgActivity::GetRequireTimeStr(const Activity* pAct, bool bFullTime)
{
	ACString str;

	if( bFullTime )
	{
		if( pAct->p->time_type == 0 )
		{
			bool bFirst = true;
			str = GetStringFromTable(8026);
			for( int i=0;i<7;i++ )
			{
				if( pAct->p->week & (1 << i) )
				{
					if( bFirst ) bFirst = false;
					else str += GetStringFromTable(268);
					str += GetStringFromTable(8019 + i);
				}
			}
		}
		else
		{
			bool yearly = (pAct->YearBegin() == 0);
			bool monthly = (pAct->MonthBegin() == 0);
			bool dayly = (pAct->DayBegin() == 0);
			
			// Consider 8 cases depending on the above 3 booleans
			
			// Case tuple: (yearly, monthly, dayly)
			// Case 0: (false, false, false)
			// Case 1: (true, false, false)
			// Case 2: (false, true, false)
			// Case 3: (false, false, true)
			// Case 4: (true, true, false)
			// Case 5: (true, false, true)
			// Case 6: (false, true, true)
			// Case 7: (true, true, true)
			
			if (yearly)
			{
				if (monthly)
				{
					if (dayly)
					{
						// Case 7: happens each day
						str = GetStringFromTable(8027);
					}
					else
					{
						// Case 4: happens each year、month but within day range
						if( pAct->DayBegin() != pAct->DayEnd() )
							str.Format(GetStringFromTable(8028), pAct->DayBegin(), pAct->DayEnd());
						else
							str.Format(GetStringFromTable(8029), pAct->DayBegin());
					}
				}
				else
				{
					if (dayly)
					{
						// Case 5: happens each year、day but within month range
						if( pAct->MonthBegin() != pAct->MonthEnd() )
							str.Format(GetStringFromTable(8030), pAct->MonthBegin(), pAct->MonthEnd());
						else
							str.Format(GetStringFromTable(8031), pAct->MonthBegin());
					}
					else
					{
						// Case 1: happens each year but within month and day range
						if( pAct->MonthBegin() != pAct->MonthEnd() || pAct->DayBegin() != pAct->DayEnd() )
							str.Format(GetStringFromTable(8032), pAct->MonthBegin(), pAct->DayBegin(), pAct->MonthEnd(), pAct->DayEnd());
						else
							str.Format(GetStringFromTable(8033), pAct->MonthBegin(), pAct->DayBegin());
					}
				}
			}
			else
			{
				if (monthly)
				{
					if (dayly)
					{
						// Case 6: happens each month、day but within year range
						if( pAct->YearBegin() != pAct->YearEnd() )
							str.Format(GetStringFromTable(8034), pAct->YearBegin(), pAct->YearEnd());
						else
							str.Format(GetStringFromTable(8035), pAct->YearBegin());
					}
					else
					{
						// Case 2: happens each month but within year and day range
						if( pAct->YearBegin() != pAct->YearEnd() && pAct->DayBegin() != pAct->DayEnd() )
							str.Format(GetStringFromTable(8036), pAct->YearBegin(), pAct->YearEnd(), pAct->DayBegin(), pAct->DayEnd());
						else if( pAct->DayBegin() != pAct->DayEnd() )
							str.Format(GetStringFromTable(8037), pAct->YearBegin(), pAct->DayBegin(), pAct->DayEnd());
						else if( pAct->YearBegin() != pAct->YearEnd() )
							str.Format(GetStringFromTable(8038), pAct->YearBegin(), pAct->YearEnd(), pAct->DayBegin());
						else
							str.Format(GetStringFromTable(8039), pAct->YearBegin(), pAct->DayBegin());
					}
				}
				else
				{
					if (dayly)
					{
						// Case 3: happens each day but within year and month range
						if( pAct->YearBegin() != pAct->YearEnd() || pAct->MonthBegin() != pAct->MonthEnd() )
							str.Format(GetStringFromTable(8040), pAct->YearBegin(), pAct->MonthBegin(), pAct->YearEnd(), pAct->MonthEnd());
						else
							str.Format(GetStringFromTable(8041), pAct->YearBegin(), pAct->MonthBegin());
					}
					else
					{
						// case 0: happens within year、month、day range
						if( pAct->YearBegin() != pAct->YearEnd() || pAct->MonthBegin() != pAct->MonthEnd() || pAct->DayBegin() != pAct->DayEnd() )
							str.Format(GetStringFromTable(8042), pAct->YearBegin(), pAct->MonthBegin(), pAct->DayBegin(), pAct->YearEnd(), pAct->MonthEnd(), pAct->DayEnd());
						else
							str.Format(GetStringFromTable(8043), pAct->YearBegin(), pAct->MonthBegin(), pAct->DayBegin());
					}
				}
			}
		}
	}

	ACString strTime;
	if( pAct->full_day )
		strTime = GetStringFromTable(8017);
	else
		strTime.Format(GetStringFromTable(8018), pAct->p->daytime[0], pAct->p->daytime[1], pAct->p->daytime[2], pAct->p->daytime[3]);

	if( bFullTime )
	{
		ACString strTemp;
		strTemp.Format(GetStringFromTable(8044), strTime);
		str += strTemp;
	}
	else
		str += strTime;
	return str;
}

struct ActivitySorterAsc
{
	bool operator () (const CDlgActivity::Activity* rhs1, const CDlgActivity::Activity* rhs2)
	{
		if( rhs1->IsRecommend() )
		{
			if( rhs2->IsRecommend() )
				return rhs1->p->require_level[0] < rhs2->p->require_level[0];
			else
				return true;
		}
		else
		{
			if( rhs2->IsRecommend() )
				return false;
			else
				return rhs1->p->require_level[0] < rhs2->p->require_level[0];
		}
	}
};

struct ActivitySorterDesc
{
	bool operator () (const CDlgActivity::Activity* rhs1, const CDlgActivity::Activity* rhs2)
	{
		if( rhs1->IsRecommend() )
		{
			if( rhs2->IsRecommend() )
				return rhs1->p->require_level[0] > rhs2->p->require_level[0];
			else
				return true;
		}
		else
		{
			if( rhs2->IsRecommend() )
				return false;
			else
				return rhs1->p->require_level[0] > rhs2->p->require_level[0];
		}
	}
};

void CDlgActivity::OnCommand_SortList(const char* szCommand)
{
	m_bSortByDesc = !m_bSortByDesc;
	if( m_bSortByDesc )
		CECAutoTeamConfig::Instance().Sort(ActivitySorterDesc());
	else
		CECAutoTeamConfig::Instance().Sort(ActivitySorterAsc());

	if( szCommand != NULL )
		UpdateActivityList();
}

bool CDlgActivity::IsShowAllActivity() {
	return m_pChk_ShowAllActivty->IsChecked();
}
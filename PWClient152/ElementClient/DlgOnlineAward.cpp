// Filename	: DlgOnlineAward.cpp
// Creator	: Wang Yongdong
// Date		: 2012/05/17

#include "AFI.h"
#include "DlgOnlineAward.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Global.h"
#include "elementdataman.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_GameSession.h"
#include "AUIStillImageButton.h"
#include "EC_FixedMsg.h"
#include "EC_World.h"
#include "EC_TimeSafeChecker.h"

#define new A_DEBUG_NEW

#define INTERAL_LONG	618
#define INTERAL_SHORT	100

const int MAX_AWARD_ITEM = 8;

AUI_BEGIN_COMMAND_MAP(CDlgOnlineAward, CDlgBase)
AUI_ON_COMMAND("add*",			OnCommand_Add)
AUI_ON_COMMAND("confirm",		OnCommand_Confirm)
AUI_ON_COMMAND("Btn_Addexp2",	OnCommand_AddExp2)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgOnlineAward, CDlgBase)

AUI_ON_EVENT("Btn_Up*",			WM_LBUTTONDOWN,		OnEventLButtonDown_add)
AUI_ON_EVENT("Btn_Up*",			WM_LBUTTONDBLCLK,	OnEventLButtonDown_add)
AUI_ON_EVENT("Btn_Down*",		WM_LBUTTONDOWN,		OnEventLButtonDown_minus)
AUI_ON_EVENT("Btn_Down*",		WM_LBUTTONDBLCLK,	OnEventLButtonDown_minus)
AUI_ON_EVENT("Btn_Up*",			WM_LBUTTONUP,		OnEventLBUttonUp_stop)
AUI_ON_EVENT("Btn_Down*",		WM_LBUTTONUP,		OnEventLBUttonUp_stop)

AUI_END_EVENT_MAP()

COnlineAwardCtrl::COnlineAwardCtrl(CECHostPlayer* player):m_pHostPlayer(player)
{
	memset(m_type_state,0,sizeof(m_type_state));
	m_total_time = 0;
	m_cur_total_time = 0;
	int i = 0;
	for (;i<MAX_AWARD_TYPE;i++)
	{
		m_award_data[i].type = -1;
		m_award_data[i].time = 0;
		m_award_data[i].reserved = 0;
	}
	m_pAwardData = NULL;

	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);
	
	while(tid)
	{
		if(DataType == DT_ONLINE_AWARDS_CONFIG)
		{
			m_pAwardData = (ONLINE_AWARDS_CONFIG *)pDataMan->get_data_ptr(tid,ID_SPACE_CONFIG, DataType);
			m_total_time = m_pAwardData->max_time;
			break;
		}
		tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
	}
}
void COnlineAwardCtrl::OnRecvOnlineAwardData(int total_time,int count,S2C::cmd_online_award_data::_entry* pEntry)
{
	m_cur_total_time = total_time;

	int i(0);
	for (;i<count;i++)
	{
		if (pEntry[i].type>=0 && pEntry[i].type<MAX_AWARD_TYPE)
		{
			m_award_data[pEntry[i].type].time = pEntry[i].time;
		}
	}

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		((CDlgOnlineAward*)pGameUI->GetDialog("Win_AddExp2"))->UpdateTime(this);
	}
}
void COnlineAwardCtrl::OnToggleOnlineAward(int type,int active)
{
	m_type_state[type] = active;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		((CDlgOnlineAward*)pGameUI->GetDialog("Win_AddExp2"))->ToggleStartFlag(active!=0,false);
	}
}
int COnlineAwardCtrl::ChargeAwardTime(int type,int count)
{
	if(type>= MAX_AWARD_TYPE || type<0 || !m_pAwardData) return -1;	

	abase::vector<C2S::cmd_recharge_online_award::entry> vec;
	CECInventory* pIvtr = m_pHostPlayer->GetPack();

	int i(0);	
	int realCount(0); // total count

	for (;i<MAX_AWARD_ITEM;i++)
	{
		int item_tid = m_pAwardData->choice[type].ids[i];
		if(item_tid<=0) continue;

		int baseIdx = 0;
		while(count>0 && baseIdx< pIvtr->GetSize())
		{
			C2S::cmd_recharge_online_award::entry e;
			int idx = pIvtr->FindItem(item_tid,baseIdx);
			if(idx<0) break;

			CECIvtrItem* pItem = pIvtr->GetItem(idx);
			if (pItem)
			{
				e.count = count > pItem->GetCount() ? pItem->GetCount() : count;
				count -= e.count;
				e.index = idx;
				e.type = item_tid;
				baseIdx = idx+1;
				
				realCount += e.count;

				vec.push_back(e);
			}	
		}
	}	
	
	if (m_award_data[type].time + realCount * m_pAwardData->choice[type].time <= 7 * 24 * 3600)
	{
		g_pGame->GetGameSession()->c2s_CmdRechargeOnlineAward(type,vec.size(),vec.begin());
		return 1;
	}
	else
		return 0; // 超出时间限制	
}
void COnlineAwardCtrl::ToggleAwardType(int type,int active)
{
	if(type>=0 && type<MAX_AWARD_TYPE)
		g_pGame->GetGameSession()->c2s_CmdToggleOnlineAward(type,active);
}
int COnlineAwardCtrl::GetActiveType() const 
{
	int i;
	for (i=0;i<MAX_AWARD_TYPE;i++)
	{
		if(m_type_state[i]) return i;
	}

	return -1;
}

int COnlineAwardCtrl::GetAwardTime(int type) const
{
	if (type>=0 && type < MAX_AWARD_TYPE)
	{
		return m_award_data[type].time;
	}

	return 0;
}
int COnlineAwardCtrl::GetTotalItemCount(int type)
{
	if(type<0 || type>=MAX_AWARD_TYPE) return -1;

	int i(0), ret(0);
	CECInventory* pIvtr = m_pHostPlayer->GetPack();

	for (;i<MAX_AWARD_ITEM;i++)
	{
		int item_tid = m_pAwardData->choice[type].ids[i];
		if(item_tid)
			ret += pIvtr->GetItemTotalNum(item_tid);
	}

	return ret;
}
//////////////////////////////////////////////////////////////////////////

CDlgOnlineAward::CDlgOnlineAward()
{
	m_iIntervalTime = INTERAL_LONG;
	m_dwLastTime = 0;
	m_pButtonPress = NULL;
	m_pProgress = NULL;
	m_pRadion1 = NULL;
	m_pRadion2 = NULL;
	m_award_type = 0;

	m_item_count[0] = 1;
	m_item_count[1] = 1;
	m_item_count[2] = 1;
	m_item_count[3] = 1;

	m_bStart = false;
	m_bShowFlag = false;
} 

CDlgOnlineAward::~CDlgOnlineAward()
{
}
void CDlgOnlineAward::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	if (!Is2QDlg())
	{
		DDX_EditBox(bSave,"Edt_Number1", m_item_count[0]);
		DDX_EditBox(bSave,"Edt_Number2", m_item_count[1]);
		
		DDX_Control("Rdo_Exp1",m_pRadion1);
		DDX_Control("Rdo_Exp2",m_pRadion2);
		DDX_Control("Pro_Time",m_pProgress);
	}
}
void CDlgOnlineAward::OnShowDialog()
{
	if(Is2QDlg())
	{
		GetDlgItem("Btn_Addexp2")->SetFlash(true);
	//	SetCanEscClose(false);
		return;
	}

	int i=0;
	for (i=0;i<MAX_AWARD_TYPE;i++)
	{
		m_item_count[i] = 1;
	}

	UpdateData(false);

	ToggleStartFlag(m_bStart,true);
}
bool CDlgOnlineAward::OnInitDialog()
{
	if (!Is2QDlg())
	{
		m_pRadion1->Check(true);
	}
	
	return true;
}
void CDlgOnlineAward::OnTick()
{
	if(Is2QDlg()) return;

	if (m_pButtonPress != NULL )
	{
		if (AUISTILLIMAGEBUTTON_STATE_NORMAL != m_pButtonPress->GetState())
		{
			if (CECTimeSafeChecker::ElapsedTimeFor(m_dwLastTime) >= (DWORD)m_iIntervalTime)
			{
				if (m_bAdd)
				{
					PropertyAdd(m_pButtonPress);
				}
				else
					PropertyMinus(m_pButtonPress);
				m_dwLastTime += m_iIntervalTime;
				
				if (m_iIntervalTime == INTERAL_LONG)
					m_iIntervalTime = INTERAL_SHORT;
			}
		}
		else
		{
			m_pButtonPress = NULL;
		}
	}
	// hack code： 解决切换界面后 按钮文字恢复的问题。DlgTheme.cpp：Line 622：AUI_SWAP_RES("Text Content"); // we switch the text on a button
	if (m_bStart)
	{
		ACString strText = GetDlgItem("Btn_Confirm")->GetText();
		ACString strShould = GetGameUIMan()->GetStringFromTable(9632);
		if (strText != strShould)
		{
			GetDlgItem("Btn_Confirm")->SetText(strShould);
		}
	}
}
void CDlgOnlineAward::RestartWhenLevelup()
{
	if (m_bStart)
	{
		COnlineAwardCtrl* pCtrl = g_pGame->GetGameRun()->GetHostPlayer()->GetOnlineAwardCtrl();
		
		int sel = -1;
		if(m_pRadion1->IsChecked()) sel = 0;
		else if(m_pRadion2->IsChecked()) sel = 1;
		if(sel<0)
			return ;
		
		if (pCtrl)
		{
			pCtrl->ToggleAwardType(sel,1);	//开启
		}
	}
}
void CDlgOnlineAward::OnCommand_Confirm(const char* szCommand)
{
	if (GetHostPlayer()->GetCoolTime(GP_CT_TOGGLE_ONLINE_AWARD))
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
		return;
	}

	COnlineAwardCtrl* pCtrl = g_pGame->GetGameRun()->GetHostPlayer()->GetOnlineAwardCtrl();
	
	int sel = -1;
	if(m_pRadion1->IsChecked()) sel = 0;
	else if(m_pRadion2->IsChecked()) sel = 1;
	if(sel<0)
		return ;

	if (pCtrl)
	{
		pCtrl->ToggleAwardType(sel,m_bStart ? 0:1);
	}
}
void CDlgOnlineAward::OnCommand_Add(const char* szCommand)
{
	UpdateData();
	int nSet = atoi(szCommand + strlen("add"));

	nSet -= 1;

	COnlineAwardCtrl* pCtrl = g_pGame->GetGameRun()->GetHostPlayer()->GetOnlineAwardCtrl();
	if(pCtrl)
	{
		if(m_item_count[nSet] <= pCtrl->GetTotalItemCount(nSet))
		{
			int ret = pCtrl->ChargeAwardTime(nSet,m_item_count[nSet]);
			if(ret ==0)
				GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(9702),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
		}
		else
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(8501),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgOnlineAward::OnEventLButtonDown_add(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PropertyAdd(pObj);
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
	m_iIntervalTime = INTERAL_LONG;
	m_pButtonPress = pObj;
	m_bAdd = true;
	
}

void CDlgOnlineAward::PropertyAdd(AUIObject *pObj)
{
	UpdateData();
	const char *szCommand = pObj->GetName();
	COnlineAwardCtrl* pCtrl = g_pGame->GetGameRun()->GetHostPlayer()->GetOnlineAwardCtrl();
	if( 0 == stricmp(szCommand, "Btn_Up1") )
	{
		if(pCtrl && m_item_count[AWARD_TYPE_EXP1] < pCtrl->GetTotalItemCount(AWARD_TYPE_EXP1))
			m_item_count[AWARD_TYPE_EXP1]++;
	}
	else if( 0 == stricmp(szCommand, "Btn_Up2") )
	{
		if(pCtrl && m_item_count[AWARD_TYPE_EXP2] < pCtrl->GetTotalItemCount(AWARD_TYPE_EXP2))
			m_item_count[AWARD_TYPE_EXP2]++;
	}
	UpdateData(false);
}
void CDlgOnlineAward::OnEventLButtonDown_minus(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PropertyMinus(pObj);
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
	m_iIntervalTime = INTERAL_LONG;
	m_pButtonPress = pObj;
	m_bAdd = false;
}

void CDlgOnlineAward::PropertyMinus(AUIObject *pObj)
{
	UpdateData();
	const char *szCommand = pObj->GetName();
	if( 0 == stricmp(szCommand, "Btn_Down1") )
	{
		if(m_item_count[AWARD_TYPE_EXP1]>0)
			m_item_count[AWARD_TYPE_EXP1]--;
	}
	else if( 0 == stricmp(szCommand, "Btn_Down2") )
	{
		if(m_item_count[AWARD_TYPE_EXP2]>0)
			m_item_count[AWARD_TYPE_EXP2]--;
	}
	UpdateData(false);
}

void CDlgOnlineAward::OnEventLBUttonUp_stop(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	m_pButtonPress = NULL;
}

void CDlgOnlineAward::ToggleStartFlag(bool bStart,bool bInit) 
{
	m_bStart = bStart;
	
	GetDlgItem("Btn_Confirm")->SetText(GetGameUIMan()->GetStringFromTable(m_bStart ? 9632:9631));
	m_pRadion1->Enable(!m_bStart);
	m_pRadion2->Enable(!m_bStart);
	m_pProgress->SetFlash(m_bStart);

	if(!bInit)
		GetGameUIMan()->AddChatMessage(GetStringFromTable(m_bStart ? 9700:9701), GP_CHAT_SYSTEM);
}
ACString CDlgOnlineAward::GetDetailTime(int t) // 
{
	const int TIME_DAY = 3600 * 24;
	const int TIME_HOUR = 3600;
	const int TIME_MINUTE = 60;

	ACString str = GetGameUIMan()->GetFormatTime(t);
	ACString ret = str;
	ACString strTime;
/*	if (t> 3600 * 25)
	{
		strTime.Format(GetStringFromTable(5537),(t % TIME_HOUR) / TIME_MINUTE,t % TIME_MINUTE);
		ret = str + strTime;
	}
	else if(t>3660)
	{
		strTime.Format(GetStringFromTable(5538),t % TIME_MINUTE);
		ret = str + strTime;
	}*/
	if (t> 3600 * 25)
	{
		strTime.Format(GetStringFromTable(1602),(t % TIME_HOUR) / TIME_MINUTE);
		ret = str + strTime;
	}

	return ret;
}
void CDlgOnlineAward::UpdateTime(COnlineAwardCtrl* pCtrl)
{
	if (pCtrl && pCtrl->GetTotalAwardTime())
	{
		ACString str = GetDetailTime(pCtrl->GetCurTotalAwardTime());
		GetDlgItem("Lab_Time")->SetText(str);

		m_pProgress->SetProgress(pCtrl->GetCurTotalAwardTime() * AUIPROGRESS_MAX / pCtrl->GetTotalAwardTime() );	
	
		str = GetDetailTime(pCtrl->GetAwardTime(AWARD_TYPE_EXP1));		
		GetDlgItem("Lab_Time1")->SetText(str);

		str = GetDetailTime(pCtrl->GetAwardTime(AWARD_TYPE_EXP2));			
		GetDlgItem("Lab_Time2")->SetText(str);
	}
}

bool CDlgOnlineAward::Is2QDlg()
{
	return m_szName == "Win_Addexp2Pop";
}
void CDlgOnlineAward::OnCommand_AddExp2(const char* szCommand)
{
	if (Is2QDlg())
	{
		PAUIDIALOG pAddExp2 = GetGameUIMan()->GetDialog("Win_AddExp2");
		if(pAddExp2) pAddExp2->Show(!pAddExp2->IsShow());
	}
}
void CDlgOnlineAward::SwitchGuajiIsland()
{	
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();

	if(!m_bShowFlag && pWorld->IsGuajiIslandMap())
	{
		GetGameUIMan()->m_pDlgOnlineAward->Show(true);
		GetGameUIMan()->m_pDlgAddExp2Q->Show(true);
		m_bShowFlag = true;
	}
	if (m_bShowFlag && !pWorld->IsGuajiIslandMap())
	{
		m_bShowFlag = false;
		GetGameUIMan()->m_pDlgOnlineAward->Show(false);
		GetGameUIMan()->m_pDlgAddExp2Q->Show(false);
	}
}
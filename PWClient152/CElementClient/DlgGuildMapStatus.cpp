// Filename	: DlgGuildMapStatus.cpp
// Creator	: Xiao Zhou
// Date		: 2005/1/5

#include "DlgGuildMapStatus.h"
#include "DlgGuildMap.h"
#include "AUIListBox.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_Faction.h"
#include "EC_IvtrItem.h"
#include "globaldataman.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_Configs.h"
#include "EC_TimeSafeChecker.h"

#include "AUIStillImageButton.h"
#include "A2DSprite.h"

#define new A_DEBUG_NEW

#define INTERAL_LONG	618
#define INTERAL_SHORT	100

const int l_max_sliver = 99;

AUI_BEGIN_COMMAND_MAP(CDlgGuildMapStatus, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("allbattletime",	OnCommandAllBattleTime)
AUI_ON_COMMAND("Btn_Max",		OnCommandMax)
AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgGuildMapStatus, CDlgBase)

AUI_ON_EVENT("Btn_Up",			WM_LBUTTONDOWN,		OnEventLButtonDown_add)
AUI_ON_EVENT("Btn_Up",			WM_LBUTTONDBLCLK,	OnEventLButtonDown_add)
AUI_ON_EVENT("Btn_Down",		WM_LBUTTONDOWN,		OnEventLButtonDown_minus)
AUI_ON_EVENT("Btn_Down",		WM_LBUTTONDBLCLK,	OnEventLButtonDown_minus)
AUI_ON_EVENT("Btn_Up",			WM_LBUTTONUP,		OnEventLBUttonUp_stop)
AUI_ON_EVENT("Btn_Down",		WM_LBUTTONUP,		OnEventLBUttonUp_stop)

AUI_END_EVENT_MAP()


CDlgGuildMapStatus::CDlgGuildMapStatus()
{
}

CDlgGuildMapStatus::~CDlgGuildMapStatus()
{
}

void CDlgGuildMapStatus::OnCommandCancel(const char * szCommand)
{
	if( stricmp(m_szName, "Win_GMapChallenge") == 0 )
		Show(false);
	else
		GetGameUIMan()->m_pDlgGuildMap->OnCommandCancel("");
}

void CDlgGuildMapStatus::OnCommandAllBattleTime(const char * szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_GMapBattleTime");
	PAUILISTBOX pList = (PAUILISTBOX)pDlg->GetDlgItem("Lst_Time");
	pList->ResetContent();
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	CECFactionMan *pMan = GetGame()->GetFactionMan();
	
	DWORD i;
	for(i = 0; i < info->size(); i++)
		if( (*info)[i].battle_time != 0 )
		{
			Faction_Info *finfo;
			ACString strText, strAttacker, strDefender;
			finfo = pMan->GetFaction((*info)[i].id_challenger);
			if( finfo )
				strAttacker = finfo->GetName();
			else
				strAttacker = GetStringFromTable(574);
			finfo = pMan->GetFaction((*info)[i].id_owner);
			if( finfo )
				strDefender = finfo->GetName();
			else
				strDefender = GetStringFromTable(574);
			long stime = (*info)[i].battle_time;
			stime -= GetGame()->GetTimeZoneBias() * 60;	// localtime = UTC - bias, which bias is in minute
			tm *gtime = gmtime(&stime);
			strText.Format(GetStringFromTable(785), (*info)[i].name,
				strAttacker, strDefender,
				gtime->tm_mon + 1, gtime->tm_mday, gtime->tm_hour, gtime->tm_min);
			pList->AddString(strText);
		}
	pDlg->Show(true, true);
	pDlg->SetPosEx(0, 0, alignCenter, alignCenter);
	pDlg->SetCanMove(false);
}

void CDlgGuildMapStatus::OnCommandConfirm(const char * szCommand)
{
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	int nDeposit = a_atoi(m_pTxt_NewMoney->GetText());
	if( nDeposit >= (int)m_pTxt_NewMoney->GetData() )
	{
		int moneyNum = GetHostPlayer()->GetPack()->GetItemTotalNum(21652); // 银票数量

		if(nDeposit > l_max_sliver )
		{
			GetGameUIMan()->MessageBox("ChallengeMax", GetStringFromTable(9752),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		else if( nDeposit > moneyNum )
		{
			GetGameUIMan()->MessageBox("ChallengeErr", GetStringFromTable(9750),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
// 		else if( (*info)[m_nIndex].id_owner != 0 && nDeposit * 9 / 10 > (int)GetData() )
// 		{
// 			ACString strText;
// 			strText.Format(GetStringFromTable(813), GetGameUIMan()->GetFormatNumber(nDeposit),
// 				GetGameUIMan()->GetFormatNumber(GetData()));
// 			PAUIDIALOG pMsgBox;
// 			GetGameUIMan()->MessageBox("Game_GuildChallenge", strText,
// 				MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
// 			pMsgBox->SetData((*info)[m_nIndex].id);
// 			pMsgBox->SetDataPtr((void*)nDeposit);
// 		}
		else
		{
 			abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
// 			GetGameSession()->battle_Challenge((*info)[m_nIndex].id, 
// 				GetHostPlayer()->GetFactionID(), nDeposit);
// 			Show(false);
			ACString str;
			PAUIDIALOG pDlg;
			str.Format(GetStringFromTable(9751),nDeposit,m_pTxt_Name->GetText());
			GetGameUIMan()->MessageBox("ChallengeConfirm", str,
				MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160),&pDlg);

			pDlg->SetData((*info)[m_nIndex].id);
			pDlg->SetDataPtr((int*)nDeposit,"int");
		}
	}
}

bool CDlgGuildMapStatus::OnInitDialog()
{
	m_pTxt_Name = (PAUILABEL)GetDlgItem("Txt_Name");
	m_pTxt_Level = (PAUILABEL)GetDlgItem("Txt_Level");
	m_pTxt_Attack = (PAUILABEL)GetDlgItem("Txt_Attack");
	m_pTxt_Win = (PAUILABEL)GetDlgItem("Txt_Win");
	m_pTxt_Attach = (PAUILABEL)GetDlgItem("Txt_Attach");
	m_pTxt_Time = (PAUILABEL)GetDlgItem("Txt_Time");

	m_pTxt_NewMoney = GetDlgItem("Txt_NewMoney");
	m_nIndex = -1;

	return true;
}

void CDlgGuildMapStatus::OnShowDialog()
{
	m_pTxt_Name->SetText(_AL(""));
	m_pTxt_Level->SetText(_AL(""));
	if(m_pTxt_Attack) m_pTxt_Attack->SetText(_AL(""));
	m_pTxt_Win->SetText(_AL(""));
	m_pTxt_Attach->SetText(_AL(""));
	m_pTxt_Time->SetText(_AL(""));

	m_iIntervalTime = INTERAL_LONG;
	m_dwLastTime = 0;
	m_dwStartTime = 0;
	m_pButtonPress = NULL;
	m_bAdd = false;

	// align the status dialog pos
	PAUIOBJECT pGuildMap = GetGameUIMan()->m_pDlgGuildMap->GetDlgItem("GuildMap");
	SetPosEx(pGuildMap->GetPos().x + GetGameUIMan()->m_pDlgGuildMap->GetSize().cx / 20, 
			 0, AUIDialog::alignNone, AUIDialog::alignCenter);
}

void CDlgGuildMapStatus::SetIndex(int n)
{
	if( n != -1 && n != m_nIndex )
	{
		abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
		CECFactionMan *pMan = GetGame()->GetFactionMan();
		if( (*info)[n].id_owner )
			pMan->GetFaction((*info)[n].id_owner, true);
		if( (*info)[n].id_challenger )
			pMan->GetFaction((*info)[n].id_challenger, true);
		if( (*info)[n].id_tempchallenger )
			pMan->GetFaction((*info)[n].id_tempchallenger, true);
	}
	m_nIndex = n;
	if( stricmp(GetName(), "Win_GMapChallenge") == 0 )
	{
		abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
		int nDeposit;
		ACHAR szText[20];
// 		if( (*info)[m_nIndex].id_tempchallenger != 0 )
// 			nDeposit = (*info)[m_nIndex].deposit + 100000;
// 		else if( (*info)[m_nIndex].type == 1 )
// 			nDeposit = 500000;
// 		else if( (*info)[m_nIndex].type == 2 )
// 			nDeposit = 1000000;
// 		else
// 			nDeposit = 2000000;

	//	nDeposit = 100000;
		nDeposit = 1;
		a_sprintf(szText, _AL("%d"), nDeposit);
		m_pTxt_NewMoney->SetText(szText);
		m_pTxt_NewMoney->SetData(nDeposit);
	}
}

void CDlgGuildMapStatus::OnTick()
{
	if( m_nIndex == -1 )
	{
		OnShowDialog();
		return;
	}

	// align the status dialog pos
	PAUIOBJECT pGuildMap = GetGameUIMan()->m_pDlgGuildMap->GetDlgItem("GuildMap");
	SetPosEx(pGuildMap->GetPos().x + GetGameUIMan()->m_pDlgGuildMap->GetSize().cx / 20, 
			 0, AUIDialog::alignNone, AUIDialog::alignCenter);

	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	CECFactionMan *pMan = GetGame()->GetFactionMan();
	
	int domainLevel = (*info)[m_nIndex].type;
	a_Clamp(domainLevel, 1, 3);

	const DOMAIN_INFO &domainInfo = (*info)[m_nIndex];
	ACString strText;
	if (GetGame()->GetConfigs()->GetShowIDFlag()){
		strText.Format(_AL("%s(%d)"), domainInfo.name, domainInfo.id);
	}else{
		strText = domainInfo.name;
	}
	m_pTxt_Name->SetText(strText);
	strText.Format(_AL("%d"), 4 - domainLevel);
	m_pTxt_Level->SetText(strText);
	
	const CDlgGuildMap::BonusInfo& bonusInfo = GetGameUIMan()->m_pDlgGuildMap->GetBonusInfo();
	CECIvtrItem* pItem = bonusInfo.item_id <= 0 ? NULL : CECIvtrItem::CreateItem(bonusInfo.item_id, 0, 0);
	int rewardCount[] = {bonusInfo.count1, bonusInfo.count2, bonusInfo.count3};
	if(pItem)
	{
		strText.Format(_AL("%s(%d)"), pItem->GetName(), rewardCount[domainLevel - 1]);
		delete pItem;
	}
	else
	{
		strText.Empty();
	}
	m_pTxt_Win->SetText(strText);

	if( (*info)[m_nIndex].id_owner != 0 )
	{
		Faction_Info *finfo = pMan->GetFaction((*info)[m_nIndex].id_owner, false);
		if( finfo )
			m_pTxt_Attach->SetText(finfo->GetName());
		else
			m_pTxt_Attach->SetText(GetStringFromTable(574));
	}
	else
		m_pTxt_Attach->SetText(GetStringFromTable(786));

	if( stricmp(GetName(), "Win_GMapStatus1") == 0 )
	{
		if(m_pTxt_Attack)
		{
			if( (*info)[m_nIndex].id_challenger > 0)
			{
				Faction_Info *finfo = pMan->GetFaction((*info)[m_nIndex].id_challenger, false);
				if( finfo )
					m_pTxt_Attack->SetText(finfo->GetName());
				else
					m_pTxt_Attack->SetText(GetStringFromTable(574));
			}
			else
			{
				m_pTxt_Attack->SetText(GetStringFromTable(786));
			}
		}
		
		if( (*info)[m_nIndex].battle_time != 0 )
		{
			long stime = (*info)[m_nIndex].battle_time;
			stime -= GetGame()->GetTimeZoneBias() * 60;	// localtime = UTC - bias, which bias is in minute
			tm *gtime = gmtime(&stime);
			strText.Format(GetStringFromTable(789), 
				gtime->tm_mon + 1, gtime->tm_mday, gtime->tm_hour, gtime->tm_min);
			m_pTxt_Time->SetText(strText);
		}
		else
			m_pTxt_Time->SetText(_AL(""));
	}
	else
	{
		int tempchallenger = (*info)[m_nIndex].id_tempchallenger;
		if(m_pTxt_Attack)
		{
			if(tempchallenger > 0)
			{
				Faction_Info *finfo = pMan->GetFaction(tempchallenger, true);
				if( finfo )
					m_pTxt_Attack->SetText(finfo->GetName());
				else
					m_pTxt_Attack->SetText(GetStringFromTable(574));
			}
			else
			{
				m_pTxt_Attack->SetText(GetStringFromTable(786));
			}
		}
		
		if( GetGame()->GetServerGMTTime() <= (*info)[m_nIndex].cutoff_time )
		{
			long stime = (*info)[m_nIndex].cutoff_time;
			stime -= GetGame()->GetTimeZoneBias() * 60;	// localtime = UTC - bias, which bias is in minute
			tm *gtime = gmtime(&stime);
			strText.Format(GetStringFromTable(789), 
				gtime->tm_mon + 1, gtime->tm_mday, gtime->tm_hour, gtime->tm_min);
			m_pTxt_Time->SetText(strText);
		}
		else
			m_pTxt_Time->SetText(GetStringFromTable(790));

		if( m_pTxt_NewMoney )
		{
			int nDeposit = a_atoi(m_pTxt_NewMoney->GetText());
			PAUIOBJECT pBtn_Confirm = GetDlgItem("Btn_Confirm");
			pBtn_Confirm->Enable(nDeposit >= (int)m_pTxt_NewMoney->GetData());
		}
	}

	// up down 按钮处理
	if (stricmp(GetName(), "Win_GMapChallenge") == 0 && m_pButtonPress != NULL )
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
}
void CDlgGuildMapStatus::OnEventLButtonDown_add(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PropertyAdd(pObj);
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
	m_iIntervalTime = INTERAL_LONG;
	m_pButtonPress = pObj;
	m_bAdd = true;	
}
void CDlgGuildMapStatus::OnEventLButtonDown_minus(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PropertyMinus(pObj);
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
	m_iIntervalTime = INTERAL_LONG;
	m_pButtonPress = pObj;
	m_bAdd = false;
}
void CDlgGuildMapStatus::OnEventLBUttonUp_stop(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	m_pButtonPress = NULL;
}
void CDlgGuildMapStatus::PropertyAdd(AUIObject *pObj)
{
	int nDeposit = a_atoi(m_pTxt_NewMoney->GetText());
	int moneyNum = GetHostPlayer()->GetPack()->GetItemTotalNum(21652); // 银票数量
	if(nDeposit<l_max_sliver)
		nDeposit++;
	ACString str;
	str.Format(_AL("%d"),nDeposit);
	m_pTxt_NewMoney->SetText(str);
}
void CDlgGuildMapStatus::PropertyMinus(AUIObject *pObj)
{
	int nDeposit = a_atoi(m_pTxt_NewMoney->GetText());
	if(nDeposit>1)
		nDeposit--;
	ACString str;
	str.Format(_AL("%d"),nDeposit);
	m_pTxt_NewMoney->SetText(str);
}
void CDlgGuildMapStatus::OnCommandMax(const char* szCommand)
{
	ACString str;
	str.Format(_AL("%d"),l_max_sliver);
	m_pTxt_NewMoney->SetText(str);
}

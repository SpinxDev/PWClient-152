#include "DlgFortressWarList.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_Faction.h"
#include "EC_Game.h"
#include "EC_HostPlayer.h"
#include "CSplit.h"
#include "NetWork\\factionfortressbattlelist_re.hpp"
#include "Network\\rpcdata\\gfactionfortressbattleinfo"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressWarList, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Refresh",	OnCommand_Refresh)
AUI_ON_COMMAND("Btn_Enter",		OnCommand_Enter)
AUI_ON_COMMAND("Lst_GvG",	OnCommand_Lst_GvG)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressWarList, CDlgBase)
AUI_END_EVENT_MAP()

CDlgFortressWarList::CDlgFortressWarList()
{
	m_pBtn_Enter = NULL;
	m_pLst_GvG = NULL;
	m_status = ST_OPEN;
	m_bAllInfoReady = true;
}

bool CDlgFortressWarList::OnInitDialog()
{
	DDX_Control("Btn_Enter", m_pBtn_Enter);
	DDX_Control("Lst_GvG", m_pLst_GvG);
	return true;
}

void CDlgFortressWarList::OnShowDialog()
{
	OnCommand_Refresh(NULL);
}

void CDlgFortressWarList::OnCommand_CANCEL(const char *szCommand)
{
	Show(false);
	
	//	关闭相关对话框
	PAUIDIALOG pDlgInfo = GetGameUIMan()->GetDialog("Win_FortressInfo");
	if (pDlgInfo && pDlgInfo->IsShow())
		pDlgInfo->OnCommand("IDCANCEL");
	
	//	关闭 NPC 服务
	if (GetGameUIMan()->m_pCurNPCEssence != NULL)
		GetGameUIMan()->EndNPCService();
}

void CDlgFortressWarList::OnCommand_Enter(const char *szCommand)
{
	int idTarget = GetFortressToEnter();
	if (idTarget > 0)
	{
		GetGameSession()->factionFortress_Enter(idTarget);
		OnCommand_CANCEL(NULL);
	}
}

void CDlgFortressWarList::OnCommand_Refresh(const char *szCommand)
{
	GetGameSession()->factionFortress_BattleList();

	m_pLst_GvG->ResetContent();
	m_bAllInfoReady = true;
	OnListSelChange();
}

int	CDlgFortressWarList::GetFortressToEnter()
{
	int idTarget(0);

	while (true)
	{
		int count = m_pLst_GvG->GetCount();
		if (count <= 0)
			break;
		
		int nSel = m_pLst_GvG->GetCurSel();
		if (nSel < 0 || nSel >=count)
			break;
		
		int idMyFaction = GetHostPlayer()->GetFactionID();
		if (idMyFaction <= 0)
			break;
		
		idTarget = m_pLst_GvG->GetItemData(nSel, FORTRESS_LIST_OWNER_INDEX);
		if (idTarget != idMyFaction)
		{
			if (m_status != ST_BATTLE)
			{
				//	状态不正确，不能进入
				idTarget = 0;
				break;
			}
		}
		break;
	}

	return idTarget;
}

void CDlgFortressWarList::OnCommand_Lst_GvG(const char *szCommand)
{
	OnListSelChange();
}

void CDlgFortressWarList::OnPrtcFactionFortressBattleList_Re(const GNET::FactionFortressBattleList_Re *p)
{
	m_status = (FORTRESS_STATE)p->status;
	m_pBtn_Enter->Enable(false);
	m_pLst_GvG->ResetContent();

	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	ACString strText, strName1, strName2;

	m_bAllInfoReady = true;
	for (size_t i = 0; i < p->list.size(); ++ i)
	{
		const GNET::GFactionFortressBattleInfo &info = p->list[i];
		bool bName1OK = GetFactionName(info.factionid, strName1, true);
		bool bName2OK = GetFactionName(info.offense_faction, strName2, true);

		strText.Format(_AL("%s\t%s"), strName1, strName2);
		m_pLst_GvG->AddString(strText);

		//	记录数据用于进入战场
		m_pLst_GvG->SetItemData(m_pLst_GvG->GetCount()-1, info.factionid, FORTRESS_LIST_OWNER_INDEX);
		m_pLst_GvG->SetItemData(m_pLst_GvG->GetCount()-1, info.offense_faction, FORTRESS_LIST_OFFENSE_INDEX);
		if (!bName1OK)
		{
			//	记录数据用于 Tick 中更新帮派名称
			m_pLst_GvG->SetItemData(m_pLst_GvG->GetCount()-1, 1, FORTRESS_LIST_NAME1_INDEX);
			m_bAllInfoReady = false;
		}
		if (!bName2OK)
		{
			//	记录数据用于 Tick 中更新帮派名称
			m_pLst_GvG->SetItemData(m_pLst_GvG->GetCount()-1, 1, FORTRESS_LIST_NAME2_INDEX);
			m_bAllInfoReady = false;
		}
	}

	if (m_pLst_GvG->GetCount())
	{
		//	默认选取第一项
		m_pLst_GvG->SetCurSel(0);
		OnListSelChange();
	}
}

void CDlgFortressWarList::OnTick()
{
	int count = m_pLst_GvG->GetCount();
	if (count <= 0)
		return;

	if (m_bAllInfoReady)
	{
		//	所有信息都已完整，不需要再检查
		return;
	}

	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	ACString strText, strName1, strName2;

	m_bAllInfoReady = true;
	for (int i = 0; i < count; ++ i)
	{
		bool bName1Empty = m_pLst_GvG->GetItemData(i, FORTRESS_LIST_NAME1_INDEX) != 0;
		bool bName2Empty = m_pLst_GvG->GetItemData(i, FORTRESS_LIST_NAME2_INDEX) != 0;

		if (!bName1Empty && !bName2Empty)
		{
			//	帮派信息已经完整
			continue;
		}

		int idFaction1 = m_pLst_GvG->GetItemData(i, FORTRESS_LIST_OWNER_INDEX);
		if (!GetFactionName(idFaction1, strName1, false))
		{
			//	服务器尚未返回帮派信息
			m_bAllInfoReady = false;
		}
		else
		{
			//	已经返回信息
			if (bName1Empty)
				m_pLst_GvG->SetItemData(i, 0, FORTRESS_LIST_NAME1_INDEX);
		}

		int idFaction2 = m_pLst_GvG->GetItemData(i, FORTRESS_LIST_OFFENSE_INDEX);
		if (!GetFactionName(idFaction2, strName2, false))
		{
			//	服务器尚未返回帮派信息
			m_bAllInfoReady = false;
		}
		else
		{
			//	已经返回信息
			if (bName2Empty)
				m_pLst_GvG->SetItemData(i, 0, FORTRESS_LIST_NAME2_INDEX);
		}

		//	更新显示
		CSplit s(m_pLst_GvG->GetText(i));
		CSplit::VectorAWString vec = s.Split(_AL("\t"));
		strText.Format(_AL("%s\t%s"), strName1, strName2);
		m_pLst_GvG->SetText(i, strText);
	}
}


bool CDlgFortressWarList::GetFactionName(unsigned int uID, ACString &strName, bool bRequestFromServer)
{
	//	根据 id 查询指定帮派名称
	//	无法在本地查询时返回 false
	//
	bool bRet(false);

	while (true)
	{
		if (!uID)
		{
			//	传入0时，查询得“无”，以同没查询到时“-”区别
			strName = GetStringFromTable(9152);
			if (strName.IsEmpty())
				strName = _AL("none");
			bRet = true;
			break;
		}
		CECFactionMan *pFMan = GetGame()->GetFactionMan();
		const Faction_Info *pInfo = pFMan->GetFaction(uID, bRequestFromServer);
		if (pInfo)
		{
			strName = pInfo->GetName();
			bRet = true;
		}
		
		//	防空处理
		if (strName.IsEmpty())
			strName = _AL("-");
		break;
	}

	return bRet;
}

void CDlgFortressWarList::OnListSelChange()
{
	int idTarget = GetFortressToEnter();
	m_pBtn_Enter->Enable(idTarget > 0);
}
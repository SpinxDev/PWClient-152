#include "DlgFortressWar.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_Faction.h"
#include "EC_Game.h"
#include "EC_HostPlayer.h"
#include "CSplit.h"
#include "ExpTypes.h"
#include "NetWork\\factionfortresslist_re.hpp"
#include "Network\\factionfortresschallenge_re.hpp"
#include "Network\\rpcdata\\gfactionfortressbriefinfo"
#include "Network\\factionfortressget_re.hpp"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressWar, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Prev",	OnCommand_Prev)
AUI_ON_COMMAND("Btn_Next",	OnCommand_Next)
AUI_ON_COMMAND("Btn_Refresh",	OnCommand_Refresh)
AUI_ON_COMMAND("Btn_Search",	OnCommand_Search)
AUI_ON_COMMAND("Btn_DeclareWar",OnCommand_DeclareWar)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressWar, CDlgBase)
AUI_ON_EVENT("Lst_Fortress", WM_LBUTTONDOWN, OnEventLButtonDown_Lst_Fortress)
AUI_END_EVENT_MAP()

CDlgFortressWar::CDlgFortressWar()
{
	m_pEdit_Name = NULL;
	m_pLst_Fortress = NULL;
	m_pBtn_DeclareWar = NULL;
	m_nBegin = 0;
	m_status = ST_OPEN;
	m_bAllInfoReady = true;
}

bool CDlgFortressWar::OnInitDialog()
{
	DDX_Control("Edit_Name", m_pEdit_Name);
	DDX_Control("Lst_Fortress", m_pLst_Fortress);
	DDX_Control("Btn_DeclareWar", m_pBtn_DeclareWar);
	return true;
}

void CDlgFortressWar::OnShowDialog()
{
	m_nBegin = 0;
	Refresh();
}

void CDlgFortressWar::OnCommand_CANCEL(const char *szCommand)
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

void CDlgFortressWar::Refresh()
{
	//	根据当前指定的初始位置，更新基地列表

	if (m_nBegin < 0)
		return;

	//	发送请求协议
	GetGameSession()->factionFortress_List(m_nBegin);

	//	清除界面等待返回
	m_pLst_Fortress->ResetContent();
	m_bAllInfoReady = true;
	m_status = ST_OPEN;
	OnListSelChange();
}

void CDlgFortressWar::OnCommand_Prev(const char *szCommand)
{
	if (m_nBegin > 0)
	{
		m_nBegin -= FORTRESS_LIST_SIZE;
		a_ClampFloor(m_nBegin, 0);
	}
	//	else 已经是第一页（或显示的是搜索到的单独的帮派基地），重新更新
	Refresh();
}

void CDlgFortressWar::OnCommand_Next(const char *szCommand)
{
	int count = m_pLst_Fortress->GetCount();
	if (count == 0)
	{
		//	列表中当前页尚未显示，自然不能更新下一页
		return;
	}
	if (count == FORTRESS_LIST_SIZE)
	{
		//	可能有后续页
		m_nBegin += FORTRESS_LIST_SIZE;
	}
	//	else	已经是最后一页（或显示的是搜索到的单独的帮派基地），重新更新
	Refresh();
}

void CDlgFortressWar::OnCommand_DeclareWar(const char *szCommand)
{
	int idTarget = GetDeclareWarTarget();
	if (idTarget > 0)
	{
		//	检查金钱数目
		int nMoneyNeeded = 5000000;
		CECGameUIMan *pGameUIMan = GetGameUIMan();

		if (nMoneyNeeded > GetHostPlayer()->GetMoneyAmount())
		{
			ACString strMsg;
			strMsg.Format(GetStringFromTable(9153), nMoneyNeeded);
			pGameUIMan->ShowErrorMsg(strMsg);
			return;
		}

		ACString strMsg;
		strMsg.Format(GetStringFromTable(9154), nMoneyNeeded);
		PAUIDIALOG pMsgBox(NULL);
		pGameUIMan->MessageBox("Fortress_DeclareWar", strMsg, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(idTarget);
	}
}

void CDlgFortressWar::OnCommand_Refresh(const char *szCommand)
{
	Refresh();
}

void CDlgFortressWar::OnEventLButtonDown_Lst_Fortress(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnListSelChange();
}

void CDlgFortressWar::OnListSelChange()
{
	int idTarget = GetDeclareWarTarget();
	m_pBtn_DeclareWar->Enable(idTarget > 0);
}

void CDlgFortressWar::OnPrtcFactionFortressList_Re(const GNET::FactionFortressList_Re *p)
{
	m_nBegin = (int)p->begin;
	m_status = (FORTRESS_STATE)p->status;

	m_pLst_Fortress->ResetContent();

	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	ACString strText, strName1, strName2, strBuilding;

	m_bAllInfoReady = true;
	for (size_t i = 0; i < p->list.size(); ++ i)
	{
		const GNET::GFactionFortressBriefInfo &info = p->list[i];
		bool bName1OK = GetFactionName(info.factionid, strName1, true);
		bool bName2OK = GetFactionName(info.offense_faction, strName2, true);
		strBuilding = GetBuildingString(info.building.begin(), info.building.size());
		
		strText.Format(_AL("%s\t%d\t%d\t%s\t%s")
			, strName1
			, info.level
			, info.health
			, strBuilding
			, strName2);
		m_pLst_Fortress->AddString(strText);

		//	记录数据用于宣战
		m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, info.factionid, FORTRESS_LIST_OWNER_INDEX);
		m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, info.offense_faction, FORTRESS_LIST_OFFENSE_INDEX);
		if (!bName1OK)
		{
			//	记录数据用于 Tick 中更新帮派名称
			m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, 1, FORTRESS_LIST_NAME1_INDEX);
			m_bAllInfoReady = false;
		}
		if (!bName2OK)
		{
			//	记录数据用于 Tick 中更新帮派名称
			m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, 1, FORTRESS_LIST_NAME2_INDEX);
			m_bAllInfoReady = false;
		}
	}

	if (m_pLst_Fortress->GetCount())
	{
		//	默认选取第一项
		m_pLst_Fortress->SetCurSel(0);
		OnListSelChange();
	}
}

void CDlgFortressWar::OnPrtcFactionFortressGet_Re(const GNET::FactionFortressGet_Re *p)
{
	//	此处保持 m_nBegin 和 m_status 不变

	if (p->retcode != ERR_SUCCESS)
	{
		GetGameSession()->OutputLinkSevError(p->retcode);
		return;
	}

	m_pLst_Fortress->ResetContent();

	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	ACString strText, strName1, strName2, strBuilding;

	m_bAllInfoReady = true;
	while (true)
	{
		const GNET::GFactionFortressBriefInfo &info = p->brief;
		bool bName1OK = GetFactionName(info.factionid, strName1, true);
		bool bName2OK = GetFactionName(info.offense_faction, strName2, true);
		strBuilding = GetBuildingString(info.building.begin(), info.building.size());
		
		strText.Format(_AL("%s\t%d\t%d\t%s\t%s")
			, strName1
			, info.level
			, info.health
			, strBuilding
			, strName2);
		m_pLst_Fortress->AddString(strText);

		//	记录数据用于宣战
		m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, info.factionid, FORTRESS_LIST_OWNER_INDEX);
		m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, info.offense_faction, FORTRESS_LIST_OFFENSE_INDEX);
		if (!bName1OK)
		{
			//	记录数据用于 Tick 中更新帮派名称
			m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, 1, FORTRESS_LIST_NAME1_INDEX);
			m_bAllInfoReady = false;
		}
		if (!bName2OK)
		{
			//	记录数据用于 Tick 中更新帮派名称
			m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, 1, FORTRESS_LIST_NAME2_INDEX);
			m_bAllInfoReady = false;
		}
		break;
	}

	if (m_pLst_Fortress->GetCount())
	{
		//	默认选取第一项
		m_pLst_Fortress->SetCurSel(0);
		OnListSelChange();
	}
}

void CDlgFortressWar::OnPrtcFactionFortressChallenge_Re(const GNET::FactionFortressChallenge_Re *p)
{
	if (p->retcode != ERR_SUCCESS)
	{
		GetGameSession()->OutputLinkSevError(p->retcode);
		return;
	}

	//	更新界面
	GetGameUIMan()->MessageBox("", GetStringFromTable(9151), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgFortressWar::OnTick()
{
	int count = m_pLst_Fortress->GetCount();
	if (count <= 0)
		return;

	if (m_bAllInfoReady)
	{
		//	所有信息都已完整，不需要再检查
		return;
	}

	int idFaction = 0;
	const Faction_Info *pFInfo = NULL;	
	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	ACString strText, strName1, strName2;

	m_bAllInfoReady = true;
	for (int i = 0; i < count; ++ i)
	{
		bool bName1Empty = m_pLst_Fortress->GetItemData(i, FORTRESS_LIST_NAME1_INDEX) != 0;
		bool bName2Empty = m_pLst_Fortress->GetItemData(i, FORTRESS_LIST_NAME2_INDEX) != 0;

		if (!bName1Empty && !bName2Empty)
		{
			//	帮派信息已经完整
			continue;
		}

		int idFaction1 = m_pLst_Fortress->GetItemData(i, FORTRESS_LIST_OWNER_INDEX);
		if (!GetFactionName(idFaction1, strName1, false))
		{
			//	服务器尚未返回帮派信息
			m_bAllInfoReady = false;
		}
		else
		{
			//	已经返回信息
			if (bName1Empty)
				m_pLst_Fortress->SetItemData(i, 0, FORTRESS_LIST_NAME1_INDEX);
		}

		int idFaction2 = m_pLst_Fortress->GetItemData(i, FORTRESS_LIST_OFFENSE_INDEX);
		if (!GetFactionName(idFaction2, strName2, false))
		{
			//	服务器尚未返回帮派信息
			m_bAllInfoReady = false;
		}
		else
		{
			//	已经返回信息
			if (bName2Empty)
				m_pLst_Fortress->SetItemData(i, 0, FORTRESS_LIST_NAME2_INDEX);
		}

		CSplit s(m_pLst_Fortress->GetText(i));
		CSplit::VectorAWString vec = s.Split(_AL("\t"));
		strText.Format(_AL("%s\t%s\t%s\t%s\t%s"), strName1, vec[1], vec[2], vec[3], strName2);
		m_pLst_Fortress->SetText(i, strText);
	}
}

int	CDlgFortressWar::GetDeclareWarTarget()
{
	int idTarget(0);

	while (true)
	{
		int count = m_pLst_Fortress->GetCount();
		if (count <= 0)
			break;
		
		int nSel = m_pLst_Fortress->GetCurSel();
		if (nSel < 0 || nSel >=count)
			break;
		
		int idMyFaction = GetHostPlayer()->GetFactionID();
		if (idMyFaction <= 0)
			break;
		
		int idFaction = m_pLst_Fortress->GetItemData(nSel, FORTRESS_LIST_OWNER_INDEX);
		if (idFaction == idMyFaction)
		{
			//	不能对自己的基地宣战
			break;
		}

		if (m_status != ST_CHALLENGE)
		{
			//	当前不是可宣战状态
			break;
		}

		idTarget = idFaction;
		break;
	}

	return idTarget;
}

void CDlgFortressWar::OnCommand_Search(const char *szCommand)
{
	//	根据帮派名称查询 id 填充到列表中并向其宣战
	//

	//	获取输入的帮派名称
	ACString strName = m_pEdit_Name->GetText();
	if (strName.IsEmpty())
	{
		//	未输入名称
		ChangeFocus(m_pEdit_Name);
		return;
	}

	//	在本地缓存中查询相应帮派 id
	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	const Faction_Info *pInfo = pFMan->FindFactionByName(strName);
	if (!pInfo)
	{
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9115));
		return;
	}
	
	//	发送协议请求此帮派信息
	GetGameSession()->factionFortress_Get(pInfo->GetID());

	//	清除界面等待返回
	m_pLst_Fortress->ResetContent();
	m_bAllInfoReady = true;
	OnListSelChange();
}

bool CDlgFortressWar::GetFactionName(unsigned int uID, ACString &strName, bool bRequestFromServer)
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

ACString CDlgFortressWar::GetBuildingString(const void *pBuf, size_t sz)
{
	//	从缓冲中获取此基地代表性设施及其等级
	ACString strText = GetStringFromTable(9152);

	while (true)
	{
		if (!pBuf)
			break;

#pragma pack(1)
		struct _data 
		{
			int id;
			int finish_time;
		};
#pragma pack()

		if (sz % sizeof(_data))
		{
			//	格式错误
			break;
		}

		int nBuilding = sz/sizeof(_data);
		const _data *pData = (const _data *)pBuf;
		CECHostPlayer *pHost = GetHostPlayer();

		int maxLevel = -1;
		const FACTION_BUILDING_ESSENCE *pEssence = NULL;
		for (int i = 0; i < nBuilding; ++ i)
		{
			pEssence = pHost->GetBuildingEssence(pData[i].id);
			if (!pEssence)
			{
				//	错误的设施id
				ASSERT(false);
				continue;
			}
			if (maxLevel < pEssence->level)
			{
				//	记录设施名称及等级
				maxLevel = pEssence->level;
				strText = pEssence->name;
				if (strText.IsEmpty())
					strText = _AL("error");
			}
		}
		break;
	}

	return strText;
}
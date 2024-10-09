#include "DlgGuildDiplomacyApply.h"
#include "DlgGuildDiplomacyMan.h"
#include "DlgGuildMan.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Faction.h"
#include "AWStringWithWildcard.h"
#include "Network\\factionlistonline_re.hpp"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGuildDiplomacyApply, CDlgBase)
AUI_ON_COMMAND("Btn_Alliance", OnCommand_Btn_Alliance)
AUI_ON_COMMAND("Btn_Hostility", OnCommand_Btn_Hostility)
AUI_ON_COMMAND("Btn_Search", OnCommand_Btn_Search)
AUI_ON_COMMAND("Edt_GuildName", OnCommand_Btn_Search)
AUI_ON_COMMAND("Btn_Num", OnCommand_Sort_By_NUM)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGuildDiplomacyApply, CDlgBase)
AUI_END_EVENT_MAP()

CDlgGuildDiplomacyApply::CDlgGuildDiplomacyApply()
{
	m_pList_Guild = NULL;
	m_pBtn_Search = NULL;
	m_pEdt_GuildName = NULL;
	m_bFactionInfoReady = false;
	m_queryCounter.SetPeriod(1000);
	m_queryCounter.Reset(true);
}

bool CDlgGuildDiplomacyApply::OnInitDialog()
{
	DDX_Control("List_Guild", m_pList_Guild);
	DDX_Control("Btn_Search", m_pBtn_Search);
	DDX_Control("Edt_GuildName", m_pEdt_GuildName);
	return true;
}

void CDlgGuildDiplomacyApply::OnShowDialog()
{
	Refresh();
}

void CDlgGuildDiplomacyApply::Refresh()
{
	//	 清除帮派数据
	m_factionArray.clear();
	GetGameSession()->faction_get_onlinelist();
	m_bFactionInfoReady = false;
	m_queryCounter.Reset(true);

	//	更新界面
	UpdateUI();
}

void CDlgGuildDiplomacyApply::OnCommand_Btn_Alliance(const char *szCommand)
{
	int idFaction = GetFactionSel();
	if (idFaction > 0 && idFaction != GetHostPlayer()->GetFactionID())
	{
		//	申请同盟需要花钱
		int nMoneyNeeded = 3000000;
		ACString strMsg;
		if (GetHostPlayer()->GetMoneyAmount() < nMoneyNeeded)
		{
			//	条件不满足
			strMsg.Format(GetStringFromTable(9170), nMoneyNeeded);
			GetGameUIMan()->ShowErrorMsg(strMsg);

			OnCommand_CANCEL(NULL);
		}
		else
		{
			if (GetGameUIMan()->m_pDlgGuildDiplomacyMan->CanSend())
			{
				strMsg.Format(GetStringFromTable(9171), nMoneyNeeded);
				PAUIDIALOG pMsgBox = NULL;
				GetGameUIMan()->MessageBox("GDiplomacy_Alliance_Apply", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
				pMsgBox->SetData(idFaction);

				OnCommand_CANCEL(NULL);
			}
		}
	}
}

void CDlgGuildDiplomacyApply::OnCommand_Btn_Hostility(const char *szCommand)
{
	int idFaction = GetFactionSel();
	if (idFaction > 0 && idFaction != GetHostPlayer()->GetFactionID())
	{
		//	申请敌对不需要花钱
		if (GetGameUIMan()->m_pDlgGuildDiplomacyMan->CanSend())
		{
			ACString strMsg = GetStringFromTable(9172);
			PAUIDIALOG pMsgBox = NULL;
			GetGameUIMan()->MessageBox("GDiplomacy_Hostility_Apply", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(idFaction);
			
			OnCommand_CANCEL(NULL);
		}
	}
}

void CDlgGuildDiplomacyApply::OnCommand_Btn_Search(const char *szCommand)
{
	if (!m_bFactionInfoReady)	return;

	//	获取输入
	ACString strSearch = m_pEdt_GuildName->GetText();
	strSearch.TrimLeft();		//	除去左侧空格
	strSearch.TrimRight();	//	除去右侧空格

	//	逐个匹配
	m_pList_Guild->ResetContent();

	ACString strPattern = _AL("*") + strSearch + _AL("*");
	ACStringWithWildcard matcher;

	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	ACString strName, strLine;
	for (size_t i = 0; i < m_factionArray.size(); ++ i)
	{
		int idFaction = m_factionArray[i];

		const Faction_Info *pFInfo = pFMan->GetFaction(idFaction, false);
		if (!pFInfo)	continue;

		strName = pFInfo->GetName();
		if (strName.IsEmpty())	continue;
		if (!matcher.IsMatch(strName, strPattern))
			continue;

		if (GetLineString(idFaction, strLine))
		{
			m_pList_Guild->AddString(strLine);
			m_pList_Guild->SetItemData(m_pList_Guild->GetCount()-1, idFaction, LIST_FACTION_INDEX);
		}
	}

	//	显示当前显示的帮派个数及帮派总数
	ACString strNum;
	strNum.Format(_AL("%d/%d"), m_pList_Guild->GetCount(), m_factionArray.size());
	GetDlgItem("Btn_Num")->SetHint(strNum);
}


void CDlgGuildDiplomacyApply::OnCommand_Sort_By_NUM(const char *szCommand)
{
	if (!m_bFactionInfoReady)	return;
	m_pList_Guild->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_INT, 1);
}

int CDlgGuildDiplomacyApply::GetFactionSel()
{
	//	获取当前选中的帮派 id
	int idFaction(0);

	int nSel = m_pList_Guild->GetCurSel();
	if (nSel >= 0 && nSel < m_pList_Guild->GetCount())
	{
		idFaction = m_pList_Guild->GetItemData(nSel, LIST_FACTION_INDEX);
	}

	return idFaction;
}

void CDlgGuildDiplomacyApply::UpdateUI()
{
	//	更新当前页在页面中的内容

	//	清除原内容
	m_pList_Guild->ResetContent();

	//	添加新内容
	if (!m_factionArray.empty())
	{
		ACString strText;
		for (size_t i = 0; i < m_factionArray.size(); ++ i)
		{
			int idFaction = m_factionArray[i];
			if (GetLineString(idFaction, strText))
			{
				m_pList_Guild->AddString(strText);
			}
			else
			{
				//	记录暂时未查询到的帮派信息，在OnTick中查询更新
				m_pList_Guild->AddString(strText);
				m_pList_Guild->SetItemData(m_pList_Guild->GetCount()-1, QUERY_HINT_WAITING, LIST_QUERYHINT_INDEX);		//	需向服务器查询
			}
			m_pList_Guild->SetItemData(m_pList_Guild->GetCount()-1, idFaction, LIST_FACTION_INDEX);
		}
	}

	//	显示当前显示的帮派个数及帮派总数
	ACString strNum;
	strNum.Format(_AL("%d/%d"), m_pList_Guild->GetCount(), m_factionArray.size());
	GetDlgItem("Btn_Num")->SetHint(strNum);

	//	重置搜索界面
	m_pEdt_GuildName->SetText(_AL(""));
	m_pEdt_GuildName->Enable(false);
	m_pBtn_Search->Enable(false);
}

bool CDlgGuildDiplomacyApply::GetLineString(int idFaction, ACString &strText)
{
	//	根据帮派 ID 获取每行显示的内容
	//	返回 true 表示成功获取帮派信息
	//

	bool bRet(false);

	CECFactionMan *pFMan = GetGame()->GetFactionMan();	
	const Faction_Info *pFInfo = pFMan->GetFaction(idFaction, false);
	if (pFInfo)
	{
		ACString strName = pFInfo->GetName();
		if (strName.IsEmpty())
			strName = _AL("-");
		strText.Format(_AL("%s\t%d"), strName, pFInfo->GetMemNum());
		bRet = true;
	}
	else
	{
		strText = _AL("-\t-");
	}
	return bRet;
}

void CDlgGuildDiplomacyApply::OnTick()
{
	//	检查是否有帮派信息需要更新显示
	//
	if (m_pList_Guild->GetCount() == 0) return;	//	列表为空时返回，服务器还未发送列表、或列表本身即为空
	if (m_bFactionInfoReady) return;					//	信息完整时返回，无需做多余计算

	bool bCanQueryNow = m_queryCounter.IncCounter(GetGame()->GetRealTickTime());

	const int MAX_QUERY_ONCE = 50;				//	每次最多向服务器查询的帮派名称个数
	AArray<int, int> factionIDs;
	bool bReady(true);

	ACString strText;
	for (int i = 0; i < m_pList_Guild->GetCount(); ++ i)
	{
		DWORD dwData = m_pList_Guild->GetItemData(i, LIST_QUERYHINT_INDEX);
		if (dwData == QUERY_HINT_OK)
			continue;

		//	待向服务器查询、已向服务器查询两种状态下都可尝试获取帮派信息
		int idFaction = m_pList_Guild->GetItemData(i, LIST_FACTION_INDEX);
		if (GetLineString(idFaction, strText))
		{
			//	更新显示并清除相应标记
			m_pList_Guild->SetText(i, strText);
			m_pList_Guild->SetItemData(i, QUERY_HINT_OK, LIST_QUERYHINT_INDEX);
			continue;
		}

		bReady = false;	//	至少有一个帮派信息还未获取

		if (dwData == QUERY_HINT_SENT)
			continue;		//	已向服务器查询

		if (!bCanQueryNow)
			continue;		//	查询冷却中

		//	添加查询
		if (factionIDs.GetSize() < MAX_QUERY_ONCE)
		{
			m_pList_Guild->SetItemData(i, QUERY_HINT_SENT, LIST_QUERYHINT_INDEX);
			factionIDs.Add(idFaction);
		}
	}

	m_bFactionInfoReady = bReady;

	if (m_bFactionInfoReady)
	{
		//	搜索功能可以开放了
		m_pEdt_GuildName->Enable(true);
		m_pBtn_Search->Enable(true);
	}

	if (factionIDs.GetSize() > 0)
	{
		GetGameSession()->GetFactionInfo(factionIDs.GetSize(), factionIDs.GetData());
		m_queryCounter.Reset();
	}
}

void CDlgGuildDiplomacyApply::OnPrtcFactionListOnline_Re(GNET::FactionListOnline_Re *pProtocol)
{
	if (pProtocol)
	{
		//	清除并拷贝新帮派列表
		m_factionArray.clear();

		m_factionArray.reserve(pProtocol->fid_list.size());
		for (size_t i = 0; i < pProtocol->fid_list.size(); ++ i)
			m_factionArray.push_back(pProtocol->fid_list[i]);

		//	避免收到当前未预期的此协议
		m_bFactionInfoReady = false;
		m_queryCounter.Reset(true);

		//	更新界面
		UpdateUI();
	}
}

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
	//	 �����������
	m_factionArray.clear();
	GetGameSession()->faction_get_onlinelist();
	m_bFactionInfoReady = false;
	m_queryCounter.Reset(true);

	//	���½���
	UpdateUI();
}

void CDlgGuildDiplomacyApply::OnCommand_Btn_Alliance(const char *szCommand)
{
	int idFaction = GetFactionSel();
	if (idFaction > 0 && idFaction != GetHostPlayer()->GetFactionID())
	{
		//	����ͬ����Ҫ��Ǯ
		int nMoneyNeeded = 3000000;
		ACString strMsg;
		if (GetHostPlayer()->GetMoneyAmount() < nMoneyNeeded)
		{
			//	����������
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
		//	����жԲ���Ҫ��Ǯ
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

	//	��ȡ����
	ACString strSearch = m_pEdt_GuildName->GetText();
	strSearch.TrimLeft();		//	��ȥ���ո�
	strSearch.TrimRight();	//	��ȥ�Ҳ�ո�

	//	���ƥ��
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

	//	��ʾ��ǰ��ʾ�İ��ɸ�������������
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
	//	��ȡ��ǰѡ�еİ��� id
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
	//	���µ�ǰҳ��ҳ���е�����

	//	���ԭ����
	m_pList_Guild->ResetContent();

	//	���������
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
				//	��¼��ʱδ��ѯ���İ�����Ϣ����OnTick�в�ѯ����
				m_pList_Guild->AddString(strText);
				m_pList_Guild->SetItemData(m_pList_Guild->GetCount()-1, QUERY_HINT_WAITING, LIST_QUERYHINT_INDEX);		//	�����������ѯ
			}
			m_pList_Guild->SetItemData(m_pList_Guild->GetCount()-1, idFaction, LIST_FACTION_INDEX);
		}
	}

	//	��ʾ��ǰ��ʾ�İ��ɸ�������������
	ACString strNum;
	strNum.Format(_AL("%d/%d"), m_pList_Guild->GetCount(), m_factionArray.size());
	GetDlgItem("Btn_Num")->SetHint(strNum);

	//	������������
	m_pEdt_GuildName->SetText(_AL(""));
	m_pEdt_GuildName->Enable(false);
	m_pBtn_Search->Enable(false);
}

bool CDlgGuildDiplomacyApply::GetLineString(int idFaction, ACString &strText)
{
	//	���ݰ��� ID ��ȡÿ����ʾ������
	//	���� true ��ʾ�ɹ���ȡ������Ϣ
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
	//	����Ƿ��а�����Ϣ��Ҫ������ʾ
	//
	if (m_pList_Guild->GetCount() == 0) return;	//	�б�Ϊ��ʱ���أ���������δ�����б����б���Ϊ��
	if (m_bFactionInfoReady) return;					//	��Ϣ����ʱ���أ��������������

	bool bCanQueryNow = m_queryCounter.IncCounter(GetGame()->GetRealTickTime());

	const int MAX_QUERY_ONCE = 50;				//	ÿ��������������ѯ�İ������Ƹ���
	AArray<int, int> factionIDs;
	bool bReady(true);

	ACString strText;
	for (int i = 0; i < m_pList_Guild->GetCount(); ++ i)
	{
		DWORD dwData = m_pList_Guild->GetItemData(i, LIST_QUERYHINT_INDEX);
		if (dwData == QUERY_HINT_OK)
			continue;

		//	�����������ѯ�������������ѯ����״̬�¶��ɳ��Ի�ȡ������Ϣ
		int idFaction = m_pList_Guild->GetItemData(i, LIST_FACTION_INDEX);
		if (GetLineString(idFaction, strText))
		{
			//	������ʾ�������Ӧ���
			m_pList_Guild->SetText(i, strText);
			m_pList_Guild->SetItemData(i, QUERY_HINT_OK, LIST_QUERYHINT_INDEX);
			continue;
		}

		bReady = false;	//	������һ��������Ϣ��δ��ȡ

		if (dwData == QUERY_HINT_SENT)
			continue;		//	�����������ѯ

		if (!bCanQueryNow)
			continue;		//	��ѯ��ȴ��

		//	��Ӳ�ѯ
		if (factionIDs.GetSize() < MAX_QUERY_ONCE)
		{
			m_pList_Guild->SetItemData(i, QUERY_HINT_SENT, LIST_QUERYHINT_INDEX);
			factionIDs.Add(idFaction);
		}
	}

	m_bFactionInfoReady = bReady;

	if (m_bFactionInfoReady)
	{
		//	�������ܿ��Կ�����
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
		//	����������°����б�
		m_factionArray.clear();

		m_factionArray.reserve(pProtocol->fid_list.size());
		for (size_t i = 0; i < pProtocol->fid_list.size(); ++ i)
			m_factionArray.push_back(pProtocol->fid_list[i]);

		//	�����յ���ǰδԤ�ڵĴ�Э��
		m_bFactionInfoReady = false;
		m_queryCounter.Reset(true);

		//	���½���
		UpdateUI();
	}
}

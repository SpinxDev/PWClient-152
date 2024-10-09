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
	
	//	�ر���ضԻ���
	PAUIDIALOG pDlgInfo = GetGameUIMan()->GetDialog("Win_FortressInfo");
	if (pDlgInfo && pDlgInfo->IsShow())
		pDlgInfo->OnCommand("IDCANCEL");
	
	//	�ر� NPC ����
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
				//	״̬����ȷ�����ܽ���
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

		//	��¼�������ڽ���ս��
		m_pLst_GvG->SetItemData(m_pLst_GvG->GetCount()-1, info.factionid, FORTRESS_LIST_OWNER_INDEX);
		m_pLst_GvG->SetItemData(m_pLst_GvG->GetCount()-1, info.offense_faction, FORTRESS_LIST_OFFENSE_INDEX);
		if (!bName1OK)
		{
			//	��¼�������� Tick �и��°�������
			m_pLst_GvG->SetItemData(m_pLst_GvG->GetCount()-1, 1, FORTRESS_LIST_NAME1_INDEX);
			m_bAllInfoReady = false;
		}
		if (!bName2OK)
		{
			//	��¼�������� Tick �и��°�������
			m_pLst_GvG->SetItemData(m_pLst_GvG->GetCount()-1, 1, FORTRESS_LIST_NAME2_INDEX);
			m_bAllInfoReady = false;
		}
	}

	if (m_pLst_GvG->GetCount())
	{
		//	Ĭ��ѡȡ��һ��
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
		//	������Ϣ��������������Ҫ�ټ��
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
			//	������Ϣ�Ѿ�����
			continue;
		}

		int idFaction1 = m_pLst_GvG->GetItemData(i, FORTRESS_LIST_OWNER_INDEX);
		if (!GetFactionName(idFaction1, strName1, false))
		{
			//	��������δ���ذ�����Ϣ
			m_bAllInfoReady = false;
		}
		else
		{
			//	�Ѿ�������Ϣ
			if (bName1Empty)
				m_pLst_GvG->SetItemData(i, 0, FORTRESS_LIST_NAME1_INDEX);
		}

		int idFaction2 = m_pLst_GvG->GetItemData(i, FORTRESS_LIST_OFFENSE_INDEX);
		if (!GetFactionName(idFaction2, strName2, false))
		{
			//	��������δ���ذ�����Ϣ
			m_bAllInfoReady = false;
		}
		else
		{
			//	�Ѿ�������Ϣ
			if (bName2Empty)
				m_pLst_GvG->SetItemData(i, 0, FORTRESS_LIST_NAME2_INDEX);
		}

		//	������ʾ
		CSplit s(m_pLst_GvG->GetText(i));
		CSplit::VectorAWString vec = s.Split(_AL("\t"));
		strText.Format(_AL("%s\t%s"), strName1, strName2);
		m_pLst_GvG->SetText(i, strText);
	}
}


bool CDlgFortressWarList::GetFactionName(unsigned int uID, ACString &strName, bool bRequestFromServer)
{
	//	���� id ��ѯָ����������
	//	�޷��ڱ��ز�ѯʱ���� false
	//
	bool bRet(false);

	while (true)
	{
		if (!uID)
		{
			//	����0ʱ����ѯ�á��ޡ�����ͬû��ѯ��ʱ��-������
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
		
		//	���մ���
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
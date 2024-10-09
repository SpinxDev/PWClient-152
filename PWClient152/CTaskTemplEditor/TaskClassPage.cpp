// TaskClassPage.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskClassPage.h"
#include "TaskTempl.h"
#include "AVariant.h"
#include "MonWantedDlg.h"
#include "ItemsWantedDlg.h"
#include "AwardDlg.h"
#include "TemplIDSelDlg.h"
#include "BaseDataTemplate.h"
#include "VSSOperation.h"
#include "shlwapi.h"
#include "TaskTemplMan.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern unsigned long period_map[];
extern int find_period_index(unsigned long ulPeriod);

/////////////////////////////////////////////////////////////////////////////
// CTaskClassPage property page

IMPLEMENT_DYNCREATE(CTaskClassPage, CPropertyPage)

CTaskClassPage::CTaskClassPage() : CPropertyPage(CTaskClassPage::IDD)
{
	//{{AFX_DATA_INIT(CTaskClassPage)
	m_strName = _T("");
	m_strID = _T("");
	m_dwSuitLev = 0;
	m_dwTimeLimit = 0;
	m_strDelvNoText = _T("");
	m_strDelvNPCText = _T("");
	m_strDelvYesText = _T("");
	m_strDescript = _T("");
	m_strFinishNPCText = _T("");
	m_strFinishPlayerText = _T("");
	m_dwLevMax = 0;
	m_dwLevMin = 0;
	m_strUnfinishedNPCText = _T("");
	m_strUnfinishedPlayerText = _T("");
	m_strDelvNPC = _T("");
	m_strAwardNPC = _T("");
	m_strPreTask = _T("");
	m_nType = 0;
	m_bTeamwork = FALSE;
	m_dwSpecialAward = 0;
	m_bCanSeekOut = FALSE;
	m_nPeriod = 0;
	m_bAutoDeliver = FALSE;
	m_bShowDirection = FALSE;
	//}}AFX_DATA_INIT

	m_pTask = NULL;
}

CTaskClassPage::~CTaskClassPage()
{
}

void CTaskClassPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskClassPage)
	DDX_Text(pDX, IDC_TASK_NAME, m_strName);
	DDX_Text(pDX, IDC_TASK_ID, m_strID);
	DDX_Text(pDX, IDC_SUIT_LEVEL, m_dwSuitLev);
	DDX_Text(pDX, IDC_TIME_LIMIT, m_dwTimeLimit);
	DDX_Text(pDX, IDC_DELV_NO_TEXT, m_strDelvNoText);
	DDX_Text(pDX, IDC_DELV_NPC_TEXT, m_strDelvNPCText);
	DDX_Text(pDX, IDC_DELV_YES_TEXT, m_strDelvYesText);
	DDX_Text(pDX, IDC_DESCRIPT, m_strDescript);
	DDX_Text(pDX, IDC_FINISH_NPC_TEXT, m_strFinishNPCText);
	DDX_Text(pDX, IDC_FINISH_PLAYER_TEXT, m_strFinishPlayerText);
	DDX_Text(pDX, IDC_LEVEL_MAX, m_dwLevMax);
	DDX_Text(pDX, IDC_LEVEL_MIN, m_dwLevMin);
	DDX_Text(pDX, IDC_UNFINISHED_NPC_TEXT, m_strUnfinishedNPCText);
	DDX_Text(pDX, IDC_UNFINISHED_PLAYER_TEXT, m_strUnfinishedPlayerText);
	DDX_Text(pDX, IDC_DELV_NPC, m_strDelvNPC);
	DDX_Text(pDX, IDC_AWARD_NPC, m_strAwardNPC);
	DDX_Text(pDX, IDC_PRE_TASK, m_strPreTask);
	DDX_CBIndex(pDX, IDC_TYPE, m_nType);
	DDX_Check(pDX, IDC_TEAM, m_bTeamwork);
	DDX_Text(pDX, IDC_SPECIAL_AWARD, m_dwSpecialAward);
	DDX_Check(pDX, IDC_SEEK_OUT, m_bCanSeekOut);
	DDX_CBIndex(pDX, IDC_PERIOD, m_nPeriod);
	DDX_Check(pDX, IDC_AUTO_DELIVER, m_bAutoDeliver);
	DDX_Check(pDX, IDC_SHOW_DIRECTION, m_bShowDirection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskClassPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTaskClassPage)
	ON_BN_CLICKED(IDC_SEL_MONSTER, OnSelMonster)
	ON_BN_CLICKED(IDC_EDIT_ITEMS_WANTED, OnEditItemsWanted)
	ON_BN_CLICKED(IDC_AWARD, OnAward)
	ON_BN_CLICKED(IDC_SEL_DELV_NPC, OnSelDelvNpc)
	ON_BN_CLICKED(IDC_SEL_AWARD_NPC, OnSelAwardNpc)
	ON_BN_CLICKED(IDC_EDIT_GIVEN_ITEMS, OnEditGivenItems)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskClassPage message handlers

BOOL CTaskClassPage::UpdatePage(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly)
{
	if (bUpdateData)
	{
		UpdateData();

		pTask->SetName(CSafeString(m_strName));
		pTask->m_ulSuitableLevel = m_dwSuitLev;
		pTask->m_ulTimeLimit = m_dwTimeLimit;
		pTask->m_ulPremise_Lev_Max = m_dwLevMax;
		pTask->m_ulPremise_Lev_Min = m_dwLevMin;
		pTask->m_ulType = GetTaskTypeFromComboBox(m_nType);
		pTask->m_bTeamwork = (m_bTeamwork != 0);
		pTask->m_ulSpecialAward = m_dwSpecialAward;
		pTask->m_bCanSeekOut = (m_bCanSeekOut != 0);
		pTask->m_bShowDirection = (m_bShowDirection != 0);
		pTask->m_ulPremise_Period = period_map[m_nPeriod];
		pTask->m_bAutoDeliver = (m_bAutoDeliver != 0);

		int len;

		CSafeString strDes(m_strDescript);
		len = wcslen(strDes);
		delete[] pTask->m_pwstrDescript;
		g_ulDelCount++;
		pTask->m_pwstrDescript = NULL;

		if (len)
		{
			pTask->m_pwstrDescript = new wchar_t[len+1];
			g_ulNewCount++;
			wcscpy(pTask->m_pwstrDescript, strDes);
		}

		CSafeString strDelvNPCText(m_strDelvNPCText);
		len = wcslen(strDelvNPCText);

		if (pTask->m_DelvTaskTalk.windows[0].talk_text_len)
		{
			delete[] pTask->m_DelvTaskTalk.windows[0].talk_text;
			g_ulDelCount++;
		}

		pTask->m_DelvTaskTalk.windows[0].talk_text_len = len + 1;
		pTask->m_DelvTaskTalk.windows[0].talk_text = new wchar_t[len+1];
		g_ulNewCount++;
		wcscpy(pTask->m_DelvTaskTalk.windows[0].talk_text, strDelvNPCText);

		wcscpy(pTask->m_DelvTaskTalk.windows[0].options[0].text, CSafeString(m_strDelvYesText));
		wcscpy(pTask->m_DelvTaskTalk.windows[0].options[1].text, CSafeString(m_strDelvNoText));

		CSafeString strUnfinishedNPCText(m_strUnfinishedNPCText);
		len = wcslen(strUnfinishedNPCText);

		if (pTask->m_ExeTalk.windows[0].talk_text_len)
		{
			delete[] pTask->m_ExeTalk.windows[0].talk_text;
			g_ulDelCount++;
		}

		pTask->m_ExeTalk.windows[0].talk_text_len = len + 1;
		pTask->m_ExeTalk.windows[0].talk_text = new wchar_t[len+1];
		g_ulNewCount++;
		wcscpy(pTask->m_ExeTalk.windows[0].talk_text, strUnfinishedNPCText);

		wcscpy(pTask->m_ExeTalk.windows[0].options[0].text, CSafeString(m_strUnfinishedPlayerText));

		CSafeString strFinishNPCText(m_strFinishNPCText);
		len = wcslen(strFinishNPCText);

		if (pTask->m_AwardTalk.windows[0].talk_text_len)
		{
			delete[] pTask->m_AwardTalk.windows[0].talk_text;
			g_ulDelCount++;
		}

		pTask->m_AwardTalk.windows[0].talk_text_len = len + 1;
		pTask->m_AwardTalk.windows[0].talk_text = new wchar_t[len+1];
		wcscpy(pTask->m_AwardTalk.windows[0].talk_text, strFinishNPCText);
		g_ulNewCount++;

		wcscpy(pTask->m_AwardTalk.windows[0].options[0].text, CSafeString(m_strFinishPlayerText));
	}
	else
	{
		switch (pTask->m_enumMethod)
		{
		case enumTMKillNumMonster:
			GetDlgItem(IDC_SEL_MONSTER)->EnableWindow();
			GetDlgItem(IDC_EDIT_ITEMS_WANTED)->EnableWindow(false);
			GetDlgItem(IDC_EDIT_GIVEN_ITEMS)->EnableWindow(false);
			break;
		case enumTMCollectNumArticle:
		case enumTMTalkToNPC:
			GetDlgItem(IDC_SEL_MONSTER)->EnableWindow(false);
			GetDlgItem(IDC_EDIT_ITEMS_WANTED)->EnableWindow();
			GetDlgItem(IDC_EDIT_GIVEN_ITEMS)->EnableWindow();
			break;
		case enumTMWaitTime:
			GetDlgItem(IDC_SEL_MONSTER)->EnableWindow(false);
			GetDlgItem(IDC_EDIT_ITEMS_WANTED)->EnableWindow(false);
			GetDlgItem(IDC_EDIT_GIVEN_ITEMS)->EnableWindow(false);
			break;
		default:
			ASSERT(FALSE);
		}

		GetDlgItem(IDC_SEL_DELV_NPC)->EnableWindow(!bReadOnly);
		GetDlgItem(IDC_SEL_AWARD_NPC)->EnableWindow(!bReadOnly);
		GetDlgItem(IDC_SPECIAL_AWARD)->EnableWindow(false);

		switch (pTask->m_DynTaskType)
		{
		case enumDTTSpecialAward:
			GetDlgItem(IDC_SEL_DELV_NPC)->EnableWindow(false);
			GetDlgItem(IDC_SEL_AWARD_NPC)->EnableWindow(false);
			GetDlgItem(IDC_SEL_MONSTER)->EnableWindow(false);
			GetDlgItem(IDC_EDIT_ITEMS_WANTED)->EnableWindow(false);
			GetDlgItem(IDC_EDIT_GIVEN_ITEMS)->EnableWindow(false);
			GetDlgItem(IDC_SPECIAL_AWARD)->EnableWindow(!bReadOnly);
			break;
		case enumDTTGiftCard:
			GetDlgItem(IDC_SEL_DELV_NPC)->EnableWindow(false);
			GetDlgItem(IDC_SEL_AWARD_NPC)->EnableWindow(false);
			GetDlgItem(IDC_SEL_MONSTER)->EnableWindow(false);
			GetDlgItem(IDC_EDIT_ITEMS_WANTED)->EnableWindow(false);
			GetDlgItem(IDC_EDIT_GIVEN_ITEMS)->EnableWindow(false);
			GetDlgItem(IDC_AUTO_DELIVER)->EnableWindow(false);
			GetDlgItem(IDC_TEAM)->EnableWindow(false);
			GetDlgItem(IDC_SPECIAL_AWARD)->EnableWindow(!bReadOnly);
			break;
		}

		m_strName = pTask->GetName();
		m_strID.Format("%d", pTask->GetID());
		m_dwSuitLev = pTask->m_ulSuitableLevel;
		m_dwTimeLimit = pTask->m_ulTimeLimit;
		m_dwLevMax = pTask->m_ulPremise_Lev_Max;
		m_dwLevMin = pTask->m_ulPremise_Lev_Min;

		m_strDelvNPC = GetBaseDataNameByID(pTask->m_ulDelvNPC);
		m_strAwardNPC = GetBaseDataNameByID(pTask->m_ulAwardNPC);
		m_strDelvNPCText = CSafeString(pTask->m_DelvTaskTalk.windows[0].talk_text, pTask->m_DelvTaskTalk.windows[0].talk_text_len).GetAnsi();

		m_strDelvYesText = CSafeString(pTask->m_DelvTaskTalk.windows[0].options[0].text).GetAnsi();
		m_strDelvNoText = CSafeString(pTask->m_DelvTaskTalk.windows[0].options[1].text).GetAnsi();
		m_strUnfinishedNPCText = CSafeString(pTask->m_ExeTalk.windows[0].talk_text, pTask->m_ExeTalk.windows[0].talk_text_len).GetAnsi();
		m_strUnfinishedPlayerText = CSafeString(pTask->m_ExeTalk.windows[0].options[0].text).GetAnsi();

		m_strFinishNPCText = CSafeString(pTask->m_AwardTalk.windows[0].talk_text, pTask->m_AwardTalk.windows[0].talk_text_len).GetAnsi();
		m_strFinishPlayerText = CSafeString(pTask->m_AwardTalk.windows[0].options[0].text).GetAnsi();

		if (pTask->m_pwstrDescript)
			m_strDescript = CSafeString(pTask->GetDescription()).GetAnsi();
		else
			m_strDescript.Empty();

		m_strPreTask.Empty();

		if (pTask->m_ulPremise_Task_Count)
		{
			ATaskTempl* pPrem = GetTaskTemplMan()->GetTaskTemplByID(pTask->m_ulPremise_Tasks[0]);
			if (pPrem) m_strPreTask = pPrem->GetName();
		}

		m_nType = GetComboBoxFromTaskType(pTask->m_ulType);
		m_bTeamwork = pTask->m_bTeamwork;
		m_dwSpecialAward = pTask->m_ulSpecialAward;
		m_bCanSeekOut = pTask->m_bCanSeekOut;
		m_bShowDirection = pTask->m_bShowDirection;
		m_nPeriod = find_period_index(pTask->m_ulPremise_Period);
		m_bAutoDeliver = pTask->m_bAutoDeliver;

		m_pTask = pTask;
		UpdateData(FALSE);
	}

	return TRUE;
}

void CTaskClassPage::OnSelMonster()
{
	if (!m_pTask)
		return;

	CMonWantedDlg dlg(m_pTask->m_MonsterWanted, &m_pTask->m_ulMonsterWanted);
	dlg.DoModal();
}

void CTaskClassPage::OnEditItemsWanted()
{
	if (!m_pTask) return;

	CItemsWantedDlg dlg(
		MAX_ITEM_WANTED,
		m_pTask->m_ItemsWanted,
		&m_pTask->m_ulItemsWanted);

	dlg.DoModal();

	if (m_pTask->m_ulItemsWanted)
		m_pTask->m_enumMethod = enumTMCollectNumArticle;
	else
		m_pTask->m_enumMethod = enumTMTalkToNPC;
}

void CTaskClassPage::OnEditGivenItems()
{
	if (!m_pTask) return;

	CItemsWantedDlg dlg(
		MAX_ITEM_WANTED,
		m_pTask->m_GivenItems,
		&m_pTask->m_ulGivenItems);

	dlg.DoModal();
}

void CTaskClassPage::OnAward()
{
	if (!m_pTask) return;

	CAwardDlg dlg(m_pTask->m_Award_S);
	dlg.DoModal();
}

extern BOOL IsReadOnly(LPCTSTR szFile);

bool change_npc_task(bool bDelv, unsigned long ulNPC, unsigned long ulTask, bool bAdd)
{
	if (g_BaseIDMan.Load("BaseData\\TemplID.dat") != 0)
		return false;

	CString strPath = g_BaseIDMan.GetPathByID(ulNPC);
	BaseDataTempl tmpl;
	unsigned long ulSvrID = 0;
	bool bRet = false;

	if (strPath.IsEmpty())
	{
		// npc被删除
		g_BaseIDMan.Release();
		return !bAdd;
	}

	if (tmpl.Load(strPath) == 0)
	{
		int nItem = tmpl.GetItemIndex(bDelv ? "发放任务服务" : "验证完成任务服务");
		if (nItem >= 0) ulSvrID = tmpl.GetItemValue(nItem);
		tmpl.Release();
	}

	if (ulSvrID)
	{
		CString strSvr = g_BaseIDMan.GetPathByID(ulSvrID);
		BaseDataTempl svr;

		bool bCurCheckOut;
		int nFind = strSvr.ReverseFind('\\');
		CString strPath = strSvr.Left(nFind);

		if (IsReadOnly(strSvr))
		{
			g_VSS.SetProjectPath(strPath);
			g_VSS.CheckOutFile(strSvr);
			bCurCheckOut = false;
		}
		else
			bCurCheckOut = true;

		if (!IsReadOnly(strSvr))
		{
			if (svr.Load(strSvr) == 0)
			{
				bool bFind = false;

				if (bAdd)
				{
					for (int i = 0; i < svr.GetItemNum(); i++)
					{
						unsigned long ulId = svr.GetItemValue(i);

						if (ulId == ulTask)
						{
							bFind = true;
							break;
						}
					}
				}

				if (!bFind)
				{
					int i = 0;
					const char* sTask = "任务";
					while (1)
					{
						AString sName = svr.GetItemName(i);
						if (sName.Find(sTask) == 0)
						{
							break;
						}
						++i;
					}
					
					for (; i < svr.GetItemNum(); i++)
					{
						unsigned long ulId = svr.GetItemValue(i);

						if (bAdd)
						{
							if (ulId == 0)
							{
								svr.SetItemValue(i, static_cast<unsigned long>(ulTask));
								bRet = true;
								break;
							}
						}
						else if (ulId == ulTask)
						{
							svr.SetItemValue(i, static_cast<unsigned long>(0));
							bRet = true;
						}
					}

					if (!bRet)
					{
						if (bAdd)
							AfxMessageBox("NPC任务已满");
						else
						{
							AfxMessageBox("NPC无此任务");
							bRet = true;
						}
					}
					else
					{
						svr.Save(strSvr);
						WriteMd5CodeToTemplate(strSvr);
					}
				}

				svr.Release();
			}

			if (!bCurCheckOut)
			{
				g_VSS.SetProjectPath(strPath);
				g_VSS.CheckInFile(PathFindFileName(strSvr));
			}
		}
	}

	g_BaseIDMan.Release();
	return bRet;
}

bool sel_task_delv_npc(ATaskTempl* pTask)
{
	CTemplIDSelDlg dlg;
	dlg.m_strRoot = "BaseData\\NPC\\功能NPC";
	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = pTask->m_ulDelvNPC;

	if (dlg.DoModal() != IDOK)
		return false;

	if (dlg.m_ulID == pTask->m_ulDelvNPC)
		return false;

	if (pTask->m_ulDelvNPC)
	{
		if (change_npc_task(true, pTask->m_ulDelvNPC, pTask->GetID(), false))
			pTask->m_ulDelvNPC = 0;
		else
			return false;
	}

	if (dlg.m_ulID)
	{
		if (change_npc_task(true, dlg.m_ulID, pTask->GetID(), true))
			pTask->m_ulDelvNPC = dlg.m_ulID;
	}

	return true;
}

bool sel_task_award_npc(ATaskTempl* pTask)
{
	CTemplIDSelDlg dlg;
	dlg.m_strRoot = "BaseData\\NPC\\功能NPC";
	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = pTask->m_ulAwardNPC;

	if (dlg.DoModal() != IDOK)
		return false;

	if (dlg.m_ulID == pTask->m_ulAwardNPC)
		return false;

	if (pTask->m_ulAwardNPC)
	{
		if (change_npc_task(false, pTask->m_ulAwardNPC, pTask->GetID(), false))
			pTask->m_ulAwardNPC = 0;
		else
			return false;
	}

	if (dlg.m_ulID)
	{
		if (change_npc_task(false, dlg.m_ulID, pTask->GetID(), true))
			pTask->m_ulAwardNPC = dlg.m_ulID;
	}

	return true;
}

void CTaskClassPage::OnSelDelvNpc()
{
	if (!m_pTask) return;

	if (sel_task_delv_npc(m_pTask))
		GetDlgItem(IDC_DELV_NPC)->SetWindowText(GetBaseDataNameByID(m_pTask->m_ulDelvNPC));
}

void CTaskClassPage::OnSelAwardNpc()
{
	if (!m_pTask) return;

	if (sel_task_award_npc(m_pTask))
		GetDlgItem(IDC_AWARD_NPC)->SetWindowText(GetBaseDataNameByID(m_pTask->m_ulAwardNPC));
}

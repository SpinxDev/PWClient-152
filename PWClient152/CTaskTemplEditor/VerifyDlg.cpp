// VerifyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "VerifyDlg.h"
#include "BaseDataIDMan.h"
#include "BaseDataTemplate.h"
#include "ExpTypes.h"
#include "TaskTempl.h"
#include "TaskTemplMan.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVerifyDlg dialog


CVerifyDlg::CVerifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVerifyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVerifyDlg)
	m_strDescript = _T("错误描述");
	//}}AFX_DATA_INIT
}


void CVerifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVerifyDlg)
	DDX_Control(pDX, IDC_REPORT, m_Report);
	DDX_Text(pDX, IDC_DESCRIPT, m_strDescript);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVerifyDlg, CDialog)
	//{{AFX_MSG_MAP(CVerifyDlg)
	ON_BN_CLICKED(IDC_NO_DELIVER_TALK, OnNoDeliverTalk)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_DUP_MONSTER, OnDupMonster)
	ON_BN_CLICKED(IDC_WRONG_NPC, OnWrongNpc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVerifyDlg message handlers

abase::vector<AString> PathArray;

BOOL CVerifyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Report.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	PathArray.clear();
	g_BaseIDMan.Release();

	if (g_BaseIDMan.Load("BaseData\\TemplID.dat") != 0)
		AfxMessageBox("打开BaseID文件失败", MB_ICONSTOP);

	g_BaseIDMan.GeneratePathArray(PathArray);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVerifyDlg::OnNoDeliverTalk()
{
	DeleteAllCol();

	m_strDescript = "任务挂在NPC上，但NPC无接收任务对话，或有显示条件，但无不满足条件对话";
	UpdateData(FALSE);

	m_Report.InsertColumn(0, "NPC名称", LVCFMT_LEFT, 150);
	m_Report.InsertColumn(1, "任务名称", LVCFMT_LEFT, 150);
	m_Report.InsertColumn(2, "任务路径", LVCFMT_LEFT, 150);
	int nCount = 0;

	for (size_t i = 0; i < PathArray.size(); i++)
	{
		BaseDataTempl tmpl;

		if (PathArray[i].Find("BaseData\\NPC\\功能NPC\\") == -1
		 || tmpl.Load(PathArray[i]) != 0)
			continue;

		AString strNPC = tmpl.GetName();

		unsigned long ulSvrID = 0;
		int nItem = tmpl.GetItemIndex("发放任务服务");
		if (nItem >= 0) ulSvrID = tmpl.GetItemValue(nItem);
		tmpl.Release();
		if (!ulSvrID) continue;

		AString strSvr = g_BaseIDMan.GetPathByID(ulSvrID);

		if (strSvr.IsEmpty() || tmpl.Load(strSvr) != 0)
			continue;

		for (int j = 0; j < tmpl.GetItemNum(); j++)
		{
			unsigned long ulId = tmpl.GetItemValue(j);
			ATaskTempl task;

			AString path = g_TaskIDMan.GetPathByID(ulId);
			if (!task.LoadFromTextFile("BaseData\\TaskTemplate\\" + path + ".tkt"))
				continue;

			if (task.GetDeliverTaskTalk()->num_window == 0
			 || task.HasShowCond() && task.GetUnqualifiedTalk()->num_window == 0)
			{
				m_Report.InsertItem(nCount, CSafeString(strNPC));
				m_Report.SetItemText(nCount, 1, CSafeString(task.GetName()));
				m_Report.SetItemText(nCount, 2, path);
				nCount++;
			}
		}

		tmpl.Release();
	}
}

void CVerifyDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	PathArray.clear();
	g_BaseIDMan.Release();
}

void CVerifyDlg::DeleteAllCol()
{
	m_Report.DeleteAllItems();

	int nCols = m_Report.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nCols; i++)
		m_Report.DeleteColumn(0);
}

void CVerifyDlg::OnDupMonster() 
{
	DeleteAllCol();

	m_strDescript = "不同任务所杀怪相同";
	UpdateData(FALSE);

	m_Report.InsertColumn(0, "任务名称", LVCFMT_LEFT, 150);
	m_Report.InsertColumn(1, "任务路径", LVCFMT_LEFT, 150);
	m_Report.InsertColumn(2, "怪物ID", LVCFMT_LEFT, 150);
	int nCount = 0;

	typedef abase::hash_map<int, int> MonsterMap;
	MonsterMap monster;
	TaskTemplMap& all_map = GetTaskTemplMan()->GetAllTemplMap();

	TaskTemplMap::iterator it;
	for (it = all_map.begin(); it != all_map.end(); ++it)
	{
		ATaskTempl* pTask = it->second;
		if (pTask->m_pFirstChild || pTask->m_enumMethod != enumTMKillNumMonster)
			continue;

		monster[pTask->m_MonsterWanted[0].m_ulMonsterTemplId] = monster[pTask->m_MonsterWanted[0].m_ulMonsterTemplId] + 1; 
	}

	for (MonsterMap::iterator itMon = monster.begin(); itMon != monster.end(); ++itMon)
	{
		if (itMon->second < 2) continue;

		for (it = all_map.begin(); it != all_map.end(); ++it)
		{
			ATaskTempl* pTask = it->second;
			if (pTask->m_pFirstChild
			 || pTask->m_enumMethod != enumTMKillNumMonster
			 || pTask->m_MonsterWanted[0].m_ulMonsterTemplId != itMon->first)
				continue;

			m_Report.InsertItem(nCount, CSafeString(pTask->GetName()));
			m_Report.SetItemText(nCount, 1, g_TaskIDMan.GetPathByID(pTask->GetID()));
			CString str;
			str.Format("%d", pTask->m_MonsterWanted[0].m_ulMonsterTemplId);
			m_Report.SetItemText(nCount, 2, str);
			nCount++;
		}
	}
}

extern void check_task_npc(
	bool bDelv,
	bool bCheckOnly,
	CUIntArray* pNoTaskArr,
	CStringArray* pNoTaskNPCArr,
	CStringArray* pWrongTaskArr,
	CStringArray* pWrongTaskNPCArr,
	CStringArray* pWrongTaskPath
	);

void CVerifyDlg::wrong_npc_check(bool bDelv)
{
	int nCount = m_Report.GetItemCount();

	CUIntArray NoTaskArr;
	CStringArray NoTaskNPCArr;
	CStringArray WrongTaskArr;
	CStringArray WrongTaskNPCArr;
	CStringArray WrongTaskPath;

	check_task_npc(bDelv, true, &NoTaskArr, &NoTaskNPCArr, &WrongTaskArr, &WrongTaskNPCArr, &WrongTaskPath);

	for (int i = 0; i < NoTaskArr.GetSize(); i++)
	{
		CString str;
		m_Report.InsertItem(nCount, bDelv ? "发放" : "接收");
		m_Report.SetItemText(nCount, 1, NoTaskNPCArr[i]);
		const ATaskTempl* p = GetTaskTemplMan()->GetTaskTemplByID(NoTaskArr[i]);
		if (p)
			str = p->GetName();
		else
			str.Format("%d", NoTaskArr[i]);
		m_Report.SetItemText(nCount, 2, str);
		nCount++;
	}
}

void CVerifyDlg::OnWrongNpc()
{
	DeleteAllCol();

	m_strDescript = "发放或接收NPC不匹配";
	UpdateData(FALSE);

	m_Report.InsertColumn(0, "NPC类型", LVCFMT_LEFT, 50);
	m_Report.InsertColumn(1, "NPC", LVCFMT_LEFT, 180);
	m_Report.InsertColumn(2, "任务名称", LVCFMT_LEFT, 180);

	wrong_npc_check(true);
	wrong_npc_check(false);
}

// MonWantedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "MonWantedDlg.h"
#include "MonPropDlg.h"
#include "TaskTempl.h"
#include "TemplIDSelDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonWantedDlg dialog


CMonWantedDlg::CMonWantedDlg(MONSTER_WANTED* pMonsters, unsigned long* pCount, CWnd* pParent /*=NULL*/)
	: m_pMonsters(pMonsters), m_pCount(pCount), CDialog(CMonWantedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonWantedDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMonWantedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonWantedDlg)
	DDX_Control(pDX, IDC_MONSTERS, m_Monsters);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMonWantedDlg, CDialog)
	//{{AFX_MSG_MAP(CMonWantedDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_EDIT_MONSTER, OnEditMonster)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonWantedDlg message handlers

BOOL CMonWantedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Monsters.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_Monsters.InsertColumn(0, _T("怪物名称"), LVCFMT_LEFT, 100);
	m_Monsters.InsertColumn(1, _T("数量"), LVCFMT_LEFT, 40);
	m_Monsters.InsertColumn(2, _T("掉落物品名称"), LVCFMT_LEFT, 100);
	m_Monsters.InsertColumn(3, _T("数量"), LVCFMT_LEFT, 40);
	m_Monsters.InsertColumn(4, _T("概率"), LVCFMT_LEFT, 60);
	m_Monsters.InsertColumn(5, _T("等级限制"), LVCFMT_LEFT, 80);

	UpdateList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMonWantedDlg::UpdateList()
{
	m_Monsters.DeleteAllItems();

	for (unsigned long i = 0; i < *m_pCount; i++)
	{
		MONSTER_WANTED& mw = m_pMonsters[i];

		m_Monsters.InsertItem(
			i,
			GetBaseDataNameByID(mw.m_ulMonsterTemplId));

		CString strNum;
		strNum.Format("%d", mw.m_ulMonsterNum);
		m_Monsters.SetItemText(i, 1, strNum);

		m_Monsters.SetItemText(
			i,
			2,
			GetBaseDataNameByID(mw.m_ulDropItemId));

		strNum.Format("%d", mw.m_ulDropItemCount);
		m_Monsters.SetItemText(i, 3, strNum);

		CString strProb;
		strProb.Format("%f", mw.m_fDropProb);
		m_Monsters.SetItemText(i, 4, strProb);

		m_Monsters.SetItemText(i, 5, mw.m_bKillerLev ? "是" : "否");
	}
}

void CMonWantedDlg::OnAdd()
{
	if (*m_pCount >= MAX_MONSTER_WANTED)
	{
		AfxMessageBox("已到达上限", MB_ICONSTOP);
		return;
	}

	MONSTER_WANTED& mw = m_pMonsters[*m_pCount];
	memset(&mw, 0, sizeof(mw));
	mw.m_fDropProb = 1.0f;

	CMonPropDlg dlg(&m_pMonsters[*m_pCount]);

	if (dlg.DoModal() == IDOK)
	{
		(*m_pCount)++;
		UpdateList();
	}
}

void CMonWantedDlg::OnDel() 
{
	POSITION pos = m_Monsters.GetFirstSelectedItemPosition();
	if (!pos || AfxMessageBox("确定吗？", MB_YESNO) != IDYES) return;

	unsigned long uItem = m_Monsters.GetNextSelectedItem(pos);

	for (unsigned long i = uItem+1; i < *m_pCount; i++)
		m_pMonsters[i-1] = m_pMonsters[i];

	(*m_pCount)--;
	UpdateList();	
}

void CMonWantedDlg::OnEditMonster()
{
	POSITION pos = m_Monsters.GetFirstSelectedItemPosition();
	if (!pos) return;
	unsigned long uItem = m_Monsters.GetNextSelectedItem(pos);

	MONSTER_WANTED& mw = m_pMonsters[uItem];

	CMonPropDlg dlg(&mw);

	if (dlg.DoModal() == IDOK)
		UpdateList();
}

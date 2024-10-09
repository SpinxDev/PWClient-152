// MonPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "MonPropDlg.h"
#include "TemplIDSelDlg.h"
#include "TaskTempl.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonPropDlg dialog

CMonPropDlg::CMonPropDlg(MONSTER_WANTED* pMonster, CWnd* pParent /*=NULL*/)
	: m_pMonster(pMonster), CDialog(CMonPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonPropDlg)
	m_strItem = _T("");
	m_dwDropNum = 0;
	m_fDropProb = 0.0f;
	m_strMonster = _T("");
	m_dwMonsterNum = 0;
	m_bKillerLev = FALSE;
	m_iDPS = 0;
	m_iDPH = 0;
	//}}AFX_DATA_INIT
}


void CMonPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonPropDlg)
	DDX_Text(pDX, IDC_DROP_ITEM, m_strItem);
	DDX_Text(pDX, IDC_DROP_NUM, m_dwDropNum);
	DDX_Text(pDX, IDC_DROP_PROB, m_fDropProb);
	DDX_Text(pDX, IDC_MONSTER, m_strMonster);
	DDX_Text(pDX, IDC_MONSTER_NUM, m_dwMonsterNum);
	DDX_Check(pDX, IDC_PLAYER_LEV, m_bKillerLev);
	DDX_Text(pDX, IDC_DPS, m_iDPS);
	DDX_Text(pDX, IDC_DPH, m_iDPH);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMonPropDlg, CDialog)
	//{{AFX_MSG_MAP(CMonPropDlg)
	ON_BN_CLICKED(IDC_SEL_ITEM, OnSelItem)
	ON_BN_CLICKED(IDC_SEL_MONSTER, OnSelMonster)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonPropDlg message handlers

BOOL CMonPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_strMonster	= GetBaseDataNameByID(m_pMonster->m_ulMonsterTemplId);
	m_dwMonsterNum	= m_pMonster->m_ulMonsterNum;
	m_strItem		= GetBaseDataNameByID(m_pMonster->m_ulDropItemId);
	m_dwDropNum		= m_pMonster->m_ulDropItemCount;
	m_fDropProb		= m_pMonster->m_fDropProb;
	m_bKillerLev	= m_pMonster->m_bKillerLev;
	m_iDPS			= m_pMonster->m_iDPS;
	m_iDPH			= m_pMonster->m_iDPH;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMonPropDlg::OnSelItem() 
{
	CTemplIDSelDlg dlg;
	dlg.m_strRoot = "BaseData\\物品";
	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = m_pMonster->m_ulDropItemId;

	if (dlg.DoModal() != IDOK) return;

	m_pMonster->m_ulDropItemId = dlg.m_ulID;
	m_pMonster->m_bDropCmnItem = (dlg.m_strSelPath.Find("任务物品\\") == -1);

	GetDlgItem(IDC_DROP_ITEM)->SetWindowText(dlg.m_strName);
}

void CMonPropDlg::OnSelMonster() 
{
	CTemplIDSelDlg dlg;
	dlg.m_strRoot = "BaseData\\怪物";
	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = m_pMonster->m_ulMonsterTemplId;

	if (dlg.DoModal() == IDOK)
	{
		m_pMonster->m_ulMonsterTemplId = dlg.m_ulID;
		GetDlgItem(IDC_MONSTER)->SetWindowText(dlg.m_strName);
	}
}

void CMonPropDlg::OnOK() 
{
	UpdateData();

	m_pMonster->m_ulMonsterNum		= m_dwMonsterNum;
	m_pMonster->m_ulDropItemCount	= m_dwDropNum;
	m_pMonster->m_fDropProb			= m_fDropProb;
	m_pMonster->m_bKillerLev		= (m_bKillerLev != FALSE);
	m_pMonster->m_iDPS				= m_iDPS;
	m_pMonster->m_iDPH				= m_iDPH;

	CDialog::OnOK();
}

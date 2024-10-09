// ItemsCandDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskTempl.h"
#include "ItemsCandDlg.h"
#include "ItemsWantedDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemsCandDlg dialog


CItemsCandDlg::CItemsCandDlg(AWARD_DATA* pAward, CWnd* pParent /*=NULL*/)
	: CDialog(CItemsCandDlg::IDD, pParent), m_pAward(pAward)
{
	//{{AFX_DATA_INIT(CItemsCandDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CItemsCandDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemsCandDlg)
	DDX_Control(pDX, IDC_ITEMS_CAND, m_Cands);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CItemsCandDlg, CDialog)
	//{{AFX_MSG_MAP(CItemsCandDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemsCandDlg message handlers

BOOL CItemsCandDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Cands.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_Cands.InsertColumn(0, _T("候选物品种类"), LVCFMT_LEFT, 110);
	InitList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CItemsCandDlg::OnAdd()
{
	if (m_pAward->m_ulCandItems >= MAX_AWARD_CANDIDATES)
	{
		AfxMessageBox("已到达种类上限");
		return;
	}

	AWARD_ITEMS_CAND& ic = m_pAward->m_CandItems[m_pAward->m_ulCandItems];
	CItemsWantedDlg dlg(
		MAX_ITEM_AWARD,
		ic.m_AwardItems,
		&ic.m_ulAwardItems);

	dlg.DoModal();
	ic.m_bRandChoose = dlg.m_bRandOne != 0;
	m_pAward->m_ulCandItems++;

	CString str;
	str.Format("物品种类%d", m_pAward->m_ulCandItems);
	m_Cands.InsertItem(m_pAward->m_ulCandItems-1, str);
}

void CItemsCandDlg::OnDel() 
{
	POSITION pos = m_Cands.GetFirstSelectedItemPosition();
	if (!pos || AfxMessageBox("确定吗？", MB_YESNO) != IDYES) return;

	int nItem = m_Cands.GetNextSelectedItem(pos);
	unsigned long i;

	for (i = nItem+1; i < m_pAward->m_ulCandItems; i++)
		m_pAward->m_CandItems[i-1] = m_pAward->m_CandItems[i];

	m_pAward->m_ulCandItems--;
	InitList();
}

void CItemsCandDlg::OnEdit() 
{
	POSITION pos = m_Cands.GetFirstSelectedItemPosition();
	if (!pos) return;

	int nItem = m_Cands.GetNextSelectedItem(pos);
	AWARD_ITEMS_CAND& ic = m_pAward->m_CandItems[nItem];
	CItemsWantedDlg dlg(
		MAX_ITEM_AWARD,
		ic.m_AwardItems,
		&ic.m_ulAwardItems);

	dlg.SetRandOne(ic.m_bRandChoose);
	dlg.DoModal();
	ic.m_bRandChoose = dlg.m_bRandOne != 0;
}

void CItemsCandDlg::InitList()
{
	m_Cands.DeleteAllItems();
	unsigned long i;

	for (i = 0; i < m_pAward->m_ulCandItems; i++)
	{
		CString str;
		str.Format("候选种类%d", i + 1);
		m_Cands.InsertItem(i, str);
	}
}

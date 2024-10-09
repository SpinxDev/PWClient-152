// AwardByItemsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskTempl.h"
#include "AwardByItemsDlg.h"
#include "NumDlg.h"
#include "AwardDlg.h"
#include "TemplIDSelDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAwardByItemsDlg dialog


CAwardByItemsDlg::CAwardByItemsDlg(AWARD_ITEMS_SCALE* pAward, CWnd* pParent /*=NULL*/)
	: m_pAward(pAward), CDialog(CAwardByItemsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAwardByItemsDlg)
	m_strItem = _T("");
	//}}AFX_DATA_INIT
}


void CAwardByItemsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAwardByItemsDlg)
	DDX_Control(pDX, IDC_AWARD, m_Award);
	DDX_Text(pDX, IDC_ITEM, m_strItem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAwardByItemsDlg, CDialog)
	//{{AFX_MSG_MAP(CAwardByItemsDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_EDIT_AWARD, OnEditAward)
	ON_BN_CLICKED(IDC_EDIT_NUM, OnEditNum)
	ON_BN_CLICKED(IDC_EDIT_ITEM, OnEditItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static LPCTSTR _ratio_text = _T("请输入物品数量");

/////////////////////////////////////////////////////////////////////////////
// CAwardByItemsDlg message handlers

BOOL CAwardByItemsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Award.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_Award.InsertColumn(0, _T("物品数量"), LVCFMT_LEFT, 100);
	UpdateList();
	if (m_pAward->m_ulItemId) m_strItem = GetBaseDataNameByID(m_pAward->m_ulItemId);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAwardByItemsDlg::OnAdd() 
{
	if (m_pAward->m_ulScales >= MAX_AWARD_SCALES)
	{
		AfxMessageBox("到达最大比例数目！", MB_ICONSTOP);
		return;
	}

	CNumDlg dlgNum(_ratio_text);
	if (dlgNum.DoModal() != IDOK) return;

	AWARD_DATA ad;
	memset(&ad, 0, sizeof(ad));
	CAwardDlg dlgAward(&ad);
	if (dlgAward.DoModal() != IDOK) return;

	int i, j;

	for (i = 0; i < (int)m_pAward->m_ulScales; i++)
		if ((int)m_pAward->m_Counts[i] < dlgNum.m_nNum)
			break;

	for (j = m_pAward->m_ulScales-1; j >= i; j--)
	{
		m_pAward->m_Awards[j+1] = m_pAward->m_Awards[j];
		m_pAward->m_Counts[j+1] = m_pAward->m_Counts[j];
	}

	m_pAward->m_Awards[i] = ad;
	m_pAward->m_Counts[i] = dlgNum.m_nNum;
	m_pAward->m_ulScales++;

	UpdateList();
}

void CAwardByItemsDlg::OnDel() 
{
	POSITION pos = m_Award.GetFirstSelectedItemPosition();
	if (!pos) return;

	if (AfxMessageBox("确定吗？", MB_YESNO) != IDYES)
		return;

	int nIndex = m_Award.GetNextSelectedItem(pos);
	m_Award.DeleteItem(nIndex);	

	unsigned long i = (unsigned long)nIndex + 1;

	for (; i < m_pAward->m_ulScales; i++)
	{
		m_pAward->m_Counts[i-1] = m_pAward->m_Counts[i];
		m_pAward->m_Awards[i-1] = m_pAward->m_Awards[i];
	}

	m_pAward->m_ulScales--;
}

void CAwardByItemsDlg::OnEditAward() 
{
	POSITION pos = m_Award.GetFirstSelectedItemPosition();
	if (!pos) return;
	
	int nIndex = m_Award.GetNextSelectedItem(pos);

	CAwardDlg dlg(&m_pAward->m_Awards[nIndex]);
	dlg.DoModal();
}

void CAwardByItemsDlg::OnEditNum() 
{
	POSITION pos = m_Award.GetFirstSelectedItemPosition();
	if (!pos) return;

	int nIndex = m_Award.GetNextSelectedItem(pos);

	CNumDlg dlgNum(_ratio_text);
	dlgNum.m_nNum = m_pAward->m_Counts[nIndex];
	if (dlgNum.DoModal() != IDOK) return;
	m_pAward->m_Counts[nIndex] = dlgNum.m_nNum;
	UpdateList();
}

void CAwardByItemsDlg::OnEditItem() 
{
	CTemplIDSelDlg dlg;
	dlg.m_strRoot = "BaseData\\物品\\任务物品";
	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = m_pAward->m_ulItemId;
	if (dlg.DoModal() != IDOK) return;
	m_pAward->m_ulItemId = dlg.m_ulID;
	GetDlgItem(IDC_ITEM)->SetWindowText(dlg.m_strName);
}

void CAwardByItemsDlg::UpdateList()
{
	m_Award.DeleteAllItems();
	unsigned long i;

	for (i = 0; i < m_pAward->m_ulScales; i++)
	{
		CString str;
		str.Format("%d", m_pAward->m_Counts[i]);
		m_Award.InsertItem(i, str);
	}
}

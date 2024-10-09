// AwardByRatioDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskTempl.h"
#include "AwardByRatioDlg.h"
#include "NumDlg.h"
#include "AwardDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAwardByRatioDlg dialog


CAwardByRatioDlg::CAwardByRatioDlg(AWARD_RATIO_SCALE* pAward, CWnd* pParent /*=NULL*/)
	: m_pAward(pAward), CDialog(CAwardByRatioDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAwardByRatioDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAwardByRatioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAwardByRatioDlg)
	DDX_Control(pDX, IDC_AWARD, m_Award);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAwardByRatioDlg, CDialog)
	//{{AFX_MSG_MAP(CAwardByRatioDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_EDIT_AWARD, OnEditAward)
	ON_BN_CLICKED(IDC_EDIT_RATIO, OnEditRatio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAwardByRatioDlg message handlers

static LPCTSTR _ratio_text = _T("请输入百分比");

void CAwardByRatioDlg::OnAdd() 
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

	float fRatio = (float)dlgNum.m_nNum / 100.f;
	int i, j;

	for (i = 0; i < (int)m_pAward->m_ulScales; i++)
		if (m_pAward->m_Ratios[i] > fRatio)
			break;

	for (j = m_pAward->m_ulScales-1; j >= i; j--)
	{
		m_pAward->m_Awards[j+1] = m_pAward->m_Awards[j];
		m_pAward->m_Ratios[j+1] = m_pAward->m_Ratios[j];
	}

	m_pAward->m_Awards[i] = ad;
	m_pAward->m_Ratios[i] = fRatio;
	m_pAward->m_ulScales++;

	UpdateList();
}

void CAwardByRatioDlg::OnDel() 
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
		m_pAward->m_Ratios[i-1] = m_pAward->m_Ratios[i];
		m_pAward->m_Awards[i-1] = m_pAward->m_Awards[i];
	}

	m_pAward->m_ulScales--;
}

void CAwardByRatioDlg::OnEditAward() 
{
	POSITION pos = m_Award.GetFirstSelectedItemPosition();
	if (!pos) return;
	
	int nIndex = m_Award.GetNextSelectedItem(pos);

	CAwardDlg dlg(&m_pAward->m_Awards[nIndex]);
	dlg.DoModal();
}

void CAwardByRatioDlg::OnEditRatio() 
{
	POSITION pos = m_Award.GetFirstSelectedItemPosition();
	if (!pos) return;

	int nIndex = m_Award.GetNextSelectedItem(pos);

	CNumDlg dlgNum(_ratio_text);
	dlgNum.m_nNum = int(m_pAward->m_Ratios[nIndex] * 100.f + .1f);
	if (dlgNum.DoModal() != IDOK) return;
	m_pAward->m_Ratios[nIndex] = dlgNum.m_nNum / 100.f;
	UpdateList();
}

BOOL CAwardByRatioDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Award.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_Award.InsertColumn(0, _T("比例"), LVCFMT_LEFT, 100);
	UpdateList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAwardByRatioDlg::UpdateList()
{
	m_Award.DeleteAllItems();
	unsigned long i;

	for (i = 0; i < m_pAward->m_ulScales; i++)
	{
		CString str;
		str.Format("%d", int(m_pAward->m_Ratios[i] * 100.f + .1f));
		m_Award.InsertItem(i, str);
	}
}

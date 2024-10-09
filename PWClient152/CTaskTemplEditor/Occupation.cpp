// Occupation.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskTempl.h"
#include "Occupation.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COccupation dialog


COccupation::COccupation(ATaskTempl* pTempl, CWnd* pParent /*=NULL*/)
	: m_pTempl(pTempl), CDialog(COccupation::IDD, pParent)
{
	//{{AFX_DATA_INIT(COccupation)
	m_nOccup = 0;
	//}}AFX_DATA_INIT
}


void COccupation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COccupation)
	DDX_Control(pDX, IDC_OCCUPATIONS, m_Occupations);
	DDX_CBIndex(pDX, IDC_OCCUPATION, m_nOccup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COccupation, CDialog)
	//{{AFX_MSG_MAP(COccupation)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COccupation message handlers

static const char* _occup[] =
{
	"武侠",
	"法师",
	"巫师",
	"妖精",
	"妖兽",
	"刺客",
	"羽芒",
	"羽灵",
	"剑灵",
	"魅灵",
	"夜影",
	"月仙"
};

BOOL COccupation::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Occupations.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_Occupations.InsertColumn(0, _T("职业"), LVCFMT_LEFT, 90);

	unsigned long i;

	for (i = 0; i < m_pTempl->m_ulOccupations; i++)
		m_Occupations.InsertItem(i, _occup[m_pTempl->m_Occupations[i]]);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COccupation::OnAdd() 
{
	if (m_pTempl->m_ulOccupations >= MAX_OCCUPATIONS)
	{
		AfxMessageBox("到达最大职业类型数目！", MB_ICONSTOP);
		return;
	}
	
	UpdateData();

	m_Occupations.InsertItem(
		m_pTempl->m_ulOccupations,
		_occup[m_nOccup]);

	m_pTempl->m_Occupations[m_pTempl->m_ulOccupations] = (unsigned long)m_nOccup;
	m_pTempl->m_ulOccupations++;
}

void COccupation::OnDel() 
{
	if (AfxMessageBox("确定吗？", MB_YESNO) != IDYES)
		return;

	POSITION pos = m_Occupations.GetFirstSelectedItemPosition();
	if (!pos) return;

	int nIndex = m_Occupations.GetNextSelectedItem(pos);
	m_Occupations.DeleteItem(nIndex);

	unsigned long i = (unsigned long)nIndex + 1;

	for (; i < m_pTempl->m_ulOccupations; i++)
		m_pTempl->m_Occupations[i-1] = m_pTempl->m_Occupations[i];

	m_pTempl->m_ulOccupations--;
}

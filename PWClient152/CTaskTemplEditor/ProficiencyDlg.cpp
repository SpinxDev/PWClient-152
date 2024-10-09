// ProficiencyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "ProficiencyDlg.h"
#include "TaskTempl.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char* _skill_name[MAX_LIVING_SKILLS] =
{
	"武器制造",
	"护具制造",
	"饰品制造",
	"药品、符箓制造"
};

/////////////////////////////////////////////////////////////////////////////
// CProficiencyDlg dialog


CProficiencyDlg::CProficiencyDlg(long* pData, CWnd* pParent /*=NULL*/)
	: CDialog(CProficiencyDlg::IDD, pParent), m_pData(pData)
{
	//{{AFX_DATA_INIT(CProficiencyDlg)
	m_dwVal1 = 0;
	m_dwVal2 = 0;
	m_dwVal3 = 0;
	m_dwVal4 = 0;
	m_str1 = _T("");
	m_str2 = _T("");
	m_str3 = _T("");
	m_str4 = _T("");
	//}}AFX_DATA_INIT
}


void CProficiencyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProficiencyDlg)
	DDX_Text(pDX, IDC_PROFICIENCY1, m_dwVal1);
	DDX_Text(pDX, IDC_PROFICIENCY2, m_dwVal2);
	DDX_Text(pDX, IDC_PROFICIENCY3, m_dwVal3);
	DDX_Text(pDX, IDC_PROFICIENCY4, m_dwVal4);
	DDX_Text(pDX, IDC_SKILL1, m_str1);
	DDX_Text(pDX, IDC_SKILL2, m_str2);
	DDX_Text(pDX, IDC_SKILL3, m_str3);
	DDX_Text(pDX, IDC_SKILL4, m_str4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProficiencyDlg, CDialog)
	//{{AFX_MSG_MAP(CProficiencyDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProficiencyDlg message handlers

BOOL CProficiencyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_str1 = _skill_name[0];
	m_str2 = _skill_name[1];
	m_str3 = _skill_name[2];
	m_str4 = _skill_name[3];

	m_dwVal1 = m_pData[0];
	m_dwVal2 = m_pData[1];
	m_dwVal3 = m_pData[2];
	m_dwVal4 = m_pData[3];

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProficiencyDlg::OnOK() 
{
	UpdateData();

	_skill_name[0] = m_str1;
	_skill_name[1] = m_str2;
	_skill_name[2] = m_str3;
	_skill_name[3] = m_str4;

	m_pData[0] = m_dwVal1;
	m_pData[1] = m_dwVal2;
	m_pData[2] = m_dwVal3;
	m_pData[3] = m_dwVal4;

	CDialog::OnOK();
}

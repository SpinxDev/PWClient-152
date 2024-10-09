// TimeSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "TimeSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeSetDlg dialog


CTimeSetDlg::CTimeSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimeSetDlg)
	m_nDay = 0;
	m_nHours = 0;
	m_nMinu = 0;
	m_nMouth = 0;
	m_nSec = 0;
	m_nYear = 0;
	//}}AFX_DATA_INIT
}


void CTimeSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeSetDlg)
	DDX_Text(pDX, IDC_EDIT_DAY, m_nDay);
	DDX_Text(pDX, IDC_EDIT_HOURS, m_nHours);
	DDX_Text(pDX, IDC_EDIT_MINU, m_nMinu);
	DDX_Text(pDX, IDC_EDIT_MONTH, m_nMouth);
	DDX_Text(pDX, IDC_EDIT_SEC, m_nSec);
	DDX_Text(pDX, IDC_EDIT_YEAR, m_nYear);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimeSetDlg, CDialog)
	//{{AFX_MSG_MAP(CTimeSetDlg)
	ON_BN_CLICKED(ID_SET_TO_NULL, OnSetToNull)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeSetDlg message handlers

BOOL CTimeSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_nYear  = m_Time.GetYear();
	m_nMouth = m_Time.GetMonth();
	m_nDay   = m_Time.GetDay();
	m_nHours = m_Time.GetHour();
	m_nMinu  = m_Time.GetMinute();
	m_nSec   = m_Time.GetSecond();

	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTimeSetDlg::OnOK()
{
	UpdateData(TRUE);
	m_Time = CTime(m_nYear,m_nMouth,m_nDay,m_nHours,m_nMinu,m_nSec);
	CDialog::OnOK();
}

void CTimeSetDlg::OnSetToNull() 
{
	// TODO: Add your control notification handler code here
	m_Time = CTime(0);
	CDialog::OnOK();
}

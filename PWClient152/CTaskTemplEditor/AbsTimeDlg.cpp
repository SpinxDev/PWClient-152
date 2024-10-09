// AbsTimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "AbsTimeDlg.h"
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsTimeDlg dialog


CAbsTimeDlg::CAbsTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAbsTimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbsTimeDlg)
	m_iHour = 0;
	m_iMinute = 0;
	//}}AFX_DATA_INIT
}


void CAbsTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsTimeDlg)
	DDX_Text(pDX, IDC_EDIT_HOUR, m_iHour);
	DDV_MinMaxInt(pDX, m_iHour, 0, 23);
	DDX_Text(pDX, IDC_EDIT_MINUTE, m_iMinute);
	DDV_MinMaxInt(pDX, m_iMinute, 0, 59);
	DDX_DateTimeCtrl(pDX, IDC_ABS_DATE, m_tmAbsTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbsTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CAbsTimeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsTimeDlg message handlers

BOOL CAbsTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

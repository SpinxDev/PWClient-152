// WeekSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "WeekSelDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWeekSelDlg dialog


CWeekSelDlg::CWeekSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWeekSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWeekSelDlg)
	m_nDayOfWeek = 0;
	//}}AFX_DATA_INIT
}


void CWeekSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWeekSelDlg)
	DDX_Radio(pDX, IDC_WEEK_1, m_nDayOfWeek);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWeekSelDlg, CDialog)
	//{{AFX_MSG_MAP(CWeekSelDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWeekSelDlg message handlers

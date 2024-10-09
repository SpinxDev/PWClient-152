// TimespanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TimespanDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimespanDlg dialog


CTimespanDlg::CTimespanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimespanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimespanDlg)
	m_dwStartHour = 0;
	m_dwStartMin = 0;
	m_dwEndHour = 0;
	m_dwEndMin = 0;
	m_tmEnd = CTime::GetCurrentTime();
	m_tmStart = CTime::GetCurrentTime();
	m_dwMonDayEnd = 1;
	m_dwMonDayStart = 1;
	m_nDateType = 0;
	m_dwWeekDayEnd = 1;
	m_dwWeekDayStart = 1;
	m_dwYearDayEnd = 1;
	m_dwYearMonthEnd = 1;
	m_dwYearDayStart = 1;
	m_dwYearMonthStart = 1;
	//}}AFX_DATA_INIT
}


void CTimespanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimespanDlg)
	DDX_Text(pDX, IDC_START_HOUR, m_dwStartHour);
	DDV_MinMaxDWord(pDX, m_dwStartHour, 0, 23);
	DDX_Text(pDX, IDC_START_MIN, m_dwStartMin);
	DDV_MinMaxDWord(pDX, m_dwStartMin, 0, 59);
	DDX_Text(pDX, IDC_END_HOUR, m_dwEndHour);
	DDV_MinMaxDWord(pDX, m_dwEndHour, 0, 23);
	DDX_Text(pDX, IDC_END_MIN, m_dwEndMin);
	DDV_MinMaxDWord(pDX, m_dwEndMin, 0, 59);
	DDX_DateTimeCtrl(pDX, IDC_END_DATE, m_tmEnd);
	DDX_DateTimeCtrl(pDX, IDC_START_DATE, m_tmStart);
	DDX_Text(pDX, IDC_MONTH_DAY_END, m_dwMonDayEnd);
	DDV_MinMaxDWord(pDX, m_dwMonDayEnd, 1, 31);
	DDX_Text(pDX, IDC_MONTH_DAY_START, m_dwMonDayStart);
	DDV_MinMaxDWord(pDX, m_dwMonDayStart, 1, 31);
	DDX_Radio(pDX, IDC_TYPE_1, m_nDateType);
	DDX_Text(pDX, IDC_WEEK_DAY_END, m_dwWeekDayEnd);
	DDV_MinMaxDWord(pDX, m_dwWeekDayEnd, 1, 7);
	DDX_Text(pDX, IDC_WEEK_DAY_START, m_dwWeekDayStart);
	DDV_MinMaxDWord(pDX, m_dwWeekDayStart, 1, 7);
	DDX_Text(pDX, IDC_YEAR_END_DAY, m_dwYearDayEnd);
	DDV_MinMaxDWord(pDX, m_dwYearDayEnd, 1, 31);
	DDX_Text(pDX, IDC_YEAR_END_MONTH, m_dwYearMonthEnd);
	DDV_MinMaxDWord(pDX, m_dwYearMonthEnd, 1, 12);
	DDX_Text(pDX, IDC_YEAR_START_DAY, m_dwYearDayStart);
	DDV_MinMaxDWord(pDX, m_dwYearDayStart, 1, 31);
	DDX_Text(pDX, IDC_YEAR_START_MONTH, m_dwYearMonthStart);
	DDV_MinMaxDWord(pDX, m_dwYearMonthStart, 1, 12);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimespanDlg, CDialog)
	//{{AFX_MSG_MAP(CTimespanDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimespanDlg message handlers

// TimetableDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskTempl.h"
#include "TimetableDlg.h"
#include "TimespanDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimetableDlg dialog


CTimetableDlg::CTimetableDlg(ATaskTempl* pTempl, CWnd* pParent /*=NULL*/)
	: m_pTempl(pTempl), CDialog(CTimetableDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimetableDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTimetableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimetableDlg)
	DDX_Control(pDX, IDC_TIMETALBE, m_Timetable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimetableDlg, CDialog)
	//{{AFX_MSG_MAP(CTimetableDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimetableDlg message handlers

static const char* _fm = "%Y年%m月%d日%H时%M分";
static const char* _fm_per_month = "每月%d日%d时%d分";
static const char* _fm_per_week = "每周%d, %d时%d分";
static const char* _fm_per_day = "每天%d时%d分";
static const char* _fm_per_year = "每年%d月%d日%d时%d分";

const TCHAR* _tm_type[] =
{
	_T("具体时间段"),
	_T("每月时间段"),
	_T("每周时间段"),
	_T("每天时间段"),
	_T("每年时间段")
};

BOOL CTimetableDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Timetable.InsertColumn(0, _T("类型"), LVCFMT_LEFT, 100);
	m_Timetable.InsertColumn(1, _T("起始时间"), LVCFMT_LEFT, 150);
	m_Timetable.InsertColumn(2, _T("终止时间"), LVCFMT_LEFT, 150);

	m_Timetable.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	_Update();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTimetableDlg::_Update()
{
	m_Timetable.DeleteAllItems();

	unsigned long i;
	CString str;

	for (i = 0; i < m_pTempl->m_ulTimetable; i++)
	{
		task_tm& s = m_pTempl->m_tmStart[i];
		task_tm& e = m_pTempl->m_tmEnd[i];

		m_Timetable.InsertItem(i, _tm_type[m_pTempl->m_tmType[i]]);

		switch (m_pTempl->m_tmType[i])
		{
		case 0: // date

			m_Timetable.SetItemText(i, 1, CTime(s.year, s.month, s.day, s.hour, s.min, 0).Format(_fm));
			m_Timetable.SetItemText(i, 2, CTime(e.year, e.month, e.day, e.hour, e.min, 0).Format(_fm));

			break;

		case 1: // per month

			str.Format(_fm_per_month, s.day, s.hour, s.min);
			m_Timetable.SetItemText(i, 1, str);

			str.Format(_fm_per_month, e.day, e.hour, e.min);
			m_Timetable.SetItemText(i, 2, str);

			break;

		case 2: // per week

			str.Format(_fm_per_week, s.wday, s.hour, s.min);
			m_Timetable.SetItemText(i, 1, str);

			str.Format(_fm_per_week, e.wday, e.hour, e.min);
			m_Timetable.SetItemText(i, 2, str);

			break;

		case 3: // per day

			str.Format(_fm_per_day, s.hour, s.min);
			m_Timetable.SetItemText(i, 1, str);

			str.Format(_fm_per_day, e.hour, e.min);
			m_Timetable.SetItemText(i, 2, str);

			break;

		case 4: // per year

			str.Format(_fm_per_year, s.month, s.day, s.hour, s.min);
			m_Timetable.SetItemText(i, 1, str);

			str.Format(_fm_per_year, e.month, e.day, e.hour, e.min);
			m_Timetable.SetItemText(i, 2, str);

			break;
		}
	}
}

void CTimetableDlg::OnAdd() 
{
	if (m_pTempl->m_ulTimetable >= MAX_TIMETABLE_SIZE)
	{
		AfxMessageBox("到达时间段最大数");
		return;
	}

	CTimespanDlg dlg;

	if (dlg.DoModal() == IDOK)
	{
		m_pTempl->m_tmType[m_pTempl->m_ulTimetable] = dlg.m_nDateType;
		task_tm& s = m_pTempl->m_tmStart[m_pTempl->m_ulTimetable];
		task_tm& e = m_pTempl->m_tmEnd[m_pTempl->m_ulTimetable];
		memset(&s, 0, sizeof(s));
		memset(&e, 0, sizeof(e));

		switch (dlg.m_nDateType)
		{
		case 0: // date

			s.year = dlg.m_tmStart.GetYear();
			s.month = dlg.m_tmStart.GetMonth();
			s.day = dlg.m_tmStart.GetDay();

			e.year = dlg.m_tmEnd.GetYear();
			e.month = dlg.m_tmEnd.GetMonth();
			e.day = dlg.m_tmEnd.GetDay();

			break;

		case 1: // per month

			s.day = dlg.m_dwMonDayStart;
			e.day = dlg.m_dwMonDayEnd;

			break;

		case 2: // per week

			s.wday = dlg.m_dwWeekDayStart;
			e.wday = dlg.m_dwWeekDayEnd;

			break;

		case 3: // per day

			break;
			
		case 4: // per year
			
			s.month = dlg.m_dwYearMonthStart;
			s.day = dlg.m_dwYearDayStart;

			e.month = dlg.m_dwYearMonthEnd;
			e.day = dlg.m_dwYearDayEnd;

			break;
		}

		s.hour = dlg.m_dwStartHour;
		s.min = dlg.m_dwStartMin;

		e.hour = dlg.m_dwEndHour;
		e.min = dlg.m_dwEndMin;

		m_pTempl->m_ulTimetable++;
	}

	_Update();
}

void CTimetableDlg::OnDel() 
{
	if (AfxMessageBox("确定吗？", MB_YESNO) != IDYES)
		return;

	POSITION pos = m_Timetable.GetFirstSelectedItemPosition();
	if (!pos) return;

	int nIndex = m_Timetable.GetNextSelectedItem(pos);
	unsigned long i = (unsigned long)nIndex + 1;

	for (; i < m_pTempl->m_ulTimetable; i++)
	{
		m_pTempl->m_tmType[i-1] = m_pTempl->m_tmType[i];
		m_pTempl->m_tmStart[i-1] = m_pTempl->m_tmStart[i];
		m_pTempl->m_tmEnd[i-1] = m_pTempl->m_tmEnd[i];
	}

	m_pTempl->m_tmType[i-1] = 0;
	memset(&m_pTempl->m_tmStart[i-1], 0, sizeof(task_tm));
	memset(&m_pTempl->m_tmEnd[i-1], 0, sizeof(task_tm));
	m_pTempl->m_ulTimetable--;

	_Update();
}

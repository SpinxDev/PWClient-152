// DlgExpireTime.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "DlgExpireTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExpireTime dialog


CDlgExpireTime::CDlgExpireTime(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExpireTime::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExpireTime)
	m_nDay = 0;
	m_nHour = 0;
	m_nMinute = 0;
	m_nMonth = 0;
	m_nSecond = 0;
	m_nYear = 0;
	//}}AFX_DATA_INIT
}


void CDlgExpireTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExpireTime)
	DDX_Control(pDX, IDC_EDIT_YEAR, m_year);
	DDX_Control(pDX, IDC_EDIT_SECOND, m_second);
	DDX_Control(pDX, IDC_EDIT_MONTH, m_month);
	DDX_Control(pDX, IDC_EDIT_MINUTE, m_minute);
	DDX_Control(pDX, IDC_EDIT_HOUR, m_hour);
	DDX_Control(pDX, IDC_EDIT_DAY, m_day);
	DDX_Text(pDX, IDC_EDIT_DAY, m_nDay);
	DDX_Text(pDX, IDC_EDIT_HOUR, m_nHour);
	DDX_Text(pDX, IDC_EDIT_MINUTE, m_nMinute);
	DDX_Text(pDX, IDC_EDIT_MONTH, m_nMonth);
	DDX_Text(pDX, IDC_EDIT_SECOND, m_nSecond);
	DDX_Text(pDX, IDC_EDIT_YEAR, m_nYear);
	//}}AFX_DATA_MAP
}


void CDlgExpireTime::SetTime(int value)
{
	time_t temp = (time_t)value;
	tm * t = localtime(&temp);

	m_nYear = t->tm_year + 1900;	//	1900~
	m_nMonth = t->tm_mon + 1;		//	0-11
	m_nDay = t->tm_mday;			//	1-31
	m_nHour = t->tm_hour;			//	0-23
	m_nMinute = t->tm_min;			//	0-59
	m_nSecond = t->tm_sec;			//	0-59?

	UpdateData(FALSE);
}

bool CDlgExpireTime::Validate()
{
	if (!UpdateData(TRUE))
		return false;
	
	if (m_nYear < 1970){
		m_year.SetFocus();
		m_year.SetSel(0, -1);
		return false;
	}
	if (m_nMonth < 1 || m_nMonth > 12){
		m_month.SetFocus();
		m_month.SetSel(0, -1);
		return false;
	}
	if (m_nDay < 1 || m_nDay > 31){
		m_day.SetFocus();
		m_day.SetSel(0, -1);
		return false;
	}	
	if (m_nHour < 0 || m_nHour > 23){
		m_hour.SetFocus();
		m_hour.SetSel(0, -1);
		return false;
	}
	if (m_nMinute < 0 || m_nMinute > 59){
		m_minute.SetFocus();
		m_minute.SetSel(0, -1);
		return false;
	}	
	if (m_nSecond < 0 || m_nSecond > 59){
		m_second.SetFocus();
		m_second.SetSel(0, -1);
		return false;
	}

	if (GetTimeImpl() == -1){
		MessageBox("mktime 失败，请使用其它时间！");
		return false;
	}

	return true;
}

int CDlgExpireTime::GetTime()
{
	int value = 0;
	if (Validate()){
		value = GetTimeImpl();
	}
	return value;
}

int CDlgExpireTime::GetTimeImpl()
{
	tm t;
	::memset(&t, 0, sizeof(t));
	t.tm_year = m_nYear - 1900;
	t.tm_mon = m_nMonth - 1;
	t.tm_mday = m_nDay;
	t.tm_hour = m_nHour;
	t.tm_min = m_nMinute;
	t.tm_sec = m_nSecond;
	t.tm_isdst = 0;	//	不采用夏令时
	return mktime(&t);
}


BEGIN_MESSAGE_MAP(CDlgExpireTime, CDialog)
	//{{AFX_MSG_MAP(CDlgExpireTime)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExpireTime message handlers

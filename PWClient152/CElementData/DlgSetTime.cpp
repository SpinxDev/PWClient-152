// DlgSetTime.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "DlgSetTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// SetTimeLengthFunc
BOOL CALLBACK SetTimeLengthFunc::OnActivate(void)
{
	CDlgSetTime dlg(m_var, CDlgSetTime::AS_TIME_LENGTH);
	if (dlg.DoModal() != IDOK) return FALSE;
	m_var = dlg.GetTime();
	return TRUE;
}

LPCTSTR CALLBACK SetTimeLengthFunc::OnGetShowString(void) const
{
	m_strShow.Empty();
	
	AVariant var = m_var;
	int value = var;
	int hour = value/3600;
	int minute = (value%3600)/60;
	int second = (value%3600)%60;

	m_strShow.Format("%d小时%02d分钟%02d秒", hour, minute, second);
	return m_strShow;
}

AVariant CALLBACK SetTimeLengthFunc::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK SetTimeLengthFunc::OnSetValue(const AVariant& var)
{
	m_var = var;
}

// SetExpireTimeFunc
BOOL CALLBACK SetExpireTimeFunc::OnActivate(void)
{
	CDlgSetTime dlg(m_var, CDlgSetTime::AS_EXPIRE_TIME);
	if (dlg.DoModal() != IDOK) return FALSE;
	m_var = dlg.GetTime();
	return TRUE;
}

LPCTSTR CALLBACK SetExpireTimeFunc::OnGetShowString(void) const
{
	m_strShow.Empty();
	
	AVariant var = m_var;
	time_t value = var;	
	tm t_loc = *localtime(&value);	
	int year_loc = t_loc.tm_year + 1900;	//	1900~
	int month_loc = t_loc.tm_mon + 1;		//	0-11
	int day_loc = t_loc.tm_mday;			//	1-31
	int hour_loc = t_loc.tm_hour;			//	0-23
	int minute_loc = t_loc.tm_min;			//	0-59
	int second_loc = t_loc.tm_sec;			//	0-59?

	tm t_gm = *gmtime(&value);
	int year_gm = t_gm.tm_year + 1900;	//	1900~
	int month_gm = t_gm.tm_mon + 1;		//	0-11
	int day_gm = t_gm.tm_mday;			//	1-31
	int hour_gm = t_gm.tm_hour;			//	0-23
	int minute_gm = t_gm.tm_min;			//	0-59
	int second_gm = t_gm.tm_sec;			//	0-59?
	
	m_strShow.Format("%d年%02d月%02d日%02d时%02d分%02d秒（格林威治时间：%d年%02d月%02d日%02d时%02d分%02d秒）"
		, year_loc, month_loc, day_loc, hour_loc, minute_loc, second_loc
		, year_gm, month_gm, day_gm, hour_gm, minute_gm, second_gm);
	return m_strShow;
}

AVariant CALLBACK SetExpireTimeFunc::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK SetExpireTimeFunc::OnSetValue(const AVariant& var)
{
	m_var = var;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgSetTime dialog


CDlgSetTime::CDlgSetTime(int value /* = 0 */, unsigned int type /* = AS_TIME_LENGTH */, CWnd* pParent /* = NULL */)
	: CDialog(CDlgSetTime::IDD, pParent)
	, m_type(type)
	, m_value(value)
{
	//{{AFX_DATA_INIT(CDlgSetTime)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSetTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetTime)
	DDX_Control(pDX, IDC_TAB_TIME, m_tab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetTime, CDialog)
	//{{AFX_MSG_MAP(CDlgSetTime)
	ON_WM_CANCELMODE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_TIME, OnSelchangeTabTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetTime message handlers

BOOL CDlgSetTime::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.top += 20;
	rect.bottom -= 5;
	rect.left += 5;
	rect.right -= 5;

	switch(m_type)
	{
	case AS_TIME_LENGTH:
		{
			m_tab.InsertItem(0, "时间长度");
			m_item1.Create(IDD_TIME_LENGTH, GetDlgItem(IDC_TAB_TIME));
			m_item1.SetTime(m_value);
			m_item1.MoveWindow(&rect);
			m_item1.ShowWindow(SW_SHOWNORMAL);
		}
		break;
	case AS_EXPIRE_TIME:
		{
			m_tab.InsertItem(0, "到期时间");
			m_item2.Create(IDD_EXPIRE_TIME, GetDlgItem(IDC_TAB_TIME));
			m_item2.SetTime(m_value);
			m_item2.MoveWindow(&rect);
			m_item2.ShowWindow(SW_SHOWNORMAL);
		}
	}

	m_tab.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSetTime::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CDlgSetTime::OnSelchangeTabTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgSetTime::OnOK() 
{
	// TODO: Add extra validation here
	switch(m_type)
	{
	case AS_TIME_LENGTH:
		{
			if (!m_item1.Validate()) return;
			m_value = m_item1.GetTime();
		}
		break;
	case AS_EXPIRE_TIME:
		{
			if (!m_item2.Validate()) return;
			m_value = m_item2.GetTime();
		}
	}
	
	CDialog::OnOK();
}

int CDlgSetTime::GetTime()
{
	return m_value;
}

// DlgTimeLength.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "DlgTimeLength.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTimeLength dialog


CDlgTimeLength::CDlgTimeLength(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTimeLength::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTimeLength)
	m_nHour = 0;
	m_nMinute = 0;
	m_nSecond = 0;
	//}}AFX_DATA_INIT
}


void CDlgTimeLength::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTimeLength)
	DDX_Control(pDX, IDC_EDIT_SECOND, m_second);
	DDX_Control(pDX, IDC_EDIT_MINUTE, m_minute);
	DDX_Control(pDX, IDC_EDIT_HOUR, m_hour);
	DDX_Text(pDX, IDC_EDIT_HOUR, m_nHour);
	DDX_Text(pDX, IDC_EDIT_MINUTE, m_nMinute);
	DDX_Text(pDX, IDC_EDIT_SECOND, m_nSecond);
	//}}AFX_DATA_MAP
}


void CDlgTimeLength::SetTime(int value)
{
	m_nHour = value/3600;
	m_nMinute = (value%3600)/60;
	m_nSecond = (value%3600)%60;
	UpdateData(FALSE);
}

bool CDlgTimeLength::Validate()
{
	if (!UpdateData(TRUE))
		return false;

	if (m_nHour < 0 || m_nHour > 365 * 24){
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

	if (GetTimeImpl() < 0)
		return false;

	return true;
}

int CDlgTimeLength::GetTime()
{
	int value = 0;
	if (Validate()){
		value = GetTimeImpl();
	}
	return value;
}

int CDlgTimeLength::GetTimeImpl()
{
	int value = m_nHour * 3600 + m_nMinute * 60 + m_nSecond;
	return value;
}

BEGIN_MESSAGE_MAP(CDlgTimeLength, CDialog)
	//{{AFX_MSG_MAP(CDlgTimeLength)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTimeLength message handlers

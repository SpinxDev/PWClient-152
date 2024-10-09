// DlgSetSpeed.cpp : implementation file
//

#include "stdafx.h"
#include "PWDownloader.h"
#include "DlgSetSpeed.h"
#include "EC_PWDownload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetSpeed dialog

static int l_iSpeedValue[] =
{
	65536, 262144, 524288, 1048576, 2097152, 4194304, 0
};


CDlgSetSpeed::CDlgSetSpeed(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetSpeed::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetSpeed)
	m_iDownSpeedLevel = -1;
	m_iUpSpeedLevel = -1;
	//}}AFX_DATA_INIT
}


void CDlgSetSpeed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetSpeed)
	DDX_CBIndex(pDX, IDC_DOWNSPEED, m_iDownSpeedLevel);
	DDX_CBIndex(pDX, IDC_UPSPEED, m_iUpSpeedLevel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetSpeed, CDialog)
	//{{AFX_MSG_MAP(CDlgSetSpeed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetSpeed message handlers

void CDlgSetSpeed::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	int dspeed = l_iSpeedValue[m_iDownSpeedLevel];
	int uspeed = l_iSpeedValue[m_iUpSpeedLevel];
	CECPWDownload::GetInstance().SetDownSpeed(dspeed, uspeed);	
	CDialog::OnOK();
}

int CDlgSetSpeed::GetSpeedLevel(int speed)
{
	for(int i=0;i<sizeof(l_iSpeedValue);i++)
	{
		if( speed == l_iSpeedValue[i] )
			return i;
	}

	return 0;
}

BOOL CDlgSetSpeed::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int dspeed = 0, uspeed = 0;
	CECPWDownload::GetInstance().GetMaxSpeed(dspeed, uspeed);
	m_iDownSpeedLevel = GetSpeedLevel(dspeed);
	m_iUpSpeedLevel = GetSpeedLevel(uspeed);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

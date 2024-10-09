// CDlgProgressNotify.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "DlgProgressNotify.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProgressNotify dialog

CString CDlgProgressNotify::m_strWndText = "请等待...";
int CDlgProgressNotify::m_nCurProgress = 0;
int CDlgProgressNotify::m_nProgressMax = 100;

static const UINT ID_TIMER_UPDATE = 0x1000;

CDlgProgressNotify::CDlgProgressNotify(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProgressNotify::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProgressNotify)
	//}}AFX_DATA_INIT
}


void CDlgProgressNotify::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProgressNotify)
	DDX_Control(pDX, IDC_STATIC1, m_Text);
	DDX_Control(pDX, IDC_PROGRESS1, m_Prgs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProgressNotify, CDialog)
	//{{AFX_MSG_MAP(CDlgProgressNotify)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProgressNotify message handlers

BOOL CDlgProgressNotify::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(m_strWndText);

	SetTimer(ID_TIMER_UPDATE, 20, 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProgressNotify::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	UpdateProgress();
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgProgressNotify::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	UpdateProgress();
}

void CDlgProgressNotify::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == ID_TIMER_UPDATE)
	{
		if (m_nCurProgress >= m_nProgressMax)
		{
			//	完成任务
			KillTimer(ID_TIMER_UPDATE);
			Finish();
			return;
		}

		//	显示进度
		InvalidateRect(NULL, FALSE);
	}
}

void CDlgProgressNotify::UpdateProgress()
{
	m_Prgs.SetRange32(0, m_nProgressMax);
	m_Prgs.SetStep(1);
	m_Prgs.SetPos(m_nCurProgress);

	CString strText;
	strText.Format("%d/%d", m_nCurProgress, m_nProgressMax);
	m_Text.SetWindowText(strText);
}

void CDlgProgressNotify::OnOK()
{
}

void CDlgProgressNotify::OnCancel()
{
}

void CDlgProgressNotify::Finish()
{
	CDialog::OnOK();
}
// VssDlgBar.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "VssDlgBar.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CVssDlgBar dialog


CVssDlgBar::CVssDlgBar(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CVssDlgBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CVssDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVssDlgBar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVssDlgBar, CDialogBar)
	//{{AFX_MSG_MAP(CVssDlgBar)
	ON_WM_SIZE()
	ON_EN_SETFOCUS(IDC_EDIT_VSS_LOG, OnSetfocusEditVssLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVssDlgBar message handlers

void CVssDlgBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CEdit *pWnd = (CEdit*)GetDlgItem(IDC_EDIT_VSS_LOG);
	CRect rect;
	GetClientRect(&rect);
	if(pWnd)
	{	
		pWnd->SetWindowPos(NULL,1,1,rect.Width()-2,20,SWP_FRAMECHANGED);
		pWnd->SetLimitText(1024*1024);
	}
}

void CVssDlgBar::AddString(const char *szMsg)
{
	CEdit*pEdit = (CEdit*)GetDlgItem(IDC_EDIT_VSS_LOG);
	if(pEdit==NULL) return;
	char szBuf[48*1024];
	va_list vaList;
	va_start(vaList, szMsg);
	vsprintf(szBuf, szMsg, vaList);
	va_end(vaList);

	strcat(szBuf, "\n");

	pEdit->SetSel(0, -1);
	pEdit->SetSel(-1, -1);
	pEdit->ReplaceSel(szBuf);
}

void CVssDlgBar::OnSetfocusEditVssLog() 
{
	// TODO: Add your control notification handler code here
	CEdit*pEdit = (CEdit*)GetDlgItem(IDC_EDIT_VSS_LOG);
	if(pEdit==NULL) return;
	GetParent()->SetFocus();
}

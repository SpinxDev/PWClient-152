// ObjectLightPanel.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "ObjectLightPanel.h"
#include "AObject.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CObjectLightPanel dialog
CObjectLightPanel::CObjectLightPanel(CWnd* pParent /*=NULL*/)
: CDialog(CObjectLightPanel::IDD, pParent)
{
}


void CObjectLightPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectLightPanel)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectLightPanel, CDialog)
	//{{AFX_MSG_MAP(CObjectLightPanel)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectLightPanel message handlers

void CObjectLightPanel::OnOK()
{

}

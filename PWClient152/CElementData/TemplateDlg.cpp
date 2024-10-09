// TemplateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "PropertyList.h"
#include "AObject.h"
#include "TemplateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg dialog


CTemplateDlg::CTemplateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg message handlers

BOOL CTemplateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pList = new CPropertyList();
	CRect rc;
	GetClientRect(&rc);
	rc.bottom = rc.bottom - 40;
	
	BOOL bResult = m_pList->Create("ÊôÐÔ±í", WS_CHILD | WS_VISIBLE, rc, this, 101);
	ASSERT(bResult);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTemplateDlg::SetProperty(ADynPropertyObject *pProperty)
{
	m_pList->AttachDataObject(pProperty);
}



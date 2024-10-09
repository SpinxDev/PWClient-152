// TemplateCreateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "TemplateCreateDlg.h"
#include "TemplIDSelDlg.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTemplateCreateDlg dialog


CTemplateCreateDlg::CTemplateCreateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateCreateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplateCreateDlg)
	m_strName = _T("");
	m_strSrcTempl = _T("");
	//}}AFX_DATA_INIT
}


void CTemplateCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateCreateDlg)
	DDX_Control(pDX, IDC_EDIT_TEMPLATE_NAME, m_EditTmplName);
	DDX_Text(pDX, IDC_EDIT_TEMPLATE_NAME, m_strName);
	DDX_Text(pDX, IDC_SRC_TEMPL, m_strSrcTempl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateCreateDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplateCreateDlg)
	ON_BN_CLICKED(IDC_SEL_COPY, OnSelCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateCreateDlg message handlers

void CTemplateCreateDlg::OnSelCopy() 
{
	CTemplIDSelDlg dlg;
	dlg.m_strRoot = m_strPath;
	dlg.m_strExt = ".tmpl";
	UpdateData();

	if (dlg.DoModal() == IDOK)
	{
		m_strSrcTempl = dlg.m_strFileName;
		UpdateData(FALSE);
	}
}

BOOL CTemplateCreateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_EditTmplName.SetFocus();
	return FALSE;
}

// DynamicObjectDlg.cpp : implementation file
//

#include "global.h"
#include "Render.h"
#include "elementeditor.h"
#include "DynamicObjectDlg.h"
#include "CommonFileDlg.h"

#include <A3D.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDynamicObjectDlg dialog


CDynamicObjectDlg::CDynamicObjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDynamicObjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDynamicObjectDlg)
	m_strPath = _T("");
	m_strID = _T("");
	m_strName = _T("");
	//}}AFX_DATA_INIT
	InitDynamicObj(&m_Object);
	m_bReadOnly = false;
}


void CDynamicObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDynamicObjectDlg)
	DDX_Text(pDX, IDC_EDIT_DYNAMIC_OBJECT_PATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_ID, m_strID);
	DDX_Text(pDX, IDC_EDIT_OBJ_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDynamicObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CDynamicObjectDlg)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynamicObjectDlg message handlers

void CDynamicObjectDlg::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	if(m_bReadOnly) return;
	UpdateData(true);
	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "","*.*");
	if (FileDlg.DoModal() != IDOK)
		return; 
	m_strPath = FileDlg.GetFullFileName();
	m_strPath.MakeLower();
	CString temp = m_strPath.Left(4);
	if(temp=="gfx\\") m_strPath.Delete(0,4);
	UpdateData(false);
}

BOOL CDynamicObjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_strID.Format("%d",m_Object.id);
	m_strName = m_Object.szName;
	m_strPath = m_Object.szObjectPath;
	UpdateData(false);
	
	if(m_bReadOnly)
	{
		CWnd *pWnd = GetDlgItem(IDC_EDIT_OBJ_NAME);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_EDIT_DYNAMIC_OBJECT_PATH);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_EDIT_ID);
		pWnd->EnableWindow(false);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDynamicObjectDlg::OnOK()
{
	UpdateData(true);
	
	CString temp = m_strPath.Left(4);
	if(temp=="gfx\\") m_strPath.Delete(0,4);
	
	m_Object.id = (unsigned int)atof(m_strID);
	strcpy(m_Object.szName, m_strName);
	strcpy(m_Object.szObjectPath, m_strPath);

	CDialog::OnOK();
}

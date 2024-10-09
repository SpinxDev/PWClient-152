// CopySubTaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "CopySubTaskDlg.h"
#include "TaskTempl.h"
#include "AVariant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCopySubTaskDlg dialog


CCopySubTaskDlg::CCopySubTaskDlg(ATaskTempl* pSrcTask,ATaskTempl* pDestTask,CWnd* pParent /*=NULL*/)
	: m_pSrcTask(pSrcTask), m_pDestTask(m_pDestTask), CDialog(CCopySubTaskDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCopySubTaskDlg)
	m_bCopySubTask = TRUE;
	m_strNewName = _T("");
	m_strOldName = _T("");
	//}}AFX_DATA_INIT
}


void CCopySubTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCopySubTaskDlg)
	DDX_Control(pDX, IDC_TREE_SUB_TASK, m_TreeSouceTask);
	DDX_Check(pDX, IDC_CHECK_COPY_SUB_TASK, m_bCopySubTask);
	DDX_Text(pDX, IDC_EDIT_NEW_NAME, m_strNewName);
	DDX_Text(pDX, IDC_STATIC_OLD_NAME, m_strOldName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCopySubTaskDlg, CDialog)
	//{{AFX_MSG_MAP(CCopySubTaskDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCopySubTaskDlg message handlers

BOOL CCopySubTaskDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ImageList.Create(IDB_LISTTAG, 16, 3, RGB(255, 0, 255));
	m_TreeSouceTask.SetImageList(&m_ImageList, TVSIL_NORMAL);

	HTREEITEM hItem = m_TreeSouceTask.InsertItem(CSafeString(m_pSrcTask->GetName()), 0,1);
	if(m_pSrcTask->m_pFirstChild)
		m_TreeSouceTask.InsertItem(CSafeString(m_pSrcTask->m_pFirstChild->GetName()), 0,1,hItem);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

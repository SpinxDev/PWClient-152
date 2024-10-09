// NewTaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "NewTaskDlg.h"
#include "TaskIDSelDlg.h"
#include "NumDlg.h"
#include "BaseDataIDMan.h"
#include "CopySubTaskDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewTaskDlg dialog


CNewTaskDlg::CNewTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewTaskDlg::IDD, pParent), m_ulSrc(0)
{
	//{{AFX_DATA_INIT(CNewTaskDlg)
	m_strName = _T("");
	m_strSrc = _T("");
	m_bCopySubTask = FALSE;
	//}}AFX_DATA_INIT
}


void CNewTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewTaskDlg)
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_SRC, m_strSrc);
	DDX_Check(pDX, IDC_COPY_SUB_TASK, m_bCopySubTask);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewTaskDlg, CDialog)
	//{{AFX_MSG_MAP(CNewTaskDlg)
	ON_BN_CLICKED(IDC_SEL_SRC, OnSelSrc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewTaskDlg message handlers

void CNewTaskDlg::OnSelSrc() 
{
	CNumDlg dlgNum("请输入任务id");
	dlgNum.m_nNum = m_ulSrc;

	if(dlgNum.DoModal() == IDOK)
	{
		if(dlgNum.m_nNum <= 0)
		{
			m_ulSrc = 0;
			m_strSrc = "";
			GetDlgItem(IDC_SRC)->SetWindowText("");
			return;
		}
		
		ATaskTempl* pTask = new ATaskTempl;
		
		CString strPath = g_TaskIDMan.GetPathByID(dlgNum.m_nNum);
		strPath = "BaseData\\TaskTemplate\\" + strPath + ".tkt";

		if(!pTask->LoadFromTextFile(strPath))
		{
			AfxMessageBox("找不到此任务，请输入正确的任务ID。");
			delete pTask;
			return;
		}
		
		CString strName = pTask->GetName();
		m_ulSrc = dlgNum.m_nNum;
		m_strSrc = strName;
		GetDlgItem(IDC_SRC)->SetWindowText(m_strSrc);

		delete pTask;
	}
}

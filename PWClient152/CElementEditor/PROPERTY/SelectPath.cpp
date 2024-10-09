// SelectPath.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SelectPath.h"

/////////////////////////////////////////////////////////////////////////////
// CSelectPath dialog


CSelectPath::CSelectPath(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectPath::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectPath)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelectPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectPath)
	DDX_Text(pDX, IDC_FILENAME, m_filename);
	DDX_Control(pDX, IDC_TREE1, m_pathtree);
	//}}AFX_DATA_MAP
}

void CSelectPath::SetPath(CString path, CString ext)
{
	m_filter = ext;
	m_path = path;
}

CString CSelectPath::GetPathName()
{
	return m_filename;
}

BEGIN_MESSAGE_MAP(CSelectPath, CDialog)
	//{{AFX_MSG_MAP(CSelectPath)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectPath message handlers

static BOOL CALLBACK  FILTER(DWORD param, CString filepath, PATHTREEITEMTYPE type)
{
	CString * str = (CString*)param;
	if(type == PTIT_FILE)
	{
		CString path = filepath;
		CString ext = *str;
		path.MakeUpper();
		ext.MakeUpper();
		int tmp = -1;
		int index;
		do 
		{
			index = tmp;
			tmp = path.Find(ext,index<0?0:index+1);
		}while(tmp != -1);
		path.GetLength();
		if(index > 0 && index + ext.GetLength() == path.GetLength()) return TRUE;
	}
	else
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CSelectPath::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	char curr[MAX_PATH + 1];
	GetCurrentDirectoryA(MAX_PATH, curr);
	m_cur_dir = curr;

	if(m_filter.GetLength())
		m_pathtree.SetPath(m_path,FILTER,(DWORD)&m_filter);
	else
		m_pathtree.SetPath(m_path);
	m_pathtree.EnsureVisiblePath(m_filename,FALSE);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectPath::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: 当选择改变时
	HTREEITEM item = pNMTreeView->itemNew.hItem;
	*pResult = 0;

	PATHTREEITEMTYPE type = (PATHTREEITEMTYPE)m_pathtree.GetItemData(item);
	if(type == PTIT_FILE)
	{
		//当选择了文件时
		m_filename = m_pathtree.GetItemPath(item, FALSE);		//文件名
		UpdateData(FALSE);
	}
}

void CSelectPath::OnDestroy() 
{
	CDialog::OnDestroy();
	
	SetCurrentDirectory(m_cur_dir);
	// TODO: Add your message handler code here
	
}

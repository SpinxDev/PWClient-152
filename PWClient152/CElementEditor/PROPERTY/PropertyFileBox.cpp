// PropertyFileBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AProperty.h"
#include "PropertyFileBox.h"
#include "pagedialog.h"
#include <AString.h>
#include "CommonFileDlg.h"
#include "AFI.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyFileBox

CPropertyFileBox::CPropertyFileBox()
{
}

CPropertyFileBox::~CPropertyFileBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyFileBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyFileBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(BN_CLICKED, 101, OnBnClicked)
	ON_CONTROL(BN_CLICKED, 102, OnEditChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyFileBox message handlers

int CPropertyFileBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: 创建控制
	BOOL bResult = m_wnd_button.Create(
		_T(".."),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(0,0,0,0),
		this,
		101);

	ASSERT(bResult);
	m_wnd_button.SetFont(GetFont());
	m_wnd_button.ModifyStyle(WS_TABSTOP,0);
	m_str_file = (AString)m_ptr_property->GetValue(m_ptr_data);
	if(m_str_file == _T(" "))
		m_str_file = _T("");
	m_str_path = m_ptr_property->GetPath();

	// TODO: 创建控件
	bResult = m_wnd_edit.Create(
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		CRect(0,0,0,0),
		this,
		102);

	ASSERT(bResult);
	m_wnd_edit.SetFont(GetFont());

	UpdateData(FALSE);
	return 0;
}

void CPropertyFileBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	if(pDX->m_bSaveAndValidate)
	{
		m_wnd_edit.GetWindowText(m_str_file);
		DDX_Text(pDX, 102, m_str_file);
		//if(m_str_file.GetLength()==0) 
		//	m_str_file = _T(" ");
		//杨智盈为什么要这样写，还没有搞清除....
		m_ptr_property->SetValue(m_ptr_data, AString(CSafeString(m_str_file)));
		m_wnd_edit.SetModify(FALSE);
	}
	else
	{
		DDX_Text(pDX, 102, m_str_file);
	}

	CPropertyBox::DoDataExchange(pDX);
}

void CPropertyFileBox::DrawSelf(CDC *pDC)
{
	ASSERT(pDC);
	ASSERT(m_ptr_property);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

	CString text = m_str_file;

	CRect rc;
	GetClientRect(rc);

	rc.top += edge_cy;
	rc.left += edge_cx + 1;
	rc.bottom -= edge_cy;

	int iSave = pDC->SaveDC();
	pDC->SelectObject(GetFont());
	pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));

	pDC->DrawText(text, rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	pDC->RestoreDC(iSave);
}

BOOL CPropertyFileBox::IsModified()
{
	return m_wnd_edit.GetModify();
}

void CPropertyFileBox::OnBnClicked()
{
	// 浏览文件，并设置编辑标志
	// 由于路径均为相对路径，此处将路径加上工作目录
	/*
	char curr_path[MAX_PATH + 1];
	GetCurrentDirectoryA(MAX_PATH, curr_path);
	char c = curr_path[strlen(curr_path)-1];
	CString str;
	if(c == '/' || c == '\\')
		str = "";
	else
		str = "\\";

	if(BrowseFile(m_str_file,"",CSafeString(CString(curr_path) + str + m_str_path)))
	{
		m_wnd_edit.SetWindowText(m_str_file);
		m_wnd_edit.SetModify(TRUE);
	}
	*/

	CCommonFileDlg dlg(g_pA3DEngine, m_str_path);
	if (dlg.DoModal() == IDOK)
	{
		m_str_file = dlg.GetRelativeFileName();
		m_wnd_edit.SetWindowText(m_str_file);
		m_wnd_edit.SetModify(TRUE);
	}
}

void CPropertyFileBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	
	// TODO: 将按钮定位在右侧
	m_wnd_edit.MoveWindow(4,4,cx - cy -4,cy - 4);
	m_wnd_button.MoveWindow(
		cx - cy + 1, 1,
		cy - 2, cy - 1);
}

void CPropertyFileBox::OnEditChanged()
{
	m_wnd_edit.SetModify();
}


// PropertyMemoBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropertyMemoBox.h"
#include "AProperty.h"
#include "MemoEditDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyMemoBox

CPropertyMemoBox::CPropertyMemoBox()
{
	m_bool_modified = false;
}

CPropertyMemoBox::~CPropertyMemoBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyMemoBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyMemoBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(BN_CLICKED, 101, OnBnClicked)
	ON_CONTROL(EN_CHANGE, 102, OnEditChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyMemoBox message handlers

void CPropertyMemoBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	if(pDX->m_bSaveAndValidate)
	{
		m_wnd_edit.GetWindowText(m_str_value);
		DDX_Text(pDX, 102, m_str_value);
		m_ptr_property->SetValue(m_ptr_data, AString(CSafeString(m_str_value)));
		m_wnd_edit.SetModify(FALSE);
	}
	else
	{
		DDX_Text(pDX, 102, m_str_value);
	}

	CPropertyBox::DoDataExchange(pDX);
}

int CPropertyMemoBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
	m_str_value = (LPCSTR)(AString)m_ptr_property->GetValue(m_ptr_data);

	// TODO: 创建控件
	bResult = m_wnd_edit.Create(
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL|ES_MULTILINE,
		CRect(0,0,0,0),
		this,
		102);

	ASSERT(bResult);
	m_wnd_edit.SetFont(GetFont());

	UpdateData(FALSE);
	return 0;
}

void CPropertyMemoBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	
	// TODO: 将按钮定位在右侧
	m_wnd_edit.MoveWindow(4,4,cx - cy -4,cy - 4);
	m_wnd_button.MoveWindow(
		cx - cy + 1, 1,
		cy - 2, cy - 1);
}

void CPropertyMemoBox::DrawSelf(CDC *pDC)
{
	ASSERT(pDC);
	ASSERT(m_ptr_property);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

	CString text = m_str_value;

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

void CPropertyMemoBox::OnEditChanged()
{
	m_bool_modified = true;
}

BOOL CPropertyMemoBox::IsModified()
{
	return m_bool_modified;
}

void CPropertyMemoBox::OnBnClicked()
{
	// 编辑Memo
	CMemoEditDlg dlg;
	m_wnd_edit.GetWindowText(dlg.m_Memo);
	if(dlg.DoModal() == IDOK)
	{
		m_str_value = dlg.m_Memo;
		m_wnd_edit.SetWindowText(m_str_value);
		m_wnd_edit.SetModify(TRUE);
	}
}
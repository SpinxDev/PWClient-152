// PropertyVectorBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropertyVectorBox.h"
#include "AObject.h"
#include "CustomVectorDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertyVectorBox

CPropertyVectorBox::CPropertyVectorBox()
{
	m_bool_modified = FALSE;
}

CPropertyVectorBox::~CPropertyVectorBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyVectorBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyVectorBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(BN_CLICKED, 101, OnBnClicked)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyVectorBox message handlers

void CPropertyVectorBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	ASSERT(m_ptr_property);

	if(pDX->m_bSaveAndValidate)
	{
		m_ptr_property->SetValue(m_ptr_data, m_av3_value);
		m_bool_modified = FALSE;
	}
	else
	{
		m_av3_value = m_ptr_property->GetValue(m_ptr_data);
	}

	CPropertyBox::DoDataExchange(pDX);
}

int CPropertyVectorBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: 创建按钮
	BOOL bResult = m_wnd_button.Create(
		_T(".."),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(0,0,0,0),
		this,
		101);

	ASSERT(bResult);
	m_wnd_button.SetFont(GetFont());
	m_wnd_button.ModifyStyle(WS_TABSTOP,0);
	m_av3_value = m_ptr_property->GetValue(m_ptr_data);
	UpdateData(FALSE);
	return 0;
}

void CPropertyVectorBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	
	// TODO: 移动按钮
	m_wnd_button.MoveWindow(
		cx - cy + 1, 1,
		cy - 2, cy - 1);
}

BOOL CPropertyVectorBox::IsModified()
{
	return m_bool_modified;
}

void CPropertyVectorBox::DrawSelf(CDC * pDC)
{
	ASSERT(pDC);
	ASSERT(m_ptr_property);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

	CString text;
	text.Format(_T("X:%-.2f Y:%-.2f Z:%-.2f"), m_av3_value.x, m_av3_value.y, m_av3_value.z);
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

afx_msg void CPropertyVectorBox::OnBnClicked()
{
	CCustomVectorDlg dlg(m_av3_value, this);
	if(dlg.DoModal() == IDOK)
	{
		m_av3_value.x = dlg.m_x;
		m_av3_value.y = dlg.m_y;
		m_av3_value.z = dlg.m_z;
		m_bool_modified = TRUE;
		Invalidate(FALSE);
	}
}

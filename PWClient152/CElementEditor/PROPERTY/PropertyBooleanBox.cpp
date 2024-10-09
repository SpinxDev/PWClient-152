// PropertyBooleanBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropertyBooleanBox.h"
#include "AProperty.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertyBooleanBox

CPropertyBooleanBox::CPropertyBooleanBox()
{
	m_int_sel = -1;
	m_bool_modified = FALSE;
}

CPropertyBooleanBox::~CPropertyBooleanBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyBooleanBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyBooleanBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(CBN_SELCHANGE, 101, OnSelChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyBooleanBox message handlers

int CPropertyBooleanBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: 创建控制并联系数据
	BOOL bResult = m_wnd_combobox.Create(
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
		CRect(0,0,0,100),
		this,
		101);

	m_wnd_combobox.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
	m_wnd_combobox.AddString(_T("True"));
	m_wnd_combobox.AddString(_T("False"));

	ASSERT(bResult);
	ASSERT(m_ptr_property);
	UpdateData(FALSE);
	return 0;
}

void CPropertyBooleanBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 数据交换
	if(pDX->m_bSaveAndValidate)
	{
		DDX_CBIndex(pDX, 101, m_int_sel);
		m_ptr_property->SetValue(m_ptr_data, m_int_sel == 0 ? true : false);
		m_bool_modified = FALSE;
	}
	else
	{
		bool bProperty = m_ptr_property->GetValue(m_ptr_data);
		m_int_sel = bProperty ? 0 : 1;
		DDX_CBIndex(pDX, 101, m_int_sel);
	}
	CPropertyBox::DoDataExchange(pDX);
}

void CPropertyBooleanBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	
	// TODO: 移动窗口
	m_wnd_combobox.MoveWindow(0,0,cx,cy);
}

void CPropertyBooleanBox::DrawSelf(CDC * pDC)
{
	ASSERT(pDC);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

	CString text;
	m_wnd_combobox.GetLBText(m_wnd_combobox.GetCurSel(), text);

	CRect rc;
	GetClientRect(rc);

	rc.top += edge_cy;
	rc.left += edge_cx + 1;
	rc.bottom -= edge_cy;

	int iSave = pDC->SaveDC();
	pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));

	pDC->DrawText(text, rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	pDC->RestoreDC(iSave);
}

BOOL CPropertyBooleanBox::IsModified()
{
	return m_bool_modified;
}

void CPropertyBooleanBox::OnSelChanged()
{
	m_bool_modified = TRUE;
}

// PropertyIntegerBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropertyIntegerBox.h"
#include "AProperty.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyIntegerBox

CPropertyIntegerBox::CPropertyIntegerBox()
{
	m_int_index = -1;
	m_bool_modified = FALSE;
}

CPropertyIntegerBox::~CPropertyIntegerBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyIntegerBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyIntegerBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_CONTROL(CBN_SELCHANGE, 101, OnEditChanged)
	ON_CONTROL(EN_CHANGE, 101, OnEditChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyIntegerBox message handlers

int CPropertyIntegerBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT(m_ptr_property);	
	ASet * pSet = m_ptr_property->GetPropertySet();
	
	if(pSet)
	{
		// 使用列表控件
		int height = (pSet->GetCount() + 1) * 20;
		BOOL bResult = m_wnd_list.Create(
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
			CRect(0,0,0,height > 250 ? 250 : height),
			this,
			101);
		ASSERT(bResult);

		m_wnd_list.SetFont(GetFont());

		int count = pSet->GetCount();
		for(int a = 0;a < count;a ++)
		{
			int value = pSet->GetValueByIndex(a);
			CString name = pSet->GetNameByIndex(a);
			CString text;
			text.Format(_T("%d(%s)"), value, name);
			int item = m_wnd_list.AddString(text);
			m_wnd_list.SetItemData(item, value);
		}
	}
	else
	{
		// 使用编辑控件
		BOOL bResult = m_wnd_edit.Create(
			WS_CHILD | WS_VISIBLE,
			CRect(0,0,0,0),
			this,
			101);
		ASSERT(bResult);
		m_wnd_edit.SetFont(GetFont());
	}

	UpdateData(FALSE);
	return 0;
}

void CPropertyIntegerBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	
	// TODO: 移动编辑框
	if(IsWindow(m_wnd_edit.GetSafeHwnd()))
	{
		m_wnd_edit.MoveWindow(4,4,cx - 4,cy - 4);
	}

	if(IsWindow(m_wnd_list.GetSafeHwnd()))
	{
		m_wnd_list.MoveWindow(0,0,cx,cy);
	}
}

void CPropertyIntegerBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 数据交换
	if(IsWindow(m_wnd_edit.GetSafeHwnd()))
	{
		if(pDX->m_bSaveAndValidate)
		{
			int value;
			DDX_Text(pDX, 101, value);
			m_ptr_property->SetValue(m_ptr_data, value);
			m_bool_modified = FALSE;
		}
		else
		{
			int value = m_ptr_property->GetValue(m_ptr_data);
			DDX_Text(pDX, 101, value);
		}
	}

	if(IsWindow(m_wnd_list.GetSafeHwnd()))
	{
		if(pDX->m_bSaveAndValidate)
		{
			// 将索引转换成值
			DDX_CBIndex(pDX, 101, m_int_index);
			if(m_int_index != -1)
			{
				int value = m_wnd_list.GetItemData(m_int_index);
				m_ptr_property->SetValue(m_ptr_data, value);
				m_bool_modified = FALSE;
			}
		}
		else
		{
			ASet * pSet = m_ptr_property->GetPropertySet();
			ASSERT(pSet);
			m_int_index = pSet->FindValue(m_ptr_property->GetValue(m_ptr_data));
//			if(m_int_index == -1) m_int_index = 0;
			DDX_CBIndex(pDX, 101, m_int_index);
		}
	}

	CPropertyBox::DoDataExchange(pDX);
}

void CPropertyIntegerBox::OnEditChanged()
{
	m_bool_modified = TRUE;
}

void CPropertyIntegerBox::DrawSelf(CDC *pDC)
{
	UpdateData(FALSE);
	CString text;
	if(IsWindow(m_wnd_edit.GetSafeHwnd())) m_wnd_edit.GetWindowText(text);
	if(IsWindow(m_wnd_list.GetSafeHwnd())) m_wnd_list.GetWindowText(text);

	ASSERT(pDC);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

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

BOOL CPropertyIntegerBox::IsModified()
{
	return m_bool_modified;
}

void CPropertyIntegerBox::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
	// TODO: 显示窗口时自动将焦点置向第一个字窗口
	if(bShow)
	{
		UpdateData(FALSE);
		ASet * pSet = m_ptr_property->GetPropertySet();
		if(!pSet)
		{
			m_wnd_edit.SetFocus();
			m_wnd_edit.SetSel(0,-1);
		}else
		{
			CWnd * pChild = GetWindow(GW_CHILD);
			if(pChild)
			{
				pChild->SetFocus();
			}
		}
	
	}
	else
	{
		if(IsModified())
		{
			UpdateData(TRUE);
		}
	}
}
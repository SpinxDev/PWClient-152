// PropertyIDBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropertyIDBox.h"
#include "AObject.h"
#include "ac.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyIDBox

CPropertyIDBox::CPropertyIDBox(CString type)
: m_str_type(type)
{
	m_bool_modified = FALSE;
}

CPropertyIDBox::~CPropertyIDBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyIDBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyIDBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(CBN_SELCHANGE, 101, OnSelChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyIDBox message handlers

void CPropertyIDBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	if(pDX->m_bSaveAndValidate)
	{
		// 将索引转换成值
		DDX_CBString(pDX, 101, m_str_object);
//		int value = m_wnd_list.GetItemData(m_int_index);
		m_ptr_property->SetValue(m_ptr_data, AString(CSafeString(m_str_object)));
		m_bool_modified = FALSE;
	}
	else
	{
//		DDX_CBString(pDX, 101, m_str_object);
/*		
		int item;
		for(item = 0;item < m_wnd_list.GetCount();item ++)
		{
			char buf[1024];
			m_wnd_list.GetLBText(item,buf);
			if(str == buf) 
			{
				m_wnd_list.SetCurSel(item);
			}
		}
		if(item == m_wnd_list.GetCount())
		{
			//m_wnd_list.SetWindowText(str);
			DDX_CBString(pDX, 101, CString(str));
		}*/
		
	}
	CPropertyBox::DoDataExchange(pDX);
}

int CPropertyIDBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: 列出场景中所有的对象

	BOOL bResult = m_wnd_list.Create(
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL | CBS_SORT ,
		CRect(0,0,0, 250),
		this,
		101);
	ASSERT(bResult);

	m_wnd_list.SetFont(GetFont());
	
	// 循环
	//m_wnd_list.AddString(pData->GetName());
	//
	

	AString str =  m_ptr_property->GetValue(m_ptr_data);
	m_str_object = str;
	m_wnd_list.SetWindowText(CSafeString(str));
	//m_wnd_list.

	UpdateData(FALSE);
	return 0;
}

void CPropertyIDBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	// TODO: 移动大小
	if(IsWindow(m_wnd_list.GetSafeHwnd()))
	{
		m_wnd_list.MoveWindow(0,0,cx,cy);
	}
}

void CPropertyIDBox::DrawSelf(CDC * pDC)
{
//	UpdateData(FALSE);
	CString text;
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

BOOL CPropertyIDBox::IsModified()
{
	return m_bool_modified;
}

void CPropertyIDBox::OnSelChanged()
{
	m_bool_modified = TRUE;
}

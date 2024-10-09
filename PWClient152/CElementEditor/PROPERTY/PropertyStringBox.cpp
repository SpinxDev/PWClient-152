// PropertyStringBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropertyStringBox.h"
#include "AProperty.h"
#include <ac.h>

/////////////////////////////////////////////////////////////////////////////
// CPropertyStringBox

CPropertyStringBox::CPropertyStringBox()
{
	m_bool_modified = false;
}

CPropertyStringBox::~CPropertyStringBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyStringBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyStringBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(CBN_SELCHANGE, 101, OnEditChanged)
	ON_CONTROL(EN_CHANGE, 101, OnEditChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyStringBox message handlers

void CPropertyStringBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	if(IsWindow(m_wnd_edit.GetSafeHwnd()))
	{
		if(pDX->m_bSaveAndValidate)
		{
			DDX_Text(pDX, 101, m_str_value);
			m_ptr_property->SetValue(m_ptr_data, AString(CSafeString(m_str_value)));
			m_wnd_edit.SetModify(FALSE);
		}
		else
		{
			m_str_value = (AString)m_ptr_property->GetValue(m_ptr_data);
			DDX_Text(pDX, 101, m_str_value);
		}
	}
		
	if(IsWindow(m_wnd_list.GetSafeHwnd()))
	{
		if(pDX->m_bSaveAndValidate)
		{
			// 将索引转换成值
			int index;
			DDX_CBIndex(pDX, 101, index);
			if(index!= -1)
			{
				index = m_wnd_list.GetItemData(index);
				ASet * pSet = m_ptr_property->GetPropertySet();
				AVariant var = pSet->GetValueByIndex(index);
				AString value = var;
				m_ptr_property->SetValue(m_ptr_data, value);
				m_bool_modified = FALSE;
				
			}
		}
		else
		{
			ASet * pSet = m_ptr_property->GetPropertySet();
			ASSERT(pSet);
			int index  = pSet->FindValue(m_ptr_property->GetValue(m_ptr_data));
			DDX_CBIndex(pDX, 101, index);
		}		
	}

	CPropertyBox::DoDataExchange(pDX);
}

int CPropertyStringBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
			//AString value = pSet->GetValueByIndex(a);
			CString name = pSet->GetNameByIndex(a);
			CString text;
			//text.Format("%d(%s)", value, name);
			text.Format(_T("%s"), name);
			int item = m_wnd_list.AddString(text);
			m_wnd_list.SetItemData(item, a);
		}
	}
	else
	{
		// 使用编辑控件
		BOOL bResult = m_wnd_edit.Create(
			WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
			CRect(0,0,0,0),
			this,
			101);
		ASSERT(bResult);
		m_wnd_edit.SetFont(GetFont());
	}

	UpdateData(FALSE);
	return 0;
}

void CPropertyStringBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	
	// TODO: 移动控制
	if(IsWindow(m_wnd_edit.GetSafeHwnd()))
	{
		m_wnd_edit.MoveWindow(4,4,cx - 4,cy - 4);
	}

	if(IsWindow(m_wnd_list.GetSafeHwnd()))
	{
		m_wnd_list.MoveWindow(0,0,cx,cy);
	}
}

void CPropertyStringBox::DrawSelf(CDC *pDC)
{
	ASSERT(pDC);

	CString text;
	if(IsWindow(m_wnd_edit.GetSafeHwnd())) m_wnd_edit.GetWindowText(text);
	if(IsWindow(m_wnd_list.GetSafeHwnd())) m_wnd_list.GetWindowText(text);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

	CRect rc;
	GetClientRect(rc);

	rc.top += edge_cy + 1;
	rc.left += edge_cx + 1;
	rc.bottom -= edge_cy;
	rc.right -= 5;

	int iSave = pDC->SaveDC();
	pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));

	pDC->DrawText(text, rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	pDC->RestoreDC(iSave);
}

void CPropertyStringBox::OnEditChanged()
{
	m_bool_modified = TRUE;	
}

BOOL CPropertyStringBox::IsModified()
{
	return m_bool_modified;
}
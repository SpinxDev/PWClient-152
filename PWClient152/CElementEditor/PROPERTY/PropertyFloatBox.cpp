// PropertyFloatBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropertyFloatBox.h"
#include "AProperty.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyFloatBox

CPropertyFloatBox::CPropertyFloatBox()
{
}

CPropertyFloatBox::~CPropertyFloatBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyFloatBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyFloatBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_CONTROL(EN_CHANGE, 101, OnEditChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyFloatBox message handlers

int CPropertyFloatBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: 创建控制
	BOOL bResult = m_wnd_edit.Create(
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		CRect(0,0,0,0),
		this,
		101);

	ASSERT(bResult);
	m_wnd_edit.SetFont(GetFont());
	UpdateData(FALSE);

	return 0;
}

void CPropertyFloatBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	if(pDX->m_bSaveAndValidate)
	{
		double value;
		DDX_Text(pDX, 101, value);
		m_ptr_property->SetValue(m_ptr_data, value);
		m_wnd_edit.SetModify(FALSE);
	}
	else
	{
		double value = m_ptr_property->GetValue(m_ptr_data);
		DDX_Text(pDX, 101, value);
	}

	CPropertyBox::DoDataExchange(pDX);
}

BOOL CPropertyFloatBox::IsModified()
{
	return m_wnd_edit.GetModify();
}

void CPropertyFloatBox::OnEditChanged()
{
	m_wnd_edit.SetModify(TRUE);
}

void CPropertyFloatBox::DrawSelf(CDC *pDC)
{
	UpdateData(FALSE);
	CString text;
	m_wnd_edit.GetWindowText(text);
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

void CPropertyFloatBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	
	// TODO: 移动控制
	m_wnd_edit.MoveWindow(4,4,cx - 4,cy - 4);
}

void CPropertyFloatBox::OnShowWindow(BOOL bShow, UINT nStatus) 
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

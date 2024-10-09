// PropertyCustomBox.cpp: implementation of the CPropertyCustomBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PropertyCustomBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPropertyCustomBox::CPropertyCustomBox() : m_bModified(FALSE), m_pCustomFuncs(0)
{
}

CPropertyCustomBox::~CPropertyCustomBox()
{
}

BEGIN_MESSAGE_MAP(CPropertyCustomBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyCustomBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(BN_CLICKED, 101, OnBnClicked)
END_MESSAGE_MAP()

void CPropertyCustomBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	ASSERT(m_ptr_property);

	if(pDX->m_bSaveAndValidate)
	{
	}
	else
	{
	}

	CPropertyBox::DoDataExchange(pDX);
}

int CPropertyCustomBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: 创建按钮
	BOOL bResult = m_wndButton.Create(
		_T(".."),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(0,0,0,0),
		this,
		101);

	ASSERT(bResult);
	m_wndButton.SetFont(GetFont());
	m_wndButton.ModifyStyle(WS_TABSTOP,0);

	m_pCustomFuncs = (CUSTOM_FUNCS*)m_ptr_property->GetPropertySet();
	ASSERT(m_pCustomFuncs);
	m_strText = m_pCustomFuncs->OnGetShowString();

	UpdateData(FALSE);
	return 0;
}

void CPropertyCustomBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	
	m_wndButton.MoveWindow(
		cx - cy + 1, 1,
		cy - 2, cy - 1);
}

BOOL CPropertyCustomBox::IsModified()
{
	return m_bModified;
}

void CPropertyCustomBox::DrawSelf(CDC * pDC)
{
	ASSERT(pDC);
	ASSERT(m_ptr_property);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

	CRect rc;
	GetClientRect(rc);

	rc.top += edge_cy;
	rc.left += edge_cx + 1;
	rc.bottom -= edge_cy;

	int iSave = pDC->SaveDC();
	pDC->SelectObject(GetFont());
	pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
	pDC->FillSolidRect(&rc, GetSysColor(COLOR_WINDOW));
	pDC->DrawText(m_strText, rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	pDC->RestoreDC(iSave);
}

void CPropertyCustomBox::OnBnClicked()
{
	
	if (m_pCustomFuncs->OnActivate())
	{
		m_strText = m_pCustomFuncs->OnGetShowString();
		m_ptr_property->SetValue(m_ptr_data, m_pCustomFuncs->OnGetValue());
		m_bModified = TRUE;
		Invalidate();
	}
	else
		m_bModified = FALSE;
	
}

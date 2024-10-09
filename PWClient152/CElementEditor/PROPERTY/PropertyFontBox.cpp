// PropertyFontBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropertyFontBox.h"
#include "AObject.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertyFontBox

CPropertyFontBox::CPropertyFontBox()
{
	m_bool_modified = FALSE;
}

CPropertyFontBox::~CPropertyFontBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyFontBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyFontBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(BN_CLICKED, 101, OnBnClicked)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyFontBox message handlers

void CPropertyFontBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	ASSERT(m_ptr_property);

	if(pDX->m_bSaveAndValidate)
	{
		m_ptr_property->SetValue(m_ptr_data, m_font);
		m_bool_modified = FALSE;
	}
	else
	{
		m_font = m_ptr_property->GetValue(m_ptr_data);
	}

	CPropertyBox::DoDataExchange(pDX);
}

int CPropertyFontBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
	m_font = m_ptr_property->GetValue(m_ptr_data);
	UpdateData(FALSE);
	return 0;
}

void CPropertyFontBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	
	// TODO: 移动按钮
	m_wnd_button.MoveWindow(
		cx - cy + 1, 1,
		cy - 2, cy - 1);
}

BOOL CPropertyFontBox::IsModified()
{
	return m_bool_modified;
}

void CPropertyFontBox::DrawSelf(CDC * pDC)
{
	ASSERT(pDC);
	ASSERT(m_ptr_property);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

	CString text;
	text.Format(_T("%s(%d)"), m_font.szName, m_font.iHeight);
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

afx_msg void CPropertyFontBox::OnBnClicked()
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfCharSet = GB2312_CHARSET;
#ifdef _UNICODE
	wcscpy(lf.lfFaceName, m_font.szName);
#else
	strcpy(lf.lfFaceName, m_font.szName);
#endif
	CClientDC dc(this);
	lf.lfHeight = -MulDiv(m_font.iHeight, dc.GetDeviceCaps(LOGPIXELSY), 72);
	CFontDialog dlg(&lf, CF_SCREENFONTS);
	if (dlg.DoModal() == IDOK)
	{
#ifdef _UNICODE
		wcscpy(m_font.szName, dlg.m_lf.lfFaceName);
#else
		strcpy(m_font.szName, dlg.m_lf.lfFaceName);
#endif
		m_font.iHeight = dlg.m_cf.iPointSize / 10;
		m_bool_modified = TRUE;
		Invalidate(FALSE);
	}
}

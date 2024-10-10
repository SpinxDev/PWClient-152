#include "StdAfx.h"
#include "WndLabel.h"

CWndLabel::CWndLabel(void)
{
}

CWndLabel::~CWndLabel(void)
{
}

BEGIN_MESSAGE_MAP(CWndLabel, CWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int CWndLabel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_wndIcon.Create(NULL, WS_CHILD|WS_VISIBLE|SS_ICON|SS_CENTERIMAGE, CRect(0,0,0,0), this , 1))
		return -1;

	return 0;
}

void CWndLabel::DrawRealize(CDC* pDC)
{	
	CRect rc;
	rc.left = 50;
	rc.right = 400;
	rc.top = 10;
	rc.bottom = 30;
	DrawText(pDC, rc, m_csLabel, RGB(255,255,255));
}

void CWndLabel::EndDraw()
{
	if(m_wndIcon.GetIcon())
		m_wndIcon.ShowWindow(SW_SHOWNORMAL);
	else
		m_wndIcon.ShowWindow(SW_HIDE);
	m_wndIcon.RedrawWindow();
}

void CWndLabel::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
	m_wndIcon.SetWindowPos(NULL, 10, 10, 24, 24, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CWndLabel::SetLabel(const CString& csLabel, UINT uiIconID)
{
	HICON hIcon = AfxGetApp()->LoadIcon(uiIconID);
	m_wndIcon.SetIcon(hIcon);
	m_csLabel = csLabel;
	Update();
}
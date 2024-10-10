#include "StdAfx.h"
#include "BounceButton.h"

CBounceButton::CBounceButton(UINT uiDownResID, UINT uiUpResID)
{
	m_bDown = false;
	m_uiDownResID = uiDownResID;
	m_uiUpResID = uiUpResID;
	m_iWidth = 24;
	m_iHeight = 21;
}

CBounceButton::~CBounceButton(void)
{
	if(m_bmpDown.m_hObject)
		m_bmpDown.DeleteObject();
	if(m_bmpUp.m_hObject)
		m_bmpUp.DeleteObject();
}
BEGIN_MESSAGE_MAP(CBounceButton, CButton)
END_MESSAGE_MAP()

void CBounceButton::SetButtonState(bool bDown)
{
	m_bDown = bDown;
	RedrawWindow();
}

void CBounceButton::PreSubclassWindow()
{
	m_bmpDown.LoadBitmap(m_uiDownResID);
	m_bmpUp.LoadBitmap(m_uiUpResID);
	CDC* pDC = GetDC();
	memdc.CreateCompatibleDC(pDC);
	memdc.SelectObject(&m_bmpDown);
	memdc2.CreateCompatibleDC(pDC);
	memdc2.SelectObject(&m_bmpUp);

	BITMAP bmp;
	GetObject((HBITMAP)m_bmpDown, sizeof(BITMAP), (PSTR)&bmp);
	m_iWidth = bmp.bmWidth;
	m_iHeight = bmp.bmHeight;
	CButton::PreSubclassWindow();
}

void CBounceButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CRect rtClient = lpDIS->rcItem;
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	DWORD state = lpDIS->itemState;
	CBrush brush;
	brush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
	pDC->FillRect(rtClient, &brush);
	if(m_bDown)
	{
		pDC->StretchBlt(rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), &memdc, 0, 0, m_iWidth, m_iHeight, SRCCOPY);
		GetParent()->SetWindowPos(&CWnd::wndTopMost,0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
	}
	else
	{
		pDC->StretchBlt(rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), &memdc2, 0, 0, m_iWidth, m_iHeight, SRCCOPY);
		GetParent()->SetWindowPos(&CWnd::wndNoTopMost,0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
	}
}
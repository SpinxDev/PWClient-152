#include "StdAfx.h"
#include "WndBase.h"

#include "WndDefine.h"

CFont		CWndBase::m_Font;
CBrush		CWndBase::m_Brush;
COLORREF	CWndBase::m_crBkColor = WND_SYS_BKCOLOR;

bool CWndBase::Init()
{
	if(!m_Font.CreateFont(15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_ROMAN, NULL))
		return false;
	return true;
}

void CWndBase::Release()
{
	m_Brush.DeleteObject();
	m_Font.DeleteObject();
}

CWndBase::CWndBase(void)
{
	m_clBK = WND_SYS_BKCOLOR;
}

CWndBase::~CWndBase(void)
{
}
BEGIN_MESSAGE_MAP(CWndBase, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

int CWndBase::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	if(!m_Brush.m_hObject)
		m_Brush.CreateSolidBrush(m_crBkColor);

	return 0;
}

BOOL CWndBase::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
}

void CWndBase::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()

	Draw(&dc);
}

void CWndBase::Draw(CDC* pDC /*= NULL*/)
{
	if(!pDC)
		pDC = GetDC();
	if(!pDC)
		return;

	CRect rtClient;
	GetClientRect(&rtClient);
	CRect rtPaint(0, 0, rtClient.Width(), rtClient.Height());
	CBitmap ShowBitmap;
	ShowBitmap.CreateCompatibleBitmap(pDC, rtPaint.Width(), rtPaint.Height());
	if(!ShowBitmap.m_hObject)
	{
		ReleaseDC(pDC);
		return;
	}
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	if(!memDC.m_hDC)
	{
		ShowBitmap.DeleteObject();
		ReleaseDC(pDC);
		return;
	}

	CBitmap* pOldBitmap = memDC.SelectObject(&ShowBitmap);
	memDC.FillSolidRect(rtPaint, m_clBK);
	CRect rcEdge = rtClient;
	rcEdge.right -= 1;
	rcEdge.bottom -= 1;
	DrawRect(&memDC, rcEdge, 0);

	DrawRealize(&memDC);
	

	StretchBlt(pDC->GetSafeHdc(),
		rtClient.left,
		rtClient.top,
		rtClient.Width(),
		rtClient.Height(),
		memDC.GetSafeHdc(),
		0,
		0,
		rtPaint.Width(),
		rtPaint.Height(),
		SRCCOPY);	

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	ShowBitmap.DeleteObject();
	ReleaseDC(pDC);

	EndDraw();
}

void CWndBase::DrawLine(CDC* pDC, int iSX, int iSY, int iEX, int iEY, COLORREF cl, int iWidth /*= 1*/)
{
	ASSERT(pDC != NULL);

	CPen pen(PS_SOLID, iWidth, cl);
	CPen *oldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(iSX, iSY);
	pDC->LineTo(iEX, iEY);
	pDC->SelectObject(oldPen);
}

void CWndBase::DrawLine(CDC* pDC, CPoint ptS, CPoint ptE, COLORREF cl, int iWidth /*= 1*/)
{
	DrawLine(pDC, ptS.x, ptS.y, ptE.x, ptE.y, cl, iWidth);
}

void CWndBase::DrawRect(CDC* pDC, int iLTX, int iLTY, int iRBX, int iRBY, COLORREF cl, int iWidth /*= 1*/)
{
	ASSERT(pDC != NULL);

	CPen pen(PS_SOLID, iWidth, cl);
	CPen *oldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(iLTX, iLTY);
	pDC->LineTo(iRBX, iLTY);
	pDC->LineTo(iRBX, iRBY);
	pDC->LineTo(iLTX, iRBY);
	pDC->LineTo(iLTX, iLTY);
	pDC->SelectObject(oldPen);
}

void CWndBase::DrawRect(CDC* pDC, CPoint ptLT, CPoint ptRB, COLORREF cl, int iWidth /*= 1*/)
{
	DrawRect(pDC, ptLT.x, ptLT.y, ptRB.x, ptRB.y, cl, iWidth);
}

void CWndBase::DrawRect(CDC* pDC, CRect rtRect, COLORREF cl, int iWidth /*= 1*/)
{
	DrawRect(pDC, rtRect.left, rtRect.top, rtRect.right, rtRect.bottom, cl, iWidth);
}

void CWndBase::DrawText(CDC* pDC, CRect rtText, CString strText, COLORREF cl, DWORD dwAlign /*= DT_LEFT*/)
{
	ASSERT(pDC != NULL);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(cl);
	CFont* oldFont = pDC->SelectObject(&m_Font);
	pDC->DrawText(strText, &rtText, DT_SINGLELINE | dwAlign | DT_VCENTER);
	pDC->SelectObject(oldFont);
}

BOOL CWndBase::Create(CWnd* pParentWnd, const RECT& rect)
{
	return CWnd::Create(NULL,
		_T("WndBase"),
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, rect,
		pParentWnd,
		NULL,
		NULL);
}

void CWndBase::Update()
{
	Draw(NULL);
}

void CWndBase::SetBKColor(COLORREF cl)
{
	m_clBK = cl;
	Draw(NULL);
}
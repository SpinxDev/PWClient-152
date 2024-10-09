// HeightMapView.cpp : implementation file
//

#include "Global.h"
#include "Terrain.h"
#include "HeightMapView.h"
#include "Bitmap.h"
#include "AC.h"

#include "afxpriv.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CHeightMapView

CHeightMapView::CHeightMapView()
{
	m_pTerrain		= NULL;
	m_iViewWid		= 0;
	m_iViewHei		= 0;
	m_iOffsetX		= 0;
	m_iOffsetY		= 0;
	m_bViewWhole	= false;
	m_pBitmap		= NULL;
}

CHeightMapView::~CHeightMapView()
{
}


BEGIN_MESSAGE_MAP(CHeightMapView, CWnd)
	//{{AFX_MSG_MAP(CHeightMapView)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHeightMapView message handlers

//	Create window
bool CHeightMapView::Create(CTerrain* pTerrain, const RECT& rc, CWnd* pParent, DWORD dwID)
{
	//	Register class name
	CString strClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
							LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)),
							(HBRUSH)GetStockObject(WHITE_BRUSH));
	
	DWORD dwWndStyles = WS_BORDER | WS_HSCROLL | WS_VSCROLL | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
	if (!CWnd::CreateEx(0, strClassName, "HeightMapView", dwWndStyles, rc, pParent, dwID, NULL))
		return false;

	ShowScrollBar(SB_BOTH, TRUE);

	m_pTerrain = pTerrain;

	return true;
}

void CHeightMapView::OnDestroy() 
{
	ReleaseHeightMapBmp();

	CWnd::OnDestroy();
}

//	Resize window
void CHeightMapView::Resize(int x, int y, int iClientWid, int iClientHei)
{
	DWORD dwStyles = ::GetWindowLong(m_hWnd, GWL_STYLE);

	m_iViewWid	= iClientWid;
	m_iViewHei	= iClientHei;

	iClientWid += GetSystemMetrics(SM_CXHSCROLL);
	iClientHei += GetSystemMetrics(SM_CYHSCROLL);

	RECT rcWnd = {0, 0, iClientWid, iClientHei};
	AdjustWindowRect(&rcWnd, dwStyles, FALSE);
	OffsetRect(&rcWnd, x, y);
	MoveWindow(&rcWnd);

	//	Reset scroll bar
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();
	if (pHeightMap->pHeightData)
		ResetScrollBar(pHeightMap->iWidth, pHeightMap->iHeight);
}

BOOL CHeightMapView::OnEraseBkgnd(CDC* pDC) 
{
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();

	if (pHeightMap->pHeightData)
		return TRUE;
	else
		return CWnd::OnEraseBkgnd(pDC);
}

//	Paint window
void CHeightMapView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (!m_pTerrain || !m_pBitmap)
		return;

	HDC hMemDC = ::CreateCompatibleDC(dc.m_hDC);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, m_pBitmap->GetBitmap());

	if (m_bViewWhole)
	{
		int iStretchMode = ::SetStretchBltMode(dc.m_hDC, HALFTONE);
		::StretchBlt(dc.m_hDC, 0, 0, m_iViewWid, m_iViewHei, hMemDC, 0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight(), SRCCOPY);
		::SetStretchBltMode(dc.m_hDC, iStretchMode);

		//	Do this as SDK said
		::SetBrushOrgEx(dc.m_hDC, 0, 0, NULL);
	}
	else
		::BitBlt(dc.m_hDC, 0, 0, m_iViewWid, m_iViewHei, hMemDC, m_iOffsetX, m_iOffsetY, SRCCOPY);

	::SelectObject(hMemDC, hOldBmp);
	::DeleteDC(hMemDC);
}

//	Load height map from file
bool CHeightMapView::LoadHeightMap(int iWid, int iHei, int iType, const char* szFile)
{
	if (!m_pTerrain->LoadHeightMap(iWid, iHei, iType, szFile, true))
	{
		g_Log.Log("Failed to load height map %s", szFile);
		return false;
	}

	if (!CreateHeightMapBmp())
		return false;

	//	Reset scroll bar
	ResetScrollBar(iWid, iHei);

	//	Force to update map view
	InvalidateRect(NULL);

	return true;
}

//	Create height map bitmap
bool CHeightMapView::CreateHeightMapBmp()
{
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();
	if (!pHeightMap->pHeightData)
		return false;

	//	Release current bitmap
	ReleaseHeightMapBmp();

	if (!(m_pBitmap = new CELBitmap))
		return false;

	if (!m_pBitmap->CreateBitmap(pHeightMap->iWidth, pHeightMap->iHeight, 8, NULL))
	{
		g_Log.Log("CHeightMapView::CreateHeightMapBmp, Failed to create bitmap");
		return false;
	}

	//	Fill bitmap data
	BYTE* pBmpLine = m_pBitmap->GetBitmapData();
	float* pHeight = pHeightMap->pHeightData;

	for (int i=0; i < pHeightMap->iHeight; i++)
	{
		BYTE* pPixel = pBmpLine;
		pBmpLine += m_pBitmap->GetPitch();

		for (int j=0; j < pHeightMap->iWidth; j++)
		{
			*pPixel = (BYTE)(*pHeight * 255.0f);

			pPixel++;
			pHeight++;
		}
	}

	return true;
}

void CHeightMapView::SetHeightMap(CELBitmap * pBitmap)
{
	
	m_pBitmap = pBitmap;
	int iWid = m_pBitmap->GetWidth();;
	int iHei = m_pBitmap->GetHeight();
	//	Reset scroll bar
	ResetScrollBar(iWid, iHei);

	//	Force to update map view
	InvalidateRect(NULL);
}

//	Release height map bitmap
void CHeightMapView::ReleaseHeightMapBmp()
{
	if (m_pBitmap)
	{
		m_pBitmap->Release();
		delete m_pBitmap;
		m_pBitmap = NULL;
	}
}

//	Reset scroll bar
void CHeightMapView::ResetScrollBar(int iMapWid, int iMapHei)
{
	if (iMapWid <= m_iViewWid)
		EnableScrollBar(SB_HORZ, ESB_DISABLE_BOTH);
	else
	{
		SCROLLINFO Info;
		Info.cbSize		= sizeof (Info);
		Info.fMask		= SIF_ALL;
		Info.nMin		= 0;
		Info.nMax		= iMapWid;
		Info.nPos		= 0;
		Info.nPage		= m_iViewWid;
		Info.nTrackPos	= 0;

		SetScrollInfo(SB_HORZ, &Info);
	}

	if (iMapHei <= m_iViewHei)
		EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
	else
	{
		SCROLLINFO Info;
		Info.cbSize		= sizeof (Info);
		Info.fMask		= SIF_ALL;
		Info.nMin		= 0;
		Info.nMax		= iMapHei;
		Info.nPos		= 0;
		Info.nPage		= m_iViewHei;
		Info.nTrackPos	= 0;

		SetScrollInfo(SB_VERT, &Info);
	}

	m_iOffsetX	= 0;
	m_iOffsetY	= 0;
}

void CHeightMapView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();

	switch (nSBCode)
	{
	case SB_BOTTOM:		m_iOffsetY = pHeightMap->iHeight - m_iViewHei;		break;
	case SB_TOP:		m_iOffsetY = 0;	break;
	case SB_LINEDOWN:	m_iOffsetY++;	break;
	case SB_LINEUP:		m_iOffsetY--;	break;
	case SB_PAGEDOWN:	m_iOffsetY += m_iViewHei;	break;
	case SB_PAGEUP:		m_iOffsetY -= m_iViewHei;	break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:

		m_iOffsetY = nPos;
		break;
	}

	a_Clamp(m_iOffsetY, 0, pHeightMap->iHeight - m_iViewHei);
	SetScrollPos(SB_VERT, m_iOffsetY);

	InvalidateRect(NULL);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CHeightMapView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();

	switch (nSBCode)
	{
	case SB_BOTTOM:		m_iOffsetX = pHeightMap->iWidth - m_iViewWid;	break;
	case SB_TOP:		m_iOffsetX = 0;	break;
	case SB_LINEDOWN:	m_iOffsetX++;	break;
	case SB_LINEUP:		m_iOffsetX--;	break;
	case SB_PAGEDOWN:	m_iOffsetX += m_iViewWid;	break;
	case SB_PAGEUP:		m_iOffsetX -= m_iViewWid;	break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:

		m_iOffsetX = nPos;
		break;
	}

	a_Clamp(m_iOffsetX, 0, pHeightMap->iWidth - m_iViewWid);
	SetScrollPos(SB_HORZ, m_iOffsetX);

	InvalidateRect(NULL);
	
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

//	View whole height map or not
void CHeightMapView::ViewWhole(bool bWhole)
{
	m_bViewWhole = bWhole;

	if (m_bViewWhole)
	{
		//	Disable scroll bars
		EnableScrollBar(SB_HORZ, ESB_DISABLE_BOTH);
		EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
	}
	else
	{
		//	Enable scroll bars
		EnableScrollBar(SB_HORZ, ESB_ENABLE_BOTH);
		EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH);
	}

	//	Force to update view
	InvalidateRect(NULL);

	CWnd* pParent = GetParent();
	AfxCallWndProc(pParent, pParent->m_hWnd,
			WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
	pParent->SendMessageToDescendants(WM_IDLEUPDATECMDUI,
			(WPARAM)TRUE, 0, TRUE, TRUE);
}


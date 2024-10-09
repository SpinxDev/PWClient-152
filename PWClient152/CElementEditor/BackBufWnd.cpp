/*
 * FILE: BackBufWnd.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#include "BackBufWnd.h"
#include "AMemory.h"

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CBackBufWnd
//	
///////////////////////////////////////////////////////////////////////////

CBackBufWnd::CBackBufWnd()
{
	m_hMemDC	= NULL;
	m_hBackBmp	= NULL;
	m_hOldBmp	= NULL;
	m_lpBackBuf	= NULL;
}

CBackBufWnd::~CBackBufWnd()
{
}

BEGIN_MESSAGE_MAP(CBackBufWnd, CWnd)
	//{{AFX_MSG_MAP(CBackBufWnd)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CBackBufWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (lpCreateStruct->cx > 0 && lpCreateStruct->cy > 0)
	{
		//	Destory previous back buffer
		ReleaseBackBuffer();
		
		//	Create new back buffer
		CreateBackBuffer();
	}
	
	return 0;
}

void CBackBufWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	//	Destory previous back buffer
	ReleaseBackBuffer();
		
	//	Create new back buffer
	CreateBackBuffer();
}

void CBackBufWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	
	ReleaseBackBuffer();	
}

//	Create back buffer
bool CBackBufWnd::CreateBackBuffer()
{
	BITMAPINFO* pbi = (BITMAPINFO*)a_malloc(sizeof (BITMAPINFO) - 4);
	if (!pbi)
	{
		TRACE("CBackBufWnd::CreateBackBuffer, not enough memory !");
		return false;
	}

	RECT rc;
	GetClientRect(&rc);	
	ASSERT(rc.bottom && rc.right);

	pbi->bmiHeader.biSize			= sizeof (BITMAPINFOHEADER);
	pbi->bmiHeader.biWidth			= rc.right;
	pbi->bmiHeader.biHeight			= -rc.bottom;
	pbi->bmiHeader.biPlanes			= 1;
	pbi->bmiHeader.biBitCount		= 32;
	pbi->bmiHeader.biCompression	= BI_RGB;
	pbi->bmiHeader.biSizeImage		= 0;
	pbi->bmiHeader.biXPelsPerMeter	= 0;
	pbi->bmiHeader.biYPelsPerMeter	= 0;
	pbi->bmiHeader.biClrUsed		= 0;
	pbi->bmiHeader.biClrImportant	= 0;

	if (!(m_hMemDC = ::CreateCompatibleDC(NULL)))
	{
		a_free(pbi);
		TRACE("CBackBufWnd::CreateBackBuffer, Failed to call CreateCompatibleDC() !");
		return false;
	}

	if (!(m_hBackBmp = ::CreateDIBSection(m_hMemDC, pbi, DIB_RGB_COLORS, (void**)&m_lpBackBuf, 0, 0)))
	{
		a_free(pbi);
		TRACE("CBackBufWnd::CreateBackBuffer, Failed to call CreateDIBSection() !");
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;

		return false;
	}

	a_free(pbi);

	//	Select backbuffer to memory DC
	m_hOldBmp = (HBITMAP)::SelectObject(m_hMemDC, m_hBackBmp);

	return true;
}

//	Release back buffer
void CBackBufWnd::ReleaseBackBuffer()
{
	if (m_hMemDC)
	{
		::SelectObject(m_hMemDC, m_hOldBmp);
		::DeleteDC(m_hMemDC);

		m_hMemDC	= NULL;
		m_hOldBmp	= NULL;
	}

	if (m_hBackBmp)
	{
		::DeleteObject(m_hBackBmp);
		m_hBackBmp	= NULL;
		m_lpBackBuf = NULL;
	}
}

//	Update window
bool CBackBufWnd::Update()
{
	ASSERT(m_hWnd);

	//	Draw back buffer
	if (!DrawBackBuffer(NULL))
		return false;

	HDC hDC = ::GetDC(m_hWnd);
	Flip(hDC, NULL);
	::ReleaseDC(m_hWnd, hDC);

	return true;
}

//	Flip back buffer
bool CBackBufWnd::Flip(HDC hDC, RECT* lprc)
{
	ASSERT(hDC);

	if (!m_hMemDC)
		return false;

	RECT rc;
	GetClientRect(&rc);

	if (!lprc)
	{
		BitBlt(hDC, 0, 0, rc.right, rc.bottom, m_hMemDC, 0, 0, SRCCOPY);
	}
	else
	{
		BitBlt(hDC, lprc->left, lprc->top, 
				lprc->right - lprc->left, lprc->bottom - lprc->top,
				m_hMemDC, lprc->left, lprc->top, SRCCOPY);
	}

	return true;
}

//	Clear back buffer
void CBackBufWnd::ClearBackBuffer(DWORD dwCol)
{
	if (!m_hBackBmp || !m_lpBackBuf)
		return;

	//	Exchange r and g component
	BYTE b = (BYTE)((dwCol & 0x00ff0000) >> 16);
	BYTE g = (BYTE)((dwCol & 0x0000ff00) >> 8);
	BYTE r = (BYTE)(dwCol & 0x000000ff);
	dwCol = RGB(b, g, r);

	BITMAP Bmp;
	::GetObject(m_hBackBmp, sizeof (Bmp), &Bmp);

	int i, iNumPixel = Bmp.bmWidthBytes * Bmp.bmHeight / 4;
	DWORD* pPixel = (DWORD*)m_lpBackBuf;

	for (i=0; i < iNumPixel; i++)
		*pPixel++ = dwCol;
}


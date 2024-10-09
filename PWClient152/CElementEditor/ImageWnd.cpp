/*
 * FILE: ImageWnd.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#include "ImageWnd.h"
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
//	Implement CImageWnd
//	
///////////////////////////////////////////////////////////////////////////

CImageWnd::CImageWnd()
{
	m_pPicture		= NULL;
	m_hBitmap		= NULL;
	m_bOwnBmp		= false;
	m_strFileName	= "";
}

CImageWnd::~CImageWnd()
{
	CImageWnd::ReleaseImage();
}

BEGIN_MESSAGE_MAP(CImageWnd, CWnd)
	//{{AFX_MSG_MAP(CImageWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	Create scroll bar
bool CImageWnd::Create(DWORD dwStyle, CWnd* pParent, const RECT& rc, DWORD dwID)
{
	dwStyle |= WS_CLIPSIBLINGS;

	if (!CWnd::CreateEx(0, AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW, 0, 
						HBRUSH(COLOR_BTNFACE+1)),
					//	(HBRUSH)::GetStockObject(BLACK_BRUSH)),
						"Image", dwStyle, rc, pParent, dwID, NULL))
		return false;

	return true;
}

//	Release image
void CImageWnd::ReleaseImage()
{
	if (m_pPicture)
	{
		m_pPicture->Release();
		m_pPicture = NULL;
	}

	if (m_hBitmap && m_bOwnBmp)
	{
		::DeleteObject(m_hBitmap);
	}
	
	m_hBitmap = NULL;
}

//	Load image from file
bool CImageWnd::LoadImage(const char* szFile)
{
	//	Release current image
	ReleaseImage();

	InvalidateRect(NULL);

	CFileStatus fstatus;
	CFile File; 
	if (!File.Open(szFile, CFile::modeRead) || !File.GetStatus(szFile, fstatus))
		return false;

	LONG cb; 
	if ((cb = fstatus.m_size) == -1)
	{
		File.Close();
		return false;
	}

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, cb); 
	if (!hGlobal)
	{
		File.Close();
		return false;
	}

	LPVOID pvData = GlobalLock(hGlobal);
	if (!pvData)
	{
		File.Close();
		return false;
	}

	File.Read(pvData, cb); 
	File.Close();

	IStream* pStream = NULL;
	GlobalUnlock(hGlobal); 
	CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);

    if (FAILED(OleLoadPicture(pStream, fstatus.m_size, TRUE, IID_IPicture, (LPVOID*)&m_pPicture)))
	{
		pStream->Release();
		return false;
	}

	pStream->Release();

	//	Save file name
	m_strFileName = szFile;

	return true;
}

//	Set bitmap
void CImageWnd::SetBitmap(HBITMAP hBitmap, bool bOwnBmp)
{
	//	Release current iamge
	ReleaseImage();

	m_hBitmap	= hBitmap;
	m_bOwnBmp	= bOwnBmp;

	InvalidateRect(NULL);
}

BOOL CImageWnd::OnEraseBkgnd(CDC* pDC) 
{
	if (m_pPicture)
		return TRUE;

	return CWnd::OnEraseBkgnd(pDC);
}

void CImageWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (!m_pPicture && !m_hBitmap)
		return;

	RECT rcClient;
	GetClientRect(&rcClient);

	if (m_pPicture)
	{
		OLE_XSIZE_HIMETRIC hmWidth;
		OLE_YSIZE_HIMETRIC hmHeight;
		m_pPicture->get_Width(&hmWidth);
		m_pPicture->get_Height(&hmHeight);

	//	double fX,fY; 
	//	fX = (double)dc.GetDeviceCaps(HORZRES)*(double)hmWidth/((double)dc.GetDeviceCaps(HORZSIZE)*100.0); 
	//	fY = (double)dc.GetDeviceCaps(VERTRES)*(double)hmHeight/((double)dc.GetDeviceCaps(VERTSIZE)*100.0); 

		m_pPicture->Render(dc, 0, 0, rcClient.right, rcClient.bottom, 0, hmHeight, hmWidth, -hmHeight, NULL);
	}
	else
	{
		BITMAP Bmp;
		::GetObject(m_hBitmap, sizeof (Bmp), &Bmp);

		//	Load bitmap into memory DC
		CDC MemDC;
		MemDC.CreateCompatibleDC(&dc);
		CBitmap* pOldBmp = MemDC.SelectObject(CBitmap::FromHandle(m_hBitmap));

		int iStretchMode = dc.SetStretchBltMode(HALFTONE);
		dc.StretchBlt(0, 0, rcClient.right, rcClient.bottom, &MemDC, 0, 0, Bmp.bmWidth, Bmp.bmHeight, SRCCOPY);
		dc.SetStretchBltMode(iStretchMode);
		
		//	Do this as SDK said
		::SetBrushOrgEx(dc.m_hDC, 0, 0, NULL);

		MemDC.SelectObject(pOldBmp);
	}

	//	Do not call CWnd::OnPaint() for painting messages
}


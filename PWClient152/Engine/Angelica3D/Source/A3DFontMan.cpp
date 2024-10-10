/*
 * FILE: A3DFontMan.cpp
 *
 * DESCRIPTION: Routines for managing font used by A3D
 *
 * CREATED BY: hedi, duyuxin, 2001/4/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DFontMan.h"
#include "A3DPI.h"
#include "A3DFont.h"
#include "A3DDevice.h"
#include "AChar.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new	A_DEBUG_NEW

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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DFontMan::A3DFontMan()
{
	m_pA3DDevice	= NULL;
	m_hDC			= NULL;
	m_hBitmap		= NULL;
}

A3DFontMan::~A3DFontMan()
{
}

//	Initialize object.
//	Return true for success, otherwise return false
bool A3DFontMan::Init(A3DDevice* pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;

	//	Create memory DC
	if (!(m_hDC = CreateCompatibleDC(NULL)))
	{
		g_A3DErrLog.Log("A3DFontMan::Init, Failed to create memory DC.");
		return false;
	}

	BITMAPINFO bmi;
	memset(&bmi.bmiHeader, 0, sizeof (BITMAPINFOHEADER));
	bmi.bmiHeader.biSize        = sizeof (BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       = 4;
	bmi.bmiHeader.biHeight      = -4;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;

	DWORD* pDIBData;	//	Address of memory bitmap's data
	m_hBitmap = CreateDIBSection(m_hDC, &bmi, DIB_RGB_COLORS, (void**) &pDIBData, NULL, 0);
	if (!m_hBitmap)
	{
		g_A3DErrLog.Log("A3DFontMan::Init, Failed to create bitmap.");
		return false;
	}

	SetMapMode(m_hDC, MM_TEXT);

	return true;
}

//	Release object's resource
void A3DFontMan::Release()
{
	int i;

	for (i=0; i < m_aFontTypes.GetSize(); i++)
	{
		if (m_aFontTypes[i]->hFont)
			DeleteObject(m_aFontTypes[i]->hFont);

		delete m_aFontTypes[i];
	}

	m_aFontTypes.RemoveAll();

	//	Release all A3D font objects
	if (m_aA3DFonts.GetSize())
	{
		g_A3DErrLog.Log("A3DFontMan::Release, %d A3DFont object found unreleased.", m_aA3DFonts.GetSize());

		for (i=0; i < m_aA3DFonts.GetSize(); i++)
		{
			m_aA3DFonts[i]->ResetRenderBuffer();
			delete m_aA3DFonts[i];
		}

		m_aA3DFonts.RemoveAll();
	}

	//	Release windows DC
	if (m_hDC)
	{
		DeleteDC(m_hDC);
		m_hDC = NULL;
	}

	if (m_hBitmap)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}

	m_pA3DDevice = NULL;
}

/*	Register font

	Return font's handle for success, otherwise return 0.

	szName: name of font will be created
	iHeight: font's height
	dwFlags: A3D font flag
	chSample: a sample character of this font
*/
HA3DFONT A3DFontMan::RegisterFontType(const ACHAR* szName, int iHeight, DWORD dwFlags, 
									  ACHAR chSample)
{
	//	Remove A3DFONT_FILTERED flag
	dwFlags &= ~A3DFONT_FILTERED;

	//	This font has been registered ?
	HA3DFONT hA3DFont = SearchFontType(szName, iHeight, dwFlags);
	if (hA3DFont)
		return hA3DFont;

	FONTTYPE* pFont = new FONTTYPE;
	if (!pFont)
	{
		g_A3DErrLog.Log("A3DFontMan::RegisterFontType, Not enough memory.");
		return 0;
	}

	//	Create font
	DWORD dwBold	= (dwFlags & A3DFONT_BOLD) ? FW_BOLD : FW_NORMAL;
	DWORD dwItalic	= (dwFlags & A3DFONT_ITALIC) ? TRUE : FALSE;
	HFONT hFont		= CreateFont(-iHeight, 0, 0, 0, dwBold, dwItalic,
								 FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								 CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
								 VARIABLE_PITCH, szName);
	if (!hFont)
	{
		g_A3DErrLog.Log("A3DFontMan::RegisterFontType, Failed to register font.");
		return 0;
	}

	pFont->hFont	= hFont;
	pFont->dwFlags	= dwFlags;
	pFont->iHeight	= iHeight;
	pFont->iOutHei	= CaluOutputFont(hFont, chSample);
	pFont->chSample	= chSample;

	a_strcpy(pFont->szName, szName);

	return (DWORD)(m_aFontTypes.Add(pFont) + 1);
}

/*	Register a new font with same type and properties as specified old font
	but with new height.

	Return font's handle for success, otherwise return 0.

	hOldFont: valid A3D font's handle
	iHeight: new height of font.
*/
HA3DFONT A3DFontMan::RegisterFontType(HA3DFONT hOldFont, int iHeight)
{
	int iIndex = HandleToIndex(hOldFont);
	return RegisterFontType(m_aFontTypes[iIndex]->szName, iHeight, m_aFontTypes[iIndex]->dwFlags, m_aFontTypes[iIndex]->chSample);
}

/*	Search a font

	Return font's handle for success, otherwise return 0.

	szName: font's name
	iHeight: font's height
	dwFlags: A3D font flag
*/
HA3DFONT A3DFontMan::SearchFontType(const ACHAR* szName, int iHeight, DWORD dwFlags)
{
	for (int i=0; i < m_aFontTypes.GetSize(); i++)
	{
		FONTTYPE* pFontType = m_aFontTypes[i];
		if (iHeight == pFontType->iHeight && dwFlags == pFontType->dwFlags &&
			!a_strcmp(szName, pFontType->szName))
			return (DWORD)(i+1);
	}

	return 0;
}

/*	Get text string's extent

	Return true for success, otherwise return false.
	
	hA3DFont: A3D font handle.
	szText: text string.
	iNumChar: number of character will be calculated in szText
	piWid, piHei (out): used to receive text string's extent, can be NULL
*/
bool A3DFontMan::GetTextExtent(HA3DFONT hA3DFont, const ACHAR* szText, int iNumChar, int* piWid, int* piHei)
{
	int iIndex = HandleToIndex(hA3DFont);
	HFONT hOldFont = (HFONT)SelectObject(m_hDC, m_aFontTypes[iIndex]->hFont);

	//	Select a bitmap into DC is necessary for anti-aliased font
	HBITMAP hOldBmp	= (HBITMAP)SelectObject(m_hDC, m_hBitmap);

	GetTextExtentBE(szText, iNumChar, piWid, piHei);

	//	Restore DC's resource
	SelectObject(m_hDC, hOldBmp);
	SelectObject(m_hDC, hOldFont);
	
	return true;
}

//	Begin to get text extent
bool A3DFontMan::BeginGetTextExtent(HA3DFONT hA3DFont)
{
	int iIndex  = HandleToIndex(hA3DFont);
	m_hTempFont = (HFONT)SelectObject(m_hDC, m_aFontTypes[iIndex]->hFont);

	//	Select a bitmap into DC is necessary for anti-aliased font
	m_hTempBmp	= (HBITMAP)SelectObject(m_hDC, m_hBitmap);

	return true;
}

//	End getting text extent
bool A3DFontMan::EndGetTextExtent()
{
	//	Restore DC's resource
	SelectObject(m_hDC, m_hTempBmp);
	SelectObject(m_hDC, m_hTempFont);
	return true;
}

//	Get text extent between BeginGetTextExtent() and EndGetTextExtent()
bool A3DFontMan::GetTextExtentBE(const ACHAR* szText, int iNumChar, int* piWid, int* piHei)
{
	SIZE Size;
	GetTextExtentPoint32(m_hDC, szText, iNumChar, &Size);
	
	if (piWid)
		*piWid = Size.cx;

	if (piHei)
		*piHei = Size.cy;

	return true;
}

/*	Calculate font's output height

	Return font's output height.

	hFont: font's handle
	chSample: sample character of this font
*/
int	A3DFontMan::CaluOutputFont(HFONT hFont, ACHAR chSample)
{
	HFONT hOldFont = (HFONT)SelectObject(m_hDC, hFont);

	//	Select a bitmap into DC is necessary for anti-aliased font
	HBITMAP hOldBmp	= (HBITMAP)SelectObject(m_hDC, m_hBitmap);

	SIZE Size;
	GetTextExtentPoint32(m_hDC, &chSample, 1, &Size);

	SelectObject(m_hDC, hOldBmp);
	SelectObject(m_hDC, hOldFont);

	return Size.cy;
}

//	Create A3DFont object
A3DFont* A3DFontMan::CreateA3DFont(HA3DFONT hA3DFont, int iSizeRdCache/* 512 */, 
								   int iTexWid/* 256 */, int iTexHei/* 256 */)
{
	A3DFont* pFont = new A3DFont;
	if (!pFont)
	{
		g_A3DErrLog.Log("A3DFontMan::CreateA3DFont, Not enough memory !");
		return NULL;
	}

	if (!pFont->Init(m_pA3DDevice, hA3DFont, iSizeRdCache, iTexWid, iTexHei))
	{
		g_A3DErrLog.Log("A3DFontMan::CreateA3DFont, Failed to initialize A3DFont !");
		return NULL;
	}

	m_aA3DFonts.Add(pFont);

	return pFont;
}

//	Release A3DFont object
void A3DFontMan::ReleaseA3DFont(A3DFont* pFont)
{
	for (int i=0; i < m_aA3DFonts.GetSize(); i++)
	{
		if (m_aA3DFonts[i] == pFont)
		{
			pFont->Release();
			delete pFont;
			m_aA3DFonts.RemoveAtQuickly(i);
			return;
		}
	}
}

//	Remove all unrendered characters
void A3DFontMan::RemoveUnrenderedChars()
{
	for (int i=0; i < m_aA3DFonts.GetSize(); i++)
		m_aA3DFonts[i]->ResetRenderBuffer();
}



/*
 * FILE: A3DFontMan.h
 *
 * DESCRIPTION: Routines for managing font used by A3D
 *
 * CREATED BY: hedi, duyuxin, 2001/4/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DFONTMAN_H_
#define _A3DFONTMAN_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "AArray.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Font flags
#define A3DFONT_BOLD		0x0001		//	Bold
#define A3DFONT_ITALIC		0x0002		//	Italic
#define A3DFONT_FILTERED	0x0100		//	Filterd

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DDevice;
class A3DFont;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DFontMan
//
///////////////////////////////////////////////////////////////////////////

class A3DFontMan
{
public:		//	Types

	struct FONTTYPE
	{
		HFONT	hFont;			//	Font's handle
		ACHAR	szName[128];	//	Font's name
		ACHAR	chSample;		//	Sample character of this font
		int		iHeight;		//	User set height
		int		iOutHei;		//	Font's output height
		DWORD	dwFlags;		//	Flags
	};

public:		//	Constructors and Destructors

	A3DFontMan();
	virtual ~A3DFontMan();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DDevice* pA3DDevice);
	//	Release object
	void Release();

	//	Register font type
	HA3DFONT RegisterFontType(const ACHAR* szName, int iHeight, DWORD dwFlags, ACHAR chSample);
	//	Register font type with new height
	HA3DFONT RegisterFontType(HA3DFONT hOldFont, int iHeight);
	//	Search a font type by name
	HA3DFONT SearchFontType(const ACHAR* szName, int iHeight, DWORD dwFlags);

	//	Get text string's extent
	bool GetTextExtent(HA3DFONT hA3DFont, const ACHAR* szText, int iNumChar, int* piWid, int* piHei);
	//	Begin to get text extent
	bool BeginGetTextExtent(HA3DFONT hA3DFont);
	//	End getting text extent
	bool EndGetTextExtent();
	//	Get text extent between BeginGetTextExtent() and EndGetTextExtent()
	bool GetTextExtentBE(const ACHAR* szText, int iNumChar, int* piWid, int* piHei);
	
	//	Create A3DFont object
	A3DFont* CreateA3DFont(HA3DFONT hA3DFont, int iSizeRdCache=512, int iTexWid=256, int iTexHei=256);
	//	Release A3DFont object
	void ReleaseA3DFont(A3DFont* pFont);
	//	Remove all unrendered characters
	void RemoveUnrenderedChars();

	//	Get A3DFont object number
	int GetA3DFontNum() { return m_aA3DFonts.GetSize(); }
	//	Get A3DFont object
	A3DFont* GetA3DFont(int n) { return m_aA3DFonts[n]; }

	//	Get font type information
	const FONTTYPE* GetFontTypeInfo(HA3DFONT hA3DFont) { return m_aFontTypes[HandleToIndex(hA3DFont)]; }
	//	Get windows font handle
	HFONT GetWindowsFont(HA3DFONT hA3DFont) { return m_aFontTypes[HandleToIndex(hA3DFont)]->hFont; }
	//	Get font's height
	int GetFontHeight(HA3DFONT hA3DFont) { return m_aFontTypes[HandleToIndex(hA3DFont)]->iHeight; }
	//	Get font's output height
	int GetFontOutputHeight(HA3DFONT hA3DFont) { return m_aFontTypes[HandleToIndex(hA3DFont)]->iOutHei; }
	//	Get font's flags
	DWORD GetFontFlags(HA3DFONT hA3DFont) { return m_aFontTypes[HandleToIndex(hA3DFont)]->dwFlags; }

	HDC	GetWindowsDC() { return m_hDC; }

protected:	//	Attributes

	A3DDevice*	m_pA3DDevice;	//	Device object

	HDC			m_hDC;			//	Windows DC handle
	HBITMAP		m_hBitmap;		//	Bitmap
	HBITMAP		m_hTempBmp;		//	Temporary bitmap handler
	HFONT		m_hTempFont;	//	Temporary font handler

	APtrArray<FONTTYPE*>	m_aFontTypes;	//	Information of font types
	APtrArray<A3DFont*>		m_aA3DFonts;	//	A3DFont objects

protected:	//	Operations

	//	Calculate font's output height
	int	CaluOutputFont(HFONT hFont, ACHAR chSample);
	//	Convert A3D font handle to index
	int HandleToIndex(HA3DFONT hA3DFont) { return (int)hA3DFont - 1; }
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DFONTMAN_H_



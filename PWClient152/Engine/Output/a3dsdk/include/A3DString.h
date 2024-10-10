/*
 * FILE: A3DString.h
 *
 * DESCRIPTION: Routines for drawing text strings
 *
 * CREATED BY: duyuxin, 2001/11/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSTRING_H_
#define _A3DSTRING_H_

#include "A3DVertex.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DStream;
class A3DTexture;
class A3DViewport;
class A3DFontMan;
class A3DDevice;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DString
//
///////////////////////////////////////////////////////////////////////////

class A3DString
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DString();
	virtual ~A3DString();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DDevice* pDevice, const ACHAR* szText, HA3DFONT hA3DFont, bool bFilter=true);
	//	Release object
	void Release();

	//	Output 2D text
	bool DrawText(int x, int y, A3DCOLOR Color, int iHeight=0);
	//	Draw 3D text
	bool Draw3DText(A3DViewport* pView, A3DVECTOR3& vPos, float fRatio, A3DCOLOR Color);
	//	Get sub-string's extent
	bool GetCharsExtent(int iStart, int iEnd, int* piWid, int* piHei);

	//	Enable / Disable filter
	void EnableFilter(bool bEnable) { m_bFilter = bEnable; }
	//	Get filter flag
	bool IsFilter() { return m_bFilter; }
	//	Get text extent
	void GetTextExtent(int* piWid, int* piHei) { *piWid = m_iTextExtX; *piHei = m_iTextExtY; }
	//	Get text lenght
	int	GetTextLength() { return m_iTextLen; }
	//	Get text string
	const ACHAR* GetTextString() { return m_szText; }

	//	Set / Get output height of string
	void SetOutputHeight(int iHei) { m_iOutHei = iHei; }
	int	GetOutputHeight() { return m_iOutHei; }

	//	Get A3D font handler used by this string
	HA3DFONT GetA3DFont() { return m_hA3DFont; }

protected:	//	Attributes

	bool		m_bHWIString;	//	Using outside graphics engine

	A3DDevice*	m_pA3DDevice;	//	A3D device object
	A3DStream*	m_pA3DStream;	//	A3D stream object
	A3DTexture*	m_pA3DTexture;	//	A3D texture object
	HA3DFONT	m_hA3DFont;		//	A3D font handler
	ACHAR*		m_szText;		//	Text string
	int*		m_aCharWids;	//	Every character's width
	int			m_iTextLen;		//	Text length in character

	bool		m_bFilter;		//	Filter flag
	bool		m_bReady;		//	Ready flag

	int			m_iTexWid;		//	Texture's width
	int			m_iTexHei;		//	Texture's height
	int			m_iLineHei;		//	Height of every line in texture
	int			m_iNumLines;	//	Number of line of text laid in texture
	int			m_iLastWid;		//	Width of text on the last line of texture
	int			m_iOutHei;		//	Output height

	int			m_iTextExtX;	//	Text's real extent on screen
	int			m_iTextExtY;

protected:	//	Operations

	//	Calculate texture size
	bool CalculateTextureSize(A3DFontMan* pFontMan, SIZE* pSize);
	//	Create vertices and indices
	bool CreateStream();
	//	Fill text's texture
	bool FillTexture(BYTE* pDIBData, int iPitch);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSTRING_H_




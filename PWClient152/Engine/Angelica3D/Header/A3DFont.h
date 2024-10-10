/*
 * FILE: A3DFont.h
 *
 * DESCRIPTION: Routines for display font
 *
 * CREATED BY: duyuxin, 2003/12/5
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DFONT_H_
#define _A3DFONT_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DVertex.h"
#include "hashtab.h"
#include "AArray.h"
#include "ARect.h"

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

class A3DDevice;
class A3DTexture;
class A3DStream;
class A3DFontMan;
class A3DViewport;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DFont
//
///////////////////////////////////////////////////////////////////////////

class A3DFont
{
public:		//	Types

	//	Character properties
	struct CHARPROP
	{
		ACHAR	ch;			//	Character
		int		iTexPage;	//	Texture page
		float	tu1;		//	Texture cooridinates
		float	tv1;
		float	tu2;
		float	tv2;
		ARectI	rect;		//	Rectangle on texture
	};

	//	Texture slot
	struct TEXTURESLOT
	{
		A3DTexture*		pTexture;
		A3DTLVERTEX*	pVertBuf;		//	Locked vertex buffer
		int				iVertCnt;		//	Vertex fill counter
		int				iIdxCnt;		//	Index fill counter
	};

	typedef abase::hashtab<CHARPROP*, wchar_t, abase::_hash_function>	CharTable;

public:		//	Constructors and Destructors

	A3DFont();
	virtual ~A3DFont();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DDevice* pDevice, HA3DFONT hA3DFont, int iSizeRdCache, int iTexWid, int iTexHei);
	//	Release object
	void Release();

	//	Load characters from file and add them to table
	bool LoadCharTable(const char* szFile);
	//	Save character table to file
	bool SaveCharTable(const char* szFile);
	//	Add new characters to char table
	bool AddCharsToTable(const ACHAR* szText, int iNumChar);

	//	Text out 
	bool TextOut(int x, int y, const ACHAR* szText, A3DCOLOR color, int iHeight=0, float z=0.0f, bool bBorder=false);
	//	Draw text
	bool DrawText(int x, int y, const ACHAR* szText, A3DCOLOR color, A3DRECT* pRect, int iHeight=0, float z=0.0f);
	//	3D text out
	bool TextOut3D(const ACHAR* szText, A3DViewport* pView, const A3DVECTOR3& vCenter, int iHeight, A3DCOLOR color);
	//	Flush all streams
	bool Flush();
	//	Reset streams, all unrendered characters will be discarded
	bool ResetRenderBuffer();

	//	Enable / Disable filter
	void EnableFilter(bool bEnable) { m_bFilter = bEnable; }
	//	Get filter flag
	bool IsFilter() { return m_bFilter; }

	//	Get texture size
	int GetTextureWidth() { return m_iTexWidth; }
	int GetTextureHeight() { return m_iTexHeight; }
	//	Get texture number
	int	GetTextureNum() { return m_aTexSlots.GetSize(); }

	//	Get text string's extent
	bool GetTextExtent(const ACHAR* szText, int iNumChar, int* piWid, int* piHei);
	//	Get font's height
	int GetFontHeight();
	//	Get font's output height
	int GetFontOutputHeight();
	//	Get font's flags
	DWORD GetFontFlags();

	//	Get A3D font handler
	HA3DFONT GetA3DFont() { return m_hA3DFont; }

protected:	//	Attributes

	A3DDevice*	m_pA3DDevice;
	A3DFontMan*	m_pA3DFontMan;
	HA3DFONT	m_hA3DFont;
	A3DStream*	m_pA3DStream;	

	bool		m_bNoRender;		//	No rendering flag
	bool		m_bFilter;			//	Filter flag
	int			m_iSizeRdCache;		//	Size of render cache (in characters)
	int			m_iVertBufSize;		//	Size of stream's vertex buffer in vertex number
	int			m_iIdxBufSize;		//	Size of stream's index buffer in index number
	int			m_iTexWidth;		//	Texture size
	int			m_iTexHeight;

	int			m_iLineHei;			//	Line height in texture
	int			m_iFillHei;			//	Currently filled height
	int			m_iFillWid;			//	Currently filled width

	APtrArray<TEXTURESLOT*>	m_aTexSlots;	//	Texture slots
	CharTable				m_CharTable;	//	Character table

protected:	//	Operations

	//	Create stream
	bool CreateStream();
	//	Lock stream
	bool LockStream(int iSlot, bool bLock);
	//	Add character vertices to vertex buffer waiting for rendering
	bool AddVertsToOutputBuf(int iSlot, A3DTLVERTEX* aVerts, int iNumChar);
	//	Flush output buffer
	bool FlushOutputBuf(int iSlot);
	//	Allocate a texture slot
	TEXTURESLOT* AllocTextureSlot();
	//	Free a texture slot
	void FreeTextureSlot(TEXTURESLOT* pSlot);
	//	Print character onto texture
	bool CalculateCharPos(CHARPROP* pProp, ACHAR ch);
	//	Print new characters on textures
	bool PrintCharsOnTextures(APtrArray<CHARPROP*>& aChars);
	//	Create DC's bitamp
	BYTE* CreateDCBitmap(APtrArray<CHARPROP*>& aChars, HBITMAP* phBmp, int* piPitch, int* piHeight);
	//	Print a character on texture
	void PrintCharOnTexture(BYTE* pDst, ARectI* pDstRect, int iDstPitch, BYTE* pSrc, int cx, int cy, int iSrcPitch);
};



#endif	//	_A3DFONT_H_
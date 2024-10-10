/*
 * FILE: A3DStringSet.h
 *
 * DESCRIPTION: Routines for drawing multiline text strings at a same time
 *
 * CREATED BY: duyuxin, 2002/1/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSTRINGSET_H_
#define _A3DSTRINGSET_H_

#include "A3DVertex.h"
#include "AArray.h"

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
class A3DStream;
class A3DTexture;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DStringSet
//
///////////////////////////////////////////////////////////////////////////

class A3DStringSet
{
public:		//	Types

	enum
	{
		SIZE_VERTEXBUF	= 512,		//	Size of vertex buffer in vertices
		SIZE_INDEXBUF	= 768,		//	Size of index buffer in indices
	};

	struct TEXPOS
	{
		float	u1;
		float	v1;
		float	u2;
		float	v2;
	};

	//	String information
	struct STRING
	{
		ACHAR*	pszText;		//	Text string
		int		iTextLen;		//	Text length in characters
		int		iExtX;			//	String's extent
		int		iExtY;
		DWORD	dwCol;			//	Color of string
		DWORD	dwSaveCol;		//	Saved color

		int		iNumSeg;		//	Number of segment the text has been cut to be
								//		iNumSeg = iLastLine - iFirstLine + 1
		int		iFirstLine;		//	First line in texture
		int		iFirstOff;		//	Offset of the first line in texture
		int		iLastLine;		//	Last line in texture
		int		iLastOff;		//	Offset of the last line in texture
		TEXPOS*	aTexPos;		//	Texture coordinates of each segments
	};

public:		//	Constructors and Destructors

	A3DStringSet();
	virtual ~A3DStringSet();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DDevice* pDevice, HA3DFONT hA3DFont, bool bFilter);
	//	Release object
	void Release();
	//	Reset
	bool Reset();

	//	Build texture and prepare to render
	bool Build();

	//	Add a string
	bool AddString(const ACHAR* szStr, A3DCOLOR Color);
	//	Add a multibyte string
	bool AddMultiByteString(const char* szStr, A3DCOLOR Color);
	//	Get a string's extent
	bool GetStringExtent(int iIndex, int* piWid, int* piHei);
	//	Get output area size
	bool GetOutputAreaSize(int iLineSpace, int* pcx, int* pcy, int iOutHei=0);
	//	Set color of a string or all strings
	bool SetColor(int iIndex, A3DCOLOR Color);
	//	Save all strings' color
	void SaveColors();
	//	Restore all strings' color
	void RestoreColors();

	//	Draw a string
	bool OutputString(int iIndex, int x, int y, int iHeight=0);	
	//	Flush rendering
	bool Flush();

	void EnableFilter(bool bEnable)	{ m_bFilter = bEnable; }
	int GetStringNum() { return m_aStrings.GetSize(); }
	void SetOutputHeight(int iHei) { m_iOutHei = iHei; }
	int GetOutputHeight() { return m_iOutHei; }

	//	Get texture size
	int GetTextureWid() { return m_iTexWid; }
	int GetTextureHei() { return m_iTexHei; }

	//	Get A3D font handler used by this string
	HA3DFONT GetA3DFont() { return m_hA3DFont; }

protected:	//	Attributes

	bool			m_bHWIStringSet;	//	using outside graphics engine;
	A3DDevice*		m_pA3DDevice;		//	A3D device object
	A3DStream*		m_pA3DStream;		//	A3D stream object
	A3DTexture*		m_pA3DTexture;		//	A3D texture object

	A3DTLVERTEX*	m_aVertBuf;			//	Locked vertex buffer
	int				m_iVertCnt;			//	Vertex filling count

	bool		m_bReady;		//	Ready to output ?
	bool		m_bFilter;		//	Filter

	HA3DFONT	m_hA3DFont;		//	A3D font handle

	int			m_iTexWid;		//	Texture's width
	int			m_iTexHei;		//	Texture's height
	int			m_iLineHei;		//	Height of every line in texture
	int			m_iNumLines;	//	Number of line of text laid in texture
	int			m_iOutHei;		//	Output height

	APtrArray<STRING*>	m_aStrings;		//	String array

protected:	//	Operations

	bool		CalculateTextureSize(SIZE* pSize);	//	Calculate texture size
	void		ReleaseAllStrings();				//	Release all strings
	bool		CreateStream();						//	Create vertices and indices
	bool		FillTexture(BYTE* pDIBData, int iPitch);	//	Fill text's texture
	bool		CalcTexPos();								//	Calculate texture positions for segments
	void		FillRect(BYTE* pDst, RECT* pDstRect, int iDstPitch, 
						 BYTE* pSrc, RECT* pSrcRect, int iSrcPitch);	//	Copy rectangle
	bool		LockStream(bool bLock);		//	Lock/unlock stream
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSTRINGSET_H_




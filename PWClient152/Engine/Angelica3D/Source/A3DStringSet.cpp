/*
 * FILE: A3DStringSet.cpp
 *
 * DESCRIPTION: Routines for drawing multiline text strings at a same time
 *
 * CREATED BY: duyuxin, 2002/1/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DPI.h"
#include "A3DStringSet.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DConfig.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "A3DFontMan.h"
#include "A3DFuncs.h"
#include "AChar.h"
#include "AAssist.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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

A3DStringSet::A3DStringSet()
{
	m_pA3DDevice	= NULL;
	m_pA3DStream	= NULL;
	m_pA3DTexture	= NULL;
	m_aVertBuf		= NULL;
	m_hA3DFont		= NULL;

	m_bHWIStringSet	= false;

	m_iTexWid		= 0;
	m_iTexHei		= 0;
	m_iLineHei		= 0;
	m_iNumLines		= 0;
	m_iVertCnt		= 0;
	m_iOutHei		= 16;
}

A3DStringSet::~A3DStringSet()
{
}

/*	Initialize object.

	Return true for success, otherwise return false

	pDevice: address of A3D device.
	hA3DFont: A3D font handle got from A3DFontMan
	bFilter: true, use antiliase font,
*/	
bool A3DStringSet::Init(A3DDevice* pDevice, HA3DFONT hA3DFont, bool bFilter)
{
	if (!hA3DFont)
	{
		g_A3DErrLog.Log("A3DStringSet::Init, invalid font handle.");
		return false;
	}

	m_pA3DDevice = pDevice;
	m_bFilter	 = bFilter;
	m_bReady	 = false;
	m_hA3DFont	 = hA3DFont;

	A3DFontMan* pFontMan = m_pA3DDevice->GetA3DEngine()->GetA3DFontMan();
	m_iOutHei = pFontMan->GetFontOutputHeight(hA3DFont);

	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
	{
		m_bHWIStringSet = true;
		return true;
	}

	//	Create texture and stream objects
	if (!(m_pA3DTexture = new A3DTexture))
	{
		g_A3DErrLog.Log("A3DStringSet::Init Failed to create texture object.");
		goto Error;
	}

	if (!CreateStream())
	{
		g_A3DErrLog.Log("A3DStringSet::Init Failed to create stream object.");
		goto Error;
	}

	return true;

Error:

	Release();
	return false;
}

/*	Create vertex and indices buffer

	Return true for success, otherwise return false
*/
bool A3DStringSet::CreateStream()
{
	//	Create stream
	if (!(m_pA3DStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DStringSet::Init Failed to create stream object.");
		return false;
	}

	if (!m_pA3DStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, SIZE_VERTEXBUF, SIZE_INDEXBUF,
				A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
	{
		g_A3DErrLog.Log("A3DStringSet::Init Failed to create stream object.");
		return false;
	}

	//	Fill indices
	WORD* aIndices;

	if (!m_pA3DStream->LockIndexBuffer(0, 0, (BYTE**) &aIndices, 0))
	{
		g_A3DErrLog.Log("A3DStringSet::CreateStream(), Failed to lock index buffer!");
		return false;
	}

	int i, n, m, iNumRect = SIZE_VERTEXBUF / 4;
	for (i=0, n=0, m=0; i < iNumRect; i++, n+=6, m+=4)
	{
		aIndices[n+0] = m + 0;
		aIndices[n+1] = m + 1;
		aIndices[n+2] = m + 2;
		aIndices[n+3] = m + 1;
		aIndices[n+4] = m + 3;
		aIndices[n+5] = m + 2;
	}

	if (!m_pA3DStream->UnlockIndexBuffer())
	{
		g_A3DErrLog.Log("A3DStringSet::CreateStream(), Failed to unlock index buffer!");
		return false;
	}

	return true;
}

/*	Reset object.

	Return true for success, otherwise return false
*/
bool A3DStringSet::Reset()
{
	//	Release all strings
	ReleaseAllStrings();

	if (m_pA3DTexture)
		m_pA3DTexture->Release();

	if (m_pA3DStream && m_aVertBuf)
	{
		//	Stream still be locked ! this shouldn't happen
		LockStream(false);
	}

	m_bReady	= false;
	m_iVertCnt	= 0;

	if (m_pA3DDevice)
		m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DEngine::OBJECTCNT_STRING);

	return true;
}

//	Release all resource
void A3DStringSet::Release()
{
	//	Release all strings
	ReleaseAllStrings();

	//	Release texture and stream objects
	if (m_pA3DTexture)
	{
		m_pA3DTexture->Release();
		delete m_pA3DTexture;
		m_pA3DTexture = NULL;
	}

	if (m_pA3DStream)
	{
		if (m_aVertBuf)		//	Stream still be locked ! this shouldn't happen
			LockStream(false);
	
		m_pA3DStream->Release();
		delete m_pA3DStream;
		m_pA3DStream = NULL;
	}

	m_hA3DFont	 = NULL;
	m_pA3DDevice = NULL;
	m_bReady	 = false;
	m_iVertCnt	 = 0;
}

//	Release all strings
void A3DStringSet::ReleaseAllStrings()
{
	for (int i=0; i < m_aStrings.GetSize(); i++)
	{
		STRING* pSlot = m_aStrings[i];
		if (pSlot->aTexPos)
			delete [] pSlot->aTexPos;

		if (pSlot->pszText)
			delete [] pSlot->pszText;

		delete pSlot;
	}
	
	m_aStrings.RemoveAll();
}

/*	Add a string. All strings want to be rendered by A3DStringSet should be added by
	this function before A3DStringSet::Build is called, otherwise this operation
	will fail.

	Return true for success, otherwise return false

	szStr: text string
	Color: string's color
*/
bool A3DStringSet::AddString(const ACHAR* szStr, A3DCOLOR Color)
{
	if (m_bHWIStringSet)
		return true;

	if (m_bReady)
	{
		g_A3DErrLog.Log("A3DStringSet::AddString, Cannot add string after building !");
		return false;
	}

	STRING* pSlot = new STRING;
	if (!pSlot)
	{
		g_A3DErrLog.Log("A3DStringSet::AddString, Not enough memory !");
		return false;
	}

	A3DFontMan* pFontMan = m_pA3DDevice->GetA3DEngine()->GetA3DFontMan();

	if (!szStr || !szStr[0])	//	Insert an empty string
	{
		pSlot->pszText		= NULL;
		pSlot->iTextLen		= 0;
		pSlot->dwCol		= Color;
		pSlot->dwSaveCol	= Color;
		pSlot->iExtX		= 0;
		pSlot->iExtY		= pFontMan->GetFontOutputHeight(m_hA3DFont);
		pSlot->aTexPos		= NULL;
		pSlot->iNumSeg		= 0;

		m_aStrings.Add(pSlot);
		return true;
	}

	pSlot->iTextLen	= a_strlen(szStr);
	pSlot->pszText	= new ACHAR [pSlot->iTextLen+1];
	pSlot->dwCol	= Color;
	pSlot->iNumSeg	= 0;
	pSlot->aTexPos	= NULL;

	a_strcpy(pSlot->pszText, szStr);

	//	Get string's extent from A3DFontMan
	pFontMan->GetTextExtent(m_hA3DFont, pSlot->pszText, pSlot->iTextLen, 
							&pSlot->iExtX, &pSlot->iExtY);

	m_aStrings.Add(pSlot);

	return true;
}

//	Add a multibyte string
bool A3DStringSet::AddMultiByteString(const char* szStr, A3DCOLOR Color)
{
#ifdef UNICODE

	if (szStr && szStr[0])
	{
		int iLen = strlen(szStr);
		wchar_t* wszStr = new wchar_t [iLen+1];
		a_MultiByteToWideChar(szStr, -1, wszStr, iLen+1);
		
		bool bRet = AddString(wszStr, Color);
		delete [] wszStr;

		return bRet;
	}
	else
	{
		//	Add a empty line
		return AddString(NULL, Color);
	}

#else

	return AddString(szStr, Color);

#endif	//	UNICODE
}

/*	Get a string's extent.

	Return true for success, otherwise return false.

	iIndex: string's index.
	piWid, piHei (out): receive string's extent if true is returned. can be NULL
*/
bool A3DStringSet::GetStringExtent(int iIndex, int* piWid, int* piHei)
{
	if (m_bHWIStringSet)
	{
		*piWid = 0;
		*piHei = 0;
		return true;
	}

	if (piWid)
		*piWid = m_aStrings[iIndex]->iExtX;

	if (piHei)
		*piHei = m_aStrings[iIndex]->iExtY;

	return true;
}

//	Get output area size
bool A3DStringSet::GetOutputAreaSize(int iLineSpace, int* pcx, int* pcy,  int iOutHei/* 0 */)
{
	if (m_bHWIStringSet || !m_aStrings.GetSize())
	{
		if (pcx) *pcx = 0;
		if (pcy) *pcy = 0;
		return true;
	}

	int i, iHei, iFontHei, iRight=-9999;
	int iNumString = m_aStrings.GetSize();

	for (i=0; i < iNumString; i++)
	{
		if (m_aStrings[i]->iExtX > iRight)
			iRight = m_aStrings[i]->iExtX;
	}

	iFontHei = m_aStrings[0]->iExtY;
	iHei	 = iOutHei ? iOutHei : iFontHei;

	if (pcx)
		*pcx = (int)ceil(iRight / (float)iFontHei * iHei);

	if (pcy)
	{
		if (iNumString == 1)
			*pcy = iNumString * iHei;
		else
			*pcy = iNumString * iHei + (iNumString-1) * iLineSpace;
	}

	return true;
}

/*	Set color of a stirng or all strings.

	Return true for success, otherwise return false.

	iIndex: string's index. -1 means set color for all strings
	Color: new color of string
*/
bool A3DStringSet::SetColor(int iIndex, A3DCOLOR Color)
{
	if( m_bHWIStringSet ) return true;
	
	if (iIndex < 0)
	{
		for (int i=0; i < m_aStrings.GetSize(); i++)
			m_aStrings[i]->dwCol = Color;
	}
	else
		m_aStrings[iIndex]->dwCol = Color;

	return true;
}

//	Save all strings' color
void A3DStringSet::SaveColors()
{
	for (int i=0; i < m_aStrings.GetSize(); i++)
		m_aStrings[i]->dwSaveCol = m_aStrings[i]->dwCol;
}

//	Restore all strings' color
void A3DStringSet::RestoreColors()
{
	for (int i=0; i < m_aStrings.GetSize(); i++)
		m_aStrings[i]->dwCol = m_aStrings[i]->dwSaveCol;
}

/*	Build texture and prepare to render. After this operation, no stirng can be added
	to A3DStringSet unless Reset() is called.

	Return true for success, otherwise return false.
*/
bool A3DStringSet::Build()
{
	if (m_bHWIStringSet)
		return true;

	if (!m_aStrings.GetSize() || m_bReady)
		return true;

	ASSERT(m_pA3DTexture);

	//	Get windows's DC and font from A3DFontMan
	A3DFontMan* pFontMan = m_pA3DDevice->GetA3DEngine()->GetA3DFontMan();

	HDC	hDC	= pFontMan->GetWindowsDC();
	HFONT hOldFont, hFont;

	hFont	 = pFontMan->GetWindowsFont(m_hA3DFont);
	hOldFont = (HFONT)SelectObject(hDC, hFont);

	SIZE Size;
	HBITMAP hBitmap, hOldBmp;
	BYTE* pDIBData;
	
	//	Calculate texture size
	if (!CalculateTextureSize(&Size))
	{
		g_A3DErrLog.Log("A3DStringSet::Build, invalid size.");
		return false;
	}

	//	All strings are empty line ?
	if (!Size.cx || !Size.cy)
		return true;	//	Only return true but don't set m_bReady flag

	//	Create texture
	if (!m_pA3DTexture->Create(m_pA3DDevice, m_iTexWid, m_iTexHei, A3DFMT_A8R8G8B8))
	{
		g_A3DErrLog.Log("A3DStringSet::Build failed to create new texture.");
		return false;
	}

	//	Create a memory bitmap to store text
	BITMAPINFO bmi;
	memset(&bmi.bmiHeader, 0, sizeof (BITMAPINFOHEADER));
	bmi.bmiHeader.biSize        = sizeof (BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       = Size.cx;
	bmi.bmiHeader.biHeight      = -Size.cy;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;

	hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (VOID**) &pDIBData, NULL, 0);
	hOldBmp	= (HBITMAP)SelectObject(hDC, hBitmap);

	//	Clear bitmap
	int iPitch = ((Size.cx * 4 + 3) >> 2) << 2;
	memset(pDIBData, 0, Size.cy * iPitch);

	//	Set text properties
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, 0);
	SetTextAlign(hDC, TA_TOP);

	//	Select font into screen DC so that anti-aliased can do effect
	HDC hScreenDC = GetDC(NULL);
	HFONT hOldSFont = (HFONT)SelectObject(hScreenDC, hFont);

	//	Text out string to memory bitmap
	int i, y;
	for (i=0, y=0; i < m_aStrings.GetSize(); i++)
	{
		STRING* pSlot = m_aStrings[i];
		if (pSlot->iTextLen)
		{
			ExtTextOut(hDC, 0, y, ETO_OPAQUE, NULL, pSlot->pszText, 
					   pSlot->iTextLen, NULL);

			y += m_iLineHei;
		}
	}

	SelectObject(hScreenDC, hOldSFont);
	ReleaseDC(NULL, hScreenDC);

	//	Fill texture
	FillTexture((BYTE*)pDIBData, iPitch);

	//	Release resources
	SelectObject(hDC, hOldFont);
	SelectObject(hDC, hOldBmp);
	DeleteObject(hBitmap);

	//	Calculate texture coordinates for segments
	if (!CalcTexPos())
	{
		g_A3DErrLog.Log("A3DStringSet::Build, failed to call CalcTexPos().");
		return false;
	}

	m_bReady = true;

	if (m_pA3DDevice)
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_STRING);

	return true;
}

/*	Calculate texture size. This function will consider texture to be square

	Return true for success, otherwise return false.

	pSize (out): used to receive DC's size on which text will be output
*/
bool A3DStringSet::CalculateTextureSize(SIZE* pSize)
{
	int i, iArea, iWid, iHei, iMaxWid=0, iLen=0;
	int iNumString;

	m_iLineHei = 0;
	iNumString = 0;		//	Number of none empty string

	for (i=0; i < m_aStrings.GetSize(); i++)
	{
		STRING* pSlot = m_aStrings[i];
		if (!pSlot->iTextLen)
			continue;

		iLen += pSlot->iExtX;

		if (pSlot->iExtY > m_iLineHei)
			m_iLineHei = pSlot->iExtY;

		if (pSlot->iExtX > iMaxWid)
			iMaxWid = pSlot->iExtX;

		iNumString++;
	}

	if (!iMaxWid)	//	All strings are empty line
	{
		pSize->cx	= 0;
		pSize->cy	= 0;
		m_iTexWid	= 0;
		m_iTexHei	= 0;
		m_iNumLines	= 0;
		return true;	
	}

	iArea	= iLen * m_iLineHei;
	iWid	= 4;
	iHei	= 4;

	for (i=0; i < 16; i++)
	{
		if ((1 << (iWid + iHei)) > iArea)
			break;

		iWid++;

		if ((1 << (iWid + iHei)) > iArea)
			break;

		iHei++;
	}

	if (i == 16)	//	Too big texture !
		return false;

	m_iTexWid	= 1 << iWid;
	m_iTexHei	= 1 << iHei;
	m_iNumLines = iLen / m_iTexWid + 1;

	//	Check again
	if (m_iNumLines * m_iLineHei > m_iTexHei)
		m_iTexHei <<= 1;

	pSize->cx = iMaxWid;
	pSize->cy = iNumString * m_iLineHei;

	return true;
}

/*	Fill text's texture.

	Return true for success, otherwise return false.

	pDIBData: address of DIB which contains text.
	iPitch: pitch of text image (in bytes)
*/
bool A3DStringSet::FillTexture(BYTE* pDIBData, int iPitch)
{
	int i, iTexPitch, iOff, iRemain, iLine, iLeftWid, iSrcTop;
	RECT rcSrc, rcDst;
	BYTE* pTexBuf;

	if (!m_pA3DTexture->LockRect(NULL, (void**) &pTexBuf, &iTexPitch, 0))
		return false;
	
	iOff	= 0;
	iRemain	= m_iTexWid;
	iLine	= 0;
	iSrcTop	= 0;

	for (i=0; i < m_aStrings.GetSize(); i++)
	{
		STRING* pSlot = m_aStrings[i];
		if (!pSlot->iTextLen)	//	Is an empty line
			continue;

		pSlot->iFirstOff	= iOff;
		pSlot->iFirstLine	= iLine;

		rcSrc.top	 = iSrcTop;
		rcSrc.bottom = rcSrc.top + m_iLineHei - 1;

		rcDst.top	 = m_iLineHei * iLine;
		rcDst.bottom = rcDst.top + m_iLineHei - 1;

		iSrcTop += m_iLineHei;

		//	Total string can be put in this texture line ?
		if (pSlot->iExtX <= iRemain)
		{
			rcSrc.left	= 0;
			rcSrc.right	= pSlot->iExtX - 1;

			rcDst.left	= iOff;
			rcDst.right	= iOff + pSlot->iExtX - 1;

			FillRect(pTexBuf, &rcDst, iTexPitch, pDIBData, &rcSrc, iPitch);

			iOff	+= pSlot->iExtX;
			iRemain -= pSlot->iExtX;

			pSlot->iLastLine = iLine;
			pSlot->iLastOff	 = iOff;
			pSlot->iNumSeg	 = 1;

			continue;
		}

		//	String must be divided to two or more segments
		iLeftWid	 = pSlot->iExtX - iRemain;
		rcSrc.left	 = 0;
		rcSrc.right	 = iRemain - 1;

		rcDst.left	 = iOff;
		rcDst.right	 = m_iTexWid - 1;

		FillRect(pTexBuf, &rcDst, iTexPitch, pDIBData, &rcSrc, iPitch);

		//	The rest segment
		rcSrc.left	= iRemain;
		iOff		= 0;
		iRemain		= m_iTexWid;

		while (iLeftWid > m_iTexWid)
		{
			iLine++;

			iLeftWid   -= m_iTexWid;
			rcSrc.right	= rcSrc.left + m_iTexWid - 1;
		
			rcDst.top	+= m_iLineHei;
			rcDst.bottom = rcDst.top + m_iLineHei - 1;	
			rcDst.left	 = 0;
			rcDst.right	 = m_iTexWid - 1;

			FillRect(pTexBuf, &rcDst, iTexPitch, pDIBData, &rcSrc, iPitch);

			rcSrc.left += m_iTexWid;
		}

		iLine++;

		//	The last tail of current string
		if (iLeftWid)
		{
			rcSrc.right	 = rcSrc.left + iLeftWid - 1;
			rcDst.top	+= m_iLineHei;
			rcDst.bottom = rcDst.top + m_iLineHei - 1;
			rcDst.left	 = 0;
			rcDst.right	 = iLeftWid - 1;

			FillRect(pTexBuf, &rcDst, iTexPitch, pDIBData, &rcSrc, iPitch);

			iOff	= iLeftWid;
			iRemain	= m_iTexWid - iOff;

			pSlot->iLastLine = iLine;
			pSlot->iLastOff	 = iOff;
		}
		else
		{
			iOff	= 0;
			iRemain	= m_iTexWid;

			pSlot->iLastLine = iLine - 1;
			pSlot->iLastOff	 = m_iTexWid;
		}

		pSlot->iNumSeg = pSlot->iLastLine - pSlot->iFirstLine + 1;
	}

	if (!m_pA3DTexture->UnlockRect())
		return false;

	return true;
}

/*	Fill a rectangle area in texture.

	pDst: 16-bit texture data pointer
	pDstRect: destination rectangle
	iDstPitch: texture's pitch
	pSrc: 16-bit source image data
	pSrcRect: source rectangle
	iSrcPitch: source image's pitch
*/
void A3DStringSet::FillRect(BYTE* pDst, RECT* pDstRect, int iDstPitch, 
							BYTE* pSrc, RECT* pSrcRect, int iSrcPitch)
{
	WORD *pDstPixel;
	DWORD *pSrcPixel;
	int i, j, iWidth, iHeight;
	BYTE byAlpha;

	iWidth	= pDstRect->right - pDstRect->left + 1;
	iHeight	= pDstRect->bottom - pDstRect->top + 1;

	pDst = pDst + pDstRect->top * iDstPitch + pDstRect->left * 2;
	pSrc = pSrc + pSrcRect->top * iSrcPitch + pSrcRect->left * 4;

	for (i=0; i < iHeight; i++)
	{
		pDstPixel = (WORD*)pDst;
		pSrcPixel = (DWORD*)pSrc;

		for (j=0; j < iWidth; j++, pDstPixel++, pSrcPixel++)
		{
			byAlpha = (*((BYTE*)pSrcPixel) & 0xff) >> 4;
			if (byAlpha > 0)
				*pDstPixel = (byAlpha << 12) | 0x0fff;
			else
				*pDstPixel = 0;
		}

		pDst += iDstPitch;
		pSrc += iSrcPitch;
	}
}

//	Calculate texture positions for segments
bool A3DStringSet::CalcTexPos()
{
	//	Calculate number of rectangle needed to render strings
	int i, n, m;
	float fTexLineHei;

	float fInvWSize = 1.0f / m_iTexWid;
	float fInvHSize = 1.0f / m_iTexHei;
	
	fTexLineHei = m_iLineHei * fInvHSize;

	//	Fill vertices' texture coordinates
	for (i=0; i < m_aStrings.GetSize(); i++)
	{
		STRING* pSlot = m_aStrings[i];
		if (!pSlot->iTextLen)	//	Is an empty line
			continue;

		if (!(pSlot->aTexPos = new TEXPOS[pSlot->iNumSeg]))
		{
			g_A3DErrLog.Log("A3DStringSet::CalcTexPos(), Not enough memory!");
			return false;
		}

		TEXPOS* pPos = &pSlot->aTexPos[0];

		if (pSlot->iNumSeg == 1)
		{
			//	Only one segment
			pPos->u1 = pSlot->iFirstOff * fInvWSize;
			pPos->v1 = pSlot->iFirstLine * m_iLineHei * fInvHSize;
			pPos->u2 = pSlot->iLastOff * fInvWSize;
			pPos->v2 = pPos->v1 + fTexLineHei;
			continue;
		}
		
		//	First segment
		pPos->u1 = pSlot->iFirstOff * fInvWSize;
		pPos->v1 = pSlot->iFirstLine * m_iLineHei * fInvHSize;
		pPos->u2 = 1.0f;
		pPos->v2 = pPos->v1 + fTexLineHei;
		pPos++;

		m = pSlot->iFirstLine + 1;

		//	Middle segments
		for (n=1; n < pSlot->iNumSeg-1; n++, m++)
		{
			pPos->u1 = 0.0f;
			pPos->v1 = m * m_iLineHei * fInvHSize;
			pPos->u2 = 1.0f;
			pPos->v2 = pPos->v1 + fTexLineHei;
			pPos++;
		}

		//	The last segment
		pPos->u1 = 0.0f;
		pPos->v1 = pSlot->iLastLine * m_iLineHei * fInvHSize;
		pPos->u2 = pSlot->iLastOff * fInvWSize;
		pPos->v2 = pPos->v1 + fTexLineHei;
	}

	return true;
}

//	Lock/unlock stream
bool A3DStringSet::LockStream(bool bLock)
{
	if (bLock)
	{
		if (!m_pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**)&m_aVertBuf, 0))
			return false;

		m_iVertCnt = 0;
	}
	else
	{
		if (!m_pA3DStream->UnlockVertexBufferDynamic())
			return false;

		m_aVertBuf = NULL;
	}

	return true;
}

/*	Draw specified string.

	Return true for success, otherwise return false

	iIndex: string's index
	x, y: The first string's left-top corner position on screen.
	iHeight: output height of text. 0 means to use the m_iOutHei. This parameter
			 won't change the value of m_iOutHei
*/
bool A3DStringSet::OutputString(int iIndex, int x, int y, int iHeight/* 0 */)
{
	STRING* pSlot = m_aStrings[iIndex];

	if (m_bHWIStringSet || !m_bReady || !pSlot->iTextLen)
	{
		//	No string exists or all strings are empty lines will go here
	//	g_A3DErrLog.Log("A3DStringSet::OutputStrings(), Need building before render");
		return true;
	}

	A3DTLVERTEX* pVert;
	float rx, ry, fWid, fHei;
	int i, iNumVert = pSlot->iNumSeg * 4;

	//	Vertex buffer has been full ?
	if (m_aVertBuf && m_iVertCnt + iNumVert > SIZE_VERTEXBUF)
		Flush();
	
	m_pA3DDevice->GetA3DEngine()->BeginPerformanceRecord(A3DEngine::PERF_RENDER_TEXT);

	//	Stream hasn't been locked, lock it.
	if (!m_aVertBuf)
	{
		if (!LockStream(true))
		{
			g_A3DErrLog.Log("A3DStringSet::OutputString(), Failed to lock stream!");
			return false;
		}
	}

	if (!iHeight)
		iHeight = m_iOutHei;

	pVert	= &m_aVertBuf[m_iVertCnt];
	rx		= (float)x;
	ry		= (float)y;
	fHei	= (float)iHeight;

	TEXPOS* pPos = &pSlot->aTexPos[0];
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);
	A3DCOLOR Color = pSlot->dwCol;

	if (pSlot->iNumSeg == 1)	//	Only one line this string hold in texture
	{
		fWid = (float)pSlot->iExtX / m_iLineHei * iHeight;

		pVert[0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u1, pPos->v1);
		pVert[1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u2, pPos->v1);
		pVert[2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u1, pPos->v2);
		pVert[3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u2, pPos->v2);
	
		m_iVertCnt += 4;
		return true;
	}

	//	First segment
	fWid = ((float)m_iTexWid - pSlot->iFirstOff) / m_iLineHei * iHeight;
	
	pVert[0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u1, pPos->v1);
	pVert[1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u2, pPos->v1);
	pVert[2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u1, pPos->v2);
	pVert[3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u2, pPos->v2);
	pPos++;
	
	m_iVertCnt	+= 4;
	pVert		+= 4;
	rx			+= fWid;
	
	//	Middle segments
	fWid = (float)m_iTexWid / m_iLineHei * iHeight;

	for (i=1; i < pSlot->iNumSeg-1; i++)
	{
		pVert[0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u1, pPos->v1);
		pVert[1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u2, pPos->v1);
		pVert[2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u1, pPos->v2);
		pVert[3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u2, pPos->v2);
		pPos++;
		
		m_iVertCnt	+= 4;
		pVert		+= 4;
		rx			+= fWid;
	}

	//	Last segment
	fWid = (float)pSlot->iLastOff / m_iLineHei * iHeight;
	
	pVert[0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u1, pPos->v1);
	pVert[1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u2, pPos->v1);
	pVert[2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u1, pPos->v2);
	pVert[3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pPos->u2, pPos->v2);
	
	m_iVertCnt += 4;

	m_pA3DDevice->GetA3DEngine()->EndPerformanceRecord(A3DEngine::PERF_RENDER_TEXT);

	return true;
}

//	Flush rendering
bool A3DStringSet::Flush()
{
	m_pA3DDevice->GetA3DEngine()->BeginPerformanceRecord(A3DEngine::PERF_RENDER_TEXT);

	if (m_aVertBuf)
		LockStream(false);

	if (!m_iVertCnt)
		return true;

	if (m_bFilter)
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	else
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);

	//	Set render states
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	m_pA3DTexture->Appear(0);
	m_pA3DStream->Appear();

	//	Render
	if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iVertCnt,
											0, m_iVertCnt >> 1))
		return false;

	m_pA3DTexture->Disappear(0);

	//	Restore render state
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

	m_iVertCnt = 0;

	m_pA3DDevice->GetA3DEngine()->EndPerformanceRecord(A3DEngine::PERF_RENDER_TEXT);
	
	return true;
}


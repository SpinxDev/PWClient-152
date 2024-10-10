/*
 * FILE: A3DFont.cpp
 *
 * DESCRIPTION: Routines for display font
 *
 * CREATED BY: duyuxin, 2003/12/5
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DFont.h"
#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DConfig.h"
#include "A3DStream.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DFontMan.h"
#include "A3DViewport.h"
#include "A3DTexture.h"
#include "AMemory.h"
#include "AFI.h"
#include "AFileImage.h"

#define new A_DEBUG_NEW

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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DFont::A3DFont() : m_CharTable(1024)
{
	m_pA3DDevice	= NULL;
	m_pA3DFontMan	= NULL;
	m_pA3DStream	= NULL;
	m_hA3DFont		= 0;
	m_bFilter		= true;
	m_bNoRender		= false;
	m_iIdxBufSize	= 0;
	m_iVertBufSize	= 0;
	m_iSizeRdCache	= 256;
	m_iTexWidth		= 0;
	m_iTexHeight	= 0;
}

A3DFont::~A3DFont()
{
}

/*	Initialize object.

	Return true for success, otherwise return false

	pDevice: address of A3D device.
	hA3DFont: A3D font handle got from A3DFontMan
	iSizeRdCache: size of character render cache, number of characters
	iTexWid, iTexHei: size of each texture
*/	
bool A3DFont::Init(A3DDevice* pDevice, HA3DFONT hA3DFont, int iSizeRdCache, int iTexWid, int iTexHei)
{
	if (!pDevice || !hA3DFont)
		return false;

	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
	{
		m_bNoRender = true;
		return true;
	}
	
	m_pA3DDevice	= pDevice;
	m_hA3DFont		= hA3DFont;
	m_iSizeRdCache	= iSizeRdCache > 32 ? iSizeRdCache : 32;
	m_iVertBufSize	= m_iSizeRdCache * 4;
	m_iIdxBufSize	= m_iSizeRdCache * 6;
	m_iTexWidth		= iTexWid > 64 ? iTexWid : 64;
	m_iTexHeight	= iTexHei > 64 ? iTexHei : 64;

	m_pA3DFontMan	= pDevice->GetA3DEngine()->GetA3DFontMan();
	m_iLineHei		= m_pA3DFontMan->GetFontOutputHeight(hA3DFont);
	m_iFillHei		= 0;
	m_iFillWid		= 0;

	ASSERT(m_iLineHei < m_iTexHeight);

	//	Create stream
	if (!CreateStream())
	{
		g_A3DErrLog.Log("A3DFont::Init, Failed to create stream !");
		return false;
	}

	return true;
}	

//	Release object
void A3DFont::Release()
{
	if (m_bNoRender)
		return;

	//	Release all textures
	for (int i=0; i < m_aTexSlots.GetSize(); i++)
		FreeTextureSlot(m_aTexSlots[i]);

	m_aTexSlots.RemoveAll();

	//	Release all character records
	CharTable::iterator it = m_CharTable.begin();
	for (; it != m_CharTable.end(); ++it)
	{
		CHARPROP* pProp = *it.value();
		delete pProp;
	}

	m_CharTable.clear();

	//	Release stream
	A3DRELEASE(m_pA3DStream);

	m_pA3DDevice = NULL;
}

/*	Create vertices and indices stream

	Return true for success, otherwise return false.
*/
bool A3DFont::CreateStream()
{
	if (!(m_pA3DStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DFont::CreateStream Not Enough Memory!");
		return false;
	}

	if (!m_pA3DStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, m_iVertBufSize, 
		m_iIdxBufSize, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
	{
		g_A3DErrLog.Log("A3DFont::CreateStream m_pA3DStream Init Fail!");
		return false;
	}

	WORD* pVBIndices;
	if (!m_pA3DStream->LockIndexBuffer(0, 0, (LPBYTE*) &pVBIndices, NULL))
	{
		g_A3DErrLog.Log("A3DFont::CreateStream, Fail to lock index buffer!");
		return false;
	}

	for (int n=0, iIdx=0, iVert=0; n < m_iSizeRdCache; n++, iIdx+=6, iVert+=4)
	{
		pVBIndices[iIdx+0] = iVert;
		pVBIndices[iIdx+1] = iVert + 1;
		pVBIndices[iIdx+2] = iVert + 2;
		pVBIndices[iIdx+3] = iVert + 2;
		pVBIndices[iIdx+4] = iVert + 1;
		pVBIndices[iIdx+5] = iVert + 3;
	}

	if (!m_pA3DStream->UnlockIndexBuffer())
	{
		g_A3DErrLog.Log("A3DFont::CreateStream, Fail to unlock index buffer!");
		return false;
	}

	return true;
}

//	Load characters from file and add them to table
bool A3DFont::LoadCharTable(const char* szFile)
{
	if (m_bNoRender)
		return true;

	char szFullFileName[MAX_PATH];
	af_GetFullPath(szFullFileName, szFile);

	AFileImage File;

	if (!File.Open(szFullFileName, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSkin::LoadCharTable, Cannot open file %s!", szFullFileName);
		return false;
	}

	DWORD dwRead;

#ifdef UNICODE

	//	Check unicode file header
	wchar_t wChar;
	if (!File.Read(&wChar, sizeof (wchar_t), &dwRead) || wChar != 0xfeff)
	{
		g_A3DErrLog.Log("A3DFont::LoadCharTable, %s isn't unicode file !", szFile);
		return false;
	}

#else

	//	Shouldn't be unicode file
	wchar_t wChar;
	if (!File.Read(&wChar, sizeof (wchar_t), &dwRead) && wChar == 0xfeff)
	{
		g_A3DErrLog.Log("A3DFont::LoadCharTable, %s shouldn't be unicode file !", szFile);
		return false;
	}

#endif	//	UNICODE

	ACHAR* pBuf = (ACHAR*)File.GetFileBuffer();
	if (!pBuf)
		return false;

	ACHAR* pEnd = (ACHAR*)((BYTE*)pBuf + File.GetFileLength());

	//	Alloclate temporary buffer
	ACHAR* pNewChars = (ACHAR*)a_malloc(File.GetFileLength());
	if (!pNewChars)
	{
		g_A3DErrLog.Log("A3DFont::LoadCharTable, Not enough memory !");
		return false;
	}

	int iNewCharCnt = 0;

#ifdef UNICODE

	//	Skip unicode file header
	pBuf++;

#endif	//	UNICODE

	while (pBuf < pEnd)
	{
		while (pBuf < pEnd && *pBuf < 32)
			pBuf++;

		if (pBuf >= pEnd)
			break;

		ACHAR* pLineStart = pBuf;

		while (pBuf < pEnd && *pBuf != '\n')
			pBuf++;

		ACHAR* pLineEnd = pBuf;

		while (pLineStart < pLineEnd)
		{
			if (*pLineStart >= 32)
				pNewChars[iNewCharCnt++] = *pLineStart;

			pLineStart++;
		}

		pBuf = pLineEnd;
	}

	AddCharsToTable(pNewChars, iNewCharCnt);

	a_free(pNewChars);

	File.Close();

	return true;
}

//	Save character table to file
bool A3DFont::SaveCharTable(const char* szFile)
{
	int iNumChar = m_CharTable.size();
	if (!iNumChar)
		return true;

	FILE* fp = fopen(szFile, "wb+");
	if (!fp)
	{
		g_A3DErrLog.Log("A3DFont::SaveCharTable, Failed to create file %s !", szFile);
		return false;
	}

#ifdef UNICODE
	
	//	Write unicode file header
	wchar_t wChar = 0xfeff;
	fwrite(&wChar, 1, sizeof (wchar_t), fp);

#endif

	//	Alloclate temporary buffer
	ACHAR* pChars = new ACHAR [iNumChar];
	if (!pChars)
	{
		g_A3DErrLog.Log("A3DFont::LoadCharTable, Not enough memory !");

		fclose(fp);
		return false;
	}

	int iCharCnt = 0;

	//	Release all character records
	CharTable::iterator it = m_CharTable.begin();
	for (; it != m_CharTable.end(); ++it)
	{
		CHARPROP* pProp = *it.value();

		pChars[iCharCnt++] = pProp->ch;
	}

	fwrite(pChars, 1, iCharCnt * sizeof (ACHAR), fp);

	delete [] pChars;

	fclose(fp);

	return true;
}

//	Allocate a texture slot
A3DFont::TEXTURESLOT* A3DFont::AllocTextureSlot()
{
	TEXTURESLOT* pSlot = new TEXTURESLOT;
	if (!pSlot)
		return NULL;

	//	Create texture
	if (!(pSlot->pTexture = new A3DTexture))
	{
		FreeTextureSlot(pSlot);
		g_A3DErrLog.Log("A3DFont::AllocTextureSlot, Failed to create texture object.");
		return false;
	}

	//	Create texture
	if (!pSlot->pTexture->Create(	m_pA3DDevice, 
									m_iTexWidth, 
									m_iTexHeight, 
									A3DFMT_A8R8G8B8, 
									1,
									m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag() ? D3DUSAGE_DYNAMIC : 0))
	{
		FreeTextureSlot(pSlot);
		g_A3DErrLog.Log("A3DFont::AllocTextureSlot, failed to create texture.");
		return false;
	}

	//	Create vertex buffer
	if (!(pSlot->pVertBuf = (A3DTLVERTEX*)a_malloc(m_iVertBufSize * sizeof (A3DTLVERTEX))))
	{
		FreeTextureSlot(pSlot);
		g_A3DErrLog.Log("A3DFont::AllocTextureSlot, failed to vertex buffer.");
		return false;
	}

	pSlot->iVertCnt	= 0;
	pSlot->iIdxCnt	= 0;

	return pSlot;
}

//	Free a texture slot
void A3DFont::FreeTextureSlot(TEXTURESLOT* pSlot)
{
	if (!pSlot)
		return;

	if (pSlot->pTexture)
	{
		pSlot->pTexture->Release();
		delete pSlot->pTexture;
	}

	if (pSlot->pVertBuf)
		a_free(pSlot->pVertBuf);

	delete pSlot;
}

//	Add new characters to char table
bool A3DFont::AddCharsToTable(const ACHAR* szText, int iNumChar)
{
	if (m_bNoRender)
		return true;

	int i;
	APtrArray<CHARPROP*> aNewChars;
	float fInvTexWid = 1.0f / m_iTexWidth;
	float fInvTexHei = 1.0f / m_iTexHeight;

	m_pA3DFontMan->BeginGetTextExtent(m_hA3DFont);

	for (i=0; i < iNumChar && szText[i]; i++)
	{
		ACHAR ch = szText[i];

		CharTable::pair_type Pair = m_CharTable.get(ch);
		if (Pair.second)	//	Character has been added
			continue;

		CHARPROP* pProp = new CHARPROP;
		if (!pProp)
		{
			m_pA3DFontMan->EndGetTextExtent();
			return true;
		}

		pProp->ch = ch;

		//	Calcualte char's position on texture
		if (!CalculateCharPos(pProp, ch))
		{
			m_pA3DFontMan->EndGetTextExtent();
			return false;
		}

		pProp->tu1 = pProp->rect.left * fInvTexWid;
		pProp->tv1 = pProp->rect.top * fInvTexHei;
		pProp->tu2 = pProp->rect.right * fInvTexWid;
		pProp->tv2 = pProp->rect.bottom * fInvTexHei;

		//	Add to char-table
		m_CharTable.put(ch, pProp);

		aNewChars.Add(pProp);
	}

	m_pA3DFontMan->EndGetTextExtent();

	//	Print new characters on textures
	if (aNewChars.GetSize())
		PrintCharsOnTextures(aNewChars);

	return true;
}

//	Print character onto texture
bool A3DFont::CalculateCharPos(CHARPROP* pProp, ACHAR ch)
{
	int iCharWid, iCharHei;
	m_pA3DFontMan->GetTextExtentBE(&ch, 1, &iCharWid, &iCharHei);

	ASSERT(iCharWid + 1 < m_iTexWidth);

	if (m_aTexSlots.GetSize())
	{
		int iTexture = m_aTexSlots.GetSize() - 1;

		if (m_iFillWid + iCharWid + 1 <= m_iTexWidth)
		{
			pProp->iTexPage = iTexture;
			pProp->rect.SetRect(m_iFillWid, m_iFillHei, m_iFillWid+iCharWid, m_iFillHei+m_iLineHei);
			m_iFillWid += iCharWid + 1;
			return true;
		}

		//	Change line
		m_iFillHei += m_iLineHei + 1;
		m_iFillWid  = 0;

		if (m_iFillHei + m_iLineHei + 1 <= m_iTexHeight)
		{
			pProp->iTexPage = iTexture;
			pProp->rect.SetRect(m_iFillWid, m_iFillHei, m_iFillWid+iCharWid, m_iFillHei+m_iLineHei);
			m_iFillWid += iCharWid + 1;
			return true;
		}
	}

	//	Create a new texture
	TEXTURESLOT* pSlot = AllocTextureSlot();
	if (!pSlot)
	{
		g_A3DErrLog.Log("A3DFont::CalculateCharPos, Failed to create texture slot.");
		return false;
	}

	m_aTexSlots.Add(pSlot);

	m_iFillHei = 0;
	m_iFillWid = 0;

	pProp->iTexPage = m_aTexSlots.GetSize() - 1;
	pProp->rect.SetRect(m_iFillWid, m_iFillHei, m_iFillWid+iCharWid, m_iFillHei+m_iLineHei);
	m_iFillWid += iCharWid + 1;

	return true;
}

//	Print new characters on textures
bool A3DFont::PrintCharsOnTextures(APtrArray<CHARPROP*>& aChars)
{
	int iBmpPitch, iBmpHei;
	HBITMAP hBitmap;

	//	Create DC bitmap
	BYTE* pDIBData = CreateDCBitmap(aChars, &hBitmap, &iBmpPitch, &iBmpHei);
	if (!pDIBData)
		return false;

	HDC	hDC	= m_pA3DFontMan->GetWindowsDC();
	HBITMAP hOldBmp	= (HBITMAP)SelectObject(hDC, hBitmap);

	//	Select font
	HFONT hFont		= m_pA3DFontMan->GetWindowsFont(m_hA3DFont);
	HFONT hOldFont	= (HFONT)SelectObject(hDC, hFont);

	//	Set text properties
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, 0);
	SetTextAlign(hDC, TA_TOP);

	//	Select font into screen DC so that anti-aliased can do effect
	HDC hScreenDC = GetDC(NULL);
	HFONT hOldSFont = (HFONT)SelectObject(hScreenDC, hFont);

	int i, j, iTexPitch, iNumChar = aChars.GetSize();
	BYTE* pTexBuf;
	bool bRet = true;

	for (i=0; i < iNumChar; i++)
	{
		CHARPROP* pProp = aChars[i];
		TEXTURESLOT* pSlot = m_aTexSlots[pProp->iTexPage];

		if (!pSlot->pTexture->LockRectDynamic(NULL, (void**) &pTexBuf, &iTexPitch, 0))
		{
			g_A3DErrLog.Log("A3DFont::PrintCharsOnTextures failed to lock texture.");
			bRet = false;
			break;
		}
		
		//	Print all character belong to this texture
		for (j=i; j < iNumChar; j++)
		{
			CHARPROP* p = aChars[j];
			if (p->iTexPage != pProp->iTexPage)
				break;
	
			//	Clear bitmap
			memset(pDIBData, 0, iBmpHei * iBmpPitch);

			//	Print character
			ExtTextOut(hDC, 0, 0, ETO_OPAQUE, NULL, &p->ch, 1, NULL);

			//	Consider right and bottom border
			int cx = p->rect.Width() + 1;
			int cy = p->rect.Height() + 1;
	
			PrintCharOnTexture(pTexBuf, &p->rect, iTexPitch, pDIBData, cx, cy, iBmpPitch);
		}

		pSlot->pTexture->UnlockRectDynamic();

		//	Skip printed characters
		i = j - 1;
	}

	SelectObject(hScreenDC, hOldSFont);
	ReleaseDC(NULL, hScreenDC);

	//	Release resources
	SelectObject(hDC, hOldFont);
	SelectObject(hDC, hOldBmp);
	DeleteObject(hBitmap);

	return bRet;
}

//	Create DC's bitamp
BYTE* A3DFont::CreateDCBitmap(APtrArray<CHARPROP*>& aChars, HBITMAP* phBmp, int* piPitch, int* piHeight)
{
	int i, cx=0, cy=0, iNumChar=aChars.GetSize();

	m_pA3DFontMan->BeginGetTextExtent(m_hA3DFont);

	for (i=0; i < iNumChar; i++)
	{
		CHARPROP* pProp = aChars[i];
		int x = pProp->rect.Width();
		int y = pProp->rect.Height();

		if (x > cx)	cx = x;
		if (y > cy) cy = y;
	}

	ASSERT(cx > 0 && cy > 0);

	//	Consider right and bottom borders
	cx++;
	cy++;

	m_pA3DFontMan->EndGetTextExtent();

	//	Create bitmap
	BYTE* pDIBData;

	BITMAPINFO bmi;
	memset(&bmi.bmiHeader, 0, sizeof (BITMAPINFOHEADER));
	bmi.bmiHeader.biSize        = sizeof (BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       = cx;
	bmi.bmiHeader.biHeight      = -cy;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;

	HDC	hDC	= m_pA3DFontMan->GetWindowsDC();
	*phBmp = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (VOID**) &pDIBData, NULL, 0);

	*piPitch	= ((cx * 4 + 3) >> 2) << 2;
	*piHeight	= cy;

	return pDIBData;
}

//	Print a character on texture
void A3DFont::PrintCharOnTexture(BYTE* pDst, ARectI* pDstRect, int iDstPitch, BYTE* pSrc, 
								 int cx, int cy, int iSrcPitch)
{
	int i, j;

	// It outputs per glyph pixel data on the texture with D3DFMT_A8R8G8B8 format. Don't forget to change the code here if you changed the texture format.
	pDst = pDst + pDstRect->top * iDstPitch + pDstRect->left * 4;

	for (i=0; i < cy; i++)
	{
		DWORD* pDstPixel = (DWORD*)pDst;
		DWORD* pSrcPixel = (DWORD*)pSrc;

		for (j=0; j < cx; j++, pDstPixel++, pSrcPixel++)
		{
			BYTE byAlpha = (*((BYTE*)pSrcPixel) & 0xff);
			if (byAlpha > 0)
				*pDstPixel = (byAlpha << 24) | 0xffffff;
			else
				*pDstPixel = 0;
		}

		pDst += iDstPitch;
		pSrc += iSrcPitch;
	}
}

/*	Textout text using map file font.

	x, y: text string's left-up corner in screen coordinates
	szText: text string will be output
	color: text's color
	iHeight: output height of text. 0 means to use font height.
	z: z value of text board
	bBorder: true, add a 1-pixel black border
*/
bool A3DFont::TextOut(int x, int y, const ACHAR* szText, A3DCOLOR color, 
					  int iHeight/* 0 */, float z/* 0.0f */, bool bBorder/* false */)
{
	if (!m_pA3DDevice || m_bNoRender)
		return true;

	int i, iLen = a_strlen(szText);

	//	Add unregistered characters
	AddCharsToTable(szText, iLen);

	if (!iHeight)
		iHeight = m_pA3DFontMan->GetFontOutputHeight(m_hA3DFont);

	float fHei		= (float)iHeight;
	float rx		= (float)x;
	float ry		= (float)y;
	float fScale	= fHei / m_iLineHei;

	A3DTLVERTEX aVerts[4];
	DWORD dwSpecular = A3DCOLORRGBA(0, 0, 0, 255);

	if (!bBorder)
	{
		for (i=0; i < iLen; i++)
		{
			CharTable::pair_type Pair = m_CharTable.get(szText[i]);
			if (!Pair.second)	//	Don't find this character !
				continue;

			CHARPROP* pProp = *Pair.first;

			float fWid = pProp->rect.Width() * fScale;
			
			aVerts[0].Set(A3DVECTOR4(rx-0.5f, ry-0.5f, z, 1.0f), color, dwSpecular, pProp->tu1, pProp->tv1);
			aVerts[1].Set(A3DVECTOR4(rx+fWid-0.5f, ry-0.5f, z, 1.0f), color, dwSpecular, pProp->tu2, pProp->tv1);
			aVerts[2].Set(A3DVECTOR4(rx-0.5f, ry+fHei-0.5f, z, 1.0f), color, dwSpecular, pProp->tu1, pProp->tv2);
			aVerts[3].Set(A3DVECTOR4(rx+fWid-0.5f, ry+fHei-0.5f, z, 1.0f), color, dwSpecular, pProp->tu2, pProp->tv2);

			AddVertsToOutputBuf(pProp->iTexPage, aVerts, 1);

			rx += fWid;
		}
	}
	else	//	Draw border
	{
		struct BORDER
		{
			int x, y;	//	Offset of x and y
			DWORD col;	//	Color
		};

		int g = int(A3DCOLOR_GETRED(color) * 0.3f + A3DCOLOR_GETGREEN(color) * 0.5f + A3DCOLOR_GETBLUE(color) * 0.2f);
		if( g > 100 ) g = 0;
		else g = 255;
		DWORD bc = (color & 0xff000000) | A3DCOLORRGB(g, g, g);

		BORDER aBorders[9] = 
		{
			{1, 1, bc}, {-1, -1, bc}, {1, -1, bc}, {-1, 1, bc},
			{1, 0, bc}, {-1, 0, bc}, {0, -1, bc}, {0, 1, bc},
			{0, 0, color}
		};

		for (i=0; i < iLen; i++)
		{
			CharTable::pair_type Pair = m_CharTable.get(szText[i]);
			if (!Pair.second)	//	Don't find this character !
				continue;

			CHARPROP* pProp = *Pair.first;
			float fWid = pProp->rect.Width() * fScale;

			for (int j=0; j < 9; j++)
			{
				const BORDER& b = aBorders[j];
				int tx = int(rx + b.x);
				int ty = int(ry + b.y);

				aVerts[0].Set(A3DVECTOR4(tx-0.5f, ty-0.5f, z, 1.0f), b.col, dwSpecular, pProp->tu1, pProp->tv1);
				aVerts[1].Set(A3DVECTOR4(tx+fWid-0.5f, ty-0.5f, z, 1.0f), b.col, dwSpecular, pProp->tu2, pProp->tv1);
				aVerts[2].Set(A3DVECTOR4(tx-0.5f, ty+fHei-0.5f, z, 1.0f), b.col, dwSpecular, pProp->tu1, pProp->tv2);
				aVerts[3].Set(A3DVECTOR4(tx+fWid-0.5f, ty+fHei-0.5f, z, 1.0f), b.col, dwSpecular, pProp->tu2, pProp->tv2);

				AddVertsToOutputBuf(pProp->iTexPage, aVerts, 1);
			}

			rx += fWid;
		}
	}

	return true;
}

/*	Draw text using map file font.

	x, y: text string's left-up corner in screen coordinates
	szText: text string will be output
	color: text's color
	pRect: Clipping area not including the right column and bottom row. 
	iHeight: output height of text. 0 means to use font height. 
	z: z value of text board
*/
bool A3DFont::DrawText(int x, int y, const ACHAR* szText, A3DCOLOR color, A3DRECT* pRect, 
					   int iHeight/* 0 */, float z/* 0.0f */)
{
	if (!m_pA3DDevice || m_bNoRender)
		return true;

	if (!pRect)
		return TextOut(x, y, szText, color, iHeight);

	if (!iHeight)
		iHeight = m_pA3DFontMan->GetFontOutputHeight(m_hA3DFont);

	if (y >= pRect->bottom || y+iHeight <= pRect->top || x >= pRect->right)
		return true;

	int i, iLen = a_strlen(szText);

	//	Add unregistered characters
	AddCharsToTable(szText, iLen);

	float fHei	= (float)iHeight;
	float rx	= (float)x;
	float ry	= (float)y;

	float fInvTexWid = 1.0f / m_iTexWidth;
	float fInvTexHei = 1.0f / m_iTexHeight;

	ARectF rc1(0.0f, 0.0f, 0.0f, 0.0f);
	ARectF rc2(0.0f, 0.0f, 0.0f, 0.0f);

	if (y < pRect->top)
	{
		rc1.top = (float)pRect->top;
		rc2.top = (pRect->top - ry) / fHei * m_iLineHei * fInvTexHei;
	}
	else
		rc1.top = ry;
	
	if (ry + fHei >= (float)pRect->bottom)
	{
		rc1.bottom = (float)pRect->bottom;
		rc2.bottom = -(ry + fHei - pRect->bottom) / fHei * m_iLineHei * fInvTexHei;
	}
	else
		rc1.bottom = ry + fHei;
		
	A3DTLVERTEX aVerts[4];
	DWORD dwSpecular = A3DCOLORRGBA(0, 0, 0, 255);
	A3DVECTOR4 v;

	for (i=0; i < iLen; i++)
	{
		CharTable::pair_type Pair = m_CharTable.get(szText[i]);
		if (!Pair.second)	//	Don't find this character !
			continue;

		CHARPROP* pProp = *Pair.first;

		float fWid = (float)pProp->rect.Width() * iHeight / m_iLineHei;
		if (rx >= (float)pRect->right)
			break;
		
		if (rx + fWid <= (float)pRect->left)
		{
			rx += fWid;
			continue;
		}

		rc1.left = rc1.right = 0.0f;
		rc2.left = rc2.right = 0.0f;

		if (rx < (float)pRect->left)
		{
			rc1.left = (float)pRect->left;
			rc2.left = (pRect->left - rx) / fWid * pProp->rect.Width() * fInvTexWid;
		}
		else
			rc1.left = rx;

		if (rx + fWid >= (float)pRect->right)
		{
			rc1.right = (float)pRect->right;
			rc2.right = -(rx + fWid - pRect->right) / fWid * pProp->rect.Width() * fInvTexWid;
		}
		else
			rc1.right = rx + fWid;

		v.Set(rc1.left-0.5f, rc1.top-0.5f, z, 1.0f);
		aVerts[0].Set(v, color, dwSpecular, pProp->tu1+rc2.left, pProp->tv1+rc2.top);

		v.Set(rc1.right-0.5f, rc1.top-0.5f, z, 1.0f);
		aVerts[1].Set(v, color, dwSpecular, pProp->tu2+rc2.right, pProp->tv1+rc2.top);

		v.Set(rc1.left-0.5f, rc1.bottom-0.5f, z, 1.0f);
		aVerts[2].Set(v, color, dwSpecular, pProp->tu1+rc2.left, pProp->tv2+rc2.bottom);

		v.Set(rc1.right-0.5f, rc1.bottom-0.5f, z, 1.0f);
		aVerts[3].Set(v, color, dwSpecular, pProp->tu2+rc2.right, pProp->tv2+rc2.bottom);

		AddVertsToOutputBuf(pProp->iTexPage, aVerts, 1);

		rx += fWid;
	}

	return true;
}

/*	3D text out

	Return true for success, otherwise return false

	szText: string will be outputed
	pView: current viewport.
	vCenter: string's center position in world space
	iHeight: output text height on screen
	color: text color including alpha channel
*/
bool A3DFont::TextOut3D(const ACHAR* szText, A3DViewport* pView, const A3DVECTOR3& vCenter,
						int iHeight, A3DCOLOR color)
{
	if (!m_pA3DDevice || m_bNoRender)
		return true;

	A3DCameraBase* pCamera = pView->GetCamera();

	//	Transfrom center from world to screen
	A3DVECTOR3 vScrCenter;
	pView->Transform(vCenter, vScrCenter);

	if (vScrCenter.z < 0.0f || vScrCenter.z > 1.0f)
		return true;

	//	Get text extents in normal height
	int iExtX, iExtY, iTextLen = a_strlen(szText);
	m_pA3DFontMan->GetTextExtent(m_hA3DFont, szText, iTextLen, &iExtX, &iExtY);

	if (!iHeight)
		iHeight = iExtY;

	float fScale  = (float)iHeight / iExtY;
	float fScrWid = iExtX * fScale;

	//	Text string left-top corner on screen. We must use floor to ensure
	//	rx, ry are interger value, otherwise text will be blur
	float rx = (float)floor(vScrCenter.x - fScrWid * 0.5f);
	float ry = (float)floor(vScrCenter.y - iHeight * 0.5f);

	//	If string is out of screen, don't render it
	A3DVIEWPORTPARAM* pViewPara = pView->GetParam();
	if (rx + fScrWid < 0.0f || ry + iHeight < 0.0f ||
		rx > (float)pViewPara->Width || ry > (float)pViewPara->Height)
		return true;

	//	Add unregistered characters
	AddCharsToTable(szText, iTextLen);

	float fHei = (float)iHeight;

	A3DTLVERTEX aVerts[4];
	DWORD dwSpecular = A3DCOLORRGBA(0, 0, 0, 255);

	for (int i=0; i < iTextLen; i++)
	{
		CharTable::pair_type Pair = m_CharTable.get(szText[i]);
		if (!Pair.second)	//	Don't find this character !
			continue;

		CHARPROP* pProp = *Pair.first;

		float fWid = pProp->rect.Width() * fScale;
		
		aVerts[0].Set(A3DVECTOR4(rx-0.5f, ry-0.5f, vScrCenter.z, 1.0f), color, dwSpecular, pProp->tu1, pProp->tv1);
		aVerts[1].Set(A3DVECTOR4(rx+fWid-0.5f, ry-0.5f, vScrCenter.z, 1.0f), color, dwSpecular, pProp->tu2, pProp->tv1);
		aVerts[2].Set(A3DVECTOR4(rx-0.5f, ry+fHei-0.5f, vScrCenter.z, 1.0f), color, dwSpecular, pProp->tu1, pProp->tv2);
		aVerts[3].Set(A3DVECTOR4(rx+fWid-0.5f, ry+fHei-0.5f, vScrCenter.z, 1.0f), color, dwSpecular, pProp->tu2, pProp->tv2);

		AddVertsToOutputBuf(pProp->iTexPage, aVerts, 1);

		rx += fWid;
	}

	return true;
}

//	Add character vertices to vertex buffer waiting for rendering
bool A3DFont::AddVertsToOutputBuf(int iSlot, A3DTLVERTEX* aVerts, int iNumChar)
{
	TEXTURESLOT* pSlot = m_aTexSlots[iSlot];
	ASSERT(pSlot->pVertBuf);

	int iNumVert = iNumChar << 2;
	int iNumIdx = iNumChar * 6;

	//	Check whether vertex buffer has been full ?
	if (pSlot->iVertCnt + iNumVert > m_iVertBufSize || 
		pSlot->iIdxCnt + iNumIdx > m_iIdxBufSize)
		FlushOutputBuf(iSlot);

	//	Fill vertex buffer
	A3DTLVERTEX* pVert = &pSlot->pVertBuf[pSlot->iVertCnt];
	memcpy(pVert, aVerts, iNumVert * sizeof (A3DTLVERTEX));

	pSlot->iVertCnt	+= iNumVert;
	pSlot->iIdxCnt	+= iNumIdx;

	return true;
}

//	Flush stream
bool A3DFont::FlushOutputBuf(int iSlot)
{
	m_pA3DDevice->GetA3DEngine()->BeginPerformanceRecord(A3DEngine::PERF_RENDER_TEXT);
	
	TEXTURESLOT* pSlot = m_aTexSlots[iSlot];
	if (!pSlot->iVertCnt || !pSlot->iIdxCnt)
		return true;
	
	ASSERT(m_pA3DStream && pSlot->pVertBuf);

	int iNumVert = pSlot->iVertCnt;
	int iNumIdx	 = pSlot->iIdxCnt;
	pSlot->iVertCnt = 0;
	pSlot->iIdxCnt	= 0;

	//	Lock stream
	A3DTLVERTEX* pDestBuf;
	DWORD dwLockSize = iNumVert * sizeof (A3DTLVERTEX);
	if (!m_pA3DStream->LockVertexBufferDynamic(0, dwLockSize, (BYTE**) &pDestBuf, 0))
	{
		g_A3DErrLog.Log("A3DFont::FlushOutputBuf, failed to lock stream.");
		return false;
	}

	//	Copy data
	memcpy(pDestBuf, pSlot->pVertBuf, dwLockSize);

	//	Unlock stream
	m_pA3DStream->UnlockVertexBufferDynamic();

	//	Set render states
	if (m_bFilter)
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	else
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);

//	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	pSlot->pTexture->Appear(0);
	m_pA3DStream->Appear();

	//	Render
	if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, iNumVert,
											0, iNumIdx / 3))
		return false;

	//	Restore render state
	pSlot->pTexture->Disappear(0);

	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

	m_pA3DDevice->GetA3DEngine()->EndPerformanceRecord(A3DEngine::PERF_RENDER_TEXT);

	return true;
}

//	Flush all streams
bool A3DFont::Flush()
{
	if (!m_pA3DDevice || m_bNoRender)
		return true;

	for (int i=0; i < m_aTexSlots.GetSize(); i++)
		FlushOutputBuf(i);

	return true;
}

//	Reset streams, all unrendered characters will be discarded
bool A3DFont::ResetRenderBuffer()
{
	if (m_bNoRender)
		return true;

	for (int i=0; i < m_aTexSlots.GetSize(); i++)
	{
		TEXTURESLOT* pSlot = m_aTexSlots[i];
		pSlot->iVertCnt	= 0;
		pSlot->iIdxCnt	= 0;
	}

	return true;
}

//	Get text string's extent
bool A3DFont::GetTextExtent(const ACHAR* szText, int iNumChar, int* piWid, int* piHei)
{
	return m_pA3DFontMan->GetTextExtent(m_hA3DFont, szText, iNumChar, piWid, piHei);
}

//	Get font's height
int A3DFont::GetFontHeight()
{
	return m_pA3DFontMan->GetFontHeight(m_hA3DFont);
}

//	Get font's output height
int A3DFont::GetFontOutputHeight()
{
	return m_pA3DFontMan->GetFontOutputHeight(m_hA3DFont);
}

//	Get font's flags
DWORD A3DFont::GetFontFlags()
{
	return m_pA3DFontMan->GetFontFlags(m_hA3DFont);
}


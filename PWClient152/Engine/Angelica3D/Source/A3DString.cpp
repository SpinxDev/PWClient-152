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

#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DString.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DConfig.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "A3DFontMan.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "AChar.h"
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

A3DString::A3DString()
{
	m_pA3DDevice	= NULL;
	m_pA3DStream	= NULL;
	m_pA3DTexture	= NULL;
	m_hA3DFont		= NULL;

	m_bHWIString	= false;
	m_bReady		= false;

	m_iTexWid		= 0;
	m_iTexHei		= 0;
	m_iTextExtX		= 0;
	m_iTextExtY		= 0;
	m_aCharWids		= NULL;
	m_iTextLen		= 0;
	m_iOutHei		= 16;
	m_szText		= NULL;
}

A3DString::~A3DString()
{
}

/*	Initialize object.

	Return true for success, otherwise return false.

	pDevice: valid A3DDevice object's address.
	szText: text string
	hA3DFont: A3D font handle got from A3DFontMan
	bFilter: true, use antiliase font, 
*/
bool A3DString::Init(A3DDevice* pDevice, const ACHAR* szText, HA3DFONT hA3DFont, bool bFilter/* true */)
{
	if (!hA3DFont)
		return false;

	if (!szText || !szText[0])
		return true;

	m_pA3DDevice	= pDevice;
	m_bFilter		= bFilter;
	m_hA3DFont		= hA3DFont;

	//	Pure server needn't rendering
	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
	{
		m_bHWIString = true;
		m_bReady	 = true;
		return true;
	}

	//	Copy string
	m_iTextLen = a_strlen(szText);
	if (!(m_szText = new ACHAR [m_iTextLen + 1]))
		return false;

	a_strcpy(m_szText, szText);

	SIZE Size;
	A3DFontMan* pFontMan = pDevice->GetA3DEngine()->GetA3DFontMan();

	//	Calculate texture size
	if (!CalculateTextureSize(pFontMan, &Size))
	{
		g_A3DErrLog.Log("A3DString::Init invalid size.");
		Release();
		return false;
	}

	m_iTextExtX	= Size.cx;
	m_iTextExtY	= Size.cy;
	m_iOutHei	= m_iLineHei;

	//	Create texture
	if (!(m_pA3DTexture = new A3DTexture()))
	{
		g_A3DErrLog.Log("A3DString::Init no memory for texture.");
		Release();
		return false;
	}

	if (!m_pA3DTexture->Create(pDevice, m_iTexWid, m_iTexHei, A3DFMT_A4R4G4B4))
	{
		g_A3DErrLog.Log("A3DString::Init failed to create new texture.");
		Release();
		return false;
	}

	HBITMAP	hBitmap, hOldBmp;	//	Memory bitmap's handle
	HDC	hDC;					//	Memory DC's handle
	HFONT hFont, hOldFont;		//	Font's handle
	WORD* pDIBData;				//	Address of memory bitmap's data

	//	Get windows's DC and font from A3DFontMan
	hDC		 = pFontMan->GetWindowsDC();
	hFont	 = pFontMan->GetWindowsFont(hA3DFont);
	hOldFont = (HFONT)SelectObject(hDC, hFont);

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
	hOldBmp = (HBITMAP)SelectObject(hDC, hBitmap);

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
	ExtTextOut(hDC, 0, 0, ETO_OPAQUE, NULL, m_szText, a_strlen(m_szText), NULL);

	SelectObject(hScreenDC, hOldSFont);
	ReleaseDC(NULL, hScreenDC);

	//	Fill texture
	FillTexture((BYTE*)pDIBData, iPitch);

	//	Release resources
	SelectObject(hDC, hOldFont);
	SelectObject(hDC, hOldBmp);

	DeleteObject(hBitmap);

	//	Create vertices and indices buffer
	if (!CreateStream())
	{
		g_A3DErrLog.Log("A3DString::Init failed to create stream.");
		Release();
		return false;
	}

	m_bReady = true;

	m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_STRING);

	return true;
}

//	Release object
void A3DString::Release()
{
	if (m_pA3DTexture)
	{
		m_pA3DTexture->Release();
		delete m_pA3DTexture;
		m_pA3DTexture = NULL;
	}

	if (m_pA3DStream)
	{
		m_pA3DStream->Release();
		delete m_pA3DStream;
		m_pA3DStream = NULL;
	}

	if (m_aCharWids)
	{
		free(m_aCharWids);
		m_aCharWids = NULL;
	}

	if (m_szText)
	{
		delete [] m_szText;
		m_szText = NULL;
		m_iTextLen = 0;
	}

	if (m_pA3DDevice)
		m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DEngine::OBJECTCNT_STRING);

	m_bReady		= false;
	m_pA3DDevice	= NULL;
	m_hA3DFont		= NULL;
}

/*	Calculate texture size. This function will conside texture to be square

	Return true for success, otherwise return false.

	pFontMan: A3D font manager
	hDC: handle of DC
	pSize: used to receive text string's size
*/
bool A3DString::CalculateTextureSize(A3DFontMan* pFontMan, SIZE* pSize)
{
	int i, iArea, iMaxLen, iWid, iHei, cx, cy;

	pFontMan->GetTextExtent(m_hA3DFont, m_szText, m_iTextLen, &cx, &cy);
	iMaxLen = (cx > cy) ? cx : cy;

	iArea	= cx * cy;
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
	m_iNumLines = cx / m_iTexWid + 1;
	m_iLineHei	= cy;

	//	Check again
	if (m_iNumLines * m_iLineHei > m_iTexHei)
		m_iTexHei <<= 1;

	m_iLastWid  = cx - (m_iNumLines-1) * m_iTexWid;
	pSize->cx	= cx;
	pSize->cy	= cy;

	return true;
}

/*	Fill text's texture.

	Return true for success, otherwise return false.

	pDIBData: address of DIB which contains text.
	iPitch: pitch of text image (in bytes)
*/
bool A3DString::FillTexture(BYTE* pDIBData, int iPitch)
{
	int i, j, k, iTexPitch, x;
	BYTE* pTexBuf, *pDest, *pSrc, byAlpha;
	WORD *pDestPixel;
	DWORD *pSrcPixel;

	if (!m_pA3DTexture->LockRect(NULL, (void**) &pTexBuf, &iTexPitch, 0))
		return false;
	
	x	  = 0;
	pDest = pTexBuf;
	pSrc  = pDIBData;

	for (i=0; i < m_iNumLines-1; i++)
	{
		pSrc = pDIBData + (x << 2);

		for (j=0; j < m_iLineHei; j++)
		{
			pDestPixel	= (WORD*)pDest;
			pSrcPixel	= (DWORD*)pSrc;

			for (k=0; k < m_iTexWid; k++, pDestPixel++, pSrcPixel++)
			{
				byAlpha = (*((BYTE*)pSrcPixel) & 0xff) >> 4;
				if (byAlpha > 0)
					*pDestPixel = (byAlpha << 12) | 0x0fff;
				else
					*pDestPixel = 0;
			}

			pDest += iTexPitch;
			pSrc  += iPitch;
		}

		x += m_iTexWid;
	}

	//	Handle the last line
	pSrc = pDIBData + (x << 2);

	for (j=0; j < m_iLineHei; j++)
	{
		pDestPixel	= (WORD*)pDest;
		pSrcPixel	= (DWORD*)pSrc;

		for (k=0; k < m_iLastWid; k++, pDestPixel++, pSrcPixel++)
		{
			byAlpha = (*((BYTE*)pSrcPixel) & 0xff) >> 4;
			if (byAlpha > 0)
				*pDestPixel = (byAlpha << 12) | 0x0fff;
			else
				*pDestPixel = 0;
		}

		pDest += iTexPitch;
		pSrc  += iPitch;
	}

	if (!m_pA3DTexture->UnlockRect())
		return false;

	return true;
}

bool A3DString::CreateStream()
{
	//	Create vertex and index stream
	if (!(m_pA3DStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DString::CreateStream() not enough memory for stream!");
		return false;
	}

	if (!m_pA3DStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, m_iNumLines * 4, 
		m_iNumLines * 6, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
	{
		g_A3DErrLog.Log("A3DString::CreateStream(), Failed to initialize stream!");
		return false;
	}

	//	fill indices
	WORD* pIndices;
	if (!m_pA3DStream->LockIndexBuffer(0, 0, (BYTE**)&pIndices, 0))
	{
		g_A3DErrLog.Log("A3DString::CreateStream(), Failed to lock index buffer!");
		return false;
	}

	int i, n, m;

	for (i=0, n=0, m=0; i < m_iNumLines; i++, n+=6, m+=4)
	{
		pIndices[n+0] = m + 0;
		pIndices[n+1] = m + 1;
		pIndices[n+2] = m + 2;
		pIndices[n+3] = m + 1;
		pIndices[n+4] = m + 3;
		pIndices[n+5] = m + 2;
	}

	if (!m_pA3DStream->UnlockIndexBuffer())
	{
		g_A3DErrLog.Log("A3DString::CreateStream(), Failed to unlock index buffer!");
		return false;
	}

	return true;
}

/*	Get sub-string's  extent

	Return true for success, otherwise return false.

	iStart: the first character's index in origin string
	iEnd: the last character's index in origin string
	piWid (out): used to receive sub-string's width extent, can be NULL
	piHei (out): used to receive sub-string's height extent, can be NULL

	Note: iEnd must >= iStart;
*/
bool A3DString::GetCharsExtent(int iStart, int iEnd, int* piWid, int* piHei)
{
	if (m_bHWIString)
	{
		*piWid = 0;
		*piHei = 0;
		return true;
	}

	if (!m_bReady || iStart < 0 || iEnd < 0 || iStart >= m_iTextLen ||
		iEnd >= m_iTextLen || iStart > iEnd)
		return false;

	int i, iWid = 0;

	for (i=iStart; i <= iEnd; i++)
		iWid += m_aCharWids[i];

	if (piWid)
		*piWid = iWid;

	if (piHei)
		*piHei = m_iTextExtY;

	return true;
}

/*	Draw text string

	Return true for success, otherwise return false.

	x, y: left corner of text on screen.
	Color: text's color and transparent
	iHeight: new output height of text, 0 means to use the m_iOutHei. This parameter
			 won't change the value of m_iOutHei
*/
bool A3DString::DrawText(int x, int y, A3DCOLOR Color, int iHeight/* 0 */)
{
	if (!m_bReady || m_bHWIString)
		return true;

	m_pA3DDevice->GetA3DEngine()->BeginPerformanceRecord(A3DEngine::PERF_RENDER_TEXT);

	int i, n;
	float rx, fWid, fHei, fTexWid, fTexHei, v;

	A3DTLVERTEX* pVerts;
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);

	if (!iHeight)
		iHeight = m_iOutHei;

	rx		= (float)x;
	v		= 0.0f;
	fTexHei = (float)m_iLineHei / m_iTexHei;
	fWid	= (float)m_iTexWid / m_iLineHei * iHeight;
	fHei	= (float)iHeight;

	if (!m_pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**) &pVerts, 0))
		return false;

	for (i=0, n=0; i < m_iNumLines-1; i++, n+=4)
	{
		pVerts[n+0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, y-0.5f, 0.0f, 1.0f), Color, Specular, 0.0f, v+0.0f);
		pVerts[n+1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, y-0.5f, 0.0f, 1.0f), Color, Specular, 1.0f, v+0.0f);
		pVerts[n+2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, y+fHei-0.5f, 0.0f, 1.0f), Color, Specular, 0.0f, v+fTexHei);
		pVerts[n+3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, y+fHei-0.5f, 0.0f, 1.0f), Color, Specular, 1.0f, v+fTexHei);
		
		v  += fTexHei;
		rx += fWid;
	}

	//	Handle the last line
	fWid	= (float)m_iLastWid / m_iLineHei * iHeight;
	fTexWid	= (float)m_iLastWid / m_iTexWid;

	pVerts[n+0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, y-0.5f, 0.0f, 1.0f), Color, Specular, 0.0f, v+0.0f);
	pVerts[n+1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, y-0.5f, 0.0f, 1.0f), Color, Specular, fTexWid, v+0.0f);
	pVerts[n+2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, y+fHei-0.5f, 0.0f, 1.0f), Color, Specular, 0.0f, v+fTexHei);
	pVerts[n+3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, y+fHei-0.5f, 0.0f, 1.0f), Color, Specular, fTexWid, v+fTexHei);
	
	if (!m_pA3DStream->UnlockVertexBufferDynamic())
		return false;

	//	Set render states
	if (m_bFilter)
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	else
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);

	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	m_pA3DTexture->Appear(0);
	m_pA3DStream->Appear();

	//	Render
	if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumLines << 2, 0, m_iNumLines << 1))
		return false;

	//	Restore render state
	m_pA3DTexture->Disappear(0);

	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

	m_pA3DDevice->GetA3DEngine()->EndPerformanceRecord(A3DEngine::PERF_RENDER_TEXT);

	return true;
}

/*	Draw 3D text.

	Return true for success, otherwise return false.

	pView: current viewport.
	vPos: text center's position in 3D world space
	fRatio: distance in 3D space which is represented by a pixel
	Color: text's color
*/
bool A3DString::Draw3DText(A3DViewport* pView, A3DVECTOR3& vPos, float fRatio, A3DCOLOR Color)
{
	if (!m_bReady || m_bHWIString)
		return true;

	m_pA3DDevice->GetA3DEngine()->BeginPerformanceRecord(A3DEngine::PERF_RENDER_TEXT);

	A3DCameraBase* pCamera = pView->GetCamera();
	A3DVECTOR3 vCenter;
	float fWidth, fHeight;

	//	Transfrom center from world to screen
	vCenter = vPos;
	pView->Transform(vPos, vPos);

	if (vPos.z < 0.0f || vPos.z > 1.0f)
		return true;

	A3DVIEWPORTPARAM* pViewPara = pView->GetParam();

	//	String's size in 3D space
	fWidth  = m_iTextExtX * fRatio;
	fHeight = m_iTextExtY * fRatio;

	//	Extents from world space to screen
	A3DVECTOR3 vUp	  = pCamera->GetUp();
	A3DVECTOR3 vRight = pCamera->GetRight();
	A3DVECTOR3 vExts  = vCenter + vRight * fWidth * 0.5f + vUp * fHeight * 0.5f;
	
	pView->Transform(vExts, vExts);
	
	//	String's half size on screen
	fWidth	= vExts.x - vPos.x;
	fHeight	= vPos.y - vExts.y;

	//	If string is out of screen, don't render it
	if (vPos.x < -fWidth || vPos.x >= (float)pViewPara->Width + fWidth ||
		vPos.y < -fHeight || vPos.y >= (float)pViewPara->Height + fHeight)
		return true;

	//	String's left-up corner on screen
	float x = vPos.x - fWidth;
	float y = vPos.y - fHeight;
	float z = vPos.z;

	//	Build vertices
	int i, n;
	float rx, fWid, fHei, fTexWid, fTexHei, v;

	A3DTLVERTEX* pVerts;
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);

	rx		= x;
	v		= 0.0f;
	fTexHei = (float)m_iLineHei / m_iTexHei;
	fWid	= m_iTexWid * (fWidth / (m_iTextExtX * 0.5f));
	fHei	= m_iLineHei * (fHeight / (m_iTextExtY * 0.5f));

	if (!m_pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**) &pVerts, 0))
		return false;

	for (i=0, n=0; i < m_iNumLines-1; i++, n+=4)
	{
		pVerts[n+0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, y-0.5f, z, 1.0f), Color, Specular, 0.0f, v+0.0f);
		pVerts[n+1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, y-0.5f, z, 1.0f), Color, Specular, 1.0f, v+0.0f);
		pVerts[n+2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, y+fHei-0.5f, z, 1.0f), Color, Specular, 0.0f, v+fTexHei);
		pVerts[n+3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, y+fHei-0.5f, z, 1.0f), Color, Specular, 1.0f, v+fTexHei);
		
		v  += fTexHei;
		rx += fWid;
	}

	//	Handle the last line
	fWid	= m_iLastWid * (fWidth / (m_iTextExtX * 0.5f));
	fTexWid	= (float)m_iLastWid / m_iTexWid;

	pVerts[n+0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, y-0.5f, z, 1.0f), Color, Specular, 0.0f, v+0.0f);
	pVerts[n+1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, y-0.5f, z, 1.0f), Color, Specular, fTexWid, v+0.0f);
	pVerts[n+2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, y+fHei-0.5f, z, 1.0f), Color, Specular, 0.0f, v+fTexHei);
	pVerts[n+3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, y+fHei-0.5f, z, 1.0f), Color, Specular, fTexWid, v+fTexHei);
	
	if (!m_pA3DStream->UnlockVertexBufferDynamic())
		return false;
	
	//	Set render states
	if (m_bFilter)
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	else
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	m_pA3DTexture->Appear(0);
	m_pA3DStream->Appear();

	//	Render
	if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumLines << 2, 0, m_iNumLines << 1))
		return false;

	//	Restore render state
	m_pA3DTexture->Disappear(0);

	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

	m_pA3DDevice->GetA3DEngine()->EndPerformanceRecord(A3DEngine::PERF_RENDER_TEXT);

	return true;
}


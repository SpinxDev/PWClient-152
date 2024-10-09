/*
 * FILE: Bitmap.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Render.h"
#include "Bitmap.h"
#include "AMemory.h"
#include "AF.h"
#include "A3D.h"

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
//	Implement CELBitmap
//	
///////////////////////////////////////////////////////////////////////////

CELBitmap::CELBitmap() : m_strFile("")
{
	m_hBitmap	= NULL;
	m_pBmpData	= NULL;
	m_iWidth	= 0;
	m_iHeight	= 0;
	m_iBPP		= 0;
	m_iPitch	= 0;
}

CELBitmap::~CELBitmap()
{
	CELBitmap::Release();
}

//	Release object
void CELBitmap::Release()
{
	if (m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
		m_pBmpData = NULL;
	}
}

//	Create empty bitmap
bool CELBitmap::CreateBitmap(int iWidth, int iHeight, int iBPP, const PALETTEENTRY* aEntries)
{
	//	Don't support 16-bit now
	ASSERT(iBPP != 16);

	bool bRet;

	if (iBPP == 8)	//	8-bit
		bRet = Create8BitBitmap(iWidth, iHeight, aEntries);
	else	//	non 8-bit
		bRet = CreateNon8BitBitmap(iWidth, iHeight, iBPP);

	if (bRet)
	{
		//	Clear bitmap
		memset(m_pBmpData, 0, m_iPitch * m_iHeight);
	}

	m_strFile = "";

	return bRet;
}

//	Load a picture from file and store it as 32-bit bitmap
bool CELBitmap::LoadAs32BitBitmap(const char* szFile, int iWidth, int iHeight)
{
	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return false;

	if (!pTexture->LoadFromFile(g_Render.GetA3DDevice(), szFile, iWidth, iHeight, A3DFMT_UNKNOWN, 1, NULL))
	{
		g_Log.Log("CELBitmap::LoadAs32BitBitmap, Failed to load texture");
		return false;
	}

	if (af_CheckFileExt(szFile, ".dds", 4))
	{
		//	Save texture as bitmap and reload it.
		char szNewFile[MAX_PATH], szTitle[MAX_PATH];
		af_GetFileTitle(szFile, szTitle, MAX_PATH);
		sprintf(szNewFile, "%sTemp\\%s.bmp", g_szWorkDir, szTitle);
		if (FAILED(D3DXSaveTextureToFile(szNewFile, D3DXIFF_BMP, pTexture->GetD3DTexture(), NULL)))
		{
			A3DRELEASE(pTexture);
			g_Log.Log("CELBitmap::LoadAs32BitBitmap, Failed to save .dds texture to .bmp file");
			return false;
		}

		//	Reload texture
		A3DRELEASE(pTexture);

		if (!(pTexture = new A3DTexture))
			return false;

		if (!pTexture->LoadFromFile(g_Render.GetA3DDevice(), szNewFile, iWidth, iHeight, A3DFMT_UNKNOWN, 1, NULL))
		{
			g_Log.Log("CELBitmap::LoadAs32BitBitmap, Failed to load temporary .bmp texture");
			return false;
		}
	}

	bool bRet = Create32BitBmpFromTexture(pTexture);
	if (!bRet)
		g_Log.Log("CELBitmap::LoadAs32BitBitmap, Failed to create bitmap from texture");

	A3DRELEASE(pTexture);

	//	Record file name
	af_GetRelativePath(szFile, m_strFile);

	return bRet;
}

//	Load a gray map from file and store it as 8-bit bitmap
bool CELBitmap::LoadAsGrayBitmap(const char* szFile, int iWidth, int iHeight)
{
	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return false;

	if (!pTexture->LoadFromFile(g_Render.GetA3DDevice(), szFile, 0, 0, A3DFMT_UNKNOWN, 1, m_PalEntries))
	{
		g_Log.Log("CELBitmap::LoadAsGrayBitmap, Failed to load texture");
		return false;
	}

	bool bRet = CreateGrayBmpFromTexture(pTexture, m_PalEntries);
	if (!bRet)
		g_Log.Log("CELBitmap::LoadAsGrayBitmap, Failed to create bitmap from texture");

	A3DRELEASE(pTexture);

	//	Record file name
	af_GetRelativePath(szFile, m_strFile);

	return true;
}

//	Create a non 8-bit bitmap
bool CELBitmap::CreateNon8BitBitmap(int iWidth, int iHeight, int iBPP)
{
	BITMAPINFO* pbi = (BITMAPINFO*)a_malloc(sizeof (BITMAPINFO) - 4);
	if (!pbi)
	{
		g_Log.Log("CELBitmap::CreateNon8BitBitmap, Not enough memory");
		return false;
	}

	pbi->bmiHeader.biSize			= sizeof (BITMAPINFOHEADER);
	pbi->bmiHeader.biWidth			= iWidth;
	pbi->bmiHeader.biHeight			= -iHeight;
	pbi->bmiHeader.biPlanes			= 1;
	pbi->bmiHeader.biBitCount		= iBPP;
	pbi->bmiHeader.biCompression	= BI_RGB;
	pbi->bmiHeader.biSizeImage		= 0;
	pbi->bmiHeader.biXPelsPerMeter	= 0;
	pbi->bmiHeader.biYPelsPerMeter	= 0;
	pbi->bmiHeader.biClrUsed		= 0;
	pbi->bmiHeader.biClrImportant	= 0;

	HDC hDC = ::GetDC(NULL);
	m_hBitmap = ::CreateDIBSection(hDC, pbi, DIB_RGB_COLORS, (void**) &m_pBmpData, 0, 0);
	::ReleaseDC(NULL, hDC);

	if (!m_hBitmap)
	{
		a_free(pbi);
		g_Log.Log("CELBitmap::CreateNon8BitBitmap, Failed to call CreateDIBSection");
		return false;
	}

	a_free(pbi);

	//	Get bitmap information
	BITMAP Bmp;
	::GetObject(m_hBitmap, sizeof (Bmp), &Bmp);

	m_iWidth	= iWidth;
	m_iHeight	= iHeight;
	m_iBPP		= iBPP;
//	m_iPitch	= Bmp.bmWidthBytes;
	m_iPitch	= (((iWidth * (iBPP / 8)) + 3) >> 2) << 2;

	return true;
}

//	Create a 8-bit bitmap
bool CELBitmap::Create8BitBitmap(int iWidth, int iHeight, const PALETTEENTRY* aEntries)
{
	BITMAPINFO* pbi = (BITMAPINFO*)a_malloc(sizeof (BITMAPINFO) - 4 + 256 * sizeof (RGBQUAD));
	if (!pbi)
	{
		g_Log.Log("CELBitmap::Create8BitBitmap, Not enough memory");
		return false;
	}

	pbi->bmiHeader.biSize			= sizeof (BITMAPINFOHEADER);
	pbi->bmiHeader.biWidth			= iWidth;
	pbi->bmiHeader.biHeight			= -iHeight;
	pbi->bmiHeader.biPlanes			= 1;
	pbi->bmiHeader.biBitCount		= 8;
	pbi->bmiHeader.biCompression	= BI_RGB;
	pbi->bmiHeader.biSizeImage		= 0;
	pbi->bmiHeader.biXPelsPerMeter	= 0;
	pbi->bmiHeader.biYPelsPerMeter	= 0;
	pbi->bmiHeader.biClrUsed		= 256;
	pbi->bmiHeader.biClrImportant	= 256;

	//	Fill palette
	if (aEntries)
	{
		for (int i=0; i < 256; i++)
		{
			pbi->bmiColors[i].rgbRed		= aEntries[i].peRed;
			pbi->bmiColors[i].rgbGreen		= aEntries[i].peGreen;
			pbi->bmiColors[i].rgbBlue		= aEntries[i].peBlue;
			pbi->bmiColors[i].rgbReserved	= 0;

			m_PalEntries[i].peRed	= aEntries[i].peRed;
			m_PalEntries[i].peGreen	= aEntries[i].peRed;
			m_PalEntries[i].peBlue	= aEntries[i].peRed;
			m_PalEntries[i].peFlags	= aEntries[i].peFlags;
		}
	}
	else	//	Use default palette
	{
		for (int i=0; i < 256; i++)
		{
			pbi->bmiColors[i].rgbRed		= (BYTE)i;
			pbi->bmiColors[i].rgbGreen		= (BYTE)i;
			pbi->bmiColors[i].rgbBlue		= (BYTE)i;
			pbi->bmiColors[i].rgbReserved	= 0;

			m_PalEntries[i].peRed	= (BYTE)i;
			m_PalEntries[i].peGreen	= (BYTE)i;
			m_PalEntries[i].peBlue	= (BYTE)i;
			m_PalEntries[i].peFlags	= PC_EXPLICIT;
		}
	}

	HDC hDC = ::GetDC(NULL);
	m_hBitmap = ::CreateDIBSection(hDC, pbi, DIB_RGB_COLORS, (void**) &m_pBmpData, 0, 0);
	::ReleaseDC(NULL, hDC);

	if (!m_hBitmap)
	{
		a_free(pbi);
		g_Log.Log("CELBitmap::Create8BitBitmap, Failed to call CreateDIBSection");
		return false;
	}

	a_free(pbi);

	//	Get bitmap information
	BITMAP Bmp;
	::GetObject(m_hBitmap, sizeof (Bmp), &Bmp);

	m_iWidth	= iWidth;
	m_iHeight	= iHeight;
	m_iBPP		= 8;
//	m_iPitch	= Bmp.bmWidthBytes;
	m_iPitch	= ((iWidth + 3) >> 2) << 2;

	return true;
}

//	Create a 32-bit windows bitmap form a texture
bool CELBitmap::Create32BitBmpFromTexture(A3DTexture* pA3DTexture)
{
	if (!pA3DTexture)
		return NULL;

	int iWidth, iHeight;
	pA3DTexture->GetDimension(&iWidth, &iHeight);
	A3DFORMAT Fmt = pA3DTexture->GetFormat();

	//	Create bitmap
	if (!CreateNon8BitBitmap(iWidth, iHeight, 32))
	{
		g_Log.Log("CELBitmap::Create32BitBmpFromTexture, Failed to create bitmap");
		return false;
	}

	//	Get texture data
	void* pSrcData;
	int iSrcPitch;
	if (!pA3DTexture->LockRect(NULL, &pSrcData, &iSrcPitch, 0))
	{
		Release();
		g_Log.Log("CELBitmap::Create32BitBmpFromTexture, Failed to lock texture");
		return NULL;
	}

	BYTE* pSrcLine = (BYTE*)pSrcData;
	BYTE* pDstLine = m_pBmpData;
	int iDstPitch = m_iPitch;

	//	Fill data
	if (Fmt == A3DFMT_A8R8G8B8 || Fmt == A3DFMT_X8R8G8B8)
	{
		for (int i=0; i < iHeight; i++)
		{
			DWORD* pSrcPixel = (DWORD*)pSrcLine;
			DWORD* pDstPixel = (DWORD*)pDstLine;

			for (int j=0; j < iWidth; j++, pDstPixel++, pSrcPixel++)
			{
				*pDstPixel = *pSrcPixel & 0x00ffffff;
			}

			pSrcLine += iSrcPitch;
			pDstLine += iDstPitch;
		}
	}
	else if (Fmt == A3DFMT_R5G6B5)
	{
		float fScale = 1.0f / 31.0f * 255.0f;

		for (int i=0; i < iHeight; i++)
		{
			WORD* pSrcPixel = (WORD*)pSrcLine;
			DWORD* pDstPixel = (DWORD*)pDstLine;

			for (int j=0; j < iWidth; j++, pDstPixel++, pSrcPixel++)
			{
				BYTE r = (BYTE)(((*pSrcPixel & 0xf800) >> 11) * fScale);
				BYTE g = (BYTE)(((*pSrcPixel & 0x07e0) >> 5) * fScale + 0.5f);
				BYTE b = (BYTE)(((*pSrcPixel & 0x001f)) * fScale);

				*pDstPixel = RGB(r, g, b);
			}

			pSrcLine += iSrcPitch;
			pDstLine += iDstPitch;
		}
	}
	else if (Fmt == A3DFMT_X1R5G5B5 || Fmt == A3DFMT_A1R5G5B5)
	{
		float fScale = 1.0f / 31.0f * 255.0f;

		for (int i=0; i < iHeight; i++)
		{
			WORD* pSrcPixel = (WORD*)pSrcLine;
			DWORD* pDstPixel = (DWORD*)pDstLine;

			for (int j=0; j < iWidth; j++, pDstPixel++, pSrcPixel++)
			{
				BYTE r = (BYTE)(((*pSrcPixel & 0x7d00) >> 10) * fScale);
				BYTE g = (BYTE)(((*pSrcPixel & 0x03e0) >> 5) * fScale);
				BYTE b = (BYTE)(((*pSrcPixel & 0x001f)) * fScale);

				*pDstPixel = RGB(r, g, b);
			}

			pSrcLine += iSrcPitch;
			pDstLine += iDstPitch;
		}
	}
	else
	{
		g_Log.Log("CELBitmap::Create32BitBmpFromTexture, Unable to handle format %d", Fmt);
	}

	pA3DTexture->UnlockRect();

	return true;
}

//	Create a 8-bit gray bitmap from a texture
bool CELBitmap::CreateGrayBmpFromTexture(A3DTexture* pA3DTexture, const PALETTEENTRY* aEntries)
{
	int i, j, iWidth, iHeight;
	pA3DTexture->GetDimension(&iWidth, &iHeight);
	A3DFORMAT Fmt = pA3DTexture->GetFormat();

	//	Create a 8-bit bitmap with default palette
	if (!Create8BitBitmap(iWidth, iHeight, NULL))
	{
		g_Log.Log("CELBitmap::CreateGrayBmpFromTexture, Failed to create bitmap");
		return false;
	}

	BYTE aEntryMaps[256];
	bool bDifferentPal = false;

	if (Fmt == A3DFMT_P8 && aEntries)
	{
		//	All of our mask maps should use default palette, so we record
		//	the palette difference here. Because mask maps are gray maps,
		//	we only check blue channel
		for (i=0; i < 256; i++)
		{
			aEntryMaps[i] = aEntries[i].peBlue;	//	Destination index
			if (aEntries[i].peBlue != i)
				bDifferentPal = true;
		}
	}

	//	Get texture data
	void* pSrcData;
	int iSrcPitch;
	if (!pA3DTexture->LockRect(NULL, &pSrcData, &iSrcPitch, 0))
	{
		Release();
		g_Log.Log("CELBitmap::CreateGrayBmpFromTexture, Failed to lock texture");
		return false;
	}

	BYTE* pSrcLine = (BYTE*)pSrcData;
	BYTE* pDstLine = m_pBmpData;
	int iDstPitch = m_iPitch;

	//	Fill data
	if (Fmt == A3DFMT_A8R8G8B8 || Fmt == A3DFMT_X8R8G8B8)
	{
		for (i=0; i < iHeight; i++)
		{
			DWORD* pSrcPixel = (DWORD*)pSrcLine;
			BYTE* pDstPixel = pDstLine;

			//	Use blue channel as gray value
			for (j=0; j < iWidth; j++, pDstPixel++, pSrcPixel++)
				*pDstPixel = (BYTE)(*pSrcPixel & 0x000000ff);
			
			pSrcLine += iSrcPitch;
			pDstLine += iDstPitch;
		}
	}
	else if (Fmt == A3DFMT_R5G6B5 || Fmt == A3DFMT_X1R5G5B5 || Fmt == A3DFMT_A1R5G5B5)
	{
		float fScale = 1.0f / 31.0f * 255.0f;

		for (i=0; i < iHeight; i++)
		{
			WORD* pSrcPixel = (WORD*)pSrcLine;
			BYTE* pDstPixel = pDstLine;

			//	Use blue channel as gray value
			for (j=0; j < iWidth; j++, pDstPixel++, pSrcPixel++)
				*pDstPixel = (BYTE)(((*pSrcPixel & 0x001f)) * fScale);

			pSrcLine += iSrcPitch;
			pDstLine += iDstPitch;
		}
	}
	else if (Fmt == A3DFMT_A8 || Fmt == A3DFMT_L8 || Fmt == A3DFMT_P8)
	{
		BYTE* pSrcPixel = pSrcLine;
		BYTE* pDstPixel = pDstLine;

		if (Fmt == A3DFMT_P8 && bDifferentPal)
		{
			//	Need to convert palette
			for (i=0; i < iHeight; i++)
			{
				for (j=0; j < iWidth; j++, pDstPixel++, pSrcPixel++)
					*pDstPixel = aEntryMaps[*pSrcPixel];

				pSrcLine += iSrcPitch;
				pDstLine += iDstPitch;
			}
		}
		else
		{
			for (i=0; i < iHeight; i++)
			{
				for (j=0; j < iWidth; j++, pDstPixel++, pSrcPixel++)
					*pDstPixel = *pSrcPixel;

				pSrcLine += iSrcPitch;
				pDstLine += iDstPitch;
			}
		}
	}
	else
	{
		g_Log.Log("CELBitmap::CreateGrayBmpFromTexture, Unable to handle format %d", Fmt);
	}

	pA3DTexture->UnlockRect();

	return true;
}

//	Save bitmap data to file
bool CELBitmap::SaveToFile(const char* szFile)
{
	if (!m_hBitmap || !m_pBmpData)
		return false;

	FILE* fp = fopen(szFile, "wb");
	if (!fp)
	{
		g_Log.Log("CELBitmap::SaveToFile, Failed to create file %s", szFile);
		return false;
	}

	BITMAPINFO* pbi = NULL;
	int i, iBmpInfoSize = 0;

	if (m_iBPP == 8)
	{
		iBmpInfoSize = sizeof (BITMAPINFO) - 4 + 256 * sizeof (RGBQUAD);

		if (!(pbi = (BITMAPINFO*)a_malloc(iBmpInfoSize)))
		{
			fclose(fp);
			g_Log.Log("CELBitmap::SaveToFile, Not enough memory");
			return NULL;
		}

		//	Fill palette
		for (i=0; i < 256; i++)
		{
			pbi->bmiColors[i].rgbRed		= m_PalEntries[i].peRed;
			pbi->bmiColors[i].rgbGreen		= m_PalEntries[i].peGreen;
			pbi->bmiColors[i].rgbBlue		= m_PalEntries[i].peBlue;
			pbi->bmiColors[i].rgbReserved	= 0;
		}
	}
	else	//	m_iBP == 32 || m_iBP == 24 || m_iBP == 16
	{
		iBmpInfoSize = sizeof (BITMAPINFO) - 4;

		if (!(pbi = (BITMAPINFO*)a_malloc(iBmpInfoSize)))
		{
			fclose(fp);
			g_Log.Log("CELBitmap::SaveToFile, Not enough memory");
			return false;
		}
	}

	//	Fill bitmap information
	pbi->bmiHeader.biSize			= sizeof (BITMAPINFOHEADER);
	pbi->bmiHeader.biWidth			= m_iWidth;
	pbi->bmiHeader.biHeight			= m_iHeight;
	pbi->bmiHeader.biPlanes			= 1;
	pbi->bmiHeader.biBitCount		= m_iBPP;
	pbi->bmiHeader.biCompression	= BI_RGB;
	pbi->bmiHeader.biSizeImage		= 0;
	pbi->bmiHeader.biXPelsPerMeter	= 0;
	pbi->bmiHeader.biYPelsPerMeter	= 0;
	pbi->bmiHeader.biClrUsed		= (m_iBPP == 8) ? 256 : 0;
	pbi->bmiHeader.biClrImportant	= (m_iBPP == 8) ? 256 : 0;

	int iImageSize = m_iHeight * m_iPitch;

	//	Fill file header
	BITMAPFILEHEADER Header;
	memset(&Header, 0, sizeof (Header));

	Header.bfType		= 0x4D42;
	Header.bfSize		= sizeof (Header) + iBmpInfoSize + iImageSize;
	Header.bfOffBits	= sizeof (Header) + iBmpInfoSize;

	//	Write file header
	fwrite(&Header, 1, sizeof (Header), fp);
	//	Write bitmap informaton.
	fwrite(pbi, 1, iBmpInfoSize, fp);

	//	Write bitmap data.  Note: iamge data in .bmp file is upside down
	const BYTE* pLineData = m_pBmpData + (m_iHeight-1) * m_iPitch;
	for (i=0; i < m_iHeight; i++)
	{
		fwrite(pLineData, 1, m_iPitch, fp);
		pLineData -= m_iPitch;
	}

	a_free(pbi);
	fclose(fp);

	return true;
}

//	Lock rectangle
bool CELBitmap::LockRect(const ARectI& rc, LOCKINFO* pLockInfo)
{
	if (!m_pBmpData)
		return false;

	if (rc.left < 0 || rc.top < 0 || rc.right > m_iWidth || rc.bottom > m_iHeight)
		return false;

	pLockInfo->iWidth	= rc.Width();
	pLockInfo->iHeight	= rc.Height();
	pLockInfo->iPitch	= m_iPitch;
	pLockInfo->pData	= m_pBmpData + rc.top * m_iPitch + rc.left * m_iBPP / 8;
	
	return true;
}

//	Clear bitmap with black color
void CELBitmap::Clear()
{
	if (!m_pBmpData)
		return;

	memset(m_pBmpData, 0, m_iPitch * m_iHeight);
}

//	Calculate size of memory buffer need to store whole bitmap file
int CELBitmap::CalcBitmapFileSize()
{
	if (!m_hBitmap || !m_pBmpData)
		return 0;

	int iBmpInfoSize, iImageSize;
	
	iImageSize = m_iHeight * m_iPitch;

	if (m_iBPP == 8)
		iBmpInfoSize = sizeof (BITMAPINFO) - 4 + 256 * sizeof (RGBQUAD);
	else	//	m_iBP == 32 || m_iBP == 24 || m_iBP == 16
		iBmpInfoSize = sizeof (BITMAPINFO) - 4;

	return iBmpInfoSize + sizeof (BITMAPFILEHEADER) + iImageSize;
}

//	Save bitmap data to memory buffer
bool CELBitmap::SaveToMemory(BYTE* pBuffer)
{
	if (!pBuffer || !m_hBitmap || !m_pBmpData)
		return false;

	int i, iBmpInfoSize;

	//	Fill bitmap information
	BYTE* pWritePtr = pBuffer + sizeof (BITMAPFILEHEADER);	//	Skip file header
	BITMAPINFO* pbi = (BITMAPINFO*)pWritePtr;

	if (m_iBPP == 8)
	{
		iBmpInfoSize = sizeof (BITMAPINFO) - 4 + 256 * sizeof (RGBQUAD);
		pWritePtr += iBmpInfoSize;

		//	Fill palette
		for (i=0; i < 256; i++)
		{
			pbi->bmiColors[i].rgbRed		= m_PalEntries[i].peRed;
			pbi->bmiColors[i].rgbGreen		= m_PalEntries[i].peGreen;
			pbi->bmiColors[i].rgbBlue		= m_PalEntries[i].peBlue;
			pbi->bmiColors[i].rgbReserved	= 0;
		}
	}
	else	//	m_iBP == 32 || m_iBP == 24 || m_iBP == 16
	{
		iBmpInfoSize = sizeof (BITMAPINFO) - 4;
		pWritePtr += iBmpInfoSize;
	}

	//	Fill bitmap information
	pbi->bmiHeader.biSize			= sizeof (BITMAPINFOHEADER);
	pbi->bmiHeader.biWidth			= m_iWidth;
	pbi->bmiHeader.biHeight			= m_iHeight;
	pbi->bmiHeader.biPlanes			= 1;
	pbi->bmiHeader.biBitCount		= m_iBPP;
	pbi->bmiHeader.biCompression	= BI_RGB;
	pbi->bmiHeader.biSizeImage		= 0;
	pbi->bmiHeader.biXPelsPerMeter	= 0;
	pbi->bmiHeader.biYPelsPerMeter	= 0;
	pbi->bmiHeader.biClrUsed		= (m_iBPP == 8) ? 256 : 0;
	pbi->bmiHeader.biClrImportant	= (m_iBPP == 8) ? 256 : 0;

	//	Write bitmap data.  Note: iamge data in .bmp file is upside down
	const BYTE* pLineData = m_pBmpData + (m_iHeight-1) * m_iPitch;
	for (i=0; i < m_iHeight; i++)
	{
		memcpy(pWritePtr, pLineData, m_iPitch);
		pWritePtr += m_iPitch;
		pLineData -= m_iPitch;
	}

	int iImageSize = m_iHeight * m_iPitch;

	//	Fill file header
	BITMAPFILEHEADER* pHeader = (BITMAPFILEHEADER*)pBuffer;
	pWritePtr += sizeof (BITMAPFILEHEADER);

	memset(pHeader, 0, sizeof (BITMAPFILEHEADER));
	pHeader->bfType		= 0x4D42;
	pHeader->bfSize		= sizeof (BITMAPFILEHEADER) + iBmpInfoSize + iImageSize;
	pHeader->bfOffBits	= sizeof (BITMAPFILEHEADER) + iBmpInfoSize;

	return true;

}

//	Load bitmap from from memory buffer
bool CELBitmap::LoadFromMemory(BYTE* pBuffer)
{
	if (!pBuffer)
		return false;

	//	Get file header
	BITMAPFILEHEADER* pHeader = (BITMAPFILEHEADER*)pBuffer;
	int i;

	//	Skip file header
	BYTE* pWritePtr = pBuffer + sizeof (BITMAPFILEHEADER);
	BITMAPINFO* pbi = (BITMAPINFO*)pWritePtr;

	if (pbi->bmiHeader.biBitCount == 8)
	{
		for (i=0; i < 256; i++)
		{
			//	Fill palette
			m_PalEntries[i].peRed	= pbi->bmiColors[i].rgbRed;
			m_PalEntries[i].peGreen	= pbi->bmiColors[i].rgbGreen;
			m_PalEntries[i].peBlue	= pbi->bmiColors[i].rgbBlue;
			m_PalEntries[i].peFlags	= PC_EXPLICIT;
		}
	}

	HDC hDC = ::GetDC(NULL);
	m_hBitmap = ::CreateDIBSection(hDC, pbi, DIB_RGB_COLORS, (void**) &m_pBmpData, 0, 0);
	::ReleaseDC(NULL, hDC);

	if (!m_hBitmap)
	{
		g_Log.Log("CELBitmap::LoadFromMemory, Failed to call CreateDIBSection");
		return false;
	}

	//	Save bitmap information
	m_iWidth	= pbi->bmiHeader.biWidth;
	m_iHeight	= pbi->bmiHeader.biHeight;
	m_iBPP		= pbi->bmiHeader.biBitCount;
	m_iPitch	= ((m_iWidth + 3) >> 2) << 2;

	//	Fill image data, Note: iamge data in .bmp file is upside down
	pWritePtr = pBuffer + pHeader->bfOffBits;

	BYTE* pSrcLine = pWritePtr;
	BYTE* pDstLine = m_pBmpData + (m_iHeight-1) * m_iPitch;

	for (i=0; i < m_iHeight; i++)
	{
		memcpy(pDstLine, pSrcLine, m_iPitch);
		pSrcLine += m_iPitch;
		pDstLine -= m_iPitch;
	}

	return true;
}


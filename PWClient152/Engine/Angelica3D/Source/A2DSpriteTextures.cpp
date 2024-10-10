/*
 * FILE: A2DSpriteTextures.cpp
 *
 * DESCRIPTION: Textures build for 2D sprite for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2004/11/19
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A2DSpriteTextures.h"
#include "A3DTexture.h"
#include "A3DDevice.h"
#include "A3DSurface.h"
#include "A3DSurfaceMan.h"
#include "A3DEngine.h"
#include "A3DPI.h"
#include "A2DSpriteBuffer.h"

A2DSpriteTextures::A2DSpriteTextures()
{
	m_pA3DDevice		= NULL;
	m_nTextureNum		= 0;
	m_dwPosInMan		= 0;
	m_pSpriteBuffer		= NULL;
	m_bHasBeenCached	= false;
	m_nUserRef			= 0;
	m_bDynamicTex		= false;
}

A2DSpriteTextures::~A2DSpriteTextures()
{
}

bool A2DSpriteTextures::Init(A3DDevice* pA3DDevice, const char* szSpriteMap, int nWidth, int nHeight, A3DCOLOR colorKey)
{
	m_pA3DDevice	= pA3DDevice;
	
	A3DSurface * pSurface = NULL;
	// init from a surface
	if (nWidth > 0 && nHeight > 0)
	{
		if (!m_pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->LoadSurfaceFromFile(nWidth, nHeight, szSpriteMap, colorKey, &pSurface))
		{
			g_A3DErrLog.Log("A2DSpriteTextures::Init(), %s LoadSurfaceFromFile Fail!", szSpriteMap);
			return false;
		}
	}
	else
	{
		if (!m_pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->LoadSurfaceFromFile(szSpriteMap, colorKey, &pSurface))
		{
			g_A3DErrLog.Log("A2DSpriteTextures::Init() %s LoadSurfaceFromFile Fail!", szSpriteMap);
			return false;
		}

		nWidth	= pSurface->GetWidth();
		nHeight	= pSurface->GetHeight();
	}

	// set width and height of this sprite's textures
	m_nWidth	= nWidth;
	m_nHeight	= nHeight;

	// determine the format of each texture
	m_Format	= pSurface->GetFormat();
	if (m_Format == A3DFMT_X8R8G8B8)
		m_Format = A3DFMT_A8R8G8B8;
	
	if( !BuildTextures(pSurface) )
	{
		g_A3DErrLog.Log("A2DSpriteTextures::Init(), BuildTextures() Fail!");
		return false;
	}

	// now release the unused surface object
	m_pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->ReleaseSurface(pSurface);

	SetName(szSpriteMap);
	return true;
}

bool A2DSpriteTextures::InitFromFileInMemory(A3DDevice* pA3DDevice, const char* szSpriteMap, const void * pMemFile, DWORD dwFileLength, int nWidth, int nHeight, A3DCOLOR colorKey)
{
	m_pA3DDevice	= pA3DDevice;

	A3DSurface * pSurface = NULL;
	// init from a surface
	if (nWidth > 0 && nHeight > 0)
	{
		if (!m_pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->LoadSurfaceFromFileInMemory(nWidth, nHeight, szSpriteMap, pMemFile, dwFileLength, colorKey, &pSurface))
		{
			g_A3DErrLog.Log("A2DSpriteTextures::InitFromFileInMemory(), %s LoadSurfaceFromFile Fail!", szSpriteMap);
			return false;
		}
	}
	else
	{
		if (!m_pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->LoadSurfaceFromFileInMemory(szSpriteMap, pMemFile, dwFileLength, colorKey, &pSurface))
		{
			g_A3DErrLog.Log("A2DSpriteTextures::InitFromFileInMemory() %s LoadSurfaceFromFile Fail!", szSpriteMap);
			return false;
		}

		nWidth	= pSurface->GetWidth();
		nHeight	= pSurface->GetHeight();
	}

	// set width and height of this sprite's textures
	m_nWidth	= nWidth;
	m_nHeight	= nHeight;

	// determine the format of each texture
	m_Format	= pSurface->GetFormat();
	if (m_Format == A3DFMT_X8R8G8B8)
		m_Format = A3DFMT_A8R8G8B8;
	
	if( !BuildTextures(pSurface) )
	{
		g_A3DErrLog.Log("A2DSpriteTextures::InitFromFileInMemory(), BuildTextures() Fail!");
		return false;
	}

	// now release the unused surface object
	m_pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->ReleaseSurface(pSurface);

	SetName(szSpriteMap);
	return true;
}

bool A2DSpriteTextures::InitWithoutSurface(A3DDevice* pA3DDevice, int nWidth, int nHeight, A3DFORMAT format)
{
	m_pA3DDevice	= pA3DDevice;

	// set width and height of this sprite's textures
	m_nWidth	= nWidth;
	m_nHeight	= nHeight;

	// determine the format of each texture
	m_Format	= format;

	if( !BuildTextures(NULL) )
		return false;

	SetName("A2DSpriteTextures");

	// sys created texture does not use any sprite buffer
	//if( m_pSpriteBuffer )
	//{
	//	m_pSpriteBuffer->Release();
	//	delete m_pSpriteBuffer;
	//	m_pSpriteBuffer = NULL;
	//}

	return true;
}

bool A2DSpriteTextures::BuildTextures(A3DSurface * pSrcSurface)
{
	m_nTextureNum = 0;

	int		x = 0, y = 0;
	int		nThisWidth, nThisHeight;

	while(x < m_nWidth)
	{
		if( m_nWidth - x > 256 )
			nThisWidth = 256;
		else
			nThisWidth = m_nWidth - x;

		y = 0;
		while(y < m_nHeight)
		{
			if( m_nHeight - y > 256 )
				nThisHeight = 256;
			else
				nThisHeight = m_nHeight - y;

			A3DRECT rect;
			int		nTexWidth, nTexHeight;

			rect.left = x; rect.top = y;
			rect.right = x + nThisWidth; rect.bottom = y + nThisHeight;

			A3DTexture * pA3DTexture = new A3DTexture;
			if( NULL == pA3DTexture )
			{
				g_A3DErrLog.Log("A2DSpriteTextures::BuildTextures(), not enough memory!");
				return false;
			}
			
			if( !pA3DTexture->CreateFromSurface(m_pA3DDevice, pSrcSurface, m_bDynamicTex, rect, &nTexWidth, &nTexHeight, m_Format) )
			{
				g_A3DErrLog.Log("A2DSpriteTextures::BuildTextures(), CreateTextureFromSurface Fail!");
				return false;
			}
			
			TEXTURE_RECT * pTextureRect = new TEXTURE_RECT;
			if (NULL == pTextureRect)
			{
				g_A3DErrLog.Log("A2DSprite::BuildTextures Not enough memory!");
				return false;
			}

			pTextureRect->nWidth = nTexWidth;
			pTextureRect->nHeight = nTexHeight;
			pTextureRect->rect = rect;
			pTextureRect->pA3DTexture = pA3DTexture;

			m_aTextures.Add(pTextureRect);
			m_nTextureNum ++;

			y += nThisHeight;
		}

		x += nThisWidth;
	}

	// now create the internal sprite buffer
	m_pSpriteBuffer = new A2DSpriteBuffer();
	if( !m_pSpriteBuffer->Init(m_pA3DDevice, this, 64) )
	{
		g_A3DErrLog.Log("A2DSpriteTextures::Init(), SpriteBuffer Init Fail!");
		return false;
	}
	return true;
}

bool A2DSpriteTextures::UnloadTextures()
{
	// now create the internal sprite buffer
	if( m_pSpriteBuffer )
	{
		m_pSpriteBuffer->Release();
		delete m_pSpriteBuffer;
		m_pSpriteBuffer = NULL;
	}

	for (int i=0; i < m_aTextures.GetSize(); i++)
	{
		TEXTURE_RECT* pTextureRect = m_aTextures[i];

		A3DTexture * pA3DTexture = (A3DTexture *) pTextureRect->pA3DTexture;
		pA3DTexture->Release();
		delete pA3DTexture;
		pA3DTexture = NULL;

		delete pTextureRect;
	}

	m_aTextures.RemoveAll(false);
	return true;
}

bool A2DSpriteTextures::Release()
{
	UnloadTextures();
	m_nUserRef = 0;
	return true;
}

bool A2DSpriteTextures::UpdateTextures(BYTE* pDataBuffer, DWORD dwPitch, A3DFORMAT format,bool bBottomToTop/*=true*/)
{
	DWORD dwBytesPerPixel;
	if( format == A3DFMT_R5G6B5 || format == A3DFMT_X1R5G5B5 )
		dwBytesPerPixel = 2;
	else if( format == A3DFMT_X8R8G8B8 || format == A3DFMT_A8R8G8B8 )
		dwBytesPerPixel = 4;
	else
	{
		g_A3DErrLog.Log("A2DSpriteTextures::UpdateTextures(), Unsupported video format");
		return false;
	}

	for (int i=0; i < m_aTextures.GetSize(); i++)
	{
		TEXTURE_RECT*	pTextureRect = m_aTextures[i];

		A3DTexture *	pTexture = pTextureRect->pA3DTexture;
		A3DRECT			rect = pTextureRect->rect;
		int				nTexWidth = rect.right - rect.left;//pTextureRect->nWidth;
		int				nTexHeight = rect.bottom - rect.top;//pTextureRect->nHeight;

		LPBYTE			pTexBuffer;
		int				iTexPitch;
		if( !pTexture->LockRect(NULL, (LPVOID *)&pTexBuffer, &iTexPitch, 0) )
		{
			g_A3DErrLog.Log("A2DSpriteTextures::UpdateTextures(), Can not lock texture!");
			return false;
		}

		if( format != m_Format ) 
		{
			g_A3DErrLog.Log("A2DSpriteTextures::UpdateTextures(), Different format!");
			return false;
		}

		// Copy the bits    
		// OPTIMIZATION OPPORTUNITY: Use a video and texture
		// format that allows a simpler copy than this one.
		int nSX, nSY;
		if( m_Format == A3DFMT_X8R8G8B8 || m_Format == A3DFMT_A8R8G8B8 )
		{
			for(int y=0; y<nTexHeight; y++)
			{
				nSY = y + rect.top;
				nSX = rect.left;
				
				int iRow = bBottomToTop ? m_nHeight - 1 - nSY : nSY;
				BYTE *pBmpBuffer = pDataBuffer + iRow * dwPitch + nSX * dwBytesPerPixel;
				BYTE *pSurfaceBuffer = pTexBuffer;

				memcpy(pSurfaceBuffer, pBmpBuffer, nTexWidth * dwBytesPerPixel);
				
				pTexBuffer += iTexPitch;
			}
		}
		else if( m_Format == A3DFMT_R5G6B5 || m_Format == A3DFMT_X1R5G5B5 )
		{
			for(int y=0; y<nTexHeight; y++)
			{
				nSY = y + rect.top;
				nSX = rect.left;

				int iRow = bBottomToTop ? m_nHeight - 1 - nSY : nSY;
				BYTE *pBmpBuffer = pDataBuffer + iRow * dwPitch + nSX * dwBytesPerPixel;
				BYTE *pSurfaceBuffer = pTexBuffer;

				memcpy(pSurfaceBuffer, pBmpBuffer, nTexWidth * dwBytesPerPixel);

				pTexBuffer += iTexPitch;
			}
		}
		else
		{
			g_A3DErrLog.Log("A2DSpriteTextures::UpdateTextures(), Unsupported format!");
			return false;
		}
		
		if( !pTexture->UnlockRect() )
		{
			g_A3DErrLog.Log("A2DSpriteTextures::UpdateTextures(), Can not unlock texture!");
			return false;
		}
	}

	return true;
}

/*
 * FILE: A2DSprite.cpp
 *
 * DESCRIPTION: 2D Graphics Engine for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A3DSurfaceMan.h"
#include "A2DSpriteTexMan.h"
#include "A3DPI.h"
#include "A3DConfig.h"
#include "A3DMacros.h"
#include "A3DSurface.h"
#include "A3DDevice.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

A2DSprite::A2DSprite()
{
	m_pA3DDevice			= NULL;
	m_bHWISprite			= false;
	m_bDynamicTex			= false;

	m_pTextures				= NULL;
	m_pExtraTexture			= NULL;

	m_nItemNum				= 0;
	m_pRects				= NULL;
							
	m_pCurItem				= NULL;
	m_nCurItem				= -1;
							
	m_ptCenterPos.x			= 0.0f;
	m_ptCenterPos.y			= 0.0f;

	m_vZPos					= 0.0f;
	m_vRHW					= 1.0f;
	m_bNeedStereo			= false;

	m_ptLocalCenterPos.x	= 0;
	m_ptLocalCenterPos.y	= 0;

	m_ptScaleCenterPos.x	= 0;
	m_ptScaleCenterPos.y	= 0;

	m_nDegree				= 0;
	m_vScaleX				= 1.0f;
	m_vScaleY				= 1.0f;

	m_color					= A3DCOLORRGBA(255, 255, 255, 255);
	m_SrcBlend				= A3DBLEND_SRCALPHA;
	m_DestBlend				= A3DBLEND_INVSRCALPHA;

	m_bFilterLinear			= g_pA3DConfig->GetFlagLinearSprite();
}

A2DSprite::~A2DSprite()
{
}

bool A2DSprite::InitWithoutSurface(A3DDevice* pA3DDevice, int nWidth, int nHeight, A3DFORMAT format, int nItemCount, A3DRECT* pRects)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWISprite = true;
		return true;
	}

	m_pA3DDevice	= pA3DDevice;
	m_nWidth		= nWidth;
	m_nHeight		= nHeight;

	m_pTextures = new A2DSpriteTextures();
	if( !m_pTextures->InitWithoutSurface(pA3DDevice, nWidth, nHeight, format) )
	{
		g_A3DErrLog.Log("A2DSprite::InitWithoutSurface(), failed to init the sprite textures!");
		return false;
	}

	if( !ResetItems(nItemCount, pRects) )
	{
		g_A3DErrLog.Log("A2DSprite::InitWithoutSurface(), failed to reset items.");
		return false;
	}

	if (m_pA3DDevice)
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_SPRITE);

	return true;
}

/*	Create A2DSprite object.

	pA3DDevice: device object
	szSpriteMap: image file name
	nWidth, nHeight: image size in pixels. if any one of the two values <= 0, they
			will be ignored and the image's real size will be used
	colorKey: color which will be used as color key
	iItemCount: number of iItem. if this parameter == 0, the whole image will be
			treated as one item
	pRects: item area rectangles. if this parameter == 0, the while iamge will be
			treated as one item
	bAutoBuild: build texture automatically
	bNeedBackup: Does texture needs a backup in memory ?
*/
bool A2DSprite::Init(A3DDevice* pA3DDevice, const char* szSpriteMap, int nWidth, int nHeight, A3DCOLOR colorKey, int nItemCount, A3DRECT* pRects)
{
	//FIXME!! DDS texture need lock
	SetDynamicTex(true);

	m_pA3DDevice = pA3DDevice;
	
	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
	{
		m_bHWISprite = true;
		return true;
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->Load2DSpriteTexturesFromFile(
		szSpriteMap, nWidth, nHeight, colorKey, m_bDynamicTex, &m_pTextures) )
	{
		g_A3DErrLog.Log("A2DSprite::Init(), failed to load sprite's textures!");
		return false;
	}
	
	m_nWidth	= m_pTextures->GetWidth();
	m_nHeight	= m_pTextures->GetHeight();
	
	if( !ResetItems(nItemCount, pRects) )
	{
		g_A3DErrLog.Log("A2DSprite::InitWithoutSurface(), failed to reset items.");
		return false;
	}

	SetName(szSpriteMap);

	if (m_pA3DDevice)
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_SPRITE);

	return true;
}

//	This function is a short way to treat whole image as one item
bool A2DSprite::Init(A3DDevice* pA3DDevice, const char* szSpriteMap, A3DCOLOR colorKey)
{
	return Init(pA3DDevice, szSpriteMap, 0, 0, colorKey, 0, NULL);
}

bool A2DSprite::InitFromFileInMemory(A3DDevice* pA3DDevice, const char* szSpriteMap, const void * pMemFile, DWORD dwFileLength, int nWidth, int nHeight, A3DCOLOR colorKey, int nItemCount, A3DRECT * pRects)
{
	m_pA3DDevice = pA3DDevice;
	
	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
	{
		m_bHWISprite = true;
		return true;
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->Load2DSpriteTexturesFromFileInMemory(
		szSpriteMap, pMemFile, dwFileLength, nWidth, nHeight, colorKey, &m_pTextures) )
	{
		g_A3DErrLog.Log("A2DSprite::InitFromFileInMemory(), failed to load sprite's textures!");
		return false;
	}
	
	m_nWidth	= m_pTextures->GetWidth();
	m_nHeight	= m_pTextures->GetHeight();
	
	if( !ResetItems(nItemCount, pRects) )
	{
		g_A3DErrLog.Log("A2DSprite::InitFromFileInMemory(), failed to reset items.");
		return false;
	}

	SetName(szSpriteMap);

	if (m_pA3DDevice)
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_SPRITE);

	return true;
}

bool A2DSprite::InitFromFileInMemory(A3DDevice* pA3DDevice, const char* szSpriteMap, const void * pMemFile, DWORD dwFileLength, A3DCOLOR colorKey)
{
	return InitFromFileInMemory(pA3DDevice, szSpriteMap, pMemFile, dwFileLength, 0, 0, colorKey, 0, NULL);
}

bool A2DSprite::Release()
{
	if (m_bHWISprite) return true;

	if (m_pA3DDevice)
		m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DEngine::OBJECTCNT_SPRITE);

	if( m_pTextures )
	{
		m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->Release2DSpriteTexture(m_pTextures);
		m_pTextures = NULL;
	}

	if( m_pExtraTexture )
		m_pExtraTexture = NULL;
	
	ReleaseItems();

	return true;
}

// Update the content of the textures from the surface;
bool A2DSprite::UpdateTextures(BYTE* pDataBuffer, DWORD dwPitch, A3DFORMAT format, bool bBottomToTop/*=true*/)
{
	if( m_bHWISprite ) return true;

	m_pTextures->UpdateTextures(pDataBuffer, dwPitch, format, bBottomToTop);

	return true;
}

bool A2DSprite::BuildItems()
{
	A3DRECT rect;
	bool	bval;

	for(int i=0; i<m_nItemNum; i++)
	{
		A2DSpriteItem * pItem;

		rect = m_pRects[i];

		if( rect.left < 0 || rect.top < 0 || rect.right > m_nWidth || rect.bottom > m_nHeight )
		{
			g_A3DErrLog.Log("A2DSprite::BuildItems one of the item rects is invalid");
			return false;
		}

		pItem = new A2DSpriteItem;
		if( NULL == pItem )
		{
			g_A3DErrLog.Log("A2DSprite::BuildItems Not enough memory!");
			return false;
		}

		bval = pItem->Init(m_pA3DDevice, this, rect);
		if( !bval )
		{
			g_A3DErrLog.Log("A2DSprite::BuildItems Init Item Fail!");
			return false;
		}

		m_aItems.Add(pItem);
	}

	return true;
}

bool A2DSprite::ReleaseItems()
{
	if (m_pRects)
	{
		delete [] m_pRects;
		m_pRects = NULL;
	}

	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		A2DSpriteItem * pItem = m_aItems[i];
		pItem->Release();
		delete pItem;
	}

	m_aItems.RemoveAll(false);
	return true;
}

bool A2DSprite::DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer, FLOAT x, FLOAT y)
{
	if( m_bHWISprite ) return true;

	if( m_pExtraTexture )
		return DrawWithExtra(x, y);

	if( m_pCurItem )
	{
		if( !m_pCurItem->DrawToBuffer(pSpriteBuffer, x, y, m_vZPos, m_vRHW, m_color, m_nDegree, m_ptLocalCenterPos, m_ptScaleCenterPos, m_vScaleX, m_vScaleY) )
			return false;
	}

	return true;
}

bool A2DSprite::DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer)
{
	return DrawToBuffer(pSpriteBuffer, m_ptCenterPos.x, m_ptCenterPos.y);
}

bool A2DSprite::DrawToBack(FLOAT x, FLOAT y)
{
	if( m_bHWISprite )
		return true;

	if( m_pExtraTexture )
		return DrawWithExtra(x, y);

	if( m_pCurItem )
	{
		if( m_SrcBlend != A3DBLEND_SRCALPHA )
			m_pA3DDevice->SetSourceAlpha(m_SrcBlend);
		if( m_DestBlend != A3DBLEND_INVSRCALPHA )
			m_pA3DDevice->SetDestAlpha(m_DestBlend);

		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

		if( !m_pCurItem->DrawToBack(x, y, m_vZPos, m_vRHW, m_color, m_nDegree, m_ptLocalCenterPos, m_ptScaleCenterPos, m_bFilterLinear, m_vScaleX, m_vScaleY) )
			return false;

		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

		if( m_SrcBlend != A3DBLEND_SRCALPHA )
			m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		if( m_DestBlend != A3DBLEND_INVSRCALPHA )
			m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	}

	return true;
}

bool A2DSprite::DrawToBack()
{
	return DrawToBack(m_ptCenterPos.x, m_ptCenterPos.y);
}

bool A2DSprite::DrawToInternalBuffer()
{
	if( m_pTextures->GetSpriteBuffer() )
	{
		m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->RecordCachedTextures(m_pTextures);
		return DrawToBuffer(m_pTextures->GetSpriteBuffer());
	}
	else
		return DrawToBack();
}

bool A2DSprite::DrawToInternalBuffer(FLOAT x, FLOAT y)
{
	if( m_pTextures->GetSpriteBuffer() )
	{
		m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->RecordCachedTextures(m_pTextures);
		return DrawToBuffer(m_pTextures->GetSpriteBuffer(), x, y);
	}
	else
		return DrawToBack(x, y);
}

bool A2DSprite::SetCurrentItem(int nItem)
{
	if( m_bHWISprite ) return true;

	if( nItem >= m_nItemNum )
		nItem = 0;

	if( nItem < 0 )
		m_pCurItem = NULL;
	else
		m_pCurItem = m_aItems[nItem];

	m_nCurItem = nItem;
	return true;
}

/*
	Get the intersection of one item with a texture
	IN:
		nTexID			texture's id
		rectItem		the item's rect
	OUT:
		rectTexture		if intersect this will be the texture's real covered rect
		rectIntersect	if intersect this will be the intersection rect
	return true if intersects or return false;
*/
bool A2DSprite::GetInterRect(int nTexID, A3DRECT& rectItem, A3DRECT& rectTexture, A3DRECT& rectIntersect)
{
	A3DRECT rectDest, rectSrc1, rectSrc2;

	A2DSpriteTextures::TEXTURE_RECT* pTextureRect = m_pTextures->GetTextureRect(nTexID);
	if (!pTextureRect)
		return false;

	rectSrc1 = rectItem;
	rectSrc2 = pTextureRect->rect;

	if( !IntersectRect((RECT *)&rectDest, (RECT *)&rectSrc1, (RECT *)&rectSrc2) )
		return false;

	rectTexture.left = pTextureRect->rect.left;
	rectTexture.top = pTextureRect->rect.top;
	rectTexture.right = rectTexture.left + pTextureRect->nWidth;
	rectTexture.bottom = rectTexture.top + pTextureRect->nHeight;
	rectIntersect = rectDest;
	return true;
}

//	Reset items
bool A2DSprite::ResetItems(int iItemNum, A3DRECT* aRects)
{
	//	Release old items
	ReleaseItems();

	if (iItemNum > 0 && aRects)
	{
		m_nItemNum = iItemNum;
		if (!(m_pRects = new A3DRECT[m_nItemNum]))
		{
			g_A3DErrLog.Log("A2DSprite::ResetItems() Not enough memory!");
			return false;
		}

		memcpy(m_pRects, aRects, sizeof (A3DRECT) * m_nItemNum);
	}
	else
	{
		m_nItemNum = 1;
		if (!(m_pRects = new A3DRECT[1]))
		{
			g_A3DErrLog.Log("A2DSprite::Init() Not enough memory!");
			return false;
		}

		m_pRects->SetRect(0, 0, m_nWidth, m_nHeight);
	}

	if( !BuildItems() )
	{
		g_A3DErrLog.Log("A2DSprite::ResetItems(), BuildItems failed");
		return false;
	}

	if( m_nItemNum == 0 )
		SetCurrentItem(-1);
	else
		SetCurrentItem(0);

	return true;
}

bool A2DSprite::DrawWithExtra(FLOAT x, FLOAT y)
{
	if( m_bHWISprite || !m_pExtraTexture ) return true;

	if( m_pCurItem )
	{
		if( m_SrcBlend != A3DBLEND_SRCALPHA )
			m_pA3DDevice->SetSourceAlpha(m_SrcBlend);
		if( m_DestBlend != A3DBLEND_INVSRCALPHA )
			m_pA3DDevice->SetDestAlpha(m_DestBlend);

		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

		m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureColorOP(1, A3DTOP_MODULATE);
		m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_MODULATE);

		m_pExtraTexture->Appear(1);

		if( !m_pCurItem->DrawWithExtra(x, y, m_vZPos, m_color, m_nDegree, m_ptLocalCenterPos, m_ptScaleCenterPos, m_bFilterLinear, m_vScaleX, m_vScaleY) )
			return false;

		m_pExtraTexture->Disappear(1);
		m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
		m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);

		if( m_SrcBlend != A3DBLEND_SRCALPHA )
			m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		if( m_DestBlend != A3DBLEND_INVSRCALPHA )
			m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	}

	return true;
}


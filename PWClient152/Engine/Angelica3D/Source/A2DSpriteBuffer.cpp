/*
 * FILE: A2DSpriteBuffer.cpp
 *
 * DESCRIPTION: This is a cache buffer for 2D Graphics Engine for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2003/1/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A2DSprite.h"
#include "A2DSpriteBuffer.h"
#include "A3DPI.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "AMemory.h"
#include "A3DEngine.h"
#include "A2DSpriteTexMan.h"
#include "A3DStream.h"
#include "A3DViewport.h"
#include "A3DVertexShader.h"

#define new A_DEBUG_NEW

A2DSpriteBuffer::A2DSpriteBuffer()
{
	m_pA3DDevice	= NULL;
	m_nTextureNum	= 0;
	m_pTextureSlots = NULL;
}

A2DSpriteBuffer::~A2DSpriteBuffer()
{
}

bool A2DSpriteBuffer::Init(A3DDevice * pA3DDevice, A2DSpriteTextures * pSpriteTextures, int nElementNum)
{
	m_pA3DDevice	= pA3DDevice;
	m_nTextureNum	= pSpriteTextures->GetTextureNum();
	
	// Now alloc the texture slots buffer
	m_pTextureSlots = new TEXTURE_SLOT[m_nTextureNum];
	if( NULL == m_pTextureSlots )
	{
		g_A3DErrLog.Log("A2DSpriteBuffer::Init(), Not enough memory!");
		return false;
	}

	// Init each slot;
	for(int i=0; i<m_nTextureNum; i++)
	{
		m_pTextureSlots[i].nVertsCount = 0;
		m_pTextureSlots[i].pA3DTexture = pSpriteTextures->GetTexture(i);
		m_TextureSlotMap[m_pTextureSlots[i].pA3DTexture] = &m_pTextureSlots[i];
	}
	
	return true;
}

bool A2DSpriteBuffer::Release()
{
	m_TextureSlotMap.clear();

	// First free each slot;
	if( m_pTextureSlots )
	{
		for(int i=0; i<m_nTextureNum; i++)
		{
			m_pTextureSlots[i].aVertices.RemoveAll(true);
		}

		delete [] m_pTextureSlots;
		m_pTextureSlots = NULL;
	}

	return true;
}

bool A2DSpriteBuffer::FlushBuffer()
{
	// First of all, setup render states;
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	for(int i=0; i<m_nTextureNum; i++)
	{
		if( m_pTextureSlots[i].nVertsCount > 0 )
		{
			if( !m_pTextureSlots[i].bFilterLinear )
				m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
			
			if( m_pTextureSlots[i].srcBlend != A3DBLEND_SRCALPHA )
				m_pA3DDevice->SetSourceAlpha(m_pTextureSlots[i].srcBlend);

			if( m_pTextureSlots[i].destBlend != A3DBLEND_INVSRCALPHA )
				m_pA3DDevice->SetDestAlpha(m_pTextureSlots[i].destBlend);

			m_pTextureSlots[i].pA3DTexture->Appear(0);
			
			A3DTLVERTEX * pVerts;
			int	nVertStart = 0;
			int nMaxBlades = m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->GetMaxBlades();

			int nNumBlades = m_pTextureSlots[i].nVertsCount / 6;
			if( nNumBlades > nMaxBlades )
				nNumBlades = nMaxBlades;
			if( !m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->LockStream((BYTE**) &pVerts, nNumBlades * 6, nVertStart) )
			{
				m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
				return false;
			}
			memcpy(pVerts, m_pTextureSlots[i].aVertices.GetData(), nNumBlades * 6 * sizeof(A3DTLVERTEX));
			m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->UnlockStream();

			m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->GetStream()->Appear();

			if( m_pTextureSlots[i].bNeedStereo && m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->GetStereoShader() )
			{
				A3DViewport * pView = m_pA3DDevice->GetA3DEngine()->GetActiveViewport();
				m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->GetStereoShader()->Appear();
				A3DVECTOR4 c0(2.0f / pView->GetParam()->Width, -2.0f / pView->GetParam()->Height, 1.0f, 0.0f);
				m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);
				A3DVECTOR4 c1((float)pView->GetParam()->X, (float)pView->GetParam()->Y, 0.0f, 0.0f);
				m_pA3DDevice->SetVertexShaderConstants(1, &c1, 1);
			}

			m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, nVertStart, nNumBlades * 2);

			m_pTextureSlots[i].pA3DTexture->Disappear(0);
			
			m_pTextureSlots[i].nVertsCount = 0;
			m_pTextureSlots[i].aVertices.RemoveAll(false);
			
			if( m_pTextureSlots[i].srcBlend != A3DBLEND_SRCALPHA )
				m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);

			if( m_pTextureSlots[i].destBlend != A3DBLEND_INVSRCALPHA )
				m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

			if( !m_pTextureSlots[i].bFilterLinear )
				m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
		}
	}
	
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	return true;
}

bool A2DSpriteBuffer::AddToBuffer(A3DTexture * pTexture, A3DTLVERTEX * v, A3DBLEND srcBlend, A3DBLEND destBlend, bool bFilterLinear, bool bNeedStereo)
{
	TEXTURE_SLOT * pSlot = NULL;

	TEXTURESLOTMAP::iterator it = m_TextureSlotMap.find(pTexture);
	if (it != m_TextureSlotMap.end())
		pSlot = it->second;
	else
		return false; // not found
	
	int nVertsCount = pSlot->nVertsCount;
	pSlot->aVertices.Add(v[0]);
	pSlot->aVertices.Add(v[1]);
	pSlot->aVertices.Add(v[2]);

	pSlot->aVertices.Add(v[1]);
	pSlot->aVertices.Add(v[3]);
	pSlot->aVertices.Add(v[2]);
								
	pSlot->nVertsCount += 6;

	pSlot->bFilterLinear = bFilterLinear;
	pSlot->bNeedStereo = bNeedStereo;
	pSlot->srcBlend = srcBlend;
	pSlot->destBlend = destBlend;
	return true;
}
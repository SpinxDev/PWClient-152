/*
 * FILE: A2DSpriteItem.cpp
 *
 * DESCRIPTION: Sprite Item class
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A2DSpriteItem.h"
#include "A2DSprite.h"
#include "A2DSpriteBuffer.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "A3DEngine.h"
#include "A3DStream.h"
#include "A2DSpriteTexMan.h"
#include "A3DVertexShader.h"
#include "A3DViewport.h"

A2DSpriteItem::A2DSpriteItem()
{
	m_pA3DDevice = NULL;
	m_pParentSprite = NULL;
}

A2DSpriteItem::~A2DSpriteItem()
{
}

bool A2DSpriteItem::Init(A3DDevice * pA3DDevice, A2DSprite * pParentSprite, A3DRECT& rect)
{
	m_pA3DDevice	= pA3DDevice;
	m_pParentSprite = pParentSprite;
	m_rectItem		= rect;

	m_ListSlices.Init();

	for(int i=0; i<m_pParentSprite->GetTextureNum(); i++)
	{
		A3DRECT rectTexture, rectIntersect;

		if( m_pParentSprite->GetInterRect(i, m_rectItem, rectTexture, rectIntersect) )
		{
			SPRITESLICE * pSlice = (SPRITESLICE *) malloc(sizeof(SPRITESLICE));
			if( NULL == pSlice )
			{
				g_A3DErrLog.Log("A2DSpriteItem::Init Not enough memory!");
				return false;
			}

			pSlice->nTextureID	= i;
			pSlice->rect.left	= rectIntersect.left - m_rectItem.left;
			pSlice->rect.top	= rectIntersect.top - m_rectItem.top;
			pSlice->rect.right	= rectIntersect.right - m_rectItem.left;
			pSlice->rect.bottom = rectIntersect.bottom - m_rectItem.top;
			pSlice->u0 = (rectIntersect.left - rectTexture.left) * 1.0f / (rectTexture.right - rectTexture.left);
			pSlice->u1 = (rectIntersect.right - rectTexture.left) * 1.0f / (rectTexture.right - rectTexture.left);
			pSlice->v0 = (rectIntersect.top - rectTexture.top) * 1.0f / (rectTexture.bottom - rectTexture.top);
			pSlice->v1 = (rectIntersect.bottom - rectTexture.top) * 1.0f / (rectTexture.bottom - rectTexture.top);
			
			m_ListSlices.Append((LPVOID) pSlice);
		}
	}
	return true;
}

bool A2DSpriteItem::Release()
{
	ALISTELEMENT * pThisSliceElement = m_ListSlices.GetFirst();
	while( pThisSliceElement != m_ListSlices.GetTail() )
	{
		SPRITESLICE * pSlice = (SPRITESLICE *)pThisSliceElement->pData;
		free(pSlice);
		pSlice = NULL;

		pThisSliceElement = pThisSliceElement->pNext;
	}
	m_ListSlices.Release();

	return true;
}

bool A2DSpriteItem::DrawToBack(FLOAT x, FLOAT y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY)
{
	ALISTELEMENT * pThisSliceElement = m_ListSlices.GetFirst();

	if( !bFilterLinear )
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	
	m_pA3DDevice->SetFVF(A3DFVF_A3DTLVERTEX);

	while( pThisSliceElement != m_ListSlices.GetTail() )
	{
		SPRITESLICE * pSlice = (SPRITESLICE *)pThisSliceElement->pData;
		
		if( !Render(x, y, vZPos, vRHW, color, nDegree, ptLocalCenter, ptScaleCenter, pSlice, bFilterLinear, vScaleX, vScaleY) )
		{
			if( !bFilterLinear )
				m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
			return false;
		}
		
		pThisSliceElement = pThisSliceElement->pNext;
	}

	if( !bFilterLinear )
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	return true;
}

bool A2DSpriteItem::Render(FLOAT x, FLOAT y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, SPRITESLICE * pSlice, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY)
{
	A3DCOLOR	diffuse = color;
	A3DCOLOR	specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR3  vec3[4];
	A3DVECTOR3  vecCenter;
	A3DVECTOR4	vec4[4];

	A3DMATRIX4  matRotate;

	A3DTLVERTEX	v[4];

	A3DTexture * pTexture = m_pParentSprite->GetTexture(pSlice->nTextureID);
	if( !pTexture )
	{
		g_A3DErrLog.Log("A2DSpriteItem::Render, pTexture failed");
		return false;
	}
	//First construct a rect in the coorinates of ptLocalCenter;
	//NOTICE: pSlice->rect is in item space, that is to say, at the item's left-top corner, pSlice->rect.left and top are both 0
	if( ptLocalCenter == ptScaleCenter || (vScaleX == 1.0f && vScaleY == 1.0f) )
	{
		vec3[0] = A3DVECTOR3((pSlice->rect.left - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.top - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
		vec3[1] = A3DVECTOR3((pSlice->rect.right - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.top - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
		vec3[2] = A3DVECTOR3((pSlice->rect.left - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.bottom - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
		vec3[3] = A3DVECTOR3((pSlice->rect.right - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.bottom - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
	}
	else
	{	
		vec3[0] = A3DVECTOR3((pSlice->rect.left - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.top - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);
		vec3[1] = A3DVECTOR3((pSlice->rect.right - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.top - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);
		vec3[2] = A3DVECTOR3((pSlice->rect.left - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.bottom - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);
		vec3[3] = A3DVECTOR3((pSlice->rect.right - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.bottom - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);

		vec3[0].x = vec3[0].x - ptLocalCenter.x - 0.5f; vec3[0].y = vec3[0].y - ptLocalCenter.y - 0.5f;
		vec3[1].x = vec3[1].x - ptLocalCenter.x - 0.5f; vec3[1].y = vec3[1].y - ptLocalCenter.y - 0.5f;
		vec3[2].x = vec3[2].x - ptLocalCenter.x - 0.5f; vec3[2].y = vec3[2].y - ptLocalCenter.y - 0.5f;
		vec3[3].x = vec3[3].x - ptLocalCenter.x - 0.5f; vec3[3].y = vec3[3].y - ptLocalCenter.y - 0.5f;
	}

	vecCenter = A3DVECTOR3(x, y, 0.0f);

	matRotate = a3d_RotateZ(DEG2RAD(nDegree));
	for(int i=0; i<4; i++)
	{
		if( nDegree != 0 )
			vec3[i] = vec3[i] * matRotate;

		vec3[i] = vec3[i] + vecCenter;
		vec4[i] = A3DVECTOR4(vec3[i].x, vec3[i].y, vZPos, vRHW);
	}

	v[0] = A3DTLVERTEX(vec4[0], diffuse, specular, pSlice->u0, pSlice->v0);
	v[1] = A3DTLVERTEX(vec4[1], diffuse, specular, pSlice->u1, pSlice->v0);
	v[2] = A3DTLVERTEX(vec4[2], diffuse, specular, pSlice->u0, pSlice->v1);
	v[3] = A3DTLVERTEX(vec4[3], diffuse, specular, pSlice->u1, pSlice->v1);

	A3DTLVERTEX * pVerts;
	int	nVertStart = 0;
	if( !m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->LockStream((BYTE**) &pVerts, 6, nVertStart) )
	{
		g_A3DErrLog.Log("A2DSpriteItem::Render, LockStream failed");
		return false;
	}	
	// face is 0-1-2, 1-3-2
	pVerts[0] = v[0];
	pVerts[1] = v[1];
	pVerts[2] = v[2];
	pVerts[3] = v[1];
	pVerts[4] = v[3];
	pVerts[5] = v[2];

	m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->UnlockStream();
	
	m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->GetStream()->Appear();

	if( m_pParentSprite->IsNeedStereo() && m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->GetStereoShader() )
	{
		A3DViewport * pView = m_pA3DDevice->GetA3DEngine()->GetActiveViewport();
		m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->GetStereoShader()->Appear();
		A3DVECTOR4 c0(2.0f / pView->GetParam()->Width, -2.0f / pView->GetParam()->Height, 1.0f, 0.0f);
		m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);
		A3DVECTOR4 c1((float)pView->GetParam()->X, (float)pView->GetParam()->Y, 0.0f, 0.0f);
		m_pA3DDevice->SetVertexShaderConstants(1, &c1, 1);
	}

	pTexture->Appear(0);
	
	if( !m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, nVertStart, 2) )
		return false;

	pTexture->Disappear(0);
	
	return true;
}

bool A2DSpriteItem::DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer, FLOAT x, FLOAT y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, FLOAT vScaleX, FLOAT vScaleY)
{
	ALISTELEMENT * pThisSliceElement = m_ListSlices.GetFirst();

	while( pThisSliceElement != m_ListSlices.GetTail() )
	{
		SPRITESLICE * pSlice = (SPRITESLICE *)pThisSliceElement->pData;
		
		if( !RenderToBuffer(pSpriteBuffer, x, y, vZPos, vRHW, color, nDegree, ptLocalCenter, ptScaleCenter, pSlice, vScaleX, vScaleY) )
		{
			return false;
		}
		
		pThisSliceElement = pThisSliceElement->pNext;
	}

	return true;
}

bool A2DSpriteItem::RenderToBuffer(A2DSpriteBuffer * pSpriteBuffer, FLOAT x, FLOAT y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, SPRITESLICE * pSlice, FLOAT vScaleX, FLOAT vScaleY)
{
	A3DCOLOR	diffuse = color;
	A3DCOLOR	specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR3  vec3[4];
	A3DVECTOR3  vecCenter;
	A3DVECTOR4	vec4[4];

	A3DMATRIX4  matRotate;

	A3DTLVERTEX	v[4];

	A3DTexture * pTexture = m_pParentSprite->GetTexture(pSlice->nTextureID);
	if( !pTexture ) return false;

	//First construct a rect in the coorinates of ptLocalCenter;
	//NOTICE: pSlice->rect is in item space, that is to say, at the item's left-top corner, pSlice->rect.left and top are both 0
	if( ptLocalCenter == ptScaleCenter || (vScaleX == 1.0f && vScaleY == 1.0f) )
	{
		vec3[0] = A3DVECTOR3((pSlice->rect.left - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.top - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
		vec3[1] = A3DVECTOR3((pSlice->rect.right - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.top - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
		vec3[2] = A3DVECTOR3((pSlice->rect.left - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.bottom - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
		vec3[3] = A3DVECTOR3((pSlice->rect.right - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.bottom - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
	}
	else
	{	
		vec3[0] = A3DVECTOR3((pSlice->rect.left - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.top - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);
		vec3[1] = A3DVECTOR3((pSlice->rect.right - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.top - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);
		vec3[2] = A3DVECTOR3((pSlice->rect.left - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.bottom - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);
		vec3[3] = A3DVECTOR3((pSlice->rect.right - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.bottom - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);

		vec3[0].x = vec3[0].x - ptLocalCenter.x - 0.5f; vec3[0].y = vec3[0].y - ptLocalCenter.y - 0.5f;
		vec3[1].x = vec3[1].x - ptLocalCenter.x - 0.5f; vec3[1].y = vec3[1].y - ptLocalCenter.y - 0.5f;
		vec3[2].x = vec3[2].x - ptLocalCenter.x - 0.5f; vec3[2].y = vec3[2].y - ptLocalCenter.y - 0.5f;
		vec3[3].x = vec3[3].x - ptLocalCenter.x - 0.5f; vec3[3].y = vec3[3].y - ptLocalCenter.y - 0.5f;
	}

	vecCenter = A3DVECTOR3(x, y, 0.0f);

	matRotate = a3d_RotateZ(DEG2RAD(nDegree));
	for(int i=0; i<4; i++)
	{
		if( nDegree != 0 )
			vec3[i] = vec3[i] * matRotate;

		vec3[i] = vec3[i] + vecCenter;
		vec4[i] = A3DVECTOR4(vec3[i].x, vec3[i].y, vZPos, vRHW);
	}

	v[0] = A3DTLVERTEX(vec4[0], diffuse, specular, pSlice->u0, pSlice->v0);
	v[1] = A3DTLVERTEX(vec4[1], diffuse, specular, pSlice->u1, pSlice->v0);
	v[2] = A3DTLVERTEX(vec4[2], diffuse, specular, pSlice->u0, pSlice->v1);
	v[3] = A3DTLVERTEX(vec4[3], diffuse, specular, pSlice->u1, pSlice->v1);

	return pSpriteBuffer->AddToBuffer(pTexture, v, m_pParentSprite->GetSrcBlend(), m_pParentSprite->GetDestBlend(), m_pParentSprite->IsLinearFilter(), m_pParentSprite->IsNeedStereo());
}

bool A2DSpriteItem::DrawWithExtra(FLOAT x, FLOAT y, FLOAT vZPos, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY)
{
	ALISTELEMENT * pThisSliceElement = m_ListSlices.GetFirst();

	if( !bFilterLinear )
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
	
	m_pA3DDevice->SetFVF(A3DFVF_A3DTLVERTEX2);

	while( pThisSliceElement != m_ListSlices.GetTail() )
	{
		SPRITESLICE * pSlice = (SPRITESLICE *)pThisSliceElement->pData;
		
		if( !RenderWithExtra(x, y, vZPos, color, nDegree, ptLocalCenter, ptScaleCenter, pSlice, bFilterLinear, vScaleX, vScaleY) )
		{
			if( !bFilterLinear )
				m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
			return false;
		}
		
		pThisSliceElement = pThisSliceElement->pNext;
	}

	if( !bFilterLinear )
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	return true;
}

bool A2DSpriteItem::RenderWithExtra(FLOAT x, FLOAT y, FLOAT vZPos, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, SPRITESLICE * pSlice, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY)
{
	A3DCOLOR	diffuse = color;
	A3DCOLOR	specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR3  vec3[4];
	A3DVECTOR3  vecCenter;
	A3DVECTOR4	vec4[4];

	A3DMATRIX4  matRotate;

	A3DTLVERTEX2 v[4];

	A3DTexture * pTexture = m_pParentSprite->GetTexture(pSlice->nTextureID);
	if( !pTexture )
	{
		g_A3DErrLog.Log("A2DSpriteItem::RenderWithExtra, pTexture failed");
		return false;
	}
	//First construct a rect in the coorinates of ptLocalCenter;
	//NOTICE: pSlice->rect is in item space, that is to say, at the item's left-top corner, pSlice->rect.left and top are both 0
	if( ptLocalCenter == ptScaleCenter || (vScaleX == 1.0f && vScaleY == 1.0f) )
	{
		vec3[0] = A3DVECTOR3((pSlice->rect.left - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.top - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
		vec3[1] = A3DVECTOR3((pSlice->rect.right - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.top - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
		vec3[2] = A3DVECTOR3((pSlice->rect.left - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.bottom - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
		vec3[3] = A3DVECTOR3((pSlice->rect.right - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.bottom - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
	}
	else
	{	
		vec3[0] = A3DVECTOR3((pSlice->rect.left - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.top - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);
		vec3[1] = A3DVECTOR3((pSlice->rect.right - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.top - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);
		vec3[2] = A3DVECTOR3((pSlice->rect.left - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.bottom - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);
		vec3[3] = A3DVECTOR3((pSlice->rect.right - ptScaleCenter.x) * vScaleX + ptScaleCenter.x, (pSlice->rect.bottom - ptScaleCenter.y) * vScaleY + ptScaleCenter.y, 0.0f);

		vec3[0].x = vec3[0].x - ptLocalCenter.x - 0.5f; vec3[0].y = vec3[0].y - ptLocalCenter.y - 0.5f;
		vec3[1].x = vec3[1].x - ptLocalCenter.x - 0.5f; vec3[1].y = vec3[1].y - ptLocalCenter.y - 0.5f;
		vec3[2].x = vec3[2].x - ptLocalCenter.x - 0.5f; vec3[2].y = vec3[2].y - ptLocalCenter.y - 0.5f;
		vec3[3].x = vec3[3].x - ptLocalCenter.x - 0.5f; vec3[3].y = vec3[3].y - ptLocalCenter.y - 0.5f;
	}

	vecCenter = A3DVECTOR3(x, y, 0.0f);

	matRotate = a3d_RotateZ(DEG2RAD(nDegree));
	for(int i=0; i<4; i++)
	{
		if( nDegree != 0 )
			vec3[i] = vec3[i] * matRotate;

		vec3[i] = vec3[i] + vecCenter;
		vec4[i] = A3DVECTOR4(vec3[i].x, vec3[i].y, vZPos, 1.0f);
	}

	float eu0 = (float)pSlice->rect.left / m_rectItem.Width();
	float eu1 = (float)pSlice->rect.right / m_rectItem.Width();
	float ev0 = (float)pSlice->rect.top / m_rectItem.Height();
	float ev1 = (float)pSlice->rect.bottom / m_rectItem.Height();

	v[0] = A3DTLVERTEX2(vec4[0], diffuse, specular, pSlice->u0, pSlice->v0, eu0, ev0);
	v[1] = A3DTLVERTEX2(vec4[1], diffuse, specular, pSlice->u1, pSlice->v0, eu1, ev0);
	v[2] = A3DTLVERTEX2(vec4[2], diffuse, specular, pSlice->u0, pSlice->v1, eu0, ev1);
	v[3] = A3DTLVERTEX2(vec4[3], diffuse, specular, pSlice->u1, pSlice->v1, eu1, ev1);

	A3DTLVERTEX2 * pVerts;
	int	nVertStart = 0;
	if( !m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->LockStream2Layer((BYTE**) &pVerts, 6) )
	{
		g_A3DErrLog.Log("A2DSpriteItem::RenderWithExtra, LockStream2Layer failed");
		return false;
	}	
	// face is 0-1-2, 1-3-2
	pVerts[0] = v[0];
	pVerts[1] = v[1];
	pVerts[2] = v[2];
	pVerts[3] = v[1];
	pVerts[4] = v[3];
	pVerts[5] = v[2];

	m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->UnlockStream2Layer();
	m_pA3DDevice->GetA3DEngine()->GetA2DSpriteTexMan()->GetStream2Layer()->Appear();
	pTexture->Appear(0);
	
	if( !m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, 2) )
		return false;

	pTexture->Disappear(0);
	
	return true;
}

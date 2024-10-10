/*
 * FILE: A2DSpriteItem.h
 *
 * DESCRIPTION: Sprite Item class
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A2DSPRITEITEM_H_
#define _A2DSPRITEITEM_H_

#include "A3DVertex.h"
#include "AList.h"
	   
class A2DSprite;
class A2DSpriteBuffer;
class A3DDevice;

class A2DSpriteItem
{
public:
	typedef struct _SPRITESLICE
	{
		// Rectangle in which this slice exist;
		A3DRECT rect;
		// Four Corner's UV Coordinates;
		FLOAT	u0, u1;
		FLOAT	v0, v1;
		int		nTextureID;
	} SPRITESLICE;

private:
	A3DDevice *		m_pA3DDevice;
	A2DSprite *		m_pParentSprite;
	A3DRECT			m_rectItem;

	AList			m_ListSlices;

protected:
public:
	A2DSpriteItem();
	~A2DSpriteItem();

	bool Init(A3DDevice * pA3DDevice, A2DSprite * pParentSprite, A3DRECT& itemRect);
	bool Release();

	// Show this item onto screen;
	bool DrawToBack(int x, int y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY) 
	{ 
		return DrawToBack((float)x, (float)y, vZPos, vRHW, color, nDegree, ptLocalCenter, ptScaleCenter, bFilterLinear, vScaleX, vScaleY); 
	}
	bool DrawToBack(FLOAT x, FLOAT y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY);

	// Draw into the sprite buffer;
	bool DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer, int x, int y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, FLOAT vScaleX, FLOAT vScaleY)
	{
		return DrawToBuffer(pSpriteBuffer, (float)x, (float)y, vZPos, vRHW, color, nDegree, ptLocalCenter, ptScaleCenter, vScaleX, vScaleY);
	}
	bool DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer, FLOAT x, FLOAT y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, FLOAT vScaleX, FLOAT vScaleY);

	// Draw to back with extra texture
	bool DrawWithExtra(FLOAT x, FLOAT y, FLOAT vZPos, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY);
	
	A3DRECT & GetRect() { return m_rectItem; }

private:
	bool Render(FLOAT x, FLOAT y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, SPRITESLICE * pSlice, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY);
	bool RenderToBuffer(A2DSpriteBuffer * pSpriteBuffer, FLOAT x, FLOAT y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, SPRITESLICE * pSlice, FLOAT vScaleX, FLOAT vScaleY);
	bool RenderWithExtra(FLOAT x, FLOAT y, FLOAT vZPos, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, SPRITESLICE * pSlice, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY);

	bool Render(int x, int y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, SPRITESLICE * pSlice, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY)
	{
		return Render((float)x, (float)y, vZPos, vRHW, color, nDegree, ptLocalCenter, ptScaleCenter, pSlice, bFilterLinear, vScaleX, vScaleY);
	}
	bool RenderToBuffer(A2DSpriteBuffer * pSpriteBuffer, int x, int y, FLOAT vZPos, FLOAT vRHW, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, A3DPOINT2& ptScaleCenter, SPRITESLICE * pSlice, FLOAT vScaleX, FLOAT vScaleY)
	{
		return RenderToBuffer(pSpriteBuffer, (float)x, (float)y, vZPos, vRHW, color, nDegree, ptLocalCenter, ptScaleCenter, pSlice, vScaleX, vScaleY);
	}
};

typedef A2DSpriteItem * PA2DSpriteItem;

#endif //_A2DSPRITEITEM_H_
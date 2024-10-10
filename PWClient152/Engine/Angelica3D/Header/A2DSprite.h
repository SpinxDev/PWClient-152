/*
 * FILE: A2DSprite.h
 *
 * DESCRIPTION: 2D Graphics Engine for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A2DSPRITE_H_
#define _A2DSPRITE_H_

#include "A3DTypes.h"
#include "A3DObject.h"
#include "AArray.h"
#include "A2DSpriteTextures.h"

class A3DTexture;
class A3DDevice;
class A3DSurface;

class A2DSprite : public A3DObject
{
	friend class A2DSpriteBuffer;
	friend class A2DSpriteItem;

protected:
	A3DDevice *				m_pA3DDevice;
	bool					m_bHWISprite;	// Flag indicates using outside graphics engine;
	bool					m_bDynamicTex;

	A2DSpriteTextures *		m_pTextures;	// textures that has been built from the surface

	A3DRECT *				m_pRects;		// item rects.
	APtrArray<A2DSpriteItem*>m_aItems;		// items array

	int						m_nWidth;		// width of the sprite
	int						m_nHeight;		// height of the sprite

	// This is the position where the center of the sprite locates;
	APointF					m_ptCenterPos;
	float					m_vZPos;		// position of z on screen coordinates.
	float					m_vRHW;			// rhw of this sprite
	bool					m_bNeedStereo;	// flag indicate whether this sprite wants 3D stereo effect
	
	// This is the degree of this sprite, range [0~360); Left handed; Screen Coordinates
	int						m_nDegree;

	// This is the local center position relative to the left-top corner of the sprite;
	A3DPOINT2				m_ptLocalCenterPos;
	// This is the local scale center position relative to the left-top corner of the sprite;
	A3DPOINT2				m_ptScaleCenterPos;

	// This is the color of this sprite, pay some attention to its alpha channel, because it will take effect;
	A3DBLEND				m_SrcBlend;
	A3DBLEND				m_DestBlend;			
	A3DCOLOR				m_color;

	// This is the scale factor that this sprite will applied;
	FLOAT					m_vScaleX;
	FLOAT					m_vScaleY;

	// This flag controls wether use linear interpolation or not;
	// For large still sprites, be sure not set this flag, but for small rotating sprite, you may set this 
	bool					m_bFilterLinear;

	int						m_nCurItem;
	int						m_nItemNum;
	A2DSpriteItem *			m_pCurItem;

	A3DTexture *			m_pExtraTexture; // now extra texture will modulate with the sprite

public:
	A2DSprite();
	virtual ~A2DSprite();

	bool InitWithoutSurface(A3DDevice* pA3DDevice, int nWidth, int nHeight, A3DFORMAT format, int nItemCount, A3DRECT* pRects);

	bool Init(A3DDevice* pA3DDevice, const char* szSpriteMap, int nWidth, int nHeight, A3DCOLOR colorKey, int nItemCount, A3DRECT * pRects);
	bool Init(A3DDevice* pA3DDevice, const char* szSpriteMap, A3DCOLOR colorKey);

	// init from memory version
	bool InitFromFileInMemory(A3DDevice* pA3DDevice, const char* szSpriteMap, const void * pMemFile, DWORD dwFileLength, int nWidth, int nHeight, A3DCOLOR colorKey, int nItemCount, A3DRECT * pRects);
	bool InitFromFileInMemory(A3DDevice* pA3DDevice, const char* szSpriteMap, const void * pMemFile, DWORD dwFileLength, A3DCOLOR colorKey);

	bool Release();

	// Update the content of the textures from one image buffer;
	bool UpdateTextures(BYTE* pDataBuffer, DWORD dwPitch, A3DFORMAT format, bool bBottomToTop = true);

protected:
	bool BuildItems();
	bool ReleaseItems();
	bool DrawWithExtra(FLOAT x, FLOAT y);

public:
	// Draw to back at the position of (x, y);
	bool DrawToBack(int x, int y) { return DrawToBack((float)x, (float)y); }
	bool DrawToBack(FLOAT x, FLOAT y);
	// Draw to back using preset m_ptPos;
	bool DrawToBack();

	// Draw to Sprite Buffer to reduce the call to DrawPrimitives
	bool DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer, int x, int y) { return DrawToBuffer(pSpriteBuffer, (float)x, (float)y); }
	bool DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer, FLOAT x, FLOAT y);
	bool DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer);

	// Draw to the internal sprite buffer in SpriteTextures
	bool DrawToInternalBuffer(int x, int y) { return DrawToInternalBuffer((float)x, (float)y); }
	bool DrawToInternalBuffer(FLOAT x, FLOAT y);
	bool DrawToInternalBuffer();

	//	Reset items
	bool ResetItems(int iItemNum, A3DRECT* aRects);
	//	Set current active item to show;
	bool SetCurrentItem(int nItem);
	//	Get a sprite item pointer;
	A2DSpriteItem* GetItem(int nIndex) { return m_aItems[nIndex]; }

protected:
	bool GetInterRect(int nTexID, A3DRECT& rectItem, A3DRECT& rectTexture, A3DRECT& rectIntersect);

public:
	inline A3DTexture * GetExtraTexture() { return m_pExtraTexture; }
	inline void SetExtraTexture(A3DTexture * pTexture) { m_pExtraTexture = pTexture; }
	inline A2DSpriteTextures * GetTextures() { return m_pTextures; }
	inline int GetTextureNum() { return m_pTextures->GetTextureNum(); }
	inline A3DTexture * GetTexture(int index) { return m_pTextures->GetTexture(index); }
	inline int GetItemNum() { return m_nItemNum; }
	
	inline APointF GetPosition() { return m_ptCenterPos; }
	inline void SetPosition(FLOAT x, FLOAT y) { m_ptCenterPos.x = x; m_ptCenterPos.y = y; }
	inline void SetPosition(int x, int y) { SetPosition((float)x, (float)y); }

	inline float GetZPos()			{ return m_vZPos; }
	inline void SetZPos(float v, float rhw=1.0f)	{ m_vZPos = v; m_vRHW = rhw; }

	inline A3DPOINT2 GetLocalCenterPosition() { return m_ptLocalCenterPos; }
	inline void SetLocalCenterPos(int x, int y, bool bScaleCenterIsSame=true) 
	{ 
		m_ptLocalCenterPos.x = x; m_ptLocalCenterPos.y = y; 
		if( bScaleCenterIsSame )
			SetScaleCenterPos(x, y);
	}
	inline A3DPOINT2 GetScaleCenterPosition() { return m_ptScaleCenterPos; }
	inline void SetScaleCenterPos(int x, int y)
	{
		m_ptScaleCenterPos.x = x; m_ptScaleCenterPos.y = y;
	}
	
	inline int GetDegree() { return m_nDegree; }
	inline void SetDegree(int nDegree) { m_nDegree = nDegree; }

	//	Get image size
	inline int GetWidth() { return m_nWidth; }
	inline int GetHeight() { return m_nHeight; }

	inline void SetColor(A3DCOLOR color) { m_color = color; }
	inline A3DCOLOR GetColor() { return m_color; }

	inline void SetAlpha(int alpha) { m_color = (m_color & 0x00ffffff) | (alpha << 24); }
	inline int GetAlpha() { return (m_color & 0xff000000) >> 24; }

	inline void SetScaleX(FLOAT vScaleX) { m_vScaleX = vScaleX; }
	inline void SetScaleY(FLOAT vScaleY) { m_vScaleY = vScaleY; }
	inline FLOAT GetScaleX() { return m_vScaleX; }
	inline FLOAT GetScaleY() { return m_vScaleY; }

	inline void SetLinearFilter(bool bEnable) { m_bFilterLinear = bEnable; }
	inline bool IsLinearFilter() { return m_bFilterLinear; }

	inline void SetSrcBlend(A3DBLEND srcBlend) { m_SrcBlend = srcBlend; }
	inline void SetDestBlend(A3DBLEND destBlend) { m_DestBlend = destBlend; }
	inline A3DBLEND GetSrcBlend() { return m_SrcBlend; }
	inline A3DBLEND GetDestBlend() { return m_DestBlend; }
	inline bool IsNeedStereo() { return m_bNeedStereo; }
	inline void SetNeedStereo(bool bFlag) { m_bNeedStereo = bFlag; }

	void SetDynamicTex(bool b) { m_bDynamicTex = b; }

	bool IsDynamicTexEnabled() const
	{
		return m_pTextures && m_pTextures->IsDynamicTex();
	}
};

	
#endif //_A2DSPRITE_H_

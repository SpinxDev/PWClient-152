/*
 * FILE: A2DSpriteTextures.h
 *
 * DESCRIPTION: Textures build for 2D sprite for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A2DSPRITETEXTURES_H_
#define _A2DSPRITETEXTURES_H_

#include "A3DTypes.h"
#include "A3DObject.h"
#include "AArray.h"

class A3DTexture;
class A3DDevice;
class A3DSurface;
class A2DSpriteBuffer;

class A2DSpriteTextures : public A3DObject
{
	friend class A2DSpriteTexMan;

public:
	struct TEXTURE_RECT
	{
		A3DRECT		rect;
		int			nWidth;
		int			nHeight;
		A3DTexture*	pA3DTexture;
	};

protected:
	A3DDevice *				m_pA3DDevice;
	
	DWORD					m_dwPosInMan;

	A3DFORMAT				m_Format;
	int						m_nTextureNum;
	APtrArray<TEXTURE_RECT*>m_aTextures;
	
	int						m_nWidth;
	int						m_nHeight;
	
	A2DSpriteBuffer *		m_pSpriteBuffer;		// a buffer to cache all sprites using this texture

	bool					m_bHasBeenCached;		// flag indicates whether this sprite textures has been cached from last flush.
	int						m_nUserRef;
	bool					m_bDynamicTex;

public:
	A2DSpriteTextures();
	virtual ~A2DSpriteTextures();

	bool Init(A3DDevice* pA3DDevice, const char* szSpriteMap, int nWidth, int nHeight, A3DCOLOR colorKey);
	bool InitFromFileInMemory(A3DDevice* pA3DDevice, const char* szSpriteMap, const void * pMemFile, DWORD dwFileLength, int nWidth, int nHeight, A3DCOLOR colorKey);
	bool InitWithoutSurface(A3DDevice* pA3DDevice, int nWidth, int nHeight, A3DFORMAT format);
	bool Release();

	// Copy the surface into a3dtextures for using;
	bool BuildTextures(A3DSurface * pSrcSurface);
	// Unload these a3dtextures to free some video memory!
	bool UnloadTextures();

	// Update the content of the textures from one image buffer;
	bool UpdateTextures(BYTE* pDataBuffer, DWORD dwPitch, A3DFORMAT format, bool bBottomToTop = true);

public:
	inline int GetWidth()					{ return m_nWidth; }
	inline int GetHeight()					{ return m_nHeight; }

	inline int GetTextureNum()				{ return m_nTextureNum; }
	inline A3DTexture* GetTexture(int n)	{ return m_aTextures[n]->pA3DTexture; }
	inline TEXTURE_RECT * GetTextureRect(int n) { return m_aTextures[n]; }
	inline A2DSpriteBuffer * GetSpriteBuffer() { return m_pSpriteBuffer; }

	int GetUserRef() const { return m_nUserRef; }
	void IncUserRef() { m_nUserRef++; }
	void DecUserRef() { m_nUserRef--; }

	bool IsDynamicTex() const { return m_bDynamicTex; }
	void SetDynamicTex(bool b) { m_bDynamicTex = b; }
};

#endif //_A2DSPRITETEXTURES_H_

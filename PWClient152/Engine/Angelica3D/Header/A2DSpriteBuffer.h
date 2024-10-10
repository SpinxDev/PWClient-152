/*
 * FILE: A2DSpriteBuffer.h
 *
 * DESCRIPTION: This is a cache buffer for 2D Graphics Engine for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2003/1/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A2DSPRITEBUFFER_H_
#define _A2DSPRITEBUFFER_H_

#include "A3DVertex.h"
#include "hashmap.h"
#include "AArray.h"

class A3DDevice;
class A2DSprite;
class A3DTexture;
class A3DViewport;
class A2DSpriteTextures;

class A2DSpriteBuffer
{
public:
	typedef struct _TEXTURE_SLOT
	{
		A3DTexture *	pA3DTexture;

		int				nVertsCount;
		AArray<A3DTLVERTEX, const A3DTLVERTEX&> aVertices;	// all vertices using this texture

		A3DBLEND		srcBlend;		// texture blend operation, this can only be the last sprite's setting
		A3DBLEND		destBlend;	
		
		bool			bFilterLinear;	// flag of filter mode, also, only last sprite's setting
		bool			bNeedStereo;	// flag of stereo processing

	} TEXTURE_SLOT, * PTEXTURE_SLOT;

protected:
	A3DDevice *				m_pA3DDevice;
	
	int						m_nTextureNum;
	TEXTURE_SLOT *			m_pTextureSlots;

	typedef abase::hash_map<A3DTexture *, TEXTURE_SLOT *> TEXTURESLOTMAP;
	TEXTURESLOTMAP			m_TextureSlotMap;

public:
	A2DSpriteBuffer();
	~A2DSpriteBuffer();

	bool Init(A3DDevice * pA3DDevice, A2DSpriteTextures * pSpriteTextures, int nElementNum);
	bool Release();

public:
	bool FlushBuffer();

	// Add a slice to the buffer;
	// v is the vertex buffer that consist of 4 TLVERTEX, face is 0-1-2, 1-3-2
	bool AddToBuffer(A3DTexture * pTexture, A3DTLVERTEX * v, A3DBLEND srcBlend, A3DBLEND destBlend, bool bFilterLinear, bool bNeedStereo);
};

typedef A2DSpriteBuffer * PA2DSpriteBuffer;
	
#endif //_A2DSPRITEBUFFER_H_

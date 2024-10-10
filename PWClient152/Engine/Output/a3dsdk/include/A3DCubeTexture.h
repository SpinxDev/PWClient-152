/*
 * FILE: A3DCubeTexture.h
 *
 * DESCRIPTION: Cube Texture
 *
 * CREATED BY: Hedi, 2005/11/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DCUBETEXTURE_H_
#define _A3DCUBETEXTURE_H_

#include "A3DTexture.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DDevice;
class A3DEngine;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DCubeTexture
//
///////////////////////////////////////////////////////////////////////////

class A3DCubeTexture : public A3DTexture
{
public:		//	Types

	enum A3DCUBEMAP_FACES 
	{
		A3DCUBEMAP_FACE_POSITIVE_X     = 0,
		A3DCUBEMAP_FACE_NEGATIVE_X     = 1,
		A3DCUBEMAP_FACE_POSITIVE_Y     = 2,
		A3DCUBEMAP_FACE_NEGATIVE_Y     = 3,
		A3DCUBEMAP_FACE_POSITIVE_Z     = 4,
		A3DCUBEMAP_FACE_NEGATIVE_Z     = 5,
	};

protected:
	IDirect3DCubeTexture9* m_pDXCubeTexture;

public:		//	Constructors and Destructors

	A3DCubeTexture();
	virtual ~A3DCubeTexture();

public:		//	Attributes

protected:
	virtual void CalcTextureDataSize();

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DDevice * pDevice);
	//	Release object
	virtual bool Release();
	virtual	bool Reload(bool bForceReload);

	//	Apply shader
	virtual bool Appear(int nLayer=0);
	//	Abolish shader
	virtual bool Disappear(int nLayer=0);
	
	bool Load(AFile* pFile);
	bool Load(const char* szFile);
    bool LoadWithFormat(AFile * pFile, A3DFORMAT fmt);

	bool Create(A3DDevice * pDevice, int nEdgeSize, A3DFORMAT format, int nMipLevel=1);
	
	bool LockRect(A3DCUBEMAP_FACES idFace, RECT* pRect, void** ppData, int* iPitch, DWORD dwFlags, int nMipLevel=0);
	bool UnlockRect(A3DCUBEMAP_FACES idFace);

	// Create an X8R8G8B8 normalization cube map with only 1 mip map
	bool CreateNormalizationCubeMap(A3DDevice * pDevice, int nEdgeSize, const A3DMATRIX4& matTM);

	virtual IDirect3DBaseTexture9* GetD3DBaseTexture() const;
	inline IDirect3DCubeTexture9 * GetD3DCubeTexture() { return m_pDXCubeTexture; }
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DCUBETEXTURE_H_



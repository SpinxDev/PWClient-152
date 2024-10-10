/*
 * FILE: A3DCubeTexture.h
 *
 * DESCRIPTION: Volume Texture
 *
 * CREATED BY: Hedi, 2005/11/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DVOLUMETEXTURE_H_
#define _A3DVOLUMETEXTURE_H_

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
class AFileImage;

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

class A3DVolumeTexture : public A3DTexture
{
public:		//	Types

protected:
	IDirect3DVolumeTexture9* m_pDXVolumeTexture;

public:		//	Constructors and Destructors

	A3DVolumeTexture();
	virtual ~A3DVolumeTexture();

public:		//	Attributes

protected:
	virtual void CalcTextureDataSize();
	virtual bool LoadTextureFromBuffer(BYTE* pDataBuf, int iDataLen, DWORD dwTexFlags=0);

	bool IsValidVolumeTexture(IDirect3DVolumeTexture9* pVolumeTex);

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DDevice * pDevice);
	virtual	bool Reload(bool bForceReload);
	
	//  Create a Volume Textuer
	virtual bool Create(A3DDevice * pDevice, int iWidth, int iHeight, int iDepth, A3DFORMAT Format, int nMipLevel = 1);
	virtual bool LockBox(D3DBOX* pBox, void** ppData, int* iSlicePitch, int* iRowPitch, DWORD dwFlags, int nMipLevel = 0);
	virtual bool UnlockBox(int nMipLevel = 0);
	virtual bool ExportToDDSFile(const char* szFileName);

	//	Release object
	virtual bool Release();

	bool Load(AFile* pFile);
	bool Load(const char* szFile);

	//	Apply shader
	virtual bool Appear(int nLayer=0);
	//	Abolish shader
	virtual bool Disappear(int nLayer=0);

	virtual IDirect3DBaseTexture9* GetD3DBaseTexture() const;
	inline IDirect3DVolumeTexture9 * GetD3DVolumeTexture() { return m_pDXVolumeTexture; }
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DVOLUMETEXTURE_H_



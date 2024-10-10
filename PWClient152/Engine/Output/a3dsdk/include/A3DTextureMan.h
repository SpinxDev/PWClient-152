/*
 * FILE: A3DTextureMan.h
 *
 * DESCRIPTION: Class that manage all textures in A3D Engine
 *
 * CREATED BY: Hedi, 2001/11/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTEXTUREMAN_H_
#define _A3DTEXTUREMAN_H_

#include "A3DTypes.h"
#include "AList2.h"
#include "AString.h"
#include "vector.h"

class A3DTexture;
class A3DDevice;
class A3DEffect;

class A3DTextureMan
{
public:		//	Types

	struct TEXTURERECORD
	{
		AString			strFilename;
		A3DTexture*		pA3DTexture;
		A3DFORMAT		fmtDesired;		//	Desired format
		int				nRefCount;
	};

private:

	A3DDevice*	m_pA3DDevice;
	int			m_iTexSizeCnt;		//	Total texture size counter in bytes

	CRITICAL_SECTION			m_cs;
	APtrList<TEXTURERECORD*>	m_ListTexture;
	abase::vector<A3DEffect*>	m_aEffects;
    A3DTexture*                 m_pTextureError;
	A3DTexture*					m_pLMTexError;
	A3DTexture*					m_pWhiteTex;		// white texture 
	A3DTexture*					m_pUnitNorm;		// unit normal texture used in angelica 2.2

protected:

	//	Search a texture with speicfied file name
	TEXTURERECORD* FindTexture(const char* szFilename, A3DFORMAT FmtDesired);
	//	Release all textures
	void ReleaseAllTextures();
    void LoadSystemTextures();

public:

	A3DTextureMan();
	virtual ~A3DTextureMan();

	bool Init(A3DDevice * pDevice);
	bool Release();
	bool Reset();
	bool ReloadAllTextures(bool bForceReload);
	bool ReloadEffectTex(bool bForceReload);
	bool UpdateEffectTexPtr();

	bool TickAnimation();

	// Use dwTextureFlags to specify special texture usage, eg. detail map or lightmap
	// if dwTextureFlags is 0, then it is a diffuse texture map only;
	// if dwTextureFlags contains A3DTF_MIPLEVEL, then nMipLevel will be used when
	// create mipmap chain;
	bool LoadTextureFromFile(const char* pszFilename, A3DTexture** ppA3DTexture, DWORD dwTextureFlags=0, int nMipLevel=0);
	bool LoadTextureFromFileInFolder(const char* pszFilename, const char* szFolder, A3DTexture ** ppA3DTexture, DWORD dwTextureFlags=0, int nMipLevel=0);
	//	Load texture file with specified size and format
	bool LoadTextureFromFile(const char* szFile, int iWidth, int iHeight, A3DFORMAT Fmt, int iMipLevel, A3DTexture** ppA3DTexture, PALETTEENTRY* aPalEntries=NULL);
	//	Use an existing texture object by increasing the reference count
	//	return NULL if can not duplicate it.
	A3DTexture * DuplicateTexture(A3DTexture* pA3DTexture);
	//	release a texture object
	bool ReleaseTexture(A3DTexture*& pA3DTexture);

	//	Check whether textures of specified format can be created or not
	bool IsFormatSupported(A3DFORMAT Fmt) const;

	bool PrecacheAllTexture();

	inline int GetTextureCount() const { return m_ListTexture.GetCount(); }

	//	Add texture data size
	inline int AddTextureDataSize(int iSize);
	inline int GetTextureDataSize() const;

	A3DTexture* GetErrorTexture() const { return m_pTextureError; }
	A3DTexture* GetWhiteTexture() const { return m_pWhiteTex; }
	A3DTexture* GetUnitNormTexture() const { return m_pUnitNorm; }
	A3DTexture* GetErrLMTexture() const { return m_pLMTexError; }

    void GetTextureRecords(TEXTURERECORD** pRecordArray);
};

//////////////////////////////////////////////////////////////////////////
int A3DTextureMan::AddTextureDataSize(int iSize) 
{ 
	return (m_iTexSizeCnt += iSize); 
}

int A3DTextureMan::GetTextureDataSize() const
{ 
	return m_iTexSizeCnt; 
}


typedef A3DTextureMan * PA3DTextureMan;

#endif

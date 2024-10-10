/*
 * FILE: A2DSpriteTexMan.h
 *
 * DESCRIPTION: Class that manage all textures for A2DSprite
 *
 * CREATED BY: Hedi, 2004/11/19
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A2DSPRITETEXMAN_H_
#define _A2DSPRITETEXMAN_H_

#include "vector.h"
#include "A3DTypes.h"
#include "AList2.h"
#include "AString.h"

class A2DSpriteTextures;
class A3DDevice;
class A3DViewport;
class A3DStream;
class A3DVertexShader;

using namespace abase;

class A2DSpriteTexMan
{
public:		//	Types
	struct TEXTURERECORD
	{
		AString					strFilename;
		A2DSpriteTextures*		pTextures;
		int						nRefCount;
	};

private:

	A3DDevice*	m_pA3DDevice;
	int			m_iTexSizeCnt;		//	Total size of texture

	APtrList<TEXTURERECORD*>	m_ListTexture;

	int							m_nMaxBlades;
	int							m_nMaxVerts;
	int							m_nVertsCount;
	A3DStream *					m_pStream;

	A3DStream *					m_pStream2Layer;

	A3DVertexShader *			m_pStereoShader; // vertex shader try to convert TLvertex back to cuboid space

	vector<A2DSpriteTextures*> m_CachedTextures;

protected:

	//	Search a texture with specified file name
	TEXTURERECORD* FindTexture(const char* szFilename);
	//	Release all textures
	void ReleaseAllTextures();

public:

	A2DSpriteTexMan();
	virtual ~A2DSpriteTexMan();

	bool Init(A3DDevice * pDevice, int nMaxBlades);
	bool Release();
	bool Reset();

	bool FlushInternalBuffer();

	bool Load2DSpriteTexturesFromFile(const char* pszFilename, int nWidth, int nHeight, A3DCOLOR colorKey,bool bDynamicTex, A2DSpriteTextures** ppTextures);
	bool Load2DSpriteTexturesFromFileInMemory(const char* pszFilename, const void * pMemFile, DWORD dwFileLength, int nWidth, int nHeight, A3DCOLOR colorKey, A2DSpriteTextures** ppTextures);
	bool Release2DSpriteTexture(A2DSpriteTextures*& pTextures);

	inline int GetTextureCount() { return m_ListTexture.GetCount(); }
	inline A3DStream * GetStream()	{ return m_pStream; }
	inline A3DStream * GetStream2Layer() { return m_pStream2Layer; }

	inline int GetMaxBlades() { return m_nMaxBlades; }

	bool LockStream(BYTE ** ppVerts, int nVertsToLock, int& nVertLockStart);
	bool UnlockStream();

	bool LockStream2Layer(BYTE ** ppVerts, int nVertsToLock);
	bool UnlockStream2Layer();

	bool RecordCachedTextures(A2DSpriteTextures * pTextures);

	inline A3DVertexShader * GetStereoShader() { return m_pStereoShader; }
};

#endif//_A2DSPRITETEXMAN_H_

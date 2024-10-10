/*
 * FILE: A2DSpriteTexMan.cpp
 *
 * DESCRIPTION: Class that manage all textures for A2DSprite
 *
 * CREATED BY: Hedi, 2004/11/19
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#include "A2DSpriteBuffer.h"
#include "A2DSpriteTexMan.h"
#include "A2DSpriteTextures.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DGDI.h"
#include "A3DStream.h"
#include "AFI.h"
#include "AMemory.h"
#include "A3DVertexShader.h"
#include "A3DShaderMan.h"

#define new A_DEBUG_NEW

A2DSpriteTexMan::A2DSpriteTexMan()
{
	m_pA3DDevice	= NULL;
	m_pStream		= NULL;
	m_pStream2Layer	= NULL;
	m_pStereoShader = NULL;
	m_iTexSizeCnt	= 0;
}

A2DSpriteTexMan::~A2DSpriteTexMan()
{
}

bool A2DSpriteTexMan::Init(A3DDevice * pDevice, int nMaxBlades)
{
	m_pA3DDevice = pDevice;
	m_nMaxBlades = nMaxBlades;
	m_nMaxVerts = nMaxBlades * 6;

	m_pStream = new A3DStream();
	if( !m_pStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, m_nMaxVerts, 0, A3DSTRM_REFERENCEPTR, 0) )
	{
		g_A3DErrLog.Log("A2DSpriteTexMan::Init(), failed to initialize the stream!");
		return false;
	}
	m_nVertsCount = 0;

	// stream with 2 layer only for special usage where we need the sprite to act with some extra texture,
	// for this kind of sprite, we will not offer drawtobuffer capability, so only one draw vertices, 6 vertices needed;
	m_pStream2Layer = new A3DStream();
	if( !m_pStream2Layer->Init(m_pA3DDevice, A3DVT_TLVERTEX2, 6, 0, A3DSTRM_REFERENCEPTR, 0) )
	{
		g_A3DErrLog.Log("A2DSpriteTexMan::Init(), failed to initialize the stream!");
		return false;
	}


	D3DVERTEXELEMENT9 vertDecl[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0},	// x, y, z, rhw in world
		{0, 16, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},		// diffuse.
		{0, 20, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1},		// specular.
		{0, 24, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},	// u, v coords.
		D3DDECL_END()
	};


	m_pStereoShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\2.2\\vs\\stereo_vs.txt", false, vertDecl);
	return true;
}

bool A2DSpriteTexMan::Release()
{
	ReleaseAllTextures();

	if( m_pStereoShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pStereoShader);
		m_pStereoShader = NULL;
	}

	if( m_pStream2Layer )
	{
		m_pStream2Layer->Release();
		delete m_pStream2Layer;
		m_pStream2Layer = NULL;
	}

	if( m_pStream )
	{
		m_pStream->Release();
		delete m_pStream;
		m_pStream = NULL;
	}

	return true;
}

//	Release all textures
void A2DSpriteTexMan::ReleaseAllTextures()
{
	ALISTPOSITION pos = m_ListTexture.GetHeadPosition();
	while (pos)
	{
		TEXTURERECORD* pTextureRecord = m_ListTexture.GetNext(pos);
		A2DSpriteTextures* pTextures = pTextureRecord->pTextures;

		if (pTextures)
		{
			pTextures->Release();
			delete pTextures;
			pTextureRecord->pTextures = NULL;
		}
	}

	m_ListTexture.RemoveAll();
}

A2DSpriteTexMan::TEXTURERECORD* A2DSpriteTexMan::FindTexture(const char* szFilename)
{
	ALISTPOSITION pos = m_ListTexture.GetHeadPosition();
	while (pos)
	{
		TEXTURERECORD* pTextureRecord = m_ListTexture.GetNext(pos);

		if (!_stricmp(szFilename, pTextureRecord->strFilename))
			return pTextureRecord;
	}

	return NULL;
}

bool A2DSpriteTexMan::Load2DSpriteTexturesFromFile(const char* pszFilename, int nWidth, int nHeight, A3DCOLOR colorKey,bool bDynamicTex, A2DSpriteTextures** ppTextures)
{
	//	We don't need to load an empty texture;
	if (!pszFilename || !pszFilename[0])
		return true;

	TEXTURERECORD* pTextureRecord = FindTexture(pszFilename);
	if (pTextureRecord)
	{
		pTextureRecord->nRefCount++;
		pTextureRecord->pTextures->IncUserRef();

		if (pTextureRecord->pTextures->GetSpriteBuffer() == NULL)
			pTextureRecord->pTextures->Init(m_pA3DDevice, pszFilename, nWidth, nHeight, colorKey);

		*ppTextures = pTextureRecord->pTextures;
		return true;
	}

	*ppTextures = NULL;
	
	A2DSpriteTextures* pNewTextures;
	if (!(pNewTextures = new A2DSpriteTextures))
	{
		g_A3DErrLog.Log("A2DSpriteTexMan::Load2DSpriteTexturesFromFile(), Not enough memory!");
		return false;
	}

	pNewTextures->SetDynamicTex(bDynamicTex);

	if (!pNewTextures->Init(m_pA3DDevice, pszFilename, nWidth, nHeight, colorKey))
	{
		pNewTextures->Release();
		delete pNewTextures;

		g_A3DErrLog.Log("A2DSpriteTexMan::Load2DSpriteTexturesFromFile(), Create new A2DSpriteTextures fail!");
		return false;
	}
	
	if (!(pTextureRecord = new TEXTURERECORD))
	{
		g_A3DErrLog.Log("A2DSpriteTexMan::Load2DSpriteTexturesFromFile Not enough Memory!");
		return false;
	}

	pTextureRecord->strFilename = pszFilename;
	pTextureRecord->pTextures = pNewTextures;
	pTextureRecord->nRefCount	= 1;
	pNewTextures->IncUserRef();
	
	pNewTextures->m_dwPosInMan = (DWORD)m_ListTexture.AddTail(pTextureRecord);
	*ppTextures = pNewTextures;

	return true;
}

bool A2DSpriteTexMan::Load2DSpriteTexturesFromFileInMemory(const char* pszFilename, const void * pMemFile, DWORD dwFileLength, int nWidth, int nHeight, A3DCOLOR colorKey, A2DSpriteTextures** ppTextures)
{
	//	We don't need to load an empty texture;
	if (!pszFilename || !pszFilename[0])
		return true;

	TEXTURERECORD* pTextureRecord = FindTexture(pszFilename);
	if (pTextureRecord)
	{
		pTextureRecord->nRefCount++;
		pTextureRecord->pTextures->IncUserRef();

		if (pTextureRecord->pTextures->GetSpriteBuffer() == NULL)
			pTextureRecord->pTextures->InitFromFileInMemory(m_pA3DDevice, pszFilename, pMemFile, dwFileLength, nWidth, nHeight, colorKey);

		*ppTextures = pTextureRecord->pTextures;
		return true;
	}

	*ppTextures = NULL;
	
	A2DSpriteTextures* pNewTextures;
	if (!(pNewTextures = new A2DSpriteTextures))
	{
		g_A3DErrLog.Log("A2DSpriteTexMan::Load2DSpriteTexturesFromFileInMemory(), Not enough memory!");
		return false;
	}

	if (!pNewTextures->InitFromFileInMemory(m_pA3DDevice, pszFilename, pMemFile, dwFileLength, nWidth, nHeight, colorKey))
	{
		g_A3DErrLog.Log("A2DSpriteTexMan::Load2DSpriteTexturesFromFileInMemory(), Create new A2DSpriteTextures fail!");
		return false;
	}
	
	if (!(pTextureRecord = new TEXTURERECORD))
	{
		g_A3DErrLog.Log("A2DSpriteTexMan::Load2DSpriteTexturesFromFileInMemory Not enough Memory!");
		return false;
	}

	pTextureRecord->strFilename = pszFilename;
	pTextureRecord->pTextures = pNewTextures;
	pTextureRecord->nRefCount	= 1;
	pNewTextures->IncUserRef();
	
	pNewTextures->m_dwPosInMan = (DWORD)m_ListTexture.AddTail(pTextureRecord);
	*ppTextures = pNewTextures;

	return true;
}

bool A2DSpriteTexMan::Release2DSpriteTexture(A2DSpriteTextures*& pTextures)
{
	if (!pTextures )
		return true;

	if (!pTextures->m_dwPosInMan)
	{
		// not managed by me, so just release it
		pTextures->Release();
		delete pTextures;
		pTextures = NULL;
		return true;
	}

	ALISTPOSITION pos = (ALISTPOSITION)pTextures->m_dwPosInMan;

	TEXTURERECORD* pTextureRecord = m_ListTexture.GetAt(pos);
	ASSERT(pTextureRecord && pTextureRecord->pTextures == pTextures);

	pTextureRecord->nRefCount--;
	pTextureRecord->pTextures->DecUserRef();
	if (!pTextureRecord->nRefCount)
	{
		pTextures->Release();
		delete pTextures;

		delete pTextureRecord;
		m_ListTexture.RemoveAt(pos);
	}

	pTextures = NULL;

	return true;
}

bool A2DSpriteTexMan::Reset()
{
	ReleaseAllTextures();
	return true;
}

bool A2DSpriteTexMan::FlushInternalBuffer()
{
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	int nNumCache = m_CachedTextures.size();
	for(int i=0; i<nNumCache; i++)
	{
		A2DSpriteTextures * pTextures = m_CachedTextures[i];
		
		if (pTextures)
		{
			if( pTextures->GetSpriteBuffer() )
				pTextures->GetSpriteBuffer()->FlushBuffer();
			pTextures->m_bHasBeenCached = false;
		}
	}

	m_CachedTextures.clear();
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	return true;
}

bool A2DSpriteTexMan::LockStream(BYTE ** ppVerts, int nVertsToLock, int& nVertLockStart)
{
	DWORD dwVertsLockFlag = 0;
	
	if( m_nVertsCount + nVertsToLock > m_nMaxVerts )
	{
		// not enough free vertex buffer, so lock entire buffer with discard flag here
		m_nVertsCount = 0;
	}
	if( m_nVertsCount > 0 )
		dwVertsLockFlag = D3DLOCK_NOOVERWRITE;
	
	nVertLockStart = m_nVertsCount;
	if( !m_pStream->LockVertexBufferDynamic(nVertLockStart * sizeof(A3DTLVERTEX), nVertsToLock * sizeof(A3DTLVERTEX), ppVerts, dwVertsLockFlag) )
		return false;

	m_nVertsCount += nVertsToLock;
	return true;
}

bool A2DSpriteTexMan::UnlockStream()
{
	if( !m_pStream->UnlockVertexBufferDynamic() )
		return false;

	return true;
}

bool A2DSpriteTexMan::RecordCachedTextures(A2DSpriteTextures * pTextures)
{
	if( pTextures && !pTextures->m_bHasBeenCached )
	{
		pTextures->m_bHasBeenCached = true;
		m_CachedTextures.push_back(pTextures);
	}

	return true;
}

bool A2DSpriteTexMan::LockStream2Layer(BYTE ** ppVerts, int nVertsToLock)
{
	DWORD dwVertsLockFlag = 0;
	
	if( nVertsToLock > 6 )
	{
		// not enough free vertex buffer
		return false;
	}

	if( !m_pStream2Layer->LockVertexBufferDynamic(0, nVertsToLock * sizeof(A3DTLVERTEX), ppVerts, dwVertsLockFlag) )
		return false;
	
	return true;
}

bool A2DSpriteTexMan::UnlockStream2Layer()
{
	if( !m_pStream2Layer->UnlockVertexBufferDynamic() )
		return false;

	return true;
}
/*
 * FILE: HomeTrnMask.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/6/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#include "HomeTrnMask.h"
#include "HomeTerrain.h"
#include "AutoScene.h"
#include "AutoSceneConfig.h"
#include "Bitmap.h"

#include <A3DTexture.h>
#include <A3DTextureMan.h>
#include <A3DTerrain2Env.h>
#include <A3DMacros.h>
#include <A3DEngine.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CHomeTrnMaskLayer
//	
///////////////////////////////////////////////////////////////////////////

CHomeTrnMaskLayer::CHomeTrnMaskLayer(CHomeTrnMask* pMaskArea)
{
	m_pMaskArea	= NULL;
	m_pTerrain	= (CHomeTerrain*)pMaskArea->GetTerrain();
	m_pTexture	= NULL;
	m_pMaskBmp	= NULL;
	m_iLayerIdx	= -1;
	m_bDefault	= false;
}

CHomeTrnMaskLayer::~CHomeTrnMaskLayer()
{
}

//	Initalize object
bool CHomeTrnMaskLayer::Init(int iLayerIdx)
{
	m_iLayerIdx	= iLayerIdx;
	m_bDefault	= iLayerIdx ? false : true;

	//	Load layer texture
	CAutoSceneConfig* pCfg = m_pTerrain->GetAutoScene()->GetAutoSceneConfig();
	AUTOTEXTURETYPELIST TexType;
	int iTexIdx = pCfg->GetAutoTextureTypeListByID(pCfg->GetTextureID(iLayerIdx), &TexType);
	if ((-1 == iTexIdx) || !ChangeTexture(TexType.strTexture))
	{
		a_LogOutput(1, "CHomeTrnMaskLayer::Init, Failed to load texture.");
		return false;
	}

	//	Create layer mask bitmap
	if (!(m_pMaskBmp = new CELBitmap))
		return false;

	int iSize = CHomeTrnMask::MASKMAP_SIZE;
	if (!m_pMaskBmp->CreateBitmap(iSize, iSize, 8, NULL))
	{
		a_LogOutput(1, "CHomeTrnMaskLayer::Init, Failed to create mask bitmap.");
		return false;
	}

	if (m_bDefault)
	{
		BYTE* pData = m_pMaskBmp->GetBitmapData();
		memset(pData, 0xff, m_pMaskBmp->GetPitch() * m_pMaskBmp->GetHeight());
	}
	else
		m_pMaskBmp->Clear();

	return true;
}

//	Release object
void CHomeTrnMaskLayer::Release()
{
	if (m_pTexture)
	{
		A3DTextureMan* pTexMan = m_pTerrain->GetA3DEngine()->GetA3DTextureMan();
		pTexMan->ReleaseTexture(m_pTexture);
		m_pTexture = NULL;
	}

	if (m_pMaskBmp)
	{
		m_pMaskBmp->Release();
		delete m_pMaskBmp;
		m_pMaskBmp = NULL;
	}
}

//	Check whether mask map is pure black
bool CHomeTrnMaskLayer::IsMaskPureBlack()
{
	if (m_bDefault)
		return false;

	if (!m_pMaskBmp)
		return true;

	int iHeight = m_pMaskBmp->GetHeight();
	BYTE* pDataLine = m_pMaskBmp->GetBitmapData();

	ASSERT(!(m_pMaskBmp->GetWidth() % 4));
	int iStep = m_pMaskBmp->GetWidth() / 4;

	for (int i=0; i < iHeight; i++)
	{
		DWORD* p = (DWORD*)pDataLine;
		pDataLine += m_pMaskBmp->GetPitch();

		for (int j=0; j < iStep; j++, p++)
		{
			if (*p)
				return false;
		}
	}

	return true;
}

//	Check whether mask map is pure white
bool CHomeTrnMaskLayer::IsMaskPureWhite()
{
	if (m_bDefault)
		return true;

	if (!m_pMaskBmp)
		return false;

	int iHeight = m_pMaskBmp->GetHeight();
	BYTE* pDataLine = m_pMaskBmp->GetBitmapData();

	ASSERT(!(m_pMaskBmp->GetWidth() % 4));
	int iStep = m_pMaskBmp->GetWidth() / 4;

	for (int i=0; i < iHeight; i++)
	{
		DWORD* p = (DWORD*)pDataLine;
		pDataLine += m_pMaskBmp->GetPitch();

		for (int j=0; j < iStep; j++, p++)
		{
			if (*p != 0xff)
				return false;
		}
	}

	return true;
}

//	Change layer texture
bool CHomeTrnMaskLayer::ChangeTexture(const char* szFile)
{
	A3DTextureMan* pTexMan = m_pTerrain->GetA3DEngine()->GetA3DTextureMan();
	A3DTexture* pNewTex = NULL;

	if (!pTexMan->LoadTextureFromFile(szFile, &pNewTex))
	{
		a_LogOutput(1, "CHomeTrnMaskLayer::ChangeTexture, Failed to load texture %s", szFile);
		return false;
	}

	if (m_pTexture)
	{
		pTexMan->ReleaseTexture(m_pTexture);
		m_pTexture = NULL;
	}

	m_pTexture = pNewTex;

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CHomeTrnMask
//	
///////////////////////////////////////////////////////////////////////////

CHomeTrnMask::CHomeTrnMask(A3DTerrain2* pTerrain, int iMaskArea) :
A3DTerrain2Mask(pTerrain, iMaskArea)
{
}

CHomeTrnMask::~CHomeTrnMask()
{
}

//	Initialize object
bool CHomeTrnMask::Init(int iNumLayer)
{
	if (!CreateMaskLayers(iNumLayer))
	{
		a_LogOutput(1, "CHomeTrnMask::Init, Failed to create mask layers.");
		return false;
	}

	return true;
}

//	Release object
void CHomeTrnMask::Release()
{
	int i;

	//	Clear texture array so that A3DTerrain2Mask::Release will
	//	not release them in it own way.
	for (i=0; i < m_aTextures.GetSize(); i++)
		m_aTextures[i] = NULL;

	ReleaseMaskLayers();

	A3DTerrain2Mask::Release();
}

//	Create mask layers
bool CHomeTrnMask::CreateMaskLayers(int iNumLayer)
{
	for (int i=0; i < iNumLayer; i++)
	{
		CHomeTrnMaskLayer* pLayer = new CHomeTrnMaskLayer(this);
		if (!pLayer)
			return false;

		if (!pLayer->Init(i))
		{
			a_LogOutput(1, "CHomeTrnMask::CreateMaskLayers, Failed to init mask layer %d.", i);
			return false;
		}
		
		m_aAllLayers.Add(pLayer);
	}

	return true;
}

//	Release mask layers
void CHomeTrnMask::ReleaseMaskLayers()
{
	for (int i=0; i < m_aAllLayers.GetSize(); i++)
	{
		CHomeTrnMaskLayer* pLayer = m_aAllLayers[i];
		pLayer->Release();
		delete pLayer;
	}

	m_aAllLayers.RemoveAll();
	m_aAppLayers.RemoveAll();
	m_aCandLayers.RemoveAll();
}

//	Create layer mask texture
A3DTexture* CHomeTrnMask::CreateMaskTexture(A3DFORMAT fmt)
{
	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return NULL;

	//	Create mask texture
	if (!pTexture->Create(m_pTerrain->GetA3DDevice(), MASKMAP_SIZE, MASKMAP_SIZE, fmt, 1))
	{
		delete pTexture;
		a_LogOutput(1, "CAutoTerrain::CreateMaskTexture, Failed to create mask texture.");
		return NULL;
	}

	return pTexture;
}

//	Release mask textures
void CHomeTrnMask::ReleaseMaskTextures()
{
	//	Release mask texture
	for (int i=0; i < m_aMaskTextures.GetSize(); i++)
	{
		A3DTexture* pTexture = m_aMaskTextures[i];
		if (pTexture)
		{
			pTexture->Release();
			delete pTexture;
		}
	}

	m_aMaskTextures.RemoveAll();
}

//	Get specified layer bitmap
CELBitmap* CHomeTrnMask::GetLayerBitmap(int n)
{
	CHomeTrnMaskLayer* pLayer = GetLayer(n);
	return pLayer ? pLayer->GetMaskBitmap() : NULL;
}

//	Apply specified layer's mask map
bool CHomeTrnMask::ApplyLayerBitmap(int* aLayers, int iNumLayer)
{
	if (!aLayers || !iNumLayer)
		return false;

	//	Only select layers which aren't pure black
	m_aCandLayers.RemoveAll(false);

	int i, iAllLayerNum = m_aAllLayers.GetSize();

	//	Search the last pure white mask
	for (i=iAllLayerNum-1; i >= 0; i--)
	{
		CHomeTrnMaskLayer* pLayer = m_aAllLayers[i];
		if (pLayer->IsMaskPureWhite())
		{
			m_aCandLayers.Add(pLayer);
			break;
		}
	}

	i++;

	//	Then add all non-black mask
	for (; i < iAllLayerNum; i++)
	{
		CHomeTrnMaskLayer* pLayer = m_aAllLayers[i];
		if (!pLayer->IsMaskPureBlack())
			m_aCandLayers.Add(pLayer);
	}

	if (A3D::g_pA3DTerrain2Env->GetSupportPSFlag())	//	Use PS
		return ApplyLayerMaskBmpPS();
	else
		return ApplyLayerMaskBmpNoPS(aLayers, iNumLayer);
}

//	Apply layer mask data to texture
bool CHomeTrnMask::ApplyLayerMaskBmpPS()
{
	int i, j, iNumLayer = m_aCandLayers.GetSize();

	//	Rebuild layer info
	m_aAppLayers.RemoveAll(false);
	m_aLayers.RemoveAll(false);
	m_aTextures.RemoveAll(false);

	for (i=0; i < iNumLayer; i++)
	{
		CHomeTrnMaskLayer* pCand = m_aCandLayers[i];
		m_aAppLayers.Add(pCand);

		//	Texture
		m_aTextures.Add(pCand->GetTexture());

		//	Basic layer info
		LAYER lay;
		FillBaseLayerInfo(pCand->GetLayerIndex(), lay);
		m_aLayers.Add(lay);
	}

	//	Build mask texture in PS version
	BYTE* aSrcData[3] = {NULL, NULL, NULL};
	int iTexCnt = 0;
	CHomeTerrain* pTrn = (CHomeTerrain*)m_pTerrain;

	for (i=0; i < iNumLayer; i+=3)
	{
		for (j=0; j < 3; j++)
		{
			int iIndex = i + j;
			if (!iIndex)	//	Default layer ?
				aSrcData[j] = pTrn->GetPureWhiteBmp()->GetBitmapData();
			else if (iIndex < iNumLayer)
				aSrcData[j] = m_aCandLayers[iIndex]->GetMaskBitmap()->GetBitmapData();
			else
				aSrcData[j] = pTrn->GetPureBlackBmp()->GetBitmapData();
		}

		//	Try to get exist texture
		A3DTexture* pTexture = NULL;
		if (iTexCnt < m_aMaskTextures.GetSize())
			pTexture = m_aMaskTextures[iTexCnt];
		else	//	Create new 32-bit texture
		{
			if (!(pTexture = CreateMaskTexture(A3DFMT_A8R8G8B8)))
				return false;

			//	Store this texture
			m_aMaskTextures.Add(pTexture);
		}

		iTexCnt++;

		//	Lock texture
		BYTE* pDstData;
		int iDstPitch;
		if (!pTexture->LockRect(NULL, (void**) &pDstData, &iDstPitch, 0))
		{
			a_LogOutput(1, "CHomeTrnMask::ApplyLayerMaskBmpPS, Failed to lock texture");
			return false;
		}

		//	Fill texture data
		int iSrcPitch = pTrn->GetPureWhiteBmp()->GetPitch();
		int iSrcOffset = 0;
		BYTE* pDstLine = pDstData;

		for (int r=0; r < MASKMAP_SIZE; r++)
		{
			BYTE* pDst = pDstLine;
			pDstLine += iDstPitch;

			BYTE* pSrc0 = aSrcData[0] + iSrcOffset;
			BYTE* pSrc1 = aSrcData[1] + iSrcOffset;
			BYTE* pSrc2 = aSrcData[2] + iSrcOffset;
			iSrcOffset += iSrcPitch;

			for (int c=0; c < MASKMAP_SIZE; c++)
			{
				*pDst++ = *pSrc0++;
				*pDst++ = *pSrc1++;
				*pDst++ = *pSrc2++;
				*pDst++ = 0xff;
			}
		}

		pTexture->UnlockRect();
	}

	return true;
}

bool CHomeTrnMask::ApplyLayerMaskBmpNoPS(int* aLayers, int iNumLayer)
{
	CHomeTerrain* pTrn = (CHomeTerrain*)m_pTerrain;
	int i;

	//	Save existing textures
	APtrArray<A3DTexture*> aOldTexs(m_aAllLayers.GetSize(), 16);
	for (i=0; i < m_aAllLayers.GetSize(); i++)
	{
		int iAppIndex = m_aAppLayers.Find(m_aAllLayers[i]);
		if (iAppIndex >= 0)
			aOldTexs[i] = m_aMaskTextures[iAppIndex];
		else
			aOldTexs[i] = NULL;
	}

	m_aMaskTextures.RemoveAll(false);
	m_aLayers.RemoveAll(false);
	m_aAppLayers.RemoveAll(false);
	m_aTextures.RemoveAll(false);

	//	Update pair
	APtrArray<CELBitmap*> aUpdSrc;
	APtrArray<A3DTexture*> aUpdDst;

	for (i=0; i < m_aCandLayers.GetSize(); i++)
	{
		CHomeTrnMaskLayer* pCand = m_aCandLayers[i];
		int iIdxInTotal = pCand->GetLayerIndex();
		A3DTexture* pTexture = aOldTexs[iIdxInTotal];
		bool bNewTex = false;

		m_aAppLayers.Add(pCand);

		if (!pTexture)
		{
			bNewTex = true;

			//	Create new texture
			A3DFORMAT fmt = A3D::g_pA3DTerrain2Env->GetMaskMapFormat();
			if (!(pTexture = CreateMaskTexture(fmt)))
				return false;

			m_aMaskTextures.Add(pTexture);
		}
		else	//	Use old texture
		{
			m_aMaskTextures.Add(pTexture);
			aOldTexs[iIdxInTotal] = NULL;
		}

		int j(0);
		for (j=0; j < iNumLayer; j++)
		{
			if (aLayers[j] == iIdxInTotal)
				break;
		}

		if (bNewTex || j < iNumLayer)
		{
			//	This layer needs updating
			aUpdDst.Add(pTexture);

			if (pCand->IsDefault())
				aUpdSrc.Add(pTrn->GetPureWhiteBmp());
			else
				aUpdSrc.Add(pCand->GetMaskBitmap());
		}

		//	Texture
		m_aTextures.Add(pCand->GetTexture());

		//	Basic layer info
		LAYER lay;
		FillBaseLayerInfo(pCand->GetLayerIndex(), lay);
		m_aLayers.Add(lay);
	}

	//	Clear no-use old textures
	for (i=0; i < aOldTexs.GetSize(); i++)
	{
		A3DTexture* pTexture = aOldTexs[i];
		if (pTexture)
		{
			pTexture->Release();
			delete pTexture;
		}
	}

	//	Update textures
	for (i=0; i < aUpdDst.GetSize(); i++)
		ApplyLayerMaskBmpNoPS(aUpdDst[i], aUpdSrc[i]);

	return true;
}

bool CHomeTrnMask::ApplyLayerMaskBmpNoPS(A3DTexture* pDstTex, CELBitmap* pSrcBmp)
{
	//	Lock texture
	BYTE* pDstData;
	int iDstPitch;

	if (!pDstTex->LockRect(NULL, (void**) &pDstData, &iDstPitch, 0))
	{
		a_LogOutput(1, "CAutoTerrain::ApplyLayerMaskBmpNoPS, Failed to lock texture");
		return false;
	}

	int iSrcPitch = pSrcBmp->GetPitch();
	BYTE* pSrcLine = pSrcBmp->GetBitmapData();
	BYTE* pDstLine = pDstData;
	A3DFORMAT fmt = A3D::g_pA3DTerrain2Env->GetMaskMapFormat();

	int iWidth = MASKMAP_SIZE;
	int iHeight = MASKMAP_SIZE;

	switch (fmt)
	{
	case A3DFMT_A8:
	case A3DFMT_L8:			//	8-bit
	{
		for (int i=0; i < iHeight; i++)
		{
			memcpy(pDstLine, pSrcLine, iWidth);
			pDstLine += iDstPitch;
			pSrcLine += iSrcPitch;
		}

		break;
	}
	case A3DFMT_A4L4:		//	8-bit
	{
		float fScale = 15.0f / 255.0f;
		for (int i=0; i < iHeight; i++)
		{
			BYTE* pDst = pDstLine;
			pDstLine += iDstPitch;

			BYTE* pSrc = pSrcLine;
			pSrcLine += iSrcPitch;

			for (int j=0; j < iWidth; j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = (BYTE)(c * fScale);
			}
		}

		break;
	}
	case A3DFMT_A8L8:
	case A3DFMT_A8P8:
	case A3DFMT_A8R3G3B2:
	{
		for (int i=0; i < iHeight; i++)
		{
			WORD* pDst = (WORD*)pDstLine;
			pDstLine += iDstPitch;

			BYTE* pSrc = pSrcLine;
			pSrcLine += iSrcPitch;

			for (int j=0; j < iWidth; j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = (((WORD)c) << 8) | c;
			}
		}

		break;
	}
	case A3DFMT_R5G6B5:
	case A3DFMT_A1R5G5B5:
	case A3DFMT_X1R5G5B5:
	{
		float fScale = 31.0f / 255.0f;
		for (int i=0; i < iHeight; i++)
		{
			WORD* pDst = (WORD*)pDstLine;
			pDstLine += iDstPitch;

			BYTE* pSrc = pSrcLine;
			pSrcLine += iSrcPitch;

			for (int j=0; j < iWidth; j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = (WORD)(c * fScale);
			}
		}

		break;
	}
	case A3DFMT_A4R4G4B4:
	case A3DFMT_X4R4G4B4:	//	16-bit
	{
		float fScale = 15.0f / 255.0f;
		for (int i=0; i < iHeight; i++)
		{
			WORD* pDst = (WORD*)pDstLine;
			pDstLine += iDstPitch;

			BYTE* pSrc = pSrcLine;
			pSrcLine += iSrcPitch;

			for (int j=0; j < iWidth; j++)
			{
				WORD c = *pSrc++;
				c = (WORD)(c * fScale);
				*pDst++ = (c << 12) | (c << 8) | (c << 4) | c;
			}
		}

		break;
	}
	case A3DFMT_R8G8B8:		//	24-bit
	{
		for (int i=0; i < iHeight; i++)
		{
			BYTE* pDst = pDstLine;
			pDstLine += iDstPitch;

			BYTE* pSrc = pSrcLine;
			pSrcLine += iSrcPitch;

			for (int j=0; j < iWidth; j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = c;
				*pDst++ = c;
				*pDst++ = c;
			}
		}

		break;
	}
	case A3DFMT_A8R8G8B8:
	case A3DFMT_X8R8G8B8:	//	32-bit
	{
		for (int i=0; i < iHeight; i++)
		{
			DWORD* pDst = (DWORD*)pDstLine;
			pDstLine += iDstPitch;

			BYTE* pSrc = pSrcLine;
			pSrcLine += iSrcPitch;

			for (int j=0; j < iWidth; j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = A3DCOLORRGBA(c, c, c, c);
			}
		}

		break;
	}
	default:
		ASSERT(0);
		break;
	}

	pDstTex->UnlockRect();

	return true;
}

//	Fill a A3DTerrain2Mask::LAYER structure
void CHomeTrnMask::FillBaseLayerInfo(int iLayerIdx, LAYER& layer)
{
	layer.iMaskMapSize	= MASKMAP_SIZE;
	layer.iTextureIdx	= -1;
	layer.iSpecMapIdx	= -1;
	layer.byProjAxis	= 1;
	layer.byWeight		= 0;
	layer.fUScale		= 0.1f;
	layer.fVScale		= 0.1f;
}

//	Change layer texture
bool CHomeTrnMask::ChangeLayerTexture(int iLayer, const char* szFile)
{
	CHomeTrnMaskLayer* pLayer = GetLayer(iLayer);
	if (!pLayer || !pLayer->ChangeTexture(szFile))
		return false;

	//	If this layer has been applied, apply new texture
	for (int i=0; i < m_aAppLayers.GetSize(); i++)
	{
		CHomeTrnMaskLayer* pLayer = m_aAppLayers[i];
		if (pLayer->GetLayerIndex() == iLayer)
		{
			m_aTextures[i] = pLayer->GetTexture();
			break;
		}
	}

	return true;
}


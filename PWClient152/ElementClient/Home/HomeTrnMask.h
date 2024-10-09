/*
 * FILE: HomeTrnMask.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/6/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#pragma	once

#include <AArray.h>
#include <A3DTerrain2Blk.h>

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

class CELBitmap;
class CHomeTerrain;
class CHomeTrnMask;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CHomeTrnBlock
//	
///////////////////////////////////////////////////////////////////////////

class CHomeTrnBlock : public A3DTerrain2Block
{
public:		//	Types

	friend class CHomeTerrainLoader;
	friend class CHomeTerrain;

public:		//	Constructor and Destructor

	CHomeTrnBlock(A3DTerrain2* pTerrain) : A3DTerrain2Block(pTerrain) {}

public:		//	Attributes

public:		//	Operations

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CHomeTrnMaskLayer
//	
///////////////////////////////////////////////////////////////////////////

class CHomeTrnMaskLayer
{
public:		//	Types

	friend class CHomeTrnMask;

public:		//	Constructor and Destructor

	CHomeTrnMaskLayer(CHomeTrnMask* pMaskArea);
	virtual ~CHomeTrnMaskLayer();

public:		//	Attributes

public:		//	Operations

	//	Initalize object
	bool Init(int iLayerIdx);
	//	Release object
	void Release();

	//	Get layer index
	int GetLayerIndex() { return m_iLayerIdx; }
	//	Is default layer ?
	bool IsDefault() { return m_bDefault; }

	//	Check whether mask map is pure black
	bool IsMaskPureBlack();
	//	Check whether mask map is pure white
	bool IsMaskPureWhite();

	//	Change layer texture
	bool ChangeTexture(const char* szFile);

	//	Get layer texture
	A3DTexture* GetTexture() { return m_pTexture; }
	//	Get mask bitmap
	CELBitmap* GetMaskBitmap() { return m_pMaskBmp; }

protected:	//	Attributes

	CHomeTerrain*	m_pTerrain;		//	Terrain object
	CHomeTrnMask*	m_pMaskArea;	//	Mask area object
	A3DTexture*		m_pTexture;		//	Layer texture
	CELBitmap*		m_pMaskBmp;		//	Mask bitmap

	bool	m_bDefault;		//	Default layer flag
	int		m_iLayerIdx;	//	Layer index

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CHomeTrnMask
//	
///////////////////////////////////////////////////////////////////////////

class CHomeTrnMask : public A3DTerrain2Mask
{
public:		//	Types

	enum
	{
		MASKMAP_SIZE	= 128
	};

	friend class CHomeTerrainLoader;

public:		//	Constructor and Destructor

	CHomeTrnMask(A3DTerrain2* pTerrain, int iMaskArea);
	virtual ~CHomeTrnMask();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(int iNumLayer);
	//	Release object
	virtual void Release();

	//	Change layer texture
	bool ChangeLayerTexture(int iLayer, const char* szFile);

	//	Apply specified layer's mask map
	bool ApplyLayerBitmap(int* aLayers, int iNumLayer);
	//	Get specified layer object
	CHomeTrnMaskLayer* GetLayer(int n) { return m_aAllLayers[n]; }
	//	Get specified layer bitmap
	CELBitmap* GetLayerBitmap(int n);

protected:	//	Attributes

	APtrArray<CHomeTrnMaskLayer*>	m_aAllLayers;	//	All mask layers
	APtrArray<CHomeTrnMaskLayer*>	m_aCandLayers;	//	Candidated mask layers which will be applied
	APtrArray<CHomeTrnMaskLayer*>	m_aAppLayers;	//	Applied layers

protected:	//	Operations

	//	Create mask layers
	bool CreateMaskLayers(int iNumLayer);
	//	Release mask layers
	void ReleaseMaskLayers();
	//	Create layer mask texture
	A3DTexture* CreateMaskTexture(A3DFORMAT fmt);
	//	Release mask textures
	void ReleaseMaskTextures();

	//	Fill a A3DTerrain2Mask::LAYER structure
	void FillBaseLayerInfo(int iLayerIdx, LAYER& layer);

	//	Apply layer mask data to texture
	bool ApplyLayerMaskBmpPS();
	bool ApplyLayerMaskBmpNoPS(int* aLayers, int iNumLayer);
	bool ApplyLayerMaskBmpNoPS(A3DTexture* pDstTex, CELBitmap* pSrcBmp);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


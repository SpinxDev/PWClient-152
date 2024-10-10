/*
 * FILE: HomeTerrain.h
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

#include <A3DTerrain2.h>
#include <A3DTerrain2Loader.h>
#include <AList2.h>

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

class CAutoScene;
class CELBitmap;
class CHomeTrnMask;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CHomeTerrainLoader
//	
///////////////////////////////////////////////////////////////////////////

class CHomeTerrainLoader : public A3DTerrain2Loader
{
public:		//	Types

public:		//	Constructor and Destructor

	CHomeTerrainLoader(A3DTerrain2* pTerrain, int iLoaderID);
	virtual ~CHomeTerrainLoader();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(const char* szTrn2File);
	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime) { return true; }

	//	Load block
	virtual A3DTerrain2Block* LoadBlock(int r, int c, int iBlock);
	//	Unload a block
	virtual void UnloadBlock(A3DTerrain2Block* pBlock);
	//	Load a mask area
	virtual A3DTerrain2Mask* LoadMask(int iMaskArea);
	//	Unload a mask area
	virtual void UnloadMask(A3DTerrain2Mask* pMaskArea);

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CHomeTerrain
//	
///////////////////////////////////////////////////////////////////////////

class CHomeTerrain : public A3DTerrain2
{
public:		//	Types

	struct BLOCKPOS
	{
		int r;
		int c;
	};

	friend class CHomeTerrainLoader;

public:		//	Constructor and Destructor

	CHomeTerrain(CAutoScene* pScene);
	virtual ~CHomeTerrain();

public:		//	Attributes

public:		//	Operations

	//	Initialize terrain object
	bool Init(float fViewRadius);
	//	Release terrain object
	virtual void Release();

	//	Update routine
	virtual bool Update(DWORD dwDeltaTime, A3DCameraBase* pCamera, const A3DVECTOR3& vLoadCenter);

	//	Get maximum layer number
	int GetMaxLayerNum() const { return m_iMaxLayerNum; }
	//	Ge pure black bitmap object
	CELBitmap* GetPureBlackBmp() { return m_pBlackBmp; }
	//	Ge pure white bitmap object
	CELBitmap* GetPureWhiteBmp() { return m_pWhiteBmp; }

	//	Update specified mask area height and normal
	bool UpdateHomeAreaHeight(int r, int c, const float* aHeiData, const A3DVECTOR3* aNormals, int iHMSize);
	//	Update specified mask area vertices color
	bool UpdateHomeAreaColor(int r, int c, const DWORD* aColors1, const DWORD* aColors2, int iHMSize);
	//	Get specified mask area
	CHomeTrnMask* GetHomeAreaMask(int r, int c);
	//	Get bitmap of specified mask and layer
	CELBitmap* GetHomeAreaMaskBmp(int r, int c, int iLayer);
	//	Update specified mask area data
	bool UpdateHomeAreaMask(int r, int c, int* aLayers, int iNumLayer);
	bool UpdateHomeAreaMask(int r, int c);

	//	Get home areas which are just created and needs updating
	bool GetNextNewHomeArea(int n, int& r, int& c);

	//	Get auto scene object
	CAutoScene* GetAutoScene() { return m_pAutoScene; }
	
protected:	//	Attributes

	CAutoScene*	m_pAutoScene;	//	Auto scene object
	CELBitmap*	m_pWhiteBmp;	//	Pure white bitmap
	CELBitmap*	m_pBlackBmp;	//	Pure black bitmap

	int		m_iMaxLayerNum;		//	Maximum layer number

	AList2<BLOCKPOS, BLOCKPOS&>	m_NewBlockList;	//	Blocks whose data isn't loaded

protected:	//	Operations

	//	Create sub-terrain
	bool CreateSubTerrain();
	//	Create terrain loader
	bool CreateTerrainLoader();
	//	Create pure black and white bitmap
	bool CreatePureColorBmp();
	//	Create temporary buffers
	bool CreateTempBufs();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


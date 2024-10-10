/*
 * FILE: A3DTerrain2Loader.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/7
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DTERRAIN2LOADER_H_
#define _A3DTERRAIN2LOADER_H_

#include "ABaseDef.h"
#include "A3DTypes.h"

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

class A3DTerrain2;
class A3DTerrain2Block;
class A3DTerrain2Mask;
class A3DTexture;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2Loader
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2Loader
{
public:		//	Types

	//	Loader ID
	enum
	{
		LOADER_A = 0,
		LOADER_B,
	};

public:		//	Constructor and Destructor

	A3DTerrain2Loader(A3DTerrain2* pTerrain, int iLoaderID)
	{
		ASSERT(pTerrain);
		m_pt = pTerrain;
		m_iLoaderID = iLoaderID;
	}

	virtual ~A3DTerrain2Loader() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(const char* szTrn2File) { return true; };
	//	Release object
	virtual void Release() {};

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime) { return true; }

	//	Load block
	virtual A3DTerrain2Block* LoadBlock(int r, int c, int iBlock) = 0;
	//	Unload a block
	virtual void UnloadBlock(A3DTerrain2Block* pBlock) = 0;
	//	Load a mask area
	virtual A3DTerrain2Mask* LoadMask(int iMaskArea) = 0;
	//	Unload a mask area
	virtual void UnloadMask(A3DTerrain2Mask* pMaskArea) = 0;

	//	Get loader ID
	int GetLoaderID() { return m_iLoaderID; }

	//	Get interface
	A3DTerrain2* GetTerrain() { return m_pt; }

protected:	//	Attributes

	A3DTerrain2*	m_pt;			//	Terrain object
	int				m_iLoaderID;	//	Loader ID

protected:	//	Operations

	//	Fill mask texture content
	bool FillMaskTexture(int iMapSize, BYTE* pSrcData, A3DFORMAT DstFmt, A3DTexture* pTexture);
	//	Fill light map content
	bool FillLightMap(int iMapSize, BYTE* pSrcData, A3DFORMAT DstFmt, A3DTexture* pTexture, int iSrcBitsPerPixel = 0);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DTERRAIN2LOADER_H_

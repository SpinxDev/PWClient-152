/*
 * FILE: A3DTerrain2LoaderA.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/7
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DTERRAIN2LOADERA_H_
#define _A3DTERRAIN2LOADERA_H_

#include "AArray.h"
#include "AFile.h"
#include "A3DTerrain2Loader.h"
#include "A3DTerrain2File.h"

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

class A3DTexture;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2LoaderA
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2LoaderA : public A3DTerrain2Loader
{
public:		//	Types

	//	Index of mask buffer
	enum
	{
		MASK_L0 = 0,		//	No.1 layer
		MASK_L1,			//	No.2 layer
		MASK_L2,			//	No.3 layer
		MASK_ZERO,			//	Pure zero mask
		MASK_ONE,			//	Pure one mask
		NUM_MASKBUF,
	};

	//	Lumps of mask area data
	struct MASKLUMPS
	{
		int*	aTexIndices;	//	Texture indices
		int*	aSpecuMaps;		//	Specular map indices
		int*	aMaskSize;		//	Mask size
		DWORD*	aMaskMapOffs;	//	Mask map data offset
		BYTE*	aProjAxises;	//	Projection axises
		float*	aUVScales;		//	uv scales
		BYTE*	aWeights;		//	Layer weight
	};

public:		//	Constructor and Destructor

	A3DTerrain2LoaderA(A3DTerrain2* pTerrain);
	virtual ~A3DTerrain2LoaderA();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(const char* szTrn2File);
	//	Release object
	virtual void Release();

	//	Load block
	virtual A3DTerrain2Block* LoadBlock(int r, int c, int iBlock);
	//	Unload a block
	virtual void UnloadBlock(A3DTerrain2Block* pBlock);
	//	Load a mask area
	virtual A3DTerrain2Mask* LoadMask(int iMaskArea);
	//	Unload a mask area
	virtual void UnloadMask(A3DTerrain2Mask* pMaskArea);

protected:	//	Attributes

	AFile		m_BlockFile;		//	Block data file object
	AFile		m_MaskFile;			//	Mask data file object
	AFile		m_LightMapFile;		//	Light map file object
	MASKLUMPS	m_MaskLumps;		//	Mask area lumps
	BYTE*		m_pLMLoadBuf;		//	Buffer used to load lightmap data
	bool		m_bHasLightMap;		//	true, has light map file
	int			m_iLightMapSize;	//	Each mask area's light map size
	int			m_iLMDataSize;		//	Lightmap data length (in bytes) of each mask area
	DWORD		m_dwLMFileFlags;	//	Lightmap file flags

	BYTE*		m_aMaskBuf[NUM_MASKBUF];	//	Temporary buffers used to load mask textures
	int			m_iMaskMapSize;				//	Mask map size

	AArray<DWORD, DWORD>	m_aBlockOffs;		//	Block data offsets in terrain file
	AArray<DWORD, DWORD>	m_aMaskOffs;		//	Mask area offsets in mask file
	AArray<DWORD, DWORD>	m_aLightMapOffs;	//	Light map data offsets in light map file

protected:	//	Operations

	//	Open terrain2 block data file
	bool LoadBlockDataFile(const char* szFile);
	//	Open terrain2 mask data file
	bool LoadMaskDataFile(const char* szFile);
	//	Open terrain2 light map data file
	bool LoadLightMapFile(const char* szFile);
	//	Read lump data in mask file
	bool LoadMaskLumpData(int iLump, const A3DTRN2LOADERA::T2MKFILELUMP& Lump);
	//	Read uv scale lump in mask file
	bool LoadUVScaleLump(const A3DTRN2LOADERA::T2MKFILELUMP& Lump);
	//	Load mask map data
	bool LoadMaskMapData(A3DTerrain2Mask* pMask, int iLayer, BYTE* aMaskBufs[3]);
	//	Build 32-bit mask map from 4 8-bit mask maps
	A3DTexture* BuildMaskTexture(int iNumLayer, BYTE* aMaskBufs[3]);
	//	Load light map of a mask area
	A3DTexture* LoadMaskLightMap(int iMaskArea);
	//	Load light map of a mask area, dds version
	A3DTexture* LoadMaskLightMap_dds(int iMaskArea);
	//	Read block data from file
	bool ReadBlockData(A3DTerrain2Block* pBlock, float sx, float sz);
	//	Create mask loading buffers
	bool CreateMaskBuffers();

	//	Get mask area lump data by index
	int GetTextureIndex(int n) { return m_MaskLumps.aTexIndices[n]; }
	int GetSpecularMapIndex(int n) { return m_MaskLumps.aSpecuMaps[n]; }
	int GetMaskMapSize(int n) { return m_MaskLumps.aMaskSize[n]; }
	DWORD GetMaskMapOff(int n) { return m_MaskLumps.aMaskMapOffs[n]; }
	BYTE GetProjAxis(int n) { return m_MaskLumps.aProjAxises[n]; }
	float GetUScale(int n) { return m_MaskLumps.aUVScales[n << 1]; }
	float GetVScale(int n) { return m_MaskLumps.aUVScales[(n << 1) + 1]; }
	BYTE GetLayerWeight(int n) { return m_MaskLumps.aWeights[n]; }
	//	Get mask area's light map offset
	DWORD GetLightMapOff(int iMaskArea) { return m_aLightMapOffs[iMaskArea]; }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DTERRAIN2LOADERA_H_

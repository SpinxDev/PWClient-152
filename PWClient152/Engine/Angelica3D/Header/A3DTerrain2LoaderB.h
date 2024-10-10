/*
 * FILE: A3DTerrain2LoaderB.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DTERRAIN2LOADERB_H_
#define _A3DTERRAIN2LOADERB_H_

#include "AArray.h"
#include "AFileImage.h"
#include "A3DTerrain2Loader.h"
#include "A3DTerrain2File.h"
#include "AString.h"

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

class A3DTerrain2LoaderB;
class A3DTerrain2;
class A3DTexture;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTrn2SubLoaderB
//	
///////////////////////////////////////////////////////////////////////////

class A3DTrn2SubLoaderB
{
	enum 
	{
		LM_NIGHT = 0,
		LM_DAY,
		LM_MANUAL,
	};

public:		//	Types

	//	Sub-terrain layer information
	struct LAYER
	{
		int		iTextureIdx;	//	Texture index
		int		iSpecMapIdx;	//	Specular map index
		int		iMaskMapSize;	//	Mask map size
		BYTE	byProjAxis;		//	Texture project axis
		float	fUScale;		//	U scale of texture
		float	fVScale;		//	V scale of texture
		BYTE	byWeight;		//	Layer weight
	};

public:		//	Constructor and Destructor

	A3DTrn2SubLoaderB(A3DTerrain2LoaderB* pLoaderB);
	virtual ~A3DTrn2SubLoaderB();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(const char* szBaseName);
	//	Release object
	void Release();

	//	Load block
	A3DTerrain2Block* LoadBlock(int iBlock, float sx, float sz);
	//	Unload a block
	void UnloadBlock(A3DTerrain2Block* pBlock);
	//	Load a mask area
	A3DTerrain2Mask* LoadMask(int iMaskArea, int iIdxInSub);
	//	Unload a mask area
	void UnloadMask(A3DTerrain2Mask* pMaskArea);

	//	Activate / Disactivate this sub-terrain
	bool Activate(bool bActivate);
	//	Get block load counter
	int GetBlockLoadCnt() { return m_iBlkLoadCnt; }
	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Check activate state
	void CheckActivate();

	//	Get block number
	int GetBlockNum() { return m_aBlockOffs.GetSize(); }
	//	Get mask area number
	int GetMaskAreaNum() { return m_aMaskOffs.GetSize(); }
	//	Get layer number
	int GetLayerNum() { return m_aLayers.GetSize(); }
	//	Get layer information
	const LAYER& GetLayer(int n) { return m_aLayers[n]; }

protected:	//	Attributes

	A3DTerrain2*		m_pt;		//	Terrain object
	A3DTerrain2LoaderB*	m_pl;		//	Terrain loaderB object

	AString		m_strBaseName;		//	Base file name
	AFileImage	m_BlockFile;		//	Block data file object
	AFileImage	m_MaskFile;			//	Mask data file object
	AFileImage	m_LightMapFile;		//	Light map (day) file object
	AFileImage	m_LightMapFile1;	//	Light map (night) file object
	AFileImage	m_ColorMapFile;		//	Color map file object
	bool		m_bHasLightMap;		//	true, has lightmap (day) file
	bool		m_bHasLightMap1;	//	true, has lightmap (night) file
	bool		m_bHasColorMap;		//	true, has colormap file
	int			m_iLightMapSize;	//	Each mask area's light map size
	int			m_iLMDataSize;		//	Lightmap data length (in bytes) of each mask area
	DWORD		m_dwLMFileFlags;	//	Lightmap file flags
	BYTE		m_byBlockComp;		//	Block data flag
	bool		m_bMaskComp;		//	true, mask data is compressed
	DWORD		m_dwBlkFileVer;		//	Block data file version
	DWORD		m_dwMaskFileVer;	//	Mask data file version

	bool		m_bActive;			//	true, in active state
	int			m_iBlkLoadCnt;		//	Block load counter
	DWORD		m_dwWaitTime;		//	Wait time counter after m_iBlkLoadCnt = 0

	AArray<DWORD, DWORD>	m_aBlockOffs;		//	Block data offsets in terrain file
	AArray<DWORD, DWORD>	m_aMaskOffs;		//	Mask area offsets in mask file
	AArray<DWORD, DWORD>	m_aLightMapOffs;	//	Light map data offsets in light map file
	AArray<DWORD, DWORD>	m_aLightMapOffs1;	//	Light map data offsets in light map file
	AArray<DWORD, DWORD>	m_aColorMapOffs;	//	Color map data offsets in color map file
	AArray<LAYER, LAYER&>	m_aLayers;			//	Sub-terrain layers

protected:	//	Operations

	//	create terrain mask instance
	virtual A3DTerrain2Mask* CreateTerrain2Mask(A3DTerrain2* pTrn, int iMaskArea) const;
	//	Open terrain2 block data file
	bool LoadBlockDataFile(const char* szFile);
	//	Open terrain2 mask data file
	bool LoadMaskDataFile(const char* szFile);
	//	Open terrain2 light map data file
	bool LoadLightMapFile(const char* szFile, int iLMType);
	//	Open terrain2 color map data file
	bool LoadColorMapFile(const char* szFile);
	//	Load mask map data, pixel shader version
	bool LoadMaskMapData(A3DTerrain2Mask* pMask, int iLayer, BYTE* aMaskBufs[3]);
	//	Build 32-bit mask map from 4 8-bit mask maps
	A3DTexture* BuildMaskTexture(int iNumLayer, BYTE* aMaskBufs[3]);
	//	Load mask textures from mask file
	bool LoadMaskTextures(AFile* pFile, int iNumMaskTex, A3DTerrain2Mask* pMask);
	//	Load mask map data and create mask map texture, non-ps version
	A3DTexture* LoadMaskMap(A3DTerrain2Mask* pMask, int iLayer);
	//	Load light map of a mask area, non dds version
	A3DTexture* LoadMaskLightMap(int iMaskArea, int iLMType);
	//	Load light map of a mask area, dds version
	A3DTexture* LoadMaskLightMap_dds(int iMaskArea, int iLMType);
	//	Read block data from file
	bool ReadBlockData(A3DTerrain2Block* pBlock, float sx, float sz);
	//	Create mask loading buffers
	bool CreateMaskBuffers();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2LoaderB
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2LoaderB : public A3DTerrain2Loader
{
public:		//	Types

	//	Index of mask buffer
	enum
	{
		MASK_L0 = 0,	//	No.1 layer
		MASK_L1,		//	No.2 layer
		MASK_L2,		//	No.3 layer
		MASK_ZERO,		//	Pure zero mask
		MASK_ONE,		//	Pure one mask
		NUM_MASKBUF,
	};

	friend class A3DTrn2SubLoaderB;

public:		//	Constructor and Destructor

	A3DTerrain2LoaderB(A3DTerrain2* pTerrain);
	virtual ~A3DTerrain2LoaderB();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(const char* szTrn2File);
	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);

	//	Load block
	virtual A3DTerrain2Block* LoadBlock(int r, int c, int iBlock);
	//	Unload a block
	virtual void UnloadBlock(A3DTerrain2Block* pBlock);
	//	Load a mask area
	virtual A3DTerrain2Mask* LoadMask(int iMaskArea);
	//	Unload a mask area
	virtual void UnloadMask(A3DTerrain2Mask* pMaskArea);

	//	Get lightmap load buffer
	BYTE* GetLMLoadBuffer(int iLightMapSize, int iBufLen);
	BYTE* GetColMapLoadBuffer(int iBufLen);
	//	Deactivate idle sub-terrains
	void DeactivateIdleSubTerrains();

protected:	//	Attributes

	BYTE*		m_pLMLoadBuf;				//	Buffer used to load lightmap data
	BYTE*		m_aMaskBuf[NUM_MASKBUF];	//	Temporary buffers used to load mask textures
	int			m_iMaskMapSize;				//	Mask map size
	int			m_iLightMapSize;			//	Each mask area's light map size
	BYTE*		m_pColMapData;				//	Temporary buffer used to load color map data
	int			m_iCMDataSize;				//	Size of Temporary buffer used to load color map data
	int			m_iMaskMapBufSize;			//	Size of Temporary buffer used to load mask map data
	BYTE*		m_pHeightData;				//	Temporary buffer used to load block's height data
	BYTE*		m_pNormalData;				//	Temporary buffer used to load block's normal data
	BYTE*		m_pHeightData2;				//	Temporary buffer used to load block's height data
	BYTE*		m_pNormalData2;				//	Temporary buffer used to load block's normal data
	BYTE*		m_pDiffData;				//	Temporary buffer used to load block's diffuse data
	BYTE*		m_pMaskMapData;				//	Temporary buffer used to load mask map data

	AString				m_strPathName;		//	File path name
	APtrArray<AString*>	m_aSubTrnNames;		//	Sub-terrain file name
	
	APtrArray<A3DTrn2SubLoaderB*>	m_aSubTrnLoaders;	//	Sub-terrain loaders

protected:	//	Operations

	//	Create sub loader
	virtual A3DTrn2SubLoaderB* CreateSubLoader();
	//	Load terrain2 description file
	bool LoadDescriptionFile(const char* szFile);
	//	Build 32-bit mask map from 4 8-bit mask maps
	A3DTexture* BuildMaskTexture(int iNumLayer, BYTE* aMaskBufs[4]);
	//	Create mask loading buffers
	bool CreateMaskBuffers();
	//	Get temporary buffers used to load block data
	bool GetBlockTempBuffers(BYTE** ppHeiBuf, BYTE** ppNormalBuf, BYTE** ppDiffBuf, BYTE** ppHeiBuf2, BYTE** ppNormalBuf2);
	//	Get temporary buffer used to load mask map data
	BYTE* GetMaskMapTempBuffer(int iSize);
	BYTE* GetMaskMapTempBuffer() { return GetMaskMapTempBuffer(m_iMaskMapSize * m_iMaskMapSize * 2); }

	//	Convert a global mask area index to sub-terrain index
	void ConvertMaskAreaIdx(int iMaskArea, int& iSubTrn, int& iIdxInSubTrn);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DTERRAIN2LOADERB_H_

/*
 * FILE: A3DTerrain2Blk.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DTERRAIN2BLK_H_
#define _A3DTERRAIN2BLK_H_

#include "AString.h"
#include "A3DTerrain2File.h"
#include "A3DGeometry.h"
#include "A3DTypes.h"
#include "AArray.h"
#include "ARect.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Vertex FVF
#define A3DTRN2VERT1_FVF	D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX4
#define A3DTRN2VERT2_FVF	D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX4

//	Color map unit white is (0.5, 0.5, 0.5), will be multiplied by 2.0 in shader,to map [0, 255] to [0.0, 2.0), we want it to be 1.0, so we get 0x80
//	here it is:
//	(255 - 0) / x = (2.0 - 0.0) / 1.0
//	x = 128 (0x80)
//	Max lightness is 1.9921875 in shader (in order to give 0x80 / 0xFF -> 1.0f / 1.9921875 value)
#define COLORMAP_UNITWHITE	0x80808080

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

//	Trerrain2 vertex format when use lightmap
struct A3DTRN2VERTEX1
{
	float	vPos[3];		//	Position
	float	vNormal[3];		//	Normal
	float	u1, v1;			//	Texture coordinates project on xz
	float	u2, v2;			//	Texture coordinates project on xy
	float	u3, v3;			//	Texture coordinates project on yz
	float	u4, v4;			//	Texture coordinate of layer mask
};

//	Terrain2 vertex format when use vertex-light (not lightmap)
struct A3DTRN2VERTEX2
{
	float	vPos[3];		//	Position
	DWORD	dwDiffuse;		//	Diffuse color
	DWORD	dwSpecular;		//	Specular color
	float	u1, v1;			//	Texture coordinates project on xz
	float	u2, v2;			//	Texture coordinates project on xy
	float	u3, v3;			//	Texture coordinates project on yz
	float	u4, v4;			//	Texture coordinate of layer mask
};

class AFile;
class A3DDevice;
class A3DTexture;
class A3DVECTOR3;
class A3DRenderTarget;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2Mask
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2Mask
{
public:		//	Types

	//	Layer information
	struct LAYER
	{
		int		iTextureIdx;	//	Texture index
		int		iSpecMapIdx;	//	Specular map index
		int		iMaskMapSize;	//	Mask map size
		BYTE	byProjAxis;		//	Texture project axis
		float	fUScale;		//	U scale of texture
		float	fVScale;		//	V scale of texture
		BYTE	byWeight;		//	Layer weight
		DWORD	dwMapDataOff;	//	Mask map data offset in data file
		DWORD	dwDataSize;		//	Mask map data size in data file
	};

	friend class A3DTerrain2;
	friend class A3DTerrain2LoaderA;
	friend class A3DTrn2SubLoaderB;

public:		//	Constructor and Destructor

	A3DTerrain2Mask(A3DTerrain2* pTerrain, int iMaskArea);
	virtual ~A3DTerrain2Mask();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(int iNumLayer);
	//	Release object
	virtual void Release();

	//	Get index of this mask area in whole world
	int GetMaskAreaIndex() { return m_iMaskArea; }
	//	Get number of layer
	int GetLayerNum() const { return m_aLayers.GetSize(); }
	//	Get texture of specified layer
	A3DTexture* GetTexture(int iLayer) const { return m_aTextures[iLayer]; }
	//	Get specular map of specified layer
	A3DTexture* GetSpecularMap(int iLayer) const { return m_aSpecularMaps[iLayer]; }
	//	Get mask texture by index
	A3DTexture* GetMaskTexture(int n) const { return m_aMaskTextures[n]; }
    //	Get mask texture count
    int GetMaskTextureCount() const { return m_aMaskTextures.GetSize(); }
	//	Get light map texture
	A3DTexture* GetLightMap(bool bDay) const { return bDay ? m_pLMTexture : m_pLMTexture1; }
	//	Get color map texture
	A3DTexture* GetDetailMap() const { return m_pColTexture; }
	//	Get merged texture
	A3DRenderTarget* GetMergedTexture() const { return m_pMergedTex; }
	//	Get mask area
	const ARectF& GetMaskArea() const { return m_rcArea; }

	//	Get layer properties
	int GetTextureIndex(int iLayer) const { return m_aLayers[iLayer].iTextureIdx; }
	int GetSpecularMapIndex(int iLayer) const { return m_aLayers[iLayer].iSpecMapIdx; }
	int GetMaskMapSize(int iLayer) const { return m_aLayers[iLayer].iMaskMapSize; }
	BYTE GetProjAxis(int iLayer) const { return m_aLayers[iLayer].byProjAxis; }
	float GetUScale(int iLayer) const { return m_aLayers[iLayer].fUScale; }
	float GetVScale(int iLayer) const { return m_aLayers[iLayer].fVScale; }
	BYTE GetLayerWeight(int iLayer) const { return m_aLayers[iLayer].byWeight; }
	LAYER& GetLayer(int n) { return m_aLayers[n]; }

	//	Get terrain object
	A3DTerrain2* GetTerrain() { return m_pTerrain; }

protected:	//	Attributes

	A3DDevice*			m_pA3DDevice;	//	Device object
	A3DTerrain2*		m_pTerrain;		//	Terrain object
	A3DTexture*			m_pLMTexture;	//	Light map texture for day
	A3DTexture*			m_pLMTexture1;	//	Light map texture for night
	A3DTexture*			m_pColTexture;	//	Color map texture
	A3DRenderTarget*	m_pMergedTex;	//	Merged texture
	bool				m_bOwnLM;		//	true, m_pLMTexture is owned by mask itself
	bool				m_bOwnLM1;		//	true, m_pLMTexture1 is owned by mask itself
	int					m_iMaskArea;	//	Index of this mask area
	ARectF				m_rcArea;		//	Area convered by mask

	APtrArray<A3DTexture*>	m_aTextures;		//	Texture array
	APtrArray<A3DTexture*>	m_aSpecularMaps;	//	Specular map array
	APtrArray<A3DTexture*>	m_aMaskTextures;	//	Mask map array
	AArray<LAYER, LAYER&>	m_aLayers;			//	Layer information

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2Block
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2Block
{
public:		//	Types

	//	Neighbour index
	enum
	{
		NB_LEFT = 0,
		NB_TOP,
		NB_RIGHT,
		NB_BOTTOM,
	};

	//	Block render data
	struct RENDERDATA
	{
		void*	aVertices;		//	Vertex buffer
		int		iNumVert;		//	Number vertex
		WORD*	aIndices[5];	//	Indices of 4 border and block center area
		int		aIdxNum[5];		//	Index number of in aIndices arrays
		int		iNumIndex;		//	Total index number
	};

	//	Vertex extra data used in non-pixel shader version
	struct VERTEXEXTRA
	{
		A3DVECTOR3	vNormal;	//	Normal
		DWORD		dwDiffuse;	//	Diffuse color for day
		DWORD		dwDiffuse1;	//	Diffuse color for night
	};

	friend class A3DTerrain2;
	friend class A3DTerrain2LoaderA;
	friend class A3DTrn2SubLoaderB;

public:		//	Constructor and Destructor

	A3DTerrain2Block(A3DTerrain2* pTerrain);
	virtual ~A3DTerrain2Block();

public:		//	Attributes

public:		//	Operations

	//	Initalize object
	virtual bool Init();
	//	Release object
	virtual void Release();

	//	Get data loaded flag
	bool IsDataLoaded() const { return m_bDataLoaded; }
	//	Get block size in logic unit (metres)
	float GetBlockSize() const { return m_fBlockSize; }
	//	Get row in mask area
	int GetRowInMask() const { return m_iRowInMask; }
	//	Get column in mask area
	int GetColInMask() const { return m_iColInMask; }
	//	Get row in whole terrain
	int GetRowInTerrain() const { return m_iRowInTrn; }
	//	Get column in whole terrain
	int GetColInTerrain() const { return m_iColInTrn; }
	//	Get block grid
	int GetBlockGrid() const { return m_iBlockGrid; }
	//	Get block's position and size in whole world
	const A3DAABB& GetBlockAABB() const { return m_aabbBlock; }
	//	Get index of sub-terrain this block belongs to
	int GetSubTerrainIdx() const { return m_iSubTerrain; }
	//	Get index of mask area this block belongs to
	int GetMaskAreaIdx() const { return m_iMaskArea; }
	//	Set LOD level
	void SetLODLevel(int iLevel) { m_iLODLevel = iLevel; }
	//	Get LOD level
	int GetLODLevel() const { return m_iLODLevel; }
	//	Get LOD flags
	DWORD GetLODFlags() const { return m_dwLODFlag; }
	//	Set visible flag
	void SetVisibleFlag(bool bVisible) { m_bVisible = bVisible; }
	//	Get visible flag
	bool IsVisible() const { return m_bVisible; }
	//	Get flag for this whole block no need for rendering
	bool IsNoRender() const;
	//	Get flag for this whole block not taking part in RayTrace
	bool IsNoTrace() const;
	//	Get flag for this whole block unable for GetPosHeight
	bool IsNoPosHeight() const;
	//	Get block flag
	DWORD GetBlockFlags() const { return m_dwBlockFlags; }
	//	Set block flag (render / raytrace / posheight)
	void SetBlockFlags(DWORD dwBlkFlags) { m_dwBlockFlags = dwBlkFlags; }
	//	Get LOD scale factor
	float GetLODScale() const { return m_fLODScale; }
	//	Set LOD scale factor
	void SetLODScale(float fScale) { m_fLODScale = fScale; }
	//	Get effect layer flags
	DWORD GetLayerFlags() const { return m_dwLayerFlags; }
	//	Set HC weight
	void SetHCWeight(float fWeight) { m_fHCWeight = fWeight; }
	//	Get HC weight
	float GetHCWeight() const { return m_fHCWeight; }

	//	Get vertex
	A3DVECTOR3 GetVertexPos(int n)
	{
		if (m_aVertices1)
			return A3DVECTOR3(m_aVertices1[n].vPos[0], m_aVertices1[n].vPos[1], m_aVertices1[n].vPos[2]);
		else
		{
			ASSERT(m_aVertices2);
			return A3DVECTOR3(m_aVertices2[n].vPos[0], m_aVertices2[n].vPos[1], m_aVertices2[n].vPos[2]);
		}
	}

	//	Set neighbour block
	void SetNeighbour(int iIndex, A3DTerrain2Block* pBlock)
	{
		ASSERT(iIndex >= 0 && iIndex < 4);
		m_aNeighbours[iIndex] = pBlock;
	}

	//	Adjust block's LOD level
	void AdjustLODLevel();
	//	Build render data
	bool BuildRenderData();
	//	Get render data
	const RENDERDATA* GetRenderData() { return &m_RenderData; }
	//	Get first effect layer index
	int GetFirstEffectLayer() { return m_iFirstLayer; }
	//	Check whether a layer has effect to this block
	bool IsEffectLayer(int iLayer) { return (m_dwLayerFlags & (1 << iLayer)) ? true : false; }

	//	Note: Below functions are used by editors, use them carefully !
	//	Fill vertex buffer of stream
	bool FillVertexBuffer(float sx, float sz);
	//	Fill vertex color data
	bool FillVertexColor(ARectI* prcArea=NULL);
	//	Set block size
	void SetBlockSize(float fSize) { m_fBlockSize = fSize; }

	//	Copy block vertices to destination buffer
	bool CopyVertexPos(A3DVECTOR3* pDestBuf, int iWid, int iHei, int iDestPitch, int sx, int sy);
	//	Copy grid faces to destination buffer
	bool CopyFaces(A3DVECTOR3* pDestVert, WORD* pDestIdx, int iWid, int iHei, int iDstVertPitch, int iBaseVert, int sx, int sy);

	//	Get terrain object
	A3DTerrain2* GetTerrain() { return m_pTerrain; }

protected:	//	Attributes

	A3DDevice*		m_pA3DDevice;	//	Device object
	A3DTerrain2*	m_pTerrain;		//	Terrain object
	A3DTRN2VERTEX1*	m_aVertices1;	//	Vertex buffer, ps version
	A3DTRN2VERTEX2*	m_aVertices2;	//	Vertex buffer, non-ps version
	VERTEXEXTRA*	m_aVertExtras;	//	Vertex extra data in non-ps version

	bool		m_bDataLoaded;		//	Data loaded flag
	int			m_iSubTerrain;		//	Index of sub-terrain this block belongs to
	int			m_iMaskArea;		//	Index of mask area this block belongs to
	float		m_fBlockSize;		//	Block size (on x and z axis) in logic unit (metres)
	int			m_iRowInTrn;		//	Block position in whole terrain
	int			m_iColInTrn;
	int			m_iRowInMask;		//	Block position in mask area
	int			m_iColInMask;
	int			m_iBlockGrid;		//	Each block has m_iBlockGrid * m_iBlockGrid terrain grids
	A3DAABB		m_aabbBlock;		//	Block postion and size in whole world
	int			m_iNumVert;			//	Number of vertex
	int			m_iLODLevel;		//	LOD level of this block [0, 3]
	bool		m_bVisible;			//	Visible flag
	DWORD		m_dwBlockFlags;		//	Block flags
	DWORD		m_dwLODFlag;		//	LOD flag
	float		m_fLODScale;		//	LOD scale factor
	DWORD		m_dwLayerFlags;		//	Effect layer flags
	int			m_iFirstLayer;		//	First effect layer index
	RENDERDATA	m_RenderData;		//	Render data
	float		m_fDNFactor;		//	Day-night factor
	float		m_fHCWeight;		//	Weight used when terrain doing horizon culling

	A3DTerrain2Block*	m_aNeighbours[4];				//	Neighbours when do LOD

protected:	//	Operations

	//	Create vertex buffer
	bool CreateVertexBuf();
	//	Update diffuse color of block vertices
	void UpdateVertexDiffuse();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	__A3DTERRAIN2BLK_H_

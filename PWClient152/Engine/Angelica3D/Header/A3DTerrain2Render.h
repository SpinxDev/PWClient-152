/*
 * FILE: A3DTerrain2Render.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/7
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DTERRAIN2RENDER_H_
#define _A3DTERRAIN2RENDER_H_

#include "AArray.h"
#include "AList2.h"
#include "A3DMatrix.h"
#include <vector>

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
class A3DDevice;
class A3DViewport;
class A3DTerrain2Env;
class A3DStream;
class A3DOrthoCamera;
class A3DRenderTarget;
class A3DHLSL;
class A3DTexture;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTrn2RenderSlot
//	
///////////////////////////////////////////////////////////////////////////

class A3DTrn2RenderSlot
{
public:		//	Types

	enum
	{
		MAX_BLOCK_NUM = 64,		//	Maximum block number in a mask area
	};

	//	Block item
	struct BLOCK_ITEM
	{
		A3DTerrain2Block*	pBlock;		//	Block object

		DWORD	dwLODFlags;		//	LOD flags
		int		iRdOrder;		//	Render order

		void ClearStreamData()
		{
		}
	};

	friend class A3DTerrain2Render;

public:		//	Constructor and Destructor

	A3DTrn2RenderSlot(A3DTerrain2Render* pTrnRender);
	virtual ~A3DTrn2RenderSlot();

public:		//	Attributes

public:		//	Operations

	//	Reset slot for reusing
	void Reset(int iMaskIdx);

	//	Register a terrain block which will be rendered.
	//	iRdOrder: render order, more small, more early render
	bool RegisterBlock(A3DTerrain2Block* pBlock, int iRdOrder);
	//	Prepare blocks' render info
	void PrepareRenderInfo();
	//	Transfer block streams to another
	bool TransferStreams(A3DTrn2RenderSlot* pSlot);
	//	Clear block object pointers
	void ClearBlockPtrs();
	//	Get mask area index
	int GetMaskAreaIndex() const { return m_iMaskIdx; }

	//	Create streams for blocks
	void CreateStreams();
	//	Release all block streams
	void ReleaseStreams();

	//	Draw all blocks
	void DrawAllBlocks(bool bUseFVF);

	//	Get distance used to sort render slot
	int GetSortDist() const { return m_iDistance; }
	//	Get layer number used to sort render slot
	int GetSortLayerNum() const { return m_iLayerNum; }

protected:	//	Attributes

	A3DTerrain2Render*	m_pTrnRender;	//	Terrain render

	A3DStream*		m_pVertStream;		//	Vertex stream
	A3DStream*		m_pIdxStream;		//	Index stream
	int				m_iNumVert;			//	Number of vertex will be drawn
	int				m_iNumIndex;

	BLOCK_ITEM		m_aBlocks[MAX_BLOCK_NUM];	//	Block item array

	int		m_iMaskIdx;		//	Mask area index
	int		m_iDistance;	//	Distance used to sort render slot
	int		m_iLayerNum;	//	Layer number used to sort render slot
    int     m_nUsedLayerNum;
    int     m_iUsedLayerIndices[6];
    
protected:	//	Operations

	//	Calculate distance from eye to mask area
	void CalcSortDist(const A3DVECTOR3& vEyePos);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTrn2StreamMan
//	
///////////////////////////////////////////////////////////////////////////

class A3DTrn2StreamMan
{
public:		//	Types

	enum
	{
		NUM_LOD_LEVEL		= 3,
		MAX_INDEX_STREAM	= 16,
	};

	//	Index stream of a LOD level
	struct INDEX_STREAM
	{
		A3DStream*	aStreams[MAX_INDEX_STREAM];

		INDEX_STREAM();
	};

	//	Dynamic stream node
	struct STREAM_NODE
	{
		DWORD		dwTimeStamp;	//	Last access time
		A3DStream*	pStream;		//	Stream object
	};

	friend class A3DTerrain2Render;

public:		//	Constructor and Destructor

	A3DTrn2StreamMan();
	virtual ~A3DTrn2StreamMan();

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	bool Init(A3DTerrain2Render* pTrnRender);
	//	Release object
	void Release();

	//	Allocate a vertex stream
	A3DStream* AllocVertStream(int iNumVert);
	//	Free a vertex stream
	void FreeVertStream(A3DStream* pA3DStream);
	//	Allocate a index stream
	A3DStream* AllocIndexStream(int iNumIndex);
	//	Free a index stream
	void FreeIndexStream(A3DStream* pA3DStream);

	//	Get statistics info
	DWORD GetStreamNum() const { return m_dwIdxStrmCnt + m_dwVertStrmCnt; }
	DWORD GetStreamSize() const { return m_dwIdxStrmSize + m_dwVertStrmSize; }

protected:	//	Attributes

	A3DTerrain2*		m_pTrn;			//	Terrain
	A3DTerrain2Render*	m_pTrnRender;	//	Terrain render
	INDEX_STREAM		m_aIdxStrms[NUM_LOD_LEVEL];	//	Streams for total 3 LOD levels

	DWORD	m_dwIdxStrmCnt;			//	Index stream counter
	DWORD	m_dwIdxStrmSize;		//	Memory size of all index streams
	DWORD	m_dwVertStrmCnt;		//	Vertex stream counter
	DWORD	m_dwVertStrmSize;		//	Memory size of all vertex streams

	AList2<STREAM_NODE>		m_FreeVSList;	//	Free vertex streams
	AList2<STREAM_NODE>		m_FreeISList;	//	Free index streams

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2Render
//	
///////////////////////////////////////////////////////////////////////////

// 使同shader同slot的block成组
struct TerrainRenderSlotRec
{
    A3DTrn2RenderSlot* pSlot;
    A3DTerrain2Mask* pMask;
    int nCount;
    std::vector<A3DTerrain2Block*> aBlocks;

    ~TerrainRenderSlotRec();
};

// 使同shader的block成组
struct TerrainShaderSlotRec
{
    A3DHLSL* pShader;
    int nCount;
    std::vector<TerrainRenderSlotRec*> aSlots;
    ~TerrainShaderSlotRec();
};


class A3DTerrain2Render
{
public:		//	Types

	typedef APtrArray<A3DTerrain2Block*>	BlockArray;

	friend class A3DTerrain2;
	friend class A3DTrn2RenderSlot;
	friend class A3DTrn2StreamMan;

public:		//	Constructor and Destructor

	A3DTerrain2Render();
	virtual ~A3DTerrain2Render();

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	bool Init(A3DTerrain2* pTerrain);
	//	Release object
	void Release();
	
	//	After terrain data has been loaded, this function is called
	bool OnTerrainLoaded();
	//	Register a terrain block which will be rendered.
	bool RegisterBlock(A3DTerrain2Block* pBlock);
	//	Reset render information, this function is called every frame
	void ResetRenderInfo();
	//	Render routine
	// pTerrainColorRT must has depth buffer
	bool Render(A3DViewport* pViewport, bool bShadow = false, A3DRenderTarget* pTerrainColorRT = NULL, 
		float fZBias = 0.0f, float fShadowLum = 0.0f, bool bHDRLight = false, bool bOnePassHDRLM = false, bool bShadowMap = false);
	bool ZPrePass(A3DViewport* pViewport);
	//	Render raw mesh routine
	bool RenderRawMesh(A3DViewport* pViewport, bool bApplyFVF);
	
	//	Get render mesh number
	int GetRenderMeshNum() { return m_aCurSlots.GetSize(); }
	//	Get render mesh data
	bool GetRenderMesh(int iMesh, BYTE* pVertData, int iVertStride, WORD* pIdxData, int* piVertNum, int* piIdxNum, DWORD dwBlockFlag = 0);

	bool RenderShadowMap(A3DViewport* pViewport,const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRadius, const float fShadowRealRadius);
	bool RenderTerrainShadowMapL8(const A3DVECTOR3& vShadowCenter, float fShadowRealRadius, const A3DVECTOR3& vSunLightDir, float fZBias, float fShadowLum, bool bUpdateTotal);
	bool RenderWithShadowOnePass(A3DViewport* pViewport);

	bool RenderWithLightMapHDR(A3DViewport* pViewport, A3DRenderTarget* pTerrainColorRT);

	//	Set rebuild stream flag, this will force this render to rebuild streams
	void SetRebuildStreamFlag(bool bRebuild) { m_bRebuildStream = bRebuild; }

	//	Get stream manager
	A3DTrn2StreamMan* GetStreamMan() const { return m_pStreamMan; }
	A3DTexture*	GetBlackTex() const { return m_pBlackTex; }
	A3DTerrain2* GetTerrain() const { return m_pTerrain; }

protected:	//	Attributes

	A3DTerrain2*		m_pTerrain;		//	Terrain object
	A3DDevice*			m_pA3DDevice;	//	A3DDevice object
	A3DTrn2StreamMan*	m_pStreamMan;	//	Stream manager
	A3DTexture*			m_pBlackTex;	//	Black texture for convient

	A3DMATRIX4	m_matScale;			//	Scale matrix
	int			m_iVertFillCnt;		//	Vertex stream fill counter
	int			m_iIdxFillCnt;		//	Index stream fill counter
	DWORD		m_dwFogColor;

	bool		m_bRebuildStream;	//	true, force to rebuild streams

	APtrArray<A3DTrn2RenderSlot*>	m_aLastSlots;	//	Render slots of last time
	APtrArray<A3DTrn2RenderSlot*>	m_aCurSlots;	//	Render slots of this time
	APtrArray<A3DTrn2RenderSlot*>	m_aFreeSlots;	//	Slot pool
    TerrainShaderSlotRec            m_aRenderHLSLSlots[7];

protected:	//	Operations

	//	Allocate a render slot
	A3DTrn2RenderSlot* AllocRenderSlot(int iMaskArea);
	//	Free a render slot
	void FreeRenderSlot(A3DTrn2RenderSlot* pSlot);
	//	Sync last render slots to current slots
	void SyncLastRenderSlots();
	//	Sort render slots by distance
	void SortRenderSlotsByDist(const A3DVECTOR3& vEyePos);

    bool Render_PS20_NEW(A3DViewport* pViewport, bool bShadowMap);

    bool RenderSlot_PS20_NEW(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot, bool bShadowMap);
    int PrepareLayers(A3DTerrain2Block* pBlock, A3DTerrain2Mask* pMask, 
        float* pProjAxis, float* pScales, A3DVECTOR4* pEnableMask, A3DTexture** pTextures);

    //	Render routine using ps 2.0 hlsl and lightmap
	bool Render_PS20_LM(A3DViewport* pViewport, bool bShadowMap);
	template <int LAYERCNTONEPASS>
	bool RenderSlot_PS20_LM(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot, A3DHLSL* p3Layer, bool bShadowMap);

	//	Render routine using ps 1.4 and lightmap
	bool Render_PS14_LM(A3DViewport* pViewport, bool bShadow = false, bool bHDR = false, bool bOnePassHDRLM = false);
	bool RenderSlot_PS14_LM(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot, bool bShadow = false, bool bHDR = false, bool bOnePassHDRLM = false );
	//	Render routine using ps 1.4 and vertex-light
	bool Render_PS14_VL(A3DViewport* pViewport);
	bool RenderSlot_PS14_VL(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot);
	//	Render routine without ps and lightmap
	bool Render_NoPS_VL(A3DViewport* pViewport);
	bool RenderSlot_NoPS_VL(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot);
	bool ZPrePass_NoPS_VL(A3DViewport* pViewport);
	bool ZPrePassSlot_NoPS_VL(A3DViewport * pViewport, A3DTrn2RenderSlot* pSlot);

	//	Prepare rendering 1-4 layers using ps 1.4
	bool PrepareRender4Layers_PS14_LM(A3DTerrain2Mask* pMask, int iStartLayer, int iNumLayers);
	bool PrepareRender3Layers_PS14_LM(A3DTerrain2Mask* pMask, int iStartLayer, int iNumLayers);
	bool PrepareRender3Layers_PS14_VL(A3DTerrain2Mask* pMask, int iStartLayer, int iNumLayers);
	bool PrepareRender_TM_LM(A3DTerrain2Mask* pMask);
	//	Prepare vertex shader constants for render refract
	bool PrepareVertexConstants(A3DViewport* pViewport);
	//	Render HDR
	bool RenderSlot_HDRPass(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot);
	//	Render routine using Texture Merge technique, lightmap version
	bool Render_TM_LM(A3DViewport* pViewport);
	//	Render routine using Texture Merge technique, vertex light version
	bool Render_TM_VL(A3DViewport* pViewport);

	//	Apply specified layer's texture in a mask area
	void ApplyLayerTexture(int iStage, A3DTerrain2Mask* pMask, int iLayer);
	//	Apply specified layer's specular map in a mask area
	void ApplyLayerCausticMap(int iStage, A3DTerrain2Mask* pMask, int iLayer);
	//	Apply specified layer's mask texture in a mask area
	void ApplyLayerMaskTexture(int iStage, A3DTerrain2Mask* pMask, int iLayer);
	//	Apply light map
	void ApplyLightMap(int iStage, A3DTerrain2Mask* pMask, bool bDay);

	bool RenderWithShadow(A3DViewport* pViewport, A3DRenderTarget* pTerrainColorRT,float fZBias, float fShadowLum);
	bool RenderShadowSlot(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot);
	void SetShadowVSConst(A3DMATRIX4* pMatMVP, A3DMATRIX4* pMatShadowMVP, int nShadowMapSize, float fZBias);

	void SetShadowMapL8VSConst(A3DMATRIX4* pMatMVP, A3DMATRIX4* pMatShadowMVP, int nShadowMapSize, float fZBias);
	bool RenderSlot_ShadowMapL8(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot);
	bool PrepareVertexConstantsL8(A3DViewport* pViewport);
	bool PrepareRenderShadowL8PS(A3DTerrain2Mask* pMask, int iStartLayer, int iNumLayers);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2TexMerger
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2TexMerger
{
public:		//	Types

public:		//	Constructor and Destructor

	A3DTerrain2TexMerger();
	virtual ~A3DTerrain2TexMerger();

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	bool Init(A3DTerrain2* pTerrain);
	//	Release object
	void Release();
	
	//	Merge mask textures
	A3DRenderTarget* MergeMaskTextures(A3DTerrain2Mask* pTrnMask);
	//	Release merged texture of mask
	void ReleaseMergedTexture(A3DRenderTarget* pTarget);

protected:	//	Attributes

	A3DTerrain2*	m_pTerrain;		//	Terrain object
	A3DDevice*		m_pA3DDevice;	//	A3DDevice object
	A3DViewport*	m_pViewport;	//	Viewport object
	A3DOrthoCamera*	m_pCamera;		//	Camera object
	A3DStream*		m_pStream;		//	Stream object

	A3DMATRIX4	m_matScale;			//	Scale matrix
	int			m_iTexWidth;		//	Texture width
	int			m_iTexHeight;		//	Texture height

protected:	//	Operations

	//	Calculate vertex's texture coordinates
	bool CalcTextureCoords(A3DTerrain2Mask* pTrnMask);
	//	Apply specified layer's texture
	bool ApplyLayerTexture(A3DTerrain2Mask* pTrnMask, int iLayer);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DTERRAIN2RENDER_H_

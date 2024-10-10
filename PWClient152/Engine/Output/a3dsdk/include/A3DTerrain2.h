 /*
 * FILE: A3DTerrain2.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DTERRAIN2_H_
#define _A3DTERRAIN2_H_

#include "AArray.h"
#include "ARect.h"
#include "AList2.h"
#include "AString.h"
#include "AFile.h"
#include "A3DVector.h"
#include "A3DTerrain2File.h"
#include "A3DTerrain2Render.h"
#include "A3DDevObject.h"
#include "A3DTrace.h"
#include "A3DMaterial.h"

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

class A3DEngine;
class A3DDevice;
class A3DTerrain2Block;
class A3DTexture;
class A3DCameraBase;
class A3DViewport;
class A3DTerrain2Mask;
class A3DTerrain2Loader;
class A3DTerrain2LOD;
class A3DTerrain2Cull;
class A3DTerrain2TexMerger;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2 : public A3DDevObject
{
public:		//	Types

	typedef APtrArray<A3DTerrain2Block*>	BlockArray;
	typedef APtrList<A3DTerrain2Block*>		BlockList;

	//	Some constants
	enum
	{
		MAX_LAYERWEIGHT = 4,
	};

	//	LOD type
	enum LODTYPE
	{
		LOD_NONE = 0,		//	Disable LOD
		LOD_USEHEIGHT,		//	Consider camera's height when do LOD
		LOD_NOHEIGHT,		//	Don't consider camera's height when do LOD
	};

	//	Sub-terrain information
	struct SUBTERRAIN
	{
		float	fMinHei;	//	Minimum height
		float	fMaxHei;	//	Maximum height
	};

	//	Texture slot
	struct TEXTURE
	{
		AString		strFileName;	//	Texture file name
		A3DTexture*	pA3DTexture;	//	Texture object
		int			iRefCount;		//	Reference counter
	};

	//	Mask slot
	struct MASKSLOT
	{
		A3DTerrain2Mask*	pMask;	//	Mask object
		DWORD				dwRef;	//	Reference counter
	};

	//	2DDDA algorithm
	struct TWODDDA
	{
		int		iMajor;			//	Major axis
		int		aSteps[2];		//	Steps need to go
		int		vStart[2];		//	Start voxel
		int		vEnd[2];		//	End voxel
		int		s[2];			//	Sign delta
		float	e;				//	Errors' initial value
		float	i;				//	Increments
		float	d;				//	Deltas to next voxel
		float	c;				//	Compare value
	};

	//	Ray trace informatino
	struct RAYTRACE
	{
		A3DVECTOR3	vStart;		//	Start position
		A3DVECTOR3	vEnd;		//	End position
		A3DVECTOR3	vDelta;		//	Ray move delta
		A3DVECTOR3	vNormal;	//	Hit face's normal
		float		fFraction;	//	Minimum fraction
	};

	//	Temporary buffers used to load terrain block data
	struct TEMPBUFS
	{
		int		iNumVert;		//	Number of vertices in a block
		float*	aTempHei;		//	Temporary height buffer
		float*	aTempNormal;	//	Temporary normal buffer
		DWORD*	aTempDiff;		//	Temporary diffuse color buffer
		DWORD*	aTempDiff1;		//	Temporary diffuse color buffer
	};

	//	Active blocks
	struct ACTBLOCKS
	{
		ACTBLOCKS() : aBlocks(0, 128) {}

		ARectI		rcArea;		//	Active area represented in blocks
		BlockArray	aBlocks;	//	Active block array

		//	Get block object at specified row, column.
		A3DTerrain2Block* GetBlock(int r, int c, bool bClear)
		{
			ASSERT(rcArea.PtInRect(c, r));

			int iIndex = GetBlockIndex(r, c);
			A3DTerrain2Block* pBlock = aBlocks[iIndex];

			if (bClear)
				aBlocks[iIndex] = NULL;

			return pBlock;
		}

		//	Set block object at specified row, column
		void SetBlock(int r, int c, A3DTerrain2Block* pBlock)
		{
			ASSERT(rcArea.PtInRect(c, r));
			int iIndex = GetBlockIndex(r, c);
			aBlocks[iIndex] = pBlock;
		}

		//	Get block index in aBlocks
		int GetBlockIndex(int r, int c)
		{
			return (r - rcArea.top) * rcArea.Width() + c - rcArea.left;
		}
	};

	friend class A3DTerrain2LoaderA;
	friend class A3DTerrain2LoaderB;
	friend class A3DTerrain2CullHC;

public:		//	Constructor and Destructor

	A3DTerrain2();
	virtual ~A3DTerrain2();

public:		//	Attributes

public:		//	Operations

	//	Initialize terrain object
	bool Init(A3DEngine* pA3DEngine, float fViewRadius, float fActRadius, bool bNoRender=false, bool bVertexLight=false, bool bLightMap32Bit=false);
	//	Release terrain object
	virtual void Release();

	//	Before device reset
	virtual bool BeforeDeviceReset();
	//	After device reset
	virtual bool AfterDeviceReset();

	//	Load terrain data from file
	bool Load(const char* szFileName, float cx, float cz, bool bMultiThread=false);

	//	Update routine
	virtual bool Update(DWORD dwDeltaTime, A3DCameraBase* pCamera, const A3DVECTOR3& vLoadCenter);
	//	Render routine
	//  pTerrainColorRT must has depth buffer
	bool Render(A3DViewport* pViewport, bool bShadow = false, A3DRenderTarget* pTerrainColorRT = NULL,
		float fZBias = 0.0f, float fShadowLum = 0.0f,bool bHDRLightMap = false, bool bOnePassHDRLM = false, bool bShadowMap = false);
	bool ZPrePass(A3DViewport* pViewport);
	//	Render with clamp
	bool RenderForRefract(A3DViewport * pViewport, float vHeight);
	//	Render for underwater
	bool RenderUnderWater(A3DViewport * pViewport, A3DTexture * pTexCaustic, float vCaustDU, float vCaustDV);
	//	Render raw meshes without any textures
	bool RenderRawMesh(A3DViewport * pViewport, bool bCullBlock, bool bApplyFVF);
	//	Reset render information
	void ResetRenderInfo();
	//	Set external render. NULL means to use default render
	void SetExtRender(A3DTerrain2Render* pExtRender);

	//	Load normal texture
	A3DTexture* LoadTexture(int iIndex);
	//	Release normal texture
	void ReleaseTexture(int iIndex);

	//	Set center position used to load terrain blocks, this function will update
	//	all active blocks immediately
	bool SetLoadCenter(const A3DVECTOR3& vCenter);
	//	Get load center position
	const A3DVECTOR3& GetLoadCenter() { return m_vLoadCenter; }
	//	Attach terrain culler
	bool AttachTerrainCuller(A3DTerrain2Cull* pTrnCuller);
	//	Do blocks cull
	void DoBlocksCull(A3DViewport* pViewport);

	//	Ray trace function
	bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, float fTime, RAYTRACERT* pTraceRt);
	//	Get height and normal of specified position
	float GetPosHeight(const A3DVECTOR3& vPos, A3DVECTOR3* pvNormal=NULL);
	//	Get grid vertices of specified area
	bool GetVertsOfArea(const A3DVECTOR3& vCenter, int iGridWid, int iGridLen, A3DVECTOR3* pVertBuf);
	//	Get grid faces of specified area
	bool GetFacesOfArea(const A3DVECTOR3& vCenter, int iGridWid, int iGridLen, A3DVECTOR3* pVertBuf, WORD* pIdxBuf);
	bool GetFacesOfArea(const ARectI& rcGridArea, A3DVECTOR3* pVertBuf, WORD* pIdxBuf);
	//	Get light map of specified position
	bool GetPosLightMap(const A3DVECTOR3& vPos, ARectF* prcArea, A3DTexture** ppDayLM, A3DTexture** ppNightLM);

	//	Is asynchronous work method ?
	bool IsAsynchronous() const { return m_bAsynLoad; }
	//	Get block size in meters
	float GetBlockSize() const { return m_fBlockSize; } 
	//	Get block grid
	int GetBlockGrid() const { return m_iBlockGrid; }
	//	Get mask grid
	int GetMaskGrid() const { return m_iMaskGrid; }
	//	Get block row number in whole terrain
	int GetBlockRowNum() const { return m_iNumAllBlockRow; }
	//	Get block column number in whole terrain
	int GetBlockColNum() const { return m_iNumAllBlockCol; }
	//	Get sub-terrain grid
	int GetSubTerrainGrid() const { return m_iSubTrnGrid; } 
	//	Get sub-terrain number
	int GetSubTerrainNum() const { return m_aSubTerrains.GetSize(); }
	//	Get sub-terrain info.
	const SUBTERRAIN& GetSubTerrain(int n) { return m_aSubTerrains[n]; }
	//	Get grid size
	float GetGridSize() const { return m_fGridSize; }
	//	Get block number
	int GetBlockNum() const { return m_iNumBlock; }
	//	Get mask area number
	int GetMaskAreaNum() const { return m_iNumMaskArea; }
	//	Get specified mask
	A3DTerrain2Mask* GetMaskArea(int n) const { return m_aMaskSlots[n].pMask; }
	//	Set force update flag
	void SetForceUpdateFlag(bool bTrue) { m_bForceUpdate = bTrue; }
	//	Get force update flag
	bool GetForceUpdateFlag() const { return m_bForceUpdate; }
	//	Set block load speed (number of blocks loaded every frame)
	void SetBlockLoadSpeed(int iNum) { ASSERT(iNum >= 0); m_iBlkLoadSpeed = iNum; }
	//	Get block laod speed
	int GetBlockLoadSpeed() const { return m_iBlkLoadSpeed; }
	//	Get terrain area
	const ARectF& GetTerrainArea() { return m_rcTerrain; }
	//	Get terrain material
	A3DMaterial& GetTerrainMaterial() { return m_Material; }
	//	Get terrain loader ID
	int GetTerrainLoaderID() { return m_iLoaderID; }

	//	Get number of block which is waiting to be loaded
	int GetCandidateBlockNum() { return m_CandidateBlkList.GetCount(); }
	//	Get a candidate block
	bool GetNextCandidateBlock(int& iRow, int& iCol, int& iBlock);
	//	Block loading routine used by other thread
	bool ThreadLoadBlock(int r, int c, int iBlock);
	//	Block unloading routine used by other thread
	void ThreadUnloadBlock(A3DTerrain2Block* pBlock);
	//	Get next block which is waiting to be unloaded
	A3DTerrain2Block* GetNextBlockToBeUnloaded();
	//	Activate loaded blocks to current active area
	void ActivateLoadedBlocks();
	//	Get active blocks
	ACTBLOCKS* GetActiveBlocks() { return m_pCurActBlocks; }

	//	Set terrain LOD type
	void SetLODType(LODTYPE Type) { m_LODType = Type; }
	//	Get LOD enable flag
	LODTYPE GetLODType() const { return m_LODType; }
	//	Set LOD distance
	void SetLODDist(float fLevel2, float fLevel1);
	//	Get LOD level 2 distance
	float GetLOD2Dist() { return m_aLODDist[0]; }
	//	Get LOD level 1 distance
	float GetLOD1Dist() { return m_aLODDist[1]; }
	//	Get view raidus
	float GetViewRadius() const { return m_fViewRadius; }
	//	Get active area radius
	float GetActRadius() const { return m_fActRadius; }
	//	Set view radius
	void SetViewRadius(float fViewRadius);
	//	Set active radius
	void SetActRadius(float fActRadius);
	//	Set layer weight limit
	void SetMaxLayerWeight(int iWeight);
	//	Get layer weight limit
	int GetMaxLayerWeight() { return m_iLayerWeight; }
	//	Get light color scale
	const A3DCOLORVALUE& GetLightScale() { return m_LightScale; }
	//	Set light color scale
	void SetLightScale(const A3DCOLORVALUE& col) { m_LightScale = col; }
	//	Get no render flag
	bool GetNoRenderFlag() { return m_bNoRender; }
	//	Get number of rendered block of this frame
	int GetRenderedBlockNum() const { return m_iRdBlockCnt; }
	//	Get data loaded flag
	bool GetDataLoadFlag() { return m_bDataLoaded; }
	//	Is multi-threads loading
	bool IsMultiThreadLoad() { return m_bMultiThread; }
	//	Get the approximate memory size hold by terrain blocks
	int GetBlockDataSize();
	//	Calculate area represented in blocks
	void CalcAreaInBlocks(const A3DVECTOR3& vCenter, float fRadius, ARectI& rcArea);
	//	Set Day-Night factor
	void SetDNFactor(float fFactor) { m_fDNFactor = fFactor; }
	//	Get Day-Night factor
	float GetDNFactor() { return m_fDNFactor; }
	//	Set light enable flag
	void EnableLight(bool bEnable) { m_bLightEnable = bEnable; }
	//	Get light enable flag
	bool GetLightEnableFlag() const { return m_bLightEnable; } 
	//	We use main light direction for specular calculation
	//	Set main light param (currently only direction is needed)
	void SetMainLightParam(const A3DLIGHTPARAM& lightParam) { m_MainLightParam = lightParam; }
	const A3DLIGHTPARAM& GetMainLightParam() const { return m_MainLightParam; }
	//	Set vertex light flag
	void UseVertexLight(bool bTrue);
	//	Check lightmap or vertex-light is used
	bool UseLightmapTech() { return !m_bVertexLight; }
	//	Check if texture merge technique is used
	bool IsTexMergeUsed() { return m_bTexMerge; }

	//	Offset texture u and v, this must be done before Load() is called
	void OffsetTextureUV(float x, float z) { m_fTexOffU = x; m_fTexOffV = z; }
	//	Get offset of texture
	float GetTexOffsetU() { return m_fTexOffU; } 
	float GetTexOffsetV() { return m_fTexOffV; }

	//	Get temporary buffers
	TEMPBUFS& GetTempBuffers() { return m_TempBufs; }

	//	Get interfaces
	A3DEngine* GetA3DEngine() const { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() const { return m_pA3DDevice; }
	A3DTerrain2Loader* GetTerrainLoader() { return m_pTrnLoader; }
	A3DTerrain2Render* GetTerrainRender() { return &m_TerrainRender; }
	A3DTerrain2Render* GetCurTerrainRender() { return m_pCurRender; }
	A3DTerrain2LOD* GetLODManager() { return m_pLODMan; }
	A3DTerrain2Cull* GetTerrainCuller() { return m_pTrnCuller; }
	A3DTerrain2TexMerger* GetTextureMerger() { return &m_TexMerger; }

	//	Get if is rendering under water scene
	bool IsRenderUnderWater() { return m_bUnderWaterRender; }
	bool IsRenderForRefract() { return m_bRenderForRefract; }
	A3DTexture* GetCausticTexture() { return m_pTexCaustic; }
	float GetCaustDU()  { return m_vCaustDU; }
	float GetCaustDV()	{ return m_vCaustDV; }
	float GetRefractSurfaceHeight() { return m_vRefractSurfaceHeight; }

	int GetTextureCount() const { return m_aTextures.GetSize(); }
	A3DTexture* GetTexture(int iIndex) { return m_aTextures[iIndex]->pA3DTexture; }
	
	bool AABBBelowTerrain(const A3DAABB& aabb) const { return false; }
	//	Compression routines used to compress/decompress terrain2 block's
	//	height and normal data.
	static bool CompressHeightMap(const float* aSrcHeis, int iNumVert, void* pDstBuf, DWORD* pdwSize);
	static DWORD DecompressHeightMap(const void* pSrcData, float* aDstHeis);
	static bool CompressNormalMap(const float* aSrcNormals, int iNumVert, void* pDstBuf, DWORD* pdwSize);
	static DWORD DecompressNormalMap(const void* pSrcData, float* aDstNormals);
	static bool CompressColorMap(const DWORD* aSrcCols, int iNumVert, void* pDstBuf, DWORD* pdwSize);
	static DWORD DecompressColorMap(const void* pSrcData, DWORD* aDstCols);

protected:	//	Attributes

	A3DEngine*				m_pA3DEngine;		//	Engine object
	A3DDevice*				m_pA3DDevice;		//	Device object
	A3DTerrain2Render		m_TerrainRender;	//	Terrain render
	A3DTerrain2Render*		m_pCurRender;		//	Current terrain render
	A3DTerrain2TexMerger	m_TexMerger;		//	Texture merger
	A3DTerrain2Loader*		m_pTrnLoader;		//	Terrain data loader
	A3DTerrain2LOD*			m_pLODMan;			//	LOD manager
	A3DTerrain2Cull*		m_pTrnCuller;		//	Terrain culler
	A3DMaterial				m_Material;			//	Terrain material
	A3DCOLORVALUE			m_LightScale;		//	Light color scale factor
	A3DLIGHTPARAM			m_MainLightParam;	//	Main light param

	int			m_iLoaderID;			//	Terrain loader ID
	bool		m_bNoRender;			//	No render flag
	bool		m_bMultiThread;			//	Mutli-thread loading
	bool		m_bDataLoaded;			//	Terrain data loaded flag
	bool		m_bAsynLoad;			//	Asynchronously load block
	bool		m_bForceUpdate;			//	Force udpate flag
	int			m_iNumActBlockRow;		//	Row and column number of active blocks
	int			m_iNumAllBlockRow;		//	Row number of all blocks
	int			m_iNumAllBlockCol;		//	Column number of all blocks
	float		m_fTerrainWid;			//	Terrain width (x axis) in logic unit (metres)
	float		m_fTerrainLen;			//	Terrain length (z axis) in logic unit (metres)
	int			m_iLayerWeight;			//	Layer weight limit when rendering
	float		m_fDNFactor;			//	Day and Night factor; 0.0f-day; 1.0f-night

	bool		m_bRenderForRefract;	//	Render for refract
	float		m_vRefractSurfaceHeight;//	Refract surface height
	
	bool		m_bUnderWaterRender;	//	Render for under water situation
	A3DTexture* m_pTexCaustic;			//	Under water caustic texture, a reference from Terrainwater
	float		m_vCaustDU;				//	U, V offset for caustic texture layer
	float		m_vCaustDV;	
	bool		m_bLightEnable;			//	true, apply light effect on terrain
	bool		m_bVertexLight;			//	true, force to use vertex light rather than lightmap
	bool		m_bTexMerge;			//	true, use texture merge technique

	int			m_iMaxCachedBlk;		//	Maximum number of cached block
	float		m_fViewRadius;			//	View radius in logic unit (metres)
	float		m_fActRadius;			//	Active area radius in logic unit (metres)
	float		m_fBlockSize;			//	Block size (on x and z axis) in logic unit (metres)
	float		m_fBlockRadius;			//	Block radius in logic unit (meters)
	float		m_fGridSize;			//	Terrain grid size (on x and z axis) in logic unit (metres)
	int			m_iBlockGrid;			//	Each block has m_iBlockGrid * m_iBlockGrid terrain grids
	int			m_iMaskGrid;			//	Each mask area has m_iMaskGrid * m_iMaskGrid terrain grids
	int			m_iSubTrnGrid;			//	Each sub-terrain has m_iSubTrnGrid * m_iSubTrnGrid terrain grids
	int			m_iNumBlock;			//	Number of block
	int			m_iNumMaskArea;			//	Number of mask area
	ARectF		m_rcTerrain;			//	Whole terrain area in logic unit (metres)
	float		m_fTexOffU;				//	Offset of texture UV
	float		m_fTexOffV;
	
	MASKSLOT*	m_aMaskSlots;			//	Mask area slots

	ACTBLOCKS	m_ActBlocks1;			//	Active block array
	ACTBLOCKS	m_ActBlocks2;			//	Active blocks array
	ACTBLOCKS*	m_pCurActBlocks;		//	Currently active block array
	ACTBLOCKS*	m_pOldActBlocks;		//	Old active block array
	int			m_iBlkLoadSpeed;		//	Number of block loaded every frame
	A3DVECTOR3	m_vLoadCenter;			//	Center position used to load blocks

	LODTYPE		m_LODType;				//	LOD type
	float		m_aLODDist[2];			//	LOD distance

	TWODDDA		m_Block2DDA;			//	Block 2DDA information
	TWODDDA		m_Grid2DDA;				//	Grid 2DDA information
	RAYTRACE	m_RayTraceInfo;			//	Ray trace information

	TEMPBUFS	m_TempBufs;				//	Temporary buffers

	//	Statistics
	int			m_iRdBlockCnt;			//	Rendered block counter

	CRITICAL_SECTION	m_csLoadBlk;	//	Loaded block lock
	CRITICAL_SECTION	m_csUnloadBlk;	//	Unloaded block lock
	CRITICAL_SECTION	m_csMaskSlot;	//	Mask area slot lock
	BlockList			m_LdBlkList;	//	Loaded block list
	BlockList			m_UnldBlkList;	//	List of block waiting to be unloaded

	AList2<int, int>				m_CandidateBlkList;	//	Candidate block list
	APtrArray<TEXTURE*>				m_aTextures;		//	Textures
	AArray<SUBTERRAIN, SUBTERRAIN&>	m_aSubTerrains;		//	Sub-terrains

protected:	//	Operations

	//	create terrain loader B
	virtual A3DTerrain2LoaderB* CreateLoaderB();
	//	Read sub-terrain data in terrain file
	bool LoadSubTerrainData(AFile* pFile, int iNumSubTerrain);
	//	Read texture information in terrain file
	bool LoadTextureInfo(AFile* pFile, int iNumTexture);

	//	Load specified mask area data
	bool LoadMask(int iMaskArea);
	//	Unload specified mask area data
	void UnloadMask(int iMasmArea);
	//	Release all mask slots
	void ReleaseMaskSlots();

	//	Synchronously update active blocks
	bool UpdateActiveBlocks(const A3DVECTOR3& vCenter);
	//	Load all active blocks immediately
	bool LoadActiveBlocks(ACTBLOCKS& ActBlocks);
	//	Unload active blocks
	void UnloadActiveBlocks(ACTBLOCKS& ActBlocks);
	//	Load blocks from candidate list
	bool LoadCandidateBlocks();
	//	Load a block
	A3DTerrain2Block* LoadBlock(int r, int c, int iBlock);
	//	Unload a block
	void UnloadBlock(A3DTerrain2Block* pBlock);
	//	Aasynchronously update active blocks
	bool AsynUpdateActiveBlocks(const A3DVECTOR3& vCenter, bool bForceUpdate);
	//	Release all loaded blocks
	void ReleaseAllBlocks();
	//	Default blocks culling routine
	void DefaultBlocksCull(A3DViewport* pViewport, const ARectI& rcView);
	//	Update block LOD info
	void UpdateBlocksLOD(const A3DVECTOR3& vCenter);

	//	Initialize block 2DDA information
	bool InitBlock2DDA();
	//	Initiailze grid 2DDA information
	bool InitGrid2DDA(A3DTerrain2Block* pBlock);
	//	Ray trace in all active blocks
	bool RayTraceInActiveBlocks();
	//	Ray trace in active blocks
	bool RayTraceInBlock(int iBlockRow, int iBlockCol, int iBlock);
	//	Ray trace in grid
	bool RayTraceInGrid(A3DTerrain2Block* pBlock, int r, int c, int iGrid);

	//	Create temporary buffers
	bool CreateTempBufs();
	//	Release temporary buffers
	void ReleaseTempBufs();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DTERRAIN2_H_

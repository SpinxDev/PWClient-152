/*
 * FILE: EC_Scene.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AArray.h"
#include "AList2.h"
#include "hashtab.h"
#include "AFile.h"
#include "A3DVector.h"
#include "A3DTypes.h"
#include "EC_WorldFile.h"
#include "EC_Counter.h"

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

class CECSceneObject;
class CECSceneBlock;
class CECViewport;
class A3DCameraBase;
class CELTree;
class CELGrassType;
class CECScnBoxArea;

class CECRandomMapProcess;
///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECScene
//	
///////////////////////////////////////////////////////////////////////////

class CECScene
{
public:		//	Types

	typedef abase::hashtab<CECSceneObject*, int, abase::_hash_function>	ObjectTable;
	typedef APtrArray<CECSceneBlock*>	BlockArray;

	friend class CECWorld;

	//	Active blocks
	struct ACTBLOCKS
	{
		ACTBLOCKS() : aBlocks(0, 128) {}

		ARectI		rcArea;		//	Active area represented in blocks
		BlockArray	aBlocks;	//	Active block array

		//	Get block object at specified row, column.
		CECSceneBlock* GetBlock(int r, int c, bool bClear)
		{
			ASSERT(rcArea.PtInRect(c, r));

			int iIndex = GetBlockIndex(r, c);
			CECSceneBlock* pBlock = aBlocks[iIndex];

			if (bClear)
				aBlocks[iIndex] = NULL;

			return pBlock;
		}

		//	Set block object at specified row, column
		void SetBlock(int r, int c, CECSceneBlock* pBlock)
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

	class CFloatShader
	{
	public:

		float cur;		//	Current value
		float target;	//	Target value
		float delta;	//	Value change delta

	public:

		void Reset(float v) { cur = target = v; delta = 0.0f; }

		void SetTarget(float t, float fInvTime)
		{
			if (!fInvTime)
				Reset(t);
			else
			{
				target = t;
				delta  = (t - cur) * fInvTime;
			}
		}

		static void Shade(float& f, float f1, float fDelta, float fTime)
		{
			if (!fDelta || f == f1)
				return;

			f += fDelta * fTime;
			if ((fDelta > 0.0f && f > f1) || (fDelta < 0.0f && f < f1))
				f = f1;
		}

		void Shade(float fTime) { Shade(cur, target, delta, fTime); }
	};

	class CColorShader
	{
	public:

		A3DCOLORVALUE cur;		//	Current value
		A3DCOLORVALUE target;	//	Target value
		A3DCOLORVALUE delta;	//	Value change delta

	public:

		void Reset(const A3DCOLORVALUE& v) { cur = target = v; delta.Set(0.0f, 0.0f, 0.0f, 0.0f); }

		void SetTarget(const A3DCOLORVALUE& t, float fInvTime)
		{
			if (!fInvTime)
				Reset(t);
			else
			{
				target = t;
				delta  = (t - cur) * fInvTime;
			}
		}

		void Shade(float fTime)
		{
			CFloatShader::Shade(cur.r, target.r, delta.r, fTime);
			CFloatShader::Shade(cur.g, target.g, delta.g, fTime);
			CFloatShader::Shade(cur.b, target.b, delta.b, fTime);
			CFloatShader::Shade(cur.a, target.a, delta.a, fTime);
		}
	};

	//	Environment information
	struct ENVIRONMENT
	{
		bool			bUnderWater;	//	Under water flag
		DWORD			dwAreaID;		//	Current area ID

		CColorShader	FogCol;			//	Fog color
		CFloatShader	FogStart;		//	Fog start distance
		CFloatShader	FogEnd;			//	Fog end distance
		CFloatShader	FogDensity;		//	Fog density

		CColorShader	FogCol_n;		//	Fog color at night
		CFloatShader	FogStart_n;		//	Fog start distance at night
		CFloatShader	FogEnd_n;		//	Fog end distance at night
		CFloatShader	FogDensity_n;	//	Fog density at night

		CColorShader	UWFogCol;		//	Under water fog color
		CFloatShader	UWFogStart;		//	Under water fog start distance
		CFloatShader	UWFogEnd;		//	Under water fog end distance
		CFloatShader	UWFogDensity;	//	Under water fog density
		CColorShader	UWFogCol_n;		//	Under water fog color at night
		CFloatShader	UWFogStart_n;	//	Under water fog start distance at night
		CFloatShader	UWFogEnd_n;		//	Under water fog end distance at night
		CFloatShader	UWFogDensity_n;	//	Under water fog density at night

		CFloatShader	SkySpeedU;		//	Sky move speed on U axis
		CFloatShader	SkySpeedV;		//	Sky move speed on V axis

		CColorShader	Ambient;		//	Ambient color
		CColorShader	DirLtColor;		//	Directional light color
		CColorShader	Ambient_n;		//	Ambient color at night
		CColorShader	DirLtColor_n;	//	Directional light color at night

		CColorShader	CloudAmb;		//	Cloud ambient color
		CColorShader	CloudAmb_n;		//	Cloud ambient color at night
		CColorShader	CloudDirCol;	//	Cloud directional color
		CColorShader	CloudDirCol_n;	//	Cloud directional color at night
	};

	struct TREETYPE
	{
		CELTree*	pTree;
		AString		strSPDTreeFile;		//	Speed tree file
		AString		strCompMapFile;		//	Composite map file name
		DWORD		dwIdleTime;			//	Idle time counter
	};

	struct GRASSTYPE
	{
		CELGrassType*		pGrass;
		ECWDFILEGRASSTYPE	Data;
		AString				strGrassFile;	//	Grass model file
		DWORD				dwIdleTime;		//	Idle time counter
	};

	//	Sub-terrain
	struct SUBTRNDATA
	{
		AFile*		pDataFile;
		int			iDataRefCnt;
		int			iNumOffset;
		DWORD*		aOffsets;
		AString		strFile;
		DWORD		dwIdleTime;
		DWORD		dwVersion;
		bool		bCompress;
	};

	struct NAVIGATEBEZIERFILEHEADER
	{
		int		iVersion;		//	Version
		int		iNumBezier;		//	Number of bezier route
	};

public:		//	Constructor and Destructor

	CECScene(CECWorld* pWorld);
	virtual ~CECScene();

public:		//	Attributes

public:		//	Operations

	//	Load scene data from file
	bool Load(const char* szFile, float cx, float cz);
	//	Release scene
	void Release();

	//	Update routine
	bool Update(DWORD dwDeltaTime, A3DCameraBase* pCamera, const A3DVECTOR3& vLoadCenter);
	//	Render routine
	bool Render(CECViewport* pViewport);
	//	Adjust environment
	bool AdjustEnvironment(DWORD dwDeltaTime, A3DCameraBase* pCamera, bool bInit);

	//	Set center position used to load scene blocks, this function will update
	//	all active blocks immediately
	bool SetLoadCenter(const A3DVECTOR3& vCenter);
	//	Get load center position
	const A3DVECTOR3& GetLoadCenter() { return m_vLoadCenter; }

	//	Load water area
	bool LoadWaterArea(DWORD dwExportID, DWORD dwOffset,float cx,float cz);
	//	Load box area
	bool LoadBoxArea(DWORD dwExportID, DWORD dwOffset,float cx,float cz);
	//	Load grass area
	bool LoadGrassArea(DWORD dwExportID, DWORD dwOffset,float cx,float cz);
	//	Load critter group
	bool LoadCritterGroup(DWORD dwExportID, DWORD dwOffset,float cx,float cz);
	//	Load ornament
	bool LoadOrnament(DWORD dwExportID, DWORD dwOffset, DWORD* pdwOnmtID,float cx,float cz);
	//	Load bezier curve
	bool LoadBezier(DWORD dwExportID, DWORD dwOffset,float cx,float cz);
	//	Load bezier for navigate
	bool LoadBezierNavigate(const char* szFile, float cx,float cz);

	//	Get specified tree type. If it doesn't exist, create it.
	CELTree* GetTreeType(DWORD dwType);
	//	Get specified grass type, if it doesn't exist, create it
	CELGrassType* GetGlassType(DWORD dwType);
	//	Release scene object
	void ReleaseSceneObject(DWORD dwExportID);
	//	Get active area radius
	float GetActRadius() const { return m_fActRadius; }
	//	Set active radius
	void SetActRadius(float fActRadius) { m_fActRadius = fActRadius; }
	//	Set block load speed (number of blocks loaded every frame)
	void SetBlockLoadSpeed(int iNum) { ASSERT(iNum >= 0); m_iBlkLoadSpeed = iNum; }
	//	Get block laod speed
	int GetBlockLoadSpeed() const { return m_iBlkLoadSpeed; }
	//	Get number of block which is waiting to be loaded
	int GetCandidateBlockNum() { return m_CandidateBlkList.GetCount(); }

	//	Get center position of world
	float GetCenterX() { return m_fWorldCenterX; }
	float GetCenterZ() { return m_fWorldCenterZ; }
	//	Get block size
	float GetBlockSize() { return m_fBlockSize; }
	//	Get scene file version
	int GetSceneFileVersion() { return m_iSceneVersion; }
	//	Get BSD file version
	int GetBSDFileVersion() { return m_iBSDVersion; }
	//	Get scene file flags
	DWORD GetSceneFileFlags() { return m_dwScnFileFlags; }

	//	Get scene object through it's export ID
	CECSceneObject* GetSceneObject(DWORD dwExportID);
	CECSceneObject* GetWaterObject(DWORD dwExportID);
	CECSceneObject* GetOnmtObject(DWORD dwExportID);
	CECSceneObject* GetBoxAreaObject(DWORD dwExportID);
	CECSceneObject* GetGrassObject(DWORD dwExportID);
	CECSceneObject* GetCritterObject(DWORD dwExportID);
	CECSceneObject* GetBezierObject(DWORD dwExportID);
	CECSceneObject* GetBezierObjectByGlobalID(int iGlobalID);

	//	Get world object
	CECWorld* GetWorld() { return m_pWorld; }
	//	Get file object
	AFile* GetSceneFile() { return &m_SceneFile; }
	AFile* GetBSDFile() { return &m_BSDFile; }

	void GetNewPosOffset(int r,int c,float& x,float& z);
	int GetSubTrnIndex(int row,int col);
	
protected:	//	Attributes

	CECWorld*		m_pWorld;			//	World object
	ObjectTable		m_WaterTab;			//	Water object table
	ObjectTable		m_BoxAreaTab;		//	Box area object table
	ObjectTable		m_GrassTab;			//	Grass area table
	ObjectTable		m_CritterTab;		//	Critter group table
	ObjectTable		m_OnmtTab;			//	Ornament table
	ObjectTable		m_BezierTab;		//	Bezier table
	ObjectTable		m_BezierForceNavigateTab; //	Bezier table for navigate

	AFile			m_SceneFile;		//	Scene file
	AFile			m_BSDFile;			//	Scene block shared data file
	int				m_iSceneVersion;	//	Scene file version
	int				m_iBSDVersion;		//	Scene block shared data file version
	DWORD			m_dwScnFileFlags;	//	Scene file flags

	ACTBLOCKS		m_ActBlocks1;		//	Active block array
	ACTBLOCKS		m_ActBlocks2;		//	Active blocks array
	ACTBLOCKS*		m_pCurActBlocks;	//	Currently active block array
	ACTBLOCKS*		m_pOldActBlocks;	//	Old active block array
	int				m_iBlkLoadSpeed;	//	Number of block loaded every frame
	A3DVECTOR3		m_vLoadCenter;		//	Load center
	CECCounter		m_ResLoadCnt;		//	Resource loading time counter
	CECCounter		m_BrushManCnt;		//	Brush man time counter

	float			m_fWorldWid;		//	world width (x axis) in logic unit (metres)
	float			m_fWorldLen;		//	World length (z axis) in logic unit (metres)
	float			m_fActRadius;		//	Active area radius in logic unit (metres)
	float			m_fBlockSize;		//	Block size (on x and z axis) in logic unit (metres)
	int				m_iNumBlock;		//	Number of block
	int				m_iNumBlockRow;		//	Block row number in whole world
	int				m_iNumBlockCol;		//	Block column number of whole world
	ARectF			m_rcWorld;			//	Whole world area in logic unit (metres)
	float			m_fWorldCenterX;	//	Center position of whole world
	float			m_fWorldCenterZ;
	DWORD			m_dwIdleTime;		//	Idle time counter

	ENVIRONMENT		m_Env;				//	Environment parameters
	bool			m_bTransFromOtherArea;	//	正在从其它天空盒子过渡

	AArray<DWORD, DWORD>	m_aBlockOffs;		//	Block data offset in file
	AList2<int, int>		m_CandidateBlkList;	//	Candidate block list
	APtrArray<TREETYPE*>	m_aTreeTypes;		//	Tree type array
	APtrArray<GRASSTYPE*>	m_aGrassTypes;		//	Grass type array
	APtrArray<SUBTRNDATA*>	m_aSubTrnData;		//	Sub-terrain data 

protected:	//	Operations

	void GetNewBlockRowCol(int& row,int& col);	
	void GetOffsetByRowCol(int row,int col,float& cx,float& cz);
	void GetSubTernByBlockRowCol(int& r,int& c);
	int GetSubTernColCount();	
	int GetBlockCountInSubTern();
	float GetSubTernSize();

	//	Open scene data file
	bool OpenSceneFile(const char* szFile, ECWDFILEHEADER* pHeader);
	//	Open scene block shared data file
	bool OpenBSDFile(const char* szFile);
	//	Read block offsets data in scene data file
	bool LoadBlockOffsetsData(int iNumBlock);
	//	Load tree type data in scene data file
	bool LoadTreeTypeData(int iNumType, DWORD dwOffset);
	//	Load grass type data in scene data file
	bool LoadGrassTypeData(int iNumType, DWORD dwOffset);
	//	Update tree and grass types
	void UpdatePlantTypes(DWORD dwDeltaTime);
	//	Apply weather of box area
	void ApplyAreaWeather(CECScnBoxArea* pArea);

	//	Prepare to load data from sub-terrain data file
	SUBTRNDATA* PrepareSubTerrainData(int iSubTrn, int iIndex);
	//	Release sub-terrain data
	void ReleaseSubTerrainData(int iSubTrn);
	//	Unload idle sub-terrain data
	void UnloadIdleSubTerrainData();

	//	Calculate area represented in blocks
	void CalcAreaInBlocks(const A3DVECTOR3& vCenter, float fRadius, ARectI& rcArea);
	//	Synchronously update active blocks
	bool UpdateActiveBlocks(const A3DVECTOR3& vCenter);
	//	Load all active blocks immediately
	bool LoadActiveBlocks(ACTBLOCKS& ActBlocks);
	//	Unload active blocks
	void UnloadActiveBlocks(ACTBLOCKS& ActBlocks);
	//	Load blocks from candidate list
	bool LoadCandidateBlocks();
	//	Load a block
	CECSceneBlock* LoadBlock(int r, int c, int iBlock);
	//	Unload a block
	void UnloadBlock(CECSceneBlock* pBlock);
	//	Release all loaded blocks
	void ReleaseAllBlocks();
	//	Update block resources
	void UpdateBlockRes();
	
	bool SkyTransToDaytime(CECScnBoxArea *pCurArea, DWORD dwTransTime);
	bool SkyTransToNight(CECScnBoxArea *pCurArea, DWORD dwTransTime);
	
	void SkyTransToDaytimeFromDayNightBetweenBySkipTime(CECScnBoxArea *pCurArea);
	void SkyTransToNightFromDayNightBetweenBySkipTime(CECScnBoxArea *pCurArea);
	
	void SkyTransToDaytimeFromDayNightBetweenByAccelerating(CECScnBoxArea *pCurArea);
	void SkyTransToNightFromDayNightBetweenByAccelerating(CECScnBoxArea *pCurArea);

	void SkyUpdate(DWORD dwDeltaTime, CECScnBoxArea* pCurArea, bool bInit, bool bSwitchFromOtherArea);
	void SkyForceLoadTexture();
	void SkyFinishTrans();
	void SkyInit(CECScnBoxArea* pCurArea);
	void SkyTransCurrent(CECScnBoxArea* pCurArea);
	void SkyTransFromOtherAreaStart(CECScnBoxArea* pCurArea);
	void SkyTransFromOtherAreaUpdate(DWORD dwDeltaTime, CECScnBoxArea* pCurArea);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



/*
* FILE: A3DWaterArea2.h
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: liyi, 2012, 3, 5
*
* HISTORY:
*
* Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
*/
#ifndef _A3D_WATER_AREA_2_H_
#define _A3D_WATER_AREA_2_H_

#include "AArray.h"
#include "ABaseDef.h"
//#include "A3DColorValue.h"
#include "A3DGeometry.h"
#include "A3DVector.h"

#include "ARect.h"
//#include "AExpDecl.h"

//FIXME!! water2
#include "A3DWaterWave2.h"

// class that represent a rectangle water area at a specified height
#define WATERGRID_VISIBLE	0x1
#define WATERGRID_DARK		0x2
#define WATERGRID_EDGE		0x4

//////////////////////////////////////////////////////////////////////////////////////////
//
//	 Types
//
//////////////////////////////////////////////////////////////////////////////////////////

#define A3DWATERVERT2_FVF		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1

struct A3DWATERVERTEX2
{
	A3DWATERVERTEX2() {}
	A3DWATERVERTEX2(const A3DVECTOR3& _pos, const A3DVECTOR3& _normal, float _tu, float _tv)
	{
		vPos = _pos;
		vNormal = _normal;
		tu = _tu;
		tv = _tv;
	}

	A3DVECTOR3	vPos;		//	Position
	A3DVECTOR3  vNormal;	//	Normal
	float		tu, tv;		//	texture coordinates
};

class A3DTerrainWater2;
class A3DWaterArea2;
class A3DStream;
class A3DMtlWaterBase;
class A3DViewport;
class A3DWaterWave2;
class A3DOcclusionProxy;

//////////////////////////////////////////////////////////////////////////////////////////
//
//	 A3DWaterMeshBlock
//
//////////////////////////////////////////////////////////////////////////////////////////

class A3DWaterMeshBlock2
{
public:		//	Types

	enum
	{
		MAX_LOD_LEVEL = 3,		//	Maximum LOD level number
		BLOCK_GRID_DIM = 10,	//	A mesh block contains BLOCK_GRID_DIM x BLOCK_GRID_DIM water grids
	};

public:		//	Constructor and Destructor

	A3DWaterMeshBlock2();
	virtual ~A3DWaterMeshBlock2();

public:		//	Attributes

public:		//	Operations

	//	Get the highest mesh grid dim
	static int GetHighestMeshGridDim();

	//	Intialize object
	bool Init(A3DWaterArea2* pWaterArea, int left, int top);
	//	Release object
	void Release();

	//	Get visible grid number
	int GetVisGridNum() const { return m_iVisGridNum; }
	//	Get block area
	const ARectF& GetBlockArea() const { return m_rcBlkArea; }
	//	Get mesh aabb
	A3DAABB& GetMeshAABB() { return m_aabb; }
	//	Get index stream of specified LOD
	//	iLOD: [1, MAX_LOD_LEVEL]
	A3DStream* GetIndexStream(int iLOD);

protected:	//	Attributes

	A3DWaterArea2*	m_pWaterArea;	//	Water area that contains this block
	A3DAABB			m_aabb;			//	Block area's aabb
	ARectF			m_rcBlkArea;	//	Area covered by this block
	ARectI			m_rcBlkGrid;	//	Grid covered by this block
	int				m_iVisGridNum;	//	Number of visible grids in this block

	A3DStream*		m_aIdxStrms[MAX_LOD_LEVEL];		//	Index stream of each LOD level

protected:	//	Operations

	//	Initialize block data, return visible grid number
	bool InitData(int left, int top, int* aRows, int* aCols);
	//	Create stream
	bool CreateStreams(int* aRows, int* aCols);
	//	Add mesh indices of a grid
	void AddIndices(WORD* aIndices, int& iIdxCnt, int r, int c, int iStepSize);
};

//////////////////////////////////////////////////////////////////////////////////////////
//
//	 water area object
//
//////////////////////////////////////////////////////////////////////////////////////////

class A3DWaterArea2
{
public:

	//	Parameters that changed with time
	struct TIME_PARAM
	{
		A3DCOLOR	colWater;		//	water color
		float		fFogStart;
		float		fFogEnd;
		float		fFogDensity;
		float		fDarkFactor;	//	Darkness factor

		TIME_PARAM() :
		colWater(0xA0254F55),
			fFogStart(0.0f),
			fFogEnd(30.0f),
			fFogDensity(0.001f),
			fDarkFactor(0.2f)
		{
		}
	};

	friend class A3DTerrainWater2;

public:

	inline int GetWaveCount() { return m_theWaves.GetSize(); }
	inline A3DWaterWave2 * GetWave(int nIndex) { return m_theWaves[nIndex]; }

	int GetWaveVertCount();
	int GetWaveFaceCount();

	DWORD GetID() { return m_dwAreaID; }
	void SetID(DWORD dwID) { m_dwAreaID = dwID; }

	float GetWaterHeight() { return m_vWaterHeight; }
	void SetWaterHeight(float vWaterHeight) { m_vWaterHeight = vWaterHeight; }

	float GetGridSize() { return m_vGridSize; }
	void SetGridSize(float vGridSize) { m_vGridSize = vGridSize; }

	const A3DVECTOR3& GetCenter()	{ return m_vecCenter; }
	void SetCenter(const A3DVECTOR3& vecCenter)	{ m_vecCenter = vecCenter; }

	int GetWidth() const { return m_nWidth; }
	int GetHeight() const { return m_nHeight; }

	void SetGridFlag(int x, int y, BYTE flag);
	void ClearGridFlag(int x, int y, BYTE flag);
	inline BYTE GetGridFlag(int x, int y)
	{
		assert(x >= 0 && x < m_nWidth && y >= 0 && y < m_nHeight);
		return m_pGridFlags[y * m_nWidth + x];
	}

	//	Get time parameters
	//	bDay: true, get day paraemters; false, get night parameters
	TIME_PARAM* GetTimeParams(bool bDay);
	//	Get current time parameters
	A3DCOLOR GetCurWaterColor() const;
	float GetCurFogStart() const;
	float GetCurFogEnd() const;
	float GetCurFogDensity() const;
	float GetCurDarkFactor() const;
	//	Set/Get edge height
	float GetEdgeHeight() const	{ return m_fEdgeHeight; }
	void SetEdgeHeight(float fHeight) { m_fEdgeHeight = fHeight; }
	//	Set/Get normal map scroll speed for high quality water
	//	iIndex: normal map index. 0 or 1
	void GetNMScrollSpeed(int iIndex, float& fOffU, float& fOffV) const;
	void SetNMScrollSpeed(int iIndex, float fOffU, float fOffV);
	//	Set/Get normal map uv scale
	float GetNMScale() const { return m_fNMScale; }
	void SetNMScale(float fScale) { m_fNMScale = fScale; }
	//	Wave edge size
	float GetEdgeSize()	const { return m_vEdgeSize; }
	//	Wave edge alpha
	void SetWaveAlpha(float fAlpha) { m_fWaveAlpha = fAlpha; }
	float GetWaveAlpha() const { return m_fWaveAlpha; }

	const A3DAABB& GetAABB() const { return m_aabb; }
	const A3DAABB& GetVisGridAABB()	const { return m_aabbVisGrids; }

	A3DTerrainWater2 * GetTerrainWater() const	{ return m_pWater; }

    
public:

	A3DWaterArea2();
	virtual ~A3DWaterArea2();

	// initialize the water area
	bool Init(A3DTerrainWater2 * pWater, DWORD dwID, int nWidth, int nHeight, float vGridSize, const A3DVECTOR3& vecCenter, float vWaterHeight);

    bool Release();

	bool Load(A3DTerrainWater2 * pWater, AFile * pFileToLoad);
	bool Save(AFile * pFileToSave);

	// copy the vertices of this water area into a buffer, and set correct indices buffer
	//bool CopyWaveToBuffer(A3DWAVEVERTEX * pVerts, WORD * pIndices, int nVertStart);

	// update area vertex and indices according to new grid flags
	bool UpdateAll();

	// adjust the water area's width and height, after adjusting, all grid are invisible
	bool AdjustWidthHeight(int nWidth, int nHeight);
	// adjust the water area's center to a new center pos, without recalculate the waves' shape
	bool ShiftCenterTo(const A3DVECTOR3& vecCenter);

	// wave manipulations
	bool AddWave(float vEdgeSize, int nMaxSegment, A3DWaterWave2** ppWave);
	bool DeleteWave(int nIndex);
	bool RemoveAllWaves();

	//	Set minor water flag
	void SetMinorWaterFlag(bool bMinor) { m_bMinorWater = bMinor; }
	//	Get minor water flag
	bool GetMinorWaterFlag() { return m_bMinorWater; }

    bool IsOccluded(A3DViewport* pViewport) const;
private:

	A3DTerrainWater2* m_pWater;			// the water object pointer

	DWORD			m_dwAreaID;			// id of this water area

	TIME_PARAM		m_paramDay;			//	Time parameters in day
	TIME_PARAM		m_paramNight;		//	Time parameters at night
	float			m_fEdgeHeight;		//	Edge height
	float			m_vNMOffSpeed0[2];	//	Normal map offset for high quality water
	float			m_vNMOffSpeed1[2];	//	Normal map offset for high quality water
	float			m_fNMScale;			//	Normal map uv scale

	float			m_vWaterHeight;		//	water surface altitude;
	float			m_vEdgeSize;		//	wave edge size
	float			m_fWaveAlpha;		//	Wave edge alpha
	bool			m_bMinorWater;		//	true, this is minor water that will always use simple rendering

	int				m_nWidth;			// area dimention in grids
	int				m_nHeight;			// area dimention in grids
	float			m_vGridSize;		// each grid's dimention
	A3DVECTOR3		m_vecCenter;		// area center
	A3DAABB			m_aabb;				// area bounding box

	A3DAABB			m_aabbVisGrids;		// aabb built by grids that have visible flags
	bool			m_bDoRender;		// Do render flag updated when rendering

	BYTE *			m_pGridFlags;		// a 2D array to store the flag for each grid

	APtrArray<A3DWaterWave2*>		m_theWaves;
	APtrArray<A3DWaterMeshBlock2*>	m_aMeshBlks;
    A3DOcclusionProxy*          m_pOccProxy;        //Proxy for occlusion culling

protected:

	//	Build visible grids aabb
	bool BuildVisGridsAABB();
	//	Get all grids flags
	BYTE* GetGridFlags() { return m_pGridFlags; }
	//	Check if specified position is in a visible grid
	bool IsInVisbleGrid(const A3DVECTOR3& vecPos);
	//	Create static meshes
	bool CreateStaticMeshes();
	//	Draw static meshes
	bool DrawStaticMeshes(A3DViewport* pViewport, A3DHLSL* pHLSL, float fViewDist, bool bPPL);
    //  Create occlusion query object
    void CreateOccQuery();

};


#endif //_A3D_WATER_AREA_2_H_
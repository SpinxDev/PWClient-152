/*
* FILE: A3DWaterArea.h
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: liyi, 2011, 04, 12
*
* HISTORY:
*
* Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
*/

#ifndef _A3D_WATER_AREA_H_
#define _A3D_WATER_AREA_H_

#include "AArray.h"
#include "ABaseDef.h"
#include "A3DTypes.h"
#include "A3DGeometry.h"
#include "A3DVector.h"
#include "A3DWaterWave.h"
#include "ARect.h"

// class that represent a rectangle water area at a specified height
#define WATERAREA_VER					0x10003000
#define WATERGRID_VISIBLE				0x1
#define WATERGRID_DARK					0x2
#define WATERGRID_EDGE					0x4

//////////////////////////////////////////////////////////////////////////////////////////
//
//	 Types
//
//////////////////////////////////////////////////////////////////////////////////////////

#define A3DWATERVERT_FVF		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1

struct A3DWATERVERTEX
{
	A3DWATERVERTEX() {}
	A3DWATERVERTEX(const A3DVECTOR3& _pos, const A3DVECTOR3& _normal, float _tu, float _tv)
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

class A3DTerrainWater;
class A3DWaterArea;
class A3DStream;
class A3DMtlWaterBase;
class A3DViewport;
class A3DVertexShader;
class A3DCamera;

//////////////////////////////////////////////////////////////////////////////////////////
//
//	 A3DWaterMeshBlock
//
//////////////////////////////////////////////////////////////////////////////////////////

class A3DWaterMeshBlock
{
public:		//	Types

	enum
	{
		MAX_LOD_LEVEL = 3,		//	Maximum LOD level number
		BLOCK_GRID_DIM = 10,	//	A mesh block contains BLOCK_GRID_DIM x BLOCK_GRID_DIM water grids
	};

public:		//	Constructor and Destructor

	A3DWaterMeshBlock();
	virtual ~A3DWaterMeshBlock();

public:		//	Attributes

public:		//	Operations

	//	Get the highest mesh grid dim
	static int GetHighestMeshGridDim();

	//	Intialize object
	bool Init(A3DWaterArea* pWaterArea, int left, int top);
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

	A3DWaterArea*	m_pWaterArea;	//	Water area that contains this block
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

class A3DWaterArea
{
	friend class A3DTerrainWater;
	friend class A3DTerrainWaterRender;
	friend class A3DTerrainWaterSimpleAlphaRender;

public:
	inline int GetWaveCount() { return m_theWaves.GetSize(); }
	inline A3DWaterWave * GetWave(int nIndex) { return m_theWaves[nIndex]; }

	int GetWaveVertCount();
	int GetWaveFaceCount();

	inline DWORD GetID() { return m_dwAreaID; }
	inline void SetID(DWORD dwID) { m_dwAreaID = dwID; }

	inline float GetWaterHeight() { return m_vWaterHeight; }
	inline void SetWaterHeight(float vWaterHeight) { m_vWaterHeight = vWaterHeight; }

	inline float GetGridSize() { return m_vGridSize; }
	inline void SetGridSize(float vGridSize) { m_vGridSize = vGridSize; }

	inline const A3DVECTOR3& GetCenter()	{ return m_vecCenter; }
	inline void SetCenter(const A3DVECTOR3& vecCenter)	{ m_vecCenter = vecCenter; }

	inline int GetWidth() const { return m_nWidth; }
	inline int GetHeight() const { return m_nHeight; }

	void SetGridFlag(int x, int y, BYTE flag);
	void ClearGridFlag(int x, int y, BYTE flag);
	inline BYTE GetGridFlag(int x, int y)
	{
		assert(x >= 0 && x < m_nWidth && y >= 0 && y < m_nHeight);
		return m_pGridFlags[y * m_nWidth + x];
	}

	inline A3DCOLOR GetWaterColor()				{ return m_waterColor; }
	inline void SetWaterColor(A3DCOLOR color)	{ m_waterColor = color; }

	inline float GetEdgeSize()					{ return m_vEdgeSize; }
	inline const A3DAABB& GetAABB()				{ return m_aabb; }
	inline const A3DAABB& GetVisGridAABB()		{ return m_aabbVisGrids; }

	inline A3DTerrainWater * GetTerrainWater()	{ return m_pWater; }
	bool IsUnderWater(const A3DVECTOR3& vecPos);


public:

	A3DWaterArea();
	virtual ~A3DWaterArea();

	// initialize the water area
	bool Init(A3DTerrainWater * pWater, DWORD dwID, int nWidth, int nHeight, float vGridSize, const A3DVECTOR3& vecCenter, float vWaterHeight);
	bool Release();

	bool Load(A3DTerrainWater * pWater, AFile * pFileToLoad);
	bool Save(AFile * pFileToSave);

	// copy the vertices of this water area into a buffer, and set correct indices buffer
	bool CopyWaveToBuffer(A3DWAVEVERTEX * pVerts, WORD * pIndices, int nVertStart);

	// update area vertex and indices according to new grid flags
	bool UpdateAll();

	// adjust the water area's width and height, after adjusting, all grid are invisible
	bool AdjustWidthHeight(int nWidth, int nHeight);
	// adjust the water area's center to a new center pos, without recalculate the waves' shape
	bool ShiftCenterTo(const A3DVECTOR3& vecCenter);

	// wave manipulations
	bool AddWave(float vEdgeSize, int nMaxSegment, A3DWaterWave** ppWave);
	bool DeleteWave(int nIndex);
	bool RemoveAllWaves();

	//	Set minor water flag
	void SetMinorWaterFlag(bool bMinor) { m_bMinorWater = bMinor; }
	//	Get minor water flag
	bool GetMinorWaterFlag() { return m_bMinorWater; }
	//	Draw static meshes
	bool DrawStaticMeshes(A3DViewport* pViewport,  float fViewDist, A3DCamera* pReflectCamera);

private:

	A3DTerrainWater * m_pWater;			// the water object pointer

	DWORD			m_dwAreaID;			// id of this water area
	A3DCOLOR		m_waterColor;		// water color

	float			m_vWaterHeight;		// water surface altitude;
	float			m_vEdgeSize;		// wave edge size
	bool			m_bMinorWater;		//	true, this is minor water that will always use simple rendering

	int				m_nWidth;			// area dimention in grids
	int				m_nHeight;			// area dimention in grids
	float			m_vGridSize;		// each grid's dimention
	A3DVECTOR3		m_vecCenter;		// area center
	A3DAABB			m_aabb;				// area bounding box

	A3DAABB			m_aabbVisGrids;		// aabb built by grids that have visible flags
	bool			m_bDoRender;		// Do render flag updated when rendering

	BYTE *			m_pGridFlags;		// a 2D array to store the flag for each grid

	APtrArray<A3DWaterWave*>		m_theWaves;
	APtrArray<A3DWaterMeshBlock*>	m_aMeshBlks;

protected:

	//	Build visible grids aabb
	bool BuildVisGridsAABB();
	//	Get all grids flags
	BYTE* GetGridFlags() { return m_pGridFlags; }
	//	Check if specified position is in a visible grid
	bool IsInVisbleGrid(const A3DVECTOR3& vecPos);
	//	Create static meshes
	bool CreateStaticMeshes();

};

#endif //_A3D_WATER_AREA_H_
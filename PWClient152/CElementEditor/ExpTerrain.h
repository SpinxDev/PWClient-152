/*
 * FILE: ExpTerrain.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "Terrain.h"
#include "ARect.h"
#include "AArray.h"

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

class CELBitmap;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CExpTerrain
//	
///////////////////////////////////////////////////////////////////////////

class CExpTerrain : public CTerrain
{
public:		//	Types

	//	Some constants
	enum
	{
		BLOCK_GRID_SHIFT	= 6,
		BLOCK_GRID			= 1 << BLOCK_GRID_SHIFT,
	};

	//	Mask map data of each layer
	struct MASKMAPDATA
	{
		int		iLayer;		//	Layer index
		int		iWidth;		//	Map size in pixels
		int		iHeight;
		int		iPitch;		//	Map pitch
		BYTE*	pData;		//	Point to the first pixel
	};

	//	Height information of a mask map area
	struct HEIGHTINFO
	{
		int		iWidth;		//	Block size in vertex
		int		iHeight;
		int		iPitch;		//	Data pitch
		void*	pData;		//	Point to the first height data
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

	//	Block used to accelerate ray trace
	struct BLOCK
	{
		ARectI	rcGrid;			//	Block area in grids
		A3DAABB	aabb;			//	Block's aabb
		bool	bCollision;		//	Collision flag of ray and block
		float	fRayMinY;		//	Minimum y of ray in block
	};

public:		//	Constructor and Destructor

	CExpTerrain(CElementMap* pMap);
	virtual ~CExpTerrain();

public:		//	Attributes

public:		//	Operations

	//	Release object
	virtual void Release();

	//	Load data from file
	bool Load(CELArchive& ar, int iLoadFlag)
	{
		m_iLoadFlag = iLoadFlag;
		return Load(ar);
	}

	//	Ray trace function
	virtual bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, float fTime, RAYTRACERT* pTraceRt);
	//	Get height of specified position
	virtual float GetPosHeight(const A3DVECTOR3& vPos);
	//	Get normal of specified position
	virtual bool GetPosNormal(const A3DVECTOR3& vPos, A3DVECTOR3& vNormal);
	//	Set map offset
	virtual void SetMapOffset(float x, float z);

	//	Get mask area information
	MASKMAPDATA* GetMaskAreaInfo(const ARectI& rcMap);
	//	Release mask area information
	void ReleaseMaskAreaInfo(MASKMAPDATA* aMaskData);
	//	Check whether a mask map is pure black
	bool IsPureBlackMask(const MASKMAPDATA& MaskData);
	//	Get the first pure white mask layer
	int GetFirstPureWriteLayer(const MASKMAPDATA* aMaskData);
	//	Get height data of specified terrain area
	bool GetHeightData(const ARectI& rcMap, HEIGHTINFO& Data);
	//	Get normal data of specified terrain area
	bool GetNormalData(const ARectI& rcMap, HEIGHTINFO& Data);
	//	Set load flag
	void SetLoadFlag(int iFlag) { m_iLoadFlag = iFlag; }
	//	Get load flag
	int GetLoadFlag() { return m_iLoadFlag; }


	//	Get mask map bitmap
	CELBitmap* GetMaskMap(int n) { return m_aMaskBmps[n]; }

protected:	//	Attributes

	APtrArray<CELBitmap*>	m_aMaskBmps;	//	Mask bitmaps
	AArray<BLOCK, BLOCK&>	m_aBlocks;		//	Blocks
	

	TWODDDA		m_Grid2DDA;			//	Grid 2DDA information
	RAYTRACE	m_RayTraceInfo;		//	Ray trace information
	int			m_iLoadFlag;		//	Load flag
	int			m_iTerrainBlock;	//	Terrain has m_iTerrainBlock * m_iTerrainBlock blocks

protected:	//	Operations

	//	Load data
	virtual bool Load(CELArchive& ar);
	//	Save data
	virtual bool Save(CELArchive& ar) { return true; }

	//	Load height map for exporting scene (when m_iLoadFlag == LOAD_EXPSCENE)
	bool ExpScene_LoadHeighMap(CELArchive& ar, const TERRAINFILEDATA& Data);
	//	Create blocks
	bool CreateBlocks();
	//	Initialize block data
	bool InitBlock(BLOCK& Block, int r, int c);
	//	Initiailze grid 2DDA information
	bool InitGrid2DDA();
	//	Ray trace in whole terrain
	bool RayTraceInTerrain();
	//	Ray trace in grid
	bool RayTraceInGrid(int r, int c);
	//	Initialize block information before ray trace
	bool InitBlockBeforeRayTrace();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


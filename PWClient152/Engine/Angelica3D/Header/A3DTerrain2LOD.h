/*
 * FILE: A3DTerrain2LOD.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DTERRAIN2LOD_H_
#define _A3DTERRAIN2LOD_H_

#include "ABaseDef.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2LOD
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2LOD
{
public:		//	Types

	//	Some constants
	enum
	{
		NUM_LODLEVEL = 3,
	};

	//	Direction
	enum
	{
		D_LEFT = 0,
		D_TOP,
		D_RIGHT,
		D_BOTTOM,
		D_CENTER,
	};

	//	Index stream
	struct INDEXSTRM
	{
		WORD*	aIndices;			//	Original indices
		WORD*	aMappedIndices;		//	Mapped indices
		int		iNumIndex;			//	Number of index
	};

	//	LOD level structure
	struct LODLEVEL
	{
		INDEXSTRM	aOrigin[5];		//	No descend indices
		INDEXSTRM	aDescend1[5];	//	Descend 1 level indices
		INDEXSTRM	aDescend2[5];	//	Descend 2 levels indices
		int			iNumVert;		//	Vertex number
	};

	//	Vertex indices in a terrain grid 
	struct GRID
	{
		int		v0, v1, v2;		//	Face 1
		int		v3, v4, v5;		//	Face 2
	};

	friend class A3DTerrain2Env;

public:		//	Constructor and Destructor

	A3DTerrain2LOD();
	virtual ~A3DTerrain2LOD();

public:		//	Attributes

public:		//	Operations

	//	Intialize object
	bool Init(int iBlockGrid);
	//	Release object
	void Release();

	//	Get index stream
	WORD* GetIndexStream(int iLevel, int iDir, int iDescend, int* piNumIdx, bool bMapped);
	//	Get index maps
	WORD* GetIndexMaps() { return m_aIndexMaps; }
	//	Map a index
	WORD MapIndex(WORD wIndex) { ASSERT(wIndex < m_iVertPitch * m_iVertPitch); return m_aIndexMaps[wIndex]; }
	//	Get vertex number of each level
	int GetLevelVertexNum(int iLevel) { return m_aLevels[iLevel-1].iNumVert; }
	//	Get grids
	GRID* GetGrids() { return m_aGrids; }
	//	Get block grids
	int GetBlockGrid() { return m_iBlockGrid; }

protected:	//	Attributes

	int			m_iBlockGrid;				//	Each block has m_iBlockGrid * m_iBlockGrid grids
	int			m_iVertPitch;				//	Vertex pitch
	LODLEVEL	m_aLevels[NUM_LODLEVEL];	//	Indices of each level
	WORD*		m_aIndexMaps;				//	Index map
	GRID*		m_aGrids;					//	Grid of a block
	int			m_iRefCnt;					//	Reference counter

protected:	//	Operations

	//	Create center indices for specified LOD level
	bool CreateCenterIndices(int iLevel);
	//	Create left boder indices for specified LOD level
	bool CreateLeftIndices(int iLevel);
	//	Create top border indices for specified LOD level
	bool CreateTopIndices(int iLevel);
	//	Create right border indices for specified LOD level
	bool CreateRightIndices(int iLevel);
	//	Create bottom border indices for specified LOD level
	bool CreateBottomIndices(int iLevel);

	//	Create index maps
	bool CreateIndexMaps();
	//	Create mapped indices
	WORD* CreateMappedIndices(const WORD* aOrigin, int iNumIndex);
	//	Create block grids
	bool CreateGrids();

	//	Increase reference counter
	int IncRefCnt() { return ++m_iRefCnt; }
	//	Decrease reference counter
	int DecRefCnt() { return --m_iRefCnt; }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DTERRAIN2LOD_H_

/*
 * FILE: TerrainSquare.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/7/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#ifndef _TERRAINSQUARE_H_
#define _TERRAINSQUARE_H_

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

class A3DTerrain2;
class A3DTerrain2Block;
class CTerrainSquare;
class CProgressCtrl;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CBlockSquare
//	
///////////////////////////////////////////////////////////////////////////

class CBlockSquare
{
public:		//	Types

	//	Edge index
	enum
	{
		EDGE_LEFT = 0,
		EDGE_TOP,
		EDGE_RIGHT,
		EDGE_BOTTOM,
	};

	//	Least square sum
	struct SQUARESUM
	{
		float x, y;
		float xx, xy;
		float samples;

		void Clear()
		{
			x = 0;
			y = 0;
			xx = 0;
			xy = 0;
			samples = 0;
		}
	};

	//	Least square line y = ax + c
	struct LSLINE
	{
		float	a;
		float	c;
		float	e;		//	Minimum line error
	};

	//	Block edges
	struct EDGE
	{
		A3DVECTOR3 v1, v2;
	};

public:		//	Constructor and Destructor

	CBlockSquare(CTerrainSquare* pTrnSquare);
	virtual ~CBlockSquare();

public:		//	Attributes

public:		//	Operations

	//	Build block square from terrain2 block data
	bool Build(A3DTerrain2Block* pBlock);
	//	Save square data to file
	bool Save(FILE* fp);

	//	Get edges
	const EDGE* GetEdges() { return m_aEdges; }

protected:	//	Attributes

	CTerrainSquare*		m_pTrnSquare;	//	Terrain square object
	A3DTerrain2Block*	m_pBlock;		//	Terrain block object
	A3DVECTOR3			m_vBlkOff;		//	Block offset

	SQUARESUM	m_Sum;			//	Least square sum
	LSLINE		m_Line;			//	Least square line
	EDGE		m_aEdges[4];	//	Block edges

protected:	//	Operations

	//	Calculate least square sum
	void CalcLeastSquareSum(int iEdge);
	//	Calculate lease square line
	void CalcLeastSquareLine(int iEdge);
	//	Calculate edge
	void CalcEdge(int iEdge);
	//	Calculate offset between block's center to origin
	void CalcBlockOffset();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CTerrainSquare
//	
///////////////////////////////////////////////////////////////////////////

class CTerrainSquare
{
public:		//	Types

public:		//	Constructor and Destructor

	CTerrainSquare();
	virtual ~CTerrainSquare();

public:		//	Attributes

public:		//	Operations

	//	Build terrain squares from terrain2 file
	bool Build(const char* szFile, CProgressCtrl* pProgress=NULL);
	//	Save square data to file
	bool Save(const char* szFile);
	//	Release object
	void Release();

protected:	//	Attributes

	A3DTerrain2*	m_pTerrain;		//	Terrain object
	CProgressCtrl*	m_pProgress;	//	Progress bar
	float			m_fStepSize;

	APtrArray<CBlockSquare*>	m_aBlkSquares;

protected:	//	Operations

	//	Load terrain
	bool LoadTerrain(const char* szFile);
	//	Reset block squares
	void ResetBlockSquares();
	//	Build block squares in current active area
	bool BuildBlockSquares();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_TERRAINSQUARE_H_

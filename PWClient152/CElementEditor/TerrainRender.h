/*
 * FILE: TerrainRender.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DTerrain2ForEditor.h"
#include "ARect.h"

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

class CTerrain;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CTerrainRender
//	
///////////////////////////////////////////////////////////////////////////

class CTerrainRender : public A3DTerrain2ForEditor
{
public:		//	Types

public:		//	Constructor and Destructor

	CTerrainRender(CTerrain* pTerrain);
	virtual ~CTerrainRender();

public:		//	Attributes

public:		//	Operations

	//	Reset terrain information
	void UpdateTerrainInfo();
	//	Update terrain height and normal of specified area
	bool UpdateTerrainHeight(const ARectF& rcArea, const float* aHeiData, const A3DVECTOR3* aNormals, int iHMSize);
	//	Reset all blocks' LOD scale
	void ResetLODScales();

	//	Get specified block
	A3DTerrain2Block* GetBlock(int r, int c);

protected:	//	Attributes

	CTerrain*	m_pTerrain;

protected:	//	Operations

	//	Update height of a block
	bool UpdateBlockHeight(A3DTerrain2Block* pBlock, const float* aHeiData, const A3DVECTOR3* aNormals, int iHMSize);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


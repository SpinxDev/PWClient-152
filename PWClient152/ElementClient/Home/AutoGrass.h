/*
 * FILE: AutoGrass.h
 *
 * DESCRIPTION: Class for automaticlly generating grass
 *
 * CREATED BY: Jiang Dalong, 2005/01/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOGRASS_H_
#define _AUTOGRASS_H_

#include "AutoHomeCommon.h"
#include <AArray.h>

const float GRASS_DENSITY			= 20;
const float BELOW_WATER_HEIGHT		= 3.0f;

class CAutoHome;
class CELGrassLand;
class CBitChunk;

class CAutoGrass
{
public:
	CAutoGrass(CAutoHome* pAutoHome);
	virtual ~CAutoGrass();
public:
	struct VERTEX
	{
		int x;
		int y;
	};
	struct PLANTEDGRASSID 
	{
		DWORD	dwTypeID;
		DWORD	dwGrassMapID;
	};

public:
	// Init parameters
	bool Init(CELGrassLand* pGrassLand);
	// Add grass
	bool AddGrassAt(int nTypeIndex, int nBlockIndex, int x, int y);
	// Save grass
	bool SaveGrassToFile(const char* szFile);
	// Release
	void Release();
	// Translate from whole grass map coordinate to height map coordinate
	bool GrassMapToHeightMap(int nInX, int nInY, int* pOutX, int* pOutY);
	// Translate from height map coordinate to whole grass map coordinate
	bool HeightMapToGrassMap(int nInX, int nInY, int* pOutX, int* pOutY);
	// Load grass
	bool Load(const char* szFile);
	// Tick
	bool Tick(DWORD dwTime);
	// Render
	bool Render(A3DViewport* pViewport, float fDayWeight);
	// Delete all grass types
	void DeleteAllGrassTypes();

	// Add grass type
	bool AddGrassType(DWORD dwID);
	// Get added grass index by ID
	int GetAddedGrassIndexById(DWORD dwID);
	// Plant one grass
	bool PlantGrass(int x, int y, int nNumGrassId, DWORD* pGrassIds, float* pGrassWeights);
	// Update grass types and maps to engine
	bool UpdateGrassMaps();
	// Remove all grass
	bool RemoveAllGrass();

	inline CELGrassLand* GetGrassland() { return m_pGrassLand; }

protected:
	CAutoHome*		m_pAutoHome;						// Home
	CELGrassLand*	m_pGrassLand;						// Grass land
	float			m_fLandGrassDensity;				// Grass density
	float			m_fWaterGrassDensity;

	AArray<PLANTIDINDEXINFO, PLANTIDINDEXINFO&>	m_aGrassAddedTypeIndex;// Added grass type index
	AArray<CBitChunk*, CBitChunk*>		m_aBitGrassMap;				// Grass map
	AArray<bool*, bool*>				m_aHasGrassInMap;			// Grass map has grass?
	AArray<PLANTEDGRASSID, PLANTEDGRASSID&>	m_aPlantedGrassID;		// Planted tree ID

protected:
	// Convert grass map to grass block coordinate
	bool GrassMapToGrassBlock(int nInX, int nInY, int* pIndex, int* pOutX, int* pOutY);
	// Convert grass map to height map mask coordinate
	bool GrassMapToMaskMap(int nInX, int nInY, int* pOutX, int* pOutY);
	// Can grass be planted at the position?
	bool IsGrassPosValid(int nIndex, int x, int y);
	// Release grass map
	void ReleaseGrassMap();
	// Get a random grass type
	int RandGrassType(int nNumType, DWORD* pTreeIDs, float* pWeights);
};

#endif // #ifndef _AUTOGRASS_H_

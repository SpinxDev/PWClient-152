/*
 * FILE: AutoForest.h
 *
 * DESCRIPTION: Class for automaticlly generating forest
 *
 * CREATED BY: Jiang Dalong, 2005/02/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOFOREST_H
#define _AUTOFOREST_H

#include "AutoHomeCommon.h"
#include <AArray.h>
#include <ABaseDef.h>
#include <A3DVector.h>

class CAutoHome;
class CELForest;
class A3DViewport;

const int	TREE_DENSITY	= 50;		// Tree density
const float TREE_INTERVAL	= 5.0f;		// Min distance between two trees

class CAutoForest  
{
public:
	// Tree type
	enum TERRAINTREETYPE
	{
		TTT_NEVER_TREE		= 0,	// Can't have tree
		TTT_NO_TREE,				// No tree here
		TTT_HAVE_TREE,				// Have tree
	};

	struct PLANTEDTREEID 
	{
		DWORD	dwTypeID;
		DWORD	dwTreeID;
	};

public:
	CAutoForest(CAutoHome* pAutoHome);
	virtual ~CAutoForest();

public:
	// Save tree
	bool SaveTreeToFile(const char* szFile);
	// Initialize forest
	bool Init(CELForest* pForest);
	// Release forest
	void Release();
	// Load forest
	bool Load(const char* szFile);
	// Tick
	bool Tick(DWORD dwTime);
	// Render
	bool Render(A3DViewport* pViewport);
	
	// Add tree type
	bool AddTreeType(DWORD dwID);
	// Plant one tree
	bool PlantTree(int x, int z, int nNumType, DWORD* pTreeIDs, float* pWeights);
	// Get tree type number
	int GetTreeTypeNumber();
	// Get added tree index by ID
	int GetAddedTreeIndexById(DWORD dwID);
	// Remove all tree
	bool RemoveAllTree();

	inline void SetForestReady(bool bReady) { m_bForestReady = bReady; }
	inline bool GetForestReady() { return m_bForestReady; }
	inline float GetDensity() { return m_fDensity; }
	inline float GetMaxSlope() { return m_fMaxSlope; }
	inline DWORD GetSeed() { return m_dwSeed; }
	inline CELForest* GetForest() { return m_pForest; }

protected:
	CAutoHome*			m_pAutoHome;					// Point of auto home
	CELForest*			m_pForest;						// EL forest

	TERRAINTREETYPE*	m_pTerrainForest;				// Terrain forest

	bool				m_bForestReady;					// Forest ready?
	float				m_fDensity;						// Forest density
	float				m_fMaxSlope;					// Plant no tree if slope > this value
	DWORD				m_dwSeed;						// Random seed

	AArray<PLANTIDINDEXINFO, PLANTIDINDEXINFO&>	m_aTreeAddedTypeIndex;// Tree type index
	AArray<PLANTEDTREEID, PLANTEDTREEID&>	m_aPlantedTreeID;		// Planted tree ID

protected:
	// Check the tree position is valid or not
	bool IsTreePosValid(TERRAINTREETYPE* pArea, int x, int z, int nInterval);
	// Get a random tree type
	int RandTreeType(int nNumType, DWORD* pTreeIDs, float* pWeights);
	// A kind of tree type already exists?
	bool IsTreeTypeExist(DWORD dwID);
	// Add a tree
	bool AddTree(DWORD dwID, const A3DVECTOR3& vPos);
};

#endif // #ifndef _AUTOFOREST_H

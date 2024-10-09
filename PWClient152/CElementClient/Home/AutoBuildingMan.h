/*
 * FILE: AutoBuildingMan.h
 *
 * DESCRIPTION: Class for building operation
 *
 * CREATED BY: Jiang Dalong, 2006/06/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOBUILDINGMAN_H_
#define _AUTOBUILDINGMAN_H_

#include "AutoBuilding.h"
#include "AutoSceneFunc.h"

enum BUILDINGOPERA
{
	BP_NONE		= 0,
	BP_ADD,
	BP_TRANSLATE,
	BP_ROTATE,
	BP_SCALE,
	BP_REPOS,
};

class CAutoHome;

class CAutoBuildingMan  
{
public:
	struct COLLISIONNODE
	{
		int nCount;
		int* pCollisionIndex;
	};
public:
	CAutoBuildingMan(CAutoHome* pAutoHome);
	virtual ~CAutoBuildingMan();

	void Release();
	bool DoRayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, bool* pAdd = NULL);
	bool Render();
	bool Tick(DWORD dwTickTime);

	// Delete building
	bool DeleteBuilding(int nIndex);
	// Delete all buildings
	bool DeleteAllBuildings();
	// Translate a building
	bool TranslateBuilding(int nIndex, const A3DVECTOR3& vTrans);
	// Translate current selected building
	bool TranslateCurBuilding(const A3DVECTOR3& vTrans);
	// rotate building by itself
	bool RotateBuildingSelf(int nIndex, float fRotateY);
	// Rotate Current selected building by itself
	bool RotateCurBuildingSelf(float fRotateY);
	// Get building world position
	bool GetBuildingPos(int nIndex, A3DVECTOR3& vPosWorld);
	// Get building count
	int GetNumBuildings();
	// Get a building status
	bool GetBuildingStatus(int nIndex, MODELSTATUS& status);
	// Adjust models after terrain change
	bool AdjustModelsAfterTerrainChange();
	
	inline int GetBuildingOpera()				{ return m_nBuildingOperation; }
	inline void SetBuildingOpera(int nOpera)	{ m_nBuildingOperation = nOpera; }
	inline void SetCurModelID(DWORD dwID)		{ m_dwCurModelID = dwID; }
	inline void SetSelectedModelIndex(int nIndex) { m_nSelectedModelIndex = nIndex; }
	inline int GetSelectedModelIndex()			{ return m_nSelectedModelIndex; }
	inline bool GetModelFailed()				{ return m_bAddModelFailed; }
	inline void SetModelFailed(bool bFailed)	{ m_bAddModelFailed = bFailed; }
	inline bool GetBuildingMoved()				{ return m_bModelMoved; }
	inline void SetBuildingMoved(bool bMoved)	{ m_bModelMoved = bMoved; }
	inline void SetDrawSelectedOBB(bool bDraw)	{ m_bDrawSelectedOBB = bDraw; }

protected:
	CAutoHome*				m_pAutoHome;
	CAutoBuilding*			m_pAutoBuilding;

	int						m_nBuildingOperation;	// Building operation type
	DWORD					m_dwCurModelID;			// Current model ID
	int						m_nSelectedModelIndex;	// Selected model index
	bool					m_bAddModelFailed;		// Add model failed
	bool					m_bModelMoved;			// Model moved
	bool					m_bDrawSelectedOBB;		// Draw selected model obb
	
protected:
	// Try to add a model to scene
	bool AddModelToScene(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd);
	// Select model in scene
	bool SelectModel(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd);
	// Move a model to a position
	bool SetBuildingPosByRayTrace(int nIndex, const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd);
	// Get a up or down face vertices of a OBB, nUpDown=1: up, -1:down
	bool GetFaceVerticesOfOBB(A3DOBB& obb, float* fCenter, POINT_FLOAT* ptFace, int nUpDown);
};

#endif // #ifndef _AUTOBUILDINGMAN_H_


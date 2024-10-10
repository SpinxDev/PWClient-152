/*
 * FILE: AutoHomeMan.h
 *
 * DESCRIPTION: Interface class for managing a home area, include terrain, grass, forest, water...
 *
 * CREATED BY: Jiang Dalong, 2006/06/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOHOMEMAN_H_
#define _AUTOHOMEMAN_H_

#include "AutoSceneFunc.h"
#include "AutoHomeAction.h"
#include <ABaseDef.h>
#include <Arect.h>

class CAutoScene;
class CAutoSceneConfig;
class CAutoHome;
class CAutoSelectedArea;
class CAutoBuildingMan;
class CAutoParameters;
class A3DViewport;
class A3DVECTOR3;

enum SELECTEDAREATYPE
{
	AT_NONE,
	AT_POLY,
	AT_LINE,
	AT_VIEW,
};

class CAutoHomeMan
{
public:
	CAutoHomeMan(CAutoScene* pAutoScene);
	virtual ~CAutoHomeMan();

	/****************************************************
	Common functions. 
	*****************************************************/
	bool Init();
	void Release();
	void SetHome(CAutoHome* pAutoHome);
	bool SetHome(int r, int c);
	bool LoadHome();
	bool SaveHome();
	bool IsReady();
	bool Tick(DWORD dwTickTime);
	bool Render(A3DViewport* pA3DViewport);
	// Is a position in camera move rect
	bool IsInCameraRect(const A3DVECTOR3& vPos);

	/****************************************************
	Home managing functions. 
	*****************************************************/
	// Create base terrain
	bool CreateBaseTerrain(float fAmplitude, int nFrequency, float fPersistence, 
					 float fTextureRatio, float fMainTexture, 
					 float fPartTexture, float fPartDivision, DWORD dwRandSeed);
	// Create hill poly
	bool CreateHillPoly(float fMaxHeight, float fMainSmooth, float fPartSmooth,
					DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, bool bHillUp, float fTextureRatio, float fMainTexture, float fPatchTexture, 
					float fPatchDivision, float fBridgeScope, float fBridgeDepth, int nNumPoints, POINT_FLOAT* pPoints, float fBorderErode);
	// Create hill line
	bool CreateHillLine(float fMaxHeight, float fHillWidth, float fMainSmooth, float fPartSmooth,
					DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, int nNumCurveLinePoint, float* pCurveLinePoints, bool bHillUp, int nNumPoints, POINT_FLOAT* pPoints,
					float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, float fBridgeScope, float fBorderErode);
	// Create part texture
	bool CreatePartTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, 
					DWORD dwSeed, float fBorderScope, int nNumPoints, POINT_FLOAT* pPoints, int nTextureType);
	// Create water
	bool CreateWater(float fWaterHeight, float fSandWidth, float fWaveSize);
	// Create road
	bool CreateRoad(float fRoadWidth, float fRoadHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, bool bFlatRoad);
	// Create terrain flat
	bool CreateTerrainFlat(int nNumPoints, POINT_FLOAT* pPoints);
	// Create polygon forest
	bool CreateForestPoly(int nPlantType, float fDensityX, float fDensityZ, float fMaxSlope,
				int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints);
	// Create linear forest
	bool CreateForestLine(float fSpace, float fSpaceNoise, float fMaxSlope,
				int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints);
	// Create grass poly
	bool CreateGrassPoly(int nGrassKind, float fDensity,
				int nGrassTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints);

	// Change hill poly
	bool ChangeHillPoly(float fMaxHeight, float fMainSmooth, float fPartSmooth,
					DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, bool bHillUp, float fTextureRatio, float fMainTexture, float fPatchTexture, 
					float fPatchDivision, float fBridgeScope, float fBridgeDepth, int nNumPoints, POINT_FLOAT* pPoints, float fBorderErode, int nIndex);
	// Change hill line
	bool ChangeHillLine(float fMaxHeight, float fHillWidth, float fMainSmooth, float fPartSmooth,
				DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, int nNumCurveLinePoint, float* pCurveLinePoints, bool bHillUp, int nNumPoints, POINT_FLOAT* pPoints,
				float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, float fBridgeScope, float fBorderErode, int nIndex);
	// Change part texture
	bool ChangePartTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, 
							  DWORD dwSeed, float fBorderScope, int nNumPoints, POINT_FLOAT* pPoints, int nTextureType, int nIndex);
	// Change road
	bool ChangeRoad(float fRoadWidth, float fRoadHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nFlatRoad, int nIndex);
	// Change polygon forest
	bool ChangeForestPoly(int nPlantType, float fDensityX, float fDensityZ, float fMaxSlope,
				int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex);
	// Change linear forest
	bool ChangeForestLine(float fSpace, float fSpaceNoise, float fMaxSlope,
				int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex);
	// Change grass poly
	bool ChangeGrassPoly(int nGrassKind, float fDensity,
				int nGrassTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex);
	// Change terrain flat
	bool ChangeTerrainFlat(int nNumPoints, POINT_FLOAT* pPoints, int nIndex);
	
	// Change main terrain texutre
	bool ChangeMainTerrainTexture(DWORD dwMainID, DWORD dwPatchID);
	// Change hill texutre
	bool ChangeHillTexture(DWORD dwMainID, DWORD dwPatchID);
	// Change part texutre
	bool ChangePartTexture(DWORD dwMainID, DWORD dwPatchID);
	// Change water texutre
	bool ChangeWaterTexture(DWORD dwMainID, DWORD dwPatchID);
	// Change road texutre
	bool ChangeRoadTexture(DWORD dwMainID, DWORD dwPatchID);

	// Undo action
	bool UndoAction(int nActionType);
	// Redo action
	bool RedoAction(int nActionType);
	// Delete action
	bool DeleteAction(int nActionType, int nIndex);
	// Get action
	void* GetAction(int nActionType, int nIndex);
	// Edit action
	bool EditAction(int nActionType, int nIndex);
	// Update height action
	bool UpdateHeightActions();
	// Set terrain up and down pri
	bool SetUpDownPri(int nPri);

	/*****************************************************
	Home selecting functions. 
	******************************************************/
	// Select action
	bool SelectAction(int nActionType, int nIndex);
	// Select last action
	bool SelectLastAction(int nActionType);

	/*****************************************************
	Home retrieving functions. 
	******************************************************/
	// Get auto scene config
	CAutoSceneConfig* GetAutoSceneConfig();
	// Get texture ID
	DWORD GetTextureID(int nLayer);
	// Get action number
	int GetActionNumber(int nActionType);

	inline CAutoBuildingMan* GetAutoBuildingMan()		{ return m_pAutoBuildingMan; }
	inline CAutoSelectedArea*	GetAutoSelectedArea()	{ return m_pAutoSelectedArea; }
	inline CAutoScene*		GetAutoScene()	{ return m_pAutoScene; }
	inline CAutoHome* GetAutoHome()			{ return m_pAutoHome; }
	inline CAutoParameters* GetAutoParameters()			{ return m_pAutoParameters; }
	inline ARectF& GetCameraMoveRect()		{ return m_rcCameraMoveRect; }
	inline bool	IsEdited()					{ return m_bEdited; }
	inline void SetEdited(bool bEdited)		{ m_bEdited = bEdited; }

protected:
	CAutoScene*				m_pAutoScene;
	CAutoHome*				m_pAutoHome;
	CAutoSelectedArea*		m_pAutoSelectedArea;
	CAutoBuildingMan*		m_pAutoBuildingMan;
	CAutoParameters*		m_pAutoParameters;

	ARectF					m_rcCameraMoveRect;

	bool					m_bEdited;				// Home edited?

protected:
	// Draw border of home
	bool DrawHomeBorder();
	// Set default camera move rect
	void SetDefaultCameraMoveRect();
};

#endif // #ifndef _AUTOHOMEMAN_H_


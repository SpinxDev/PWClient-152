/*
 * FILE: AutoHomeMan.cpp
 *
 * DESCRIPTION: Class for managing a home area, include terrain, grass, forest, water...
 *
 * CREATED BY: Jiang Dalong, 2006/06/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "AutoHomeMan.h"
#include "AutoScene.h"
#include "AutoHome.h"
#include "AutoTerrain.h"
#include "AutoTexture.h"
#include "AutoHillPoly.h"
#include "AutoHillLine.h"
#include "AutoPartTexture.h"
#include "AutoWater.h"
#include "AutoRoad.h"
#include "AutoForest.h"
#include "AutoGrass.h"
#include "AutoTerrainFlat.h"
#include "AutoForestPoly.h"
#include "AutoForestLine.h"
#include "AutoGrassPoly.h"
#include "AutoHomeAction.h"
#include "AutoSelectedArea.h"
#include "AutoBuildingMan.h"
#include "Render.h"
#include "AutoHomeBorder.h"
#include "AutoParameters.h"
#include <A3DWireCollector.h>
#include <A3DEngine.h>
#include <A3DCamera.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoHomeMan::CAutoHomeMan(CAutoScene* pAutoScene)
{
	m_pAutoScene		= pAutoScene;
	m_pAutoHome			= NULL;
	m_pAutoSelectedArea	= NULL;
	m_pAutoBuildingMan	= NULL;
	m_pAutoParameters	= NULL;
	m_bEdited			= false;

	SetDefaultCameraMoveRect();
}

CAutoHomeMan::~CAutoHomeMan()
{
	Release();
}

void CAutoHomeMan::Release()
{
	A3DRELEASE(m_pAutoSelectedArea);
	A3DRELEASE(m_pAutoBuildingMan);
	A3DRELEASE(m_pAutoParameters);
}

void CAutoHomeMan::SetHome(CAutoHome* pAutoHome)
{
	m_pAutoHome = pAutoHome;

	// Set camera move rect
	if (NULL == m_pAutoHome)
	{
		SetDefaultCameraMoveRect();
	}
	else
	{
		float fExpand = m_pAutoScene->GetActiveRadius();
		ARectF rcTerrain = m_pAutoHome->GetAutoTerrain()->GetTerrainArea();
		m_rcCameraMoveRect.left = rcTerrain.left - fExpand;
		m_rcCameraMoveRect.right = rcTerrain.right + fExpand;
		m_rcCameraMoveRect.top = rcTerrain.top + fExpand;
		m_rcCameraMoveRect.bottom = rcTerrain.bottom - fExpand;
	}

	// Reset selected area
	A3DRELEASE(m_pAutoSelectedArea);
	m_pAutoSelectedArea = new CAutoSelectedArea(m_pAutoHome);

	// Reset building manager
	A3DRELEASE(m_pAutoBuildingMan);
	m_pAutoBuildingMan = new CAutoBuildingMan(m_pAutoHome);
}

bool CAutoHomeMan::SetHome(int r, int c)
{
	CAutoHome* pAutoHome = m_pAutoScene->GetHome(r , c);
	SetHome(pAutoHome);
	if (NULL == pAutoHome)
		return false;
	else
		return true;
}

bool CAutoHomeMan::Tick(DWORD dwTickTime)
{
	return true;
}

bool CAutoHomeMan::Render(A3DViewport* pA3DViewport)
{
	if (NULL == m_pAutoHome)
		return true;

	if (!m_pAutoHome->GetAutoHomeBorder()->Render())
		return false;
	
	if (NULL != m_pAutoSelectedArea)
	{
		if (!m_pAutoSelectedArea->Render())
			return false;
	}

	if (NULL != m_pAutoBuildingMan)
	{
		if (!m_pAutoBuildingMan->Render())
			return false;
	}

	return true;
}

// Draw border of home
bool CAutoHomeMan::DrawHomeBorder()
{
	const float fUpGround = 5.0f;
	A3DVECTOR3 vLeftTop, vLeftBottom, vRightTop, vRightBottom;
	float fHalfBlockSize = m_pAutoScene->GetBlockSize() * 0.5f;

	vLeftTop.x = -fHalfBlockSize;
	vLeftTop.z = fHalfBlockSize;
	m_pAutoHome->LocalToWorld(vLeftTop, vLeftTop);
	vLeftTop.y = m_pAutoScene->GetPosHeight(vLeftTop) + fUpGround;

	vLeftBottom.x = -fHalfBlockSize;
	vLeftBottom.z = -fHalfBlockSize;
	m_pAutoHome->LocalToWorld(vLeftBottom, vLeftBottom);
	vLeftBottom.y = m_pAutoScene->GetPosHeight(vLeftBottom) + fUpGround;

	vRightTop.x = fHalfBlockSize;
	vRightTop.z = fHalfBlockSize;
	m_pAutoHome->LocalToWorld(vRightTop, vRightTop);
	vRightTop.y = m_pAutoScene->GetPosHeight(vRightTop) + fUpGround;

	vRightBottom.x = fHalfBlockSize;
	vRightBottom.z = -fHalfBlockSize;
	m_pAutoHome->LocalToWorld(vRightBottom, vRightBottom);
	vRightBottom.y = m_pAutoScene->GetPosHeight(vRightBottom) + fUpGround;

	A3DWireCollector* pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	DWORD dwColor = 0xffff0000;
	pWireCollector->Add3DLine(vLeftTop, vLeftBottom, dwColor);
	pWireCollector->Add3DLine(vLeftBottom, vRightBottom, dwColor);
	pWireCollector->Add3DLine(vRightBottom, vRightTop, dwColor);
	pWireCollector->Add3DLine(vRightTop, vLeftTop, dwColor);

	return true;
}

// Set default camera move rect
void CAutoHomeMan::SetDefaultCameraMoveRect()
{
	float fHalfWidth = m_pAutoScene->GetWorldWid();
	float fHalfHeight = m_pAutoScene->GetWorldLen();
	m_rcCameraMoveRect.left = -fHalfWidth;
	m_rcCameraMoveRect.right = fHalfWidth;
	m_rcCameraMoveRect.top = fHalfHeight;
	m_rcCameraMoveRect.bottom = -fHalfHeight;
}

// Is camera pos in move rect
bool CAutoHomeMan::IsInCameraRect(const A3DVECTOR3& vPos)
{
	if (vPos.x < m_rcCameraMoveRect.left
		|| vPos.x > m_rcCameraMoveRect.right
		|| vPos.z < m_rcCameraMoveRect.bottom
		|| vPos.z > m_rcCameraMoveRect.top)
		return false;

	return true;
}

// Create base terrain
bool CAutoHomeMan::CreateBaseTerrain(float fAmplitude, int nFrequency, float fPersistence, 
				 float fTextureRatio, float fMainTexture, 
				 float fPartTexture, float fPartDivision, DWORD dwRandSeed)
{
	if (NULL == m_pAutoHome)
		return false;

	// Create height
	CAutoTerrain* pAutoTerrain = m_pAutoHome->GetAutoTerrain();
	if (!pAutoTerrain->CreateTerrainHeight(
		fAmplitude, nFrequency, fPersistence, dwRandSeed))
		return false;
	if (!pAutoTerrain->ApplyHeightAndNormal())
		return false;
	if (!pAutoTerrain->ApplyTerrainColor())
		return false;

	// Create mask
	CAutoTexture* pAutoTexture = m_pAutoHome->GetAutoTexture();
	if (!pAutoTexture->CreateTerrainMask(
		fTextureRatio, fMainTexture, fPartTexture, fPartDivision, -1))
		return false;
	int nLayer[2];
	nLayer[0] = TT_MAIN;
	nLayer[1] = TT_MAIN_PATCH;
	if (!pAutoTexture->ApplyLayerMaskData(nLayer, 2))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionBaseTerrain(
		fAmplitude, nFrequency, fPersistence,
		fTextureRatio, fMainTexture, fPartTexture, fPartDivision, dwRandSeed))
		return false;

	// Adjust buildings
	m_pAutoBuildingMan->AdjustModelsAfterTerrainChange();
	
	// Set edit flag
	m_bEdited = true;

	return true;
}

bool CAutoHomeMan::LoadHome()
{
	if (NULL == m_pAutoHome)
		return false;

	if (!m_pAutoHome->LoadHome())
		return false;

	m_bEdited = false;

	return true;
}

bool CAutoHomeMan::SaveHome()
{
	if (NULL == m_pAutoHome)
		return false;

	if (!m_pAutoHome->SaveHome())
		return false;
	m_bEdited = false;

	return true;
}

// Change main terrain texutre
bool CAutoHomeMan::ChangeMainTerrainTexture(DWORD dwMainID, DWORD dwPatchID)
{
	if (NULL == m_pAutoHome)
		return false;

	// Change texture
	if (!m_pAutoHome->GetAutoTexture()->ChangeTextureByID(TT_MAIN, dwMainID))
		return false;
	if (!m_pAutoHome->GetAutoTexture()->ChangeTextureByID(TT_MAIN_PATCH, dwPatchID))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionBaseTextureInfo(10.0f, 10.0f, 10.0f, 10.0f,
		dwMainID, dwPatchID))
		return false;

	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Get texture ID
DWORD CAutoHomeMan::GetTextureID(int nLayer)
{
	if (NULL == m_pAutoHome)
		return -1;

	return m_pAutoHome->GetAutoTexture()->GetTextureID(nLayer);
}

bool CAutoHomeMan::IsReady()
{
	if (NULL == m_pAutoHome)
		return false;
	return true;
}

// Get auto scene config
CAutoSceneConfig* CAutoHomeMan::GetAutoSceneConfig()
{ 
	return m_pAutoScene->GetAutoSceneConfig(); 
}

// Create hill poly
bool CAutoHomeMan::CreateHillPoly(float fMaxHeight, float fMainSmooth, float fPartSmooth,
				DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, bool bHillUp, float fTextureRatio, float fMainTexture, float fPatchTexture, 
				float fPatchDivision, float fBridgeScope, float fBridgeDepth, int nNumPoints, POINT_FLOAT* pPoints, float fBorderErode)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHillPoly* pAutoHillPoly = m_pAutoHome->GetAutoHillPoly();
	CAutoTerrain* pAutoTerrain = m_pAutoHome->GetAutoTerrain();
	CAutoTexture* pAutoTexture = m_pAutoHome->GetAutoTexture();
	CAutoSelectedArea* pSelectedArea = m_pAutoHome->GetAutoSelectedArea();

	// Set area points
	if (!pAutoHillPoly->SetPoints(nNumPoints, pPoints))
		return false;

	// Create hill height
	if (!pAutoHillPoly->CreateHillPoly(fMaxHeight, fMainSmooth, fPartSmooth,
		dwSeed, nNumCurvePoint, pCurvePoints, bHillUp, fBorderErode))
		return false;
	if (!pAutoTerrain->ApplyHeightAndNormal())
		return false;
	if (!pAutoTerrain->ApplyTerrainColor())
		return false;

	// Create hill top texture
	if (!pAutoHillPoly->RenderHillTop(fTextureRatio, fMainTexture, fPatchTexture, fPatchDivision,
		fBridgeScope, fBridgeDepth, true))
		return false;
	int nLayer[2];
	nLayer[0] = TT_BRIDGE;
	nLayer[1] = TT_BRIDGE_PATCH;
	if (!pAutoTexture->ApplyLayerMaskData(nLayer, 2))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionHillPoly(fMaxHeight, fMainSmooth, fPartSmooth, dwSeed, nNumCurvePoint,
		pCurvePoints, bHillUp, fTextureRatio, fMainTexture, fPatchTexture, fPatchDivision, 
		fBridgeScope, fBridgeDepth, nNumPoints, pPoints, fBorderErode))
		return false;

	// Adjust buildings
	m_pAutoBuildingMan->AdjustModelsAfterTerrainChange();

	// Set edit flag
	m_bEdited = true;

	return true;
}

// Create hill line
bool CAutoHomeMan::CreateHillLine(float fMaxHeight, float fHillWidth, float fMainSmooth, float fPartSmooth,
				DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, int nNumCurveLinePoint, float* pCurveLinePoints, bool bHillUp, int nNumPoints, POINT_FLOAT* pPoints,
				float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, float fBridgeScope, float fBorderErode)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHillLine* pAutoHillLine = m_pAutoHome->GetAutoHillLine();
	CAutoTerrain* pAutoTerrain = m_pAutoHome->GetAutoTerrain();
	CAutoTexture* pAutoTexture = m_pAutoHome->GetAutoTexture();
	CAutoSelectedArea* pSelectedArea = m_pAutoHome->GetAutoSelectedArea();

	// Set area points
	if (!pAutoHillLine->SetPoints(nNumPoints, pPoints))
		return false;

	// Create hill height
	if (!pAutoHillLine->CreateHillLine(fMaxHeight, fHillWidth, fMainSmooth, fPartSmooth,
		dwSeed, nNumCurvePoint, pCurvePoints, nNumCurveLinePoint, pCurveLinePoints,bHillUp, fBorderErode))
		return false;
//	pAutoTerrain->ResetHeightData(MAX_HEIGHT);
//	pAutoTerrain->UpdateVertexNormals();
	if (!pAutoTerrain->ApplyHeightAndNormal())
		return false;
	if (!pAutoTerrain->ApplyTerrainColor())
		return false;

	// Create hill top texture
	if (!pAutoHillLine->RenderHillTop(fTextureRatio, fMainTexture, fPatchTexture, fPatchDivision,
		fBridgeScope, true))
		return false;
	int nLayer[2];
	nLayer[0] = TT_BRIDGE;
	nLayer[1] = TT_BRIDGE_PATCH;
	if (!pAutoTexture->ApplyLayerMaskData(nLayer, 2))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionHillLine(fMaxHeight, fHillWidth, fMainSmooth, fPartSmooth, dwSeed, nNumCurvePoint,
		pCurvePoints, nNumCurveLinePoint, pCurveLinePoints, bHillUp, nNumPoints, pPoints, fTextureRatio, fMainTexture, fPatchTexture, fPatchDivision, 
		fBridgeScope, fBorderErode))
		return false;

	// Adjust buildings
	m_pAutoBuildingMan->AdjustModelsAfterTerrainChange();

	// Set edit flag
	m_bEdited = true;

	return true;
}

// Change main terrain texutre
bool CAutoHomeMan::ChangeHillTexture(DWORD dwMainID, DWORD dwPatchID)
{
	if (NULL == m_pAutoHome)
		return false;

	// Change texture
	if (!m_pAutoHome->GetAutoTexture()->ChangeTextureByID(TT_BRIDGE, dwMainID))
		return false;
	if (!m_pAutoHome->GetAutoTexture()->ChangeTextureByID(TT_BRIDGE_PATCH, dwPatchID))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionHillTextureInfo(10.0f, 10.0f, 10.0f, 10.0f,
		dwMainID, dwPatchID))
		return false;

	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Create part texture
bool CAutoHomeMan::CreatePartTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, 
				DWORD dwSeed, float fBorderScope, int nNumPoints, POINT_FLOAT* pPoints, int nTextureType)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoPartTexture* pPartTexture = m_pAutoHome->GetAutoPartTexture();

	// Create part texture
	if (!pPartTexture->SetPoints(nNumPoints, pPoints))
		return false;
	if (!pPartTexture->CreatePartTexture(fTextureRatio, fMainTexture, fPatchTexture,
		fPatchDivision, dwSeed, fBorderScope, nTextureType))
		return false;
	int nLayer[2];
	nLayer[0] = TT_PART;
	nLayer[1] = TT_PART_PATCH;
	if (!m_pAutoHome->GetAutoTexture()->ApplyLayerMaskData(nLayer, 2))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionPartTexture(fTextureRatio, fMainTexture, fPatchTexture,
		fPatchDivision, dwSeed, fBorderScope, nNumPoints, pPoints, nTextureType))
		return false;

	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Change part texutre
bool CAutoHomeMan::ChangePartTexture(DWORD dwMainID, DWORD dwPatchID)
{
	if (NULL == m_pAutoHome)
		return false;

	// Change texture
	if (!m_pAutoHome->GetAutoTexture()->ChangeTextureByID(TT_PART, dwMainID))
		return false;
	if (!m_pAutoHome->GetAutoTexture()->ChangeTextureByID(TT_PART_PATCH, dwPatchID))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionPartTextureInfo(10.0f, 10.0f, 10.0f, 10.0f,
		dwMainID, dwPatchID))
		return false;

	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Change water texutre
bool CAutoHomeMan::ChangeWaterTexture(DWORD dwMainID, DWORD dwPatchID)
{
	if (NULL == m_pAutoHome)
		return false;

	// Change texture
	if (!m_pAutoHome->GetAutoTexture()->ChangeTextureByID(TT_RIVERBED, dwMainID))
		return false;
	if (!m_pAutoHome->GetAutoTexture()->ChangeTextureByID(TT_RIVERSAND, dwPatchID))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionWaterTextureInfo(10.0f, 10.0f, 10.0f, 10.0f,
		dwMainID, dwPatchID))
		return false;

	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Create water
bool CAutoHomeMan::CreateWater(float fWaterHeight, float fSandWidth, float fWaveSize)
{
	if (NULL == m_pAutoHome)
		return false;

	// Create water
	CAutoWater* pAutoWater = m_pAutoHome->GetAutoWater();
	if (!pAutoWater->CreateWater(fWaterHeight, fSandWidth, fWaveSize))
		return false;
	int nLayer[2];
	nLayer[0] = TT_RIVERBED;
	nLayer[1] = TT_RIVERSAND;
	if (!m_pAutoHome->GetAutoTexture()->ApplyLayerMaskData(nLayer, 2))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionWater(fWaterHeight, fSandWidth, fWaveSize))
		return false;
	
	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Change road texutre
bool CAutoHomeMan::ChangeRoadTexture(DWORD dwMainID, DWORD dwPatchID)
{
	if (NULL == m_pAutoHome)
		return false;

	// Change texture
	if (!m_pAutoHome->GetAutoTexture()->ChangeTextureByID(TT_ROADBED, dwMainID))
		return false;
	if (!m_pAutoHome->GetAutoTexture()->ChangeTextureByID(TT_ROAD, dwPatchID))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionRoadTextureInfo(10.0f, 10.0f, 10.0f, 10.0f,
		dwMainID, dwPatchID))
		return false;

	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Create road
bool CAutoHomeMan::CreateRoad(float fRoadWidth, float fRoadHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, bool bFlatRoad)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoRoad* pAutoRoad = m_pAutoHome->GetAutoRoad();
	CAutoTerrain* pAutoTerrain = m_pAutoHome->GetAutoTerrain();
	CAutoTexture* pAutoTexture = m_pAutoHome->GetAutoTexture();

	// Set area points
	if (!pAutoRoad->SetPoints(nNumPoints, pPoints))
		return false;
	// Create road
	if (!pAutoRoad->CreateRoad(fRoadWidth, fRoadHeight, fMainSmooth, fPartSmooth, dwSeed, bFlatRoad))
		return false;
	if (!pAutoTerrain->ApplyHeightAndNormal())
		return false;
	if (!pAutoTerrain->ApplyTerrainColor())
		return false;
	int nLayer[2];
	nLayer[0] = TT_ROADBED;
	nLayer[1] = TT_ROAD;
	if (!m_pAutoHome->GetAutoTexture()->ApplyLayerMaskData(nLayer, 2))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionRoad(fRoadWidth, fRoadHeight, fMainSmooth, fPartSmooth,
		dwSeed, nNumPoints, pPoints, bFlatRoad? 1 : 0))
		return false;

	// Adjust buildings
	m_pAutoBuildingMan->AdjustModelsAfterTerrainChange();

	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Create terrain flat
bool CAutoHomeMan::CreateTerrainFlat(int nNumPoints, POINT_FLOAT* pPoints)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoTerrainFlat* pAutoTerrainFlat = m_pAutoHome->GetAutoTerrainFlat();
	CAutoTerrain* pAutoTerrain = m_pAutoHome->GetAutoTerrain();

	// Set area points
	if (!pAutoTerrainFlat->SetPoints(nNumPoints, pPoints))
		return false;

	// Flat terrain
	if (!pAutoTerrainFlat->FlatTerrain())
		return false;
	if (!pAutoTerrain->ApplyHeightAndNormal())
		return false;
	if (!pAutoTerrain->ApplyTerrainColor())
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionTerrainFlat(nNumPoints, pPoints))
		return false;

	// Adjust buildings
	m_pAutoBuildingMan->AdjustModelsAfterTerrainChange();
	
	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Create polygon forest
bool CAutoHomeMan::CreateForestPoly(int nPlantType, float fDensityX, float fDensityZ, float fMaxSlope,
			int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoForestPoly* pAutoForestPoly = m_pAutoHome->GetAutoForestPoly();
	// Set area points
	if (!pAutoForestPoly->SetPoints(nNumPoints, pPoints))
		return false;
	// Create forest poly
	if (!pAutoForestPoly->CreateForestPoly(nPlantType, fDensityX, fDensityZ, fMaxSlope,
		nTreeTypeCount, pIDs, pWeights, dwSeed))
		return false;

		// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionForestPoly(nPlantType, fDensityX, fDensityZ, fMaxSlope,
		nTreeTypeCount, pIDs, pWeights, dwSeed, nNumPoints, pPoints))
		return false;

	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Create linear forest
bool CAutoHomeMan::CreateForestLine(float fSpace, float fSpaceNoise, float fMaxSlope,
			int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoForestLine* pAutoForestLine = m_pAutoHome->GetAutoForestLine();
	// Set area points
	if (!pAutoForestLine->SetPoints(nNumPoints, pPoints))
		return false;
	// Create forest line
	if (!pAutoForestLine->CreateForestLine(fSpace, fSpaceNoise, fMaxSlope, 
		nTreeTypeCount, pIDs, pWeights, dwSeed))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionForestLine(fSpace, fSpaceNoise, fMaxSlope, 
		nTreeTypeCount, pIDs, pWeights, dwSeed, nNumPoints, pPoints))
		return false;

	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Create grass poly
bool CAutoHomeMan::CreateGrassPoly(int nGrassKind, float fDensity,
			int nGrassTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoGrassPoly* pAutoGrassPoly = m_pAutoHome->GetAutoGrassPoly();

	// Set area points
	if (!pAutoGrassPoly->SetPoints(nNumPoints, pPoints))
		return false;

	// Create grass poly
	if (!pAutoGrassPoly->CreateGrassPoly(nGrassKind, fDensity, nGrassTypeCount, pIDs,
		pWeights, dwSeed))
		return false;

	// Add action
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->AddActionGrassPoly(nGrassKind, fDensity, nGrassTypeCount, pIDs,
		pWeights, dwSeed, nNumPoints, pPoints))
		return false;

	// Set edit flag
	m_bEdited = true;
	
	return true;
}

// Undo action
bool CAutoHomeMan::UndoAction(int nActionType)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoTerrain* pAutoTerrain = m_pAutoHome->GetAutoTerrain();
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	CAutoTexture* pAutoTexture = m_pAutoHome->GetAutoTexture();
	switch(nActionType)
	{
	case TAT_HILL_RECT:
		break;
	case TAT_HILL_POLY:
		if (!pAutoHomeAction->ActionUndo(TAT_HILL_POLY))
			return false;
		pAutoTerrain->ClearAllHill(true);
		pAutoTexture->ResetHillTexture();
		pAutoHomeAction->DoHillActions();
		pAutoHomeAction->DoTerrainFlatActions();
		m_bEdited = true;
		break;
	case TAT_HILL_LINE:
		if (!pAutoHomeAction->ActionUndo(TAT_HILL_LINE))
			return false;
		pAutoTerrain->ClearAllHill(true);
		pAutoTexture->ResetHillTexture();
		pAutoHomeAction->DoHillActions();
		pAutoHomeAction->DoTerrainFlatActions();
		m_bEdited = true;
		break;
	case TAT_TEXTURE_POLY:
		if (!pAutoHomeAction->ActionUndo(TAT_TEXTURE_POLY))
			return false;
		pAutoTexture->ResetPartTexture();
		pAutoHomeAction->DoPartTextureActions();
		m_bEdited = true;
		break;
	case TAT_ROAD:
		if (!pAutoHomeAction->ActionUndo(TAT_ROAD))
			return false;
		pAutoTerrain->ClearRoadHeight(true);
		pAutoTexture->ResetRoadTexture();
		pAutoHomeAction->DoRoadActions();
		pAutoHomeAction->DoTerrainFlatActions();
		m_bEdited = true;
		break;
	case TAT_TERRAIN_FLAT:
		if (!pAutoHomeAction->ActionUndo(TAT_TERRAIN_FLAT))
			return false;
		pAutoTerrain->UpdateAbsHeight();
		pAutoHomeAction->DoTerrainFlatActions();
		m_bEdited = true;
		break;
	case TAT_FOREST_POLY:
		if (!pAutoHomeAction->ActionUndo(TAT_FOREST_POLY))
			return false;
		m_pAutoHome->GetAutoForest()->RemoveAllTree();
		pAutoHomeAction->DoForestPolyActions();
		pAutoHomeAction->DoForestLineActions();
		m_bEdited = true;
		break;
	case TAT_FOREST_LINE:
		if (!pAutoHomeAction->ActionUndo(TAT_FOREST_LINE))
			return false;
		m_pAutoHome->GetAutoForest()->RemoveAllTree();
		pAutoHomeAction->DoForestPolyActions();
		pAutoHomeAction->DoForestLineActions();
		m_bEdited = true;
		break;
	case TAT_GRASS_POLY:
		if (!pAutoHomeAction->ActionUndo(TAT_GRASS_POLY))
			return false;
		m_pAutoHome->GetAutoGrass()->RemoveAllGrass();
		pAutoHomeAction->DoGrassPolyActions();
		m_bEdited = true;
		break;
	}

	// Apply height
	if (nActionType == TAT_HILL_RECT 
		|| nActionType == TAT_HILL_POLY 
		|| nActionType == TAT_HILL_LINE
		|| nActionType == TAT_ROAD
		|| nActionType == TAT_TERRAIN_FLAT)
	{
		if (!pAutoTerrain->ApplyHeightAndNormal())
			return false;
		if (!pAutoTerrain->ApplyTerrainColor())
			return false;
		// Clear hill bridge texture on down area.
		if (2 == pAutoTerrain->GetUpDownPri())// Down first
		{
			pAutoTexture->ClearBridgeAtDownArea();
		}
		// Update buildings
		m_pAutoBuildingMan->AdjustModelsAfterTerrainChange();
	}

	// Apply texture
	int nLayer[2];
	if (nActionType == TAT_HILL_RECT 
		|| nActionType == TAT_HILL_POLY 
		|| nActionType == TAT_HILL_LINE
		|| nActionType == TAT_TERRAIN_FLAT)
	{
		pAutoTexture->BlurMask(TT_BRIDGE);
		pAutoTexture->BlurMask(TT_BRIDGE_PATCH);
		nLayer[0] = TT_BRIDGE;
		nLayer[1] = TT_BRIDGE_PATCH;
		if (!pAutoTexture->ApplyLayerMaskData(nLayer, 2))
			return false;
	}
	if (nActionType == TAT_ROAD)
	{
		nLayer[0] = TT_ROADBED;
		nLayer[1] = TT_ROAD;
		if (!pAutoTexture->ApplyLayerMaskData(nLayer, 2))
			return false;
	}
	if (nActionType == TAT_TEXTURE_POLY)
	{
		nLayer[0] = TT_PART;
		nLayer[1] = TT_PART_PATCH;
		if (!pAutoTexture->ApplyLayerMaskData(nLayer, 2))
			return false;
	}

	return true;
}
// Redo action
bool CAutoHomeMan::RedoAction(int nActionType)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoTerrain* pAutoTerrain = m_pAutoHome->GetAutoTerrain();
	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	CAutoTexture* pAutoTexture = m_pAutoHome->GetAutoTexture();
	switch(nActionType)
	{
	case TAT_HILL_RECT:
		break;
	case TAT_HILL_POLY:
		if (!pAutoHomeAction->ActionRedo(TAT_HILL_POLY))
			return false;
		pAutoHomeAction->RedoHillPolyAction();
		m_bEdited = true;
		break;
	case TAT_HILL_LINE:
		if (!pAutoHomeAction->ActionRedo(TAT_HILL_LINE))
			return false;
		pAutoHomeAction->RedoHillLineAction();
		m_bEdited = true;
		break;
	case TAT_TEXTURE_POLY:
		if (!pAutoHomeAction->ActionRedo(TAT_TEXTURE_POLY))
			return false;
		pAutoHomeAction->RedoPartTextureAction();
		m_bEdited = true;
		break;
	case TAT_ROAD:
		if (!pAutoHomeAction->ActionRedo(TAT_ROAD))
			return false;
		pAutoHomeAction->RedoRoadAction();
		m_bEdited = true;
		break;
	case TAT_FOREST_POLY:
		if (!pAutoHomeAction->ActionRedo(TAT_FOREST_POLY))
			return false;
		pAutoHomeAction->RedoForestPolyAction();
		m_bEdited = true;
		break;
	case TAT_FOREST_LINE:
		if (!pAutoHomeAction->ActionRedo(TAT_FOREST_LINE))
			return false;
		pAutoHomeAction->RedoForestLineAction();
		m_bEdited = true;
		break;
	case TAT_GRASS_POLY:
		if (!pAutoHomeAction->ActionRedo(TAT_GRASS_POLY))
			return false;
		pAutoHomeAction->RedoGrassPolyAction();
		m_bEdited = true;
		break;
	case TAT_TERRAIN_FLAT:
		if (!pAutoHomeAction->ActionRedo(TAT_TERRAIN_FLAT))
			return false;
		pAutoHomeAction->RedoTerrainFlatAction();
		m_bEdited = true;
		break;
	}

	if (nActionType == TAT_HILL_RECT 
		|| nActionType == TAT_HILL_POLY 
		|| nActionType == TAT_HILL_LINE
		|| nActionType == TAT_ROAD
		|| nActionType == TAT_TERRAIN_FLAT)
	{
		pAutoTerrain->UpdateAbsHeight();
		if (!pAutoTerrain->ApplyHeightAndNormal())
			return false;
		if (!pAutoTerrain->ApplyTerrainColor())
			return false;

//		pAutoTexture->RestoreBridgeMask();
		// Clear hill bridge texture on down area.
		if (2 == pAutoTerrain->GetUpDownPri())// Down first
		{
			pAutoTexture->ClearBridgeAtDownArea();
		}
		// Update buildings
		m_pAutoBuildingMan->AdjustModelsAfterTerrainChange();
	}	
	
	// Apply texture
	int nLayer[2];
	if (nActionType == TAT_HILL_RECT 
		|| nActionType == TAT_HILL_POLY 
		|| nActionType == TAT_HILL_LINE
		|| nActionType == TAT_TERRAIN_FLAT)
	{
		pAutoTexture->BlurMask(TT_BRIDGE);
		pAutoTexture->BlurMask(TT_BRIDGE_PATCH);
		nLayer[0] = TT_BRIDGE;
		nLayer[1] = TT_BRIDGE_PATCH;
		if (!pAutoTexture->ApplyLayerMaskData(nLayer, 2))
			return false;
	}
	if (nActionType == TAT_ROAD)
	{
		nLayer[0] = TT_ROADBED;
		nLayer[1] = TT_ROAD;
		if (!pAutoTexture->ApplyLayerMaskData(nLayer, 2))
			return false;
	}
	if (nActionType == TAT_TEXTURE_POLY)
	{
		nLayer[0] = TT_PART;
		nLayer[1] = TT_PART_PATCH;
		if (!pAutoTexture->ApplyLayerMaskData(nLayer, 2))
			return false;
	}

	return true;
}

bool CAutoHomeMan::Init()
{
	A3DRELEASE(m_pAutoParameters);
	m_pAutoParameters = new CAutoParameters();
	if (NULL == m_pAutoParameters)
		return false;
	if (!m_pAutoParameters->Init())
		return false;

	return true;
}

// Get action number
int CAutoHomeMan::GetActionNumber(int nActionType)
{
	if (NULL == m_pAutoHome)
		return 0;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	int nNumAction = 0;

	switch (nActionType)
	{
	case TAT_HILL_POLY:
		nNumAction = pAutoHomeAction->GetNumActionHillPoly();
		break;
	case TAT_HILL_LINE:
		nNumAction = pAutoHomeAction->GetNumActionHillLine();
		break;
	case TAT_TEXTURE_POLY:
		nNumAction = pAutoHomeAction->GetNumActionPartTexture();
		break;
	case TAT_ROAD:
		nNumAction = pAutoHomeAction->GetNumActionRoad();
		break;
	case TAT_FOREST_POLY:
		nNumAction = pAutoHomeAction->GetNumActionForestPoly();
		break;
	case TAT_FOREST_LINE:
		nNumAction = pAutoHomeAction->GetNumActionForestLine();
		break;
	case TAT_GRASS_POLY:
		nNumAction = pAutoHomeAction->GetNumActionGrassPoly();
		break;
	case TAT_TERRAIN_FLAT:
		nNumAction = pAutoHomeAction->GetNumActionTerrainFlat();
		break;
	case TAT_BUILDING:
		nNumAction = m_pAutoBuildingMan->GetNumBuildings();
		break;
	}

	return nNumAction;
}

// Select last action
bool CAutoHomeMan::SelectLastAction(int nActionType)
{
	int nLastIndex = GetActionNumber(nActionType) - 1;
	if (-1 == nLastIndex)
		return false;

	return SelectAction(nActionType, nLastIndex);
}

// Select action
bool CAutoHomeMan::SelectAction(int nActionType, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	int nNumPoints;
	POINT_FLOAT* pAreaPoints;
	
	switch (nActionType)
	{
	case TAT_HILL_POLY:
	case TAT_TEXTURE_POLY:
	case TAT_FOREST_POLY:
	case TAT_GRASS_POLY:
	case TAT_TERRAIN_FLAT:
		pAutoHomeAction->GetAreaPointsInAction(nActionType, nIndex, &nNumPoints, &pAreaPoints);
		m_pAutoSelectedArea->SetViewPoints(nNumPoints, pAreaPoints);
		m_pAutoSelectedArea->SetViewAreaClosed(true);
		m_pAutoSelectedArea->DeleteAllPolyEditPoints();
		break;
	case TAT_HILL_LINE:
	case TAT_ROAD:
	case TAT_FOREST_LINE:
		pAutoHomeAction->GetAreaPointsInAction(nActionType, nIndex, &nNumPoints, &pAreaPoints);
		m_pAutoSelectedArea->SetViewPoints(nNumPoints, pAreaPoints);
		m_pAutoSelectedArea->SetViewAreaClosed(false);
		m_pAutoSelectedArea->DeleteAllLineEditPoints();
		break;
	case TAT_BUILDING:
		if (BP_ADD != m_pAutoBuildingMan->GetBuildingOpera())
			m_pAutoBuildingMan->SetSelectedModelIndex(nIndex);
		break;
	}
	
	return true;
}

// Delete action
bool CAutoHomeMan::DeleteAction(int nActionType, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	CAutoTerrain* pAutoTerrain = m_pAutoHome->GetAutoTerrain();
	CAutoTexture* pAutoTexture = m_pAutoHome->GetAutoTexture();
	CAutoForest* pAutoForest = m_pAutoHome->GetAutoForest();
	CAutoGrass* pAutoGrass = m_pAutoHome->GetAutoGrass();

	int nLayer[4];

	switch (nActionType)
	{
	case TAT_HILL_POLY:
	case TAT_HILL_LINE:
	case TAT_ROAD:
	case TAT_TERRAIN_FLAT:
		pAutoHomeAction->DeleteAction(nActionType, nIndex);
		UpdateHeightActions();
		// Delete view points
		m_pAutoSelectedArea->DeleteAllViewPoints();
		m_bEdited = true;
		break;
	case TAT_FOREST_LINE:
	case TAT_FOREST_POLY:
		pAutoHomeAction->DeleteAction(nActionType, nIndex);
		pAutoForest->RemoveAllTree();
		pAutoHomeAction->DoForestPolyActions();
		pAutoHomeAction->DoForestLineActions();
		// Delete view points
		m_pAutoSelectedArea->DeleteAllViewPoints();
		m_bEdited = true;
		break;
	case TAT_GRASS_POLY:
		pAutoHomeAction->DeleteAction(nActionType, nIndex);
		pAutoGrass->RemoveAllGrass();
		pAutoHomeAction->DoGrassPolyActions();
		// Delete view points
		m_pAutoSelectedArea->DeleteAllViewPoints();
		m_bEdited = true;
		break;
	case TAT_TEXTURE_POLY:
		pAutoHomeAction->DeleteAction(nActionType, nIndex);
		pAutoTexture->ResetMask(TT_PART);
		pAutoTexture->ResetMask(TT_PART_PATCH);
		pAutoHomeAction->DoPartTextureActions();
		nLayer[0] = TT_PART;
		nLayer[1] = TT_PART_PATCH;
		pAutoTexture->ApplyLayerMaskData(nLayer, 2);
		// Delete view points
		m_pAutoSelectedArea->DeleteAllViewPoints();
		m_bEdited = true;
		break;
	case TAT_BUILDING:
		if (m_pAutoBuildingMan->GetBuildingOpera() != BP_ADD)
		{
			m_pAutoBuildingMan->DeleteBuilding(nIndex);
			m_bEdited = true;
		}
		break;
	}

	return true;
}

// Get action
void* CAutoHomeMan::GetAction(int nActionType, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();

	void* pAction = NULL;
	switch (nActionType)
	{
	case TAT_HILL_POLY:
		pAction = &((*(pAutoHomeAction->GetActionHillPoly()))[nIndex]);
		break;
	case TAT_HILL_LINE:
		pAction = &((*(pAutoHomeAction->GetActionHillLine()))[nIndex]);
		break;
	case TAT_TEXTURE_POLY:
		pAction = &((*(pAutoHomeAction->GetActionPartTexture()))[nIndex]);
		break;
	case TAT_ROAD:
		pAction = &((*(pAutoHomeAction->GetActionRoad()))[nIndex]);
		break;
	case TAT_TERRAIN_FLAT:
		pAction = &((*(pAutoHomeAction->GetActionTerrainFlat()))[nIndex]);
		break;
	case TAT_FOREST_POLY:
		pAction = &((*(pAutoHomeAction->GetActionForestPoly()))[nIndex]);
		break;
	case TAT_FOREST_LINE:
		pAction = &((*(pAutoHomeAction->GetActionForestLine()))[nIndex]);
		break;
	case TAT_GRASS_POLY:
		pAction = &((*(pAutoHomeAction->GetActionGrassPoly()))[nIndex]);
		break;
	case TAT_BUILDING:
		break;
	}

	return pAction;
}

// Edit action
bool CAutoHomeMan::EditAction(int nActionType, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	int nNumPoints;
	POINT_FLOAT* pAreaPoints;

	switch (nActionType)
	{
	case TAT_HILL_POLY:
	case TAT_TEXTURE_POLY:
	case TAT_FOREST_POLY:
	case TAT_GRASS_POLY:
	case TAT_TERRAIN_FLAT:
		pAutoHomeAction->GetAreaPointsInAction(nActionType, nIndex, &nNumPoints, &pAreaPoints);
		m_pAutoSelectedArea->SetPolyEditPoints(nNumPoints, pAreaPoints);
		m_pAutoSelectedArea->SetPolyEditAreaClosed(true);
		m_pAutoSelectedArea->DeleteAllViewPoints();
		break;
	case TAT_HILL_LINE:
	case TAT_ROAD:
	case TAT_FOREST_LINE:
		pAutoHomeAction->GetAreaPointsInAction(nActionType, nIndex, &nNumPoints, &pAreaPoints);
		m_pAutoSelectedArea->SetLineEditPoints(nNumPoints, pAreaPoints);
		m_pAutoSelectedArea->DeleteAllViewPoints();
		break;
	case TAT_BUILDING:
		break;
	}

	return true;
}

// Change hill poly
bool CAutoHomeMan::ChangeHillPoly(float fMaxHeight, float fMainSmooth, float fPartSmooth,
				DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, bool bHillUp, float fTextureRatio, float fMainTexture, float fPatchTexture, 
				float fPatchDivision, float fBridgeScope, float fBridgeDepth, int nNumPoints, POINT_FLOAT* pPoints, float fBorderErode, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->ChangeActionHillPoly(fMaxHeight, fMainSmooth, fPartSmooth, dwSeed,
			nNumCurvePoint, pCurvePoints, bHillUp, fTextureRatio, fMainTexture, fPatchTexture,
			fPatchDivision, fBridgeScope, fBridgeDepth, nNumPoints, pPoints, fBorderErode, nIndex))
		return false;

	if (!UpdateHeightActions())
		return false;

	// Set view points
	m_pAutoSelectedArea->DeleteAllPolyEditPoints();
	m_pAutoSelectedArea->SetViewPoints(nNumPoints, pPoints);
	m_pAutoSelectedArea->SetViewAreaClosed(true);
	m_bEdited = true;

	return true;
}

//Update height action
bool CAutoHomeMan::UpdateHeightActions()
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	CAutoTerrain* pAutoTerrain = m_pAutoHome->GetAutoTerrain();
	CAutoTexture* pAutoTexture = m_pAutoHome->GetAutoTexture();

	int nLayer[4];
	pAutoTerrain->ClearAllHill(false);
	pAutoTerrain->ClearRoadHeight(false);
	pAutoTexture->ResetMask(TT_BRIDGE);
	pAutoTexture->ResetMask(TT_BRIDGE_PATCH);
	pAutoTexture->ResetMask(TT_ROADBED);
	pAutoTexture->ResetMask(TT_ROAD);
	pAutoHomeAction->DoHillActions();
	pAutoHomeAction->DoRoadActions();
	pAutoTerrain->UpdateAbsHeight();
	pAutoHomeAction->DoTerrainFlatActions();
	pAutoTerrain->UpdateVertexNormals();
	if (CAutoTerrain::UDP_DOWN == pAutoTerrain->GetUpDownPri())
		pAutoTexture->ClearBridgeAtDownArea();
	// Apply terrain data
	pAutoTerrain->ApplyHeightAndNormal();
	pAutoTerrain->ApplyTerrainColor();
	nLayer[0] = TT_BRIDGE;
	nLayer[1] = TT_BRIDGE_PATCH;
	nLayer[2] = TT_ROADBED;
	nLayer[3] = TT_ROAD;
	pAutoTexture->ApplyLayerMaskData(nLayer, 4);
	// Adjust buildings
	m_pAutoBuildingMan->AdjustModelsAfterTerrainChange();

	return true;
}

// Set terrain up and down pri
bool CAutoHomeMan::SetUpDownPri(int nPri)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoTerrain* pAutoTerrain = m_pAutoHome->GetAutoTerrain();
	pAutoTerrain->SetUpDownPri(nPri);

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	pAutoHomeAction->SetUpDownPri(nPri);

	return true;
}

// Change hill line
bool CAutoHomeMan::ChangeHillLine(float fMaxHeight, float fHillWidth, float fMainSmooth, float fPartSmooth,
			DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, int nNumCurveLinePoint, float* pCurveLinePoints, bool bHillUp, int nNumPoints, POINT_FLOAT* pPoints,
			float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, float fBridgeScope, float fBorderErode, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->ChangeActionHillLine(fMaxHeight, fHillWidth, fMainSmooth, fPartSmooth, dwSeed,
			nNumCurvePoint, pCurvePoints, nNumCurveLinePoint, pCurveLinePoints, bHillUp, nNumPoints, pPoints, 
			fTextureRatio, fMainTexture, fPatchTexture,	fPatchDivision, fBridgeScope, fBorderErode, nIndex))
		return false;

	if (!UpdateHeightActions())
		return false;

	// Set view points
	m_pAutoSelectedArea->DeleteAllLineEditPoints();
	m_pAutoSelectedArea->SetViewPoints(nNumPoints, pPoints);
	m_pAutoSelectedArea->SetViewAreaClosed(false);
	m_bEdited = true;

	return true;
}

// Change part texture
bool CAutoHomeMan::ChangePartTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, 
			DWORD dwSeed, float fBorderScope, int nNumPoints, POINT_FLOAT* pPoints, int nTextureType, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->ChangeActionPartTexture(fTextureRatio, fMainTexture, fPatchTexture, fPatchDivision,
		dwSeed, fBorderScope, nNumPoints, pPoints, nTextureType, nIndex))
		return false;

	CAutoTexture* pAutoTexture = m_pAutoHome->GetAutoTexture();
	pAutoTexture->ResetMask(TT_PART);
	pAutoTexture->ResetMask(TT_PART_PATCH);
	pAutoHomeAction->DoPartTextureActions();
	int nLayer[2];
	nLayer[0] = TT_PART;
	nLayer[1] = TT_PART_PATCH;
	pAutoTexture->ApplyLayerMaskData(nLayer, 2);

	// Set view points
	m_pAutoSelectedArea->DeleteAllPolyEditPoints();
	m_pAutoSelectedArea->SetViewPoints(nNumPoints, pPoints);
	m_pAutoSelectedArea->SetViewAreaClosed(true);
	m_bEdited = true;

	return true;
}

// Change generating road
bool CAutoHomeMan::ChangeRoad(float fRoadWidth, float fRoadHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nFlatRoad, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->ChangeActionRoad(fRoadWidth, fRoadHeight, fMainSmooth, fPartSmooth, dwSeed, 
		nNumPoints, pPoints, nFlatRoad, nIndex))
		return false;

	if (!UpdateHeightActions())
		return false;

	// Set view points
	m_pAutoSelectedArea->DeleteAllLineEditPoints();
	m_pAutoSelectedArea->SetViewPoints(nNumPoints, pPoints);
	m_pAutoSelectedArea->SetViewAreaClosed(false);
	m_bEdited = true;

	return true;
}

// Change polygon forest
bool CAutoHomeMan::ChangeForestPoly(int nPlantType, float fDensityX, float fDensityZ, float fMaxSlope,
			int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->ChangeActionForestPoly(nPlantType, fDensityX, fDensityZ, fMaxSlope,
		nTreeTypeCount, pIDs, pWeights, dwSeed, nNumPoints, pPoints, nIndex))
		return false;

	CAutoForest* pAutoForest = m_pAutoHome->GetAutoForest();
	pAutoForest->RemoveAllTree();
	pAutoHomeAction->DoForestPolyActions();
	pAutoHomeAction->DoForestLineActions();

	// Set view points
	m_pAutoSelectedArea->DeleteAllPolyEditPoints();
	m_pAutoSelectedArea->SetViewPoints(nNumPoints, pPoints);
	m_pAutoSelectedArea->SetViewAreaClosed(true);
	m_bEdited = true;

	return true;
}

// Change linear forest
bool CAutoHomeMan::ChangeForestLine(float fSpace, float fSpaceNoise, float fMaxSlope,
			int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->ChangeActionForestLine(fSpace, fSpaceNoise, fMaxSlope,
		nTreeTypeCount, pIDs, pWeights, dwSeed, nNumPoints, pPoints, nIndex))
		return false;

	CAutoForest* pAutoForest = m_pAutoHome->GetAutoForest();
	pAutoForest->RemoveAllTree();
	pAutoHomeAction->DoForestPolyActions();
	pAutoHomeAction->DoForestLineActions();

	// Set view points
	m_pAutoSelectedArea->DeleteAllLineEditPoints();
	m_pAutoSelectedArea->SetViewPoints(nNumPoints, pPoints);
	m_pAutoSelectedArea->SetViewAreaClosed(false);
	m_bEdited = true;

	return true;
}

// Change grass poly
bool CAutoHomeMan::ChangeGrassPoly(int nGrassKind, float fDensity,
			int nGrassTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->ChangeActionGrassPoly(nGrassKind, fDensity,
		nGrassTypeCount, pIDs, pWeights, dwSeed, nNumPoints, pPoints, nIndex))
		return false;

	CAutoGrass* pAutoGrass = m_pAutoHome->GetAutoGrass();
	pAutoGrass->RemoveAllGrass();
	pAutoHomeAction->DoGrassPolyActions();

	// Set view points
	m_pAutoSelectedArea->DeleteAllPolyEditPoints();
	m_pAutoSelectedArea->SetViewPoints(nNumPoints, pPoints);
	m_pAutoSelectedArea->SetViewAreaClosed(true);
	m_bEdited = true;

	return true;
}

// Change terrain flat
bool CAutoHomeMan::ChangeTerrainFlat(int nNumPoints, POINT_FLOAT* pPoints, int nIndex)
{
	if (NULL == m_pAutoHome)
		return false;

	CAutoHomeAction* pAutoHomeAction = m_pAutoHome->GetAutoHomeAction();
	if (!pAutoHomeAction->ChangeActionTerrainFlat(nNumPoints, pPoints, nIndex))
		return false;

	if (!UpdateHeightActions())
		return false;

	// Set view points
	m_pAutoSelectedArea->DeleteAllPolyEditPoints();
	m_pAutoSelectedArea->SetViewPoints(nNumPoints, pPoints);
	m_pAutoSelectedArea->SetViewAreaClosed(true);
	m_bEdited = true;

	return true;
}

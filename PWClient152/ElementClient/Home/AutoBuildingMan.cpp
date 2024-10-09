/*
 * FILE: AutoBuildingMan.cpp
 *
 * DESCRIPTION: Class for building operation
 *
 * CREATED BY: Jiang Dalong, 2006/06/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "AutoBuildingMan.h"
#include "AutoBuilding.h"
#include "AutoHome.h"
#include "Render.h"
#include "AutoScene.h"
#include "AutoTerrain.h"

#include <A3DEngine.h>
#include <A3DWireCollector.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoBuildingMan::CAutoBuildingMan(CAutoHome* pAutoHome)
{
	m_pAutoHome = pAutoHome;
	if (NULL != pAutoHome)
		m_pAutoBuilding = pAutoHome->GetAutoBuilding();
	else
		m_pAutoBuilding = NULL;
	m_nSelectedModelIndex = -1;
	m_bAddModelFailed = false;
	m_nBuildingOperation = BP_NONE;
	m_bModelMoved = false;
	m_bDrawSelectedOBB = false;
}

CAutoBuildingMan::~CAutoBuildingMan()
{
	Release();
}

void CAutoBuildingMan::Release()
{
}

bool CAutoBuildingMan::DoRayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, bool* pAdd)
{
	if (NULL != pAdd)
		*pAdd = false;

	switch (m_nBuildingOperation)
	{
	case BP_NONE:
//		SelectModel(vStart, vEnd);
		break;
	case BP_ADD:
		if (AddModelToScene(vStart, vEnd))
		{
//			AddCollision();
//			CheckModelCollision(m_alstCollision.GetSize()-1);
			m_bAddModelFailed = false;
			if (NULL != pAdd)
				*pAdd = true;
		}
		else
			m_bAddModelFailed = true;
		m_nBuildingOperation = BP_NONE;
		break;
	case BP_REPOS:
		if (m_nSelectedModelIndex >= 0)
		{
			if (SetBuildingPosByRayTrace(m_nSelectedModelIndex, vStart, vEnd))
			{
//				CheckModelCollision(m_nSelectedModelIndex);
//				CheckOthersCollision();
				m_bAddModelFailed = false;
			}
			else
				m_bAddModelFailed = true;
		}
		m_nBuildingOperation = BP_NONE;
		break;
	case BP_TRANSLATE:
		break;
	case BP_ROTATE:
		break;
	case BP_SCALE:
		break;
	}

	return true;
}

// Try to add a model to scene
bool CAutoBuildingMan::AddModelToScene(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd)
{
	A3DVECTOR3 vTracePos;
	if(!m_pAutoHome->RayTrace(vStart, vEnd, vTracePos, true)) 
		return false;

	if(!m_pAutoBuilding->LoadBuildingByID(m_dwCurModelID, vTracePos, 0))
		return false;

	m_nSelectedModelIndex = m_pAutoBuilding->GetNumBuildings() - 1;

	return true;
}

bool CAutoBuildingMan::Render()
{
	if (m_nSelectedModelIndex < 0)
		return false;

	if (!m_bDrawSelectedOBB)
		return true;

	MODELSTATUS status = m_pAutoBuilding->GetModelStatus(m_nSelectedModelIndex);
	A3DOBB* pOBB = &(status.obbModel);
	A3DVECTOR3 vRight;
	vRight = CrossProduct(-pOBB->ZAxis, A3DVECTOR3(0, 1, 0));
	DWORD dwColor;
//	if (IsInCollision(m_nSelectedModelIndex))
//		dwColor = 0xffff0000;
//	else 
		dwColor = 0xff0000ff;

	g_Render.GetA3DEngine()->GetA3DWireCollector()->Add3DBox(
		pOBB->Center, -pOBB->ZAxis, A3DVECTOR3(0, 1, 0), vRight, pOBB->Extents, dwColor);	
	return true;
}

// Select model in scene
bool CAutoBuildingMan::SelectModel(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd)
{
	A3DVECTOR3 vPos;
	m_nSelectedModelIndex = m_pAutoBuilding->RayTraceConvexHull(vStart, vEnd-vStart, vPos);

	if (-1 == m_nSelectedModelIndex)
		return false;

	return true;
}

// Delete model
bool CAutoBuildingMan::DeleteBuilding(int nIndex)
{
	if (NULL == m_pAutoBuilding)
		return false;
	m_pAutoBuilding->DeleteBuildingByIndex(nIndex);
	SetSelectedModelIndex(-1);

	return true;
}

// Delete all buildings
bool CAutoBuildingMan::DeleteAllBuildings()
{
	if (NULL == m_pAutoBuilding)
		return false;
	m_pAutoBuilding->DeleteAllBuildings();
	SetSelectedModelIndex(-1);

	return true;
}

// Move a model to a position
bool CAutoBuildingMan::SetBuildingPosByRayTrace(int nIndex, const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd)
{
	A3DVECTOR3 vTracePos;
	if(!m_pAutoHome->RayTrace(vStart, vEnd, vTracePos, true)) 
		return false;

	vTracePos += A3DVECTOR3(0, 0.1f, 0);
	A3DVECTOR3 vOldPos;
	if (!m_pAutoBuilding->GetBuildingPos(nIndex, vOldPos))
		return false;
	if (!m_pAutoHome->WorldToLocal(vOldPos, vOldPos))
		return false;
	A3DVECTOR3 vDelta = vTracePos - vOldPos;
	TranslateBuilding(nIndex, vDelta);
//	TranslateOthersInModel(vDelta);

	return true;
}

// Translate a model
bool CAutoBuildingMan::TranslateBuilding(int nIndex, const A3DVECTOR3& vTrans)
{
	m_pAutoBuilding->TranslateBuildingByIndex(nIndex, vTrans);
	m_pAutoBuilding->TranslateOBB(nIndex, vTrans);

	// Update model collision
//	CheckModelCollision(nIndex);
//	CheckOthersCollision();

	m_bModelMoved = true;

	return true;
}

// Translate current selected building
bool CAutoBuildingMan::TranslateCurBuilding(const A3DVECTOR3& vTrans)
{
	if (m_nSelectedModelIndex < 0)
		return false;
	if (!TranslateBuilding(m_nSelectedModelIndex, vTrans))
		return false;

	return true;
}

// Get building world position
bool CAutoBuildingMan::GetBuildingPos(int nIndex, A3DVECTOR3& vPosWorld)
{
	if (NULL == m_pAutoBuilding)
		return false;

	if (!m_pAutoBuilding->GetBuildingPos(nIndex, vPosWorld))
		return false;

	return true;
}

// Rotate model by itself
bool CAutoBuildingMan::RotateCurBuildingSelf(float fRotateY)
{
	if (m_nSelectedModelIndex < 0)
		return false;
	if (!RotateBuildingSelf(m_nSelectedModelIndex, fRotateY))
		return false;

	return true;
}

// rotate building by itself
bool CAutoBuildingMan::RotateBuildingSelf(int nIndex, float fRotateY)
{
	if (NULL == m_pAutoBuilding)
		return false;

	m_pAutoBuilding->RotateBuildingByIndex(nIndex, fRotateY);
	m_pAutoBuilding->SetOBBDirWithBuilding(nIndex);
	m_pAutoBuilding->CalculateBuildingColor(nIndex);
	
	// Update model collision
//	CheckModelCollision(m_nSelectedModelIndex);
//	CheckOthersCollision();

	return true;
}

// Get building count
int CAutoBuildingMan::GetNumBuildings()
{
	if (NULL == m_pAutoBuilding)
		return 0;
	
	return m_pAutoBuilding->GetNumBuildings();
}

// Get a building status
bool CAutoBuildingMan::GetBuildingStatus(int nIndex, MODELSTATUS& status)
{
	if (NULL == m_pAutoBuilding)
		return false;

	status =  m_pAutoBuilding->GetModelStatus(nIndex);

	return true;
}

// Get a up or down face vertices of a OBB
bool CAutoBuildingMan::GetFaceVerticesOfOBB(A3DOBB& obb, float* fCenter, POINT_FLOAT* ptFace, int nUpDown)
{
	A3DVECTOR3 vFaceCenter = obb.Center + nUpDown * obb.ExtY;
	*fCenter = vFaceCenter.y;

	A3DVECTOR3 vVertices[4];
	vVertices[0] = vFaceCenter - obb.ExtX - obb.ExtZ;
	vVertices[1] = vFaceCenter - obb.ExtX + obb.ExtZ;
	vVertices[2] = vFaceCenter + obb.ExtX + obb.ExtZ;
	vVertices[3] = vFaceCenter + obb.ExtX - obb.ExtZ;

	for (int i=0; i<4; i++)
	{
		ptFace[i].x = vVertices[i].x;
		ptFace[i].y = vVertices[i].z;
	}

	return true;
}

// Adjust models after terrain change
bool CAutoBuildingMan::AdjustModelsAfterTerrainChange()
{
	int nNumModels = GetNumBuildings();
	if (0 == nNumModels)
		return false;

	bool* bChecked = new bool[nNumModels];
	if (NULL == bChecked) 
		return false;
	memset(bChecked, 0, sizeof(bool)*nNumModels);

	int i;
	int nOldSelectedIndex = GetSelectedModelIndex();
	POINT_FLOAT ptDrop[4];
	float fDropCenter;
	A3DVECTOR3 vModelPos;
	float fTerrainHeight, fTrans;
	MODELSTATUS status;

	bool bBuildingMoved = false;

	// Check all houses first
	for (i=0; i<nNumModels; i++)
	{
		if (bChecked[i])
			continue;
		status = m_pAutoBuilding->GetModelStatus(i);
		if (status.dwAttribute == CAutoBuilding::MA_HOUSE)
		{
			SetSelectedModelIndex(i);
//			SelectContainedModels();
			
			// Get the drop model's four vertices on bottom face.
			GetFaceVerticesOfOBB(status.obbModel, &fDropCenter, ptDrop, -1);

			// Get terrain height below the drop model
			m_pAutoBuilding->GetBuildingPos(i, vModelPos);
			fTerrainHeight = m_pAutoHome->GetAutoTerrain()->GetPosHeight(vModelPos);

			if (fDropCenter <= fTerrainHeight)
			{
				fTrans = fTerrainHeight - fDropCenter;
				TranslateBuilding(i, A3DVECTOR3(0, fTrans, 0));
//				TranslateOthersInModel(A3DVECTOR3(0, fTrans, 0));
				bBuildingMoved = true;
			}
			bChecked[i] = true;
/*			for (j=0; i<m_aAutoSelectedModels.GetSize(); i++)
			{
				bChecked[m_aAutoSelectedModels[i]] = true;	
			}
*/		}
	}

	// Check other models
	for (i=0; i<nNumModels; i++)
	{
		if (bChecked[i])
			continue;
		status = m_pAutoBuilding->GetModelStatus(i);
		// Get the drop model's four vertices on bottom face.
		GetFaceVerticesOfOBB(status.obbModel, &fDropCenter, ptDrop, -1);

		// Get terrain height below the drop model
		m_pAutoBuilding->GetBuildingPos(i, vModelPos);
		fTerrainHeight = m_pAutoHome->GetAutoTerrain()->GetPosHeight(vModelPos);

		if (fDropCenter <= fTerrainHeight)
		{
			fTrans = fTerrainHeight - fDropCenter;
			TranslateBuilding(i, A3DVECTOR3(0, fTrans, 0));
			bBuildingMoved = true;
		}
		bChecked[i] = true;
	}

	delete[] bChecked;

	SetSelectedModelIndex(nOldSelectedIndex);

//	if (bBuildingMoved)
//		CheckAllModelsCollision();

	return true;
}

bool CAutoBuildingMan::Tick(DWORD dwTickTime)
{
	return true;
}
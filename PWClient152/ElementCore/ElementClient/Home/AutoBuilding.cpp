/*
 * FILE: AutoBuilding.cpp
 *
 * DESCRIPTION: Class for automaticlly generating building
 *
 * CREATED BY: Jiang Dalong, 2005/05/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */
#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "Render.h"
#include "AutoHome.h"
#include "AutoScene.h"
#include "EC_World.h"
#include "AutoBuilding.h"
#include "EC_ManOrnament.h"
#include "EC_HomeOrnament.h"
#include "EL_BuildingWithBrush.h"
#include "AutoSceneConfig.h"
#include "SoftGouraud.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoBuilding::CAutoBuilding(CAutoHome* pAutoHome)
{
	m_pAutoHome		= pAutoHome;
	m_pOrnamentMan	= pAutoHome->GetAutoScene()->GetECWorld()->GetOrnamentMan();
}

CAutoBuilding::~CAutoBuilding()
{
}

void CAutoBuilding::Release()
{
	DeleteAllBuildings();
}

void CAutoBuilding::DeleteAllBuildings()
{
	for (int i=0; i<m_aModelStatus.GetSize(); i++)
	{
		m_pOrnamentMan->ReleaseHomeOrnament(m_aModelStatus[i].dwUniqueID);
	}
	m_aModelStatus.RemoveAll();
}

bool CAutoBuilding::Render(A3DViewport* pA3DViewport)
{
	return true;
}

// Load model by ID
bool CAutoBuilding::LoadBuildingByID(DWORD dwTypeID, const A3DVECTOR3& vPos, float fRotateY)
{
	CAutoSceneConfig* pAutoSceneConfig = m_pAutoHome->GetAutoScene()->GetAutoSceneConfig();
	AUTOMODELTYPELIST type;
	if (!pAutoSceneConfig->GetAutoModelTypeListByID(dwTypeID, &type))
		return false;

	// Create building ID
	DWORD dwUniqueID = m_pAutoHome->GetAutoScene()->GenerateID();

	// Get building position and dir
	A3DVECTOR3 vPosWorld;
	m_pAutoHome->LocalToWorld(vPos, vPosWorld);
	A3DMATRIX4 matTM, matTrans, matRotate;
	matTrans.Translate(vPosWorld.x, vPosWorld.y, vPosWorld.z);
	matRotate.RotateY(fRotateY);
	matTM = matRotate * matTrans;

	// Load building
	if (!m_pOrnamentMan->LoadHomeOrnament(dwUniqueID, m_pAutoHome->GetAutoScene(), 
		type.strMoxFile, type.strChfFile, matTM))
		return false;

	// Store building attributes
	MODELSTATUS status;
	status.dwTypeID = dwTypeID;
	status.dwAttribute = type.dwAttribute;
	status.dwUniqueID = dwUniqueID;
	status.vPos = vPos;
	status.fRotateY = fRotateY;
	status.bInit = false;
	m_aModelStatus.Add(status);

	return true;
}

// Set model pos
bool CAutoBuilding::SetBuildingPosByIndex(int nIndex, const A3DVECTOR3& vPos)
{
	ASSERT (nIndex >= 0 && nIndex < m_aModelStatus.GetSize());

	CELBuildingWithBrush* pELBuilding = GetELBuilding(m_aModelStatus[nIndex].dwUniqueID);
	if (NULL == pELBuilding)
		return false;

	pELBuilding->SetPos(vPos);
	A3DVECTOR3 vPosLocal;
	m_pAutoHome->WorldToLocal(vPos, vPosLocal);
	m_aModelStatus[nIndex].vPos = vPosLocal;

	return true;
}

// Delete a building
bool CAutoBuilding::DeleteBuildingByIndex(int nIndex)
{
	ASSERT (nIndex >= 0 && nIndex < m_aModelStatus.GetSize());

	m_pOrnamentMan->ReleaseHomeOrnament(m_aModelStatus[nIndex].dwUniqueID);
	m_aModelStatus.RemoveAt(nIndex);
	
	return true;
}

// Translate building
bool CAutoBuilding::TranslateBuildingByIndex(int nIndex, const A3DVECTOR3& vTrans)
{
	ASSERT (nIndex >= 0 && nIndex < m_aModelStatus.GetSize());

	CELBuildingWithBrush* pELBuilding = GetELBuilding(m_aModelStatus[nIndex].dwUniqueID);
	if (NULL == pELBuilding)
		return false;

	pELBuilding->Move(vTrans);
	m_aModelStatus[nIndex].vPos += vTrans;
	A3DVECTOR3 vPos = m_aModelStatus[nIndex].vPos;

	return true;
}

// Rotate building
bool CAutoBuilding::RotateBuildingByIndex(int nIndex, float fRotateY)
{
	ASSERT (nIndex >= 0 && nIndex < m_aModelStatus.GetSize());

	CELBuildingWithBrush* pELBuilding = GetELBuilding(m_aModelStatus[nIndex].dwUniqueID);
	if (NULL == pELBuilding)
		return false;

	pELBuilding->RotateY(fRotateY);
	m_aModelStatus[nIndex].fRotateY += fRotateY;

	return true;
}

// Get ELBuilding by ID
CELBuildingWithBrush* CAutoBuilding::GetELBuilding(DWORD dwUniqueID)
{
	CECHomeOrnament* pHomeOrnament = m_pOrnamentMan->GetHomeOrnament(dwUniqueID);
	if (NULL == pHomeOrnament)
		return NULL;
	if (!pHomeOrnament->IsLoaded())
		return NULL;

	CELBuildingWithBrush* pELBuilding = pHomeOrnament->GetBuildingWithBrush();
	return pELBuilding;
}

// Get index by ID
int CAutoBuilding::GetIndexByID(DWORD dwUniqueID)
{
	int i;
	for (i=0; i<m_aModelStatus.GetSize(); i++)
	{
		if (dwUniqueID == m_aModelStatus[i].dwUniqueID)
			return i;
	}

	return -1;
}

// Set model pos
bool CAutoBuilding::SetBuildingPosByID(DWORD dwUniqueID, const A3DVECTOR3& vPos)
{
	int nIndex = GetIndexByID(dwUniqueID);
	if (-1 == nIndex)
		return false;

	if (!SetBuildingPosByIndex(nIndex, vPos))
		return false;

	return true;
}

// Delete a building
bool CAutoBuilding::DeleteBuildingByID(DWORD dwUniqueID)
{
	int nIndex = GetIndexByID(dwUniqueID);
	if (-1 == nIndex)
		return false;

	if (!DeleteBuildingByIndex(nIndex))
		return false;

	return true;
}

// Translate building
bool CAutoBuilding::TranslateBuildingByID(DWORD dwUniqueID, const A3DVECTOR3& vTrans)
{
	int nIndex = GetIndexByID(dwUniqueID);
	if (-1 == nIndex)
		return false;

	if (!TranslateBuildingByIndex(nIndex, vTrans))
		return false;

	return true;
}

// Rotate building
bool CAutoBuilding::RotateBuildingByID(DWORD dwUniqueID, float fRotateY)
{
	int nIndex = GetIndexByID(dwUniqueID);
	if (-1 == nIndex)
		return false;

	if (!RotateBuildingByIndex(nIndex, fRotateY))
		return false;

	return true;
}

// Set obb dir with building
bool CAutoBuilding::SetOBBDirWithBuilding(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_aModelStatus.GetSize());

	CELBuildingWithBrush* pELBuilding = GetELBuilding(m_aModelStatus[nIndex].dwUniqueID);
	if (NULL == pELBuilding)
		return false;

	A3DVECTOR3 vTemp, vDir;
	vDir = pELBuilding->GetDir();
	m_aModelStatus[nIndex].obbModel.XAxis = CrossProduct(A3DVECTOR3(0, 1, 0), vDir);
	m_aModelStatus[nIndex].obbModel.ZAxis = -vDir;
	m_aModelStatus[nIndex].obbModel.CompleteExtAxis();

	return true;
}

// Translate building obb
bool CAutoBuilding::TranslateOBB(int nIndex, const A3DVECTOR3& vTrans)
{
	ASSERT(nIndex >= 0 && nIndex < m_aModelStatus.GetSize());

	m_aModelStatus[nIndex].obbModel.Center += vTrans;

	return true;
}

// Rotate building obb
bool CAutoBuilding::RotateOBB(int nIndex, A3DMATRIX4& matRoate)
{
	ASSERT(nIndex >= 0 && nIndex < m_aModelStatus.GetSize());

	CELBuildingWithBrush* pELBuilding = GetELBuilding(m_aModelStatus[nIndex].dwUniqueID);
	if (NULL == pELBuilding)
		return false;

	A3DVECTOR3 vTemp, vDir;
	vTemp = m_aModelStatus[nIndex].obbModel.Center;
	m_aModelStatus[nIndex].obbModel.Center = vTemp * matRoate;
	vDir = pELBuilding->GetDir();
	m_aModelStatus[nIndex].obbModel.XAxis = CrossProduct(A3DVECTOR3(0, 1, 0), vDir);
	m_aModelStatus[nIndex].obbModel.ZAxis = -vDir;
	m_aModelStatus[nIndex].obbModel.CompleteExtAxis();

	return true;

}

// Ray trace model
int CAutoBuilding::RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vTracePos)
{
	CELBuildingWithBrush* pELBuilding;
	A3DVECTOR3 vHitPos, vNormal, vFinalTracePos;
	float fFraction;
	float fMinDis = 1.1f;
	int nSelectedIndex = -1;
	for (int i=0; i<m_aModelStatus.GetSize(); i++)
	{
		pELBuilding = GetELBuilding(m_aModelStatus[i].dwUniqueID);
		if (NULL == pELBuilding)
			continue;
		if (!(pELBuilding->RayTraceConvexHull(vecStart, vecDelta, vTracePos, &fFraction, vNormal)))
			continue;
		if (fFraction < fMinDis)
		{
			fMinDis = fFraction;
			nSelectedIndex = i;
			vFinalTracePos = vTracePos;
		}
	}

	vTracePos = vFinalTracePos;

	return nSelectedIndex;
}

// Get building position
bool CAutoBuilding::GetBuildingPos(int nIndex, A3DVECTOR3& vPos)
{
	ASSERT(nIndex >= 0 && nIndex < m_aModelStatus.GetSize());

	DWORD dwUniqueID = m_aModelStatus[nIndex].dwUniqueID;
	CECHomeOrnament* pHomeOrnament = m_pOrnamentMan->GetHomeOrnament(dwUniqueID);
	CELBuildingWithBrush* pELBuilding = pHomeOrnament->GetBuildingWithBrush();
	if (pHomeOrnament->IsLoaded() && NULL != pELBuilding)
	{
		vPos = pELBuilding->GetPos();
		return true;
	}

	return false;
}

// Calculate building light
bool CAutoBuilding::CalculateBuildingColor(int nIndex)
{
	DWORD dwUniqueID = GetModelStatus(nIndex).dwUniqueID;

	CECHomeOrnament* pHomeOrnament = GetOrnamentMan()->GetHomeOrnament(dwUniqueID);
	if (NULL == pHomeOrnament)
		return false;
	CELBuildingWithBrush* pELBuilding = pHomeOrnament->GetBuildingWithBrush();
	if (pHomeOrnament->IsLoaded() && NULL != pELBuilding)
	{
		// Calculate building light, test
		CalculateBuildingColor(pELBuilding);
		return true;
	}

	return false;
}

// Calculate building light
bool CAutoBuilding::CalculateBuildingColor(CELBuildingWithBrush* pELBuilding)
{
	// Calculate building light, test
	CAutoSceneConfig* pAutoSceneConfig = m_pAutoHome->GetAutoScene()->GetAutoSceneConfig();
	CSoftGouraud sg;
	sg.Create(m_pAutoHome, pAutoSceneConfig->GetSunDir(),
		pAutoSceneConfig->GetSunColorDay(), pAutoSceneConfig->GetAmbientDay(),
		pAutoSceneConfig->GetSunColorNight(), pAutoSceneConfig->GetAmbientNight());
	sg.CalculateBuildingVertsColor(pELBuilding);
	pELBuilding->SetLitFlag(true);
	return true;
}

// Check building status
bool CAutoBuilding::CheckBuildingStatus()
{
	int i;
	CECHomeOrnament* pHomeOrnament;
	CELBuildingWithBrush* pELBuilding;
	for (i=0; i<m_aModelStatus.GetSize(); i++)
	{
		if (m_aModelStatus[i].bInit)
			continue;
		pHomeOrnament = m_pOrnamentMan->GetHomeOrnament(m_aModelStatus[i].dwUniqueID);
		pELBuilding = pHomeOrnament->GetBuildingWithBrush();
		if (pHomeOrnament->IsLoaded() && NULL != pELBuilding)
		{
			// Calculate building light, test
			CalculateBuildingColor(pELBuilding);

			// Get building obb
			A3DVECTOR3 vOldDir = pELBuilding->GetDir();
			pELBuilding->SetDirAndUp(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1, 0));
			m_aModelStatus[i].obbModel.Build(pELBuilding->GetModelAABB());
			pELBuilding->SetDirAndUp(vOldDir, A3DVECTOR3(0, 1, 0));
			SetOBBDirWithBuilding(i);
			m_aModelStatus[i].bInit = true;
		}
	}
	
	return true;
}

bool CAutoBuilding::Tick(DWORD dwTickTime)
{
	if (!CheckBuildingStatus())
		return false;

	return true;
}
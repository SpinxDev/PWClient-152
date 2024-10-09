//-----------------------------------------
//FileName: SofGouraud.cpp
//
//Project:SofGouraud.cpp
//
//Desc:
//
//Created by:Liyi 2005/5/11
//-----------------------------------------

#include "SoftGouraud.h"

#include "AutoHome.h"
#include "AutoBuilding.h"
#include "AutoTerrain.h"

#include "Render.h"
#include "AutoSceneFunc.h"
#include "EL_BuildingWithBrush.h"
#include <A3DMacros.h>
#include <A3DDevice.h>
#include <A3DFrame.h>
#include <A3DLitModel.h>



CSoftGouraud::CSoftGouraud()
{
	Clear();
}

CSoftGouraud::~CSoftGouraud()
{
	Destroy();
}

void CSoftGouraud::Clear()
{
	m_pAutoHome = NULL;

	m_vSunDir.Clear();
	m_vSunColorDay.Clear();
	m_vSunColorNight.Clear();
	m_vAmbientColorDay.Clear();
	m_vAmbientColorNight.Clear();

	m_pTerrainVerts = NULL;
	
	m_nGridX = 0;
	m_nGridZ = 0;
	
	m_nTerrainVertsNum = 0;

	m_pColorBufDay		= NULL;
	m_pColorBufNight	= NULL;
}

bool CSoftGouraud::Create(CAutoHome* i_pAutoHome, const A3DVECTOR3 &i_vSunDir,
		 DWORD i_dwSunColor, DWORD i_dwAmbient, DWORD i_dwSunColorNight, DWORD i_dwAmbientNight)
{
	if( i_pAutoHome == NULL)
		return false;

	m_pAutoHome = i_pAutoHome;

	m_vSunDir = -i_vSunDir;

	float r, g ,b;
	r = A3DCOLOR_GETRED(i_dwAmbient) / 255.0f;
	g = A3DCOLOR_GETGREEN(i_dwAmbient) / 255.0f;
	b = A3DCOLOR_GETBLUE(i_dwAmbient) / 255.0f;
	m_vAmbientColorDay.Set(r, g, b);
	
	r = A3DCOLOR_GETRED(i_dwSunColor) / 255.0f;
	g = A3DCOLOR_GETGREEN(i_dwSunColor) / 255.0f;
	b = A3DCOLOR_GETBLUE(i_dwSunColor) / 255.0f;
	m_vSunColorDay.Set(r, g, b);
	
	r = A3DCOLOR_GETRED(i_dwAmbientNight) / 255.0f;
	g = A3DCOLOR_GETGREEN(i_dwAmbientNight) / 255.0f;
	b = A3DCOLOR_GETBLUE(i_dwAmbientNight) / 255.0f;
	m_vAmbientColorNight.Set(r, g, b);
	
	r = A3DCOLOR_GETRED(i_dwSunColorNight) / 255.0f;
	g = A3DCOLOR_GETGREEN(i_dwSunColorNight) / 255.0f;
	b = A3DCOLOR_GETBLUE(i_dwSunColorNight) / 255.0f;
	m_vSunColorNight.Set(r,g,b);

	m_vAmbientColorDay *= 0.5f;
	m_vSunColorDay *= 0.5f;
	m_vAmbientColorNight *= 0.5f;
	m_vSunColorNight *= 0.5f;

	return true;
}

void CSoftGouraud::Destroy()
{
	ReleaseTerrainStream();
//	ReleaseBuildingStream();

	//µ¥Ò»
	ReleaseBuildingVB();

	SAFERELEASE(m_pColorBufDay);
	SAFERELEASE(m_pColorBufNight);
}

bool CSoftGouraud::CreateTerrainStream()
{
	// try to release old one
	ReleaseTerrainStream();

	CAutoTerrain* pTerrain = m_pAutoHome->GetAutoTerrain();
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();

	m_nGridX = pTerrain->GetHeightMap()->iWidth - 1;
	m_nGridZ = pTerrain->GetHeightMap()->iHeight - 1;



	m_nTerrainVertsNum = (m_nGridX + 1) * (m_nGridZ + 1);
	m_pTerrainVerts	= new A3DVERTEX[m_nTerrainVertsNum];

	CAutoTerrain::HEIGHTMAP * pHeightMap = pTerrain->GetHeightMap();
	int nVertIndex = 0;
	A3DVECTOR3 vecTerrain;
	A3DVECTOR3 vecNormal;
	for(int i=0; i<=m_nGridZ; i++)
	{
		for(int j=0; j<=m_nGridX; j++)
		{
			nVertIndex = i * (m_nGridX + 1) + j;

			vecTerrain = A3DVECTOR3(0);
			vecNormal = pHeightMap->aNormals[nVertIndex];		
			m_pTerrainVerts[nVertIndex] = A3DVERTEX(vecTerrain,	vecNormal, 0.0f, 0.0f);
			
		}
	}

	return true;
}


void CSoftGouraud::ReleaseTerrainStream()
{
	SAFERELEASE(m_pTerrainVerts);
}
/*
void CSoftGouraud::CreateBuildingStream()
{
	//
	ReleaseBuildingStream();

	//building
	AArray<CELBuilding*, CELBuilding*>* pAryBuiling = m_pAutoHome->GetAutoBuilding()->GetBuildings();
	CELBuilding* pBuiling;
	A3DLitModel* pLitModel;
	A3DLitMesh* pLitMesh;
	A3DLMVERTEX* pLitVertex;
	WORD* pLitIndex;

	Vertex_t* pMeshVB;
//	int* pMeshIB;

	int nNumMeshVerts;
//	int nNumMeshFaces;
	int n;

	A3DVECTOR3 vPos;
	int i, j;

	A3DMATRIX4 matModelWorld;
	A3DMATRIX4 matRotate;

	A3DVECTOR3* pLitNormal;
	A3DVECTOR3 vNormal;

	for( i = 0; i < pAryBuiling->GetSize(); i++)
	{
		pBuiling = *pAryBuiling->ElementAt(i);
		pLitModel = pBuiling->GetA3DLitModel();
		matModelWorld = pBuiling->GetAbsoluteTM();
		
		matRotate = matModelWorld;
		matRotate.SetRow(3,A3DVECTOR3(0.0f));
		
		//matTransInvModelWorld = matModelWorld.GetInverseTM().GetTranspose();
	
		for( j = 0; j < pLitModel->GetNumMeshes(); j++)
		{
			pLitMesh = pLitModel->GetMesh(j);
			nNumMeshVerts = pLitMesh->GetNumVerts();
//			nNumMeshFaces = pLitMesh->GetNumFaces();
			pLitVertex = pLitMesh->GetVerts();
			pLitIndex = pLitMesh->GetIndices();
			pLitNormal = pLitMesh->GetNormals();

			pMeshVB = new Vertex_t[nNumMeshVerts];
//			pMeshIB = new int[nNumMeshFaces * 3];
			
			for( n = 0; n< nNumMeshVerts; n++)
			{
				vPos.Set(pLitVertex[n].pos.x, pLitVertex[n].pos.y, pLitVertex[n].pos.z);
				//pMeshVB[n].vPos = vPos * matModelWorld;

				vNormal.Set(pLitNormal[n].x, pLitNormal[n].y, pLitNormal[n].z);

				pMeshVB[n].vNormal = vNormal * matRotate;
				pMeshVB[n].vNormal.Normalize();
			}

			m_vecBuildingVertexNum.push_back(nNumMeshVerts);

// 			for( n = 0; n < nNumMeshFaces*3; n++)
// 			{
// 				pMeshIB[n] = pLitIndex[n];
// 			}

			m_vecBuildingVertexBuffer.push_back(pMeshVB);
// 			m_vecBuildingIndexBuffer.push_back(pMeshIB);
// 			m_vecBuildingFacesCount.push_back(nNumMeshFaces);
		}
	}
}

void CSoftGouraud::ReleaseBuildingStream()
{
	VertsVectorIter_t iter;
	VertsVectorIter_t iterEnd;

	iter = m_vecBuildingVertexBuffer.begin();
	iterEnd = m_vecBuildingVertexBuffer.end();

	for( ; iter != iterEnd; ++iter)
	{
		if(*iter)
			delete[] *iter;
	}
	m_vecBuildingVertexBuffer.clear();

// 	abase::vector<int*>::iterator iter2;
// 	abase::vector<int*>::iterator iterEnd2;

// 	iter2 = m_vecBuildingIndexBuffer.begin();
// 	iterEnd2 = m_vecBuildingIndexBuffer.end();
// 
// 	for(; iter2 != iterEnd2; ++iter2)
// 	{
// 		if(*iter2)
// 			delete[] *iter2;
// 	}

	m_vecBuildingVertexNum.clear();
// 	m_vecBuildingIndexBuffer.clear();
// 	m_vecBuildingFacesCount.clear();
}
*/
void CSoftGouraud::CalculateSceneVertsColor(DWORD** pColorDay, DWORD** pColorNight)
{
	if(!CalculateTerrainVertsColor())
		ASSERT("CSoftGouraud::CalculateSceneVertsColor(), Failed to Calculate Terrain Verts Color!");

	*pColorDay = m_pColorBufDay;
	*pColorNight = m_pColorBufNight;

}

bool CSoftGouraud::CalculateTerrainVertsColor()
{
	CreateTerrainStream();

	if( m_pTerrainVerts == NULL)
		return false;

	m_pColorBufDay = new A3DCOLOR [m_nTerrainVertsNum];
	m_pColorBufNight = new A3DCOLOR[m_nTerrainVertsNum];

	for( int i = 0 ; i < m_nTerrainVertsNum; i++)
	{
		A3DVECTOR3 vNormal(m_pTerrainVerts[i].nx, m_pTerrainVerts[i].ny, m_pTerrainVerts[i].nz);
		float fDot = 0.0f;

		fDot = DotProduct( m_vSunDir, vNormal);
		if( fDot < 0.0f)
		{
			fDot = 0.0f;
		}
	

		A3DVECTOR3 vClrDay= fDot * m_vSunColorDay + m_vAmbientColorDay;

		
		A3DCOLORVALUE clrDay(vClrDay.x, vClrDay.y, vClrDay.z, 1.0f);

		A3DVECTOR3 vClrNight = fDot * m_vSunColorNight + m_vAmbientColorNight;

		A3DCOLORVALUE clrNight(vClrNight.x, vClrNight.y, vClrNight.z, 1.0f);

		m_pColorBufDay[i] = clrDay.ToRGBAColor();
		m_pColorBufNight[i] = clrNight.ToRGBAColor();
	}

	return true;
}
/*
bool CSoftGouraud::CalculateBuildingsVertsColor()
{
	CreateBuildingStream();

	for( int i = 0; i < (int)m_vecBuildingVertexBuffer.size(); i++)
	{
		for( int j = 0; j < m_vecBuildingVertexNum[i]; j++)
		{
			Vertex_t* pVertex = m_vecBuildingVertexBuffer[i];

			float fDot = 0.0f;
			fDot = DotProduct( m_vSunDir, pVertex[j].vNormal);
			if( fDot < 0.0f)
				fDot = 0.0f;
			pVertex[j].vClrDay = fDot * m_vSunColorDay + m_vAmbientColorDay;
			pVertex[j].vClrNight = fDot * m_vSunColorNight + m_vAmbientColorNight;

		}
	}

	FillBuildingsDiffColor();

	return true;
}

void CSoftGouraud::FillBuildingsDiffColor()
{
	//building
	AArray<CELBuilding*, CELBuilding*>* pAryBuiling = m_pAutoHome->GetAutoBuilding()->GetBuildings();
	CELBuilding* pBuiling;
	A3DLitModel* pLitModel;
	A3DLitMesh* pLitMesh;

	A3DCOLOR* pDayColor;
	A3DCOLOR* pNightColor;

	int nNumMeshVerts;
	int n;	
	int i, j;

	int nMeshCount = 0;

	for( i = 0; i < pAryBuiling->GetSize(); i++)
	{
		pBuiling = *pAryBuiling->ElementAt(i);
		pLitModel = pBuiling->GetA3DLitModel();
		

		for( j = 0; j < pLitModel->GetNumMeshes(); j++)
		{

			pLitMesh = pLitModel->GetMesh(j);
			nNumMeshVerts = pLitMesh->GetNumVerts();
			pDayColor = pLitMesh->GetDayColors();
			pNightColor = pLitMesh->GetNightColors();
			
			for( n = 0; n< nNumMeshVerts; n++)
			{
			
				A3DCOLORVALUE clrDiff (m_vecBuildingVertexBuffer[nMeshCount][n].vClrDay.x,
										m_vecBuildingVertexBuffer[nMeshCount][n].vClrDay.y,
										m_vecBuildingVertexBuffer[nMeshCount][n].vClrDay.z,
										1.0f);

				A3DCOLORVALUE clrDiffNight(m_vecBuildingVertexBuffer[nMeshCount][n].vClrNight.x,
										m_vecBuildingVertexBuffer[nMeshCount][n].vClrNight.y,
										m_vecBuildingVertexBuffer[nMeshCount][n].vClrNight.z,
										1.0f);
			
// 				clrDiff.Clamp();
// 				clrDiffNight.Clamp();
				
				pDayColor[n] = clrDiff.ToRGBAColor();

				pNightColor[n] = clrDiffNight.ToRGBAColor();
				
			}

			pLitMesh->UpdateColors();
			nMeshCount ++;
		}
	}
}
*/

bool CSoftGouraud::CalculateBuildingVertsColor(CELBuildingWithBrush* pELBuilding)
{
	if (NULL == pELBuilding)
		return false;

	CreateBuildingVB(pELBuilding);

	int i, j;

	//Calculate Color
	for( i = 0; i < (int)m_vecVerts.size(); i++)
	{
		for( j = 0; j < m_vecVertsNum[i]; j++)
		{
			Vertex_t* pVertex = m_vecVerts[i];

			float fDot = 0.0f;
			fDot = DotProduct( m_vSunDir, pVertex[j].vNormal);
			if( fDot < 0.0f)
				fDot = 0.0f;
			pVertex[j].vClrDay = fDot * m_vSunColorDay + m_vAmbientColorDay;
			pVertex[j].vClrNight = fDot * m_vSunColorNight + m_vAmbientColorNight;

		}
	}


	FillBuildingDiffColor(pELBuilding);
	
	//Release VB
	ReleaseBuildingVB();

	return true;
}

void CSoftGouraud::CreateBuildingVB(CELBuildingWithBrush* pELBuilding)
{
	ReleaseBuildingVB();

	A3DLitModel* pLitModel = pELBuilding->GetA3DLitModel();

	A3DMATRIX4 matModelWorld = pELBuilding->GetAbsoluteTM();
	A3DMATRIX4 matRotate = matModelWorld;
	matRotate.SetRow(3,A3DVECTOR3(0.0f));

	int i, n;

	//Create Verts
	for( i = 0; i < pLitModel->GetNumMeshes(); i++)
	{
		A3DLitMesh* pLitMesh = pLitModel->GetMesh(i);
		int nNumVerts = pLitMesh->GetNumVerts();
		A3DVECTOR3* pListNormal = pLitMesh->GetNormals();

		Vertex_t* pMeshVB = new Vertex_t[nNumVerts];
		for( n = 0; n < nNumVerts; n++)
		{
			pMeshVB[n].vNormal = pListNormal[n] * matRotate;
			pMeshVB[n].vNormal.Normalize();
		}

		m_vecVertsNum.push_back(nNumVerts);
		m_vecVerts.push_back(pMeshVB);
	}

}

void CSoftGouraud::ReleaseBuildingVB()
{
	//Release Buffer
	VertsVectorIter_t iter;
	VertsVectorIter_t iterEnd;

	iter = m_vecVerts.begin();
	iterEnd = m_vecVerts.end();

	for( ; iter != iterEnd; ++iter)
	{
		if(*iter)
			delete[] *iter;
	}
	m_vecVerts.clear();
	m_vecVertsNum.clear();
}

void CSoftGouraud::FillBuildingDiffColor(CELBuildingWithBrush* pELBuilding)
{
	A3DLitModel* pLitModel = pELBuilding->GetA3DLitModel();
	
	//Fill Verts Color
	for(int i= 0; i < pLitModel->GetNumMeshes(); i++)
	{
		A3DLitMesh* pLitMesh = pLitModel->GetMesh(i);
		int nNumVerts = pLitMesh->GetNumVerts();
		A3DCOLOR* pDayColor = pLitMesh->GetDayColors();
		A3DCOLOR* pNightColor = pLitMesh->GetNightColors();
		for(int n = 0; n< nNumVerts; n++)
		{
		
			A3DCOLORVALUE clrDiff (m_vecVerts[i][n].vClrDay.x,
									m_vecVerts[i][n].vClrDay.y,
									m_vecVerts[i][n].vClrDay.z,
									1.0f);

			A3DCOLORVALUE clrDiffNight(m_vecVerts[i][n].vClrNight.x,
									m_vecVerts[i][n].vClrNight.y,
									m_vecVerts[i][n].vClrNight.z,
									1.0f);
		

			
			pDayColor[n] = clrDiff.ToRGBAColor();

			pNightColor[n] = clrDiffNight.ToRGBAColor();
			
		}

		pLitMesh->UpdateColors();
		
	}
}

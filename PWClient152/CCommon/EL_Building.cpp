/*
 * FILE: EL_Building.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2005/1/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTypes.h"
#include "A3DFuncs.h"
#include "A3DFrame.h"
#include "A3DMesh.h"
#include "A3DLitModel.h"
#include "AFile.h"
#include "AFileImage.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DCollision.h"
#include "A3DTexture.h"
#include "A3DStream.h"
#include "A3DTextureMan.h"

#include "EL_Building.h"
#include "ConvexHullData.h"
#include "CDWithCH.h"
#include "aabbcd.h"

CELBuilding::CELBuilding()
{
	m_pA3DDevice		= NULL;
	m_pA3DLitModel		= NULL;

	m_bCollideOnly		= false;
	m_nNumHull			= 0;
	m_dwTimeLived		= 0;
}

CELBuilding::~CELBuilding()
{
}

bool CELBuilding::Init(A3DDevice * pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;
	m_dwTimeLived = 0;
	return true;
}

bool CELBuilding::Release()
{
	ReleaseHullMeshList();
	ReleaseConvexHullData();

	if( m_pA3DLitModel )
	{
		m_pA3DLitModel->Release();
		delete m_pA3DLitModel;
		m_pA3DLitModel = NULL;
	}

	m_nNumHull = 0;
	m_dwTimeLived = 0;
	m_bCollideOnly = false;
	return true;
}

bool CELBuilding::ReleaseHullMeshList()
{
	m_HullMeshList.clear();
	return true;
}

bool CELBuilding::ReleaseConvexHullData()
{
	for(int i=0; i<m_nNumHull; i++)
	{
		if( m_ConvexHullData[i] )
		{
			delete m_ConvexHullData[i];
			m_ConvexHullData[i] = NULL;
		}
	}

	m_ConvexHullData.clear();
	m_nNumHull = 0;
	return true;
}

bool CELBuilding::Tick(DWORD dwDeltaTime)
{
	// no alpha now, for on GF2 alpha mesh cost a lot.
	if( m_pA3DDevice && !m_pA3DDevice->TestPixelShaderVersion(1, 1) )
		return true;

	if( m_dwTimeLived > BUILDING_FADE_IN_TIME )
	{
		m_dwTimeLived += dwDeltaTime;
		return true;
	}

	m_dwTimeLived += dwDeltaTime;
	int nAlpha = m_dwTimeLived * 255 / BUILDING_FADE_IN_TIME;
	if( nAlpha > 255 )
		nAlpha = 255;

	// now set alpha to all building meshes.
	if( m_pA3DLitModel )
	{
		int nNumMeshes = m_pA3DLitModel->GetNumMeshes();
		for(int i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = m_pA3DLitModel->GetMesh(i);
			if( pMesh )
				pMesh->SetAlphaValue(nAlpha);
		}
	}

	return true;
}

bool CELBuilding::Render(A3DViewport * pViewport, bool bRenderAlpha)
{
	if( m_pA3DLitModel )
		m_pA3DLitModel->Render(pViewport);

	return true;
}

bool CELBuilding::RenderConvexHull(A3DViewport * pViewport)
{
	for(int i=0; i<m_nNumHull; i++)
	{
		if(m_ConvexHullData[i]->GetFlags() & CHDATA_NPC_CANNOT_PASS)
			m_ConvexHullData[i]->Render(m_pA3DDevice->GetA3DEngine()->GetA3DFlatCollector(),FALSE, NULL,0x00000000, 0xa0ff0000);
		else
			m_ConvexHullData[i]->Render(m_pA3DDevice->GetA3DEngine()->GetA3DFlatCollector());
	}

	return true;
}

// save hull's meshes list
bool CELBuilding::SaveHullMeshList(AFile * pFileToSave)
{
	DWORD dwWrittenLen;

	for(int i=0; i<m_nNumHull; i++)
	{
		int nNumMesh = m_HullMeshList[i].size();
		if( !pFileToSave->Write(&nNumMesh, sizeof(int), &dwWrittenLen) )
			return false;

		for(int j=0; j<nNumMesh; j++)
		{
			int idMesh = m_HullMeshList[i][j];
			if( !pFileToSave->Write(&idMesh, sizeof(int), &dwWrittenLen) )
				return false;
		}
	}
	
	return true;
}

// load hull's meshes list
bool CELBuilding::LoadHullMeshList(AFile * pFileToLoad)
{
	DWORD dwReadLen;

	for(int i=0; i<m_nNumHull; i++)
	{
		int nNumMesh = 0;
		if( !pFileToLoad->Read(&nNumMesh, sizeof(int), &dwReadLen) )
			return false;

		MESHLIST meshList;
		for(int j=0; j<nNumMesh; j++)
		{
			int idMesh = 0;
			if( !pFileToLoad->Read(&idMesh, sizeof(int), &dwReadLen) )
				return false;
			meshList.push_back(idMesh);
		}

		m_HullMeshList.push_back(meshList);
	}

	return true;
}

// Load convex hull data
bool CELBuilding::LoadConvexHullData(AFile * pFileToLoad)
{
	int i;
	for(i=0; i<m_nNumHull; i++)
	{
		CConvexHullData * pHullData = new CConvexHullData();
		if( !pHullData->LoadBinaryData(pFileToLoad) )
			return false;
		m_ConvexHullData.push_back(pHullData);
	}

	return true;
}

// Save convex hull data
bool CELBuilding::SaveConvexHullData(AFile * pFileToSave)
{
	int i;
	for(i=0; i<m_nNumHull; i++)
	{
		if( !m_ConvexHullData[i]->SaveBinaryData(pFileToSave) )
			return false;
	}

	return true;
}

// Load and Save
bool CELBuilding::Save(const char * szFileName)
{
	AFile fileToSave;

	if( !fileToSave.Open(szFileName, AFILE_CREATENEW | AFILE_BINARY) )
		return false;

	if( !Save(&fileToSave) )
		return false;

	fileToSave.Close();
	return true;
}

bool CELBuilding::Save(AFile * pFileToSave)
{
	DWORD dwWrittenLen;

	DWORD version = ELBUILDING_VERSION;
	if( !pFileToSave->Write(&version, sizeof(DWORD), &dwWrittenLen) )
		return false;

	if( !pFileToSave->Write(&m_bCollideOnly, sizeof(bool), &dwWrittenLen) )
		return false;

	if( !m_pA3DLitModel->Save(pFileToSave) )
		return false;

	int nNumHull = m_ConvexHullData.size();
	if( !pFileToSave->Write(&nNumHull, sizeof(int), &dwWrittenLen) )
		return false;

	if( !SaveHullMeshList(pFileToSave) )
		return false;

	if( !SaveConvexHullData(pFileToSave) )
		return false;

	return true;
}

bool CELBuilding::Load(A3DDevice * pA3DDevice, const char * szFileName)
{
	AFileImage fileToLoad;

	if( !fileToLoad.Open(szFileName, AFILE_OPENEXIST | AFILE_BINARY) )
		return false;

	if( !Load(pA3DDevice, &fileToLoad) )
		return false;

	fileToLoad.Close();
	return true;
}

bool CELBuilding::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	m_pA3DDevice = pA3DDevice;

	// try to release old ones.
	Release();

	DWORD dwReadLen;
	// first test if the file version is correct.
	DWORD	version;

	if( !pFileToLoad->Read(&version, sizeof(DWORD), &dwReadLen) )
		return false;

	bool bCollideOnly = false;
	if( version == 0x80000001 )
	{
		if( !pFileToLoad->Read(&bCollideOnly, sizeof(bool), &dwReadLen) )
			return false;
	}
	else
	{
		pFileToLoad->Seek(-int(sizeof(DWORD)), AFILE_SEEK_CUR);
	}

	// then load lit model and convex hull data
	m_pA3DLitModel = new A3DLitModel();
	if( !m_pA3DLitModel->Load(pA3DDevice, pFileToLoad) )
		return false;

	if( !pFileToLoad->Read(&m_nNumHull, sizeof(int), &dwReadLen) )
		return true;	//	For compatability with old version

	if( !LoadHullMeshList(pFileToLoad) )
		return false;

	if( !LoadConvexHullData(pFileToLoad) )
		return false;

	// now clear the empty hulls.
	int i;
	for(i=0; i<(int)m_ConvexHullData.size(); )
	{
		if( m_ConvexHullData[i]->IsEmpty() )
		{
			m_ConvexHullData.erase(m_ConvexHullData.begin() + i, m_ConvexHullData.begin() + i + 1);
			m_HullMeshList.erase(m_HullMeshList.begin() + i, m_HullMeshList.begin() + i + 1);
		}
		else
			i ++;
	}
	m_nNumHull = m_ConvexHullData.size();

	SetCollideOnly(bCollideOnly);

	// now call once tick to make alpha value of all meshes correct.
#ifndef _MODEDITOREX
	Tick(0);
#endif
	return true;
}

bool CELBuilding::SaveCHFDataForEditor(AFile * pFileToSave)
{
	DWORD dwWrittenLen;

	int nNumHull = m_ConvexHullData.size();
	if( !pFileToSave->Write(&nNumHull, sizeof(int), &dwWrittenLen) )
		return false;

	if( !SaveHullMeshList(pFileToSave) )
		return false;

	if( !SaveConvexHullData(pFileToSave) )
		return false;	

	return true;
}

bool CELBuilding::LoadCHFDataForEditor(AFile * pFileToRead)
{
	DWORD dwReadLen;
	if( !pFileToRead->Read(&m_nNumHull, sizeof(int), &dwReadLen) )
		return true;	//	For compatability with old version

	if( !LoadHullMeshList(pFileToRead) )
		return false;

	if( !LoadConvexHullData(pFileToRead) )
		return false;	
	return true;
}

// Load from seperate file
bool CELBuilding::LoadFromMOXAndCHF(A3DDevice * pA3DDevice, const char * szFileMox, const char * szFileCHF)
{
	m_pA3DDevice = pA3DDevice;

	// try to release old ones.
	Release();
 
	// then load lit model and convex hull data
	m_pA3DLitModel = new A3DLitModel();
	if( !m_pA3DLitModel->LoadFromMox(pA3DDevice, szFileMox) )
		return false;

	AFileImage fileToLoad;
	if( !fileToLoad.Open(szFileCHF, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		// current we allow .chf missing
		fileToLoad.Close();
		return true;
	}

	DWORD dwReadLen;
	if( !fileToLoad.Read(&m_nNumHull, sizeof(int), &dwReadLen) )
		return false;

	if( !LoadHullMeshList(&fileToLoad) )
		return false;

	if( !LoadConvexHullData(&fileToLoad) )
		return false;

	// now clear the empty hulls.
	int i;
	for(i=0; i<(int)m_ConvexHullData.size(); )
	{
		if( m_ConvexHullData[i]->IsEmpty() )
		{
			m_ConvexHullData.erase(m_ConvexHullData.begin() + i, m_ConvexHullData.begin() + i + 1);
			m_HullMeshList.erase(m_HullMeshList.begin() + i, m_HullMeshList.begin() + i + 1);
		}
		else
			i ++;
	}
	m_nNumHull = m_ConvexHullData.size();

	fileToLoad.Close();
	return true;
}

// Adjust scale factor
bool CELBuilding::SetScale(float vScale)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->SetScale(vScale, vScale, vScale);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;

	return TransformHull(tm);
}

// Direct Set the position and orientation methods
bool CELBuilding::SetPos(const A3DVECTOR3& vecPos)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->SetPos(vecPos);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;
	
	return TransformHull(tm);
}

bool CELBuilding::SetDirAndUp(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->SetDirAndUp(vecDir, vecUp);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;
	
	return TransformHull(tm);
}

// Relatively adjust orientation and position methods
bool CELBuilding::Move(const A3DVECTOR3& vecDeltaPos)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->Move(vecDeltaPos);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;

	return TransformHull(tm);
}

bool CELBuilding::RotateX(float vDeltaRad)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->RotateX(vDeltaRad);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;

	return TransformHull(tm);
}

bool CELBuilding::RotateY(float vDeltaRad)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->RotateY(vDeltaRad);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;

	return TransformHull(tm);
}

bool CELBuilding::RotateZ(float vDeltaRad)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->RotateZ(vDeltaRad);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;

	return TransformHull(tm);
}

A3DVECTOR3 CELBuilding::GetPos()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetPos();
	
	return A3DVECTOR3(0.0f);
}

A3DVECTOR3 CELBuilding::GetDir()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetDir();
	
	return A3DVECTOR3(0.0f, 0.0f, 1.0f);
}

A3DVECTOR3 CELBuilding::GetUp()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetUp();
	
	return A3DVECTOR3(0.0f, 1.0f, 0.0f);
}

A3DVECTOR3 CELBuilding::GetScale()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetScale();
	
	return A3DVECTOR3(1.0f, 1.0f, 1.0f);
}

A3DMATRIX4 CELBuilding::GetAbsoluteTM()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetAbsoluteTM();
	
	return IdentityMatrix();
}

A3DAABB CELBuilding::GetModelAABB()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetModelAABB();
	
	return A3DAABB(A3DVECTOR3(0.0f), A3DVECTOR3(0.0f));
}

bool CELBuilding::TransformHull(const A3DMATRIX4& tm)
{
	int i;
	for(i=0; i<m_nNumHull; i++)
	{
		CConvexHullData * pHullData = m_ConvexHullData[i];

		pHullData->Transform(tm);
	}

	return true;
}

bool CELBuilding::RayTraceAABB(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal)
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->RayTraceAABB(vecStart, vecDelta, vecHitPos, pvFraction, vecNormal);

	return false;
}

bool CELBuilding::RayTraceMesh(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal)
{
	if( RayTraceAABB(vecStart,vecDelta,vecHitPos,pvFraction,vecNormal))
		return m_pA3DLitModel->RayTraceMesh(vecStart, vecDelta, vecHitPos, pvFraction, vecNormal);

	return false;
}

bool CELBuilding::RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pTraceCHData, const CFace* & pTraceFace)
{

	//*
	// ÏÈºÍAABBÅö×²
	const A3DAABB& aabb=m_pA3DLitModel->GetModelAABB();
	if(!CLS_RayToAABB3(vecStart,vecDelta,aabb.Mins,aabb.Maxs,vecHitPos,pvFraction,vecNormal))
		return false;
	//*/
	
	const CFace* pCDFace=NULL, *pRealCDFace=NULL;
	float fraction, fMinFraction=1.0f;
	A3DVECTOR3 vCollidePos;

	for(int i=0;i<m_nNumHull;i++)
	{
		CConvexHullData* pCHData = m_ConvexHullData[i];
		if(RayIntersectWithCH(vecStart,vecDelta,*pCHData,&pCDFace,vCollidePos,fraction))
		{
			if(fraction<fMinFraction)
			{
				fMinFraction=fraction;
				pRealCDFace=pCDFace;
				vecHitPos=vCollidePos;
				pTraceCHData=pCHData;
			}
		}
	}

	// Collision occurs!
	if(pRealCDFace)
	{
		*pvFraction=fMinFraction;
		vecNormal=pRealCDFace->GetNormal();
		pTraceFace = pRealCDFace;
		return true;
	}
	return false;
}

bool CELBuilding::RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pTraceCHData)
{
	const CFace* pTraceFace = NULL;
	return RayTraceConvexHull(vecStart, vecDelta, vecHitPos, pvFraction, vecNormal, pTraceCHData, pTraceFace);
}

bool CELBuilding::RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal)
{
	CConvexHullData* pTraceCHData=NULL;
	return RayTraceConvexHull(vecStart, vecDelta, vecHitPos, pvFraction, vecNormal, pTraceCHData);
}

bool CELBuilding::SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pCDCHData, const CFace* & pTraceFace /*, bool bExactCD=false */)
{
	// Do the pretest!
	// Construct the sweeping sphere
	A3DVECTOR3 vSSCenter=vecStart+(vecDelta/2.0f);
	float fSSRadius=fRadius+vecDelta.Magnitude()/2.0f;

	//*
	// Expand the AABB of the model to handle the case that
	// convex hulls of the model protrude the AABB!
	A3DAABB aabb = GetModelAABB();
	aabb.Extents+=A3DVECTOR3(8.0f, 8.0f, 8.0f);
	aabb.CompleteMinsMaxs();
	if(!CLS_AABBSphereOverlap(aabb,vSSCenter,fSSRadius))
		return false;
	//*/
	
	// Detect the collision with Convex Hull
	const CFace *pCDFace=NULL, *pRealCDFace=NULL;
	float fraction, fMinFraction=1.0f;
	A3DVECTOR3 vCollidePos;
	
	for(int i=0;i<m_nNumHull;i++)
	{
		CConvexHullData* pCHData = m_ConvexHullData[i];
		if(SphereCollideWithCH(vecStart,vecDelta,fRadius,*pCHData,&pCDFace,vCollidePos,fraction))
		{
			if(fraction<fMinFraction)
			{
				fMinFraction=fraction;
				pRealCDFace=pCDFace;
				vecHitPos=vCollidePos;
				pCDCHData=pCHData;
			}
		}
	}	

	if(pRealCDFace)
	{
		/*
		if(bExactCD)
		{
			// To get the exact collision detection, we should go further 
			// to detect the sphere-mesh collision.now it is just skipped!

		}
		*/

		*pvFraction=fMinFraction;
		pTraceFace = pRealCDFace;
		vecNormal=pRealCDFace->GetNormal();
		vecHitPos-=fRadius*vecNormal;					//Adjust the hit pos!
		return true;
	}

	return false;

}

bool CELBuilding::SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pCDCHData /*, bool bExactCD=false */)
{
	const CFace* pTraceFace = NULL;
	return SphereCollideWithMe(vecStart, fRadius,  vecDelta,  vecHitPos,  pvFraction,  vecNormal, pCDCHData, pTraceFace);

}

bool CELBuilding::SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal /*, bool bExactCD */)
{
	CConvexHullData* pCDCHData=NULL;
	return SphereCollideWithMe(vecStart, fRadius,  vecDelta,  vecHitPos,  pvFraction,  vecNormal, pCDCHData);
}

bool CELBuilding::ExportBrushBuilding(const char * szFileName)
{
	AFile fileToSave;

	if( !fileToSave.Open(szFileName, AFILE_CREATENEW | AFILE_BINARY) )
		return false;

	if( !ExportBrushBuilding(&fileToSave) )
		return false;

	fileToSave.Close();
	return true;
}

bool CELBuilding::ExportBrushBuilding(AFile * pFileToSave)
{
	// then save the building.
	/*
	if( m_pA3DLitModel )
		m_pA3DLitModel->NormalizeTransforms();
	*/

	DWORD dwWrittenLen;

	DWORD version = ELBUILDING_VERSION;
	if( !pFileToSave->Write(&version, sizeof(DWORD), &dwWrittenLen) )
		return false;

	if( !pFileToSave->Write(&m_bCollideOnly, sizeof(bool), &dwWrittenLen) )
		return false;

	if( !m_pA3DLitModel->Save(pFileToSave) )
		return false;

	int nNumHull = m_ConvexHullData.size();
	if( !pFileToSave->Write(&nNumHull, sizeof(int), &dwWrittenLen) )
		return false;

	if( !SaveHullMeshList(pFileToSave) )
		return false;

	// last, export all convex hull as cdbrushes.
	for(int i=0; i<m_nNumHull; i++)
	{
		CQBrush * pBrush = new CQBrush;
		pBrush->AddBrushBevels(m_ConvexHullData[i]);
		CCDBrush * pCDBrush = new CCDBrush;
		pBrush->Export(pCDBrush);

		pCDBrush->Save(pFileToSave);

		delete pBrush;
		delete pCDBrush;
	}

	return true;
}

bool CELBuilding::SetCollideOnly(bool bFlag)
{
	if( m_bCollideOnly == bFlag )
		return true;

	m_bCollideOnly = bFlag;

	if( m_bCollideOnly )
	{
		for(int i=0; i<m_nNumHull; i++)
		{
			m_ConvexHullData[i]->SetFlags(m_ConvexHullData[i]->GetFlags() | CHDATA_SKIP_RAYTRACE);
		}	
		if( m_pA3DLitModel )
			m_pA3DLitModel->SetVisibility(false);
	}
	else
	{
		for(int i=0; i<m_nNumHull; i++)
		{
			m_ConvexHullData[i]->SetFlags(m_ConvexHullData[i]->GetFlags() & (~CHDATA_SKIP_RAYTRACE));
		}	
		if( m_pA3DLitModel )
			m_pA3DLitModel->SetVisibility(true);
	}

	return true;
}
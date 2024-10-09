/*
 * FILE: EL_BuildingWithBrush.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2006/6/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
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

#include "EL_BuildingWithBrush.h"
#include "ConvexHullData.h"
#include "CDWithCH.h"
#include "aabbcd.h"

CELBuildingWithBrush::CELBuildingWithBrush()
{
	m_pA3DDevice		= NULL;
	m_pA3DLitModel		= NULL;

	m_bCollideOnly		= false;
	m_nNumHull			= 0;
	m_nNumBrush			= 0;
	m_dwTimeLived		= 0;
	m_bHasLit			= false;

	m_bManualUpdateBrush = false;
}

CELBuildingWithBrush::~CELBuildingWithBrush()
{
}

bool CELBuildingWithBrush::Init(A3DDevice * pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;
	m_dwTimeLived = 0;
	return true;
}

bool CELBuildingWithBrush::Release()
{
	ReleaseHullMeshList();
	ReleaseCDBrushes();
	ReleaseConvexHullData();

	if( m_pA3DLitModel )
	{
		m_pA3DLitModel->Release();
		delete m_pA3DLitModel;
		m_pA3DLitModel = NULL;
	}

	m_nNumHull = 0;
	m_nNumBrush = 0;
	m_dwTimeLived = 0;
	m_bCollideOnly = false;
	return true;
}

bool CELBuildingWithBrush::ReleaseHullMeshList()
{
	m_HullMeshList.clear();
	return true;
}

bool CELBuildingWithBrush::ReleaseCDBrushes()
{
	for(int i=0; i<m_nNumBrush; i++)
	{
		if( m_CDBrushes[i] )
		{
			delete m_CDBrushes[i];
			m_CDBrushes[i] = NULL;
		}
	}

	m_CDBrushes.clear();
	m_nNumBrush = 0;
	return true;
}

bool CELBuildingWithBrush::ReleaseConvexHullData()
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

bool CELBuildingWithBrush::Tick(DWORD dwDeltaTime)
{
	// no alpha now, for on GF2 alpha mesh cost a lot.
	if( m_pA3DDevice && !m_pA3DDevice->TestPixelShaderVersion(1, 1) )
		return true;

	if( m_dwTimeLived > BUILDINGWITHBRUSH_FADE_IN_TIME )
	{
		m_dwTimeLived += dwDeltaTime;
		return true;
	}

	m_dwTimeLived += dwDeltaTime;
	int nAlpha = m_dwTimeLived * 255 / BUILDINGWITHBRUSH_FADE_IN_TIME;
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

bool CELBuildingWithBrush::Render(A3DViewport * pViewport, bool bRenderAlpha)
{
	if( m_pA3DLitModel && m_bHasLit )
		m_pA3DLitModel->Render(pViewport);

	return true;
}

bool CELBuildingWithBrush::RenderConvexHull(A3DViewport * pViewport)
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
bool CELBuildingWithBrush::SaveHullMeshList(AFile * pFileToSave)
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
bool CELBuildingWithBrush::LoadHullMeshList(AFile * pFileToLoad)
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
bool CELBuildingWithBrush::LoadConvexHullData(AFile * pFileToLoad)
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
bool CELBuildingWithBrush::SaveConvexHullData(AFile * pFileToSave)
{
	int i;
	for(i=0; i<m_nNumHull; i++)
	{
		if( !m_ConvexHullData[i]->SaveBinaryData(pFileToSave) )
			return false;
	}

	return true;
}

// Load from seperate file
bool CELBuildingWithBrush::LoadFromMOXAndCHF(A3DDevice * pA3DDevice, const char * szFileMox, const char * szFileCHF, const A3DMATRIX4& matTM)
{
	m_pA3DDevice = pA3DDevice;

	// try to release old ones.
	Release();
 
	// then load lit model and convex hull data
	m_pA3DLitModel = new A3DLitModel();
	if( !m_pA3DLitModel->LoadFromMox(pA3DDevice, szFileMox) )
		return false;

	// now initiate model's orientation
	m_pA3DLitModel->SetPos(matTM.GetRow(3));

	// create model obb
	m_ModelOBB.Build(m_pA3DLitModel->GetModelAABB());
	
	float sx, sy, sz;
	A3DVECTOR3 vecX, vecY, vecZ;
	vecX = matTM.GetRow(0);
	vecY = matTM.GetRow(1);
	vecZ = matTM.GetRow(2);

	sx = Magnitude(vecX);
	sy = Magnitude(vecY);
	sz = Magnitude(vecZ);
	m_pA3DLitModel->SetDirAndUp(vecZ / sz, vecY / sy);
	m_pA3DLitModel->SetScale(sx, sy, sz);
	m_pA3DLitModel->NormalizeTransforms();

	// Update obb
	SetOBBScale(sx, sy, sz);
	UpdateOBBWithBuilding();

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

	TransformHull(matTM);

	return true;
}

// Adjust scale factor
bool CELBuildingWithBrush::SetScale(float vScale)
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
bool CELBuildingWithBrush::SetPos(const A3DVECTOR3& vecPos)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->SetPos(vecPos);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;
	
	return TransformHull(tm);
}

bool CELBuildingWithBrush::SetDirAndUp(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp)
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
bool CELBuildingWithBrush::Move(const A3DVECTOR3& vecDeltaPos)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->Move(vecDeltaPos);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;

	if (!TransformHull(tm))
		return false;

	return true;
}

bool CELBuildingWithBrush::RotateX(float vDeltaRad)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->RotateX(vDeltaRad);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;

	return TransformHull(tm);
}

bool CELBuildingWithBrush::RotateY(float vDeltaRad)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->RotateY(vDeltaRad);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;

	return TransformHull(tm);
}

bool CELBuildingWithBrush::RotateZ(float vDeltaRad)
{
	if( !m_pA3DLitModel )
		return false;

	A3DMATRIX4 tmOld = m_pA3DLitModel->GetAbsoluteTM();

	m_pA3DLitModel->RotateZ(vDeltaRad);

	A3DMATRIX4 tmNew = m_pA3DLitModel->GetAbsoluteTM();
	A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;

	return TransformHull(tm);
}

A3DVECTOR3 CELBuildingWithBrush::GetPos()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetPos();
	
	return A3DVECTOR3(0.0f);
}

A3DVECTOR3 CELBuildingWithBrush::GetDir()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetDir();
	
	return A3DVECTOR3(0.0f, 0.0f, 1.0f);
}

A3DVECTOR3 CELBuildingWithBrush::GetUp()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetUp();
	
	return A3DVECTOR3(0.0f, 1.0f, 0.0f);
}

A3DVECTOR3 CELBuildingWithBrush::GetScale()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetScale();
	
	return A3DVECTOR3(1.0f, 1.0f, 1.0f);
}

A3DMATRIX4 CELBuildingWithBrush::GetAbsoluteTM()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetAbsoluteTM();
	
	return IdentityMatrix();
}

A3DAABB CELBuildingWithBrush::GetModelAABB()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetModelAABB();
	
	return A3DAABB(A3DVECTOR3(0.0f), A3DVECTOR3(0.0f));
}

bool CELBuildingWithBrush::TransformHull(const A3DMATRIX4& tm)
{
	int i;
	for(i=0; i<m_nNumHull; i++)
	{
		CConvexHullData * pHullData = m_ConvexHullData[i];

		pHullData->Transform(tm);
	}

	if( !m_bManualUpdateBrush )
		RebuildBrushes();
	return true;
}

bool CELBuildingWithBrush::RayTraceAABB(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal)
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->RayTraceAABB(vecStart, vecDelta, vecHitPos, pvFraction, vecNormal);

	return false;
}

bool CELBuildingWithBrush::RayTraceMesh(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal)
{
	if( RayTraceAABB(vecStart,vecDelta,vecHitPos,pvFraction,vecNormal))
		return m_pA3DLitModel->RayTraceMesh(vecStart, vecDelta, vecHitPos, pvFraction, vecNormal);

	return false;
}

bool CELBuildingWithBrush::RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pTraceCHData, const CFace* & pTraceFace)
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

bool CELBuildingWithBrush::RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pTraceCHData)
{
	const CFace* pTraceFace = NULL;
	return RayTraceConvexHull(vecStart, vecDelta, vecHitPos, pvFraction, vecNormal, pTraceCHData, pTraceFace);
}

bool CELBuildingWithBrush::RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal)
{
	CConvexHullData* pTraceCHData=NULL;
	return RayTraceConvexHull(vecStart, vecDelta, vecHitPos, pvFraction, vecNormal, pTraceCHData);
}

bool CELBuildingWithBrush::SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pCDCHData, const CFace* & pTraceFace /*, bool bExactCD=false */)
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

bool CELBuildingWithBrush::SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pCDCHData /*, bool bExactCD=false */)
{
	const CFace* pTraceFace = NULL;
	return SphereCollideWithMe(vecStart, fRadius,  vecDelta,  vecHitPos,  pvFraction,  vecNormal, pCDCHData, pTraceFace);

}

bool CELBuildingWithBrush::SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal /*, bool bExactCD */)
{
	CConvexHullData* pCDCHData=NULL;
	return SphereCollideWithMe(vecStart, fRadius,  vecDelta,  vecHitPos,  pvFraction,  vecNormal, pCDCHData);
}

bool CELBuildingWithBrush::SetCollideOnly(bool bFlag)
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

bool CELBuildingWithBrush::TraceWithBrush(BrushTraceInfo * pInfo, bool bCheckCHFlags)
{

	// Expand the AABB of the model to handle the case that
	// convex hulls of the model protrude the AABB!
	A3DAABB aabb = GetModelAABB();
	aabb.Extents+=A3DVECTOR3(8.0f, 8.0f, 8.0f);
	aabb.CompleteMinsMaxs();

	A3DVECTOR3 vPoint, vNormal;
	float		fFraction;		//	Fraction
	
	if (pInfo->bIsPoint && !CLS_RayToAABB3(pInfo->vStart, pInfo->vDelta, aabb.Mins, aabb.Maxs, vPoint, &fFraction, vNormal ) ) {
		return false;
	}
	
	if (!pInfo->bIsPoint && !CLS_AABBAABBOverlap(aabb.Center, aabb.Extents, pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents) ) {
		return false;
	}
	
	//save original result
	bool		bStartSolid = pInfo->bStartSolid;	//	Collide something at start point
	bool		bAllSolid = pInfo->bAllSolid;		//	All in something
	int			iClipPlane = pInfo->iClipPlane;		//	Clip plane's index
	
	fFraction = 100.0f;
	vNormal = pInfo->ClipPlane.GetNormal(); //clip plane normal
	float       fDist = pInfo->ClipPlane.GetDist();	//clip plane dist
//#define LOCAL_DEBUG
	
#ifdef LOCAL_DEBUG
	char msg[200];
	sprintf(msg, " %d" , m_CDBrushes.size());
	OutputDebugStringA(msg);
#endif

	// Detect the collision with Convex Hull
	bool bCollide = false;
	for(int i=0;i<(int)m_CDBrushes.size();i++)
	{
		if(bCheckCHFlags)
		{
			// Check the flags to test whether we skip current CDBrush!
			DWORD dwFlags = m_CDBrushes[i]->GetReservedDWORD();
			if(pInfo->bIsPoint && (dwFlags & CHDATA_SKIP_RAYTRACE) ||
               !pInfo->bIsPoint && (dwFlags & CHDATA_SKIP_COLLISION) )
			{
				continue;
			}
		}

		if (m_CDBrushes[i]->Trace(pInfo) && (pInfo->fFraction < fFraction)) {
				//update the saving info
				bStartSolid = pInfo->bStartSolid;
				bAllSolid = pInfo->bAllSolid;
				iClipPlane = pInfo->iClipPlane;
				fFraction = pInfo->fFraction;
				vNormal = pInfo->ClipPlane.GetNormal();
				fDist = pInfo->ClipPlane.GetDist();

				bCollide = true;
		}
	}	
	if (bCollide) {
		pInfo->bStartSolid = bStartSolid;
		pInfo->bAllSolid = bAllSolid;
		pInfo->iClipPlane = iClipPlane;
		pInfo->fFraction = fFraction;
		pInfo->ClipPlane.SetNormal(vNormal);
		pInfo->ClipPlane.SetD(fDist);
	}
	return bCollide;
}

bool CELBuildingWithBrush::RebuildBrushes()
{
	// First, release old ones.
	ReleaseCDBrushes();

	for(int i=0; i<m_nNumHull; i++)
	{
		CQBrush * pBrush = new CQBrush();
		pBrush->AddBrushBevels(m_ConvexHullData[i]);
		CCDBrush * pCDBrush = new CCDBrush();
		pBrush->Export(pCDBrush);
		m_CDBrushes.push_back(pCDBrush);
		delete pBrush;
	}

	m_nNumBrush = m_nNumHull;
	return true;
}

bool CELBuildingWithBrush::SetOBBScale(float x, float y, float z)
{
	m_ModelOBB.Extents.x *= x;
	m_ModelOBB.Extents.y *= y;
	m_ModelOBB.Extents.z *= z;
	m_ModelOBB.CompleteExtAxis();

	return true;
}

bool CELBuildingWithBrush::UpdateOBBWithBuilding()
{
	if (NULL == m_pA3DLitModel)
		return false;

	A3DVECTOR3 vDir, vUp;
	vDir = m_pA3DLitModel->GetDir();
	vUp = m_pA3DLitModel->GetUp();
	m_ModelOBB.Center = m_pA3DLitModel->GetModelAABB().Center;
	m_ModelOBB.XAxis = CrossProduct(vDir, vUp);
	m_ModelOBB.YAxis = vUp;
	m_ModelOBB.ZAxis = vDir;
	m_ModelOBB.CompleteExtAxis();

	return true;
}
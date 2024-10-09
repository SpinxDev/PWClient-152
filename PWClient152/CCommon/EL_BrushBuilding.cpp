/*
 * FILE: EL_BrushBuilding.cpp
 *
 * DESCRIPTION: building using brush for cd, only used in element client
 *
 * CREATED BY: Hedi, 2005/8/23
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

#include "EL_BrushBuilding.h"
#include "CDWithCH.h"
#include "aabbcd.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "A3DLight.h"

CELBrushBuilding::CELBrushBuilding()
{
	m_pA3DDevice		= NULL;
	m_pA3DLitModel		= NULL;

	m_nNumHull			= 0;
	m_dwTimeLived		= 0;
}

CELBrushBuilding::~CELBrushBuilding()
{
}

bool CELBrushBuilding::Init(A3DDevice * pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;
	m_dwTimeLived = 0;
	return true;
}

bool CELBrushBuilding::Release()
{
	ReleaseHullMeshList();
	ReleaseCDBrushes();

	if( m_pA3DLitModel )
	{
		m_pA3DLitModel->Release();
		delete m_pA3DLitModel;
		m_pA3DLitModel = NULL;
	}

	m_bCollideOnly = false;
	m_nNumHull = 0;
	m_dwTimeLived = 0;
	return true;
}

bool CELBrushBuilding::ReleaseHullMeshList()
{
	m_HullMeshList.clear();
	return true;
}

bool CELBrushBuilding::ReleaseCDBrushes()
{
	for(int i=0; i<m_nNumHull; i++)
	{
		if( m_CDBrushes[i] )
		{
			delete m_CDBrushes[i];
			m_CDBrushes[i] = NULL;
		}
	}

	m_CDBrushes.clear();
	m_nNumHull = 0;
	return true;
}

bool CELBrushBuilding::Tick(DWORD dwDeltaTime)
{			
	// no alpha now, for on GF2 alpha mesh cost a lot.
	if( m_pA3DDevice && !m_pA3DDevice->TestPixelShaderVersion(1, 1) )
		return true;

	if( m_dwTimeLived > BRUSHBUILDING_FADE_IN_TIME )
	{
		m_dwTimeLived += dwDeltaTime;
		return true;
	}

	m_dwTimeLived += dwDeltaTime;
	int nAlpha = m_dwTimeLived * 255 / BRUSHBUILDING_FADE_IN_TIME;
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

bool CELBrushBuilding::Render(A3DViewport * pViewport, bool bRenderAlpha)
{
	if( m_pA3DLitModel )
		m_pA3DLitModel->Render(pViewport);

	return true;
}

// save hull's meshes list
bool CELBrushBuilding::SaveHullMeshList(AFile * pFileToSave)
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
bool CELBrushBuilding::LoadHullMeshList(AFile * pFileToLoad)
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

// Load cd brushes
bool CELBrushBuilding::LoadCDBrushes(AFile * pFileToLoad,float cx,float cz)
{
	int i;
	for(i=0; i<m_nNumHull; i++)
	{
		CCDBrush * pCDBrush = new CCDBrush();
		if( !pCDBrush->Load(pFileToLoad) )
			return false;

		pCDBrush->Offset(cx,cz);

		m_CDBrushes.push_back(pCDBrush);
	}

	return true;
}

// Save convex hull data
bool CELBrushBuilding::SaveCDBrushes(AFile * pFileToSave)
{
	int i;
	for(i=0; i<m_nNumHull; i++)
	{
		if( !m_CDBrushes[i]->Save(pFileToSave) )
			return false;
	}

	return true;
}

// Load and Save
bool CELBrushBuilding::Save(const char * szFileName)
{
	AFile fileToSave;

	if( !fileToSave.Open(szFileName, AFILE_CREATENEW | AFILE_BINARY) )
		return false;

	if( !Save(&fileToSave) )
		return false;

	fileToSave.Close();
	return true;
}

bool CELBrushBuilding::Save(AFile * pFileToSave)
{
	DWORD dwWrittenLen;

	DWORD version = ELBRUSHBUILDING_VERSION;
	if( !pFileToSave->Write(&version, sizeof(DWORD), &dwWrittenLen) )
		return false;

	if( !pFileToSave->Write(&m_bCollideOnly, sizeof(bool), &dwWrittenLen) )
		return false;

	if( !m_pA3DLitModel->Save(pFileToSave) )
		return false;

	int nNumHull = m_CDBrushes.size();
	if( !pFileToSave->Write(&nNumHull, sizeof(int), &dwWrittenLen) )
		return false;

	if( !SaveHullMeshList(pFileToSave) )
		return false;

	if( !SaveCDBrushes(pFileToSave) )
		return false;

	return true;
}

bool CELBrushBuilding::Load(A3DDevice * pA3DDevice, const char * szFileName,float cx,float cz)
{
	AFileImage fileToLoad;

	if( !fileToLoad.Open(szFileName, AFILE_OPENEXIST | AFILE_BINARY) )
		return false;

	if( !Load(pA3DDevice, &fileToLoad,cx,cz) )
		return false;

	fileToLoad.Close();
	return true;
}

bool CELBrushBuilding::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad,float cx,float cz)
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

	if(fabs(cx)>0.001 || fabs(cz)>0.001)
		m_pA3DLitModel->Move(A3DVECTOR3(cx,0,cz));

	m_pA3DLitModel->NormalizeTransforms();

	

#ifdef LM_BUMP_ENABLE
	// test only, 
	m_pA3DLitModel->SetLightInfo(g_pGame->GetDirLight()->GetLightparam(), g_pGame->GetDirLight()->GetLightparam());
#endif

	if( !pFileToLoad->Read(&m_nNumHull, sizeof(int), &dwReadLen) )
		return true;	//	For compatability with old version

	if( !LoadHullMeshList(pFileToLoad) )
		return false;

	if( !LoadCDBrushes(pFileToLoad,cx,cz) )
		return false;

	SetCollideOnly(bCollideOnly);
	// now call once tick to make alpha value of all meshes correct.
#ifndef _MODEDITOREX
	Tick(0);
#endif
	return true;
}

A3DAABB CELBrushBuilding::GetModelAABB()
{
	if( m_pA3DLitModel )
		return m_pA3DLitModel->GetModelAABB();
	
	return A3DAABB(A3DVECTOR3(0.0f), A3DVECTOR3(0.0f));
}

bool CELBrushBuilding::TraceWithBrush(BrushTraceInfo * pInfo, bool bCheckCHFlags)
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

bool CELBrushBuilding::SetCollideOnly(bool bFlag)
{
	if( m_bCollideOnly == bFlag )
		return true;

	m_bCollideOnly = bFlag;

	if( m_bCollideOnly )
	{
		for(int i=0; i<m_nNumHull; i++)
		{
			m_CDBrushes[i]->SetReservedDWORD(m_CDBrushes[i]->GetReservedDWORD() | CHDATA_SKIP_RAYTRACE);
		}	
		if( m_pA3DLitModel )
			m_pA3DLitModel->SetVisibility(false);
	}
	else
	{
		for(int i=0; i<m_nNumHull; i++)
		{
			m_CDBrushes[i]->SetReservedDWORD(m_CDBrushes[i]->GetReservedDWORD() & (~CHDATA_SKIP_RAYTRACE));
		}	
		if( m_pA3DLitModel )
			m_pA3DLitModel->SetVisibility(true);
	}

	return true;
}
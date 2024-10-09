/*
 * FILE: EL_Forest.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2004/9/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include <A3DDevice.h>
#include <A3DFuncs.h>
#include <A3DCamera.h>
#include <A3DEngine.h>
#include <A3DTexture.h>
#include <A3DTextureMan.h>
#include <A3DViewport.h>
#include <A3DFont.h>
#include <AFile.h>
#include <A3DStream.h>
#include <A3DLight.h>
#include <A3DPixelShader.h>
#include <A3DVertexShader.h>
#include <A3DShaderMan.h>

#include "EL_Forest.h"
#include "EL_Tree.h"

CELForest::CELForest()
{
	m_pA3DDevice		= NULL;
	m_pDirLight			= NULL;
	m_pLogFile			= NULL;

#ifdef TREE_BUMP_ENABLE
	m_pBranchPixelShader	= NULL;
	m_pBranchVertexShader	= NULL;
#endif

	m_vecLightDir		= A3DVECTOR3(0.0f, -1.0f, 0.0f);

	m_nTreeCount		= 0;
	m_vTime				= 0.0f;

	m_pStream			= NULL;

	m_vLODLevel			= 1.0f;
}


CELForest::~CELForest()
{
}

bool CELForest::Init(A3DDevice * pA3DDevice, A3DLight * pDirLight, ALog * pLogFile)
{
	// for evaluation version only
	CSpeedTreeRT::Authorize("RT-0637-9ea30d39f89d");

	m_pA3DDevice	= pA3DDevice;
	m_pDirLight		= pDirLight;
	m_pLogFile		= pLogFile;
	m_vTime			= 0.0f;

	m_wind.Init(256, 1.0f, 256, 0.999f, 8, 0x12345);

	// testing a light
	float afLight1[] = 
	{ 
	   400.0f, 1000.0f, -400.0f,	// [indexes 0-2] position (x, y, z) 
	   0.0f, 0.0f, 0.0f,			// [indexes 3-5] diffuse (r, g, b) 
	   0.0f, 0.0f, 0.0f,			// [indexes 6-8] ambient (r, g, b) 
	   1.0f, 1.0f, 1.0f,			// [indexes 9-11] specular (r, g, b) 
	   0.0f,						// [index 12] directional flag (0 = directional, 1 = positional) 
	   1.0f, 0.0f, 0.0f				// [indexes 13-15] attenuation (constant, linear, quadratic) 
	}; 

	CSpeedTreeRT::SetLightAttributes(0, afLight1); 
	CSpeedTreeRT::SetLightState(0, true); 

	if( !CreateStream(40000 / 4) )
	{
		m_pLogFile->Log("CELForest::Init(), failed to call CreateStream()!");
		return false;
	}

	A3DMATERIALPARAM param;
	param.Ambient = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	param.Diffuse = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	param.Emissive = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	param.Specular = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	param.Power = 10.0f;
	m_material.Init(m_pA3DDevice, param);

#ifdef TREE_BUMP_ENABLE
	m_pBranchPixelShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("shaders\\ps\\treebranch.txt", false);

	DWORD	dwVSDecl[20];
	int		n = 0;
	dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT3);		// x, y, z in local space
	dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_FLOAT3);		// nx, ny, nz in local space
	dwVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_FLOAT2);		// u, v coords.
	dwVSDecl[n ++] = D3DVSD_REG(3, D3DVSDT_FLOAT3);		// tangent coords in local space.
	dwVSDecl[n ++] = D3DVSD_REG(4, D3DVSDT_FLOAT3);		// binormal coords in local space.
	dwVSDecl[n ++] = D3DVSD_END();						// end tokens.
	m_pBranchVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("shaders\\vs\\treebranchvs.txt", false, dwVSDecl);
#endif
	return true;
}

bool CELForest::CreateStream(int nMaxBlades)
{
	m_nMaxBlades = nMaxBlades;
	
	m_pStream = new A3DStream();
	if( !m_pStream->Init(m_pA3DDevice, sizeof(TREE_VERTEX1), TREE_FVF_VERTEX1, m_nMaxBlades * 4, m_nMaxBlades * 6,
		A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC) ) 
	{
		m_pLogFile->Log("CELForest::CreateStream(), failed to initialize stream!");
		return false;
	}

	// now we set up the index buffer here
	WORD * pIndices;

	if( !m_pStream->LockIndexBuffer(0, 0, (BYTE**) &pIndices, 0) )
		return false;

	for(int i=0; i<m_nMaxBlades; i++)
	{
		pIndices[i * 6    ] = i * 4 + 1;
		pIndices[i * 6 + 1] = i * 4 + 0;
		pIndices[i * 6 + 2] = i * 4 + 3;

		pIndices[i * 6 + 3] = i * 4 + 1;
		pIndices[i * 6 + 4] = i * 4 + 3;
		pIndices[i * 6 + 5] = i * 4 + 2;
	}

	m_pStream->UnlockIndexBuffer();
	m_nBladesCount = 0;
	return true;
}

bool CELForest::ReleaseStream()
{
	if( m_pStream )
	{
		m_pStream->Release();
		delete m_pStream;
		m_pStream = NULL;
	}

	return true;
}

bool CELForest::LoadTypesFromConfigFile(const char* szConfigFile)
{
	// first load all tree types
	AFile configFile;
	if( !configFile.Open(szConfigFile, AFILE_OPENEXIST | AFILE_TYPE_TEXT) )
	{
		m_pLogFile->Log("CELForest::Init(), Can not open config file!");
		return false;
	}
	
	char	szLine[256];
	DWORD	dwReadLen;

	// first line, number of tree types
	configFile.ReadLine(szLine, 256, &dwReadLen);
	int		nNumTreeType;
	sscanf(szLine, "%d", &nNumTreeType);

	DWORD	dwTypeID;
	for(int i=0; i<nNumTreeType; i++)
	{
		// first render type id of this type of tree
		configFile.ReadLine(szLine, 256, &dwReadLen);
		sscanf(szLine, "%d", &dwTypeID);

		configFile.ReadLine(szLine, 256, &dwReadLen);
		CELTree * pNewTree = new CELTree();
		if( !pNewTree->Init(m_pA3DDevice, this, dwTypeID, szLine, "trees\\Sample_Trees\\CompositeMap.dds", m_pLogFile) )
		{
			m_pLogFile->Log("CELForest::Init(), failed to load tree type [%s]", szLine);
			return false;
		}

		m_TreeTypes.Add(pNewTree);
	}

	// now read composite texture, but not load it;
	configFile.ReadLine(szLine, 256, &dwReadLen);
	
	// now close the config file
	configFile.Close();

	SortTreeTypeByCompositeMap();
	return true;
}

bool CELForest::Release()
{
	int nNumTreeType = m_TreeTypes.GetSize();

	for(int i=0; i<nNumTreeType; i++)
	{
		CELTree * pTreeType = m_TreeTypes[i];

		if( pTreeType )
		{
			pTreeType->Release();
			delete pTreeType;
			pTreeType = NULL;
		}
	}

	m_TreeTypes.RemoveAll();

	ReleaseStream();

#ifdef TREE_BUMP_ENABLE
	if( m_pBranchPixelShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pBranchPixelShader);
		m_pBranchPixelShader = NULL;
	}
	if( m_pBranchVertexShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pBranchVertexShader);
		m_pBranchVertexShader = NULL;
	}
#endif

	return true;
}

bool CELForest::Update(DWORD dwDeltaTime)
{
	// first update speed tree's global data for animation
	m_vTime += dwDeltaTime * 0.001f;
	CSpeedTreeRT::SetTime(m_vTime);

	return true;
}

bool CELForest::Render(A3DViewport * pViewport)
{
	if( m_pDirLight )
		m_vecLightDir = m_pDirLight->GetLightparam().Direction;

	A3DCameraBase * pCamera = pViewport->GetCamera();
	A3DVECTOR3 vecCamPos = pCamera->GetPos();
	A3DVECTOR3 vecCamDir = pCamera->GetDir();

	float temp;
	temp = vecCamPos.y;
	vecCamPos.y = vecCamPos.z;
	vecCamPos.z = temp;
	temp = vecCamDir.y;
	vecCamDir.y = vecCamDir.z;
	vecCamDir.z = temp;
	CSpeedTreeRT::SetCamera(vecCamPos.m, vecCamDir.m);

	m_nTreeCount = 0;

	int nNumTreeType = m_TreeTypes.GetSize();

	m_pA3DDevice->SetAlphaTestEnable(true);
	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetAlphaFunction(A3DCMP_GREATER);
	m_pA3DDevice->SetSpecularEnable(false);
	
	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();

	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X); 
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE); 
	
	//	Save light information
	DWORD dwAmbient = m_pA3DDevice->GetAmbientColor();
	A3DLIGHTPARAM DirLightParams;
	DirLightParams = m_pDirLight->GetLightparam();

	// now adjust for modulate 2x
	m_pA3DDevice->SetAmbient(0xff000000 | ColorValueToColorRGBA(m_pA3DDevice->GetAmbientValue() * 0.5f));
	A3DLIGHTPARAM newParam = DirLightParams;
	newParam.Diffuse = newParam.Diffuse * 0.5f;
	newParam.Diffuse.a = 1.0f;
	m_pDirLight->SetLightParam(newParam);
	m_material.Appear();
	
	A3DCULLTYPE	cullOld = m_pA3DDevice->GetFaceCull();

	if( cullOld == A3DCULL_CW )
		m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	else if( cullOld == A3DCULL_CCW )
		m_pA3DDevice->SetFaceCull(A3DCULL_CW);
	
#ifdef TREE_BUMP_ENABLE
	bool bUserClip = false;
	D3DXPLANE cp, hcp;
	if( m_pBranchPixelShader && m_pBranchVertexShader && !(GetAsyncKeyState(VK_F7) & 0x8000) )
	{
		A3DCOLORVALUE c1;
		c1 = m_pA3DDevice->GetAmbientValue();
		m_pA3DDevice->SetPixelShaderConstants(1, &c1, 1);
		A3DCOLORVALUE c2;
		c2 = newParam.Diffuse;
		m_pA3DDevice->SetPixelShaderConstants(2, &c2, 1);
		m_pBranchPixelShader->Appear();

		//	Prepare a transformed clip plane
		DWORD dwState;
		dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);
		if( dwState == D3DCLIPPLANE0 ) // only one clip plane supported now
		{
			bUserClip = true;
			m_pA3DDevice->GetD3DDevice()->GetClipPlane(0, (float *)&cp);
			A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
			matVP.InverseTM();
			matVP.Transpose();
			D3DXPlaneTransform(&hcp, &cp, (D3DXMATRIX *) &matVP);
			m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float *)&hcp);
		}
	}
#endif

	int i;
	int numTrees = 0;
	A3DMATRIX4 matShadow = IdentityMatrix();
	for(i=0; i<nNumTreeType; i++)
	{
		CELTree * pTreeType = m_TreeTypes[i];

		// first we render all trees' branch here
		if( pTreeType && pTreeType->GetTreeCount() > 0 )
		{
			// before first time render, we should do visible testing and compute lod level
			pTreeType->UpdateForRender(pViewport);

			// then render branch part
			pTreeType->RenderBranches(pViewport);
		}
	}

#ifdef TREE_BUMP_ENABLE
	if( m_pBranchPixelShader && m_pBranchVertexShader )
	{
		if( bUserClip )
			m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float *)&cp);
		m_pBranchPixelShader->Disappear();
		m_pBranchVertexShader->Disappear();
	}
#endif

	TREE_VERTEX1 *		pVerts;
	int					nBladeCount;

	// render for fronds, this will be optimized later with collecting and rendering
	for(i=0; i<nNumTreeType; i++)
	{
		CELTree * pTreeType = m_TreeTypes[i];
		
		numTrees += pTreeType->GetTreeCount();
		if( pTreeType && pTreeType->GetTreeCount() > 0 )
		{
			pTreeType->GetCompositeTexture()->Appear(0);
			
			pTreeType->RenderFronds(pViewport);

			m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
			m_pA3DDevice->SetAlphaRef(84);
			if( pTreeType->m_nCurLeavesCount + pTreeType->m_nCurBillboardCount > 0 )
			{
				m_pStream->Appear();

				int nLastTree = 0;
				int nThisTree = 0;

DOAGAIN:
				nLastTree = nThisTree;
				
				if( !m_pStream->LockVertexBuffer(0, 0, (BYTE **) &pVerts, 0) )
					return false;

				pTreeType->DrawLeavesAndBillboardsToBuffer(pVerts, m_nMaxBlades, nBladeCount, nThisTree);

				if( !m_pStream->UnlockVertexBuffer() )
					return false;

				if( nThisTree == nLastTree ) // fuck, one tree with more than 10000 leafs, not supported now!
					continue;

				m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nBladeCount * 4, 0, nBladeCount * 2);
					
				if( nThisTree != pTreeType->GetTreeCount() )
					goto DOAGAIN;
			}

			pTreeType->GetCompositeTexture()->Disappear(0);
		}
	}

	// restore light for modulate texture color op
	m_pA3DDevice->SetAmbient(dwAmbient);
	m_pDirLight->SetLightParam(DirLightParams);

	m_pA3DDevice->SetFaceCull(cullOld);
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetAlphaTestEnable(false);
	return true;
}

bool CELForest::Load(const char * szForestFile)
{	
	AFile fileToLoad;
	if( !fileToLoad.Open(szForestFile, AFILE_OPENEXIST) )
	{
		m_pLogFile->Log("CELForest::Load(), failed to open file [%s]", szForestFile);
		return true;
	}

	DWORD dwReadLen;
	int i;
	int nNumTypes;

	// first of all write a version for future compatibility
	DWORD dwVersion;
	if( !fileToLoad.Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;
	if( dwVersion != FOREST_VERSION )
	{
		if( !LoadTypesFromConfigFile("trees\\trees.cfg") )
		{
			m_pLogFile->Log("CELForest::Load(), failed to load tree types from config file!");
			return false;
		}

		// use it as the number of types
		nNumTypes = *(int *)&dwVersion;
		if( nNumTypes != m_TreeTypes.GetSize() )
		{
			m_pLogFile->Log("CELForest::Load(), tree type library has been modified, so all tree data can't be loaded!");
			return false;
		}
	}
	else
	{
		// then read types count
		if( !fileToLoad.Read(&nNumTypes, sizeof(int), &dwReadLen) )
			goto READFAIL;

		for(i=0; i<nNumTypes; i++)
		{
			// then read all types information from the file and create all types here.
			DWORD dwTypeID;
			if( !fileToLoad.Read(&dwTypeID, sizeof(DWORD), &dwReadLen) )
				goto READFAIL;

			char szTreeFile[256];
			if( !fileToLoad.Read(szTreeFile, 256, &dwReadLen) )
				goto READFAIL;

			char szCompositeMap[256];
			if( !fileToLoad.Read(szCompositeMap, 256, &dwReadLen) )
				goto READFAIL;

			CELTree * pNewTree = new CELTree();
			if( !pNewTree->Init(m_pA3DDevice, this, dwTypeID, szTreeFile, szCompositeMap, m_pLogFile) )
			{
				m_pLogFile->Log("CELForest::Load(), failed to init tree type [%s]", szTreeFile);
				return false;
			}

			m_TreeTypes.Add(pNewTree);
		}
	}

	// now write each tree's information out
	for(i=0; i<nNumTypes; i++)
	{
		CELTree * pTree = m_TreeTypes[i];

		if( !pTree->LoadTrees(&fileToLoad) )
		{
			m_pLogFile->Log("CELForest::Load(), failed to call CELTree::Load()!");
			return false;
		}
	}

	fileToLoad.Close();
	return true;

READFAIL:
	m_pLogFile->Log("CELForest::Load(), Read from file failed!");
	return false;
}

bool CELForest::Save(const char * szForestFile)
{
	AFile fileToSave;
	if( !fileToSave.Open(szForestFile, AFILE_CREATENEW | AFILE_BINARY) )
	{
		m_pLogFile->Log("CELForest::Save(), failed to create file [%s]", szForestFile);
		return false;
	}

	DWORD dwWriteLen;
	
	// first of all write a version for future compatibility
	DWORD dwVersion = FOREST_VERSION;
	if( !fileToSave.Write(&dwVersion, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;

	// then write types count
	int nNumTypes;
	nNumTypes = m_TreeTypes.GetSize();
	if( !fileToSave.Write(&nNumTypes, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;

	int i;

	for(i=0; i<nNumTypes; i++)
	{
		// then write out each type's information
		DWORD dwTreeTypeID = m_TreeTypes[i]->GetTypeID();
		if( !fileToSave.Write(&dwTreeTypeID, sizeof(int), &dwWriteLen) )
			goto WRITEFAIL;

		char szTreeFile[256];
		strncpy(szTreeFile, m_TreeTypes[i]->GetTreeFile(), 256);
		if( !fileToSave.Write(szTreeFile, 256, &dwWriteLen) )
			goto WRITEFAIL;

		char szCompositeMap[256];
		strncpy(szCompositeMap, m_TreeTypes[i]->GetCompositeMap(), 256);
		if( !fileToSave.Write(szCompositeMap, 256, &dwWriteLen) )
			goto WRITEFAIL;

	}
	
	// now write each tree's information out
	for(i=0; i<nNumTypes; i++)
	{
		CELTree * pTree = m_TreeTypes[i];

		if( !pTree->SaveTrees(&fileToSave) )
		{
			m_pLogFile->Log("CELForest::Save(), failed to call CELTree::Save()!");
			return false;
		}
	}

	fileToSave.Close();
	return true;

WRITEFAIL:
	m_pLogFile->Log("CELForest::Save(), Write to the file failed!");
	return false;
}

float CELForest::GetWindStrength(const A3DVECTOR3& vecPos)
{
	float fWind;
	m_wind.GetValue(m_vTime * 1.0f, &fWind, 1);
	return fWind;
}

CELTree* CELForest::AddTreeType(DWORD dwTypeID, const char * szTreeFile, const char * szCompositeMap)
{
	CELTree * pNewTree = new CELTree();
	if( !pNewTree->Init(m_pA3DDevice, this, dwTypeID, szTreeFile, szCompositeMap, m_pLogFile) )
	{
		m_pLogFile->Log("CELForest::AddTreeType(), failed to load tree type [%s]", szTreeFile);
		return NULL;
	}

	m_TreeTypes.Add(pNewTree);

	//SortTreeTypeByCompositeMap();
	return pNewTree;
}

bool CELForest::DeleteTreeType(CELTree * pTreeType)
{
	int nNumType = m_TreeTypes.GetSize();
	for(int i=0; i<nNumType; i++)
	{
		if( pTreeType == m_TreeTypes[i] )
		{
			pTreeType->Release();
			delete pTreeType;

			m_TreeTypes.RemoveAt(i);
			return true;
		}
	}

	return true;
}

CELTree * CELForest::GetTreeTypeByID(DWORD dwTypeID)
{
	int nNumType = m_TreeTypes.GetSize();

	for(int i=0; i<nNumType; i++)
	{
		CELTree * pTreeType = m_TreeTypes[i];

		if( pTreeType->GetTypeID() == dwTypeID )
			return pTreeType;
	}

	return NULL;
}

bool CELForest::SortTreeTypeByCompositeMap()
{
	int nNumType = m_TreeTypes.GetSize();

	for(int i=0; i<nNumType-1; i++)
	{
		for(int j=i+1; j<nNumType; j++)
		{
			if( m_TreeTypes[i]->GetCompositeTexture() > m_TreeTypes[j]->GetCompositeTexture() )
			{
				CELTree * pTemp = m_TreeTypes[i];
				m_TreeTypes[i] = m_TreeTypes[j];
				m_TreeTypes[j] = pTemp;
			}
		}
	}

	return true;
}

bool CELForest::SetLODLevel(float level)
{
	m_vLODLevel = level + 0.4f;

	if( m_vLODLevel > 1.2f )
		m_vLODLevel = 1.2f;

	int nNumTypes = m_TreeTypes.GetSize();
	for(int i=0; i<nNumTypes; i++)
	{
		CELTree * pTree = m_TreeTypes[i];
		pTree->ReComputeAllTrees();
	}

	return true;
}

bool CELForest::TraceWithBrush(BrushTraceInfo * pInfo)
{

	bool bCollide=false;
	//save original result
	bool		bStartSolid = pInfo->bStartSolid;	//	Collide something at start point
	bool		bAllSolid = pInfo->bAllSolid;		//	All in something
	int			iClipPlane = pInfo->iClipPlane;		//	Clip plane's index
	//float		fFraction = pInfo->fFraction;		//	Fraction
	float		fFraction = 100.0f;		//	Fraction
	A3DVECTOR3  vNormal = pInfo->ClipPlane.GetNormal(); //clip plane normal
	float       fDist = pInfo->ClipPlane.GetDist();	//clip plane dist
	DWORD		dwTreeType = 0xffffffff;
	DWORD		dwTreeID = 0xffffffff;
	
	int nNumTypes = m_TreeTypes.GetSize();
	for(int i=0; i<nNumTypes; i++){
		CELTree * pTree = m_TreeTypes[i];
		if ((pTree) && pTree->TraceWithBrush(pInfo) && (pInfo->fFraction < fFraction)) {
			bStartSolid = pInfo->bStartSolid;
			bAllSolid = pInfo->bAllSolid;
			iClipPlane = pInfo->iClipPlane;
			fFraction = pInfo->fFraction;
			vNormal = pInfo->ClipPlane.GetNormal();
			fDist = pInfo->ClipPlane.GetDist();
			dwTreeID = pInfo->dwUser2;
			dwTreeType = pTree->GetTypeID();
			bCollide = true;
		}
	}

	//set back
	pInfo->bAllSolid = bAllSolid;
	pInfo->bStartSolid = bStartSolid;
	pInfo->iClipPlane = iClipPlane;
	pInfo->fFraction = fFraction;
	pInfo->ClipPlane.SetNormal(vNormal);
	pInfo->ClipPlane.SetD(fDist);
	pInfo->dwUser1 = dwTreeType;
	pInfo->dwUser2 = dwTreeID;

	return bCollide;

}

// Sphere- Collision Detection, for editor only
bool CELForest::SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal)
{
	float fraction, fMinFraction=999999.0f;
	A3DVECTOR3 vecHitPosThis;
	A3DVECTOR3 vecNormalThis;

	int nNumTypes = m_TreeTypes.GetSize();
	for(int i=0; i<nNumTypes; i++)
	{
		CELTree * pTree = m_TreeTypes[i];

		if( pTree->SphereCollideWithMe(vecStart, fRadius, vecDelta, vecHitPosThis, &fraction, vecNormalThis) )
		{
			if( fraction < fMinFraction )
			{
				fMinFraction = fraction;
				vecHitPos = vecHitPosThis;
				vecNormal = vecNormalThis;
			}
		}
	}

	if( fMinFraction < 1.0f )
	{
		*pvFraction = fMinFraction;
		vecHitPos-=fRadius*vecNormal;					//Adjust the hit pos!
		return true;
	}

	return false;
}

// Add tree, by jdl
bool CELForest::AddTree(int nIndex, const A3DVECTOR3& vPos)
{
	ASSERT(nIndex>=0 && nIndex<m_TreeTypes.GetSize());

	DWORD dwID;
	return (m_TreeTypes[nIndex]->AddTree(vPos, dwID));
}

// Delete all tree types, by jdl
bool CELForest::DeleteAllTreeTypes()
{
	int nNumTreeType = m_TreeTypes.GetSize();

	for(int i=0; i<nNumTreeType; i++)
	{
		CELTree * pTreeType = m_TreeTypes[i];
		A3DRELEASE(pTreeType);
	}

	m_TreeTypes.RemoveAll();

	return true;
}

// Get tree type count, by jdl
int CELForest::GetTreeTypeNumber()
{
	return m_TreeTypes.GetSize();
}


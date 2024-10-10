/*
 * FILE: A3DPlants.cpp
 *
 * DESCRIPTION: Class used to operating the plants in the world
 *
 * CREATED BY: Hedi, 2002/1/25
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPlants.h"
#include "A3DEngine.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DDevice.h"
#include "A3DFrame.h"
#include "A3DStream.h"
#include "A3DMesh.h"
#include "A3DTexture.h"
#include "A3DCollision.h"
#include "A3DCamera.h"
#include "A3DViewport.h"			 
#include "A3DMoxMan.h"
#include "A3DWorld.h"
#include "A3DTerrain.h"
#include "A3DTextureMan.h"
#include "A3DGDI.h"
#include "A3DMoxMan.h"
#include "A3DConfig.h"
#include "AFI.h"

A3DPlants::A3DPlants()
{
	m_pA3DDevice	= NULL;
	m_bHWIPlants	= false;

	m_bShowBox		= false; // We do not show box of the trees;

	m_vTreeDis1		= 120.0f;
	m_vTreeDis2		= 600.0f;
	m_vTreeDis3		= 800.0f;

	m_vGrassDis1	= 800.0f;
	m_vGrassDis2	= 1200.0f;

	m_nNumTree		= 0;
	m_nNumGrass		= 0;

	m_nNumTreeType	= 0;
	m_nNumGrassType = 0;

	ZeroMemory(m_pTreeTypes, sizeof(TREETYPE) * TREE_MAX_TYPE);
	ZeroMemory(m_pGrassTypes, sizeof(GRASSTYPE) * GRASS_MAX_TYPE);

	m_pA3DStream		= NULL;

	m_bRayTraceEnable	= true;
	m_bAABBTraceEnable	= true;
}

A3DPlants::~A3DPlants()
{
}

bool A3DPlants::Init(A3DDevice * pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;

	/*m_pA3DStream = new A3DStream();
	if( NULL == m_pA3DStream )
	{
		g_A3DErrLog.Log("A3DPlants::Init, Not enough memory!");
		return false;
	}

	if( !m_pA3DStream->Init(m_pA3DDevice, A3DVT_LVERTEX, VERTEX_MAX_NUM, FACE_MAX_NUM * 3, 0, A3DSTRM_REFERENCEPTR) )
	{
		g_A3DErrLog.Log("A3DPlants::Init, Init A3DStream fail!");
		return false;
	}*/

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWIPlants = true;
		return true;
	}

	return true;
}

bool A3DPlants::Release()
{
	int		i;

	if( m_bHWIPlants )
	{
		// we have only kept the tree's frame here, so we should release them now.
		for(i=0; i<m_nNumTreeType; i++)
			m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->ReleaseFrame(m_pTreeTypes[i].pFrame);
		return true;
	}

	for(i=0; i<m_nNumTreeType; i++)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->ReleaseFrame(m_pTreeTypes[i].pFrame);
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTreeTypes[i].pSpriteTex);
		m_pTreeTypes[i].pSpriteTex = NULL;
	}

	for(i=0; i<m_nNumGrassType; i++)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->ReleaseFrame(m_pGrassTypes[i].pFrame);
	}

	if( m_pA3DStream )
	{
		m_pA3DStream->Release();
		delete m_pA3DStream;
		m_pA3DStream = NULL;
	}
	return true;
}

bool A3DPlants::Render(A3DViewport * pCurrentViewport)
{
	if( m_bHWIPlants ) return true;

	ZeroMemory(m_nVertNum, sizeof(int) * MAX_TEXTURE);
	ZeroMemory(m_nFaceNum, sizeof(int) * MAX_TEXTURE);
	m_nVertSpriteNum = 0;
	m_nFaceSpriteNum = 0;

	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	//m_pA3DStream->Appear();
	m_pA3DDevice->SetFVF(A3DFVF_A3DLVERTEX);
	m_pA3DDevice->SetAlphaRef(0x80);
	m_pA3DDevice->SetAlphaTestEnable(true);
	m_pA3DDevice->SetAlphaFunction(A3DCMP_GREATEREQUAL);
	
	if( !RenderTrees(pCurrentViewport) )
		return false;

	if( !RenderGrasses(pCurrentViewport) )
		return false;

	m_pA3DDevice->SetLightingEnable(TRUE);
	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	m_pA3DDevice->SetAlphaTestEnable(false);
	return true;
}							 

bool A3DPlants::AddTreeType(char * szTreeFile, char * szSpriteFile, int nDegNum, int nTexRow, int nTexCol, FLOAT vTexDU, FLOAT vTexDV, DWORD * pTreeHandle, A3DVECTOR3& vecScale)
{
	A3DFrame * pTreeFrame;

	if( m_nNumTreeType >= TREE_MAX_TYPE )
	{
		g_A3DErrLog.Log("A3DPlants::AddTreeType(), Too many tree types have been added!");
		return false;
	}
	
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->LoadMoxFile(szTreeFile, &pTreeFrame) )
	{
		g_A3DErrLog.Log("A3DPlants::AddTreeType(), Load tree file [%s] fail!", szTreeFile);
		return false;
	}

	TREETYPE	treeType;
	if( m_bHWIPlants )
	{
		// We need only kept the treetype's frame here;
		if( pTreeFrame->GetNumChilds() > 0 )
		{
			g_A3DErrLog.Log("A3DPlants::AddTreeType(), the tree file [%s] contains hyrarchies, this is not supported!", szTreeFile);
			return false;
		}
		
		// Now get the tree information;
		ZeroMemory(&treeType, sizeof(TREETYPE));
		strncpy(treeType.szTreeFile, szTreeFile, MAX_PATH);
		strncpy(treeType.szSpriteFile, szSpriteFile, MAX_PATH);
		treeType.vecScale = vecScale;
		treeType.pFrame = pTreeFrame;
		treeType.nDegNum = nDegNum;
		treeType.nTexRow = nTexRow;
		treeType.nTexCol = nTexCol;
		treeType.vTexDU = vTexDU;
		treeType.vTexDV = vTexDV;

		// Then we must dig out the bouding box;
		if( pTreeFrame->GetFrameOBBNum() != 1 )
		{
			g_A3DErrLog.Log("A3DPlants::AddTreeType(), the tree [%s] must have one (and only one) BB_ dummy object on it!", szTreeFile);
			return false;
		}
	}
	else
	{
		if( pTreeFrame->GetNumChilds() > 0 )
		{
			g_A3DErrLog.Log("A3DPlants::AddTreeType(), the tree file [%s] contains hyrarchies, this is not supported!", szTreeFile);
			return false;
		}
		if( pTreeFrame->GetNumMeshes() == 0 )
		{
			g_A3DErrLog.Log("A3DPlants::AddTreeType(), the tree file [%s] is empty!", szTreeFile);
			return false;
		}

		// Now get the tree information;
		A3DMesh		*pTreeMesh;

		ZeroMemory(&treeType, sizeof(TREETYPE));
		strncpy(treeType.szTreeFile, szTreeFile, MAX_PATH);
		strncpy(treeType.szSpriteFile, szSpriteFile, MAX_PATH);
		treeType.vecScale = vecScale;
		treeType.pFrame = pTreeFrame;
		treeType.nDegNum = nDegNum;
		treeType.nTexRow = nTexRow;
		treeType.nTexCol = nTexCol;
		treeType.vTexDU = vTexDU;
		treeType.vTexDV = vTexDV;

		// First retrieve mesh information;
		pTreeMesh = pTreeFrame->GetFirstMesh();
		while( pTreeMesh )
		{
			if( treeType.nNumTex >= MAX_TEXTURE )
			{
				g_A3DErrLog.Log("A3DPlants::AddTreeType(), the tree file [%s] used more than %d textures!", szTreeFile, MAX_TEXTURE);
				return false;
			}

			treeType.pMeshes[treeType.nNumTex] = pTreeMesh;
			treeType.pTreeTex[treeType.nNumTex] = pTreeMesh->GetTexture();
			treeType.nNumTex ++;
			
			pTreeMesh = pTreeFrame->GetNextMesh();
		}

		// Then we must dig out the bouding box;
		if( pTreeFrame->GetFrameOBBNum() != 1 )
		{
			g_A3DErrLog.Log("A3DPlants::AddTreeType(), the tree [%s] must have one (and only one) BB_ dummy object on it!", szTreeFile);
			return false;
		}

		char szRelativePath[MAX_PATH];
		char szTextureName[MAX_PATH];

		af_GetFilePath(szTreeFile, szRelativePath, MAX_PATH);
		if( szRelativePath[0] == '\0' )
		{
			sprintf(szTextureName, "%s\\Textures\\%s", m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->GetFolderName(),
				szSpriteFile);
		}
		else
		{
			sprintf(szTextureName, "%s\\%s\\Textures\\%s", m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->GetFolderName(),
				szRelativePath, szSpriteFile);
		}
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szTextureName, &treeType.pSpriteTex) )
			return false;

		// For 2D sprites' width and height;
		treeType.vHeight = pTreeFrame->GetFrameAutoAABB(0).Extents.y * 2.0f;
		treeType.vWidth = max2(pTreeFrame->GetFrameAutoAABB(0).Extents.x, pTreeFrame->GetFrameAutoAABB(0).Extents.z);
	}

	m_pTreeTypes[m_nNumTreeType] = treeType;
	*pTreeHandle = (DWORD) &m_pTreeTypes[m_nNumTreeType];

	m_nNumTreeType ++;
	return true;
}

bool A3DPlants::SetTreeTypeScale(DWORD dwTreeHandle, A3DVECTOR3& vecScale)
{
	TREETYPE * pTreeType = (TREETYPE *)dwTreeHandle;

	pTreeType->vecScale = vecScale;

	return UpdateAllTreesScale(pTreeType);
}

bool A3DPlants::UpdateAllTreesScale(A3DPlants::TREETYPE * pTreeType)
{
	A3DMATRIX4 matScale = a3d_Scaling(pTreeType->vecScale.x, pTreeType->vecScale.y, pTreeType->vecScale.z);

	A3DVECTOR3 vecDir, vecUp, vecPos;
	for(int i=0; i<pTreeType->nNumTree; i++)
	{
		int idTree = pTreeType->wTreeIndex[i];

		// First retrieve dir, up and pos info from the matTM
		vecDir = Normalize(m_pTrees[idTree].matTM.GetRow(0));
		vecUp  = Normalize(m_pTrees[idTree].matTM.GetRow(1));
		vecPos = m_pTrees[idTree].matTM.GetRow(3);

		m_pTrees[idTree].matTM = matScale * a3d_TransformMatrix(vecDir, vecUp, vecPos);
		// For collision detection;
		A3DOBB treeOBB = pTreeType->pFrame->GetFrameOBB(0).a3dOBB;
		treeOBB.Center = treeOBB.Center * m_pTrees[idTree].matTM;
		treeOBB.XAxis = Normalize(treeOBB.XAxis * m_pTrees[idTree].matTM - m_pTrees[idTree].matTM.GetRow(3));
		treeOBB.YAxis = Normalize(treeOBB.YAxis * m_pTrees[idTree].matTM - m_pTrees[idTree].matTM.GetRow(3));
		treeOBB.ZAxis = Normalize(treeOBB.ZAxis * m_pTrees[idTree].matTM - m_pTrees[idTree].matTM.GetRow(3));
		treeOBB.Extents.x *= pTreeType->vecScale.x;
		treeOBB.Extents.y *= pTreeType->vecScale.y;
		treeOBB.Extents.z *= pTreeType->vecScale.z;
		a3d_CompleteOBB(&treeOBB);
		m_pTrees[idTree].obb = treeOBB;

		if( m_bHWIPlants )
		{
		}
		else
		{
			// For visiblity determination;
			A3DAABB treeAABB = pTreeType->pFrame->GetFrameAutoAABB(0);
			A3DOBB	obb;
			obb.Center = treeAABB.Center * m_pTrees[idTree].matTM;
			obb.XAxis = m_pTrees[idTree].matTM.GetRow(0);
			obb.YAxis = m_pTrees[idTree].matTM.GetRow(1);
			obb.ZAxis = m_pTrees[idTree].matTM.GetRow(2);
			obb.Extents = treeAABB.Extents;
			a3d_CompleteOBB(&obb);

			a3d_ClearAABB(treeAABB.Mins, treeAABB.Maxs);
			a3d_ExpandAABB(treeAABB.Mins, treeAABB.Maxs, treeOBB);
			a3d_CompleteAABB(&treeAABB);
			m_pTrees[idTree].aabb = treeAABB;
		}
	}
	return true;
}

A3DPlants::TREETYPE * A3DPlants::FindTreeType(char * szTreeFile)
{
	for(int i=0; i<m_nNumTreeType; i++)
	{
		if( _stricmp(szTreeFile, m_pTreeTypes[i].szTreeFile) == 0 )
			return &m_pTreeTypes[i];
	}
	return NULL;
}

bool A3DPlants::AddTree(char * szTreeFile, TREEINFO * pTreeInfo)
{
	TREETYPE * pTreeType = FindTreeType(szTreeFile);

	if( NULL == pTreeType )
	{
		g_A3DErrLog.Log("A3DPlants::AddTree(), Can not find tree type [%s]!", szTreeFile);
		return false;
	}

	return AddTree((DWORD)pTreeType, pTreeInfo);
}

void A3DPlants::UpdateTreeInfo(int nTreeIndex, TREEINFO * pTreeInfo)
{
	if( nTreeIndex >= m_nNumTree )
		return;

	TREETYPE * pTreeType = m_pTrees[nTreeIndex].pType;

	// Update the position related informations;
	m_pTrees[nTreeIndex].matTM = a3d_TransformMatrix(pTreeInfo->vecDir, pTreeInfo->vecUp, pTreeInfo->vecPos);
	m_pTrees[nTreeIndex].nFrame = 0;
	
	// For collision detection;
	A3DOBB treeOBB = pTreeType->pFrame->GetFrameOBB(0).a3dOBB;
	treeOBB.Center = treeOBB.Center * m_pTrees[nTreeIndex].matTM;
	treeOBB.XAxis = treeOBB.XAxis * m_pTrees[nTreeIndex].matTM - m_pTrees[nTreeIndex].matTM.GetRow(3);
	treeOBB.YAxis = treeOBB.YAxis * m_pTrees[nTreeIndex].matTM - m_pTrees[nTreeIndex].matTM.GetRow(3);
	treeOBB.ZAxis = treeOBB.ZAxis * m_pTrees[nTreeIndex].matTM - m_pTrees[nTreeIndex].matTM.GetRow(3);
	a3d_CompleteOBB(&treeOBB);
	m_pTrees[nTreeIndex].obb = treeOBB;

	// For visiblity determination;
	A3DAABB treeAABB;
	a3d_ClearAABB(treeAABB.Mins, treeAABB.Maxs);
	a3d_ExpandAABB(treeAABB.Mins, treeAABB.Maxs, treeOBB);
	a3d_CompleteAABB(&treeAABB);
	m_pTrees[nTreeIndex].aabb = treeAABB;
	return;
}

bool A3DPlants::AddTree(DWORD dwTreeHandle, TREEINFO * pTreeInfo, int * pTreeIndex)
{
	TREETYPE * pTreeType = (TREETYPE *) dwTreeHandle;
	A3DMATRIX4 matScale = a3d_Scaling(pTreeType->vecScale.x, pTreeType->vecScale.y, pTreeType->vecScale.z);

	m_pTrees[m_nNumTree].pType = pTreeType;
	m_pTrees[m_nNumTree].matTM = matScale * a3d_TransformMatrix(pTreeInfo->vecDir, pTreeInfo->vecUp, pTreeInfo->vecPos);
	m_pTrees[m_nNumTree].nFrame = 0;
	m_pTrees[m_nNumTree].nTreeTypeIndex = pTreeType->nNumTree;
	pTreeType->wTreeIndex[pTreeType->nNumTree ++] = m_nNumTree;

	// For collision detection;
	A3DOBB treeOBB = pTreeType->pFrame->GetFrameOBB(0).a3dOBB;
	treeOBB.Center = treeOBB.Center * m_pTrees[m_nNumTree].matTM;
	treeOBB.XAxis = Normalize(treeOBB.XAxis * m_pTrees[m_nNumTree].matTM - m_pTrees[m_nNumTree].matTM.GetRow(3));
	treeOBB.YAxis = Normalize(treeOBB.YAxis * m_pTrees[m_nNumTree].matTM - m_pTrees[m_nNumTree].matTM.GetRow(3));
	treeOBB.ZAxis = Normalize(treeOBB.ZAxis * m_pTrees[m_nNumTree].matTM - m_pTrees[m_nNumTree].matTM.GetRow(3));
	treeOBB.Extents.x *= pTreeType->vecScale.x;
	treeOBB.Extents.y *= pTreeType->vecScale.y;
	treeOBB.Extents.z *= pTreeType->vecScale.z;
	a3d_CompleteOBB(&treeOBB);
	m_pTrees[m_nNumTree].obb = treeOBB;

	// Build OBB bevels used to do AABB trace
	TRA_BuildOBBBevels(treeOBB, &m_pTrees[m_nNumTree].Bevels);

	if( m_bHWIPlants )
	{
	}
	else
	{
		// For visiblity determination;
		A3DAABB treeAABB = pTreeType->pFrame->GetFrameAutoAABB(0);
		A3DOBB	obb;
		obb.Center = treeAABB.Center * m_pTrees[m_nNumTree].matTM;
		obb.XAxis = m_pTrees[m_nNumTree].matTM.GetRow(0);
		obb.YAxis = m_pTrees[m_nNumTree].matTM.GetRow(1);
		obb.ZAxis = m_pTrees[m_nNumTree].matTM.GetRow(2);
		obb.Extents = treeAABB.Extents;
		a3d_CompleteOBB(&obb);

		a3d_ClearAABB(treeAABB.Mins, treeAABB.Maxs);
		a3d_ExpandAABB(treeAABB.Mins, treeAABB.Maxs, obb);
		a3d_CompleteAABB(&treeAABB);
		m_pTrees[m_nNumTree].aabb = treeAABB;
	}

	if( pTreeIndex )
		*pTreeIndex = m_nNumTree;

	m_nNumTree ++;
	return true;
}

bool A3DPlants::DelTree(int nTreeIndex, int * pNewTreeIndex)
{
	if( nTreeIndex >= m_nNumTree )
	{
		g_A3DErrLog.Log("A3DPlants::DelTree(), the index [%d] is beyond the tree num [%d]", nTreeIndex, m_nNumTree);
		return false;
	}

	TREETYPE * pTreeType = m_pTrees[nTreeIndex].pType;
	
	// First remove the record from tree type;
	// Swap this tree index with the last one;
	int nTreeTypeIndex = m_pTrees[nTreeIndex].nTreeTypeIndex;

	WORD wTypeLastTreeIndex;
	wTypeLastTreeIndex = pTreeType->wTreeIndex[nTreeTypeIndex] = pTreeType->wTreeIndex[-- pTreeType->nNumTree];

	// First we should find the tree of the type in m_pTrees, and modify its info;
	m_pTrees[wTypeLastTreeIndex].nTreeTypeIndex = nTreeTypeIndex;

	// Then remove the trees from tree array;
	// Swap this tree with the last tree
	m_pTrees[nTreeIndex] = m_pTrees[-- m_nNumTree];

	// Last we should modify the tree type's wTreeIndex array to find out the last tree, and
	// Set it tree index to current new index;
	pTreeType = m_pTrees[nTreeIndex].pType;
	pTreeType->wTreeIndex[m_pTrees[nTreeIndex].nTreeTypeIndex] = nTreeIndex;
	
	if( pNewTreeIndex )
		*pNewTreeIndex = m_nNumTree;
	return true;
}

bool A3DPlants::AddGrassType(char * szGrassFile, DWORD * pGrassHandle, A3DVECTOR3& vecScale)
{
	A3DFrame * pGrassFrame;

	if( m_bHWIPlants )
	{
		*pGrassHandle = 0;
		return true;
	}

	if( m_nNumGrassType >= GRASS_MAX_TYPE )
	{
		g_A3DErrLog.Log("A3DPlants::AddGrassType(), Too many grass types have been added!");
		return false;
	}
	
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->LoadMoxFile(szGrassFile, &pGrassFrame) )
	{
		g_A3DErrLog.Log("A3DPlants::AddGrassType(), Load grass file [%s] fail!", szGrassFile);
		return false;
	}

	if( pGrassFrame->GetNumChilds() > 0 )
	{
		g_A3DErrLog.Log("A3DPlants::AddGrassType(), the grass file [%s] contains hyrarchies, this is not supported!", szGrassFile);
		return false;
	}
	if( pGrassFrame->GetNumMeshes() == 0 )
	{
		g_A3DErrLog.Log("A3DPlants::AddGrassType(), the grass file [%s] is empty!", szGrassFile);
		return false;
	}

	// Now get the tree information;
	GRASSTYPE	grassType;
	A3DMesh		*pGrassMesh;

	ZeroMemory(&grassType, sizeof(GRASSTYPE));
	strncpy(grassType.szGrassFile, szGrassFile, MAX_PATH);
	grassType.vecScale = vecScale;

	// First retrieve mesh information;
	grassType.pFrame = pGrassFrame;
	pGrassMesh = pGrassFrame->GetFirstMesh();
	while( pGrassMesh )
	{
		if( grassType.nNumTex >= MAX_TEXTURE )
		{
			g_A3DErrLog.Log("A3DPlants::AddGrassType(), the grass file [%s] used more than %d textures!", szGrassFile, MAX_TEXTURE);
			return false;
		}

		grassType.pMeshes[grassType.nNumTex] = pGrassMesh;
		grassType.pGrassTex[grassType.nNumTex] = pGrassMesh->GetTexture();
		grassType.nNumTex ++;
		
		pGrassMesh = pGrassFrame->GetNextMesh();
	}

	m_pGrassTypes[m_nNumGrassType] = grassType;
	*pGrassHandle = (DWORD) &m_pGrassTypes[m_nNumGrassType];

	m_nNumGrassType ++;
	return true;
}

bool A3DPlants::SetGrassTypeScale(DWORD dwGrassHandle, A3DVECTOR3& vecScale)
{
	if( m_bHWIPlants )	return true;

	GRASSTYPE * pGrassType = (GRASSTYPE *) dwGrassHandle;
	pGrassType->vecScale = vecScale;

	return UpdateAllGrassesScale(pGrassType);
}

bool A3DPlants::UpdateAllGrassesScale(A3DPlants::GRASSTYPE * pGrassType)
{
	if( m_bHWIPlants )	return true;

	A3DMATRIX4 matScale = a3d_Scaling(pGrassType->vecScale.x, pGrassType->vecScale.y, pGrassType->vecScale.z);

	A3DVECTOR3 vecDir, vecUp, vecPos;
	for(int i=0; i<pGrassType->nNumGrass; i++)
	{
		int idGrass = pGrassType->wGrassIndex[i];

		// First retrieve dir, up and pos info from the matTM
		vecDir = Normalize(m_pGrasses[idGrass].matTM.GetRow(0));
		vecUp  = Normalize(m_pGrasses[idGrass].matTM.GetRow(1));
		vecPos = m_pGrasses[idGrass].matTM.GetRow(3);

		m_pGrasses[idGrass].matTM = matScale * a3d_TransformMatrix(vecDir, vecUp, vecPos);
	}
	return true;
}

A3DPlants::GRASSTYPE * A3DPlants::FindGrassType(char * szGrassFile)
{
	for(int i=0; i<m_nNumGrassType; i++)
	{
		if( _stricmp(szGrassFile, m_pGrassTypes[i].szGrassFile) == 0 )
			return &m_pGrassTypes[i];
	}
	return NULL;
}

void A3DPlants::UpdateGrassInfo(int nGrassIndex, GRASSINFO * pGrassInfo)
{
	if( m_bHWIPlants )	return;

	if( nGrassIndex < m_nNumGrass )
		return;

	GRASSTYPE * pGrassType = m_pGrasses[nGrassIndex].pType;

	m_pGrasses[nGrassIndex].matTM = a3d_TransformMatrix(pGrassInfo->vecDir, pGrassInfo->vecUp, pGrassInfo->vecPos);
	m_pGrasses[m_nNumGrass].nFrame = rand() % pGrassType->pMeshes[0]->GetFrameCount();
}

bool A3DPlants::AddGrass(DWORD dwGrassHandle, GRASSINFO * pGrassInfo, int * pGrassIndex)
{
	if( m_bHWIPlants )
	{
		*pGrassIndex = 0;
		return true;
	}

	GRASSTYPE * pGrassType = (GRASSTYPE *) dwGrassHandle;
	A3DMATRIX4  matScale = a3d_Scaling(pGrassType->vecScale.x, pGrassType->vecScale.y, pGrassType->vecScale.z) *
		a3d_Scaling(pGrassInfo->vecScale.x, pGrassInfo->vecScale.y, pGrassInfo->vecScale.z);

	m_pGrasses[m_nNumGrass].pType = pGrassType;
	m_pGrasses[m_nNumGrass].matTM = matScale * a3d_TransformMatrix(pGrassInfo->vecDir, pGrassInfo->vecUp, pGrassInfo->vecPos);
	m_pGrasses[m_nNumGrass].nFrame = rand() % pGrassType->pMeshes[0]->GetFrameCount();
	m_pGrasses[m_nNumGrass].nGrassTypeIndex = pGrassType->nNumGrass;
	pGrassType->wGrassIndex[pGrassType->nNumGrass ++] = m_nNumGrass;

	if( pGrassIndex )
		*pGrassIndex = m_nNumGrass;

	m_nNumGrass ++;
	return true;
}

bool A3DPlants::DelGrass(int nGrassIndex, int * pNewGrassIndex)
{
	if( m_bHWIPlants ) 
	{
		*pNewGrassIndex = 0;
		return true;
	}

	if( nGrassIndex >= m_nNumGrass )
	{
		g_A3DErrLog.Log("A3DPlants::DelGrass(), the index [%d] is beyond the grass num [%d]", nGrassIndex, m_nNumGrass);
		return false;
	}

	GRASSTYPE * pGrassType = m_pGrasses[nGrassIndex].pType;
	// First remove the record from tree type;
	// Swap this tree index with the last one;
	pGrassType->wGrassIndex[m_pGrasses[nGrassIndex].nGrassTypeIndex] = 
		pGrassType->wGrassIndex[-- pGrassType->nNumGrass];

	// Then remove the trees from tree array;
	// Swap this tree with the last tree
	m_pGrasses[nGrassIndex] = m_pGrasses[-- m_nNumGrass];

	if( pNewGrassIndex )
		*pNewGrassIndex = m_nNumGrass;
	
	return true;
}

bool A3DPlants::RenderToBuffer(A3DMesh * pMesh, int nStartVert, A3DLVERTEX * pVertexBuffer, WORD * pIndices, A3DMATRIX4 absoluteTM,  int nCurrentFrame, A3DCOLOR diffuse, A3DCOLOR specular)
{
	if( m_bHWIPlants ) return true;

	A3DVERTEX	*pMyVertBuffer = pMesh->GetVerts(nCurrentFrame);
	WORD		*pMyIndices = pMesh->GetIndices();
	int			i;

	for(i=0; i<pMesh->GetVertCount(); i++)
	{
		A3DVECTOR3 vecPos = A3DVECTOR3(pMyVertBuffer[i].x, pMyVertBuffer[i].y, pMyVertBuffer[i].z);
		vecPos = vecPos * absoluteTM;
		pVertexBuffer[i] = A3DLVERTEX(vecPos, diffuse, specular, pMyVertBuffer[i].tu, pMyVertBuffer[i].tv);
	}

	for(i=0; i<pMesh->GetIndexCount(); i++)
		pIndices[i] = pMyIndices[i] + nStartVert;

	return true;
}

bool A3DPlants::RenderTrees(A3DViewport * pCurrentViewport)
{
	if( m_bHWIPlants ) return true;

	if (pCurrentViewport->GetCamera()->GetClassID() != A3D_CID_CAMERA)
		return true;

	A3DCamera* pCamera = (A3DCamera*)pCurrentViewport->GetCamera();
	
	
	A3DVECTOR3 vecCamPos = pCamera->GetPos();
	A3DVECTOR3 vecCamDir = pCamera->GetDir();
	A3DVECTOR3 vecCamUp  = pCamera->GetUp();
	FLOAT	   vDisToCam;
	A3DCOLOR	diffuse, specular;
	BYTE	   alpha;

	int		i, j, k;
	for(i=0; i<TREE_MAX_TYPE; i++)
	{
		for(j=0; j<m_pTreeTypes[i].nNumTree; j++)
		{
			TREE thisTree = m_pTrees[m_pTreeTypes[i].wTreeIndex[j]];
			A3DAABB thisAABB = thisTree.aabb;
			A3DVECTOR3 vecPos = thisTree.matTM.GetRow(3);
			A3DVECTOR3 vecFromCam = vecPos - vecCamPos;

			if( !pCamera->AABBInViewFrustum(thisAABB) )
				continue;

			if( m_bShowBox )
			{
				//g_pA3DGDI->DrawBox(thisAABB, A3DCOLORRGBA(255, 0, 0, 128));
				g_pA3DGDI->DrawBox(thisTree.obb, A3DCOLORRGBA(0, 255, 0, 128));
			}
			
			vDisToCam = vecFromCam.Magnitude() * pCamera->GetFOV() / DEG2RAD(65.0f);
			vDisToCam /= (m_pTreeTypes[i].vHeight * m_pTreeTypes[i].vecScale.y / 6.0f);

			if( m_pA3DDevice->GetA3DEngine()->GetA3DWorld() )
				m_pA3DDevice->GetA3DEngine()->GetA3DWorld()->GetA3DTerrain()->GetTerrainColor(vecPos, &diffuse, &specular);
			else
			{
				diffuse = A3DCOLORRGBA(255, 255, 255, 255);
				specular = A3DCOLORRGBA(0, 0, 0, 255);
			}

			if( vDisToCam > m_vTreeDis3 )
				continue;
			
			if( vDisToCam > m_vTreeDis1 )
			{
				vecFromCam = Normalize(vecFromCam);

				//Render as sprites;
				if( vDisToCam > m_vTreeDis2 )
				{
					alpha = (BYTE) ((m_vTreeDis3 - vDisToCam) / (m_vTreeDis3 - m_vTreeDis2) * 255);
					diffuse = (diffuse & 0x00ffffff) | (alpha << 24);
				}

				if( m_nVertSpriteNum + 4 > VERTEX_MAX_NUM || m_nFaceSpriteNum + 2 > FACE_MAX_NUM )
				{
					m_pA3DDevice->SetAlphaRef(0x80);
					//m_pA3DStream->SetVerts((BYTE *)m_pVertsSprite, m_nVertSpriteNum);
					//m_pA3DStream->SetIndices(m_pIndicesSprite, m_nFaceSpriteNum * 3);
					m_pTreeTypes[i].pSpriteTex->Appear(0);

					//m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertSpriteNum,
					//	0, m_nFaceSpriteNum);

					m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0,
						m_nVertSpriteNum, m_nFaceSpriteNum, m_pIndicesSprite, A3DFMT_INDEX16, m_pVertsSprite, sizeof(A3DLVERTEX));

					m_pTreeTypes[i].pSpriteTex->Disappear(0);
					m_nVertSpriteNum = 0;
					m_nFaceSpriteNum = 0;
				}

				// Now construct a bill board for that tree;
				int nDeg, nFrame;
				FLOAT u0, v0, u1, v1;
				A3DVECTOR3 vecAxis, vecRight;
				FLOAT vCX, vCZ;

				// The sprite series are generated from front view and rotate to right 360 within nDegNum frames;
				vCZ = DotProduct(Normalize(thisTree.matTM.GetRow(2)), vecFromCam);
				vCX = DotProduct(Normalize(thisTree.matTM.GetRow(0)), vecFromCam);
				nDeg = (int)(RAD2DEG(atan2(vCX, vCZ)));
				
				if( nDeg < 0 ) nDeg += 360;
				if( nDeg > 360 ) nDeg -= 360;
				nFrame = (int)(nDeg / (360.0f / m_pTreeTypes[i].nDegNum));

				u0 = (nFrame % m_pTreeTypes[i].nTexCol) * m_pTreeTypes[i].vTexDU + 0.01f; 
				v0 = (nFrame / m_pTreeTypes[i].nTexCol) * m_pTreeTypes[i].vTexDV + 0.01f;
				u1 = u0 + m_pTreeTypes[i].vTexDU - 0.02f; 
				v1 = v0 + m_pTreeTypes[i].vTexDV - 0.02f;

				vecAxis = Normalize(thisTree.matTM.GetRow(1)) * m_pTreeTypes[i].vHeight * m_pTreeTypes[i].vecScale.y;
				vecRight = Normalize(CrossProduct(vecAxis, vecCamDir)) * m_pTreeTypes[i].vWidth * (m_pTreeTypes[i].vecScale.y + m_pTreeTypes[i].vecScale.z) / 2.0f;
				
				m_pVertsSprite[m_nVertSpriteNum + 0] = A3DLVERTEX(vecPos - vecRight + vecAxis, diffuse, specular, u0, v0);
				m_pVertsSprite[m_nVertSpriteNum + 1] = A3DLVERTEX(vecPos + vecRight + vecAxis, diffuse, specular, u1, v0);
				m_pVertsSprite[m_nVertSpriteNum + 2] = A3DLVERTEX(vecPos - vecRight, diffuse, specular, u0, v1);
				m_pVertsSprite[m_nVertSpriteNum + 3] = A3DLVERTEX(vecPos + vecRight, diffuse, specular, u1, v1);

				m_pIndicesSprite[m_nFaceSpriteNum * 3 + 0] = m_nVertSpriteNum + 0;
				m_pIndicesSprite[m_nFaceSpriteNum * 3 + 1] = m_nVertSpriteNum + 1;
				m_pIndicesSprite[m_nFaceSpriteNum * 3 + 2] = m_nVertSpriteNum + 2;
				m_pIndicesSprite[m_nFaceSpriteNum * 3 + 3] = m_nVertSpriteNum + 2;
				m_pIndicesSprite[m_nFaceSpriteNum * 3 + 4] = m_nVertSpriteNum + 1;
				m_pIndicesSprite[m_nFaceSpriteNum * 3 + 5] = m_nVertSpriteNum + 3;

				m_nVertSpriteNum += 4;
				m_nFaceSpriteNum += 2;
				continue;
			}

			// This is a near tree, so use the tree model here;
			alpha = 255;
			for(k=0; k<m_pTreeTypes[i].nNumTex; k++)
			{
				A3DMesh * pMesh = m_pTreeTypes[i].pMeshes[k];
				if( m_nVertNum[k] + pMesh->GetVertCount() > VERTEX_MAX_NUM ||
					m_nFaceNum[k] * 3 + pMesh->GetIndexCount() > FACE_MAX_NUM * 3 )
				{
					m_pA3DDevice->SetAlphaRef(0x80);
					//m_pA3DStream->SetVerts((BYTE *)m_pVerts[k], m_nVertNum[k]);
					//m_pA3DStream->SetIndices(m_pIndices[k], m_nFaceNum[k] * 3);
					m_pTreeTypes[i].pTreeTex[k]->Appear(0);

					//m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertNum[k],
					//	0, m_nFaceNum[k]);

					m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0,
						m_nVertNum[k], m_nFaceNum[k], m_pIndices[k], A3DFMT_INDEX16, m_pVerts[k], sizeof(A3DLVERTEX));

					m_pTreeTypes[i].pTreeTex[k]->Disappear(0);
					m_nVertNum[k] = 0;
					m_nFaceNum[k] = 0;
				}

				if( !RenderToBuffer(pMesh, m_nVertNum[k], m_pVerts[k] + m_nVertNum[k],
						m_pIndices[k] + m_nFaceNum[k] * 3, thisTree.matTM, 
						thisTree.nFrame, diffuse, specular) )
				{
					g_A3DErrLog.Log("A3DPlants::Render(), RenderToBuffer Fail!");
					return false;
				}

				m_nVertNum[k] += pMesh->GetVertCount();
				m_nFaceNum[k] += pMesh->GetIndexCount() / 3;
			}
		}

		// Now see if still left some verts to render;
		for(k=0; k<m_pTreeTypes[i].nNumTex; k++)
		{
			if( m_nVertNum[k] && m_nFaceNum[k] )
			{
				m_pA3DDevice->SetAlphaRef(0x80);
				//m_pA3DStream->SetVerts((BYTE *)m_pVerts[k], m_nVertNum[k]);
				//m_pA3DStream->SetIndices(m_pIndices[k], m_nFaceNum[k] * 3);
				m_pTreeTypes[i].pTreeTex[k]->Appear(0);

				//m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertNum[k],
				//	0, m_nFaceNum[k]);

				m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0,
					m_nVertNum[k], m_nFaceNum[k], m_pIndices[k], A3DFMT_INDEX16, m_pVerts[k], sizeof(A3DLVERTEX));

				m_pTreeTypes[i].pTreeTex[k]->Disappear(0);
				m_nVertNum[k] = 0;
				m_nFaceNum[k] = 0;
			}
		}
		if( m_nVertSpriteNum && m_nFaceSpriteNum )
		{
			m_pA3DDevice->SetAlphaRef(0x80);
			//m_pA3DStream->SetVerts((BYTE *)m_pVertsSprite, m_nVertSpriteNum);
			//m_pA3DStream->SetIndices(m_pIndicesSprite, m_nFaceSpriteNum * 3);
			m_pTreeTypes[i].pSpriteTex->Appear(0);

			//m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertSpriteNum,
			//	0, m_nFaceSpriteNum);
			m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0,
				m_nVertSpriteNum, m_nFaceSpriteNum, m_pIndicesSprite, A3DFMT_INDEX16, m_pVertsSprite, sizeof(A3DLVERTEX));

			m_pTreeTypes[i].pSpriteTex->Disappear(0);
			m_nVertSpriteNum = 0;
			m_nFaceSpriteNum = 0;
		}
	}
	return true;
}

bool A3DPlants::RenderGrasses(A3DViewport * pCurrentViewport)
{
	if( m_bHWIPlants ) return true;

	if (pCurrentViewport->GetCamera()->GetClassID() != A3D_CID_CAMERA)
		return true;

	A3DCamera* pCamera = (A3DCamera*)pCurrentViewport->GetCamera();
	A3DVECTOR3 vecCamPos = pCamera->GetPos();
	A3DVECTOR3 vecCamDir = pCamera->GetDir();
	A3DVECTOR3 vecCamUp  = pCamera->GetUp();

	FLOAT	   vDisToCam;
	BYTE	   alpha;
	A3DCOLOR   diffuse, specular;

	int		i, j, k;

	for(i=0; i<GRASS_MAX_TYPE; i++)
	{
		for(j=0; j<m_pGrassTypes[i].nNumGrass; j++)
		{
			GRASS thisGrass = m_pGrasses[m_pGrassTypes[i].wGrassIndex[j]];
			A3DVECTOR3 vecPos = thisGrass.matTM.GetRow(3);
			A3DVECTOR3 vecFromCam = vecPos - vecCamPos;

			// See if it is behind the camera;
			if( DotProduct(vecFromCam, vecCamDir) < 0.0f )
				continue;

			// Update To Next Frame, this should be done in tick animation function;
			m_pGrasses[m_pGrassTypes[i].wGrassIndex[j]].nFrame = (m_pGrasses[m_pGrassTypes[i].wGrassIndex[j]].nFrame + 1) % m_pGrassTypes[i].pMeshes[0]->GetFrameCount();

			vDisToCam = vecFromCam.Magnitude() * pCamera->GetFOV() / DEG2RAD(65.0f);
			vDisToCam /= (m_pGrassTypes[i].pFrame->GetFrameAutoAABB(0).Extents.y * m_pGrassTypes[i].vecScale.y / 1.0f);
			
			// If far away enough we use only sprites;
			if( vDisToCam > m_vGrassDis2 )
				continue;

			if( m_pA3DDevice->GetA3DEngine()->GetA3DWorld() )
				m_pA3DDevice->GetA3DEngine()->GetA3DWorld()->GetA3DTerrain()->GetTerrainColor(vecPos, &diffuse, &specular);
			else
			{
				diffuse = A3DCOLORRGBA(255, 255, 255, 255);
				specular = A3DCOLORRGBA(0, 0, 0, 255);
			}

			if( vDisToCam > m_vGrassDis1 )
			{
				alpha = (BYTE) ((m_vGrassDis2 - vDisToCam) / (m_vGrassDis2 - m_vGrassDis1) * 255);
				diffuse = (diffuse & 0x00ffffff) | (alpha << 24);
			}

			for(k=0; k<m_pGrassTypes[i].nNumTex; k++)
			{
				A3DMesh * pMesh = m_pGrassTypes[i].pMeshes[k];
				if( m_nVertNum[k] + pMesh->GetVertCount() > VERTEX_MAX_NUM ||
					m_nFaceNum[k] * 3 + pMesh->GetIndexCount() > FACE_MAX_NUM * 3 )
				{
					m_pA3DDevice->SetAlphaRef(0x80);
					//m_pA3DStream->SetVerts((BYTE *)m_pVerts[k], m_nVertNum[k]);
					//m_pA3DStream->SetIndices(m_pIndices[k], m_nFaceNum[k] * 3);
					m_pGrassTypes[i].pGrassTex[k]->Appear(0);

					//m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertNum[k],
					//	0, m_nFaceNum[k]);
					m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, m_nVertNum[k],
						m_nFaceNum[k], m_pIndices[k], A3DFMT_INDEX16, m_pVerts[k], sizeof(A3DLVERTEX));

					m_pGrassTypes[i].pGrassTex[k]->Disappear(0);
					m_nVertNum[k] = 0;
					m_nFaceNum[k] = 0;
				}

				if( !RenderToBuffer(pMesh, m_nVertNum[k], m_pVerts[k] + m_nVertNum[k],
					m_pIndices[k] + m_nFaceNum[k] * 3, thisGrass.matTM, thisGrass.nFrame, diffuse, specular) )
				{
					g_A3DErrLog.Log("A3DPlants::RenderGrasses(), RenderToBuffer Fail!");
					return false;
				}

				m_nVertNum[k] += pMesh->GetVertCount();
				m_nFaceNum[k] += pMesh->GetIndexCount() / 3;
			}
		}

		// Now see if still left some verts to render;
		for(k=0; k<m_pGrassTypes[i].nNumTex; k++)
		{
			if( m_nVertNum[k] && m_nFaceNum[k] )
			{
				m_pA3DDevice->SetAlphaRef(0x80);
				//m_pA3DStream->SetVerts((BYTE *)m_pVerts[k], m_nVertNum[k]);
				//m_pA3DStream->SetIndices(m_pIndices[k], m_nFaceNum[k] * 3);
				m_pGrassTypes[i].pGrassTex[k]->Appear(0);

				//m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertNum[k],
				//	0, m_nFaceNum[k]);
				m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, m_nVertNum[k],
					m_nFaceNum[k], m_pIndices[k], A3DFMT_INDEX16, m_pVerts[k], sizeof(A3DLVERTEX));

				m_pGrassTypes[i].pGrassTex[k]->Disappear(0);
				m_nVertNum[k] = 0;
				m_nFaceNum[k] = 0;
			}
		}
	}
	return true;
}

bool A3DPlants::AABBTrace(AABBTRACEINFO* pInfo, AABBTRACERT* pTrace)
{
	if( !m_bAABBTraceEnable )
		return false;

	bool bHit = false;
	A3DVECTOR3 vNormal;
	A3DAABB	aabb;
	A3DOBB obb;

	pTrace->fFraction = 1.0f;

	for(int i=0; i<m_nNumTree; i++)
	{
		aabb = m_pTrees[i].aabb;
		if (CLS_AABBAABBOverlap(pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents, aabb.Center, aabb.Extents) )
		{
			obb = m_pTrees[i].obb;
			if (TRA_AABBMoveToBrush(pInfo, m_pTrees[i].Bevels.aPlaneAddrs, m_pTrees[i].Bevels.iNumPlane))
			{
				bHit = true;

				if (pInfo->fFraction < pTrace->fFraction)
				{
					pTrace->fFraction	= pInfo->fFraction;
					pTrace->vNormal		= pInfo->ClipPlane.vNormal;

					m_AABBTraceRt.iTreeIndex = i;
				}
			}
		}
	}

	if (bHit)	//	Yes collided
	{
		pTrace->vDestPos = pInfo->vStart + pInfo->vDelta * pTrace->fFraction;
		return true;
	}

	return false;
}

bool A3DPlants::RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, RAYTRACERT* pRayTrace)
{
	if( !m_bRayTraceEnable )
		return false;
	
	FLOAT		vFraction;
	A3DVECTOR3	vecNormal;
	A3DVECTOR3	vecPoint;
	int			nTreeIndex = -1;

	pRayTrace->fFraction = 1.0f;
	
	for(int i=0; i<m_nNumTree; i++)
	{
		if( CLS_RayToAABB3(vecStart, vecDelta, m_pTrees[i].aabb.Mins, m_pTrees[i].aabb.Maxs, vecPoint, &vFraction, vecNormal) )
		{
			if( CLS_RayToOBB3(vecStart, vecDelta, m_pTrees[i].obb, vecPoint, &vFraction, vecNormal) )
			{
				if( vFraction < pRayTrace->fFraction )
				{
					pRayTrace->fFraction	= vFraction;
					pRayTrace->vPoint		= vecPoint;
					pRayTrace->vNormal		= vecNormal;

					m_RayTraceRt.iTreeIndex = i;
				}
			}
		}
	}

	if( pRayTrace->fFraction < 1.0f )
	{
		//Yes collided;
		return true;
	}

	return false;
}

bool A3DPlants::Reset()
{
	int i;

	if( m_bHWIPlants )
	{
		// We have only kept the tree's frame, so just release it here;
		for(i=0; i<m_nNumTreeType; i++)
			m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->ReleaseFrame(m_pTreeTypes[i].pFrame);
		m_nNumTreeType = 0;
		m_nNumGrassType = 0;

		m_nNumTree		= 0;
		m_nNumGrass		= 0;

		ZeroMemory(m_pTreeTypes, sizeof(TREETYPE) * TREE_MAX_TYPE);
		ZeroMemory(m_pGrassTypes, sizeof(GRASSTYPE) * GRASS_MAX_TYPE);
		return true;
	}

	// Now come to common release procedure;
	for(i=0; i<m_nNumTreeType; i++)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->ReleaseFrame(m_pTreeTypes[i].pFrame);
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTreeTypes[i].pSpriteTex);;
		m_pTreeTypes[i].pSpriteTex = NULL;
	}
	m_nNumTreeType = 0;

	for(i=0; i<m_nNumGrassType; i++)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DMoxMan()->ReleaseFrame(m_pGrassTypes[i].pFrame);
	}
	m_nNumGrassType = 0;

	m_nNumTree		= 0;
	m_nNumGrass		= 0;

	ZeroMemory(m_pTreeTypes, sizeof(TREETYPE) * TREE_MAX_TYPE);
	ZeroMemory(m_pGrassTypes, sizeof(GRASSTYPE) * GRASS_MAX_TYPE);
	return true;
}
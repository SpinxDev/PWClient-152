/*
 * FILE: EL_Tree.h
 *
 * DESCRIPTION: Tree class for one type in element
 *
 * CREATED BY: Hedi, 2004/9/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include <A3DDevice.h>
#include <A3DFuncs.h>
#include <A3DEngine.h>
#include <A3DTexture.h>
#include <A3DTextureMan.h>
#include <A3DViewport.h>
#include <A3DStream.h>
#include <AF.h>	  	  
#include <A3DCameraBase.h>
#include <A3DCollision.h>
#include <A3DPixelShader.h>
#include <A3DVertexShader.h>

#include "EL_Tree.h"
#include "EL_Forest.h"
#include "ConvexHullData.h"
#include "aabbcd.h"
#include "CDWithCH.h"

CELTree::CELTree()
{
	m_pSpeedTree		= NULL;
	m_pTextureInfo		= NULL;
	m_pGeometryCache	= NULL;

	m_pA3DDevice		= NULL;
	m_pForest			= NULL;
	m_pLogFile			= NULL;

	m_dwIDNext			= 0;

	m_szCompositeMap[0]		= '\0';
	m_szBranchMap[0]		= '\0';
#ifdef TREE_BUMP_ENABLE
	m_szBranchNormalMap[0]	= '\0';
	m_pTextureBranchNormal	= NULL;
#endif
	m_pTextureComposite		= NULL;
	m_pTextureBranch		= NULL;
	
	// now vertex buffer variables preset to NULL;
	m_pBranchStream		= NULL;
	m_pBranchVerts		= NULL;
	m_pBranchIndices	= NULL;
	m_pBranchIndexCount	= NULL ;

	m_pFrondStream		= NULL;
	m_pFrondVerts		= NULL;
	m_pFrondIndices		= NULL;
	m_pFrondIndexCount	= NULL ;
	m_bBadTree			= true;

	m_nNumCDObject		= 0;
	m_ppConvexHulls		= NULL;
	m_nNumCDBrushes		= 0;
	m_ppCDBrushes		= NULL;
}

CELTree::~CELTree()
{
	Release();
}

bool CELTree::Init(A3DDevice * pA3DDevice, CELForest * pForest, DWORD dwTypeID, const char * szTreeFile, const char * szCompositeMap, ALog * pLogFile)
{
	m_pA3DDevice	= pA3DDevice;
	m_pForest		= pForest;
	m_pLogFile		= pLogFile;

	m_dwTypeID		= dwTypeID;
	strcpy(m_szSPTFile, szTreeFile);
	af_GetFileTitle(szTreeFile, m_szTreeName, 256);
	
	// default orient
	m_vecDir = A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_vecUp = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	m_vecPos = A3DVECTOR3(0.0f, 0.0f, 0.0f);

	m_pSpeedTree = new CSpeedTreeRT();

	// direct x should call texture flip for speedtree
	m_pSpeedTree->SetTextureFlip(true);

	// load the tree file
	AFileImage fileTree;
	if( !fileTree.Open(szTreeFile, AFILE_OPENEXIST) )
	{
		m_pLogFile->Log("CELTree::Init(), failed to load speed tree file [%s]!", szTreeFile);
		// not found original tree, so try to use the bad tree instead
		if( !fileTree.Open("trees\\badtree.spt", AFILE_OPENEXIST) )
		{
			m_pLogFile->Log("CELTree::Init(), failed to load bad tree, so tree type will not be initialized!");
			return false;
		}
	}

	if( !m_pSpeedTree->LoadTree(fileTree.GetFileBuffer(), fileTree.GetFileLength()) )
	{
		m_pLogFile->Log("CELTree::Init(), failed to load speed tree file [%s]!", fileTree.GetFileName());
	}

	m_bBadTree = false;

	// now set some setup data for speed tree
	m_pSpeedTree->SetBranchLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
	m_pSpeedTree->SetLeafLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
	m_pSpeedTree->SetFrondLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
	
	m_pSpeedTree->SetBranchWindMethod(CSpeedTreeRT::WIND_NONE);
	m_pSpeedTree->SetLeafWindMethod(CSpeedTreeRT::WIND_NONE);
	m_pSpeedTree->SetFrondWindMethod(CSpeedTreeRT::WIND_NONE);
	
	m_pSpeedTree->SetNumLeafRockingGroups(1);

	float vTreeSize, vVariance;
	m_pSpeedTree->GetTreeSize(vTreeSize, vVariance);
	m_pSpeedTree->SetTreeSize(vTreeSize * TREEUNIT_SCALE);

    // generate tree geometry
    if( !m_pSpeedTree->Compute(NULL) )
	{
		m_pLogFile->Log("CELTree::Init(), failed to call speedtree's Compute method!");
		return false;
	}
    // get the dimensions
	m_pSpeedTree->GetBoundingBox(m_afBoundingBox);
	// adjust lod distances
    float fHeight = m_afBoundingBox[5] - m_afBoundingBox[2];
	if( fHeight < 5.0f )
		fHeight = 5.0f;
	fHeight *= m_pForest->GetLODLevel();
	m_pSpeedTree->SetLodLimits(fHeight * 5.0f, fHeight * 16.0f);
	
	// make the leaves rock in the wind
	m_pSpeedTree->SetLeafRockingState(true);

    // billboard setup
	// now billboard has some bugs, so we turn off it temporaly
	CSpeedTreeRT::SetDropToBillboard(false);
	
    // query textures
	m_pTextureInfo = new CSpeedTreeRT::STextures;
	m_pSpeedTree->GetTextures(*m_pTextureInfo);

	// load branch textures
	char szPath[MAX_PATH];
	
	af_GetFilePath(szTreeFile, szPath, MAX_PATH);

	strncpy(m_szCompositeMap, szCompositeMap, 256);
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szCompositeMap, &m_pTextureComposite) )
	{
		m_pLogFile->Log("CELTree::Init(), failed to load tree's composite texture!");
		return false;
	}

	sprintf(m_szBranchMap, "%s\\%s", szPath, m_pTextureInfo->m_pBranchTextureFilename);
	m_szBranchMap[strlen(m_szBranchMap) - 3] = '\0';
	strcat(m_szBranchMap, "dds");
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szBranchMap, &m_pTextureBranch) )
	{
		m_pLogFile->Log("CELTree::Init(), failed to load tree's branch texture!");
		return false;
	}

	/*
	if( m_pTextureInfo->m_pSelfShadowFilename != NULL )
	{
		sprintf(szTextureFile, "%s\\%s", szPath, m_pTextureInfo->m_pSelfShadowFilename);
		szTextureFile[strlen(szTextureFile) - 3] = '\0';
		strcat(szTextureFile, "dds");
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szTextureFile, &m_pTextureBranchShadow) )
		{
			m_pLogFile->Log("CELTree::Init(), failed to load tree's branch selfshadow texture!");
			return false;
		}
	}*/
	
#ifdef TREE_BUMP_ENABLE
	if( m_pForest->GetBranchPixelShader() )
	{
		sprintf(m_szBranchNormalMap, "%s\\%s", szPath, m_pTextureInfo->m_pBranchTextureFilename);
		m_szBranchNormalMap[strlen(m_szBranchNormalMap) - 4] = '\0';
		strcat(m_szBranchNormalMap, "_normal.dds");
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szBranchNormalMap, &m_pTextureBranchNormal) )
		{
			m_pLogFile->Log("CELTree::Init(), failed to load tree's branch normal texture!");
			return false;
		}
	}
#endif
	
	// create the geometry cache object for obtaining data from SpeedTreeRT
	m_pGeometryCache = new CSpeedTreeRT::SGeometry;

	if( !CreateVertexBuffers() )
	{
		m_pLogFile->Log("CELTree::Init(), failed to create vertex buffers!");
		return false;
	}

	if( !InitCDObjects() )
	{
		m_pLogFile->Log("CELTree::Init(), failed to init cd objects!");
		return false;
	}

	return true;
}

bool CELTree::Release()
{
	ReleaseCDObjects();

	ReleaseAllTrees();
	ReleaseVertexBuffers();

	if( m_pGeometryCache )
	{
		delete m_pGeometryCache;
		m_pGeometryCache = NULL;
	}

	if( m_pTextureBranch )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureBranch);
		m_pTextureBranch = NULL;
	}

	if( m_pTextureComposite )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureComposite);
		m_pTextureComposite = NULL;
	}

	if( m_pTextureInfo )
	{
		delete m_pTextureInfo;
		m_pTextureInfo = NULL;
	}

	if( m_pSpeedTree )
	{
		delete m_pSpeedTree;
		m_pSpeedTree = NULL;
	}

	return true;
}

int CELTree::GetTreeIndexByID(DWORD dwID)
{
	int nNumTree = m_Trees.GetSize();

	for(int i=0; i<nNumTree; i++)
	{
		if( m_Trees[i].dwID == dwID )
			return i;
	}

	return -1;
}

bool CELTree::AddTree(const A3DVECTOR3& vecPos, DWORD& dwID)
{
	CSpeedTreeRT * pNewInstance = m_pSpeedTree->MakeInstance();

	if( !pNewInstance )
	{
		m_pLogFile->Log("CELTree::AddTree(), failed to create a new instance of tree!");
		return false;
	}

	dwID = GenerateID();
	ELTREE tree;
	memset(&tree, 0, sizeof(tree));
	tree.dwID = dwID;
	tree.pTreeInstance = pNewInstance;
	pNewInstance->SetTreePosition(vecPos.x, vecPos.z, vecPos.y);
	tree.vecPos = vecPos;

	A3DAABB aabb;
	aabb.Extents = A3DVECTOR3((m_afBoundingBox[3] - m_afBoundingBox[0]) * 0.5f, (m_afBoundingBox[5] - m_afBoundingBox[2]) * 0.5f, (m_afBoundingBox[4] - m_afBoundingBox[1]) * 0.5f);
	aabb.Center = tree.vecPos;
	aabb.Center.y += aabb.Extents.y;
	aabb.CompleteMinsMaxs();
	tree.aabb = aabb;

	m_Trees.Add(tree);
	return true;
}

bool CELTree::DeleteTree(DWORD dwID)
{
	int nTreeIndex = GetTreeIndexByID(dwID);
	if( nTreeIndex == -1 )
		return true;

	// now release this tree
	ELTREE& tree = m_Trees[nTreeIndex];
	if( tree.pTreeInstance )
	{
		delete tree.pTreeInstance;
		tree.pTreeInstance = NULL;
	}
	
	m_Trees.RemoveAtQuickly(nTreeIndex);
	return true;
}

// Get tree aabb
bool CELTree::GetTreeAABB(DWORD dwID, A3DAABB& aabb)
{
	aabb.Clear();
	
	int nTreeIndex = GetTreeIndexByID(dwID);
	if( nTreeIndex == -1 )
		return false;

	aabb = m_Trees[nTreeIndex].aabb;
	return true;
}


bool CELTree::AdjustTreePosition(DWORD dwID, const A3DVECTOR3& vecPos)
{
	int nTreeIndex = GetTreeIndexByID(dwID);
	if( nTreeIndex == -1 )
		return false;

	ELTREE& tree = m_Trees[nTreeIndex];
	tree.vecPos = vecPos;
	A3DAABB aabb;
	aabb.Extents = A3DVECTOR3((m_afBoundingBox[3] - m_afBoundingBox[0]) * 0.5f, (m_afBoundingBox[5] - m_afBoundingBox[2]) * 0.5f, (m_afBoundingBox[4] - m_afBoundingBox[1]) * 0.5f);
	aabb.Center = tree.vecPos;
	aabb.Center.y += aabb.Extents.y;
	aabb.CompleteMinsMaxs();
	tree.aabb = aabb;

	tree.pTreeInstance->SetTreePosition(vecPos.x, vecPos.z, vecPos.y);
	return true;
}

bool CELTree::ReleaseAllTrees()
{
	int nNumTree = m_Trees.GetSize();

	for(int i=0; i<nNumTree; i++)
	{
		ELTREE& tree = m_Trees[i];

		if( tree.pTreeInstance )
		{
			delete tree.pTreeInstance;
			tree.pTreeInstance = NULL;
		}
	}

	m_Trees.RemoveAll();
	return true;
}

bool CELTree::UpdateForRender(A3DViewport * pViewport)
{
	int nNumTree = m_Trees.GetSize();

	m_nCurLeavesCount = 0;
	m_nCurBillboardCount = 0;

	bool bRenderBill = !(m_pForest->GetLODLevel() == 0.4f);

	for(int i=0; i<nNumTree; i++)
	{
		ELTREE& tree = m_Trees[i];

		if( !pViewport->GetCamera()->AABBInViewFrustum(tree.aabb) )
			tree.bVisible = false;
		else
		{
			tree.bVisible = true;

			// compute lod level here;
			tree.pTreeInstance->ComputeLodLevel();
			tree.pTreeInstance->SetWindStrength(m_pForest->GetWindStrength(tree.vecPos));
			
			tree.pTreeInstance->GetGeometry(*m_pGeometryCache, SpeedTree_BillboardGeometry);

			if( !bRenderBill )
			{
				if( m_pGeometryCache->m_sBillboard0.m_bIsActive || m_pGeometryCache->m_sBillboard1.m_bIsActive )
				{
					tree.bVisible = false;
					continue;
				}
			}

			if (m_pGeometryCache->m_sBillboard0.m_bIsActive)
				m_nCurBillboardCount ++;

			// if tree has 360 degree billboards, render the second
			if (m_pGeometryCache->m_sBillboard1.m_bIsActive)
				m_nCurBillboardCount ++;

			/*
			// if tree has horizontal billboards, render this static billboard
			if (m_pGeometryCache->m_sHorizontalBillboard.m_bIsActive)
				m_nCurBillboardCount ++;
			*/

			// now update this tree type's leaves count and bill board count
			tree.pTreeInstance->GetGeometry(*m_pGeometryCache, SpeedTree_LeafGeometry);
			const CSpeedTreeRT::SGeometry::SLeaf* pLeaf = &m_pGeometryCache->m_sLeaves0;
			int nLod = pLeaf->m_nDiscreteLodLevel;
			if( nLod > -1 && tree.pTreeInstance->GetDiscreteLeafLodLevel() < tree.pTreeInstance->GetNumLeafLodLevels() 
				&& pLeaf->m_bIsActive && pLeaf->m_usLeafCount > 0 )
			{
				m_nCurLeavesCount += pLeaf->m_usLeafCount;
				tree.nLeafLOD = nLod;
			}
			else
			{
				tree.nLeafLOD = -1;
			}
		}
	}

	return true;
}

bool CELTree::RenderBranches(A3DViewport * pViewport)
{
	if( !m_pBranchStream )
		return true;

	m_pBranchStream->Appear(0);
	m_pTextureBranch->Appear(0);

#ifdef TREE_BUMP_ENABLE
	if( m_pForest->GetBranchPixelShader() && m_pForest->GetBranchVertexShader() && !(GetAsyncKeyState(VK_F7) & 0x8000) )
	{
		m_pTextureBranchNormal->Appear(1);
		m_pForest->GetBranchVertexShader()->Appear();
		// set vertex shader constants here
		A3DVECTOR4 c0(0.5f, 0.5f, 0.5f, 1.0f);
		m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);
	}
#endif

	A3DVECTOR3 vecCamPos = pViewport->GetCamera()->GetPos();

	int nNumTree = m_Trees.GetSize();
	for(int i=0; i<nNumTree; i++)
	{
		ELTREE& tree = m_Trees[i];

		if( tree.pTreeInstance && tree.bVisible )
		{
			CSpeedTreeRT * pSpeedTree = tree.pTreeInstance;
			pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_BranchGeometry);

			if (m_pGeometryCache->m_fBranchAlphaTestValue)
			{
				A3DMATRIX4 matTree = a3d_Translate(tree.vecPos.x, tree.vecPos.y, tree.vecPos.z);

				m_pA3DDevice->SetWorldMatrix(matTree);

#ifdef TREE_BUMP_ENABLE
				if( m_pForest->GetBranchPixelShader() && m_pForest->GetBranchVertexShader() && !(GetAsyncKeyState(VK_F7) & 0x8000) )
				{
					A3DVECTOR3 vecLightDir = Normalize(m_pForest->GetLightDir());
					A3DVECTOR4 c1;
					c1.x = vecLightDir.x;
					c1.y = vecLightDir.z;
					c1.z = vecLightDir.y;
					m_pA3DDevice->SetVertexShaderConstants(1, &c1, 1);
					A3DVECTOR3 vecEyePos = vecCamPos - tree.vecPos;
					A3DVECTOR4 c2;
					c2.x = vecEyePos.x;
					c2.y = vecEyePos.y;
					c2.z = vecEyePos.z;
					m_pA3DDevice->SetVertexShaderConstants(2, &c2, 1);
					A3DMATRIX4 mat = matTree * pViewport->GetCamera()->GetVPTM();
					mat.Transpose();
					m_pA3DDevice->SetVertexShaderConstants(3, &mat, 4);
				}
#endif
				// set alpha test value
				m_pA3DDevice->SetAlphaRef(DWORD(m_pGeometryCache->m_fBranchAlphaTestValue));
				
				int nLOD = m_pGeometryCache->m_sBranches.m_nDiscreteLodLevel;
				// render if this LOD has branches
				if( nLOD > -1 && m_pBranchIndexCount[nLOD] > 0)
					m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLESTRIP, 0, m_nBranchVertCount, 0, m_pBranchIndexCount[nLOD] - 2);

				m_pForest->AddTreeCount(m_pBranchIndexCount[nLOD] - 2);
			}
		}
	}

#ifdef TREE_BUMP_ENABLE
	if( m_pForest->GetBranchPixelShader() && m_pForest->GetBranchVertexShader() && !(GetAsyncKeyState(VK_F7) & 0x8000) )
	{
		m_pTextureBranchNormal->Disappear(1);
		m_pForest->GetBranchVertexShader()->Disappear();
	}
#endif

	m_pTextureBranch->Disappear(0);

	return true;
}

bool CELTree::RenderFronds(A3DViewport * pViewport)
{			  
	if( !m_pFrondStream )
		return true;

	m_pFrondStream->Appear(0);
	
	// fronds are 2-sided faces
	A3DCULLTYPE cull = m_pA3DDevice->GetFaceCull();
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

	int nNumTree = m_Trees.GetSize();
	for(int i=0; i<nNumTree; i++)
	{
		ELTREE& tree = m_Trees[i];

		if( tree.pTreeInstance && tree.bVisible )
		{
			CSpeedTreeRT * pSpeedTree = tree.pTreeInstance;

			pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_FrondGeometry);
			if (m_pGeometryCache->m_fFrondAlphaTestValue)
			{
				A3DMATRIX4 matTree = a3d_Translate(tree.vecPos.x, tree.vecPos.y, tree.vecPos.z);

				m_pA3DDevice->SetWorldMatrix(matTree);

				// set alpha test value
				m_pA3DDevice->SetAlphaRef(DWORD(m_pGeometryCache->m_fFrondAlphaTestValue));
				
				int nLOD = m_pGeometryCache->m_sFronds.m_nDiscreteLodLevel;
				// render if this LOD has branches
				if( nLOD > -1 && m_pFrondIndexCount[nLOD] > 0)
					m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLESTRIP, 0, m_nFrondVertCount, 0, m_pFrondIndexCount[nLOD] - 2);

				m_pForest->AddTreeCount(m_pFrondIndexCount[nLOD] - 2);
			}
		}
	}

	m_pA3DDevice->SetFaceCull(cull);
	return true;
}

bool CELTree::DrawLeavesAndBillboardsToBuffer(TREE_VERTEX1 * pVerts, int nMaxBlade, int& nBladeCount, int& nTree)
{
	int		n;

	nBladeCount = 0;
	int nNumBlades = nBladeCount;

	int nNumTree = m_Trees.GetSize();
	for(int id=nTree; id<nNumTree; id++)
	{
		ELTREE& tree = m_Trees[id];

		if( tree.pTreeInstance && tree.bVisible )
		{
			CSpeedTreeRT * pSpeedTree = tree.pTreeInstance;
			
			float x, y, z;
			x = tree.vecPos.x;
			y = tree.vecPos.y;
			z = tree.vecPos.z;

			A3DVECTOR3 billNormal = -m_pForest->GetLightDir();// * (-0.5f);

			// now check if we show the billboard here
			pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_BillboardGeometry);
			if (m_pGeometryCache->m_sBillboard0.m_bIsActive)
			{
				nNumBlades ++;
				if( nNumBlades > nMaxBlade )
					return true;			

				const float* pCoords = m_pGeometryCache->m_sBillboard0.m_pCoords;
				const float* pTexCoords = m_pGeometryCache->m_sBillboard0.m_pTexCoords;
				
				pVerts->pos.x = pCoords[0] + x;
				pVerts->pos.y = pCoords[2] + y;
				pVerts->pos.z = pCoords[1] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[0];
				pVerts->v = pTexCoords[1];
				pVerts ++;

				pVerts->pos.x = pCoords[3] + x;
				pVerts->pos.y = pCoords[5] + y;
				pVerts->pos.z = pCoords[4] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[2];
				pVerts->v = pTexCoords[3];
				pVerts ++;

				pVerts->pos.x = pCoords[6] + x;
				pVerts->pos.y = pCoords[8] + y;
				pVerts->pos.z = pCoords[7] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[4];
				pVerts->v = pTexCoords[5];
				pVerts ++;

				pVerts->pos.x = pCoords[9] + x;
				pVerts->pos.y = pCoords[11] + y;
				pVerts->pos.z = pCoords[10] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[6];
				pVerts->v = pTexCoords[7];
				pVerts ++;
			}

			// if tree supports 360 degree billboards, render the second
			if (m_pGeometryCache->m_sBillboard1.m_bIsActive)
			{
				nNumBlades ++;
				if( nNumBlades > nMaxBlade )
					return true;			
				
				const float* pCoords = m_pGeometryCache->m_sBillboard1.m_pCoords;
				const float* pTexCoords = m_pGeometryCache->m_sBillboard1.m_pTexCoords;
				
				pVerts->pos.x = pCoords[0] + x;
				pVerts->pos.y = pCoords[2] + y;
				pVerts->pos.z = pCoords[1] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[0];
				pVerts->v = pTexCoords[1];
				pVerts ++;

				pVerts->pos.x = pCoords[3] + x;
				pVerts->pos.y = pCoords[5] + y;
				pVerts->pos.z = pCoords[4] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[2];
				pVerts->v = pTexCoords[3];
				pVerts ++;

				pVerts->pos.x = pCoords[6] + x;
				pVerts->pos.y = pCoords[8] + y;
				pVerts->pos.z = pCoords[7] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[4];
				pVerts->v = pTexCoords[5];
				pVerts ++;

				pVerts->pos.x = pCoords[9] + x;
				pVerts->pos.y = pCoords[11] + y;
				pVerts->pos.z = pCoords[10] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[6];
				pVerts->v = pTexCoords[7];
				pVerts ++;
			}

			// if tree has horizontal billboards, render this static billboard
			/*
			if (m_pGeometryCache->m_sHorizontalBillboard.m_bIsActive)
			{
				nNumBlades ++;
				if( nNumBlades > nMaxBlade )
					return true;			
				
				const float* pCoords = m_pGeometryCache->m_sHorizontalBillboard.m_pCoords;
				const float* pTexCoords = m_pGeometryCache->m_sHorizontalBillboard.m_pTexCoords;
				
				pVerts->pos.x = pCoords[0] + x;
				pVerts->pos.y = pCoords[2] + y;
				pVerts->pos.z = pCoords[1] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[0];
				pVerts->v = pTexCoords[1];
				pVerts ++;

				pVerts->pos.x = pCoords[3] + x;
				pVerts->pos.y = pCoords[5] + y;
				pVerts->pos.z = pCoords[4] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[2];
				pVerts->v = pTexCoords[3];
				pVerts ++;

				pVerts->pos.x = pCoords[6] + x;
				pVerts->pos.y = pCoords[8] + y;
				pVerts->pos.z = pCoords[7] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[4];
				pVerts->v = pTexCoords[5];
				pVerts ++;

				pVerts->pos.x = pCoords[9] + x;
				pVerts->pos.y = pCoords[11] + y;
				pVerts->pos.z = pCoords[10] + z;
				pVerts->normal = billNormal;
				pVerts->u = pTexCoords[6];
				pVerts->v = pTexCoords[7];
				pVerts ++;
			}*/

			if( tree.nLeafLOD > -1 )
			{
				// now we only draw m_sLeaves0
				pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_LeafGeometry);
				const CSpeedTreeRT::SGeometry::SLeaf* pLeaf = &m_pGeometryCache->m_sLeaves0;

				for(n=0; n<pLeaf->m_usLeafCount; n++)
				{
					nNumBlades ++;
					if( nNumBlades > nMaxBlade )
						return true;

					A3DVECTOR3 vecCenter;
					vecCenter.x = pLeaf->m_pCenterCoords[n * 3] + x;
					vecCenter.y = pLeaf->m_pCenterCoords[n * 3 + 2] + y;
					vecCenter.z = pLeaf->m_pCenterCoords[n * 3 + 1] + z;

					A3DVECTOR3 vecNormal;
					vecNormal.x = pLeaf->m_pNormals[n * 3];
					vecNormal.y = pLeaf->m_pNormals[n * 3 + 2];
					vecNormal.z = pLeaf->m_pNormals[n * 3 + 1];

					A3DCOLOR diffuse = AGBR2ARGB(pLeaf->m_pColors[n]);
					pVerts->pos.x = vecCenter.x + pLeaf->m_pLeafMapCoords[n][0];
					pVerts->pos.y = vecCenter.y + pLeaf->m_pLeafMapCoords[n][2];
					pVerts->pos.z = vecCenter.z + pLeaf->m_pLeafMapCoords[n][1];
					pVerts->normal = vecNormal;
					pVerts->u = pLeaf->m_pLeafMapTexCoords[n][0];
					pVerts->v = pLeaf->m_pLeafMapTexCoords[n][1];
					pVerts ++;
					
					pVerts->pos.x = vecCenter.x + pLeaf->m_pLeafMapCoords[n][4];
					pVerts->pos.y = vecCenter.y + pLeaf->m_pLeafMapCoords[n][6];
					pVerts->pos.z = vecCenter.z + pLeaf->m_pLeafMapCoords[n][5];
					pVerts->normal = vecNormal;
					pVerts->u = pLeaf->m_pLeafMapTexCoords[n][2];
					pVerts->v = pLeaf->m_pLeafMapTexCoords[n][3];
					pVerts ++;
					
					pVerts->pos.x = vecCenter.x + pLeaf->m_pLeafMapCoords[n][8];
					pVerts->pos.y = vecCenter.y + pLeaf->m_pLeafMapCoords[n][10];
					pVerts->pos.z = vecCenter.z + pLeaf->m_pLeafMapCoords[n][9];
					pVerts->normal = vecNormal;
					pVerts->u = pLeaf->m_pLeafMapTexCoords[n][4];
					pVerts->v = pLeaf->m_pLeafMapTexCoords[n][5];
					pVerts ++;
					
					pVerts->pos.x = vecCenter.x + pLeaf->m_pLeafMapCoords[n][12];
					pVerts->pos.y = vecCenter.y + pLeaf->m_pLeafMapCoords[n][14];
					pVerts->pos.z = vecCenter.z + pLeaf->m_pLeafMapCoords[n][13];
					pVerts->normal = vecNormal;
					pVerts->u = pLeaf->m_pLeafMapTexCoords[n][6];
					pVerts->v = pLeaf->m_pLeafMapTexCoords[n][7];
					pVerts ++;
				}
			}
		}

		nBladeCount = nNumBlades;
		nTree ++;
	}

	m_pForest->AddTreeCount(nBladeCount * 2);
	return true;
}

bool CELTree::GetFrondsData()
{
	return true;
}

bool CELTree::GetLeafData()
{
	return true;
}

bool CELTree::GetBillboardData()
{
	return true;
}

bool CELTree::CreateVertexBuffers()
{
	// try to release old one
	ReleaseVertexBuffers();

	// read all the geometry for highest LOD into the geometry cache (just a precaution, it's updated later)
	m_pSpeedTree->SetLodLevel(1.0f);
	m_pSpeedTree->GetGeometry(*m_pGeometryCache);

	// now create branch vertex buffer first;
	if( !CreateBranchVertexBuffer() )
		return false;

	// then create frond vertex buffer
	if( !CreateFrondVertexBuffer() )
		return false;

	return true;
}

bool CELTree::ReleaseVertexBuffers()
{
	ReleaseFrondVertexBuffer();
	ReleaseBranchVertexBuffer();

	return true;
}

bool CELTree::CreateBranchVertexBuffer()
{
	m_pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_BranchGeometry, 0);

	// reference to branch structure
	CSpeedTreeRT::SGeometry::SIndexed* pBranches = &(m_pGeometryCache->m_sBranches);
	m_nBranchVertCount = (int)pBranches->m_usVertexCount; // vertex are shared among all LODs

	// check if this tree has branches
	if( m_nBranchVertCount > 1 )
	{
		int i;
		m_pBranchVerts = new TREE_BRANCHVERTEX[m_nBranchVertCount];

		// fill the vertex buffer by interleaving SpeedTree data
		for(i=0; i<m_nBranchVertCount; i++)
		{
			m_pBranchVerts[i].pos.x = pBranches->m_pCoords[i * 3];
			m_pBranchVerts[i].pos.y = pBranches->m_pCoords[i * 3 + 2];
			m_pBranchVerts[i].pos.z = pBranches->m_pCoords[i * 3 + 1];

			m_pBranchVerts[i].normal.x = pBranches->m_pNormals[i * 3];
			m_pBranchVerts[i].normal.y = pBranches->m_pNormals[i * 3 + 2];
			m_pBranchVerts[i].normal.z = pBranches->m_pNormals[i * 3 + 1];

			m_pBranchVerts[i].u = pBranches->m_pTexCoords0[i * 2];
			m_pBranchVerts[i].v = pBranches->m_pTexCoords0[i * 2 + 1];

			// self shadow texture coords
			/*
			if( pBranches->m_pTexCoords1 )
			{
				m_pBranchVerts[i].su = pBranches->m_pTexCoords1[i * 2];
				m_pBranchVerts[i].sv = pBranches->m_pTexCoords1[i * 2 + 1];
			}*/

#ifdef TREE_BUMP_ENABLE
			if( pBranches->m_pTangents )
			{
				m_pBranchVerts[i].tangent[0] = pBranches->m_pTangents[i * 3 + 0];
				m_pBranchVerts[i].tangent[1] = pBranches->m_pTangents[i * 3 + 1];
				m_pBranchVerts[i].tangent[2] = pBranches->m_pTangents[i * 3 + 2];
			}

			if( pBranches->m_pBinormals )
			{
				m_pBranchVerts[i].binormal[0] = pBranches->m_pBinormals[i * 3 + 0];
				m_pBranchVerts[i].binormal[1] = pBranches->m_pBinormals[i * 3 + 1];
				m_pBranchVerts[i].binormal[2] = pBranches->m_pBinormals[i * 3 + 2];
			}
#endif
		}

		// create and fill the index counts for each LOD
		int nNumLod = (int)m_pSpeedTree->GetNumBranchLodLevels();
		m_pBranchIndexCount = new int[nNumLod];
		for(i=0; i<nNumLod; i++)
		{
			// force update for particular LOD
			m_pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_BranchGeometry, (short)i);

			// check if has stiched the seperate strips together
			if( pBranches->m_usNumStrips > 1 )
			{
				m_pLogFile->Log("CELTree::CreateBranchVertexBuffer(), SpeedTree strips must be stitched together by Compute()!");
				return false;
			}

			// check if this LOD has branches
			if( pBranches->m_usNumStrips == 1 )
				m_pBranchIndexCount[i] = pBranches->m_pStripLengths[0];
			else
				m_pBranchIndexCount[i] = 0;
		}

		// set back to highest LOD before we get the longest index list
		m_pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_BranchGeometry, 0);

		// the first LOD level contains the most indices of all the levels, so
		// we use its size to allocate the index buffer
		m_pBranchIndices = new WORD[m_pBranchIndexCount[0]];
		// fill the index buffer
		memcpy(m_pBranchIndices, pBranches->m_pStrips[0], sizeof(WORD) * pBranches->m_pStripLengths[0]);

		// now create render stream for branch
		m_pBranchStream = new A3DStream();
		if( !m_pBranchStream->Init(m_pA3DDevice, sizeof(TREE_BRANCHVERTEX), TREE_FVF_BRANCHVERTEX, m_nBranchVertCount, m_pBranchIndexCount[0], A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR) )
		{
			m_pLogFile->Log("CELTree::CreateBranchVertexBuffer(), failed to create branch stream!");
			return false;
		}

		m_pBranchStream->SetVerts((BYTE *) m_pBranchVerts, m_nBranchVertCount);
		m_pBranchStream->SetVertexRef((BYTE *) m_pBranchVerts);
		m_pBranchStream->SetIndices((BYTE*) m_pBranchIndices, m_pBranchIndexCount[0]);
		m_pBranchStream->SetIndexRef((void *) m_pBranchIndices);
	}

	return true;
}

bool CELTree::ReleaseBranchVertexBuffer()
{
	if( m_pBranchStream )
	{
		m_pBranchStream->Release();
		delete m_pBranchStream;
		m_pBranchStream = NULL;
	}

	if( m_pBranchIndices )
	{
		delete [] m_pBranchIndices;
		m_pBranchIndices = NULL;
	}

	if( m_pBranchVerts )
	{
		delete [] m_pBranchVerts;
		m_pBranchVerts = NULL;
	}

	if( m_pBranchIndexCount )
	{
		delete [] m_pBranchIndexCount;
		m_pBranchIndexCount = NULL;
	}

	return true;
}

bool CELTree::CreateFrondVertexBuffer()
{
	m_pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_FrondGeometry, 0);

	// reference to frond structure
	CSpeedTreeRT::SGeometry::SIndexed* pFronds = &(m_pGeometryCache->m_sFronds);
	m_nFrondVertCount = (int)pFronds->m_usVertexCount; // vertex are shared among all LODs

	// check if this tree has fronds
	if( m_nFrondVertCount > 1 )
	{
		m_pFrondVerts = new TREE_VERTEX1[m_nFrondVertCount];

		// fill the vertex buffer by interleaving SpeedTree data
		int i(0);
		for(i=0; i<m_nFrondVertCount; i++)
		{
			m_pFrondVerts[i].pos.x = pFronds->m_pCoords[i * 3];
			m_pFrondVerts[i].pos.y = pFronds->m_pCoords[i * 3 + 2];
			m_pFrondVerts[i].pos.z = pFronds->m_pCoords[i * 3 + 1];
			m_pFrondVerts[i].normal.x = pFronds->m_pNormals[i * 3];
			m_pFrondVerts[i].normal.y = pFronds->m_pNormals[i * 3 + 2];
			m_pFrondVerts[i].normal.z = pFronds->m_pNormals[i * 3 + 1];
			m_pFrondVerts[i].u = pFronds->m_pTexCoords0[i * 2];
			m_pFrondVerts[i].v = pFronds->m_pTexCoords0[i * 2 + 1];

			// self shadow texture coords
			/*
			if( pFronds->m_pTexCoords1 )
			{
				m_pFrondVerts[i].su = pFronds->m_pTexCoords1[i * 2];
				m_pFrondVerts[i].sv = pFronds->m_pTexCoords1[i * 2 + 1];
			}*/
		}

		// create and fill the index counts for each LOD
		int nNumLod = (int)m_pSpeedTree->GetNumFrondLodLevels();
		m_pFrondIndexCount = new int[nNumLod];
		for(i=0; i<nNumLod; i++)
		{
			// force update for particular LOD
			m_pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_FrondGeometry, -1, (short)i);

			// check if has stiched the seperate strips together
			if( pFronds->m_usNumStrips > 1 )
			{
				m_pLogFile->Log("CELTree::CreateFrondVertexBuffer(), SpeedTree strips must be stitched together by Compute()!");
				return false;
			}

			// check if this LOD has fronds
			if( pFronds->m_usNumStrips == 1 )
				m_pFrondIndexCount[i] = pFronds->m_pStripLengths[0];
			else
				m_pFrondIndexCount[i] = 0;
		}

		// set back to highest LOD before we get the longest index list
		m_pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_FrondGeometry, -1, 0);

		// the first LOD level contains the most indices of all the levels, so
		// we use its size to allocate the index buffer
		m_pFrondIndices = new WORD[m_pFrondIndexCount[0]];
		// fill the index buffer
		memcpy(m_pFrondIndices, pFronds->m_pStrips[0], sizeof(WORD) * pFronds->m_pStripLengths[0]);

		// now create render stream for frond
		m_pFrondStream = new A3DStream();
		if( !m_pFrondStream->Init(m_pA3DDevice, sizeof(TREE_VERTEX1), TREE_FVF_VERTEX1, m_nFrondVertCount, m_pFrondIndexCount[0], A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR) )
		{
			m_pLogFile->Log("CELTree::CreateFrondVertexBuffer(), failed to create frond stream!");
			return false;
		}

		m_pFrondStream->SetVerts((BYTE *) m_pFrondVerts, m_nFrondVertCount);
		m_pFrondStream->SetVertexRef((BYTE *) m_pFrondVerts);
		m_pFrondStream->SetIndices((BYTE*) m_pFrondIndices, m_pFrondIndexCount[0]);
		m_pFrondStream->SetIndexRef((void *) m_pFrondIndices);
	}

	return true;
}

bool CELTree::ReleaseFrondVertexBuffer()
{
	if( m_pFrondStream )
	{
		m_pFrondStream->Release();
		delete m_pFrondStream;
		m_pFrondStream = NULL;
	}

	if( m_pFrondIndices )
	{
		delete [] m_pFrondIndices;
		m_pFrondIndices = NULL;
	}

	if( m_pFrondVerts )
	{
		delete [] m_pFrondVerts;
		m_pFrondVerts = NULL;
	}

	if( m_pFrondIndexCount )
	{
		delete [] m_pFrondIndexCount;
		m_pFrondIndexCount = NULL;
	}

	return true;
}

bool CELTree::LoadTrees(AFile * pFileToLoad)
{	
	DWORD dwReadLen;

	DWORD dwVersion = 0;

	if( !pFileToLoad->Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;
	
	if( dwVersion < 0x10000000 )
	{
		// the earliest version does not contain version information
		dwVersion = 0;
		pFileToLoad->Seek(-4, AFILE_SEEK_CUR);
	}

	if( dwVersion == 0x10001000 )
	{
		// read in tree count
		int nNumTree;
		if( !pFileToLoad->Read(&nNumTree, sizeof(int), &dwReadLen) )
			goto READFAIL;

		int	i;
		// now read each tree's information in
		for(i=0; i<nNumTree; i++)
		{
			DWORD dwTreeID;
			A3DVECTOR3 vecPos;

			if( !pFileToLoad->Read(&dwTreeID, sizeof(DWORD), &dwReadLen) )
				goto READFAIL;

			if( !pFileToLoad->Read(&vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
				goto READFAIL;

			CSpeedTreeRT * pNewInstance = m_pSpeedTree->MakeInstance();

			if( !pNewInstance )
			{
				m_pLogFile->Log("CELTree::LoadTrees(), failed to create a new instance of tree!");
				return false;
			}

			ELTREE tree;
			memset(&tree, 0, sizeof(tree));
			tree.dwID = dwTreeID;
			tree.pTreeInstance = pNewInstance;
			pNewInstance->SetTreePosition(vecPos.x, vecPos.z, vecPos.y);
			tree.vecPos = vecPos; 
			A3DAABB aabb;
			aabb.Extents = A3DVECTOR3((m_afBoundingBox[3] - m_afBoundingBox[0]) * 0.5f, (m_afBoundingBox[5] - m_afBoundingBox[2]) * 0.5f, (m_afBoundingBox[4] - m_afBoundingBox[1]) * 0.5f);
			aabb.Center = tree.vecPos;
			aabb.Center.y += aabb.Extents.y;
			aabb.CompleteMinsMaxs();
			tree.aabb = aabb;

			m_Trees.Add(tree);
		}

		// last read in the next generation id
		if( !pFileToLoad->Read(&m_dwIDNext, sizeof(DWORD), &dwReadLen) )
			goto READFAIL;

	}
	else if( dwVersion == 0 )
	{
		// then read in tree count
		int nNumTree;
		if( !pFileToLoad->Read(&nNumTree, sizeof(int), &dwReadLen) )
			goto READFAIL;

		int	i;
		// now read each tree's information in
		for(i=0; i<nNumTree; i++)
		{
			DWORD dwTreeID;
			A3DVECTOR3 vecPos;

			if( !pFileToLoad->Read(&dwTreeID, sizeof(DWORD), &dwReadLen) )
				goto READFAIL;

			if( !pFileToLoad->Read(&vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
				goto READFAIL;

			CSpeedTreeRT * pNewInstance = m_pSpeedTree->MakeInstance();

			if( !pNewInstance )
			{
				m_pLogFile->Log("CELTree::LoadTrees(), failed to create a new instance of tree!");
				return false;
			}

			ELTREE tree;
			memset(&tree, 0, sizeof(tree));
			tree.dwID = dwTreeID;
			tree.pTreeInstance = pNewInstance;
			pNewInstance->SetTreePosition(vecPos.x, vecPos.z, vecPos.y);
			tree.vecPos = vecPos;
			A3DAABB aabb;
			aabb.Extents = A3DVECTOR3((m_afBoundingBox[3] - m_afBoundingBox[0]) * 0.5f, (m_afBoundingBox[5] - m_afBoundingBox[2]) * 0.5f, (m_afBoundingBox[4] - m_afBoundingBox[1]) * 0.5f);
			aabb.Center = tree.vecPos;
			aabb.Center.y += aabb.Extents.y;
			aabb.CompleteMinsMaxs();
			tree.aabb = aabb;

			m_Trees.Add(tree);

			m_dwIDNext = max(m_dwIDNext, dwTreeID + 1);
		}
	}
	
	return true;

READFAIL:
	m_pLogFile->Log("CELTree::Load(), Read from the file failed!");
	return false;
}

bool CELTree::SaveTrees(AFile * pFileToSave)
{
	DWORD dwWriteLen;
	
	// first of all output the current version of the file
	DWORD dwVersion = TREE_VERSION;
	if( !pFileToSave->Write(&dwVersion, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;

	// then write tree count
	int nNumTree;
	nNumTree = m_Trees.GetSize();
	if( !pFileToSave->Write(&nNumTree, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;

	int	i;
	// now write each tree's information out
	for(i=0; i<nNumTree; i++)
	{
		ELTREE tree = m_Trees[i];
		if( !pFileToSave->Write(&tree.dwID, sizeof(DWORD), &dwWriteLen) )
			goto WRITEFAIL;

		if( !pFileToSave->Write(&tree.vecPos, sizeof(A3DVECTOR3), &dwWriteLen) )
			goto WRITEFAIL;
	}

	// last output the next generation id
	if( !pFileToSave->Write(&m_dwIDNext, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;

	return true;

WRITEFAIL:
	m_pLogFile->Log("CELTree::Save(), Write to the file failed!");
	return false;
}

bool CELTree::ReComputeAllTrees()
{
	int	i;
	int nNumTrees = m_Trees.GetSize();

	// first release old instanced trees
	for(i=0; i<nNumTrees; i++)
	{
		ELTREE& tree = m_Trees[i];
		if( tree.pTreeInstance )
		{
			delete tree.pTreeInstance;
			tree.pTreeInstance = NULL;
		}
	}

	if( m_pGeometryCache )
	{
		delete m_pGeometryCache;
		m_pGeometryCache = NULL;
	}

	if( m_pTextureInfo )
	{
		delete m_pTextureInfo;
		m_pTextureInfo = NULL;
	}

	if( m_pSpeedTree )
	{
		delete m_pSpeedTree;
		m_pSpeedTree = NULL;
	}

	m_bBadTree = true;

	// now reload a origin tree file and create all instanced from it
	m_pSpeedTree = new CSpeedTreeRT();
	
	// direct x should call texture flip for speedtree
	m_pSpeedTree->SetTextureFlip(true);

	// load the tree file
	AFileImage fileTree;
	if( !fileTree.Open(m_szSPTFile, AFILE_OPENEXIST) )
	{
		m_pLogFile->Log("CELTree::ReComputeAllTrees(), failed to load speed tree file [%s]!", m_szSPTFile);
		// not found original tree, so try to use the bad tree instead
		if( !fileTree.Open("trees\\badtree.spt", AFILE_OPENEXIST) )
		{
			m_pLogFile->Log("CELTree::ReComputeAllTrees(), failed to load bad tree, so tree type will not be initialized!");
			return false;
		}
	}

	if( !m_pSpeedTree->LoadTree(fileTree.GetFileBuffer(), fileTree.GetFileLength()) )
	{
		m_pLogFile->Log("CELTree::ReComputeAllTrees(), failed to load speed tree file [%s]!", fileTree.GetFileName());
	}

	m_bBadTree = false;

	// now set some setup data for speed tree
	m_pSpeedTree->SetBranchLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
	m_pSpeedTree->SetLeafLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
	m_pSpeedTree->SetFrondLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
	
	m_pSpeedTree->SetBranchWindMethod(CSpeedTreeRT::WIND_NONE);
	m_pSpeedTree->SetLeafWindMethod(CSpeedTreeRT::WIND_NONE);
	m_pSpeedTree->SetFrondWindMethod(CSpeedTreeRT::WIND_NONE);
	
	m_pSpeedTree->SetNumLeafRockingGroups(1);

	float vTreeSize, vVariance;
	m_pSpeedTree->GetTreeSize(vTreeSize, vVariance);
	m_pSpeedTree->SetTreeSize(vTreeSize * TREEUNIT_SCALE);

    // generate tree geometry
    if( !m_pSpeedTree->Compute(NULL) )
	{
		m_pLogFile->Log("CELTree::ReComputeAllTrees(), failed to call speedtree's Compute method!");
		return false;
	}
    // get the dimensions
	m_pSpeedTree->GetBoundingBox(m_afBoundingBox);
	// adjust lod distances
    float fHeight = m_afBoundingBox[5] - m_afBoundingBox[2];
	if( fHeight < 5.0f )
		fHeight = 5.0f;
	fHeight *= m_pForest->GetLODLevel();
    m_pSpeedTree->SetLodLimits(fHeight * 5.0f, fHeight * 16.0f);
	
	// make the leaves rock in the wind
	m_pSpeedTree->SetLeafRockingState(true);

    // query textures
	m_pTextureInfo = new CSpeedTreeRT::STextures;
	m_pSpeedTree->GetTextures(*m_pTextureInfo);

	for(i=0; i<nNumTrees; i++)
	{
		ELTREE& tree = m_Trees[i];
		tree.pTreeInstance = m_pSpeedTree->MakeInstance();
		if( !tree.pTreeInstance )
		{
			m_pLogFile->Log("CELTree::ReComputeAllTrees(), failed to create a new instance of tree!");
			return false;
		}

		tree.pTreeInstance->SetTreePosition(tree.vecPos.x, tree.vecPos.z, tree.vecPos.y);
	}

	// now refill cached geometry
	m_pGeometryCache = new CSpeedTreeRT::SGeometry;
	m_pSpeedTree->GetGeometry(*m_pGeometryCache);

	if( !CreateVertexBuffers() )
	{
		m_pLogFile->Log("CELTree::ReComputeAllTrees(), call create vertex buffers failed!");
		return false;
	}
	return true;
}

bool CELTree::InitCDObjects()
{
	ReleaseCDObjects();

	// we get the speed tree's convex hull from a corresponding file
	AFileImage fileTBH;

	char szTBHFile[MAX_PATH];
	strncpy(szTBHFile, m_szSPTFile, MAX_PATH);
	strlwr(szTBHFile);
	char * pExt = strstr(szTBHFile, ".spt");
	if( !pExt )
	{
		m_pLogFile->Log("CELTree::InitCDObjects(), bad speed tree file [%s]!", m_szSPTFile);
		return false;
	}
	*pExt = '\0';
	strcat(szTBHFile, ".tbh");

	if( !fileTBH.Open(szTBHFile, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		m_pLogFile->Log("CELTree::InitCDObjects(), can not find tch file [%s]!", szTBHFile);
		return true;
	}

	DWORD dwReadLen;
	DWORD version;
	if( !fileTBH.Read(&version, sizeof(DWORD), &dwReadLen) )
	{
		m_pLogFile->Log("CELTree::InitCDObjects(), read version failed!");
		return false;
	}

	if( version != 0x00000009 )
	{
		m_pLogFile->Log("CELTree::InitCDObjects(), wrong version, current version is 9!");
		return false;
	}

	if( !fileTBH.Read(&m_nNumCDObject, sizeof(int), &dwReadLen) )
	{
		m_pLogFile->Log("CELTree::InitCDObjects(), read number cd object failed!");
		return false;
	}
	m_nNumCDBrushes = m_nNumCDObject;

	// now load convex hull and brush data from file.
	int i;
	m_ppConvexHulls = new CConvexHullData*[m_nNumCDObject];
	m_ppCDBrushes = new CCDBrush *[m_nNumCDBrushes];
	for(i=0; i<m_nNumCDObject; i++)
	{
		CConvexHullData * pCD = new CConvexHullData();
		if( !pCD->LoadBinaryData(&fileTBH) )
		{
			m_pLogFile->Log("CELTree::InitCDObjects(), load convex hull data failed!");
			return false;
		}
		m_ppConvexHulls[i] = pCD;

		CCDBrush * pCDBrush = new CCDBrush;
		if( !pCDBrush->Load(&fileTBH) )
		{
			m_pLogFile->Log("CELTree::InitCDObjects(), load cd brush failed!");
			return false;
		}
		m_ppCDBrushes[i] = pCDBrush;
	}

	fileTBH.Close();
	return true;
}

bool CELTree::ReleaseCDObjects()
{
	if( m_ppCDBrushes )
	{
		for(int i=0; i<m_nNumCDBrushes; i++)
		{
			CCDBrush * pCDBrush = m_ppCDBrushes[i];
			if( pCDBrush )
			{
				delete pCDBrush;
				pCDBrush = NULL;
			}
		}

		delete [] m_ppCDBrushes;
		m_ppCDBrushes = NULL;
	}

	if( m_ppConvexHulls )
	{
		for(int i=0; i<m_nNumCDObject; i++)
		{
			CConvexHullData * pConvexHullData = m_ppConvexHulls[i];
			if( pConvexHullData )
			{
				delete pConvexHullData;
				pConvexHullData = NULL;
			}
		}

		delete [] m_ppConvexHulls;
		m_ppConvexHulls = NULL;
	}

	return true;
}

// below function is in the A3DMesh.cpp
extern bool RayIntersectTriangle(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3 * pTriVerts, float * pvFraction, A3DVECTOR3 * pVecPos);

// Ray trace with branch meshes
bool CELTree::RayTraceBranchMesh(const A3DVECTOR3& vecLocalStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal)
{
	float			vFractionThis;
	A3DVECTOR3		vecHitPosThis;
	float			vMinFraction = 99999.9f;
	A3DVECTOR3		v0, v1, v2;

	int nIndicesCount = m_pBranchIndexCount[0];
	
	int				n0, n1, n2;
	A3DVECTOR3		v[3];

	// strip start from first 2 verts.
	n0 = m_pBranchIndices[0];
	n1 = m_pBranchIndices[1];

	for(int i=2; i<nIndicesCount; i++)
	{
		n2 = m_pBranchIndices[i];

		if( i & 0x1 )
		{
			// odd triangle, flip, but no flip because we flip it in render
			v[0] = m_pBranchVerts[n0].pos;
			v[1] = m_pBranchVerts[n1].pos;
			v[2] = m_pBranchVerts[n2].pos;
		}
		else
		{
			// even triangle, no flip, but flip because we flip it in render
			v[0] = m_pBranchVerts[n0].pos;
			v[1] = m_pBranchVerts[n2].pos;
			v[2] = m_pBranchVerts[n1].pos;
		}

		if( RayIntersectTriangle(vecLocalStart, vecDelta, v, &vFractionThis, &vecHitPosThis) )
		{
			if( vFractionThis < vMinFraction )
			{
				*pvFraction = vMinFraction = vFractionThis;
				vecHitPos = vecHitPosThis;
				v0 = v[0]; v1 = v[1]; v2 = v[2];
			}
		}

		n0 = n1;
		n1 = n2;
	}

	if( vMinFraction < 1.0f )
	{
		// Should calculate normal here;
		vecNormal = Normalize(CrossProduct(v1 - v0, v2 - v1));
		return true;
	}

	return false;
}

bool CELTree::TraceWithBrush(BrushTraceInfo * pInfo)
{
	//save original result
	bool		bStartSolid = pInfo->bStartSolid;	//	Collide something at start point
	bool		bAllSolid = pInfo->bAllSolid;		//	All in something
	int			iClipPlane = pInfo->iClipPlane;		//	Clip plane's index
	float		fFraction = 100.0f;		//	Fraction
	A3DVECTOR3  vNormal = pInfo->ClipPlane.GetNormal(); //clip plane normal
	float       fDist = pInfo->ClipPlane.GetDist();	//clip plane dist
	DWORD		dwTreeID = 0xffffffff;

	A3DVECTOR3	vStart = pInfo->vStart;
	bool bCollide = false;
	
	for(int n=0; n<m_Trees.GetSize(); n++)
	{
		const ELTREE& tree = m_Trees[n];
		
		A3DVECTOR3 vPoint, vRayNormal;
		float    fRayFrac;
		if (pInfo->bIsPoint && !CLS_RayToAABB3(vStart, pInfo->vDelta, tree.aabb.Mins, tree.aabb.Maxs, vPoint,  &fRayFrac, vRayNormal )) {
			continue;
		}
		if (!pInfo->bIsPoint && !CLS_AABBAABBOverlap(pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents, tree.aabb.Center, tree.aabb.Extents)) {
			continue;
		}
		//@note: during the CCDBrush tracing, boundaabb is not used. 
		//       so it is safe to set the start directly.   by kuiwu.
		pInfo->vStart = vStart - tree.vecPos;
		for(int i=0;i<m_nNumCDBrushes;i++)
		{
			if (m_ppCDBrushes[i] && m_ppCDBrushes[i]->Trace(pInfo) && (pInfo->fFraction < fFraction)) 
			{
					//update the saving info
					bStartSolid = pInfo->bStartSolid;
					bAllSolid = pInfo->bAllSolid;
					iClipPlane = pInfo->iClipPlane;
					fFraction = pInfo->fFraction;
					vNormal = pInfo->ClipPlane.GetNormal();
					fDist = pInfo->ClipPlane.GetDist();
					dwTreeID = tree.dwID;
					bCollide = true;
			}
		}	
	}

	pInfo->bStartSolid = bStartSolid;
	pInfo->bAllSolid = bAllSolid;
	pInfo->iClipPlane = iClipPlane;
	pInfo->fFraction = fFraction;
	pInfo->ClipPlane.SetNormal(vNormal);
	pInfo->ClipPlane.SetD(fDist);
	pInfo->vStart = vStart;
	pInfo->dwUser2 = dwTreeID;

	return bCollide;
}

// sphere collide with tree, for editor only
bool CELTree::SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal)
{
	// Do the pretest!
	// Construct the sweeping sphere
	A3DVECTOR3 vSSCenter = vecStart + (vecDelta / 2.0f);
	float fSSRadius = fRadius + vecDelta.Magnitude() / 2.0f;

	// Detect the collision with Convex Hull
	const CFace *pCDFace=NULL;
	float fraction, fMinFraction=999999.0f;
	A3DVECTOR3 vecHitPosThis;
	
	for(int n=0; n<m_Trees.GetSize(); n++)
	{
		const ELTREE& tree = m_Trees[n];

		if( !CLS_AABBSphereOverlap(tree.aabb, vSSCenter, fSSRadius) )
			continue;

		A3DVECTOR3 vecLocalStart;
		vecLocalStart = vecStart - tree.vecPos;

		for(int i=0;i<m_nNumCDObject;i++)
		{
			CConvexHullData* pCHData = m_ppConvexHulls[i];
			if( !pCHData )
				continue;

			if( SphereCollideWithCH(vecLocalStart, vecDelta, fRadius, *pCHData, &pCDFace, vecHitPosThis, fraction) )
			{
				if( fraction < fMinFraction )
				{
					fMinFraction	= fraction;
					vecHitPos		= vecHitPosThis + tree.vecPos;
					vecNormal		= pCDFace->GetNormal();
				}
			}
		}
	}

	if( fMinFraction < 1.0f )
	{
		*pvFraction=fMinFraction;
		return true;
	}

	return false;

}

bool CELTree::DrawLeavesToBufferForShadowMap(TREE_VERTEX1 * pVerts, int nMaxBlade, int& nBladeCount, int& nTree)
{
	int		n;
	nBladeCount = 0;

	int nNumTree = m_Trees.GetSize();
	for(int id=nTree; id<nNumTree; id++)
	{
		ELTREE& tree = m_Trees[id];

	
			CSpeedTreeRT * pSpeedTree = tree.pTreeInstance;
			
			float x, y, z;
			x = tree.vecPos.x;
			y = tree.vecPos.y;
			z = tree.vecPos.z;


			// now we only draw m_sLeaves0
			pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_LeafGeometry);
			const CSpeedTreeRT::SGeometry::SLeaf* pLeaf = &m_pGeometryCache->m_sLeaves0;

			for(n=0; n<pLeaf->m_usLeafCount; n++)
			{
				nBladeCount ++;
				if( nBladeCount >= nMaxBlade )
					return true;

				A3DVECTOR3 vecCenter;
				vecCenter.x = pLeaf->m_pCenterCoords[n * 3] + x;
				vecCenter.y = pLeaf->m_pCenterCoords[n * 3 + 2] + y;
				vecCenter.z = pLeaf->m_pCenterCoords[n * 3 + 1] + z;

				A3DVECTOR3 vecNormal;
				vecNormal.x = pLeaf->m_pNormals[n * 3];
				vecNormal.y = pLeaf->m_pNormals[n * 3 + 2];
				vecNormal.z = pLeaf->m_pNormals[n * 3 + 1];

				A3DCOLOR diffuse = AGBR2ARGB(pLeaf->m_pColors[n]);
				pVerts->pos.x = vecCenter.x + pLeaf->m_pLeafMapCoords[n][0];
				pVerts->pos.y = vecCenter.y + pLeaf->m_pLeafMapCoords[n][2];
				pVerts->pos.z = vecCenter.z + pLeaf->m_pLeafMapCoords[n][1];
				pVerts->normal = vecNormal;
				pVerts->u = pLeaf->m_pLeafMapTexCoords[n][0];
				pVerts->v = pLeaf->m_pLeafMapTexCoords[n][1];
				pVerts ++;
				
				pVerts->pos.x = vecCenter.x + pLeaf->m_pLeafMapCoords[n][4];
				pVerts->pos.y = vecCenter.y + pLeaf->m_pLeafMapCoords[n][6];
				pVerts->pos.z = vecCenter.z + pLeaf->m_pLeafMapCoords[n][5];
				pVerts->normal = vecNormal;
				pVerts->u = pLeaf->m_pLeafMapTexCoords[n][2];
				pVerts->v = pLeaf->m_pLeafMapTexCoords[n][3];
				pVerts ++;
				
				pVerts->pos.x = vecCenter.x + pLeaf->m_pLeafMapCoords[n][8];
				pVerts->pos.y = vecCenter.y + pLeaf->m_pLeafMapCoords[n][10];
				pVerts->pos.z = vecCenter.z + pLeaf->m_pLeafMapCoords[n][9];
				pVerts->normal = vecNormal;
				pVerts->u = pLeaf->m_pLeafMapTexCoords[n][4];
				pVerts->v = pLeaf->m_pLeafMapTexCoords[n][5];
				pVerts ++;
				
				pVerts->pos.x = vecCenter.x + pLeaf->m_pLeafMapCoords[n][12];
				pVerts->pos.y = vecCenter.y + pLeaf->m_pLeafMapCoords[n][14];
				pVerts->pos.z = vecCenter.z + pLeaf->m_pLeafMapCoords[n][13];
				pVerts->normal = vecNormal;
				pVerts->u = pLeaf->m_pLeafMapTexCoords[n][6];
				pVerts->v = pLeaf->m_pLeafMapTexCoords[n][7];
				pVerts ++;
			}
		nTree ++;
	}
	
	return true;
}

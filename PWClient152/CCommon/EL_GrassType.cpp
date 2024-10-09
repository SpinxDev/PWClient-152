/*
 * FILE: EL_GrassType.cpp
 *
 * DESCRIPTION: Grass Type class for element
 *
 * CREATED BY: Hedi, 2004/10/23
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
#include <A3DPixelShader.h>
#include <A3DVertexShader.h>
#include <AF.h>
#include <A3DCameraBase.h>
#include <A3DFrame.h>
#include <A3DMesh.h>
#include <A3DTerrain2.h>

#include "EL_GrassLand.h"
#include "EL_GrassType.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Class CELGrassType
//	
///////////////////////////////////////////////////////////////////////////

CELGrassType::CELGrassType()
{
	m_pA3DDevice			= NULL;
	m_pLogFile				= NULL;
	m_pGrassLand			= NULL;

	m_pTextureGrass			= NULL;
	m_pVertsTemplate		= NULL;
	m_pIndicesTemplate		= NULL;

	m_dwIDNext				= 0;
	m_bBadGrass				= true;
}

CELGrassType::~CELGrassType()
{
}

bool CELGrassType::Init(A3DDevice * pA3DDevice, CELGrassLand * pGrassLand, DWORD dwTypeID, int nMaxVisibleCount, const char * szGrassFile, ALog * pLogFile, CELGrassType::GRASSDEFINE_DATA * pDefineData)
{
	m_pA3DDevice		= pA3DDevice;
	m_pGrassLand		= pGrassLand;
	m_dwTypeID			= dwTypeID;
	m_pLogFile			= pLogFile;
	strncpy(m_szGrassFile, szGrassFile, 256);

	A3DFrame * pGrassFrame = new A3DFrame();

	AFileImage moxFile;
	if( !moxFile.Open(szGrassFile, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		m_pLogFile->Log("CELGrassType::Init(), can not found grass file [%s]!", szGrassFile);

		// we use bad grass instead
		if( !moxFile.Open("grasses\\badgrass.mox", AFILE_OPENEXIST | AFILE_BINARY) )
		{
			m_pLogFile->Log("CELGrassType::Init(), can not found bad grass file, so grass type will not be loaded");
			return false;
		}
	}

	// load from a mox file
	if( !pGrassFrame->Load(m_pA3DDevice, &moxFile) )
		return false;
	moxFile.Close();

	if( pGrassFrame->GetNumChilds() != 0 || pGrassFrame->GetNumMeshes() != 1 )
		return false;

	m_bBadGrass = false;

	A3DMesh * pGrassMesh = pGrassFrame->GetFirstMesh();
	
	// use texture directly from this mesh
	m_pTextureGrass = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->DuplicateTexture(pGrassMesh->GetTexture());
	strncpy(m_szGrassMap, m_pTextureGrass->GetMapFile(), MAX_PATH);

	// vertex and face count
	m_nNumOneGrassVert = pGrassMesh->GetVertCount();
	m_nNumOneGrassFace = pGrassMesh->GetIndexCount() / 3;
	m_pVertsTemplate = new GRASSVERTEX[m_nNumOneGrassVert];
	m_pIndicesTemplate = new WORD[m_nNumOneGrassFace * 3];

	// retrieve vertex data
	int i;

	A3DVERTEX * pMeshVerts = pGrassMesh->GetVerts(0);
	A3DAABB aabb = pGrassMesh->GetMeshAutoAABB(0);
	float	vHeight = aabb.Maxs.y - aabb.Mins.y;
	for(i=0; i<m_nNumOneGrassVert; i++)
	{
		GRASSVERTEX vert;
		vert.pos.x = pMeshVerts[i].x;
		vert.pos.y = pMeshVerts[i].y;
		vert.pos.z = pMeshVerts[i].z;

		vert.normal.x = vert.normal.z = 0.0f;
		vert.normal.y = vert.pos.y / vHeight;
		vert.normal.y *= vert.normal.y;

		vert.u = pMeshVerts[i].tu;
		vert.v = pMeshVerts[i].tv;

		m_pVertsTemplate[i] = vert;
	}

	// retrieve index data
	memcpy(m_pIndicesTemplate, pGrassMesh->GetIndices(), sizeof(WORD) * m_nNumOneGrassFace * 3);

	// no longer use frame, so delete it now.
	pGrassFrame->Release();
	delete pGrassFrame;
	
	// set a default define data
	if( pDefineData )
	{
		m_GrassDefineData = *pDefineData;
		m_GrassDefineData.bAlphaBlendEnable = false;
		m_GrassDefineData.nAlphaRefValue = 84;
	}
	else
	{	// set to defaults value
		m_GrassDefineData.bAlphaBlendEnable = false;
		m_GrassDefineData.nAlphaRefValue = 84;
		m_GrassDefineData.vSize = 1.0f;
		m_GrassDefineData.vSizeVar = 0.2f;
		m_GrassDefineData.vSoftness = 1.0f;
		m_GrassDefineData.vSightRange = 50.0f;
	}
	return true;
}

bool CELGrassType::Release()
{
	// first release all grass bits maps
	int nNumMaps = (int)m_GrassBitsMaps.size();
	for(int i=0; i<nNumMaps; i++)
	{
		CELGrassBitsMap * pGrassBitsMap = m_GrassBitsMaps[i];
		pGrassBitsMap->Release();
		delete pGrassBitsMap;
	}
	m_GrassBitsMaps.clear();

	if( m_pTextureGrass )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureGrass);
		m_pTextureGrass = NULL;
	}

	if( m_pIndicesTemplate )
	{
		delete [] m_pIndicesTemplate;
		m_pIndicesTemplate = NULL;
	}

	if( m_pVertsTemplate )
	{
		delete [] m_pVertsTemplate;
		m_pVertsTemplate = NULL;
	}

	return true;
}

bool CELGrassType::AdjustDefineData(A3DViewport * pViewport, const GRASSDEFINE_DATA& data)
{
	m_GrassDefineData = data;

	return RebuildAllGrasses(pViewport);
}

bool CELGrassType::AdjustForLODLevel()
{
	// now update the blocks that are rendering
	int nNumMaps = m_GrassBitsMaps.size();

	for(int i=0; i<nNumMaps; i++)
	{
		CELGrassBitsMap * pGrassBitsMap = m_GrassBitsMaps[i];
		if( pGrassBitsMap )
		{
			pGrassBitsMap->AdjustForLODLevel();
		}
	}
	return true;
}

bool CELGrassType::RebuildAllGrasses(A3DViewport * pViewport)
{
	// now update the blocks that are rendering
	int nNumMaps = m_GrassBitsMaps.size();

	for(int i=0; i<nNumMaps; i++)
	{
		CELGrassBitsMap * pGrassBitsMap = m_GrassBitsMaps[i];
		if( pGrassBitsMap )
		{
			pGrassBitsMap->UpdateGrassesForRender(pViewport);
		}
	}

	return true;
}

bool CELGrassType::Load(AFile * pFileToLoad)
{
	DWORD dwReadLen;

	// first read the next bits map id
	if( !pFileToLoad->Read(&m_dwIDNext, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;

	// then, save all bits map
	int nNumBitsMap; 
	if( !pFileToLoad->Read(&nNumBitsMap, sizeof(int), &dwReadLen) )
		goto READFAIL;

	int	i;
	for(i=0; i<nNumBitsMap; i++)
	{
		// first write out the bits map's id
		DWORD dwID;
		if( !pFileToLoad->Read(&dwID, sizeof(DWORD), &dwReadLen) )
			goto READFAIL;

		// width and height of the bits map
		int nWidth, nHeight;
		if( !pFileToLoad->Read(&nWidth, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nHeight, sizeof(int), &dwReadLen) )
			goto READFAIL;

		// center and grid size
		A3DVECTOR3 vecCenter;
		if( !pFileToLoad->Read(&vecCenter, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		float vGridSize;
		if( !pFileToLoad->Read(&vGridSize, sizeof(float), &dwReadLen) )
			goto READFAIL;

		// now create the CELGrassBitsMap
		CELGrassBitsMap * pBitsMap = new CELGrassBitsMap();
		if( !pBitsMap->Init(m_pA3DDevice, this, dwID, vecCenter, vGridSize, nWidth, nHeight) )
			return false;

		// now read the bits from file
		if( !pFileToLoad->Read(pBitsMap->GetBitsMap(), sizeof(BYTE) * nWidth / 8 * nHeight, &dwReadLen) )
			goto READFAIL;

		m_GrassBitsMaps.push_back(pBitsMap);
	}

	return true;

READFAIL:
	//m_pLogFile->Log("CELGrassType::Load(), Read from the file failed!");
	return false;
}

bool CELGrassType::Save(AFile * pFileToSave)
{
	DWORD dwWriteLen;

	// first write the next bits map id
	if( !pFileToSave->Write(&m_dwIDNext, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;

	// then, save all bits map
	int nNumBitsMap;
	nNumBitsMap = m_GrassBitsMaps.size();
	if( !pFileToSave->Write(&nNumBitsMap, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;

	int	i;
	for(i=0; i<nNumBitsMap; i++)
	{
		CELGrassBitsMap * pBitsMap = m_GrassBitsMaps[i];

		// first write out the bits map's id
		DWORD dwID = pBitsMap->GetID();
		if( !pFileToSave->Write(&dwID, sizeof(DWORD), &dwWriteLen) )
			goto WRITEFAIL;

		// width and height of the bits map
		int nWidth, nHeight;
		nWidth = pBitsMap->GetWidth();
		nHeight = pBitsMap->GetHeight();
		if( !pFileToSave->Write(&nWidth, sizeof(int), &dwWriteLen) )
			goto WRITEFAIL;
		if( !pFileToSave->Write(&nHeight, sizeof(int), &dwWriteLen) )
			goto WRITEFAIL;

		// center and grid size
		A3DVECTOR3 vecCenter = pBitsMap->GetCenter();
		float vGridSize = pBitsMap->GetGridSize();
		if( !pFileToSave->Write(&vecCenter, sizeof(A3DVECTOR3), &dwWriteLen) )
			goto WRITEFAIL;
		if( !pFileToSave->Write(&vGridSize, sizeof(float), &dwWriteLen) )
			goto WRITEFAIL;

		// now write the bits out
		if( !pFileToSave->Write(pBitsMap->GetBitsMap(), sizeof(BYTE) * nWidth / 8 * nHeight, &dwWriteLen) )
			goto WRITEFAIL;
	}

	return true;

WRITEFAIL:
	//m_pLogFile->Log("CELGrassType::Save(), Write to the file failed!");
	return false;
}

bool CELGrassType::Update(const A3DVECTOR3& vecCenter)
{
	int nNumMaps = m_GrassBitsMaps.size();
	for(int i=0; i<nNumMaps; i++)
	{
		CELGrassBitsMap * pGrassBitsMap = m_GrassBitsMaps[i];
		if( pGrassBitsMap )
		{
			pGrassBitsMap->Update(vecCenter);
		}
	}
	return true;
}
	
bool CELGrassType::RenderGrasses(A3DViewport * pViewport)
{
	int nNumMaps = m_GrassBitsMaps.size();
	if( !nNumMaps )
		return true;

	A3DCULLTYPE cullType = m_pA3DDevice->GetFaceCull();
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

	if( m_GrassDefineData.bAlphaBlendEnable )
	{
		m_pA3DDevice->SetAlphaBlendEnable(true);
		m_pA3DDevice->SetAlphaTestEnable(false);
		m_pA3DDevice->SetZWriteEnable(false);
	}
	else
	{
		m_pA3DDevice->SetAlphaTestEnable(true);
		m_pA3DDevice->SetAlphaBlendEnable(false);
		m_pA3DDevice->SetAlphaFunction(A3DCMP_GREATER);
		m_pA3DDevice->SetAlphaRef(m_GrassDefineData.nAlphaRefValue);
	}
	
	m_pTextureGrass->Appear(0);
	for(int i=0; i<nNumMaps; i++)
	{
		CELGrassBitsMap * pGrassBitsMap = m_GrassBitsMaps[i];
		if( pGrassBitsMap )
		{
			pGrassBitsMap->Render(pViewport);
		}
	}
	m_pA3DDevice->SetFaceCull(cullType);
	m_pTextureGrass->Disappear(0);
	
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetAlphaTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(true);

	return true;
}

bool CELGrassType::AddGrassBitsMap(BYTE * pBitsMap, const A3DVECTOR3& vecCenter, float vGridSize, int nWidth, int nHeight, DWORD& dwID)
{
	dwID = GenerateID();

	CELGrassBitsMap * pGrassBitsMap = new CELGrassBitsMap();

	if( !pGrassBitsMap->Init(m_pA3DDevice, this, dwID, vecCenter, vGridSize, nWidth, nHeight) )
	{
		m_pLogFile->Log("CELGrassType::AddGrassBitsMap(), failed to initialize new grass bits map!");
		return false;
	}

	pGrassBitsMap->UpdateBitsMap(pBitsMap);

	m_GrassBitsMaps.push_back(pGrassBitsMap);
	return true;
}

CELGrassBitsMap * CELGrassType::GetGrassBitsMap(DWORD dwID)
{
	int nNumMaps = m_GrassBitsMaps.size();

	for(int i=0; i<nNumMaps; i++)
	{
		CELGrassBitsMap * pGrassBitsMap = m_GrassBitsMaps[i];
		if( pGrassBitsMap->GetID() == dwID )
			return pGrassBitsMap;
	}

	return NULL;
}

bool CELGrassType::DeleteGrassBitsMap(DWORD dwID)
{
	int nNumMaps = m_GrassBitsMaps.size();

	for(int i=0; i<nNumMaps; i++)
	{
		CELGrassBitsMap * pGrassBitsMap = m_GrassBitsMaps[i];
		if( pGrassBitsMap->GetID() == dwID )
		{
			pGrassBitsMap->Release();
			delete pGrassBitsMap;

			m_GrassBitsMaps.erase(m_GrassBitsMaps.begin() + i);
			return true;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Class CELGrassBitMap
//		a map used to determine where the grasses grow
//	
///////////////////////////////////////////////////////////////////////////

CELGrassBitsMap::CELGrassBitsMap()
{
	m_pGrassType			= NULL;

	m_pBitsMap				= NULL;
	m_nWidth = m_nHeight	= 0;

	m_nNumBlockX			= 0;
	m_nNumBlockY			= 0;
	m_nNumBlocks			= 0;
	m_ppBlocks				= NULL;

	m_pTerrainLM			= NULL;
	m_pTerrainLM_n			= NULL;

	m_dwExportID			= 0;
	m_bAllBitsZero			= true;
}

CELGrassBitsMap::~CELGrassBitsMap()
{
}

bool CELGrassBitsMap::Init(A3DDevice * pA3DDevice, CELGrassType * pGrassType, DWORD dwID, const A3DVECTOR3& vecCenter, float vGridSize, int nWidth, int nHeight)
{
	m_pA3DDevice		= pA3DDevice;
	m_pGrassType		= pGrassType;
	m_dwID				= dwID;
	m_vGridSize			= vGridSize;
	m_nWidth			= nWidth;
	m_nHeight			= nHeight;
	m_vecCenter			= vecCenter;

	m_vecOrigin			= vecCenter - A3DVECTOR3(m_vGridSize * m_nWidth / 2.0f, 0.0f, 0.0f) - A3DVECTOR3(0.0f, 0.0f, -m_vGridSize * m_nHeight / 2.0f);

	assert(((nWidth >> 3) << 3) == nWidth);

	// now create a bit map to hold the bits data
	int nSizeMap = (nWidth >> 3) * nHeight;
	m_pBitsMap = new BYTE[nSizeMap];
	if( !m_pBitsMap ) return false;
	memset(m_pBitsMap, 0, nSizeMap);

	// create all block objects
	if( !CreateBlocks() )
		return false;

	return true;
}

bool CELGrassBitsMap::Release()
{
	ReleaseBlocks();

	if( m_pBitsMap )
	{
		delete [] m_pBitsMap;
		m_pBitsMap = NULL;
	}

	return true;
}

bool CELGrassBitsMap::UpdateBitsMap(BYTE * pBitsMap)
{
	memcpy(m_pBitsMap, pBitsMap, (m_nWidth * m_nHeight) >> 3);

	return true;
}

bool CELGrassBitsMap::UpdateGrassesForRender(A3DViewport * pViewport)
{
	// update the block's grasses information, only update blocks that have been built
	// for those that have not been built yet, the other call to Update will build them
	for(int i=0; i<m_nNumBlocks; i++)
	{
		CELGrassBlock * pBlock = m_ppBlocks[i];
		if( pBlock && pBlock->IsBuilt() )
		{
			pBlock->BuildGrasses();
		}
	}

	return true;
}

bool CELGrassBitsMap::AdjustForLODLevel()
{
	// update the block's grasses information, only update blocks that have been built
	for(int i=0; i<m_nNumBlocks; i++)
	{
		CELGrassBlock * pBlock = m_ppBlocks[i];
		if( pBlock )
		{
			if( pBlock->IsBuilt() )
				pBlock->BuildGrasses();
		}
	}

	return true;
}

bool CELGrassBitsMap::UpdateVisibleBlocks(const A3DVECTOR3& vecCenter)
{
	// first we should determine the sight range
	int cx, cy;
	int nSightRangeInGrid = int(m_pGrassType->GetDefineData().vSightRange / m_vGridSize);

	cx = int((vecCenter.x - m_vecOrigin.x) / m_vGridSize);
	cy = int((m_vecOrigin.z - vecCenter.z) / m_vGridSize);

	ARectI rectSight;
	rectSight.left = cx - nSightRangeInGrid;
	rectSight.right = rectSight.left + nSightRangeInGrid * 2;
	rectSight.top = cy - nSightRangeInGrid;
	rectSight.bottom = rectSight.top + nSightRangeInGrid * 2;

	int		nBlockX = cx / GRASS_BLOCKSIZE;
	int		nBlockY = cy / GRASS_BLOCKSIZE;

	int		i, j;
	
	// first let all visible blocks disappear if it can not be activated again by below test
	for(i=0; i<(int)m_VisibleBlocks.size(); i++)
		m_VisibleBlocks[i]->SetAboutToDestroy();

	int	r = nSightRangeInGrid / GRASS_BLOCKSIZE;
	for(i=nBlockY-r; i<nBlockY+r; i++)
	{
		for(j=nBlockX-r; j<nBlockX+r; j++)
		{
			if( i < 0 || i >= m_nNumBlockY || j < 0 || j >= m_nNumBlockX )
				continue;

			CELGrassBlock * pBlock = m_ppBlocks[i * m_nNumBlockX + j];
			if( pBlock )
			{
				ARectI rectBlock = pBlock->GetBlockRect();
				ARectI rectCombined = rectBlock & rectSight;

				if( !rectCombined.IsEmpty() )
				{
					if( !pBlock->IsBuilt() && !pBlock->IsAboutToBuild() )
					{
						m_VisibleBlocks.push_back(pBlock);
						pBlock->SetAboutToBuild();
					}
					pBlock->ClearAboutToDestroy();
				}
			}
		}
	}

	// now destroy some old ones;
	for(i=0; i<(int)m_VisibleBlocks.size(); )
	{
		CELGrassBlock * pBlock = m_VisibleBlocks[i];
		if( pBlock->IsAboutToDestroy() )
		{
			pBlock->DestroyGrasses();

			// remove from visible blocks list
			m_VisibleBlocks.erase(m_VisibleBlocks.begin() + i);
			continue;
		}

		i ++;
	}
	return true;
}

bool CELGrassBitsMap::Update(const A3DVECTOR3& vecCenter)
{
	// first see if we need to update it.
	float vRadius = MagnitudeH(m_vecCenter - m_vecOrigin);
	if( MagnitudeH(vecCenter - m_vecCenter) > vRadius + m_pGrassType->GetDefineData().vSightRange)
		return true;
	
	if( !UpdateVisibleBlocks(vecCenter) )
		return false;

	int i;
	for(i=0; i<(int)m_VisibleBlocks.size(); i++)
	{
		CELGrassBlock * pBlock = m_VisibleBlocks[i];
		if( !pBlock->IsBuilt() && pBlock->IsAboutToBuild() )
		{
			pBlock->BuildGrasses();
			if( m_pGrassType->GetGrassLand()->GetProgressLoad() )
			{
				if( a_GetTime() > m_pGrassType->GetGrassLand()->GetUpdateStartTime() + 6 )
					break;
			}
		}
	}
	return true;
}

bool CELGrassBitsMap::Render(A3DViewport * pViewport)
{
	// first see if we need to render it.
	float vRadius = MagnitudeH(m_vecCenter - m_vecOrigin);
	if( MagnitudeH(pViewport->GetCamera()->GetPos() - m_vecCenter) > vRadius + m_pGrassType->GetDefineData().vSightRange )
		return true;
	
	A3DStream * pStream = NULL;

	A3DVertexShader * pVertexShader = m_pGrassType->GetGrassLand()->GetCurGrassVertexShader();

	if( pVertexShader )
		pStream = m_pGrassType->GetGrassLand()->GetStream();
	else
		pStream = m_pGrassType->GetGrassLand()->GetSoftStream();
	if( !pStream )
		return true;
	
	pStream->Appear();

	m_pGrassType->GetGrassLand()->GetTerrainLMAt(m_vecCenter, &m_rectLM, &m_pTerrainLM, &m_pTerrainLM_n);
	if( m_pTerrainLM && m_pTerrainLM_n )
	{
		if( m_pGrassType->GetGrassLand()->GetGrassRenderShader() )
		{
			m_pGrassType->GetGrassLand()->GetGrassRenderShader()->Appear();
			m_pTerrainLM->Appear(1);
			m_pTerrainLM_n->Appear(2);
		}
		else
		{
			m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
			m_pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG1);
			m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_MODULATE2X);
			
			m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
			m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
			m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
			m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_SELECTARG2);

			if( m_pGrassType->GetGrassLand()->GetDNFactor() < 0.5f )
				m_pTerrainLM->Appear(1);
			else
				m_pTerrainLM_n->Appear(1);
		}
	}
	else
	{
		m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG1);
		m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);

		m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
		m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
	}

	float vSightRange = m_pGrassType->GetDefineData().vSightRange;
	if( pVertexShader )
	{
		A3DMATRIX4 matLM = IdentityMatrix();
		matLM._22 = 0.0f;
		matLM._23 = 1.0f;
		matLM._33 = 0.0f;
		matLM._32 = -1.0f;
		matLM._41 = -m_rectLM.left;
		matLM._42 = m_rectLM.top;
		matLM = matLM * Scaling(1.0f / (m_rectLM.right - m_rectLM.left), 1.0f / (m_rectLM.top - m_rectLM.bottom), 1.0f);
		matLM.Transpose();
		m_pA3DDevice->SetVertexShaderConstants(5, &matLM, 4);
		pVertexShader->Appear();
		A3DVECTOR4 c0(1.0f, 0.0f, 0.0f, 1.0f/(vSightRange*vSightRange));
		m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);
	}
	else
	{
		A3DMATRIX4 matLM = IdentityMatrix();
		matLM._22 = 0.0f;
		matLM._23 = 0.0f;
		matLM._33 = 0.0f;
		matLM._32 = -1.0f;
		matLM._41 = -m_rectLM.left;
		matLM._42 = m_rectLM.top;
		matLM = InverseTM(pViewport->GetCamera()->GetViewTM()) * matLM * Scaling(1.0f / (m_rectLM.right - m_rectLM.left), 1.0f / (m_rectLM.top - m_rectLM.bottom), 1.0f);
		m_pA3DDevice->SetTextureMatrix(1, matLM);
		m_pA3DDevice->SetTextureCoordIndex(1, D3DTSS_TCI_CAMERASPACEPOSITION);
		m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_COUNT3);
		if( m_pGrassType->GetGrassLand()->GetGrassRenderShader() )
		{
			// need two texture layer
			m_pA3DDevice->SetTextureCoordIndex(2, 1);
		}
	}

	BYTE * pVerts;
	WORD * pIndices;

	if( !pStream->LockVertexBuffer(0, 0, (BYTE **) &pVerts, 0) )
		return false;
	if( !pStream->LockIndexBuffer(0, 0, (BYTE **) &pIndices, 0) )
		return false;

	int nVertCount = 0;
	int nFaceCount = 0;

	int i, s, t;
	for(i=0; i<(int)m_VisibleBlocks.size(); i++)
	{
		CELGrassBlock * pBlock = m_VisibleBlocks[i];
		
		if( pBlock && pBlock->IsBuilt() && pBlock->GetNumGrasses() && pBlock->GetGrassesVerts() )
		{
			if( !pViewport->GetCamera()->AABBInViewFrustum(pBlock->GetAABB()) )
				continue;

			float vDis = MagnitudeH(pViewport->GetCamera()->GetPos() - pBlock->GetCenter());
			if( vDis > GRASS_BLOCKSIZE * GetGridSize() * 1.414f + m_pGrassType->GetDefineData().vSightRange )
				continue;

			int nNumGrasses = pBlock->GetNumGrasses();
			
			int nNumVerts = nNumGrasses * m_pGrassType->GetNumOneGrassVert();
			int nNumFaces = nNumGrasses * m_pGrassType->GetNumOneGrassFace();

			if( nVertCount + nNumVerts > m_pGrassType->GetGrassLand()->GetMaxVerts() ||
				nFaceCount + nNumFaces > m_pGrassType->GetGrassLand()->GetMaxFaces() )
			{
				if( !pStream->UnlockVertexBuffer() )
					return false;
				if( !pStream->UnlockIndexBuffer() )
					return false;

				// flush it once here
				m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nVertCount, 0, nFaceCount);
				m_pGrassType->GetGrassLand()->AddGrassCount(nFaceCount);

				// lock it again;
				if( !pStream->LockVertexBuffer(0, 0, (BYTE **) &pVerts, 0) )
					return false;
				if( !pStream->LockIndexBuffer(0, 0, (BYTE **) &pIndices, 0) )
					return false;

				nVertCount = 0;
				nFaceCount = 0;
			}
			
			if( pVertexShader )
			{
				memcpy(pVerts, pBlock->GetGrassesVerts(), nNumVerts * sizeof(GRASSVERTEX));
				pVerts += nNumVerts * sizeof(GRASSVERTEX);
			}
			else
			{
				A3DVECTOR3 vecCamPos = pViewport->GetCamera()->GetPos();
				A3DLVERTEX * pLVerts = (A3DLVERTEX *) pVerts;
				GRASSVERTEX * pGrassVerts = pBlock->GetGrassesVerts();
				float rs = 1.0f / (vSightRange * vSightRange);
				// software processing.
				for(int idVert=0; idVert<nNumVerts; idVert++)
				{
					A3DVECTOR4 offset = m_pGrassType->GetGrassLand()->GetWindOffset()[int(pGrassVerts[idVert].normal.x)];

					float soft = pGrassVerts[idVert].normal.y;
					pLVerts[idVert].x = pGrassVerts[idVert].pos.x + offset.x * soft;
					pLVerts[idVert].y = pGrassVerts[idVert].pos.y + offset.y * soft;
					pLVerts[idVert].z = pGrassVerts[idVert].pos.z + offset.z * soft;

					A3DVECTOR3 vecDelta = pGrassVerts[idVert].pos - vecCamPos;
					float a = 1.0f - (vecDelta.x * vecDelta.x + vecDelta.z * vecDelta.z) * rs;
					a_Clamp(a, 0.0f, 1.0f);

					pLVerts[idVert].diffuse = A3DCOLORRGBA(255, 255, 255, int(a * 255.0f));
					pLVerts[idVert].specular = 0xff000000;
					pLVerts[idVert].tu = pGrassVerts[idVert].u;
					pLVerts[idVert].tv = pGrassVerts[idVert].v;
				}
				pVerts += nNumVerts * sizeof(A3DLVERTEX);
			}

			for(s=0; s<pBlock->GetNumGrasses(); s++)
			{
				WORD * pOneIndices = m_pGrassType->GetIndicesTemplate();
				for(t=0; t<m_pGrassType->GetNumOneGrassFace() * 3; t++)
				{
					*pIndices = nVertCount + (*pOneIndices);
					
					pIndices ++;
					pOneIndices ++;
				}

				nVertCount += m_pGrassType->GetNumOneGrassVert();
			}
			
			nFaceCount += nNumFaces;
		}
	}

	if( !pStream->UnlockVertexBuffer() )
		return false;
	if( !pStream->UnlockIndexBuffer() )
		return false;

	if( nFaceCount > 0 )
	{
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nVertCount, 0, nFaceCount);
		m_pGrassType->GetGrassLand()->AddGrassCount(nFaceCount);
	}

	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);

	if( pVertexShader )
		pVertexShader->Disappear();
	else
	{
		m_pA3DDevice->SetTextureCoordIndex(1, 1);
		m_pA3DDevice->SetTextureCoordIndex(2, 2);
		m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	}

	if( m_pTerrainLM )
		m_pTerrainLM->Disappear(1);
	if( m_pTerrainLM_n )
		m_pTerrainLM_n->Disappear(2);
	
	return true;
}

bool CELGrassBitsMap::ShiftCenterTo(const A3DVECTOR3& vecCenter)
{
	m_vecCenter = vecCenter;
	m_vecOrigin	= vecCenter - A3DVECTOR3(m_vGridSize * m_nWidth / 2.0f, 0.0f, 0.0f) - A3DVECTOR3(0.0f, 0.0f, -m_vGridSize * m_nHeight / 2.0f);

	return CreateBlocks();
}

bool CELGrassBitsMap::CreateBlocks()
{
	// try to release old ones.
	ReleaseBlocks();

	// now create all blocks, but not build them
	assert(m_nWidth / GRASS_BLOCKSIZE * GRASS_BLOCKSIZE == m_nWidth);
	assert(m_nHeight / GRASS_BLOCKSIZE * GRASS_BLOCKSIZE == m_nHeight);

	m_nNumBlockX = m_nWidth / GRASS_BLOCKSIZE;
	m_nNumBlockY = m_nHeight / GRASS_BLOCKSIZE;

	m_nNumBlocks = m_nNumBlockX * m_nNumBlockY;
	m_ppBlocks = new CELGrassBlock* [m_nNumBlocks];
	for(int y=0; y<m_nNumBlockY; y++)
	{
		for(int x=0; x<m_nNumBlockX; x++)
		{
			ARectI rectBlock;
			rectBlock.left = x * GRASS_BLOCKSIZE;
			rectBlock.right = rectBlock.left + GRASS_BLOCKSIZE;
			rectBlock.top = y * GRASS_BLOCKSIZE;
			rectBlock.bottom = rectBlock.top + GRASS_BLOCKSIZE;

			CELGrassBlock * pNewBlock = new CELGrassBlock();

			if( !pNewBlock->Init(this, rectBlock) )
				return false;

			m_ppBlocks[y * m_nNumBlockX + x] = pNewBlock;
		}
	}

	return true;
}

bool CELGrassBitsMap::ReleaseBlocks()
{
	if( m_ppBlocks )
	{
		for(int i=0; i<m_nNumBlocks; i++)
		{
			CELGrassBlock * pBlock = m_ppBlocks[i];

			if( pBlock )
			{
				pBlock->Release();
				delete pBlock;
			}
		}

		delete [] m_ppBlocks;
		m_ppBlocks = NULL;
	}

	m_nNumBlockX = 0;
	m_nNumBlockY = 0;
	m_nNumBlocks = 0;

	m_VisibleBlocks.clear();
	return true;
}

//	Used when export grass data. Return true if not all bits are zero,
//	this function also set m_bAllBitsZero flag
bool CELGrassBitsMap::BitsMapCheck()
{
	m_bAllBitsZero = true;

	if (!m_pBitsMap)
		return false;

	int i, iNumByte = m_nWidth * m_nHeight / 8;
	for (i=0; i < iNumByte; i++)
	{
		if (m_pBitsMap[i])
		{
			m_bAllBitsZero = false;
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////
//	
//	class CELGrassBlock
//	
///////////////////////////////////////////////////////////////////////////

CELGrassBlock::CELGrassBlock()
{
	m_pGrassBitsMap		= NULL;

	m_pGrassesVerts		= NULL;
	m_nNumGrasses		= 0;

	m_bBuilt			= false;
	m_bAboutToBuild		= false;
	m_bAboutToDestroy	= false;
}

CELGrassBlock::~CELGrassBlock()
{
}

bool CELGrassBlock::Init(CELGrassBitsMap * pBitsMap, const ARectI& rectBlock)
{
	m_pGrassBitsMap		= pBitsMap;
	m_rectBlock			= rectBlock;

	m_bBuilt			= false;

	A3DVECTOR3 vecDeltaX = A3DVECTOR3(m_pGrassBitsMap->GetGridSize(), 0.0f, 0.0f);
	A3DVECTOR3 vecDeltaZ = A3DVECTOR3(0.0f, 0.0f, -m_pGrassBitsMap->GetGridSize());
	A3DVECTOR3 vecOrigin = m_pGrassBitsMap->GetOrigin() + vecDeltaX * m_rectBlock.left + vecDeltaZ * m_rectBlock.top;

	m_aabb.Mins = m_pGrassBitsMap->GetOrigin() + vecDeltaX * m_rectBlock.left + vecDeltaZ * m_rectBlock.top;
	m_aabb.Mins.y = 0.0f;
	m_aabb.Maxs = m_pGrassBitsMap->GetOrigin() + vecDeltaX * m_rectBlock.right + vecDeltaZ * m_rectBlock.bottom;
	m_aabb.Maxs.y = 800.0f;
	float temp = m_aabb.Mins.z;
	m_aabb.Mins.z = m_aabb.Maxs.z;
	m_aabb.Maxs.z = temp;
	m_aabb.CompleteCenterExts();

	m_vecCenter = m_aabb.Center;
	return true;
}

bool CELGrassBlock::Release()
{
	DestroyGrasses();
	return true;
}

bool CELGrassBlock::BuildGrasses()
{
	// now try to destroy old ones
	DestroyGrasses();

	if( !m_pGrassBitsMap || !m_pGrassBitsMap->GetGrassType() )
		return true;

	// then build the grasses here
	A3DVECTOR3 vecDeltaX = A3DVECTOR3(m_pGrassBitsMap->GetGridSize(), 0.0f, 0.0f);
	A3DVECTOR3 vecDeltaZ = A3DVECTOR3(0.0f, 0.0f, -m_pGrassBitsMap->GetGridSize());
	A3DVECTOR3 vecOrigin = m_pGrassBitsMap->GetOrigin() + vecDeltaX * m_rectBlock.left + vecDeltaZ * m_rectBlock.top;
	
	int nPitch = m_pGrassBitsMap->GetWidth() / 8;
	BYTE * pBitsMap = m_pGrassBitsMap->GetBitsMap();
	A3DVECTOR3 vecLineOrigin;
	A3DVECTOR3 vecPos;

	ARandomGen grassScatter;
	grassScatter.Init(m_rectBlock.left + m_rectBlock.top);

	int	x, y;

	int s = 5 - int(m_pGrassBitsMap->GetGrassType()->GetGrassLand()->GetLODLevel() * 4 + 0.5f);
	if( s == 5 )
	{
		m_nNumGrasses = 0;
		m_bBuilt = true;
		m_bAboutToDestroy = false;
		m_bAboutToBuild = false;
		return true;
	}
	else if( s < 1 ) 
		s = 1;

	// first scan to get out the number of grasses
	m_nNumGrasses = 0;
	for(y=m_rectBlock.top; y<m_rectBlock.bottom; y+=s)
	{
		for(x=m_rectBlock.left; x<m_rectBlock.right; x+=s)
		{
			BYTE data = pBitsMap[y * nPitch + (x >> 3)];
			if( data & (1 << (7 - (x & 7))) )
			{
				m_nNumGrasses ++;
			}
		}
	}
	
	if( m_nNumGrasses == 0 )
	{
		m_bBuilt = true;
		m_bAboutToDestroy = false;
		m_bAboutToBuild = false;
		return true;
	}

	int nNumVertOne = m_pGrassBitsMap->GetGrassType()->GetNumOneGrassVert();
	GRASSVERTEX * pVertsOne = m_pGrassBitsMap->GetGrassType()->GetVertsTemplate();

	m_pGrassesVerts = new GRASSVERTEX[m_nNumGrasses * nNumVertOne];
	GRASSVERTEX * pVerts = m_pGrassesVerts;

	float vSize = m_pGrassBitsMap->GetGrassType()->GetDefineData().vSize;
	float vSizeVar = m_pGrassBitsMap->GetGrassType()->GetDefineData().vSizeVar;
	float vSoftness = m_pGrassBitsMap->GetGrassType()->GetDefineData().vSoftness;
	vecLineOrigin = vecOrigin;
	for(y=m_rectBlock.top; y<m_rectBlock.bottom; y+=s)
	{
		vecPos = vecLineOrigin;
		for(x=m_rectBlock.left; x<m_rectBlock.right; x+=s)
		{
			BYTE data = pBitsMap[y * nPitch + (x >> 3)];

			// let random generator run
			A3DVECTOR3 vecGrassRandPos = vecPos;
			vecGrassRandPos.x += grassScatter.RandomFloat(-0.5f, 0.5f) * m_pGrassBitsMap->GetGridSize();
			vecGrassRandPos.z += grassScatter.RandomFloat(-0.5f, 0.5f) * m_pGrassBitsMap->GetGridSize();
			vecGrassRandPos.y = m_pGrassBitsMap->GetGrassType()->GetGrassLand()->GetHeightAt(vecGrassRandPos) - 0.02f;
			float vRad = grassScatter.RandomFloat(0.0f, 6.28f);
			float vScale = vSize * grassScatter.RandomFloat(1.0f - vSizeVar, 1.0f + vSizeVar);
			A3DMATRIX4 tm = RotateY(vRad) * Scaling(vScale, vScale, vScale) * Translate(vecGrassRandPos.x, vecGrassRandPos.y, vecGrassRandPos.z);
			int nPhase = int(fabs(vecGrassRandPos.x - 10000.0f) * 2.0f + grassScatter.RandomFloat(0.0f, 10.0f)) % WIND_BUF_LEN;

			if( data & (1 << (7 - (x & 7))) )
			{
				// has one grass here
				for(int i=0; i<nNumVertOne; i++)
				{
					pVerts[i] = pVertsOne[i];

					pVerts[i].pos = pVerts[i].pos * tm;
					pVerts[i].normal.x = float(nPhase);
					pVerts[i].normal.y *= vSoftness;
				}

				pVerts += nNumVertOne;
			}

			vecPos = vecPos + vecDeltaX;
		}

		vecLineOrigin = vecLineOrigin + vecDeltaZ;
	}

	m_bBuilt = true;
	m_bAboutToDestroy = false;
	m_bAboutToBuild = false;
	return true;
}

bool CELGrassBlock::DestroyGrasses()
{
	if( m_pGrassesVerts )
	{
		delete [] m_pGrassesVerts;
		m_pGrassesVerts = NULL;
	}

	m_bBuilt = false;
	m_bAboutToDestroy = false;
	m_bAboutToBuild = false;

	m_nNumGrasses = 0;
	return true;
}

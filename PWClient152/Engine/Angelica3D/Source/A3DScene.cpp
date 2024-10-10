/*
 * FILE: A3DScene.cpp
 *
 * DESCRIPTION: Class representing the whole scene in A3D Engine
 *
 * CREATED BY: Hedi, 2002/3/25
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DScene.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DMaterial.h"
#include "A3DTexture.h"
#include "A3DDevice.h"
#include "A3DStream.h"
#include "A3DFrame.h"
#include "A3DMesh.h"
#include "A3DCamera.h"
#include "A3DViewport.h"
#include "A3DConfig.h"
#include "AFileImage.h"
#include "AFI.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

A3DScene::A3DScene()
{
	m_pA3DEngine		= NULL;
	m_pA3DDevice		= NULL;
	m_pA3DVisHandler	= NULL;

	m_nNumFaces			= 0;
	m_pAllFaces			= NULL;
	m_pSortedFaces		= NULL;

	m_nNumTextures		= 0;
	m_pTextureRecords	= NULL;
	m_pFaceRecords		= NULL;

	m_nNumVisibleFaces	= 0;

	m_Option.nVertexType = A3DVT_VERTEX;

	m_bRenderAll		= true;
	m_bLastRenderAll	= false;
}

A3DScene::~A3DScene()
{
}

bool A3DScene::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();

	if (m_pA3DDevice)
		m_pA3DDevice->AddUnmanagedDevObject(this);
	
	return true;
}

bool A3DScene::Release()
{
	if (m_pA3DDevice)
		m_pA3DDevice->RemoveUnmanagedDevObject(this);

	for(int i=0; i<m_nNumTextures; i++)
	{
	#ifdef A3DSCENE_USESTREAM

		if (m_pTextureRecords[i].pStream)
		{
			m_pTextureRecords[i].pStream->Release();
			delete m_pTextureRecords[i].pStream;
		}

	#endif

		if (m_pTextureRecords[i].pVerts)
			a_free(m_pTextureRecords[i].pVerts);
		
		if (m_pTextureRecords[i].pTexture)
			m_pA3DEngine->GetA3DTextureMan()->ReleaseTexture(m_pTextureRecords[i].pTexture);
		
		if (m_pTextureRecords[i].pDetailTexture)
			m_pA3DEngine->GetA3DTextureMan()->ReleaseTexture(m_pTextureRecords[i].pDetailTexture);
	}

	m_nNumTextures = 0;

	if (m_pTextureRecords)
	{
		a_free(m_pTextureRecords);
		m_pTextureRecords = NULL;
	}

	if( m_pFaceRecords )
	{
		a_free(m_pFaceRecords);
		m_pFaceRecords = NULL;
	}

	if( m_pSortedFaces )
	{
		a_free(m_pSortedFaces);
		m_pSortedFaces = NULL;
	}

	if( m_pAllFaces )
	{
		a_free(m_pAllFaces);
		m_pAllFaces = NULL;
	}

	m_pA3DVisHandler	= NULL;
	m_nNumFaces			= 0;
	m_nNumTextures		= 0;
	m_nNumVisibleFaces	= 0;
	m_bLastRenderAll	= false;

	return true;
}

bool A3DScene::Load(A3DEngine* pA3DEngine, const char* szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DScene::Load, Cannot open file %s!", szFile);
		return false;
	}

	if (!Load(pA3DEngine, &File))
	{
		File.Close();
		return false;
	}

	File.Close();

	return true;
}

bool A3DScene::Load(A3DEngine* pA3DEngine, AFile* pFileToLoad)
{
	if (!Init(m_pA3DEngine))
		return false;

	if (pFileToLoad->IsText())
	{
		return false;
	}
	else
	{
		int	i;
		DWORD dwReadLength;

		pFileToLoad->Read(&m_Option, sizeof(OPTION), &dwReadLength);
		pFileToLoad->Read(&m_nNumTextures, sizeof(int), &dwReadLength);

		m_pTextureRecords = (TEXTURE_RECORD*)a_malloc(sizeof(TEXTURE_RECORD) * m_nNumTextures);
		if (!m_pTextureRecords)
		{
			g_A3DErrLog.Log("A3DScene::Load(), Not enough memory!");
			return false;
		}

		pFileToLoad->Read(m_pTextureRecords, sizeof(TEXTURE_RECORD) * m_nNumTextures, &dwReadLength);
		for(i=0; i<m_nNumTextures; i++)
		{
			//Load needed textures;
			if( strlen(m_pTextureRecords[i].szTextureName) )
			{
				if( m_pA3DDevice && !m_pA3DEngine->GetA3DTextureMan()->LoadTextureFromFile(m_pTextureRecords[i].szTextureName, &m_pTextureRecords[i].pTexture) )
				{
					g_A3DErrLog.Log("A3DScene::AddMesh(), Can't load texture [%s]!", m_pTextureRecords[i].szTextureName);
				}
			}
			if( strlen(m_pTextureRecords[i].szDetailTextureName) )
			{
				if( m_pA3DDevice && !m_pA3DEngine->GetA3DTextureMan()->LoadTextureFromFile(m_pTextureRecords[i].szDetailTextureName, &m_pTextureRecords[i].pDetailTexture, A3DTF_DETAILMAP) )
				{
					g_A3DErrLog.Log("A3DScene::AddMesh(), Can't load texture [%s]!", m_pTextureRecords[i].szDetailTextureName);
				}
			}
			m_pTextureRecords[i].pVerts = (A3DVERTEX *)a_malloc(sizeof(A3DVERTEX) * m_pTextureRecords[i].nFaceNum * 3);
			if( NULL == m_pTextureRecords[i].pVerts )
			{
				g_A3DErrLog.Log("A3DScene::Load(), Not enough memory!");
				return false;
			}

		#ifdef A3DSCENE_USESTREAM

			if (!(m_pTextureRecords[i].pStream = new A3DStream))
			{
				g_A3DErrLog.Log("A3DScene::Load(), Not enough memory!");
				return false;
			}

			if (!m_pTextureRecords[i].pStream->Init(m_pA3DDevice, A3DVT_VERTEX, 
				m_pTextureRecords[i].nFaceNum * 3, 0, A3DSTRM_REFERENCEPTR, 0))
			{
				g_A3DErrLog.Log("A3DScene::Load(), Init a stream fail!");
				return false;
			}

		#endif
			
			m_pTextureRecords[i].nFaceVisible = 0;
		}

		pFileToLoad->Read(&m_nNumFaces, sizeof(int), &dwReadLength);
		m_pAllFaces = (A3DVERTEX *)a_malloc(sizeof(A3DVERTEX) * m_nNumFaces * 3);
		if( NULL == m_pAllFaces )
		{
			g_A3DErrLog.Log("A3DScene::Load(), Not enough memory!");
			return false;
		}

		m_pSortedFaces = (SORTEDFACE*)a_malloc(sizeof(SORTEDFACE) * m_nNumFaces);
		if( NULL == m_pSortedFaces )
		{
			g_A3DErrLog.Log("A3DScene::Load(), Not enough memory!");
			return false;
		}

		m_pFaceRecords = (FACE_RECORD*)a_malloc(sizeof(FACE_RECORD) * m_nNumFaces);
		if( NULL == m_pFaceRecords )
		{
			g_A3DErrLog.Log("A3DScene::Load(), Not enough memory!");
			return false;
		}
		
		pFileToLoad->Read(m_pAllFaces, sizeof(A3DVERTEX) * m_nNumFaces * 3, &dwReadLength);
		pFileToLoad->Read(m_pFaceRecords, sizeof(FACE_RECORD) * m_nNumFaces, &dwReadLength);

		for(i=0; i<m_nNumFaces; i++)
			m_pFaceRecords[i].nRefTicks = -1;
	}
	return true;
}

bool A3DScene::Save(AFile * pFileToSave)
{
	if( pFileToSave->IsText() )
	{
		return false;
	}
	else
	{
		DWORD		dwWriteLength;
		
		pFileToSave->Write(&m_Option, sizeof (OPTION), &dwWriteLength);
		pFileToSave->Write(&m_nNumTextures, sizeof(int), &dwWriteLength);
		pFileToSave->Write(m_pTextureRecords, sizeof(TEXTURE_RECORD) * m_nNumTextures, &dwWriteLength);

		pFileToSave->Write(&m_nNumFaces, sizeof(int), &dwWriteLength);
		pFileToSave->Write(m_pAllFaces, sizeof(A3DVERTEX) * m_nNumFaces * 3, &dwWriteLength);
		pFileToSave->Write(m_pFaceRecords, sizeof(FACE_RECORD) * m_nNumFaces, &dwWriteLength);
		return true;
	}
	return true;
}

bool A3DScene::Import(char * szMoxFileName)
{
	AFileImage	theMoxFile;

	if( !theMoxFile.Open(szMoxFileName, AFILE_OPENEXIST) )
	{
		g_A3DErrLog.Log("A3DScene::Import(), Can't find mox file [%s]!", szMoxFileName);
		return false;
	}

	A3DFrame	theFrame;

	if( !theFrame.Load(m_pA3DDevice, &theMoxFile) )
	{
		theMoxFile.Close();
		g_A3DErrLog.Log("A3DScene::Import(), Can't load mox file");
		return false;
	}

	A3DMesh * pMesh;

	pMesh = theFrame.GetFirstMesh();
	while( pMesh )
	{
		AddMesh(pMesh);

		pMesh = theFrame.GetNextMesh();
	}

	theFrame.Release();
	theMoxFile.Close();
	return true;
}

bool A3DScene::Export(char * szMoxFileName)
{
	return true;
}

bool A3DScene::AddMesh(A3DMesh * pMesh)
{
	int	nVertCount;
	int nFaceCount;

	nVertCount = pMesh->GetVertCount();
	nFaceCount = pMesh->GetIndexCount() / 3;

	// First determine the texture record index;
	A3DTexture *	pMeshTexture = pMesh->GetTexture();
	A3DTexture *	pMeshDetailTexture = pMesh->GetDetailTexture();
	A3DMATRIX4		matDetailTM = pMesh->GetDetailTM();
	A3DMaterial *	pMeshMaterial = pMesh->GetMaterial();

	char			szTextureName[MAX_PATH];
	char			szDetailTextureName[MAX_PATH];
	
	szTextureName[0] = szDetailTextureName[0] = '\0';
	strncpy(szTextureName, pMesh->GetTextureMap(), MAX_PATH);
	strncpy(szDetailTextureName, pMesh->GetDetailTextureMap(), MAX_PATH);
	TEXTURE_RECORD* pTextureRecord = FindTextureRecord(szTextureName, pMeshMaterial->GetMaterialParam(), pMeshMaterial->Is2Sided(), pMesh->GetProperty());
	if( NULL == pTextureRecord )
	{
		// Create a new texture record;
		m_nNumTextures ++;
		m_pTextureRecords = (TEXTURE_RECORD*)a_realloc(m_pTextureRecords, m_nNumTextures * sizeof(TEXTURE_RECORD));
		if( NULL == m_pTextureRecords )
		{
			g_A3DErrLog.Log("A3DScene::AddMesh(), Not enough memory!");
			return false;
		}

		pTextureRecord = &m_pTextureRecords[m_nNumTextures - 1];
		ZeroMemory(pTextureRecord, sizeof(TEXTURE_RECORD));
		
		// Initialize this texture record;
		strncpy(pTextureRecord->szTextureName, szTextureName, MAX_PATH);
		if( strlen(pTextureRecord->szTextureName) )
		{
			if( m_pA3DDevice && !m_pA3DEngine->GetA3DTextureMan()->LoadTextureFromFile(szTextureName, &pTextureRecord->pTexture) )
			{
				g_A3DErrLog.Log("A3DScene::AddMesh(), Can't load texture [%s]!", szTextureName);
			}
		}

		strncpy(pTextureRecord->szDetailTextureName, szDetailTextureName, MAX_PATH);
		if( strlen(pTextureRecord->szDetailTextureName) )
		{
			if( m_pA3DDevice && !m_pA3DEngine->GetA3DTextureMan()->LoadTextureFromFile(szDetailTextureName, &pTextureRecord->pDetailTexture, A3DTF_DETAILMAP) )
			{
				g_A3DErrLog.Log("A3DScene::AddMesh(), Can't load texture [%s]!", szDetailTextureName);
			}
			pTextureRecord->matDetailTM = matDetailTM;
		}

		pTextureRecord->material		= pMeshMaterial->GetMaterialParam();
		pTextureRecord->b2Sided			= pMeshMaterial->Is2Sided();
		pTextureRecord->MeshProps		= pMesh->GetProperty();
		pTextureRecord->nFaceNum		= 0;
		pTextureRecord->nFaceVisible	= 0;
	#ifdef A3DSCENE_USESTREAM
		pTextureRecord->pStream			= NULL;
	#endif
		pTextureRecord->pVerts			= NULL;
		pTextureRecord->nTexID			= m_nNumTextures - 1;

		bool bAlpha = false;
		if( !m_pA3DDevice || !pTextureRecord->pTexture )
		{
			if (af_CheckFileExt(szTextureName, ".tga", 4) ||
				af_CheckFileExt(szTextureName, ".sdr", 4))
				bAlpha = true;
		}
		else
			bAlpha = pTextureRecord->pTexture->IsAlphaTexture();

		if( bAlpha || pTextureRecord->material.Diffuse.a < 0.99f )
			pTextureRecord->dwRenderFlag = A3DSCENE_RENDER_ALPHA;
		else
			pTextureRecord->dwRenderFlag = A3DSCENE_RENDER_SOLID;
	}

	pTextureRecord->nFaceNum += nFaceCount;
#ifdef A3DSCENE_USESTREAM
	if( pTextureRecord->pStream )
	{
		pTextureRecord->pStream->Release();
		delete pTextureRecord->pStream;
	}
	pTextureRecord->pStream = new A3DStream();
	if( !pTextureRecord->pStream->Init(m_pA3DDevice, A3DVT_VERTEX, pTextureRecord->nFaceNum * 3, 0, 0, A3DSTRM_REFERENCEPTR) )
	{
		g_A3DErrLog.Log("A3DScene::AddMesh(), Init a stream fail!");
		return false;
	}
#endif
	pTextureRecord->pVerts = (A3DVERTEX *)a_realloc(pTextureRecord->pVerts, pTextureRecord->nFaceNum * 3 * sizeof(A3DVERTEX));
	if( NULL == pTextureRecord->pVerts )
	{
		g_A3DErrLog.Log("A3DScene::AddMesh(), Not enough memory!");
		return false;
	}

	m_nNumFaces += nFaceCount;
	m_pAllFaces = (A3DVERTEX *)a_realloc(m_pAllFaces, m_nNumFaces * 3 * sizeof(A3DVERTEX));
	if( NULL == m_pAllFaces )
	{
		g_A3DErrLog.Log("A3DScene::AddMesh(), Not enough memory!");
		return false;
	}
	m_pSortedFaces = (SORTEDFACE*)a_realloc(m_pSortedFaces, m_nNumFaces * sizeof(SORTEDFACE));
	if( NULL == m_pSortedFaces )
	{
		g_A3DErrLog.Log("A3DScene::AddMesh(), Not enough memory!");
		return false;
	}
	m_pFaceRecords = (FACE_RECORD*)a_realloc(m_pFaceRecords, m_nNumFaces * sizeof(FACE_RECORD));
	if( NULL == m_pFaceRecords )
	{
		g_A3DErrLog.Log("A3DScene::AddMesh(), Not enough memory!");
		return false;
	}

	A3DVERTEX * pMeshVerts = pMesh->GetVerts(0); // We use only the mesh's first animation frame;
	WORD * pMeshIndices = pMesh->GetIndices();
	for(int i=0; i<nFaceCount; i++)
	{
		int idFace = m_nNumFaces - nFaceCount + i;
		
		m_pAllFaces[idFace * 3    ] = pMeshVerts[pMeshIndices[i * 3    ]];
		m_pAllFaces[idFace * 3 + 1] = pMeshVerts[pMeshIndices[i * 3 + 1]];
		m_pAllFaces[idFace * 3 + 2] = pMeshVerts[pMeshIndices[i * 3 + 2]];

		m_pFaceRecords[idFace].nTexID = pTextureRecord->nTexID;
		m_pFaceRecords[idFace].nRefTicks = -1;
	}

	return true;
}

A3DScene::TEXTURE_RECORD* A3DScene::FindTextureRecord(const char* szTextureName, 
					const A3DMATERIALPARAM& material, bool b2Sided, const A3DMESHPROP& MeshProps)
{
	for(int i=0; i<m_nNumTextures; i++)
	{
		if (_stricmp(szTextureName, m_pTextureRecords[i].szTextureName))
			continue;

		if (m_pTextureRecords[i].b2Sided ^ b2Sided)
			continue;

		if (m_pTextureRecords[i].MeshProps != MeshProps)
			continue;

		if (m_pTextureRecords[i].material.Ambient != material.Ambient)
			continue;

		if (m_pTextureRecords[i].material.Diffuse != material.Diffuse)
			continue;

		if (m_pTextureRecords[i].material.Specular != material.Specular)
			continue;

		if (m_pTextureRecords[i].material.Emissive != material.Emissive)
			continue;

		if (m_pTextureRecords[i].material.Power != material.Power)
			continue;

		return &m_pTextureRecords[i];
	}

	return NULL;
}

int FaceSortCompare(const void *arg1, const void *arg2)
{
	return ((A3DScene::SORTEDFACE*)arg1)->vDisToCam - ((A3DScene::SORTEDFACE*)arg2)->vDisToCam > 0.0f ? 1 : -1;
}

// Render current visible sets sorting by distance to camera;
bool A3DScene::RenderSort(A3DViewport * pCurrentViewport, DWORD dwFlag, bool bNear2Far)
{
#ifdef A3DSCENE_USESTREAM
	return false; // using stream can not sort;
#endif

	A3DVECTOR3		vecCamPos = pCurrentViewport->GetCamera()->GetPos();

	// First construct sorting buffer;
	int				i, n;
	int				nSortedFaceNum = 0;

	for(i=0; i<m_nNumTextures; i++)
	{
		if( !(dwFlag & m_pTextureRecords[i].dwRenderFlag) )
			continue;
	
		A3DVERTEX *		pVerts;
		pVerts = m_pTextureRecords[i].pVerts;
		for(n=0; n<m_pTextureRecords[i].nFaceVisible; n++)
		{
			A3DVECTOR3 vecCenter;
			vecCenter.x = (pVerts[n * 3].x + pVerts[n * 3 + 1].x + pVerts[n * 3 + 2].x) / 3.0f;
			vecCenter.y = (pVerts[n * 3].y + pVerts[n * 3 + 1].y + pVerts[n * 3 + 2].y) / 3.0f;
			vecCenter.z = (pVerts[n * 3].z + pVerts[n * 3 + 1].z + pVerts[n * 3 + 2].z) / 3.0f;
			m_pSortedFaces[nSortedFaceNum].vDisToCam = a3d_Magnitude(vecCamPos - vecCenter) * (bNear2Far ? 1.0f : -1.0f);
			m_pSortedFaces[nSortedFaceNum].pTexRecord = &m_pTextureRecords[i];
			m_pSortedFaces[nSortedFaceNum].wIndexInTexVisible = n;

			nSortedFaceNum ++;
		}
	}

	qsort(m_pSortedFaces, nSortedFaceNum, sizeof(SORTEDFACE), FaceSortCompare);

	// Now render the sorted faces;
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());

#define MAX_RENDER_FACE			256

	A3DVERTEX			pRenderVerts[MAX_RENDER_FACE * 3];
	int					nRenderFaceNum = 0;
	TEXTURE_RECORD *	pLastTex = NULL;

	for(i=0; i<nSortedFaceNum; i++)
	{
		TEXTURE_RECORD*	pThisTex = m_pSortedFaces[i].pTexRecord;

		if( pThisTex != pLastTex )
		{
			if( pLastTex )
			{
				// Flush the render buffer;
				m_pA3DDevice->SetFVF(A3DFVF_A3DVERTEX);
				m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, nRenderFaceNum, pRenderVerts, sizeof(A3DVERTEX));
				nRenderFaceNum = 0;

				pLastTex->pTexture->Disappear(0);

				if( pLastTex->pDetailTexture && g_pA3DConfig->GetFlagDetailScene() )
				{
					pLastTex->pDetailTexture->Disappear(1);
					m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
				}
			}

			pThisTex->pTexture->Appear(0);

			m_pA3DDevice->SetMaterial(&pThisTex->material, pThisTex->b2Sided);
			if( pThisTex->b2Sided )
				m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
			else
				m_pA3DDevice->SetFaceCull(A3DCULL_CCW);

			if( pThisTex->pDetailTexture && g_pA3DConfig->GetFlagDetailScene() )
			{
				pThisTex->pDetailTexture->Appear(1);
				m_pA3DDevice->SetTextureColorOP(1, A3DTOP_ADDSIGNED);
				m_pA3DDevice->SetTextureCoordIndex(1, 0);
				m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
				m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_COUNT2);
				m_pA3DDevice->SetTextureMatrix(1, pThisTex->matDetailTM);
			}

			pLastTex = pThisTex;
		}

		pRenderVerts[nRenderFaceNum * 3    ] = pThisTex->pVerts[m_pSortedFaces[i].wIndexInTexVisible * 3    ];
		pRenderVerts[nRenderFaceNum * 3 + 1] = pThisTex->pVerts[m_pSortedFaces[i].wIndexInTexVisible * 3 + 1];
		pRenderVerts[nRenderFaceNum * 3 + 2] = pThisTex->pVerts[m_pSortedFaces[i].wIndexInTexVisible * 3 + 2];
		nRenderFaceNum ++;

		if( nRenderFaceNum >= MAX_RENDER_FACE )
		{
			m_pA3DDevice->SetFVF(A3DFVF_A3DVERTEX);
			m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, nRenderFaceNum, pRenderVerts, sizeof(A3DVERTEX));
			nRenderFaceNum = 0;
		}
	}

	if( nRenderFaceNum > 0 )
	{
		m_pA3DDevice->SetFVF(A3DFVF_A3DVERTEX);
		m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, nRenderFaceNum, pRenderVerts, sizeof(A3DVERTEX));
	}

	if( pLastTex )
	{
		pLastTex->pTexture->Disappear(0);

		if( pLastTex->pDetailTexture && g_pA3DConfig->GetFlagDetailScene() )
		{
			pLastTex->pDetailTexture->Disappear(1);
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
		}
	}

	return true;
}

bool A3DScene::Render(A3DViewport * pCurrentViewport, DWORD dwFlag)
{
	if (!m_pA3DDevice)
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
	
	for(int i=0; i<m_nNumTextures; i++)
	{
		if( !(dwFlag & m_pTextureRecords[i].dwRenderFlag) )
			continue;
	
		if( m_pTextureRecords[i].nFaceVisible > 0 )
		{
			if( m_pTextureRecords[i].pTexture )
				m_pTextureRecords[i].pTexture->Appear(0);

			if( m_pTextureRecords[i].pDetailTexture && g_pA3DConfig->GetFlagDetailScene() )
			{
				m_pTextureRecords[i].pDetailTexture->Appear(1);
				m_pA3DDevice->SetTextureColorOP(1, A3DTOP_ADDSIGNED);
				m_pA3DDevice->SetTextureCoordIndex(1, 0);
				m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
				m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_COUNT2);
				m_pA3DDevice->SetTextureMatrix(1, m_pTextureRecords[i].matDetailTM);
			}

			m_pA3DDevice->SetMaterial(&m_pTextureRecords[i].material, m_pTextureRecords[i].b2Sided);
			if( m_pTextureRecords[i].b2Sided )
				m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
			else
				m_pA3DDevice->SetFaceCull(A3DCULL_CCW);

		#ifndef A3DSCENE_USESTREAM
			m_pA3DDevice->SetDeviceVertexShader(A3DFVF_A3DVERTEX);
			m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, m_pTextureRecords[i].nFaceVisible, m_pTextureRecords[i].pVerts, sizeof(A3DVERTEX));
		#else
			m_pTextureRecords[i].pStream->Appear();
			m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, m_pTextureRecords[i].nFaceVisible);
		#endif

			if( m_pTextureRecords[i].pTexture )
				m_pTextureRecords[i].pTexture->Disappear(0);

			if( m_pTextureRecords[i].pDetailTexture && g_pA3DConfig->GetFlagDetailScene() )
			{
				m_pTextureRecords[i].pDetailTexture->Disappear(1);
				m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
			}
		}
	}

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);

	return true;
}

bool A3DScene::UpdateVisibleSets(A3DViewport* pCurrentViewport)
{
	if (m_nNumTextures == 0)
		return true;

	int i;
	bool bVisChange = false;

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);

	if (!m_pA3DVisHandler || m_bRenderAll)
	{
	AllRender:

		if (!m_bLastRenderAll)
		{
			for (i=0; i < m_nNumTextures; i++)
				m_pTextureRecords[i].nFaceVisible = 0;

			for (i=0; i < m_nNumFaces; i++)
			{
				int idTex = m_pFaceRecords[i].nTexID;
				int nFaceVisible = m_pTextureRecords[idTex].nFaceVisible++;

				memcpy(m_pTextureRecords[idTex].pVerts + nFaceVisible * 3, m_pAllFaces + i * 3, 3 * sizeof (A3DVERTEX));
			}

			bVisChange = true;
		}

		m_bLastRenderAll = true;
	}
	else 
	{
		int iNumSurf, *pSurfs;

		DWORD dwRet = m_pA3DVisHandler->GetVisibleSurfs(pCurrentViewport, &pSurfs, &iNumSurf);
		if (dwRet != A3DSceneVisible::GVS_OK_NOCHANGE && 
			dwRet != A3DSceneVisible::GVS_OK)
			goto AllRender;

		if (m_bLastRenderAll || dwRet != A3DSceneVisible::GVS_OK_NOCHANGE)
		{
			for (i=0; i < m_nNumTextures; i++)
				m_pTextureRecords[i].nFaceVisible = 0;

			//	Visible set has been changed
			for (i=0; i < iNumSurf; i++)
			{
				int indexFace = pSurfs[i];
				int idTex = m_pFaceRecords[indexFace].nTexID;
				int nFaceVisible = m_pTextureRecords[idTex].nFaceVisible++;

				memcpy(m_pTextureRecords[idTex].pVerts + nFaceVisible * 3, m_pAllFaces + indexFace * 3, 3 * sizeof (A3DVERTEX));
			}

			bVisChange = true;
		}

		m_bLastRenderAll = false;
	}

	m_nNumVisibleFaces = 0;

	for (i=0; i < m_nNumTextures; i++)
	{
	#ifdef A3DSCENE_USESTREAM
	
		if (bVisChange && m_pTextureRecords[i].nFaceVisible)
			m_pTextureRecords[i].pStream->SetVertsDynamic((LPBYTE)m_pTextureRecords[i].pVerts, m_pTextureRecords[i].nFaceVisible * 3);

	#endif

		m_nNumVisibleFaces += m_pTextureRecords[i].nFaceVisible;
	}

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);

	return true;
}

bool A3DScene::GetAllFaceVerts(A3DVECTOR3 * pVertsBuffer, int * pnBufferSize)
{
	if( !pVertsBuffer || *pnBufferSize < int(m_nNumFaces * 3 * sizeof(A3DVECTOR3)))
	{
		*pnBufferSize = m_nNumFaces * 3 * sizeof(A3DVECTOR3);
		return false;
	}

	for(int i=0; i<m_nNumFaces; i++)
	{
		pVertsBuffer[i * 3 + 0].x = m_pAllFaces[i * 3 + 0].x;
		pVertsBuffer[i * 3 + 0].y = m_pAllFaces[i * 3 + 0].y;
		pVertsBuffer[i * 3 + 0].z = m_pAllFaces[i * 3 + 0].z;

		pVertsBuffer[i * 3 + 1].x = m_pAllFaces[i * 3 + 1].x;
		pVertsBuffer[i * 3 + 1].y = m_pAllFaces[i * 3 + 1].y;
		pVertsBuffer[i * 3 + 1].z = m_pAllFaces[i * 3 + 1].z;

		pVertsBuffer[i * 3 + 2].x = m_pAllFaces[i * 3 + 2].x;
		pVertsBuffer[i * 3 + 2].y = m_pAllFaces[i * 3 + 2].y;
		pVertsBuffer[i * 3 + 2].z = m_pAllFaces[i * 3 + 2].z;
	}

	return true;
}

//	Before device reset
bool A3DScene::BeforeDeviceReset()
{
	return true;
}

//	After device reset
bool A3DScene::AfterDeviceReset()
{
	m_bLastRenderAll = false;

	if (m_pA3DVisHandler)
		m_pA3DVisHandler->RecalcVisibleSet();

	return true;
}


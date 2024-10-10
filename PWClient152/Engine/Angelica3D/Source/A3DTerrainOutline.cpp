/*
 * FILE: A3DTerrainOutline.cpp
 *
 * DESCRIPTION: Class representing for terrain outline in A3D Engine;
 *
 * CREATED BY: Hedi, 2004/8/17
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DTerrainOutline.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DCamera.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "A3DTextureMan.h"
#include "A3DViewport.h"
#include "AFile.h"
#include "A3DFuncs.h"
#include "A3DPI.h"

#define new A_DEBUG_NEW

A3DTerrainOutline::A3DTerrainOutline()
{
	m_pClusterOffsets		= NULL;
	m_pFileOutline			= NULL;
	m_pSampleDir			= NULL;
	m_pOutlineTexture		= NULL;

	m_pOutlines				= NULL;
	m_pStreamLT				= NULL;
	m_pStreamRT				= NULL;
	m_pStreamLB				= NULL;
	m_pStreamRB				= NULL;

	m_vViewRadius			= 500.0f;
}

A3DTerrainOutline::~A3DTerrainOutline()
{
	Release();
}

bool A3DTerrainOutline::Init(A3DDevice * pA3DDevice, A3DCamera * pHostCamera, A3DCOLOR colorFog, const char * szOutlineTex)
{
	m_pA3DDevice	= pA3DDevice;
	m_pHostCamera	= pHostCamera;
	m_colorFog		= colorFog;

	m_nNumSamples	= 0;
	m_nNumClustersX = 0;
	m_nNumClustersZ = 0;
	m_nNumLT		= 0;
	m_nNumRT		= 0;
	m_nNumLB		= 0;
	m_nNumRB		= 0;

	memset(m_Clusters, 0, sizeof(OUTLINECLUSTER) * 4);
	for(int i=0; i<4; i++)
	{
		m_Clusters[i].nCX = -1;
		m_Clusters[i].nCY = -1;
		m_Clusters[i].bToRelease = false;
	}

	if( szOutlineTex )
	{
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szOutlineTex, &m_pOutlineTexture) )
		{
			g_A3DErrLog.Log("A3DTerrainOutline::Init(), Can not load outline texture!");
			return false;
		}
	}

	return true;
}

bool A3DTerrainOutline::Release()
{
	ReleaseOutlines();

	ReleaseCluster(m_Clusters[0]);
	ReleaseCluster(m_Clusters[1]);
	ReleaseCluster(m_Clusters[2]);
	ReleaseCluster(m_Clusters[3]);

	if( m_pOutlineTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pOutlineTexture);
		m_pOutlineTexture = NULL;
	}

	if( m_pFileOutline )
	{
		m_pFileOutline->Close();
		delete m_pFileOutline;
		m_pFileOutline = NULL;
	}

	if( m_pClusterOffsets )
	{
		delete [] m_pClusterOffsets;
		m_pClusterOffsets = NULL;
	}

	return true;
}

bool A3DTerrainOutline::Load(const char * szOutlineFile, const A3DVECTOR3& vecCenter)
{
	DWORD		dwReadLen;

	A3DVECTOR3 vecDeltaX;
	A3DVECTOR3 vecDeltaZ;

	m_pFileOutline = new AFile();
	if( !m_pFileOutline->Open(szOutlineFile, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		g_A3DErrLog.Log("A3DTerrainOutline::Load(), Can not open outline file!");
		return false;
	}

	// read the terrain's information
	if( !m_pFileOutline->Read(&m_nTerrainWidth, sizeof(int), &dwReadLen) )
		goto READFAIL;
	if( !m_pFileOutline->Read(&m_nTerrainHeight, sizeof(int), &dwReadLen) )
		goto READFAIL;
	if( !m_pFileOutline->Read(&m_vTerrainMaxHeight, sizeof(float), &dwReadLen) )
		goto READFAIL;
	if( !m_pFileOutline->Read(&m_vTerrainGridSize, sizeof(float), &dwReadLen) )
		goto READFAIL;

	// read outline generation parameters.
	if( !m_pFileOutline->Read(&m_vStepSize, sizeof(float), &dwReadLen) )
		goto READFAIL;
	if( !m_pFileOutline->Read(&m_vProjStart, sizeof(float), &dwReadLen) )
		goto READFAIL;
	if( !m_pFileOutline->Read(&m_vProjEnd, sizeof(float), &dwReadLen) )
		goto READFAIL;
	if( !m_pFileOutline->Read(&m_nNumSamples, sizeof(int), &dwReadLen) )
		goto READFAIL;
	if( !m_pFileOutline->Read(&m_nNumClustersX, sizeof(int), &dwReadLen) )
		goto READFAIL;
	if( !m_pFileOutline->Read(&m_nNumClustersZ, sizeof(int), &dwReadLen) )
		goto READFAIL;
	if( !m_pFileOutline->Read(&m_vHeightLow, sizeof(float), &dwReadLen) )
		goto READFAIL;
	if( !m_pFileOutline->Read(&m_vHeightMiddle, sizeof(float), &dwReadLen) )
		goto READFAIL;
	if( !m_pFileOutline->Read(&m_vHeightHigh, sizeof(float), &dwReadLen) )
		goto READFAIL;

	// load the offset of each cluster.
	m_pClusterOffsets = new DWORD[m_nNumClustersX * m_nNumClustersZ];
	m_pFileOutline->Seek(-4, AFILE_SEEK_END);
	DWORD dwOffsetStart;
	if( !m_pFileOutline->Read(&dwOffsetStart, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;
	m_pFileOutline->Seek(dwOffsetStart, AFILE_SEEK_SET);
	if( !m_pFileOutline->Read(m_pClusterOffsets, sizeof(DWORD) * m_nNumClustersX * m_nNumClustersZ, &dwReadLen) )
		goto READFAIL;

	// initialize some internal variables
	m_vSampleRad = A3D_PI * 2.0f / m_nNumSamples;
	m_nMaxSamples = m_nNumSamples << 2;

	if( !CreateOutlines() )
	{
		g_A3DErrLog.Log("A3DTerrainOutline::Load(), Fail to create outlines!");
		return false;
	}

	vecDeltaX = A3DVECTOR3(m_vTerrainGridSize, 0.0f, 0.0f);
	vecDeltaZ = A3DVECTOR3(0.0f, 0.0f, -m_vTerrainGridSize);
	m_vecOrigin = vecCenter - vecDeltaX * ((m_nTerrainWidth - 1) / 2.0f) - vecDeltaZ * ((m_nTerrainHeight - 1.0f) / 2.0f);
	return true;

READFAIL:
	g_A3DErrLog.Log("A3DTerrainOutline::Load(), Fail to read from outline file!");
	return false;
}

bool A3DTerrainOutline::LoadCluster(OUTLINECLUSTER& cluster, int nCX, int nCY)
{
	// first locate the file pointer to the start of that cluster;
	DWORD		dwOffset = m_pClusterOffsets[nCY * m_nNumClustersX + nCX];
	m_pFileOutline->Seek(dwOffset, AFILE_SEEK_SET);

	DWORD		dwReadLen;

	// first load the lowest samples from disk
	if( !m_pFileOutline->Read(&cluster.numSegments, sizeof(int), &dwReadLen) )
		goto READFAIL;

	int j;
	cluster.pSegments = new OUTLINESEGMENT[cluster.numSegments];
	for(j=0; j<cluster.numSegments; j++)
	{
		OUTLINESEGMENT& segment = cluster.pSegments[j];
		if( !m_pFileOutline->Read(&segment.start, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !m_pFileOutline->Read(&segment.num, sizeof(int), &dwReadLen) )
			goto READFAIL;

		segment.pSamples = new OUTLINESAMPLE[segment.num];

		if( !m_pFileOutline->Read(segment.pSamples, sizeof(OUTLINESAMPLE) * segment.num, &dwReadLen) )
			goto READFAIL;
	}

	int numSegments;
	
	// then load the middle height samples
	if( !m_pFileOutline->Read(&numSegments, sizeof(int), &dwReadLen) )
		goto READFAIL;
	if( numSegments != 0 )
	{
		g_A3DErrLog.Log("A3DTerrainOutline::LoadCluster(), middle sampling not supported now!");
		return false;
	}

	// then load the high height samples
	if( !m_pFileOutline->Read(&numSegments, sizeof(int), &dwReadLen) )
		goto READFAIL;
	if( numSegments != 0 )
	{
		g_A3DErrLog.Log("A3DTerrainOutline::LoadCluster(), high sampling not supported now!");
		return false;
	}

	cluster.nCX = nCX;
	cluster.nCY = nCY;
	cluster.bToRelease = false;
	return true;

READFAIL:
	g_A3DErrLog.Log("A3DTerrainOutline::LoadCluster(), Read from outline file failed!");
	return false;
}

bool A3DTerrainOutline::ReleaseCluster(OUTLINECLUSTER& cluster)
{
	// see has been inited.
	if( cluster.pSegments == NULL )
		return true;

	if( cluster.pSegments )
	{
		for(int j=0; j<cluster.numSegments; j++)
		{
			OUTLINESEGMENT & segment = cluster.pSegments[j];
			if( segment.pSamples )
			{
				delete [] segment.pSamples;
				segment.pSamples = NULL;
			}
		}

		delete [] cluster.pSegments;
		cluster.pSegments = NULL;
	}

	cluster.bToRelease = false;
	cluster.nCX = -1;
	cluster.nCY = -1;

	return true;
}

bool A3DTerrainOutline::FindClusterFromCache(OUTLINECLUSTER& cluster, int nCX, int nCY)
{
	for(int i=0; i<4; i++)
	{
		if( m_ClustersLast[i].nCX == nCX && m_ClustersLast[i].nCY == nCY )
		{
			cluster = m_ClustersLast[i];
			m_ClustersLast[i].bToRelease = false;
			return true;
		}
	}
	
	return false;
}

bool A3DTerrainOutline::ConstructOutline(const OUTLINECLUSTER& cluster, const A3DVECTOR3& vecCenter, const A3DVECTOR3& vecClusterCenter, A3DLVERTEX * pOutlines, int& nNum)
{
	A3DLVERTEX *	pVert;
	A3DVECTOR3		vecTop;

	int				i, j;
	bool			bBreak = false;

	nNum = 0;
	pVert = pOutlines;

	pVert[0].x = vecCenter.x;
	pVert[0].y = vecCenter.y;
	pVert[0].z = vecCenter.z;

	pVert ++;
	for(i=0; i<cluster.numSegments; i++)
	{
		OUTLINESEGMENT& segment = cluster.pSegments[i];
		for(j=0; j<segment.num; j++)
		{
			OUTLINESAMPLE& sample = segment.pSamples[j];
			vecTop		= vecClusterCenter + m_pSampleDir[(j + segment.start) % m_nNumSamples] * sample.d;
			vecTop.y	= sample.top;
			
			//float f = sample.d / m_vProjEnd;
			int alpha = 0xff;//(int)((1.0f - f) * 255.0f);
			if( bBreak )
			{
				bBreak = false;		

				AddToOutlines(pVert[0], vecCenter);
				pVert[0].diffuse = (alpha << 24)  | 0x00ffffff;

				pVert ++;
				nNum ++;
				if( nNum == m_nMaxSamples )
					break;
			}
			
			AddToOutlines(pVert[0], vecTop);
			pVert[0].diffuse = (alpha << 24)  | 0x00ffffff;

			pVert ++;
			nNum ++;
			if( nNum == m_nMaxSamples )
				break;
		}

		bBreak = true;
	}

	return true;
}

bool A3DTerrainOutline::Update()
{
	A3DVECTOR3 vecCamPos = m_pHostCamera->GetPos();
	int nX, nZ;
	float vX, vZ;
	float fx, fz;

	A3DVECTOR3 vecXStep, vecZStep;
	vecXStep = A3DVECTOR3(m_vStepSize, 0.0f, 0.0f);
	vecZStep = A3DVECTOR3(0.0f, 0.0f, -m_vStepSize);

	vX = (vecCamPos.x - m_vecOrigin.x) / m_vStepSize;
	vZ = (m_vecOrigin.z - vecCamPos.z) / m_vStepSize;
	nX = (int) vX;
	nZ = (int) vZ;
	fx = vX - nX;
	fz = vZ - nZ;
	
	// don't update if it is out of range
	if( nX < 1 || nX >= m_nNumClustersX - 1 || nZ < 1 || nZ >= m_nNumClustersZ - 1 )
		return false;

	// now update it;
	int		i;

	// first back up current cluster's information
	for(i=0; i<4; i++)
	{
		m_ClustersLast[i] = m_Clusters[i];
		m_ClustersLast[i].bToRelease = true;
	}
	
	// first of all try to get one cluster that has been loaded
	if( !FindClusterFromCache(m_Clusters[0], nX, nZ) )
		LoadCluster(m_Clusters[0], nX, nZ);
	if( !FindClusterFromCache(m_Clusters[1], nX + 1, nZ) )
		LoadCluster(m_Clusters[1], nX + 1, nZ);
	if( !FindClusterFromCache(m_Clusters[2], nX, nZ + 1) )
		LoadCluster(m_Clusters[2], nX, nZ + 1);
	if( !FindClusterFromCache(m_Clusters[3], nX + 1, nZ + 1) )
		LoadCluster(m_Clusters[3], nX + 1, nZ + 1);

	bool bNeedUpdateStream = false;
	// now we know which cluster to be release
	for(i=0; i<4; i++)
	{
		if( m_ClustersLast[i].bToRelease )
		{
			ReleaseCluster(m_ClustersLast[i]);
			bNeedUpdateStream = true;
		}
	}

	if( 1 )//bNeedUpdateStream )
	{
		A3DVECTOR3 vecLTCenter, vecRTCenter, vecLBCenter, vecRBCenter;
		vecLTCenter = m_vecOrigin + nX * vecXStep + nZ * vecZStep;
		vecRTCenter = vecLTCenter + vecXStep;
		vecLBCenter = vecLTCenter + vecZStep;
		vecRBCenter = vecLBCenter + vecXStep;

		vecCamPos.y = -m_vProjStart * 0.15f;
		ConstructOutline(m_Clusters[0], vecCamPos, vecLTCenter, m_pOutlines, m_nNumLT);
		m_pStreamLT->SetVertsDynamic((BYTE *)m_pOutlines, m_nNumLT + 1);

		ConstructOutline(m_Clusters[1], vecCamPos, vecRTCenter, m_pOutlines, m_nNumRT);
		m_pStreamRT->SetVertsDynamic((BYTE *)m_pOutlines, m_nNumRT + 1);

		ConstructOutline(m_Clusters[2], vecCamPos, vecLBCenter, m_pOutlines, m_nNumLB);
		m_pStreamLB->SetVertsDynamic((BYTE *)m_pOutlines, m_nNumLB + 1);

		ConstructOutline(m_Clusters[3], vecCamPos, vecRBCenter, m_pOutlines, m_nNumRB);
		m_pStreamRB->SetVertsDynamic((BYTE *)m_pOutlines, m_nNumRB + 1);
	}

	return true;
}

bool A3DTerrainOutline::RenderForReflected(A3DViewport * pCurrentViewport)
{
	m_pA3DDevice->ClearTexture(0);
	bool bFogEnable = m_pA3DDevice->GetFogEnable();
	bool bFogTableEnable = m_pA3DDevice->GetFogTableEnable();
	bool bFogVertEnable = m_pA3DDevice->GetFogVertEnable();
	m_pA3DDevice->SetFogEnable(true);
	m_pA3DDevice->SetFogTableEnable(false);
	m_pA3DDevice->SetFogVertEnable(false);
	
	A3DMATRIX4 matWorld = IdentityMatrix();

	A3DCameraBase * pCamera = pCurrentViewport->GetCamera();

	float vFront, vBack;
	vFront = pCamera->GetZFront();
	vBack = pCamera->GetZBack();
	pCamera->SetZFrontAndBack(min2(m_vViewRadius, m_vProjStart) * 0.8f, m_vProjEnd * 1.2f, true);
	pCamera->Active();

	if( m_pOutlineTexture )
		m_pOutlineTexture->Appear(0);

	//matWorld._42 = m_pHostCamera->GetPos().y - m_pTerrain->GetPosHeight(m_pHostCamera->GetPos()) - 2.0f;
	m_pA3DDevice->SetWorldMatrix(matWorld);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetLightingEnable(false);

	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
					  
	m_pStreamLT->Appear(0);
	m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLEFAN, 0, m_nNumLT - 1);

	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFaceCull(A3DCULL_CW);

	pCamera->SetZFrontAndBack(vFront, vBack, true);
	pCamera->Active();

	if( m_pOutlineTexture )
		m_pOutlineTexture->Disappear(0);
	
	m_pA3DDevice->SetFogEnable(bFogEnable);
	m_pA3DDevice->SetFogTableEnable(bFogTableEnable);
	m_pA3DDevice->SetFogVertEnable(bFogVertEnable);
	return true;
}

bool A3DTerrainOutline::Render(A3DViewport * pCurrentViewport)
{
	m_pA3DDevice->ClearTexture(0);
	bool bFogEnable = m_pA3DDevice->GetFogEnable();
	bool bFogTableEnable = m_pA3DDevice->GetFogTableEnable();
	bool bFogVertEnable = m_pA3DDevice->GetFogVertEnable();
	m_pA3DDevice->SetFogEnable(true);
	m_pA3DDevice->SetFogTableEnable(false);
	m_pA3DDevice->SetFogVertEnable(false);

	A3DMATRIX4 matWorld = IdentityMatrix();

	if( m_pOutlineTexture )
		m_pOutlineTexture->Appear(0);

	A3DCameraBase * pCamera = pCurrentViewport->GetCamera();

	float vFront, vBack;
	vFront = pCamera->GetZFront();
	vBack = pCamera->GetZBack();
	pCamera->SetZFrontAndBack(min2(m_vViewRadius, m_vProjStart) * 0.8f, m_vProjEnd * 1.2f, true);
	pCamera->Active();

	m_pA3DDevice->SetWorldMatrix(matWorld);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_CW);
					 
	if( !(GetKeyState('1') & 0x8000) )
	{
		m_pStreamLT->Appear(0);
		m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLEFAN, 0, m_nNumLT - 1);
	}

	/*
	if( !(GetKeyState('2') & 0x8000) )
	{
		m_pStreamRT->Appear(0);
		m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLEFAN, 0, m_nNumRT - 1);
	}*/
	
	if( !(GetKeyState('3') & 0x8000) )
	{
		m_pStreamLB->Appear(0);
		m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLEFAN, 0, m_nNumLB - 1);
	}

	/*
	if( !(GetKeyState('4') & 0x8000) )
	{
		m_pStreamRB->Appear(0);
		m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLEFAN, 0, m_nNumRB - 1);
	}*/
	
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);

	pCamera->SetZFrontAndBack(vFront, vBack, true);
	pCamera->Active();

	if( m_pOutlineTexture )
		m_pOutlineTexture->Disappear(0);
	
	m_pA3DDevice->SetFogEnable(bFogEnable);
	m_pA3DDevice->SetFogTableEnable(bFogTableEnable);
	m_pA3DDevice->SetFogVertEnable(bFogVertEnable);
	return true;
}

bool A3DTerrainOutline::CreateOutlines()
{
	// try to release old ones.
	ReleaseOutlines();

	if( m_nNumSamples )
	{
		m_pSampleDir = new A3DVECTOR3[m_nNumSamples + 1];
		m_pOutlines = new A3DLVERTEX[m_nMaxSamples + 1];
		
		float		alpha = 0.0f;
		float		delta = m_vSampleRad;
		float		x, z;
		A3DVECTOR3  vecDir;

		//1*----3
		// |\	|
		// | \	|
		// |  \	|
		// |   \|
		//0*----*2

		int i;
		// now prepare direction table
		for(i=0; i<=m_nNumSamples; i++)
		{
			// first bottom point, never change again
			x = (float)cos(alpha);
			z = (float)sin(alpha);

			vecDir = A3DVECTOR3(x, 0.0f, z);
			m_pSampleDir[i] = vecDir;

			alpha += delta;
		}

		// first center point
		m_pOutlines[0] = A3DLVERTEX(A3DVECTOR3(0.0f), 0xffffffff, 0x00000000, 0.0f, 0.0f);
		// setup outline vertex buffer
		for(i=1; i<=m_nMaxSamples; i++)
		{
			m_pOutlines[i] = A3DLVERTEX(A3DVECTOR3(0.0f), 0xffffffff, 0x00000000, 0.0f, 1.0f);
		}

		m_pStreamLT = new A3DStream();
		if( !m_pStreamLT->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nMaxSamples + 1, 0, A3DSTRM_REFERENCEPTR, 0) )
		{
			g_A3DErrLog.Log("A3DTerrainOutline::CreateOutlines, fail to init stream outline!");
			return false;
		}

		m_pStreamRT = new A3DStream();
		if( !m_pStreamRT->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nMaxSamples + 1, 0, A3DSTRM_REFERENCEPTR, 0) )
		{
			g_A3DErrLog.Log("A3DTerrainOutline::CreateOutlines, fail to init stream outline!");
			return false;
		}

		m_pStreamLB = new A3DStream();
		if( !m_pStreamLB->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nMaxSamples + 1, 0, A3DSTRM_REFERENCEPTR, 0) )
		{
			g_A3DErrLog.Log("A3DTerrainOutline::CreateOutlines, fail to init stream outline!");
			return false;
		}

		m_pStreamRB = new A3DStream();
		if( !m_pStreamRB->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nMaxSamples + 1, 0, A3DSTRM_REFERENCEPTR, 0) )
		{
			g_A3DErrLog.Log("A3DTerrainOutline::CreateOutlines, fail to init stream outline!");
			return false;
		}
	}

	return true;
}

bool A3DTerrainOutline::ReleaseOutlines()
{
	if( m_pSampleDir )
	{
		delete [] m_pSampleDir;
		m_pSampleDir = NULL;
	}

	if( m_pOutlines )
	{
		delete [] m_pOutlines;
		m_pOutlines = NULL;
	}

	if( m_pStreamLT )
	{
		m_pStreamLT->Release();
		delete m_pStreamLT;
		m_pStreamLT = NULL;
	}

	if( m_pStreamRT )
	{
		m_pStreamRT->Release();
		delete m_pStreamRT;
		m_pStreamRT = NULL;
	}

	if( m_pStreamLB )
	{
		m_pStreamLB->Release();
		delete m_pStreamLB;
		m_pStreamLB = NULL;
	}

	if( m_pStreamRB )
	{
		m_pStreamRB->Release();
		delete m_pStreamRB;
		m_pStreamRB = NULL;
	}

	return true;
}

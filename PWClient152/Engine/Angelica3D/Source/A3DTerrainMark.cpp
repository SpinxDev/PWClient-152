/*
 * FILE: A3DTerrainMark.cpp
 *
 * DESCRIPTION: Terrain mark is a square patch that lies on the terrain
 *
 * CREATED BY: Hedi, 2001/11/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DTerrainMark.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DStream.h"
#include "A3DTerrain.h"
#include "A3DTexture.h"
#include "A3DWorld.h"
#include "A3DViewport.h"
#include "A3DVertex.h"
#include "A3DTextureMan.h"
#include "A3DConfig.h"

A3DTerrainMark::A3DTerrainMark()
{							
	m_pA3DDevice	= NULL;
	m_pA3DTexture	= NULL;
	m_pA3DStream	= NULL;
}

A3DTerrainMark::~A3DTerrainMark()
{
}

bool A3DTerrainMark::Init(A3DDevice * pA3DDevice, char * szTexture)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	m_pA3DDevice = pA3DDevice;
	m_pA3DTerrain = m_pA3DDevice->GetA3DEngine()->GetA3DWorld()->GetA3DTerrain();

	m_matMyWorldTM.Identity();

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szTexture, &m_pA3DTexture) )
		return false;

	m_pA3DStream = new A3DStream();
	if (!m_pA3DStream)
	{
		g_A3DErrLog.Log("A3DTerrainMark::Init Not enough memory!");
		return false;
	}

	if (!m_pA3DStream->Init(m_pA3DDevice, A3DVT_LVERTEX, 4, 6, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR))
	{
		g_A3DErrLog.Log("A3DTerrainMark::Init init stream fail!");
		return false;
	}

	SetAllParameters(0, 0, A3DCOLORRGBA(255, 255, 255, 0), 0.0f);
	return true;
}

bool A3DTerrainMark::Release()
{
	if( m_pA3DStream )
	{
		m_pA3DStream->Release();
		delete m_pA3DStream;
		m_pA3DStream = NULL;
	}

	if( m_pA3DTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DTexture);
		m_pA3DTexture = NULL;
	}
	return true;
}

bool A3DTerrainMark::Render(A3DViewport * pCurrentViewport)
{
	if( !m_pA3DDevice ) return true;

	bool bval;

	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetWorldMatrix(m_matMyWorldTM);
	m_pA3DTexture->Appear(0);
	m_pA3DStream->Appear();

	bval = m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2);
	if( !bval ) return false;

 	m_pA3DTexture->Disappear(0);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	return true;
}

bool A3DTerrainMark::SetAllParameters(int x, int y, A3DCOLOR color, FLOAT vSize)
{
	if( !m_pA3DDevice ) return true;
    
	bool			bval;
	A3DVECTOR3		pVertexPos[4];
	A3DVECTOR3		vecCenterPos;
	WORD			pIndex[6];

	A3DLVERTEX*		pVBVerts;
	WORD*			pVBIndices;

	m_pA3DTerrain->GetCellAllVertex(x, y, pVertexPos, pIndex);
	vecCenterPos = m_pA3DTerrain->GetCellCenterPos(x, y);

	bval = m_pA3DStream->LockVertexBuffer(0, 0, (LPBYTE *)&pVBVerts, NULL);
	if( !bval ) return false;
	bval = m_pA3DStream->LockIndexBuffer(0, 0, (LPBYTE*) &pVBIndices, NULL);
	if( !bval ) return false;

	for(int i=0; i<4; i++)
	{
		A3DVECTOR3 vecPos;

		vecPos = vecCenterPos + (pVertexPos[i] - vecCenterPos) * (vSize / m_pA3DTerrain->GetCellSize());
		vecPos.y += 0.1f;
		pVBVerts[i] = A3DLVERTEX(vecPos, color, A3DCOLORRGBA(0, 0, 0, 255), 
			(i % 2) * 1.0f, i / 2.0f);
	}
	memcpy(pVBIndices, pIndex, sizeof(WORD) * 6);

	bval = m_pA3DStream->UnlockIndexBuffer();
	if( !bval ) return false;
	bval = m_pA3DStream->UnlockVertexBuffer();
	if( !bval ) return false;

	return true;
}
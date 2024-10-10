/*
 * FILE: A3DSkyBox.cpp
 *
 * DESCRIPTION: Class that standing for the sky box in A3D Engine
 *
 * CREATED BY: Hedi, 2002/5/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DSkyBox.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DTextureMan.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "A3DConfig.h"
#include "A3DCameraBase.h"
#include "A3DFuncs.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

A3DSkyBox::A3DSkyBox()
{
	m_pA3DDevice	= NULL;
	m_pCamera		= NULL;
	m_pSkyFace		= NULL;

	ZeroMemory(m_pMapTexture, sizeof(A3DTexture *) * 6);
}

A3DSkyBox::~A3DSkyBox()
{
}

bool A3DSkyBox::Init(A3DDevice* pA3DDevice, A3DCameraBase* pCamera, const char* szRight, const char* szLeft,
					 const char* szTop, const char* szBottom, const char* szFront, const char* szBack)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	m_pA3DDevice = pA3DDevice;
	m_pCamera = pCamera;

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szRight, "Textures\\Sky", &m_pMapTexture[0], A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkyBox::Init(), Can not load texture [%s]", szRight);
		return false;
	}
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szLeft, "Textures\\Sky", &m_pMapTexture[1], A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkyBox::Init(), Can not load texture [%s]", szLeft);
		return false;
	}
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTop, "Textures\\Sky", &m_pMapTexture[2], A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkyBox::Init(), Can not load texture [%s]", szTop);
		return false;
	}
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szBottom, "Textures\\Sky", &m_pMapTexture[3], A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkyBox::Init(), Can not load texture [%s]", szBottom);
		return false;
	}
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szFront, "Textures\\Sky", &m_pMapTexture[4], A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkyBox::Init(), Can not load texture [%s]", szFront);
		return false;
	}
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szBack, "Textures\\Sky", &m_pMapTexture[5], A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkyBox::Init(), Can not load texture [%s]", szBack);
		return false;
	}

	m_pSkyFace = new A3DStream();
	if( NULL == m_pSkyFace )
	{
		g_A3DErrLog.Log("A3DSkyBox::Init(), Not enough memory!");
		return false;
	}

	if (!m_pSkyFace->Init(m_pA3DDevice, A3DVT_LVERTEX, 4, 6, A3DSTRM_STATIC, A3DSTRM_STATIC))
	{
		g_A3DErrLog.Log("A3DSkyBox::Init(), Init the A3DStream fail!");
		return false;
	}

	A3DLVERTEX *	pVerts;
	WORD *			pIndices;

	// Fill verts;
	if( !m_pSkyFace->LockVertexBuffer(0, 0, (LPBYTE *)&pVerts, NULL) )
		return false;
	pVerts[0] = A3DLVERTEX(A3DVECTOR3(-50.0f,  50.0f, 50.0f), 0xffffffff, 0xff000000, 0.0f, 0.0f);
	pVerts[1] = A3DLVERTEX(A3DVECTOR3( 50.0f,  50.0f, 50.0f), 0xffffffff, 0xff000000, 1.0f, 0.0f);
	pVerts[2] = A3DLVERTEX(A3DVECTOR3(-50.0f, -50.0f, 50.0f), 0xffffffff, 0xff000000, 0.0f, 1.0f);
	pVerts[3] = A3DLVERTEX(A3DVECTOR3( 50.0f, -50.0f, 50.0f), 0xffffffff, 0xff000000, 1.0f, 1.0f);
	m_pSkyFace->UnlockVertexBuffer();

	// Fill indcies;
	if( !m_pSkyFace->LockIndexBuffer(0, 0, (LPBYTE *)&pIndices, NULL) )
		return false;
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
	pIndices[3] = 1; pIndices[4] = 3; pIndices[5] = 2;
	m_pSkyFace->UnlockIndexBuffer();
	return true;
}

bool A3DSkyBox::Release()
{
	if( m_pSkyFace )
	{
		m_pSkyFace->Release();
		delete m_pSkyFace;
		m_pSkyFace = NULL;
	}

	for(int i=0; i<6; i++)
	{
		if( m_pMapTexture[i] )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pMapTexture[i]);
			m_pMapTexture[i] = NULL;
		}
	}
	return true;
}

bool A3DSkyBox::Render()
{
	if( !m_pA3DDevice ) return true;

	if( m_pCamera )
	{
		A3DVECTOR3 vecPos;
		if( m_pCamera->IsMirrored() )
		{
			vecPos = m_pCamera->GetMirrorCamera()->GetPos();
			A3DMATRIX4 matMirror = m_pCamera->GetMirrorMatrix();
			vecPos = vecPos * InverseTM(matMirror);
		}
		else
		{
			vecPos = m_pCamera->GetPos();
		}
		m_WorldMatrix = a3d_Translate(vecPos.x, vecPos.y, vecPos.z);
	}
	
	m_pA3DDevice->SetLightingEnable(false);
	bool bFogEnable = m_pA3DDevice->GetFogEnable();
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	
	//First of all show top cap of the surround cloud;
	m_pA3DDevice->SetWorldMatrix(m_WorldMatrix);

	A3DMATRIX4 matWorld;
	A3DVECTOR3 vecDir, vecUp;

	m_pSkyFace->Appear();
	for(int i=0; i<6; i++)
	{
		a3d_GetCubeMapDirAndUp(i, &vecDir, &vecUp);

		matWorld = a3d_TransformMatrix(vecDir, vecUp, A3DVECTOR3(0.0f)) * m_WorldMatrix;
		m_pA3DDevice->SetWorldMatrix(matWorld);
		if( m_pMapTexture[i] )
			m_pMapTexture[i]->Appear(0);
		else
			m_pA3DDevice->ClearTexture(0);

		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2);

		if( m_pMapTexture[i] )
			m_pMapTexture[i]->Disappear(0);
	}

	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFogEnable(bFogEnable);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	return true;
}

bool A3DSkyBox::TickAnimation()
{
	if( !m_pA3DDevice ) return true;

	return true;
}

bool A3DSkyBox::SetMapTexture(int nIndex, char * szMapTexture)
{
	if( !m_pA3DDevice ) return true;

	if( nIndex < 0 || nIndex >= 6 )
		return false;

	if( m_pMapTexture[nIndex] )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pMapTexture[nIndex]);
		m_pMapTexture[nIndex] = NULL;
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szMapTexture, "Textures\\Sky", &m_pMapTexture[nIndex], A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkyBox::Init(), Can not load texture [%s]", szMapTexture);
		return false;
	}

	return true;
}
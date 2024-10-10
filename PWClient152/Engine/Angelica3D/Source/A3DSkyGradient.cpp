/*
 * FILE: A3DSkyGradient.h
 *
 * DESCRIPTION: Class that standing for a gradient sky implementation algorithm, which
				take use of a gradient color and a hemisphere cloud sky
 *
 * CREATED BY: Hedi, 2002/5/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2003 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DSkyGradient.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "A3DTextureMan.h"
#include "A3DStream.h"
#include "A3DConfig.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
#include "A3DCameraBase.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

A3DSkyGradient::A3DSkyGradient()
{
	m_pA3DDevice			= NULL;
	m_pCamera				= NULL;

	m_pTextureSkyCloud		= NULL;
	m_pTextureSkyGradient	= NULL;
	m_pSkyCloud				= NULL;
	m_pSkyGradient			= NULL;

	m_vRadius				= 100.0f;
	m_vPitchRange			= DEG2RAD(17.5f);
	m_vTile					= 2.0f;
	m_nSegmentCount			= 8; // Hemisphere segment number ranging [1, n]

	m_vFlySpeedU = 0.003f;
	m_vFlySpeedV = 0.003f;
}

A3DSkyGradient::~A3DSkyGradient()
{
}

bool A3DSkyGradient::Init(A3DDevice* pA3DDevice, A3DCameraBase* pCamera, const char* szSkyCloud, const char* szSkyGradient, float vTile)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	m_pA3DDevice = pA3DDevice;
	m_vTile = vTile;
	m_pCamera = pCamera;

	// First load textures;
	if( szSkyCloud[0] )
	{
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szSkyCloud, "Textures\\Sky", &m_pTextureSkyCloud, A3DTF_MIPLEVEL, 1) )
		{
			g_A3DErrLog.Log("A3DSkyGradient::Init(), Can not load texture [%s]", szSkyCloud);
			return false;
		}
	}
	if( szSkyGradient[0] )
	{
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szSkyGradient, "Textures\\Sky", &m_pTextureSkyGradient, A3DTF_MIPLEVEL, 1) )
		{
			g_A3DErrLog.Log("A3DSkyGradient::Init(), Can not load texture [%s]", szSkyGradient);
			return false;
		}
	}

	// Now calculate the face count and vert count of the hemisphere sky cone part;
	m_nVertCount = 1 + m_nSegmentCount * (4 * m_nSegmentCount);
	m_nFaceCount = 4 * m_nSegmentCount + (m_nSegmentCount - 1) * (4 * m_nSegmentCount) * 2;

	// Then create stream for rendering usage
	m_pSkyCloud = new A3DStream();
	if (!m_pSkyCloud)
	{
		g_A3DErrLog.Log("A3DSkyGradient::Init(), Not enough memory!");
		return false;
	}

	if (!m_pSkyCloud->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nVertCount, m_nFaceCount * 3,	A3DSTRM_STATIC, A3DSTRM_STATIC))
	{
		g_A3DErrLog.Log("A3DSkyGradient::Init(), Init the A3DStream fail!");
		return false;
	}

	m_pSkyGradient = new A3DStream();
	if (!m_pSkyGradient)
	{
		g_A3DErrLog.Log("A3DSkyGradient::Init(), Not enough memory!");
		return false;
	}

	if (!m_pSkyGradient->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nVertCount, m_nFaceCount * 3, A3DSTRM_STATIC, A3DSTRM_STATIC))
	{
		g_A3DErrLog.Log("A3DSkyGradient::Init(), Init the A3DStream fail!");
		return false;
	}

	A3DLVERTEX *	pVerts;
	WORD *			pIndices;
	A3DLVERTEX *	pVertsGradient;
	WORD *			pIndicesGradient;

	// Fill verts;
	if( !m_pSkyCloud->LockVertexBuffer(0, 0, (LPBYTE *)&pVerts, NULL) )
		return false;
	if( !m_pSkyGradient->LockVertexBuffer(0, 0, (LPBYTE *)&pVertsGradient, NULL) )
		return false;

	float r = m_vRadius * (float)sin(m_vPitchRange);
	float h = m_vRadius - m_vRadius * (float)cos(m_vPitchRange);

	// First it is my cap center vertex;
	pVertsGradient[0] = pVerts[0] = A3DLVERTEX(A3DVECTOR3(0.0f, m_vRadius, 0.0f), 0xffffffff, 0xff000000, 0.0f, 0.0f);

	// Then hemisphere middle part;
	int i, ix, iy;
	int is = m_nSegmentCount * 4; // vert count of each segment
	int nVertIndex = 1;
	for(iy=0; iy<m_nSegmentCount; iy++)
	{
		for(ix=0; ix<4 * m_nSegmentCount; ix++)
		{
			float alpha = A3D_PI * 2.0f * ix / is;
			float pitch = A3D_PI / 2.0f - (iy + 1) * m_vPitchRange / m_nSegmentCount;
			
			float vX = float(m_vRadius * cos(alpha) * cos(pitch));
			float vY = float(m_vRadius * sin(pitch));
			float vZ = float(m_vRadius * sin(alpha) * cos(pitch));

			pVertsGradient[nVertIndex] = pVerts[nVertIndex] = A3DLVERTEX(A3DVECTOR3(vX, vY, vZ), 0xffffffff, 0xff000000, 0.0f, 0.0f);
			pVertsGradient[nVertIndex].tu = alpha / 2.0f / A3D_PI;
			pVertsGradient[nVertIndex].tv = float(iy + 1) / m_nSegmentCount; 

			if( iy == m_nSegmentCount - 1 )
			{
				pVerts[nVertIndex].diffuse = 0x00ffffff;
				pVertsGradient[nVertIndex].y -= h;
			}
			
			nVertIndex ++;
		}
	}

	// Now we apply a plain texture maping to it;
	for(i=0; i<m_nVertCount; i++)
	{
		pVerts[i].tu = pVerts[i].x / r;
		pVerts[i].tv = pVerts[i].z / r;
	}

	m_pSkyGradient->UnlockVertexBuffer();
	m_pSkyCloud->UnlockVertexBuffer();

	// Fill the indcies;
	if( !m_pSkyCloud->LockIndexBuffer(0, 0, (LPBYTE *)&pIndices, NULL) )
		return false;
	if( !m_pSkyGradient->LockIndexBuffer(0, 0, (LPBYTE *)&pIndicesGradient, NULL) )
		return false;

	// First conic cap;
	int nFaceIndex = 0;
	for(i=0; i<is; i++)
	{
		pIndices[nFaceIndex * 3] = 0;
		pIndices[nFaceIndex * 3 + 1] = 1 + i;
		pIndices[nFaceIndex * 3 + 2] = 1 + ((i + 1) % is);

		nFaceIndex ++;
	}

	// Then middle part;
	for(iy=0; iy<m_nSegmentCount-1; iy++)
	{
		for(ix=0; ix<is; ix++)
		{
			int index0 = 1 + iy * is + ix;
			int index1 = 1 + iy * is + ((ix + 1) % is);
			int index2 = index0 + is;
			int index3 = index1 + is;

			pIndices[nFaceIndex * 3] = index0;
			pIndices[nFaceIndex * 3 + 1] = index2;
			pIndices[nFaceIndex * 3 + 2] = index3;

			pIndices[nFaceIndex * 3 + 3] = index0;
			pIndices[nFaceIndex * 3 + 4] = index3;
			pIndices[nFaceIndex * 3 + 5] = index1;																

			nFaceIndex += 2;
		}
	}
	memcpy(pIndicesGradient, pIndices, sizeof(WORD) * m_nFaceCount * 3);

	m_pSkyCloud->UnlockIndexBuffer();
	m_pSkyGradient->UnlockIndexBuffer();

	m_tmCloud.Identity();
	return true;
}

bool A3DSkyGradient::Build()
{
	// Not implemented yet;
	return true;
}

bool A3DSkyGradient::Release()
{
	if( m_pSkyCloud )
	{
		m_pSkyCloud->Release();
		delete m_pSkyCloud;
		m_pSkyCloud = NULL;
	}

	if( m_pSkyGradient )
	{
		m_pSkyGradient->Release();
		delete m_pSkyGradient;
		m_pSkyGradient = NULL;
	}

	if( m_pTextureSkyCloud )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSkyCloud);
		m_pTextureSkyCloud = NULL;
	}

	if( m_pTextureSkyGradient )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSkyGradient);
		m_pTextureSkyGradient = NULL;
	}
	return true;
}

bool A3DSkyGradient::Render()
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
		m_WorldMatrix = a3d_Translate(vecPos.x, vecPos.y - m_vRadius * (float)cos(m_vPitchRange), vecPos.z);
	}

	m_pA3DDevice->SetLightingEnable(false);
	bool bFogEnable = m_pA3DDevice->GetFogEnable();
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetAlphaTestEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetWorldMatrix(m_WorldMatrix);
	
	if( m_pTextureSkyGradient )
	{
		m_pSkyGradient->Appear();
		m_pTextureSkyGradient->Appear(0);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_WRAP0, D3DWRAP_U);
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCount, 0, m_nFaceCount);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_WRAP0, 0);
		m_pTextureSkyGradient->Disappear(0);
	}

	if( m_pTextureSkyCloud )
	{
		m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_COUNT2);
		m_pA3DDevice->SetTextureMatrix(0, m_tmCloud);
		m_pSkyCloud->Appear();
		m_pTextureSkyCloud->Appear(0);
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCount, 0, m_nFaceCount);
		m_pTextureSkyCloud->Disappear(0);
		m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	}

	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFogEnable(bFogEnable);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	return true;
}

bool A3DSkyGradient::TickAnimation()
{
	if( !m_pA3DDevice ) return true;

	m_tmCloud._11 = m_vTile;
	m_tmCloud._22 = m_vTile;
	m_tmCloud._31 += m_vFlySpeedU / 30.0f;
	m_tmCloud._32 += m_vFlySpeedV / 30.0f;
	return true;
}

bool A3DSkyGradient::SetSkyCloudMap(char * szCloudTexture)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureSkyCloud )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSkyCloud);
		m_pTextureSkyCloud = NULL;
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szCloudTexture, "Textures\\Sky", &m_pTextureSkyCloud, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkyGradient::Init(), Can not load texture [%s]", szCloudTexture);
		return false;
	}

	return true;
}

bool A3DSkyGradient::SetSkyGradientMap(char * szGradientTexture)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureSkyGradient )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSkyGradient);
		m_pTextureSkyGradient = NULL;
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szGradientTexture, "Textures\\Sky", &m_pTextureSkyGradient, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkyGradient::Init(), Can not load texture [%s]", szGradientTexture);
		return false;
	}

	return true;
}
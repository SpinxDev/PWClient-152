/*
 * FILE: A3DPerlinNormalMap.cpp
 *
 * DESCRIPTION: a normal map generated with perlin noise dynamically, need pixel shader 2.0 at least
 *
 * CREATED BY: Hedi, 2005/8/3
 *
 * HISTORY:
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DTexture.h"
#include "A3DCamera.h"
#include "A3DRenderTarget.h"
#include "A3DStream.h"
#include "A3DPixelShader.h"
#include "A3DShaderMan.h"
#include "A3DVertex.h"
#include "A3DFuncs.h"
#include "A3DMacros.h"
#include "A3DPI.h"
#include "A3DTextureMan.h"
#include "A3DViewport.h"
#include "A3DPerlinNormalMap.h"

A3DPerlinNormalMap::A3DPerlinNormalMap()
{
	m_pA3DDevice		= NULL;
	m_pNormalMapTarget	= NULL;
	m_pViewport			= NULL;
	m_pCamera			= NULL;
	m_pStream			= NULL;
	m_pPixelShader		= NULL;

	m_nSize				= 0;

	memset(m_pNoiseMaps, 0, sizeof(A3DTexture *) * 6);
}

A3DPerlinNormalMap::~A3DPerlinNormalMap()
{
	Release();
}

bool A3DPerlinNormalMap::Init(A3DDevice * pA3DDevice, int nSize, float vAmplitude, float vPersistence)
{
	m_pA3DDevice	= pA3DDevice;
	m_nSize			= nSize;

	m_vAmplitude	= vAmplitude;
	m_vPersistence	= vPersistence;

	// create render target here;
	A3DRenderTarget::RTFMT fmt;
	fmt.iWidth		= m_nSize;
	fmt.iHeight		= m_nSize;
	fmt.fmtTarget	= A3DFMT_X8R8G8B8;
	fmt.fmtDepth	= A3DFMT_D24X8;

	m_pNormalMapTarget = new A3DRenderTarget();
	if( !m_pNormalMapTarget->Init(m_pA3DDevice, fmt, true, true) )
	{
		g_A3DErrLog.Log("A3DPerlinNormalMap::Init(), failed to create render target!");
		return false;
	}
	m_pCamera = new A3DCamera();
	if( !m_pCamera->Init(m_pA3DDevice, DEG2RAD(90.0f), 0.2f, 2000.0f, 1.0f) )
	{
		g_A3DErrLog.Log("A3DPerlinNormalMap::Init(), failed to create camera!");
		return false;
	}

	if( !m_pA3DDevice->CreateViewport(&m_pViewport, 0, 0, fmt.iWidth, fmt.iHeight,
									0.0f, 1.0f, false, false, 0xFF000000) )
	{
		g_A3DErrLog.Log("A3DPerlinNormalMap::Init(), failed to create viewport!");
		return false;
	}
	m_pViewport->SetCamera(m_pCamera);

	// now initialize the stream
	m_pStream = new A3DStream;
	if( !m_pStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, 6, 0, A3DSTRM_STATIC, 0) )
	{
		g_A3DErrLog.Log("A3DPerlinNormalMap::Init(), failed to create stream!");
		return false;
	}

	A3DTLVERTEX * pVerts;
	if( !m_pStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, 0) )
	{
		g_A3DErrLog.Log("A3DPerlinNormalMap::Init(), failed to lock stream!");
		return false;
	}
	
	pVerts[0] = A3DTLVERTEX(A3DVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff, 0xff000000, 0.0f, 0.0f);
	pVerts[1] = A3DTLVERTEX(A3DVECTOR4((float)m_nSize, 0.0f, 0.0f, 1.0f), 0xffffffff, 0xff000000, 1.0f, 0.0f);
	pVerts[2] = A3DTLVERTEX(A3DVECTOR4(0.0f, (float)m_nSize, 0.0f, 1.0f), 0xffffffff, 0xff000000, 0.0f, 1.0f);

	pVerts[3] = pVerts[2];
	pVerts[4] = pVerts[1];
	pVerts[5] = A3DTLVERTEX(A3DVECTOR4((float)m_nSize, (float)m_nSize, 0.0f, 1.0f), 0xffffffff, 0xff000000, 1.0f, 1.0f);

	m_pStream->UnlockVertexBuffer();

	// load pixel shader used to composite the normal map
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\2.2\\ps\\perlinnm.txt", false) )
	{
		g_A3DErrLog.Log("A3DPerlinNormalMap::Init(), failed to load pixel shader");
		return false;
	}

	char	szNoiseMap[MAX_PATH];
	for(int i=0; i<6; i++)
	{
		A3DTexture * pNoiseMap;

		sprintf(szNoiseMap, "textures\\noisemap\\noisemap%0d", i + 1);
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szNoiseMap, &pNoiseMap) )
		{
			g_A3DErrLog.Log("A3DPerlinNormalMap::Init(), failed to load normal map [%s].", szNoiseMap);			
			return false;
		}

		m_pNoiseMaps[i] = pNoiseMap;
	}

	return true;
}

bool A3DPerlinNormalMap::Release()
{
	if( m_pPixelShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pPixelShader);
		m_pPixelShader = NULL;
	}

	if( m_pStream )
	{
		m_pStream->Release();
		delete m_pStream;
		m_pStream = NULL;
	}

	if( m_pCamera )
	{
		m_pCamera->Release();
		delete m_pCamera;
		m_pCamera = NULL;
	}

	if( m_pViewport )
	{
		m_pViewport->Release();
		delete m_pViewport;
		m_pViewport = NULL;
	}

	if( m_pNormalMapTarget )
	{
		m_pNormalMapTarget->Release();
		delete m_pNormalMapTarget;
		m_pNormalMapTarget = NULL;
	}

	for(int i=0; i<6; i++)
	{
		if( m_pNoiseMaps[i] )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pNoiseMaps[i]);
			m_pNoiseMaps[i] = NULL;
		}
	}

	return true;
}

bool A3DPerlinNormalMap::UpdateNormalMap(int nDeltaTime)
{
	int		i;

	m_pStream->Appear();
	for(i=0; i<6; i++)
	{
		m_pNoiseMaps[i]->Appear(i);
		
		A3DMATRIX4 mat;
		m_pA3DDevice->SetTextureCoordIndex(i, 0);
		m_pA3DDevice->SetTextureTransformFlags(i, A3DTTFF_COUNT3);
		m_pA3DDevice->SetTextureMatrix(i, mat);
	}

	m_pPixelShader->Appear();

	m_pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, 2);

	m_pPixelShader->Disappear();
	for(i=0; i<6; i++)
	{
		m_pNoiseMaps[i]->Disappear(i);		
		m_pA3DDevice->SetTextureCoordIndex(i, i);
	}

	return true;
}

bool A3DPerlinNormalMap::Appear(int nLayer)
{
	//m_pA3DDevice->GetD3DDevice()->SetTexture(nLayer, m_pNormalMapTarget->GetTargetTexture());
	m_pNormalMapTarget->AppearAsTexture(nLayer);
	return true;
}
bool A3DPerlinNormalMap::Disappear(int nLayer)
{
	m_pA3DDevice->ClearTexture(nLayer);
	//m_pA3DDevice->GetD3DDevice()->SetTexture(nLayer, NULL);
	return true;
}
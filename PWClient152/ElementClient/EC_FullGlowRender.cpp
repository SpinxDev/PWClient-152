/*
 * FILE: EC_FullGlowRender.cpp
 *
 * DESCRIPTION: Fullscreen glow for the Element Client
 *
 * CREATED BY: Hedi, 2004/10/9 
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_FullGlowRender.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Viewport.h"
#include "EC_Configs.h"
#include "EC_GameRun.h"
#include "EC_World.h"

#include <A3DDevice.h>
#include <A3DEngine.h>
#include <A3DMacros.h>
#include <A3DFuncs.h>
#include <A3DShaderMan.h>
#include <A3DRenderTarget.h>
#include <A3DPixelShader.h>
#include <A3DStream.h>
#include <A3DViewport.h>
#include <A3DTexture.h>
#include <A3DTextureMan.h>
#include <AChar.h>
#include <A3DVertex.h>
#include <A3DTerrainWater.h>
#include <A3DCamera.h>

#include "A3DGfxExMan.h"
#include "A3DGFXElement.h"

#define new A_DEBUG_NEW

static float B1 = 0.107f;
static float B2 = 0.238f;
static float B3	= 0.310f;

static float r = 4.0f;

CECFullGlowRender::CECFullGlowRender()
{
	m_bRenderToBack		= false;

	m_bCanDoFullGlow	= false;
	m_bGlowOn			= false;
	m_bWarpOn			= false;
	m_bFlareOn			= false;
	m_bSharpenOn		= false;

	m_pBackTarget		= NULL;
	m_pGlowTarget1		= NULL;	
	m_pGlowTarget2		= NULL;	

	m_pGlow1Shader		= NULL;
	m_pGlow2Shader		= NULL;
	m_pMonoGlowShader	= NULL;

	m_pStreamStretchCopy= NULL;
	m_pStreamBlurX1		= NULL;
	m_pStreamBlurY2		= NULL;

	m_pBumpUnderWater	= NULL;

	m_glowType			= FULLGLOW_TYPE_NULL;
	m_color				= 0xffffffff;

	m_pTLWarpShader		= NULL;
	m_pTLWarpStream		= NULL;
	m_nMaxTLWarpVerts	= 0;
	m_nMaxTLWarpIndices	= 0;

	m_pWarpShader		= NULL;
	m_pWarpStream		= NULL;
	m_nMaxWarpVerts		= 0;
	m_nMaxWarpIndices	= 0;

	m_pSharpenShader	= NULL;
	m_pStreamSharpen	= NULL;

	m_pPresenter		= NULL;
}

CECFullGlowRender::~CECFullGlowRender()
{
	Release();
}

bool CECFullGlowRender::Init(A3DDevice * pA3DDevice)
{
	m_bCanDoFullGlow	= false;
	m_bGlowOn			= false;
	m_bWarpOn			= false;
	m_bFlareOn			= false;
	m_pA3DDevice		= pA3DDevice;

	m_pPresenter = new CECFullGlowPresenter();

	m_bResourceLoaded = false;
	if( !LoadResource() )
	{
		g_Log.Log("CECFullGlowRender::Init(), failed to create resource!");
		ReleaseResource();
	}

	m_bGlowOn			= m_bCanDoFullGlow;
	m_bWarpOn			= m_bCanDoFullGlow;
	m_bFlareOn			= m_bCanDoFullGlow;
	m_bSharpenOn		= false;//m_bCanDoFullGlow;
	
	return true;
}

bool CECFullGlowRender::Release()
{
	ReleaseResource();

	if( m_pPresenter )
	{
		delete m_pPresenter;
		m_pPresenter = NULL;
	}

	return true;
}

bool CECFullGlowRender::LoadResource()
{
	if( m_pA3DDevice->GetDevFormat().fmtTarget != A3DFMT_A8R8G8B8 )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), we need device target to be A8R8G8B8 format!");
		return false;
	}

	m_nWidth			= m_pA3DDevice->GetDevFormat().nWidth;
	m_nHeight			= m_pA3DDevice->GetDevFormat().nHeight;

	//r = r / 1024.0f * m_nWidth;
	//CalculateGaussian(r);

	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_nWidth;
	devFmt.nHeight		= m_nHeight;
	devFmt.fmtTarget	= A3DFMT_A8R8G8B8;
	devFmt.fmtDepth		= A3DFMT_D24X8;

	m_pBackTarget = new A3DRenderTarget();
	if( !m_pBackTarget->Init(m_pA3DDevice, devFmt, true, false) )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to create glow back target!");
		return false;
	}

	devFmt.nWidth		= m_nWidth / 4;
	devFmt.nHeight		= m_nHeight / 4;
	m_pGlowTarget1 = new A3DRenderTarget();
	if( !m_pGlowTarget1->Init(m_pA3DDevice, devFmt, true, false) )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to create glow render target1!");
		return false;
	}

	devFmt.nWidth		= m_nWidth / 4;
	devFmt.nHeight		= m_nHeight / 4;
	m_pGlowTarget2 = new A3DRenderTarget();
	if( !m_pGlowTarget2->Init(m_pA3DDevice, devFmt, true, false) )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to create glow render target2!");
		return false;
	}

	// now load mono rendering shader
	// now load pixel shader for render glow1
	m_pGlow1Shader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADER_DIR"ps\\fullglow1_14.txt", false);
	if( NULL == m_pGlow1Shader )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to load fullglow1.txt");
		return false;
	}
	// now load pixel shader for render glow2
	m_pGlow2Shader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADER_DIR"ps\\fullglow2_14.txt", false);
	if( NULL == m_pGlow2Shader )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to load fullglow2.txt");
		return false;
	}


	m_pMonoGlowShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADER_DIR"ps\\monofullglow_14.txt", false);
	if( NULL == m_pMonoGlowShader )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to load monofullglow.txt");
		return false;
	}

	// now create streams
	m_pStreamStretchCopy = new A3DStream();
	if( !m_pStreamStretchCopy->Init(m_pA3DDevice, A3DVT_TLVERTEX, 4, 6, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR) )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to create stretch copy stream!");
		return false;
	}

	m_pStreamBlurX1 = new A3DStream();
	if( !m_pStreamBlurX1->Init(m_pA3DDevice, sizeof(A3DGLOWVERTEX), A3DGLOWVERT_FVF, 4, 6, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR) )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to create blur x stream!");
		return false;
	}

	m_pStreamBlurY2 = new A3DStream();
	if( !m_pStreamBlurY2->Init(m_pA3DDevice, sizeof(A3DGLOWVERTEX), A3DGLOWVERT_FVF, 4, 6, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR) )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to create blur y stream!");
		return false;
	}

	m_indices[0] = 0; m_indices[1] = 1; m_indices[2] = 2;
	m_indices[3] = 2; m_indices[4] = 1; m_indices[5] = 3;

	int w = m_nWidth / 4;
	int h = m_nHeight / 4;

	m_vertsStretchCopy[0] = A3DTLVERTEX(A3DVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f), 0xffffffff, 0xff000000, 0.0f, 0.0f);
	m_vertsStretchCopy[1] = A3DTLVERTEX(A3DVECTOR4((float)w - 0.5f, -0.5f, 0.0f, 1.0f), 0xffffffff, 0xff000000, 1.0f, 0.0f);
	m_vertsStretchCopy[2] = A3DTLVERTEX(A3DVECTOR4(-0.5f, (float)h - 0.5f, 0.0f, 1.0f), 0xffffffff, 0xff000000, 0.0f, 1.0f);
	m_vertsStretchCopy[3] = A3DTLVERTEX(A3DVECTOR4((float)w - 0.5f, (float)h - 0.5f, 0.0f, 1.0f), 0xffffffff, 0xff000000, 1.0f, 1.0f);
	m_pStreamStretchCopy->SetVerts((BYTE *) m_vertsStretchCopy, 4);
	m_pStreamStretchCopy->SetVertexRef((BYTE *) m_vertsStretchCopy);
	m_pStreamStretchCopy->SetIndices((BYTE*) m_indices, 6);
	m_pStreamStretchCopy->SetIndexRef((void *) m_indices);

	float d;
	d = r / m_nWidth;
	m_vertsBlurX1[0] = A3DGLOWVERTEX(A3DVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f), m_color, d * 2, 0.0f, d, 0.0f, -d, 0.0f, -d * 2, 0.0f, 0.0f, 0.0f);
	m_vertsBlurX1[1] = A3DGLOWVERTEX(A3DVECTOR4((float)w - 0.5f, -0.5f, 0.0f, 1.0f), m_color, 1 + d * 2, 0.0f, 1 + d, 0.0f, 1 - d, 0.0f, 1 - d * 2, 0.0f, 1.0f, 0.0f);
	m_vertsBlurX1[2] = A3DGLOWVERTEX(A3DVECTOR4(-0.5f, (float)h - 0.5f, 0.0f, 1.0f), m_color, d * 2, 1.0f, d, 1.0f, -d, 1.0f, -d * 2, 1.0f, 0.0f, 1.0f);
	m_vertsBlurX1[3] = A3DGLOWVERTEX(A3DVECTOR4((float)w - 0.5f, (float)h - 0.5f, 0.0f, 1.0f), m_color, 1 + d * 2, 1.0f, 1 + d, 1.0f, 1 - d, 1.0f, 1 - d * 2, 1.0f, 1.0f, 1.0f);
	m_pStreamBlurX1->SetVerts((BYTE *) m_vertsBlurX1, 4);
	m_pStreamBlurX1->SetVertexRef((BYTE *) m_vertsBlurX1);
	m_pStreamBlurX1->SetIndices((BYTE*) m_indices, 6);
	m_pStreamBlurX1->SetIndexRef((void *) m_indices);

	w = m_nWidth;
	h = m_nHeight;
	d = r / m_nHeight;
	m_vertsBlurY2[0] = A3DGLOWVERTEX(A3DVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f), m_color, 0.0f, d * 2, 0.0f, d, 0.0f, -d, 0.0f, -d * 2, 0.0f, 0.0f);
	m_vertsBlurY2[1] = A3DGLOWVERTEX(A3DVECTOR4((float)w - 0.5f, -0.5f, 0.0f, 1.0f), m_color, 1.0f, d * 2, 1.0f, d, 1.0f, -d, 1.0f, -d * 2, 1.0f, 0.0f);
	m_vertsBlurY2[2] = A3DGLOWVERTEX(A3DVECTOR4(-0.5f, (float)h - 0.5f, 0.0f, 1.0f), m_color, 0.0f, 1 + d * 2, 0.0f, 1 + d, 0.0f, 1 - d, 0.0f, 1 - d * 2, 0.0f, 1.0f);
	m_vertsBlurY2[3] = A3DGLOWVERTEX(A3DVECTOR4((float)w - 0.5f, (float)h - 0.5f, 0.0f, 1.0f), m_color, 1.0f, 1 + d * 2, 1.0f, 1 + d, 1.0f, 1 - d, 1.0f, 1 - d * 2, 1.0f, 1.0f);
	m_pStreamBlurY2->SetVerts((BYTE *) m_vertsBlurY2, 4);
	m_pStreamBlurY2->SetVertexRef((BYTE *) m_vertsBlurY2);
	m_pStreamBlurY2->SetIndices((BYTE*) m_indices, 6);
	m_pStreamBlurY2->SetIndexRef((void *) m_indices);

	// now load resource for space warps.
	m_nMaxWarpVerts = 1000;
	m_nMaxWarpIndices = 1500;
	m_pWarpStream = new A3DStream();
	if( !m_pWarpStream->Init(m_pA3DDevice, sizeof(A3DWARPVERTEX), A3DFVF_A3DWARPVERT, m_nMaxWarpVerts, m_nMaxWarpIndices, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC) )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to create warp stream!");
		return false;
	}

	WORD * pIndices;
	if( !m_pWarpStream->LockIndexBuffer(0, 0, (BYTE **)&pIndices, 0) )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to lock warp stream's index buffer!");
		return false;
	}
	int i(0);
	for(i=0; i<250; i++)
	{
		pIndices[i * 6 + 0] = i * 4 + 0;		
		pIndices[i * 6 + 1] = i * 4 + 1;		
		pIndices[i * 6 + 2] = i * 4 + 2;		
		pIndices[i * 6 + 3] = i * 4 + 2;		
		pIndices[i * 6 + 4] = i * 4 + 1;		
		pIndices[i * 6 + 5] = i * 4 + 3;		
	}
	m_pWarpStream->UnlockIndexBuffer();
	
	m_pWarpShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADER_DIR"ps\\spacewarp.txt", false);
	if( NULL == m_pWarpShader )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to load spacewarp.txt");
		return false;
	}

	// now load resource for TL space warps.
	m_nMaxTLWarpVerts = 1000;
	m_nMaxTLWarpIndices = 1500;
	m_pTLWarpStream = new A3DStream();
	if( !m_pTLWarpStream->Init(m_pA3DDevice, sizeof(A3DTLWARPVERTEX), A3DFVF_A3DTLWARPVERT, m_nMaxTLWarpVerts, m_nMaxTLWarpIndices, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC) )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to create blur y stream!");
		return false;
	}

	if( !m_pTLWarpStream->LockIndexBuffer(0, 0, (BYTE **)&pIndices, 0) )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to lock warp stream's index buffer!");
		return false;
	}
	for(i=0; i<250; i++)
	{
		pIndices[i * 6 + 0] = i * 4 + 0;		
		pIndices[i * 6 + 1] = i * 4 + 1;		
		pIndices[i * 6 + 2] = i * 4 + 2;		
		pIndices[i * 6 + 3] = i * 4 + 2;		
		pIndices[i * 6 + 4] = i * 4 + 1;		
		pIndices[i * 6 + 5] = i * 4 + 3;		
	}
	m_pTLWarpStream->UnlockIndexBuffer();
	
	m_pTLWarpShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADER_DIR"ps\\spacewarp_tl.txt", false);
	if( NULL == m_pTLWarpShader )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to load spacewarp_tl.txt");
		return false;
	}

	if( !CreateBumpMap() )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to create bump map for under water");
		return false;
	}

	// now load resource for sharpen
	m_pSharpenShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADER_DIR"ps\\fullsharpen_14.txt", false);
	if( NULL == m_pSharpenShader )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to load fullsharpen_14.txt");
		return false;
	}

	m_pStreamSharpen = new A3DStream();
	if( !m_pStreamSharpen->Init(m_pA3DDevice, sizeof(A3DGLOWVERTEX), A3DGLOWVERT_FVF, 4, 6, A3DSTRM_STATIC, A3DSTRM_STATIC) )
	{
		g_Log.Log("CECFullGlowRender::LoadResource(), failed to create sharpen stream!");
		return false;
	}

	float du = 1.0f / m_nWidth;
	float dv = 1.0f / m_nHeight;
	A3DGLOWVERTEX sharpenVerts[4];
	sharpenVerts[0] = A3DGLOWVERTEX(A3DVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f), 0xffffffff, 0.0f, 0.0f, -du, 0.0f, du, 0.0f, 0.0f, -dv, 0.0f, dv);
	sharpenVerts[1] = A3DGLOWVERTEX(A3DVECTOR4((float)m_nWidth - 0.5f, -0.5f, 0.0f, 1.0f), 0xffffffff, 1.0f, 0.0f, 1.0f-du, 0.0f, 1.0f+du, 0.0f, 1.0f, -dv, 1.0f, dv);
	sharpenVerts[2] = A3DGLOWVERTEX(A3DVECTOR4(-0.5f, (float)m_nHeight - 0.5f, 0.0f, 1.0f), 0xffffffff, 0.0f, 1.0f, -du, 1.0f, du, 1.0f, 0.0f, 1.0f-dv, 0.0f, 1.0f+dv);
	sharpenVerts[3] = A3DGLOWVERTEX(A3DVECTOR4((float)m_nWidth - 0.5f, (float)m_nHeight - 0.5f, 0.0f, 1.0f), 0xffffffff, 1.0f, 1.0f, 1.0f-du, 1.0f, 1.0f+du, 1.0f, 1.0f, 1.0f-dv, 1.0f, 1.0f+dv);
	m_pStreamSharpen->SetVerts((BYTE *) sharpenVerts, 4);
	m_pStreamSharpen->SetIndices((BYTE*) m_indices, 6);

	m_pPresenter->SetData(m_pGlow1Shader, m_pGlow2Shader, m_pMonoGlowShader);

	m_bCanDoFullGlow	= true;
	m_bResourceLoaded	= true;
	return true;
}

bool CECFullGlowRender::ReleaseResource()
{
	if( m_pPresenter )
		m_pPresenter->SetData(NULL, NULL, NULL);

	if( m_pStreamStretchCopy )
	{
		m_pStreamStretchCopy->Release();
		delete m_pStreamStretchCopy;
		m_pStreamStretchCopy = NULL;
	}

	if( m_pStreamBlurX1 )
	{
		m_pStreamBlurX1->Release();
		delete m_pStreamBlurX1;
		m_pStreamBlurX1 = NULL;
	}

	if( m_pStreamBlurY2 )
	{
		m_pStreamBlurY2->Release();
		delete m_pStreamBlurY2;
		m_pStreamBlurY2 = NULL;
	}

	if( m_pGlowTarget1 )
	{
		m_pGlowTarget1->Release();
		delete m_pGlowTarget1;
		m_pGlowTarget1 = NULL;
	}

	if( m_pGlowTarget2 )
	{
		m_pGlowTarget2->Release();
		delete m_pGlowTarget2;
		m_pGlowTarget2 = NULL;
	}

	if( m_pBackTarget )
	{
		m_pBackTarget->Release();
		delete m_pBackTarget;
		m_pBackTarget = NULL;
	}

	if( m_pGlow1Shader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pGlow1Shader);
		m_pGlow1Shader = NULL;
	}

	if( m_pGlow2Shader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pGlow2Shader);
		m_pGlow2Shader = NULL;
	}

	if( m_pMonoGlowShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pMonoGlowShader);
		m_pMonoGlowShader = NULL;
	}

	if( m_pWarpShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pWarpShader);
		m_pWarpShader = NULL;
	}

	if( m_pWarpStream )
	{
		m_pWarpStream->Release();
		delete m_pWarpStream;
		m_pWarpStream = NULL;
	}

	if( m_pTLWarpShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pTLWarpShader);
		m_pTLWarpShader = NULL;
	}

	if( m_pTLWarpStream )
	{
		m_pTLWarpStream->Release();
		delete m_pTLWarpStream;
		m_pTLWarpStream = NULL;
	}

	if( m_pBumpUnderWater )
	{
		m_pBumpUnderWater->Release();
		delete m_pBumpUnderWater;
		m_pBumpUnderWater = NULL;
	}

	if( m_pSharpenShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pSharpenShader);
		m_pSharpenShader = NULL;
	}

	if( m_pStreamSharpen )
	{
		m_pStreamSharpen->Release();
		delete m_pStreamSharpen;
		m_pStreamSharpen = NULL;
	}

	m_bResourceLoaded = false;
	return true;
}

bool CECFullGlowRender::BeginGlow()
{
	if( !m_bCanDoFullGlow || !m_bResourceLoaded )
		return true;

	if( m_pA3DDevice->GetDevFormat().fmtTarget != A3DFMT_A8R8G8B8 )
		m_bRenderToBack = false;
	else
		m_bRenderToBack = true;
	
	return true;
}

void CECFullGlowRender::CalculateGaussian(double halfsize)
{
	double	size = halfsize * 3.0f;
	double	sigma2;
    double	l;
    int		length, n;

    sigma2 = -(size + 1) * (size + 1) / log(1.0 / 255.0);
    l = size;

    n = int(ceil(l) * 2.0);
    if( (n % 2) == 0 ) n++;
	length = n / 2;

	B3 = 1.0f;
	B2 = (float)exp(-(halfsize * halfsize) / sigma2);
	B1 = (float)exp(-(halfsize * halfsize * 4) / sigma2);
    
	float sum = B1 + B1 + B2 + B2 + B3;
	float rh = 1.0f / sum;
	B3 *= rh;
	B2 *= rh;
	B1 *= rh;
}

bool CECFullGlowRender::Update(int nDeltaTime)
{
	if( !m_bGlowOn || !m_bCanDoFullGlow )
		return true;

	m_pPresenter->Update(nDeltaTime);

	return true;
}

bool CECFullGlowRender::EndGlow(A3DViewport * pViewport)
{
	if( !m_bCanDoFullGlow || !m_bResourceLoaded || !m_bRenderToBack )
		return true;

	// move content from back buffer to glow target;
#ifdef ANGELICA_2_2
	g_pGame->GetA3DDevice()->GetD3DDevice()->StretchRect(g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, m_pBackTarget->GetTargetSurface(), NULL, D3DTEXF_POINT);
#else
	g_pGame->GetA3DDevice()->GetD3DDevice()->CopyRects(g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, 0, m_pBackTarget->GetTargetSurface(), NULL);
#endif // ANGELICA_2_2

	if( m_bWarpOn )
	{
		DoWarps(pViewport);

		// then move content from back to glow target again;
#ifdef ANGELICA_2_2
		g_pGame->GetA3DDevice()->GetD3DDevice()->StretchRect(g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, m_pBackTarget->GetTargetSurface(), NULL, D3DTEXF_POINT);
#else
		g_pGame->GetA3DDevice()->GetD3DDevice()->CopyRects(g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, 0, m_pBackTarget->GetTargetSurface(), NULL);
#endif // ANGELICA_2_2
	}

	// glow and sharpen will not work together
	if( m_bGlowOn )
	{
		DoGlow();
	}

	if( m_bSharpenOn )
	{
		if( m_bGlowOn )
		{
#ifdef ANGELICA_2_2
			g_pGame->GetA3DDevice()->GetD3DDevice()->StretchRect(g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, m_pBackTarget->GetTargetSurface(), NULL, D3DTEXF_POINT);
#else
			g_pGame->GetA3DDevice()->GetD3DDevice()->CopyRects(g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, 0, m_pBackTarget->GetTargetSurface(), NULL);
#endif // ANGELICA_2_2
		}

		DoSharpen();
	}

	return true;

}

bool CECFullGlowRender::DoGlow()
{
	// first move back target's content to small target, this will do a nature blur
	m_pA3DDevice->SetRenderTarget(m_pGlowTarget1);
	A3DVIEWPORTPARAM param;
	param.X = 0;
	param.Y = 0;
	param.Width = m_nWidth / 4;
	param.Height = m_nHeight / 4;
	param.MinZ = 0.0f;
	param.MaxZ = 1.0f;
	m_pA3DDevice->SetViewport(&param);

	m_pA3DDevice->GetD3DDevice()->SetTexture(0, m_pBackTarget->GetTargetTexture());

	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);

	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_ZERO);
	m_pStreamStretchCopy->Appear();
	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2);
	m_pA3DDevice->ClearTexture(0);

	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_POINT);

	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(3, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(4, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(5, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	m_pA3DDevice->SetTextureTransformFlags(5, A3DTTFF_DISABLE);

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_ZERO);

	// then move screen content onto blur target by bluring it
	m_pA3DDevice->SetRenderTarget(m_pGlowTarget2);
	
	m_pA3DDevice->GetD3DDevice()->SetTexture(0, m_pGlowTarget1->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(1, m_pGlowTarget1->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(2, m_pGlowTarget1->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(3, m_pGlowTarget1->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(4, m_pGlowTarget1->GetTargetTexture());
	
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_ZERO);
	m_pPresenter->PrepareGlow1Pass(B1, B2, B3);
	m_pStreamBlurX1->Appear();
	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2);
	m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->ClearTexture(1);
	m_pA3DDevice->ClearTexture(2);
	m_pA3DDevice->ClearTexture(3);
	m_pA3DDevice->ClearTexture(4);
	m_pPresenter->AfterGlow1Pass();
	m_pA3DDevice->RestoreRenderTarget();
	g_pGame->GetViewport()->GetA3DViewport()->Active();
	
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pPresenter->PrepareGlow2Pass(m_color, B1, B2, B3);
	m_pA3DDevice->GetD3DDevice()->SetTexture(0, m_pGlowTarget2->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(1, m_pGlowTarget2->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(2, m_pGlowTarget2->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(3, m_pGlowTarget2->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(4, m_pGlowTarget2->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(5, m_pBackTarget->GetTargetTexture());
	m_pStreamBlurY2->Appear();
	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2);
	m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->ClearTexture(1);
	m_pA3DDevice->ClearTexture(2);
	m_pA3DDevice->ClearTexture(3);
	m_pA3DDevice->ClearTexture(4);
	m_pA3DDevice->ClearTexture(5);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(4, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(5, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
	m_pPresenter->AfterGlow2Pass();	
	
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFogEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	return true;
}

bool CECFullGlowRender::DoWarps(A3DViewport * pViewport)
{
	m_pA3DDevice->GetD3DDevice()->SetTexture(1, m_pBackTarget->GetTargetTexture());
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	A3DCULLTYPE oldCull = m_pA3DDevice->GetFaceCull();
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

	A3DGFXExMan * pGfxMan = g_pGame->GetA3DGFXExMan();
	GfxElementList& elementList = pGfxMan->GetWarpEleList();
	pGfxMan->LockWarpEleList();

	// first TL space warps.
	m_pTLWarpStream->Appear();
	m_pTLWarpShader->Appear();
	
	bool bUnderWater = false;
	CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
	if( pWorld && pWorld->GetTerrainWater() )
	{
		bUnderWater = pWorld->GetTerrainWater()->IsUnderWater(g_pGame->GetViewport()->GetA3DCamera()->GetPos());
	}

	if( bUnderWater )
	{
		A3DTLWARPVERTEX * pVerts;
		if( !m_pTLWarpStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, 0) )
		{
			return false;
		}

		float w = m_nWidth * 1.0f;
		float h = m_nHeight * 1.0f;

		float	xt = 3;
		float	yt = m_nHeight * xt / m_nWidth;
		static float yd = 0.0f;
		yd += g_pGame->GetTickTime() * 0.001f * 0.5f;
		if( yd > 1.0f ) yd -= 1.0f;

		float	xw = 0.005f;
		float	yw = m_nHeight * xw / m_nHeight;
		pVerts[0] = A3DTLWARPVERTEX(A3DVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, yd, 0.0f, 0.0f, xw, yw);
		pVerts[1] = A3DTLWARPVERTEX(A3DVECTOR4(w, 0.0f, 0.0f, 1.0f), xt, yd, 1.0f, 0.0f, xw, yw);
		pVerts[2] = A3DTLWARPVERTEX(A3DVECTOR4(0.0f, h, 0.0f, 1.0f), 0.0f, yt + yd, 0.0f, 1.0f, xw, yw);
		pVerts[3] = A3DTLWARPVERTEX(A3DVECTOR4(w, h, 0.0f, 1.0f), xt, yt + yd, 1.0f, 1.0f, xw, yw);

		m_pTLWarpStream->UnlockVertexBuffer();

		m_pBumpUnderWater->Appear(0);
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2);
		m_pBumpUnderWater->Disappear(0);

#ifdef ANGELICA_2_2
		g_pGame->GetA3DDevice()->GetD3DDevice()->StretchRect(g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, m_pBackTarget->GetTargetSurface(), NULL, D3DTEXF_POINT);
#else
		g_pGame->GetA3DDevice()->GetD3DDevice()->CopyRects(g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, 0, m_pBackTarget->GetTargetSurface(), NULL);
#endif // ANGELICA_2_2
	}

	int		i;
	float	rw = 1.0f / m_nWidth;
	float	rh = 1.0f / m_nHeight;

	for(i=0; i<(int)elementList.size(); i++)
	{
		A3DGFXElement * pElement = elementList[i];
		if( pElement && pElement->IsTLVert() )
		{
			A3DTLWARPVERTEX * pVerts;
			if( !m_pTLWarpStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, 0) )
			{
				break;
			}
			int nVerts = pElement->DrawToBuffer(pViewport, pVerts, m_nMaxTLWarpVerts, rw, rh);
			m_pTLWarpStream->UnlockVertexBuffer();

			pElement->GetTexture()->Appear(0);
			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nVerts, 0, nVerts >> 1);
			pElement->GetTexture()->Disappear(0);
		}
	}
	
	m_pTLWarpShader->Disappear();

	A3DCameraBase * pCamera = pViewport->GetCamera();

	// then do 3d warps.
	m_pWarpStream->Appear();
	m_pWarpShader->Appear();

	
	float scaleY = pViewport->GetParam()->Height / (float)m_nHeight;	//	缩放Y轴以处理宽屏时“空间扭曲”特效显示为黑色的问题
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	A3DMATRIX4 matScale = IdentityMatrix();
	matScale._11 = 0.5f;
	matScale._22 = -0.5f * scaleY ;
	matScale._41 = 0.5f;
	matScale._42 = 0.5f;
	A3DMATRIX4 matProjectedView;
	matProjectedView = pCamera->GetProjectionTM() * pCamera->GetPostProjectTM() * matScale;
	m_pA3DDevice->SetTextureCoordIndex(1, D3DTSS_TCI_CAMERASPACEPOSITION);
	m_pA3DDevice->SetTextureTransformFlags(1, (A3DTEXTURETRANSFLAGS)(A3DTTFF_COUNT4 | A3DTTFF_PROJECTED));
	m_pA3DDevice->SetTextureMatrix(1, matProjectedView);
	
	for(i=0; i<(int)elementList.size(); i++)
	{
		A3DGFXElement * pElement = elementList[i];
		if( pElement && !pElement->IsTLVert() )
		{
			A3DWARPVERTEX * pVerts;
			if( !m_pWarpStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, 0) )
			{
				break;
			}
			int nVerts = pElement->DrawToBuffer(pViewport, pVerts, m_nMaxWarpVerts);
			m_pWarpStream->UnlockVertexBuffer();

			pElement->GetTexture()->Appear(0);
			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nVerts, 0, nVerts >> 1);
			pElement->GetTexture()->Disappear(0);
		}
	}

	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);

	m_pWarpShader->Disappear();
	
	pGfxMan->UnlockWarpEleList();
	pGfxMan->ClearWarpEleList();

	// restore render states.
	m_pA3DDevice->ClearTexture(1);
	m_pA3DDevice->SetFaceCull(oldCull);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFogEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
	return true;
}

bool CECFullGlowRender::DoSharpen()
{
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);

	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
	
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(3, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(4, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	
	m_pA3DDevice->GetD3DDevice()->SetTexture(0, m_pBackTarget->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(1, m_pBackTarget->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(2, m_pBackTarget->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(3, m_pBackTarget->GetTargetTexture());
	m_pA3DDevice->GetD3DDevice()->SetTexture(4, m_pBackTarget->GetTargetTexture());
	
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_ZERO);
	m_pStreamSharpen->Appear();
	m_pSharpenShader->Appear();

	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2);

	m_pSharpenShader->Disappear();
	m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->ClearTexture(1);
	m_pA3DDevice->ClearTexture(2);
	m_pA3DDevice->ClearTexture(3);
	m_pA3DDevice->ClearTexture(4);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(4, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFogEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	return true;
}

bool CECFullGlowRender::SetGlowType(FULLGLOW_TYPE type, A3DCOLOR color)
{
	if( !m_bGlowOn || !m_bCanDoFullGlow )
		return true;

	if( m_glowType == type && m_color == color )
		return true;

	m_glowType = type;
	m_color = color;

	switch(m_glowType)
	{
	case FULLGLOW_TYPE_DAYLIGHT:
		m_pPresenter->StopFade();
		m_pPresenter->StopMonoRender();
		m_pPresenter->SetGlowLevel(-1.0f, 0.25f, 5000);
		m_pPresenter->SetGlowPower(-1.0f, 0.25f, 5000);
		break;

	case FULLGLOW_TYPE_NIGHT:
		m_pPresenter->StopFade();
		m_pPresenter->StopMonoRender();
		m_pPresenter->SetGlowLevel(-1.0f, 0.35f, 5000);
		m_pPresenter->SetGlowPower(-1.0f, 0.35f, 5000);
		break;

	case FULLGLOW_TYPE_UNDERWATER:
		m_pPresenter->StopFade();
		m_pPresenter->SetMonoRender(0.0f, 0.5f, 1500, 0xffBBEABB);
		m_pPresenter->SetGlowLevel(-1.0f, 0.6f, 2000);
		m_pPresenter->SetGlowPower(-1.0f, 0.5f, 2000);
		break;

	case FULLGLOW_TYPE_DEAD:
		m_pPresenter->SetMonoRender(0.0f, 1.0f, 2000, 0xFFE2FFFF);
		m_pPresenter->SetGlowLevel(-1.0f, 0.7f, 2000);
		m_pPresenter->SetGlowPower(-1.0f, 1.0f, 2000);
		break;

	case FULLGLOW_TYPE_LOGIN:
		m_pPresenter->StopFade();
		m_pPresenter->StopMonoRender();
		m_pPresenter->SetGlowLevel(-1.0f, 0.25f, 1000);
		m_pPresenter->SetGlowPower(-1.0f, 0.1f, 1000);
		break;
	}

	return true;
}

void CECFullGlowRender::SetFade(float vStart, float vEnd, int nTime)
{
	if( !m_bGlowOn || !m_bCanDoFullGlow )
		return;

	m_pPresenter->SetFade(vStart, vEnd, nTime);
}

void CECFullGlowRender::SetMotionBlur(float vStart, float vEnd, int nTime)
{
	// currently we render to device back buffer directly, so motion blur can not be achieved without extra processing
	return;

	if( !m_bGlowOn || !m_bCanDoFullGlow )
		return;

	m_pPresenter->SetMotionBlur(vStart, vEnd, nTime);
}

void CECFullGlowRender::SetMonoRender(float vStart, float vEnd, int nTime, A3DCOLOR monoHue)
{
	if( !m_bGlowOn || !m_bCanDoFullGlow )
		return;

	m_pPresenter->SetMonoRender(vStart, vEnd, nTime, monoHue);
}

bool CECFullGlowRender::SetGlowOn(bool bFlag)
{
	if( !m_bCanDoFullGlow )
		return true;

	if( m_bGlowOn == bFlag )
		return true;

	m_bGlowOn = bFlag;

	if( m_bGlowOn )
	{
		if( !m_bResourceLoaded && !LoadResource() )
		{
			ReleaseResource();
			m_bGlowOn = false;
			return false;
		}
	}
	else
	{
		if( m_bResourceLoaded && !GetNeedResource() )
			ReleaseResource();
	}

	return true;
}

bool CECFullGlowRender::SetWarpOn(bool bFlag)
{
	if( !m_bCanDoFullGlow )
		return true;

	if( m_bWarpOn == bFlag )
		return true;

	m_bWarpOn = bFlag;

	if( m_bWarpOn )
	{
		if( !m_bResourceLoaded && !LoadResource() )
		{
			ReleaseResource();
			m_bWarpOn = false;
			return false;
		}
	}
	else
	{
		if( m_bResourceLoaded && !GetNeedResource() )
			ReleaseResource();
	}

	return true;
}

bool CECFullGlowRender::SetFlareOn(bool bFlag)
{
	if( !m_bCanDoFullGlow )
		return true;

	if( m_bFlareOn == bFlag )
		return true;

	m_bFlareOn = bFlag;

	if( m_bFlareOn )
	{
		if( !m_bResourceLoaded && !LoadResource() )
		{
			ReleaseResource();
			m_bFlareOn = false;
			return false;
		}
	}
	else
	{
		if( m_bResourceLoaded && !GetNeedResource() )
			ReleaseResource();
	}

	return true;
}

bool CECFullGlowRender::SetSharpenOn(bool bFlag)
{
	if( !m_bCanDoFullGlow )
		return true;

	if( m_bSharpenOn == bFlag )
		return true;

	m_bSharpenOn = bFlag;

	if( m_bSharpenOn )
	{
		if( !m_bResourceLoaded && !LoadResource() )
		{
			ReleaseResource();
			m_bSharpenOn = false;
			return false;
		}
	}
	else
	{
		if( m_bResourceLoaded && !GetNeedResource() )
			ReleaseResource();
	}

	return true;
}

bool CECFullGlowRender::ReloadResource()
{
	if( m_bResourceLoaded )
	{
		ReleaseResource();
		if( !LoadResource() )
		{
			m_bGlowOn = false;
			m_bWarpOn = false;
			m_bFlareOn = false;
			m_bRenderToBack = false;
		}
	}
	
	return true;
}

bool CECFullGlowRender::CreateBumpMap()
{
	m_pBumpUnderWater = new A3DTexture;
	if( !m_pBumpUnderWater->Create(g_pGame->GetA3DDevice(), 256, 256, A3DFMT_A8R8G8B8, 1) )
		return false;

    // Fill the bumpmap texels to simulate a lens
	BYTE *	pBits;
	int		iPitch;
	if( !m_pBumpUnderWater->LockRect(NULL, (void **)&pBits, &iPitch, 0) )
		return false;

	DWORD * pDst = (DWORD*)pBits;

    for(int y=0; y<256; y++)
    {
		pDst = (DWORD *)(pBits + y * iPitch);
        for(int x=0; x<256; x++)
        {
            FLOAT fx = x * (1.0f / 256) - 0.5f;
            FLOAT fy = y * (1.0f / 256) - 0.5f;
            FLOAT r  = sqrtf(fx*fx + fy*fy);

            int iDu = (int)(32*cosf(4.0f*(fx+fy)*A3D_PI)) + 128;
            int iDv = (int)(32*sinf(4.0f*(fx+fy)*A3D_PI)) + 128;

            pDst[x] = A3DCOLORRGBA(iDu, iDv, 0, 0xff);
        }
    }

    m_pBumpUnderWater->UnlockRect();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// class CECFullGlowPresenter 
//////////////////////////////////////////////////////////////////////////////////////
CECFullGlowPresenter::CECFullGlowPresenter()
{
	Reset();
}

void CECFullGlowPresenter::SetData(A3DPixelShader * pGlow1Shader, A3DPixelShader * pGlow2Shader, A3DPixelShader * pMonoGlowShader)
{
	m_pGlow1Shader		= pGlow1Shader;
	m_pGlow2Shader		= pGlow2Shader;
	m_pMonoGlowShader	= pMonoGlowShader;
}
	
void CECFullGlowPresenter::Reset()
{
	m_vGlowPower			= 0.0f;
	m_vGlowPowerSet			= 0.0f;

	m_vGlowLevel			= 0.0f;
	m_vGlowLevelSet			= 0.0f;

	m_vFadeLevel			= 0.0f;
	m_vFadeLevelSet			= 0.0f;

	m_vMotionBlurLevel		= 0.0f;
	m_vMotionBlurLevelSet	= 0.0f;

	m_vMonoRenderLevel		= 0.0f;
	m_vMonoRenderLevelSet	= 0.0f;

	m_nGlowPowerTime		= 0;
	m_nGlowLevelTime		= 0;
	m_nFadeTime				= 0;
	m_nMotionBlurTime		= 0;
	m_nMonoRenderTime		= 0;
	m_MonoHue				= 0xffffffff;
}

void CECFullGlowPresenter::SetMotionBlur(float vStart, float vEnd, int nTime)
{
	m_vMotionBlurLevel		= vStart;
	m_vMotionBlurLevelSet	= vEnd;
	m_vMotionBlurStep		= (vEnd - vStart) / nTime;
	m_nMotionBlurTime		= nTime;
}

void CECFullGlowPresenter::SetFade(float vStart, float vEnd, int nTime)
{
	m_vFadeLevel			= vStart;
	m_vFadeLevelSet			= vEnd;
	m_vFadeStep				= (vEnd - vStart) / nTime;
	m_nFadeTime				= nTime;
}

void CECFullGlowPresenter::SetMonoRender(float vStart, float vEnd, int nTime, A3DCOLOR monoHue)
{
	m_vMonoRenderLevel		= vStart;
	m_vMonoRenderLevelSet	= vEnd;
	m_vMonoRenderStep		= (vEnd - vStart) / nTime;
	m_nMonoRenderTime		= nTime;
	m_MonoHue				= monoHue;
}

void CECFullGlowPresenter::SetGlowPower(float vStart, float vEnd, int nTime)
{
	if( vStart >= 0.0f )
		m_vGlowPower = vStart;
	else
		vStart = m_vGlowPower;

	m_vGlowPowerSet			= vEnd;
	m_vGlowPowerStep		= (vEnd - vStart) / nTime;
	m_nGlowPowerTime		= nTime;
}

void CECFullGlowPresenter::SetGlowLevel(float vStart, float vEnd, int nTime)
{
	if( vStart >= 0.0f )
		m_vGlowLevel = vStart;
	else
		vStart = m_vGlowLevel;

	m_vGlowLevelSet			= vEnd;
	m_vGlowLevelStep		= (vEnd - vStart) / nTime;
	m_nGlowLevelTime		= nTime;
}

void CECFullGlowPresenter::Update(int nDeltaTime)
{
	if( m_nMotionBlurTime )
	{
		m_nMotionBlurTime -= nDeltaTime;
		if( m_nMotionBlurTime > 0 )
			m_vMotionBlurLevel += m_vMotionBlurStep * nDeltaTime;
		else
		{
			m_vMotionBlurLevel = m_vMotionBlurLevelSet;
			m_nMotionBlurTime = 0;
		}
	}

	if( m_nFadeTime )
	{
		m_nFadeTime -= nDeltaTime;
		if( m_nFadeTime > 0 )
			m_vFadeLevel += m_vFadeStep * nDeltaTime;
		else
		{
			m_vFadeLevel = m_vFadeLevelSet;
			m_nFadeTime = 0;
		}
	}

	if( m_nMonoRenderTime )
	{
		m_nMonoRenderTime -= nDeltaTime;
		if( m_nMonoRenderTime > 0 )
			m_vMonoRenderLevel += m_vMonoRenderStep * nDeltaTime;
		else
		{
			m_vMonoRenderLevel = m_vMonoRenderLevelSet;
			m_nMonoRenderTime = 0;
		}
	}

	if( m_nGlowPowerTime )
	{
		m_nGlowPowerTime -= nDeltaTime;
		if( m_nGlowPowerTime > 0 )
			m_vGlowPower += m_vGlowPowerStep * nDeltaTime;
		else
		{
			m_vGlowPower = m_vGlowPowerSet;
			m_nGlowPowerTime = 0;
		}
	}

	if( m_nGlowLevelTime )
	{
		m_nGlowLevelTime -= nDeltaTime;
		if( m_nGlowLevelTime > 0 )
			m_vGlowLevel += m_vGlowLevelStep * nDeltaTime;
		else
		{
			m_vGlowLevel = m_vGlowLevelSet;
			m_nGlowLevelTime = 0;
		}
	}
}

void CECFullGlowPresenter::PrepareGlow1Pass(float vBlur0, float vBlur1, float vCenter)
{
	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();
	
	A3DCOLORVALUE blur(0.0f);
	blur.a = vBlur0;
	pA3DDevice->SetPixelShaderConstants(0, &blur, 1);
	blur.a = vBlur1;
	blur.r = vCenter;
	pA3DDevice->SetPixelShaderConstants(1, &blur, 1);
	m_pGlow1Shader->Appear();
}

void CECFullGlowPresenter::AfterGlow1Pass()
{
	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();
	pA3DDevice->ClearPixelShader();
	pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
}

void CECFullGlowPresenter::PrepareGlow2Pass(A3DCOLOR colorNow, float vBlur0, float vBlur1, float vCenter)
{
	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();
	A3DCOLORVALUE clNow = a3d_ColorRGBAToColorValue(colorNow);
	clNow.a = 1.0f;
	
	A3DCOLORVALUE blur(0.0f);
	blur.a = vBlur0;
	blur.r = m_vGlowLevel;
	blur.g = m_vGlowPower;
	pA3DDevice->SetPixelShaderConstants(0, &blur, 1);
	blur.a = vBlur1;
	blur.r = vCenter;
	pA3DDevice->SetPixelShaderConstants(1, &blur, 1);
	m_pGlow2Shader->Appear();

	if( m_vFadeLevel != 0.0f )
	{
		// this can affect the whole color, except the alpha channel
		clNow.r = clNow.r * (1.0f - m_vFadeLevel);
		clNow.g = clNow.g * (1.0f - m_vFadeLevel);
		clNow.b = clNow.b * (1.0f - m_vFadeLevel);
	}

	if( m_vMotionBlurLevel != 0.0f )
	{
		clNow.a = clNow.a * (1.0f - m_vMotionBlurLevel);
	}

	if( m_vMonoRenderLevel != 0.0f )
	{
		A3DCOLORVALUE mono = a3d_ColorRGBAToColorValue(m_MonoHue);
		mono.a = m_vMonoRenderLevel;
		pA3DDevice->SetPixelShaderConstants(2, &mono, 1);
		m_pMonoGlowShader->Appear();
	}

	pA3DDevice->SetPixelShaderConstants(3, &clNow, 1);
}

void CECFullGlowPresenter::AfterGlow2Pass()
{
	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();
	pA3DDevice->ClearPixelShader();
	pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
}

bool CECFullGlowPresenter::IsFading()
{
	if( m_vFadeLevel != 0.0f )
		return true;
	else
		return false;
}

bool CECFullGlowPresenter::IsMonoRendering()
{
	if( m_vMonoRenderLevel != 0.0f )
		return true;
	else
		return false;
}

bool CECFullGlowPresenter::IsMontionBluring()
{
	if( m_vMotionBlurLevel != 0.0f )
		return true;
	else
		return false;
}

void CECFullGlowPresenter::StopMotionBlur()
{
	SetMotionBlur(m_vMotionBlurLevel, 0.0f, 1000);
}

void CECFullGlowPresenter::StopFade(bool bToWhite)
{
	m_nFadeTime = 1000;

	if( bToWhite )
	{
		SetFade(m_vFadeLevel, 0.0f, 1000);
	}
	else
	{
		SetFade(m_vFadeLevel, 1.0f, 1000);
	}
}

void CECFullGlowPresenter::StopMonoRender()
{
	SetMonoRender(m_vMonoRenderLevel, 0.0f, 5000, m_MonoHue);
}

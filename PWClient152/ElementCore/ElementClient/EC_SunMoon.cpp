/*
 * FILE: EC_SunMoon.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2005/3/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_SunMoon.h"
#include "EC_Viewport.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_CDS.h"
#include "EC_FullGlowRender.h"

#include "A3DFuncs.h"
#include "A3DCamera.h"
#include "A3DTextureMan.h"
#include "A3DShaderMan.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "A3DRenderTarget.h"
#include "A3DPixelShader.h"
#include "A3DStream.h"
#include "A3DTexture.h"
				  
#define TIME_SCALE				(6.0f)

#define SKY_FAR					100.0f

CECSunMoon::CECSunMoon()
{
	m_pSunTexture		= NULL;
	m_pSunShineTexture	= NULL;
	m_pMoonTexture		= NULL;

	m_bCanDoFlare		= false;
	m_pFlareAlphaTarget	= NULL;
	m_pFlareShader		= NULL;
	m_pStreamFlareAlpha	= NULL;
	m_pFlaresTexture	= NULL;
	m_pStreamFlares		= NULL;

	m_pSunFlareTexture	= NULL;
	m_pMoonFlareTexture	= NULL;
	m_pStreamSun		= NULL;
	m_pStreamSunShine	= NULL;
	m_pStreamMoon		= NULL;
	m_pStreamSunPoint	= NULL;

	m_fDNFactor			= 0.0f;
	m_fDNFactorDest		= 0.0f;

	m_bTimeHasBeenSet	= false;
	m_bDirHasBeenSet	= false;
	m_bFirstAlpha		= true;

	m_bSunVisible		= true;
	m_bMoonVisible		= true;
	m_vSunVisibility	= true;
	m_vMoonVisibility	= true;
}

CECSunMoon::~CECSunMoon()
{
	Release();
}

bool CECSunMoon::Init()
{
	Release();

	m_vSunSize		= 1.0f;
	m_vMoonSize		= 1.0f;
	m_colorSun		= 0xffffffff;
	m_colorMoon		= 0xffffffff;
	m_vSunPitch		= 0.0f;
	m_vMoonPitch	= 0.0f;
	m_vDeg			= 0.0f;
	m_vTimeOfTheDay = 0.0f;
	m_vecLightDir	= A3DVECTOR3(-1.0f, 0.0f, 0.0f);

	A3DTextureMan * pTextureMan = g_pGame->GetA3DEngine()->GetA3DTextureMan();

	if( !pTextureMan->LoadTextureFromFile("textures\\nature\\sun.bmp", &m_pSunTexture) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to load sun texture");
		return false;
	}

	if( !pTextureMan->LoadTextureFromFile("textures\\nature\\sunshine.bmp", &m_pSunShineTexture) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to load sun shine texture");
		return false;
	}

	if( !pTextureMan->LoadTextureFromFile("textures\\nature\\moon.tga", &m_pMoonTexture) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to load moon texture");
		return false;
	}

	m_pStreamSun = new A3DStream();
	if( !m_pStreamSun->Init(g_pGame->GetA3DDevice(), A3DVT_LVERTEX, 6, 0, A3DSTRM_STATIC, 0) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to create sun stream");
		return false;
	}

	m_pStreamSunShine = new A3DStream();
	if( !m_pStreamSunShine->Init(g_pGame->GetA3DDevice(), A3DVT_LVERTEX, 8, 18, A3DSTRM_STATIC, A3DSTRM_STATIC) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to create sun shine stream");
		return false;
	}

	m_pStreamMoon = new A3DStream();
	if( !m_pStreamMoon->Init(g_pGame->GetA3DDevice(), A3DVT_LVERTEX, 6, 0, A3DSTRM_STATIC, 0) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to create moon stream");
		return false;
	}

	A3DLVERTEX * pVerts = NULL;
	if( !m_pStreamSun->LockVertexBuffer(0, 0, (BYTE **)&pVerts, NULL) )
	{
		a_LogOutput(1, "CECSunMoon::Init, failed to lock sun stream!");
		return false;
	}
	pVerts[0] = A3DLVERTEX(A3DVECTOR3(-SUN_UNIT_SIZE, 0.0f, 0.0f), 0xffffffff, 0xff000000, 0.0f, 0.0f);
	pVerts[1] = A3DLVERTEX(A3DVECTOR3(SUN_UNIT_SIZE, 0.0f, 0.0f), 0xffffffff, 0xff000000, 1.0f, 0.0f);
	pVerts[2] = A3DLVERTEX(A3DVECTOR3(-SUN_UNIT_SIZE, -2.0f * SUN_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 0.0f, 1.0f);

	pVerts[3] = pVerts[2];
	pVerts[4] = pVerts[1];
	pVerts[5] = A3DLVERTEX(A3DVECTOR3(SUN_UNIT_SIZE, -2.0f * SUN_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 1.0f, 1.0f);
	m_pStreamSun->UnlockVertexBuffer();

	if( !m_pStreamSunShine->LockVertexBuffer(0, 0, (BYTE **)&pVerts, NULL) )
	{
		a_LogOutput(1, "CECSunMoon::Init, failed to lock sun shine stream!");
		return false;
	}
	pVerts[0] = A3DLVERTEX(A3DVECTOR3(-SUN_UNIT_SIZE * 6.0f, SUN_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 0.0f, 0.0f);
	pVerts[1] = A3DLVERTEX(A3DVECTOR3(-SUN_UNIT_SIZE * 2.0f, SUN_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 1.0f / 3.0f, 0.0f);
	pVerts[2] = A3DLVERTEX(A3DVECTOR3(SUN_UNIT_SIZE * 2.0f, SUN_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 2.0f / 3.0f, 0.0f);
	pVerts[3] = A3DLVERTEX(A3DVECTOR3(SUN_UNIT_SIZE * 6.0f, SUN_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 1.0f, 0.0f);

	pVerts[4] = A3DLVERTEX(A3DVECTOR3(-SUN_UNIT_SIZE * 6.0f, -3.0f * SUN_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 0.0f, 1.0f);
	pVerts[5] = A3DLVERTEX(A3DVECTOR3(-SUN_UNIT_SIZE * 2.0f, -3.0f * SUN_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 1.0f / 3.0f, 1.0f);
	pVerts[6] = A3DLVERTEX(A3DVECTOR3(SUN_UNIT_SIZE * 2.0f, -3.0f * SUN_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 2.0f / 3.0f, 1.0f);
	pVerts[7] = A3DLVERTEX(A3DVECTOR3(SUN_UNIT_SIZE * 6.0f, -3.0f * SUN_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 1.0f, 1.0f);
	m_pStreamSunShine->UnlockVertexBuffer();

	WORD * pIndices = NULL;
	if( !m_pStreamSunShine->LockIndexBuffer(0, 0, (BYTE **)&pIndices, NULL) )
	{
		a_LogOutput(1, "CECSunMoon::Init, failed to lock sun shine stream!");
		return false;
	}
	for(int i=0; i<3; i++)
	{
		pIndices[i * 6 + 0] = i;
		pIndices[i * 6 + 1] = i + 1;
		pIndices[i * 6 + 2] = i + 4;

		pIndices[i * 6 + 3] = i + 4;
		pIndices[i * 6 + 4] = i + 1;
		pIndices[i * 6 + 5] = i + 5;
	}
	m_pStreamSunShine->UnlockIndexBuffer();

	if( !m_pStreamMoon->LockVertexBuffer(0, 0, (BYTE **)&pVerts, NULL) )
	{
		a_LogOutput(1, "CECSunMoon::Init, failed to lock moon stream!");
		return false;
	}
	pVerts[0] = A3DLVERTEX(A3DVECTOR3(-MOON_UNIT_SIZE, 0.0f, 0.0f), 0xffffffff, 0xff000000, 0.0f, 0.0f);
	pVerts[1] = A3DLVERTEX(A3DVECTOR3(MOON_UNIT_SIZE, 0.0f, 0.0f), 0xffffffff, 0xff000000, 1.0f, 0.0f);
	pVerts[2] = A3DLVERTEX(A3DVECTOR3(-MOON_UNIT_SIZE, -2.0f * MOON_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 0.0f, 1.0f);

	pVerts[3] = pVerts[2];
	pVerts[4] = pVerts[1];
	pVerts[5] = A3DLVERTEX(A3DVECTOR3(MOON_UNIT_SIZE, -2.0f * MOON_UNIT_SIZE, 0.0f), 0xffffffff, 0xff000000, 1.0f, 1.0f);
	m_pStreamMoon->UnlockVertexBuffer();
	
	m_bCanDoFlare = false;
	if( !InitFlare() )
	{
		ReleaseFlare();
	}
	m_bCanDoFlare = true;

	return true;
}

bool CECSunMoon::Release()
{
	A3DTextureMan * pTextureMan = g_pGame->GetA3DEngine()->GetA3DTextureMan();

	if( m_pSunTexture )
	{
		pTextureMan->ReleaseTexture(m_pSunTexture);
		m_pSunTexture = NULL;
	}

	if( m_pSunShineTexture )
	{
		pTextureMan->ReleaseTexture(m_pSunShineTexture);
		m_pSunShineTexture = NULL;
	}

	if( m_pMoonTexture )
	{
		pTextureMan->ReleaseTexture(m_pMoonTexture);
		m_pMoonTexture = NULL;
	}

	A3DRELEASE(m_pStreamSun);
	A3DRELEASE(m_pStreamSunShine);
	A3DRELEASE(m_pStreamMoon);
	
	ReleaseFlare();
	return true;
}

bool CECSunMoon::InitFlare()
{
	A3DTextureMan * pTextureMan = g_pGame->GetA3DEngine()->GetA3DTextureMan();

	if( !pTextureMan->LoadTextureFromFile("textures\\nature\\sunflare.bmp", &m_pSunFlareTexture) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to load moon texture");
		return false;
	}

	if( !pTextureMan->LoadTextureFromFile("textures\\nature\\moonflare.bmp", &m_pMoonFlareTexture) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to load moon texture");
		return false;
	}

	if( !pTextureMan->LoadTextureFromFile("textures\\nature\\flares.bmp", &m_pFlaresTexture) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to load flares texture");
		return false;
	}

	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= 1;
	devFmt.nHeight		= 1;
	devFmt.fmtTarget	= A3DFMT_A8R8G8B8;
	devFmt.fmtDepth		= A3DFMT_D24X8;

	m_pFlareAlphaTarget = new A3DRenderTarget();
	if( !m_pFlareAlphaTarget->Init(g_pGame->GetA3DDevice(), devFmt, true, false) )
	{
		a_LogOutput(1, "CECSunMoon::Init, failed to create flare alpha accumulat target!");
		return false;
	}
	
	m_pFlareShader = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADER_DIR"ps\\solarflare_14.txt", false);
	if( !m_pFlareShader )
	{
		a_LogOutput(1, "CECSunMoon::Init, failed to load solar shade!");
	}

	// now create a rectangle
	m_pStreamSunPoint = new A3DStream();
	if( !m_pStreamSunPoint->Init(g_pGame->GetA3DDevice(), A3DVT_TLVERTEX, 9 * 6, 0, A3DSTRM_REFERENCEPTR, 0) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to create sun point stream");
		return false;
	}
	m_pStreamFlareAlpha = new A3DStream();
	if( !m_pStreamFlareAlpha->Init(g_pGame->GetA3DDevice(), sizeof(FLARE_ALPHA_VERT), FLARE_ALPHA_FVF, 1, 0, A3DSTRM_REFERENCEPTR, 0) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to create flare alpha stream");
		return false;
	}
	m_pStreamFlares = new A3DStream();
	if( !m_pStreamFlares->Init(g_pGame->GetA3DDevice(), A3DVT_TLVERTEX, 6 * 15, 0, A3DSTRM_REFERENCEPTR, 0) )
	{
		a_LogOutput(1, "CECSunMoon::Init, Falied to create flares stream");
		return false;
	}

	return true;
}

bool CECSunMoon::ReleaseFlare()
{
	A3DTextureMan * pTextureMan = g_pGame->GetA3DEngine()->GetA3DTextureMan();

	if( m_pSunFlareTexture )
	{
		pTextureMan->ReleaseTexture(m_pSunFlareTexture);
		m_pSunFlareTexture = NULL;
	}

	if( m_pMoonFlareTexture )
	{
		pTextureMan->ReleaseTexture(m_pMoonFlareTexture);
		m_pMoonFlareTexture = NULL;
	}

	if( m_pFlaresTexture )
	{
		pTextureMan->ReleaseTexture(m_pFlaresTexture);
		m_pFlaresTexture = NULL;
	}

	if( m_pFlareShader )
	{
		g_pGame->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pFlareShader);
		m_pFlareShader = NULL;
	}

	if( m_pFlareAlphaTarget )
	{
		m_pFlareAlphaTarget->Release();
		delete m_pFlareAlphaTarget;
		m_pFlareAlphaTarget = NULL;
	}

	A3DRELEASE(m_pStreamSunPoint);
	A3DRELEASE(m_pStreamFlareAlpha);
	A3DRELEASE(m_pStreamFlares);

	return true;
}

bool CECSunMoon::Render(CECViewport * pViewport)
{
	// see if not set time yet.
	if( !m_bTimeHasBeenSet || !m_bDirHasBeenSet )
		return true;

	DWORD dwClipState;
	dwClipState = g_pGame->GetA3DDevice()->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);
	
	A3DCamera * pCamera = pViewport->GetA3DCamera();
	A3DVECTOR3 vecCamPos;
	if( pCamera->IsMirrored() )
	{
		vecCamPos = pCamera->GetMirrorCamera()->GetPos();
		A3DMATRIX4 matMirror = pCamera->GetMirrorMatrix();
		vecCamPos = vecCamPos * InverseTM(matMirror);
		g_pGame->GetA3DDevice()->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);
	}
	else
	{
		vecCamPos = pCamera->GetPos();
	}

	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();
	pA3DDevice->SetTextureColorOP(1, A3DTOP_MODULATE);
	pA3DDevice->SetTextureColorArgs(1, A3DTA_TFACTOR, A3DTA_CURRENT);
	pA3DDevice->SetTextureAlphaOP(1, A3DTOP_MODULATE);
	pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TFACTOR, A3DTA_CURRENT);

	pA3DDevice->SetLightingEnable(false);
	pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pA3DDevice->SetDestAlpha(A3DBLEND_ONE);
	pA3DDevice->SetZWriteEnable(false);
	pA3DDevice->SetZTestEnable(false);
	pA3DDevice->SetFogEnable(false);

	BYTE sunVis = BYTE(m_vSunVisibility * 255.0f);
	BYTE moonVis = (BYTE)(m_vMoonVisibility * 255.0f);

	// first render sun
	if( m_vSunPitch > 0.0f )
	{
		m_pStreamSun->Appear();
		m_pSunTexture->Appear(0);
		pA3DDevice->SetTextureFactor((m_colorSun & 0x00ffffff) | (sunVis << 24));

		float	vSin = (float)sin(m_vSunPitch);
		float	vCos = (float)cos(m_vSunPitch);
		A3DVECTOR3 vecDir = A3DVECTOR3(vCos * (float)cos(m_vDeg), vSin, vCos * (float)sin(m_vDeg));
		A3DVECTOR3 vecRight = CrossProduct(A3DVECTOR3(0.0f, 1.0f, 0.0f), vecDir);
		A3DVECTOR3 vecUp = Normalize(CrossProduct(vecDir, vecRight));
		A3DVECTOR3 vecPos = vecCamPos + vecDir * SKY_FAR;

		float vSunScale = 1.0f + m_fDNFactor * 2.0f;
		A3DMATRIX4 mat = Scaling(vSunScale, vSunScale, vSunScale) * TransformMatrix(vecDir, vecUp, vecPos);
		pA3DDevice->SetWorldMatrix(mat);

		pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, 2);

		m_pSunTexture->Disappear(0);
	}

	// then render the sun shine
	if( m_fDNFactor != 0.0f && m_fDNFactor != 1.0f )
	{
		m_pStreamSunShine->Appear();
		m_pSunShineTexture->Appear(0);

		float vSunShine = 1.0f;
		if( m_fDNFactor < 0.3f )
		{
			vSunShine = 0.0f;
		}
		else if( m_fDNFactor < 0.6f )
		{
			vSunShine = (m_fDNFactor - 0.3f) / 0.3f;
		}
		else if( m_fDNFactor < 0.8f )
		{
			vSunShine = 1.0f;
		}
		else
		{
			vSunShine = (1.0f - m_fDNFactor) / 0.2f;
		}
		
		if( m_fDNFactorDest == 1.0f )
			vSunShine *= 0.8f; // sunset, just not too bright
		else
			vSunShine *= 1.0f; // sun rise, just make a little bright

		pA3DDevice->SetTextureFactor((sunVis << 24) | ((a3d_ColorRGBAToColorValue(m_colorSun) * vSunShine).ToRGBAColor() & 0x00ffffff));

		float	vSin = (float)sin(m_vSunPitch);
		float	vCos = (float)cos(m_vSunPitch);
		A3DVECTOR3 vecDir = A3DVECTOR3(vCos * (float)cos(m_vDeg), vSin, vCos * (float)sin(m_vDeg));
		A3DVECTOR3 vecRight = CrossProduct(A3DVECTOR3(0.0f, 1.0f, 0.0f), vecDir);
		A3DVECTOR3 vecUp = Normalize(CrossProduct(vecDir, vecRight));
		A3DVECTOR3 vecPos = vecCamPos + vecDir * SKY_FAR;

		float vSunScale = 1.0f + m_fDNFactor * 2.0f;
		A3DMATRIX4 mat = Scaling(vSunScale, vSunScale, vSunScale) * TransformMatrix(vecDir, vecUp, vecPos);
		pA3DDevice->SetWorldMatrix(mat);

		pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 8, 0, 6);

		m_pSunShineTexture->Disappear(0);
	}

	// then render the moon
	if( m_vMoonPitch > 0.0f )
	{
		m_pStreamMoon->Appear();
		m_pMoonTexture->Appear(0);

		pA3DDevice->SetTextureFactor((moonVis << 24) | 0x00808080 | ((m_colorMoon >> 1) & 0x007f7f7f));

		float	vMoonSin = (float)sin(m_vMoonPitch);
		float	vMoonCos = (float)cos(m_vMoonPitch);
		A3DVECTOR3 vecMoonDir = A3DVECTOR3(vMoonCos * (float)cos(m_vDeg), vMoonSin, vMoonCos * (float)sin(m_vDeg));
		A3DVECTOR3 vecMoonRight = CrossProduct(A3DVECTOR3(0.0f, 1.0f, 0.0f), vecMoonDir);
		A3DVECTOR3 vecMoonUp = Normalize(CrossProduct(vecMoonDir, vecMoonRight));
		A3DVECTOR3 vecMoonPos = vecCamPos + vecMoonDir * SKY_FAR;

		A3DMATRIX4 mat = TransformMatrix(vecMoonDir, vecMoonUp, vecMoonPos);
		pA3DDevice->SetWorldMatrix(mat);

		pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, 2);

		m_pMoonTexture->Disappear(0);
	}

	pA3DDevice->SetFogEnable(true);
	pA3DDevice->SetZWriteEnable(true);
	pA3DDevice->SetZTestEnable(true);
	pA3DDevice->SetLightingEnable(true);
	pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
	pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);

	if( pCamera->IsMirrored() )
	{
		g_pGame->GetA3DDevice()->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, dwClipState);
	}
	return true;
}

inline A3DVECTOR3& CLAMPVECTOR(A3DVECTOR3& v, int minx, int maxx, int miny, int maxy)
{
	if( v.x < minx ) v.x = minx + 0.5f;
	else if( v.x > maxx ) v.x = (float)maxx - 0.5f;

	if( v.y < miny ) v.y = miny + 0.5f;
	else if( v.y > maxy ) v.y = (float)maxy - 0.5f;

	return v;
}

bool CECSunMoon::RenderSolarFlare(CECViewport * pViewport)
{
	// see if not set time yet.
	if( !m_bCanDoFlare || !m_bTimeHasBeenSet || !m_bDirHasBeenSet )
		return true;

	A3DCamera * pCamera = pViewport->GetA3DCamera();
	A3DVECTOR3 vecCamPos, vecCamDir;
	vecCamPos = pCamera->GetPos();
	vecCamDir = pCamera->GetDir();

	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();

	float	vPitch = max(m_vSunPitch, m_vMoonPitch);
	float	vSin = (float)sin(vPitch);
	float	vCos = (float)cos(vPitch);
	A3DVECTOR3 vecDir = A3DVECTOR3(vCos * (float)cos(m_vDeg), vSin, vCos * (float)sin(m_vDeg));
	A3DVECTOR3 vecRight = CrossProduct(A3DVECTOR3(0.0f, 1.0f, 0.0f), vecDir);
	A3DVECTOR3 vecUp = Normalize(CrossProduct(vecDir, vecRight));
	A3DVECTOR3 vecPos = vecCamPos + vecDir * SKY_FAR;

	// first test if we need the flare
	CECFullGlowRender * pGlowRender = g_pGame->GetGameRun()->GetFullGlowRender();
	if( !pGlowRender || !pGlowRender->IsFlareOn() || !pGlowRender->IsRenderToBack() || !m_pFlareShader )
		return true;

	float vScale;
	A3DVECTOR3 vecCenterPos;
	A3DVECTOR3 vecCorner;
	A3DMATRIX4 mat;

	if( vPitch == m_vSunPitch )
	{
		vScale = 1.0f + m_fDNFactor * 2.0f;
		mat = Scaling(vScale, vScale, vScale) * TransformMatrix(vecDir, vecUp, vecPos);
		vecCenterPos = A3DVECTOR3(0.0f, -1.0f * SUN_UNIT_SIZE, 0.0f) * mat;
		vecCorner = A3DVECTOR3(-1.0f * SUN_UNIT_SIZE, 0.0f, 0.0f) * mat;
	}
	else
	{
		vScale = 1.0f;
		mat = Scaling(vScale, vScale, vScale) * TransformMatrix(vecDir, vecUp, vecPos);
		vecCenterPos = A3DVECTOR3(0.0f, -1.0f * MOON_UNIT_SIZE, 0.0f) * mat;
		vecCorner = A3DVECTOR3(-1.0f * MOON_UNIT_SIZE, 0.0f, 0.0f) * mat;
	}

	pViewport->GetA3DViewport()->Transform(vecCenterPos, vecCenterPos);
	pViewport->GetA3DViewport()->Transform(vecCorner, vecCorner);

	A3DVIEWPORTPARAM param = *pViewport->GetA3DViewport()->GetParam();
	int minx, maxx, miny, maxy;
	minx = param.X; maxx = param.X + param.Width;
	miny = param.Y; maxy = param.Y + param.Height;

	float v = max(float(fabs(vecCorner.x - vecCenterPos.x)), float(fabs(vecCorner.y - vecCenterPos.y))) * 0.2f;

	int cx, cy;
	cx = (int) vecCenterPos.x;
	cy = (int) vecCenterPos.y;

	m_pStreamSunPoint->Appear();
	A3DTLVERTEX * pVerts;

	int i;
	const float dx[] = {0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f};
	const float dy[] = {0.0f, -1.0f, -0.5f, 1.0f, 0.0f, 0.0f};

	pA3DDevice->SetAlphaBlendEnable(false);
	pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA);
	
	if( !m_pStreamSunPoint->LockVertexBuffer(0, 0, (BYTE **)&pVerts, NULL) )
	{
		a_LogOutput(1, "CECSunMoon::Init, failed to lock sun point stream!");
		return false;
	}

	for(i=0; i<6; i++)
	{
		pVerts[i * 9 + 0] = A3DTLVERTEX(CLAMPVECTOR(A3DVECTOR3(cx + dx[i] * v - 1.0f, cy + dy[i] * v - 1.0f, 1.0f), minx - 1, maxx - 1, miny - 1, maxy - 1), 0x000000ff, 0xff000000, 0.0f, 0.0f);
		pVerts[i * 9 + 1] = A3DTLVERTEX(CLAMPVECTOR(A3DVECTOR3(cx + dx[i] * v - 1.0f, cy + dy[i] * v + 0.0f, 1.0f), minx - 1, maxx - 1, miny    , maxy    ), 0x000000ff, 0xff000000, 0.0f, 0.0f);
		pVerts[i * 9 + 2] = A3DTLVERTEX(CLAMPVECTOR(A3DVECTOR3(cx + dx[i] * v - 1.0f, cy + dy[i] * v + 1.0f, 1.0f), minx - 1, maxx - 1, miny + 1, maxy + 1), 0x000000ff, 0xff000000, 0.0f, 0.0f);
		pVerts[i * 9 + 3] = A3DTLVERTEX(CLAMPVECTOR(A3DVECTOR3(cx + dx[i] * v + 0.0f, cy + dy[i] * v - 1.0f, 1.0f), minx    , maxx    , miny - 1, maxy - 1), 0x000000ff, 0xff000000, 0.0f, 0.0f);
		pVerts[i * 9 + 4] = A3DTLVERTEX(CLAMPVECTOR(A3DVECTOR3(cx + dx[i] * v + 0.0f, cy + dy[i] * v + 0.0f, 1.0f), minx    , maxx    , miny    , maxy    ), 0x000000ff, 0xff000000, 0.0f, 0.0f);
		pVerts[i * 9 + 5] = A3DTLVERTEX(CLAMPVECTOR(A3DVECTOR3(cx + dx[i] * v + 0.0f, cy + dy[i] * v + 1.0f, 1.0f), minx    , maxx    , miny + 1, maxy + 1), 0x000000ff, 0xff000000, 0.0f, 0.0f);
		pVerts[i * 9 + 6] = A3DTLVERTEX(CLAMPVECTOR(A3DVECTOR3(cx + dx[i] * v + 1.0f, cy + dy[i] * v - 1.0f, 1.0f), minx + 1, maxx + 1, miny - 1, maxy - 1), 0x000000ff, 0xff000000, 0.0f, 0.0f);
		pVerts[i * 9 + 7] = A3DTLVERTEX(CLAMPVECTOR(A3DVECTOR3(cx + dx[i] * v + 1.0f, cy + dy[i] * v + 0.0f, 1.0f), minx + 1, maxx + 1, miny    , maxy    ), 0x000000ff, 0xff000000, 0.0f, 0.0f);
		pVerts[i * 9 + 8] = A3DTLVERTEX(CLAMPVECTOR(A3DVECTOR3(cx + dx[i] * v + 1.0f, cy + dy[i] * v + 1.0f, 1.0f), minx + 1, maxx + 1, miny + 1, maxy + 1), 0x000000ff, 0xff000000, 0.0f, 0.0f);
	}
	m_pStreamSunPoint->UnlockVertexBuffer();
	pA3DDevice->ClearTexture(0);
	pA3DDevice->SetZTestEnable(false);
	pA3DDevice->SetZWriteEnable(false);
	pA3DDevice->DrawPrimitive(A3DPT_POINTLIST, 0, 9 * 6);
	
	if( vecCenterPos.z >= 0.0f && vecCenterPos.z <= 1.0f )
	{
		if( !m_pStreamSunPoint->LockVertexBuffer(0, 0, (BYTE **)&pVerts, NULL) )
		{
			a_LogOutput(1, "CECSunMoon::Init, failed to lock sun point stream!");
			return false;
		}

		for(i=0; i<6; i++)
		{
			pVerts[i * 9 + 0] = A3DTLVERTEX(A3DVECTOR3(cx + dx[i] * v - 1.0f, cy + dy[i] * v - 1.0f, 1.0f), 0xffff0000, 0xff000000, 0.0f, 0.0f);
			pVerts[i * 9 + 1] = A3DTLVERTEX(A3DVECTOR3(cx + dx[i] * v - 1.0f, cy + dy[i] * v + 0.0f, 1.0f), 0xffff0000, 0xff000000, 0.0f, 0.0f);
			pVerts[i * 9 + 2] = A3DTLVERTEX(A3DVECTOR3(cx + dx[i] * v - 1.0f, cy + dy[i] * v + 1.0f, 1.0f), 0xffff0000, 0xff000000, 0.0f, 0.0f);
			pVerts[i * 9 + 3] = A3DTLVERTEX(A3DVECTOR3(cx + dx[i] * v + 0.0f, cy + dy[i] * v - 1.0f, 1.0f), 0xffff0000, 0xff000000, 0.0f, 0.0f);
			pVerts[i * 9 + 4] = A3DTLVERTEX(A3DVECTOR3(cx + dx[i] * v + 0.0f, cy + dy[i] * v + 0.0f, 1.0f), 0xffff0000, 0xff000000, 0.0f, 0.0f);
			pVerts[i * 9 + 5] = A3DTLVERTEX(A3DVECTOR3(cx + dx[i] * v + 0.0f, cy + dy[i] * v + 1.0f, 1.0f), 0xffff0000, 0xff000000, 0.0f, 0.0f);
			pVerts[i * 9 + 6] = A3DTLVERTEX(A3DVECTOR3(cx + dx[i] * v + 1.0f, cy + dy[i] * v - 1.0f, 1.0f), 0xffff0000, 0xff000000, 0.0f, 0.0f);
			pVerts[i * 9 + 7] = A3DTLVERTEX(A3DVECTOR3(cx + dx[i] * v + 1.0f, cy + dy[i] * v + 0.0f, 1.0f), 0xffff0000, 0xff000000, 0.0f, 0.0f);
			pVerts[i * 9 + 8] = A3DTLVERTEX(A3DVECTOR3(cx + dx[i] * v + 1.0f, cy + dy[i] * v + 1.0f, 1.0f), 0xffff0000, 0xff000000, 0.0f, 0.0f);
		}

		m_pStreamSunPoint->UnlockVertexBuffer();
		pA3DDevice->SetZTestEnable(true);
		pA3DDevice->SetZWriteEnable(false);
		pA3DDevice->DrawPrimitive(A3DPT_POINTLIST, 0, 9 * 6);
	}

	pA3DDevice->SetZTestEnable(true);
	pA3DDevice->SetZWriteEnable(true);
	
	pA3DDevice->SetAlphaBlendEnable(true);
	pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	A3DRenderTarget * pTarget = pGlowRender->GetBackTarget();
	// Todo: we can only update these point we need.
#ifdef ANGELICA_2_2
	g_pGame->GetA3DDevice()->GetD3DDevice()->StretchRect(g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, pTarget->GetTargetSurface(), NULL, D3DTEXF_POINT);
#else
	g_pGame->GetA3DDevice()->GetD3DDevice()->CopyRects(g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, 0, pTarget->GetTargetSurface(), NULL);
#endif // ANGELICA_2_2

	// now render to alpha accumulate target
	pA3DDevice->SetRenderTarget(m_pFlareAlphaTarget);
	param.X = 0;
	param.Y = 0;
	param.Width = 1;
	param.Height = 1;
	param.MinZ = 0.0f;
	param.MaxZ = 1.0f;
	pA3DDevice->SetViewport(&param);
	pA3DDevice->SetZWriteEnable(false);
	pA3DDevice->SetZTestEnable(false);

	m_pFlareShader->Appear();
	m_pStreamFlareAlpha->Appear();

	FLARE_ALPHA_VERT * pVert;
	if( !m_pStreamFlareAlpha->LockVertexBuffer(0, 0, (BYTE **)&pVert, NULL) )
	{
		a_LogOutput(1, "CECSunMoon::Init, failed to lock flare alpha stream!");
		return false;
	}
	pVert[0].pos = A3DVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
	pVert[0].diffuse = 0xffffffff;
	for(i=0; i<6; i++)
	{
		pA3DDevice->GetD3DDevice()->SetTexture(i, pTarget->GetTargetTexture());
		pA3DDevice->SetTextureCoordIndex(i, i);
		pA3DDevice->SetTextureAddress(i, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		pVert[0].tex_coords[i].u = (cx + dx[i] * v + 0.5f) / pGlowRender->GetWidth();
		pVert[0].tex_coords[i].v = (cy + dy[i] * v + 0.5f) / pGlowRender->GetHeight();
	}
	m_pStreamFlareAlpha->UnlockVertexBuffer();

	A3DCOLORVALUE c0(1.0f, 1.0f, 1.0f, 1.0f);
	if( m_bFirstAlpha )
	{
		c0.a = 1.0f;
		m_bFirstAlpha = false;
	}
	else
		c0.a = g_pGame->GetTickTime() / 200.0f;
	pA3DDevice->SetPixelShaderConstants(0, &c0, 1);

	pA3DDevice->DrawPrimitive(A3DPT_POINTLIST, 0, 1);

	m_pFlareShader->Disappear();
	for(i=0; i<6; i++)
	{
		pA3DDevice->SetTextureAddress(i, A3DTADDR_WRAP, A3DTADDR_WRAP);
	}
	
	// now we use accumulated flare alpha texture to combine with the flare texture
	pA3DDevice->RestoreRenderTarget();
	pViewport->GetA3DViewport()->Active();

	pA3DDevice->GetD3DDevice()->SetTexture(1, m_pFlareAlphaTarget->GetTargetTexture());

	pA3DDevice->SetLightingEnable(false);
	pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pA3DDevice->SetDestAlpha(A3DBLEND_ONE);
	pA3DDevice->SetFogEnable(false);

	pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_TFACTOR);
	pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_TFACTOR);
	pA3DDevice->SetTextureColorOP(1, A3DTOP_MODULATE);
	pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	pA3DDevice->SetTextureAlphaOP(1, A3DTOP_SELECTARG2);
	pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	pA3DDevice->SetTextureCoordIndex(1, 0);

	BYTE sunVis = BYTE(m_vSunVisibility * 255.0f);
	BYTE moonVis = BYTE(m_vMoonVisibility * 255.0f);
	if( m_vSunPitch > 0.0f )
	{
		pA3DDevice->SetTextureFactor((sunVis << 24) | ((a3d_ColorRGBAToColorValue(m_colorSun) * 0.8f).ToRGBAColor() & 0x00ffffff));

		// render lens flares
		UpdateLensFlares(pViewport->GetA3DViewport(), vecCamPos, vecCamDir, A3DVECTOR3(0.0f, -1.0f * SUN_UNIT_SIZE, 0.0f) * mat);

		m_pStreamFlares->Appear();
		m_pFlaresTexture->Appear(0);
		
		pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, 15 * 2);
	
		m_pFlaresTexture->Disappear(0);
	}

	if( m_vMoonPitch > 0.0f )
	{
		m_pStreamMoon->Appear();
		m_pMoonFlareTexture->Appear(0);

		pA3DDevice->SetTextureFactor((moonVis << 24) | ((a3d_ColorRGBAToColorValue(m_colorMoon) * 0.5f).ToRGBAColor() & 0x00ffffff));
		
		float vMoonScale = 1.0f;
		A3DMATRIX4 mat = Scaling(vMoonScale, vMoonScale, vMoonScale) * TransformMatrix(vecDir, vecUp, vecPos);
		pA3DDevice->SetWorldMatrix(mat);
		
		pA3DDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, 2);

		m_pMoonFlareTexture->Disappear(0);
	}

	if( m_fDNFactor != 0.0f && m_fDNFactor != 1.0f )
	{
		float vSunShine = 1.0f;
		if( m_fDNFactor < 0.1f )
		{					
			vSunShine = 0.0f;
		}
		else if( m_fDNFactor < 0.6f )
		{
			vSunShine = (m_fDNFactor - 0.1f) / 0.5f;
		}
		else if( m_fDNFactor < 0.8f )
		{
			vSunShine = (0.8f - m_fDNFactor) / 0.2f;
		}
		else
		{
			vSunShine = 0.0f;
		}
		
		if( m_fDNFactorDest == 1.0f )
			vSunShine *= 0.5f; // sunset, just not too bright
		else
			vSunShine *= 0.6f; // sun rise, just make a little bright

		m_pStreamSunShine->Appear();
		m_pSunShineTexture->Appear(0);

		pA3DDevice->SetTextureFactor((sunVis << 24) | ((a3d_ColorRGBAToColorValue(m_colorSun) * vSunShine).ToRGBAColor() & 0x00ffffff));
		
		float vSunScale = 1.0f + m_fDNFactor * 2.0f;
		A3DMATRIX4 mat = Scaling(vSunScale, vSunScale, vSunScale) * TransformMatrix(vecDir, vecUp, vecPos);
		pA3DDevice->SetWorldMatrix(mat);
		
		pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 8, 0, 6);

		m_pSunShineTexture->Disappear(0);
	}

	pA3DDevice->GetD3DDevice()->SetTexture(1, NULL);

	pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
	pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	pA3DDevice->SetTextureCoordIndex(1, 1);
	pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	
	pA3DDevice->SetFogEnable(true);
	pA3DDevice->SetZWriteEnable(true);
	pA3DDevice->SetZTestEnable(true);
	pA3DDevice->SetLightingEnable(true);
	pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	return true;
}

bool CECSunMoon::Tick(DWORD dwDeltaTime)
{
	double vDeltaTime = 0.001 * dwDeltaTime / 3600.0 / 24.0 * TIME_SCALE;

	m_vTimeOfTheDay += vDeltaTime;
	while( m_vTimeOfTheDay > 1.0 )
		m_vTimeOfTheDay -= 1.0;

	UpdateWithTime();

	// update sun moon visibility
	if( m_bSunVisible )
		m_vSunVisibility += dwDeltaTime * 0.0003f;
	else
		m_vSunVisibility -= dwDeltaTime * 0.0003f;
	a_Clamp(m_vSunVisibility, 0.0f, 1.0f);

	if( m_bMoonVisible )
		m_vMoonVisibility += dwDeltaTime * 0.0003f;
	else
		m_vMoonVisibility -= dwDeltaTime * 0.0003f;
	a_Clamp(m_vMoonVisibility, 0.0f, 1.0f);

	return true;
}

bool CECSunMoon::SetTimeOfTheDay(float vTime)
{
	m_bTimeHasBeenSet = true;

	while(vTime < 0.0f )
		vTime += 1.0f;
	while(vTime > 1.0f )
		vTime -= 1.0f;
	m_vTimeOfTheDay = vTime;

	UpdateWithTime();
	return true;
}

bool CECSunMoon::SetSunColor(A3DCOLOR color)
{
	m_colorSun = color;
	return true;
}

bool CECSunMoon::SetMoonColor(A3DCOLOR color)
{
	m_colorMoon = color;
	return true;
}

bool CECSunMoon::SetLightDir(const A3DVECTOR3& vecLightDir)
{
	m_bDirHasBeenSet = true;

	m_vDeg = (float)atan2(-vecLightDir.z, -vecLightDir.x);
	m_vecLightDir = vecLightDir;
	return true;
}

bool CECSunMoon::UpdateWithTime()
{
	float vTimeOfTheDay = (float)m_vTimeOfTheDay;

	float vPitch;
	if( vTimeOfTheDay < DAWN_TIME )
	{
		m_vSunPitch = 0.0f;
		m_vMoonPitch = (1.0f - vTimeOfTheDay / DAWN_TIME) * MOON_MAX_PITCH;
		vPitch = m_vMoonPitch;
	}
	else if( vTimeOfTheDay < MID_NOON )
	{
		m_vSunPitch = (vTimeOfTheDay - DAWN_TIME) / (MID_NOON - DAWN_TIME) * SUN_MAX_PITCH;
		m_vMoonPitch = 0.0f;
		vPitch = m_vSunPitch;
	}
	else if( vTimeOfTheDay < SUN_SET )
	{
		m_vSunPitch = (1.0f - (vTimeOfTheDay - MID_NOON) / (SUN_SET - MID_NOON)) * SUN_MAX_PITCH;
		m_vMoonPitch = 0.0f;
		vPitch = m_vSunPitch;
	}
	else
	{
		m_vSunPitch = 0.0f;
		m_vMoonPitch = (vTimeOfTheDay - SUN_SET) / (MID_NIGHT - SUN_SET) * MOON_MAX_PITCH;
		vPitch = m_vMoonPitch;
	}

	// update day night factor
	if( vTimeOfTheDay < NIGHT_DAY_START )
	{
		m_fDNFactor = 1.0f;
		m_fDNFactorDest = 1.0f;
	}
	else if( vTimeOfTheDay < NIGHT_DAY_END )
	{
		m_fDNFactor = 1.0f - (vTimeOfTheDay - NIGHT_DAY_START) / (NIGHT_DAY_END - NIGHT_DAY_START);
		m_fDNFactorDest = 0.0f;
	}
	else if( m_vTimeOfTheDay < DAY_NIGHT_START )
	{
		m_fDNFactor = 0.0f;
		m_fDNFactorDest = 0.0f;
	}
	else if( vTimeOfTheDay < DAY_NIGHT_END )
	{
		m_fDNFactor = (vTimeOfTheDay - DAY_NIGHT_START) / (DAY_NIGHT_END - DAY_NIGHT_START);
		m_fDNFactorDest = 1.0f;
	}
	else
	{
		m_fDNFactor = 1.0f;
		m_fDNFactorDest = 1.0f;
	}

	float vCos = (float)cos(vPitch);
	m_vecLightDir.y = -(float)sin(vPitch);
	m_vecLightDir.x = -vCos * (float)cos(m_vDeg);
	m_vecLightDir.z = -vCos * (float)sin(m_vDeg);
	return true;
}

int CECSunMoon::GetDNTransTime()
{
	if( m_fDNFactorDest == 0.0f )
	{
		return int((NIGHT_DAY_END - NIGHT_DAY_START) * 24 * 3600 * (1.0f / TIME_SCALE) * 1000);
	}
	else 
	{
		return int((DAY_NIGHT_END - DAY_NIGHT_START) * 24 * 3600 * (1.0f / TIME_SCALE) * 1000);
	}

	return 0;
}

bool CECSunMoon::UpdateLensFlares(A3DViewport * pViewport, const A3DVECTOR3 vecCamPos, const A3DVECTOR3 vecCamDir, const A3DVECTOR3 vecSunPos)
{
	const float vSize[] = {
		0.06f, 
		0.45f, 0.35f,
		0.04f, 0.09f, 0.05f, 0.04f, 0.015f, 
		0.02f, 0.07f, 0.12f, 0.03f,
		0.03f, 0.15f, 0.3f};
	const float vDis[] = {
		860.0f, 
		100.0f, 100.0f,
		40.0f, 25.0f, 20.0f, 10.0f, 4.0f, 
		-2.5f, -4.0f, -4.4f, -4.5f, 
		-6.2f, -8.0f, -9.0f};
	
#define WHITERING {0.75f, 0.25f}
#define BLUEDISK {0.0f, 0.0f}
#define BLUEDISKWEAK {0.25f, 0.0f}
#define BROWNRING {0.5f, 0.0f}
#define WHITEGRADIENT {0.75f, 0.0f}
#define BROWNDISK {0.0f, 0.25f}
#define GREENRING {0.25f, 0.25f}
#define RAINBOWRING {0.5f, 0.25f}
#define WHITERING {0.75f, 0.25f}
#define BLUEGRADIENT {0.0f, 0.5f}
#define WHITEREDRING {0.25f, 0.5f}
#define WHITEREDRING2 {0.5f, 0.5f}

	const float uv[][2] = { 
		WHITERING,
		WHITEREDRING, WHITEREDRING2,
		BLUEDISK, BLUEDISKWEAK, BLUEDISK, BROWNRING, WHITEGRADIENT,
		WHITEGRADIENT, BROWNRING, BROWNRING, BROWNRING,
		BLUEGRADIENT, GREENRING, RAINBOWRING
	};

	const float ts[] = {
		0.25f, 
		0.25f, 0.5f, 
		0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 
		0.25f, 0.25f, 0.25f, 0.25f, 
		0.25f, 0.25f, 0.25f};

	A3DVIEWPORTPARAM param = *pViewport->GetParam();
	A3DVECTOR3 vecCenterPos = vecCamPos + vecCamDir * 15.0f;
	A3DVECTOR3 vecRayDir = Normalize(vecSunPos - vecCenterPos);

	A3DTLVERTEX * pVerts;
	if( !m_pStreamFlares->LockVertexBuffer(0, 0, (BYTE **)&pVerts, NULL) )
	{
		return false;
	}

	int i;
	for(i=0; i<15; i++)
	{
		float u = uv[i][0];
		float v = uv[i][1];

		A3DVECTOR3 vecPos = vecCenterPos + vecRayDir * vDis[i];
		A3DVECTOR3 vecScreenPos;

		A3DVECTOR3 vecHalfX(0.0f);
		A3DVECTOR3 vecHalfY(0.0f);
		if( pViewport->Transform(vecPos, vecScreenPos) )
		{
			vecScreenPos.x = -1.0f;
			vecScreenPos.y = -1.0f;
		}
		else
		{
			vecScreenPos.z = 0.0f;
			vecHalfX = A3DVECTOR3(vSize[i] * param.Height, 0.0f, 0.0f);
			vecHalfY = A3DVECTOR3(0.0f, vSize[i] * param.Height, 0.0f);
		}
	
		pVerts[i * 6 + 0] = A3DTLVERTEX(vecScreenPos - vecHalfX - vecHalfY, 0xffffffff, 0xff000000, u, v);
		pVerts[i * 6 + 1] = A3DTLVERTEX(vecScreenPos + vecHalfX - vecHalfY, 0xffffffff, 0xff000000, u + ts[i], v);
		pVerts[i * 6 + 2] = A3DTLVERTEX(vecScreenPos - vecHalfX + vecHalfY, 0xffffffff, 0xff000000, u, v + ts[i]);
		pVerts[i * 6 + 3] = pVerts[i * 6 + 2];
		pVerts[i * 6 + 4] = pVerts[i * 6 + 1];
		pVerts[i * 6 + 5] = A3DTLVERTEX(vecScreenPos + vecHalfX + vecHalfY, 0xffffffff, 0xff000000, u + ts[i], v + ts[i]);
	}

	m_pStreamFlares->UnlockVertexBuffer();

	return true;
}
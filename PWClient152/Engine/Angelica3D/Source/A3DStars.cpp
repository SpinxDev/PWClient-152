/*
 * FILE: A3DStars.h
 *
 * DESCRIPTION: Class that manipulating the stars in A3D Engine
 *
 * CREATED BY: Hedi, 2002/5/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DStars.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DMathUtility.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DConfig.h"
#include "A3DTextureMan.h"
#include "A3DTexture.h"
#include "A3DCameraBase.h"
#include "AAssist.h"

A3DStars::A3DStars()
{
	m_pA3DDevice		= NULL;
	m_pVertexBuffer		= NULL;
	m_pStarTexture		= NULL;
	m_nNumStars			= 0;
	m_bShow				= true;
	m_bUsePointSize		= false;
	m_vPointSize		= 1.0f;
}

A3DStars::~A3DStars()
{
}

bool A3DStars::Init(A3DDevice * pA3DDevice, int nNumStars, A3DCOLOR color, DWORD dwRandSeed, 
					DWORD dwBlinkInterval, DWORD dwBlinkDistance, DWORD dwBlinkAmplitudeMin, DWORD dwBlinkAmplitudeMax,
					FLOAT vPointSize, char * pszStarTexture)
{
	m_pA3DDevice = pA3DDevice;

	SetNumStars(nNumStars);
	SetColor(color);
	SetRandSeed(dwRandSeed);
	SetBlinkInterval(dwBlinkInterval);
	SetBlinkDistance(dwBlinkDistance);
	SetBlinkAmplitudeMax(dwBlinkAmplitudeMax);
	SetBlinkAmplitudeMin(dwBlinkAmplitudeMin);

	GenerateStars();

	if( pszStarTexture )
	{
		m_strStarTexture = pszStarTexture;
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_strStarTexture, "Textures\\Sky", &m_pStarTexture, A3DTF_MIPLEVEL, 1) )
		{
			g_A3DErrLog.Log("A3DStars::Init(), Can not load texture [%s]", m_strStarTexture);
			return false;
		}
	}

	m_vPointSize = vPointSize;
	if( m_pA3DDevice->GetD3DCaps().MaxPointSize > 1.0f )
		m_bUsePointSize = true;
	else
		m_bUsePointSize = false;

	return true;
}

bool A3DStars::Release()
{
	if( m_pStarTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pStarTexture);
		m_pStarTexture = NULL;
	}
	if( m_pVertexBuffer )
	{
		free(m_pVertexBuffer);
		m_pVertexBuffer = NULL;
	}
	m_pA3DDevice = NULL;
	return true;
}

bool A3DStars::Render(A3DViewport * pCurrentViewport)
{
	if( !m_bShow && g_pA3DConfig->GetRunEnv() == A3DRUNENV_SCENEEDITOR ) return true;

	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetFogEnable(false);

	A3DCameraBase* pCamera = pCurrentViewport->GetCamera();
	A3DMATRIX4  matWorldMatrix;
	if( pCamera )
	{
		A3DVECTOR3 vecPos = pCamera->GetPos();
		matWorldMatrix = a3d_Translate(vecPos.x, vecPos.y, vecPos.z);
		m_pA3DDevice->SetWorldMatrix(matWorldMatrix);

		if( m_bUsePointSize && m_pStarTexture )
		{
			m_pA3DDevice->SetPointSize(m_vPointSize);
			m_pA3DDevice->SetPointSpriteEnable(true);
			m_pStarTexture->Appear(0);
		}

		m_pA3DDevice->DrawPrimitiveUP(A3DPT_POINTLIST, m_nNumStars, m_pVertexBuffer, sizeof(A3DLVERTEX));

		if( m_bUsePointSize && m_pStarTexture )
		{
			m_pStarTexture->Disappear(0);
			m_pA3DDevice->SetPointSize(1.0f);
			m_pA3DDevice->SetPointSpriteEnable(false);
		}
	}

	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFogEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	return true;
}

bool A3DStars::TickAnimation()
{
	// We should make some or all the stars blinks
	if( m_dwBlinkInterval == 0 )
		return true;

	for(int i=0; i<m_nNumStars; i++)
	{
		DWORD	dwBlinkTick = m_pA3DDevice->GetA3DEngine()->GetAnimationTicks() + i;
		DWORD	dwBlinkInterval	= m_dwBlinkInterval;

		if( m_dwBlinkDistance > 0 )
			dwBlinkInterval += i % m_dwBlinkDistance;

		FLOAT	vAmplitudeFactor = (g_A3DMathUtility.SIN((dwBlinkTick % dwBlinkInterval) * 360.0f / dwBlinkInterval) + 1.0f) / 2.0f; 
		BYTE	byteAlpha = (BYTE)(m_dwBlinkAmplitudeMin + vAmplitudeFactor * (m_dwBlinkAmplitudeMax - m_dwBlinkAmplitudeMin));
		
		m_pVertexBuffer[i].diffuse = (m_pVertexBuffer[i].diffuse & 0x00ffffff) | (byteAlpha << 24);
	}
	return true;
}

#define UNIVERSAL_RADIUS		1000.0f

bool A3DStars::GenerateStars()
{
	if( m_pVertexBuffer )
	{
		free(m_pVertexBuffer);
		m_pVertexBuffer = NULL;
	}

	m_pVertexBuffer = (A3DLVERTEX *) malloc(sizeof(A3DLVERTEX) * m_nNumStars);
	if( NULL == m_pVertexBuffer )
	{
		g_A3DErrLog.Log("A3DStars::GenerateStars(), Not enough memory!");
		return false;
	}

	FLOAT		vDeg, vPitch;
	A3DVECTOR3	vecPos;
	for(int i=0; i<m_nNumStars; i++)
	{
		vDeg = a_Random(0.0f, 720.0f);
		vPitch = a_Random(-360.0f, 360.0f);

		// If we use sphere coordinates to generate random star, we will
		// get a hign density at the top and bottom of the sphere, this 
		// is caused by the combination of two rand function.
		/*vecPos.x = (FLOAT)(sin(DEG2RAD(vDeg)) * cos(DEG2RAD(vPitch)));
		vecPos.y = (FLOAT)sin(DEG2RAD(vPitch));
		vecPos.z = (FLOAT)(cos(DEG2RAD(vDeg)) * cos(DEG2RAD(vPitch)));*/

		vecPos.x = a_Random(-1.0f, 1.0f);
		vecPos.y = a_Random(-1.0f, 1.0f);
		vecPos.z = a_Random(-1.0f, 1.0f);

		vecPos = Normalize(vecPos) * UNIVERSAL_RADIUS;
		
		m_pVertexBuffer[i] = A3DLVERTEX(vecPos, m_color, 0xff000000, 0.0f, 0.0f);
	}

	return true;
}

bool A3DStars::RegenerateStars()
{
	return GenerateStars();
}

bool A3DStars::SetStarTexture(const char* szStarTexture)
{
	if( m_pStarTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pStarTexture);
		m_pStarTexture = NULL;
	}

	m_strStarTexture = szStarTexture;
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_strStarTexture, "Textures\\Sky", &m_pStarTexture) )
	{
		g_A3DErrLog.Log("A3DStars::SetStarTexture(), Can not load texture [%s]", m_strStarTexture);
		return false;
	}
	return true;
}
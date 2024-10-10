/*
 * FILE: A3DLightning.cpp
 *
 * DESCRIPTION: A electric strike from sky to ground 
 *
 * CREATED BY: Hedi, 2002/10/18
 *
 * HISTORY:
 *
 * Copyright (c) 2002 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DLightning.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DWorld.h"
#include "A3DTerrain.h"
#include "A3DMathUtility.h"
#include "AMemory.h"
#include "AAssist.h"

#define new A_DEBUG_NEW

A3DLightning::A3DLightning()
{
	m_pA3DDevice	= NULL;
	m_nTicks		= 0;
	m_bStrike		= false;

	m_ppFlashes		= NULL;
	m_nNumFlashes	= 0;
}

A3DLightning::~A3DLightning()
{
}

bool A3DLightning::Init(A3DDevice * pDevice, FLOAT vStrikeSize, FLOAT vStepSize, int nSteps, int nMaxLevel/*=4*/, A3DCOLOR color/*=0xffffffff*/)
{
	m_noiseForFlash.Init(256, 2.0f, 32, 0.75f, 6, 0x12345);
	m_noiseForMe.Init(256, 2.0f, 32, 0.85f, 6, 0x54321);
	
	m_pA3DDevice	= pDevice;
	m_nNumFlashes	= nSteps;
	m_vStrikeSize	= vStrikeSize;
	m_vStepSize		= vStepSize;
	m_nMaxLevel		= nMaxLevel;
	m_color			= color;

	m_nNumFlashesToRen = 0;
	m_nTicks		= 0;
	m_bStrike		= false;

	m_ppFlashes = (A3DFlash **)a_malloc(sizeof(A3DFlash *) * m_nNumFlashes);
	if( NULL == m_ppFlashes )
	{
		g_A3DErrLog.Log("A3DLightning::Init(), Not enough memory!");
		return false;
	}

	for(int i=0; i<m_nNumFlashes; i++)
	{
		m_ppFlashes[i] = new A3DFlash();
		
		if( !m_ppFlashes[i]->Init(m_pA3DDevice, &m_noiseForFlash, 1.0f, 3) )
			return false;

		m_ppFlashes[i]->SetTextureMap("flash.bmp");
	}

	m_flashBackground.Init(m_pA3DDevice, &m_noiseForFlash, m_vStepSize, 1);
	m_flashBackground.SetTextureMap("flash_back.bmp");

	m_colorAmbient = m_pA3DDevice->GetAmbientColor();
	return true;
}

bool A3DLightning::Release()
{
	m_flashBackground.Release();

	m_noiseForMe.Release();
	m_noiseForFlash.Release();

	if( m_ppFlashes )
	{
		for(int i=0; i<m_nNumFlashes; i++)
		{
			if( m_ppFlashes[i] )
			{
				m_ppFlashes[i]->Release();
				delete m_ppFlashes[i];
				m_ppFlashes[i] = NULL;
			}
		}

		a_free(m_ppFlashes);
		m_ppFlashes = NULL;
	}

	return true;
}

bool A3DLightning::Render(A3DViewport * pCurrentViewport)
{
	if( !m_bStrike )
		return true;

	m_flashBackground.Render(pCurrentViewport);
	for(int i=0; i<m_nNumFlashesToRen; i++)
		m_ppFlashes[i]->Render(pCurrentViewport);
	
	return true;
}

bool A3DLightning::TickAnimation()
{
	if( !m_bStrike )
		return true;

	if( m_nTicks <= 7 )
	{
		m_nNumFlashesToRen = m_nNumFlashes * m_nTicks / 7;
		if( 0 == (m_nTicks % 2) )
		{
			Build();
			if( 0 == ((m_nTicks / 2) % 2) )
			{
				m_pA3DDevice->SetAmbient(A3DCOLORRGB(255, 255, 255));
				if (m_pA3DDevice->GetA3DEngine()->GetA3DWorld())
				{
					A3DTerrain* pTerrain = m_pA3DDevice->GetA3DEngine()->GetA3DWorld()->GetA3DTerrain();
					if (pTerrain)
					{
						pTerrain->SetNoLight(true);
						pTerrain->UpdateAllChanges(true);
					}
				}
			}
			else
			{
				// restore ambient light;
				m_pA3DDevice->SetAmbient(m_colorAmbient);
				if (m_pA3DDevice->GetA3DEngine()->GetA3DWorld())
				{
					A3DTerrain* pTerrain = m_pA3DDevice->GetA3DEngine()->GetA3DWorld()->GetA3DTerrain();
					if (pTerrain)
					{
						pTerrain->SetNoLight(false);
						pTerrain->UpdateAllChanges(true);
					}
				}
			}
		}
	}
	else if( m_nTicks < 80 )
	{
		// Fade out;
		for(int i=0; i<m_nNumFlashes; i++)
		{
			A3DCOLOR flashColor = m_ppFlashes[i]->GetColor();

			int r, g, b, a;
			r = A3DCOLOR_GETRED(flashColor);
			g = A3DCOLOR_GETGREEN(flashColor);
			b = A3DCOLOR_GETBLUE(flashColor);
			a = A3DCOLOR_GETALPHA(flashColor);
			r -= 20; g -= 20; b -= 20; a -= 20;
			if( r < 0 ) r = 0;
			if( g < 0 ) g = 0;
			if( b < 0 ) b = 0;
			if( a < 0 ) a = 0;
			m_ppFlashes[i]->SetColor(A3DCOLORRGBA(r, g, b, a));
			m_flashBackground.SetColor(A3DCOLORRGBA(r, g, b, a));
		}
	}
	else
	{
		m_bStrike = false;
	}

	m_nTicks ++;
	return true;
}

bool A3DLightning::AddPath(const A3DMATRIX4& matOrient, const A3DVECTOR3& vecStart, const A3DVECTOR3 &vecDir, int nNoisePos, int nLevel, int& nNumLeft)
{
	if( nNumLeft <= 0 )
		return true;

	// First add a path
	A3DVECTOR3 vecEnd;
	int nFlashIndex = m_nNumCount;

	vecEnd = vecStart + vecDir * (a_Random(0.5f, 1.5f) * m_vStepSize);
	m_ppFlashes[nFlashIndex]->RebuildStrikePath(vecStart, vecEnd);
	m_ppFlashes[nFlashIndex]->SetSize(m_vStrikeSize / (nLevel + 1));
	int r, g, b, a;
	r = A3DCOLOR_GETRED(m_color);
	g = A3DCOLOR_GETGREEN(m_color);
	b = A3DCOLOR_GETBLUE(m_color);
	a = A3DCOLOR_GETALPHA(m_color);
	r >>= nLevel; g >>= nLevel; b >>= nLevel; a >>= nLevel;
	m_ppFlashes[nFlashIndex]->SetColor(A3DCOLORRGBA(r, g, b, a));
	nNumLeft --;
	m_nNumCount ++;

	// Calculate main path
	if( nLevel == 0 )
		m_nMainPathStep ++;

	if( nNumLeft <= 0 )
		return true;
	
	FLOAT vDeg, vPitch;
	A3DVECTOR3 vecPert, vecNewDir;

	// Then branchize if need
	if( nLevel <= m_nMaxLevel && a_Random(0.0f, 1.0f) < 0.5f )
	{
		int nNum1;
		nNum1 = int(nNumLeft * a_Random(0.2f, 0.8f));
		nNumLeft = nNumLeft - nNum1;
		if( nNum1 )
		{
			m_noiseForMe.GetValue(nNoisePos * 8.0f, vecPert.m, 3);

			vDeg		= vecPert.x * 360.0f;
			vPitch		= vecPert.y * 180.0f / 3.0f; // about -30 ~ 30 degree
			vecNewDir.z = g_A3DMathUtility.COS(vPitch);
			vecNewDir.x = g_A3DMathUtility.SIN(vPitch) * g_A3DMathUtility.COS(vDeg);
			vecNewDir.y = g_A3DMathUtility.SIN(vPitch) * g_A3DMathUtility.SIN(vDeg);
			vecNewDir	= vecNewDir * matOrient;
			// Fork;
			A3DVECTOR3 vecUp;
			if( fabs(vecNewDir.y) < 0.9f )
				vecUp = Normalize(CrossProduct(vecNewDir, A3DVECTOR3(0.0f, 1.0f, 0.0f)));
			else
				vecUp = Normalize(CrossProduct(vecNewDir, A3DVECTOR3(1.0f, 0.0f, 0.0f)));
			A3DMATRIX4 matNewOrient = a3d_TransformMatrix(vecNewDir, vecUp, A3DVECTOR3(0.0f));
			AddPath(matNewOrient, vecEnd, vecNewDir, nNoisePos + a_Random(0, 1022), nLevel + 1, nNum1);
		}
	}

	if( nNumLeft <= 0 )
		return true;

	// Last continue next path
	m_noiseForMe.GetValue(nNoisePos * 2.0f, vecPert.m, 3);

	vDeg		= vecPert.x * 360.0f;
	vPitch		= vecPert.y * 180.0f / 3.0f; // about -30 ~ 30 degree
	vecNewDir.z = g_A3DMathUtility.COS(vPitch);
	vecNewDir.x = g_A3DMathUtility.SIN(vPitch) * g_A3DMathUtility.COS(vDeg);
	vecNewDir.y = g_A3DMathUtility.SIN(vPitch) * g_A3DMathUtility.SIN(vDeg);
	vecNewDir	= vecNewDir * matOrient;

	if( !AddPath(matOrient, vecEnd, vecNewDir, nNoisePos + 1, nLevel, nNumLeft) )
		return false;

	return true;
}

bool A3DLightning::Build()
{
	int nTotalCount = m_nNumFlashes;

	m_nNumCount			= 0;
	m_nMainPathStep		= 0;

	A3DVECTOR3 vecUp;
	if( fabs(m_vecDir.y) < 0.9f )
		vecUp = Normalize(CrossProduct(m_vecDir, A3DVECTOR3(0.0f, 1.0f, 0.0f)));
	else
		vecUp = Normalize(CrossProduct(m_vecDir, A3DVECTOR3(1.0f, 0.0f, 0.0f)));

	m_matOrient = a3d_TransformMatrix(m_vecDir, vecUp, A3DVECTOR3(0.0f));

	// Build extire strike path
	if( !AddPath(m_matOrient, m_vecStart, m_vecDir, a_Random(0, 1022), 0, nTotalCount) )
		return true;

	A3DCameraBase* pCamera = m_pA3DDevice->GetA3DEngine()->GetActiveCamera();
	A3DVECTOR3 vecTop = m_vecStart + A3DVECTOR3(0.0f, 1000.0f, 0.0f);
	A3DVECTOR3 vecDown = m_vecStart + A3DVECTOR3(0.0f, -2000.0f, 0.0f);
	m_flashBackground.RebuildStrikePath(vecTop, vecDown);
	m_flashBackground.SetSize(1500.0f);//m_nMainPathStep * m_vStepSize / 2);
	m_flashBackground.SetColor(m_color);
	return true;
}

bool A3DLightning::ReCharge(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDir, const A3DVECTOR3& vecDownward)
{
	m_vecStart		= vecStart;
	m_vecDir		= vecDir;
	m_vecDownward	= vecDownward;
	
	return Build();
}

bool A3DLightning::Strike()
{
	m_nTicks			= 0;
	m_bStrike			= true;
	m_nNumFlashesToRen	= 0;

	return true;
}
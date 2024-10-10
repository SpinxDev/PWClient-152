/*
 * FILE: A3DLamp.cpp
 *
 * DESCRIPTION: Routines for light effect
 *
 * CREATED BY: duyuxin, 2001/11/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DLamp.h"
#include "A3DPI.h"
#include "A3DWorld.h"
#include "A3DConfig.h"
#include "A3DCameraBase.h"
#include "A3DLensFlare.h"
#include "A3DViewport.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DCDS.h"
#include "AAssist.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DLamp::A3DLamp()
{
	m_pA3DDevice	= NULL;
	m_pLensFlare	= NULL;
	m_dwFlags		= 0;
	m_fCurAlpha		= 0.0f;
	m_fCurSize		= 1.0f;
	m_bDrawLamp		= false;
	m_bDrawBurnout	= false;
	m_dwTickOff		= a_Random() % UPDATE_INTERVAL;
}

A3DLamp::~A3DLamp()
{
}

/*	Initialize object.

	Return true for success, otherwise return false.

	pDevice: address of A3DDevice object.
	szShineTex: texture for shine decal.
	dwLampFlags: lamp flags defined in A3DLamp.h
*/
bool A3DLamp::Init(A3DDevice* pDevice, char* szShineTex, DWORD dwLampFlags)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	A3DSHADER Shader = {A3DBLEND_SRCCOLOR, A3DBLEND_ONE};

	//	Shine decal
	if (!m_ShineDecal.Init(DECALTYPE_CENTER3D, pDevice, szShineTex, true))
	{
		g_A3DErrLog.Log("A3DLamp::Init initlaize shine decal failed.");
		return false;
	}

	//	Burnout decal
	if (!m_BurnoutDecal.Init(DECALTYPE_PURE2D, pDevice, NULL, true))
	{
		g_A3DErrLog.Log("A3DLamp::Init initlaize shine decal failed.");
		return false;
	}

	m_ShineDecal.SetShader(Shader);
	m_BurnoutDecal.SetShader(Shader);

	m_bObstruct	 = true;
	m_dwFlags	 = dwLampFlags;
	m_pA3DDevice = pDevice;

	return true;
}

//	Release object
void A3DLamp::Release()
{
	if( !m_pA3DDevice )		return;

	m_ShineDecal.Release();
	m_BurnoutDecal.Release();

	if (m_pLensFlare)
	{
		m_pLensFlare->Release();
		delete m_pLensFlare;
		m_pLensFlare = NULL;
	}

	m_pA3DDevice = NULL;
}

/*	Set lens flare parameters.

	szBaseName: base texture name of lens flare
*/
void A3DLamp::SetLensFlareParams(char* szBaseName)
{
	if( !m_pA3DDevice )		return;

	if (m_pLensFlare)
	{
		m_pLensFlare->Release();
		delete m_pLensFlare;
	}

	if (!(m_pLensFlare = new A3DLensFlare))
	{
		g_A3DErrLog.Log("A3DLamp::SetLensFlareParams create lens flare object fail.");
		return;
	}

	if (!m_pLensFlare->Init(m_pA3DDevice, szBaseName))
	{
		g_A3DErrLog.Log("A3DLamp::SetLensFlareParams init lens flare fail.");
		return;
	}
}

//	Update lamp's states.
void A3DLamp::Update(A3DViewport* pView)
{
	if (!m_pA3DDevice)
		return;

	A3DCameraBase* pCamera = pView->GetCamera();
	A3DVIEWPORTPARAM* pViewPara = pView->GetParam();
	A3DVECTOR3 vLampPos, vViewPos, vViewDir;
	float fAlphaLimit = 1.0f, fTempAlpha, fCosLamp = 1.0f, fCosView;
	float fDist;
	A3DVECTOR3 vLampToView, vViewToLamp;
	
	m_bDrawLamp		= false;
	m_bDrawBurnout	= false;

	vViewPos	= pCamera->GetPos();
	vViewDir	= pCamera->GetDir();

	//	Calculate light's absolute position
	if (m_dwFlags & LAMPFLAG_FOLLOWVIEW)
		vLampPos = pCamera->GetPos() + m_vLampPos;
	else
		vLampPos = m_vLampPos;

	//	We check obstruct flag only every 5 ticks, so we can save some CPU time
	if ((m_dwTickOff + m_pA3DDevice->GetA3DEngine()->GetRenderTicks()) % 5 == 0)
	{
		//	Ray can pass to view point ?
		A3DCDS* pCDS = m_pA3DDevice->GetA3DEngine()->GetA3DCDS();
		if (pCDS)
			m_bObstruct = !pCDS->PosIsVisible(vLampPos, VISOBJ_LAMP, 0);
		else
			m_bObstruct = false;
	}

	if (m_bObstruct)
	{
		//	Lamp can't be seen from view point.
		if (m_fCurAlpha > 0.0f)
		{
			//	Let lamp disappear gradually
			m_fCurAlpha -= 0.1f;
			if (m_fCurAlpha < 0.0f)
				m_fCurAlpha = 0.0f;

			goto UpdateDecal;
		}

		return;
	}

	//	Direction from lamp to view point
	fDist		= Normalize(vViewPos - vLampPos, vLampToView);
	vViewToLamp = -vLampToView;

	//	Conside distance factor
	if (m_dwFlags & LAMPFLAG_DISTANCE)
	{
		if (fDist > m_fFarDist)
		{
			if( fDist > m_fFarDist * 4.0f )
			{
				//	Let lamp disappear gradually
				if (m_fCurAlpha > 0.0f)
				{
					m_fCurAlpha -= 0.1f;
					if (m_fCurAlpha < 0.0f)
						m_fCurAlpha = 0.0f;
				}
			}
			else
			{
				m_fCurAlpha = 1.0f - (fDist - m_fFarDist) / (3.0f * m_fFarDist);
			}

			m_fCurSize = m_fCurAlpha;
			if (m_fCurAlpha == 0.0f)
				return; // Need not to show
		}
		else if (fDist > m_fNearDist)
		{
			m_fCurSize = (fDist - m_fNearDist) / (m_fFarDist - m_fNearDist);
			m_fCurAlpha = min2(1.0f, m_fCurSize + 0.5f);
			if (m_fCurAlpha == 0.0f)
				return; // Need not to show
		}
		else
		{
			m_fCurSize = 0.0f;
			m_fCurAlpha = 0.0f;
			return; // Need not to show
		}
	}

	//	Treat lamp as spot light ?
	if (m_dwFlags & LAMPFLAG_SPOTLIGHT)
	{
		fCosLamp = DotProduct(vLampToView, m_vLampDir);

		if (fCosLamp < m_fCosSpotPhi)	//	View point out of light radius
		{
			m_fCurAlpha = 0.0f;
			return;
		}
		else if (fCosLamp >= m_fCosSpotTheta)
			fTempAlpha = 1.0f;
		else
			fTempAlpha = (fCosLamp - m_fCosSpotPhi) / (m_fCosSpotTheta - m_fCosSpotPhi);

		fAlphaLimit *= fTempAlpha;
		m_fCurSize	*= fTempAlpha;
	}

	fCosView = DotProduct(vViewToLamp, vViewDir);

	//	Conside lamp's position in view cone ?
	if (m_dwFlags & LAMPFLAG_VIEWCONE)
	{
		if (fCosView < m_fCosViewPhi)	//	Lamp point out of view cone radius
		{
			m_fCurAlpha = 0.0f;
			return;
		}
		else if (fCosView >= m_fCosViewTheta)
			fTempAlpha = 1.0f;
		else
			fTempAlpha = (fCosView - m_fCosViewPhi) / (m_fCosViewTheta - m_fCosViewPhi);

		fAlphaLimit *= fTempAlpha;
	}

	if (m_fCurAlpha < fAlphaLimit)
		m_fCurAlpha += 0.1f;
	
	if (m_fCurAlpha > fAlphaLimit)
		m_fCurAlpha = fAlphaLimit;

UpdateDecal:

	m_bDrawLamp = true;

	//	Calculate shine's color
	BYTE r, g, b;
	r = (BYTE)(A3DCOLOR_GETRED(m_Color) * m_fCurAlpha);
	g = (BYTE)(A3DCOLOR_GETGREEN(m_Color) * m_fCurAlpha);
	b = (BYTE)(A3DCOLOR_GETBLUE(m_Color) * m_fCurAlpha);

	m_ShineDecal.SetPos(vLampPos.x, vLampPos.y, vLampPos.z);
	m_ShineDecal.SetColor(A3DCOLORRGB(r, g, b));
	m_ShineDecal.SetSize(m_fWidth * m_fCurSize, m_fHeight * m_fCurSize);

	//	Do lens flare effect ?
	if (m_pLensFlare)
	{
		if (m_dwFlags & LAMPFLAG_VIEWCONE)
			m_pLensFlare->Update(pView, vLampPos, fCosView, m_fCosViewTheta, m_fCosViewPhi, m_bObstruct);
		else if (m_dwFlags & LAMPFLAG_SPOTLIGHT)
			m_pLensFlare->Update(pView, vLampPos, fCosView, m_fCosSpotTheta, m_fCosSpotPhi, m_bObstruct);
		else
			m_pLensFlare->Update(pView, vLampPos, fCosView, 0.86f, 0.7f, m_bObstruct);
	}
	
	//	Do burnout effect ?
	if (m_dwFlags & LAMPFLAG_BURNOUT)
	{
		bool bFlag = false;
		float fLow;

		if (m_dwFlags & LAMPFLAG_SPOTLIGHT)
		{
			if (fCosLamp > 0.92f && fCosView > 0.92f)	//	< 22.5 deg
			{
				fLow  = fCosLamp * fCosView;
				bFlag = true;
			}
		}
		else if (fCosView > 0.92f)
		{
			fLow  = 0.92f;
			bFlag = true;
		}

		if (bFlag)
		{
			m_bDrawBurnout = true;

			m_BurnoutDecal.SetPos((float)(pViewPara->Width >> 1), (float)(pViewPara->Height >> 1), 0.0f);
			m_BurnoutDecal.SetSize((float)(pViewPara->Width >> 1), (float)(pViewPara->Height >> 1));

			float fAlpha = (fCosView - fLow) / (1.0f - fLow);
			BYTE byCol	 = (BYTE)(128.0f * fAlpha);
			m_BurnoutDecal.SetColor(A3DCOLORRGB(byCol, byCol, byCol));
		}
	}
}

//	Render lamp
bool A3DLamp::Render(A3DViewport* pView)
{
	if (!m_pA3DDevice)
		return true;

	Update(pView);

	if (m_bDrawLamp)
	{
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCCOLOR);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_ONE);
	
		m_ShineDecal.Render(pView, false);

		if (m_pLensFlare)
			m_pLensFlare->Render(pView);

		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	}

	if (m_bDrawBurnout)
		m_BurnoutDecal.Render(pView, false);

	return true;
}




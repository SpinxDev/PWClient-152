/*
 * FILE: A3DLensFlare.cpp
 *
 * DESCRIPTION: Routines for Lens Flare effect
 *
 * CREATED BY: duyuxin, 2001/11/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DLensFlare.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DWorld.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DDecal.h"
#include "A3DCamera.h"
#include "A3DConfig.h"
#include "A3DCDS.h"
#include "A3DEngine.h"

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

A3DLensFlare::A3DLensFlare()
{
	m_pA3DDevice	= NULL;
	m_aFlares		= NULL;
	m_pA3DDevice	= NULL;
	m_bBurnout		= false;
	m_fRealAlpha	= 1.0f;
	m_bUpdateOK		= false;
}

A3DLensFlare::~A3DLensFlare()
{
}

/*	Initialize object.

	Return true for success, otherwise return false.

	pDevice: valid device pointer.
	szBaseName: base name of flare texture files. for example, base name is "Flare.bmp",
				then "Flare0.bmp", "Flare1.bmp" ... "Flare5.bmp" will be used as flares'
				texture.
*/
bool A3DLensFlare::Init(A3DDevice* pDevice, char* szBaseName)
{
	if (!pDevice || g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
		return true;

	m_pA3DDevice = pDevice;

	//	Dynamic allocate memory for m_aFlares just for convenient extending in future
	m_iNumFlares = 9;

	m_aFlares = (PFLARE)malloc(m_iNumFlares * sizeof (FLARE));
	if (!m_aFlares)
		return false;

	memset(m_aFlares, 0, m_iNumFlares * sizeof (FLARE));

	if (!CreateDecals(pDevice, szBaseName))
		return false;

	//	Set default parameters for flares
	SetFlare(0, 0.09f, 60, A3DCOLORRGB(128, 90, 0));
	SetFlare(1, 0.07f, 48, A3DCOLORRGB(128, 128, 128));
	SetFlare(2, 0.03f, 30, A3DCOLORRGB(128, 128, 128));
	SetFlare(3, 0.12f, 20, A3DCOLORRGB(128, 120, 0));
	SetFlare(4, 0.03f, 16, A3DCOLORRGB(128, 128, 128));
	SetFlare(5, 0.08f, 7, A3DCOLORRGB(128, 128, 128));
	SetFlare(6, 0.03f, -1, A3DCOLORRGB(128, 128, 128));
	SetFlare(7, 0.15f, -5, A3DCOLORRGB(128, 80, 110));
	SetFlare(8, 0.3f, 0, A3DCOLORRGB(100, 0, 0));

	return true;
}

/*	Create decals.

	Return true for success, otherwise return false.

	pDevice: A3DDevice object.
	szBaseName: base name of flare texture files. for example, base name is "Flare.bmp",
				then "Flare0.bmp", "Flare1.bmp" ... "Flare5.bmp" will be used as flares'
				texture.
*/
bool A3DLensFlare::CreateDecals(A3DDevice* pDevice, char* szBaseName)
{
	if( !m_pA3DDevice )	return true;

	A3DSHADER Shader = {A3DBLEND_SRCCOLOR, A3DBLEND_ONE};

	//	Create decals
	for (int i=0; i < m_iNumFlares; i++)
	{
		if (!(m_aFlares[i].pDecal = new A3DDecal))
		{
			g_A3DErrLog.Log("A3DLensFlare::Init() create decal failed!");
			Release();
			return false;
		}

		m_aFlares[i].pDecal->SetShader(Shader);
	}

	//	Load textures
	char szTexName[256], *pTemp;
	int iLen;

	pTemp = strrchr(szBaseName, '.');
	iLen  = pTemp - szBaseName;
	
	strncpy(szTexName, szBaseName, iLen);
	szTexName[iLen]		= '0';
	szTexName[iLen+1]	= '\0';
	strcat(szTexName, pTemp);

	if (!m_aFlares[6].pDecal->Init(DECALTYPE_CENTER3D, pDevice, szTexName))
	{
		Release();
		return false;
	}

	szTexName[iLen] = '1';
	if (!m_aFlares[4].pDecal->Init(DECALTYPE_CENTER3D, pDevice, szTexName))
	{
		Release();
		return false;
	}

	szTexName[iLen] = '2';
	if (!m_aFlares[0].pDecal->Init(DECALTYPE_CENTER3D, pDevice, szTexName))
	{
		Release();
		return false;
	}

	szTexName[iLen] = '3';
	if (!m_aFlares[2].pDecal->Init(DECALTYPE_CENTER3D, pDevice, szTexName))
	{
		Release();
		return false;
	}

	szTexName[iLen] = '4';
	if (!m_aFlares[1].pDecal->Init(DECALTYPE_CENTER3D, pDevice, szTexName) ||
		!m_aFlares[3].pDecal->Init(DECALTYPE_CENTER3D, pDevice, szTexName) ||
		!m_aFlares[5].pDecal->Init(DECALTYPE_CENTER3D, pDevice, szTexName) ||
		!m_aFlares[7].pDecal->Init(DECALTYPE_CENTER3D, pDevice, szTexName) ||
		!m_aFlares[8].pDecal->Init(DECALTYPE_CENTER3D, pDevice, szTexName))
	{
		Release();
		return false;
	}

	return true;
}

//	Release object's resource
void A3DLensFlare::Release()
{
	if( !m_pA3DDevice )		return;

	if (m_aFlares)
	{
		for (int i=0; i < m_iNumFlares; i++)
		{
			if (m_aFlares[i].pDecal)
			{
				m_aFlares[i].pDecal->Release();
				delete m_aFlares[i].pDecal;
			}
		}

		free(m_aFlares);

		m_aFlares	 = NULL;
		m_iNumFlares = 0;
	}

	m_pA3DDevice = NULL;
}

/*	Set color of a flare.

	iIndex: index of flare whose color will be set.
	Color: flare's color.
*/
void A3DLensFlare::SetFlareCol(int iIndex, A3DCOLOR Color)
{
	if( !m_pA3DDevice )	return;

	if (iIndex < 0 || iIndex >= m_iNumFlares)
		return;

	PFLARE pFlare = &m_aFlares[iIndex];

	pFlare->r	= A3DCOLOR_GETRED(Color);
	pFlare->g	= A3DCOLOR_GETGREEN(Color);
	pFlare->b	= A3DCOLOR_GETBLUE(Color);
}

/*	Set a flare's property.

	iIndex: index of flare whose color will be set.
	fSize: flare's relative size
	fPos: relative position to center point
	Color: flare's color.
*/
void A3DLensFlare::SetFlare(int iIndex, FLOAT fSize, FLOAT fPos, A3DCOLOR Color)
{
	if( !m_pA3DDevice )	return;

	if (iIndex < 0 || iIndex >= m_iNumFlares)
		return;

	PFLARE pFlare = &m_aFlares[iIndex];

	pFlare->fSize	= fSize;
	pFlare->fPos	= fPos;
	pFlare->r		= A3DCOLOR_GETRED(Color);
	pFlare->g		= A3DCOLOR_GETGREEN(Color);
	pFlare->b		= A3DCOLOR_GETBLUE(Color);
}

/*	Update flares' information (include real size and position) with specified viewport.

	pView: specified viewport.
*/
void A3DLensFlare::Update(A3DViewport* pView)
{
	if (!m_pA3DDevice)
		return;

	m_bUpdateOK = false;

	if (pView->GetCamera()->GetClassID() != A3D_CID_CAMERA)
		return;

	A3DCamera* pCamera = (A3DCamera*)pView->GetCamera();
	A3DVIEWPORTPARAM* pViewPara = pView->GetParam();
	A3DVECTOR3 vPos, vViewToLight, vViewPos, vViewDir, vCenterPos;
	float fSize, fCos, fLowLimit, fCosAngle;

	//	Light's real position
	vPos = pCamera->GetPos() + m_vLightPos;
	fCos = (float)cos(pCamera->GetFOV() * 0.5f);

	vViewPos = pCamera->GetPos();
	vViewDir = pCamera->GetDir();

	//	Center position is a little in front of camera
	vCenterPos	 = vViewPos + vViewDir * 15.0f;
	m_vRayDir	 = Normalize(vPos - vCenterPos);
	vViewToLight = Normalize(vPos - vViewPos);
	fCosAngle	 = DotProduct(m_vRayDir, vViewDir);

	if (fCosAngle > (fLowLimit = fCos * 0.7f))
	{
		//	Check whether ray is obstructed by something
		A3DCDS* pCDS = m_pA3DDevice->GetA3DEngine()->GetA3DCDS();
		if (pCDS && pCDS->PosIsVisible(m_vLightPos, VISOBJ_LAMP, 0))
		{
			m_fRealAlpha  = 0.0f;
			m_bDrawFlares = false;
		}
		else
		{
			m_bDrawFlares = true;
			
			BYTE r, g, b;
			PFLARE pFlare;
			float fAlphaLimit, fHighLimit = fCos * 0.8f;
			
			fAlphaLimit = (fCosAngle - fLowLimit) / (fHighLimit - fLowLimit);
			if (fAlphaLimit > 1.0f)
				fAlphaLimit = 1.0f;
			
			if (m_fRealAlpha < fAlphaLimit)
				m_fRealAlpha += 0.1f;
			
			if (m_fRealAlpha > fAlphaLimit)
				m_fRealAlpha = fAlphaLimit;
	
			pFlare = m_aFlares;

			//	Update flares
			for (int i=0; i < m_iNumFlares; i++, pFlare++)
			{
				vPos  = vCenterPos + m_vRayDir * pFlare->fPos;
				fSize = pFlare->fSize * pViewPara->Width;
				
				pFlare->pDecal->SetPos(vPos.x, vPos.y, vPos.z);
				pFlare->pDecal->SetSize(fSize, fSize);
				
				//	Color
				r = (BYTE)(pFlare->r * m_fRealAlpha);
				g = (BYTE)(pFlare->g * m_fRealAlpha);
				b = (BYTE)(pFlare->b * m_fRealAlpha);
				pFlare->pDecal->SetColor(A3DCOLORRGB(r, g, b));
			}
		}
	}
	else
		m_bDrawFlares = false;

	m_bUpdateOK = true;
}

/*	Updating function used by A3DLamp class.

	pView: current viewport information
	vLightPos: absolute light position
	fCosAngle: cosine of angle between view and light
	fCosInter: cosine of view's inter cone
	fCosOuter: cosine of view's outer cone
	bObstruct: true if ray was obstructed by something
*/
void A3DLensFlare::Update(A3DViewport* pView, A3DVECTOR3& vLightPos, FLOAT fCosAngle, 
						  FLOAT fCosInter, FLOAT fCosOuter, bool bObstruct)
{
	if (!m_pA3DDevice)
		return;

	A3DCameraBase* pCamera = pView->GetCamera();
	A3DVIEWPORTPARAM* pViewPara = pView->GetParam();
	A3DVECTOR3 vPos, vViewPos, vViewDir, vCenterPos;
	float fSize;

	vViewPos = pCamera->GetPos();
	vViewDir = pCamera->GetDir();

	//	Center position is a little in front of camera
	vCenterPos	= vViewPos + vViewDir * 15.0f;
	m_vRayDir	= Normalize(vLightPos - vCenterPos);

	if (fCosAngle > fCosOuter)
	{
		BYTE r, g, b;
		PFLARE pFlare;
		float fAlphaLimit;

		if (bObstruct)	//	Ray is obstructed by something ?
		{
			if (m_fRealAlpha > 0.0f)
			{
				m_fRealAlpha -= 0.1f;
				
				if (m_fRealAlpha < 0.0f)
					m_fRealAlpha = 0.0f;
			}
			else
				m_fRealAlpha = 0.0f;

			if (m_fRealAlpha != 0.0f)
				goto UpdateDecal;

			m_bDrawFlares = false;
			return;
		}

		m_bDrawFlares = true;
			
		fAlphaLimit = (fCosAngle - fCosOuter) / (fCosInter - fCosOuter);
		if (fAlphaLimit > 1.0f)
			fAlphaLimit = 1.0f;
		
		if (m_fRealAlpha < fAlphaLimit)
			m_fRealAlpha += 0.1f;
		
		if (m_fRealAlpha > fAlphaLimit)
			m_fRealAlpha = fAlphaLimit;
		
UpdateDecal:

		pFlare = m_aFlares;
		
		//	Update flares
		for (int i=0; i < m_iNumFlares; i++, pFlare++)
		{
			vPos  = vCenterPos + m_vRayDir * pFlare->fPos;
			fSize = pFlare->fSize * pViewPara->Width;
			
			pFlare->pDecal->SetPos(vPos.x, vPos.y, vPos.z);
			pFlare->pDecal->SetSize(fSize, fSize);
			
			//	Color
			r = (BYTE)(pFlare->r * m_fRealAlpha);
			g = (BYTE)(pFlare->g * m_fRealAlpha);
			b = (BYTE)(pFlare->b * m_fRealAlpha);
			pFlare->pDecal->SetColor(A3DCOLORRGB(r, g, b));
		}
	}
	else
		m_bDrawFlares = false;

	m_bUpdateOK = true;
}

//	Render flares
bool A3DLensFlare::Render(A3DViewport* pView)
{
	if (!m_pA3DDevice || !m_bUpdateOK)
		return true;

	if (!m_pA3DDevice->GetA3DEngine()->IsOptimizedEngine())
	{
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCCOLOR);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_ONE);
	}

	if (m_bDrawFlares)
	{
		for (int i=0; i < m_iNumFlares; i++)
			m_aFlares[i].pDecal->Render(pView, false);
	}

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	return true;
}



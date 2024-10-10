/*
 * FILE: A3DLightingEngine.cpp
 *
 * DESCRIPTION: Soft lighting routines
 *
 * CREATED BY: duyuxin, 2001/12/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DLightingEngine.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DLightMan.h"
#include "A3DEngine.h"
#include "A3DLight.h"
#include "A3DDevice.h"
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

A3DLightingEngine::A3DLightingEngine()
{
	m_pA3DEngine	= NULL;

	m_aFogTab		= NULL;
	m_FogMode		= A3DFOG_NONE;
	m_fViewField	= 0.0f;
	m_fStepLen		= 0.0f;
	m_iNumStep		= 0;
	m_fNear			= 0.0f;
	m_fFar			= 0.0f;
	m_fDensity		= 0.0f;

	m_iNumStaticLight	= 0;
	m_iNumDynamicLight	= 0;
}

A3DLightingEngine::~A3DLightingEngine()
{
}

/*	Initialize object.

	Return true for success, otherwise return false.

	pA3DEngine: valid address of A3D engine
*/
bool A3DLightingEngine::Init(A3DEngine*	pA3DEngine)
{
	m_pA3DEngine = pA3DEngine;

	//	Update static light list
	UpdateStaticLights();

	return true;
}

//	Release resources
void A3DLightingEngine::Release()
{
	if (m_aFogTab)
	{
		free(m_aFogTab);
		m_aFogTab = NULL;
	}

	m_pA3DEngine = NULL;
}

/*	Build fog table.

	Return true for success, otherwise return false.

	FogMode: fog mode, if mode is A3DFOG_NONE, old fog table will be released.
	fNear: the distance fog begin appear
	fFar: the distance fog is most dense
	fDensity: fog's density
	fViewField: distance of view field 
	fStep: sample step length
*/
bool A3DLightingEngine::BuildFogTable(A3DFOGMODE FogMode, float fNear, float fFar, 
									  float fDensity, float fViewField, float fStep)
{
	if (FogMode == A3DFOG_NONE)
	{
		if (m_aFogTab)
		{
			free(m_aFogTab);
			m_aFogTab = NULL;
		}

		m_FogMode = FogMode;
		return true;
	}

	if (fViewField <= 0.0f || fStep <= 0.0f)
		return false;

	m_iNumStep = (int)ceil(fViewField / fStep);

	if (!(m_aFogTab = (BYTE*)malloc(m_iNumStep * m_iNumStep)))
	{
		g_A3DErrLog.Log("A3DLightingEngine::BuildFogTable(), not enough memory");
		return false;
	}

	m_FogMode		= FogMode;
	m_fNear			= fNear;
	m_fFar			= fFar;
	m_fViewField	= fViewField;
	m_fStepLen		= fStep;
	m_fDensity		= fDensity;
	m_fInvStepLen	= 1.0f / m_fStepLen;

	//	Fill fog table
	int i, j;
	float x, z, fDist, fFactor;
	BYTE* pLine = m_aFogTab;

	for (i=0; i < m_iNumStep; i++)
	{
		for (j=0; j < m_iNumStep; j++)
		{
			x = i * m_fStepLen;
			z = j * m_fStepLen;

			fDist = (float)sqrt(x * x + z * z);
			if (fDist < fNear)
				fFactor = 1.0f;
			else if (FogMode == A3DFOG_LINEAR)
				fFactor = (fFar - fFactor) / (fFar - fNear);
			else if (FogMode == A3DFOG_EXP)
			{
				fFactor = (fDist - fNear) / (fFar - fNear) * fDensity;
				fFactor = (float)(1.0 / exp(fFactor));
			}
			else if (FogMode == A3DFOG_EXP2)
			{
				fFactor = (fDist - fNear) / (fFar - fNear) * fDensity;
				fFactor = (float)(1.0 / exp(fFactor * fFactor));
			}

			a_Clamp(fFactor, 0.0f, 1.0f);
			pLine[j] = (BYTE)(fFactor * 255.0f);
		}

		pLine += m_iNumStep;
	}

	return true;
}

//	Update static lights. Call this function only if static lights change.
//	Note: after call this function, dynamic light should be reset
void A3DLightingEngine::UpdateStaticLights()
{
	AList *pList = m_pA3DEngine->GetA3DLightMan()->GetLightList();
	if (!pList)
		return;

	A3DLight* pLight;

	m_iNumStaticLight = 0;

	ALISTELEMENT* pElem = pList->GetFirst();
	while (pElem != pList->GetTail())
	{
		pLight = (A3DLight*)pElem->pData;
		if (!pLight->IsOn())
			continue;

		if (m_iNumStaticLight >= MAXNUM_LIGHT)
		{
			//	This shouldn't happen
			g_A3DErrLog.Log("A3DLightingEngine::UpdateStaticLights(), too many static lights");
			return;
		}

		m_aLights[m_iNumStaticLight++] = pLight->GetLightparam();

		pElem = pElem->pNext;
	}

	m_iNumDynamicLight = 0;
}

/*	Set new dynamic lights, this operation will clear current dynamic lights

	pList: dynamic light list, if this parameter is NULL, function only clear current
		   dynamic lights.
*/	
void A3DLightingEngine::SetDynamicLights(AList* pList)
{
	if (!pList)
		return;

	A3DLight* pLight;

	m_iNumDynamicLight = 0;

	ALISTELEMENT* pElem = pList->GetFirst();
	while (pElem != pList->GetTail())
	{
		pLight = (A3DLight*)pElem->pData;
		if (!pLight->IsOn())
			continue;

		if (m_iNumStaticLight + m_iNumDynamicLight >= MAXNUM_LIGHT)
		{
			//	This shouldn't happen
			g_A3DErrLog.Log("A3DLightingEngine::SetDynamicLights(), too many lights");
			return;
		}

		m_aLights[m_iNumStaticLight + m_iNumDynamicLight++] = pLight->GetLightparam();

		pElem = pElem->pNext;
	}
}

/*	Add dynamic lights. This operation will add new dynamic lights.

	pList: new dynamic light list
*/
void A3DLightingEngine::AddDynamicLights(AList* pList)
{
	if (!pList)
		return;

	A3DLight* pLight;

	ALISTELEMENT* pElem = pList->GetFirst();
	while (pElem != pList->GetTail())
	{
		pLight = (A3DLight*)pElem->pData;
		if (!pLight->IsOn())
			continue;

		if (m_iNumStaticLight + m_iNumDynamicLight >= MAXNUM_LIGHT)
		{
			//	This shouldn't happen
			g_A3DErrLog.Log("A3DLightingEngine::SetDynamicLights(), too many lights");
			return;
		}

		m_aLights[m_iNumStaticLight + m_iNumDynamicLight++] = pLight->GetLightparam();

		pElem = pElem->pNext;
	}
}

/*	Illuminate vertices.

	Return true for success, otherwise return false.

	aSrcVerts: source vertices will be illuminated
	iNumVert: number of vertex
	mat: transform matrix used to transfrom vertices from object coordinates to world coordinates
	pMaterial: material of vertices
	aDestVerts: buffer used to store illuminated vertices.
*/
bool A3DLightingEngine::Illuminate(A3DVERTEX* aSrcVerts, int iNumVert, A3DMATRIX4& mat, 
									A3DMATERIALPARAM* pMaterial, A3DLVERTEX* aDestVerts)
{
	A3DVECTOR3 vLDir, vH, vViewDir, vVertPos, vNormal;
	A3DCOLORVALUE Ambient, Diffuse(0.0f), Specular(0.0f), Diff(0.0f), Spec(0.0f);
	A3DCOLORVALUE GlobalAmb;
	float rd, rs, fDist, fIntensity=1.0f;
	int i, j, iNumLight;
	A3DLIGHTPARAM* pLight;
	BYTE byFog;
	
	Ambient	  = m_pA3DEngine->GetA3DDevice()->GetAmbientValue();
	iNumLight = m_iNumStaticLight + m_iNumDynamicLight;

	//	Calculate some constant value at first
	for (i=0; i < iNumLight; i++)
	{
		pLight = &m_aLights[i];

		m_aLtValues[i].Ambient.r	= pMaterial->Ambient.r * pLight->Ambient.r;
		m_aLtValues[i].Ambient.g	= pMaterial->Ambient.g * pLight->Ambient.g;
		m_aLtValues[i].Ambient.b	= pMaterial->Ambient.b * pLight->Ambient.b;
		m_aLtValues[i].Ambient.a	= pMaterial->Ambient.a * pLight->Ambient.a;

		m_aLtValues[i].Diffuse.r	= pMaterial->Diffuse.r * pLight->Diffuse.r;
		m_aLtValues[i].Diffuse.g	= pMaterial->Diffuse.g * pLight->Diffuse.g;
		m_aLtValues[i].Diffuse.b	= pMaterial->Diffuse.b * pLight->Diffuse.b;
		m_aLtValues[i].Diffuse.a	= pMaterial->Diffuse.a * pLight->Diffuse.a;

		m_aLtValues[i].Specular.r	= pMaterial->Specular.r * pLight->Specular.r;
		m_aLtValues[i].Specular.g	= pMaterial->Specular.g * pLight->Specular.g;
		m_aLtValues[i].Specular.b	= pMaterial->Specular.b * pLight->Specular.b;
		m_aLtValues[i].Specular.a	= pMaterial->Specular.a * pLight->Specular.a;
	}

	GlobalAmb.r	= Ambient.r * pMaterial->Ambient.r + pMaterial->Emissive.r;
	GlobalAmb.g	= Ambient.g * pMaterial->Ambient.g + pMaterial->Emissive.g;
	GlobalAmb.b	= Ambient.b * pMaterial->Ambient.b + pMaterial->Emissive.b;
	GlobalAmb.a	= Ambient.a * pMaterial->Ambient.a + pMaterial->Emissive.a;

	for (i=0; i < iNumVert; i++)
	{
		Diffuse  = A3DCOLORVALUE(0.0f);
		Specular = A3DCOLORVALUE(0.0f);
		
		//	Transform vertex from object space to world space
		aDestVerts[i].x	 = mat._11*aSrcVerts[i].x + mat._21*aSrcVerts[i].y + mat._31*aSrcVerts[i].z + mat._41;
		aDestVerts[i].y	 = mat._12*aSrcVerts[i].x + mat._22*aSrcVerts[i].y + mat._32*aSrcVerts[i].z + mat._42;
		aDestVerts[i].z	 = mat._13*aSrcVerts[i].x + mat._23*aSrcVerts[i].y + mat._33*aSrcVerts[i].z + mat._43;
		aDestVerts[i].tu = aSrcVerts[i].tu;
		aDestVerts[i].tv = aSrcVerts[i].tv;

		vNormal.x	= mat._11*aSrcVerts[i].nx + mat._21*aSrcVerts[i].ny + mat._31*aSrcVerts[i].nz;
		vNormal.y	= mat._12*aSrcVerts[i].nx + mat._22*aSrcVerts[i].ny + mat._32*aSrcVerts[i].nz;
		vNormal.z	= mat._13*aSrcVerts[i].nx + mat._23*aSrcVerts[i].ny + mat._33*aSrcVerts[i].nz;

		vVertPos = A3DVECTOR3(aDestVerts[i].x, aDestVerts[i].y, aDestVerts[i].z);
		fDist	 = Normalize(vVertPos - m_vEyePos, vViewDir);
		
		for (j=0; j < iNumLight; j++)
		{
			pLight = &m_aLights[j];

			if (pLight->Type == A3DLIGHT_POINT)
				vLDir = Normalize(vVertPos, pLight->Position);
			else if (pLight->Type == A3DLIGHT_DIRECTIONAL)
				vLDir = -pLight->Direction;
			
			rd = DotProduct(vNormal, vLDir);
			if (rd <= 0.0f)
			{
				Diffuse.r += fIntensity * m_aLtValues[j].Ambient.r;
				Diffuse.g += fIntensity * m_aLtValues[j].Ambient.g;
				Diffuse.b += fIntensity * m_aLtValues[j].Ambient.b;
				Diffuse.a += fIntensity * m_aLtValues[j].Ambient.a;
				continue;
			}
			
			Diff.r = fIntensity * (rd * m_aLtValues[j].Diffuse.r + m_aLtValues[j].Ambient.r);
			Diff.g = fIntensity * (rd * m_aLtValues[j].Diffuse.g + m_aLtValues[j].Ambient.g);
			Diff.b = fIntensity * (rd * m_aLtValues[j].Diffuse.b + m_aLtValues[j].Ambient.b);
			Diff.a = fIntensity * (rd * m_aLtValues[j].Diffuse.a + m_aLtValues[j].Ambient.a);

			Diffuse.r	+= Diff.r;
			Diffuse.g	+= Diff.g;
			Diffuse.b	+= Diff.b;
			Diffuse.a	+= Diff.a;
			
			//	Specular
			if (pMaterial->Power != 0.0f)
			{
				vH = Normalize(vLDir - vViewDir);
				rs = (float)pow(DotProduct(vH, vNormal), pMaterial->Power) * fIntensity;
				
				Spec.r = rs * m_aLtValues[j].Specular.r;
				Spec.g = rs * m_aLtValues[j].Specular.g;
				Spec.b = rs * m_aLtValues[j].Specular.b;
				Spec.a = rs * m_aLtValues[j].Specular.a;
				
				Specular.r	+= Spec.r;
				Specular.g	+= Spec.g;
				Specular.b	+= Spec.b;
				Specular.a	+= Spec.a;
			}
		}

		Diffuse.r += GlobalAmb.r;
		Diffuse.g += GlobalAmb.g;
		Diffuse.b += GlobalAmb.b;
		Diffuse.a += GlobalAmb.a;

		//	Clamp colors
		a_ClampRoof(Diffuse.r, 1.0f);
		a_ClampRoof(Diffuse.g, 1.0f);
		a_ClampRoof(Diffuse.b, 1.0f);
		a_ClampRoof(Diffuse.a, 1.0f);
		
		a_ClampRoof(Specular.r, 1.0f);
		a_ClampRoof(Specular.g, 1.0f);
		a_ClampRoof(Specular.b, 1.0f);
		a_ClampRoof(Specular.a, 1.0f);
		
		byFog = CalculateFogFactor(vVertPos);

		aDestVerts[i].diffuse = A3DCOLORRGBA((int)(Diffuse.r * 255.0f), (int)(Diffuse.g * 255.0f),
											(int)(Diffuse.b * 255.0f), (int)(Diffuse.a * 255.0f));
		aDestVerts[i].specular = A3DCOLORRGBA((int)(Specular.r * 255.0f), (int)(Specular.g * 255.0f),
											(int)(Specular.b * 255.0f), byFog);
	}

	return true;
}



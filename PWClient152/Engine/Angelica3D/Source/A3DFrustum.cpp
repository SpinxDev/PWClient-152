/*
 * FILE: A3DFrustum.cpp
 *
 * DESCRIPTION: Frustum class
 *
 * CREATED BY: duyuxin, 2003/6/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DFrustum.h"
#include "A3DCameraBase.h"
#include "A3DViewport.h"
#include "A3DCollision.h"

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
//	Implement A3DFrustum
//
///////////////////////////////////////////////////////////////////////////

A3DFrustum::A3DFrustum()
{
}

//	Initialize object
bool A3DFrustum::Init(int iNumPlane)
{
	if (iNumPlane >= 0)
	{
		m_aPlanes.SetSize(iNumPlane, 10);

		for (int i=0; i < iNumPlane; i++)
			m_aPlanes[i].bEnable = true;
	}

	return true;
}

/*	Create a view frustum.

	Return true for success, otherwise return false.

	pCamera: camera object.
	fLeft, fTop, fRight, fBottom: project rectangle of frustum in identity view space.
			-1.0f, 1.0f, 1.0f, -1.0f, means whole view space.
	bZClip: create near and far clip planes on z axis
*/
bool A3DFrustum::Init(A3DCameraBase* pCamera, float fLeft, float fTop, float fRight, float fBottom, bool bZClip)
{
	if (bZClip)
	{
		m_aPlanes.SetSize(6, 5);

		for (int i=0; i < 6; i++)
			m_aPlanes[i].bEnable = true;
	}
	else
	{
		m_aPlanes.SetSize(4, 5);

		for (int i=0; i < 4; i++)
			m_aPlanes[i].bEnable = true;
	}

	A3DVECTOR3 vLT(fLeft, fTop, 0.5f);
	A3DVECTOR3 vLB(fLeft, fBottom, 0.5f);
	A3DVECTOR3 vRT(fRight, fTop, 0.5f);
	A3DVECTOR3 vRB(fRight, fBottom, 0.5f);

	//	Convert to world space
	pCamera->InvTransform(vLT, vLT);
	pCamera->InvTransform(vLB, vLB);
	pCamera->InvTransform(vRT, vRT);
	pCamera->InvTransform(vRB, vRB);

	A3DVECTOR3 vEye = pCamera->GetPos();

	CreateClipPlane(vEye, vLT, vLB, GetPlane(VF_LEFT));
	CreateClipPlane(vEye, vRB, vRT, GetPlane(VF_RIGHT));
	CreateClipPlane(vEye, vRT, vLT, GetPlane(VF_TOP));
	CreateClipPlane(vEye, vLB, vRB, GetPlane(VF_BOTTOM));

	if (bZClip)
	{
		A3DVECTOR3 vDir = pCamera->GetDir();

		//	Create near plane
		A3DVECTOR3 vPos = vEye + vDir * pCamera->GetZFront();
		A3DSPLANE* pPlane = GetPlane(VF_NEAR);

		pPlane->vNormal	= vDir;
		pPlane->fDist	= DotProduct(pPlane->vNormal, vPos);
		pPlane->byType	= A3DSPLANE::TYPE_UNKNOWN;
		pPlane->MakeSignBits();
		
		//	Create far plane
		vPos = vEye + vDir * pCamera->GetZBack();
		pPlane = GetPlane(VF_FAR);
		
		pPlane->vNormal	= -vDir;
		pPlane->fDist	= DotProduct(pPlane->vNormal, vPos);
		pPlane->byType	= A3DSPLANE::TYPE_UNKNOWN;
		pPlane->MakeSignBits();
	}

	return true;
}

/*	Create a view frustum.

	Return true for success, otherwise return false.

	pView: viewport object.
	iLeft, iTop, iRight, iBottom: project rectangle of frustum on screen.
			0, 0, screen width, screen height, means whole screen.
	bZClip: create near and far clip planes on z axis
*/
bool A3DFrustum::Init(A3DViewport* pView, int iLeft, int iTop, int iRight, int iBottom, bool bZClip)
{
	if (bZClip)
	{
		m_aPlanes.SetSize(6, 5);

		for (int i=0; i < 6; i++)
			m_aPlanes[i].bEnable = true;
	}
	else
	{
		m_aPlanes.SetSize(4, 5);

		for (int i=0; i < 4; i++)
			m_aPlanes[i].bEnable = true;
	}

	A3DVECTOR3 vLT((float)iLeft, (float)iTop, 0.5f);
	A3DVECTOR3 vLB((float)iLeft, (float)iBottom, 0.5f);
	A3DVECTOR3 vRT((float)iRight, (float)iTop, 0.5f);
	A3DVECTOR3 vRB((float)iRight, (float)iBottom, 0.5f);

	//	Convert to world space
	pView->InvTransform(vLT, vLT);
	pView->InvTransform(vLB, vLB);
	pView->InvTransform(vRT, vRT);
	pView->InvTransform(vRB, vRB);

	A3DCameraBase* pCamera = pView->GetCamera();
	A3DVECTOR3 vEye = pCamera->GetPos();

	CreateClipPlane(vEye, vLT, vLB, GetPlane(VF_LEFT));
	CreateClipPlane(vEye, vRB, vRT, GetPlane(VF_RIGHT));
	CreateClipPlane(vEye, vRT, vLT, GetPlane(VF_TOP));
	CreateClipPlane(vEye, vLB, vRB, GetPlane(VF_BOTTOM));

	if (bZClip)
	{
		A3DVECTOR3 vDir = pCamera->GetDir();

		//	Create near plane
		A3DVECTOR3 vPos = vEye + vDir * pCamera->GetZFront();
		A3DSPLANE* pPlane = GetPlane(VF_NEAR);

		pPlane->vNormal	= vDir;
		pPlane->fDist	= DotProduct(pPlane->vNormal, vPos);
		pPlane->byType	= A3DSPLANE::TYPE_UNKNOWN;
		pPlane->MakeSignBits();
		
		//	Create far plane
		vPos = vEye + vDir * pCamera->GetZBack();
		pPlane = GetPlane(VF_FAR);
		
		pPlane->vNormal	= -vDir;
		pPlane->fDist	= DotProduct(pPlane->vNormal, vPos);
		pPlane->byType	= A3DSPLANE::TYPE_UNKNOWN;
		pPlane->MakeSignBits();
	}

	return true;
}

//	Release object
void A3DFrustum::Release()
{
	m_aPlanes.RemoveAll();
}

//	Create clip plane
void A3DFrustum::CreateClipPlane(const A3DVECTOR3& v0, const A3DVECTOR3& v1, const A3DVECTOR3& v2, A3DSPLANE* pPlane)
{
	pPlane->CreatePlane(v0, v1, v2);

	pPlane->byType = A3DSPLANE::TYPE_UNKNOWN;
	pPlane->MakeSignBits();
}

//	Add a clip plane
bool A3DFrustum::AddPlane(const A3DSPLANE& Plane)
{
	PLANE p;

	p.Plane		= Plane;
	p.bEnable	= true;

	m_aPlanes.Add(p);
	return true;
}

//	Add a clip plane
bool A3DFrustum::AddPlane(const A3DVECTOR3& vNormal, float fDist)
{
	PLANE p;
	
	p.Plane.vNormal	= vNormal;
	p.Plane.fDist	= fDist;
	p.Plane.byType	= A3DSPLANE::TYPE_UNKNOWN;
	p.Plane.MakeSignBits();

	p.bEnable = true;

	m_aPlanes.Add(p);
	return true;
}

/*	Check if AABB is in or out frustum. This functions assume that all frustum
	planes point to frustem inner.

	Return value:

		1: AABB is complete in frustum
		0: AABB cross with frustum
		-1: AABB is complete out of frustum
*/
int A3DFrustum::AABBInFrustum(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
{
	bool bCross = false;

	for (int i=0; i < m_aPlanes.GetSize(); i++)
	{
		if (!m_aPlanes[i].bEnable)
			continue;

		int iRet = CLS_PlaneAABBOverlap(m_aPlanes[i].Plane, vMins, vMaxs);
		if (iRet < 0)
			return -1;
		else if (!iRet)
			bCross = true;
	}
	
	return bCross ? 0 : 1;
}

/*	Check if sphere is in or out frustum. This function assumes that all frustum
	planes point to frustem inner.

	Return value:

		1: sphere is complete in frustum
		0: sphere cross with frustum
		-1: sphere is complete out of frustum
*/
int A3DFrustum::SphereInFrustum(const A3DVECTOR3& vCenter, float fRadius)
{
	bool bCross = false;

	for (int i=0; i < m_aPlanes.GetSize(); i++)
	{
		if (!m_aPlanes[i].bEnable)
			continue;

		int iRet = CLS_PlaneSphereOverlap(m_aPlanes[i].Plane, vCenter, fRadius);
		if (iRet < 0)
			return -1;
		else if (!iRet)
			bCross = true;
	}

	return bCross ? 0 : 1;
}

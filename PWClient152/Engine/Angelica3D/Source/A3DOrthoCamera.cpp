/*
 * FILE: A3DOrthoCamera.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DOrthoCamera.h"
#include "A3DDevice.h"
#include "A3DFuncs.h"

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
//	Implement A3DOrthoCamera
//	
///////////////////////////////////////////////////////////////////////////

A3DOrthoCamera::A3DOrthoCamera()
{ 
	m_dwClassID = A3D_CID_ORTHOCAMERA;
}

//	Initialize camera object
bool A3DOrthoCamera::Init(A3DDevice* pA3DDevice, float l, float r, float b, float t, float zn, float zf)
{
	if (!A3DCameraBase::Init(pA3DDevice, zn, zf))
		return false;

	m_fLeft		= l;
	m_fRight	= r;
	m_fTop		= t;
	m_fBottom	= b;

	D3DXMATRIX matProj;
	D3DXMatrixOrthoOffCenterLH(&matProj, l, r, b, t, zn, zf);
	m_matProjectTM = *(A3DMATRIX4 *)&matProj;
	a3d_InverseMatrix(m_matProjectTM, &m_matInvProjectTM);

	m_matPostProjectTM.Identity();

	//	Create view frustum
	m_ViewFrustum.Init(6);
	m_WorldFrustum.Init(6);
	CreateViewFrustum();

	return true;
}

//	Release object
void A3DOrthoCamera::Release()
{
	A3DCameraBase::Release();
}

//	Update project TM
bool A3DOrthoCamera::UpdateProjectTM()
{
	D3DXMATRIX matProj;

	D3DXMatrixOrthoOffCenterLH(&matProj, m_fLeft, m_fRight, m_fBottom, m_fTop, m_vFront, m_vBack);
	m_matProjectTM = *(A3DMATRIX4 *)&matProj;
	a3d_InverseMatrix(m_matProjectTM, &m_matInvProjectTM);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseMatrix(m_matVPTM, &m_matInvVPTM);

	CreateViewFrustum();

	return true;
}

//	Create view frustum
bool A3DOrthoCamera::CreateViewFrustum()
{
	A3DPLANE *p;

	//	Top clip plane
	p = m_ViewFrustum.GetPlane(A3DFrustum::VF_TOP);
	p->vNormal.Set(0.0f, -1.0f, 0.0f);
	p->fDist = -m_fTop;

	//	Bottom clip plane
	p = m_ViewFrustum.GetPlane(A3DFrustum::VF_BOTTOM);
	p->vNormal.Set(0.0f, 1.0f, 0.0f);
	p->fDist = m_fBottom;

	//	Left clip plane
	p = m_ViewFrustum.GetPlane(A3DFrustum::VF_LEFT);
	p->vNormal.Set(1.0f, 0.0f, 0.0f);
	p->fDist = m_fLeft;

	//	Right clip plane
	p = m_ViewFrustum.GetPlane(A3DFrustum::VF_RIGHT);
	p->vNormal.Set(-1.0f, 0.0f, 0.0f);
	p->fDist = -m_fRight;

	//	Near clip plane
	p = m_ViewFrustum.GetPlane(A3DFrustum::VF_NEAR);
	p->vNormal.Set(0.0f, 0.0f, 1.0f);
	p->fDist = m_vFront;

	//	Far clip plane
	p = m_ViewFrustum.GetPlane(A3DFrustum::VF_FAR);
	p->vNormal.Set(0.0f, 0.0f, -1.0f);
	p->fDist = -m_vBack;

	//	Update frustum in world coordinates
	UpdateWorldFrustum();

	return true;
}

//	Update world frustum
bool A3DOrthoCamera::UpdateWorldFrustum()
{
	A3DSPLANE *p;

	//	Top clip plane
	p = m_WorldFrustum.GetPlane(A3DFrustum::VF_TOP);
	p->vNormal = -m_vecUp;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecUp * m_fTop);
	p->byType = A3DSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	//	Bottom clip plane
	p = m_WorldFrustum.GetPlane(A3DFrustum::VF_BOTTOM);
	p->vNormal = m_vecUp;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecUp * m_fBottom);
	p->byType = A3DSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	//	Left clip plane
	p = m_WorldFrustum.GetPlane(A3DFrustum::VF_LEFT);
	p->vNormal = m_vecRight;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecRight * m_fLeft);
	p->byType = A3DSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	//	Right clip plane
	p = m_WorldFrustum.GetPlane(A3DFrustum::VF_RIGHT);
	p->vNormal = m_vecLeft;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecRight * m_fRight);
	p->byType = A3DSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	//	Near clip plane
	p = m_WorldFrustum.GetPlane(A3DFrustum::VF_NEAR);
	p->vNormal = m_vecDir;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecDir * m_vFront);
	p->byType = A3DSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	//	Far clip plane
	p = m_WorldFrustum.GetPlane(A3DFrustum::VF_FAR);
	p->vNormal = -m_vecDir;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecDir * m_vBack);
	p->byType = A3DSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	return true;
}

//	Set / Get projection parameters
bool A3DOrthoCamera::SetProjectionParam(float l, float r, float b, float t, float zn, float zf)
{
	m_fLeft		= l;
	m_fRight	= r;
	m_fTop		= t;
	m_fBottom	= b;
	m_vFront	= zn;
	m_vBack		= zf;

	UpdateProjectTM();
	return true;

	// could not call base camera's setfrontandback, because if zn < 0.0f, it will not update it!!!
	//return SetZFrontAndBack(zn, zf, true);
}

/*	Calculate view frustum's 8 corners' position

	Return number of corners;

	aCorners (out): array used to receive corners position, you can pass NULL to get how
			many corners by return value. 
			corner index is defined as below:
			0 - 3: near plane corners, left-bottom, left-top, right-top, right-bottom
			4 - 7: far plane corners, left-bottom, left-top, right-top, right-bottom
	bInViewSpace: true, positions are in view space; false, in world space
	fNear, fFar: near and far distance.
*/
int A3DOrthoCamera::CalcFrustumCorners(A3DVECTOR3* aCorners, bool bInViewSpace, float fNear, float fFar)
{
	if (!aCorners)
		return 8;

	A3DVECTOR3 vPos, vZ, vX, vY;

	if (bInViewSpace)
	{
		vPos.Clear();
		vZ = A3DVECTOR3(0,0,1);//A3DVECTOR3::vAxisZ;
		vX = A3DVECTOR3(1,0,0);//A3DVECTOR3::vAxisX;
		vY = A3DVECTOR3(0,1,0);//A3DVECTOR3::vAxisY;
	}
	else
	{
		vPos = GetPos();
		vZ = GetDir();
		vX = GetRight();
		vY = GetUp();
	}

	float fNearPlaneHeight = (GetRight() - GetLeft()) * 0.5f;
	float fNearPlaneWidth = (GetTop() - GetBottom()) * 0.5f;
	float fFarPlaneHeight = fNearPlaneHeight;
	float fFarPlaneWidth = fNearPlaneWidth;

	A3DVECTOR3 vNearPlaneCenter = vPos + vZ * fNear;
	A3DVECTOR3 vFarPlaneCenter = vPos + vZ * fFar;

	aCorners[0] = vNearPlaneCenter - vX * fNearPlaneWidth - vY * fNearPlaneHeight;
	aCorners[1] = vNearPlaneCenter - vX * fNearPlaneWidth + vY * fNearPlaneHeight;
	aCorners[2] = vNearPlaneCenter + vX * fNearPlaneWidth + vY * fNearPlaneHeight;
	aCorners[3] = vNearPlaneCenter + vX * fNearPlaneWidth - vY * fNearPlaneHeight;

	aCorners[4] = vFarPlaneCenter - vX * fFarPlaneWidth - vY * fFarPlaneHeight;
	aCorners[5] = vFarPlaneCenter - vX * fFarPlaneWidth + vY * fFarPlaneHeight;
	aCorners[6] = vFarPlaneCenter + vX * fFarPlaneWidth + vY * fFarPlaneHeight;
	aCorners[7] = vFarPlaneCenter + vX * fFarPlaneWidth - vY * fFarPlaneHeight;	

	return 8;
}




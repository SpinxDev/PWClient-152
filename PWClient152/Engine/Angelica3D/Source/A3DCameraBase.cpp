/*
 * FILE: A3DCameraBaseBase.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DCameraBase.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DFuncs.h"
#include "A3DCollision.h"
#include "AMSoundEngine.h"
#include "AM3DSoundDevice.h"
#include "AAssist.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

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
//	Implement A3DCameraBaseBase
//	
///////////////////////////////////////////////////////////////////////////

A3DCameraBase::A3DCameraBase()
{
	m_dwClassID			= A3D_CID_CAMERABASE;
	m_bMirrored			= false;
	m_pA3DDevice		= NULL;
	m_pAM3DSoundDevice	= NULL;
	m_pMirrorCamera		= NULL;
}

A3DCameraBase::~A3DCameraBase()
{
}

//	Initlaize object
bool A3DCameraBase::Init(A3DDevice* pA3DDevice, FLOAT vFront, FLOAT vBack)
{
	m_pA3DDevice = pA3DDevice;

	m_vFront	= vFront;
	m_vBack		= vBack;

	m_matPostProjectTM.Identity();

	m_vecPos	= A3DVECTOR3(0.0f);
	m_vecDir	= A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_vecDirH	= A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_vecUp		= A3DVECTOR3(0.0f, 1.0f, 0.0f);
	m_vecRight	= A3DVECTOR3(1.0f, 0.0f, 0.0f);
	m_vecLeft	= A3DVECTOR3(-1.0f, 0.0f, 0.0f);

	m_matViewTM.Identity();
	
	return true;
}

//	Is this a perspective camera ?
bool A3DCameraBase::IsPerspective()
{
	return m_dwClassID == A3D_CID_CAMERA;
}

//	Is this a orthogonal camera ?
bool A3DCameraBase::IsOrthogonal()
{
	return m_dwClassID == A3D_CID_ORTHOCAMERA;
}

void A3DCameraBase::SetDirAndUp(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp)
{
	m_vecDir = Normalize(vecDir);
	m_vecUp = Normalize(vecUp);
	m_vecRight = Normalize(CrossProduct(m_vecUp, m_vecDir));
	m_vecUp = Normalize(CrossProduct(m_vecDir, m_vecRight));
	m_vecLeft = -m_vecRight;

	m_vecDirH = m_vecDir;
	m_vecDirH.y = 0.0f;
	m_vecDirH = Normalize(m_vecDirH);
	m_vecLeftH = m_vecLeft;
	m_vecLeftH.y = 0.0f;
	m_vecLeftH = Normalize(m_vecLeftH);

	m_vDeg = (FLOAT)RAD2DEG(atan2(-m_vecDir.z, m_vecDir.x));
	m_vPitch = (FLOAT)RAD2DEG(asin(m_vecDir.y));
	UpdateViewTM();
}

// Set a camera's view matrix using an existing view matrix
bool A3DCameraBase::SetViewTM(const A3DMATRIX4& matView)
{
	A3DMATRIX4 matInv;

	m_matViewTM = matView;

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseMatrix(m_matVPTM, &m_matInvVPTM);

	A3DVECTOR3 vecDir, vecUp, vecPos;
	vecDir.x = matView.m[0][2];
	vecDir.y = matView.m[1][2];
	vecDir.z = matView.m[2][2];
	vecUp.x = matView.m[0][1];
	vecUp.y = matView.m[1][1];
	vecUp.z = matView.m[2][1];

	vecPos = matView.GetRow(3);

	matInv = matView;
	matInv._41 = matInv._42 = matInv._43 = 0.0f;
	matInv = a3d_Transpose(matInv);
	vecPos = matInv * (-1.0f * vecPos);
	
	SetDirAndUp(vecDir, vecUp);
	SetPos(vecPos);
	return true;
}

// Set a camera's view matrix force to a specified view matrix
bool A3DCameraBase::UpdateForMirroredCamera()
{
	// first update orientation information according to the mirrored camera
	A3DMATRIX4 matInv;
	m_matViewTM = m_matMirror * m_pMirrorCamera->GetViewTM();

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseMatrix(m_matVPTM, &m_matInvVPTM);

	A3DVECTOR3 vecDir, vecUp, vecPos;
	vecDir.x = m_matViewTM.m[0][2];
	vecDir.y = m_matViewTM.m[1][2];
	vecDir.z = m_matViewTM.m[2][2];
	vecUp.x = m_matViewTM.m[0][1];
	vecUp.y = m_matViewTM.m[1][1];
	vecUp.z = m_matViewTM.m[2][1];

	vecPos = m_matViewTM.GetRow(3);

	matInv = m_matViewTM;
	matInv._41 = matInv._42 = matInv._43 = 0.0f;
	matInv = a3d_Transpose(matInv);
	vecPos = matInv * (-1.0f * vecPos);
	
	m_vecDir = vecDir;
	m_vecUp = vecUp;
	m_vecPos = vecPos;
	m_vecRight = CrossProduct(m_vecDir, m_vecUp);

	// now update the viewfrustum
	// get view frustum from origin camera;
	m_ViewFrustum = *m_pMirrorCamera->GetViewFrustum();

	// then update world frustum.
	UpdateWorldFrustum();
	return true;
}

bool A3DCameraBase::UpdateViewTM()
{
	m_matViewTM = a3d_ViewMatrix(m_vecPos, m_vecDir, m_vecUp, 0.0f);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseMatrix(m_matVPTM, &m_matInvVPTM);

	UpdateWorldFrustum();
	return true;
}

bool A3DCameraBase::SetProjectionTM(const A3DMATRIX4& matProjection)
{
	m_matProjectTM = matProjection;
	a3d_InverseMatrix(m_matProjectTM, &m_matInvProjectTM);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseMatrix(m_matVPTM, &m_matInvVPTM);

	CreateViewFrustum();
	return true;
}

bool A3DCameraBase::Active()
{
	if (!m_pA3DDevice)
		return true;

	if (m_bMirrored)
		m_pA3DDevice->SetFaceCull(A3DCULL_CW);
	else
		m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	
	m_pA3DDevice->SetViewMatrix(GetViewTM());
	m_pA3DDevice->SetProjectionMatrix(m_matProjectTM * m_matPostProjectTM);

	return true;
}

bool A3DCameraBase::MoveFront(FLOAT vDistance)
{
	m_vecPos = m_vecPos + m_vecDirH * vDistance;
	UpdateViewTM();
	return true;
}

bool A3DCameraBase::MoveBack(FLOAT vDistance)
{
	m_vecPos = m_vecPos - m_vecDirH * vDistance;
	UpdateViewTM();
	return true;
}

bool A3DCameraBase::MoveLeft(FLOAT vDistance)
{
	m_vecPos = m_vecPos + m_vecLeftH * vDistance;
	UpdateViewTM();
	return true;
}

bool A3DCameraBase::MoveRight(FLOAT vDistance)
{
	m_vecPos = m_vecPos - m_vecLeftH * vDistance;
	UpdateViewTM();
	return true;
}

bool A3DCameraBase::DegDelta(FLOAT vDelta)
{
	m_vDeg += vDelta;
	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

bool A3DCameraBase::PitchDelta(FLOAT vDelta)
{
	m_vPitch += vDelta;
	a_Clamp(m_vPitch, -89.0f, 89.0f);

	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

bool A3DCameraBase::Move(const A3DVECTOR3& vecDelta)
{
	m_vecPos = m_vecPos + vecDelta;
	UpdateViewTM();
	return true;
}

bool A3DCameraBase::SetMirror(A3DCameraBase* pCamera, const A3DVECTOR3& vecOrg, const A3DVECTOR3& vecNormal)
{
	m_matMirror = a3d_MirrorMatrix(vecOrg, vecNormal);
	m_pMirrorCamera = pCamera;
	m_bMirrored = true;
	return true;
}

const A3DVECTOR3& A3DCameraBase::GetPos() const
{
	return m_bMirrored ? m_pMirrorCamera->GetPos() : m_vecPos;
}

bool A3DCameraBase::UpdateDirAndUp()
{
	m_vecDir.x = (FLOAT)(cos(DEG2RAD(m_vDeg)) * cos(DEG2RAD(m_vPitch)));
	m_vecDir.y = (FLOAT)sin(DEG2RAD(m_vPitch));
	m_vecDir.z = (FLOAT)(-sin(DEG2RAD(m_vDeg)) * cos(DEG2RAD(m_vPitch)));

	m_vecRight = Normalize(CrossProduct(A3D::g_vAxisY, m_vecDir));
	m_vecUp = Normalize(CrossProduct(m_vecDir, m_vecRight));
	m_vecLeft = -m_vecRight;

	m_vecDirH = m_vecDir;
	m_vecDirH.y = 0.0f;
	m_vecDirH = Normalize(m_vecDirH);
	m_vecLeftH = m_vecLeft;
	m_vecLeftH.y = 0.0f;
	m_vecLeftH = Normalize(m_vecLeftH);
	return true;
}

/*	Transform one 3D world space vector into projection plane coordinates;
	the coordinated ranges are 
	return true if this point should be clipped;
	return false if not;
*/
bool A3DCameraBase::Transform(const A3DVECTOR3& vecIn, A3DVECTOR3& vecOut)
{
	vecOut = vecIn * m_matVPTM;

	if (vecOut.x < -1.0f || vecOut.y < -1.0f ||
		vecOut.x > 1.0f || vecOut.y > 1.0f ||
		vecOut.z < 0.0f || vecOut.z > 1.0f )
		return true;
	else
		return false;
}

/*
	Transform one projection plane coordinates into world space coordinates;
*/
bool A3DCameraBase::InvTransform(const A3DVECTOR3& vecIn, A3DVECTOR3& vecOut)
{
	vecOut = vecIn * m_matInvVPTM;
	return true;
}

/* 
	Rotate the camera around an axis in world space;
*/
bool A3DCameraBase::TurnAroundAxis(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecAxis, FLOAT vDeltaRad)
{
	A3DMATRIX4 matRotate = a3d_RotateAxis(vecPos, vecAxis, vDeltaRad);
	A3DVECTOR3 vecOrigin = A3DVECTOR3(0.0f) * matRotate;
	m_vecPos = m_vecPos * matRotate;
	m_vecDir = m_vecDir * matRotate - vecOrigin;
	m_vecUp  = m_vecUp  * matRotate - vecOrigin;

	SetDirAndUp(m_vecDir, m_vecUp);
	return true;
}

bool A3DCameraBase::SetDeg(FLOAT vDeg)
{
	m_vDeg = vDeg;

	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

bool A3DCameraBase::SetPitch(FLOAT vPitch)
{
	m_vPitch = vPitch;
	a_Clamp(m_vPitch, -89.0f, 89.0f);

	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

// Please don't call this function only once per tick, or maybe for several tick,
// so we can save some cpu usage;
bool A3DCameraBase::UpdateEar()
{
	AM3DSoundDevice * pAM3DSoundDevice = m_pA3DDevice->GetA3DEngine()->GetAMSoundEngine()->GetAM3DSoundDevice();

	// Now we should adjust the 3d sound device's pos and orientation;
	if (pAM3DSoundDevice)
	{
		pAM3DSoundDevice->SetPosition(m_vecPos);
		pAM3DSoundDevice->SetOrientation(m_vecDir, m_vecUp);
		pAM3DSoundDevice->UpdateChanges();
	}

	return true;
}

bool A3DCameraBase::SetZBias(FLOAT vZBias)
{
	if (!m_pA3DDevice)
		return true;

	A3DMATRIX4 matProjectTM = m_matProjectTM;
//	matProjectTM._43 += vZBias;
	matProjectTM._33 *= vZBias;
	
	m_pA3DDevice->SetProjectionMatrix(matProjectTM * m_matPostProjectTM);
	return true;
}

//	Set Z front and back distance
bool A3DCameraBase::SetZFrontAndBack(float fFront, float fBack, bool bUpdateProjMat/* true */)
{
	if( fFront > 0.0f )
		m_vFront = fFront;
	if( fBack > 0.0f )
		m_vBack		= fBack;

	if (bUpdateProjMat)
		UpdateProjectTM();

	return true;
}


bool A3DCameraBase::AABBInViewFrustum(const A3DAABB& aabb) 
{ 
	return m_WorldFrustum.AABBInFrustum(aabb) < 0 ? false : true; 
}

bool A3DCameraBase::AABBInViewFrustum(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
{ 
	return m_WorldFrustum.AABBInFrustum(vMins, vMaxs) < 0 ? false : true; 
}

bool A3DCameraBase::SphereInViewFrustum(const A3DVECTOR3& vCenter, float fRadius) 
{ 
	return m_WorldFrustum.SphereInFrustum(vCenter, fRadius) < 0 ? false : true; 
}

bool A3DCameraBase::PointInViewFrustum(const A3DVECTOR3& vPos)
{ 
	return m_WorldFrustum.PointInFrustum(vPos); 
}

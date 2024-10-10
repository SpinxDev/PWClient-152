/*
 * FILE: A3DCameraBase.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DCAMERA_BASE_H_
#define _A3DCAMERA_BASE_H_

#include "A3DTypes.h"
#include "A3DFrustum.h"
#include "A3DObject.h"
#include "A3DMacros.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DDevice;
class AM3DSoundDevice;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DCameraBase
//	
///////////////////////////////////////////////////////////////////////////

class A3DCameraBase	: public A3DObject
{
public:		//	Types

public:		//	Constructor and Destructor

	A3DCameraBase();
	virtual ~A3DCameraBase();

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	bool Init(A3DDevice* pA3DDevice, FLOAT vFront, FLOAT vBack);
	//	Release object
	virtual void Release() {}

	//	Apply camera to device
	virtual bool Active();

	//	Is this a perspective camera ?
	bool IsPerspective();
	//	Is this a orthogonal camera ?
	bool IsOrthogonal();


	//	Set camera's direction and up
	void SetDirAndUp(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp);

	//	Camera moving routines
	bool MoveFront(FLOAT vDistance);
	bool MoveBack(FLOAT vDistance);
	bool MoveLeft(FLOAT vDistance);
	bool MoveRight(FLOAT vDistance);
	bool Move(const A3DVECTOR3& vecDelta);

	//	Camera rotating routines
	bool TurnAroundAxis(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecAxis, FLOAT vDeltaRad);
	bool DegDelta(FLOAT vDelta);
	bool PitchDelta(FLOAT vDelta);
	bool SetDeg(FLOAT vDeg);
	bool SetPitch(FLOAT vPitch);

	//	Please don't call this function only once per tick, or maybe for several tick,
	//	so we can save some cpu usage;
	bool UpdateEar();
	
	//	Check whether aabb is in view frustum
	virtual bool AABBInViewFrustum(const A3DAABB& aabb);
	virtual bool AABBInViewFrustum(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs);
	//	Check whether sphere is in view frustum
	virtual bool SphereInViewFrustum(const A3DVECTOR3& vCenter, float fRadius);
	//	Check wether point is in view frustum
	virtual bool PointInViewFrustum(const A3DVECTOR3& vPos);

	//	Transform position from world space to cuboid coordinates space
	virtual bool Transform(const A3DVECTOR3& vecIn, A3DVECTOR3& vecOut);
	//	Transform position from coboid coordinates to world space
	virtual bool InvTransform(const A3DVECTOR3& vecIn, A3DVECTOR3& vecOut);

	//	Get camera's position
	const A3DVECTOR3& GetPos() const;

	//	Set mirror camera
	bool SetMirror(A3DCameraBase* pCamera, const A3DVECTOR3& vecOrg, const A3DVECTOR3& vecNormal);

	bool SetViewTM(const A3DMATRIX4& matView);
	bool SetZBias(FLOAT vZBias);
	bool SetProjectionTM(const A3DMATRIX4& matProjection);
	
	//	Set / Get Z front and back distance
	bool SetZFrontAndBack(float fFront, float fBack, bool bUpdateProjMat=true);
	float GetZFront() const { return m_vFront; }
	float GetZBack() const { return m_vBack; }

	//	UpdateViewTM should be called after set pos; but we should think
	//	carefully about when to call it;
	void SetPos(const A3DVECTOR3& vecPos) { m_vecPos = vecPos; UpdateViewTM(); }
	const A3DMATRIX4& GetViewTM() const { return m_matViewTM; }
	const A3DMATRIX4& GetProjectionTM() const { return m_matProjectTM; }
	const A3DMATRIX4& GetVPTM() const { return m_matVPTM; }
	const A3DVECTOR3& GetDir() const { return m_vecDir; }
	const A3DVECTOR3& GetDirH() const { return m_vecDirH; }
	const A3DVECTOR3& GetUp() const { return m_vecUp; }
	const A3DVECTOR3& GetRight() const { return m_vecRight; }
	A3DVECTOR3 GetRightH() const { return -1.0f * m_vecLeftH; }
	const A3DVECTOR3& GetLeft() const { return m_vecLeft; }
	const A3DVECTOR3& GetLeftH() const { return m_vecLeftH; }
	FLOAT GetDeg() const { return m_vDeg; }
	FLOAT GetPitch() const { return m_vPitch; }

	//	Set / Get post project TM
	const A3DMATRIX4& GetPostProjectTM() const { return m_matPostProjectTM; }
	void SetPostProjectTM(A3DMATRIX4 matPostProjectTM) { m_matPostProjectTM = matPostProjectTM; UpdateProjectTM(); }

	//	Set / Get sound device
	void SetAM3DSoundDevice(AM3DSoundDevice* pAM3DSoundDevice) { m_pAM3DSoundDevice = pAM3DSoundDevice; }
	AM3DSoundDevice* GetAM3DSoundDevice() { return m_pAM3DSoundDevice; }

	//	Get view frustum in world space
	A3DFrustum* GetWorldFrustum() { return &m_WorldFrustum; }
	A3DFrustum* GetViewFrustum()  { return &m_ViewFrustum; }


	// mirror information
	inline bool IsMirrored()						{ return m_bMirrored; }
	inline A3DCameraBase * GetMirrorCamera()	{ return m_pMirrorCamera; }
	inline const A3DMATRIX4& GetMirrorMatrix()	{ return m_matMirror; }

	virtual int CalcFrustumCorners(A3DVECTOR3* aCorners, bool bInViewSpace, float fNear, float fFar) = 0;

protected:	//	Attributes

	A3DDevice*			m_pA3DDevice;
	AM3DSoundDevice*	m_pAM3DSoundDevice;

	A3DVECTOR3			m_vecPos;
	A3DVECTOR3			m_vecDir;
	A3DVECTOR3			m_vecUp;
	A3DVECTOR3			m_vecLeft;
	A3DVECTOR3			m_vecRight;
	A3DVECTOR3			m_vecDirH;
	A3DVECTOR3			m_vecLeftH;

	A3DMATRIX4			m_matViewTM;
	A3DMATRIX4			m_matProjectTM;
	A3DMATRIX4			m_matVPTM;
	A3DMATRIX4			m_matPostProjectTM;	// Transform matrix used for post project process

	A3DMATRIX4			m_matInvProjectTM;
	A3DMATRIX4			m_matInvVPTM;

	A3DCameraBase*		m_pMirrorCamera;
	A3DMATRIX4			m_matMirror;
	bool				m_bMirrored;

	FLOAT				m_vDeg;
	FLOAT				m_vPitch;

	FLOAT				m_vFront;
	FLOAT				m_vBack;

	//	Frustum in view coordinates and world coordinates
	A3DFrustum			m_ViewFrustum;
	A3DFrustum			m_WorldFrustum;

protected:	//	Operations

	//	Update view transform matrix
	bool UpdateViewTM();
	//	Update camera's direction and up
	bool UpdateDirAndUp();

	//	Update all things according to the mirror information
	bool UpdateForMirroredCamera();

	//	Update project TM
	virtual bool UpdateProjectTM() = 0;
	//	Create view frustum
	virtual bool CreateViewFrustum() = 0;
	//	Update world frustum
	virtual bool UpdateWorldFrustum() = 0;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DCAMERABASE_H_

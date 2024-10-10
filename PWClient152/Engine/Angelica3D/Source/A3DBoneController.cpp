/*
 * FILE: A3DBoneController.cpp
 *
 * DESCRIPTION: a set of class which used for control of bones
 *
 * CREATED BY: Hedi, 2004/12/2
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DBoneController.h"

#include "A3DTypes.h"
#include "A3DFuncs.h"
#include "A3DBone.h"
#include "A3DSkeleton.h"

//////////////////////////////////////////////////////////////////////////////////////
// base class for bone controller
//////////////////////////////////////////////////////////////////////////////////////
A3DBoneController::A3DBoneController(A3DBone * pTargetBone)
: m_pTargetBone(pTargetBone), m_pNextController(NULL), m_pUserData(NULL)
{
}

A3DBoneController::~A3DBoneController()
{
	if( m_pUserData )
	{
		delete m_pUserData;
		m_pUserData = NULL;
	}
}

void A3DBoneController::SetUserData(const char * key, int value)
{
	if( !m_pUserData )
		m_pUserData = new USERDATATAB(16);

	USERDATATAB::pair_type Pair = m_pUserData->get(key);
	if (!Pair.second)
	{
		m_pUserData->put(key, value);
	}
	else
	{
		*Pair.first = value;
	}
}

bool A3DBoneController::GetUserData(const char * key, int * pValue)
{
	if( !pValue || !m_pUserData )
		return false;

	USERDATATAB::pair_type Pair = m_pUserData->get(key);
	if (!Pair.second)
		return false;	

	*pValue = *Pair.first;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// Position controller
//////////////////////////////////////////////////////////////////////////////////////
A3DBonePosController::A3DBonePosController(A3DBone * pTargetBone)
: A3DBoneController(pTargetBone), m_vecPos(0.0f), m_nTransTime(0), m_nCurTime(0)
{
}

A3DBonePosController::~A3DBonePosController()
{
}
   
bool A3DBonePosController::Move(const A3DVECTOR3& vecPos, int nTransTime)
{
	m_vecPosStart = m_vecPos;
	m_vecPosEnd = vecPos;

	m_nTransTime = nTransTime;
	m_nCurTime = 0;

	if( m_nTransTime == 0 )
		m_vecPos = m_vecPosEnd;

	return true;
}

bool A3DBonePosController::Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled)
{
	if( m_nCurTime < m_nTransTime )
	{
		m_nCurTime += nDeltaTime;

		if( m_nCurTime > m_nTransTime )
			m_nCurTime = m_nTransTime;
		
		float f = (float)m_nCurTime / m_nTransTime;

		m_vecPos = m_vecPosStart * (1.0f - f) + m_vecPosEnd * f;
	}

	m_matController = Translate(m_vecPos.x, m_vecPos.y, m_vecPos.z);
	matControlled = matControlled * m_matController;

	if( m_pNextController )
		return m_pNextController->Update(nDeltaTime, pTargetBone, matControlled);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// Rotation controller
//////////////////////////////////////////////////////////////////////////////////////
A3DBoneRotController::A3DBoneRotController(A3DBone * pTargetBone)
: A3DBoneController(pTargetBone), m_qu(1.0f, 0.0f, 0.0f, 0.0f), m_nTransTime(0), m_nCurTime(0)
{
}

A3DBoneRotController::~A3DBoneRotController()
{
}

bool A3DBoneRotController::Rotate(const A3DQUATERNION& qu, int nTransTime)
{
	m_quStart = m_qu;
	m_quEnd = qu;

	m_nTransTime = nTransTime;
	m_nCurTime = 0;

	if( m_nTransTime == 0 )
		m_qu = m_quEnd;

	return true;
}

bool A3DBoneRotController::Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled)
{
	if( m_nCurTime < m_nTransTime )
	{
		m_nCurTime += nDeltaTime;

		if( m_nCurTime > m_nTransTime )
			m_nCurTime = m_nTransTime;
		
		float f = (float)m_nCurTime / m_nTransTime;

		m_qu = SLERPQuad(m_quStart, m_quEnd, f);
	}

	A3DMATRIX4 matRotate;
	m_qu.ConvertToMatrix(matRotate);

	m_matController = matRotate;
	matControlled = matControlled * m_matController;

	if( m_pNextController )
		return m_pNextController->Update(nDeltaTime, pTargetBone, matControlled);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// Scale controller
//////////////////////////////////////////////////////////////////////////////////////
A3DBoneScaleController::A3DBoneScaleController(A3DBone * pTargetBone)
: A3DBoneController(pTargetBone), m_scale(1.0f), m_nTransTime(0), m_nCurTime(0)
{
}

A3DBoneScaleController::~A3DBoneScaleController()
{
}

bool A3DBoneScaleController::Scale(const A3DVECTOR3& scale, int nTransTime)
{
	m_scaleStart	= m_scale;
	m_scaleEnd		= scale;

	m_nTransTime = nTransTime;
	m_nCurTime = 0;

	if( m_nTransTime == 0 )
		m_scale = m_scaleEnd;

	return true;
}

bool A3DBoneScaleController::Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled)
{
	if( m_nCurTime < m_nTransTime )
	{
		m_nCurTime += nDeltaTime;

		if( m_nCurTime > m_nTransTime )
			m_nCurTime = m_nTransTime;
		
		float f = (float)m_nCurTime / m_nTransTime;

		m_scale = m_scaleStart * (1.0f - f) + m_scaleEnd * f;
	}

	m_matController = Scaling(m_scale.x, m_scale.y, m_scale.z);
	matControlled = matControlled * m_matController;

	if( m_pNextController )
		return m_pNextController->Update(nDeltaTime, pTargetBone, matControlled);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// Local Scale controller
//////////////////////////////////////////////////////////////////////////////////////
A3DBoneLocalScaleController::A3DBoneLocalScaleController(A3DBone * pTargetBone)
: A3DBoneController(pTargetBone), m_scale(1.0f), m_nTransTime(0), m_nCurTime(0)
{
}

A3DBoneLocalScaleController::~A3DBoneLocalScaleController()
{
}

bool A3DBoneLocalScaleController::Scale(const A3DVECTOR3& scale, int nTransTime)
{
	m_scaleStart	= m_scale;
	m_scaleEnd		= scale;

	m_nTransTime = nTransTime;
	m_nCurTime = 0;

	if( m_nTransTime == 0 )
		m_scale = m_scaleEnd;

	return true;
}

bool A3DBoneLocalScaleController::Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled)
{
	if( m_nCurTime < m_nTransTime )
	{
		m_nCurTime += nDeltaTime;

		if( m_nCurTime > m_nTransTime )
			m_nCurTime = m_nTransTime;
		
		float f = (float)m_nCurTime / m_nTransTime;

		m_scale = m_scaleStart * (1.0f - f) + m_scaleEnd * f;
	}

	A3DMATRIX4 matRelativeTM = m_pTargetBone->GetRelativeTM();

	m_matController = Scaling(m_scale.x, m_scale.y, m_scale.z);
	matControlled = matControlled * InverseTM(matRelativeTM) * m_matController * matRelativeTM;

	if( m_pNextController )
		return m_pNextController->Update(nDeltaTime, pTargetBone, matControlled);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// Matrix controller											
//////////////////////////////////////////////////////////////////////////////////////
A3DBoneMatrixController::A3DBoneMatrixController(A3DBone * pTargetBone)
: A3DBoneController(pTargetBone), m_vecPos(0.0f), m_qu(1.0f, 0.0f, 0.0f, 0.0f), m_scale(1.0f), m_nTransTime(0), m_nCurTime(0)
{
}

A3DBoneMatrixController::~A3DBoneMatrixController()
{
}

bool A3DBoneMatrixController::CombineMatrixElements(const A3DVECTOR3& vecScale, const A3DQUATERNION& quOrient, const A3DVECTOR3& vecPos, int nTransTime)
{
	m_vecPosStart	= m_vecPos;
	m_quStart		= m_qu;
	m_scaleStart	= m_scale;

	m_vecPosEnd		= vecPos;
	m_quEnd			= quOrient;
	m_scaleEnd		= vecScale;

	m_nTransTime	= nTransTime;
	m_nCurTime		= 0;

	if( m_nTransTime == 0 )
	{
		m_vecPos	= m_vecPosEnd;
		m_qu		= m_quEnd;
		m_scale		= m_scaleEnd;
	}

	return true;
}

bool A3DBoneMatrixController::CombineMatrix(const A3DMATRIX4& mat, int nTransTime)
{
	A3DVECTOR3 scale;
	A3DQUATERNION orient;
	A3DVECTOR3 pos;

	A3DMATRIX4 matRelativeTM = m_pTargetBone->GetOriginalMatrix();
	a3d_DecomposeMatrix(matRelativeTM * mat * InverseTM(matRelativeTM), scale, orient, pos);

	pos = a3d_VectorMatrix3x3(pos, matRelativeTM);
	return CombineMatrixElements(scale, orient, pos, nTransTime);
}

bool A3DBoneMatrixController::CombineMatrixList(const A3DMATRIX4 * mats, float * weights, int nNumMat, int nTransTime)
{
	A3DVECTOR3 scale(0.0f);
	A3DQUATERNION orient(0.0f, 0.0f, 0.0f, 0.0f);
	A3DVECTOR3 pos(0.0f);

	A3DMATRIX4 matRelativeTM = m_pTargetBone->GetOriginalMatrix();
	A3DMATRIX4 matInv = InverseTM(matRelativeTM);

	A3DVECTOR3 scaleThis;
	A3DQUATERNION orientThis;
	A3DVECTOR3 posThis;

	for(int i=0; i<nNumMat; i++)
	{
		a3d_DecomposeMatrix(matRelativeTM * mats[i] * InverseTM(matRelativeTM), scaleThis, orientThis, posThis);

		pos = pos + a3d_VectorMatrix3x3(posThis, matRelativeTM) * weights[i];
		scale = scale + (scaleThis - A3DVECTOR3(1.0f)) * weights[i];
		orient = orient + (orientThis - A3DQUATERNION(0, 0, 0)) * weights[i];
	}

	scale = scale + A3DVECTOR3(1.0f);
	orient = orient + A3DQUATERNION(0, 0, 0);
	orient.Normalize();

	return CombineMatrixElements(scale, orient, pos, nTransTime);
}

bool A3DBoneMatrixController::Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled)
{
	if( m_nCurTime < m_nTransTime )
	{
		m_nCurTime += nDeltaTime;

		if( m_nCurTime > m_nTransTime )
			m_nCurTime = m_nTransTime;
		
		float f = (float)m_nCurTime / m_nTransTime;

		m_vecPos = m_vecPosStart * (1.0f - f) + m_vecPosEnd * f;
		m_scale = m_scaleStart * (1.0f - f) + m_scaleEnd * f;
		m_qu = SLERPQuad(m_quStart, m_quEnd, f);
	}

	A3DMATRIX4 matRelativeTM = pTargetBone->GetOriginalMatrix();

	A3DMATRIX4 matRotate, mat = IdentityMatrix();
	m_qu.ConvertToMatrix(matRotate);
	mat.SetRow(0, matRotate.GetRow(0) * m_scale.x);
	mat.SetRow(1, matRotate.GetRow(1) * m_scale.y);
	mat.SetRow(2, matRotate.GetRow(2) * m_scale.z);
	
	matControlled = InverseTM(matRelativeTM) * mat * matRelativeTM * Translate(m_vecPos.x, m_vecPos.y, m_vecPos.z);

	m_matController = matControlled;
	if( m_pNextController )
		return m_pNextController->Update(nDeltaTime, pTargetBone, matControlled);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// LookAt controller
//////////////////////////////////////////////////////////////////////////////////////
A3DBoneLookAtController::A3DBoneLookAtController(A3DBone * pTargetBone)
: A3DBoneController(pTargetBone), m_qu(1.0f, 0.0f, 0.0f, 0.0f), m_quStart(1.0f, 0.0f, 0.0f, 0.0f), 
  m_quEnd(1.0f, 0.0f, 0.0f, 0.0f), m_nTransTime(0), m_nCurTime(0)
{
	m_bHasTarget = false;
	
	m_vDegMin = 0.0f;
	m_vDegMax = A3D_PI;
	m_vDegScale = 1.0f;

	m_vPitchMin = 0.0f;
	m_vPitchMax = A3D_PI;
	m_vPitchScale = 1.0f;

	m_bOutRangeAutoReturn = true;

	m_vecEyeAxisInBoneSpace = A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_vecHeadUpAxisInBoneSpace = A3DVECTOR3(0.0f, 1.0f, 0.0f);
}

A3DBoneLookAtController::~A3DBoneLookAtController()
{
}

bool A3DBoneLookAtController::StopLookAt(int nTransTime)
{
	m_bHasTarget = false;

	A3DQUATERNION quIdentity(1.0f, 0.0f, 0.0f, 0.0f);
	
	m_quStart = m_qu;
	m_quEnd = quIdentity;

	m_nTransTime = nTransTime;
	m_nCurTime = 0;

	return true;
}

bool A3DBoneLookAtController::LookAt(const A3DVECTOR3& vecTargetPos, int nTransTime)
{
	m_bHasTarget = true;
	m_vecTargetPos = vecTargetPos;

	m_quStart = m_qu;

	m_nTransTime = nTransTime;
	m_nCurTime = 0;
	return true;
}

bool A3DBoneLookAtController::SolveLookAt()
{
	A3DMATRIX4 tmLook = TransformMatrix(m_vecEyeAxisInBoneSpace, m_vecHeadUpAxisInBoneSpace, A3DVECTOR3(0.0f));
	A3DMATRIX4 tmInvLook = InverseTM(tmLook);

	A3DMATRIX4 matRelativeTM = m_pTargetBone->GetRelativeTM();
	A3DMATRIX4 matParentTM = IdentityMatrix();

	A3DBone * pParentBone = m_pTargetBone->GetParentPtr();
	if( pParentBone ) 
		matParentTM = pParentBone->GetAbsoluteTM();
	else
		matParentTM = m_pTargetBone->GetSkeleton()->GetAbsoluteTM();

	A3DMATRIX4 matAbsoluteTM = tmLook * matRelativeTM * matParentTM;
	A3DMATRIX4 matInvAbsTM = InverseTM(matAbsoluteTM);

	A3DVECTOR3 vecDelta = m_vecTargetPos * matInvAbsTM;
	
	A3DVECTOR3 vecDir, vecUp, vecRight;
	vecDir = Normalize(vecDelta);

	// first calculate the sphere angle of this direction;
	float vPitch = (float)asin(vecDir.y);
	if( vPitch < m_vPitchMin && vPitch > -m_vPitchMin )
	{
		vPitch = 0.0f;
	}
	else if( vPitch > m_vPitchMax || vPitch < -m_vPitchMax )
	{
		if( m_bOutRangeAutoReturn )
			vPitch = 0.0f;
		else
		{
			if( vPitch > m_vPitchMax )
				vPitch = m_vPitchMax;
			else
				vPitch = -m_vPitchMax;
		}
	}
	
	float vDeg = (float)atan2(vecDir.x, vecDir.z);
	if( vDeg < m_vDegMin && vDeg > -m_vDegMin )
	{
		vDeg = 0.0f;
	}
	else if( vDeg > m_vDegMax || vDeg < -m_vDegMax )
	{
		if( m_bOutRangeAutoReturn )
		{
			vDeg = 0.0f;
			vPitch = 0.0f;
		}
		else
		{
			if( vDeg > m_vDegMax ) 
				vDeg = m_vDegMax;
			else 
				vDeg = -m_vDegMax;
		}
	}

	if( vPitch )
		vPitch = (vPitch - m_vPitchMin) * m_vPitchScale;

	if( vDeg )
		vDeg = (vDeg - m_vDegMin) * m_vDegScale;

	vecDir.y = (float)sin(vPitch);
	float c = (float)cos(vPitch);
	vecDir.x = c * (float)sin(vDeg);
	vecDir.z = c * (float)cos(vDeg);

	vecUp = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	vecRight = CrossProduct(vecUp, vecDir);
	vecUp = Normalize(CrossProduct(vecDir, vecRight));
	
	A3DMATRIX4 matTarget = TransformMatrix(vecDir, vecUp, A3DVECTOR3(0.0f));
	matTarget = tmInvLook * matTarget * tmLook;

	// we use quaternion to do blend
	A3DQUATERNION quTarget;
	quTarget.ConvertFromMatrix(matTarget);

	// update final orientation
	m_quEnd = quTarget;
	return true;
}

bool A3DBoneLookAtController::Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled)
{
	if( m_bHasTarget )
		SolveLookAt();

	float f;
	if( m_nCurTime < m_nTransTime )
	{
		m_nCurTime += nDeltaTime;

		if( m_nCurTime > m_nTransTime )
			m_nCurTime = m_nTransTime;
		
		f = (float)m_nCurTime / m_nTransTime;
	}
	else
		f = 1.0f;

	m_qu = SLERPQuad(m_quStart, m_quEnd, f);

	A3DMATRIX4 matRelativeTM = pTargetBone->GetRelativeTM();
	
	A3DMATRIX4 matRot;
	m_qu.ConvertToMatrix(matRot);

	matControlled = InverseTM(matRelativeTM) * matRot * matRelativeTM *  matControlled;
	m_matController = matControlled;

	// no next controller, so look at controller have to be the last bone controller
	// in the chain
	if( 0 && m_pNextController )
		return m_pNextController->Update(nDeltaTime, pTargetBone, matControlled);
	return true;
}

void A3DBoneLookAtController::SetDegMaxMin(float vDegMax, float vDegMin, float vDegScale)
{
	m_vDegMin = vDegMin;
	m_vDegMax = vDegMax;
	m_vDegScale = vDegScale;
}

void A3DBoneLookAtController::SetPitchMaxMin(float vPitchMax, float vPitchMin, float vPitchScale)
{
	m_vPitchMin = vPitchMin;
	m_vPitchMax = vPitchMax;
	m_vPitchScale = vPitchScale;
}

void A3DBoneLookAtController::SetAxis(const A3DVECTOR3& vecEyeAxis, const A3DVECTOR3& vecHeadUpAxis)
{
	m_vecEyeAxisInBoneSpace = vecEyeAxis;
	m_vecHeadUpAxisInBoneSpace = vecHeadUpAxis;
}

//////////////////////////////////////////////////////////////////////////////////////
// Body Turn controller
//////////////////////////////////////////////////////////////////////////////////////
A3DBoneBodyTurnController::A3DBoneBodyTurnController(A3DBone * pTargetBone)
: A3DBoneController(pTargetBone), m_type(BODYTURN_AXIS_Y), m_vDegTurn(0.0f)
{
}

A3DBoneBodyTurnController::~A3DBoneBodyTurnController()
{
}

void A3DBoneBodyTurnController::SetTurnDeg(float vDegTurn)
{
	m_vDegTurn = vDegTurn;
}

void A3DBoneBodyTurnController::SetTurnType(BODYTURN_TYPE type)
{
	m_type = type;
}

bool A3DBoneBodyTurnController::Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled)
{
	A3DMATRIX4 matAbsParent = IdentityMatrix();
	if( m_pTargetBone->GetParentPtr() )
		matAbsParent = m_pTargetBone->GetParentPtr()->GetAbsoluteTM();

	A3DMATRIX4 matAbs = m_pTargetBone->GetRelativeTM() * matAbsParent;
	
	
	A3DVECTOR3 vecPos = matAbs.GetRow(3);
	A3DMATRIX4 matRotateInWorld = IdentityMatrix();
	
	A3DVECTOR3 vecAxis = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	switch(m_type)
	{
	case BODYTURN_AXIS_Y:
		vecAxis = A3DVECTOR3(0.0f, 1.0f, 0.0f);
		break;

	case BODYTURN_AXIS_X:
		vecAxis = A3DVECTOR3(1.0f, 0.0f, 0.0f);
		break;

	case BODYTURN_AXIS_Z:
		vecAxis = A3DVECTOR3(0.0f, 0.0f, 1.0f);
		break;

	case BODYTURN_AXIS_BONE:
		if( m_pTargetBone->GetChildNum() == 1 )
			vecAxis = Normalize(m_pTargetBone->GetChildPtr(0)->GetAbsoluteTM().GetRow(3) - vecPos);
		break;

	default:
		break;
	}

	matRotateInWorld = RotateAxis(vecPos, vecAxis, DEG2RAD(m_vDegTurn));
	m_matController = matAbsParent * matRotateInWorld * InverseTM(matAbsParent);
	matControlled = matControlled * m_matController;

	if( m_pNextController )
		return m_pNextController->Update(nDeltaTime, pTargetBone, matControlled);
	return true;
}
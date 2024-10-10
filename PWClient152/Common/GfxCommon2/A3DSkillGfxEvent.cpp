#include "StdAfx.h"
#include "A3DGfxExMan.h"
#include "A3DSkillGfxEvent.h"
#include "A3DGFXKeyPoint.h"
#include "A3DSkillGfxComposer.h"
#include "A3DSkinModelAux.h"

namespace _SGC
{

static const float _hit_radius		= .1f;
static const float _gravity			= 9.8e-6f;
static const float _missile_acc		= 20.0f;
static const float _missile_vel		= 5.0f;
static const float _missile_rot		= A3D_PI * 1.5f;
static const float _fall_height		= 10.0f;
static const float _fall_speed		= 8.0f;
static const float _ang_vel			= A3D_PI * 4.f / 1000.0f;

static const float _move_step_co	= 1.0f / 2.0f;
static const float _curved_acc		= 20.0e-6f;
static const float _max_yaw			= DEG2RAD(90.f);

#ifdef _ANGELICA21
#define HIT_GFX_MAX_TIMESPAN	2000
#else
#define HIT_GFX_MAX_TIMESPAN	5000
#endif
#define HIT_GFX_MAX_DELAY		3000

static const char* _format_link_moving = "Link_moving: %d";
static const char* _format_link_acc = "Link_acc: %f";

static const char* _format_parabolic_init_yaw = "Parabolic_init_yaw: %f";
static const char* _format_parabolic_angle_speed = "Parabolic_angle_speed: %f";
static const char* _format_parabolic_fix_amplitude = "Parabolic_fix_amplitude: %f";
static const char* _format_parabolic_proportional_amplitude = "Parabolic_proportional_amplitude: %f";
static const char* _format_parabolic_average = "Parabolic_average: %d";
static const char* _format_parabolic_half_average = "Parabolic_half_average: %d";
static const char* _format_parabolic_group = "Parabolic_group: %d";
static const char* _format_parabolic_group_delay = "Parabolic_group_delay: %f";
static const char* _format_parabolic_seg = "Parabolic_seg: %d";
static const char* _format_parabolic_lasting = "Parabolic_lasting: %d";

static const char* _format_missile_init_yaw = "Missile_init_yaw: %f";
static const char* _format_missile_curve_time = "Missile_curve_time: %d";
static const char* _format_missile_curve_amplitude = "Missile_curve_amplitude: %f";
static const char* _format_missile_average = "Missile_average: %d";
static const char* _format_missile_half_average = "Missile_half_average: %d";
static const char* _format_missile_curve_stay_time = "Missile_curve_stay_time: %d";
static const char* _format_missile_stay_amplitude = "Missile_stay_amplitude: %f";
static const char* _format_missile_stay_angle_speed = "Missile_stay_angle_speed: %f";
static const char* _format_missile_stay_self_rotate = "Missile_stay_self_rotate: %d";

static const char* _format_meteoric_fall_height = "Meteoric_fall_height: %f";
static const char* _format_meteoric_ground_height = "Meteoric_ground_height: %f";
static const char* _format_meteoric_radius = "Meteoric_radius: %f";
static const char* _format_meteoric_fix_radius = "Meteoric_fix_radius: %d";
static const char* _format_meteoric_x_offset = "Meteoric_x_offset: %f";
static const char* _format_meteoric_z_offset = "Meteoric_z_offset: %f";
static const char* _format_meteoric_random_offset = "Meteoric_random_offset: %d";
static const char* _format_meteoric_innercentric = "Meteoric_innercentric: %d";

void CGfxLinearMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	if (m_bArea)
	{
		CalcRange(Normalize(vTarget - vHost));
		m_vPos = vHost + GetRandOff();
	}
	else
		m_vPos = vHost;

	m_vMoveDir = Normalize(vTarget - m_vPos);
	m_fSpeed = m_fFlySpeed;

	m_bIsMoving = true;
}

bool CGfxLinearMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	if (!m_bIsMoving)		// 目标被击中
		return true;

	A3DVECTOR3 vFlyDir = vTargetPos - m_vPos;
	float fDist = vFlyDir.Normalize();
	float fFlyDist = m_fSpeed * dwDeltaTime;

	if (fFlyDist >= fDist)
	{
		fFlyDist = fDist;
		m_bIsMoving = false;
	}

	m_vPos += vFlyDir * fFlyDist;
	m_vMoveDir = vFlyDir;
	return false;
}

void CGfxParabolicMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	if (m_bArea)
	{
		CalcRange(Normalize(vTarget - vHost));
		m_vOrgPos = m_vPos = vHost + GetRandOff();
	}
	else
		m_vOrgPos = m_vPos = vHost;

	m_vHorzVel = vTarget - m_vPos;
	float fVert = m_vHorzVel.y;
	m_vHorzVel.y = 0;
	float fDist = m_vHorzVel.Normalize();
	m_fSpeed = m_fFlySpeed;
	m_fTotalTime = fDist / m_fSpeed;
	m_vHorzVel *= m_fSpeed;
	m_fVertVel = fVert / m_fTotalTime + .5f * _gravity * m_fTotalTime;
	m_vMoveDir = Normalize(m_vHorzVel + m_fVertVel * _unit_y);
	m_fCurTime = 0;

	m_bIsMoving = true;
}

bool CGfxParabolicMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	if (!m_bIsMoving)		// 目标被击中
		return true;

	m_fCurTime += dwDeltaTime;
	if (m_fCurTime >= m_fTotalTime)
	{
		m_fCurTime = m_fTotalTime;
		m_bIsMoving = false;
	}

	float fVertVel = m_fVertVel - _gravity * m_fCurTime;
	float fVertDist = .5f * (fVertVel + m_fVertVel) * m_fCurTime;
	m_vPos = m_vOrgPos + m_fCurTime * m_vHorzVel + fVertDist * _unit_y;
	m_vMoveDir = Normalize(m_vHorzVel + fVertVel * _unit_y);
	return false;
}

void CGfxMissileMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	if (m_bArea)
	{
		CalcRange(Normalize(vTarget - vHost));
		m_vOrgPos = m_vPos = vHost + GetRandOff();
	}
	else
		m_vOrgPos = m_vPos = vHost;

	m_vMoveDir = vTarget - m_vPos;
	m_vMoveDir.y = 0;
	m_vMoveDir.Normalize();
	float fAngle = A3D_PI / 2.0f + A3D_PI * _UnitRandom();
	A3DQUATERNION q(_unit_y, fAngle);
	m_vMoveDir = RotateVec(q, m_vMoveDir);
	m_fCurVel = _missile_vel;

	m_bIsMoving = true;
}

bool CGfxMissileMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	if (!m_bIsMoving)		// 目标被击中
		return true;

	static const float fLimit = (float)cos(DEG2RAD(15.0f));

	float fTime = dwDeltaTime / 1000.0f;
	A3DVECTOR3 vDir = vTargetPos - m_vPos;
	float fDist = m_fCurVel * fTime;

	float fLeft = vDir.Normalize();
	if (fDist >= fLeft)
	{
		fDist = fLeft;
		m_bIsMoving = false;
	}

	float fAngle = DotProduct(m_vMoveDir, vDir);

	if (fAngle > fLimit)
	{
		m_fCurVel += _missile_acc * fTime;
		m_vPos += vDir * fDist;
		m_vMoveDir = vDir;
	}
	else
	{
		m_vPos += m_vMoveDir * fDist;
		A3DVECTOR3 vUp = CrossProduct(m_vMoveDir, vDir);
		vUp.Normalize();
		A3DQUATERNION q(vUp, _missile_rot * fTime);
		m_vMoveDir = RotateVec(q, m_vMoveDir);
	}

	return false;
}

void CGfxMeteoricMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	m_vPos = vTarget;
	m_vPos.y += _fall_height;

	if (m_bOneOfCluser)
	{
		float fRandAng = _UnitRandom() * A3D_2PI;
		float fRadius = _UnitRandom() * m_fRadius;
		m_vPos.x += (float)cos(fRandAng) * fRadius;
		m_vPos.z += (float)sin(fRandAng) * fRadius;
	}

	m_vMoveDir = -_unit_y;
	m_vFallVel = _fall_speed * m_vMoveDir;

	m_bIsMoving = true;
}

bool CGfxMeteoricMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	if (!m_bIsMoving)		// 目标被击中
		return true;

	m_vPos += m_vFallVel * (dwDeltaTime / 1000.f);
	if (m_vPos.y <= vTargetPos.y)
	{
		m_vPos.y = vTargetPos.y;
		m_bIsMoving = true;
	}

	return false;
}

void CGfxHelixMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	if (m_bArea)
	{
		CalcRange(Normalize(vTarget - vHost));
		m_vCenter = vHost + GetRandOff();
	}
	else
		m_vCenter = vHost;

	m_vMoveDir = vTarget - m_vCenter;
	float fDist = m_vMoveDir.Normalize();
	float fMax = m_fFlySpeed * m_dwMaxFlyTime;

	if (fMax >= fDist)
		m_fSpeed = m_fFlySpeed;
	else
		m_fSpeed = fDist / m_dwMaxFlyTime;

	m_fFlyTime = fDist / m_fSpeed;
	m_fShrinkRate = m_fRadius / m_fFlyTime;

	if (fabs(m_vMoveDir.y) > .9f)
	{
		m_vAxisY = CrossProduct(m_vMoveDir, _unit_x);
		m_vAxisX = CrossProduct(m_vAxisY, m_vMoveDir);
	}
	else
	{
		m_vAxisX = CrossProduct(_unit_y, m_vMoveDir);
		m_vAxisY = CrossProduct(m_vMoveDir, m_vAxisX);
	}

	m_vPos = m_vCenter + m_vAxisX * m_fRadius;
	m_fAngle = 0;

	m_bIsMoving = true;
}

bool CGfxHelixMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	if (!m_bIsMoving)		// 目标被击中
		return true;

	float fDelta = static_cast<float>(dwDeltaTime);
	m_vCenter += m_vMoveDir * (m_fSpeed * fDelta);
	m_fRadius -= m_fShrinkRate * fDelta;
	m_fAngle -= _ang_vel * fDelta;
	A3DVECTOR3 vOff = m_vAxisX * (float)(cos(m_fAngle) * m_fRadius)
					+ m_vAxisY * (float)(sin(m_fAngle) * m_fRadius);
	m_vPos = m_vCenter + vOff;
	m_fFlyTime -= fDelta;
	if (m_fFlyTime <= 0)
	{
		m_fFlyTime = 0;
		m_bIsMoving = false;
	}

	return false;
}

void CGfxCurvedMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	if (m_bArea)
	{
		CalcRange(Normalize(vTarget - vHost));
		m_vPos = vHost + GetRandOff();
	}
	else
		m_vPos = vHost;

	m_vAxisZ = vTarget - m_vPos;
	float fDist = m_vAxisZ.Normalize();
	float fMax = m_dwMaxFlyTime * m_fFlySpeed;

	if (fMax >= fDist)
		m_fSpeed = m_fFlySpeed;
	else
		m_fSpeed = fDist / m_dwMaxFlyTime;

	m_fTime = fDist / m_fSpeed;
	m_fLatSpd = _curved_acc * .5f * m_fTime;

	if (fabs(m_vAxisZ.y) > .9f)
	{
		A3DVECTOR3 vY = CrossProduct(m_vAxisZ, _unit_x);
		m_vAxisX = CrossProduct(vY, m_vAxisZ);
	}
	else
		m_vAxisX = CrossProduct(_unit_y, m_vAxisZ);

	if (_UnitRandom() > .5f) m_vAxisX = -m_vAxisX;

	m_vZSpd = m_fSpeed * m_vAxisZ;
	m_vMoveDir = m_vZSpd + m_fLatSpd * m_vAxisX;
	m_vMoveDir.Normalize();

	m_bIsMoving = true;
}

bool CGfxCurvedMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	if (!m_bIsMoving)		// 目标被击中
		return true;

	float fDelta = static_cast<float>(dwDeltaTime);
	float fVEnd = m_fLatSpd - _curved_acc * fDelta;
	float fOff = .5f * (m_fLatSpd + fVEnd) * fDelta;
	m_fLatSpd = fVEnd;
	m_vPos += fDelta * m_vZSpd + fOff * m_vAxisX;
	m_vMoveDir = m_vZSpd + m_fLatSpd *m_vAxisX;
	m_vMoveDir.Normalize();
	m_fTime -= fDelta;

	if (m_fTime <= 0)
	{
		m_fTime = 0;
		m_bIsMoving = false;
	}

	return false;
}

void CGfxAccMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	m_fSpeed = 0;

	if (m_bArea)
	{
		CalcRange(Normalize(vTarget - vHost));
		m_vPos = vHost + GetRandOff();
	}
	else
		m_vPos = vHost;

	m_vMoveDir = Normalize(vTarget - m_vPos);

	m_bIsMoving = true;
}

bool CGfxAccMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	if (!m_bIsMoving)		// 目标被击中
		return true;

	float fDelta = dwDeltaTime / 1000.0f;
	A3DVECTOR3 vFlyDir = vTargetPos - m_vPos;
	float fDist = vFlyDir.Normalize();
	float fVEnd = m_fSpeed + m_fAcc * fDelta;
	float fFlyDist = (m_fSpeed + fVEnd) * fDelta * .5f;
	m_fSpeed = fVEnd;

	if (fFlyDist >= fDist) 
	{
		fFlyDist = fDist;
		m_bIsMoving = false;
	}	

	m_vPos += vFlyDir * fFlyDist;
	m_vMoveDir = vFlyDir;
	return false;
}

void CGfxOnTargetMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	m_vPos = vTarget;
	m_vMoveDir = m_bReverse ? vHost - vTarget : vTarget - vHost;
	m_vMoveDir.y = 0;

	if (m_vMoveDir.Normalize() == 0)
		m_vMoveDir = _unit_z;

	if (m_bOneOfCluser)
	{
		float fRandAng = _UnitRandom() * A3D_2PI;
		float fRadius = _UnitRandom() * m_fRadius;
		m_vOffset.x = cosf(fRandAng) * fRadius;
		m_vOffset.z = sinf(fRandAng) * fRadius;
		m_vOffset.y = 0;
		m_vPos += m_vOffset;
	}
	else
		m_vOffset.Clear();
}

bool CGfxOnTargetMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	m_vPos = vTargetPos + m_vOffset;
	return false;
}

bool CGfxOnTargetFixMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	return false;
}

void CGfxLinkMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	for (size_t i = 0; i < m_Params.size(); i++)
		delete m_Params[i];
	m_Params.clear();

	m_vPos = vHost;
	m_vMoveDir = Normalize(vTarget - vHost);

	m_bIsMoving = false;
	m_fSpeed = m_fFlySpeed;
	if (m_pMoveParam)
	{
		GFX_CUSTOM_LINK_MOVE_PARAM* pParam = (GFX_CUSTOM_LINK_MOVE_PARAM*)m_pMoveParam;
		m_bIsMoving = pParam->m_bMoving;
		m_fAcc = pParam->m_fAcc * 0.001f;
		m_vDestPos = m_vPos;
	}
}

bool CGfxLinkMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	if (m_bIsMoving)
	{
		A3DVECTOR3 vFlyDir = vTargetPos - m_vDestPos;
		float fDist = vFlyDir.Normalize();
		m_fSpeed += m_fAcc * dwDeltaTime;
		float fFlyDist = m_fSpeed * dwDeltaTime;

		if (fFlyDist >= fDist)
		{
			fFlyDist = fDist;
			m_bIsMoving = false;
		}

		m_vDestPos += vFlyDir * fFlyDist;
	}

	m_vPos = vHostPos;
	m_vMoveDir = Normalize(vTargetPos - vHostPos);
	return false;
}

void CGfxLinkMove::UpdateGfxParam(A3DGFXEx* pGfx, const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	A3DVECTOR3 vDest;
	if (!m_bIsMoving)
		vDest = vTarget;
	else
		vDest = m_vDestPos;

	if (m_Params.size() == 0) pGfx->GetParamInfoList(m_Params);
	for (size_t i = 0; i < m_Params.size(); i++)
	{
		GfxParamInfo* pInfo = m_Params[i];

		switch (pInfo->m_nParamId)
		{
		case ID_PARAM_LTN_POS1:
			pGfx->UpdateEleParam(pInfo->m_strEleName, ID_PARAM_LTN_POS1, vHost);
			break;
		case ID_PARAM_LTN_POS2:
			pGfx->UpdateEleParam(pInfo->m_strEleName, ID_PARAM_LTN_POS2, vDest);
			break;
		}
	}
}

inline void CGfxRandMove::CalcStep()
{
	m_fStep = 0;
	if (m_vSize.x > m_fStep) m_fStep = m_vSize.x;
	if (m_vSize.y > m_fStep) m_fStep = m_vSize.y;
	if (m_vSize.z > m_fStep) m_fStep = m_vSize.z;
	m_fStep *= _move_step_co;
}

void CGfxRandMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	CalcStep();
	m_fTimeSpan = m_fStep / m_fSpeed;
	m_fCurSpan = 0;

	if (m_bArea)
	{
		CalcRange(Normalize(vTarget - vHost));
		m_vPos = vTarget + GetRandOff();
	}
	else
		m_vPos = vTarget;

	m_vMoveDir.x = _UnitRandom();
	m_vMoveDir.y = _UnitRandom();
	m_vMoveDir.z = _UnitRandom();
	m_vMoveDir.Normalize();

	m_CtrlPoints[0] = m_vPos;
	m_CtrlPoints[3] = GetNextStep(m_vPos, m_vMoveDir, vTarget);
	A3DVECTOR3 vDelta1 = m_CtrlPoints[3] - m_vPos;
	m_CtrlPoints[5] = GetNextStep(m_CtrlPoints[3], Normalize(vDelta1), vTarget);

	A3DVECTOR3 vDiff = (m_CtrlPoints[5] - m_vPos) / 6.0f;
	m_CtrlPoints[2] = m_CtrlPoints[3] - vDiff;
	m_CtrlPoints[4] = m_CtrlPoints[3] + vDiff;
	m_CtrlPoints[1] = m_CtrlPoints[2] + vDelta1 / 3.0f;
}

bool CGfxRandMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	m_fCurSpan += dwDeltaTime / 1000.0f;

	if (m_fCurSpan >= m_fTimeSpan)
	{
		m_vPos = m_CtrlPoints[3];
		m_fCurSpan = 0;

		m_CtrlPoints[0] = m_vPos;
		m_CtrlPoints[3] = m_CtrlPoints[5];
		A3DVECTOR3 vDelta1 = m_CtrlPoints[3] - m_vPos;
		m_CtrlPoints[5] = GetNextStep(m_CtrlPoints[3], Normalize(vDelta1), vTargetPos);

		m_CtrlPoints[1] = m_CtrlPoints[4];
		A3DVECTOR3 vDiff = (m_CtrlPoints[5] - m_vPos) / 6.0f;
		m_CtrlPoints[2] = m_CtrlPoints[3] - vDiff;
		m_CtrlPoints[4] = m_CtrlPoints[3] + vDiff;
	}
	else
	{
		A3DVECTOR3 vOldPos = m_vPos;

		m_vPos = Bezier4(
			m_CtrlPoints[0],
			m_CtrlPoints[1],
			m_CtrlPoints[2],
			m_CtrlPoints[3],
			m_fCurSpan / m_fTimeSpan
			);

		A3DVECTOR3 vNewDir = m_vPos - vOldPos;
		float fMag = vNewDir.Normalize();
		if (fMag > .0001f) m_vMoveDir = vNewDir;
	}

	return false;
}

A3DVECTOR3 CGfxRandMove::GetNextStep(
	const A3DVECTOR3& vPos,
	const A3DVECTOR3& vDir,
	const A3DVECTOR3& vCenter) const
{
	A3DVECTOR3 vUp, vNewDir, vNewPos;

	vUp = CalcVertVec(vDir);
	A3DQUATERNION q(vDir, _UnitRandom() * A3D_2PI);
	vUp = RotateVec(q, vUp);

	A3DQUATERNION q2(vUp, _UnitRandom() * _max_yaw);
	vNewDir = RotateVec(q2, vDir);
	vNewPos = vPos + vNewDir * m_fStep;

	if (!IsPosInRange(vNewPos - vCenter))
	{
		vNewDir = Normalize(vCenter - vPos);
		vNewPos = vPos + vNewDir * m_fStep;
	}

	return vNewPos;
}


GETPOSITIONBYID_PARAMS::GETPOSITIONBYID_PARAMS(clientid_t nId
											   , const SGC_POS_INFO& sgcPosInfo
											   , void* pUserData)
{
	init(nId, sgcPosInfo.HitPos, sgcPosInfo.szHook, sgcPosInfo.bRelHook, &sgcPosInfo.vOffset, sgcPosInfo.szHanger, sgcPosInfo.bChildHook, pUserData);
}


//////////////////////////////////////////////////////////////////////////
//
//	Implement A3DSkillGfxEvent
//
//////////////////////////////////////////////////////////////////////////

A3DSkillGfxEvent::A3DSkillGfxEvent(A3DSkillGfxMan* pMan, GfxMoveMode mode) :
	m_pMan(pMan),
	m_pComposer(0),
	m_pFlyGfx(0),
	m_nHostID(0),
	m_fFlySpeed(0),
	m_dwCurSpan(0),
	m_dwHitSpan(0),
	m_nDivisions(1),
	m_enumState(enumWait),
	m_pPrvEvent(0),
	m_pSblEvent(0),
	m_bHasTarget(false),
	m_vFixedPoint(0),
	m_bTargetAdded(false),
	m_FlyHitMode(enumAttBothFlyHit),
	m_nFlySfxPriority(0),
	m_nHitSfxPriority(0),
	m_bGfxUseLod(true),
	m_bGfxDisableCamShake(false),
	m_bHostECMCreatedByGfx(false),
	m_dwUserData(0),
	m_nAttIndex(0),
	m_nAttOrientation(0),
	m_fGfxScale(1.f),
	m_bRandomHook(false),
	m_bAllExtraHit(false)
{
	memset(m_strHook,0,80);

	m_pMoveMethod = CGfxMoveBase::CreateMoveMethod(mode);
	m_pMoveMethod->SetSkillEvent(this);
}
	
A3DSkillGfxEvent::~A3DSkillGfxEvent()
{
	ReleaseGfx(false);
	delete m_pMoveMethod;
}

void A3DSkillGfxEvent::FlushTargetsInfo()
{
	if (m_TargetsInfo.empty())
		return;

	DWORD dwModifier = 0;

	for (size_t i = 0; i < m_TargetsInfo.size(); i++)
	{
		const TARGET_DATA& td = m_TargetsInfo[i];
		dwModifier |= td.dwModifier;

		// 显示伤害数值
		if (A3DSkillGfxMan::GetDamageShowFunc())
			A3DSkillGfxMan::GetDamageShowFunc()(m_nCasterID, td.idTarget, td.nDamage, m_nDivisions, td.dwModifier);
	}

	// 显示攻击者的特殊信息
	if (A3DSkillGfxMan::GetCasterShowFunc())
		A3DSkillGfxMan::GetCasterShowFunc()(m_nCasterID, dwModifier);

	m_TargetsInfo.clear();
}

void A3DSkillGfxEvent::Tick(DWORD dwDeltaTime)
{
	m_dwCurSpan += dwDeltaTime;

	SGC_POS_INFO posHitInfo = m_pComposer->m_HitPos;

	if(IsRandomHook())
		strcpy_s(posHitInfo.szHook, 80, m_strHook);

	// 无击中效果，则立即显示伤害
	if (m_pComposer->m_szHitGfx[0] == 0 && m_TargetsInfo.size())
	{
		FlushTargetsInfo();
	}

	if (m_enumState == enumFinished)
		return; //结束

	if (m_enumState == enumHit) // 击中
	{
		m_dwHitSpan += dwDeltaTime;

		FlushTargetsInfo();

		// Tick效果，判断是否结束
		if (m_HitGfxArr.size())
		{
			size_t i = 0;

			while (i < m_HitGfxArr.size())
			{
				HitGfxInfo& info = m_HitGfxArr[i];
				A3DGFXEx* pHit = info.hit_gfx;

				bool bReleaseHixGfx = false;
				if (pHit->GetState() == ST_STOP)
					bReleaseHixGfx = true;
				else
				{
					if (info.is_infinite == -1 && pHit->IsResourceReady())
						info.is_infinite = (pHit->IsInfinite() ? 1 : 0);

					if (info.is_infinite == 1 && pHit->GetTimeElapse() > HIT_GFX_MAX_TIMESPAN)
						bReleaseHixGfx = true;
				}

				if (bReleaseHixGfx)
				{
					AfxGetGFXExMan()->CacheReleasedGfx(pHit);
					m_HitGfxArr.erase(m_HitGfxArr.begin() + i);
					continue;
				}

				if ((info.id_target == m_nCastTargetID && m_pComposer->m_bTraceTarget && !info.hit_ground) ||
					(info.id_target != m_nCastTargetID && m_pComposer->m_bExtraTraceTarget && !info.hit_ground))
				{
					A3DVECTOR3 vPos;

					if (m_pMan->GetPositionById(GETPOSITIONBYID_PARAMS(info.id_target, posHitInfo, NULL),  vPos))
						pHit->SetPos(vPos);
				}

				pHit->TickAnimation(dwDeltaTime);
				i++;
			}
		}
		else
		{
			m_enumState = enumFinished;
			return;
		}
	}
	else if ((m_FlyHitMode & enumAttFlyOnly) == 0)
		HitTarget(IsRandomHook() ? m_strHook : NULL);
	else if (m_dwCurSpan > m_pComposer->m_dwFlyTime + m_pComposer->m_dwStayTime) //飞行超时
		HitTarget(IsRandomHook() ? m_strHook : NULL);
	else
	{
		A3DVECTOR3 vHost, vTarget;
		const SGC_POS_INFO *pHostPos, *pTargetPos;

		if (m_pMoveMethod->IsReverse())
		{
			pHostPos = &m_pComposer->m_FlyEndPos;
			pTargetPos = &m_pComposer->m_FlyPos;
		}
		else
		{
			pHostPos = &m_pComposer->m_FlyPos;
			pTargetPos = &m_pComposer->m_FlyEndPos;
		}

		if (m_nHostID)
		{
			if (!m_pMan->GetPositionById(GETPOSITIONBYID_PARAMS(m_nHostID, *pHostPos, NULL), vHost))
			{
				HitTarget(IsRandomHook() ? m_strHook : NULL);
				return;
			}
		}
		else
			vHost = m_vFixedPoint;

		if (m_nTargetID)
		{
// 			if(IsRandomHook())			//飞行结束点和击中点一致
// 				strcpy_s(targetPos.szHook, 80, m_strHook);
// 			
// 			if (!m_pMan->GetPositionById(GETPOSITIONBYID_PARAMS(m_nTargetID, targetPos, NULL), vTarget))
// 			{
// 				HitTarget(IsRandomHook() ? m_strHook : NULL);
// 				return;
// 			}

			if (!m_pMan->GetPositionById(m_nTargetID, vTarget,
				pTargetPos->HitPos,
				IsRandomHook() ? m_strHook : pTargetPos->szHook,		//飞行结束点和击中点一致
				pTargetPos->bRelHook,
				&pTargetPos->vOffset,
				pTargetPos->szHanger,
				pTargetPos->bChildHook))
			{
				HitTarget(IsRandomHook() ? m_strHook : NULL);
				return;
			}

		}
		else
			vTarget = m_vFixedPoint;

		if (m_enumState == enumWait)
		{
			if (m_dwCurSpan < m_dwDelayTime) return;

			m_enumState = enumFlying;
			m_pMoveMethod->SetFlySpeed(m_fFlySpeed / 1000.0f);
			m_pMoveMethod->SetMaxFlyTime(m_pComposer->m_dwFlyTime);
			m_pMoveMethod->StartMove(vHost, vTarget);

			if (m_pFlyGfx)
			{
				A3DVECTOR3 vDir, vUp;

				bool bParentDir = (m_pMoveMethod->GetMode() == enumOnTarget ||
					m_pMoveMethod->GetMode() == enumOnTargetFix) &&
					m_pMoveMethod->IsReverse() && 
					m_pMan->GetDirAndUpById(m_nTargetID, vDir, vUp);

				if (bParentDir)
					m_pFlyGfx->SetParentTM(a3d_TransformMatrix(vDir, vUp, m_pMoveMethod->GetPos()));
				else
					m_pFlyGfx->SetParentTM(_build_matrix(m_pMoveMethod->GetMoveDir(), m_pMoveMethod->GetPos()));

				m_pMoveMethod->UpdateGfxParam(m_pFlyGfx, vHost, vTarget);
				m_pFlyGfx->SetSfxPriority(m_nFlySfxPriority);
				m_pFlyGfx->Start(true);
				m_pFlyGfx->TickAnimation(0);
			}

			if (A3DSkillGfxMan::GetSkillGfxTickFunc())
				A3DSkillGfxMan::GetSkillGfxTickFunc()(m_nCasterID, enumAttStateStart, 0, &m_pMoveMethod->GetMoveDir(), &m_pMoveMethod->GetPos());
		}
		else
		{
			if (m_dwCurSpan > m_dwDelayTime + m_pComposer->m_dwStayTime && m_pMoveMethod->TickMove(dwDeltaTime, vHost, vTarget)) // 目标被击中
				HitTarget(IsRandomHook() ? m_strHook : NULL);
			else
			{
				if (m_pFlyGfx)
				{
					A3DVECTOR3 vDir, vUp;

					bool bParentDir = (m_pMoveMethod->GetMode() == enumOnTarget ||
						m_pMoveMethod->GetMode() == enumOnTargetFix) &&
						m_pMoveMethod->IsReverse() && 
						m_pMan->GetDirAndUpById(m_nTargetID, vDir, vUp);

					if (bParentDir)
						m_pFlyGfx->SetParentTM(a3d_TransformMatrix(vDir, vUp, m_pMoveMethod->GetPos()));
					else
						m_pFlyGfx->SetParentTM(_build_matrix(m_pMoveMethod->GetMoveDir(), m_pMoveMethod->GetPos()));

					m_pMoveMethod->UpdateGfxParam(m_pFlyGfx, vHost, vTarget);
					m_pFlyGfx->TickAnimation(dwDeltaTime);
				}

				if (A3DSkillGfxMan::GetSkillGfxTickFunc())
					A3DSkillGfxMan::GetSkillGfxTickFunc()(m_nCasterID, enumAttStateFlying, dwDeltaTime, &m_pMoveMethod->GetMoveDir(), &m_pMoveMethod->GetPos());
			}
		}
	}
}

void A3DSkillGfxEvent::Render()
{
	if (m_pFlyGfx) AfxGetGFXExMan()->RegisterGfx(m_pFlyGfx);
	
	for (size_t i = 0; i < m_HitGfxArr.size(); i++)
	{
		A3DGFXEx* pGfx = m_HitGfxArr[i].hit_gfx;
		
		if (pGfx && pGfx->GetState() != ST_STOP)
			AfxGetGFXExMan()->RegisterGfx(pGfx);
	}
}

A3DGFXEx* A3DSkillGfxEvent::LoadFlyGfx(A3DDevice* pDev, const char* szPath) 
{ 
	return AfxGetGFXExMan()->LoadGfx(pDev, szPath); 
}

A3DGFXEx* A3DSkillGfxEvent::LoadHitGfx(A3DDevice* pDev, const char* szPath, DWORD dwModifier) 
{ 
	return AfxGetGFXExMan()->LoadGfx(pDev, szPath); 
}

void A3DSkillGfxEvent::PlayExtraHitGfx(DWORD dwModifier, bool bOnTarget)
{
	A3DVECTOR3 vTarget;
	A3DVECTOR3 vHost;
	A3DVECTOR3 vDir;

	if (bOnTarget)
	{
		if (m_nTargetID)
		{
			if (!m_pMan->GetPositionById(GETPOSITIONBYID_PARAMS(m_nTargetID, m_pComposer->m_GroundPos, NULL), vTarget))
			{
				return;
			}
		}
		else
			vTarget = m_vFixedPoint;
	}
	else
	{
		vTarget = m_pMoveMethod->GetPos();
	}

	SGC_POS_INFO hostPosInfo;
	hostPosInfo.HitPos = enumHitBottom;
	if (!m_pMan->GetPositionById(GETPOSITIONBYID_PARAMS(m_nHostID, hostPosInfo, NULL), vHost))
		return;

	vDir = vTarget - vHost;
	vDir.y = 0;
	vDir.Normalize();

	A3DGFXEx* pGfx = AfxGetGFXExMan()->LoadGfx(m_pMan->m_pDevice, m_pComposer->m_szHitGrndGfx);

	if (pGfx)
	{
		pGfx->SetPos(vTarget);
		pGfx->SetDirAndUp(vDir, _unit_y);
		pGfx->SetSfxPriority(m_nHitSfxPriority);
		pGfx->SetId(GetCasterID(), 0);
		pGfx->SetDisableCamShake(GetDisableCamShake());
		pGfx->SetCreatedByGFXECM(GetHostModelCreatedByGfx());
		pGfx->SetUseLOD(GetGfxUseLod());
		pGfx->Start(true);
		pGfx->TickAnimation(0);

		HitGfxInfo info;
		info.hit_gfx = pGfx;
		info.is_infinite = pGfx->IsResourceReady() ? (pGfx->IsInfinite() ? 1 : 0) : -1;
		info.id_target = m_nCastTargetID;
		info.hit_ground = true;
		m_HitGfxArr.push_back(info);
	}
}

void A3DSkillGfxEvent::PlayHitGfxOnAllTargets()
{
	for (TargetDataVec::const_iterator itr = m_TargetsInfo.begin(); itr != m_TargetsInfo.end(); ++itr)
	{
		if (itr->idTarget == m_nCastTargetID)
			continue;

		PlayHitGfxOnTarget(itr->idTarget);
	}
}

void A3DSkillGfxEvent::PlayHitGfxOnTarget(clientid_t idTarget, const char* szHook)
{
	A3DVECTOR3 vTarget;

// 	SGC_POS_INFO targetPosInfo = m_pComposer->m_HitPos;
// 	if (szHook)
// 	{
// 		strcpy_s(targetPosInfo.szHook, 80, szHook);
// 	}
	
// 	if (!m_pMan->GetPositionById(GETPOSITIONBYID_PARAMS(idTarget, targetPosInfo, NULL), vTarget))
// 		return;

	if (!m_bAllExtraHit && idTarget == m_nCastTargetID)
	{
		if (!m_pMan->GetPositionById(idTarget, vTarget, 
			m_pComposer->m_HitPos.HitPos,
			szHook ? szHook : m_pComposer->m_HitPos.szHook,
			m_pComposer->m_HitPos.bRelHook,
			&m_pComposer->m_HitPos.vOffset,
			m_pComposer->m_HitPos.szHanger,
			m_pComposer->m_HitPos.bChildHook))
			return;

		if (m_pComposer->m_szHitGfx[0])
		{
			A3DGFXEx* pGfx = LoadHitGfx(m_pMan->m_pDevice, m_pComposer->m_szHitGfx, 0);

			if (pGfx)
			{
				A3DVECTOR3 vHost;
				A3DVECTOR3 vDir;

				SGC_POS_INFO posInfo;
				posInfo.HitPos = enumHitCenter;
				if (m_pMan->GetPositionById(GETPOSITIONBYID_PARAMS(m_nHostID, posInfo, NULL), vHost))
				{
					vDir = vTarget - vHost;
					vDir.y = 0;

					if (vDir.Normalize() < 1e-3)
						vDir = _unit_z;
				}
				else
					vDir = _unit_z;

				A3DVECTOR3 vUp = _unit_y;
				if (!m_pComposer->m_vHitGfxDir.IsZero())
				{
	 				vDir = m_pComposer->m_vHitGfxDir * a3d_TransformMatrix(vDir, _unit_y, _unit_zero);
	 
	 				if (vDir.Normalize() < 1e-3)
	 					vDir = _unit_z;
	 
	 				if (fabs(vDir.y) > 0.999)
	 					vUp = Normalize(vHost - vTarget);
	 			}

				if (m_pComposer->m_bRelScl)
				{
					float fRelScale = m_pMan->GetTargetScale(idTarget) / m_pComposer->m_fDefTarScl * m_pComposer->m_fHitGfxScale;
					a_Clamp(fRelScale, m_pComposer->m_fMinHitGfxScaleFinal, m_pComposer->m_fMaxHitGfxScaleFinal);
					pGfx->SetScale(fRelScale);
				}
				else
					pGfx->SetScale(m_pComposer->m_fHitGfxScale);

				pGfx->SetPos(vTarget);
				pGfx->SetDirAndUp(vDir, vUp);
				pGfx->SetSfxPriority(m_nHitSfxPriority);
				pGfx->SetUseLOD(GetGfxUseLod());
				pGfx->SetDisableCamShake(GetDisableCamShake());
				pGfx->SetCreatedByGFXECM(GetHostModelCreatedByGfx());
				pGfx->SetId(idTarget, 0);
				pGfx->Start(true);
				pGfx->TickAnimation(0);

				HitGfxInfo info;
				info.hit_gfx = pGfx;
				info.is_infinite = pGfx->IsResourceReady() ? (pGfx->IsInfinite() ? 1 : 0) : -1;
				info.id_target = idTarget;
				info.hit_ground = false;
				m_HitGfxArr.push_back(info);
			}
		}		

		return;
	}

	//extra hit
	if (!m_pMan->GetPositionById(idTarget, vTarget,
		m_pComposer->m_extraHitPos))
		return;
	
	if (m_pComposer->m_szExtraHitGfx[0])
	{
		A3DGFXEx* pGfx = LoadHitGfx(m_pMan->m_pDevice, m_pComposer->m_szExtraHitGfx, 0);

		if (pGfx)
		{
			A3DVECTOR3 vHost;
			A3DVECTOR3 vDir;

			SGC_POS_INFO posInfo;
			posInfo.HitPos = enumHitCenter;
			if (m_pMan->GetPositionById(GETPOSITIONBYID_PARAMS(m_nHostID, posInfo, NULL), vHost))
			{
				vDir = vTarget - vHost;
				vDir.y = 0;

				if (vDir.Normalize() < 1e-3)
					vDir = _unit_z;
			}
			else
				vDir = _unit_z;

			A3DVECTOR3 vUp = _unit_y;

			if (m_pComposer->m_bExtraRelScl)
			{
				float fRelScale = m_pMan->GetTargetScale(idTarget) / m_pComposer->m_fExtraDefTarScl * m_pComposer->m_fHitGfxScale;
				a_Clamp(fRelScale, m_pComposer->m_fExtraMinHitGfxScaleFinal, m_pComposer->m_fExtraMaxHitGfxScaleFinal);
				pGfx->SetScale(fRelScale);
			}
			else
				pGfx->SetScale(m_pComposer->m_fHitGfxScale);

			pGfx->SetPos(vTarget);
			pGfx->SetDirAndUp(vDir, vUp);
			pGfx->SetSfxPriority(m_nHitSfxPriority);
			pGfx->SetUseLOD(GetGfxUseLod());
			pGfx->SetDisableCamShake(GetDisableCamShake());
			pGfx->SetCreatedByGFXECM(GetHostModelCreatedByGfx());
			pGfx->SetId(idTarget, 0);
			pGfx->Start(true);
			pGfx->TickAnimation(0);

			HitGfxInfo info;
			info.hit_gfx = pGfx;
			info.is_infinite = pGfx->IsResourceReady() ? (pGfx->IsInfinite() ? 1 : 0) : -1;
			info.id_target = idTarget;
			info.hit_ground = false;
			m_HitGfxArr.push_back(info);
		}
	}
}

//	call back at the start point of hit target
void A3DSkillGfxEvent::OnPreHitTarget()
{
	if (A3DSkillGfxMan::GetSkillGfxTickFunc())
		A3DSkillGfxMan::GetSkillGfxTickFunc()(m_nCasterID, enumAttStateHit, 0, &m_pMoveMethod->GetMoveDir(), &m_pMoveMethod->GetPos());

	if (A3DSkillGfxMan::GetSkillGfxTickHitTargetFunc())
	{
		SKILL_GFX_HITTARGET_PARAM param;
		param.idCaster = m_nCasterID;
		param.idTarget = m_nCastTargetID;
		param.userData = m_dwUserData;
		param.pComposer = m_pComposer;
		param.nAttIndex = m_nAttIndex;
		param.nAttOrientation = m_nAttOrientation;
		A3DSkillGfxMan::GetSkillGfxTickHitTargetFunc()(&param);
	}
}

void A3DSkillGfxEvent::HitTarget(const char* szHook)
{
	OnPreHitTarget();

	ReleaseFlyGfx();

	m_enumState = enumFinished;

	if (m_FlyHitMode & enumAttHitGround)
	{
		// 播防额外的击中效果
		m_enumState = enumHit;
		PlayExtraHitGfx(0, m_pMoveMethod->GetMode()!=enumCustomMeteoricMove);
	}

	if (m_FlyHitMode & enumAttHitOnly)
	{
		// 播放击中效果
		m_enumState = enumHit;

		PlayHitGfx(szHook);
	}
}

void A3DSkillGfxEvent::PlayHitGfx(const char* szHook)
{
	PlayHitGfxOnTarget(m_nCastTargetID, szHook);
}

void A3DSkillGfxEvent::ReleaseFlyGfx(bool bCache)
{
	if (m_pFlyGfx)
	{
		if (bCache)
		{
			// only need to test StopEmit in cached situation
			if (m_pComposer->m_bStopEmit)
				m_pFlyGfx->StopParticleEmit();

			if (m_pComposer->m_bFadeOut)
				AfxGetGFXExMan()->QueueFadeOutGfx(m_pFlyGfx, 1000, m_pComposer->m_dwDelayTime);
			else
				AfxGetGFXExMan()->CacheReleasedGfx(m_pFlyGfx);
		}
		else
		{
			m_pFlyGfx->Release();
			delete m_pFlyGfx;
		}

		m_pFlyGfx = NULL;
	}
}

void A3DSkillGfxEvent::ReleaseGfx(bool bCache)
{
	ReleaseFlyGfx(bCache);

	for (size_t i = 0; i < m_HitGfxArr.size(); i++)
	{
		A3DGFXEx* pGfx = m_HitGfxArr[i].hit_gfx;

		if (pGfx)
		{
			if (bCache)
				AfxGetGFXExMan()->CacheReleasedGfx(pGfx);
			else
			{
				pGfx->Release();
				delete pGfx;
			}
		}
	}

	m_HitGfxArr.clear();
}

void A3DSkillGfxEvent::Resume()
{
	ReleaseGfx();
	m_enumState = enumWait;
	m_nDivisions= 1;
	m_dwCurSpan = 0;
	m_dwHitSpan = 0;
	m_pPrvEvent = 0;
	m_pSblEvent = 0;
	m_TargetsInfo.clear();
	m_bTargetAdded = false;
	m_FlyHitMode = enumAttBothFlyHit;
	m_nFlySfxPriority = 0;
	m_nHitSfxPriority = 0;
	m_bGfxUseLod = true;
	m_bGfxDisableCamShake = false;
	m_bHostECMCreatedByGfx = false;
}

bool A3DSkillGfxEvent::AddOneTarget(const TARGET_DATA& td)
{
	A3DSkillGfxEvent* pEvent = this;

	while (pEvent && pEvent->m_SerialId == m_SerialId)
	{
		A3DSkillGfxEvent* pNext = pEvent->m_pSblEvent;

		if (!pEvent->m_bHasTarget || pEvent->IsFinished())
		{
			pEvent = pNext;
			continue;
		}

		if (!pEvent->GetTargetAddedFlag() || !pNext || pNext->m_SerialId != m_SerialId)
		{
			pEvent->m_TargetsInfo.push_back(td);
			pEvent->SetTargetAddedFlag(true);
			return true;
		}

		pEvent = pNext;
	}

	return false;
}

SHOW_DAMAGE_VALUE A3DSkillGfxMan::m_DamageShowFunc = 0;

void A3DSkillGfxMan::SetDamageShowFunc(SHOW_DAMAGE_VALUE func)
{
	m_DamageShowFunc = func;
}

SHOW_DAMAGE_VALUE A3DSkillGfxMan::GetDamageShowFunc()
{
	return m_DamageShowFunc;
}

SHOW_CASTER_INFO A3DSkillGfxMan::m_CasterShowFunc = 0;

void A3DSkillGfxMan::SetCasterShowFunc(SHOW_CASTER_INFO func)
{
	m_CasterShowFunc = func;
}

SHOW_CASTER_INFO A3DSkillGfxMan::GetCasterShowFunc()
{
	return m_CasterShowFunc;
}

SKILL_GFX_TICK_EVENT A3DSkillGfxMan::m_SkillGfxTickFunc = 0;

void A3DSkillGfxMan::SetSkillGfxTickFunc(SKILL_GFX_TICK_EVENT func)
{
	m_SkillGfxTickFunc = func;
}

SKILL_GFX_TICK_EVENT A3DSkillGfxMan::GetSkillGfxTickFunc()
{
	return m_SkillGfxTickFunc;
}


SKILL_GFX_HITTARGET A3DSkillGfxMan::m_SkillGfxHitTargetFunc = 0;

void A3DSkillGfxMan::SetSkillGfxHitTargetFunc(SKILL_GFX_HITTARGET func)
{
	m_SkillGfxHitTargetFunc = func;
}

SKILL_GFX_HITTARGET A3DSkillGfxMan::GetSkillGfxTickHitTargetFunc()
{
	return m_SkillGfxHitTargetFunc;
}

bool A3DSkillGfxMan::AddSkillGfxEvent(A3DSkillGfxComposer* pComposer, const SKILL_GFX_EVENT_PARAMS& params, int nIndex)
{
	A3DSkillGfxEvent* pEvent = GetEmptyEvent(pComposer->m_MoveMode);
	if (!pEvent)
		return false;

	pEvent->SetComposer(pComposer);
	pEvent->SetCasterID(params.nCasterID);
	pEvent->SetCastTargetID(params.nCastTargetID);
	pEvent->SetSerialId(params.SerialID);

	if (params.pFixedPt)
		pEvent->SetFixedPoint(*params.pFixedPt);

	pEvent->SetHostID(params.nHostID);
	pEvent->SetTargetID(params.nTargetID);
	pEvent->SetFlySpeed(params.fFlySpeed);
	pEvent->SetDelay(params.dwDelayTime);
	pEvent->SetHasTarget(params.bHasTarget);
	pEvent->SetReverse(params.bReverse);
	pEvent->SetIsCluster(pComposer->m_FlyCluster.m_ulCount > 1,nIndex, pComposer->m_FlyCluster.m_ulCount);
	pEvent->SetParam(&pComposer->m_param);
	pEvent->SetAllExtraHit(params.bAllExtraHit);

	pEvent->SetCustomMoveParam(pComposer->m_pGfxCustomMoveParam);

	ECMODEL_GFX_PROPERTY Prop;
	if (GetPropertyById(params.nCasterID, &Prop))
	{
		pEvent->SetGfxUseLod(Prop.bGfxUseLod);
		pEvent->SetDisableCamShake(Prop.bGfxDisableCamShake);
		pEvent->SetHostModelCreatedByGfx(Prop.bHostECMCreatedByGfx);
	}

#ifdef _ANGELICA21
	if (pComposer->m_szFlyGfx[0] && (params.mode & enumAttFlyOnly))
#else
	if (pComposer->m_szFlyGfx[0])
#endif
	{
		A3DGFXEx* pGfx = pEvent->LoadFlyGfx(m_pDevice, pComposer->m_szFlyGfx);

		if (pGfx)
		{
			pGfx->SetScale(pComposer->m_fFlyGfxScale);
			pGfx->SetDisableCamShake(pEvent->GetDisableCamShake());
			pGfx->SetCreatedByGFXECM(pEvent->GetHostModelCreatedByGfx());
			pGfx->SetUseLOD(pEvent->GetGfxUseLod());
			pGfx->SetId(pEvent->GetCasterID(), 0);
			pEvent->SetFlyGfx(pGfx);
		}
	}

	pEvent->SetFlyHitMode(params.mode);
	pEvent->SetFlySfxPriority(params.nFlySfxPriority);
	pEvent->SetHitSfxPriority(params.nHitSfxPriority);
	pEvent->SetAttIndex(params.nAttIndex);		
	pEvent->SetAttOrientation(params.nAttOrientation);
	pEvent->SetGfxScale(params.fGfxScale);

	char strHook[80] = {0};
	bool randomHook = pComposer->m_HitPos.RandomHook(strHook);
		
	pEvent->SetRandomHook(randomHook);
	if(randomHook)
		pEvent->SetHitHook(strHook);

	pEvent->Tick(0);
	PushEvent(params.nCasterID, pEvent);
	return true;
}


GFX_CUSTOM_MOVE_PARAM* GFX_CUSTOM_MOVE_PARAM::CreateCustomMoveParam( GfxMoveMode nMode )
{
	switch(nMode)
	{
	case enumLink:
		return new GFX_CUSTOM_LINK_MOVE_PARAM(nMode);
	case enumCustomParabolicMove:
		return new GFX_CUSTOM_PARAMBOLIC_MOVE_PARAM(nMode);
	case enumCustomMissileMove:
		return new GFX_CUSTOM_MISSILE_MOVE_PARAM(nMode);
	case enumCustomMeteoricMove:
		return new GFX_CUSTOM_METEORIC_MOVE_PARAM(nMode);
	default:
		return NULL;
	}
}

void GFX_CUSTOM_LINK_MOVE_PARAM::Load( AFile* pFile, DWORD dwVersion )
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;

	int d = 0;

	if (dwVersion >= 23)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_link_moving, &d);
		m_bMoving = d!=0;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_link_acc, &m_fAcc);
	}
}

void GFX_CUSTOM_LINK_MOVE_PARAM::Save( AFile* pFile )
{
	char	szLine[AFILE_LINEMAXLEN];
	sprintf(szLine, _format_link_moving, m_bMoving);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_link_acc, m_fAcc);
	pFile->WriteLine(szLine);
}

void GFX_CUSTOM_LINK_MOVE_PARAM::Clone( GFX_CUSTOM_MOVE_PARAM* pParam )
{
	if(pParam->GetMoveMode() != m_nMode)
		return;
	GFX_CUSTOM_LINK_MOVE_PARAM* pLinkParam = (GFX_CUSTOM_LINK_MOVE_PARAM*)pParam;

	pLinkParam->m_bMoving = m_bMoving;
	pLinkParam->m_fAcc = m_fAcc;
}

void GFX_CUSTOM_PARAMBOLIC_MOVE_PARAM::Load( AFile* pFile, DWORD dwVersion )
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;
	
	int d = 0;
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_parabolic_init_yaw, &m_fInitYaw);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_parabolic_angle_speed, &m_fAngleSpeed);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_parabolic_fix_amplitude, &m_fFixAmplitude);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_parabolic_proportional_amplitude, &m_fProportionalAmplitude);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_parabolic_average, &d);
	m_bAverage = (d!=0);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_parabolic_half_average, &d);
	m_bHalfAverage = (d!=0);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_parabolic_group, &d);
	m_bGroup = (d!=0);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_parabolic_group_delay, &m_fGroupDelay);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_parabolic_seg, &m_nSeg);
	

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_parabolic_lasting, &d);
	m_bLasting = (d!=0);
}

void GFX_CUSTOM_PARAMBOLIC_MOVE_PARAM::Save( AFile* pFile )
{
	char	szLine[AFILE_LINEMAXLEN];
	sprintf(szLine, _format_parabolic_init_yaw, m_fInitYaw);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_parabolic_angle_speed, m_fAngleSpeed);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_parabolic_fix_amplitude, m_fFixAmplitude);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_parabolic_proportional_amplitude, m_fProportionalAmplitude);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_parabolic_average, m_bAverage);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_parabolic_half_average, m_bHalfAverage);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_parabolic_group, m_bGroup);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_parabolic_group_delay, m_fGroupDelay);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_parabolic_seg, m_nSeg);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_parabolic_lasting, m_bLasting);
	pFile->WriteLine(szLine);
}

void GFX_CUSTOM_PARAMBOLIC_MOVE_PARAM::Clone( GFX_CUSTOM_MOVE_PARAM* pParam )
{
	if(pParam->GetMoveMode() != m_nMode)
		return;
	GFX_CUSTOM_PARAMBOLIC_MOVE_PARAM* pParabolicParam = (GFX_CUSTOM_PARAMBOLIC_MOVE_PARAM*)pParam;
	
	pParabolicParam->m_fInitYaw = m_fInitYaw;
	pParabolicParam->m_fAngleSpeed = m_fAngleSpeed;
	pParabolicParam->m_fFixAmplitude = m_fFixAmplitude;
	pParabolicParam->m_fProportionalAmplitude = m_fProportionalAmplitude;
	pParabolicParam->m_bAverage = m_bAverage;
	pParabolicParam->m_bHalfAverage = m_bHalfAverage;
	pParabolicParam->m_bGroup = m_bGroup;
	pParabolicParam->m_fGroupDelay = m_fGroupDelay;
	pParabolicParam->m_nSeg = m_nSeg;
	pParabolicParam->m_bLasting = m_bLasting;
}

void GFX_CUSTOM_MISSILE_MOVE_PARAM::Load( AFile* pFile, DWORD dwVersion )
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;
	int d = 0;
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_missile_init_yaw, &m_fInitYaw);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_missile_curve_time, &m_nCurveTime);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_missile_curve_amplitude, &m_fCurveAmplitude);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_missile_average, &d);
	m_bAverage = (d!=0);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_missile_half_average, &d);
	m_bHalfAverage = (d!=0);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_missile_curve_stay_time, &m_nCurveStayTime);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_missile_stay_amplitude, &m_fCurveStayAmplitude);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_missile_stay_angle_speed, &m_fAngleSpeedStay);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_missile_stay_self_rotate, &d);
	m_bSelfRotate = (d!=0);
}

void GFX_CUSTOM_MISSILE_MOVE_PARAM::Save( AFile* pFile )
{
	char	szLine[AFILE_LINEMAXLEN];
	sprintf(szLine, _format_missile_init_yaw, m_fInitYaw);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_missile_curve_time, m_nCurveTime);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_missile_curve_amplitude, m_fCurveAmplitude);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_missile_average, m_bAverage);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_missile_half_average, m_bHalfAverage);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_missile_curve_stay_time, m_nCurveStayTime);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_missile_stay_amplitude, m_fCurveStayAmplitude);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_missile_stay_angle_speed, m_fAngleSpeedStay);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_missile_stay_self_rotate, m_bSelfRotate);
	pFile->WriteLine(szLine);
}

void GFX_CUSTOM_MISSILE_MOVE_PARAM::Clone( GFX_CUSTOM_MOVE_PARAM* pParam )
{
	if(pParam->GetMoveMode() != m_nMode)
		return;
	
	GFX_CUSTOM_MISSILE_MOVE_PARAM* pMissileParam = (GFX_CUSTOM_MISSILE_MOVE_PARAM*)pParam;
	pMissileParam->m_fInitYaw = m_fInitYaw;
	pMissileParam->m_nCurveTime = m_nCurveTime;
	pMissileParam->m_fCurveAmplitude = m_fCurveAmplitude;
	pMissileParam->m_bAverage = m_bAverage;
	pMissileParam->m_bHalfAverage = m_bHalfAverage;
	pMissileParam->m_nCurveStayTime = m_nCurveStayTime;
	pMissileParam->m_fCurveStayAmplitude = m_fCurveStayAmplitude;
	pMissileParam->m_fAngleSpeedStay = m_fAngleSpeedStay;
	pMissileParam->m_bSelfRotate = m_bSelfRotate;
}

void GFX_CUSTOM_METEORIC_MOVE_PARAM::Load( AFile* pFile, DWORD dwVersion )
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;
	
	int d = 0;
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_meteoric_fall_height, &m_fFallHeight);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_meteoric_ground_height, &m_fGroundHeight);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_meteoric_radius, &m_fRadius);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_meteoric_fix_radius, &d);
	m_bFixRadius = (d!=0);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_meteoric_x_offset, &m_fXOffset);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_meteoric_z_offset, &m_fZOffset);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_meteoric_random_offset, &d);
	m_bRandomOffset = (d!=0);
	
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_meteoric_innercentric, &d);
	m_bInnerCentric = (d!=0);
	
}

void GFX_CUSTOM_METEORIC_MOVE_PARAM::Save( AFile* pFile )
{
	char	szLine[AFILE_LINEMAXLEN];
	sprintf(szLine, _format_meteoric_fall_height, m_fFallHeight);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_meteoric_ground_height, m_fGroundHeight);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_meteoric_radius, m_fRadius);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_meteoric_fix_radius, m_bFixRadius);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_meteoric_x_offset, m_fXOffset);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_meteoric_z_offset, m_fZOffset);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_meteoric_random_offset, m_bRandomOffset);
	pFile->WriteLine(szLine);
	sprintf(szLine, _format_meteoric_innercentric, m_bInnerCentric);
	pFile->WriteLine(szLine);
}

void GFX_CUSTOM_METEORIC_MOVE_PARAM::Clone( GFX_CUSTOM_MOVE_PARAM* pParam )
{
	if(pParam->GetMoveMode() != m_nMode)
		return;
	
	GFX_CUSTOM_METEORIC_MOVE_PARAM* pMeteoricParam = (GFX_CUSTOM_METEORIC_MOVE_PARAM*)pParam;
	pMeteoricParam->m_fFallHeight = m_fFallHeight;
	pMeteoricParam->m_fGroundHeight = m_fGroundHeight;
	pMeteoricParam->m_fRadius = m_fRadius;
	pMeteoricParam->m_bFixRadius = m_bFixRadius;
	pMeteoricParam->m_fXOffset = m_fXOffset;
	pMeteoricParam->m_fZOffset = m_fZOffset;
	pMeteoricParam->m_bRandomOffset = m_bRandomOffset;
	pMeteoricParam->m_bInnerCentric = m_bInnerCentric;
}


void CGfxCustomParabolicMove::StartMove( const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget )
{
	GFX_CUSTOM_PARAMBOLIC_MOVE_PARAM* pParam = (GFX_CUSTOM_PARAMBOLIC_MOVE_PARAM*)m_pMoveParam;
	
	m_fAngleSpeed = -pParam->m_fAngleSpeed * A3D_2PI * 0.001f; 
	
	m_fFixAmplitude = fabs(pParam->m_fFixAmplitude);
	m_fAmplitude = fabs(pParam->m_fProportionalAmplitude);
	m_bAverage = pParam->m_bAverage;
	m_bGroup =	pParam->m_bGroup;
	m_fGroupDelay = fabs(pParam->m_fGroupDelay);
	m_nSeg = abs(pParam->m_nSeg);
	if (m_nSeg == 0)
		m_nSeg = 1;
	m_bLasting = pParam->m_bLasting;
	
	if (m_bOneOfCluser )
	{
		if (!m_bGroup)
		{
			if( m_pEvent->GetDelay() ==0 && m_nIndex != 0 )					//有多个的情况，如果间隔时间为0，则只算一次击中, 如果不为0则算多次击中
			{
				m_pEvent->SetFlyHitMode(enumAttFlyOnly);				//后面的只飞行不击中 
			}
		}
		else
		{
			if (m_nIndex % 2 != 0)					//每两个算一次击中
			{
				m_pEvent->SetFlyHitMode(enumAttFlyOnly);
			}
			
			m_pEvent->SetDelay(static_cast<DWORD>(m_fGroupDelay * (m_nIndex * 0.5f)));
		}	
	}
	
	if (m_bArea)
	{
		CalcRange(Normalize(vTarget - vHost));
		m_vOrgPos = m_vPos = vHost + GetRandOff();
	}
	else
		m_vOrgPos = m_vPos = vHost;
	
	m_vHorzPos = m_vOrgPos;
	m_vMoveDir = m_vHorzDir = Normalize(vTarget - m_vPos);
	m_fSpeed = m_fFlySpeed;
	
	if (!m_bOneOfCluser ||
		(m_bOneOfCluser && !m_bAverage))
		m_vParabolicDir = A3DQUATERNION(m_vHorzDir, DEG2RAD(-pParam->m_fInitYaw)) * _unit_y;
	else
	{
		if (!m_bGroup)
		{
			int nAdd;
			if (m_nCount == 1)
				nAdd = 0;
			else
				nAdd = pParam->m_bHalfAverage ? 180 * m_nIndex / (m_nCount-1) : 360 * m_nIndex / m_nCount;
			
			m_vParabolicDir = A3DQUATERNION(m_vHorzDir, DEG2RAD(-(pParam->m_fInitYaw + nAdd))) * _unit_y;
		}
		else
		{
			m_vParabolicDir = A3DQUATERNION(m_vHorzDir, DEG2RAD(-(pParam->m_fInitYaw + 360 * (m_nIndex%2)/2))) * _unit_y;
		}
	}

	m_bIsMoving = true;
}

bool CGfxCustomParabolicMove::TickMove( DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos )
{
	if (!m_bIsMoving)		// 目标被击中
		return true;

	m_vHorzDir = vTargetPos - m_vHorzPos;
	float fDist = m_vHorzDir.Normalize();		//剩余长度
	float fFlyDist = m_fSpeed * dwDeltaTime;
	
	if (fFlyDist >= fDist) 
	{
		if (m_bLasting)
		{
			if (m_pEvent && m_pEvent->GetFlyGfx())
				m_pEvent->GetFlyGfx()->Start(true);			//重置gfx
			
			m_vPos = m_vHorzPos = vHostPos;
			return false;
		}
		else
		{
			fFlyDist = fDist;
			m_bIsMoving = false;
		}
	}
	
	m_vHorzPos += m_vHorzDir * fFlyDist;
	
	float fTotalDist = (vTargetPos - m_vOrgPos).Normalize()/m_nSeg;
	float distance =  m_fFixAmplitude == 0.0f ? fTotalDist * m_fAmplitude : m_fFixAmplitude;
	float t = fDist * A3D_PI / fTotalDist;
	float fYOffset = distance * fabs(sin(t));		//根据距离按正弦曲线
	
	if (m_fAngleSpeed != 0.0f)
		m_vParabolicDir = A3DQUATERNION(m_vHorzDir, m_fAngleSpeed * dwDeltaTime) * m_vParabolicDir;
	
	m_vPos = m_vHorzPos + m_vParabolicDir * fYOffset;
	
	
	return false;
}

void CGfxCustomMissileMove::StartMove( const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget )
{
	GFX_CUSTOM_MISSILE_MOVE_PARAM* pParam = (GFX_CUSTOM_MISSILE_MOVE_PARAM*)m_pMoveParam;
	m_bAverage = pParam->m_bAverage;
	m_dwCurveTime = abs(pParam->m_nCurveTime);
	m_fCurveAmplitude = pParam->m_fCurveAmplitude;
	m_dwCurveStayTime = abs(pParam->m_nCurveStayTime);
	m_fCurveStayAmplitude = fabs(pParam->m_fCurveStayAmplitude);
	m_fAngleSpeedStay = pParam->m_fAngleSpeedStay;
	m_bSelfRotate = pParam->m_bSelfRotate;
	
	if (m_bOneOfCluser )	//有多个的情况，如果间隔时间为0，则只算一次击中, 如果不为0则算多次击中
	{	
		if( m_pEvent->GetDelay() ==0 && m_nIndex != 0)
		{
			m_pEvent->SetFlyHitMode(enumAttFlyOnly);			//后面的只飞行不击中 
		}
	}
	
	if (m_bArea)
	{
		CalcRange(Normalize(vTarget - vHost));
		m_vOrgPos = m_vPos = vHost + GetRandOff();
	}
	else
		m_vOrgPos = m_vPos = vHost;
	
	m_vMoveDir = m_vHorzDir = Normalize(vTarget - m_vPos);
	m_dwTotalTime = 0;
	
	if (!m_bOneOfCluser ||
		(m_bOneOfCluser && !m_bAverage))
		m_vMissileDir = A3DQUATERNION(m_vHorzDir, DEG2RAD(-pParam->m_fInitYaw)) * _unit_y;
	else
	{
		int nAdd;
		if (m_nCount == 1)
			nAdd = 0;
		else
			nAdd = pParam->m_bHalfAverage ? 180 * m_nIndex / (m_nCount-1) : 360 * m_nIndex / m_nCount;
		m_vMissileDir = A3DQUATERNION(m_vHorzDir, DEG2RAD(-(pParam->m_fInitYaw + nAdd))) * _unit_y;
	}
	m_nAdjustCount = 0;
	m_bIsMoving = true;
}

bool CGfxCustomMissileMove::TickMove( DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos )
{
	if (!m_bIsMoving)		// 目标被击中
		return true;

	m_vHorzDir = Normalize(vTargetPos - m_vOrgPos);
	
	if (m_dwTotalTime <= m_dwCurveTime)				//第一阶段，弧线
	{
		
		float f = (float)(m_dwTotalTime+dwDeltaTime) / (float)m_dwCurveTime;
		
		m_vPos = m_vOrgPos - m_vHorzDir * m_fCurveAmplitude * (float)sin(A3D_PI * f) + f * m_vMissileDir * m_fCurveStayAmplitude;
		
// 		if(m_dwTotalTime+dwDeltaTime > m_dwCurveTime)
// 			m_vMoveDir = m_vHorzDir;
// 		else
			m_vMoveDir = - m_vHorzDir * m_fCurveAmplitude * (float)cos(A3D_PI * f) * A3D_PI + m_vMissileDir * m_fCurveStayAmplitude;
		m_vMoveDir.Normalize();
	}
	else	
		if (m_dwTotalTime <= m_dwCurveTime+m_dwCurveStayTime)		//第二阶段,停留
		{
			float angle = -A3D_2PI * m_fAngleSpeedStay * (m_dwTotalTime - m_dwCurveTime)/m_dwCurveStayTime;
			if (!m_bSelfRotate && m_nIndex%2==0)
				angle = -angle;
			
			if (m_dwTotalTime+dwDeltaTime > m_dwCurveTime+m_dwCurveStayTime)
			{
				m_vPos = m_vOrgPos + m_vMissileDir * m_fCurveStayAmplitude;
			}
			else
			{
				A3DVECTOR3 vNewMissileDir;
				if (m_bSelfRotate)
				{
					A3DVECTOR3 vUp = CrossProduct(m_vHorzDir, m_vMissileDir);
					vNewMissileDir = A3DQUATERNION(vUp, angle) * m_vMissileDir;
				}
				else
				{
					vNewMissileDir = A3DQUATERNION(m_vHorzDir, angle) * m_vMissileDir;
				}
				
				m_vPos = m_vOrgPos + vNewMissileDir * m_fCurveStayAmplitude;
			}
			m_vMoveDir = m_vHorzDir;
		}
		else		//第三阶段，直线
		{
			float fDist = (vTargetPos-m_vPos).Normalize();		//剩余长度
			A3DVECTOR3 vDir = Normalize(vTargetPos-m_vPos);
			
			float fFlyDist = m_fFlySpeed * dwDeltaTime;
			
			float fAngle = DotProduct(m_vMoveDir, vDir);

			if (fFlyDist * fAngle >= fDist) 
			{
				fFlyDist = fDist;
				m_bIsMoving = false;
			}
				
			if (m_nAdjustCount >= 10)
			{
				m_vMoveDir = vDir;
			}
			else
			{
				++m_nAdjustCount;

				float f = m_nAdjustCount * 0.1f;
				m_vMoveDir = Normalize(vDir*f + m_vMoveDir*(1-f));
			}
			m_vPos += m_vMoveDir * fFlyDist * fAngle;
		}
		
		m_dwTotalTime += dwDeltaTime;
		
	return false;
}

void CGfxCustomMeteoricMove::StartMove( const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget )
{
	GFX_CUSTOM_METEORIC_MOVE_PARAM* pParam = (GFX_CUSTOM_METEORIC_MOVE_PARAM*)m_pMoveParam;
	m_fFallHeight = fabs(pParam->m_fFallHeight);
	if( m_fFallHeight < 2.0f ) m_fFallHeight = 2.0f;
	m_fGroundHeight = pParam->m_fGroundHeight;
	m_fRadius = pParam->m_fRadius;
	m_bFixRadius = pParam->m_bFixRadius;
	m_fXOffset = pParam->m_fXOffset;
	m_fZOffset = pParam->m_fZOffset;
	m_bRandomOffset = pParam->m_bRandomOffset;
	m_bInnerCentric = pParam->m_bInnerCentric;
	
	m_fGoundY = 0.0f;
	
	if (m_pEvent && m_pEvent->GetFlyGfx())
	{
		A3DVECTOR3 vGroudPos = vTarget;
		vGroudPos.y = 0;
		m_fGoundY = AfxGetGrndNorm(vGroudPos, NULL);
	}
	
	m_fGoundY += m_fGroundHeight;
	
	//
	m_vPos = vTarget;
	m_vPos.y += m_fFallHeight;
	
	A3DVECTOR3 vDirH = vTarget - vHost;
	vDirH.y = 0;
	vDirH.Normalize();
	A3DVECTOR3 vRight = CrossProduct(_unit_y, vDirH);
	vRight.Normalize();
	
	A3DVECTOR3 vDest;	//目标落点
	
	if (m_bOneOfCluser)
	{
		if (m_bFixRadius)
		{
			A3DVECTOR3 vDir = vDirH;
			vDir = A3DQUATERNION( _unit_y, A3D_2PI * m_nIndex / m_nCount ) * vDir;
			m_vPos += vDir * m_fRadius;
			
			vDest = m_vPos;
			vDest.y = m_fGoundY;
			
			if (m_bInnerCentric)
			{
				m_vPos += vDir * m_fRadius;
			}
		}
		else
		{
			float fRandAng = _UnitRandom() * A3D_2PI;
			float fRadius = _UnitRandom() * m_fRadius;
			m_vPos.x += (float)cos(fRandAng) * fRadius;
			m_vPos.z += (float)sin(fRandAng) * fRadius;
			
			vDest = m_vPos;
			vDest.y = m_fGoundY;
			
			if (m_bInnerCentric)
			{
				m_vPos.x += (float)cos(fRandAng) * fRadius;
				m_vPos.z += (float)sin(fRandAng) * fRadius;
			}
		}
	}
	else
	{
		vDest = m_vPos;
		vDest.y = m_fGoundY;
	}
	
	if (m_bRandomOffset)
	{
		m_vPos += a3d_RandDirH() * _UnitRandom() * max(m_fXOffset, m_fZOffset);
	}
	else
	{
		m_vPos += vDirH * m_fZOffset;
		m_vPos += vRight * m_fXOffset;
	}
	
	m_vMoveDir = Normalize(vDest - m_vPos);
	m_vFallVel = m_fFlySpeed * m_vMoveDir;

	m_bIsMoving = true;
}

bool CGfxCustomMeteoricMove::TickMove( DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos )
{
	if (!m_bIsMoving)		// 目标被击中
		return true;

	m_fOldY = m_vPos.y;
	
	m_vPos += m_vFallVel * (float)dwDeltaTime;
	
	bool bHit = m_vPos.y <= m_fGoundY;
	if (bHit)
	{
		m_vPos.y = m_fGoundY;
		if (m_fOldY > m_fGoundY)
			return false;
		else
			m_bIsMoving = false;
	}
	return false;
}



} // namespace

#ifdef _SKILLGFXCOMPOSER

#include "A3DSkeleton.h"

A3DVECTOR3 _host_pos(0, 0, 0);
A3DVECTOR3 _target_pos(0, 0, 10.0f);

static A3DSkinModel* _host_model = NULL;
static A3DSkinModel* _target_model = NULL;

namespace _SGC
{
	class A3DSkillGfxMan;
}


typedef _SGC::A3DSkillGfxMan* (*HookGetSkillGfxMan)();
static HookGetSkillGfxMan s_pHookGetSkillGfxMan = NULL;

void SGCSetHostAndTargetModel(A3DSkinModel* pHost, A3DSkinModel* pTarget)
{
	_host_model = pHost;
	_target_model = pTarget;
}

void SetHookGetSkillGfxMan(HookGetSkillGfxMan pHookGetSkillGfxMan)
{
	s_pHookGetSkillGfxMan = pHookGetSkillGfxMan;
}

namespace _SGC
{


class CTempMan : public A3DSkillGfxMan
{
public:
	CTempMan() {}
	~CTempMan() {}

protected:

	virtual A3DSkillGfxEvent* CreateOneEvent(GfxMoveMode mode)
	{
		return new A3DSkillGfxEvent(this, mode);
	}

	bool GetPosition(A3DSkinModel* pModel, clientid_t nId, A3DVECTOR3& vPos, GfxHitPos HitPos, const char* szHook, bool bRelHook, const A3DVECTOR3* pOffset, const char* szHanger, bool bChildHook)
	{
		if (pModel == NULL)
			return false;

		if (HitPos == enumHitCenter)
		{
			vPos = pModel->GetPos() + A3DVECTOR3(0,pModel->GetModelAABB().Extents.y,0);
		}
		else
		{
			vPos = pModel->GetPos();
		}

		if (szHook && strlen(szHook))
		{
			A3DSkeletonHook* pHook = NULL;

			if (bChildHook && szHanger)
			{
				A3DSkinModelHanger* pHanger = pModel->GetSkinModelHanger(szHanger);

				if (pHanger)
				{
					A3DSkinModel* pChild = pHanger->GetModelObject();

					if (pChild)
						pHook = pChild->GetSkeletonHook(szHook, true);
				}
			}
			else
				pHook = pModel->GetSkeletonHook(szHook, true);

			if (pHook)
			{
				if (bRelHook)
					vPos = pHook->GetAbsoluteTM() * (*pOffset);
				else
				{
					vPos = pModel->GetAbsoluteTM() * (*pOffset);
					vPos = vPos - pModel->GetAbsoluteTM().GetRow(3) + pHook->GetAbsoluteTM().GetRow(3);
				}
			}		
		}
		else if (pOffset)
			vPos += *pOffset;	

		return true;
	}

	virtual bool GetPositionById(clientid_t nId, A3DVECTOR3& vPos, GfxHitPos HitPos, const char* szHook, bool bRelHook, const A3DVECTOR3* pOffset, const char* szHanger, bool bChildHook)
	{
		if (nId == 1)
		{
			if (!GetPosition(_host_model, nId, vPos, HitPos, szHook, bRelHook, pOffset, szHanger, bChildHook))
				vPos = _host_pos;
			else
				return true;
		}
		else
		{
			if (!GetPosition(_target_model, nId, vPos, HitPos, szHook, bRelHook, pOffset, szHanger, bChildHook))
				vPos = _target_pos;
			else
				return true;
		}

		if (HitPos != enumHitBottom)
			vPos.y += .9f;

		if (pOffset)
			vPos += *pOffset;

		return true;
	}

	virtual bool GetDirAndUpById(clientid_t nId, A3DVECTOR3& vDir, A3DVECTOR3& vUp)
	{
		vDir.Set(0, 0, 1.0f);
		vUp.Set(0, 1.0f, 0);
		return true;
	}

public:

	float GetTargetScale(clientid_t nTargetId) { return 1.8f; }
};

CTempMan _event_man;

A3DSkillGfxMan* AfxGetSkillGfxEventMan()
{
	if (s_pHookGetSkillGfxMan)
		return (s_pHookGetSkillGfxMan)();

	return &_event_man;
}

}

#endif

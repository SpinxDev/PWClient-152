#include "A3DGfxExMan.h"
#include "A3DSkillGfxEvent2.h"
#include "A3DSkillGfxComposer2.h"
#include "A3DGFXKeyPoint.h"

static const float _hit_radius		= .1f;
static const float _fly_speed		= 20.0f / 1000.0f;
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

#define HIT_GFX_MAX_TIMESPAN 5000

void CGfxLinearMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	if (m_bArea)
	{
		CalcRange(Normalize(vTarget - vHost));
		m_vPos = vHost + GetRandOff();
	}
	else
		m_vPos = vHost;

	m_vMoveDir = vTarget - m_vPos;
	float fDist = m_vMoveDir.Normalize();
	float fMax = _fly_speed * m_dwMaxFlyTime;

	if (fMax >= fDist)
		m_fSpeed = _fly_speed;
	else
		m_fSpeed = fDist / m_dwMaxFlyTime;
}

bool CGfxLinearMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	A3DVECTOR3 vFlyDir = vTargetPos - m_vPos;
	float fDist = vFlyDir.Normalize();
	float fFlyDist = m_fSpeed * dwDeltaTime;

	if (fFlyDist >= fDist) return true; // 目标被击中
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
	float fMax = _fly_speed * m_dwMaxFlyTime;
	
	if (fMax >= fDist)
		m_fSpeed = _fly_speed;
	else
		m_fSpeed = fDist / m_dwMaxFlyTime;

	m_fTotalTime = fDist / m_fSpeed;
	m_vHorzVel *= m_fSpeed;
	m_fVertVel = fVert / m_fTotalTime + .5f * _gravity * m_fTotalTime;
	m_vMoveDir = Normalize(m_vHorzVel + m_fVertVel * _unit_y);
	m_fCurTime = 0;
}

bool CGfxParabolicMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	m_fCurTime += dwDeltaTime;
	if (m_fCurTime >= m_fTotalTime) return true;

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
}

bool CGfxMissileMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	static const float fLimit = (float)cos(DEG2RAD(15.0f));

	float fTime = dwDeltaTime / 1000.0f;
	A3DVECTOR3 vDir = vTargetPos - m_vPos;
	float fDist = m_fCurVel * fTime;

	if (fDist >= vDir.Normalize())
		return true;

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
}

bool CGfxMeteoricMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	m_vPos += m_vFallVel * (dwDeltaTime / 1000.f);
	return m_vPos.y <= vTargetPos.y;
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
	float fMax = _fly_speed * m_dwMaxFlyTime;

	if (fMax >= fDist)
		m_fSpeed = _fly_speed;
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
}

bool CGfxHelixMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	float fDelta = static_cast<float>(dwDeltaTime);
	m_vCenter += m_vMoveDir * (m_fSpeed * fDelta);
	m_fRadius -= m_fShrinkRate * fDelta;
	m_fAngle -= _ang_vel * fDelta;
	A3DVECTOR3 vOff = m_vAxisX * (float)(cos(m_fAngle) * m_fRadius)
					+ m_vAxisY * (float)(sin(m_fAngle) * m_fRadius);
	m_vPos = m_vCenter + vOff;
	m_fFlyTime -= fDelta;
	return m_fFlyTime <= 0;
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
	float fMax = m_dwMaxFlyTime * _fly_speed;

	if (fMax >= fDist)
		m_fSpeed = _fly_speed;
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
}

bool CGfxCurvedMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	float fDelta = static_cast<float>(dwDeltaTime);
	float fVEnd = m_fLatSpd - _curved_acc * fDelta;
	float fOff = .5f * (m_fLatSpd + fVEnd) * fDelta;
	m_fLatSpd = fVEnd;
	m_vPos += fDelta * m_vZSpd + fOff * m_vAxisX;
	m_vMoveDir = m_vZSpd + m_fLatSpd *m_vAxisX;
	m_vMoveDir.Normalize();
	m_fTime -= fDelta;
	return m_fTime <= 0;
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
}

bool CGfxAccMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	float fDelta = dwDeltaTime / 1000.0f;
	A3DVECTOR3 vFlyDir = vTargetPos - m_vPos;
	float fDist = vFlyDir.Normalize();
	float fVEnd = m_fSpeed + m_fAcc * fDelta;
	float fFlyDist = (m_fSpeed + fVEnd) * fDelta * .5f;
	m_fSpeed = fVEnd;

	if (fFlyDist >= fDist) return true; // 目标被击中
	m_vPos += vFlyDir * fFlyDist;
	m_vMoveDir = vFlyDir;
	return false;
}

void CGfxOnTargetMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	m_vPos = vTarget;
	m_vMoveDir = vTarget - vHost;
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

void CGfxLinkMove::StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
	for (size_t i = 0; i < m_Params.size(); i++)
		delete m_Params[i];
	m_Params.clear();

	m_vPos = vHost;
	m_vMoveDir = Normalize(vTarget - vHost);
}

bool CGfxLinkMove::TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos)
{
	return false;
}

void CGfxLinkMove::UpdateGfxParam(A3DGFXEx* pGfx, const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget)
{
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
			pGfx->UpdateEleParam(pInfo->m_strEleName, ID_PARAM_LTN_POS2, vTarget);
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


A3DSkillGfxEvent::A3DSkillGfxEvent(GfxMoveMode mode) :
	m_pComposer(NULL),
	m_pFlyGfx(0),
	m_pHitGfx(0),
	m_nHostID(0),
	m_nTargetID(0),
	m_dwModifier(0),
	m_dwFlyTimeSpan(0),
	m_dwCurSpan(0),
	m_enumState(enumWait),
	m_bHitGfxInfinite(false),
	m_bIsGoblinSkill(false),
	m_bTargetDirAndUpExist(false),
	m_bGfxUseLod(true),
	m_bGfxDisableCamShake(false),
	m_bHostECMCreatedByGfx(false)
{
	m_pMoveMethod = CGfxMoveBase::CreateMoveMethod(mode);
}

A3DSkillGfxEvent::~A3DSkillGfxEvent()
{
	ReleaseGfx();
	delete m_pMoveMethod;
}

void A3DSkillGfxEvent::Tick(DWORD dwDeltaTime)
{
	m_dwCurSpan += dwDeltaTime;

	if (m_enumState == enumFinished) return; //结束
	else if (m_enumState == enumHit) // 击中
	{
		if (!m_pHitGfx || m_pHitGfx->GetState() == ST_STOP)
			m_enumState = enumFinished;
		else
		{
			if (!m_bTargetExist || m_bHitGfxInfinite && m_pHitGfx->GetTimeElapse() > HIT_GFX_MAX_TIMESPAN)
				m_enumState = enumFinished;
			else 
			{
				if (m_bTraceTarget)
				{
					A3DMATRIX4 matTran;
					matTran.Identity();
					matTran.SetRow(3, GetTargetCenter());
					m_pHitGfx->SetParentTM(matTran);
				}

				m_pHitGfx->TickAnimation(dwDeltaTime);
			}
		}
	}
	else if (m_dwCurSpan > m_dwFlyTimeSpan) //飞行超时
	{
		if (!m_bTargetExist)
			m_enumState = enumFinished;
		else
			HitTarget(GetTargetCenter());
	}
	else if (!m_bTargetExist)
		m_enumState = enumFinished;
	else if (m_enumState == enumWait)
	{
		if (m_dwCurSpan < m_dwDelayTime) return;

		if (!m_bHostExist)
			m_enumState = enumFinished;
		else
		{
			m_enumState = enumFlying;
			m_pMoveMethod->SetMaxFlyTime(m_dwFlyTimeSpan);
			m_pMoveMethod->StartMove(m_vHostPos, m_vTargetPos);

			if (m_pFlyGfx)
			{
				A3DVECTOR3 vDir, vUp;

				if (m_pMoveMethod->GetMode() == enumOnTarget && m_pMoveMethod->IsReverse() && GetTargetDirAndUp(vDir, vUp))
					m_pFlyGfx->SetParentTM(a3d_TransformMatrix(vDir, vUp, m_pMoveMethod->GetPos()));
				else
					m_pFlyGfx->SetParentTM(_build_matrix(m_pMoveMethod->GetMoveDir(), m_pMoveMethod->GetPos()));

				m_pFlyGfx->Start(true);
				m_pMoveMethod->UpdateGfxParam(m_pFlyGfx, m_vHostPos, m_vTargetPos);
				m_pFlyGfx->TickAnimation(0);
			}
		}
	}
	else
	{
		if (m_pMoveMethod->TickMove(dwDeltaTime, m_vHostPos, m_vTargetPos)) // 目标被击中
			HitTarget(GetTargetCenter());
		else if (m_pFlyGfx)
		{

			A3DVECTOR3 vDir, vUp;

			if (m_pMoveMethod->GetMode() == enumOnTarget && m_pMoveMethod->IsReverse() && GetTargetDirAndUp(vDir, vUp))
				m_pFlyGfx->SetParentTM(a3d_TransformMatrix(vDir, vUp, m_pMoveMethod->GetPos()));
			else
				m_pFlyGfx->SetParentTM(_build_matrix(m_pMoveMethod->GetMoveDir(), m_pMoveMethod->GetPos()));
			
			m_pMoveMethod->UpdateGfxParam(m_pFlyGfx, m_vHostPos, m_vTargetPos);
			m_pFlyGfx->TickAnimation(dwDeltaTime);
		}
	}
}

void A3DSkillGfxEvent::HitTarget(const A3DVECTOR3& vTarget)
{
	m_enumState = enumHit;
	ReleaseFlyGfx();

	if (m_pHitGfx)
	{
		m_bHitGfxInfinite = m_pHitGfx->IsInfinite(); 
		A3DMATRIX4 matTran;

		// now try to make the hit gfx face to the attacker
		if (m_bHostExist)
		{
			A3DVECTOR3 vDir = vTarget - m_vHostPos;
			vDir.y = 0;
			
			if (vDir.Normalize() < 1e-3)
				vDir = A3DVECTOR3(0, 0, 1.0f);
			
			matTran = _build_matrix(vDir, vTarget);
		}
		else
		{
			matTran = IdentityMatrix();
			matTran.SetRow(3, vTarget);
		}

		m_pHitGfx->SetParentTM(matTran);
		m_pHitGfx->Start(true);
		m_pHitGfx->TickAnimation(0);
	}
}

bool A3DSkillGfxMan::AddOneSkillGfxEvent(
		A3DSkillGfxComposer* pComposer,
		clientid_t nHostID,
		clientid_t nTargetID,
		const char* szFlyGfx,
		GfxMoveMode mode,
		DWORD dwDelayTime,
		DWORD dwFlyTimeSpan,
		const char* szHitGfx,
		const GFX_SKILL_PARAM* param,
		bool bTraceTarget,
		float fFlyGfxScale,
		float fHitGfxScale,
		DWORD dwModifier,
		bool bCluster,
		bool bFadeOut,
		bool bIsGoblinSkill,
		bool bReverse)
{
	assert(nHostID);
	assert(nTargetID);

	A3DSkillGfxEvent* pEvent = GetEmptyEvent(mode);
	pEvent->SetComposer(pComposer);
	pEvent->SetHostID(nHostID);
	pEvent->SetTargetID(nTargetID);
	pEvent->SetFlyTimeSpan(dwFlyTimeSpan);
	pEvent->SetDelay(dwDelayTime);
	pEvent->SetReverse(bReverse);
	if (param) pEvent->SetParam(param);
	pEvent->SetTraceTarget(bTraceTarget);
	pEvent->SetModifier(dwModifier);
	pEvent->SetIsCluster(bCluster);
	pEvent->SetFadeOut(bFadeOut);
	pEvent->SetGoblinSkill(bIsGoblinSkill);

	ECMODEL_GFX_PROPERTY Prop;
	if (GetPropertyById(nHostID, &Prop))
	{
		pEvent->SetGfxUseLod(Prop.bGfxUseLod);
		pEvent->SetDisableCamShake(Prop.bGfxDisableCamShake);
		pEvent->SetHostModelCreatedByGfx(Prop.bHostECMCreatedByGfx);
	}

	if (szFlyGfx)
	{
		A3DGFXEx* pGfx = pEvent->LoadFlyGfx(m_pDevice, szFlyGfx);
		if (pGfx)
		{
			pGfx->SetScale(fFlyGfxScale);
			pGfx->SetDisableCamShake(pEvent->GetDisableCamShake());
			pGfx->SetCreatedByGFXECM(pEvent->GetHostModelCreatedByGfx());
			pGfx->SetUseLOD(pEvent->GetGfxUseLod());
			pGfx->SetId(pEvent->GetHostID());
			pEvent->SetFlyGfx(pGfx);
		}
	}

	if (szHitGfx)
	{
		A3DGFXEx* pGfx = pEvent->LoadHitGfx(m_pDevice, szHitGfx);
		if (pGfx)
		{
			pGfx->SetScale(fHitGfxScale);
			pEvent->SetHitGfx(pGfx);
		}
	}

#ifndef _SKILLGFXCOMPOSER
	pEvent->Tick(0);
#endif
	PushEvent(pEvent);
	return true;
}

bool A3DSkillGfxMan::AddSkillGfxEvent(
		A3DSkillGfxComposer* pComposer,
		clientid_t nHostID,
		clientid_t nTargetID,
		const char* szFlyGfx,
		const char* szHitGfx,
		DWORD dwFlyTimeSpan,
		bool bTraceTarget,
		GfxMoveMode FlyMode,
		int nFlyGfxCount,
		DWORD dwInterval,
		const GFX_SKILL_PARAM* param,
		float fFlyGfxScale,
		float fHitGfxScale,
		DWORD dwModifier,
		bool bOnlyOneHit,
		bool bFadeOut,
		bool bIsGoblinSkill,
		bool bReverse
	)
{
	bool bRet = true, bCluster;
	DWORD dwDelayTime;

	if (nFlyGfxCount == 1)
	{
		dwDelayTime = dwInterval;
		bCluster = false;
	}
	else
	{
		dwDelayTime = 0;
		bCluster = true;
	}

	for (int i = 0; i < nFlyGfxCount; i++)
	{
		if (!AddOneSkillGfxEvent(
			pComposer,
			nHostID,
			nTargetID,
			szFlyGfx,
			FlyMode,
			dwDelayTime,
			dwFlyTimeSpan,
			bOnlyOneHit ? (i == nFlyGfxCount -1 ? szHitGfx : NULL) : szHitGfx,
			param,
			bTraceTarget,
			fFlyGfxScale,
			fHitGfxScale,
			dwModifier,
			bCluster,
			bFadeOut,
			bIsGoblinSkill,
			bReverse
			))
			bRet = false;

		dwDelayTime += dwInterval;
	}

	return bRet;
}

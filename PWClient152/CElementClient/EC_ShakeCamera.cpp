
////////////////////////////////
#include "EC_ShakeCamera.h"

#include "A3DGFXKeyPoint.h"
#include "A3DGFXShakeCamera.h"
#include "A3DGFXExMan.h"

static A3DVECTOR3 GenerateDirection(SHAKECAM sc, A3DCamera* pCamera)
{
	A3DVECTOR3 vDir;
	switch (sc)
	{
	case SHAKECAM_DIR_RAND:
		{
			A3DQUATERNION q(_unit_y, _UnitRandom() * A3D_2PI);
			vDir = RotateVec(q, _unit_x);
			q.ConvertFromAxisAngle(vDir, _UnitRandom() * A3D_2PI);
			vDir = RotateVec(q, _unit_y);
			vDir.Normalize();
		}
		break;
	case SHAKECAM_DIR_UPDOWN:
		vDir = _unit_y;
		break;
	case SHAKECAM_DIR_CAMERA:
		vDir = pCamera->GetDir();
		break;
	default:
		vDir = _unit_zero;
		break;
	}
	return vDir;
}

CECShakeCameraBase::CECShakeCameraBase(DWORD dwPeriod, bool bShakebyDistance):
m_bShaking(false)
, m_vShakeOffset(0)
, m_dwPeriod(dwPeriod)
, m_bShakeByDistance(bShakebyDistance)
, m_shakeType(SHAKECAM_DIR_UPDOWN)
, m_dwTimeSpan(0)
{

}

CECShakeCameraBase* CECShakeCameraBase::CreateShakeCam(DWORD dwPeriod, bool bShakeByDistance)
{
	if (dwPeriod == 0)
		return new CECShakeCamera(dwPeriod, bShakeByDistance);
	else
		return new CECShakeCamera2(dwPeriod, bShakeByDistance);
}

bool CECShakeCameraBase::Tick(DWORD dwTickTime, A3DCamera* pCamera, float fDistToCam)
{
	if(!GetShaking()) return false;

	m_dwTimeSpan += dwTickTime;

	return true;
}
//////////////////////////////////////////////////////////////////////////
//
//	Implement ShakeCamera
//
//////////////////////////////////////////////////////////////////////////

static const float _shake_attenu_dist = 30.0f;

inline static float 
CalcDistCo(float fDistToCam)
{
	return 1.0f - fDistToCam / _shake_attenu_dist;
}

CECShakeCamera::CECShakeCamera(DWORD dwPeriod, bool bShakebyDistance)
: CECShakeCameraBase(dwPeriod, bShakebyDistance)
, m_nShakeCount(0)
{
	m_pShakeNoise = new A3DGFXCtrlNoiseBase();
	m_pShakeNoise->Resume();
	m_pAffector = ShakeAffector::CreateAffector(ShakeAffector::SA_DAMPING_LINEAR,m_pShakeNoise);
}

CECShakeCamera::~CECShakeCamera()
{
	delete m_pShakeNoise;
	delete m_pAffector;
}
void CECShakeCamera::Start()
{
	m_bShaking = true;

	m_nShakeCount = 0;
	AfxBeginShakeCam();
}
bool CECShakeCamera::Tick(DWORD dwTickTime, A3DCamera* pCamera, float fDistToCam)
{
	if(!CECShakeCameraBase::Tick(dwTickTime,pCamera,fDistToCam))
		return false;

	ASSERT( GetShakePeriod() == 0 );
	const float fTimeSpan = GetTimeSpan();
	A3DGFXCtrlNoiseBase* pShakeNoise = m_pShakeNoise;
	ASSERT( pShakeNoise );
	
	if (!pShakeNoise
		|| (!pShakeNoise->Tick(fTimeSpan)) )
	{
		Stop();
		return true;
	}
	
	if ((m_nShakeCount++ % 2) == 0)
	{
		if (fDistToCam >= _shake_attenu_dist)
			m_vShakeOffset.Clear();
		else
		{
			const A3DVECTOR3 v = GenerateDirection(static_cast<SHAKECAM>(m_shakeType), pCamera);
			const float fCo = GetShakeByDistance() ? CalcDistCo(fDistToCam) : 1.0f;
			m_vShakeOffset = (pShakeNoise->GetNoiseVal(m_dwTimeSpan) * fCo) * v;
		}
		
		ShakeAffector* pAffector = m_pAffector;
		if (pAffector)
			pAffector->AffectShakeOffset(fTimeSpan, m_vShakeOffset);
	}
	else
		m_vShakeOffset = -m_vShakeOffset;
	
	AfxSetCamOffset(m_vShakeOffset);

	return true;
}

A3DVECTOR3 CECShakeCamera::GetShakeOffset() const
{
	return m_vShakeOffset;
}

void CECShakeCamera::Stop()
{
	ASSERT( GetShakePeriod() == 0 );
	if (!GetShaking()) return;
	if ((m_nShakeCount % 2) != 0) AfxSetCamOffset(-m_vShakeOffset);
	AfxEndShakeCam();
	SetShaking(false);
}


//////////////////////////////////////////////////////////////////////////
//
//	Implement ShakeCamera2
//
//////////////////////////////////////////////////////////////////////////

CECShakeCamera2::CECShakeCamera2(DWORD dwPeriod, bool bShakebyDistance)
: CECShakeCameraBase(dwPeriod, bShakebyDistance)
, m_dwShakeTime(0)
, m_dwHalfShakeTime(0)
{
	m_vLastShakeOffset.Clear();
	m_vTotalShakeOffset.Clear();

	m_pShakeNoise = new A3DGFXCtrlNoiseBase();
	m_pShakeNoise->Resume();
	m_pAffector = ShakeAffector::CreateAffector(ShakeAffector::SA_DAMPING_QUADRATIC,m_pShakeNoise);
}

CECShakeCamera2::~CECShakeCamera2()
{
	delete m_pShakeNoise;
	delete m_pAffector;
}

A3DVECTOR3 CECShakeCamera2::GetShakeOffset() const
{
	return m_vShakeOffset;
}
void CECShakeCamera2::Start()
{
	SetShaking(true);
	m_dwShakeTime = 0;
	m_dwHalfShakeTime = 0;
	m_iShakeCycleCnt = 0;
	m_vLastShakeOffset.Clear();
	m_vTotalShakeOffset.Clear();
	m_bCalcNewDir = true;
	AfxBeginShakeCam();
}
bool CECShakeCamera2::Tick( DWORD dwTickTime, A3DCamera* pCamera, float fDistToCam )
{
	if(!CECShakeCameraBase::Tick(dwTickTime,pCamera,fDistToCam))
		return false;

	ASSERT( GetShakePeriod() != 0 );

	const float fTimeSpan = GetTimeSpan();
	A3DGFXCtrlNoiseBase* pShakeNoise = m_pShakeNoise;
	
	if (!pShakeNoise
		|| !pShakeNoise->Tick(fTimeSpan))
	{
		Stop();
		return true;
	}		
	
	const DWORD dwPeriod = GetShakePeriod();
	//	每周期重新计算方向
	if (m_dwShakeTime / dwPeriod != (DWORD)m_iShakeCycleCnt)
	{
		m_iShakeCycleCnt = m_dwShakeTime / dwPeriod;
		m_bCalcNewDir = true;
	}

	// 需要计算新的振动方向
	if (m_bCalcNewDir)
	{
		//	超过振动距离就不振了
		if (fDistToCam >= _shake_attenu_dist)
			m_vDir.Clear();
		else
		{
			const float fCo = GetShakeByDistance() ? CalcDistCo(fDistToCam) : 1.0f;
			const A3DVECTOR3 v = GenerateDirection(static_cast<SHAKECAM>(m_shakeType), pCamera);
			//m_vDir = (pShakeNoise->GetNoiseVal(dwTimeSpan) * fCo) * v;
			m_vDir = (pShakeNoise->GetAmplitude() * fCo) * v;
		}
	}

	m_dwShakeTime += dwTickTime;

	//	半周期衰减振动
	DWORD dwHalfPeriod = dwPeriod / 2;
	if ( m_dwHalfShakeTime >= dwHalfPeriod )
	{
		m_dwHalfShakeTime -= dwHalfPeriod;

		ShakeAffector* pAffector = m_pAffector;
		if (pAffector)
			pAffector->AffectShakeOffset(fTimeSpan, m_vDir);
	}

	m_dwHalfShakeTime += dwTickTime;

	//	offset
	A3DVECTOR3 vCurrent = m_vDir * (float)sin( A3D_2PI * (float)m_dwShakeTime / (float)dwPeriod );
	m_vShakeOffset = vCurrent - m_vLastShakeOffset;
	m_vLastShakeOffset = vCurrent;
	
	AfxSetCamOffset(m_vShakeOffset);
	m_vTotalShakeOffset += m_vShakeOffset;

	m_bCalcNewDir = false;

	return true;
}

void CECShakeCamera2::Stop()
{
	ASSERT( GetShakePeriod() != 0 );

	if (!GetShaking()) return;
	AfxSetCamOffset(-m_vTotalShakeOffset);
	AfxEndShakeCam();
	m_vTotalShakeOffset.Clear();
	m_vLastShakeOffset.Clear();
	SetShaking(false);
}
#include "StdAfx.h"
#include "A3DGFXShakeCamera.h"


A3DVECTOR3 GenerateDirection(SHAKECAM sc, A3DCamera* pCamera)
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

//////////////////////////////////////////////////////////////////////////
//
//	Implement SANone
//
//////////////////////////////////////////////////////////////////////////

class SANone : public ShakeAffector
{
public:
	SANone(const A3DGFXCtrlNoiseBase* pShakeNoise)
		: ShakeAffector(pShakeNoise, ShakeAffector::SA_NONE)
	{

	}
	
	virtual void AffectShakeOffset(float fTimeSpan, A3DVECTOR3& vCurShake) {}
};

//////////////////////////////////////////////////////////////////////////
//
//	Implement SADampingLinear
//
//////////////////////////////////////////////////////////////////////////

class SADampingLinear : public ShakeAffector
{
public:
	
	SADampingLinear(const A3DGFXCtrlNoiseBase* pShakeNoise);
	virtual void AffectShakeOffset(float fTimeSpan, A3DVECTOR3& vCurShake);

protected:
	float m_fInvShakeTime;
};

SADampingLinear::SADampingLinear(const A3DGFXCtrlNoiseBase* pShakeNoise)
: ShakeAffector(pShakeNoise, ShakeAffector::SA_DAMPING_LINEAR)
{
	m_fInvShakeTime = 1.0f / (m_pShakeNoise->GetEndTm() - m_pShakeNoise->GetStartTm());
}

void SADampingLinear::AffectShakeOffset(float fTimeSpan, A3DVECTOR3& vCurShake)
{
	if (!(m_pShakeNoise->AfterStart(fTimeSpan) && m_pShakeNoise->BeforeEnd(fTimeSpan)))
		return;

	const float fCurFactor = 1.0f - (fTimeSpan - m_pShakeNoise->GetStartTm()) * m_fInvShakeTime;
	vCurShake *= fCurFactor;
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement SADampingQuadratic
//
//////////////////////////////////////////////////////////////////////////

class SADampingQuadratic : public ShakeAffector
{
public:
	
	SADampingQuadratic(const A3DGFXCtrlNoiseBase* pShakeNoise);
	virtual void AffectShakeOffset(float fTimeSpan, A3DVECTOR3& vCurShake);

protected:
	float m_fStartTm, m_fEndTm;
	float m_fInvShakeTimeSquare;
};

SADampingQuadratic::SADampingQuadratic(const A3DGFXCtrlNoiseBase* pShakeNoise)
: ShakeAffector(pShakeNoise, ShakeAffector::SA_DAMPING_QUADRATIC)
, m_fStartTm(m_pShakeNoise->GetStartTm())
, m_fEndTm(m_pShakeNoise->GetEndTm())
{
	float fInvShakeTime = 1.0f / (m_fEndTm - m_fStartTm);
	m_fInvShakeTimeSquare = fInvShakeTime * fInvShakeTime;
}

void SADampingQuadratic::AffectShakeOffset(float fTimeSpan, A3DVECTOR3& vCurShake)
{
	if (!(m_pShakeNoise->AfterStart(fTimeSpan) && m_pShakeNoise->BeforeEnd(fTimeSpan)))
		return;
	
	const float fXSubEnd = m_fEndTm - fTimeSpan;
	const float fCurFactor = m_fInvShakeTimeSquare * fXSubEnd * fXSubEnd;
	vCurShake *= fCurFactor;
}

ShakeAffector* ShakeAffector::CreateAffector(int iType, const A3DGFXCtrlNoiseBase* pShakeNoise)
{
	if (!pShakeNoise)
		return NULL;

	switch(iType)
	{
	case SA_DAMPING_LINEAR:
		return new SADampingLinear(pShakeNoise);
	case SA_DAMPING_QUADRATIC:
	    return new SADampingQuadratic(pShakeNoise);
	default:
	    return new SANone(pShakeNoise);
	}
}

static const char _shake_cam_period[]				= "ShakePeriod: %d";
static const char _shake_cam_shakebydistance[]		= "ShakeByDistance: %d";
static const char _shake_cam_blur_info[]			= "ShakeCamBlur: %f, %d";
static const char _shake_cam_blur_max_pixel_off[]	= "ShakeCamPixOff: %f";
static const char _shake_cam_attenu_dist[]			= "ShakeCamAttenu: %f";

//////////////////////////////////////////////////////////////////////////
//
//	Implement IShakeCamera
//
//////////////////////////////////////////////////////////////////////////

IShakeCamera::IShakeCamera(A3DGFXEx* pGFXEx, DWORD dwPeriod, bool bShakebyDistance, const A3DGFX_CAMERA_BLUR_INFO& info, float fShakeAttenuDist)
: m_pGfx(pGFXEx)
, m_bShaking(false)
, m_vShakeOffset(0)
, m_dwPeriod(dwPeriod)
, m_bShakeByDistance(bShakebyDistance)
, m_CameraBlurInfo(info)
, m_fShakeAttenuDist(fShakeAttenuDist) 
{
	ASSERT( pGFXEx );
}

IShakeCamera* IShakeCamera::CreateShakeCam(A3DGFXEx* pGfx, DWORD dwPeriod, bool bShakeByDistance, const A3DGFX_CAMERA_BLUR_INFO& info, float fShakeAttenuDist)
{
	if (dwPeriod == 0)
		return new ShakeCamera(pGfx, dwPeriod, bShakeByDistance, info, fShakeAttenuDist);
	else
		return new ShakeCamera2(pGfx, dwPeriod, bShakeByDistance, info, fShakeAttenuDist);
}

IShakeCamera* IShakeCamera::LoadFromFile(AFile* pFile, DWORD dwVersion, A3DGFXEx* pGfx)
{
	A3DGFX_CAMERA_BLUR_INFO BlurInfo;
	float fShakeAttenuDist = 30.f;

	if (dwVersion >= 90)
	{
		char szLine[AFILE_LINEMAXLEN];
		DWORD dwReadLen;
		DWORD dwPeriod = 0;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _shake_cam_period, &dwPeriod);
		
		int iReadInt = 0;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _shake_cam_shakebydistance, &iReadInt);
		bool bShakeByDistance = iReadInt != 0;

		if (dwVersion >= 97)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _shake_cam_blur_info, &BlurInfo.fFrameCo, &BlurInfo.dwFadeTime);

			if (dwVersion >= 98)
			{
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _shake_cam_blur_max_pixel_off, &BlurInfo.fMaxPixelOffset);
			}
		}

		if (dwVersion >= 121)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _shake_cam_attenu_dist, &fShakeAttenuDist);
		}

		return CreateShakeCam(pGfx, dwPeriod, bShakeByDistance, BlurInfo, fShakeAttenuDist);
	}
	else
		return new ShakeCamera(pGfx, 0, true, BlurInfo, fShakeAttenuDist);
}

void IShakeCamera::SaveToFile(IShakeCamera* pShakeCamera, AFile* pFile)
{
	char	szLine[AFILE_LINEMAXLEN];
	
	sprintf(szLine, _shake_cam_period, pShakeCamera->GetShakePeriod());
	pFile->WriteLine(szLine);
	
	sprintf(szLine, _shake_cam_shakebydistance, pShakeCamera->GetShakeByDistance());
	pFile->WriteLine(szLine);

	sprintf(szLine, _shake_cam_blur_info, pShakeCamera->GetCameraBlurInfo().fFrameCo, pShakeCamera->GetCameraBlurInfo().dwFadeTime);
	pFile->WriteLine(szLine);

	sprintf(szLine, _shake_cam_blur_max_pixel_off, pShakeCamera->GetCameraBlurInfo().fMaxPixelOffset);
	pFile->WriteLine(szLine);

	sprintf(szLine, _shake_cam_attenu_dist, pShakeCamera->GetAttenuDist());
	pFile->WriteLine(szLine);
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement ShakeCamera
//
//////////////////////////////////////////////////////////////////////////

ShakeCamera::ShakeCamera(A3DGFXEx* pGfx, DWORD dwPeriod, bool bShakebyDistance, const A3DGFX_CAMERA_BLUR_INFO& info, float fShakeAttenuDist)
: IShakeCamera(pGfx, dwPeriod, bShakebyDistance, info, fShakeAttenuDist)
, m_nShakeCount(0)
{
}

ShakeCamera::~ShakeCamera()
{

}

void ShakeCamera::Tick(DWORD dwTickTime, DWORD dwTimeSpan, A3DCamera* pCamera, float fDistToCam)
{
	ASSERT( GetShakePeriod() == 0 );
	const float fTimeSpan = dwTimeSpan / 1000.f;
	A3DGFXCtrlNoiseBase* pShakeNoise = GetGfx()->GetShakeNoise();
	ASSERT( pShakeNoise );
	
	if (!pShakeNoise
		|| (!pShakeNoise->Tick(fTimeSpan)) )
	{
		Stop();
		return;
	}
	
	if (!GetShaking())
	{
		SetShaking(true);
		m_nShakeCount = 0;
		AfxBeginShakeCam();
	}
	
	if ((m_nShakeCount++ % 2) == 0)
	{
		if (fDistToCam >= GetAttenuDist())
			m_vShakeOffset.Clear();
		else
		{
			const A3DVECTOR3 v = GenerateDirection(static_cast<SHAKECAM>(GetGfx()->GetShakeCamType()), pCamera);
			const float fCo = GetShakeByDistance() ? CalcDistCo(fDistToCam) : 1.0f;
			m_vShakeOffset = (pShakeNoise->GetNoiseVal(dwTimeSpan) * fCo) * v;
		}
		
		ShakeAffector* pAffector = GetGfx()->GetShakeAffector();
		if (pAffector)
			pAffector->AffectShakeOffset(fTimeSpan, m_vShakeOffset);
	}
	else
		m_vShakeOffset = -m_vShakeOffset;
	
	AfxSetCamOffset(m_vShakeOffset);

	if (IsEnableCamBlur() &&
		GetCameraBlurInfo().fFrameCo > 0.0f && A3DGFXExMan::GetCameraBlurInfo().fFrameCo <= 0.0f)
		A3DGFXExMan::SetCameraBlurInfo(GetCameraBlurInfo());
}

A3DVECTOR3 ShakeCamera::GetShakeOffset() const
{
	return m_vShakeOffset;
}

void ShakeCamera::Stop()
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

ShakeCamera2::ShakeCamera2( A3DGFXEx* pGfx, DWORD dwPeriod, bool bShakebyDistance, const A3DGFX_CAMERA_BLUR_INFO& info, float fShakeAttenuDist )
: IShakeCamera(pGfx, dwPeriod, bShakebyDistance, info, fShakeAttenuDist)
, m_dwShakeTime(0)
, m_dwHalfShakeTime(0)
, m_iShakeCycleCnt(0)
{
	m_vLastShakeOffset.Clear();
	m_vTotalShakeOffset.Clear();
}

ShakeCamera2::~ShakeCamera2()
{
}

A3DVECTOR3 ShakeCamera2::GetShakeOffset() const
{
	return m_vShakeOffset;
}

void ShakeCamera2::Tick( DWORD dwTickTime, DWORD dwTimeSpan, A3DCamera* pCamera, float fDistToCam )
{
	ASSERT( GetShakePeriod() != 0 );

	const float fTimeSpan = dwTimeSpan / 1000.f;
	A3DGFXCtrlNoiseBase* pShakeNoise = GetGfx()->GetShakeNoise();
	
	if (!pShakeNoise
		|| !pShakeNoise->Tick(fTimeSpan))
	{
		Stop();
		return;
	}

	bool bCalcNewDir;
	if (!GetShaking())
	{
		SetShaking(true);
		m_dwShakeTime = 0;
		m_dwHalfShakeTime = 0;
		m_iShakeCycleCnt = 0;
		bCalcNewDir = true;
		m_vLastShakeOffset.Clear();
		m_vTotalShakeOffset.Clear();
		AfxBeginShakeCam();
	}
	else
		bCalcNewDir = false;

	const DWORD dwPeriod = GetShakePeriod();
	//	每周期重新计算方向
	if (m_dwShakeTime / dwPeriod != (DWORD)m_iShakeCycleCnt)
	{
		m_iShakeCycleCnt = m_dwShakeTime / dwPeriod;
		bCalcNewDir = true;
	}

	// 需要计算新的振动方向
	if (bCalcNewDir)
	{
		//	超过振动距离就不振了
		if (fDistToCam >= GetAttenuDist())
			m_vDir.Clear();
		else
		{
			const float fCo = GetShakeByDistance() ? CalcDistCo(fDistToCam) : 1.0f;
			const A3DVECTOR3 v = GenerateDirection(static_cast<SHAKECAM>(GetGfx()->GetShakeCamType()), pCamera);
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

		ShakeAffector* pAffector = GetGfx()->GetShakeAffector();
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

	if (GetCameraBlurInfo().fFrameCo > 0.0f && A3DGFXExMan::GetCameraBlurInfo().fFrameCo <= 0.0f)
		A3DGFXExMan::SetCameraBlurInfo(GetCameraBlurInfo());
}

void ShakeCamera2::Stop()
{
	ASSERT( GetShakePeriod() != 0 );

	if (!GetShaking()) return;
	AfxSetCamOffset(-m_vTotalShakeOffset);
	AfxEndShakeCam();
	m_vTotalShakeOffset.Clear();
	m_vLastShakeOffset.Clear();
	SetShaking(false);
}

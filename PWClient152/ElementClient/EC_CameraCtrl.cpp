// Filename	: EC_CameraCtrl.cpp
// Creator	: Tom Zhou
// Date		: September 21, 2004

#include "EC_Global.h"
#include "EC_CameraCtrl.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Viewport.h"
#include "EC_World.h"
#include "EC_Configs.h"

#include "A3DCamera.h"
#include "A3DFuncs.h"

#define CECCAMERA_DISTANCE_NEAR		0.5f
#define CECCAMERA_DISTANCE_MIDDLE	(CECCAMERA_MAX_DISTANCE / 2.0f)
#define CECCAMERA_DISTANCE_FAR		CECCAMERA_MAX_DISTANCE

#define CECCAMERA_DEFAULT_PITCH		(-40.0f)

#define CECCAMERA_MIN_ZOOM			0.5f
#define CECCAMERA_MAX_ZOOM			2.0f

#define PRESSING(vk)				(GetAsyncKeyState(vk) & 0x8000)

A3DVECTOR3 CECCamera::m_vecAxisX = A3DVECTOR3(1.0f, 0.0f, 0.0f);
A3DVECTOR3 CECCamera::m_vecAxisY = A3DVECTOR3(0.0f, 1.0f, 0.0f);
A3DVECTOR3 CECCamera::m_vecAxisZ = A3DVECTOR3(0.0f, 0.0f, 1.0f);

CECCamera::CECCamera()
{
	m_bFixed = false;
	m_bSliding = false;
	m_bSceneryMode = false;
	m_nCameraPos = CAMERA_POS_GROUND;
	m_nCameraMode = CAMERA_MODE_MIDDLE;
	m_vecDirCam = A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_vecUpCam = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	m_fPitch = m_fPitchDest = 0.0f;
	m_fDegree = m_fDegreeDest = 0.0f;
	m_fDistance = m_fDistanceDest = CECCAMERA_MAX_DISTANCE * 2.0f / 3.0f;

	memset(m_fPitchUser, 0, sizeof(float) * CECCAMERA_MAX_USERDEFINE);
	memset(m_fDistanceUser, 0, sizeof(float) * CECCAMERA_MAX_USERDEFINE);
	memset(m_bVirtualKeyState, false, sizeof(m_bVirtualKeyState));
}

CECCamera::~CECCamera()
{
}

bool CECCamera::Init(P_INIT_DATA pInitData)
{
	//m_fDistance = m_fDistanceDest = pInitData->fDistance;
	m_vecDirCam = Normalize(pInitData->vecDirCam);
	m_vecUpCam = Normalize(pInitData->vecUpCam);

	m_fPitch = m_fPitchDest = -float(RAD2DEG(acos(DotProduct(m_vecAxisY, m_vecUpCam))));

	return true;
}

bool CECCamera::Release()
{
	return true;
}

bool CECCamera::Action(P_ACTION_DATA pActionData)
{
	if( m_bFixed ) return true;

	const EC_GAME_SETTING& gs = g_pGame->GetConfigs()->GetGameSettings();
	float fDegTick = gs.fCamTurnSpeed;

	// Adjust camera degree.
	if( pActionData->nDeltaX < 0 )
	{
		m_fDegree += fDegTick * pActionData->nDeltaX / 80;
		if( m_fDegree < 0.0f ) m_fDegree += 360.0f;
		m_fDegreeDest = m_fDegree;
	}
	else if( pActionData->nDeltaX > 0 )
	{
		m_fDegree += fDegTick * pActionData->nDeltaX / 80;
		if( m_fDegree > 360.0f ) m_fDegree -= 360.0f;
		m_fDegreeDest = m_fDegree;
	}

	// Adjust camera pitch.
	if( pActionData->nDeltaY < 0 )
	{
		m_fPitch -= fDegTick * pActionData->nDeltaY / 80;
		if( m_fPitch > 89.0f ) m_fPitch = 89.0f;
		m_fPitchDest = m_fPitch;
	}
	else if( pActionData->nDeltaY > 0 )
	{
		m_fPitch -= fDegTick * pActionData->nDeltaY / 80;
		if( m_fPitch < -89.0f ) m_fPitch = -89.0f;
		m_fPitchDest = m_fPitch;
	}

	// Mouse wheel roll.
	if( pActionData->nDeltaZ < 0 )
	{
		m_bSliding = true;
		m_fDistanceDest -= gs.fCamZoomSpeed;
		if( m_fDistanceDest < CECCAMERA_MIN_DISTANCE )
			m_fDistanceDest = CECCAMERA_MIN_DISTANCE;
	}
	else if( pActionData->nDeltaZ > 0 )
	{
		m_bSliding = true;
		m_fDistanceDest += gs.fCamZoomSpeed;
		if( m_fDistanceDest > CECCAMERA_MAX_DISTANCE )
			m_fDistanceDest = CECCAMERA_MAX_DISTANCE;
	}

	// Key action.
	if( pActionData->nAction == ACTION_DEFAULT )
	{
		pActionData->vecDirChar.y = 0.0f;
		float fDot = DotProduct(Normalize(pActionData->vecDirChar), m_vecAxisZ);
		float fDegChar = float(RAD2DEG(acos(fDot)));
		if( pActionData->vecDirChar.x < 0.0f ) fDegChar = 360.0f - fDegChar;
		
		if( fabs(m_fDegree - fDegChar) < gs.fCamTurnSpeed )
			m_fDegreeDest = fDegChar + 180.0f;
		else
			m_fDegreeDest = fDegChar;

		m_fPitchDest = CECCAMERA_DEFAULT_PITCH;
		m_fDistanceDest = CECCAMERA_DISTANCE_MIDDLE;
		m_bSliding = true;
	}
	else if( pActionData->nAction == ACTION_PRESET )
	{
		m_nCameraMode++;
		m_nCameraMode %= CAMERA_MODE_MAX;
		if( m_nCameraMode == CAMERA_MODE_NEAR )
		{
			m_fPitchDest = 0.0f;
			m_fDistanceDest = CECCAMERA_DISTANCE_NEAR;
		}
		else if( m_nCameraMode == CAMERA_MODE_MIDDLE )
		{
			m_fPitchDest = CECCAMERA_DEFAULT_PITCH;
			m_fDistanceDest = CECCAMERA_DISTANCE_MIDDLE;
		}
		else if( m_nCameraMode == CAMERA_MODE_FAR )
		{
			m_fPitchDest = CECCAMERA_DEFAULT_PITCH;
			m_fDistanceDest = CECCAMERA_DISTANCE_FAR;
		}
		m_bSliding = true;
	}

	UpdateDirAndUp();

	return true;
}

bool CECCamera::Tick(P_TICK_DATA pTickData, P_ADJUST_DATA pAdjustData)
{
	const EC_GAME_SETTING& gs = g_pGame->GetConfigs()->GetGameSettings();

	if( !m_bFixed )
	{
		float fDegTick = gs.fCamTurnSpeed * pTickData->dwTime / 1000.0f;
		float fZoomTick = gs.fCamZoomSpeed * pTickData->dwTime / 1000.0f;

		if( m_bSliding )
		{
			if( fabs(m_fPitchDest - m_fPitch) < fDegTick &&
				fabs(m_fDegreeDest - m_fDegree) < fDegTick &&
				fabs(m_fDistanceDest - m_fDistance) < fZoomTick )
			{
				m_fPitch = m_fPitchDest;
				m_fDegree = m_fDegreeDest;
				m_fDistance = m_fDistanceDest;
				m_bSliding = false;
			}
			else
			{
				if( fabs(m_fPitchDest - m_fPitch) >= fDegTick )
					m_fPitch += (m_fPitchDest - m_fPitch) * pTickData->dwTime / 200.0f;
				if( fabs(m_fDegreeDest - m_fDegree) >= fDegTick )
					m_fDegree += (m_fDegreeDest - m_fDegree) * pTickData->dwTime / 200.0f;
				if( fabs(m_fDistanceDest - m_fDistance) >= fZoomTick )
					m_fDistance += (m_fDistanceDest - m_fDistance) * pTickData->dwTime / 200.0f;
			}
		}
		else if( 0 && pTickData->bRunning && !pTickData->bTuning )
		{
			pTickData->vecDirChar.y = 0.0f;
			float fDot = DotProduct(Normalize(pTickData->vecDirChar), m_vecAxisZ);
			float fDegChar = float(RAD2DEG(acos(fDot)));
			if( pTickData->vecDirChar.x < 0.0f ) fDegChar = 360.0f - fDegChar;

			float fDegBetween = m_fDegree - fDegChar;
			if( fDegBetween > 180.0f )
				fDegBetween -= 360.0f;
			else if( fDegBetween < -180.0f )
				fDegBetween += 360.0f;

			if( fabs(fDegBetween) <= fDegTick )
				m_fDegree = fDegChar;
			else if( fDegBetween > 0.0f )
				m_fDegree -= fDegTick;
			else if( fDegBetween < 0.0f )
				m_fDegree += fDegTick;

			m_fDegreeDest = m_fDegree;
		}

		UpdateDirAndUp();
	}

	pAdjustData->vecDirCam = m_vecDirCam;
	pAdjustData->vecUpCam = m_vecUpCam;

	if( m_bSceneryMode )
	{
		float fDist = 3.0f * pTickData->dwTime / 1000.0f;
		A3DCamera *pCamera = g_pGame->GetViewport()->GetA3DCamera();
		float fFOV = RAD2DEG(pCamera->GetFOV());
		A3DVECTOR3 vecPosCam = pCamera->GetPos();

		if( PRESSING(VK_DELETE) || m_bVirtualKeyState[0] )
			pCamera->MoveLeft(fDist);
		else if( PRESSING(VK_NEXT) || m_bVirtualKeyState[1] )
			pCamera->MoveRight(fDist);
		else if( PRESSING(VK_HOME) || m_bVirtualKeyState[2] )
		{
			if( PRESSING(VK_CONTROL) || m_bVirtualKeyState[6] )
			{
				if( fFOV > 20.0f ) fFOV -= 1.0f;
				pCamera->SetFOV(DEG2RAD(fFOV));
				pCamera->SetFOVSet(DEG2RAD(fFOV));
			}
			else
				pCamera->MoveFront(fDist);
		}
		else if( PRESSING(VK_END) || m_bVirtualKeyState[3] )
		{
			if( PRESSING(VK_CONTROL) || m_bVirtualKeyState[6] )
			{
				if( fFOV < DEFCAMERA_FOV ) fFOV += 1.0f;
				pCamera->SetFOV(DEG2RAD(fFOV));
				pCamera->SetFOVSet(DEG2RAD(fFOV));
			}
			else
				pCamera->MoveBack(fDist);
		}
		else if( PRESSING(VK_INSERT) || m_bVirtualKeyState[4] )
			pCamera->Move(fDist * g_vAxisY);
		else if( PRESSING(VK_PRIOR) || m_bVirtualKeyState[5] )
			pCamera->Move(-fDist * g_vAxisY);

		if( Magnitude(pCamera->GetPos() - pTickData->vecPosChar) <= CECCAMERA_MAX_DISTANCE + 3 )
			pAdjustData->vecPosCam = pCamera->GetPos();
		else
			pAdjustData->vecPosCam = vecPosCam;
	}
	else
		pAdjustData->vecPosCam = pTickData->vecPosChar - m_fDistance * m_vecDirCam;

	static float fMinDist = 0.5f;
	float fWaterHeight = g_pGame->GetGameRun()->
		GetWorld()->GetWaterHeight(pAdjustData->vecPosCam);

	if( pTickData->bWater )
	{
		if( pAdjustData->vecPosCam.y > fWaterHeight - fMinDist )
			pAdjustData->vecPosCam.y = fWaterHeight - fMinDist;
	}
	else
	{
		if( pAdjustData->vecPosCam.y < fWaterHeight + fMinDist )
			pAdjustData->vecPosCam.y = fWaterHeight + fMinDist;
		else if( (float)fabs(pAdjustData->vecPosCam.y - fWaterHeight) < fMinDist )
		{
			if( pAdjustData->vecPosCam.y < fWaterHeight )
				pAdjustData->vecPosCam.y = fWaterHeight - fMinDist;
			else
				pAdjustData->vecPosCam.y = fWaterHeight + fMinDist;
		}
	}

	return true;
}

bool CECCamera::UpdateDirAndUp()
{
	m_vecDirCam.x = (FLOAT)(sin(DEG2RAD(m_fDegree)) * cos(DEG2RAD(m_fPitch)));
	m_vecDirCam.y = (FLOAT)sin(DEG2RAD(m_fPitch));
	m_vecDirCam.z = (FLOAT)(cos(DEG2RAD(m_fDegree)) * cos(DEG2RAD(m_fPitch)));

	A3DVECTOR3 vecRightCam = Normalize(CrossProduct(m_vecAxisY, m_vecDirCam));
	m_vecUpCam = Normalize(CrossProduct(m_vecDirCam, vecRightCam));

	return true;
}

void CECCamera::UserView_Define(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < CECCAMERA_MAX_USERDEFINE);

	m_fPitchUser[nIndex] = m_fPitch;
	m_fDistanceUser[nIndex] = m_fDistance;
}

void CECCamera::UserView_Retrieve(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < CECCAMERA_MAX_USERDEFINE);

	m_bSliding = true;
	m_fPitchDest = m_fPitchUser[nIndex];
	m_fDistanceDest = m_fDistanceUser[nIndex];
}

int CECCamera::CameraPos_Get()
{
	return m_nCameraPos;
}

void CECCamera::CameraPos_Set(int nPos)
{
	m_nCameraPos = nPos;
}

bool CECCamera::Fix_Get()
{
	return m_bFixed;
}

void CECCamera::Fix_Set(bool bFix)
{
	m_bFixed = bFix;
}

bool CECCamera::SceneryMode_Get()
{
	return m_bSceneryMode;
}

void CECCamera::SceneryMode_Set(bool bSet)
{
	m_bSceneryMode = bSet;
	g_pGame->GetViewport()->GetA3DCamera()->SetFOV(DEG2RAD(DEFCAMERA_FOV));
	g_pGame->GetViewport()->GetA3DCamera()->SetFOVSet(DEG2RAD(DEFCAMERA_FOV));
}

void CECCamera::SetVirtualKeyState(bool *bState)
{
	memcpy(m_bVirtualKeyState, bState, sizeof(m_bVirtualKeyState));
}
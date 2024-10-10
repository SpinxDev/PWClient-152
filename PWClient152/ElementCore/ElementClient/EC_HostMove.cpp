/*
 * FILE: EC_HostMove.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_HostMove.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EC_GameSession.h"
#include "EC_Utility.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_CDR.h"
#include "EC_AssureMove.h"
#include "EC_Instance.h"

#include "A3DFuncs.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Move length minimum threshold
#define MIN_MOVELEN_IN_AIR_WATER		0.5f
#define MIN_MOVELEN_ON_GROUND			0.5f
#define MIN_MOVELEN_FOR_DETECT_VIBRATION 0.05f

//	Interval of sending move command
#define MOVECMD_INTERVAL	500

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

static const A3DVECTOR3 l_vGravity(0.0f, -EC_GRAVITY, 0.0f);

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

//	Calculate average speed
float l_CalcAverageSpeed(const A3DVECTOR3& p1, const A3DVECTOR3& p2, float fTime, float fDefSpeed)
{
	if (!fTime)
		return fDefSpeed;

	A3DVECTOR3 d = p2 - p1;
	float fSpeed = d.Magnitude() / fTime;
	if (fTime < 0.05f || fSpeed > 50.0f)
	{
	//	ASSERT(0);
		g_pGame->RuntimeDebugInfo(0xffffffff, _AL("speed: %f, time: %f"), fSpeed, fTime);
		return fDefSpeed;
	}

	return fSpeed;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHostMove
//	
///////////////////////////////////////////////////////////////////////////

CECHostMove::CECHostMove(CECHostPlayer* pHost)
{
	m_pHost			= pHost;
	m_bStop			= true;
	m_iBlockedCnt	= 0;
	m_dwLastTime	= 80;
	m_fAverSpeedH	= 0.0f;
	m_wMoveStamp	= 0;
	m_bSlideLock	= false;
	m_fMoveTime		= 0.0f;
	m_fBlockTime	= 0.0f;
	m_bLocalMove	= false;
	m_fBlockMove	= 0.0f;
	m_vBlockMove.Clear();

	m_CmdTimeCnt.SetPeriod(MOVECMD_INTERVAL);
	m_StopCmdCnt.SetPeriod(MOVECMD_INTERVAL);
	m_StopCmdCnt.Reset(true);
//	m_CmdTimeCnt.SetPeriod(100);
	
	memset(&m_DelayedStop, 0, sizeof (m_DelayedStop));
}

CECHostMove::~CECHostMove()
{
}

//	Tick routine
void CECHostMove::Tick(DWORD dwDeltaTime)
{
	DWORD dwRealTime = g_pGame->GetRealTickTime();
	m_CmdTimeCnt.IncCounter(dwRealTime);

	if (m_StopCmdCnt.IncCounter(dwRealTime) && m_DelayedStop.bValid)
	{
		const STOPMOVE& m = m_DelayedStop;
		DWORD dwCurrent = a_GetTime();
		int iTime = (int)(m.fTime * 1000) + (dwCurrent > m.dwTimeStamp)?(dwCurrent-m.dwTimeStamp):0;
		float fSpeed = l_CalcAverageSpeed(m_vLastSevPos, m.vPos, iTime * 0.001f, m.fSpeed);

		int iMoveMode = m.iMoveMode;
		if( g_pGame->GetGameRun()->GetWorld()->GetAssureMove()->IsCheatCD() || 
			g_pGame->GetGameRun()->GetWorld()->GetAssureMove()->IsCheatFly() )
			iMoveMode |= GP_MOVE_DEAD;

		// make a potential check with tuojigua
		iMoveMode |= GP_MOVE_DEAD;

		if (!m_bLocalMove)
			g_pGame->GetGameSession()->c2s_CmdStopMove(m.vPos, fSpeed, iMoveMode, m.byDir, m_wMoveStamp++, iTime);

		//	Record this position
		m_vLastSevPos = m.vPos;

		Reset();
	}

	//	Calculate host average horizonal speed
	a_ClampFloor(m_dwLastTime, (DWORD)1);
	A3DVECTOR3 vOffset = m_pHost->GetPos() - m_vLastPos;
	m_fAverSpeedH = vOffset.MagnitudeH() * 1000.0f / m_dwLastTime;
	m_vLastPos	 = m_pHost->GetPos();
	m_dwLastTime = dwDeltaTime;
}

//	Reset move control to stop state
void CECHostMove::Reset()
{
	m_StopCmdCnt.Reset();
	m_bStop = true;
	m_fBlockTime = 0.0f;
	m_fBlockMove = 0.0f;
	m_vBlockMove.Clear();

	m_DelayedStop.bValid = false;
}

//	Forcase moving and send move command
void CECHostMove::SendMoveCmd(const A3DVECTOR3& vCurPos, int iDestType, const A3DVECTOR3& vDest,
							  const A3DVECTOR3& vMoveDir, float fSpeed, int iMoveMode, bool bForceSend)
{
	if (m_pHost->m_CameraCtrl.SceneryMode_Get())
		m_pHost->m_CameraCtrl.SceneryMode_Set(false);

	if (m_pHost->IsJumping())
	{
		iMoveMode &= GP_MOVE_ENVMASK;
		iMoveMode += GP_MOVE_JUMP;
	}

	//	If stop flag is true, that means this is the first step, so reset
	//	time counter to ensure first move command will be sent 500ms later
	//	rather than be sent immediately
	if (m_bStop)
	{
	//	m_CmdTimeCnt.Reset();
		m_CmdTimeCnt.SetCounter((int)(m_fMoveTime * 1000));
		m_bStop = false;
	}

	if(!bForceSend && !m_CmdTimeCnt.IsFull())
		return;

	int iTime = (int)(m_fMoveTime * 1000);

//	if(bForceSend && iTime < 200)
//		iTime = 200;
	
	if (iTime < 200)
	{
		if(iTime ==0 || !bForceSend)
		{
			//	if time is too little, wait again
			m_CmdTimeCnt.SetCounter(iTime);
			return;
		}
	}

	m_CmdTimeCnt.Reset();
	m_DelayedStop.bValid = false;

	//	Use average speed
	fSpeed = l_CalcAverageSpeed(m_vLastSevPos, vCurPos, m_fMoveTime, fSpeed);
	m_fMoveTime = 0.0f;

	if( g_pGame->GetGameRun()->GetWorld()->GetAssureMove()->IsCheatCD() || 
		g_pGame->GetGameRun()->GetWorld()->GetAssureMove()->IsCheatFly() )
		iMoveMode |= GP_MOVE_DEAD;

	// make a potential check with tuojigua
	iMoveMode |= GP_MOVE_DEAD;
	
	if (!m_bLocalMove)
		g_pGame->GetGameSession()->c2s_CmdPlayerMove(vCurPos, vCurPos, iTime/* MOVECMD_INTERVAL */, fSpeed, iMoveMode, m_wMoveStamp++);
	
	//	Record this position
	m_vLastSevPos = vCurPos;

	if( rand() % 10 == 0 )
		glb_RepairExeInMemory();
}


//	Send stop move command
void CECHostMove::SendStopMoveCmd(const A3DVECTOR3& vPos, float fSpeed, int iMoveMode)
{
	if (m_pHost->m_CameraCtrl.SceneryMode_Get())
		m_pHost->m_CameraCtrl.SceneryMode_Set(false);

	if (m_pHost->IsJumping())
	{
		iMoveMode &= GP_MOVE_ENVMASK;
		iMoveMode += GP_MOVE_JUMP;
	}

	if( g_pGame->GetGameRun()->GetWorld()->GetAssureMove()->IsCheatCD() || 
		g_pGame->GetGameRun()->GetWorld()->GetAssureMove()->IsCheatFly() )
		iMoveMode |= GP_MOVE_DEAD;

	// make a potential check with tuojigua
	iMoveMode |= GP_MOVE_DEAD;
	
	if (m_StopCmdCnt.IsFull())
	{
		A3DVECTOR3 vDir = m_pHost->GetDir();
		BYTE byDir = glb_CompressDirH(vDir.x, vDir.z);
		fSpeed = l_CalcAverageSpeed(m_vLastSevPos, vPos, m_fMoveTime, fSpeed);
		int iTime = (int)(m_fMoveTime * 1000);

		if (!m_bLocalMove)
			g_pGame->GetGameSession()->c2s_CmdStopMove(vPos, fSpeed, iMoveMode, byDir, m_wMoveStamp++, iTime);

		//	Record this position
		m_vLastSevPos = vPos;

		Reset();
	}
	else
	{
		A3DVECTOR3 vDir = m_pHost->GetDir();
		BYTE byDir = glb_CompressDirH(vDir.x, vDir.z);

		if (!m_DelayedStop.bValid)
		{
			m_DelayedStop.dwTimeStamp	= a_GetTime();
			m_DelayedStop.fTime			= m_fMoveTime;
		}

		m_DelayedStop.bValid	= true;
		m_DelayedStop.byDir		= byDir;
		m_DelayedStop.vPos		= vPos;
		m_DelayedStop.iMoveMode	= iMoveMode;
		m_DelayedStop.fSpeed	= fSpeed;
	}

	m_fMoveTime = 0.0f;
}

//	Send stop move command using current position and default speed and move mode
void CECHostMove::SendStopMoveCmd()
{
	int iMoveMode = GP_MOVE_RUN;
	float fSpeed;
	switch (m_pHost->GetMoveEnv())
	{
	case CECPlayer::MOVEENV_AIR:
		
		iMoveMode |= GP_MOVE_AIR;
		fSpeed	   = m_pHost->GetFlySpeed();	
		break;

	case CECPlayer::MOVEENV_WATER:
		
		iMoveMode |= GP_MOVE_WATER;
		fSpeed	   = m_pHost->GetSwimSpeedSev();
		break;

	default:
		
		fSpeed = m_pHost->GetGroundSpeed();
		break;
	}

	SendStopMoveCmd(m_pHost->GetPos(), fSpeed, iMoveMode);
}

//	Ground move
A3DVECTOR3 CECHostMove::GroundMove(const A3DVECTOR3& vDirH, float fSpeedH, float fTime, 
							 float fSpeedV/* 0.0f */, float fGravity/* EC_GRAVITY */)
{
	A3DVECTOR3 vRealDirH = vDirH;

	if (fabs(vRealDirH.y) > 0.0001)
	{
		vRealDirH.y = 0.0f;
		vRealDirH.Normalize();
	}

	//	OnGroundMove only accept positive speed value
	if (fSpeedH < 0.0f)
	{
		vRealDirH = -vDirH;
		fSpeedH = -fSpeedH;
	}

	int idInst = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
	CECInstance * pInstance = g_pGame->GetGameRun()->GetInstance(idInst);
	if (pInstance->GetLimitJump())
		fGravity *= 4.0f;
	
	CDR_INFO& cdr = m_pHost->m_CDRInfo;

	cdr.vCenter			= m_pHost->m_aabbServer.Center;
	cdr.vXOZVelDir		= vRealDirH;
	cdr.fSpeed			= fSpeedH;
	cdr.t				= fTime;
	cdr.fGravityAccel	= fGravity;
	cdr.fYVel		   += fSpeedV;

	OnGroundMove(cdr);

	if( g_pGame->GetGameRun()->GetWorld()->GetAssureMove() )
		g_pGame->GetGameRun()->GetWorld()->GetAssureMove()->AssureMove(m_pHost->m_aabbServer.Center, cdr.vCenter);

	if( !cdr.vTPNormal.IsZero() )
		m_pHost->SetGroundNormal(cdr.vTPNormal);
	else
		m_pHost->SetGroundNormal(g_vAxisY);
	
	A3DVECTOR3 vNewPos = cdr.vCenter - g_vAxisY * m_pHost->m_aabbServer.Extents.y;

	m_iBlockedCnt = 0;
	m_fBlockMove += (vNewPos - m_pHost->GetPos()).Magnitude();
	m_vBlockMove += vNewPos - m_pHost->GetPos();
	if ((m_fBlockTime += fTime) >= 1.0f)
	{
		if (m_fBlockMove < MIN_MOVELEN_ON_GROUND || m_vBlockMove.Magnitude() < MIN_MOVELEN_FOR_DETECT_VIBRATION)
		{
			m_iBlockedCnt = 5;
		}

		m_fBlockTime = 0.0f;
		m_fBlockMove = 0.0f;
		m_vBlockMove.Clear();
	}

	m_fMoveTime += fTime;

	return vNewPos;
}

//	Air/Water move
A3DVECTOR3 CECHostMove::AirWaterMove(const A3DVECTOR3& vDir, float fSpeed, float fTime, 
									 bool bInAir, bool bTrace/* false */)
{
	A3DVECTOR3 vRealDir = vDir;

	//	OnAirMove only accept positive speed value
	if (fSpeed < 0.0f)
	{
		vRealDir = -vDir;
		fSpeed = -fSpeed;
	}

	float fMaxSpeed = bInAir ? m_pHost->GetFlySpeed() : m_pHost->GetSwimSpeedSev();
	if (fSpeed > fMaxSpeed)
		fSpeed = fMaxSpeed;

	ON_AIR_CDR_INFO& cdr = m_pHost->m_AirCDRInfo;

	cdr.vCenter			= m_pHost->m_aabbServer.Center;
	cdr.vVelDir			= vRealDir;
	cdr.fSpeed			= fSpeed;
	cdr.fHeightThresh	= bInAir ? m_pHost->m_MoveConst.fMinAirHei : m_pHost->m_MoveConst.fMinWaterHei;
	cdr.t				= fTime;
	cdr.bOnAir			= bInAir;

	//	If player is tracing something, let he fly/swim low enough
//	if (bTrace)
//		cdr.fHeightThresh = 0.2f;

	OnAirMove(cdr);

	if( g_pGame->GetGameRun()->GetWorld()->GetAssureMove() )
		g_pGame->GetGameRun()->GetWorld()->GetAssureMove()->NoAssureMove();

	m_pHost->SetGroundNormal(g_vAxisY);

  	A3DVECTOR3 vNewPos = cdr.vCenter - g_vAxisY * m_pHost->m_aabbServer.Extents.y;

	m_iBlockedCnt = 0;
	m_fBlockMove += (vNewPos - m_pHost->GetPos()).Magnitude();
	m_vBlockMove += vNewPos - m_pHost->GetPos();
	if ((m_fBlockTime += fTime) >= 1.0f)
	{
		if (m_fBlockMove < MIN_MOVELEN_IN_AIR_WATER || m_vBlockMove.Magnitude() < MIN_MOVELEN_FOR_DETECT_VIBRATION)
		{
			m_iBlockedCnt = 5;
		}

		m_fBlockTime = 0.0f;
		m_fBlockMove = 0.0f;
		m_vBlockMove.Clear();
	}

	m_fMoveTime += fTime;

	return vNewPos;
}

//	Check whether host meet a slope
//	vMoveDirH: normalized horizontal moving direction
//	fMaxSpeed (out): maximum vertical speed
bool CECHostMove::MeetSlope(const A3DVECTOR3& vMoveDirH, float& fMaxSpeedV)
{
	A3DVECTOR3 vTangent = m_pHost->m_GndInfo.vGndNormal;

	float d = vTangent.MagnitudeH();
	float tan = d / (float)fabs(vTangent.y);
	float max = m_pHost->GetGroundSpeed() * tan * 0.96f;
	//	Prevent max is too small, tan60 = 1.732
	a_ClampFloor(max, m_pHost->GetGroundSpeed() * 1.732f);
	fMaxSpeedV = a_Min(max, 19.5f);

//	fMaxSpeedV = 100.0f;

	vTangent.y = 0.0f;
	vTangent.Normalize();

	if (DotProduct(vMoveDirH, vTangent) <= -0.85f)
		return true;

	return false;
}

//	Flash move
A3DVECTOR3 CECHostMove::FlashMove(const A3DVECTOR3& vDir, float fSpeed, float fDist)
{
	//	Copy cdr data from host
	CDR_INFO cdr = m_pHost->m_CDRInfo;

	A3DVECTOR3 vStartPos = m_vLastSevPos + g_vAxisY * cdr.vExtent.y;
	if (!fDist || !fSpeed)
		return m_vLastSevPos;

	//	Get horizonal directional
	A3DVECTOR3 vRealDirH = vDir;
	if (vRealDirH.y)
	{
		vRealDirH.y = 0.0f;
		vRealDirH.Normalize();
	}

	static int iNumSeg = 6;
	static float aFactors[] = {0.3f, 0.3f, 0.15f, 0.1f, 0.1f, 0.5f};
	
	float fTotalTime = fDist / fSpeed;
	A3DVECTOR3 vCurPos = vStartPos;
	A3DVECTOR3 vCurTPNormal = cdr.vTPNormal;

	cdr.vCenter	= vStartPos;
	cdr.fYVel	= 0.0f;

	for (int i=0; i < iNumSeg; i++)
	{
		cdr.vXOZVelDir		= vRealDirH;
		cdr.fSpeed			= fSpeed;
		cdr.t				= fTotalTime * aFactors[i];
		cdr.fGravityAccel	= EC_GRAVITY;

		OnGroundMove(cdr);

		if( g_pGame->GetGameRun()->GetWorld()->GetAssureMove() )
			g_pGame->GetGameRun()->GetWorld()->GetAssureMove()->NoAssureMove();

		if (a3d_Magnitude(cdr.vCenter - vStartPos) >= fDist * 0.98f)
		{
			//	Use the result of last time
			cdr.vCenter		= vCurPos;
			cdr.vTPNormal	= vCurTPNormal;
			break;
		}

		if (a3d_Magnitude(cdr.vCenter - vCurPos) < 0.3f)
			break;

		//	Record result of this time
		vCurPos	= cdr.vCenter;
		vCurTPNormal = cdr.vTPNormal;
	}

	//	test code
//	float ff = a3d_Magnitude(cdr.vCenter - vStartPos);

	//	Calculate new position
	vCurPos = cdr.vCenter - g_vAxisY * cdr.vExtent.y;

	m_vFlashTPNormal = cdr.vTPNormal;

	return vCurPos;
}

/*
 * FILE: EC_HPWorkFall.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_HPWorkFall.h"
#include "EC_Inventory.h"
#include "EC_HostPlayer.h"
#include "EC_Utility.h"
#include "EC_ActionSwitcher.h"

#include "A3DFuncs.h"

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

enum FlyFallStage
{
	enumStageNone = 0,
	enumStageUpperAir,
	enumStageLowerAir,
	enumStageShallowWater,
	enumStageDeepWater,
	enumStageLandOn
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkFall
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkFall::CECHPWorkFall(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_FREEFALL, pWorkMan)
{
	m_dwMask		= MASK_FREEFALL;
	m_dwTransMask	= MASK_STAND;

	CECHPWorkFall::Reset();
}

CECHPWorkFall::~CECHPWorkFall()
{
}

//	Reset work
void CECHPWorkFall::Reset()
{
	CECHPWork::Reset();

	m_bEnterWater	= false;
	m_nCurStage		= enumStageNone;
	m_iFallType		= TYPE_FREEFALL;
	m_fForceDown	= false;
}

//	Copy work data
bool CECHPWorkFall::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkFall* pSrc = (CECHPWorkFall*)pWork;

	m_bEnterWater	= pSrc->m_bEnterWater;
	m_nCurStage		= pSrc->m_nCurStage;
	m_iFallType		= pSrc->m_iFallType;
	m_fForceDown	= pSrc->m_fForceDown;

	return true;
}

//	On first tick
void CECHPWorkFall::OnFirstTick()
{
	m_pHost->m_iMoveEnv	 = CECPlayer::MOVEENV_GROUND;
	m_pHost->m_iMoveMode = CECPlayer::MOVE_FREEFALL;

	m_vDirH = m_pHost->m_vVelocity;
	m_vDirH.y = 0.0f;
	m_fSpeedH = m_vDirH.Normalize();

	if (m_fSpeedH > m_pHost->GetGroundSpeed())
		m_fSpeedH = m_pHost->GetGroundSpeed();

	//	Clear N
	m_pHost->m_CDRInfo.vTPNormal.Clear();
}

//	Work is cancel
void CECHPWorkFall::Cancel()
{
	m_pHost->m_CDRInfo.vAbsVelocity.Clear();
	if (m_pHost->m_CDRInfo.vTPNormal.IsZero())
		m_pHost->m_CDRInfo.fYVel = 0.0f;
	CECHPWork::Cancel();
}

//	Tick routine
bool CECHPWorkFall::Tick(DWORD dwDeltaTime)
{
	CECHPWork::Tick(dwDeltaTime);

	if (!m_pHost->m_pEquipPack->GetItem(EQUIPIVTR_FLYSWORD))
		m_iFallMode = GP_MOVE_FALL;
	else
		m_iFallMode = GP_MOVE_FLYFALL;

	if (m_pHost->IsDead())
		m_iFallMode |= GP_MOVE_DEAD;

	if (!m_pHost->IsRooting())
	{
		float fDeltaTime = dwDeltaTime * 0.001f;

		if (m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_GROUND || 
			m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_AIR)
		{
			if (m_iFallType == TYPE_FLYFALL)
				Fall_Air(fDeltaTime);
			else
				FreeFall_Air(fDeltaTime);
		}
		else	//	m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_WATER
		{
			if (m_iFallType == TYPE_FLYFALL)
				Fall_Water(fDeltaTime);
			else
				FreeFall_Water(fDeltaTime);
		}
	}

	return true;
}

//	Free fall in air
bool CECHPWorkFall::Fall_Air(float fDeltaTime)
{
	A3DVECTOR3 vCurPos = m_pHost->GetPos();
//	A3DVECTOR3 vCurVel = m_pHost->m_vVelocity;
	CDR_INFO& cdr = m_pHost->m_CDRInfo;

	bool bWorkEnd = false;

	if (m_pHost->m_GndInfo.bOnGround)
	{
		bWorkEnd = true;
		//if (m_pHost->UsingWing())
		if (m_pHost->GetWingType() == WINGTYPE_WING)
			m_pHost->PlayAction(CECPlayer::ACT_LANDON, false);
		else
			m_pHost->PlayAction(CECPlayer::ACT_LANDON_SWORD, false);
	}
	else
	{
		vCurPos = m_pHost->m_MoveCtrl.GroundMove(m_vDirH, m_fSpeedH, fDeltaTime);

		if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
		{
			bWorkEnd = true;
			//if (m_pHost->UsingWing())
			if (m_pHost->GetWingType() == WINGTYPE_WING)
				m_pHost->PlayAction(CECPlayer::ACT_LANDON, false);
			else
				m_pHost->PlayAction(CECPlayer::ACT_LANDON_SWORD, false);
		}
		else
		{
			float fHei = vCurPos.y - m_pHost->m_GndInfo.fGndHei;

			if (fHei > 25.f)
			{
				if (m_nCurStage == enumStageNone)
				{
					m_nCurStage = enumStageUpperAir;
					//if (m_pHost->UsingWing())
					if (m_pHost->GetWingType() == WINGTYPE_WING)
						m_pHost->PlayAction(CECPlayer::ACT_FLYDOWN, false);
					else
						m_pHost->PlayAction(CECPlayer::ACT_FLYDOWN_SWORD_HIGH, false);
				}
			}
			else if (fHei > 20.f)
			{
				if (m_nCurStage == enumStageNone)
				{
					m_nCurStage = enumStageLowerAir;
					//if (m_pHost->UsingWing())
					if (m_pHost->GetWingType() == WINGTYPE_WING)
						m_pHost->PlayAction(CECPlayer::ACT_FLYDOWN_WING_LOW, false);
					else
						m_pHost->PlayAction(CECPlayer::ACT_FLYDOWN_SWORD_LOW, false);
				}
			}
			else if (fHei > 3.0f)
			{
				if (m_nCurStage == enumStageNone
				 || m_nCurStage == enumStageUpperAir)
				{
					m_nCurStage = enumStageLowerAir;
					//if (m_pHost->UsingWing())
					if (m_pHost->GetWingType() == WINGTYPE_WING)
						m_pHost->PlayAction(CECPlayer::ACT_FLYDOWN_WING_LOW, false);
					else
						m_pHost->PlayAction(CECPlayer::ACT_FLYDOWN_SWORD_LOW, false);
				}
			}
			else
			{
				if (m_nCurStage != enumStageLandOn)
				{
					m_nCurStage = enumStageLandOn;
					//if (m_pHost->UsingWing())
					if (m_pHost->GetWingType() == WINGTYPE_WING)
						m_pHost->PlayAction(CECPlayer::ACT_LANDON, false);
					else
						m_pHost->PlayAction(CECPlayer::ACT_LANDON_SWORD, false);
				}
			}
		}

		m_pHost->SetPos(vCurPos);
	}

	if (bWorkEnd)
	{
		Finish();
		m_pHost->SetPos(vCurPos);

		float fSpeed = cdr.vAbsVelocity.Magnitude();
		if (!fSpeed)
			fSpeed = m_pHost->GetFlySpeed();

		m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed, m_iFallMode);
		m_pHost->m_vVelocity.Clear();
		return true;
	}
	else
	{
		m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 2, g_vOrigin, cdr.vAbsVelocity, m_iFallMode);
	}

	return true;
}

//	Free fall in water
bool CECHPWorkFall::Fall_Water(float fDeltaTime)
{
	A3DVECTOR3 vCurPos = m_pHost->GetPos();
	A3DVECTOR3 vCurVel = m_pHost->m_vVelocity;
	float fGndHei = m_pHost->m_GndInfo.fGndHei;

	if (!m_bEnterWater)
	{
		m_bEnterWater = true;
		vCurVel = m_pHost->m_CDRInfo.vAbsVelocity;
		float fWaterHei = m_pHost->m_GndInfo.fWaterHei;
		if (fWaterHei - fGndHei > 1.5f)
			m_nCurStage = enumStageDeepWater;
		else
			m_nCurStage = enumStageShallowWater;
	}

	float fSpeed1 = vCurVel.Normalize();
	float na = -fSpeed1 * 0.7f + EC_NACCE_WATER * 4.0f;
	float fSpeed2 = fSpeed1 + na * fDeltaTime;
	if (fSpeed2 * fSpeed1 < 0.0f)
		fSpeed2 = 0.0f;

	if (m_fForceDown)
	{
		//	Note: When speed=0 we should check whether a special case: EstimateMoveEnv find
		//		we are in water but we stop at a underwater position which is very close
		//		to water surface.
	//	if (vCurPos.y < m_pHost->m_GndInfo.fWaterHei && 
	//		m_pHost->m_GndInfo.fWaterHei - m_pHost->m_GndInfo.fGndHei > m_pHost->m_MoveConst.fShoreDepth &&
		if (m_pHost->CheckWaterMoveEnv(vCurPos, m_pHost->m_GndInfo.fWaterHei, m_pHost->m_GndInfo.fGndHei) &&
			m_pHost->m_GndInfo.fWaterHei - vCurPos.y < m_pHost->m_MoveConst.fShoreDepth)
		{
			vCurPos = m_pHost->m_MoveCtrl.GroundMove(m_vDirH, m_fSpeedH, fDeltaTime, -5.0f);
			m_pHost->SetPos(vCurPos);

			if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
			{
				//	Fall on ground
				Finish();
				m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, m_pHost->m_vVelocity.Magnitude(), GP_MOVE_WATER | m_iFallMode);
				m_pHost->m_vVelocity.Clear();
			}

			return true;
		}
	}

	if (fSpeed2)
	{
		A3DVECTOR3 vOldPos = vCurPos;
		vCurPos = m_pHost->m_MoveCtrl.AirWaterMove(vCurVel, fSpeed2, fDeltaTime, false);
		vCurVel *= fSpeed2;

		if (m_nCurStage == enumStageShallowWater)
		{
			if (vCurPos.y - fGndHei < 1.0f)
			{
				//if (m_pHost->UsingWing())
				if (m_pHost->GetWingType() == WINGTYPE_WING)
					m_pHost->PlayAction(CECPlayer::ACT_LANDON, false);
				else
					m_pHost->PlayAction(CECPlayer::ACT_LANDON_SWORD, false);

				m_nCurStage = enumStageLandOn;
			}
		}
		else if (m_nCurStage == enumStageDeepWater)
		{
			if (vCurVel.y > -9.0f)
				m_pHost->PlayAction(CECPlayer::ACT_HANGINWATER, false);
		}

		if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3 ||
			vCurPos.y - m_pHost->m_MoveConst.fMinWaterHei <= fGndHei)
		{
			//	Fall on ground
			Finish();

			m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, m_pHost->m_vVelocity.Magnitude(), GP_MOVE_WATER | m_iFallMode);

			m_pHost->SetPos(vCurPos);
			m_pHost->m_vVelocity.Clear();
		}
		else
		{
			m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 2, g_vOrigin, vCurVel, GP_MOVE_WATER | m_iFallMode);

			m_pHost->SetPos(vCurPos);
			m_pHost->m_vVelocity = vCurVel;
		}

		//	If moving too small
		if (a3d_Magnitude(vCurPos - vOldPos) < 0.001f)
			m_fForceDown = true;
	}
	else
	{
		Finish();
		m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed1, GP_MOVE_WATER | m_iFallMode);

		m_pHost->SetPos(vCurPos);
		m_pHost->m_vVelocity.Clear();
	}

	return true;
}

//	Free fall in air
bool CECHPWorkFall::FreeFall_Air(float fDeltaTime)
{
	A3DVECTOR3 vCurPos = m_pHost->GetPos();
//	A3DVECTOR3 vCurVel = m_pHost->m_vVelocity;
	CDR_INFO& cdr = m_pHost->m_CDRInfo;

	if (m_pHost->m_GndInfo.bOnGround)
	{
		Finish();
		m_pHost->SetPos(vCurPos);

		float fSpeed = cdr.vAbsVelocity.Magnitude();
		if (!fSpeed)
			fSpeed = m_pHost->GetFlySpeed();

		m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed, m_iFallMode);
		m_pHost->m_vVelocity.Clear();
	}
	else
	{
		vCurPos = m_pHost->m_MoveCtrl.GroundMove(m_vDirH, m_fSpeedH, fDeltaTime);
		
		//	If player is blocked, add disturb speed at random direction
		if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
		{
			cdr.fYVel = 0.0f;
			
			Finish();

			m_pHost->SetPos(vCurPos);
			m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, 5.0f, m_iFallMode);
		}
	/*	{
			m_fSpeedH = 3.0f;
			m_vDirH = glb_RandomVectorH();
		}
		else
		{
			m_fSpeedH = 0.0f;
			m_vDirH.Clear();
		}	*/

		m_pHost->SetPos(vCurPos);
		m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 2, g_vOrigin, cdr.vAbsVelocity, m_iFallMode);

		m_pHost->PlayAction(CECPlayer::ACT_JUMP_LOOP, false);
	}

	return true;
}

//	Free fall in water
bool CECHPWorkFall::FreeFall_Water(float fDeltaTime)
{
	A3DVECTOR3 vCurPos = m_pHost->GetPos();
	A3DVECTOR3 vCurVel = m_pHost->m_vVelocity;
	float fGndHei = m_pHost->m_GndInfo.fGndHei;

	if (!m_bEnterWater)
	{
		m_bEnterWater = true;
		vCurVel = m_pHost->m_CDRInfo.vAbsVelocity;
		float fWaterHei = m_pHost->m_GndInfo.fWaterHei;
		if (fWaterHei - fGndHei > 1.5f)
			m_nCurStage = enumStageDeepWater;
		else
			m_nCurStage = enumStageShallowWater;
	}

	float fSpeed1 = vCurVel.Normalize();
	float na = EC_NACCE_WATER * 4.0f;
	float fSpeed2 = fSpeed1 + na * fDeltaTime;
	if (fSpeed2 * fSpeed1 < 0.0f)
		fSpeed2 = 0.0f;

	if (fSpeed2)
	{
		vCurPos = m_pHost->m_MoveCtrl.AirWaterMove(vCurVel, fSpeed2, fDeltaTime, false);
		vCurVel *= fSpeed2;

		if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3 ||
			vCurPos.y - m_pHost->m_MoveConst.fMinWaterHei <= fGndHei)
		{
			//	Fall on ground
			Finish();

			m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, m_pHost->m_vVelocity.Magnitude(), GP_MOVE_WATER | m_iFallMode);

			m_pHost->SetPos(vCurPos);
			m_pHost->m_vVelocity.Clear();
		}
		else
		{
			m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 2, g_vOrigin, vCurVel, GP_MOVE_WATER | m_iFallMode);
			m_pHost->SetPos(vCurPos);
			m_pHost->m_vVelocity = vCurVel;

			if (m_nCurStage == enumStageDeepWater)
				m_pHost->PlayAction(CECPlayer::ACT_HANGINWATER, false);
			else
				m_pHost->PlayAction(CECPlayer::ACT_JUMP_LOOP, false);
		}
	}
	else
	{
		Finish();
		m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed1, GP_MOVE_WATER | m_iFallMode);

		m_pHost->SetPos(vCurPos);
		m_pHost->m_vVelocity.Clear();
	}

	return true;
}

//	Finish
void CECHPWorkFall::Finish()
{
	m_bFinished = true;
	m_pHost->m_CDRInfo.vAbsVelocity.Clear();
//	m_pHost->m_CDRInfo.fYVel = 0.0f;

	if (m_iFallType == TYPE_FLYFALL)	
		m_pHost->ShowWing(false);	
}



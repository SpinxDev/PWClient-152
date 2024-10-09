/*
 * FILE: EC_HPWorkMove.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_HPWorkMove.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Model.h"
#include "A3DGFXEx.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "EC_IntelligentRoute.h"
#include "EC_IntelligentRouteUtil.h"
#include "EC_AutoPolicy.h"
#include "EC_PlayerWrapper.h"
#include "EC_NPC.h"
#include "EC_ManNPC.h"
#include "EC_HPWorkTrace.h"

#include <A3DFuncs.h>

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

static const float pitch_ang_wing = DEG2RAD(45.0f);
static const float pitch_ang_fly_sword = DEG2RAD(25.0f);
static const float lean_max_ang = DEG2RAD(45.0f);
static const float ang_vel_fly = 1.0f / DEG2RAD(60.0f);
static const float ang_vel_swim = 1.0f / DEG2RAD(180.0f);
static const float pitch_co_wing = pitch_ang_wing / A3D_PI;
static const float pitch_co_fly_sword = pitch_ang_fly_sword / A3D_PI;
static const float sidle_co = .5f;
static const float sidle_co_r = 1.0f - sidle_co;
static const float push_pitch_vel_wing = pitch_ang_wing / 0.5f;
static const float push_pitch_vel_fly_sword = pitch_ang_fly_sword / 0.5f;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkMove
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkMove::CECHPWorkMove(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_MOVETOPOS, pWorkMan)
{
	m_dwMask		= MASK_MOVETOPOS;
	m_dwTransMask	= MASK_STAND | MASK_TRACEOBJECT | MASK_FOLLOW;

	CECHPWorkMove::Reset();
}

CECHPWorkMove::~CECHPWorkMove()
{
}

//	Reset work
void CECHPWorkMove::Reset()
{
	CECHPWork::Reset();

	m_iDestType		= DEST_2D;
	m_bHaveDest		= false;
	m_bMeetSlide	= false;
	m_bReadyCancel	= false;
	m_bGliding		= false;
	m_fGlideTime	= 0;
	m_fCurPitch		= 0;
	m_fPushPitch	= 0;
	m_fPushLean		= 0;
	
	m_bUseAutoMoveDialog		= false;
	m_fAutoHeight	= -1.0f; 
	m_bAutoLand		= false;
	m_bAutoFly		= false;
	m_bReachedHeight = true;
	m_bAutoFlyPending = false;

	m_iNPCTempleId	= 0;
	m_iTaskId		= 0;
	m_bSwitchTo2D	= false;

	m_bResetAutoPF	= false;
}

//	Copy work data
bool CECHPWorkMove::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkMove* pSrc = (CECHPWorkMove*)pWork;

	m_iDestType		= pSrc->m_iDestType;
	m_bHaveDest		= pSrc->m_bHaveDest;
	m_bMeetSlide	= pSrc->m_bMeetSlide;
	m_bReadyCancel	= pSrc->m_bReadyCancel;
	m_bGliding		= pSrc->m_bGliding;
	m_fGlideTime	= pSrc->m_fGlideTime;
	m_fGlideSpan	= pSrc->m_fGlideSpan;
	m_fGlideAng		= pSrc->m_fGlideAng;
	m_fGlideVel		= pSrc->m_fGlideVel;	
	m_fGlidePitch	= pSrc->m_fGlidePitch;
	m_fCurPitch		= pSrc->m_fCurPitch;
	m_fPushPitch	= pSrc->m_fPushPitch;
	m_fPushLean		= pSrc->m_fPushLean;
	m_iDestType		= pSrc->m_iDestType;
	m_vMoveDest		= pSrc->m_vMoveDest;
	m_vCurDir		= pSrc->m_vCurDir;

	m_bUseAutoMoveDialog		= pSrc->m_bUseAutoMoveDialog;
	m_fAutoHeight	= pSrc->m_fAutoHeight; 
	m_bAutoLand		= pSrc->m_bAutoLand;
	m_bAutoFly		= pSrc->m_bAutoFly;
	m_bReachedHeight= pSrc->m_bReachedHeight;
	m_bAutoFlyPending = pSrc->m_bAutoFlyPending;

	m_iNPCTempleId	= pSrc->m_iNPCTempleId;
	m_iTaskId		= pSrc->m_iTaskId;
	m_bSwitchTo2D	= pSrc->m_bSwitchTo2D;

	m_bResetAutoPF	= pSrc->m_bResetAutoPF;

	return true;
}

//	On first tick
void CECHPWorkMove::OnFirstTick()
{
	m_pHost->m_iMoveMode = CECPlayer::MOVE_MOVE;
	//PlayMoveTargetGFX();

	if (m_pHost->m_iMoveEnv != CECPlayer::MOVEENV_AIR)
		m_pHost->ShowWing(false);

	if (!m_pHost->IsJumping())
	{
		int iAction = m_pHost->GetMoveStandAction(true);
		m_pHost->PlayAction(iAction, false);
	}
}

#ifdef SHOW_AUTOMOVE_FOOTPRINTS
extern abase::vector<A3DVECTOR3> g_AutoPFFollowPoints;
extern abase::vector<A3DVECTOR3> g_AutoPFPathPoints;
#endif

void CECHPWorkMove::ResetUseAutoPF(){
	m_bResetAutoPF = true;
}

void CECHPWorkMove::ClearResetUseAutoPF(){
	m_bResetAutoPF = false;
}

void CECHPWorkMove::UpdateResetUseAutoPF(){
	if (!m_bResetAutoPF){
		return;
	}
	CECIntelligentRoute::Instance().SetUsage(CECIntelligentRoute::enumUsageWorkMove);
	CECIntelligentRoute::Instance().ResetSearch();
	if (m_iDestType == DEST_2D || m_iDestType == DEST_3D)
	{
#ifdef SHOW_AUTOMOVE_FOOTPRINTS
		g_AutoPFFollowPoints.clear();
		g_AutoPFPathPoints.clear();
#endif
	}else if (IsAutoPF()){
		bool bSwitchTo2D(true);
		while (true){
			if (m_pHost->IsFlying()){
				break;
			}
			CECHostBrushTest brushTester(m_pHost->GetPos(), m_pHost->m_CDRInfo.vExtent, m_pHost->m_CDRInfo.fStepHeight);
			if (CECIntelligentRoute::Instance().Search(m_pHost->GetPos(), m_vMoveDest, &brushTester) != CECIntelligentRoute::enumSearchSuccess){
				break;
			}
			bSwitchTo2D = false;
			break;
		}
		if (bSwitchTo2D){
			m_bSwitchTo2D = true;
		}
	}
	ClearResetUseAutoPF();
}

//	Set destination position or direction
void CECHPWorkMove::SetDestination(int iDestType, const A3DVECTOR3& vMoveDest)
{
	m_iDestType	= iDestType;
	m_vMoveDest	= vMoveDest;
	m_bHaveDest	= true;
	m_bGliding	= false;
	//m_pHost->SetAdjustOrient(false);	//	2014-9-10 徐文彬：在 CECHPWorkMove 创建的时候都会调用此函数，会导致其它时机通过 SetDestDirAndUp 调用对人物的朝向调整失效。
										//	重现方法为：月仙“望月咏”技能释放后、马上按住'A'或'D'等移动键并摁住不放，将导致人物施法方向偏离目标而朝向移动方向（需多次尝试）

	//清空任务信息
	m_iTaskId = 0;
	m_iNPCTempleId = 0;
	ResetUseAutoPF();

	if (iDestType == DEST_DIR)
	{
		m_vCurDir = vMoveDest;
		if(m_bUseAutoMoveDialog)
		{
			m_bUseAutoMoveDialog = false;
			m_bAutoLand = false;
			m_fAutoHeight	= -1.0f; 
			m_bAutoFly		= false;
			m_bReachedHeight = true;
			m_bAutoFlyPending = false;
		}
	}
	else if (iDestType == DEST_2D || iDestType == DEST_3D)
	{
		m_vCurDir = vMoveDest - m_pHost->GetPos();
		m_vCurDir.y = 0.0f;
		m_vCurDir.Normalize();
	}else if (IsAutoPF()){
		m_vCurDir = CECIntelligentRoute::Instance().GetCurDest() - m_pHost->GetPos();
		m_vCurDir.y = 0.0f;
		m_vCurDir.Normalize();
		if(m_bUseAutoMoveDialog){
			//	此处禁用 m_bUseAutoMoveDialog，见 SetUseAutoMoveDialog 中说明
			m_bUseAutoMoveDialog = false;
			m_bAutoLand = false;
			m_fAutoHeight	= -1.0f; 
			m_bAutoFly		= false;
			m_bReachedHeight = true;
			m_bAutoFlyPending = false;
		}
	}

	if (m_pHost->m_pMoveTargetGFX)
	{
		if (iDestType != DEST_PUSH)
			m_pHost->m_pMoveTargetGFX->Stop();
	}
}

//	Play walk target effect
void CECHPWorkMove::PlayMoveTargetGFX(const A3DVECTOR3& vPos, const A3DVECTOR3& vNormal)
{
	/*
	if (!m_bHaveDest || m_iDestType != DEST_2D || !m_pHost->m_pMoveTargetGFX)
		return;
	
	A3DVECTOR3 vTestPos = m_vMoveDest;
	vTestPos.y += 0.2f;

	A3DVECTOR3 vNormal, vHitPos;
	if (!VertRayTrace(vTestPos, vHitPos, vNormal, 5.0f))
		vNormal = g_vAxisY;
	*/

	A3DVECTOR3 vRight;
	vRight.CrossProduct(vNormal, g_vAxisX);
	vRight.Normalize();

	A3DMATRIX4 mat = a3d_TransformMatrix(vRight, vNormal, vPos + vNormal * 0.2f);

	if (m_pHost->m_pMoveTargetGFX)
	{
		m_pHost->m_pMoveTargetGFX->SetParentTM(mat);
		m_pHost->m_pMoveTargetGFX->Start(true);
	}
}

//	Tick routine
bool CECHPWorkMove::Tick(DWORD dwDeltaTime)
{
	UpdateResetUseAutoPF();
	if (m_bSwitchTo2D)
	{
		SwitchToDest2D();
		m_bSwitchTo2D = false;
		return true;
	}
	if (IsAutoPF()){
		if (CECIntelligentRoute::Instance().IsIdle()){
			//	智能寻路模式未成功时，等待下个 Tick 切换到 DEST_2D 模式
			return true;
		}
		if (m_pHost->IsFlying()){
			//	中途切换到飞行模式时，切换到 DEST_2D 模式
			CECIntelligentRoute::Instance().ResetSearch();
			m_bSwitchTo2D = true;
			return true;
		}
	}

	CECHPWork::Tick(dwDeltaTime);

	if (m_pHost->IsRooting())
		return true;

	if(m_bUseAutoMoveDialog)
	{
		if(m_pHost->IsFlying())
		{
			m_bAutoFly = false;
			m_bAutoFlyPending = false;
		}

		if(m_bAutoFly && !m_bAutoFlyPending && !m_pHost->IsFlying())
		{
			if(m_pHost->CmdFly())
			{
				m_bAutoFly = false;
				m_bAutoFlyPending = true;
			}
		}	
	}
	else
	{
		// Make sure 'Win_AutoPlay' dialog doesn't show up
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->AutoMoveShowDialog(false);
	}
	
	//寻路过程中找到NPC对象转为WorkTrace状态
	if((m_vMoveDest - m_pHost->GetPos()).MagnitudeH() <= 5.0f){
		if (m_iNPCTempleId){
			CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->FindNPCByTempleID(m_iNPCTempleId);
			if (pNPC && m_pHost->SelectTarget(pNPC->GetNPCID())){
				CECHPWorkTrace *pWork = m_pWorkMan->CreateNPCTraceWork(pNPC, m_iTaskId);
				if (pWork){
					m_bAutoLand = false;	//防止飞行状态寻路结束，转到worktrace之前自动着陆，进入workfall。
					Finish();
					m_pWorkMan->SetPostTickCommand(new CECHPWorkPostTickRunWorkCommand(pWork, true));
					return true;
				}
			}
		}
	}

	float fDeltaTime = dwDeltaTime * 0.001f;
	if (m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_GROUND ||
		m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_WATER && m_pHost->IsJumping() && (m_pHost->m_CDRInfo.vAbsVelocity.y > 0 || m_pHost->m_CDRInfo.fYVel > 0))
	{
		//	Play appropriate actions
		if (!m_pHost->IsJumping() && !m_pHost->IsPlayingAction(CECPlayer::ACT_TRICK_RUN) &&
			m_pHost->m_iMoveMode != CECPlayer::MOVE_SLIDE && !m_bMeetSlide)
		{
			int iAction = m_pHost->GetMoveStandAction(true);
			m_pHost->PlayAction(iAction, false);
		}

		Tick_Walk(fDeltaTime);
	}
	else	//	(m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_AIR || m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_WATER)
	{
		m_pHost->ResetJump();

		//	Play appropriate actions
		if (!m_bGliding)
		{
			int iAction = m_pHost->GetMoveStandAction(true);
			m_pHost->PlayAction(iAction, false);
		}

		Tick_FlySwim(fDeltaTime);
	}

	return true;
}

//	Tick routine of walking on ground
bool CECHPWorkMove::Tick_Walk(float fDeltaTime)
{
	A3DVECTOR3 vCurPos = m_pHost->GetPos();
	CDR_INFO& cdr = m_pHost->m_CDRInfo;

	if (m_pHost->m_iMoveMode == CECPlayer::MOVE_SLIDE)
	{
		m_pHost->PlayAction(CECPlayer::ACT_JUMP_LOOP, false);

		//	This will cause stop moving after we slide down.
		A3DVECTOR3 vDir;
		if (m_iDestType == DEST_DIR)
			vDir = m_vCurDir;
		else if (m_iDestType == DEST_PUSH)
			vDir = m_pHost->GetModelMoveDir();
		else if (IsAutoPF()){
			vDir = CECIntelligentRoute::Instance().GetCurDest() - vCurPos;
			vDir.y = 0;
			vDir.Normalize();
		}
		else
		{
			vDir = m_vMoveDest - vCurPos;
			vDir.y = 0;
			vDir.Normalize();
		}

		float fMaxSpeedV;
		m_bMeetSlide = m_pHost->m_MoveCtrl.MeetSlope(vDir, fMaxSpeedV);
		a_ClampFloor(cdr.fYVel, -fMaxSpeedV);

		if (m_pHost->m_GndInfo.bOnGround)
			m_vCurDir = vDir;

		//	Ground move
		vCurPos = m_pHost->m_MoveCtrl.GroundMove(m_vCurDir, m_pHost->GetGroundSpeed(), fDeltaTime);
		if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
		{
			m_pHost->m_MoveCtrl.SetSlideLock(true);

			//	blocked, so we clear the fYVel of cdr!
			cdr.fYVel = 0.0f;

			Finish();
			m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, m_pHost->GetGroundSpeed(), GP_MOVE_SLIDE);
		}		
		else
		{
			m_pHost->SetPos(vCurPos);
			if (IsAutoPF() && CECIntelligentRoute::Instance().IsMoveOn()){
				CECIntelligentRoute::Instance().OnPlayerPosChange(vCurPos);
			}
#ifdef SHOW_AUTOMOVE_FOOTPRINTS
			if (IsAutoPF() || DEST_2D == m_iDestType){
				g_AutoPFFollowPoints.push_back(vCurPos);
			}
#endif

			m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 2, g_vOrigin, cdr.vAbsVelocity, GP_MOVE_SLIDE);
		}
	}
	else if (/*m_pHost->m_iMoveMode == CECPlayer::MOVE_FREEFALL || */!m_bMeetSlide)
	{
		float fSpeed = m_pHost->GetGroundSpeed();

		int iMoveMode = m_pHost->m_bWalkRun ? GP_MOVE_RUN : GP_MOVE_WALK;
		if (m_pHost->IsJumping())
			iMoveMode = GP_MOVE_JUMP;
		else if (!m_pHost->m_GndInfo.bOnGround)
			iMoveMode = GP_MOVE_FALL;

		if (m_bReadyCancel && m_pHost->m_GndInfo.bOnGround)
		{
			Finish();
			m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed, iMoveMode);
			return true;
		}

		if (m_pHost->m_dwMoveRelDir & (CECHostPlayer::MD_LEFT | CECHostPlayer::MD_RIGHT | CECHostPlayer::MD_FORWARD | CECHostPlayer::MD_BACK))
			m_iDestType = DEST_PUSH;

		if (m_iDestType == DEST_2D)
		{
			float fDist;
			if (m_pHost->m_GndInfo.bOnGround)
			{
				//	Adjust direction only when player on ground
				m_vCurDir = m_vMoveDest - vCurPos;
				m_vCurDir.y = 0.0f;
				fDist = m_vCurDir.Normalize();
			}
			else
				fDist = a3d_MagnitudeH(m_vMoveDest-vCurPos);

			//	Ground move
			vCurPos = m_pHost->m_MoveCtrl.GroundMove(m_vCurDir, fSpeed, fDeltaTime, m_pHost->m_fVertSpeed);
			if (!m_vCurDir.IsZero())
				m_pHost->StartModelMove(m_vCurDir, g_vAxisY, 0);

			//	test code...
		/*	float fTestDist = a3d_MagnitudeH(vCurPos - m_pHost->GetPos());
			float fTestSpeed = fTestDist / fDeltaTime;
			if (fTestDist > 5.0f)
			{
				int aaa = 0;
			}
		*/
			if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
			{
				// blocked, so we clear the fYVel of cdr!
				cdr.fYVel = 0.0f;

				Finish();
				m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed, iMoveMode);
			}
		//	else if (m_pHost->m_GndInfo.bOnGround)
			else if (!cdr.vTPNormal.IsZero())
			{
				//	Check whether we have arrived destination
				A3DVECTOR3 vMoveDelta = vCurPos - m_pHost->GetPos();
				vMoveDelta.y = 0.0f;
				float fMoveDist = vMoveDelta.Normalize();

				if (fMoveDist >= fDist)
				{
					Finish();
					m_bUseAutoMoveDialog = false;
					m_pHost->SetPos(vCurPos);
					m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed, iMoveMode);
				}
				else
				{
					m_pHost->SetPos(vCurPos);
					m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 0, m_vMoveDest, cdr.vAbsVelocity, iMoveMode);
				}
#ifdef SHOW_AUTOMOVE_FOOTPRINTS
				g_AutoPFFollowPoints.push_back(vCurPos);
#endif
			}
			else
			{
#ifdef SHOW_AUTOMOVE_FOOTPRINTS
				g_AutoPFFollowPoints.push_back(vCurPos);
#endif
				m_pHost->SetPos(vCurPos);
				m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 1, m_vMoveDest, cdr.vAbsVelocity, iMoveMode);
			}
		}
		else if (m_iDestType == DEST_DIR)
		{
			vCurPos = m_pHost->m_MoveCtrl.GroundMove(m_vCurDir, fSpeed, fDeltaTime, m_pHost->m_fVertSpeed);
			m_pHost->SetPos(vCurPos);
			if (!m_vCurDir.IsZero())
				m_pHost->StartModelMove(m_vCurDir, g_vAxisY, 0);

			if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
			{
				// blocked, so we clear the fYVel of cdr!
				cdr.fYVel = 0.0f;

				Finish();
				m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed, iMoveMode);
			}
			else if (m_pHost->m_GndInfo.bOnGround)
			//	m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 2, g_vOrigin, m_vCurDir * fSpeed, GP_MOVE_RUN);
				m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 2, g_vOrigin, cdr.vAbsVelocity, iMoveMode);
			else
				m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 1, m_vMoveDest, cdr.vAbsVelocity, iMoveMode);
		}
		else if (m_iDestType == DEST_STANDJUMP)
		{
			if (!m_pHost->IsJumping())
			{
				Finish();
				m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed, iMoveMode);
			}
			else
			{
				vCurPos = m_pHost->m_MoveCtrl.GroundMove(g_vOrigin, 0.0f, fDeltaTime, m_pHost->m_fVertSpeed);
				m_pHost->SetPos(vCurPos);

				if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
				{
					m_pHost->ResetJump();	//	Reset jump counter
					Finish();
					m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed, iMoveMode);
				}
				else
					m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 0, m_vMoveDest, cdr.vAbsVelocity, iMoveMode);
			}
		}
		else if (m_iDestType == DEST_PUSH)
		{
			A3DVECTOR3 vMoveDir;
			float fSpeed = m_pHost->GetGroundSpeed();
			bool bFinish = false;

			if (m_pHost->GetPushDir(vMoveDir, CECHostPlayer::MD_FORWARD | CECHostPlayer::MD_BACK | CECHostPlayer::MD_LEFT | CECHostPlayer::MD_RIGHT, fDeltaTime))
			{
				if (!vMoveDir.IsZero())
					m_pHost->StartModelMove(vMoveDir, g_vAxisY, 0);

				A3DVECTOR3 vOldPos111 = vCurPos;
				vCurPos = m_pHost->m_MoveCtrl.GroundMove(vMoveDir, fSpeed, fDeltaTime, m_pHost->m_fVertSpeed);
				m_pHost->SetPos(vCurPos);
			}
			else
			{
				if(!m_bUseAutoMoveDialog)
					bFinish = true;
				else
					m_iDestType = DEST_2D;
			}
			if (bFinish || m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
			{
				if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
				{
					//	blocked case, so we clear the fYVel of cdr!
					cdr.fYVel = 0.0f;
				}

				Finish();
				m_pHost->m_vVelocity.Clear();
				m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed, iMoveMode);
			}
			else
			{
				m_pHost->m_vVelocity = vMoveDir * fSpeed;
				m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 2, g_vOrigin, m_pHost->m_vVelocity, iMoveMode);
			}
		}
		else if (IsAutoPF())	//	参考 DEST_2D 修改
		{
			float fDist(0.0f);
			A3DVECTOR3 vCurDest = CECIntelligentRoute::Instance().GetCurDest();
			if (m_pHost->m_GndInfo.bOnGround)
			{
				//	Adjust direction only when player on ground
				m_vCurDir = vCurDest - vCurPos;
				m_vCurDir.y = 0.0f;
				fDist = m_vCurDir.Normalize();
			}
			else{
				fDist = a3d_MagnitudeH(vCurDest-vCurPos);
			}

			//	Ground move
			vCurPos = m_pHost->m_MoveCtrl.GroundMove(m_vCurDir, fSpeed, fDeltaTime, m_pHost->m_fVertSpeed);
			if (!m_vCurDir.IsZero()){
				m_pHost->StartModelMove(m_vCurDir, g_vAxisY, 100);
				m_pHost->ChangeModelTargetDirAndUp(m_vCurDir, g_vAxisY);
			}

			if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
			{
				// blocked, so we clear the fYVel of cdr!
				cdr.fYVel = 0.0f;
				Finish();
				m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed, iMoveMode);
			}
			else
			{
				m_pHost->SetPos(vCurPos);
				CECIntelligentRoute::Instance().OnPlayerPosChange(vCurPos);
				if (CECIntelligentRoute::Instance().IsPathFinished())
				{
					//	CECHostPlayer::SetPos 中，已经调用 CECIntelligentRoute::OnPlayerPosChange 对下一步进行修正
					//	如果已经到达目标，则停止移动
					Finish();
					m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fSpeed, iMoveMode);
				}
				else
				{
					m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 1, vCurDest, cdr.vAbsVelocity, iMoveMode);
				}
			}
		}
	}
	else	//	m_bMeetSlide == true
	{
		m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, m_pHost->GetGroundSpeed(), GP_MOVE_SLIDE);
		Finish();
	}

	return true;
}

void CECHPWorkMove::Glide(float fMoveTime, const A3DVECTOR3& vMoveDirH,  float fDeltaTime, bool bFly)
{
	A3DVECTOR3 vHostDir = m_pHost->GetModelMoveDir();
	float fAngle = DotProduct(vMoveDirH, vHostDir);
	a_Clamp(fAngle, -1.0f, 1.0f);
	fAngle = acosf(fAngle);

	if (m_pHost->IsShapeChanged())
	{
		float fTime = fAngle * (bFly ? ang_vel_fly : ang_vel_swim);
		m_pHost->StartModelMove(vMoveDirH, g_vAxisY, DWORD(fTime * 1000.f));
		return;
	}

	m_fGlideSpan += fDeltaTime;
	float fDeltaRot = fabsf(m_fGlideVel * fDeltaTime);

	if (fAngle < DEG2RAD(5.0f) ||
	   (!m_bGliding && fMoveTime < 1.0f) ||
	   ( m_bGliding && (m_fGlideSpan >= m_fGlideTime || fDeltaRot >= fAngle)))
	{
		if (!vMoveDirH.IsZero())
			m_pHost->StartModelMove(vMoveDirH, g_vAxisY, 0);

		if (m_bGliding)
		{
			m_bGliding = false;
			m_fCurPitch = 0;

			if (bFly)
			{
				//if (m_pHost->UsingWing())
				if (m_pHost->GetWingType() == WINGTYPE_WING)
					m_pHost->PlayAction(CECPlayer::ACT_FLY, false);
				else
					m_pHost->PlayAction(CECPlayer::ACT_FLY_SWORD, false);
			}
		}

		return;
	}

	float pitch_co = m_pHost->GetWingType() == WINGTYPE_WING/*m_pHost->UsingWing()*/ ? pitch_co_wing :pitch_co_fly_sword;

	if (!m_bGliding)
	{
		m_fGlideSpan = 0;
		m_fGlideTime = fAngle * (bFly ? ang_vel_fly : ang_vel_swim);
		if (m_fGlideTime > fMoveTime) m_fGlideTime = fMoveTime;

		if (CrossProduct(vHostDir, vMoveDirH).y > 0) // turn right
		{
			m_fGlidePitch = (-pitch_co) * fAngle;
			m_fGlideAng = fAngle;
		}
		else
		{
			m_fGlidePitch = pitch_co * fAngle;
			m_fGlideAng = -fAngle;
		}

		m_fGlideVel = m_fGlideAng / m_fGlideTime;
		m_fGlideVel *= 1.5f;
		m_bGliding = true;
	}

	float fRatio = 1.0f - fabsf(fAngle / m_fGlideAng);
	A3DVECTOR3 vNewDir = a3d_RotatePosAroundY(vHostDir, m_fGlideVel * fDeltaTime);
	vNewDir.y = 0.0f;
	vNewDir.Normalize();

	if (fRatio > sidle_co)
	{
		fRatio -= sidle_co;
		fRatio = 1.0f - fRatio / sidle_co_r;
		m_fCurPitch = m_fGlidePitch * fRatio;
	}
	else
	{
		fRatio /= sidle_co;
		m_fCurPitch = m_fCurPitch * (1.0f - fRatio) + m_fGlidePitch * fRatio;
	}

	if (m_pHost->IsShapeChanged())
		m_pHost->StartModelMove(vNewDir, g_vAxisY, 0);
	else
	{
		A3DVECTOR3 vNewUp = a3d_RotatePosAroundAxis(g_vAxisY, vNewDir, m_fCurPitch);
		m_pHost->StartModelMove(vNewDir, vNewUp, 0);
	}
}

//	Tick routine of flying or swimming
bool CECHPWorkMove::Tick_FlySwim(float fDeltaTime)
{
	A3DVECTOR3 vCurPos = m_pHost->GetPos();
	int iMoveMode = (m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_AIR) ? GP_MOVE_AIR : GP_MOVE_WATER;

	float na, fMaxSpeed;
	bool bInAir;
	if (m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_AIR)
	{
		bInAir = true;
		na = EC_NACCE_AIR;
		fMaxSpeed = m_pHost->GetFlySpeed();
	}
	else
	{
		bInAir = false;
		na = EC_NACCE_WATER;
		fMaxSpeed = m_pHost->GetSwimSpeedSev();
	}

	if (m_bReadyCancel || m_bMeetSlide)
	{
		m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fMaxSpeed, iMoveMode | GP_MOVE_RUN);
		Finish();
		return true;
	}
	
	// for auto move
	if(m_bUseAutoMoveDialog && m_fAutoHeight > 0.0f && m_iDestType != DEST_PUSH)	
	{
		if(!m_pHost->m_dwMoveRelDir)
		{
			if((int)(vCurPos.y/10.0f) == (int)m_fAutoHeight)
			{
				if(!m_bReachedHeight)
				{
					m_pHost->m_vVelocity.y = 0.0f;			
					m_pHost->m_dwMoveRelDir &= ~(CECHostPlayer::MD_ABSUP | CECHostPlayer::MD_ABSDOWN);
					m_bReachedHeight = true;
				}
			}
			else
			{
				if(!m_bReachedHeight)
				{
					if(vCurPos.y < m_fAutoHeight*10.0f)
					{
						m_pHost->m_dwMoveRelDir &= ~(CECHostPlayer::MD_ABSDOWN);
						m_pHost->m_dwMoveRelDir |= CECHostPlayer::MD_ABSUP;		
					}
					else if(vCurPos.y > m_fAutoHeight*10.0f)
					{
						m_pHost->m_dwMoveRelDir &= ~(CECHostPlayer::MD_ABSUP);	
						m_pHost->m_dwMoveRelDir |= CECHostPlayer::MD_ABSDOWN;
					}
				}
				else  // Auto adjust height, so we should refresh move height
				{
					m_fAutoHeight = vCurPos.y/10.0f;
				}
			}
		}
		else	// Player manually set auto move height
		{
			m_bReachedHeight = true;

			m_fAutoHeight = vCurPos.y/10.0f;
		}
	}

	if (m_pHost->m_dwMoveRelDir & (CECHostPlayer::MD_LEFT | CECHostPlayer::MD_RIGHT | CECHostPlayer::MD_FORWARD | CECHostPlayer::MD_BACK))
		m_iDestType = DEST_PUSH;

	ON_AIR_CDR_INFO& cdr = m_pHost->m_AirCDRInfo;

	if (m_iDestType == DEST_DIR)
	{
		A3DVECTOR3 vPushDir(0.0f);
		m_pHost->GetPushDir(vPushDir, CECHostPlayer::MD_ALL);
		vPushDir.x = vPushDir.z = 0.0f;

		float fSpeed1H = m_pHost->m_vVelocity.MagnitudeH();
		float fSpeed1V = m_pHost->m_vVelocity.y;

		A3DVECTOR3 vMoveDirH = m_vMoveDest;
		float pa=EC_PUSH_ACCE;

		float fSpeed2H = fSpeed1H + (pa + na) * fDeltaTime;
		if (!pa && fSpeed2H < 0.0f)
			fSpeed2H = 0.0f;	//	Only resistance couldn't generate negative speed
		else if (fSpeed2H > fMaxSpeed)
			fSpeed2H = fMaxSpeed;

		Glide(5.0f, vMoveDirH, fDeltaTime, bInAir);
		
		vMoveDirH = m_pHost->GetModelMoveDir();
		vMoveDirH.y = 0;
		vMoveDirH.Normalize();

		//	Vertical speed
		float fSpeed2V = CalcFlySwimVertSpeed(fSpeed1V, vPushDir.y, EC_PUSH_ACCE, fDeltaTime);
		A3DVECTOR3 vVel2 = vMoveDirH * fSpeed2H + g_vAxisY * fSpeed2V;

		//	Air/water move
		vCurPos = m_pHost->m_MoveCtrl.AirWaterMove(vVel2, fDeltaTime, bInAir);
		m_pHost->SetPos(vCurPos);
		m_pHost->m_vVelocity = vVel2;

		if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
		{
			Finish();
			m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fMaxSpeed, iMoveMode | GP_MOVE_RUN);
		}
		else
			m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 0, m_vMoveDest, vVel2, iMoveMode | GP_MOVE_RUN);
	}
	else if (m_iDestType == DEST_2D)
	{
		A3DVECTOR3 vPushDir(0.0f);
		m_pHost->GetPushDir(vPushDir, CECHostPlayer::MD_ALL);
		vPushDir.x = vPushDir.z = 0.0f;

		float fSpeed1H = m_pHost->m_vVelocity.MagnitudeH();
		float fSpeed1V = m_pHost->m_vVelocity.y;

		A3DVECTOR3 vMoveDirH = m_vMoveDest - vCurPos;
		vMoveDirH.y = 0.0f;
		float fDistH = vMoveDirH.Normalize();

		float pa=0.0f;

		//	Calculate the distance to reduce velocity to 0
		float s = -0.5f * fSpeed1H * fSpeed1H / na;
		if (fDistH > s - 0.01f)
			pa = EC_PUSH_ACCE;

		float fSpeed2H = fSpeed1H + (pa + na) * fDeltaTime;
		if (!pa && fSpeed2H < 0.0f)
			fSpeed2H = 0.0f;	//	Only resistance couldn't generate negative speed
		else if (fSpeed2H > fMaxSpeed)
			fSpeed2H = fMaxSpeed;

		Glide(fDistH / fMaxSpeed, vMoveDirH, fDeltaTime, bInAir);
		
		vMoveDirH = m_pHost->GetModelMoveDir();
		vMoveDirH.y = 0;
		vMoveDirH.Normalize();

		//	Vertical speed
		float fSpeed2V = CalcFlySwimVertSpeed(fSpeed1V, vPushDir.y, EC_PUSH_ACCE, fDeltaTime);
		A3DVECTOR3 vVel2 = vMoveDirH * fSpeed2H + g_vAxisY * fSpeed2V;

		//	Air/water move
		vCurPos = m_pHost->m_MoveCtrl.AirWaterMove(vVel2, fDeltaTime, bInAir);

		if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
		{
			vVel2.Clear();
			Finish();
		}
		else
		{
			//	Reached destination ?
			A3DVECTOR3 vMoveDelta = vCurPos - m_pHost->GetPos();
			vMoveDelta.y = 0.0f;
			float fMoveDistH = vMoveDelta.Normalize();
			if (fMoveDistH >= fDistH)
			{
				vVel2.x = vVel2.z = 0.0f;
				if (!vVel2.y)
				{
					Finish();
				}
				else
				{
					if(m_bUseAutoMoveDialog)
					{
						Finish();
						vVel2.y = 0.0f;
					}

					m_iDestType = DEST_PUSH;
				}
			}
		}

		m_pHost->SetPos(vCurPos);
		m_pHost->m_vVelocity = vVel2;

		if (m_bFinished)
			m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, vVel2.Magnitude(), iMoveMode | GP_MOVE_RUN);
		else
			m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 0, m_vMoveDest, vVel2, iMoveMode | GP_MOVE_RUN);
	}
	else if (m_iDestType == DEST_3D)
	{
		float fSpeed1 = m_pHost->m_vVelocity.Magnitude();
		A3DVECTOR3 vMoveDir = m_vMoveDest - vCurPos;
		float fDist = vMoveDir.Normalize();

		float pa=0.0f;

		//	Calculate the distance to reduce velocity to 0
		float s = -0.5f * fSpeed1 * fSpeed1 / na;
		if (fDist > s - 0.01f)
			pa = EC_PUSH_ACCE;

		float fSpeed2 = fSpeed1 + (pa + na) * fDeltaTime;
		if (!pa && fSpeed2 < 0.0f)
			fSpeed2 = 0.0f;	//	Only resistance couldn't generate negative speed

		a_Clamp(fSpeed2, -fMaxSpeed, fMaxSpeed);

		A3DVECTOR3 vMoveDirH(vMoveDir.x, 0.0f, vMoveDir.z);
		if (!vMoveDirH.IsZero()){
			m_pHost->StartModelMove(vMoveDirH, g_vAxisY, 100);
			m_pHost->ChangeModelTargetDirAndUp(vMoveDirH, g_vAxisY);
		}

		//	Air/water move
	//	A3DVECTOR3 vVel1 = vMoveDir * fSpeed1;
		A3DVECTOR3 vVel2 = vMoveDir * fSpeed2;
		vCurPos = m_pHost->m_MoveCtrl.AirWaterMove(vMoveDir, fSpeed2, fDeltaTime, bInAir);

		if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
		{
			vVel2.Clear();
			Finish();
		}
		else
		{
			//	Reached destination ?
			A3DVECTOR3 vMoveDelta = vCurPos - m_pHost->GetPos();
			float fMoveDist = vMoveDelta.Normalize();
			if (fMoveDist >= fDist)
			{
				vVel2.Clear();
				Finish();
				m_bUseAutoMoveDialog = false;
			}
		}

		m_pHost->SetPos(vCurPos);
		m_pHost->m_vVelocity = vVel2;

		if (m_bFinished)
			m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fMaxSpeed, iMoveMode | GP_MOVE_RUN);
		else
			m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 1, m_vMoveDest, vVel2, iMoveMode | GP_MOVE_RUN);
	}
	else if (m_iDestType == DEST_PUSH)
	{
		A3DVECTOR3 vPushDir(0.0f), vUp;
		bool bPush = m_pHost->GetPushDir(vPushDir, CECHostPlayer::MD_FORWARD | CECHostPlayer::MD_BACK | CECHostPlayer::MD_LEFT | CECHostPlayer::MD_RIGHT, fDeltaTime);

		if (!bPush)
		{
			vPushDir = m_pHost->GetCameraCoord()->GetDir();
			vPushDir.y = 0;
			vPushDir.Normalize();
		}
	
  		int nPitchDir = 0;
		if ((m_pHost->m_dwMoveRelDir & (CECHostPlayer::MD_LEFT | CECHostPlayer::MD_RIGHT)) == 0)
		{
			A3DVECTOR3 vOldDir = m_pHost->GetModelMoveDir();
			vOldDir.y = 0;
			vOldDir.Normalize();

			A3DVECTOR3 vNewDir = vPushDir;
			vNewDir.y = 0;
			vNewDir.Normalize();

			float fAngle = DotProduct(vOldDir, vNewDir);
			if (fAngle < 1.0f - 1e-4)
			{
				A3DVECTOR3 vUp = CrossProduct(vOldDir, vNewDir);
				if (vUp.y > 0) nPitchDir = 1;
				else nPitchDir = -1;
				if (m_pHost->m_dwMoveRelDir & CECHostPlayer::MD_BACK)
					nPitchDir = -nPitchDir;
			}
		}

		if (m_pHost->m_dwMoveRelDir)
		{
			if (m_pHost->m_dwMoveRelDir & ~(CECHostPlayer::MD_ABSDOWN | CECHostPlayer::MD_ABSUP))
			{
				float fPitchDelta = (/*m_pHost->UsingWing()*/m_pHost->GetWingType() == WINGTYPE_WING ? push_pitch_vel_wing : push_pitch_vel_fly_sword) * fDeltaTime;

				if ((m_pHost->m_dwMoveRelDir & CECHostPlayer::MD_LEFT) || nPitchDir == -1)
				{
					if (m_pHost->m_dwMoveRelDir & CECHostPlayer::MD_BACK)
						m_fPushPitch -= fPitchDelta;
					else
						m_fPushPitch += fPitchDelta;
				}
				else if ((m_pHost->m_dwMoveRelDir & CECHostPlayer::MD_RIGHT) || nPitchDir == 1)
				{
					if (m_pHost->m_dwMoveRelDir & CECHostPlayer::MD_BACK)
						m_fPushPitch += fPitchDelta;
					else
						m_fPushPitch -= fPitchDelta;
				}
				else if (m_fPushPitch > fPitchDelta)
					m_fPushPitch -= fPitchDelta;
				else if (m_fPushPitch < -fPitchDelta)
					m_fPushPitch += fPitchDelta;
				else
					m_fPushPitch = 0;

				A3DVECTOR3 vRight = m_pHost->GetCameraCoord()->GetRight();
				float fLean = -asinf(m_pHost->GetCameraCoord()->GetDir().y);
				a_Clamp(fLean, -lean_max_ang, lean_max_ang);

				if (fabsf(fLean) > DEG2RAD(3.0f))
				{
					vPushDir = a3d_RotatePosAroundAxis(vPushDir, vRight, fLean);
					vUp = a3d_RotatePosAroundAxis(g_vAxisY, vRight, fLean);
				}
				else
					vUp = g_vAxisY;

				float pitch_ang = /*m_pHost->UsingWing()*/ m_pHost->GetWingType() == WINGTYPE_WING ? pitch_ang_wing : pitch_ang_fly_sword;
				a_Clamp(m_fPushPitch, -pitch_ang, pitch_ang);

				if (fabsf(m_fPushPitch) > DEG2RAD(4.0f))
					vUp = a3d_RotatePosAroundAxis(vUp, vPushDir, m_fPushPitch);

				m_pHost->StartModelMove(vPushDir, vUp, 0);
			}

		//	if (bPush)
			if (bPush || (m_pHost->m_dwMoveRelDir & (CECHostPlayer::MD_ABSDOWN | CECHostPlayer::MD_ABSUP)))
			{
			//	float pa = bPush ? EC_PUSH_ACCE : 0.0f;
				float pa = EC_PUSH_ACCE;
				float na = m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_AIR ? EC_NACCE_AIR : EC_NACCE_WATER;
				float fAccel = pa + na;

				float fSpeed1 = m_pHost->m_vVelocity.Magnitude();
				float fSpeed2 = fSpeed1 + fAccel * fDeltaTime;
				a_Clamp(fSpeed2, 0.0f, fMaxSpeed);

				//	Air/water move
				A3DVECTOR3 vVelDir(0.0f);
				if (bPush)
					vVelDir = vPushDir;

				if (m_pHost->m_dwMoveRelDir & CECHostPlayer::MD_ABSDOWN)
				{
					vVelDir += -g_vAxisY;
					vVelDir.Normalize();
				}
				else if (m_pHost->m_dwMoveRelDir & CECHostPlayer::MD_ABSUP)
				{
					vVelDir += g_vAxisY;
					vVelDir.Normalize();
				}

			//	A3DVECTOR3 vVel = vPushDir * fSpeed2;
				A3DVECTOR3 vVel = vVelDir * fSpeed2;
				vCurPos = m_pHost->m_MoveCtrl.AirWaterMove(vVel, fDeltaTime, bInAir);

				if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
				{
					Finish();
					m_pHost->m_vVelocity.Clear();
					m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fMaxSpeed, iMoveMode | GP_MOVE_RUN);
				}
				else
				{
					m_pHost->SetPos(vCurPos);
					if(m_bUseAutoMoveDialog)
					{
						m_fAutoHeight = vCurPos.y/10.0f;
					}	
					m_pHost->m_vVelocity = vVel;
					m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 2, g_vOrigin, vVel, iMoveMode | GP_MOVE_RUN);
				}
			}
		}
		else
		{
			if(!m_bUseAutoMoveDialog)
				Finish();
			else 
				m_iDestType = DEST_2D;

			m_fPushPitch = 0;
			m_pHost->m_vVelocity.Clear();
			m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fMaxSpeed, iMoveMode | GP_MOVE_RUN);			
		}
	}
	else if (m_iDestType == DEST_STANDJUMP)
	{
		//	If host player fly off when jumping up, code will go here. In the
		//	case, just stop move work is well.
		Finish();
	}
	else if (IsAutoPF())
	{
		CECIntelligentRoute::Instance().ResetSearch();
		m_bSwitchTo2D = true;
	}

	return true;
}

//	Calculate vertical speed when fly or swim
float CECHPWorkMove::CalcFlySwimVertSpeed(float fSpeed1, float fPushDir, float fPushAccel,
										  float fDeltaTime)
{
	float pa = fPushDir ? fPushAccel : 0.0f;

	float na=0.0f, fMaxSpeed;
	if (m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_AIR)
	{
		na = EC_NACCE_AIR;
		fMaxSpeed = m_pHost->GetFlySpeed();
	}
	else
	{
		na = EC_NACCE_WATER;
		fMaxSpeed = m_pHost->GetSwimSpeedSev();
	}

	//	When player free fall into water, fSpeed1 may be >= fMaxSpeed
	if (fabs(fSpeed1) > fMaxSpeed)
	{
		na *= 2.0f;
		if (fPushDir * fSpeed1 > 0.0f)
			pa = 0.0f;
	}

	//	Vertical accelerate
	float fAccel = 0.0f;
	if (fPushDir > 0.0f)
		fAccel = pa + na;
	else if (fPushDir < 0.0f)
		fAccel = -(pa + na);
	else if (fSpeed1 > 0.0f)
		fAccel = na;
	else if (fSpeed1 < 0.0f)
		fAccel = -na;

	//	Vertical speed
	float fSpeed2 = fSpeed1 + fAccel * fDeltaTime;
	if (!pa && fSpeed2 * fSpeed1 < 0.0f)
		fSpeed2 = 0.0f;
	
	//	If accelerate and speed on same direction, limit speed
	if (fAccel * fSpeed2 > 0.0f)
		a_Clamp(fSpeed2, -fMaxSpeed, fMaxSpeed);

	return fSpeed2;
}

//	Finish work
void CECHPWorkMove::Finish()
{
	m_bFinished = true;
	Cancel();

	// Close 'Win_AutoPlay' dialog if it exists
	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->AutoMoveShowDialog(false);

	if(m_bUseAutoMoveDialog)
	{
		if(m_bAutoLand)
		{
			m_bAutoLand = false;
			if(g_pGame->GetGameRun()->GetHostPlayer()->IsFlying())
				g_pGame->GetGameRun()->GetHostPlayer()->CmdFly(false);
		}

		m_bUseAutoMoveDialog		= false;
		m_fAutoHeight	= -1.0f; 
		m_bAutoFly		= false;
		m_bReachedHeight = true;
	}

	//清空任务追踪数据
	m_iNPCTempleId = 0;
	m_iTaskId = 0;
}

//	Work is cancel
void CECHPWorkMove::Cancel()
{
	if (m_pHost->m_pMoveTargetGFX)
		m_pHost->m_pMoveTargetGFX->Stop();

	A3DVECTOR3 vDir = m_pHost->GetDir();
	vDir.y = 0;
	vDir.Normalize();
	if (!vDir.IsZero()){
		m_pHost->StopModelMove(vDir, g_vAxisY, 0);
	}

	ClearResetUseAutoPF();
	if (CECIntelligentRoute::Instance().IsUsageMove()){
		if (!CECIntelligentRoute::Instance().IsIdle()){
			CECIntelligentRoute::Instance().ResetSearch();
			m_bSwitchTo2D = true;	//	启动飞行中断后恢复时、需要切换到 DEST_2D 模式
		}
	}
	CECHPWork::Cancel();
	AP_ActionEvent(AP_EVENT_MOVEFINISHED);
}

void CECHPWorkMove::SetUseAutoMoveDialog(bool bUseAutoMoveDialog)
{
	if (bUseAutoMoveDialog && IsAutoPF()){
		//	bUseAutoMoveDialog 将触发飞行（含调整高度）、允许临时切换到 DEST_PUSH 并切换回 DEST_2D，因此需禁用
		return;
	}
	m_bUseAutoMoveDialog = bUseAutoMoveDialog; 
	if(m_bUseAutoMoveDialog)
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->AutoMoveShowDialog(true);
	}
}

bool CECHPWorkMove::GetAutoMove()
{
	return m_bUseAutoMoveDialog
		|| IsAutoPF() && CECIntelligentRoute::Instance().IsUsageMove() && !CECIntelligentRoute::Instance().IsIdle();
}

void CECHPWorkMove::SetTaskNPCInfo(int tid, int taskid)
{
	m_iNPCTempleId = tid;
	m_iTaskId = taskid;
}

void CECHPWorkMove::SwitchToDest2D()
{
	int tid, taskid;
	tid = m_iNPCTempleId;
	taskid = m_iTaskId;
	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->SetAutoMoveShowDialogTarget((int)m_vMoveDest.x, (int)m_vMoveDest.z);
	SetDestination(CECHPWorkMove::DEST_2D, m_vMoveDest);
	SetTaskNPCInfo(tid, taskid);
	SetUseAutoMoveDialog(true);
}
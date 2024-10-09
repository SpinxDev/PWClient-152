/*
 * FILE: EC_HPWorkTrace.cpp
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
#include "EC_HPWorkTrace.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_ManNPC.h"
#include "EC_NPC.h"
#include "EC_Matter.h"
#include "EC_ManMatter.h"
#include "EC_ElsePlayer.h"
#include "EC_GameSession.h"
#include "EC_ManPlayer.h"
#include "EC_Skill.h"
#include "EC_Model.h"
#include "EC_FixedMsg.h"
#include "EC_Inventory.h"
#include "EC_Utility.h"
#include "elementdataman.h"
#include "EC_AutoPolicy.h"
#include "EC_PlayerWrapper.h"
#include "EC_ComboSkill.h"
#include "EC_IntelligentRoute.h"
#include "EC_IntelligentRouteUtil.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "DlgNPC.h"

#include "A3DFuncs.h"
#include "AUIDialog.h"

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
#ifdef SHOW_AUTOMOVE_FOOTPRINTS
extern abase::vector<A3DVECTOR3> g_AutoPFFollowPoints;
extern abase::vector<A3DVECTOR3> g_AutoPFPathPoints;
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////
#define AUTOPF_CHECK_TIME_INTERVAL 3000


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkTrace
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkTrace::CECHPWorkTrace(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_TRACEOBJECT, pWorkMan)
{
	m_dwMask		= MASK_TRACEOBJECT;
	m_dwTransMask	= MASK_STAND | MASK_MOVETOPOS | MASK_FLYOFF | MASK_FREEFALL |
						MASK_FOLLOW | MASK_USEITEM;

	CECHPWorkTrace::Reset();
}

CECHPWorkTrace::~CECHPWorkTrace()
{
	delete m_pTraceObject;
}

//	Reset work
void CECHPWorkTrace::Reset()
{
	CECHPWork::Reset();

	m_bHaveMoved	= false;
	m_bMeetSlide	= false;
	m_bCheckTouch	= true;
	m_bReadyCancel	= false;
	m_bMoreClose	= false;
	m_pPrepSkill	= NULL;
	m_bForceAttack	= false;
	m_bActionDone	= false;
	ClearResetUseAutoPF();
	m_bUseAutoPF	= false;
	m_dwAutoPFNextCheckTime = 0;
	m_pTraceObject	= NULL;
}

//	Copy work data
bool CECHPWorkTrace::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkTrace* pSrc = (CECHPWorkTrace*)pWork;

	m_bHaveMoved	= pSrc->m_bHaveMoved;
	m_bMeetSlide	= pSrc->m_bMeetSlide;
	m_bCheckTouch	= pSrc->m_bCheckTouch;
	m_bReadyCancel	= pSrc->m_bReadyCancel;
	m_bMoreClose	= pSrc->m_bMoreClose;
	m_vCurDirH		= pSrc->m_vCurDirH;
	m_pPrepSkill	= pSrc->m_pPrepSkill;
	m_bForceAttack	= pSrc->m_bForceAttack;
	m_bActionDone	= pSrc->m_bActionDone;
	m_bShouldResetUseAutoPF = pSrc->m_bShouldResetUseAutoPF;
	m_bUseAutoPFResetValue	= pSrc->m_bUseAutoPFResetValue;
	m_bUseAutoPF	= pSrc->m_bUseAutoPF;
	m_dwAutoPFNextCheckTime = pSrc->m_dwAutoPFNextCheckTime;

	delete m_pTraceObject;
	m_pTraceObject	= pSrc->m_pTraceObject->Clone();

	return true;
}

//	On first tick
void CECHPWorkTrace::OnFirstTick()
{
	m_pHost->m_iMoveMode = CECPlayer::MOVE_MOVE;
	m_bHaveMoved = false;
}


//	Stop move when touch target
void CECHPWorkTrace::StopMove(bool bFinish)
{
	//	If 'trace' work was transfered from a work which 
	//	needs moving (such as 'move to' work) and we touch the target 
	//	immediately (m_bHaveMoved = false), we must need to send 'stop move'
	//	command
	if (m_bHaveMoved || !m_pHost->m_MoveCtrl.IsStop())
		m_pHost->m_MoveCtrl.SendStopMoveCmd();

	m_pHost->m_vVelocity.Clear();
	m_pHost->StopModelMove();

	if (bFinish){
		m_bFinished = true;
	}
}

//	Tick routine
bool CECHPWorkTrace::Tick(DWORD dwDeltaTime)
{
	CECHPWork::Tick(dwDeltaTime);

	CheckPrepSkill();

	UpdateResetUseAutoPF();
	UpdateUseAutoPF();
	
	//	m_bFinished flag may be set both in OnFirstTick() and CheckPrepSkill(),
	//	so check it here !
	if (m_bFinished){
		return true;
	}
	if (m_pTraceObject->IsTargetMissing()){
		OnTargetMissing();
		return true;
	}

	if (m_bCheckTouch){
		if (IsGoodTimeToTouch()){
			if (m_pTraceObject->CanTouchFrom(m_pHost->GetPos())){
				OnTouchTarget();
				return true;
			}
		}
	}
	m_bCheckTouch = true;

	if (!m_pHost->IsRooting())
	{
		//	Continue tracing object
		float fDeltaTime = dwDeltaTime * 0.001f;
		if (m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_GROUND)
		{
			//	Play appropriate actions
			if (!m_pHost->IsJumping() && !m_pHost->IsPlayingAction(CECPlayer::ACT_TRICK_RUN) &&
				m_pHost->m_iMoveMode != CECPlayer::MOVE_SLIDE)
			{
				int iAction = m_pHost->GetMoveStandAction(true);
				m_pHost->PlayAction(iAction, false);
			}

			Trace_Walk(fDeltaTime);
		}
		else	//	(m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_AIR || m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_WATER)
		{
			m_pHost->ResetJump();

			//	Play appropriate actions
			int iAction = m_pHost->GetMoveStandAction(true);
			m_pHost->PlayAction(iAction, false);

			Trace_FlySwim(fDeltaTime);
		}

		m_bHaveMoved = true;
	}

	return true;
}

//	Trace on ground
bool CECHPWorkTrace::Trace_Walk(float fDeltaTime)
{
	A3DVECTOR3 vCurPos = m_pHost->GetPos();
	A3DVECTOR3 vTargetPos = GetCurMovingDest();
	CDR_INFO& cdr = m_pHost->m_CDRInfo;

	if (m_pHost->m_iMoveMode == CECPlayer::MOVE_SLIDE)
	{
		m_pHost->PlayAction(CECPlayer::ACT_JUMP_LOOP, false);

		//	This will cause stop moming after we slide down.
		A3DVECTOR3 vDir = vTargetPos - vCurPos;
		vDir.y = 0;
		vDir.Normalize();
		
		float fMaxSpeedV;
		m_bMeetSlide = m_pHost->m_MoveCtrl.MeetSlope(vDir, fMaxSpeedV);
		a_ClampFloor(cdr.fYVel, -fMaxSpeedV);

		if (!vDir.IsZero())
			m_vCurDirH = vDir;

		vCurPos = m_pHost->m_MoveCtrl.GroundMove(m_vCurDirH, m_pHost->GetGroundSpeed(), fDeltaTime);

		if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
		{
			m_pHost->m_MoveCtrl.SetSlideLock(true);
			m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, m_pHost->GetGroundSpeed(), GP_MOVE_SLIDE);
			m_bFinished = true;
		}
		else
		{
			m_pHost->SetPos(vCurPos);
			if (GetUseAutoPF() && CECIntelligentRoute::Instance().IsMoveOn()){
				CECIntelligentRoute::Instance().OnPlayerPosChange(vCurPos);
			}
			m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 2, g_vOrigin, cdr.vAbsVelocity, GP_MOVE_SLIDE);
		}
	}
	else if (!m_bMeetSlide)
	{
		int iMoveMode = m_pHost->m_bWalkRun ? GP_MOVE_RUN : GP_MOVE_WALK;
		if (m_pHost->IsJumping())
			iMoveMode = GP_MOVE_JUMP;
		else if (!m_pHost->m_GndInfo.bOnGround)
			iMoveMode = GP_MOVE_FALL;

		if (m_pHost->m_GndInfo.bOnGround)
		{
			if (m_bReadyCancel)
			{
				StopMove(true);
				return true;
			}

			//	Ajust direction only when player on ground
			A3DVECTOR3 vDirH = vTargetPos - vCurPos;
			A3DVECTOR3 v = a3d_Normalize(vDirH);
			if (fabs(v.y) > 0.9848f)
			{
				PressCancel();
				return true;
			}

			vDirH.y = 0.0f;
			vDirH.Normalize();
			if (!vDirH.IsZero())
				m_vCurDirH = vDirH;
		}

		vCurPos = m_pHost->m_MoveCtrl.GroundMove(m_vCurDirH, m_pHost->GetGroundSpeed(), fDeltaTime, m_pHost->m_fVertSpeed);
		m_pHost->SetPos(vCurPos);
		if (GetUseAutoPF() && CECIntelligentRoute::Instance().IsMoveOn()){
			CECIntelligentRoute::Instance().OnPlayerPosChange(vCurPos);
		}

		if (cdr.vTPNormal.IsZero())
			m_bCheckTouch = false;

		if (!m_vCurDirH.IsZero()){
			m_pHost->StartModelMove(m_vCurDirH, g_vAxisY, 0);
		}

		if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
		{
		//	m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, m_pHost->GetGroundSpeed(), iMoveMode);
			PressCancel();
		}
		else
			m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 0, vTargetPos, cdr.vAbsVelocity, iMoveMode);
	}
	else	//	m_bMeetSlide == true
	{
		if (m_bHaveMoved)
			m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, m_pHost->GetGroundSpeed(), GP_MOVE_SLIDE);

		m_bFinished = true;
	}
	
#ifdef SHOW_AUTOMOVE_FOOTPRINTS
	g_AutoPFFollowPoints.push_back(vCurPos);
#endif

	return true;
}

//	Trace in air and water
bool CECHPWorkTrace::Trace_FlySwim(float fDeltaTime)
{
	if (m_bMeetSlide || m_bReadyCancel)
	{
		StopMove(true);
		return true;
	}
	if (GetUseAutoPF()){
		SetUseAutoPF(false);
	}

	A3DVECTOR3 vCurPos = m_pHost->GetPos();
	A3DVECTOR3 vTargetPos = GetCurMovingDest();

	float fSpeed1 = m_pHost->m_vVelocity.Magnitude();
	A3DVECTOR3 vMoveDir = vTargetPos - vCurPos;
	float fDist = vMoveDir.Normalize();
	int iMoveMode;

	float na=0.0f, pa=0.0f, fMaxSpeed;
	bool bInAir;
	if (m_pHost->m_iMoveEnv == CECPlayer::MOVEENV_AIR)
	{
		bInAir = true;
		na = EC_NACCE_AIR;
		fMaxSpeed = m_pHost->GetFlySpeed();
		iMoveMode = GP_MOVE_AIR | GP_MOVE_RUN;
	}
	else
	{
		bInAir = false;
		na = EC_NACCE_WATER;
		fMaxSpeed = m_pHost->GetSwimSpeedSev();
		iMoveMode = GP_MOVE_WATER | GP_MOVE_RUN;
	}

	ON_AIR_CDR_INFO& cdr = m_pHost->m_AirCDRInfo;

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

	A3DVECTOR3 vVel2 = vMoveDir * fSpeed2;
	vCurPos = m_pHost->m_MoveCtrl.AirWaterMove(vMoveDir, fSpeed2, fDeltaTime, bInAir, true);

	m_pHost->SetPos(vCurPos);
	m_pHost->m_vVelocity = vVel2;

	if (fabs(vMoveDir.y) > 0.9848f)
	{
		A3DVECTOR3 v1 = vMoveDir;
		A3DVECTOR3 v2 = vTargetPos - vCurPos;
		v1.y = v2.y = 0.0f;
		if (DotProduct(v1, v2) < 0.0f)
		{
			PressCancel();
			return true;
		}
	}
	
	if (m_pHost->m_MoveCtrl.MoveBlocked() >= 3)
	{
		m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, fMaxSpeed, iMoveMode);
		m_bFinished = true;
	}
	else
		m_pHost->m_MoveCtrl.SendMoveCmd(vCurPos, 1, vTargetPos, vVel2, iMoveMode);

	return true;
}

//	User press cancel button
void CECHPWorkTrace::PressCancel()
{
	m_bReadyCancel = true; 

	if (m_pTraceObject->GetTraceReason() == TRACE_SPELL)
		m_pHost->m_pPrepSkill = NULL;
}

//	Check prepare skill
void CECHPWorkTrace::CheckPrepSkill()
{
	if (!m_bReadyCancel){
		if (m_pTraceObject->GetTraceReason() == TRACE_SPELL)
		{
			ASSERT(m_pPrepSkill);
			m_pHost->m_pPrepSkill = m_pPrepSkill;
		}
		else if (m_pHost->m_pPrepSkill)
			m_pHost->m_pPrepSkill = NULL;
	}
}

//	On work shift
void CECHPWorkTrace::Cancel()
{
	if (m_pHost->m_pPrepSkill && m_pTraceObject->GetTraceReason() == TRACE_SPELL)
		m_pHost->m_pPrepSkill = NULL;
	
	ClearResetUseAutoPF();
	if (GetUseAutoPF()){
		SetUseAutoPF(false);
	}
	m_pHost->StopModelMove();
	CECHPWork::Cancel();

	AP_ActionEvent(m_bActionDone ? AP_EVENT_TRACEOK : AP_EVENT_MOVEFINISHED, m_pTraceObject->GetTraceReason());
}

A3DVECTOR3 CECHPWorkTrace::GetCurMovingDest()
{
	return IsAutoPF() ? CECIntelligentRoute::Instance().GetCurDest() : m_pTraceObject->GetTargetPos();
}

void CECHPWorkTrace::ResetUseAutoPF(bool bUseAutoPF){
	m_bShouldResetUseAutoPF = true;
	m_bUseAutoPFResetValue = bUseAutoPF;
}

void CECHPWorkTrace::ClearResetUseAutoPF(){
	m_bShouldResetUseAutoPF = false;
	m_bUseAutoPFResetValue = false;
}

void CECHPWorkTrace::SetUseAutoPF(bool bUse)
{
	m_bUseAutoPF = bUse;
	if (!m_bUseAutoPF && CECIntelligentRoute::Instance().IsUsageTrace()){
		CECIntelligentRoute::Instance().ResetSearch();
	}
}

bool CECHPWorkTrace::GetUseAutoPF()const
{
	return m_bUseAutoPF;
}

bool CECHPWorkTrace::IsAutoPF()const
{
	return GetUseAutoPF() && CECIntelligentRoute::Instance().IsUsageTrace() && !CECIntelligentRoute::Instance().IsIdle();
}

void CECHPWorkTrace::UpdateUseAutoPF()
{
	//	更新最新目标位置、展开自动寻路或直奔目标
	//
	if (!m_bUseAutoPF || !CECIntelligentRoute::Instance().IsUsageTrace()){
		return;
	}
	while (m_bUseAutoPF)
	{
		if (m_bFinished || m_pHost->IsFlying() ||(m_pTraceObject->GetTargetObject() == NULL)){
			m_bUseAutoPF = false;
			break;
		}		
		if (CECIntelligentRoute::Instance().IsIdle()){
			CECHostBrushTest brushTester(m_pHost->GetPos(), m_pHost->m_CDRInfo.vExtent, m_pHost->m_CDRInfo.fStepHeight);
			if (CECIntelligentRoute::Instance().Search(m_pHost->GetPos(), m_pTraceObject->GetTargetPos(), &brushTester) == CECIntelligentRoute::enumSearchSuccess){
				if (m_pHost->GetPos().y - CECIntelligentRoute::Instance().GetCurDest().y > 100)	{	//当玩家在空中很高的位置时不再进行自动寻路，防止寻路方向垂直而终止
					CECIntelligentRoute::Instance().ResetSearch();
					m_bUseAutoPF = false;
				}else{
					m_dwAutoPFNextCheckTime = GetTickCount() + 3000;
				}
			}else{
				m_bUseAutoPF = false;
			}
		}else{
			if (CECIntelligentRoute::Instance().IsPathFinished()){
				//	路径完成仍无法接触目标
				if (!CanTouch()){
					//	目标位置转移、切换到非自动寻路模式
					m_bUseAutoPF = false;
				}//else 其它原因，忽略
			}else{
				//	检查目标是否已经偏离位置较远了
				DWORD dwTime = GetTickCount();
				if (dwTime >= m_dwAutoPFNextCheckTime){	//	3 秒钟检查一次
					m_dwAutoPFNextCheckTime = dwTime + 3000;
					if (!m_pTraceObject->CanTouchFrom(CECIntelligentRoute::Instance().GetDest())){
						//	就算当前自动寻路结束、也无法接触目标了，重新再寻
						m_bUseAutoPF = true;
					}
				}
			}
		}
		break;
	}
	if (!m_bUseAutoPF){
		CECIntelligentRoute::Instance().ResetSearch();
	}
}

bool CECHPWorkTrace::CanTouch()
{
	CECSkill *pPrepSkill = m_pHost->m_pPrepSkill;
	CheckPrepSkill();
	bool result = m_pTraceObject->CanTouchFrom(m_pHost->GetPos());	//	因此函数调用可能依赖 m_pHost->m_pPrepSkill，因此需设置此值再查询，然后再恢复
	m_pHost->m_pPrepSkill = pPrepSkill;
	return result;
}

void CECHPWorkTrace::UpdateResetUseAutoPF(){
	if (!m_bShouldResetUseAutoPF){
		return;
	}
#ifdef SHOW_AUTOMOVE_FOOTPRINTS
	g_AutoPFFollowPoints.clear();
	g_AutoPFPathPoints.clear();
#endif
	CECIntelligentRoute::Instance().SetUsage(CECIntelligentRoute::enumUsageWorkTrace);
	CECIntelligentRoute::Instance().ResetSearch();
	SetUseAutoPF(m_bUseAutoPFResetValue);
	ClearResetUseAutoPF();
}

void CECHPWorkTrace::SetTraceTarget( CECTracedObject* pTraceObj, bool bUseAutoPF )
{
	ResetUseAutoPF(bUseAutoPF);
	if (!pTraceObj->GetTargetObject() || pTraceObj->GetObjectID() == m_pHost->GetCharacterID())
	{
		//	This is special case
		ReplaceTarget(pTraceObj);
		return;
	}
	int idTarget = pTraceObj->GetObjectID();
	if (!ISPLAYERID(idTarget) && !ISNPCID(idTarget) && !ISMATTERID(idTarget)){
		ASSERT(0);
		return;
	}
	CECObject* pObject = pTraceObj->GetTargetObject();
	if (!pObject){
		delete pTraceObj;
		return;
	}
	ReplaceTarget(pTraceObj);
	if (m_pTraceObject->GetTargetObject()){		
		A3DVECTOR3 vDirH = pTraceObj->GetTargetPos() - m_pHost->GetPos();
		vDirH.y = 0.0f;
		vDirH.Normalize();
		m_vCurDirH = !vDirH.IsZero() ? vDirH : m_vCurDirH = g_vAxisZ;
	}
}

CECTracedObject* CECHPWorkTrace::CreatTraceTarget(int iTraceObjId, int iReason, bool bForceAttack)
{
	if (ISPLAYERID(iTraceObjId)){
		return new CECTracedPlayer(TRACE_PLAYER, iTraceObjId, m_pHost, iReason, bForceAttack);
	}else if (ISNPCID(iTraceObjId)){
		return new CECTracedNPC(TRACE_NPC, iTraceObjId, m_pHost, iReason, bForceAttack);
	}else if (ISMATTERID(iTraceObjId)){
		return new CECTracedMatter(TRACE_MATTER, iTraceObjId, m_pHost, iReason);
	}
	return NULL;
}

void CECHPWorkTrace::ReplaceTarget( CECTracedObject* ptraceobj )
{
	if (m_pTraceObject)
	{
		delete m_pTraceObject;
	}
	m_pTraceObject = ptraceobj;
}

bool CECHPWorkTrace::IsGoodTimeToTouch()
{
	if (m_pHost->IsJumping()){
		return false;
	}
	return true;	
}

void CECHPWorkTrace::OnTargetMissing()
{
	StopMove(true);
	if ((m_pTraceObject->GetTraceType() == TRACE_NPC) || (m_pTraceObject->GetTraceType() == TRACE_PLAYER)){
		m_pTraceObject->OnTargetMissing();
	}else if(m_pTraceObject->GetTraceReason() == TRACE_SPELL){
		m_pHost->m_pPrepSkill = NULL;
	}
}

void CECHPWorkTrace::OnTouchTarget()
{
	StopMove(true);
	m_bActionDone = m_pTraceObject->OnTouched();
}
CECTracedObject::CECTracedObject(int type, int id, CECHostPlayer* pHost, int ireason)
:m_iTraceType(type)
,m_iObjectId(id)
,m_iReason(ireason)
{
	m_pHost = pHost;
	m_bMoreClose = false;
}

CECTracedObject::CECTracedObject( const CECTracedObject& rhs )
:m_iTraceType(rhs.m_iTraceType)
,m_iObjectId(rhs.m_iObjectId),
m_iReason(rhs.m_iReason),
m_bMoreClose(rhs.m_bMoreClose)
{
	m_pHost = rhs.m_pHost;
}

bool CECTracedObject::IsTargetMissing()
{
	if (m_iObjectId){
		CECObject* pObj = g_pGame->GetGameRun()->GetWorld()->GetObject(m_iObjectId, 0);
		if (pObj){
			return false;
		}
	}
	return true;
}

int CECTracedObject::GetObjectID()
{
	return m_iObjectId;
}

bool CECTracedObject::CanTouchMoreClose()
{
	return m_bMoreClose;
}

void CECTracedObject::SetMoveCloseFlag( bool bMoveClode )
{
	m_bMoreClose = bMoveClode; 
}

int CECTracedObject::GetTraceReason()
{
	return m_iReason;
}

CECObject* CECTracedObject::GetTargetObject()
{
	return g_pGame->GetGameRun()->GetWorld()->GetObject(m_iObjectId, 0);
}

bool CECTracedObject::CanTouchFrom( const A3DVECTOR3 &vHostPos )
{
	while (m_iObjectId)
	{
		A3DVECTOR3 vTargetPos(0.0f);
		
		vTargetPos = GetTargetPos();
		int iTouchReason = 0;
		if (m_iReason == CECHPWorkTrace::TRACE_ATTACK)
			iTouchReason = 1;
		else if (m_iReason == CECHPWorkTrace::TRACE_SPELL)
			iTouchReason = 2;
		else if (m_iReason == CECHPWorkTrace::TRACE_TALK)
			iTouchReason = 3;
		
		float fMaxCut = m_bMoreClose ? -1.0f : 1.0f;
		
		CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(m_iObjectId, 0);
		
		float fTouchRadius(0.0f);
		if (ISPLAYERID(m_iObjectId)){	
			if (m_iReason == CECHPWorkTrace::TRACE_TALK){
				fTouchRadius = 0.0f;
			}else{
				CECPlayer* pPlayer = dynamic_cast<CECPlayer*>(pObject);
				fTouchRadius = pPlayer->GetTouchRadius();
			}
			return m_pHost->CanTouchTarget(vHostPos, vTargetPos, fTouchRadius, iTouchReason, fMaxCut);
		}else if (ISNPCID(m_iObjectId)){
			CECNPC* pNPC = dynamic_cast<CECNPC*>(pObject);
			fTouchRadius = pNPC->GetTouchRadius();
			return m_pHost->CanTouchTarget(vHostPos, vTargetPos, fTouchRadius, iTouchReason, fMaxCut);
		}else if (ISMATTERID(m_iObjectId)){
			CECMatter* pMatter = dynamic_cast<CECMatter*>(pObject);
			return pMatter->CalcDist(vHostPos, true) < pMatter->GetGatherDist();
		}
		break;
	}
	return false;
}

A3DVECTOR3 CECTracedObject::GetMovePos()
{
	A3DVECTOR3 vMovePos(NULL);
	CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(m_iObjectId, 0);
	if (pObject){
		if (ISPLAYERID(m_iObjectId)){
			if (m_iObjectId == m_pHost->GetCharacterID()){
				vMovePos = m_pHost->GetPos();
			}else{
				vMovePos = dynamic_cast<CECElsePlayer*>(pObject)->GetServerPos();
			}
		}else if (ISNPCID(m_iObjectId)){
			vMovePos = dynamic_cast<CECNPC*>(pObject)->GetServerPos();
		}else{
			vMovePos = pObject->GetPos();
		}
	}
	return vMovePos;
}

int CECTracedObject::GetTraceType()
{
	return m_iTraceType;
}

CECTracedNPC::CECTracedNPC( const CECTracedNPC& rhs )
:CECTracedObject(rhs),
m_bForceAttack(rhs.m_bForceAttack)
{
	
}

CECTracedNPC::CECTracedNPC( int type, int id, CECHostPlayer* pHost, int ireason, bool bForceAttack )
:CECTracedObject(type, id, pHost, ireason),
m_bForceAttack(bForceAttack)
{
}

bool CECTracedNPC::OnTargetMissing()
{
	bool bRet = false;
	
	if (m_iReason == CECHPWorkTrace::TRACE_ATTACK){
		bRet = true;
	}else if (m_iReason == CECHPWorkTrace::TRACE_SPELL){
		CECSkill* pSkill = m_pHost->m_pPrepSkill;
		if (!pSkill || pSkill->GetTargetType() != 2){
			bRet = true;
			m_pHost->m_pPrepSkill = NULL;
		}
	}	
	return bRet;
}

A3DVECTOR3 CECTracedNPC::GetTargetPos()
{
	return dynamic_cast<CECNPC*>(GetTargetObject())->GetServerPos();
}

bool CECTracedNPC::OnTouched( )
{
	bool bActionDone(false);
	
	if (ISNPCID(m_iObjectId)){
		CECNPC* pNPC = (CECNPC*)GetTargetObject();

		if (m_iReason == CECHPWorkTrace::TRACE_TALK){
			if ((!m_pHost->IsInBattle() || m_pHost->InSameBattleCamp(pNPC)) &&
				!g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_SkillAction")->IsShow()){
				g_pGame->GetGameSession()->c2s_CmdNPCSevHello(m_iObjectId);
				bActionDone = true;
			}
		}
		else if (m_iReason == CECHPWorkTrace::TRACE_ATTACK){
			if (m_iObjectId == m_pHost->m_idSelTarget && 
				m_pHost->AttackableJudge(m_iObjectId, m_bForceAttack) == 1){
				BYTE byPVPMask = glb_BuildPVPMask(m_bForceAttack);
				g_pGame->GetGameSession()->c2s_CmdNormalAttack(byPVPMask);
				m_pHost->m_bPrepareFight = true;
				bActionDone = true;
			}
		}
		else if (m_iReason == CECHPWorkTrace::TRACE_SPELL){
			if (!m_pHost->CannotAttack()){
				if( m_pHost->CastSkill(m_iObjectId, m_bForceAttack) )
					bActionDone = true;
			}
			else
				m_pHost->m_pPrepSkill = NULL;
		}
	}
	return bActionDone;
}

CECTracedObject* CECTracedNPC::Clone() const
{
	return new CECTracedNPC(*this);
}

bool CECTracedNPC::IsTargetMissing()
{
	if (CECTracedObject::IsTargetMissing()){
		return true;
	}
	CECNPC* pNPC = dynamic_cast<CECNPC*>(GetTargetObject());
	if (pNPC->IsDead()){
		return true;
	}
	return false;

}
CECTracedPlayer::CECTracedPlayer( const CECTracedPlayer& rhs )
:CECTracedObject(rhs),
m_bForceAttack(rhs.m_bForceAttack)
{
	
}

CECTracedPlayer::CECTracedPlayer(int type, int id, CECHostPlayer* pHost, int ireason, bool bForceAttack )
:CECTracedObject(type, id, pHost, ireason),
m_bForceAttack(bForceAttack)
{

}

bool CECTracedPlayer::OnTargetMissing()
{
	bool bRet = false;
	
	if (m_iReason == CECHPWorkTrace::TRACE_ATTACK){
		bRet = true;
	}
	else if (m_iReason == CECHPWorkTrace::TRACE_SPELL){
		CECSkill* pSkill = m_pHost->m_pPrepSkill;
		if (!pSkill || pSkill->GetTargetType() != 2){
			bRet = true;
			m_pHost->m_pPrepSkill = NULL;
		}
	}	
	return bRet;
}

A3DVECTOR3 CECTracedPlayer::GetTargetPos()
{
	CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(m_iObjectId, 0);
	if (m_iObjectId == m_pHost->GetCharacterID()){
		return m_pHost->GetPos();
	}else{
		return dynamic_cast<CECElsePlayer*>(pObject)->GetServerPos();
	}
}

bool CECTracedPlayer::OnTouched( )
{
	bool bActionDone(false);
	if (ISPLAYERID(m_iObjectId)){
		if (m_iObjectId == 0 || m_iObjectId == m_pHost->GetCharacterID()){
			//	Handle special case
			ASSERT(m_iReason == CECHPWorkTrace::TRACE_SPELL);
			if (!m_pHost->CannotAttack()){
				if( m_pHost->CastSkill(m_iObjectId, m_bForceAttack, NULL) )
					bActionDone = true;
			}else{
				m_pHost->m_pPrepSkill = NULL;
			}
			return bActionDone;
		}
		if (m_iReason == CECHPWorkTrace::TRACE_ATTACK){
			if (m_iObjectId == m_pHost->m_idSelTarget && 
				m_pHost->AttackableJudge(m_iObjectId, m_bForceAttack) == 1)	{
				BYTE byPVPMask = glb_BuildPVPMask(m_bForceAttack);
				g_pGame->GetGameSession()->c2s_CmdNormalAttack(byPVPMask);
				m_pHost->m_bPrepareFight = true;
				bActionDone = true;
			}
		}
		else if (m_iReason == CECHPWorkTrace::TRACE_SPELL){
			if (!m_pHost->CastSkill(m_iObjectId, m_bForceAttack, GetTargetObject())){
				m_pHost->m_pPrepSkill = NULL;
			}else{
				bActionDone = true;
			}
		}
		else if (m_iReason == CECHPWorkTrace::TRACE_TALK){
			//	Visite other's booth, send hello message to him
			g_pGame->GetGameSession()->c2s_CmdNPCSevHello(m_iObjectId);
			bActionDone = true;
		}
	}
	return bActionDone;
}

CECTracedObject* CECTracedPlayer::Clone() const
{
	return new CECTracedPlayer(*this);
}

bool CECTracedPlayer::IsTargetMissing()
{
	if (CECTracedObject::IsTargetMissing()){
		return true;
	}
	CECPlayer* pPlayer = dynamic_cast<CECPlayer*>(GetTargetObject());
	if (pPlayer->IsElsePlayer()){
		if (pPlayer->IsDead()){
			if (m_iReason == CECHPWorkTrace::TRACE_SPELL){
				CECSkill* pSkill = m_pHost->m_pPrepSkill;
				if (pSkill && pSkill->GetTargetType() == 2){
					return false;
				}
			}
			return true;
		}
	}
	return false;
}
CECTracedMatter::CECTracedMatter( const CECTracedMatter& rhs )
:CECTracedObject(rhs)
{
	
}

CECTracedMatter::CECTracedMatter(int type, int id, CECHostPlayer* pHost, int ireason )
:CECTracedObject(type, id, pHost, ireason)
{
}

bool CECTracedMatter::OnTargetMissing()
{
	return false;	
}

A3DVECTOR3 CECTracedMatter::GetTargetPos()
{
	return GetTargetObject()->GetPos();
}

bool CECTracedMatter::OnTouched( )
{
	bool bActionDone(false);
	if (ISMATTERID(m_iObjectId)){		
		if(m_pHost->GetProfession() == PROF_GHOST && m_pHost->IsInvisible()){
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CANNOT_USE_WHEN_INVISIBLE);
			return bActionDone;
		}
		CECMatter* pMatter = (CECMatter*)GetTargetObject();

		if (m_iReason == CECHPWorkTrace::TRACE_PICKUP){
			//	Check whether we have enougth place to hold this item or money
			if (m_pHost->CanTakeItem(pMatter->GetTemplateID(), 1)){
				//	Send pickup asking and wait response command
				g_pGame->GetGameSession()->c2s_CmdPickup(m_iObjectId, pMatter->GetTemplateID());
				bActionDone = true;
			}else{
				//	Print a notify message
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_PACKISFULL);
			}
		}else{	//	m_iReason == TRACE_GATHER
			int tidMatter = pMatter->GetTemplateID();

			//	Check mine level requirement
			if (m_pHost->GetBasicProps().iLevel < pMatter->GetLevelReq()){
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_LEVELTOOLOW);
				return bActionDone;
			}

			//	Check whether we have a mine tool
			int iPack, iIndex, idTool;
			if (m_pHost->FindMineTool(tidMatter, &iPack, &iIndex, &idTool))	{
				DATA_TYPE DataType;
				const MINE_ESSENCE* pData = (const MINE_ESSENCE*)g_pGame->GetElementDataMan()->get_data_ptr(pMatter->GetTemplateID(), ID_SPACE_ESSENCE, DataType);
				if (DataType != DT_MINE_ESSENCE){
					ASSERT(DataType == DT_MINE_ESSENCE);
					return bActionDone;
				}

				if (m_pHost->GetCoolTime(GP_CT_PLAYER_GATHER)){
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
				}else{
					//	Send gather asking and wait response command
					g_pGame->GetGameSession()->c2s_CmdGatherMaterial(m_iObjectId, iPack, iIndex, idTool, pData->task_in);
				}
			}else{
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_NEEDTOOL);
			}
		}
	}
	return bActionDone;
}

CECTracedObject* CECTracedMatter::Clone() const
{
	return new CECTracedMatter(*this);
}

CECTracedTaskNPC::CECTracedTaskNPC( int type, int id, CECHostPlayer* pHost, int taskid )
:CECTracedNPC(type, id, pHost, CECHPWorkTrace::TRACE_TALK),
m_iTaskId(taskid)
{
	
}

CECTracedTaskNPC::CECTracedTaskNPC( const CECTracedTaskNPC& rhs )
:CECTracedNPC(rhs),
m_iTaskId(rhs.m_iTaskId)
{

}

bool CECTracedTaskNPC::OnTouched( )
{
	bool bActionDone(false);
	
	if (ISNPCID(m_iObjectId)){
		CECNPC* pNPC = (CECNPC*)GetTargetObject();
		
		if (m_iReason == CECHPWorkTrace::TRACE_TALK){
			if ((!m_pHost->IsInBattle() || m_pHost->InSameBattleCamp(pNPC)) &&
				!g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_SkillAction")->IsShow()){
				g_pGame->GetGameSession()->c2s_CmdNPCSevHello(m_iObjectId);
				CDlgNPC* pDlgNPC = (CDlgNPC*)g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_NPC");
				pDlgNPC->SetTraceTaskInfo(m_iObjectId,m_iTaskId);
				bActionDone = true;
			}
		}
	}
	return bActionDone;
}

CECTracedObject* CECTracedTaskNPC::Clone() const
{
	return new CECTracedTaskNPC(*this);
}
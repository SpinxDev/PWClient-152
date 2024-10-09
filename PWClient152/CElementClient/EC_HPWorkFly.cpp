/*
 * FILE: EC_HPWorkFly.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_HPWorkFly.h"
#include "EC_HostPlayer.h"
#include "EC_Configs.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_FullGlowRender.h"
#include "EC_ComboSkill.h"
#include "EC_CDR.h"
#include "EC_Utility.h"
#include "EC_World.h"

#include "EC_UIManager.h"
#include "EC_GameUIMan.h"

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

static const DWORD fly_prepare = 420;
static const DWORD fly_launch = 1400;
static const float lauch_hei = 2.f;
static const float launch_vel = lauch_hei * 1000.0f / fly_prepare;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkFly
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkFly::CECHPWorkFly(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_FLYOFF, pWorkMan)
{
	m_dwMask		= MASK_FLYOFF;
	m_dwTransMask	= MASK_STAND | MASK_MOVETOPOS;

	CECHPWorkFly::Reset();
}

CECHPWorkFly::~CECHPWorkFly()
{
}

//	Reset work
void CECHPWorkFly::Reset()
{
	CECHPWork::Reset();

	m_dwLaunchTime	= 0;
	m_bAddSpeed		= true;
}

//	Copy work data
bool CECHPWorkFly::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkFly* pSrc = (CECHPWorkFly*)pWork;

	m_vDestPos		= pSrc->m_vDestPos;
	m_dwLaunchTime	= pSrc->m_dwLaunchTime;
	m_bContinueFly	= pSrc->m_bContinueFly;
	m_bAddSpeed		= pSrc->m_bAddSpeed;

	return true;
}

//	Work is cancel
void CECHPWorkFly::Cancel()
{
	m_pHost->m_CDRInfo.vAbsVelocity.Clear();
	m_pHost->m_CDRInfo.fYVel = 0.0f;
	CECHPWork::Cancel();
}

//	On first tick
void CECHPWorkFly::OnFirstTick()
{
	m_pHost->m_iMoveEnv		= CECPlayer::MOVEENV_AIR;
	m_pHost->m_iMoveMode	= CECPlayer::MOVE_MOVE;

	m_pHost->m_vVelocity.Clear();
	m_pHost->m_vAccel.Clear();
	m_pHost->ResetJump();

	m_vDestPos = m_pHost->GetPos();
	m_vDestPos.y += lauch_hei;

	// for test: solving the pull down problem, by sxw 2009-09-03
	m_bAddSpeed = false;
	if(m_pHost->m_GndInfo.fGndHei > m_pHost->m_GndInfo.fWaterHei)
	{
		if(m_pHost->GetPos().y - m_pHost->m_GndInfo.fGndHei < 0.2f)
			m_bAddSpeed = true;
	}
	else
		m_bAddSpeed = true;

	m_pHost->ShowWing(true);

	//if (m_pHost->UsingWing())
	if (m_pHost->GetWingType() == WINGTYPE_WING)
		m_pHost->PlayAction(CECPlayer::ACT_TAKEOFF);
	else
		m_pHost->PlayAction(CECPlayer::ACT_TAKEOFF_SWORD);
}

//	Tick routine
bool CECHPWorkFly::Tick(DWORD dwDeltaTime)
{
	CECHPWork::Tick(dwDeltaTime);
	m_dwLaunchTime += dwDeltaTime;

	if (m_dwLaunchTime < fly_prepare)
	{
		if (m_bContinueFly)
			return true;
		else
		{
			float fDeltaTime = dwDeltaTime * 0.001f;
		//	A3DVECTOR3 vCurPos = m_pHost->m_MoveCtrl.AirWaterMove(g_vAxisY * launch_vel, fDeltaTime, true);
			float fVerSpeed = m_bAddSpeed ? launch_vel : 0.0f;
			m_bAddSpeed = false;
			A3DVECTOR3 vCurPos = m_pHost->m_MoveCtrl.GroundMove(g_vOrigin, 0.0f, fDeltaTime, fVerSpeed, 0.0f);
			m_pHost->SetPos(vCurPos);
		}
	}
	else if (m_dwLaunchTime < fly_launch && !m_bContinueFly)
		return true;
	else
	{
		m_bFinished = true;
		A3DVECTOR3 vCurPos = m_pHost->GetPos();
		m_pHost->m_MoveCtrl.SendStopMoveCmd(vCurPos, 0, GP_MOVE_AIR | GP_MOVE_RUN);
		m_pHost->PlayAction(
			m_pHost->GetWingType() == WINGTYPE_WING/*m_pHost->UsingWing()*/ ? CECPlayer::ACT_HANGINAIR : CECPlayer::ACT_HANGINAIR_SWORD,
			true,
			300);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkFMove
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkFMove::CECHPWorkFMove(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_FLASHMOVE, pWorkMan)
{
	m_dwMask		= MASK_FLYOFF;
	m_dwTransMask	= MASK_STAND;

	CECHPWorkFMove::Reset();
}

CECHPWorkFMove::~CECHPWorkFMove()
{
}

//	Reset work
void CECHPWorkFMove::Reset()
{
	CECHPWork::Reset();

	m_fSpeed	= 0.0f;
	m_fDist		= 0.0f;
	m_fDistCnt	= 0.0f;
	m_iFMoveSkillID = 0;
}

//	Copy work data
bool CECHPWorkFMove::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkFMove* pSrc = (CECHPWorkFMove*)pWork;

	m_vDestPos	= pSrc->m_vDestPos;
	m_fSpeed	= pSrc->m_fSpeed;
	m_fDist		= pSrc->m_fDist;
	m_fDistCnt	= pSrc->m_fDistCnt;
	m_vMoveDir	= pSrc->m_vMoveDir;
	m_iFMoveSkillID = pSrc->m_iFMoveSkillID;
	return true;
}

//	Prepare to move
void CECHPWorkFMove::PrepareMove(const A3DVECTOR3& vDestPos, float fMoveTime, int iFMoveSkillID)
{
	ASSERT(fMoveTime > 0.0f);
	a_ClampFloor(fMoveTime, 0.001f * 50);

	m_vDestPos	= vDestPos;
	m_fDistCnt	= 0.0f;
	m_vMoveDir	= vDestPos - m_pHost->GetPos();
	m_fDist		= m_vMoveDir.Normalize();
	m_fSpeed	= m_fDist / fMoveTime;
	m_iFMoveSkillID = iFMoveSkillID;
}

//	On first tick
void CECHPWorkFMove::OnFirstTick()
{
	m_pHost->SetPos(m_pHost->GetPos() + A3DVECTOR3(0.0f, 0.7f, 0.0f));
	m_pHost->m_MoveCtrl.SetHostLastPos(m_pHost->GetPos() + A3DVECTOR3(0.0f, 0.7f, 0.0f));

	if(m_iFMoveSkillID > 0)
		m_pHost->PlayAttackEffect(m_pHost->GetSelectedTarget(), m_iFMoveSkillID, 0, -2, 0, 0);
	else
	{
	 	m_pHost->PlayAction(CECPlayer::ACT_JUMP_START, true, 0);
		m_pHost->PlayAction(CECPlayer::ACT_JUMP_LOOP, false, 0, true);
	}

	CECFullGlowRender * pGlow = g_pGame->GetGameRun()->GetFullGlowRender();
	if( pGlow && pGlow->IsGlowOn() )
	{
		pGlow->SetMotionBlur(0.8f, 0.8f, 1);
	}
}

//	Tick routine
bool CECHPWorkFMove::Tick(DWORD dwDeltaTime)
{
	CECHPWork::Tick(dwDeltaTime);
	float fMoveDelta = m_fSpeed * dwDeltaTime * 0.001f;
	A3DVECTOR3 vCurPos = m_pHost->GetPos();

	if (m_fDistCnt + fMoveDelta >= m_fDist)
	{
		fMoveDelta = m_fDist - m_fDistCnt;
		m_fDistCnt = m_fDist;

		m_pHost->SetPos(m_vDestPos);
		m_pHost->m_MoveCtrl.Reset();
		m_pHost->m_MoveCtrl.SetLastSevPos(m_vDestPos);
		m_pHost->m_MoveCtrl.SetHostLastPos(m_vDestPos);

		if (!m_pHost->m_CDRInfo.vTPNormal.IsZero())
			m_pHost->SetGroundNormal(m_pHost->m_CDRInfo.vTPNormal);

		CECFullGlowRender* pGlow = g_pGame->GetGameRun()->GetFullGlowRender();
		if (pGlow && pGlow->IsGlowOn())
			pGlow->SetMotionBlur(0.8f, 0.0f, 3000);

		//	Finish work
		Finish();
	}
	else
	{
		m_fDistCnt += fMoveDelta;

		vCurPos += m_vMoveDir * fMoveDelta;
		m_pHost->SetPos(vCurPos);
		m_pHost->m_MoveCtrl.SetHostLastPos(vCurPos);
	}

	return true;
}

//	Finish work
void CECHPWorkFMove::Finish()
{
	m_bFinished = true;

	CECComboSkill* pcs = m_pHost->m_pComboSkill;
	if (pcs && !pcs->IsStop())
		pcs->Continue(false);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkPassiveMove
//	
///////////////////////////////////////////////////////////////////////////

CECHPWorkPassiveMove::CECHPWorkPassiveMove(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_PASSIVEMOVE, pWorkMan)
{
	CECHPWorkPassiveMove::Reset();
}

CECHPWorkPassiveMove::~CECHPWorkPassiveMove()
{
}

//	Reset work
void CECHPWorkPassiveMove::Reset()
{
	CECHPWork::Reset();

	m_fSpeed	= 0.0f;
	m_nMoveTime = 0;
	m_nMoveTimeCnt = 0;
	m_nWaitTime = 0;
	m_nWaitTimeCnt = 0;
}

//	Copy work data
bool CECHPWorkPassiveMove::CopyData(CECHPWork* pWork)
{
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkPassiveMove* pSrc = (CECHPWorkPassiveMove*)pWork;

	m_vStartPos	= pSrc->m_vStartPos;
	m_vDestPos	= pSrc->m_vDestPos;
	m_vMoveDir	= pSrc->m_vMoveDir;
	m_fSpeed	= pSrc->m_fSpeed;
	m_nMoveTime		= pSrc->m_nMoveTime;
	m_nMoveTimeCnt	= pSrc->m_nMoveTimeCnt;
	m_nWaitTime		= pSrc->m_nWaitTime;
	m_nWaitTimeCnt	= pSrc->m_nWaitTimeCnt;
	return true;
}

//	Prepare to move
void CECHPWorkPassiveMove::PrepareMove(const A3DVECTOR3& vDestPos, int nMoveTime)
{
	ASSERT(nMoveTime > 0);
	a_ClampFloor(nMoveTime, 50);
	A3DVECTOR3 vCurPos = m_pHost->GetPos();

	m_vStartPos	= vCurPos;
	m_vDestPos	= vDestPos;
	m_vMoveDir	= vDestPos - vCurPos;

	//	�ƶ��ٶȱ��ֲ��䣨���ȴ�ʱ���޹أ�������Ϊ�ȴ�ʱ�䳤�����ƶ��ٶȱ�����
	float fDist	= m_vMoveDir.Normalize();
	m_fSpeed	= fDist / (nMoveTime * 0.001f);

	//	�����ƶ�ʱ�䡢�ȴ�ʱ�䡢�ƶ�λ��
	m_nMoveTime = nMoveTime;
	m_nWaitTime = nMoveTime - m_nMoveTime;
	m_nWaitTimeCnt = 0;
	m_nMoveTimeCnt	= 0;
}

//	On first tick
void CECHPWorkPassiveMove::OnFirstTick()
{
	m_pHost->PlayAction(CECPlayer::ACT_STRIKEBACK, true, 0);

	CECFullGlowRender * pGlow = g_pGame->GetGameRun()->GetFullGlowRender();
	if( pGlow && pGlow->IsGlowOn() )
	{
		pGlow->SetMotionBlur(0.8f, 0.8f, 1);
	}
}

bool CalculateNoBrushCollidePos(int nMaxTryTimes, float fStep, float fMaxMove, bool bDownward, const A3DVECTOR3 &vCurPos, const A3DVECTOR3 &vExt, A3DVECTOR3 &vPos)
{
	//	nMaxTryTimes:	��ೢ�Դ���
	//	fStep:			ÿ�γ�������ֱ�����ϵ��ƶ�����ʼ��Ϊ����
	//	fMaxMove:		��ೢ�Ծ��루ʼ��Ϊ����
	//	bDownward:		�Ƿ������ƶ�
	//	vCurPos:		��ʼ�ο�λ��
	//	vExt:			��Χ�д�С
	//	vPos:			�������ͻλ�ã�����Ϊ true ��Ч)

	//	����ο�λ����ֱ����ķǳ�ͻ��
	//

	bool bOK(false);

	env_trace_t trcInfo;
	trcInfo.vExt = vExt;
	trcInfo.dwCheckFlag = CDR_BRUSH;

	A3DVECTOR3 vStart = vCurPos + g_vAxisY*vExt.y;	//	��Χ�����Ĳο�λ��

	int nTry = 0;			//	�����ܹ����Դ���
	float fSum = 0;			//	���ҹ������ۻ����³��Եľ���
	while (++nTry < nMaxTryTimes)
	{
		fSum += fStep;
		a_ClampRoof(fSum, fMaxMove);
		
		//	�����ɢ���Ƿ��ͻ
		trcInfo.vStart = vStart;
		trcInfo.vStart.y -= fSum * (bDownward ? 1 : -1);
		trcInfo.vDelta.Clear();
		if (!CollideWithEnv(&trcInfo))
		{
			//	�ҵ�����ͻλ��
			bOK = true;
			
			//	���������·�����ͻλ����ԭλ�ò��ң�ϣ����þ�ȷλ��
			trcInfo.vDelta.y = (bDownward ? fSum : -fSum);
			CollideWithEnv(&trcInfo);
			vPos = vCurPos;
			vPos.y -= (1.0f-trcInfo.fFraction) * fSum * (bDownward?1:-1);
			break;
		}
		
		//	û�ҵ�
		
		//	����Ƿ��Ѵ����λ��
		if (fabs(fSum-fMaxMove) <= 0.001f)
		{
			//	���������ҵ�
			break;
		}
	}

	return bOK;
}

//	Tick routine
bool CECHPWorkPassiveMove::Tick(DWORD dwDeltaTime)
{
	CECHPWork::Tick(dwDeltaTime);
	
	//	���㱾�ε��ƶ�ʱ�䡢�ȴ�ʱ��
	int nMoveTime = dwDeltaTime;
	int nWaitTime = 0;
	if (nMoveTime + m_nMoveTimeCnt > m_nMoveTime)
	{
		nMoveTime = m_nMoveTime-m_nMoveTimeCnt;
		nWaitTime = dwDeltaTime-nMoveTime;
		if (nWaitTime + m_nWaitTimeCnt > m_nWaitTime)
			nWaitTime = m_nWaitTime-m_nWaitTimeCnt;
	}
	
	if (nMoveTime > 0)
	{
		//	��Ҫ�ƶ�

		//	�����ƶ���ľ���
		float fMoveDelta = m_fSpeed * (m_nMoveTimeCnt + nMoveTime) * 0.001f;
		A3DVECTOR3 vCurPos = m_vStartPos;
		vCurPos += m_vMoveDir * fMoveDelta;

		//	���ݵ��ε�����Ҹ߶ȣ���֤�ڵ�������
		float terrianHeight = g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(vCurPos);
		a_ClampFloor(vCurPos.y, terrianHeight);
		
		A3DVECTOR3 vExt = m_pHost->m_CDRInfo.vExtent;
		A3DVECTOR3 vStart = vCurPos + g_vAxisY * vExt.y;

		//	��鵱ǰλ���Ƿ��͹����ͻ
		
		env_trace_t  trcInfo;
		trcInfo.dwCheckFlag = CDR_BRUSH;
		trcInfo.vExt = vExt;
		trcInfo.vStart = vStart;
		trcInfo.vDelta.Clear();

		if (CollideWithEnv(&trcInfo))
		{
			//	��ͻ����������ֱ������е���������޷���������ֻ�б��ֳ�ͻλ�ã�

			//	�ȳ�������ֱ�����ҵ�����ͻ��

			//	���峢�Գ���
			const int MAX_TRY_TIMES = 10;
			const float STEP_SIZE = 0.1f;

			//	����һ�������������Ѱ�ң��ҵ����µĲ���ͻ��
			bool bDownOK(false);		//	�ҵ��·��Ĳ���ͻ��
			A3DVECTOR3 vDownPos;		//	��ͻ�����Чλ��
			float fMaxDown = vCurPos.y - terrianHeight;	//	���Ծ�������ֵ�����ܴ����������£�
			bDownOK = CalculateNoBrushCollidePos(MAX_TRY_TIMES, STEP_SIZE, fMaxDown, true, vCurPos, vExt, vDownPos);

			//	���Զ����ҵ����ϵĲ���ͻ��
			bool bUpOK(false);
			A3DVECTOR3 vUpPos;
			bUpOK = CalculateNoBrushCollidePos(MAX_TRY_TIMES, STEP_SIZE, 1.0f, false, vCurPos, vExt, vUpPos);

			//	�ӽ���н���ѡ��
			if (bDownOK && bUpOK)
			{
				float fDistDown = vCurPos.y - vDownPos.y;
				float fDistUp = vUpPos.y - vCurPos.y;
				if (fabs(fDistDown) < fabs(fDistUp))
				{
					//	�������·���
					vCurPos = vDownPos;

#ifdef _DEBUG
					//	���������Ϣ
					ACString strMsg;
					strMsg.Format(_AL("Down2:%f,%f,%f"), vDownPos.x, vDownPos.y, vDownPos.z);
					g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->AddChatMessage(strMsg, GP_CHAT_MISC);
#endif
				}
				else
				{
					//	�������Ϸ���
					vCurPos = vUpPos;
#ifdef _DEBUG
					//	���������Ϣ
					ACString strMsg;
					strMsg.Format(_AL("Up2:%f,%f,%f"), vUpPos.x, vUpPos.y, vUpPos.z);
					g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->AddChatMessage(strMsg, GP_CHAT_MISC);
#endif
				}
			}
			else if (bDownOK)
			{
				//	ֻ�в������·���
				vCurPos = vDownPos;
				
#ifdef _DEBUG
				//	���������Ϣ
				ACString strMsg;
				strMsg.Format(_AL("Down1:%f,%f,%f"), vDownPos.x, vDownPos.y, vDownPos.z);
				g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->AddChatMessage(strMsg, GP_CHAT_MISC);
#endif
			}
			else if (bUpOK)
			{
				//	ֻ�в������Ϸ���
				vCurPos = vUpPos;	
#ifdef _DEBUG
				//	���������Ϣ
				ACString strMsg;
				strMsg.Format(_AL("Up1:%f,%f,%f"), vUpPos.x, vUpPos.y, vUpPos.z);
				g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->AddChatMessage(strMsg, GP_CHAT_MISC);
#endif
			}
			//	else �Ҳ�������ͻλ�ã���ֻ�б���ԭλ��
		}

		//	��¼��λ��
		m_pHost->SetPos(vCurPos);
		m_pHost->m_MoveCtrl.SetHostLastPos(vCurPos);

		//	��¼ʱ��
		m_nMoveTimeCnt += nMoveTime;
	}

	if (nWaitTime > 0)
	{
		//	��Ҫ�ȴ����������ͬ����
		m_nWaitTimeCnt += nWaitTime;
	}

	if (m_nMoveTimeCnt >= m_nMoveTime &&
		m_nWaitTimeCnt >= m_nWaitTime)
	{
		//	�������

		const A3DVECTOR3 &vCurPos = m_pHost->GetPos();

		m_pHost->m_MoveCtrl.Reset();
		m_pHost->m_MoveCtrl.SetLastSevPos(vCurPos);
		m_pHost->m_MoveCtrl.SetHostLastPos(vCurPos);

		CECFullGlowRender* pGlow = g_pGame->GetGameRun()->GetFullGlowRender();
		if (pGlow && pGlow->IsGlowOn())
			pGlow->SetMotionBlur(0.8f, 0.0f, 3000);

		Finish();
	}

	return true;
}

//	Finish work
void CECHPWorkPassiveMove::Finish()
{
	m_bFinished = true;
}
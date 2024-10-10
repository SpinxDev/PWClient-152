#ifndef ECMANSKILLGFX_H_
#define ECMANSKILLGFX_H_

#include "AList2.h"
#include "A3DGfxEx.h"
#include "A3DGfxExMan.h"
#include "A3DSkillGfxEvent2.h"
#include "EC_Manager.h"
#include "EC_Game.h"
#include "EC_World.h"
#include "EC_GameRun.h"
#include "EC_GPDataType.h"
#include "EC_Player.h"
#include "EC_ManPlayer.h"
#include "EC_ManNPC.h"
#include "EC_NPC.h"
#include "EC_ManAttacks.h"

#define	DEFAULT_EVENT_BUF_SIZE	10

extern CECGame* g_pGame;

class CECSkillGfxEvent : public A3DSkillGfxEvent
{
public:
	CECSkillGfxEvent(GfxMoveMode mode);
	~CECSkillGfxEvent() {}

protected:
	CECPlayerMan*	m_pPlayerMan;
	CECNPCMan*		m_pNPCMan;

protected:
	void HitTarget(const A3DVECTOR3& vTarget);
	friend class CECSkillGfxMan;

	A3DSkillGfxComposer* GetComposer() const;
	clientid_t	GetOriginalHost()const;
	clientid_t	GetOriginalTarget()const;

public:
	void Tick(DWORD dwDeltaTime);
	A3DGFXEx* LoadHitGfx(A3DDevice* pDev, const char* szPath);
	void SetHitGfx(A3DGFXEx* pHitGfx);
	virtual A3DVECTOR3 GetTargetCenter();
	virtual bool GetTargetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp);
};

inline A3DGFXEx* CECSkillGfxEvent::LoadHitGfx(A3DDevice* pDev, const char* szPath)
{
	if (m_dwModifier & CECAttackEvent::MOD_NULLITY) szPath = "程序联入\\击中\\无效攻击击中.gfx";
	return AfxGetGFXExMan()->LoadGfx(pDev, szPath);
}

inline void CECSkillGfxEvent::SetHitGfx(A3DGFXEx* pHitGfx)
{
//	if (m_dwModifier & CECAttackEvent::MOD_CRITICAL_STRIKE) pHitGfx->SetActualScale(pHitGfx->GetScale() * 2.0f);
	m_pHitGfx = pHitGfx;
}

class SkillGfxMan : public A3DSkillGfxMan
{
public:
	SkillGfxMan(CECGameRun* pGameRun);
	~SkillGfxMan() {}

protected:
	friend class CECSkillGfxEvent;
	typedef APtrList<CECSkillGfxEvent*> SkillGfxEventLst;

	SkillGfxEventLst m_EventLst;
	SkillGfxEventLst m_FreeLst[enumMoveModeNum];

	CECPlayerMan* m_pPlayerMan;
	CECNPCMan* m_pNPCMan;

protected:
	
	virtual bool GetPropertyById(clientid_t nId, ECMODEL_GFX_PROPERTY* pProperty);

public:
	A3DSkillGfxEvent* GetEmptyEvent(GfxMoveMode mode)
	{
		if (m_FreeLst[static_cast<int>(mode)].GetCount() == 0)
			return new CECSkillGfxEvent(mode);
		else
		{
			A3DSkillGfxEvent* pEvent = m_FreeLst[static_cast<int>(mode)].RemoveTail();
			pEvent->Resume();
			return pEvent;
		}
	}
	void PushEvent(A3DSkillGfxEvent* pEvent) { m_EventLst.AddTail(static_cast<CECSkillGfxEvent*>(pEvent)); }
	float GetTargetScale(clientid_t nTargetId);
	void Release()
	{
		ALISTPOSITION pos = m_EventLst.GetHeadPosition();

		while (pos) delete m_EventLst.GetNext(pos);
		m_EventLst.RemoveAll();

		for (int i = 0; i < enumMoveModeNum; i++)
		{
			pos = m_FreeLst[i].GetHeadPosition();

			while (pos) delete m_FreeLst[i].GetNext(pos);
			m_FreeLst[i].RemoveAll();
		}
	}
	bool Tick(DWORD dwDeltaTime)
	{
		ALISTPOSITION pos = m_EventLst.GetHeadPosition();

		while (pos)
		{
			ALISTPOSITION posCur = pos;
			CECSkillGfxEvent* pEvent = m_EventLst.GetNext(pos);

			if (pEvent->IsFinished())
			{
				m_EventLst.RemoveAt(posCur);
				pEvent->Resume();
				m_FreeLst[pEvent->GetMode()].AddTail(pEvent);
			}
			else
				pEvent->Tick(dwDeltaTime);
		}

		return true;
	}
	bool Render(CECViewport* pViewport)
	{
		ALISTPOSITION pos = m_EventLst.GetHeadPosition();
		while (pos) m_EventLst.GetNext(pos)->Render();
		return true;
	}
};

class CECSkillGfxMan : public CECManager
{
public:
	CECSkillGfxMan(CECGameRun* pGameRun);
	~CECSkillGfxMan() {}

protected:
	SkillGfxMan m_GfxMan;

public:
	A3DSkillGfxMan* GetPtr() { return &m_GfxMan; }
	virtual void Release() { m_GfxMan.Release(); }
	virtual bool Tick(DWORD dwDeltaTime) { return m_GfxMan.Tick(dwDeltaTime); }
	virtual bool Render(CECViewport* pViewport) { return m_GfxMan.Render(pViewport); }
	bool AddSkillGfxEvent(
		int nHostID,
		int nTargetID,
		const char* szFlyGfx,
		const char* szHitGfx,
		DWORD dwFlyTimeSpan,
		bool bTraceTarget = true,
		GfxMoveMode FlyMode = enumLinearMove,
		int nFlyGfxCount = 1,
		DWORD dwInterval = 0,
		const GFX_SKILL_PARAM* param = NULL,
		float fFlyGfxScale = 1.0f,
		float fHitGfxScale = 1.0f,
		DWORD dwModifier = 0
		)
	{
		return m_GfxMan.AddSkillGfxEvent(
			NULL,
			nHostID,
			nTargetID,
			szFlyGfx,
			szHitGfx,
			dwFlyTimeSpan,
			bTraceTarget,
			FlyMode,
			nFlyGfxCount,
			dwInterval,
			param,
			fFlyGfxScale,
			fHitGfxScale,
			dwModifier
			);
	}
};

inline CECSkillGfxEvent::CECSkillGfxEvent(GfxMoveMode mode) : A3DSkillGfxEvent(mode)
{
	m_pPlayerMan	= g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
	m_pNPCMan		= g_pGame->GetGameRun()->GetWorld()->GetNPCMan();
}

#endif

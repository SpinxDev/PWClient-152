#include "EC_Global.h"
#include "EC_ManSkillGfx.h"
#include "EC_GFXCaster.h"
#include "EC_ManAttacks.h"
#include "EC_Goblin.h"
#include "EC_Optimize.h"
#include "EC_Model.h"
#include <A3DSkeleton.h>

inline bool _get_pos_by_id(
	CECPlayerMan* pPlayerMan,
	CECNPCMan* pNPCMan,
	int nID,
	A3DVECTOR3& vPos,
	GfxHitPos HitPos,
	bool bIsGoblinSkill = false,
	//	added 09.9.29 -- usage : to specify the host's and target's hook to set the fly gfx and hit gfx
	const char* szHook = NULL, 
	bool bRelHook = false, 
	const A3DVECTOR3* pOffset = NULL, 
	const char* szHanger = NULL, 
	bool bChildHook = false)
{
	if (ISPLAYERID(nID))
	{
		CECPlayer* pPlayer = pPlayerMan->GetPlayer(nID);

		if (pPlayer)
		{
			if(bIsGoblinSkill)
			{
				if(pPlayer->GetGoblinModel())
				{
					vPos = pPlayer->GetGoblinModel()->GetModel()->GetModelAABB().Center;
				}
				else
					return false;
			}
			else
			{
				// currently hook does not affect the Goblin Skill
				while (1)
				{
					if (!szHook)
						break;

					CECModel* pModel = pPlayer->GetPlayerModel();
					if (!pModel)
						break;
					
					if (szHanger && bChildHook)
						pModel = pModel->GetChildModel(szHanger);
					
					if (!pModel)
						break;

					
					A3DSkinModel* pSkin = pModel->GetA3DSkinModel();
					A3DSkeletonHook* pHook = pSkin->GetSkeletonHook(szHook, true);
					
					if (!pHook)
						break;

					if (bRelHook)
						vPos = pHook->GetAbsoluteTM() * (*pOffset);
					else
					{
						vPos = pSkin->GetAbsoluteTM() * (*pOffset);
						vPos = vPos - pSkin->GetAbsoluteTM().GetRow(3) + pHook->GetAbsoluteTM().GetRow(3);
					}
					
					return true;
				}


				if (HitPos == enumHitBottom)
					vPos = pPlayer->GetPos();
				else
				{
					const A3DAABB& aabb = pPlayer->GetPlayerAABB();
					vPos = aabb.Center;
					vPos.y += aabb.Extents.y * .5f;
				}
			}

			return true;
		}
	}
	else if (ISNPCID(nID))
	{
		CECNPC* pNPC = pNPCMan->GetNPCFromAll(nID);

		if (pNPC){
			while (true){
				A3DSkeletonHook* pHook = pNPC->GetSgcHook(szHanger, bChildHook, szHook);
				if (!pHook){
					break;
				}
				A3DSkinModel *pSkin = pNPC->GetSgcSkinModel(szHanger, bChildHook, szHook);
				if (bRelHook){
					vPos = pHook->GetAbsoluteTM() * (*pOffset);
				}else{
					vPos = pSkin->GetAbsoluteTM() * (*pOffset);
					vPos = vPos - pSkin->GetAbsoluteTM().GetRow(3) + pHook->GetAbsoluteTM().GetRow(3);
				}
				return true;
			}
			if (HitPos == enumHitBottom)
				vPos = pNPC->GetPos();
			else
			{
				const A3DAABB& aabb = pNPC->GetPickAABB();
				vPos = aabb.Center;
				vPos.y += aabb.Extents.y * .5f;
			}

			return true;
		}
	}

	return false;
}

inline bool _get_dir_and_up_by_id(
	CECPlayerMan* pPlayerMan,
	CECNPCMan* pNPCMan,
	int nId,
	A3DVECTOR3& vDir,
	A3DVECTOR3& vUp
	)
{
	if (ISPLAYERID(nId))
	{
		CECPlayer* pPlayer = pPlayerMan->GetPlayer(nId);
		
		if (pPlayer)
		{
			vDir = pPlayer->GetDir();
			vUp = pPlayer->GetUp();
			return true;
		}
	}
	else if (ISNPCID(nId))
	{
		CECNPC* pNPC = pNPCMan->GetNPCFromAll(nId);
		
		if (pNPC)
		{
			vDir = pNPC->GetDir();
			vUp = pNPC->GetUp();
			return true;
		}
	}
	
	return false;
}

inline float _get_scale_by_id(
	CECPlayerMan* pPlayerMan,
	CECNPCMan* pNPCMan,
	int nID)
{
	if (ISPLAYERID(nID))
	{
		CECPlayer* pPlayer = pPlayerMan->GetPlayer(nID);

		if (pPlayer)
		{
			const A3DAABB& aabb = pPlayer->GetPlayerAABB();
			return aabb.Maxs.y - aabb.Mins.y;
		}
	}
	else if (ISNPCID(nID))
	{
		CECNPC* pNPC = pNPCMan->GetNPCFromAll(nID);

		if (pNPC)
		{
			const A3DAABB& aabb = pNPC->GetPickAABB();
			return aabb.Maxs.y - aabb.Mins.y;
		}
	}

	return 1.0f;
}


inline bool _get_ecm_property_by_id(clientid_t nID, ECMODEL_GFX_PROPERTY* pProperty)
{
	ASSERT(pProperty);

	int _32bitID = (int)nID;
	if (ISPLAYERID(_32bitID))
	{
		CECPlayer* pPlayer = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(_32bitID);
		if (!pPlayer)
			return false;

		CECModel* pModel = pPlayer->GetPlayerModel();
		if (!pModel)
			return false;

		pProperty->bGfxUseLod = pModel->IsGfxUseLOD();
		pProperty->bGfxDisableCamShake = pModel->GetDisableCamShake();
		pProperty->bHostECMCreatedByGfx = pModel->IsCreatedByGfx();
	}
	else if (ISNPCID(_32bitID))
	{
		CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(_32bitID);
		if (!pNPC || !pNPC->HasModel())
			return false;
		pNPC->GetEcmProperty(pProperty);
	}

	return false;
}


SkillGfxMan::SkillGfxMan(CECGameRun* pGameRun)
{
	for (int i = 0; i < enumMoveModeNum; i++)
	{
		for (int j = 0; j < DEFAULT_EVENT_BUF_SIZE; j++)
		{
			CECSkillGfxEvent* pEvent = new CECSkillGfxEvent(static_cast<GfxMoveMode>(i));
			m_FreeLst[i].AddTail(pEvent);
		}
	}

	m_pPlayerMan	= pGameRun->GetWorld()->GetPlayerMan();
	m_pNPCMan		= pGameRun->GetWorld()->GetNPCMan();
}

float SkillGfxMan::GetTargetScale(clientid_t nTargetId)
{
	return _get_scale_by_id(
		g_pGame->GetGameRun()->GetWorld()->GetPlayerMan(),
		g_pGame->GetGameRun()->GetWorld()->GetNPCMan(),
		nTargetId);
}

bool SkillGfxMan::GetPropertyById(clientid_t nId, ECMODEL_GFX_PROPERTY* pProperty)
{
	return _get_ecm_property_by_id(nId, pProperty);
}

A3DSkillGfxComposer* CECSkillGfxEvent::GetComposer() const
{
	return m_pComposer;
}

clientid_t	CECSkillGfxEvent::GetOriginalHost()const
{
	//	GFX 特效显示可能导致原始的攻击者与目标换位，提供此方法查询原始攻击者
	return m_pMoveMethod->IsReverse() ? m_nTargetID : m_nHostID;
}

clientid_t	CECSkillGfxEvent::GetOriginalTarget()const
{
	//	GFX 特效显示可能导致原始的攻击者与目标换位，提供此方法查询原始目标
	return m_pMoveMethod->IsReverse() ? m_nHostID : m_nTargetID;
}

bool CECSkillGfxEvent::GetTargetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp)
{
	if (!m_bTargetDirAndUpExist)
		return false;

	vDir = m_vTargetDir;
	vUp = m_vTargetUp;
	return true;
}

A3DVECTOR3 CECSkillGfxEvent::GetTargetCenter()
{
	A3DVECTOR3 vTargetCenter;

	// if composer has been set
	// use the composer's parameter to make the hook information affect.
	if (A3DSkillGfxComposer* pComposer = GetComposer())
	{
		_get_pos_by_id(m_pPlayerMan,
			m_pNPCMan,
			m_nTargetID,
			vTargetCenter,
			pComposer->m_HitPos.HitPos,
			false,
			pComposer->m_HitPos.szHook,
			pComposer->m_HitPos.bRelHook,
			&pComposer->m_HitPos.vOffset,
			pComposer->m_HitPos.szHanger,
			pComposer->m_HitPos.bChildHook);
	}
	else
	{
		_get_pos_by_id(
			m_pPlayerMan,
			m_pNPCMan,
			m_nTargetID,
			vTargetCenter,
			enumHitCenter);
	}

	return vTargetCenter;
}

void CECSkillGfxEvent::Tick(DWORD dwDeltaTime)
{
	if (A3DSkillGfxComposer* pComposer = GetComposer())
	{
		const SGC_POS_INFO *pHostPos, *pTargetPos;
		
		if (m_pMoveMethod->IsReverse())
		{
			pHostPos = &m_pComposer->m_FlyEndPos;
			pTargetPos = &m_pComposer->m_FlyPos;
		}
		else
		{
			pHostPos = &m_pComposer->m_FlyPos;
			pTargetPos = &m_pComposer->m_FlyEndPos;
		}

		m_bHostExist = _get_pos_by_id(
			m_pPlayerMan,
			m_pNPCMan,
			m_nHostID,
			m_vHostPos,
			pHostPos->HitPos,
			//m_pMoveMethod->GetHitPos(),
			m_bIsGoblinSkill,
			pHostPos->szHook,
			pHostPos->bRelHook,
			&pHostPos->vOffset,
			pHostPos->szHanger,
			pHostPos->bChildHook);
		
		m_bTargetExist = _get_pos_by_id(
			m_pPlayerMan,
			m_pNPCMan,
			m_nTargetID,
			m_vTargetPos,
			//m_pMoveMethod->GetHitPos(),
			pTargetPos->HitPos,
			false,
			pTargetPos->szHook,
			pTargetPos->bRelHook,
			&pTargetPos->vOffset,
			pTargetPos->szHanger,
			pTargetPos->bChildHook);

		m_bTargetDirAndUpExist = _get_dir_and_up_by_id(m_pPlayerMan, m_pNPCMan, m_nTargetID, m_vTargetDir, m_vTargetUp);
	}
	else
	{
		m_bHostExist = _get_pos_by_id(
			m_pPlayerMan,
			m_pNPCMan,
			m_nHostID,
			m_vHostPos,
			m_pMoveMethod->GetHitPos(),
			m_bIsGoblinSkill);
		
		m_bTargetExist = _get_pos_by_id(
			m_pPlayerMan,
			m_pNPCMan,
			m_nTargetID,
			m_vTargetPos,
			m_pMoveMethod->GetHitPos());
	}


	A3DSkillGfxEvent::Tick(dwDeltaTime);
}

void CECSkillGfxEvent::HitTarget(const A3DVECTOR3& vTarget)
{
	A3DSkillGfxEvent::HitTarget(vTarget);

	// now show some special hit gfx
	if (CECOptimize::Instance().GetGFX().CanShowHit(GetOriginalHost())){
		if( m_dwModifier & CECAttackEvent::MOD_PHYSIC_ATTACK_RUNE )
			g_pGame->GetGFXCaster()->PlayAutoGFXEx("程序联入\\击中\\物攻符击中.gfx", 60000, vTarget,
			g_vAxisZ, g_vAxisY, 0.0f);
		
		if( m_dwModifier & CECAttackEvent::MOD_MAGIC_ATTACK_RUNE )
			g_pGame->GetGFXCaster()->PlayAutoGFXEx("程序联入\\击中\\法攻符击中.gfx", 60000, vTarget,
			g_vAxisZ, g_vAxisY, 0.0f);
		
		if( m_dwModifier & CECAttackEvent::MOD_PHYSIC_DEFENCE_RUNE )
			g_pGame->GetGFXCaster()->PlayAutoGFXEx("程序联入\\击中\\物防符击中.gfx", 60000, vTarget,
			g_vAxisZ, g_vAxisY, 0.0f);
		
		if( m_dwModifier & CECAttackEvent::MOD_MAGIC_DEFENCE_RUNE )
			g_pGame->GetGFXCaster()->PlayAutoGFXEx("程序联入\\击中\\法防符击中.gfx", 60000, vTarget,
			g_vAxisZ, g_vAxisY, 0.0f);
	}
}

CECSkillGfxMan::CECSkillGfxMan(CECGameRun* pGameRun)
: CECManager(pGameRun)
, m_GfxMan(pGameRun)
{
	m_GfxMan.Init(g_pGame->GetA3DDevice());
}

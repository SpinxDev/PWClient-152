/*
* FILE: EC_ModelPhys.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/8/23
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "EC_ModelPhys.h"

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

static const DWORD PARTPHYS_BLEND_DEF_FADEINTIME = 200;
static const DWORD PARTPHYS_BLEND_DEF_HOLDTIME = 0;
static const DWORD PARTPHYS_BLEND_DEF_FADEOUTTIME = 300;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECModelPhys
//	
///////////////////////////////////////////////////////////////////////////

CECModelPhysBlendOverlap::CECModelPhysBlendOverlap(CECModel* pHostModel, BLEND_MODE iMode,
		float fTouchRadius, float fMaxPhysWeight, float fForceValue)
	: m_pHostModel(pHostModel)
	, m_bEnableOverlapPhys(true)
	, m_pForceBone(NULL)
	, m_pRootBone(NULL)
	, m_HostPhysicsState(HPS_IDLE)
	, m_fHostTouchRadius(fTouchRadius)
	, m_kMaxPhysWeight(fMaxPhysWeight)
	, m_forceMag(fForceValue)
	, m_dwPlayTimeCount(0)
	, m_bCloseClothDuringOverlap(false)
{
	SetBlendMode(iMode);

	SetFadeTimes(PARTPHYS_BLEND_DEF_FADEINTIME,
		PARTPHYS_BLEND_DEF_HOLDTIME,
		PARTPHYS_BLEND_DEF_FADEOUTTIME);

	m_HostMoveDir.Clear();
}

CECModelPhysBlendOverlap::~CECModelPhysBlendOverlap()
{

}

bool CECModelPhysBlendOverlap::BindModel(CECModel* pModel, const char* szRootBone)
{
	m_pHostModel = pModel;
	m_pRootBone = NULL;
	m_strRootBone = szRootBone;

	if (pModel && pModel->GetA3DSkinModel())
		m_pRootBone = pModel->GetA3DSkinModel()->GetSkeleton()->GetBone(szRootBone, NULL);

	if (!pModel || !m_pRootBone)
		return false;

	return true;
}

void CECModelPhysBlendOverlap::SetForceBone(const char* szForceBone)
{
	m_strForceBone = szForceBone;
	if (m_pHostModel && m_pHostModel->GetA3DSkinModel())
		m_pForceBone = m_pHostModel->GetA3DSkinModel()->GetSkeleton()->GetBone(szForceBone, NULL);
}

void CECModelPhysBlendOverlap::SetBlendMode(BLEND_MODE bm)
{
	m_iBlendMode = bm;
	if (m_pHostModel)
	{
		if (BM_PURE_PHYSXDRV == m_iBlendMode)
			BindModel(m_pHostModel, "Bip01 Spine1");
		else
			BindModel(m_pHostModel, "000");
	}
}

//	Host touch radius
void CECModelPhysBlendOverlap::SetHostTouchRadius(float fHostTouchRadius)
{
	m_fHostTouchRadius = fHostTouchRadius;
}

void CECModelPhysBlendOverlap::EnableOverlapPhys(bool bEnable)
{
	m_bEnableOverlapPhys = bEnable;
}

int CECModelPhysBlendOverlap::UpdateHostPhysicsState(bool bForceExist)
{
	if (m_HostPhysicsState == HPS_IDLE)
	{
		if (bForceExist)
		{
			if (BM_SOFT_KEYFRAME == m_iBlendMode)
			{
#ifdef _ANGELICA21
				A3DModelPhysSync* pSync = m_pHostModel->GetPhysSync();
				if (!pSync)
					return 0;

				if (pSync->GetPhysState() == A3DModelPhysSync::PHY_STATE_ANIMATION)
				{
					AString strName = A3DModelPhysSync::GetDefSKFConfigName(A3DModelPhysSync::CN_INJURY);
					if (pSync->ApplySoftKeyFrameConfig(strName, true))
						pSync->ChangePhysState(A3DModelPhysSync::PHY_STATE_SIMULATE, A3DModelPhysSync::RB_SIMMODE_SOFT_KEYFRAME);
					
					//--------------------------------------------
					// revised by Wenfeng, Dec. 25, 2012
					// close cloth simulation during overlapping since there is one frame delay which will cause jittering...
					// updated by YangLiu, Jun. 9, 2013
					// this action move to A3DModelPhysSync::EnableSettingsForSoftKeyFrame()
				}
				// since the weight will be cleared when the state change to PHY_STATE_SIMULATE, so we have to set the weight after ChangePhysState()
				pSync->SwitchToPhys(m_pRootBone, 0.01f, true, false);
#endif
			}
			m_HostPhysicsState = HPS_PLAY;
			m_HostPlayState = HPP_FADEIN;
			m_dwPlayTimeCount = 0;
			return 1;
		}
		return 0;
	}
	else
	{
		// HPS_PLAY
		if ((!bForceExist) && m_HostPlayState == HPP_END)
		{
			if (BM_SOFT_KEYFRAME == m_iBlendMode)
			{
#ifdef _ANGELICA21
				A3DModelPhysSync* pSync = m_pHostModel->GetPhysSync();
				if (pSync)
				{
					if (pSync->GetPhysState() == A3DModelPhysSync::PHY_STATE_SIMULATE)
						pSync->ChangePhysState(A3DModelPhysSync::PHY_STATE_ANIMATION);
				}
#endif
			}
			m_HostPhysicsState = HPS_IDLE;
			return 2;
		}
		
		return 0;
	}
}

//	Update overlap models
void CECModelPhysBlendOverlap::UpdateOverlapPos(clientid_t nTargetId, const A3DVECTOR3& vTargetPos, float fTargetTouchRadius)
{
	if (!IsEnabled())
		return;

	OVERLAP_ENTITY& entity = m_mapOverlapObjs[nTargetId];

	entity.vPos = vTargetPos;
	entity.TTL = m_dwTotalPlayTime;
	entity.fTargetTouchRadius = fTargetTouchRadius;
}

//	Erase overlap Target
void CECModelPhysBlendOverlap::EraseOverlapTarget(clientid_t nTargetId)
{
	m_mapOverlapObjs.erase(nTargetId);
}

void CECModelPhysBlendOverlap::Tick(DWORD dwTickDelta)
{
	if (!IsEnabled())
		return;

	A3DVECTOR3 vSumForce(0.0f);
	for (std::map<clientid_t, OVERLAP_ENTITY>::iterator itr = m_mapOverlapObjs.begin()
		; itr != m_mapOverlapObjs.end()
		; )
	{
		OVERLAP_ENTITY& oe = itr->second;

		float fSquaredTouchDistance = (m_fHostTouchRadius + oe.fTargetTouchRadius) * (m_fHostTouchRadius + oe.fTargetTouchRadius);
		float fSquaredObjDistance = (oe.vPos - m_pHostModel->GetPos()).SquaredMagnitude();
		if (fSquaredObjDistance > fSquaredTouchDistance)
		{
			oe.TTL -= dwTickDelta;
			if (fSquaredObjDistance > 25.0f)
			{
				m_mapOverlapObjs.erase(itr++);
			}
			else
				itr++;
		}
		else
		{
			itr++;
			
			A3DVECTOR3 vDelta = m_pHostModel->GetPos() - oe.vPos;
			vDelta.Normalize();
			float d = DotProduct(vDelta, m_HostMoveDir);
			if (0 < d)
			{
				A3DVECTOR3 u = m_HostMoveDir * d;
				A3DVECTOR3 v = vDelta - u;
				u *= -1;
				vDelta = u + v;
			}

			float fkForceFactor = 1.0f - sqrt(fSquaredObjDistance / fSquaredTouchDistance);
			vSumForce += 1.0f * fkForceFactor * m_forceMag * vDelta;
		}
	}

	const bool bForceExist = !vSumForce.IsZero();	
	int iStateChange = UpdateHostPhysicsState(bForceExist);
	UpdateHostPlayState(bForceExist, dwTickDelta);


	bool bToAddForce = false;
	if (BM_SOFT_KEYFRAME == m_iBlendMode)
	{
		if (m_HostPhysicsState == HPS_PLAY)
			bToAddForce = true;
	}
	else
	{
		if (m_HostPhysicsState == HPS_PLAY && iStateChange == 1)
			bToAddForce = true;
	}

	if (bToAddForce)
	{
		if (!vSumForce.IsZero())
		{
			A3DVECTOR3 vBeForcePos = 0;
			if (m_pForceBone)
			{
				vBeForcePos = m_pForceBone->GetAbsoluteTM().GetRow(3);
			}
			else
			{
				vBeForcePos = m_pHostModel->GetModelAABB().Center + A3DVECTOR3(0, m_pHostModel->GetModelAABB().Extents.y / 4.0f, 0);
			}

			int nForceType = CECModel::PFT_IMPULSE;
			float fMaxVelChange = 10.0f;
			float fForceMagnitude = vSumForce.Normalize();
			if (!m_strForceBone.IsEmpty())
			{
				m_pHostModel->AddForceToBone(m_strForceBone, vSumForce, fForceMagnitude, nForceType, fMaxVelChange);
				//AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(vBeForcePos, vBeForcePos + vSumForce, A3DCOLORRGB(255, 0, 0));
			}
			else
				m_pHostModel->AddForce(vBeForcePos - vSumForce, vSumForce, fForceMagnitude, FLT_MAX, nForceType, 0.2f, fMaxVelChange);

			//char buf[128];
			//sprintf(buf, "%s: Force Magnitude = %f\n", "Add Force", fForceMagnitude);
			//OutputDebugStringA(buf);
		}
	}
}

void CECModelPhysBlendOverlap::UpdateHostPlayState(bool bForceExist, DWORD dwTickDelta)
{
#ifdef _ANGELICA21
	A3DModelPhysics* pModelPhysics = m_pHostModel->GetModelPhysics();
	if (!pModelPhysics)
		return;

	A3DModelPhysSync* pSync = pModelPhysics->GetModelSync();
	if (!pSync)
		return;
#endif

	if (m_HostPhysicsState == HPS_PLAY)
	{
		m_dwPlayTimeCount += dwTickDelta;

		float fPlayedRatio = (float)m_dwPlayTimeCount / m_dwTotalPlayTime;

		float fFadeInRatio = (float)m_kFadeInTime / m_dwTotalPlayTime;
		float fFadeOutRatio = 1.f - (float)m_kFadeOutTime / m_dwTotalPlayTime;

		float fWeightMax = m_kMaxPhysWeight;
		float fWeight;
		if (fPlayedRatio < fFadeInRatio)
		{
			//	Fade In
			m_HostPlayState = HPP_FADEIN;
			fWeight = fWeightMax * (fPlayedRatio / fFadeInRatio);
		}
		else if (fPlayedRatio >= 1.f)
		{
			//	End
			m_HostPlayState = HPP_END;
			fWeight = 0.f;
		}
		else if (fPlayedRatio > fFadeOutRatio)
		{
			//	Fade Out
			m_HostPlayState = HPP_FADEOUT;
			float fRatio = (1.f - (fPlayedRatio - fFadeOutRatio) / (1.f - fFadeOutRatio));
			a_Clamp(fRatio, 0.f, 1.f);
			fWeight = fWeightMax * fRatio;
		}
		else
		{
			//	Playing
			m_HostPlayState = HPP_PLAYING;
			fWeight = fWeightMax;
		}

#ifdef _ANGELICA21
		if (fWeight == 0.f)
		{
			if (pSync->GetPhysState() == A3DModelPhysSync::PHY_STATE_SIMULATE)
			{
				pSync->ChangePhysState(A3DModelPhysSync::PHY_STATE_ANIMATION);
				pSync->EnableAnimation(true);
			}
		}
		else
		{
			if (BM_SOFT_KEYFRAME == m_iBlendMode)
				pSync->SwitchToPhys(m_pRootBone, fWeight, true, false);
			else
				pSync->SwitchToPhys(m_pRootBone, fWeight, true, true);
		}
#endif
	}
}

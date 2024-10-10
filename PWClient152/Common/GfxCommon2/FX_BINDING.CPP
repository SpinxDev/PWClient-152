#include "StdAfx.h"
#include "A3DGfxInterface.h"
#include "FX_BINDING.H"
#ifndef _ANGELICA21
#include <FCriticalSectionWrapper.h>
#endif

//////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
//////////////////////////////////////////////////////////////////////////

extern void 
_ApplyMatChgToChild(const A3DCOLORVALUE& cl, CECModel* pECModel, bool bIsApplyComboModel);

#ifdef _ANGELICA22
inline AudioEngine::VECTOR A3DVECTOR2AudioVECTOR(const A3DVECTOR3& v)
{
	AudioEngine::VECTOR ret;
	ret.x = v.x;
	ret.y = v.y;
	ret.z = v.z;
	return ret;
}
#endif

//////////////////////////////////////////////////////////////////////////
//
//	Implement MODEL_SCALE_BINDING
//
//////////////////////////////////////////////////////////////////////////

MODEL_SCALE_BINDING::MODEL_SCALE_BINDING(A3DCombActDynData* pDynData, const BoneScaleChange* pModelScaleEvent, float fModelScale)
: FX_BINDING_BASE(pDynData)
, m_pModelScaleEvent(pModelScaleEvent)
, m_dwTickTime(0)
, m_fScale(fModelScale)
, m_bUseFileScale(pModelScaleEvent->IsUseFileScale())
{
}

MODEL_SCALE_BINDING::~MODEL_SCALE_BINDING()
{
	RetriveBoneScale();
}

void MODEL_SCALE_BINDING::RetriveBoneScale() const 
{
	CECModel* pHostModel = m_pDynData->GetHostModel();
	if (!pHostModel)
		return;

	if (!m_bUseFileScale)
		pHostModel->ScaleAllRootBonesAndGfx(m_fScale);
	else
		pHostModel->UpdateBoneScaleEx();
}

bool MODEL_SCALE_BINDING::IsStop() const
{
	return m_dwTickTime >= m_pModelScaleEvent->GetTimeSpan();
}

void MODEL_SCALE_BINDING::Stop() const
{
	RetriveBoneScale();
}

int MODEL_SCALE_BINDING::CalcCurTimeOfCurPeriod(const BoneScaleChange::BoneScaleParam* pBoneSclParam, float& fCurTimeOfCurPeriod)
{
	DWORD dwDestTotalTime = 0;
	int iDestIdx = 0;
	for (iDestIdx = 0; iDestIdx < pBoneSclParam->m_iDestNum; ++iDestIdx)
	{
		const BoneScaleChange::ScaleDestParam& destParam = pBoneSclParam->m_aScaleArray[iDestIdx];
		if (m_dwTickTime >= dwDestTotalTime && dwDestTotalTime + destParam.m_dwScaleTime > m_dwTickTime)
		{
			fCurTimeOfCurPeriod = (m_dwTickTime - dwDestTotalTime) / (float)destParam.m_dwScaleTime;
			break;
		}
		
		dwDestTotalTime += destParam.m_dwScaleTime;
	}

	return iDestIdx;
}

void MODEL_SCALE_BINDING::UpdateParam(CECModel* pECModel, int nDeltaTime) 
{
	m_dwTickTime += nDeltaTime;

	if (!pECModel)
		return;

	A3DSkinModel* pSkinModel = pECModel->GetA3DSkinModel();
	if (!pSkinModel)
		return;

	const APtrArray<BoneScaleChange::BoneScaleParam*>& boneScaleChangeArray = m_pModelScaleEvent->GetBoneScaleParams();
	for (int iBoneSclIdx = 0; iBoneSclIdx < boneScaleChangeArray.GetSize(); ++iBoneSclIdx)
	{
		const BoneScaleChange::BoneScaleParam* pBoneSclParam = boneScaleChangeArray[iBoneSclIdx];
		ASSERT(pBoneSclParam);
		A3DBone* pBone = pSkinModel->GetSkeleton()->GetBone(pBoneSclParam->m_strBone, NULL);
		if (!pBone)
			continue;
		
		float fCurTimeOfCurPeriod = 0.0f;
		int iDestIdx = CalcCurTimeOfCurPeriod(pBoneSclParam, fCurTimeOfCurPeriod);
		bool bIsOutOfTime = (iDestIdx == pBoneSclParam->m_iDestNum);

		float fCurValue = 1.0f;
		if (!bIsOutOfTime)	
		{
			const BoneScaleChange::ScaleDestParam& curParam = pBoneSclParam->m_aScaleArray[iDestIdx];
			float fCurLeftScale = (iDestIdx == 0 ? pBoneSclParam->m_fStartScale : pBoneSclParam->m_aScaleArray[iDestIdx - 1].m_fDestScale);
			float fCurRightScale = curParam.m_fDestScale;
			fCurValue = fCurLeftScale + (fCurRightScale - fCurLeftScale) * powf(fCurTimeOfCurPeriod, curParam.m_fScaleFactor);
		}
		else
		{
			fCurValue = pBoneSclParam->m_aScaleArray[pBoneSclParam->m_iDestNum - 1].m_fDestScale;
		}
		
		switch (pBoneSclParam->m_iScaleType)
		{
		case CECModel::SCALE_LOCAL:
			pBone->SetLocalScale(fCurValue, fCurValue);
			break;
		case CECModel::SCALE_WHOLE:
			pBone->SetWholeScale(fCurValue);
			break;
		case CECModel::SCALE_NONE:
		default:
			break;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
//	Implement MATTRANS_BINDING
//
//////////////////////////////////////////////////////////////////////////

MATTRANS_BINDING::MATTRANS_BINDING(A3DCombActDynData* pDynData, const MaterialScaleTrans* pTransEvent) 
		: FX_BINDING_BASE(pDynData)
		, m_pTransEvent(pTransEvent)
		, m_dwTimeSpan(0)
{
	ASSERT( m_pTransEvent );
	m_dwTimeEnd = static_cast<DWORD>(m_pTransEvent->m_fTotalSpan);
}

MATTRANS_BINDING::~MATTRANS_BINDING()
{
	
}

bool MATTRANS_BINDING::IsStop() const
{
	return m_dwTimeSpan >= m_dwTimeEnd;
}

void MATTRANS_BINDING::UpdateParam(CECModel* pECModel, int nDeltaTime)
{
	if (m_dwTimeSpan >= m_dwTimeEnd)
		return;

	const MaterialScaleTrans* pTransInfo = m_pTransEvent;
		
	int nCurSeg = 0;
	m_dwTimeSpan += nDeltaTime;
	float fSpan = static_cast<float>(m_dwTimeSpan);

	while (nCurSeg < pTransInfo->m_nDestNum)
	{
		if (fSpan < pTransInfo->m_fTransTime[nCurSeg]) break;
		fSpan -= pTransInfo->m_fTransTime[nCurSeg];
		nCurSeg++;
	}

	float fPortion;

	if (nCurSeg >= pTransInfo->m_nDestNum)
	{
		nCurSeg = pTransInfo->m_nDestNum - 1;
		fPortion = 1.0f;
	}
	else
		fPortion = fSpan / pTransInfo->m_fTransTime[nCurSeg];

	if(fPortion > 1.0f)
		fPortion = 1.0f;
	
	m_CurColor = pTransInfo->GetCurrentColor(nCurSeg, fPortion);

	
	A3DCOLORVALUE c(pECModel->GetOrgColor());
	c = c * m_CurColor;

	pECModel->SetInnerColor(c);

	pECModel->SetInnerAlpha(c.a);

	if (pTransInfo->m_bApplyToChild)
	{
		_ApplyMatChgToChild(m_CurColor, pECModel, true);
	}
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement AUDIOEVENT_BINDING
//	
///////////////////////////////////////////////////////////////////////////

#if defined(_USEAUDIOENGINE)

AUDIOEVENT_BINDING::AUDIOEVENT_BINDING(A3DCombActDynData* pDynData, AudioEngine::EventInstance* pEventInstance)
: FX_BINDING_BASE(pDynData)
, m_pEventInstance(pEventInstance)
, m_vLastTickPos(0)
, m_bPosInited(false)
{
	if(m_pEventInstance)
		m_pEventInstance->AddListener(this);
}

AUDIOEVENT_BINDING::~AUDIOEVENT_BINDING()
{
	CSController csc(&m_csForThreadSafe);
	if (m_pEventInstance)
	{
		AudioEngine::Event* pEvent = m_pEventInstance->GetEvent();
		if(!pEvent)
			return;
		CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
		CriticalSectionController cscei(m_pEventInstance->GetCriticalSectionWrapper());
		m_pEventInstance->RemoveListener(this);
		Stop(false);

		if(m_pEventInstance)
			AfxGetGFXExMan()->GetGfxInterface()->ReleaseAudioEventInstance(m_pEventInstance);
		m_pEventInstance = NULL;
	}
}

void AUDIOEVENT_BINDING::Stop(bool bForce) const
{
	CSController csc(const_cast<CSWrapper*>(&m_csForThreadSafe));
	if (m_pEventInstance)
	{
		AudioEngine::Event* pEvent = m_pEventInstance->GetEvent();
		if(!pEvent)
			return;
		CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
		CriticalSectionController cscei(m_pEventInstance->GetCriticalSectionWrapper());
		m_pEventInstance->Stop(bForce);
	}
}

bool AUDIOEVENT_BINDING::IsStop() const
{
	CSController csc(const_cast<CSWrapper*>(&m_csForThreadSafe));
	if (!m_pEventInstance)
		return true;
	AudioEngine::Event* pEvent = m_pEventInstance->GetEvent();
	if(!pEvent)
		return false;
	CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
	CriticalSectionController cscei(m_pEventInstance->GetCriticalSectionWrapper());

	return !m_pEventInstance->IsPlaying();
}

void AUDIOEVENT_BINDING::UpdateParam(CECModel* pECModel, int nDeltaTime)
{
	CSController csc(&m_csForThreadSafe);
	A3DSkinModel* pModel = pECModel->GetA3DSkinModel();
	PAUDIOEVENT_INFO pInfo = static_cast<PAUDIOEVENT_INFO>(m_pInfo);
	A3DSkeletonHook* pHook;

	if (pInfo->GetHookName().IsEmpty() || !pModel)
		pHook = NULL;
	else
		pHook = pModel->GetSkeletonHook(pInfo->GetHookName(), true);

	A3DMATRIX4 matTran;

	if (!pHook)
		matTran = pInfo->GetMatTran() * pECModel->GetAbsTM();
	else
		matTran = pInfo->GetMatTran() * pHook->GetAbsoluteTM();

	A3DVECTOR3 vCurPos = matTran.GetRow(3);

	A3DVECTOR3 vDeltaPerSecond;
	if (!m_bPosInited)
	{
		vDeltaPerSecond.Clear();
		m_bPosInited = true;
	}
	else
	{
		vDeltaPerSecond = (vCurPos - m_vLastTickPos) * 1000 / (float)nDeltaTime;
	}
	
	m_vLastTickPos = vCurPos;

	if(m_pEventInstance)
	{
		AudioEngine::Event* pEvent = m_pEventInstance->GetEvent();
		if(!pEvent)
			return;
		CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
		CriticalSectionController cscei(m_pEventInstance->GetCriticalSectionWrapper());
		m_pEventInstance->Set3DAttributes(A3DVECTOR2AudioVECTOR(vCurPos), A3DVECTOR2AudioVECTOR(vDeltaPerSecond));
		m_pEventInstance->Set3DConeOrientation(A3DVECTOR2AudioVECTOR(pModel->GetDir()));
	}
}

bool AUDIOEVENT_BINDING::OnStop( AudioEngine::EventInstance* pEventInstance )
{
	CSController csc(&m_csForThreadSafe);
	if (m_pEventInstance)
	{
		AudioEngine::Event* pEvent = m_pEventInstance->GetEvent();
		if(!pEvent)
			return false;
		CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
		CriticalSectionController cscei(m_pEventInstance->GetCriticalSectionWrapper());
		m_pEventInstance->RemoveListener(this);
		AfxGetGFXExMan()->GetGfxInterface()->ReleaseAudioEventInstance(m_pEventInstance);
		m_pEventInstance = NULL;
	}
	return true;
}

#endif
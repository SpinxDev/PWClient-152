/*
* FILE: A3DSkeletonEmitter.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/4/23
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "A3DSkeletonEmitter.h"
#include "A3DGFXExMan.h"
#include "A3DGFXInterface.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DSkeletonEmitter
//	
///////////////////////////////////////////////////////////////////////////

A3DSkeletonEmitter::A3DSkeletonEmitter()
: m_pSkeleton(NULL)
, m_fTotalBoneLength(0.0f)
, m_pECModel(NULL)
{
	m_nType = EMITTER_TYPE_SKELETON;
	m_bBind = false;
}

A3DSkeletonEmitter::~A3DSkeletonEmitter()
{

}

int SelectBoneIdx(int iBoneNum)
{
	return a_Random(0, iBoneNum - 1);
}

void A3DSkeletonEmitter::InitParticle(A3DParticle* pParticle)
{
	if (!m_pSkeleton || !m_pECModel)
		return;

	A3DParticleEmitter::InitParticle(pParticle);
	int iSelBoneIdx = 0;
	ParticleBoneArray& boneList = m_pECModel->GetParticleBoneLst();
	if (boneList.empty())
	{
		iSelBoneIdx = SelectBoneIdx(m_pSkeleton->GetBoneNum());
	}
	else
	{
		int idx = SelectBoneIdx((int)boneList.size());
		iSelBoneIdx = boneList[idx];
	}

	//A3DVECTOR3 vSkeletonPos = m_pSkeleton->GetAbsoluteTM().GetRow(3);
	A3DVECTOR3 vParticlePos = 0;
	if (!m_BonePairProbs.empty() && m_fTotalBoneLength > 0.0f)
	{
		float fDistPercent = 0.0f;
		BoneIdxPair bip = PeekRandomBonePair(&fDistPercent);
		ASSERT( fDistPercent >= 0.0f && fDistPercent < 1.0f );
		A3DBone* pParent = m_pSkeleton->GetBone(GetParentBoneIdx(bip));
		A3DBone* pChild = m_pSkeleton->GetBone(GetChildBoneIdx(bip));
		A3DVECTOR3 vChildPos = pChild->GetAbsoluteTM().GetRow(3);
		A3DVECTOR3 vParentPos = pParent->GetAbsoluteTM().GetRow(3);
		A3DVECTOR3 vDirection = vChildPos - vParentPos;
		A3DVECTOR3 vRandPos = vParentPos + vDirection * fDistPercent;
		vParticlePos = vRandPos;
	}
	else
	{
		vParticlePos = m_pSkeleton->GetBone(iSelBoneIdx)->GetAbsoluteTM().GetRow(3);
	}
	pParticle->m_vOldPos = pParticle->m_vPos = vParticlePos;

	pParticle->m_vDir = m_qParticleDir;
	GenDirection(pParticle->m_vMoveDir);
	pParticle->m_vOldMoveDir = pParticle->m_vMoveDir;

	pParticle->m_fTotalTTL = GenTTL();
	pParticle->m_fTTL = 0;
}

int A3DSkeletonEmitter::GetEmissionCount(float fTimeSpan)
{ 
	return GenEmissionCount(fTimeSpan);
}

void A3DSkeletonEmitter::OnStart(A3DParticleSystemEx* pParticleSystem)
{
	A3DParticleEmitter::OnStart(pParticleSystem);

#ifndef _ANGELICA21
	A3DGFXEx* pGFXEx = pParticleSystem->GetGfx();
	CECModel* pECModel = AfxGetGFXExMan()->GetGfxInterface()->GetECModelById(pGFXEx->GetId(), pGFXEx->GetHostModel());
	m_pECModel = pECModel;
	m_BonePairProbs.clear();
	if (!pECModel || !pECModel->GetA3DSkinModel())
	{
		m_pSkeleton = NULL;
		return;
	}

	A3DSkinModel* pModel = pECModel->GetA3DSkinModel();
	m_pSkeleton = pModel->GetSkeleton();
	if (!m_pSkeleton)
		return;

	m_strSkeletonFile = m_pSkeleton->GetFileName();
	Pair2DistanceMap mapPair2Distance;
	InitPair2DistMap(mapPair2Distance, m_pSkeleton, pECModel);
#endif
}

void A3DSkeletonEmitter::OnTick(A3DParticleSystemEx* pParticleSystem)
{
	A3DParticleEmitter::OnTick(pParticleSystem);

	m_pSkeleton = NULL;

	// when each frame starts, the skinmodel may be no longer exist, so we need to get it again
	A3DGFXEx* pGfx = pParticleSystem->GetGfx();
	CECModel* pECModel = AfxGetGFXExMan()->GetGfxInterface()->GetECModelById(pGfx->GetId(), pGfx->GetHostModel());
	m_pECModel = pECModel;
	if (!pECModel || !pECModel->GetA3DSkinModel() || !pECModel->GetA3DSkinModel()->GetSkeleton())
	{
		// a_LogOutput(1, "Skeleton Emitter %s Can not find a skin model by id %I64d", m_pParticleSystem->GetName(), pParticleSystem->GetGfx()->GetId());
		m_pSkeleton = NULL;
		m_BonePairProbs.clear();
		return;
	}
	
	A3DSkinModel* pModel = pECModel->GetA3DSkinModel();
	m_pSkeleton = pModel->GetSkeleton();
	//	test skeleton is changed or not
	if (m_strSkeletonFile != m_pSkeleton->GetFileName())
	{
		m_strSkeletonFile = m_pSkeleton->GetFileName();
		Pair2DistanceMap mapPair2Distance;
		InitPair2DistMap(mapPair2Distance, m_pSkeleton, pECModel);
	}
}

bool A3DSkeletonEmitter::Load(AFile* pFile, DWORD dwVersion)
{
	if (!A3DParticleEmitter::Load(pFile, dwVersion))
		return false;

	m_bBind = false;
	return true;
}

bool A3DSkeletonEmitter::Save(AFile* pFile)
{
	if (!A3DParticleEmitter::Save(pFile))
		return false;

	return true;
}

bool A3DSkeletonEmitter::InitPair2DistMap(Pair2DistanceMap& mapPair2Distance, A3DSkeleton* pSkeleton, CECModel* pModel)
{
	ASSERT(pSkeleton && pModel);
	m_BonePairProbs.clear();

	//	Init all distance between pairs of bones
	ParticleBoneArray& boneList = pModel->GetParticleBoneLst();
	if (boneList.empty())
	{
		for (int i = 0; i < pSkeleton->GetRootBoneNum(); ++i)
		{
			int iRootIdx = pSkeleton->GetRootBone(i);
			InitPair2DistMapForBone(mapPair2Distance, pSkeleton, iRootIdx, boneList);
		}
	}
	else
	{
		for (size_t i = 0; i < boneList.size(); ++i)
		{
			int iRootIdx = pSkeleton->GetRootBone(boneList[i]);
			InitPair2DistMapForBone(mapPair2Distance, pSkeleton, iRootIdx, boneList);
		}
	}
	
	
	m_fTotalBoneLength = 0.0f;
	//	Calculate the weights of each pair of bones
	for (Pair2DistanceMap::const_iterator itr = mapPair2Distance.begin()
		; itr != mapPair2Distance.end()
		; ++itr)
	{
		m_fTotalBoneLength += itr->second;
	}

	float fCurAccuLength = 0;
	for (Pair2DistanceMap::const_iterator itr = mapPair2Distance.begin()
		; itr != mapPair2Distance.end()
		; ++itr)
	{
		const BoneIdxPair bip = itr->first;
		const float fCurLength = itr->second;
		m_BonePairProbs.push_back(std::make_pair(bip, std::make_pair(fCurAccuLength, fCurLength)));
		fCurAccuLength += fCurLength;
	}

	return true;
}

A3DSkeletonEmitter::BoneIdxPair A3DSkeletonEmitter::PeekRandomBonePair(float* pDistPercent) const
{
	ASSERT(!m_BonePairProbs.empty());

	float fRandLength = a_Random(0.0f, m_fTotalBoneLength);
	PairProbArray::const_iterator last_itr = m_BonePairProbs.end();
	for (PairProbArray::const_iterator itr = m_BonePairProbs.begin()
		; itr != m_BonePairProbs.end()
		; last_itr = itr, ++itr)
	{
		const std::pair<AccuLength, Distance>& curDistPair = itr->second;
		if (curDistPair.first > fRandLength && last_itr != m_BonePairProbs.end())
		{
			*pDistPercent = (fRandLength - last_itr->second.first) / last_itr->second.second;
			return last_itr->first;
		}
	}

	if (pDistPercent)
		*pDistPercent = (fRandLength - last_itr->second.first) / (last_itr->second.second);

	return last_itr->first;
}

bool A3DSkeletonEmitter::InitPair2DistMapForBone(Pair2DistanceMap& mapPair2Distance, A3DSkeleton* pSkeleton, int iBone, const ParticleBoneArray& boneList)
{
	ASSERT(pSkeleton);

	if (!boneList.empty())
	{
		if(boneList.end() == std::find(boneList.begin(), boneList.end(), iBone))
			return true;
	}

	A3DBone* pBone = pSkeleton->GetBone(iBone);
	bool bIs000 = (strcmp(pBone->GetName(), "000") == 0);
	for (int iIdx = 0; iIdx < pBone->GetChildNum(); ++iIdx)
	{
		A3DBone* pChild = pBone->GetChildPtr(iIdx);
		int iChildIdx = pBone->GetChild(iIdx);

		// 只有非000根骨骼，才予以考虑
		if (!bIs000)
		{
			float fDist = (pBone->GetAbsoluteTM().GetRow(3) - pChild->GetAbsoluteTM().GetRow(3)).Magnitude();
			fDist = a_Max(fDist, 0.001f);
			mapPair2Distance[MakeBoneIdxPair(iBone, iChildIdx)] = fDist;
		}

		InitPair2DistMapForBone(mapPair2Distance, pSkeleton, iChildIdx, boneList);
	}

	return true;
}
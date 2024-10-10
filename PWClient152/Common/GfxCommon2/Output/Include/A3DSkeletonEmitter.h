/*
* FILE: A3DSkeletonEmitter.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/4/23
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DSkeletonEmitter_H_
#define _A3DSkeletonEmitter_H_

#include <hash_map>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkeleton;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DSkeletonEmitter
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkeletonEmitter : public A3DParticleEmitter
{
public:		//	Types

public:		//	Constructor and Destructor

	A3DSkeletonEmitter(void);
	virtual ~A3DSkeletonEmitter(void);

public:		//	Attributes

public:		//	Operations

private:	//	Attributes

	//	This is a runtime pointer (only valid during a frame, updated by OnTick)
	//	Keep aware on this
	A3DSkeleton* m_pSkeleton;
	//	Runtime variable used to keep track on the skeleton
	AString m_strSkeletonFile;
	CECModel*	m_pECModel;


	typedef int BoneIdxPair;
	typedef stdext::hash_map<BoneIdxPair, float> Pair2DistanceMap;

	float m_fTotalBoneLength;

	typedef float AccuLength;
	typedef float Distance;
	typedef std::vector<std::pair<BoneIdxPair, std::pair<AccuLength, Distance> > > PairProbArray;
	PairProbArray m_BonePairProbs;

private:	//	Operations

	BoneIdxPair PeekRandomBonePair(Distance* pDist) const;
	bool InitPair2DistMap(Pair2DistanceMap& mapPair2Distance, A3DSkeleton* pSkeleton, CECModel* pModel);
	bool InitPair2DistMapForBone(Pair2DistanceMap& mapPair2Distance, A3DSkeleton* pSkeleton, int iBone, const ParticleBoneArray& boneList);
	static inline BoneIdxPair MakeBoneIdxPair(int iBoneParent, int iBoneChild) { return ((iBoneParent & 0xffff) << 16) | (iBoneChild & 0xffff); }
	static inline int GetParentBoneIdx(BoneIdxPair bip) { return ((bip & 0xffff0000) >> 16); }
	static inline int GetChildBoneIdx(BoneIdxPair bip) { return (bip & 0xffff); }

	void InitParticle(A3DParticle* pParticle);
	int GetEmissionCount(float fTimeSpan);
	virtual void OnStart(A3DParticleSystemEx* pParticleSystem);
	virtual void OnTick(A3DParticleSystemEx* pParticleSystem);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual bool IsParticlePosAbs() const { return true; }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSkeletonEmitter_H_


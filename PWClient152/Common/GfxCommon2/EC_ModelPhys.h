/*
* FILE: EC_ModelPhys.h
*
* DESCRIPTION: CECModelPhys helps client to use CECModel with Physics
*				CECModelPhysBlendOverlap 这个类用于在主角与NPC发生碰撞的时候提供物理表现
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/8/23
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#ifndef _EC_ModelPhys_H_
#define _EC_ModelPhys_H_

#include "A3DVector.h"
#include "GfxCommonTypes.h"
#include "AString.h"
#include <map>

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

class A3DBone;
class CECModel;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECModelPhys
//	
///////////////////////////////////////////////////////////////////////////


class CECModelPhysBlendOverlap
{
	CECModelPhysBlendOverlap();

public:		//	Types

	enum BLEND_MODE
	{
		BM_SOFT_KEYFRAME,
		BM_PURE_PHYSXDRV,
	};

	struct OVERLAP_ENTITY
	{
		A3DVECTOR3 vPos;
		int TTL;
		float fTargetTouchRadius;
	};

	enum HOST_PARTPHYS_STATE
	{
		HPS_IDLE,
		HPS_PLAY,
	};

	enum HOST_PARTPHYS_PLAYSTATE
	{
		HPP_FADEIN,
		HPP_PLAYING,
		HPP_FADEOUT,
		HPP_END,
	};

public:		//	Constructor and Destructor

	CECModelPhysBlendOverlap(CECModel* pHostModel, BLEND_MODE iMode,
		float fTouchRadius, float fMaxPhysWeight, float fForceValue);
	virtual ~CECModelPhysBlendOverlap(void);

public:		//	Attributes

public:		//	Operations

	//	Bind the overlap
	bool BindModel(CECModel* pModel, const char* szRootBone);

	//	Bone that be affected by force
	void SetForceBone(const char* szForceBone);

	//	Get/Set blend mode (soft key frame by default)
	void SetBlendMode(BLEND_MODE bm);
	BLEND_MODE GetBlendMode() const { return m_iBlendMode; }

	//	Get HostModel
	const CECModel* GetHostModel() const { return m_pHostModel; }

	//	Get total play time (500 by default)
	DWORD GetTotalPlayTime() const { return m_dwTotalPlayTime; }

	//	Set Fade Times
	void SetFadeTimes(DWORD dwFadeInTime, DWORD dwHoldTime, DWORD dwFadeOutTime)
	{
		m_kFadeInTime = dwFadeInTime;
		m_kHoldTime = dwHoldTime;
		m_kFadeOutTime = dwFadeOutTime;
		m_dwTotalPlayTime = m_kFadeInTime + m_kHoldTime + m_kFadeOutTime;
	}
	//	Get fade in time (200 by default)
	DWORD GetFadeInTime() const { return m_kFadeInTime; }
	//	Get hold time (0 by default)
	DWORD GetHoldTime() { return m_kHoldTime; }
	//	Get fade out time (300 by default)
	DWORD GetFadeOutTime() const { return m_kFadeOutTime; }

	//	Get/Set force value (2 by default)
	void SetForceValue(float f) { m_forceMag = f; }
	float GetForceValue() const { return m_forceMag; }

	//	Get/Set m_kMaxPhysWeight
	void SetMaxPhysWeight(float f) { m_kMaxPhysWeight = f; }
	float GetMaxPhysWeight() const { return m_kMaxPhysWeight; }

	//	Enable overlap phys only enable the overlap,
	//	CECModel should EnablePhysSystem to see the real effect
	//	true for default
	void EnableOverlapPhys(bool bEnable);

	//	Set overlap radius (0.5 by default)
	void SetHostTouchRadius(float fHostTouchRadius);

	//	Update overlap models
	void UpdateHostMoveDir(const A3DVECTOR3& vDir) { m_HostMoveDir = vDir; }

	//	Update overlap models
	void UpdateOverlapPos(clientid_t nTargetId, const A3DVECTOR3& vTargetPos, float fTargetTouchRadius);

	//	Erase overlap Target
	void EraseOverlapTarget(clientid_t nTargetId);

	//	Apply the overlap effect
	void Tick(DWORD dwTickDelta);

	//	Is Idle
	bool IsIdle() const { return m_HostPhysicsState == HPS_IDLE; }

private:	//	Attributes

	BLEND_MODE	m_iBlendMode;
	bool		m_bEnableOverlapPhys;

	CECModel*	m_pHostModel;
	float		m_fHostTouchRadius;
	A3DVECTOR3	m_HostMoveDir;

	AString		m_strRootBone;
	A3DBone*	m_pRootBone;
	AString		m_strForceBone;
	A3DBone*	m_pForceBone;

	std::map<clientid_t, OVERLAP_ENTITY> m_mapOverlapObjs;
	HOST_PARTPHYS_STATE		m_HostPhysicsState;
	HOST_PARTPHYS_PLAYSTATE m_HostPlayState;

	DWORD	m_dwPlayTimeCount;

	DWORD	m_dwTotalPlayTime;
	DWORD	m_kFadeInTime;
	DWORD	m_kHoldTime;
	DWORD	m_kFadeOutTime;

	float	m_forceMag;
	float	m_kMaxPhysWeight;

	bool	m_bCloseClothDuringOverlap;

private:	//	Operations

	int UpdateHostPhysicsState(bool bForceExist);
	void UpdateHostPlayState(bool bForceExist, DWORD dwTickDelta);
	bool IsEnabled() const { return m_bEnableOverlapPhys && m_pHostModel != NULL; }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_EC_ModelPhys_H_



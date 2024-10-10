#ifndef _A3DSKILLGFXCOMPOSER2_H_
#define _A3DSKILLGFXCOMPOSER2_H_

#include "A3DGFXExMan.h"
#include "A3DSkillGfxEvent2.h"
#include "hashmap.h"

enum GfxTargetMode
{
	enumHostToTarget = 0,
	enumTargetToHost,
	enumTargetDescend,
	enumTargetAscend,
	enumHostDescend,
	enumHostAscend,
	enumTargetSelf,
	enumHostSelf,
	enumHostLinkTarget,
	enumTargetLinkHost,
	enumTargetModeNum
};

enum GfxAttackMode
{
	enumAttPoint,
	enumAttArea,
	enumAttModeNum
};

struct GfxCluster
{
	unsigned long	m_ulCount;
	DWORD			m_dwInterv;
};

struct SGC_POS_INFO
{
	GfxHitPos				HitPos;
	char					szHook[80];
	char					szHanger[80];
	A3DVECTOR3				vOffset;
	bool					bRelHook;
	bool					bChildHook;

	void Clear()
	{
		HitPos = enumHitCenter;
		memset(szHook, 0, sizeof(szHook));
		memset(szHanger, 0, sizeof(szHanger));
		vOffset.Clear();
		bRelHook = true;
		bChildHook = false;
	}
};

class A3DSkillGfxComposer
{
public:
	A3DSkillGfxComposer() 
	{
		m_FlyPos.Clear();
		m_FlyEndPos.Clear();
		m_HitPos.Clear();
	}
	~A3DSkillGfxComposer() {}

public:
	// Fly Gfx
	char			m_szFlyGfx[MAX_PATH];
	SGC_POS_INFO	m_FlyPos;
	SGC_POS_INFO	m_FlyEndPos;
	float			m_fFlyGfxScale;
	GfxMoveMode		m_MoveMode;
	GfxTargetMode	m_TargetMode;
	GfxAttackMode	m_AttFlyMode;
	GfxCluster		m_FlyCluster;
	DWORD			m_dwFlyTime;
	bool			m_bRelScl;
	float			m_fDefTarScl;
	bool			m_bOneHit;
	bool			m_bFadeOut;

	// Rand Area
	bool			m_bArea;
	EmitShape		m_Shape;
	A3DVECTOR3		m_vSize;

	// Hit Gfx
	char			m_szHitGfx[MAX_PATH];
	SGC_POS_INFO	m_HitPos;
	float			m_fHitGfxScale;
	GfxAttackMode	m_AttHitMode;
	GfxCluster		m_HitCluster;
	bool			m_bTraceTarget;

	char			m_szHitGrndGfx[MAX_PATH];

	// Param
	GfxSkillValType m_paramType;
	GFX_SKILL_PARAM	m_param;

	A3DSkillGfxMan*	m_pSkillGfxMan;

protected:
	void AddOneTarget(clientid_t nCastTargetID, clientid_t nHostID, const char* szFly, const char* szHit, const TARGET_DATA& tar, bool bFirst, bool bIsGoblinSkill = false);

public:
	bool Load(const char* szFile);
	bool Load(AFileImage* pFile);
	bool Save(const char* szFile);
	bool Save(AFile* pFile);
	void SetDefault();
	void Init(A3DSkillGfxMan* pMan) { m_pSkillGfxMan = pMan; }
	void Play(clientid_t nHostID, clientid_t nCastTargetID, const abase::vector<TARGET_DATA>& Targets, bool bIsGoblinSkill = false);
};

inline void A3DSkillGfxComposer::SetDefault()
{
	memset(m_szFlyGfx, 0, sizeof(m_szFlyGfx));
	memset(m_szHitGrndGfx, 0, sizeof(m_szHitGrndGfx));
	m_FlyPos.Clear();
	m_FlyEndPos.Clear();
	m_HitPos.Clear();
	m_fFlyGfxScale			= 1.0f;
	m_MoveMode				= enumLinearMove;
	m_TargetMode			= enumHostToTarget;	
	m_AttFlyMode			= enumAttPoint;
	m_FlyCluster.m_ulCount	= 1;
	m_FlyCluster.m_dwInterv	= 0;
	m_bOneHit				= true;
	m_bFadeOut				= false;
	m_dwFlyTime				= 0;
	m_bRelScl				= true;
	m_bArea					= false;
	m_Shape					= enumBox;
	m_vSize.Clear();
	memset(&m_param, 0, sizeof(m_param));
	m_paramType				= enumGfxSkillInt;
	m_fDefTarScl			= 1.8f;
	memset(m_szHitGfx, 0, sizeof(m_szHitGfx));
	m_fHitGfxScale			= 1.0f;
	m_AttHitMode			= enumAttPoint;
	m_HitCluster.m_ulCount	= 1;
	m_HitCluster.m_dwInterv	= 0;
	m_bTraceTarget			= false;
}

typedef abase::hash_map<int, A3DSkillGfxComposer*> ComposerMap;

class A3DSkillGfxComposerMan
{
public:
	A3DSkillGfxComposerMan() {}
	~A3DSkillGfxComposerMan() {}

protected:
	ComposerMap m_ComposerMap;

public:
	bool LoadOneComposer(int nSkillID, const char* szPath);
	void Release();
	void Play(
		int nSkillID,
		clientid_t nHostID,
		clientid_t nCastTargetID,
		const abase::vector<TARGET_DATA>& Targets,
		bool bIsGoblinSkill = false);
	const A3DSkillGfxComposer* GetSkillGfxComposer(int skill);
};

#endif

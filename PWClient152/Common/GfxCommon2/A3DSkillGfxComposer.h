#ifndef _A3DSKILLGFXCOMPOSER_H_
#define _A3DSKILLGFXCOMPOSER_H_

#include "A3DSkillGfxEvent.h"
#include "hashmap.h"
#include "GfxCommonTypes.h"

namespace _SGC
{

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
	enumAttPoint = 0,
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

	abase::vector<AString>		strHooks;

	SGC_POS_INFO()
	{
		HitPos = enumHitCenter;
		memset(szHook, 0, sizeof(szHook));
		memset(szHanger, 0, sizeof(szHanger));
		vOffset.Clear();
		bRelHook = true;
		bChildHook = false;
	}

	void Clear()
	{
		HitPos = enumHitCenter;
		memset(szHook, 0, sizeof(szHook));
		memset(szHanger, 0, sizeof(szHanger));
		vOffset.Clear();
		bRelHook = true;
		bChildHook = false;
		strHooks.clear();
	}

	bool RandomHook(char* szRet)
	{
		if (strHooks.size() <= 1)
		{
			return false;
		}

		int r = a_Random(0, strHooks.size()-1);
		strcpy_s(szRet, 80, strHooks[r]);

		return true;
	}

};

class A3DSkillGfxComposer
{
public:		//	Types

#define PARAM_STRING_LEN 64
#define PARAM_STRING_NUM 5

public:		//	Constructors / Destructor

	A3DSkillGfxComposer();

	~A3DSkillGfxComposer();

	A3DSkillGfxComposer& operator = (const A3DSkillGfxComposer& src);

public:
	// Fly
	char			m_szFlyGfx[MAX_PATH];
	SGC_POS_INFO	m_FlyPos;
	SGC_POS_INFO	m_FlyEndPos;
	float			m_fFlyGfxScale;
	GfxMoveMode		m_MoveMode;
	GFX_CUSTOM_MOVE_PARAM*	m_pGfxCustomMoveParam;
	GfxTargetMode	m_TargetMode;
	GfxAttackMode	m_AttFlyMode;
	GfxCluster		m_FlyCluster;
	float			m_fFlySpeed;
	DWORD			m_dwFlyTime;
	bool			m_bFadeOut;
	bool			m_bStopEmit;				//	Stop particle emit when arrived
	DWORD			m_dwStayTime;
	DWORD			m_dwDelayTime;				//	delay time after hit, then fade out

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
	bool			m_bRelScl;
	float			m_fDefTarScl;
	bool			m_bOneHit;
	float			m_fMinHitGfxScaleFinal;		//	The floor scale value for the hit gfx
	float			m_fMaxHitGfxScaleFinal;		//	The roof scale value for the hit gfx
	A3DVECTOR3		m_vHitGfxDir;				//	Direction will be added to the final direction when hit

	//	This id is used in Audio Event System
	//	Edited by artist in the ATT Editor
	//	There should be a config file for this id, which includes the id-typename pair values
	//	for convenient, we make a config file at: ABASEDIR\\Configs\\att_audio_types.txt
	//	-1 for default
	int				m_iHitSoundType;			//	Hit sound type

	//Extra Hit Gfx
	char		m_szExtraHitGfx[MAX_PATH];
	GfxHitPos		m_extraHitPos;
	float			m_fExtraDefTarScl;
	bool			m_bExtraRelScl;
	float			m_fExtraMinHitGfxScaleFinal;		//	The floor scale value for the hit gfx
	float			m_fExtraMaxHitGfxScaleFinal;		//	The roof scale value for the hit gfx
	bool			m_bExtraTraceTarget;

	// Ground
	char			m_szHitGrndGfx[MAX_PATH];
	SGC_POS_INFO	m_GroundPos;

	// Param
	GfxSkillValType m_paramType;
	GFX_SKILL_PARAM	m_param;

	const char*	GetCommand(int iIdx) const { return m_szCmd[iIdx]; }
	bool SetCommand(int iIdx, const char* szCmd);
	int GetCommmandCount() const { return PARAM_STRING_NUM; }

private:
	char			m_szCmd[PARAM_STRING_NUM][PARAM_STRING_LEN];

public:
	bool Load(const char* szFile);
	bool Load(AFileImage* pFile);
	bool Save(const char* szFile);
	bool Save(AFile* pFile);
	void SetDefault();
	void Play(
		unsigned char SerialID,
		clientid_t nCasterID,
		clientid_t nCastTargetID,
		const A3DVECTOR3* pFixedPoint,
		float fSpeed,
		GfxFlyHitMode mode,
		int nFlySfxPriority = 0,
		int nHitSfxPriority = 0,
		int nAttIndex = 0,
		int nAttOrientation = 0,
		float fGfxScale = 1.f,
		bool bAllExtraHit = false);
};

typedef abase::hash_map<AString, A3DSkillGfxComposer*> ComposerMap;

class A3DSkillGfxComposerMan
{
public:

	A3DSkillGfxComposerMan() { m_strBaseDir = "GFX\\SkillAttack\\"; }
	~A3DSkillGfxComposerMan() {}

protected:

	ComposerMap m_ComposerMap;
	AString m_strBaseDir;

public:

	void SetBaseDir(const char* szDir) { m_strBaseDir = szDir; }
	void Release();
	void Play(
		const AString& strAtkFile,
		unsigned char SerialID,
		clientid_t nCasterID,
		clientid_t nCastTargetID,
		const A3DVECTOR3* pFixedPoint,
		float fSpeed = 0.0f,
		GfxFlyHitMode mode = enumAttBothFlyHit,
		int nFlySfxPriority = 0,
		int nHitSfxPriority = 0,
		int nAttIndex = 0,
		int nAttOrientation = 0,
		float fGfxScale = 1.f,
		bool bAllExtraHit = false);

	const A3DSkillGfxComposer* FindSkillGfx(const AString& strAtkFile) const
	{
		ComposerMap::const_iterator it = m_ComposerMap.find(strAtkFile);
		return it == m_ComposerMap.end() ? NULL : it->second;
	}

	A3DSkillGfxComposer* LoadSkillGfx(const AString& strAtkFile);
};

extern A3DSkillGfxComposerMan* AfxGetSkillGfxComposerMan();

}

void _get_rand_damage(int damage, int divisions, int* parts);

#endif

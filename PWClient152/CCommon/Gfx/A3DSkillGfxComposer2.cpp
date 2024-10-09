#include "A3DSkillGfxComposer2.h"
#include <AFileImage.h>

#ifndef _SKILLGFXCOMPOSER
	#include "EC_ManSkillGfx.h"
	#include "EC_Optimize.h"
	extern CECGame* g_pGame;
#endif

#define	SGC_CUR_VERSION	5

static const char* _format_version		= "Version: %d";
static const char* _format_path			= "Path: %s";
static const char* _format_hook			= "Hook: %s";
static const char* _format_move_mode	= "MoveMode: %d";
static const char* _format_target_mode	= "TargetMode: %d";
static const char* _format_att_mode		= "AttMode: %d";
static const char* _format_count		= "Count: %d";
static const char* _format_radius		= "Radius: %f";
static const char* _format_interv		= "Interv: %d";
static const char* _format_flytime		= "FlyTime: %d";
static const char* _format_rel_scle		= "RelScl: %d";
static const char* _format_tar_scl		= "TarScl: %f";
static const char* _format_trace		= "TraceTarget: %d";
static const char* _format_val_type		= "ValType: %d";
static const char* _format_val_bool		= "Bool: %d";
static const char* _format_val_int		= "Int: %d";
static const char* _format_val_float	= "Float: %f";
static const char* _format_area			= "IsArea: %d";
static const char* _format_shape		= "Shape: %d";
static const char* _format_range		= "Range: %f, %f, %f";
static const char* _format_one_hit		= "OneHit: %d";
static const char* _format_fade_out		= "FadeOut: %d";
static const char* _format_scale		= "Scale: %f";
static const char* _format_pos			= "Pos: %f, %f, %f";
static const char* _format_rel_hook		= "RelHook: %d";
static const char* _format_child_hook	= "ChildHook: %d";
static const char* _format_hanger		= "Hanger: %s";
static const char* _format_staytime		= "StayTime: %d";
static const char* _format_hit_pos		= "HitPos: %d";


inline void _load_sgc_pos(DWORD dwVersion, AFileImage* pFile, char* szLine, SGC_POS_INFO& Pos)
{
	if (dwVersion < 5)
		return;

	DWORD dwReadLen;
	int nRead;
	Pos.szHook[0] = '\0';

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_hook, Pos.szHook);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_pos, VECTORADDR_XYZ(Pos.vOffset));

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_rel_hook, &nRead);
	Pos.bRelHook = (nRead != 0);

	Pos.szHanger[0] = '\0';
	Pos.bChildHook = false;

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_hanger, Pos.szHanger);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_child_hook, &nRead);
	Pos.bChildHook = (nRead != 0);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_hit_pos, &Pos.HitPos);
}

inline void _save_sgc_pos(AFile* pFile, char* szLine, SGC_POS_INFO& Pos)
{
	sprintf(szLine, _format_hook, Pos.szHook);
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_pos, VECTOR_XYZ(Pos.vOffset));
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_rel_hook, Pos.bRelHook);
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_hanger, Pos.szHanger);
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_child_hook, Pos.bChildHook);
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_hit_pos, Pos.HitPos);
	pFile->WriteLine(szLine);
}


bool A3DSkillGfxComposer::Load(const char* szFile)
{
	AFileImage file;
	if (!file.Open(szFile, AFILE_OPENEXIST)) return false;
	if (!Load(&file))
	{
		file.Close();
		return false;
	}
	file.Close();
	return true;
}

bool A3DSkillGfxComposer::Load(AFileImage* pFile)
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;
	DWORD	dwVersion = 0;

	if (pFile->IsBinary())
	{
	}
	else
	{
		int nRead;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_version, &dwVersion);

		if (dwVersion > 0) pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		m_szFlyGfx[0] = '\0';
		sscanf(szLine, _format_path, m_szFlyGfx);

		m_szHitGrndGfx[0] = '\0';
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_path, m_szHitGrndGfx);

		if (dwVersion >= 3)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_scale, &m_fFlyGfxScale);
		}
		else
			m_fFlyGfxScale = 1.0f;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_move_mode, &m_MoveMode);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_target_mode, &m_TargetMode);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_att_mode, &m_AttFlyMode);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_count, &m_FlyCluster.m_ulCount);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_interv, &m_FlyCluster.m_dwInterv);

		if (dwVersion >= 2)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_one_hit, &nRead);
			m_bOneHit = (nRead != 0);
		}
		else
			m_bOneHit = true;

		if (dwVersion >= 4)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_fade_out, &nRead);
			m_bFadeOut = (nRead != 0);
		}
		else
			m_bFadeOut = false;

		if (dwVersion >= 1)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_area, &nRead);
			m_param.m_bArea = (nRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_shape, &m_param.m_Shape);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_range, VECTORADDR_XYZ(m_param.m_vSize));
		}

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_flytime, &m_dwFlyTime);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_rel_scle, &nRead);
		m_bRelScl = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_tar_scl, &m_fDefTarScl);
		if (m_fDefTarScl == 0) m_fDefTarScl = 1.8f;

		m_szHitGfx[0] = '\0';
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_path, m_szHitGfx);

		if (dwVersion >= 3)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_scale, &m_fHitGfxScale);
		}
		else
			m_fHitGfxScale = 1.0f;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_att_mode, &m_AttHitMode);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_count, &m_HitCluster.m_ulCount);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_interv, &m_HitCluster.m_dwInterv);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_trace, &nRead);
		m_bTraceTarget = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_val_type, &m_paramType);

		if (m_paramType == enumGfxSkillBool)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_val_bool, &nRead);
			m_param.bVal = (nRead != 0);
		}
		else if (m_paramType == enumGfxSkillInt)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_val_int, &m_param.nVal);
		}
		else if (m_paramType == enumGfxSkillFloat)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_val_float, &m_param.fVal);
		}
		else
			ASSERT(FALSE);

		_load_sgc_pos(dwVersion, pFile, szLine, m_FlyPos);
		_load_sgc_pos(dwVersion, pFile, szLine, m_FlyEndPos);
		_load_sgc_pos(dwVersion, pFile, szLine, m_HitPos);
		
		//	A bug(made by changing the composer file 2009.9.30) fix
		if (dwVersion < 5)
		{
			// 对老文件特殊处理
			// 当且仅当运动方式为定点时，修改FlyGfx的POS_INFO中的默认值为定点运动方式的默认值(地面)
			if (m_MoveMode == enumOnTarget)
			{
				m_FlyPos.HitPos = enumHitBottom;
				m_FlyEndPos.HitPos = enumHitBottom;
			}
		}
	}

	return true;
}

bool A3DSkillGfxComposer::Save(const char* szFile)
{
	AFile file;
	if (!file.Open(szFile, AFILE_CREATENEW | AFILE_TEXT))
		return false;

	Save(&file);

	file.Close();
	return true;
}

bool A3DSkillGfxComposer::Save(AFile* pFile)
{
	char	szLine[AFILE_LINEMAXLEN];

	if (pFile->IsBinary())
	{
	}
	else
	{
		sprintf(szLine, _format_version, SGC_CUR_VERSION);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_path, m_szFlyGfx);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_path, m_szHitGrndGfx);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_scale, m_fFlyGfxScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_move_mode, m_MoveMode);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_target_mode, m_TargetMode);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_att_mode, m_AttFlyMode);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_count, m_FlyCluster.m_ulCount);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_interv, m_FlyCluster.m_dwInterv);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_one_hit, (int)m_bOneHit);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_fade_out, (int)m_bFadeOut);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_area, (int)m_param.m_bArea);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_shape, m_param.m_Shape);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_range, VECTOR_XYZ(m_param.m_vSize));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_flytime, m_dwFlyTime);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_rel_scle, m_bRelScl);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_tar_scl, m_fDefTarScl);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_path, m_szHitGfx);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_scale, m_fHitGfxScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_att_mode, m_AttHitMode);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_count, m_HitCluster.m_ulCount);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_interv, m_HitCluster.m_dwInterv);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_trace, m_bTraceTarget);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_val_type, m_paramType);
		pFile->WriteLine(szLine);

		if (m_paramType == enumGfxSkillBool)
		{
			sprintf(szLine, _format_val_bool, m_param.bVal);
			pFile->WriteLine(szLine);
		}
		else if (m_paramType == enumGfxSkillInt)
		{
			sprintf(szLine, _format_val_int, m_param.nVal);
			pFile->WriteLine(szLine);
		}
		else if (m_paramType == enumGfxSkillFloat)
		{
			sprintf(szLine, _format_val_float, m_param.fVal);
			pFile->WriteLine(szLine);
		}
		else
			ASSERT(FALSE);

		
		_save_sgc_pos(pFile, szLine, m_FlyPos);
		_save_sgc_pos(pFile, szLine, m_FlyEndPos);
		_save_sgc_pos(pFile, szLine, m_HitPos);

	}

	return true;
}

inline void A3DSkillGfxComposer::AddOneTarget(
	clientid_t nCastTargetID,
	clientid_t nHostID,
	const char* szFly,
	const char* szHit,
	const TARGET_DATA& tar,
	bool bFirst,
	bool bIsGoblinSkill)
{
	clientid_t _Host, _Target;
	float fScale;
	bool bReverse;

	switch (m_TargetMode)
	{
	case enumTargetToHost:
	case enumHostDescend:
	case enumHostAscend:
	case enumHostSelf:
	case enumTargetLinkHost:
		_Host = tar.idTarget;
		_Target = nHostID;
		bReverse = true;
		break;
	default:
		_Host = nHostID;
		_Target = tar.idTarget;
		bReverse = false;
		break;
	}

	if (m_bRelScl)
		fScale = m_pSkillGfxMan->GetTargetScale(_Target) / m_fDefTarScl * m_fHitGfxScale;
	else
		fScale = m_fHitGfxScale;

	if (nCastTargetID != 0 && tar.idTarget != nCastTargetID
	 || nCastTargetID == 0 && !bFirst)
	{
		if (m_AttFlyMode == enumAttArea) szFly = NULL;
		if (m_AttHitMode == enumAttArea) szHit = NULL;
	}

	m_pSkillGfxMan->AddSkillGfxEvent(
		this,
		_Host,
		_Target,
		szFly,
		szHit,
		m_dwFlyTime,
		m_bTraceTarget,
		m_MoveMode,
		m_FlyCluster.m_ulCount,
		m_FlyCluster.m_dwInterv,
		&m_param,
		m_fFlyGfxScale,
		fScale,
		tar.dwModifier,
		m_bOneHit,
		m_bFadeOut,
		bIsGoblinSkill,
		bReverse
		);
}

void A3DSkillGfxComposer::Play(
	clientid_t nHostID,
	clientid_t nCastTargetID,
	const abase::vector<TARGET_DATA>& Targets,
	bool bIsGoblinSkill)
{
	size_t i;
	bool bCastInTargets = false;

	const char* szFly = m_szFlyGfx[0] ? m_szFlyGfx : NULL;
	const char* szHit = m_szHitGfx[0] ? m_szHitGfx : NULL;

#ifndef _SKILLGFXCOMPOSER
	if (!CECOptimize::Instance().GetGFX().CanShowFly(nHostID))
		szFly = NULL;
	if (!CECOptimize::Instance().GetGFX().CanShowHit(nHostID))
		szHit = NULL;
#endif

	for (i = 0; i < Targets.size(); i++)
	{
		const TARGET_DATA& tar = Targets[i];

		if (nCastTargetID == tar.idTarget)
			bCastInTargets = true;

		AddOneTarget(nCastTargetID, nHostID, szFly, szHit, tar, i == 0, bIsGoblinSkill);
	}

	if (nCastTargetID && !bCastInTargets)
	{
		TARGET_DATA tar;
		tar.idTarget = nCastTargetID;
		tar.dwModifier = 0;

		AddOneTarget(nCastTargetID, nHostID, szFly, szHit, tar, false, bIsGoblinSkill);
	}
}

bool A3DSkillGfxComposerMan::LoadOneComposer(int nSkillID, const char* szPath)
{
	if (m_ComposerMap.find(nSkillID) != m_ComposerMap.end())
		return false;

	A3DSkillGfxComposer* pComposer = new A3DSkillGfxComposer();
	if (!pComposer->Load(szPath))
	{
		delete pComposer;
		return false;
	}

#ifndef _SKILLGFXCOMPOSER
	pComposer->Init(g_pGame->GetGameRun()->GetWorld()->GetSkillGfxMan()->GetPtr());
#endif

	m_ComposerMap[nSkillID] = pComposer;
	return true;
}

void A3DSkillGfxComposerMan::Release()
{
	ComposerMap::iterator it = m_ComposerMap.begin();
	for (; it != m_ComposerMap.end(); ++it) delete it->second;
	m_ComposerMap.clear();
}

void A3DSkillGfxComposerMan::Play(
	int nSkillID,
	clientid_t nHostID,
	clientid_t nCastTargetID,
	const abase::vector<TARGET_DATA>& Targets,
	bool bIsGoblinSkill)
{
	ComposerMap::iterator it = m_ComposerMap.find(nSkillID);
	if (it == m_ComposerMap.end()) return;
	it->second->Play(nHostID, nCastTargetID, Targets, bIsGoblinSkill);
}
const A3DSkillGfxComposer* A3DSkillGfxComposerMan::GetSkillGfxComposer(int skill)
{
	ComposerMap::iterator it = m_ComposerMap.find(skill);
	if (it == m_ComposerMap.end()) return NULL;
	return it->second;
}
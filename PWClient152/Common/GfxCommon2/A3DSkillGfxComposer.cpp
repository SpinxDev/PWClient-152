#include "StdAfx.h"
#include "A3DSkillGfxComposer.h"

/*
 * V17 11.10.24  add hit sound type (only used in 2.2)
 * V18 12.3.11  add custom move methods 
 * V19 12.4.26	add hittarget multi hook support
 * V20 12.5.8	add delay time (delay after being hit, then fade out)
 * V21 12.6.21  add extra hit target
 * V22 12.11.28 add move method OnTargetFix
 * V23 12.12.20 add link move method custom params
 */
#define	SGC_CUR_VERSION	23

static const char* _format_version		= "Version: %d";
static const char* _format_path			= "Path: %s";
static const char* _format_hook			= "Hook: %s";
static const char* _format_move_mode	= "MoveMode: %d";
static const char* _format_target_mode	= "TargetMode: %d";
static const char* _format_att_mode		= "AttMode: %d";
static const char* _format_count		= "Count: %d";
static const char* _format_radius		= "Radius: %f";
static const char* _format_interv		= "Interv: %d";
static const char* _format_flyspeed		= "FlySpeed: %f";
static const char* _format_flytime		= "FlyTime: %d";
static const char* _format_stopemitarr	= "StopEmit: %d";	// stop emit when arrived
static const char* _format_rel_scle		= "RelScl: %d";
static const char* _format_rel_scale_max = "MaxScl: %f";
static const char* _format_rel_scale_min = "MinScl: %f";
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
static const char* _format_dir			= "Dir: %f, %f, %f";
static const char* _format_rel_hook		= "RelHook: %d";
static const char* _format_child_hook	= "ChildHook: %d";
static const char* _format_hanger		= "Hanger: %s";
static const char* _format_staytime		= "StayTime: %d";
static const char* _format_hit_pos		= "HitPos: %d";
static const char* _format_cmd_str		= "CmdStr: %s";
static const char* _format_hit_sound_type = "HitSoundType: %d";
static const char* _format_hit_hook_num	= "HitHookNum: %d";
static const char* _format_delaytime	= "DelayTime: %d";

namespace _SGC
{

void A3DSkillGfxComposer::SetDefault()
{
	memset(m_szFlyGfx, 0, sizeof(m_szFlyGfx));
	memset(m_szHitGrndGfx, 0, sizeof(m_szHitGrndGfx));
	m_FlyPos.Clear();
	m_FlyEndPos.Clear();
	m_HitPos.Clear();
	m_GroundPos.Clear();
	m_fFlyGfxScale			= 1.0f;
	m_MoveMode				= enumLinearMove;
	m_TargetMode			= enumHostToTarget;
	m_AttFlyMode			= enumAttPoint;
	m_FlyCluster.m_ulCount	= 1;
	m_FlyCluster.m_dwInterv	= 0;
	m_bOneHit				= true;
	m_bFadeOut				= false;
	m_fFlySpeed				= 20.0f;
	m_dwFlyTime				= 0;
	m_dwStayTime			= 0;
	m_dwDelayTime			= 0;
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
	m_fMinHitGfxScaleFinal	= 0.01f;
	m_fMaxHitGfxScaleFinal	= 99999.f;
	m_vHitGfxDir.Clear();
	m_bStopEmit				= false;
	m_iHitSoundType			= 0;
	memset(m_szExtraHitGfx, 0, sizeof(m_szExtraHitGfx));
	m_fExtraDefTarScl		= 1.8f;
	m_bExtraRelScl			= true;
	m_fExtraMinHitGfxScaleFinal	= 0.01f;
	m_fExtraMaxHitGfxScaleFinal	= 99999.f;
	m_extraHitPos		=	enumHitCenter;
	m_bExtraTraceTarget		= false;

	for (int iCmdStr = 0; iCmdStr < PARAM_STRING_NUM; ++iCmdStr)
		memset(m_szCmd[iCmdStr], 0, PARAM_STRING_LEN);

	delete m_pGfxCustomMoveParam;
	m_pGfxCustomMoveParam = GFX_CUSTOM_MOVE_PARAM::CreateCustomMoveParam(m_MoveMode);
}

bool A3DSkillGfxComposer::SetCommand(int iIdx, const char* szCmd) 
{
	if (iIdx < 0 || iIdx >= PARAM_STRING_NUM)
		return false;

	if (strlen(szCmd) >= PARAM_STRING_LEN - 1)
		return false;

	strcpy(&m_szCmd[iIdx][0], szCmd);
	return true;
}

bool A3DSkillGfxComposer::Load(const char* szFile)
{
	if (!szFile || !szFile[0])
		return false;

	if (!af_CheckFileExt(szFile, ".att", 4))
	{
		assert("GfxComposer::Load: FileExt is not .att !");
		return false;
	}

	AFileImage file;
	if (!file.Open("", szFile, AFILE_OPENEXIST | AFILE_TEMPMEMORY))
		return false;

	if (!Load(&file))
	{
		file.Close();
		return false;
	}
	file.Close();
	return true;
}

inline void _load_sgc_pos(DWORD dwVersion, AFileImage* pFile, char* szLine, SGC_POS_INFO& Pos)
{
	DWORD dwReadLen;
	int nRead;

	if (dwVersion >= 7)
	{
		Pos.strHooks.clear();
		Pos.szHook[0] = '\0';
		if (dwVersion < 19)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_hook, Pos.szHook);
			if (strlen(Pos.szHook))
				Pos.strHooks.push_back(Pos.szHook);
		}
		else
		{
			int numHooks = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_hit_hook_num, &numHooks);

			char strHook[80];
			memset(strHook,0,80);
			for (int i=0; i<numHooks; ++i)
			{
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_hook, strHook);
				Pos.strHooks.push_back(strHook);
			}
			if(!Pos.strHooks.empty())
				strcpy_s(Pos.szHook, 80, Pos.strHooks[0]);
			else
				memset(Pos.szHook, 0, 80);
		}
	}

	if (dwVersion >= 8)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_pos, VECTORADDR_XYZ(Pos.vOffset));
	}
	else
		Pos.vOffset.Clear();

	if (dwVersion >= 9)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_rel_hook, &nRead);
		Pos.bRelHook = (nRead != 0);
	}
	else
		Pos.bRelHook = false;

	Pos.szHanger[0] = '\0';
	Pos.bChildHook = false;

	if (dwVersion >= 10)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_hanger, Pos.szHanger);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_child_hook, &nRead);
		Pos.bChildHook = (nRead != 0);
	}

	if (dwVersion >= 12)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_hit_pos, &Pos.HitPos);
	}
	else
		Pos.HitPos = enumHitCenter;
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
		
		if (dwVersion > SGC_CUR_VERSION)
		{
			return false;
		}

		if (dwVersion > 0) pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		m_szFlyGfx[0] = '\0';
		sscanf(szLine, _format_path, m_szFlyGfx);

		_load_sgc_pos(dwVersion, pFile, szLine, m_FlyPos);

		if (dwVersion >= 13)
			_load_sgc_pos(dwVersion, pFile, szLine, m_FlyEndPos);

		m_szHitGrndGfx[0] = '\0';
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_path, m_szHitGrndGfx);

		if (dwVersion >= 5)
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);

		if (dwVersion >= 3)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_scale, &m_fFlyGfxScale);
		}
		else
			m_fFlyGfxScale = 1.0f;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_move_mode, &m_MoveMode);

		if (dwVersion >= 18)
		{
			delete m_pGfxCustomMoveParam;
			m_pGfxCustomMoveParam = GFX_CUSTOM_MOVE_PARAM::CreateCustomMoveParam(m_MoveMode);
			if (m_pGfxCustomMoveParam)
			{
				m_pGfxCustomMoveParam->Load(pFile,dwVersion);
			}
		}

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

		if (dwVersion >= 6)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_flyspeed, &m_fFlySpeed);
		}

		if (dwVersion >= 12)
		{
			_load_sgc_pos(dwVersion, pFile, szLine, m_HitPos);
			_load_sgc_pos(dwVersion, pFile, szLine, m_GroundPos);
		}

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

		if (dwVersion >= 11) {
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, & dwReadLen);
			sscanf(szLine, _format_staytime, &m_dwStayTime);
		}
		else
			m_dwStayTime = 0;

		if (dwVersion >= 20)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, & dwReadLen);
			sscanf(szLine, _format_delaytime, &m_dwDelayTime);
		}
		else
			m_dwDelayTime = 0;

		if (dwVersion >= 14) 
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_rel_scale_max, &m_fMaxHitGfxScaleFinal);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_rel_scale_min, &m_fMinHitGfxScaleFinal);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_dir, &m_vHitGfxDir.x, &m_vHitGfxDir.y, &m_vHitGfxDir.z);
		}
		else
		{
			m_fMaxHitGfxScaleFinal = 99999.f;
			m_fMinHitGfxScaleFinal = 0.01f;
			m_vHitGfxDir.Clear();
		}

		if (dwVersion >= 15)
		{
			int iReadInt = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_stopemitarr, &iReadInt);
			m_bStopEmit = (iReadInt != 0);
		}

		if (dwVersion >= 16)
		{
			for (int iCmdStr = 0; iCmdStr < PARAM_STRING_NUM; ++iCmdStr)
			{
				m_szCmd[iCmdStr][0] = '\0';
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_cmd_str, m_szCmd[iCmdStr]);
			}
		}

		if (dwVersion >= 17)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_hit_sound_type, &m_iHitSoundType);
		}
		else
		{
			m_iHitSoundType = -1;
		}

		if (dwVersion < 12)
		{
			if (m_MoveMode == enumOnTarget)
			{
				m_FlyPos.HitPos = enumHitBottom;
				m_FlyEndPos.HitPos = enumHitBottom;
			}
		}

		
		m_szExtraHitGfx[0] = '\0';
		if (dwVersion >= 21)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_tar_scl, &m_fExtraDefTarScl);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_rel_scle, &nRead);
			m_bExtraRelScl = (nRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_rel_scale_min, &m_fExtraMinHitGfxScaleFinal);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_rel_scale_max, &m_fExtraMaxHitGfxScaleFinal);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_path, m_szExtraHitGfx);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_hit_pos, &m_extraHitPos);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_trace, &nRead);	
			m_bExtraTraceTarget = (nRead != 0);
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

inline void _save_sgc_pos(AFile* pFile, char* szLine, SGC_POS_INFO& Pos)
{
// 	sprintf(szLine, _format_hook, Pos.szHook);
// 	pFile->WriteLine(szLine);

	int numHooks = Pos.strHooks.size();
	sprintf(szLine, _format_hit_hook_num, numHooks);
	pFile->WriteLine(szLine);

	for (int i=0; i<numHooks; ++i)
	{
		sprintf(szLine, _format_hook, Pos.strHooks[i]);
		pFile->WriteLine(szLine);
	}

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

		_save_sgc_pos(pFile, szLine, m_FlyPos);

		_save_sgc_pos(pFile, szLine, m_FlyEndPos);

		sprintf(szLine, _format_path, m_szHitGrndGfx);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_path, "");
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_scale, m_fFlyGfxScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_move_mode, m_MoveMode);
		pFile->WriteLine(szLine);

		if (m_pGfxCustomMoveParam)
		{
			m_pGfxCustomMoveParam->Save(pFile);
		}

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

		sprintf(szLine, _format_flyspeed, m_fFlySpeed);
		pFile->WriteLine(szLine);

		_save_sgc_pos(pFile, szLine, m_HitPos);
		_save_sgc_pos(pFile, szLine, m_GroundPos);

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

		sprintf(szLine, _format_staytime, m_dwStayTime);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_delaytime, m_dwDelayTime);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_rel_scale_max, m_fMaxHitGfxScaleFinal);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_rel_scale_min, m_fMinHitGfxScaleFinal);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_dir, m_vHitGfxDir.x, m_vHitGfxDir.y, m_vHitGfxDir.z);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_stopemitarr, m_bStopEmit);
		pFile->WriteLine(szLine);

		for (int iCmdStr = 0; iCmdStr < PARAM_STRING_NUM; ++iCmdStr)
		{
			_snprintf(szLine, sizeof(szLine), _format_cmd_str, m_szCmd[iCmdStr]);
			pFile->WriteLine(szLine);
		}

		_snprintf(szLine, sizeof(szLine), _format_hit_sound_type, m_iHitSoundType);
		pFile->WriteLine(szLine);

		//extra hit

		_snprintf(szLine, sizeof(szLine), _format_tar_scl, m_fExtraDefTarScl);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_rel_scle, m_bExtraRelScl);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_rel_scale_min, m_fExtraMinHitGfxScaleFinal);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_rel_scale_max, m_fExtraMaxHitGfxScaleFinal);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_path, m_szExtraHitGfx);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_hit_pos, m_extraHitPos);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_trace, m_bExtraTraceTarget);
		pFile->WriteLine(szLine);

	}

	return true;
}

void A3DSkillGfxComposer::Play(
	unsigned char SerialID,
	clientid_t nCasterID,
	clientid_t nCastTargetID,
	const A3DVECTOR3* pFixedPoint,
	float fSpeed,
	GfxFlyHitMode mode,
	int nFlySfxPriority,
	int nHitSfxPriority,
	int nAttIndex,
	int nAttOrientation,
	float fGfxScale/* = 1.f*/,
	bool bAllExtraHit)
{
	clientid_t _Host, _Target;
	bool bReverse;

	switch (m_TargetMode)
	{
	case enumTargetToHost:
	case enumHostDescend:
	case enumHostAscend:
	case enumHostSelf:
	case enumTargetLinkHost:
		_Host = nCastTargetID;
		_Target = nCasterID;
		bReverse = true;
		break;
	default:
		_Host = nCasterID;
		_Target = nCastTargetID;
		bReverse = false;
		break;
	}

	SKILL_GFX_EVENT_PARAMS params;

	for (unsigned long i = 0; i < m_FlyCluster.m_ulCount; i++)
	{
		params.nCasterID			=	nCasterID;
		params.nCastTargetID		=	nCastTargetID;
		params.SerialID				=	SerialID;
		params.pFixedPt				=	pFixedPoint;
		params.nHostID				=	_Host;
		params.nTargetID			=	_Target;
		params.fFlySpeed			=	fSpeed > 0.0f? fSpeed : m_fFlySpeed;
		params.dwDelayTime			=	m_FlyCluster.m_dwInterv * i;
		params.bHasTarget			=	i == 0;
		params.bReverse				=	bReverse;
		params.mode					=	mode;
		params.nFlySfxPriority		=	nFlySfxPriority;
		params.nHitSfxPriority		=	nHitSfxPriority;
		params.nAttIndex			=	nAttIndex;
		params.nAttOrientation		=	nAttOrientation;
		params.fGfxScale			=	fGfxScale;
		params.bAllExtraHit			=	bAllExtraHit;
		AfxGetSkillGfxEventMan()->AddSkillGfxEvent(this, params, i);
	}
}

A3DSkillGfxComposer::A3DSkillGfxComposer()
{	
	m_pGfxCustomMoveParam = NULL;
	m_FlyPos.Clear();
	m_FlyEndPos.Clear();
	m_HitPos.Clear();
	m_GroundPos.Clear();	
}

A3DSkillGfxComposer::~A3DSkillGfxComposer()
{
	delete m_pGfxCustomMoveParam;
}

A3DSkillGfxComposer& A3DSkillGfxComposer::operator = (const A3DSkillGfxComposer& src)
{
	if (&src == this)
		return *this;

	strncpy(m_szFlyGfx, src.m_szFlyGfx, MAX_PATH);

	m_FlyPos = src.m_FlyPos;

	m_FlyEndPos = src.m_FlyEndPos;

	strncpy(m_szHitGrndGfx, src.m_szHitGrndGfx,MAX_PATH);

	m_fFlyGfxScale = src.m_fFlyGfxScale;

	m_MoveMode = src.m_MoveMode;

	if (src.m_pGfxCustomMoveParam)
	{
		delete m_pGfxCustomMoveParam;
		m_pGfxCustomMoveParam = GFX_CUSTOM_MOVE_PARAM::CreateCustomMoveParam(m_MoveMode);
		if (m_pGfxCustomMoveParam)
			src.m_pGfxCustomMoveParam->Clone(m_pGfxCustomMoveParam);
	}

	m_TargetMode = src.m_TargetMode;

	m_AttFlyMode = src.m_AttFlyMode;

	m_FlyCluster = src.m_FlyCluster;

	m_bOneHit = src.m_bOneHit;

	m_bFadeOut = src.m_bFadeOut;

	m_param = src.m_param;

	m_dwFlyTime = src.m_dwFlyTime;

	m_fFlySpeed = src.m_fFlySpeed;

	m_HitPos = src.m_HitPos;

	m_GroundPos = src.m_GroundPos;

	m_bRelScl = src.m_bRelScl;

	m_fDefTarScl = src.m_fDefTarScl;

	strncpy(m_szHitGfx, src.m_szHitGfx, MAX_PATH);

	m_fHitGfxScale = src.m_fHitGfxScale;

	m_AttHitMode = src.m_AttHitMode;

	m_HitCluster = src.m_HitCluster;

	m_bTraceTarget = src.m_bTraceTarget;

	m_paramType = src.m_paramType;

	m_dwStayTime = src.m_dwStayTime;

	m_dwDelayTime = src.m_dwDelayTime;

	m_fMaxHitGfxScaleFinal = src.m_fMaxHitGfxScaleFinal;

	m_fMinHitGfxScaleFinal = src.m_fMinHitGfxScaleFinal;

	m_vHitGfxDir = src.m_vHitGfxDir;

	m_bStopEmit	= src.m_bStopEmit;

	for (int iCmdStr = 0; iCmdStr < PARAM_STRING_NUM; ++iCmdStr)
	{
		strncpy(m_szCmd[iCmdStr], src.m_szCmd[iCmdStr], PARAM_STRING_LEN);
	}

	m_iHitSoundType = src.m_iHitSoundType;

	m_bExtraRelScl	= src.m_bExtraRelScl;

	m_fExtraDefTarScl = src.m_fExtraDefTarScl;

	m_fExtraMinHitGfxScaleFinal	= src.m_fExtraMinHitGfxScaleFinal;

	m_fExtraMaxHitGfxScaleFinal = src.m_fExtraMaxHitGfxScaleFinal;

	m_extraHitPos	= src.m_extraHitPos;

	strncpy(m_szExtraHitGfx, src.m_szExtraHitGfx, MAX_PATH);

	m_bExtraTraceTarget	= src.m_bExtraTraceTarget;

	return *this;
}

void A3DSkillGfxComposerMan::Release()
{
	ComposerMap::iterator it = m_ComposerMap.begin();
	for (; it != m_ComposerMap.end(); ++it) delete it->second;
	m_ComposerMap.clear();
}

void A3DSkillGfxComposerMan::Play(
	const AString& strAtkFile,
	unsigned char SerialID,
	clientid_t nCasterID,
	clientid_t nCastTargetID,
	const A3DVECTOR3* pFixedPoint,
	float fSpeed,
	GfxFlyHitMode mode,
	int nFlySfxPriority,
	int nHitSfxPriority,
	int nAttIndex,
	int nAttOrientation,
	float fGfxScale/* = 1.f*/,
	bool bAllExtraHit)
{
	ComposerMap::iterator it = m_ComposerMap.find(strAtkFile);

	if (it == m_ComposerMap.end())
	{
		A3DSkillGfxComposer* pComposer = new A3DSkillGfxComposer();

		if (!pComposer->Load(m_strBaseDir + strAtkFile))
		{
			delete pComposer;
			return;
		}

		m_ComposerMap[strAtkFile] = pComposer;
		pComposer->Play(SerialID, nCasterID, nCastTargetID, pFixedPoint, fSpeed, mode, nFlySfxPriority, nHitSfxPriority, nAttIndex, nAttOrientation, fGfxScale, bAllExtraHit);
	}
	else
	{
		A3DSkillGfxComposer* pComposer = it->second;
		pComposer->Play(SerialID, nCasterID, nCastTargetID, pFixedPoint, fSpeed, mode, nFlySfxPriority, nHitSfxPriority, nAttIndex, nAttOrientation, fGfxScale, bAllExtraHit);
	}
}

A3DSkillGfxComposer* A3DSkillGfxComposerMan::LoadSkillGfx(const AString& strAtkFile)
{
	ComposerMap::iterator it = m_ComposerMap.find(strAtkFile);

	if (it == m_ComposerMap.end())
	{
		A3DSkillGfxComposer* pComposer = new A3DSkillGfxComposer();

		if (!pComposer->Load(m_strBaseDir + strAtkFile))
		{
			delete pComposer;
			return NULL;
		}

		m_ComposerMap[strAtkFile] = pComposer;
		return pComposer;
	}
	else
		return it->second;
}

}

#ifdef _SKILLGFXCOMPOSER

namespace _SGC
{
	static A3DSkillGfxComposerMan _composer_man;

	A3DSkillGfxComposerMan* AfxGetSkillGfxComposerMan()
	{
		return &_composer_man;
	}
}

bool AfxPlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets)
{
	using namespace _SGC;
	A3DSkillGfxComposerMan* pMan = AfxGetSkillGfxComposerMan();
	A3DSkillGfxMan* pEventMan = AfxGetSkillGfxEventMan();

	pMan->Play(
		strAtkFile,
		SerialID,
		nCasterID,
		nCastTargetID,
		pFixedPoint,
		0.0f,
		enumAttBothFlyHit | enumAttHitGround);

	A3DSkillGfxEvent* pEvent = pEventMan->GetSkillGfxEvent(nCasterID, SerialID);

	if (!pEvent)
		return true;

	pEvent = pEvent->GetLast();
	pEvent->SetDivisions(nDivisions);

	TARGET_DATA td;
	td.idTarget = 2;
	td.nDamage = 100;
	td.dwModifier = 0;
	pEvent->AddOneTarget(td);
	return true;
}

void AfxSkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier)
{
	using namespace _SGC;
	if (A3DSkillGfxMan::GetDamageShowFunc())
		A3DSkillGfxMan::GetDamageShowFunc()(idCaster, idTarget, nDamage, 1, dwModifier);
}

void AfxSkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier)
{
}

bool AfxSkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage)
{
	using namespace _SGC;
	A3DSkillGfxMan* pMan = AfxGetSkillGfxEventMan();
	A3DSkillGfxEvent* pEvent = pMan->GetSkillGfxEvent(nCaster, SerialId);

	if (pEvent)
	{
		TARGET_DATA td;
		td.dwModifier = dwModifier;
		td.idTarget = nTarget;
		td.nDamage = nDamage;

		if (!pEvent->AddOneTarget(td))
		{
			if(A3DSkillGfxMan::GetDamageShowFunc())
				A3DSkillGfxMan::GetDamageShowFunc()(nCaster, nTarget, nDamage, 1, td.dwModifier);
		}

		return true;
	}

	return false;
}

#endif

void _get_rand_damage(int damage, int divisions, int* parts)
{
	memset(parts, 0, sizeof(int) * divisions);

	int p = int((float)rand() / RAND_MAX + .5f);
	float pp = powf(-1.0f, (float)p);

	if (damage == divisions)
	{
		for (int i = 0; i < divisions; i++)
			parts[i] = 1;
	}
	else if (damage < divisions)
	{
		int d = int(.5f + pp * .5f);

		for (int i = 0; i < divisions; i++)
			parts[i] = d;
	}
	else
	{
		int y = damage;

		for (int i = 1; i <= divisions; i++)
		{
			if (y >= 0)
			{
				if (i <= divisions-1)
				{
					int t = int(pp * rand() * damage / (RAND_MAX * 20.f * divisions) + 1.0f);
					int o = int((float)damage / divisions + (float)t + .5f);
					y -= o;
					parts[i-1] = o;
				}
				else
				{
					parts[i-1] = y;
				}
			}
		}
	}
}

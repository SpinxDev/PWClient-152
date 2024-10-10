#include "StdAfx.h"
#include "AMSoundBufferMan.h"
#include "A3DCombinedAction.h"
#include "A3DCombActDynData.h"
#include "EC_Model.h"
#include "EC_ModelMan.h"
#include "AFile.h"
#include "A3DEngine.h"
#include "AMSoundEngine.h"
#include "A3DSkinModel.h"
#include "A3DSkinModelAux.h"
#include "A3DSkinModelAct.h"
#include "A3DBone.h"
#include "A3DCameraBase.h"
#include "AM3DSoundDevice.h"
#include "A3DSkillGfxEvent.h"
#include "FX_BINDING.H"
#include "RandStringContainer.h"
#include "base64.h"
#include "A3DGFXInterface.h"

extern const char* _script_var_name[];

static const char* _format_cact_name		= "CombineActName: %s";

static const char* _format_act_count		= "BaseActCount: %d";
static const char* _format_act_name			= "BaseActName: %s";
static const char* _format_act_start_time	= "ActStartTime: %d";
static const char* _format_act_LoopCount	= "LoopCount: %d";
static const char* _format_act_LoopMinNum	= "LoopMinNum: %d";
static const char* _format_act_LoopMaxNum	= "LoopMaxNum: %d";

static const char* _format_start_time		= "StartTime: %d";
static const char* _format_time_span		= "TimeSpan: %d";
static const char* _format_rank_count		= "RankCount: %d";
static const char* _format_rank				= "Channel: %d, Rank: %d";
static const char* _format_event_channel	= "EventChannel: %d";
static const char* _format_play_speed		= "PlaySpeed: %f";
static const char* _format_stopchildact		= "StopChildAct: %d";
static const char* _format_resetmtlonstop	= "ResetMtl: %d";

static const char* _format_once				= "Once: %d";
static const char* _format_fx_count			= "FxCount: %d";
static const char* _format_fx_type			= "FxType: %d";
static const char* _format_fx_start_time	= "FxStartTime: %d";
static const char* _format_fx_path_num		= "FxFileNum: %d";
static const char* _format_fx_path			= "FxFilePath: %s";
static const char* _format_hook_name		= "HookName: %s";
static const char* _format_hook_offset		= "HookOffset: %f, %f, %f";
static const char* _format_hook_yaw			= "HookYaw: %f";
static const char* _format_hook_pitch		= "HookPitch: %f";
static const char* _format_hook_rot			= "HookRot: %f";
static const char* _format_bind_parent		= "BindParent: %d";
static const char* _format_fadeout			= "FadeOut: %d";
static const char* _format_model_alpha		= "UseModelAlpha: %d";
static const char* _format_custom_path		= "CustomPath: %d";
static const char* _format_atk_path			= "AtkPath: %s";
static const char* _format_divisions		= "Divisions: %d";
static const char* _format_atk_usedelay		= "AtkUseDelay: %d";
static const char* _format_atk_delaycount	= "AtkDelayNum: %d";
static const char* _format_atk_delaytime	= "AtkDelayTime: %d";
static const char* _format_atk_orientation	= "AtkOrient: %d";
static const char* _format_custom_data		= "CustomData: %d";

static const char* _format_gfx_scale		= "GfxScale: %f";
static const char* _format_gfx_alpha		= "GfxAlpha: %f";
static const char* _format_gfx_play_speed	= "GfxSpeed: %f";
static const char* _format_gfx_outer_path	= "GfxOuterPath: %d";
static const char* _format_gfx_rel_ecm		= "GfxRelToECM: %d";
static const char* _format_gfx_param_count	= "GfxParamCount: %d";
static const char* _format_gfx_delay_time	= "GfxDelayTime: %d";
static const char* _format_gfx_rot_with_model = "GfxRotWithModel: %d";
static const char* _format_gfx_use_fixedpoint = "GfxUseFixedPoint: %d";

static const char* _format_param_ele_name	= "ParamEleName: %s";
static const char* _format_param_id			= "ParamId: %d";

static const char* _format_param_type		= "ParamDataType: %d";
static const char* _format_param_is_cmd		= "ParamDataIsCmd: %d";
static const char* _format_param_cmd		= "ParamDataCmd: %s";
static const char* _format_param_pos		= "ParamDataPos: %f, %f, %f";
static const char* _format_param_hook		= "ParamDataHook: %s";

static const char* _format_child_act_count	= "ChildActCount: %d";
static const char* _format_child_act_name	= "ChildActName: %s";
static const char* _format_chld_hhname		= "HHName: %s";
static const char* _format_transtime		= "TransTime: %d";
static const char* _format_chld_istrail		= "IsTrail: %d";
static const char* _format_chld_span		= "TrailSpan: %d";
static const char* _format_chld_segs		= "Segs: %d";
static const char* _format_pos				= "Pos: %f, %f, %f";
static const char* _format_dir				= "Dir: %f, %f, %f, %f";

static const char* _format_matchg			= "ColorValue: %f, %f, %f, %f";
static const char* _format_apply_child		= "ApplyChild: %d";

static const char* _format_mst_orgcol			= "OrgColor: %d";
static const char* _format_mst_destnum			= "DestNum: %d";
static const char* _format_mst_destcol			= "Col: %d";
static const char* _format_mst_desttime			= "Time: %f";


static const char* _format_event_type		= "EventType: %d";
static const char* _format_event_count		= "EventCount: %d";

static const char* _format_script_lines		= "ScriptLines: %d";
static const char* _format_script_cfg_state	= "ScriptCfgState: %d";
static const char* _format_script_usage		= "ScriptUsage: %d";

static const char* _format_cam_dist2host	= "Dist2Host: %f";
static const char* _format_cam_yaw2host		= "Yaw2Host: %f";
static const char* _format_cam_pitch		= "Pitch: %f";
static const char* _format_cam_yawacc		= "YawAcc: %f";
static const char* _format_cam_pitchacc		= "PitchAcc: %f";
static const char* _format_cam_angleacc		= "AngleAcc: %f";
static const char* _format_cam_linearacc	= "LinearAcc: %f";
static const char* _format_cam_isinterp		= "IsInterp: %d";
static const char* _format_cam_beziernum	= "BezierNum: %d";
static const char* _format_cam_beziervert	= "%f %f %f";

//	AUDIOENGINE_INFO
static const char* _format_audioevent		= "AudioEvent: %s";
static const char* _format_audiomindist		= "MinDist: %f";
static const char* _format_audiomaxdist		= "MaxDist: %f";
static const char* _format_audiousecustom	= "Custom: %d";
static const char* _format_audiovolume		= "Volume: %f";
static const char* _format_audiogroup		= "Group: %d";
static const char* _format_audiofadeout		= "FadeOut: %d";

const DWORD _trail_delta = 3;

extern void _DelayUpdateGfxParentTM(A3DGFXEx* pGfx, const A3DMATRIX4& dstTM, float fPortion);

A3DCombinedAction::A3DCombinedAction() 
	: m_nLoops(1)
	, m_bInfinite(false)
	, m_nEventChannel(0)
	, m_fPlaySpeed(1.f)
	, m_bResetMaterialScale(true)
	, m_bStopChildrenAct(false)
	, m_eSpecType( SPECIALTYPE_NONE )
	, m_iWalkRunEventFromCombAct(-1)
	, m_pWalkRunEventFromCombAct( NULL )
{
	memset(m_Ranks, 0, sizeof(m_Ranks));
	memset(m_aEventCounter, 0, sizeof(m_aEventCounter));
}

A3DCombinedAction& A3DCombinedAction::operator = (const A3DCombinedAction& src)
{
	if (this == &src)
		return *this;

	m_nLoops			= src.m_nLoops;
	m_dwComActMinSpan	= src.m_dwComActMinSpan;
	m_bInfinite			= src.m_bInfinite;
	memcpy(m_Ranks, src.m_Ranks, sizeof(m_Ranks));
	memcpy(m_aEventCounter, src.m_aEventCounter, sizeof(m_aEventCounter));

	m_ActLst.RemoveAll();
	m_EventInfoLst.RemoveAll();

	ALISTPOSITION pos = src.m_ActLst.GetHeadPosition();

	while (pos)
		m_ActLst.AddTail(new ACTION_INFO(*src.m_ActLst.GetNext(pos)));

	pos = src.m_EventInfoLst.GetHeadPosition();

	while (pos)
		m_EventInfoLst.AddTail(EVENT_INFO::CloneFrom(this, *src.m_EventInfoLst.GetNext(pos)));

	return *this;
}


bool A3DCombinedAction::Load(A3DDevice* pDev, AFile* pFile, DWORD dwVersion)
{
	DWORD dwReadLen;
	int nActCount = 0;
	int nFxCount = 0;
	int nChildCount = 0;
	int nEventCount = 0;

	if (pFile->IsBinary())
	{
		pFile->ReadString(m_strName);
		pFile->Read(&m_nLoops, sizeof(m_nLoops), &dwReadLen);
		pFile->Read(&nEventCount, sizeof(nEventCount), &dwReadLen);

		for (int i = 0; i < nEventCount; i++)
		{
			EVENT_INFO* pEvent = EVENT_INFO::LoadFromFile(this, pFile, dwVersion);
			if (!pEvent) continue;
			m_EventInfoLst.AddTail(pEvent);
		}
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		char	szBuf[AFILE_LINEMAXLEN];
		szBuf[0] = '\0';

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_cact_name, szBuf);
		SetName(szBuf);

		if (dwVersion >= 3)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_act_LoopCount, &m_nLoops);
		}
		else
			m_nLoops = 1;

		if (dwVersion >= 30)
		{
			int rank_count = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_rank_count, &rank_count);

			for (int i = 0; i < rank_count; i++)
			{
				int channel = 0;
				int rank = 0;
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_rank, &channel, &rank);

				if (channel >= 0 && channel < A3DSkinModel::ACTCHA_MAX)
					m_Ranks[channel] = rank;
			}
		}

		if (dwVersion >= 32)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_event_channel, &m_nEventChannel);
		}

		if (dwVersion >= 40)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_play_speed, &m_fPlaySpeed);
		}

		if (dwVersion >= 49)
		{
			int iRead = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_stopchildact, &iRead);
			m_bStopChildrenAct = (iRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_resetmtlonstop, &iRead);
			m_bResetMaterialScale = (iRead != 0);
		}

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_act_count, &nActCount);

		int i;

		for (i = 0; i < nActCount; i++)
		{
			PACTION_INFO pInfo = new ACTION_INFO;
			pInfo->Load(pFile, dwVersion);
			m_ActLst.AddTail(pInfo);
		}

		if (m_nLoops == -1 && nActCount == 1)
			m_ActLst.GetHead()->SetLoops(-1, -1);

		if (dwVersion < 7)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_fx_count, &nFxCount);

			for (i = 0; i < nFxCount; i++)
			{
				PFX_BASE_INFO pInfo = FX_BASE_INFO::LoadFromFile(this, pFile, dwVersion);
				if (pInfo == NULL) continue;
				pInfo->Load(pFile, dwVersion);
				m_EventInfoLst.AddTail(pInfo);
			}

			if (dwVersion >= 5)
			{
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_child_act_count, &nChildCount);

				for (i = 0; i < nChildCount; i++)
				{
					ChildActInfo* pChild = new ChildActInfo(this);
					pChild->Load(pFile, dwVersion);
					m_EventInfoLst.AddTail(pChild);
				}
			}

			SortEventInfoList();
		}
		else
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_event_count, &nEventCount);

			for (i = 0; i < nEventCount; i++)
			{
				EVENT_INFO* pEvent = EVENT_INFO::LoadFromFile(this, pFile, dwVersion);
				if (!pEvent) continue;
				m_EventInfoLst.AddTail(pEvent);

				if (pEvent->GetType() > EVENT_TYPE_END)
					continue;

				m_aEventCounter[pEvent->GetType() - EVENT_TYPE_BASE]++;
			}
		}
	}

	ALISTPOSITION pos = m_EventInfoLst.GetHeadPosition();
	while (pos)	m_EventInfoLst.GetNext(pos)->Init(pDev);

	pos = m_ActLst.GetHeadPosition();
	while (pos)
	{
		if (m_ActLst.GetNext(pos)->IsInfinite()/*GetLoops() == -1*/)
		{
			m_bInfinite = true;
			break;
		}
	}

	return true;
}

bool A3DCombinedAction::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;

		pFile->WriteString(m_strName);
		pFile->Write(&m_nLoops, sizeof(m_nLoops), &dwWrite);

		int nActCount = m_ActLst.GetCount();
		pFile->Write(&nActCount, sizeof(nActCount), &dwWrite);

		int i;

		for (i = 0; i < m_ActLst.GetCount(); i++)
			m_ActLst.GetByIndex(i)->Save(pFile);

		int nEventCount = m_EventInfoLst.GetCount();
		pFile->Write(&nEventCount, sizeof(nEventCount), &dwWrite);

		for (i = 0; i < m_EventInfoLst.GetCount(); i++)
			m_EventInfoLst.GetByIndex(i)->Save(pFile);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		int		i;

		sprintf(szLine, _format_cact_name, GetName());
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_act_LoopCount, m_nLoops);
		pFile->WriteLine(szLine);

		int rank_count = 0;

		for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		{
			if (m_Ranks[i])
				rank_count++;
		}

		sprintf(szLine, _format_rank_count, rank_count);
		pFile->WriteLine(szLine);

		for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		{
			if (m_Ranks[i])
			{
				sprintf(szLine, _format_rank, i, (int)m_Ranks[i]);
				pFile->WriteLine(szLine);
			}
		}

		sprintf(szLine, _format_event_channel, m_nEventChannel);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_play_speed, m_fPlaySpeed);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_stopchildact, m_bStopChildrenAct);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_resetmtlonstop, m_bResetMaterialScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_act_count, m_ActLst.GetCount());
		pFile->WriteLine(szLine);

		for (i = 0; i < m_ActLst.GetCount(); i++)
			m_ActLst.GetByIndex(i)->Save(pFile);

		sprintf(szLine, _format_event_count, m_EventInfoLst.GetCount());
		pFile->WriteLine(szLine);

		for (i = 0; i < m_EventInfoLst.GetCount(); i++)
			m_EventInfoLst.GetByIndex(i)->Save(pFile);
	}

	return true;
}

EVENT_INFO* EVENT_INFO::LoadFromFile(
	A3DCombinedAction* pAct,
	AFile* pFile,
	DWORD dwVersion)
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;
	int		nType = EVENT_TYPE_NONE;

	if (pFile->IsBinary())
		pFile->Read(&nType, sizeof(nType), &dwReadLen);
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_event_type, &nType);
	}

	EVENT_INFO* pEvent = CreateEventByType(pAct, nType);
	if (!pEvent) return NULL;

	if (dwVersion >= 18 && !pEvent->LoadEventBase(pFile, dwVersion)
	 || !pEvent->Load(pFile, dwVersion))
	{
		delete pEvent;
		return NULL;
	}

	return pEvent;
}

bool EVENT_INFO::LoadEventBase(AFile* pFile, DWORD dwVersion)
{
	char	szLine[AFILE_LINEMAXLEN];
	DWORD	dwReadLen;
	int		nRead;

	if (pFile->IsBinary())
	{
		pFile->Read(&m_dwStartTime, sizeof(m_dwStartTime), &dwReadLen);
		pFile->Read(&m_dwTimeSpan, sizeof(m_dwTimeSpan), &dwReadLen);
		pFile->Read(&m_bOnce, sizeof(m_bOnce), &dwReadLen);
	}
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_start_time, &m_dwStartTime);

		if (dwVersion >= 20)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_time_span, &m_dwTimeSpan);
		}

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_once, &nRead);
		m_bOnce = (nRead != 0);
	}

	return true;
}

void EVENT_INFO::SaveEventBase(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;

		pFile->Write(&m_nType, sizeof(m_nType), &dwWrite);
		pFile->Write(&m_dwStartTime, sizeof(m_dwStartTime), &dwWrite);
		pFile->Write(&m_dwTimeSpan, sizeof(m_dwTimeSpan), &dwWrite);
		pFile->Write(&m_bOnce, sizeof(m_bOnce), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_event_type, m_nType);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_start_time, m_dwStartTime);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_time_span, m_dwTimeSpan);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_once, (int)m_bOnce);
		pFile->WriteLine(szLine);
	}
}

EVENT_INFO* EVENT_INFO::CreateEventByType(A3DCombinedAction* pAct, int nType)
{
	switch (nType)
	{
	case EVENT_TYPE_GFX:
		return new GFX_INFO(pAct);
	case EVENT_TYPE_SFX:
		return new SFX_INFO(pAct);
	case EVENT_TYPE_CHLDACT:
		return new ChildActInfo(pAct);
	case EVENT_TYPE_MATCHG:
		return new MaterialScaleChange(pAct);
	case EVENT_TYPE_ATT_PT:
		return new SGCAttackPointMark(pAct);
	case EVENT_TYPE_SCRIPT:
		return new GfxScriptEvent(pAct);
	case EVENT_TYPE_CAM_PT:
		return new CameraControlEvent(pAct);
	case EVENT_TYPE_MODELSCLCHG:
		return new BoneScaleChange(pAct);
	case EVENT_TYPE_MATTRANS:
		return new MaterialScaleTrans(pAct);
	case EVENT_TYPE_AUDIOEVENT:
		return new AUDIOEVENT_INFO(pAct);
	}

	return NULL;
}

bool ACTION_INFO::Load(AFile* pFile, DWORD dwVersion)
{
	DWORD dwReadLen;

	if (pFile->IsBinary())
	{
		pFile->ReadString(m_strName);
		pFile->Read(&m_dwStartTime, sizeof(m_dwStartTime), &dwReadLen);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		char	szBuf[AFILE_LINEMAXLEN];
		szBuf[0] = '\0';

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_act_name, szBuf);
		m_strName = szBuf;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_act_start_time, &m_dwStartTime);

		if (dwVersion < 6)
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		else if (dwVersion >= 9 && dwVersion < 36)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			int nLoops;
			sscanf(szLine, _format_act_LoopCount, &nLoops);
			m_nMinLoops = m_nMaxLoops = nLoops;
		}
		else if (dwVersion >= 36)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_act_LoopMinNum, &m_nMinLoops);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_act_LoopMaxNum, &m_nMaxLoops);
		}
		
	}

	return true;
}

bool ACTION_INFO::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;

		pFile->WriteString(m_strName);
		pFile->Write(&m_dwStartTime, sizeof(m_dwStartTime), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_act_name, m_strName);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_act_start_time, m_dwStartTime);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_act_LoopMinNum, m_nMinLoops);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_act_LoopMaxNum, m_nMaxLoops);
		pFile->WriteLine(szLine);
	}

	return true;
}

FX_BASE_INFO::FX_BASE_INFO(A3DCombinedAction* pAct) 
: EVENT_INFO(pAct)
, m_vOffset(0)
, m_pFiles(new RandStringContainer(5))
{
	m_strHookName.Empty();
	m_fYaw			= 0;
	m_fPitch		= 0;
	m_fRot			= 0;
	m_bBindParent	= true;
	m_bModelAlpha	= true;
	m_bCustomFilePath = false;
	m_bUseECMHook	= false;
	m_dwFadeOutTime = DEFAULT_GFX_FADE_OUT_TIME;
	m_nCustomData	= 0;
}

FX_BASE_INFO::~FX_BASE_INFO()
{
	delete m_pFiles;
	m_pFiles = NULL;
}

const char* FX_BASE_INFO::GetFilePath() const 
{ 
	if (m_pFiles->GetSize() > 0)
		return m_pFiles->GetString(0);

	return "";
}

void FX_BASE_INFO::SetFilePath(const char* szPath) 
{
	if(m_pFiles->GetSize() > 0)
		m_pFiles->SetString(0, szPath);
	else
		m_pFiles->UniqueAdd(szPath);
}

const char* FX_BASE_INFO::GetRandFilePath() const
{
	return m_pFiles->GetRandString();
}
	
int FX_BASE_INFO::GetFilePathCount() const
{
	return m_pFiles->GetSize();
}

const char* FX_BASE_INFO::GetFilePathByIndex(int iIdx) const
{
	return m_pFiles->GetString(iIdx);
}

void FX_BASE_INFO::SetFadeOut(bool bCan) 
{ 
	if (bCan)
		m_dwFadeOutTime = DEFAULT_GFX_FADE_OUT_TIME; 
	else
		m_dwFadeOutTime = 0;
}

EVENT_INFO& FX_BASE_INFO::Clone(const EVENT_INFO& src)
{
	EVENT_INFO::Clone(src);
	
	const FX_BASE_INFO* pSrc = static_cast<const FX_BASE_INFO*>(&src);
	
	*m_pFiles = *(pSrc->m_pFiles);
	m_strHookName	= pSrc->m_strHookName;
	m_bUseECMHook	= pSrc->m_bUseECMHook;
	m_vOffset		= pSrc->m_vOffset;
	m_fYaw			= pSrc->m_fYaw;
	m_fPitch		= pSrc->m_fPitch;
	m_fRot			= pSrc->m_fRot;
	m_bBindParent	= pSrc->m_bBindParent;
	m_dwFadeOutTime = pSrc->m_dwFadeOutTime;
	m_bModelAlpha	= pSrc->m_bModelAlpha;
	m_bCustomFilePath = pSrc->m_bCustomFilePath;
	m_matTran		= pSrc->m_matTran;
	m_nCustomData	= pSrc->m_nCustomData;
	
	return *this;
}

FX_BASE_INFO* FX_BASE_INFO::CreateFxInfo(
	A3DCombinedAction* pAct,
	int nType)
{
	switch (nType)
	{
	case EVENT_TYPE_GFX:
		return new GFX_INFO(pAct);
	case EVENT_TYPE_SFX:
		return new SFX_INFO(pAct);
	case EVENT_TYPE_AUDIOEVENT:
		return new AUDIOEVENT_INFO(pAct);
	}
	return NULL;
}

FX_BASE_INFO* FX_BASE_INFO::LoadFromFile(
	A3DCombinedAction* pAct,
	AFile* pFile,
	DWORD dwVersion)
{
	DWORD dwReadLen;
	int nType = EVENT_TYPE_NONE;

	if (pFile->IsBinary())
		pFile->Read(&nType, sizeof(nType), &dwReadLen);
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_fx_type, &nType);
	}

	return CreateFxInfo(pAct, nType);
}

bool FX_BASE_INFO::LoadBase(AFile* pFile, DWORD dwVersion)
{
	DWORD dwReadLen;

	if (pFile->IsBinary())
	{
		if (dwVersion < 88)
		{
			AString strTmp;
			pFile->ReadString(strTmp);
			SetFilePath(strTmp);
		}
		else // dwVersion >= 88
		{
			int iNum = 0;
			pFile->Read(&iNum, sizeof(int), &dwReadLen);
			
			AString strTmp;
			for (int iIdx = 0; iIdx < iNum; ++iIdx)
			{
				pFile->ReadString(strTmp);
				m_pFiles->UniqueAdd(strTmp);
			}
		}

		pFile->ReadString(m_strHookName);
		pFile->Read(m_vOffset.m, sizeof(m_vOffset.m), &dwReadLen);
		pFile->Read(&m_fYaw, sizeof(m_fYaw), &dwReadLen);
		pFile->Read(&m_fPitch, sizeof(m_fPitch), &dwReadLen);
		pFile->Read(&m_fRot, sizeof(m_fRot), &dwReadLen);
		pFile->Read(&m_bBindParent, sizeof(m_bBindParent), &dwReadLen);
		//pFile->Read(&m_bFadeOut, sizeof(m_bFadeOut), &dwReadLen);
		pFile->Read(&m_bModelAlpha, sizeof(m_bModelAlpha), &dwReadLen);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		char	szBuf[AFILE_LINEMAXLEN];
		int		nBool;

		if (dwVersion < 18)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_fx_start_time, &m_dwStartTime);
		}

		if (dwVersion < 53)
		{	
			szBuf[0] = '\0';
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_fx_path, szBuf);
			SetFilePath(szBuf);
		}
		else // dwVersion >= 53
		{
			int iNum = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_fx_path_num, &iNum);
			
			for (int iIdx = 0; iIdx < iNum; ++iIdx)
			{
				szBuf[0] = '\0';
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_fx_path, szBuf);
				m_pFiles->UniqueAdd(szBuf);
			}
		}

		szBuf[0] = '\0';
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_hook_name, szBuf);
		m_strHookName = szBuf;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_hook_offset, VECTORADDR_XYZ(m_vOffset));

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_hook_yaw, &m_fYaw);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_hook_pitch, &m_fPitch);

		if (dwVersion >= 19)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_hook_rot, &m_fRot);
		}

		if (dwVersion > 1)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_bind_parent, &nBool);
			m_bBindParent = (nBool != 0);
		}

		if (dwVersion >= 15 && dwVersion < 50)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_fadeout, &nBool);
			bool bFadeOut = (nBool != 0);
			if (bFadeOut)
				m_dwFadeOutTime = DEFAULT_GFX_FADE_OUT_TIME;
			else
				m_dwFadeOutTime = 0;
		}
		else if (dwVersion >= 50)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_fadeout, &m_dwFadeOutTime);
		}

		//	如果淡出时间为负数，直接置为0
		if (m_dwFadeOutTime >= 0x80000000)
			m_dwFadeOutTime = 0;

		if (dwVersion >= 17)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_model_alpha, &nBool);
			m_bModelAlpha = (nBool != 0);
		}

		if (dwVersion >= 58)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_custom_path, &nBool);
			m_bCustomFilePath = (nBool != 0);
		}

		if( dwVersion >= 62 )
		{
			pFile->ReadLine(szLine,AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine,_format_custom_data,&m_nCustomData);
		}
	}

	BuildTranMat();
	return true;
}

bool FX_BASE_INFO::SaveBase(AFile* pFile)
{
	SaveEventBase(pFile);

	if (pFile->IsBinary())
	{
		DWORD dwWrite;

		int iIdx, iNum = m_pFiles->GetSize();
		pFile->Write(&iNum, sizeof(iNum), &dwWrite);
		for (iIdx = 0; iIdx < iNum; ++iIdx)
		{
			pFile->WriteString(m_pFiles->GetString(iIdx));
		}
		pFile->WriteString(m_strHookName);
		pFile->Write(m_vOffset.m, sizeof(m_vOffset.m), &dwWrite);
		pFile->Write(&m_fYaw, sizeof(m_fYaw), &dwWrite);
		pFile->Write(&m_fPitch, sizeof(m_fPitch), &dwWrite);
		pFile->Write(&m_fRot, sizeof(m_fRot), &dwWrite);
		pFile->Write(&m_bBindParent,sizeof(m_bBindParent), &dwWrite);
		//pFile->Write(&m_bFadeOut, sizeof(m_bFadeOut), &dwWrite);
		pFile->Write(&m_bModelAlpha, sizeof(m_bModelAlpha), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_fx_path_num, m_pFiles->GetSize());
		pFile->WriteLine(szLine);

		for (int iIdx = 0; iIdx < m_pFiles->GetSize(); ++iIdx)
		{
			_snprintf(szLine, AFILE_LINEMAXLEN, _format_fx_path, m_pFiles->GetString(iIdx));
			pFile->WriteLine(szLine);
		}

		sprintf(szLine, _format_hook_name, m_strHookName);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_hook_offset, VECTOR_XYZ(m_vOffset));
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_hook_yaw, m_fYaw);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_hook_pitch, m_fPitch);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_hook_rot, m_fRot);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_bind_parent, (int)m_bBindParent);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_fadeout, m_dwFadeOutTime);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_model_alpha, (int)m_bModelAlpha);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_custom_path, (int)m_bCustomFilePath);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_custom_data, (int)m_nCustomData);
		pFile->WriteLine(szLine);
		
	}

	return true;
}

bool ModActParamData::Load(AFile* pFile, DWORD dwVersion)
{
	DWORD	dwRead;
	
	if (pFile->IsBinary())
	{
		pFile->Read(&m_Type, sizeof(m_Type), &dwRead);
		pFile->Read(&m_bOutCmd, sizeof(m_bOutCmd), &dwRead);
		pFile->ReadString(m_strHook);
	}
	else
	{
		char szLine[AFILE_LINEMAXLEN];
		char szBuf[AFILE_LINEMAXLEN];
		int nVal = 0;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_param_type, &m_Type);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_param_is_cmd, &nVal);
		m_bOutCmd = (nVal != 0);
		
		if (m_bOutCmd)
		{
			szBuf[0] = '\0';
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
			sscanf(szLine, _format_param_cmd, szBuf);
			m_strHook = szBuf;
		}
		else
		{
			switch (m_Type)
			{
			case MOD_PARAM_POS:
			case MOD_PARAM_HOOK:
				szBuf[0] = '\0';
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
				sscanf(szLine, _format_param_hook, szBuf);
				m_strHook = szBuf;
				break;
			}
		}
	}

	return true;
}

bool ModActParamData::Save(AFile* pFile)
{
	DWORD dwWrite;
	
	if (pFile->IsBinary())
	{
		pFile->Write(&m_Type, sizeof(m_Type), &dwWrite);
		pFile->Write(&m_bOutCmd, sizeof(m_bOutCmd), &dwWrite);
		pFile->WriteString(m_strHook);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_param_type, m_Type);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_param_is_cmd, (int)m_bOutCmd);
		pFile->WriteLine(szLine);

		if (m_bOutCmd)
		{
			sprintf(szLine, _format_param_cmd, m_strHook);
			pFile->WriteLine(szLine);
		}
		else
		{
			switch (m_Type)
			{
			case MOD_PARAM_POS:
			case MOD_PARAM_HOOK:
				sprintf(szLine, _format_param_hook, m_strHook);
				pFile->WriteLine(szLine);
				break;
			}
		}
	}
	return true;
}

bool ModActParam::Load(AFile* pFile, DWORD dwVersion)
{
	DWORD dwReadLen;

	if (pFile->IsBinary())
	{
		pFile->ReadString(m_strEleName);
		pFile->Read(&m_nParamId, sizeof(m_nParamId), &dwReadLen);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		char	szBuf[AFILE_LINEMAXLEN];
		szBuf[0] = '\0';

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_param_ele_name, szBuf);
		m_strEleName = szBuf;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_param_id	, &m_nParamId);
	}

	return m_ParamData.Load(pFile, dwVersion);
}

bool ModActParam::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;

		pFile->WriteString(m_strEleName);
		pFile->Write(&m_nParamId, sizeof(m_nParamId), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_param_ele_name, m_strEleName);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_param_id, m_nParamId);
		pFile->WriteLine(szLine);
	}

	return m_ParamData.Save(pFile);
}

class GFX_BINDING : public FX_BINDING_BASE
{
public:
	A3DGFXEx*	m_pGfx;
	bool		m_bNeedSetTM;
	bool		m_bInitTMUpdated;	// is the first time a delay update gfx to be set to place
	DWORD		m_dwChildTrailTimeSpan; 
    DWORD		m_dwGfxTimeSpan;
	CECModel*	m_pHostModel;
	int			m_iDeltaTime;

	GFX_BINDING(A3DCombActDynData* pDynData);
	~GFX_BINDING();

	virtual void Stop() const { m_pGfx->Stop(); }
	virtual bool IsStop() const { return m_pGfx->GetState() == ST_STOP; }
	virtual void Render(A3DViewport* pView) { AfxGetGFXExMan()->RegisterGfx(m_pGfx); }
	virtual void UpdateParam(CECModel* pECModel, int nDeltaTime);
private:
	void ImplUpdateParam(CECModel* pECModel, int nDeltaTime);
	A3DMATRIX4 CalcGfxParentTM(A3DSkeletonHook* pHook, CECModel* pECModel);
	void UpdateGfxParentTM(A3DSkeletonHook* pHook, CECModel* pECModel, int iDeltaTime);
	A3DSkeletonHook* GetHook() const
	{
		PGFX_INFO pInfo = (PGFX_INFO)m_pInfo;
		if (pInfo->GetHookName().IsEmpty() || !m_pHostModel)
			return NULL;
		else
			return m_pHostModel->GetA3DSkinModel()->GetSkeletonHook(pInfo->GetHookName(), true);
	}
};


int GFX_INFO::s_ActBindGfxCount = 0;

GFX_INFO::GFX_INFO(A3DCombinedAction* pAct) :
FX_BASE_INFO(pAct),
m_pDev(0),
m_pGfx(0),
m_fScale(1.0f),
m_fAlpha(1.0f),
m_fPlaySpeed(1.0f),
m_bUseOuterPath(false),
m_bRelativeToECModel(false),
m_iDelayTime(0),
m_bRotWithModel(true),
m_bUseFixedPoint(false)
{
	++s_ActBindGfxCount;

	m_nType = EVENT_TYPE_GFX;
}

GFX_INFO::~GFX_INFO() 
{
	for (size_t i = 0; i < m_ParamList.size(); i++)
		delete m_ParamList[i];
	
	if (m_pGfx) 
	{ 
		if (CanFadeOut())
			AfxGetGFXExMan()->QueueFadeOutGfx(m_pGfx, GetFadeOutTime());
		else
			AfxGetGFXExMan()->CacheReleasedGfx(m_pGfx);
	}

	--s_ActBindGfxCount;
}


bool GFX_INFO::Load(AFile* pFile, DWORD dwVersion)
{
	if (!FX_BASE_INFO::LoadBase(pFile, dwVersion))
		return false;

	DWORD dwReadLen;
	int nParamCount = 0;

	if (pFile->IsBinary())
	{
		pFile->Read(&m_fScale, sizeof(m_fScale), &dwReadLen);
		pFile->Read(&m_fAlpha, sizeof(m_fAlpha), &dwReadLen);
		pFile->Read(&m_fPlaySpeed, sizeof(m_fPlaySpeed), &dwReadLen);
		pFile->Read(&m_bUseOuterPath, sizeof(m_bUseOuterPath), &dwReadLen);
		pFile->Read(&nParamCount, sizeof(nParamCount), &dwReadLen);
	}
	else
	{
		char szLine[AFILE_LINEMAXLEN];
		int nRead;
		bool bOldVersion = false;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_gfx_scale, &m_fScale);

		if (dwVersion >= 24)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_gfx_alpha, &m_fAlpha);
		}

		if (dwVersion == 22)
		{
			float val = -1.0f;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_gfx_alpha, &val);

			if (val != -1.0f)
			{
				m_fAlpha = val;
				bOldVersion = true;
			}
		}

		if (dwVersion >= 10)
		{
			if (dwVersion != 22 || bOldVersion)
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);

			sscanf(szLine, _format_gfx_play_speed, &m_fPlaySpeed);
		}

		if (dwVersion >= 22 && !bOldVersion)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_gfx_outer_path, &nRead);
			m_bUseOuterPath = (nRead != 0);
		}

		if (dwVersion >= 13 && dwVersion < 15)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_fadeout, &nRead);
			bool bFadeOut = (nRead != 0);

			if (bFadeOut)
				m_dwFadeOutTime = DEFAULT_GFX_FADE_OUT_TIME;
			else
				m_dwFadeOutTime = 0;
		}

		if (dwVersion >= 35)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_gfx_rel_ecm, &nRead);
			m_bRelativeToECModel = (nRead != 0);
		}

		if (dwVersion >= 54)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_gfx_delay_time, &m_iDelayTime);
		}

		if (dwVersion >= 66)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_gfx_rot_with_model, &nRead);
			m_bRotWithModel = nRead != 0;
		}

		if (dwVersion >= 69)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_gfx_use_fixedpoint, &nRead);
			m_bUseFixedPoint = nRead != 0;
		}

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_gfx_param_count, &nParamCount);
	}

	for (int i = 0; i < nParamCount; i++)
	{
		ModActParam* pParam = new ModActParam;
		pParam->Load(pFile, dwVersion);
		m_ParamList.push_back(pParam);
	}

	return true;
}

bool GFX_INFO::Save(AFile* pFile)
{
	if (!FX_BASE_INFO::SaveBase(pFile))
		return false;

	if (pFile->IsBinary())
	{
		DWORD dwWrite;

		pFile->Write(&m_fScale, sizeof(m_fScale), &dwWrite);
		pFile->Write(&m_fAlpha, sizeof(m_fAlpha), &dwWrite);
		pFile->Write(&m_fPlaySpeed, sizeof(m_fPlaySpeed), &dwWrite);
		pFile->Write(&m_bUseOuterPath, sizeof(m_bUseOuterPath), &dwWrite);
		int nParamCount = (int)m_ParamList.size();
		pFile->Write(&nParamCount, sizeof(nParamCount), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_gfx_scale, m_fScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_gfx_alpha, m_fAlpha);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_gfx_play_speed, m_fPlaySpeed);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_gfx_outer_path, m_bUseOuterPath);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_gfx_rel_ecm, m_bRelativeToECModel);
		pFile->WriteLine(szLine);
		
		_snprintf(szLine, AFILE_LINEMAXLEN, _format_gfx_delay_time, m_iDelayTime);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_gfx_rot_with_model, m_bRotWithModel);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_gfx_use_fixedpoint, m_bUseFixedPoint);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_gfx_param_count, m_ParamList.size());
		pFile->WriteLine(szLine);

	}

	for (size_t i = 0; i < m_ParamList.size(); i++)
		m_ParamList[i]->Save(pFile);

	return true;
}

void GFX_INFO::CheckBindParam(GFX_BINDING* pBind)
{
	size_t i = 0;
	while (i < m_ParamList.size())
	{
		ModActParam* pParam = m_ParamList[i];
		if (!pBind->m_pGfx->IsParamEnable(pParam->m_strEleName, pParam->m_nParamId))
		{
			m_ParamList.erase(m_ParamList.begin()+i);
			delete pParam;
			continue;
		}
		i++;
	}
}

bool GFX_INFO::Start(A3DCombActDynData* pDynData)
{
	const char* szPath = GetFilePath();

	if (m_bCustomFilePath && szPath)
	{
		ECM_CUSTOM_FX_FILE_PATH pFunc = AfxGetGFXExMan()->GetCustomFxFilePathFunc();

		if (pFunc)
		{
			ECM_CUSTOM_FX_PARAM_IN pin;
			ECM_CUSTOM_FX_PARAM_OUT pout;
			pin.szDefaultPath = szPath;

			if (pFunc(&pin, &pout))
				szPath = pout.szCustomPath;
		}
	}

	A3DGFXEx* pGfx = AfxGetGFXExMan()->LoadGfx(m_pDev, szPath);
	if (!pGfx)
		return false;

	PGFX_BINDING pBind =  new GFX_BINDING(pDynData);
	pBind->SetInfo(this);
	pBind->m_pGfx = pGfx;
	CheckBindParam(pBind);
	
	CECModel* pECModel = pDynData->GetHostModel();
	ASSERT(pECModel && "This pointer must be valid here, so assert is enough");
	if (IsUseFixedPoint())
	{
		pGfx->SetPos(pDynData->GetFixedPoint());
	}

	pGfx->SetScale(GetScale() * pECModel->GetGfxScale());
	pGfx->SetAlpha(GetAlpha());
	pGfx->SetPlaySpeed(GetPlaySpeed());
	pGfx->SetSfxPriority(pECModel->GetSfxPriority());
	pECModel->TransferEcmProperties(pGfx);
	if (!pECModel->GetDisableCamShake())
	{
		pGfx->SetDisableCamShake(pDynData->GetNoCamShakeFlag());
	}
	pGfx->Start(true);

	++A3DCombActDynData::s_CombActFxArrayCount;
	pDynData->m_ActFxArray.push_back(pBind);
	return true;
}

void GFX_INFO::LoadGfx() 
{ 
	m_pGfx = AfxGetGFXExMan()->LoadGfx(m_pDev, GetFilePath());
}

EVENT_INFO& GFX_INFO::Clone(const EVENT_INFO& src)
{
	FX_BASE_INFO::Clone(src);
	const GFX_INFO* pSrc = static_cast<const GFX_INFO*>(&src);
	
	m_pDev			= pSrc->m_pDev;
	m_fScale		= pSrc->m_fScale;
	m_fAlpha		= pSrc->m_fAlpha;
	m_fPlaySpeed	= pSrc->m_fPlaySpeed;
	m_bUseOuterPath	= pSrc->m_bUseOuterPath;
	m_bRelativeToECModel = pSrc->m_bRelativeToECModel;
	m_iDelayTime	= pSrc->m_iDelayTime;
	m_bRotWithModel = pSrc->m_bRotWithModel;
	m_bUseFixedPoint = pSrc->m_bUseFixedPoint;
	
	for (size_t i = 0; i < pSrc->m_ParamList.size(); i++)
		m_ParamList.push_back(new ModActParam(*pSrc->m_ParamList[i]));
	
	return *this;
}

GFX_BINDING::GFX_BINDING(A3DCombActDynData* pDynData) :
FX_BINDING_BASE(pDynData),
m_pGfx(0),
m_bNeedSetTM(true),
m_bInitTMUpdated(false),
m_dwChildTrailTimeSpan(0), m_dwGfxTimeSpan(0),
m_pHostModel(pDynData->GetHostModel()),
m_iDeltaTime(0)
{
}

GFX_BINDING::~GFX_BINDING()
{	
	if (m_pGfx)
	{
		PGFX_INFO pGfxInfo = static_cast<PGFX_INFO>(m_pInfo);
		if (pGfxInfo->CanFadeOut())
			AfxGetGFXExMan()->QueueFadeOutGfx(m_pGfx, pGfxInfo->GetFadeOutTime());
		else
			AfxGetGFXExMan()->CacheReleasedGfx(m_pGfx);
	}
}

//	This function is a fake implementation instead of the CECModel::GetGFXParam
//	which is never a real function with any functionality
//	Implement this to make sure the old fake parameters still take affect
static GFX_PROPERTY _FakeECMGetGFXParam(CECModel* pModel, const AString& strParam)
{
	if (!pModel->GetA3DSkinModel())
		return GFX_PROPERTY();

	// test only
	A3DSkeletonHook* pHook = pModel->GetA3DSkinModel()->GetSkeleton()->GetHook(0);

	if (strParam == "hook1_mat")
		return GFX_PROPERTY(pHook->GetAbsoluteTM());
	else //if (strParam == "hook1_pos")
		return GFX_PROPERTY(pHook->GetAbsoluteTM().GetRow(3));
}

A3DMATRIX4 GFX_BINDING::CalcGfxParentTM(A3DSkeletonHook* pHook, CECModel* pECModel)
{
	PGFX_INFO pInfo = (PGFX_INFO)m_pInfo;
	A3DMATRIX4 matBaseTM;
	if (!pHook)
	{
		if (pInfo->RelativeToECModel())
			matBaseTM = pECModel->GetAbsoluteTM();
		else
			matBaseTM = pECModel->GetAbsTM();
	}
	else
		matBaseTM = pHook->GetAbsoluteTM();

	return pInfo->GetMatTran() * matBaseTM;
}

void GFX_BINDING::UpdateGfxParentTM(A3DSkeletonHook* pHook, CECModel* pECModel, int iDeltaTime)
{
	PGFX_INFO pInfo = (PGFX_INFO)m_pInfo;
	if (!m_bNeedSetTM)
		return;

	int iDelayTime = pInfo->GetDelayTime();
	A3DMATRIX4 matDst = CalcGfxParentTM(pHook, pECModel);
	if (iDelayTime && m_bInitTMUpdated)
	{
		_DelayUpdateGfxParentTM(m_pGfx, matDst, (float)iDeltaTime / (float)iDelayTime);
	}
	else
		m_pGfx->SetParentTM(matDst);
	
	if (iDelayTime)
	{
		m_bInitTMUpdated = true;
	}

	if (!pInfo->IsBindParent())
		m_bNeedSetTM = false;
}

void GFX_BINDING::ImplUpdateParam(CECModel* pECModel, int nDeltaTime)
{
	if (!m_pGfx->IsResourceReady())
		return;

	A3DSkinModel* pModel = pECModel->GetA3DSkinModel();
	PGFX_INFO pInfo = (PGFX_INFO)m_pInfo;
	A3DSkeletonHook* pHook = GetHook();

	if(!pInfo->IsUseFixedPoint())
		UpdateGfxParentTM(pHook, pECModel, nDeltaTime);

	if (m_pGfx->GetState() == ST_STOP)
		return;

	if (pInfo->UseModelAlpha() && pModel)
	{
		float fAlpha, fTans = pModel->GetTransparent();

		if (fTans < 0)
			fAlpha = 1.0f;
		else
			fAlpha = 1.0f - fTans;

		m_pGfx->SetAlpha(fAlpha * pInfo->GetAlpha());
	}

    if (m_pInfo->GetTimeSpan() < 0xFFFFFFFF)
    {
        int nLife = m_pInfo->GetTimeSpan() > m_dwGfxTimeSpan ? m_pInfo->GetTimeSpan() - m_dwGfxTimeSpan : 0;
        //AString strDbg;
        //strDbg.Format("life = %d, span = %d\n", nLife, m_dwGfxTimeSpan);
        //OutputDebugStringA(strDbg);
        for (int iEle = 0; iEle < m_pGfx->GetElementCount(); iEle++)
        {
            A3DGFXElement* pElement = m_pGfx->GetElement(iEle);
            if (pElement->GetEleTypeId() ==  ID_ELE_TYPE_TRAIL_EX)
            {
                pElement->UpdateParam(ID_PARAM_TRAIL_LIFE, nLife);

            }
        }
    }
    m_dwGfxTimeSpan += nDeltaTime;

	if (pECModel->IsGfxScaleChanged())
		m_pGfx->SetScale(pInfo->GetScale() * pECModel->GetGfxScale());

	ModActParamList& lst = static_cast<PGFX_INFO>(m_pInfo)->GetParamList();

	for (size_t nParam = 0; nParam < lst.size(); nParam++)
	{
		ModActParam* pParam = lst[nParam];
		ModActParamData& md = pParam->m_ParamData;

		if (md.m_bOutCmd)
		{
			// Actually, this case is unnecessary, because the md.m_bOutCmd == true case causes fake parameters in the modeditorex.exe
			GFX_PROPERTY gp = _FakeECMGetGFXParam(pECModel, md.m_strHook); //pECModel->GetGFXParam(md.m_strHook);

			if (gp.GetType() == GFX_VALUE_UNKNOWN)
				continue;

			m_pGfx->UpdateEleParam(pParam->m_strEleName, pParam->m_nParamId, gp);
		}
		else
		{
			A3DGFXElement* pEle = m_pGfx->GetElementByName(pParam->m_strEleName);

			if (!pEle)
				break;

			A3DSkeletonHook* pParamHook;

			if (md.m_strHook.IsEmpty() || !pModel)
				pParamHook = NULL;
			else
				pParamHook = pModel->GetSkeletonHook(md.m_strHook, true);

			switch (pParam->m_nParamId)
			{
			case ID_PARAM_LTN_POS1:
			case ID_PARAM_LTN_POS2:

				if (pEle->NeedUpdateParam(pParam->m_nParamId))
				{
					if (pParamHook)
						pEle->UpdateParam(pParam->m_nParamId, pParamHook->GetAbsoluteTM().GetRow(3));
					else
						pEle->UpdateParam(pParam->m_nParamId, m_pGfx->GetPos());
				}

				break;

			case ID_PARAM_TRAIL_POS1:
			case ID_PARAM_TRAIL_POS2: {

				if (pEle->NeedUpdateParam(pParam->m_nParamId) && pParamHook)
				{
					ChildActInfo* pParent = const_cast<ChildActInfo*>(m_pDynData->GetParentActInfo());

					if (pParent && pParent->IsTrailAct())
						pEle->UpdateParam(pParam->m_nParamId, pParamHook->GetHookTM().GetRow(3));
					else
						pEle->UpdateParam(pParam->m_nParamId, pParamHook->GetAbsoluteTM().GetRow(3));
				}

				break; }

			case ID_PARAM_LTNTRAIL_POS1:
			case ID_PARAM_LTNTRAIL_POS2:

				if (pEle->NeedUpdateParam(pParam->m_nParamId) && pParamHook)
					pEle->UpdateParam(pParam->m_nParamId, pParamHook->GetHookTM().GetRow(3));

				break;

			case ID_PARAM_TRAIL_MATRIX: {

				if (!pParamHook)
					break;

				ChildActInfo* pParent = const_cast<ChildActInfo*>(m_pDynData->GetParentActInfo());

				if (!pParent)
					break;

				if (pParent->IsTrailAct())
				{
					if (m_dwChildTrailTimeSpan > pParent->GetTrailTmSpan())
					{
						m_pGfx->Stop();
						break;
					}

					if (nDeltaTime == 0)
					{
						m_pGfx->TickAnimation(0);
						return;
					}

					DWORD dwStartTm = m_dwChildTrailTimeSpan;

					if (m_dwChildTrailTimeSpan == 0)
						m_dwChildTrailTimeSpan = m_pDynData->GetParentDeltaTime();
					else
						m_dwChildTrailTimeSpan += (DWORD)nDeltaTime;

					DWORD dwEndTm = m_dwChildTrailTimeSpan;

					if (!m_pGfx->TickAnimation(dwEndTm - dwStartTm))
						return;

					A3DMATRIX4 matTran;
					if (pEle->NeedUpdateParam(ID_PARAM_TRAIL_ORGMAT))
					{
						pEle->UpdateParam(ID_PARAM_TRAIL_ORGMAT, pParamHook->GetHookTM());
						pEle->UpdateParam(ID_PARAM_TRAIL_TIMESPAN, pParent->GetTrailTmSpan());
					}

					dwStartTm += _trail_delta - 1;
					dwStartTm -= dwStartTm % _trail_delta;

					while (dwStartTm < dwEndTm)
					{
						if (pParent->GetTraiMatTran(dwStartTm, matTran))
						{
							pEle->UpdateParam(ID_PARAM_TRAIL_DELTATM, GFX_PROPERTY(dwEndTm - dwStartTm));
							pEle->UpdateParam(ID_PARAM_TRAIL_MATRIX, matTran);
						}

						dwStartTm += _trail_delta;
					}

					if (pParent->GetTraiMatTran(dwEndTm, matTran))
					{
						pEle->UpdateParam(ID_PARAM_TRAIL_DELTATM, GFX_PROPERTY(0));
						pEle->UpdateParam(ID_PARAM_TRAIL_MATRIX, matTran);
					}

					pEle->UpdateParam(ID_PARAM_TRAIL_PARENTTM, m_pDynData->GetParentModel()->GetAbsTM());
					return;
				}
				else
				{
					if (pParent->GetTrailTmSpan())
						pEle->UpdateParam(ID_PARAM_TRAIL_TIMESPAN, pParent->GetTrailTmSpan());

					pEle->UpdateParam(ID_PARAM_TRAIL_ADD_DATA, 0);
				}

				break; }

			case ID_PARAM_LTNTRAIL_MATRIX:

				if (!pParamHook || !pModel)
					break;

				pEle->UpdateParam(ID_PARAM_LTNTRAIL_MATRIX, pModel->GetSkeleton()->GetBone(pParamHook->GetBone())->GetAbsoluteTM());
				break;
			}
		}
	}

	m_pGfx->TickAnimation(nDeltaTime);
}


void GFX_BINDING::UpdateParam(CECModel* pECModel, int nDeltaTime)
{
	ImplUpdateParam(pECModel, nDeltaTime);
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement SFX_INFO
//
//////////////////////////////////////////////////////////////////////////

SFX_INFO::SFX_INFO(A3DCombinedAction* pAct) 
: FX_BASE_INFO(pAct)
, m_pAMEngine(0)
, m_iLastSoundCount(0)
{
	m_nType = EVENT_TYPE_SFX;
}

void SFX_INFO::Init(A3DDevice* pDev)
{
	m_pAMEngine = pDev->GetA3DEngine()->GetAMSoundEngine();
	m_pDev = pDev;
}

bool SFX_INFO::Load(AFile* pFile, DWORD dwVersion)
{
	if (!LoadBase(pFile, dwVersion) || !m_SoundParamInfo.LoadSoundParamInfo(pFile))
		return false;

	return true;
}

bool SFX_INFO::Save(AFile* pFile)
{
	if (!SaveBase(pFile) || !m_SoundParamInfo.SaveSoundParamInfo(pFile))
		return false;

	return true;
}

EVENT_INFO& SFX_INFO::Clone(const EVENT_INFO& src)
{
	FX_BASE_INFO::Clone(src);
	const SFX_INFO* pSrc = static_cast<const SFX_INFO*>(&src);
	
	m_SoundParamInfo= pSrc->m_SoundParamInfo;
	m_pAMEngine		= pSrc->m_pAMEngine;
	m_pDev			= pSrc->m_pDev;
	
	return *this;
}

void SFX_INFO::CheckRandamSoundCount(AString& strFile)
{
	if (m_pFiles->GetSize() <= 1)
		return;

	if (m_strLastSoundFile == strFile)
	{
		if (++m_iLastSoundCount >= 3)
		{
			for (int iIdx = 0; iIdx < m_pFiles->GetSize(); ++iIdx)
			{
				if (AString(m_pFiles->GetString(iIdx)) == strFile)
					continue;

				strFile = m_pFiles->GetString(iIdx);
				UpdateLastSound(strFile);
				return;
			}
		}
	}
	else
	{
		UpdateLastSound(strFile);
	}
}

void SFX_INFO::UpdateLastSound(const AString& strFile)
{
	m_strLastSoundFile = strFile;
	m_iLastSoundCount = 0;
}

bool SFX_INFO::Start(A3DCombActDynData* pDynData)
{
	bool bEnable = pDynData->GetHostModel()->GetStaticData()->IsAudioGroupEnable(m_SoundParamInfo.GetGroup());
	if (!bEnable)
		return false;

	//按一定几率播放
	float fPercent = m_SoundParamInfo.GetPercentStart();
	if ( fPercent < 1.0f)
	{
		int rad = a_Random(0, 100);
		if (rad * 0.01f > fPercent)
			return true;
	}

	DWORD startTickCount = GetTickCount();

	if (m_pAct)
	{
		A3DVECTOR3 vPos = m_pDev->GetA3DEngine()->GetAMSoundEngine()->GetAM3DSoundDevice()->GetPosition();
		A3DSkinModel* pModel = pDynData->GetHostModel()->GetA3DSkinModel();

		if (pModel && (vPos - pModel->GetPos()).Normalize() > m_SoundParamInfo.GetMaxDist())
			return false;
	}

	AString strFilePath = GetRandSfxFilePath();
	CheckRandamSoundCount(strFilePath);
	if (strFilePath.IsEmpty())
		return false;

	AfxGetGFXExMan()->PushSfxFile(strFilePath, 200);

	AM3DSoundBuffer* pSfx;
	if (m_SoundParamInfo.GetLoop())
		pSfx = AfxLoadLoopSound("Sfx\\" + strFilePath);
	else
		pSfx = AfxLoadNonLoopSound("Sfx\\" + strFilePath, pDynData->GetHostModel()->GetSfxPriority());

	if (!pSfx)
	{
#ifndef _ANGELICA21
		a_LogOutput(1, "%s, Error Load sfx : %s", __FUNCTION__, "Sfx\\" + strFilePath);
#endif
		return false;
	}

	if (AfxGetGFXExMan()->IsSfxVolumeEnable())
	{
		if (m_SoundParamInfo.GetIsAbsoluteVolume())
		{
			//TRICK SetVolume效果会被覆盖，所以设置较大的相对音量以达到等同绝对音量的效果
			double globalVolume = AfxGetA3DDevice()->GetA3DEngine()->GetAMSoundEngine()->GetVolume() / 100.0;	// 取得引擎音量
			pSfx->SetRelativeVolume(int(m_SoundParamInfo.GetRandVolume()/globalVolume));
		}
		else
		{
			pSfx->SetRelativeVolume(m_SoundParamInfo.GetRandVolume());
		}
	}

#ifdef _ANGELICA21		//因为新加 SetSkipTime, GetTotalTime
	if (m_SoundParamInfo.GetFixSpeed())
		pSfx->SetPlaySpeed((float)pow(2, m_SoundParamInfo.GetRandPitch() / 12.f));
	else
		pSfx->SetPlaySpeed((float)pow(2, m_SoundParamInfo.GetRandPitch() / 12.f) * pDynData->GetHostModel()->GetPlaySpeed());

	// 需要跳过的时间长度 (已加算文件读取时间)
	// 注意：时间长度与播放速度有关
	DWORD dwSkipTime = GetTickCount()-startTickCount + pDynData->GetCurTimeSpan() - GetStartTime();
	// 开头静音部分长度
	int silentHeaderTime = int(m_SoundParamInfo.GetSilentHeader() / pSfx->GetPlaySpeed());

	if (dwSkipTime <= silentHeaderTime)		// 安全跳过
	{
		pSfx->SetSkipTime(dwSkipTime);
	}
	else if (dwSkipTime-silentHeaderTime > 200)		// 超出200ms，用渐变
	{
		if (pSfx->GetTotalTime() < 500)		// 根据某需求，此情况下总长小于500ms就不播放，为什么？
		{
			pSfx->SetSkipTime(pSfx->GetTotalTime());
		}
		else
		{
			pSfx->SetSkipTime(dwSkipTime);
			pSfx->FadeInRelatively(200);
		}
	}
	else		// 超出一些，仅跳过开头的静音
	{
		pSfx->SetSkipTime(silentHeaderTime);
	}
#else
	pSfx->SetPlaySpeed((float)pow(2, m_SoundParamInfo.GetRandPitch() / 12.f));
#endif

	pSfx->SetForce2D(m_SoundParamInfo.GetForce2D());
	pSfx->SetMinDistance(m_SoundParamInfo.GetMinDist());
	pSfx->SetMaxDistance(m_SoundParamInfo.GetMaxDist());

	pSfx->Play(m_SoundParamInfo.GetLoop());
	PSFX_BINDING pBind = new SFX_BINDING(pDynData);
	pBind->m_pSfx = pSfx;
	pBind->SetInfo(this);

	++A3DCombActDynData::s_CombActFxArrayCount;
	pDynData->m_ActFxArray.push_back(pBind);
	return true;
}

const char* SFX_INFO::GetRandSfxFilePath() const
{
#ifdef _ANGELICA21

	AList2<int> IndexList;
	
	int nIndexMinTTL = -1;
	int nMinTTL = 100000;
	A3DGFXExMan* pGfxMan = AfxGetGFXExMan();

	for (int i=0; i<m_pFiles->GetSize(); ++i)
	{
		int nttl;
		if (pGfxMan->IsInSfxFileList(m_pFiles->GetString(i), nttl))
		{
			if (nttl < nMinTTL)
			{
				nMinTTL = nttl;
				nIndexMinTTL = i;
			}
		}
		else
		{
			IndexList.AddTail(i);
		}
	}

	if (IndexList.GetCount() == 0)		//全部在最近的200ms内,选取ttl最小者
	{
		if (nIndexMinTTL == -1)
		{
			//assert(nIndexMinTTL != -1);
			return NULL;
		}
		return m_pFiles->GetString(nIndexMinTTL);
	}
	else	//在剩余的sfx文件中随机
	{
		int idx = a_Random(0, IndexList.GetCount() - 1);
		int nCurrent = 0;

		ALISTPOSITION pos = IndexList.GetHeadPosition();
		while (pos)
		{
			int nIdx = IndexList.GetNext(pos);
			if (nCurrent == idx)
			{
				return m_pFiles->GetString(nIdx);
			}
			++nCurrent;
		}
	}

	assert(false);

#endif
	return m_pFiles->GetRandString();
}

SFX_BINDING::~SFX_BINDING()
{
	if (m_pSfx)
	{
		PSFX_INFO pInfo = static_cast<PSFX_INFO>(m_pInfo);

		if (m_pDynData && pInfo->CanFadeOut() && !pInfo->GetSoundParamInfo().GetLoop())
			m_pDynData->GetHostModel()->QueueFadeOutSfx(m_pSfx);
		else
		{
			if (pInfo->GetSoundParamInfo().GetLoop())
				AfxReleaseSoundLoop(m_pSfx);
			else
				AfxReleaseSoundNonLoop(m_pSfx);
		}
	}
}

void SFX_BINDING::UpdateParam(CECModel* pECModel, int nDeltaTime)
{
	A3DSkinModel* pModel = pECModel->GetA3DSkinModel();
	PSFX_INFO pInfo = static_cast<PSFX_INFO>(m_pInfo);
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

	m_pSfx->SetPosition(matTran.GetRow(3));

#ifdef _ANGELICA21

	if (!m_pSfx->IsLoaded())
		return;

	m_pSfx->CheckInitState();

#endif

	m_pSfx->UpdateChanges();
	m_pSfx->CheckEnd();
}

bool ChildActInfo::Load(AFile* pFile, DWORD dwVersion)
{
	DWORD dwReadLen;

	if (pFile->IsBinary())
	{
		pFile->ReadString(m_strActName);
		pFile->ReadString(m_strHHName);
		pFile->Read(&m_bIsTrail, sizeof(m_bIsTrail), &dwReadLen);
		pFile->Read(&m_dwTrailTmSpan, sizeof(m_dwTrailTmSpan), &dwReadLen);
		m_dwSegCount = 0;
		pFile->Read(&m_dwSegCount, sizeof(m_dwSegCount), &dwReadLen);

		if (m_dwSegCount > 0)
		{
			m_aqDir = new A3DQUATERNION[m_dwSegCount];
			m_avPos = new A3DVECTOR3[m_dwSegCount];
		}

		for (DWORD i = 0; i < m_dwSegCount; i++)
		{
			pFile->Read(m_avPos[i].m, sizeof(m_avPos[i].m), &dwReadLen);
			pFile->Read(m_aqDir[i].m, sizeof(m_aqDir[i].m), &dwReadLen);
		}
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		char	szBuf[AFILE_LINEMAXLEN];

		if (dwVersion < 18)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_act_start_time, &m_dwStartTime);
		}

		szBuf[0] = '\0';
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_child_act_name, szBuf);
		m_strActName = szBuf;

		if (dwVersion >= 12)
		{
			szBuf[0] = '\0';
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_chld_hhname, szBuf);
			SetHHName(szBuf);

			if (dwVersion >= 42)
			{
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_transtime, &m_dwTransTime);
			}

			int nBool;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_chld_istrail, &nBool);
			SetIsTrailAct(nBool != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_chld_span, &m_dwTrailTmSpan);

			m_dwSegCount = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_chld_segs, &m_dwSegCount);

			if (m_dwSegCount > 0)
			{
				m_aqDir = new A3DQUATERNION[m_dwSegCount];
				m_avPos = new A3DVECTOR3[m_dwSegCount];
			}

			for (DWORD i = 0; i < m_dwSegCount; i++)
			{
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_pos, VECTORADDR_XYZ(m_avPos[i]));
				
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_dir, QUATADDR_XYZW(m_aqDir[i]));
			}
		}
	}

	return true;
}

bool ChildActInfo::Save(AFile* pFile)
{
	SaveEventBase(pFile);

	if (pFile->IsBinary())
	{
		DWORD dwWrite;

		pFile->WriteString(m_strActName);
		pFile->WriteString(m_strHHName);
		pFile->Write(&m_bIsTrail, sizeof(m_bIsTrail), &dwWrite);
		pFile->Write(&m_dwTrailTmSpan, sizeof(m_dwTrailTmSpan), &dwWrite);

		pFile->Write(&m_dwSegCount, sizeof(m_dwSegCount), &dwWrite);
		for (DWORD i = 0; i < m_dwSegCount; i++)
		{
			pFile->Write(m_avPos[i].m, sizeof(m_avPos[i].m), &dwWrite);
			pFile->Write(m_aqDir[i].m, sizeof(m_aqDir[i].m), &dwWrite);
		}
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_child_act_name, m_strActName);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_chld_hhname, m_strHHName);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_transtime, m_dwTransTime);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_chld_istrail, (int)m_bIsTrail);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_chld_span, m_dwTrailTmSpan);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_chld_segs, m_dwSegCount);
		pFile->WriteLine(szLine);

		for (DWORD i = 0; i < m_dwSegCount; i++)
		{
			sprintf(szLine, _format_pos, VECTOR_XYZ(m_avPos[i]));
			pFile->WriteLine(szLine);

			sprintf(szLine, _format_dir, QUAT_XYZW(m_aqDir[i]));
			pFile->WriteLine(szLine);
		}
	}

	return true;
}

static const DWORD _max_trail_len = 30000;

DWORD get_model_trail_total_len(CECModel& model)
{
	DWORD len = 0;

	for (int i = 0; i < model.GetComActCount(); i++)
	{
		A3DCombinedAction* pAct = model.GetComActByIndex(i);

		for (int j = 0; j < pAct->GetEventCount(); j++)
		{
			EVENT_INFO* pEvent = pAct->GetEventInfo(j);

			if (pEvent->GetType() != EVENT_TYPE_CHLDACT)
				continue;

			ChildActInfo* pInfo = static_cast<ChildActInfo*>(pEvent);
			if (!pInfo->IsTrailAct()) continue;
			len += pInfo->GetTrailTmSpan();
		}
	}

	return len;
}

bool ChildActInfo::CalcTrailData(CECModel* pECModel)
{
	if (!m_pAct)
	{
		assert(false);
		return false;
	}

	if (!m_bIsTrail)
	{
		m_dwSegCount = 0;
		if (m_avPos) delete[] m_avPos;
		m_avPos = NULL;
		if (m_aqDir) delete[] m_aqDir;
		m_aqDir = NULL;
		return true;
	}

	A3DSkinModel* pSkin = pECModel->GetA3DSkinModel();

	if (!pSkin)
		return false;

	A3DSkeletonHook* pHook = pSkin->GetSkeletonHook(m_strHHName, false);

	if (!pHook)
		return true;

	DWORD dwTotal = get_model_trail_total_len(*pECModel);

	if (m_dwTrailTmSpan + dwTotal > _max_trail_len)
	{
		m_dwSegCount = 0;
		m_dwTrailTmSpan = 0;
		m_bIsTrail = false;
		delete[] m_aqDir;
		delete[] m_avPos;
		m_aqDir = NULL;
		m_avPos = NULL;
		return false;
	}

	DWORD dwSeg = m_dwTrailTmSpan / _trail_delta;
	if (dwSeg == 0) return true;

	m_dwSegCount = (int)dwSeg + 1;
	if (m_aqDir) delete[] m_aqDir;
	if (m_avPos) delete[] m_avPos;
	m_aqDir = new A3DQUATERNION[dwSeg + 1];
	m_avPos = new A3DVECTOR3[dwSeg + 1];

	pSkin->StopAllActions();
	A3DCombActDynData* pDyn = new A3DCombActDynData(m_pAct, pECModel);
	pDyn->Play(0, 1.f, 200, -1);
	pSkin->Update(m_dwStartTime);

	const A3DMATRIX4& mat = pHook->GetAbsoluteTM();
	m_aqDir[0].ConvertFromMatrix(mat);
	m_avPos[0] = mat.GetRow(3);

	for (DWORD i = 1; i <= dwSeg; i++)
	{
		pSkin->Update(_trail_delta);

		const A3DMATRIX4& mat = pHook->GetAbsoluteTM();
		m_aqDir[i].ConvertFromMatrix(mat);
		m_avPos[i] = mat.GetRow(3);
	}

	pSkin->StopAllActions();
	delete pDyn;
	return true;
}

bool ChildActInfo::GetTraiMatTran(DWORD dwTimeSpan, A3DMATRIX4& matTran) const
{
	if (dwTimeSpan > m_dwTrailTmSpan)
		return false;

	DWORD dwRemain = dwTimeSpan % _trail_delta;
	dwTimeSpan /= _trail_delta;
	if (dwTimeSpan >= m_dwSegCount) return false;

	if (dwRemain == 0 || dwTimeSpan == m_dwSegCount - 1)
	{
		m_aqDir[dwTimeSpan].ConvertToMatrix(matTran);
		matTran.SetRow(3, m_avPos[dwTimeSpan]);
	}
	else
	{
		float fPortion = dwRemain / (float)_trail_delta;
		A3DQUATERNION q = SLERPQuad(m_aqDir[dwTimeSpan], m_aqDir[dwTimeSpan+1], fPortion);
		A3DVECTOR3 v = m_avPos[dwTimeSpan] * (1.0f - fPortion) + m_avPos[dwTimeSpan+1] * fPortion;
		q.ConvertToMatrix(matTran);
		matTran.SetRow(3, v);
	}

	return true;
}

bool ChildActInfo::Start(A3DCombActDynData* pDynData)
{
	pDynData->m_pECModel->ActivateChildAct(
		GetActName(),
		this,
		pDynData->m_pECModel,
		pDynData->GetCurTimeSpan() - GetStartTime());

	return true;
}

void ChildActInfo::DebugDrawTrail(const A3DVECTOR3& vOffset1, const A3DVECTOR3& vOffset2)
{
    if (m_avPos == NULL || m_aqDir == NULL || m_dwSegCount < 2)
        return;
    
    A3DVECTOR3* pVertices = new A3DVECTOR3[m_dwSegCount * 2];
    for (DWORD iPos = 0; iPos < m_dwSegCount; iPos++)
    {
        pVertices[iPos * 2] = m_aqDir[iPos] * vOffset1 + m_avPos[iPos];
        pVertices[iPos * 2 + 1] = m_aqDir[iPos] * vOffset2 + m_avPos[iPos];
    }

    DWORD dwColorArg1 = AfxGetA3DDevice()->GetDeviceTextureStageState(0, D3DTSS_COLORARG1);
    DWORD dwColorArg2 = AfxGetA3DDevice()->GetDeviceTextureStageState(0, D3DTSS_COLORARG2);
    DWORD dwColorOp = AfxGetA3DDevice()->GetDeviceTextureStageState(0, D3DTSS_COLOROP);
    AfxGetA3DDevice()->SetTextureColorArgs(0, D3DTA_TFACTOR, D3DTA_CURRENT);
    AfxGetA3DDevice()->SetTextureColorOP(0, A3DTOP_SELECTARG1);
    AfxGetA3DDevice()->SetTextureFactor(0xFFAAFFFF);
    AfxGetA3DDevice()->SetFVF(D3DFVF_XYZ);

    AfxGetA3DDevice()->DrawPrimitiveUP(A3DPT_LINELIST, m_dwSegCount, pVertices, sizeof(A3DVECTOR3));

    AfxGetA3DDevice()->SetTextureColorArgs(0, dwColorArg1, dwColorArg2);
    AfxGetA3DDevice()->SetTextureColorOP(0, (A3DTEXTUREOP)dwColorOp);
    delete[] pVertices;
}
bool MaterialScaleChange::Load(AFile* pFile, DWORD dwVersion)
{
	DWORD dwReadLen;

	if (pFile->IsBinary())
	{
		pFile->Read(&m_ScaleOrg.r, sizeof(m_ScaleOrg.r), &dwReadLen);
		pFile->Read(&m_ScaleOrg.g, sizeof(m_ScaleOrg.g), &dwReadLen);
		pFile->Read(&m_ScaleOrg.b, sizeof(m_ScaleOrg.b), &dwReadLen);
		pFile->Read(&m_ScaleOrg.a, sizeof(m_ScaleOrg.a), &dwReadLen);
		pFile->Read(&m_ScaleDelta.r, sizeof(m_ScaleDelta.r), &dwReadLen);
		pFile->Read(&m_ScaleDelta.g, sizeof(m_ScaleDelta.g), &dwReadLen);
		pFile->Read(&m_ScaleDelta.b, sizeof(m_ScaleDelta.b), &dwReadLen);
		pFile->Read(&m_ScaleDelta.a, sizeof(m_ScaleDelta.a), &dwReadLen);
		pFile->Read(&m_ScaleMin.r, sizeof(m_ScaleMin.r), &dwReadLen);
		pFile->Read(&m_ScaleMin.g, sizeof(m_ScaleMin.g), &dwReadLen);
		pFile->Read(&m_ScaleMin.b, sizeof(m_ScaleMin.b), &dwReadLen);
		pFile->Read(&m_ScaleMin.a, sizeof(m_ScaleMin.a), &dwReadLen);
		pFile->Read(&m_ScaleMax.r, sizeof(m_ScaleMax.r), &dwReadLen);
		pFile->Read(&m_ScaleMax.g, sizeof(m_ScaleMax.g), &dwReadLen);
		pFile->Read(&m_ScaleMax.b, sizeof(m_ScaleMax.b), &dwReadLen);
		pFile->Read(&m_ScaleMax.a, sizeof(m_ScaleMax.a), &dwReadLen);
		pFile->Read(&m_bApplyToChild, sizeof(m_bApplyToChild), &dwReadLen);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		if (dwVersion < 18)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_start_time, &m_dwStartTime);
		}

		if (dwVersion >= 11)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_matchg, &m_ScaleOrg.r, &m_ScaleOrg.g, &m_ScaleOrg.b, &m_ScaleOrg.a);
		}

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_matchg, &m_ScaleDelta.r, &m_ScaleDelta.g, &m_ScaleDelta.b, &m_ScaleDelta.a);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_matchg, &m_ScaleMin.r, &m_ScaleMin.g, &m_ScaleMin.b, &m_ScaleMin.a);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_matchg, &m_ScaleMax.r, &m_ScaleMax.g, &m_ScaleMax.b, &m_ScaleMax.a);

		int nBool;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_apply_child, &nBool);
		m_bApplyToChild = (nBool != 0);
	}

	return true;
}

bool MaterialScaleChange::Save(AFile* pFile)
{
	SaveEventBase(pFile);
	DWORD dwWriteLen;

	if (pFile->IsBinary())
	{
		pFile->Write(&m_ScaleDelta.r, sizeof(m_ScaleDelta.r), &dwWriteLen);
		pFile->Write(&m_ScaleDelta.g, sizeof(m_ScaleDelta.g), &dwWriteLen);
		pFile->Write(&m_ScaleDelta.b, sizeof(m_ScaleDelta.b), &dwWriteLen);
		pFile->Write(&m_ScaleDelta.a, sizeof(m_ScaleDelta.a), &dwWriteLen);
		pFile->Write(&m_ScaleMin.r, sizeof(m_ScaleMin.r), &dwWriteLen);
		pFile->Write(&m_ScaleMin.g, sizeof(m_ScaleMin.g), &dwWriteLen);
		pFile->Write(&m_ScaleMin.b, sizeof(m_ScaleMin.b), &dwWriteLen);
		pFile->Write(&m_ScaleMin.a, sizeof(m_ScaleMin.a), &dwWriteLen);
		pFile->Write(&m_ScaleMax.r, sizeof(m_ScaleMax.r), &dwWriteLen);
		pFile->Write(&m_ScaleMax.g, sizeof(m_ScaleMax.g), &dwWriteLen);
		pFile->Write(&m_ScaleMax.b, sizeof(m_ScaleMax.b), &dwWriteLen);
		pFile->Write(&m_ScaleMax.a, sizeof(m_ScaleMax.a), &dwWriteLen);
		pFile->Write(&m_bApplyToChild, sizeof(m_bApplyToChild), &dwWriteLen);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_matchg, m_ScaleOrg.r, m_ScaleOrg.g, m_ScaleOrg.b, m_ScaleOrg.a);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_matchg, m_ScaleDelta.r, m_ScaleDelta.g, m_ScaleDelta.b, m_ScaleDelta.a);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_matchg, m_ScaleMin.r, m_ScaleMin.g, m_ScaleMin.b, m_ScaleMin.a);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_matchg, m_ScaleMax.r, m_ScaleMax.g, m_ScaleMax.b, m_ScaleMax.a);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_apply_child, (int)m_bApplyToChild);
		pFile->WriteLine(szLine);
	}

	return true;
}

void 
_ApplyMatChgToChild(const A3DCOLORVALUE& cl, CECModel* pECModel, bool bIsApplyComboModel)
{
	for (int i = 0; i < pECModel->GetChildCount(); i++)
	{
		CECModel* pChild = pECModel->GetChildModel(i);
		A3DCOLORVALUE c(pChild->GetOrgColor());
		c = c * cl;
		A3DSkinModel * pSkinModel = pChild->GetA3DSkinModel();
		
		if (pSkinModel)
		{
			pSkinModel->SetMaterialMethod(A3DSkinModel::MTL_SCALED);
			pSkinModel->SetMaterialScale(c);
		}
		
		pChild->SetInnerAlpha(c.a);
	}

	if (!bIsApplyComboModel)
		return;

	for (int iComboIdx = 0; iComboIdx < pECModel->GetComboModelNum(); ++iComboIdx)
	{
		CECModel* pComboModel = pECModel->GetComboModel(iComboIdx);

		A3DCOLORVALUE c(1.0f);
		if (pComboModel->GetStaticData())
			c = pComboModel->GetOrgColor();

		c = c * cl;
		A3DSkinModel* pComboSkinModel = pComboModel->GetA3DSkinModel();

		if (pComboSkinModel)
		{
			pComboSkinModel->SetMaterialMethod(A3DSkinModel::MTL_SCALED);
			pComboSkinModel->SetMaterialScale(c);
		}

		pComboModel->SetInnerAlpha(c.a);
	}
}

bool MaterialScaleChange::Start(A3DCombActDynData* pDynData)
{
	CECModel* pModel = pDynData->m_pECModel;
	A3DSkinModel* pSkin = pModel->GetA3DSkinModel();
	MATCHG_BINDING* pBind = new MATCHG_BINDING(pDynData);
	pBind->SetInfo(this);
	pBind->GetScaleCur() = m_ScaleOrg;

	++A3DCombActDynData::s_CombActFxArrayCount;
	pDynData->m_ActFxArray.push_back(pBind);

	A3DCOLORVALUE c(pModel->GetOrgColor());
	c = c * m_ScaleOrg;

	if (pSkin)
	{
		pSkin->SetMaterialMethod(A3DSkinModel::MTL_SCALED);
		pSkin->SetMaterialScale(c);
	}

	pModel->SetInnerAlpha(c.a);

	if (m_bApplyToChild)
	{
		_ApplyMatChgToChild(m_ScaleOrg, pModel, true);
	}

	return true;
}

void MATCHG_BINDING::UpdateParam(CECModel* pECModel, int nDeltaTime)
{
	if (m_bStopped)
		return;

	A3DSkinModel* pSkin = pECModel->GetA3DSkinModel();
	MaterialScaleChange* pInfo = static_cast<MaterialScaleChange*>(m_pInfo);
	const A3DCOLORVALUE& clMin = pInfo->GetMin();
	const A3DCOLORVALUE& clMax = pInfo->GetMax();
	A3DCOLORVALUE& cl = m_ScaleCur;

	cl += pInfo->GetDelta() * (nDeltaTime / 1000.f);
	if (cl.r > clMax.r) { cl.r = clMax.r; m_bStopped = true; }
	if (cl.g > clMax.g) { cl.g = clMax.g; m_bStopped = true; }
	if (cl.b > clMax.b) { cl.b = clMax.b; m_bStopped = true; }
	if (cl.a > clMax.a) { cl.a = clMax.a; m_bStopped = true; }
	if (cl.r < clMin.r) { cl.r = clMin.r; m_bStopped = true; }
	if (cl.g < clMin.g) { cl.g = clMin.g; m_bStopped = true; }
	if (cl.b < clMin.b) { cl.b = clMin.b; m_bStopped = true; }
	if (cl.a < clMin.a) { cl.a = clMin.a; m_bStopped = true; }

	A3DCOLORVALUE c(pECModel->GetOrgColor());
	c = c * cl;

	if (pSkin)
		pSkin->SetMaterialScale(c);

	pECModel->SetInnerAlpha(c.a);

	if (pInfo->GetApplyToChild())
	{
		_ApplyMatChgToChild(cl, pECModel, true);
	}
}

bool SGCAttackPointMark::Load(AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
	}
	else
	{
		DWORD dwReadLen;
		char szLine[AFILE_LINEMAXLEN];
		if (dwVersion >= 23)
		{
			char szBuf[AFILE_LINEMAXLEN];
			szBuf[0] = '\0';

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_atk_path, szBuf);
			m_strAtkFile = szBuf;

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_divisions, &m_nDivisions);
		}

		if (dwVersion >= 38)
		{
			int nData = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_atk_usedelay, &nData);
			m_bIsUseAttDelay = nData != 0;
			
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_atk_delaycount, &nData);

			memset(m_nDelay, 0, DELAY_TYPE_NUM * sizeof(int));
			for (int i = 0; i < nData; ++i)
			{
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_atk_delaytime, &m_nDelay[i]);
			}
		}

		if (dwVersion >= 64)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_atk_orientation, &m_nAttackOrientation);
		}
	}

	return true;
}

bool SGCAttackPointMark::Save(AFile* pFile)
{
	SaveEventBase(pFile);

	if (pFile->IsBinary())
	{
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_atk_path, m_strAtkFile);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_divisions, m_nDivisions);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_atk_usedelay, m_bIsUseAttDelay);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_atk_delaycount, DELAY_TYPE_NUM);
		pFile->WriteLine(szLine);

		for (int i = 0; i < DELAY_TYPE_NUM; ++i)
		{
			sprintf(szLine, _format_atk_delaytime, m_nDelay[i]);
			pFile->WriteLine(szLine);
		}

		sprintf(szLine, _format_atk_orientation, m_nAttackOrientation);
		pFile->WriteLine(szLine);
	}

	return true;
}

/*
	当攻击点激活后，先播放对应的SGC，再匹配对应的伤害消息
*/
bool SGCAttackPointMark::Start(A3DCombActDynData* pDynData)
{
	// 如果出发此ATT的模型是由GFX创建出来的，则该事件触发失败
	if (pDynData->GetHostModel()->IsCreatedByGfx())
		return false;

	A3DGFXInterface::PARAM_PLAYSKILLGFX param;
	param.strAtkFile	= m_strAtkFile;
	param.SerialID		= pDynData->GetSerialId();
	param.nCasterID		= pDynData->GetCasterId();
	param.nCastTargetID	= pDynData->GetCastTargetId();
	param.vFixedPoint	= pDynData->GetFixedPoint();
	param.dwUserData	= pDynData->GetUserData();
	param.nDivisions	= m_nDivisions;
	param.pTargets		= &pDynData->m_TargetsInfo;
	param.nAttIndex		= m_pAction->GetEventIndexOfType(this, m_nType);
	param.nAttackOrientation = m_nAttackOrientation;
	AfxGetGFXExMan()->GetGfxInterface()->PlaySkillGfx(&param);

	if (pDynData->GetHostModel()->IsDelayActive()
		&& pDynData->m_TargetsInfo.size() == 1
		&& pDynData->m_TargetsInfo[0].bIsAttDelay
		&& m_bIsUseAttDelay)
	{
		int nAttDelayTimeIdx = pDynData->m_TargetsInfo[0].nAttDelayTimeIdx;
		ASSERT(nAttDelayTimeIdx >= 0
			&& nAttDelayTimeIdx < DELAY_TYPE_NUM);
		int nDelayTime = m_nDelay[nAttDelayTimeIdx];
		pDynData->GetHostModel()->StartECModelDelayMode(nDelayTime);

		if (CECModel::GetNotifyDelayStartFunc())
			CECModel::GetNotifyDelayStartFunc()(pDynData->GetCastTargetId(), pDynData->GetCasterId(), nDelayTime);
	}

	pDynData->m_TargetsInfo.clear();
	return true;
}

bool GfxScriptEvent::Load(AFile* pFile, DWORD dwVersion)
{
	DWORD dwReadLen;
	char szLine[AFILE_LINEMAXLEN];

	int len = 0;
	int lines = 0;
	AString str;
	int nBool;

	if (dwVersion >= 56)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_script_cfg_state, &nBool);
		m_bConfigState = (nBool != 0);
	}

	if (dwVersion >= 63)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		int intValue;
		if (sscanf(szLine, _format_script_usage, &intValue) == 1)
			m_subType = intValue;
		else
			m_subType = 1;
	}
	else	// old version
	{
		m_subType = 1;
	}

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_script_lines, &lines);

	if (lines == 0)
		return true;

	while (lines)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		str += szLine;
		lines--;
	}

	int buf_len = str.GetLength() + 32;
	char* pBuf = new char[buf_len];
	len = base64_decode((char*)(const char*)str, str.GetLength(), (unsigned char*)pBuf);
	pBuf[len] = 0;
	m_strScript = pBuf;
	delete[] pBuf;

	return true;
}

bool GfxScriptEvent::Save(AFile* pFile)
{
	char szLine[AFILE_LINEMAXLEN];
	SaveEventBase(pFile);

	m_strScript.TrimLeft();
	m_strScript.TrimRight();

	sprintf(szLine, _format_script_cfg_state, m_bConfigState);
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_script_usage, m_subType);
	pFile->WriteLine(szLine);

	if (m_strScript.IsEmpty())
	{
		sprintf(szLine, _format_script_lines, 0);
		pFile->WriteLine(szLine);
		return true;
	}

	int nBufLen = (m_strScript.GetLength() + 3) / 3 * 4 + 32;
	char* pBuf = new char[nBufLen];
	int len = base64_encode((unsigned char*)(const char*)m_strScript, m_strScript.GetLength()+1, pBuf);
	int nLines = len / 1500;

	if (len > nLines * 1500)
		nLines++;

	sprintf(szLine, _format_script_lines, nLines);
	pFile->WriteLine(szLine);

	const char* pWrite = pBuf;

	while (len)
	{
		int nWrite = len > 1500 ? 1500 : len;
		len -= nWrite;

		AString s(pWrite, nWrite);
		pFile->WriteLine(s);
		pWrite += nWrite;
	}

	delete[] pBuf;
	return true;
}

void GfxScriptEvent::GenerateFakeScriptParams(abase::vector<AString>& vargs)
{
	vargs.push_back(_script_var_name[enumECMVarSelf]);
	vargs.push_back(_script_var_name[enumECMVarId]);
	vargs.push_back(_script_var_name[enumECMVarModel]);
	vargs.push_back(_script_var_name[enumECMVarActChannel]);
	vargs.push_back(_script_var_name[enumECMVarCasterId]);
	vargs.push_back(_script_var_name[enumECMVarCastTargetId]);
}

bool GfxScriptEvent::Start(A3DCombActDynData* pDynData)
{
	CECModel* pModel = pDynData->GetHostModel();

	if (!pModel->IsScriptEventEnable())
		return true;

	if (!m_bInit) // 在此时加载脚本以避免多线程问题
	{
		static int _script_func_id = 1;
		m_strFuncName.Format("GfxScriptEvent_%d", _script_func_id++);
		abase::vector<AString> vargs;
		GenerateFakeScriptParams(vargs);

		if (AfxGetECModelMan()->IsActScriptInit())
		{
#ifdef FUNCTION_ANALYSER
			ATTACH_FUNCTION_ANALYSER(true, 3, 0, AString(pModel->GetFilePath()) + pDynData->GetComAct()->GetName());
#endif
			pModel->InitGlobalScript();

			if (m_bConfigState)
				m_bInit = AfxGetECModelMan()->GetActScriptTblConfigState().AddMethod(m_strFuncName, vargs, m_strScript);
			else
				m_bInit = AfxGetECModelMan()->GetActScriptTbl().AddMethod(m_strFuncName, vargs, m_strScript);
		}
	}

	if (m_bInit)
	{
#ifdef FUNCTION_ANALYSER
		ATTACH_FUNCTION_ANALYSER(true, 3, 0, AString(pModel->GetFilePath()) + pDynData->GetComAct()->GetName());
#endif

		abase::vector<CScriptValue> args, results;
		args.push_back(CScriptValue((void*)this));
		args.push_back(CScriptValue(_64BITS_ID(pModel->GetId())));
		args.push_back(CScriptValue((void*)pModel));
		args.push_back(CScriptValue((double)pDynData->GetActChannel()));
		args.push_back(CScriptValue(_64BITS_ID(pDynData->GetCasterId())));
		args.push_back(CScriptValue(_64BITS_ID(pDynData->GetCastTargetId())));

		if (m_bConfigState)
			AfxGetECModelMan()->GetActScriptTblConfigState().Call(m_strFuncName, args, results);
		else
			AfxGetECModelMan()->GetActScriptTbl().Call(m_strFuncName, args, results);
	}

	return true;
}

#ifdef GFX_EDITOR

//	Update script function to lua (remove if exist, then add)
bool GfxScriptEvent::UpdateScript()
{
	abase::vector<AString> vargs;
	GenerateFakeScriptParams(vargs);

	if (m_bInit)
	{
		if (m_bConfigState)
		{
			AfxGetECModelMan()->GetActScriptTblConfigState().RemoveMethod(m_strFuncName);
			return AfxGetECModelMan()->GetActScriptTblConfigState().AddMethod(m_strFuncName, vargs, m_strScript);
		}
		else
		{
			AfxGetECModelMan()->GetActScriptTbl().RemoveMethod(m_strFuncName);
			return AfxGetECModelMan()->GetActScriptTbl().AddMethod(m_strFuncName, vargs, m_strScript);
		}
	}

	return true;
}

#endif

//////////////////////////////////////////////////////////////////////////
//
//	Implement CameraControlEvent::CameraControlParam
//
//////////////////////////////////////////////////////////////////////////

CameraControlEvent::CameraControlParam::CameraControlParam()
: m_vCamera(0.f)
, m_vTarget(0.f)
, m_fCameraAngleAcc(0.f)
, m_fCameraLinearAcc(0.f)
, m_bIsInterp(false)
{

}

CameraControlEvent::CameraControlParam::~CameraControlParam()
{

}

bool CameraControlEvent::CameraControlParam::Load(AFile* pFile, DWORD dwVersion)
{
	if (!pFile)
		return false;

	char szLineBuf[AFILE_LINEMAXLEN];
	DWORD dwReadLen;

	// discarded after version 47
	if (dwVersion >= 45 && dwVersion < 47)
	{
		float fTempCameraDist2Host;
		pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuf, _format_cam_dist2host, &fTempCameraDist2Host);
		
		float fTempCameraYaw2Host;
		pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuf, _format_cam_yaw2host, &fTempCameraYaw2Host);
		
		float fTempCameraPitch;
		pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuf, _format_cam_pitch, &fTempCameraPitch);
		
		if (dwVersion >= 46)
		{
			float fTempCameraYawAcc;
			pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuf, _format_cam_yawacc, &fTempCameraYawAcc);
			
			float fTempCameraPitchAcc;
			pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuf, _format_cam_pitchacc, &fTempCameraPitchAcc);
			
			float fTempCameraLinearAcc;
			pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuf, _format_cam_linearacc, &fTempCameraLinearAcc);
		}

		int iReadInt = 0;
		pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuf, _format_cam_isinterp, &iReadInt);
		m_bIsInterp = (iReadInt != 0);
	}
	else if (dwVersion >= 47)
	{

		pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuf, _format_pos, &m_vCamera.x, &m_vCamera.y, &m_vCamera.z);

		pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuf, _format_pos, &m_vTarget.x, &m_vTarget.y, &m_vTarget.z);

		pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuf, _format_cam_angleacc, &m_fCameraAngleAcc);

		pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuf, _format_cam_linearacc, &m_fCameraLinearAcc);
		
		int iReadInt = 0;
		pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuf, _format_cam_isinterp, &iReadInt);
		m_bIsInterp = (iReadInt != 0);

		int iVertNum = 0;
		pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuf, _format_cam_beziernum, &iVertNum);

		for (int iVertIdx = 0; iVertIdx < iVertNum; ++iVertIdx)
		{
			A3DVECTOR3 vPos;
			pFile->ReadLine(szLineBuf, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuf, _format_cam_beziervert, &vPos.x, &vPos.y, &vPos.z);
			m_VertArray.push_back(vPos);
		}
	}


	return true;
}

bool CameraControlEvent::CameraControlParam::Save(AFile* pFile)
{
	if (!pFile)
		return false;

	char szLineBuf[AFILE_LINEMAXLEN];
	
	_snprintf(szLineBuf, AFILE_LINEMAXLEN, _format_pos, m_vCamera.x, m_vCamera.y, m_vCamera.z);
	pFile->WriteLine(szLineBuf);

	_snprintf(szLineBuf, AFILE_LINEMAXLEN, _format_pos, m_vTarget.x, m_vTarget.y, m_vTarget.z);
	pFile->WriteLine(szLineBuf);

	_snprintf(szLineBuf, AFILE_LINEMAXLEN, _format_cam_angleacc, m_fCameraAngleAcc);
	pFile->WriteLine(szLineBuf);

	_snprintf(szLineBuf, AFILE_LINEMAXLEN, _format_cam_linearacc, m_fCameraLinearAcc);
	pFile->WriteLine(szLineBuf);

	_snprintf(szLineBuf, AFILE_LINEMAXLEN, _format_cam_isinterp, m_bIsInterp);
	pFile->WriteLine(szLineBuf);

	_snprintf(szLineBuf, AFILE_LINEMAXLEN, _format_cam_beziernum, m_VertArray.size());
	pFile->WriteLine(szLineBuf);

	for (size_t nIdx = 0; nIdx < m_VertArray.size(); ++nIdx)
	{
		const A3DVECTOR3& vPos = m_VertArray[nIdx];
		_snprintf(szLineBuf, AFILE_LINEMAXLEN, _format_cam_beziervert, vPos.x, vPos.y, vPos.z);
		pFile->WriteLine(szLineBuf);
	}

	
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement CameraControlEvent
//
//////////////////////////////////////////////////////////////////////////

CameraControlEvent::CameraControlEvent(A3DCombinedAction* pAct)
: EVENT_INFO(pAct)
{
	m_nType = EVENT_TYPE_CAM_PT;
}

CameraControlEvent::~CameraControlEvent()
{

}

bool CameraControlEvent::Load(AFile* pFile, DWORD dwVersion)
{
	if (dwVersion < 44)
		return false;

	return m_Param.Load(pFile, dwVersion);
}

bool CameraControlEvent::Save(AFile* pFile)
{
	SaveEventBase(pFile);

	return m_Param.Save(pFile);
}

EVENT_INFO& CameraControlEvent::Clone(const EVENT_INFO& src)
{
	EVENT_INFO::Clone(src);
	CameraControlEvent* pSrcCamCtrlEvent = (CameraControlEvent*)&src;
	m_Param = pSrcCamCtrlEvent->m_Param;
	return *this;
}

void CameraControlEvent::SetCameraControlParam(const CameraControlParam* pParam)
{
	if (!pParam)
		return;

	m_Param = *pParam;
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement BoneScaleChange
//
//////////////////////////////////////////////////////////////////////////

static const char * _format_bone_scale_dest_param = "SclDest: %f %f %d";

bool BoneScaleChange::ScaleDestParam::Save(AFile* pFile)
{
	char szBuffer[AFILE_LINEMAXLEN];
	_snprintf(szBuffer, AFILE_LINEMAXLEN, _format_bone_scale_dest_param, m_fDestScale, m_fScaleFactor, m_dwScaleTime);
	pFile->WriteLine(szBuffer);
	return true;
}

bool BoneScaleChange::ScaleDestParam::Load(AFile* pFile, DWORD dwVersion)
{
	char szBuffer[AFILE_LINEMAXLEN];
	DWORD dwRead = 0;

	pFile->ReadLine(szBuffer, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szBuffer, _format_bone_scale_dest_param, &m_fDestScale, &m_fScaleFactor, &m_dwScaleTime);
	return true;
}

static const char * _format_bone_scale_name_read = "Name: %[^\n]";
static const char * _format_bone_scale_name_write = "Name: %s";
static const char * _format_bone_scale_param = "SclParam: %d %f %d";

bool BoneScaleChange::BoneScaleParam::Save(AFile* pFile)
{
	char szBuffer[AFILE_LINEMAXLEN];
	_snprintf(szBuffer, AFILE_LINEMAXLEN, _format_bone_scale_name_write, m_strBone);
	pFile->WriteLine(szBuffer);

	_snprintf(szBuffer, AFILE_LINEMAXLEN, _format_bone_scale_param, m_iScaleType, m_fStartScale, m_iDestNum);
	pFile->WriteLine(szBuffer);

	for (int iDestIdx = 0; iDestIdx < m_iDestNum; ++iDestIdx)
	{
		if (!m_aScaleArray[iDestIdx].Save(pFile))
			return false;
	}

	return true;
}

bool BoneScaleChange::BoneScaleParam::Load(AFile* pFile, DWORD dwVersion)
{
	char szBuffer[AFILE_LINEMAXLEN];
	char szBoneName[AFILE_LINEMAXLEN];
	DWORD dwRead = 0;
	pFile->ReadLine(szBuffer, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szBuffer, _format_bone_scale_name_read, szBoneName);
	m_strBone = szBoneName;

	pFile->ReadLine(szBuffer, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szBuffer, _format_bone_scale_param, &m_iScaleType, &m_fStartScale, &m_iDestNum);

	for (int iDestIdx = 0; iDestIdx < m_iDestNum; ++iDestIdx)
	{
		if (!m_aScaleArray[iDestIdx].Load(pFile, dwVersion))
			return false;
	}

	return true;
}

static const char * _format_use_file_scale = "Use File Scale: %d";
static const char * _format_bone_scale_num = "Num: %d";

BoneScaleChange::BoneScaleChange(A3DCombinedAction* pAct)
: EVENT_INFO(pAct)
, m_bUseFileScale(false)
{
	m_nType = EVENT_TYPE_MODELSCLCHG;
}

BoneScaleChange::~BoneScaleChange()
{
	RemoveAllScaleParams();
}

void BoneScaleChange::RemoveAllScaleParams()
{
	for (int iBoneSclIdx = 0; iBoneSclIdx < m_aBoneScaleChanges.GetSize(); ++iBoneSclIdx)
	{
		delete m_aBoneScaleChanges[iBoneSclIdx];
	}

	m_aBoneScaleChanges.RemoveAll();
}

bool BoneScaleChange::AddBoneScaleParam(const BoneScaleParam& boneSclParam)
{
	if (GetBoneScaleParam(boneSclParam.m_strBone))
		return false;

	m_aBoneScaleChanges.Add(new BoneScaleParam(boneSclParam));
	return true;
}

void BoneScaleChange::RemoveBoneScaleParam(const char* szName)
{
	int iIndex = -1;
	if (!GetBoneScaleParam(szName, &iIndex))
		return;

	delete m_aBoneScaleChanges[iIndex];
	m_aBoneScaleChanges.RemoveAt(iIndex);
}

BoneScaleChange::BoneScaleParam* BoneScaleChange::GetBoneScaleParam(const char* szName, int* piIndex /*= NULL*/) const
{
	for (int iBoneSclIdx = 0; iBoneSclIdx < m_aBoneScaleChanges.GetSize(); ++iBoneSclIdx)
	{
		BoneScaleParam* pParam = m_aBoneScaleChanges[iBoneSclIdx];
		if (pParam->m_strBone == szName)
		{
			if (piIndex)
				*piIndex = iBoneSclIdx;

			return pParam;
		}
	}

	if (piIndex)
		*piIndex = -1;

	return NULL;
}

bool BoneScaleChange::Load(AFile* pFile, DWORD dwVersion)
{
	char szBuffer[AFILE_LINEMAXLEN];
	DWORD dwRead = 0;

	int iBoneSclNum = 0;
	pFile->ReadLine(szBuffer, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szBuffer, _format_bone_scale_num, &iBoneSclNum);

	for (int iBoneSclIdx= 0; iBoneSclIdx < iBoneSclNum; ++iBoneSclIdx)
	{
		BoneScaleParam* pParam = new BoneScaleParam();
		if (!pParam->Load(pFile, dwVersion))
		{
			a_LogOutput(1, "Error in BoneScaleChange::Load, Failed to load BoneScaleParam.");
			delete pParam;
			return false;
		}

		m_aBoneScaleChanges.Add(pParam);
	}

	if (dwVersion >= 67)
	{
		int nRead = 0;
		pFile->ReadLine(szBuffer, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szBuffer, _format_use_file_scale, &nRead);
		m_bUseFileScale = (nRead != 0);
	}

	return true;
}

bool BoneScaleChange::Save(AFile* pFile)
{
	char szBuffer[AFILE_LINEMAXLEN];
	SaveEventBase(pFile);
	
	_snprintf(szBuffer, AFILE_LINEMAXLEN, _format_bone_scale_num, m_aBoneScaleChanges.GetSize());
	pFile->WriteLine(szBuffer);

	for (int iBoneScaleIdx = 0; iBoneScaleIdx < m_aBoneScaleChanges.GetSize(); ++iBoneScaleIdx)
	{
		if (!m_aBoneScaleChanges[iBoneScaleIdx]->Save(pFile))
		{
			a_LogOutput(1, "Error in BoneScaleChange::Save, Can not save bone scale info.");
			return false;
		}
	}

	_snprintf(szBuffer, AFILE_LINEMAXLEN, _format_use_file_scale, m_bUseFileScale);
	pFile->WriteLine(szBuffer);

	return true;
}

EVENT_INFO& BoneScaleChange::Clone(const EVENT_INFO& src)
{
	if (this == &src)
		return *this;
	
	EVENT_INFO::Clone(src);
	BoneScaleChange* pSrcEvent = (BoneScaleChange*)(&src);
	
	RemoveAllScaleParams();
	for (int iBoneSclIdx = 0; iBoneSclIdx <  pSrcEvent->m_aBoneScaleChanges.GetSize(); ++iBoneSclIdx)
		AddBoneScaleParam(*(pSrcEvent->m_aBoneScaleChanges[iBoneSclIdx]));
	
	return *this;
}

bool BoneScaleChange::Start(A3DCombActDynData* pDynData)
{
	MODEL_SCALE_BINDING* pFx = new MODEL_SCALE_BINDING(pDynData, this, pDynData->GetModelScale());
	pFx->SetInfo(this);

	++A3DCombActDynData::s_CombActFxArrayCount;
	pDynData->m_ActFxArray.push_back(pFx);
	return true; 
}

//////////////////////////////////////////////////////////////////////////
//
//	Implememnt MaterialScaleTrans
//
//////////////////////////////////////////////////////////////////////////

MaterialScaleTrans::MaterialScaleTrans(A3DCombinedAction* pAct) :
EVENT_INFO(pAct),
m_bApplyToChild(false),
m_clOrg(A3DCOLORRGBA(255, 255, 255, 255))
{ 
	m_nType = EVENT_TYPE_MATTRANS; 
	m_nDestNum = 1;
	
	for (int i = 0; i < COLOR_TBL_MAX_NUM; i++)
	{
		m_clDest[i] = A3DCOLORRGBA(255, 255, 255, 255);
		m_fTransTime[i] = 100.0f;
	}
	
	m_fTotalSpan = m_fTransTime[0];
}

MaterialScaleTrans::~MaterialScaleTrans()
{

}

void MaterialScaleTrans::SetDestNum(int iDestNum)
{
	if (iDestNum > COLOR_TBL_MAX_NUM)
		iDestNum = COLOR_TBL_MAX_NUM;

	if (iDestNum < 0)
		iDestNum = 0;

	m_nDestNum = iDestNum;
}

void MaterialScaleTrans::SetDestColor(int iIdx, A3DCOLOR dwColor)
{
	ASSERT( iIdx < COLOR_TBL_MAX_NUM && iIdx >= 0 );
	m_clDest[iIdx] = dwColor;
}

void MaterialScaleTrans::SetTransTime(int iIdx, float fTransTime)
{
	ASSERT( iIdx < COLOR_TBL_MAX_NUM && iIdx >= 0 );
	m_fTransTime[iIdx] = fTransTime;
}

bool MaterialScaleTrans::Load(AFile* pFile, DWORD dwVersion)
{
	//	should only be in version 51 or upper files
	if (dwVersion < 51)
		return false;

	char szLine[AFILE_LINEMAXLEN];
	DWORD dwRead = 0;

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_mst_destcol, &m_clOrg);

	m_nDestNum = 0;
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_mst_destnum, &m_nDestNum);
	
	if (dwVersion >= 55)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		int nBool = 0;
		sscanf(szLine, _format_apply_child, &nBool);
		m_bApplyToChild = (nBool != 0);
	}

	for (int i = 0; i < m_nDestNum; ++i)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		if (i < COLOR_TBL_MAX_NUM) 
			sscanf(szLine, _format_mst_destcol, &m_clDest[i]);
		
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		if (i < COLOR_TBL_MAX_NUM)
			sscanf(szLine, _format_mst_desttime, &m_fTransTime[i]);
	}	


	CalcLookupTbl();

	return true;
}

bool MaterialScaleTrans::Save(AFile* pFile)
{
	char szLine[AFILE_LINEMAXLEN];
	SaveEventBase(pFile);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_mst_destcol, m_clOrg);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_mst_destnum, m_nDestNum);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_apply_child, (int)m_bApplyToChild);
	pFile->WriteLine(szLine);

	for (int i = 0; i < m_nDestNum; ++i)
	{
		_snprintf(szLine, AFILE_LINEMAXLEN, _format_mst_destcol, m_clDest[i]);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_mst_desttime, m_fTransTime[i]);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool MaterialScaleTrans::Start(A3DCombActDynData* pDynData)
{
	MATTRANS_BINDING* pFx = new MATTRANS_BINDING(pDynData, this);
	pFx->SetInfo(this);

	++A3DCombActDynData::s_CombActFxArrayCount;
	pDynData->m_ActFxArray.push_back(pFx);

	CECModel* pModel = pDynData->GetHostModel();
	A3DCOLORVALUE c(pModel->GetOrgColor());
	c = c * A3DCOLORVALUE(m_clOrg);

	A3DSkinModel* pSkin = pModel->GetA3DSkinModel();
	if (pSkin)
	{
		pSkin->SetMaterialMethod(A3DSkinModel::MTL_SCALED);
		pSkin->SetMaterialScale(c);
	}

	pModel->SetInnerAlpha(c.a);

	if (m_bApplyToChild)
	{
		_ApplyMatChgToChild(m_clOrg, pModel, true);
	}

	return true; 
}

EVENT_INFO& MaterialScaleTrans::Clone(const EVENT_INFO& src)
{
	EVENT_INFO::Clone(src);
	const MaterialScaleTrans* pSrc = static_cast<const MaterialScaleTrans*>(&src);
	
	m_bApplyToChild	= pSrc->m_bApplyToChild;
	
	m_clOrg			= pSrc->m_clOrg;
	m_nDestNum		= pSrc->m_nDestNum;
	m_fTotalSpan	= pSrc->m_fTotalSpan;
	
	for (int i = 0; i < m_nDestNum; i++)
	{
		m_clDest[i] = pSrc->m_clDest[i];
		m_fTransTime[i] = pSrc->m_fTransTime[i];
	}
	
	return *this;
}

A3DCOLOR MaterialScaleTrans::GetCurrentColor( int nSeg, float fPortion ) const
{
	A3DCOLOR finalColor;
	if (nSeg == 0)
		finalColor = Interp_Color(m_clOrg, m_clDest[0], fPortion);
	else
		finalColor = Interp_Color(m_clDest[nSeg-1], m_clDest[nSeg], fPortion);
	return finalColor;
}

void MaterialScaleTrans::CalcLookupTbl()
{
	m_fTotalSpan = 0.0f;

	for (int n = 0; n < m_nDestNum; n++)
	{
		m_fTotalSpan += m_fTransTime[n];
	}
}
//////////////////////////////////////////////////////////////////////////
//
//	Implememnt AUDIOEVENT_INFO
//
//////////////////////////////////////////////////////////////////////////

AUDIOEVENT_INFO::AUDIOEVENT_INFO(A3DCombinedAction* pAct) 
: FX_BASE_INFO(pAct)
{
	m_nType = EVENT_TYPE_AUDIOEVENT;
	m_bOnce = true;
	m_fMinDist = 1.0f;
	m_fMaxDist = 10000.0f;
	m_fVolume = 1.0f;
	m_bUseCustom = false;
	m_nGroup = 0;
	m_bFadeout = true;
}

AUDIOEVENT_INFO::~AUDIOEVENT_INFO()
{
}

bool AUDIOEVENT_INFO::Start( A3DCombActDynData* pDynData )
{
#ifdef _USEAUDIOENGINE
	ASSERT(pDynData);
	if (!pDynData)
		return false;

	bool bEnable = pDynData->GetHostModel()->GetStaticData()->IsAudioGroupEnable(m_nGroup);
	if (!bEnable)
		return false;

	A3DSkinModel* pModel = pDynData->GetHostModel()->GetA3DSkinModel();

	AString strEvent(GetFilePath());
	if (strEvent.IsEmpty())
		return false;

	if (AudioEngine::EventInstance* pEventInstance = AfxGetGFXExMan()->GetGfxInterface()->CreateAudioEventInstance(strEvent, true))
	{
		AudioEngine::EVENT_PROPERTY prop;
		pEventInstance->GetProperty(prop);
		if (m_bUseCustom)
		{			
			prop.fMinDistance = m_fMinDist;
			prop.fMaxDistance = m_fMaxDist;
			pEventInstance->SetProperty(prop);
		}

		if(prop.mode == AudioEngine::MODE_3D)
		{
			AudioEngine::VECTOR pos, dir;
			A3DVECTOR3 vPos = pModel->GetPos();
			A3DVECTOR3 vDir = pModel->GetDir();
			pos.x = vPos.x;
			pos.y = vPos.y;
			pos.z = vPos.z;
			dir.x = vDir.x;
			dir.y = vDir.y;
			dir.z = vDir.z;
			pEventInstance->Set3DAttributes(pos, 0);
			pEventInstance->Set3DConeOrientation(dir);
		}

		pEventInstance->SetVolume(m_fVolume);

		if (!pEventInstance->Start())
		{
			AfxGetGFXExMan()->GetGfxInterface()->ReleaseAudioEventInstance(pEventInstance);
			return false;
		}

		PAUDIOEVENT_BINDING pBind = new AUDIOEVENT_BINDING(pDynData, pEventInstance);
		pBind->SetInfo(this);

		++A3DCombActDynData::s_CombActFxArrayCount;
		pDynData->m_ActFxArray.push_back(pBind);
		return true;
	}
	else
		return false;

#else
	return false;
#endif
}

bool AUDIOEVENT_INFO::Load( AFile* pFile, DWORD dwVersion )
{
	if (!FX_BASE_INFO::LoadBase(pFile, dwVersion))
		return false;

	DWORD dwReadLen;

	char szLine[AFILE_LINEMAXLEN];
	if (dwVersion >= 61)
	{
		int iRead = 0;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_audiousecustom, &iRead);
		m_bUseCustom = iRead != 0;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_audiomindist, &m_fMinDist);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_audiomaxdist, &m_fMaxDist);
	}

	if (dwVersion >= 65)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_audiovolume, &m_fVolume);
	}

	if (dwVersion >= 68)
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_audiogroup, &m_nGroup);
	}

	if (dwVersion >= 71)
	{
		int iRead = 0;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_audiofadeout, &iRead);
		m_bFadeout = iRead != 0;
	}

	return true;
}

bool AUDIOEVENT_INFO::Save( AFile* pFile )
{
	if (!SaveBase(pFile))
		return false;

	char	szLine[AFILE_LINEMAXLEN];

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_audiousecustom, m_bUseCustom);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_audiomindist, m_fMinDist);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_audiomaxdist, m_fMaxDist);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_audiovolume, m_fVolume);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_audiogroup, m_nGroup);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_audiofadeout, m_bFadeout);
	pFile->WriteLine(szLine);

	return true;
}

EVENT_INFO& AUDIOEVENT_INFO::Clone( const EVENT_INFO& src )
{
	FX_BASE_INFO::Clone(src);
	const AUDIOEVENT_INFO* pSrc = static_cast<const AUDIOEVENT_INFO*>(&src);

	m_fMinDist = pSrc->m_fMinDist;
	m_fMaxDist = pSrc->m_fMaxDist;
	m_fVolume  = pSrc->m_fVolume;
	m_bUseCustom = pSrc->m_bUseCustom;
	m_nGroup = pSrc->m_nGroup;

	return *this;
}

void AUDIOEVENT_INFO::SetVolume(float fVolume)
{ 
	m_fVolume = fVolume; 
	a_Clamp(m_fVolume, 0.0f, 1.0f);
}

void AUDIOEVENT_INFO::SetMinMaxDist(float fMinDist, float fMaxDist)
{
	m_fMinDist = fMinDist;
	m_fMaxDist = fMaxDist;
}



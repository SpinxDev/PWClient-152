/*
 * FILE: EC_Model.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#include <A3DEngine.h>
#include <AFileImage.h>
#include <A3DBone.h>
#include <A3DCamera.h>
#include <A3DSkinModel.h>
#include <A3DSkinModelAux.h>
#include <AMSoundBufferMan.h>
#include <A3DSkinModelAct.h>
#include <A3DConfig.h>
#include <shlwapi.h>
#include <afi.h>
#include <A3DJoint.h>
#include <A3DPI.h>

#ifdef A3D_PHYSX
	#include "A3DModelPhysSync.h"
	#include "A3DModelPhysSyncData.h"
	#include "A3DModelPhysics.h"
#endif

#include "base64.h"
#include "EC_Model.h"
#include "EC_ModelMan.h"
#include "EC_ModelHook.h"
#include "LuaUtil.h"
#include "scriptvalue.h"
#include "LuaAPI.h"
#include "LuaState.h"
#ifndef GFX_EDITOR
	#include "ConvexHullData.h"
	#include "ConvexHullArchive.h"
#endif

#include "EC_ModelLua.h"
#include "EC_ModelBlur.h"
#include "EC_ModelPhys.h"
#include "EC_AnimNodeMan.h"


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

// 为了兼容不同国家语言code page
char* ECMPathFindFileNameA(const char* szPath)
{
	char* ret = NULL;
	int i = 0;
	char ch;

	while ((ch = szPath[i]) != 0)
	{
		if (::IsDBCSLeadByteEx(936, ch))
		{
			if (szPath[i+1] == 0)
				break;

			i += 2;
			continue;
		}

		if (ch == '\\' || ch == '/' || ch == ':')
			ret = (char*)szPath + i;

		i++;
	}

	if (ret == NULL)
		return (char*)szPath;

	return ret + 1;
}

char* ECMPathFindExtensionA(const char* szPath)
{
	char* ret = NULL;
	int i = 0;
	char ch;

	while ((ch = szPath[i]) != 0)
	{
		if (::IsDBCSLeadByteEx(936, ch))
		{
			i++;

			if (szPath[i] == 0)
				break;

			i++;
			continue;
		}

		if (ch == '.')
			ret = (char*)szPath + i;

		i++;
	}

	if (ret)
		return ret;

	return (char*)szPath + i;
}

static void ExpandStringBuffer(AString& str, int iLength)
{
	a_ClampFloor(iLength, 64);
	char* tmp_buffer = str.GetBuffer(iLength);
	if (tmp_buffer)
	{
		tmp_buffer[0] = '\0';
	}
	str.ReleaseBuffer();
}

#define MODEL_FILE_TYPE	AFILE_TEXT

#define MODEL_CHILD_PHYDELAY_TICK 2

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


CECModel::GFX_NOTIFY_DELAY_START_FUNC CECModel::m_NotifyDelayStartFunc = NULL;
extern float _ecm_total_tick_time;
extern bool s_bShowTrackBoneVel = false;
int g_nECMDeadlockFlag = 0;

#ifdef _ANGELICA21
	#include <A3DExternalFunc.h>
	CECModel::ECM_CUSTOM_RENDER_FUNC CECModel::m_CustomRenderFunc = NULL;
#else
	CECModel::ECM_CUSTOM_RENDER_FUNC CECModel::m_CustomRenderFunc = NULL;
#endif

CECModel::ECM_GET_LIGHT_INFO_FUNC CECModel::m_GetLightInfoFunc = NULL;

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

bool CECModel::s_bForbidPhys = false;

static const char* _format_file_version		= "Version: %d";
static const char* _format_skinmodel_path	= "SkinModelPath: %s";
static const char* _format_addi_skin_count	= "AddiSkinCount: %d";
static const char* _format_addi_skin_path	= "AddiSkinPath: %s";
static const char* _format_comact_count		= "ComActCount: %d";
static const char* _format_child_count		= "ChildCount: %d";
static const char* _format_child_path		= "ChildPath: %s";
static const char* _format_child_name		= "ChildName: %s";
static const char* _format_hh_name			= "HHName: %s";
static const char* _format_cc_name			= "CCName: %s";
static const char* _format_cogfx_num		= "CoGfxNum: %d";
static const char* _format_new_scale		= "NewScale: %d";
static const char* _format_bone_num			= "BoneNum: %d";
static const char* _format_bone_index		= "BoneIndex: %d";
static const char* _format_bone_scl_type	= "BoneSclType: %d";
static const char* _format_bone_scale		= "BoneScale: %f, %f, %f";
static const char* _format_org_color		= "OrgColor: %x";
static const char* _format_emissive_col		= "EmissiveCol: %x";
static const char* _format_def_speed		= "DefSpeed: %f";
static const char* _format_srcbld			= "SrcBlend: %d";
static const char* _format_destbld			= "DestBlend: %d";
static const char* _format_float			= "Float: %f";
static const char* _format_outer_num		= "OuterNum: %d";
static const char* _format_script_count		= "ScriptCount: %d";
static const char* _format_id				= "id: %d";
static const char* _format_script_lines		= "ScriptLines: %d";
static const char* _format_channel_count	= "ChannelCount: %d";
static const char* _format_channel			= "Channel: %d";
static const char* _format_channel_mask		= "ChannelMask: %d";
static const char* _format_autoupdate		= "AutoUpdata: %d";
static const char* _format_phys_file		= "PhysFileName: %s";
static const char* _format_ecmhook_count	= "ECMHookCount: %d";
static const char* _format_can_castshadow	= "CanCastShadow: %d";
static const char* _format_render_model		= "RenderModel: %d";
static const char* _format_render_edge		= "RenderEdge: %d";
static const char* _format_shaderpath		= "ShaderFile: %s";
static const char* _format_shadertex		= "ShaderTex: %s";
static const char* _format_psconstcount		= "PSConstCount: %d";
static const char* _format_psindex			= "PSConstIndex: %d";
static const char* _format_psvalue			= "PSConstValue: %f, %f, %f, %f";
static const char* _format_pstargetcount	= "PSTargetCount: %d";
static const char* _format_psinterval		= "PSInterval: %d";
static const char* _format_psloop			= "PSLoopCount: %d";
static const char* _format_audiogroupenable = "AudioEventGroupEnable: %d";
static const char* _format_particlebones_count = "ParticleBonesCount: %d";

#define GFX_FADE_OUT_TIME	1000

/*
 *	V2 04.11.27
 *  Remark: Add Bind Parent
 *	V3 04.12.11
 *	Remark: Add Com Act Loops
 *	V5 04.12.20
 *	Remark: Add Child Models Hierarchy
 *	V6 05.1.4
 *	Remark: Update com action loop
 *	V7 05.1.4
 *	Remark: add event info
 *	V8 05.1.18
 *	Remark: add additional skin
 *	V9 05.1.19
 *	Remark: add per basic act loop
 *	V10 05.1.24
 *	Remark: add gfx play speed control
 *	V11 05.1.29
 *	Remark: add material change org val
 *	V12 05.2.1
 *	Remark: update blade trail
 * 	V13 05.3.26
 *	Remark: update can fadeout
 *	V14 05.4.9
 *	Remark: add co gfx
 *	V15 05.4.28
 *	Remark: update sfx info, now can fade out
 *	V16	05.5.12
 *	Remark: add bone scale
 *	V17 05.5.13
 *	Remark: use model alpha
 *	V18	05.5.18
 *	Remark: event play once in loops
 *	V19	05.6.10
 *	Remark: add hook rot angle
 *	V20	05.10.11
 *	Remark: add event timespan
 *	V21	05.10.20
 *	Remark: add blend mode
 *	V22 06.4.20
 *	Remark: gfx use outer path
 *	V23	06.5.22
 *	Remark: attack point add path and divisions attr
 *	V24	06.5.29
 *	Remark: add gfx alpha
 *	V25	06.8.21
 *	Remark: add scripts
 *	V26	06.9.25
 *	Remark: script length unlimited
 *	V27	07.1.3
 *	Remark: add def play speed
 *	V28 07.6.8
 *	Remark: add new bone scale method
 *	V29 07.7.5
 *	Remark: add scale base bone
 *	V30 07.8.20
 *	Remark: add comact rank
 *	V31	07.8.20
 *	Remark: add model channel info
 *	V32 07.8.22
 *	Remark: add channel event mask, comact event channel
 *	V33	08.1.24
 *	Remark: save ecm auto update flag
 *	V34 08.6.5
 *	Remark: add physics data
 *	V35 08.10.30
 *	Remark: add gfx relative to ECModel original flag
 *	V36 08.12.19
 *	Remark: change ACTION_INFO's m_nLoops to m_nMinLoop and m_nMaxLoop, may generate a random loop number if minLoop != maxLoop
 *	V37 08.12.23
 *	Remark: change the save method of ECM's smd filepath, version before37, save the path as (NOTION: Relative path's head NEVER has a "\", it just starts by Models\\) Models\\...\\xxx.smd
 *			After version 37, will only save the smd's filename
 *	V38 09.1.12
 *	Remark: change the save method of ECM's smd filepath, version 37 would only save the smd's filename and try to find the file in the ecm's path
 *			After version 38, will save the relative pathname of the smd file
 *	V39 09.2.12			
 *	Remark: attack point add delaytime
 *	V40 09.3.6
 *	Remark: add playspeed to A3DCombinedAction
 *	V41 09.5.21
 *	Remark: add ECModelHook to CECModelStaticData
 *	V42 09.6.4
 *	Remark: add child act transtime
 *	V43	09.11.4
 *	Remark: add can cast shadow flag
 *	V44 09.11.5
 *	Remark: add a new event type of camera control point
 *	V45 09.11.9
 *	Remark: can render skin model
 *	V46 09.11.11
 *	Remark: add accelaration to camera control point's movement
 *	V47 09.11.18
 *	Remark: big changes to camera control point's parameters
 *			Add bezier control style keypoint, keep keypoint interp mode, add camera's target point
 *	V48 09.12.21
 *	Remark: add render edge flag
 *	V49	10.1.7
 *	Remark: add a new event type of model scale change
 *			add StopChildrenAct property to A3DCombinedAction
 *			add ResetMaterialScale property to A3DCombinedAction
 *	V50 10.2.26
 *	Remark: change ecm's gfx_info fadeout property from a boolean flag to a time period
 *	V51 10.3.11
 *	Remark: a new event type of MaterialScaleTrans
 *	V52	10.3.12
 *	Remark: add ExtraEmissive Color to model file, it takes effect when loading
 *	V53 10.5.13
 *	Remark: FX_BASE_INFO add random files support
 *	V54 10.5.14
 *	Remark: GFX_INFO add delay time in order to appear a delay effect while updated by hook or model
 *	V55 10.9.19
 *	Remark: fix MaterialScaleTrans apply to child property is not saved
 *	V56	10.9.27
 *	Remark: script event now can run in config state
 *	V57 11.2.28
 *	Remark: Now A3DShader supported, but only supported in dx9 for some flags only active in it
 *	V58	11.3.15
 *	Remark: FX_BASE_INFO add custom file path method, application can change the default path
 *	V59 11.3.18
 *	Remark: Add shader consts loop property
 *	V60	11.5.31
 *	Remark: Shader info can be loaded from file
 *	V61 11.7.27
 *	Remark: Add Audio type event
 *	V62 11.10.24
 *	Remark: ECM Event add custom data property
 *	V63 11.12.22
 *	Remark: GfxScriptEvent add member 'usage' to identify event sub type
 *	V64 12.1.19
 *	Remark: SGC event add orientation
 *	V65 12.2.7
 *	Remark: AudioEventInfo add audio volume property
 *  V66 12.3.14
 *	Remark: Gfx info cogfx rot with model
 *  V67 12.4.16
 *  Remark: Bone Scale Info use file scale when Stopped
 *	V68 12.5.16
 *	Remark: AudioEvent Group Support
 *  V69 12.5.23
 *	Remark: Gfx use Fixed Point property
 *	v70 12.5.24
 *  Remark: ECModel particle bones
 *  V71 12.7.9
 *  Remark: AudioEvent StopWhenDestruct option
 */

DWORD EC_MODEL_CUR_VERSION = 71;

#define ECMODEL_PROJ_MAGIC (unsigned int) (('e'<<24) | ('c' << 16) | ('b' << 8) | ('p'))
#define ECMODEL_PROJ_VERSION (unsigned int) (0x00000001)

struct ECModelBrushProjHead_t
{
	unsigned int	nMagic;
	unsigned int	nVersion;
	char			szECModelFile[MAX_PATH];
	int				nFrame;
	unsigned int	nNumHull;
};

#ifndef GFX_EDITOR

	extern const char* AfxGetECMHullPath();

#endif

// script

const char* _script_descript[enumECMScriptCount] =
{
	"每次播放动作时生效",
	"每次停止动作时生效",
	"每次加载时生效",
	"每次释放时生效",
	"每个模型加载后生效",
	"每次换装备时生效",
	"每次加载时生效",
	"每次物理破碎时生效",
};

const char* _script_func_name[enumECMScriptCount] =
{
	"StartAct",
	"EndAct",
	"Init",
	"Release",
	"ModelLoaded",
	"ChangeEquip",
	"ChangeEquipTableInit",
	"OnPhysBreak",
};

const ECM_SCRIPT_VAR ecm_script_var_tb[enumECMScriptCount] =
{
	{	4,	enumECMVarModel, enumECMVarActChannel, enumECMVarActName, enumECMVarFashionMode },
	{   3,  enumECMVarModel, enumECMVarActChannel, enumECMVarActName },
	{   0	},
	{   0	},
	{	1,	enumECMVarModel	},
	{	6,	enumECMVarModel, enumECMVarEquipId, enumECMVarPathId, enumECMVarEquipFlag, enumECMVarEquipIndex, enumECMVarFashionMode },
	{	0	},
	{	4,	enumECMVarModel, enumECMVarBreakOffsetX, enumECMVarBreakOffsetY, enumECMVarBreakOffsetZ	},
};

const char* _script_var_name[enumECMVarCount] =
{
	"model",
	"act_name",
	"act_channel",
	"equip_id",
	"equip_flag",
	"fashion_mode",
	"path_id",
	"id",
	"equip_index",
	"break_offset_x",
	"break_offset_y",
	"break_offset_z",
	"caster_id",
	"casttarget_id",
	"self",
};

int ecm_get_var_count(int func)
{
	return ecm_script_var_tb[func].var_count;
}

const char* ecm_get_var_name(int func, int var_index)
{
	return _script_var_name[ecm_script_var_tb[func].var_index[var_index]];
}

inline AString _format_ecm_table_name(void* p)
{
	AString strTable;
	strTable.Format("ECM%x", (int)p);
	return strTable;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static int _str_compare(const void* arg1, const void* arg2)
{
	A3DCombinedAction* p1;
	A3DCombinedAction* p2;
	p1 = *(A3DCombinedAction**)arg1;
	p2 = *(A3DCombinedAction**)arg2;
	return strcmp(p1->GetName(), p2->GetName());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement TrackBoneVelocity
//	
///////////////////////////////////////////////////////////////////////////

//#define TRACKBONE_LOG

class TrackBoneVelocity
{
private:
	TrackBoneVelocity(const TrackBoneVelocity&);
	TrackBoneVelocity& operator = (const TrackBoneVelocity&);

public:		//	Types

	enum 
	{
		MAXNODE_CNT = 3,
	};

	struct NODE
	{
		NODE() { Reset(); }
		void Reset() { memset(this, 0, sizeof(*this)); }

		void Set(float _fMag, const A3DVECTOR3& _vDir, const A3DVECTOR3 _vPos, DWORD _dwTime, BYTE _bValid)
		{
			fMag = _fMag;
			vDir = _vDir;
			vPos = _vPos;
			dwTime = _dwTime;
			bValid = _bValid;
		}

		float fMag;
		A3DVECTOR3 vDir;
		A3DVECTOR3 vPos;
		DWORD dwTime;
		BYTE bValid;
	};

	struct BONETRACK
	{
		BONETRACK()
		{
			index = -1;
			bValid = 0;
			vLastPos.Clear();
			vCurVel.Clear();
		}

		A3DVECTOR3 vLastPos;//	Bone's last pos
		A3DVECTOR3 vCurVel;	//	Bone's current vel

		NODE MaxNode[MAXNODE_CNT];	//	MaxNode reserved int m_dwWindowTimeMs. [0]-FirstMax, [1]-SecondMax, ...
		AList2<NODE> aTrackedVels;	//	All vels reserved to calculate average in m_dwWindowTimeMs

		A3DVECTOR3 GetMaxVel() const
		{
			return MaxNode[0].fMag * MaxNode[0].vDir;
		}

		A3DVECTOR3 GetAvgVel() const
		{
			A3DVECTOR3 vAccuVel(0.0f);
			if (aTrackedVels.GetCount() < 1)
				return vAccuVel;

			ALISTPOSITION pos = aTrackedVels.GetHeadPosition();
			while (pos)
			{
				const NODE& node = aTrackedVels.GetNext(pos);
				vAccuVel += node.vDir * node.fMag;
			}
			return vAccuVel / aTrackedVels.GetCount();
		}

		short index;	//	Bone's index
		BYTE bValid;	//	Valid?
	};

public:		//	Constructor and Destructor

	TrackBoneVelocity(A3DDevice* pA3DDevice, A3DSkeleton* pSkeleton, DWORD dwWindowTimeMs)
		: m_pA3DDevice(pA3DDevice),
		m_pSkeleton(pSkeleton),
		m_dwWindowTimeMs(dwWindowTimeMs),
		m_dwPlayedTime(0),
		m_dwStopWorkTime(0),
		m_bWorking(false)
	{
		assert(m_pA3DDevice);
		assert(m_pSkeleton);
	}

	~TrackBoneVelocity()
	{
		ClearTrackBoneVelocity();
	}

public:		//	Operations

	void SetMode(CECModel::TRACKBONEVEL_MODE mode) { m_Mode = mode; }
	CECModel::TRACKBONEVEL_MODE GetMode() const { return m_Mode; }

	void SetWindowTime(DWORD dwWndTime) { m_dwWindowTimeMs = dwWndTime; }

	bool SetTrackBones(const abase::vector<AString>& vecBone, bool bClearUnUsedBones);
	int SetCurActiveTrackBone(const char* szName);

	bool IsWorking() const { return m_bWorking; }
	DWORD GetPlayedTime() const { return m_dwPlayedTime; }
	DWORD GetWorkStopTime() const { return m_dwStopWorkTime; }

	//	dwWorkDuration: 从此刻开始更新BoneVel的时间长度(ms)，过了这个时间停止更新。-1表示一直更新。
	void Start(DWORD dwWorkDuration)
	{
		if (!m_bWorking)
			m_bWorking = true;

		if (dwWorkDuration == -1)
			m_dwStopWorkTime = -1;
		else
			m_dwStopWorkTime = m_dwPlayedTime + dwWorkDuration;
	}
	void Stop() { m_bWorking = false; }

	const abase::vector<BONETRACK*>& GetAllTrackBoneInfo() const { return m_vecTraceBone; }

	void ClearTrackBoneVelocity()
	{
		m_dwPlayedTime = 0;
		m_dwStopWorkTime = -1;

		for (size_t i = 0; i < m_vecTraceBone.size(); ++i)
			delete m_vecTraceBone[i];
		m_vecTraceBone.clear();
	}

	//	Get track bone velocity
	const A3DBone* GetTrackBoneVelocity(A3DVECTOR3& vel, const char* szName) const
	{
		if (!szName)
			return NULL;

		for (size_t i = 0; i < m_vecTraceBone.size(); ++i)
		{
			BONETRACK* pBoneTrack = m_vecTraceBone[i];
			A3DBone* pBone = m_pSkeleton->GetBone(pBoneTrack->index);

			if (pBone && 0 == strcmp(szName, pBone->GetName()))
			{
				switch (m_Mode)
				{
				case CECModel::TRACKBONEVEL_MODE_AVERAGE:
					vel = pBoneTrack->GetAvgVel();
					return pBone;

				case CECModel::TRACKBONEVEL_MODE_MAX:
					vel = pBoneTrack->GetMaxVel();
					return pBone;
				}
			}
		}
		return NULL;
	}

	bool UpdateTrackBoneVelocity(DWORD dwTickDelta);

	bool Render(A3DViewport* pViewport);

private:	//	Attributes

	A3DDevice* m_pA3DDevice;
	A3DSkeleton* m_pSkeleton;

	abase::vector<BONETRACK*> m_vecTraceBone;

	DWORD	m_dwWindowTimeMs;	//	Window Time for Track
	DWORD	m_dwPlayedTime;		//	Current Played Time
	DWORD	m_dwStopWorkTime;	//	Time for Stop Work

	CECModel::TRACKBONEVEL_MODE m_Mode;
	bool	m_bWorking;
};

bool TrackBoneVelocity::SetTrackBones(const abase::vector<AString>& vecBone, bool bClearUnUsedBones)
{
	if (!vecBone.size())
	{
		ClearTrackBoneVelocity();
		return false;
	}

	abase::vector<int> vecBoneToAdd;
	vecBoneToAdd.reserve(vecBone.size());

	if (bClearUnUsedBones)
	{
		//	Clear UnUsed Bones: Mark All as invalid first
		for (size_t i = 0; i < m_vecTraceBone.size(); ++i)
			m_vecTraceBone[i]->bValid = 0;
	}

	for (size_t j = 0; j < vecBone.size(); ++j)
	{
		const AString& szNewBone = vecBone[j];
		int idxNewBone = -1;
		A3DBone* pBone = m_pSkeleton->GetBone(szNewBone, &idxNewBone);
		if (!pBone)
			continue;

		size_t i;
		for (i = 0; i < m_vecTraceBone.size(); ++i)
		{
			BONETRACK* pBoneTrack = m_vecTraceBone[i];
			int idxOldBone = pBoneTrack->index;
			if (idxOldBone == idxNewBone)
			{
				m_vecTraceBone[i]->bValid = 1;
				break; // Found this Bone: Mark as valid
			}
		}
		if (i == m_vecTraceBone.size())
		{
			//	Not Found This NewBone:
			vecBoneToAdd.push_back(idxNewBone);
		}
	}

	if (bClearUnUsedBones)
	{
		//	Clear InValid bones
		for (size_t i = 0; i < m_vecTraceBone.size(); )
		{
			if (!m_vecTraceBone[i]->bValid)
			{
#ifdef TRACKBONE_LOG
				char szDbg[100];
				int idxBone = m_vecTraceBone[i]->index;
				A3DBone* pBone = m_pSkeleton->GetBone(idxBone);
				sprintf(szDbg, "@%d: RemoveTrackBone %d, %s\n", a_GetTime(), idxBone,
					pBone ? pBone->GetName() : "(nil)");
				OutputDebugStringA(szDbg);
#endif
				delete m_vecTraceBone[i];
				m_vecTraceBone.erase(m_vecTraceBone.begin() + i);
			}
			else
				++i;
		}
	}

	//	Add New
	if (vecBoneToAdd.size() > 0)
	{
		m_vecTraceBone.reserve(m_vecTraceBone.size() + vecBoneToAdd.size());
		for (size_t i = 0; i < vecBoneToAdd.size(); ++i)
		{
			int idxBone = vecBoneToAdd[i];
			A3DBone* pBone = m_pSkeleton->GetBone(idxBone);
			if (!pBone)
			{
				assert(pBone);
				continue;
			}

#ifdef TRACKBONE_LOG
			char szDbg[100];
			sprintf(szDbg, "@%d: AddTrackBone %d, %s\n", a_GetTime(), idxBone,
				pBone ? pBone->GetName() : "(nil)");
			OutputDebugStringA(szDbg);
#endif
			BONETRACK* pBt = new BONETRACK;
			pBt->bValid = 1;
			pBt->index = idxBone;
			pBt->vLastPos = pBone->GetAbsoluteTM().GetRow(3);
			m_vecTraceBone.push_back(pBt);
		}
	}

	return true;
}

//#define TRACKBONE_LOG2

bool TrackBoneVelocity::UpdateTrackBoneVelocity(DWORD dwTickDelta)
{
	m_dwPlayedTime += dwTickDelta;
	if (m_dwPlayedTime > m_dwStopWorkTime)
		m_bWorking = false;

	if (!dwTickDelta) // 是否需要过滤掉太小的tick，避免速度计算误差过大？
		return false;

	for (size_t i = 0; i < m_vecTraceBone.size(); ++i)
	{
		BONETRACK* pBoneTrack = m_vecTraceBone[i];
		A3DBone* pBone = m_pSkeleton->GetBone(pBoneTrack->index);
		if (!pBone)
		{
			assert(pBone);
			continue;
		}

		A3DVECTOR3 vecCurPos = pBone->GetAbsoluteTM().GetRow(3);

		A3DVECTOR3 vecTrackBone = (vecCurPos - pBoneTrack->vLastPos) * (1000.f / dwTickDelta);
		pBoneTrack->vLastPos = vecCurPos;
		pBoneTrack->vCurVel = vecTrackBone;

		if (!m_bWorking)
			continue;

		float fMag = vecTrackBone.Normalize();

		switch (m_Mode)
		{
		case CECModel::TRACKBONEVEL_MODE_AVERAGE:
			{
				//	remove old
				ALISTPOSITION pos = pBoneTrack->aTrackedVels.GetHeadPosition();
				while (pos)
				{
					ALISTPOSITION posTemp = pos;
					NODE& node = pBoneTrack->aTrackedVels.GetNext(pos);
					if (m_dwPlayedTime > node.dwTime + m_dwWindowTimeMs)
						pBoneTrack->aTrackedVels.RemoveAt(posTemp);
					else
						break;
				}

				//	add new
				NODE node;
				node.Set(fMag, vecTrackBone, vecCurPos, m_dwPlayedTime, 1);
				pBoneTrack->aTrackedVels.AddTail(node);
			}
			break;

		case CECModel::TRACKBONEVEL_MODE_MAX:
			{
				//	Check valid first
#ifdef TRACKBONE_LOG2
				char szDbg[256];
				sprintf(szDbg, "=== UpdateTrackBoneVelocity: PlayedTime %d, InputMag %.3f\n", m_dwPlayedTime, fMag);
				OutputDebugStringA(szDbg);
#endif

				for (int i = 0; i < MAXNODE_CNT; ++i)
				{
					if (pBoneTrack->MaxNode[i].bValid && m_dwPlayedTime > pBoneTrack->MaxNode[i].dwTime + m_dwWindowTimeMs)
						pBoneTrack->MaxNode[i].Reset();
				}
				for (int i = 0, iNextValid = i + 1; i < MAXNODE_CNT - 1 && iNextValid < MAXNODE_CNT; ++i)
				{
					if (pBoneTrack->MaxNode[i].bValid)
					{
						++iNextValid;
						continue;
					}

					while (!pBoneTrack->MaxNode[iNextValid].bValid && ++iNextValid < MAXNODE_CNT) {}
					if (iNextValid >= MAXNODE_CNT)
						break;

					pBoneTrack->MaxNode[i] = pBoneTrack->MaxNode[iNextValid];
					pBoneTrack->MaxNode[iNextValid].Reset();

					++iNextValid;
				}

				//	Refresh Max & SecondMax & ThirdMax
				for (int i = 0; i < MAXNODE_CNT; ++i)
				{
					if (!pBoneTrack->MaxNode[i].bValid)
					{
						pBoneTrack->MaxNode[i].Set(fMag, vecTrackBone, vecCurPos, m_dwPlayedTime, 1);
						break;
					}
					else
					{
						if (fMag > pBoneTrack->MaxNode[i].fMag)
						{
							for (int j = MAXNODE_CNT - 1; j > i && j > 0; --j)
							{
								if (pBoneTrack->MaxNode[j - 1].bValid)
								{
									pBoneTrack->MaxNode[j] = pBoneTrack->MaxNode[j - 1];
								}
								else
									assert(!pBoneTrack->MaxNode[j].bValid);
							}

							pBoneTrack->MaxNode[i].Set(fMag, vecTrackBone, vecCurPos, m_dwPlayedTime, 1);
							break;
						}
					}
				}

#ifdef TRACKBONE_LOG2
				for (int i = 0; i < MAXNODE_CNT; ++i)
				{
					sprintf(szDbg, "    [%d]: Mag %.3f, Time %d, bValid %d\n",
						i, pBoneTrack->MaxNode[i].fMag, pBoneTrack->MaxNode[i].dwTime, pBoneTrack->MaxNode[i].bValid);
					OutputDebugStringA(szDbg);
				}
#endif
			}
			break;
		}
	}

	return true;
}

bool TrackBoneVelocity::Render(A3DViewport* pViewport)
{
	A3DWireCollector* pWire = m_pA3DDevice->GetA3DEngine()->GetA3DWireCollector();
	A3DFlatCollector* pFlat = m_pA3DDevice->GetA3DEngine()->GetA3DFlatCollector();
	DWORD dwColCurVel = 0xffff0000;
	DWORD dwColNodeRetVel = 0xffffff00;
	A3DAABB aabb;
	aabb.Extents.Set(0.03f, 0.03f, 0.03f);

	for (size_t i = 0; i < m_vecTraceBone.size(); ++i)
	{
		const BONETRACK* pBoneTrack = m_vecTraceBone[i];
		//	CurVel
		pWire->Add3DLine(pBoneTrack->vLastPos - pBoneTrack->vCurVel, pBoneTrack->vLastPos, dwColCurVel);
		aabb.Center = pBoneTrack->vLastPos;
		aabb.CompleteMinsMaxs();
		pFlat->AddAABB_3D(aabb, dwColCurVel);

		switch (m_Mode)
		{
		case CECModel::TRACKBONEVEL_MODE_AVERAGE:
			{
				A3DVECTOR3 vAvgVel = pBoneTrack->GetAvgVel();
				pWire->Add3DLine(pBoneTrack->vLastPos - vAvgVel, pBoneTrack->vLastPos, dwColNodeRetVel);
			}
			break;

		case CECModel::TRACKBONEVEL_MODE_MAX:
			{
				for (int j = 0; j < MAXNODE_CNT; ++j)
				{
					const NODE& node = pBoneTrack->MaxNode[j];
					pWire->Add3DLine(node.vPos - node.vDir * node.fMag, node.vPos, dwColNodeRetVel);
					aabb.Center = node.vPos;
					aabb.CompleteMinsMaxs();
					pFlat->AddAABB_3D(aabb, dwColNodeRetVel);
				}
			}
			break;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECModel::ChannelActNode
//	
///////////////////////////////////////////////////////////////////////////

CECModel::ChannelActNode::ChannelActNode() :
m_Rank(0),
m_pActive(0),
m_pActFlag(0),
m_dwFlagMode(0)
{
}

CECModel::ChannelActNode::~ChannelActNode()
{
	delete m_pActive;
	RemoveQueuedActs();
}

void CECModel::ChannelActNode::RemoveQueuedActs()
{
	ALISTPOSITION pos = m_QueuedActs.GetHeadPosition();
	
	while (pos)
		delete m_QueuedActs.GetNext(pos);
	
	m_QueuedActs.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECModel::DelayModeInfo
//	
///////////////////////////////////////////////////////////////////////////

void CECModel::DelayModeInfo::OnStartDelay(int nDelayTime)
{
	m_nDelayMode = CECModel::DM_PLAYSPEED_DELAY;
	m_nDelayTime = nDelayTime;
	m_nTimeElapsed = 0;
}

int CECModel::DelayModeInfo::OnUpdate(DWORD dwTickTime)
{
	if (m_nDelayMode == CECModel::DM_PLAYSPEED_JUMP)
		m_nDelayMode = CECModel::DM_PLAYSPEED_NORMAL;

	if (m_nDelayMode == CECModel::DM_PLAYSPEED_NORMAL)
		return m_nDelayMode;

	m_nTimeElapsed += dwTickTime;
	if (m_nTimeElapsed >= m_nDelayTime)
		m_nDelayMode = CECModel::DM_PLAYSPEED_JUMP;
	
	return m_nDelayMode;
}

int CECModel::DelayModeInfo::GetElapsedTime()
{
	return m_nTimeElapsed;
}

void CECModel::DelayModeInfo::OnPlayComAction()
{
	m_nDelayMode = CECModel::DM_PLAYSPEED_NORMAL;
	m_nDelayTime = 0;
	m_nTimeElapsed = 0;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECModel
//	
///////////////////////////////////////////////////////////////////////////


int CECModel::s_nCoGfxCount = 0;

int _ecm_total_count = 0;

bool CECModel::IsPhysXForbidden()
{
	if (s_bForbidPhys)
		return true;

#ifdef A3D_PHYSX
	APhysXScene* pScene = gGetAPhysXScene();
	if (0 != pScene)
	{
		if (pScene->GetAPhysXEngine()->IsPhysXDisbaled())
			return true;
	}
#endif

	return false; 
}

CECModel::CECModel()
{
	m_nId					= 0;
	m_bInheritParentId		= true;
	m_pA3DDevice			= NULL;
	m_pMapModel				= NULL;
	m_pA3DSkinModel			= NULL;
	m_pParentModel			= NULL;
	m_dwBindFlag			= 0;
	m_pECAnimNodeMan		= NULL;
	m_pTrackBoneVelocity	= NULL;
	m_bAutoUpdate			= true;
	m_bLoadSkinModel 		= false;
	m_fPlaySpeed			= 1.0f;

	m_bDisableCamShake		= false;
	m_bEnableCamBlur		= false;

	m_bCastShadow			= false;
	m_bSlowestUpdate		= false;
	m_iAABBType				= AABB_AUTOSEL;
	m_bCanAffectedByParent	= true;
	m_fInnerAlpha			= 1.0f;
	m_fOuterAlpha			= 1.0f;
	m_fAlpha				= 1.0f;
	m_InnerColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_OuterColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_Color.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_bAbsTrack				= false;
	m_fRootBoneScaleFactor	= 1.0f;
	m_fGfxScaleFactor		= 1.0f;
	m_bGfxScaleChanged		= false;
	m_bUpdateLightInfo		= true;
	m_bFirstActPlayed		= false;
	m_bGfxUseLod			= true;
	m_bCoGfxSleep			= false;
	m_nSfxPriority			= 0;
	m_nMainChannel			= 0;
	m_pModelPhysics			= NULL;
	m_bKeepCurPose			= false;
	m_bPhysEnabled			= true;
	m_bClothSimOpen			= false;
	m_fHitGroundActorVelThresh = 6.0f;
	m_dwInitTexFlag			= 0;

#ifdef GFX_EDITOR
	m_bIsDelayActive		= true;
#else
	m_bIsDelayActive		= false;
#endif
	m_nErrCode				= ECMERR_UNDEFINED;
	m_bFashionMode			= false;
	m_bCreatedByGfx			= false;
	m_pPhysBreakHandler		= NULL;
	m_pPhysHitGroundHandler = NULL;
	m_pBlurInfo				= NULL;
	m_bEnableScriptEvent	= true;
	m_bUseParentSpeed		= false;
	m_bEventUsePlaySpeed	= true;
	m_pPixelShader			= NULL;
	m_dwPSTickTime			= 0;
	m_pReplacePS			= NULL;
	m_pOverlapBlend			= NULL;
	m_ModelAABB.Clear();

	ZeroMemory( & m_WalkRunBlendInfo, sizeof( m_WalkRunBlendInfo ) );
	ZeroMemory( & m_WalkRunUpDownBlendInfo, sizeof( m_WalkRunUpDownBlendInfo ) );
	ZeroMemory(&m_RunTurnBlendInfo, sizeof(m_RunTurnBlendInfo));

	memset(m_EventMasks, EVENT_MASK_ALL, sizeof(m_EventMasks));
	m_dwChildRenderFlag	= 0;

	m_PhysXWindFlags = WF_DISABLE;

	m_bShown = true;
	m_bNeedUpdateWhenHidden = false;
	m_bHideByHiddenHH = false;
	m_bRealShown = true;

	_ecm_total_count++;
}

CECModel::~CECModel()
{
	Release();
	_ecm_total_count--;
}

bool CECModel::IsHumanBeingSkeleton(const A3DSkeleton* pSekelton)
{
	if (!pSekelton)
		return false;

	int idxPelvis = -1;
	A3DBone* pPelvisBone = pSekelton->GetBone("Bip01 Pelvis", &idxPelvis);
	if (!pPelvisBone) // 无Bip01 Pelvis骨骼：不是人形skeleton
		return false;

	const A3DMATRIX4& matPelvis = pPelvisBone->GetUpToRootTM();
	A3DVECTOR3 vX = matPelvis.GetRow(0);
	float fDot = DotProduct(vX, A3DVECTOR3(0, 1, 0));
	if (fDot > 0.8f)
	{
		//	 Pelvis的OriginUp为竖直：是人形skeleton
		return true;
	}

	return false;
}

//	Initialize model
bool CECModel::Load(
	const char* szModelFile,
	bool bLoadSkinModel,
	int iSkinFlag,
	bool bLoadChildECModel,
	bool bLoadAdditionalSkin,
	bool bIsCreateModelPhys,
	bool bLoadConvexHull)
{
	using namespace LuaBind;

	if (m_pMapModel)
	{
		assert(false);
		return false;
	}

	m_pMapModel = AfxGetECModelMan()->LoadModelData(szModelFile, bLoadAdditionalSkin, bLoadConvexHull);

	if (!m_pMapModel)
		return false;

	//	Initialize the event mask from static data
	memcpy(m_EventMasks, m_pMapModel->m_EventMasks, sizeof(m_EventMasks));

	CoGfxMap& GfxMap = m_pMapModel->m_CoGfxMap;
	CoGfxMap::iterator it = GfxMap.begin();

	for (; it != GfxMap.end(); ++it)
	{
		PGFX_INFO pSrc = it->second;
		PGFX_INFO pInfo = static_cast<PGFX_INFO>(EVENT_INFO::CloneFrom(pSrc->GetComAct(), *pSrc));

		if (!pInfo)
			continue;

		pInfo->LoadGfx();

		if (!pInfo->GetGfx())
		{
			delete pInfo;
			continue;
		}

		TransferEcmProperties(pInfo->GetGfx());

		++s_nCoGfxCount;
		m_CoGfxMap[it->first] = pInfo;
	}

	// 可以为空模型
	if (!m_pMapModel->m_strSkinModelPath.IsEmpty())
	{
		if (af_IsFileExist(m_pMapModel->m_strSkinModelPath))
		{
#ifdef _ANGELICA21
			m_pA3DSkinModel = AfxLoadA3DSkinModel(m_pMapModel->m_strSkinModelPath, iSkinFlag, m_dwInitTexFlag);
#else
			m_pA3DSkinModel = AfxLoadA3DSkinModel(m_pMapModel->m_strSkinModelPath, iSkinFlag);
#endif
		}
		else
			m_pA3DSkinModel = NULL;

		bool single = (m_pMapModel->m_strSkinModelPath.FindOneOf("/\\") == -1);

		// if load failed, suppose the m_strSkinModelPath is a single file name 
		// and try to find the file in the ecm's path
		if (!m_pA3DSkinModel && single) 
		{
			char szBuf[MAX_PATH];
			strcpy(szBuf, szModelFile);
			strcpy(ECMPathFindFileNameA(szBuf), m_pMapModel->m_strSkinModelPath);
#ifdef _ANGELICA21
			m_pA3DSkinModel = AfxLoadA3DSkinModel(szBuf, iSkinFlag, m_dwInitTexFlag);
#else
			m_pA3DSkinModel = AfxLoadA3DSkinModel(szBuf, iSkinFlag);
#endif
		}

		if (!m_pA3DSkinModel)
		{
			m_nErrCode = ECMERR_SMDLOADFAILED;
			Release();
			return false;
		}

		//	Judge Skeleton is Human bing
		if (m_pMapModel->m_nHumanbingSkeleton < 0)
		{
			A3DSkeleton* pSkeleton = m_pA3DSkinModel->GetSkeleton();
			if (pSkeleton)
			{
				pSkeleton->Update(0); // Tick(0) to init mat			
				m_pMapModel->m_nHumanbingSkeleton = IsHumanBeingSkeleton(pSkeleton) ? 1 : 0;
			}
		}

		//	We want to update child model's skin model in child CECModels
		//	so disable internal update child model by this flag
		//	but we need to update the whole AABB by ourself
		m_pA3DSkinModel->SetAutoUpdateChildFlag(false);
	}
	else
		m_pA3DSkinModel = NULL;

	m_pA3DDevice = AfxGetA3DDevice();
	ReloadPixelShader();
	m_bLoadSkinModel = (m_pA3DSkinModel != 0);
	m_bAutoUpdate = m_pMapModel->m_bAutoUpdate;

	UpdateBaseActTimeSpan();
	ShowCoGfx(true);

	if (g_pA3DConfig->GetFlagNewBoneScale())
		UpdateBoneScaleEx();
	else
		UpdateBoneScales();

#ifdef GFX_EDITOR

	if (bLoadAdditionalSkin && m_pA3DSkinModel)
	{
		AString strBase = "Models\\";

		for (size_t i = 0; i < m_pMapModel->m_AdditionalSkinLst.size(); i++)
		{
			AString strSkin = strBase + m_pMapModel->m_AdditionalSkinLst[i];

			if (m_pA3DSkinModel->AddSkinFile(strSkin, true) < 0)
				continue;
		}
	}

#endif

	ResetMaterialOrg();


	ResetMaterialScale();

	if (bLoadChildECModel)
		LoadChildModels();

	UpdateModelAABB();

	if (m_pMapModel->m_ScriptEnable[enumECMScriptModelLoaded])
	{
		abase::vector<CScriptValue> args, results;
		args.push_back((void *)this);
		m_pMapModel->m_ScriptMemTbl.Call(_script_func_name[enumECMScriptModelLoaded], args, results);
	}

	if (m_pA3DSkinModel)
	{
		if (!m_pMapModel->m_bChannelInfoInit)
		{
			m_pMapModel->m_bChannelInfoInit = true;
			A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();

			if (pSke)
			{
				for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
				{
					ActChannelInfo* pChannel = m_pMapModel->m_ChannelInfoArr[i];

					if (!pChannel)
						continue;
					
					for (size_t j = 0; j < pChannel->bone_names.size(); j++)
					{
						int index;
						A3DBone* pBone = pSke->GetBone(pChannel->bone_names[j], &index);
						
						if (!pBone)
							continue;
						
						A3DAnimJoint* pJoint = pBone->GetAnimJointPtr();
						pSke->GetJoint(pJoint->GetName(), &index);
						pChannel->joint_indices.push_back(index);						
					}
					
				}
			}
		}

		for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		{
			ActChannelInfo* pChannel = m_pMapModel->m_ChannelInfoArr[i];

			if (pChannel && pChannel->joint_indices.size())
				m_pA3DSkinModel->BuildActionChannel(i, pChannel->joint_indices.size(), pChannel->joint_indices.begin());
		}

		//	Create model physics system
		if (bIsCreateModelPhys)
			CreateModelPhysics();
	}

#ifdef _ANGELICA21
	if( ! m_pECAnimNodeMan )
	{
		m_pECAnimNodeMan = new CECAnimNodeMan();
		if( ! m_pECAnimNodeMan->Init( this ) )
		{
			delete m_pECAnimNodeMan;
			m_pECAnimNodeMan = 0;
			a_LogOutput(1, "%s, ECAnimNodeMan init failed for ecmodel: %s", __FUNCTION__, szModelFile);
			return false;
		}
	}
#endif

	m_bKeepCurPose = false;
	return true;
}

void CECModel::LoadChildModels()
{
	CECModelStaticData::ChildInfoArray& ChildInfoArray = m_pMapModel->m_ChildInfoArray;
	for (size_t i = 0; i < ChildInfoArray.size(); i++)
	{
		const CECModelStaticData::ChildInfo* pChild = ChildInfoArray[i];

		AddChildModel(
			pChild->m_strName,
			false,
			pChild->m_strHHName,
			pChild->m_strPath,
			pChild->m_strCCName);
	}
}

bool CECModel::LoadBoneScaleInfo(const char* szFile)
{
	if (!g_pA3DConfig->GetFlagNewBoneScale())
		return false;

	AFileImage file;

	if (!file.Open(szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
		return false;

	BoneScaleExArray arr;
	char* name = 0;
	bool ok = false;

	try
	{
		DWORD len;
		int num;
		file.Read(&num, sizeof(num), &len);

		if (len != sizeof(num))
			throw "";

		for (int i = 0; i < num; i++)
		{
			BoneScaleEx* p = new BoneScaleEx;
			file.Read(p, sizeof(BoneScaleEx), &len);

			if (len != sizeof(BoneScaleEx))
			{
				delete p;
				throw "";
			}

			arr.push_back(p);
		}

		file.Read(&num, sizeof(num), &len);

		if (len != sizeof(num))
			throw "";

		if( num )
		{
			name = new char[num+1];
			name[num] = 0;
			file.Read(name, num, &len);

			if ((int)len != num)
				throw "";
		}

		UpdateBoneScaleEx(arr, name);
		ok = true;
	}
	catch(const char*)
	{
	}

	for (size_t i = 0; i < arr.size(); i++)
		delete arr[i];

	delete[] name;
	file.Close();
	return ok;
}

//	Release model
void CECModel::Release()
{
	m_bKeepCurPose = false;
	if (m_pOverlapBlend)
	{
		delete m_pOverlapBlend;
		m_pOverlapBlend = NULL;
	}

	m_aComboModels.RemoveAll();

	for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		m_ChannelActs[i].Release();

	if ((m_dwBindFlag & ECM_BIND_DONT_DEL_BY_PARENT) != 0)
	{
		assert(false);

		if (m_pParentModel)
		{
			char log[1024];
			sprintf(log, "CECModel::Release, this = %x, %I64x, %s, parent = %x, %I64x, %s",
				this,
				m_nId,
				GetFilePath(),
				m_pParentModel,
				m_pParentModel->m_nId,
				m_pParentModel->GetFilePath());

			a_LogOutput(1, log);
			m_pParentModel->RemoveChildByPtr(this);
		}

		a_LogOutput(1, "CECModel::Release, ECM_BIND_DONT_DEL_BY_PARENT");
	}

	m_pParentModel = NULL;
	abase::vector<AString>* pTempDontDelByParentVec = NULL;

	// must release child models before release self
	for (ECModelMap::iterator child_it = m_ChildModels.begin(); child_it != m_ChildModels.end(); ++child_it)
	{
		CECModel* pChild = child_it->second;

		if ((pChild->m_dwBindFlag & ECM_BIND_DONT_DEL_BY_PARENT) == 0)
		{
			child_it->second->Release();
			delete child_it->second;
		}
		else
		{
			assert(false);

			char log[1024];
			sprintf(log, "CECModel::ReleaseChild, this = %x, %I64x, %s, child = %x, %I64x, %s",
				this,
				m_nId,
				GetFilePath(),
				pChild,
				pChild->m_nId,
				pChild->GetFilePath());

			a_LogOutput(1, log);

			if (!pTempDontDelByParentVec)
				pTempDontDelByParentVec = new abase::vector<AString>();

			pTempDontDelByParentVec->push_back(child_it->first);
		}
	}

	if (pTempDontDelByParentVec)
	{
		for (size_t i = 0; i < pTempDontDelByParentVec->size(); i++)
			RemoveChildModel((*pTempDontDelByParentVec)[i], false);

		delete pTempDontDelByParentVec;
	}

	m_ChildModels.clear();
	m_ChildPhyDelayMap.clear();

#ifdef A3D_PHYSX

	// must release A3DModelPhysics before A3DSkinModel
	// because A3DModelPhysics contains a lot of A3DSkinModel information
	// otherwise, there would be a problem when we use the A3DSkinModel information in the A3DModelPhysics::Release()

	A3DRELEASE(m_pModelPhysics);
	m_arrPhysXHitGroundInfo.clear();

#endif
	
#ifdef _ANGELICA21

	//	Anim nodes must be released before A3DSkinModel is released
	//	there are some resources should be released which may use A3DSkinModel
	A3DRELEASE(m_pECAnimNodeMan);
	ZeroMemory( & m_WalkRunBlendInfo, sizeof( m_WalkRunBlendInfo ) );
	ZeroMemory( & m_WalkRunUpDownBlendInfo, sizeof( m_WalkRunUpDownBlendInfo ) );
	ZeroMemory(&m_RunTurnBlendInfo, sizeof(m_RunTurnBlendInfo));

#endif

	if (m_pTrackBoneVelocity)
	{
		delete m_pTrackBoneVelocity;
		m_pTrackBoneVelocity = NULL;
	}

	if (m_bLoadSkinModel && m_pA3DSkinModel)
	{
		AfxReleaseA3DSkinModel(m_pA3DSkinModel);
		m_pA3DSkinModel = NULL;
		m_bLoadSkinModel = false;
	}

	for (CoGfxMap::iterator itGfx = m_CoGfxMap.begin(); itGfx != m_CoGfxMap.end(); ++itGfx)
	{
		if (itGfx->second)
		{
			delete itGfx->second;
			--s_nCoGfxCount;
		}
	}
	m_CoGfxMap.clear();

	// must after act being released
	ALISTPOSITION pos = m_FadeOutSfxLst.GetHeadPosition();
	while (pos) AfxReleaseSoundNonLoop(m_FadeOutSfxLst.GetNext(pos));
	m_FadeOutSfxLst.RemoveAll();

	if (m_pMapModel)
	{		
		AfxGetECModelMan()->Lock();
		CECModelStaticData* pData = AfxGetECModelMan()->ReleaseModel(m_pMapModel->m_strFilePath);
		AfxGetECModelMan()->Unlock();
		if (NULL != pData)
		{
			delete pData;
			pData = NULL;
		}		
		m_pMapModel = NULL;
	}

	m_bFashionMode = false;
	m_EquipSlotMap.clear();
	m_SlotEquipMap.clear();

	if (m_pBlurInfo)
	{
		delete m_pBlurInfo;
		m_pBlurInfo = NULL;
	}

	if (m_pPixelShader)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseShader(&m_pPixelShader);
		m_pPixelShader = NULL;
	}

	RemoveReplaceShader();

	ClearHideModelHHVec();
}

bool CECModel::SetSkinModel(A3DEngine* pEngine, const char* szFile)
{
	A3DSkinModel* pSkin = NULL;

	if (strlen(szFile))
	{
		pSkin = new A3DSkinModel;

		if (!pSkin->Init(pEngine) || !pSkin->Load(szFile))
		{
			delete pSkin;
			return false;
		}

		pSkin->SetAutoUpdateChildFlag(false);
	}

	if (!m_pMapModel)
	{
		m_pMapModel = new CECModelStaticData;
		m_pMapModel->m_strFilePath = "temp.ecm";
		AfxGetECModelMan()->Lock();
		AfxGetECModelMan()->AddModel(m_pMapModel->m_strFilePath, m_pMapModel);
		AfxGetECModelMan()->Unlock();
	}

	if (m_pA3DSkinModel)
	{
		m_pA3DSkinModel->Release();
		delete m_pA3DSkinModel;
	}

	m_pA3DSkinModel = pSkin;
	m_bLoadSkinModel = (m_pA3DSkinModel != 0);
	// Still get the relative path of the .smd file, but when saving the .ecm file, we would only save the filename of the .smd file
	m_pMapModel->m_strSkinModelPath = szFile;
	
	//	Create model physics system
	CreateModelPhysics();

	return true;
}

bool CECModel::SetSkinModel(A3DSkinModel* pSkinModel)
{
	assert(pSkinModel);

	if (!m_pMapModel)
	{
		static int nTempIndex = 1;
		m_pMapModel = new CECModelStaticData;
		m_pMapModel->m_strFilePath.Format("temp_smd_%d.ecm", nTempIndex++);
		AfxGetECModelMan()->Lock();
		AfxGetECModelMan()->AddModel(m_pMapModel->m_strFilePath, m_pMapModel);
		AfxGetECModelMan()->Unlock();
	}

	pSkinModel->SetAutoUpdateChildFlag(false);
	m_pA3DSkinModel = pSkinModel;
	m_bLoadSkinModel = (m_pA3DSkinModel != 0);

	//	Create model physics system
	CreateModelPhysics();

	return true;
}

void CECModel::RemoveSkinModel()
{
	m_pA3DSkinModel = NULL;
	m_bLoadSkinModel = false;
}

const A3DSHADER& CECModel::GetBlendMode() const { return m_pMapModel->m_BlendMode; }
void CECModel::SetBlendMode(const A3DSHADER& bld) { m_pMapModel->m_BlendMode = bld; }
A3DCOLOR CECModel::GetOrgColor() const { return m_pMapModel->m_OrgColor; }
void CECModel::SetOrgColor(A3DCOLOR cl) { m_pMapModel->m_OrgColor = cl; }
A3DCOLOR CECModel::GetExtraEmissive() const { return m_pMapModel->m_EmissiveColor; }
void CECModel::SetExtraEmissive(A3DCOLOR cl) { m_pMapModel->m_EmissiveColor = cl; }
float* CECModel::GetOuterData() { return m_pMapModel->m_OuterData; }
float CECModel::GetDefPlaySpeed() const { return m_pMapModel->m_fDefPlaySpeed; }
void CECModel::SetDefPlaySpeed(float fSpeed) { m_pMapModel->m_fDefPlaySpeed = fSpeed; }
const bool CECModel::CanCastShadow() const { return m_pMapModel->m_bCanCastShadow; }
void CECModel::SetCanCastShadow(bool b) { m_pMapModel->m_bCanCastShadow = b; }
const bool CECModel::CanRenderSkinModel() const { return m_pMapModel->m_bRenderSkinModel; }
void CECModel::SetRenderSkinModel(bool b) { m_pMapModel->m_bRenderSkinModel = b; }
bool CECModel::CanRenderEdge() const { return m_pMapModel->m_bRenderEdge; }
void CECModel::SetRenderEdge(bool b) { m_pMapModel->m_bRenderEdge = b; }

//	Create model physical sync
bool CECModel::CreateModelPhysics()
{
	if (!m_bPhysEnabled)
		return true;

#ifdef A3D_PHYSX

	A3DRELEASE(m_pModelPhysics);
	m_arrPhysXHitGroundInfo.clear();

	if (m_pA3DSkinModel && gGetAPhysXScene())
	{
		if (gGetAPhysXScene()->GetAPhysXEngine()->IsPhysXDisbaled())
			return false;

		m_pModelPhysics = new A3DModelPhysics;

#ifdef _ANGELICA21
		// Note here: always return true. due to some client logic requirements.
		if (m_pModelPhysics->Init(gGetAPhysXScene(), this))
		{
			if (m_pMapModel->m_pPhysSyncData)
			{
				m_pModelPhysics->CreateModelSync(m_pMapModel->m_pPhysSyncData);
			}
		}
		return true;
#else
		if (!m_pModelPhysics->Init(gGetAPhysXScene(), this))
		{
			delete m_pModelPhysics;
			m_pModelPhysics = 0;
			return false;
		}

		if (m_pMapModel->m_pPhysSyncData)
			return m_pModelPhysics->CreateModelSync(m_pMapModel->m_pPhysSyncData);
#endif
	}

#endif

	return true;
}

void CECModel::ResetMaterialOrg()
{
	if (m_pA3DSkinModel)
	{
		A3DSkinModel::LIGHTINFO LightInfo;
		memset(&LightInfo, 0, sizeof (LightInfo));
		const A3DLIGHTPARAM& lp = AfxGetLightparam();

		LightInfo.colAmbient	= m_pA3DDevice->GetAmbientValue();
		LightInfo.vLightDir		= lp.Direction;
		LightInfo.colDirDiff	= lp.Diffuse;
		LightInfo.colDirSpec	= lp.Specular;
		LightInfo.bPtLight		= false;

		m_pA3DSkinModel->SetLightInfo(LightInfo);
		m_pA3DSkinModel->SetSrcBlend(m_pMapModel->m_BlendMode.SrcBlend);
		m_pA3DSkinModel->SetDstBlend(m_pMapModel->m_BlendMode.DestBlend);
		m_pA3DSkinModel->EnableSpecular(true);
		m_pA3DSkinModel->SetExtraEmissive(m_pMapModel->m_EmissiveColor);
		m_pA3DSkinModel->Update(5);
	}
}

void CECModel::ResetMaterialScale()
{
	A3DCOLORVALUE cl(1.0f);
	if (m_pMapModel)
		cl = m_pMapModel->m_OrgColor;

	SetInnerAlpha(cl.a);
	cl.a = 1.0f;
	SetInnerColor(cl);
	
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		if (it->second->m_bCanAffectedByParent)
			it->second->ResetMaterialScale();
	}

	for (int iComboIdx = 0; iComboIdx < m_aComboModels.GetSize(); ++iComboIdx)
	{
		CECModel* pComboModel = m_aComboModels[iComboIdx];
		if (!pComboModel)
			continue;

		pComboModel->ResetMaterialScale();
	}
}

void CECModel::SetTransparent(float fTransparent)
{
	m_fOuterAlpha = (fTransparent == -1.0f ? 1.0f : 1.0f - fTransparent);
	m_fAlpha = m_fOuterAlpha * m_fInnerAlpha;
	
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetTransparent(GetTransparent());
	
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetTransparent(fTransparent);
	
	for (int iComboIdx = 0; iComboIdx < m_aComboModels.GetSize(); ++iComboIdx)
	{
		CECModel* pComboModel = m_aComboModels[iComboIdx];
		if (!pComboModel)
			continue;

		pComboModel->SetTransparent(fTransparent);
	}
}

void CECModel::SetInnerAlpha(float fInner)
{
	m_fInnerAlpha = fInner;
	m_fAlpha = m_fOuterAlpha * m_fInnerAlpha;

	float fTransparent = GetTransparent();

	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetTransparent(fTransparent);

	for (int iComboIdx = 0; iComboIdx < m_aComboModels.GetSize(); ++iComboIdx)
	{
		CECModel* pComboModel = m_aComboModels[iComboIdx];
		if (!pComboModel || !pComboModel->GetA3DSkinModel())
			continue;

		//pComboModel->GetA3DSkinModel()->SetTransparent(fTransparent);
		pComboModel->SetInnerAlpha(fInner);
	}
}

void CECModel::SetSrcBlend(A3DBLEND srcBlend)
{
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetSrcBlend(srcBlend);

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetSrcBlend(srcBlend);
}

void CECModel::SetDstBlend(A3DBLEND dstBlend)
{
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetDstBlend(dstBlend);

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetDstBlend(dstBlend);
}

void CECModel::SetColor(const A3DCOLORVALUE& cl)
{
	m_OuterColor = cl;
	m_Color = m_OuterColor * m_InnerColor;
	m_Color.Clamp();
	ApplyColor();

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetColor(cl);

	for (int iComboIdx = 0; iComboIdx < m_aComboModels.GetSize(); ++iComboIdx)
	{
		CECModel* pComboModel = m_aComboModels[iComboIdx];
		if (!pComboModel)
			continue;

		pComboModel->SetColor(cl);
	}
}

void CECModel::SetInnerColor(const A3DCOLORVALUE& cl)
{
	m_InnerColor = cl;
	m_Color = m_OuterColor * m_InnerColor;
	m_Color.Clamp();
	ApplyColor();
}

void CECModel::ApplyColor()
{
	static const A3DCOLORVALUE _scale(1.f, 1.f, 1.f, 1.f);

	if (m_pA3DSkinModel)
	{
		if (m_Color == _scale)
		{
			m_pA3DSkinModel->SetMaterialMethod(A3DSkinModel::MTL_ORIGINAL);
			m_pA3DSkinModel->SetMaterialScale(_scale);
		}
		else
		{
			m_pA3DSkinModel->SetMaterialMethod(A3DSkinModel::MTL_SCALED);
			m_pA3DSkinModel->SetMaterialScale(m_Color);
		}
	}
}

void CECModel::Show(bool bShow, bool bAffectChild/* = true*/, bool bUpdateWhenHidden)
{
	bool bOldRealShow = m_bRealShown;

	m_bShown = bShow;

	m_bRealShown = m_bShown && !m_bHideByHiddenHH;

	if (!m_bRealShown)
	{
		m_bNeedUpdateWhenHidden = bUpdateWhenHidden;
	}

	if (bOldRealShow != m_bRealShown)
		InnerSetShow(m_bRealShown, bAffectChild);
}

void CECModel::SetHideByHiddenHH(bool bSet, bool bAffectChild/* = true*/, bool bUpdateWhenHidden)
{
	bool bOldRealShow = m_bRealShown;

	m_bHideByHiddenHH = bSet;

	m_bRealShown = m_bShown && !m_bHideByHiddenHH;

	if (!m_bRealShown)
	{
		m_bNeedUpdateWhenHidden = bUpdateWhenHidden;
	}

	if (bOldRealShow != m_bRealShown)
		InnerSetShow(m_bRealShown, bAffectChild);
}

void CECModel::InnerSetShow(bool bShow, bool bAffectChild/* = true*/)
{
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->Hide(!bShow);

	ShowCoGfx(bShow);

	if (!bAffectChild)
		return;

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		if (it->second->m_bCanAffectedByParent)
			it->second->Show(bShow, true);
	}

	for (int iComboIdx = 0; iComboIdx < m_aComboModels.GetSize(); ++iComboIdx)
	{
		CECModel* pComboModel = m_aComboModels[iComboIdx];
		if (!pComboModel)
			continue;

		pComboModel->Show(bShow, true);
	}
}

void CECModel::ClearHideModelHHVec()
{
	if (m_HideModelHHMap.size())
	{
		m_HideModelHHMap.clear();

		for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		{
			CECModel* pChild = it->second;
			ASSERT( pChild );

			pChild->SetHideByHiddenHH(false);
		}
	}
}

void CECModel::AddHideModelHH(const abase::vector<AString>& vecHH)
{
	if (vecHH.size() == 0)
		return;

	for (size_t i = 0; i < vecHH.size(); ++i)
	{
		const AString& strHH = vecHH[i];
		if (strHH.GetLength() <= 0)
			continue;

		HideModelHHMap::iterator it = m_HideModelHHMap.find(strHH);
		if (it != m_HideModelHHMap.end())
		{
			++it->second;
		}
		else
			m_HideModelHHMap[strHH] = 1;
	}

	RefreshAllChildrenHiddenByHHFlag();
}

void CECModel::RemoveHideModelHH(const abase::vector<AString>& vecHH)
{
	if (vecHH.size() == 0)
		return;

	for (size_t i = 0; i < vecHH.size(); ++i)
	{
		HideModelHHMap::iterator it = m_HideModelHHMap.find(vecHH[i]);
		if (it != m_HideModelHHMap.end())
		{
			if (--it->second <= 0)
				m_HideModelHHMap.erase(it);
		}
	}

	RefreshAllChildrenHiddenByHHFlag();
}

void CECModel::RefreshAllChildrenHiddenByHHFlag()
{
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		CECModel* pChild = it->second;
		ASSERT( pChild );

		pChild->SetHideByHiddenHH(pChild->GetId() == m_nId && FindHideModelHH(pChild->GetHookName()));
	}
}

//	Scale specified bone
bool CECModel::ScaleBone(const char* szBone, int iScaleType, const A3DVECTOR3& vScale)
{
	if (!m_pA3DSkinModel)
		return false;

	A3DBone* pBone = m_pA3DSkinModel->GetSkeleton()->GetBone(szBone, NULL);

	if (!pBone)
		return false;

	switch (iScaleType)
	{
	case SCALE_NONE:	iScaleType = A3DBone::SCALE_NONE;	break;
	case SCALE_WHOLE:	iScaleType = A3DBone::SCALE_WHOLE;	break;
	case SCALE_LOCAL:	iScaleType = A3DBone::SCALE_LOCAL;	break;
	default:
		return false;
	}

	pBone->SetScaleType(iScaleType);
	pBone->SetScaleFactor(vScale);

	return true;
}

bool CECModel::ScaleBoneEx(const char* szBone, const A3DVECTOR3& vScale)
{
	if (!m_pA3DSkinModel)
		return false;

	A3DBone* pBone = m_pA3DSkinModel->GetSkeleton()->GetBone(szBone, NULL);

	if (!pBone)
		return false;

	pBone->SetLocalScale(vScale.x, vScale.y);
	pBone->SetWholeScale(vScale.z);
	return true;
}

//	Automatically calculate foot offset caused by bone scaling
//	Return:
//		0: succ
//		-1: A3DConfig FlagNewBoneScale not set
//		-2: Bones Size <= 0
//		-10: No Skeleton
int CECModel::CalcFootOffset(const char* szBaseBone)
{
	A3DSkeleton* pSkeleton = m_pA3DSkinModel ? m_pA3DSkinModel->GetSkeleton() : NULL;
	if (!pSkeleton)
		return -10;

#if defined(_ANGELICA21)

	return pSkeleton->CalcFootOffset(szBaseBone);

#else

	pSkeleton->CalcFootOffset(szBaseBone);
	return 0;

#endif
}

DWORD CECModel::GetComActTimeSpanByName(const AString& strName)
{
	CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;
	CombinedActMap::iterator it = ActionMap.find(strName);
	if (it == ActionMap.end()) return 0;
	return it->second->GetMinComActTimeSpan();
}

void CECModel::AddCombinedAction(A3DCombinedAction* pAct)
{
	CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;
	ActionMap[AString(pAct->GetName())] = pAct;
}

void CECModel::RemoveCombinedAction(const AString& strName)
{
	CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;
	CombinedActMap::iterator it = ActionMap.find(strName);
	if (it == ActionMap.end()) return;
	delete it->second;
	ActionMap.erase(strName);
}

bool CECModel::RenameCombinedAction(A3DCombinedAction* pAct, const AString& strNewName)
{
	CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;
	CombinedActMap::iterator it = ActionMap.find(strNewName);
	if (it != ActionMap.end()) return false;
	ActionMap.erase(pAct->GetName());
	ActionMap[strNewName] = pAct;
	pAct->SetName(strNewName);
	return true;
}

//	Get bone's scale information
bool CECModel::GetBoneScaleInfo(const char* szBone, int* piScaleType, A3DVECTOR3& vScale)
{
	if (!m_pA3DSkinModel)
		return false;

	A3DBone* pBone = m_pA3DSkinModel->GetSkeleton()->GetBone(szBone, NULL);

	if (!pBone)
		return false;

	int iScaleType;

	switch (pBone->GetScaleType())
	{
	case A3DBone::SCALE_NONE:	iScaleType = SCALE_NONE;	break;
	case A3DBone::SCALE_WHOLE:	iScaleType = SCALE_WHOLE;	break;
	case A3DBone::SCALE_LOCAL:	iScaleType = SCALE_LOCAL;	break;
	default:
		return false;
	}

	*piScaleType = iScaleType;
	vScale = pBone->GetScaleFactor();

	return true;
}

void CECModel::SetBoneTransFlag(const char* szBone, bool bFlag)
{
	if (!m_pA3DSkinModel)
		return;

	A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();

	if (!pSke)
		return;

	A3DBone* pBone = pSke->GetBone(szBone, NULL);

	if (pBone)
		pBone->SetTransMask(bFlag ? A3DBone::TRANS_ALL : 0);
}

//	Add a new skin
bool CECModel::AddSkinFile(const char* szSkinFile)
{
	if (!m_pA3DSkinModel)
		return false;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		return m_pModelPhysics->AddSkinFile(szSkinFile);
	}

#endif	//	A3D_PHYSX

	int iSkin = m_pA3DSkinModel->AddSkinFile(szSkinFile, true);
	if (iSkin < 0)
		return false;

	return true;
}

//	Add a new skin
bool CECModel::AddSkin(A3DSkin* pSkin, bool bAutoRem/* true */)
{
	if (!m_pA3DSkinModel)
		return false;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		return m_pModelPhysics->AddSkin(pSkin, bAutoRem);
	}

#endif	//	A3D_PHYSX

	int iSkin = m_pA3DSkinModel->AddSkin(pSkin, bAutoRem);
	if (iSkin < 0)
		return false;

	return true;
}

//	Replace a skin
bool CECModel::ReplaceSkinFile(int iIndex, const char* szSkinFile, bool bAutoRem/* true */)
{
	if (!m_pA3DSkinModel)
		return false;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		return m_pModelPhysics->ReplaceSkinFile(iIndex, szSkinFile, bAutoRem);
	}

#endif	//	A3D_PHYSX

	if (!m_pA3DSkinModel->ReplaceSkinFile(iIndex, szSkinFile, bAutoRem))
		return false;

	return true;
}

//	Replace a skin
bool CECModel::ReplaceSkin(int iIndex, A3DSkin* pSkin, bool bAutoRem/* true */)
{
	if (!m_pA3DSkinModel)
		return false;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		return m_pModelPhysics->ReplaceSkin(iIndex, pSkin, bAutoRem);
	}

#endif	//	A3D_PHYSX

	if (!m_pA3DSkinModel->ReplaceSkin(iIndex, pSkin, bAutoRem))
		return false;

	return true;
}

//	Remove a skin item, this operation release both skin anit skin item
void CECModel::RemoveSkinItem(int iItem)
{
	if (!m_pA3DSkinModel)
		return;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		m_pModelPhysics->RemoveSkinItem(iItem);
		return;
	}

#endif	//	A3D_PHYSX

	A3DSkin* pOldSkin = NULL;
	A3DSkinModel::SKIN* pSkinItem = m_pA3DSkinModel->GetSkinItem(iItem);

	if (pSkinItem->pA3DSkin)
		pOldSkin = pSkinItem->pA3DSkin;

	m_pA3DSkinModel->RemoveSkinItem(iItem);
}

//	Release all skins
void CECModel::ReleaseAllSkins()
{
	if (!m_pA3DSkinModel)
		return;

#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		m_pModelPhysics->ReleaseAllSkins();
		return;
	}

#endif	//	A3D_PHYSX

	m_pA3DSkinModel->ReleaseAllSkins();
}

//	Get skin
A3DSkin* CECModel::GetA3DSkin(int iIndex)
{
	return m_pA3DSkinModel ? m_pA3DSkinModel->GetA3DSkin(iIndex) : NULL;
}

//	Show / Hide skin
void CECModel::ShowSkin(int iIndex, bool bShow)
{
	if (m_pA3DSkinModel)
		m_pA3DSkinModel->ShowSkin(iIndex, bShow);
}

//	GetSkinShowFlag
bool CECModel::IsSkinShown(int iIndex)
{
	if (!m_pA3DSkinModel)
		return false;

	A3DSkinModel::SKIN* pSkinItem = m_pA3DSkinModel->GetSkinItem(iIndex);
	ASSERT(pSkinItem);
	return pSkinItem->bRender;
}

//	Open one cloth skin
bool CECModel::OpenClothSkin(int iSkinSlot, int iLinkType/* 0 */, int iClothSimDelayTime /*= 500*/, int iClothSimBlendTime /*= 800*/)
{
	if (IsPhysXForbidden())
		return false;

#ifdef A3D_PHYSX

	//	Note: do not support parameters backup yet
	if (m_pModelPhysics)
		return m_pModelPhysics->OpenClothSkin(iSkinSlot, iLinkType, iClothSimDelayTime, iClothSimBlendTime);

#endif	//	A3D_PHYSX

	return false;
}

//	Get skin number
int CECModel::GetSkinNum()
{
	return m_pA3DSkinModel ? m_pA3DSkinModel->GetSkinNum() : 0;
}

//	Open all clothes
bool CECModel::OpenAllClothes(int iLinkType/* 0 */, int iClothSimDelayTime /*= 500*/, int iClothSimBlendTime /*= 800*/)
{
	if (IsPhysXForbidden())
		return false;

#ifdef A3D_PHYSX

	if (!m_pA3DSkinModel || !m_pModelPhysics)
		return false;

	
	bool bIsCatched = false;
	A3DModelPhysSync* pSync = GetPhysSync();
	if (0 != pSync)
		bIsCatched = pSync->ClothesStateChangeEvent(true);

	if (!bIsCatched)
	{
		int iNumSlot = m_pA3DSkinModel->GetSkinNum();
		for (int i = 0; i < iNumSlot; ++i)
		{
			gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ECM_OpenClothSkin");
			m_pModelPhysics->OpenClothSkin(i, iLinkType, iClothSimDelayTime, iClothSimBlendTime);
			gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ECM_OpenClothSkin");
		}

		m_bClothSimOpen = true;
	}

	m_ClothSimPara.iLinkType = iLinkType;
	m_ClothSimPara.iDelayTime = iClothSimDelayTime;
	m_ClothSimPara.iBlendTime = iClothSimBlendTime;

#endif	//	A3D_PHYSX

	return true;
}

//	Close one cloth skin
void CECModel::CloseClothSkin(int iSkinSlot, bool bCompletely/* false */)
{
#ifdef A3D_PHYSX

	if (m_pModelPhysics)
		m_pModelPhysics->CloseClothSkin(iSkinSlot, bCompletely);

#endif	//	A3D_PHYSX
}

//	Close all clothes
void CECModel::CloseAllClothes(bool bCompletely/* false */)
{
#ifdef A3D_PHYSX

	if (!m_pA3DSkinModel || !m_pModelPhysics)
		return;

	int i, iNumSlot = m_pA3DSkinModel->GetSkinNum();

	for (i=0; i < iNumSlot; i++)
		m_pModelPhysics->CloseClothSkin(i, bCompletely);

	m_bClothSimOpen = false;

#endif	//	A3D_PHYSX
}

//	Open child model link
bool CECModel::OpenChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher)
{
	if (IsPhysXForbidden())
		return false;

#ifdef A3D_PHYSX

	if (!szParentHook || !szChildAttacher)
		return false;

	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return false;

	A3DModelPhysics* pPhys = pModel->GetModelPhysics();
	if (!pPhys->OpenPhysLinkAsChildModel(szParentHook, szChildAttacher))
		return false;

	//if (pPhys->GetPhysState() != A3DModelPhysSync::PHY_STATE_SIMULATE)
	//	pPhys->ChangePhysState(A3DModelPhysSync::PHY_STATE_SIMULATE);

	//	Why do the copy? because the next operation of clear() may make the passed in parameters invalid
	ChildModelLinkInfo linkInfo(szParentHook, szChildAttacher);
	pModel->m_PhysHookAttachLinks.clear();
	pModel->m_PhysHookAttachLinks.push_back(linkInfo);
	return true;
	
#endif

	return false;
}

//	Add child model link (Must first open one child model phys link, then use add, the child attacher must not be used)
bool CECModel::AddChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher)
{
	if (IsPhysXForbidden())
		return false;

#ifdef A3D_PHYSX

	if (!szParentHook || !szChildAttacher)
		return false;

	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return false;

	A3DModelPhysics* pPhys = pModel->GetModelPhysics();
	if (!pPhys->AddPhysLinkAsChildModel(szParentHook, szChildAttacher))
		return false;

	//if (pPhys->GetPhysState() != A3DModelPhysSync::PHY_STATE_SIMULATE)
	//	pPhys->ChangePhysState(A3DModelPhysSync::PHY_STATE_SIMULATE);

	ChildModelLinkInfo linkInfo(szParentHook, szChildAttacher);
	pModel->m_PhysHookAttachLinks.push_back(linkInfo);
	return true;

#endif

	return false;
}

//	Remove child model link (find the link between szParentHook and szChildAttacher, if there is one, remove it)
bool CECModel::RemoveChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher)
{
#ifdef A3D_PHYSX

	if (!szParentHook || !szChildAttacher)
		return false;

	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return false;

	A3DModelPhysics* pPhys = pModel->GetModelPhysics();
	if (!pPhys->RemovePhysLinkAsChildModel(szParentHook, szChildAttacher))
		return false;

	//if (pPhys->GetPhysState() != A3DModelPhysSync::PHY_STATE_ANIMATION)
	//	pPhys->ChangePhysState(A3DModelPhysSync::PHY_STATE_ANIMATION);

	ChildModelLinkInfoArray& aChildModelLinks = pModel->m_PhysHookAttachLinks;
	for (ChildModelLinkInfoArray::iterator ent_itr = aChildModelLinks.begin()
		; ent_itr != aChildModelLinks.end()
		; ++ent_itr)
	{
		if (ent_itr->m_strChildAttacher.Compare(szChildAttacher) != 0
			|| ent_itr->m_strParentHook.Compare(szParentHook) != 0)
			continue;

		aChildModelLinks.erase(ent_itr);
		break;		
	}

	return true;

#endif

	return false;

}

//	Close child model link
void CECModel::CloseChildModelLink(const char* szHanger)
{
#ifdef A3D_PHYSX

	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return;

	A3DModelPhysics* pPhys = pModel->GetModelPhysics();
	pPhys->ClosePhysLinkAsChildModel();

	//if (pPhys->GetPhysState() != A3DModelPhysSync::PHY_STATE_ANIMATION)
	//	pPhys->ChangePhysState(A3DModelPhysSync::PHY_STATE_ANIMATION);

	pModel->m_PhysHookAttachLinks.clear();

#endif
}

//	Child model change physical link
void CECModel::ChangeChildModelLink(const char* szHanger, const char* szNewHook)
{
#ifdef A3D_PHYSX

	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return;

	ChildModelLinkInfoArray& aLinkInfos = pModel->m_PhysHookAttachLinks;
	if (aLinkInfos.empty())
		return;

	for (ChildModelLinkInfoArray::iterator itr = aLinkInfos.begin()
		; itr != aLinkInfos.end()
		; ++itr)
	{
		pModel->GetModelPhysics()->ChangePhysLinkAsChildModel(itr->m_strChildAttacher, szNewHook);
		itr->m_strParentHook = szNewHook;
	}

#endif // A3D_PHYSX
}

//	Change child model physical state
void CECModel::ChangeChildModelPhysState(const char* szHanger, int iState)
{
#ifdef A3D_PHYSX

	if (iState != A3DModelPhysSync::PHY_STATE_ANIMATION
		&& iState != A3DModelPhysSync::PHY_STATE_PARTSIMULATE
		&& iState != A3DModelPhysSync::PHY_STATE_SIMULATE)
		return;

	CECModel* pModel = GetChildModel(szHanger);
	if (!pModel || !pModel->GetModelPhysics())
		return;

	pModel->GetModelPhysics()->ChangePhysState(iState);

	m_ChildPhyDelayMap.erase(pModel);
#endif
}

//	Change all child model physical state
void CECModel::ChangeChildModelPhysState(int iState)
{
#ifdef A3D_PHYSX

	for (ECModelMap::iterator itr = m_ChildModels.begin()
		; itr != m_ChildModels.end()
		; ++itr)
	{
		ChangeChildModelPhysState(itr->first, iState);
	}

#endif
}

//	Set ECModel's collision channel, call this after A3DModelPhysics::Bind() or after EnablePhysSystem()
bool CECModel::SetCollisionChannel(int iMethod, CECModel* pParent/* = NULL*/, bool bSetChildCollisionChannel /* = true */)
{
#ifdef A3D_PHYSX

	if (!m_pModelPhysics)
		return false;

	A3DModelPhysics* pParentPhysics = NULL;
	if (pParent)
		pParentPhysics = pParent->GetModelPhysics();

	bool bRes = m_pModelPhysics->SetModelCollisionChannel(iMethod, pParentPhysics);

	if (bSetChildCollisionChannel)
	{
		for (ECModelMap::iterator child_itr = m_ChildModels.begin()
			; child_itr != m_ChildModels.end()
			; ++child_itr)
		{
			CECModel* pChild = child_itr->second;
			ASSERT( pChild );

			pChild->SetCollisionChannel(COLLISION_CHANNEL_AS_PARENT, this);
		}
	}

	return bRes;

#else

	return false;

#endif

}

//	Break a joint (Break a joint if exist)
void CECModel::BreakJoint(const char * szJointName)
{
#ifdef A3D_PHYSX

	if (!m_pModelPhysics)
		return;

	m_pModelPhysics->BreakJoint(szJointName);

#endif
}

//	Break joint by bone
void CECModel::BreakJointByBone(const char * szBoneName)
{
#ifdef A3D_PHYSX

	if (!m_pModelPhysics)
		return;

	m_pModelPhysics->BreakJointByBone(szBoneName);

#endif
}

//	Apply force
bool CECModel::AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, float fForceMagnitude, float fMaxDist, int iPhysForceType, float fSweepRadius /*= 0.0f*/, float fMaxVelChangeHint /*= FLT_MAX*/)
{
#ifdef A3D_PHYSX

	if (!m_pModelPhysics)
		return false;

	return m_pModelPhysics->AddForce(vStart, vDir, fForceMagnitude, fMaxDist, iPhysForceType, fSweepRadius, fMaxVelChangeHint);

#endif

	return false;
}

bool CECModel::AddForceToBone(const char* szBoneName, const A3DVECTOR3& vForceDir, float fForceMagnitude, int iPhysForceType /*= PFT_FORCE*/, float fMaxVelChange /*= 20.0f*/)
{
#ifdef A3D_PHYSX

	if(!gGetAPhysXScene()) return false;

	if(m_pA3DSkinModel && m_pA3DSkinModel->GetSkeleton())
	{
		A3DBone* pBone = m_pA3DSkinModel->GetSkeleton()->GetBone(szBoneName, NULL);
		A3DModelPhysSync* pSync = m_pModelPhysics ? m_pModelPhysics->GetModelSync() : NULL;
		if(pBone && pSync)
		{
			NxVec3 scale3D(1.0f);
			NxActor* pActor = m_pModelPhysics->GetModelSync()->GetNxActor(pBone, &scale3D);
			if(pActor)
			{
				NxForceMode nxForceMode = A3DModelPhysSync::TranslateForceMode(iPhysForceType);

				bool bMomentArmOnly = false;
				if (NX_VELOCITY_CHANGE == nxForceMode || NX_SMOOTH_VELOCITY_CHANGE == nxForceMode || NX_ACCELERATION == nxForceMode)
					bMomentArmOnly = true;

				float fScale = pSync->GetScaleForSoftKeyFrame(scale3D, bMomentArmOnly);
				fForceMagnitude *= fScale;
				fMaxVelChange *= fScale;
				gGetAPhysXScene()->AddLimitForce(pActor, APhysXConverter::A2N_Vector3(fForceMagnitude * vForceDir), nxForceMode, fMaxVelChange);
				return true;
			}
		}
	}

#endif

	return false;
}

bool CECModel::AddTorque(int iRotateAxis, float fTorqueMagnitude, DWORD dwDuration /* = 0 */, int iPhysTorqueType /* = PFT_VELOCITY_CHANGE */)
{
#ifdef A3D_PHYSX

	ROT_AXIS iAxis = (ROT_AXIS)iRotateAxis;
	PHYFORCE_TYPE iPTT = (PHYFORCE_TYPE)iPhysTorqueType;

	if (0 == dwDuration)
		return InnerAddTorque(iAxis, fTorqueMagnitude, iPTT);

	m_PhyTorqueInfo.bValid = 1;
	m_PhyTorqueInfo.iAxis = iAxis;
	m_PhyTorqueInfo.iForceType = iPTT;
	m_PhyTorqueInfo.fMagnitude = fTorqueMagnitude;
	m_PhyTorqueInfo.dwDuration = dwDuration;
	m_PhyTorqueInfo.dwPlayedTime = 0;
	return true;

#endif
	return false;
}

bool CECModel::InnerAddTorque(ROT_AXIS iRotateAxis, float fTorqueMagnitude, PHYFORCE_TYPE iPhysTorqueType)
{
	if (!m_pModelPhysics)
		return false;

#ifdef A3D_PHYSX
	A3DModelPhysSync* pAMPS = m_pModelPhysics->GetModelSync();
	if (!pAMPS)
		return false;

	return pAMPS->AddTorque(iRotateAxis, fTorqueMagnitude, iPhysTorqueType);
#else
	return false;
#endif
}

bool CECModel::TickPhyTorque(DWORD dwDeltaTime)
{
	if (!m_PhyTorqueInfo.bValid)
		return false;

	m_PhyTorqueInfo.dwPlayedTime += dwDeltaTime;
	if (m_PhyTorqueInfo.dwPlayedTime > m_PhyTorqueInfo.dwDuration)
	{
		m_PhyTorqueInfo.bValid = 0;
		return false;
	}

	return InnerAddTorque(m_PhyTorqueInfo.iAxis, m_PhyTorqueInfo.fMagnitude, m_PhyTorqueInfo.iForceType);
}

void CECModel::SetPhysXWindAcceleration(const WindInfo& info, bool bRecur)
{
	m_PhysXWindInfo = info;

	if (bRecur)
	{
		ECModelMap::iterator it = m_ChildModels.begin();
		ECModelMap::iterator itEnd = m_ChildModels.end();
		for (; it != itEnd; ++it)
		{
			CECModel* pChild = it->second;
			ASSERT(pChild);
			pChild->SetPhysXWindAcceleration(info, true);
		}
	}
}

void CECModel::EnablePhysXWind(int flags, bool bRecur)
{
	m_PhysXWindFlags = flags;
	if (bRecur)
	{
		ECModelMap::iterator it = m_ChildModels.begin();
		ECModelMap::iterator itEnd = m_ChildModels.end();
		for (; it != itEnd; ++it)
		{
			CECModel* pChild = it->second;
			ASSERT(pChild);
			pChild->EnablePhysXWind(flags, true);
		}
	}
}

bool CECModel::IsPhysXWindEnabled(int flags) const
{
	if ((WF_DISABLE == flags) || (WF_ALL == flags))
	{
		if (m_PhysXWindFlags == flags)
			return true;
		return false;
	}

	if (m_PhysXWindFlags & flags)
		return true;
	return false;
}

//	Creator of EC_Model could set a break handler here
//	which could be called back when PhysX makes a break to this model.
void CECModel::SetPhysBreakHandler(ON_PHYS_BREAK pPhysBreakHandler)
{
	m_pPhysBreakHandler = pPhysBreakHandler;
}

void CECModel::SetPhysHitGroundHandler(ON_PHYS_HITGROUND pPhysHitGroundHandler)
{
	m_pPhysHitGroundHandler = pPhysHitGroundHandler;
}

//	This method is called back by the lower level objects in (A3DModelPhysSync::OnBreak)
//	User should not call this method directly
void CECModel::OnPhysBreak(const PhysBreakInfo& physBreakInfo)
{
#ifdef A3D_PHYSX

	A3DVECTOR3 vBreakPosOffset(0.0f);
	if(m_pA3DSkinModel)
	{
		vBreakPosOffset = physBreakInfo.mBreakPos - m_pA3DSkinModel->GetPos();
	}

	//	回调脚本
	if (m_pMapModel)
		m_pMapModel->OnScriptPhysBreak(this, vBreakPosOffset.x, vBreakPosOffset.y, vBreakPosOffset.z);

	//	回调Handler
	if (m_pPhysBreakHandler)
		(m_pPhysBreakHandler)(this);

#endif
}

void CECModel::OnPhysHitGround(const ECMPhysXHitGroundInfo& ecmpHitGroundInfo)
{

#ifdef A3D_PHYSX
	
	if(ecmpHitGroundInfo.mHitActor)
	{
		NxActor* pHitActor = ecmpHitGroundInfo.mHitActor;
		float fV = pHitActor->getLinearVelocity().magnitude();
		if(fV > m_fHitGroundActorVelThresh)
		{
			for(int i=0; i<m_arrPhysXHitGroundInfo.size(); ++i)
			{
				if(pHitActor == m_arrPhysXHitGroundInfo[i].mHitActor)
					return;
			}

			m_arrPhysXHitGroundInfo.push_back(ecmpHitGroundInfo);
		}

	}


#endif

}

//	Enable physical system
bool CECModel::EnablePhysSystem(bool bHardLink, bool bRecur /*= false*/, bool bOpenCloth /* = true*/)
{
	if (IsPhysXForbidden())
		return false;

	if (m_bKeepCurPose)
		return false;

#ifdef A3D_PHYSX

	m_bPhysEnabled = true;

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ECM_CreateModelPhysX");
	//	Create model physics system
	if (!CreateModelPhysics())
	{
		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ECM_CreateModelPhysX");
		return false;
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ECM_CreateModelPhysX");

	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ECM_OpenClothes");
	if (bOpenCloth)
	{
		//	Open all clothes
		int iClothLinkType = (bHardLink)?2:1;		// how about the saving default type?
		OpenAllClothes(iClothLinkType);
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ECM_OpenClothes");

	if (!bRecur)
		return true;

	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ECM_EnableChildPhysX");

	//	When recur this function on child models
	for (ECModelMap::iterator child_itr = m_ChildModels.begin()
		; child_itr != m_ChildModels.end()
		; ++child_itr)
	{
		CECModel* pModel = child_itr->second;
		ASSERT( pModel );

		if (!pModel->EnablePhysSystem(bHardLink, bRecur, bOpenCloth))
			continue;

		const ChildModelLinkInfoArray& aLinkInfos = pModel->m_PhysHookAttachLinks;
		if (!TryOpenChildModelLinks(child_itr->first, aLinkInfos))
			continue;
	}

	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ECM_EnableChildPhysX");

	return true;

#else

	return false;

#endif	//	A3D_PHYSX
}

//	Try to open all child model links
bool CECModel::TryOpenChildModelLinks(const char* szHanger, const ChildModelLinkInfoArray& aChildModelLinks)
{
	if (s_bForbidPhys)
		return false;

#ifdef A3D_PHYSX

	ChildModelLinkInfoArray::const_iterator itr = aChildModelLinks.begin();
	if (itr == aChildModelLinks.end())
		return false;

	if (!OpenChildModelLink(szHanger, itr->m_strParentHook, itr->m_strChildAttacher))
		return false;

	for ( ++itr
		; itr != aChildModelLinks.end()
		; ++itr)
	{
		if (!AddChildModelLink(szHanger, itr->m_strParentHook, itr->m_strChildAttacher))
			return false;
	}

	return true;

#endif

	return false;

}

//	Disable physical system
void CECModel::DisablePhysSystem()
{
#ifdef A3D_PHYSX

	//	Close all clothes
	CloseAllClothes(true);
	A3DRELEASE(m_pModelPhysics);
	m_arrPhysXHitGroundInfo.clear();
	m_bPhysEnabled = false;

#endif
}

bool  CECModel::DisablePhysSystemEx()
{
	bool bRtn = false;
#ifdef A3D_PHYSX

	A3DModelPhysSync* pSync = GetPhysSync();
	if (0 != pSync)
	{
		if (A3DModelPhysSync::PHY_STATE_SIMULATE == pSync->GetPhysState())
		{
			if (A3DModelPhysSync::RB_SIMMODE_RAGDOLL == pSync->GetRBSimMode())
			{
				// stop all actions
				for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; ++i)
					StopChannelAction(i, true);
				if (0 != m_pA3DSkinModel)
					m_pA3DSkinModel->StopAllActions();

				bRtn = true;
				m_bKeepCurPose = true;
			}
		}
	}
	DisablePhysSystem();

#endif
	return bRtn;
}

A3DMATRIX4 CECModel::GetGfxBindHookTransMat(GFX_INFO* pInfo, bool& bIsHookMat)
{
	ASSERT(pInfo);
	bIsHookMat = false;
	if (!pInfo || !m_pA3DSkinModel)
		return GetAbsTM();
	else
	if (pInfo->GetHookName().IsEmpty())
	{
		A3DMATRIX4 mat;
		if (!pInfo->IsRotWithModel())
		{
			mat.Identity();
			mat.SetRow(3, GetAbsTM().GetRow(3));
		}
		else
		{
			mat = GetAbsTM();
		}
		return mat;
	}
	
	if (pInfo->IsUseECMHook()) 
	{
		CECModelHook* pECMHook = GetECMHook(pInfo->GetHookName());
		if (pECMHook)
			return pECMHook->GetRelativeMatrix() * GetAbsTM();
	}
	
	// else 笑傲需要2者都要找
	A3DSkeletonHook* pSkeletonHook = m_pA3DSkinModel->GetSkeletonHook(pInfo->GetHookName(), true);
	if (pSkeletonHook)
	{
		bIsHookMat = true;
		return pSkeletonHook->GetAbsoluteTM();
	}
	
	return GetAbsTM();
}

static bool _VectorCompare(const A3DVECTOR3& v1, const A3DVECTOR3& v2)
{
	return fabs(v1.x - v2.x) >= 0.0001
		|| fabs(v1.y - v2.y) >= 0.0001
		|| fabs(v1.z - v2.z) >= 0.0001;
}

//-----------------------------------------------------------------------
static A3DQUATERNION Slerp (float fT, const A3DQUATERNION& rkP,
							  const A3DQUATERNION& rkQ, bool shortestPath)
{
	float fCos = DotProduct(rkP, rkQ);
	A3DQUATERNION rkT;
	
	// Do we need to invert rotation?
	if (fCos < 0.0f && shortestPath)
	{
		fCos = -fCos;
		rkT = rkQ * -1.f;
	}
	else
	{
		rkT = rkQ;
	}
	
	if (fabs(fCos) < 1 - 1e-03)
	{
		// Standard case (slerp)
		float fSin = (float)sqrt(1.f - (fCos * fCos));
		float fAngle = (float)atan2(fSin, fCos);
		float fInvSin = 1.0f / fSin;
		float fCoeff0 = (float)sin((1.0f - fT) * fAngle) * fInvSin;
		float fCoeff1 = (float)sin(fT * fAngle) * fInvSin;
		return fCoeff0 * rkP + fCoeff1 * rkT;
	}
	else
	{
		// There are two situations:
		// 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
		//    interpolation safely.
		// 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
		//    are an infinite number of possibilities interpolation. but we haven't
		//    have method to fix this case, so just use linear interpolation here.
		A3DQUATERNION t = (1.0f - fT) * rkP + fT * rkT;
		// taking the complement requires renormalisation
		t.Normalize();
		return t;
	}
}

void _DelayUpdateGfxParentTM(A3DGFXEx* pGfx, const A3DMATRIX4& dstTM, float fPortion)
{
	A3DVECTOR3 vDstScale, vDstPos;
	A3DQUATERNION vDstOrient;
	a3d_DecomposeMatrix(dstTM, vDstScale, vDstOrient, vDstPos);	
	if (_VectorCompare(pGfx->GetPos(), vDstPos))
	{
		A3DVECTOR3 vNewPos = Interp_Position(pGfx->GetPos(), vDstPos, fPortion);
		A3DQUATERNION qNewDir = Slerp(fPortion, pGfx->GetDir(), vDstOrient, true);

		A3DMATRIX4 matNew;
		qNewDir.ConvertToMatrix(matNew);
		A3DQUATERNION qTestConvertBackDir(matNew);
		matNew.SetRow(3, vNewPos);
		pGfx->SetScale(a_Max(vDstScale.x, vDstScale.y, vDstScale.z));
		pGfx->SetParentTM(matNew);
	}
}

//	Set parent model
void CECModel::SetParentModel(CECModel* pParent)
{
	m_pParentModel = pParent;
}

DWORD CECModel::GetActionTickTime(DWORD dwUpdateTime, bool bNoAnim) const
{
	return bNoAnim ? 0 : (DWORD)(dwUpdateTime * GetSkinModelTRCtrl().GetTickSpeed());
}

//	dwUpdateTime 是已经考虑过CECModel的PlaySpeed的Time
//	dwDeltaTime 是游戏中实际经过的时间（不考虑PlaySpeed的Time）
//	返回的EventTickTime，是依照IsEventUsePlaySpeed()状态所选择的TickTime
DWORD CECModel::GetEventTickTime(DWORD dwUpdateTime, DWORD dwDeltaTime) const
{
	return IsEventUsePlaySpeed() ? dwUpdateTime : dwDeltaTime;
}

//	GetAccuPlaySpeed 返回从父模型开始直到当前模型所有PlaySpeed的累积影响
float CECModel::GetAccuPlaySpeed() const
{
	if (!GetParentModel())
		return GetPlaySpeed();

	return GetPlaySpeed() * GetParentModel()->GetAccuPlaySpeed();
}

//	传入：dwDeltaTime是真实传入的时间
//	传出：pUpdateTime是乘以播放速度之后的更新时间
//	传出：pActionTickTime是UpdateTime乘以SkinModelTRCtrl的TickSpeed或者0，取决于bNoAnim标志
//	传出：pEventTickTime是dwUpdateTime或者dwDeltaTime，取决于事件是否使用PlaySpeed
void CECModel::GetTickTimes(DWORD dwDeltaTime, bool bNoAnim, DWORD* pUpdateTime, DWORD* pActionTickTime, DWORD* pEventTickTime)
{
	//	dwDeltaTime 始终被认为是外部传入的准确时间
	//	如果当前CECModel是子模型，且希望受到父模型PlaySpeed的影响，则在此计算所有累计的PlaySpeed
	//	如果当前CECModel没有父模型，或者有父模型而不希望受到父模型的PlaySpeed的影响，也同样在此处理
	const float fCurPlaySpeed = IsUsingParentSpeed() ? GetAccuPlaySpeed() : GetPlaySpeed();
	DWORD dwUpdateTime = (DWORD)(dwDeltaTime * fCurPlaySpeed);

	/********************************************************************/
	//	这段逻辑笑傲和圣斗士应该都没有用过，留在这里，为了保证以后2.0的项目迁移的时候，如果有项目用了的，可以正常运行
	//	但是不建议再继续使用此功能
	//	UpdateTime可能被Delay逻辑所修改
	int nDelayMode = m_kECMDelayInfo.OnUpdate(dwDeltaTime);
	if (nDelayMode == DM_PLAYSPEED_DELAY)
		dwUpdateTime = 0;
	else if (nDelayMode == DM_PLAYSPEED_JUMP)
		dwUpdateTime += m_kECMDelayInfo.GetElapsedTime();
	/********************************************************************/

	//	后续的ActionUpdateTime和EventUpdateTime都以之前修改好的UpdateTime为基础进行计算
	const DWORD dwActionUpdateTime = GetActionTickTime(dwUpdateTime, bNoAnim);
	const DWORD dwEventUpdateTime = GetEventTickTime(dwUpdateTime, dwDeltaTime);

	*pUpdateTime = dwUpdateTime;
	*pActionTickTime = dwActionUpdateTime;
	*pEventTickTime = dwEventUpdateTime;
}

//	Tick routine
bool CECModel::Tick(DWORD dwDeltaTime, bool bNoAnim)
{
	g_nECMDeadlockFlag = 0;
	AGPA_RECORD_FUNCTION_TIME;
#ifdef FUNCTION_ANALYSER
	ATTACH_FUNCTION_ANALYSER(true, 5, 0, m_pMapModel->m_strFilePath);
#endif

#ifdef GFX_STAT

	__int64 _start = ACounter::GetMicroSecondNow();

#endif

	DWORD dwUpdateTime, dwActionUpdateTime, dwEventUpdateTime;
	GetTickTimes(dwDeltaTime, bNoAnim, &dwUpdateTime, &dwActionUpdateTime, &dwEventUpdateTime);

	//	Update all channel combined actions
	g_nECMDeadlockFlag = 1;
	UpdateChannelActs(dwActionUpdateTime);

	//	Tick PhyTorque
	g_nECMDeadlockFlag = 2;
	TickPhyTorque(dwUpdateTime);

	//	Tick skin model or model physics when in physics state
	g_nECMDeadlockFlag = 3;
	TickSkinModel(dwActionUpdateTime);

	m_dwPSTickTime += dwDeltaTime;

	//	Update fade out sfxs
	g_nECMDeadlockFlag = 4;
	UpdateFadeOutSfxLst(dwDeltaTime);

	//	Tick all shown child models
	g_nECMDeadlockFlag = 5;
	TickShownChildModels(dwDeltaTime, bNoAnim);

#if !defined(_ANGELICA21) || !defined(A3D_PHYSX)
	//	对2.2项目保持原有流程
	bool bTickRelyOnPhysics = true;
#else
	//	没有物理的时候直接在Tick里TickPostPhysics
	bool bTickRelyOnPhysics = !m_bPhysEnabled || (GetModelPhysics() == NULL);
#endif

	g_nECMDeadlockFlag = 6;
	if (bTickRelyOnPhysics)
		TickRelyOnPhysics(dwUpdateTime, dwActionUpdateTime, dwEventUpdateTime);

	//	After skin model and all child model updated
	//	Update current model and all child models' aabb
	g_nECMDeadlockFlag = 7;
	UpdateModelAABB();

	g_nECMDeadlockFlag = 8;
	if (m_pTrackBoneVelocity)
		m_pTrackBoneVelocity->UpdateTrackBoneVelocity(dwDeltaTime);

	g_nECMDeadlockFlag = 9;
#if defined(_ANGELICA21) && defined(UNICODE)
	m_LightTransInfo.Update(dwDeltaTime, GetPos());
#endif

#ifdef GFX_STAT

	{
		float _tick_time = (ACounter::GetMicroSecondNow() - _start) * 0.001f;
		_ecm_total_tick_time += _tick_time;
	}

#endif


	//  pyp: 2012.12.14, move from render to here. update once per frame
	//	dyx: 2009.4.1, For the models that only some parts of them have convex hulls,
	//		CH aabb doesn't works well. A3DSkinModel::UpdateHitBoxes use new way 
	//		to calculate model's aabb, so don't use HasCHAABB() anymore. 
	if (HasCHAABB())
	{
		m_aabbUsedToCullInRender.Center = GetCHAABB().Center * GetAbsTM();
		m_aabbUsedToCullInRender.Extents = GetCHAABB().Extents;
		m_aabbUsedToCullInRender.Extents.x = m_aabbUsedToCullInRender.Extents.y
			= m_aabbUsedToCullInRender.Extents.z 
			= max(max(m_aabbUsedToCullInRender.Extents.x, m_aabbUsedToCullInRender.Extents.y), m_aabbUsedToCullInRender.Extents.z);
		m_aabbUsedToCullInRender.CompleteMinsMaxs();

		m_aabbUsedToCullInRender.Merge(m_ModelAABB);
	}
	else
		m_aabbUsedToCullInRender = m_ModelAABB;

	// clear all hit ground info created in last tick...
	m_arrPhysXHitGroundInfo.clear();
	g_nECMDeadlockFlag = 0;
	return true;
}

bool CECModel::TickRelyOnPhysics(DWORD dwUpdateTime, DWORD dwActionUpdateTime, DWORD dwEventUpdateTime)
{
	//	Tick motion blur
	if (m_pBlurInfo)
		TickMotionBlur(dwUpdateTime);

	//	Update all channel combined action events
	UpdateChannelEvents(dwActionUpdateTime, dwEventUpdateTime);

	//	CoGfx's tick use the UpdateTime (which is with playspeed considered)
	TickCoGfx(dwEventUpdateTime);

	return true;
}

//	Tick ECModel combined actions
void CECModel::UpdateChannelActs(DWORD dwUpdateTime)
{
	if (dwUpdateTime == 0)
		return;
	
	for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
	{
		ChannelAct& ca = m_ChannelActs[i];
		ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();
		
		while (pos)
		{
			ALISTPOSITION posCur = pos;
			ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);
			bool bActFinished = false;
			
			if (pNode->m_dwFlagMode == COMACT_FLAG_MODE_ONCE_IGNOREGFX || pNode->m_dwFlagMode == COMACT_FLAG_MODE_ONCE_MULTIIGNOREGFX)
				bActFinished = pNode->m_pActive->IsActionStopped();
			else
				bActFinished = pNode->m_pActive->IsAllFinished();
			
			if (bActFinished)
			{
				if (pNode->m_dwFlagMode != COMACT_FLAG_MODE_NONE && pNode->m_pActFlag)
				{
					*pNode->m_pActFlag = true;
					
					if (pNode->m_dwFlagMode != COMACT_FLAG_MODE_ONCE_MULTIIGNOREGFX)
						pNode->m_dwFlagMode = COMACT_FLAG_MODE_NONE;
					
					pNode->m_pActFlag = NULL;
				}
				
				if (pNode->m_QueuedActs.GetCount())
				{
					A3DCombActDynData* pNext = pNode->m_QueuedActs.RemoveHead();
					
					if (pNext->GetStopPrevAct())
					{
						pNode->m_pActive->Stop(true);
						StopChildrenAct();
					}
					else
						pNode->m_pActive->Stop(false);
					
					// script
					m_pMapModel->OnScriptPlayAction(this, i, pNext->GetComAct()->GetName());

					//	如果QueueAction的时候设置了ResetSpeed，则这里将速度设置重置，默认情况下该标志为false
					if (pNext->IsSetSpeedWhenActStart())
						SetPlaySpeedByChannel(i, pNext->GetComAct());
					
					delete pNode->m_pActive;
					pNode->m_pActive = pNext;
					pNext->Play(i, 1.f, pNode->m_pActive->GetTransTime(), m_EventMasks[i], true, m_bAbsTrack, pNext->GetNoFxFlag());
					pNext->UpdateAct(dwUpdateTime);
					m_bAbsTrack = false;
				}
				else
				{
					pNode->m_pActive->Stop(false);
					ca.m_RankNodes.RemoveAt(posCur);
					delete pNode;
				}
			}
			else
				pNode->m_pActive->UpdateAct(dwUpdateTime);
		}
	}
}

//	Tick Skin Model
void CECModel::TickSkinModel(DWORD dwUpdateTime)
{
	if (!m_pA3DSkinModel)
		return;
	
#ifdef A3D_PHYSX

	if (m_pModelPhysics)
		m_pModelPhysics->UpdateBeforePhysXSim(dwUpdateTime);
	else
	{
		if (!m_bKeepCurPose)
			m_pA3DSkinModel->Update(dwUpdateTime);
	}

#else

	m_pA3DSkinModel->Update(dwUpdateTime);

#endif
	
}

//	Tick ECModel combined action events
void CECModel::UpdateChannelEvents(DWORD dwActTime, DWORD dwEventTime)
{
	for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
	{
		ChannelAct& ca = m_ChannelActs[i];
		ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();

		while (pos)
		{
			ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);
			pNode->m_pActive->UpdateEvent(dwActTime, dwEventTime);
		}
	}
}

//	Tick CoGfx
void CECModel::TickCoGfx(DWORD dwUpdateTime)
{
	if (m_bCoGfxSleep)
		return;

	for (CoGfxMap::iterator it = m_CoGfxMap.begin(); it != m_CoGfxMap.end(); ++it)
	{
		GFX_INFO* pInfo = it->second;
		A3DGFXEx* pGfx = pInfo->GetGfx();

		if (!pGfx || pGfx->GetState() == ST_STOP || !pGfx->IsVisible())
			continue;

		bool bIsHookMat = false;
		A3DMATRIX4 matHookOrAbs = GetGfxBindHookTransMat(pInfo, bIsHookMat);
		A3DMATRIX4 matGfxInfo = pInfo->GetMatTran();

		if (pInfo->GetDelayTime())
			_DelayUpdateGfxParentTM(pGfx, matGfxInfo * matHookOrAbs, (float)dwUpdateTime / (float)pInfo->GetDelayTime());
		else
			pGfx->SetParentTM(matGfxInfo * matHookOrAbs);

		if (m_bGfxScaleChanged)
			pGfx->SetScale(pInfo->GetScale() * m_fGfxScaleFactor);

		if (pInfo->UseModelAlpha() && m_pA3DSkinModel)
		{
			float fAlpha, fTans = m_pA3DSkinModel->GetTransparent();

			if (m_pA3DSkinModel->IsHidden())
				fAlpha = 0;
			else if (fTans < 0)
				fAlpha = 1.0f;
			else
				fAlpha = 1.0f - fTans;

			pGfx->SetAlpha(fAlpha * pInfo->GetAlpha());
		}

		if (m_pA3DSkinModel)
			InnerUpdateCoGfxParam(static_cast<PGFX_INFO>(pInfo), m_pA3DSkinModel->GetSkeleton());

		if (dwUpdateTime)
			pGfx->TickAnimation(dwUpdateTime);
	}

	//	we have updated all active gfx scales above
	m_bGfxScaleChanged = false;
}

//	Tick Child Models
void CECModel::TickShownChildModels(DWORD dwDeltaTime, bool bNoAnim)
{
#ifdef A3D_PHYSX

	for (ChildPhyMap::iterator itPhy = m_ChildPhyDelayMap.begin();
		itPhy != m_ChildPhyDelayMap.end(); )
	{
		CECModel* pChild = itPhy->first;
		ChildPhyDelayInfo& delayInfo = itPhy->second;

		if (delayInfo.iDelayTime > 0)
		{
			if (--delayInfo.iDelayTime <= 0)
			{
				A3DModelPhysics* pPhys = pChild->GetModelPhysics();
				if (pPhys && pPhys->GetPhysState() != delayInfo.nPhyState)
					pPhys->ChangePhysState(delayInfo.nPhyState);

				itPhy = m_ChildPhyDelayMap.erase(itPhy);
			}
			else
				++itPhy;
		}
		else
			itPhy = m_ChildPhyDelayMap.erase(itPhy);
	}

#endif

	for (ECModelMap::iterator it2 = m_ChildModels.begin(); it2 != m_ChildModels.end(); ++it2)
	{
		CECModel* pChild = it2->second;

		if (pChild->IsShown() || pChild->IsNeedUpdateWhenHidden())
		{
			pChild->Tick(dwDeltaTime, bNoAnim);
		}
	}

}

//	Update model aabb by skin model and all child models
void CECModel::UpdateModelAABB()
{
	m_ModelAABB.Clear();

	if (m_pA3DSkinModel)
	{
		a3d_ExpandAABB(m_ModelAABB.Mins, m_ModelAABB.Maxs, m_pA3DSkinModel->GetModelAABB());
	}

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		CECModel* pChild = it->second;
		pChild->UpdateModelAABB();

		if (pChild->IsShown())
			a3d_ExpandAABB(m_ModelAABB.Mins, m_ModelAABB.Maxs, pChild->m_ModelAABB);
	}

	m_ModelAABB.CompleteCenterExts();
}

// Set scale to all root-bones and gfx
bool CECModel::ScaleAllRootBonesAndGfx(float fScale)
{
	if (!m_pMapModel || !m_pA3DSkinModel || !m_pA3DSkinModel->GetSkeleton())
		return false;

	A3DSkeleton* pSkeleton = m_pA3DSkinModel->GetSkeleton();
	for (int iRootIdx = 0; iRootIdx < pSkeleton->GetRootBoneNum(); ++iRootIdx)
	{
		int iBoneIdx = pSkeleton->GetRootBone(iRootIdx);
		A3DBone* pRootBone = pSkeleton->GetBone(iBoneIdx);

		pRootBone->SetLocalScale(1.f, 1.f);

		float fInitScl = 1.f;
		if (BoneScaleEx* pSclInfo = GetBoneScaleExByIndex(iBoneIdx))
		{
			fInitScl = pSclInfo->m_fWholeFactor;
		}

		pRootBone->SetWholeScale(fScale * fInitScl);
	}

	m_fRootBoneScaleFactor = fScale;

	SetGfxScale(fScale);
	return true;
}

//	Inner Update CoGfx's ModParam
bool CECModel::InnerUpdateCoGfxParam(GFX_INFO* pInfo, A3DSkeleton* pSkeleton)
{
	if (!pInfo || !pSkeleton)
		return false;

	A3DGFXEx* pGfx = pInfo->GetGfx();
	if (!pGfx)
		return false;

	ModActParamList& lst = pInfo->GetParamList();
	size_t nModParamIdx , nModParamCount = lst.size();
	for (nModParamIdx = 0; nModParamIdx < nModParamCount; ++nModParamIdx)
	{
		ModActParam* pParam = lst[nModParamIdx];
		switch(pParam->m_nParamId)
		{
		case ID_PARAM_LTN_POS1:
		case ID_PARAM_LTN_POS2:
			{
				int nIndex = -1;
				A3DSkeletonHook* pHook = pSkeleton->GetHook(pParam->m_ParamData.m_strHook, &nIndex);
				if (!pHook)
					continue;
				pGfx->UpdateEleParam(pParam->m_strEleName, pParam->m_nParamId, pHook->GetAbsoluteTM().GetRow(3));
			}
			break;
		}
	}

	return true;
}

//	Check and update action's play speed
void CECModel::SetPlaySpeedByChannel(int nChannel, A3DCombinedAction* pComAct)
{
	if (m_nMainChannel == nChannel || 0 == nChannel)
	{
		m_fPlaySpeed = m_pMapModel->m_fDefPlaySpeed * pComAct->GetPlaySpeed();
	}
}

void CECModel::ApplyPixelShader()
{
	if (m_pReplacePS)
	{
		m_pReplacePS->Appear();
		g_GfxApplyPixelShaderConsts(m_pA3DDevice, m_vecReplacePSConsts, m_dwPSTickTime, 0);
	}
	else
	{
		m_pPixelShader->Appear();
		g_GfxApplyPixelShaderConsts(m_pA3DDevice, m_pMapModel->m_vecPSConsts, m_dwPSTickTime, 0);
	}
}

void CECModel::RestorePixelShader()
{
	m_pA3DDevice->ClearPixelShader();
}

bool CECModel::SetReplaceShader(const char* szShader, const char* szShaderTex, const GfxPSConstVec& consts)
{
	static const AString _base_path = "Gfx\\Textures\\";

	RemoveReplaceShader();
	m_pReplacePS = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile(szShader);

	if (!m_pReplacePS)
		return false;

#ifdef _ANGELICA21

	if (szShaderTex[0])
	{
		AString strPath = _base_path + szShaderTex;
		m_pReplacePS->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)(const char*)strPath);
	}

#else

	if (szShaderTex[0])
	{
		AString strPath = _base_path + szShaderTex;
		m_pReplacePS->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)(const char*)strPath);
	}

#endif

	const size_t consts_count = consts.size();

	if (consts_count)
	{
		m_vecReplacePSConsts.reserve(consts_count);

		for (size_t i = 0; i < consts_count; i++)
		{
			m_vecReplacePSConsts.push_back(consts[i]);
			m_vecReplacePSConsts[i].CalcTotalTime();
		}
	}

	m_dwPSTickTime = 0;
	return true;
}

void CECModel::RemoveReplaceShader()
{
	if (m_pReplacePS)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseShader(&m_pReplacePS);
		m_pReplacePS = NULL;
		m_vecReplacePSConsts.clear();
		m_dwPSTickTime = 0;
	}
}

bool CECModel::IsRenderSkinModel(bool bRenderSkinModel) const
{
	bool bRenderSmdFlag = m_pA3DSkinModel && m_pMapModel->m_bRenderSkinModel && bRenderSkinModel && !m_SkinModelRenderCtrl.IsHidden();

#ifdef _ANGELICA21

	return bRenderSmdFlag && (m_bLoadSkinModel || (m_dwChildRenderFlag & SMD_CHILD_RENDER_SKIP));

#else

	return bRenderSmdFlag && m_bLoadSkinModel;

#endif
}

//	Render routine
bool CECModel::Render(A3DViewport* pViewport, bool bRenderAtOnce/* false */, bool bRenderSkinModel/*=true*/, 
	DWORD dwFlags/*=0*/, A3DSkinModelRenderModifier* pRenderModifier/* = NULL*/, bool bRenderGfx/* = true*/)
{
	AGPA_RECORD_FUNCTION_TIME;
	bool bVisible = false;

	// first of all update light info
	if (m_pA3DSkinModel && m_bLoadSkinModel && m_bUpdateLightInfo)
	{
#if defined(_ANGELICA21)
		if (m_GetLightInfoFunc)
		{
			DWORD dwLightId;
			A3DSkinModel::LIGHTINFO LightInfo;

			if ((*m_GetLightInfoFunc)(m_pA3DSkinModel->GetPos(), LightInfo, dwLightId))
			{
				m_LightTransInfo.CheckCurLight(m_pA3DSkinModel, dwLightId, LightInfo);
			}
		}
		else
			AfxSetA3DSkinModelSceneLightInfo(m_pA3DSkinModel);
#else
		AfxSetA3DSkinModelSceneLightInfo(m_pA3DSkinModel);
#endif
	}

	if(bRenderGfx)
		RenderActionFX(pViewport);

	bool bRenderMainModel = IsRenderSkinModel(bRenderSkinModel);
	if (bRenderMainModel)
	{
		bVisible = pViewport->GetCamera()->AABBInViewFrustum(m_aabbUsedToCullInRender);

		if (bVisible)
		{
#ifdef _ANGELICA21
			A3DShader* pCustomShader;
			const GfxPSConstVec* pPSConstVec;

			if (m_pReplacePS)
			{
				pCustomShader = m_pReplacePS;
				pPSConstVec = &m_vecReplacePSConsts;
			}
			else
			{
				pCustomShader = m_pPixelShader;
				pPSConstVec = &m_pMapModel->m_vecPSConsts;
			}

			if (0)//pCustomShader && (dwFlags & (A3DSkinModel::RAO_NOPIXELSHADER | A3DSkinModel::RAO_NOTEXTURE)) == 0)
			{
				if (m_CustomRenderFunc)
				{
					SHADERGENERAL& sh = pCustomShader->GetGeneralProps();
					(*m_CustomRenderFunc)(pViewport, m_pA3DSkinModel, pCustomShader, *pPSConstVec, m_dwPSTickTime, sh.strHLSLPrefix, false);
				}
				else
				{
					ApplyPixelShader();
					m_pA3DSkinModel->RenderAtOnce(pViewport, dwFlags | A3DSkinModel::RAO_NOPIXELSHADER, false);
					RestorePixelShader();
				}
			}
			else
			{
				if (!bRenderAtOnce)
					m_pA3DSkinModel->Render(pViewport, false);
				else
					m_pA3DSkinModel->RenderAtOnce(pViewport, dwFlags, false);
			}
#else
			if (!bRenderAtOnce)
			{
				#ifdef _ANGELICA22
					m_pA3DSkinModel->Render(pViewport, false, pRenderModifier);
				#else
					m_pA3DSkinModel->Render(pViewport, false);
				#endif
			}
			else
				m_pA3DSkinModel->RenderAtOnce(pViewport, dwFlags, false);
#endif
		}

		if (m_pMapModel->m_bCanCastShadow && m_bCastShadow && m_pA3DSkinModel)
			AfxECModelAddShadower(m_aabbUsedToCullInRender.Center, m_aabbUsedToCullInRender, m_pA3DSkinModel);
	}

#ifdef A3D_PHYSX
	if (0 != m_pModelPhysics)
		m_pModelPhysics->OnRender();
#endif

	if (m_pBlurInfo && m_pBlurInfo->m_bRoot && m_pA3DSkinModel)
	{
		if(bRenderGfx)
			AfxGetGFXExMan()->RegisterECMForMotionBlur(this);
	}

	if(bRenderGfx)
		RenderCoGfx();
	
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		CECModel* pChild = it->second;

		if (pChild->IsShown())
		{
#ifdef _ANGELICA21
			if (pChild->GetChildRenderFlag() & SMD_CHILD_RENDER_SKIP)
				continue;

			bool bRenderChildModel = bRenderSkinModel;
			if (bRenderChildModel && !bRenderMainModel)
			{
				//	Not render Main Model : Not render ComboModel too
				if (FindComboModel(pChild) >= 0)
					bRenderChildModel = false;
			}

			pChild->Render(pViewport, bRenderAtOnce, bRenderChildModel, dwFlags, pRenderModifier, bRenderGfx);

#else

			pChild->Render(pViewport, bRenderAtOnce, bRenderSkinModel, dwFlags, pRenderModifier, bRenderGfx);

#endif
		}
	}

	if (m_pTrackBoneVelocity && s_bShowTrackBoneVel)
		m_pTrackBoneVelocity->Render(pViewport);

#ifdef _ANGELICA21
	return bVisible;
#else
	return true;
#endif
}

//	Render CoGfx
void CECModel::RenderCoGfx()
{
	if (m_bCoGfxSleep)
		return;

	for (CoGfxMap::iterator itGfx = m_CoGfxMap.begin(); itGfx != m_CoGfxMap.end(); ++itGfx)
	{
		A3DGFXEx* pGfx = itGfx->second->GetGfx();
		if (!pGfx)
			continue;

		if (pGfx->GetState() != ST_STOP)
			AfxGetGFXExMan()->RegisterGfx(pGfx);
	}
}


void CECModel::RenderActionFX(A3DViewport* pViewport)
{
	for (int i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
	{
		ChannelAct& ca = m_ChannelActs[i];
		ChannelActNode* pNode = ca.GetHighestRankNode();

		if (pNode)
			pNode->m_pActive->Render(pViewport);
	}
}

void CECModel::StartECModelDelayMode(int nDelayTime)
{
	m_kECMDelayInfo.OnStartDelay(nDelayTime);
}

void CECModel::UpdateBaseActTimeSpan()
{
#ifndef GFX_EDITOR

	if (m_pMapModel->m_bActMapped)
		return;

#endif

	if (!m_pA3DSkinModel)
		return;

	CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;

	for (CombinedActMap::iterator it = ActionMap.begin(); it != ActionMap.end(); ++it)
	{
		for (int i = 0; i < it->second->GetBaseActCount(); i++)
		{
			PACTION_INFO pInfo = it->second->GetBaseAct(i);
			A3DSkinModelActionCore* pBaseAct = m_pA3DSkinModel->GetAction(pInfo->GetName());
			if (pBaseAct) pInfo->SetTimeSpan(pBaseAct->GetActionTime());
		}

		it->second->CalcMinComActTimeSpan();
	}

#ifndef GFX_EDITOR

	m_pMapModel->m_bActMapped = true;

#endif
}

void CECModel::ShowCoGfx(bool bShow)
{
	for (CoGfxMap::iterator it = m_CoGfxMap.begin(); it != m_CoGfxMap.end(); ++it)
	{
		PGFX_INFO pInfo = it->second;
		A3DGFXEx* pGfx = pInfo->GetGfx();
		assert(pGfx);

		if (bShow)
		{
			pGfx->SetScale(pInfo->GetScale() * m_fGfxScaleFactor);
			pGfx->SetAlpha(pInfo->GetAlpha());
			pGfx->SetPlaySpeed(pInfo->GetPlaySpeed());
			pGfx->SetSfxPriority(m_nSfxPriority);
			TransferEcmProperties(pGfx);
			pGfx->Start(true);
		}
		else
			pGfx->Stop();
	}
}

//	This function will make all CoGfx stop from Ticking or Rendering, like they are sleeping
void CECModel::SleepCoGfx(bool bSleep, bool bRecurOnChild)
{
	m_bCoGfxSleep = bSleep;

	if (!bRecurOnChild)
		return;

	for (ECModelMap::iterator it = m_ChildModels.begin()
		; it != m_ChildModels.end()
		; ++it)
	{
		it->second->SleepCoGfx(bSleep, bRecurOnChild);
	}
}

void CECModel::RemoveBoneScale(BoneScale* pScale)
{
	for (size_t i = 0; i < m_pMapModel->m_BoneScales.size(); i++)
	{
		if (m_pMapModel->m_BoneScales[i] == pScale)
		{
			if (m_pA3DSkinModel)
			{
				A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();
				A3DBone* pBone = pSke->GetBone(pScale->m_nIndex);
				pBone->SetScaleFactor(1.0f);
				pBone->SetScaleType(0);
			}
			
			m_pMapModel->m_BoneScales.erase(&m_pMapModel->m_BoneScales[i]);
			return;
		}
	}
}

void CECModel::UpdateBoneScales()
{
	if (!m_pA3DSkinModel)
		return;

	A3DSkeleton * pSke = m_pA3DSkinModel->GetSkeleton();

	if (!pSke)
		return;

	BoneScaleArray& BoneScales = m_pMapModel->m_BoneScales;

	for (size_t i = 0; i < BoneScales.size(); i++)
	{
		BoneScale* pScale = BoneScales[i];

		if (pScale->m_nIndex >= pSke->GetBoneNum()) continue;
		A3DBone* pBone = pSke->GetBone(pScale->m_nIndex);
		if (!pBone) continue;
		pBone->SetScaleFactor(pScale->m_vScale);
		pBone->SetScaleType(pScale->m_nType);
	}
}


void CECModel::RemoveBoneScaleEx(BoneScaleEx* pScale)
{
	for (size_t i = 0; i < m_pMapModel->m_BoneScaleExArr.size(); i++)
	{
		if (m_pMapModel->m_BoneScaleExArr[i] == pScale)
		{
			if (m_pA3DSkinModel)
			{
				A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();
				A3DBone* pBone = pSke->GetBone(pScale->m_nIndex);
				
				if (pBone)
				{
					pBone->SetLocalScale(1.0f, 1.0f);
					pBone->SetWholeScale(1.0f);
				}
			}
			
			m_pMapModel->m_BoneScaleExArr.erase(m_pMapModel->m_BoneScaleExArr.begin()+i);
			return;
		}
	}
}

void CECModel::UpdateBoneScaleEx()
{
	UpdateBoneScaleEx(m_pMapModel->m_BoneScaleExArr, m_pMapModel->m_strScaleBaseBone);
}

void CECModel::UpdateBoneScaleEx(BoneScaleExArray& arr, const char* szBaseBone)
{
	if (!m_pA3DSkinModel)
		return;

	A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();

	if (!pSke)
		return;

	for (int n = 0; n < pSke->GetBoneNum(); n++)
	{
		A3DBone* pBone = pSke->GetBone(n);

		if (pBone)
		{
			pBone->SetLocalScale(1.0f, 1.0f);
			pBone->SetWholeScale(1.0f);
		}
	}

	for (size_t i = 0; i < arr.size(); i++)
	{
		BoneScaleEx* pScale = arr[i];

		if (pScale->m_nIndex >= pSke->GetBoneNum())
			continue;

		A3DBone* pBone = pSke->GetBone(pScale->m_nIndex);

		if (pBone)
		{
			pBone->SetLocalScale(pScale->m_fLenFactor, pScale->m_fThickFactor);
			pBone->SetWholeScale(pScale->m_fWholeFactor);
		}
	}

	pSke->CalcFootOffset(szBaseBone);
}

bool CECModel::PlayActionByName(const char* szActName, float fWeight, bool bRestart/* true */,
					int nTransTime/* 200 */, bool bForceStop/* true */, DWORD dwUserData/* 0 */, bool bNoFx, bool bNoCamShake)
{
	return PlayActionByName(0, szActName, fWeight, bRestart, nTransTime, bForceStop, dwUserData, bNoFx, bNoCamShake);
}

bool CECModel::IsShouldRemoveMotionBlurInfo(int nChannel) const
{
	return m_pBlurInfo && m_pBlurInfo->m_bRoot && m_pBlurInfo->m_bStopWhenActChange && m_pBlurInfo->GetInChannel() == nChannel;
}

bool CECModel::PlayActionByName(int nChannel, const char* szActName, float fWeight, bool bRestart/* true */,
					int nTransTime/* 200 */, bool bForceStop/* true */, DWORD dwUserData/* 0 */, bool bNoFx, bool bNoCamShake)
{
	if (m_bKeepCurPose)
		return false;

	if (!szActName)
		return false;

	A3DCombinedAction* pComAct = GetComActByName(szActName);

	if (pComAct == NULL)
		return false;

	BYTE rank = pComAct->GetRank(nChannel);
	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = ca.GetNodeByRank(rank);
	A3DCombActDynData* pActive;

	if (pNode)
	{
		pActive = pNode->m_pActive;
		pNode->RemoveQueuedActs();
	}
	else
		pActive = 0;

	if (!bRestart && pActive && stricmp(pActive->GetComAct()->GetName(), szActName) == 0)
		return true;

	if (IsShouldRemoveMotionBlurInfo(nChannel))
		RemoveMotionBlurInfo();

	// script
	m_pMapModel->OnScriptPlayAction(this, nChannel, szActName);

	// delay mode state
	m_kECMDelayInfo.OnPlayComAction();

	if (pActive != NULL)
	{
		pActive->Stop(bForceStop);
		delete pActive;
	}
	else if (!m_bFirstActPlayed)
	{
		m_bFirstActPlayed = true;
		nTransTime = 0;
	}

	if (!pNode)
	{
		pNode = new ChannelActNode();
		pNode->m_Rank = rank;
		ca.m_RankNodes.AddTail(pNode);
	}

	SetPlaySpeedByChannel(nChannel, pComAct);

	A3DCombinedAction::Enum_SpecType eSpecType = pComAct->GetSpecType();
	switch( eSpecType )
	{
	case A3DCombinedAction::SPECIALTYPE_NONE:
		pNode->m_pActive = new A3DCombActDynData( pComAct, this );
		break;
	case A3DCombinedAction::SPECIALTYPE_WALKRUN:
		pNode->m_pActive = new A3DWalkRunDynData( pComAct, this );
		break;
	case A3DCombinedAction::SPECIALTYPE_WALKRUNUPDOWN:
		pNode->m_pActive = new A3DWalkRunUpDownDynData( pComAct, this );
		break;
	case A3DCombinedAction::SPECIALTYPE_RUNTURN:
		pNode->m_pActive = new A3DRunTurnDynData( pComAct, this );
		break;
	default:
		ASSERT(0);
		return false;
	}
	pNode->m_pActive->SetUserData(dwUserData);
	pNode->m_pActive->Play(nChannel, fWeight, nTransTime, m_EventMasks[nChannel], bRestart, m_bAbsTrack, bNoFx, bNoCamShake);
	m_bAbsTrack = false;

	// clear notify flag each time we play a new action
	pNode->m_pActFlag = NULL;
	pNode->m_dwFlagMode = COMACT_FLAG_MODE_NONE;
	return true;
}

bool CECModel::InitWalkRunBlend( const _WALKRUNBLEND_PARAMS& c_params )
{
#ifdef _ANGELICA21

	if( m_WalkRunUpDownBlendInfo.m_pWalkRunUpDownCombAct )
		goto Error;

	if( m_WalkRunBlendInfo.m_pWalkRunCombAct && ! c_params.m_strWalkRunCombAct.Compare( m_WalkRunBlendInfo.m_pWalkRunCombAct->GetName() ) )
		return true;

	for( int i = 0; i < A3DSkinModel::ACTCHA_MAX; ++ i )
	{
		A3DAnimNode* pNode = m_pECAnimNodeMan->GetAnimNode( A3DCombinedAction::SPECIALTYPE_WALKRUN, i );
		A3DAnimNodeBlendWalkRun* pWalkRunNode = dynamic_cast< A3DAnimNodeBlendWalkRun* >( pNode );
		if( ! pWalkRunNode )
			goto Error;

		bool bRet = pWalkRunNode->Init( m_pA3DSkinModel, c_params.m_strWalkBaseAct, c_params.m_fWalkSpeed, c_params.m_strRunBaseAct, c_params.m_fRunSpeed );
		if( ! bRet )
			goto Error;
	}
	A3DCombinedAction* pWalkCombAct		= GetComActByName( c_params.m_strWalkCombAct );
	A3DCombinedAction* pRunCombAct		= GetComActByName( c_params.m_strRunCombAct );
	A3DCombinedAction* pWalkRunCombAct	= GetComActByName( c_params.m_strWalkRunCombAct );
	if( ! pWalkRunCombAct )
	{
		pWalkRunCombAct = new A3DCombinedAction;

		int iEventFromCombAct = -1;
		A3DCombinedAction* pEventFromCombAct = NULL;

		if( pRunCombAct )
		{
			iEventFromCombAct = 1;
			pEventFromCombAct = pRunCombAct;
		}
		else if( pWalkCombAct )
		{
			iEventFromCombAct = 0;
			pEventFromCombAct = pWalkCombAct;
		}
		if( pEventFromCombAct )
		{
			int iCount = pEventFromCombAct->GetEventCount();
			for( int i = 0; i < iCount; ++ i )
			{
				EVENT_INFO* pInfo = pEventFromCombAct->GetEventInfo(i);
				if( ! pInfo )
					continue;
				EVENT_INFO* pNewInfo = EVENT_INFO::CloneFrom( pWalkRunCombAct, * pInfo );
				pWalkRunCombAct->AddOneEvent( pNewInfo );
			}
		}
		pWalkRunCombAct->SetName( c_params.m_strWalkRunCombAct );
		pWalkRunCombAct->SetSpecType( A3DCombinedAction::SPECIALTYPE_WALKRUN );
		pWalkRunCombAct->SetWalkRunEventFromCombAct( iEventFromCombAct, pEventFromCombAct );

		AddCombinedAction( pWalkRunCombAct );
	}
	m_WalkRunBlendInfo.m_fWalkSpeed			= c_params.m_fWalkSpeed;
	m_WalkRunBlendInfo.m_fRunSpeed			= c_params.m_fRunSpeed;
	m_WalkRunBlendInfo.m_pWalkRunCombAct	= pWalkRunCombAct;

#endif

	return true;

Error:

	return false;
}

//	Set walk run velocity
bool CECModel::SetWalkRunVelocity( float fVelocity, int nBlendInTime )
{
#ifdef _ANGELICA21

	if( ! m_pECAnimNodeMan || ! m_WalkRunBlendInfo.m_pWalkRunCombAct )
		return false;

	return m_pECAnimNodeMan->SetWalkRunVelocity( fVelocity, nBlendInTime );

#else
	
	return false;

#endif
}

bool CECModel::InitWalkRunUpDownBlend( const _WALKRUNUPDOWNBLEND_PARAMS& c_params )
{
#ifdef _ANGELICA21

	if( m_WalkRunBlendInfo.m_pWalkRunCombAct )
		goto Error;

	if( ! c_params.m_pCB )
		goto Error;

	if( m_WalkRunUpDownBlendInfo.m_pWalkRunUpDownCombAct && ! c_params.m_strWalkRunUpDownCombAct.Compare( m_WalkRunUpDownBlendInfo.m_pWalkRunUpDownCombAct->GetName() ) )
		return true;

	for( int i = 0; i < A3DSkinModel::ACTCHA_MAX; ++ i )
	{
		A3DAnimNode* pNode = m_pECAnimNodeMan->GetAnimNode( A3DCombinedAction::SPECIALTYPE_WALKRUNUPDOWN, i );
		AnimBlendTree* pWalkRunUpDownTree = dynamic_cast< AnimBlendTree* >( pNode );
		if( ! pWalkRunUpDownTree )
			goto Error;

		AnimBlendNode* pBlendNode = pWalkRunUpDownTree->FindNode( "walkrun" );
		if( ! pBlendNode )
			goto Error;

		pNode = pBlendNode->GetAnimNode();
		A3DAnimNodeBlendWalkRun* pWalkRunNode = dynamic_cast< A3DAnimNodeBlendWalkRun* >( pNode );
		if( ! pWalkRunNode )
			goto Error;

		bool bRet = pWalkRunNode->Init( m_pA3DSkinModel, c_params.m_strWalkBaseAct, c_params.m_fWalkSpeed, c_params.m_strRunBaseAct, c_params.m_fRunSpeed );
		if( ! bRet )
			goto Error;

		float fDuration = pWalkRunNode->GetDuration();

		pBlendNode = pWalkRunUpDownTree->FindNode( "walkup" );
		if( ! pBlendNode )
			goto Error;

		pBlendNode->SetDuration( fDuration );

		pNode = pBlendNode->GetAnimNode();
		A3DAnimNodeAction* pWalkUpNode = dynamic_cast< A3DAnimNodeAction* >( pNode );
		if( ! pWalkUpNode )
			goto Error;

		bRet = pWalkUpNode->Init( m_pA3DSkinModel, c_params.m_strWalkUpBaseAct );
		if( ! bRet )
			goto Error;

		pBlendNode = pWalkRunUpDownTree->FindNode( "walkdown" );
		if( ! pBlendNode )
			goto Error;

		pBlendNode->SetDuration( fDuration );

		pNode = pBlendNode->GetAnimNode();
		A3DAnimNodeAction* pWalkDownNode = dynamic_cast< A3DAnimNodeAction* >( pNode );
		if( ! pWalkDownNode )
			goto Error;

		bRet = pWalkDownNode->Init( m_pA3DSkinModel, c_params.m_strWalkDownBaseAct );
		if( ! bRet )
			goto Error;

		pBlendNode = pWalkRunUpDownTree->FindNode( "runup" );
		if( ! pBlendNode )
			goto Error;

		pBlendNode->SetDuration( fDuration );

		pNode = pBlendNode->GetAnimNode();
		A3DAnimNodeAction* pRunUpNode = dynamic_cast< A3DAnimNodeAction* >( pNode );
		if( ! pRunUpNode )
			goto Error;

		bRet = pRunUpNode->Init( m_pA3DSkinModel, c_params.m_strRunUpBaseAct );
		if( ! bRet )
			goto Error;

		pBlendNode = pWalkRunUpDownTree->FindNode( "rundown" );
		if( ! pBlendNode )
			goto Error;

		pBlendNode->SetDuration( fDuration );

		pNode = pBlendNode->GetAnimNode();
		A3DAnimNodeAction* pRunDownNode = dynamic_cast< A3DAnimNodeAction* >( pNode );
		if( ! pRunDownNode )
			goto Error;

		bRet = pRunDownNode->Init( m_pA3DSkinModel, c_params.m_strRunDownBaseAct );
		if( ! bRet )
			goto Error;
	}
	A3DCombinedAction* pWalkCombAct				= GetComActByName( c_params.m_strWalkCombAct );
	A3DCombinedAction* pRunCombAct				= GetComActByName( c_params.m_strRunCombAct );
	A3DCombinedAction* pWalkRunUpDownCombAct	= GetComActByName( c_params.m_strWalkRunUpDownCombAct );
	if( ! pWalkRunUpDownCombAct )
	{
		pWalkRunUpDownCombAct = new A3DCombinedAction;

		int iEventFromCombAct = -1;
		A3DCombinedAction* pEventFromCombAct = NULL;

		if( pRunCombAct )
		{
			iEventFromCombAct = 1;
			pEventFromCombAct = pRunCombAct;
		}
		else if( pWalkCombAct )
		{
			iEventFromCombAct = 0;
			pEventFromCombAct = pWalkCombAct;
		}
		if( pEventFromCombAct )
		{
			int iCount = pEventFromCombAct->GetEventCount();
			for( int i = 0; i < iCount; ++ i )
			{
				EVENT_INFO* pInfo = pEventFromCombAct->GetEventInfo(i);
				if( ! pInfo )
					continue;
				EVENT_INFO* pNewInfo = EVENT_INFO::CloneFrom( pWalkRunUpDownCombAct, * pInfo );
				pWalkRunUpDownCombAct->AddOneEvent( pNewInfo );
			}
		}
		pWalkRunUpDownCombAct->SetName( c_params.m_strWalkRunUpDownCombAct );
		pWalkRunUpDownCombAct->SetSpecType( A3DCombinedAction::SPECIALTYPE_WALKRUNUPDOWN );
		pWalkRunUpDownCombAct->SetWalkRunEventFromCombAct( iEventFromCombAct, pEventFromCombAct );

		AddCombinedAction( pWalkRunUpDownCombAct );
	}
	m_WalkRunUpDownBlendInfo.m_fWalkSpeed				= c_params.m_fWalkSpeed;
	m_WalkRunUpDownBlendInfo.m_fRunSpeed				= c_params.m_fRunSpeed;
	m_WalkRunUpDownBlendInfo.m_pWalkRunUpDownCombAct	= pWalkRunUpDownCombAct; 
	m_WalkRunUpDownBlendInfo.m_pCB						= c_params.m_pCB;

#endif

	return true;

Error:

	return false;
}

bool CECModel::SetWalkRunUpDownVelocity( float fVelocity, int nBlendInTime )
{
#ifdef _ANGELICA21

	if( ! m_pECAnimNodeMan || ! m_WalkRunUpDownBlendInfo.m_pWalkRunUpDownCombAct )
		return false;

	return m_pECAnimNodeMan->SetWalkRunUpDownVelocity( fVelocity, nBlendInTime );

#else

	return false;

#endif
}

void CECModel::EnableSlopAnimBlend(bool bEnable)
{
#ifdef _ANGELICA21
	A3DWalkRunUpDownDynData::SetUseSlope(bEnable);
#endif
}
bool CECModel::IsSlopAnimBlendEnabled() const
{
#ifdef _ANGELICA21
	return A3DWalkRunUpDownDynData::IsUseSlope();
#else
	return false;
#endif
}
//get & set slope animation angle threshold. angle above this slope animation weight 1, 0~fAngleDeg slope animation weight 0 - 1.
void CECModel::SetSlopAnimAngle(float fAngleDeg)
{
#ifdef _ANGELICA21
	A3DWalkRunUpDownDynData::SetSlopeAngel(fAngleDeg);
#endif
}
float CECModel::GetSlopAnimAngle() const
{
#ifdef _ANGELICA21
	return A3DWalkRunUpDownDynData::GetSlopeAngle();
#else
	return 0.0f;
#endif
}

bool CECModel::InitRunTurnBlend( const _RUNTURNBLEND_PARAMS& c_params )
{
#ifdef _ANGELICA21

	if( m_RunTurnBlendInfo.m_pRunTurnCombAct && ! c_params.m_strRunTurnCombAct.Compare( m_RunTurnBlendInfo.m_pRunTurnCombAct->GetName() ) )
		return true;

	for( int i = 0; i < A3DSkinModel::ACTCHA_MAX; ++ i )
	{
		A3DAnimNode* pNode = m_pECAnimNodeMan->GetAnimNode( A3DCombinedAction::SPECIALTYPE_RUNTURN, i );
		A3DAnimNodeBlend3Children* pRunTurnNode = dynamic_cast< A3DAnimNodeBlend3Children* >( pNode );
		if( ! pRunTurnNode )
			goto Error;

		bool bRet = pRunTurnNode->Init( m_pA3DSkinModel, c_params.m_strRunBaseAct, c_params.m_strTurnLeftBaseAct, c_params.m_strTurnRightBaseAct );
		if( ! bRet )
			goto Error;
	}
	A3DCombinedAction* pRunCombAct		= GetComActByName( c_params.m_strRunCombAct );
	A3DCombinedAction* pRunTurnCombAct	= GetComActByName( c_params.m_strRunTurnCombAct );
	if( ! pRunTurnCombAct )
	{
		pRunTurnCombAct = new A3DCombinedAction;

		int iEventFromCombAct = -1;
		A3DCombinedAction* pEventFromCombAct = NULL;

		if( pRunCombAct )
		{
			iEventFromCombAct = 0;
			pEventFromCombAct = pRunCombAct;
		}
		if( pEventFromCombAct )
		{
			int iCount = pEventFromCombAct->GetEventCount();
			for( int i = 0; i < iCount; ++ i )
			{
				EVENT_INFO* pInfo = pRunCombAct->GetEventInfo(i);
				if( ! pInfo )
					continue;
				EVENT_INFO* pNewInfo = EVENT_INFO::CloneFrom( pRunTurnCombAct, * pInfo );
				pRunTurnCombAct->AddOneEvent( pNewInfo );
			}
		}
		pRunTurnCombAct->SetName( c_params.m_strRunTurnCombAct );
		pRunTurnCombAct->SetSpecType( A3DCombinedAction::SPECIALTYPE_RUNTURN );
		pRunTurnCombAct->SetRunTurnEventFromCombAct( iEventFromCombAct, pEventFromCombAct );

		AddCombinedAction( pRunTurnCombAct );
	}
	m_RunTurnBlendInfo.m_fRunSpeed			= c_params.m_fRunSpeed;
	m_RunTurnBlendInfo.m_pRunTurnCombAct	= pRunTurnCombAct;

#endif

	return true;

Error:

	return false;
}

bool CECModel::SetRunTurnVelocity( float fVelocity )
{
#ifdef _ANGELICA21
	return SetAnimNodeSpeed( A3DCombinedAction::SPECIALTYPE_RUNTURN, fVelocity );
#else
	return false;
#endif
}

bool CECModel::SetRunTurnWeight(float fWeight1, float fWeight2, int nBlendInTime)
{
#ifdef _ANGELICA21
	return SetAnimNodeWeight( A3DCombinedAction::SPECIALTYPE_RUNTURN, fWeight1, fWeight2, nBlendInTime );
#else
	return false;
#endif
}

bool CECModel::SetAnimNodeSpeed( int iSpecType, float fSpeed )
{
#ifdef _ANGELICA21

	if( ! m_pECAnimNodeMan )
		return false;

	return m_pECAnimNodeMan->SetAnimNodeSpeed( iSpecType, fSpeed );

#else

	return false;

#endif
}

bool CECModel::SetAnimNodeWeight( int iSpecType, float fWeight1, float fWeight2, int iBlendInTime )
{
#ifdef _ANGELICA21

	if( ! m_pECAnimNodeMan )
		return false;

	return m_pECAnimNodeMan->SetAnimNodeWeight( iSpecType, fWeight1, fWeight2, iBlendInTime );

#else

	return false;

#endif
}

bool CECModel::PlayAttackAction(
	int nChannel,
	const char* szActName,
	int nTransTime,
	unsigned char SerialId,
	clientid_t nCasterId,
	clientid_t nCastTarget,
	const A3DVECTOR3* pFixedPoint,
	DWORD dwUserData,
	bool bNoFx,
	bool bNoCamShake)
{
	if (m_bKeepCurPose)
		return false;

	if (m_pA3DSkinModel == NULL)
		return false;

	A3DCombinedAction* pComAct = GetComActByName(szActName);

	if (pComAct == NULL)
		return false;

	BYTE rank = pComAct->GetRank(nChannel);
	StopChannelAction(nChannel, rank, true);

	// script
	m_pMapModel->OnScriptPlayAction(this, nChannel, szActName);

	// delay mode on play
	m_kECMDelayInfo.OnPlayComAction();

	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = new ChannelActNode();
	pNode->m_Rank = rank;
	ca.m_RankNodes.AddTail(pNode);
	pNode->m_pActive = new A3DCombActDynData(pComAct, this);
	pNode->m_pActive->SetAttackAct(true);
	pNode->m_pActive->SetCasterId(nCasterId);
	pNode->m_pActive->SetCastTargetId(nCastTarget);
	pNode->m_pActive->SetSerialId(SerialId);
	pNode->m_pActive->SetUserData(dwUserData);

	if (!nCastTarget && pFixedPoint)
		pNode->m_pActive->SetFixedPoint(*pFixedPoint);

	SetPlaySpeedByChannel(nChannel, pComAct);
	
	pNode->m_pActive->Play(nChannel, 1.0f, nTransTime, m_EventMasks[nChannel], true, m_bAbsTrack, bNoFx, bNoCamShake);
	m_bAbsTrack = false;
	pNode->m_pActFlag = NULL;
	pNode->m_dwFlagMode = COMACT_FLAG_MODE_ONCE_IGNOREGFX;

	return true;
}

void CECModel::OnScriptChangeEquip(int nEquipId, int nEquipFlag, bool bFashionMode, int nPathId, int nEquipIndex)
{
	m_bFashionMode = bFashionMode;

	if (nEquipFlag)
	{
		m_SlotEquipMap[nEquipIndex] = nEquipId;
		m_EquipSlotMap[nEquipId] = nEquipIndex;
	}
	else
	{
		m_SlotEquipMap.erase(nEquipIndex);
		m_EquipSlotMap.erase(nEquipId);
	}

	m_pMapModel->OnScriptChangeEquip(this, nEquipId, nEquipFlag, bFashionMode, nPathId, nEquipIndex);
}

void CECModel::AddOneAttackDamageData(
	int nChannel,
	clientid_t nCaster,
	clientid_t nTarget,
	unsigned char SerialId,
	DWORD dwModifier,
	int nDamage,
	bool bIsAttDelay,
	int nAttDelayTimeIdx)
{
	if (AfxSkillGfxAddDamageData(nCaster, nTarget, SerialId, dwModifier, nDamage))
		return;

	ChannelAct& ca = m_ChannelActs[nChannel];
	ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();

	while (pos)
	{
		ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);

		if (pNode->m_pActive->IsAttackAct())
		{
			TARGET_DATA td;
			td.dwModifier = dwModifier;
			td.idTarget = nTarget;
			td.nDamage = nDamage;
			td.bIsAttDelay = bIsAttDelay;
			td.nAttDelayTimeIdx = nAttDelayTimeIdx;
			pNode->m_pActive->AddOneTarget(td);
			return;
		}
	}

	AfxSkillGfxShowDamage(nCaster, nTarget, nDamage, 1, dwModifier);
	AfxSkillGfxShowCaster(nCaster, dwModifier);
}

bool CECModel::QueueAction(int nChannel, const char* szActName, int nTransTime, DWORD dwUserData/* 0 */, bool bForceStopPrevAct, bool bCheckTailDup, bool bNoFx, bool bResetSpeed)
{
	A3DCombinedAction* pComAct = GetComActByName(szActName);

	if (!pComAct)
		return false;

	BYTE rank = pComAct->GetRank(nChannel);
	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = ca.GetNodeByRank(rank);

	if (!pNode)
		return false;

	if (bCheckTailDup
		&& pNode->m_QueuedActs.GetCount()
		&& pNode->m_QueuedActs.GetTail()->GetComAct() == pComAct)
		return false;

	A3DCombActDynData* pDynData = new A3DCombActDynData(pComAct, this);
	pDynData->SetUserData(dwUserData);
	pDynData->SetTransTime(nTransTime);
	pDynData->SetStopPrevAct(bForceStopPrevAct);
	pDynData->SetNoFxFlag(bNoFx);
	pDynData->SetSpeedWhenActStart(bResetSpeed);
	pNode->m_QueuedActs.AddTail(pDynData);
	return true;
}

bool CECModel::RemoveQueuedActions( int nChannel, const char* szActName )
{
	A3DCombinedAction* pComAct = GetComActByName(szActName);

	if (!pComAct)
		return false;

	BYTE rank = pComAct->GetRank(nChannel);
	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = ca.GetNodeByRank(rank);

	if (!pNode)
		return false;

	pNode->RemoveQueuedActs();

	return true;
}

void CECModel::StopChildrenAct()
{
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		if (it->second->m_bCanAffectedByParent)
			it->second->StopAllActions();
}

void CECModel::StopAllActions(bool bStopFx)
{
	ResetMaterialScale();
	StopChannelAction(0, true, bStopFx);
	StopChildrenAct();
}

void CECModel::StopChannelAction(int nChannel, BYTE rank, bool bStopAct, bool bStopFx)
{
	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = ca.RemoveNodeByRank(rank);

	if (pNode)
	{
		pNode->m_pActive->Stop(bStopAct, bStopFx);
		delete pNode;
	}
}

void CECModel::StopChannelAction(int nChannel, bool bStopAct, bool bStopFx)
{
	ChannelAct& ca = m_ChannelActs[nChannel];
	ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();

	while (pos)
	{
		ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);
		pNode->m_pActive->Stop(bStopAct, bStopFx);
		delete pNode;
	}

	ca.m_RankNodes.RemoveAll();
}

void CECModel::SetId(clientid_t id)
{
	m_nId = id;
	
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		CECModel* pChild = it->second;

		if (pChild->InheritParentId())
			pChild->SetId(id);
	}
}

bool CECModel::AddChildModel(
	const char* szHangerName,
	bool bOnBone,
	const char* szHookName,
	const char* szModelFile,
	const char* szCCName,
	const bool* pbSetHangerInheritScale/* = NULL*/,
	DWORD dwBindFlag/* = 0*/)
{
	CECModel* pECModel = new CECModel;

	pECModel->SetGfxUseLOD(m_bGfxUseLod, false);
	pECModel->SetDisableCamShake(m_bDisableCamShake);
	pECModel->SetCreatedByGfx(m_bCreatedByGfx);
	pECModel->SetId(m_nId);

	if (!pECModel->Load(szModelFile, true, 0, true, true, false))
	{
		delete pECModel;
		return false;
	}

	if (!AddChildModel(szHangerName, bOnBone, szHookName, pECModel, szCCName, pbSetHangerInheritScale, dwBindFlag))
	{
		pECModel->Release();
		delete pECModel;
		return false;
	}

#ifndef _ANGELICA21
	//	tick 0 to make the child model be update to the new position
	//	for phys attacher and related joints' creation
	pECModel->Tick(0);
#endif

	return true;
}

bool CECModel::AddChildModel(
	const char* szHangerName,
	bool bOnBone,
	const char* szHookName,
	CECModel* pChild,
	const char* szCCName,
	const bool* pbSetHangerInheritScale/* = NULL*/,
	DWORD dwBindFlag/* = 0*/)
{
#ifdef _ANGELICA21

	if (pChild->m_pParentModel)
	{
		assert(false);

		char log[1024];
		sprintf(log, "CECModel::AddChildModel already has parent, this = %x, %I64x, %s, child = %x, %I64x, %s, old parent = %x, %I64x, %s",
			this,
			m_nId,
			GetFilePath(),
			pChild,
			pChild->m_nId,
			pChild->GetFilePath(),
			pChild->m_pParentModel,
			pChild->m_pParentModel->m_nId,
			pChild->m_pParentModel->GetFilePath()
			);

		a_LogOutput(1, log);
		return false;
	}

	{
		CECModel* pTempParent = m_pParentModel;

		while (pTempParent)
		{
			if (pTempParent == pChild)
			{
				char log[1024];
				sprintf(log, "CECModel::AddChildModel parent is child, this = %x, %I64x, %s, child = %x, %I64x, %s",
					this,
					m_nId,
					GetFilePath(),
					pChild,
					pChild->m_nId,
					pChild->GetFilePath()
					);

				a_LogOutput(1, log);
				return false;
			}

			pTempParent = pTempParent->m_pParentModel;
		}
	}

#endif

	if (pChild->m_pParentModel)
	{
		assert(false);

		char log[1024];
		sprintf(log, "CECModel::AddChildModel already has parent, this = %x, %I64x, %s, child = %x, %I64x, %s, old parent = %x, %I64x, %s",
			this,
			m_nId,
			GetFilePath(),
			pChild,
			pChild->m_nId,
			pChild->GetFilePath(),
			pChild->m_pParentModel,
			pChild->m_pParentModel->m_nId,
			pChild->m_pParentModel->GetFilePath()
			);

		a_LogOutput(1, log);
		return false;
	}

	if (m_ChildModels.find(szHangerName) != m_ChildModels.end())
		return false;

	if (m_pA3DSkinModel && pChild->m_pA3DSkinModel)
	{
		A3DSkinModelHanger* pSkmHanger = m_pA3DSkinModel->AddChildModel(
			szHangerName,
			bOnBone,
			szHookName,
			pChild->m_pA3DSkinModel,
			szCCName);

		if (!pSkmHanger)
		{
			char log[1024];
			sprintf(log, "CECModel::AddChildModel m_pA3DSkinModel->AddChildModel failed!, szHangerName: %s, bOnBone : %s, szHookName: %s, szCCName: %s",
				szHangerName, 
				bOnBone ? "true" : "false",
				szHookName,
				szCCName);

			a_LogOutput(1, log);
			return false;
		}

		if (pbSetHangerInheritScale)
			pSkmHanger->EnableInheritScale(*pbSetHangerInheritScale);

		pChild->m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);
		pChild->m_bLoadSkinModel = false;
	}

	pChild->SetParentModel(this);
	pChild->m_dwBindFlag = dwBindFlag;
	pChild->m_strHookName = szHookName;
	pChild->m_strCC = szCCName;
	pChild->SetGfxUseLOD(m_bGfxUseLod, false);
	pChild->SetDisableCamShake(m_bDisableCamShake);
	pChild->SetCreatedByGfx(m_bCreatedByGfx);

	if (pChild->InheritParentId())
		pChild->SetId(m_nId);

	pChild->RemoveMotionBlurInfo();
	m_ChildModels[szHangerName] = pChild;

	pChild->SetHideByHiddenHH(pChild->GetId() == m_nId && FindHideModelHH(pChild->m_strHookName));

	return true;
}

bool CECModel::RemoveChildModel(const char* szHangerName, bool bDel)
{
	if (!bDel) 
	{
		if (m_pA3DSkinModel)
			m_pA3DSkinModel->UnbindChildModel(szHangerName);
	}

	ECModelMap::iterator it = m_ChildModels.find(szHangerName);

	if (it == m_ChildModels.end())
		return false;

	CECModel* pChild = it->second;
	pChild->SetParentModel(NULL);
	pChild->m_dwBindFlag = 0;

	m_ChildPhyDelayMap.erase(pChild);

	if (bDel)
	{
		pChild->Release();
		delete pChild;
	}
	else
	{
		if (pChild->m_pA3DSkinModel)
			pChild->m_bLoadSkinModel = true;

		pChild->m_strHookName.Empty();
		pChild->m_strCC.Empty();
		CloseChildModelLink(szHangerName);

		pChild->SetHideByHiddenHH(false);
	}

	m_ChildModels.erase(it);

	// call this A3DSkinModel::RemoveChildModel function after pChild->Release()
	// for pChild->Release() will use information in A3DSkeleton
	// which would be released in A3DSkinModel::RemoveChildModel
	// otherwise, it will crash in pChild->Release()
	if (bDel && m_pA3DSkinModel)
	{
		m_pA3DSkinModel->RemoveChildModel(szHangerName);
	}

	return true;
}

bool CECModel::ChildChangeHook(const char* szHanger, const char* szNewHook)
{
	CECModel* pChild = GetChildModel(szHanger);

	if (!pChild)
		return false;

	if (stricmp(pChild->m_strHookName, szNewHook) == 0)
		return true;

	AString strCC = pChild->m_strCC;
	
	//	First copy a new link info array if the original one exists
	ChildModelLinkInfoArray aPhyslinks;
	for (ChildModelLinkInfoArray::iterator itr = pChild->m_PhysHookAttachLinks.begin()
		; itr != pChild->m_PhysHookAttachLinks.end()
		; ++itr)
		aPhyslinks.push_back(ChildModelLinkInfo(szNewHook, itr->m_strChildAttacher));

#ifdef A3D_PHYSX
	int nOldPhyState = A3DModelPhysSync::PHY_STATE_ANIMATION;
	A3DModelPhysics* pPhys = pChild->GetModelPhysics();
	if (pPhys)
	{
		nOldPhyState = pPhys->GetPhysState();
		if (nOldPhyState != A3DModelPhysSync::PHY_STATE_ANIMATION)
			pPhys->ChangePhysState(A3DModelPhysSync::PHY_STATE_ANIMATION);
	}
#endif

	bool bOldInheritScale = false;
	bool* pbSetInheritScale = NULL;

	if (m_pA3DSkinModel)
	{
		A3DHanger* pHanger = m_pA3DSkinModel->GetHanger(szHanger);
		if (pHanger)
		{
			bOldInheritScale = pHanger->GetInheritScaleFlag();
			pbSetInheritScale = &bOldInheritScale;
		}
	}

	RemoveChildModel(szHanger, false);

	if (!AddChildModel(szHanger, false, szNewHook, pChild, strCC, pbSetInheritScale))
	{
		pChild->Release();
		delete pChild;
		return false;
	}

	TryOpenChildModelLinks(szHanger, aPhyslinks);

#ifdef A3D_PHYSX
	if (pPhys && nOldPhyState != A3DModelPhysSync::PHY_STATE_ANIMATION)
	{
		pPhys->ChangePhysState(nOldPhyState);
		//m_ChildPhyDelayMap[pChild] = ChildPhyDelayInfo(MODEL_CHILD_PHYDELAY_TICK, nOldPhyState);
	}
#endif

	return true;
}

void CECModel::AddCoGfx(GFX_INFO* pInfo)
{
	AString strIndex;
	int i = m_pMapModel->m_CoGfxMap.size();
	
	while (true)
	{
		strIndex.Format("%d_co", i);
		
		if (m_pMapModel->m_CoGfxMap.find(strIndex) == m_pMapModel->m_CoGfxMap.end())
			break;
		
		i++;
	}

	if (pInfo)
		++s_nCoGfxCount;

	m_CoGfxMap[strIndex] = pInfo;

	m_pMapModel->m_CoGfxMap[strIndex] = static_cast<PGFX_INFO>(EVENT_INFO::CloneFrom(pInfo->GetComAct(), *pInfo));
}

void CECModel::DelCoGfx(GFX_INFO* pInfo)
{
	CoGfxMap::iterator it = m_CoGfxMap.begin();
	
	for (; it != m_CoGfxMap.end(); ++it)
	{
		if (it->second == pInfo)
		{
			delete m_pMapModel->m_CoGfxMap[it->first];
			m_pMapModel->m_CoGfxMap.erase(it->first);
			delete pInfo;

			if (pInfo)
				++s_nCoGfxCount;

			m_CoGfxMap.erase(it->first);
			return;
		}
	}
}

const char* CECModel::GetCoGfxName(GFX_INFO* pInfo)
{
	CoGfxMap::iterator it = m_CoGfxMap.begin();
	
	for (; it != m_CoGfxMap.end(); ++it)
	{
		if (it->second == pInfo)
			return it->first;
	}
	
	return NULL;
}

void CECModel::UpdateCoGfx(const char* strIndex, GFX_INFO* pSrc)
{
	if (!strIndex)
		return;

	CoGfxMap::iterator it = m_pMapModel->m_CoGfxMap.find(strIndex);
	
	if (it == m_pMapModel->m_CoGfxMap.end())
		return;
	
	PGFX_INFO pInfo = new GFX_INFO(NULL);
	pInfo->Clone(*pSrc);
	delete it->second;
	it->second = pInfo;
}

bool CECModel::ParentActivate(
	const AString& strActName,
	const ChildActInfo* pHostInfo,
	CECModel* pParentModel,
	DWORD dwDeltaTime)
{
	if (m_bKeepCurPose)
		return false;

	ASSERT( pParentModel == GetParentModel() && "Member Parent Model should be just the passed in one!!!" );
	if (pHostInfo->IsTrailAct() && stricmp(pHostInfo->GetHHName(), m_strHookName) != 0)
		return false;

	A3DCombinedAction* pComAct = GetComActByName(strActName);

	if (!pComAct)
		return false;

	BYTE rank = pComAct->GetRank(0);
	ChannelAct& ca = m_ChannelActs[0];
	ChannelActNode* pNode = ca.RemoveNodeByRank(rank);

	// 当前播放动作是要播放动作，且是无限的
	if (pNode
	&& pNode->m_pActive->GetComAct() == pComAct
	&& !pNode->m_pActive->IsActionStopped()
	&& pNode->m_pActive->GetComAct()->IsInfinite())
	{
		ca.m_RankNodes.AddTail(pNode);
		return true;
	}

	delete pNode;
	pNode = new ChannelActNode();
	pNode->m_Rank = rank;
	ca.m_RankNodes.AddTail(pNode);

	pNode->m_pActive = new A3DCombActDynData(pComAct, this);
	pNode->m_pActive->SetParentActInfo(pHostInfo, pParentModel, dwDeltaTime);
	pNode->m_pActive->Play(0, 1.f, pHostInfo->GetTransTime(), m_EventMasks[0], false);

	return true;
}

void CECModel::ActivateChildAct(
	const AString& strActName,
	const ChildActInfo* pHostInfo,
	CECModel* pParentModel,
	DWORD dwDeltaTime)
{
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		if (it->second->m_bCanAffectedByParent)
			it->second->ParentActivate(
				strActName,
				pHostInfo,
				pParentModel,
				dwDeltaTime);
}

void CECModel::UpdateFadeOutSfxLst(DWORD dwDelta)
{
	ALISTPOSITION pos = m_FadeOutSfxLst.GetHeadPosition();
	AM3DSoundBuffer* pSfx;

	while (pos)
	{
		ALISTPOSITION posCur = pos;
		pSfx = m_FadeOutSfxLst.GetNext(pos);
		pSfx->CheckEnd();

#ifdef _ANGELICA21
		if (pSfx->IsStopped() || pSfx->IsToRelease())
#else
		if (pSfx->IsStopped())
#endif
		{
			AfxReleaseSoundNonLoop(pSfx);
			m_FadeOutSfxLst.RemoveAt(posCur);
		}
	}
}

bool CECModel::IsECMHookValid(const ECModelHookMap& hookMap, const char* szName) const
{
	// already exist one with this name
	if (hookMap.find(szName) != hookMap.end())
		return false;

	//	exist a A3DSkeletonHook with this name
	if (m_pA3DSkinModel && m_pA3DSkinModel->GetSkeleton() && m_pA3DSkinModel->GetSkeletonHook(szName, true))
		return false;

	return true;
}

bool CECModel::AddECMHook(const char* szName, int nID, float fScale, const A3DVECTOR3& vPos)
{
	if (!m_pMapModel)
		return false;

	ECModelHookMap& hookMap = m_pMapModel->m_ECModelHookMap;
	
	// check the name is valid
	if (!IsECMHookValid(hookMap, szName))
		return false;

	CECModelHook* pNewHook = new CECModelHook;
	pNewHook->SetName(szName);
	pNewHook->SetRelativeMatrix(a3d_Translate(vPos.x, vPos.y, vPos.z));
	pNewHook->SetID(nID);
	pNewHook->SetScaleFactor(fScale);
	hookMap[szName] = pNewHook;
	return true;
}

bool CECModel::RenameECMHook(const char* szOldName, const char* szNewName)
{
	if (!m_pMapModel)
		return false;

	ECModelHookMap& hookMap = m_pMapModel->m_ECModelHookMap;
	
	// already exist one with this name
	if (!IsECMHookValid(hookMap, szNewName))
		return false;

	CECModelHook* pHook = GetECMHook(szOldName);
	pHook->SetName(szNewName);
	return true;
}

void CECModel::RemoveECMHook(const char* szName)
{
	if (!m_pMapModel)
		return;

	ECModelHookMap& hookMap = m_pMapModel->m_ECModelHookMap;
	ECModelHookMap::iterator itr = hookMap.find(szName);
	if (itr != hookMap.end()) {
		delete itr->second;
		hookMap.erase(itr);
	}
}

int CECModel::GetECMHookCount() const
{
	if (!m_pMapModel)
		return 0;

	return m_pMapModel->m_ECModelHookMap.size();
}

CECModelHook* CECModel::GetECMHook(int iIndex) const
{
	ECModelHookMap& hookMap = m_pMapModel->m_ECModelHookMap;
	ECModelHookMap::iterator it = hookMap.begin();
	while (iIndex-- && it != hookMap.end()) ++it;
	if (it == hookMap.end())
		return NULL;
	return it->second;
}

CECModelHook* CECModel::GetECMHook(const char* szName) const
{
	if (!m_pMapModel)
		return NULL;

	ECModelHookMap& hookMap = m_pMapModel->m_ECModelHookMap;
	ECModelHookMap::iterator citr = hookMap.find(szName);
	if (citr == hookMap.end())
		return NULL;

	return citr->second;
}

//	Inner help functions for gfx property transition
void CECModel::TransferEcmProperties(A3DGFXEx* pGfx)
{
	ASSERT(pGfx);

	pGfx->SetDisableCamShake(m_bDisableCamShake);
	pGfx->SetEnableCamBlur(m_bEnableCamBlur);
	pGfx->SetUseLOD(m_bGfxUseLod);
	pGfx->SetCreatedByGFXECM(m_bCreatedByGfx);
	pGfx->SetId(GetId(), reinterpret_cast<modelh_t>(this));
}

void CECModel::PlayGfx(const char* szPath, const char* szHook, float fScale, bool bFadeOut, bool bUseECMHook, DWORD dwFadeOutTime)
{
	if (!bFadeOut)
		dwFadeOutTime = 0;

	PGFX_INFO pInfo;
	AString strKey = szPath;
	strKey += szHook;
	CoGfxMap::iterator it =	m_CoGfxMap.find(strKey);

	if (bUseECMHook) {
		if (CECModelHook* pHook = GetECMHook(szHook))
		{
			fScale *= pHook->GetScaleFactor();
		}
	}

	if (it != m_CoGfxMap.end())
	{
		pInfo = it->second;
		A3DGFXEx* pGfx = pInfo->GetGfx();
		TransferEcmProperties(pGfx);
		pGfx->SetScale(fScale * m_fGfxScaleFactor);
		pInfo->SetFadeOutTime(dwFadeOutTime);
		pInfo->SetModelAlpha(true);
		pInfo->SetHookName(szHook);
		pInfo->SetUseECMHook(bUseECMHook);
		pInfo->SetScale(fScale);
		pGfx->SetSfxPriority(m_nSfxPriority);
		pGfx->Start(true);
		return;
	}

	pInfo = new GFX_INFO(NULL);
	pInfo->SetFilePath(szPath);
	pInfo->SetHookName(szHook);
	pInfo->SetUseECMHook(bUseECMHook);
	pInfo->SetScale(fScale);
	pInfo->Init(m_pA3DDevice);
	pInfo->LoadGfx();

	if (!pInfo->GetGfx())
	{
		delete pInfo;
		return;
	}

	pInfo->BuildTranMat();
	pInfo->SetFadeOutTime(dwFadeOutTime);
	pInfo->SetModelAlpha(true);
	pInfo->GetGfx()->SetScale(fScale * m_fGfxScaleFactor);
	pInfo->GetGfx()->SetSfxPriority(m_nSfxPriority);
	TransferEcmProperties(pInfo->GetGfx());
	pInfo->GetGfx()->Start(true);

	++s_nCoGfxCount;
	m_CoGfxMap[strKey] = pInfo;
}

void CECModel::PlayGfx(const char* szPath, const char* szHook, float fScale, bool bFadeOut, const A3DVECTOR3& vOffset, float fPitch, float fYaw, float fRot, bool bUseECMHook, DWORD dwFadeOutTime)
{
	if (!bFadeOut)
		dwFadeOutTime = 0;

	PGFX_INFO pInfo;
	AString strKey = szPath;
	strKey += szHook;
	CoGfxMap::iterator it =	m_CoGfxMap.find(strKey);
	if (bUseECMHook) {
		if (CECModelHook* pHook = GetECMHook(szHook))
		{
			fScale *= pHook->GetScaleFactor();
		}
	}
	
	if (it != m_CoGfxMap.end())
	{
		pInfo = it->second;
		A3DGFXEx* pGfx = pInfo->GetGfx();
		TransferEcmProperties(pGfx);
		pGfx->SetScale(fScale * m_fGfxScaleFactor);
		pInfo->SetFadeOutTime(dwFadeOutTime);
		pInfo->SetModelAlpha(true);
		pInfo->SetHookName(szHook);
		pInfo->SetUseECMHook(bUseECMHook);
		pInfo->SetScale(fScale);
		pInfo->SetOffset(vOffset);
		pInfo->SetPitch(fPitch);
		pInfo->SetYaw(fYaw);
		pInfo->SetRot(fRot);
		pInfo->BuildTranMat();
		pGfx->SetSfxPriority(m_nSfxPriority);
		pGfx->Start(true);
		return;
	}

	pInfo = new GFX_INFO(NULL);
	pInfo->SetFilePath(szPath);
	pInfo->SetHookName(szHook);
	pInfo->SetUseECMHook(bUseECMHook);
	pInfo->SetScale(fScale);
	pInfo->Init(m_pA3DDevice);
	pInfo->SetOffset(vOffset);
	pInfo->SetPitch(fPitch);
	pInfo->SetYaw(fYaw);
	pInfo->SetRot(fRot);
	pInfo->LoadGfx();

	if (!pInfo->GetGfx())
	{
		delete pInfo;
		return;
	}

	pInfo->BuildTranMat();
	pInfo->SetFadeOutTime(dwFadeOutTime);
	pInfo->SetModelAlpha(true);
	TransferEcmProperties(pInfo->GetGfx());
	pInfo->GetGfx()->SetScale(fScale * m_fGfxScaleFactor);
	pInfo->GetGfx()->SetSfxPriority(m_nSfxPriority);
	pInfo->GetGfx()->Start(true);

	++s_nCoGfxCount;
	m_CoGfxMap[strKey] = pInfo;
}

void CECModel::RemoveGfx(const char* szPath, const char* szHook)
{
	AString strKey = szPath;
	strKey += szHook;
	RemoveGfx(strKey);
}

void CECModel::RemoveGfx(const char* szKey)
{
	CoGfxMap::iterator it = m_CoGfxMap.find(szKey);
	if (it == m_CoGfxMap.end())
		return;

	delete it->second;
	m_CoGfxMap.erase(szKey);

	--s_nCoGfxCount;
}

A3DGFXEx* CECModel::GetGfx(const char* szPath, const char* szHook)
{
	AString strKey = szPath;
	strKey += szHook;

	CoGfxMap::iterator it = m_CoGfxMap.find(strKey);
	if (it == m_CoGfxMap.end())
		return NULL;
	else
		return it->second->GetGfx();
}

bool CECModel::IsActionStopped(int nChannel)
{
	assert(nChannel >= 0 && nChannel < A3DSkinModel::ACTCHA_MAX);
	ChannelAct& ca = m_ChannelActs[nChannel];
	ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();
	
	while (pos)
	{
		ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);
		
		if (!pNode->m_pActive->IsActionStopped())
			return false;
	}
	
	return true;
}

void CECModel::UpdateCoGfxHook()
{
	CoGfxMap::iterator it = m_CoGfxMap.begin();

	for (; it != m_CoGfxMap.end(); ++it)
	{
		PGFX_INFO pInfo = it->second;
		pInfo->Init(m_pA3DDevice);
		pInfo->LoadGfx();

		if (!pInfo->GetGfx())
			continue;

		A3DGFXEx* pGfx = pInfo->GetGfx();
		pGfx->SetScale(pInfo->GetScale() * m_fGfxScaleFactor);
		pGfx->SetAlpha(pInfo->GetAlpha());
		pGfx->SetPlaySpeed(pInfo->GetPlaySpeed());
		pGfx->SetSfxPriority(m_nSfxPriority);
		TransferEcmProperties(pGfx);
		pGfx->Start(true);
	}
}

void CECModel::SetGfxUseLOD(bool b, bool bForce)
{
	if (m_bGfxUseLod == b && !bForce)
		return;

	m_bGfxUseLod = b;
	CoGfxMap::iterator itGfx = m_CoGfxMap.begin();

	for (; itGfx != m_CoGfxMap.end(); ++itGfx)
	{
		PGFX_INFO pInfo = itGfx->second;
		A3DGFXEx* pGfx = pInfo->GetGfx();

		if (pGfx)
			pGfx->SetUseLOD(b);
	}

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetGfxUseLOD(b, bForce);
}

void CECModel::SetDisableCamShake(bool b)
{ 
	if (m_bDisableCamShake == b)
		return;

	m_bDisableCamShake = b; 

	// DO NOT set disable camera shake property for GFX that is already playing
	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		it->second->SetDisableCamShake(b);
}

void CECModel::SetEnableCamBlur(bool b)
{ 
	if (m_bEnableCamBlur == b)
		return;

	m_bEnableCamBlur = b;
}

//	Get current action's user data
DWORD CECModel::GetCurActionUserData(int nChannel)
{
	assert(nChannel >= 0 && nChannel < A3DSkinModel::ACTCHA_MAX);
	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = ca.GetHighestRankNode();

	if (pNode)
		return pNode->m_pActive->GetUserData();

	return (DWORD)(-1);
}

inline bool _is_vec_nan(const A3DVECTOR3& v)
{
	return _isnan(v.x) || _isnan(v.y) || _isnan(v.z);
}

//	Set absolute position
void CECModel::SetPos(const A3DVECTOR3& vPos)
{
	if (_is_vec_nan(vPos))
	{
		assert(false);
		return;
	}

	A3DCoordinate::SetPos(vPos);

	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetPos(vPos);
}

//	Set absolute forward and up direction
void CECModel::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if (_is_vec_nan(vDir) || _is_vec_nan(vUp))
	{
		assert(false);
		return;
	}

	A3DCoordinate::SetDirAndUp(vDir, vUp);

	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetDirAndUp(vDir, vUp);
}

//	Set absolute transform matrix
void CECModel::SetAbsoluteTM(const A3DMATRIX4& mat)
{
	A3DCoordinate::SetAbsoluteTM(mat);

	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetAbsoluteTM(mat);
}

//	Set aabb type
void CECModel::SetAABBType(int iType)
{
	m_iAABBType = iType;

	if (m_pA3DSkinModel)
	{
		if (iType == AABB_SKELETON)
			m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_SKELETON);
		else if (iType == AABB_INITMESH)
			m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);
		else
		{
			int iNumBone = m_pA3DSkinModel->GetSkeleton()->GetBoneNum();
			const A3DAABB& aabb = m_pA3DSkinModel->GetModelAABB();
			if (iNumBone >= 10)
				m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_SKELETON);
			else if (iNumBone < 3 || aabb.Extents.x < 0.6f || aabb.Extents.y < 0.6f || aabb.Extents.z < 0.6f)
				m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);
			else
				m_pA3DSkinModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_SKELETON);
		}
	}
}

const A3DAABB& CECModel::GetModelAABB() const
{
	if (m_pA3DSkinModel)
		return m_ModelAABB;
	else
		return m_pMapModel->GetCHAABB();
}

//	Build wound action channel
bool CECModel::BuildWoundActionChannel()
{
	if (!m_pA3DSkinModel)
		return false;

	A3DSkeleton* pSkeleton = m_pA3DSkinModel->GetSkeleton();
	int i, iNumJoint = pSkeleton->GetJointNum();

	int* aJoints = new int [iNumJoint];
	if (!aJoints)
		return false;

	for (i=0; i < iNumJoint; i++)
		aJoints[i] = i;

	A3DSMActionChannel* pChannel = m_pA3DSkinModel->BuildActionChannel(ACTCHA_WOUND, iNumJoint, aJoints);

	if (pChannel)
		pChannel->SetPlayMode(A3DSMActionChannel::PLAY_COMBINE);

	delete [] aJoints;

	return pChannel ? true : false;
}

A3DModelPhysSync* CECModel::GetPhysSync() const
{
#ifdef A3D_PHYSX
	return m_pModelPhysics ? m_pModelPhysics->GetModelSync() : NULL;
#else
	return NULL;
#endif
}

bool CECModel::IsInPartSimulate() const
{
#ifdef A3D_PHYSX
	return GetPhysSync() ? GetPhysSync()->GetPhysState() == A3DModelPhysSync::PHY_STATE_PARTSIMULATE : false;
#endif
	return false;
}

CECModelPhysBlendOverlap* CECModel::CreatePhysBlendOverlap(int iBlendMode, const char* szForceBone,
	float fTouchRadius, float fMaxPhysWeight, float fForceValue /* = 2.f */,
	DWORD dwFadeInTime /* = 200 */, DWORD dwHoldTime /* = 0 */, DWORD dwFadeOutTime /* = 300 */)
{
	if (m_pOverlapBlend)
		ASSERT(m_pOverlapBlend->GetHostModel() == this);

	CECModelPhysBlendOverlap::BLEND_MODE eBlendMode = CECModelPhysBlendOverlap::BLEND_MODE(iBlendMode);
	if (!m_pOverlapBlend)
	{
		m_pOverlapBlend = new CECModelPhysBlendOverlap(this, eBlendMode, fTouchRadius, fMaxPhysWeight, fForceValue);
	}
	else
	{
		m_pOverlapBlend->SetBlendMode(eBlendMode);
		m_pOverlapBlend->SetHostTouchRadius(fTouchRadius);
		m_pOverlapBlend->SetMaxPhysWeight(fMaxPhysWeight);
		m_pOverlapBlend->SetForceValue(fForceValue);
	}

	m_pOverlapBlend->SetForceBone(szForceBone);
	m_pOverlapBlend->SetFadeTimes(dwFadeInTime, dwHoldTime, dwFadeOutTime);

	return m_pOverlapBlend;
}

bool CECModel::SyncModelPhys(DWORD dwDeltaTime)
{
	g_nECMDeadlockFlag = 0;
	bool bRes = true;
#ifdef A3D_PHYSX

	if (m_pModelPhysics)
	{
		g_nECMDeadlockFlag = 100;
		bRes = m_pModelPhysics->SyncAfterPhysXSim();
	}

	// Sync all child models' physics
	g_nECMDeadlockFlag = 101;
	for (ECModelMap::iterator it2 = m_ChildModels.begin(); it2 != m_ChildModels.end(); ++it2)
	{
		CECModel* pChild = it2->second;

		if (pChild->IsShown())
			pChild->SyncModelPhys(dwDeltaTime);
	}

	// update model's AABB in case of physics driven skeleton
	if (GetPhysSync() && !GetPhysSync()->IsFullAnimation())
	{
		g_nECMDeadlockFlag = 102;
		m_pA3DSkinModel->UpdateHitBoxes(0);
		UpdateModelAABB();
		m_aabbUsedToCullInRender = m_ModelAABB;
	}

	if (m_bPhysEnabled && GetModelPhysics())
	{
		g_nECMDeadlockFlag = 103;
#ifdef _ANGELICA21
		DWORD dwUpdateTime, dwActionUpdateTime, dwEventUpdateTime;
		GetTickTimes(dwDeltaTime, false, &dwUpdateTime, &dwActionUpdateTime, &dwEventUpdateTime);
		TickRelyOnPhysics(dwUpdateTime, dwActionUpdateTime, dwEventUpdateTime);
#endif

		// handle the hit ground call back...
		g_nECMDeadlockFlag = 104;
		if (m_pPhysHitGroundHandler && !m_arrPhysXHitGroundInfo.empty())
			(m_pPhysHitGroundHandler)(this, m_arrPhysXHitGroundInfo);
	}

#endif

	g_nECMDeadlockFlag = 0;
	return bRes;
}

//	Resize physical actors and clothes after model size changed
bool CECModel::ResizePhysObjects()
{
#ifdef A3D_PHYSX

	if (!m_bPhysEnabled)
		return true;

	if (!m_pModelPhysics)
		return false;

	struct SKIN_STATE
	{
		bool	bCloth;		//	Changed to cloth
		int		iLinkType;	//	Link type
	};

	//	Record what skins have been changed to cloth and their link type
	SKIN_STATE aSkinStates[256];
	memset(aSkinStates, 0, sizeof aSkinStates);

	int i, iNumSkin = m_pModelPhysics->GetSkinSlotNum();
	ASSERT(iNumSkin <= 256);

	for (i=0; i < iNumSkin; i++)
	{
		if (m_pModelPhysics->IsClothSkin(i))
			aSkinStates[i].bCloth = true;

		aSkinStates[i].iLinkType = m_pModelPhysics->GetClothLinkType(i);
	}

	//	Cloth all clothes
	CloseAllClothes(true);

	//	Create model physics
	if (!m_pModelPhysics->CreateModelSync(m_pMapModel->m_pPhysSyncData))
		return false;

	//	Open all clothes
	for (i=0; i < iNumSkin; i++)
	{
		const SKIN_STATE& st = aSkinStates[i];
		if (st.bCloth)
			m_pModelPhysics->OpenClothSkin(i, st.iLinkType);
	}

#endif	//	A3D_PHYSX

	return true;
}

//	Remove combo model
void CECModel::RemoveComboModel(CECModel* pModel)
{
	int n = FindComboModel(pModel);
	if (n >= 0)
		RemoveComboModelByIndex(n);
}

void CECModel::ReloadPixelShader()
{
	if (m_pPixelShader)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseShader(&m_pPixelShader);
		m_pPixelShader = NULL;
	}

	if (!m_pMapModel->m_strPixelShader.IsEmpty())
	{
		m_pPixelShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile(m_pMapModel->m_strPixelShader);

		if (m_pPixelShader)
		{
#ifdef _ANGELICA21
			if (!m_pMapModel->m_strShaderTex.IsEmpty())
			{
				AString strPath = "Gfx\\Textures\\" + m_pMapModel->m_strShaderTex;
				m_pPixelShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)(const char*)strPath);
			}
#else
			if (!m_pMapModel->m_strShaderTex.IsEmpty())
			{
				AString strPath = "Gfx\\Textures\\" + m_pMapModel->m_strShaderTex;
				m_pPixelShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)(const char*)strPath);
			}
#endif
			m_dwPSTickTime = 0;
		}
	}
}

void CECModel::SetChildRenderFlag(DWORD dwFlag)
{
#ifdef _ANGELICA21

	m_dwChildRenderFlag = dwFlag;

	if (m_pA3DSkinModel)
		m_pA3DSkinModel->SetChildRenderFlag(dwFlag);

#endif
}

void CECModel::FinalizeCustomRenderFunc()
{
#ifdef _ANGELICA21
	// a3d_FinalizeCustomRender();
#endif
}

//	Record track bone velocity
//	记录一系列骨骼在dwWindowTimeMs内的运动速度
//	vecBone: 设置的骨骼名列表
//	bClearUnUsedBones: 是否清除当前在vecBone中未用到的Bone
//	dwWindowTimeMs: 记录的窗口时间。最大值1000
bool CECModel::SetTrackBones(const abase::vector<AString>& vecBone, bool bClearUnUsedBones, DWORD dwWindowTimeMs /* = 300 */)
{
	if (dwWindowTimeMs > 1000)
		dwWindowTimeMs = 1000;

	if (!m_pTrackBoneVelocity)
	{
		A3DSkinModel* pSkinModel = GetA3DSkinModel();
		A3DSkeleton* pSkeleton = pSkinModel ? pSkinModel->GetSkeleton() : NULL;
		if (!pSkeleton)
			return false;

		m_pTrackBoneVelocity = new TrackBoneVelocity(m_pA3DDevice, pSkeleton, dwWindowTimeMs);
		return m_pTrackBoneVelocity->SetTrackBones(vecBone, bClearUnUsedBones);
	}
	else
	{
		m_pTrackBoneVelocity->SetWindowTime(dwWindowTimeMs);
		return m_pTrackBoneVelocity->SetTrackBones(vecBone, bClearUnUsedBones);
	}
}

//	Get current trackbone size
size_t CECModel::GetTrackBoneCount() const
{
	if (!m_pTrackBoneVelocity)
		return 0;

	return m_pTrackBoneVelocity->GetAllTrackBoneInfo().size();
}

//	Track bone vel mode
bool CECModel::SetTrackBoneVelMode(TRACKBONEVEL_MODE mode)
{
	if (!m_pTrackBoneVelocity
		|| mode <= TRACKBONEVEL_MODE_NONE
		|| mode >= TRACKBONEVEL_MODE_COUNT)
		return false;

	m_pTrackBoneVelocity->SetMode(mode);
	return true;
}

CECModel::TRACKBONEVEL_MODE CECModel::GetTrackBoneVelMode() const
{
	if (!m_pTrackBoneVelocity)
		return CECModel::TRACKBONEVEL_MODE_NONE;

	return m_pTrackBoneVelocity->GetMode();
}

bool CECModel::StartTrackBoneVel(DWORD dwWorkDuration)
{
	if (!m_pTrackBoneVelocity)
		return false;

	m_pTrackBoneVelocity->Start(dwWorkDuration);
	return true;
}
bool CECModel::StopTrackBoneVel(bool bClear)
{
	if (!m_pTrackBoneVelocity)
		return false;

	m_pTrackBoneVelocity->Stop();

	if (bClear)
		m_pTrackBoneVelocity->ClearTrackBoneVelocity();

	return true;
}

//	Is TrackBoneVel Working?
bool CECModel::IsTrackBoneVelWorking() const
{
	if (!m_pTrackBoneVelocity)
		return false;

	return m_pTrackBoneVelocity->IsWorking();
}

//	Get velocity of specified bone
const A3DBone* CECModel::GetTrackBoneVelocity(A3DVECTOR3& vel, const char* szBoneName) const
{
	if (!m_pTrackBoneVelocity)
		return NULL;

	return m_pTrackBoneVelocity->GetTrackBoneVelocity(vel, szBoneName);
}

bool CECModel::HasCHAABB() const
{
	if(!m_pMapModel)
		return false;
	return m_pMapModel->HasCHAABB();
}


inline A3DQUATERNION _quat_from_dir_up(const A3DVECTOR3& vDir)
{
	A3DVECTOR3 vRight = CrossProduct(A3D::g_vAxisY, vDir);
	A3DVECTOR3 vRealUp = CrossProduct(vDir, vRight);
	vRight.Normalize();
	vRealUp.Normalize();
	A3DMATRIX4 mat;
	mat.SetRow(0, vRight);
	mat.SetRow(1, vRealUp);
	mat.SetRow(2, vDir);
	mat.SetRow(3, A3DVECTOR3(0));
	mat.SetCol(3, A3DVECTOR3(0));
	mat.m[3][3] = 1.0f;
	return A3DQUATERNION(mat);
}

void CECModel::LIGHT_TRANS_INFO::Update(DWORD dwTickTime, const A3DVECTOR3& vPos)
{
	static const DWORD _total_trans_time = 1000;

	if (m_pLightInfo2)
	{
		m_dwCurTransTime += dwTickTime;

		if (m_dwCurTransTime >= _total_trans_time)
		{
			*m_pLightInfo1 = *m_pLightInfo2;
			m_dwLightId1 = m_dwLightId2;
			delete m_pLightInfo2;
			m_pLightInfo2 = 0;
			m_dwLightId2 = 0;
			delete m_pCurLightInfo;
			m_pCurLightInfo = 0;
			m_dwCurTransTime = 0;
		}
		else
		{
			float fRatio = m_dwCurTransTime / (float)_total_trans_time;
			float fr = 1.f - fRatio;
			A3DVECTOR3 vDir1 = m_pLightInfo1->vPtLightPos - vPos;
			A3DVECTOR3 vDir2 = m_pLightInfo2->vPtLightPos - vPos;
			float fDist1 = vDir1.Normalize();
			float fDist2 = vDir2.Normalize();
			A3DQUATERNION q1 = _quat_from_dir_up(vDir1);
			A3DQUATERNION q2 = _quat_from_dir_up(vDir2);
			A3DQUATERNION q = Slerp(fRatio, q1, q2, true);
			A3DVECTOR3 vIntDir = RotateVec(q, A3D::g_vAxisZ);
			float fIntDist = fDist1 * fr + fDist2 * fRatio;
			m_pCurLightInfo->colPtAmb = m_pLightInfo1->colPtAmb * fr + m_pLightInfo2->colPtAmb * fRatio;
			m_pCurLightInfo->colPtDiff = m_pLightInfo1->colPtDiff * fr + m_pLightInfo2->colPtDiff * fRatio;
			m_pCurLightInfo->fPtRange = m_pLightInfo1->fPtRange * fr + m_pLightInfo2->fPtRange * fRatio;
			m_pCurLightInfo->vPtAtten = m_pLightInfo1->vPtAtten * fr + m_pLightInfo2->vPtAtten * fRatio;
			m_pCurLightInfo->vPtLightPos = vPos + vIntDir * fIntDist;
		}
	}
}

void CECModel::LIGHT_TRANS_INFO::CheckCurLight(A3DSkinModel* pSkinModel, DWORD dwLightId, A3DSkinModel::LIGHTINFO& info)
{
	if (m_pLightInfo2) // 过渡中，忽略新光源
	{
		pSkinModel->SetLightInfo(*m_pCurLightInfo);
	}
	else if (m_dwLightId1 == 0) // 没有当前光源
	{
		if (dwLightId && info.bPtLight)
		{
			m_dwLightId1 = dwLightId;
			m_pLightInfo1 = new A3DSkinModel::LIGHTINFO;
			*m_pLightInfo1 = info;
		}

		pSkinModel->SetLightInfo(info);
	}
	else if (m_dwLightId1 == dwLightId) // 与当前光源相同
	{
		*m_pLightInfo1 = info;
		pSkinModel->SetLightInfo(info);
	}
	else // 与当前光源不同
	{
		if (dwLightId && info.bPtLight)
		{
			m_dwLightId2 = dwLightId;
			m_pLightInfo2 = new A3DSkinModel::LIGHTINFO;
			*m_pLightInfo2 = info;
			m_pCurLightInfo = new A3DSkinModel::LIGHTINFO;
			m_dwCurTransTime = 0;
			*m_pCurLightInfo = *m_pLightInfo1;
			pSkinModel->SetLightInfo(*m_pLightInfo1);
		}
		else
		{
			delete m_pLightInfo1;
			m_pLightInfo1 = 0;
			m_dwLightId1 = 0;
			pSkinModel->SetLightInfo(info);
		}
	}
}

bool CECModel::RemoveChildByPtr(CECModel* pChildToRemove, AString* pHangerOut/* = NULL*/)
{
	ECModelMap::iterator child_it = m_ChildModels.begin();

	for (; child_it != m_ChildModels.end(); ++child_it)
	{
		CECModel* pChild = child_it->second;

		if (pChildToRemove == pChild)
		{
			if (pHangerOut) *pHangerOut = child_it->first;
			RemoveChildModel(child_it->first, false);
			return true;
		}
	}

	return false;
}

void CECModel::UnbindOuterChildren()
{
	ECModelMap::iterator child_it = m_ChildModels.begin();

	while (child_it != m_ChildModels.end())
	{
		CECModel* pChild = child_it->second;

		if ((pChild->m_dwBindFlag & ECM_BIND_DONT_DEL_BY_PARENT) != 0)
		{
			pChild->SetParentModel(NULL);
			pChild->m_dwBindFlag = 0;
			child_it = m_ChildModels.erase(child_it);
		}
		else
		{
			++child_it;
		}
	}	
}

void CECModel::UnbindParent()
{
	if (m_pParentModel)
	{
		m_pParentModel->RemoveChildByPtr(this);
	}
}

void CECModel::CloseOnBoneChildCloth(bool bRecursive /*= false*/)
{
	if(m_pA3DSkinModel)
	{
		for(int i=0; i<m_pA3DSkinModel->GetHangerNum(); i++)
		{
			A3DHanger* pHanger = m_pA3DSkinModel->GetHanger(i);
			CECModel* pChild = GetChildModel(pHanger->GetName());
			if(pChild && pHanger->IsBoundOnBone())
			{
				pChild->CloseAllClothes();
			}
		}
	}

	if(bRecursive)
	{
		for(int i=0; i<GetChildCount(); i++)
		{
			CECModel* pChild = GetChildModel(i);
			pChild->CloseOnBoneChildCloth(true);
		}
	}
}

void CECModel::OpenOnBoneChildCloth(bool bRecursive /*= false*/)
{
	if(m_pA3DSkinModel)
	{
		for(int i=0; i<m_pA3DSkinModel->GetHangerNum(); i++)
		{
			A3DHanger* pHanger = m_pA3DSkinModel->GetHanger(i);
			CECModel* pChild = GetChildModel(pHanger->GetName());
			if(pChild && pHanger->IsBoundOnBone())
			{
				const ClothSimDesc& paras = pChild->GetClothSimParameters();
				pChild->OpenAllClothes(paras.iLinkType, paras.iDelayTime, paras.iBlendTime);
			}
		}
	}

	if(bRecursive)
	{
		for(int i=0; i<GetChildCount(); i++)
		{
			CECModel* pChild = GetChildModel(i);
			pChild->OpenOnBoneChildCloth(true);
		}
	}

}

CECModelStaticData::CECModelStaticData() :
m_bActMapped(false),
m_bAutoUpdate(true),
m_dwVersion(0),
m_OrgColor(A3DCOLORRGBA(255, 255, 255, 255)),
m_EmissiveColor(A3DCOLORRGBA(0, 0, 0, 0)),
m_fDefPlaySpeed(1.0f),
m_bCanCastShadow(true),
m_bRenderSkinModel(true),
m_bRenderEdge(true),
m_bChannelInfoInit(false),
m_bInitGlobalScript(false),
m_bMphyLoaded(false),
m_nHumanbingSkeleton(-1)
{
	int i;
	for (i = 0; i < OUTER_DATA_COUNT; i++)
		m_OuterData[i] = 1.0f;

	m_BlendMode.SrcBlend = A3DBLEND_SRCALPHA;
	m_BlendMode.DestBlend = A3DBLEND_INVSRCALPHA;
	m_CHAABB.Clear();
	memset(m_ScriptEnable, 0, sizeof(m_ScriptEnable));
	memset(m_ChannelInfoArr, 0, sizeof(m_ChannelInfoArr));
	memset(m_EventMasks, -1, sizeof(m_EventMasks));
	for (i = 0; i < AUDIOEVENT_GROUP_COUNT; ++i)
		m_bAudioGroupEnable[i] = true;

	m_pPhysSyncData = NULL;
}

bool CECModelStaticData::LoadData(const char* szModelFile, bool bLoadAdditionalSkin, bool bLoadConvexHull)
{
	if (szModelFile[0] == '\0')
		return false;

	AFileImage file;

	if (!file.Open(szModelFile, AFILE_OPENEXIST | MODEL_FILE_TYPE | AFILE_TEMPMEMORY))
	{
		a_LogOutput(1, "%s, Failed to open ecm file %s", "CECModelStaticData::LoadData", szModelFile);
		return false;
	}

#ifdef GFX_EDITOR

	Release();

#endif

	A3DDevice* pA3DDevice = AfxGetA3DDevice();
	m_strFilePath = szModelFile;
	AString strECMTable = _format_ecm_table_name(this);
	m_ScriptMemTbl.Init(strECMTable);

	DWORD dwReadLen;
	int i;
	int nComActCount	= 0;
	int nCoGfxNum		= 0;
	int nBoneScaleNum	= 0;
	int nOuterDataNum	= 0;

	if (file.IsBinary())
	{
		file.Read(&m_dwVersion, sizeof(m_dwVersion), &dwReadLen);
		file.ReadString(m_strSkinModelPath);
		file.Read(&m_OrgColor, sizeof(m_OrgColor), &dwReadLen);
		file.Read(&m_BlendMode, sizeof(m_BlendMode), &dwReadLen);

		file.Read(&nOuterDataNum, sizeof(nOuterDataNum), &dwReadLen);
		for (i = 0; i < nOuterDataNum; i++)
			file.Read(&m_OuterData[i], sizeof(m_OuterData[i]), &dwReadLen);

		file.Read(&nBoneScaleNum, sizeof(nBoneScaleNum), &dwReadLen);
		for (i = 0; i < nBoneScaleNum; i++)
		{
			BoneScale* pBoneScale = new BoneScale;
			file.Read(&pBoneScale->m_nIndex, sizeof(pBoneScale->m_nIndex), &dwReadLen);
			file.Read(&pBoneScale->m_nType, sizeof(pBoneScale->m_nType), &dwReadLen);
			file.Read(&pBoneScale->m_vScale.m, sizeof(pBoneScale->m_vScale.m), &dwReadLen);
		}

		file.Read(&nCoGfxNum, sizeof(nCoGfxNum), &dwReadLen);
		file.Read(&nComActCount, sizeof(nComActCount), &dwReadLen);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		char	szBuf[AFILE_LINEMAXLEN];

		file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_file_version, &m_dwVersion);

		// Version check is necessary in Client
		if (m_dwVersion > EC_MODEL_CUR_VERSION)
		{
			a_LogOutput(1, "%s, Wrong file version %u (%u or lower required).", "CECModelStaticData::LoadData", m_dwVersion, EC_MODEL_CUR_VERSION);
			m_dwVersion = (DWORD)-1;
			file.Close();
			return false;
		}

		// Notion: some files of version 37 would get a single file name here
		// This is because we tried to save only file name in this version which is proved inconsistent with the Project of ReWuPaiDui.
		// As we changed back to save the relative pathname and process as if all files have saved with the pathname.
		// Some file of version 37 would be opened failed and need a fix operation in the ECM editor.
		szBuf[0] = '\0';
		file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_skinmodel_path, szBuf);
		m_strSkinModelPath = szBuf;

		if (m_dwVersion >= 33)
		{
			int nBool;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_autoupdate, &nBool);
			m_bAutoUpdate = (nBool != 0);
		}

		if (m_dwVersion >= 16)
		{
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_org_color, &m_OrgColor);
			
			if (m_dwVersion >= 52)
			{
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_emissive_col, &m_EmissiveColor);
			}

			if (m_dwVersion >= 21)
			{
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_srcbld, &m_BlendMode.SrcBlend);

				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_destbld, &m_BlendMode.DestBlend);
			}

			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_outer_num, &nOuterDataNum);

			for (i = 0; i < nOuterDataNum; i++)
			{
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_float, &m_OuterData[i]);
			}

			bool bNewBoneScaleMethod = false;

			if (m_dwVersion >= 28)
			{
				int nRead;
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_new_scale, &nRead);
				bNewBoneScaleMethod = (nRead != 0);
			}

			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_bone_num, &nBoneScaleNum);

			if (bNewBoneScaleMethod)
			{
				for (i = 0; i < nBoneScaleNum; i++)
				{
					BoneScaleEx* pBoneScale = new BoneScaleEx;

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_bone_index, &pBoneScale->m_nIndex);

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_bone_scale, &pBoneScale->m_fLenFactor, &pBoneScale->m_fThickFactor, &pBoneScale->m_fWholeFactor);

					m_BoneScaleExArr.push_back(pBoneScale);
				}
			}
			else
			{
				for (i = 0; i < nBoneScaleNum; i++)
				{
					BoneScale* pBoneScale = new BoneScale;

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_bone_index, &pBoneScale->m_nIndex);

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_bone_scl_type, &pBoneScale->m_nType);

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_bone_scale, VECTORADDR_XYZ(pBoneScale->m_vScale));

					m_BoneScales.push_back(pBoneScale);
				}
			}

			if (m_dwVersion >= 29)
			{
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				m_strScaleBaseBone = szLine;
			}
		}

		if (m_dwVersion >= 27)
		{
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_def_speed, &m_fDefPlaySpeed);
		}

		if (m_dwVersion >= 43)
		{
			int nBool = 0;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_can_castshadow, &nBool);
			m_bCanCastShadow = (nBool != 0);
		}

		if (m_dwVersion >= 45)
		{
			int nBool = 0;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_render_model, &nBool);
			m_bRenderSkinModel = (nBool != 0);
		}

		if (m_dwVersion >= 48)
		{
			int nBool = 0;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_render_edge, &nBool);
			m_bRenderEdge = (nBool != 0);
		}

		if (m_dwVersion >= 57)
		{
			if (m_dwVersion >= 60)
				g_GfxLoadPixelShaderConsts(&file, m_strPixelShader, m_strShaderTex, m_vecPSConsts);
			else
			{
				char szBuf[MAX_PATH];
				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_shaderpath, szBuf);
				m_strPixelShader = szBuf;

				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_shadertex, szBuf);
				m_strShaderTex = szBuf;

				int i, j;
				int nPSConstCount = 0;
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_psconstcount, &nPSConstCount);
				m_vecPSConsts.reserve(nPSConstCount);

				int nTargetCount;

				for (i = 0; i < nPSConstCount; i++)
				{
					GfxPixelShaderConst psconst;
					A3DCOLORVALUE& value = psconst.init_val;

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_psindex, &psconst.index);

					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_psvalue, &value.r, &value.g, &value.b, &value.a);

					if (m_dwVersion >= 59)
					{
						file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLine, _format_psloop, &psconst.loop_count);
					}
					else
						psconst.loop_count = 1;

					nTargetCount = 0;
					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_pstargetcount, &nTargetCount);
					psconst.target_vals.reserve(nTargetCount);

					for (j = 0; j < nTargetCount; j++)
					{
						GfxPixelShaderTargetValue tv;
						A3DCOLORVALUE& value = tv.value;

						file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLine, _format_psinterval, &tv.interval);

						if (int(tv.interval) < 0 && m_dwVersion < 59)
							tv.interval = 0;

						file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLine, _format_psvalue, &value.r, &value.g, &value.b, &value.a);

						psconst.target_vals.push_back(tv);
					}

					psconst.CalcTotalTime();
					m_vecPSConsts.push_back(psconst);
				}
			}
		}

		if (m_dwVersion >= 31)
		{
			int channel_count = 0;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_channel_count, &channel_count);

			for (i = 0; i < channel_count; i++)
			{
				ActChannelInfo* pChannel = new ActChannelInfo;
				int channel = 0;
				int bone_num = 0;
				int j;

				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_channel, &channel);

				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_bone_num, &bone_num);

				for (j = 0; j < bone_num; j++)
				{
					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					pChannel->bone_names.push_back(AString(szLine));
				}

				if (channel >= 0 && channel < A3DSkinModel::ACTCHA_MAX)
				{
					delete m_ChannelInfoArr[channel];
					m_ChannelInfoArr[channel] = pChannel;
				}
				else
				{
					delete pChannel;
					assert(false);
				}
			}
		}

		if (m_dwVersion >= 32)
		{
			int count = 0;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_channel_count, &count);

			for (i = 0; i < count; i++)
			{
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_channel_mask, &m_EventMasks[i]);
			}
		}

		if (m_dwVersion >= 14)
		{
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_cogfx_num, &nCoGfxNum);
		}

		file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_comact_count, &nComActCount);

		if (m_dwVersion >= 68)
		{
			for (i=0; i < AUDIOEVENT_GROUP_COUNT; ++i)
			{
				int nEnable;
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_audiogroupenable, &nEnable);
				m_bAudioGroupEnable[i] = nEnable != 0;
			}
		}

		if (m_dwVersion >= 70)
		{
			int nParticleBonesCount = 0;
			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_particlebones_count, &nParticleBonesCount);
			
			m_PaticleBoneLst.clear();
			for (i=0; i < nParticleBonesCount; ++i)
			{
				int boneIdx = 0;
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, "%d", &boneIdx);
				m_PaticleBoneLst.push_back(boneIdx);
			}
		}
	}

	for (i = 0; i < nCoGfxNum; i++)
	{
		PGFX_INFO pEvent = static_cast<PGFX_INFO>(EVENT_INFO::LoadFromFile(
				NULL,
				&file,
				m_dwVersion));

		if (!pEvent) continue;

		char buf[20];
		sprintf(buf, "%d_co", i);

		pEvent->Init(pA3DDevice);
		m_CoGfxMap[AString(buf)] = pEvent;
	}
	
	for (i = 0; i < nComActCount; i++)
	{
		A3DCombinedAction* pAct = new A3DCombinedAction();

		if (!pAct->Load(pA3DDevice, &file, m_dwVersion))
		{
			delete pAct;
			continue;
		}

		AString strName(pAct->GetName());
		CombinedActMap::iterator it = m_ActionMap.find(strName);

		if (it != m_ActionMap.end())
			delete it->second;

		m_ActionMap[strName] = pAct;
	}

	if (file.IsBinary())
	{
	}
	else
	{
		char szLine[AFILE_LINEMAXLEN];

		if (m_dwVersion >= 25)
		{
			int nScriptCount = 0;

			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_script_count, &nScriptCount);

			AString str;
			for (i = 0; i < nScriptCount; i++)
			{
				ExpandStringBuffer(str, file.GetFileLength() - file.GetPos());

				int id = -1;
				int len = 0;
				int lines = 1;

				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_id, &id);

				if (m_dwVersion >= 26)
				{
					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_script_lines, &lines);
				}

				while (lines)
				{
					file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					str += szLine;
					lines--;
				}

				int buf_len = str.GetLength() + 32;
				char* pBuf = new char[buf_len];
				len = base64_decode((char*)(const char*)str, str.GetLength(), (unsigned char*)pBuf);
				pBuf[len] = 0;

				if (id >= 0 && id < enumECMScriptCount)
				{
					m_Scripts[id] = pBuf;
					AddScriptMethod(id, pBuf);
				}

				delete[] pBuf;
			}
		}
	}

	//	Load A3D skin model from file
	if (m_dwVersion >= 8)
		LoadAdditionalSkin(&file, m_dwVersion, bLoadAdditionalSkin);

	if (m_dwVersion >= 5)
	{
		int nChildCount = 0;

		if (file.IsBinary())
		{
			file.Read(&nChildCount, sizeof(nChildCount), &dwReadLen);

			for (int i = 0; i < nChildCount; i++)
			{
				ChildInfo* pChild = new ChildInfo;

				file.ReadString(pChild->m_strName);
				file.ReadString(pChild->m_strPath);
				file.ReadString(pChild->m_strHHName);
				file.ReadString(pChild->m_strCCName);

				m_ChildInfoArray.push_back(pChild);
			}
		}
		else
		{
			char	szLine[AFILE_LINEMAXLEN];
			char	szBuf[AFILE_LINEMAXLEN];

			file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_child_count, &nChildCount);

			for (int i = 0; i < nChildCount; i++)
			{
				ChildInfo* pChild = new ChildInfo;

				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_child_name, szBuf);
				pChild->m_strName = szBuf;

				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_child_path, szBuf);
				pChild->m_strPath = szBuf;

				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_hh_name, szBuf);
				pChild->m_strHHName = szBuf;

				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_cc_name, szBuf);
				pChild->m_strCCName = szBuf;

				m_ChildInfoArray.push_back(pChild);
			}

			if (m_dwVersion >= 34)
			{
				szBuf[0] = '\0';
				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_phys_file, szBuf);
				m_strPhysFileName = szBuf;
			}

			if (m_dwVersion >= 41)
			{

				file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				int iCount = 0;
				sscanf(szLine, _format_ecmhook_count, &iCount);
		
				for (int i = 0; i < iCount; ++i)
				{
					CECModelHook* pHook = new CECModelHook;
					if (!pHook->Load(&file, m_dwVersion))
					{
						delete pHook;
						continue;
					}

					ECModelHookMap::iterator itr = m_ECModelHookMap.find(pHook->GetName());
					if (itr != m_ECModelHookMap.end())
					{
						delete itr->second;
					}

					m_ECModelHookMap[pHook->GetName()] = pHook;
				}
			}
		}
	}

	file.Close();

	if (m_ScriptEnable[enumECMScriptInit])
	{
		abase::vector<CScriptValue> args, results;
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptInit], args, results);
	}

	if (m_ScriptEnable[enumECMScriptChangeEquipTableInit])
	{
		abase::vector<CScriptValue> args, results;
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptChangeEquipTableInit], args, results);
	}

#ifdef A3D_PHYSX

	m_bMphyLoaded = false;
	if (!m_strPhysFileName.IsEmpty())
	{
		char sz[MAX_PATH];
		strcpy(sz, szModelFile);
		strcpy(ECMPathFindFileNameA(sz), m_strPhysFileName);

		if (af_IsFileExist(sz))
		{
			if (!m_pPhysSyncData)
				m_pPhysSyncData = new A3DModelPhysSyncData;
			
			if (!m_pPhysSyncData->Load(sz))
			{
				delete m_pPhysSyncData;
				m_pPhysSyncData = new A3DModelPhysSyncData;
			}
			else
			{
				m_bMphyLoaded = true;
			}
		}
	}

#endif

#ifndef GFX_EDITOR

	if( bLoadConvexHull )
	{
		const char* szPath = strstr(szModelFile, "\\");
		if (!szPath) return true;

		char szProjFile[MAX_PATH];
		strcpy(szProjFile, AfxGetECMHullPath());
		strcat(szProjFile, szPath);

		char* ext = ECMPathFindExtensionA(szProjFile);
		if (*ext==0) return true;
		strcpy(ext, ".ecp");

		if (!af_IsFileExist(szProjFile) || !file.Open(szProjFile, AFILE_OPENEXIST | AFILE_BINARY))
			return true;
		/*
		CHBasedCD::ConvexHullArchive ar;
		if (!ar.LoadHeader(file))
			return true;

		int nNumHull = ar.GetNumHull();
		m_CHAABB.Clear();
		for (int i = 0; i < nNumHull; i++)
		{
			CHBasedCD::CConvexHullData* pHull = new CHBasedCD::CConvexHullData;
			ar.LoadHull(file, pHull);		
			m_ConvexHullDataArr.push_back(pHull);
			m_CHAABB.Merge(pHull->GetAABB());
		}
		*/
		ECModelBrushProjHead_t ProjHead;
		DWORD dwLen;

		file.Read(&ProjHead, sizeof(ProjHead), &dwLen);
		m_CHAABB.Clear();

		if (ProjHead.nVersion == ECMODEL_PROJ_VERSION)
		{
			for (unsigned i = 0; i < ProjHead.nNumHull; i++)
			{
				CHBasedCD::CConvexHullData* pHull = new CHBasedCD::CConvexHullData;
				pHull->LoadBinaryData(&file);
				// currently, the ECModelHullEditor has turned the model 180 degree which is a mistake,
				// so we have to turn it back before we use it.
				pHull->Transform(TransformMatrix(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0), A3DVECTOR3(0)));
				m_ConvexHullDataArr.push_back(pHull);
				m_CHAABB.Merge(pHull->GetAABB());
			}
		}

		file.Close();
	}

#endif

	return true;
}

bool CECModelStaticData::Save(const char* szFile, CECModel* pModel)
{
	AFile file;

	if (!file.Open(szFile, AFILE_CREATENEW | MODEL_FILE_TYPE))
		return false;

	m_dwVersion = EC_MODEL_CUR_VERSION;

	if (file.IsBinary())
	{
		DWORD dwWrite;
		size_t size, i;

		file.Write(&m_dwVersion, sizeof(m_dwVersion), &dwWrite);
		file.WriteString(m_strSkinModelPath);
		file.Write(&m_OrgColor, sizeof(m_OrgColor), &dwWrite);
		file.Write(&m_BlendMode, sizeof(m_BlendMode), &dwWrite);

		i = OUTER_DATA_COUNT;
		file.Write(&i, sizeof(i), &dwWrite);
		for (i = 0; i < OUTER_DATA_COUNT; i++)
			file.Write(&m_OuterData[i], sizeof(float), &dwWrite);

		size = m_BoneScales.size();
		file.Write(&size, sizeof(size), &dwWrite);

		for (i = 0; i < size; i++)
		{
			BoneScale* pScale = m_BoneScales[i];

			file.Write(&pScale->m_nIndex, sizeof(pScale->m_nIndex), &dwWrite);
			file.Write(&pScale->m_nType, sizeof(pScale->m_nType), &dwWrite);
			file.Write(&pScale->m_vScale.m, sizeof(pScale->m_vScale.m), &dwWrite);
		}

		size = m_ActionMap.size();
		file.Write(&size, sizeof(size), &dwWrite);

		/*
		 *	Modified
		 */
		for (CombinedActMap::iterator it_act = m_ActionMap.begin(); it_act != m_ActionMap.end(); ++it_act)
			it_act->second->Save(&file);

		SaveAdditionalSkin(&file);
		
		size = m_CoGfxMap.size();
		file.Write(&size, sizeof(size), &dwWrite);

		size = pModel->m_ChildModels.size();
		file.Write(&size, sizeof(size), &dwWrite);

		for (CoGfxMap::iterator it = m_CoGfxMap.begin(); it != m_CoGfxMap.end(); ++it)
			it->second->Save(&file);

		for (ECModelMap::iterator it_child = pModel->m_ChildModels.begin(); it_child != pModel->m_ChildModels.end(); ++it_child)
		{
			file.WriteString(it_child->first);
			file.WriteString(it_child->second->m_pMapModel->m_strFilePath);
			file.WriteString(it_child->second->m_strHookName);
			file.WriteString(it_child->second->m_strCC);
		}
	}
	else
	{
		size_t	i;
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_file_version, m_dwVersion);
		file.WriteLine(szLine);

		sprintf(szLine, _format_skinmodel_path, m_strSkinModelPath);
		file.WriteLine(szLine);

		sprintf(szLine, _format_autoupdate, m_bAutoUpdate);
		file.WriteLine(szLine);

		sprintf(szLine, _format_org_color, m_OrgColor);
		file.WriteLine(szLine);

		sprintf(szLine, _format_emissive_col, m_EmissiveColor);
		file.WriteLine(szLine);

		sprintf(szLine, _format_srcbld, m_BlendMode.SrcBlend);
		file.WriteLine(szLine);

		sprintf(szLine, _format_destbld, m_BlendMode.DestBlend);
		file.WriteLine(szLine);				

		sprintf(szLine, _format_outer_num, OUTER_DATA_COUNT);
		file.WriteLine(szLine);

		for (i = 0; i < OUTER_DATA_COUNT; i++)
		{
			sprintf(szLine, _format_float, m_OuterData[i]);
			file.WriteLine(szLine);
		}

		bool bNewBoneScaleMethod = g_pA3DConfig->GetFlagNewBoneScale();
		sprintf(szLine, _format_new_scale, bNewBoneScaleMethod);
		file.WriteLine(szLine);

		if (bNewBoneScaleMethod)
		{
			sprintf(szLine, _format_bone_num, m_BoneScaleExArr.size());
			file.WriteLine(szLine);

			for (i = 0; i < m_BoneScaleExArr.size(); i++)
			{
				BoneScaleEx* pScale = m_BoneScaleExArr[i];

				sprintf(szLine, _format_bone_index, pScale->m_nIndex);
				file.WriteLine(szLine);

				sprintf(szLine, _format_bone_scale, pScale->m_fLenFactor, pScale->m_fThickFactor, pScale->m_fWholeFactor);
				file.WriteLine(szLine);
			}
		}
		else
		{
			sprintf(szLine, _format_bone_num, m_BoneScales.size());
			file.WriteLine(szLine);

			for (i = 0; i < m_BoneScales.size(); i++)
			{
				BoneScale* pScale = m_BoneScales[i];

				sprintf(szLine, _format_bone_index, pScale->m_nIndex);
				file.WriteLine(szLine);

				sprintf(szLine, _format_bone_scl_type, pScale->m_nType);
				file.WriteLine(szLine);

				sprintf(szLine, _format_bone_scale, VECTOR_XYZ(pScale->m_vScale));
				file.WriteLine(szLine);
			}
		}

		file.WriteLine(m_strScaleBaseBone);

		sprintf(szLine, _format_def_speed, m_fDefPlaySpeed);
		file.WriteLine(szLine);

		sprintf(szLine, _format_can_castshadow, (int)m_bCanCastShadow);
		file.WriteLine(szLine);

		sprintf(szLine, _format_render_model, (int)m_bRenderSkinModel);
		file.WriteLine(szLine);

		sprintf(szLine, _format_render_edge, (int)m_bRenderEdge);
		file.WriteLine(szLine);

		g_GfxSavePixelShaderConsts(&file, m_strPixelShader, m_strShaderTex, m_vecPSConsts);
		int channel_count = 0;

		for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		{
			ActChannelInfo* pChannel = m_ChannelInfoArr[i];

			if (pChannel && pChannel->bone_names.size())
				channel_count++;
		}

		sprintf(szLine, _format_channel_count, channel_count);
		file.WriteLine(szLine);

		for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		{
			ActChannelInfo* pChannel = m_ChannelInfoArr[i];

			if (pChannel && pChannel->bone_names.size())
			{
				sprintf(szLine, _format_channel, i);
				file.WriteLine(szLine);

				sprintf(szLine, _format_bone_num, (int)pChannel->bone_names.size());
				file.WriteLine(szLine);

				for (size_t j = 0; j < pChannel->bone_names.size(); j++)
				{
					file.WriteLine(pChannel->bone_names[j]);
				}
			}
		}

		sprintf(szLine, _format_channel_count, A3DSkinModel::ACTCHA_MAX);
		file.WriteLine(szLine);

		for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
		{
			sprintf(szLine, _format_channel_mask, m_EventMasks[i]);
			file.WriteLine(szLine);
		}

		sprintf(szLine, _format_cogfx_num, m_CoGfxMap.size());
		file.WriteLine(szLine);

		sprintf(szLine, _format_comact_count, m_ActionMap.size());
		file.WriteLine(szLine);

		for (i = 0; i < AUDIOEVENT_GROUP_COUNT; ++i)
		{
			sprintf(szLine, _format_audiogroupenable, m_bAudioGroupEnable[i]);
			file.WriteLine(szLine);
		}

		sprintf(szLine, _format_particlebones_count, m_PaticleBoneLst.size());
		file.WriteLine(szLine);
		for (i = 0; i < (int)m_PaticleBoneLst.size(); ++i)
		{
			sprintf(szLine, "%d", m_PaticleBoneLst[i]);
			file.WriteLine(szLine);
		}

		for (CoGfxMap::iterator it = m_CoGfxMap.begin(); it != m_CoGfxMap.end(); ++it)
			it->second->Save(&file);

		abase::vector<A3DCombinedAction*> act_sort;

		for (CombinedActMap::iterator it_act = m_ActionMap.begin(); it_act != m_ActionMap.end(); ++it_act)
			act_sort.push_back(it_act->second);

		qsort(act_sort.begin(), act_sort.size(), sizeof(int), _str_compare);

		for (i = 0; i < act_sort.size(); i++)
			act_sort[i]->Save(&file);

		int nScriptCount = 0;

		for (i = 0; i < enumECMScriptCount; i++)
			if (!m_Scripts[i].IsEmpty())
				nScriptCount++;

		sprintf(szLine, _format_script_count, nScriptCount);
		file.WriteLine(szLine);

		for (i = 0; i < enumECMScriptCount; i++)
		{
			if (!m_Scripts[i].IsEmpty())
			{
				sprintf(szLine, _format_id, i);
				file.WriteLine(szLine);

				AString& str = m_Scripts[i];
				int nBufLen = (str.GetLength() + 3) / 3 * 4 + 32;
				char* pBuf = new char[nBufLen];
				int len = base64_encode((unsigned char*)(const char*)str, str.GetLength()+1, pBuf);
				int nLines = len / 1500;

				if (len > nLines * 1500)
					nLines++;

				sprintf(szLine, _format_script_lines, nLines);
				file.WriteLine(szLine);

				const char* pWrite = pBuf;

				while (len)
				{
					int nWrite = len > 1500 ? 1500 : len;
					len -= nWrite;

					AString s(pWrite, nWrite);
					file.WriteLine(s);
					pWrite += nWrite;
				}

				delete[] pBuf;
			}
		}

		SaveAdditionalSkin(&file);

		if (pModel)
		{
			sprintf(szLine, _format_child_count, pModel->m_ChildModels.size());
			file.WriteLine(szLine);

			for (ECModelMap::iterator it_child = pModel->m_ChildModels.begin(); it_child != pModel->m_ChildModels.end(); ++it_child)
			{
				sprintf(szLine, _format_child_name, it_child->first);
				file.WriteLine(szLine);

				sprintf(szLine, _format_child_path, it_child->second->m_pMapModel->m_strFilePath);
				file.WriteLine(szLine);

				sprintf(szLine, _format_hh_name, it_child->second->m_strHookName);
				file.WriteLine(szLine);

				sprintf(szLine, _format_cc_name, it_child->second->m_strCC);
				file.WriteLine(szLine);
			}
		}
		else
		{
			sprintf(szLine, _format_child_count, m_ChildInfoArray.size());
			file.WriteLine(szLine);

			for (i = 0; i < m_ChildInfoArray.size(); i++)
			{
				const ChildInfo* pChildInfo = m_ChildInfoArray[i];
				sprintf(szLine, _format_child_name, (const char*)pChildInfo->m_strName);
				file.WriteLine(szLine);

				sprintf(szLine, _format_child_path, (const char*)pChildInfo->m_strPath);
				file.WriteLine(szLine);

				sprintf(szLine, _format_hh_name, (const char*)pChildInfo->m_strHHName);
				file.WriteLine(szLine);

				sprintf(szLine, _format_cc_name, (const char*)pChildInfo->m_strCCName);
				file.WriteLine(szLine);
			}
		}

		sprintf(szLine, _format_phys_file, m_strPhysFileName);
		file.WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_ecmhook_count, m_ECModelHookMap.size());
		file.WriteLine(szLine);

		for (ECModelHookMap::iterator it_ecmhook = m_ECModelHookMap.begin(); it_ecmhook != m_ECModelHookMap.end(); ++it_ecmhook)
		{
			CECModelHook* pHook = it_ecmhook->second;
			if (!pHook)
				continue;

			pHook->Save(&file);
		}

	}

	file.Close();

#ifdef A3D_PHYSX

	if (m_pPhysSyncData)
	{
		char sz [MAX_PATH];
		strcpy(sz, szFile);
		strcpy(ECMPathFindFileNameA(sz), m_strPhysFileName);

		if (!m_pPhysSyncData->Save(sz))
			return false;
	}

#endif

	return true;
}

bool CECModelStaticData::SaveBoneScaleInfo(const char* szFile)
{
	AFile file;

	if (!file.Open(szFile, AFILE_CREATENEW | AFILE_BINARY))
		return false;

	DWORD len;
	int nNum = m_BoneScaleExArr.size();
	file.Write(&nNum, sizeof(nNum), &len);

	for (int i = 0; i < nNum; i++)
	{
		BoneScaleEx* p = m_BoneScaleExArr[i];
		file.Write(p, sizeof(BoneScaleEx), &len);
	}

	nNum = m_strScaleBaseBone.GetLength();
	file.Write(&nNum,sizeof(nNum), &len);
	file.Write((void*)(const char*)m_strScaleBaseBone, nNum, &len);

	file.Close();
	return true;
}

void CECModelStaticData::SaveAdditionalSkin(AFile* pFile)
{
	DWORD dwWrite;

	if (pFile->IsBinary())
	{
		size_t iCount = m_AdditionalSkinLst.size();
		pFile->Write(&iCount, sizeof(iCount), &dwWrite);

		for (size_t i = 0; i < iCount; i++)
			pFile->WriteString(m_AdditionalSkinLst[i]);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_addi_skin_count, m_AdditionalSkinLst.size());
		pFile->WriteLine(szLine);

		for (size_t i = 0; i < m_AdditionalSkinLst.size(); i++)
		{
			sprintf(szLine, _format_addi_skin_path, m_AdditionalSkinLst[i]);
			pFile->WriteLine(szLine);
		}
	}
}

void CECModelStaticData::Reset()
{
	int i;

	m_OrgColor = A3DCOLORRGBA(255, 255, 255, 255);
	m_EmissiveColor = A3DCOLORRGBA(0, 0, 0, 0);
	m_fDefPlaySpeed = 1.0f;
	m_BlendMode.SrcBlend = A3DBLEND_SRCALPHA;
	m_BlendMode.DestBlend = A3DBLEND_INVSRCALPHA;


	for (i = 0; i < OUTER_DATA_COUNT; i++)
		m_OuterData[i] = 1.0f;

	for (i = 0; i < (int)m_BoneScales.size(); i++)
		delete m_BoneScales[i];

	m_BoneScales.clear();

	for (i = 0; i < (int)m_BoneScaleExArr.size(); i++)
		delete m_BoneScaleExArr[i];

	m_BoneScaleExArr.clear();
	m_strScaleBaseBone.Empty();

	for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
	{
		delete m_ChannelInfoArr[i];
		m_ChannelInfoArr[i] = 0;
	}

	m_strPhysFileName.Empty();
}

void CECModelStaticData::AddScriptMethod(int index, const char* szScript)
{
	abase::vector<AString> vargs;
	int var_count = ecm_get_var_count(index);

	for (int i = 0; i < var_count; i++)
		vargs.push_back(ecm_get_var_name(index, i));

	m_ScriptEnable[index] = m_ScriptMemTbl.AddMethod(_script_func_name[index], vargs, szScript);
}

void CECModelStaticData::OnScriptPlayAction(CECModel* pModel, int nChannel, const char* szActName)
{
	using namespace LuaBind;

	if (m_ScriptEnable[enumECMScriptStartAction])
	{
#ifdef FUNCTION_ANALYSER
		ATTACH_FUNCTION_ANALYSER(true, 3, 0, m_strFilePath + szActName);
#endif

		abase::vector<CScriptValue> args, results;
		args.push_back(CScriptValue((void *)pModel));
		args.push_back(CScriptValue((double)nChannel));
		CScriptString str;
		str.SetUtf8(szActName, strlen(szActName));
		args.push_back(CScriptValue(str));
		args.push_back(CScriptValue(pModel->m_bFashionMode));
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptStartAction], args, results);
	}
}

void CECModelStaticData::OnScriptEndAction(CECModel* pModel, int nChannel, const char* szActName)
{
	using namespace LuaBind;

	if (m_ScriptEnable[enumECMScriptEndActioin])
	{
#ifdef FUNCTION_ANALYSER
		ATTACH_FUNCTION_ANALYSER(true, 3, 0, m_strFilePath + szActName);
#endif

		abase::vector<CScriptValue> args, results;
		args.push_back(CScriptValue((void*)pModel));
		args.push_back(CScriptValue((double)nChannel));
		CScriptString str;
		str.SetUtf8(szActName, strlen(szActName));
		args.push_back(CScriptValue(str));
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptEndActioin], args, results);
	}
}

void CECModelStaticData::OnScriptChangeEquip(CECModel* pModel, int nEquipId, int nEquipFlag, bool bFashionMode, int nPathId, int nEquipIndex)
{
	using namespace LuaBind;

	if (m_ScriptEnable[enumECMScriptChangeEquip])
	{
#ifdef FUNCTION_ANALYSER
		ATTACH_FUNCTION_ANALYSER(true, 3, 0, m_strFilePath);
#endif

		abase::vector<CScriptValue> args, results;
		args.push_back(CScriptValue((void *)pModel));
		args.push_back(CScriptValue((double)nEquipId));
		args.push_back(CScriptValue((double)nPathId));
		args.push_back(CScriptValue((double)nEquipFlag));
		args.push_back(CScriptValue((double)nEquipIndex));
		args.push_back(CScriptValue(bFashionMode));
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptChangeEquip], args, results);
	}
}

void CECModelStaticData::OnScriptPhysBreak(CECModel* pModel, float fBreakOffsetX, float fBreakOffsetY, float fBreakOffsetZ)
{
	using namespace LuaBind;

	if (m_ScriptEnable[enumECMScriptPhysBreak])
	{
#ifdef FUNCTION_ANALYSER
		ATTACH_FUNCTION_ANALYSER(true, 3, 0, m_strFilePath);
#endif

		abase::vector<CScriptValue> args, results;
		args.push_back(CScriptValue((void*)pModel));
		args.push_back(CScriptValue((double)fBreakOffsetX));
		args.push_back(CScriptValue((double)fBreakOffsetY));
		args.push_back(CScriptValue((double)fBreakOffsetZ));

		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptPhysBreak], args, results);
	}
}

void CECModelStaticData::InitGlobalScript()
{
	static int _index = 1;

	if (!m_bInitGlobalScript)
	{
#ifdef FUNCTION_ANALYSER
		ATTACH_FUNCTION_ANALYSER(true, 3, 0, m_strFilePath);
#endif

		m_strGlobalScriptName.Format("GlobalScriptFunc_%d", _index++);
		abase::vector<AString> vargs;
		int var_count = ecm_get_var_count(enumECMScriptInit);

		for (int i = 0; i < var_count; i++)
			vargs.push_back(ecm_get_var_name(enumECMScriptInit, i));

		m_bInitGlobalScript = AfxGetECModelMan()->GetActScriptTbl().AddMethod(m_strGlobalScriptName, vargs, m_Scripts[enumECMScriptInit]);

		if (m_bInitGlobalScript)
		{
			abase::vector<CScriptValue> args, results;
			AfxGetECModelMan()->GetActScriptTbl().Call(m_strGlobalScriptName, args, results);
		}

#ifdef GFX_EDITOR
		m_bInitGlobalScript = false;
#endif
	}
}

bool CECModelStaticData::LoadAdditionalSkin(AFile* pFile, DWORD dwVersion, bool bLoadAdditionalSkin)
{
	DWORD dwReadLen;
	int nSkinCount = 0;

	if (!pFile->IsBinary())
	{
		char	szLine[AFILE_LINEMAXLEN];
		char	szBuf[AFILE_LINEMAXLEN];

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_addi_skin_count, &nSkinCount);

		for (int i = 0; i < nSkinCount; i++)
		{
			szBuf[0] = '\0';
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);

#ifdef GFX_EDITOR

			sscanf(szLine, _format_addi_skin_path, szBuf);

			if (strlen(szBuf) && bLoadAdditionalSkin)
				m_AdditionalSkinLst.push_back(szBuf);

#endif

		}
	}

	return true;
}

void CECModelStaticData::Release()
{
	size_t i;

	m_strPixelShader.Empty();
	m_strShaderTex.Empty();
	m_vecPSConsts.clear();

	for (i = 0; i < m_BoneScales.size(); i++)
		delete m_BoneScales[i];

	m_BoneScales.clear();

	for (i = 0; i < m_BoneScaleExArr.size(); i++)
		delete m_BoneScaleExArr[i];

	m_BoneScaleExArr.clear();

	for (CoGfxMap::iterator itGfx = m_CoGfxMap.begin(); itGfx != m_CoGfxMap.end(); ++itGfx)
		delete itGfx->second;

	m_CoGfxMap.clear();

	for (CombinedActMap::iterator it = m_ActionMap.begin(); it != m_ActionMap.end(); ++it)
		delete it->second;

	m_AdditionalSkinLst.clear();
	m_ActionMap.clear();

	m_PaticleBoneLst.clear();

	for (i = 0; i < m_ChildInfoArray.size(); i++)
		delete m_ChildInfoArray[i];

	m_ChildInfoArray.clear();

#ifndef GFX_EDITOR

	for (i = 0; i < m_ConvexHullDataArr.size(); i++)
		delete m_ConvexHullDataArr[i];

	m_ConvexHullDataArr.clear();

#endif

	for (i = 0; i < A3DSkinModel::ACTCHA_MAX; i++)
	{
		delete m_ChannelInfoArr[i];
		m_ChannelInfoArr[i] = 0;
	}

	m_bChannelInfoInit = false;

	if (m_ScriptEnable[enumECMScriptRelease])
	{
		abase::vector<CScriptValue> args, results;
		m_ScriptMemTbl.Call(_script_func_name[enumECMScriptRelease], args, results);
	}

	for (i = 0; i < enumECMScriptCount; i++)
		m_Scripts[i].Empty();

	memset(m_ScriptEnable, 0, sizeof(m_ScriptEnable));
	m_ScriptMemTbl.Release();

	if (m_bInitGlobalScript)
	{
		AfxGetECModelMan()->GetActScriptTbl().RemoveMethod(m_strGlobalScriptName);
		m_bInitGlobalScript = false;
	}

#ifdef A3D_PHYSX

	if (m_pPhysSyncData)
	{
		delete m_pPhysSyncData;
		m_pPhysSyncData = 0;
	}

#endif

	{
		for (ECModelHookMap::iterator itECMHook = m_ECModelHookMap.begin(); itECMHook != m_ECModelHookMap.end(); ++itECMHook)
			delete itECMHook->second;
		
		m_ECModelHookMap.clear();
	}
}

bool CECModelStaticData::UpdateScript(int index, const char* strScript, bool bCompileOnly)
{
	if (!bCompileOnly)
		m_Scripts[index] = strScript;

	m_ScriptMemTbl.RemoveMethod(_script_func_name[index]);
	AddScriptMethod(index, strScript);
	return m_ScriptEnable[index];
}

void CECModelStaticData::InitScript()
{
	AString strECMTable = _format_ecm_table_name(this);
	m_ScriptMemTbl.Init(strECMTable);
}

bool CECModel::PWPlayActionByName(const char* szActName, float fWeight, bool bRestart/* true */,
	int nTransTime/* 200 */, bool bForceStop/* true */, DWORD dwUserData/* 0 */, bool bNoFx
	/*joslian*/, bool* pActFlag, DWORD dwFlagMode)
{
	return PWPlayActionByName(0, szActName, fWeight, bRestart, nTransTime, bForceStop, dwUserData, bNoFx/*joslian*/, pActFlag, dwFlagMode);
}

bool CECModel::PWPlayActionByName(int nChannel, const char* szActName, float fWeight, bool bRestart/* true */,
	int nTransTime/* 200 */, bool bForceStop/* true */, DWORD dwUserData/* 0 */, bool bNoFx
	/*joslian*/, bool* pActFlag, DWORD dwFlagMode)
{
	if (!szActName)
		return false;

	A3DCombinedAction* pComAct = GetComActByName(szActName);

	if (pComAct == NULL)
		return false;

	BYTE rank = pComAct->GetRank(nChannel);
	ChannelAct& ca = m_ChannelActs[nChannel];
	ChannelActNode* pNode = ca.GetNodeByRank(rank);
	A3DCombActDynData* pActive;

	if (pNode)
	{
		pActive = pNode->m_pActive;
		pNode->RemoveQueuedActs();
	}
	else
		pActive = 0;

	if (!bRestart && pActive && stricmp(pActive->GetComAct()->GetName(), szActName) == 0)
		return true;

	if (m_pBlurInfo && m_pBlurInfo->m_bRoot && m_pBlurInfo->m_bStopWhenActChange)
		RemoveMotionBlurInfo();

	// script
	m_pMapModel->OnScriptPlayAction(this, nChannel, szActName);

	// delay mode state
	m_kECMDelayInfo.OnPlayComAction();

	if (pActive != NULL)
	{
		pActive->Stop(bForceStop);
		delete pActive;
	}
	else if (!m_bFirstActPlayed)
	{
		m_bFirstActPlayed = true;
		nTransTime = 0;
	}

	if (!pNode)
	{
		pNode = new ChannelActNode();
		pNode->m_Rank = rank;
		ca.m_RankNodes.AddTail(pNode);
	}

	SetPlaySpeedByChannel(nChannel, pComAct);

	pNode->m_pActive = new A3DCombActDynData(pComAct, this);
	pNode->m_pActive->SetUserData(dwUserData);
	pNode->m_pActive->Play(nChannel, fWeight, nTransTime, m_EventMasks[nChannel], bRestart, m_bAbsTrack, bNoFx);
	m_bAbsTrack = false;

	// clear notify flag each time we play a new action
	pNode->m_pActFlag = pActFlag;
	pNode->m_dwFlagMode = dwFlagMode;
	return true;
}









#ifdef GFX_EDITOR

CECModelMan _ecmodel_man;

CECModelMan* AfxGetECModelMan()
{
	return &_ecmodel_man;
}

#endif

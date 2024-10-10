/*
 * FILE: EC_Model.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <A3DVector.h>
#include <A3DCoordinate.h>
#include <AArray.h>
#include <A3DGeometry.h>
#include <A3DSkinModel.h>
#include <AString.h>
#include <A3DBone.h>
#include <AList2.h>
#include <hashtab.h>
#include <hashmap.h>
#include <A3DTypes.h>

#include <float.h>
#include "LuaMemTbl.h"
#include "GfxCommonTypes.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////
#ifdef _ANGELICA21
	#define ECM_ACTION_TRANSTIME 100
#else
	#define ECM_ACTION_TRANSTIME 200
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkinModel;
class A3DSkinModelRenderModifier;
class A3DSkin;
class CECViewport;
class AM3DSoundBuffer;
class A3DCombinedAction;
class A3DCombActDynData;
class CECModelStaticData;
class CECModel;
class CECModelHook;
class A3DModelPhysSyncData;
class A3DModelPhysSync;
class A3DModelPhysics;
struct PhysBreakInfo;
class A3DGFXEx;
class A3DShader;
class A3DHLPixelShader;
class GFX_INFO;
class ChildActInfo;
class CECAnimNodeMan;
// ecm level motion blur
class ECMActionBlurInfo;
class CECModelPhysBlendOverlap;
class TrackBoneVelocity;
class BlendTreeInterface;

#define ECM_BIND_DONT_DEL_BY_PARENT		0x1

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

// script
enum
{
	enumECMScriptStartAction = 0,
	enumECMScriptEndActioin,
	enumECMScriptInit,
	enumECMScriptRelease,
	enumECMScriptModelLoaded,
	enumECMScriptChangeEquip,
	enumECMScriptChangeEquipTableInit,
	enumECMScriptPhysBreak,
	enumECMScriptCount
};

enum
{
	enumECMVarModel = 0,
	enumECMVarActName,
	enumECMVarActChannel,
	enumECMVarEquipId,
	enumECMVarEquipFlag,
	enumECMVarFashionMode,
	enumECMVarPathId,
	enumECMVarId,
	enumECMVarEquipIndex,
	enumECMVarBreakOffsetX,
	enumECMVarBreakOffsetY,
	enumECMVarBreakOffsetZ,
	enumECMVarCasterId,
	enumECMVarCastTargetId,
	enumECMVarSelf,
	enumECMVarCount,
};

#define ECM_SCRIPT_MAX_VAR_COUNT 8

struct ECM_SCRIPT_VAR
{
	int var_count;
	int var_index[ECM_SCRIPT_MAX_VAR_COUNT];
};

class CLuaState;
int ecm_get_var_count(int func);
const char* ecm_get_var_name(int func, int var_index);
void InitECMApi(CLuaState * pState);

#define COMACT_FLAG_MODE_NONE					0
#define COMACT_FLAG_MODE_ONCE					1
#define COMACT_FLAG_MODE_ONCE_IGNOREGFX			2
#define COMACT_FLAG_MODE_ONCE_MULTIIGNOREGFX	3

#define OUTER_DATA_COUNT						8
#define	AUDIOEVENT_GROUP_COUNT					11

struct BoneScale
{
	int			m_nIndex;
	int			m_nType;
	A3DVECTOR3	m_vScale;
};

struct BoneScaleEx
{
	int			m_nIndex;
	float		m_fLenFactor;
	float		m_fThickFactor;
	float		m_fWholeFactor;
};

struct ClothSimDesc 
{
	ClothSimDesc()
	{
		iLinkType = 0;
		iDelayTime = 500;
		iBlendTime = 800;
	}

	int iLinkType;
	int iDelayTime;
	int iBlendTime;
};

typedef abase::vector<BoneScale*> BoneScaleArray;
typedef abase::vector<BoneScaleEx*> BoneScaleExArray;
typedef abase::hash_map<AString, A3DCombinedAction*> CombinedActMap;
typedef abase::hash_map<AString, GFX_INFO*> CoGfxMap;
typedef abase::hash_map<AString, CECModel*> ECModelMap;
typedef abase::hash_map<AString, CECModelHook*> ECModelHookMap;
typedef abase::vector<int>	ParticleBoneArray;

class SkinModelTRControl
{
public:
	SkinModelTRControl()
		: m_HideCount(0)
		, m_fTickSpeed(1.0f)
	{

	}

	int RequestHide() { return ++m_HideCount; }
	int RequestShow() { ASSERT(m_HideCount > 0); return --m_HideCount; }

	bool IsHidden() const { return m_HideCount > 0; }
	bool IsShown() const { return !IsHidden(); }

	void SetTickSpeed(float fTickSpeed) { m_fTickSpeed = fTickSpeed; }
	float GetTickSpeed() const { return m_fTickSpeed; }

	void Reset()
	{
		m_HideCount = 0;
		m_fTickSpeed = 1.0f;
	}

private:
	int m_HideCount;
	float m_fTickSpeed;
};

struct ActChannelInfo
{
	abase::vector<AString> bone_names;
	abase::vector<int> joint_indices;

	ActChannelInfo& operator = (const ActChannelInfo& src)
	{
		bone_names = src.bone_names;
		joint_indices = src.joint_indices;
		return *this;
	}
};

namespace CHBasedCD
{
	class CConvexHullData;
	typedef abase::vector<CConvexHullData*> ConvexHullDataArray;
}

class CECModelStaticData
{
public:

	CECModelStaticData();

	~CECModelStaticData()
	{
		Release();
	}

protected:

	AString			m_strFilePath;
	AString			m_strSkinModelPath;		//	Skin model file name
	bool			m_bAutoUpdate;
	AString			m_strHangerName;
	bool			m_bActMapped;
	CombinedActMap	m_ActionMap;
	DWORD			m_dwVersion;
	AString			m_strHook;
	AString			m_strCCName;
	BoneScaleArray	m_BoneScales;
	BoneScaleExArray m_BoneScaleExArr;
	AString			m_strScaleBaseBone;
	A3DCOLOR		m_OrgColor;
	A3DCOLOR		m_EmissiveColor;
	float			m_fDefPlaySpeed;
	A3DSHADER		m_BlendMode;
	float			m_OuterData[OUTER_DATA_COUNT];
	bool			m_bCanCastShadow;
	bool			m_bRenderSkinModel;
	bool			m_bRenderEdge;

	CoGfxMap		m_CoGfxMap;
	CHBasedCD::ConvexHullDataArray m_ConvexHullDataArr;
	A3DAABB			m_CHAABB;
	ActChannelInfo*	m_ChannelInfoArr[A3DSkinModel::ACTCHA_MAX];
	bool			m_bChannelInfoInit;
	DWORD			m_EventMasks[A3DSkinModel::ACTCHA_MAX];

	AString			m_Scripts[enumECMScriptCount];
	bool			m_ScriptEnable[enumECMScriptCount];
	CLuaMemTbl		m_ScriptMemTbl;
	AString			m_strGlobalScriptName;
	bool			m_bInitGlobalScript;
	bool			m_bMphyLoaded;
	char			m_nHumanbingSkeleton; //-1: Not Init; 0: Not Humanbing; 1: Humanbing

	bool			m_bAudioGroupEnable[AUDIOEVENT_GROUP_COUNT];

	struct ChildInfo
	{
		AString		m_strName;
		AString		m_strPath;
		AString		m_strHHName;
		AString		m_strCCName;
	};

	typedef abase::vector<ChildInfo*> ChildInfoArray;
	ChildInfoArray m_ChildInfoArray;
	abase::vector<AString> m_AdditionalSkinLst;

	abase::vector<int>	m_PaticleBoneLst;			//bones that emit particles

	AString m_strPhysFileName;
	A3DModelPhysSyncData* m_pPhysSyncData;

	ECModelHookMap m_ECModelHookMap;

	AString				m_strPixelShader;
	AString				m_strShaderTex;
	GfxPSConstVec		m_vecPSConsts;

	friend class CECModel;
	friend class GfxScriptEvent;

protected:

	void SaveAdditionalSkin(AFile* pFile);
	bool LoadAdditionalSkin(AFile* pFile, DWORD dwVersion, bool bLoadAdditionalSkin);
	void AddScriptMethod(int index, const char* szScript);
	void OnScriptPlayAction(CECModel* pModel, int nChannel, const char* szActName);
	void OnScriptChangeEquip(CECModel* pModel, int nEquipId, int nEquipFlag, bool bFashionMode, int nPathId, int nEquipIndex);
	void OnScriptPhysBreak(CECModel* pModel, float fBreakOffsetX, float fBreakOffsetY, float fBreakOffsetZ);
	void InitGlobalScript();
	
public:

	bool LoadData(const char* szModelFile, bool bLoadAdditionalSkin, bool bLoadConvexHull = true);
	bool Save(const char* szFile, CECModel* pModel);
	bool SaveBoneScaleInfo(const char* szFile);
	void Release();
	void InitScript();
	const AString& GetFilePath() const { return m_strFilePath; }
	CHBasedCD::ConvexHullDataArray& GetConvexHullData() { return m_ConvexHullDataArr; }
	bool HasCHAABB() const { return m_ConvexHullDataArr.size() != 0; }
	const A3DAABB& GetCHAABB() const { return m_CHAABB; }
	const AString& GetScript(size_t index) const { ASSERT(index<enumECMScriptCount); return m_Scripts[index]; }
	bool UpdateScript(int index, const char* strScript, bool bCompileOnly);
	CoGfxMap& GetCoGfxMap() { return m_CoGfxMap; }
	CombinedActMap& GetCombinedActMap() { return m_ActionMap; }
	const AString& GetPhysFileName() const { return m_strPhysFileName; }
	void SetPhysFileName(const char* szFile) { m_strPhysFileName = szFile; }
	A3DModelPhysSyncData* GetPhysSyncData() { return m_pPhysSyncData; }
	void SetPhysSyncData(A3DModelPhysSyncData* p) { m_pPhysSyncData = p; }
	DWORD GetVersion() const { return m_dwVersion; }
	const AString& GetPixelShaderPath() const { return m_strPixelShader; }
	void SetPixelShaderPath(const char* szPath) { m_strPixelShader = szPath; }
	const AString& GetShaderTexture() const { return m_strShaderTex; }
	void SetShaderTexture(const char* szPath) { m_strShaderTex = szPath; }
	GfxPSConstVec& GetPSConstVec() { return m_vecPSConsts; }
	void SetActionEventMask(int nChannel, DWORD dwMask) { m_EventMasks[nChannel] = dwMask; }
	DWORD GetActionEventMask(int nChannel) const { return m_EventMasks[nChannel]; }
	const DWORD* GetActionEventMask() const { return m_EventMasks; }

	void OnScriptEndAction(CECModel* pModel, int nChannel, const char* szActName);

	bool IsAudioGroupEnable(int n) const 
	{
		if (n<0 || n>=AUDIOEVENT_GROUP_COUNT)
			return false;
		return m_bAudioGroupEnable[n];
	}
	void SetAudioGroupEnable(int n, bool enable)
	{
		if (n>=0 && n<AUDIOEVENT_GROUP_COUNT)
		{
			m_bAudioGroupEnable[n] = enable;
		}
	}

	// for editor use only
	void Reset();
};


//---------------------------------------------------------------------
// physics related...
typedef void (*ON_PHYS_BREAK)(CECModel* pECModel);


class NxActor;
struct ECMPhysXHitGroundInfo
{
	NxActor* mHitActor;
	A3DVECTOR3 mHitPos;
	A3DVECTOR3 mHitNormalForce;

	ECMPhysXHitGroundInfo():mHitActor(NULL), mHitPos(0.0f), mHitNormalForce(0.0f)
	{

	}
};

typedef void (*ON_PHYS_HITGROUND)(CECModel* pECModel, const abase::vector<ECMPhysXHitGroundInfo>& arrPhysXHitGroundInfo);

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECModel
//	
///////////////////////////////////////////////////////////////////////////


class CECModel : public A3DCoordinate
{
public:		//	Types

	//	Bone scale type
	enum
	{
		SCALE_NONE = 0,		//	No scale
		SCALE_WHOLE,		//	Scale will effect all children
		SCALE_LOCAL,		//	Scale only effect this bone
	};

	//	AABB type
	enum
	{
		AABB_SKELETON,
		AABB_INITMESH,
		AABB_AUTOSEL,
	};

	//	Action channel
	enum
	{
		ACTCHA_WOUND = 1,
	};

	//	Delay Mode
	enum
	{
		DM_PLAYSPEED_NORMAL	= 0,		//	ECM tick as normal
		DM_PLAYSPEED_DELAY,				//	ECM hold for a while in which we tick 0
		DM_PLAYSPEED_JUMP,				//	ECM jump the elapsed time in tick
	};

	//	Collision channel method
	enum
	{
		COLLISION_CHANNEL_INDEPENDENT = 0,	//	As independent collision channel
		COLLISION_CHANNEL_AS_PARENT,	//	As same as parent (if exist parent)
		COLLISION_CHANNEL_AS_COMMON,	//	As common (collision with all objects)

		COLLISION_CHANNEL_AS_CC_COMMON_HOST,	// just for the ECModel which will be the Avatar model and moved by APhysXCharacterController...
		COLLISION_CHANNEL_AS_MAJORRB,			// just as the major RB's collision channel which may be set in the ModEditor...
		
	};

	//	Phys AddForce Type
	enum PHYFORCE_TYPE
	{
		PFT_FORCE,						// parameter has unit of mass * distance/ time^2, i.e. a force
		PFT_IMPULSE,					// parameter has unit of mass * distance /time
		PFT_VELOCITY_CHANGE,			// parameter has unit of distance / time, i.e. the effect is mass independent: a velocity change.
		PFT_SMOOTH_IMPULSE,				// same as NX_IMPULSE but the effect is applied over all substeps. Use this for motion controllers that repeatedly apply an impulse.
		PFT_SMOOTH_VELOCITY_CHANGE,		// same as NX_VELOCITY_CHANGE but the effect is applied over all substeps. Use this for motion controllers that repeatedly apply an impulse.
		PFT_ACCELERATION				// parameter has unit of distance/ time^2, i.e. an acceleration. It gets treated just like a force except the mass is not divided out before integration.
	};

	//	Rotate Axis
	enum ROT_AXIS
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
	};
	
	//	Error Types
	enum
	{
		ECMERR_SMDLOADFAILED	= 0,	//	SMD file load failed
		ECMERR_UNDEFINED		= -1,	//	Undefined error
	};

	//	TrackBoneMode
	enum TRACKBONEVEL_MODE
	{
		TRACKBONEVEL_MODE_NONE	= 0,

		TRACKBONEVEL_MODE_AVERAGE =	1,
		TRACKBONEVEL_MODE_MAX,

		TRACKBONEVEL_MODE_COUNT,
	};

	struct PHYTORQUE_INFO
	{
		PHYTORQUE_INFO() { memset(this, 0, sizeof(*this)); }

		char bValid;
		ROT_AXIS iAxis;
		PHYFORCE_TYPE iForceType;
		float fMagnitude;
		DWORD dwDuration;
		DWORD dwPlayedTime;
	};

	struct LIGHT_TRANS_INFO
	{
		DWORD m_dwLightId1;
		DWORD m_dwLightId2;
		A3DSkinModel::LIGHTINFO* m_pLightInfo1;
		A3DSkinModel::LIGHTINFO* m_pLightInfo2;
		A3DSkinModel::LIGHTINFO* m_pCurLightInfo;
		DWORD m_dwCurTransTime;

		LIGHT_TRANS_INFO()
		{
			m_dwLightId1 = 0;
			m_dwLightId2 = 0;
			m_pLightInfo1 = 0;
			m_pLightInfo2 = 0;
			m_pCurLightInfo = 0;
			m_dwCurTransTime = 0;
		}

		~LIGHT_TRANS_INFO()
		{
			delete m_pLightInfo1;
			delete m_pLightInfo2;
			delete m_pCurLightInfo;
		}

		void Update(DWORD dwTickTime, const A3DVECTOR3& vPos);
		void CheckCurLight(A3DSkinModel* pSkinModel, DWORD dwLightId, A3DSkinModel::LIGHTINFO& info);
	};

	typedef abase::hash_map<AString, int> HideModelHHMap;

public:		//	Constructor and Destructor

	CECModel();
	virtual ~CECModel();

public:		//	Attributes

public:		//	Operations

	//	Initialize model
	//	Do not load additional skin for default (only editors need this feature)
	//	Do not create physics for default
	//	User is acquired to create physics after the model is set to correct position
	bool Load(const char* szModelFile, bool bLoadSkinModel=true, int iSkinFlag=0, bool bLoadChildECModel=true, bool bLoadAdditionalSkin=false, bool bIsCreateModelPhys = false, bool bLoadConvexHull = true);
	bool LoadBoneScaleInfo(const char* szFile);
	bool Save(const char* szFile) { return m_pMapModel->Save(szFile, this); }
	bool SaveBoneScaleInfo(const char* szFile) { return m_pMapModel->SaveBoneScaleInfo(szFile); }

	bool IsLoaded() { return m_pMapModel != NULL; }
	DWORD GetECMVersion() const { return m_pMapModel->m_dwVersion; }
	
	//	This function will set current model's id property to the id passed in
	//	And also all the child models
	void SetId(clientid_t id);
	clientid_t GetId() const { return m_nId; }
	bool InheritParentId() const { return m_bInheritParentId; }
	void SetInheritParentId(bool b) { m_bInheritParentId = b; }

	//	Release model
	void Release();

	//	Set absolute position
	virtual void SetPos(const A3DVECTOR3& vPos);
	//	Set absolute forward and up direction
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	//	Set absolute transform matrix
	virtual void SetAbsoluteTM(const A3DMATRIX4& mat);

	const A3DMATRIX4& GetAbsTM()
	{
		if (m_pA3DSkinModel)
			return m_pA3DSkinModel->GetAbsoluteTM();

		return m_matAbsoluteTM;
	}

	//	Tick routine
	bool Tick(DWORD dwDeltaTime, bool bNoAnim = false);
	//	Sync routine after physics simulation
	//	!!!!!!!!!!!!!!!!!!!
	//	This function must be called no matter there is Physics simulation exist or not
	//	Some of the tick routines have been moved into this function due to corresponding the physics routine
	bool SyncModelPhys(DWORD dwDeltaTime);
	//	Render routine
	bool Render(A3DViewport* pViewport, bool bRenderAtOnce=false, bool bRenderSkinModel=true, 
		DWORD dwFlags=0, A3DSkinModelRenderModifier* pRenderModifier = NULL, bool bRenderGfx = true);
	//	Render action fx, no skin model
	void RenderActionFX(A3DViewport* pViewport);

	//	Add a new skin
	bool AddSkinFile(const char* szSkinFile);
	//	Add a new skin
	bool AddSkin(A3DSkin* pSkin, bool bAutoRem=true);
	//	Replace a skin
	bool ReplaceSkinFile(int iIndex, const char* szSkinFile, bool bAutoRem=true);
	//	Replace a skin
	bool ReplaceSkin(int iIndex, A3DSkin* pSkin, bool bAutoRem=true);
	//	Remove a skin item, this operation release both skin and skin item
	void RemoveSkinItem(int iItem);
	//	Get skin
	A3DSkin* GetA3DSkin(int iIndex);
	//	Release all skins
	void ReleaseAllSkins();
	//	Show / Hide skin
	void ShowSkin(int iIndex, bool bShow);
	//	GetSkinShowFlag
	bool IsSkinShown(int iIndex);
	//	Get skin number
	int GetSkinNum();

	//	Open one cloth skin
	//	iLinkType: 0 - default type; 1: soft link; 2: hard link
	//	Note: do not support parameters backup yet
	bool OpenClothSkin(int iSkinSlot, int iLinkType=0, int iClothSimDelayTime = 500, int iClothSimBlendTime = 800);
	//	Open all clothes
	//	iLinkType: 0 - default type; 1: soft link; 2: hard link
	bool OpenAllClothes(int iLinkType=0, int iClothSimDelayTime = 500, int iClothSimBlendTime = 800);
	//	Close one cloth skin
	void CloseClothSkin(int iSkinSlot, bool bCompletely=false);
	//	Close all clothes
	void CloseAllClothes(bool bCompletely=false);

	bool IsClothSimOpened() const { return m_bClothSimOpen;}
	const ClothSimDesc& GetClothSimParameters() const {	return m_ClothSimPara; }

	//	Open child model link
	bool OpenChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher);
	//	Add child model link (Must first open one child model phys link, then use add, the child attacher must not be used)
	bool AddChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher);
	//	Remove child model link (find the link between szParentHook and szChildAttacher, if there is one, remove it)
	bool RemoveChildModelLink(const char* szHanger, const char* szParentHook, const char* szChildAttacher);
	//	Close child model link
	void CloseChildModelLink(const char* szHanger);
	//	Child model change physical link
	void ChangeChildModelLink(const char* szHanger, const char* szNewHook);
	//	Change child model physical state
	//	iState: A3DModelPhysSync::PHY_STATE_ANIMATION / PHY_STATE_SIMULATE / (not prefered)PHY_STATE_PARTSIMULATE
	void ChangeChildModelPhysState(const char* szHanger, int iState);
	//	Change all child model physical state
	void ChangeChildModelPhysState(int iState);
	//	Set ECModel's collision channel, call this after A3DModelPhysics::Bind() or after EnablePhysSystem()
	//	iMethod: COLLISION_CHANNEL_INDEPENDENT - As indenpend collision channel
	//			 COLLISION_CHANNEL_AS_PARENT - As same as parent (if exist parent)
	//			 COLLISION_CHANNEL_AS_COMMON - As common (collision with all objects)
	bool SetCollisionChannel(int iMethod, CECModel* pParent = NULL, bool bSetChildCollisionChannel = true);

	//	Interface related for breakable
	//	Break a joint (Break a joint if exist)
	void BreakJoint(const char * szJointName);
	//	Break joint by bone
	void BreakJointByBone(const char * szBoneName);
	//	Apply force
	//	iPhysForceType can be one of the following values
	// 		PFT_FORCE,						// parameter has unit of mass * distance/ time^2, i.e. a force
	// 		PFT_IMPULSE,					// parameter has unit of mass * distance /time
	// 		PFT_VELOCITY_CHANGE,			// parameter has unit of distance / time, i.e. the effect is mass independent: a velocity change.
	// 		PFT_SMOOTH_IMPULSE,				// same as NX_IMPULSE but the effect is applied over all substeps. Use this for motion controllers that repeatedly apply an impulse.
	// 		PFT_SMOOTH_VELOCITY_CHANGE,		// same as NX_VELOCITY_CHANGE but the effect is applied over all substeps. Use this for motion controllers that repeatedly apply an impulse.
	// 		PFT_ACCELERATION				// parameter has unit of distance/ time^2, i.e. an acceleration. It gets treated just like a force except the mass is not divided out before integration.
	//  Note: not implement "scaled soft key frame force" yet
	//	fMaxVelChangeHint  hint the max value of velocity change, this parameter will be ignored if the ECM is a breakable model(no matter is broken or not).
	bool AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, float fForceMagnitude, float fMaxDist = FLT_MAX, int iPhysForceType = PFT_FORCE, float fSweepRadius = 0.0f, float fMaxVelChangeHint = FLT_MAX);

	// Apply a force to the actor which the specified bone is bound to.
	// Note: If the model is scaled and is using SoftKeyFrame mode, the applied force will be scaled internally to get almost the same force effect with non-scaled model. 
	bool AddForceToBone(const char* szBoneName, const A3DVECTOR3& vForceDir, float fForceMagnitude, int iPhysForceType = PFT_FORCE, float fMaxVelChange = 20.0f);

	//	Apply torque to make model rotation
	//  iRotateAxis
	//		rotate axis in model space
	//	iPhysTorqueType
	//		define the torque mode(the same enumeration value as iPhysForceType)
	//		default: PFT_VELOCITY_CHANGE  (angular velocity, unit: radian per second)
	//	dwDuration
	//		keep add torque in the duration. Only add torque once if the value is 0.
	//		default: 0 (unit: micro second)
	bool AddTorque(int iRotateAxis, float fTorqueMagnitude, DWORD dwDuration = 0, int iPhysTorqueType = PFT_VELOCITY_CHANGE);

	enum WINDFLAGS 
	{
		WF_DISABLE		= 0,
		WF_CLOTH		= (1<<0),
		WF_RIGIDBODY	= (1<<1),
		WF_ALL			= 0xffffffff,
	};
	struct WindInfo
	{
		WindInfo() { vBasicAcc.Clear(); fClothScale = 1.0f; fRBScale = 1.0f; }
		WindInfo(const A3DVECTOR3& vWindAcc) { vBasicAcc = vWindAcc; fClothScale = 1.0f; fRBScale = 1.0f; }
		WindInfo(const A3DVECTOR3& vWindAcc, float fScaleCloth, float fScaleRB) { vBasicAcc = vWindAcc; fClothScale = fScaleCloth; fRBScale = fScaleRB; }

		A3DVECTOR3 vBasicAcc;
		float	  fClothScale;
		float	  fRBScale;
	};

	void SetPhysXWindAcceleration(const A3DVECTOR3& vBasicAcc, bool bRecur = true, float fScaleCloth = 1.f, float fScaleRB = 1.f)
	{
		SetPhysXWindAcceleration(WindInfo(vBasicAcc, fScaleCloth, fScaleRB), bRecur);
	}
	void SetPhysXWindAcceleration(const WindInfo& info, bool bRecur = true);
	const WindInfo& GetPhysXWindAcceleration() const { return m_PhysXWindInfo; }
	void EnablePhysXWind(int flags = WF_ALL, bool bRecur = true);
	bool IsPhysXWindEnabled(int flags) const;

	//	Creator of EC_Model could set a break handler here
	//	which could be called back when PhysX makes a break to this model.
	void SetPhysBreakHandler(ON_PHYS_BREAK pPhysBreakHandler);

	//	This method is called back by the lower level objects in (A3DModelPhysSync::OnBreak)
	//	User should not call this method directly
	void OnPhysBreak(const PhysBreakInfo& physBreakInfo);

	void SetPhysHitGroundHandler(ON_PHYS_HITGROUND pPhysHitGroundHandler);
	void OnPhysHitGround(const ECMPhysXHitGroundInfo& ecmpHitGroundInfo);

	void SetHitGroundActorVelThresh(float fV) { m_fHitGroundActorVelThresh = fV; }
	float GetHitGroundActorVelThresh() const { return m_fHitGroundActorVelThresh; }

	//	Enable physical system
	//	Parameters:
	//	bHardLink == true: means the Skin is linked with the model physics objects by a physics joint and updated in physics system
	//				false: means the skin is updated by the model's hook position each time the model is animated
	//	bRecur	  == true: means recur this function on each child model, and try open child models' physics link
	//			    false: do nothing above
	bool EnablePhysSystem(bool bHardLink, bool bRecur = false, bool bOpenCloth = true);
	//	Disable physical system and enforce set to animation state
	void DisablePhysSystem();
	//  If the model is in PhysX simulation state with rag-doll model, stop all actions and keep current bones pose(enter static state, skin model would not be updated) beside disable physical system.
	//  For other cases except above motioned, just the same with DisablePhysSystem()
	//  Return value:
	//		true ---- execute extend function successful(enter the static state)
	//		false --- just call DisablePhysSystem()
	//  Note: 
	//		Make sure call this before ECM.Tick() function if using in callback or asynchronous mode.
	//		if true was returned, EnablePhysSystem() will not available any more until the model released.
	//		if true was returned, PlayActionByName() will not available any more until the model released.
	bool DisablePhysSystemEx();

	//	Forbid Create Phys flag
	static void ForbidPhysX(bool bForbid) { s_bForbidPhys = bForbid; }
	static bool IsPhysXForbidden();

	//	Scale specified bone
	bool ScaleBone(const char* szBone, int iScaleType, const A3DVECTOR3& vScale);
	//	Get bone's scale information
	bool GetBoneScaleInfo(const char* szBone, int* piScaleType, A3DVECTOR3& vScale);
	//	Set bone trans flag
	void SetBoneTransFlag(const char* szBone, bool bFlag);
	
	bool ScaleBoneEx(const char* szBone, const A3DVECTOR3& vScale);

	//	Automatically calculate foot offset caused by bone scaling
	//	Return:
	//		0: succ
	//		-1: A3DConfig FlagNewBoneScale not set
	//		-2: Bones Size <= 0
	//		-10: No Skeleton
	int CalcFootOffset(const char* szBaseBone);

	CECModelStaticData* GetStaticData() const { return m_pMapModel; }
	//	Get A3D skin model object
	A3DSkinModel* GetA3DSkinModel() const { return m_pA3DSkinModel; }
	//	Get parent model
	CECModel* GetParentModel() const { return m_pParentModel; }
	//	Get auto-update flag
	bool GetAutoUpdateFlag() const { return m_bAutoUpdate; }
	//	Set auto-update flag
	void SetAutoUpdateFlag(bool bAuto) { m_bAutoUpdate = bAuto; }
	//	Get slowest update flag
	bool GetSlowestUpdateFlag() const { return m_bSlowestUpdate; } 
	//	Set slowest update flag
	void SetSlowestUpdateFlag(bool bTrue) { m_bSlowestUpdate = bTrue; } 

	bool GetDefAutoUpdateFlag() const { return m_pMapModel->m_bAutoUpdate; }
	void SetDefAutoUpdateFlag(bool bAuto) { m_pMapModel->m_bAutoUpdate = bAuto; }

	//	Combination action interfaces
	int GetComActCount() const { return (int)m_pMapModel->m_ActionMap.size(); }

	A3DCombinedAction* GetComActByIndex(int nIndex)
	{
		CombinedActMap::iterator it = m_pMapModel->m_ActionMap.begin();
		while (nIndex--) ++it;
		return it->second;
	}

	A3DCombinedAction* GetComActByName(const AString& strName)
	{
		if (m_pMapModel == NULL)
			return NULL;

		CombinedActMap& ActionMap = m_pMapModel->m_ActionMap;
		CombinedActMap::iterator it = ActionMap.find(strName);
		if (it == ActionMap.end())
			return NULL;

		return it->second;
	}

	DWORD GetComActTimeSpanByName(const AString& strName);
	void AddCombinedAction(A3DCombinedAction* pAct);
	void RemoveCombinedAction(const AString& strName);
	bool RenameCombinedAction(A3DCombinedAction* pAct, const AString& strNewName);

	int GetAdditionalSkinCount() const { return (int)m_pMapModel->m_AdditionalSkinLst.size(); }
	const AString& GetAdditionalSkin(int nIndex) const { return m_pMapModel->m_AdditionalSkinLst[nIndex]; }
	void AddAdditionalSkin(const char* szFile) { m_pMapModel->m_AdditionalSkinLst.push_back(szFile); }
	void DelAdditionalSkin(int nIndex) { m_pMapModel->m_AdditionalSkinLst.erase(&m_pMapModel->m_AdditionalSkinLst[nIndex]); }
	ParticleBoneArray&	GetParticleBoneLst() { return m_pMapModel->m_PaticleBoneLst; }

	//	Play a combined action by name
	//	nChannel	: Action channel
	//	szActName	: Action name
	//	fWeight		: Weight used in action blending
	//	bRestart	: Whether to replay this action while already playing the exact same action
	//	nTransTime	: Length of interpolation time with last action
	//	bForceStop	: Stop the current playing action
	//	dwUserData	: User data you may set in and be used later (get it by GetCurActionUserData)
	//	bNoFx		: Do not play effects on this action
	//	bNoCamShake	: Do not play cam shake on this action
	bool PlayActionByName(const char* szActName, float fWeight, bool bRestart=true, int nTransTime=ECM_ACTION_TRANSTIME, bool bForceStop=true, DWORD dwUserData=0, bool bNoFx=false, bool bNoCamShake= false);
	bool PlayActionByName(int nChannel, const char* szActName, float fWeight, bool bRestart=true, int nTransTime=ECM_ACTION_TRANSTIME, bool bForceStop=true, DWORD dwUserData=0, bool bNoFx=false, bool bNoCamShake=false);

	// --------------------------------------------------------------------------------------------
	// action blend
	typedef struct _WALKRUNBLEND_INFO
	{
		float	m_fWalkSpeed;						//	走的速度
		float	m_fRunSpeed;						//	跑的速度
		A3DCombinedAction*	m_pWalkRunCombAct;		//	走跑的组合动作

	} WALKRUNBLEND_INFO, * PWALKRUNBLEND_INFO;

	typedef struct _WALKRUNBLEND_PARAMS
	{
		float	m_fWalkSpeed;			//	走的速度
		float	m_fRunSpeed;			//	跑的速度
		AString	m_strWalkBaseAct;		//	走的基础动作名
		AString m_strRunBaseAct;		//	跑的基础动作名
		AString	m_strWalkCombAct;		//	走的组合动作名（为了取事件）
		AString m_strRunCombAct;		//	跑的组合动作名（为了取事件）
		AString m_strWalkRunCombAct;	//	走跑的组合动作名
		
	} WALKRUNBLEND_PARAMS, * PWALKRUNBLEND_PARMAS;

	bool InitWalkRunBlend( const _WALKRUNBLEND_PARAMS& c_params );
	const _WALKRUNBLEND_INFO& GetWalkRunBlendInfo() const { return m_WalkRunBlendInfo; }
	bool SetWalkRunVelocity( float fVelocity, int nBlendInTime );

	class IWalkRunUpDownBlendCB
	{
	public:

		virtual bool RayTrace( const A3DVECTOR3& c_vStart, const A3DVECTOR3& c_vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fFraction ) = 0;
		virtual bool IsWalkRunChannel( int iChannel ) = 0;
	};

	typedef struct _WALKRUNUPDOWNBLEND_INFO
	{
		float	m_fWalkSpeed;							//	走的速度
		float	m_fRunSpeed;							//	跑的速度
		A3DCombinedAction*	m_pWalkRunUpDownCombAct;	//	走跑上下坡的组合动作
		IWalkRunUpDownBlendCB*	m_pCB;					//	回调接口

	} WALKRUNUPDOWNBLEND_INFO, * PWALKRUNUPDOWNBLEND_INFO;

	typedef struct _WALKRUNUPDOWNBLEND_PARAMS
	{
		float	m_fWalkSpeed;				//	走的速度
		float	m_fRunSpeed;				//	跑的速度
		AString	m_strWalkBaseAct;			//	走的基础动作名
		AString m_strRunBaseAct;			//	跑的基础动作名
		AString	m_strWalkUpBaseAct;			//	走上坡的基础动作名
		AString m_strWalkDownBaseAct;		//	走下坡的基础动作名
		AString	m_strRunUpBaseAct;			//	跑上坡的基础动作名
		AString m_strRunDownBaseAct;		//	跑下坡的基础动作名
		AString	m_strWalkCombAct;			//	走的组合动作名（为了取事件）
		AString m_strRunCombAct;			//	跑的组合动作名（为了取事件）
		AString	m_strWalkRunUpDownCombAct;	//	走跑上下坡的组合动作名
		IWalkRunUpDownBlendCB*	m_pCB;		//	回调接口

	} WALKRUNUPDOWNBLEND_PARAMS, * PWALKRUNUPDOWNBLEND_PARMAS;

	bool InitWalkRunUpDownBlend( const _WALKRUNUPDOWNBLEND_PARAMS& c_params );
	const _WALKRUNUPDOWNBLEND_INFO& GetWalkRunUpDownBlendInfo() const { return m_WalkRunUpDownBlendInfo; }
	bool SetWalkRunUpDownVelocity( float fVelocity, int nBlendInTime );
	void EnableSlopAnimBlend(bool bEnable);//enable slope animation blend
	bool IsSlopAnimBlendEnabled() const;//is slope animation blend enabled?
	//get & set slope animation angle threshold. angle above this slope animation weight 1, 0~fAngleDeg slope animation weight 0 - 1.
	void SetSlopAnimAngle(float fAngleDeg);
	float GetSlopAnimAngle() const;

	typedef struct _RUNTURNBLEND_INFO
	{
		float	m_fRunSpeed;						//	跑的速度
		A3DCombinedAction*	m_pRunTurnCombAct;		//	跑动转身的组合动作

	} RUNTURNBLEND_INFO, * PRUNTURNBLEND_INFO;

	typedef struct _RUNTURNBLEND_PARAMS
	{
		float	m_fRunSpeed;			//	跑的速度
		AString m_strRunBaseAct;		//	跑的基础动作名
		AString	m_strTurnLeftBaseAct;	//	左转的基础动作名
		AString m_strTurnRightBaseAct;	//	右转的基础动作名
		AString m_strRunCombAct;		//	跑的组合动作名（为了取事件）
		AString m_strRunTurnCombAct;	//	跑动转身的组合动作名

	} RUNTURNBLEND_PARAMS, * PRUNTURNBLEND_PARMAS;

	bool InitRunTurnBlend( const _RUNTURNBLEND_PARAMS& c_params );
	const _RUNTURNBLEND_INFO& GetRunTurnBlendInfo() const { return m_RunTurnBlendInfo; }
	bool SetRunTurnVelocity( float fVelocity );
	bool SetRunTurnWeight(float fWeight1, float fWeight2, int nBlendInTime);

	bool SetAnimNodeSpeed( int iSpecType, float fSpeed );
	bool SetAnimNodeWeight( int iSpectType, float fWeight1, float fWeight2, int iBlendInTime );

	CECAnimNodeMan*	GetAnimNodeMan() const { return m_pECAnimNodeMan; }
	// ============================================================================================
 
	//	Queue an action into the waiting list
	//	szActName	: Action name
	//	nTransTime	: Length of interpolation time with last action
	//	dwUserData	: User data you may set in and be used later (get it by GetCurActionUserData)
	//	bForceStopPRevAct	: Do not wait until the end of all previous action's GFXs
	//	bCheckTailDup		: Check the tail action of queue, if same, ignore this one
	//	bNoFx				: Do not play effects on this action
	bool QueueAction(const char* szActName, int nTransTime=ECM_ACTION_TRANSTIME, DWORD dwUserData=0, bool bForceStopPrevAct=false, bool bCheckTailDup = false, bool bNoFx = false, bool bResetSpeed = false)
	{
		return QueueAction(0, szActName, nTransTime, dwUserData, bForceStopPrevAct, bCheckTailDup, bNoFx, bResetSpeed);
	}

	bool QueueAction(int nChannel, const char* szActName, int nTransTime=ECM_ACTION_TRANSTIME, DWORD dwUserData=0, bool bForceStopPrevAct=false, bool bCheckTailDup = false, bool bNoFx = false, bool bResetSpeed = false);

	bool RemoveQueuedActions(const char* szActName)
	{
		return RemoveQueuedActions(0, szActName);
	}
	
	bool RemoveQueuedActions(int nChannel, const char* szActName);

	void SetComActFlag(bool* pFlag, DWORD dwFlagMode) {	SetComActFlag(0, pFlag, dwFlagMode); }
	void SetComActFlag(int nChannel, bool* pFlag, DWORD dwFlagMode)
	{
		ChannelAct& ca = m_ChannelActs[nChannel];
		ChannelActNode* pNode = ca.GetHighestRankNode();

		if (pNode)
		{
			pNode->m_pActFlag = pFlag;
			pNode->m_dwFlagMode = dwFlagMode;
		}
	}

	void ClearComActFlag(bool bSignalCurrent) { ClearComActFlag(0, bSignalCurrent); }
	void ClearComActFlag(int nChannel, bool bSignalCurrent)
	{
		ChannelAct& ca = m_ChannelActs[nChannel];
		ChannelActNode* pNode = ca.GetHighestRankNode();

		if (pNode)
		{
			if (pNode->m_pActFlag && bSignalCurrent)
				*pNode->m_pActFlag = true;

			//ca.m_dwFlagMode = COMACT_FLAG_MODE_NONE;
			pNode->m_pActFlag = NULL;
		}
	}

	void StopAllActions(bool bStopFx = false);
	void StopChildrenAct();
	void StopChannelAction(int nChannel, bool bStopAct, bool bStopFx = false);
	void StopChannelAction(int nChannel, BYTE rank, bool bStopAct, bool bStopFx = false);

	bool AddECMHook(const char* szName, int nID, float fScale, const A3DVECTOR3& vPos);
	bool RenameECMHook(const char* szOldName, const char* szNewName);
	void RemoveECMHook(const char* szName);
	int GetECMHookCount() const;
	CECModelHook* GetECMHook(int iIndex) const;
	CECModelHook* GetECMHook(const char* szName) const;

private:

	bool IsECMHookValid(const ECModelHookMap& hookMap, const char* szName) const;
	A3DMATRIX4 GetGfxBindHookTransMat(GFX_INFO* pInfo, bool& bIsHookMat);
	bool IsRenderSkinModel(bool bRenderSkinModel) const;

	//	Tick ECModel combined actions
	void UpdateChannelActs(DWORD dwUpdateTime);
	//	Tick ECModel combined action events
	void UpdateChannelEvents(DWORD dwActTime, DWORD dwEventTime);
	//	Tick Skin Model
	void TickSkinModel(DWORD dwUpdateTime);
	//	Tick CoGfx
	void TickCoGfx(DWORD dwUpdateTime);
	//	Tick Child Models
	void TickShownChildModels(DWORD dwDeltaTime, bool bNoAnim);
	//	Tick after phys sync
	bool TickRelyOnPhysics(DWORD dwUpdateTime, DWORD dwActionUpdateTime, DWORD dwEventUpdateTime);

	//	Set parent model
	void SetParentModel(CECModel* pParent);

	//	传入：dwDeltaTime是真实传入的时间
	//	传出：pUpdateTime是乘以播放速度之后的更新时间
	//	传出：pActionTickTime是UpdateTime乘以SkinModelTRCtrl的TickSpeed或者0，取决于bNoAnim标志
	//	传出：pEventTickTime是dwUpdateTime或者dwDeltaTime，取决于事件是否使用IsEventUsePlaySpeed()标志
	void GetTickTimes(DWORD dwDeltaTime, bool bNoAnim, DWORD* pUpdateTime, DWORD* pActionTickTime, DWORD* pEventTickTime);
	//	dwUpdateTime 是已经考虑过CECModel的PlaySpeed的Time
	//	bNoAnim 表示是否不播放动画
	//	返回值是考虑了动画停播状态标志，以及由GFX模型代理元素对SkinModelTRControl造成的影响所得到的实际的Action Tick Time
	DWORD GetActionTickTime(DWORD dwUpdateTime, bool bNoAnim) const;
	//	dwUpdateTime 是已经考虑过CECModel的PlaySpeed的Time
	//	dwDeltaTime 是游戏中实际经过的时间（不考虑PlaySpeed的Time）
	//	返回的EventTickTime，是依照IsEventUsePlaySpeed()状态所选择的TickTime
	DWORD GetEventTickTime(DWORD dwUpdateTime, DWORD dwDeltaTime) const;
	//	GetAccuPlaySpeed 返回从父模型开始直到当前模型所有PlaySpeed的累积影响
	float GetAccuPlaySpeed() const;

	//	是否应当移除MotionBlurInfo
	bool IsShouldRemoveMotionBlurInfo(int nChannel) const;

public:
	
	void PlayGfx(const char* szPath, const char* szHook, float fScale = 1.0f, bool bFadeOut = true, bool bUseECMHook = false, DWORD dwFadeOutTime = 0);
	//	Play Gfx with offset, pitch, yaw and rot : Called by script in ECM_PlayGfxEx
	void PlayGfx(const char* szPath, const char* szHook, float fScale, bool bFadeOut, const A3DVECTOR3& vOffset, float fPitch, float fYaw, float fRot, bool bUseECMHook = false, DWORD dwFadeOutTime = 0);
	
	void RemoveGfx(const char* szPath, const char* szHook);
	void RemoveGfx(const char* szKey);
	A3DGFXEx* GetGfx(const char* szPath, const char* szHook);

	A3DSMActionChannel* BuildActionChannel(int iChannel, int iNumJoint, int* aJoints)
	{
		if (m_pA3DSkinModel)
			return m_pA3DSkinModel->BuildActionChannel(iChannel, iNumJoint, aJoints);
		else
			return NULL;
	}

	A3DSMActionChannel* GetActionChannel(int iChannel)
	{
		if (m_pA3DSkinModel)
			return m_pA3DSkinModel->GetActionChannel(iChannel);
		else
			return NULL;
	}

	ActChannelInfo** GetActChannelInfo() { return m_pMapModel ? m_pMapModel->m_ChannelInfoArr : 0; }

	A3DCombActDynData* GetCurAction(int nChannel)
	{
		assert(nChannel >= 0 && nChannel < A3DSkinModel::ACTCHA_MAX);
		ChannelAct& ca = m_ChannelActs[nChannel];
		ChannelActNode* pNode = ca.GetHighestRankNode();

		if (pNode)
			return pNode->m_pActive;
		else
			return 0;
	}

	bool IsActionStopped(int nChannel);

	//	Get current action's user data
	DWORD GetCurActionUserData(int nChannel);
	DWORD GetCurActionUserData() { return GetCurActionUserData(0); }


	//	Set Action Event Mask will make some type of the event be enabled
	//	Use the method like this:
	//	SetActionEventMask(nChannel, EVENT_MASK_GFX) -> this will make all events not be played except the GFX
	//	the definition of EVENT_MASK_GFX and other related could be see in A3DCombinedAction.h
	//	Use EVENT_MASK_ALL to enable all events, which is the default value
	void SetActionEventMask(int nChannel, DWORD dwMask) { m_EventMasks[nChannel] = dwMask; }
	DWORD GetActionEventMask(int nChannel) const { return m_EventMasks[nChannel]; }

	//	Is Real Shown
	bool IsShown() const { return m_bRealShown; }

	bool IsNeedUpdateWhenHidden() const { return m_bNeedUpdateWhenHidden; }

	//	Show Simply
	void Show(bool bShow, bool bAffectChild = true, bool bUpdateWhenHidden = false);
	void ShowChildModel(const char* szHangerName, bool bShow, bool bUpdate)
	{
		CECModel* pChild = GetChildModel(szHangerName);

		if (pChild)
			pChild->Show(bShow, true, bUpdate);
	}
	bool IsChildModelShown(const char* szHangerName) const
	{
		CECModel* pChild = GetChildModel(szHangerName);
		if (pChild)
			return pChild->IsShown();
		
		return false;
	}

	//	特定HH，隐藏（与Parent相同id的）子模型
	void ClearHideModelHHVec();
	void AddHideModelHH(const abase::vector<AString>& vecHH);
	void RemoveHideModelHH(const abase::vector<AString>& vecHH);
	bool FindHideModelHH(const AString& strHH) const
	{
		HideModelHHMap::const_iterator it = m_HideModelHHMap.find(strHH);
		if (it == m_HideModelHHMap.end())
			return false;

		return it->second > 0;
	}
	void SetHideByHiddenHH(bool bSet, bool bAffectChild = true, bool bUpdateWhenHidden = false);
	bool IsHideByHiddenHH() const { return m_bHideByHiddenHH; }

	//	Request call will request the skin model be rendered or not
	//	But may not actually take effect (because there maybe a lot of callers ask the state to be changed, especially by the GFX model Proxy element)
	//	We use a reference count tech here to control the skin model be rendered or not
	SkinModelTRControl& GetSkinModelTRCtrl() { return m_SkinModelRenderCtrl; }
	const SkinModelTRControl& GetSkinModelTRCtrl() const { return m_SkinModelRenderCtrl; }

	//	szHangerName : a name that specify this particular child model
	//	szHookName : this model's hook name on which the child model will be hanged on
	//	szCCName : child model's hook name by which the child model is hooked
	bool AddChildModel(const char* szHangerName, bool bOnBone, const char* szHookName, const char* szModelFile, const char* szCCName, const bool* pbSetHangerInheritScale = NULL, DWORD dwBindFlag = 0);
	bool AddChildModel(const char* szHangerName, bool bOnBone, const char* szHookName, CECModel* pChild, const char* szCCName, const bool* pbSetHangerInheritScale = NULL, DWORD dwBindFlag = 0);
	bool RemoveChildModel(const char* szHangerName, bool bDel = true);
	int GetChildCount() const { return (int)m_ChildModels.size(); }
	bool ChildChangeHook(const char* szHanger, const char* szNewHook);

	CECModel* GetChildModel(const char* szChild) const
	{
		ECModelMap::const_iterator it = m_ChildModels.find(szChild);
		if (it == m_ChildModels.end()) return NULL;
		return it->second;
	}

	CECModel* GetChildModel(int nIndex) const
	{
		ECModelMap::const_iterator it = m_ChildModels.begin();
		while (nIndex--) ++it;
		return it->second;
	}

	const AString& GetHangerName(int nIndex) const
	{
		ECModelMap::const_iterator it = m_ChildModels.begin();
		while (nIndex--) ++it;
		return it->first;
	}

	const char* GetHangerName(CECModel* pModel) const
	{
		for (ECModelMap::const_iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		{
			if (it->second == pModel)
				return it->first;
		}

		return NULL;
	}

	static int s_nCoGfxCount;

	void AddCoGfx(GFX_INFO* pInfo);
	void DelCoGfx(GFX_INFO* pInfo);
	const char* GetCoGfxName(GFX_INFO* pInfo);
	CoGfxMap& GetCoGfxMap() { return m_CoGfxMap; }
	void UpdateCoGfx(const char* strIndex, GFX_INFO* pSrc);

	void QueueFadeOutSfx(AM3DSoundBuffer* pSfx) { m_FadeOutSfxLst.AddTail(pSfx); }
	const AString& GetFilePath() const { return m_pMapModel->m_strFilePath; }
	bool ParentActivate(const AString& strActName, const ChildActInfo* pHostInfo, CECModel* pParentModel, DWORD dwDeltaTime);
	void ActivateChildAct(const AString& strActName, const ChildActInfo* pHostInfo, CECModel* pParentModel, DWORD dwDeltaTime);

	void ResetMaterialOrg();

	void ResetMaterialScale();

	void SetPlaySpeed(float fSpeed) { m_fPlaySpeed = fSpeed * m_pMapModel->m_fDefPlaySpeed; }
	float GetPlaySpeed() const { return m_fPlaySpeed; }

	BoneScaleArray& GetBoneScales() { return m_pMapModel->m_BoneScales; }
	BoneScale* GetBoneScaleByIndex(int nIndex)
	{
		size_t i;

		for (i = 0; i < m_pMapModel->m_BoneScales.size(); i++)
			if (m_pMapModel->m_BoneScales[i]->m_nIndex == nIndex)
				return m_pMapModel->m_BoneScales[i];

		return NULL;
	}

	void RemoveBoneScale(BoneScale* pScale);
	void UpdateBoneScales();

	BoneScaleExArray& GetBoneScaleExArr() { return m_pMapModel->m_BoneScaleExArr; }
	BoneScaleEx* GetBoneScaleExByIndex(int nIndex)
	{
		for (size_t i = 0; i < m_pMapModel->m_BoneScaleExArr.size(); i++)
			if (m_pMapModel->m_BoneScaleExArr[i]->m_nIndex == nIndex)
				return m_pMapModel->m_BoneScaleExArr[i];

		return NULL;
	}

	void RemoveBoneScaleEx(BoneScaleEx* pScale);
	void UpdateBoneScaleEx();
	void UpdateBoneScaleEx(BoneScaleExArray& arr, const char* szBaseBone);
	const AString& GetBaseBoneName() const { return m_pMapModel->m_strScaleBaseBone; }
	void SetBaseBoneName(const char* szName) { m_pMapModel->m_strScaleBaseBone = szName; }

	//	Set aabb type
	void SetAABBType(int iType);

	//	shadow cast manipulation
	inline bool GetCastShadow()	const		{ return m_bCastShadow; }
	inline void SetCastShadow(bool flag)	{ m_bCastShadow = flag; }

	const char * GetSkinModelPath()			{ return m_pMapModel->m_strSkinModelPath; }
	CHBasedCD::ConvexHullDataArray& GetConvexHullData() { return m_pMapModel->GetConvexHullData(); }
	bool HasCHAABB() const;
	const A3DAABB& GetCHAABB() const { return m_pMapModel->GetCHAABB(); }

	//	Get Model AABB 
	//	If skinmodel exist, return m_ModelAABB 
	//	which is updated by Skin model and all visible child models
	//	Else return the CH AABB
	const A3DAABB& GetModelAABB() const;

	void SetAffectedByParent(bool b) { m_bCanAffectedByParent = b; }
	bool CanAffectedByParent() const { return m_bCanAffectedByParent; }

	float GetTransparent() const { return m_fAlpha == 1.0f ? -1.0f : 1.0f - m_fAlpha; }
	void SetTransparent(float fTransparent);

	float GetOuterAlpha() const { return m_fOuterAlpha; }
	float GetInnerAlpha() const { return m_fInnerAlpha; }

	void SetSrcBlend(A3DBLEND srcBlend);
	void SetDstBlend(A3DBLEND dstBlend);

	const A3DCOLORVALUE& GetColor() const { return m_Color; }
	void SetColor(const A3DCOLORVALUE& cl);
	void ApplyColor();

	void Reset()
	{
		m_bPhysEnabled = true;		// turn on physics by default...

		if (!m_pMapModel)
			return;

		m_pMapModel->Reset();
		
	}

	//	Build wound action channel
	bool BuildWoundActionChannel();

	bool IsPlayingComAct() const
	{
		const ChannelAct& ca = m_ChannelActs[0];
		return ca.GetHighestRankNode()!= 0;
	}

	A3DCombActDynData* GetCurComAct()
	{
		ChannelAct& ca = m_ChannelActs[0];
		ChannelActNode* pNode = ca.GetHighestRankNode();
		return pNode ? pNode->m_pActive : 0;
	}

	bool SetSkinModel(A3DEngine* pEngine, const char* szFile);
	bool SetSkinModel(A3DSkinModel* pSkinModel);
	void RemoveSkinModel();

	//	Interfaces for modifying m_pMapModel data
	const A3DSHADER& GetBlendMode() const;
	void SetBlendMode(const A3DSHADER& bld);
	A3DCOLOR GetOrgColor() const;
	void SetOrgColor(A3DCOLOR cl);
	A3DCOLOR GetExtraEmissive() const;
	void SetExtraEmissive(A3DCOLOR cl);
	float* GetOuterData();
	float GetDefPlaySpeed() const;
	void SetDefPlaySpeed(float fSpeed);
	const bool CanCastShadow() const;
	void SetCanCastShadow(bool b);
	const bool CanRenderSkinModel() const;
	void SetRenderSkinModel(bool b);
	bool CanRenderEdge() const;
	void SetRenderEdge(bool b);

	void SetUseAbsTrack(bool b) { m_bAbsTrack = b; }

	//	Set scale to all root-bones and gfx
	//	bMulSclInfo will take the bone scale info into consideration
	bool ScaleAllRootBonesAndGfx(float fScale);
	float	GetAllRootBonesScale() const { return m_fRootBoneScaleFactor; }
	//	Set a scale factor for gfx on this ecmodel
	//	Set gfx size scale and (added 2009.10.23) gfx offset scale relative to the hook.
	float GetGfxScale() const { return m_fGfxScaleFactor; }
	void SetGfxScale(float fScale) { m_fGfxScaleFactor = fScale; m_bGfxScaleChanged = true; }

	bool IsGfxUseLOD() const { return m_bGfxUseLod; }
	//	Set Gfx Use Lod will affect on all gfxs currently loaded, default is with force for compatibility
	inline void SetGfxUseLOD(bool b) { SetGfxUseLOD(b, true); }
	//	bForce means do not check current state, and force set property to all related gfxs
	void SetGfxUseLOD(bool b, bool bForce);

	//	Is this ECModel and all related GFX disable shaking camera
	inline bool GetDisableCamShake() const { return m_bDisableCamShake; }
	//	Set disable cam shake will affect on all gfxs currently loaded
	void SetDisableCamShake(bool b);

	//	Is this ECModel and all related GFX enable camera blur
	inline bool GetEnableCamBlur() const { return m_bEnableCamBlur; }
	//	Set enable cam blur will affect on all gfxs currently loaded
	void SetEnableCamBlur(bool b);

	//	Get / Set this ECModel is created by GFX or not
	inline bool IsCreatedByGfx() const { return m_bCreatedByGfx; }
	void SetCreatedByGfx(bool b) { m_bCreatedByGfx = b; }


	int GetSfxPriority() const { return m_nSfxPriority; }
	void SetSfxPriority(int n) { m_nSfxPriority = n; }
	int GetMainChannel() const { return m_nMainChannel; }
	void SetMainChannel(int n) { m_nMainChannel = n; }
	bool GetFashionMode() const { return m_bFashionMode; }
	bool HasEquip(int id) const { return m_EquipSlotMap.find(id) != m_EquipSlotMap.end(); }

	int GetEquipId(int nIndex) const
	{
		abase::hash_map<int, int>::const_iterator it = m_SlotEquipMap.find(nIndex);

		if (it == m_SlotEquipMap.end())
			return 0;
		else
			return it->second;
	}

	bool GetUpdateLightInfoFlag() const { return m_bUpdateLightInfo; }
	void SetUpdateLightInfoFlag(bool b) { m_bUpdateLightInfo = b; }

	bool RegisterAbsTrackOfBone(const char* szBoneName)
	{
		if (!m_pA3DSkinModel)
			return false;

		return m_pA3DSkinModel->RegisterAbsTrackOfBone(szBoneName);
	}
	const AString& GetHookName() const { return m_strHookName; }
	const AString& GetCCName() const { return m_strCC; }

	bool PlayAttackAction(const char* szActName, unsigned char SerialId, clientid_t nCasterId, clientid_t nCastTarget, const A3DVECTOR3* pFixedPoint, DWORD dwUserData = 0, bool bNoFx = false, bool bNoCamShake = false)
	{
		return PlayAttackAction(0, szActName, ECM_ACTION_TRANSTIME, SerialId, nCasterId, nCastTarget, pFixedPoint, dwUserData, bNoFx, bNoCamShake);
	}

	bool PlayAttackAction(int nChannel, const char* szActName, unsigned char SerialId, clientid_t nCasterId, clientid_t nCastTarget, const A3DVECTOR3* pFixedPoint, DWORD dwUserData = 0, bool bNoFx = false, bool bNoCamShake = false)
	{
		return PlayAttackAction(nChannel, szActName, ECM_ACTION_TRANSTIME, SerialId, nCasterId, nCastTarget, pFixedPoint, dwUserData, bNoFx, bNoCamShake);
	}

	bool PlayAttackAction(int nChannel, const char* szActName, int nTransTime, unsigned char SerialId, clientid_t nCasterId, clientid_t nCastTarget, const A3DVECTOR3* pFixedPoint, DWORD dwUserData, bool bNoFx, bool bNoCamShake);

	void AddOneAttackDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage)
	{
		AddOneAttackDamageData(0, nCaster, nTarget, SerialId, dwModifier, nDamage);
	}

	void AddOneAttackDamageData(int nChannel, clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage, bool bIsAttDelay = false, int nAttDelayTimeIdx = 0);

	void OnScriptChangeEquip(int nEquipId, int nEquipFlag, bool bFashionMode, int nPathId = 0, int nEquipIndex = 0);

	A3DModelPhysics* GetModelPhysics() const { return m_pModelPhysics; }
	A3DModelPhysSync* GetPhysSync() const;
	bool IsInPartSimulate() const;

	//	Get PhysBlendOverlap
	CECModelPhysBlendOverlap* GetPhysBlendOverlap() { return m_pOverlapBlend; }
	//	Create PhysBlendOverlap
	//		iBlendMode: CECModelPhysBlendOverlap::BM_SOFT_KEYFRAME
	//					CECModelPhysBlendOverlap::BM_PURE_PHYSXDRV
	CECModelPhysBlendOverlap* CreatePhysBlendOverlap(int iBlendMode, const char* szForceBone,
		float fTouchRadius, float fMaxPhysWeight, float fForceValue = 2.f,
		DWORD dwFadeInTime = 200, DWORD dwHoldTime = 0, DWORD dwFadeOutTime = 300);
	
	//	Resize physical actors and clothes after model size changed
	bool ResizePhysObjects();
	
	//	Add a combo model
	void AddComboModel(CECModel* pModel) { m_aComboModels.Add(pModel); }
	//	Remove combo model
	void RemoveComboModel(CECModel* pModel);
	void RemoveComboModelByIndex(int n) { m_aComboModels.RemoveAt(n); }
	//	Find combo model
	int FindComboModel(CECModel* pModel) { return m_aComboModels.Find(pModel); }
	//	Get combo model number
	int GetComboModelNum() { return m_aComboModels.GetSize(); }
	//	Get a combo model by index
	CECModel* GetComboModel(int n) { return m_aComboModels[n]; }

	//	Is HumanBing Skeleton?
	bool IsHumanBeingSkeleton() const { return m_pMapModel && m_pMapModel->m_nHumanbingSkeleton == 1; }

	//	Used by editor
	int GetLastError() const { return m_nErrCode; }

	bool SetReplaceShader(const char* szShader, const char* szShaderTex, const GfxPSConstVec& consts);
	void RemoveReplaceShader();

	typedef void (WINAPI *GFX_NOTIFY_DELAY_START_FUNC)(clientid_t nTargetId, clientid_t nCasterId, int nDelayTime);
	typedef void (*ECM_CUSTOM_RENDER_FUNC)(A3DViewport* pView, A3DSkinModel* pSkinModel, A3DShader* pRefShader, const GfxPSConstVec& ConstsVec, DWORD dwPSPlayedTime, const AString& strPrefix, bool bForceAlpha);
	typedef bool (*ECM_GET_LIGHT_INFO_FUNC)(const A3DVECTOR3& vPos, A3DSkinModel::LIGHTINFO& info, DWORD& dwUniqueId);

	static GFX_NOTIFY_DELAY_START_FUNC m_NotifyDelayStartFunc;
	static GFX_NOTIFY_DELAY_START_FUNC GetNotifyDelayStartFunc() { return m_NotifyDelayStartFunc; }
	static void SetNotifyDelayStartFunc(GFX_NOTIFY_DELAY_START_FUNC _func) { m_NotifyDelayStartFunc = _func; }

	static ECM_CUSTOM_RENDER_FUNC m_CustomRenderFunc;
	static ECM_CUSTOM_RENDER_FUNC GetCustomRenderFunc() { return m_CustomRenderFunc; }
	static void SetCustomRenderFunc(ECM_CUSTOM_RENDER_FUNC _func) { m_CustomRenderFunc = _func; }
	static void FinalizeCustomRenderFunc();

	static ECM_GET_LIGHT_INFO_FUNC m_GetLightInfoFunc;
	static void SetLightInfoFunc(ECM_GET_LIGHT_INFO_FUNC _func) { m_GetLightInfoFunc = _func; }

	static bool IsHumanBeingSkeleton(const A3DSkeleton* pSekelton);

	// close the clothes of all child models which is bound to the bone hanger...
	void CloseOnBoneChildCloth(bool bRecursive = false);
	void OpenOnBoneChildCloth(bool bRecursive = false);

protected:	//	Attributes

	static bool			s_bForbidPhys;			//	static flag indicates whether phys is forbidden

	bool				m_bCreatedByGfx;		//	flag indicates whether this ecmodel is created by a GFX (ECModel element type)

	bool				m_bDisableCamShake;		//	flag indicates whether this ecmodel and all its related gfx will shake camera, false for default
	bool				m_bEnableCamBlur;		//	flag indicates whether this ecmodel and all its related gfx will enable camera blur, false for default

	bool				m_bCastShadow;			//	flag indicates whether it will cast shadow
	bool				m_bAutoUpdate;			//	true, automatically change update frequence
	bool				m_bSlowestUpdate;		//	true, force to use slowest auto update frequence
	int					m_nErrCode;				//	Last error code
	A3DAABB				m_ModelAABB;			//	ECModel's aabb (updated by skinmodel and all child models)
	SkinModelTRControl m_SkinModelRenderCtrl;	//	Is skin model is rendered (controlled by reference count)

	A3DDevice*			m_pA3DDevice;
	A3DSkinModel*		m_pA3DSkinModel;		//	A3D model object
	CECModelStaticData*	m_pMapModel;
	CECModel*			m_pParentModel;			//	Parent CECModel (if this is a child model), if no parent, it is NULL 
	DWORD				m_dwBindFlag;

	APtrArray<CECModel*>	m_aComboModels;		//	Combo models. combo models aren't controlled by this model. It only gives a way for high
												//	level application a chance to reference other models through this model

	typedef APtrList<A3DCombActDynData*> ActQueue;

	struct ChannelActNode
	{
		BYTE				m_Rank;
		A3DCombActDynData*	m_pActive;
		bool*				m_pActFlag;
		DWORD				m_dwFlagMode;
		ActQueue			m_QueuedActs;

		ChannelActNode();
		~ChannelActNode();
		void RemoveQueuedActs();
	};

	struct ChannelAct
	{
		APtrList<ChannelActNode*> m_RankNodes;

		~ChannelAct()
		{
			Release();
		}

		void Release()
		{
			ALISTPOSITION pos = m_RankNodes.GetHeadPosition();

			while (pos)
				delete m_RankNodes.GetNext(pos);

			m_RankNodes.RemoveAll();
		}

		ChannelActNode* GetNodeByRank(BYTE rank) const
		{
			ALISTPOSITION pos = m_RankNodes.GetHeadPosition();

			while (pos)
			{
				ChannelActNode* pNode = m_RankNodes.GetNext(pos);

				if (pNode->m_Rank == rank)
					return pNode;
			}

			return 0;
		}

		ChannelActNode* GetHighestRankNode() const
		{
			ChannelActNode* pHighest = 0;
			ALISTPOSITION pos = m_RankNodes.GetHeadPosition();

			while (pos)
			{
				ChannelActNode* pNode = m_RankNodes.GetNext(pos);

				if (!pHighest || pNode->m_Rank > pHighest->m_Rank)
					pHighest = pNode;
			}

			return pHighest;
		}

		ChannelActNode* RemoveNodeByRank(BYTE rank)
		{
			ALISTPOSITION pos = m_RankNodes.GetHeadPosition();

			while (pos)
			{
				ALISTPOSITION posCur = pos;
				ChannelActNode* pNode = m_RankNodes.GetNext(pos);

				if (pNode->m_Rank == rank)
				{
					m_RankNodes.RemoveAt(posCur);
					return pNode;
				}
			}

			return 0;
		}
	};

	CECAnimNodeMan*		m_pECAnimNodeMan;		//	The Manager's pointer for animation nodes used by this instance
	_WALKRUNBLEND_INFO	m_WalkRunBlendInfo;
	_WALKRUNUPDOWNBLEND_INFO	m_WalkRunUpDownBlendInfo;
	_RUNTURNBLEND_INFO	m_RunTurnBlendInfo;

	ON_PHYS_BREAK		m_pPhysBreakHandler;
	ON_PHYS_HITGROUND	m_pPhysHitGroundHandler;

	clientid_t			m_nId;				// id defined by user
	bool				m_bInheritParentId;
	ChannelAct			m_ChannelActs[A3DSkinModel::ACTCHA_MAX];
	AString				m_strHookName;		// parent hook name
	AString				m_strCC;			// child model CC hook name
	bool				m_bLoadSkinModel;
	float				m_fPlaySpeed;

	int					m_iAABBType;
	bool				m_bCanAffectedByParent;
	float				m_fInnerAlpha;
	float				m_fOuterAlpha;
	float				m_fAlpha;
	A3DCOLORVALUE		m_InnerColor;
	A3DCOLORVALUE		m_OuterColor;
	A3DCOLORVALUE		m_Color;
	bool				m_bAbsTrack;
	float				m_fRootBoneScaleFactor;
	float				m_fGfxScaleFactor;
	bool				m_bGfxScaleChanged;
	bool				m_bUpdateLightInfo;
	bool				m_bFirstActPlayed;
	bool				m_bGfxUseLod;
	bool				m_bCoGfxSleep;				// All Co Gfxs do not take part in tick or render
	int					m_nSfxPriority;
	int					m_nMainChannel;
	bool				m_bFashionMode;
	bool				m_bUseParentSpeed;
	bool				m_bEventUsePlaySpeed;
	abase::hash_map<int, int>	m_EquipSlotMap;
	abase::hash_map<int, int>	m_SlotEquipMap;

	ECModelMap m_ChildModels;

	HideModelHHMap		m_HideModelHHMap;

	struct ChildPhyDelayInfo
	{
		ChildPhyDelayInfo() { Reset(); }
		ChildPhyDelayInfo(int _iDelayTime, int _nPhyState)
			: iDelayTime(_iDelayTime), nPhyState(_nPhyState) {}
		void Reset() { iDelayTime = 0; nPhyState = 1; }

		int iDelayTime;
		int nPhyState;
	};
	typedef abase::hash_map<CECModel*, ChildPhyDelayInfo> ChildPhyMap;
	ChildPhyMap m_ChildPhyDelayMap;

	typedef abase::vector<A3DCombinedAction*> CombinedActLst;

	typedef APtrList<AM3DSoundBuffer*> FadeOutSfxLst;
	FadeOutSfxLst m_FadeOutSfxLst;

	CoGfxMap m_CoGfxMap;

	A3DModelPhysics*	m_pModelPhysics;
	bool				m_bKeepCurPose;
	bool				m_bPhysEnabled;		// Physical system enable flag
	bool				m_bClothSimOpen;	// Whether the cloth simulation is opened or closed...
	ClothSimDesc		m_ClothSimPara;
	
	float				m_fHitGroundActorVelThresh;			// only the actor with velocity beyond this threshold will be notified to hit ground
	abase::vector<ECMPhysXHitGroundInfo> m_arrPhysXHitGroundInfo;

	struct ChildModelLinkInfo
	{
		ChildModelLinkInfo() {}
		ChildModelLinkInfo(const char* szParentHook, const char* szChildAttacher)
			: m_strParentHook(szParentHook)
			, m_strChildAttacher(szChildAttacher)
		{

		}

		void SetHook(const char* szHook) { m_strParentHook = szHook; }

		AString m_strParentHook;
		AString m_strChildAttacher;
	};

	typedef abase::vector<ChildModelLinkInfo> ChildModelLinkInfoArray;
	ChildModelLinkInfoArray m_PhysHookAttachLinks;

	//	Delay Mode
	struct DelayModeInfo {
		DelayModeInfo() 
			: m_nDelayMode(CECModel::DM_PLAYSPEED_NORMAL)
			, m_nDelayTime(0)
			, m_nTimeElapsed(0) {}

		void OnStartDelay(int nDelayTime);	// On start delay mode
		int OnUpdate(DWORD dwTickTime);							// Return the current delay mode state
		int GetElapsedTime();
		void OnPlayComAction();

		int m_nDelayMode;
		int m_nDelayTime;
		int m_nTimeElapsed;
	};

	DelayModeInfo		m_kECMDelayInfo;		//	ECModel delay mode information
	bool				m_bIsDelayActive;		//	Specify whether ECModel uses Delay

	//	ecm level motion blur
	ECMActionBlurInfo*	m_pBlurInfo;

	//	ecm phys blend overlap
	CECModelPhysBlendOverlap* m_pOverlapBlend;

	//	Enable script event
	bool				m_bEnableScriptEvent;

	//	A3DShader
	A3DShader*			m_pPixelShader;
	DWORD				m_dwPSTickTime;

	A3DShader*			m_pReplacePS;
	GfxPSConstVec		m_vecReplacePSConsts;

	//	Event mask
	DWORD				m_EventMasks[A3DSkinModel::ACTCHA_MAX];

	DWORD				m_dwChildRenderFlag;

	TrackBoneVelocity*	m_pTrackBoneVelocity;

	PHYTORQUE_INFO		m_PhyTorqueInfo;

	A3DAABB				m_aabbUsedToCullInRender;

	LIGHT_TRANS_INFO	m_LightTransInfo;

	int					m_PhysXWindFlags;
	WindInfo			m_PhysXWindInfo;

	DWORD				m_dwInitTexFlag;

private:

	bool				m_bHideByHiddenHH;
	bool				m_bShown;
	bool				m_bNeedUpdateWhenHidden;

	bool				m_bRealShown;

protected:	//	Operations

	//	Try to open all child model links
	bool TryOpenChildModelLinks(const char* szHanger, const ChildModelLinkInfoArray& aChildModelLinks);

	bool IsGfxScaleChanged() const { return m_bGfxScaleChanged; }

	//	Create model physical sync
	bool CreateModelPhysics();
	
	//	Inner Update CoGfx's ModParam
	bool InnerUpdateCoGfxParam(GFX_INFO* pInfo, A3DSkeleton* pSkeleton);
	
	//	Check and update action's play speed
	void SetPlaySpeedByChannel(int nChannel, A3DCombinedAction* pComAct);

	//	Render CoGfx
	void RenderCoGfx();

	//	Motion blur functions should only called by CECModel'self functions
	//	Apply blur matrix
	//	Below functions are implemented in EC_ModelBlur.cpp
	void ApplyBlurMatrices(int nIndex);
	bool CalcMotionBlurParams();
	void TickMotionBlur(DWORD dwDeltaTime);
	//	Clear motion blur
	void ClearMotionBlur();

	void ApplyPixelShader();
	void RestorePixelShader();

	//	Update model aabb by skin model and all child models
	void UpdateModelAABB();

	//	Inner functions, should only be called from in-module functions
	void SetInnerColor(const A3DCOLORVALUE& cl);
	void SetInnerAlpha(float fInner);

	//	Inner Add Torque
	bool InnerAddTorque(ROT_AXIS iRotateAxis, float fTorqueMagnitude, PHYFORCE_TYPE iPhysTorqueType);
	//	Update PhyTorque
	bool TickPhyTorque(DWORD dwDeltaTime);

	//	Inner Set Show Model
	void InnerSetShow(bool bShow, bool bAffectChild/* = true*/);

	//	Refresh All Children's HH Hidden Flag
	void RefreshAllChildrenHiddenByHHFlag();

public:		//	Operations

	//	Inner help functions for gfx property transition
	void TransferEcmProperties(A3DGFXEx* pGfx);
	void SetDelayActive(bool b) { m_bIsDelayActive = b; }
	bool IsDelayActive() const { return m_bIsDelayActive; }
	void StartECModelDelayMode(int nDelayTime);
	void UpdateBaseActTimeSpan();
	void UpdateHookIndex() {}
	void UpdateCoGfxHook();
	void UpdateFadeOutSfxLst(DWORD dwDelta);

	//	This function only show or stop all current CoGfx, it does not contain any state, just a command like operation
	void ShowCoGfx(bool bShow);
	//	This function will make all CoGfx stop from Ticking or Rendering, like they are sleeping
	void SleepCoGfx(bool bSleep, bool bRecurOnChild);
	bool GetSleepCoGfx() const { return m_bCoGfxSleep; }

	void LoadChildModels();
	const AString& GetScript(int index) const { return m_pMapModel->m_Scripts[index]; }
	bool UpdateScript(int index, const char* strScript, bool bCompileOnly) { return m_pMapModel->UpdateScript(index, strScript, bCompileOnly); }
	bool IsGlobalScriptInit() const { return m_pMapModel->m_bInitGlobalScript; }
	void SetGlobalScriptInit(bool b) { m_pMapModel->m_bInitGlobalScript = b; }
	void InitGlobalScript() { m_pMapModel->InitGlobalScript(); }
	ECModelMap::iterator GetChildModelIterator() { return m_ChildModels.begin(); }

	bool IsMphyLoaded() const { return m_pMapModel->m_bMphyLoaded; }

	//	Motion blur functions should only called by CECModel'self functions
	//	This would be called from script api
	void SetMotionBlurInfo(ECMActionBlurInfo* pInfo);
	const ECMActionBlurInfo* GetMotionBlurInfo() const { return m_pBlurInfo; }
	void RemoveMotionBlurInfo();
	//	Render would be called from A3DGFXExMan
	void RenderMotionBlur(A3DViewport* pViewport);

	bool IsScriptEventEnable() const { return m_bEnableScriptEvent; }
	void SetScriptEventEnable(bool b) { m_bEnableScriptEvent = b; }

	bool IsUsingParentSpeed() const { return m_bUseParentSpeed; }
	void SetUseParentSpeed(bool b) { m_bUseParentSpeed = b; }

	bool IsEventUsePlaySpeed() const { return m_bEventUsePlaySpeed; }
	void SetEventUsePlaySpeed(bool b) { m_bEventUsePlaySpeed = b; }

	void ReloadPixelShader();

	DWORD GetChildRenderFlag() const { return m_dwChildRenderFlag; }
	void SetChildRenderFlag(DWORD dwFlag);

	//	Record track bone velocity: 记录一系列骨骼在dwWindowTimeMs内的运动速度
	//		vecBone: 设置的骨骼名列表
	//		bClearUnUsedBones: 是否清除当前在vecBone中未用到的Bone
	//		dwWindowTimeMs: 记录的窗口时间。最大值1000
	bool SetTrackBones(const abase::vector<AString>& vecBone, bool bClearUnUsedBones, DWORD dwWindowTimeMs = 300);
	//	Get current trackbone size
	size_t GetTrackBoneCount() const;

	//	Track bone vel mode
	//		mode: 取平均值，或者取最大值
	bool SetTrackBoneVelMode(TRACKBONEVEL_MODE mode);
	TRACKBONEVEL_MODE GetTrackBoneVelMode() const;

	//	Start track bone velocity
	//		dwWorkDuration: 从此刻开始更新BoneVel的时间长度(ms)，过了这个时间停止更新。-1表示一直更新。
	bool StartTrackBoneVel(DWORD dwWorkDuration);
	//	Stop track bone velocity
	bool StopTrackBoneVel(bool bClear);
	//	Is TrackBoneVel Working?
	bool IsTrackBoneVelWorking() const;

	//	Get velocity of specified bone
	const A3DBone* GetTrackBoneVelocity(A3DVECTOR3& vel, const char* szBoneName) const;

	void SetInitTexFlag(DWORD dwFlag) { m_dwInitTexFlag = dwFlag; }

	void UnbindOuterChildren();
	bool RemoveChildByPtr(CECModel* pChild, AString* pHangerOut = NULL);
	void UnbindParent();

	//joslian
	void ClearComActFlagAllRankNodes(bool bSignalCurrent) { ClearComActFlagAllRankNodes(0, bSignalCurrent); }
	void ClearComActFlagAllRankNodes(int nChannel, bool bSignalCurrent)
	{
		ChannelAct& ca = m_ChannelActs[nChannel];
		ALISTPOSITION pos = ca.m_RankNodes.GetHeadPosition();

		while (pos)
		{
			ChannelActNode* pNode = ca.m_RankNodes.GetNext(pos);

			if (pNode)
			{
				if (pNode->m_pActFlag && bSignalCurrent)
					*pNode->m_pActFlag = true;

				//ca.m_dwFlagMode = COMACT_FLAG_MODE_NONE;
				pNode->m_pActFlag = NULL;
			}
		}
	}
	//end of joslian

	bool PWPlayActionByName(const char* szActName, float fWeight, bool bRestart = true, int nTransTime = 200, bool bForceStop = true, DWORD dwUserData = 0, bool bNoFx = false
		/*joslian*/, bool* pActFlag = NULL, DWORD dwFlagMode = COMACT_FLAG_MODE_NONE);
	bool PWPlayActionByName(int nChannel, const char* szActName, float fWeight, bool bRestart = true, int nTransTime = 200, bool bForceStop = true, DWORD dwUserData = 0, bool bNoFx = false
		/*joslian*/, bool* pActFlag = NULL, DWORD dwFlagMode = COMACT_FLAG_MODE_NONE);

private:

	friend class A3DCombinedAction;
	friend class A3DCombActDynData;
	friend class A3DWalkRunDynData;
	friend class A3DWalkRunUpDownDynData;
	friend class A3DRunTurnDynData;
	friend class CECModelStaticData;
	friend class GFX_INFO;
	friend class GFX_BINDING;
	friend class MATCHG_BINDING;
	friend class MATTRANS_BINDING;
	friend class MaterialScaleChange;
	friend class MaterialScaleTrans;
	friend class ECMActionBlurInfo;
	friend void _ApplyMatChgToChild(const A3DCOLORVALUE& cl, CECModel* pECModel, bool bIsApplyComboModel);
};

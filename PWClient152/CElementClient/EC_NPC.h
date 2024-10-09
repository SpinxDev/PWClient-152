/*
 * FILE: EC_NPC.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_MsgDef.h"
#include "EC_StringTab.h"
#include "EC_Object.h"
#include "EC_GPDataType.h"
#include "EC_RoleTypes.h"
#include "EC_Counter.h"
#include "EC_CDR.h"
#include "EC_CDS.h"
#include "A3DGeometry.h"
#include "AAssist.h"
#include "CDWithCH.h"
#include "aabbcd.h"
#include "hashmap.h"

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

class A3DViewport;
class A3DSkin;
class CECViewport;
class CECModel;
class CECSkill;
class CECPateText;
class CECBubbleDecalList;
class CECPolicyAction;
class A3DGFXEx;

struct MONSTER_ESSENCE;
struct EC_NPCLOADRESULT;

struct ECMODEL_GFX_PROPERTY;
class A3DSkeletonHook;
class A3DSkinModel;
class CECNPCModelPolicy;
class CECPlayer;

using namespace CHBasedCD;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECNPC
//	
///////////////////////////////////////////////////////////////////////////
class CECNPC : public CECObject
{
	friend class CECNPCModelDefaultPolicy;
	friend class CECNPCModelClonePlayerPolicy;

public:		//	Types

	//	NPC type
	enum
	{
		NPC_UNKNOWN = -1,
		NPC_MONSTER = 0,
		NPC_SERVER,
		NPC_PET,
	};

	//	NPC action index
	enum
	{
		ACT_STAND = 0,
		ACT_IDLE,
		ACT_GUARD,
		ACT_SKILL1,
		ACT_WALK,
		ACT_ATTACK1,
		ACT_ATTACK2,
		ACT_RUN,
		ACT_DIE,
		ACT_JUMP_START,
		ACT_JUMP_LAND,
		ACT_JUMP_LOOP,
		ACT_MAGIC1,
		ACT_WOUNDED,
		ACT_NPC_CHAT1,
		ACT_NPC_CHAT2,
		ACT_NPC_CHATLOOP,
		ACT_NPC_IDLE1,
		ACT_NPC_IDLE2,
		ACT_NPC_STAND,
		ACT_NPC_WALK,
		ACT_NPC_RUN,
		ACT_NPC_ATTACK,
		ACT_NPC_DIE,
		ACT_COMMON_BORN,
		ACT_NPC_DISAPPEAR,
		ACT_WOUNDED2,
		ACT_MAX,
	};

	//	Render Name Flag
	enum
	{
		RNF_NPCNAME		= 0x01,
		RNF_MONSTERNAME	= 0x02,
		RNF_WORDS		= 0x04,
		RNF_ALL			= 0x07,
	};

	//	Work ID
	enum
	{
		WORK_STAND = 1,		//	Stand and do nothing
		WORK_FIGHT,			//	Fighting
		WORK_SPELL,			//	Monster is cast skill
		WORK_DEAD,			//	Monster is dead
		WORK_MOVE,			//	Move to a destination terrain position
		WORK_POLICYACTION,		//	Is playing policy action，WT_INTERRUPT
	};

	//	Work type
	enum
	{
		WT_NOTHING = 0,		//	Do thing
		WT_NORMAL,			//	Normal type work
		WT_INTERRUPT,		//	Interrupt type work
		NUM_WORKTYPE,
	};

	//	Bubble text
	enum
	{
		BUBBLE_DAMAGE = 0,
		BUBBLE_HITMISSED,
		BUBBLE_INVALIDHIT,
		BUBBLE_IMMUNE,
		BUBBLE_HPWARN,
		BUBBLE_LOSE,
		BUBBLE_SUCCESS,
		BUBBLE_REBOUND,		// 反弹
		BUBBLE_BEAT_BACK,	// 反击	
		BUBBLE_DODGE_DEBUFF,
	};

	//	NPC information got from server
	struct INFO
	{
		int		nid;	//	NPC id
		int		tid;	//	Template id
		int		vis_tid;//	template id for shape
	};

	//	Pate content render info
	struct PATECONTENT
	{
		int		iVisible;	//	Visible flag. 0, not set; 1, not visible; 2, visible
		int		iBaseX;		//	Base x
		int		iBaseY;		//	Base y
		int		iCurY;		//	Current y
		float	z;			//	z value
	};

	struct MULTIOBJECT_EFFECT
	{
		int iTarget;
		char cType;

		bool operator == (const MULTIOBJECT_EFFECT& src) const
		{
			return iTarget == src.iTarget && cType == src.cType;
		}
	};
	struct _mo_hash_function
	{
		unsigned long operator()(const MULTIOBJECT_EFFECT &rhs)const{ return rhs.iTarget; }
	};

	friend class CECNPCMan;

public:		//	Constructor and Destructor

	CECNPC(CECNPCMan* pNPCMan);
	virtual ~CECNPC();

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	virtual bool Init(int tid, const S2C::info_npc& Info);
	//	Release object
	virtual void Release();

	void QueueLoadNPCModel();

	//	Set loaded model to NPC object, this function is used in multithread loading process
	virtual bool SetNPCLoadedResult(EC_NPCLOADRESULT& Ret);

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport, int iRenderFlag=0) { return InternalRender(pViewport, iRenderFlag, false); }
	virtual bool RenderHighLight(CECViewport* pViewport) { return InternalRender(pViewport, RD_NORMAL, true); }
	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	//	Set absolute position
	virtual void SetPos(const A3DVECTOR3& vPos);
	//	Set absolute forward and up direction
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);

	//	Move to a destination
	virtual void MoveTo(const S2C::cmd_object_move& Cmd);
	//	Stop moving to a destination position
	virtual void StopMoveTo(const S2C::cmd_object_stop_move& Cmd);
	//	NPC was killed
	virtual void Killed(bool bDelay);
	virtual void Disappear();
	//	Stop attack
	virtual void StopAttack();
	//	NPC is damaged
	virtual void Damaged(int iDamage, DWORD dwModifier=0);
	//	Play an attack effect
	void PlayAttackEffect(int idTarget, int idSkill, int skillLevel, int nDamage, DWORD dwModifier, int nAttackSpeed, int nSection=0);
	//	The function that will call into CECObject::TurnFaceTo()
	void NPCTurnFaceTo(int idTarget, DWORD dwTime=200);

	//	Get NPC name color
	virtual DWORD GetNameColor() { return 0xffffff00; }
	//	Is monster in invader camp in battle ?
	virtual bool IsInBattleInvaderCamp() { return false; }
	//	Is monster in defender camp in battle ?
	virtual bool IsInBattleDefenderCamp() { return false; }
	//	Get role in battle
	virtual int GetRoleInBattle() { return 0; }

	//	Get NPC name
	const ACHAR* GetName()const{ return m_strName; }
	const ACHAR* GetNameToShow()const;
	//	Get NPC information got from server
	const INFO& GetNPCInfo()const{ return m_NPCInfo; }
	//	Get basic properties
	ROLEBASICPROP& GetBasicProps() { return m_BasicProps; }
	//	Get extend properties
	ROLEEXTPROP& GetExtendProps() { return m_ExtProps; }
	//	Get selected target
	int GetSelectedTarget() { return m_idSelTarget; }
	//	Set selected target
	void SetSelectedTarget(int id) { m_idSelTarget = id; }
	//	Get NPC ID
	int GetNPCID() { return m_NPCInfo.nid; }
	//	Get NPC template ID
	int GetTemplateID() { return m_NPCInfo.tid; }
	//	Get NPC's aabb used to pick
	const A3DAABB& GetPickAABB();
	//	Get NPC state
	bool IsDead()const{ return (m_dwStates & GP_STATE_CORPSE) ? true : false; }
	bool IsFlying()const{ return (m_dwStates & GP_STATE_NPC_FLY) ? true : false; }
	bool IsSwimming()const{ return (m_dwStates & GP_STATE_NPC_SWIM) ? true : false; }
	bool IsSitting()const{ return (m_dwStates & GP_STATE_SITDOWN) ? true : false; }
	bool IsMoving()const;
	bool IsDirFixed()const{ return (m_dwStates & GP_STATE_NPC_FIXDIR) ? true : false; }
	//	Get NPC's real position on server
	const A3DVECTOR3& GetServerPos()const{ return m_vServerPos; }
	//	Whether this NPC should be removed
	bool ShouldDisappear() { return m_DisappearCnt.IsFull(); }
	//	Is NPC disappearing ?
	bool IsDisappearing() { return m_DisappearCnt.GetCounter() ? true : false; }
	//	Set NPC's transparency
	bool StartAdjustTransparency(float fCur, float fDest, DWORD dwTime);
	//	Update current transparency
	float UpdateTransparency(DWORD dwDeltaTime);
	//	Set player's transparency
	virtual float GetTransparentLimit();

	//	Get master id
	int GetMasterID() const { return m_idMaster; }
	//	Set last said words
	void SetLastSaidWords(const ACHAR* szWords, int nTimeShow = -1);
	
	bool IsAboutToDie() const { return m_bAboutToDie; }
	void SetAboutToDie(bool bFlag) { m_bAboutToDie = bFlag; }
	//	Set / Get name rendering flag
	void SetRenderNameFlag(bool bTrue) { m_bRenderName = bTrue; }
	bool GetRenderNameFlag()const{ return m_bRenderName && !ShouldHideName(); }
	//	Set / Get bars rendering flag
	void SetRenderBarFlag(bool bTrue) { m_bRenderBar = bTrue; }
	bool GetRenderBarFlag() { return m_bRenderBar; }

	//	Check extend state (bit index)
	bool GetExtState(int n);

	//	Get icon state array
	const S2C::IconStates& GetIconStates() const { return m_aIconStates; }

	//	Get inherent random properties
	int GetRandomProp() { return m_iRandomProp; }

	//	Get touch radius
	float GetTouchRadius() { return m_fTouchRad; }
	//	Is server NPC ?
	bool IsServerNPC() { return OCID_SERVER == m_iCID; }
	//	Is monster NPC ?
	bool IsMonsterNPC() { return OCID_MONSTER == m_iCID; }
	//	Is pet NPC ?
	bool IsPetNPC() { return OCID_PET == m_iCID; }	
	//	Is monster or pet
	bool IsMonsterOrPet() { return IsMonsterNPC() || IsPetNPC(); }
	//	Get distance to host player
	float GetDistToHost() { return m_fDistToHost; }
	float GetDistToHostH() { return m_fDistToHostH; }
	//	Get distance to camera
	float GetDistToCamera() { return m_fDistToCamera; }

	//	Print bubble text
	void BubbleText(int iIndex, DWORD dwNum, int p1=0);

	//	宠物有话说
	void OnPetSays(int type);

	bool IsInPolicyAction()const;

	bool GetCHAABB(A3DAABB &aabb)const;

	//	Initialize static resources
	static bool InitStaticRes();
	//	Release static resources
	static void ReleaseStaticRes();
	/*	Distinguish a NPC id. Return values:
		0 - if it's monster 
		1 - if it's server
		2 - if it's a pure pet
		-1 if it's not a NPC id
	*/
	static int DistinguishID(int tid);
	//	Load NPC model
	static bool LoadNPCModel(int tid, const char* szFile, EC_NPCLOADRESULT& Ret);
	//	Release NPC model
	static void ReleaseNPCModel(EC_NPCLOADRESULT& Ret);

	//  multi object effect
	void AddMultiObjectEffect(int idTarget,char cType);
	void RemoveMultiObjectEffect(int idTarget,char cType);
	void UpdateMultiObjectEffect(DWORD dwDeltaTime);
	void UpdateOneMultiObjectEffect(int idTarget,A3DGFXEx* pGfx,DWORD dwDeltaTime);
	void RenderMultiObjectGfx();

	void OptimizeShowExtendStates();
	void OptimizeWeaponStoneGfx();
	void OptimizeArmorStoneGfx();
	void OptimizeSuiteGfx();

	int GetOwnerFaction()const{ return m_idOwnerFaction; }
	virtual const MONSTER_ESSENCE * GetMonsterEssence()const{ return NULL; }
	bool IsFactionPVPMineCar()const;
	bool IsFactionPVPMineBase()const;

	void TransformShape(int vis_tid);
	bool IsClickNamePateText(int x, int y);
	void SetMouseOnNameFlag(bool IsHover);

	bool HasModel()const;
	void ClearComActFlag(bool bSignalCurrent);
	bool GetEcmProperty(ECMODEL_GFX_PROPERTY* pProperty)const;
	A3DSkinModel * GetSgcSkinModel(const char *szHanger, bool bChildHook, const char *szHook);
	A3DSkeletonHook * GetSgcHook(const char *szHanger, bool bChildHook, const char *szHook);

protected:
	CECNPCModelPolicy	*	m_pNPCModelPolicy;

private:
	bool ShouldPlayNewActionFor(int iMoveMode);
	void BeforeChangeNPCModel();
	bool TestAndLoadMasterModel();
	void AfterChangeNPCModel();

protected:	//	Attributes

	static CECStringTab		m_ActionNames;	//	Action name table

	CECNPCMan*		m_pNPCMan;			//	NPC manager
	INFO			m_NPCInfo;			//	NPC information got from server
	ROLEBASICPROP	m_BasicProps;		//	Basic properties
	ROLEEXTPROP		m_ExtProps;			//	Extend properties
	int				m_idSelTarget;		//	选中目标的ID
	float			m_fTouchRad;		//	Touch radius
	bool			m_bCastShadow;		//	flag indicates whether it will cast shadow
	int				m_iMMIndex;			//	Index in CECNPCMan::m_aMMNPCs array
	DWORD			m_dwStates;			//	NPC's basic states
	DWORD			m_dwStates2;		//  NPC's basic states 2
	DWORD			m_aExtStates[OBJECT_EXT_STATE_COUNT];	//	Visible extend states from server
	DWORD			m_aExtStatesShown[OBJECT_EXT_STATE_COUNT];	//	Visible extend states currently shown
	CECCounter		m_DisappearCnt;		//	Disappear time counter
	int				m_iRandomProp;		//	NPC's inherent random properties
	DWORD			m_dwISTimeCnt;		//	Icon state time counter
	int				m_idMaster;			//	id of master if the NPC is a pet
	ACString		m_strName;			//	Name of NPC

	float			m_fCurTrans;		//	Current transparence
	float			m_fDstTrans;		//	Target transparence
	float			m_fTransDelta;		//	Transparence delta
	CECCounter		m_TransCnt;			//	Transparence time counter

	bool			m_bAboutToDie;		//	a object_attack_result with deadly strike flag has been received
	CECCounter		m_IdleCnt;			//	Idle time counter
	
	float			m_fDistToHost;		//	Distance to host player
	float			m_fDistToHostH;		//	Horizontal distance to host player
	float			m_fDistToCamera;	//	Distance to camera

	A3DVECTOR3		m_vMoveDir;			//	Move direction
	A3DVECTOR3		m_vServerPos;		//	NPC position on server
	A3DVECTOR3		m_vStopDir;			//	The direction after moving stopped the NPC will face to
	float			m_fMoveSpeed;		//	Move speed
	int				m_iMoveEnv;			//	Move environment
	bool			m_bStopMove;		//	Stop move flag
	int				m_iPassiveMove;		//	Passive move flag (Push back / Pull)

	int				m_aWorks[NUM_WORKTYPE];		//	Work array
	int				m_iCurWork;					//	Current work's id
	int				m_iCurWorkType;				//	Current work type

	int				m_iAction;					//	Current action being played

	CECSkill*		m_pCurSkill;		//	The skill NPC is casting
	CECCounter		m_SkillCnt;			//	Time counter for casting skill
	int				m_nFightTimeLeft;	//	Work Fight time left
	int				m_idAttackTarget;	//	Attack target
	int				m_idCurSkillTarget;	//	Spell target
	bool			m_bStartFight;		//	Start fight flag

	bool			m_bRenderName;		//	Render name flag
	CECPateText*	m_pPateName;		//	NPC name
	bool			m_bRenderBar;		//	Render HP, MP, EXP bars
	PATECONTENT		m_PateContent;		//	Pate content
	CECPateText*	m_pPateLastWords1;	//	The words player said last time, line 1
	CECPateText*	m_pPateLastWords2;	//	The words player said last time, line 2
	int				m_iLastSayCnt;		//	Time counter of last said words

	CECBubbleDecalList*		m_pBubbleTexts;	//	Bubble text list

	S2C::IconStates		m_aIconStates;	//	Icon states (un-visible extend states)
	OtherPlayer_Move_Info	m_cdr;

	CECPolicyAction *	m_pPolicyAction;	//	策略动作
	DWORD	m_nPolicyActionIntervalTimer;	//	策略动作播放间隔定时器（当策略动作在NPC模型上存在时起作用）

	typedef abase::hash_map<MULTIOBJECT_EFFECT, A3DGFXEx*, _mo_hash_function> MOEffectMAP;
	MOEffectMAP m_mapMOEffect;			//   Multi Object Effect

	int				m_idOwnerFaction;	//	NPC 归属的帮派（帮派PVP中矿车NPC等使用）

protected:	//	Operations
	int GetMoveAction(int iMoveMode);
	//	Play model action
	void PlayModelAction(int iAction, bool bRestart=true);
	//	Play attack action
	bool PlayAttackAction(int nAttackSpeed, bool *pActFlag);
	// Play skill cast action
	void PlaySkillCastAction(int idSkill);
	// Play skill attack action
	bool PlaySkillAttackAction(int idSkill, int nAttackSpeed,int nSection, bool *pActFlag);
	//	Play move action
	void PlayMoveAction(int iMoveMode);
	//	Render titles / names / talks above player's header
	bool RenderName(CECViewport* pViewport, DWORD dwFlags);
	//	Render bar above player's header
	bool RenderBars(CECViewport* pViewport);
	//	Set new visible extend states
	void SetNewExtendStates(int start, DWORD* pData, int count);
	void ClearShowExtendStates();
	void ShowExtendStates(int start, DWORD* pData, int count, bool bIgnoreOptimize=false);
	//	Fill pate content
	bool FillPateContent(CECViewport* pViewport);
	//	Internal render routine
	bool InternalRender(CECViewport* pViewport, int iRenderFlag, bool bHighLight);

	//	Move to destination position
	bool MovingTo(DWORD dwDeltaTime);
	//	Move step
	A3DVECTOR3 MoveStep(const A3DVECTOR3& vDir, float fSpeed, float fTime);

	//	Start a work of specified type
	void StartWork(int iWorkType, int iNewWork, DWORD dwParam=0);
	//	Current work finished
	void WorkFinished(int iWorkID);
	//	Stop / pause work of specified type
	void StopWork(int iWorkType);
	//	Release work resources of specified type
	void ReleaseWork(int iWorkType);
	//  check whether we need to force pull the actor
	bool IsLag(float fDist);

	//	Start works 
	void StartWorkByID(int iWorkID, DWORD dwParam);
	virtual void StartWork_Stand(DWORD dwParam);
	virtual void StartWork_Fight(DWORD dwParam);
	virtual void StartWork_Spell(DWORD dwParam);
	virtual void StartWork_Dead(DWORD dwParam);
	virtual void StartWork_Move(DWORD dwParam);
	virtual void StartWork_PolicyAction(DWORD dwParam);

	//	Work tick routine
	virtual void TickWork_Stand(DWORD dwDeltaTime);
	virtual void TickWork_Fight(DWORD dwDeltaTime);
	virtual void TickWork_Spell(DWORD dwDeltaTime);
	virtual void TickWork_Dead(DWORD dwDeltaTime);
	virtual void TickWork_Move(DWORD dwDeltaTime);
	virtual	void TickWork_PolicyAction(DWORD dwDeltaTime);

	//	Message handlers
	virtual void OnMsgNPCAtkResult(const ECMSG& Msg);
	virtual void OnMsgNPCSkillResult(const ECMSG& Msg);
	virtual void OnMsgNPCExtState(const ECMSG& Msg);
	virtual void OnMsgNPCCastSkill(const ECMSG& Msg);
	virtual void OnMsgNPCEnchantResult(const ECMSG& Msg);
	virtual void OnMsgNPCRoot(const ECMSG& Msg);
	virtual void OnMsgNPCLevel(const ECMSG& Msg);
	virtual void OnMsgNPCInvisible(const ECMSG& Msg);
	virtual void OnMsgNPCStartPlayAction(const ECMSG &Msg);
	virtual void OnMsgNPCStopPlayAction(const ECMSG &Msg);
	virtual void OnMsgNPCMultiObjectEffect(const ECMSG &Msg);

	static bool IsAttackAction(int iAct);

	void CheckStartPolicyAction();
	void CheckStopPolicyAction();
	bool PlayPolicyActionAgain(bool bFirst);
	bool CanPlayPolicyAction()const;
	void PlayPolicyIntervalAction();

	//  Is the immune bubble text is disable ?
	virtual bool IsImmuneDisable() { return false; }
	virtual bool ShouldHideName()const{ return false; }

	static bool GetModelFile(int tid, const char*& szModelFile);
	bool GetVisibleModel(int &tid, const char*& szModelFile)const;
	bool ShouldUseMasterModel()const;
	bool IsUsingMasterModel()const;
	CECPlayer * GetMaster()const;

public:
	CECPlayer * GetClonedMaster();

	void OnMsgAttackHostResult(int idHost, int nDamage, int nFlag, int nSpeed);
	bool RayTrace(ECRAYTRACE* pTraceInfo);
	bool TraceWithBrush(BrushTraceInfo * pInfo);
	void RebuildTraceBrush();
	void ReleaseTraceBrush();

	static const char* GetBaseActionName(int iAct);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

bool NPCRenderForShadow(A3DViewport * pViewport, void * pArg);

DWORD MakeNPCRenderCameraParam(int nRotateAngle, int nMove);
void  GetNPCRenderCameraParam(DWORD dwCameraParam, float &vDeg, float &fMoveRatio);
void NPCRenderDemonstration(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3);
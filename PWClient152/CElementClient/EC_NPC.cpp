/*
 * FILE: EC_NPC.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_NPC.h"
#include "EC_Pet.h"
#include "EC_Model.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_Viewport.h"
#include "EC_GPDataType.h"
#include "EC_ManPlayer.h"
#include "EC_ManNPC.h"
#include "EC_ShadowRender.h"
#include "EC_Player.h"
#include "EC_Utility.h"
#include "EC_Decal.h"
#include "EC_ManDecal.h"
#include "EC_Resource.h"
#include "EC_FixedMsg.h"
#include "EC_HostPlayer.h"
#include "EC_ManAttacks.h"
#include "EC_Skill.h"
#include "EC_Monster.h"
#include "EC_PateText.h"
#include "EC_ImageRes.h"
#include "EC_Configs.h"
#include "EC_SceneLoader.h"
#include "CDWithCH.h"
#include "EC_UIManager.h"
#include "EC_PetWords.h"
#include "EC_PortraitRender.h"
#include "EC_NPCServer.h"
#include "EC_PolicyAction.h"
#include "EC_Optimize.h"
#include "EC_PetCorral.h"
#include "EC_AutoPolicy.h"
#include "EC_NPCModel.h"

#include "A3DCombinedAction.h"

#include <A3DCamera.h>
#include <A3DFont.h>
#include <A3DViewport.h>
#include <A3DFlatCollector.h>
#include <A3DSkinMan.h>
#include <A3DTerrainWater.h>
#include <A3DCollision.h>
#include <AFI.h>
#include <AUIManager.h>
#include <A3DViewport.h>
#include <A3DLight.h>
#include <A3DSkinRender.h>
#include <A3DGFXFuncs.h>
#include <A3DConfig.h>

#include "elementdataman.h"

using namespace CHBasedCD;

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define MAX_LAGDIST		25.0f	//	Maximum lag distance

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static bool ValidateWork(int iWork){
	return iWork >= CECNPC::WORK_STAND && iWork <= CECNPC::WORK_POLICYACTION;
}

static const char * GetWorkName(int iWork){
	const char *l_szWorkNames[] = {
	"Invalid Work",
	"WORK_STAND",
	"WORK_FIGHT",
	"WORK_SPELL",
	"WORK_DEAD",
	"WORK_MOVE",
	"WORK_POLICYACTION",
	};
	if (iWork >= 0 && iWork <= sizeof(l_szWorkNames)/sizeof(l_szWorkNames[0])){
		return l_szWorkNames[iWork];
	}
	return "Invalid Work";
}

//	NPC's render for shadow call back function
bool NPCRenderForShadow(A3DViewport * pViewport, void * pArg)
{
	A3DSkinModel * pModel = (A3DSkinModel *) pArg;

	if( !pModel )
		return false;

	g_pGame->GetA3DDevice()->SetZTestEnable(true);
	g_pGame->GetA3DDevice()->SetZWriteEnable(true);
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(true);
	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(false);
	g_pGame->GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATEREQUAL);
	g_pGame->GetA3DDevice()->SetAlphaRef(84);

	pModel->RenderAtOnce(pViewport, A3DSkinModel::RAO_NOMATERIAL, false);

	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(true);
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(false);
	g_pGame->GetA3DDevice()->SetZTestEnable(true);
	g_pGame->GetA3DDevice()->SetZWriteEnable(true);
	return true;
}

//	Distinguish a NPC id. Return CECNPC::NPC_MONSTER if it's monster NPC id. 
//	CECNPC::NPC_SERVER if it's server NPC id. NPC_UNKNOWN if it's not a NPC id
int CECNPC::DistinguishID(int tid)
{
	DATA_TYPE DataType = g_pGame->GetElementDataMan()->get_data_type(tid, ID_SPACE_ESSENCE);
	int iNPCType = -1;

	switch (DataType)
	{
	case DT_NPC_ESSENCE:		iNPCType = NPC_SERVER;		break;
	case DT_MONSTER_ESSENCE:	iNPCType = NPC_MONSTER; 	break;
	case DT_PET_ESSENCE:		iNPCType = NPC_PET;			break;
	default:
		ASSERT(0);
		break;
	}

	return iNPCType;
}

//	Load NPC model
bool CECNPC::LoadNPCModel(int tid, const char* szFile, EC_NPCLOADRESULT& Ret)
{
	Ret.pModel		= NULL;
	Ret.pSkin		= NULL;
	Ret.pLowSkin	= NULL;

	if (!(Ret.pModel = new CECModel))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECNPC::LoadNPCModel", __LINE__);
		return false;
	}

	char szModelFile[MAX_PATH];
	strcpy(szModelFile, szFile);

	int iNPCType = DistinguishID(tid);
	if (iNPCType == NPC_MONSTER || iNPCType == NPC_PET)
	{
	//	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("armor.sdr", A3DSkinMan::SHADERREPLACE_REFLECTPREFIX);
		g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile((const char*)glb_ArmorReplaceShader_ReflectPrefix, A3DSkinMan::SHADERREPLACE_USERDEFINE);
		if (!Ret.pModel->Load(szModelFile, true, A3DSkinModel::LSF_NOSKIN, true))
		{
			//	Now try to load error.ecm
			strcpy(szModelFile, "models\\error\\error.ecm");
			if (!Ret.pModel->Load(szModelFile, true, A3DSkinModel::LSF_NOSKIN, true))
			{
				g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);
				a_LogOutput(1, "CECNPC::LoadNPCModel, Failed to load NPC model %s", szFile);
				return false;
			}
		}

		Ret.pModel->BuildWoundActionChannel();

		char szSkin[MAX_PATH];
		strncpy(szSkin, szModelFile, MAX_PATH);
		glb_ChangeExtension(szSkin, "ski");

		if (!(Ret.pSkin = g_pGame->LoadA3DSkin(szSkin, true)))
		{
			a_LogOutput(1, "CECNPC::Init(), Failed to load skin %s", szSkin);
			Ret.pSkin = g_pGame->LoadA3DSkin("Models\\error\\error.ski", true);
		}

		glb_ClearExtension(szSkin);
		strcat(szSkin, "二级.ski");

		if( af_IsFileExist(szSkin) )
		{
			if (!(Ret.pLowSkin = g_pGame->LoadA3DSkin(szSkin, true)))
			{
				a_LogOutput(1, "CECNPC::Init(), Falied to load skin %s", szSkin);
				Ret.pLowSkin = g_pGame->LoadA3DSkin("Models\\error\\error.ski", true);
			}
		}

		if (Ret.pModel->GetA3DSkinModel())
		{
			Ret.pModel->GetA3DSkinModel()->AddSkin(NULL, false);
			Ret.pModel->SetAABBType(CECModel::AABB_AUTOSEL);
		}
		g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);
	}
	else
	{
	//	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("armor.sdr", A3DSkinMan::SHADERREPLACE_REFLECTPREFIX);
		g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile((const char*)glb_ArmorReplaceShader_ReflectPrefix, A3DSkinMan::SHADERREPLACE_USERDEFINE);
		if (!Ret.pModel->Load(szFile, true, A3DSkinModel::LSF_NOSKIN, true))
		{
			//	Now try to load error.ecm
			strcpy(szModelFile, "models\\error\\error.ecm");
			if (!Ret.pModel->Load(szModelFile))
			{
				g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);
				a_LogOutput(1, "CECNPC::LoadNPCModel, Failed to load NPC model %s", szFile);
				return false;
			}
		}

		Ret.pModel->BuildWoundActionChannel();

		char szSkin[MAX_PATH];
		strncpy(szSkin, szModelFile, MAX_PATH);
		glb_ChangeExtension(szSkin, "ski");

		if (!(Ret.pSkin = g_pGame->LoadA3DSkin(szSkin, true)))
		{
			a_LogOutput(1, "CECNPC::Init(), Failed to load skin %s", szSkin);
			Ret.pSkin = g_pGame->LoadA3DSkin("Models\\error\\error.ski", true);
		}

		if (Ret.pModel->GetA3DSkinModel())
		{
			Ret.pModel->GetA3DSkinModel()->AddSkin(NULL, false);
			Ret.pModel->SetAABBType(CECModel::AABB_AUTOSEL);
		}
		g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);
	}

	return true;
}

//	Release NPC model
void CECNPC::ReleaseNPCModel(EC_NPCLOADRESULT& Ret)
{
	if (Ret.pModel)
	{
		QueueECModelForRelease(Ret.pModel);
		Ret.pModel = NULL;
	}

	if (Ret.pSkin)
	{
		g_pGame->ReleaseA3DSkin(Ret.pSkin, true);
		Ret.pSkin = NULL;
	}

	if (Ret.pLowSkin)
	{
		g_pGame->ReleaseA3DSkin(Ret.pLowSkin, true);
		Ret.pLowSkin = NULL;
	}
}

//	Initialize static resources
bool CECNPC::InitStaticRes()
{
	//	Load action names from file
	if (!m_ActionNames.IsInitialized())
		m_ActionNames.Init("Configs\\actions_npc.txt", false);

	return true;
}

//	Release static resources
void CECNPC::ReleaseStaticRes()
{
	m_ActionNames.Release();
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECNPC
//	
///////////////////////////////////////////////////////////////////////////

CECStringTab CECNPC::m_ActionNames;	//	Action name table

CECNPC::CECNPC(CECNPCMan* pNPCMan) : 
m_vServerPos(0.0f),
m_strName(_AL("NPC"))
{
	m_iCID				= OCID_NPC;
	m_pNPCMan			= pNPCMan;
	m_pNPCModelPolicy	= new CECNPCModelDefaultPolicy(this);
	m_bCastShadow		= false;
	m_iMoveEnv			= MOVEENV_GROUND;
	m_fMoveSpeed		= 1.0f;
	m_bStopMove			= false;
	m_fTouchRad			= 1.0f;
	m_bVisible			= true;
	m_fDistToHost		= 0.0f;
	m_fDistToCamera		= 0.0f;
	m_fDistToHostH		= 0.0f;
	m_iMMIndex			= -1;
	m_dwStates			= 0;
	m_dwStates2			= 0;
	m_iPassiveMove		= 0;
	m_bAboutToDie		= false;
	m_bRenderName		= false;
	m_nFightTimeLeft	= 0;
	m_pCurSkill			= NULL;
	memset(m_aExtStates, 0, sizeof(m_aExtStates));
	memset(m_aExtStatesShown, 0, sizeof(m_aExtStatesShown));
	m_iRandomProp		= 0;
	m_dwISTimeCnt		= 0;
	m_bRenderBar		= false;
	m_fCurTrans			= 0.0f;
	m_fDstTrans			= 0.0f;
	m_idMaster			= 0;
	m_bStartFight		= false;

	m_iCurWork			= 0;
	m_iCurWorkType		= -1;
	m_idAttackTarget	= 0;
	m_iAction			= -1;

	m_PateContent.iVisible = 0;

	if ((m_pPateName = new CECPateText))
		m_pPateName->EnableBorder(true);

	m_pPateLastWords1	= NULL;
	m_pPateLastWords2	= NULL;
	m_iLastSayCnt		= 0;
	m_pBubbleTexts		= new CECBubbleDecalList;

	m_DisappearCnt.SetPeriod(5000);
	m_IdleCnt.SetPeriod(25000);
	m_IdleCnt.IncCounter(rand() % 6000);

	memset(m_aWorks, 0, sizeof (m_aWorks));
	memset(&m_NPCInfo, 0, sizeof (m_NPCInfo));
	memset(&m_BasicProps, 0, sizeof (m_BasicProps));
	memset(&m_ExtProps, 0, sizeof (m_ExtProps));

	m_pPolicyAction =  NULL;
	m_nPolicyActionIntervalTimer = 0;
	m_idOwnerFaction = 0;

	m_idSelTarget = 0;
}

CECNPC::~CECNPC()
{
}

//	Initlaize object
bool CECNPC::Init(int tid, const S2C::info_npc& Info)
{
	using namespace S2C;

	m_NPCInfo.nid	= Info.nid;
	m_NPCInfo.tid	= tid;
	m_NPCInfo.vis_tid = Info.vis_tid;
	m_dwStates		= Info.state;
	m_dwStates2		= Info.state2;
	m_vServerPos	= Info.pos;
	m_iRandomProp	= (Info.state & 0x0f00) >> 8;

	if (Info.state & GP_STATE_NPC_FLY){
		m_iMoveEnv = MOVEENV_AIR;
	}else if (Info.state & GP_STATE_NPC_SWIM){
		m_iMoveEnv = MOVEENV_WATER;
	}else{
		m_iMoveEnv = MOVEENV_GROUND;
	}

	//	Parse following data
	BYTE* pData = (BYTE*) &Info + sizeof (info_npc);

	//	Fill extend states	
	DWORD aExtStates[OBJECT_EXT_STATE_COUNT] = {0};
	if (Info.state & GP_STATE_EXTEND_PROPERTY)
	{
		memcpy(aExtStates, pData, sizeof(aExtStates));
		pData += sizeof(aExtStates);
	}
	SetNewExtendStates(0, aExtStates, OBJECT_EXT_STATE_COUNT);

	//	Pet
	m_idMaster = 0;
	if (Info.state & GP_STATE_NPC_PET)
	{
		m_idMaster = *(int*)pData;
		pData += sizeof (int);
	}

	//	Read name
	if (Info.state & GP_STATE_NPC_NAME)
	{
		BYTE len = *pData;
		pData += sizeof (BYTE);

		if (len)
		{
			m_strName = ACString((ACHAR*)pData, len / sizeof (ACHAR));
			pData += len;
		}
	}

	SetSelectable( !(Info.state & GP_STATE_FORBIDBESELECTED) );

	if (Info.state & GP_STATE_MULTIOBJ_EFFECT)
	{
		int iCount = *(int*) pData;
		pData += sizeof(int);
		for (int i = 0;i < iCount;++i)
		{
			int idTarget = *(int*) pData;
			pData += sizeof(int);
			char cType = *(char*) pData;
			pData += sizeof(char);
			AddMultiObjectEffect(idTarget,cType);
		}
	}
	m_idOwnerFaction = 0;
	if (Info.state & GP_STATE_NPC_MAFIA){
		m_idOwnerFaction = *(int*)pData;
		pData += sizeof (int);
	}

	if (m_pPateName)
		m_pPateName->SetText(m_strName, false);

	m_cdr.fStepHeight = 0.4f;
	m_cdr.vVelocity.Clear();
	SetUseGroundNormal(false);
	
	//	Calculate distance to host player. This is necessary because
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost && pHost->IsSkeletonReady())
	{
		m_fDistToHost  = Magnitude(m_vServerPos - pHost->GetPos()); //CalcDist(pHost->GetPos(), true);
		m_fDistToHostH = MagnitudeH(m_vServerPos - pHost->GetPos()); //CalcDist(pHost->GetPos(), false);
	}

	return true;
}

//	Release object
void CECNPC::Release()
{
	//	Release current skill if it exists
	if (m_pCurSkill)
	{
		delete m_pCurSkill;
		m_pCurSkill = NULL;
	}

	//	Clear extend states before model is released
	ClearShowExtendStates();
	::memset(m_aExtStates, 0, sizeof(m_aExtStates));
	m_aIconStates.clear();

	delete m_pNPCModelPolicy;
	m_pNPCModelPolicy = NULL;

	if (m_pPateName)
	{
		delete m_pPateName;
		m_pPateName = NULL;
	}

	if (m_pPateLastWords1)
	{
		delete m_pPateLastWords1;
		m_pPateLastWords1 = NULL;
	}

	if (m_pPateLastWords2)
	{
		delete m_pPateLastWords2;
		m_pPateLastWords2 = NULL;
	}
	
	if (m_pBubbleTexts)
	{
		delete m_pBubbleTexts;
		m_pBubbleTexts = NULL;
	}

	delete m_pPolicyAction;
	m_pPolicyAction = NULL;
	m_nPolicyActionIntervalTimer = 0;

	for (MOEffectMAP::iterator it = m_mapMOEffect.begin();it != m_mapMOEffect.end();++it)
	{
		A3DGFXExMan *pGFXExMan = g_pGame->GetA3DGFXExMan();
		pGFXExMan->CacheReleasedGfx(it->second);
	}

	m_mapMOEffect.clear();
}

void CECNPC::QueueLoadNPCModel(){
	if (ShouldUseMasterModel()){
		if (GetMaster()){
			return;	//	能获取角色模型时，到下个 Tick 加载模型
		}			//	无法获取角色时、暂时使用 NPC 模型
	}
	int tid = 0;
	const char* szModelFile = "";
	if (!GetVisibleModel(tid, szModelFile)){
		return;
	}
	QueueECModelForLoad(MTL_ECM_NPC, GetNPCInfo().nid, GetBornStamp(), GetServerPos(), szModelFile, tid);
}

bool CECNPC::TestAndLoadMasterModel(){
	if (!ShouldUseMasterModel()){
		return false;
	}
	if (IsUsingMasterModel()){
		return true;
	}
	CECPlayer *pPlayer = GetMaster();
	if (!pPlayer){
		return false;
	}
	CECNPCModelClonePlayerPolicy *pNPCModelPolicy = new CECNPCModelClonePlayerPolicy(this);
	if (!pNPCModelPolicy->SetPlayerToClone(pPlayer)){
		delete pNPCModelPolicy;
		return false;
	}
	if (m_pNPCModelPolicy != NULL){
		if (m_pNPCModelPolicy->IsModelLoaded()){
			LOG_DEBUG_INFO(AString().Format("CECNPC::TestAndLoadMasterModel, old model found"));
		}
		pNPCModelPolicy->InheritFromPreviousPolicy(m_pNPCModelPolicy);
	}
	BeforeChangeNPCModel();
	delete m_pNPCModelPolicy;
	m_pNPCModelPolicy = pNPCModelPolicy;
	LOG_DEBUG_INFO(AString().Format("CECNPC::TestAndLoadMasterModel, master model loaded"));
	AfterChangeNPCModel();
	return true;
}

void CECNPC::BeforeChangeNPCModel(){
	ClearShowExtendStates();
}

//	Set loaded model to NPC object, this function is used in multithread
//	loading process
bool CECNPC::SetNPCLoadedResult(EC_NPCLOADRESULT& Ret)
{
	if (IsUsingMasterModel()){
		ReleaseNPCModel(Ret);			//	宠物复制角色模型时、可能由于角色临时不存在而导致加载了NPC原有模型，
		return false;					//	但有可能随时发现角色模型而加载了角色模型，此时原加载结果不再使用
	}
	BeforeChangeNPCModel();
	CECNPCModelDefaultPolicy *policy = dynamic_cast<CECNPCModelDefaultPolicy *>(m_pNPCModelPolicy);
	policy->SetNPCLoadedResult(Ret);
	AfterChangeNPCModel();
	return true;
}

void CECNPC::AfterChangeNPCModel(){
	m_pNPCModelPolicy->SetPos(GetPos());
	m_pNPCModelPolicy->SetDirAndUp(GetDir(), GetUp());

	ShowExtendStates(0, m_aExtStates, OBJECT_EXT_STATE_COUNT);

	if (IsDead())
	{
		if (!(m_dwStates & GP_STATE_NPC_DELAYDEAD))
			Disappear();
	
		StartWork(WT_NORMAL, WORK_DEAD);
	}
	else if (m_iAction != -1)
	{
		//	make it stand or play a correct action	
		PlayModelAction(m_iAction);
	}
	else
	{
		if (IsMonsterOrPet())
			PlayModelAction(ACT_STAND);
		else
			PlayModelAction(ACT_NPC_STAND);
	}

	// if it has a special born action, just play it. if not, this call will not stop current action
	if( m_bBornInSight )
	{
		if (m_pNPCModelPolicy->HasAction(ACT_COMMON_BORN)){
			PlayModelAction(ACT_COMMON_BORN);
		}
	}

	RebuildTraceBrush();
	
	//	Reset aabb extents
	m_cdr.vExts	= GetPickAABB().Extents;

	CheckStartPolicyAction();
}

//	Get NPC's aabb used to pick
const A3DAABB& CECNPC::GetPickAABB()
{
	return m_pNPCModelPolicy->GetPickAABB();
}

//	Set last said words
void CECNPC::SetLastSaidWords(const ACHAR* szWords, int nTimeShow /*= -1*/)
{
	if (!m_pPateLastWords1)
		m_pPateLastWords1 = new CECPateText;

	if (!m_pPateLastWords2)
		m_pPateLastWords2 = new CECPateText;

	if (!m_pPateLastWords1 || !m_pPateLastWords2 || !szWords)
		return;

	ACString strWords;
	int iLen1 = m_pPateLastWords1->SetText(szWords, true, false, NULL, &strWords);

	if (iLen1 < strWords.GetLength())
		m_pPateLastWords2->SetText(&strWords[iLen1], true, true);
	else
		m_pPateLastWords2->Clear();

	if (nTimeShow > 0)
		m_iLastSayCnt = nTimeShow;
	else
		m_iLastSayCnt = 20000;
}

//	Tick routine
bool CECNPC::Tick(DWORD dwDeltaTime)
{
	CECObject::Tick(dwDeltaTime);

	DWORD dwRealTime = g_pGame->GetRealTickTime();

	//	Update last said words
	if (m_pPateLastWords1 && m_pPateLastWords1->GetItemNum() && m_iLastSayCnt)
	{
		m_iLastSayCnt -= (int)dwDeltaTime;
		if (m_iLastSayCnt <= 0)
		{
			m_iLastSayCnt = 0;

			//	Clear string
			m_pPateLastWords1->Clear();
			m_pPateLastWords2->Clear();
		}
		else
		{
			m_pPateLastWords1->Tick(dwDeltaTime);
			m_pPateLastWords2->Tick(dwDeltaTime);
		}
	}

	switch (m_iCurWork)
	{
	case WORK_STAND:	TickWork_Stand(dwDeltaTime);	break;
	case WORK_FIGHT:	TickWork_Fight(dwDeltaTime);	break;
	case WORK_SPELL:	TickWork_Spell(dwDeltaTime);	break;
	case WORK_DEAD:		TickWork_Dead(dwDeltaTime);		break;
	case WORK_MOVE:		TickWork_Move(dwDeltaTime);		break;
	case WORK_POLICYACTION:	TickWork_PolicyAction(dwDeltaTime);	break;
	}

	if (ShouldUseMasterModel() && !IsUsingMasterModel()){
		TestAndLoadMasterModel();
	}
	m_pNPCModelPolicy->Tick(dwDeltaTime);

	//	Calculate distance to host player
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost && pHost->IsSkeletonReady())
	{
		m_fDistToHost  = CalcDist(pHost->GetPos(), true);
		m_fDistToHostH = CalcDist(pHost->GetPos(), false);
	}

	//	Sort icon state
	if (m_aIconStates.size() > 1 && (m_dwISTimeCnt += dwRealTime) >= 3000)
	{
		m_dwISTimeCnt = 0;
		qsort(m_aIconStates.begin(), m_aIconStates.size(), sizeof(m_aIconStates[0]), glb_IconStateCompare);
	}

	//	Increase disappear counter if it has been triggered
	if (IsDisappearing())
	{
		//	When m_DisappearCnt passed half length, start changing model's transparence
		DWORD dwOldCnt = m_DisappearCnt.GetCounter();
		m_DisappearCnt.IncCounter(dwDeltaTime);
		DWORD dwHalf = m_DisappearCnt.GetPeriod() / 2;
		
		if (dwOldCnt < dwHalf && m_DisappearCnt.GetCounter() >= dwHalf)
			StartAdjustTransparency(-1.0f, 1.0f, dwHalf);
	}
	else
	{
		StartAdjustTransparency(-1.0f, GetTransparentLimit(), 500);
	}

	UpdateTransparency(dwDeltaTime);

	if (m_pBubbleTexts)
		m_pBubbleTexts->Tick(dwDeltaTime);

	if (m_pNPCModelPolicy->IsModelLoaded())
		UpdateMultiObjectEffect(dwDeltaTime);
	
	return true;
}

//	Internal render routine
bool CECNPC::InternalRender(CECViewport* pViewport, int iRenderFlag, bool bHighLight)
{
	m_PateContent.iVisible = 0;

	A3DCameraBase* pCamera = pViewport->GetA3DCamera();
	m_fDistToCamera	= CalcDist(pCamera->GetPos(), true);

	if (!m_bVisible)
		return true;

	if( iRenderFlag == RD_NORMAL )
	{
		if (m_fDistToCamera < 30.0f)
			m_bCastShadow = true;
		else
			m_bCastShadow = false;
	}
	else
	{
		// only show nearby npc in reflect and refract and don't show shadow
		if (m_fDistToCamera > 30.0f)
			return true;

		A3DTerrainWater * pTerrainWater = g_pGame->GetGameRun()->GetWorld()->GetTerrainWater();
		if( iRenderFlag == RD_REFLECT && pTerrainWater->IsUnderWater(GetPickAABB().Maxs) )
			return true;
		else if( iRenderFlag == RD_REFRACT )
		{	
			bool bCamUnderWater = pTerrainWater->IsUnderWater(pCamera->GetPos());
			if( !bCamUnderWater && !pTerrainWater->IsUnderWater(GetPickAABB().Mins) )
				return true;
			else if( bCamUnderWater && pTerrainWater->IsUnderWater(GetPickAABB().Maxs) )
				return true;
		}

		m_bCastShadow = false;
	}

	if (m_pNPCModelPolicy->IsModelLoaded()){
		m_pNPCModelPolicy->Render(pViewport, bHighLight);
		if (m_bCastShadow && g_pGame->GetShadowRender()){
			m_pNPCModelPolicy->AddShadower();
		}
	}
	if (iRenderFlag == RD_NORMAL){
		m_pNPCModelPolicy->RenderPateContent(pViewport, bHighLight);
		
		//	Print NPC's ID
		if (g_pGame->GetConfigs()->IsNPCIDShown())
		{
			ACHAR szText[20];
			a_sprintf(szText, _AL("NPC: %x"), m_NPCInfo.nid);
			g_pGame->GetA3DEngine()->GetSystemFont()->TextOut3D(szText, pViewport->GetA3DViewport(),
				GetPos() + g_vAxisY * (GetPickAABB().Extents.y * 2.1f), 0, A3DCOLORRGB(255, 255, 0));
		}
		
		//	Print host's precise position
		if (g_pGame->GetConfigs()->GetShowPosFlag())
		{
			ACHAR szMsg[100];
			A3DVECTOR3 vPos = GetPos();
			a_sprintf(szMsg, _AL("%.3f, %.3f, %.3f"), vPos.x, vPos.y, vPos.z);
			vPos += g_vAxisY * (GetPickAABB().Extents.y * 2.8f);
			g_pGame->GetA3DEngine()->GetSystemFont()->TextOut3D(szMsg, pViewport->GetA3DViewport(), vPos, 0, 0xffffffff);
			
			a_sprintf(szMsg, _AL("%.3f, %.3f, %.3f"), m_vServerPos.x, m_vServerPos.y, m_vServerPos.z);
			vPos += g_vAxisY * (GetPickAABB().Extents.y * 3.5f);
			g_pGame->GetA3DEngine()->GetSystemFont()->TextOut3D(szMsg, pViewport->GetA3DViewport(), vPos, 0, 0xffffffff);
		}
	}

	RenderMultiObjectGfx();

	return true;
}

//	Process message
bool CECNPC::ProcessMessage(const ECMSG& Msg)
{
	switch (Msg.dwMsg)
	{
	case MSG_NM_NPCATKRESULT:	OnMsgNPCAtkResult(Msg);		break;
	case MSG_NM_NPCEXTSTATE:	OnMsgNPCExtState(Msg);		break;
	case MSG_NM_NPCCASTSKILL:	OnMsgNPCCastSkill(Msg);		break;
	case MSG_NM_ENCHANTRESULT:	OnMsgNPCEnchantResult(Msg);	break;
	case MSG_NM_NPCROOT:		OnMsgNPCRoot(Msg);			break;
	case MSG_NM_NPCSKILLRESULT:	OnMsgNPCSkillResult(Msg);	break;
	case MSG_NM_NPCLEVELUP:		OnMsgNPCLevel(Msg);			break;
	case MSG_NM_NPCINVISIBLE:	OnMsgNPCInvisible(Msg);		break;
	case MSG_NM_NPCSTARTPLAYACTION:	OnMsgNPCStartPlayAction(Msg);	break;
	case MSG_NM_NPCSTOPPLAYACTION:	OnMsgNPCStopPlayAction(Msg);	break;
	case MSG_NM_MULTIOBJECT_EFFECT: OnMsgNPCMultiObjectEffect(Msg);	break;
	}

	return true;
}

//	Move to a destination
void CECNPC::MoveTo(const S2C::cmd_object_move& Cmd)
{
	if (!Cmd.use_time)
		return;

	m_vServerPos	= Cmd.dest;
	m_vMoveDir		= Cmd.dest - GetPos();
	float fDist		= m_vMoveDir.Normalize();

	//	If destination position is too far to us, forcely pull player
	//	to that position.
	if (IsLag(fDist))
	{
		SetPos(Cmd.dest);
		return;
	}

	int iMoveMode = Cmd.move_mode & GP_MOVE_MASK;

	m_bStopMove = false;

	if ( iMoveMode == GP_MOVE_PUSH || iMoveMode == GP_MOVE_PULL )
	{
		//	Push back or pull should occur in stop move command
		ASSERT(false);
		return;
	}

	m_cdr.bTraceGround = true;

	if (Cmd.move_mode & GP_MOVE_AIR)
	{
		m_iMoveEnv = MOVEENV_AIR;
		m_cdr.bTraceGround = false;
	}
	else if (Cmd.move_mode & GP_MOVE_WATER)
	{
		m_iMoveEnv = MOVEENV_WATER;
		m_cdr.bTraceGround = false;
	}
	else
	{
		m_iMoveEnv = MOVEENV_GROUND;

		int iTemp = iMoveMode & GP_MOVE_MASK;
		if (iTemp == GP_MOVE_FALL || iTemp == GP_MOVE_FLYFALL)
			m_cdr.bTraceGround = false;
	}

	m_fMoveSpeed = fDist / (Cmd.use_time * 0.001f);	
//	float fSpeed = FIX8TOFLOAT(Cmd.sSpeed);
//	a_Clamp(m_fMoveSpeed, 0.0f, fSpeed * 1.2f);

	//	Adjust NPC's direction
	if (!IsDirFixed())
	{
		A3DVECTOR3 vDir = m_vMoveDir;
		vDir.y = 0.0f;
		if (!vDir.IsZero())
		{
			vDir.Normalize();
			SetDestDirAndUp(vDir, g_vAxisY, 150);
		}
	}

	if (m_aWorks[WT_NORMAL] != WORK_MOVE || ShouldPlayNewActionFor(iMoveMode) )
	{
		StartWork(WT_NORMAL, WORK_MOVE);
		//	Play run or walk aciton
		PlayMoveAction(iMoveMode);
	}
}

//	Stop moving to a destination position
void CECNPC::StopMoveTo(const S2C::cmd_object_stop_move& Cmd)
{
	if (IsDead())
		return;

	int iMoveMode = Cmd.move_mode & GP_MOVE_MASK;

	m_vMoveDir		= Cmd.dest - GetPos();
	m_bStopMove		= true;
	m_fMoveSpeed	= FIX8TOFLOAT(Cmd.sSpeed);
	m_vServerPos	= Cmd.dest;
	m_vStopDir		= glb_DecompressDirH(Cmd.dir);

	// only store the passive move mode
	m_iPassiveMove	= (iMoveMode == GP_MOVE_PUSH || iMoveMode == GP_MOVE_PULL || iMoveMode == GP_MOVE_BLINK) ? 
					  iMoveMode : 0;

	if (IsDirFixed()){
		// 朝向固定，直接设定
		SetDirAndUp(m_vStopDir, g_vAxisY);
	}

	float fDist = m_vMoveDir.Normalize();

	//	In below case, we should forcely pull npc to destination position.
	//		1: If destination position is too far to npc.
	//		2: If destination position is very near to npc and npc's move-dir point
	//			to a different direction from npc's stop-dir (m_vStopDir)
	//		3: 瞬移模式
	if (iMoveMode != GP_MOVE_RETURN && iMoveMode != GP_MOVE_PUSH && iMoveMode != GP_MOVE_PULL)
	{
		bool bPull = false;
		
		if (IsLag(fDist)){
			//	case 1
			bPull = true;
		}else if (fDist < 1.0f){
			//	case 2
			A3DVECTOR3 vDirH = m_vMoveDir;
			vDirH.y = 0.0f;
			vDirH.Normalize();
			if (DotProduct(vDirH, m_vStopDir) < 0.7f)
				bPull = true;
		}else if (iMoveMode == GP_MOVE_BLINK){
			//	case 3
			bPull = true;
		}

		if (bPull)
		{
			SetPos(Cmd.dest);
			if (!IsDirFixed()){
				SetDestDirAndUp(m_vStopDir, g_vAxisY, 150);
			}
			WorkFinished(WORK_MOVE);
			return;
		}
	}

	m_cdr.bTraceGround = true;

	if (Cmd.move_mode & GP_MOVE_AIR)
	{
		m_iMoveEnv = MOVEENV_AIR;
		m_cdr.bTraceGround = false;
	}
	else if (Cmd.move_mode & GP_MOVE_WATER)
	{
		m_iMoveEnv = MOVEENV_WATER;
		m_cdr.bTraceGround = false;
	}
	else
	{
		m_iMoveEnv = MOVEENV_GROUND;

		if (iMoveMode == GP_MOVE_FALL || iMoveMode == GP_MOVE_FLYFALL)
			m_cdr.bTraceGround = false;
	}

	if (!IsDirFixed() && !m_iPassiveMove)
	{
		A3DVECTOR3 vDir = m_vMoveDir;
		vDir.y = 0.0f;
		if (!vDir.IsZero())
		{
			vDir.Normalize();
			SetDestDirAndUp(vDir, g_vAxisY, 150);
		}
	}

	if (m_aWorks[WT_NORMAL] != WORK_MOVE)
	{
		StartWork(WT_NORMAL, WORK_MOVE);

		if (!m_iPassiveMove)
		{
			//	Play run or walk action when not passive move
			PlayMoveAction(iMoveMode);
		}
	}
}

int CECNPC::GetMoveAction(int iMoveMode)
{
	if (iMoveMode == GP_MOVE_RUN || iMoveMode == GP_MOVE_RETURN)
	{
		if (IsMonsterOrPet())
			return ACT_RUN;
		else
			return ACT_NPC_RUN;
	}
	else
	{
		if (IsMonsterOrPet())
			return ACT_WALK;
		else
			return ACT_NPC_WALK;
	}
}
//	Play move action
void CECNPC::PlayMoveAction(int iMoveMode)
{
	//	Play run or walk aciton
	if (iMoveMode == GP_MOVE_RUN || iMoveMode == GP_MOVE_RETURN)
	{
		if (IsMonsterOrPet())
			PlayModelAction(ACT_RUN, false);
		else
			PlayModelAction(ACT_NPC_RUN, false);
	}
	else
	{
		if (IsMonsterOrPet())
			PlayModelAction(ACT_WALK, false);
		else
			PlayModelAction(ACT_NPC_WALK, false);
	}
}

//	NPC ground move
bool CECNPC::MovingTo(DWORD dwDeltaTime)
{
	//	If NPC is disappearing, doesn't move
	if (IsDisappearing())
		return true;

	bool bRet = false;

	A3DVECTOR3 vPos, vCurPos = GetPos();
	float fDeltaTime = dwDeltaTime * 0.001f;
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();

	if (m_bStopMove)
	{
		A3DVECTOR3 vDir = m_vServerPos - vCurPos;
		float fDist = vDir.Normalize();
		vPos = MoveStep(vDir, m_fMoveSpeed, fDeltaTime);

		float fMoveDelta = a3d_Magnitude(vPos - vCurPos);
		if (fMoveDelta >= fDist)
		{
			SetPos(m_vServerPos);
			bRet = true;
		}
		else
			SetPos(vPos);
	}
	else	//	Just move on
	{
		//	If we have move so far from destination and still don't 
		//	receive new 'move' or 'stop move' command, it's better to
		//	stop moving and goto last destination at once
		float fDist = a3d_Magnitude(m_vServerPos - vCurPos);
		if (IsLag(fDist))
		{
			SetPos(m_vServerPos);
			m_vStopDir = GetDir();
			return true;
		}
	
		A3DVECTOR3 vDir = m_vMoveDir;
		vDir.Normalize();
		vPos = MoveStep(vDir, m_fMoveSpeed, fDeltaTime);
		SetPos(vPos);
	}

	return bRet;
}

//	Move step
A3DVECTOR3 CECNPC::MoveStep(const A3DVECTOR3& vDir, float fSpeed, float fTime)
{
	A3DVECTOR3 vRealDir = vDir;

	//	OnAirMove only accept positive speed value
	if (fSpeed < 0.0f)
	{
		vRealDir = -vDir;
		fSpeed = -fSpeed;
	}

	m_cdr.vCenter		= GetPos() + g_vAxisY * m_cdr.vExts.y;
	m_cdr.vVelocity		= vRealDir * fSpeed;
	m_cdr.t				= fTime;
	m_cdr.bTestTrnOnly	= true;

	OtherPlayerMove(m_cdr);

	if( m_cdr.bTraceGround )
		SetGroundNormal(m_cdr.vecGroundNormal);
	
	return m_cdr.vCenter - g_vAxisY * m_cdr.vExts.y;
}

//	NPC is moving ?
bool CECNPC::IsMoving()const
{
	return m_iCurWork == WORK_MOVE ? true : false;
}

const char* CECNPC::GetBaseActionName(int iAct)
{
	return m_ActionNames.GetANSIString(iAct);
}

bool CECNPC::IsAttackAction(int iAct)
{
	return iAct == ACT_ATTACK1
		|| iAct == ACT_ATTACK2
		|| iAct == ACT_NPC_ATTACK;
}

//	Play model action
void CECNPC::PlayModelAction(int iAction, bool bRestart/* =true */)
{
	m_iAction = iAction;	
	if (IsDead())
	{
		//	检查死亡状态，以屏蔽其它动作
		//	死亡状态设置后，只允许播放死亡动作
		//	玩家攻击NPC时，会首先播放玩家的攻击动作，完成后播放怪物的受伤动作
		//	但在玩家自身的攻击动作未完成时，可能就收到NPC死亡的消息
		//	因此，可能会先播放死亡动作，而后又播放受伤动作，导致死亡动作不能正常播放的表现结果
		//	不符合期望
		if (IsMonsterOrPet())
		{
			if (iAction != CECNPC::ACT_DIE)
			{
				return;
			}
		}
		else
		{
			if (iAction != CECNPC::ACT_NPC_DIE)
			{
				return;
			}
		}
	}
	m_pNPCModelPolicy->PlayModelAction(iAction, bRestart);
}

//	Set absolute position
void CECNPC::SetPos(const A3DVECTOR3& vPos)
{
	CECObject::SetPos(vPos);
	m_pNPCModelPolicy->SetPos(vPos);
}

//	Set absolute forward and up direction
void CECNPC::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	CECObject::SetDirAndUp(vDir, vUp);
	m_pNPCModelPolicy->SetDirAndUp(GetDir(), GetUp());
}

//	NPC was killed
void CECNPC::Killed(bool bDelay)
{
	ClearComActFlag(true);

	m_dwStates |= GP_STATE_CORPSE;

	//	No delay die, enter disappear process immediately
	if (!bDelay)
		Disappear();

	StartWork(WT_NORMAL, WORK_DEAD, m_dwStates);

	SetUseGroundNormal(true);
}

void CECNPC::Disappear()
{
	//	Trigger disappear counting to start
	m_DisappearCnt.SetCounter(1);

	PlayModelAction(ACT_NPC_DISAPPEAR);
}

//	NPC is damaged
void CECNPC::Damaged(int iDamage, DWORD dwModifier/* 0 */)
{
	if (iDamage == -1 || iDamage == -2)
	{
		//	when else player hit this npc iDamage is -1, 
		//	so if iDamage is -1 we will shoud the wounded animation
		if (iDamage == -1)
			PlayModelAction(ACT_WOUNDED);

		if ((dwModifier & CECAttackEvent::MOD_IMMUNE) && !IsImmuneDisable())
			BubbleText(BUBBLE_IMMUNE, 0);
		else if (dwModifier & CECAttackEvent::MOD_NULLITY)
			BubbleText(BUBBLE_INVALIDHIT, 0);
		else if (dwModifier & CECAttackEvent::MOD_ENCHANT_FAILED)
			BubbleText(BUBBLE_LOSE, 0);
		else if (dwModifier & CECAttackEvent::MOD_SUCCESS)
			BubbleText(BUBBLE_SUCCESS, 0);
		else if (dwModifier & CECAttackEvent::MOD_DODGE_DEBUFF)
			BubbleText(BUBBLE_DODGE_DEBUFF, 0);
	}
	else
	{
		//	this message is related to the host, so we should show a pop up message
		//	Popup a damage decal
		bool bDeadlyStrike = (dwModifier & CECAttackEvent::MOD_CRITICAL_STRIKE) ? true : false;
		bool bRetort = (dwModifier & CECAttackEvent::MOD_RETORT) ? true : false;

		if (iDamage > 0)
		{
			PlayModelAction(ACT_WOUNDED);

			int p1 = 0;
			if (bDeadlyStrike)
				p1 |= 0x0001;
			else if (bRetort)
				p1 |= 0x0002;

			if (dwModifier & CECAttackEvent::MOD_REBOUND)
				BubbleText(BUBBLE_REBOUND, (DWORD)iDamage);
			else if (dwModifier & CECAttackEvent::MOD_BEAT_BACK)
				BubbleText(BUBBLE_BEAT_BACK, (DWORD)iDamage);
			else
				BubbleText(BUBBLE_DAMAGE, (DWORD)iDamage, p1);
		}
		else if ((dwModifier & CECAttackEvent::MOD_IMMUNE) && !IsImmuneDisable())
			BubbleText(BUBBLE_IMMUNE, 0);
		else if (dwModifier & CECAttackEvent::MOD_NULLITY)
			BubbleText(BUBBLE_INVALIDHIT, 0);
		else if (dwModifier & CECAttackEvent::MOD_ENCHANT_FAILED)
			BubbleText(BUBBLE_LOSE, 0);
		else if (dwModifier & CECAttackEvent::MOD_SUCCESS)
			BubbleText(BUBBLE_SUCCESS, 0);
		else
			BubbleText(BUBBLE_HITMISSED, 0);
	}
}

//	Play attack action
bool CECNPC::PlayAttackAction(int nAttackSpeed, bool *pActFlag)
{
	return m_pNPCModelPolicy->PlayAttackAction(nAttackSpeed, pActFlag);
}

// Play skill cast action
void CECNPC::PlaySkillCastAction(int idSkill)
{
	m_pNPCModelPolicy->PlaySkillCastAction(idSkill);
}

// Play skill attack action
bool CECNPC::PlaySkillAttackAction(int idSkill, int nAttackSpeed,int nSection, bool *pActFlag)
{
	return m_pNPCModelPolicy->PlaySkillAttackAction(idSkill, nAttackSpeed, nSection, pActFlag);
}

void CECNPC::PlayAttackEffect(int idTarget, int idSkill, int skillLevel, int nDamage, DWORD dwModifier, int nAttackSpeed, int nSection)
{
	if (!m_pNPCModelPolicy->IsModelLoaded())
		return;

	m_bStartFight = true;

	CECAttacksMan* pAttacksMan = g_pGame->GetGameRun()->GetWorld()->GetAttacksMan();

	// see if a melee attack
	if( !idSkill )
	{
		int nTimeFly = 10;

		if (IsMonsterNPC())
		{
			MONSTER_ESSENCE * pData = (MONSTER_ESSENCE *) (((CECMonster *)this)->GetDBEssence());
			if( pData->short_range_mode == 0 ) // 远程物理攻击
				nTimeFly = 700;
		}
		if (IsPetNPC()) // && strlen(gfx )>0
		{
			PET_ESSENCE* pData = (PET_ESSENCE*)(((CECPet*)this)->GetDBEssence());
			if(pData && strlen(pData->file_gfx_short))
				nTimeFly = 700;
		}

		if( pAttacksMan->FindAttackByAttacker(m_NPCInfo.nid) )
		{
			// signal early attack event
			ClearComActFlag(true);
		}

		// melee attack
		CECAttackEvent *pAttack = pAttacksMan->AddMeleeAttack(m_NPCInfo.nid, idTarget, 0, 
							dwModifier, nDamage, nTimeFly);

		if( !IsDead() && (dwModifier & CECAttackEvent::MOD_RETORT) == 0
			&& (dwModifier & CECAttackEvent::MOD_ATTACK_AURA) == 0
			&& PlayAttackAction(nAttackSpeed, &pAttack->m_bSignaled) 
			&& (dwModifier & CECAttackEvent::MOD_BEAT_BACK) == 0){
		}
		else
			pAttack->m_bSignaled = true;
	}
	else
	{
		if( skillLevel == 0 )
		{
			if( m_pCurSkill )
				skillLevel = m_pCurSkill->GetSkillLevel();
			else
				skillLevel = 1;
		}
		
		// first try to find if there is already a skill attack event in attackman
		CECAttackerEvents attackerEvents = pAttacksMan->FindAttackByAttacker(m_NPCInfo.nid);

		if( attackerEvents )
		{
			if(CECAttackEvent *pAttack = attackerEvents.Find(idSkill, nSection))
			{
				// 面攻击的非第一次伤害消息
				pAttack->AddTarget(idTarget, dwModifier, nDamage);
				return;
			}
			else
			{
				attackerEvents.Signal();
			}
		}

		// begin a skill attack
		CECAttackEvent *pAttack = pAttacksMan->AddSkillAttack(m_NPCInfo.nid, m_idCurSkillTarget, 
							idTarget, 0, idSkill, skillLevel, dwModifier, nDamage);

		if( pAttack )
		{
			pAttack->SetSkillSection(nSection);
			if( !IsDead() && (dwModifier & CECAttackEvent::MOD_RETORT) == 0
				&& (dwModifier & CECAttackEvent::MOD_ATTACK_AURA) == 0
				&& PlaySkillAttackAction(idSkill, nAttackSpeed,nSection, &pAttack->m_bSignaled)
				&& (dwModifier & CECAttackEvent::MOD_BEAT_BACK) == 0){
			}
			else
				pAttack->m_bSignaled = true;
		}
	}
}

//	Stop attack
void CECNPC::StopAttack()
{
	if (m_aWorks[WT_NORMAL] == WORK_FIGHT)
	{
		ReleaseWork(WT_NORMAL);

		if (m_iCurWorkType == WT_NORMAL)
		{
			m_iCurWorkType = WT_NOTHING;
			StartWorkByID(m_aWorks[WT_NOTHING], 0);
		}

		// signal early attack event
		ClearComActFlag(true);
	}
}

//	Release work resources of specified type
void CECNPC::ReleaseWork(int iWorkType)
{
	ASSERT(iWorkType >= 0 && iWorkType < NUM_WORKTYPE);

	switch (m_aWorks[iWorkType])
	{
	case WORK_STAND:	break;
	case WORK_FIGHT:	break;
	case WORK_SPELL:	break;
	case WORK_DEAD:		break;
	case WORK_MOVE:
		{
			//	为防止 WORK_MOVE 被 WORK_SPELL 等 WORK 冲掉导致 NPC 位置错误，此时检查并直接将 NPC 拉向目标位置
			A3DVECTOR3 vDelta = m_vServerPos - GetPos();
			float fDist = vDelta.Magnitude();
			if (fDist > 0.1f){
				SetPos(m_vServerPos);
				if (!IsDirFixed()){
					SetDestDirAndUp(m_vStopDir, g_vAxisY, 150);
				}
				RebuildTraceBrush();
			}
			break;
		}
	case WORK_POLICYACTION:
		{
			m_pNPCModelPolicy->StopChannelAction();
			delete m_pPolicyAction;
			m_pPolicyAction = NULL;
			m_nPolicyActionIntervalTimer = 0;
			break;
		}
	}

	m_aWorks[iWorkType] = 0;

	if (m_iCurWorkType == iWorkType)
		m_iCurWork = 0;
}

//	Start a work of specified type
void CECNPC::StartWork(int iWorkType, int iNewWork, DWORD dwParam/* 0 */)
{
	ASSERT(iWorkType >= 0 && iWorkType < NUM_WORKTYPE);

	if (iNewWork == WORK_DEAD)
	{
		//	Dead is a special work
		ReleaseWork(WT_INTERRUPT);
		ReleaseWork(WT_NORMAL);

		m_aWorks[WT_NORMAL]	= iNewWork;
		m_iCurWorkType		= WT_NORMAL;
	}
	else if (iWorkType == WT_INTERRUPT)
	{
		//	Release old work
		ReleaseWork(WT_INTERRUPT);
		m_aWorks[WT_INTERRUPT] = iNewWork;

		if (m_iCurWorkType == WT_NORMAL || m_iCurWorkType == WT_NOTHING)
			StopWork(m_iCurWorkType);

		m_aWorks[WT_INTERRUPT] = iNewWork;
		m_iCurWorkType = WT_INTERRUPT;
	}
	else if (iWorkType == WT_NORMAL)
	{
		//	Release old work
		ReleaseWork(WT_NORMAL);
		m_aWorks[WT_NORMAL] = iNewWork;

		if (m_iCurWorkType < 0 || m_iCurWorkType == WT_NORMAL || m_iCurWorkType == WT_NOTHING)
		{
			if (m_iCurWorkType == WT_NOTHING)
				StopWork(WT_NOTHING);

			m_iCurWorkType = WT_NORMAL;
		}
		else
			return;
	}
	else	//	iWorkType == WT_NOTHING
	{
		//	Release old work
		ReleaseWork(WT_NOTHING);
		m_aWorks[WT_NOTHING] = iNewWork;

		if (m_iCurWorkType < 0 || m_iCurWorkType == WT_NOTHING)
			m_iCurWorkType = WT_NOTHING;
		else
			return;
	}

	StartWorkByID(iNewWork, dwParam);
}

//	Start works 
void CECNPC::StartWorkByID(int iWorkID, DWORD dwParam)
{
	//	Ignore all message if this NPC is dead.
//	if (IsDead())
//		return;

	switch (iWorkID)
	{
	case WORK_STAND:	StartWork_Stand(dwParam);	break;
	case WORK_FIGHT:	StartWork_Fight(dwParam);	break;
	case WORK_SPELL:	StartWork_Spell(dwParam);	break;
	case WORK_DEAD:		StartWork_Dead(dwParam);	break;
	case WORK_MOVE:		StartWork_Move(dwParam);	break;
	case WORK_POLICYACTION:	StartWork_PolicyAction(dwParam);	break;
	}

	//if(iWorkID != WORK_MOVE) m_iPassiveMove = 0;
	m_iCurWork = iWorkID;
}

//	Stop / pause work of specified type
void CECNPC::StopWork(int iWorkType)
{
}

//	Current work finished
void CECNPC::WorkFinished(int iWorkID)
{
	//	Note: below judge can prevent much problems when we attempt to
	//		finish a work but don't assure we are doing this work
	if (m_iCurWork != iWorkID)
		return;

	ASSERT(m_iCurWork > 0 && m_iCurWorkType >= 0);

	//	Release current work
	ReleaseWork(m_iCurWorkType);

	for (int i=m_iCurWorkType-1; i >= 0; i--)
	{
		if (m_aWorks[i])
		{
			m_iCurWorkType = i;
			StartWorkByID(m_aWorks[i], 0);
			break;
		}
	}

	// clear passive move flag
	if(iWorkID == WORK_MOVE) m_iPassiveMove = 0;
}

//	Start work WORK_STAND
void CECNPC::StartWork_Stand(DWORD dwParam)
{
	if (!m_bStartFight)
	{
		if (IsMonsterOrPet())
			PlayModelAction(ACT_STAND);
		else
			PlayModelAction(ACT_NPC_STAND);
	}
}

//	Start work WORK_FIGHT
void CECNPC::StartWork_Fight(DWORD dwParam)
{
	m_nFightTimeLeft = dwParam;
	// we should not play attack animation here, because all animation are controlled
	// through attack result messages.
}

//	Start work WORK_SPELL
void CECNPC::StartWork_Spell(DWORD dwParam)
{
}

//	Start work WORK_DEAD
void CECNPC::StartWork_Dead(DWORD dwParam)
{
	if (IsMonsterOrPet())
		PlayModelAction(ACT_DIE);
	else
		PlayModelAction(ACT_NPC_DIE);
}

//	Start work WORK_MOVE
void CECNPC::StartWork_Move(DWORD dwParam)
{
	m_bStartFight = false;

	if( m_pNPCModelPolicy->IsModelLoaded() )
	{
		ClearComActFlag(true);

		// when npc is moving, no collision detection needed.
		ReleaseTraceBrush();
	}
}

void CECNPC::StartWork_PolicyAction(DWORD dwParam)
{
	if (!m_pPolicyAction)
		m_pPolicyAction = new CECPolicyAction;
	
	m_pPolicyAction->Init((const S2C::cmd_object_start_play_action *)dwParam);
	m_pPolicyAction->Tick(0);
	m_nPolicyActionIntervalTimer = 0;

	CheckStartPolicyAction();
}

bool CECNPC::IsInPolicyAction()const
{
	return m_pPolicyAction != NULL;
	//return (m_iCurWork == WORK_POLICYACTION);
}

void CECNPC::CheckStartPolicyAction()
{
	//	收到服务器协议后、判断首次启动策略动作
	//
	if (!IsInPolicyAction())
		return;

	//	模型未准备好
	if (!m_pNPCModelPolicy->IsModelLoaded())
		return;

	//	清除动作通知状态
	ClearComActFlag(true);

	//	播放新动作
	if (!PlayPolicyActionAgain(true))
	{
		//	NPC 模型没有指定策略动作时，立即停止当前动作
		m_pNPCModelPolicy->StopChannelAction();
	}
}

bool CECNPC::PlayPolicyActionAgain(bool bFirst)
{
	//	播放动作成功时返回 true
	//
	bool bRet(false);

	if (CanPlayPolicyAction())
	{
		const AString &strAct = m_pPolicyAction->GetActionName();
		m_pNPCModelPolicy->PlayActionByName(strAct);

		//	重置空闲计时器
		m_nPolicyActionIntervalTimer = 0;

		bRet = true;
	}
	
	return bRet;
}

void CECNPC::PlayPolicyIntervalAction()
{
	//	策略动作播放间隔中，播放此动作
	int iAction = IsMonsterOrPet() ? ACT_STAND : ACT_NPC_STAND;
	PlayModelAction(iAction);
}

bool CECNPC::CanPlayPolicyAction()const
{
	//	有策略动作需要播放、并且 NPC 模型上有这个动作时，返回 true	
	//
	bool bRet(false);

	while (true)
	{
		if (!m_pNPCModelPolicy->IsModelLoaded())
			break;
		
		if (!m_pPolicyAction || !m_pPolicyAction->IsValid() || !m_pPolicyAction->HasAction())
			break;
		
		const AString &strAct = m_pPolicyAction->GetActionName();
		if (strAct.IsEmpty() || !m_pNPCModelPolicy->HasComAct(strAct))
			break;

		bRet = true;
		break;
	}

	return bRet;
}

void CECNPC::TickWork_PolicyAction(DWORD dwDeltaTime)
{
	ASSERT(m_pPolicyAction != NULL);
	if (!m_pPolicyAction)
		return;

	m_pPolicyAction->Tick(dwDeltaTime);
	if (!m_pPolicyAction->IsInPolicy())
	{
		//	策略动作结束

		//	由于服务器不能保证 HostPlayer 成对收到 OnMsgNPCStartPlayAction 和 OnMsgNPCStopPlayAction
		//	客户端因此需要自己计时处理、并增强 OnMsgNPCStartPlayAction 和 OnMsgNPCStopPlayAction 的鲁棒性
		//

		CheckStopPolicyAction();
		return;
	}

	if (!m_pNPCModelPolicy->IsModelLoaded())
		return;

	//	处理动作播放

	if (CanPlayPolicyAction())
	{
		//	策略动作有机会播放情况处理

		if (m_nPolicyActionIntervalTimer > 0)
		{
			//	当前处于空闲状态

			DWORD t = timeGetTime();
			if (t >= m_nPolicyActionIntervalTimer + m_pPolicyAction->GetIntervalTime())
			{
				//	空闲时间已过，重新播放动作
				PlayPolicyActionAgain(false);
				return;
			}

			//	空闲时间未过
			
			//	循环播放备选动作
			if (!m_pNPCModelPolicy->IsPlayingAction())
				PlayPolicyIntervalAction();
			return;
		}

		//	当前不处于空闲状态

		if (m_pNPCModelPolicy->IsPlayingAction())
		{
			//	策略动作还在继续播放
			return;
		}

		//	策略动作播放结束

		//	检查是否有空闲状态
		if (m_pPolicyAction->HasInterval())
		{
			//	进入空闲状态
			m_nPolicyActionIntervalTimer = timeGetTime();

			//	播放空闲状态动作
			PlayPolicyIntervalAction();
			return;
		}

		//	没有空闲状态，继续播放动作
		PlayPolicyActionAgain(false);
		return;
	}

	//	没有播放过有效策略动作，继续保持当前状态
}

void CECNPC::TickWork_Stand(DWORD dwDeltaTime)
{
	if( m_IdleCnt.IncCounter(dwDeltaTime) )
	{
		m_IdleCnt.Reset(0);

		if (IsMonsterOrPet())
		{
			PlayModelAction(ACT_IDLE);
			if (IsPetNPC() && !IsDead() && !IsDisappearing())
				OnPetSays(CECPetWords::TW_REST);
		}
		else
			PlayModelAction(ACT_NPC_IDLE1 + (rand() % 2));
	}
}

void CECNPC::TickWork_Fight(DWORD dwDeltaTime)
{
	m_nFightTimeLeft -= dwDeltaTime;

	if (m_nFightTimeLeft < 0)
	{
		m_nFightTimeLeft = 0;
		WorkFinished(WORK_FIGHT);
		return;
	}

	//	Face to destination
	NPCTurnFaceTo(m_idAttackTarget, 100);
	
	//	Slide to server position
	A3DVECTOR3 vDir = m_vServerPos - GetPos();
	float fDist = vDir.Normalize();
	if (fDist > 0.0001f)
	{
		float fMoveDist = 10.0f * dwDeltaTime * 0.001f;
		if (fMoveDist > fDist)
			fMoveDist = fDist;

		SetPos(GetPos() + vDir * fMoveDist);
	}
}

void CECNPC::TickWork_Spell(DWORD dwDeltaTime)
{
	int iRealTime = g_pGame->GetRealTickTime();
	if (m_SkillCnt.IncCounter(iRealTime))
	{
		WorkFinished(WORK_SPELL);

		if (m_pCurSkill)
		{
			//	Release current skill
			delete m_pCurSkill;
			m_pCurSkill = NULL;
		}

		m_idCurSkillTarget = 0;
	}
	else
	{
		if (m_idCurSkillTarget && m_idCurSkillTarget != m_NPCInfo.nid)
			NPCTurnFaceTo(m_idCurSkillTarget);
	}
}

void CECNPC::TickWork_Dead(DWORD dwDeltaTime)
{
}

void CECNPC::TickWork_Move(DWORD dwDeltaTime)
{
	//	Move NPC
	if (m_bAboutToDie)
	{
		WorkFinished(WORK_MOVE);
	}
	else if (MovingTo(dwDeltaTime))
	{
		if (!IsDirFixed()){
			SetDestDirAndUp(m_vStopDir, g_vAxisY, 150);
		}
		WorkFinished(WORK_MOVE);

		// when stopped, we should rebuild the convex brushes for collision detection.
		RebuildTraceBrush();
	}
}

void CECNPC::OnMsgNPCAtkResult(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_object_atk_result* pCmd = (cmd_object_atk_result*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->attacker_id == m_NPCInfo.nid);

	if( !IsDead() )
	{
		//	Face to target
		NPCTurnFaceTo(pCmd->target_id);
		m_idAttackTarget = pCmd->target_id;

		//	now start a fight work
		if (IsMonsterNPC())
		{
			CECMonster * pMonster = (CECMonster *) this;
			const MONSTER_ESSENCE * pMonsterEssence = pMonster->GetDBEssence();
			StartWork(WT_NORMAL, WORK_FIGHT, DWORD(pMonsterEssence->attack_speed * 1000));
		}
	}

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	int iDamage = -1;
	if (IsPetNPC() && GetMasterID() == pHost->GetCharacterID())
	{
		//	Attacker is host's pet
		iDamage = pCmd->damage;
	}
	else if (ISNPCID(pCmd->target_id))
	{
		//	Attack target is host's pet
		CECNPC* pTarget = m_pNPCMan->GetNPC(pCmd->target_id);
		if (pTarget && pTarget->GetMasterID() == pHost->GetCharacterID())
			iDamage = pCmd->damage;
	}

	// common melee attack result, so let idSkill to be 0
	PlayAttackEffect(pCmd->target_id, 0, 0, iDamage, pCmd->attack_flag, pCmd->speed * 50);

	if (!IsDead() && IsPetNPC())
		OnPetSays(CECPetWords::TW_FIGHT);
}

void CECNPC::OnMsgAttackHostResult(int idHost, int nDamage, int nFlag, int nSpeed)
{
	if( !IsDead() )
	{
		//	Face to target
		NPCTurnFaceTo(idHost);
		m_idAttackTarget = idHost;

		//	now start a fight work
		if (IsMonsterNPC())
		{
			CECMonster * pMonster = (CECMonster *) this;
			const MONSTER_ESSENCE * pMonsterEssence = pMonster->GetDBEssence();
			StartWork(WT_NORMAL, WORK_FIGHT, DWORD(pMonsterEssence->attack_speed * 1000));
		}
	}

	PlayAttackEffect(idHost, 0, 0, nDamage, nFlag, nSpeed);
	
	if (!IsDead() && IsPetNPC())
		OnPetSays(CECPetWords::TW_FIGHT);
}

void CECNPC::OnMsgNPCSkillResult(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_object_skill_attack_result* pCmd = (cmd_object_skill_attack_result*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->attacker_id == m_NPCInfo.nid);

	if( !IsDead() )
	{
		//	Face to target
		NPCTurnFaceTo(pCmd->target_id);
	}

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	int iDamage = -1;
	if (IsPetNPC() && GetMasterID() == pHost->GetCharacterID())
	{
		//	Attacker is host's pet
		iDamage = pCmd->damage;
	}
	else if (ISNPCID(pCmd->target_id))
	{
		//	Attack target is host's pet
		CECNPC* pTarget = m_pNPCMan->GetNPC(pCmd->target_id);
		if (pTarget && pTarget->GetMasterID() == pHost->GetCharacterID())
			iDamage = pCmd->damage;
	}

	// common melee attack result, so let idSkill to be 0
	PlayAttackEffect(pCmd->target_id, pCmd->skill_id, 0, iDamage, pCmd->attack_flag, pCmd->speed * 50,pCmd->section);
}

void CECNPC::OnMsgNPCExtState(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == UPDATE_EXT_STATE)
	{
		cmd_update_ext_state* pCmd = (cmd_update_ext_state*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->id == m_NPCInfo.nid);
		SetNewExtendStates(0, pCmd->states, OBJECT_EXT_STATE_COUNT);
	}
	else
	{
		ASSERT(Msg.dwParam2 == ICON_STATE_NOTIFY);
		cmd_icon_state_notify cmd;
		if (!cmd.Initialize((const void *)Msg.dwParam1, Msg.dwParam3))
		{
			ASSERT(0);
			return;
		}
		ASSERT(cmd.id == m_NPCInfo.nid);
		
		m_aIconStates = cmd.states;
	}
}

void CECNPC::OnMsgNPCCastSkill(const ECMSG& Msg)
{
	using namespace S2C;

	bool bDoOther = false;
	int iSkillOver = 0;
	CECPetCorral* pPetCorral = g_pGame->GetGameRun()->GetHostPlayer()->GetPetCorral();

	if( Msg.dwParam2 == OBJECT_CAST_SKILL )
	{
		cmd_object_cast_skill* pCmd = (cmd_object_cast_skill*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->caster == m_NPCInfo.nid);

		if( pCmd->caster == pPetCorral->GetActivePetNPCID() )
		{
			iSkillOver = pCmd->skill;
			bDoOther = true;
		}

		if (m_pCurSkill)
		{
			//	Release current skill
			delete m_pCurSkill;
		}

		if (!(m_pCurSkill = new CECSkill(pCmd->skill, pCmd->level)))
			return;

		m_SkillCnt.SetPeriod((pCmd->time + m_pCurSkill->GetExecuteTime()) * 10);
		m_SkillCnt.Reset();
		StartWork(WT_NORMAL, WORK_SPELL);

		m_idCurSkillTarget = pCmd->target;
		PlaySkillCastAction(m_pCurSkill->GetSkillID());
		
		if (!IsDead() && IsPetNPC())
			OnPetSays(CECPetWords::TW_FIGHT);
	}
	else if( Msg.dwParam2 == SKILL_INTERRUPTED )
	{
		ClearComActFlag(false);

		WorkFinished(WORK_SPELL);

		if (m_pCurSkill)
		{
			//	Release current skill
			delete m_pCurSkill;
			m_pCurSkill = NULL;
		}

		m_idCurSkillTarget = 0;

		StartWork_Stand(0);
	}
	else if (Msg.dwParam2 == OBJECT_CAST_INSTANT_SKILL)
	{
		cmd_object_cast_instant_skill* pCmd = (cmd_object_cast_instant_skill*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->caster == m_NPCInfo.nid);

		if( pCmd->caster == pPetCorral->GetActivePetNPCID() )
		{
			iSkillOver = pCmd->skill;
			bDoOther = true;
		}

		if (m_pCurSkill)
		{
			//	Release current skill
			delete m_pCurSkill;
			m_pCurSkill = NULL;
			m_idCurSkillTarget = 0;
		}

	//	m_idCurSkillTarget = pCmd->target;
		PlaySkillCastAction(pCmd->skill);
		
		if (!IsDead() && IsPetNPC())
			OnPetSays(CECPetWords::TW_FIGHT);
	}

	// 宠物出场自动技能释放成功
	if( bDoOther )
	{
		CECPetData* pPetData = pPetCorral->GetActivePet();
		if( pPetData )
			pPetData->OnAutoCastOver(iSkillOver);
	}
}

void CECNPC::OnMsgNPCEnchantResult(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_enchant_result* pCmd = (cmd_enchant_result*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->caster == m_NPCInfo.nid);

	// we should filter out these things that will cause bubble texts
//	char mask = (char)0xff;
//	if( pCmd->target != g_pGame->GetGameRun()->GetHostPlayer()->GetCharacterID() )
//		mask &= CECAttackEvent::MOD_PHYSIC_ATTACK_RUNE | CECAttackEvent::MOD_MAGIC_ATTACK_RUNE | CECAttackEvent::MOD_CRITICAL_STRIKE;

//	DWORD dwModifier = (((DWORD)pCmd->modifier2) << 8) | pCmd->modifier;
	DWORD dwModifier = (DWORD)pCmd->attack_flag;

	if( GNET::ElementSkill::GetType(pCmd->skill) == GNET::TYPE_ATTACK )
	{
		// only attack skill will cause wounded action, when damage is -1
		PlayAttackEffect(pCmd->target, pCmd->skill, pCmd->level, -1, dwModifier, 0,pCmd->section);
	}
	else
	{
		// other skills will not cause wounded action, so we set damage to -2 which will be discarded to play wounded action
		PlayAttackEffect(pCmd->target, pCmd->skill, pCmd->level, -2, dwModifier, 0,pCmd->section);
	}

	// 怪物诅咒玩家的情况
	if( IsMonsterNPC() &&
		pCmd->target == g_pGame->GetGameRun()->GetHostPlayer()->GetCharacterID() &&
		CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
	{
		// 通知自动策略，玩家被攻击
		CECAutoPolicy::GetInstance().SendEvent_BeHurt(pCmd->caster);
	}
}

void CECNPC::OnMsgNPCRoot(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_object_root* pCmd = (cmd_object_root*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->id == m_NPCInfo.nid);

	m_vServerPos = pCmd->pos;
	
	WorkFinished(WORK_MOVE);
}

void CECNPC::OnMsgNPCLevel(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_level_up* pCmd = (cmd_level_up*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->id == m_NPCInfo.nid);
	m_pNPCModelPolicy->PlayGfx(res_GFXFile(RES_GFX_LEVELUP), NULL);
}

void CECNPC::OnMsgNPCInvisible(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_object_invisible* pCmd = (cmd_object_invisible*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->id == m_NPCInfo.nid);
	
	if(pCmd->invisible_degree > 0)
	{
		m_dwStates |= GP_STATE_INVISIBLE;
	}
	else
	{
		m_dwStates &= ~GP_STATE_INVISIBLE;
	}
}

void CECNPC::OnMsgNPCStartPlayAction(const ECMSG& Msg)
{
	if (IsInPolicyAction())
		CheckStopPolicyAction();

	StartWork(WT_INTERRUPT, WORK_POLICYACTION, Msg.dwParam1);
}

void CECNPC::OnMsgNPCStopPlayAction(const ECMSG& Msg)
{
	CheckStopPolicyAction();
}

void CECNPC::OnMsgNPCMultiObjectEffect(const ECMSG &Msg)
{
	using namespace S2C;
	ASSERT(Msg.dwParam2 == ADD_MULTIOBJECT_EFFECT || Msg.dwParam2 == REMOVE_MULTIOBJECT_EFFECT);
	cmd_multiobj_effect *pCmd = (cmd_multiobj_effect *)Msg.dwParam1;

	if (Msg.dwParam2 == ADD_MULTIOBJECT_EFFECT)
	{
		AddMultiObjectEffect(pCmd->target,pCmd->type);
	}
	else
	{
		RemoveMultiObjectEffect(pCmd->target,pCmd->type);
	}
}

void CECNPC::CheckStopPolicyAction()
{
	if (!IsInPolicyAction())	return;

	WorkFinished(WORK_POLICYACTION);

	ASSERT(m_pPolicyAction == NULL);
	if (m_pPolicyAction)
	{
		m_pNPCModelPolicy->StopChannelAction();
		delete m_pPolicyAction;
		m_pPolicyAction = NULL;
		m_nPolicyActionIntervalTimer = 0;
	}
}

//	Render titles / names / talks above player's header
bool CECNPC::RenderName(CECViewport* pViewport, DWORD dwFlags)
{
	if (!FillPateContent(pViewport))
		return true;

	CECGameRun *pGameRun = g_pGame->GetGameRun();
	CECHostPlayer* pHost = pGameRun->GetHostPlayer();
	AUIManager* pUIMan = pGameRun->GetUIManager()->GetCurrentUIManPtr();
	float fScale = CECPateText::GetRenderScale();

	//	Draw name string
	bool bDrawName = true;
	if (IsPetNPC())
	{
		//	Pet's name showing flag follows it's master
		if (GetMasterID() == pHost->GetCharacterID())
			bDrawName = g_pGame->GetConfigs()->GetVideoSettings().bPlayerSelfName;
		else
			bDrawName = (g_pGame->GetConfigs()->GetPlayerTextFlags() & CECPlayer::RNF_NAME) ? true : false;
	}
	else if (IsServerNPC())
		bDrawName = (dwFlags & RNF_NPCNAME) ? true : false;
	else if (IsMonsterNPC())
		bDrawName = (dwFlags & RNF_MONSTERNAME) ? true : false;

	int x, y=m_PateContent.iCurY, cx, cy;

	if (m_pPateName && bDrawName)
	{
		m_pPateName->GetExtent(&cx, &cy);
		cx = int(cx * fScale);
		cy = int(cy * fScale);
		x = m_PateContent.iBaseX - (cx >> 1);
		y -= cy + 2;

		DWORD dwNameCol = GetNameColor();
		m_pPateName->RegisterRender(x, y, dwNameCol, m_PateContent.z);

		y -= 2;
	}
	
	//	Draw last said words
	if (dwFlags & RNF_WORDS)
	{
		DWORD dwCol = A3DCOLORRGB(255, 255, 255);

		//	Draw last said words, line 2 first
		if (m_pPateLastWords2 && m_pPateLastWords2->GetItemNum())
		{
			m_pPateLastWords1->GetExtent(&cx, &cy);
			cx = int(cx * fScale);
			cy = int(cy * fScale);
			y -= cy + 2;
			x = m_PateContent.iBaseX - (cx >> 1);
			m_pPateLastWords2->RegisterRender(x, y, dwCol, m_PateContent.z);
		}

		//	Last said words, line 1
		if (m_pPateLastWords1 && m_pPateLastWords1->GetItemNum())
		{
			m_pPateLastWords1->GetExtent(&cx, &cy);
			cx = int(cx * fScale);
			cy = int(cy * fScale);
			y -= cy + 2;
			x = m_PateContent.iBaseX - (cx >> 1);
			m_pPateLastWords1->RegisterRender(x, y, dwCol, m_PateContent.z);
		}
	}

	m_PateContent.iCurY	= y;

	return true;
}

//	Render bar above player's header
bool CECNPC::RenderBars(CECViewport* pViewport)
{
	if (!FillPateContent(pViewport))
		return true;

	int x, y, cx=100, cy=3;
	A3DFlatCollector* pFlat = g_pGame->GetA3DEngine()->GetA3DFlatCollector();
	CECConfigs* pConfigs = g_pGame->GetConfigs();
	DWORD colEmpty = A3DCOLORRGB(234, 159, 90);

	x = m_PateContent.iBaseX - (cx >> 1);
	y = m_PateContent.iCurY - 3;

	//	Draw HP bar
	if (pConfigs->GetVideoSettings().bPlayerHP)
	{
		if (m_ExtProps.bs.max_hp && m_BasicProps.iCurHP)
		{
			int iLen = (int)ceil(cx * ((float)m_BasicProps.iCurHP / m_ExtProps.bs.max_hp));
			a_Clamp(iLen, 1, cx);
			pFlat->AddRect_2D(x, y, x+cx-1, y+cy-1, colEmpty);
			pFlat->AddRect_2D(x, y, x+iLen-1, y+cy-1, A3DCOLORRGB(255, 57, 62));
		}

		y -= cy + 1;
	}

	m_PateContent.iCurY	= y;

	return true;
}

//	Set new visible extend states
void CECNPC::SetNewExtendStates(int start, DWORD* pData, int count)
{
	if(!pData || start + count > OBJECT_EXT_STATE_COUNT)
	{
		ASSERT(false);
		a_LogOutput(1, "CECNPC::SetNewExtendStates, invalid data(start=%d, count=%d)", start, count);
		return;
	}
	ShowExtendStates(start, pData, count);
	memcpy(m_aExtStates + start, pData, sizeof(DWORD) * count );
}

void CECNPC::ClearShowExtendStates()
{	
	DWORD aExtStates[OBJECT_EXT_STATE_COUNT] = {0};
	ShowExtendStates(0, aExtStates, OBJECT_EXT_STATE_COUNT, true);
}

void CECNPC::OptimizeShowExtendStates()
{
	m_pNPCModelPolicy->OptimizeShowExtendStates();
}

void CECNPC::OptimizeWeaponStoneGfx()
{
	m_pNPCModelPolicy->OptimizeWeaponStoneGfx();
}

void CECNPC::OptimizeArmorStoneGfx()
{
	m_pNPCModelPolicy->OptimizeArmorStoneGfx();
}

void CECNPC::OptimizeSuiteGfx()
{
	m_pNPCModelPolicy->OptimizeSuiteGfx();
}

void CECNPC::ShowExtendStates(int start, DWORD* pData, int count, bool bIgnoreOptimize/* =false */)
{
	if(!pData || start + count > OBJECT_EXT_STATE_COUNT)
	{
		ASSERT(false);
		return;
	}
	if (!m_pNPCModelPolicy->IsModelLoaded())
		return;
	if (!bIgnoreOptimize && !CECOptimize::Instance().GetGFX().CanShowState(GetNPCID(), GetClassID()))
		return;

	static const char* szBasePath = "策划联入\\状态效果\\";
	AString strGFXFile;

	const int bitSize = sizeof(DWORD) * 8;
	for(int index = 0; index<count; index++)
	{
		int idState = index + start;
		for (int i=0; i < bitSize; i++)
		{
			DWORD dwMask = 1 << i;

			DWORD dwFlag1 = m_aExtStatesShown[idState] & dwMask;
			DWORD dwFlag2 = pData[index] & dwMask;

			if ((!dwFlag1 && !dwFlag2) || (dwFlag1 && dwFlag2))
				continue;

			const GNET::VisibleState* pvs = GNET::VisibleState::Query(127, i + idState*32);
			if (!pvs)
				continue;

			strGFXFile = pvs->GetEffect();
			if (!strGFXFile.GetLength())
				continue;

			strGFXFile = szBasePath + strGFXFile;

			if (dwFlag1)
			{
				//	Remove old state
				m_pNPCModelPolicy->RemoveGfx(strGFXFile, pvs->GetHH());
			}
			else
			{
				//	Add new state
				m_pNPCModelPolicy->PlayGfx(strGFXFile, pvs->GetHH());
			}
		}
	}
	
	memcpy(m_aExtStatesShown + start, pData, sizeof(DWORD) * count );
}

//	Fill pate content
bool CECNPC::FillPateContent(CECViewport* pViewport)
{
	//	If m_PateContent.iVisible != 0, pate content has been filled
	//	for this frame
	if (m_PateContent.iVisible)
		return m_PateContent.iVisible == 2 ? true : false;

	A3DVIEWPORTPARAM* pViewPara = pViewport->GetA3DViewport()->GetParam();

	//	Calculate basic center position on screen
	const A3DAABB& aabb = GetPickAABB();
	A3DVECTOR3 vPos = GetPos();
	vPos.y = aabb.Center.y + aabb.Extents.y * 1.15f;
	A3DVECTOR3 vScrPos;
	pViewport->GetA3DViewport()->Transform(vPos, vScrPos);
	if (vScrPos.z < pViewPara->MinZ || vScrPos.z > pViewPara->MaxZ)
	{
		m_PateContent.iVisible = 1;		//	In-visible
		return false;
	}

	m_PateContent.iVisible	= 2;
	m_PateContent.iBaseX	= (int)vScrPos.x;
	m_PateContent.iBaseY	= (int)vScrPos.y;
	m_PateContent.iCurY		= m_PateContent.iBaseY;
	m_PateContent.z			= vScrPos.z;

	return true;
}

//	Set NPC's transparency
bool CECNPC::StartAdjustTransparency(float fCur, float fDest, DWORD dwTime)
{
	// use current value for starting
	if (fCur < 0.0f)
	{
		if (!m_pNPCModelPolicy->GetTransparent(fCur)){
			fCur = m_fCurTrans;
		}
	}

	if(fDest < 0.f) fDest = 0.f;

	// ignore the invalid params
	if(dwTime == 0 || fabs(fDest - m_fDstTrans) < 0.0001f || fabs(fDest - fCur) < 0.0001f)
		return false;

	m_fCurTrans		= fCur;
	m_fDstTrans		= fDest;
	m_fTransDelta	= (fDest - m_fCurTrans) / dwTime;

	m_TransCnt.SetPeriod(dwTime);
	m_TransCnt.Reset();

	return true;
}

float CECNPC::UpdateTransparency(DWORD dwDeltaTime)
{
	//	Change model's transparency
	if (!m_TransCnt.IsFull() && m_fDstTrans != m_fCurTrans)
	{
		m_fCurTrans += m_fTransDelta * dwDeltaTime;
		if (m_fTransDelta > 0.0f)
			a_ClampRoof(m_fCurTrans, m_fDstTrans);
		else
			a_ClampFloor(m_fCurTrans, m_fDstTrans);
	}

	float fTrans = (m_fCurTrans <= 0.f) ? -1.0f : m_fCurTrans;
	m_pNPCModelPolicy->SetTransparent(fTrans);
	return fTrans;
}

bool CECNPC::RayTrace(ECRAYTRACE* pTraceInfo)
{
	bool bRet = false;

	if(m_pNPCModelPolicy->RayTraceModelConvexHull(pTraceInfo)){
		bRet = true;
	}else{
		const A3DAABB& aabb = GetPickAABB();
		A3DVECTOR3 vHitPos, vNormal;
		float fFraction;
		if (CLS_RayToAABB3(pTraceInfo->vStart, pTraceInfo->vDelta, aabb.Mins, aabb.Maxs, vHitPos, &fFraction, vNormal))
		{
			if (fFraction < pTraceInfo->pTraceRt->fFraction)
			{
				pTraceInfo->pTraceRt->fFraction		= fFraction;
				pTraceInfo->pTraceRt->vHitPos		= vHitPos;
				pTraceInfo->pTraceRt->vPoint		= vHitPos;
				pTraceInfo->pTraceRt->vNormal		= vNormal;

				pTraceInfo->pECTraceRt->fFraction	= fFraction;
				pTraceInfo->pECTraceRt->iEntity		= ECENT_NPC;
				pTraceInfo->pECTraceRt->iObjectID	= GetNPCID();
				pTraceInfo->pECTraceRt->vNormal		= vNormal;

				bRet = true;
			}
		}
	}

	return bRet;
}

bool CECNPC::GetCHAABB(A3DAABB &aabb)const
{
	return m_pNPCModelPolicy->GetCHAABB(aabb);
}

bool CECNPC::TraceWithBrush(BrushTraceInfo * pInfo)
{
	return m_pNPCModelPolicy->TraceWithBrush(pInfo);
}

void CECNPC::RebuildTraceBrush()
{
	if (IsMonsterNPC())
	{
		int role_in_war = ((CECMonster *)this)->GetDBEssence()->role_in_war;
		if( role_in_war == 2 || role_in_war == 3 || role_in_war == 5 )
		{
			// we need collision detection for large building monster in war
		}
		else
			return;
	}
	else if (IsPetNPC())
	{
		return;
	}
	
	ReleaseTraceBrush();
	//	服务型 NPC，如果需要忽略此 NPC 碰撞（比如属于移动型 NPC，服务器端不做检查），则直接返回，不生成碰撞检测数据
	if (IsServerNPC() && ((CECNPCServer *)this)->GetDBEssence()->no_collision)
		return;
	m_pNPCModelPolicy->RebuildTraceBrush();
}

void CECNPC::ReleaseTraceBrush()
{
	m_pNPCModelPolicy->ReleaseTraceBrush();
}

void CECNPC::NPCTurnFaceTo(int idTarget, DWORD dwTime)
{
	if (IsDirFixed()){
		return;
	}

	// tower in war can not turn face to.
	if (IsMonsterNPC())
	{
		int role_in_war = ((CECMonster *)this)->GetDBEssence()->role_in_war;
		if( role_in_war == 2 || role_in_war == 5 )
			return;
	}

	TurnFaceTo(idTarget, dwTime);
}

//	Print bubble text
void CECNPC::BubbleText(int iIndex, DWORD dwNum, int p1/* 0 */)
{
	if (!m_pBubbleTexts)
		return;

	DWORD dwCol = A3DCOLORRGB(237, 56, 0);
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (GetMasterID() == pHost->GetCharacterID())
		dwCol = A3DCOLORRGB(255, 205, 75);

	const A3DAABB& aabb = GetPickAABB();
	A3DVECTOR3 vPos = aabb.Center + g_vAxisY * (aabb.Extents.y * 1.5f);
	
	CECBubbleDecal* pBubbleDecal = m_pBubbleTexts->AddDecal(vPos, CECDecal::DCID_ICONDECAL);
	CECIconDecal* pDecal = (CECIconDecal*)pBubbleDecal->GetDecal();

	switch (iIndex)
	{
	case BUBBLE_DAMAGE:

		if (p1 & 0x0001)
			pDecal->AddIcon(CECImageRes::IMG_DEADLYSTRIKE, 0, dwCol);
		else if (p1 & 0x0002)
			pDecal->AddIcon(CECImageRes::IMG_RETORT, 0, dwCol);

		pDecal->AddNumIcons(CECImageRes::IMG_POPUPNUM, dwNum, dwCol);
		break;

	case BUBBLE_HITMISSED:
		
		pDecal->AddIcon(CECImageRes::IMG_HITMISSED, 0, dwCol);
		break;

	case BUBBLE_INVALIDHIT:

		pDecal->AddIcon(CECImageRes::IMG_INVALIDHIT, 0, dwCol);
		break;

	case BUBBLE_IMMUNE:
		
		pDecal->AddIcon(CECImageRes::IMG_IMMUNE, 0, dwCol);
		break;

	case BUBBLE_HPWARN:

		dwCol = A3DCOLORRGB(255, 255, 255);
		pDecal->AddIcon(CECImageRes::IMG_HPWARN, 0, dwCol);
		break;

	case BUBBLE_LOSE:

		pDecal->AddIcon(CECImageRes::IMG_ATTACKLOSE, 0, dwCol);
		break;

	case BUBBLE_SUCCESS:

		pDecal->AddIcon(CECImageRes::IMG_SUCCESS, 0, dwCol);
		break;

	case BUBBLE_REBOUND:

		pDecal->AddIcon(CECImageRes::IMG_REBOUND, 0, dwCol);
		pDecal->AddNumIcons(CECImageRes::IMG_POPUPNUM, dwNum, dwCol);
		break;

	case BUBBLE_BEAT_BACK:

		pDecal->AddIcon(CECImageRes::IMG_BEAT_BACK, 0, dwCol);
		pDecal->AddNumIcons(CECImageRes::IMG_POPUPNUM, dwNum, dwCol);
		break;
		
	case BUBBLE_DODGE_DEBUFF:
		
		pDecal->AddIcon(CECImageRes::IMG_DODGE_DEBUFF, 0, dwCol);
		break;

	default:
		return;
	}
}

void CECNPC::OnPetSays(int type)
{
	if (IsPetNPC())
	{
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		pHost->OnPetSays(GetTemplateID(), GetNPCID(), type);
	}
}

DWORD MakeNPCRenderCameraParam(int nRotateAngle, int nMove)
{
	return MAKELONG((short)nRotateAngle, (short)nMove);
}

void  GetNPCRenderCameraParam(const DWORD dwCameraParam, float &vDeg, float &fMoveRatio)
{
	int nRotateAngle = (short)LOWORD(dwCameraParam);
	vDeg = DEG2RAD(nRotateAngle);
	int nMove = (short)HIWORD(dwCameraParam);
	fMoveRatio = nMove/100.0f;
}

void NPCRenderDemonstration(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3)
{
	if( param1 == 0 || param3 == 0)
		return;

	CECModel * pNPCModel = (CECModel *) param1;
	float vDeg(0.0f), fMoveRatio(0.0f);
	GetNPCRenderCameraParam(param2, vDeg, fMoveRatio);

	const A3DAABB &aabb = *(A3DAABB *)param3;

	A3DViewport * pCurViewport = g_pGame->GetA3DEngine()->GetActiveViewport();

	A3DViewport * pPortraitViewport = g_pGame->GetPortraitRender()->GetPortraitViewport();
	A3DCamera * pCamera = (A3DCamera *)(pPortraitViewport->GetCamera());
	float fovBak(0.0f), frontBak(0.0f), backBak(0.0f), ratioBak(0.0f);
	pCamera->GetProjectionParam(&fovBak, &frontBak, &backBak, &ratioBak);

	A3DVIEWPORTPARAM viewParam = *pPortraitViewport->GetParam();
	viewParam.X = rc.left;
	viewParam.Y = rc.top;
	viewParam.Width = rc.right - rc.left;
	viewParam.Height = rc.bottom - rc.top;
	pPortraitViewport->SetParam(&viewParam);

	A3DMATRIX4 mat = a3d_RotateY(vDeg);

	float ext = a_Max(aabb.Extents.x, aabb.Extents.y, aabb.Extents.z);
	float dist = ext * 4.7f + 1.0f;
	float height = aabb.Extents.y*2.0f;
	
	fMoveRatio *= 3.5f;		//	提高移动范围
	a_ClampRoof(fMoveRatio, 4.7f);		//	相机远近不能移到中心另一边，实际应为 deltaZ < dist => fMoveRatio < 4.7+1/ext
	float deltaZ = ext*fMoveRatio;
	float deltaY = height/dist*deltaZ;	//	按原视线移近/远相机，达到放大/缩小效果

	A3DVECTOR3 vecPos = A3DVECTOR3(0, 0, -dist-deltaZ) * mat + aabb.Center + A3DVECTOR3(0, height+deltaY, 0);
	A3DVECTOR3 vecDir = Normalize(aabb.Center - vecPos);
	A3DVECTOR3 vecUp = A3DVECTOR3(0, 1.0f, 0);

	pCamera->SetProjectionParam(-1.0f, -1.0f, 100.0f, ((float)viewParam.Width) / viewParam.Height);
	pCamera->SetPos(vecPos);
	pCamera->SetDirAndUp(vecDir, vecUp);
	pPortraitViewport->Active();
	pPortraitViewport->ClearDevice();

	A3DSkinModel::LIGHTINFO LightInfo;
	memset(&LightInfo, 0, sizeof(LightInfo));

	const A3DLIGHTPARAM& lp = g_pGame->GetDirLight()->GetLightparam();
	LightInfo.colDirDiff	= lp.Diffuse;
	LightInfo.colDirSpec	= lp.Specular;
	LightInfo.vLightDir		= lp.Direction;
	LightInfo.colAmbient	= A3DCOLORVALUE(0.5f, 0.5f, 0.5f, 1.0f) + g_pGame->GetA3DDevice()->GetAmbientValue();
	LightInfo.colAmbient.r = min(LightInfo.colAmbient.r, 1.0f);
	LightInfo.colAmbient.g = min(LightInfo.colAmbient.g, 1.0f);
	LightInfo.colAmbient.b = min(LightInfo.colAmbient.b, 1.0f);
	LightInfo.colAmbient.a = min(LightInfo.colAmbient.a, 1.0f);
	LightInfo.colPtDiff = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	LightInfo.colPtAmb = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	LightInfo.fPtRange = 0.01f;
	LightInfo.vPtAtten = A3DVECTOR3(0.0f);
	LightInfo.vPtLightPos = A3DVECTOR3(0.0f);
	LightInfo.bPtLight = true;
	if (pNPCModel->GetA3DSkinModel())
		pNPCModel->GetA3DSkinModel()->SetLightInfo(LightInfo);

	//	开始渲染

	//	设置图元收集对象
	A3DEngine* pA3DEngine = g_pGame->GetA3DEngine();
	A3DSkinRender* psr = g_pGame->GetSkinRender2();
	A3DSkinRender* psrold = pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
	pA3DEngine->GetA3DSkinMan()->SetCurSkinRender(psr);

	//	渲染 非Alpha 物体
	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(false);
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(true);
	g_pGame->GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATER);
	g_pGame->GetA3DDevice()->SetAlphaRef(84);

	//	CECModel::Render 中不更新光照参数，以免试衣模型受环境光影响
	bool bUpdateFlagOld = pNPCModel->GetUpdateLightInfoFlag();
	pNPCModel->SetUpdateLightInfoFlag(false);
	pNPCModel->Render(pPortraitViewport);
	pNPCModel->SetUpdateLightInfoFlag(bUpdateFlagOld);
	
	psr->Render(pPortraitViewport, false);
	psr->ResetRenderInfo(false);

	//	渲染 Alpha 物体
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(false);
	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(true);

	psr->RenderAlpha(pPortraitViewport);
	psr->ResetRenderInfo(true);

	g_pGame->GetA3DGFXExMan()->RenderAllGfx(pPortraitViewport);
	
	if (g_pA3DConfig->RT_GetShowBoundBoxFlag()){
		g_pGame->GetA3DGFXExMan()->RenderAllGfxAABB();
		g_pGame->GetA3DEngine()->GetA3DWireCollector()->AddAABB(aabb, A3DCOLORRGBA(255, 0, 0, 255));
		g_pGame->GetA3DEngine()->GetA3DWireCollector()->Flush();
	}

	//	恢复原有设置
	pCamera->SetProjectionParam(-1.0f, -1.0f, backBak, -1.0f);
	pA3DEngine->GetA3DSkinMan()->SetCurSkinRender(psrold);
	pCurViewport->Active();
}

bool CECNPC::GetExtState(int n)
{
	const int bitSize = sizeof(DWORD) * 8;
	const int totalBitSize = bitSize * OBJECT_EXT_STATE_COUNT;
	if(n < 0 || n >= totalBitSize)
		return false;

	int index = n / bitSize;
	int bitOffset = n % bitSize;
	return (m_aExtStates[index] & (1 << bitOffset)) ? true : false;
}

float CECNPC::GetTransparentLimit()
{
	if (m_dwStates & GP_STATE_INVISIBLE)
	{
		return 0.7f;//隐身
	}
	else if (!IsSelectable())
	{
		return 0.5f;//无法选中
	}

	return -1.f;
}

void CECNPC::AddMultiObjectEffect(int idTarget,char cType)
{
	if (cType < 0 || cType > 2)
		return;

	static const char* szBasePath = "策划联入\\状态效果\\";

	MULTIOBJECT_EFFECT mo;
	mo.cType = cType;
	mo.iTarget = idTarget;

	MOEffectMAP::iterator it = m_mapMOEffect.find(mo);
	if (it == m_mapMOEffect.end())
	{
		AString strGfxFile(_multiobject_effect[cType]);
		strGfxFile = szBasePath + strGfxFile;
		A3DGFXExMan *pGFXExMan = g_pGame->GetA3DGFXExMan();
		A3DGFXEx* pGfx = pGFXExMan->LoadGfx(g_pGame->GetA3DDevice(), strGfxFile);
		if (!pGfx)
			return;
		pGfx->SetScale(1.0f);
		pGfx->SetVisible(false);

		if (m_pNPCModelPolicy->IsModelLoaded())
			UpdateOneMultiObjectEffect(idTarget,pGfx,0);

		m_mapMOEffect[mo] = pGfx;
	}
}

void CECNPC::RemoveMultiObjectEffect(int idTarget,char cType)
{
	MULTIOBJECT_EFFECT mo;
	mo.cType = cType;
	mo.iTarget = idTarget;

	MOEffectMAP::iterator it = m_mapMOEffect.find(mo);
	if (it != m_mapMOEffect.end())
	{
		A3DGFXExMan *pGFXExMan = g_pGame->GetA3DGFXExMan();
		pGFXExMan->CacheReleasedGfx(it->second);

		m_mapMOEffect.erase(mo);
	}
}

void CECNPC::UpdateOneMultiObjectEffect(int idTarget,A3DGFXEx* pGfx,DWORD dwDeltaTime)
{
	if (idTarget == 0 || pGfx == NULL)
		return;

	CECObject* pObject = NULL;

	if (ISNPCID(idTarget))
	{
		if (!(pObject = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(idTarget)))
			return;
	}
	else if (ISPLAYERID(idTarget))
	{
		if (!(pObject = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(idTarget)))
			return;
	}
	else
		return;

	A3DVECTOR3 pos[2];
	const A3DAABB &playerAABB = GetPickAABB();
	pos[0] = playerAABB.Center;
	const A3DAABB &targetAABB = pObject->IsNPC() ? ((CECNPC*)pObject)->GetPickAABB() : ((CECPlayer*)pObject)->GetPlayerAABB();
	pos[1] = targetAABB.Center;
	A3DVECTOR3 delta = pos[0]-pos[1];
	float dist2 = delta.SquaredMagnitude();
	if (dist2 > 2025.0f)
	{
		if (pGfx->IsVisible())
			pGfx->SetVisible(false);
		return;
	}

	if (pGfx->GetState() != ST_PLAY)
		pGfx->Start(true);
	if (!pGfx->IsVisible())
		pGfx->SetVisible(true);

	int nElement = pGfx->GetElementCount();
	bool ret;
	for (int i = 0; i < nElement; ++ i)
	{
		A3DGFXElement *pElement = pGfx->GetElement(i);
		if (pElement)
		{
			ret = GFX_UpdateLightingEdgePos(pGfx, pElement->GetName(), pos);
		}
	}
	pGfx->TickAnimation(dwDeltaTime);
}

void CECNPC::UpdateMultiObjectEffect(DWORD dwDeltaTime)
{
	for (MOEffectMAP::iterator it = m_mapMOEffect.begin();it != m_mapMOEffect.end();++it)
	{
		UpdateOneMultiObjectEffect(it->first.iTarget,it->second,dwDeltaTime);
	}
}

void CECNPC::RenderMultiObjectGfx()
{
	A3DGFXExMan *pGFXExMan = g_pGame->GetA3DGFXExMan();

	for (MOEffectMAP::iterator it = m_mapMOEffect.begin();it != m_mapMOEffect.end();++it)
	{
		if (it->second && it->second->IsVisible())
		{
			pGFXExMan->RegisterGfx(it->second);
		}
	}
}

bool CECNPC::IsLag(float fDist)
{
	return m_iPassiveMove == 0 && fDist > MAX_LAGDIST;
}

bool CECNPC::IsFactionPVPMineCar()const
{ 
	if (const MONSTER_ESSENCE *pMonsterEssence = GetMonsterEssence()){
		return (pMonsterEssence->faction & (1 << 19)) != 0;
	}
	return false; 
}

bool CECNPC::IsFactionPVPMineBase()const
{ 
	if (const MONSTER_ESSENCE *pMonsterEssence = GetMonsterEssence()){
		return (pMonsterEssence->faction & (1 << 20)) != 0;
	}
	return false; 
}

bool CECNPC::GetModelFile(int tid, const char*& szModelFile)
{
	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	const void* pDBEssence = pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	if(!pDBEssence) return false;

	bool ret = true;
	if (DataType == DT_MONSTER_ESSENCE)
	{
		MONSTER_ESSENCE* pESS = (MONSTER_ESSENCE*)pDBEssence;
		szModelFile = pESS->file_model;
	}
	else if (DataType == DT_PET_ESSENCE)
	{
		PET_ESSENCE* pESS = (PET_ESSENCE*)pDBEssence;
		szModelFile = pESS->file_model;
	}
	else if (DataType == DT_NPC_ESSENCE)
	{
		NPC_ESSENCE* pESS = (NPC_ESSENCE*)pDBEssence;
		szModelFile = pESS->file_model;
	}
	else ret = false;

	return ret;
}

void CECNPC::TransformShape(int vis_tid)
{
	if (m_NPCInfo.vis_tid == vis_tid){
		return;
	}
	m_NPCInfo.vis_tid = vis_tid;
	QueueLoadNPCModel();
}

bool CECNPC::IsClickNamePateText( int x, int y )
{
	if (m_bRenderName){
		return m_pPateName->IsMouseInRect(x,y);
	}
	return false;
}

void CECNPC::SetMouseOnNameFlag( bool IsHover )
{
	m_pPateName->SetIsHover(IsHover);
}

bool CECNPC::HasModel()const{
	return m_pNPCModelPolicy->IsModelLoaded();
}

void CECNPC::ClearComActFlag(bool bSignalCurrent){
	m_pNPCModelPolicy->ClearComActFlag(bSignalCurrent);
}

bool CECNPC::GetEcmProperty(ECMODEL_GFX_PROPERTY* pProperty)const{
	return m_pNPCModelPolicy->GetEcmProperty(pProperty);
}

A3DSkinModel * CECNPC::GetSgcSkinModel(const char *szHanger, bool bChildHook, const char *szHook){
	return m_pNPCModelPolicy->GetSgcSkinModel(szHanger, bChildHook, szHook);
}

A3DSkeletonHook * CECNPC::GetSgcHook(const char *szHanger, bool bChildHook, const char *szHook){
	return m_pNPCModelPolicy->GetSgcHook(szHanger, bChildHook, szHook);
}

bool CECNPC::ShouldPlayNewActionFor(int iMoveMode){
	if (m_pNPCModelPolicy->IsPlayingAction()){
		int iAction = GetMoveAction(iMoveMode);
		return !m_pNPCModelPolicy->IsPlayingAction(iAction)
			&& m_pNPCModelPolicy->HasAction(iAction);
	}
	return false;
}

bool CECNPC::GetVisibleModel(int &tid, const char*& szModelFile)const{
	tid = 0;
	szModelFile = "";
	if (GetModelFile(GetNPCInfo().vis_tid, szModelFile)){
		tid = GetNPCInfo().vis_tid;
	}else if (GetModelFile(tid, szModelFile)){
		tid = GetNPCInfo().tid;
	}
	return tid > 0;
}

bool CECNPC::ShouldUseMasterModel()const{
	int tid(0);
	const char *szModelFile = "";
	if (!GetVisibleModel(tid, szModelFile)){
		return false;
	}
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType(DT_INVALID);;
	const void* pDBEssence = pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	if (DataType != DT_PET_ESSENCE){
		return false;
	}
	const PET_ESSENCE* pEssence = (const PET_ESSENCE*)pDBEssence;
	return (pEssence->combined_switch & PCS_USE_MASTER_MODEL) != 0;
}

bool CECNPC::IsUsingMasterModel()const{
	return m_pNPCModelPolicy != NULL && dynamic_cast<const CECNPCModelClonePlayerPolicy *>(m_pNPCModelPolicy) != NULL;
}

CECPlayer * CECNPC::GetMaster()const{
	return GetMasterID() == 0 ? NULL : g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(GetMasterID());
}

const ACHAR* CECNPC::GetNameToShow()const{
	return m_pNPCModelPolicy ? m_pNPCModelPolicy->GetNameToShow() : m_strName;
}

CECPlayer * CECNPC::GetClonedMaster(){
	if (CECNPCModelClonePlayerPolicy *pClonePlayerPolicy = dynamic_cast<CECNPCModelClonePlayerPolicy*>(m_pNPCModelPolicy)){
		return (CECPlayer *)(pClonePlayerPolicy->m_pPlayer);
	}
	return NULL;
}
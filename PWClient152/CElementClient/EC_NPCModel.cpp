// Filename	: EC_NPCModel.cpp
// Creator	: Xu Wenbin
// Date		: 2014/8/20

#include "EC_NPCModel.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_SceneLoader.h"
#include "EC_NPC.h"
#include "EC_ManNPC.h"
#include "EC_Monster.h"
#include "EC_Configs.h"
#include "EC_Viewport.h"
#include "EC_ShadowRender.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_ManAttacks.h"
#include "EC_Pet.h"
#include "EC_PlayerClone.h"

#include "ExpTypes.h"
#include "ElementSkill.h"

#include "AABBCD.h"
#include "A3DSkillGfxEvent2.h"
#include "EC_Model.h"
#include <A3DFuncs.h>
#include <A3DCollision.h>
#include <A3DCombinedAction.h>

using namespace CHBasedCD;

//	class CECNPCModelDefaultPolicy
CECNPCModelDefaultPolicy::CECNPCModelDefaultPolicy(CECNPC *pNPC)
: m_pNPC(pNPC)
, m_pNPCModel(NULL)
, m_pNPCSkin(NULL)
, m_pNPCSkinLow(NULL)
, m_pCurSkin(NULL)
, m_nBrushes(0)
, m_ppBrushes(NULL)
{
	m_CHAABB.Clear();	
	m_aabbDefPick.Extents.Set(0.3f, 0.85f, 0.3f);
}

CECNPCModelDefaultPolicy::~CECNPCModelDefaultPolicy(){
	Release();
}

void CECNPCModelDefaultPolicy::Release(){
	ReleaseTraceBrush();
	ReleaseModel();
}

void CECNPCModelDefaultPolicy::ReleaseModel(){	
	if (m_pNPCModel){
		QueueECModelForRelease(m_pNPCModel);
		m_pNPCModel = NULL;
	}	
	if (m_pNPCSkin){
		g_pGame->ReleaseA3DSkin(m_pNPCSkin, true);
		m_pNPCSkin = NULL;
	}	
	if (m_pNPCSkinLow){
		g_pGame->ReleaseA3DSkin(m_pNPCSkinLow, true);
		m_pNPCSkinLow = NULL;
	}
}

void CECNPCModelDefaultPolicy::ReleaseTraceBrush()
{
	if (!m_ppBrushes){
		return;
	}
	for (int i=0; i<m_nBrushes; i++){
		if (m_ppBrushes[i] ){
			delete m_ppBrushes[i];
			m_ppBrushes[i] = NULL;
		}
	}		
	delete [] m_ppBrushes;
	m_ppBrushes = NULL;
	m_nBrushes = 0;
	m_CHAABB.Clear();
}

void CECNPCModelDefaultPolicy::RebuildTraceBrush()
{
	ReleaseTraceBrush();	
	if (m_pNPCModel && m_pNPCModel->HasCHAABB()){		
		ConvexHullDataArray& cdArray = m_pNPCModel->GetConvexHullData();
		A3DMATRIX4 matTM = TransformMatrix(m_pNPC->GetDir(), m_pNPC->GetUp(), m_pNPC->GetPos());
		
		m_nBrushes = cdArray.size();
		m_ppBrushes = new CCDBrush *[m_nBrushes];
		m_CHAABB.Clear();
		
		for(int i=0; i<m_nBrushes; i++){
			CConvexHullData chData = *cdArray[i];
			chData.Transform(matTM);
			
			CQBrush qBrush;
			qBrush.AddBrushBevels(&chData);
			CCDBrush * pCDBrush = new CCDBrush();
			qBrush.Export(pCDBrush);
			
			m_ppBrushes[i] = pCDBrush;
			
			m_CHAABB.Merge(pCDBrush->GetAABB());
		}
	}
}

void CECNPCModelDefaultPolicy::SetNPCLoadedResult(const EC_NPCLOADRESULT& Ret){	
	if (m_pNPCModel)
	{
		//	This case may happen like below way:
		//	- first, NPC(n) is created by NPC manager, it send resource loading
		//	  request to loading thread.
		//	- Immediately, for some reason, server tell NPC manager to delete NPC(n),
		//	  but in this time resources hasn't been loaded by loading thread.
		//	- Immediately, for some other reason, server create NPC(n) again, we
		//	  send another resource loading request.
		//	- the first resource loading request is responsed and model loaded
		//	- the second resource loading request is responsed.
		Release();
	}
	
	m_pNPCModel		= Ret.pModel;
	m_pNPCSkin		= Ret.pSkin;
	m_pNPCSkinLow	= Ret.pLowSkin;
	m_pCurSkin		= NULL;
	
	if (m_pNPCModel && m_pNPC->IsServerNPC())
		m_pNPCModel->GetA3DSkinModel()->ReplaceSkin(0, m_pNPCSkin, false);
	
	m_pNPCModel->SetPos(m_pNPC->GetPos());
	m_pNPCModel->SetDirAndUp(m_pNPC->GetDir(), m_pNPC->GetUp());
	
	// update the transparency
	m_pNPCModel->SetTransparent(m_pNPC->GetTransparentLimit());
	
	//	Force to update model once completely to avoid rendering error
	m_pNPCModel->SetAutoUpdateFlag(false);
	m_pNPCModel->Tick(1);
	m_pNPCModel->SetAutoUpdateFlag(true);
}

bool CECNPCModelDefaultPolicy::HasAction(int iAction)const{
	if (!m_pNPCModel){
		return false;
	}
	const char *szAct = GetActionName(iAction);
	return m_pNPCModel->GetComActByName(szAct) != NULL;
}

const A3DAABB & CECNPCModelDefaultPolicy::GetPickAABB(){	
	if (!m_pNPCModel || !m_pNPCModel->GetA3DSkinModel()){
		m_aabbDefPick.Center = m_pNPC->GetPos() + g_vAxisY * m_aabbDefPick.Extents.y;
		m_aabbDefPick.CompleteMinsMaxs();
		return m_aabbDefPick;
	}
	if (m_pNPCModel->HasCHAABB()){
		m_aabbDefPick.Center = m_pNPC->GetPos() + g_vAxisY * m_pNPCModel->GetCHAABB().Center.y;
		m_aabbDefPick.Extents = m_pNPCModel->GetCHAABB().Extents;
		m_aabbDefPick.CompleteMinsMaxs();
		return m_aabbDefPick;
	}	
	return m_pNPCModel->GetA3DSkinModel()->GetModelAABB();
}

void CECNPCModelDefaultPolicy::Tick(DWORD dwDeltaTime){
	if (!m_pNPCModel){
		return;
	}
	if (m_pNPC->IsMonsterOrPet()){		
		if( m_pNPCSkinLow && m_pNPCSkin )
		{
			if( m_pNPC->GetDistToCamera() < g_pGame->GetConfigs()->GetSceneLoadRadius() * 0.08f * m_pNPC->GetTouchRadius() )
			{
				if (m_pCurSkin != m_pNPCSkin)
				{
					m_pNPCModel->GetA3DSkinModel()->ReplaceSkin(0, m_pNPCSkin, false);
					m_pCurSkin = m_pNPCSkin;
				}
			}
			else
			{
				if (m_pCurSkin != m_pNPCSkinLow)
				{
					m_pNPCModel->GetA3DSkinModel()->ReplaceSkin(0, m_pNPCSkinLow, false);
					m_pCurSkin = m_pNPCSkinLow;
				}
			}
		}
		else if( m_pNPCSkin )
		{
			if (m_pCurSkin != m_pNPCSkin)
			{
				m_pNPCModel->GetA3DSkinModel()->ReplaceSkin(0, m_pNPCSkin, false);
				m_pCurSkin = m_pNPCSkin;
			}
		}
	}
	m_pNPCModel->SetPos(m_pNPC->GetPos());
	m_pNPCModel->Tick(dwDeltaTime);
}

bool CECNPCModelDefaultPolicy::IsModelLoaded()const{
	return m_pNPCModel != NULL;
}

void CECNPCModelDefaultPolicy::Render(CECViewport* pViewport, bool bHighlight){
	if (!IsModelLoaded()){
		return;
	}
	if (bHighlight){
		g_pGame->RenderHighLightModel(pViewport, m_pNPCModel);
	}else{
		m_pNPCModel->Render(pViewport->GetA3DViewport());
	}
}

void CECNPCModelDefaultPolicy::AddShadower(){
	if (!IsModelLoaded()){
		return;
	}
	const A3DAABB& aabb = GetPickAABB();
	g_pGame->GetShadowRender()->AddShadower(aabb.Center, aabb, SHADOW_RECEIVER_TERRAIN, NPCRenderForShadow, m_pNPCModel->GetA3DSkinModel());
}

void CECNPCModelDefaultPolicy::RenderPateContent(CECViewport* pViewport, bool bHighlight){
	
	//	Print NPC's name above header
	if (m_pNPC->GetRenderBarFlag()){
		m_pNPC->RenderBars(pViewport);
	}	
	if (m_pNPC->GetRenderNameFlag()){
		if (m_pNPC->IsServerNPC()){
			m_pNPC->SetMouseOnNameFlag(bHighlight);
		}
		m_pNPC->RenderName(pViewport, g_pGame->GetConfigs()->GetNPCTextFlags());
	}
}

void CECNPCModelDefaultPolicy::SetPos(const A3DVECTOR3& vPos){
	if (m_pNPCModel){
		m_pNPCModel->SetPos(vPos);
	}
}

void CECNPCModelDefaultPolicy::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp){
	if (m_pNPCModel){
		m_pNPCModel->SetDirAndUp(vDir, vUp);
	}
}

void CECNPCModelDefaultPolicy::ClearComActFlag(bool bSignalCurrent){
	if (m_pNPCModel){
		m_pNPCModel->ClearComActFlag(bSignalCurrent);
	}
}

void CECNPCModelDefaultPolicy::StopChannelAction(){
	if (m_pNPCModel){
		m_pNPCModel->StopChannelAction(0, true);
	}
}

void CECNPCModelDefaultPolicy::PlayActionByName(const char *szActName){
	if (m_pNPCModel){
		m_pNPCModel->PlayActionByName(szActName, 1.0f);
	}
}

bool CECNPCModelDefaultPolicy::HasComAct(const char *szActName){
	return m_pNPCModel != NULL && m_pNPCModel->GetComActByName(szActName) != NULL;
}

bool CECNPCModelDefaultPolicy::IsPlayingAction(){
	return m_pNPCModel != NULL && m_pNPCModel->GetCurComAct() != NULL;
}

bool CECNPCModelDefaultPolicy::IsPlayingAction(const char *szActName){
	return IsPlayingAction() && !stricmp(m_pNPCModel->GetCurComAct()->GetComAct()->GetName(), szActName);
}

bool CECNPCModelDefaultPolicy::IsPlayingAction(int iAction){
	if (CECNPC::IsAttackAction(iAction)){
		return IsPlayingAction(GetActionName(iAction, true))
			|| IsPlayingAction(GetActionName(iAction, false));
	}else{
		return IsPlayingAction(GetActionName(iAction));
	}
}

void CECNPCModelDefaultPolicy::PlayGfx(const char* szPath, const char* szHook){
	if (m_pNPCModel){
		m_pNPCModel->PlayGfx(szPath, szHook);
	}
}

void CECNPCModelDefaultPolicy::RemoveGfx(const char* szPath, const char* szHook){
	if (m_pNPCModel){
		m_pNPCModel->RemoveGfx(szPath, szHook);
	}
}

bool CECNPCModelDefaultPolicy::GetTransparent(float &fTransparent)const{	
	if (m_pNPCModel && m_pNPCModel->GetA3DSkinModel()){
		fTransparent = m_pNPCModel->GetA3DSkinModel()->GetTransparent();
		return true;
	}
	return false;
}

void CECNPCModelDefaultPolicy::SetTransparent(float fTransparent){
	if (m_pNPCModel && m_pNPCModel->GetA3DSkinModel())
		m_pNPCModel->GetA3DSkinModel()->SetTransparent(fTransparent);
}

bool CECNPCModelDefaultPolicy::HasCHAABB()const{
	return m_pNPCModel
		&& m_pNPCModel->HasCHAABB()
		&& m_nBrushes > 0;
}

bool CECNPCModelDefaultPolicy::GetCHAABB(A3DAABB &aabb)const
{
	bool bRet(false);
	if (HasCHAABB()){
		aabb = m_CHAABB;		
		//	m_CHAABB 根据凸包上的顶点位置来计算包围盒，更准确并与凸包检测保持一致
		//	不能使用 GetPos() 来调整 m_CHAABB 后作为计算结果返回
		//aabb.Center = GetPos() + A3DVECTOR3(0.0f, m_CHAABB.Extents.y, 0.0f);
		//aabb.CompleteMinsMaxs();		
		bRet = true;
	}
	return bRet;
}

bool CECNPCModelDefaultPolicy::HasModelCHAABB()const{
	return m_pNPCModel
		&& m_pNPCModel->HasCHAABB();
}

bool CECNPCModelDefaultPolicy::GetModelCHAABB(A3DAABB &aabb)const{
	if (HasModelCHAABB()){
		aabb = m_pNPCModel->GetCHAABB();
		return true;
	}
	return false;
}

const A3DAABB & CECNPCModelDefaultPolicy::GetDefaultPickAABB()const{
	return m_aabbDefPick;
}

void CECNPCModelDefaultPolicy::SetDefaultPickAABBExt(const A3DVECTOR3 &vExt){
	m_aabbDefPick.Extents = vExt;
}

bool CECNPCModelDefaultPolicy::RayTraceModelConvexHull(ECRAYTRACE* pTraceInfo)const{
	if(!HasModelCHAABB()){
		return false;
	}
	A3DMATRIX4 matToNPC = a3d_ViewMatrix(m_pNPC->GetPos(), m_pNPC->GetDir(), m_pNPC->GetUp(), 0.0f);
	A3DVECTOR3 vecStart = pTraceInfo->vStart * matToNPC;
	A3DVECTOR3 vecDelta = a3d_VectorMatrix3x3(pTraceInfo->vDelta, matToNPC);
	
	const A3DAABB& aabb = m_pNPCModel->GetCHAABB();	
	A3DVECTOR3 vHitPos, vNormal;
	float fFraction;
	if (CLS_RayToAABB3(vecStart, vecDelta, aabb.Mins, aabb.Maxs, vHitPos, &fFraction, vNormal)){
		CHBasedCD::ConvexHullDataArray & chArray = m_pNPCModel->GetConvexHullData();
		
		for(int i=0; i<int(chArray.size()); i++){
			CConvexHullData * pCH = chArray[i];
			const CFace * pFace;
			if( RayIntersectWithCH(vecStart, vecDelta, *pCH, &pFace, vHitPos, fFraction) )
			{
				if (fFraction < pTraceInfo->pTraceRt->fFraction)
				{
					vNormal = pFace->GetNormal();
					
					A3DMATRIX4 matNPC = m_pNPC->GetAbsoluteTM();
					vHitPos = vHitPos * matNPC;
					vNormal = a3d_VectorMatrix3x3(vNormal, matNPC);
					
					pTraceInfo->pTraceRt->fFraction		= fFraction;
					pTraceInfo->pTraceRt->vHitPos		= vHitPos;
					pTraceInfo->pTraceRt->vPoint		= vHitPos;
					pTraceInfo->pTraceRt->vNormal		= vNormal;
					
					pTraceInfo->pECTraceRt->fFraction	= fFraction;
					pTraceInfo->pECTraceRt->iEntity		= ECENT_NPC;
					pTraceInfo->pECTraceRt->iObjectID	= m_pNPC->GetNPCID();
					pTraceInfo->pECTraceRt->vNormal		= vNormal;
					return true;
				}
			}
		}
	}
	return false;
}

bool CECNPCModelDefaultPolicy::TraceWithBrush(BrushTraceInfo * pInfo)const{	
	if (!HasCHAABB())
		return false;
	
	BrushTraceInfo info = *pInfo;
	
	A3DAABB aabb = m_pNPCModel->GetCHAABB();
	aabb.Extents+=A3DVECTOR3(1.0f, 1.0f, 1.0f);
	aabb.Center += m_pNPC->GetPos();
	aabb.CompleteMinsMaxs();
	
	A3DVECTOR3	vPoint, vNormal;
	float		fFraction;		//	Fraction
	if (pInfo->bIsPoint && !CLS_RayToAABB3(pInfo->vStart, pInfo->vDelta, aabb.Mins, aabb.Maxs, vPoint, &fFraction, vNormal ) ) 
		return false;
	if (!pInfo->bIsPoint && !CLS_AABBAABBOverlap(aabb.Center, aabb.Extents, pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents) )
		return false;
	
	//save original result
	bool		bStartSolid = pInfo->bStartSolid;	//	Collide something at start point
	bool		bAllSolid = pInfo->bAllSolid;		//	All in something
	int			iClipPlane = pInfo->iClipPlane;		//	Clip plane's index
	
	fFraction = 100.0f;
	vNormal = pInfo->ClipPlane.GetNormal(); //clip plane normal
	float       fDist = pInfo->ClipPlane.GetDist();	//clip plane dist
	
	// Detect the collision with Convex Hull
	bool bCollide = false;
	for(int i=0;i<m_nBrushes;i++)
	{
		if (m_ppBrushes[i]->Trace(&info) && (info.fFraction < fFraction)) {
			//update the saving info
			bStartSolid = info.bStartSolid;
			bAllSolid = info.bAllSolid;
			iClipPlane = info.iClipPlane;
			fFraction = info.fFraction;
			vNormal = info.ClipPlane.GetNormal();
			fDist = info.ClipPlane.GetDist();
			
			bCollide = true;
		}
	}	
	
	//	if (bCollide && !bStartSolid)	//	2012-1-31: 忽略 bStartSolid=true 时的碰撞会导致瞬移技能计算失败，参照同期其它个体 TraceWithBrush 去掉
	if (bCollide)
	{
		*pInfo = info;
		pInfo->bStartSolid = bStartSolid;
		pInfo->bAllSolid = bAllSolid;
		pInfo->iClipPlane = iClipPlane;
		pInfo->fFraction = fFraction;
		pInfo->ClipPlane.SetNormal(vNormal);
		pInfo->ClipPlane.SetD(fDist);
	}
	return bCollide;
}

bool CECNPCModelDefaultPolicy::GetEcmProperty(ECMODEL_GFX_PROPERTY* pProperty)const{
	if (!m_pNPCModel){
		return false;
	}
	pProperty->bGfxUseLod = m_pNPCModel->IsGfxUseLOD();
	pProperty->bGfxDisableCamShake = m_pNPCModel->GetDisableCamShake();
	pProperty->bHostECMCreatedByGfx = m_pNPCModel->IsCreatedByGfx();
	return true;
}

CECModel * CECNPCModelDefaultPolicy::GetSgcModel(const char *szHanger, bool bChildHook, const char *szHook){
	CECModel *result = NULL;
	while (szHook){
		if (!IsModelLoaded()){
			break;
		}
		result = m_pNPCModel;
		if (szHanger && bChildHook){
			result = result->GetChildModel(szHanger);
		}
		break;
	}
	return result;
}

A3DSkinModel * CECNPCModelDefaultPolicy::GetSgcSkinModel(const char *szHanger, bool bChildHook, const char *szHook){
	if (CECModel *pModel = GetSgcModel(szHanger, bChildHook, szHook)){
		return pModel->GetA3DSkinModel();
	}
	return NULL;
}

A3DSkeletonHook * CECNPCModelDefaultPolicy::GetSgcHook(const char *szHanger, bool bChildHook, const char *szHook){
	if (A3DSkinModel *pSkinModel = GetSgcSkinModel(szHanger, bChildHook, szHook)){
		return pSkinModel->GetSkeletonHook(szHook, true);
	}
	return NULL;
}

const ACHAR *CECNPCModelDefaultPolicy::GetNameToShow()const{
	return m_pNPC->GetName();
}

void CECNPCModelDefaultPolicy::OptimizeShowExtendStates(){
	if (CECOptimize::Instance().GetGFX().CanShowState(m_pNPC->GetNPCID(), m_pNPC->GetClassID())){
		m_pNPC->ShowExtendStates(0, m_pNPC->m_aExtStates, OBJECT_EXT_STATE_COUNT);
	}else{
		m_pNPC->ClearShowExtendStates();
	}
}

void CECNPCModelDefaultPolicy::OptimizeWeaponStoneGfx(){
}

void CECNPCModelDefaultPolicy::OptimizeArmorStoneGfx(){
}

void CECNPCModelDefaultPolicy::OptimizeSuiteGfx(){
}

const char * CECNPCModelDefaultPolicy::GetActionEnvString()const{
	if (!m_pNPC->IsPetNPC()){
		return NULL;
	}
	const CECPet *pPet = dynamic_cast<const CECPet *>(m_pNPC);
	bool bIsSummonPet = pPet->IsSummonPet();								// 召唤宠
	bool bIsCombatPet = pPet && (pPet->GetDBEssence()->inhabit_type == 6);	// 战斗宠, 海陆空
	bool bIsEvoPet    = pPet && (pPet->GetDBEssence()->inhabit_type == 6);	// 进化宠
	if (bIsSummonPet || bIsCombatPet || bIsEvoPet)
	{
		static const char *szEnv[3] = {"地面", "水中", "空中"};
		return szEnv[m_pNPC->m_iMoveEnv];
	}
	return NULL;
}

const char * CECNPCModelDefaultPolicy::GetActionName(int iAct, bool bAttackStart)const
{
	//	查询普通动作名称
	//
	
	static char szAct[128];
	
	//	添加基本动作名称
	strcpy(szAct, CECNPC::GetBaseActionName(iAct));
	
	//	攻击动作添加起、落动作名称
	if (CECNPC::IsAttackAction(iAct))
		strcat(szAct, bAttackStart ? "起" : "落");
	
	//	添加环境字符串
	const char *szEnv = GetActionEnvString();
	if (szEnv)
	{
		strcat(szAct, "_");
		strcat(szAct, szEnv);
	}
	
	return szAct;
}

const char * CECNPCModelDefaultPolicy::GetSkillCastActionName(int idSkill)const
{
	//	查询技能吟唱动作名
	//
	
	static char szAct[128];
	
	//	添加技能名称
	strcpy(szAct, GNET::ElementSkill::GetNativeName(idSkill));
	
	//	添加环境字符串
	const char *szEnv = GetActionEnvString();
	if (szEnv)
	{
		strcat(szAct, "_");
		strcat(szAct, szEnv);
		strcat(szAct, "_");
	}
	
	//	添加吟唱字符串
	strcat(szAct, "吟唱");
	
	return szAct;
}

const char * CECNPCModelDefaultPolicy::GetSkillAttackActionName(int idSkill, int nSection,bool bStart)const
{
	//	查询技能施法动作名
	//
	
	static char szAct[128];
	
	//	添加技能名称
	strcpy(szAct, GNET::ElementSkill::GetNativeName(idSkill));
	
	//	添加环境字符串
	const char *szEnv = GetActionEnvString();
	if (szEnv)
	{
		strcat(szAct, "_");
		strcat(szAct, szEnv);
		strcat(szAct, "_");
	}
	
	//	添加施放字符串
	strcat(szAct, bStart ? "施放起" : "施放落");
	
	// 多段技能后缀
	if (nSection)
	{
		CECAttacksMan* pMan = g_pGame->GetGameRun()->GetWorld()->GetAttacksMan();
		AString suffix;
		if(pMan->GetSkillSectionActionSuffix(idSkill,nSection,suffix))
		{
			strcat(szAct,"_");
			strcat(szAct,suffix);
		}
	}	
	
	return szAct;
}

bool CECNPCModelDefaultPolicy::PlayModelAction(int iAction, bool bRestart)
{	
	if (!m_pNPCModel){
		return false;
	}
	bool result(false);
	if (iAction == CECNPC::ACT_WOUNDED)
	{
		//	怪物伤害动作以前名称是“伤害叠加”，并且直接存于bon中而不在ecm中出现
		//	后来美术又使用了“伤害”，存于ecm和bon中都有
		//	后来有新的NPC（如召唤宠）有不同的环境（水陆空）中有不同的动作表现
		//	因此有了如下表现
		
		//	查询并播放组合动作
		const char *szAct = GetActionName(iAction);
		if (!m_pNPCModel->GetComActByName(szAct)){
			szAct = GetActionName(CECNPC::ACT_WOUNDED2);
		}
		result = m_pNPCModel->PlayActionByName(CECModel::ACTCHA_WOUND, szAct, 1.0f, false, 0);
	}
	else if(iAction == CECNPC::ACT_ATTACK1 || iAction == CECNPC::ACT_ATTACK2 )
	{
		bool bHideFX = !CECOptimize::Instance().GetGFX().CanShowAttack(m_pNPC->GetNPCID(), m_pNPC->GetClassID());
		result = m_pNPCModel->PlayActionByName(GetActionName(iAction, true), 1.0f, bRestart, 200, true, 0, bHideFX);
		if (result){
			m_pNPCModel->QueueAction(GetActionName(iAction, false), 0, 0, false, false, bHideFX);
			m_pNPCModel->QueueAction(GetActionName(CECNPC::ACT_GUARD), 300);
		}
	}
	else if(iAction == CECNPC::ACT_IDLE)
	{
		result = m_pNPCModel->PlayActionByName(GetActionName(iAction), 1.0f, bRestart, 200);
		if (result){
			m_pNPCModel->QueueAction(GetActionName(CECNPC::ACT_STAND));
		}
	}
	else if(iAction == CECNPC::ACT_NPC_IDLE1 || iAction == CECNPC::ACT_NPC_IDLE2)
	{
		result = m_pNPCModel->PlayActionByName(GetActionName(iAction), 1.0f, bRestart, 200);
		if (result){
			m_pNPCModel->QueueAction(GetActionName(CECNPC::ACT_NPC_STAND));
		}
	}
	else if(iAction == CECNPC::ACT_NPC_ATTACK)
	{
		bool bHideFX = !CECOptimize::Instance().GetGFX().CanShowAttack(m_pNPC->GetNPCID(), m_pNPC->GetClassID());
		result = m_pNPCModel->PlayActionByName(GetActionName(iAction, true), 1.0f, bRestart, 200, true, 0, bHideFX);
		if (result){
			m_pNPCModel->QueueAction(GetActionName(iAction, false), 0, 0, false, false, bHideFX);
			m_pNPCModel->QueueAction(GetActionName(CECNPC::ACT_NPC_STAND), 300);
		}
	}
	else if (iAction == CECNPC::ACT_COMMON_BORN)
	{
		result = m_pNPCModel->PlayActionByName(GetActionName(iAction), 1.0f, bRestart, 200);
		if (result){
			m_pNPCModel->QueueAction(GetActionName(CECNPC::ACT_STAND));
		}
	}
	else{
		result = m_pNPCModel->PlayActionByName(GetActionName(iAction), 1.0f, bRestart, 200);
	}
	return result;
}

bool CECNPCModelDefaultPolicy::PlayAttackAction(int nAttackSpeed, bool *pActFlag){
	if (!m_pNPCModel){
		return false;
	}
	int iAction = (m_pNPC->IsMonsterOrPet()) ? (CECNPC::ACT_ATTACK1 + (rand() % 2)) : CECNPC::ACT_NPC_ATTACK;
	if (!PlayModelAction(iAction, true)){
		return false;
	}
	m_pNPCModel->SetComActFlag(pActFlag, COMACT_FLAG_MODE_ONCE_MULTIIGNOREGFX);
	return true;
}

void CECNPCModelDefaultPolicy::PlaySkillCastAction(int idSkill)
{
	if (!m_pNPCModel)
		return;
	
	char szAct[100];
	szAct[0] = '\0';
	strcpy(szAct, GetSkillCastActionName(idSkill));
	
	A3DCombinedAction * pAct;
	pAct = m_pNPCModel->GetComActByName(szAct);
	if( !pAct )
		strcpy(szAct, "技能吟唱");
	
	if(g_pGame->GetGameRun()->GetWorld()->GetAttacksMan()->FindAttackByAttacker(m_pNPC->GetNPCInfo().nid))
	{
		// signal early attack event
		ClearComActFlag(true);
	}
	
	bool bHideFX = !CECOptimize::Instance().GetGFX().CanShowCast(m_pNPC->GetNPCID(), m_pNPC->GetClassID());
	m_pNPCModel->PlayActionByName(szAct, 1.0f, true, 200, true, 0, bHideFX);
}

bool CECNPCModelDefaultPolicy::PlaySkillAttackAction(int idSkill, int nAttackSpeed,int nSection, bool *pActFlag)
{
	if (!m_pNPCModel){
		return false;
	}
	
	char szAct1[100];
	char szAct2[100];
	szAct1[0] = '\0';
	szAct2[0] = '\0';
	
	strcpy(szAct1, GetSkillAttackActionName(idSkill, nSection,true));
	strcpy(szAct2, GetSkillAttackActionName(idSkill, nSection,false));
	
	A3DCombinedAction * pAct = m_pNPCModel->GetComActByName(szAct1);
	if( !pAct ){
		strcpy(szAct1, "技能施放起");
		strcpy(szAct2, "技能施放落");
	}
	
	pAct = m_pNPCModel->GetComActByName(szAct1);
	if (!pAct){
		return false;
	}
	
	bool bHideFX = !CECOptimize::Instance().GetGFX().CanShowAttack(m_pNPC->GetNPCID(), m_pNPC->GetClassID());
	m_pNPCModel->PlayActionByName(szAct1, 1.0f, true, 200, true, 0, bHideFX);
	m_pNPCModel->QueueAction(szAct2, 0, 0, false, false, bHideFX);
	m_pNPCModel->QueueAction(GetActionName(CECNPC::ACT_GUARD), 300);
	m_pNPCModel->SetComActFlag(pActFlag, COMACT_FLAG_MODE_ONCE_MULTIIGNOREGFX);
	return true;
}

//	class CECNPCModelClonePlayerPolicy
CECNPCModelClonePlayerPolicy::CECNPCModelClonePlayerPolicy(CECNPC *pNPC)
: m_pNPC(pNPC)
, m_pPlayer(NULL)
{
	m_aabbDefPick.Extents.Set(0.3f, 0.85f, 0.3f);
}

void CECNPCModelClonePlayerPolicy::InheritFromPreviousPolicy(const CECNPCModelPolicy *pOtherPolicy){
	if (!pOtherPolicy){
		return;
	}
	m_aabbDefPick = pOtherPolicy->GetDefaultPickAABB();
}

CECNPCModelClonePlayerPolicy::~CECNPCModelClonePlayerPolicy(){
	Release();
}

void CECNPCModelClonePlayerPolicy::Release(){
	ReleaseModel();
}

bool CECNPCModelClonePlayerPolicy::SetPlayerToClone(CECPlayer *pPlayer){
	if (m_pPlayer == pPlayer){
		return true;
	}
	ReleaseModel();
	if (!pPlayer || !CECClonePlayer::CanClone(pPlayer)){
		return false;
	}
	CECNPCClonedMaster *pClone = new CECNPCClonedMaster(g_pGame->GetGameRun()->GetWorld()->GetPlayerMan());
	if (!pClone->Clone(pPlayer, true)){
		ASSERT(false);
		delete pClone;
		return false;
	}
	m_pPlayer = pClone;
	m_pPlayer->SetMoveEnv(m_pNPC->m_iMoveEnv);
	return true;
}

void CECNPCModelClonePlayerPolicy::ReleaseModel(){
	if (m_pPlayer){
		m_pPlayer->Release();
		delete m_pPlayer;
		m_pPlayer = NULL;
	}
}

bool CECNPCModelClonePlayerPolicy::NPCAction2PlayerAction(int iMoveEnv, int iNPCAction, int &iPlayerAction){
	//	特殊情况处理
	switch (iNPCAction){
	case CECNPC::ACT_DIE:
	case CECNPC::ACT_NPC_DIE:{
			switch (iMoveEnv){
			case CECNPC::MOVEENV_GROUND:
				iPlayerAction = CECPlayer::ACT_GROUNDDIE;
				break;
			case CECNPC::MOVEENV_WATER:
				iPlayerAction = CECPlayer::ACT_WATERDIE;
				break;
			case CECNPC::MOVEENV_AIR:
				iPlayerAction = CECPlayer::ACT_AIRDIE_ST;
				break;
			default:
				ASSERT(false);
				return false;
			}
			return true;
		}
	case CECNPC::ACT_ATTACK1:
	case CECNPC::ACT_ATTACK2:
	case CECNPC::ACT_NPC_ATTACK:
		iPlayerAction = CECPlayer::ACT_ATTACK_1;
		return true;
	}
	const static int s_playerActionMap[CECNPC::ACT_MAX] = {
		CECPlayer::ACT_STAND,		/*CECNPC::ACT_STAND*/
		CECPlayer::ACT_LOOKAROUND,	/*CECNPC::ACT_IDLE*/
		CECPlayer::ACT_FIGHTSTAND,	/*CECNPC::ACT_GUARD*/
		-1,							/*CECNPC::ACT_SKILL1*/
		CECPlayer::ACT_WALK,		/*CECNPC::ACT_WALK*/
		-1,	/*单独处理*/			/*CECNPC::ACT_ATTACK1*/
		-1,	/*单独处理*/			/*CECNPC::ACT_ATTACK2*/
		CECPlayer::ACT_RUN,			/*CECNPC::ACT_RUN*/
		-1,	/*单独处理*/			/*CECNPC::ACT_DIE*/
		CECPlayer::ACT_JUMP_START,	/*CECNPC::ACT_JUMP_START*/
		CECPlayer::ACT_JUMP_LAND,	/*CECNPC::ACT_JUMP_LAND*/
		CECPlayer::ACT_JUMP_LOOP,	/*CECNPC::ACT_JUMP_LOOP*/
		-1,							/*CECNPC::ACT_MAGIC1*/
		CECPlayer::ACT_WOUNDED,		/*CECNPC::ACT_WOUNDED*/
		-1,							/*CECNPC::ACT_NPC_CHAT1*/
		-1,							/*CECNPC::ACT_NPC_CHAT2*/
		-1,							/*CECNPC::ACT_NPC_CHATLOOP*/
		CECPlayer::ACT_GAPE,		/*CECNPC::ACT_NPC_IDLE1*/
		CECPlayer::ACT_LOOKAROUND,	/*CECNPC::ACT_NPC_IDLE2*/
		CECPlayer::ACT_STAND,		/*CECNPC::ACT_NPC_STAND*/
		CECPlayer::ACT_WALK,		/*CECNPC::ACT_NPC_WALK*/
		CECPlayer::ACT_RUN,			/*CECNPC::ACT_NPC_RUN*/
		-1,	/*单独处理*/			/*CECNPC::ACT_NPC_ATTACK*/
		-1,	/*单独处理*/			/*CECNPC::ACT_NPC_DIE*/
		-1,							/*CECNPC::ACT_COMMON_BORN*/
		-1,							/*CECNPC::ACT_NPC_DISAPPEAR*/
		CECPlayer::ACT_WOUNDED,		/*CECNPC::ACT_WOUNDED2*/
	};
	if (iNPCAction >= 0 && iNPCAction < CECNPC::ACT_MAX &&
		s_playerActionMap[iNPCAction] >= 0){
		iPlayerAction = s_playerActionMap[iNPCAction];
		return true;
	}
	return false;
}

bool CECNPCModelClonePlayerPolicy::NPCAction2PlayerAction(int iNPCAction, int &iPlayerAction)const{
	if (!NPCAction2PlayerAction(m_pNPC->m_iMoveEnv, iNPCAction, iPlayerAction)){
		return false;
	}
	switch (iPlayerAction){
	case CECPlayer::ACT_STAND:
		iPlayerAction = m_pPlayer->GetStandAction();
		break;
	case CECPlayer::ACT_WALK:
	case CECPlayer::ACT_RUN:
		iPlayerAction = m_pPlayer->GetMoveAction();
		break;
	}
	return true;
}

bool CECNPCModelClonePlayerPolicy::HasAction(int iAction)const{
	if (!m_pPlayer){
		return false;
	}
	int iPlayerAction(-1);
	return NPCAction2PlayerAction(iAction, iPlayerAction);
}

void CECNPCModelClonePlayerPolicy::Tick(DWORD dwDeltaTime){
	if (!m_pPlayer){
		return;
	}
	m_pPlayer->SetMoveEnv(m_pNPC->m_iMoveEnv);
	m_pPlayer->SetUseHintModelFlag(g_pGame->GetConfigs()->GetVideoSettings().bModelLimit && m_pNPC->GetDistToHost() > g_pGame->GetConfigs()->GetPlayerVisRadius());
	m_pPlayer->SetPos(m_pNPC->GetPos());
	m_pPlayer->Tick(dwDeltaTime);
}

void CECNPCModelClonePlayerPolicy::Render(CECViewport* pViewport, bool bHighlight){
	if (!IsModelLoaded()){
		return;
	}
	m_pPlayer->RenderModel(pViewport, bHighlight);
}

void CECNPCModelClonePlayerPolicy::AddShadower(){
	if (!IsModelLoaded()){
		return;
	}
	m_pPlayer->AddShadower();
}

void CECNPCModelClonePlayerPolicy::RenderPateContent(CECViewport* pViewport, bool bHighlight){
	if (!m_pPlayer){
		return;
	}
	m_pPlayer->SetRenderBarFlag(m_pNPC->GetRenderBarFlag());
	m_pPlayer->SetRenderNameFlag(m_pNPC->GetRenderNameFlag());
	m_pPlayer->RenderPateContent(pViewport, bHighlight);
}

void CECNPCModelClonePlayerPolicy::SetPos(const A3DVECTOR3& vPos){
	if (m_pPlayer){
		m_pPlayer->SetPos(vPos);
	}
}

void CECNPCModelClonePlayerPolicy::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp){
	if (m_pPlayer){
		m_pPlayer->ChangeModelMoveDirAndUp(vDir, vUp);
	}
}
bool CECNPCModelClonePlayerPolicy::HasModelCHAABB()const{
	return false;
}

bool CECNPCModelClonePlayerPolicy::GetModelCHAABB(A3DAABB &aabb)const{
	return false;
}

bool CECNPCModelClonePlayerPolicy::HasCHAABB()const{
	return false;
}

bool CECNPCModelClonePlayerPolicy::GetCHAABB(A3DAABB &aabb)const{
	return false;
}

const A3DAABB & CECNPCModelClonePlayerPolicy::GetPickAABB(){
	m_aabbDefPick.Center = m_pNPC->GetPos() + g_vAxisY * m_aabbDefPick.Extents.y;
	m_aabbDefPick.CompleteMinsMaxs();
	if (!m_pPlayer){
		return m_aabbDefPick;
	}
	if (m_pNPC->IsDead()){
		return m_pPlayer->GetPlayerAABB();
	}
	return m_pPlayer->GetPlayerPickAABB();
}

const A3DAABB & CECNPCModelClonePlayerPolicy::GetDefaultPickAABB()const{
	return m_aabbDefPick;
}

void CECNPCModelClonePlayerPolicy::SetDefaultPickAABBExt(const A3DVECTOR3 &vExt){
	m_aabbDefPick.Extents = vExt;
}

bool CECNPCModelClonePlayerPolicy::RayTraceModelConvexHull(ECRAYTRACE* pTraceInfo)const{
	return false;
}

bool CECNPCModelClonePlayerPolicy::TraceWithBrush(BrushTraceInfo * pInfo)const{	
	return false;
}

bool CECNPCModelClonePlayerPolicy::IsModelLoaded()const{
	return m_pPlayer != NULL && m_pPlayer->GetPlayerModel() != NULL;
}

void CECNPCModelClonePlayerPolicy::SetTransparent(float fTransparent){
	if (m_pPlayer){
		m_pPlayer->SetTransparent(fTransparent);
	}
}

bool CECNPCModelClonePlayerPolicy::GetTransparent(float &fTransparent)const{
	if (IsModelLoaded()){
		if (m_pPlayer->GetPlayerModel()->GetA3DSkinModel()){
			fTransparent = m_pPlayer->GetPlayerModel()->GetA3DSkinModel()->GetTransparent();
			return true;
		}
	}
	return false;
}

void CECNPCModelClonePlayerPolicy::PlayGfx(const char* szPath, const char* szHook){
	if (IsModelLoaded()){
		m_pPlayer->GetPlayerModel()->PlayGfx(szPath, szHook);
	}
}

void CECNPCModelClonePlayerPolicy::RemoveGfx(const char* szPath, const char* szHook){
	if (IsModelLoaded()){
		m_pPlayer->GetPlayerModel()->RemoveGfx(szPath, szHook);
	}
}

void CECNPCModelClonePlayerPolicy::ClearComActFlag(bool bSignalCurrent){
	if (m_pPlayer){
		m_pPlayer->ClearComActFlagAllRankNodes(bSignalCurrent);
	}
}

void CECNPCModelClonePlayerPolicy::StopChannelAction(){
	if (m_pPlayer){
		m_pPlayer->StopChannelAction();
	}
}

void CECNPCModelClonePlayerPolicy::PlayActionByName(const char *szActName){
	if (m_pPlayer != NULL){
		m_pPlayer->PlayActionByName(szActName);
	}
}

bool CECNPCModelClonePlayerPolicy::HasComAct(const char *szActName){
	return m_pPlayer != NULL && m_pPlayer->HasComAct(szActName);
}

bool CECNPCModelClonePlayerPolicy::IsPlayingAction(){
	return m_pPlayer != NULL && m_pPlayer->IsPlayingAction();
}

bool CECNPCModelClonePlayerPolicy::IsPlayingAction(const char *szActName){
	return m_pPlayer != NULL && m_pPlayer->IsPlayingAction(szActName);
}

bool CECNPCModelClonePlayerPolicy::IsPlayingAction(int iAction){
	int iPlayerAction(-1);
	if (IsModelLoaded() && NPCAction2PlayerAction(iAction, iPlayerAction)){
		return m_pPlayer->IsPlayingAction(iPlayerAction);
	}
	return false;
}

bool CECNPCModelClonePlayerPolicy::PlayModelAction(int iAction, bool bRestart)
{	
	if (!IsModelLoaded()){
		return false;
	}
	if (CECNPC::IsAttackAction(iAction)){
		return m_pPlayer->PlayAttackAction(0);
	}
	int iPlayerAction(-1);
	if (!NPCAction2PlayerAction(iAction, iPlayerAction)){
		return false;
	}
	return m_pPlayer->PlayAction(iPlayerAction, bRestart);
}

bool CECNPCModelClonePlayerPolicy::PlayAttackAction(int nAttackSpeed, bool *pActFlag){
	if (!IsModelLoaded()){
		return false;
	}
	return m_pPlayer->PlayAttackAction(nAttackSpeed, NULL, pActFlag);
}

void CECNPCModelClonePlayerPolicy::PlaySkillCastAction(int idSkill)
{
	if (!IsModelLoaded()){
		return;
	}
	m_pPlayer->PlaySkillCastAction(idSkill);
}

bool CECNPCModelClonePlayerPolicy::PlaySkillAttackAction(int idSkill, int nAttackSpeed,int nSection, bool *pActFlag)
{
	if (!IsModelLoaded()){
		return false;
	}
	return m_pPlayer->PlaySkillAttackAction(idSkill, nAttackSpeed, NULL, nSection, pActFlag);
}

bool CECNPCModelClonePlayerPolicy::GetEcmProperty(ECMODEL_GFX_PROPERTY* pProperty)const{
	if (!IsModelLoaded()){
		return false;
	}
	CECModel *pModel = m_pPlayer->GetPlayerModel();
	pProperty->bGfxUseLod = pModel->IsGfxUseLOD();
	pProperty->bGfxDisableCamShake = pModel->GetDisableCamShake();
	pProperty->bHostECMCreatedByGfx = pModel->IsCreatedByGfx();
	return true;
}

CECModel * CECNPCModelClonePlayerPolicy::GetSgcModel(const char *szHanger, bool bChildHook, const char *szHook){
	CECModel *result = NULL;
	while (szHook){
		if (!IsModelLoaded()){
			break;
		}
		result = m_pPlayer->GetPlayerModel();
		if (szHanger && bChildHook){
			result = result->GetChildModel(szHanger);
		}
		break;
	}
	return result;
}

A3DSkinModel * CECNPCModelClonePlayerPolicy::GetSgcSkinModel(const char *szHanger, bool bChildHook, const char *szHook){
	if (CECModel *pModel = GetSgcModel(szHanger, bChildHook, szHook)){
		return pModel->GetA3DSkinModel();
	}
	return NULL;
}

A3DSkeletonHook * CECNPCModelClonePlayerPolicy::GetSgcHook(const char *szHanger, bool bChildHook, const char *szHook){
	if (A3DSkinModel *pSkinModel = GetSgcSkinModel(szHanger, bChildHook, szHook)){
		return pSkinModel->GetSkeletonHook(szHook, true);
	}
	return NULL;
}

const ACHAR *CECNPCModelClonePlayerPolicy::GetNameToShow()const{
	return m_pPlayer != NULL ? m_pPlayer->GetName() : m_pNPC->GetName();
}

void CECNPCModelClonePlayerPolicy::OptimizeShowExtendStates(){
	m_pPlayer->OptimizeShowExtendStates();
}

void CECNPCModelClonePlayerPolicy::OptimizeWeaponStoneGfx(){
	m_pPlayer->OptimizeWeaponStoneGfx();
}

void CECNPCModelClonePlayerPolicy::OptimizeArmorStoneGfx(){
	m_pPlayer->OptimizeArmorStoneGfx();
}

void CECNPCModelClonePlayerPolicy::OptimizeSuiteGfx(){
	m_pPlayer->OptimizeSuiteGfx();
}

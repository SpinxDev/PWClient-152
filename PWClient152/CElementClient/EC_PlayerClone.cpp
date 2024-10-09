/*
 * FILE: EC_PlayerClone.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY:
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4284)

#include "EC_PlayerClone.h"

#include "EC_Inventory.h"
#include "EC_Goblin.h"
#include "EC_IvtrGoblin.h"
#include "EC_IvtrFashion.h"
#include "EC_Viewport.h"
#include "EC_ShadowRender.h"
#include "ExpTypes.h"
#include "elementdataman.h"
#include "EC_ManPlayer.h"
#include "EC_Configs.h"
#include "EC_NPC.h"
#include "EC_HostPlayer.h"
#include "EC_HPWorkForceNavigate.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_ElsePlayer.h"
#include "EC_Bezier.h"
#include "EC_PateText.h"

#include <A3DTerrainWater.h>
#include <A3DFont.h>
#include <A3DViewport.h>
#include <A3DEngine.h>
#include <A3DCamera.h>
#include <A3DCameraBase.h>
#include <A3DSkeleton.h>
#include <A3DCombinedAction.h>


extern const char* _hh_ride;
extern const char* _cc_ride; 
extern const char* _hanger_ride;

CECClonePlayer::CECClonePlayer(CECPlayerMan* pPlayerMan) : CECPlayer(pPlayerMan){
	m_iCID			= OCID_CLONED_PLAYER;
	m_bCastShadow	= true;	
	m_bShowCustomize= true;
	m_bUseHintModel	= g_pGame->GetConfigs()->GetVideoSettings().bModelLimit;
}

void CECClonePlayer::Release(){
	CECPlayer::Release();
}

bool CECClonePlayer::CanClone(CECPlayer *player){
	return player != NULL
		&& (player->IsHostPlayer() || player->IsElsePlayer());
}

bool CECClonePlayer::Clone(CECPlayer *player, bool atOnce){
	if (!player){
		ASSERT(false);
		return false;
	}
	if (player->IsHostPlayer()){
		if (LoadFrom(dynamic_cast<CECHostPlayer *>(player), atOnce)){
			return true;
		}
	}
	if (player->IsElsePlayer()){
		if (LoadFrom(dynamic_cast<CECElsePlayer *>(player), atOnce)){
			return true;
		}
	}
	ASSERT(false);
	return false;
}

bool CECClonePlayer::Load(CECPlayer* player, bool atOnce){	
	player->CloneSimplePropertyTo(this);
	OnCloneSimpleProperty();
	if (!LoadPlayerSkeleton(atOnce)){
		a_LogOutput(1, "CECCloneElsePlayer::Load, Failed to load skeleton.");
		return false;
	}
	SetNewExtendStates(0, m_aExtStates, OBJECT_EXT_STATE_COUNT);
	if (player->GetOriginalShapeID()){
		TransformShape(player->GetShapeMask(), true);
	}
	return true;
}
bool CECClonePlayer::ShouldLoadEquipment(int index)const{
	return true;
}
//	Load player data
bool CECClonePlayer::LoadFrom(CECHostPlayer* player, bool atOnce){
	int i = 0;	
	CECIvtrItem* aEquipItems[SIZE_ALL_EQUIPIVTR] = {0};

	//	Create equipments
	for (i=0; i < player->GetEquipment()->GetSize(); i++){
		if (!ShouldLoadEquipment(i)){
			continue;
		}		
		CECIvtrItem* Equip = player->GetEquipment()->GetItem(i);
		aEquipItems[i] = Equip;
	}
	
	//  Create goblin
	if( aEquipItems[EQUIPIVTR_GOBLIN] )
	{
		m_pGoblin = new CECGoblin();
		CECIvtrGoblin* pIvtrGoblin = (CECIvtrGoblin*)aEquipItems[EQUIPIVTR_GOBLIN];
		m_pGoblin->Init(pIvtrGoblin->GetTemplateID(), pIvtrGoblin, this);
	}
	
	//	Build new equipments id array
	int aNewEquips[IVTRSIZE_EQUIPPACK];
	
	for (i=0; i < IVTRSIZE_EQUIPPACK; i++)	{
		CECIvtrItem* pItem = aEquipItems[i];
		if (pItem) {
			aNewEquips[i] = pItem->GetTemplateID();
			if( ((i >= EQUIPIVTR_FASHION_BODY && i <= EQUIPIVTR_FASHION_WRIST) || i == EQUIPIVTR_FASHION_HEAD ) && 
				pItem->GetClassID() == CECIvtrItem::ICID_FASHION )	{
				CECIvtrFashion* pFashionItem = (CECIvtrFashion*)pItem;
				aNewEquips[i] |= (pFashionItem->GetWordColor() << 16) & 0x7fffffff;
			}
			else {
				CECIvtrEquip * pEquip = (CECIvtrEquip *) pItem;
				WORD stoneStatus = pEquip ? pEquip->GetStoneMask() : 0;
				aNewEquips[i] |= (stoneStatus << 16) & 0x7fffffff;
			}
		}
		else{
			aNewEquips[i] = 0;
		}
	}

	memcpy(m_aEquips, aNewEquips, sizeof(m_aEquips));

	return Load(player, atOnce);
}

bool CECClonePlayer::LoadFrom(CECElsePlayer* player, bool atOnce){
	//	Create equipments
	for (int i=0; i < SIZE_ALL_EQUIPIVTR; i++){
		if (!ShouldLoadEquipment(i)){
			continue;
		}
		m_aEquips[i] = player->GetEquipment(i);
	}
	return Load(player, atOnce);
}

bool CECClonePlayer::IsFromHostPlayer()const{
	return GetCharacterID() == g_pGame->GetGameRun()->GetHostPlayer()->GetCharacterID();
}

bool CECClonePlayer::Tick(DWORD dwDeltaTime){
	return IsFromHostPlayer() ? TickHostPlayer(dwDeltaTime) : TickElsePlayer(dwDeltaTime);
}

bool CECClonePlayer::TickHostPlayer(DWORD dwDeltaTime){
	CECPlayer::Tick(dwDeltaTime);

	CECModel* pModel = GetRenderModel();

	if(pModel)
		pModel->Tick(dwDeltaTime);
	
	if (m_pFaceModel)
		m_pFaceModel->Tick(dwDeltaTime);

	return true;
}

bool CECClonePlayer::TickElsePlayer(DWORD dwDeltaTime){
	CECPlayer::Tick(dwDeltaTime);

	if (m_iBoothState != 2)
	{	
		if (m_bShowCustomize != g_pGame->GetConfigs()->GetVideoSettings().bShowCustomize && IsAllResReady())
		{
			if( m_bShowCustomize || !m_pFaceModel )
			{
				// turn off customize
				StoreCustomizeData();
				SetBodyColor(0xffffffff);

				if (GetMajorModel())
					GetMajorModel()->ShowSkin(SKIN_HEAD_INDEX, true);
			}
			else
			{
				ASSERT(m_pFaceModel);

				// turn on customize
				RestoreCustomizeData();
				
				if (InFashionMode())
					UpdateHairModel(true, m_aEquips[EQUIPIVTR_FASHION_HEAD]);
				else
					UpdateHairModel(true, m_aEquips[EQUIPIVTR_HEAD]);

				if (GetMajorModel())
					GetMajorModel()->ShowSkin(SKIN_HEAD_INDEX, false);
			}

			m_bShowCustomize = g_pGame->GetConfigs()->GetVideoSettings().bShowCustomize;
		}

		CECPlayer* pBuddy = m_pPlayerMan->GetPlayer(m_iBuddyId);
		if (m_AttachMode != enumAttachNone && m_bHangerOn)
		{	
			if( (!pBuddy || !pBuddy->GetPlayerModel()) && m_pPlayerModel )
			{
				m_pPlayerModel->SetSlowestUpdateFlag(!m_bVisible || m_bUseHintModel);
				m_pPlayerModel->Tick(dwDeltaTime);
			}
		}
		else if (m_pPetModel)
		{
			m_pPetModel->SetSlowestUpdateFlag(!m_bVisible || m_bUseHintModel);
			m_pPetModel->Tick(dwDeltaTime);
		}
		else if (m_pPlayerModel)
		{
			m_pPlayerModel->SetSlowestUpdateFlag(!m_bVisible || m_bUseHintModel);
			m_pPlayerModel->Tick(dwDeltaTime);
		}
	}

	if (m_bShowCustomize && m_iBoothState != 2)
	{
		if (m_pFaceModel && GetMajorModel() && !GetMajorModel()->IsSkinShown(SKIN_HEAD_INDEX))
			m_pFaceModel->Tick(dwDeltaTime);
	}
	return true;
}

//	Render routine
bool CECClonePlayer::Render(CECViewport* pViewport, int iRenderFlag/* 0 */){
	CECPlayer::Render(pViewport,iRenderFlag);

	CECPlayer* player = m_pPlayerMan->GetPlayer(GetCharacterID());
	if(player)
		SetRenderGoblin(player->GetRenderGoblin());

	m_PateContent.iVisible = 0;

	A3DCameraBase* pCamera = pViewport->GetA3DCamera();
	m_fDistToCamera	= CalcDist(pCamera->GetPos(), true);

	if( iRenderFlag == RD_NORMAL ){
		if (m_fDistToCamera < 30.0f && GetBoothState() != 2)
			m_bCastShadow = true;
		else
			m_bCastShadow = false;
	}
	else{
		if (m_fDistToCamera > 30.0f )
			return true;

		A3DTerrainWater * pTerrainWater = g_pGame->GetGameRun()->GetWorld()->GetTerrainWater();
		if( iRenderFlag == RD_REFLECT && pTerrainWater->IsUnderWater(GetPlayerAABB().Maxs) )
			return true;
		else if( iRenderFlag == RD_REFRACT ){	
			bool bCamUnderWater = pTerrainWater->IsUnderWater(pCamera->GetPos());
			if( !bCamUnderWater && !pTerrainWater->IsUnderWater(GetPlayerAABB().Mins) )
				return true;
			else if( bCamUnderWater && pTerrainWater->IsUnderWater(GetPlayerAABB().Maxs) )
				return true;
		}

		m_bCastShadow = false;
	}

	if (!IsAllResReady())
		return true;

	if( !UpdateCurSkins() )
		return true;

	if( !IsClothesOn() && ShouldUseClothedModel() )	// 正常模型不允许不穿衣服出现
		return true;

	if (!m_bVisible){
		//	Only render player head text
		if (iRenderFlag == RD_NORMAL && m_bRenderName)
			RenderName(pViewport, g_pGame->GetConfigs()->GetPlayerTextFlags());

		return true;
	}

	bool bSkipPlayerRender = false;
	
	CECModel* pModel = GetRenderModel();
	
	// skip rendering when model is not ready
	if( pModel == m_pPlayerModel && !IsShapeModelReady() ) {
		pModel = NULL;
		bSkipPlayerRender = true;
	}
	
	if (pModel) {
		
		if (!IsShapeModelChanged() && GetMajorModel()){
			if (m_fDistToCamera < 30.0f && m_pFaceModel)
				GetMajorModel()->ShowSkin(SKIN_HEAD_INDEX, false);
			else
				GetMajorModel()->ShowSkin(SKIN_HEAD_INDEX, true);
		}

		pModel->Render(pViewport->GetA3DViewport());
		
		if (m_bCastShadow && g_pGame->GetShadowRender()){
			CECPlayer * pPlayer = this;
			A3DAABB shadowAABB = GetShadowAABB();
			g_pGame->GetShadowRender()->AddShadower(shadowAABB.Center, shadowAABB, SHADOW_RECEIVER_TERRAIN, PlayerRenderForShadow, pPlayer);
		}
	}

	if ( m_pFaceModel && !IsShapeModelChanged() && !bSkipPlayerRender &&
		 GetMajorModel() && !GetMajorModel()->IsSkinShown(SKIN_HEAD_INDEX) ){
		m_pFaceModel->Render(pViewport, false, false);
	}

	if (iRenderFlag == RD_NORMAL){
		//	Render player head text
		if (m_bRenderBar)
			RenderBars(pViewport);

		if (m_bRenderName)
			RenderName(pViewport, g_pGame->GetConfigs()->GetPlayerTextFlags());

		//	Print player's precise position
		if (g_pGame->GetConfigs()->GetShowPosFlag()){
			ACHAR szMsg[100];
			A3DVECTOR3 vPos = GetPos();
			a_sprintf(szMsg, _AL("%.3f, %.3f, %.3f"), vPos.x, vPos.y, vPos.z);
			int y = pViewport->GetA3DViewport()->GetParam()->Height - 20;
			vPos += g_vAxisY * (m_aabb.Extents.y * 3.0f);
			g_pGame->GetA3DEngine()->GetSystemFont()->TextOut3D(szMsg, pViewport->GetA3DViewport(), vPos, 0, 0xffffffff);
		}
	}

	RenderGoblinOrSprite(pViewport);

//	RenderPetCureGFX();
	RenderMultiObjectGFX();
//	RenderMonsterSpiritGfx();

	return true;
}
CECModel* CECClonePlayer::GetRenderModel(){
	return m_pPetModel ? m_pPetModel : m_pPlayerModel;
}

void CECClonePlayer::RenderModel(CECViewport* pViewport, bool bHighlight){
	m_PateContent.iVisible = 0;
	
	//	打坐使用特殊模型时、无高亮效果
	if (bHighlight && IsSitting() && (GetRace() == RACE_GHOST || GetRace() == RACE_OBORO))
		bHighlight = false;

	A3DCameraBase* pCamera = pViewport->GetA3DCamera();
	m_fDistToCamera	= CalcDist(pCamera->GetPos(), true);
	
	if (!IsAllResReady())
		return;
	
	if( !UpdateCurSkins() )
		return;
	
	if( !IsClothesOn() && ShouldUseClothedModel() )	// 正常模型不允许不穿衣服出现
		return;
	
	if (!m_bVisible){		
		return;
	}
	CECModel* pModel = GetRenderModel();
	
	// skip rendering when model is not ready
	bool bSkipPlayerRender = false;
	if( pModel == m_pPlayerModel && !IsShapeModelReady() )
	{
		pModel = NULL;
		bSkipPlayerRender = true;
	}	
	if (pModel)
	{		
		if (!IsShapeModelChanged() && m_bShowCustomize && GetMajorModel())
		{
			if (m_fDistToCamera < 30.0f && m_pFaceModel)
				GetMajorModel()->ShowSkin(SKIN_HEAD_INDEX, false);
			else
				GetMajorModel()->ShowSkin(SKIN_HEAD_INDEX, true);
		}
		else if( !m_bShowCustomize && GetMajorModel() && !GetMajorModel()->IsSkinShown(SKIN_HEAD_INDEX) )
		{
			GetMajorModel()->ShowSkin(SKIN_HEAD_INDEX, true);
		}
		
		if (bHighlight)
			g_pGame->RenderHighLightModel(pViewport, pModel);
		else
			pModel->Render(pViewport->GetA3DViewport());
	}
	
	if ( m_pFaceModel && 
		!IsShapeModelChanged() && !bSkipPlayerRender &&
		GetMajorModel() && !GetMajorModel()->IsSkinShown(SKIN_HEAD_INDEX) )
	{
		m_pFaceModel->Render(pViewport, false, bHighlight);
	}
	RenderGoblinOrSprite(pViewport);
}

void CECClonePlayer::AddShadower(){
	if (!GetRenderModel()){
		return;
	}
	A3DAABB shadowAABB = GetShadowAABB();
	g_pGame->GetShadowRender()->AddShadower(shadowAABB.Center, shadowAABB, SHADOW_RECEIVER_TERRAIN, PlayerRenderForShadow, this);
}

void CECClonePlayer::RenderPateContent(CECViewport* pViewport, bool bHighlight){
	bool backupShowID = g_pGame->GetConfigs()->GetShowIDFlag();
	g_pGame->GetConfigs()->SetShowIDFlag(false);
	if (GetRenderBarFlag()){
		RenderBars(pViewport);
	}	
	if (GetRenderNameFlag()){
		RenderName(pViewport, g_pGame->GetConfigs()->GetPlayerTextFlags());
	}
	g_pGame->GetConfigs()->SetShowIDFlag(backupShowID);
}

void CECClonePlayer::PlayActionByName(const char *szActName){
	PlayNonSkillActionWithName(CECPlayer::ACT_MAX, szActName);
}

bool CECClonePlayer::HasComAct(const char *szActName){
	return GetPlayerModel() != NULL && GetPlayerModel()->GetComActByName(szActName) != NULL;
}

bool CECClonePlayer::IsPlayingAction(const char *szActName){	
	return IsPlayingAction() && !stricmp(GetPlayerModel()->GetCurComAct()->GetComAct()->GetName(), szActName);
}

//////////////////////////////////////////////////////////////////////////

CECHostNavigatePlayer::CECHostNavigatePlayer(CECPlayerMan* pPlayerMan,CECHostPlayer* pHost):CECClonePlayer(pPlayerMan),
m_pHostPlayer(pHost),
m_szNavigateModelFile(NULL),
m_pNavigateModel(NULL),
m_bNavigateModelApplied(false),
m_pNavigateCtrl(NULL)
{
	m_iCID = OCID_HOST_NAVIGATER;

	SetPos(pHost->GetPos());
	ChangeModelMoveDirAndUp(pHost->GetDir(),pHost->GetUp());
}
bool CECHostNavigatePlayer::LoadConfig(){
	if (m_pNavigateCtrl = new CECNavigateCtrl(m_pHostPlayer))
		return m_pNavigateCtrl->LoadConfig("configs\\force_navigate.txt");
	else {		
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECHostNavigatePlayer::Init", __LINE__);
	}	
	return false;
}
CECModel* CECHostNavigatePlayer::GetRenderModel(){
	return m_pNavigateModel ? m_pNavigateModel : CECClonePlayer::GetRenderModel();
}

bool CECHostNavigatePlayer::IsNavigateMoving(){
	if (m_pNavigateCtrl && m_pNavigateCtrl->GetBezierWalker())	
		return m_pNavigateCtrl->GetBezierWalker()->IsWalking();

	return false;	
}
void CECHostNavigatePlayer::OnNavigateEvent(int task,int e){
	if(!m_pNavigateCtrl) return;

	if (e == CECNavigateCtrl::EM_PREPARE)		
		m_pNavigateCtrl->OnPrepareNavigate(task);	
	else if(e == CECNavigateCtrl::EM_BEGIN)
		m_pNavigateCtrl->OnBeginNavigate();
	else if(e == CECNavigateCtrl::EM_END)
		m_pNavigateCtrl->OnEndNavigate();
}
void CECHostNavigatePlayer::Release(){
	CECPlayer::Release();

	if (m_pNavigateModel){
		if(m_pNavigateModel->GetChildModel(_hanger_ride))
			m_pNavigateModel->RemoveChildModel(_hanger_ride, false);
		A3DRELEASE(m_pNavigateModel);
	}
	if (m_pNavigateCtrl){
		delete m_pNavigateCtrl;
		m_pNavigateCtrl = NULL;
	}
}
bool CECHostNavigatePlayer::Init(){
	// navigate
	m_bNavigateModelApplied = false;

	// 加载navigate model
	if(m_szNavigateModelFile){
		m_pNavigateModel = new CECModel();
		if (m_pNavigateModel && !m_pNavigateModel->Load(m_szNavigateModelFile)){
			a_LogOutput(1, "CECHostNavigatePlayer::Init, Failed to load navigate model %s", m_szNavigateModelFile);			
			delete m_pNavigateModel;
		}
	}
	// 加载 player model
	return LoadFrom(m_pHostPlayer, false);
}
bool CECHostNavigatePlayer::Tick(DWORD dwDeltatime){
	CECClonePlayer::Tick(dwDeltatime);

	if (!m_bNavigateModelApplied && IsAllResReady()){
		if(!IsShapeChanged())
			ApplyNavigateModel();
		else if (IsShapeChanged() && IsShapeModelChanged() && m_pNavigateModel)
			ApplyNavigateModel();
	}
	return true;
}
bool CECHostNavigatePlayer::IsReadyNavigate(){
	return IsAllResReady() && m_bNavigateModelApplied;
}

bool CECHostNavigatePlayer::PlayNavigateAction(){
	if (m_pNavigateModel) {		
		const char * szPetAct = NULL;
		szPetAct = CECNPC::GetBaseActionName(CECNPC::ACT_WALK);
		m_pNavigateModel->PlayActionByName(szPetAct, 1.0f, false, 200, true);
	}
	
	PLAYER_ACTION action = m_PlayerActions[ACT_STAND];
	
	char szAct[256];
	sprintf(szAct, "%s_%s", action.data->action_prefix, "骑乘_通用");

	CECPlayer::PlayNonSkillActionWithName(GetMoveStandAction(false, IsFighting()),szAct);

	return true;
}
A3DAABB CECHostNavigatePlayer::GetShadowAABB(){
	A3DAABB shadowAABB;	
	shadowAABB.Clear();	
	shadowAABB.Merge(m_aabb);
	
	if(m_pNavigateModel)
		shadowAABB.Merge(m_pNavigateModel->GetModelAABB());
	else if (IsRidingOnPet() && m_pPetModel && m_pPetModel->GetA3DSkinModel())
		shadowAABB.Merge(m_pPetModel->GetModelAABB());
	else if (GetPlayerModel() && GetPlayerModel()->GetA3DSkinModel())
		shadowAABB.Merge(GetPlayerModel()->GetModelAABB());
	
	return shadowAABB;
}
void CECHostNavigatePlayer::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp){
	CECPlayer::SetDirAndUp(vDir, vUp);
	if(m_pNavigateModel)
		m_pNavigateModel->SetDirAndUp(vDir,vUp);
}
bool CECHostNavigatePlayer::ShouldLoadEquipment(int index)const{
	return index != EQUIPIVTR_FLYSWORD;
}
void CECHostNavigatePlayer::OnCloneSimpleProperty(){
	m_RidingPet.Reset();	//	因骑宠与 m_pNavigateModel 都需要挂接人物模型挂点，会重复释放，故不能再加载使用骑宠
}
void CECHostNavigatePlayer::SetPos(const A3DVECTOR3& vPos){
	CECPlayer::SetPos(vPos);

	if(m_pNavigateModel)
		m_pNavigateModel->SetPos(vPos);
}
bool CECHostNavigatePlayer::ApplyNavigateModel(){	
	if ( !GetMajorModel() || !m_pNavigateModel || m_bNavigateModelApplied)
		return false;
	
	const A3DVECTOR3 vCurPos = GetPos();
	
	m_aabbServer.Center = vCurPos + A3DVECTOR3(0.0f, m_aabbServer.Extents.y, 0.0f);
	m_aabbServer.CompleteMinsMaxs();
	
	SetUseGroundNormal(true);
	m_pNavigateModel->SetAffectedByParent(false);
	m_pNavigateModel->SetPos(vCurPos);
	m_pNavigateModel->SetDirAndUp(GetDir(), GetUp());
	
	// 把角色模型挂到强制移动模型上
	int iIndex;
	A3DSkeletonHook* pHook = GetMajorModel()->GetA3DSkinModel()->GetSkeleton()->GetHook(_cc_ride, &iIndex);
	if (pHook) pHook->SetFixDirFlag(true);
	
	if(IsShapeModelChanged())
		m_pNavigateModel->AddChildModel(_hanger_ride, false, _hh_ride, GetPlayerModel(), "HH_feijian");
	else
		m_pNavigateModel->AddChildModel(_hanger_ride, false, _hh_ride, GetMajorModel(), _cc_ride);
	
	m_pNavigateModel->GetA3DSkinModel()->Update(0);
	
//	PlayAction(GetMoveStandAction(false, IsFighting()), 1.0f, true);
	PlayNavigateAction();

	m_bNavigateModelApplied = true;

	OnNavigateEvent(0,CECNavigateCtrl::EM_BEGIN);

	return true;
}
///////////////////////////////////////////////////////////////////////////
CECNPCClonedMaster::CECNPCClonedMaster(CECPlayerMan* pPlayerMan)
: CECClonePlayer(pPlayerMan)
{
	m_iCID = OCID_NPCCLONED_MASTER;
}

int	CECNPCClonedMaster::GetMoveAction()const{
	return GetMoveStandAction(true);
}

int	CECNPCClonedMaster::GetStandAction()const{
	return GetMoveStandAction(false);
}

bool CECNPCClonedMaster::Tick(DWORD dwDeltaTime){
	CECClonePlayer::Tick(dwDeltaTime);

	//	根据环境启用、禁用飞行器
	switch (GetMoveEnv()){
	case MOVEENV_GROUND:
	case MOVEENV_WATER:
		if (IsFlying()){
			m_dwStates &= ~GP_STATE_FLY;
			ShowWing(false);
		}
		break;
	case MOVEENV_AIR:
		if (!IsFlying()){
			if (!HasWingModel()){
				if (CECPlayer *pMasterPlayer = m_pPlayerMan->GetPlayer(GetCharacterID())){
					if (int idFlySword = pMasterPlayer->GetEquippedItem(EQUIPIVTR_FLYSWORD)){
						int aNewEquips[IVTRSIZE_EQUIPPACK];
						memcpy(aNewEquips, m_aEquips, sizeof(aNewEquips));
						aNewEquips[EQUIPIVTR_FLYSWORD] = idFlySword;
						ShowEquipments(aNewEquips, true);
					}
				}
			}
			if (HasWingModel()){
				m_dwStates |= GP_STATE_FLY;
				ShowWing(true);
			}
		}
		break;
	}
	return true;
}
/*
 * FILE: EC_HostPlayer.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/1
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_InputCtrl.h"
#include "EC_Model.h"
#include "EC_Utility.h"
#include "EC_World.h"
#include "EC_CDS.h"
#include "EC_GameSession.h"
#include "EC_ShadowRender.h"
#include "EC_PortraitRender.h"
#include "EC_GPDataType.h"
#include "EC_Viewport.h"
#include "EC_NPCServer.h"
#include "EC_Matter.h"
#include "EC_RTDebug.h"
#include "EC_ManNPC.h"
#include "EC_ManMatter.h"
#include "EC_ManPlayer.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "EC_FixedMsg.h"
#include "EC_Inventory.h"
#include "EC_DealInventory.h"
#include "EC_IvtrTypes.h"
#include "EC_IvtrScroll.h"
#include "EC_IvtrWeapon.h"
#include "EC_IvtrArrow.h"
#include "EC_IvtrConsume.h"
#include "EC_IvtrFlySword.h"
#include "EC_IvtrFashion.h"
#include "EC_IvtrPetItem.h"
#include "EC_IvtrEquipMatter.h"
#include "EC_IvtrGoblin.h"
#include "EC_IvtrTaskItem.h"
#include "EC_ElsePlayer.h"
#include "EC_GFXCaster.h"
#include "EC_Face.h"
#include "EC_Resource.h"
#include "EC_ShortcutSet.h"
#include "EC_TaskInterface.h"
#include "EC_Skill.h"
#include "EC_HostInputFilter.h"
#include "EC_Configs.h"
#include "EC_PateText.h"
#include "EC_Team.h"
#include "EC_Friend.h"
#include "EC_Faction.h"
#include "EC_Sprite.h"
#include "EC_PetCorral.h"
#include "EC_ComboSkill.h"
#include "EC_Pet.h"
#include "EC_PetWords.h"
#include "EC_ForceMgr.h"
#include "EC_AutoTeam.h"
#include "EC_AutoPolicy.h"
#include "EC_PlayerWrapper.h"
#include "EC_UIConfigs.h"
#include "EC_Shortcut.h"
#include "EC_QuickBuyPopMan.h"
#include "EC_QuickBuySaveLife.h"
#include "EC_RandMallShopping.h"
#include "EC_FactionPVP.h"
#include "EC_UseUniversalToken.h"
#include "EC_UniversalTokenVisitHTTPCommand.h"
#include "EC_ShoppingManager.h"
#include "EC_ShoppingItemsMover.h"
#include "EC_SkillConvert.h"

#include "EC_HPWork.h"
#include "EC_HPWorkUse.h"
#include "EC_HPWorkDead.h"
#include "EC_HPWorkMelee.h"
#include "EC_HPWorkStand.h"
#include "EC_HPWorkTrace.h"
#include "EC_HPWorkFollow.h"
#include "EC_HPWorkMove.h"
#include "EC_HPWorkFly.h"
#include "EC_HPWorkFall.h"
#include "EC_HPWorkSit.h"
#include "EC_HPWorkSpell.h"
#include "defence/stackchecker.h"
#include "DlgInputNO.h"

#include "DlgWorldMap.h"
#include "DlgStorage.h"
#include "EC_Instance.h"
#include "roleinfo"
#include "playerbriefinfo"
#include "Network\\gnetdef.h"
#include "Network\\ids.hxx"
#include "Network\\createfactionfortress_re.hpp"

#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"
#include "A3DCombinedAction.h"
#include "A2DSprite.h"
#include "A3DTerrain2.h"
#include "A3DViewport.h"
#include "A3DFont.h"
#include "A3DCamera.h"
#include "A3DCollision.h"
#include "A3DTerrainWater.h"
#include "EC_HostGoblin.h"

#include "elementdataman.h"

#include "AMSoundBuffer.h"
#include "AMSoundBufferMan.h"
#include "AScriptFile.h"

#include "EC_Algorithm.h"
#include "EC_IvtrArmor.h"
#include "EC_IvtrWeapon.h"
#include "EC_ComputerAid.h"
#include "EC_CountryConfig.h"
#include "EC_Bezier.h"
#include "EC_CastSkillWhenMove.h"

#include "DlgSkillEdit.h"
#include "DlgOnlineAward.h"
#include "EC_OfflineShopCtrl.h"

#include "DlgWikiRecipe.h"
#include "DlgAutoHelp.h"
#include "DlgChariot.h"
#include "DlgTokenShop.h"
#include "DlgCountryWarMap.h"

#include "EC_ActionSwitcher.h"
#include "EC_FashionShop.h"
#include "EC_HostSkillModel.h"
#include "EC_ComboSkillState.h"
#include "EC_HPWorkForceNavigate.h"
#include "EC_PlayerClone.h"
#include "EC_UIHelper.h"
#include "EC_InstanceReenter.h"
#include "EC_PlayerLevelRankRealmChangeCheck.h"
#include "EC_DragDropHelper.h"

#include <A3DLight.h>
#include <A3DConfig.h>
#include <set>
#include <algorithm>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Host config data version
#define HOSTCFG_VERSION		11

//  Favorite auction version
#define FAVOR_AUCTION_VERSION 1

//	ID of return-town skill
#define ID_RETURNTOWN_SKILL	167

//	ID of summon player skill
#define ID_SUMMONPLAYER_SKILL	1824

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

static float l_fTestDist = -1.0f;
#define MAX_JUMP_COUNT	2

#define FASHION_NUM		6	// Current fashion num: head, wrist, leg, foot, body, weapon
//static int max_jump_count = 2;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHostPlayer
//	
///////////////////////////////////////////////////////////////////////////
const int CECHostPlayer::REINCARNATION_TOME::max_exp = 1800000000;
static bool compare_rank(const S2C::cmd_countrybattle_live_show_result::score_rank_entry& lhs, 
						 const S2C::cmd_countrybattle_live_show_result::score_rank_entry& rhs)
{
	return lhs.rank > rhs.rank;
}
void CECHostPlayer::BATTLEINFO::SetCountryBattleLiveShowInfo(const S2C::cmd_countrybattle_live_show_result& cmd)
{
	// 攻方排名
	OffenseRanks.assign(cmd.offense_ranks.begin(), cmd.offense_ranks.end());
	std::sort(OffenseRanks.begin(), OffenseRanks.end(), compare_rank);
	// 攻方死亡
	OffenseDeaths.assign(cmd.offense_deaths.begin(), cmd.offense_deaths.end());
	// 守方排名
	DefenceRanks.assign(cmd.defence_ranks.begin(), cmd.defence_ranks.end());
	std::sort(DefenceRanks.begin(), DefenceRanks.end(), compare_rank);
	// 守方死亡
	DefenceDeaths.assign(cmd.defence_deaths.begin(), cmd.defence_deaths.end());
	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI) {
		CDlgCountryWarMap* pDlg = dynamic_cast<CDlgCountryWarMap*>(pGameUI->GetDialog("Win_CountryWarMap"));
		if (pDlg) pDlg->UpdateLiveShowImg();
	}
}

bool CECHostPlayer::TARGETITEM_DLG_CTRL::NeedShow(int& iSlot)
{
	iSlot = -1;
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if( !pHost->GetSelectedTarget() || pHost->GetBoothState() != 0 )
		return false;

	CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(pHost->GetSelectedTarget());

	CECInventory* pPack = pHost->GetPack();
	for(int i=0; i<pPack->GetSize(); i++)
	{
		CECIvtrItem* pItem = pPack->GetItem(i);
		if( !pItem || pItem->GetClassID() != CECIvtrItem::ICID_TARGETITEM )
			continue;

		CECIvtrTargetItem* pTargetItem = dynamic_cast<CECIvtrTargetItem*>(pItem);
		if( !pTargetItem ) continue;

		const TARGET_ITEM_ESSENCE* pDB = pTargetItem->GetDBEssence();
		if( !(pDB->combined_switch & TICS_CAN_POP) )
			continue;
		if (!pTargetItem->CheckUseCondition()){
			continue;
		}
		if( pNPC && pTargetItem->FindTargetIDForPop(pNPC->GetTemplateID()) )
		{
			iSlot = i;
			break;
		}
		else if( !pTargetItem->HasTargetIDForPop() && iSlot == -1 )
			iSlot = i;
	}
	
	return iSlot != -1 ? true : false;
}

void CECHostPlayer::TARGETITEM_DLG_CTRL::Update(DWORD dwDeltaTime)
{
	PAUIDIALOG pDlg = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_ItemPop");
	if( !pDlg ) return;

	if( cntCheck.IncCounter(dwDeltaTime) )
	{
		int iSlot = -1;
		if( NeedShow(iSlot) != pDlg->IsShow() )
		{
			pDlg->SetData(iSlot);
			pDlg->Show(!pDlg->IsShow());
		}
		else if( iSlot != (int)pDlg->GetData() && pDlg->IsShow() )
		{
			pDlg->Show(false);
			pDlg->SetData(iSlot);
			pDlg->Show(true);
		}

		cntCheck.Reset();
	}
}

CECHostPlayer::CECHostPlayer(CECPlayerMan* pPlayerMan) : 
CECPlayer(pPlayerMan),
m_MoveCtrl(this)
{
	m_iCID				= OCID_HOSTPLAYER;
	m_fCameraPitch		= 0.0f;
	m_fCameraYaw		= 0.0f;
	m_dwMoveRelDir		= 0;
	m_iTurnCammera		= 0;
	m_bCastShadow		= true;
	m_selectTargetUpdateTimer.Reset(500);
	m_idUCSelTarget		= 0;
	m_idClickedMan		= 0;
	m_idSevNPC			= 0;
	m_idTradePlayer		= 0;
	m_bUsingTrashBox	= false;
	m_fFlyHeight		= 0.0f;
	m_iOldWalkMode		= MOVE_STAND;
	m_bTalkWithNPC		= false;
	m_pPrepSkill		= NULL;
	m_pTargetItemSkill	= NULL;
	m_bRenderName		= true;
	m_bBeRoot			= false;
	m_bMelee			= false;
	m_bTrashPsw			= false;
	m_iTrashBoxMoneyCnt	= 0;
	m_bFirstTBOpen		= true;
	m_iAccountBoxMoneyCnt = 0;
	m_bFirstAccountBoxOpen = true;
	m_bFirstFashionOpen	= true;
	m_dwLIES			= 0;
	m_idCurHover		= 0;
	m_fTransparent		= -1.0f;
	m_bEnterGame		= false;
	m_idBoothTrade		= 0;
	m_bPrepareFight		= false;
	m_fReviveExp		= -1.0f;
	m_bChangingFace		= false;
	m_bSpellDSkill		= false;
	m_iGetFriendCnt		= 0;
	m_dwPariahTime		= 0;
	m_nCash				= 0;
	m_nCashMoneyRate	= 0;
	m_iTouchPoint		= 0;
	m_bCashMoneyOpen	= false;
	m_nDividend			= 0;
	m_bDividentInitialized = false;
	m_nCurEmotionSet	= 0;
	
	m_pPack				= NULL;
	m_pEquipPack		= NULL;
	m_pTaskPack			= NULL;
	m_pDealPack			= NULL;
	m_pEPDealPack		= NULL;
	m_pTrashBoxPack		= NULL;
	m_pTrashBoxPack2	= NULL;
	m_pTrashBoxPack3	= NULL;
	m_pAccountBoxPack	= NULL;
	m_pGeneralCardPack	= NULL;
	m_pBuyPack			= NULL;
	m_pSellPack			= NULL;
	m_pBoothSPack		= NULL;
	m_pBoothBPack		= NULL;
	m_pEPBoothSPack		= NULL;
	m_pEPBoothBPack		= NULL;
	m_pEPEquipPack		= NULL;
	m_pClientGenCardPack = NULL;

	m_pMoveTargetGFX	= NULL;
	m_pSelectedGFX		= NULL;
	m_pHoverGFX			= NULL;
	m_pFloatDust		= NULL;
	m_pTaskInterface	= NULL;
	m_pWorkMan			= NULL;
	m_pSpritePortrait	= NULL;
	memset(m_pDuelCountDown, 0, sizeof(A2DSprite *) * 3);
	memset(m_pDuelStates, 0, sizeof(A2DSprite *) * 3);
	m_pCurMoveSnd		= NULL;
	m_pFriendMan		= NULL;
	m_pPetCorral		= NULL;
	m_pPetWords			= NULL;
	m_pComboSkill		= NULL;
	m_pNavigatePlayer	= NULL;
	
	m_iJumpCount		= 0;
	m_bJumpInWater		= false;
	m_fVertSpeed		= 0.0f;

	m_bAutoFashion		= false;
	m_AutoFashionCnt.SetPeriod(60000);
	m_AutoFashionCnt.Reset(true);

	m_iCurFashionSuitID	= 0;

	m_vVelocity.Clear();
	m_vAccel.Clear();

	m_TaskCounter.SetPeriod(3000);
	m_TrickCnt.SetPeriod(1000);
	m_TrickCnt.Reset(true);
	m_IncantCnt.SetPeriod(1000);
	m_IncantCnt.Reset(true);
	m_GatherCnt.SetPeriod(1000);
	m_GatherCnt.Reset(true);
	m_TLPosCnt.SetPeriod(10000);
	m_TLPosCnt.Reset(true);
	m_TMPosCnt.SetPeriod(60000);
	m_TMPosCnt.Reset(true);
	m_BindCmdCoolCnt.SetPeriod(35000);
	m_BindCmdCoolCnt.Reset(true);
	m_PetOptCnt.SetPeriod(1000);
	m_PetOptCnt.Reset(true);

	m_bRenderGoblin = false;

	memset(m_aNPCPacks, 0, sizeof (m_aNPCPacks));
	memset(m_aSCSets1, 0, sizeof (m_aSCSets1));
	memset(m_aSCSets2, 0, sizeof (m_aSCSets2));
	memset(&m_BreathData, 0, sizeof (m_BreathData));
	memset(&m_BattleInfo, 0, sizeof (m_BattleInfo));

	int i;
	for (i=0; i < GP_CT_MAX; i++)
	{
		m_aCoolTimes[i].iCurTime = 0;
		m_aCoolTimes[i].iMaxTime = 0;
	}
	
	for (i=0; i < SIZE_FASHIONSCSET; i++)
	{
		m_aFashionSCSets[i] = 0;
	}

	m_nTimeToExitInstance = 0;

	//	Create inventories
	if (!CreateInventories())
	{
		a_LogOutput(1, "CECHostPlayer::CECHostPlayer, Failed to create host's inventoies");
	}

	//	Create shortcuts
	if (!CreateShortcutSets())
	{
		a_LogOutput(1, "CECHostPlayer::CECHostPlayer, Failed to create host's shortcut sets");
	}

	m_multiExpState = MES_UNINITED;
	m_multiExpFactor = 1;
	m_multiExpLastTimeStamp = 0;
	m_multiExpEnhanceTime = 0;
	m_multiExpBufferTime = 0;
	m_multiExpImpairTime = 0;
	m_multiExpExpireMoment = 0;
	m_multiExpBeginTimesLeft = 0;
	
	m_contribInfoInitialized = false;
	m_playerLimits.reset();

	m_pForceMgr = NULL;
	m_pOnlineAwardCtrl = NULL;
	m_pAutoTeam = NULL;

	m_bPVPNoPenaltyState = false;
	::memset(m_bDefenseRuneEnable, 0, sizeof(m_bDefenseRuneEnable));
	m_bIsInKingService = false;
	m_iKingScore = 0;

	m_bInRebuildPet = false;	
	m_pOffShopCtrl = NULL;
	m_bTitleDataReady = false;
	m_ReincarnationTome.Clear();
	m_iRealmExp = 0;

	m_pChariot = NULL;
	m_iCurSysModIndex = 0;

	m_pActionSwitcher = NULL;
	m_pSaveLifeTrigger = NULL;
	m_iWorldContribution = 0;
	m_iWorldContributionSpend = 0;

	m_fPrayDistancePlus = 0.0f;
	m_dwReviveTime = 0;

	RandMallShoppingManager::Instance().Init(CECUIConfig::Instance().GetGameUI().nPokerShopConfig);
}

CECHostPlayer::~CECHostPlayer()
{
}

//	Initlaize object
bool CECHostPlayer::Init(const S2C::cmd_self_info_1& Info)
{
	m_PlayerInfo.cid	= Info.cid;
	m_PlayerInfo.crc_e	= Info.crc_e;
	m_PlayerInfo.crc_c	= Info.crc_c;

	m_BasicProps.iExp	= Info.iExp;
	m_BasicProps.iSP	= Info.iSP;	

	//	Create work manager
	if (!(m_pWorkMan = new CECHPWorkMan(this)))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECHostPlayer::Init", __LINE__);
		return false;
	}

	//	Create friend manager
	if (!(m_pFriendMan = new CECFriendMan))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECHostPlayer::Init", __LINE__);
		return false;
	}

	//	Create pet corral
	if (!(m_pPetCorral = new CECPetCorral))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECHostPlayer::Init", __LINE__);
		return false;
	}

	//	Create pet words
	if (!(m_pPetWords = new CECPetWords))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECHostPlayer::Init", __LINE__);
		return false;
	}
	m_pPetWords->LoadWords("configs\\petwords.txt");

	//	Create force manager
	if (!(m_pForceMgr = new CECForceMgr))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECHostPlayer::Init", __LINE__);
		return false;
	}

	//	Parse following data
	BYTE* pData = (BYTE*) &Info + sizeof (S2C::cmd_self_info_1);
	if (Info.state & GP_STATE_ADV_MODE)
		pData += sizeof (int) * 2;
	
	//	Parse shape data, must set before LoadResources()
	int iShape = 0;
	if (Info.state & GP_STATE_SHAPE)
	{
		iShape = *pData;
		pData += sizeof (BYTE);
	}
	SetShape(iShape);

	//	Load resources
	if (!LoadResources())
	{
		a_LogOutput(1, "CECHostPlayer::Init, Failed to load resources");
		return false;
	}

	// online award controller
	m_pOnlineAwardCtrl = new COnlineAwardCtrl(this);

	//	Get host's necessary data
	g_pGame->GetGameSession()->c2s_CmdGetAllData(true, true, false);

	//	Load GFX
	m_pMoveTargetGFX	= g_pGame->GetGFXCaster()->LoadGFXEx(res_GFXFile(RES_GFX_MOVETARGET));
	m_pSelectedGFX		= g_pGame->GetGFXCaster()->LoadGFXEx(res_GFXFile(RES_GFX_SELECTED));
	m_pHoverGFX			= g_pGame->GetGFXCaster()->LoadGFXEx(res_GFXFile(RES_GFX_CURSORHOVER));
	m_pFloatDust		= g_pGame->GetGFXCaster()->LoadGFXEx(res_GFXFile(RES_GFX_FLOATING_DUST));

	//	Set host's position and direction
	A3DVECTOR3 vPos(Info.pos), vNormal(0.0f);
	float fHei = g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(Info.pos, &vNormal);
	a_ClampFloor(vPos.y, fHei);
	SetPos(vPos);
	ChangeModelMoveDirAndUp(glb_DecompressDirH(Info.dir), g_vAxisY);
	m_MoveCtrl.SetHostLastPos(vPos);
	m_MoveCtrl.SetLastSevPos(vPos);

	//	Update upper body radius using new AABB
	m_CDRInfo.vExtent		= m_aabbServer.Extents;
	m_CDRInfo.vTPNormal		= vPos.y < fHei + 0.1f ? vNormal : g_vOrigin;
	m_CDRInfo.fYVel			= 0.0f;
	m_CDRInfo.fSlopeThresh	= EC_SLOPE_Y;
	m_CDRInfo.fStepHeight	= m_MoveConst.fStepHei;
	m_CDRInfo.vAbsVelocity.Clear();

	m_AirCDRInfo.vExtent	= m_aabbServer.Extents;
	m_AirCDRInfo.fUnderWaterDistThresh = m_MoveConst.fWaterSurf;

	//	Initialize camera control
	CECCamera::INIT_DATA InitData;
	InitData.fDistance	= 3.0f;
	InitData.vecDirCam	= g_vAxisZ;
	InitData.vecUpCam	= g_vAxisY;
	m_CameraCtrl.Init(&InitData);

	UpdateFollowCamera(false, 10);

	//	Get friend list and offline messages from server
	g_pGame->GetGameSession()->friend_GetList();
	g_pGame->GetGameSession()->friend_GetOfflineMsg();
	m_iGetFriendCnt = 60000;

	//	Load duel images.
	if( !LoadDuelImages() )
		a_LogOutput(1, "CECHostPlayer::Init, Failed to load duel images");

	//	Load sounds
	if (!LoadSounds())
		a_LogOutput(1, "CECHostPlayer::Init, Failed to load sounds");

	m_dwStates = Info.state;
	m_dwStates2 = Info.state2;
	
	// NOTICE: the data sequence must be same as the definition in server
	//GP_STATE_ADV_MODE
	//GP_STATE_SHAPE

	//	Parse emote info
	if (Info.state & GP_STATE_EMOTE)
	{
		pData += sizeof (BYTE);
	}

	//	Fill extend states
	DWORD aExtStates[OBJECT_EXT_STATE_COUNT] = {0};
	if (Info.state & GP_STATE_EXTEND_PROPERTY)
	{
		memcpy(aExtStates, pData, sizeof(aExtStates));
		pData += sizeof(aExtStates);
	}
	SetNewExtendStates(0, aExtStates, OBJECT_EXT_STATE_COUNT);

	//	Parse faction ID
	int idFaction = 0;
	if (Info.state & GP_STATE_FACTION)
	{
		idFaction = *(int*)pData;
		pData += sizeof (int);
		m_idFRole = *pData;
		pData++;
	}

	SetFactionID(idFaction);

	//	Parse booth crc flag
	if (Info.state & GP_STATE_BOOTH)
	{
		pData += sizeof (BYTE);
	}
	
	//	Parse effect data
	if (Info.state & GP_STATE_EFFECT)
	{
		BYTE byNum = *pData;
		pData += sizeof (BYTE);
		short* aEffects = (short*)pData;
		pData += byNum * sizeof (short);

		for (BYTE n=0; n < byNum; n++)
			ApplyEffect((int)aEffects[n], true);
	}

	//	Parse pariah data
	m_byPariahLvl = 0;
	if (Info.state & GP_STATE_PARIAH)
	{
		m_byPariahLvl = *pData;
		pData += sizeof (BYTE);
	}

	//	Parse mount data
	if (Info.state & GP_STATE_IN_MOUNT)
	{
		pData += sizeof (unsigned short);
		pData += sizeof (int);
	}

	//	Parse bind data
	if (Info.state & GP_STATE_IN_BIND)
	{
		pData += sizeof (char);
		m_idCandBuddy = *(int*)pData;
		pData += sizeof (int);
	}
	
	//	Parse spouse data
	m_idSpouse = 0;
	if (Info.state & GP_STATE_SPOUSE)
	{
		m_idSpouse = *(int*) pData;
		pData += sizeof(int);
	}

	//	Parse equip disabled mask
	m_i64EquipDisabled = 0;
	if (Info.state & GP_STATE_EQUIPDISABLED)
	{
		m_i64EquipDisabled = *(__int64*) pData;
		pData += sizeof(__int64);
	}

	if (Info.state & GP_STATE_PLAYERFORCE)
	{
		m_idForce = *(int*) pData;
		pData += sizeof(int);
	}

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
	
	if (Info.state & GP_STATE_COUNTRY)
	{
		SetCountry(*(int*) pData);
		pData += sizeof(int);
	}
	if (Info.state2 & GP_STATE2_TITLE)
	{
		SetCurrentTitle(*(unsigned short*) pData);
		pData += sizeof(unsigned short);
	}
	if (Info.state2 & GP_STATE2_REINCARNATION)
	{
		SetReincarnationCount(*(unsigned char*) pData);
		pData += sizeof(unsigned char);
	}
	if (Info.state2 & GP_STATE2_REALM)
	{
		SetRealmLevel(*(unsigned char*) pData);
		pData += sizeof(unsigned char);
	}
	unsigned char factionPVPMask = 0;
	if (Info.state2 & GP_STATE2_FACTION_PVP_MASK)
	{
		factionPVPMask = *(unsigned char*) pData;
		pData += sizeof(unsigned char);
	}
	SetFactionPVPMask(factionPVPMask);
	CECFactionPVPModel::Instance().OnJoinFactionPVP(IsInFactionPVP());

	if (m_idSpouse && !g_pGame->GetGameRun()->GetPlayerName(m_idSpouse, false))
		g_pGame->GetGameSession()->GetPlayerBriefInfo(1, &m_idSpouse, 2);
	
	//	Parse travel flag
	m_bFashionMode = false;
	if (Info.state & GP_STATE_FASHION)
		m_bFashionMode = true;

	//	Parse battle data
	m_iBattleCamp = GP_BATTLE_CAMP_NONE;
	if (Info.state & GP_STATE_BC_INVADER)
		m_iBattleCamp = GP_BATTLE_CAMP_INVADER;
	else if (Info.state & GP_STATE_BC_DEFENDER)
		m_iBattleCamp = GP_BATTLE_CAMP_DEFENDER;

	m_dwGMFlags = (Info.state & GP_STATE_GMFLAG) ? GMF_IAMGM : 0;
	m_pvp.bEnable = (Info.state & GP_STATE_PVPFLAG) ? true : false;
	m_pvp.bInPVPCombat = (Info.state & GP_STATE_INPVPCOMBAT) ? true : false;

	SetSelectable( !(Info.state & GP_STATE_FORBIDBESELECTED) );

	//	Stand and do nothing
	m_pWorkMan->StartWork_p0(m_pWorkMan->CreateWork(CECHPWork::WORK_STAND));
	if (IsDead())
	{
		CECHPWorkDead* pWork = (CECHPWorkDead*)m_pWorkMan->CreateWork(CECHPWork::WORK_DEAD);
		pWork->SetBeDeadFlag(true);
		m_pWorkMan->StartWork_p0(pWork);
	}
	else if (IsSitting())
	{
		CECHPWorkSit* pWork = (CECHPWorkSit*)m_pWorkMan->CreateWork(CECHPWork::WORK_SIT);
		pWork->SetBeSittingFlag(true);
		m_pWorkMan->StartWork_p1(pWork);
	}

	m_pSpritePortrait = new A2DSprite();
	A3DRECT rect;
	rect.left = 10;
	rect.top = 0;
	rect.right = 54;
	rect.bottom = 64;
	if( !m_pSpritePortrait->InitWithoutSurface(g_pGame->GetA3DDevice(), 64, 64, g_pGame->GetPortraitRender()->GetTextureFormat(), 1, &rect) )
	{
		a_LogOutput(1, "CECHostPlayer::Init, Failed to create host's portrait");
		return false;
	}
	m_pSpritePortrait->SetCurrentItem(0);
	m_pSpritePortrait->SetColor(0);

	//  Load favorite auctions
	LoadFavorAucItems();

	// 
	m_pOffShopCtrl = new COfflineShopCtrl(this);

	// 自动组队
	m_pAutoTeam = new CECAutoTeam();
	
	m_pChariot = new CChariot(this);
	m_pChariot->ExtractChariotFromData();

	// 创建PlayerWrapper
	CECAutoPolicy::GetInstance().OnEnterWorld();

	if (CECUIConfig::Instance().GetGameUI().bEnableActionSwitch)
		m_pActionSwitcher = new CECActionSwitcher(this);
	else
		m_pActionSwitcher = new CECActionSwitcherBase(this);

	return true;
}

bool CECHostPlayer::LoadDuelImages()
{
	// now create some sprites for duel usage.
	m_pDuelCountDown[0] = new A2DSprite();
	if( !m_pDuelCountDown[0]->Init(g_pGame->GetA3DDevice(), "ingame\\决斗一.dds", 0) )
	{
		a_LogOutput(1, "CECHostPlayer::Init, Failed to load duel count down 1");
		return false;
	}
	m_pDuelCountDown[0]->SetLocalCenterPos(64, 64);
	m_pDuelCountDown[0]->SetLinearFilter(true);

	m_pDuelCountDown[1] = new A2DSprite();
	if( !m_pDuelCountDown[1]->Init(g_pGame->GetA3DDevice(), "ingame\\决斗二.dds", 0) )
	{
		a_LogOutput(1, "CECHostPlayer::Init, Failed to load duel count down 2");
		return false;
	}
	m_pDuelCountDown[1]->SetLocalCenterPos(64, 64);
	m_pDuelCountDown[1]->SetLinearFilter(true);

	m_pDuelCountDown[2] = new A2DSprite();
	if( !m_pDuelCountDown[2]->Init(g_pGame->GetA3DDevice(), "ingame\\决斗三.dds", 0) )
	{
		a_LogOutput(1, "CECHostPlayer::Init, Failed to load duel count down 3");
		return false;
	}
	m_pDuelCountDown[2]->SetLocalCenterPos(64, 64);
	m_pDuelCountDown[2]->SetLinearFilter(true);

	m_pDuelStates[0] = new A2DSprite();
	if( !m_pDuelStates[0]->Init(g_pGame->GetA3DDevice(), "ingame\\决斗开始.dds", 0) )
	{
		a_LogOutput(1, "CECHostPlayer::Init, Failed to load duel state 1");
		return false;
	}
	m_pDuelStates[0]->SetLocalCenterPos(128, 64);
	m_pDuelStates[0]->SetLinearFilter(true);

	m_pDuelStates[1] = new A2DSprite();
	if( !m_pDuelStates[1]->Init(g_pGame->GetA3DDevice(), "ingame\\决斗胜利.dds", 0) )
	{
		a_LogOutput(1, "CECHostPlayer::Init, Failed to load duel state 2");
		return false;
	}
	m_pDuelStates[1]->SetLocalCenterPos(128, 64);
	m_pDuelStates[1]->SetLinearFilter(true);

	m_pDuelStates[2] = new A2DSprite();
	if( !m_pDuelStates[2]->Init(g_pGame->GetA3DDevice(), "ingame\\决斗失败.dds", 0) )
	{
		a_LogOutput(1, "CECHostPlayer::Init, Failed to load duel state 3");
		return false;
	}
	m_pDuelStates[2]->SetLocalCenterPos(128, 64);
	m_pDuelStates[2]->SetLinearFilter(true);
	return true;
}

void CECHostPlayer::ReleaseDuelImages()
{
	for(int i=0; i<3; i++)
	{
		A3DRELEASE(m_pDuelCountDown[i]);
		A3DRELEASE(m_pDuelStates[i]);
	}
}

//	Load host resources
bool CECHostPlayer::LoadResources()
{
	//	Get cached role information
	const GNET::RoleInfo& RoleInfo = g_pGame->GetGameRun()->GetSelectedRoleInfo();
	
	ASSERT(RoleInfo.roleid == m_PlayerInfo.cid);

	m_iProfession	= RoleInfo.occupation;
	m_iGender		= RoleInfo.gender;
	
	m_iRoleCreateTime	 = RoleInfo.create_time;
	m_iRoleLastLoginTime = RoleInfo.lastlogin_time;
	m_iAccountTotalCash	 = RoleInfo.cash_add;

	// compatible fix, force refresh shape id after get the prof and gender
	SetShape(GetShapeMask());

	//	Get player name and save into name cache
	SetName(ACString((const ACHAR*)RoleInfo.name.begin(), RoleInfo.name.size() / sizeof (ACHAR)));
	g_pGame->GetGameRun()->AddPlayerName(m_PlayerInfo.cid, m_strName);

	//	Calculate player's AABB
	CalcPlayerAABB();

	//	Load custom data
	if (RoleInfo.custom_data.size() >= 4)
	{
		if (!ChangeCustomizeData(PLAYER_CUSTOMIZEDATA::From(RoleInfo.custom_data.begin(), RoleInfo.custom_data.size()), false))
		{
			a_LogOutput(1, "CECHostPlayer::LoadResources, Failed to load custom data");
		//	return false;
		}
	}
	else
		SetResReadyFlag(RESFG_CUSTOM, true);

	// 在这个时候还不知道装备信息
	memset(m_aEquips, 0xff, sizeof(m_aEquips));

	//	Load player skeleton
	if (!LoadPlayerSkeleton(true))
	{
		a_LogOutput(1, "CECHostPlayer::LoadResources, Failed to load skeleton");
		return false;
	}

	//	Host's skin will be updated when inventory data (it's comming) is 
	//	got, so we neen't to update here.

	return true;
}
//  Add to favorite auction item list
bool CECHostPlayer::AddToFavorAucItem(int item)
{
	//  Check if item already exists
	int iIndex = m_aFavorAucItems.Find(item);

	if(iIndex < 0) // not exist
	{
		m_aFavorAucItems.Add(item); 
		return true;
	}
	else
		return false;
}
//  Remove auction item
bool CECHostPlayer::RemoveFavorAucItem(int item)
{
	int iIndex = m_aFavorAucItems.Find(item);
	
	if(iIndex >= 0)
	{
		m_aFavorAucItems.RemoveAt(iIndex);
		return true;
	}	
	return false;
}
//  Remove all auction items
void CECHostPlayer::RemoveAllFavorAucItems()
{
	m_aFavorAucItems.RemoveAll();
}
//  Save favorite auction list to local disk
bool CECHostPlayer::SaveFavorAucItems()
{
	if(m_aFavorAucItems.GetSize() ==0)
		return false;

	const wchar_t* szServerName = g_pGame->GetGameRun()->GetServerName();

	//  Get file name
	int cid = m_PlayerInfo.cid;
	char szFileName[255];
	AFile file;

	// Check if directory exists
	strcpy(szFileName, "userdata\\favorite_auction");
	if(GetFileAttributesA(szFileName) != FILE_ATTRIBUTE_DIRECTORY)
	{
		CreateDirectoryA(szFileName,NULL);
	}

	sprintf(szFileName, "userdata\\favorite_auction\\%s_%d.txt", AC2AS(szServerName), cid);

 	//  Save to file 
	if(!file.Open(szFileName, AFILE_TEXT | AFILE_CREATENEW))
	{
		ASSERT(0);
		return false;
	}

	//  Add version info
	char temp[20];
	sprintf(temp, "%d", FAVOR_AUCTION_VERSION);
	file.WriteLine(temp);

	//  Add server time
	sprintf(temp, "%d", g_pGame->GetServerGMTTime());
	file.WriteLine(temp);

	//  Add favorite auction data
	for(int i=0;i<m_aFavorAucItems.GetSize();i++)
	{
		sprintf(temp, "%d", m_aFavorAucItems[i]);
		file.WriteLine(temp);
	}

	file.Close();

	return true;
}

//  Load favorite auction list from local disk
bool CECHostPlayer::LoadFavorAucItems()
{
	const wchar_t* szServerName = g_pGame->GetGameRun()->GetServerName();

	int cid = m_PlayerInfo.cid;
	char szFileName[255];
	AFile file;

	//  Get file name
	sprintf(szFileName, "userdata\\favorite_auction\\%s_%d.txt", AC2AS(szServerName), cid);

	int iVersion;
	int iLastServerTime;
	if(file.Open(szFileName, AFILE_OPENEXIST))
	{
		char temp[255];
		unsigned long iLength;

		//  Get Version info
		file.ReadLine(temp, 255, &iLength);
		iVersion = atoi(temp);

		// Here we can check version information and do something for compatibility
		// if(iVersion != FAVOR_AUCTION_VERSION) 
		//	....

		//  Get server time when these auction items info saved
		file.ReadLine(temp, 255, &iLength);
		iLastServerTime = atoi(temp);

		int iCurrServerTime = g_pGame->GetServerGMTTime();
		// check if 24 hours has past
		// ...
		if(iCurrServerTime - iLastServerTime >= 24*3600)
			return false;

		while(file.ReadLine(temp,255,&iLength))
		{
			int id = atoi(temp);
			AddToFavorAucItem(id);
		}
	}

	file.Close();

	return true;
}
//	Release object
void CECHostPlayer::Release()
{
	CECInstanceReenter::Instance().Clear();
	CECShoppingItemsMover::Instance().Clear();
	CECFashionShopManager::Instance().Clear();
	CECShoppingManager::Instance().Clear();
	CECUseUniversalTokenCommandManager::Instance().Clear();
	CECUniversalTokenHTTPOSNavigatorTicketHandler::Instance().Clear();
	RandMallShoppingManager::Instance().Release();
	CECFactionPVPModel::Instance().Clear();
	CECHostSkillModel::Instance().Release();
	CECComboSkillState::Instance().Release();
	CECPlayerLevelRankRealmChangeCheck::Instance().Release();
	CECHostFashionEquipFromStorageSystem::Instance().Clear();

	m_pSaveLifeTrigger = NULL;
	CECQuickBuyPopManager::Instance().ClearPolicies();

	// 销毁PlayerWrapper
	CECAutoPolicy::GetInstance().OnLeaveWorld();

	//  Save favorite auction list first
	SaveFavorAucItems();

	//	Release duel images
	ReleaseDuelImages();

	//	Release sounds
	g_pGame->GetGameRun()->ReleaseSoundTable();
	m_pCurMoveSnd = NULL;

	//	Release friend manger
	if (m_pFriendMan)
	{
		delete m_pFriendMan;
		m_pFriendMan = NULL;
	}

	//	Release pet corral
	if (m_pPetCorral)
	{
		delete m_pPetCorral;
		m_pPetCorral = NULL;
	}

	if (m_pPetWords)
	{
		delete m_pPetWords;
		m_pPetWords = NULL;
	}

	if (m_pForceMgr)
	{
		delete m_pForceMgr;
		m_pForceMgr = NULL;
	}

	if (m_pOnlineAwardCtrl)
	{
		delete m_pOnlineAwardCtrl;
		m_pOnlineAwardCtrl = NULL;
	}

	if (m_pOffShopCtrl)
	{
		delete m_pOffShopCtrl;
		m_pOffShopCtrl = NULL;
	}

	if (m_pAutoTeam)
	{
		delete m_pAutoTeam;
		m_pAutoTeam = NULL;
	}

	if (m_pChariot)
	{
		delete m_pChariot;
		m_pChariot = NULL;
	}

	int i;

	//	Release all shortcuts
	for (i=0; i < NUM_HOSTSCSETS1; i++)
		A3DRELEASE(m_aSCSets1[i]);

	for (i=0; i < NUM_HOSTSCSETS2; i++)
		A3DRELEASE(m_aSCSets2[i]);

	for (i=0; i < NUM_SYSMODSETS; i++)
		A3DRELEASE(m_aSCSetSysMod[i]);

	//	Release all inventories
	A3DRELEASE(m_pPack);
	A3DRELEASE(m_pEquipPack);
	A3DRELEASE(m_pTrashBoxPack);
	A3DRELEASE(m_pTrashBoxPack2);
	A3DRELEASE(m_pTrashBoxPack3);
	A3DRELEASE(m_pAccountBoxPack);
	A3DRELEASE(m_pGeneralCardPack);
	A3DRELEASE(m_pTaskPack);
	A3DRELEASE(m_pDealPack);
	A3DRELEASE(m_pEPDealPack);
	A3DRELEASE(m_pTaskInterface);
	A3DRELEASE(m_pSpritePortrait);
	A3DRELEASE(m_pBuyPack);
	A3DRELEASE(m_pSellPack);
	A3DRELEASE(m_pBoothSPack);
	A3DRELEASE(m_pBoothBPack);
	A3DRELEASE(m_pEPBoothSPack);
	A3DRELEASE(m_pEPBoothBPack);
	A3DRELEASE(m_pEPEquipPack);
	A3DRELEASE(m_pClientGenCardPack);

	for (i=0; i < NUM_NPCIVTR; i++)
	{
		A3DRELEASE(m_aNPCPacks[i]);
	}

	//	Release all skills
	ReleaseSkills();

	//	Clear current combo skill
	ClearComboSkill();

	if (m_pWorkMan)
	{
		delete m_pWorkMan;
		m_pWorkMan = NULL;
	}

	m_CameraCtrl.Release();
	
	m_aTeamInvs.RemoveAll();

	g_pGame->GetGFXCaster()->ReleaseGFXEx(m_pMoveTargetGFX);
	g_pGame->GetGFXCaster()->ReleaseGFXEx(m_pSelectedGFX);
	g_pGame->GetGFXCaster()->ReleaseGFXEx(m_pHoverGFX);
	g_pGame->GetGFXCaster()->ReleaseGFXEx(m_pFloatDust);

	m_pMoveTargetGFX = NULL;
	m_pSelectedGFX = NULL;
	m_pHoverGFX = NULL;
	m_pFloatDust = NULL;

	//	Clear tab select table
	m_aTabSels.RemoveAll(false);
	
	m_aForceInfo.RemoveAll();

	if (m_pActionSwitcher)
	{
		delete m_pActionSwitcher;
		m_pActionSwitcher = NULL;
	}
	
	CECQShopConfig::Instance().ClearBuyedItem();

	A3DRELEASE(m_pNavigatePlayer);

	CECPlayer::Release();
}

void CECHostPlayer::SetNickname(const void* p, int len)
{
	 _cp_str(m_strNickname, p, len);
}

const FACTION_FORTRESS_CONFIG * CECHostPlayer::GetFactionFortressConfig()
{
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE dt = DT_INVALID;
	const FACTION_FORTRESS_CONFIG *pConfig =
		(const FACTION_FORTRESS_CONFIG *)pDataMan->get_data_ptr(854, ID_SPACE_CONFIG, dt);	
	if (!pConfig || dt != DT_FACTION_FORTRESS_CONFIG)
	{
		//	没有基地配置表
		pConfig = NULL;
	}
	return pConfig;
}

const CECHostPlayer::CONTRIB_INFO * CECHostPlayer::GetContribInfo()
{
	return &m_contribInfo;
}

const CECHostPlayer::FACTION_FORTRESS_INFO * CECHostPlayer::GetFactionFortressInfo()
{
	if (GetFactionID() > 0
		&& GetFactionID() == m_fortressInfo.faction_id)
		return &m_fortressInfo;
	return NULL;
}

const CECHostPlayer::FACTION_FORTRESS_ENTER * CECHostPlayer::GetFactionFortressEnter()
{
	return &m_fortressEnter;
}

void CECHostPlayer::SetFactionFortressBriefInfo(const void *pBriefInfo)
{
	using namespace GNET;

	const GNET::GFactionFortressBriefInfo* p1 = (const GNET::GFactionFortressBriefInfo*)pBriefInfo;
	if (!p1)
		return;

	FACTION_FORTRESS_INFO defaultInfo;
	m_fortressInfo = defaultInfo;

	m_fortressInfo.faction_id = p1->factionid;
	m_fortressInfo.level = p1->level;
	m_fortressInfo.health = p1->health;
}

const FACTION_BUILDING_ESSENCE * CECHostPlayer::GetBuildingEssence(int tid)
{	
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	
	DATA_TYPE dt = DT_INVALID;
	const FACTION_BUILDING_ESSENCE *pEssence =
		(const FACTION_BUILDING_ESSENCE *)pDataMan->get_data_ptr(tid, ID_SPACE_ESSENCE, dt);
	if (!pEssence || dt != DT_FACTION_BUILDING_ESSENCE)
		pEssence = NULL;

	return pEssence;
}

bool CECHostPlayer::IsInFortressWar()
{
	bool bInWar(false);
	if (IsInFortress())
	{
		int serverTime = g_pGame->GetServerGMTTime();
		if (m_fortressEnter.end_time > serverTime)
			bInWar = true;
	}
	return bInWar;
}

//	Create inventories
bool CECHostPlayer::CreateInventories()
{
	if (!(m_pPack = new CECInventory) ||
		!(m_pEquipPack = new CECInventory) ||
		!(m_pTrashBoxPack = new CECInventory) ||
		!(m_pTrashBoxPack2 = new CECInventory) ||
		!(m_pTrashBoxPack3 = new CECInventory) ||
		!(m_pAccountBoxPack = new CECInventory) ||
		!(m_pGeneralCardPack = new CECInventory) ||
		!(m_pTaskPack = new CECInventory) ||
		!(m_pDealPack = new CECDealInventory) ||
		!(m_pEPDealPack = new CECDealInventory) ||
		!(m_pBuyPack = new CECDealInventory) ||
		!(m_pSellPack = new CECDealInventory) ||
		!(m_pBoothSPack = new CECDealInventory) ||
		!(m_pBoothBPack = new CECDealInventory) ||
		!(m_pEPBoothSPack = new CECDealInventory) ||
		!(m_pEPBoothBPack = new CECDealInventory) ||
		!(m_pEPEquipPack = new CECInventory)||
		!(m_pClientGenCardPack = new CECInventory))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECHostPlayer::CreateInventories", __LINE__);
		return false;
	}

	for (int i=0; i < NUM_NPCIVTR; i++)
	{
		if (!(m_aNPCPacks[i] = new CECNPCInventory) || 
			!m_aNPCPacks[i]->Init(IVTRSIZE_NPCPACK))
			return false;
	}

	if (!m_pPack->Init(0) || !m_pEquipPack->Init(IVTRSIZE_EQUIPPACK) ||
		!m_pTaskPack->Init(IVTRSIZE_TASKPACK) || !m_pDealPack->Init(IVTRSIZE_DEALPACK) ||
		!m_pTrashBoxPack->Init(IVTRSIZE_TRASHBOX) || !m_pTrashBoxPack2->Init(0) ||
		!m_pTrashBoxPack3->Init(0) || !m_pEPDealPack->Init(IVTRSIZE_DEALPACK) ||
		!m_pAccountBoxPack->Init(0) || !m_pGeneralCardPack->Init(0) ||
		!m_pBuyPack->Init(IVTRSIZE_BUYPACK) || !m_pSellPack->Init(IVTRSIZE_SELLPACK) ||
		!m_pBoothSPack->Init(IVTRSIZE_BOOTHSPACK) || !m_pBoothBPack->Init(IVTRSIZE_BOOTHBPACK) ||
		!m_pEPBoothSPack->Init(IVTRSIZE_BOOTHSPACK_MAX) || !m_pEPBoothBPack->Init(IVTRSIZE_BOOTHBPACK_MAX) ||
		!m_pEPEquipPack->Init(IVTRSIZE_EQUIPPACK))
	{
		a_LogOutput(1, "CECHostPlayer::CreateInventories, Failed to initialize inventories");
		return false;
	}

	if(!m_pClientGenCardPack->Init(IVTRSIZE_CLIENTCARDPACK))
	{
		a_LogOutput(1, "CECHostPlayer::CreateInventories, Failed to initialize client general card pack");
		//return false;
		A3DRELEASE(m_pClientGenCardPack);
	}

	return true;
}

//	Create shortcut sets
bool CECHostPlayer::CreateShortcutSets()
{
	int i;

	for (i=0; i < NUM_HOSTSCSETS1; i++)
	{
		m_aSCSets1[i] = new CECShortcutSet;
		m_aSCSets1[i]->Init(SIZE_HOSTSCSET1);
	}

	for (i=0; i < NUM_HOSTSCSETS2; i++)
	{
		m_aSCSets2[i] = new CECShortcutSet;
		m_aSCSets2[i]->Init(SIZE_HOSTSCSET2);
	}

	for (i=0; i < NUM_SYSMODSETS; i++)
	{
		m_aSCSetSysMod[i] = new CECShortcutSet;
		m_aSCSetSysMod[i]->Init(SIZE_SYSMODSCSET);
	}
		
	return true;
}

//	Load sounds
bool CECHostPlayer::LoadSounds()
{
	//	预先加载的声音文件
	CECGameRun *pGameRun = g_pGame->GetGameRun();

	int iIndex = m_iProfession * NUM_GENDER + m_iGender;
	int idArray[] = {
		100 + iIndex,	//	走路声音 Walk
		130 + iIndex,	//	跑步声音 Run
		160,	//	游泳 Swim
		161,	//	水中悬浮 Float
		162,	//	跳跃 Jump
		163,	//	下落 Fall
		164,	//	安静 Quiet
	};

	for (int i(0); i < sizeof(idArray)/sizeof(idArray[0]); ++ i)
		pGameRun->GetOrLoadSoundFromTable(idArray[i]);
	
	m_pCurMoveSnd = NULL;
	
	return true;
}

//	Play move sound
void CECHostPlayer::PlayMoveSound(int iIndex)
{
	AMSoundBuffer* pNewBuf = g_pGame->GetGameRun()->GetOrLoadSoundFromTable(iIndex);
	if (pNewBuf == m_pCurMoveSnd)
		return;

	if (m_pCurMoveSnd)
		m_pCurMoveSnd->Stop();

	if (pNewBuf)
		pNewBuf->Play(true);

	m_pCurMoveSnd = pNewBuf;
}

//	Update move sound
void CECHostPlayer::UpdateMoveSound()
{
	static DWORD dwUpdateCnt = 0;
	dwUpdateCnt++;
	if (dwUpdateCnt % 3){
		return;
	}
	if (!m_pWorkMan->IsAnyWorkRunning()){
		return;
	}
	if (m_pWorkMan->IsMoving())
	{
		if (m_iMoveEnv == MOVEENV_GROUND)
		{
			if (IsJumping())
				PlayMoveSound(162);
			else if (!m_GndInfo.bOnGround)
				PlayMoveSound(163);
			else
			{
				int iIndex = m_iProfession * NUM_GENDER + m_iGender;
				int iSnd = m_bWalkRun ? (130+iIndex) : (100+iIndex);

				int iWalkRunOffset = (m_bWalkRun ? 1 : 0);

				if (IsShapeChanged() && (GetShapeID() == RES_MOD_ORC_FOX || GetShapeID() == RES_MOD_ORC_FOX2))
				{
					iSnd = 170 + iWalkRunOffset;
				}
				else if(IsShapeChanged() && GetShapeID() == RES_MOD_ORC_TIGER)
				{
					iSnd = 172 + iWalkRunOffset;
				}
				else
				{
					if( m_RidingPet.id )
					{
						DATA_TYPE dt;
						PET_ESSENCE * pData = (PET_ESSENCE *) g_pGame->GetElementDataMan()->get_data_ptr(m_RidingPet.id, ID_SPACE_ESSENCE, dt);
						if( dt == DT_PET_ESSENCE && pData )
							iSnd = 200 + pData->pet_snd_type*2 + iWalkRunOffset;
					}
				}

				PlayMoveSound(iSnd);
			}
		}
		else if (m_iMoveEnv == MOVEENV_WATER)
		{
			CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
			A3DVECTOR3 vEyePos = m_CameraCoord.GetPos();
			float fWaterHei = pWorld->GetWaterHeight(vEyePos);
			PlayMoveSound(vEyePos.y > fWaterHei ? 160 : 161);
		}
		else	//	m_iMoveEnv == MOVEENV_AIR
		{
			PlayMoveSound(164);
		}
	}
	else
		PlayMoveSound(164);
}

CECInventory* CECHostPlayer::GetPack(int iPack)
{
	CECInventory* pInventory = NULL;
	switch (iPack)
	{
	case IVTRTYPE_PACK:			pInventory = m_pPack;			break;
	case IVTRTYPE_EQUIPPACK:	pInventory = m_pEquipPack;		break;
	case IVTRTYPE_TASKPACK:		pInventory = m_pTaskPack;		break;
	case IVTRTYPE_TRASHBOX:		pInventory = m_pTrashBoxPack;	break;
	case IVTRTYPE_TRASHBOX2:	pInventory = m_pTrashBoxPack2;	break;
	case IVTRTYPE_TRASHBOX3:	pInventory = m_pTrashBoxPack3;	break;
	case IVTRTYPE_ACCOUNT_BOX:	pInventory = m_pAccountBoxPack;	break;
	case IVTRTYPE_GENERALCARD_BOX: pInventory = m_pGeneralCardPack; break;
	case IVTRTYPE_CLIENT_GENERALCARD_PACK: pInventory = m_pClientGenCardPack; break;
	default:
		ASSERT(0);
		return NULL;
	}
	return pInventory;
}

//	class CECHostPlayer::PackSorter
CECHostPlayer::PackSorter::PackSorter()
: m_pInventory(NULL)
{
}

void CECHostPlayer::PackSorter::SetInventory(CECInventory *pInventory){
	m_pInventory = pInventory;
}

CECIvtrItem *CECHostPlayer::PackSorter::GetItem(int index)const{
	return m_pInventory ? m_pInventory->GetItem(index) : 0;
}

//	class DefaultPackSorter
class DefaultPackSorter : public CECHostPlayer::PackSorter
{
public:
	virtual bool operator()(int index1, int index2)const
	{
		// Return whether index1 and index2
		// Index1 points to next
		// Index2 points to next
		CECIvtrItem *pItem1 = GetItem(index1);
		CECIvtrItem *pItem2 = GetItem(index2);
		if (!pItem1)
		{
			// whether item2 is empty or not, need not exchange
			return false;
		}
		if (!pItem2)
		{
			// item2 should be put after item1
			return true;
		}

		// Process same class id
		int cid1 = pItem1->GetClassID();
		int tid1 = pItem1->GetTemplateID();

		int cid2 = pItem2->GetClassID();
		int tid2 = pItem2->GetTemplateID();

		if (cid1 != cid2)
		{
			// For class we concern, sort by importance
			int cidOrder1 = GetClassIDOrder(cid1);
			int cidOrder2 = GetClassIDOrder(cid2);
			if (cidOrder1 != cidOrder2)
			{
				return cidOrder1 > cidOrder2;
			}

			// other different class according to class id
			return cid1 < cid2;
		}
		else
		{
			// For the same class id
			if (cid1 == CECIvtrItem::ICID_WEAPON)
			{
				// weapon
				CECIvtrWeapon *pWeapon1 = static_cast<CECIvtrWeapon *>(pItem1);
				const WEAPON_ESSENCE *pEssence1 = pWeapon1->GetDBEssence();

				CECIvtrWeapon *pWeapon2 = static_cast<CECIvtrWeapon *>(pItem2);
				const WEAPON_ESSENCE *pEssence2 = pWeapon2->GetDBEssence();

				// according to level if not equal
				if (pEssence1->level != pEssence2->level)
					return pEssence1->level > pEssence2->level;
			}
			else if (cid1 == CECIvtrItem::ICID_ARMOR)
			{
				// armor
				CECIvtrArmor *pWeapon1 = static_cast<CECIvtrArmor *>(pItem1);
				const ARMOR_ESSENCE *pEssence1 = pWeapon1->GetDBEssence();
				
				CECIvtrArmor *pWeapon2 = static_cast<CECIvtrArmor *>(pItem2);
				const ARMOR_ESSENCE *pEssence2 = pWeapon2->GetDBEssence();
				
				// according to level if not equal
				if (pEssence1->level != pEssence2->level)
					return pEssence1->level > pEssence2->level;
			}
			else if (cid1 == CECIvtrItem::ICID_GENERALCARD)
			{
				CECIvtrGeneralCard* pCard1 = dynamic_cast<CECIvtrGeneralCard*>(pItem1);
				CECIvtrGeneralCard* pCard2 = dynamic_cast<CECIvtrGeneralCard*>(pItem2);
				if (pCard1 && pCard2)
				{
					if (pCard1->GetEssence().rank > pCard2->GetEssence().rank)
						return true;
					else if (pCard1->GetEssence().rank == pCard2->GetEssence().rank)
						return pCard1->GetEssence().type < pCard2->GetEssence().type;
					else return false;
				}
			}
			// else according to template id			
			return tid1 < tid2;
		}
	}
private:
	int GetClassIDOrder(int cid)const
	{
		static const int s_CIDs[] = 
		{
			// weapon
			CECIvtrItem::ICID_WEAPON,
			CECIvtrItem::ICID_ARROW,
			CECIvtrItem::ICID_TOSSMAT,
			CECIvtrItem::ICID_ARMOR,
			CECIvtrItem::ICID_DECORATION,
			CECIvtrItem::ICID_BIBLE,
			CECIvtrItem::ICID_FLYSWORD,
			CECIvtrItem::ICID_WING,
			CECIvtrItem::ICID_GOBLIN,
			CECIvtrItem::ICID_GOBLIN_EQUIP,
			CECIvtrItem::ICID_FASHION,

			// medicine
			CECIvtrItem::ICID_AUTOHP,
			CECIvtrItem::ICID_AUTOMP,
			CECIvtrItem::ICID_MEDICINE,
			CECIvtrItem::ICID_SKILLMATTER,
			CECIvtrItem::ICID_TARGETITEM,

			// others
			CECIvtrItem::ICID_STONE,
			CECIvtrItem::ICID_PETEGG,
			CECIvtrItem::ICID_REFINETICKET,
			CECIvtrItem::ICID_DYETICKET,
			CECIvtrItem::ICID_GOBLIN_EXPPILL,
			CECIvtrItem::ICID_GENERALCARD,
			CECIvtrItem::ICID_GENERALCARD_DICE
		};
		static const int count = sizeof(s_CIDs)/sizeof(s_CIDs[0]);
		int order(0);
		for (int i(0); i<count; ++i)
		{
			if (cid == s_CIDs[i])
			{
				order = count-i;
				break;
			}
		}
		return order;
	}
};

class PackSortWrapper{
	CECHostPlayer::PackSorter *	m_pPackSorter;	
	DefaultPackSorter			m_defaultSorter;
public:
	PackSortWrapper(CECInventory *pInventory, CECHostPlayer::PackSorter *pPackSorter)
	{
		m_pPackSorter = pPackSorter ? pPackSorter : &m_defaultSorter;
		m_pPackSorter->SetInventory(pInventory);
	}
	bool operator()(int index1, int index2)const{
		return m_pPackSorter->operator ()(index1, index2);
	}
};

void CECHostPlayer::SortPack(int iPack, PackSorter *pPackSorter/* = NULL*/)
{
	// Check whether pack is valid
	CECInventory *pInventory = GetPack(iPack);
	if (!pInventory)
		return;

	int nIvtrSize = pInventory->GetSize();
	if (nIvtrSize <= 0)
		return;

	// Check whether there at least two items
	if (pInventory->GetEmptySlotNum() == nIvtrSize)
		return;

	CECIvtrItem *pItem = NULL;
	int i(0);

	// Check whether any item is frozen for use
	for (i = 0; i < nIvtrSize; ++ i)
	{
		pItem = pInventory->GetItem(i);
		if (pItem && pItem->IsFrozen())
			return;
	}

	// Freeze all item in pack first
	for (i = 0; i < nIvtrSize; ++ i)
	{
		if (pItem = pInventory->GetItem(i))
			pItem->Freeze(true);
	}

	// Get all item index for rearrange
	typedef abase::vector<int> ItemIndexVector;
	ItemIndexVector vecItem;
	vecItem.reserve(nIvtrSize);
	for (i = 0; i < nIvtrSize; ++ i)
		vecItem.push_back(i);

	// Sort item index
	BubbleSort(vecItem.begin(), vecItem.end(), PackSortWrapper(pInventory, pPackSorter));

	// Now vecItem stores index like 3,2,0,1,4
	// We need build exchange pairs for server so that the pairs transform 0,1,2,3,4 to 3,2,0,1,4 orderly

	ItemIndexVector vecExchange;

	// Firstly We get the transform pairs which restore 3,2,0,1,4 to 0,1,2,3,4
	i=0;
	while (i<nIvtrSize)
	{
		int j = vecItem[i];
		if (j == i)
		{
			// Need not change position
			// Turn to next
			++ i;
			continue;
		}

		// Need exchange pair [i, j]

		// Record exchange if valid
		int k = vecItem[j];
		if (pInventory->GetItem(j) || pInventory->GetItem(k))
		{
			// The exchange is valid when either item is not empty
			// to avoid needless exchange
			vecExchange.push_back(i);
			vecExchange.push_back(j);
		}

		abase::swap(vecItem[i], vecItem[j]);
	}

	// Then we get the reverse pairs and notify server
	if (!vecExchange.empty())
	{
		// Get reverse pairs
		i=0;
		int nPairSize = static_cast<int>(vecExchange.size());
		int j=nPairSize-1;
		while (i<j)
		{
			abase::swap(vecExchange[i++], vecExchange[j--]);
		}

		// Notify server
		g_pGame->GetGameSession()->c2s_CmdMultiExchangeItem(iPack, nPairSize/2, vecExchange.begin());
	}
	else
	{
		//	无需整理
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->AddChatMessage(pGameUI->GetStringFromTable(9161), GP_CHAT_SYSTEM);
	}

	// UnFreeze all items
	for (i = 0; i < nIvtrSize; ++ i)
	{
		if (pItem = pInventory->GetItem(i))
			pItem->Freeze(false);
	}
}

void CECHostPlayer::ValidatePackItemPointer()
{
	//	验证 Pack 中物品指针引用的有效性

	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (!pGameUI)	return;

	CECInventory *pPack = GetPack();
	CECInventory *pEquipPack = GetPack(IVTRTYPE_EQUIPPACK);
	CECDealInventory *pSell = GetSellPack();

	bool bShopShow = pGameUI->GetDialog("Win_Shop")->IsShow();
	if (bShopShow)
	{
		//	验证向 NPC 商店出售的自有物品的包裹
		for (int i(0); i < pSell->GetSize(); ++ i)
		{
			CECIvtrItem *pItemCopy = pSell->GetItem(i);
			if (!pItemCopy)	continue;

			const CECDealInventory::ITEMINFO &ii = pSell->GetItemInfo(i);

			CECIvtrItem *pItem = pPack->GetItem(ii.iOrigin);
			if (!pItem || pItemCopy != pItem)
			{
				//	物品已消失
				pSell->RemoveItemByIndex(i, ii.iAmount);
				continue;
			}
			if (pItem->GetCount() < ii.iAmount)
			{
				//	物品数量减少
				pSell->RemoveItemByIndex(i, ii.iAmount-pItem->GetCount());
			}
		}
	}

	//	数量输入框修改
	CDlgInputNO *pDlgInputNO = dynamic_cast<CDlgInputNO *>(pGameUI->GetDialog("Win_InputNO"));
	if (pDlgInputNO->IsShow())
	{
		int nInputNO = pDlgInputNO->GetType();
		switch (nInputNO)
		{
		case CDlgInputNO::INPUTNO_SELL_ADD:

			if (bShopShow)
			{
				//	向 NPC 商店添加待售物品

				int nCur(0), nMax(0);
				pDlgInputNO->GetInfo(nCur, nMax);

				PAUIOBJECT pObjSrc = (PAUIOBJECT)pDlgInputNO->GetDataPtr("ptr_AUIObject");
				int iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
				CECIvtrItem *pItem = pPack->GetItem(iSrc);
				if (!pItem)
				{
					//	物品消失
					pDlgInputNO->OnCommand_CANCEL("");
					break;
				}

				int sellCount(0);
				int iSlot = pSell->GetItemIndex(iSrc);
				if (iSlot >= 0)
				{
					const CECDealInventory::ITEMINFO &ii = pSell->GetItemInfo(iSlot);
					sellCount = pItem->GetCount() - ii.iAmount;
				}
				else sellCount = pItem->GetCount();
				if (nMax != sellCount)
				{
					//	数量减少
					nMax = sellCount;
					nCur = a_Min(nCur, nMax);
					pDlgInputNO->SetInfo(nCur, nMax);
					break;
				}
			}
			break;

		case CDlgInputNO::INPUTNO_SELL_REMOVE:

			if (bShopShow)
			{
				//	从 NPC 商店移除待售物品

				int nCur(0), nMax(0);
				pDlgInputNO->GetInfo(nCur, nMax);

				PAUIOBJECT pObj = (PAUIOBJECT)pDlgInputNO->GetDataPtr("ptr_AUIObject");
				int iSrc = atoi(pObj->GetName() + strlen("Sell_")) - 1;
				const CECDealInventory::ITEMINFO &ii = pSell->GetItemInfo(iSrc);
				if (!pSell->GetItem(iSrc) || ii.iAmount <= 0)
				{
					//	物品消失
					pDlgInputNO->OnCommand_CANCEL("");
					break;
				}
				if (nMax != ii.iAmount)
				{
					//	数量减少
					nMax = ii.iAmount;
					nCur = a_Min(nCur, nMax);
					pDlgInputNO->SetInfo(nCur, nMax);
					break;
				}
			}
			break;
		}
	}

	PAUIDIALOG pDlgDrag = pGameUI->GetDialog("DragDrop");
	if( pDlgDrag->IsShow() )
	{
		//	检查 DragDrop 中控件是否包含了已消失的物品指针

		PAUIOBJECT pCell = pDlgDrag->GetDlgItem("Goods");
		PAUIOBJECT pObjSrc = (PAUIOBJECT)pCell->GetDataPtr("ptr_AUIObject");
		while (pObjSrc)
		{
			//	获取 DragDrop 中保存的物品指针
			void *ptr = NULL;
			AString szType;
			pDlgDrag->ForceGetDataPtr(ptr, szType);
			if (szType != "ptr_CECIvtrItem") break;

			//	限定检查普通包裹的物品
			PAUIDIALOG pDlgSrc = pObjSrc->GetParent();
			if (pDlgSrc != pGameUI->GetDialog("Win_Inventory") &&
				pDlgSrc != pGameUI->GetDialog("Win_Bag"))
				break;

			if (strstr(pObjSrc->GetName(), "Item_") != pObjSrc->GetName())
				break;

			//	获取普通包裹中对应物品的当前指针
			int iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
			CECIvtrItem *pItem = pPack->GetItem(iSrc);

			//	验证
			if (!pItem || ptr != pItem)
				pDlgDrag->OnCommand("IDCANCEL");

			break;
		}
	}
}

//	Get detail data of specified item
void CECHostPlayer::GetIvtrItemDetailData(int iPack, int iSlot)
{
	CECInventory* pPack = GetPack(iPack);
	if (!pPack) return;
	CECIvtrItem* pItem = pPack->GetItem(iSlot);
	if (pItem)
		pItem->GetDetailDataFromSev(iPack, iSlot);
}

bool CECHostPlayer::UseEquipment(int iSlot)
{
	if (!CanDo(CANDO_USEITEM))
		return false;

	CECInventory *pEquipPack = GetPack(IVTRTYPE_EQUIPPACK);
	if (!pEquipPack)
		return false;
	CECIvtrItem *pItem = pEquipPack->GetItem(iSlot);
	if (!pItem)
		return false;

	CECGameSession* pSession = g_pGame->GetGameSession();
	pSession->c2s_CmdUseItem((BYTE)IVTRTYPE_EQUIPPACK, (BYTE)iSlot, pItem->GetTemplateID(), 1);
	return true;
}

//	Use specified inventory item in pack
bool CECHostPlayer::UseItemInPack(int iPack, int iSlot, bool showMsg/* = true*/)
{
	//	Reject this action if we are in some special states
	if (!CanDo(CANDO_USEITEM))
		return false;

	CECInventory* pPack = GetPack(iPack);
	if (!pPack) return false;
	CECIvtrItem* pItem = pPack->GetItem(iSlot);
	if (!pItem || pItem->IsFrozen())
		return false;

	if (pItem->Use_Persist() && (IsJumping() || IsFalling()))
		return false;

	CECGameSession* pSession = g_pGame->GetGameSession();
	CECGameRun* pGameRun = g_pGame->GetGameRun();

	//  Is item disabled in mini client ?
	if( (g_pGame->GetConfigs()->IsMiniClient() || glb_IsInMapForMiniClient()) &&
		CECUIConfig::Instance().GetGameUI().IsItemDisabledInMiniClient(pItem->GetTemplateID()) )
	{
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if( pGameUI ) pGameUI->MessageBox("", pGameUI->GetStringFromTable(10713), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return false;
	}

	//  Is item an firework?
	if (pItem->GetClassID() == CECIvtrItem::ICID_FIREWORK)
	{
		if(GetProfession() == PROF_GHOST && IsInvisible()) // 刺客隐身时无法播放烟花
		{
			if (showMsg) pGameRun->AddFixedMessage(FIXMSG_CANNOT_USE_WHEN_INVISIBLE);
			return false;
		}
	}	
	
	if (pItem->GetClassID() == CECIvtrItem::ICID_INCSKILLABILITY)
	{
		const INC_SKILL_ABILITY_ESSENCE *pDBEssence = static_cast<CECIvtrIncSkillAbility *>(pItem)->GetDBEssence();
		CECSkill *pSkill = GetNormalSkill(pDBEssence->id_skill);
		if (pSkill)
		{
			if (pSkill->GetSkillLevel() != pDBEssence->level_required)
			{
				if (showMsg) pGameRun->AddFixedMessage(FIXMSG_PRODUCE_LEVEL_INVALID);
				return false;
			}
			if (GetSkillAbilityPercent(pDBEssence->id_skill) >= 100)
			{
				if (showMsg) pGameRun->AddFixedMessage(FIXMSG_PRODUCE_ABILITY_FULL);
				return false;
			}
		}
	}

	//	Is item an transmitscroll ?
	if( pItem->GetClassID() == CECIvtrItem::ICID_TRANSMITSCROLL )
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if( pGameUI && !IsFighting() )
		{
			CDlgWorldMap *pMap = (CDlgWorldMap*)pGameUI->GetDialog("Win_WorldMapTravel");
			pMap->BuildTravelMap(DT_TRANSMITSCROLL_ESSENCE, (void*)iSlot);
			pMap->Show(true);
		}
		return true;
	}

	if (pItem->GetClassID() == CECIvtrItem::ICID_SHOPTOKEN)
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if(pGameUI && !IsFighting()) {
			CDlgTokenShop* pDlg = dynamic_cast<CDlgTokenShop*>(pGameUI->GetDialog("Win_TokenShop"));
			if (pDlg){
				pDlg->InitTokenShopItem(pItem->GetTemplateID());
				pDlg->Show(!pDlg->IsShow());
			}
		}
		return true;
	}

	if (pItem->GetClassID() == CECIvtrItem::ICID_UNIVERSAL_TOKEN){
		CECIvtrUniversalToken *pUniversalToken = dynamic_cast<CECIvtrUniversalToken *>(pItem);
		if (pUniversalToken->HasAnyUsage()){
			CECUseUniversalTokenCommandManager::Instance().Use(pUniversalToken, pUniversalToken->UsageIndexAt(0));	//	选择头一个Usage执行
			return true;
		}
	}
	
	if (pItem->GetClassID() == CECIvtrItem::ICID_TASKDICE)
	{
		CECIvtrTaskDice* pTaskDice = (CECIvtrTaskDice*)pItem;
		
		//  是否能够在战斗中使用？
		if (IsFighting() && pTaskDice->GetDBEssence()->no_use_in_combat == 1)
		{
			if (showMsg) pGameRun->AddFixedMessage(FIXMSG_CANNOT_USE_IN_BATTLE);
			return false;
		}
	}
	
	//	Is item a target item (use item is similar to casting skill to target)?
	if( pItem->GetClassID() == CECIvtrItem::ICID_TARGETITEM )
	{
		CECIvtrTargetItem* pTargetItem = (CECIvtrTargetItem*)pItem;

		//  是否能够在战斗中使用？
		if( IsFighting() && pTargetItem->GetDBEssence()->use_in_combat == 0)
		{
			if (showMsg) pGameRun->AddFixedMessage(FIXMSG_CANNOT_USE_IN_BATTLE);
			return false;
		}
		
		//	是否只能在安全区使用
		if (pTargetItem->GetDBEssence()->use_in_sanctuary_only && !IsInSanctuary())
		{
			if (showMsg) pGameRun->AddFixedMessage(FIXMSG_USE_IN_SANCTUARY_ONLY);
			return false;
		}

		//  能否在当前地图中使用？
		int i, iCurrMap = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
		if(pTargetItem->GetDBEssence()->num_area != 0)
		{
			for(i=0;i<pTargetItem->GetDBEssence()->num_area;i++)
			{
				if(pTargetItem->GetDBEssence()->area_id[i] == iCurrMap)
					break;
			}
			if(i == pTargetItem->GetDBEssence()->num_area)
			{
				if (showMsg) pGameRun->AddFixedMessage(FIXMSG_CANNOT_USE_IN_CURR_MAP);
				return false;
			}
		}

		//  物品对应技能能否释放？
		if (!CanDo(CANDO_SPELLMAGIC))
			return false;

		if (InSlidingState())
			return false;

		if (!m_idSelTarget)
			return false;

		CECSkill* pSkill = pTargetItem->GetTargetSkill();
		if(!pSkill)
			return false;

		//	If we press a chargeable skill again when it's being charged, 
		//	we cast it out at once
		if (IsSpellingMagic() && m_pCurSkill && m_pCurSkill->IsCharging() &&
			m_pCurSkill->GetSkillID() == pSkill->GetSkillID())
		{
			m_pCurSkill->EndCharging();
			g_pGame->GetGameSession()->c2s_CmdContinueAction();
			return true;
		}

		int iCon = CheckSkillCastCondition(pSkill);
		if (iCon)
		{
			if (showMsg) ProcessSkillCondition(iCon);
			return false;
		}

		//	Get force attack flag
		bool bForceAttack = glb_GetForceAttackFlag(NULL);
		//	Check negative effect skill
		if (pSkill->GetType() == CECSkill::TYPE_ATTACK || pSkill->GetType() == CECSkill::TYPE_CURSE)
		{
			if (m_idSelTarget == m_PlayerInfo.cid)
			{
				//	Host cannot spell negative effect magic to himself.
				if (showMsg) g_pGame->GetGameRun()->AddFixedChannelMsg(FIXMSG_TARGETWRONG, GP_CHAT_FIGHT);
				return false;
			}
			else if (m_idSelTarget)
			{
				if (AttackableJudge(m_idSelTarget, bForceAttack) != 1)
					return false;
			}
		}

		//	Check whether target type match
		int idCastTarget = m_idSelTarget;
		int iTargetType = pSkill->GetTargetType();

		if (pSkill->GetType() == CECSkill::TYPE_BLESS ||
			pSkill->GetType() == CECSkill::TYPE_NEUTRALBLESS)
		{
			if (!iTargetType || !ISPLAYERID(m_idSelTarget))
				idCastTarget = m_PlayerInfo.cid;

			//	In some case, we shouldn't add bless effect to other players
			if (ISPLAYERID(idCastTarget) && idCastTarget != m_PlayerInfo.cid)
			{
				//  If host has set bless skill filter only to himself, bless skill couldn't add to other players
				BYTE byBLSMask = glb_BuildBLSMask();

				if(	pSkill->GetRangeType() == CECSkill::RANGE_POINT )
				{
					if(!IsTeamMember(idCastTarget))
					{
						if(byBLSMask & GP_BLSMASK_SELF)
							idCastTarget = m_PlayerInfo.cid;
						else
						{
							CECElsePlayer* pPlayer = (CECElsePlayer*)g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(idCastTarget);
							if (!pPlayer)
							{
								// 目标消失
								return false;
							}

							if(pPlayer->IsInvader() || pPlayer->IsPariah())
							{
								if(byBLSMask & GP_BLSMASK_NORED)
									idCastTarget = m_PlayerInfo.cid;
							}

							if (!IsFactionMember(pPlayer->GetFactionID()))
							{
								if(byBLSMask & GP_BLSMASK_NOMAFIA)
									idCastTarget = m_PlayerInfo.cid;
							}
							
							if (!IsFactionAllianceMember(pPlayer->GetFactionID()))
							{
								if(byBLSMask & GP_BLSMASK_NOALLIANCE)
									idCastTarget = m_PlayerInfo.cid;
							}
							if (GetForce() != pPlayer->GetForce())
							{
								if(byBLSMask & GP_BLSMASK_NOFORCE)
									idCastTarget = m_PlayerInfo.cid;
							}
						}
					}
				}

				//	If host is in duel, bless skill couldn't add to opponent
				if (IsInDuel() && m_idSelTarget == m_pvp.idDuelOpp)
					idCastTarget = m_PlayerInfo.cid;
				
				//	If host is in battle, bless skill couldn't add to enemies
				if (IsInBattle())
				{
					CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(idCastTarget);
					if (!InSameBattleCamp(pPlayer))
						idCastTarget = m_PlayerInfo.cid;
				}
			}
		}
		else if (pSkill->GetType() == CECSkill::TYPE_BLESSPET)
		{
			CECPet* pPet = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetPetByID(m_idSelTarget);
			if (!pPet || pPet->GetMasterID() == GetCharacterID())
			{
				//	Spell skill on host's pet
				CECPetData* pPetData = m_pPetCorral->GetActivePet();
				if (!pPetData ||
					pPetData->GetClass() != GP_PET_CLASS_COMBAT &&
					pPetData->GetClass() != GP_PET_CLASS_SUMMON &&
					pPetData->GetClass() != GP_PET_CLASS_EVOLUTION)
					return false;
				
				idCastTarget = m_pPetCorral->GetActivePetNPCID();
			}
			//	Only fighting pet can be blessed.
			if( pPet && !pPet->CanBeAttacked() )
				return false;
		}
		else
		{
			if (iTargetType != 0 && !idCastTarget)
				return false;
		}

		//	iTargetType == 4 means target must be pet. The problem is that pet will
		//	disappear from world after it died, so GetWorld()->GetObject() will return
		//	NULL when host spells revive-pet skill on his dead pet. So, the target
		//	type of revive-pet skill should be 0
		if (iTargetType)
		{
			//	Target shoundn't be a corpse ?
			int iAliveFlag = 0;
			if (iTargetType == 1)
				iAliveFlag = 1;
			else if (iTargetType == 2)
				iAliveFlag = 2;

			CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(idCastTarget, iAliveFlag);
			if (!pObject)
				return false;
		}	

		if (!IsMeleeing() && !IsSpellingMagic() &&
			(!iTargetType || idCastTarget == m_PlayerInfo.cid))
		{
			//	Cast this skill need't checking cast distance
			if (!pSkill->ReadyToCast())
				return false;

			//	Prepare to cast skill, if skill isn't INSTANT and FLASHMOVE, 
			//	we must stop moving and stand
			if (!pSkill->IsInstant() && pSkill->GetType() != CECSkill::TYPE_FLASHMOVE)
			{
				if (!NaturallyStopMoving())
					return false;	//	Couldn't stop naturally, so cancel casting skill
			}
			else if (pSkill->GetType() == CECSkill::TYPE_FLASHMOVE)
			{
				if (!CanDo(CANDO_FLASHMOVE))
					return false;
			}

			m_pPrepSkill = pSkill;
//			CastSkill(m_PlayerInfo.cid, bForceAttack);
		}
		else if (IsSpellingMagic() && m_pCurSkill == pSkill && !pSkill->ReadyToCast())
		{
			//	If we are casting the same skill and it's in cooling time
			return false;
		}

	}

	//	Is item an equipment ?
	if (pItem->IsEquipment())
	{
		if (iPack == IVTRTYPE_EQUIPPACK)
		{
			//	Take off equipment, search an empty place at first
			int iEmpty = m_pPack->SearchEmpty();
			if (iEmpty < 0)
				return false;

			pSession->c2s_CmdEquipItem(iEmpty, iSlot);
			return true;
		}

		//	Can equip this equipment ?
		if (!CanUseEquipment((CECIvtrEquip*)pItem))
			return false;

		//	Find the first free slot this equipment can be equipped
		int i, iDst, iFirstFree=-1, iFirstCan=-1;
		for (i=0; i < SIZE_ALL_EQUIPIVTR; i++)
		{
			if (pItem->CanEquippedTo(i))
			{
				if (iFirstCan < 0)
					iFirstCan = i;

				if (!m_pEquipPack->GetItem(i) && iFirstFree < 0)
				{
					iFirstFree = i;
					break;
				}
			}
		}

		if (iFirstFree >= 0)
			iDst = iFirstFree;
		else if (iFirstCan >= 0)
			iDst = iFirstCan;
		else
		{
			ASSERT(0);
			return false;
		}

		if (pItem->GetClassID() == CECIvtrItem::ICID_DYNSKILLEQUIP)
		{
			//	技能装备同时只能存在一个
			int iSameIDPos = m_pEquipPack->FindItem(pItem->GetTemplateID());
			if (iSameIDPos >= 0)
				iDst = iSameIDPos;
		}

		//	Equip item
		if (pItem->GetClassID() == CECIvtrItem::ICID_ARROW ||
			pItem->GetClassID() == CECIvtrItem::ICID_DYNSKILLEQUIP)
		{
			CECIvtrItem* pDstItem = m_pEquipPack->GetItem(iDst);
			if (!pDstItem || pItem->GetTemplateID() != pDstItem->GetTemplateID())
				pSession->c2s_CmdEquipItem(iSlot, iDst);
			else
				pSession->c2s_CmdMoveItemToEquip(iSlot, iDst);
		}
		else {
			if (pItem->GetClassID() == CECIvtrItem::ICID_GENERALCARD) {
				CECIvtrGeneralCard* pCard = dynamic_cast<CECIvtrGeneralCard*>(pItem);
				if (pCard) iDst = EQUIPIVTR_GENERALCARD1 + pCard->GetEssence().type;
			}
			pSession->c2s_CmdEquipItem(iSlot, iDst);
		} 

		return true;
	}

	if (iPack != IVTRTYPE_PACK)
		return false;	//	Non-equipment must be in normal package

	if (!pItem->CheckUseCondition())
	{
		if (showMsg) pGameRun->AddFixedMessage(FIXMSG_ITEM_CANNOTUSE);
		return false;
	}

	if (pItem->GetCoolTime())
	{
		if (showMsg) pGameRun->AddFixedMessage(FIXMSG_ITEM_INCOOLTIME);
		return false;
	}

	//	Using item to attack a target ?
	if (pItem->Use_AtkTarget() || pItem->Use_Target())
	{
		if (pItem->Use_AtkTarget() && CannotAttack())
			return false;

		CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();

		if (!m_idSelTarget || m_idSelTarget == m_PlayerInfo.cid)
		{
			if (showMsg) pGameRun->AddChatMessage(pStrTab->GetWideString(FIXMSG_NOTARGET), GP_CHAT_SYSTEM);
			return false;
		}

		//	Check whether the target is too far away ?
		//	TODO: Should we completely depend on server to tell us this ?
		float fDist, fTargetRad, fAttackRange = 10000.0f;
		if (pItem->GetClassID() == CECIvtrItem::ICID_TOSSMAT)
			fAttackRange = ((CECIvtrTossMat*)pItem)->GetDBEssence()->attack_range;
		else if (pItem->GetClassID() == CECIvtrItem::ICID_TANKCALLIN)
			fAttackRange = 5.0f;
		else if (pItem->GetClassID() == CECIvtrItem::ICID_TARGETITEM)
		{
			CECIvtrTargetItem* pTargetItem = (CECIvtrTargetItem*)pItem;
			fAttackRange = ((CECIvtrTargetItem*)pItem)->GetTargetSkill()->GetCastRange(m_ExtProps.ak.attack_range, GetPrayDistancePlus());
		}

		CECObject* pObject;
		if (!CalcDist(m_idSelTarget, &fDist, &pObject))
			return false;

		if (ISNPCID(m_idSelTarget))
			fTargetRad = ((CECNPC*)pObject)->GetTouchRadius();
		else if (ISPLAYERID(m_idSelTarget))
			fTargetRad = ((CECElsePlayer*)pObject)->GetTouchRadius();

		if (fDist - fTargetRad > fAttackRange * 0.8f)
		{
			if (showMsg) pGameRun->AddChatMessage(pStrTab->GetWideString(FIXMSG_TARGETISFAR), GP_CHAT_SYSTEM);
			return false;
		}

		BYTE byPVPMask = glb_BuildPVPMask(glb_GetForceAttackFlag(NULL));
		pSession->c2s_CmdUseItemWithTarget((BYTE)iPack, (BYTE)iSlot, pItem->GetTemplateID(), byPVPMask);
	}
	else
	{
		if( pItem->GetClassID() == CECIvtrItem::ICID_DOUBLEEXP )
		{
			CECIvtrDoubleExp *pDoubleExp = (CECIvtrDoubleExp*)pItem;
			if( pDoubleExp->GetDBEssence()->double_exp_time + g_pGame->GetGameRun()->GetRemainDblExpTime() > 3600 * 4 )
			{
				if (showMsg)
				{
					CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
					pGameUI->MessageBox("", pGameUI->GetStringFromTable(828), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				}
				return false;
			}
		}

		if (pItem->GetClassID() == CECIvtrItem::ICID_SHARPENER)
		{
			if (showMsg) pGameRun->AddFixedMessage(FIXMSG_SHARPEN_ON_DRAG);
			return false;
		}

		pSession->c2s_CmdUseItem((BYTE)iPack, (BYTE)iSlot, pItem->GetTemplateID(), 1);
	}
	
	return true;
}

//	Apply shortcut of specified skill
bool CECHostPlayer::ApplySkillShortcut(int idSkill, bool bCombo/* false */, 
								int idSelTarget/* 0 */, int iForceAtk/* -1 */)
{
	StackChecker::ACTrace(4);

	if (m_pActionSwitcher)
		m_pActionSwitcher->PostMessge(CECActionSwitcherBase::MSG_CASTSKILL);

	//	Return-town skill is very special, handle it separately
	if (idSkill == ID_RETURNTOWN_SKILL)
		return ReturnToTargetTown(0, bCombo);

	if (idSkill == ID_SUMMONPLAYER_SKILL)
		return SummonPlayer(idSelTarget, bCombo);

	if (!CanDo(CANDO_SPELLMAGIC))
		return false;

	if (InSlidingState())
		return false;

	if (!bCombo)
		ClearComboSkill();

	if (!idSelTarget)
		idSelTarget = m_idSelTarget;

	CECSkill* pSkill = GetPositiveSkillByID(idSkill);
	if (!pSkill) pSkill = GetEquipSkillByID(idSkill);
	if (!pSkill) pSkill = CECComboSkillState::Instance().GetInherentSkillByID(idSkill);
	if (!pSkill)
	{
		ASSERT(0);
		return false;
	}

	//	If we press a chargeable skill again when it's being charged, 
	//	we cast it out at once
	if (IsSpellingMagic() && m_pCurSkill && m_pCurSkill->IsCharging() &&
		m_pCurSkill->GetSkillID() == pSkill->GetSkillID())
	{
		m_pCurSkill->EndCharging();
		g_pGame->GetGameSession()->c2s_CmdContinueAction();
		return true;
	}

	int iCon = CheckSkillCastCondition(pSkill);
	if (iCon)
	{
		ProcessSkillCondition(iCon);
		return false;
	}

	//	Get force attack flag
	bool bForceAttack;
	if (iForceAtk < 0)
		bForceAttack = glb_GetForceAttackFlag(NULL);
	else
		bForceAttack = iForceAtk > 0 ? true : false;

	//	Check negative effect skill
	if (pSkill->GetType() == CECSkill::TYPE_ATTACK || pSkill->GetType() == CECSkill::TYPE_CURSE)
	{
		if (idSelTarget == m_PlayerInfo.cid)
		{
			//	Host cannot spell negative effect magic to himself.
			g_pGame->GetGameRun()->AddFixedChannelMsg(FIXMSG_TARGETWRONG, GP_CHAT_FIGHT);
			return false;
		}
		else if (idSelTarget)
		{
			if (AttackableJudge(idSelTarget, bForceAttack) != 1)
				return false;
		}
	}

	//	Check whether target type match
	int idCastTarget = idSelTarget;
	int iTargetType = pSkill->GetTargetType();

	if (pSkill->GetType() == CECSkill::TYPE_BLESS ||
		pSkill->GetType() == CECSkill::TYPE_NEUTRALBLESS)
	{
		if (!iTargetType || !ISPLAYERID(idSelTarget))
			idCastTarget = m_PlayerInfo.cid;

		//	In some case, we shouldn't add bless effect to other players
		if (ISPLAYERID(idCastTarget) && idCastTarget != m_PlayerInfo.cid)
		{
			//  If host has set bless skill filter only to himself, bless skill couldn't add to other players
			BYTE byBLSMask = glb_BuildBLSMask();

			if(	pSkill->GetRangeType() == CECSkill::RANGE_POINT )
			{
				if(!IsTeamMember(idCastTarget))
				{
					if(byBLSMask & GP_BLSMASK_SELF)
						idCastTarget = m_PlayerInfo.cid;
					else
					{
						CECElsePlayer* pPlayer = (CECElsePlayer*)g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(idCastTarget);
						if (!pPlayer)
						{
							// 目标消失
							return false;
						}

						if(pPlayer->IsInvader() || pPlayer->IsPariah())
						{
							if(byBLSMask & GP_BLSMASK_NORED)
								idCastTarget = m_PlayerInfo.cid;
						}

						if (!IsFactionMember(pPlayer->GetFactionID()))
						{
							if(byBLSMask & GP_BLSMASK_NOMAFIA)
								idCastTarget = m_PlayerInfo.cid;
						}

						if (!IsFactionAllianceMember(pPlayer->GetFactionID()))
						{
							if(byBLSMask & GP_BLSMASK_NOALLIANCE)
								idCastTarget = m_PlayerInfo.cid;
						}
						if(GetForce() != pPlayer->GetForce())
						{
							if(byBLSMask & GP_BLSMASK_NOFORCE)
								idCastTarget = m_PlayerInfo.cid;
						}
					}
				}
			}

			//	If host is in duel, bless skill couldn't add to opponent
			if (IsInDuel() && idSelTarget == m_pvp.idDuelOpp)
				idCastTarget = m_PlayerInfo.cid;
			
			//	If host is in battle, bless skill couldn't add to enemies
			if (IsInBattle())
			{
				CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(idCastTarget);
				if (!InSameBattleCamp(pPlayer))
					idCastTarget = m_PlayerInfo.cid;
			}
		}
	}
	else if (pSkill->GetType() == CECSkill::TYPE_BLESSPET)
	{
		CECPet* pPet = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetPetByID(idSelTarget);
		if (!pPet || pPet->GetMasterID() == GetCharacterID())
		{
			//	Spell skill on host's pet
			CECPetData* pPetData = m_pPetCorral->GetActivePet();
			if (!pPetData ||
				pPetData->GetClass() != GP_PET_CLASS_COMBAT &&
				pPetData->GetClass() != GP_PET_CLASS_SUMMON &&
				pPetData->GetClass() != GP_PET_CLASS_EVOLUTION)
				return false;
			
			idCastTarget = m_pPetCorral->GetActivePetNPCID();
		}
		//	Only fighting pet can be blessed.
		if( pPet && !pPet->CanBeAttacked() )
			return false;
	}
	else
	{
		if (iTargetType != 0 && !idCastTarget)
			return false;
	}

	//	iTargetType == 4 means target must be pet. The problem is that pet will
	//	disappear from world after it died, so GetWorld()->GetObject() will return
	//	NULL when host spells revive-pet skill on his dead pet. So, the target
	//	type of revive-pet skill should be 0
	if (iTargetType)
	{
		//	Target shoundn't be a corpse ?
		int iAliveFlag = 0;
		if (iTargetType == 1)
			iAliveFlag = 1;
		else if (iTargetType == 2)
			iAliveFlag = 2;

		CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(idCastTarget, iAliveFlag);
		if (!pObject)
			return false;
	}

	if (!IsMeleeing() && !IsSpellingMagic() &&
		(!iTargetType || idCastTarget == m_PlayerInfo.cid))
	{
		//	Cast this skill need't checking cast distance
		if (!pSkill->ReadyToCast())
			return false;

		//	Prepare to cast skill, if skill isn't INSTANT and FLASHMOVE, 
		//	we must stop moving and stand
		if (!pSkill->IsInstant() && pSkill->GetType() != CECSkill::TYPE_FLASHMOVE)
		{
			if (!NaturallyStopMoving())
				return false;	//	Couldn't stop naturally, so cancel casting skill
		}
		else if (pSkill->GetType() == CECSkill::TYPE_FLASHMOVE)
		{
			if (!CanDo(CANDO_FLASHMOVE))
				return false;
		}

		m_pPrepSkill = pSkill;
		CastSkill(m_PlayerInfo.cid, bForceAttack);
	}
	else if (IsSpellingMagic() && m_pCurSkill == pSkill)
	{
		//	If we are casting the same skill and it's in cooling time
		return false;
	}
	else	//	Have to trace selected object before cast skill
	{
		if(!pSkill->ReadyToCast())
			return false;

		if (CECCastSkillWhenMove::Instance().IsSkillSupported(pSkill->GetSkillID(), this) &&
			m_pWorkMan->IsMovingToPosition() &&
			m_pWorkMan->CanCastSkillImmediately(pSkill->GetSkillID())){
			m_pPrepSkill = pSkill;
			return CastSkill(idCastTarget, bForceAttack);
		}else{			
			bool bTraceOK = false;
			bool bUseAutoPF = false;
			CECPlayerWrapper* pWrapper = CECAutoPolicy::GetInstance().GetPlayerWrapper();
			if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() && pWrapper->GetAttackError() >= 2 )
				bUseAutoPF = true;

			if (!idCastTarget){
				idCastTarget = GetCharacterID();	//	避免瞬移等技能时 idCastTarget 为0导致 CECWorkTrace::CreateTraceTarget 返回空
			}
			if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_TRACEOBJECT))
			{
				CECHPWorkTrace* pWorkTrace = dynamic_cast<CECHPWorkTrace*>(pWork);
				if (pWorkTrace->GetTraceReason() == CECHPWorkTrace::TRACE_SPELL &&
					pWorkTrace->GetTarget() == idCastTarget &&
					pWorkTrace->GetPrepSkill() == pSkill)
					return false;	//	We are just doing the same thing
				
				pWorkTrace->SetTraceTarget(pWorkTrace->CreatTraceTarget(idCastTarget,CECHPWorkTrace::TRACE_SPELL, bForceAttack), bUseAutoPF);
				pWorkTrace->SetPrepSkill(pSkill);
				bTraceOK = true;
			}
			else if (m_pWorkMan->CanStartWork(CECHPWork::WORK_TRACEOBJECT))
			{
				CECHPWorkTrace* pWork = (CECHPWorkTrace*)m_pWorkMan->CreateWork(CECHPWork::WORK_TRACEOBJECT);
				pWork->SetTraceTarget(pWork->CreatTraceTarget(idCastTarget, CECHPWorkTrace::TRACE_SPELL, bForceAttack), bUseAutoPF);
				pWork->SetPrepSkill(pSkill);
				m_pWorkMan->StartWork_p1(pWork);
				bTraceOK = true;
			}
			
			if( !bTraceOK ) return false;
		}
	}

	return true;
}

//	Return to a target town through skill
bool CECHostPlayer::ReturnToTargetTown(int idTarget, bool bCombo/* false */)
{
	if (!CanDo(CANDO_SPELLMAGIC))
		return false;

	int idSkill = ID_RETURNTOWN_SKILL;
	CECSkill* pSkill = GetPositiveSkillByID(idSkill);
	if (!pSkill) pSkill = GetEquipSkillByID(idSkill);
	if (!pSkill)
		return false;

	if( !bCombo )
		ClearComboSkill();

	int iCon = CheckSkillCastCondition(pSkill);
	if (iCon)
	{
		ProcessSkillCondition(iCon);
		return false;
	}

	//	If this skill is in cooling time or we are casting other skill, return 
	if (!pSkill->ReadyToCast() ||
		!m_pWorkMan->CanCastSkillImmediately(pSkill->GetSkillID()))		//	若当前 WORK 级别比 CECHPWorkSpell 高（如 CECHPWorkFly）, 则不应允许执行，
																		//	否则，当收到 OBJECT_CAST_SKILL 协议时, CECHPWorkSpell 无法立即执行，
																		//	将导致 CECHostPlayer::IsSpellingMagic() 返回 false，从而导致客户端发送 c2s_CmdCancelAction，
																		//	最终轮到 CECHPWorkSpell 执行时，无法正常操作
																		//	重现方法：执行起飞命令后、马上再按回城
		return false;

	m_pPrepSkill = pSkill;
	BYTE byPVPMask = glb_BuildPVPMask(false);
	g_pGame->GetGameSession()->c2s_CmdCastSkill(idSkill, byPVPMask, 1, &idTarget);

	return true;
}

//	Summon a player through skill (id == 0 means check the skill condition)
bool CECHostPlayer::SummonPlayer(int idTarget, bool bCombo/* false */)
{
	if (!CanDo(CANDO_SPELLMAGIC))
		return false;

	// can be only used in major map
	if (g_pGame->GetGameRun()->GetWorld()->GetInstanceID() != 1)
	{
		if(idTarget > 0) g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CANNOT_USE_IN_CURR_MAP);
		return false;
	}

	int idSkill = ID_SUMMONPLAYER_SKILL;
	CECSkill* pSkill = GetPositiveSkillByID(idSkill);
	if (!pSkill) pSkill = GetEquipSkillByID(idSkill);
	if (!pSkill)
		return false;

	if(idTarget > 0 && !bCombo)
	{
		ClearComboSkill();
	}

	int iCon = CheckSkillCastCondition(pSkill);
	if (iCon)
	{
		// do not print message when checking summon condition
		if(idTarget > 0) ProcessSkillCondition(iCon);
		return false;
	}

	//	If this skill is in cooling time or we are casting other skill, return 
	if (!pSkill->ReadyToCast() ||
		!m_pWorkMan->CanCastSkillImmediately(pSkill->GetSkillID()))	//	参照 CECHostPlayer::ReturnToTargetTown
		return false;

	if(idTarget > 0)
	{
		m_pPrepSkill = pSkill;
		BYTE byPVPMask = glb_BuildPVPMask(false);
		g_pGame->GetGameSession()->c2s_CmdCastSkill(idSkill, byPVPMask, 1, &idTarget);
	}

	return true;
}

//  Add to goblin skill list
bool CECHostPlayer::AddGoblinSkill(CECSkill* pSkill)
{
	if(!pSkill)
		return false;

	m_aGoblinSkills.Add(pSkill);
	return true;
}
//	Stop host moving naturally, return true host stopped, otherwise return false
bool CECHostPlayer::NaturallyStopMoving()
{
//	if (!m_MoveCtrl.IsStop())
	if (!IsPlayerMoving())
		return true;	//	Host has been stopped

	if (m_iMoveMode == MOVE_FREEFALL || InSlidingState() || IsJumping())
		return false;	//	Host couldn't stop naturally

	if (!m_pWorkMan->IsStanding()){
		m_pWorkMan->FinishAllWork(true);
	}

	m_MoveCtrl.SendStopMoveCmd();

	return true;
}

void CECHostPlayer::TickDiscardedFrame(DWORD dwDeltaTime)
{
	UpdateTimers(dwDeltaTime);
}

//	Tick routine
bool CECHostPlayer::Tick(DWORD dwDeltaTime)
{
	if (!IsSkeletonReady())
		return true;

	CECPlayer::Tick(dwDeltaTime);

	int iRealTime = g_pGame->GetRealTickTime();

	//	Estimate cursor
	EstimateCursor();

	m_MoveCtrl.Tick(dwDeltaTime);

	//	Update selected target
	UpdateSelectedTarget();

	//	Estimatie moving environment
	//EstimateMoveEnv(GetPos() + g_vAxisY * 0.2f);
	//@note :  By Kuiwu[12/10/2005]
	EstimateMoveEnv(GetPos() );

	//	Work tick routine
	if (!IsChangingFace()){
		m_pWorkMan->Tick(dwDeltaTime);
	}

//	m_MoveCtrl.Tick(dwDeltaTime);

	//	Update task
	if (m_pTaskInterface && m_TaskCounter.IncCounter(dwDeltaTime))
	{
		m_TaskCounter.Reset();
		OnTaskCheckStatus(m_pTaskInterface);
		m_pTaskInterface->UpdateEmotionDlg();
	}	

	//	Update timers
	UpdateTimers(dwDeltaTime);

	//	Adjust camera's position
	if (!IsChangingFace())
		UpdateFollowCamera(m_iMoveMode != MOVE_STAND ? true : false, dwDeltaTime);

	if (m_iBoothState != 2)
	{
		if (m_AttachMode != enumAttachNone && m_bHangerOn)
		{
			CECPlayer* pBuddy = m_pPlayerMan->GetPlayer(m_iBuddyId);
			if( (!pBuddy || !pBuddy->GetPlayerModel()) && m_pPlayerModel )
			{
				m_pPlayerModel->Tick(dwDeltaTime);
			}
		}
		else if (m_pPetModel)
			m_pPetModel->Tick(dwDeltaTime);
		else if (m_pPlayerModel)
		{
			m_pPlayerModel->Tick(dwDeltaTime);
			if(IsShapeModelChanged() && GetMajorModel())
			{
				GetMajorModel()->Tick(dwDeltaTime);
			}
		}

		//	Update move sound
		UpdateMoveSound();
	}

	if (m_pFaceModel)
		m_pFaceModel->Tick(dwDeltaTime);

	//	Update GFXs
	UpdateGFXs(dwDeltaTime);

	//	Update team leader position
	UpdateTeamMemberPos((DWORD)iRealTime);

	m_TrickCnt.IncCounter(dwDeltaTime);
	if (m_pvp.dwCoolTime > (DWORD)iRealTime)
		m_pvp.dwCoolTime -= (DWORD)iRealTime;
	else
		m_pvp.dwCoolTime = 0;

	if( m_nTimeToExitInstance > iRealTime )
		m_nTimeToExitInstance -= iRealTime;
	else
		m_nTimeToExitInstance = 0;

	m_dwMoveRelDir	= 0;
	m_fVertSpeed	= 0.0f;
	
	m_DiplomacyReminder.Tick(dwDeltaTime);

	// 自动组队
	m_pAutoTeam->Tick(dwDeltaTime);

	CEComputerAid::Instance().Tick();
	CECShoppingManager::Instance().Tick();
	CECShoppingItemsMover::Instance().Tick();

	if(m_pNavigatePlayer)		
		m_pNavigatePlayer->Tick(dwDeltaTime);

	CECInstanceReenter::Instance().Tick();
	CECPlayerLevelRankRealmChangeCheck::Instance().Tick();
	CECHostFashionEquipFromStorageSystem::Instance().Tick();

	return true;
}

//	Render routine
bool CECHostPlayer::Render(CECViewport* pViewport, int iRenderFlag/* 0 */)
{
	if(IsInForceNavigateState())	{
		// 强制移动时，只绘制clone player，不绘制host player
		m_pNavigatePlayer->Render(pViewport,iRenderFlag);		
		return true;
	}

	m_PateContent.iVisible = 0;

	if (!IsAllResReady())
		return true;

	if( !UpdateCurSkins() )
		return true;

	if( !IsClothesOn() )
		return true;


	A3DTerrainWater * pTerrainWater = g_pGame->GetGameRun()->GetWorld()->GetTerrainWater();
	if( iRenderFlag == RD_REFLECT && pTerrainWater->IsUnderWater(GetPlayerAABB().Maxs) )
		return true;
	else if( iRenderFlag == RD_REFRACT && !pTerrainWater->IsUnderWater(GetPlayerAABB().Mins) )
		return true;

	bool bSkipPlayerRender = false;
	CECPlayer* pBuddy = m_pPlayerMan->GetPlayer(m_iBuddyId);
	if (m_AttachMode != enumAttachNone && m_bHangerOn && pBuddy && pBuddy->GetPlayerModel())
	{
	}
	else
	{
		CECModel* pModel = m_pPetModel ? m_pPetModel : m_pPlayerModel;
		
		// skip rendering when model is not ready
		if( pModel == m_pPlayerModel && !IsShapeModelReady() )
		{
			pModel = NULL;
			bSkipPlayerRender = true;
		}

		if (pModel)
		{
			pModel->Render(pViewport->GetA3DViewport());

			if (m_bCastShadow && g_pGame->GetShadowRender())
			{
				CECPlayer * pPlayer = this;

				SHADOW_RECEIVER receiver = SHADOW_RECEIVER_TERRAIN;
				if( iRenderFlag == RD_NORMAL )
					receiver = (SHADOW_RECEIVER) (receiver | SHADOW_RECEIVER_LITMODEL);

				A3DAABB shadowAABB = GetShadowAABB();
				g_pGame->GetShadowRender()->AddShadower(shadowAABB.Center, shadowAABB, receiver, PlayerRenderForShadow, pPlayer);
			}
		}
	}

	if (m_pFaceModel &&
		!IsShapeModelChanged() && !bSkipPlayerRender &&
		GetMajorModel() && !GetMajorModel()->IsSkinShown(SKIN_HEAD_INDEX))
	{
		m_pFaceModel->Render(pViewport, false, false);
	}

	if (iRenderFlag == RD_NORMAL)
	{
		CECConfigs* pConfigs = g_pGame->GetConfigs();

		if (m_bRenderBar)
			RenderBars(pViewport);

		if (m_bRenderName)
		{
			DWORD dwFlags = pConfigs->GetPlayerTextFlags();
			if (pConfigs->GetVideoSettings().bPlayerSelfName)
				dwFlags |= RNF_NAME;
			else
				dwFlags &= ~RNF_NAME;

			RenderName(pViewport, dwFlags);
		}

		//	Print host's precise position
		if (g_pGame->GetConfigs()->GetShowPosFlag())
		{
			ACHAR szMsg[100];
			A3DVECTOR3 vPos = GetPos();
			a_sprintf(szMsg, _AL("%.3f, %.3f, %.3f"), vPos.x, vPos.y, vPos.z);
			int y = pViewport->GetA3DViewport()->GetParam()->Height - 20;
			vPos += g_vAxisY * (m_aabb.Extents.y * 3.0f);
			g_pGame->GetA3DEngine()->GetSystemFont()->TextOut3D(szMsg, pViewport->GetA3DViewport(), vPos, 0, 0xffffffff);
		}

		//	Print distance between camera to position under cursor
		if (g_pGame->GetConfigs()->GetTestDistFlag())
		{
			int x, y;
			g_pGame->GetGameRun()->GetInputCtrl()->GetMousePos(&x, &y);
			ACHAR szMsg[100];
			if (l_fTestDist >= 0.0f)
				a_sprintf(szMsg, _AL("Dist: %f"), l_fTestDist);
			else
				a_sprintf(szMsg, _AL("Dist: too far"));

			g_pGame->GetA3DEngine()->GetSystemFont()->TextOut(x-40, y-20, szMsg, 0xffffffff);
		}
	}
	
	RenderGoblinOrSprite(pViewport);	
	RenderPetCureGFX();
	RenderMultiObjectGFX();
	RenderMonsterSpiritGfx();

	return true;
}

//	Render when player is opening booth
bool CECHostPlayer::RenderForBooth(CECViewport* pViewport, int iRenderFlag/* 0 */)
{
	m_PateContent.iVisible = 0;

	if (iRenderFlag == RD_REFLECT || iRenderFlag == RD_REFRACT)
		return true;

	CECModel* pBoothModel = GetBoothModel();
	if (pBoothModel)
	{
		//	Set light for model
		A3DSkinModel *pSkinModel = pBoothModel->GetA3DSkinModel();
		if (pSkinModel)
		{
			A3DSkinModel::LIGHTINFO LightInfo;
			memset(&LightInfo, 0, sizeof (LightInfo));
			const A3DLIGHTPARAM& lp = g_pGame->GetDirLight()->GetLightparam();
			
			LightInfo.colAmbient	= g_pGame->GetA3DDevice()->GetAmbientValue();
			LightInfo.vLightDir		= lp.Direction;
			LightInfo.colDirDiff	= lp.Diffuse;
			LightInfo.colDirSpec	= lp.Specular;
			LightInfo.bPtLight		= false;
			
			pSkinModel->SetLightInfo(LightInfo);
		}
		if (pBoothModel->GetComActCount()>0)
		{
			const char *szAct = pBoothModel->GetComActByIndex(0)->GetName();
			pBoothModel->PlayActionByName(szAct, 1.0f, false);
		}

		pBoothModel->SetPos(GetPos());
		pBoothModel->SetDirAndUp(GetDir(), GetUp());
		pBoothModel->Tick(g_pGame->GetTickTime());
		pBoothModel->Render(pViewport->GetA3DViewport(), true);
	}

	CECConfigs* pConfigs = g_pGame->GetConfigs();

	if (m_bRenderBar)
		RenderBars(pViewport);

	if (m_bRenderName)
	{
		DWORD dwFlags = pConfigs->GetPlayerTextFlags();
		if (pConfigs->GetVideoSettings().bPlayerSelfName)
			dwFlags |= RNF_NAME;
		else
			dwFlags &= ~RNF_NAME;

		RenderName(pViewport, dwFlags);
	}

	//	Print host's precise position
	if (g_pGame->GetConfigs()->GetShowPosFlag())
	{
		ACHAR szMsg[100];
		A3DVECTOR3 vPos = GetPos();
		a_sprintf(szMsg, _AL("%.3f, %.3f, %.3f"), vPos.x, vPos.y, vPos.z);
		int y = pViewport->GetA3DViewport()->GetParam()->Height - 20;
		vPos += g_vAxisY * (m_aabb.Extents.y * 3.0f);
		g_pGame->GetA3DEngine()->GetSystemFont()->TextOut3D(szMsg, pViewport->GetA3DViewport(), vPos, 0, 0xffffffff);
	}

	return true;
}

bool CECHostPlayer::RenderForUI(CECViewport * pViewport)
{
	float	f;
	int		n;
	int		t;
	int		l;

	int x = pViewport->GetA3DViewport()->GetParam()->X + (pViewport->GetA3DViewport()->GetParam()->Width >> 1);
	int y = pViewport->GetA3DViewport()->GetParam()->Y + (pViewport->GetA3DViewport()->GetParam()->Height >> 1);
	
	float sf = pViewport->GetA3DViewport()->GetParam()->Width * 1.0f / 800.0f;

	//	Render for dueling
	if (m_pvp.iDuelState != DUEL_ST_NONE)
	{
		switch (m_pvp.iDuelState)
		{
		case DUEL_ST_PREPARE:
			l = m_pvp.iDuelTimeCnt;
			a_Clamp(l, 0, 2999);
			n = l / 1000;
			t = l - n * 1000;
			if( t > 800 )
				f = 1.0f + (t - 800) / 200.0f;
			else
				f = 1.0f;
			if( m_pDuelCountDown[n] )
			{
				m_pDuelCountDown[n]->SetPosition(x, y);
				m_pDuelCountDown[n]->SetScaleX(f * sf);
				m_pDuelCountDown[n]->SetScaleY(f * sf);
				m_pDuelCountDown[n]->DrawToBack();
			}
			break;

		case DUEL_ST_INDUEL:
			if( m_pvp.iDuelTimeCnt < 1000 )
			{
				t = m_pvp.iDuelTimeCnt;
				if( t < 200 )
					f = 1.0f + (200 - t) / 200.0f;
				else
					f = 1.0f;
				if( m_pDuelStates[0] )
				{
					m_pDuelStates[0]->SetPosition(x, y);
					m_pDuelStates[0]->SetScaleX(f * sf);
					m_pDuelStates[0]->SetScaleY(f * sf);
					m_pDuelStates[0]->DrawToBack();
				}
			}
			break;

		case DUEL_ST_STOPPING:
			switch(m_pvp.iDuelRlt)
			{
			case 0:
				break;
			case 1:
				if( m_pDuelStates[1] )
				{
					m_pDuelStates[1]->SetPosition(x, y);
					m_pDuelStates[1]->SetScaleX(sf);
					m_pDuelStates[1]->SetScaleY(sf);
					m_pDuelStates[1]->DrawToBack();
				}
				break;
			case 2:
				if( m_pDuelStates[2] )
				{
					m_pDuelStates[2]->SetPosition(x, y);
					m_pDuelStates[2]->SetScaleX(sf);
					m_pDuelStates[2]->SetScaleY(sf);
					m_pDuelStates[2]->DrawToBack();
				}
				break;
			case 3:
				break;
			}
			break;
		}
	}

	//	Render for battle
	if (IsInBattle() && !IsInFortress() && m_BattleInfo.iResult && m_BattleInfo.iResultCnt)
	{
		int iImage;
		if ((m_BattleInfo.iResult == 1 && m_iBattleCamp == GP_BATTLE_CAMP_INVADER) ||
			(m_BattleInfo.iResult != 1 && m_iBattleCamp == GP_BATTLE_CAMP_DEFENDER))
			iImage = 1;		//	win
		else
			iImage = 2;		//	lose

		m_pDuelStates[iImage]->SetPosition(x, y);
		m_pDuelStates[iImage]->SetScaleX(sf);
		m_pDuelStates[iImage]->SetScaleY(sf);
		m_pDuelStates[iImage]->DrawToBack();
	}

	return true;
}


//	Camera pitch
void CECHostPlayer::CameraPitch(float fDelta)
{
	float fPitch = m_fCameraPitch + fDelta;
	a_Clamp(fPitch, -89.0f, 89.0f);
	
	if (fPitch != m_fCameraPitch)
	{
		m_CameraCoord.RotateX(DEG2RAD(fPitch - m_fCameraPitch));
		m_fCameraPitch = fPitch;
	}
}

//	Camera yaw
void CECHostPlayer::CameraYaw(float fDelta)
{
	float fYaw = m_fCameraYaw + fDelta;

	if (fYaw != m_fCameraYaw)
	{
		A3DVECTOR3 vPos = m_CameraCoord.GetPos();
		m_CameraCoord.SetPos(g_vOrigin);
		m_CameraCoord.RotateAxis(g_vAxisY, DEG2RAD(fYaw - m_fCameraYaw), false);
		m_CameraCoord.SetPos(vPos);

		m_fCameraYaw = fYaw;
	}
}

extern A3DVECTOR3 _cam_shake_offset;
extern bool _shaking_cam;

class CHostCameraUpdater
{
public:
	CHostCameraUpdater(CECHostPlayer* host,CECHostNavigatePlayer* pClone):m_pHost(host),m_pHostClone(pClone),m_bCloneOK(false){}

	void Update(){
		if (m_pHost->IsInForceNavigateState()){
				m_bCloneOK = true;
		}
	}
	A3DMATRIX4 GetMartrix(){
		return m_bCloneOK ? m_pHostClone->GetAbsoluteTM(): m_pHost->GetAbsoluteTM();
	}
	A3DAABB GetAABB(){
		return m_bCloneOK ? m_pHostClone->GetPlayerAABB() : m_pHost->GetPlayerAABB();
	}
	A3DVECTOR3 GetDir(){
		return m_bCloneOK ? m_pHostClone->GetDir() : m_pHost->GetDir();
	}
	A3DVECTOR3 GetUp(){
		return m_bCloneOK ? m_pHostClone->GetUp() : m_pHost->GetUp();
	}
	A3DVECTOR3 GetPos(){
		return m_bCloneOK ? m_pHostClone->GetPos() : m_pHost->GetPos();
	}
protected:
	CECHostPlayer* m_pHost;
	CECHostNavigatePlayer* m_pHostClone;
	bool m_bCloneOK;
};
//	Update camera in following mode
void CECHostPlayer::UpdateFollowCamera(bool bRunning, DWORD dwTime)
{
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();

	CECCamera::TICK_DATA InData;
	CECCamera::ADJUST_DATA OutData;

	CHostCameraUpdater cameraUpdate(this,m_pNavigatePlayer);

	cameraUpdate.Update();

	A3DAABB aabb = cameraUpdate.GetAABB();
	A3DMATRIX4 targetMatrix = cameraUpdate.GetMartrix();

	// always look at the mass center
	A3DVECTOR3 vLookAt = A3DVECTOR3(0.0f, aabb.Extents.y * 1.2f, 0.0f) * targetMatrix;
	//A3DVECTOR3 vLookAt = GetPos() + A3DVECTOR3(0.0f, m_aabb.Extents.y * 1.2f, 0.0f);

	InData.bRunning		= bRunning;
	InData.bTuning		= m_iTurnCammera ? true : false;
	InData.vecDirChar	= cameraUpdate.GetDir();
	InData.vecPosChar	= vLookAt;
	InData.vecUpChar	= cameraUpdate.GetUp();
	InData.dwTime		= dwTime;

	InData.bWater		= false;

	A3DVECTOR3 vPos = cameraUpdate.GetPos();
	A3DTerrainWater * pTerrainWater = pWorld->GetTerrainWater();
	if( pTerrainWater )
	{
		if( vPos.y < pTerrainWater->GetWaterHeight(vPos) - 2.5f )
			InData.bWater = true;
	}

	m_CameraCtrl.Tick(&InData, &OutData);

	if (!m_CameraCtrl.SceneryMode_Get())
	{
		if (_shaking_cam) OutData.vecPosCam += _cam_shake_offset;
		A3DVECTOR3 vDelta = OutData.vecPosCam - vLookAt;
		A3DVECTOR3 vDir = vDelta;
		float fDist = vDir.Normalize();

		float fFactor = 1.0f - (fDist - CECCAMERA_MIN_DISTANCE) / (CECCAMERA_MAX_DISTANCE - CECCAMERA_MIN_DISTANCE);
		vLookAt.y += m_aabb.Extents.y * 0.3f * fFactor;

		//	Do camera collision from player's head
		CECCDS* pCDS = pWorld->GetCDS();

		RAYTRACERT TraceRt;

	//	if (pCDS->RayTrace(vLookAt, vDelta, 1.0f, &TraceRt, TRACEOBJ_CAMERA, 0))
		if (RayTrace(vLookAt, vDelta, TraceRt.vHitPos, TraceRt.vNormal, TraceRt.fFraction))
		{
			float fOffset = fDist * TraceRt.fFraction;
			OutData.vecPosCam = vLookAt + vDir * fOffset;
			OutData.vecPosCam += TraceRt.vNormal * .2f;
		}
		else
		{
			OutData.vecPosCam = vLookAt + vDir * fDist;
			OutData.vecPosCam.y += .2f;
		}
	}

	//	Ensure camera isn't under groud
	A3DVECTOR3 vTerrNorm;
	float fHei = pWorld->GetTerrain()->GetPosHeight(OutData.vecPosCam, &vTerrNorm);

	if (OutData.vecPosCam.y < fHei + 0.2f)
	{
		OutData.vecPosCam.y = fHei;
		OutData.vecPosCam += vTerrNorm * .2f;
	}

	m_CameraCoord.SetPos(OutData.vecPosCam);
	m_CameraCoord.SetDirAndUp(OutData.vecDirCam, OutData.vecUpCam);

	//	Adjust host's transparency
	AdjustTransparency(a3d_Magnitude(OutData.vecPosCam - vLookAt), OutData.vecDirCam, dwTime);

	if (m_iTurnCammera & TURN_LBUTTON)
	{
		A3DVECTOR3 vDir = OutData.vecDirCam;
		vDir.y = 0.0f;
		vDir.Normalize();
		if (!vDir.IsZero())
			ChangeModelMoveDirAndUp(vDir, g_vAxisY);
	}
}

//	Adjust host's transparency
void CECHostPlayer::AdjustTransparency(float fDistToCam, const A3DVECTOR3& vDir, DWORD dwTime)
{
	float fDstTrans = -1.f;

	if (fDistToCam < 0.9f || vDir.y > 0.8f)
	{
		//	Increase transparency
		fDstTrans = 1.f;
	}
	else if (fDistToCam < 1.2f || vDir.y > 0.75f)
	{
		//	Keep adjustment of last time
	}
	else
	{
		//	Decrease transparency
		fDstTrans = GetTransparentLimit();
	}

	// refresh the adjusting counter
	StartAdjustTransparency(-1.f, fDstTrans, 500);

	float fTrans = UpdateTransparency(dwTime);
	SetTransparent(fTrans);

	//	Adjust buddy's transparency
	if (GetBuddyState() && m_iBuddyId)
	{
		CECElsePlayer* pBuddy = m_pPlayerMan->GetElsePlayer(m_iBuddyId);
		pBuddy->SetTransparent(fTrans);
	}
}

static const float _yaw_vel = 500.0f;
static const float _yaw_vel_air = 300.0f;

//	Get push direction
bool CECHostPlayer::GetPushDir(A3DVECTOR3& vPushDir, DWORD dwMask, float fDeltaTime)
{
	vPushDir.Set(0.0f, 0.0f, 0.0f);
	A3DVECTOR3 vRight(0);
	bool bMove = false;

	if (!m_dwMoveRelDir)
		return false;

	if (m_dwMoveRelDir & MD_LEFT)
	{
		ECMSG msg;
		msg.dwParam2 = 0;
		if (m_iMoveEnv == MOVEENV_AIR)
		{
			msg.dwParam1 = DWORD(_yaw_vel_air * fDeltaTime);
			if (m_dwMoveRelDir & MD_BACK) msg.dwParam1 = -(int)msg.dwParam1;
			OnMsgHstYaw(msg);
		}
		else if (m_iMoveEnv == MOVEENV_WATER)
		{
			msg.dwParam1 = DWORD(_yaw_vel * fDeltaTime);
			if (m_dwMoveRelDir & MD_BACK) msg.dwParam1 = -(int)msg.dwParam1;
			OnMsgHstYaw(msg);
		}
		else
		{
			vRight = -m_CameraCoord.GetRight();
			vRight.y = 0;
			vRight.Normalize();
			vPushDir = vRight;
			bMove = true;
		}
	}
	else if (m_dwMoveRelDir & MD_RIGHT)
	{
		ECMSG msg;
		msg.dwParam2 = 0;
		if (m_iMoveEnv == MOVEENV_AIR)
		{
			msg.dwParam1 = DWORD((-_yaw_vel_air) * fDeltaTime);
			if (m_dwMoveRelDir & MD_BACK) msg.dwParam1 = -(int)msg.dwParam1;
			OnMsgHstYaw(msg);
		}
		else if (m_iMoveEnv == MOVEENV_WATER)
		{
			msg.dwParam1 = DWORD((-_yaw_vel) * fDeltaTime);
			if (m_dwMoveRelDir & MD_BACK) msg.dwParam1 = -(int)msg.dwParam1;
			OnMsgHstYaw(msg);
		}
		else
		{
			vRight = m_CameraCoord.GetRight();
			vRight.y = 0;
			vRight.Normalize();
			vPushDir = vRight;
			bMove = true;
		}
	}

	if (m_dwMoveRelDir & MD_FORWARD)
	{
		vPushDir = m_CameraCoord.GetDir();
		vPushDir.y = 0;
		vPushDir.Normalize();

		if (bMove)
		{
			vPushDir += vRight;
			vPushDir.Normalize();
		}
	}
	else if (m_dwMoveRelDir & MD_BACK)
	{
		vPushDir = -m_CameraCoord.GetDir();
		vPushDir.y = 0;
		vPushDir.Normalize();

		if (bMove)
		{
			vPushDir += vRight;
			vPushDir.Normalize();
		}
	}
	else if (m_dwMoveRelDir & MD_ABSUP & dwMask)
		vPushDir = g_vAxisY;
	else if (m_dwMoveRelDir & MD_ABSDOWN & dwMask)
		vPushDir = -g_vAxisY;
	else
		return bMove;

/*
	A3DVECTOR3 vDir = m_CameraCoord.GetDir();
	A3DVECTOR3 vRight = m_CameraCoord.GetRight();

	if (m_dwMoveRelDir & MD_FORWARD)
	{
		if (m_dwMoveRelDir & MD_LEFT)
			vPushDir = a3d_Normalize(vDir-vRight);
		else if (m_dwMoveRelDir & MD_RIGHT)
			vPushDir = a3d_Normalize(vDir+vRight);
		else
			vPushDir = vDir;
	}
	else if (m_dwMoveRelDir & MD_BACK)
	{
		if (m_dwMoveRelDir & MD_LEFT)
			vPushDir = a3d_Normalize(-vDir-vRight);
		else if (m_dwMoveRelDir & MD_RIGHT)
			vPushDir = a3d_Normalize(-vDir+vRight);
		else
			vPushDir = -vDir;
	}
	else if (m_dwMoveRelDir & MD_LEFT)
		vPushDir = -vRight;
	else if (m_dwMoveRelDir & MD_RIGHT)
		vPushDir = vRight;
	else
		return false;
*/
	return true;
}

//	Message MSG_HST_LBTNCLICK handler
void CECHostPlayer::OnMsgLBtnClick(const ECMSG& Msg)
{
	// 结束自动策略
	if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
		CECAutoPolicy::GetInstance().StopPolicy();

	CECCDS* pCDS = g_pGame->GetGameRun()->GetWorld()->GetCDS();
	CECInputCtrl* pInputCtrl = g_pGame->GetGameRun()->GetInputCtrl();

	A3DVECTOR3 vDest((float)Msg.dwParam1, (float)Msg.dwParam2, 1.0f);
	A3DViewport* pView = g_pGame->GetViewport()->GetA3DViewport();
	pView->InvTransform(vDest, vDest);

	A3DVECTOR3 vStart = pView->GetCamera()->GetPos();
	A3DVECTOR3 vDelta = vDest - vStart;

	RAYTRACERT TraceRt;
	int iTraceFlag = TRACEOBJ_LBTNCLICK;
	if (pInputCtrl->IsShiftPressed(Msg.dwParam3))
		iTraceFlag |= 0x80000000;
	
	if (pInputCtrl->IsAltPressed(Msg.dwParam3))
		iTraceFlag |= 0x40000000;

	//GetWorld()->GetDecalMan();
	int idTraceTarget = 0, idSelTarget = 0;
	bool bForceAttack = false;
	int iTraceReason = CECHPWorkTrace::TRACE_NONE;
	bool bWikiMonster = false;

	CECNPCMan* pNPCMan = g_pGame->GetGameRun()->GetWorld()->GetNPCMan();
	CECNPC* pNPC = pNPCMan->GetMouseOnPateTextNPC(Msg.dwParam1, Msg.dwParam2);
	if (pNPC){
		idTraceTarget = pNPC->GetNPCID();
		idSelTarget = idTraceTarget;
		iTraceReason = CECHPWorkTrace::TRACE_TALK;	
	}else{	
		CECPlayerMan* pPlayerMan = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
		CECElsePlayer* pBoothBarPlayer = pPlayerMan->GetMouseOnPateTextPlayer(Msg.dwParam1, Msg.dwParam2);
		if (pBoothBarPlayer && m_idCurHover == pBoothBarPlayer->GetCharacterID()){
			idTraceTarget = pBoothBarPlayer->GetCharacterID();
			idSelTarget = idTraceTarget;
			iTraceReason = CECHPWorkTrace::TRACE_TALK;
		}else if (pCDS->RayTrace(vStart, vDelta, 1.0f, &TraceRt, iTraceFlag, (DWORD)this)){		
			
			if (pCDS->m_RayTraceRt.iEntity == ECENT_TERRAIN ||
				pCDS->m_RayTraceRt.iEntity == ECENT_BUILDING ||
				pCDS->m_RayTraceRt.iEntity == ECENT_FOREST ){
				if (m_pWorkMan->IsSitting()){
					g_pGame->GetGameSession()->c2s_CmdStandUp();
					return;
				}
				if (!CanDo(CANDO_MOVETO))
					return;			
				//	Hit terrain
				A3DVECTOR3 vMoveDest = vStart + vDelta * pCDS->m_RayTraceRt.fFraction;
				if (pInputCtrl->IsCtrlPressed(Msg.dwParam3)){
					g_pGame->GetGameSession()->c2s_CmdGoto(vMoveDest.x, vMoveDest.y, vMoveDest.z);
				}else{
					if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_MOVETOPOS)) {
						CECHPWorkMove* pWorkMove = dynamic_cast<CECHPWorkMove*>(pWork);
						pWorkMove->SetDestination(CECHPWorkMove::DEST_2D, vMoveDest);
						pWorkMove->SetUseAutoMoveDialog(false);
						pWorkMove->PlayMoveTargetGFX(TraceRt.vHitPos, TraceRt.vNormal);
					}
					else if (m_pWorkMan->CanStartWork(CECHPWork::WORK_MOVETOPOS)) {
						//	If destination is too near, ignore it.
						A3DVECTOR3 vDist = vMoveDest - GetPos();
						if (vDist.MagnitudeH() > 0.5f)	{
							CECHPWorkMove* pWork = (CECHPWorkMove*)m_pWorkMan->CreateWork(CECHPWork::WORK_MOVETOPOS);
							pWork->SetDestination(CECHPWorkMove::DEST_2D, vMoveDest);
							pWork->PlayMoveTargetGFX(TraceRt.vHitPos, TraceRt.vNormal);
							m_pWorkMan->StartWork_p1(pWork);
						}
					}
				}
			}else{
				//	Hit a object
				int idHitObject = pCDS->m_RayTraceRt.iObjectID;				
				if (pCDS->m_RayTraceRt.iEntity == ECENT_MATTER)	{
					CECMatter* pMatter = g_pGame->GetGameRun()->GetWorld()->GetMatterMan()->GetMatter(idHitObject);
					
					idTraceTarget	= idHitObject;
					iTraceReason	= pMatter->IsMine() ? CECHPWorkTrace::TRACE_GATHER : CECHPWorkTrace::TRACE_PICKUP;
				}else if (pCDS->m_RayTraceRt.iEntity == ECENT_DYN_OBJ){
					return;
				}else {
					// NPC or Player
					if (pCDS->m_RayTraceRt.iEntity == ECENT_NPC){
						CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(idHitObject);
						
						//	Msg.dwParam4 is double click flag
						if (!pNPC->IsDead() && (m_idSelTarget == idHitObject ||
							(Msg.dwParam4 && m_idUCSelTarget == idHitObject)))
							idTraceTarget = idHitObject;
						else
							idSelTarget = idHitObject;
						
						if (idTraceTarget){
							bForceAttack = glb_GetForceAttackFlag(&Msg.dwParam3);
							if (AttackableJudge(idHitObject, bForceAttack) == 1)
								iTraceReason = CECHPWorkTrace::TRACE_ATTACK;
							else if (pNPC->IsServerNPC()){
								if (!IsInBattle() || InSameBattleCamp(pNPC))
									iTraceReason = CECHPWorkTrace::TRACE_TALK;
							}
						}
					}else{
						//	pCDS->m_RayTraceRt.iEntity == ECENT_PLAYER
						CECPlayer* pPlayer = m_pPlayerMan->GetPlayer(idHitObject);
						
						//	1. Msg.dwParam4 is double click flag.
						//	2. Buddy player counld't be traced
						if (!pPlayer->IsDead() && pPlayer->GetCharacterID() != m_iBuddyId &&
							(m_idSelTarget == idHitObject || (Msg.dwParam4 && m_idUCSelTarget == idHitObject)))	{
							idTraceTarget	= idHitObject;
							bForceAttack	= glb_GetForceAttackFlag(&Msg.dwParam3);
							
							if (AttackableJudge(idHitObject, bForceAttack) == 1)
								iTraceReason = CECHPWorkTrace::TRACE_ATTACK;
							else if (pPlayer->GetBoothState())
								iTraceReason = CECHPWorkTrace::TRACE_TALK;
						}else{
							idSelTarget = idHitObject;
						}
					}
					
					// cancel this action if not selectable
					if(!CanSelectTarget(idTraceTarget))	{
						return;
					}
				}
				
				
				CECNPC* pTarget = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(idHitObject);
				if(CDlgAutoHelp::IsAutoHelp() && pTarget && pTarget->IsMonsterNPC())
					bWikiMonster = true;
				
				if (bWikiMonster){
					CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
					CDlgWikiShortcut::PopMonsterWiki(pGameUI,idHitObject);
				}
			}
		}else{
			//	Nothing is clicked
			if (m_pWorkMan->IsSitting()){
				g_pGame->GetGameSession()->c2s_CmdStandUp();
				return;
			}
			
			if (!CanDo(CANDO_MOVETO))
				return;
			
			A3DVECTOR3 vMoveDir = vDelta;
			vMoveDir.y = 0.0f;
			if (vMoveDir.IsZero())
				return;
			
			vMoveDir.Normalize();
			
			//	Move on the clicked direction
			if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_MOVETOPOS)){
				CECHPWorkMove* pWorkMove = dynamic_cast<CECHPWorkMove*>(pWork);
				pWorkMove->SetDestination(CECHPWorkMove::DEST_DIR, vMoveDir);
			}
			else if (m_pWorkMan->CanStartWork(CECHPWork::WORK_MOVETOPOS)){
				CECHPWorkMove* pWork = (CECHPWorkMove*)m_pWorkMan->CreateWork(CECHPWork::WORK_MOVETOPOS);
				pWork->SetDestination(CECHPWorkMove::DEST_DIR, vMoveDir);
				m_pWorkMan->StartWork_p1(pWork);
			}
		}
	}
	
	if (idTraceTarget){
		if (m_pWorkMan->IsSitting()){
			g_pGame->GetGameSession()->c2s_CmdStandUp();
			return;
		}
		//	Trace a object
		if (iTraceReason == CECHPWorkTrace::TRACE_ATTACK){
			if (!CanDo(CANDO_MELEE))
				return;			
			NormalAttackObject(idTraceTarget, bForceAttack);
		}else{
			if (!CanDo(CANDO_MOVETO))
				return;
			
			if (iTraceReason == CECHPWorkTrace::TRACE_PICKUP){
				PickupObject(idTraceTarget, false);
			}else if (iTraceReason == CECHPWorkTrace::TRACE_GATHER){
				PickupObject(idTraceTarget, true);
			}else if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_TRACEOBJECT)){
				CECHPWorkTrace* pWorkTrace = dynamic_cast<CECHPWorkTrace*>(pWork);
				pWorkTrace->SetTraceTarget(pWorkTrace->CreatTraceTarget(idTraceTarget, iTraceReason, bForceAttack));
			}else if (m_pWorkMan->CanStartWork(CECHPWork::WORK_TRACEOBJECT) && !bWikiMonster){
				CECHPWorkTrace* pWork = (CECHPWorkTrace*)m_pWorkMan->CreateWork(CECHPWork::WORK_TRACEOBJECT);
				pWork->SetTraceTarget(pWork->CreatTraceTarget(idTraceTarget, iTraceReason, bForceAttack));
				m_pWorkMan->StartWork_p1(pWork);
			}
		}
	}
	
	//	Tell server we select a target
	if (idSelTarget && m_idSelTarget != idSelTarget){
		m_idUCSelTarget = idSelTarget;
		SelectTarget(m_idUCSelTarget);
	}

}

//	Message MSG_HST_RBTNCLICK handler
void CECHostPlayer::OnMsgRBtnClick(const ECMSG& Msg)
{
	if (!IsAllResReady() || IsAboutToDie() || IsDead() || IsTrading() ||
		IsUsingTrashBox()|| IsTalkingWithNPC() || IsChangingFace())
		return;

	CECCDS* pCDS = g_pGame->GetGameRun()->GetWorld()->GetCDS();

	A3DVECTOR3 vDest((float)Msg.dwParam1, (float)Msg.dwParam2, 1.0f);
	A3DViewport* pView = g_pGame->GetViewport()->GetA3DViewport();
	pView->InvTransform(vDest, vDest);

	A3DVECTOR3 vStart = pView->GetCamera()->GetPos();
	A3DVECTOR3 vDelta = vDest - vStart;

	RAYTRACERT TraceRt;
	int iTraceFlag = TRACEOBJ_RBTNCLICK;
	if (g_pGame->GetGameRun()->GetInputCtrl()->IsShiftPressed(Msg.dwParam3))
		iTraceFlag |= 0x80000000;

	if (g_pGame->GetGameRun()->GetInputCtrl()->IsAltPressed(Msg.dwParam3))
		iTraceFlag |= 0x40000000;

	if (pCDS->RayTrace(vStart, vDelta, 1.0f, &TraceRt, iTraceFlag, (DWORD)this))
	{
		if (pCDS->m_RayTraceRt.iEntity == ECENT_PLAYER)
		{
			m_idClickedMan = pCDS->m_RayTraceRt.iObjectID;
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			pGameUI->PopupPlayerContextMenu();
		}
	}
}

//	Revive
void CECHostPlayer::Revive(const S2C::cmd_player_revive* pCmd)
{
	//	Revive type
	enum
	{
		REVIVE_RETURN = 0,
		REVIVE_HERE_START,
		REVIVE_HERE_END,
	};
	
	m_dwReviveTime = timeGetTime();

	if (pCmd->sReviveType != REVIVE_HERE_END)
	{
		SetUseGroundNormal(false);

		if (!IsDead())
			return;

		m_dwStates &= ~GP_STATE_CORPSE;

		if (!m_pWorkMan->IsDead())
		{
			ASSERT(false);
			return;
		}

		m_bAboutToDie	= false;
		m_fReviveExp	= -1.0f;
		
		//	Close death dialog
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->PopupReviveDialog(false);

		m_pWorkMan->FinishRunningWork(CECHPWork::WORK_DEAD);
		
		if (pCmd->sReviveType == REVIVE_HERE_START)
		{
			CECHPWorkRevive* pWork = (CECHPWorkRevive*)m_pWorkMan->CreateWork(CECHPWork::WORK_REVIVE);
			m_pWorkMan->StartWork_p1(pWork);
		}
	}
	else	//	pCmd->sReviveType == REVIVE_HERE_END
	{
		if (!m_pWorkMan->IsReviving())
		{
			ASSERT(false);
			return;
		}
		m_pWorkMan->FinishRunningWork(CECHPWork::WORK_REVIVE);
	}
}

//	Begin following the selected target
void CECHostPlayer::BeginFollow(int idTarget)
{
	if (!CanDo(CANDO_FOLLOW))
		return;

	if (!ISPLAYERID(idTarget) || idTarget == m_PlayerInfo.cid)
		return;

	if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_FOLLOW))
	{
		dynamic_cast<CECHPWorkFollow*>(pWork)->ChangeTarget(idTarget);
	}
	else if (m_pWorkMan->CanStartWork(CECHPWork::WORK_FOLLOW))
	{
		CECHPWorkFollow* pWork = (CECHPWorkFollow*)m_pWorkMan->CreateWork(CECHPWork::WORK_FOLLOW);
		pWork->ChangeTarget(idTarget);
		m_pWorkMan->StartWork_p1(pWork);
	}
}

//	Level up
void CECHostPlayer::LevelUp()
{
	CECGameSession *pSession = g_pGame->GetGameSession();

	m_BasicProps.iLevel++;
	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_LEVELUP, m_BasicProps.iLevel);

	//	Get all extend properties
	pSession->c2s_CmdGetExtProps();

//	if (m_pLevelUpGFX)
//		m_pLevelUpGFX->Start(true);
	PlayGfx(res_GFXFile(RES_GFX_LEVELUP), NULL, 1.0, PLAYERMODEL_TYPEALL);
	
	//	Popup notify bubble text
	BubbleText(BUBBLE_LEVELUP, 0);

	//	Notify my friends that my level changed
	ACHAR szInfo[40];
	a_sprintf(szInfo, _AL("L%d"), m_BasicProps.iLevel);

	for (int i=0; i < m_pFriendMan->GetGroupNum(); i++)
	{
		CECFriendMan::GROUP* pGroup = m_pFriendMan->GetGroupByIndex(i);
		for (int j=0; j < pGroup->aFriends.GetSize(); j++)
		{
			CECFriendMan::FRIEND* pFriend = pGroup->aFriends[j];
			if (pFriend->IsGameOnline())
			{
				pSession->SendPrivateChatData(pFriend->GetName(),
							szInfo, GNET::CHANNEL_USERINFO, pFriend->id);
			}
		}
	}

	if (GetBasicProps().iLevel==30)
	{
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->AddChatMessage(pGameUI->GetStringFromTable(9638), GP_CHAT_SYSTEM);
	}
	if (GetBasicProps().iLevel>31)
	{
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		((CDlgOnlineAward*)pGameUI->GetDialog("Win_AddExp2"))->RestartWhenLevelup();
	}
}
//	Change invisible
void CECHostPlayer::ChangeInvisible(int iDegree)
{
	if(m_pPlayerModel)
	{
		if(iDegree > 0)
		{
			m_dwStates |= GP_STATE_INVISIBLE;
			// add fix msg such as "enter invisible state", etc.
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ENTER_INVISIBLE_STATE);
		}
		else
		{
			m_dwStates &= ~GP_STATE_INVISIBLE;
			// add fix msg such as "leave invisible state", etc.
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_LEAVE_INVISIBLE_STATE);
		}
	}
}
//	Host touched target ?
bool CECHostPlayer::CanTouchTarget(const A3DAABB& aabbTarget)
{
	if (CLS_AABBAABBOverlap(m_aabb.Center, m_aabbServer.Extents, aabbTarget.Center, 
			aabbTarget.Extents * 1.5f))
		return true;

	return false;
}

//	Can host touch target ?
//	iReason: 0 - no special reason; 1 - melee; 2 - cast magic; 3 - talk;
//	fMaxCut: Maximum cut distance
bool CECHostPlayer::CanTouchTarget(const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos, float fTargetRad, 
								int iReason, float fMaxCut/* 1.0f */)
{
	float fDist = a3d_Magnitude(vTargetPos - vHostPos);

	switch (iReason)
	{
	case 1:		//	melee
	{
		float fRange;
		if (fMaxCut >= 0.0f)
		{
			float fCutDist = m_ExtProps.ak.attack_range * 0.3f;
			if (fCutDist > fMaxCut)
				fCutDist = fMaxCut;

			fRange = m_ExtProps.ak.attack_range - fCutDist;
		}
		else
			fRange = m_ExtProps.ak.attack_range * 0.7f;

		if (fDist - fTargetRad <= fRange)
			return true;

		break;
	}
	case 2:		//	cast magic
	{
		if (m_pPrepSkill)
		{
			float fRange = m_pPrepSkill->GetCastRange(m_ExtProps.ak.attack_range, GetPrayDistancePlus());
			if (fRange > 0.0f)
			{
				if (fDist - fTargetRad <= fRange)
					return true;
			}
			else
				return true;
		}

		break;
	}
	case 3:		//	talk
	{
		if (fDist - fTargetRad <= 5.0f)
			return true;

		break;
	}
	default:	//	no special reason
	{
		if (fDist < (fTargetRad + m_fTouchRad) * 3.0f)
			return true;

		break;
	}
	}
	
	return false;
}

bool CECHostPlayer::CanTouchTarget(const A3DVECTOR3& vTargetPos, float fTargetRad, int iReason, float fMaxCut/* 1.0f */)
{
	return CanTouchTarget(GetPos(), vTargetPos, fTargetRad, iReason, fMaxCut);
}

//	Update selected target
void CECHostPlayer::UpdateSelectedTarget()
{
	if (m_idSelTarget && m_selectTargetUpdateTimer.IsTimeArrived()){
		m_selectTargetUpdateTimer.Reset();
		if (!CanSelectTarget(m_idSelTarget)){
			g_pGame->RuntimeDebugInfo(0xffffff00, _AL("Client Unselect 0x%x(%d)"), m_idSelTarget, m_idSelTarget);
			SelectTarget(0);
		}
	}
}

//	Estimate move environment
void CECHostPlayer::EstimateMoveEnv(const A3DVECTOR3& vPos)
{
	if (IsFlying())
	{
		m_iMoveEnv = MOVEENV_AIR;
		return;
	}

	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
//	m_GndInfo.fGndHei = pWorld->GetTerrainHeight(vPos, &m_GndInfo.vGndNormal);

	//  Test whether last pos is in air
	A3DVECTOR3 vGndPos0, vTestPos0 = m_MoveCtrl.GetLastSevPos() + g_vAxisY * m_aabbServer.Extents.y;
	VertRayTrace(vTestPos0, vGndPos0, m_GndInfo.vGndNormal);
	m_GndInfo.fGndHei = vGndPos0.y;

	A3DVECTOR3 vAABBGnd;
	VertAABBTrace(vTestPos0, m_aabbServer.Extents, vAABBGnd, m_GndInfo.vGndNormal);
	vAABBGnd.y -= m_aabbServer.Extents.y;

	bool bIsInAir = false;
	if(m_MoveCtrl.GetLastSevPos().y - vAABBGnd.y > 0.2f)
		bIsInAir = true;

	A3DVECTOR3 vGndPos, vTestPos = vPos + g_vAxisY * m_aabbServer.Extents.y;
	VertRayTrace(vTestPos, vGndPos, m_GndInfo.vGndNormal);
	m_GndInfo.fGndHei = vGndPos.y;
	m_GndInfo.fWaterHei = pWorld->GetWaterHeight(vTestPos);

	//@note : use the aabb trace. By Kuiwu[12/10/2005]
	VertAABBTrace(vTestPos, m_aabbServer.Extents, vAABBGnd, m_GndInfo.vGndNormal);
	vAABBGnd.y -= m_aabbServer.Extents.y;

	//@note : By Kuiwu[12/10/2005]
	int iNewEnv = MOVEENV_GROUND;
	if (CheckWaterMoveEnv(vPos, m_GndInfo.fWaterHei, vAABBGnd.y))
		iNewEnv = MOVEENV_WATER;

	// 飞天挂问题

	if (iNewEnv == MOVEENV_GROUND && GetPos().y - vAABBGnd.y < 0.2f && bIsInAir && GetPos() != m_MoveCtrl.GetLastSevPos())
	{
		m_MoveCtrl.SendMoveCmd(GetPos(), 2, g_vAxisY, m_CDRInfo.vAbsVelocity, m_iMoveMode, true);
//		BubbleText(BUBBLE_LEVELUP, 0);
	}

	if (iNewEnv == MOVEENV_GROUND)
	{
		m_GndInfo.bOnGround = true;

	//	if (vPos.y > m_GndInfo.fGndHei + 0.2f)
		if (m_CDRInfo.vTPNormal.IsZero())
		{
			if (m_iMoveMode != MOVE_FREEFALL)
				m_iMoveMode = MOVE_FREEFALL;

			m_GndInfo.bOnGround = false;
			if (IsJumping() && m_CDRInfo.vAbsVelocity.y < 0.0f && vPos.y - vAABBGnd.y < 0.6f)
			{
				PlayAction(ACT_JUMP_LAND, false);
			}
		}
		else
		{
			if (IsJumping() && m_CDRInfo.vAbsVelocity.y < 0.0f && vPos.y - vAABBGnd.y < 0.6f)
			{
				PlayAction(ACT_JUMP_LAND, false);
				ResetJump();
			}

		//	if (m_GndInfo.vGndNormal.y < EC_SLOPE_Y)
			if (m_CDRInfo.vTPNormal.y < EC_SLOPE_Y)
			{
				if (!m_MoveCtrl.GetSlideLock())
				{
					if (m_iMoveMode != MOVE_SLIDE)
					{
						m_iOldWalkMode = m_iMoveMode;
						m_iMoveMode = MOVE_SLIDE;
					}
				}
				else
				{
					m_MoveCtrl.SetSlideLock(false);
					m_iMoveMode = MOVE_STAND;
				}
			}
			else
			{
				m_MoveCtrl.SetSlideLock(false);
				if (m_iMoveMode == MOVE_FREEFALL)
				{
					m_iMoveMode = MOVE_STAND;
				}
				else if (m_iMoveMode == MOVE_SLIDE)
					m_iMoveMode = m_iOldWalkMode;
			}
		}
	}
	else if (iNewEnv == MOVEENV_WATER)
	{
		m_CDRInfo.fYVel = 0.0f;

		if (m_iMoveMode == MOVE_SLIDE)
		{
			if (m_pWorkMan->IsMoving())
				m_iMoveMode = MOVE_MOVE;
			else
				m_iMoveMode = MOVE_STAND;
		}
	}

	m_iMoveEnv = iNewEnv;
}

//	Estimate mouse cursor
void CECHostPlayer::EstimateCursor()
{
	CECGameRun* pGameRun = g_pGame->GetGameRun();

	m_idCurHover = 0;
	if (pGameRun->UIHasCursor() || IsChangingFace())
		return;

	CECCDS* pCDS = pGameRun->GetWorld()->GetCDS();
	CECInputCtrl* pInputCtrl = pGameRun->GetInputCtrl();

	int x, y, iCursor=RES_CUR_NORMAL;

	if (CDlgAutoHelp::IsAutoHelp())
	{
		iCursor = RES_CUR_HAND;
	}

	pInputCtrl->GetMousePos(&x, &y);
	A3DVECTOR3 vDest((float)x, (float)y, 1.0f);
	A3DViewport* pView = g_pGame->GetViewport()->GetA3DViewport();
	pView->InvTransform(vDest, vDest);

	A3DVECTOR3 vStart = pView->GetCamera()->GetPos();
	A3DVECTOR3 vDelta = vDest - vStart;

	RAYTRACERT TraceRt;
	int iTraceFlag = TRACEOBJ_LBTNCLICK;
	if (pInputCtrl->KeyIsBeingPressed(VK_SHIFT))
		iTraceFlag |= 0x80000000;

	if (pInputCtrl->KeyIsBeingPressed(VK_MENU))
		iTraceFlag |= 0x40000000;
	
	
	
	CECPlayerMan* pPlayerMan = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
	CECElsePlayer* pPatePlayer = pPlayerMan->GetMouseOnPateTextPlayer(x,y);
	if (pPatePlayer && pPatePlayer->GetBoothState() == 2){
		iCursor = RES_CUR_HAND;
		m_idCurHover = pPatePlayer->GetCharacterID();
	}

	if (pCDS->RayTrace(vStart, vDelta, 1.0f, &TraceRt, iTraceFlag, (DWORD)this))
	{
		int idHitObject(0);
		A3DVECTOR3 hitDis = TraceRt.vHitPos - vStart;
		if(pPatePlayer){
			A3DVECTOR3 playerPateDis = pPatePlayer->GetPos() - vStart;
			if (hitDis.Magnitude() < playerPateDis.Magnitude()){
				idHitObject = pCDS->m_RayTraceRt.iObjectID;
			}
		}else{
			idHitObject = pCDS->m_RayTraceRt.iObjectID;
		}
		if (idHitObject){
			bool bForceAttack = glb_GetForceAttackFlag(NULL);
			
			if (pCDS->m_RayTraceRt.iEntity == ECENT_MATTER)
			{
				CECMatter* pMatter = pGameRun->GetWorld()->GetMatterMan()->GetMatter(idHitObject);
				if (pMatter)
				{
					if (!pMatter->IsMine())
						iCursor = RES_CUR_PICKUP;
					else if (CanGatherMatter(pMatter))
						iCursor = pMatter->IsMonsterSpiritMine() ? RES_CUR_SWALLOW : RES_CUR_DIG;
				}
				
				if (iCursor != RES_CUR_NORMAL)
					m_idCurHover = idHitObject;
			}
			else if (pCDS->m_RayTraceRt.iEntity == ECENT_NPC)
			{
				CECNPC* pNPC = pGameRun->GetWorld()->GetNPCMan()->GetNPC(idHitObject);
				if (pNPC)
				{
					m_idCurHover = idHitObject;
					if (m_idSelTarget == idHitObject &&	AttackableJudge(idHitObject, bForceAttack) == 1)
						iCursor = RES_CUR_ATTACK;
					else if (pNPC->IsServerNPC())
					{														
						if (!IsInBattle() || InSameBattleCamp(pNPC))
							iCursor = RES_CUR_TALK;
					}
				}
			}
			else if (pCDS->m_RayTraceRt.iEntity == ECENT_PLAYER)
			{
				CECElsePlayer* pTracePlayer = pGameRun->GetWorld()->GetPlayerMan()->GetElsePlayer(idHitObject);
				m_idCurHover = idHitObject;				
				if (m_idSelTarget == idHitObject && AttackableJudge(idHitObject, bForceAttack) == 1)
					iCursor = RES_CUR_ATTACK;
			}
			
			l_fTestDist = vDelta.Magnitude() * TraceRt.fFraction;
		}
	}else{
		l_fTestDist = -1.0f;
	}

	CECNPCMan* pNPCMan = g_pGame->GetGameRun()->GetWorld()->GetNPCMan();
	CECNPC* pHoverNPC = pNPCMan->GetMouseOnPateTextNPC(x,y);
	if (pHoverNPC){
		iCursor = RES_CUR_TALK;
		m_idCurHover = pHoverNPC->GetNPCID();
	}
	
	g_pGame->ChangeCursor(iCursor);
}

//	Update time counters
void CECHostPlayer::UpdateTimers(DWORD dwDeltaTime)
{
	//	Get real time tick of this frame
	int iRealTime = g_pGame->GetRealTickTime();

	//	Update flysword time
	if (IsFlying() && GetRushFlyFlag())
	{
		CECIvtrItem* pItem = m_pEquipPack->GetItem(EQUIPIVTR_FLYSWORD);
		ASSERT(pItem);

		if (pItem->GetClassID() == CECIvtrItem::ICID_FLYSWORD)
		{
			CECIvtrFlySword* pFlySword = (CECIvtrFlySword*)pItem;
			pFlySword->TimePass(iRealTime);
		}
	}
	
	int i;

	//	Update skills
	for (i=0; i < m_aPtSkills.GetSize(); i++)
		m_aPtSkills[i]->Tick();

	for (i=0; i < m_aPsSkills.GetSize(); i++)
		m_aPsSkills[i]->Tick();
	
	for (i=0; i < m_aGoblinSkills.GetSize(); i++)
		m_aGoblinSkills[i]->Tick();

	for (i=0; i < m_aEquipSkills.GetSize(); i++)
		m_aEquipSkills[i]->Tick();

	if(m_pTargetItemSkill)
		m_pTargetItemSkill->Tick();

	CECComboSkillState::Instance().Tick();

	//	Update cool times
	for (i=0; i < GP_CT_MAX; i++)
		m_aCoolTimes[i].Update(iRealTime);

	for (SkillCoolTime::iterator it = m_skillCoolTime.begin(); it != m_skillCoolTime.end(); ++ it)
		it->second.Update(iRealTime);

	//	Gather time counter
	if (m_GatherCnt.IncCounter(iRealTime))
		m_GatherCnt.Reset(true);

	//	Incant time counter
	if (m_IncantCnt.IncCounter(iRealTime))
		m_IncantCnt.Reset(true);

	//	Bind command cool counter
	if (m_BindCmdCoolCnt.IncCounter(iRealTime))
		m_BindCmdCoolCnt.Reset(true);

	//  Auto fashion time counter
	if (m_bAutoFashion && GetBoothState() != 2 && !IsShapeChanged())
	{
		if(m_AutoFashionCnt.IncCounter(iRealTime))
		{
			if(!CheckAutoFashionCondition())
			{
				while(!EquipFashionBySuitID( (m_iCurFashionSuitID+1) % GetMaxFashionSuitNum() )) 
					m_iCurFashionSuitID++;
	
				m_AutoFashionCnt.Reset();
			}
		}
	}

	//  自动转换完美银票
	if( m_AutoYinpiao.open )
	{
		if( m_AutoYinpiao.cnt.IncCounter(iRealTime) )
		{
			ExchangeYinpiao();
			m_AutoYinpiao.cnt.Reset();
		}
	}

	//  Control the dialog of the target item
	m_TargetItemDlgCtrl.Update(iRealTime);

	//	For some reasons on server, sometimes friend list couldn't be got
	//	successfully. Try to get it again every 20s if this case really happen
	if (m_pFriendMan && !m_pFriendMan->CheckInit())
	{
		m_iGetFriendCnt -= iRealTime;
		if (m_iGetFriendCnt < 0)
		{
			g_pGame->GetGameSession()->friend_GetList();
			m_iGetFriendCnt = 60000;
		}
	}
	
	//	Duel stopping time counter
	if (m_pvp.iDuelState == DUEL_ST_PREPARE)
	{
		m_pvp.iDuelTimeCnt -= iRealTime;
		if (m_pvp.iDuelTimeCnt < 0)
			m_pvp.iDuelTimeCnt = 0;
	}
	else if (m_pvp.iDuelState == DUEL_ST_INDUEL)
	{
		m_pvp.iDuelTimeCnt += iRealTime;
	}
	else if (m_pvp.iDuelState == DUEL_ST_STOPPING)
	{
		m_pvp.iDuelTimeCnt -= iRealTime;
		if (m_pvp.iDuelTimeCnt < 0)
		{
			m_pvp.iDuelTimeCnt = 0;
			m_pvp.iDuelState = DUEL_ST_NONE;
			m_pvp.idDuelOpp = 0;
		}
	}

	//	Update pariah time counter
	if (m_dwPariahTime)
	{
		if (m_dwPariahTime > (DWORD)iRealTime)
			m_dwPariahTime -= (DWORD)iRealTime;
		else
			m_dwPariahTime = 0;
	}

	//	Update pet operation time counter
	m_PetOptCnt.IncCounter(iRealTime);

	//	Update battle result time counter
	if (IsInBattle() && !IsInFortress() && m_BattleInfo.iResult && m_BattleInfo.iResultCnt)
	{
		if ((m_BattleInfo.iResultCnt -= iRealTime) < 0)
			m_BattleInfo.iResultCnt = 0;
	}

	//	Update pet corral
	if (m_pPetCorral)
		m_pPetCorral->Tick((DWORD)iRealTime);

	//  Update the related people
	abase::hash_map<int, int>::iterator itP = m_RelatedPlayer.begin();
	while( itP != m_RelatedPlayer.end() )
	{
		itP->second -= iRealTime;
		if( itP->second <= 0 )
			itP = m_RelatedPlayer.erase(itP);
		else
			++itP;
	}
}

//	Update GFXs
void CECHostPlayer::UpdateGFXs(DWORD dwDeltaTime)
{
//	if (m_pLevelUpGFX)
//		m_pLevelUpGFX->SetParentTM(GetAbsoluteTM());

	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();

	if (m_pHoverGFX)// && m_idCurHover != m_idSelTarget)
	{
		if (!IsChangingFace() && (ISPLAYERID(m_idCurHover) || ISNPCID(m_idCurHover)))
		{
			CECObject* pObject = pWorld->GetObject(m_idCurHover, 1);
			if (pObject)
			{
				if (m_pHoverGFX->GetState() == ST_STOP)
					m_pHoverGFX->Start();

				m_pHoverGFX->SetParentTM(pObject->GetAbsoluteTM());
			}
			else
				m_pHoverGFX->Stop();
		}
		else
			m_pHoverGFX->Stop();
	}

	if (m_pSelectedGFX)
	{
		if (!IsChangingFace() && (ISPLAYERID(m_idSelTarget) || ISNPCID(m_idSelTarget)))
		{
			CECObject* pObject = pWorld->GetObject(m_idSelTarget, 1);
			if (pObject)
			{
				if (m_pSelectedGFX->GetState() == ST_STOP)
					m_pSelectedGFX->Start();

				m_pSelectedGFX->SetParentTM(pObject->GetAbsoluteTM());
			}
			else
				m_pSelectedGFX->Stop();
		}
		else
			m_pSelectedGFX->Stop();
	}

	if (m_pFloatDust)
	{
		A3DTerrainWater* pWater = g_pGame->GetGameRun()->GetWorld()->GetTerrainWater();

		if (pWater->IsUnderWater(m_CameraCoord.GetPos()))
		{
			if (m_pFloatDust->GetState() == ST_STOP)
			{
				m_pFloatDust->Start(true);
				m_pFloatDust->TickAnimation(2000);
			}

			m_pFloatDust->SetParentTM(GetAbsoluteTM());
		}
		else if (m_pFloatDust->GetState() != ST_STOP)
			m_pFloatDust->Stop();
	}
	UpdateMonsterSpiritGfx(dwDeltaTime);
}

/*	Check whether player can use specified equipment

	Return true if player can use specified equipment, otherwise return false.

	pEquip: specified equipment.
	piReason (out): store refuse reason if false returned. reason is defined as blow:
		
		reason: 0 - player can use equipment, no refuse reason
				1 - invalid equipment
				2 - player's ability doesn't meet.
				3 - player's profession doesn't meet.
				4 - player's gender doesn't meet.
				5 - not enough ammo or ammo type doesn't meet.
		
*/
bool CECHostPlayer::CanUseEquipment(CECIvtrEquip* pEquip, int* piReason/* NULL */)
{
	int iReason = 0;

	if (!pEquip)
	{
		ASSERT(pEquip);
		iReason = 1;
		goto End;
	}

	//	Equipment's endurance will be checked separatly in other place,
	//	so ignore it here
	if (GetMaxLevelSofar() < pEquip->GetLevelRequirement() ||
		m_ExtProps.bs.strength < pEquip->GetStrengthRequirement() ||
		m_ExtProps.bs.agility < pEquip->GetAgilityRequirement() ||
		m_ExtProps.bs.vitality < pEquip->GetVitalityRequirement() ||
		m_ExtProps.bs.energy < pEquip->GetEnergyRequirement() ||
		GetReputation() < pEquip->GetReputationRequirement())
	{
		iReason = 2;
		goto End;
	}

	switch (pEquip->GetClassID())
	{
	case CECIvtrItem::ICID_ARROW:
	{
	//	if (!CanUseProjectile((CECIvtrArrow*)pEquip))
	//		return false;

		break;
	}
	case CECIvtrItem::ICID_WING:
	{
		if (m_iProfession != PROF_ARCHOR && m_iProfession != PROF_ANGEL)
			iReason = 3;

		break;
	}
	case CECIvtrItem::ICID_FLYSWORD:
	{
		CECIvtrFlySword* pFlySword = (CECIvtrFlySword*)pEquip;
		if (!(pFlySword->GetProfRequirement() & (1 << m_iProfession)))
			iReason = 3;

		break;
	}
	case CECIvtrItem::ICID_FASHION:
	{
		CECIvtrFashion* pFashion = (CECIvtrFashion*)pEquip;
		if (pFashion->GetGenderRequirement() != m_iGender)
			iReason = 4;
		const FASHION_ESSENCE* pfEssion = pFashion->GetDBEssence();
		if (pfEssion->equip_location == enumSkinShowHand 
			&& !(pfEssion->character_combo_id & (1 << m_iProfession)))
			iReason = 3;

		break;
	}
	case CECIvtrItem::ICID_ARMOR:
	case CECIvtrItem::ICID_DECORATION:
	{
		if (!(pEquip->GetProfessionRequirement() & (1 << m_iProfession)))
			iReason = 3;

		break;
	}
	case CECIvtrItem::ICID_WEAPON:
	{
		if (!(pEquip->GetProfessionRequirement() & (1 << m_iProfession)))
			iReason = 3;
		else
		{
			//	If the weapon is a range weapon and just the one in host's hand
			//	(in equipment pack), check ammo type.
			if (pEquip == m_pEquipPack->GetItem(EQUIPIVTR_WEAPON))
			{
				CECIvtrWeapon* pWeapon = (CECIvtrWeapon*)pEquip;
				CECIvtrItem* pArrow = m_pEquipPack->GetItem(EQUIPIVTR_PROJECTILE);
				if (pWeapon->IsRangeWeapon() && !CanUseProjectile((CECIvtrArrow*)pArrow))
					iReason = 5;
			}
		}

		break;
	}
	case CECIvtrItem::ICID_GENERALCARD:
	{
			if (!CanEquipCard(pEquip))
				iReason = 3;
			
			break;
	}
	}
	
End:

	if (piReason)
		*piReason = iReason;

	return iReason == 0 ? true : false;
}

//	Check whetehr player rcan use specified projectile
bool CECHostPlayer::CanUseProjectile(CECIvtrArrow* pArrow)
{
	if (!pArrow)
		return false;

	CECIvtrWeapon* pWeapon = (CECIvtrWeapon*)m_pEquipPack->GetItem(EQUIPIVTR_WEAPON);
	if (!pWeapon)
		return false;

	const IVTR_ESSENCE_WEAPON& we = pWeapon->GetEssence();
	if (we.weapon_type != WEAPONTYPE_RANGE)
		return false;

	const IVTR_ESSENCE_ARROW& ae = pArrow->GetEssence();
	if (we.require_projectile != (int)pArrow->GetDBSubType()->id ||
		we.weapon_level < ae.iWeaponReqLow || we.weapon_level > ae.iWeaponReqHigh)
		return false;

	return true;
}

bool CECHostPlayer::CanTransferCash()
{
	return CanDo(CANDO_MOVETO) && !IsInBattle()
		   && CanDo(CANDO_BINDBUDDY) && !IsInDuel();
}

//	Check ammo amount.
//	Return the percent amount of current ammo if player use range weapon, 
//	for free hands or melee weapon this function always return 1.0f
float CECHostPlayer::CheckAmmoAmount()
{
	CECIvtrWeapon* pWeapon = (CECIvtrWeapon*)m_pEquipPack->GetItem(EQUIPIVTR_WEAPON);
	if (!pWeapon || !pWeapon->IsRangeWeapon())
		return 1.0f;

	CECIvtrArrow* pArrow = (CECIvtrArrow*)m_pEquipPack->GetItem(EQUIPIVTR_PROJECTILE);
	if (!pArrow || !CanUseProjectile(pArrow))
		return 0.0f;

//	float ret = (float)pArrow->GetCount() / pArrow->GetPileLimit();
//	a_Clamp(ret, 0.0f, 1.0f);
//	return ret;	
	int iArrowCount = pArrow->GetCount();
	if( iArrowCount < 500)
		return 0.1f;

	return 0.5f;
}

//	Is spelling magic
bool CECHostPlayer::IsSpellingMagic()
{
	return m_pWorkMan->IsSpellingMagic();
}

//	Is flash moving ?
bool CECHostPlayer::IsFlashMoving()
{
	return m_pWorkMan->IsFlashMoving();
}

//	Is taking off ?
bool CECHostPlayer::IsTakingOff()
{
	return m_pWorkMan->IsFlyingOff();
}

//	Is picking up something
bool CECHostPlayer::IsPicking()
{
	if (CECHPWork *pWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_PICKUP))
		return !dynamic_cast<CECHPWorkPick*>(pWork)->IsGather();
	else
		return false;
}
//  Is auto moving ?
bool CECHostPlayer::IsAutoMoving()
{
	if (CECHPWork* pWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_MOVETOPOS))
		return dynamic_cast<CECHPWorkMove*>(pWork)->GetAutoMove();
	else
		return false;
}

//	Is gathering resources
bool CECHostPlayer::IsGathering()
{
	if (CECHPWork* pWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_PICKUP))
		return dynamic_cast<CECHPWorkPick*>(pWork)->IsGather();
	else
		return false;
}

//	Is reviving
bool CECHostPlayer::IsReviving()
{
	return m_pWorkMan->IsReviving();
}

//	Is using item ?
bool CECHostPlayer::IsUsingItem()
{
	return m_pWorkMan->IsUsingItem();
}

//  Is following someone
bool CECHostPlayer::IsFollowing()
{
	return m_pWorkMan->IsFollowing();
}
//	Is doing session pose ?
bool CECHostPlayer::DoingSessionPose()
{
	if (CECHPWork* pWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_STAND))
		return dynamic_cast<CECHPWorkStand*>(pWork)->DoingSessionPose();
	else
		return false;
}

//	Update item shortcut when item position changed
void CECHostPlayer::UpdateMovedItemSC(int tidItem, int iSrcIvtr, int iSrcSlot, int iDstIvtr, int iDstSlot)
{
	int i;

	//	Update shortcut sets 1
	for (i=0; i < NUM_HOSTSCSETS1; i++)
	{
		if (m_aSCSets1[i])
			m_aSCSets1[i]->UpdateMovedItemSC(tidItem, iSrcIvtr, iSrcSlot, iDstIvtr, iDstSlot);
	}

	//	Update shortcut sets 2
	for (i=0; i < NUM_HOSTSCSETS2; i++)
	{
		if (m_aSCSets2[i])
			m_aSCSets2[i]->UpdateMovedItemSC(tidItem, iSrcIvtr, iSrcSlot, iDstIvtr, iDstSlot);
	}

	//  Update booth package
	if(iSrcIvtr == IVTRTYPE_PACK)
	{
		// Remove item if
		if(iDstIvtr != iSrcIvtr)
		{
			this->GetBoothSellPack()->UpdateRemovedBoothItem(iSrcSlot);
			this->GetBoothBuyPack()->UpdateRemovedBoothItem(iSrcSlot);
		}
		else
		{
			int tid = m_pPack->GetItem(iDstSlot)->GetTemplateID();
			bool bSrcRemoved = (GetPack()->GetItem(iSrcSlot) == NULL);

			CECDealInventory *pBoothBuyPack = GetBoothBuyPack();
			CECDealInventory *pBoothSellPack = GetBoothSellPack();

			bool bDstReferencedByOthers = pBoothBuyPack->ValidateItem(iDstSlot, tid);
			pBoothSellPack->UpdateMovedBoothItem(iSrcSlot, iDstSlot, tid, bSrcRemoved, bDstReferencedByOthers);

			bDstReferencedByOthers = pBoothSellPack->ValidateItem(iDstSlot, tid);
			pBoothBuyPack->UpdateMovedBoothItem(iSrcSlot, iDstSlot, tid, bSrcRemoved, bDstReferencedByOthers);
		}
	}
}

//	Update item shortcut when item removed
void CECHostPlayer::UpdateRemovedItemSC(int tidItem, int iIvtr, int iSlot,
								int* aExcSlots/* NULL */, int iNumExcSlot/* 0 */)
{
	int i, iSameItem = -1;

	//	Are there some same kind items exists ? Some items with different
	//	id but have the same effect. For example, when we counldn't find
	//	middle heal bottle, we can use small or large heal bottles as candidates.
	if (iIvtr == IVTRTYPE_PACK)
	{
		int iCandidate = -1;

		for (i=0; i < m_pPack->GetSize(); i++)
		{
			CECIvtrItem* pItem = m_pPack->GetItem(i);
			if (i == iSlot || !pItem)
				continue;

			//	Check exclude slots
			if (aExcSlots && iNumExcSlot)
			{
				int j(0);
				for (j=0; j < iNumExcSlot; j++)
				{
					if (i == aExcSlots[j])
						break;
				}

				if (j < iNumExcSlot)
					continue;
			}

			int idSlotItem = pItem->GetTemplateID();
			if (idSlotItem == tidItem)
			{
				iSameItem = i;
				break;
			}
			else if (iCandidate < 0 && CECIvtrItem::IsCandidate(tidItem, pItem))
				iCandidate = i;
		}

		if (i >= m_pPack->GetSize() && iCandidate >= 0)
			iSameItem = iCandidate;

		//  Update booth package
		this->GetBoothSellPack()->UpdateRemovedBoothItem(iSlot);
		this->GetBoothBuyPack()->UpdateRemovedBoothItem(iSlot);
	}

	//	Update horizontal shortcut sets
	for (i=0; i < NUM_HOSTSCSETS1; i++)
	{
		if (m_aSCSets1[i])
			m_aSCSets1[i]->UpdateRemovedItemSC(tidItem, iIvtr, iSlot, iSameItem);
	}

	//	Update vertical shortcut sets
	for (i=0; i < NUM_HOSTSCSETS2; i++)
	{
		if (m_aSCSets2[i])
			m_aSCSets2[i]->UpdateRemovedItemSC(tidItem, iIvtr, iSlot, iSameItem);
	}
}

//	Update item shortcut when two items exchanged
void CECHostPlayer::UpdateExchangedItemSC(int tidItem1, int iIvtr1, int iSlot1, int tidItem2, int iIvtr2, int iSlot2)
{
	int i;

	//	Update shortcut sets 1
	for (i=0; i < NUM_HOSTSCSETS1; i++)
	{
		if (m_aSCSets1[i])
			m_aSCSets1[i]->UpdateExchangedItemSC(tidItem1, iIvtr1, iSlot1, tidItem2, iIvtr2, iSlot2);
	}

	//	Update shortcut sets 2
	for (i=0; i < NUM_HOSTSCSETS2; i++)
	{
		if (m_aSCSets2[i])
			m_aSCSets2[i]->UpdateExchangedItemSC(tidItem1, iIvtr1, iSlot1, tidItem2, iIvtr2, iSlot2);
	}

	
	//  Update booth package
	if(iIvtr1 == IVTRTYPE_PACK || iIvtr2 == IVTRTYPE_PACK)
	{
		if(iIvtr2 != iIvtr1)
		{
			this->GetBoothSellPack()->UpdateRemovedBoothItem(iIvtr1 == IVTRTYPE_PACK ? iSlot1:iSlot2);
			this->GetBoothBuyPack()->UpdateRemovedBoothItem(iIvtr1 == IVTRTYPE_PACK ? iSlot1:iSlot2);
		}
		else
		{
			this->GetBoothSellPack()->UpdateExchangedBoothItem(iSlot1, m_pPack->GetItem(iSlot2)->GetTemplateID(), iSlot2, m_pPack->GetItem(iSlot1)->GetTemplateID());
			this->GetBoothBuyPack()->UpdateExchangedBoothItem(iSlot1, m_pPack->GetItem(iSlot2)->GetTemplateID(), iSlot2, m_pPack->GetItem(iSlot1)->GetTemplateID());
		}
	}
}

//	Update pet shortcut when pet freeed
void CECHostPlayer::UpdateFreedPetSC(int iPetIndex)
{
	int i;

	//	Update shortcut sets 1
	for (i=0; i < NUM_HOSTSCSETS1; i++)
	{
		if (m_aSCSets1[i])
			m_aSCSets1[i]->UpdateFreedPetSC(iPetIndex);
	}

	//	Update shortcut sets 2
	for (i=0; i < NUM_HOSTSCSETS2; i++)
	{
		if (m_aSCSets2[i])
			m_aSCSets2[i]->UpdateFreedPetSC(iPetIndex);
	}
}

//	Prepare to begin NPC service
void CECHostPlayer::PrepareNPCService(int idSev)
{
	if (!ISNPCID(m_idSevNPC))
	{
		ASSERT(0);
		return;
	}

	DATA_TYPE DataType;
	const void* pBuf = g_pGame->GetElementDataMan()->get_data_ptr(idSev, ID_SPACE_ESSENCE, DataType);

	switch (DataType)
	{
	case DT_NPC_TALK_SERVICE:

		break;

	case DT_NPC_SELL_SERVICE:
	case DT_NPC_BUY_SERVICE:
	{
		//	Get NPC's tex rate
		float fScale = 1.0f;
		CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(m_idSevNPC);
		if (pNPC && pNPC->IsServerNPC())
		{
			CECNPCServer* pServer = (CECNPCServer*)pNPC;
			fScale = (1.0f + pServer->GetTaxRate()) * pServer->GetPriceScale();
		}

		//	Fill NPC package
		NPC_SELL_SERVICE* pData = (NPC_SELL_SERVICE*)pBuf;

		int	id_goods[IVTRSIZE_NPCPACK] = {0};
		for (int j=0; j < NUM_NPCIVTR; j++)
		{
			for (int i = 0; i < IVTRSIZE_NPCPACK; ++ i)
				id_goods[i] = pData->pages[j].goods[i].id;
			FillNPCPack(j, pData->pages[j].page_title, id_goods, fScale, false);
		}
		
		//	Clear packs
		m_pBuyPack->RemoveAllItems();
		m_pSellPack->RemoveAllItems();
		break;
	}
	case DT_NPC_SKILL_SERVICE:
	case DT_NPC_PETLEARNSKILL_SERVICE:
	{
		CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(m_idSevNPC);
		if (!pNPC || !pNPC->IsServerNPC())
		{
			ASSERT(pNPC && pNPC->IsServerNPC());
			return;
		}

		if (DataType == DT_NPC_SKILL_SERVICE)
			((CECNPCServer*)pNPC)->BuildSkillList(idSev);
		else
			((CECNPCServer*)pNPC)->BuildPetSkillList();
		
		break;
	}
	case DT_NPC_REPAIR_SERVICE:
	case DT_NPC_INSTALL_SERVICE:
	case DT_NPC_UNINSTALL_SERVICE:
	case DT_NPC_TASK_IN_SERVICE:
	case DT_NPC_TASK_OUT_SERVICE:
	case DT_NPC_TASK_MATTER_SERVICE:
	case DT_NPC_HEAL_SERVICE:
	case DT_NPC_TRANSMIT_SERVICE:
	case DT_NPC_TRANSPORT_SERVICE:
	case DT_NPC_PROXY_SERVICE:
	case DT_NPC_STORAGE_SERVICE:
	case DT_NPC_DECOMPOSE_SERVICE:
	case DT_NPC_PETNAME_SERVICE:
	case DT_NPC_PETFORGETSKILL_SERVICE:

		break;

	case DT_NPC_MAKE_SERVICE:
	{
		//	Fill NPC package
		NPC_MAKE_SERVICE* pData = (NPC_MAKE_SERVICE*)pBuf;

		for (int j=0; j < NUM_NPCIVTR; j++)
			FillNPCPack(j, pData->pages[j].page_title, (int*)pData->pages[j].id_goods, 1.0f, true);
		
		//	Clear deal pack
		m_pDealPack->RemoveAllItems();
		break;
	}

	case DT_NPC_RANDPROP_SERVICE:
	{
		NPC_RANDPROP_SERVICE* pData = (NPC_RANDPROP_SERVICE*)pBuf;
		elementdataman *pDataMan = g_pGame->GetElementDataMan();

		//	Fill equip data into NPC pack
		ASSERT( sizeof(pData->pages)/sizeof(pData->pages[0]) == NUM_NPCIVTR );
		for (int j=0; j < NUM_NPCIVTR; j++)
		{
			unsigned int id_recipe = pData->pages[j].id_recipe;
			DATA_TYPE dt = DT_INVALID;
			RANDPROP_ESSENCE *pRecipe = (RANDPROP_ESSENCE *)pDataMan->get_data_ptr(id_recipe, ID_SPACE_RECIPE, dt);

			if (pRecipe && dt == DT_RANDPROP_ESSENCE)
			{
				FillNPCPack(j, pData->pages[j].page_title, (int*)pRecipe->equip_id, 1.0f, false);
			}
			else
			{
				// skip the invalid recipe id
				GetNPCSevPack(j)->RemoveAllItems();
				GetNPCSevPack(j)->SetName(_AL(""));
			}
		}
		
		//	Clear deal pack
		m_pDealPack->RemoveAllItems();
		break;

	}

	}
}

//	End NPC service
void CECHostPlayer::EndNPCService()
{
	m_idSevNPC = 0;
	m_bTalkWithNPC = false;
	m_iBoothState = 0;
	m_bIsInKingService = false;
	m_pOffShopCtrl->SetNPCSevFlag(COfflineShopCtrl::NPCSEV_NULL);
}

//	Buy items from NPC
void CECHostPlayer::BuyItemsFromNPC()
{
	using namespace C2S;

	//	Check package content at first
	if (m_pBuyPack->GetSize() - m_pBuyPack->GetEmptySlotNum() > m_pPack->GetEmptySlotNum())
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_PACKFULL1);
		return;
	}

	AArray<npc_trade_item, npc_trade_item&> aItems(0, m_pBuyPack->GetSize());
	int iPrice = 0;

	for (int i=0; i < m_pBuyPack->GetSize(); i++)
	{
		CECIvtrItem* pItem = m_pBuyPack->GetItem(i);
		if (!pItem)
			continue;

		const CECDealInventory::ITEMINFO& ItemInfo = m_pBuyPack->GetItemInfo(i);

		npc_trade_item dst;
		dst.tid		= pItem->GetTemplateID();
		dst.index	= ItemInfo.iOrigin;
		dst.count	= ItemInfo.iAmount;
		aItems.Add(dst);

		iPrice += CECIvtrItem::GetScaledPrice(pItem->GetUnitPrice(), ItemInfo.iAmount, 
			CECIvtrItem::SCALE_SELL, PLAYER_PRICE_SCALE);
	}

	if (aItems.GetSize() && iPrice <= m_iMoneyCnt)
		g_pGame->GetGameSession()->c2s_CmdNPCSevBuy(aItems.GetSize(), aItems.GetData());

	//	Clear buy pack
	m_pBuyPack->RemoveAllItems();
}

//	Sell items to NPC
void CECHostPlayer::SellItemsToNPC()
{
	using namespace C2S;

	AArray<npc_sell_item, npc_sell_item&> aItems(0, m_pSellPack->GetSize());

	for (int i=0; i < m_pSellPack->GetSize(); i++)
	{
		CECIvtrItem* pItem = m_pSellPack->GetItem(i);
		if (!pItem)
			continue;

		const CECDealInventory::ITEMINFO& ItemInfo = m_pSellPack->GetItemInfo(i);

		npc_sell_item dst;
		dst.tid		= pItem->GetTemplateID();
		dst.index	= ItemInfo.iOrigin;
		dst.count	= ItemInfo.iAmount;

		if( pItem->GetClassID() == CECIvtrItem::ICID_MONEYCONVERTIBLE )
			dst.price = pItem->GetUnitPrice() * m_nCashMoneyRate;
		else
			dst.price = pItem->GetUnitPrice();

		aItems.Add(dst);
	}

	if (aItems.GetSize())
		g_pGame->GetGameSession()->c2s_CmdNPCSevSell(aItems.GetSize(), aItems.GetData());

	//	Clear sell pack
	m_pSellPack->RemoveAllItems();
}

//	Buy items from else player's booth
void CECHostPlayer::BuyItemsFromBooth(int iYinpiao)
{
	using namespace C2S;

	//	Check package content at first
	if (m_pBuyPack->GetSize() - m_pBuyPack->GetEmptySlotNum() > m_pPack->GetEmptySlotNum())
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_PACKFULL1);
		return;
	}

	AArray<npc_booth_item, npc_booth_item&> aItems(0, m_pBuyPack->GetSize());
	int iPrice = 0;

	for (int i=0; i < m_pBuyPack->GetSize(); i++)
	{
		CECIvtrItem* pItem = m_pBuyPack->GetItem(i);
		if (!pItem)
			continue;

		const CECDealInventory::ITEMINFO& ItemInfo = m_pBuyPack->GetItemInfo(i);

		npc_booth_item dst;
		dst.tid			= pItem->GetTemplateID();
		dst.index		= ItemInfo.iOrigin;
		dst.inv_index	= 0;
		dst.count		= ItemInfo.iAmount;
		aItems.Add(dst);

		iPrice += CECIvtrItem::GetScaledPrice(pItem->GetUnitPrice(), ItemInfo.iAmount, 
			CECIvtrItem::SCALE_BOOTH, 1.0f);
	}

	if (aItems.GetSize() && iPrice <= m_iMoneyCnt + iYinpiao*10000000)
		g_pGame->GetGameSession()->c2s_CmdNPCSevBoothBuy(m_idBoothTrade, aItems.GetSize(), aItems.GetData(), iYinpiao);

	//	Clear buy pack
	m_pBuyPack->RemoveAllItems();
}

//	Sell items to else player's booth
void CECHostPlayer::SellItemsToBooth()
{
	using namespace C2S;

	AArray<npc_booth_item, npc_booth_item&> aItems(0, m_pSellPack->GetSize());

	for (int i=0; i < m_pSellPack->GetSize(); i++)
	{
		CECIvtrItem* pItem = m_pSellPack->GetItem(i);
		if (!pItem)
			continue;

		const CECDealInventory::ITEMINFO& ItemInfo = m_pSellPack->GetItemInfo(i);

		npc_booth_item dst;
		dst.tid			= pItem->GetTemplateID();
		dst.inv_index	= ItemInfo.iFlag;
		dst.index		= ItemInfo.iOrigin;
		dst.count		= ItemInfo.iAmount;
		aItems.Add(dst);
	}

	if (aItems.GetSize())
		g_pGame->GetGameSession()->c2s_CmdNPCSevBoothSell(m_idBoothTrade, aItems.GetSize(), aItems.GetData());

	//	Clear sell pack
	m_pSellPack->RemoveAllItems();
}

//	When the booth we are visiting closed
void CECHostPlayer::OnOtherBoothClosed()
{
	EndNPCService();
	
	//	Unfreeze items dragged into sell pack
	for (int i=0; i < m_pSellPack->GetSize(); i++)
	{
		if (m_pSellPack->GetItem(i))
		{
			const CECDealInventory::ITEMINFO& ii = m_pSellPack->GetItemInfo(i);
			CECIvtrItem* pItem = m_pPack->GetItem(ii.iFlag);
			if (pItem)
				pItem->Freeze(false);
		}
	}
	
	//	Else player close booth
	m_pEPBoothSPack->RemoveAllItems();
	m_pEPBoothBPack->RemoveAllItems();

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->PopupBoothDialog(false, false);
}

//	Open booth
void CECHostPlayer::OpenBooth(const ACHAR* szName)
{
	int i, iCount=0;
	int iSellSize = m_pBoothSPack->GetSize();
	int iBuySize = m_pBoothBPack->GetSize();

	C2S::cmd_open_booth::entry_t* aEntries = (C2S::cmd_open_booth::entry_t*)a_malloctemp(sizeof(C2S::cmd_open_booth::entry_t)*(iSellSize + iBuySize));
		//[IVTRSIZE_BOOTHSPACK + IVTRSIZE_BOOTHBPACK];

	for (i=0; i < iSellSize; i++)
	{
		CECIvtrItem *pItem = m_pBoothSPack->GetItem(i);
		if (!pItem)
			continue;

		//	Record booth index
		m_pBoothSPack->SetItemInfoFlag(i, iCount);

		const CECDealInventory::ITEMINFO& ii = m_pBoothSPack->GetItemInfo(i);
		aEntries[iCount].type = pItem->GetTemplateID();
		aEntries[iCount].index = ii.iOrigin;
		aEntries[iCount].count = ii.iAmount;
		aEntries[iCount].price = pItem->GetUnitPrice();
		iCount++;
	}

	for (i=0; i < iBuySize; i++)
	{
		CECIvtrItem *pItem = m_pBoothBPack->GetItem(i);
		if (!pItem)
			continue;

		//	Record booth index
		m_pBoothBPack->SetItemInfoFlag(i, iCount);

		const CECDealInventory::ITEMINFO& ii = m_pBoothBPack->GetItemInfo(i);
		aEntries[iCount].type = pItem->GetTemplateID();
		aEntries[iCount].index = 0xFFFF;
		aEntries[iCount].count = ii.iAmount;
		aEntries[iCount].price = pItem->GetUnitPrice();
		iCount++;
	}

	g_pGame->GetGameSession()->c2s_CmdOpenBooth(iCount, szName, aEntries);

	a_freetemp(aEntries);

	SetBoothName(szName);

	//	Stop current work and restore to stand state
	if (IsFollowing())
		m_pWorkMan->FinishAllWork(true);
	// save the booth setting when open booth
	g_pGame->GetGameRun()->SaveConfigsToServer();
}

//	Release skills
void CECHostPlayer::ReleaseSkills()
{
	int i;

	//	Release positive skills
	for (i=0; i < m_aPtSkills.GetSize(); i++)
		delete m_aPtSkills[i];
	m_aPtSkills.RemoveAll(false);

	//	Release passive skills
	for (i=0; i < m_aPsSkills.GetSize(); i++)
		delete m_aPsSkills[i];
	m_aPsSkills.RemoveAll(false);

	//	Release goblin skills
	for (i=0; i < m_aGoblinSkills.GetSize(); i++)
		delete m_aGoblinSkills[i];
	m_aGoblinSkills.RemoveAll(false);

	for (i=0; i < m_aEquipSkills.GetSize(); i++)
		delete m_aEquipSkills[i];
	m_aEquipSkills.RemoveAll(false);

	//  Release target item skill
	if(m_pTargetItemSkill)
	{
		delete m_pTargetItemSkill;
		m_pTargetItemSkill = NULL;
	}
}

//	Update equipment skins
bool CECHostPlayer::UpdateEquipSkins()
{
	int i, aNewEquips[IVTRSIZE_EQUIPPACK];

	for (i=0; i < IVTRSIZE_EQUIPPACK; i++)
	{
		CECIvtrItem* pItem = m_pEquipPack->GetItem(i);
		if (pItem)
		{
			if( i == EQUIPIVTR_SPEAKER )
				m_nCurEmotionSet = ((CECIvtrSpeaker *)pItem)->GetDBEssence()->id_icon_set;

			aNewEquips[i] = pItem->GetTemplateID();
			if( ((i >= EQUIPIVTR_FASHION_BODY && i <= EQUIPIVTR_FASHION_WRIST) || i == EQUIPIVTR_FASHION_HEAD ) && 
				pItem->GetClassID() == CECIvtrItem::ICID_FASHION )
			{
				CECIvtrFashion * pFashionItem = (CECIvtrFashion *) pItem;
				aNewEquips[i] |= (pFashionItem->GetWordColor() << 16) & 0x7fffffff;
			}
			else
			{
				CECIvtrEquip * pEquip = (CECIvtrEquip *) pItem;
				WORD stoneStatus = pEquip ? pEquip->GetStoneMask() : 0;
				aNewEquips[i] |= (stoneStatus << 16) & 0x7fffffff;
			}

			if( i == EQUIPIVTR_CERTIFICATE )
			{
				CECIvtrCertificate* pTempItem = (CECIvtrCertificate*) pItem;
				UpdateBoothPack(pTempItem->GetBuyColNum(), pTempItem->GetSellColNum());				
			}

			if( i == EQUIPIVTR_HEAD && !InFashionMode())
			{
				UpdateHairModel(true, aNewEquips[i]);			
			}
			
			if( i== EQUIPIVTR_FASHION_HEAD && InFashionMode())
			{
				UpdateHairModel(true, aNewEquips[i]);
			}
		}
		else
		{
			aNewEquips[i] = 0;
			if( i == EQUIPIVTR_SPEAKER )
				m_nCurEmotionSet = 0;

			// Player drop goblin
			if( i == EQUIPIVTR_GOBLIN )
			{
				if(m_pGoblin)
				{
					m_pGoblin->Release();
					delete m_pGoblin;
					m_pGoblin = NULL;
					m_bRenderGoblin = false;
				}
			}

			if( i == EQUIPIVTR_CERTIFICATE )
			{
				UpdateBoothPack(IVTRSIZE_BOOTHBPACK, IVTRSIZE_BOOTHSPACK);
			}

			if( i == EQUIPIVTR_HEAD && !InFashionMode())
			{
				UpdateHairModel(true);		
			}

			if( i == EQUIPIVTR_FASHION_HEAD && InFashionMode())
			{
				UpdateHairModel(true);
			}
		}
	}

	ShowEquipments(aNewEquips);

	SetResReadyFlag(RESFG_SKIN, true);

	return true;
}

//	Get positive skill by id
CECSkill* CECHostPlayer::GetPositiveSkillByID(int id, bool bSenior/* false */)
{
	CECSkill* pSenior = NULL;

	for (int i=0; i < m_aPtSkills.GetSize(); i++)
	{
		if (m_aPtSkills[i]->GetSkillID() == id)
			return m_aPtSkills[i];
		else if (m_aPtSkills[i]->GetJunior().Find(id))
			pSenior = m_aPtSkills[i];
	}

	if (bSenior && pSenior)
		return pSenior;

	return NULL;
}

//	Get passive skill by id
CECSkill* CECHostPlayer::GetPassiveSkillByID(int id, bool bSenior/* false */)
{
	CECSkill* pSenior = NULL;

	for (int i=0; i < m_aPsSkills.GetSize(); i++)
	{
		if (m_aPsSkills[i]->GetSkillID() == id)
			return m_aPsSkills[i];
		else if (m_aPsSkills[i]->GetJunior().Find(id))
			pSenior = m_aPsSkills[i];
	}

	if (bSenior && pSenior)
		return pSenior;

	return NULL;
}

CECSkill* CECHostPlayer::GetEquipSkillByID(int id)
{
	CECSkill *pRet = NULL;

	for (int i=0; i < GetEquipSkillNum(); i++)
	{
		CECSkill *pSkill = GetEquipSkillByIndex(i);
		if (pSkill->GetSkillID() == id)
		{
			pRet = pSkill;
			break;
		}
	}

	return pRet;
}


bool CECHostPlayer::UpdateEquipSkills(bool bShowSkillChange/* =false */)
{
	//	根据装备包裹中技能物品，更新 HostPlayer 主动技能
	//	bShowSkillChange 为 true 时，若有技能变化，则显示系统提示及技能面板

	bool bUpdated(false);

	while (true)
	{
		CECInventory *pEquipPack = GetPack(IVTRTYPE_EQUIPPACK);
		if (!pEquipPack)	break;

		using namespace std;

		int i(0);

		//	汇集原有装备技能
		set<int> oldEquipSkillIDs;
		for (i=0; i < GetEquipSkillNum(); ++ i)
		{
			CECSkill *pSkill = GetEquipSkillByIndex(i);
			oldEquipSkillIDs.insert(pSkill->GetSkillID());
		}

		//	计算新装备技能
		set<int> newEquipSkillIDs;
		for (i=0; i < pEquipPack->GetSize(); ++ i)
		{
			CECIvtrItem *pItem = pEquipPack->GetItem(i);
			if (!pItem || pItem->GetClassID() != CECIvtrItem::ICID_DYNSKILLEQUIP)
				continue;

			CECIvtrDynSkillEquip *pDynSkillEquip = dynamic_cast<CECIvtrDynSkillEquip *>(pItem);
			if (!pDynSkillEquip || pDynSkillEquip->GetSkillCount() <= 0)
				continue;

			for (int k(0); k < pDynSkillEquip->GetSkillCount(); ++ k)
			{
				CECSkill *pSkill = pDynSkillEquip->GetSkill(k);
				if (pSkill && pSkill->IsGeneralSkill() && pSkill->IsPositiveSkill())
					newEquipSkillIDs.insert(pSkill->GetSkillID());
			}
		}

		//	计算技能增删
		set<int> delSkillIDs, addSkillIDs;
		set_difference(oldEquipSkillIDs.begin(), oldEquipSkillIDs.end(), newEquipSkillIDs.begin(), newEquipSkillIDs.end(), insert_iterator< set<int> >(delSkillIDs, delSkillIDs.end()));
		set_difference(newEquipSkillIDs.begin(), newEquipSkillIDs.end(), oldEquipSkillIDs.begin(), oldEquipSkillIDs.end(), insert_iterator< set<int> >(addSkillIDs, addSkillIDs.end()));
		if (delSkillIDs.empty() && addSkillIDs.empty())
			break;

		//	执行增删操作
		if (!delSkillIDs.empty())
		{
			EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();
			bool bComboSkillChanged(false);
			for (set<int>::iterator it = delSkillIDs.begin(); it != delSkillIDs.end(); ++ it)
			{
				int idSkill = *it;

				RemoveEquipSkill(idSkill);

				for (int i=0; i < EC_COMBOSKILL_NUM; i++)
				{
					EC_COMBOSKILL & cs = vs.comboSkill[i];
					if (cs.nIcon && cs.Clear(idSkill))
						bComboSkillChanged = true;
				}
			}
			if (bComboSkillChanged)
				g_pGame->GetConfigs()->SetVideoSettings(vs);
		}

		if (!addSkillIDs.empty())
		{
			for (set<int>::iterator it=addSkillIDs.begin(); it != addSkillIDs.end(); ++ it)
			{
				int idSkill = *it;
				CECSkill* pSkill = new CECSkill(idSkill, 1);
				m_aEquipSkills.Add(pSkill);
			}
		}

		//	显示新增技能
		if (bShowSkillChange)
		{
			CECGameRun *pGameRun = g_pGame->GetGameRun();
			CECGameUIMan *pGameUIMan = pGameRun->GetUIManager()->GetInGameUIMan();

			//	增加系统提示
			set<int>::iterator it;
			for (it=delSkillIDs.begin(); it != delSkillIDs.end(); ++ it)
			{
				int idSkill = *it;
				CECSkill* pSkill = new CECSkill(idSkill, 1);
				pGameRun->AddFixedMessage(FIXMSG_LOSE_SKILL, pSkill->GetNameDisplay());
				delete pSkill;
			}
			for (it=addSkillIDs.begin(); it != addSkillIDs.end(); ++ it)
			{
				int idSkill = *it;
				CECSkill* pSkill = new CECSkill(idSkill, 1);
				pGameRun->AddFixedMessage(FIXMSG_GAIN_SKILL, pSkill->GetNameDisplay());
				delete pSkill;
			}

			if (!addSkillIDs.empty())
			{
				//	有新增技能时显示技能面板
				/*
				PAUIDIALOG pDlgSkill = pGameUIMan->GetDialog("Win_Skill");
				if (!pDlgSkill->IsShow())
					pDlgSkill->Show(true);
				*/
			}
		}

		bUpdated = true;
		break;
	}

	return bUpdated;
}

bool  CECHostPlayer::IsEquipSkill(CECSkill *pSkill)
{
	bool bFound(false);

	for (int i(0); i < GetEquipSkillNum(); ++ i)
	{
		if (GetEquipSkillByIndex(i) == pSkill)
		{
			bFound = true;
			break;
		}
	}

	return bFound;
}

void CECHostPlayer::UpdateEquipSkillCoolDown(int cooldown_index/* =-1 */)
{
	if (cooldown_index < 0)
	{
		//	更新所有装备技能

		for (int i(0); i < GetEquipSkillNum(); ++ i)
		{
			CECSkill *pSkill = GetEquipSkillByIndex(i);

			//	检查技能冷却
			COOLTIME temp;
			if (GetSkillCoolTime(pSkill->GetSkillID(), temp))
			{
				pSkill->StartCooling(temp.iMaxTime, temp.iCurTime);
				continue;
			}

			//	检查公共冷却
			int ccd = pSkill->GetCommonCoolDown();
			if (!ccd) continue;
			for (int j(0); j<5; ++ j)
			{
				if (ccd & (1 << j))
				{
					const COOLTIME& ct = m_aCoolTimes[GP_CT_SKILLCOMMONCOOLDOWN0+j];
					pSkill->StartCooling(ct.iMaxTime, ct.iCurTime);
					break;
				}
			}
		}
	}
	else
	{
		//	检查更新指定技能
		if (cooldown_index > GP_CT_SKILL_START)
		{
			int idSkill = cooldown_index - GP_CT_SKILL_START;
			CECSkill *pSkill = GetEquipSkillByID(idSkill);
			COOLTIME temp;
			if (pSkill && GetSkillCoolTime(idSkill, temp))
				pSkill->StartCooling(temp.iMaxTime, temp.iCurTime);
		}
		else if (cooldown_index >= GP_CT_SKILLCOMMONCOOLDOWN0 && cooldown_index <= GP_CT_SKILLCOMMONCOOLDOWN4)
		{
			int index = cooldown_index-GP_CT_SKILLCOMMONCOOLDOWN0;
			const COOLTIME& ct = m_aCoolTimes[GP_CT_SKILLCOMMONCOOLDOWN0+index];
			unsigned int mask = 1 << index;
			for (int i(0); i < GetEquipSkillNum(); ++ i)
			{
				CECSkill *pSkill = GetEquipSkillByIndex(i);
				int ccd = pSkill->GetCommonCoolDown();
				if (ccd & mask)
					pSkill->StartCooling(ct.iMaxTime, ct.iCurTime);
			}
		}
	}
}

//  Get goblin skill by id
CECSkill* CECHostPlayer::GetGoblinSkillByID(int id)
{
	CECSkill* pSkill = NULL;
	for (int i=0; i< m_aGoblinSkills.GetSize();i++)
	{
		if(m_aGoblinSkills[i]->GetSkillID() == id)
			return m_aGoblinSkills[i];
	}

	return NULL;
}

//	Get skill by id
CECSkill* CECHostPlayer::GetNormalSkill(int id, bool bSenior/* false */)
{
	CECSkill* pSkill = NULL;
	if (GNET::ElementSkill::GetType(id) == CECSkill::TYPE_PASSIVE ||
		GNET::ElementSkill::GetType(id) == CECSkill::TYPE_PRODUCE ||
		GNET::ElementSkill::GetType(id) == CECSkill::TYPE_LIVE)
		pSkill = GetPassiveSkillByID(id, bSenior);
	else
		pSkill = GetPositiveSkillByID(id, bSenior);	

	if(!pSkill)// && m_pGoblin)
	{
		// This is a goblin skill
		for(int i=0;i<m_aGoblinSkills.GetSize();i++)
		{
			if(m_aGoblinSkills[i]->GetSkillID() == id)
				return m_aGoblinSkills[i];
		}
	}
	
	if(!pSkill)  // may be target item skill
	{
		if(m_pTargetItemSkill && m_pTargetItemSkill->GetSkillID() == id)
			pSkill = m_pTargetItemSkill;
	}

	return pSkill;
}
//	Check whether host can learn specified skill
int CECHostPlayer::CheckSkillLearnCondition(int idSkill, bool bCheckBook)
{
	int iLevel = 1;
	CECSkill* pSkill = GetNormalSkill(idSkill);
	if (pSkill)
		iLevel = pSkill->GetSkillLevel() + 1;

	if (iLevel == 1 && bCheckBook)
	{
		//	Do we have the skill book ?
		int idBook = GNET::ElementSkill::GetRequiredBook(idSkill, iLevel);
		if (idBook && m_pPack->FindItem(idBook) < 0)
			return 8;
	}

	//	Build player information
	GNET::LearnRequirement Info;
	memset(&Info, 0, sizeof (Info));

	Info.level		= GetMaxLevelSofar();
	Info.sp			= m_BasicProps.iSP;
	Info.money		= m_iMoneyCnt;
	Info.profession	= m_iProfession;
	Info.rank		= m_BasicProps.iLevel2;
	Info.realm_level = GetRealmLevel();

	return GNET::ElementSkill::LearnCondition(idSkill, Info, iLevel);
}

//	Check whether host can cast specified skill
int CECHostPlayer::CheckSkillCastCondition(CECSkill* pSkill)
{
	int idItem = pSkill->GetRequiredItem();
	if (idItem > 0 &&
		GetPack()->GetItemTotalNum(idItem) <= 0)
	{
		return 20;
	}
	// 刺客的瞬移技能只能在下面几个场景中使用，需单独处理
//	if(pSkill->GetSkillID() == 1095 || pSkill->GetSkillID() == 1145)
//	{
//		const int aAvailMaps[] = {1,119,121,122,230,231,232,233,234,235};
//		
//		int iCurrMap = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
//		bool bCanUse = false;
//		for(int i=0; i< sizeof(aAvailMaps)/sizeof(int); i++)
//		{
//			if(iCurrMap == aAvailMaps[i])
//			{
//				bCanUse = true;
//				break;
//			}
//		}
//
//		if(!bCanUse)
//			return 11;
// 	}

	// 单独检查夜影的连续技能
	if (pSkill->GetComboSkPreSkill() != 0) {
		if (!CECComboSkillState::Instance().IsActiveComboSkill(pSkill->GetSkillID())) {
			return 13;
		}
	}
 
	GNET::UseRequirement Info;
	memset(&Info, 0, sizeof (Info));

	Info.mp				= m_BasicProps.iCurMP;
	Info.ap				= m_BasicProps.iCurAP;
	Info.form			= m_iShape; // different from PW, no need to mask
	Info.freepackage	= m_pPack->GetEmptySlotNum();
	Info.move_env		= m_iMoveEnv;
	Info.is_combat		= IsFighting();
	Info.hp				=	m_BasicProps.iCurHP;
	Info.max_hp		=	m_ExtProps.bs.max_hp;
	Info.combo_state	= CECComboSkillState::Instance().GetComboSkillState();

	//	Get weapon's major class ID
	int iReason = 0;
	CECIvtrWeapon* pWeapon = (CECIvtrWeapon*)m_pEquipPack->GetItem(EQUIPIVTR_WEAPON);
	if (!pWeapon || !pWeapon->GetCurEndurance())
		Info.weapon	= 0;
	else if (!CanUseEquipment(pWeapon, &iReason))
		Info.weapon = (iReason == 5) ? pWeapon->GetDBMajorType()->id : 0;
	else
		Info.weapon = pWeapon->GetDBMajorType()->id;

	//	Get remain arrow number
	CECIvtrArrow* pArrow = (CECIvtrArrow*)m_pEquipPack->GetItem(EQUIPIVTR_PROJECTILE);
	if (pArrow && CanUseProjectile(pArrow))
		Info.arrow = pArrow->GetCount();

	return GNET::ElementSkill::Condition(pSkill->GetSkillID(), Info, pSkill->GetSkillLevel());
}

//	Check whether pet learn specified skill
int CECHostPlayer::CheckPetSkillLearnCondition(int idSkill, bool bCheckBook)
{
	CECPetData* pPet = m_pPetCorral->GetActivePet();
	if (!pPet)
		return -1;

	int iLevel = 1;
	const CECPetData::PETSKILL* pSkill = pPet->GetSkillByID(idSkill);
	if (pSkill)
		iLevel = pSkill->iLevel + 1;

	if (/*iLevel == 1 && */bCheckBook)
	{
		//	Do we have the skill book ?
		int idBook = GNET::ElementSkill::GetRequiredBook(idSkill, iLevel);
		if (idBook && m_pPack->FindItem(idBook) < 0)
			return 8;
	}

	//	Build player information
	GNET::PetRequirement Info;
	memset(&Info, 0, sizeof (Info));

	Info.level	= pPet->GetLevel();
	Info.sp		= m_BasicProps.iSP;

	//	Fill skill list
	int i, iCnt=0, aSkills[GP_PET_SKILL_NUM * 2];

	for (i=0; i < GP_PET_SKILL_NUM; i++)
	{
		const CECPetData::PETSKILL* pSkill = pPet->GetSkill(CECPetData::EM_SKILL_DEFAULT,i);
		if (pSkill->idSkill)
		{
			aSkills[iCnt * 2 + 0] = pSkill->idSkill;
			aSkills[iCnt * 2 + 1] = pSkill->iLevel;
			iCnt++;
		}
	}


	Info.list	= aSkills;
	Info.lsize	= iCnt * 2;

	return GNET::ElementSkill::PetLearn(idSkill, Info, iLevel);
}

bool CECHostPlayer::ProcessSkillCondition(int iCon)
{
	int iMsg = -1;
	switch (iCon)
	{
		case 1:		iMsg = FIXMSG_INVALIDWEAPON;		break;
		case 2:		iMsg = FIXMSG_NEEDMP;				break;
		case 6:		iMsg = FIXMSG_TARGETWRONG;			break;
		case 3:		iMsg = FIXMSG_SKILL_INVALIDSTATE;	break;
		case 7:		iMsg = FIXMSG_SKILL_INVALIDSTATE;	break;
		case 8:		iMsg = FIXMSG_NEEDAP;				break;
		case 9:		iMsg = FIXMSG_NOTENOUGHAMMO;		break;
		case 10:	iMsg = FIXMSG_PACKFULL1;			break;
		case 11:	iMsg = FIXMSG_SKILL_INVALIDENV;		break;
		case 20:	iMsg = FIXMSG_NEEDITEM;				break;
		case 12:	iMsg = FIXMSG_HP_UNSATISFIED;		break;
	}

	if (iMsg >= 0)
		g_pGame->GetGameRun()->AddFixedChannelMsg(iMsg, GP_CHAT_FIGHT);

	return iMsg >= 0;
}

//	Get skill ability
int CECHostPlayer::GetSkillAbility(int idSkill)
{
	return GNET::ElementSkill::GetAbility(idSkill);
}

//	Get skill ability percent
int CECHostPlayer::GetSkillAbilityPercent(int idSkill)
{
	return GNET::ElementSkill::GetAbilityPercent(idSkill);
}

void CECHostPlayer::RemoveSkillReference(int idSkill)
{
	if (idSkill <= 0)	return;

	//	删除对技能的引用
	if (m_pPrepSkill && m_pPrepSkill->GetSkillID() == idSkill)
		m_pPrepSkill = NULL;

	if (m_pCurSkill && m_pCurSkill->GetSkillID() == idSkill)
		m_pCurSkill = NULL;

	if (m_pComboSkill && m_pComboSkill->FindSkillID(idSkill))
		ClearComboSkill();

	if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_TRACEOBJECT))
	{
		CECHPWorkTrace* pWorkTrace = dynamic_cast<CECHPWorkTrace*>(pWork);
		if (pWorkTrace->GetTraceReason() == CECHPWorkTrace::TRACE_SPELL &&
			pWorkTrace->GetPrepSkill() &&
			pWorkTrace->GetPrepSkill()->GetSkillID() == idSkill)
			pWorkTrace->Reset();
	}

	int i(0);

	for (i=0; i < NUM_HOSTSCSETS1; i++)
	{
		if (m_aSCSets1[i])
			m_aSCSets1[i]->RemoveSkillShortcut(idSkill);
	}
	for (i=0; i < NUM_HOSTSCSETS2; i++)
	{
		if (m_aSCSets2[i])
			m_aSCSets2[i]->RemoveSkillShortcut(idSkill);
	}
}

//	Remove skill
void CECHostPlayer::RemoveNormalSkill(int idSkill)
{
	//	删除非装备技能

	if (idSkill <= 0)	return;

	RemoveSkillReference(idSkill);

	//	删除技能本体指针
	int i(0);

	for (i=0; i < m_aPtSkills.GetSize(); i++)
	{
		if (m_aPtSkills[i]->GetSkillID() == idSkill)
		{
			delete m_aPtSkills[i];
			m_aPtSkills.RemoveAt(i);
			return;
		}
	}

	for (i=0; i < m_aPsSkills.GetSize(); i++)
	{
		if (m_aPsSkills[i]->GetSkillID() == idSkill)
		{
			delete m_aPsSkills[i];
			m_aPsSkills.RemoveAt(i);
			return;
		}
	}

	for (i=0; i < m_aGoblinSkills.GetSize(); i++)
	{
		if (m_aGoblinSkills[i]->GetSkillID() == idSkill)
		{
			delete m_aGoblinSkills[i];
			m_aGoblinSkills.RemoveAt(i);
			return;
		}
	}
}

void CECHostPlayer::RemoveEquipSkill(int idSkill)
{
	//	删除装备技能

	if (idSkill <= 0)	return;

	RemoveSkillReference(idSkill);

	for (int i=0; i < m_aEquipSkills.GetSize(); i++)
	{
		if (m_aEquipSkills[i]->GetSkillID() == idSkill)
		{
			delete m_aEquipSkills[i];
			m_aEquipSkills.RemoveAt(i);
			return;
		}
	}
}

//	Cast skill
bool CECHostPlayer::CastSkill(int idTarget, bool bForceAttack, CECObject* pTarget/* NULL */)
{
	if (!m_pPrepSkill || !m_pPrepSkill->ReadyToCast() || IsSpellingMagic())
	{
		if(m_pPrepSkill && m_pPrepSkill->ChangeToMelee()) // Do other things like normal attack
		{
			bool bFlag = m_pPrepSkill->ReadyToCast();
			if (m_pWorkMan->IsTracing())
				m_pWorkMan->FinishRunningWork(CECHPWork::WORK_TRACEOBJECT);

			if (m_pComboSkill)
				m_pComboSkill->Continue(false);
			else
			{
//				if(idTarget != m_PlayerInfo.cid && idTarget != 0)
					NormalAttackObject(idTarget, true);
			}
		}

		m_pPrepSkill = NULL;
		return false;
	}

	//	Check target when this is a negative skill.
//	if (ISPLAYERID(idTarget) && idTarget != m_PlayerInfo.cid &&
	if (m_pPrepSkill->GetType() == CECSkill::TYPE_ATTACK ||
		m_pPrepSkill->GetType() == CECSkill::TYPE_CURSE)
	{
		if (idTarget && AttackableJudge(idTarget, bForceAttack) != 1)
		{
			m_pPrepSkill = NULL;
			return false;
		}
	}

	//	Check cast condition
	int iRet = CheckSkillCastCondition(m_pPrepSkill);
	if (iRet)
	{
		switch (iRet)
		{
		case 2:		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_NEEDMP);		break;
		case 8:		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_NEEDAP);		break;
		case 10:	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_PACKFULL1);	break;
		case 20:	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_NEEDITEM);	break;
		case 12:	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_HP_UNSATISFIED);		break;
		}

		m_pPrepSkill = NULL;
		return false;
	}

	BYTE byPVPMask = glb_BuildPVPMask(bForceAttack);	

	if (m_pPrepSkill->IsInstant())
	{
		g_pGame->GetGameSession()->c2s_CmdCastInstantSkill(m_pPrepSkill->GetSkillID(), byPVPMask, 1, &idTarget);
		m_pPrepSkill = NULL;
	}
	else if (m_pPrepSkill->GetType() == CECSkill::TYPE_FLASHMOVE)
	{
		if(m_pPrepSkill->GetRangeType() == CECSkill::RANGE_SLEF ||
		   m_pPrepSkill->GetRangeType() == CECSkill::RANGE_SELFSPHERE)
		{
			A3DVECTOR3 vDir = GetDir();
			float fDist = m_pPrepSkill->GetCastRange(m_ExtProps.ak.attack_range, GetPrayDistancePlus());

			if( m_pPrepSkill->GetSkillID() == 1844 )		// 左弦之翼，左跳
			{
				vDir = a3d_RotatePosAroundY(-vDir, A3D_PI / 2);
			}
			else if( m_pPrepSkill->GetSkillID() == 1845 )	// 右弦之翼，右跳
			{
				vDir = a3d_RotatePosAroundY(vDir, A3D_PI / 2);
			}
			else if (fDist < 0.0f)							//	范围小于0则后跳
			{
				vDir = -vDir;
			}

			fDist	= (float)fabs(fDist);
			A3DVECTOR3 vDest = m_MoveCtrl.FlashMove(vDir, 100.0f, fDist);
			g_pGame->GetGameSession()->c2s_CmdCastPosSkill(m_pPrepSkill->GetSkillID(), vDest, byPVPMask, 0, NULL);
			m_pPrepSkill = NULL;
		}
		else
		{
			//	刺客如影随行类技能
			bool bSuccess(false);
			while (true)
			{
				if (idTarget == 0 || idTarget == GetCharacterID())
					break;

				CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(idTarget, 0);
				if (!pObject) break;

				A3DVECTOR3 vHostPos = GetPos();
				A3DVECTOR3 vTargetPos = pObject->GetPos();

				//	判断技能释放距离限制是否满足
				float fTouchRadius(0.0f);
				if (ISNPCID(idTarget))
				{
					CECNPC *pNPC = dynamic_cast<CECNPC*>(pObject);
					fTouchRadius = pNPC->GetTouchRadius();
				}
				else if (ISPLAYERID(idTarget))
				{
					CECElsePlayer* pElsePlayer = dynamic_cast<CECElsePlayer*>(pObject);
					fTouchRadius = pElsePlayer->GetTouchRadius();
				}
				else	break;
				if (!CanTouchTarget(vTargetPos, fTouchRadius, 2))
				{
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_TARGETISFAR);
					break;
				}

				A3DVECTOR3 vMoveDir = vTargetPos - vHostPos;
				float fDist = vMoveDir.Magnitude();

				//	距离目标太近，不处理
				float fNearDist(0.0f);
				if (IsTooNear(vTargetPos, fNearDist))
				{
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_TARGETISNEAR);
					break;
				}

				//	计算要移往的目标位置（默认值）
				vMoveDir.Normalize();
				A3DVECTOR3 vMovePos = vHostPos + vMoveDir*(fDist-fNearDist);   //	不能跟目标位置重合，避免后续技能连续释放时遇到问题
				float fClampedHeight = ClampAboveGround(vMovePos);
				if (fabs(fClampedHeight-vMovePos.y) >= 5.0f)
				{
					//	调整过多，不可接受
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_WOULD_STUCK_OR_SO);
					break;
				}
				vMovePos.y = fClampedHeight;
				bool bPosVerified(false);

				//	目标为带凸包的 NPC 时，单独处理，以增强技能的适用性
				if (ISNPCID(idTarget))
				{
					CECNPC *pNPC = dynamic_cast<CECNPC*>(pObject);
					A3DAABB aabbNPC;
					if (pNPC->GetCHAABB(aabbNPC))
					{
						A3DVECTOR3 vTestPos;
						if (CalcCollideFreePos(aabbNPC, vTestPos))
						{
							//	NPC 附近找到无冲突的位置
							vMovePos = vTestPos;
							bPosVerified = true;
						}
						else
						{
							//	卡人，或离得很近时计算导致
							g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_WOULD_STUCK_OR_SO);
							break;
						}
					}
				}

				if (!bPosVerified && !IsPosCollideFree(vMovePos))
				{
					//	跟凸包有碰撞，尝试在竖直方向上查找其它位置
					A3DVECTOR3 vTestPos2;
					if (!CalcVerticalCollideFreePos(vMovePos, vTestPos2))
					{
						//	卡人
						g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_WOULD_STUCK_OR_SO);
						break;
					}
					vMovePos = vTestPos2;
					bPosVerified = true;
				}

				if (IsTooNear(vMovePos))
				{
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_TARGETISNEAR);
					break;
				}

				//	发送协议
				g_pGame->GetGameSession()->c2s_CmdCastPosSkill(m_pPrepSkill->GetSkillID(), vMovePos, byPVPMask, 1, &idTarget);
				bSuccess = true;
				break;
			}
			m_pPrepSkill = NULL;
			return bSuccess;
		}
	}
	else
	{
 		BYTE byPVPMask = glb_BuildPVPMask(bForceAttack);
		g_pGame->GetGameSession()->c2s_CmdCastSkill(m_pPrepSkill->GetSkillID(), byPVPMask, 1, &idTarget);
	}

	return true;
}

//	Save configs data (shortcut, etc.)
bool CECHostPlayer::SaveConfigData(void* pDataBuf, int* piSize)
{
	int i, iSize, iTotalSize=0;
	BYTE* pData = (BYTE*)pDataBuf;

	//	Version
	iTotalSize += sizeof (DWORD);
	if (pDataBuf)
	{
		*((DWORD*)pData) = HOSTCFG_VERSION;
		pData += sizeof (DWORD);
	}

	//	Save shortcut configs...
	for (i=0; i < NUM_HOSTSCSETS1; i++)
	{
		m_aSCSets1[i]->SaveConfigData(pData, &iSize);
		iTotalSize += iSize;
		if (pData)
			pData += iSize;
	}

	for (i=0; i < NUM_HOSTSCSETS2; i++)
	{
		m_aSCSets2[i]->SaveConfigData(pData, &iSize);
		iTotalSize += iSize;
		if (pData)
			pData += iSize;
	}

	//  Save auto fashion shortcut sets 	
	if(pDataBuf)
	{
		for (i=0; i < SIZE_FASHIONSCSET; i++)
		{
			*((int*)pData) = m_aFashionSCSets[i];
	
			if(pData)
				pData += sizeof (int);
		}
	}	

	iTotalSize += sizeof(int) * SIZE_FASHIONSCSET;

	//  Save system module shortcut sets 	
	for (i=0; i < NUM_SYSMODSETS; i++)
	{
		m_aSCSetSysMod[i]->SaveConfigData(pData, &iSize);
		iTotalSize += iSize;
		if (pData)
			pData += iSize;
	}

	// Save Booth Buy and Sell Pack
	struct BoothItem
	{
		DWORD idItem;
		DWORD iOrigin;
		DWORD iAmount;
		DWORD iUnitPrice;
	};
	CECDealInventory *pBoothPacks[2] = {GetBoothBuyPack(), GetBoothSellPack()};
	for (int k=0 ; k < sizeof(pBoothPacks)/sizeof(pBoothPacks[0]); ++k)
	{
		CECDealInventory *pBoothPack = pBoothPacks[k];
		bool isBuyPack = (pBoothPack == GetBoothBuyPack());
		DWORD dwSize = pBoothPack->GetSize();
		if (pData)
		{
			*((DWORD *)pData) = dwSize;
			pData += sizeof(DWORD);
			
			BoothItem temp;
			for (DWORD i = 0; i < dwSize; ++ i)
			{
				::ZeroMemory(&temp, sizeof(temp));
				const CECDealInventory::ITEMINFO &ii = pBoothPack->GetItemInfo(i);			
				CECIvtrItem* pItem1 = pBoothPack->GetItem(i);
				if (pItem1)
				{
					CECIvtrItem* pItem2 = m_pPack->GetItem(ii.iOrigin);				
					if (pItem2 && pItem2->GetTemplateID() == pItem1->GetTemplateID())
					{
						temp.idItem = pItem1->GetTemplateID();
						temp.iOrigin = ii.iOrigin;
						temp.iUnitPrice = pItem1->GetUnitPrice();
						temp.iAmount	= isBuyPack ? ii.iAmount : a_Min(ii.iAmount, pItem2->GetCount());
					}
				}
				
				*((BoothItem *)pData) = temp;
				pData += sizeof(temp);
			}
		}
		iTotalSize += sizeof(dwSize) + dwSize*sizeof(BoothItem);
	}

	//  银票自动转换
	if(pDataBuf)
	{
		*(int*)pData = m_AutoYinpiao.low_money;
		pData += sizeof(int);
		*(int*)pData = m_AutoYinpiao.high_money;
		pData += sizeof(int);
	}

	iTotalSize += sizeof(int) * 2;

	if (piSize)
		*piSize = iTotalSize;

	return true;
}

//	Load configs data (shortcut, etc.) from specified buffer
bool CECHostPlayer::LoadConfigData(const void* pDataBuf)
{
	int i;
	BYTE* pData = (BYTE*)pDataBuf;

	//	Version number
	DWORD dwVer = *(DWORD*)pData;
	pData += sizeof (DWORD);
	if (dwVer > HOSTCFG_VERSION)
	{
		ASSERT(dwVer > HOSTCFG_VERSION);
		return false;
	}

	//	Load shortcut configs...
	int iHostSCSets1 = NUM_HOSTSCSETS1;
	if(dwVer <= 4)
		iHostSCSets1 = 3;

	for (i=0; i < iHostSCSets1; i++)
	{
		int iSize;
		if (!m_aSCSets1[i]->LoadConfigData(pData, &iSize, dwVer))
			return false;

		pData += iSize;
	}

	for (i=0; i < NUM_HOSTSCSETS2; i++)
	{
		int iSize;
		if (!m_aSCSets2[i]->LoadConfigData(pData, &iSize, dwVer))
			return false;

		pData += iSize;
	}

	//  Load auto fashion shortcut sets
	if(dwVer > 5)
	{
		int size(0);
		if (dwVer == 6)
		{
			size = 120;
		}
		else
		{
			size = SIZE_FASHIONSCSET;
		}
		for (i=0; i < size; i++)
		{
			if (dwVer <= 8 && (i % 6 == 5)) continue;
			m_aFashionSCSets[i] = *((int*)pData);
			pData += sizeof (int);		
		}
	}

	//  Load system module shortcut sets
	if(dwVer > 10)
	{
		for (i=0; i < NUM_SYSMODSETS; i++)
		{
			int iSize;
			if (!m_aSCSetSysMod[i]->LoadConfigData(pData, &iSize, dwVer))
				return false;

			if (i==0) // 如果第一栏4个位置都为空，则初始默认快捷键。
			{
				CECShortcutSet *pSCS = m_aSCSetSysMod[0];
				int ksc = 0;
				for (ksc = 0;ksc<pSCS->GetShortcutNum();ksc++)
				{
					if (pSCS->GetShortcut(ksc))
						break;
				}
				if (ksc == pSCS->GetShortcutNum())
				{
					int c = CECUIConfig::Instance().GetGameUI().nDefaultSystemModuleIndex.size();
					c = min(c,4);
					for (int i=0;i<c;i++)
						pSCS->CreateSystemModuleShortcut(i, CECUIConfig::Instance().GetGameUI().nDefaultSystemModuleIndex[i]);
				}				
			}
			
			pData += iSize;
		}
	}
	
	// Load Saved Booth Buy and Sell Pack
	if (dwVer > 7)
	{
		struct BoothItem
		{
			DWORD idItem;
			DWORD iOrigin;
			DWORD iAmount;
			DWORD iUnitPrice;
		};
		BoothItem temp;
		const int nPacks = 2;
		CECDealInventory *pBoothPacks[nPacks] = {GetBoothBuyPack(), GetBoothSellPack()};
		DWORD nMaxBoothPacks[nPacks] = {IVTRSIZE_BOOTHBPACK_MAX, IVTRSIZE_BOOTHSPACK_MAX};
		for (int k=0 ; k < nPacks; ++k)
		{
			CECDealInventory *pBoothPack = pBoothPacks[k];
			bool isBuyPack = (pBoothPack == GetBoothBuyPack());
			DWORD dwCurSize = pBoothPack->GetSize();

			DWORD dwSize = *((DWORD *)pData);
			pData += sizeof(dwSize);

			// 验证数据（有出错案例）
			if (dwSize > nMaxBoothPacks[k])
			{
				a_LogOutput(1, "CECHostPlayer::LoadConfigData, Failed because pack %d size invalid (%d:%d).", k, dwSize, nMaxBoothPacks[k]);
				return false;
			}

			for (DWORD i = 0; i < dwSize; ++ i)
			{
				// 保存的包裹大小很可能与当前包裹大小不同，以当前包裹大小为准
				if (i<dwCurSize)
				{
					temp = *((BoothItem *)pData);

					// 验证读取数据的合法性
					if (temp.idItem>0 &&
						temp.iAmount>0 &&
						temp.iUnitPrice>=0 &&
						temp.iOrigin>=0)
					{
						CECIvtrItem* pItem2 = m_pPack->GetItem(temp.iOrigin);
						if (pItem2 && pItem2->GetTemplateID()==(int)temp.idItem)
						{
							// 物品在当前包裹查到、并且确认是原物品同类物品

							int iAmount = isBuyPack ? temp.iAmount : a_Min((int)temp.iAmount, pItem2->GetCount());
							pBoothPack->AddBoothItem(pItem2, (int)temp.iOrigin,	iAmount, (int)temp.iUnitPrice);
						}
					}
				}
				pData += sizeof(temp);
			}
		}
	}

	if( dwVer > 9 )
	{
		m_AutoYinpiao.low_money = *(int*)pData;
		pData += sizeof(int);
		m_AutoYinpiao.high_money = *(int*)pData;
		pData += sizeof(int);
	}

	return true;
}

//	Calculate distance between host and specified object
bool CECHostPlayer::CalcDist(int idObject, float* pfDist, CECObject** ppObject/* NULL */)
{
	if (!idObject || idObject == m_PlayerInfo.cid)
		return false;

	CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(idObject, 1);
	if (!pObject)
		return false;

	if (ppObject)
		*ppObject = pObject;

	float fDist = 0.0f;

	if (ISNPCID(idObject))
	{
		CECNPC* pNPC = (CECNPC*)pObject;
		fDist = pNPC->CalcDist(GetPos(), true);
	}
	else if (ISPLAYERID(idObject))
	{
		ASSERT(pObject->GetClassID() == CECObject::OCID_ELSEPLAYER);
		CECElsePlayer* pPlayer = (CECElsePlayer*)pObject;
		fDist = pPlayer->CalcDist(GetPos(), true);
	}
	else if (ISMATTERID(idObject))
	{
		ASSERT(pObject->GetClassID() == CECObject::OCID_MATTER);
		CECMatter* pMatter = (CECMatter*)pObject;
		fDist = a3d_Magnitude(pMatter->GetPos() - GetPos());
	}
	else
		return false;

	*pfDist = fDist;
	return true;
}

//	When all resources are ready, this function will be called
void CECHostPlayer::OnAllResourceReady()
{
	CECPlayer::OnAllResourceReady();	
	CECShoppingManager::Instance().Initialize();
	CECFashionShopManager::Instance().Initialize(CECFashionShop::ST_NEW_ARRIVE, CECFashionShop::FT_SUITE, GetProfession(), GetGender());
	CECComboSkillState::Instance().Initialize();
	CECHostSkillModel::Instance().Initialize();
	CECPlayerLevelRankRealmChangeCheck::Instance().Initialize();
}

bool CECHostPlayer::SetPetLoadResult(CECModel* pPetModel)
{
	if (CECPlayer::SetPetLoadResult(pPetModel))
	{
		m_CDRInfo.vExtent = m_aabbServer.Extents;
		//m_CDRInfo.fStepHeight *= 2.0f;
		return true;
	}

	return false;
}

//	When all initial data is ready, this function is called
void CECHostPlayer::OnAllInitDataReady()
{
	if (IsDead())
	{
		//	Popup revive notify dialog
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->PopupReviveDialog(true);
	}

	m_bEnterGame = true;
	
	//	原来这是一种尝试，看看有没有促进销售的帮助，
	//	但是我查了一下过往的数据，经过分析，帮助很小，现在销量呈下滑趋势，
	//	并且玩家对这个设定及其反感，多次要求关闭，
	//	同时现在要开帮派pvp了，这个对pvp影响很大，所以综上所述，需要关闭这个功能
// 	if (!m_pSaveLifeTrigger){
// 		CECQuickBuyPopPolicies policy = QuickBuyPopPolicy::CreateSaveLifePolicy(m_pSaveLifeTrigger);
// 		CECQuickBuyPopManager::Instance().RegisterPolicy(policy);
// 	}
}

//	Attack an object
bool CECHostPlayer::NormalAttackObject(int idTarget, bool bForceAttack, bool bMoreClose/* false */)
{
	if (!idTarget || idTarget == m_PlayerInfo.cid)
	{
		//	We should have check target isn't dead
		ASSERT(idTarget && idTarget != m_PlayerInfo.cid);
		return false;
	}

	if (!g_pGame->GetGameRun()->GetWorld()->GetObject(idTarget, 1))
		return false;
	bool bStartNewWork = false;

	bool bUseAutoPF = false;
	CECPlayerWrapper* pWrapper = CECAutoPolicy::GetInstance().GetPlayerWrapper();
	if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() && pWrapper->GetAttackError() >= 2 )
 		bUseAutoPF = true;	
	
	//	Trace a object
	CECHPWorkTrace* pWorkTrace = NULL;
	CECHPWork* pWork = NULL;
	if (pWork = m_pWorkMan->GetWork(CECHPWork::WORK_TRACEOBJECT))
	{
		pWorkTrace = dynamic_cast<CECHPWorkTrace*>(pWork);
	}
	else if (pWork = m_pWorkMan->GetWork(CECHPWork::WORK_HACKOBJECT))
	{
		if (dynamic_cast<CECHPWorkMelee*>(pWork)->GetTarget() == idTarget)
			return false;	//	Host is attacking the target

		pWorkTrace = (CECHPWorkTrace*)m_pWorkMan->CreateWork(CECHPWork::WORK_TRACEOBJECT);
		bStartNewWork = true;
	}
	else if (m_pWorkMan->CanStartWork(CECHPWork::WORK_TRACEOBJECT))
	{
		pWorkTrace = (CECHPWorkTrace*)m_pWorkMan->CreateWork(CECHPWork::WORK_TRACEOBJECT);
		bStartNewWork = true;
	}

	if (pWorkTrace)
	{
		pWorkTrace->SetTraceTarget(pWorkTrace->CreatTraceTarget(idTarget, CECHPWorkTrace::TRACE_ATTACK, bForceAttack),bUseAutoPF);
		pWorkTrace->SetMoveCloseFlag(bMoreClose);

		if (bStartNewWork)
			m_pWorkMan->StartWork_p1(pWorkTrace);
		return true;
	}

	return false;
}

//	Pickup an dobject
bool CECHostPlayer::PickupObject(int idTarget, bool bGather)
{
	if (IsDead() || IsSpellingMagic() || !idTarget || !ISMATTERID(idTarget))
		return false;

	//	Check matter type
	CECMatter* pMatter = g_pGame->GetGameRun()->GetWorld()->GetMatterMan()->GetMatter(idTarget);
	if (!pMatter || bGather != pMatter->IsMine())
		return false;

	if (bGather && !CanGatherMatter(pMatter))
		return false;

	bool bUseAutoPF = false;
	CECPlayerWrapper* pWrapper = CECAutoPolicy::GetInstance().GetPlayerWrapper();
	if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() && pWrapper->GetPickupError() >= 2 )
 		bUseAutoPF = true;

	bool bOK = false;

	//	Trace a object
	if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_TRACEOBJECT))
	{
		CECHPWorkTrace* pWorkTrace = dynamic_cast<CECHPWorkTrace*>(pWork);
		pWorkTrace->SetTraceTarget(pWorkTrace->CreatTraceTarget(idTarget, bGather ? CECHPWorkTrace::TRACE_GATHER : CECHPWorkTrace::TRACE_PICKUP),bUseAutoPF);
		bOK = true;
	}
	else if (m_pWorkMan->CanStartWork(CECHPWork::WORK_TRACEOBJECT))
	{
		CECHPWorkTrace* pWork = (CECHPWorkTrace*)m_pWorkMan->CreateWork(CECHPWork::WORK_TRACEOBJECT);
		pWork->SetTraceTarget(pWork->CreatTraceTarget(idTarget, bGather ? CECHPWorkTrace::TRACE_GATHER : CECHPWorkTrace::TRACE_PICKUP),bUseAutoPF);
		m_pWorkMan->StartWork_p1(pWork);
		bOK = true;
	}

	return bOK;
}

//	Check whether host can gather specified matter
bool CECHostPlayer::CanGatherMatter(CECMatter* pMatter)
{
	if (!pMatter || !pMatter->IsMine())
		return false;

	DATA_TYPE DataType;
	const void* pDataPtr = g_pGame->GetElementDataMan()->get_data_ptr(pMatter->GetTemplateID(), ID_SPACE_ESSENCE, DataType);

	if (DataType != DT_MINE_ESSENCE)
	{
		ASSERT(DataType == DT_MINE_ESSENCE);
		return false;
	}
	// 顶级战车不能采矿, 能量满不能采
	if (m_BattleInfo.IsChariotWar() && m_pChariot && !m_pChariot->CanGatherMine())
		return false;
	
	const MINE_ESSENCE* pData = (const MINE_ESSENCE*)pDataPtr;
	if (!pData->task_in || m_pTaskInterface->HasTask(pData->task_in))
		return true;

	return false;
}

/*	Attackable judge. Check whether we can attack an object.

	Return value:

		-1: target has missed, or target id is invalid, or target is dead
		0: target isn't attackable
		1: target is attackable
*/
int CECHostPlayer::AttackableJudge(int idTarget, bool bForceAttack)
{
	if (CannotAttack())
		return 0;

	if(CDlgAutoHelp::IsAutoHelp())
		return 0;

	if (!idTarget || idTarget == m_PlayerInfo.cid)
		return -1;

	CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(idTarget, 1);
	if (!pObject)
		return -1;

	//	If target is pet, it's attacked possibility depends on it's monster
	if (ISNPCID(idTarget))
	{
		CECNPC* pNPC = (CECNPC*)pObject;
		int idMaster = pNPC->GetMasterID();
		if (idMaster)
		{
			// master可能是hostplayer
			if (idMaster == m_PlayerInfo.cid)
				return 0;

			//	Follow pet cannot be attacked
			if (pNPC->IsPetNPC() && ((CECPet*)pNPC)->IsFollowPet())
				return 0;

			idTarget = idMaster;
			pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(idTarget, 1);
			if (!pObject)
				return -1;
		}
	}

	int iRet = 0;

	if (ISNPCID(idTarget))
	{
		CECNPC* pNPC = (CECNPC*)pObject;

		//	If this npc is host's pet, cannot be attacked
		if (pNPC->GetMasterID() == m_PlayerInfo.cid)
			return 0;

		//	If it's a pet and can not be attacked, pet can be attacked only if it's a fighting pet
		if (pNPC->IsPetNPC() && !((CECPet *)pNPC)->CanBeAttacked())
			return 0;

		if (IsInBattle())	//	Host is in battle
		{
			if (InSameBattleCamp(pNPC))
				iRet = 0;
			else
			{
				if (pNPC->IsMonsterNPC())
					iRet = 1;
				else if (pNPC->IsServerNPC() && (IsInFortress() || pNPC->GetRoleInBattle() == 8))	//	对服务型NPC的攻击，帮派基地或城战时可用
					iRet = 1;
				else
					iRet = 0;
			}
		}
		else if (pNPC->IsServerNPC())
		{
			//	In sanctuary we cannot attack NPCs
			if (!IsPVPOpen() || m_bInSanctuary || !bForceAttack)
				iRet = 0;
			else
				iRet = 1;
		}
		else	//	Is monster
		{
			iRet = 1;
		}
		if (iRet == 1 && pNPC->GetOwnerFaction() > 0){
			//	针对帮派 PVP 战争中禁止部分攻击
			if (GetFactionID() == pNPC->GetOwnerFaction() ||						//	不攻击同帮怪
				pNPC->IsFactionPVPMineCar() && !CanAttackFactionPVPMineCar() ||		//	无法再攻击他帮矿车情况
				pNPC->IsFactionPVPMineBase() && !CanAttackFactionPVPMineBase()){	//	无法再攻击他帮存矿点情况
				iRet = 0;
			}
		}
	}
	else if (ISPLAYERID(idTarget))
	{
		//	Check duel at first
		if (m_pvp.iDuelState == DUEL_ST_INDUEL && m_pvp.idDuelOpp == idTarget)
			return 1;
		else if (m_pvp.iDuelState == DUEL_ST_STOPPING && m_pvp.idDuelOpp == idTarget)
			return 0;

		//	In sanctuary we cannot attack other players
		if (m_bInSanctuary)
			return 0;

		ASSERT(pObject->GetClassID() == CECObject::OCID_ELSEPLAYER);
		CECElsePlayer* pPlayer = (CECElsePlayer*)pObject;
		const ROLEBASICPROP& bp = pPlayer->GetBasicProps();
		const EC_GAME_SETTING& gs = g_pGame->GetConfigs()->GetGameSettings();

		if (m_pvp.bFreePVP)
		{
			if (IsTeamMember(idTarget))
				return 0;

			//	In free pvp mode, for example, host is in arena.
			if (bForceAttack)
				iRet = 1;
			else if (gs.bAtk_NoMafia && IsFactionMember(pPlayer->GetFactionID()))
				iRet = 0;
			else if (gs.bAtk_NoWhite && !pPlayer->IsInvader() && !pPlayer->IsPariah())
				iRet = 0;
			else if (gs.bAtk_NoAlliance && g_pGame->GetFactionMan()->IsFactionAlliance(pPlayer->GetFactionID()))
				iRet = 0;
			else if(gs.bAtk_NoForce && GetForce() > 0 && GetForce() == pPlayer->GetForce())
				iRet = 0;
			else
				iRet = 1;
		}
		else if (m_iBattleCamp != GP_BATTLE_CAMP_NONE)
		{
			//	Host is in battle
			int iCamp = pPlayer->GetBattleCamp();
			if (iCamp != GP_BATTLE_CAMP_NONE && iCamp != m_iBattleCamp)
				iRet = 1;
			else
				iRet = 0;
		}
		else	//	Normal mode
		{
			if (IsTeamMember(idTarget))
				return 0;

			if (!IsPVPOpen() || !pPlayer->IsPVPOpen() || m_BasicProps.iLevel < EC_MAXNOPKLEVEL || bp.iLevel < EC_MAXNOPKLEVEL)
				iRet = 0;
			else if (bForceAttack)
				iRet = 1;
			else if (!gs.bAtk_Player)
				iRet = 0;
			else if (gs.bAtk_NoMafia && IsFactionMember(pPlayer->GetFactionID()))
				iRet = 0;
			else if (gs.bAtk_NoWhite && !pPlayer->IsInvader() && !pPlayer->IsPariah())
				iRet = 0;
			else if (gs.bAtk_NoAlliance && g_pGame->GetFactionMan()->IsFactionAlliance(pPlayer->GetFactionID()))
				iRet = 0;
			else if(gs.bAtk_NoForce && GetForce() > 0 && GetForce() == pPlayer->GetForce())
				iRet = 0;
			else
				iRet = 1;
		}
	}
	else
	{
		return -1;
	}

	return iRet;
}

//	Decide target name color basing on target level
DWORD CECHostPlayer::DecideTargetNameCol(int iTargetLevel)
{
	int iDelta = m_BasicProps.iLevel - iTargetLevel;
	DWORD dwCol = 0xffffffff;

	if (iDelta >= 11)
		dwCol = A3DCOLORRGB(96, 128, 42);
	else if (iDelta >= 8)
		dwCol = A3DCOLORRGB(135, 179, 59);
	else if (iDelta >= 5)
		dwCol = A3DCOLORRGB(163, 217, 72);
	else if (iDelta >= 3)
		dwCol = A3DCOLORRGB(200, 255, 100);
	else if (abs(iDelta) < 3)
		dwCol = A3DCOLORRGB(255, 255, 255);
	else if (iDelta >= -5)
		dwCol = A3DCOLORRGB(255, 221, 128);
	else if (iDelta >= -8)
		dwCol = A3DCOLORRGB(255, 196, 76);
	else
		dwCol = A3DCOLORRGB(255, 140, 0);

	return dwCol;
}

//	Check whether host can do a behavior
bool CECHostPlayer::CanDo(int iThing)
{
	bool bRet = true;

	switch (iThing)
	{
	case CANDO_SITDOWN:

		if (IsDead() || IsAboutToDie() || IsJumping() || IsTrading() || IsUsingTrashBox() || 
			IsRooting() || IsReviving() || IsTalkingWithNPC() || IsChangingFace() ||
			!m_GndInfo.bOnGround ||	GetBoothState() != 0 || m_iBuddyId || IsOperatingPet() || IsRebuildingPet() ||
			IsUsingItem() || IsRidingOnPet() || GetShapeType() == PLAYERMODEL_DUMMYTYPE2 || IsPassiveMove())
			bRet = false;

		break;

	case CANDO_MOVETO:
	{
		if (IsDead() || IsSitting() || IsTrading() || IsUsingTrashBox() || IsRooting() ||
			IsReviving() || IsTalkingWithNPC() || IsChangingFace() || IsUsingItem() ||
			GetBoothState() != 0 || m_bHangerOn || IsOperatingPet() || IsRebuildingPet() || IsPassiveMove())
			bRet = false;
	
		break;
	}
	case CANDO_MELEE:

		if (IsDead() || IsSitting() || !m_idSelTarget || m_idSelTarget == m_PlayerInfo.cid ||
			IsJumping() || ISMATTERID(m_idSelTarget) || IsTrading() || IsReviving() || 
			IsUsingTrashBox() || IsTalkingWithNPC() || IsChangingFace() || CannotAttack() ||
			GetBoothState() != 0 || m_iBuddyId || IsRidingOnPet() || IsOperatingPet() || IsRebuildingPet() ||
			IsUsingItem() || IsPassiveMove())
			bRet = false;

		break;

	case CANDO_ASSISTSEL:

		if (IsDead() || !ISPLAYERID(m_idSelTarget) || m_idSelTarget == m_PlayerInfo.cid ||
			!m_pTeam || !m_pTeam->GetMemberByID(m_idSelTarget) || m_iBuddyId || IsPassiveMove() ||
			m_playerLimits.test(PLAYER_LIMIT_NOCHANGESELECT))
			bRet = false;

		break;

	case CANDO_FLY:

		if (IsDead() || IsRooting() || IsSitting() || IsTrading() || IsReviving() || 
			IsUsingTrashBox() || IsTalkingWithNPC() || IsChangingFace() || GetBoothState() != 0 ||
			IsFlashMoving() || m_pWorkMan->HasWorkRunningOnPriority(CECHPWorkMan::PRIORITY_2) ||
			m_bHangerOn || IsOperatingPet() || IsRebuildingPet() ||
			IsUsingItem() || IsRidingOnPet() || GetShapeType() == PLAYERMODEL_DUMMYTYPE2 || IsPassiveMove() ||
			m_playerLimits.test(PLAYER_LIMIT_NOFLY) || m_BattleInfo.IsChariotWar())
			bRet = false;

		break;

	case CANDO_PICKUP:
	case CANDO_GATHER:

		if (IsDead() || IsAboutToDie() || IsSitting() || IsTrading() || IsUsingTrashBox() ||
			IsReviving() || IsTalkingWithNPC() || IsChangingFace() || GetBoothState() != 0 ||
			GetBuddyState() == 1 || IsOperatingPet() || IsRebuildingPet() || IsUsingItem() || IsPassiveMove())
			bRet = false;

		break;

	case CANDO_TRADE:

		if (IsDead() || IsAboutToDie() || IsSitting() || IsJumping() || IsMeleeing() || 
			IsTrading() || IsUsingTrashBox() || IsTalkingWithNPC() || IsChangingFace() || 
			IsSpellingMagic() || GetBoothState() != 0 || m_iBuddyId || IsOperatingPet() || IsRebuildingPet() ||
			IsUsingItem() || IsInvisible() || IsPassiveMove())
			bRet = false;

		break;

	case CANDO_PLAYPOSE:
	
		if (IsDead() || IsAboutToDie() || IsSitting() || IsJumping() || IsMeleeing() ||
			IsTrading() || IsUsingTrashBox() || IsTalkingWithNPC() || IsChangingFace() || 
			IsSpellingMagic() || IsShapeChanged() || IsReviving() || m_iMoveEnv != MOVEENV_GROUND ||
			GetBoothState() != 0 || m_iBuddyId || IsOperatingPet() ||IsRebuildingPet() || IsUsingItem() ||
			IsRidingOnPet() || GetShapeType() == PLAYERMODEL_DUMMYTYPE2 || IsPassiveMove() || m_BattleInfo.IsChariotWar())
			bRet = false;

		break;
	
	case CANDO_SPELLMAGIC:
		if (IsDead() || ISMATTERID(m_idSelTarget) || IsAboutToDie() || IsSitting() ||
			IsJumping() || IsFlashMoving() || IsTrading() || IsUsingTrashBox() ||	IsTalkingWithNPC() ||
			IsChangingFace() || CannotAttack() || IsReviving() || GetBoothState() != 0 ||
			m_iBuddyId ||  IsRidingOnPet() || IsOperatingPet() || IsRebuildingPet() ||IsUsingItem() || IsPassiveMove())
			bRet = false;

		break;

	case CANDO_SUMMONPET:

		if (IsDead() || ISMATTERID(m_idSelTarget) || IsAboutToDie() || IsSitting() ||
			IsJumping() || IsFlashMoving() || IsTrading() || IsUsingTrashBox() ||	IsTalkingWithNPC() ||
			IsChangingFace() || CannotAttack() || IsReviving() || GetBoothState() != 0 ||
			IsInvisible() || IsGMInvisible() || IsOperatingPet() ||IsRebuildingPet() || IsUsingItem() || IsPassiveMove()
			|| m_BattleInfo.IsChariotWar())
			bRet = false;

		break;
	case CANDO_REBUILDPET:
		
		if (IsDead() || ISMATTERID(m_idSelTarget) || IsAboutToDie() || IsSitting() ||
			IsJumping() || IsFlashMoving() || IsTrading() || IsUsingTrashBox() ||	IsTalkingWithNPC() ||
			IsChangingFace() || CannotAttack() || IsReviving() || GetBoothState() != 0 ||
			m_iBuddyId ||  IsInvisible() || IsGMInvisible() || IsOperatingPet() ||IsRebuildingPet() || IsUsingItem() || IsPassiveMove()||
			IsPlayerMoving() || m_BattleInfo.IsChariotWar())
			bRet = false;
		
		break;

	case CANDO_USEITEM:

		if (IsAboutToDie() || IsTrading() || IsUsingTrashBox() || IsTalkingWithNPC() ||
			IsChangingFace() || GetBoothState() != 0 || IsPassiveMove() || m_BattleInfo.IsChariotWar())
			bRet = false;

		break;
		
	case CANDO_JUMP:
	{	
		if (IsDead() ||
			m_iJumpCount >= MAX_JUMP_COUNT ||
			// cannot jump more than one time if shape mode is type2
			(IsJumping() && (GetShapeType() == PLAYERMODEL_DUMMYTYPE2)) ||
			IsJumpInWater() || m_iMoveEnv == MOVEENV_AIR || IsSitting() || 
			IsMeleeing() || IsTrading() || IsUsingTrashBox() || IsTalkingWithNPC() ||
			IsChangingFace() || IsReviving() || IsSpellingMagic() || IsPicking() ||
			IsGathering() || IsRooting() ||	GetBoothState() != 0 || m_bHangerOn || (IsJumping() && IsRidingOnPet()) ||
			IsOperatingPet() || IsRebuildingPet() ||IsUsingItem() || IsPassiveMove() || m_BattleInfo.IsChariotWar())
			bRet = false;

		break;
	}
	case CANDO_FOLLOW:
	{	
		if (IsDead() || IsAboutToDie() || IsSitting() || IsMeleeing() || IsReviving() || 
			IsTrading() || IsUsingTrashBox() || IsTalkingWithNPC() || IsChangingFace() || 
			IsSpellingMagic() || GetBoothState() != 0 || m_bHangerOn || IsOperatingPet() ||IsRebuildingPet() ||
			IsUsingItem() || IsPassiveMove())
			bRet = false;

		break;
	}
	case CANDO_BOOTH:
		
		if (IsDead() || IsAboutToDie() || IsPlayerMoving() || IsSitting() || IsReviving() || 
			IsMeleeing() || IsJumping() || IsTrading() || IsUsingTrashBox() || 
			IsTalkingWithNPC() || IsChangingFace() || IsSpellingMagic() || IsFlying() ||
			IsUnderWater() || m_iBuddyId || IsOperatingPet() ||IsRebuildingPet() || IsUsingItem() || IsRidingOnPet() || IsInvisible() ||
			IsPassiveMove())
			bRet = false;

		break;

	case CANDO_FLASHMOVE:
		
		if (IsDead() || IsAboutToDie() || IsTrading() || IsUsingTrashBox() || IsTalkingWithNPC() ||
			IsJumping() || IsFlashMoving() || IsFalling() || IsChangingFace() || GetBoothState() != 0 || IsTakingOff() ||
			m_pWorkMan->HasWorkRunningOnPriority(CECHPWorkMan::PRIORITY_2) ||
			m_iBuddyId || IsOperatingPet() ||IsRebuildingPet() || IsUsingItem() || IsPassiveMove())
			bRet = false;

		break;
		
	case CANDO_BINDBUDDY:
		
		if (IsDead() || IsAboutToDie() || IsJumping() || IsSitting() || 
			IsMeleeing() || IsTrading() || IsUsingTrashBox() || IsTalkingWithNPC() ||
			IsChangingFace() || IsReviving() || IsSpellingMagic() || IsPicking() ||
			IsGathering() || IsRooting() ||	GetBoothState() != 0 ||
			!m_pWorkMan->IsStanding() || m_iBuddyId ||
			IsOperatingPet() || IsRebuildingPet() ||IsUsingItem()|| GetShapeType() == PLAYERMODEL_DUMMYTYPE2 || IsPassiveMove() ||
			m_playerLimits.test(PLAYER_LIMIT_NOBIND)) 
			bRet = false;

		break;
		
	case CANDO_DUEL:

		if (IsDead() || IsAboutToDie() ||IsSitting() || IsFighting() || IsTrading() || 
			IsReviving() || IsUsingTrashBox() || IsTalkingWithNPC() || IsChangingFace() ||
			GetBoothState() != 0 || m_iBuddyId || m_pvp.iDuelState != DUEL_ST_NONE ||
			IsOperatingPet() || IsRebuildingPet() ||IsUsingItem() || IsPassiveMove())
			bRet = false;

		break;

	case CANDO_CHANGESELECT:

		if (m_playerLimits.test(PLAYER_LIMIT_NOCHANGESELECT))
			bRet = false;

		break;

	case CANDO_SWITCH_PARALLEL_WORLD:
		if (IsDead() || IsAboutToDie() || IsJumping() || IsFighting() || 
			IsMeleeing() || IsTrading() || IsUsingTrashBox() || IsTalkingWithNPC() ||
			IsChangingFace() || IsReviving() || IsSpellingMagic() || IsPicking() ||
			IsGathering() || IsRooting() ||	GetBoothState() != 0 ||
			m_iBuddyId || IsOperatingPet() || IsRebuildingPet() ||IsUsingItem()||
			GetShapeType() == PLAYERMODEL_DUMMYTYPE2 || IsPassiveMove()) 
			bRet = false;
		break;
	}

	return bRet;
}

bool CECHostPlayer::IsPlayerMoving()
{
	return m_pWorkMan->IsMoving();
}

bool CECHostPlayer::IsWorkMoveRunning()const{
	return m_pWorkMan->IsMovingToPosition();
}

bool CECHostPlayer::IsWorkSpellRunning()const{
	return m_pWorkMan->IsSpellingMagic();
}

//	Can jump or take off in water ?
bool CECHostPlayer::CanTakeOffWater()
{
	A3DVECTOR3 vPos = GetPos();
	if (vPos.y < g_pGame->GetGameRun()->GetWorld()->GetWaterHeight(vPos) - m_MoveConst.fShoreDepth)
		return false;
	else
		return true;
}

float CECHostPlayer::GetSwimSpeedSev()
{
	float fSpeedSev = GetSwimSpeed();
	while (true)
	{
	    if (!IsUnderWater()) break;
	    CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
	    if (!pWorld) break;

	    const A3DVECTOR3 &vPos = GetPos();
	    float fTerrainHeight = pWorld->GetTerrainHeight(vPos);
	    float fWaterHeight = pWorld->GetWaterHeight(vPos);
	    if (fWaterHeight <= fTerrainHeight) break;

	    float fBorderLine = fWaterHeight - 2.0f;
	    if (vPos.y <= fBorderLine) break;

	    //	服务器端将水面以下2米以上处理为 run_speed（有问题）
		//	但未使用加速技能时 swim_speed 小于 run_speed，
		//	可以在水面以下2米以上获取超过 swim_speed 的速度，因此，此处取两者较小值为合理做法
		fSpeedSev = min(m_ExtProps.mv.run_speed, fSpeedSev);
	    break;
	}
	return fSpeedSev;
}

//	Get cool time
int CECHostPlayer::GetCoolTime(int iIndex, int* piMax/* NULL */)
{
	if (iIndex >= 0 && iIndex < GP_CT_MAX)
	{
		if (piMax)
			*piMax = m_aCoolTimes[iIndex].iMaxTime; 

		return m_aCoolTimes[iIndex].iCurTime;
	}

	return 0;
}

bool CECHostPlayer::GetSkillCoolTime(int idSkill, COOLTIME &ct)const
{
	//	获取技能的非公共冷却时间

	bool bFound(false);

	SkillCoolTime::const_iterator it = m_skillCoolTime.find(idSkill);
	if (it != m_skillCoolTime.end())
	{
		ct = it->second;
		bFound = true;
	}

	return bFound;
}

//	Get time counter of using item in pack
bool CECHostPlayer::GetUsingItemTimeCnt(DWORD& dwCurTime, DWORD& dwMaxTime, int* piItem/* NULL */)
{
	if (!m_pWorkMan->IsUsingItem())
		return false;

	CECHPWorkUse* pWork = dynamic_cast<CECHPWorkUse*>(m_pWorkMan->GetRunningWork(CECHPWork::WORK_USEITEM));
	CECCounter& cnt = pWork->GetTimeCounter();
	dwCurTime = cnt.GetCounter();
	dwMaxTime = cnt.GetPeriod();

	if (piItem)
		*piItem = pWork->GetItem();

	return true;
}

//	Fill NPC packs
void CECHostPlayer::FillNPCPack(int iIndex, const ACHAR* szName, int* aItems, 
								float fPriceScale, bool bRecipe)
{
	CECNPCInventory* pPack = m_aNPCPacks[iIndex];
	pPack->RemoveAllItems();
	pPack->SetName(szName);

	ID_SPACE idSpace = bRecipe ? ID_SPACE_RECIPE : ID_SPACE_ESSENCE;
	int iScaleType = bRecipe ? CECIvtrItem::SCALE_MAKE : CECIvtrItem::SCALE_BUY;

	for (int i=0; i < IVTRSIZE_NPCPACK; i++)
	{
		int idItem = aItems[i];
		if (idItem)
		{
			CECIvtrItem* pItem = CECIvtrItem::CreateItem(idItem, 0, 1, idSpace);
			if (pItem->GetClassID() != CECIvtrItem::ICID_RECIPE)
			{
				pItem->GetDetailDataFromLocal();
				pItem->SetPriceScale(iScaleType, fPriceScale);
			}

			pPack->SetItem(i, pItem);
		}
	}
}

//	Set absolute position
void CECHostPlayer::SetPos(const A3DVECTOR3& vPos)
{
	CECPlayer::SetPos(vPos);

	if (m_iBuddyId)
	{
		if (m_bHangerOn)	//	Host is rider
		{
			m_MoveCtrl.SetLastSevPos(vPos);
		}
		else	//	Host is mule
		{
			CECElsePlayer* pBuddy = m_pPlayerMan->GetElsePlayer(m_iBuddyId);
			if (pBuddy)
				pBuddy->SetServerPos(vPos);
		}
	}
}

void CECHostPlayer::Killed(int idKiller)
{
	ClearComActFlagAllRankNodes(true);

	m_dwStates |= GP_STATE_CORPSE;
	m_SkillIDForStateAction = 0;

	CECHPWorkDead* pWork = (CECHPWorkDead*)m_pWorkMan->CreateWork(CECHPWork::WORK_DEAD);
	m_pWorkMan->StartWork_p1(pWork);	

	CECGameRun* pGameRun = g_pGame->GetGameRun();
	CECGameUIMan* pGameUI = pGameRun->GetUIManager()->GetInGameUIMan();
	pGameUI->PopupReviveDialog(true);

	SetUseGroundNormal(true);

	//	Print a notify
	if (ISNPCID(idKiller))
	{
		CECNPC* pKiller = pGameRun->GetWorld()->GetNPCMan()->GetNPC(idKiller);
		if (pKiller)
			pGameRun->AddFixedMessage(FIXMSG_BEKILLED, pKiller->GetName());
	}
	else if (ISPLAYERID(idKiller))
	{
		ACString strName = pGameRun->GetPlayerName(idKiller, true);

		CECPlayer* pPlayer = pGameRun->GetWorld()->GetPlayerMan()->GetPlayer(idKiller);
		if (pPlayer && pPlayer->GetShowNameInCountryWar())
			strName = pPlayer->GetNameInCountryWar();

		pGameRun->AddFixedMessage(FIXMSG_BEKILLED, strName);
	}
}

//	Get off pet
void CECHostPlayer::GetOffPet(bool bResetData)
{
	CECPlayer::GetOffPet(bResetData);

	//	Restore cdr info
	m_CDRInfo.vExtent		= m_aabbServer.Extents;
	//m_CDRInfo.fStepHeight	= m_MoveConst.fStepHei;
}

//	Find mine tool in packages
bool CECHostPlayer::FindMineTool(int tidMine, int* piPack, int* piIndex, int* pidTool)
{
	if (!tidMine)
		return false;

	DATA_TYPE DataType;
	const void* pDataPtr = g_pGame->GetElementDataMan()->get_data_ptr(tidMine, ID_SPACE_ESSENCE, DataType);

	if (DataType != DT_MINE_ESSENCE)
	{
		ASSERT(DataType != DT_MINE_ESSENCE);
		return false;
	}

	const MINE_ESSENCE* pData = (const MINE_ESSENCE*)pDataPtr;
	int idTool = pData->id_equipment_required;
	bool bRet = true;

	if (idTool)
	{
		int iIndex = m_pPack->FindItem(idTool);
		if (iIndex >= 0)
		{
			*piPack	 = IVTRTYPE_PACK;
			*piIndex = iIndex;
			*pidTool = idTool;
		}
		else if ((iIndex = m_pTaskPack->FindItem(idTool)) >= 0)
		{
			*piPack	 = IVTRTYPE_TASKPACK;
			*piIndex = iIndex;
			*pidTool = idTool;
		}
		else
			bRet = false;
	}
	else
	{
		*piPack	 = 0;
		*piIndex = 0;
		*pidTool = 0;
	}

	return bRet;
}

//	When host is going to move, this function will be called
void CECHostPlayer::GoingMove()
{
	if (DoingSessionPose())
		g_pGame->GetGameSession()->c2s_CmdCancelAction();
}

//	Update positions of team member
void CECHostPlayer::UpdateTeamMemberPos(DWORD dwDeltaTime)
{
	if (!m_pTeam)
		return;

	m_TLPosCnt.IncCounter(dwDeltaTime);
	m_TMPosCnt.IncCounter(dwDeltaTime);

	int aMemIDs[32], iMemCnt=0;
	int idCurInst = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
	
	//	Update team leader's position
	if (m_pTeam->GetLeaderID() != m_PlayerInfo.cid)
	{
		//	Team leader should be the first member if all member data is ready
		CECTeamMember* pLeader = m_pTeam->GetLeader();
		if (pLeader)
		{
			CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(m_pTeam->GetLeaderID());
			if (pPlayer && !pPlayer->InGhostState())
			{
				pLeader->SetPos(idCurInst, pPlayer->GetPos(), true);
			}
			else if (m_TLPosCnt.IsFull())
			{
				m_TLPosCnt.Reset();
				aMemIDs[iMemCnt++] = pLeader->GetCharacterID();
			}
		}
	}

	//	Update team members' positions
	int i, iNumMem = m_pTeam->GetMemberNum();
	for (i=0; i < iNumMem; i++)
	{
		CECTeamMember* pMem = m_pTeam->GetMemberByIndex(i);
		int cid = pMem->GetCharacterID();
		if (cid == m_pTeam->GetLeaderID() || cid == m_PlayerInfo.cid)
			continue;

		CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(cid);
		if (pPlayer && !pPlayer->InGhostState())
			pMem->SetPos(idCurInst, pPlayer->GetPos(), true);
		else if (m_TMPosCnt.IsFull())
			aMemIDs[iMemCnt++] = cid;
	}

	if (m_TMPosCnt.IsFull())
		m_TMPosCnt.Reset();
	
	if (iMemCnt)
	{
		//	Try to get teammates position from server. 
		g_pGame->GetGameSession()->c2s_CmdTeamMemberPos(iMemCnt, aMemIDs);
	}
}

//	Freeze / Unfreeze specified item
bool CECHostPlayer::FreezeItem(int iIvtr, int iIndex, bool bFreeze, bool bFreezeByNet)
{
	CECInventory* pPack = GetPack(iIvtr);
	if (!pPack)
		return false;

	CECIvtrItem* pItem = pPack->GetItem(iIndex);
	if (!pItem)
		return false;

	if (bFreezeByNet)
		pItem->NetFreeze(bFreeze);
	else
		pItem->Freeze(bFreeze);

	return true;
}

//	Check whether player pickup specified matter
bool CECHostPlayer::CanTakeItem(int idItem, int iAmount)
{
	bool bCanPick = false;

	if (ISMONEYTID(idItem))
	{
		if (GetMoneyAmount() < GetMaxMoneyAmount())
			bCanPick = true;
	}
	else
	{
		if (m_pPack->CanAddItem(idItem, iAmount, false) >= 0)
			bCanPick = true;
	}

	return bCanPick;
}

int CECHostPlayer::GetEquippedItem(int index)const{
	if (index < 0 || index >= m_pEquipPack->GetSize()){
		ASSERT(false);
		return 0;
	}
	CECIvtrEquip* pEquip = dynamic_cast<CECIvtrEquip*>(m_pEquipPack->GetItem(index));
	if (!pEquip){
		return 0;
	}
	return pEquip->GetTemplateID();
}

//	Get number of equipped items of specified suite
int CECHostPlayer::GetEquippedSuiteItem(int idSuite, int* aItems/* NULL */)
{
	int i, iItemCnt = 0;

	for (i=0; i < m_pEquipPack->GetSize(); i++)
	{
		CECIvtrEquip* pEquip = (CECIvtrEquip*)m_pEquipPack->GetItem(i);
		if (!pEquip || pEquip->GetSuiteID() != idSuite)
			continue;

		if (!CanUseEquipment(pEquip))
			continue;

		if (pEquip->GetClassID() == CECIvtrItem::ICID_GENERALCARD) {
			CECIvtrGeneralCard* pCard = dynamic_cast<CECIvtrGeneralCard*>(pEquip);
			if (pCard) {
				if (pCard->GetEssence().type != i - EQUIPIVTR_GENERALCARD1)
					continue;
			}
		}

		if (aItems)
			aItems[iItemCnt] = pEquip->GetTemplateID();
		
		iItemCnt++;
	}

	return iItemCnt;
}

//	Does host player have specified way point ?
bool CECHostPlayer::HasWayPoint(WORD wID)
{
	for (int i=0; i < m_aWayPoints.GetSize(); i++)
	{
		if (m_aWayPoints[i] == wID)
			return true;
	}

	return false;
}

//	Team invite
void CECHostPlayer::TeamInvite(const GNET::PlayerBriefInfo& Info)
{
	for (int i=0; i < m_aTeamInvs.GetSize(); i++)
	{
		const TEAMINV& Inv = m_aTeamInvs[i];
		if (Inv.idLeader == Info.roleid)
		{
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			ACString strName = ACString((const ACHAR*)Info.name.begin(), Info.name.size() / sizeof (ACHAR));
			pGameUI->PopupInviteGroupMenu(Inv.idLeader, strName, Info.occupation, Inv.seq, Inv.wPickFlag);
			
			//	Remove this invivation from array
			m_aTeamInvs.RemoveAtQuickly(i);
			return;	
		}
	}
}

//	Rearrange booth packages
void CECHostPlayer::RearrangeBoothPacks()
{
	struct INFO
	{
		int	iOrigin;
		int iAmount;
		int iUnitPrice;
	};

	INFO aSellItems[IVTRSIZE_BOOTHSPACK_MAX];
	INFO aBuyItems[IVTRSIZE_BOOTHBPACK_MAX];
	memset(aSellItems, 0, sizeof (aSellItems));
	memset(aBuyItems, 0, sizeof (aBuyItems));

	int i, iCnt=0;

	//	Rearrange booth sell package
	for (i=0; i < m_pBoothSPack->GetSize(); i++)
	{
		CECIvtrItem* pItem1 = m_pBoothSPack->GetItem(i);
		if (!pItem1)
			continue;

		const CECDealInventory::ITEMINFO& ti = m_pBoothSPack->GetItemInfo(i);
		CECIvtrItem* pItem2 = m_pPack->GetItem(ti.iOrigin);

		if (!pItem2 || pItem2->GetTemplateID() != pItem1->GetTemplateID())
			continue;

		//	Save item's information
		INFO& info = aSellItems[iCnt++];
		info.iAmount	= a_Min(ti.iAmount, pItem2->GetCount());
		info.iOrigin	= ti.iOrigin;
		info.iUnitPrice	= pItem1->GetUnitPrice();
	}

	m_pBoothSPack->RemoveAllItems();
	for (i=0; i < iCnt; i++)
	{
		const INFO& info = aSellItems[i];
		CECIvtrItem* pItem = m_pPack->GetItem(info.iOrigin);
		m_pBoothSPack->AddBoothItem(pItem, info.iOrigin, info.iAmount, info.iUnitPrice);
		pItem->Freeze(true);
	}

	//	Rearrange booth buy package
	iCnt = 0;
	for (i=0; i < m_pBoothBPack->GetSize(); i++)
	{
		CECIvtrItem* pItem1 = m_pBoothBPack->GetItem(i);
		if (!pItem1)
			continue;

		const CECDealInventory::ITEMINFO& ti = m_pBoothBPack->GetItemInfo(i);
		CECIvtrItem* pItem2 = m_pPack->GetItem(ti.iOrigin);

		if (!pItem2 || pItem2->GetTemplateID() != pItem1->GetTemplateID())
			continue;

		//	Save item's information
		INFO& info = aBuyItems[iCnt++];
		info.iAmount	= ti.iAmount;
		info.iOrigin	= ti.iOrigin;
		info.iUnitPrice	= pItem1->GetUnitPrice();
	}
	
	m_pBoothBPack->RemoveAllItems();
	for (i=0; i < iCnt; i++)
	{
		const INFO& info = aBuyItems[i];
		CECIvtrItem* pItem = m_pPack->GetItem(info.iOrigin);
		m_pBoothBPack->AddBoothItem(pItem, info.iOrigin, info.iAmount, info.iUnitPrice);
		pItem->Freeze(true);
	}
}

//	Get deadly strike rate (%)
int CECHostPlayer::GetDeadlyStrikeRate()
{
	const ROLEBASICPROP &bp = GetBasicProps();
	return bp.iCritRate;
}

//  Get soul power
int CECHostPlayer::GetSoulPower()
{
	//魂力=等级*等级+等级*50+武器精炼成本*1.5+其他所有装备精炼成本之和*0.25
	const int refine_cost[13] = {0,1,5,15,40,100,230,445,850,1600,2970,5495,10140};
	double power = m_BasicProps.iLevel*m_BasicProps.iLevel + m_BasicProps.iLevel*50;

	int i(0);
	for(i=0;i<m_pEquipPack->GetSize();i++)
	{
		CECIvtrItem* pItem = (CECIvtrItem*)m_pEquipPack->GetItem(i);
		if(pItem && pItem->IsEquipment() && !((CECIvtrEquip*)pItem)->IsDestroying())
		{
			CECIvtrEquip* pEquip = (CECIvtrEquip*)pItem;

			if(!pEquip->GetCurEndurance() ||
				!CanUseEquipment(pEquip))
				continue;

			if(pEquip->IsWeapon())
				power += refine_cost[pEquip->GetRefineLevel()] * 1.5;
			else
				power += refine_cost[pEquip->GetRefineLevel()] * 0.25;

			power += pEquip->GetSoulPowerAdded();
		}
	}

	//	计算套装上的魂力属性
	typedef abase::hash_map<int, int>	SuiteCounter;
	SuiteCounter suiteCounter;
	for(i=0;i<m_pEquipPack->GetSize();i++)
	{
		//	使用 map 收集有效套装，以处理多个装备共用同一套装
		CECIvtrItem* pItem = (CECIvtrItem*)m_pEquipPack->GetItem(i);
		if(pItem && pItem->IsEquipment() && !((CECIvtrEquip*)pItem)->IsDestroying())
		{
			CECIvtrEquip* pEquip = (CECIvtrEquip*)pItem;

			if(!pEquip->GetCurEndurance() ||
				!CanUseEquipment(pEquip))
				continue;

			int idSuite = pEquip->GetSuiteID();
			if (!idSuite)
				continue;

			int iItemCnt = GetEquippedSuiteItem(idSuite);
			if (iItemCnt > 1)
				suiteCounter[idSuite] = iItemCnt;
		}
	}
	if (!suiteCounter.empty())
	{
		DATA_TYPE dt = DT_INVALID;
		elementdataman* pDataMan = g_pGame->GetElementDataMan();
		CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();

		for (SuiteCounter::iterator it = suiteCounter.begin(); it != suiteCounter.end(); ++ it)
		{
			//	对每件套装，查找其激活的属性中的魂力属性值，进行累积
			int idSuite = it->first;
			int iItemCnt = it->second;

			const void* pData = pDataMan->get_data_ptr(idSuite, ID_SPACE_ESSENCE, dt);
			if (dt != DT_SUITE_ESSENCE)
			{
				// 忽略卡牌套装
				//ASSERT(dt == DT_SUITE_ESSENCE);
				continue;
			}
			const SUITE_ESSENCE* pSuiteEss = (const SUITE_ESSENCE*)pData;

			//	检查各附加属性，看是否有魂力值增加
			for (i = 1; i < iItemCnt; ++ i)
			{
				int idProp = (int)pSuiteEss->addons[i-1].id;
				if (!idProp) continue;

				CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(idProp);
				if (!Pair.second || (*Pair.first != 156))
					continue;

				pData = pDataMan->get_data_ptr(idProp, ID_SPACE_ADDON, dt);
				if (dt != DT_EQUIPMENT_ADDON)
				{
					ASSERT(dt == DT_EQUIPMENT_ADDON);
					continue;
				}
				const EQUIPMENT_ADDON* pAddOn = (const EQUIPMENT_ADDON*)pData;
				if (pAddOn->num_params != 1)
				{
					ASSERT(pAddOn->num_params == 1);
					continue;
				}

				power += pAddOn->param1;
			}
		}
	}

	return (int)power;
}

//	Is host in sliding state (in the state, host is sliding on slope) ?
bool CECHostPlayer::InSlidingState()
{
	if (m_iMoveMode != MOVE_SLIDE)
		return false;
	
	if (CECHPWorkStand* pWork = dynamic_cast<CECHPWorkStand*>(m_pWorkMan->GetRunningWork(CECHPWork::WORK_STAND))){
		if (pWork->GetStopSlideFlag()){
			return false;
		}
	}

	return true;
}

//	On start binding buddy
void CECHostPlayer::OnStartBinding(int idMule, int idRider)
{
	CECPlayer::OnStartBinding(idMule, idRider);

	//	Stop current work and restore to stand state
	if (!m_pWorkMan->IsStanding())
		m_pWorkMan->FinishAllWork(false);

	//	If host is rider, goto be bound state
	if (idRider == GetCharacterID())
	{
		ShowWing(false);
		m_CDRInfo.vTPNormal.Clear();

		CECHPWorkBeBound* pWork = (CECHPWorkBeBound*)m_pWorkMan->CreateWork(CECHPWork::WORK_BEBOUND);
		m_pWorkMan->StartWork_p1(pWork);
	}
}

//	Apply combo skill
bool CECHostPlayer::ApplyComboSkill(int iGroup, bool bIgnoreAtkLoop, int iForceAtk)
{
	//	Clear current combo skill
	ClearComboSkill();

	if (!(m_pComboSkill = new CECComboSkill))
		return false;

	bool bForceAttack;
	if( iForceAtk < 0 )
		bForceAttack = glb_GetForceAttackFlag(NULL);
	else
		bForceAttack = iForceAtk > 0 ? true : false;

	if (!(m_pComboSkill->Init(this, iGroup, m_idSelTarget, bForceAttack, bIgnoreAtkLoop)))
	{
		delete m_pComboSkill;
		m_pComboSkill = NULL;
		return false;
	}

	//	Start skill group
	m_pComboSkill->Continue(m_bMelee);

	return true;
}

//	Clear combo skill
void CECHostPlayer::ClearComboSkill()
{
	if (m_pComboSkill)
	{
		delete m_pComboSkill;
		m_pComboSkill = NULL;
	}
}

//	Replace specified skill with it's senior skill
void CECHostPlayer::ReplaceJuniorSkill(CECSkill* pSeniorSkill)
{
	if (!pSeniorSkill)
	{
		ASSERT(pSeniorSkill);
		return;
	}
	SkillArrayWrapper juniorArray = pSeniorSkill->GetJunior();
	if (juniorArray.Empty()){
		ASSERT(false);
		return;
	}

	int i, idSenior = pSeniorSkill->GetSkillID();

	//	Update shortcuts ...
	for (i=0; i < NUM_HOSTSCSETS1; i++)
	{
		if (m_aSCSets1[i])
			m_aSCSets1[i]->ReplaceSkillID(juniorArray, pSeniorSkill);
	}

	for (i=0; i < NUM_HOSTSCSETS2; i++)
	{
		if (m_aSCSets2[i])
			m_aSCSets2[i]->ReplaceSkillID(juniorArray, pSeniorSkill);
	}
	
	//	Update skill groups ...
	EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();
	for (i=0; i < EC_COMBOSKILL_NUM; i++)
	{
		bool bChange = false;

		for (int j=0; j < EC_COMBOSKILL_NUM; j++)
		{
			if (!vs.comboSkill[j].nIcon)
				continue;

			for (int n=0; n < EC_COMBOSKILL_LEN; n++)
			{
				if (juniorArray.Find(vs.comboSkill[j].idSkill[n]))
				{
					vs.comboSkill[j].idSkill[n] = idSenior;
					bChange = true;
				}
			}
		}

		if (bChange)
			g_pGame->GetConfigs()->SetVideoSettings(vs);
	}

	if (m_pComboSkill)
		m_pComboSkill->ReplaceSkillID(juniorArray, idSenior);

	if (m_pPrepSkill && juniorArray.Find(m_pPrepSkill->GetSkillID())){
		m_pPrepSkill = NULL;
	}
	if (m_pCurSkill && juniorArray.Find(m_pCurSkill->GetSkillID())){
		m_pCurSkill = NULL;
	}
	if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_TRACEOBJECT))
	{
		CECHPWorkTrace* pWorkTrace = dynamic_cast<CECHPWorkTrace*>(pWork);
		if (pWorkTrace->GetTraceReason() == CECHPWorkTrace::TRACE_SPELL &&
			pWorkTrace->GetPrepSkill() && juniorArray.Find(pWorkTrace->GetPrepSkill()->GetSkillID())){
			pWorkTrace->Reset();
		}
	}

	//	Note: Replace skills in m_aPtSkills and m_aPsSkills at last !!!
	//	Check positive skills
	for (i=0; i < m_aPtSkills.GetSize(); i++)
	{
		CECSkill* pSkill = m_aPtSkills[i];
		if (juniorArray.Find(pSkill->GetSkillID()))
		{
			delete m_aPtSkills[i];
			m_aPtSkills[i] = pSeniorSkill;
			break;
		}
	}
	for (++i; i < m_aPtSkills.GetSize(); )
	{
		CECSkill* pSkill = m_aPtSkills[i];
		if (juniorArray.Find(pSkill->GetSkillID())){
			m_aPtSkills.RemoveAt(i);
		}else{
			++ i;
		}
	}
	
	//	Check passive skills
	for (i=0; i < m_aPsSkills.GetSize(); i++)
	{
		CECSkill* pSkill = m_aPsSkills[i];
		if (juniorArray.Find(pSkill->GetSkillID()))
		{
			delete m_aPsSkills[i];
			m_aPsSkills[i] = pSeniorSkill;
			break;
		}
	}
	for (++i; i < m_aPsSkills.GetSize(); )
	{
		CECSkill* pSkill = m_aPsSkills[i];
		if (juniorArray.Find(pSkill->GetSkillID())){
			m_aPsSkills.RemoveAt(i);
		}else{
			++ i;
		}
	}

	//	更新界面中可能含有指针引用的地方
	CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUIMan->UpdateSkillRelatedUI();
}

void CECHostPlayer::SaveSkillGrpShortcut(std::vector<SkillGrpShortCutConfig>& skillGrpSCConfigArray, CECShortcutSet** aSCSets, int count)
{
	int i(0);
	for (i=0; i < count; i++){
		if (aSCSets[i]){
			int j(0);
			for (j=0; j < aSCSets[i]->GetShortcutNum(); j++){
				CECShortcut* pSC = aSCSets[i]->GetShortcut(j);
				if (!pSC || pSC->GetType() != CECShortcut::SCT_SKILLGRP )
					continue;
				CECSCSkillGrp* pSkillGrpSC = (CECSCSkillGrp*)pSC;
				SkillGrpShortCutConfig skillGrpSCConfig;
				skillGrpSCConfig.setNum = i;
				skillGrpSCConfig.slotNum = j;
				skillGrpSCConfig.groupIndex = pSkillGrpSC->GetGroupIndex();
				skillGrpSCConfigArray.push_back(skillGrpSCConfig);
			}				
		}
	}
}

void CECHostPlayer::SaveSkillShortcut(std::vector<SkillShortCutConfig>& skillSCConfigArray, CECShortcutSet** aSCSets, int count)
{
	int i(0);
	for (i=0; i < count; i++){
		if (aSCSets[i]){
			int j(0);
			for (j=0; j < aSCSets[i]->GetShortcutNum(); j++){
				CECShortcut* pSC = aSCSets[i]->GetShortcut(j);
				if (!pSC || pSC->GetType() != CECShortcut::SCT_SKILL )
					continue;
				CECSCSkill* pSkillSC = (CECSCSkill*)pSC;
				int iOldSkillId = pSkillSC->GetSkill()->GetSkillID();
				if (iOldSkillId == 0)
					continue;
				SkillShortCutConfig skillSCConfig;
				skillSCConfig.setNum = i;
				skillSCConfig.slotNum = j;
				skillSCConfig.skillId = iOldSkillId;
				skillSCConfigArray.push_back(skillSCConfig);
			}				
		}
	}
}

void CECHostPlayer::ValidateSkillGrpShortcut(std::vector<SkillGrpShortCutConfig> & skillGrpSCConfigArray)
{
	std::vector<SkillGrpShortCutConfig>::iterator it;
	for (it = skillGrpSCConfigArray.begin(); it != skillGrpSCConfigArray.end(); it++)
	{
		EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();
		if (vs.comboSkill[it->groupIndex].nIcon == 0)
			it->groupIndex = -1;	// -1 表示无效的连击技快捷键
	}
}
void CECHostPlayer::ConvertSkillShortcut(std::vector<SkillShortCutConfig> & skillSCConfigArray)
{
	std::vector<SkillShortCutConfig>::iterator it;
	for (it = skillSCConfigArray.begin(); it != skillSCConfigArray.end(); it++){
		ASSERT(it->skillId != 0);
		int convertSkillId = CECSkillConvert::Instance().GetConvertSkill(it->skillId);
		it->skillId = ( convertSkillId == 0 ) ? it->skillId : convertSkillId;
	}
}

void CECHostPlayer::AssignSkillGrpShortcut(const std::vector<SkillGrpShortCutConfig> & skillGrpSCConfigArray, CECShortcutSet** aSCSets)
{
	std::vector<SkillGrpShortCutConfig>::const_iterator it;
	for (it = skillGrpSCConfigArray.begin(); it != skillGrpSCConfigArray.end(); it++){
		if(it->groupIndex != -1)
			aSCSets[it->setNum]->CreateSkillGroupShortcut(it->slotNum, it->groupIndex);
	}
}

void CECHostPlayer::AssignSkillShortcut(const std::vector<SkillShortCutConfig> & skillSCConfigArray, CECShortcutSet** aSCSets)
{
	std::vector<SkillShortCutConfig>::const_iterator it;
	for (it = skillSCConfigArray.begin(); it != skillSCConfigArray.end(); it++)
	{
		ASSERT(it->skillId != 0);
		CECSkill* convertSkill = GetNormalSkill(it->skillId);
		if (convertSkill){
			aSCSets[it->setNum]->CreateSkillShortcut(it->slotNum,convertSkill);
		}
	}
}

void CECHostPlayer::ConvertComboSkill()
{
	EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();
	int i(0);
	for (i = 0; i< EC_COMBOSKILL_NUM; i++){
		if( 0 == vs.comboSkill[i].nIcon ){
			continue;
		}
		int j(0);
		for (j = 0 ; j<EC_COMBOSKILL_LEN; j++){
			int oldSkillId = vs.comboSkill[i].idSkill[j];
			if ( 0 == oldSkillId ){
				continue;
			}
			int convertedSkillId = CECSkillConvert::Instance().GetConvertSkill(oldSkillId);
			int newSkillId = ( convertedSkillId == 0 ) ? oldSkillId : convertedSkillId;
			if ( GetNormalSkill(newSkillId) || newSkillId == -1 || newSkillId == -2){	//循环和普攻快捷键
				vs.comboSkill[i].idSkill[j] = newSkillId;
			}else{
				vs.comboSkill[i].Clear(oldSkillId);
				vs.comboSkill[i].nIcon = 0;
				break;
			}
		}			
	}
	g_pGame->GetConfigs()->SetVideoSettings(vs);
}

//	Do emote action
bool CECHostPlayer::DoEmote(int idEmote)
{
	if (!m_pWorkMan->IsStanding())
		return false;

	CECHPWorkStand* pWork = dynamic_cast<CECHPWorkStand*>(m_pWorkMan->GetRunningWork(CECHPWork::WORK_STAND));

	int iAction = ACT_STAND;
	bool bSession = false;

	//	Select action according to pose
	switch (idEmote)
	{
	case ROLEEXP_WAVE:		iAction = ACT_EXP_WAVE;			break;
	case ROLEEXP_NOD:		iAction = ACT_EXP_NOD;			break;
	case ROLEEXP_SHAKEHEAD:	iAction = ACT_EXP_SHAKEHEAD;	break;
	case ROLEEXP_SHRUG:		iAction = ACT_EXP_SHRUG;		break;
	case ROLEEXP_LAUGH:		iAction = ACT_EXP_LAUGH;		break;
	case ROLEEXP_ANGRY:		iAction = ACT_EXP_ANGRY;		break;
	case ROLEEXP_STUN:		iAction = ACT_EXP_STUN;			break;
	case ROLEEXP_DEPRESSED:	iAction = ACT_EXP_DEPRESSED;	break;
	case ROLEEXP_KISSHAND:	iAction = ACT_EXP_KISSHAND;		break;
	case ROLEEXP_SHY:		iAction = ACT_EXP_SHY;			break;
	case ROLEEXP_SALUTE:	iAction = ACT_EXP_SALUTE;		break;

	case ROLEEXP_SITDOWN:	
		
		iAction = ACT_EXP_SITDOWN;	
		bSession = true;
		break;

	case ROLEEXP_ASSAULT:	iAction = ACT_EXP_ASSAULT;		break;
	case ROLEEXP_THINK:		iAction = ACT_EXP_THINK;		break;
	case ROLEEXP_DEFIANCE:	iAction = ACT_EXP_DEFIANCE;		break;
	case ROLEEXP_VICTORY:	iAction = ACT_EXP_VICTORY;		break;
	case ROLEEXP_GAPE:		iAction = ACT_GAPE;				break;
	case ROLEEXP_KISS:		iAction = ACT_EXP_KISS;			break;
	case ROLEEXP_FIGHT:		iAction = ACT_EXP_FIGHT;		break;
	case ROLEEXP_ATTACK1:	iAction = ACT_EXP_ATTACK1;		break;
	case ROLEEXP_ATTACK2:	iAction = ACT_EXP_ATTACK2;		break;
	case ROLEEXP_ATTACK3:	iAction = ACT_EXP_ATTACK3;		break;
	case ROLEEXP_ATTACK4:	iAction = ACT_EXP_ATTACK4;		break;
	case ROLEEXP_DEFENCE:	iAction = ACT_EXP_DEFENCE;		break;	
	case ROLEEXP_FALL:		iAction = ACT_EXP_FALL;			break;
	case ROLEEXP_FALLONGROUND:	iAction = ACT_EXP_FALLONGROUND;	break;
	case ROLEEXP_LOOKAROUND:	iAction = ACT_EXP_LOOKAROUND;	break;	
	case ROLEEXP_DANCE:		iAction = ACT_EXP_DANCE;		break;
	case ROLEEXP_FASHIONWEAPON:	iAction = ACT_EXP_FASHIONWEAPON;		break;
	case ROLEEXP_TWO_KISS:	iAction = ACT_TWO_KISS;			break;
	case ROLEEXP_FIREWORK:	iAction = ACT_ATTACK_TOSS;	break;
	default:
		ASSERT(0);
		break;
	}

	pWork->SetPoseAction(iAction, bSession); 

	return true;
}

//	Hatch pet
bool CECHostPlayer::HatchPet(int iIvtrIdx)
{
	CECIvtrItem* pItem = m_pPack->GetItem(iIvtrIdx);
	if (!pItem || pItem->GetClassID() != CECIvtrItem::ICID_PETEGG)
		return false;

	CECGameRun* pGameRun = g_pGame->GetGameRun();

	CECIvtrPetEgg* pEgg = (CECIvtrPetEgg*)pItem;
	const IVTR_ESSENCE_PETEGG& e = pEgg->GetEssence();
	
	//	Check profession
	if (!(e.req_class & (1 << m_iProfession)))
	{
		pGameRun->AddFixedMessage(FIXMSG_WRONGPROF);
		return false;
	}

	//	Check level
	int iLevelReq = a_Max((int)e.level, e.req_level);
	if (GetMaxLevelSofar() < iLevelReq)
	{
		pGameRun->AddFixedMessage(FIXMSG_LEVELTOOLOW);
		return false;
	}

	//	Check money
	if (GetMoneyAmount() < pEgg->GetDBEssence()->money_hatched)
	{
		pGameRun->AddFixedMessage(FIXMSG_NEEDMONEY);
		return false;
	}

	//	Check whether pet corral is full
	if (!m_pPetCorral->GetEmptySlotNum())
	{
		pGameRun->AddFixedMessage(FIXMSG_PET_CORRALFULL);
		return false;
	}

	g_pGame->GetGameSession()->c2s_CmdNPCSevHatchPet(iIvtrIdx, pItem->GetTemplateID());

	return true;
}

//	Restore pet
bool CECHostPlayer::RestorePet(int iPetIdx)
{
	CECPetData* pPet = m_pPetCorral->GetPetData(iPetIdx);
	if (!pPet || iPetIdx == m_pPetCorral->GetActivePetIndex())
		return false;

	CECGameRun* pGameRun = g_pGame->GetGameRun();

	//	Check money
	DATA_TYPE DataType;
	PET_EGG_ESSENCE* pdbe = (PET_EGG_ESSENCE*)g_pGame->GetElementDataMan()->get_data_ptr(pPet->GetEggID(), ID_SPACE_ESSENCE, DataType);
	if (DataType != DT_PET_EGG_ESSENCE || !pdbe)
		return false;

	if (GetMoneyAmount() < pdbe->money_restored)
	{
		pGameRun->AddFixedMessage(FIXMSG_NEEDMONEY);
		return false;
	}

	//	Check package space
	if (!m_pPack->GetEmptySlotNum())
	{
		pGameRun->AddFixedMessage(FIXMSG_PACKISFULL);
		return false;
	}

	g_pGame->GetGameSession()->c2s_CmdNPCSevRestorePet(iPetIdx);

	return true;
}

bool CECHostPlayer::RebuildPet(int iPetIdx, int iSelItem, bool bNature)
{
	if (!CanDo(CANDO_REBUILDPET))
		return false;
	
	CECGameRun* pGameRun = g_pGame->GetGameRun();
	
	CECPetData* pPet = m_pPetCorral->GetPetData(iPetIdx);
	if (!pPet)
		return false;
	
	// active pet
	if(m_pPetCorral->GetActivePetIndex() == iPetIdx)
		return false;

	if(!IsInSanctuary())
	{
		pGameRun->AddFixedMessage(FIXMSG_REBUILD_PET_IN_SANCTUARY_ONLY);
		return false;
	}

	if(!m_pPetCorral->CheckRebuildPetItemCond(iPetIdx,iSelItem,bNature ? 1:2))
		return false;
	
	//	If host could't stop naturally, cancel summoning
//	if (!NaturallyStopMoving())
//		return false;
	
	//	检查当前是否禁止召唤骑宠
//	if (pPet->IsMountPet() && m_playerLimits.test(PLAYER_LIMIT_NOMOUNT))
//		return false;

	if (IsFollowing())
		m_pWorkMan->FinishAllWork(true);
	
	if(bNature)
		g_pGame->GetGameSession()->c2s_CmdPetRebuildNature(iPetIdx,iSelItem);
	else
		g_pGame->GetGameSession()->c2s_CmdPetRebuildInheritRatio(iPetIdx,iSelItem);

	//test
	//GetMoveControl().SendStopMoveCmd();

	return true;
}
//	Summon pet
bool CECHostPlayer::SummonPet(int iPetIdx)
{
	if (m_pActionSwitcher)
		m_pActionSwitcher->PostMessge(CECActionSwitcherBase::MSG_MOUNTPET);

	CECGameRun* pGameRun = g_pGame->GetGameRun();
	
	CECPetData* pPet = m_pPetCorral->GetPetData(iPetIdx);
	if (!pPet)
		return false;

	if (!CanDo(CANDO_SUMMONPET))
		return false;	

	//	Couldn't summon daed pet
	if (pPet->IsDead())
	{
		pGameRun->AddFixedMessage(FIXMSG_PET_DEAD);
		return false;
	}

	//	If host could't stop naturally, cancel summoning
	if (!NaturallyStopMoving())
		return false;

	//	检查当前是否禁止召唤骑宠
	if (pPet->IsMountPet() && m_playerLimits.test(PLAYER_LIMIT_NOMOUNT))
		return false;

	if (m_ReincarnationCount) {
		int iLevelRequired = pPet->GetLevel() - 35 - m_ReincarnationCount * 5;
		if (m_BasicProps.iLevel < iLevelRequired) {
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			if (pGameUI) {
				ACString strText;
				strText.Format(pGameUI->GetStringFromTable(10787), iLevelRequired);
				pGameUI->MessageBox("", strText, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			}
			return false;
		}
	}

	g_pGame->GetGameSession()->c2s_CmdPetSummon(iPetIdx);

	return true;
}

//	Recall pet
bool CECHostPlayer::RecallPet()
{
	if (m_pActionSwitcher)
		m_pActionSwitcher->PostMessge(CECActionSwitcherBase::MSG_MOUNTPET);

	//	If host could't stop naturally, cancel recalling
	if (!NaturallyStopMoving())
		return false;

	g_pGame->GetGameSession()->c2s_CmdPetRecall();
	return true;
}

//	Banish pet
bool CECHostPlayer::BanishPet(int iPetIdx)
{
	CECPetData* pPet = m_pPetCorral->GetPetData(iPetIdx);
	if (!pPet)
		return false;

	g_pGame->GetGameSession()->c2s_CmdPetBanish(iPetIdx);
	return true;
}

/*	Is host operating pet ?
	return value:

	0: host doesn't operating pet.
	1: host is summoning pet.
	2: host is recalling pet.
	3: host is banishing pet.
*/
int CECHostPlayer::IsOperatingPet()
{
	if (CECHPWorkConcentrate* pWork = dynamic_cast<CECHPWorkConcentrate*>(m_pWorkMan->GetRunningWork(CECHPWork::WORK_CONCENTRATE))){
		if (pWork->GetDoWhat() == CECHPWorkConcentrate::DO_SUMMONPET)
			return 1;
		else if (pWork->GetDoWhat() == CECHPWorkConcentrate::DO_RECALLPET)
			return 2;
		else if (pWork->GetDoWhat() == CECHPWorkConcentrate::DO_BANISHPET)
			return 3;
		else if (pWork->GetDoWhat() == CECHPWorkConcentrate::DO_RESTOREPET)
			return 4;
	}
	return 0;
}

//  Update goblin
bool CECHostPlayer::UpdateGoblin()
{
 	CECIvtrGoblin* pGoblin = (CECIvtrGoblin*)m_pEquipPack->GetItem(EQUIPIVTR_GOBLIN);
 	if(!pGoblin)
 	{
 		ASSERT(0);
 		return false;
 	}

	if( !m_pGoblin->Init(pGoblin->GetTemplateID(), pGoblin, this) )
	{
		m_pGoblin->Release();
		delete m_pGoblin;
		m_pGoblin = NULL;
		a_LogOutput(1, "CECHostPlayer::UpdateGoblin(), failed to load sprite!");
	}

	return true;
}
bool CECHostPlayer::NeedPassword()
{
	if(m_bFirstTBOpen && m_bFirstFashionOpen)
		return true;

	return false;
}

// Get key object(NPC..) coordinates
A3DVECTOR3 CECHostPlayer::GetObjectCoordinates(int idTarget, CECGame::ObjectCoords & TargetCoord, bool& bInTable)
{
	TargetCoord.clear();

	A3DVECTOR3 vDestPos(0);

	bInTable = false;
	//  Get object coordinates from CECGame::m_CoordTab
	char szText[100];
	itoa(idTarget, szText, 10);
	abase::vector<CECGame::OBJECT_COORD> tempCoord;
	int iCount = g_pGame->GetObjectCoord(AS2AC(szText), tempCoord);
	if(iCount == 0)
	{
		return vDestPos;
	}

	float fMinDist = 99999999.0f;

	//  Get Current map name, such as 'a32' etc.
	int idInstance = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
	CECInstance *pInstance = g_pGame->GetGameRun()->GetInstance(idInstance);
	ACString strCurMap = AS2AC(pInstance->GetPath());
	//  先检查同一地图内是否有要查找的物品
	bool bHasObjectInCurrentInstance = std::find(tempCoord.begin(), tempCoord.end(), strCurMap) != tempCoord.end();
 	for(int i=0;i<iCount;i++)
 	{
 		CECGame::OBJECT_COORD objCoord = tempCoord[i];

		if(strCurMap == objCoord.strMap)
		{
			TargetCoord.push_back(objCoord);

			//  Check if this is the nearest target
			float tempDist = a3d_Magnitude(objCoord.vPos - GetPos());
			if(tempDist < fMinDist)
			{
				fMinDist = tempDist;
				bInTable = true;
				vDestPos = objCoord.vPos;
			}
		}
		// 当前地图里没有目标的话查找目标所在地图在当前地图的入口
		else if (!bHasObjectInCurrentInstance)
		{
			// find the entrance of instance
			abase::vector<CECGame::OBJECT_COORD> instCoord;
			int iCount2 = g_pGame->GetObjectCoord(objCoord.strMap, instCoord);
			for (int i = 0; i < iCount2; ++i)
			{
				if (instCoord[i].strMap == strCurMap)
				{
					TargetCoord.push_back(instCoord[i]);
					
					//  Check if this is the nearest target
					float tempDist = a3d_Magnitude(instCoord[i].vPos - GetPos());
					if(tempDist < fMinDist)
					{
						fMinDist = tempDist;
						bInTable = true;
						vDestPos = instCoord[i].vPos;
					}
				}
			}
		} 
	}

	return vDestPos;
}

//  Update booth pack slot number
void CECHostPlayer::UpdateBoothPack(int iSellSize, int iBuySize)
{
	ASSERT(iSellSize > 0 && iBuySize >0);

	if(m_pBoothSPack->GetSize() != iSellSize)
		m_pBoothSPack->Resize(iSellSize);
	
	if(m_pBoothBPack->GetSize() != iBuySize)
		m_pBoothBPack->Resize(iBuySize);


}
//  Get Yinpiao amount
int CECHostPlayer::GetYinpiaoTotalAmount()
{
	//  获取包裹中银票的总数量，银票ID:21652
	return m_pPack->GetItemTotalNum(21652);
}

//  Get fashion by id & color
CECIvtrFashion* CECHostPlayer::GetFashionByID(int tid, int color, int& index)
{
	CECIvtrItem* pItem = NULL;
	int i;
	for(i=0;i<m_pTrashBoxPack3->GetSize();i++)
	{
		pItem = m_pTrashBoxPack3->GetItem(i);
		if(pItem && pItem->GetTemplateID() == tid)
		{
			if(((CECIvtrFashion*)pItem)->GetWordColor() == (unsigned short)color)
			{
				index = i;
				return (CECIvtrFashion*)pItem;
			}
		}
		pItem = NULL;
	}

	index = 255;
	if(!pItem)
	{
		//  Check current 6 fashion items
		for(i=0;i<FASHION_NUM;i++)
		{
			if(i < 4)
				pItem = m_pEquipPack->GetItem(EQUIPIVTR_FASHION_BODY + i);
			else if (i == 4)
				pItem = m_pEquipPack->GetItem(EQUIPIVTR_FASHION_HEAD);
			else
				pItem = m_pEquipPack->GetItem(EQUIPIVTR_FASHION_WEAPON);

			if(pItem && pItem->GetTemplateID() == tid)
			{
				if(((CECIvtrFashion*)pItem)->GetWordColor() == (unsigned short)color)
					return (CECIvtrFashion*)pItem;
			}	
		}
	}

	return NULL;
}
// id: high byte is tid, low byte is color
CECIvtrFashion* CECHostPlayer::GetFashionByID(int id, int& index)
{
	int tid = ( ((unsigned int )id) >> 16);
	int color = id & 0x0000ffff;

	return GetFashionByID(tid, color, index);
}

//  Check auto fashion condition, return 0 means successful
int CECHostPlayer::CheckAutoFashionCondition()
{
	int ii = GetCoolTime(GP_CT_EQUIP_FASHION_ITEM);
	if( GetCoolTime(GP_CT_EQUIP_FASHION_ITEM) )
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
		return 4;
	}

	if(GetBoothState() == 2 || IsShapeChanged())
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CANNOT_AUTO_FASHION_WHEN_BOOTH);
		return 5;
	}

	int i;
	
	//	1. Check if fashion inventory is empty
	if(m_pTrashBoxPack3->GetEmptySlotNum() == m_pTrashBoxPack3->GetSize())
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FASHION_PACK_EMPTH);
		return 1;
	}

	//	2. Check if fashion shortcut is empty
	for(i=0; i<SIZE_FASHIONSCSET; i++)
	{
		if(m_aFashionSCSets[i] != 0)
			break;
	}

	if(i == SIZE_FASHIONSCSET)
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FASHION_SC_EMPTY);
		return 2;
	}

	//	3. Check if fashion inventory is full
	int iFashionEquipCnt = 0;
	for(i=EQUIPIVTR_FASHION_BODY; i<EQUIPIVTR_FASHION_BODY+4;i++)
	{
		if(m_aEquips[i] > 0)
			iFashionEquipCnt ++;
	}

	if(m_aEquips[EQUIPIVTR_FASHION_HEAD])
		iFashionEquipCnt ++;

	if(iFashionEquipCnt > m_pTrashBoxPack3->GetEmptySlotNum())
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FASHION_PACK_FULL);
		return 3;
	}	

	return 0;
}

//	Start auto fashion (time mode)
int CECHostPlayer::StartAutoFashion(int iTime)
{
	ASSERT(iTime > 0 && m_pTrashBoxPack3);

	int iRet = CheckAutoFashionCondition();

	if(iRet == 0)
	{
		m_bAutoFashion	= true;
		EquipFashionBySuitID(m_iCurFashionSuitID);
		m_AutoFashionCnt.SetPeriod(iTime*60000);
		m_AutoFashionCnt.Reset();
	}

	return iRet;
}
//  Set shortcuts for fashion
void CECHostPlayer::SetFashionSCSets(int index, CECIvtrFashion* pFashion)
{
	ASSERT(index >= 0 && index < GetMaxFashionSuitNum()*FASHION_NUM);
	if(!pFashion)
		m_aFashionSCSets[index] = 0;
	else
	{
		int tid = pFashion->GetTemplateID();
		int color = pFashion->GetWordColor();

		// high byte is tid, low byte is color
		m_aFashionSCSets[index] = color | (tid << 16);
	}
}
//  Get max fashion suit number
int CECHostPlayer::GetMaxFashionSuitNum()
{
	return m_pTrashBoxPack3->GetSize()/5;
}

//  Equip fashion by suit id
bool CECHostPlayer::EquipFashionBySuitID(int id)
{
	if(id < 0 || id >= GetMaxFashionSuitNum())
	{
		ASSERT(0);
		return false;
	}

	//	Check if current fashion suit is empty
	int i = 0;
	for(i=0; i< FASHION_NUM; i++)
	{
		if(m_aFashionSCSets[id*FASHION_NUM + i] != 0)
			break;
	}
	if(i == FASHION_NUM)
		return false;

	int j, iFashionItemIdx[FASHION_NUM], iEmptySlotIdx = 0;
	for(i=0;i<FASHION_NUM;i++)
	{
		if(m_aFashionSCSets[id*FASHION_NUM+i] == 0) // 快捷栏中无数据
		{
			// 若身上有数据，则在衣柜中找一个empty slot,换下
			if( (i==0 && m_aEquips[EQUIPIVTR_FASHION_HEAD]) ||
				(i==1 && m_aEquips[EQUIPIVTR_FASHION_BODY]) ||
				(i==2 && m_aEquips[EQUIPIVTR_FASHION_LEG]) ||
				(i==3 && m_aEquips[EQUIPIVTR_FASHION_WRIST]) ||
				(i==4 && m_aEquips[EQUIPIVTR_FASHION_FOOT]) ||
				(i==5 && m_aEquips[EQUIPIVTR_FASHION_WEAPON]))
			{
				for(j=iEmptySlotIdx;j<m_pTrashBoxPack3->GetSize();j++)
				{
					if(!m_pTrashBoxPack3->GetItem(j))
					{
						iFashionItemIdx[i] = j;
						iEmptySlotIdx = j+1;
						break;
					}
				}

				if(j==m_pTrashBoxPack3->GetSize())
				{
					ASSERT(0);
					return false;
				}
			}
			else
				iFashionItemIdx[i] = 255;

		}
		else	// 快捷栏中有数据
		{
			CECIvtrFashion* pFashion = GetFashionByID(m_aFashionSCSets[id*FASHION_NUM+i], iFashionItemIdx[i]);
		}
	}

	m_iCurFashionSuitID = id;

	if(m_bAutoFashion)
		m_AutoFashionCnt.Reset();

	g_pGame->GetGameSession()->c2s_CmdEquipFashionItem(iFashionItemIdx[1], iFashionItemIdx[2], 
		iFashionItemIdx[4], iFashionItemIdx[3], iFashionItemIdx[0], iFashionItemIdx[5]);
	return true;
}

void CECHostPlayer::ResetAutoSelMap()
{
	//	Rebuild selected table
	m_aTabSels.RemoveAll(false);
}
// for test
//	Auto select a attackable target
int CECHostPlayer::AutoSelectTarget()
{
	if (!IsSkeletonReady() || !CanDo(CANDO_CHANGESELECT))
		return 0;

	int i, idCurSel = (m_idSelTarget && m_idSelTarget != GetCharacterID()) ? m_idSelTarget : 0;

	if (!idCurSel || !IsSkeletonReady())	//	只有 IsSkeletonReady() 为 true 时 GetDistToHost() 才有效
	{
		//	Rebuild selected table
		m_aTabSels.RemoveAll(false);
	}
	else
	{
		for (i=0; i < m_aTabSels.GetSize(); i++)
		{
			float fDistToHost = 0.0f;
			CECObject* pObject = m_aTabSels[i];
			if (pObject->IsElsePlayer())
				fDistToHost = ((CECElsePlayer*)pObject)->GetDistToHost();
			else if (pObject->IsNPC())
				fDistToHost = ((CECNPC*)pObject)->GetDistToHost();

			if (fDistToHost > EC_TABSEL_DIST || !CanSafelySelectWith(fDistToHost))
			{
				m_aTabSels.RemoveAtQuickly(i);
				i--;
			}
		}
	}

	float fMinDist = 10000.0f;
	CECObject* pCand = NULL;
	int idCandidate = 0;

	const EC_VIDEO_SETTING& vs = g_pGame->GetConfigs()->GetVideoSettings();
	int mode  = vs.cTabSelType;

	if(mode<TSL_NORMAL || mode>=TSL_NUM)
	{
		ASSERT(FALSE);
		a_LogOutput(1, "CECHostPlayer::AutoSelectTarget(), error value of tab select mode!");
		return idCurSel;
	}

	if (mode == TSL_NORMAL || mode == TSL_MONSTER)
	{
		APtrArray<CECElsePlayer*> aCandPlayers;

		//	Get player candidates
		m_pPlayerMan->TabSelectCandidates(idCurSel, aCandPlayers);

		for (i=0; i < aCandPlayers.GetSize(); i++)
		{
			CECElsePlayer* pPlayer = aCandPlayers[i];

			//	Check whether this player is in selected array
			int iIndex = m_aTabSels.Find(pPlayer);
			if (iIndex >= 0)
				continue;	//	This player has been in selected array

			//	Record the nearest one as candidate
			float fDist = pPlayer->GetDistToHost();
			if (fDist < fMinDist)
			{
				fMinDist	= fDist;
				pCand		= pPlayer;
				idCandidate	= pPlayer->GetCharacterID();
			}
		}
	}
	if (mode == TSL_NORMAL || mode == TSL_PLAYER)
	{
		APtrArray<CECNPC*> aCandNPCs;
		//	Get npc candidates
		g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->TabSelectCandidates(idCurSel, aCandNPCs);

		for (i=0; i < aCandNPCs.GetSize(); i++)
		{
			CECNPC* pNPC = aCandNPCs[i];

			//	Check whether this npc is in selected array
			int iIndex = m_aTabSels.Find(pNPC);
			if (iIndex >= 0)
				continue;	//	This npc has been in selected array

			//	Record the nearest one as candidate
			float fDist = pNPC->GetDistToHost();
			if (fDist < fMinDist)
			{
				fMinDist	= fDist;
				pCand		= pNPC;
				idCandidate	= pNPC->GetNPCID();
			}
		}
	}		

	const int iMaxCand = 9;
	int idNewSel = 0;

	if (pCand && idCandidate)
	{
		if (m_aTabSels.GetSize() >= iMaxCand)
		{
			m_aTabSels.RemoveAt(m_aTabSels.GetSize()-1);
			m_aTabSels.InsertAt(0, pCand);
		}
		else
			m_aTabSels.Add(pCand);

		idNewSel = idCandidate;
	}
	else	//	No proper candidate was found
	{
		if (!m_aTabSels.GetSize())
		{
			idNewSel = idCurSel;
		}
		else	//	Try to select one which has been in selected array
		{
			int iIndex = -1;
			for (i=0; i < m_aTabSels.GetSize(); i++)
			{
				if (CECObject::GetObjectID(m_aTabSels[i]) == idCurSel)
				{
					iIndex = i;
					break;
				}
			}

			iIndex = (iIndex + 1) % m_aTabSels.GetSize();
			idNewSel = CECObject::GetObjectID(m_aTabSels[iIndex]);
		}
	}

	if (idNewSel)
	{
		if (idNewSel != idCurSel)
		{
			m_idUCSelTarget = idNewSel;
			SelectTarget(idNewSel);
		}
	}

	return idNewSel;
}

//	Remove player from tab-selected array
void CECHostPlayer::RemoveObjectFromTabSels(CECObject* pObject)
{
	for (int i=0; i < m_aTabSels.GetSize(); i++)
	{
		if (m_aTabSels[i] == pObject)
		{
			m_aTabSels.RemoveAtQuickly(i);
			break;
		}
	}
}

int   CECHostPlayer::MultiExp_GetEnhanceTime()
{
	return (MultiExp_GetState() == MES_ENHANCE) ? MultiExp_GetCurStateTimeLeft() : m_multiExpEnhanceTime;
}

int   CECHostPlayer::MultiExp_GetBufferTime()
{
	return (MultiExp_GetState() == MES_BUFFER) ? MultiExp_GetCurStateTimeLeft() : m_multiExpBufferTime;
}

int   CECHostPlayer::MultiExp_GetImpairTime()
{
	return (MultiExp_GetState() == MES_IMPAIR) ? MultiExp_GetCurStateTimeLeft() : m_multiExpImpairTime;
}

int   CECHostPlayer::MultiExp_GetCurStateTimeLeft()
{
	time_t curTime = time(NULL);
	return (m_multiExpExpireMoment >= curTime) ? (m_multiExpExpireMoment - curTime) : 0;
}

bool CECHostPlayer::MultiExp_StateEmpty(){
	return MES_NORMAL == MultiExp_GetState() && MultiExp_GetEnhanceTime() <= 0;
}

bool CECHostPlayer::MultiExp_EnhanceStateEmpty(){
	return MES_ENHANCE != MultiExp_GetState() && MultiExp_GetEnhanceTime() <= 0;
}

bool CECHostPlayer::MultiExp_EnhanceStateOnly(){
	return MultiExp_GetState() != MES_BUFFER && MultiExp_GetBufferTime() <= 0
		&& MultiExp_GetState() != MES_IMPAIR && MultiExp_GetImpairTime() <= 0
		&& !MultiExp_StateEmpty();	//	其它时间均为0，且状态不为空，则必为只剩下聚灵时间
}

bool CECHostPlayer::MultiExp_Inited(){
	return MES_UNINITED != MultiExp_GetState();
}

bool CECHostPlayer::MultiExp_CanChooseNow()
{
	bool result(false);

	while (true)
	{
		// 若当前在聚灵状态，则不允许选择其它套餐
		if (MultiExp_GetState() == MES_ENHANCE)
			break;

		// 每天只能选择一次套餐
		int serverTime = g_pGame->GetServerGMTTime();
		if (serverTime/(24*3600) == m_multiExpLastTimeStamp/(24*3600))
			break;

		result = true;
		break;
	}

	return result;
}

bool CECHostPlayer::IsOmitBlocking(int roleid)
{
	CECFactionMan* pFacMan = g_pGame->GetFactionMan();
	if(pFacMan && pFacMan->GetMember(roleid))
	{
		return true;
	}

	CECTeam *pTeam = GetTeam();
	if(pTeam && pTeam->GetMemberByID(roleid))
	{
		return true;
	}

	CECFriendMan *pFriMan = GetFriendMan();
	if(pFriMan && pFriMan->GetFriendByID(roleid))
	{
		return true;
	}

	return false;
}

bool CECHostPlayer::IsPassiveMove()
{
	return m_pWorkMan->IsPassiveMoving();
}

int CECHostPlayer::IsCongregating()
{
	int iType = 0;	
	if (CECHPWorkCongregate* pWork = dynamic_cast<CECHPWorkCongregate*>(m_pWorkMan->GetRunningWork(CECHPWork::WORK_CONGREGATE))){
		iType = pWork->GetConType() + 1;
	}	
	return iType;
}

CECCounter CECHostPlayer::GetCongregateCnt()
{
	if (CECHPWorkCongregate* pWork = dynamic_cast<CECHPWorkCongregate*>(m_pWorkMan->GetRunningWork(CECHPWork::WORK_CONGREGATE)))
	{
		return pWork->GetCounter();
	}
	else
	{
		return CECCounter();
	}
}

bool CECHostPlayer::OnPetSays(int tid, int nid, int type)
{
	bool bSaid(false);
	
	CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPCFromAll(nid);
	if (pNPC && pNPC->GetMasterID() == GetCharacterID())
	{
		const ACHAR * szWords = m_pPetWords->GetWords(tid, (CECPetWords::TYPE_WORDS)type);
		if (szWords)
		{
			pNPC->SetLastSaidWords(szWords, 5000);
			bSaid = true;
		}
	}

	return bSaid;
}

bool CECHostPlayer::SelectTarget(int idTarget)
{
	bool bRet(false);
	if (CanDo(CANDO_CHANGESELECT) && CanSelectTarget(idTarget))
	{
		bRet = true;
		if (idTarget == 0)
			g_pGame->GetGameSession()->c2s_CmdUnselect();
		else
		{
			g_pGame->GetGameSession()->c2s_CmdSelectTarget(idTarget);		
		}
	}
	return bRet;
}

bool CECHostPlayer::CanAcceptBind()const
{
	return !m_playerLimits.test(PLAYER_LIMIT_NOBIND) && CanBindBuddy();
}
bool CECHostPlayer::PlayerLimitTest(int flag) 
{
	return !m_playerLimits.test(flag);
}

bool CECHostPlayer::CanSelectTarget(int idTarget)
{
	if(idTarget == 0 || idTarget == this->GetCharacterID()){
		// 0 means unselect
		return true;
	}
	CECObject* pTarget = NULL;
	if (ISPLAYERID(idTarget)){
		if (CECElsePlayer *pElsePlayer = dynamic_cast<CECElsePlayer *>(g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(idTarget))){
			if (CanSafelySelect(pElsePlayer)){ 
				pTarget = pElsePlayer;
			}
		}
	}else if (ISNPCID(idTarget)){
		if (CECNPC *pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(idTarget)){
			if (CanSafelySelect(pNPC) && !pNPC->IsDead()){
				pTarget = pNPC;
			}
		}
	}
	return pTarget ? pTarget->IsSelectable() : false;
}

float CECHostPlayer::SafelySelectDistance(){
	//	服务器对 SelectTarget 有额外距离限制，三维距离 150.0、水平距离 125.0 以上的，都会无法选中
	//	基于以上原因，客户端选择对象、或者对已经选择的对象，都确保其在此限制范围内，即选择时使用较小距离检测
	return 100.0f;
}

bool CECHostPlayer::CanSafelySelectWith(float fDistanceToHostPlayer){
	return fDistanceToHostPlayer <= SafelySelectDistance();
}

bool CECHostPlayer::CanSafelySelect(CECElsePlayer *pElsePlayer){
	//	IsSkeletonReady() 为 true 时, GetDistToHost() 才为有效数据
	//	!IsSkeletonReady() 时，也允许使用，目的是避免未考虑到的意外情况
	//	下同
	return pElsePlayer && (!IsSkeletonReady() || CanSafelySelectWith(pElsePlayer->GetDistToHost()));
}

bool CECHostPlayer::CanSafelySelect(CECNPC *pNPC){
	return pNPC && (!IsSkeletonReady() || CanSafelySelectWith(pNPC->GetDistToHost()));
}

const CECHostPlayer::FORCE_INFO* CECHostPlayer::GetForceInfo(int force_id)
{
	if(force_id > 0)
	{
		for(int i=0;i<m_aForceInfo.GetSize();i++)
		{
			const FORCE_INFO* pInfo = m_aForceInfo.ElementAt(i);
			if(pInfo->force_id == force_id)
			{
				return pInfo;
			}
		}
	}

	return NULL;
}

void CECHostPlayer::SetForceInfo(int force_id, const CECHostPlayer::FORCE_INFO* pInfo)
{
	ASSERT(!pInfo || pInfo->force_id == force_id);

	if(force_id > 0)
	{
		for(int i=0;i<m_aForceInfo.GetSize();i++)
		{
			FORCE_INFO& info = m_aForceInfo[i];
			if(info.force_id == force_id)
			{
				// delete the specific force info
				if(pInfo == NULL)
				{
					m_aForceInfo.RemoveAt(i);
				}
				else
				{
					info = *pInfo;
				}

				// updated only
				return;
			}
		}
	}
	
	// add new info
	if(pInfo)
	{
		FORCE_INFO info = *pInfo;
		m_aForceInfo.Add(info);
	}
}


bool	 CECHostPlayer::IsFactionMember(int idTargetFaction)
{
	//	当前属于某帮派，且对方属于同一帮派时返回 true
	return GetFactionID() != 0 && GetFactionID() == idTargetFaction;
}

bool	 CECHostPlayer::IsFactionAllianceMember(int idTargetFaction)
{
	//	当前属于某帮派，对方属于同一帮派、或者属于同盟帮派
	return GetFactionID() != 0
		&& idTargetFaction != 0
		&& (GetFactionID() == idTargetFaction || g_pGame->GetFactionMan()->IsFactionAlliance(idTargetFaction));
}

bool	CECHostPlayer::IsTooNear(const A3DVECTOR3 &vTarget, float &fNearDist)
{
	//	根据空间情况，计算相近 3D 空间上应保持的较小距离，避免移动很近甚至重合的情况
	//	返回 true 表明当前距离小于计算出的最小距离

	const A3DVECTOR3 &vPos = GetPos();
	A3DVECTOR3 vMoveDir = vTarget - vPos;
	float fDist = vMoveDir.Magnitude();

	float fTestDist(0.0f);
	const float fTestDistH = 0.1f;
	const float fMoveDistH = 0.01f;	//	保证 fTestDistH > fMoveDistH，使检测更有效

	float fDeltaXZ = vMoveDir.MagnitudeH();
	if (fDeltaXZ > 0.001f && (fabs(vMoveDir.y)/fDeltaXZ) <= 50)
	{
		//	tangent 值不太大

		//	以 fTestDistH 为水平距离要求，计算空间上应保持的距离
		fTestDist = fTestDistH * fDist / fDeltaXZ;

		//	以 fMoveDistH 为水平距离要求，计算空间上应移近的距离
		fNearDist = fMoveDistH * fDist / fDeltaXZ;
	}
	else
	{
		//	tangent 值很大，或 vPos 与 vTarget 重合的情况
		fTestDist = 0.5f;
		fNearDist = fDist > 0.01f ? 0.01f : 0.0f;
	}

	return (fDist <= fTestDist);
}

bool	CECHostPlayer::IsTooNear(const A3DVECTOR3 &vMovePos)
{
	//	判断 HostPlayer 当前位置离要移往的位置 vMovePos 是否太近
	//	飞行或在水中时，移动位置在竖直方向易受凸包等影响，故修改判断规则
	//	陆上则直接判断

	bool bTooNear(false);

	while (true)
	{
		const A3DVECTOR3 &vPos = GetPos();
		A3DVECTOR3 vDelta = vMovePos - vPos;
		float fMoveDistH = vDelta.MagnitudeH();
		float fMoveDistV = (float)fabs(vDelta.y);

		if (IsFlying())
		{
			//	空中（竖直方向允许移动较大距离，具体数值参考 ClampAboveGround）
			bTooNear = (fMoveDistH <= 0.5f && fMoveDistV <= (m_MoveConst.fMinAirHei + 0.1f));
			break;
		}

		CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
		float fTerrainHeight =  pWorld->GetTerrainHeight(vMovePos);
		float fWaterHeight = pWorld->GetWaterHeight(vMovePos);
		const A3DVECTOR3 &vExts = GetPlayerAABB().Extents;
		if (fWaterHeight>fTerrainHeight && vMovePos.y+vExts.y<fWaterHeight-m_MoveConst.fWaterSurf)
		{
			//	水底（竖直方向允许移动较大距离，具体数值参考 ClampAboveGround）
			bTooNear = (fMoveDistH <= 0.5f && fMoveDistV <= (m_MoveConst.fMinWaterHei + 0.1f));
			break;
		}

		//	陆上
		float fDummy(0.0f);
		bTooNear = IsTooNear(vMovePos, fDummy);
		break;
	}

	return bTooNear;
}

bool CECHostPlayer::IsPosCollideFree(const A3DVECTOR3 &vTargetPos)
{
	bool bAvailable(false);

	while (true)
	{
		float terrianHeight = g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(vTargetPos);
		if (terrianHeight > vTargetPos.y + 1E-4f )
			break;

		A3DVECTOR3 vExt = m_CDRInfo.vExtent;
		env_trace_t  trcInfo;
		trcInfo.dwCheckFlag = CDR_BRUSH;
		trcInfo.vExt = vExt;
		trcInfo.vStart = vTargetPos + g_vAxisY * vExt.y;
		trcInfo.vDelta.Clear();
		if (CollideWithEnv(&trcInfo))
			break;

		bAvailable = true;
		break;
	}

	return bAvailable;
}

float CECHostPlayer::CalcAABBOnCollidePos(const A3DAABB &aabbTarget)
{
	//	根据 HostPlayer 与某对象（如NPC） AABB，计算 AABB 冲突临界状态下两者距离（忽略地形起伏导致 AABB 相对位置的变化）
	//	此函数可用于计算带凸包的 NPC 附近不冲突的位置
	//	若 HostPlayer AABB 与对象 AABB 当前位置实际已经冲突，则函数返回值不具参考价值
	//	返回值应适当增加一个增量，即离冲突位置稍远些，以处理地形起伏导致 AABB 相对位置的变化
	//	返回值转换成位置时，还应处理凸包碰撞

	float fAABBCollideDist(0.0f);

	const A3DAABB &aabbHost = GetPlayerAABB();

	A3DVECTOR3 vHostHeight = A3DVECTOR3(0.0f, aabbHost.Extents.y, 0.0f);
	A3DVECTOR3 vHostRoot = aabbHost.Center - vHostHeight;
	A3DVECTOR3 vTargetHeight = A3DVECTOR3(0.0f, aabbTarget.Extents.y, 0.0f);
	A3DVECTOR3 vTargetRoot = aabbTarget.Center - vTargetHeight;

	A3DVECTOR3 vRootDir = vTargetRoot - vHostRoot;
	float fRootDist = vRootDir.Normalize();
	float fMinDist = a_Min(aabbHost.Extents.x, aabbHost.Extents.y, aabbHost.Extents.z);	//	碰撞距离必然大于包围盒最小边长
	fMinDist = a_Max(fMinDist, 0.01f);		//	限制最小值，增强程序鲁棒性
	if (fRootDist >= fMinDist)
	{
		A3DVECTOR3 vCenterDelta0 = aabbHost.Center - aabbTarget.Center;
		const A3DVECTOR3 &vTargetExt = aabbTarget.Extents;
		const A3DVECTOR3 &vHostExt = aabbHost.Extents;
		A3DVECTOR3 vSumExt = vTargetExt + vHostExt;

		//	两 AABB 临界碰撞时，aabbHost.Center 对于 aabbTarget.Center 的相对位置为 vCenterDelta0 + t * vRootDir
		//	此向量在各坐标轴上投影长度，不小于向量 (vTargetExt + vHostExt) 在各坐标轴上长度
		//	据此求得 vRootDir 方向上最先导致碰撞的坐标轴，并求得 t (小于 fRootDist)，fRootDist-t 即为返回值

		A3DVECTOR3 t(0.0f, 0.0f, 0.0f);
		float fZero = 0.001f;		//	判断为零
		if (fabs(vRootDir.x) >= fZero)
		{
			float t1 = (vSumExt.x - vCenterDelta0.x) / vRootDir.x;
			float t2 = (vSumExt.x + vCenterDelta0.x) / -vRootDir.x;
			if (t1 >= fZero && t1 < fRootDist)	t.x = t1;
			if (t2 >= fZero && t2 < fRootDist && t2 > t.x)	t.x = t2;
		}
		if (fabs(vRootDir.y) >= fZero)
		{
			float t1 = (vSumExt.y - vCenterDelta0.y) / vRootDir.y;
			float t2 = (vSumExt.y + vCenterDelta0.y) / -vRootDir.y;
			if (t1 >= fZero && t1 < fRootDist)	t.y = t1;
			if (t2 >= fZero && t2 < fRootDist && t2 > t.y)	t.y = t2;
		}
		if (fabs(vRootDir.z) >= fZero)
		{
			float t1 = (vSumExt.z - vCenterDelta0.z) / vRootDir.z;
			float t2 = (vSumExt.z + vCenterDelta0.z) / -vRootDir.z;
			if (t1 >= fZero && t1 < fRootDist)	t.z = t1;
			if (t2 >= fZero && t2 < fRootDist && t2 > t.z)	t.z = t2;
		}

		float fHostMove(0.0f);
		if (t.x > 0.0f)	fHostMove = t.x;
		if (t.y > 0.0f && t.y > fHostMove) fHostMove = t.y;
		if (t.z > 0.0f && t.z > fHostMove) fHostMove = t.z;

		if (fHostMove > 0.0f)
			fAABBCollideDist = fRootDist - fHostMove;
	}

	return fAABBCollideDist;
}

bool CECHostPlayer::CalcCollideFreePos(const A3DAABB &aabbTarget, A3DVECTOR3 &vPos)
{
	//	根据当前 HostPlayer 位置与给定目标的碰撞包围盒 aabbTarget，计算目标附近(与地形和凸包)无冲突的位置
	//	vPos 存放无冲突的位置，可用于 SetPos

	bool bFound(false);

	const A3DAABB &aabbHost = GetPlayerAABB();

	A3DVECTOR3 vHostHeight = A3DVECTOR3(0.0f, aabbHost.Extents.y, 0.0f);
	A3DVECTOR3 vHostRoot = aabbHost.Center - vHostHeight;
	A3DVECTOR3 vTargetHeight = A3DVECTOR3(0.0f, aabbTarget.Extents.y, 0.0f);
	A3DVECTOR3 vTargetRoot = aabbTarget.Center - vTargetHeight;

	A3DVECTOR3 vRootDir = vTargetRoot - vHostRoot;
	float fRootDist = vRootDir.Normalize();

	float fAABBCollideDist = CalcAABBOnCollidePos(aabbTarget);

	if (fAABBCollideDist > 0.0f)
	{
		float fDeltaTests[] = {0.001f, 0.1f};
		for (int i = 0; i < sizeof(fDeltaTests)/sizeof(fDeltaTests[0]); ++ i)
		{
			//	考虑到地形起伏等因素，与实际冲突位置拉开一定距离并逐个测试，以增大成功返回几率
			float fDelta= fDeltaTests[i];
			 if (fRootDist > fAABBCollideDist + fDelta)
			 {
				 //	返回的无冲突距离指 AABB Root 的距离，转换为 SetPos 需要值
				 float fTestMoveDist = fRootDist - (fAABBCollideDist + fDelta);
				 A3DVECTOR3 vTestPos = vHostRoot + vRootDir * fTestMoveDist;

				 // 根据地形和凸包进行修正、并测试可用性
				 vTestPos.y = ClampAboveGround(vTestPos);
				 if (IsPosCollideFree(vTestPos))
				 {
					 //	和凸包无碰撞，直接可用
					 bFound = true;
					 vPos = vTestPos;
				 }
				 else
				 {
					 //	和凸包有碰撞，尝试在竖直方向上查找
					 A3DVECTOR3 vTestPos2;
					 if (CalcVerticalCollideFreePos(vTestPos, vTestPos2))
					 {
						 bFound = true;
						 vPos = vTestPos2;
					 }
				 }
				 if (bFound)	break;
			 }
		}
	}

	return bFound;
}

bool CECHostPlayer::CalcBrushOnCollidePos(const A3DVECTOR3 &vTestPos, const A3DVECTOR3 &vDelta, const A3DVECTOR3 &vExtents, A3DVECTOR3 &vPos, bool &bNoCollide)
{
	//	返回 true：vPos 为无碰撞位置；若 bNoCollide 为 true，则 vPos 为 vTestPos；否则为计算出的位置
	//	返回 false：意味着 bStartSolid = true
	//	
	bool bFound(false);

	A3DVECTOR3 vCenterHeight = A3DVECTOR3(0.0f, vExtents.y, 0.0f);

	env_trace_t  trcInfo;
	trcInfo.dwCheckFlag = CDR_BRUSH;
	trcInfo.vExt = vExtents;
	trcInfo.vStart = vTestPos + vCenterHeight;
	trcInfo.vDelta = vDelta;
	if (CollideWithEnv(&trcInfo))
	{
		bNoCollide = false;
		if (!trcInfo.bStartSolid)
		{
			//	有碰撞，且 vTestPos 处无碰撞，计算碰撞位置
			vPos = trcInfo.vStart + trcInfo.fFraction * trcInfo.vDelta - vCenterHeight;
			bFound = true;
		}
	}
	else
	{
		//	无碰撞，直接取 vTestPos
		vPos = vTestPos;
		bNoCollide = true;
		bFound = true;
	}

	return bFound;
}

bool CECHostPlayer::CalcVerticalCollideFreePos(const A3DVECTOR3 &vRefPos, A3DVECTOR3 &vPos)
{
	//	计算 vRefPos 附近竖直方向上与凸包及地形无冲突的位置
	//	vRefPos 为 HostPlayer foot 位置

	bool bFound(false);

	while (true)
	{
		const A3DVECTOR3 &vHostExts = GetPlayerAABB().Extents;

		//	构建缩小版 HostPlayer ，向下寻找可用位置
		A3DVECTOR3 vShrinkExts = A3DVECTOR3(vHostExts.x, vHostExts.y*0.5f, vHostExts.z);
		A3DVECTOR3 vShrinkPos = vRefPos + A3DVECTOR3(0.0f, vHostExts.y*0.5f, 0.0f);

		A3DVECTOR3 vStartPos = vShrinkPos;
		A3DVECTOR3 vVerticalDelta = A3DVECTOR3(0.0f, -1.0f, 0.0f);
		float terrainHeight = g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(vStartPos);
		if (vStartPos.y < terrainHeight + 0.01f)
		{
			//	测试位置需处于地形以上
			break;
		}

		A3DVECTOR3 vCandidate;
		bool bNoCollide(false);
		if (!CalcBrushOnCollidePos(vStartPos, vVerticalDelta, vShrinkExts, vCandidate, bNoCollide))
		{
			//	vStartPos 处于凸包里
			break;
		}
		if (bNoCollide)
		{
			//	从 vStartPos 到 vRefPos 都无碰撞，可能是
			//	情形1：vRefPos 处头顶有凸包导致碰撞（从而调用此函数修正）
			//	情形2：或者 vRefPos 本身即无碰撞，误调用此函数做重复计算
			//	情形3：或其它未考虑情况

			//	尝试从底部向 vRefPos 使用原始大小 HostPlayer 查找无碰撞位置，以处理上述情形1
			vStartPos += vVerticalDelta;
			vVerticalDelta = vRefPos - vStartPos;
			if (!CalcBrushOnCollidePos(vStartPos, vVerticalDelta, vHostExts, vCandidate, bNoCollide))
			{
				//	vStartPos 位于凸包中，不太可能出现
				break;
			}
			if (bNoCollide)
			{
				//	可能是情形2，维持 vRefPos 不变
				vCandidate = vRefPos;
			}
			//	else 计算出了头顶碰撞的位置，使用 vCandidate 即可
		}
		//	else vCandidate 是缩小版 HostPlayer 无碰撞的位置，也是实际大小 HostPlayer 修正 vRefPos 的位置

		vCandidate.y = ClampAboveGround(vCandidate);
		if (!IsPosCollideFree(vCandidate))
		{
			//	位置冲突
			break;
		}

		vPos = vCandidate;
		bFound = true;
		break;
	}

	return bFound;
}

float CECHostPlayer::ClampAboveGround(const A3DVECTOR3 &vPos)
{
	//	将 vPos 限制到地面以上
	//	飞行状态或水底时，限制人物在水面以上，离水面/地面一定距离；若调整后 vPos 竖直往下有凸包，则还会限制到离凸包一定距离
	//	注意：vPos 被调整后，有可能处于凸包中；需检查返回高度调整值，以避免调整过大

	A3DVECTOR3 vTemp = vPos;

	while (true)
	{
		CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();

		//	Clamp 到地形以上
		float fTerrainHeight =  pWorld->GetTerrainHeight(vTemp);
		a_ClampFloor(vTemp.y, fTerrainHeight);

		const A3DVECTOR3 &vExts = GetPlayerAABB().Extents;

		if (IsFlying())
		{
			float fAbove = m_MoveConst.fMinAirHei;

			//	先保证离地面/水面一定高度
			float fWaterHeight = pWorld->GetWaterHeight(vTemp);
			float fSurface = a_Max(fTerrainHeight, fWaterHeight);
			a_ClampFloor(vTemp.y, fSurface+fAbove);

			//	再尝试离凸包一定高度
			A3DVECTOR3 vHitPos;
			bool bNoCollide(false);
			if (!CalcBrushOnCollidePos(vTemp, g_vAxisY*(fSurface-vTemp.y), GetPlayerAABB().Extents, vHitPos, bNoCollide) || bNoCollide)
			{
				//	无碰撞时直接跳出。而初始位置在凸包里时，无能为力
				break;
			}

			//	设置离凸包一定高度
			a_ClampFloor(vTemp.y, vHitPos.y+fAbove);

			break;
		}

		float fWaterHeight = pWorld->GetWaterHeight(vTemp);
		if (fWaterHeight>fTerrainHeight && vTemp.y+vExts.y<fWaterHeight-m_MoveConst.fWaterSurf)
		{
			float fAbove = m_MoveConst.fMinWaterHei;

			//	先保证离水底一定距离
			a_ClampFloor(vTemp.y, fTerrainHeight+fAbove);

			//	再尝试离凸包一定高度
			A3DVECTOR3 vHitPos;
			bool bNoCollide(false);
			if (!CalcBrushOnCollidePos(vTemp, g_vAxisY*(fTerrainHeight-vTemp.y), GetPlayerAABB().Extents, vHitPos, bNoCollide) || bNoCollide)
			{
				//	无碰撞时直接跳出。而初始位置在凸包里时，无能为力
				break;
			}

			//	设置离凸包一定高度
			a_ClampFloor(vTemp.y, vHitPos.y+fAbove);

			break;
		}

		//	地面上，无需再处理

		break;
	}

	return vTemp.y;
}

bool CECHostPlayer::HasCountryWarFlagCarrier()
{
	return IsInCountryWar()
		&& (GetBattleInfo().bFlagCarrier || GetBattleInfo().iCarrierID > 0);
}

A3DVECTOR3 CECHostPlayer::GetCountryWarFlagCarrierPos()
{
	if (HasCountryWarFlagCarrier())
	{
		const BATTLEINFO &bInfo = GetBattleInfo();
		if (!bInfo.IsFlagCarrier())
			return bInfo.posCarrier;
	}
	else ASSERT(false);
	return GetPos();
}

bool CECHostPlayer::HasCountryWarChannel()
{
	return IsInCountryWar();
}

bool CECHostPlayer::HasCountryChannel()
{
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	return (pWorld->IsCountryMap() || pWorld->IsCountryWarMap()) && GetCountry() > 0;
}

bool CECHostPlayer::GetDefenseRuneEnable(int type)const
{
	bool bEnable(false);
	if (type >= 0 && type < DEFENSE_RUNE_NUM)
		bEnable = m_bDefenseRuneEnable[type];
	return bEnable;
}

void CECHostPlayer::ExchangeYinpiao()
{
	if( m_iMoneyCnt < m_AutoYinpiao.low_money )
	{
		int iMoney = m_AutoYinpiao.low_money - m_iMoneyCnt;
		if( iMoney % 10000000 )
			iMoney = iMoney / 10000000 + 1;
		else
			iMoney = iMoney / 10000000;

		int iCount = a_Min(GetYinpiaoTotalAmount(), iMoney);
		if( iCount > 0 )
			::c2s_SendCmdExchangeYinpiao(true, iCount);
	}
	else if( m_iMoneyCnt > m_AutoYinpiao.high_money )
	{
		int iCount = m_iMoneyCnt - m_AutoYinpiao.high_money;
		if( iCount % 10000000 )
			iCount = iCount / 10000000 + 1;
		else
			iCount = iCount / 10000000;

		// 有空余格子时，买银票
		if( m_pPack->CanAddItem(21652, iCount, true) >= 0 )
			::c2s_SendCmdExchangeYinpiao(false, iCount);
	}
}

bool CECHostPlayer::IsDeferedUpdateSlice() const
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if( pGameUI )
	{
		PAUIDIALOG pDlg = pGameUI->GetDialog("Win_CountryMap");
		if( pDlg ) return pDlg->IsShow();
	}

	return false;
}

void CECHostPlayer::AddRelatedPlayer(int cid)
{
	m_RelatedPlayer[cid] = 60000;
}

bool CECHostPlayer::IsRelatedPlayer(int cid) const
{
	return m_RelatedPlayer.find(cid) != m_RelatedPlayer.end();
}

void CECHostPlayer::ModifyTitle(unsigned short id, int expire_time, bool bAdd)
{
	if (bAdd) {
		TITLE toModify(id, 0);
		TITLE_CONTAINER::iterator iter = std::find(m_Titles.begin(), m_Titles.end(), toModify);
		if (iter != m_Titles.end()){
			(*iter).expire_time = expire_time;
		} else m_Titles.push_back(TITLE(id, expire_time));
	} else if (!m_Titles.empty()){
		TITLE_CONTAINER temp;
		for (unsigned int i = 0; i < m_Titles.size(); ++i) {
			if (m_Titles[i].id != id) temp.push_back(m_Titles[i]);
		}
		m_Titles.swap(temp);
		if (id == m_TitleID) SetCurrentTitle(0);
		const TITLE_CONFIG* pTitle = GetTitleConfig(id);
		if (pTitle){
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			ACString strText;
			strText.Format(pGameUI->GetStringFromTable(10606), pTitle->name);
			if (pGameUI) pGameUI->AddChatMessage(strText, GP_CHAT_MISC);
		}
	}
}
void CECHostPlayer::InitTitle(int count, unsigned short* id)
{
	m_Titles.clear();
	m_bTitleDataReady = true;
	if (id == NULL) return;
	for (int i = 0; i < count; ++i) {
		m_Titles.push_back(TITLE(id[i], 0));
	}
}
void CECHostPlayer::InitTitlePlus(int count, void* id_time_pair)
{
	using namespace S2C;
	if (id_time_pair == NULL) return;
	cmd_query_title_re::_entry* pEntrys = (cmd_query_title_re::_entry*)id_time_pair;
	for (int j = 0; j < count; ++j){
		for (unsigned int i = 0; i < m_Titles.size(); ++i)
		{
			if (m_Titles[i].id == pEntrys[j].id) 
				m_Titles[i].expire_time = pEntrys[j].time;
		}
	}
}
bool CECHostPlayer::NeedMeridiansGuide()
{
	int minlevel = 0;;
	return IsLevelMeetMeridianReq(minlevel) && GetMeridiansProp().level == 0;
}
const PLAYER_REINCARNATION_CONFIG* CECHostPlayer::GetReincarnationConfig()
{
	static const PLAYER_REINCARNATION_CONFIG* pConfig = NULL;
	if (NULL == pConfig) {  
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);
		while(tid) {
			if(DataType == DT_PLAYER_REINCARNATION_CONFIG) {
				pConfig = (PLAYER_REINCARNATION_CONFIG *)pDataMan->get_data_ptr(tid,ID_SPACE_CONFIG, DataType);
				break;
			}
			tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
		}
	}
	return pConfig;
}
const PLAYER_REALM_CONFIG* CECHostPlayer::GetRealmConfig()
{
	static const PLAYER_REALM_CONFIG* pConfig = NULL;
	if (NULL == pConfig) {  
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);
		while(tid) {
			if(DataType == DT_PLAYER_REALM_CONFIG) {
				pConfig = (PLAYER_REALM_CONFIG *)pDataMan->get_data_ptr(tid,ID_SPACE_CONFIG, DataType);
				break;
			}
			tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
		}
	}
	return pConfig;
}
bool CECHostPlayer::IsRealmExpFull()
{
	if (m_RealmLevel == 100) return false;
	int layer= GetRealmLayer();
	int sub_level = GetRealmSubLevel();
	const PLAYER_REALM_CONFIG* pConfig = GetRealmConfig();
	int require_exp = layer == 0 ? 0 : pConfig->list[layer - 1].level[sub_level - 1].require_exp;
	return m_iRealmExp == require_exp;
}
// 进入/退出战车时，用战车技能更新角色的主动技能列表。
void CECHostPlayer::UpdatePositiveSkillByChariot(int old_chariot_id,int new_chariot_id)
{
	if(!m_pChariot) return;

	const CHARIOT_CONFIG* pOld = m_pChariot->GetChariot(old_chariot_id);
	if (pOld) // 删除列表里的战车技能
	{
		for (int j=0;j<sizeof(pOld->skill)/sizeof(pOld->skill[0]);j++)
		{
			for (int i=0;i<m_aPtSkills.GetSize();i++)
			{
				CECSkill* skill = m_aPtSkills.GetAt(i);
				if (skill && skill->GetSkillID() == pOld->skill[j])
				{
					delete skill;
					m_aPtSkills.RemoveAt(i);
					break;
				}	
			}
		}
	}
	
	const CHARIOT_CONFIG* pNew = m_pChariot->GetChariot(new_chariot_id);
	if (pNew) // 把战车技能加到技能列表里
	{
		for (int j=0;j<sizeof(pNew->skill)/sizeof(pNew->skill[0]);j++)
		{
			if(pNew->skill[j] <1) continue;

			bool bHas = false;
			for (int i=0;i<m_aPtSkills.GetSize();i++)
			{
				CECSkill* skill = m_aPtSkills.GetAt(i);
				if (skill && skill->GetSkillID() == pNew->skill[j])
				{
					ASSERT(FALSE);
					bHas = true;
				}	
			}
			if(!bHas)
			{
				CECSkill* pSkill = new CECSkill(pNew->skill[j], 1);
				m_aPtSkills.Add(pSkill);
			}
		}
	}

	m_pChariot->PrepareChariot(new_chariot_id);
}
void CECHostPlayer::SetCurSysModShortcutSetIndex(int index)
{
	m_iCurSysModIndex = index; 
	int m = NUM_SYSMODSETS;
	a_Clamp(m_iCurSysModIndex,0,m-1);
}

const int CECHostPlayer::GENERAL_CARD::max_collection = 512;
bool CECHostPlayer::GENERAL_CARD::HasCard(int show_order) const
{
	int bit_count = 8 * sizeof(COLLECTION_TYPE);
	unsigned int index = show_order / bit_count;
	unsigned int bit = show_order % bit_count;
	bool ret(false);
	if (index < card_collection.size()) 
		ret = (card_collection[index] & (1 << bit)) != 0;
	return ret;
}
void CECHostPlayer::GENERAL_CARD::AddCollection(int show_order)
{
	if (show_order > max_collection) return;
	int bit_count = 8 * sizeof(COLLECTION_TYPE);
	unsigned int index = show_order / bit_count;
	unsigned int bit = show_order % bit_count;
	unsigned int current_size = card_collection.size();
	for (unsigned int i = current_size; i <= index; ++i)
		card_collection.push_back(0);
	card_collection[index] |= 1 << bit;
}
void CECHostPlayer::GENERAL_CARD::Init(char* collection, unsigned int size)
{
	if (size > max_collection) return;
	card_collection.clear();
	for (unsigned int i = 0; i < size; ++i)
		card_collection.push_back(collection[i]);
}
int CECHostPlayer::GENERAL_CARD::GetObtainedCount() const
{
	int ret = 0;
	int bit_count = 8 * sizeof(COLLECTION_TYPE);
	for (unsigned int i = 0; i < card_collection.size(); ++i)
	{
		COLLECTION_TYPE c = card_collection[i];
		int j=0;		
		for (;j<bit_count;j++)
		{
			if(c & (1<<j)) ret++;
		}
	}
	return ret;
}
void CECHostPlayer::CARD_HOLDER::Init(S2C::cmd_refresh_monsterspirit_level::_entry* entrys, unsigned int size, int gain_times)
{
	memset(level, 0, sizeof(level));
	memset(exp, 0, sizeof(exp));
	for (unsigned int i = 0; i < size && i < max_holder; ++i) {
		level[i] = entrys[i].level;
		exp[i] = entrys[i].exp;
	}
	this->gain_times = gain_times;
}
float CECHostPlayer::CARD_HOLDER::GetProfessionRatio(int index)
{
	float ret(1.f);
	CECHostPlayer* pHostPlayer = g_pGame->GetGameRun()->GetHostPlayer();
	const CHARRACTER_CLASS_CONFIG* pConfig = pHostPlayer->GetCharacterClassConfig();
	a_Clamp(index, 0, ELEMENTDATA_NUM_POKER_TYPE);
	if (pConfig) ret = pConfig->spirit_adjust[index];
	return ret;
}
const int CECHostPlayer::CARD_HOLDER::hp[max_holder] =				{0,		0,		330,	330,	0, 0};
const int CECHostPlayer::CARD_HOLDER::damage[max_holder] =			{200,	200,	0,		0,		0, 0};
const int CECHostPlayer::CARD_HOLDER::magic_damage[max_holder] =	{200,	200,	0,		0,		0, 0};
const int CECHostPlayer::CARD_HOLDER::defense[max_holder] =			{0,		0,		700,	0,		0, 0};
const int CECHostPlayer::CARD_HOLDER::magic_defense[max_holder] =	{0,		0,		0,		700,	0, 0};
const int CECHostPlayer::CARD_HOLDER::vigour[max_holder] =			{0,		0,		0,		0,		50,50};

const CHARRACTER_CLASS_CONFIG* CECHostPlayer::GetCharacterClassConfig()
{
	static const CHARRACTER_CLASS_CONFIG* pConfig = NULL;
	if (NULL == pConfig || (int)pConfig->character_class_id != m_iProfession) {  
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);
		while(tid) {
			if(DataType == DT_CHARRACTER_CLASS_CONFIG) {
				const CHARRACTER_CLASS_CONFIG *pData = (const CHARRACTER_CLASS_CONFIG *)
					pDataMan->get_data_ptr(tid, ID_SPACE_CONFIG, DataType);
				
				if((int)pData->character_class_id == m_iProfession) {
					pConfig = pData;
					break;
				}
			}
			tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
		}
	}
	return pConfig;
}

bool CECHostPlayer::MoveCardAuto(CECIvtrItem* pItem, int where, int slot)
{
	if (pItem == NULL) return false;
	SlotSet::iterator iter = m_CardsToMove.find(slot);
	if ((pItem->GetClassID() == CECIvtrItem::ICID_GENERALCARD || pItem->GetClassID() == CECIvtrItem::ICID_GENERALCARD_DICE) 
		&& iter != m_CardsToMove.end() && where == IVTRTYPE_PACK) {
		int iDst = m_pGeneralCardPack->SearchEmpty();
		if (iDst != -1) {
			g_pGame->GetGameSession()->c2s_CmdExgTrashBoxIvtrItem(IVTRTYPE_GENERALCARD_BOX, iDst, slot);
			m_CardsToMove.erase(iter);
			return true;
		}
	}
	return false;
}

const PLAYER_SPIRIT_CONFIG* CECHostPlayer::GetPlayerSpiritConfig(int type)
{
	static const PLAYER_SPIRIT_CONFIG* pConfig[CARD_HOLDER::max_holder] = {0};
	const PLAYER_SPIRIT_CONFIG* pRet = NULL;
	if (type >= 0 && type < CARD_HOLDER::max_holder) {
		if (NULL == pConfig[type]) {
			elementdataman *pDataMan = g_pGame->GetElementDataMan();
			DATA_TYPE DataType;
			unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);
			while(tid) {
				if(DataType == DT_PLAYER_SPIRIT_CONFIG) {
					const PLAYER_SPIRIT_CONFIG *pData = (const PLAYER_SPIRIT_CONFIG *)
						pDataMan->get_data_ptr(tid, ID_SPACE_CONFIG, DataType);
					
					if(pData->type == type) {
						pConfig[type] = pData;
						break;
					}
				}
				tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
			}			
		}
		pRet = pConfig[type];
	}
	return pRet;
}
void CECHostPlayer::DoSkillStateAction()
{
	if(m_pPlayerModel && HasSkillStateForAction() && m_SkillIDForStateAction
		&& !m_pWorkMan->IsPlayingSkillAction()
		&& !m_pWorkMan->IsDead())
	{
		CECHPWorkSkillStateAction* pWork = (CECHPWorkSkillStateAction*)m_pWorkMan->CreateWork(CECHPWork::WORK_SKILLSTATEACT);
		pWork->SetSkill(m_SkillIDForStateAction);
		m_pWorkMan->StartWork_p1(pWork);
	}
	else if (HasSkillStateForAction()==0 && m_pWorkMan->IsPlayingSkillAction())
	{
		m_pWorkMan->FinishRunningWork(CECHPWork::WORK_SKILLSTATEACT);
		m_SkillIDForStateAction = 0;
	}
}
int CECHostPlayer::GetUsedLeaderShip()
{
	int ret = 0;
	CECIvtrGeneralCard* pCard = NULL;
	for(int i = 0; i < SIZE_GENERALCARD_EQUIPIVTR; i++) { 
		int index_card = EQUIPIVTR_GENERALCARD1 + i;
		pCard = dynamic_cast<CECIvtrGeneralCard*>(m_pEquipPack->GetItem(index_card));
		if (pCard) ret += pCard->GetEssence().require_leadership;
	}
	return ret;
}
bool CECHostPlayer::CanEquipCard(CECIvtrEquip* pEquip)
{
	CECIvtrGeneralCard* pCard = dynamic_cast<CECIvtrGeneralCard*>(pEquip);
	bool ret = false;
	if (pCard) {
		int index = EQUIPIVTR_GENERALCARD1 + pCard->GetEssence().type;
		if (m_pEquipPack->FindTheVerySameItem(pEquip)) 
			index = m_pEquipPack->FindItem(pEquip->GetTemplateID());
		CECIvtrGeneralCard* pEquipedCard = dynamic_cast<CECIvtrGeneralCard*>(m_pEquipPack->GetItem(index));
		int equipedcard_leadership(0);
		if (pEquipedCard)
			equipedcard_leadership = pEquipedCard->GetEssence().require_leadership;
		ret = (GetUsedLeaderShip() - equipedcard_leadership + pCard->GetEssence().require_leadership) 
			<= m_GeneralCard.leader_ship;
	}
	return ret;
}
void CECHostPlayer::OnJumpInstance()
{
	if(m_pActionSwitcher)
		m_pActionSwitcher->Clear();
}
void CECHostPlayer::UpdateActionSwitcher(DWORD dwDeltaTime)
{
	if(m_pActionSwitcher)
		m_pActionSwitcher->Tick(dwDeltaTime);
}

bool CECHostPlayer::HaveHealthStones()
{
	CECInventory *pPack = GetPack();
	
	static int items[4] = {36764, 36765, 36766, 36767};
	for (int i(0); i < sizeof(items)/sizeof(items[0]); ++ i) {
		if (pPack->FindItem(items[i]) >= 0)
			return true;
	}
	
	return false;
}

void CECHostPlayer::OnNaviageEvent(int task,int e)
{
	if (e == CECNavigateCtrl::EM_PREPARE)	
		CreateNavigatePlayer();

	if(m_pNavigatePlayer)
		m_pNavigatePlayer->OnNavigateEvent(task,e);

	if(e == CECNavigateCtrl::EM_END)
		ReleaseNavigatePlayer();
}
bool CECHostPlayer::IsInForceNavigateState()
{
	//return m_pNavigatePlayer ? m_pNavigatePlayer->IsNavigateMoving():false;
	return m_pNavigatePlayer ? m_pNavigatePlayer->IsReadyNavigate():false;
}
CECHostNavigatePlayer* CECHostPlayer::CreateNavigatePlayer()
{
	ReleaseNavigatePlayer();
	m_pNavigatePlayer = new CECHostNavigatePlayer(m_pPlayerMan,this);
	m_pNavigatePlayer->LoadConfig();
	return m_pNavigatePlayer;
}
void CECHostPlayer::ReleaseNavigatePlayer()
{
	A3DRELEASE(m_pNavigatePlayer);
}

void CECHostPlayer::SetPrayDistancePlus(float prayDistancePlus){
	m_fPrayDistancePlus = prayDistancePlus;
}

void CECHostPlayer::StartChangeFace(){
	m_bChangingFace = true;
}

void CECHostPlayer::StopChangeFace(){
	m_bChangingFace = false;
}
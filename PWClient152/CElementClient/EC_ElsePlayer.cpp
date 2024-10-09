/*
 * FILE: EC_ElsePlayer.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_ElsePlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_Model.h"
#include "EC_ShadowRender.h"
#include "EC_NPC.h"
#include "EC_Viewport.h"
#include "EC_GPDataType.h"
#include "EC_ManNPC.h"
#include "EC_GFXCaster.h"
#include "EC_TeamMan.h"
#include "EC_Team.h"
#include "EC_FixedMsg.h"
#include "EC_ManPlayer.h"
#include "EC_IvtrTypes.h"
#include "EC_IvtrWeapon.h"
#include "EC_IvtrItem.h"
#include "EC_Resource.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_ManAttacks.h"
#include "EC_HostMove.h"
#include "EC_Utility.h"
#include "EC_Skill.h"
#include "EC_PateText.h"
#include "EC_Configs.h"

#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_Sprite.h"
#include "EC_Goblin.h"
#include "EC_ManMatter.h"
#include "EC_Matter.h"
#include "EC_EPWork.h"

#include "PlayerBaseInfo_Re.hpp"
#include "GetCustomData_Re.hpp"

#include "A3DGFXExMan.h"
#include "elementdataman.h"
#include "EC_IvtrEquipMatter.h"

#include <A3DFont.h>
#include <A3DCamera.h>
#include <A3DCameraBase.h>
#include <A3DViewport.h>
#include <A3DLight.h>
#include <A3DTerrainWater.h>
#include <A3DSkinModel.h>
#include "A3DCombinedAction.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define MAX_LAGDIST		10.0f	//	Maximum lag distance

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

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECEPCacheData
//	
///////////////////////////////////////////////////////////////////////////

CECEPCacheData::CECEPCacheData()
{
	m_dwResFlags	= 0;
	m_cid			= 0;
	m_crc_c			= -1;
	m_crc_e			= -1;
	m_crcBooth		= 0;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECElsePlayer
//	
///////////////////////////////////////////////////////////////////////////

CECElsePlayer::CECElsePlayer(CECPlayerMan* pPlayerMan) : 
CECPlayer(pPlayerMan),
m_vServerPos(0.0f)
{
	m_iCID				= OCID_ELSEPLAYER;
	m_pEPWorkMan		= new CECEPWorkMan(this);
	m_bStopMove			= false;
	m_fMoveSpeed		= 0.0f;		
	m_iMoveMode			= MOVE_STAND;
	m_bCastShadow		= true;
	m_bCustomReady		= false;
	m_bEquipReady		= false;
	m_bBaseInfoReady	= false;
	m_pAppearGFX		= NULL;
	m_i64NewEqpMask		= 0;
	m_fDistToHost		= 0.0f;
	m_fDistToHostH		= 0.0f;
	m_iMMIndex			= -1;
	m_bJumping			= false;
	m_iInTeamState		= 0;
	m_fLastSpeed		= 0.0f;
	m_dwLastMoveTime	= 0;
	m_bLoadingModel		= false;
	m_bUseHintModel		= g_pGame->GetConfigs()->GetVideoSettings().bModelLimit;

	m_FightCnt.SetPeriod(15000);
	m_FightCnt.Reset(true);

	m_bShowCustomize	= true;

	memset(m_aNewEquips, 0, sizeof (m_aNewEquips));
}

CECElsePlayer::~CECElsePlayer()
{
}

//	Initialize object
bool CECElsePlayer::Init(const S2C::info_player_1& Info, int iAppearFlag, bool bReInit)
{
	using namespace S2C;

	if (!bReInit)
	{
		m_PlayerInfo.cid	= Info.cid;
		m_PlayerInfo.crc_e	= -1;
		m_PlayerInfo.crc_c	= -1;
	}

	m_iAppearFlag = iAppearFlag;

	//	Set player info.
	SetPlayerInfo(Info);

	//	Start a work do nothing will avoid some errors
	m_pEPWorkMan->StartWork(CECEPWorkMan::WT_NOTHING, new CECEPWorkStand(m_pEPWorkMan));

	//	Check custom data crc
	m_bCustomReady = (m_PlayerInfo.crc_c == Info.crc_c) ? true : false;
	//	Check equipment data crc
	m_bEquipReady = (m_PlayerInfo.crc_e == Info.crc_e) ? true : false;

	//	Query booth name
	if (m_iBoothState == 2)
		m_pPlayerMan->QueryPlayerBoothName(Info.cid);

	m_bShowCustomize = true;
	
	if (!bReInit)
	{
		m_cdr.fStepHeight	= m_MoveConst.fStepHei;
		m_cdr.vExts			= m_aabbServer.Extents;
		m_cdr.vVelocity.Clear();
	}

	if (!m_pAppearGFX && m_iAppearFlag == APPEAR_ENTERWORLD)
	{
		CECGFXCaster* pGFXCaster = g_pGame->GetGFXCaster();
		m_pAppearGFX = pGFXCaster->LoadGFXEx(res_GFXFile(RES_GFX_PLAYERAPPEAR));
		pGFXCaster->PlayGFXEx(m_pAppearGFX, GetPos(), GetDir(), 1.0f);
	}

	return true;
}

//	Load resources
bool CECElsePlayer::InitFromCache(const S2C::info_player_1& Info, CECEPCacheData* pCacheData,
								int iAppearFlag)
{
	if (!pCacheData)
	{
		ASSERT(pCacheData);
		return Init(Info, iAppearFlag, false);
	}

	using namespace S2C;

	m_PlayerInfo.cid = pCacheData->m_cid;

	m_iAppearFlag = iAppearFlag;

	//	Set player brief info
	if (!SetPlayerBriefInfo(pCacheData->m_iProfession, Info.GetGender(), pCacheData->m_strName))
	{
		a_LogOutput(1, "CECElsePlayer::InitFromCache, Failed to set brief info");
		return false;
	}

	//	Set player info.
	SetPlayerInfo(Info);

	if(pCacheData->m_aEquips[EQUIPIVTR_GOBLIN] > 0 && ShouldUseClothedModel())	// 简单模型不加载小精灵
	{
		int tid = pCacheData->m_aEquips[EQUIPIVTR_GOBLIN] & 0x0000ffff;
		int idModel = (pCacheData->m_aEquips[EQUIPIVTR_GOBLIN] >>16) & 0x000000ff;
		int iRefineLvl = (pCacheData->m_aEquips[EQUIPIVTR_GOBLIN] >>24) & 0x000000ff;

		ASSERT(idModel >= 1 && idModel <= 4);
		ASSERT(iRefineLvl >=0 && iRefineLvl <= 36);		

		if( !m_pGoblin )
		{
			m_pGoblin = new CECGoblin();
			if(!m_pGoblin->Init(tid, idModel,iRefineLvl,this))
			{
				ASSERT(0);
				m_pGoblin->Release();
				delete m_pGoblin;
				m_pGoblin = NULL;
			}
		}
	}

	//	Start a work do nothing will avoid some errors
	m_pEPWorkMan->StartWork(CECEPWorkMan::WT_NOTHING, new CECEPWorkStand(m_pEPWorkMan));

	//	Check custom data crc
	if ((pCacheData->m_dwResFlags & RESFG_CUSTOM) && pCacheData->m_crc_c == Info.crc_c)
	{
		m_PlayerInfo.crc_c	= Info.crc_c;
		m_bCustomReady		= true;
		ChangeCustomizeData(pCacheData->m_CustomizeData, false);
	}
	else
	{
		m_bCustomReady = false;
	}

	//	Check equipment data crc
	if ((pCacheData->m_dwResFlags & RESFG_SKIN) && pCacheData->m_crc_e == Info.crc_e)
	{
		m_bEquipReady = true;

		memcpy(m_aNewEquips, pCacheData->m_aEquips, sizeof (m_aNewEquips));
		m_i64NewEqpMask = EQUIP_MASK64_ALL;
		m_PlayerInfo.crc_e = Info.crc_e;

	#ifdef _DEBUG
		
		for (int i=0; i < SIZE_ALL_EQUIPIVTR; i++)
		{
			if (m_aNewEquips[i] < 0)
			{
				ASSERT(m_aNewEquips[i] >= 0);
			}
		}

	#endif	//	_DEBUG
	}
	else
	{
		m_bEquipReady = false;
	}

	if (m_iBoothState == 2)
	{
		//	Query booth name if necessary
		if (pCacheData->m_crcBooth == m_crcBooth && (pCacheData->m_dwResFlags & CECEPCacheData::RESFG_BOOTHNAME))
			SetBoothName(pCacheData->m_strBoothName);
		else
			m_pPlayerMan->QueryPlayerBoothName(Info.cid);
	}

	m_bShowCustomize = true;

	m_cdr.fStepHeight	= m_MoveConst.fStepHei;
	m_cdr.vExts			= m_aabbServer.Extents;
	m_cdr.vVelocity.Clear();

//	a_LogOutput(1, "else player (%x) load resource: %d %d %d", Info.cid, m_bBaseInfoReady, m_bCustomReady, m_bEquipReady);

	if (!m_pAppearGFX && m_iAppearFlag == APPEAR_ENTERWORLD)
	{
		CECGFXCaster* pGFXCaster = g_pGame->GetGFXCaster();
		m_pAppearGFX = pGFXCaster->LoadGFXEx(res_GFXFile(RES_GFX_PLAYERAPPEAR));
		pGFXCaster->PlayGFXEx(m_pAppearGFX, GetPos(), GetDir(), 1.0f);
	}

	return true;
}

//	Set player info
void CECElsePlayer::SetPlayerInfo(const S2C::info_player_1& Info)
{
	using namespace S2C;

	m_dwStates = Info.state;
	m_dwStates2 = Info.state2;
	m_BasicProps.iLevel2 = Info.level2;

	//	Update position and direction
	SetServerPos(Info.pos);
	SetPos(Info.pos);
	ChangeModelMoveDirAndUp(glb_DecompressDirH(Info.dir), g_vAxisY);

	//	Set environment
	m_iMoveEnv = MOVEENV_GROUND;
	if (Info.state & GP_STATE_FLY)
		m_iMoveEnv = MOVEENV_AIR;
	else
	{
		A3DVECTOR3 vGndPos, vNormal, vTestPos = Info.pos + g_vAxisY;
		VertRayTrace(vTestPos, vGndPos, vNormal);
		float fWaterHei = g_pGame->GetGameRun()->GetWorld()->GetWaterHeight(vTestPos);

		if (Info.pos.y < fWaterHei - m_MoveConst.fShoreDepth)
			m_iMoveEnv = MOVEENV_WATER;
		else if (Info.pos.y < fWaterHei && fWaterHei - vGndPos.y > m_MoveConst.fShoreDepth)
			m_iMoveEnv = MOVEENV_WATER;
	}

	//	Set in team state
	m_iInTeamState = 0;
	if (Info.state & GP_STATE_TEAMLEADER)
		m_iInTeamState = 2;
	else if (Info.state & GP_STATE_TEAM)
		m_iInTeamState = 1;

	//	Parse travel flag
	m_bFashionMode = false;
	if (Info.state & GP_STATE_FASHION)
		m_bFashionMode = true;
	
	m_dwGMFlags = (Info.state & GP_STATE_GMFLAG) ? GMF_IAMGM : 0;
	m_pvp.bEnable = (Info.state & GP_STATE_PVPFLAG) ? true : false;
	m_pvp.bInPVPCombat = (Info.state & GP_STATE_INPVPCOMBAT) ? true : false;
	m_pvp.iDuelState = (Info.state & GP_STATE_IN_DUEL) ? DUEL_ST_INDUEL : DUEL_ST_NONE;

	//	Parse following data
	BYTE* pData = (BYTE*) &Info + sizeof (info_player_1);
	if (Info.state & GP_STATE_ADV_MODE)
	{
		int d1 = *(int*)pData;
		pData += sizeof (int);
		int d2 = *(int*)pData;
		pData += sizeof (int);
		DecompAdvData(d1, d2);
	}

	//	Parse shape data
	int iShape = 0;
	if (Info.state & GP_STATE_SHAPE)
	{
		iShape = *pData;
		pData += sizeof (BYTE);
	}
	SetShape(iShape);

	//	Parse emote info
	int iEmote = ACT_STAND;
	if (Info.state & GP_STATE_EMOTE)
	{
		iEmote = *pData;
		pData += sizeof (BYTE);
	}

	//	Parse extend states
	DWORD aExtStates[OBJECT_EXT_STATE_COUNT] = {0};
	if (Info.state & GP_STATE_EXTEND_PROPERTY)
	{
		memcpy(aExtStates, pData, sizeof(aExtStates));
		pData += sizeof(aExtStates);
	}
	SetNewExtendStates(0, aExtStates, OBJECT_EXT_STATE_COUNT);

	//	Parse Faction ID
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
	m_iBoothState = 0;
	if (Info.state & GP_STATE_BOOTH)
	{
		m_crcBooth = *(BYTE*)pData;
		pData += sizeof (BYTE);
		m_iBoothState = 2;
	}

	//	Parse effect data
	m_aCurEffects.RemoveAll(false);
	if (Info.state & GP_STATE_EFFECT)
	{
		BYTE byNum = *pData;
		pData += sizeof (BYTE);
		short* aEffects = (short*)pData;
		pData += byNum * sizeof (short);

		for (BYTE n=0; n < byNum; n++)
			m_aCurEffects.UniquelyAdd((int)aEffects[n]);
	}

	//	Parse pariah data
	m_byPariahLvl = 0;
	if (Info.state & GP_STATE_PARIAH)
	{
		m_byPariahLvl = *pData;
		pData += sizeof (BYTE);
	}
	
	//	Parse mount data
	m_RidingPet.Reset();
	if (Info.state & GP_STATE_IN_MOUNT)
	{
		m_RidingPet.color = * (unsigned short *)pData;
		pData += sizeof (unsigned short);
		m_RidingPet.id = *(int*)pData;
		pData += sizeof (int);
	}

	//	Parse bind data
	m_idCandBuddy = 0;
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
	m_bFight = (Info.state2 & GP_STATE2_IN_BATTLE) != 0;
	
	unsigned char factionPVPMask = 0;
	if (Info.state2 & GP_STATE2_FACTION_PVP_MASK)
	{
		factionPVPMask = *(unsigned char*) pData;
		pData += sizeof(unsigned char);
	}
	SetFactionPVPMask(factionPVPMask);

	if (m_idSpouse && !g_pGame->GetGameRun()->GetPlayerName(m_idSpouse, false))
		g_pGame->GetGameSession()->GetPlayerBriefInfo(1, &m_idSpouse, 2);

	//	Parse battle data
	m_iBattleCamp = GP_BATTLE_CAMP_NONE;
	if (Info.state & GP_STATE_BC_INVADER)
		m_iBattleCamp = GP_BATTLE_CAMP_INVADER;
	else if (Info.state & GP_STATE_BC_DEFENDER)
		m_iBattleCamp = GP_BATTLE_CAMP_DEFENDER;

	SetSelectable( !(Info.state & GP_STATE_FORBIDBESELECTED) );
}

//	Release object
void CECElsePlayer::Release()
{
	delete m_pEPWorkMan;

	if (m_pAppearGFX)
	{
		g_pGame->GetGFXCaster()->ReleaseGFXEx(m_pAppearGFX);
		m_pAppearGFX = NULL;
	}

	ClearCastingSkill();
	m_bLoadingModel = false;

	CECPlayer::Release();
}

//	When all resources are ready, this function will be called
void CECElsePlayer::OnAllResourceReady()
{
	CECPlayer::OnAllResourceReady();

	//	Update extend states. Clear old states before to avoid adding
	//	same state effect twice.
	ClearShowExtendStates();
	ShowExtendStates(0, m_aExtStates, OBJECT_EXT_STATE_COUNT);
	
	//	Apply effects
	for (int i=0; i < m_aCurEffects.GetSize(); i++)
		ApplyEffect(m_aCurEffects[i], false);
}

bool CECElsePlayer::SetPetLoadResult(CECModel* pPetModel)
{
	if (CECPlayer::SetPetLoadResult(pPetModel))
	{
		m_cdr.vExts = m_aabbServer.Extents;
		return true;
	}

	return false;
}

//	Create player cache data
CECEPCacheData* CECElsePlayer::CreateCacheData()
{
	if (!m_bBaseInfoReady)
		return NULL;

	CECEPCacheData* pData = new CECEPCacheData;

	//	Copy basic info
	pData->m_cid			= m_PlayerInfo.cid;
	pData->m_iProfession	= m_iProfession;
	pData->m_strName		= GetName();	//	Force to rebuild a string

	if (IsCustomizeReady())
	{
		pData->m_crc_c	= m_PlayerInfo.crc_c;
		if( m_bIsChangingFace )
			memcpy(&pData->m_CustomizeData, &m_ChgPllCustomizeData, sizeof(m_ChgPllCustomizeData));
		else
			memcpy(&pData->m_CustomizeData, &m_CustomizeData, sizeof (m_CustomizeData));
		pData->m_dwResFlags |= CECPlayer::RESFG_CUSTOM;
	}

	if (IsEquipDataReady())
	{
	#ifdef _DEBUG
		
		for (int i=0; i < SIZE_ALL_EQUIPIVTR; i++)
		{
			if (m_aNewEquips[i] < 0)
			{
				ASSERT(m_aNewEquips[i] >= 0);
			}
		}

	#endif	//	_DEBUG

		//	Note: IsEquipDataReady() return true only means m_aNewEquips is
		//		ready, but not assure m_aEquips is ready !!
		pData->m_crc_e	= m_PlayerInfo.crc_e;
		memcpy(pData->m_aEquips, m_aNewEquips, sizeof (m_aNewEquips));
		pData->m_dwResFlags |= CECPlayer::RESFG_SKIN;
	}

	if (m_iBoothState == 2 && m_strBoothName.GetLength())
	{
		pData->m_crcBooth		= m_crcBooth;
		pData->m_strBoothName	= GetBoothName();	//	Force to rebuild a string
		pData->m_dwResFlags		|= CECEPCacheData::RESFG_BOOTHNAME;
	}

	return pData;
}

//	Tick routine
bool CECElsePlayer::Tick(DWORD dwDeltaTime)
{
	CECPlayer::Tick(dwDeltaTime);

	CECHostPlayer* pHost = m_pPlayerMan->GetHostPlayer();
	bool bSelected = pHost->GetSelectedTarget() == GetPlayerInfo().cid;

	if (!m_pPlayerModel && !m_bLoadingModel && IsBaseInfoReady() && IsCustomDataReady() && IsEquipDataReady())
	{
		if( (!m_bUseHintModel && m_iBoothState != 2) || bSelected)
		{
			// prepared equip info
			memcpy(m_aEquips, m_aNewEquips, sizeof(m_aEquips));
			if (ShouldUseModel())
				LoadPlayerSkeleton(false);
			m_bLoadingModel = true;
		}
	}

	// buddy transparency depends on the host player
	if(!pHost->GetBuddyState() || pHost->GetBuddyID() != GetCharacterID())
	{
		StartAdjustTransparency(-1.0f, GetTransparentLimit(), 500);
		SetTransparent(UpdateTransparency(dwDeltaTime));
	}

	m_pEPWorkMan->Tick(dwDeltaTime);

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

	if( bSelected )
	{
		if (m_pFaceModel)
			m_pFaceModel->Tick(dwDeltaTime);
		else
		{
			if( GetMajorModel() && !GetMajorModel()->IsSkinShown(SKIN_HEAD_INDEX) )
				GetMajorModel()->ShowSkin(SKIN_HEAD_INDEX, true);

			if( m_pModels[PLAYERMODEL_MAJOR] && (m_pPlayerModel != m_pModels[PLAYERMODEL_MAJOR] || m_iBoothState == 2) )
				m_pModels[PLAYERMODEL_MAJOR]->Tick(dwDeltaTime);
		}
	}
	else if (m_bShowCustomize && m_iBoothState != 2)
	{
		if (m_pFaceModel && GetMajorModel() && !GetMajorModel()->IsSkinShown(SKIN_HEAD_INDEX))
			m_pFaceModel->Tick(dwDeltaTime);
	}
	
	if (m_pAppearGFX)
	{
		if (m_pAppearGFX->GetState() == ST_STOP)
		{
			g_pGame->GetGFXCaster()->ReleaseGFXEx(m_pAppearGFX);
			m_pAppearGFX = NULL;
		}
		else
			m_pAppearGFX->SetParentTM(GetAbsoluteTM());
	}

	//	Calculate distance to host player
	if (pHost && pHost->IsSkeletonReady())
	{
		m_fDistToHost  = CalcDist(pHost->GetPos(), true);
		m_fDistToHostH = CalcDist(pHost->GetPos(), false);
	}
	
	UpdatePosRelatedGFX(dwDeltaTime);
	UpdateMonsterSpiritGfx(dwDeltaTime);

	return true;
}

//	Internal render routine
bool CECElsePlayer::InternalRender(CECViewport* pViewport, int iRenderFlag, bool bHighLight)
{
	m_PateContent.iVisible = 0;
	
	//	打坐使用特殊模型时、无高亮效果
	if (bHighLight && IsSitting() &&
		(GetRace() == RACE_GHOST || GetRace() == RACE_OBORO))
		bHighLight = false;

	A3DCameraBase* pCamera = pViewport->GetA3DCamera();
	m_fDistToCamera	= CalcDist(pCamera->GetPos(), true);

	if( iRenderFlag == RD_NORMAL )
	{
		if (m_fDistToCamera < 30.0f && GetBoothState() != 2)
			m_bCastShadow = true;
		else
			m_bCastShadow = false;
	}
	else
	{
		if (m_fDistToCamera > 30.0f )
			return true;

		A3DTerrainWater * pTerrainWater = g_pGame->GetGameRun()->GetWorld()->GetTerrainWater();
		if( iRenderFlag == RD_REFLECT && pTerrainWater->IsUnderWater(GetPlayerAABB().Maxs) )
			return true;
		else if( iRenderFlag == RD_REFRACT )
		{	
			bool bCamUnderWater = pTerrainWater->IsUnderWater(pCamera->GetPos());
			if( !bCamUnderWater && !pTerrainWater->IsUnderWater(GetPlayerAABB().Mins) )
				return true;
			else if( bCamUnderWater && pTerrainWater->IsUnderWater(GetPlayerAABB().Maxs) )
				return true;
		}

		m_bCastShadow = false;
	}
	if (InGhostState()){
		return true;
	}
	if (!IsAllResReady()){
		if (!ShouldUseModel()){
			if (iRenderFlag == RD_NORMAL && m_bRenderName){
				RenderName(pViewport, g_pGame->GetConfigs()->GetPlayerTextFlags());
			}
		}
		return true;
	}	
	if( !UpdateCurSkins() )
		return true;
	
	if( !IsClothesOn() && ShouldUseClothedModel() )	// 正常模型不允许不穿衣服出现
		return true;
	
	if (!m_bVisible)
	{
		//	Only render player head text
		if (iRenderFlag == RD_NORMAL && m_bRenderName)
			RenderName(pViewport, g_pGame->GetConfigs()->GetPlayerTextFlags());

		return true;
	}

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

			if (bHighLight)
				g_pGame->RenderHighLightModel(pViewport, pModel);
			else
				pModel->Render(pViewport->GetA3DViewport());
			
			if (m_bCastShadow && g_pGame->GetShadowRender())
			{
				CECPlayer * pPlayer = this;
				A3DAABB shadowAABB = GetShadowAABB();
				g_pGame->GetShadowRender()->AddShadower(shadowAABB.Center, shadowAABB, SHADOW_RECEIVER_TERRAIN, PlayerRenderForShadow, pPlayer);
			}
		}
	}

	if ( m_pFaceModel && 
		!IsShapeModelChanged() && !bSkipPlayerRender &&
		 GetMajorModel() && !GetMajorModel()->IsSkinShown(SKIN_HEAD_INDEX) )
	{
		m_pFaceModel->Render(pViewport, false, bHighLight);
	}

	if (iRenderFlag == RD_NORMAL)
	{
		//	Render player head text
		if (m_bRenderBar)
			RenderBars(pViewport);

		if (m_bRenderName){
			SetMouseOnBoothFlag(bHighLight);
			RenderName(pViewport, g_pGame->GetConfigs()->GetPlayerTextFlags());
		}

		//	Print player's precise position
		if (g_pGame->GetConfigs()->GetShowPosFlag())
		{
			ACHAR szMsg[100];
			A3DVECTOR3 vPos = GetPos();
			a_sprintf(szMsg, _AL("%.3f, %.3f, %.3f"), vPos.x, vPos.y, vPos.z);
			int y = pViewport->GetA3DViewport()->GetParam()->Height - 20;
			vPos += g_vAxisY * (m_aabb.Extents.y * 3.0f);
			g_pGame->GetA3DEngine()->GetSystemFont()->TextOut3D(szMsg, pViewport->GetA3DViewport(), vPos, 0, 0xffffffff);
		}
	}
	
	RenderGoblinOrSprite(pViewport);
	RenderPetCureGFX();
	RenderMultiObjectGFX();
	RenderMonsterSpiritGfx();

	return true;
}

//	Render routine when player use simple models
bool CECElsePlayer::RenderSimpleModels(bool bBooth, CECViewport* pViewport, 
									int iRenderFlag, bool bHighLight)
{
	m_PateContent.iVisible = 0;
	
	A3DCameraBase* pCamera = pViewport->GetA3DCamera();
	m_fDistToCamera	= CalcDist(pCamera->GetPos(), true);

	if (iRenderFlag == RD_REFLECT || iRenderFlag == RD_REFRACT)
		return true;

	if (InGhostState())
		return true;

	if (!m_bVisible)
	{
		//	Only render player head text
		if (iRenderFlag == RD_NORMAL && m_bRenderName)
			RenderName(pViewport, g_pGame->GetConfigs()->GetPlayerTextFlags());

		return true;
	}

	CECModel* pSimpleModel = NULL;

	if (bBooth)
		pSimpleModel = GetBoothModel();
   
	if (pSimpleModel)
	{
		//	Set light for model
		A3DSkinModel *pSimpleSkinModel = pSimpleModel->GetA3DSkinModel();
		if (pSimpleSkinModel)
		{
			A3DSkinModel::LIGHTINFO LightInfo;
			memset(&LightInfo, 0, sizeof (LightInfo));
			const A3DLIGHTPARAM& lp = g_pGame->GetDirLight()->GetLightparam();
			
			LightInfo.colAmbient	= g_pGame->GetA3DDevice()->GetAmbientValue();
			LightInfo.vLightDir		= lp.Direction;
			LightInfo.colDirDiff	= lp.Diffuse;
			LightInfo.colDirSpec	= lp.Specular;
			LightInfo.bPtLight		= false;
			
			pSimpleSkinModel->SetLightInfo(LightInfo);
		}
		if (pSimpleModel->GetComActCount()>0)
		{
			const char *szAct = pSimpleModel->GetComActByIndex(0)->GetName();
			pSimpleModel->PlayActionByName(szAct, 1.0f, false);
		}

		pSimpleModel->SetPos(GetPos());
		pSimpleModel->SetDirAndUp(GetDir(), GetUp());
		pSimpleModel->Tick(g_pGame->GetTickTime());

		if (bHighLight)
			g_pGame->RenderHighLightModel(pViewport, pSimpleModel);
		else
			pSimpleModel->Render(pViewport->GetA3DViewport(), true);
	}

	//	Render player head text
	if (m_bRenderBar)
		RenderBars(pViewport);

	if (m_bRenderName){
		SetMouseOnBoothFlag(bHighLight);
		RenderName(pViewport, g_pGame->GetConfigs()->GetPlayerTextFlags());
	}

	RenderPetCureGFX();
	RenderMultiObjectGFX();

	//	Print player's precise position
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

//	Process message
bool CECElsePlayer::ProcessMessage(const ECMSG& Msg)
{
	switch (Msg.dwMsg)
	{
	case MSG_PM_PLAYERFLY:			OnMsgPlayerFly(Msg);			break;
	case MSG_PM_PLAYERBASEINFO:		OnMsgPlayerBaseInfo(Msg);		break;
	case MSG_PM_PLAYERCUSTOM:		OnMsgPlayerCustomData(Msg);		break;
	case MSG_PM_PLAYEREQUIPDATA:	OnMsgPlayerEquipData(Msg);		break;
	case MSG_PM_PLAYERSITDOWN:		OnMsgPlayerSitDown(Msg);		break;
	case MSG_PM_CHANGENAMECOLOR:	OnMsgChangeNameColor(Msg);		break;
	case MSG_PM_PLAYERATKRESULT:	OnMsgPlayerAtkResult(Msg);		break;
	case MSG_PM_CASTSKILL:			OnMsgPlayerCastSkill(Msg);		break;
	case MSG_PM_PLAYEREXTSTATE:		OnMsgPlayerExtState(Msg);		break;
	case MSG_PM_PLAYERDOEMOTE:		OnMsgPlayerDoEmote(Msg);		break;
	case MSG_PM_PLAYERUSEITEM:		OnMsgPlayerUseItem(Msg);		break;
	case MSG_PM_USEITEMWITHDATA:	OnMsgPlayerUseItemWithData(Msg);break;
	case MSG_PM_ENCHANTRESULT:		OnMsgEnchantResult(Msg);		break;
	case MSG_PM_PLAYERROOT:			OnMsgPlayerRoot(Msg);			break;
	case MSG_PM_PLAYERDOACTION:		OnMsgPlayerDoAction(Msg);		break;
	case MSG_PM_PLAYERSKILLRESULT:	OnMsgPlayerSkillResult(Msg);	break;
	case MSG_PM_PLAYERADVDATA:		OnMsgPlayerAdvData(Msg);		break;
	case MSG_PM_PLAYERINTEAM:		OnMsgPlayerInTeam(Msg);			break;
	case MSG_PM_PICKUPMATTER:		OnMsgPickupMatter(Msg);			break;
	case MSG_PM_PLAYERGATHER:		OnMsgPlayerGather(Msg);			break;
	case MSG_PM_DOCONEMOTE:			OnMsgDoConEmote(Msg);			break;
	case MSG_PM_PLAYERCHGSHAPE:		OnMsgPlayerChangeShape(Msg);	break;
	case MSG_PM_GOBLINOPT:			OnMsgPlayerGoblinOpt(Msg);		break;
	case MSG_PM_BOOTHOPT:			OnMsgBoothOperation(Msg);		break;
	case MSG_PM_PLAYERTRAVEL:		OnMsgPlayerTravel(Msg);			break;
	case MSG_PM_PLAYERPVP:			OnMsgPlayerPVP(Msg);			break;
	case MSG_PM_FASHIONENABLE:		OnMsgSwitchFashionMode(Msg);	break;
	case MSG_PM_PLAYEREFFECT:		OnMsgPlayerEffect(Msg);			break;
	case MSG_PM_CHANGEFACE:			OnMsgPlayerChangeFace(Msg);		break;
	case MSG_PM_PLAYERBINDOPT:		OnMsgPlayerBindOpt(Msg);		break;
	case MSG_PM_PLAYERMOUNT:		OnMsgPlayerMount(Msg);			break;
	case MSG_PM_PLAYERDUELOPT:		OnMsgPlayerDuelOpt(Msg);		break;
	case MSG_PM_PLAYERLEVEL2:		OnMsgPlayerLevel2(Msg);			break;
	case MSG_PM_PLAYERKNOCKBACK:	OnMsgPlayerKnockback(Msg);		break;
	case MSG_PM_PLAYEREQUIPDISABLED: OnMsgPlayerEquipDisabled(Msg);	break;
	case MSG_PM_CONGREGATE:			OnMsgCongregate(Msg);			break;
	case MSG_PM_TELEPORT:				OnMsgTeleport(Msg);				break;
	case MSG_PM_FORCE_CHANGED:		OnMsgForce(Msg);				break;
	case MSG_PM_MULTIOBJ_EFFECT:	OnMsgMultiobjectEffect(Msg);	break;
	case MSG_PM_COUNTRY_CHANGED:	OnMsgCountry(Msg);	break;
	case MSG_PM_KINGCHANGED:		OnMsgKingChanged(Msg);			break;
	case MSG_PM_TITLE:				OnMsgTitle(Msg);				break;	
	case MSG_PM_REINCARNATION:		OnMsgReincarnation(Msg);		break;
	case MSG_PM_REALMLEVEL:			OnMsgRealmLevel(Msg);			break;
	case MSG_PM_PLAYER_IN_OUT_BATTLE: OnMsgPlayerInOutBattle(Msg);	break;
	case MSG_PM_FACTION_PVP_MASK_MODIFY:OnMsgFactionPVPMaskModify(Msg); break;
	}

	return true;
}

//	Set server position
void CECElsePlayer::SetServerPos(const A3DVECTOR3& vPos)
{
	m_vServerPos = vPos;

	//	If this player is a mule, change it's rider's server pos too.
	if (m_iBuddyId && !m_bHangerOn)
	{
		CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(m_iBuddyId);
		if (pPlayer)
			pPlayer->SetServerPos(vPos);
	}
}

//	Move to a destination
void CECElsePlayer::MoveTo(const S2C::cmd_object_move& Cmd)
{
	if (!Cmd.use_time)
		return;
	
	if (IsCurPosWork() && !m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_MOVE))	//	此处排除 WORK_MOVE，以获得连续移动时连贯性
		StopCurPosWork();

	SetServerPos(Cmd.dest);

	m_vMoveDir	= Cmd.dest - GetPos();
	float fDist	= m_vMoveDir.Normalize();

	m_bStopMove	= false;

	//	If destination position is too far to us, forcely pull player
	//	to that position.
	if (fDist >= MAX_LAGDIST)
	{
		SetPos(Cmd.dest);
		m_pEPWorkMan->FinishWork(CECEPWork::WORK_MOVE);
		return;
	}

	int iMoveMode = Cmd.move_mode;
	m_cdr.bTraceGround = true;

	if (iMoveMode & GP_MOVE_AIR)
	{
		m_iMoveEnv = MOVEENV_AIR;
		m_cdr.bTraceGround = false;
	}
	else if (iMoveMode & GP_MOVE_WATER)
	{
		m_iMoveEnv = MOVEENV_WATER;
		m_cdr.bTraceGround = false;
		ShowWing(false);
	}
	else
	{
		m_iMoveEnv = MOVEENV_GROUND;
		
		if (!IsFlying())
			ShowWing(false);
	}

	switch (iMoveMode & GP_MOVE_MASK)
	{
	case GP_MOVE_WALK:	m_iMoveMode = MOVE_MOVE;	m_bWalkRun	= false;	break;
	case GP_MOVE_RUN:	m_iMoveMode = MOVE_MOVE;	m_bWalkRun	= true;		break;
	case GP_MOVE_SLIDE:	m_iMoveMode = MOVE_SLIDE;	break;
	case GP_MOVE_FALL:	m_iMoveMode = MOVE_FREEFALL;	m_cdr.bTraceGround = false;	break;
	case GP_MOVE_FLYFALL:	m_cdr.bTraceGround = false; break;
	case GP_MOVE_JUMP:	m_iMoveMode	= MOVE_JUMP;	m_cdr.bTraceGround = false;	break;
	default:
		ASSERT(0);
		return;
	}

	DWORD dwTimeNow = a_GetTime();
	DWORD dwDeltaTime = (dwTimeNow>m_dwLastMoveTime) ? (dwTimeNow-m_dwLastMoveTime) : 0;
	m_dwLastMoveTime = dwTimeNow;
	if( dwDeltaTime < 500 )
		dwDeltaTime = 500;
	if( dwDeltaTime > 1000 )
		dwDeltaTime = 1000;
	
	//	Calculate speed
	float fSpeed = FIX8TOFLOAT(Cmd.sSpeed);
	m_fMoveSpeed = fDist / (dwDeltaTime * 0.001f);
	a_Clamp(m_fMoveSpeed, 0.0f, fSpeed * 1.2f);

	A3DVECTOR3 vDir = m_vMoveDir;
	vDir.y = 0.0f;
	if (!vDir.IsZero())
	{
		vDir.Normalize();
		StartModelMove(vDir, g_vAxisY, 150);		
		ChangeModelTargetDirAndUp(vDir, g_vAxisY);
	}

	if (m_pEPWorkMan->GetCurrentWorkType() < CECEPWorkMan::WT_NORMAL ||
		!m_pEPWorkMan->FindWork(CECEPWorkMan::WT_NORMAL, CECEPWork::WORK_MOVE)){
		m_pEPWorkMan->StartNormalWork(new CECEPWorkMove(m_pEPWorkMan));
	}

	//	Play action
	if (m_pPlayerModel)
	{
		int iCurAction = GetLowerBodyAction();
		if (IsValidAction(iCurAction))
		{
			if (!IsPlayingAction(ACT_TRICK_JUMP) && !IsPlayingAction(ACT_TRICK_RUN))
			{
				if (m_iMoveMode	== MOVE_JUMP || m_iMoveMode == MOVE_SLIDE)
					PlayAction(ACT_JUMP_LOOP, false);
				else
					PlayAction(GetMoveStandAction(true), false);
			}
		}
		else
			PlayAction(GetMoveStandAction(true), false);
	}
}

//	Stop moving to a destination position
void CECElsePlayer::StopMoveTo(const S2C::cmd_object_stop_move& Cmd)
{
	if (IsCurPosWork() && !m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_MOVE))	//	此处排除 WORK_MOVE，以获得连续移动时连贯性
		StopCurPosWork();

	m_vMoveDir		= Cmd.dest - GetPos();
	m_bStopMove		= true;
	m_fMoveSpeed	= FIX8TOFLOAT(Cmd.sSpeed);
	m_vStopDir		= glb_DecompressDirH(Cmd.dir);

	SetServerPos(Cmd.dest);

	//	test code...Consider speed of last time so that moving appears smoothly
//	if (m_fLastSpeed)
//		m_fMoveSpeed = m_fLastSpeed * 0.4f + m_fMoveSpeed * 0.6f;

	float fDist = m_vMoveDir.Normalize();
	//	If destination position is too far to us, forcely pull player
	//	to that position.
	if (fDist >= MAX_LAGDIST || m_fMoveSpeed < 0.01f)
	{
		m_bStopMove	= false;
		SetPos(Cmd.dest);
		StopModelMove(m_vStopDir, g_vAxisY, 150);
		m_pEPWorkMan->FinishWork(CECEPWork::WORK_MOVE);
		return;
	}

	int iMoveMode = Cmd.move_mode;
	m_cdr.bTraceGround = true;

	if (iMoveMode & GP_MOVE_AIR)
	{
		m_iMoveEnv = MOVEENV_AIR;
		m_cdr.bTraceGround = false;
	}
	else if (iMoveMode & GP_MOVE_WATER)
	{
		m_iMoveEnv = MOVEENV_WATER;
		m_cdr.bTraceGround = false;
	}
	else
		m_iMoveEnv = MOVEENV_GROUND;

	switch (iMoveMode & GP_MOVE_MASK)
	{
	case GP_MOVE_WALK:	m_iMoveMode = MOVE_MOVE;	m_bWalkRun	= false;	break;
	case GP_MOVE_RUN:	m_iMoveMode = MOVE_MOVE;	m_bWalkRun	= true;		break;
	case GP_MOVE_SLIDE:	m_iMoveMode = MOVE_SLIDE;	break;
	case GP_MOVE_FALL:	m_iMoveMode = MOVE_FREEFALL;	m_cdr.bTraceGround = false;	break;
	case GP_MOVE_FLYFALL:	m_cdr.bTraceGround = false;	break;
	case GP_MOVE_JUMP:	m_iMoveMode	= MOVE_JUMP;	m_cdr.bTraceGround = false;	break;
	default:
		ASSERT(0);
		return;
	}

	A3DVECTOR3 vDir = m_vMoveDir;
	vDir.y = 0.0f;
	if (!vDir.IsZero())
	{
		vDir.Normalize();
		StartModelMove(vDir, g_vAxisY, 150);
		ChangeModelTargetDirAndUp(vDir, g_vAxisY);
	}

	if (m_pEPWorkMan->GetCurrentWorkType() < CECEPWorkMan::WT_NORMAL ||
		!m_pEPWorkMan->FindWork(CECEPWorkMan::WT_NORMAL, CECEPWork::WORK_MOVE)){
		m_pEPWorkMan->StartNormalWork(new CECEPWorkMove(m_pEPWorkMan));
	}

	//	Play action
	if (m_pPlayerModel)
	{
		int iCurAction = GetLowerBodyAction();
		if (IsValidAction(iCurAction))
		{
			if (!IsPlayingAction(ACT_TRICK_JUMP) && !IsPlayingAction(ACT_TRICK_RUN))
			{
				if (m_iMoveMode	== MOVE_JUMP || m_iMoveMode == MOVE_SLIDE)
					PlayAction(ACT_JUMP_LOOP, false);
				else
					PlayAction(GetMoveStandAction(true), false);
			}
		}
		else
			PlayAction(GetMoveStandAction(true), false);
	}
}

//	Walk on ground to destination position
bool CECElsePlayer::MovingTo(DWORD dwDeltaTime)
{
	bool bRet = false;

	A3DVECTOR3 vPos, vCurPos = GetPos();
	float fDeltaTime = dwDeltaTime * 0.001f;

	if (m_bStopMove)
	{
		A3DVECTOR3 vDir = m_vServerPos - vCurPos;
		float fDist = vDir.Normalize();
		vPos = MoveStep(vDir, m_fMoveSpeed, fDeltaTime);

		float fMoveDelta = a3d_Magnitude(vPos - vCurPos);
		if (!fMoveDelta || fMoveDelta >= fDist)
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
		if (fDist >= MAX_LAGDIST)
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
A3DVECTOR3 CECElsePlayer::MoveStep(const A3DVECTOR3& vDir, float fSpeed, float fTime)
{
	A3DVECTOR3 vRealDir = vDir;

	//	OnAirMove only accept positive speed value
	if (fSpeed < 0.0f)
	{
		vRealDir = -vDir;
		fSpeed = -fSpeed;
	}

	m_cdr.vCenter		= m_aabbServer.Center;
	m_cdr.vVelocity		= vRealDir * fSpeed;
	m_cdr.t				= fTime;
	m_cdr.bTestTrnOnly	= false;

	OtherPlayerMove(m_cdr);

	if( m_cdr.bTraceGround)
		SetGroundNormal(m_cdr.vecGroundNormal);
	else
		SetGroundNormal(g_vAxisY);

	return m_cdr.vCenter - g_vAxisY * m_cdr.vExts.y;
}

//	Player was killed
void CECElsePlayer::Killed(int idKiller)
{
	ClearComActFlagAllRankNodes(true);

	m_SkillIDForStateAction = 0;
	m_dwStates |= GP_STATE_CORPSE;
	ShowWing(false);
	
	m_pEPWorkMan->StartNormalWork(new CECEPWorkDead(m_pEPWorkMan, 0));

	SetUseGroundNormal(true);
}

//	Revive
void CECElsePlayer::Revive(const S2C::cmd_player_revive* pCmd)
{
	//	Revive type
	enum
	{
		REVIVE_RETURN = 0,
		REVIVE_HERE_START,
		REVIVE_HERE_END,
	};

	m_dwStates &= ~GP_STATE_CORPSE;

	if (pCmd->sReviveType != REVIVE_HERE_END)
	{
		m_bAboutToDie = false;
		SetUseGroundNormal(false);

		//	Finish dead work
		m_pEPWorkMan->FinishWork(CECEPWork::WORK_DEAD);

		if (pCmd->sReviveType == REVIVE_HERE_START){
			m_pEPWorkMan->StartNormalWork(new CECEPWorkIdle(m_pEPWorkMan, CECEPWork::IDLE_REVIVE, 0, 0));
		}
	}
	else
	{
		m_pEPWorkMan->FinishIdleWork(CECEPWork::IDLE_REVIVE);
	}
}

//	Level up
void CECElsePlayer::LevelUp()
{
//	if (m_pLevelUpGFX)
//		m_pLevelUpGFX->Start(true);
	PlayGfx(res_GFXFile(RES_GFX_LEVELUP), NULL, 1.0, PLAYERMODEL_TYPEALL);
}

void CECElsePlayer::InvisibleDetected()
{
	PlayGfx(res_GFXFile(RES_GFX_INVISIBLE_DETECTED), NULL, 1.0, PLAYERMODEL_TYPEALL);
}

//  Change invisible
void CECElsePlayer::ChangeInvisible(int iDegree)
{
	if(iDegree > 0)
	{
		m_dwStates |= GP_STATE_INVISIBLE;
	}
	else
	{
		m_dwStates &= ~GP_STATE_INVISIBLE;
	}
}

//	Enter fight state
void CECElsePlayer::EnterFightState()
{
	m_FightCnt.Reset(false);
}

bool CECElsePlayer::IsFighting()
{ 
	return m_bFight || !m_FightCnt.IsFull();
}

//	Get off pet
void CECElsePlayer::GetOffPet(bool bResetData)
{
	CECPlayer::GetOffPet(bResetData);

	//	Restore cdr info
	m_cdr.vExts = m_aabbServer.Extents;
}

//	Set player's brief info
bool CECElsePlayer::SetPlayerBriefInfo(int iProf, int iGender, const ACHAR* szName)
{
	m_iProfession		= iProf;
	m_iGender			= iGender;
	m_bBaseInfoReady	= true;

	// compatible fix, force refresh shape id after get the prof and gender
	SetShape(GetShapeMask());

	//	Get player name and save into name cache. Because name got this time
	//	must be right, so we force to overwrite old name
	SetName(szName);
	g_pGame->GetGameRun()->AddPlayerName(m_PlayerInfo.cid, szName, true);

	//	Calculate player's AABB
	CalcPlayerAABB();

	//	Update upper body radius using new AABB
	m_cdr.fStepHeight	= m_MoveConst.fStepHei;
	m_cdr.vExts			= m_aabbServer.Extents;
	return true;
}

//	Change equipment
bool CECElsePlayer::ChangeEquipments(bool bReset, int crc, __int64 iAddMask, __int64 iDelMask, 
									 int* aAddedEquip)
{
	m_PlayerInfo.crc_e = crc;

	if (bReset)
	{
		m_i64NewEqpMask	= EQUIP_MASK64_ALL;
		m_bEquipReady	= true;

		memset(m_aEquips, 0xff, sizeof (m_aEquips));
		memset(m_aNewEquips, 0, sizeof (m_aNewEquips));
	}

	//	Remove equipment at first
	if (iDelMask)
	{
		for (__int64 i=0; i < SIZE_ALL_EQUIPIVTR; i++)
		{
			if (iDelMask & (1 << i))
			{
				m_aNewEquips[i] = 0;
				m_i64NewEqpMask |= (1 << i);
			}
		}
	}

	if (iAddMask && aAddedEquip)
	{
		int iCount = 0;
		for (__int64 i=0; i < SIZE_ALL_EQUIPIVTR; i++)
		{
			if (iAddMask & (1 << i))
			{
				m_aNewEquips[i] = aAddedEquip[iCount++];
				m_i64NewEqpMask |= (1 << i);
			}
		}
	}

	if (!ShouldUseModel())
		return true;

	if (IsSkeletonReady())
		LoadPlayerEquipments();

	// Deal with goblin（简单模型不加载小精灵）
	if(m_aNewEquips[EQUIPIVTR_GOBLIN] != 0 && ShouldUseClothedModel())
	{
		int tid = m_aNewEquips[EQUIPIVTR_GOBLIN] & 0x0000ffff;
		int idModel = (m_aNewEquips[EQUIPIVTR_GOBLIN] >>16) & 0x000000ff;
		int iRefineLvl = (m_aNewEquips[EQUIPIVTR_GOBLIN] >>24) & 0x000000ff;

		ASSERT(idModel >= 1 && idModel <= 4);
		ASSERT(iRefineLvl >=0 && iRefineLvl <= 36);		

		if( !m_pGoblin )
		{
			m_pGoblin = new CECGoblin();
			if(!m_pGoblin->Init(tid, idModel,iRefineLvl,this))
			{
				ASSERT(0);
				m_pGoblin->Release();
				delete m_pGoblin;
				m_pGoblin = NULL;
			}
		}
		else
		{
			if(tid != m_pGoblin->GetTemplateID() ||
				idModel != (int)m_pGoblin->GetModelID() ||
				iRefineLvl != m_pGoblin->GetRefineLevel())
			{
				m_pGoblin->Release();
				delete m_pGoblin;
				m_pGoblin = NULL;
				
				m_pGoblin = new CECGoblin();
				if(!m_pGoblin->Init(tid, idModel,iRefineLvl,this))
				{
					ASSERT(0);
					m_pGoblin->Release();
					delete m_pGoblin;
					m_pGoblin = NULL;
				}
			}
		}
	}
	else	// m_aNewEquips[EQUIPIVTR_GOBLIN] == 0
	{
		if(m_pGoblin)
		{
			m_pGoblin->Release();
			delete m_pGoblin;
			m_pGoblin = NULL;
		}
	}
	
	// Deal with armet
	if(InFashionMode())
	{
		UpdateHairModel(true, m_aNewEquips[EQUIPIVTR_FASHION_HEAD]);
	}
	else
	{
		UpdateHairModel(true, m_aNewEquips[EQUIPIVTR_HEAD]);
	}

	return true;
}

//	Load player equipments
bool CECElsePlayer::LoadPlayerEquipments()
{
#ifdef _DEBUG
	
	for (int i=0; i < SIZE_EQUIPIVTR; i++)
	{
		if (m_aNewEquips[i] < 0)
		{
			ASSERT(m_aNewEquips[i] >= 0);
		}
	}

#endif	//	_DEBUG

	ShowEquipments(m_aNewEquips, false);
	SetResReadyFlag(RESFG_SKIN, true);

	if (m_pPlayerModel)
	{
		//	If weapon changed, action index may be changed, so update here
		int iCurAction = GetLowerBodyAction();
		if (IsValidAction(iCurAction))
			PlayAction(iCurAction, false);
	}

	return true;
}

//	Set loaded model to player object, this function is used in multithread loading process
bool CECElsePlayer::SetPlayerLoadedResult(EC_PLAYERLOADRESULT& Ret)
{
	if (!CECPlayer::SetPlayerLoadedResult(const_cast<EC_PLAYERLOADRESULT&>(Ret)))
		return false;

	if (IsFlying())
		ShowWing(true);

	m_pEPWorkMan->StartWork(CECEPWorkMan::WT_NOTHING, new CECEPWorkStand(m_pEPWorkMan));

	if (IsDead())
	{
		ShowWing(false);
		m_pEPWorkMan->StartNormalWork(new CECEPWorkDead(m_pEPWorkMan, 1));
	}
	else if (IsSitting())
	{
		m_pEPWorkMan->StartNormalWork(new CECEPWorkIdle(m_pEPWorkMan, CECEPWork::IDLE_SITDOWN, 0, 1));
	}
	else if (m_iBoothState == 2)
	{
		m_pEPWorkMan->StartNormalWork(new CECEPWorkIdle(m_pEPWorkMan, CECEPWork::IDLE_BOOTH, 0, 0));
	}

	return true;
}

//	Is spelling magic
bool CECElsePlayer::IsSpellingMagic()
{
	return m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_SPELL);
}

void CECElsePlayer::OnMsgPlayerFly(const ECMSG& Msg)
{
	using namespace S2C;

	if (IsAllResReady())
	{
		if (Msg.dwParam2 == OBJECT_LANDING)
			ShowWing(false);
		else	//	OBJECT_TAKEOFF
			ShowWing(true);
	}

	if (Msg.dwParam2 == OBJECT_LANDING)
		m_dwStates &= ~GP_STATE_FLY;
	else
		m_dwStates |= GP_STATE_FLY;
}

void CECElsePlayer::OnMsgPlayerBaseInfo(const ECMSG& Msg)
{
	using namespace GNET;
	
	PlayerBaseInfo_Re* p = (PlayerBaseInfo_Re*)Msg.dwParam1;
	const GRoleBase& base = p->player;

	ASSERT((int)base.id == m_PlayerInfo.cid);
	
	if (!base.name.size())
		return;

	ACString strName = ACString((const ACHAR*)base.name.begin(), base.name.size() / sizeof (ACHAR));
	SetPlayerBriefInfo(base.cls, base.gender, strName);

	m_PlayerInfo.crc_c = base.custom_stamp;
	m_bCustomReady = true;

	//	Load custom data
	if (base.custom_data.size() >= 4)
	{
		if (!ChangeCustomizeData(PLAYER_CUSTOMIZEDATA::From(base.custom_data.begin(), base.custom_data.size()), false))
		{
			a_LogOutput(1, "CECElsePlayer::OnMsgPlayerBaseInfo, Failed to load custom data");
		//	return;
		}
	}
	else
		SetResReadyFlag(RESFG_CUSTOM, true);
}

void CECElsePlayer::OnMsgPlayerCustomData(const ECMSG& Msg)
{
	using namespace GNET;
	
	GetCustomData_Re* p = (GetCustomData_Re*)Msg.dwParam1;

	//	Load custom data
	if (p->customdata.size() >= 4)
	{
		//	Apply immediately flag. If IsCustomizeReady() return false that means
		//	this is the initial time to set custom data, in this case we needn't
		//	to apply custom data immedately because it will be updated in 
		//	soon later AssembleResources().
		m_bCustomReady = true;
		bool bApplyImme = IsCustomizeReady();
		if (!ChangeCustomizeData(PLAYER_CUSTOMIZEDATA::From(p->customdata.begin(), p->customdata.size()), bApplyImme))
		{
			a_LogOutput(1, "CECElsePlayer::OnMsgPlayerCustomData, Failed to load custom data");
		//	return;
		}
	}
	else
	{
		m_bCustomReady = true;
		SetResReadyFlag(RESFG_CUSTOM, true);
	}
}

void CECElsePlayer::OnMsgPlayerEquipData(const ECMSG& Msg)
{
	using namespace S2C;

	int crc, *aAdded=NULL;
	bool bReset;
	__int64 iAddMask, iDelMask;

	if (Msg.dwParam2 == EQUIP_DATA)
	{
		cmd_equip_data* pCmd = (cmd_equip_data*)Msg.dwParam1;
		ASSERT(pCmd);

		bReset		= true;
		crc			= pCmd->crc;
		iAddMask	= pCmd->mask;
		iDelMask	= 0;
		aAdded		= pCmd->data;
	}
	else	//	Msg.dwParam2 == EQUIP_DATA_CHANGED
	{
		cmd_equip_data_changed* pCmd = (cmd_equip_data_changed*)Msg.dwParam1;
		ASSERT(pCmd);

		bReset		= false;
		crc			= pCmd->crc;
		iAddMask	= pCmd->mask_add;
		iDelMask	= pCmd->mask_del;
		aAdded		= iAddMask ? pCmd->data_add : NULL;
	}

	//	Change equipment
	ChangeEquipments(bReset, crc, iAddMask, iDelMask, aAdded);
}

void CECElsePlayer::OnMsgPlayerSitDown(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == OBJECT_SIT_DOWN)
	{
		m_dwStates |= GP_STATE_SITDOWN;
		cmd_object_sit_down* pCmd = (cmd_object_sit_down*)Msg.dwParam1;
		m_pEPWorkMan->StartNormalWork(new CECEPWorkIdle(m_pEPWorkMan, CECEPWork::IDLE_SITDOWN, 0, 0));
	}
	else	//	Msg.dwParams2 == OBJECT_STAND_UP
	{
		m_dwStates &= ~GP_STATE_SITDOWN;
		cmd_object_stand_up* pCmd = (cmd_object_stand_up*)Msg.dwParam1;
		if (m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_IDLE)){
			const CECEPWorkIdle *pWorkIdle = dynamic_cast<const CECEPWorkIdle *>(m_pEPWorkMan->GetRunningWork(CECEPWorkIdle::WORK_IDLE));
			if (pWorkIdle->GetType() == CECEPWork::IDLE_SITDOWN)
			{
				PlayAction(ACT_STANDUP);
				m_pEPWorkMan->FinishIdleWork(CECEPWork::IDLE_SITDOWN);
			}
		}
	}
}

void CECElsePlayer::OnMsgPlayerAtkResult(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_object_atk_result* pCmd = (cmd_object_atk_result*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->attacker_id == m_PlayerInfo.cid);

	//	Face to target
	TurnFaceTo(pCmd->target_id);

	PlayAttackEffect(pCmd->target_id, 0, 0, -1, pCmd->attack_flag, pCmd->speed * 50);
	
	if (!m_pEPWorkMan->FindWork(CECEPWorkMan::WT_NORMAL, CECEPWork::WORK_HACKOBJECT)){
		m_pEPWorkMan->StartNormalWork(new CECEPWorkMelee(m_pEPWorkMan, pCmd->target_id));
	}

	//	Enter fight state
	EnterFightState();
}

void CECElsePlayer::OnMsgPlayerSkillResult(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_object_skill_attack_result* pCmd = (cmd_object_skill_attack_result*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->attacker_id == m_PlayerInfo.cid);

	//	Face to target
	TurnFaceTo(pCmd->target_id);

	PlayAttackEffect(pCmd->target_id, pCmd->skill_id, 0, -1, pCmd->attack_flag, pCmd->speed * 50,NULL,pCmd->section);

	int iSkillType = GNET::ElementSkill::GetType(pCmd->skill_id);
	if (iSkillType == GNET::TYPE_ATTACK || iSkillType == TYPE_CURSE)
	{
		//	Enter fight state
		EnterFightState();
	}
}

void CECElsePlayer::OnMsgPlayerCastSkill(const ECMSG& Msg)
{
	using namespace S2C;
	
	if (Msg.dwParam2 == OBJECT_CAST_SKILL)
	{
		ClearCastingSkill();

		cmd_object_cast_skill* pCmd = (cmd_object_cast_skill*)Msg.dwParam1;
		if (!(m_pCurSkill = new CECSkill(pCmd->skill, pCmd->level)))
			return;
		m_idCurSkillTarget = pCmd->target;

		DWORD dwPeriod = (pCmd->time + m_pCurSkill->GetExecuteTime()) * 10;
		m_pEPWorkMan->StartNormalWork(new CECEPWorkSpell(m_pEPWorkMan, dwPeriod, new CECSkill(pCmd->skill, pCmd->level), pCmd->target));		
		PlaySkillCastAction(m_pCurSkill->GetSkillID());
	}
	else if (Msg.dwParam2 == SKILL_INTERRUPTED)
	{
		ClearComActFlagAllRankNodes(false);
		ClearCastingSkill();
		m_pEPWorkMan->FinishWork(CECEPWork::WORK_SPELL);
		StopSkillCastAction();
	}
	else if (Msg.dwParam2 == OBJECT_CAST_INSTANT_SKILL)
	{
		StopCurPosWork();

		ClearCastingSkill();
		m_pEPWorkMan->FinishWork(CECEPWork::WORK_SPELL);

		cmd_object_cast_instant_skill* pCmd = (cmd_object_cast_instant_skill*)Msg.dwParam1;
		PlaySkillCastAction(pCmd->skill);
	}
	else if (Msg.dwParam2 == OBJECT_CAST_POS_SKILL)
	{
		StopCurPosWork();
		ClearCastingSkill();
		m_pEPWorkMan->FinishWork(CECEPWork::WORK_SPELL);

		cmd_object_cast_pos_skill* pCmd = (cmd_object_cast_pos_skill*)Msg.dwParam1;
		SetServerPos(pCmd->pos);

		float fDist = a3d_Magnitude(pCmd->pos - GetPos());
		if (fDist <= 0.0001f)
		{
			//	技能使用可能失败（目标位置有凸包、中途有碰撞或地形限制等，返回的是自己的位置）
			SetPos(pCmd->pos);
			return;
		}

		//	检查技能执行时长，避免浮点除0导致当前ElsePlayer的A3DCoordinate中位置计算出错、及自动被选中等问题
		int nExecuteTime = GNET::ElementSkill::GetExecuteTime(pCmd->skill, pCmd->level);
		a_ClampFloor(nExecuteTime, 50);

		m_fMoveSpeed = 1.3f * fDist * 1000.0f / nExecuteTime;
		m_pEPWorkMan->StartNormalWork(new CECEPWorkFlashMove(m_pEPWorkMan, pCmd->pos, m_fMoveSpeed));
	}
}

void CECElsePlayer::StartPassiveMove(const A3DVECTOR3& pos, int time, int type)
{
	SetServerPos(pos);

	float fDist = a3d_Magnitude(pos - GetPos());
	if (fDist <= 0.0001f || time <= 0)
	{
		SetPos(pos);
		return;
	}

	//	time 值由服务器传递，目前为导致目标移动最长距离时的时间，
	//	为加快拉人过程，减少后续移动协议（如OBJECT_STOP_MOVE）中断当前过程、并发生位置相关显示问题的几率，
	//	根据当前最长可移动距离和已移动距离修改 time，从而改变 m_fMoveSpeed，加快当前过程

	const float REF_DISTANCE = 18.0f;	//	以武侠技能"赤龙索"当前最远可拉人距离为参考值（当实际值>18时，以下处理方法只获得1.3倍速；当实际值<18时，以下处理方法将获得更快速度）
	float ratio = fDist  / REF_DISTANCE;
	a_ClampRoof(ratio, 1.0f);
	float fTime = ratio * time * 0.001f;		//	缩放后的技能执行时间（单位修改为秒）

	m_fMoveSpeed = fDist / fTime;
	if (fDist >= REF_DISTANCE * 0.8f)
		m_fMoveSpeed *= 1.3f;

	m_pEPWorkMan->StartNormalWork(new CECEPWorkPassiveMove(m_pEPWorkMan, pos, m_fMoveSpeed));
}

void CECElsePlayer::OnMsgPlayerKnockback(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_player_knockback *pCmd = (cmd_player_knockback *)Msg.dwParam1;

	StopCurPosWork();
	StartPassiveMove(pCmd->pos, pCmd->time, PASSIVE_KNOCKBACK);

	EnterFightState();
}

void CECElsePlayer::OnMsgPlayerEquipDisabled(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_player_equip_disabled* pCmd = (cmd_player_equip_disabled*)Msg.dwParam1;
	ASSERT(pCmd->id == m_PlayerInfo.cid);
	
	for (__int64 i = 0; i < SIZE_ALL_EQUIPIVTR; i++)
	{
		if((1 << i) & m_i64NewEqpMask)
		{
			m_aEquips[i] = m_aNewEquips[i];
		}
	}
	ChangeEquipDisableMask(pCmd->mask);
}

void CECElsePlayer::OnMsgPlayerDoEmote(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == OBJECT_DO_EMOTE)
	{
		cmd_object_do_emote* pCmd = (cmd_object_do_emote*)Msg.dwParam1;

		DoEmote(pCmd->emotion);		

		if( m_iBuddyId )
		{
			CECPlayer* pBuddy = m_pPlayerMan->GetPlayer(m_iBuddyId);
			if (pBuddy)
				pBuddy->DoEmote(pCmd->emotion);
		}
	}
	else if (Msg.dwParam2 == OBJECT_EMOTE_RESTORE)
	{
		PlayAction(ACT_STAND, false);
	}
}

void CECElsePlayer::OnMsgPlayerUseItem(const ECMSG& Msg)
{
	using namespace S2C;

	switch (Msg.dwParam2)
	{
	case OBJECT_START_USE:
	{
		cmd_object_start_use * pCmd = (cmd_object_start_use*)Msg.dwParam1;
		m_pEPWorkMan->StartNormalWork(new CECEPWorkUseItem(m_pEPWorkMan, pCmd->item));
		break;
	}

	case OBJECT_CANCEL_USE:	
	{
		cmd_object_cancel_use * pCmd = (cmd_object_cancel_use*)Msg.dwParam1;
		m_pEPWorkMan->FinishWork(CECEPWork::WORK_USEITEM);
		break;
	}

	case OBJECT_USE_ITEM:
	{
		cmd_object_use_item* pCmd = (cmd_object_use_item*)Msg.dwParam1;
		m_pEPWorkMan->FinishWork(CECEPWork::WORK_USEITEM);
		PlayUseItemEffect(pCmd->item);
		break;
	}

	case OBJECT_START_USE_T:
	{
		cmd_object_start_use_t * pCmd = (cmd_object_start_use_t*)Msg.dwParam1;
		m_pEPWorkMan->StartNormalWork(new CECEPWorkUseItem(m_pEPWorkMan, pCmd->item));
		TurnFaceTo(pCmd->target);
		break;
	}

	default:
		ASSERT(0);
		return;
	}
}

void CECElsePlayer::OnMsgPlayerUseItemWithData(const ECMSG& Msg)
{
	using namespace S2C;
	object_use_item_with_arg* pCmd = (object_use_item_with_arg*)Msg.dwParam1;
	m_pEPWorkMan->FinishWork(CECEPWork::WORK_USEITEM);
	PlayUseItemEffect(pCmd->item, pCmd->arg, pCmd->size);
}

void CECElsePlayer::OnMsgPlayerRoot(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_object_root* pCmd = (cmd_object_root*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->id == m_PlayerInfo.cid);

	StopCurPosWork();

	SetServerPos(pCmd->pos);
	SetPos(pCmd->pos);
}

void CECElsePlayer::OnMsgPlayerDoAction(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_obj_do_action* pCmd = (cmd_obj_do_action*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->id == m_PlayerInfo.cid);

	if (m_pPlayerModel)
	{
		int iCurAction = GetLowerBodyAction();
		if (IsValidAction(iCurAction))
		{
			PlayAction(pCmd->action, false);
			PlayAction(iCurAction, true, 200, true);
		}
	}
}

void CECElsePlayer::OnMsgPlayerInTeam(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_player_in_team* pCmd = (cmd_player_in_team*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->id == m_PlayerInfo.cid);

	m_iInTeamState = pCmd->state;
}

void CECElsePlayer::OnMsgPickupMatter(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_matter_pickup* pCmd = (cmd_matter_pickup*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->who == m_PlayerInfo.cid);
	m_pEPWorkMan->StartNormalWork(new CECEPWorkPickUp(m_pEPWorkMan, 500, CECEPWorkPickUp::PICKUP_ITEM));
}

void CECElsePlayer::OnMsgPlayerGather(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == PLAYER_GATHER_START)
	{
		cmd_player_gather_start* pCmd = (cmd_player_gather_start*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->pid == m_PlayerInfo.cid);
		CECMatterMan* pMatterMan = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();
		CECMatter* pMatter = pMatterMan->GetMatter(pCmd->mid);
		if (pMatter && pMatter->IsMonsterSpiritMine()) {
			StartMonsterSpiritConnectGfx(pCmd->mid, pMatter->GetPos());
			m_pEPWorkMan->StartNormalWork(new CECEPWorkPickUp(m_pEPWorkMan, pCmd->use_time * 1000, CECEPWorkPickUp::GATHER_MONSTER_SPIRIT));
		} else {
			m_pEPWorkMan->StartNormalWork(new CECEPWorkPickUp(m_pEPWorkMan, 0, CECEPWorkPickUp::GATHER_ITEM, pMatter ? pMatter->GetTemplateID() : 0));
		}
	}
	else if (Msg.dwParam2 == PLAYER_GATHER_STOP)
	{
		cmd_player_gather_stop* pCmd = (cmd_player_gather_stop*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->pid == m_PlayerInfo.cid);
		m_pEPWorkMan->FinishWork(CECEPWork::WORK_PICKUP);
		StopMonsterSpiritConnectGfx();
	}
	else if (Msg.dwParam2 == MINE_GATHERED)
	{
		cmd_mine_gathered* pCmd = (cmd_mine_gathered*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->player_id == m_PlayerInfo.cid);
		StartMonsterSpiritBallGfx();
	}
}

void CECElsePlayer::OnMsgDoConEmote(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_do_concurrent_emote* pCmd = (cmd_do_concurrent_emote*)Msg.dwParam1;
	ASSERT(pCmd && Msg.dwParam2 == DO_CONCURRENT_EMOTE);

	PlayAction(pCmd->emotion, true);
}

void CECElsePlayer::OnMsgPlayerChangeShape(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_player_chgshape* pCmd = (cmd_player_chgshape*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->idPlayer == m_PlayerInfo.cid);
	TransformShape(pCmd->shape);
}

void CECElsePlayer::OnMsgPlayerGoblinOpt(const ECMSG& Msg)
{
	using namespace S2C;
	
	switch (Msg.dwParam2)
	{
	case ELF_REFINE_ACTIVATE:
	{
		cmd_elf_refine_activate* pCmd = (cmd_elf_refine_activate*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->pid == m_PlayerInfo.cid);
	
		if(!m_pGoblin)
		{
#ifndef	_PROFILE_MEMORY
			ASSERT(m_pGoblin);
#endif
			return;
		}
		m_pGoblin->ProcessMessage(Msg);
		break;
	}
	case CAST_ELF_SKILL:
	{
		cmd_cast_elf_skill* pCmd = (cmd_cast_elf_skill*)Msg.dwParam1;		
		ASSERT(pCmd && pCmd->pid == m_PlayerInfo.cid);
	
		if(!m_pGoblin)
		{
#ifndef	_PROFILE_MEMORY
			ASSERT(m_pGoblin);
#endif
			return;
		}
		m_pGoblin->ProcessMessage(Msg);
		
		break;
	}
	}	
}

void CECElsePlayer::OnMsgBoothOperation(const ECMSG& Msg)
{
	using namespace S2C;

	switch (Msg.dwParam2)
	{
	case PLAYER_OPEN_BOOTH:
	{
		cmd_player_open_booth* pCmd = (cmd_player_open_booth*)Msg.dwParam1;
		ASSERT(pCmd->pid == m_PlayerInfo.cid);

		m_iBoothState	= 2;
		m_crcBooth		= pCmd->booth_crc;

		//	Set booth name
		if (pCmd->name_len && pCmd->name)
		{
			ACString strBoothName((const ACHAR*)pCmd->name, pCmd->name_len / sizeof (ACHAR));
			SetBoothName(strBoothName);
		}
		else
			SetBoothName(_AL("Booth"));
		m_pEPWorkMan->StartNormalWork(new CECEPWorkIdle(m_pEPWorkMan, CECEPWork::IDLE_BOOTH, 0, 0));
		break;
	}
	case PLAYER_CLOSE_BOOTH:
	{
		cmd_player_close_booth* pCmd = (cmd_player_close_booth*)Msg.dwParam1;
		ASSERT(pCmd->pid == m_PlayerInfo.cid);

		m_iBoothState	= 0;
		
		//	If host is visiting this player's booth, notify him
		CECHostPlayer* pHost = m_pPlayerMan->GetHostPlayer();
		if (pHost->GetBoothState() == 3 && pHost->GetCurServiceNPC() == m_PlayerInfo.cid)
			pHost->OnOtherBoothClosed();

		m_pEPWorkMan->FinishIdleWork(CECEPWork::IDLE_BOOTH);
		break;
	}
	case BOOTH_NAME:
	{
		cmd_booth_name* pCmd = (cmd_booth_name*)Msg.dwParam1;
		ASSERT(pCmd->pid == m_PlayerInfo.cid);

		if (pCmd->name && pCmd->name_len)
		{
			m_crcBooth = pCmd->crc_name;
			ACString strBoothName((const ACHAR*)pCmd->name, pCmd->name_len / sizeof (ACHAR));
			SetBoothName(strBoothName);
		}

		break;
	}
	}
}

void CECElsePlayer::OnMsgPlayerTravel(const ECMSG& Msg)
{
	using namespace S2C;
}

void CECElsePlayer::OnMsgPlayerChangeFace(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_player_chg_face* pCmd = (cmd_player_chg_face*)Msg.dwParam1;
	m_PlayerInfo.crc_c = pCmd->crc_c;
	g_pGame->GetGameSession()->GetRoleCustomizeData(1, &m_PlayerInfo.cid);
}

void CECElsePlayer::OnMsgPlayerBindOpt(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == PLAYER_BIND_STOP)
	{
		m_idCandBuddy = 0;
		m_bCandHangerOn = false;

		if (!m_bHangerOn)
			DetachBuddy(NULL);
	}
}

void CECElsePlayer::OnMsgPlayerDuelOpt(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == PLAYER_DUEL_START)
	{
		m_pvp.iDuelState = DUEL_ST_INDUEL;
		m_pvp.iDuelRlt = 0;
	}
	else if (Msg.dwParam2 == DUEL_STOP)
		m_pvp.iDuelState = DUEL_ST_NONE;
	else
	{
		ASSERT(0);
	}
}

bool CECElsePlayer::DoEmote(int idEmote)
{
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

	PlayAction(iAction);

	if (!bSession)
		PlayAction(GetMoveStandAction(false, false), true, 300, true);

	return true;
}

void CECElsePlayer::OnMsgCongregate(const ECMSG &Msg)
{
	using namespace S2C;
	if(Msg.dwParam2 == CONGREGATE_START)
	{
		cmd_congregate_start* pCmd = (cmd_congregate_start*)Msg.dwParam1;
		m_pEPWorkMan->StartNormalWork(new CECEPWorkCongregate(m_pEPWorkMan, pCmd->type));
	}
	else if(Msg.dwParam2 == CANCEL_CONGREGATE)
	{
		cmd_cancel_congregate* pCmd = (cmd_cancel_congregate*)Msg.dwParam1;
		m_pEPWorkMan->FinishCongregateWork(pCmd->type);
	}
}

void CECElsePlayer::OnMsgTeleport(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_player_teleport *pCmd = (cmd_player_teleport *)Msg.dwParam1;

	if(pCmd->mode == PASSIVE_DIRECT)
	{
		// exchange pos directly
		StopCurPosWork();
		SetServerPos(pCmd->pos);
		SetPos(pCmd->pos);
	}
	else if(pCmd->mode == PASSIVE_PULL)
	{
		// pull by others
		StartPassiveMove(pCmd->pos, pCmd->use_time, PASSIVE_PULL);
	}
	else
	{
		// unknown type
		ASSERT(false);
	}
}

void CECElsePlayer::OnMsgForce(const ECMSG &Msg)
{
	using namespace S2C;
	ASSERT(Msg.dwParam2 == PLAYER_FORCE_CHANGED);
	cmd_player_force_changed *pCmd = (cmd_player_force_changed *)Msg.dwParam1;
	m_idForce = pCmd->cur_force_id;
}

void CECElsePlayer::OnMsgMultiobjectEffect(const ECMSG &Msg)
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

void CECElsePlayer::OnMsgCountry(const ECMSG &Msg)
{
	using namespace S2C;

	switch (Msg.dwParam2)
	{
	case PLAYER_COUNTRY_CHANGED:
		{
			cmd_player_country_changed *pCmd = (cmd_player_country_changed *)Msg.dwParam1;
			SetCountry(pCmd->country_id);
			break;
		}
	}
}
void CECElsePlayer::OnMsgTitle(const ECMSG &Msg)
{
	using namespace S2C;
	if (Msg.dwParam2 == QUERY_TITLE_RE) {
		
	} else if (Msg.dwParam2 == CHANGE_CURR_TITLE_RE) {
		cmd_change_curr_title_re* pCmd = (cmd_change_curr_title_re*)Msg.dwParam1;
		SetCurrentTitle(pCmd->titleid);
	} 
}
void CECElsePlayer::OnMsgReincarnation(const ECMSG &Msg)
{
	using namespace S2C;
	if (Msg.dwParam2 == PLAYER_REINCARNATION) {
		cmd_player_reincarnation* pCmd = (cmd_player_reincarnation*)Msg.dwParam1;
		SetReincarnationCount(pCmd->reincarnation_times);
	}
}
void CECElsePlayer::OnMsgRealmLevel(const ECMSG &Msg)
{
	using namespace S2C;
	if (Msg.dwParam2 == REALM_LEVEL) {
		cmd_realm_level* pCmd = (cmd_realm_level*)Msg.dwParam1;
		SetRealmLevel(pCmd->level);
		PlayGfx(res_GFXFile(RES_GFX_REALM_LEVELUP), NULL, 1.0, PLAYERMODEL_TYPEALL);
	}
		
}
void CECElsePlayer::OnMsgPlayerInOutBattle(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_player_in_out_battle* pCmd = (cmd_player_in_out_battle*)Msg.dwParam1;
	if (pCmd->in_out) PlayEnterBattleGfx();
	m_bFight = pCmd->in_out;
}

void CECElsePlayer::OnMsgFactionPVPMaskModify(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_faction_pvp_mask_modify *pCmd = (cmd_faction_pvp_mask_modify *)Msg.dwParam1;
	SetFactionPVPMask(pCmd->mask);
}

bool CECElsePlayer::IsCurPosWork()const
{
	//	当前为位移相关 WORK 时返回 true
	return m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_MOVE)
		|| m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_PASSIVEMOVE)
		|| m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_FLASHMOVE);
}

void CECElsePlayer::StopCurPosWork()
{	
	//	强行结束当前位移相关 WORK（如 WORK_MOVE、WORK_FLASHMOVE、WORK_PASSIVEMOVE）
	//	在即将启用新的位移 WORK 而又不希望两者之间相互影响时使用
	//	各位移 WORK 共用变量 m_vServerPos, m_fMoveSpeed 等，因此，某些情况下需要隔离，以避免后来者影响前者的执行效果	
	if (m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_MOVE)){
		m_pEPWorkMan->FinishRunningWork(CECEPWork::WORK_MOVE);
	}
	if (m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_PASSIVEMOVE)){
		m_pEPWorkMan->FinishRunningWork(CECEPWork::WORK_PASSIVEMOVE);
	}
	if (m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_FLASHMOVE)){
		m_pEPWorkMan->FinishRunningWork(CECEPWork::WORK_FLASHMOVE);
	}
}

void CECElsePlayer::ClearCastingSkill(){
	if (m_pCurSkill){
		delete m_pCurSkill;
		m_pCurSkill = NULL;
	}
	m_idCurSkillTarget = 0;
}

// 切换玩家简单模型模式
void CECElsePlayer::SwitchSimpleModel()
{
	if( !IsBaseInfoReady() || !IsCustomDataReady() || !IsEquipDataReady() )
		return;

	// 解决加载的BODY被LoadPlayerSkeleton(false)中的BODY冲掉的BUG.
	// 玩家进入世界后先调用LoadPlayerSkeleton(false)
	// 在加载线程完成资源加载之前，我们调用LoadBodySkin，此函数是在游戏主线程中调用，会立即得到SKIN结果。
	// 然后加载线程也完成了资源加载，所得到的BODY将会冲掉前面的BODY.
	if( !GetMajorModel() )
		return;

	if( !ShouldUseClothedModel() )
	{
		//	Release player skins
		for (int i=0; i < NUM_SKIN_INDEX; i++)
		{
			ReplaceCurSkin(i, NULL);
			if( m_aSkins[i][0] ) { g_pGame->ReleaseA3DSkin(m_aSkins[i][0]); m_aSkins[i][0] = NULL; }
			if( m_aSkins[i][1] ) { g_pGame->ReleaseA3DSkin(m_aSkins[i][1]); m_aSkins[i][1] = NULL; }
			if( m_aSkins[i][2] ) { g_pGame->ReleaseA3DSkin(m_aSkins[i][2]); m_aSkins[i][2] = NULL; }
			if( m_aCurSkins[i] ) m_aCurSkins[i] = NULL;
		}

		memset(m_pBodyShader, 0, sizeof(m_pBodyShader));

		//  Release the sprite and goblin
		A3DRELEASE(m_pSprite);
		A3DRELEASE(m_pGoblin);

		//  Load the simple body skin
		LoadBodySkin(m_CustomizeData.bodyID, true);		// 加载简单躯体
	}
	else
	{
		//  Reload the player's equip
		LoadBodySkin(m_CustomizeData.bodyID, false);	// 加载正常躯体
		ShowEquipments(m_aEquips, false, true);			// 加载装备

		//  加载小精灵
		ReloadGoblin();
		UpdateGodEvilSprite();
	}
}

bool CECElsePlayer::ReloadGoblin()
{
	if( m_aEquips[EQUIPIVTR_GOBLIN] > 0 )
	{
		int tid = m_aEquips[EQUIPIVTR_GOBLIN] & 0x0000ffff;
		int idModel = (m_aEquips[EQUIPIVTR_GOBLIN] >>16) & 0x000000ff;
		int iRefineLvl = (m_aEquips[EQUIPIVTR_GOBLIN] >>24) & 0x000000ff;

		ASSERT(idModel >= 1 && idModel <= 4);
		ASSERT(iRefineLvl >=0 && iRefineLvl <= 36);

		if( !m_pGoblin )
		{
			m_pGoblin = new CECGoblin();
			if(!m_pGoblin->Init(tid, idModel,iRefineLvl,this))
			{
				ASSERT(0);
				m_pGoblin->Release();
				delete m_pGoblin;
				m_pGoblin = NULL;
				return false;
			}
		}
		else
		{
			if(tid != m_pGoblin->GetTemplateID() ||
				idModel != (int)m_pGoblin->GetModelID() ||
				iRefineLvl != m_pGoblin->GetRefineLevel())
			{
				m_pGoblin->Release();
				delete m_pGoblin;
				m_pGoblin = NULL;
				
				m_pGoblin = new CECGoblin();
				if(!m_pGoblin->Init(tid, idModel,iRefineLvl,this))
				{
					ASSERT(0);
					m_pGoblin->Release();
					delete m_pGoblin;
					m_pGoblin = NULL;
					return false;
				}
			}
		}
	}
	else
	{
		if(m_pGoblin)
		{
			m_pGoblin->Release();
			delete m_pGoblin;
			m_pGoblin = NULL;
		}
	}

	return true;
}

// 卸载非重要玩家的模型资源
void CECElsePlayer::ReleaseModels()
{
	//  卸载模型前需要下坐骑
	if( m_pPetModel )
		GetOffPet(false);

	//  解除相依相偎
	if( m_iBuddyId )
	{
		if( m_bHangerOn )
		{
			CECPlayer* pBuddy = m_pPlayerMan->GetPlayer(m_iBuddyId);
			if( pBuddy )
				pBuddy->DetachBuddy(NULL, false);
		}
		else
			DetachBuddy(NULL, false);
	}

	//	Release face model
	ReleaseFaceModel();
	
	//	Release player model
	ReleasePlayerModel();
	
	//	Clear resource ready flags
	SetResReadyFlag(RESFG_ALL, false);
}

// 重新加载重要玩家的模型资源
void CECElsePlayer::ReloadModels()
{
	if( IsBaseInfoReady() && IsCustomDataReady() && IsEquipDataReady() )
		LoadPlayerSkeleton(false);
}
void CECElsePlayer::DoSkillStateAction()
{
	if (m_pPlayerModel && HasSkillStateForAction() && m_SkillIDForStateAction &&
		!m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_SKILLSTATEACT) &&
		!m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_DEAD))
	{	
		m_pEPWorkMan->StartNormalWork(new CECEPWorkSkillStateAct(m_pEPWorkMan, m_SkillIDForStateAction));
	}
	else if (m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_SKILLSTATEACT) && HasSkillStateForAction()==0)
	{
		m_pEPWorkMan->FinishRunningWork(CECEPWork::WORK_SKILLSTATEACT);
		m_SkillIDForStateAction = 0;
	}
}

bool CECElsePlayer::IsPlayerMoving(){
	return m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_MOVE)
		|| m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_FLASHMOVE)
		|| m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_PASSIVEMOVE);
}

bool CECElsePlayer::IsWorkMoveRunning()const{
	return m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_MOVE);
}

bool CECElsePlayer::IsWorkSpellRunning()const{
	return m_pEPWorkMan->IsWorkRunning(CECEPWork::WORK_SPELL);
}

bool CECElsePlayer::IsClickBoothBar( int x, int y )
{
	if(m_bRenderName && m_pPateBooth->HasPateImage()){
		return m_pPateBooth->IsMouseInRect(x,y);
	}
	return false;
}

void CECElsePlayer::SetMouseOnBoothFlag( bool IsHover )
{
	m_pPateBooth->SetIsHover(IsHover);
}

int CECElsePlayer::GetCertificateID()const{
	return m_aNewEquips[EQUIPIVTR_CERTIFICATE] & 0xffff;
}
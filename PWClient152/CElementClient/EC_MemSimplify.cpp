/*
 * FILE: EC_MemSimplify.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/4/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "EC_MemSimplify.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_ManPlayer.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_ElsePlayer.h"
#include "EC_HostPlayer.h"
#include "EC_Friend.h"
#include "EC_Team.h"
#include "EC_Faction.h"
#include "EC_UIConfigs.h"
#include "EC_Optimize.h"

#include "DlgSettingVideo.h"
#include <psapi.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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
enum MEMUSAGE_NOMODEL_UNIMPORTANT_MASK	//	MEMUSAGE_NOMODEL_UNIMPORTANT ״̬��ʹ�õ� Mask
{
	NOMODEL_UNIMPORTANT_RELOADING_MODELS	=	0x0001,	//	�ѵ��� CECElsePlayer::ReloadModels
};

enum MEMUSAGE_NOMODEL_MASK				//	MEMUSAGE_NOMODEL ״̬��ʹ�õ� Mask
{
	NOMODEL_RELOADING_MODELS				=	0x0001,	//	�ѵ��� CECElsePlayer::ReloadModels
};


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECMemSimplify
//	
///////////////////////////////////////////////////////////////////////////


// ���캯��
CECMemSimplify::CECMemSimplify()
{
	m_dwMemSize = 0;
	m_iMemUsage = MEMUSAGE_NORMAL;
	m_cntCheckMem.SetPeriod(CECUIConfig::Instance().GetGameUI().nAutoSimplifySpeed);
	m_bAutoOptimize = CECUIConfig::Instance().GetGameUI().bEnableOptimize;
	m_dwMemLow = CECUIConfig::Instance().GetGameUI().nMemoryUsageLow*1024*1024;
	m_dwMemHigh = CECUIConfig::Instance().GetGameUI().nMemoryUsageHigh*1024*1024;
}

// ��ȡ��ҵ���Ҫ�ȼ�
int CECMemSimplify::PlayerSorter::GetImportantLevel(const CECElsePlayer* pPlayer) const
{
	int iCID = pPlayer->GetCharacterID();
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

	if( pHost->GetSelectedTarget() == iCID )
		return 9;
	else if( pHost->GetBuddyID() == iCID )
		return 8;
	else if( pHost->IsRelatedPlayer(iCID) )
		return 7;
	else if( pHost->GetPVPInfo().idDuelOpp == iCID )
		return 6;
	else if( pHost->GetTradePlayer() == iCID )
		return 5;
	else if( pHost->GetSpouse() == iCID )
		return 4;
	else if( pHost->GetTeam() && pHost->GetTeam()->GetMemberByID(iCID) )
		return 3;
	else if( pHost->GetFriendMan()->GetFriendByID(iCID) )
		return 2;
	else if( pHost->GetFactionID() && g_pGame->GetFactionMan()->GetMember(iCID) )
		return 1;
	else
		return 0;
}

// ���ڸ���Ұ�����Ҫ�̶�����
bool CECMemSimplify::PlayerSorter::operator () (const CECMemSimplify::PlayerQueueNode &lhs, const CECMemSimplify::PlayerQueueNode &rhs) const
{
	const CECElsePlayer* rhs1 = lhs.m_pPlayer;
	const CECElsePlayer* rhs2 = rhs.m_pPlayer;
	int iImportantLvl1 = GetImportantLevel(rhs1);
	int iImportantLvl2 = GetImportantLevel(rhs2);

	if( iImportantLvl1 != iImportantLvl2 )
	{
		return iImportantLvl1 < iImportantLvl2;
	}
	else
	{
		A3DVECTOR3 vHostPos = g_pGame->GetGameRun()->GetHostPlayer()->GetPos();
		float fDist1 = (vHostPos - rhs1->GetPos()).SquaredMagnitude();
		float fDist2 = (vHostPos - rhs2->GetPos()).SquaredMagnitude();
		if( fabs(fDist1 - fDist2) > 0.0001f )
			return fDist1 > fDist2;
		else
			return rhs1 < rhs2;
	}
}

// ��Ӧ��ҽ�������
void CECMemSimplify::OnPlayerEnter(CECElsePlayer* pPlayer)
{
	if( !pPlayer )
		return;

	pPlayer->SetMemUsage(m_iMemUsage);
	m_PlayerQueue[m_iMemUsage].push_back(pPlayer);
}

// ��Ӧ����뿪����
void CECMemSimplify::OnPlayerLeave(CECElsePlayer* pPlayer)
{
	for( int i=MEMUSAGE_NORMAL;i<MEMUSAGE_NUM;i++ )
	{
		PlayerQueue::iterator it = std::find(m_PlayerQueue[i].begin(), m_PlayerQueue[i].end(), pPlayer);
		if( it != m_PlayerQueue[i].end() )
		{
			m_PlayerQueue[i].erase(it);
			break;
		}
	}
}

// ���µ�ǰ״̬
void CECMemSimplify::Tick(DWORD dwDeltaTime)
{
	// ֻ����Ϸ״̬�����Ż�
	if( g_pGame->GetGameRun()->GetGameState() != CECGameRun::GS_GAME )
		return;

	if( m_bAutoOptimize && m_cntCheckMem.IncCounter(dwDeltaTime) )
	{
		// ��ȡ��ǰ�����ڴ�
		if (CECOptimize::Instance().GetVirtualSize(m_dwMemSize)){
			if( m_dwMemSize < m_dwMemLow )
			{
				for( int i=MEMUSAGE_NUM-1;i>MEMUSAGE_NORMAL;i-- )
				{
					if( m_PlayerQueue[i].empty() )
						continue;
					
					std::sort(m_PlayerQueue[i].begin(), m_PlayerQueue[i].end(), PlayerSorter());
					TransferUsage((*m_PlayerQueue[i].rbegin()).m_pPlayer, i-1);
					break;
				}
			}
			else if( m_dwMemSize > m_dwMemHigh )
			{
				for( int i=0;i<MEMUSAGE_NUM-1;i++ )
				{
					if( m_PlayerQueue[i].empty() )
						continue;
					
					std::sort(m_PlayerQueue[i].begin(), m_PlayerQueue[i].end(), PlayerSorter());
					TransferUsage((*m_PlayerQueue[i].begin()).m_pPlayer, i+1);
					break;
				}
			}
		}

		m_cntCheckMem.Reset();
	}

	for( int i=0;i<MEMUSAGE_NUM;i++ )
	{
		if( !m_PlayerQueue[i].empty() )
		{
			m_iMemUsage = i;
			break;
		}
	}

	// ������ģ��״̬�����
 	UpdatePlayerModels();
}

// ������Ϸ״̬ʱ����
void CECMemSimplify::OnEndGameState()
{
	m_iMemUsage = MEMUSAGE_NORMAL;
	m_bAutoOptimize = CECUIConfig::Instance().GetGameUI().bEnableOptimize;

	for( int i=MEMUSAGE_NORMAL;i<MEMUSAGE_NUM;i++ )
	{
		m_PlayerQueue[i].clear();
	}
}

// �л�������ҵ�ָ�����ڴ�״̬
void CECMemSimplify::TransferUsageAll(int iNewState)
{
	// �Զ�ģʽ�������ֶ��л�״̬
	if( m_bAutoOptimize )
		return;
	if (iNewState < 0 || iNewState >= MEMUSAGE_NUM){
		ASSERT(false);
		return;
	}

	CECPlayerMan* pPlayerMan = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
	if( !pPlayerMan ) return;

	CECPlayerMan::PlayerTable::iterator it;
	CECPlayerMan::PlayerTable& playerTab = pPlayerMan->GetPlayerTable();
	for( it=playerTab.begin();it!=playerTab.end();++it )
	{
		TransferUsage(*it.value(), iNewState);
	}

	m_iMemUsage = iNewState;
}

// �л���ҵ��ڴ�״̬
void CECMemSimplify::TransferUsage(CECElsePlayer* pPlayer, int iNewUsage)
{
	static FN_TRANSFER_USAGE pfnTransfer[MEMUSAGE_NUM][MEMUSAGE_NUM] =
	{
		{ NULL, &CECMemSimplify::ToggleFaceMode, &CECMemSimplify::ToggleSimpleModel, &CECMemSimplify::ToggleNoModelNoImportant, &CECMemSimplify::ToggleNoModel },					// MEMUSAGE_NORMAL -> ...
		{ &CECMemSimplify::ToggleFaceMode, NULL, &CECMemSimplify::ToggleSimpleModel, &CECMemSimplify::ToggleNoModelNoImportant, &CECMemSimplify::ToggleNoModel },					// MEMUSAGE_NOFACE -> ...
		{ &CECMemSimplify::ToggleSimpleModel, &CECMemSimplify::ToggleSimpleModel, NULL, &CECMemSimplify::ToggleNoModelNoImportant, &CECMemSimplify::ToggleNoModel },				// MEMUSAGE_SIMPLEMODEL -> ...
		{ &CECMemSimplify::ToggleNoModelNoImportant, &CECMemSimplify::ToggleNoModelNoImportant, &CECMemSimplify::ToggleNoModelNoImportant, NULL , &CECMemSimplify::ToggleNoModel},	// MEMUSAGE_NOMODEL_UNIMPORTANT -> ...
		{ &CECMemSimplify::ToggleNoModel, &CECMemSimplify::ToggleNoModel, &CECMemSimplify::ToggleNoModel, &CECMemSimplify::ToggleNoModel, NULL },									// MEMUSAGE_NOMODEL -> ...
	};
	
	if( !pPlayer || iNewUsage < MEMUSAGE_NORMAL || iNewUsage >= MEMUSAGE_NUM )
	{
		ASSERT(0);
		return;
	}

	int iOldUsage = pPlayer->GetMemUsage();
	FN_TRANSFER_USAGE pFunc = pfnTransfer[iOldUsage][iNewUsage];
	if( pFunc )
	{
		PlayerQueue::iterator it = std::find(m_PlayerQueue[iOldUsage].begin(), m_PlayerQueue[iOldUsage].end(), pPlayer);
		if( it != m_PlayerQueue[iOldUsage].end() )
			m_PlayerQueue[iOldUsage].erase(it);

		pPlayer->SetMemUsage(iNewUsage);
		(this->*pFunc)( pPlayer );
		m_PlayerQueue[iNewUsage].push_back(pPlayer);
	}
}

// �л��Զ�������ģʽ
void CECMemSimplify::ToggleFaceMode(CECElsePlayer* pPlayer)
{
	if( !pPlayer )
	{
		ASSERT(0);
		return;
	}

	if( !pPlayer->ShouldUseFaceModel() )
	{
		if( pPlayer->GetFaceModel() )
			pPlayer->ReleaseFaceModel();
	}
	else
	{
		if( !pPlayer->GetFaceModel() && pPlayer->GetMajorModel() )
			pPlayer->QueueLoadFace(false);
	}
}

// �л���ģ��ģʽ
void CECMemSimplify::ToggleSimpleModel(CECElsePlayer* pPlayer)
{
	if( !pPlayer )
	{
		ASSERT(0);
		return;
	}

	pPlayer->SwitchSimpleModel();
	if( !pPlayer->ShouldUseClothedModel() )
	{
		// ������ģʽ�л�����ģ��ģʽ��������Face
		if( !pPlayer->ShouldUseFaceModel() && pPlayer->GetFaceModel() )
			pPlayer->ReleaseFaceModel();
	}
	else
	{
		// �ɼ�ģ��ģʽ�л�������ģʽ�������Face
		if( pPlayer->ShouldUseFaceModel() && !pPlayer->GetFaceModel() && pPlayer->GetMajorModel() )
			pPlayer->QueueLoadFace(false);
	}
}

// �л�����Ҫ�����ģ��ģʽ
void CECMemSimplify::ToggleNoModelNoImportant(CECElsePlayer* pPlayer)
{
	if( !pPlayer )
	{
		ASSERT(0);
		return;
	}

	if( !pPlayer->IsBaseInfoReady() || !pPlayer->IsCustomDataReady() ||	!pPlayer->IsEquipDataReady() )
		return;

	int iCurUsage = pPlayer->GetMemUsage();
	if( iCurUsage == MEMUSAGE_NORMAL || iCurUsage == MEMUSAGE_NOFACE )
	{
		if( pPlayer->GetPlayerModel() )
		{
			ToggleSimpleModel(pPlayer);
		}
		else
		{
			pPlayer->ReleaseModels();
			pPlayer->LoadPlayerSkeleton(false);
		}

		pPlayer->ReloadGoblin();
		pPlayer->UpdateGodEvilSprite();
	}
	else if( iCurUsage == MEMUSAGE_SIMPLEMODEL )
	{
		if( !pPlayer->GetPlayerModel() )
		{
			pPlayer->ReleaseModels();
			pPlayer->LoadPlayerSkeleton(false);
		}
	}
}

// �л����������ģ��ģʽ
void CECMemSimplify::ToggleNoModel(CECElsePlayer* pPlayer)
{
	ToggleNoModelNoImportant(pPlayer);
}

// �������ģ��״̬
void CECMemSimplify::UpdatePlayerModels()
{
	LoadUnloadByImportance(MEMUSAGE_NOMODEL_UNIMPORTANT, NOMODEL_UNIMPORTANT_RELOADING_MODELS, &CECMemSimplify::IsImportant);
	LoadUnloadByImportance(MEMUSAGE_NOMODEL, NOMODEL_RELOADING_MODELS, &CECMemSimplify::IsMostImportant);
}

void CECMemSimplify::LoadUnloadByImportance(int queueIndex, DWORD dwReloadingMask, ImportanceFunction isImportant){	
	PlayerQueue::iterator it;
	for (it = m_PlayerQueue[queueIndex].begin(); it != m_PlayerQueue[queueIndex].end(); ++ it){		
		PlayerQueueNode &node = *it;
		CECElsePlayer* pPlayer = node.m_pPlayer;
		if ((this->*isImportant)(pPlayer)){
			if (pPlayer->GetPlayerModel() && pPlayer->IsClothesOn()){
				// ʹ�ü�ģ��
				pPlayer->SwitchSimpleModel();
				// ɾ���Զ�������
				if (pPlayer->GetFaceModel()){
					pPlayer->ReleaseFaceModel();
				}
				node.Unmask(dwReloadingMask);
			}else{
				// ��Ҫ��������ģ��
				if (!pPlayer->IsAllResReady()){
					if (!node.HasMask(dwReloadingMask)){
						pPlayer->ReloadModels();
						node.Mask(dwReloadingMask);
					}
				}else{
					node.Unmask(dwReloadingMask);
				}
			}
		}else{
			if (pPlayer->GetPlayerModel()){
				pPlayer->ReleaseModels();
			}			
			node.Unmask(dwReloadingMask);
		}
	}
}

// �Ƿ���Ҫ���
bool CECMemSimplify::IsImportant(const CECPlayer* pPlayer ) const
{
	if (!pPlayer){
		ASSERT(0);
		return false;
	}
	if (!pPlayer->IsElsePlayer()){
		return true;
	}

	int iCID = pPlayer->GetCharacterID();
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if( pHost->GetSelectedTarget() == iCID ||
		pHost->IsRelatedPlayer(iCID) ||
		pHost->GetBuddyID() == iCID || pHost->GetCandBuddyID() == iCID ||
		pHost->GetPVPInfo().idDuelOpp == iCID ||
		pHost->GetSpouse() == iCID ||
		pHost->GetTradePlayer() == iCID ||
		pHost->GetTeam() != NULL && pHost->GetTeam()->GetMemberByID(iCID) != NULL
		)
		return true;
	else
		return false;
}

// �Ƿ�����Ҫ���
bool CECMemSimplify::IsMostImportant(const CECPlayer* pPlayer ) const
{
	if (!pPlayer){
		ASSERT(0);
		return false;
	}
	if (!pPlayer->IsElsePlayer()){
		return true;
	}
	
	int iCID = pPlayer->GetCharacterID();
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if( pHost->GetSelectedTarget() == iCID ||
		pHost->GetBuddyID() == iCID || pHost->GetCandBuddyID() == iCID ||
		pHost->GetPVPInfo().idDuelOpp == iCID)
		return true;
	else
		return false;
}

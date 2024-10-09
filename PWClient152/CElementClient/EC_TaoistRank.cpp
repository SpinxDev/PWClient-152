// Filename	: EC_SkillPanel.h
// Creator	: zhangyitian
// Date		: 2014/07/01
#include "EC_TaoistRank.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"

extern CECGame* g_pGame;

// 所有修真的ID
const int TaoistRankIDs[CECTaoistRank::TotalRankCount] = 
{0, 1, 2, 3, 4, 5, 6, 7, 8, 20, 21, 22, 30, 31, 32};

CECTaoistRank CECTaoistRank::s_allTaoistRanks[CECTaoistRank::TotalRankCount];

CECTaoistRank::CECTaoistRank() {

}

void CECTaoistRank::init() {
	static bool initComplete = false;
	if (initComplete) {
		return;
	}
	for (int i = 0; i < TotalRankCount; i++) {
		s_allTaoistRanks[i].m_id = TaoistRankIDs[i];
		s_allTaoistRanks[i].m_name = 
			g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetStringFromTable(1001 + s_allTaoistRanks[i].m_id);
		if (i != BaseRankCount - 1 && 
			i != BaseRankCount + GodRankCount - 1 && 
			i != TotalRankCount - 1) {
			s_allTaoistRanks[i].m_next = &s_allTaoistRanks[i + 1];
		} else {
			s_allTaoistRanks[i].m_next = NULL;
		}
	}
	initComplete = true;
}

const CECTaoistRank* CECTaoistRank::GetBaseRankBegin() {
	init();
	return &s_allTaoistRanks[0];
}

const CECTaoistRank* CECTaoistRank::GetBaseRankEnd() {
	init();
	return GetLastBaseRank()->m_next;
}

const CECTaoistRank* CECTaoistRank::GetLastBaseRank() {
	init();
	return &s_allTaoistRanks[BaseRankCount - 1];
}

const CECTaoistRank* CECTaoistRank::GetGodRankBegin() {
	init();
	return &s_allTaoistRanks[BaseRankCount];
}

const CECTaoistRank* CECTaoistRank::GetGodRankEnd() {
	init();
	return GetLastGodRank()->m_next;
}

const CECTaoistRank* CECTaoistRank::GetLastGodRank() {
	init();
	return &s_allTaoistRanks[BaseRankCount + GodRankCount - 1];
}

const CECTaoistRank* CECTaoistRank::GetEvilRankBegin() {
	init();
	return &s_allTaoistRanks[BaseRankCount + GodRankCount];
}

const CECTaoistRank* CECTaoistRank::GetEvilRankEnd() {
	init();
	return GetLastEvilRank()->m_next;
}

const CECTaoistRank* CECTaoistRank::GetLastEvilRank() {
	init();
	return &s_allTaoistRanks[TotalRankCount - 1];
}

const CECTaoistRank* CECTaoistRank::GetTaoistRank(int id) {
	init();
	for (int i = 0; i < TotalRankCount; i++) {
		if (TaoistRankIDs[i] == id) {
			return &s_allTaoistRanks[i];
		}
	}
	return NULL;
}

const CECTaoistRank* CECTaoistRank::GetNext() const {
	return m_next;
}

int CECTaoistRank::GetID() const {
	return m_id;
}

ACString CECTaoistRank::GetName() const {
	return m_name;
}

int CECTaoistRank::GetTotalTaoistRankCount() {
	return TotalRankCount;
}

int CECTaoistRank::GetBaseTaoistRankCount() {
	return BaseRankCount;
}

int CECTaoistRank::GetGodTaoistRankCount() {
	return GodRankCount;
}

int CECTaoistRank::GetEvilTaoistRankCount() {
	return EvilRankCount;
}

bool CECTaoistRank::IsEvilRank() const {
	const CECTaoistRank* EvilRank;
	for (EvilRank = GetEvilRankBegin(); EvilRank != GetEvilRankEnd(); 
	EvilRank = EvilRank->GetNext()) {
		if (this == EvilRank) {
			return true;
		}
	}
	return false;
}

bool CECTaoistRank::IsGodRank() const {
	const CECTaoistRank* GodRank;
	for (GodRank = GetGodRankBegin(); GodRank != GetGodRankEnd(); 
	GodRank = GodRank->GetNext()) {
		if (this == GodRank) {
			return true;
		}
	}
	return false;
}

bool CECTaoistRank::IsBaseRank() const {
	return !IsGodRank() && !IsEvilRank();
}
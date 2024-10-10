// Filename	: EC_PlayerLevelRankRealmChangeCheck.cpp
// Creator	: zhangyitian
// Date		: 2014/09/10

#include "EC_PlayerLevelRankRealmChangeCheck.h"
#include "EC_HostPlayer.h"
#include "EC_UIHelper.h"

CECPlayerLevelRankRealmChangeCheck& CECPlayerLevelRankRealmChangeCheck::Instance() {
	static CECPlayerLevelRankRealmChangeCheck s_changeCheck;
	return s_changeCheck;
}

CECPlayerLevelRankRealmChangeCheck::CECPlayerLevelRankRealmChangeCheck() {
	m_bInitialized = false;
}

CECPlayerLevelRankRealmChangeCheck::~CECPlayerLevelRankRealmChangeCheck() {

}

void CECPlayerLevelRankRealmChangeCheck::Initialize() {
	m_bInitialized = true;

	CECHostPlayer* pHost = CECUIHelper::GetHostPlayer();
	m_level = pHost->GetBasicProps().iLevel;
	m_rank = pHost->GetBasicProps().iLevel2;
	m_maxLevel = pHost->GetMaxLevelSofar();
	m_reincarnation = pHost->GetReincarnationCount();
	m_realmLevel = pHost->GetRealmLevel();
}

void CECPlayerLevelRankRealmChangeCheck::Release() {
	m_bInitialized = false;
}

void CECPlayerLevelRankRealmChangeCheck::Tick() {

	if (!m_bInitialized) {
		return;
	}

	CECHostPlayer* pHost = CECUIHelper::GetHostPlayer();
	unsigned int level = pHost->GetBasicProps().iLevel;
	unsigned int rank = pHost->GetBasicProps().iLevel2;
	unsigned int maxLevel = pHost->GetMaxLevelSofar();
	unsigned int reincarnation = pHost->GetReincarnationCount();
	unsigned int realmLevel = pHost->GetRealmLevel();

	if (level != m_level) {
		CECPlayerLevelRankRealmChange change(CECPlayerLevelRankRealmChange::LEVEL_CHANGE, m_level, level);
		NotifyObservers(&change);
		m_level = level;
	}
	if (rank != m_rank) {
		CECPlayerLevelRankRealmChange change(CECPlayerLevelRankRealmChange::RANK_CHANGE, m_rank, rank);
		NotifyObservers(&change);
		m_rank = rank;
	}
	if (maxLevel != m_maxLevel) {
		CECPlayerLevelRankRealmChange change(CECPlayerLevelRankRealmChange::MAX_LEVEL_CHANGE, m_maxLevel, maxLevel);
		NotifyObservers(&change);
		m_maxLevel = maxLevel;
	}
	if (reincarnation != m_reincarnation) {
		CECPlayerLevelRankRealmChange change(CECPlayerLevelRankRealmChange::REINCARNATION_CHANGE, m_reincarnation, reincarnation);
		NotifyObservers(&change);
		m_reincarnation = reincarnation;
	}
	if (realmLevel != m_realmLevel) {
		CECPlayerLevelRankRealmChange change(CECPlayerLevelRankRealmChange::REALM_LEVEL_CHANGE, m_realmLevel, realmLevel);
		NotifyObservers(&change);
		m_realmLevel = realmLevel;
	}
}


// Filename	: EC_PlayerLevelRankRealmChangeCheck.h
// Creator	: zhangyitian
// Date		: 2014/09/10

#ifndef _ELEMENTCLIENT_EC_PLAYERLEVELRANKREALMCHANGE_H_
#define _ELEMENTCLIENT_EC_PLAYERLEVELRANKREALMCHANGE_H_

#include "EC_Observer.h"

// 检查玩家的一些不易变的基本信息，包括等级、修真、境界、声望是否变化

class CECPlayerLevelRankRealmChange : public CECObservableChange {
public:
	enum enumChangeMask {
		LEVEL_CHANGE,
		MAX_LEVEL_CHANGE,
		RANK_CHANGE,
		REALM_LEVEL_CHANGE,
		REINCARNATION_CHANGE,
	};
	CECPlayerLevelRankRealmChange(enumChangeMask mask, unsigned int oldValue, unsigned int newValue) {
		m_changeMask = mask;
		m_oldValue = oldValue;
		m_newValue = newValue;
	}
public:
	enumChangeMask m_changeMask;
	unsigned int m_oldValue;
	unsigned int m_newValue;
};

class CECPlayerLevelRankRealmChangeCheck : public CECObservable<CECPlayerLevelRankRealmChangeCheck> {
public:
	void Initialize();
	void Release();
	void Tick();
	~CECPlayerLevelRankRealmChangeCheck();
	static CECPlayerLevelRankRealmChangeCheck& Instance();
private:
	CECPlayerLevelRankRealmChangeCheck();
	CECPlayerLevelRankRealmChangeCheck(CECPlayerLevelRankRealmChangeCheck&);
	CECPlayerLevelRankRealmChangeCheck& operator = (CECPlayerLevelRankRealmChangeCheck&);
private:
	unsigned int m_level;
	unsigned int m_maxLevel;
	unsigned int m_rank;
	unsigned int m_realmLevel;
	unsigned int m_reincarnation;
	bool m_bInitialized;
};



#endif
// Filename	: EC_ComboSkillState.h
// Creator	: zhangyitian
// Date		: 2014/8/11

#include "EC_ComboSkillState.h"
#include "EC_RoleTypes.h"
#include "EC_Skill.h"
#include "EC_TimeSafeChecker.h"
#include "EC_UIHelper.h"
#include "EC_HostPlayer.h"
#include "EC_TaoistRank.h"
#include "EC_UIHelper.h"
#include "EC_HostSkillModel.h"

#include <algorithm>
#include <AScriptFile.h>

CECComboSkillState& CECComboSkillState::Instance() {
	static CECComboSkillState state;
	return state;
}

CECComboSkillState::CECComboSkillState() {
	m_bHasInitStaticData = false;
}

CECComboSkillState::~CECComboSkillState() {
	Release();
	for (std::map<unsigned int, CECSkill*>::iterator itr = m_inherentSkillMap.begin();
	itr != m_inherentSkillMap.end(); ++itr) {
		delete itr->second;
	}
	m_inherentSkillMap.clear();
}

void CECComboSkillState::Initialize() {
	if (!m_bHasInitStaticData) {
		GNET::ElementSkill::InitStaticData();
		//InitQuickKeyMap();
		InitPreSkills();
		InitInherentSkills();
		m_bHasInitStaticData = true;
	}
}

void CECComboSkillState::Release() {
	m_activeSkills.clear();
	m_dwStartTime = 0;
	m_skillStayTime.clear();
}

CECSkill* CECComboSkillState::GetInherentSkillByID(unsigned int skillID) {
	std::map<unsigned int, CECSkill*>::iterator itr = m_inherentSkillMap.find(skillID);
	if (itr != m_inherentSkillMap.end()) {
		return itr->second;
	} else {
		return NULL;
	}
}

const std::map<unsigned int, CECSkill*>& CECComboSkillState::GetInherentSkillMap() {
	return m_inherentSkillMap;
}

bool CECComboSkillState::IsAnyComboSkillActive() {
	return std::find(m_skillTimeOut.begin(), m_skillTimeOut.end(), false) != m_skillTimeOut.end();
}

bool CECComboSkillState::IsActiveComboSkill(unsigned int skillID) {
	for (int i = 0; i < m_activeSkills.size(); i++) {
		if (m_activeSkills[i] == skillID && m_skillTimeOut[i] == false) {
			return true;
		}
	}
	return false;
}

void CECComboSkillState::Tick() {
	for (std::map<unsigned int, CECSkill*>::iterator itr = m_inherentSkillMap.begin();
	itr != m_inherentSkillMap.end(); ++itr) {
		itr->second->Tick();
	}

	if (IsAnyComboSkillActive()) {
		DWORD dwTimePassed = CECTimeSafeChecker::ElapsedTimeFor(m_dwStartTime);
		for (int i = 0; i < m_skillTimeOut.size(); i++) {
			if (!m_skillTimeOut[i] && dwTimePassed >= m_skillStayTime[i]) {
				// 单个技能时间到
				m_skillTimeOut[i] = true;
				// 检查是否全部技能都结束了
				if (!IsAnyComboSkillActive()) {
					m_dwStartTime = 0;
					m_activeSkills.clear();
					m_skillStayTime.clear();
					m_skillTimeOut.clear();
					CECComboSkillChange change(CECComboSkillChange::COMBOSKILL_STOP);
					NotifyObservers(&change);
				}
			}
		}
	}
}

// 返回最原始的具有前置技能的初级技能；如果初级技能不止一个，则返回第一个
int CECComboSkillState::GetFirstChildRecursively(int iSkillID) {
	std::vector<std::pair<unsigned int, int> > vecJuniors = CECHostSkillModel::Instance().GetJunior(iSkillID);
	if (vecJuniors.empty()) {
		return iSkillID;
	} else {
		for (int i = 0; i < vecJuniors.size(); i++) {
			int iComboPreSkill = GNET::ElementSkill::GetComboSkPreSkill(vecJuniors[i].first);
			if (iComboPreSkill != 0) {
				return GetFirstChildRecursively(vecJuniors[i].first);
			}
		}	
		return iSkillID;
	}
}

void CECComboSkillState::SetActiveComboSkills(const std::vector<std::pair<unsigned int, int> >& list) {

	std::vector<std::pair<unsigned int, int> > newList;
	FilterComboSkills(list, newList);

	m_activeSkills.clear();
	m_skillStayTime.clear();
	m_skillTimeOut.clear();
	std::vector<unsigned int> juniorSkills;
	int i;
	for (i = 0; i < newList.size(); i++) {
		int skillID = newList[i].first;
		int skillTime = newList[i].second;
		m_activeSkills.push_back(skillID);
		m_skillStayTime.push_back(skillTime);
		m_skillTimeOut.push_back(false);
		if (m_inherentSkillMap.find(skillID) == m_inherentSkillMap.end()) {
			juniorSkills.push_back(GetFirstChildRecursively(skillID));
		} else {
			juniorSkills.push_back(skillID);
		}
	
	}
	// 冒泡排序：有后续技能的放在前面；最原始的初级技能id小的放在前面
	int j;
	for (i = m_activeSkills.size() - 1; i > 0; i--) {
		for (j = 0; j < i; j++) {
			bool bPreSkill1 = IsComboPreSkill(juniorSkills[j]);
			bool bPreSkill2 = IsComboPreSkill(juniorSkills[j + 1]);
			if ((!bPreSkill1 && bPreSkill2) || (bPreSkill1 == bPreSkill2 && juniorSkills[j] > juniorSkills[j + 1])) {
				std::swap(m_activeSkills[j], m_activeSkills[j + 1]);
				std::swap(m_skillStayTime[j], m_skillStayTime[j + 1]);
				std::swap(juniorSkills[j], juniorSkills[j + 1]);
			}
		}
	}
}

void CECComboSkillState::FilterComboSkills(const std::vector<std::pair<unsigned int, int> >& inList, std::vector<std::pair<unsigned int, int> >& outList) {
	outList.clear();
	const CECTaoistRank* pCurTaoistRank = CECTaoistRank::GetTaoistRank(CECUIHelper::GetHostPlayer()->GetBasicProps().iLevel2);
	CECHostSkillModel& hostSkillModel = CECHostSkillModel::Instance();
	int i;
	for (i = 0; i < inList.size(); i++) {
		unsigned int uiSkillID = inList[i].first;
		if (m_inherentSkillMap.find(uiSkillID) == m_inherentSkillMap.end()) {
			// 如果不是天生技能，则执行过滤
			CECHostSkillModel::enumEvilGod skillEvilGod = hostSkillModel.GetSkillEvilGod(uiSkillID);
			CECHostSkillModel::enumSkillLearnedState skillLearnState = hostSkillModel.GetSkillLearnedState(uiSkillID);
			// 如果该技能已被覆盖，则去掉
			if (CECHostSkillModel::SKILL_OVERRIDDEN == skillLearnState) {
				continue;
			}
			// 如果是魔技能且人物修真为普通或仙，则去掉
			if (CECHostSkillModel::SKILL_EVIL == skillEvilGod) {
				if (pCurTaoistRank->IsBaseRank() || pCurTaoistRank->IsGodRank()) {
					continue;
				}
			}
			// 如果是仙技能且人物修真为普通或魔，则去掉
			if (CECHostSkillModel::SKILL_GOD == skillEvilGod) {
				if (pCurTaoistRank->IsBaseRank() || pCurTaoistRank->IsEvilRank()) {
					continue;
				}
			}
			// 如果是仙魔技能且未学习，则去掉
			if (CECHostSkillModel::SKILL_GOD == skillEvilGod || CECHostSkillModel::SKILL_EVIL) {
				if (CECHostSkillModel::SKILL_NOT_LEARNED == skillLearnState) {
					continue;
				}
			}
		}
		outList.push_back(inList[i]);
	}

}

const std::vector<unsigned int>& CECComboSkillState::GetCurComboSkills() {
	return m_activeSkills;
}

const std::vector<DWORD>& CECComboSkillState::GetSkillStayTime() {
	return m_skillStayTime;
}

const DWORD CECComboSkillState::GetStartTime() {
	return m_dwStartTime;
}

void CECComboSkillState::SetComboSkillState(const std::vector<std::pair<unsigned int, int> >& skillList, const GNET::ComboSkillState& state) {
	m_comboSkillState = state;
	if (state.skillid != 0) {
		m_dwStartTime = GetTickCount();
		SetActiveComboSkills(skillList);
		CECComboSkillChange change(CECComboSkillChange::COMBOSKILL_ACTIVE);
		NotifyObservers(&change);
	} else {
		m_dwStartTime = 0;
		m_activeSkills.clear();
		m_skillStayTime.clear();
		m_skillTimeOut.clear();
		CECComboSkillChange change(CECComboSkillChange::COMBOSKILL_STOP);
		NotifyObservers(&change);
	}
}

const GNET::ComboSkillState& CECComboSkillState::GetComboSkillState() {
	return m_comboSkillState;
}

/*
void CECComboSkillState::InitQuickKeyMap() {
	AScriptFile quickKeyFile;
	if (!quickKeyFile.Open("Configs\\comboskill_quickkey.txt")) {
		a_LogOutput(1, "CECComboSkillState::InitQuickKeyMap, failed to open file comboskill_quickkey.txt");
		return;
	}
	while (quickKeyFile.PeekNextToken(true)) {
		unsigned int first = (unsigned int)quickKeyFile.GetNextTokenAsInt(true);
		int second = quickKeyFile.GetNextTokenAsInt(false);
		enumComboSkillQuickKey comboSkillQuickKey;

		if (second == 1) {
			comboSkillQuickKey = QUICKKEY_Q;
		} else if (second == 2) {
			comboSkillQuickKey = QUICKKEY_E;
		} else if (second == 3) {
			comboSkillQuickKey = QUICKKEY_R;
		} else {
			comboSkillQuickKey = QUICKKEY_UNKNOWN;
		}

		m_quickKeyMap[first] = comboSkillQuickKey;
	}
	quickKeyFile.Close();
}
*/
/*
CECComboSkillState::enumComboSkillQuickKey CECComboSkillState::GetQuickKeyBySkillID(unsigned int skillID) {
	std::map<unsigned int, enumComboSkillQuickKey>::iterator itr = m_quickKeyMap.find(skillID);
	if (itr != m_quickKeyMap.end()) {
		return itr->second;
	} else {
		return QUICKKEY_UNKNOWN;
	}
}
*/

void CECComboSkillState::InitPreSkills() {
	int ID = 0;
	while (ID = GNET::ElementSkill::NextSkill(ID)) {
		int iComboPreSkill = GNET::ElementSkill::GetComboSkPreSkill(ID);
		if (iComboPreSkill != 0) {
			m_preSkillSet.insert(iComboPreSkill);
		}
	}
}

void CECComboSkillState::InitInherentSkills() {
	int i;
	for (i = 0; i < NUM_PROFESSION; i++) {
		const std::vector<unsigned int>& skills = GNET::ElementSkill::GetInherentSkills(i);
		int j;
		for (j = 0; j < skills.size(); j++) {
			CECSkill* pSkill = new CECSkill(skills[j], 1);
			ASSERT(pSkill && m_inherentSkillMap.find(skills[j]) == m_inherentSkillMap.end());
			m_inherentSkillMap[skills[j]] = pSkill;
		}
	}
}

bool CECComboSkillState::IsComboPreSkill(unsigned int skillID) {
	return m_preSkillSet.find(skillID) != m_preSkillSet.end();
}
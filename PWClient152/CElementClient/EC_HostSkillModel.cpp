// Filename	: EC_HoskSkillModel.cpp
// Creator	: zhangyitian
// Date		: 2014/06/20

#include "EC_HostSkillModel.h"
#include "EC_ComboSkillState.h"
#include "ElementSkill.h"
#include "EC_Skill.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "elementdataman.h"
#include "EC_GameSession.h"
#include "EC_TaoistRank.h"
#include "ExpTypes.h"
#include "EC_GPDataType.h"
#include "EC_IvtrItem.h"

#include <queue>

#include "AUICTranslate.h"

// 外部变量
extern CECGame * g_pGame;

CECHostSkillModel::CECHostSkillModel() {
	m_skillLearnNPCNID = 0;
	m_bInitialized = false;
}

void CECHostSkillModel::Release() {
	abase::hash_map<int, GNET::ElementSkill*>::iterator it;
	for (it = m_allProfSkills.begin(); it != m_allProfSkills.end(); ++it) {
		it->second->Destroy();
	}
	m_allProfSkills.clear();
	m_allRankProfSkills.clear();

	m_evilRootMap.clear();
	m_godRootMap.clear();
	m_baseRootMap.clear();
	m_treeHeightMap.clear();

	m_allProfNPCs.clear();
	m_curServiceSkills.clear();

	m_skillLearnNPCNID = 0;

	m_bInitialized = false;
}

CECHostSkillModel::~CECHostSkillModel() {
	Release();
}

CECHostSkillModel& CECHostSkillModel::Instance() {
	static CECHostSkillModel skillPanelModel;
	return skillPanelModel;
}

namespace {
	bool cmpSkillOrder(int lhs, int rhs) {
		GNET::ElementSkill* lSkill = GNET::ElementSkill::Create(lhs, 1);
		GNET::ElementSkill* rSkill = GNET::ElementSkill::Create(rhs, 1);
		
		ASSERT(lSkill && rSkill);
		bool ret;
		if (lSkill->GetType() == GNET::TYPE_PASSIVE && rSkill->GetType() != GNET::TYPE_PASSIVE) {
			ret = false;
		} else if (lSkill->GetType() != GNET::TYPE_PASSIVE && rSkill->GetType() == GNET::TYPE_PASSIVE) {
			ret = true;
		} else {
			ret = lSkill->GetShowOrder() < rSkill->GetShowOrder();
		}
		lSkill->Destroy();
		rSkill->Destroy();
		return ret;
	}
}


void CECHostSkillModel::Initialize() {
	// 清空所有技能，防止因为多个角色登录导致重复加载技能
	Release();

	InitAllSkillsOfCurProf();
	FindAllNPCsOfCurProf();
	std::set<int> rootSkills = GetRootSkillSet();
	InitSkillTreeHeightMap(rootSkills);
	InitSkillTreeRootMap(rootSkills);
	
	m_bInitialized = true;
		
	// 重新处理NPCLIST
	ProcessServiceList();
}

void CECHostSkillModel::InitAllSkillsOfCurProf() {
	//
	std::set<int> npcSkills;
	{
		DATA_TYPE dt;
		elementdataman* pDB = g_pGame->GetElementDataMan();
		int id = pDB->get_first_data_id(ID_SPACE_ESSENCE, dt);
		while (id) {
			if (DT_NPC_ESSENCE == dt) {
				NPC_ESSENCE* npcEssence = (NPC_ESSENCE*)pDB->get_data_ptr(id, ID_SPACE_ESSENCE, dt);
				if (npcEssence->id_skill_service != 0 && (npcEssence->combined_switch & NCS_IGNORE_DISTANCE_CHECK)) {
					NPC_SKILL_SERVICE* skillService = (NPC_SKILL_SERVICE*)pDB->get_data_ptr(npcEssence->id_skill_service, ID_SPACE_ESSENCE, dt);
					bool profCorrect = false;
					for (int i = 0; i < sizeof(skillService->id_skills) / sizeof(skillService->id_skills[0]); i++) {
						if (skillService->id_skills[i] != 0 && 
							CECComboSkillState::Instance().GetInherentSkillByID(skillService->id_skills[i]) == NULL) {
							npcSkills.insert(skillService->id_skills[i]);
						}
					}
				}
			}
			id = pDB->get_next_data_id(ID_SPACE_ESSENCE, dt);
		}
	}
	
	
	// 遍历全部技能，找到每个修真期内的技能
	int ID = 0;
	while (ID = GNET::ElementSkill::NextSkill(ID)) {
		GNET::ElementSkill* pSkill = GNET::ElementSkill::Create(ID, 1);
		int c1 = pSkill->GetCls();
		int c2 = g_pGame->GetGameRun()->GetHostPlayer()->GetProfession();
		if ((pSkill->GetCls() == g_pGame->GetGameRun()->GetHostPlayer()->GetProfession() ||
			pSkill->GetCls() == 255) &&
			npcSkills.find(ID) != npcSkills.end()) {
			m_allProfSkills[ID] = pSkill;
			m_allRankProfSkills[pSkill->GetRank()].push_back(ID);
		} else {
			pSkill->Destroy();
		}
	}
	
	// 对各修真期内的技能排序
	for (abase::hash_map<int, abase::vector<int> >::iterator it  = m_allRankProfSkills.begin();
	it != m_allRankProfSkills.end(); ++it) {
		std::sort(it->second.begin(), it->second.end(), cmpSkillOrder);
	}
}

void CECHostSkillModel::FindAllNPCsOfCurProf() {
	DATA_TYPE dt;
	elementdataman* pDB = g_pGame->GetElementDataMan();
	int id = pDB->get_first_data_id(ID_SPACE_ESSENCE, dt);
	while (id) {
		if (DT_NPC_ESSENCE == dt) {
			NPC_ESSENCE* npcEssence = (NPC_ESSENCE*)pDB->get_data_ptr(id, ID_SPACE_ESSENCE, dt);
			if (npcEssence->id_skill_service != 0 && (npcEssence->combined_switch & NCS_IGNORE_DISTANCE_CHECK)) {
				NPC_SKILL_SERVICE* skillService = (NPC_SKILL_SERVICE*)pDB->get_data_ptr(npcEssence->id_skill_service, ID_SPACE_ESSENCE, dt);
				bool profCorrect = false;
				for (int i = 0; i < sizeof(skillService->id_skills) / sizeof(skillService->id_skills[0]); i++) {
					if (skillService->id_skills[i] != 0) {
						GNET::ElementSkill* pSkill = GNET::ElementSkill::Create(skillService->id_skills[i], 1);
						if (pSkill->GetCls() == g_pGame->GetGameRun()->GetHostPlayer()->GetProfession()) {
							// 该NPC包含当前职业技能，需要记录该NPC的ID
							profCorrect = true;
							pSkill->Destroy();
							break;
						}
						pSkill->Destroy();
					}
				}
				if (profCorrect) {
					m_allProfNPCs.insert(npcEssence->id);
				}
			}
		}
		id = pDB->get_next_data_id(ID_SPACE_ESSENCE, dt);
	}
}

void CECHostSkillModel::InitSkillTreeRootMap(const std::set<int>& rootSkills) {
	std::set<int>::iterator itr;
	for (itr = rootSkills.begin(); itr != rootSkills.end(); ++itr) {
		int rootSkillID = *itr;
		InitializeRootOfSkillTree(rootSkillID);
	}
}

void CECHostSkillModel::InitSkillTreeHeightMap(const std::set<int>& rootSkills) {
	std::set<int>::iterator itr;
	for (itr = rootSkills.begin(); itr != rootSkills.end(); ++itr) {
		int rootSkillID = *itr;
		m_treeHeightMap[rootSkillID] = GetSkillTreeHeight(rootSkillID);
	}
}

std::set<int> CECHostSkillModel::GetRootSkillSet() {
	// 找到所有的根技能：具有初级技能并且不是其它技能的初级技能
	std::set<int> rootSkills;
	{
		abase::hash_map<int, GNET::ElementSkill*>::iterator itr;
		for (itr = m_allProfSkills.begin(); itr != m_allProfSkills.end(); ++itr) {
			if (itr->second->GetJunior().size() != 0) {
				rootSkills.insert(itr->first);
			}
		}
		for (itr = m_allProfSkills.begin(); itr != m_allProfSkills.end(); ++itr) {
			std::vector<std::pair<unsigned int, int> > juniorSkills = itr->second->GetJunior();
			for (size_t i = 0; i < juniorSkills.size(); i++) {
				rootSkills.erase(juniorSkills[i].first);
			}
		}
	}
	return rootSkills;
}

ACString CECHostSkillModel::GetSkillName(int skillID) {

	ASSERT(m_allProfSkills.find(skillID) != m_allProfSkills.end());

	CECSkill skill(skillID, 1);
	return ACString(skill.GetNameDisplay());
}

AString CECHostSkillModel::GetSkillIcon(int skillID) {

	ASSERT(m_allProfSkills.find(skillID) != m_allProfSkills.end());

	CECSkill skill(skillID, 1);
	return AString(skill.GetIconFile());
}

ACString CECHostSkillModel::GetSkillDescription(int skillID, int level) {

	ASSERT(m_allProfSkills.find(skillID) != m_allProfSkills.end());

	ACHAR tmp[1024];
	if (CECSkill::GetDesc(skillID, level, tmp, 1024)) {
		return ACString(tmp);
	}

	ASSERT(false);
	return ACString();
}

int CECHostSkillModel::GetSkillSp(int skillID, int level) {

	ASSERT(m_allProfSkills.find(skillID) != m_allProfSkills.end());

	return GNET::ElementSkill::GetRequiredSp(skillID, level);
}

int CECHostSkillModel::GetSkillMoney(int skillID, int level) {

	ASSERT(m_allProfSkills.find(skillID) != m_allProfSkills.end());

	return GNET::ElementSkill::GetRequiredMoney(skillID, level);
}

int CECHostSkillModel::GetSkillCurrentLevel(int skillID) {

	ASSERT(m_allProfSkills.find(skillID) != m_allProfSkills.end());

	CECSkill* pSkill = g_pGame->GetGameRun()->GetHostPlayer()->GetNormalSkill(skillID);
	if (pSkill) {
		return pSkill->GetSkillLevel();
	} else {
		return 0;
	}
}

int CECHostSkillModel::GetSkillMaxLevel(int skillID) {

	ASSERT(m_allProfSkills.find(skillID) != m_allProfSkills.end());

	CECSkill skill(skillID, 1);
	return skill.GetMaxLevel();
}

bool CECHostSkillModel::IsPassiveSkill(int skillID) {

	ASSERT(m_allProfSkills.find(skillID) != m_allProfSkills.end());
	
	CECSkill skill(skillID, 1);
	return skill.GetType() == CECSkill::TYPE_PASSIVE;
}

CECHostSkillModel::enumSkillFitLevelState CECHostSkillModel::GetSkillFitLevel(int skillID, int maxLevel, int rank, int realmLevel) {
	ASSERT(m_allProfSkills.find(skillID) != m_allProfSkills.end());
	
	int skillLevel = 1;	//将要学习的技能级别
	CECSkill* pSkill = g_pGame->GetGameRun()->GetHostPlayer()->GetNormalSkill(skillID);
	if (pSkill) {
		skillLevel = pSkill->GetSkillLevel() + 1;
		if (skillLevel > pSkill->GetMaxLevel()) {
			return SKILL_NOT_FIT_LEVEL;
		}
	}
	
	// 使用GNET::ElementSkill类的原因是CECSkill类没有提供获得境界需求的函数
	GNET::ElementSkill* s = GNET::ElementSkill::Create(skillID, skillLevel);
	
	// 等级不够
	if (s->GetRequiredLevel() > maxLevel) {
		return SKILL_NOT_FIT_LEVEL;
	}
	// 修真等级不够
	const CECTaoistRank* curTaoistRank = CECTaoistRank::GetTaoistRank(rank);
	const CECTaoistRank* reqTaoistRank = CECTaoistRank::GetTaoistRank(s->GetRank());
	if ((curTaoistRank->IsEvilRank() && reqTaoistRank->IsGodRank()) || 
		(curTaoistRank->IsGodRank() && reqTaoistRank->IsEvilRank()) ||
		(curTaoistRank->GetID() < reqTaoistRank->GetID())) {
		return SKILL_NOT_FIT_LEVEL;
	}
	// 境界不够
	if (s->GetRequiredRealmLevel() > realmLevel) {
		return SKILL_NOT_FIT_LEVEL;
	}
	
	s->Destroy();
	return SKILL_FIT_LEVEL;
}

CECHostSkillModel::enumSkillFitLevelState CECHostSkillModel::GetSkillFitLevel(int skillID) {
	
	ASSERT(m_allProfSkills.find(skillID) != m_allProfSkills.end());

	int maxLevel = g_pGame->GetGameRun()->GetHostPlayer()->GetMaxLevelSofar();
	int rank = g_pGame->GetGameRun()->GetHostPlayer()->GetBasicProps().iLevel2;
	int realmLevel = g_pGame->GetGameRun()->GetHostPlayer()->GetRealmLevel();

	return GetSkillFitLevel(skillID, maxLevel, rank, realmLevel);
}

bool CECHostSkillModel::CheckHasNewSkillCanLearn(int oldMaxLevel, int oldRank, int oldRealmLevel) {
	abase::hash_map<int, GNET::ElementSkill*>::iterator it;
	for (it = m_allProfSkills.begin(); it != m_allProfSkills.end(); ++it) {
		if (GetSkillFitLevel(it->first, oldMaxLevel, oldRank, oldRealmLevel) == SKILL_NOT_FIT_LEVEL &&
			GetSkillFitLevel(it->first) == SKILL_FIT_LEVEL) {
			
			int skillID = it->first;
			int skillLevelToLearn = GetSkillCurrentLevel(skillID) + 1;
			int preItemID = GetRequiredBook(skillID, skillLevelToLearn);
		
			if (!preItemID || CheckPreItem(preItemID)) {
				return true;
			}
		}
	}
	return false;
}

CECHostSkillModel::enumSkillLearnedState CECHostSkillModel::GetSkillLearnedState(int skillID) {
	CECSkill* pSkill = g_pGame->GetGameRun()->GetHostPlayer()->GetNormalSkill(skillID);
	if (pSkill) {
		if (pSkill->GetSkillLevel() < pSkill->GetMaxLevel()) {
			return SKILL_LEARNED;
		} else {
			return SKILL_FULL;
		}
	} else {
		if (GNET::ElementSkill::IsOverridden(skillID)) {
			return SKILL_OVERRIDDEN;
		} else {
			return SKILL_NOT_LEARNED;
		}
	}
}

CECHostSkillModel::enumEvilGod CECHostSkillModel::GetSkillEvilGod(int skillID) {
	CECSkill skill(skillID, 1);
	int rank = skill.GetRank();
	const CECTaoistRank* taoistRank = CECTaoistRank::GetTaoistRank(rank);
	if (taoistRank->IsGodRank()) {
		return SKILL_GOD;
	} else if (taoistRank->IsEvilRank()) {
		return SKILL_EVIL;
	} else {
		return SKILL_BASE;
	}
}

int CECHostSkillModel::GetTotalSkillCount() {
	return m_allProfSkills.size();
}

int CECHostSkillModel::GetRequiredBook(int skillID, int level) {
	int itemId = GNET::ElementSkill::GetRequiredBook(skillID, level);
	return itemId;
}

std::vector<std::pair<unsigned int, int> > CECHostSkillModel::GetRequiredSkill(int skillID, int level) {
	std::vector<std::pair<unsigned int, int> > requiredSkill;
	GNET::ElementSkill* s = GNET::ElementSkill::Create(skillID, level);
	const std::vector<std::pair<unsigned int, int> > & skills = s->GetRequiredSkill();
	for (size_t i = 0; i < skills.size(); i++) {
		if (skills[i].first != 0) {
			requiredSkill.push_back(skills[i]);
		}
	}
	s->Destroy();
	return requiredSkill; 
}

int CECHostSkillModel::GetCurrentRank() {
	return g_pGame->GetGameRun()->GetHostPlayer()->GetBasicProps().iLevel2;
}

bool CECHostSkillModel::CheckPreSkillLevel(int skillID, int level) {
	if (GetSkillLearnedState(skillID) == SKILL_OVERRIDDEN) {
		// 如果该前提技能被覆盖，则前提技能一定满足
		return true;
	}
	return GetSkillCurrentLevel(skillID) >= level;
}

bool CECHostSkillModel::CheckPreItem(int itemID) {
	return g_pGame->GetGameRun()->GetHostPlayer()->GetPack()->FindItem(itemID) != -1;
}

AString CECHostSkillModel::GetPreItemIcon(int itemID) {
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	const SKILLTOME_ESSENCE* essence = 
		static_cast<const SKILLTOME_ESSENCE*>(pDB->get_data_ptr(itemID, ID_SPACE_ESSENCE, DataType));
	return essence->file_icon;
}

ACString CECHostSkillModel::GetPreItemDescription(int itemID) {
	AUICTranslate trans;
	CECIvtrItem* item = CECIvtrItem::CreateItem(itemID, 0, 1);
	item->GetDetailDataFromLocal();
	ACString desc(trans.Translate(item->GetDesc(CECIvtrItem::DESC_NORMAL)));
	delete item;
	return desc;
}

int CECHostSkillModel::CheckLearnCondition(int skillID) {
	return g_pGame->GetGameRun()->GetHostPlayer()->CheckSkillLearnCondition(skillID, true);
}

void CECHostSkillModel::OnLearnSkill(int skillID, int skillLevel) {

	if (m_allProfSkills.find(skillID) == m_allProfSkills.end()) {
		return;
	}

	// 检查学习新技能后有没有技能被覆盖
	bool newOverridden = false;
	if (!GetJunior(skillID).empty()) {
		newOverridden = true;
	}

	if (newOverridden) {
		CECSkillPanelChange change(CECSkillPanelChange::CHANGE_SKILL_OVERRIDDEN, skillID, skillLevel);
		NotifyObservers(&change);
	} else {
		CECSkillPanelChange change(CECSkillPanelChange::CHANGE_SKILL_LEVEL_UP, skillID, skillLevel);
		NotifyObservers(&change);
	}
}

std::vector<std::pair<unsigned int, int> > CECHostSkillModel::GetJunior(int skillID) {
	abase::hash_map<int, GNET::ElementSkill*>::iterator itr = m_allProfSkills.find(skillID);
	ASSERT(itr != m_allProfSkills.end());
	std::vector<std::pair<unsigned int, int> > juniors = itr->second->GetJunior();
	std::vector<std::pair<unsigned int, int> > ret;
	for (std::vector<std::pair<unsigned int, int> >::iterator jItr = juniors.begin();
	jItr != juniors.end(); ++jItr) {
		if (jItr->first != 0) {
			ret.push_back(*jItr);
		}
	}
	return ret;
}

int CECHostSkillModel::GetSkillTreeHeight(int rootSkillID) {
	std::vector<std::pair<unsigned int, int> > juniors = GetJunior(rootSkillID);
	int maxHeight = 0;
	for (size_t i = 0; i < juniors.size(); i++) {
		int subHeight = GetSkillTreeHeight(juniors[i].first);
		if (subHeight > maxHeight) {
			maxHeight = subHeight;
		}
	}
	return 1 + maxHeight;
}

void CECHostSkillModel::InitializeRootOfSkillTree(int rootSkillID) {
	SkillRootMap& skillRootMap = GetSkillRootMap(rootSkillID);
	std::queue<int> toTravelSkills;
	toTravelSkills.push(rootSkillID);
	while (!toTravelSkills.empty()) {
		int skillID = toTravelSkills.front();
		toTravelSkills.pop();
		JuniorSkillVector juniors = GetJunior(skillID);
		for (size_t i = 0; i < juniors.size(); i++) {
			int juniorSkillID = juniors[i].first;
			skillRootMap[juniorSkillID] = rootSkillID;
			toTravelSkills.push(juniorSkillID);
		}
	}
}

CECHostSkillModel::SkillRootMap& CECHostSkillModel::GetSkillRootMap(int rootSkillID) {
	const CECTaoistRank* taoistRank = CECTaoistRank::GetTaoistRank(m_allProfSkills.find(rootSkillID)->second->GetRank());
	if (taoistRank->IsEvilRank()) {
		return m_evilRootMap;
	} else if (taoistRank->IsGodRank()) {
		return m_godRootMap;
	} else {
		return m_baseRootMap;
	}
}

int CECHostSkillModel::GetRootSkill(int skillID, bool isEvil) {
	abase::hash_map<int, int>::iterator nItr = m_baseRootMap.find(skillID);
	if (nItr != m_baseRootMap.end()) {
		return nItr->second;
	}

	if (isEvil) {
		abase::hash_map<int, int>::iterator dItr = m_evilRootMap.find(skillID);
		if (dItr != m_evilRootMap.end()) {
			return dItr->second;
		}
	} else {
		abase::hash_map<int, int>::iterator iItr = m_godRootMap.find(skillID);
		if (iItr != m_godRootMap.end()) {
			return iItr->second;
		}
	}
	return skillID;
}

int CECHostSkillModel::GetTreeHeight(int rootSkillID) {
	abase::hash_map<int, int>::iterator itr = m_treeHeightMap.find(rootSkillID);
	if (itr == m_treeHeightMap.end()) {
		return 1;
	} else {
		return itr->second;
	}
}

void CECHostSkillModel::SendHelloToSkillLearnNPC() {
	if (m_skillLearnNPCNID != 0) {
		g_pGame->GetGameSession()->c2s_CmdNPCSevHello(m_skillLearnNPCNID);
	}
}

bool CECHostSkillModel::IsSkillLearnNPC(int nid) {
	return nid == m_skillLearnNPCNID;
}

bool CECHostSkillModel::IsSkillServedByNPC(int skillID) {
	return m_curServiceSkills.find(skillID) != m_curServiceSkills.end();
}

void CECHostSkillModel::RecvNPCServiceList(const GNET::Octets& Data) {

	m_npcListData = Data;

	if (!m_bInitialized) {	
		return;
	} else {
		ProcessServiceList();
	}
}

void CECHostSkillModel::ProcessServiceList() {
	if (m_npcListData.size() > 0) {
		BYTE* pDataBuf = (BYTE*)m_npcListData.begin();
		pDataBuf += sizeof(S2C::cmd_header);
		S2C::cmd_scene_service_npc_list* npcList = (S2C::cmd_scene_service_npc_list*)pDataBuf;
		int i;
		for (i = 0; i < npcList->count; i++) {
			int tid = npcList->list[i].tid;
			// 如果该NPC是符合职业的NPC
			if (m_allProfNPCs.find(tid) != m_allProfNPCs.end()) {
				// 如果该NPC与前一场景的NPC不同，则更新
				if (m_skillLearnNPCNID != npcList->list[i].nid) {
					m_skillLearnNPCNID = npcList->list[i].nid;
					SetCurServiceSkills(tid);
					CECSkillPanelChange change(CECSkillPanelChange::CHANGE_SKILL_NPC, 0, 0);
					NotifyObservers(&change);
					break;
				}
			}
		}
		if (i == npcList->count && 0 != m_skillLearnNPCNID) {
			// 当前场景没有教授服务
			m_skillLearnNPCNID = 0;
			SetCurServiceSkills(0);
			CECSkillPanelChange change(CECSkillPanelChange::CHANGE_SKILL_NPC, 0, 0);
			NotifyObservers(&change);
		}
		m_npcListData.clear();
	}
}

void CECHostSkillModel::SetCurServiceSkills(int tid) {
	m_curServiceSkills.clear();
	if (tid == 0) {
		return;
	}
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE dt;
	NPC_ESSENCE* npcEssence = (NPC_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, dt);
	NPC_SKILL_SERVICE* skillService = (NPC_SKILL_SERVICE*)pDB->get_data_ptr(npcEssence->id_skill_service, ID_SPACE_ESSENCE, dt);

	for (int i = 0; i < sizeof(skillService->id_skills) / sizeof(skillService->id_skills[0]); i++) {
		if (skillService->id_skills[i] != 0) {
			m_curServiceSkills.insert(skillService->id_skills[i]);
		}
	}
}

bool CECHostSkillModel::IsSkillLearnNPCExsit() {
	return m_skillLearnNPCNID != 0;
}

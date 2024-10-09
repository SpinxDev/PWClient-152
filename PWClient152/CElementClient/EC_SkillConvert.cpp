// Filename	: EC_SkillConvert.cpp
// Creator	: zhangyitian
// Date		: 2014/07/09

#include "EC_SkillConvert.h"

#include "ExpTypes.h"
#include "elementdataman.h"
#include "EC_Game.h"

extern CECGame * g_pGame; 

CECSkillConvert::CECSkillConvert() {
	m_bInitialized = false;
}

CECSkillConvert& CECSkillConvert::Instance() {
	static CECSkillConvert godEvilConvert;
	return godEvilConvert;
}

void CECSkillConvert::Initialize() {
	DATA_TYPE dt;
	elementdataman* pDB = g_pGame->GetElementDataMan();
	int id = pDB->get_first_data_id(ID_SPACE_CONFIG, dt);
	while (id) {
		if (DT_GOD_EVIL_CONVERT_CONFIG == dt) {
			GOD_EVIL_CONVERT_CONFIG* config = (GOD_EVIL_CONVERT_CONFIG*)pDB->get_data_ptr(id, ID_SPACE_CONFIG, dt);
			for (size_t i = 0; i < sizeof(config->skill_map) / sizeof(config->skill_map[0]); i++) {
				if (config->skill_map[i][0] != 0 &&
					config->skill_map[i][1] != 0) {
					m_convertTable[config->skill_map[i][0]] = config->skill_map[i][1];
					m_convertTable[config->skill_map[i][1]] = config->skill_map[i][0];
				}
			}
		}
		id = pDB->get_next_data_id(ID_SPACE_CONFIG, dt);
	}
	m_bInitialized = true;
}

int CECSkillConvert::GetConvertSkill(int skillID) {
	if (!m_bInitialized) {
		Initialize();
	}
	abase::hash_map<int, int>::iterator itr = m_convertTable.find(skillID);
	if (itr == m_convertTable.end()) {
		return 0;
	} else {
		return itr->second;
	}
}
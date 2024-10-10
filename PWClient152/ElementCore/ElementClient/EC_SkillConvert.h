// Filename	: EC_SkillConvert.h
// Creator	: zhangyitian
// Date		: 2014/07/09

#include "hashmap.h"

// 技能转换配置表

class CECSkillConvert {
public:
	static CECSkillConvert& Instance();		// 单例
	int GetConvertSkill(int skillID);		// 获得转换后的技能，返回0表示没有转换的技能
private:
	CECSkillConvert();
	void Initialize();
	// 禁止
	CECSkillConvert(const CECSkillConvert &);
	CECSkillConvert & operator = (const CECSkillConvert &);
private:
	abase::hash_map<int, int> m_convertTable;
	bool m_bInitialized;
};
// Filename	: EC_SkillConvert.h
// Creator	: zhangyitian
// Date		: 2014/07/09

#include "hashmap.h"

// ����ת�����ñ�

class CECSkillConvert {
public:
	static CECSkillConvert& Instance();		// ����
	int GetConvertSkill(int skillID);		// ���ת����ļ��ܣ�����0��ʾû��ת���ļ���
private:
	CECSkillConvert();
	void Initialize();
	// ��ֹ
	CECSkillConvert(const CECSkillConvert &);
	CECSkillConvert & operator = (const CECSkillConvert &);
private:
	abase::hash_map<int, int> m_convertTable;
	bool m_bInitialized;
};
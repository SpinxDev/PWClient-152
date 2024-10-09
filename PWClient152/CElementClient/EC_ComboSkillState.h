// Filename	: EC_ComboSkillState.h
// Creator	: zhangyitian
// Date		: 2014/8/11

#ifndef _ELEMENTCLIENT_EC_COMBOSKILL_STATE_H_
#define _ELEMENTCLIENT_EC_COMBOSKILL_STATE_H_

#include "EC_Observer.h"
#include "ElementSkill.h"

#include <map>
#include <set>
#include <vector>

// ������״̬��ֻ��ҹӰ����������

class CECSkill;

class CECComboSkillChange : public CECObservableChange {
public:
	enum enumComboSkillChange {
		COMBOSKILL_ACTIVE,			// ����������
		COMBOSKILL_STOP,			// ����������
	};
public:
	CECComboSkillChange(enumComboSkillChange changeMask, int skillID = 0) {
		m_changeMask = changeMask;
		m_skillID = skillID;
	}
	enumComboSkillChange m_changeMask;
	int m_skillID;
};

class CECComboSkillState : public CECObservable<CECComboSkillState> {
public:
	/*
	enum enumComboSkillQuickKey {
		QUICKKEY_Q,
		QUICKKEY_E,
		QUICKKEY_R,
		QUICKKEY_UNKNOWN,
	};
	*/
public:
	~CECComboSkillState();
	void Initialize();
	void Release();
	CECSkill* GetInherentSkillByID(unsigned int skillID);
	const std::map<unsigned int, CECSkill*>& GetInherentSkillMap();
	void Tick();

	bool IsActiveComboSkill(unsigned int skillID);
	bool IsAnyComboSkillActive();

	const std::vector<unsigned int>& GetCurComboSkills();
	const std::vector<DWORD>& GetSkillStayTime();
	const DWORD GetStartTime();

	void SetComboSkillState(const std::vector<std::pair<unsigned int, int> >& skillList, const GNET::ComboSkillState& state);
	const GNET::ComboSkillState& GetComboSkillState();

	static CECComboSkillState& Instance();

	//enumComboSkillQuickKey GetQuickKeyBySkillID(unsigned int skillID);

	bool IsComboPreSkill(unsigned int skillID);

private:
	CECComboSkillState();
	CECComboSkillState(const CECComboSkillState&);
	CECComboSkillState& operator = (const CECComboSkillState&);
	void OneSkillTimeOut(int skillID);
	void SetActiveComboSkills(const std::vector<std::pair<unsigned int, int> >& list);
	void FilterComboSkills(const std::vector<std::pair<unsigned int, int> >& inList, std::vector<std::pair<unsigned int, int> >& outList);
	int GetFirstChildRecursively(int iSkillID);
	void InitPreSkills();
	void InitInherentSkills();
	//void InitQuickKeyMap();

private:
	std::map<unsigned int, CECSkill*> m_inherentSkillMap;		// ���е���������
	std::vector<unsigned int>	m_activeSkills;		// ��ǰ����ļ���
	std::vector<DWORD> m_skillStayTime;				// ���ܼ������ʱ��
	std::vector<bool> m_skillTimeOut;				// �����Ƿ�ʱ����ֹ
	DWORD m_dwStartTime;							// ���ܼ��ʼʱ��
	std::set<unsigned int> m_preSkillSet;			// ��������������ǰ��ļ���
	
	GNET::ComboSkillState m_comboSkillState;		// ����ȫ�ֵ�ComboSkillState����

	//std::map<unsigned int, enumComboSkillQuickKey> m_quickKeyMap;

	bool m_bHasInitStaticData;
	
};




#endif

// Filename	: EC_HostSkillModel.h
// Creator	: zhangyitian
// Date		: 2014/06/20

#ifndef _ELEMENTCLIENT_EC_HOSTSKILLMODEL_H_
#define _ELEMENTCLIENT_EC_HOSTSKILLMODEL_H_

#include "EC_Observer.h"
#include "vector.h"
#include "hashmap.h"
#include "AAssist.h"
#include "gnoctets.h"

#include <vector>
#include <set>

namespace GNET {
	class ElementSkill;
}

namespace S2C {
	struct cmd_scene_service_npc_list;
}


class CECSkill;
struct NPC_ESSENCE;

/* ����״���ı��࣬����֪ͨ����仯 */
class CECSkillPanelChange : public CECObservableChange {
public:
	enum enumChangeMask {
		CHANGE_SKILL_LEVEL_UP,		// ��������
		CHANGE_SKILL_OVERRIDDEN,	// ���ܱ�����
		CHANGE_SKILL_NPC,			// �л��������ṩ���ܷ����NPC�仯
	};
	CECSkillPanelChange(enumChangeMask mask, int id, int level) {
		m_changeMask = mask;
		m_skillID = id;
		m_skillLevel = level;
	}
public:
	enumChangeMask m_changeMask;
	int m_skillID;
	int m_skillLevel;
};


/* ��������Model�� */
class CECHostSkillModel : public CECObservable<CECHostSkillModel> {
public:
	enum enumSkillFitLevelState {
		SKILL_FIT_LEVEL,		// ��������ȼ������桢��������
		SKILL_NOT_FIT_LEVEL,	// ���ܲ�����ȼ������桢��������
	};
	enum enumSkillLearnedState {
		SKILL_NOT_LEARNED,	// ����δѧϰ
		SKILL_LEARNED,		// ������ѧϰ����������
		SKILL_FULL,			// ����������
		SKILL_OVERRIDDEN,	// �����ѱ�����
	};
	enum enumEvilGod {
		SKILL_BASE,		// ��ͨ����
		SKILL_EVIL,		// �ɼ���
		SKILL_GOD,		// ħ����
	};

public:
	ACString GetSkillName(int skillID);							// ��ȡ��������
	AString GetSkillIcon(int skillID);							// ��ȡ����ͼ��
	ACString GetSkillDescription(int skillID, int level);		// ��ȡ��������

	int GetSkillSp(int skillID, int level);			// ��ȡ��������sp
	int GetSkillMoney(int skillID, int level);		// ��ȡ�������Ľ�Ǯ
	int GetSkillCurrentLevel(int skillID);			// ��ȡ���ܵ�ǰ�ȼ�
	int GetSkillMaxLevel(int skillID);				// ��ȡ�������ȼ�
	bool IsPassiveSkill(int skillID);				// �Ƿ񱻶�����
	enumEvilGod GetSkillEvilGod(int skillID);		// �������ɼ��ܡ�ħ���ܻ�����ͨ����

	int GetRequiredBook(int skillID, int level);	// ѧϰ������Ҫ����Ʒ

	typedef std::vector<std::pair<unsigned int, int> > JuniorSkillVector;
	std::vector<std::pair<unsigned int, int> > GetRequiredSkill(int skillID, int level);	// ѧϰ������Ҫ��ǰ�ü���

	enumSkillFitLevelState GetSkillFitLevel(int skillID);		// ѧϰ�ü����Ƿ�����ȼ����澳������
	enumSkillFitLevelState GetSkillFitLevel(int skillID, int maxLevel, int rank, int realmLevel);
	bool CheckHasNewSkillCanLearn(int oldMaxLevel, int oldRank, int oldRealmLevel);
	enumSkillLearnedState GetSkillLearnedState(int skillID);	// ��ǰ��ѧϰ״̬

	void Initialize();			// ��ȡ��ǰְҵ�����м��ܣ�����������
	
	void Release();

	int GetTotalSkillCount();	// ��ȡ��ǰְҵ���м��ܵĸ���

	bool CheckPreSkillLevel(int skillID, int level);	// ���ǰ�ü��ܺͼ���
	bool CheckPreItem(int itemID);				// ���ǰ����Ʒ

	AString GetPreItemIcon(int itemID);			// ��ȡǰ����Ʒ��ͼ��
	ACString GetPreItemDescription(int itemID);	// ��ȡǰ����Ʒ������

	int CheckLearnCondition(int skillID);		// ��鼼���Ƿ��ѧ

	int GetCurrentRank();						// ��ȡ���ﵱǰ����ȼ�

	

	static CECHostSkillModel& Instance();
	
	~CECHostSkillModel();

	const abase::hash_map<int, abase::vector<int> >& GetAllRankProfSkills() { return m_allRankProfSkills;}

	void OnLearnSkill(int skillID, int skillLevel);		// �յ�ѧϰ�¼��ܺ󱻵��á�֪ͨ������и���


	// ���ܸ��������
	int GetRootSkill(int skillID, bool isEvil);						// ����һ�����ܵĿ������ɵ����ռ��ܡ����ޣ������Լ�
																	// ����������Ϊ����ǰ״̬�Ƿ�ħ���������Ƿ���ͨ������ħ������
	int GetTreeHeight(int rootSkillID);								// ��ȡһ���������ĸ߶�
	std::vector<std::pair<unsigned int, int> > GetJunior(int skillID);			// ��ȡǰ�ü���

	// NPC���
	bool IsSkillServedByNPC(int skillID);		// ��ǰ�����£��Ƿ���ѧϰĳ���ܵķ���
	void RecvNPCServiceList(const GNET::Octets& Data);	// ���յ�ǰѧϰ����NPC�б�
	void ProcessServiceList();
	bool IsSkillLearnNPC(int nid);				// NPC ID�Ƿ������ص�ѧϰ����NPC���յ�NPC Greetingʱ������
	bool IsSkillLearnNPCExsit();				// ��ǰ�����Ƿ����ѧϰ���ܵ�NPC
	void SendHelloToSkillLearnNPC();			// ����һ��SEVNPC_HELLOЭ�����ǰ������NPC�������ѧϰ���ܷ���

private:
	abase::hash_map<int, abase::vector<int> > m_allRankProfSkills;	// ÿ�����漶���Ӧ�ĸ�ְҵ���м��ܵ��б�
	abase::hash_map<int, GNET::ElementSkill*> m_allProfSkills;		// ��ְҵ���м��ܣ����ڲ�ѯ
	
	// ���ܺ��������ռ��ܵ�ӳ�䣬�������ռ���Ϊ�ɡ�ħ����ͨ��Ϊ����
	typedef abase::hash_map<int, int> SkillRootMap;
	abase::hash_map<int, int> m_baseRootMap;
	abase::hash_map<int, int> m_godRootMap;
	abase::hash_map<int, int> m_evilRootMap;

	abase::hash_map<int, int> m_treeHeightMap;	// ÿ�������ܶ�Ӧ�ĸ��Ǽ������ĸ߶�ӳ��

	std::set<int> m_curServiceSkills;	// ��ǰNPC�ṩ��������м��ܡ���ͬ������ѧ�ļ��ܿ��ܲ�ͬ
	int m_skillLearnNPCNID;				// ��ǰ������NPC
	std::set<int> m_allProfNPCs;		// ���еĵ�ǰְҵ���ڼ��ܵ�NPC

	bool m_bInitialized;				// �Ƿ��ʼ��

	// �յ�cmd_scene_service_npc_list�����ڳ�ʼ����ǰ���ʽ��䱣�棬����ʼ��ʱ�ٽ��д���
	GNET::Octets m_npcListData;

private:
	CECHostSkillModel();

	// ����һ�����ܵ����г������ܣ������㼼�����߶ȡ�tpyeΪ0��1��2�ֱ������ڵ�Ϊ��ͨ���ɡ�ħ
	int GetSkillTreeHeight(int rootSkillID);	
	void InitializeRootOfSkillTree(int rootSkillID);

	// ��ʼ����ǰְҵ�����м���
	void InitAllSkillsOfCurProf();

	// �ҵ���ǰְҵ�����޾������ƵĽ��ڼ���NPC
	void FindAllNPCsOfCurProf();

	// ��ʼ��������ӳ���
	void InitSkillTreeRootMap(const std::set<int>& rootSkills);

	// ��ʼ���������߶�ӳ���
	void InitSkillTreeHeightMap(const std::set<int>& rootSkills);

	// �õ�������Ϊ���ļ��ܼ���
	std::set<int> GetRootSkillSet();

	SkillRootMap& GetSkillRootMap(int rootSkillID);
	

	// ����
	CECHostSkillModel(const CECHostSkillModel &);
	CECHostSkillModel & operator = (const CECHostSkillModel &);

	void SetCurServiceSkills(int tid);	// ���õ�ǰ���еĿ�ѧ���ܡ�����Ϊnpc��tid
};

#endif
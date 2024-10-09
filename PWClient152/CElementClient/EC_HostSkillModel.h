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

/* 技能状况改变类，用于通知界面变化 */
class CECSkillPanelChange : public CECObservableChange {
public:
	enum enumChangeMask {
		CHANGE_SKILL_LEVEL_UP,		// 技能升级
		CHANGE_SKILL_OVERRIDDEN,	// 技能被覆盖
		CHANGE_SKILL_NPC,			// 切换场景后，提供技能服务的NPC变化
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


/* 技能面板的Model类 */
class CECHostSkillModel : public CECObservable<CECHostSkillModel> {
public:
	enum enumSkillFitLevelState {
		SKILL_FIT_LEVEL,		// 技能满足等级、修真、境界条件
		SKILL_NOT_FIT_LEVEL,	// 技能不满足等级、修真、境界条件
	};
	enum enumSkillLearnedState {
		SKILL_NOT_LEARNED,	// 技能未学习
		SKILL_LEARNED,		// 技能已学习，但不满级
		SKILL_FULL,			// 技能已满级
		SKILL_OVERRIDDEN,	// 技能已被覆盖
	};
	enum enumEvilGod {
		SKILL_BASE,		// 普通技能
		SKILL_EVIL,		// 仙技能
		SKILL_GOD,		// 魔技能
	};

public:
	ACString GetSkillName(int skillID);							// 获取技能名称
	AString GetSkillIcon(int skillID);							// 获取技能图标
	ACString GetSkillDescription(int skillID, int level);		// 获取技能描述

	int GetSkillSp(int skillID, int level);			// 获取技能消耗sp
	int GetSkillMoney(int skillID, int level);		// 获取技能消耗金钱
	int GetSkillCurrentLevel(int skillID);			// 获取技能当前等级
	int GetSkillMaxLevel(int skillID);				// 获取技能最大等级
	bool IsPassiveSkill(int skillID);				// 是否被动技能
	enumEvilGod GetSkillEvilGod(int skillID);		// 技能是仙技能、魔技能还是普通技能

	int GetRequiredBook(int skillID, int level);	// 学习技能需要的物品

	typedef std::vector<std::pair<unsigned int, int> > JuniorSkillVector;
	std::vector<std::pair<unsigned int, int> > GetRequiredSkill(int skillID, int level);	// 学习技能需要的前置技能

	enumSkillFitLevelState GetSkillFitLevel(int skillID);		// 学习该技能是否满足等级修真境界条件
	enumSkillFitLevelState GetSkillFitLevel(int skillID, int maxLevel, int rank, int realmLevel);
	bool CheckHasNewSkillCanLearn(int oldMaxLevel, int oldRank, int oldRealmLevel);
	enumSkillLearnedState GetSkillLearnedState(int skillID);	// 当前的学习状态

	void Initialize();			// 读取当前职业的所有技能，建立技能树
	
	void Release();

	int GetTotalSkillCount();	// 获取当前职业所有技能的个数

	bool CheckPreSkillLevel(int skillID, int level);	// 检查前置技能和级别
	bool CheckPreItem(int itemID);				// 检查前置物品

	AString GetPreItemIcon(int itemID);			// 获取前置物品的图标
	ACString GetPreItemDescription(int itemID);	// 获取前置物品的描述

	int CheckLearnCondition(int skillID);		// 检查技能是否可学

	int GetCurrentRank();						// 获取人物当前修真等级

	

	static CECHostSkillModel& Instance();
	
	~CECHostSkillModel();

	const abase::hash_map<int, abase::vector<int> >& GetAllRankProfSkills() { return m_allRankProfSkills;}

	void OnLearnSkill(int skillID, int skillLevel);		// 收到学习新技能后被调用。通知界面进行更新


	// 技能覆盖树相关
	int GetRootSkill(int skillID, bool isEvil);						// 返回一个技能的可升级成的最终技能。若无，返回自己
																	// 后两个参数为：当前状态是否魔，根技能是否普通（非仙魔）技能
	int GetTreeHeight(int rootSkillID);								// 获取一个技能树的高度
	std::vector<std::pair<unsigned int, int> > GetJunior(int skillID);			// 获取前置技能

	// NPC相关
	bool IsSkillServedByNPC(int skillID);		// 当前场景下，是否有学习某技能的服务
	void RecvNPCServiceList(const GNET::Octets& Data);	// 接收当前学习技能NPC列表
	void ProcessServiceList();
	bool IsSkillLearnNPC(int nid);				// NPC ID是否是隐藏的学习技能NPC，收到NPC Greeting时被调用
	bool IsSkillLearnNPCExsit();				// 当前场景是否存在学习技能的NPC
	void SendHelloToSkillLearnNPC();			// 发送一个SEVNPC_HELLO协议给当前的隐藏NPC，请求打开学习技能服务

private:
	abase::hash_map<int, abase::vector<int> > m_allRankProfSkills;	// 每个修真级别对应的该职业所有技能的列表
	abase::hash_map<int, GNET::ElementSkill*> m_allProfSkills;		// 该职业所有技能，用于查询
	
	// 技能和它的最终技能的映射，根据最终技能为仙、魔、普通分为三类
	typedef abase::hash_map<int, int> SkillRootMap;
	abase::hash_map<int, int> m_baseRootMap;
	abase::hash_map<int, int> m_godRootMap;
	abase::hash_map<int, int> m_evilRootMap;

	abase::hash_map<int, int> m_treeHeightMap;	// 每个根技能对应的覆盖技能树的高度映射

	std::set<int> m_curServiceSkills;	// 当前NPC提供服务的所有技能。不同场景可学的技能可能不同
	int m_skillLearnNPCNID;				// 当前的隐藏NPC
	std::set<int> m_allProfNPCs;		// 所有的当前职业教授技能的NPC

	bool m_bInitialized;				// 是否初始化

	// 收到cmd_scene_service_npc_list可能在初始化以前，故将其保存，当初始化时再进行处理
	GNET::Octets m_npcListData;

private:
	CECHostSkillModel();

	// 遍历一个技能的所有初级技能，并计算技能树高度。tpye为0、1、2分别代表根节点为普通、仙、魔
	int GetSkillTreeHeight(int rootSkillID);	
	void InitializeRootOfSkillTree(int rootSkillID);

	// 初始化当前职业的所有技能
	void InitAllSkillsOfCurProf();

	// 找到当前职业所有无距离限制的教授技能NPC
	void FindAllNPCsOfCurProf();

	// 初始化根技能映射表
	void InitSkillTreeRootMap(const std::set<int>& rootSkills);

	// 初始化技能树高度映射表
	void InitSkillTreeHeightMap(const std::set<int>& rootSkills);

	// 得到所有作为根的技能集合
	std::set<int> GetRootSkillSet();

	SkillRootMap& GetSkillRootMap(int rootSkillID);
	

	// 禁用
	CECHostSkillModel(const CECHostSkillModel &);
	CECHostSkillModel & operator = (const CECHostSkillModel &);

	void SetCurServiceSkills(int tid);	// 设置当前所有的可学技能。参数为npc的tid
};

#endif
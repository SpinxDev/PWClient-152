/*
 * FILE: EC_HostGoblin.h
 *
 * DESCRIPTION: Goblin follow the hostplayer
 *
 * CREATED BY: Sunxuewei, 2008/11/06
 *
 */
#ifndef _EC_HOSTGOBLIN_H_
#define _EC_HOSTGOBLIN_H_

#include "A3DTypes.h"
#include "EC_Counter.h"
#include "EC_MsgDef.h"
#include "EC_Goblin.h"
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECModel;
class CECPlayer;
class CECViewport;
class CECInventory;
class CECSkill;
class CECIvtrGoblinEquip;
class CECObject;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHostGoblin
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// 
// 
//
///////////////////////////////////////////////////////////////////////////
class CECHostGoblin : public CECGoblin
{
public:
	enum
	{
		MAX_SKILLNUM = 8,
	};
private:
	int				m_iSecurityState;	// 小精灵安全状态： 0: 安全状态，>0：转化状态，为转化完毕的时间点，-1:可交易状态 
	unsigned int	m_iExp;				// 经验值	
	int				m_iLevel;			// 等级	

	int				m_iTotalStatusPt;	// 升级带来的属性点总数
	int				m_iStrength;		// 力量
	int				m_iAgility;			// 敏捷
	int				m_iVitality;		// 体质
	int				m_iEnergy;			// 灵力

	int				m_iStrEnhance;		// Buff技能增加的力量
	int				m_iAgiEnhance;		// Buff技能增加的敏捷
	int				m_iVitEnhance;		// Buff技能增加的体质
	int				m_iEngEnhance;		// Buff技能增加的灵力
	

	int				m_iTotalGeniusPt;	// 升级带来的天赋点总数
	int				m_aGenius[5];		// 已分配的金木水火土5项天赋

	int				m_iVigor;			// 当前元气值
	int				m_iMaxVigor;		// 元气上限
	float			m_fVigorGen;		// 元气回复

	int				m_iStamina;			// 耐力值
	int				m_iGrowDegree;		// 成长度
	int				m_iMaxRandomPt;		// 当前能获取的最大随机属性值

	int				m_iCurrMaxSkillNum;	// 当前能够学习的技能的数量
	
	CECInventory*	m_pEquipPack;		// 装备栏

	CECSkill*		m_aSkills[MAX_SKILLNUM];	// 技能列表
public:		//	Constructor and Destructor
	
	CECHostGoblin();
	virtual ~CECHostGoblin();

public:		//	Operations
	//	Initialize object
	virtual bool Init(int tid, CECIvtrGoblin* pIvtrGoblin, CECPlayer* pPlayer);
	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport);
	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	unsigned int GetExp() const				{ return m_iExp; }
	void SetExp(unsigned int iExp)			{ m_iExp = iExp; }

	void SetLevel(int iLevel)				{ m_iLevel = iLevel; }
	int GetLevel() const					{ return m_iLevel; }		

	//  属性点和力敏体灵
	int GetStatusPt()						{ return m_iTotalStatusPt; }
	int GetStrength()						{ return m_iStrength; }
	int GetVitality()						{ return m_iVitality; }
	int GetAgility()						{ return m_iAgility; }
	int GetEnergy()							{ return m_iEnergy; }

	int GetStrengthEnhance()				{ return m_iStrEnhance; }
	int GetVitalityEnhance()				{ return m_iVitEnhance; }
	int GetAgilityEnhance()					{ return m_iAgiEnhance; }
	int GetEnergyEnhance()					{ return m_iEngEnhance; }

	//  升级带来的天赋点总数
	int GetGeniusPt()						{ return m_iTotalGeniusPt; }
	//  金木水火土属性点，bAddEquip为true表示加上装备的
	int GetGenius(int index, bool bAddEquip = true);

	//  小精灵基础说明文字
	const wchar_t* GetBasicDescText();
	//  小精灵安全状态 0: 安全状态，>0：转化状态，返回转化完毕的时间点，-1:可交易状态 
	int GetSecurityState()					{ return m_iSecurityState;}

	//  元气
	int GetAP()								{ return m_iVigor; }
	//  元气上限
	int GetMaxAP()							{ return m_iMaxVigor; }
	//  元气恢复速度
	float GetAPGen()						{ return m_fVigorGen; }
	//  耐力
	int GetStamina()						{ return m_iStamina; }
	//  成长度
	int GetGrowDegree()						{ return m_iGrowDegree; }

	//  精炼时耐力消耗速度
	float GetMPCost();

	//  当前获取的随机属性点
	int GetRandomStatusPt()					{ return m_iTotalStatusPt - (m_iLevel - 1); }
	//  当前能获取的最大属性点
	int GetMaxRandomStatusPt()				{ return m_iMaxRandomPt; }

	//  获取装备栏
	CECInventory* GetEquipment()			{ return m_pEquipPack; }

	//  获取技能
	CECSkill* GetSkill(int index)			{ ASSERT(index >=0 && index < GetSkillNum()); return m_aSkills[index]; }
	//	获取已学习的技能数量
	int GetSkillNum();
	//  获取当前能够学习的技能的最大数量
	int GetCurrMaxSkillNum()				{ return m_iCurrMaxSkillNum; }
	
	//	获取由id指定的技能
	CECSkill* GetSkillByID(int id);	
	
	//  是否可以交易？
	bool CanTrade()							{ return (m_iSecurityState<0) ? true : false; }

	//	判断小精灵是否可以释放指定的技能
	//  0:成功            1:职业不匹配        2:mp不足
	//  3:AP不足		  4:天赋点不足		  5:错误ID
	//  6:未选择目标      7:非小精灵技能      8:小精灵等级不够
	int CheckSkillCastCondition(CECSkill* pSkill);

	//  销毁装备
	bool DestroyEquip(int iSlot);

	//  计算小精灵实际获取的经验值， iExp为输入的经验值
	//  iLevel为经验丸的等级， > 0 则说明从经验丸注入，为0则为从主角经验值输入
	unsigned int InjectExp(unsigned int iExp, int iLevel = 0);
	
	//  计算使小精灵经验增加指定值所需的等级为iLevel的经验丸中的经验值
	unsigned int NeedPillExp(unsigned int goblin_exp, int iLevel);

	//  计算使小精灵经验增加指定值所需的主角经验值
	unsigned int NeedPlayerExp(unsigned int goblin_exp);
		
	//  计算小精灵属性点更改需要的点数
	int NeedStatusPt(int iBase, int iNum);

	//  小精灵当前未使用的属性点数
	int UnusedStatusPt();	
	
	//  小精灵当前未使用的天赋点数
	int UnusedGeniusPt();

	//	小精灵释放技能
	//  iIndex为小精灵已学会的技能索引
	bool CastSkill(int iIndex, int idTarget, bool bForceAttack);

	//  消息处理
	void OnMsgGoblinVigor(const ECMSG& Msg);
	void OnMsgGoblinEnhance(const ECMSG& Msg);
	void OnMsgGoblinStamina(const ECMSG& Msg);
	void OnMsgGoblinExp(const ECMSG& Msg);
};
///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif//_EC_HOSTGOBLIN_H_
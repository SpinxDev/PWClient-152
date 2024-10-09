// File		: DlgProfView.h
// Creator	: Chen Zhixin
// Date		: 2009/5/5

#pragma once

#include "DlgBase.h"
#include "AUIComboBox.h"
#include "AUILabel.h"

class CDlgProfView : public CDlgBase  
{

	AUI_DECLARE_COMMAND_MAP()
	
public:
	enum
	{
		PHYSICAL_DAMAGE_LOW = 0,	// 物理攻击下限
			MAGIC_DAMAGE_LOW,			// 法术攻击下限
			ACCURATE,					// 准确度
			ATTACK_LEVEL,				// 攻击等级
			PHYSICAL_DEFENCE,			// 物理防御	
			
			MAGIC_DEFENCE_GOLD,			// 金防
			MAGIC_DEFENCE_WOOD,			// 木防
			MAGIC_DEFENCE_WATER,		// 水防
			MAGIC_DEFENCE_FAIR,			// 火防
			MAGIC_DEFENCE_EARTH,		// 土防
			
			ARMOR,						// 躲闪度
			DEFENCE_LEVEL,				// 防御等级
			ATTACK_SPEED,				// 普通频率
			RUN_SPEED,					// 移动速度
			CRIT_RATE,					// 致命一击率

			DAMAGE_REDUCE,				// 装备物理减免百分比
			PRAYSPEED,					// 吟唱速度百分比
			CRIT_DAMANGE_BONUS,			// 暴伤百分比增加值
			INVISIBLE_DEGREE,			// 遁隐等级
			ANTI_INVISIBLE_DEGREE,		// 反遁隐等级
			VIGOUR,						// 气魄
			
			PROPERTYNUM,
	};

	struct ProfView
	{
		ACString name;
		int level;
		int level2;
		int	profession;
		int realm_level;
		int reincarnation_count;
		int hp;
		int mp;
		int self_damagereduce;
		int self_prayspeed;
		int prof[PROPERTYNUM];
		int attackmax[2];
	};
	
	CDlgProfView();
	void SetProperty(ProfView& prof);
	void RefreshDetails();
	virtual ~CDlgProfView();
	
protected:
	virtual void OnTick();
	virtual void OnShowDialog();
	virtual bool Render();
	virtual bool OnInitDialog();

	void SetAttckColor(PAUILABEL pObj, int attack);
	void SetDefenceColor(PAUILABEL pObj, int defence);
	void SetReduceColor(PAUILABEL pObj, int percent);
	inline int CalcDegrade(int defense, int level){ return (int)((float)defense / (defense + level * 40 - 25) * 100); }
	
	int	m_type;
	ProfView m_prof;
	PAUICOMBOBOX m_pObj;
	PAUILABEL m_pTxt_Target[PROPERTYNUM];
	PAUILABEL m_pTxt_Tome[PROPERTYNUM];
};

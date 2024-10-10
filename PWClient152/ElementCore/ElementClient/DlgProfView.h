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
		PHYSICAL_DAMAGE_LOW = 0,	// ����������
			MAGIC_DAMAGE_LOW,			// ������������
			ACCURATE,					// ׼ȷ��
			ATTACK_LEVEL,				// �����ȼ�
			PHYSICAL_DEFENCE,			// �������	
			
			MAGIC_DEFENCE_GOLD,			// ���
			MAGIC_DEFENCE_WOOD,			// ľ��
			MAGIC_DEFENCE_WATER,		// ˮ��
			MAGIC_DEFENCE_FAIR,			// ���
			MAGIC_DEFENCE_EARTH,		// ����
			
			ARMOR,						// ������
			DEFENCE_LEVEL,				// �����ȼ�
			ATTACK_SPEED,				// ��ͨƵ��
			RUN_SPEED,					// �ƶ��ٶ�
			CRIT_RATE,					// ����һ����

			DAMAGE_REDUCE,				// װ���������ٷֱ�
			PRAYSPEED,					// �����ٶȰٷֱ�
			CRIT_DAMANGE_BONUS,			// ���˰ٷֱ�����ֵ
			INVISIBLE_DEGREE,			// �����ȼ�
			ANTI_INVISIBLE_DEGREE,		// �������ȼ�
			VIGOUR,						// ����
			
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

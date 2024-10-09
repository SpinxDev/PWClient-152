// File		: EC_ComputerAid.h
// Creator	: Xu Wenbin
// Date		: 2012/8/8

#pragma once

#include "EC_Configs.h"
#include "EC_Counter.h"

class CECIvtrItem;

class CEComputerAid
{
private:
	CEComputerAid();
	CEComputerAid(const CEComputerAid &);
	CEComputerAid & operator == (const CEComputerAid &);
	
	enum {COUNT = EC_COMPUTER_AIDED_SETTING::GROUP_COUNT};
	typedef EC_COMPUTER_AIDED_SETTING::ITEM_GROUP ITEM_GROUP;

	CECCounter	m_counters[COUNT];		//	用药最少间隔定时器

	struct Condition 
	{
		float cur_hp_percent;
		float cur_mp_percent;
	};
	bool Meet(int i, const Condition &c)const;

public:
	static CEComputerAid & Instance();

	bool  Set(const EC_COMPUTER_AIDED_SETTING &cas);
	const EC_COMPUTER_AIDED_SETTING & Get()const;

	bool  CanUse()const;
	void  Tick();

	bool  ValidateItem(int i, int item)const;
	bool  ValidateItem(int i, const CECIvtrItem *pItem)const;
	bool  ValidateSlot(int i, short slot)const;
	bool  ValidatePercent(int i, char percent, bool bForEnable)const;
	int   Validate(int i, const ITEM_GROUP &group)const;
	bool  MakeValid(int i, ITEM_GROUP &group)const;
};
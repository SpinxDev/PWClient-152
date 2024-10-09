// Filename	: EC_Meridians.cpp
// Date		: 1/22, 2013


#include "EC_Meridians.h"
#include "ABaseDef.h"
#include "elementdataman.h"
#include "ExpTypes.h"
#include "EC_Global.h"
#include "EC_Game.h"

static const int levelParams[] = {25,1,5,
20,1,10,
15,1,15,
10,1,20,
9,1,25,
9,1,30,
8,1,35,
8,1,40,
7,1,45,
7,1,50,
6,1,56,
6,1,61,
5,1,67,
5,1,72,
5,1,78,
4,1,83,
4,1,89,
4,1,94,
4,1,100,
10,2,110,
8,1,115,
8,1,120,
7,1,125,
7,1,130,
6,1,136,
6,1,141,
5,1,147,
5,1,152,
4,1,158,
4,1,163,
3,1,169,
3,1,175,
2,1,182,
2,1,188,
2,1,195,
1,1,205,
1,1,215,
1,1,225,
1,1,235,
7,2,254,
5,1,259,
4,1,265,
3,1,271,
2,1,277,
1,1,287,
1,1,297,
1,1,307,
10,2,318,
10,2,329,
10,2,339,
10,2,350,
9,2,363,
9,2,376,
9,2,389,
9,2,402,
8,2,417,
8,2,432,
8,2,447,
8,2,462,
12,3,494,
1,1,504,
1,1,514,
1,1,524,
10,2,534,
10,2,545,
9,2,558,
9,2,571,
8,2,586,
8,2,601,
7,2,620,
7,2,639,
6,2,664,
6,2,689,
5,2,722,
5,2,755,
5,2,788,
4,2,828,
4,2,868,
10,3,912,
8,3,1000,
};

// 获取实例
CECMeridians& CECMeridians::GetSingleton()
{
	static CECMeridians instance;
	return instance;
}
// 构造函数
CECMeridians::CECMeridians()
{
	for(int i = 0;i < 80;i++)
	{
		m_MeridianlevelParams[i].successGate              = levelParams[i * 3];
		m_MeridianlevelParams[i].continuousSuccessGateNum = levelParams[i * 3 + 1];
		m_MeridianlevelParams[i].bonusMultiplier          = levelParams[i * 3 + 2];
	}
}
// 析构函数
CECMeridians::~CECMeridians()
{
}

const CECMeridians::MeridianlevelParam& CECMeridians::GetLevelParam(int level)
{
	if(level < 0 || level >= 80)
	{
		ASSERT(!"Meridians Level invalid!");
		return m_MeridianlevelParams[0];
	}
	
	return m_MeridianlevelParams[level];
}

bool CECMeridians::GetLevelPropBonus(int profession,int level,int& hp,int& phyDefence,int& mgiDefence,int& phyAttack,int& mgiAttack)
{
	unsigned int id = 0;

	if(level != 0)
	{
		const MeridianlevelParam& curLevelParam = GetLevelParam(level-1);
		elementdataman * pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE dt = DT_INVALID;
		id = pDataMan->get_first_data_id(ID_SPACE_CONFIG, dt);
		while (id > 0)
		{
			if (dt == DT_MERIDIAN_CONFIG) break;
			id = pDataMan->get_next_data_id(ID_SPACE_CONFIG, dt);
		}
		
		if (id > 0)
		{
			const MERIDIAN_CONFIG * pConfig = static_cast<const MERIDIAN_CONFIG *>(pDataMan->get_data_ptr(id, ID_SPACE_CONFIG, dt));
			if(pConfig)
			{
				hp         = pConfig->prof_para[profession].hp * curLevelParam.bonusMultiplier / 100;
				phyDefence = pConfig->prof_para[profession].phy_defence * curLevelParam.bonusMultiplier / 100;
				mgiDefence = pConfig->prof_para[profession].magic_defence * curLevelParam.bonusMultiplier / 100;
				phyAttack  = pConfig->prof_para[profession].phy_damage * curLevelParam.bonusMultiplier / 100;
				mgiAttack  = pConfig->prof_para[profession].magic_damage * curLevelParam.bonusMultiplier / 100;
			}
		}
	}
	else
	{
		hp = 0;
		phyDefence = 0;
		mgiDefence = 0;
		phyAttack  = 0;
		mgiAttack  = 0;
	}

	
	return id > 0;
}
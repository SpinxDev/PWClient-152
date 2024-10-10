// Filename	: EC_Meridians.h
// Date		: 1/22, 2013

#pragma once

class CECMeridians
{
public:
	enum
	{
		FRONT_MERIDIAN_HUIYIN ,		// 会阴穴
		FRONT_MERIDIAN_QUGU,		// 曲骨穴
		FRONT_MERIDIAN_SHIMEN,		// 石门穴
		FRONT_MERIDIAN_QIHAI,		// 气海穴
		FRONT_MERIDIAN_JUQUE,		 // 巨阙穴
		FRONT_MERIDIAN_SHANZHONG,	// 膻中穴
		FRONT_MERIDIAN_ZIGONG,		// 紫宫穴
		FRONT_MERIDIAN_XUANJI,		// 璇玑穴
		FRONT_MERIDIAN_TIANTU,		// 天突穴

		BACK_MERIDIAN_CHANGQIANG,   // 长强穴
		BACK_MERIDIAN_YAOYU,		// 腰俞穴
		BACK_MERIDIAN_YANGGUAN,		// 阳关穴
		BACK_MERIDIAN_MINGMEN,		// 命门穴
		BACK_MERIDIAN_ZHIYANG,		// 至阳穴
		BACK_MERIDIAN_LINGTAI,		// 灵台穴
		BACK_MERIDIAN_SHENDAO,		// 神道穴
		BACK_MERIDIAN_FENGFU,		// 风府穴
		BACK_MERIDIAN_BAIHUI,		// 百会穴
		BACK_MERIDIAN_SHENTING,		// 神庭穴

		MERIDIAN_LEVEL_THRESHOLD,   // 境界穴位
		MERIDIAN_LEVEL_COUNT,       // 穴位个数

		MeridiansLevelLayer = 4,	// 境界数量
	};

	struct MeridianlevelParam
	{
		int successGate;            // 生门数量
		int continuousSuccessGateNum; // 连续翻开生门的次数
		int bonusMultiplier;        // 奖励系数 需要除以100
	};

	~CECMeridians();
	
	static CECMeridians& GetSingleton();

	const CECMeridians::MeridianlevelParam& GetLevelParam(int level);

	bool GetLevelPropBonus(int profession,int level,int& hp,int& phyDefence,int& mgiDefence,int& phyAttack,int& mgiAttack);

protected:
	CECMeridians();
private:
	MeridianlevelParam m_MeridianlevelParams[80];
};



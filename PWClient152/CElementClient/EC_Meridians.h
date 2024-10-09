// Filename	: EC_Meridians.h
// Date		: 1/22, 2013

#pragma once

class CECMeridians
{
public:
	enum
	{
		FRONT_MERIDIAN_HUIYIN ,		// ����Ѩ
		FRONT_MERIDIAN_QUGU,		// ����Ѩ
		FRONT_MERIDIAN_SHIMEN,		// ʯ��Ѩ
		FRONT_MERIDIAN_QIHAI,		// ����Ѩ
		FRONT_MERIDIAN_JUQUE,		 // ����Ѩ
		FRONT_MERIDIAN_SHANZHONG,	// ����Ѩ
		FRONT_MERIDIAN_ZIGONG,		// �Ϲ�Ѩ
		FRONT_MERIDIAN_XUANJI,		// ���Ѩ
		FRONT_MERIDIAN_TIANTU,		// ��ͻѨ

		BACK_MERIDIAN_CHANGQIANG,   // ��ǿѨ
		BACK_MERIDIAN_YAOYU,		// ����Ѩ
		BACK_MERIDIAN_YANGGUAN,		// ����Ѩ
		BACK_MERIDIAN_MINGMEN,		// ����Ѩ
		BACK_MERIDIAN_ZHIYANG,		// ����Ѩ
		BACK_MERIDIAN_LINGTAI,		// ��̨Ѩ
		BACK_MERIDIAN_SHENDAO,		// ���Ѩ
		BACK_MERIDIAN_FENGFU,		// �縮Ѩ
		BACK_MERIDIAN_BAIHUI,		// �ٻ�Ѩ
		BACK_MERIDIAN_SHENTING,		// ��ͥѨ

		MERIDIAN_LEVEL_THRESHOLD,   // ����Ѩλ
		MERIDIAN_LEVEL_COUNT,       // Ѩλ����

		MeridiansLevelLayer = 4,	// ��������
	};

	struct MeridianlevelParam
	{
		int successGate;            // ��������
		int continuousSuccessGateNum; // �����������ŵĴ���
		int bonusMultiplier;        // ����ϵ�� ��Ҫ����100
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



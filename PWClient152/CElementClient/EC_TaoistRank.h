// Filename	: EC_SkillPanel.h
// Creator	: zhangyitian
// Date		: 2014/07/01

#ifndef _ELEMENTCLIENT_EC_TAOISTRANK_H_
#define _ELEMENTCLIENT_EC_TAOISTRANK_H_

#include "AAssist.h"

/* 表示修真级别的类，TaoistRank意思是修道级别，作为修真的翻译 */
class CECTaoistRank {
public:
	enum {
		TotalRankCount		= 15,
		BaseRankCount		= 9,
		GodRankCount		= 3,
		EvilRankCount		= 3,
	};
	static const CECTaoistRank* GetBaseRankBegin();		// 获取第一个普通修真级别
	static const CECTaoistRank* GetBaseRankEnd();			// 获取最后一个修真级别的下一个，返回NULL
	static const CECTaoistRank* GetLastBaseRank();			// 获取最后一个普通修真级别
	static const CECTaoistRank* GetGodRankBegin();			// 获取第一个仙级别
	static const CECTaoistRank* GetGodRankEnd();			// 获取最后一个仙级别的下一个，返回NULL
	static const CECTaoistRank* GetLastGodRank();			// 获取最后一个仙级别
	static const CECTaoistRank* GetEvilRankBegin();			// 获取第一个魔级别
	static const CECTaoistRank* GetEvilRankEnd();			// 获取最后一个魔级别的下一个，返回NULL
	static const CECTaoistRank* GetLastEvilRank();			// 获取最后一个魔级别
	static const CECTaoistRank* GetTaoistRank(int id);		// 根据ID得到修真级别

	static int GetTotalTaoistRankCount();		// 所有修真级别的数量
	static int GetGodTaoistRankCount();			// 所有仙修真级别的数量
	static int GetEvilTaoistRankCount();		// 所有魔修真级别的数量
	static int GetBaseTaoistRankCount();		// 所有普通修真级别的数量

	const CECTaoistRank* GetNext() const;		// 获取下一个级别
	int GetID() const;							// 获取修真级别的ID
	ACString GetName() const;					// 获取修真级别的名称

	bool IsGodRank() const;			//是否仙修真
	bool IsEvilRank() const;		// 是否魔修真
	bool IsBaseRank() const;		// 是否普通修真

private:
	CECTaoistRank();
	CECTaoistRank(const CECTaoistRank&);
	CECTaoistRank& operator = (const CECTaoistRank&);
	static void init();
private:
	int m_id;			// 修真级别的ID
	ACString m_name;	// 修真级别的名称
	CECTaoistRank* m_next;	//下一个修真级别
	static CECTaoistRank s_allTaoistRanks[TotalRankCount];
};

#endif
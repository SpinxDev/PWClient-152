// Filename	: EC_AutoTeamConfig.h
// Creator	: Xu Wenbin
// Date		: 2013/12/24

#pragma once

#include <vector.h>
#include <AString.h>
#include <AAssist.h>
#include <hashmap.h>

#include <algorithm>
#include <set>

#define ACTIVITY_AWARD_NUM 3

struct AUTOTEAM_CONFIG;
class CECAutoTeamConfig
{
public:
	struct Activity
	{
		Activity();

		const AUTOTEAM_CONFIG *p;
		AString icon;
		bool	full_day;

		typedef abase::hash_map<int, bool> TaskMap;
		TaskMap	m_tasks;

		void SetConfig(const AUTOTEAM_CONFIG *pConfig);

		bool IsHappenOn(int y, int m, int d)const;
		int YearBegin()const;
		int YearEnd()const;
		int MonthBegin()const;
		int MonthEnd()const;
		int DayBegin()const;
		int DayEnd()const;
		bool IsFullDayTime()const;
		bool IsHappenIn(char h, char m)const;
		bool IsDisable() const;
		bool IsRecommend() const;
		bool CanJumpFrom(int world_id) const;
		bool FindTask(int taskID)const;

	private:		
		bool IsHappenOnWeekDay(int w)const;
	};

public:
	static CECAutoTeamConfig & Instance();
	~CECAutoTeamConfig();

	bool InitializeActivity();

	int  GetCount()const;
	const Activity * Get(int index)const;

	bool IsInEnabledMap();
	bool IsHaveTransmitItem();
	bool CanJumpToGoal(const Activity* pAct, bool bCheckTransmitItem=true);
	bool CanDoAutoTeam(const Activity* pAct);
	bool IsFitLevelRankRealm(const Activity* pAct);

	bool UpdateNewActivity();

	template <typename T>
		void Sort(const T &p){
		std::sort(m_Activities.begin(), m_Activities.end(), p);
	}

private:
	CECAutoTeamConfig();
	CECAutoTeamConfig(const CECAutoTeamConfig &);
	CECAutoTeamConfig & operator=(const CECAutoTeamConfig &);

	void Release();
	
private:
	abase::vector<Activity *> m_Activities;
	std::set<Activity*> m_oldActivities;		// 以前可参加的所有活动。每次升级、修真变化等都要更新这个集合
};
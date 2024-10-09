// Filename	: EC_AutoTeamConfig.cpp
// Creator	: Xu Wenbin
// Date		: 2013/12/24

#include "EC_AutoTeamConfig.h"
#include "EC_UIHelper.h"
#include "EC_UIConfigs.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_Time.h"

#include "ExpTypes.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

CECAutoTeamConfig::CECAutoTeamConfig()
{
}

CECAutoTeamConfig & CECAutoTeamConfig::Instance()
{
	static CECAutoTeamConfig s_instance;
	static bool s_initialized(false);
	if (!s_initialized){
		s_instance.InitializeActivity();
		s_initialized = true;
	}
	static bool s_firstUpdateNewActivity(true);
	if (s_firstUpdateNewActivity && CECUIHelper::GetHostPlayer()->HostIsReady()) {
		s_instance.UpdateNewActivity();
		s_firstUpdateNewActivity = false;
	}
	return s_instance;
}

CECAutoTeamConfig::~CECAutoTeamConfig()
{
	Release();
}

void CECAutoTeamConfig::Release()
{
	for (size_t i(0); i < m_Activities.size(); ++ i)
	{
		delete m_Activities[i];
	}
	m_Activities.clear();
	m_oldActivities.clear();
}

bool CECAutoTeamConfig::InitializeActivity()
{
	Release();

	int id;
	DATA_TYPE dt;
	elementdataman* pDataMan = CECUIHelper::GetGame()->GetElementDataMan();

	id = pDataMan->get_first_data_id(ID_SPACE_CONFIG, dt);
	while(id)
	{
		if( dt == DT_AUTOTEAM_CONFIG )
		{
			AUTOTEAM_CONFIG* pConfig = (AUTOTEAM_CONFIG*)pDataMan->get_data_ptr(id, ID_SPACE_CONFIG, dt);

			Activity *pAct = new Activity;
			pAct->SetConfig(pConfig);
			pAct->icon = pConfig->file_icon;
			if( pAct->icon.Find("surfaces\\") == 0 )
				pAct->icon.CutLeft(strlen("surfaces\\"));
			pAct->full_day = (pConfig->daytime[0] == 0) && (pConfig->daytime[1] == 0) && (pConfig->daytime[2] == 0) && (pConfig->daytime[3] == 0);			

			m_Activities.push_back(pAct);
		}

		id = pDataMan->get_next_data_id(ID_SPACE_CONFIG, dt);
	}

	return true;
}

int  CECAutoTeamConfig::GetCount()const
{
	return (int)m_Activities.size();
}

const CECAutoTeamConfig::Activity * CECAutoTeamConfig::Get(int index)const
{
	return m_Activities[index];
}

bool CECAutoTeamConfig::IsFitLevelRankRealm(const Activity* pAct) {
	if( !pAct || pAct->IsDisable() )
		return false;

	CECHostPlayer *pHost = CECUIHelper::GetHostPlayer();

	int iLevel = pHost->GetBasicProps().iLevel;
	if( (pAct->p->require_level[0] && iLevel < pAct->p->require_level[0]) || (pAct->p->require_level[1] && iLevel > pAct->p->require_level[1]) )
		return false;
	
	// 历史最高等级
	int iMaxLevel = pHost->GetMaxLevelSofar();
	if( (pAct->p->require_maxlevel[0] && iMaxLevel < pAct->p->require_maxlevel[0]) || (pAct->p->require_maxlevel[1] && iMaxLevel > pAct->p->require_maxlevel[1]) )
		return false;
	
	// 修真
	int iLevel2 = pHost->GetBasicProps().iLevel2;
	if( iLevel2 < pAct->p->require_level2 )
		return false;
	
	// 性别
	int iGender = pHost->GetGender();
	if( pAct->p->require_gender != 2 && pAct->p->require_gender != iGender )
		return false;
	
	// 转生次数
	int reincarnation_times = pHost->GetReincarnationCount();
	if( (pAct->p->require_reincarnation_times[0] && reincarnation_times < pAct->p->require_reincarnation_times[0]) ||
		(pAct->p->require_reincarnation_times[1] && reincarnation_times > pAct->p->require_reincarnation_times[1]) )
		return false;
	
	// 境界
	int realm_level = pHost->GetRealmLevel();
	if( (pAct->p->require_realm_level[0] && realm_level < pAct->p->require_realm_level[0]) ||
		(pAct->p->require_realm_level[1] && realm_level > pAct->p->require_realm_level[1]) )
		return false;

	return true;
}

bool CECAutoTeamConfig::CanDoAutoTeam(const Activity* pAct)
{
	if( !pAct || pAct->IsDisable() )
		return false;

	// 城战特殊处理，只用于活动展示
	if( pAct->p->id == 1562 )
		return false;

	CECHostPlayer *pHost = CECUIHelper::GetHostPlayer();

	if (!IsFitLevelRankRealm(pAct)) {
		return false;
	}

	// 魂力
	int soul_power = pHost->GetSoulPower();
	if( soul_power < pAct->p->require_soul_power )
		return false;

	// 判断是否在正确的地图中
	return IsInEnabledMap();
}

bool CECAutoTeamConfig::CanJumpToGoal(const Activity* pAct, bool bCheckTransmitItem/* =true */)
{
	if( !pAct || pAct->IsDisable() )
		return false;

	// 城战特殊处理，只用于活动展示
	if( pAct->p->id == 1562 )
		return false;
	
	CECHostPlayer *pHost = CECUIHelper::GetHostPlayer();

	// 等级
	int iLevel = pHost->GetBasicProps().iLevel;
	if( (pAct->p->require_level[0] && iLevel < pAct->p->require_level[0]) || (pAct->p->require_level[1] && iLevel > pAct->p->require_level[1]) )
		return false;

	// 历史最高等级
	int iMaxLevel = pHost->GetMaxLevelSofar();
	if( (pAct->p->require_maxlevel[0] && iMaxLevel < pAct->p->require_maxlevel[0]) || (pAct->p->require_maxlevel[1] && iMaxLevel > pAct->p->require_maxlevel[1]) )
		return false;

	// 修真
	int iLevel2 = pHost->GetBasicProps().iLevel2;
	if( iLevel2 < pAct->p->require_level2 )
		return false;

	// 转生次数
	int reincarnation_times = pHost->GetReincarnationCount();
	if( (pAct->p->require_reincarnation_times[0] && reincarnation_times < pAct->p->require_reincarnation_times[0]) ||
		(pAct->p->require_reincarnation_times[1] && reincarnation_times > pAct->p->require_reincarnation_times[1]) )
		return false;
	
	// 境界
	int realm_level = pHost->GetRealmLevel();
	if( (pAct->p->require_realm_level[0] && realm_level < pAct->p->require_realm_level[0]) ||
		(pAct->p->require_realm_level[1] && realm_level > pAct->p->require_realm_level[1]) )
		return false;

	// 战斗状态
	if( pHost->IsFighting() )
		return false;

	// 检测传送道具
	if(bCheckTransmitItem && !IsHaveTransmitItem() )
		return false;

	// 判断当前地图可否传送到
	int iWorldID = CECUIHelper::GetGameRun()->GetWorld()->GetInstanceID();
	return pAct->CanJumpFrom(iWorldID);
}

bool CECAutoTeamConfig::IsHaveTransmitItem()
{
	const static int deliver_item[] = { 41542, 41543, 41544, 41545 };	
	CECHostPlayer *pHost = CECUIHelper::GetHostPlayer();
	return pHost->GetPack()->FindItems(deliver_item, sizeof(deliver_item)/sizeof(deliver_item[0]));
}

bool CECAutoTeamConfig::IsInEnabledMap()
{
	bool bFound = false;
	int world_id = CECUIHelper::GetGameRun()->GetWorld()->GetInstanceID();
	for( size_t i=0;i<CECUIConfig::Instance().GetGameUI().nAutoTeamTransmitEnabledMap.size();i++ )
	{
		if( world_id == CECUIConfig::Instance().GetGameUI().nAutoTeamTransmitEnabledMap[i] )
		{
			bFound = true;
			break;
		}
	}

	return bFound;
}

bool CECAutoTeamConfig::UpdateNewActivity() {
	bool hasNewActivity = false;
	for (size_t i = 0; i < m_Activities.size(); i++) {
		if (m_oldActivities.find(m_Activities[i]) == m_oldActivities.end() && IsFitLevelRankRealm(m_Activities[i])) {
			m_oldActivities.insert(m_Activities[i]);
			hasNewActivity = true;
		}
	}
	return hasNewActivity;
}

//	CECAutoTeamConfig::Activity
CECAutoTeamConfig::Activity::Activity()
: p(NULL)
{
}

bool CECAutoTeamConfig::Activity::IsHappenOn(int y, int m, int d)const
{
	// Check whether the activity happens given year/time/day
	//
	bool happened = false;
	
	if (p->time_type == 1)
	{
		int w = CECTime::GetWeek(y, m, d);
		happened = IsHappenOnWeekDay(w);
	}
	else
	{
		bool yearly = (YearBegin() == 0);
		bool monthly = (MonthBegin() == 0);
		bool dayly = (DayBegin() == 0);
		
		// Consider 8 cases depending on the above 3 booleans
		
		// Case tuple: (yearly, monthly, dayly)
		// Case 0: (false, false, false)
		// Case 1: (true, false, false)
		// Case 2: (false, true, false)
		// Case 3: (false, false, true)
		// Case 4: (true, true, false)
		// Case 5: (true, false, true)
		// Case 6: (false, true, true)
		// Case 7: (true, true, true)
		
		if (yearly)
		{
			if (monthly)
			{
				if (dayly)
				{
					// Case 7: happens each day
					happened = true;
				}
				else
				{
					// Case 4: happens each year、month but within day range
					happened = CECTime::IsValidSingle(d, DayBegin(), DayEnd());
				}
			}
			else
			{
				if (dayly)
				{
					// Case 5: happens each year、day but within month range
					happened = CECTime::IsValidSingle(m, MonthBegin(), MonthEnd());
				}
				else
				{
					// Case 1: happens each year but within month and day range
					happened = CECTime::IsValidContinuous(m, d, MonthBegin(), DayBegin(), MonthEnd(), DayEnd());
				}
			}
		}
		else
		{
			if (monthly)
			{
				if (dayly)
				{
					// Case 6: happens each month、day but within year range
					happened = CECTime::IsValidSingle(y, YearBegin(), YearEnd());
				}
				else
				{
					// Case 2: happens each month but within year and day range
					happened = CECTime::IsValidSingle(y, YearBegin(), YearEnd());
					if (happened)
						happened = CECTime::IsValidSingle(d, DayBegin(), DayEnd());
				}
			}
			else
			{
				if (dayly)
				{
					// Case 3: happens each day but within year and month range
					happened = CECTime::IsValidContinuous(y, m, YearBegin(), MonthBegin(), YearEnd(), MonthEnd());
				}
				else
				{
					// case 0: happens within year、month、day range
					happened = CECTime::IsValidContinuous(y, m, d, YearBegin(), MonthBegin(), DayBegin(), YearEnd(), MonthEnd(), DayEnd());
				}
			}
		}
	}
	
	return happened;
}

bool CECAutoTeamConfig::Activity::IsHappenOnWeekDay(int w)const
{
	// Check whether the activity happens on given week day if it's weekly
	return ((1 << w) & this->p->week) != 0;
}

int CECAutoTeamConfig::Activity::YearBegin()const
{
	return p->day[0][0];
}
int CECAutoTeamConfig::Activity::YearEnd()const
{
	return p->day[1][0];
}
int CECAutoTeamConfig::Activity::MonthBegin()const
{
	return p->day[0][1];
}
int CECAutoTeamConfig::Activity::MonthEnd()const
{
	return p->day[1][1];
}
int CECAutoTeamConfig::Activity::DayBegin()const
{
	return p->day[0][2];
}
int CECAutoTeamConfig::Activity::DayEnd()const
{
	return p->day[1][2];
}

bool CECAutoTeamConfig::Activity::IsFullDayTime()const
{
	// Check whether the activity happens all day long
	//
	return full_day;
}


bool CECAutoTeamConfig::Activity::IsDisable() const
{
	return (p->combined_switch & ACS_DISABLED) ? true : false;
}

bool CECAutoTeamConfig::Activity::IsRecommend() const
{
	return (p->combined_switch & ACS_RECOMMENDED) ? true : false;
}

bool CECAutoTeamConfig::Activity::IsHappenIn(char h, char m)const
{
	// Check whether the activity happens in given hour/minute
	//
	return IsFullDayTime()
		|| CECTime::IsValidContinuous(h, m, p->daytime[0], p->daytime[1], p->daytime[2], p->daytime[3]);
}

bool CECAutoTeamConfig::Activity::CanJumpFrom(int world_id) const
{
	for( int i=0;i<sizeof(p->worldtag_from)/sizeof(int);i++ )
	{
		if( p->worldtag_from[i] && p->worldtag_from[i] == world_id )
			return true;
	}

	return false;
}

void CECAutoTeamConfig::Activity::SetConfig(const AUTOTEAM_CONFIG *pConfig)
{
	p = pConfig;

	//	构建关联任务快速查询结构
	m_tasks.clear();
	for (int i(0); i < sizeof(p->task)/sizeof(p->task[0]); ++ i)
	{
		int idTask = p->task[i];
		if (idTask <= 0){
			break;
		}
		m_tasks[idTask] = true;
	}
}

bool CECAutoTeamConfig::Activity::FindTask(int taskID)const
{
	return m_tasks.find(taskID) != m_tasks.end();
}

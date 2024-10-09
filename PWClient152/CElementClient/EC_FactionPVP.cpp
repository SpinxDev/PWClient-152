// File		: EC_FactionPVP.cpp
// Creator	: Xu Wenbin
// Date		: 2014/3/27

#include "EC_FactionPVP.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_FixedMsg.h"
#include "EC_DomainGuild.h"

#include "ExpTypes.h"
#include "elementdataman.h"

#include "factionresourcebattleplayerqueryresult.hpp"
#include "factionresourcebattlegetmap_re.hpp"
#include "factionresourcebattlegetrecord_re.hpp"
#include "factionresourcebattlenotifyplayerevent.hpp"

#include <AScriptFile.h>

#include <algorithm>

#define ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0]))

extern CECGame * g_pGame;

//	class CECFactionPVPModelChange
CECFactionPVPModelChange::CECFactionPVPModelChange(unsigned int changeMask)
: m_changeMask(changeMask)
, m_pScoreChange(NULL)
{
}

void CECFactionPVPModelChange::ScoreChanged(CECFactionPVPModelScoreChange * pScoreChange){
	m_pScoreChange = pScoreChange;
}

unsigned int CECFactionPVPModelChange::ChangeMask()const
{
	return m_changeMask;
}

bool CECFactionPVPModelChange::IsMapChanged()const
{
	return (ChangeMask() & MAP_CHANGE) != 0;
}

bool CECFactionPVPModelChange::IsInBattleStatusChanged()const
{
	return (ChangeMask() & INBATTLE_STATUS_CHANGE) != 0;
}

bool CECFactionPVPModelChange::IsRankListChanged()const
{
	return (ChangeMask() & RANKLIST_CHANGE) != 0;
}

const CECFactionPVPModelScoreChange * CECFactionPVPModelChange::ScoreChange()const{
	return m_pScoreChange;
}

//	class CECFactionPVPModelScoreChange
CECFactionPVPModelScoreChange::CECFactionPVPModelScoreChange(int scoreAdded, char scoreType)
: m_scoreAdded(scoreAdded)
, m_scoreType(static_cast<ScoreType>(scoreType)){
}

int	CECFactionPVPModelScoreChange::GetScore()const{
	return m_scoreAdded;
}

CECFactionPVPModelScoreChange::ScoreType CECFactionPVPModelScoreChange::GetScoreType()const{
	return m_scoreType;
}

//	class CECFactionPVPModel
CECFactionPVPModel & CECFactionPVPModel::Instance()
{
	static CECFactionPVPModel s_instance;
	return s_instance;
}

CECFactionPVPModel::CECFactionPVPModel()
: m_domainResourcePosition(32)
{
	Clear();
}

CECFactionPVPModel::ReportItemSortIndex CECFactionPVPModel::DefaultReportSortIndex()const
{
	//	返回数据默认排序针对的数据列
	return RISI_SCORE;
}

CECFactionPVPModel::ReportItemSortMethod CECFactionPVPModel::DefaultReportSortMethod()const
{
	//	返回数据默认排序方法，为降序排列（适合积分等从高到低情况）
	return RISM_DESENT;
}

CECFactionPVPModel::ReportItemSortIndex CECFactionPVPModel::ReportSortIndex()const
{
	return m_sortIndex;
}

CECFactionPVPModel::ReportItemSortMethod CECFactionPVPModel::ReportSortMethod()const
{
	//	返回数据默认排序方法，为降序排列（适合积分等从高到低情况）
	return m_sortMethod;
}

void CECFactionPVPModel::Clear()
{
	m_factionPVPOpen = false;
	m_inFactionPVP = false;
	m_inGuildBattle = false;

	m_mapReady = false;	
	m_domainWithResource.clear();

	m_inBattleStatusReady = false;
	m_idFaction = 0;
	m_score = 0;
	m_robbedMineCarCount = 0;
	m_robbedMineBaseCount = 0;
	m_canGetBonus = false;
	m_mineCarCountCanRob = -1;
	m_mineBaseCountCanRob = -1;

	m_reportListReady = false;
	m_reportList.clear();
	m_sortIndex = DefaultReportSortIndex();
	m_sortMethod = DefaultReportSortMethod();
}

bool CECFactionPVPModel::Init()
{
	m_domainResourcePosition.clear();

	AScriptFile sf;
	const char *szFile = "Configs\\faction_pvp.txt";
	if (!sf.Open(szFile)){
		a_LogOutput(1, "Failed to open file %s", szFile);
		return false;
	}
	while (sf.PeekNextToken(true))
	{
		int domain = sf.GetNextTokenAsInt(true);

		ResourcePosition rPos;
		bool bError(true);
		while (true)
		{
			//	 读取产矿位置
			if (sf.IsEnd()){
				break;
			}
			rPos.minePos.x = sf.GetNextTokenAsFloat(false);
			
			if (sf.IsEnd()){
				break;
			}
			rPos.minePos.y = sf.GetNextTokenAsFloat(false);
			
			if (sf.IsEnd()){
				break;
			}
			rPos.minePos.z = sf.GetNextTokenAsFloat(false);
			
			//	读取存矿位置
			if (sf.IsEnd()){
				break;
			}
			rPos.mineBasePos.x = sf.GetNextTokenAsFloat(false);
			
			if (sf.IsEnd()){
				break;
			}
			rPos.mineBasePos.y = sf.GetNextTokenAsFloat(false);
			
			if (sf.IsEnd()){
				break;
			}
			rPos.mineBasePos.z = sf.GetNextTokenAsFloat(false);
			bError = false;
			break;
		}
		if (bError){
			a_LogOutput(1, "In File %s, invalid format at line %d", szFile, sf.GetCurLine());
			ASSERT(0);
			m_domainResourcePosition.clear();
			break;
		}		
		if (!m_domainResourcePosition.put(domain, rPos)){
			a_LogOutput(1, "In File %s, repeated id(%d) ignored", szFile, domain);
			ASSERT(0);
			m_domainResourcePosition.clear();
			break;
		}
	}
	sf.Close();
	return true;
}

bool CECFactionPVPModel::IsFactionPVPOpen()const
{
	return m_factionPVPOpen;
}

bool CECFactionPVPModel::IsInFactionPVP()const
{
	return m_inFactionPVP;
}

bool CECFactionPVPModel::CanShowInBattleStatus()const
{
	//	有可能显示 gmapstatus3.xml 界面时返回 true，
	//	界面内容分两部分：战斗中间结果仅在战时显示，查看战果其它时间都显示
	//	受查看战果影响，非城战期间都显示整个对话框
	//	两者同时发生服务器会数据出错，但客户端增加使用 IsFactionPVPOpen() 标志控制开启s
	return !m_inGuildBattle || IsFactionPVPOpen();
}

bool CECFactionPVPModel::CanShowReportList()const
{
	//	有机会显示帮派 PVP 活动领奖报告时返回 true，由于客户端不好控制时机（由客户端判断时间会有误差），
	//	因此，除全服帮派 PVP 活动进行期间、其它时间都显示
	return !IsFactionPVPOpen();
}

bool CECFactionPVPModel::CanShowResource()const
{
	//	可显示 PVP 战剩余资源时返回 true，允许所有玩家都能看到
	//	全服 PVP 战期间都可显示
	return IsFactionPVPOpen() && FactionPVPMapReady();
}

bool CECFactionPVPModel::IsInGuildBattle()const
{
	return m_inGuildBattle;
}

bool CECFactionPVPModel::InBattleStatusReady()const
{
	return m_inBattleStatusReady;
}

int CECFactionPVPModel::FactionID()const
{
	return m_idFaction;
}

unsigned int CECFactionPVPModel::Score()const
{
	return m_score;
}

unsigned short CECFactionPVPModel::RobbedMineCarCount()const
{
	return m_robbedMineCarCount;
}

unsigned short CECFactionPVPModel::RobbedMineBaseCount()const
{
	return m_robbedMineBaseCount;
}


int	CECFactionPVPModel::MineCarCountCanRob()const
{
	return m_mineCarCountCanRob;
}

int CECFactionPVPModel::MineBaseCountCanRob()const
{
	return m_mineBaseCountCanRob;
}

bool CECFactionPVPModel::CanGetBonus()const
{
	return m_canGetBonus;
}

void CECFactionPVPModel::GetFactionPVPMap()
{
	//	未获取时、或者当前正在活动期间，都会尝试获取
	if (!FactionPVPMapReady() || IsFactionPVPOpen()){
		g_pGame->GetGameSession()->factionPVP_GetMap();
	}
}

bool CECFactionPVPModel::FactionPVPMapReady()const
{
	return m_mapReady;
}

CECFactionPVPModel::DomainWithResourceIterator CECFactionPVPModel::BeginDomainWithResource()const
{
	return m_domainWithResource.begin();
}

CECFactionPVPModel::DomainWithResourceIterator CECFactionPVPModel::EndDomainWithResource()const
{
	return m_domainWithResource.end();
}

bool CECFactionPVPModel::IsDomainWithResource(int domain)const
{
	return m_domainWithResource.find(domain) != m_domainWithResource.end();
}

const CECFactionPVPModel::ResourcePosition * CECFactionPVPModel::GetResourcePosition(int domain)const
{
	DomainResourcePosition::pair_type p = m_domainResourcePosition.get(domain);
	if (p.second){
		return p.first;
	}
	return NULL;
}

bool CECFactionPVPModel::ReportListReady()const
{
	return m_reportListReady;
}

int CECFactionPVPModel::ReportListCount()const
{
	return (int)m_reportList.size();
}

const CECFactionPVPModel::ReportItem * CECFactionPVPModel::ReportListItem(int index)const
{
	if (index < 0 || index >= ReportListCount()){
		ASSERT(false);
		return NULL;
	}
	return &m_reportList[index];
}

struct SortByKillCount 
{
	bool ascend;
	SortByKillCount(bool bAscend)
		:ascend(bAscend){
	}
	bool operator () (const CECFactionPVPModel::ReportItem &lhs, const CECFactionPVPModel::ReportItem &rhs)const
	{
		if (ascend){
			return lhs.killCount < rhs.killCount;
		}else{
			return lhs.killCount > rhs.killCount;
		}
	}
};

struct SortByDeathCount 
{
	bool ascend;
	SortByDeathCount(bool bAscend)
		:ascend(bAscend){
	}
	bool operator () (const CECFactionPVPModel::ReportItem &lhs, const CECFactionPVPModel::ReportItem &rhs)const
	{
		if (ascend){
			return lhs.deathCount < rhs.deathCount;
		}else{
			return lhs.deathCount > rhs.deathCount;
		}
	}
};

struct SortByUseToolCount 
{
	bool ascend;
	SortByUseToolCount(bool bAscend)
		:ascend(bAscend){
	}
	bool operator () (const CECFactionPVPModel::ReportItem &lhs, const CECFactionPVPModel::ReportItem &rhs)const
	{
		if (ascend){
			return lhs.useToolCount < rhs.useToolCount;
		}else{
			return lhs.useToolCount > rhs.useToolCount;
		}
	}
};

struct SortByScore
{
	bool ascend;
	SortByScore(bool bAscend)
		:ascend(bAscend){
	}
	bool operator () (const CECFactionPVPModel::ReportItem &lhs, const CECFactionPVPModel::ReportItem &rhs)const
	{
		if (ascend){
			return lhs.score < rhs.score;
		}else{
			return lhs.score > rhs.score;
		}
	}
};

void CECFactionPVPModel::SetSortMethod(ReportItemSortIndex sortIndex, ReportItemSortMethod sortMethod, bool bForceSort/*=false*/)
{
	if (!bForceSort && m_sortIndex == sortIndex && m_sortMethod == sortMethod){
		return;
	}
	m_sortIndex = sortIndex;
	m_sortMethod = sortMethod;
	if (sortMethod == RISM_NONE){
		//	按无序方式排列，即在下次收到服务器数据时不对其进行排序
		return;
	}
	switch (sortIndex)
	{
	case RISI_KILL_COUNT:
		{
			SortByKillCount sortMethod(sortMethod == RISM_ASCENT);
			std::sort(m_reportList.begin(), m_reportList.end(), sortMethod);
		}
		break;
	case RISI_DEATH_COUNT:
		{
			SortByDeathCount sortMethod(sortMethod == RISM_ASCENT);
			std::sort(m_reportList.begin(), m_reportList.end(), sortMethod);
		}
		break;
	case RISI_USE_TOOL_COUNT:
		{
			SortByUseToolCount sortMethod(sortMethod == RISM_ASCENT);
			std::sort(m_reportList.begin(), m_reportList.end(), sortMethod);
		}
		break;
	case RISI_SCORE:
		{
			SortByScore sortMethod(sortMethod == RISM_ASCENT);
			std::sort(m_reportList.begin(), m_reportList.end(), sortMethod);
		}
		break;
	}
}

void CECFactionPVPModel::OnPrtcFactionPVPResult(const GNET::FactionResourceBattlePlayerQueryResult *p)
{
	m_inBattleStatusReady = true;
	m_idFaction = p->faction_id;
	m_score = p->score;
	m_robbedMineCarCount = p->rob_horse_count;
	m_robbedMineBaseCount = p->rob_resource_count;
	m_canGetBonus = (p->can_get_bonus != 0);

	OnGuildBattleDomainOwnerChange();
}

void CECFactionPVPModel::OnGuildBattleDomainOwnerChange()
{
	//	根据本帮当前领土数、重新计算可抢夺的资源车和资源基地数量
	m_mineCarCountCanRob = -1;
	m_mineBaseCountCanRob = -1;

	while (m_idFaction > 0){
		int domainCountOfMyFaction = CECDomainGuildInfo::Instance().GetCountForOwner(m_idFaction);
		a_LogOutput(1, "CECFactionPVPModel::OnGuildBattleDomainOwnerChange, domainCountOfMyFaction=%d", domainCountOfMyFaction);
		DATA_TYPE dataType = DT_INVALID;
		const int CONFIG_ID = 1740;
		const void *p = g_pGame->GetElementDataMan()->get_data_ptr(CONFIG_ID, ID_SPACE_CONFIG, dataType);
		if (!p){
			a_LogOutput(1, "CECFactionPVPModel::OnGuildBattleDomainOwnerChange, Failed to get FACTION_PVP_CONFIG(id=%d)!", CONFIG_ID);
			break;
		}
		const FACTION_PVP_CONFIG *pConfig = (const FACTION_PVP_CONFIG *)(p);
		const int FACTION_PVP_CONFIG_LIST_COUNT = ARRAY_SIZE(pConfig->list);
		if (domainCountOfMyFaction < pConfig->list[0].domain_count){
			a_LogOutput(1, "CECFactionPVPModel::OnGuildBattleDomainOwnerChange, Invalid FACTION_PVP_CONFIG(id=%d)!", CONFIG_ID);
			break;
		}
		for (int i(0); i < FACTION_PVP_CONFIG_LIST_COUNT; ++ i){
			if (i+1 == FACTION_PVP_CONFIG_LIST_COUNT ||								//	1.最后一项时（如果 FACTION_PVP_CONFIG_LIST_COUNT 为 1，则此项满足要求，否则下面第 3 条保证此项比前倒数第二项大）
				domainCountOfMyFaction < pConfig->list[i+1].domain_count ||			//	2.位于[i, i+1)之间时，选 i
				pConfig->list[i].domain_count >= pConfig->list[i+1].domain_count){	//	3.要求有效各项配置是单调递增的，同时允许选项后面部分填 0
				m_mineCarCountCanRob = pConfig->list[i].minecar_count_can_rob;
				m_mineBaseCountCanRob = pConfig->list[i].minebase_count_can_rob;
				break;
			}
		}
		break;
	}

	CECFactionPVPModelChange change(CECFactionPVPModelChange::INBATTLE_STATUS_CHANGE);
	NotifyObservers(&change);
}

void CECFactionPVPModel::OnPrtcFactionPVPGetMapRe(const GNET::FactionResourceBattleGetMap_Re *p)
{	
	m_mapReady = true;

	m_factionPVPOpen = (p->retcode == 0);
	m_domainWithResource.clear();
	m_domainWithResource.insert(p->domains.begin(), p->domains.end());

	if (p->domains.empty()){
		a_LogOutput(1, "CECFactionPVPModel::OnPrtcFactionPVPGetMapRe, retcode=%d, no resource!", p->retcode);
	}else{
		AString strResourceIDs;
		for (size_t u(0); u < p->domains.size(); ++ u){
			if (!strResourceIDs.IsEmpty()){
				strResourceIDs += ", ";
			}
			strResourceIDs += AString().Format("%d", p->domains[u]);
		}
		a_LogOutput(1, AString().Format("CECFactionPVPModel::OnPrtcFactionPVPGetMapRe, retcode=%d, resource=%s!", p->retcode, strResourceIDs));
	}
	
	CECFactionPVPModelChange change(CECFactionPVPModelChange::MAP_CHANGE);
	NotifyObservers(&change);
}

void CECFactionPVPModel::OnPrtcFactionPVPGetRecordRe(const GNET::FactionResourceBattleGetRecord_Re *p)
{
	m_reportList.clear();
	m_reportListReady = false;
	if (p->retcode){
		int iFixedMsg(-1);
		switch (p->retcode)
		{
		case 1:
			iFixedMsg = FIXMSG_FACTION_PVP_GETRANK_ERROR_TIME;
			break;
		case 2:
			iFixedMsg = FIXMSG_FACTION_PVP_GETRANK_PERMISSION_DENIED;
			break;
		}
		CECGameRun *pGameRun = g_pGame->GetGameRun();
		if (iFixedMsg > 0){
			pGameRun->AddFixedMessage(iFixedMsg);
		}else{
			ACString strMsg;
			strMsg.Format(_AL("FactionResourceBattleGetRecord_Re:unknown retcode=%d"), p->retcode);
			pGameRun->AddChatMessage(strMsg, GP_CHAT_SYSTEM);
		}
	}else{
		m_reportListReady = true;
		size_t count = p->records.size();
		m_reportList.resize(count);
		for (size_t u = 0; u < count; ++ u)
		{
			const GFactionResourceBattleRecord &src = p->records[u];
			ReportItem &dst = m_reportList[u];
			dst.roleid = src.roleid;
			dst.killCount = src.kill_count;
			dst.deathCount = src.death_count;
			dst.useToolCount = src.use_tool_count;
			dst.score = src.score;
		}
		SetSortMethod(m_sortIndex, m_sortMethod, true);	//	针对新数据、应用之前已经设置的排序方法，保持一致性
	}
	CECFactionPVPModelChange change(CECFactionPVPModelChange::RANKLIST_CHANGE);
	NotifyObservers(&change);
}

void CECFactionPVPModel::OnPrtcFactionPVPNotifyPlayerEvent(const GNET::FactionResourceBattleNotifyPlayerEvent *p)
{
	switch (p->event_type){
	case CECFactionPVPModelScoreChange::EVENT_ROB_MINECAR:
		break;
	case CECFactionPVPModelScoreChange::EVENT_ROB_MINEBASE:
		break;
	default:
		ASSERT(false);
		a_LogOutput(1, "CECFactionPVPModel::OnPrtcFactionPVPNotifyPlayerEvent, invalid event_type(%d) ignored!", p->event_type);
		return;
	}	
	CECFactionPVPModelChange change(CECFactionPVPModelChange::INBATTLE_STATUS_CHANGE);
	CECFactionPVPModelScoreChange scoreChange(p->score, p->event_type);
	change.ScoreChanged(&scoreChange);
	NotifyObservers(&change);
	
	QueryFactionPVPInfo();	//	积分和击杀次数有更新，再次查询
}

void CECFactionPVPModel::OnFactionPVPOpen(bool bOpenOrClose)
{
	if (bOpenOrClose == m_factionPVPOpen){
		return;
	}
	m_factionPVPOpen = bOpenOrClose;

	//	CanShowInBattleStatus() CanShowReportList() CanShowResource() 均依赖此标志（通过 IsFactionPVPOpen()）
	//	故均清空更新
	unsigned int changeMask = 0;
	m_inBattleStatusReady = false;
	changeMask |= CECFactionPVPModelChange::INBATTLE_STATUS_CHANGE;
	
	m_domainWithResource.clear();
	m_mapReady = false;
	changeMask |= CECFactionPVPModelChange::MAP_CHANGE;
	GetFactionPVPMap();				//	主动更新资源位置等信息（小地图等显示随时需要）
	
	m_reportList.clear();
	m_reportListReady = false;
	m_sortIndex = DefaultReportSortIndex();
	m_sortMethod = DefaultReportSortMethod();
	changeMask |= CECFactionPVPModelChange::RANKLIST_CHANGE;
	if (CanShowReportList()){
		g_pGame->GetGameSession()->factionPVP_GetRank();
	}
	
	CECFactionPVPModelChange change(changeMask);
	NotifyObservers(&change);
}

void CECFactionPVPModel::OnJoinFactionPVP(bool bJoinOrLeave)
{
	//	bJoinOrLeave = true，已加入帮派、且本帮 PVP 已打开
	//	bJoinOrLeave = false, 本帮 PVP 未打开、或完成 PVP 活动
	if (bJoinOrLeave == m_inFactionPVP){
		return;
	}
	m_inFactionPVP = bJoinOrLeave;

	//	加入或离开帮派PVP，对下列数据都有影响（隐含逻辑），故都需要更新
	unsigned int changeMask = 0;
	m_inBattleStatusReady = false;
	changeMask |= CECFactionPVPModelChange::INBATTLE_STATUS_CHANGE;
	QueryFactionPVPInfo();
	
	CECFactionPVPModelChange change(changeMask);
	NotifyObservers(&change);
}

void CECFactionPVPModel::OnGuildBattleEnter(bool bEnterOrExit)
{
	//	bEnterOrExit = true，当前正在城战战斗中
	if (bEnterOrExit == m_inGuildBattle){
		return;
	}
	m_inGuildBattle = bEnterOrExit;
	
	CECFactionPVPModelChange change(CECFactionPVPModelChange::INBATTLE_STATUS_CHANGE);
	NotifyObservers(&change);
}

void CECFactionPVPModel::QueryFactionPVPInfo()
{
	if (CanShowInBattleStatus()){
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		int idFaction = pHost->GetFactionID();
		if (pHost->IsInFactionPVP() && idFaction > 0){
			g_pGame->GetGameSession()->c2s_CmdQueryFactionPVPInfo(idFaction);
		}
	}
}
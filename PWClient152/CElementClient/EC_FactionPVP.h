// File		: EC_FactionPVP.h
// Creator	: Xu Wenbin
// Date		: 2014/3/27

#pragma once

#include "EC_Observer.h"

#include <vector.h>
#include <hashtab.h>
#include <A3DVector.h>

#include <set>
#include <vector>

namespace GNET
{
	class FactionResourceBattlePlayerQueryResult;
	class FactionResourceBattleGetMap_Re;
	class FactionResourceBattleGetRecord_Re;
	class FactionResourceBattleNotifyPlayerEvent;
}
class AWString;

//	class CECFactionPVPModelChange
//	类 CECFactionPVPModel 数据改变通知类型
class CECFactionPVPModelScoreChange;
class CECFactionPVPModelChange : public CECObservableChange
{
public:
	enum ChangeMask{
		MAP_CHANGE					=	0x01,
		INBATTLE_STATUS_CHANGE		=	0x02,
		RANKLIST_CHANGE				=	0x04,
	};
private:
	unsigned int	m_changeMask;
	CECFactionPVPModelScoreChange *m_pScoreChange;
public:
	CECFactionPVPModelChange(unsigned int changeMask);
	void ScoreChanged(CECFactionPVPModelScoreChange * pScoreChange);

	unsigned int	ChangeMask()const;
	bool	IsMapChanged()const;
	bool	IsInBattleStatusChanged()const;
	bool	IsRankListChanged()const;

	const CECFactionPVPModelScoreChange * ScoreChange()const;
};

class CECFactionPVPModelScoreChange : public CECObservableChange
{
public:
	enum ScoreType{
		EVENT_ROB_MINECAR = 1,		//	击杀矿车导致积分增加
		EVENT_ROB_MINEBASE,			//	摧毁矿产基地导致积分增加
    };
private:
	int			m_scoreAdded;		//	INBATTLE_STATUS_SCORE_CHANGE 改变对应的积分增加
	ScoreType	m_scoreType;		//	m_scoreAdded 增加的原因
public:	
	CECFactionPVPModelScoreChange(int scoreAdded, char scoreType);
	int			GetScore()const;
	ScoreType	GetScoreType()const;
};

//	定义 CECFactionPVPModel 的观察者
class CECFactionPVPModel;
typedef CECObserver<CECFactionPVPModel>	CECFactionPVPModelObserver;

//	帮派 PVP 数据模型
//	minebase: 存矿点（资源基地）
//	minecar: 运矿工具（资源车）
//	mine: 矿物
class CECFactionPVPModel : public CECObservable<CECFactionPVPModel>
{
public:
	struct  ResourcePosition			//	资源位置
	{
		A3DVECTOR3	minePos;			//	产矿位置
		A3DVECTOR3	mineBasePos;		//	存矿位置
	};
	struct  ReportItem
	{
		int				roleid;			//	帮友ID
		unsigned short	killCount;		//	击杀玩家次数，限有资源的领土、及开启了掠夺模式的玩家
		unsigned short	deathCount;		//	被击杀次数，限有资源的领土、及对方也开启了掠夺模式
		unsigned short	useToolCount;	//	有效使用资源保护道具次数
		int				score;			//	掠夺到的积分
	};
	enum ReportItemSortIndex
	{
		RISI_KILL_COUNT,				//	按 ReportItem::killCount 排序
		RISI_DEATH_COUNT,				//	按 ReportItem::deathCount 排序
		RISI_USE_TOOL_COUNT,			//	按 ReportItem::useToolCount 排序
		RISI_SCORE,						//	按 ReportItem::score 排序
	};
	enum ReportItemSortMethod{
		RISM_NONE,						//	未排序
		RISM_ASCENT,					//	升序
		RISM_DESENT,					//	降序
	};

private:
	bool			m_inFactionPVP;			//	本帮PVP已打开为 true，其它为 false，此数据始终有效
	bool			m_factionPVPOpen;		//	当前服务器PVP活动是否开启，此数据始终有效
	bool			m_inGuildBattle;		//	当前正进行城战战斗为 true，宣战及其它状态为 false，此数据始终有效

	bool			m_mapReady;				//	m_domainWithResource 等地图数据是否可用
	typedef std::set<int>	DomainSet;		//	领土ID集合
	DomainSet		m_domainWithResource;	//	尚有存矿点未被打爆的领土

	bool			m_inBattleStatusReady;	//	战斗中当前状态数据是否可用
	int				m_idFaction;			//	本帮ID
	unsigned int	m_score;				//	本帮当前积分
	unsigned short	m_robbedMineCarCount;	//	本帮掠夺资源车数量
	unsigned short	m_robbedMineBaseCount;	//	本帮掠夺资源基地数量
	bool			m_canGetBonus;			//	是否可以领奖
	int				m_mineCarCountCanRob;	//	可掠夺资源车数
	int				m_mineBaseCountCanRob;	//	可掠夺资源基地数

	typedef std::vector<ReportItem> ReportList;	//	PVP结果报告
	ReportList		m_reportList;
	bool			m_reportListReady;		//	m_reportList 数据是否可用
	ReportItemSortIndex		m_sortIndex;
	ReportItemSortMethod	m_sortMethod;

	typedef abase::hashtab<ResourcePosition, int, abase::_hash_function>	DomainResourcePosition;	//	领土ID：资源位置
	DomainResourcePosition	m_domainResourcePosition;	//	常量数据，始终有效
	
	CECFactionPVPModel();

	//	单例禁用
	CECFactionPVPModel(const CECFactionPVPModel &);
	CECFactionPVPModel & operator = (const CECFactionPVPModel &);

public:

	static CECFactionPVPModel &Instance();

	//	初始化相关
	bool Init();
	void Clear();

	//	总体信息
	bool IsFactionPVPOpen()const;
	bool FactionPVPMapReady()const;
	void GetFactionPVPMap();
	bool IsInGuildBattle()const;

	//	本帮信息查询
	bool IsInFactionPVP()const;
	bool CanShowInBattleStatus()const;
	bool CanShowReportList()const;
	void QueryFactionPVPInfo();
	bool InBattleStatusReady()const;
	int			 FactionID()const;
	unsigned int Score()const;
	unsigned short RobbedMineCarCount()const;
	unsigned short RobbedMineBaseCount()const;
	int	MineCarCountCanRob()const;
	int MineBaseCountCanRob()const;
	bool		 CanGetBonus()const;

	//	资源存在性查询
	bool CanShowResource()const;
	typedef DomainSet::const_iterator DomainWithResourceIterator;
	DomainWithResourceIterator BeginDomainWithResource()const;
	DomainWithResourceIterator EndDomainWithResource()const;
	bool IsDomainWithResource(int domain)const;

	//	资源位置获取
	const ResourcePosition * GetResourcePosition(int domain)const;

	//	结果排行榜查询
	bool ReportListReady()const;
	int	 ReportListCount()const;
	const ReportItem * ReportListItem(int index)const;
	ReportItemSortIndex		DefaultReportSortIndex()const;
	ReportItemSortMethod	DefaultReportSortMethod()const;
	ReportItemSortIndex		ReportSortIndex()const;
	ReportItemSortMethod	ReportSortMethod()const;
	void SetSortMethod(ReportItemSortIndex sortIndex, ReportItemSortMethod sortMethod, bool bForceSort = false);

	//	数据改变
	void OnPrtcFactionPVPResult(const GNET::FactionResourceBattlePlayerQueryResult *p);
	void OnPrtcFactionPVPGetMapRe(const GNET::FactionResourceBattleGetMap_Re *p);
	void OnPrtcFactionPVPGetRecordRe(const GNET::FactionResourceBattleGetRecord_Re *p);
	void OnPrtcFactionPVPNotifyPlayerEvent(const GNET::FactionResourceBattleNotifyPlayerEvent *p);
	void OnFactionPVPOpen(bool bOpenOrClose);
	void OnJoinFactionPVP(bool bJoinOrLeave);
	void OnGuildBattleEnter(bool bEnterOrExit);
	void OnGuildBattleDomainOwnerChange();
};

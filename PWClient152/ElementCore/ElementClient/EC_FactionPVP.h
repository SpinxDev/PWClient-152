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
//	�� CECFactionPVPModel ���ݸı�֪ͨ����
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
		EVENT_ROB_MINECAR = 1,		//	��ɱ�󳵵��»�������
		EVENT_ROB_MINEBASE,			//	�ݻٿ�����ص��»�������
    };
private:
	int			m_scoreAdded;		//	INBATTLE_STATUS_SCORE_CHANGE �ı��Ӧ�Ļ�������
	ScoreType	m_scoreType;		//	m_scoreAdded ���ӵ�ԭ��
public:	
	CECFactionPVPModelScoreChange(int scoreAdded, char scoreType);
	int			GetScore()const;
	ScoreType	GetScoreType()const;
};

//	���� CECFactionPVPModel �Ĺ۲���
class CECFactionPVPModel;
typedef CECObserver<CECFactionPVPModel>	CECFactionPVPModelObserver;

//	���� PVP ����ģ��
//	minebase: ���㣨��Դ���أ�
//	minecar: �˿󹤾ߣ���Դ����
//	mine: ����
class CECFactionPVPModel : public CECObservable<CECFactionPVPModel>
{
public:
	struct  ResourcePosition			//	��Դλ��
	{
		A3DVECTOR3	minePos;			//	����λ��
		A3DVECTOR3	mineBasePos;		//	���λ��
	};
	struct  ReportItem
	{
		int				roleid;			//	����ID
		unsigned short	killCount;		//	��ɱ��Ҵ�����������Դ�����������������Ӷ�ģʽ�����
		unsigned short	deathCount;		//	����ɱ������������Դ�����������Է�Ҳ�������Ӷ�ģʽ
		unsigned short	useToolCount;	//	��Чʹ����Դ�������ߴ���
		int				score;			//	�Ӷᵽ�Ļ���
	};
	enum ReportItemSortIndex
	{
		RISI_KILL_COUNT,				//	�� ReportItem::killCount ����
		RISI_DEATH_COUNT,				//	�� ReportItem::deathCount ����
		RISI_USE_TOOL_COUNT,			//	�� ReportItem::useToolCount ����
		RISI_SCORE,						//	�� ReportItem::score ����
	};
	enum ReportItemSortMethod{
		RISM_NONE,						//	δ����
		RISM_ASCENT,					//	����
		RISM_DESENT,					//	����
	};

private:
	bool			m_inFactionPVP;			//	����PVP�Ѵ�Ϊ true������Ϊ false��������ʼ����Ч
	bool			m_factionPVPOpen;		//	��ǰ������PVP��Ƿ�����������ʼ����Ч
	bool			m_inGuildBattle;		//	��ǰ�����г�սս��Ϊ true����ս������״̬Ϊ false��������ʼ����Ч

	bool			m_mapReady;				//	m_domainWithResource �ȵ�ͼ�����Ƿ����
	typedef std::set<int>	DomainSet;		//	����ID����
	DomainSet		m_domainWithResource;	//	���д���δ���򱬵�����

	bool			m_inBattleStatusReady;	//	ս���е�ǰ״̬�����Ƿ����
	int				m_idFaction;			//	����ID
	unsigned int	m_score;				//	���ﵱǰ����
	unsigned short	m_robbedMineCarCount;	//	�����Ӷ���Դ������
	unsigned short	m_robbedMineBaseCount;	//	�����Ӷ���Դ��������
	bool			m_canGetBonus;			//	�Ƿ�����콱
	int				m_mineCarCountCanRob;	//	���Ӷ���Դ����
	int				m_mineBaseCountCanRob;	//	���Ӷ���Դ������

	typedef std::vector<ReportItem> ReportList;	//	PVP�������
	ReportList		m_reportList;
	bool			m_reportListReady;		//	m_reportList �����Ƿ����
	ReportItemSortIndex		m_sortIndex;
	ReportItemSortMethod	m_sortMethod;

	typedef abase::hashtab<ResourcePosition, int, abase::_hash_function>	DomainResourcePosition;	//	����ID����Դλ��
	DomainResourcePosition	m_domainResourcePosition;	//	�������ݣ�ʼ����Ч
	
	CECFactionPVPModel();

	//	��������
	CECFactionPVPModel(const CECFactionPVPModel &);
	CECFactionPVPModel & operator = (const CECFactionPVPModel &);

public:

	static CECFactionPVPModel &Instance();

	//	��ʼ�����
	bool Init();
	void Clear();

	//	������Ϣ
	bool IsFactionPVPOpen()const;
	bool FactionPVPMapReady()const;
	void GetFactionPVPMap();
	bool IsInGuildBattle()const;

	//	������Ϣ��ѯ
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

	//	��Դ�����Բ�ѯ
	bool CanShowResource()const;
	typedef DomainSet::const_iterator DomainWithResourceIterator;
	DomainWithResourceIterator BeginDomainWithResource()const;
	DomainWithResourceIterator EndDomainWithResource()const;
	bool IsDomainWithResource(int domain)const;

	//	��Դλ�û�ȡ
	const ResourcePosition * GetResourcePosition(int domain)const;

	//	������а��ѯ
	bool ReportListReady()const;
	int	 ReportListCount()const;
	const ReportItem * ReportListItem(int index)const;
	ReportItemSortIndex		DefaultReportSortIndex()const;
	ReportItemSortMethod	DefaultReportSortMethod()const;
	ReportItemSortIndex		ReportSortIndex()const;
	ReportItemSortMethod	ReportSortMethod()const;
	void SetSortMethod(ReportItemSortIndex sortIndex, ReportItemSortMethod sortMethod, bool bForceSort = false);

	//	���ݸı�
	void OnPrtcFactionPVPResult(const GNET::FactionResourceBattlePlayerQueryResult *p);
	void OnPrtcFactionPVPGetMapRe(const GNET::FactionResourceBattleGetMap_Re *p);
	void OnPrtcFactionPVPGetRecordRe(const GNET::FactionResourceBattleGetRecord_Re *p);
	void OnPrtcFactionPVPNotifyPlayerEvent(const GNET::FactionResourceBattleNotifyPlayerEvent *p);
	void OnFactionPVPOpen(bool bOpenOrClose);
	void OnJoinFactionPVP(bool bJoinOrLeave);
	void OnGuildBattleEnter(bool bEnterOrExit);
	void OnGuildBattleDomainOwnerChange();
};

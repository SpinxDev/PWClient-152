#pragma once

#include "hashmap.h"
#include "vector.h"
#include "AList2.h"
#include "AAssist.h"
#include "gfactionbaseinfo"
#include "EC_Global.h"
#include "EC_Observer.h"

namespace GNET
{
	class FactionOPRequest_Re;
	class FactionCreate_Re;
	class FactionAcceptJoin_Re;
	class FactionExpel_Re;
	class FactionDelayExpelAnnounce;
	class FactionBroadcastNotice_Re;
	class FactionMasterResign_Re;
	class FactionResign_Re;
	class FactionAppoint_Re;
	class FactionLeave_Re;
	class FactionUpgrade_Re;
	class FactionDegrade_Re;
	class FactionApplyJoin_Re;
	class FactionChat;
	class FactionListMember_Re;
	class GetFactionBaseInfo_Re;
	class FactionDismiss_Re;
	class FactionInviteJoin;
	class GetPlayerFactionInfo_Re;
	class FactionRename_Re;
	class FactionChangProclaim_Re;
	class FMemberInfo;
	class FactionRenameAnnounce;
}

#define FO_OPERATOR_UNK		1
#define FO_OPERANT_UNK		2

class Faction_Info
{
public:
	Faction_Info() {}
	~Faction_Info() {}

protected:
	unsigned int m_id;
	ACString m_szName;
	int m_nLev;
	int m_nMemNum;

public:
	unsigned int GetID() const { return m_id; }
	void SetID(unsigned int nId) { m_id = nId; }
	const ACHAR* GetName() const { return m_szName; }
	void SetName(const void* name, int len) { _cp_str(m_szName, name, len); }
	int GetLevel() const { return m_nLev; }
	void SetLevel(int nLev) { m_nLev = nLev; }
	int GetMemNum() const { return m_nMemNum; }
	void SetMemNum(int nNum) { m_nMemNum = nNum; }
};

typedef abase::hash_map<unsigned int, Faction_Info*> FactionInfoMap;

class Faction_Mem_Info
{
public:
	Faction_Mem_Info() {}
	~Faction_Mem_Info() {}

protected:
	int roleid;
	int froleid;
	int lev;
	int prof;
	unsigned char online_status;
	unsigned char gender;
	int logintime;		//	服务器时间只精确到天
	ACString nickname;
	int	contrib;
	bool delay_expel;
	DWORD expel_end_time;
	int reputation;
	int reincarnation_times;


public:
	int GetID() const { return roleid; }
	int GetFRoleID() const { return froleid; }
	int GetLev() const { return lev; }
	int GetProf() const { return prof; }
	unsigned char GetGender()const{ return gender; }
	int GetLoginTime()const{ return logintime; }
	bool IsOnline() const { return online_status != 0; }
	const ACHAR* GetName() const;
	const ACHAR* GetNickName() const { return nickname; }
	int	GetContrib()const { return contrib; }
	bool IsDelayExpel()const{ return delay_expel; }
	int GetReputation()const{ return reputation; }
	int GetReincarnationTimes()const{ return reincarnation_times; }

	void SetID(int id) { roleid = id; }
	void SetFRoleID(int id) { froleid = id; }
	void SetLev(int l) { lev = l; }
	void SetProf(int p) { prof = p; }
	void SetGender(unsigned char g){ gender = g; }
	void SetLoginDay(int l){ logintime = l; }
	void SetOnline(unsigned char status) { online_status = status; }
	void SetName(const void* s, int len);
	void SetName(const ACHAR* szName);
	void SetNickName(const void* s, int len) { _cp_str(nickname, s, len); }
	void SetContrib(int c){ contrib = c; }
	void SetDelayExpel(bool delayexpel, DWORD expeltime){ delay_expel = delayexpel; expel_end_time = expeltime; }
	void SetReputation(int rep){ reputation = rep; }
	void SetReincarnationTimes(int times){ reincarnation_times = times; }

	void SetAll(const GNET::FMemberInfo &info, DWORD currentTime);
};

typedef abase::vector<Faction_Mem_Info*> FactionMemList;

//	class CECFactionManChange
//	类 CECFactionMan 数据改变通知类型
class CECFactionManChange : public CECObservableChange
{
public:
	enum ChangeMask{
		ADD_FACTION,				//	有新的帮派信息添加
		FACTION_RENAMEFACTION,		//	帮派改名
	};
private:
	unsigned int	m_changeMask;
public:
	CECFactionManChange(unsigned int changeMask);
	unsigned int	ChangeMask()const;
};

//	有新的帮派信息添加
class CECFactionManFactionChange : public CECFactionManChange
{	
	unsigned int		m_idFaction;
	const Faction_Info *m_pFactionInfo;
public:
	CECFactionManFactionChange(unsigned int mask, unsigned int idFaction, const Faction_Info *pFactionInfo);
	int FactionID()const;
	const Faction_Info * FactionInfo()const;
};

//	定义 CECFactionMan 的观察者
class CECFactionMan;
typedef CECObserver<CECFactionMan>	CECFactionManObserver;

//	class CECFactionMan
class CECFactionMan : public CECObservable<CECFactionMan>
{
public:
	CECFactionMan() : m_bMemInfoReady(false), m_MemMap(256) { m_MemList.reserve(256); }
	~CECFactionMan() { Release(); }
	void Release(bool bOnlyFreeMember = false);

protected:
	typedef APtrList<FactionInviteJoin*> PendingInviteList;
	typedef abase::hash_map<int, Faction_Mem_Info*> FactionMemMap;

	FactionInfoMap m_FactionMap;
	PendingInviteList m_PendingInviteList;
	FactionMemList m_MemList;
	FactionMemMap m_MemMap;
	bool m_bMemInfoReady;
	ACString m_strProclaim;

	typedef abase::vector<int>		FactionRelationArray;
	FactionRelationArray	m_alliance;
	FactionRelationArray	m_hostile;

protected:
	void AddFixedMsg(int iMsg);
	void RemoveMember(int iRoleId);
	void ClearHostFaction();
	const ACHAR* GetMemName(int iRoleId);
	const ACHAR* GetMemNickName(int iRoleId);

public:
	// Operations
	Faction_Info* GetFaction(unsigned int uId, bool bRequestFromServer = false);
	void RefreshMemList(int version=0);
	FactionMemList& GetMemList() { return m_MemList; }
	bool IsMemInfoReady() const { return m_bMemInfoReady; }
	const ACHAR* GetProclaim() const { return m_strProclaim; }

	Faction_Info* AddFaction(unsigned int uId, const void* name, int len, int nLev, int nMemNum);
	void ResetMemInfoList()
	{
		for (size_t i = 0; i < m_MemList.size(); i++)
			delete m_MemList[i];

		m_MemList.clear();
		m_MemMap.clear();
	}
	Faction_Mem_Info* GetMember(int nRoleId)
	{
		FactionMemMap::iterator it = m_MemMap.find(nRoleId);

		if (it == m_MemMap.end()) return NULL;
		return it->second;
	}
	Faction_Mem_Info* AddNewMember(int nRoleId, const ACHAR* szName, int lev, int prof, unsigned char gender, int repuration, int reincarnation_times);
	void RefreshOlStatus(const IntVector* pList);

	static const ACHAR* GetFRoleName(unsigned int iFRoleId);
	
	typedef DWORD TimeType;
	static TimeType GetCurrentTime();
	static int	ConverToSeconds(TimeType t);
	static TimeType ConvertFromSeconds(int sec);
	static TimeType GetTimeLeft(TimeType end_time);
	
	Faction_Info * FindFactionByName(const ACHAR *szName);

	void SetHostFactionRelation(const abase::vector<int> &alliance, const abase::vector<int> &hostile);

	bool IsFactionAlliance(int fid);
	bool IsFactionHostile(int fid);

	// Msgs
	void OnOpRequest(const FactionOPRequest_Re* p);
	void OnFactionCreate(const FactionCreate_Re* p);
	void OnBroadcastNotice(const FactionBroadcastNotice_Re* p);
	DWORD OnChat(const FactionChat* p);
	DWORD OnAcceptJoin(const FactionAcceptJoin_Re* p);
	DWORD OnExpel(const FactionExpel_Re* p);
	DWORD OnDelayExpel(const FactionDelayExpelAnnounce* p);	
	DWORD OnDelayExpelMessage(const FactionDelayExpelAnnounce* p);
	DWORD OnMasterResign(const FactionMasterResign_Re* p);
	DWORD OnResign(const FactionResign_Re* p);
	DWORD OnAppoint(const FactionAppoint_Re* p);
	DWORD OnLeave(const FactionLeave_Re* p);
	DWORD OnRename(const FactionRename_Re* p);
	void OnUpgrade(const FactionUpgrade_Re* p);
	void OnDegrade(const FactionDegrade_Re* p);
	void OnApplyJoin(const FactionApplyJoin_Re* p);
	void OnListMember(const FactionListMember_Re* p);
	void OnBaseInfo(const GetFactionBaseInfo_Re* p);
	void OnDismiss(const FactionDismiss_Re*);
	void OnInviteJoin(const FactionInviteJoin* p);
	void OnPlayerInfo(const GetPlayerFactionInfo_Re* p);
	void OnChangeProclaim(const FactionChangProclaim_Re* p);
	void OnFactionRename(const FactionRenameAnnounce* p);
};

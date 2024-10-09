#include "EC_Global.h"
#include "factionoprequest_re.hpp"
#include "factioncreate_re.hpp"
#include "factionacceptjoin_re.hpp"
#include "factionexpel_re.hpp"
#include "factiondelayexpelannounce.hpp"
#include "factionbroadcastnotice_re.hpp"
#include "factionmasterresign_re.hpp"
#include "factionresign_re.hpp"
#include "factionappoint_re.hpp"
#include "factionleave_re.hpp"
#include "factionupgrade_re.hpp"
#include "factiondegrade_re.hpp"
#include "factionlistmember_re.hpp"
#include "getfactionbaseinfo_re.hpp"
#include "factiondismiss_re.hpp"
#include "factionapplyjoin_re.hpp"
#include "factioninvitejoin.hrp"
#include "factioninvitearg"
#include "getplayerfactioninfo.hpp"
#include "GetPlayerFactionInfo_Re.hpp"
#include "FactionRename_Re.hpp"
#include "factionchat.hpp"
#include "factionchangproclaim_re.hpp"
#include "factionrenameannounce.hpp"
#include "gnetdef.h"
#include "ids.hxx"

#include "EC_Faction.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_FixedMsg.h"
#include "EC_ElsePlayer.h"
#include "EC_ManPlayer.h"
#include "chatdata.h"
#include "AUICommon.h"
#include "DlgGuildMan.h"

#define sizeof_arr(a) (sizeof(a) / sizeof((a)[0]))

const ACHAR* Faction_Mem_Info::GetName() const
{
	const ACHAR* szName = g_pGame->GetGameRun()->GetPlayerName(roleid, false);
	return szName ? szName : _AL("");
}

void Faction_Mem_Info::SetName(const void* s, int len)
{
	ACString name;
	_cp_str(name, s, len);
	SetName(name);
}

void Faction_Mem_Info::SetName(const ACHAR* szName)
{
	g_pGame->GetGameRun()->AddPlayerName(roleid, szName);
}

//	class CECFactionManChange
CECFactionManChange::CECFactionManChange(unsigned int changeMask)
: m_changeMask(changeMask){
}

unsigned int CECFactionManChange::ChangeMask()const{
	return m_changeMask;
}

//	class CECFactionManFactionChange
CECFactionManFactionChange::CECFactionManFactionChange(unsigned int mask, unsigned int idFaction, const Faction_Info *pFactionInfo)
: CECFactionManChange(mask)
, m_idFaction(idFaction)
, m_pFactionInfo(pFactionInfo){
}

int CECFactionManFactionChange::FactionID()const{
	return m_idFaction;
}

const Faction_Info * CECFactionManFactionChange::FactionInfo()const{
	return m_pFactionInfo;
}

//	class CECFactionMan
const ACHAR* CECFactionMan::GetFRoleName(unsigned int iFRoleId)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	return pGameUI->GetStringFromTable(1300+iFRoleId);
}

CECFactionMan::TimeType CECFactionMan::GetCurrentTime()
{
	return timeGetTime();
};

int	CECFactionMan::ConverToSeconds(TimeType t)
{
	return t/1000;
}

CECFactionMan::TimeType CECFactionMan::ConvertFromSeconds(int sec)
{
	return static_cast<TimeType>(sec * 1000);
}

CECFactionMan::TimeType CECFactionMan::GetTimeLeft(TimeType endTime)
{
	TimeType t = GetCurrentTime();
	return (t >= endTime) ? 0 : (endTime-t);
}

void CECFactionMan::Release(bool bOnlyFreeMember)
{
	if (!bOnlyFreeMember)
	{
		FactionInfoMap::iterator it = m_FactionMap.begin();

		for (; it != m_FactionMap.end(); ++it)
			delete it->second;

		m_FactionMap.clear();
	}

	ResetMemInfoList();
	m_bMemInfoReady = false;
	m_strProclaim.Empty();

	ALISTPOSITION pos = m_PendingInviteList.GetHeadPosition();
	while (pos) m_PendingInviteList.GetNext(pos)->Destroy();
	m_PendingInviteList.RemoveAll();
	
	m_alliance.clear();
	m_hostile.clear();
}

Faction_Info* CECFactionMan::GetFaction(unsigned int uId, bool bRequestFromServer)
{
	FactionInfoMap::iterator it = m_FactionMap.find(uId);

	if (it == m_FactionMap.end())
	{
		if (bRequestFromServer) g_pGame->GetGameSession()->GetFactionInfo(1, reinterpret_cast<int*>(&uId));
		return NULL;
	}

	return it->second;
}

inline void CECFactionMan::AddFixedMsg(int iMsg)
{
#define MSG_CASE(msg) \
{\
	case msg:\
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_##msg);\
		break;\
}

	switch (iMsg)
	{
	MSG_CASE(ERR_FC_NETWORKERR)
	MSG_CASE(ERR_FC_INVALID_OPERATION)
    MSG_CASE(ERR_FC_OP_TIMEOUT)
    MSG_CASE(ERR_FC_CREATE_ALREADY)
    MSG_CASE(ERR_FC_CREATE_DUP)
    MSG_CASE(ERR_FC_DBFAILURE)
    MSG_CASE(ERR_FC_NO_PRIVILEGE)
    MSG_CASE(ERR_FC_INVALIDNAME)
    MSG_CASE(ERR_FC_FULL)
    MSG_CASE(ERR_FC_APPLY_REJOIN)
    MSG_CASE(ERR_FC_JOIN_SUCCESS)
    MSG_CASE(ERR_FC_ACCEPT_REACCEPT)
    MSG_CASE(ERR_FC_FACTION_NOTEXIST)
    MSG_CASE(ERR_FC_NOTAMEMBER)
    MSG_CASE(ERR_FC_CHECKCONDITION)
    MSG_CASE(ERR_FC_DATAERROR)
	MSG_CASE(ERR_FC_OFFLINE)
	MSG_CASE(ERR_FC_INVALID_IN_PVP)
	}
}

void CECFactionMan::ClearHostFaction()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	pHost->SetFactionID(0);
	pHost->SetFRoleID(_R_UNMEMBER);
	Release(true);
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->UpdateFactionList();
	OutputDebugStringA("faction: leave\n");
}

DWORD CECFactionMan::OnChat(const FactionChat* p)
{
	const ACHAR* szName = GetMemName(p->src_roleid);

	if (szName)
	{
		ACString strMsg;
		_cp_str(strMsg, p->msg.begin(), p->msg.size());
		ACString strMsgOrigion = strMsg;// 原始发言信息

		CECIvtrItem* pItem = CHAT_S2C::CreateChatItem(p->data);
		CECGameUIMan* pGameUI = (CECGameUIMan*)g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		strMsg = pGameUI->FilterInvalidTags(strMsg, pItem==NULL);

		ACHAR szText[80], szText1[1024];
		AUI_ConvertChatString(szName, szText);
		AUI_ConvertChatString(strMsg, szText1, false);

		ACString str;
		str.Format(g_pGame->GetFixedMsgTab()->GetWideString(FIXMSG_CHAT), szText, szText1);
		g_pGame->GetGameRun()->AddChatMessage(str, GP_CHAT_FACTION, p->src_roleid, szName, 0, p->emotion, pItem,strMsgOrigion);
		return 0;
	}

	return FO_OPERATOR_UNK;
}

void CECFactionMan::OnOpRequest(const FactionOPRequest_Re* p)
{
	if (p->retcode != ERR_SUCCESS)
		AddFixedMsg(p->retcode);
}

void CECFactionMan::OnFactionCreate(const FactionCreate_Re* p)
{
	if (p->retcode == ERR_SUCCESS)
	{
		const Octets& o = p->faction_name;
		g_pGame->GetFactionMan()->AddFaction(p->faction_id, o.begin(), o.size(), 0, 1);
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		pHost->SetFactionID(p->faction_id);
		pHost->SetFRoleID(_R_MASTER);
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_CREATE);
		RefreshMemList();
	}
	else
		AddFixedMsg(p->retcode);
}

Faction_Info* CECFactionMan::AddFaction(unsigned int uId, const void* name, int len, int nLev, int nMemNum)
{
	FactionInfoMap::iterator it = m_FactionMap.find(uId);
	Faction_Info* p = (it == m_FactionMap.end() ? (new Faction_Info) : it->second);
	p->SetID(uId);
	p->SetName(name, len);
	p->SetLevel(nLev);
	p->SetMemNum(nMemNum);
	m_FactionMap[uId] = p;
	
	CECFactionManFactionChange change(CECFactionManChange::ADD_FACTION, uId, p);
	NotifyObservers(&change);
	return p;
}

Faction_Mem_Info* CECFactionMan::AddNewMember(int nRoleId, const ACHAR* szName, int lev, int prof, unsigned char gender, int repuration, int reincarnation_times)
{
	Faction_Mem_Info* pNewMem;
	FactionMemMap::iterator it = m_MemMap.find(nRoleId);

	if (it == m_MemMap.end())
	{
		pNewMem = new Faction_Mem_Info;
		pNewMem->SetID(nRoleId);
		pNewMem->SetFRoleID(_R_MEMBER);
		pNewMem->SetOnline(1);
		m_MemList.push_back(pNewMem);
		m_MemMap[nRoleId] = pNewMem;
	}
	else
		pNewMem = it->second;

	pNewMem->SetName(szName);
	pNewMem->SetLev(lev);
	pNewMem->SetProf(prof);
	pNewMem->SetGender(gender);
	pNewMem->SetContrib(0);
	pNewMem->SetDelayExpel(false, 0);
	pNewMem->SetLoginDay(g_pGame->GetServerGMTTime());
	pNewMem->SetReputation(repuration);
	pNewMem->SetReincarnationTimes(reincarnation_times);

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->UpdateFactionList();

	return pNewMem;
}

DWORD CECFactionMan::OnAcceptJoin(const FactionAcceptJoin_Re* p)
{
	// 某人加入帮派

	if (p->retcode == ERR_SUCCESS)
	{
		if (p->roleid == p->newmember)
		{
			CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
			pHost->SetFactionID(p->factionid);
			pHost->SetFRoleID(_R_MEMBER);
			const ACHAR* szName = GetMemName(p->operater);
			if (szName) g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_ACCEPT_JOIN2, szName);
			return 0;
		}

		DWORD dwRet = 0;

		const ACHAR* szName = GetMemName(p->operater);
		if (!szName) dwRet |= FO_OPERATOR_UNK;

		ACString strMemName;
		_cp_str(strMemName, p->name.begin(), p->name.size());
		AddNewMember(p->newmember, strMemName, p->level, p->cls, p->gender, p->reputation, p->reincarn_times);

		if (dwRet) return dwRet;

		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_ACCEPT_JOIN, szName, strMemName);
	}
	else if (p->retcode == ERR_FC_JOIN_REFUSE)
	{
		const ACHAR* szName = GetMemName(p->newmember);
		if (szName) g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ERR_FC_JOIN_REFUSE, szName);
	}
	else
		AddFixedMsg(p->retcode);

	return 0;
}

void CECFactionMan::RemoveMember(int iRoleId)
{
	for (size_t i = 0; i < m_MemList.size(); i++)
	{
		if (m_MemList[i]->GetID() == iRoleId)
		{
			m_MemList.erase(&m_MemList[i]);
			break;
		}
	}

	FactionMemMap::iterator it = m_MemMap.find(iRoleId);
	if (it != m_MemMap.end()) m_MemMap.erase(it);

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->UpdateFactionList();
}

DWORD CECFactionMan::OnExpel(const FactionExpel_Re* p)
{
	if (p->retcode == ERR_SUCCESS)
	{
		if (p->roleid == static_cast<int>(p->expelroleid))
		{
			ClearHostFaction();
			const ACHAR* szName = GetMemName(p->operater);
			if (szName == NULL) return FO_OPERATOR_UNK;
			CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_EXPEL, szName, pHost->GetName());
			return 0;
		}
		else
		{
			DWORD dwRet = 0;

			const ACHAR* szOperator = GetMemName(p->operater);
			if (!szOperator) dwRet |= FO_OPERATOR_UNK;

			const ACHAR* szOperant = GetMemName(p->expelroleid);
			if (!szOperant) dwRet |= FO_OPERANT_UNK;

			RemoveMember(p->expelroleid);
			if (dwRet) return dwRet;

			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_EXPEL, szOperator, szOperant);
		}
	}
	else
		AddFixedMsg(p->retcode);

	return 0;
}

DWORD CECFactionMan::OnDelayExpel(const FactionDelayExpelAnnounce* p)
{
	if (p->retcode == ERR_SUCCESS)
	{
		//	修改数据
		Faction_Mem_Info *pMem = GetMember(p->expelroleid);
		if (!pMem){
			ASSERT(false);
			return 0;
		}
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		switch (p->opt_type)
		{
		case 1:	//	延时删除
			pMem->SetDelayExpel(true, ConvertFromSeconds(p->time)+GetCurrentTime());
			pGameUI->UpdateFactionList();
			break;
		case 2:	//	召回
			pMem->SetDelayExpel(false, 0);
			pGameUI->UpdateFactionList();
			break;
		case 3:	//	延时删除时间到期
			{
				int idHost = g_pGame->GetGameRun()->GetHostPlayer()->GetCharacterID();
				if (idHost == p->expelroleid)
					ClearHostFaction();
				else
					RemoveMember(p->expelroleid);
			}
			break;
		default:
			ASSERT(false);
			return 0;
		}

		//	显示消息
		return OnDelayExpelMessage(p);
	}
	else
		AddFixedMsg(p->retcode);
	
	return 0;
}

DWORD CECFactionMan::OnDelayExpelMessage(const FactionDelayExpelAnnounce* p)
{
	if (p->retcode != ERR_SUCCESS) return 0;
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();
	int idHost = pHost->GetCharacterID();
	switch (p->opt_type)
	{
	case 1:	//	延时删除
		if (idHost == p->operater){
			const ACHAR* szOperant = GetMemName(p->expelroleid);
			if (!szOperant) return FO_OPERANT_UNK;
			ACString strMsg;
			strMsg.Format(pStrTab->GetWideString(FIXMSG_FC_DELAYEXPEL), szOperant, pGameUI->GetFormatTime(p->time));
			pGameUI->MessageBox("", strMsg, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}else{
			DWORD dwRet = 0;
			const ACHAR* szOperator = GetMemName(p->operater);
			if (!szOperator) dwRet |= FO_OPERATOR_UNK;
			const ACHAR* szOperant = GetMemName(p->expelroleid);
			if (!szOperant) dwRet |= FO_OPERANT_UNK;
			if (dwRet) return dwRet;
			if (idHost == p->expelroleid){
				ACString strMsg;
				strMsg.Format(pStrTab->GetWideString(FIXMSG_FC_DELAYEXPEL3), szOperant, pGameUI->GetFormatTime(p->time));
				pGameUI->MessageBox("", strMsg, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			}
			else{
				ACString strMsg;
				strMsg.Format(pStrTab->GetWideString(FIXMSG_FC_DELAYEXPEL2), szOperator, szOperant, szOperant, pGameUI->GetFormatTime(p->time));
				pGameUI->AddChatMessage(strMsg, GP_CHAT_BROADCAST);
			}
		}
		break;
	case 2:	//	召回
		{
			DWORD dwRet = 0;			
			const ACHAR* szOperator = GetMemName(p->operater);
			if (!szOperator) dwRet |= FO_OPERATOR_UNK;			
			const ACHAR* szOperant = GetMemName(p->expelroleid);
			if (!szOperant) dwRet |= FO_OPERANT_UNK;
			if (dwRet) return dwRet;
			if (idHost == p->expelroleid){
				ACString strMsg;
				strMsg.Format(pStrTab->GetWideString(FIXMSG_FC_CANCELEXPEL2), szOperator);
				pGameUI->MessageBox("", strMsg, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			}else if (idHost != p->operater){
				ACString strMsg;
				strMsg.Format(pStrTab->GetWideString(FIXMSG_FC_CANCELEXPEL), szOperator, szOperant);
				pGameUI->AddChatMessage(strMsg, GP_CHAT_BROADCAST);
			}
		}
		break;
	case 3:	//	延时删除时间到期，仅 p->expelroleid 成员有效
		{
			const ACHAR* szOperant = GetMemName(p->expelroleid);
			if (!szOperant) return FO_OPERANT_UNK;
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_EXPEL2, szOperant);
		}
		break;
	default:
		ASSERT(false);
	}
	return 0;
}

void CECFactionMan::OnBroadcastNotice(const FactionBroadcastNotice_Re* p)
{
}

DWORD CECFactionMan::OnMasterResign(const FactionMasterResign_Re* p)
{
	if (p->retcode == ERR_SUCCESS)
	{
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		if (pHost->GetFRoleID() == _R_MASTER) pHost->SetFRoleID(_R_MEMBER);
		if (p->roleid == p->newmaster) pHost->SetFRoleID(_R_MASTER);

		for (size_t i = 0; i < m_MemList.size(); i++)
		{
			if (m_MemList[i]->GetFRoleID() == _R_MASTER)
			{
				m_MemList[i]->SetFRoleID(_R_MEMBER);
				break;
			}
		}

		Faction_Mem_Info* pNewMaster = GetMember(p->newmaster);
		if (pNewMaster) pNewMaster->SetFRoleID(_R_MASTER);

		const ACHAR* szName = GetMemName(p->newmaster);
		if (!szName) return FO_OPERANT_UNK;

		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_APPOINT, _AL(""), szName, GetFRoleName(_R_MASTER));

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->UpdateFactionList();
	}
	else
		AddFixedMsg(p->retcode);

	return 0;
}

const ACHAR* CECFactionMan::GetMemName(int iRoleId)
{
	return g_pGame->GetGameRun()->GetPlayerName(iRoleId, false);
}

const ACHAR* CECFactionMan::GetMemNickName(int iRoleId)
{
	Faction_Mem_Info* pMem = GetMember(iRoleId);
	if (pMem) return pMem->GetNickName();
	return NULL;
}

DWORD CECFactionMan::OnResign(const FactionResign_Re* p)
{
	if (p->retcode == ERR_SUCCESS)
	{
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

		if (p->resigned_role == p->roleid)
			pHost->SetFRoleID(_R_MEMBER);

		Faction_Mem_Info* pMem = GetMember(p->resigned_role);
		if (pMem) pMem->SetFRoleID(_R_MEMBER);

		const ACHAR* szName = GetMemName(p->resigned_role);
		if (!szName) return FO_OPERANT_UNK;

		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_RESIGN, szName, GetFRoleName(p->old_occup));

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->UpdateFactionList();
	}
	else
		AddFixedMsg(p->retcode);

	return 0;
}

DWORD CECFactionMan::OnAppoint(const FactionAppoint_Re* p)
{
	if (p->retcode == ERR_SUCCESS)
	{
		Faction_Mem_Info* pMem = GetMember(p->dstroleid);
		if (pMem) pMem->SetFRoleID(p->newoccup);

		if (p->roleid == p->dstroleid)
		{
			CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
			pHost->SetFRoleID(p->newoccup);

			const ACHAR* szOperator = GetMemName(p->operater);
			if (!szOperator) return FO_OPERATOR_UNK;

			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_APPOINT, szOperator, pHost->GetName(), GetFRoleName(p->newoccup));
		}
		else
		{
			DWORD dwRet = 0;

			const ACHAR* szOperator = GetMemName(p->operater);
			if (!szOperator) dwRet |= FO_OPERATOR_UNK;

			const ACHAR* szOperant = GetMemName(p->dstroleid);
			if (!szOperant) dwRet |= FO_OPERANT_UNK;

			if (dwRet) return dwRet;

			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_APPOINT, szOperator, szOperant, GetFRoleName(p->newoccup));
		}

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->UpdateFactionList();
	}
	else
		AddFixedMsg(p->retcode);

	return 0;
}

DWORD CECFactionMan::OnLeave(const FactionLeave_Re* p)
{
	if (p->retcode == ERR_SUCCESS)
	{
		if (p->roleid == p->leaved_role)
		{
			ClearHostFaction();
			CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_LEAVE, pHost->GetName());
		}
		else
		{
			RemoveMember(p->leaved_role);
			const ACHAR* szName = GetMemName(p->leaved_role);
			if (!szName) return FO_OPERATOR_UNK;
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_LEAVE, szName);
		}
	}
	else
		AddFixedMsg(p->retcode);

	return 0;
}

void CECFactionMan::OnUpgrade(const FactionUpgrade_Re* p)
{
	if (p->retcode == ERR_SUCCESS)
	{
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		Faction_Info* pFaction = GetFaction(pHost->GetFactionID());
		if (pFaction) pFaction->SetLevel(pFaction->GetLevel()+1);
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_UPGRADE);
	}
}

void CECFactionMan::OnDegrade(const FactionDegrade_Re* p)
{
	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_DEGRADE);
}

void CECFactionMan::OnApplyJoin(const FactionApplyJoin_Re* p)
{
}

void CECFactionMan::RefreshMemList(int version/* =0 */)
{
	//	version=0，更新所有成员信息
	//	version=1，只更新成员在线信息
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost->GetFactionID() == 0) return;
	g_pGame->GetGameSession()->faction_listmember(version);
	m_bMemInfoReady = false;
}

void CECFactionMan::RefreshOlStatus(const IntVector* pList)
{
	size_t i(0);
	for (i = 0; i < m_MemList.size(); i++)
		m_MemList[i]->SetOnline(0);

	for (i = 0; i < const_cast<IntVector*>(pList)->size(); i++)
	{
		Faction_Mem_Info* pMem = GetMember((*const_cast<IntVector*>(pList))[i]);
		if (pMem) pMem->SetOnline(1);
	}
}

void Faction_Mem_Info::SetAll(const GNET::FMemberInfo &info, DWORD currentTime)
{
	SetID(info.roleid);
	SetLev(info.level);
	SetProf(info.occupation);
	SetFRoleID(info.froleid);
	SetGender(info.gender);
	SetLoginDay(24*3600*(int)info.loginday);
	SetName(info.name.begin(), info.name.size());
	SetNickName(info.nickname.begin(), info.nickname.size());
	SetContrib(info.contrib);
	SetDelayExpel(info.delayexpel, info.delayexpel ? CECFactionMan::ConvertFromSeconds(info.expeltime)+currentTime : 0);
	SetReputation(info.reputation);
	SetReincarnationTimes(info.reincarn_times);
}

void CECFactionMan::OnListMember(const FactionListMember_Re* p)
{
	using namespace GNET;

	if (p->handle == -1)
	{
		m_bMemInfoReady = false;
		return;
	}

	switch (p->handle)
	{
	case 0:
		{
			ResetMemInfoList();
			_cp_str(m_strProclaim, p->proclaim.begin(), p->proclaim.size());
			
			DWORD currentTime = GetCurrentTime();
			for (size_t i = 0; i < const_cast<FactionListMember_Re*>(p)->member_list.size(); i++)
			{
				const FMemberInfo& info = const_cast<FactionListMember_Re*>(p)->member_list[i];
				Faction_Mem_Info* pMem = new Faction_Mem_Info;
				pMem->SetAll(info, currentTime);
				m_MemList.push_back(pMem);
				m_MemMap[pMem->GetID()] = pMem;
			}
			
			RefreshOlStatus(&p->online_list);
			m_bMemInfoReady = true;
		}
		break;
	case 1:
		RefreshOlStatus(&p->online_list);
		break;
	case 2:
		{
			DWORD currentTime = GetCurrentTime();
			for (size_t i = 0; i < const_cast<FactionListMember_Re*>(p)->member_list.size(); i++)
			{
				const FMemberInfo& info = const_cast<FactionListMember_Re*>(p)->member_list[i];
				Faction_Mem_Info* pMem = GetMember(info.roleid);
				if (pMem){
					pMem->SetAll(info, currentTime);
					pMem->SetOnline(1);
				}
			}
		}
		break;
	default:
		ASSERT(false);
		return;
	}
	
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->UpdateFactionList();
}

void CECFactionMan::OnBaseInfo(const GetFactionBaseInfo_Re* p)
{
	const Octets& o = p->faction_info.name;

	Faction_Info* pInfo = g_pGame->GetFactionMan()->AddFaction(
		p->faction_info.fid,
		o.begin(),
		o.size(),
		p->faction_info.level,
		p->faction_info.mem_num
	);

	ALISTPOSITION pos = m_PendingInviteList.GetHeadPosition();
	while (pos)
	{
		ALISTPOSITION posCur = pos;
		FactionInviteJoin* pInvite = m_PendingInviteList.GetNext(pos);
		FactionInviteArg* pArg = static_cast<FactionInviteArg*>(pInvite->GetArgument());

		if (pArg->factionid == pInfo->GetID())
		{
			m_PendingInviteList.RemoveAt(posCur);
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			pGameUI->PopupFactionInviteMenu(pArg->roleid, pInfo->GetName(), reinterpret_cast<DWORD>(pInvite));
		}
	}

	g_pGame->GetGameSession()->RefreshPendingFactionMessage();

	if(p->faction_info.fid == g_pGame->GetGameRun()->GetHostPlayer()->GetFactionID())
		RefreshMemList();
}

void CECFactionMan::OnDismiss(const FactionDismiss_Re* p)
{
	if (p->retcode == ERR_SUCCESS)
	{
		ClearHostFaction();
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_DISMISS);
	}
	else
		AddFixedMsg(p->retcode);
}

void CECFactionMan::OnInviteJoin(const FactionInviteJoin* p)
{
	using namespace GNET;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	Faction_Info* pFaction;
	FactionInviteArg* pArg = static_cast<FactionInviteArg*>(p->GetArgument());

	ACString strMemName;
	_cp_str(strMemName, pArg->role_name.begin(), pArg->role_name.size());
	if( strMemName != _AL("") )
		g_pGame->GetGameRun()->AddPlayerName(pArg->roleid, strMemName);

	if (pArg->faction_name.size())
	{
		ACString str;
		_cp_str(str, pArg->faction_name.begin(), pArg->faction_name.size());
		pGameUI->PopupFactionInviteMenu(pArg->roleid, str, reinterpret_cast<DWORD>(p));
	}
	else if (pFaction = GetFaction(pArg->factionid, true))
		pGameUI->PopupFactionInviteMenu(pArg->roleid, pFaction->GetName(), reinterpret_cast<DWORD>(p));
	else
		m_PendingInviteList.AddTail(const_cast<FactionInviteJoin*>(p));
}

void CECFactionMan::OnPlayerInfo(const GetPlayerFactionInfo_Re* p)
{
	using namespace GNET;

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	const GUserFaction& i = p->faction_info;

	pHost->SetNickname(i.nickname.begin(), i.nickname.size());

	Faction_Mem_Info* pMem = GetMember(p->roleid);

	if (pMem)
	{
		pMem->SetNickName(i.nickname.begin(), i.nickname.size());
	}

	if (i.delayexpel.size()){
		try
		{
			Marshal::OctetsStream oval(i.delayexpel);
			char delayexpelflag = 0;
			int expeltime = 0;
			oval >> delayexpelflag;
			if (delayexpelflag)
				oval >> expeltime;
			if (pMem){
				pMem->SetDelayExpel(delayexpelflag, expeltime);
			}
			if (delayexpelflag){
				CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				ACString strMsg;
				strMsg.Format(pGameUI->GetStringFromTable(10383), pGameUI->GetFormatTime(expeltime));
				pGameUI->MessageBox("", strMsg, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			}
		}
		catch (Marshal::Exception &e)
		{
			ASSERT(false);
			a_LogOutput(1, "CECFactionMan::OnPlayerInfo, Marshal::Exception");
		}
	}
}

DWORD CECFactionMan::OnRename(const FactionRename_Re* p)
{
	if (p->retcode == ERR_SUCCESS)
	{
		Faction_Mem_Info* pMem = GetMember(p->renamed_roleid);
		if (pMem) pMem->SetNickName(p->new_name.begin(), p->new_name.size());

		if (p->roleid == p->renamed_roleid)
		{
			CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
			pHost->SetNickname(p->new_name.begin(), p->new_name.size());
			const ACHAR* szOperator = GetMemName(p->operater);
			if (!szOperator) return FO_OPERATOR_UNK;
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_RENAME, szOperator, pHost->GetName(), pHost->GetNickname());
		}
		else
		{
			DWORD dwRet = 0;

			const ACHAR* szOperator = GetMemName(p->operater);
			if (!szOperator) dwRet |= FO_OPERATOR_UNK;

			const ACHAR* szOperant = GetMemName(p->renamed_roleid);
			if (!szOperant) dwRet |= FO_OPERANT_UNK;

			if (dwRet) return dwRet;

			ACString strNickName;
			_cp_str(strNickName, p->new_name.begin(), p->new_name.size());
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_RENAME, szOperator, szOperant, strNickName);
		}

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->UpdateFactionList();
	}
	else
		AddFixedMsg(p->retcode);

	return 0;
}

void CECFactionMan::OnChangeProclaim(const FactionChangProclaim_Re* p)
{
	if (p->retcode == ERR_SUCCESS)
	{
		_cp_str(m_strProclaim, p->proclaim.begin(), p->proclaim.size());
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_CHG_PROCLAIM);
	}
	else
		AddFixedMsg(p->retcode);
}

Faction_Info * CECFactionMan::FindFactionByName(const ACHAR *szName)
{
	Faction_Info *pInfo = NULL;
	if (szName && szName[0])
	{
		for (FactionInfoMap::iterator it = m_FactionMap.begin(); it != m_FactionMap.end(); ++ it)
		{
			Faction_Info *pCur = it->second;
			if (pCur && !a_strcmp(pCur->GetName(), szName))
			{
				pInfo = pCur;
				break;
			}
		}
	}
	return pInfo;
}

void CECFactionMan::SetHostFactionRelation(const abase::vector<int> &alliance, const abase::vector<int> &hostile)
{
	m_alliance = alliance;
	m_hostile = hostile;
}

bool CECFactionMan::IsFactionAlliance(int fid)
{
	//	判断给定帮派是否为同盟帮
	bool bRet(false);

	if (fid)
	{
		for (FactionRelationArray::iterator it = m_alliance.begin(); it != m_alliance.end(); ++ it)
		{
			if (*it == fid)
			{
				bRet = true;
				break;
			}
		}
	}

	return bRet;
}

bool CECFactionMan::IsFactionHostile(int fid)
{
	//	判断给定帮派是否为敌对帮
	bool bRet(false);

	if (fid)
	{
		for (FactionRelationArray::iterator it = m_hostile.begin(); it != m_hostile.end(); ++ it)
		{
			if (*it == fid)
			{
				bRet = true;
				break;
			}
		}
	}

	return bRet;
}

void CECFactionMan::OnFactionRename(const FactionRenameAnnounce* p){
	if (p->retcode == ERR_SUCCESS){
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_RENAME_FACTION);
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		int factionID = pHost->GetFactionID();
		if (factionID > 0){
			if (Faction_Info *pFactionInfo = GetFaction(factionID, false)){
				pFactionInfo->SetName(p->newname.begin(), p->newname.size());				
				CECFactionManFactionChange change(CECFactionManChange::FACTION_RENAMEFACTION, factionID, pFactionInfo);
				NotifyObservers(&change);
			}
		}
	}else{
		AddFixedMsg(p->retcode);
	}
}
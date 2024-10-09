/*
 * FILE: EC_GameSession.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AString.h"
#include "AAssist.h"
#include "AArray.h"
#include "EC_C2SCmdCache.h"
#include "EC_StringTab.h"
#include "EC_SendC2SCmds.h"
#include "gnetdef.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DVECTOR3;
class AFile;

namespace GNET
{
	class RoleInfo;
	class Protocol;
	class GameClient;
	class Octets;
}

namespace C2S
{
	struct npc_trade_item;
	struct npc_sell_item;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

typedef void (* PONRECEIVEHELPSTATES) (const void * pData, int iSize);

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECGameSession
//	
///////////////////////////////////////////////////////////////////////////

class CECStubbornNetSender;
class CECStubbornFactionInfoSender;
class CECGameSession
{
public:		//	Types

	//	Overtime check ID
	enum
	{
		OT_CHALLENGE = 0,
		OT_ENTERGAME,
	};

	enum enumLinkBrokenReason{
		LBR_NONE,					//	未断开链接
		LBR_CHALLENGE_OVERTIME,		//	Challenge 超时
		LBR_ENTERWORLD_OVERTIME,	//	未回应 EnterWorld 协议超时
		LBR_ANTI_WALLOW,			//	防沉迷（游戏时间过长）
		LBR_MAP_INVALID,			//	找不到地图数据
		LBR_ROLE_RENAME,			//	角色改名
		LBR_PASSIVE_BEFOREONLINE,	//	成功链接后、角色上线前被动断开链接（网络原因）
		LBR_PASSIVE_ONLINE,			//	角色上线后被动断开链接（网络原因、协议原因等）
		LBR_DEBUG,					//	调试
		LBR_COUNT,
	};

	//	Overtime check
	struct OVERTIME
	{
		int		iCheckID;
		bool	bCheck;
		DWORD	dwTime;
		DWORD	dwTimeCnt;
	};		

	//	Pingtime
	struct PINGTIME
	{
		DWORD	idLink;
		DWORD	dwTime;
		DWORD	dwStatus;
		DWORD	dwLastPingTime;
		DWORD	dwCreateTime;
		unsigned char	cExpRate;	// 10-100, the real exp rate is cExpRate / 10
	};		

	//	Server attributes
	union SEVATTR
	{
		unsigned int _attr;

		struct 
		{
			unsigned char load;
			unsigned char lambda;
			unsigned char anything;
			unsigned char doubleExp:1;
            unsigned char doubleMoney:1;
            unsigned char doubleObject:1;
	        unsigned char doubleSP:1;
			unsigned char freeZone:1;
			unsigned char sellPoint:1;
			unsigned char battle:1;
		};
	};

	static int IOCallBack(void* pData, unsigned int idLink, int iEvent);
	static DWORD WINAPI ConnectThread(LPVOID pArg);

public:		//	Constructor and Destructor

	CECGameSession();
	virtual ~CECGameSession();

public:		//	Attributes

public:		//	Operations

	//	Open session
	bool Open();
	//	Close sesssion
	void Close();

	//	Update
	bool Update(DWORD dwDeltaTime);
	//	Net pulse
	void NetPulse();

	//	Set link ID
	void SetLinkID(DWORD idLink) { m_idLink = idLink; }
	//	Get link ID
	DWORD GetLinkID() { return m_idLink; }

	//	Add a protocol to m_aNewProtocols and prepare to be processed
	int AddNewProtocol(GNET::Protocol* pProtocol);
	//	Process protocols in m_aNewProtocols
	bool ProcessNewProtocols();
	//	Clear all processed protocols in m_aOldProtocols
	void ClearOldProtocols();
	//	Send game data
	bool SendGameData(void* pData, int iSize);
	//	Send chat data
	bool SendChatData(char cChannel, const ACHAR* szMsg, int iPack=-1, int iSlot=-1);
	//	Send private chat data
	bool SendPrivateChatData(const ACHAR* szDstName, const ACHAR* szMsg, BYTE byFlag=0, int idPlayer=0, int iPack=-1, int iSlot=-1);
	// Send protocl
	bool SendNetData(const GNET::Protocol& p, bool bUrg = false);

	//	Create a role
	bool CreateRole(const GNET::RoleInfo& Info, const GNET::Octets& Data);
	//	Select role
	bool SelectRole(int cid, char flag=0);
	//	Delete role
	bool DeleteRole(int cid);
	//	Restore role
	bool RestoreRole(int cid);
	//	Start game
	bool StartGame();
	//	Re-login
	bool ReLogin(bool bSelRole);
	bool QuitLoginQueue();

	//	Get player's bese info (including custom data)
	void GetRoleBaseInfo(int iNumRole, const int* aRoleIDs);
	//	Get player's customized data
	void GetRoleCustomizeData(int iNumRole, const int* aRoleIDs);
	//	Get player's faction info
	void GetFactionInfo(int iNumFaction, const int* aFactinoIDs);
	void GetFactionInfoImmediately(int iNumFaction, const int* aFactinoIDs);
	//	Set custom data
	void SetRoleCustomizeData(int idRole, void* pData, int iDataSize);
	//	Save necessary user configs (UI, shortcut, accelerate keys) to server
	void SaveConfigData(const void* pDataBuf, int iSize);
	//	Load necessary user configs (UI, shortcut, accelerate keys) from server
	void LoadConfigData();
	//	Save help states data to server
	void SaveHelpStates(const void* pDataBuf, int iSize);
	//	Load help states data from server, and when help states received, we will call callback function to
	//	let the caller get the data
	void LoadHelpStates(PONRECEIVEHELPSTATES pfnCallBack);
	//	Get player's brief information
	void GetPlayerBriefInfo(int iNumPlayer, int* aIDs, int iReason);
	//	Get player's information with command cache
	void CacheGetPlayerBriefInfo(int iNumPlayer, int* aIDs, int iReason){ m_CmdCache.SendGetPlayerBriefInfo(iNumPlayer, aIDs, iReason); }
	// 商城赠送
	void CacheSendGivePresentProtocol(int roleid, int mailid, int itemid, int index, int slot) { m_CmdCache.SendGivePresentProtocol(roleid,mailid,itemid,index,slot); }
	//  Cached the protocol 'PlayerBaseInfo'
	void CacheGetRoleBaseInfo(int iNumRoles, int* aRoleIDs) { m_CmdCache.SendGetPlayerBaseInfo(iNumRoles, aRoleIDs); }
	//	Get player's ID by name
	void GetPlayerIDByName(const ACHAR* szName, int iReason);
	//	Submit problems to GM
	void SubmitProblemToGM(const ACHAR* szContent, int idPluginUser, const ACHAR *szPluginUserName);
	//	Submit ChatSpeak to GM
	void SubmitChatSpeakToGM(const ACHAR* szContent, int idPluginUser, const ACHAR *szPluginUserName);
	//	Impeach else player to GM
	void ImpeachPlayerToGM(const ACHAR* szName, const ACHAR* szContent);
	void ReportPluginToSystem(int idPluginUser, const ACHAR* szContent);
	//	Get ping time
	DWORD GetPingTime(int idServer, DWORD& dwStatus, DWORD& dwCreateTime, unsigned char& cExpRate);
	//  Refresh faction info related messages
	void RefreshPendingFactionMessage();
	
	//	General c2s commands ...
	void c2s_CmdPlayerMove(const A3DVECTOR3& vCurPos, const A3DVECTOR3& vDest, int iTime, float fSpeed, int iMoveMode, WORD wStamp) { ::c2s_SendCmdPlayerMove(vCurPos, vDest, iTime, fSpeed, iMoveMode, wStamp); }
	void c2s_CmdStopMove(const A3DVECTOR3& vDest, float fSpeed, int iMoveMode, BYTE byDir, WORD wStamp, int iTime) { ::c2s_SendCmdStopMove(vDest, fSpeed, iMoveMode, byDir, wStamp, iTime); }
	void c2s_CmdSelectTarget(int id) { m_CmdCache.SendCmdSelectTarget(id); }
	void c2s_CmdNormalAttack(BYTE byPVPMask) { ::c2s_SendCmdNormalAttack(byPVPMask); }
	void c2s_CmdReviveVillage(int param=0) { m_CmdCache.SendCmdReviveVillage(param); }
	void c2s_CmdReviveItem(int param=0) { m_CmdCache.SendCmdReviveItem(param); }
	void c2s_CmdGoto(float x, float y, float z) { ::c2s_SendCmdGoto(x, y, z); }
	void c2s_CmdGetExtProps() { m_CmdCache.SendCmdExtProps(); }
	void c2s_CmdGetPartExtProps(int iPropIndex) { ::c2s_SendCmdGetPartExtProps(iPropIndex); }
	void c2s_CmdSetStatusPts(int v, int e, int s, int a) { ::c2s_SendCmdSetStatusPts(v, e, s, a); }
	void c2s_CmdLogout(int iOutType);
	void c2s_CmdGetAllData(bool bpd, bool bed, bool btd) { ::c2s_SendCmdGetAllData(bpd, bed, btd); }
	void c2s_CmdChargeEquipFlySword(int iEleIdx, int iCount);
	void c2s_CmdChargeFlySword(int iEleIdx, int iFSIdx, int iCount, int idFlySword);
	void c2s_CmdCancelAction() { m_CmdCache.SendCmdCancelAction(); }
	void c2s_CmdSitDown() { ::c2s_SendCmdSitDown(); }
	void c2s_CmdStandUp() { ::c2s_SendCmdStandUp(); }
	void c2s_CmdEmoteAction(WORD wPose) { ::c2s_SendCmdEmoteAction(wPose); }
	void c2s_CmdTaskNotify(const void* pData, DWORD dwDataSize) { ::c2s_SendCmdTaskNotify(pData, dwDataSize); }
	void c2s_CmdUnselect() { ::c2s_SendCmdUnselect(); }
	void c2s_CmdContinueAction() { ::c2s_SendCmdContinueAction(); }
	void c2s_CmdStopFall() { ::c2s_SendCmdStopFall(); }
	void c2s_CmdGatherMaterial(int idMatter, int iToolPack, int iToolIdx, int idTool, int idTask) { ::c2s_SendCmdGatherMaterial(idMatter, iToolPack, iToolIdx, idTool, idTask); }
	void c2s_CmdCastSkill(int idSkill, BYTE byPVPMask, int iNumTarget, int* aTargets) { m_CmdCache.SendCmdCastSkill(idSkill, byPVPMask, iNumTarget, aTargets); }
	void c2s_CmdCastInstantSkill(int idSkill, BYTE byPVPMask, int iNumTarget, int* aTargets) { m_CmdCache.SendCmdCastInstantSkill(idSkill, byPVPMask, iNumTarget, aTargets); }
	void c2s_CmdCastPosSkill(int idSkill, const A3DVECTOR3& vPos, BYTE byPVPMask, int iNumTarget, int* aTargets) { ::c2s_SendCmdCastPosSkill(idSkill, vPos, byPVPMask, iNumTarget, aTargets); }
	void c2s_CmdTrickAction(int iAction) { ::c2s_SendCmdTrickAction(iAction); }
	void c2s_CmdSetAdvData(int iData1, int iData2) { ::c2s_SendCmdSetAdvData(iData1, iData2); }
	void c2s_CmdClearAdvData() { ::c2s_SendCmdClearAdvData(); }
	void c2s_CmdQueryPlayerInfo1(int iNum, int* aIDs) { ::c2s_SendCmdQueryPlayerInfo1(iNum, aIDs); }
	void c2s_CmdQueryNPCInfo1(int iNum, int* aIDs) { ::c2s_SendCmdQueryNPCInfo1(iNum, aIDs); }
	void c2s_CmdSessionEmote(int iAction) { ::c2s_SendCmdSessionEmote(iAction); }
	void c2s_CmdConEmoteRequest(int iAction, int idTarget) { ::c2s_SendCmdConEmoteRequest(iAction, idTarget); }
	void c2s_CmdConEmoteReply(int iResult, int iAction, int idTarget) { ::c2s_SendCmdConEmoteReply(iResult, iAction, idTarget); }
	void c2s_CmdDeadMove(float y, int iTime, float fSpeed, int iMoveMode) { ::c2s_SendCmdDeadMove(y, iTime, fSpeed, iMoveMode); }
	void c2s_CmdDeadStopMove(float y, float fSpeed, BYTE byDir, int iMoveMode) { ::c2s_SendCmdDeadStopMove(y, fSpeed, byDir, iMoveMode); }
	void c2s_CmdEnterSanctuary(int id) { m_CmdCache.SendCmdEnterSanctuary(id); }
	void c2s_CmdOpenBooth(int iNumEntry, const void* szName, void* aEntries) { ::c2s_SendCmdOpenBooth(iNumEntry, szName, aEntries); }
	void c2s_CmdCloseBooth() { ::c2s_SendCmdCloseBooth(); }
	void c2s_CmdQueryBoothName(int iNum, int* aIDs) { ::c2s_SendCmdQueryBoothName(iNum, aIDs); }
	void c2s_CmdCompleteTravel() { ::c2s_SendCmdCompleteTravel(); }
	void c2s_CmdEnalbePVP() { ::c2s_SendCmdEnalbePVP(); }
	void c2s_CmdDisablePVP() { ::c2s_SendCmdDisablePVP(); }
	void c2s_CmdOpenBoothTest() { ::c2s_SendCmdOpenBoothTest(); }
	void c2s_CmdFashionSwitch() { ::c2s_SendCmdFashionSwitch(); }
	void c2s_CmdEnterInstance(int iTransIdx, int idInst) { m_CmdCache.SendCmdEnterInstance(iTransIdx, idInst); }
	void c2s_CmdRevivalAgree(int param=0) { ::c2s_SendCmdRevivalAgree(param); }
	void c2s_CmdActiveRushFly(bool bActive) { m_CmdCache.SendCmdActiveRushFly(bActive); }
	void c2s_CmdQueryDoubleExp() { ::c2s_SendCmdQueryDoubleExp(); }
	void c2s_CmdDuelRequest(int idTarget) { ::c2s_SendCmdDuelRequest(idTarget); }
	void c2s_CmdDuelReply(int idWho, int iReply) { ::c2s_SendCmdDuelReply(idWho, iReply); }
	void c2s_CmdBindPlayerRequest(int idTarget) { ::c2s_SendCmdBindPlayerRequest(idTarget); }
	void c2s_CmdBindPlayerInvite(int idTarget) { ::c2s_SendCmdBindPlayerInvite(idTarget); }
	void c2s_CmdBindPlayerRequestRe(int idWho, int iReply) { ::c2s_SendCmdBindPlayerRequestRe(idWho, iReply); }
	void c2s_CmdBindPlayerInviteRe(int idWho, int iReply) { ::c2s_SendCmdBindPlayerInviteRe(idWho, iReply); }
	void c2s_CmdCancelBindPlayer() { ::c2s_SendCmdCancelBindPlayer(); }
	void c2s_CmdDebugDeliverCmd(WORD type, const char* szCmd) { ::c2s_SendCmdDebugDeliverCmd(type, szCmd); }
	void c2s_CmdDebugGSCmd(const char* szCmd) { ::c2s_SendCmdDebugGSCmd(szCmd); }
	void c2s_SendCmdMallShopping(int iCount, const void* pGoods) { ::c2s_SendCmdMallShopping(iCount, pGoods); }
	void c2s_CmdGetWallowInfo() { ::c2s_SendCmdGetWallowInfo(); }
	void c2s_CmdUseItemWithArg(unsigned char where, unsigned char count, unsigned short index, int item_id, const void* arg, size_t size) { ::c2s_SendCmdUseItemWithArg(where, count, index, item_id, arg, size); }
	void c2s_CmdNotifyForceAttack(int iForceAttack, BYTE refuseBless) { ::c2s_SendCmdNotifyForceAttack(iForceAttack, refuseBless); }
	void c2s_CmdGetDividendMallItemPrice(int start_index, int end_index) { ::c2s_SendCmdGetDividendMallItemPrice(start_index, end_index); }
	void c2s_SendCmdDividendMallShopping(int iCount, const void* pGoods) { ::c2s_SendCmdDividendMallShopping(iCount, pGoods); }
	void c2s_CmdChooseMultiExp(int iIndex){ ::c2s_SendCmdChooseMultiExp(iIndex); }
	void c2s_CmdToggleMultiExp(bool isActivate){ ::c2s_SendCmdToggleMultiExp(isActivate); }
	void c2s_CmdMultiExchangeItem(int iPackage, int iCount, const int* aIndexPairs){ ::c2s_SendCmdMultiExchangeItem(iPackage, iCount, aIndexPairs); }
	void c2s_CmdCongregateReply(int type, bool agree, int sponsor_id){ ::c2s_SendCmdCongregateReply(type, agree, sponsor_id); }
	void c2s_CmdGetForceGlobalData(){ ::c2s_SendCmdGetForceGlobalData(); }
	void c2s_CmdPreviewProduceReply(bool bSelectOld){::c2s_SendCmdPreviewProduceReply(bSelectOld);}
	void c2s_SendCmdQueryProfitTime() { ::c2s_SendCmdQueryProfitTime(); }
	void c2s_CmdSendEnterPKPrecinct() { ::c2s_SendCmdEnterPKPrecinct(); }	
	void c2s_CmdSendCountryGetPersonalScore() { ::c2s_SendCmdCountryGetPersonalScore(); }
	void c2s_CmdSendGetServerTime() { ::c2s_SendCmdGetServerTime(); }
	void c2s_CmdSendCountryLeaveWar(){ ::c2s_SendCmdCountryLeaveWar(); }
	void c2s_CmdGetCashMoneyRate() { ::c2s_SendCmdGetCashMoneyRate(); }
	void c2s_CmdExchangeYinpiao(bool is_sell, int count) { ::c2s_SendCmdExchangeYinpiao(is_sell, count); }
	void c2s_CmdMeridiansImpact(int index) { ::c2s_SendCmdMerdiansImpact(index); }
	void c2s_CmdCountryGetStrongHoldState() { ::c2s_SendCmdCountryGetStrongHoldState(); }

	void c2s_CmdGivePresent(int roleid, int mail_id, int goods_id, int goods_index, int goods_slot) { ::c2s_SendCmdGivePresent(roleid, mail_id, goods_id, goods_index, goods_slot); }
	void c2s_CmdAskForPresent(int roleid, int goods_id, int goods_index, int goods_slot) { ::c2s_SendCmdAskForPresent(roleid, goods_id, goods_index, goods_slot); }
	void c2s_CmdQueryTouchPoint(){::c2s_SendCmdQueryTouchPoint();}
	void c2s_CmdSpendTouchPoint(unsigned int index, unsigned int lots, int id, unsigned int num, unsigned int price, int expire_time)
		{::c2s_SendCmdSpendTouchPoint(index, lots, id, num, price, expire_time);}
	void c2s_CmdQueryTitle(int roleid){::c2s_SendCmdQueryTitle(roleid);}
	void c2s_CmdChangeTitle(int titleid){::c2s_SendCmdChangeTitle(titleid);}
	void c2s_CmdDaylySignin() {::c2s_SendCmdDaylySignin();}
	void c2s_CmdLateSignin(char type, int item_pos, int dest_time){::c2s_SendCmdLateSignin(type, item_pos, dest_time);}
	void c2s_CmdSignInAward(char type, int mon){::c2s_SendCmdSignInAward(type, mon);}
	void c2s_CmdRefreshSignIn(){::c2s_SendCmdRefreshSignIn();}
	void c2s_CmdSwitchParallelWorld(int line){ ::c2s_SendCmdSwitchParallelWorld(line); }
	void c2s_CmdQueryParallelWorld(){ ::c2s_SendCmdQueryParallelWorld(); }
	void c2s_CmdAutoTeamSetGoal(int type, int goal_id, int op) { ::c2s_SendCmdAutoTeamSetGoal(type, goal_id, op); }
	void c2s_CmdAutoTeamJumpToGoal(int goal_id) { ::c2s_SendCmdAutoTeamJumpToGoal(goal_id); }
	void c2s_CmdGetReincarnationTome() { ::c2s_SendCmdGetReincarnationTome();}
	void c2s_CmdRewriteReincarnationTome(unsigned int record_index, int record_level) { ::c2s_SendCmdRewriteReincarnationTome(record_index, record_level); }
	void c2s_CmdActivateReincarnationTome(char active) {  ::c2s_SendCmdActivateReincarnationTome(active); }
	void c2s_CmdSwallowGeneralCard(unsigned char equip_index, unsigned char is_inv, unsigned char swallowed_index, unsigned int count) { ::c2s_SendCmdSwallowGeneralCard(equip_index, is_inv, swallowed_index, count); }
	void c2s_CmdSendMassMail(int type, const ACHAR* szTitle, const ACHAR* szContext, const abase::vector<int> &receivers);
	void c2s_CmdSendActiveWayPoints(int count,int* pData) { ::c2s_SendCmdActiveTransPoint(count, pData);}
	void c2s_CmdSendReenterInstance(bool agree){ ::c2s_SendCmdReenterInstance(agree); }

	//	Team commands ...
	void c2s_CmdTeamInvite(int idPlayer) { ::c2s_SendCmdTeamInvite(idPlayer); }
	void c2s_CmdTeamAgreeInvite(int idLeader, int iTeamSeq) { ::c2s_SendCmdTeamAgreeInvite(idLeader, iTeamSeq); }
	void c2s_CmdTeamRejectInvite(int idLeader) { ::c2s_SendCmdTeamRejectInvite(idLeader); }
	void c2s_CmdTeamLeaveParty() { ::c2s_SendCmdTeamLeaveParty(); }
	void c2s_CmdTeamKickMember(int idMember) { ::c2s_SendCmdTeamKickMember(idMember); }
	void c2s_CmdTeamSetPickupFlag(short sFlag) { ::c2s_SendCmdTeamSetPickupFlag(sFlag); }
	void c2s_CmdTeamMemberPos(int iNumMem, int* aMemIDs) { ::c2s_SendCmdTeamMemberPos(iNumMem, aMemIDs); }
	void c2s_CmdTeamAssistSel(int idTeamMember) { ::c2s_SendCmdTeamAssistSel(idTeamMember); }
	void c2s_CmdTeamAskJoin(int idTarget) { ::c2s_SendCmdTeamAskJoin(idTarget); }
	void c2s_CmdTeamReplyJoinAsk(int idAsker, bool bAgree) { ::c2s_SendCmdTeamReplyJoinAsk(idAsker, bAgree); }
	void c2s_CmdTeamChangeLeader(int idLeader) { ::c2s_SendCmdTeamChangeLeader(idLeader); }
	void c2s_CmdTeamNotifyOwnPos() { ::c2s_SendCmdTeamNotifyOwnPos(); }
	void c2s_CmdTeamDismissParty() { ::c2s_SendCmdTeamDismissParty(); }
	//	Inventory and equipment commands ...
	void c2s_CmdGetIvtrData(int iPackage) { ::c2s_SendCmdGetIvtrData(iPackage); }
	void c2s_CmdGetIvtrDetailData(int iPackage) { ::c2s_SendCmdGetIvtrDetailData(iPackage); }
	void c2s_CmdMoveIvtrItem(int iSrc, int iDest, int iAmount);
	void c2s_CmdExgIvtrItem(int iIndex1, int iIndex2);
	void c2s_CmdDropIvtrItem(int iIndex, int iAmount);
	void c2s_CmdDropEquipItem(int iIndex);
	void c2s_CmdExgEquipItem(int iIndex1, int iIndex2);
	void c2s_CmdEquipItem(int iIvtrIdx, int iEquipIdx);
	void c2s_CmdMoveItemToEquip(int iIvtrIdx, int iEquipIdx);
	void c2s_CmdGetOtherEquip(int iNumID, int* aIDs) { ::c2s_SendCmdGetOtherEquip(iNumID, aIDs); }
	void c2s_CmdGetWealth(bool bpd, bool bed, bool btd) { ::c2s_SendCmdGetWealth(bpd, bed, btd); }
	void c2s_CmdGetItemInfo(BYTE byPackage, BYTE bySlot) { ::c2s_SendCmdGetItemInfo(byPackage, bySlot); }
	void c2s_CmdPickup(int idItem, int tid) { m_CmdCache.SendCmdPickUp(idItem, tid); }
	void c2s_CmdUseItem(BYTE byPackage, BYTE bySlot, int tid, BYTE byCount) { m_CmdCache.SendCmdUseItem(byPackage, bySlot, tid, byCount); }	
	void c2s_CmdUseSharpen(int iIvtrIdx, int tid, int iEquipIdx);
	void c2s_CmdUseItemWithTarget(BYTE byPackage, BYTE bySlot, int tid, BYTE byPVPMask) { ::c2s_SendCmdUseItemWithTarget(byPackage, bySlot, tid, byPVPMask); }
	void c2s_CmdThrowMoney(DWORD dwAmount) { ::c2s_SendCmdThrowMoney(dwAmount); }
	void c2s_CmdGetItemInfoList(int iPackage, int iCount, BYTE* aIndices) { ::c2s_SendCmdGetItemInfoList(iPackage, iCount, aIndices); }
	void c2s_CmdGetTrashBoxData(bool bDetail, int iAccountBox = 0) { ::c2s_SendCmdGetTrashBoxData(bDetail, iAccountBox); }
	void c2s_CmdExgTrashBoxItem(int where, int iIndex1, int iIndex2);
	void c2s_CmdMoveTrashBoxItem(int where, int iSrc, int iDst, int iAmount);
	void c2s_CmdExgTrashBoxIvtrItem(int where, int iTrashIdx, int iIvtrIdx);
	void c2s_CmdMoveTrashBoxToIvtr(int where, int iTrashIdx, int iIvtrIdx, int iAmount);
	void c2s_CmdMoveIvtrToTrashBox(int where, int iTrashIdx, int iIvtrIdx, int iAmount);
	void c2s_CmdExgTrashBoxMoney(int iTrashMoney, int iIvtrMoney, int iAccountBox = 0) { ::c2s_SendCmdExgTrashBoxMoney(iTrashMoney, iIvtrMoney, iAccountBox); }
	void c2s_CmdGetOtherEquipDetail(int idTarget) { ::c2s_SendCmdGetOtherEquipDetail(idTarget); }
	void c2s_CmdGetMallItemPrice(int start_index, int end_index) { ::c2s_SendCmdGetMallItemPrice(start_index, end_index); }
	void c2s_CmdEquipFashionItem(int iBody, int iLeg, int iFoot, int iWrist, int iHead, int iWeapon) { ::c2s_SendCmdEquipFashionItem(iBody, iLeg, iFoot, iWrist, iHead, iWeapon); }
	void c2s_CmdOpenFashionTrash(const char* szPsw) { ::c2s_SendCmdOpenFashionTrash(szPsw); }
	void c2s_CmdEquipTrashBoxItem(unsigned char trashbox_index, unsigned char item_in_box_index, unsigned char equip_index) { ::c2s_SendCmdEquipTrashBoxItem(trashbox_index, item_in_box_index, equip_index); }
	void c2s_CmdCountryBattleLiveShow() { ::c2s_SendCmdCountryBattleLiveShow(); }
	void c2s_CmdRandMallShopping(int config_id, int op) { ::c2s_SendCmdRandMallShopping(config_id, op); }
	void c2s_CmdQueryFactionPVPInfo(int faction_id){ m_CmdCache.SendCmdFactionPVPQueryInfo(faction_id); }

	//	Pet commands ...
	void c2s_CmdPetSummon(int iPetIdx) { ::c2s_SendCmdPetSummon(iPetIdx); } 
	void c2s_CmdPetRecall() { ::c2s_SendCmdPetRecall(); }
	void c2s_CmdPetBanish(int iPetIdx) { ::c2s_SendCmdPetBanish(iPetIdx); }
	void c2s_CmdPetCtrl(int idTarget, int cmd, void* pParamBuf, int iParamLen) { m_CmdCache.SendCmdPetCtrl(idTarget, cmd, pParamBuf, iParamLen); }
	void c2s_CmdPetEvolution(int iPetIdx,int iFormula) {::c2s_SendCmdPetEvolution(iPetIdx,iFormula);}
	void c2s_CmdPetAddExp(int iPetIdx,int iItemNum) {::c2s_SendCmdPetAddExp(iPetIdx,iItemNum);}
	void c2s_CmdPetRebuildNature(int iPetIdx,int iFormula) {::c2s_SendCmdPetRebuildNature(iPetIdx,iFormula);}
	void c2s_CmdPetRebuildInheritRatio(int iPetIdx,int iFormula){::c2s_SendCmdPetRebuildInheritRatio(iPetIdx,iFormula);}
	void c2s_CmdPetRebuildInheritChoose(bool bAccept) {::c2s_SendCmdPetRebuildInheritChoose(bAccept);}
	void c2s_CmdPetRebuildNatureChoose(bool bAccept) {::c2s_SendCmdPetRebuildNatureChoose(bAccept);}

	//  Goblin commands ...
	void c2s_CmdGoblinAddAttribute(int str, int agi, int vit, int eng) { ::c2s_SendCmdGoblinAddAttribute(str,agi, vit, eng); }
	void c2s_CmdGoblinAddGenius(int g1, int g2, int g3, int g4, int g5) { ::c2s_SendCmdGoblinAddGenius(g1,g2,g3,g4,g5); }
	void c2s_CmdGoblinPlayerInsertExp(unsigned int exp, int use_sp = 0) { ::c2s_SendCmdGoblinPlayerInsertExp(exp, use_sp); }
	void c2s_CmdGoblinEquipItem(int index_inv) { ::c2s_SendCmdGoblinEquipItem(index_inv); }
	void c2s_CmdGoblinChangeSecureStatus(unsigned char status) { ::c2s_SendCmdGoblinChangeSecureStatus(status); }
	void c2s_CmdGoblinCastSkill(int idSkill, unsigned char force_attack, int iNumTarget, int* aTargets) { ::c2s_SendCmdGoblinCastSkill(idSkill, force_attack, iNumTarget, aTargets); }
	void c2s_CmdGoblinChargeEquip(int iEleIdx, int iCount);
	void c2s_CmdGoblinCharge(int iEleIdx, int iGoblinIdx, int iCount, int idGoblin);
	void c2s_CmdGoblinRefineActive(int item_id) { ::c2s_SendCmdGoblinRefineActive(item_id); }
	//	NPC service commands ...
	void c2s_CmdNPCSevHello(int nid) { m_CmdCache.SendCmdNPCSevHello(nid); }
	void c2s_CmdNPCSevBuy(int iItemNum, C2S::npc_trade_item* aItems) { ::c2s_SendCmdNPCSevBuy(iItemNum, aItems); }
	void c2s_CmdNPCSevSell(int iItemNum, C2S::npc_sell_item* aItems);
	void c2s_CmdNPCSevRepair(BYTE byPackage, BYTE bySlot, int idItem) { ::c2s_SendCmdNPCSevRepair(byPackage, bySlot, idItem); }
	void c2s_CmdNPCSevRepairAll() { ::c2s_SendCmdNPCSevRepairAll(); }
	void c2s_CmdNPCSevHeal() { ::c2s_SendCmdNPCSevHeal(); }
	void c2s_CmdNPCSevTransmit(int iIndex) { ::c2s_SendCmdNPCSevTransmit(iIndex); }
	void c2s_CmdNPCSevEmbed(WORD wStoneIdx, WORD wEquipIdx, int tidStone, int tidEquip) { ::c2s_SendCmdNPCSevEmbed(wStoneIdx, wEquipIdx, tidStone, tidEquip); }
	void c2s_CmdNPCSevClearEmbeddedChip(int iEquipIdx, int tidEquip) { ::c2s_SendCmdNPCSevClearEmbeddedChip(iEquipIdx, tidEquip); }
	void c2s_CmdNPCSevLearnSkill(int idSkill) { ::c2s_SendCmdNPCSevLearnSkill(idSkill); }
	void c2s_CmdNPCSevMakeItem(int idSkill, int idItem, DWORD dwCount) { ::c2s_SendCmdNPCSevMakeItem(idSkill, idItem, dwCount); }
	void c2s_CmdNPCSevBreakItem(int idSkill, int iIvtrIdx, int idItem) { ::c2s_SendCmdNPCSevBreakItem(idSkill, iIvtrIdx, idItem); }
	void c2s_CmdNPCSevAcceptTask(int idTask,int idStorage,int idRefreshItem) { ::c2s_SendCmdNPCSevAcceptTask(idTask,idStorage,idRefreshItem); }
	void c2s_CmdNPCSevReturnTask(int idTask, int iChoice) { ::c2s_SendCmdNPCSevReturnTask(idTask, iChoice); }
	void c2s_CmdNPCSevTaskMatter(int idTask) { ::c2s_SendCmdNPCSevTaskMatter(idTask); }
	void c2s_CmdNPCSevChgTrashPsw(const char* szOldPsw, const char* szNewPsw) { ::c2s_SendCmdNPCSevChgTrashPsw(szOldPsw, szNewPsw); }
	void c2s_CmdNPCSevOpenTrash(const char* szPsw) { ::c2s_SendCmdNPCSevOpenTrash(szPsw); }
	void c2s_CmdNPCSevViewTrash() { ::c2s_SendCmdNPCSevViewTrash(); }
	void c2s_CmdNPCSevIdentify(int iSlot, int tidItem) { ::c2s_SendCmdNPCSevIdentify(iSlot, tidItem); }
	void c2s_CmdNPCSevFaction(int iAction, void* pDataBuf, int iDataSize) { ::c2s_SendCmdNPCSevFaction(iAction, pDataBuf, iDataSize); }
	void c2s_CmdNPCSevTravel(int iIndex, int idLine) { ::c2s_SendCmdNPCSevTravel(iIndex, idLine); }
	void c2s_CmdNPCSevGetContent(int idService) { ::c2s_SendCmdNPCSevGetContent(idService); }
	void c2s_CmdNPCSevBoothBuy(int idBooth, int iItemNum, C2S::npc_booth_item* aItems, int iYinpiao = 0) { ::c2s_SendCmdNPCSevBoothBuy(idBooth, iItemNum, aItems, iYinpiao); }
	void c2s_CmdNPCSevBoothSell(int idBooth, int iItemNum, C2S::npc_booth_item* aItems);
	void c2s_CmdNPCSevWaypoint() { ::c2s_SendCmdNPCSevWaypoint(); }
	void c2s_CmdNPCSevForgetSkill(int idSkill) { ::c2s_SendCmdNPCSevForgetSkill(idSkill); }
	void c2s_CmdNPCSevFaceChange(int iIvtrIdx, int idItem) { ::c2s_SendCmdNPCSevFaceChange(iIvtrIdx, idItem); }
	void c2s_CmdNPCSevMail(int iAction, void* pDataBuf, int iDataSize) { ::c2s_SendCmdNPCSevMail(iAction, pDataBuf, iDataSize); }
	void c2s_CmdNPCSevVendue(int iAction, void* pDataBuf, int iDataSize) { ::c2s_SendCmdNPCSevVendue(iAction, pDataBuf, iDataSize); } 
	void c2s_CmdNPCSevDblExpTime(int iIndex) { ::c2s_SendCmdNPCSevDblExpTime(iIndex); }
	void c2s_CmdNPCSevHatchPet(int iIvtrIdx, int idEgg) { ::c2s_SendCmdNPCSevHatchPet(iIvtrIdx, idEgg); }
	void c2s_CmdNPCSevRestorePet(int iPetIdx) { ::c2s_SendCmdNPCSevRestorePet(iPetIdx); }
	void c2s_CmdNPCSevBattle(int iAction, void* pDataBuf, int iDataSize) { ::c2s_SendCmdNPCSevBattle(iAction, pDataBuf, iDataSize); }
	void c2s_CmdNPCSevBuildTower(int iTowerIdx, int idItem) { ::c2s_SendCmdNPCSevBuildTower(iTowerIdx, idItem); }
	void c2s_CmdNPCSevLeaveBattle() { ::c2s_SendCmdNPCSevLeaveBattle(); }
	void c2s_CmdNPCSevReturnStatusPt(int iIndex, int idItem) { ::c2s_SendCmdNPCSevReturnStatusPt(iIndex, idItem); }
	void c2s_CmdNPCSevAccountPoint(int iAction, void* pDataBuf, int iDataSize) { ::c2s_SendCmdNPCSevAccountPoint(iAction, pDataBuf, iDataSize); }
	void c2s_CmdNPCSevRefine(int iIndex, int idItem, int iComponentIndex = -1) { ::c2s_SendCmdNPCSevRefine(iIndex, idItem, iComponentIndex); }
	void c2s_CmdNPCSevPetName(int iPetIdx, void* szNameBuf, int iBufLen) { ::c2s_SendCmdNPCSevPetName(iPetIdx, szNameBuf, iBufLen); }
	void c2s_CmdNPCSevPetSkillDel(int idSkill) { ::c2s_SendCmdNPCSevPetSkillDel(idSkill); }
	void c2s_CmdNPCSevPetSkillLearn(int idSkill) { ::c2s_SendCmdNPCSevPetSkillLearn(idSkill); }
	void c2s_SendCmdNPCSevBindItem(int iIndex, int iItemId, int iItemNeed) { ::c2s_SendCmdNPCSevBindItem(iIndex, iItemId, iItemNeed);	}
	void c2s_SendCmdNPCSevDestroyBind(int iIndex, int iItemId) { ::c2s_SendCmdNPCSevDestroyBind(iIndex, iItemId); }
	void c2s_SendCmdNPCSevCancelDestroy(int iIndex, int iItemId) { ::c2s_SendCmdNPCSevCancelDestroy(iIndex, iItemId); }
	void c2s_CmdNPCSevStockTransaction(int nWithdraw, int nCash, int nMoney) { ::c2s_SendCmdNPCSevStockTransaction(nWithdraw, nCash, nMoney); }
	void c2s_CmdNPCSevStockOperation(int nType, void* pData, int nLen) { ::c2s_SendCmdNPCSevStockOperation(nType, pData, nLen); }
	void c2s_CmdNPCSevDye(int inv_index, int item_type, int dye_index, int dye_type) { ::c2s_SendCmdNPCSevDye(inv_index, item_type, dye_index, dye_type); }
	void c2s_CmdNPCSevRefineTrans(int iSrcIdx, int idSrc, int iDstIdx, int idDst) { ::c2s_SendCmdNPCSevRefineTrans(iSrcIdx, idSrc, iDstIdx, idDst); }
	void c2s_CmdNPCSevCompose(int idSkill, int idRecipe, int aMaterialIDs[16], int aMaterialIdx[16]) { ::c2s_SendCmdNPCSevCompose(idSkill, idRecipe, aMaterialIDs, aMaterialIdx); }
	void c2s_CmdNPCSevMakeSlot(int src_index, int src_id) { ::c2s_SendCmdNPCSevMakeSlot(src_index, src_id); }
	void c2s_CmdQueryCashInfo() { ::c2s_SendCmdQueryCashInfo(); }
	void c2s_CmdNPCSevPetDye(int pet_index, int pet_id, int dye_index, int dye_id) { ::c2s_SendCmdNPCSevPetDye(pet_index, pet_id, dye_index, dye_id); }
	void c2s_CmdNPCSevEngrave(int engrave_id, int ivtr_index) { ::c2s_SendCmdNPCSevEngrave(engrave_id, ivtr_index); }
	void c2s_CmdNPCSevAddonRegen(int recipe_id, int ivtr_index) { ::c2s_SendCmdNPCSevAddonRegen(recipe_id, ivtr_index); }
	void c2s_CmdNPCSevMark(int inv_index, int item_type, int dye_index, int dye_type, void* szNameBuf, int iBufLen) { ::c2s_SendCmdNPCSevMark(inv_index, item_type, dye_index, dye_type, szNameBuf, iBufLen); }
	void c2s_SendCmdNPCSevSplitFashion(int iIndex, int iItemId) { ::c2s_SendCmdNPCSevSplitFashion(iIndex, iItemId);	}

	void c2s_CmdNPCSevGoblinReturnStatusPt(int iIndex, int idItem, int str, int agi, int vit, int eng) { ::c2s_SendCmdNPCSevGoblinReturnStatusPt( iIndex,  idItem, str, agi, vit, eng); }
	void c2s_CmdNPCSevGoblinReturnGeniusPt(int iIndex, int idItem) { ::c2s_SendCmdNPCSevGoblinReturnGeniusPt( iIndex,  idItem); }
	void c2s_CmdNPCSevGoblinLearnSkill(int iIndex, int idSkill) { ::c2s_SendCmdNPCSevGoblinLearnSkill( iIndex,  idSkill); }
	void c2s_CmdNPCSevGoblinForgetSkill(int iIndex, int idSkill,  int forget_level) { ::c2s_SendCmdNPCSevGoblinForgetSkill( iIndex,  idSkill, forget_level); }
	void c2s_CmdNPCSevGoblinRefine(int iIndex, int idItem, int iTicketCnt = 1) { ::c2s_SendCmdNPCSevGoblinRefine( iIndex,  idItem,  iTicketCnt); }
	void c2s_CmdNPCSevGoblinTransferRefine(int iIndexSrc, int iIndexDest) {  ::c2s_SendCmdNPCSevGoblinTransferRefine( iIndexSrc,  iIndexDest); }
	void c2s_CmdNPCSevGoblinDestroy(int iIndex) { ::c2s_SendCmdNPCSevGoblinDestroy( iIndex); }
	void c2s_CmdNPCSevGoblinEquipDestroy(int iIndex, int iSlot, int equip_index = 255) {  ::c2s_SendCmdNPCSevGoblinEquipDestroy( iIndex,  iSlot,  equip_index); }
	void c2s_CmdNPCSevDyeBySuit(int iBody, int iLeg, int iFoot, int iWrist, int iDye) { ::c2s_SendCmdNPCSevDyeBySuit(iBody, iLeg, iFoot, iWrist, iDye); }
	void c2s_CmdNPCSevRepairDestroyingItem(int iIndex) { ::c2s_SendCmdNPCRepairDestroyingItem(iIndex); }
	void c2s_CmdNPCSevLevelUpProduce(int idSkill, int idRecipe, int aMaterialIDs[16], int aMaterialIdx[16], int iEquipId, int iEquipInvId, int iInheritType, bool bPreview = false, bool bAdvanced = false) { ::c2s_SendCmdNPCSevLevelUpProduce(idSkill, idRecipe, aMaterialIDs, aMaterialIdx, iEquipId, iEquipInvId, iInheritType, bPreview, bAdvanced); }
	void c2s_CmdQueryEquipmentPropData(int iEuipId, int iEquipInvId) { ::c2s_SendCmdEquipmentPropData(iEuipId, iEquipInvId); }
	void c2s_CmdNPCSevOpenAccountBox() { ::c2s_SendCmdNPCSevOpenAccountBox(); }
	
	void c2s_CmdNPCSevGodEvilConvert(int iTypeIndex) { ::c2s_SendCmdNPCSevGodEvilConvert(iTypeIndex); }
	void c2s_CmdNPCSevGetDPSDPHRank(int rank_mask) { ::c2s_SendCmdNPCSevGetDPSDPHRank(rank_mask);}
	void c2s_CmdNPCSevTransmitDirect(int iNum, int *aWayPoints) { ::c2s_SendCmdNPCSevTransmitDirect(iNum, aWayPoints); }
	void c2s_CmdNPCSevCountryJoinLeave(bool bJoin){ ::c2s_SendCmdNPCSevCountryJoinLeave(bJoin); }
	void c2s_CmdNPCSevCountryLeaveWar(){ ::c2s_SendCmdNPCSevCountryLeaveWar(); }
	void c2s_CmdNPCSevCrossServerGetIn() { ::c2s_SendCmdNPCSevCrossServerGetIn(); }
	void c2s_CmdNPCSevCrossServerGetOut() { ::c2s_SendCmdNPCSevCrossServerGetOut(); }
	void c2s_CmdNPCSevPlayerRename(int item_index,int item_id,unsigned short nameLen,const char*pNewName) { ::c2s_SendCmdNPCSevPlayerRename(item_index,item_id,nameLen,pNewName); }
	void c2s_CmdNPCSevPlayerChangeGender(int item_index,int item_id,unsigned char new_gender,void *customize_data,unsigned short customize_data_len) { ::c2s_SendCmdNPCSevPlayerChangeGender(item_index, item_id, new_gender, customize_data, customize_data_len); }
	void c2s_CmdNPCSevStoneTransfer(int equip_idx, int stone_idx, int oldstone_tid, int newstone_tid, int recipe_tid,int *aMaterialIDs, int *aMaterialIdx){::c2s_SendCmdNPCSevStoneTransfer(equip_idx, stone_idx, oldstone_tid, newstone_tid, recipe_tid,aMaterialIDs, aMaterialIdx);}
	void c2s_CmdNPCSevStoneReplace(int equip_idx, int stone_idx, int oldstone_tid, int newstone_idx, int newstone_tid){::c2s_SendCmdNPCSevStoneReplace(equip_idx, stone_idx, oldstone_tid, newstone_idx, newstone_tid);}
	void c2s_CmdNPCSevKing(int iAction, void* pDataBuf, int iDataSize) { ::c2s_SendCmdNPCSevKing(iAction, pDataBuf, iDataSize); }
	void c2s_CmdNPCSevGiftCard(const char* card_num) { ::c2s_SendCmdNPCSevGiftCard(card_num); }
	void c2s_CmdNPCSevReincarnation() { ::c2s_SendCmdNPCSevReincarantion(); }
	void c2s_CmdNPCSevTrickBattleRequest(int actor) { ::c2s_SendCmdNPCSevTrickBattleRequest(actor); } // 跨服活动NPC服务
	void c2s_CmdNPCSevOpenFactionPVP(){ ::c2s_SendCmdNPCSevOpenFactionPVP(); }
	void c2s_CmdNPCSevMallShopping(int iCount, const void* pGoods){ ::c2s_SendCmdNPCSevMallShopping(iCount, pGoods); }
	void c2s_CmdNPCSevDividendMallShopping(int iCount, const void* pGoods){ ::c2s_SendCmdNPCSevDividendMallShopping(iCount, pGoods); }

	//	结婚相关服务
	void c2s_CmdNPCSevWeddingBook(int type, int start_time, int end_time, int scene, int bookcard_index) { ::c2s_SendCmdNPCSevWeddingBook(type, start_time, end_time, scene, bookcard_index); }
	void c2s_CmdNPCSevWeddingInvite(int invitecard_index, int invitee) { ::c2s_SendCmdNPCSevWeddingInvite(invitecard_index, invitee); }
	
	void c2s_CmdQueryNetworkDelay(int timestamp){ ::c2s_SendCmdQueryNetworkDelay(timestamp); }
	
	//	帮派基地相关
	void c2s_CmdGetFactionFortressInfo() { ::c2s_SendCmdGetFactionFortressInfo(); }

	//	阵营势力相关
	void c2s_CmdNPCSevForce(int type, int param) { ::c2s_SendCmdNPCSevForce(type, param); }

	void c2s_CmdNPCSevFactionFortressLevelup() { ::c2s_SendCmdNPCSevFactionFortressService2(RT_LEVEL_UP, 0, 0, 0); }
	void c2s_CmdNPCSevFactionFortressTechLevelup(int iTech) { ::c2s_SendCmdNPCSevFactionFortressService2(RT_SET_TECH_POINT, iTech, 0, 0); }
	void c2s_CmdNPCSevFactionFortressConstructLevelup(int idBuilding, int nAccelerate) { ::c2s_SendCmdNPCSevFactionFortressService2(RT_CONSTRUCT, idBuilding, nAccelerate, 0); }
	void c2s_CmdNPCSevFactionFortressHandInMaterial(int iPackIndex, int idMaterial, int count) { ::c2s_SendCmdNPCSevFactionFortressService2(RT_HAND_IN_MATERIAL, iPackIndex, idMaterial, count); }
	void c2s_CmdNPCSevFactionFortressHandInContrib(int nContrib) { ::c2s_SendCmdNPCSevFactionFortressService2(RT_HAND_IN_CONTRIB, nContrib, 0, 0); }
	void c2s_CmdNPCSevFactionFortressConvertMaterial(int iSrc, int iDst, int nSrc) { ::c2s_SendCmdNPCSevFactionFortressService3(iSrc, iDst, nSrc); }
	void c2s_CmdNPCSevFactionFortressLeave(){ ::c2s_SendCmdNPCSevFactionFortressService2(RT_LEAVE_FORTRESS, 0, 0, 0); }
	void c2s_CmdNPCSevFactionFortressConstructDestroy(int idBuilding) { ::c2s_SendCmdNPCSevFactionFortressService2(RT_DISMANTLE, idBuilding, 0, 0); }
	void c2s_CmdNPCSevFactionFortressResetTech(int iTech, int iSlot, int tid) { ::c2s_SendCmdNPCSevFactionFortressService2(RT_RESET_TECH_POINT, iTech, iSlot, tid); }

	// 卡牌转生
	void c2s_CmdNPCSevCardRespawn(int majorIdx,int minorIdx) {::c2s_SendCmdNPCSevRespawn(majorIdx,minorIdx);}

	// 飞剑强化
	void c2s_CmdNPCSevFlySwordImprove(unsigned int idx,int flysword_id) {::c2s_SendCmdNPCSevFlySwordImprove(idx,flysword_id);}

	// 查询历史阶段的全局数据
	void c2s_CmdQueryUniqueData(const abase::vector<int>& key){::c2s_SendCmdQueryQuniqueData(key);}

	// 在线奖励
	void c2s_CmdRechargeOnlineAward(int type,int count,C2S::cmd_recharge_online_award::entry* pData){::c2s_SendCmdRechargeOnlineAward(type,count,pData);}
	void c2s_CmdToggleOnlineAward(int type,int act){::c2s_SendCmdToggleOnlineAward(type,act);}

	// 战车
	void c2s_CmdLevaveChariotBattle() {::c2s_SendCmdLeaveChariot();}
	void c2s_CmdUpdateChariot(int id) {::c2s_SendCmdUpdateChariot(id);}
	void c2s_CmdQueryChariotAmount() { ::c2s_SendCmdQueryChariotsAmount(); }

	void factionFortress_Create();
	void factionFortress_Enter(int idTarget);
	void factionFortress_List(int begin);
	void factionFortress_Challenge(int idTarget);
	void factionFortress_BattleList();
	void factionFortress_Get(int idTarget);

	//	Trade commands ...
	bool trade_Start(int idTarget);
	bool trade_StartResponse(DWORD& dwHandle, bool bAgree);
	bool trade_AddGoods(int idTrade, int idItem, int iPos, int iCount, int iMoney);
	bool trade_RemoveGoods(int idTrade, int idItem, int iPos, int iCount, int iMoney);
	bool trade_MoveItem(int idTrade, int idItem, int iSrc, int iDst, int iCount);
	bool trade_Submit(int idTrade);
	bool trade_Confirm(int idTrade);
	bool trade_Cancel(int idTrade);

	//	UserCoupon commands
	void userCoupon_Account();
	void userCoupon_Exchange(int number);

	//	Stock commands
	void stock_account();
	void stock_bill();
	void stock_commission(char bBuy, int iPrice, int iVolume);
	void stock_cancel(int tid, int price);	
	//	GM commands ...
	void gm_KickOutRole(int idPlayer, int iTime, const ACHAR* szReason);
	void gm_KickOutUser(int idPlayer, int iTime, const ACHAR* szReason);
	void gm_ListOnlineUser(int iHandler);
	void gm_OnlineNumber();
	void gm_RestartServer(int idServer, int iRestartTime);
	void gm_ShutupRole(int idPlayer, int iTime, const ACHAR* szReason);
	void gm_ShutupUser(int idPlayer, int iTime, const ACHAR* szReason);
	void gm_MoveToPlayer(int idPlayer) { ::c2s_SendCmdGMMoveToPlayer(idPlayer); }
	void gm_CallInPlayer(int idPlayer) { ::c2s_SendCmdGMCallInPlayer(idPlayer); }
	void gm_ForbidRole(int iType, int idPlayer, int iTime, const ACHAR* szReason);
	void gm_TriggerChat(bool bEnable);
	void gm_Generate(int tid) { ::c2s_SendCmdGMGenerate(tid); }
	void gm_ActiveSpawner(bool bActive, int id_spawner) { ::c2s_SendCmdGMActiveSpawner(bActive, id_spawner); }
	void gm_GenerateMob(int idMob, int idVis, short sCount, short sLifeTime, const ACHAR* szName) { ::c2s_SendCmdGMGenerateMob(idMob, idVis, sCount, sLifeTime, szName); }
	void gm_TriggerInvisible() { ::c2s_SendCmdGMTriggerInvisible(); }
	void gm_TriggerInvincible() { ::c2s_SendCmdGMTriggerInvincible(); }
	void gm_QuerySpecItem(int role_id, int item_id) { ::c2s_SendCmdGMQuerySpecItem(role_id, item_id); }
	void gm_RemoveSpecItem(int role_id, int item_id, unsigned char where, unsigned char index, unsigned int count) { ::c2s_SendCmdGMRemoveSpecItem(role_id, item_id, where, index, count); }
	void GetPlayerConsumeInfo(int iPlayerNum, const int* aIDs);
	void gm_PermanentLock(int idPlayer, bool bEnable);
	void gm_OpenActivity(int activity_id,bool bOpen) {::c2s_SendCmdGMOpenActivity(activity_id,bOpen);}
	void gm_ChangeDS(bool bToCrossServer) { ::c2s_SendCmdGMChangeDS(bToCrossServer); }

	//	Friend commands ...
	void friend_Add(int idPlayer, const ACHAR* szName);
	void friend_AddResponse(DWORD& dwHandle, bool bAgree);
	void friend_GetList();
	void friend_SetGroupName(int iGroup, const ACHAR* szName);
	void friend_SetGroup(int iGroup, int idFriend);
	void friend_Delete(int idFriend);
	void friend_GetOfflineMsg();
	void friend_SendMail(int idFriend,int idTemplete);
	void friend_SetRemarks(int idFriend, const ACHAR *szRemarks);
	//	Faction commands ...
	void faction_chat(const ACHAR* szMsg, int iPack=-1, int iSlot=-1);
	void faction_create(const ACHAR* szName, const ACHAR* szProclaim);
	void faction_change_proclaim(const ACHAR* szNew);
	void faction_accept_join(int iApplicant);
	void faction_expel_member(int iMember);
	void faction_cancel_expel_member(int iMember);
	void faction_appoint(int iMember, char new_position);
	void faction_master_resign(int iNewMaster);
	void faction_resign();
	void faction_leave();
	void faction_broadcast(const ACHAR* szMsg);
	void faction_dismiss();
	void faction_upgrade();
	void faction_degrade();
	void faction_listmember(int version);
	void faction_rename(int iRole, const ACHAR* szNewName);
	void faction_invite_response(DWORD& dwHandle, bool bAgree);
	void faction_player_info();
	
	void faction_get_onlinelist();
	void faction_relation_apply(int idTarget, bool bAlliance);
	void faction_relation_reply(int idTarget, bool bAlliance, bool bAgree);
	void faction_remove_relation_apply(int idTarget, bool bForce);
	void faction_remove_relation_reply(int idTarget, bool bAgree);
	void faction_get_relationlist();
	
	void faction_renamefaction(const ACHAR *szNewName);

	//	Chatroom commands ...
	void chatroom_Create(const ACHAR* szSubject, const ACHAR* szPassword, int iCapacity);
	void chatroom_Expel(int idRoom, int idPlayer);
	void chatroom_Invite(int idRoom, int idInvitee);
	void chatroom_Invite(int idRoom, const ACHAR* szName);
	void chatroom_RejectInvite(int idRoom, int idInviter);
	void chatroom_Join(int idRoom, const ACHAR* szPassword);
	void chatroom_Join(const ACHAR* szOwner, const ACHAR* szPassword);
	void chatroom_Leave(int idRoom);
	void chatroom_List(int iBegin, bool bReverse);
	void chatroom_Speak(int idRoom, int idDstPlayer, const ACHAR* szMsg);
	//	Mail commands ...
	void mail_CheckNew();
	void mail_GetList();
	void mail_Get(int idMail);
	void mail_GetAttachment(int idMail, int iType);
	void mail_Delete(int idMail);
	void mail_Preserve(int idMail, bool bPreserve);
	void mail_Send(int idReceiver, const ACHAR* szTitle, const ACHAR* szContext, int idItem, int iItemNum, int iItemPos, size_t dwMoney);
	//	Auction commands ...
	void auction_Open(int iCategory, int tid, int iItemPos, int iItemNum, int iBasePrice, int iBinPrice, int iTime);
	void auction_Bid(int idAuction, int iBidPrice, bool bBin);
	void auction_List(int iCategory, int iBegin, bool bReverse, int iItemID = 0);
	void auction_Close(int idAuction, int iReason);
	void auction_Get(int idAuction);
	void auction_AttendList();
	void auction_ExitBid(int idAuction);
	void auction_GetItems(int iNumItem, int* aAuctionIDs);
	void auction_ListUpdate(int iNumItem, int *aAuctionIDs);
	//  Web Trade Commands
	void webTrade_List(int iCategory, int iBegin, bool bReverse);
	void webTrade_AttendList(int iBegin, bool bSell);
	void webTrade_GetItems(int iNumItem, const __int64 *aTradeIDs);
	void webTrade_GetItemDetail(__int64 idTrade);
	void webTrade_PrePostMoney(int nMoney, int nPrice, int nSellPeriod /*sec*/, int nBuyerRoleID=0);
	void webTrade_PrePostItem(int idItem, int nSlot, int nCount, int nPrice, int nSellPeriod, int nBuyerRoleID=0);
	void webTrade_PrePostRole(int idRole, int nPrice, int nSellPeriod, int nBuyerRoleID=0);
	void webTrade_PreCancel(__int64 idTrade);
	void webTrade_RolePreCancelPost(int idRole);
	void webTrade_Update(__int64 idTrade);
	void webTrade_RoleGetDetail(int idRole);
	//	Sys Auction Commands
	void sysAuction_Account();
	void sysAuction_Bid(unsigned int sa_id, int bid_price);
	void sysAuction_CashTransfer(bool bWithdraw, int nCash);
	void sysAuction_GetItem(unsigned int *sa_ids, int count);
	void sysAuction_List();
	//	Battle commands ...
	void battle_Challenge(int idDomain, int idFaction, int iDeposit);
	void battle_ChallengeMap();
	void battle_GetMap();
	void battle_Enter(int idBattle);
	void battle_GetStatus();
	//	Account commands ...
	void account_GetSelfSellList();
	void account_GetShopSellList(int idStart, bool bReverse);
	void account_SellPoint(int iPoint, DWORD dwPrice);
	void account_CancelSellPoint(int idSell);
	void account_BuyPoint(int idSell, int idSeller);
	//	ping commands ...
	void resetPing();
	void server_Ping(int idServer, in_addr * addr, int nPort);
	//	account security commands ...
	void matrix_response(int iResponse);
	void cash_Lock(char cLock, const ACHAR* szPassword);
	void cash_SetPassword(const ACHAR* szPassword);
	//	Get the server error string
	bool GetServerError(int iRetCode, AWString& errMsg);
	//	Output link server error
	void OutputLinkSevError(int iRetCode);
	// ac answer
	void send_acanswer(int type, int seq, int reserved, int answer);
	// Autolock
	void autolock_set(int timeout);	

	// Dividend commands
	void dividend_GetReferenceCode();
	void dividend_GetListReferrals(int start_index);
	void dividend_WithdrawDividend();

	//  Consume rewards
	void reward_GetRewardList(int start_index);
	void reward_ExchangeConsumePoints(int reward_type);
	
	//	cash card
	void usercash_InstantAdd(const GNET::Octets& cardNum, const GNET::Octets& password);
	
	//	SSO
	enum SSOTicketInfoType
	{
		SSO_TI_ROLEINFO_1 = 1,
	};
	void sso_GetTicket(const GNET::Octets &local_context, const GNET::Octets &info=GNET::Octets(), int toaid=101, int tozoneid=-1);
	
	//	Country
	void country_Move(int dest_domain_id);
	void country_GetPlayerLocation();
	void country_GetMap();
	void country_GetConfig();
	void country_GetScore();
	void country_Enter(int domain_id);
	void country_BackToCapital();
	void country_GetBattleLimit(int domain_id);

	//  King protocols
	void king_CandidateAuction(int tid, int num);
	void king_CandidateVote(int candidate_id, int tid, int item_slot, int num);
	void king_GetCandidateStatus();
	void king_SetBattleLimit(int domain_id, const abase::vector<int>& occupation_cnt, const abase::vector<int>& ghost_value);
	void king_ClearBattleLimit(int domain_id);
	void king_AssignAssault(int domain_id, bool bBig);
	void king_GetKingPoint();

	// Offline shop 寄卖系统，(离线商店)
	void OffShop_CreateMyShop(int shop_type,int item_id,int item_pos,int item_num);
	void OffShop_AddItemToMySellInv(int tid,int shop_pos,unsigned int price,int count, int inv_pos);
	void OffShop_AddItemToMyBuyInv(int tid,int shop_pos,unsigned int price,int count);
	void OffShop_CancelItemFromMyShop(bool bSell,int pos);
	void OffShop_SetMyShopType(int iType);
	void OffShop_ActiveMyShop(int tid,int pos,int count); // 激活或延长期限	
	void OffShop_ClearMyShop();	// 情况商店的收购栏和出售栏
	void OffShop_GetMyShopInfo();		
	void OffShop_TakeItemFromStore(int idx); // 从仓库取物品
	void OffShop_OptMoneyStore(bool bSave,int m,int yinpiao); // bSave: true 存钱，false 取钱
	void OffShop_GetELShopInfo(int otherID); // 进入其他玩家店铺，获取该店铺信息
	void OffShop_BuyItemFrom(int otherID,int tid,int pos,int count, int money,int yinpiao);
	void OffShop_SellItemTo(int otherID,int tid,int pos,int count, unsigned int price,int my_inv_pos);	
	void OffShop_QueryShops(int iType);		// 查询商店
	void OffShop_QueryItemListPage(int tid,bool bSell,int pageNum); // 查询物品，一次一页

	void match_GetProfile();
	void match_SaveProfile(int timemask,int game_intres_mask,int personal_intres_amsk,unsigned char age,unsigned char zodiac, int match_mask);
	void match_FindPlayer(int mode);

	//  Quick Pay
	void quickpay_AddCash(int cash, int cash_after_discount, int merchant_id);
	void quickpay_GetActivatedServices();

	enum ServerConfigKeyType
	{
		SCK_COUNTRYBATTLE_BONUS = 0,		//	国战总奖励（可能为跨服或单服）
	};
	void getServerConfig(int key);
	void getServerConfig(const abase::vector<int>& keys);

	// 跨服活动 坦克战
	void tankBattle_GetRank(); 

	// 帮派 PVP 活动
	void factionPVP_GetMap(){ m_CmdCache.SendFactionPVPGetMap(); }
	void factionPVP_GetRank(){ m_CmdCache.SendFactionPVPGetRank(); }

	void sendClientMachineInfo(unsigned char *info, int size);

	//	Set user name
	void SetUserName(const char* szName, const ACString &strUnicodeName) { m_strUserName = szName; m_strUserNameUnicode = strUnicodeName; }
	//	Get user name
	const char* GetUserName() { return m_strUserName; }
	//	Set user password
	void SetUserPassword(const char* szPass) { m_strPassword = szPass; }
	//	Get user password
	const char* GetUserPassword() { return m_strPassword; }
	//	Set user login token
	void SetLoginToken(const char* szToken, char type) { m_strToken = szToken; m_tokenType = type; }
	AString GetTicketAccount();
	AString GetAgentAccount();
	void SetAgentName(const char *szName) { m_strAgentName = szName; }
	const char * GetAgentName() { return m_strAgentName; }
	//	Set user ID
	void SetUserID(int iUserID) { m_iUserID = iUserID; }
	//	Get user ID
	int GetUserID() { return m_iUserID; }
	//	Set user character ID
	void SetCharacterID(int iCharID) { m_iCharID = iCharID; }
	//	Get user character ID
	int GetCharacterID() { return m_iCharID; }
	//	Get connected flag
	bool IsConnected() { return m_bConnected; }
	//	Get kick user flag
	bool GetKickUserFlag() { return m_bKickUser; }
	//	Set kick user flag
	void SetKickUserFlag(bool bKick) { m_bKickUser = bKick; }
	//	Get account remained time
	__int64 GetAccountTime() { return m_iiAccountTime; }
	//	Get account remained free game time
	__int64 GetFreeGameTime() { return m_iiFreeGameTime; }
	//	Get account created time
	DWORD GetAccCreatedTime() { return m_dwCreatedTime; }
	//	Get newbie create time
	DWORD GetNewbieTime() { return m_dwNewbieTime; }
	//	Get end time of free game
	int GetFreeGameEndTime() { return m_iFreeGameEnd; }
	//	Get overtime counter
	bool GetOvertimeCnt(DWORD& dwTotalTime, DWORD& dwCnt);
	//	Get server attributes
	const SEVATTR& GetServerAttr() { return m_SevAttr; }

	//	Get net manager
	GNET::GameClient* GetNetManager() { return m_pNetMan; }
	//  Get the C2S Command cache
	CECC2SCmdCache* GetC2SCmdCache() { return &m_CmdCache; }

	int GetZoneID();

	//  To break line in next tick
	void SetBreakLinkFlag(enumLinkBrokenReason reason);
	void ResetBreakLinkFlag();

	void LinkBroken(enumLinkBrokenReason reason);
	void ResetLinkBroken();

protected:	//	Attributes

	APtrArray<GNET::Protocol*>	m_aNewProtocols;	//	New protocol array
	APtrArray<GNET::Protocol*>	m_aOldProtocols;	//	Old protocol array
	APtrArray<GNET::Protocol*>	m_aTempProtocols;	//	Temporary protocol array

	APtrArray<GNET::Protocol*>	m_aPendingProtocols;//	protocols which are waiting player name
	AArray<int, int>			m_aPendingPlayers;	//	ID of players whose names are still unknown for protocols
	APtrArray<PINGTIME*>		m_Ping;				//	Ping time
	CRITICAL_SECTION			m_csPingSection;

	GNET::GameClient*	m_pNetMan;		//	Net manager
	DWORD				m_idLink;		//	Link ID
	CECStringTab		m_ErrorMsgs;	//	Server error messages
	CECC2SCmdCache		m_CmdCache;		//	C2S command cache

	volatile bool		m_bConnected;		//	Connected flag
	
	AString		m_strUserName;		//	User name
	ACString	m_strUserNameUnicode;//	Unicode User name
	AString		m_strPassword;		//	User password
	AString		m_strToken;				//	User login token
	AString		m_strAgentName;		//	SSO agent name
	char			m_tokenType;		//	User login token type
	int			m_iUserID;			//	User ID
	int			m_iCharID;			//	User character ID, this ID should be same as host player's cid
	bool		m_bKickUser;		//	true, kill user who is using the same account
	__int64		m_iiAccountTime;	//	Account remained time
	__int64		m_iiFreeGameTime;	//	Account Remained free game time
	DWORD		m_dwCreatedTime;	//	Account created time
	DWORD		m_dwNewbieTime;		//	Newbie start time
	int			m_iFreeGameEnd;		//	Free game end time
	DWORD		m_dwLastAccTime;
	HANDLE		m_hConnectThread;	//	Handle of connect thread
	SEVATTR		m_SevAttr;			//	Server attributes

	PONRECEIVEHELPSTATES m_pfnHelpStatesCallBack;

	OVERTIME	m_ot;				//	Overtime info

	int			m_nS2CCmdSize;		//	total server to client command size
	ALog*		m_pS2CCmdLog;		//	Command log file
	AFile*		m_pS2CCmdBinLog;	//	Command log in binary format
	
	bool					m_bToBreakLink;
	enumLinkBrokenReason	m_breakLinkReason;
	bool					m_bLinkBroken;
	enumLinkBrokenReason	m_linkBrokenReason;

	APtrArray<CECStubbornNetSender*>	m_aStubbornNetSenders;
	CECStubbornFactionInfoSender *		m_stubbornFactionInfoSender;

protected:	//	Operations

	//	Connect server
	bool Connect();
	void CloseConnectThread();
	//	Send net data
	bool SendNetData(const GNET::Protocol* p) { return p ? SendNetData(*p) : false; }
	//	Proecess game data
	void ProcessGameData(const GNET::Octets& Data);
	//	Calculate S2C command data size
	DWORD CalcS2CCmdDataSize(int iCmd, BYTE* pDataBuf, DWORD dwDataSize);
	//	Log S2C command data
	void LogS2CCmdData(int iCmd, BYTE* pDataBuf, int nSize);
	void BreakLink();
	//	When connection was broken, this function is called
	void OnLinkBroken();
	//	Filter protocols
	bool FilterProtocols(GNET::Protocol* pProtocol);
	//	Start / End overtime
	void DoOvertimeCheck(bool bStart, int iCheckID, DWORD dwTime);
	//	On overtime happens
	void OnOvertimeHappen();
	//	Freeze host's inventory item
	void FreezeHostItem(int iIvtr, int iIndex, bool bFreeze);
	//	Add one player's id to a buffer in order to get his name later
	void AddChatPlayerID(int id);

	//	Protocol handlers
	void OnPrtcChallenge(GNET::Protocol* pProtocol);
	void OnPrtcKeyExchange(GNET::Protocol* pProtocol);
	void OnPrtcOnlineAnnounce(GNET::Protocol* pProtocol);
	void OnPrtcWaitQueueNotify(GNET::Protocol* pProtocol);
	void OnPrtcCancelWaitQueueRe(GNET::Protocol* pProtocol);
	void OnPrtcRoleListRe(GNET::Protocol* pProtocol);
	void OnPrtcResetPosition(GNET::Protocol *pProtocol);
	void OnPrtcSelectRoleRe(GNET::Protocol* pProtocol);
	void OnPrtcCreateRoleRe(GNET::Protocol* pProtocol);
	void OnPrtcDeleteRoleRe(GNET::Protocol* pProtocol);
	void OnPtrcUndoDeleteRoleRe(GNET::Protocol* pProtocol);
	bool OnPrtcChatMessage(GNET::Protocol* pProtocol, bool bCalledagain);
	bool OnPrtcWorldChat(GNET::Protocol* pProtocol, bool bCalledagain);
	void OnPrtcErrorInfo(GNET::Protocol* pProtocol);
	void OnPrtcPlayerLogout(GNET::Protocol* pProtocol);
	void OnPrtcPrivateChat(GNET::Protocol* pProtocol);
	void OnPrtcPlayerBaseInfoRe(GNET::Protocol* pProtocol);
	void OnPrtcGetConfigRe(GNET::Protocol* pProtocol);
	void OnPrtcSetConfigRe(GNET::Protocol* pProtocol);
	void OnPrtcGetHelpStatesRe(GNET::Protocol* pProtocol);
	void OnPrtcSetHelpStatesRe(GNET::Protocol* pProtocol);
	void OnPrtcSetCustomDataRe(GNET::Protocol* pProtocol);
	void OnPrtcGetCustomDataRe(GNET::Protocol* pProtocol);
	void OnPrtcGetPlayerBriefInfoRe(GNET::Protocol* pProtocol);
	void OnPrtcGetPlayerIDByNameRe(GNET::Protocol* pProtocol);
	void OnPrtcRoleStatusAnnounce(GNET::Protocol* pProtocol);
	void OnPrtcAnnounceForbidInfo(GNET::Protocol* pProtocol);
	void OnPrtcUpdateRemainTime(GNET::Protocol* pProtocol);
	void OnPrtcSubmitProblemToGMRe(GNET::Protocol* pProtocol);
	void OnPrtcImpeachPlayerToGMRe(GNET::Protocol* pProtocol);
	void OnPrtcMatrixChallenge(GNET::Protocol* pProtocol);
	void OnPrtcCashOpt(GNET::Protocol* pProtocol);
	void OnPrtcACQuestion(GNET::Protocol* pProtocol);
	void OnPrtcAutoLockSetRe(GNET::Protocol* pProtocol);
	
	void OnPrtcRefGetReferenceCodeRe(GNET::Protocol* pProtocol);
	void OnPrtcRefListReferralsRe(GNET::Protocol* pProtocol);
	void OnPrtcRefWithdrawBonusRe(GNET::Protocol* pProtocol);

	void OnPrtcGetRewardListRe(GNET::Protocol* pProtocol);
	void OnPrtcExchangeConsumePointsRe(GNET::Protocol* pProtocol);
	void OnPrtcRewardMatrueNotice(GNET::Protocol* pProtocol);
	
	void OnPrtcAccountLoginRecord(GNET::Protocol *pProtocol);

	void OnTradeProtocols(GNET::Protocol* pProtocol);
	void OnUserCouponProtocols(GNET::Protocol* pProtocol);
	void OnWebTradeProtocols(GNET::Protocol* pProtocol);
	void OnSysAuctionProtocols(GNET::Protocol* pProtocol);
	void OnStockProtocols(GNET::Protocol* pProtocol);
	void OnFriendProtocols(GNET::Protocol* pProtocol);
	void OnFactionProtocols(GNET::Protocol* pProtocol);
	bool OnFactionDiplomacyProtocols(GNET::Protocol* pProtocol, bool bFirst);
	void OnChatRoomProtocols(GNET::Protocol* pProtocol);
	void OnMailProtocols(GNET::Protocol* pProtocol);
	void OnVendueProtocols(GNET::Protocol* pProtocol);
	void OnBattleProtocols(GNET::Protocol* pProtocol);
	void OnAccountProtocols(GNET::Protocol* pProtocol);
	void OnFactionFortressProtocols(GNET::Protocol* pProtocol);

	void OnPrtcGMKickOutRoleRe(GNET::Protocol* pProtocol);
	void OnPrtcGMKickOutUserRe(GNET::Protocol* pProtocol);
	void OnPrtcGMListOnlineUserRe(GNET::Protocol* pProtocol);
	void OnPrtcGMOnlineNumberRe(GNET::Protocol* pProtocol);
	void OnPrtcGMRestartServerRe(GNET::Protocol* pProtocol);
	void OnPrtcGMRestartServer(GNET::Protocol* pProtocol);
	void OnPrtcGMShutupRoleRe(GNET::Protocol* pProtocol);
	void OnPrtcGMShutupUserRe(GNET::Protocol* pProtocol);
	void OnPrtcGMForbidRoleRe(GNET::Protocol* pProtocol);
	void OnPrtcGMToggleChatRe(GNET::Protocol* pProtocol);

	void OnPrtcGMGetConsumeInfoRe(GNET::Protocol* pProtocol);
	void OnPrtcGMPermanentLockRe(GNET::Protocol* pProtocol);

	void OnPrtcPlayerRenameRe(GNET::Protocol* pProtocol);
	void OnPrtcPlayerNameUpdate(GNET::Protocol* pProtocol);

	void OnPrtcKingCommandPoint(GNET::Protocol* pProtocol);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


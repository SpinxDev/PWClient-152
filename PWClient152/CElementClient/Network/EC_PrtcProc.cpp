/*
 * FILE: EC_GameDataPrtc.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include <AFile.h>

#include "EC_Global.h"
#include "EC_GameSession.h"
#include "EC_Game.h"
#include "EC_GPDataType.h"
#include "EC_World.h"
#include "EC_GameRun.h"
#include "EC_RTDebug.h"
#include "EC_FixedMsg.h"
#include "EC_IvtrTypes.h"
#include "EC_DealInventory.h"
#include "EC_IvtrItem.h"
#include "EC_HostPlayer.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_LoginUIMan.h"
#include "DlgGoldTrade.h"
#include "DlgFortressWar.h"
#include "DlgFortressWarList.h"
#include "DlgGuildMap.h"
#include "DlgGMConsoleForbid.h"
#include "globaldataman.h"

#include "gnet.h"
#include "gameclient.h"

#include "gmkickoutrole.hpp"
#include "gmkickoutrole_re.hpp"
#include "gmkickoutuser.hpp"
#include "gmkickoutuser_re.hpp"
#include "gmlistonlineuser.hpp"
#include "gmlistonlineuser_re.hpp"
#include "gmonlinenum.hpp"
#include "gmonlinenum_re.hpp"
#include "gmrestartserver.hpp"
#include "gmrestartserver_re.hpp"
#include "gmshutup.hpp"
#include "gmshutup_re.hpp"
#include "gmshutuprole.hpp"
#include "gmshutuprole_re.hpp"
#include "gmforbidrole.hpp"
#include "gmforbidrole_re.hpp"
#include "gmtogglechat.hpp"
#include "gmtogglechat_re.hpp"
#include "factionchat.hpp"
#include "factionacceptjoin.hpp"
#include "factionoprequest.hpp"
#include "getplayerfactioninfo.hpp"
#include "factiondata.hxx"
#include "EC_Faction.h"
#include "ids.hxx"
#include "factioninviteres"
#include "autolockset.hpp"
#include "gmsettimelessautolock.hpp"
#include "gmsettimelessautolock_re.hpp"

#include "chatroomcreate.hpp"
#include "chatroomexpel.hpp"
#include "chatroominvite.hpp"
#include "chatroominvite_re.hpp"
#include "chatroomjoin.hpp"
#include "chatroomleave.hpp"
#include "chatroomlist.hpp"
#include "chatroomspeak.hpp"

#include "checknewmail.hpp"
#include "getmail.hpp"
#include "getmaillist.hpp"
#include "playersendmail.hpp"
#include "getmailattachobj.hpp"
#include "preservemail.hpp"
#include "deletemail.hpp"

#include "auctionattendlist.hpp"
#include "auctionbid.hpp"
#include "auctionclose.hpp"
#include "auctionget.hpp"
#include "auctionlist.hpp"
#include "auctionopen.hpp"
#include "auctionexitbid.hpp"
#include "auctiongetitem.hpp"
#include "auctionlistupdate.hpp"

#include "stockaccount.hpp"
#include "stockbill.hpp"
#include "stockcommission.hpp"
#include "stockcancel.hpp"

#include "battlechallenge.hpp"
#include "battlechallengemap.hpp"
#include "battlegetmap.hpp"
#include "battleenter.hpp"
#include "battlestatus.hpp"

#include "getselllist.hpp"
#include "findsellpointinfo.hpp"
#include "sellpoint.hpp"
#include "sellcancel.hpp"
#include "buypoint.hpp"
#include "matrixresponse.hpp"
#include "cashlock.hpp"
#include "cashpasswordset.hpp"

#include "stockaccount_re.hpp"
#include "stockbill_re.hpp"
#include "stockcancel_re.hpp"
#include "stockcommission_re.hpp"
#include "stocktransaction_re.hpp"

#include "refgetreferencecode.hpp"
#include "refgetreferencecode_re.hpp"
#include "reflistreferrals.hpp"
#include "reflistreferrals_re.hpp"
#include "refwithdrawbonus.hpp"
#include "refwithdrawbonus_re.hpp"

#include "getrewardlist.hpp"
#include "getrewardlist_re.hpp"
#include "exchangeconsumepoints.hpp"
#include "exchangeconsumepoints_re.hpp"

#include "usercoupon.hpp"
#include "usercouponexchange.hpp"

#include "webtradelist.hpp"
#include "webtradeattendlist.hpp"
#include "webtradegetitem.hpp"
#include "webtradegetdetail.hpp"
#include "webtradeprepost.hpp"
#include "webtraderoleprepost.hpp"
#include "webtradeprecancelpost.hpp"
#include "webtraderoleprecancelpost.hpp"
#include "webtradeupdate.hpp"
#include "webtraderolegetdetail.hpp"

#include "webtradelist_re.hpp"
#include "webtradeattendlist_re.hpp"
#include "webtradegetitem_re.hpp"
#include "webtradegetdetail_re.hpp"
#include "webtradeprepost_re.hpp"
#include "webtradeprecancelpost_re.hpp"
#include "webtradeupdate_re.hpp"

#include "sysauctionaccount.hpp"
#include "sysauctionbid.hpp"
#include "sysauctioncashtransfer.hpp"
#include "sysauctiongetitem.hpp"
#include "sysauctionlist.hpp"

#include "sysauctionaccount_re.hpp"
#include "sysauctioncashtransfer_re.hpp"
#include "sysauctiongetitem_re.hpp"
#include "sysauctionlist_re.hpp"
#include "sysauctionbid_re.hpp"

#include "createfactionfortress.hpp"
#include "createfactionfortress_re.hpp"
#include "factionfortressbattlelist.hpp"
#include "factionfortresschallenge.hpp"
#include "factionfortressenter.hpp"
#include "factionfortresslist.hpp"
#include "factionfortressget.hpp"

#include "factionallianceapply_re.hpp"
#include "factionalliancereply_re.hpp"
#include "factionhostileapply_re.hpp"
#include "factionhostilereply_re.hpp"
#include "factionremoverelationapply_re.hpp"
#include "factionremoverelationreply_re.hpp"
#include "factionrelationrecvapply.hpp"
#include "factionrelationrecvreply.hpp"
#include "DlgGuildDiplomacyApply.h"
#include "DlgGuildDiplomacyMan.h"
//#include "gmgetplayerconsumeinfo_re.hpp"

#include "chatdata.h"

#include "factionlistonline.hpp"
#include "useraddcash.hpp"
#include "ssogetticket.hpp"
#include "countrybattlemove.hpp"
#include "countrybattlegetmap.hpp"
#include "countrybattlegetplayerlocation.hpp"
#include "countrybattlegetscore.hpp"
#include "countrybattlegetconfig.hpp"
#include "countrybattlepreenter.hpp"
#include "countrybattlereturncapital.hpp"
#include "countrybattlegetbattlelimit.hpp"
#include "countrybattlegetkingcommandpoint.hpp"
#include "countrybattlekingassignassault.hpp"
#include "countrybattlekingresetbattlelimit.hpp"
#include "kecandidateapply.hpp"
#include "kevoting.hpp"
#include "kegetstatus.hpp"

#include "qpaddcash.hpp"
#include "qpgetactivatedservices.hpp"

#include "pshopactive.hpp"
#include "pshopbuy.hpp"
#include "pshopcancelgoods.hpp"
#include "pshopcleargoods.hpp"
#include "pshopcreate.hpp"
#include "pshopdrawitem.hpp"
#include "pshoplist.hpp"
#include "pshoplistitem.hpp"
#include "pshopmanagefund.hpp"
#include "pshopplayerbuy.hpp"
#include "pshopplayerget.hpp"
#include "pshopplayersell.hpp"
#include "pshopselfget.hpp"
#include "pshopsell.hpp"
#include "pshopsettype.hpp"

#include "playerprofilegetprofiledata.hpp"
#include "playerprofilegetmatchresult.hpp"
#include "playerprofilesetprofiledata.hpp"

#include "tankbattleplayergetrank.hpp"
#include "getcnetserverconfig.hpp"
#include "factionresourcebattlegetmap.hpp"
#include "factionresourcebattlegetrecord.hpp"
#include "collectclientmachineinfo.hpp"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	GM Protocols
//	
///////////////////////////////////////////////////////////////////////////

void CECGameSession::gm_KickOutRole(int idPlayer, int iTime, const ACHAR* szReason)
{
	GNET::GMKickoutRole p;
	p.gmroleid = m_iCharID;
	p.kickroleid = idPlayer;
	p.forbid_time = iTime;

	if (szReason && szReason[0])
	{
		int iLen = a_strlen(szReason) * sizeof (ACHAR);
		p.reason.replace(szReason, iLen);
	}

	SendNetData(p);
}

void CECGameSession::gm_KickOutUser(int idPlayer, int iTime, const ACHAR* szReason)
{
	GNET::GMKickoutUser p;
	p.gmroleid = m_iCharID;
	//	p.kickuserid = idPlayer & 0xfffffff0;	// user id 和 role id 的关系已改变
	p.kickuserid = idPlayer;	//	修改为发送 role id ，由服务器计算 user id
	p.forbid_time = iTime;

	if (szReason && szReason[0])
	{
		int iLen = a_strlen(szReason) * sizeof (ACHAR);
		p.reason.replace(szReason, iLen);
	}

	SendNetData(p);
}

void CECGameSession::gm_ListOnlineUser(int iHandler)
{
	GNET::GMListOnlineUser p;
	p.gmroleid = m_iCharID;
	p.handler = iHandler;
	SendNetData(p);
}

void CECGameSession::gm_OnlineNumber()
{
	GNET::GMOnlineNum p;
	p.gmroleid = m_iCharID;
	SendNetData(p);
}

void CECGameSession::gm_RestartServer(int idServer, int iRestartTime)
{
	GNET::GMRestartServer p;
	p.gmroleid = m_iCharID;
	p.gsid = idServer;
	p.restart_time = iRestartTime;
	SendNetData(p);
}

void CECGameSession::gm_ShutupRole(int idPlayer, int iTime, const ACHAR* szReason)
{
	GNET::GMShutupRole p;
	p.gmroleid = m_iCharID;
	p.dstroleid = idPlayer;
	p.forbid_time = iTime;

	if (szReason && szReason[0])
	{
		int iLen = a_strlen(szReason) * sizeof (ACHAR);
		p.reason.replace(szReason, iLen);
	}

	SendNetData(p);
}

void CECGameSession::gm_ShutupUser(int idPlayer, int iTime, const ACHAR* szReason)
{
	GNET::GMShutup p;
	p.gmroleid = m_iCharID;
	// p.dstuserid = idPlayer & 0xfffffff0;	// user id 和 role id 的关系已改变
	p.dstuserid = idPlayer;			//	修改为发送 role id ，由服务器计算 user id
	p.forbid_time = iTime;

	if (szReason && szReason[0])
	{
		int iLen = a_strlen(szReason) * sizeof (ACHAR);
		p.reason.replace(szReason, iLen);
	}

	SendNetData(p);
}

void CECGameSession::gm_ForbidRole(int iType, int idPlayer, int iTime, const ACHAR* szReason)
{
	GNET::GMForbidRole p;
	p.fbd_type = (BYTE)iType;
	p.gmroleid = m_iCharID;
	p.dstroleid = idPlayer;
	p.forbid_time = iTime;

	if (szReason && szReason[0])
	{
		int iLen = a_strlen(szReason) * sizeof (ACHAR);
		p.reason.replace(szReason, iLen);
	}

	SendNetData(p);
}

void CECGameSession::gm_TriggerChat(bool bEnable)
{
	GNET::GMToggleChat p;
	p.gmroleid = m_iCharID;
	p.enable = bEnable ? 1 : 0;
	SendNetData(p);
}
void CECGameSession::gm_PermanentLock(int idPlayer, bool bEnable)
{
	GNET::GMSetTimelessAutoLock p;
	p.gmroleid = m_iCharID;
	p.dstroleid = idPlayer;
	p.enable = bEnable ? 1:0;
	
	SendNetData(p);
}

void CECGameSession::OnPrtcGMKickOutRoleRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GMKickoutRole_Re* p = (GMKickoutRole_Re*)pProtocol;
	if (p->retcode == ERR_SUCCESS)
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_KICKOUTROLE, p->kickroleid);
}

void CECGameSession::OnPrtcGMKickOutUserRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GMKickoutUser_Re* p = (GMKickoutUser_Re*)pProtocol;
	if (p->retcode == ERR_SUCCESS)
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_KICKOUTUSER, p->kickuserid);
}

void CECGameSession::OnPrtcGMListOnlineUserRe(GNET::Protocol* pProtocol)
{
}

void CECGameSession::OnPrtcGMOnlineNumberRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GMOnlineNum_Re* p = (GMOnlineNum_Re*)pProtocol;
	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ONLINENUMBER, p->local_num, p->total_num);
}

void CECGameSession::OnPrtcGMRestartServerRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GMRestartServer_Re* p = (GMRestartServer_Re*)pProtocol;
	if (p->retcode == ERR_SUCCESS)
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_RESTARTSEV);
}

void CECGameSession::OnPrtcGMRestartServer(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GMRestartServer* p = (GMRestartServer*)pProtocol;
	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_SERVERCLOSE, p->restart_time);
}

void CECGameSession::OnPrtcGMShutupRoleRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GMShutupRole_Re* p = (GMShutupRole_Re*)pProtocol;
	if (p->retcode == ERR_SUCCESS)
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_SHUTUPROLE, p->dstroleid, p->forbid_time);
}

void CECGameSession::OnPrtcGMShutupUserRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GMShutup_Re* p = (GMShutup_Re*)pProtocol;
	if (p->retcode == ERR_SUCCESS)
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_SHUTUPUSER, p->dstuserid, p->forbid_time);
}

void CECGameSession::OnPrtcGMForbidRoleRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GMForbidRole_Re* p = (GMForbidRole_Re*)pProtocol;
	if (p->retcode == ERR_SUCCESS)
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_OPTSUCCESS);
}

void CECGameSession::OnPrtcGMToggleChatRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GMToggleChat_Re* p = (GMToggleChat_Re*)pProtocol;
	if (p->retcode == ERR_SUCCESS)
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_OPTSUCCESS);
}
void CECGameSession::OnPrtcGMGetConsumeInfoRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	CECGameUIMan* pMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pMan)
	{
		CDlgGMConsoleForbid *pDlg = (CDlgGMConsoleForbid *)pMan->GetDialog("Win_GMConsole2");
		if (pDlg)
		{
			pDlg->OnPrtcGMGetConsumeInfo_Re((GMGetPlayerConsumeInfo_Re*)pProtocol);
		}
	}
}
void CECGameSession::OnPrtcGMPermanentLockRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GMSetTimelessAutoLock_Re* p = (GMSetTimelessAutoLock_Re*)pProtocol;
	if (p->retcode == ERR_SUCCESS)
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_OPTSUCCESS);
}
///////////////////////////////////////////////////////////////////////////
//	
//	Trade Protocols
//	
///////////////////////////////////////////////////////////////////////////

bool CECGameSession::trade_Start(int idTarget)
{
	GNET::TradeStart p;
	p.roleid = m_iCharID;
	p.partner_roleid = idTarget;

	SendNetData(p);
	return true;
}

bool CECGameSession::trade_StartResponse(DWORD& dwHandle, bool bAgree)
{
	using namespace GNET;

	if (!dwHandle)
	{
		ASSERT(0);
		return false;
	}

	Rpc* p = (Rpc*)dwHandle;
	TradeStartRqstRes* pResult = (TradeStartRqstRes*)p->GetResult();
	if (pResult)
	{
		pResult->retcode = bAgree ? ERR_TRADE_AGREE : ERR_TRADE_REFUSE;
		SendNetData(p);
	}

	p->Destroy();
	dwHandle = 0;

	return true;
}

bool CECGameSession::trade_AddGoods(int idTrade, int idItem, int iPos, int iCount, int iMoney)
{
	GNET::TradeAddGoods p;
	p.tid = idTrade;
	p.roleid = m_iCharID;
	p.money = iMoney;
	p.goods.id = idItem;
	p.goods.pos = iPos;
	p.goods.count = iCount;

	SendNetData(p);
	return true;
}

bool CECGameSession::trade_RemoveGoods(int idTrade, int idItem, int iPos, int iCount, int iMoney)
{
	GNET::TradeRemoveGoods p;
	p.tid = idTrade;
	p.roleid = m_iCharID;
	p.money = iMoney;
	p.goods.id = idItem;
	p.goods.pos = iPos;
	p.goods.count = iCount;

	SendNetData(p);
	return true;
}

bool CECGameSession::trade_MoveItem(int idTrade, int idItem, int iSrc, int iDst, int iCount)
{
	FreezeHostItem(IVTRTYPE_PACK, iSrc, true);
	FreezeHostItem(IVTRTYPE_PACK, iDst, true);

	GNET::TradeMoveObj p;
	p.roleid = m_iCharID;
	p.tid = idTrade;
	p.dst_pos = iDst;
	p.goods.id = idItem;
	p.goods.pos = iSrc;
	p.goods.count = iCount;

	SendNetData(p);
	return true;
}

bool CECGameSession::trade_Submit(int idTrade)
{
	GNET::TradeSubmit p;
	p.tid = idTrade;
	p.roleid = m_iCharID;

	SendNetData(p);
	return true;
}

bool CECGameSession::trade_Confirm(int idTrade)
{
	//	Freeze items
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CECDealInventory* pDealPack = pHost->GetDealPack();
	for (int i=0; i < pDealPack->GetSize(); i++)
	{
		CECIvtrItem* pItem = pDealPack->GetItem(i);
		if (pItem)
			pItem->NetFreeze(true);
	}

	GNET::TradeConfirm p;
	p.tid = idTrade;
	p.roleid = m_iCharID;

	SendNetData(p);
	return true;
}

bool CECGameSession::trade_Cancel(int idTrade)
{
	GNET::TradeDiscard p;
	p.tid = idTrade;
	p.roleid = m_iCharID;

	SendNetData(p);
	return true;
}

void CECGameSession::OnTradeProtocols(GNET::Protocol* pProtocol)
{
	ASSERT(pProtocol);

	int iMsg;

	switch (pProtocol->GetType())
	{
	case PROTOCOL_TRADESTART_RE:		iMsg = MSG_HST_TRADESTART;		break;
	case RPC_TRADESTARTRQST:			iMsg = MSG_HST_TRADEREQUEST;	break;
	case PROTOCOL_TRADEMOVEOBJ_RE:		iMsg = MSG_HST_TRADEMOVEITEM;	break;
	case PROTOCOL_TRADEADDGOODS_RE:		iMsg = MSG_HST_TRADEADDGOODS;	break;
	case PROTOCOL_TRADEREMOVEGOODS_RE:	iMsg = MSG_HST_TRADEREMGOODS;	break;
	case PROTOCOL_TRADEDISCARD_RE:		iMsg = MSG_HST_TRADECANCEL;		break;
	case PROTOCOL_TRADESUBMIT_RE:		iMsg = MSG_HST_TRADESUBMIT;		break;
	case PROTOCOL_TRADECONFIRM_RE:		iMsg = MSG_HST_TARDECONFIRM;	break;
	case PROTOCOL_TRADEEND:				iMsg = MSG_HST_TRADEEND;		break;

	default:
		ASSERT(0);
		return;
	}

	//	When player return login interface from game, pWorld may be NULL.
	g_pGame->GetGameRun()->PostMessage(iMsg, MAN_PLAYER, 0, (DWORD)pProtocol);
}

///////////////////////////////////////////////////////////////////////////
//	
//	UserCoupon Protocols
//	
///////////////////////////////////////////////////////////////////////////
void CECGameSession::userCoupon_Account()
{
	GNET::UserCoupon p;
	p.userid = GetUserID();

	SendNetData(p);
}

void CECGameSession::userCoupon_Exchange(int number)
{
	ASSERT(number > 0);
	if(number <= 0)
	{
		return;
	}

	GNET::UserCouponExchange p;
	p.userid = GetUserID();
	p.coupon_number = number;

	SendNetData(p);
}

void CECGameSession::OnUserCouponProtocols(GNET::Protocol* pProtocol)
{
	ASSERT(pProtocol);
	GNET::Protocol::Type type = pProtocol->GetType();
	g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->UserCouponAction(type, pProtocol);
}

void CECGameSession::OnWebTradeProtocols(GNET::Protocol* pProtocol)
{
	ASSERT(pProtocol);	
	
	int iMsg;

	int retcode = ERROR_SUCCESS;
	ACString errMsg;

	GNET::Protocol::Type type = pProtocol->GetType();
	
	switch (type)
	{
	case PROTOCOL_WEBTRADELIST_RE:
		iMsg = MSG_HST_WEBTRADELIST;
		break;

	case PROTOCOL_WEBTRADEATTENDLIST_RE:
		iMsg = MSG_HST_WEBTRADEATTENDLIST;
		break;

	case PROTOCOL_WEBTRADEGETITEM_RE:
		iMsg = MSG_HST_WEBTRADEGETITEM;
		break;

	case PROTOCOL_WEBTRADEGETDETAIL_RE:
		iMsg = MSG_HST_WEBTRADEGETDETAIL;
		retcode = ((GNET::WebTradeGetDetail_Re*)pProtocol)->retcode;
		break;

	case PROTOCOL_WEBTRADEPREPOST_RE:
		iMsg = MSG_HST_WEBTRADEPREPOST;
		retcode = ((GNET::WebTradePrePost_Re*)pProtocol)->retcode;
		break;

	case PROTOCOL_WEBTRADEPRECANCELPOST_RE:
		iMsg = MSG_HST_WEBTRADEPRECANCELPOST;
		retcode = ((GNET::WebTradePreCancelPost_Re*)pProtocol)->retcode;
		break;
		
	case PROTOCOL_WEBTRADEUPDATE_RE:
		iMsg = MSG_HST_WEBTRADEUPDATE;
		// NOTICE: this message need further process
		// retcode = ((GNET::WebTradeUpdate_Re*)pProtocol)->retcode;
		break;
		
	default:
		ASSERT(0);
		return;
	}

	if ( g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_LOGIN )
	{
		CECLoginUIMan* pUIMan = g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan();
		if ( retcode != ERR_SUCCESS )
		{
			GetServerError(retcode, errMsg);
			pUIMan->MessageBox("", errMsg, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		pUIMan->WebTradeAction(type, pProtocol);
	}
	else
	{
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if ( retcode != ERR_SUCCESS )
		{
			GetServerError(retcode, errMsg);
			pGameUI->AddChatMessage(errMsg, GP_CHAT_SYSTEM);
			return;
		}
		//	When player return login interface from game, pWorld may be NULL.
		g_pGame->GetGameRun()->PostMessage(iMsg, MAN_PLAYER, 0, (DWORD)pProtocol);
	}
}

void CECGameSession::OnFactionFortressProtocols(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	GNET::Protocol::Type type = pProtocol->GetType();

	switch(type)
	{
	case PROTOCOL_CREATEFACTIONFORTRESS_RE:
		{
			CreateFactionFortress_Re* p = (CreateFactionFortress_Re *)pProtocol;
			if (p->retcode != ERROR_SUCCESS)
			{
				OutputLinkSevError(p->retcode);
				break;
			}
			
			CECGameUIMan * pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			if (pGameUIMan)
				pGameUIMan->MessageBox("", pGameUIMan->GetStringFromTable(9150), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));

			CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
			pHost->SetFactionFortressBriefInfo(&p->brief);
			break;
		}
		
	case PROTOCOL_FACTIONFORTRESSLIST_RE:
		{
			FactionFortressList_Re * p = (FactionFortressList_Re *)pProtocol;
			CECGameUIMan * pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			if (pGameUIMan)
			{
				CDlgFortressWar *pDlg = (CDlgFortressWar *)pGameUIMan->GetDialog("Win_FortressWar");
				pDlg->OnPrtcFactionFortressList_Re(p);
			}
			break;
		}

	case PROTOCOL_FACTIONFORTRESSCHALLENGE_RE:
		{
			FactionFortressChallenge_Re * p = (FactionFortressChallenge_Re *)pProtocol;
			CECGameUIMan * pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			if (pGameUIMan)
			{
				CDlgFortressWar *pDlg = (CDlgFortressWar *)pGameUIMan->GetDialog("Win_FortressWar");
				pDlg->OnPrtcFactionFortressChallenge_Re(p);
			}
			break;
		}


	case PROTOCOL_FACTIONFORTRESSBATTLELIST_RE:
		{
			FactionFortressBattleList_Re * p = (FactionFortressBattleList_Re *)pProtocol;
			CECGameUIMan * pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			if (pGameUIMan)
			{
				CDlgFortressWarList *pDlg = (CDlgFortressWarList *)pGameUIMan->GetDialog("Win_FortressWarList");
				pDlg->OnPrtcFactionFortressBattleList_Re(p);
			}
			break;
		}


	case PROTOCOL_FACTIONFORTRESSGET_RE:
		{
			FactionFortressGet_Re * p = (FactionFortressGet_Re *)pProtocol;
			CECGameUIMan * pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			if (pGameUIMan)
			{
				CDlgFortressWar *pDlg = (CDlgFortressWar *)pGameUIMan->GetDialog("Win_FortressWar");
				pDlg->OnPrtcFactionFortressGet_Re(p);
			}
			break;
		}
	}
}

void CECGameSession::OnSysAuctionProtocols(GNET::Protocol* pProtocol)
{
	ASSERT(pProtocol);
	
	int iMsg;	
	int retcode(0);	
	GNET::Protocol::Type type = pProtocol->GetType();

	switch (type)
	{
	case PROTOCOL_SYSAUCTIONACCOUNT_RE:
		iMsg = MSG_HST_SYSAUCTIONACCOUNT;
		retcode = ((GNET::SysAuctionAccount_Re*)pProtocol)->retcode;
		break;

	case PROTOCOL_SYSAUCTIONBID_RE:
		iMsg = MSG_HST_SYSAUCTIONBID;
		retcode = ((GNET::SysAuctionBid_Re*)pProtocol)->retcode;
		break;

	case PROTOCOL_SYSAUCTIONCASHTRANSFER_RE:
		iMsg = MSG_HST_SYSAUCTIONCASHTRANSFER;
		retcode = ((GNET::SysAuctionCashTransfer_Re*)pProtocol)->retcode;
		break;
		
	case PROTOCOL_SYSAUCTIONGETITEM_RE:
		iMsg = MSG_HST_SYSAUCTIONGETITEM;
		break;

	case PROTOCOL_SYSAUCTIONLIST_RE:
		iMsg = MSG_HST_SYSAUCTIONLIST;
		break;

	default:
		ASSERT(0);
		return;
	}

	ACString errMsg;
	if( retcode != ERR_SUCCESS )
	{
		GetServerError(retcode, errMsg);
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if(pGameUI) pGameUI->AddChatMessage(errMsg, GP_CHAT_SYSTEM);
	}

	g_pGame->GetGameRun()->PostMessage(iMsg, MAN_PLAYER, 0, (DWORD)pProtocol);
}

void CECGameSession::OnStockProtocols(GNET::Protocol* pProtocol)
{
	switch (pProtocol->GetType())
	{
	case PROTOCOL_STOCKACCOUNT_RE:
	{
		GNET::StockAccount_Re* p = static_cast<GNET::StockAccount_Re*>(pProtocol);
		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->
			GoldAction(CDlgGoldTrade::GOLD_ACTION_ACCOUNT_RE, p);
		break;
	}
	case PROTOCOL_STOCKBILL_RE:
	{
		GNET::StockBill_Re* p = static_cast<GNET::StockBill_Re*>(pProtocol);
		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->
			GoldAction(CDlgGoldTrade::GOLD_ACTION_BILL_RE, p);
		break;
	}
	case PROTOCOL_STOCKCOMMISSION_RE:
	{
		GNET::StockCommission_Re* p = static_cast<GNET::StockCommission_Re*>(pProtocol);
		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->
			GoldAction(CDlgGoldTrade::GOLD_ACTION_COMMISSION_RE, p);
		break;
	}
	case PROTOCOL_STOCKTRANSACTION_RE:
	{
		GNET::StockTransaction_Re* p = static_cast<GNET::StockTransaction_Re*>(pProtocol);
		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->
			GoldAction(CDlgGoldTrade::GOLD_ACTION_TRANSACTION_RE, p);
		break;
	}
	case PROTOCOL_STOCKCANCEL_RE:
	{
		GNET::StockCancel_Re* p = static_cast<GNET::StockCancel_Re*>(pProtocol);
		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->
			GoldAction(CDlgGoldTrade::GOLD_ACTION_CANCEL_RE, p);
		break;
	}
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Stock Protocols
//	
///////////////////////////////////////////////////////////////////////////

void CECGameSession::stock_account()
{
	GNET::StockAccount p(m_iCharID);
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevStockOperation(GNET::PROTOCOL_STOCKACCOUNT, o.begin(), o.size());
}

void CECGameSession::stock_bill()
{
	GNET::StockBill p(m_iCharID);
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevStockOperation(GNET::PROTOCOL_STOCKBILL, o.begin(), o.size());
}

void CECGameSession::stock_commission(char bBuy, int iPrice, int iVolume)
{
	GNET::StockCommission p(m_iCharID, bBuy, iPrice, iVolume);
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevStockOperation(GNET::PROTOCOL_STOCKCOMMISSION, o.begin(), o.size());
}

void CECGameSession::stock_cancel(int tid, int price)
{
	GNET::StockCancel p(m_iCharID, tid, price);
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevStockOperation(GNET::PROTOCOL_STOCKCANCEL, o.begin(), o.size());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Friend Protocols
//	
///////////////////////////////////////////////////////////////////////////

void CECGameSession::friend_Add(int idPlayer, const ACHAR* szName)
{
	GNET::AddFriend p;
	p.srcroleid = m_iCharID;
	p.dstroleid = idPlayer;

	if (szName)
	{
		int iLen = a_strlen(szName) * sizeof (ACHAR);
		p.dstname.replace(szName, iLen);
	}

	SendNetData(p);
}

void CECGameSession::friend_AddResponse(DWORD& dwHandle, bool bAgree)
{
	using namespace GNET;

	if (!dwHandle)
	{
		ASSERT(0);
		return;
	}

	Rpc* p = (Rpc*)dwHandle;
	AddFriendRqstRes* pResult = (AddFriendRqstRes*)p->GetResult();
	if (pResult)
	{
		pResult->retcode = bAgree ? ERR_TRADE_AGREE : ERR_TRADE_REFUSE;
		SendNetData(p);
	}
	
	p->Destroy();
	dwHandle = 0;
}

void CECGameSession::friend_GetList()
{
	GNET::GetFriends p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

void CECGameSession::friend_SetGroupName(int iGroup, const ACHAR* szName)
{
	GNET::SetGroupName p;
	p.roleid = m_iCharID;
	p.gid = (char)iGroup;

	if (szName)
	{
		int iLen = a_strlen(szName) * sizeof (ACHAR);
		p.name.replace(szName, iLen);
	}

	SendNetData(p);
}

void CECGameSession::friend_SetGroup(int iGroup, int idFriend)
{
	GNET::SetFriendGroup p;
	p.roleid = m_iCharID;
	p.gid = (char)iGroup;
	p.friendid = idFriend;

	SendNetData(p);
}

void CECGameSession::friend_Delete(int idFriend)
{
	GNET::DelFriend p;
	p.roleid = m_iCharID;
	p.friendid = idFriend;
	
	SendNetData(p);
}

void CECGameSession::friend_GetOfflineMsg()
{
	GNET::GetSavedMsg p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

void CECGameSession::friend_SendMail(int idFriend,int idTemplete)
{
	GNET::SendAUMail p;
	p.roleid = m_iCharID;
	p.friend_id = idFriend;
	p.mail_template_id = idTemplete;

	SendNetData(p);
}

void CECGameSession::friend_SetRemarks(int idFriend, const ACHAR* szRemarks)
{
	GNET::AddFriendRemarks p;
	p.roleid = m_iCharID;
	p.friendroleid = idFriend;
	if (szRemarks){
		int iLen = a_strlen(szRemarks) * sizeof (ACHAR);
		p.friendremarks.replace(szRemarks, iLen);
	}	
	SendNetData(p);
}

void CECGameSession::OnFriendProtocols(GNET::Protocol* pProtocol)
{
	g_pGame->GetGameRun()->PostMessage(MSG_HST_FRIENDOPT, MAN_PLAYER, 0, (DWORD)pProtocol, pProtocol->GetType());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Faction Protocols
//	
///////////////////////////////////////////////////////////////////////////

inline size_t a_str_size(const ACHAR* sz)
{
	return a_strlen(sz) * sizeof(ACHAR);
}

void CECGameSession::OnFactionProtocols(GNET::Protocol* pProtocol)
{
	g_pGame->GetGameRun()->PostMessage(MSG_HST_FACTION, MAN_PLAYER, 0, (DWORD)pProtocol, pProtocol->GetType());
}

bool CECGameSession::OnFactionDiplomacyProtocols(GNET::Protocol *pProtocol, bool bFirst)
{
	//	返回 true 表明协议已正确处理，可删除
	//	返回 false 表明协议不该由此函数处理，或处理因缺少信息而失败（需要后续再处理）
	//
	bool bOK(true);	

	ACString strMsg;

	switch(pProtocol->GetType())
	{
    case PROTOCOL_FACTIONALLIANCEAPPLY_RE:
		{
			//	申请同盟服务器返回
			GNET::FactionAllianceApply_Re *p = (GNET::FactionAllianceApply_Re *)pProtocol;
			if (p->retcode != ERR_SUCCESS)
			{
				g_pGame->GetGameSession()->OutputLinkSevError(p->retcode);
				break;
			}
			else
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (bFirst)
				{
					//	更新界面显示
					CDlgGuildDiplomacyMan *pDlg = (CDlgGuildDiplomacyMan *)pGameUI->GetDialog("Win_GuildDiplomacyMan");
					pDlg->Refresh();
				}

				//	帮派喊话
				const Faction_Info *pFInfo = g_pGame->GetFactionMan()->GetFaction(p->dst_fid, bFirst);
				if (!pFInfo)
				{
					bOK = false;
					break;
				}
				strMsg.Format(pGameUI->GetStringFromTable(9252), pFInfo->GetName());
			}
		}
		break;

    case PROTOCOL_FACTIONALLIANCEREPLY_RE:
		{
			//	回复同盟申请服务器返回
			GNET::FactionAllianceReply_Re *p = (GNET::FactionAllianceReply_Re *)pProtocol;
			if (p->retcode != ERR_SUCCESS)
			{
				g_pGame->GetGameSession()->OutputLinkSevError(p->retcode);
				break;
			}
			else
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

				if (bFirst)
				{
					//	更新界面显示
					CDlgGuildDiplomacyMan *pDlg = (CDlgGuildDiplomacyMan *)pGameUI->GetDialog("Win_GuildDiplomacyMan");
					pDlg->Refresh();
				}

				//	帮派喊话
				const Faction_Info *pFInfo = g_pGame->GetFactionMan()->GetFaction(p->dst_fid, bFirst);
				if (!pFInfo)
				{
					bOK = false;
					break;
				}
				if (p->agree)
				{
					//	同意与之同盟
					strMsg.Format(pGameUI->GetStringFromTable(9200), pFInfo->GetName());
				}
			}
		}
		break;

    case PROTOCOL_FACTIONHOSTILEAPPLY_RE:
		{
			//	申请敌对服务器返回
			GNET::FactionHostileApply_Re *p = (GNET::FactionHostileApply_Re *)pProtocol;
			if (p->retcode != ERR_SUCCESS)
			{
				g_pGame->GetGameSession()->OutputLinkSevError(p->retcode);
				break;
			}
			else
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

				if (bFirst)
				{
					//	更新界面显示
					CDlgGuildDiplomacyMan *pDlg = (CDlgGuildDiplomacyMan *)pGameUI->GetDialog("Win_GuildDiplomacyMan");
					pDlg->Refresh();
				}

				//	帮派喊话
				const Faction_Info *pFInfo = g_pGame->GetFactionMan()->GetFaction(p->dst_fid, bFirst);
				if (!pFInfo)
				{
					bOK = false;
					break;
				}
				strMsg.Format(pGameUI->GetStringFromTable(9253), pFInfo->GetName());
			}
		}
		break;

    case PROTOCOL_FACTIONHOSTILEREPLY_RE:
		{
			//	回复敌对申请服务器返回
			GNET::FactionHostileReply_Re *p = (GNET::FactionHostileReply_Re *)pProtocol;
			if (p->retcode != ERR_SUCCESS)
			{
				g_pGame->GetGameSession()->OutputLinkSevError(p->retcode);
				break;
			}
			else
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (bFirst)
				{
					//	更新界面显示
					CDlgGuildDiplomacyMan *pDlg = (CDlgGuildDiplomacyMan *)pGameUI->GetDialog("Win_GuildDiplomacyMan");
					pDlg->Refresh();
				}

				//	帮派喊话
				const Faction_Info *pFInfo = g_pGame->GetFactionMan()->GetFaction(p->dst_fid, bFirst);
				if (!pFInfo)
				{
					bOK = false;
					break;
				}
				if (p->agree)
				{
					//	同意与之敌对
					strMsg.Format(pGameUI->GetStringFromTable(9201), pFInfo->GetName());
				}
			}
		}
		break;

    case PROTOCOL_FACTIONREMOVERELATIONAPPLY_RE:
		{
			//	申请解除关系服务器返回
			GNET::FactionRemoveRelationApply_Re *p = (GNET::FactionRemoveRelationApply_Re *)pProtocol;
			if (p->retcode != ERR_SUCCESS)
			{
				g_pGame->GetGameSession()->OutputLinkSevError(p->retcode);
				break;
			}
			else
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

				if (bFirst)
				{
					//	更新界面显示
					CDlgGuildDiplomacyMan *pDlg = (CDlgGuildDiplomacyMan *)pGameUI->GetDialog("Win_GuildDiplomacyMan");
					pDlg->Refresh();
				}

				//	帮派喊话
				const Faction_Info *pFInfo = g_pGame->GetFactionMan()->GetFaction(p->dst_fid, bFirst);
				if (!pFInfo)
				{
					bOK = false;
					break;
				}
				strMsg.Format(pGameUI->GetStringFromTable(9254), pFInfo->GetName());
			}
		}
		break;

    case PROTOCOL_FACTIONREMOVERELATIONREPLY_RE:
		{
			//	回复解除关系申请服务器返回
			GNET::FactionRemoveRelationReply_Re *p = (GNET::FactionRemoveRelationReply_Re *)pProtocol;
			if (p->retcode != ERR_SUCCESS)
			{
				g_pGame->GetGameSession()->OutputLinkSevError(p->retcode);
				break;
			}
			else
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (bFirst)
				{
					//	更新界面显示
					CDlgGuildDiplomacyMan *pDlg = (CDlgGuildDiplomacyMan *)pGameUI->GetDialog("Win_GuildDiplomacyMan");
					pDlg->Refresh();
				}				

				//	帮派喊话
				const Faction_Info *pFInfo = g_pGame->GetFactionMan()->GetFaction(p->dst_fid, bFirst);
				if (!pFInfo)
				{
					bOK = false;
					break;
				}
				if (p->agree)
				{
					//	同意与之解除关系
					strMsg.Format(pGameUI->GetStringFromTable(9202), pFInfo->GetName());
				}
			}
		}
		break;

	case PROTOCOL_FACTIONLISTRELATION_RE:
		{
			//	获取当前帮派关系列表及相关申请记录
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			CDlgGuildDiplomacyMan *pDlg = (CDlgGuildDiplomacyMan *)pGameUI->GetDialog("Win_GuildDiplomacyMan");
			pDlg->OnPrtcFactionListRelation_Re((GNET::FactionListRelation_Re *)pProtocol);
			g_pGame->GetGameRun()->GetHostPlayer()->GetGuildDiplomacyReminder().OnPrtcFactionListRelation_Re((GNET::FactionListRelation_Re *)pProtocol);
		}
		break;

	case PROTOCOL_FACTIONLISTONLINE_RE:
		{
			//	获取当前在线帮派列表
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			CDlgGuildDiplomacyApply *pDlg = (CDlgGuildDiplomacyApply *)pGameUI->GetDialog("Win_GuildDiplomacyApply");
			pDlg->OnPrtcFactionListOnline_Re((GNET::FactionListOnline_Re *)pProtocol);
		}
		break;

	case PROTOCOL_FACTIONRELATIONRECVAPPLY:
		{
			//	收到帮派外交申请
			GNET::FactionRelationRecvApply *p = (GNET::FactionRelationRecvApply *)pProtocol;
			const Faction_Info *pFInfo = g_pGame->GetFactionMan()->GetFaction(p->src_fid, bFirst);
			if (!pFInfo)
			{
				bOK = false;
				break;
			}

			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			switch(p->apply_type)
			{
			case ALLIANCE_FROM_OTHER:
				strMsg.Format(pGameUI->GetStringFromTable(9249), pFInfo->GetName());
				break;

			case HOSTILE_FROM_OTHER:
				strMsg.Format(pGameUI->GetStringFromTable(9250), pFInfo->GetName());
				break;

			case REMOVE_RELATION_FROM_OTHER:
				strMsg.Format(pGameUI->GetStringFromTable(9251), pFInfo->GetName());
				break;

			default:
				ASSERT(false);
				break;
			}
		}
		break;

	case PROTOCOL_FACTIONRELATIONRECVREPLY:
		{
			//	收到帮派外交回复
			GNET::FactionRelationRecvReply *p = (GNET::FactionRelationRecvReply *)pProtocol;
			const Faction_Info *pFInfo = g_pGame->GetFactionMan()->GetFaction(p->src_fid, bFirst);
			if (!pFInfo)
			{
				bOK = false;
				break;
			}

			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			switch(p->pre_apply_type)
			{
			case ALLIANCE_TO_OTHER:
				if (!p->agree)
					strMsg.Format(pGameUI->GetStringFromTable(9255), pFInfo->GetName());
				break;

			case HOSTILE_TO_OTHER:
				if (!p->agree)
					strMsg.Format(pGameUI->GetStringFromTable(9256), pFInfo->GetName());
				break;
				
			case REMOVE_RELATION_TO_OTHER:
				if (!p->agree)
					strMsg.Format(pGameUI->GetStringFromTable(9257), pFInfo->GetName());
				break;

			default:
				ASSERT(false);
				break;
			}
		}
		break;

	default:
		//	其它协议不处理
		bOK = false;
		break;
	}

	if (!strMsg.IsEmpty())
		g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_FACTION);

	return bOK;
}

void CECGameSession::faction_chat(const ACHAR* szMsg, int iPack/* =-1 */, int iSlot/* =-1 */)
{
	GNET::FactionChat p(GNET::FactionChat(0, 0, m_iCharID, Octets(szMsg, a_str_size(szMsg))));
	
	if (iPack == IVTRTYPE_CLIENT_GENERALCARD_PACK)
	{
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		CECInventory* clientPack = pHost ? pHost->GetPack(IVTRTYPE_CLIENT_GENERALCARD_PACK):NULL;
		if (pHost && clientPack)
		{
			CECIvtrItem* pCard = clientPack->GetItem(iSlot);
			if (pCard)
			{
				using namespace CHAT_C2S;
				chat_generalcard_collection item;
				item.cmd_id = CHAT_GENERALCARD_COLLECTION;
				item.card_id = pCard->GetTemplateID();
				p.data.replace(&item, sizeof(item));
			}
		}
	}
	else if (iPack >= 0 && iSlot >= 0)
	{
		using namespace CHAT_C2S;
		chat_equip_item item;
		item.cmd_id = CHAT_EQUIP_ITEM;
		item.where = iPack;
		item.index = iSlot;
		p.data.replace(&item, sizeof(item));
	}

	SendNetData(p);
}

void CECGameSession::faction_create(const ACHAR* szName, const ACHAR* szProclaim)
{
	Octets o = GNET::create_param_ct(Octets(szName, a_str_size(szName)), Octets(szProclaim, a_str_size(szProclaim))).marshal();
	c2s_CmdNPCSevFaction(GNET::_O_FACTION_CREATE, o.begin(), o.size());
}

void CECGameSession::faction_dismiss()
{
	Octets o = GNET::dismiss_param_ct().marshal();
	c2s_CmdNPCSevFaction(GNET::_O_FACTION_DISMISS, o.begin(), o.size());
}

void CECGameSession::faction_upgrade()
{
	Octets o = GNET::upgrade_param_ct().marshal();
	c2s_CmdNPCSevFaction(GNET::_O_FACTION_UPGRADE, o.begin(), o.size());
}

void CECGameSession::faction_degrade()
{
	Octets o = GNET::degrade_param_ct().marshal();
	c2s_CmdNPCSevFaction(GNET::_O_FACTION_DEGRADE, o.begin(), o.size());
}

void CECGameSession::faction_change_proclaim(const ACHAR* szNew)
{
	SendNetData(
		GNET::FactionOPRequest(
			GNET::_O_FACTION_CHANGEPROCLAIM,
			m_iCharID,
			GNET::changeproclaim_param_ct(Octets(szNew, a_str_size(szNew))).marshal()));
}

void CECGameSession::faction_accept_join(int iApplicant)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost->GetFactionID() == 0) return;
	SendNetData(GNET::FactionAcceptJoin(m_iCharID, 0, pHost->GetFactionID(), iApplicant));
}

void CECGameSession::faction_expel_member(int iMember)
{
	SendNetData(
		GNET::FactionOPRequest(
			GNET::_O_FACTION_EXPELMEMBER,
			m_iCharID,
			GNET::expelmember_param_ct(iMember).marshal()));
}

void CECGameSession::faction_cancel_expel_member(int iMember)
{
	SendNetData(
		GNET::FactionOPRequest(
		GNET::_O_FACTION_CANCELEXPELSCHEDULE,
		m_iCharID,
		GNET::cancelexpelschedule_param_ct(iMember).marshal()));
}

void CECGameSession::faction_appoint(int iMember, char new_position)
{
	SendNetData(
		GNET::FactionOPRequest(
			GNET::_O_FACTION_APPOINT,
			m_iCharID,
			GNET::appoint_param_ct(iMember, new_position).marshal()));
}

void CECGameSession::faction_master_resign(int iNewMaster)
{
	SendNetData(
		GNET::FactionOPRequest(
			GNET::_O_FACTION_MASTERRESIGN,
			m_iCharID,
			GNET::masterresign_param_ct(iNewMaster).marshal()));
}

void CECGameSession::faction_resign()
{
	SendNetData(
		GNET::FactionOPRequest(
			GNET::_O_FACTION_RESIGN,
			m_iCharID,
			GNET::resign_param_ct().marshal()));
}

void CECGameSession::faction_leave()
{
	SendNetData(
		GNET::FactionOPRequest(
			GNET::_O_FACTION_LEAVE,
			m_iCharID,
			GNET::leave_param_ct().marshal()));
}

void CECGameSession::faction_broadcast(const ACHAR* szMsg)
{
	SendNetData(
		GNET::FactionOPRequest(
			GNET::_O_FACTION_BROADCAST,
			m_iCharID,
			GNET::broadcast_param_ct(Octets(szMsg, a_str_size(szMsg))).marshal()));
}

void CECGameSession::faction_listmember(int version)
{
	SendNetData(
		GNET::FactionOPRequest(
			GNET::_O_FACTION_LISTMEMBER,
			m_iCharID,
			GNET::listmember_param_ct(version).marshal()));
}

void CECGameSession::faction_rename(int iRole, const ACHAR* szNewName)
{
	SendNetData(
		GNET::FactionOPRequest(
			GNET::_O_FACTION_RENAME,
			m_iCharID,
			GNET::rename_param_ct(iRole, Octets(szNewName, a_str_size(szNewName))).marshal()));
}

void CECGameSession::faction_invite_response(DWORD& dwHandle, bool bAgree)
{
	using namespace GNET;
	assert(dwHandle);
	if (dwHandle == 0) return;

	Rpc* p = reinterpret_cast<Rpc*>(dwHandle);
	FactionInviteRes* pResult = static_cast<FactionInviteRes*>(p->GetResult());
	if (pResult)
	{
		pResult->retcode = bAgree ? ERR_SUCCESS : ERR_FC_JOIN_REFUSE;
		SendNetData(p);
	}

	p->Destroy();
	dwHandle = 0;
}

void CECGameSession::faction_player_info()
{
	SendNetData(GNET::GetPlayerFactionInfo(m_iCharID, 0));
}

void CECGameSession::faction_get_onlinelist()
{
	SendNetData(GNET::FactionListOnline(m_iCharID, 0));
}

void CECGameSession::faction_relation_apply(int idTarget, bool bAlliance)
{
	if (bAlliance)
	{
		SendNetData(
		GNET::FactionOPRequest(
		GNET::_O_FACTION_ALLIANCEAPPLY,
		m_iCharID,
		GNET::allianceapply_param_ct(idTarget).marshal()));
	}
	else
	{
		SendNetData(
		GNET::FactionOPRequest(
		GNET::_O_FACTION_HOSTILEAPPLY,
		m_iCharID,
		GNET::hostileapply_param_ct(idTarget).marshal()));
	}
}

void CECGameSession::faction_relation_reply(int idTarget, bool bAlliance, bool bAgree)
{
	if (bAlliance)
	{
		SendNetData(
		GNET::FactionOPRequest(
		GNET::_O_FACTION_ALLIANCEREPLY,
		m_iCharID,
		GNET::alliancereply_param_ct(idTarget, bAgree).marshal()));
	}
	else
	{
		SendNetData(
		GNET::FactionOPRequest(
		GNET::_O_FACTION_HOSTILEREPLY,
		m_iCharID,
		GNET::hostilereply_param_ct(idTarget, bAgree).marshal()));
	}
}

void CECGameSession::faction_remove_relation_apply(int idTarget, bool bForce)
{
	SendNetData(
		GNET::FactionOPRequest(
		GNET::_O_FACTION_REMOVERELATIONAPPLY,
		m_iCharID,
		GNET::removerelationapply_param_ct(idTarget, bForce).marshal()));
}

void CECGameSession::faction_remove_relation_reply(int idTarget, bool bAgree)
{
	SendNetData(
		GNET::FactionOPRequest(
		GNET::_O_FACTION_REMOVERELATIONREPLY,
		m_iCharID,
		GNET::removerelationreply_param_ct(idTarget, bAgree).marshal()));
}

void CECGameSession::faction_get_relationlist()
{
	SendNetData(
		GNET::FactionOPRequest(
			GNET::_O_FACTION_LISTRELATION,
			m_iCharID,
			GNET::listrelation_param_ct().marshal()));
}

void CECGameSession::faction_renamefaction(const ACHAR* szNewName)
{
	Octets o = GNET::factionrename_param_ct(Octets(szNewName, a_str_size(szNewName))).marshal();
	c2s_CmdNPCSevFaction(GNET::_O_FACTION_FACTIONRENAME, o.begin(), o.size());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Chatroom Protocols
//	
///////////////////////////////////////////////////////////////////////////

void CECGameSession::chatroom_Create(const ACHAR* szSubject, const ACHAR* szPassword, int iCapacity)
{
	GNET::ChatRoomCreate p;
	p.roleid = m_iCharID;
	p.capacity = (unsigned short)iCapacity;

	if (szSubject && szSubject[0])
	{
		int iLen = a_strlen(szSubject) * sizeof (ACHAR);
		p.subject.replace(szSubject, iLen);
	}

	if (szPassword && szPassword[0])
	{
		int iLen = a_strlen(szPassword) * sizeof (ACHAR);
		p.password.replace(szPassword, iLen);
	}
	
	SendNetData(p);
}

void CECGameSession::chatroom_Expel(int idRoom, int idPlayer)
{
	GNET::ChatRoomExpel p;
	p.roleid = idPlayer;
	p.roomid = idRoom;
	p.owner = m_iCharID;
	
	SendNetData(p);
}

void CECGameSession::chatroom_Invite(int idRoom, int idInvitee)
{
	GNET::ChatRoomInvite p;
	p.invitee = idInvitee;
	p.roomid = idRoom;
	p.inviter = m_iCharID;
	
	SendNetData(p);
}

void CECGameSession::chatroom_Invite(int idRoom, const ACHAR* szName)
{
	if (szName && szName[0])
	{
		GNET::ChatRoomInvite p;
		p.invitee = 0;
		p.roomid = idRoom;
		p.inviter = m_iCharID;
		int iLen = a_strlen(szName) * sizeof (ACHAR);
		p.name.replace(szName, iLen);
		SendNetData(p);
	}
}

void CECGameSession::chatroom_RejectInvite(int idRoom, int idInviter)
{
	GNET::ChatRoomInvite_Re p;
	p.invitee = m_iCharID;
	p.roomid = idRoom;
	p.inviter = idInviter;
	p.retcode = GNET::ERR_CHAT_INVITE_REFUSED;
	
	SendNetData(p);
}

void CECGameSession::chatroom_Join(int idRoom, const ACHAR* szPassword)
{
	GNET::ChatRoomJoin p;
	p.roomid = idRoom;
	p.roleid = m_iCharID;
	
	if (szPassword && szPassword[0])
	{
		int iLen = a_strlen(szPassword) * sizeof (ACHAR);
		p.password.replace(szPassword, iLen);
	}

	SendNetData(p);
}

void CECGameSession::chatroom_Join(const ACHAR* szOwner, const ACHAR* szPassword)
{
	GNET::ChatRoomJoin p;
	p.roomid = 0;
	p.ownerid = 0;
	p.roleid = m_iCharID;
	
	if (!szOwner || !szOwner[0])
		return;
	else
	{
		int iLen = a_strlen(szOwner) * sizeof (ACHAR);
		p.ownername.replace(szOwner, iLen);
	}
		
	if (szPassword && szPassword[0])
	{
		int iLen = a_strlen(szPassword) * sizeof (ACHAR);
		p.password.replace(szPassword, iLen);
	}

	SendNetData(p);
}

void CECGameSession::chatroom_Leave(int idRoom)
{
	GNET::ChatRoomLeave p;
	p.roomid = idRoom;
	p.roleid = m_iCharID;
	
	SendNetData(p);
}

void CECGameSession::chatroom_List(int iBegin, bool bReverse)
{
	GNET::ChatRoomList p;
	p.begin = iBegin;
	p.reverse = bReverse ? 1 : 0;
	
	SendNetData(p);
}

void CECGameSession::chatroom_Speak(int idRoom, int idDstPlayer, const ACHAR* szMsg)
{
	GNET::ChatRoomSpeak p;
	p.roomid = idRoom;
	p.src = m_iCharID;
	p.dst = idDstPlayer;
	
	if (!szMsg || !szMsg[0])
		return;
	else
	{
		int iMsgLen = a_strlen(szMsg);
		a_ClampRoof(iMsgLen, 100);

		int iLen = iMsgLen * sizeof (ACHAR);
		p.message.replace(szMsg, iLen);
	}

	SendNetData(p);
}

void CECGameSession::OnChatRoomProtocols(GNET::Protocol* pProtocol)
{
	g_pGame->GetGameRun()->PostMessage(MSG_HST_CHATROOMOPT, MAN_PLAYER, 0, (DWORD)pProtocol, pProtocol->GetType());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Mail Protocols
//	
///////////////////////////////////////////////////////////////////////////

void CECGameSession::mail_CheckNew()
{
	GNET::CheckNewMail p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

void CECGameSession::mail_GetList()
{
	GNET::GetMailList p;
	p.roleid = m_iCharID;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevMail(GNET::PROTOCOL_GETMAILLIST, o.begin(), o.size());
}

void CECGameSession::mail_Get(int idMail)
{
	GNET::GetMail p;
	p.roleid = m_iCharID;
	p.mail_id = (BYTE)idMail;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevMail(GNET::PROTOCOL_GETMAIL, o.begin(), o.size());
}

void CECGameSession::mail_GetAttachment(int idMail, int iType)
{
	GNET::GetMailAttachObj p;
	p.roleid = m_iCharID;
	p.mail_id = (BYTE)idMail;
	p.obj_type = (BYTE)iType;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevMail(GNET::PROTOCOL_GETMAILATTACHOBJ, o.begin(), o.size());
}

void CECGameSession::mail_Delete(int idMail)
{
	GNET::DeleteMail p;
	p.roleid = m_iCharID;
	p.mail_id = (BYTE)idMail;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevMail(GNET::PROTOCOL_DELETEMAIL, o.begin(), o.size());
}

void CECGameSession::mail_Preserve(int idMail, bool bPreserve)
{
	GNET::PreserveMail p;
	p.roleid = m_iCharID;
	p.mail_id = (BYTE)idMail;
	p.blPreserve = bPreserve ? 1 : 0;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevMail(GNET::PROTOCOL_PRESERVEMAIL, o.begin(), o.size());
}

void CECGameSession::mail_Send(int idReceiver, const ACHAR* szTitle, const ACHAR* szContext,
						int idItem, int iItemNum, int iItemPos, size_t dwMoney)
{
	if (idItem > 0 && iItemNum > 0 && iItemPos >= 0)
		FreezeHostItem(IVTRTYPE_PACK, iItemPos, true);

	GNET::PlayerSendMail p;
	p.roleid = m_iCharID;
	p.receiver = idReceiver;
	p.attach_obj_id = idItem;
	p.attach_obj_num = iItemNum;
	p.attach_obj_pos = iItemPos;
	p.attach_money = dwMoney;

	if (szTitle)
	{
		int iLen = a_strlen(szTitle) * sizeof (ACHAR);
		p.title.replace(szTitle, iLen);
	}

	if (szContext)
	{
		int iLen = a_strlen(szContext) * sizeof (ACHAR);
		p.context.replace(szContext, iLen);
	}
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevMail(GNET::PROTOCOL_PLAYERSENDMAIL, o.begin(), o.size());
}

void CECGameSession::OnMailProtocols(GNET::Protocol* pProtocol)
{
	g_pGame->GetGameRun()->PostMessage(MSG_HST_MAILOPT, MAN_PLAYER, 0, (DWORD)pProtocol, pProtocol->GetType());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Auction Protocols
//	
///////////////////////////////////////////////////////////////////////////

void CECGameSession::auction_Open(int iCategory, int tid, int iItemPos, int iItemNum,
							int iBasePrice, int iBinPrice, int iTime)
{
	GNET::AuctionOpen p;
	p.roleid = m_iCharID;
	p.category = iCategory;
	p.item_id = tid;
	p.item_pos = iItemPos;
	p.item_num = iItemNum;
	p.baseprice = iBasePrice;
	p.binprice = iBinPrice;
	p.elapse_time = iTime;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevVendue(GNET::PROTOCOL_AUCTIONOPEN, o.begin(), o.size());
}

void CECGameSession::auction_Bid(int idAuction, int iBidPrice, bool bBin)
{
	GNET::AuctionBid p;
	p.roleid = m_iCharID;
	p.auctionid = idAuction;
	p.bidprice = iBidPrice;
	p.bin = bBin ? 1 : 0;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevVendue(GNET::PROTOCOL_AUCTIONBID, o.begin(), o.size());
}

void CECGameSession::auction_List(int iCategory, int iBegin, bool bReverse, int iItemID)
{
	GNET::AuctionList p;
	p.roleid = m_iCharID;
	p.category = iCategory;
	p.begin = iBegin;
	p.reverse = bReverse ? 1 : 0;
	p.item_id = iItemID;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevVendue(GNET::PROTOCOL_AUCTIONLIST, o.begin(), o.size());
}

void CECGameSession::auction_Close(int idAuction, int iReason)
{
	GNET::AuctionClose p;
	p.roleid = m_iCharID;
	p.auctionid = idAuction;
	p.reason = (BYTE)iReason;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevVendue(GNET::PROTOCOL_AUCTIONCLOSE, o.begin(), o.size());
}

void CECGameSession::auction_Get(int idAuction)
{
	GNET::AuctionGet p;
	p.roleid = m_iCharID;
	p.auctionid = idAuction;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevVendue(GNET::PROTOCOL_AUCTIONGET, o.begin(), o.size());
}

void CECGameSession::auction_AttendList()
{
	GNET::AuctionAttendList p;
	p.roleid = m_iCharID;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevVendue(GNET::PROTOCOL_AUCTIONATTENDLIST, o.begin(), o.size());
}

void CECGameSession::auction_ExitBid(int idAuction)
{
	GNET::AuctionExitBid p;
	p.roleid = m_iCharID;
	p.auctionid = idAuction;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevVendue(GNET::PROTOCOL_AUCTIONEXITBID, o.begin(), o.size());
}

void CECGameSession::auction_GetItems(int iNumItem, int* aAuctionIDs)
{
	if (!iNumItem || !aAuctionIDs)
		return;

	GNET::AuctionGetItem p;
	p.roleid = m_iCharID;
	
	for (int i=0; i < iNumItem; i++)
		p.ids.push_back(aAuctionIDs[i]);

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevVendue(GNET::PROTOCOL_AUCTIONGETITEM, o.begin(), o.size());
}

void CECGameSession::auction_ListUpdate(int iNumItem, int *aAuctionIDs)
{
	if (!iNumItem || !aAuctionIDs)
		return;

	GNET::AuctionListUpdate p;
	p.roleid = m_iCharID;
	
	for (int i=0; i < iNumItem; i++)
		p.ids.push_back(aAuctionIDs[i]);

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevVendue(GNET::PROTOCOL_AUCTIONLISTUPDATE, o.begin(), o.size());
}

void CECGameSession::OnVendueProtocols(GNET::Protocol* pProtocol)
{
	g_pGame->GetGameRun()->PostMessage(MSG_HST_VENDUEOPT, MAN_PLAYER, 0, (DWORD)pProtocol, pProtocol->GetType());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Web Trade Protocols
//	
///////////////////////////////////////////////////////////////////////////

// 按页从交易平台获取所有寄售物品列表
// 参数:
// iCategory 选中的物品类别查询号，从本地文件中获取
// iBegin 服务器端索引值，服务器从此值开始正向（或反向）查找生成本次请求结果，客户端在查询相邻页面时、不能对此值做任何假设
// bReverse 是否反向查找，若为true，则iBegin为所查找页面的下一个页面第一项索引值-1
//
void CECGameSession::webTrade_List(int iCategory, int iBegin, bool bReverse)
{
	if (iCategory<0)
		return;

	GNET::WebTradeList p;
	p.roleid = m_iCharID;
	p.category = iCategory;
	p.begin = iBegin;
	p.reverse = bReverse;
	
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevWebTrade(GNET::PROTOCOL_WEBTRADELIST, o.begin(), o.size());
}

// 按页从交易平台获取与玩家相关的寄售物品列表
// 参数:
// iBegin 服务器端索引值，服务器从此值开始正向查找生成本次请求结果，相邻页面此值连续
// bSell 为true时查找玩家的寄售物品，为false时查找指定买家为玩家的寄售物品列表
//
void CECGameSession::webTrade_AttendList(int iBegin, bool bSell)
{	
	GNET::WebTradeAttendList p;
	p.roleid = m_iCharID;
	p.getsell = bSell;
	p.begin = iBegin;
	
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevWebTrade(GNET::PROTOCOL_WEBTRADEATTENDLIST, o.begin(), o.size());
}

// 查询指定寄售物品的概要信息：ID、名称、寄售时间、价格、状态、指定买家、卖家等
//
void CECGameSession::webTrade_GetItems(int iNumItem, const __int64 *aTradeIDs)
{
	if (iNumItem<=0 || !aTradeIDs)
		return;
	
	GNET::WebTradeGetItem p;
	p.roleid = m_iCharID;
	while (iNumItem>0)
	{
		// 每次只发出10个，以限制协议大小
		// 分多次发出

		int count = min(10, iNumItem);

		p.sns.clear();
		for (int i=0; i<count; ++i)
			p.sns.push_back(aTradeIDs[i]);
		
		Octets o = p.marshal(Marshal::OctetsStream());
		c2s_SendCmdNPCSevWebTrade(GNET::PROTOCOL_WEBTRADEGETITEM, o.begin(), o.size());

		iNumItem -= count;
	}
}

// 查询指定寄售物品的所有信息，包括概要信息、详细信息
//
void CECGameSession::webTrade_GetItemDetail(__int64 idTrade)
{
	if (idTrade<0)
		return;
	
	GNET::WebTradeGetDetail p;
	p.roleid = m_iCharID;
	p.sn = idTrade;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevWebTrade(GNET::PROTOCOL_WEBTRADEGETDETAIL, o.begin(), o.size());
}

// 预寄售玩家普通包裹中金钱
//
void CECGameSession::webTrade_PrePostMoney(int nMoney, int nPrice, int nSellPeriod, int nBuyerRoleID/* =0 */)
{
	if (nMoney<=0 || nPrice<0 || nSellPeriod<=0 || nBuyerRoleID<0)
		return;
	
	GNET::WebTradePrePost p;
	p.roleid = m_iCharID;
	p.posttype = 1;
	p.money = nMoney;
	p.item_id = 0;
	p.item_pos = 0;
	p.item_num = 0;
	p.price = nPrice;
	p.sellperiod = nSellPeriod;
	p.buyer_roleid = nBuyerRoleID;
	
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevWebTrade(GNET::PROTOCOL_WEBTRADEPREPOST, o.begin(), o.size());
}

// 预寄售玩家普通包裹中物品
//
void CECGameSession::webTrade_PrePostItem(int idItem, int nSlot, int nCount, int nPrice, int nSellPeriod, int nBuyerRoleID/* =0 */)
{
	if (idItem<=0 || nSlot<0 || nCount<=0 || nPrice<0 || nSellPeriod<=0 || nBuyerRoleID<0)
		return;
	
	GNET::WebTradePrePost p;
	p.roleid = m_iCharID;
	p.posttype = 2;
	p.money = 0;
	p.item_id = idItem;
	p.item_pos = nSlot;
	p.item_num = nCount;
	p.price = nPrice;
	p.sellperiod = nSellPeriod;
	p.buyer_roleid = nBuyerRoleID;
	
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevWebTrade(GNET::PROTOCOL_WEBTRADEPREPOST, o.begin(), o.size());
}

// 预寄售玩家角色
//
void CECGameSession::webTrade_PrePostRole(int idRole, int nPrice, int nSellPeriod, int nBuyerRoleID/* =0 */)
{
	if (idRole<=0 || nPrice<0 || nSellPeriod<=0 || nBuyerRoleID<0)
		return;
	
	GNET::WebTradeRolePrePost p;
	p.userid = m_iUserID;
	p.roleid = idRole;
	p.price = nPrice;
	p.sellperiod = nSellPeriod;
	p.buyer_roleid = nBuyerRoleID;
	
	SendNetData(p);
}

// 取消指定物品的寄售（只有已寄售但尚未上架物品才能取消寄售）
// 
void CECGameSession::webTrade_PreCancel(__int64 idTrade)
{
	if (idTrade<0)
		return;
	
	GNET::WebTradePreCancelPost p;
	p.roleid = m_iCharID;
	p.sn = idTrade;
	
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevWebTrade(GNET::PROTOCOL_WEBTRADEPRECANCELPOST, o.begin(), o.size());
}

// 取消指定角色的寄售（只有已寄售但尚未上架角色才能取消寄售）
// 
void CECGameSession::webTrade_RolePreCancelPost(int idRole)
{
	if (idRole<0)
		return;
	
	GNET::WebTradeRolePreCancelPost p;
	p.userid = m_iUserID;
	p.roleid = idRole;
	
	SendNetData(p);
}

// 更新指定物品的寄售概要信息
// 
void CECGameSession::webTrade_Update(__int64 idTrade)
{
	if (idTrade<0)
		return;
	
	GNET::WebTradeUpdate p;
	p.roleid = m_iCharID;
	p.sn = idTrade;
	
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevWebTrade(GNET::PROTOCOL_WEBTRADEUPDATE, o.begin(), o.size());
}

// 获取寄售角色的详细信息
// 
void CECGameSession::webTrade_RoleGetDetail(int idRole)
{
	if (idRole<0)
		return;
	
	GNET::WebTradeRoleGetDetail p;
	p.userid = m_iUserID;
	p.roleid = idRole;
	
	SendNetData(p);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Faction Fortress Protocols
//	
///////////////////////////////////////////////////////////////////////////

//	开启帮派基地
void CECGameSession::factionFortress_Create()
{
	GNET::CreateFactionFortress p;
	
	p.roleid = m_iCharID;

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	p.factionid = pHost->GetFactionID();
	
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevFactionFortressService1(GNET::PROTOCOL_CREATEFACTIONFORTRESS, o.begin(), o.size());
}

//	进入帮派基地
void CECGameSession::factionFortress_Enter(int idTarget)
{
	GNET::FactionFortressEnter p;

	p.roleid = m_iCharID;

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	p.factionid = pHost->GetFactionID();

	p.dst_factionid = idTarget;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevFactionFortressService1(GNET::PROTOCOL_FACTIONFORTRESSENTER, o.begin(), o.size());
}

//	获取帮派基地列表
void CECGameSession::factionFortress_List(int begin)
{
	GNET::FactionFortressList p;

	p.roleid = m_iCharID;
	p.begin = (unsigned int)begin;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevFactionFortressService1(GNET::PROTOCOL_FACTIONFORTRESSLIST, o.begin(), o.size());
}

//	基地宣战
void CECGameSession::factionFortress_Challenge(int idTarget)
{
	GNET::FactionFortressChallenge p;

	p.roleid = m_iCharID;

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	p.factionid = pHost->GetFactionID();

	p.target_factionid = idTarget;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevFactionFortressService1(GNET::PROTOCOL_FACTIONFORTRESSCHALLENGE, o.begin(), o.size());
}

//	获取基地对战表
void CECGameSession::factionFortress_BattleList()
{
	GNET::FactionFortressBattleList p;

	p.roleid = m_iCharID;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevFactionFortressService1(GNET::PROTOCOL_FACTIONFORTRESSBATTLELIST, o.begin(), o.size());
}

//	查询指定帮派基地信息
void CECGameSession::factionFortress_Get(int idTarget)
{
	GNET::FactionFortressGet p;
	
	p.roleid = m_iCharID;
	p.factionid = idTarget;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevFactionFortressService1(GNET::PROTOCOL_FACTIONFORTRESSGET, o.begin(), o.size());
}

///////////////////////////////////////////////////////////////////////////
//	
//	SysAuction Protocols
//	
///////////////////////////////////////////////////////////////////////////

//  获取商城竞拍帐号信息
//
void CECGameSession::sysAuction_Account()
{
	SysAuctionAccount p(m_iCharID, 0);
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdSysAuction(GNET::PROTOCOL_SYSAUCTIONACCOUNT, o.begin(), o.size());
}

//  商城竞拍出价
//
void CECGameSession::sysAuction_Bid(unsigned int sa_id, int bid_price)
{
	if (sa_id == 0)
		return;

	SysAuctionBid p(m_iCharID, sa_id, bid_price, 0);
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdSysAuction(GNET::PROTOCOL_SYSAUCTIONBID, o.begin(), o.size());
}

//  存取商城竞拍元宝
//
void CECGameSession::sysAuction_CashTransfer(bool bWithdraw, int nCash)
{
	SysAuctionCashTransfer p;

	p.roleid = m_iCharID;
	p.withdraw = bWithdraw;
	p.cash = nCash;
	p.localsid = 0;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdSysAuction(GNET::PROTOCOL_SYSAUCTIONCASHTRANSFER, o.begin(), o.size());
}

//  查询商城竞拍物品
//
void CECGameSession::sysAuction_GetItem(unsigned int *sa_ids, int count)
{
	if (!sa_ids || count <= 0)
		return;

	SysAuctionGetItem p;

	p.roleid = m_iCharID;
	p.localsid = 0;

	const int nMaxItem = 100;

	while (count>0)
	{
		int size = min(count, nMaxItem);
		p.ids.assign(sa_ids, sa_ids+size);
		Octets o = p.marshal(Marshal::OctetsStream());
		c2s_SendCmdSysAuction(GNET::PROTOCOL_SYSAUCTIONGETITEM, o.begin(), o.size());

		count -= size;
		sa_ids += size;
	}
}

//  查询商城竞拍物品列表
//
void CECGameSession::sysAuction_List()
{
	SysAuctionList p(m_iCharID, 0);
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdSysAuction(GNET::PROTOCOL_SYSAUCTIONLIST, o.begin(), o.size());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Battle Protocols
//	
///////////////////////////////////////////////////////////////////////////

void CECGameSession::battle_Challenge(int idDomain, int idFaction, int iDeposit)
{
	GNET::BattleChallenge p;
	p.roleid = m_iCharID;
	p.id = (short)idDomain;
	p.factionid = idFaction;
	p.deposit = iDeposit;	
	
	//	计算验证信息

	//	1.查寻服务器传来的rand_num
	int rand_num = 0;
	CECGameUIMan * pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUIMan)
	{
		CDlgGuildMap *pDlg = (CDlgGuildMap *)pGameUIMan->GetDialog("Win_GuildMap");
		if (pDlg)
			rand_num = pDlg->GetBattleChallengeRandNum();
	}
	
	//	2.计算服务器当前时间戳
	time_t serverGMTTime = g_pGame->GetServerGMTTime();
	time_t serverLocalTime = serverGMTTime - g_pGame->GetTimeZoneBias()*60;
	tm tmServerLocalTime = *gmtime(&serverLocalTime);
	time_t t_base = serverGMTTime - tmServerLocalTime.tm_wday*24*3600 - tmServerLocalTime.tm_hour*3600 - tmServerLocalTime.tm_min*60 - tmServerLocalTime.tm_sec;	//	计算服务器本周开始时刻（周日00:00:00）

	//	3.计算验证值
	p.authentication = (rand_num ^ ((t_base >> 8) & 0xFFFF) ^ p.id);

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevBattle(GNET::PROTOCOL_BATTLECHALLENGE, o.begin(), o.size());
}

void CECGameSession::battle_ChallengeMap()
{
	GNET::BattleChallengeMap p;
	p.roleid = m_iCharID;
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	p.factionid = pHost->GetFactionID();

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevBattle(GNET::PROTOCOL_BATTLECHALLENGEMAP, o.begin(), o.size());
}

void CECGameSession::battle_GetMap()
{
	GNET::BattleGetMap p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

void CECGameSession::battle_Enter(int idBattle)
{
	GNET::BattleEnter p;
	p.roleid = m_iCharID;
	p.battle_id = idBattle;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevBattle(GNET::PROTOCOL_BATTLEENTER, o.begin(), o.size());
}

void CECGameSession::battle_GetStatus()
{
	GNET::BattleStatus p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

void CECGameSession::OnBattleProtocols(GNET::Protocol* pProtocol)
{
	g_pGame->GetGameRun()->PostMessage(MSG_HST_BATTLEOPT, MAN_PLAYER, 0, (DWORD)pProtocol, pProtocol->GetType());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Account Protocols
//	
///////////////////////////////////////////////////////////////////////////

void CECGameSession::account_GetSelfSellList()
{
	GNET::GetSellList p;
	p.roleid = m_iCharID;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevAccountPoint(GNET::PROTOCOL_GETSELLLIST, o.begin(), o.size());
}

void CECGameSession::account_GetShopSellList(int idStart, bool bReverse)
{
	GNET::FindSellPointInfo p;
	p.startid = idStart;
	p.forward = bReverse ? 0 : 1;
	SendNetData(p);
}

void CECGameSession::account_SellPoint(int iPoint, DWORD dwPrice)
{
	GNET::SellPoint p;
	p.roleid = m_iCharID;
	p.point = iPoint;
	p.price = (int)dwPrice;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevAccountPoint(GNET::PROTOCOL_SELLPOINT, o.begin(), o.size());
}

void CECGameSession::account_CancelSellPoint(int idSell)
{
	GNET::SellCancel p;
	p.roleid = m_iCharID;
	p.sellid = idSell;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevAccountPoint(GNET::PROTOCOL_SELLCANCEL, o.begin(), o.size());
}

void CECGameSession::account_BuyPoint(int idSell, int idSeller)
{
	GNET::BuyPoint p;
	p.roleid = m_iCharID;
	p.sellid = idSell;
	p.seller = idSeller;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevAccountPoint(GNET::PROTOCOL_BUYPOINT, o.begin(), o.size());
}

void CECGameSession::OnAccountProtocols(GNET::Protocol* pProtocol)
{
	g_pGame->GetGameRun()->PostMessage(MSG_HST_ACCOUNTPOINT, MAN_PLAYER, 0, (DWORD)pProtocol, pProtocol->GetType());
}

void CECGameSession::matrix_response(int iResponse)
{
	GNET::MatrixResponse p;
	p.response = iResponse;
	SendNetData(p);
}

void CECGameSession::cash_Lock(char cLock, const ACHAR* szPassword)
{
	GNET::CashLock p;
	p.userid = m_iCharID;
	p.lock = cLock;

	if (szPassword && szPassword[0])
	{
		int iLen = a_strlen(szPassword) * sizeof (ACHAR);
		p.cash_password.replace(szPassword, iLen);
	}

	SendNetData(p);
}

void CECGameSession::cash_SetPassword(const ACHAR* szPassword)
{
	GNET::CashPasswordSet p;
	p.userid = m_iCharID;

	if (szPassword && szPassword[0])
	{
		int iLen = a_strlen(szPassword) * sizeof (ACHAR);
		p.cash_password.replace(szPassword, iLen);
	}

	SendNetData(p);
}

void CECGameSession::autolock_set(int timeout)
{
	GNET::AutolockSet p;
	p.roleid = m_iCharID;
	p.userid = 0;
	p.timeout = timeout;
	p.localsid = 0;

	SendNetData(p);
}
// Dividend commands
void CECGameSession::dividend_GetReferenceCode()
{
	GNET::RefGetReferenceCode p;
	p.roleid = m_iCharID;
	p.localsid = 0;

	SendNetData(p);
}

void CECGameSession::dividend_GetListReferrals(int start_index)
{
	GNET::RefListReferrals p;
	p.roleid = m_iCharID;
	p.start_index = start_index;
	p.localsid = 0;

	SendNetData(p);
}

void CECGameSession::dividend_WithdrawDividend()
{
	GNET::RefWithdrawBonus p;
	p.roleid = m_iCharID;
	p.localsid = 0;

	SendNetData(p);
}

//  Consume rewards
void CECGameSession::reward_GetRewardList(int start_index)
{
	GNET::GetRewardList p;
	p.roleid = m_iCharID;
	p.start_index = start_index;
	p.localsid = 0;

	SendNetData(p);
}
void CECGameSession::reward_ExchangeConsumePoints(int reward_type)
{
	GNET::ExchangeConsumePoints p;
	p.roleid = m_iCharID;
	p.rewardtype = reward_type;
	p.localsid = 0;

	SendNetData(p);	
}

void CECGameSession::usercash_InstantAdd(const GNET::Octets& cardNum, const GNET::Octets& password)
{
	UserAddCash p;
	p.userid = GetUserID();
	p.cardnum = cardNum;
	p.cardpasswd = password;
	p.localsid = 0;

	SendNetData(p);
}

//	SSO
AString CECGameSession::GetTicketAccount()
{
	AString strAccount = GetUserName();
	const char *szSSO = "@sso";
	int nSSO = strlen(szSSO);
	if (strAccount.Right(nSSO) == szSSO)
		strAccount.CutRight(nSSO);
	return strAccount;
}

AString CECGameSession::GetAgentAccount()
{
	//	查询使用第三方登录时第三方的账号名，当前不是第三方登录时返回空

	AString strAgentAccount;

	AString strAccount = GetUserName();
	const char *szSSO = "@sso";
	int nSSO = strlen(szSSO);
	if (strAccount.Right(nSSO) == szSSO)
	{
		strAccount.CutRight(nSSO);
		
		AString strAgentName = GetAgentName();
		if (!strAgentName.IsEmpty())
		{
			AString strTestAgent = strAccount.Right(1+strAgentName.GetLength());
			if (strTestAgent == (AString("@")+strAgentName))
			{				
				//	第三方账号名称需要除去 agentname 后缀
				strAgentAccount = strAccount.Left(strAccount.GetLength()-strTestAgent.GetLength());
			}
		}
	}

	return strAgentAccount;
}

void CECGameSession::sso_GetTicket(const GNET::Octets &local_context, const GNET::Octets &info, int toaid, int tozoneid)
{
	SSOGetTicket p;

	//	填写 SSOUser
	p.user.isagent = 0;
	p.user.userid = GetUserID();
	p.user.account.clear();
	p.user.agentname.clear();
	p.user.agentaccount.clear();

	AString strAccount = GetUserName();
	const char *szSSO = "@sso";
	int nSSO = strlen(szSSO);
	if (strAccount.Right(nSSO) == szSSO)
	{
		//	任何情况下申请 ticket 都不带 szSSO 后缀
		strAccount.CutRight(nSSO);
	}

	AString strAgentAccount = GetAgentAccount();
	if (!strAgentAccount.IsEmpty())
	{
		//	是第三方登录
		p.user.isagent = 1;

		AString strAgentName = GetAgentName();
		p.user.agentname.replace(strAgentName, strAgentName.GetLength());
		
		p.user.agentaccount.replace(strAgentAccount, strAgentAccount.GetLength());
	}

	if (!p.user.isagent)
		p.user.account.replace(strAccount, strAccount.GetLength());

	//	填写其它内容
	p.toaid = toaid;
	p.tozoneid = tozoneid;
	p.info = info;
	p.local_context = local_context;

	//	发送协议
	SendNetData(p);
}

void CECGameSession::country_Move(int dest_domain_id)
{
	CountryBattleMove p;
	p.roleid = m_iCharID;
	p.dest = dest_domain_id;
	SendNetData(p);
}

void CECGameSession::country_GetPlayerLocation()
{
	CountryBattleGetPlayerLocation p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

void CECGameSession::country_GetMap()
{
	CountryBattleGetMap p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

void CECGameSession::country_GetScore()
{
	CountryBattleGetScore p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

void CECGameSession::country_GetConfig()
{
	CountryBattleGetConfig p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

void CECGameSession::country_Enter(int domain_id)
{
	CountryBattlePreEnter p;
	p.roleid = m_iCharID;
	p.battle_id = domain_id;
	SendNetData(p);
}

void CECGameSession::country_BackToCapital()
{
	CountryBattleReturnCapital p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

void CECGameSession::country_GetBattleLimit(int domain_id)
{
	CountryBattleGetBattleLimit p;
	p.roleid = m_iCharID;
	p.domain_id = domain_id;
	SendNetData(p);
}

void CECGameSession::king_CandidateAuction(int tid, int num)
{
	KECandidateApply p;
	p.roleid = GetCharacterID();
	p.item_id = tid;
	p.item_num = num;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevKing(GNET::PROTOCOL_KECANDIDATEAPPLY, o.begin(), o.size());
}

void CECGameSession::king_CandidateVote(int candidate_id, int tid, int item_slot, int num)
{
	KEVoting p;
	p.roleid = GetCharacterID();
	p.candidate_roleid = candidate_id;
	p.item_id = tid;
	p.item_pos = item_slot;
	p.item_num = num;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevKing(GNET::PROTOCOL_KEVOTING, o.begin(), o.size());
}

void CECGameSession::king_GetCandidateStatus()
{
	KEGetStatus p;
	p.roleid = GetCharacterID();

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_CmdNPCSevKing(GNET::PROTOCOL_KEGETSTATUS, o.begin(), o.size());
}

void CECGameSession::king_SetBattleLimit(int domain_id, const abase::vector<int>& occupation_cnt, const abase::vector<int>& ghost_value)
{
	ASSERT(occupation_cnt.size() == ghost_value.size());
	ASSERT(occupation_cnt.size() >= NUM_PROFESSION);

	CountryBattleKingResetBattleLimit p;
	p.king_roleid = GetCharacterID();
	p.domain_id = domain_id;
	p.op = 1;
	for(int i=0;i<NUM_PROFESSION; i++)
	{
		GCountryBattleLimit l;
		l.occupation_cnt_ceil = occupation_cnt[i];
		l.minor_str_floor = ghost_value[i];
		p.limit.push_back(l);
	}
	SendNetData(p);
}

void CECGameSession::king_ClearBattleLimit(int domain_id)
{
	CountryBattleKingResetBattleLimit p;
	p.king_roleid = GetCharacterID();
	p.domain_id = domain_id;
	p.op = 0;
	SendNetData(p);
}

void CECGameSession::king_AssignAssault(int domain_id, bool bBig)
{
	CountryBattleKingAssignAssault p;
	p.king_roleid = GetCharacterID();
	p.domain_id = domain_id;
	p.assault_type = bBig ? DOMAIN2_INFO::STATE_BIGTOKEN : DOMAIN2_INFO::STATE_SMALLTOKEN;
	SendNetData(p);
}

void CECGameSession::king_GetKingPoint()
{
	CountryBattleGetKingCommandPoint p;
	p.roleid = GetCharacterID();
	SendNetData(p);
}

void CECGameSession::quickpay_AddCash(int cash, int cash_after_discount, int merchant_id)
{
	QPAddCash p;
	p.userid = GetUserID();
	p.cash = cash;
	p.cash_after_discount = cash_after_discount;
	p.merchant_id = merchant_id;
	SendNetData(p);
}

void CECGameSession::quickpay_GetActivatedServices()
{
	QPGetActivatedServices p;
	p.userid = GetUserID();
	SendNetData(p);
}

void CECGameSession::getServerConfig(const abase::vector<int>& keys)
{
	if (keys.empty()){
		return;
	}
	GetCNetServerConfig p;
	p.roleid = GetCharacterID();
	p.keys.GetVector().assign(keys.begin(), keys.end());
	SendNetData(p);
}

void CECGameSession::getServerConfig(int key)
{
	getServerConfig(abase::vector<int>(1, key));
}

//////////////////////////////////////////////////////////////////////////

void CECGameSession::OffShop_CreateMyShop(int shop_type,int item_id,int item_pos,int item_num)
{
	PShopCreate p;
	p.roleid = GetCharacterID();
	p.shoptype = shop_type;
	p.item_id = item_id;
	p.item_pos = item_pos;
	p.item_num = item_num;
	
	Octets o = p.marshal(Marshal::OctetsStream());

	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPCREATE, o.begin(), o.size());
}
void CECGameSession::OffShop_AddItemToMySellInv(int tid,int shop_pos,unsigned int price,int count, int inv_pos)
{
	PShopSell p;
	p.roleid = GetCharacterID();
	p.item_id = tid;
	p.item_pos = shop_pos;
	p.item_count = count;
	p.item_price = price;
	p.inv_pos = inv_pos;
	
	Octets o = p.marshal(Marshal::OctetsStream());
	
	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPSELL, o.begin(), o.size());
}
void CECGameSession::OffShop_AddItemToMyBuyInv(int tid,int shop_pos,unsigned int price,int count)
{
	PShopBuy p;
	p.roleid = GetCharacterID();
//	p.shoptype = shop_type;
	p.item_id = tid;
	p.item_pos = shop_pos;
	p.item_count = count;
	p.item_price = price;
	
	Octets o = p.marshal(Marshal::OctetsStream());
	
	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPBUY, o.begin(), o.size());
}
void CECGameSession::OffShop_CancelItemFromMyShop(bool bSell,int pos)
{
	PShopCancelGoods p;
	p.roleid = GetCharacterID();
	p.canceltype = bSell ? 0:1;
	p.pos = pos;

	Octets o = p.marshal(Marshal::OctetsStream());

	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPCANCELGOODS,o.begin(),o.size());
}
void CECGameSession::OffShop_SetMyShopType(int iType)
{
	PShopSetType p;
	p.roleid = GetCharacterID();
	p.newtype = iType;

	Octets o = p.marshal(Marshal::OctetsStream());
	
	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPSETTYPE,o.begin(),o.size());
}
void CECGameSession::OffShop_ActiveMyShop(int tid,int pos,int count) // 激活或延长期限	
{
	PShopActive p;
	p.roleid = GetCharacterID();
	p.item_id = tid;
	p.item_pos = pos;
	p.item_num = count;

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPACTIVE,o.begin(),o.size());
}
void CECGameSession::OffShop_ClearMyShop()	// 情况商店的收购栏和出售栏
{
	PShopClearGoods p;
	p.roleid = GetCharacterID();

	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPCLEARGOODS,o.begin(),o.size());
}
void CECGameSession::OffShop_GetMyShopInfo()
{
	PShopSelfGet p;
	p.roleid = GetCharacterID();
	
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPSELFGET,o.begin(),o.size());
}	
void CECGameSession::OffShop_TakeItemFromStore(int idx) // 从仓库取物品
{
	PShopDrawItem p;
	p.roleid = GetCharacterID();
	p.item_pos = idx;
	
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPDRAWITEM,o.begin(),o.size());
}
void CECGameSession::OffShop_OptMoneyStore(bool bSave,int m,int yinpiao) // bSave: true 存钱，false 取钱
{
	PShopManageFund p;
	p.roleid = GetCharacterID();
	p.optype = bSave ? 0:1;
	p.money = m;
	p.yinpiao = yinpiao;
	
	Octets o = p.marshal(Marshal::OctetsStream());
	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPMANAGEFUND,o.begin(),o.size());
}
void CECGameSession::OffShop_GetELShopInfo(int otherID)// 进入其他玩家店铺，获取该店铺信息
{
	PShopPlayerGet p;
	p.roleid = GetCharacterID();
	p.master = otherID;

	SendNetData(p);
}
void CECGameSession::OffShop_BuyItemFrom(int otherID,int tid,int pos,int count, int money,int yinpiao)
{
	PShopPlayerBuy p;
	p.roleid = GetCharacterID();
	p.master = otherID;
	p.item_id = tid;
	p.item_pos = pos;
	p.item_count = count;
	p.money_cost = money;
	p.yp_cost = yinpiao;

	Octets o = p.marshal(Marshal::OctetsStream());

	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPPLAYERBUY,o.begin(),o.size());
}
void CECGameSession::OffShop_SellItemTo(int otherID,int tid,int pos,int count, unsigned int price,int my_inv_pos)
{
	PShopPlayerSell p;
	p.roleid = GetCharacterID();
	p.master = otherID;
	p.item_id = tid;
	p.item_pos = pos;
	p.item_count = count;
	p.item_price = price;
	p.inv_pos = my_inv_pos;
	
	Octets o = p.marshal(Marshal::OctetsStream());
	
	c2s_SendCmdNPCSevOfflineShop(GNET::PROTOCOL_PSHOPPLAYERSELL,o.begin(),o.size());	
}
void CECGameSession::OffShop_QueryShops(int iType)	// 查询商店
{
	PShopList p;
	p.roleid = GetCharacterID();
	p.shoptype = iType;

	SendNetData(p);
}
void CECGameSession::OffShop_QueryItemListPage(int tid,bool bSell,int pageNum) // 查询物品，一次一页
{
	PShopListItem p;
	p.roleid = GetCharacterID();
	p.itemid = tid;
	p.listtype = bSell ? 0:1;
	p.page_num = pageNum;

	SendNetData(p);
}
void CECGameSession::match_GetProfile()
{
	PlayerProfileGetProfileData p;
	p.roleid = GetCharacterID();

	SendNetData(p);

}
void CECGameSession::match_SaveProfile(int timemask,int game_intres_mask,int personal_intres_amsk,unsigned char age,unsigned char zodiac, int match_mask)
{
	PlayerProfileSetProfileData p;
	p.roleid = GetCharacterID();
	p.data.game_time_mask = timemask;
	p.data.game_interest_mask = game_intres_mask;
	p.data.personal_interest_mask = personal_intres_amsk;
	p.data.age = age;
	p.data.zodiac = zodiac;
	p.data.match_option_mask = match_mask;

	SendNetData(p);
}
void CECGameSession::match_FindPlayer(int mode)
{
	PlayerProfileGetMatchResult p;
	p.roleid = GetCharacterID();
	p.match_mode = mode;

	SendNetData(p);
}
void CECGameSession::tankBattle_GetRank()
{
	TankBattlePlayerGetRank p;
	p.roleid = GetCharacterID();

	SendNetData(p);
}

void CECGameSession::sendClientMachineInfo(unsigned char *info, int size)
{
	if (info && size > 0){
		CollectClientMachineInfo p;
		p.userid = GetUserID();
		p.machineinfo = Octets(info, size);
		SendNetData(p);
	}
}
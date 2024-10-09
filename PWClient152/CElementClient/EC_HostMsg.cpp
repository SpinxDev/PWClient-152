/*
 * FILE: EC_HostMsg.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Utility.h"
#include "EC_World.h"
#include "EC_GameSession.h"
#include "EC_GPDataType.h"
#include "EC_NPC.h"
#include "EC_NPCServer.h"
#include "EC_RTDebug.h"
#include "EC_ManNPC.h"
#include "EC_ManPlayer.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_Inventory.h"
#include "EC_DealInventory.h"
#include "EC_FixedMsg.h"
#include "EC_IvtrTypes.h"
#include "EC_IvtrWeapon.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrFlySword.h"
#include "EC_IvtrItem.h"
#include "EC_GFXCaster.h"
#include "EC_ElsePlayer.h"
#include "EC_Resource.h"
#include "EC_Team.h"
#include "EC_TeamMan.h"
#include "EC_Skill.h"
#include "EC_TaskInterface.h"
#include "EC_ManAttacks.h"
#include "EC_Configs.h"
#include "EC_Friend.h"
#include "EC_Faction.h"
#include "EC_Model.h"
#include "EC_Clipboard.h"
#include "EC_Viewport.h"
#include "EC_UIManager.h"
#include "EC_PetCorral.h"
#include "EC_ComboSkill.h"
#include "ECScriptMan.h"
#include "ECScriptContext.h"
#include "ECSCriptCheckState.h"
#include "EC_ShortcutSet.h"
#include "EC_PetWords.h"
#include "EC_ForceMgr.h"
#include "EC_Pet.h"
#include "EC_UIHelper.h"
#include "EC_AutoTeam.h"
#include "EC_AutoPolicy.h"
#include "EC_PlayerWrapper.h"
#include "EC_ManMatter.h"
#include "EC_Matter.h"
#include "EC_RandMallShopping.h"
#include "EC_FactionPVP.h"
#include "EC_ShoppingManager.h"
#include "EC_ShoppingItemsMover.h"
#include "EC_Shortcut.h"
#include "EC_MsgDataDef.h"
#include "ECScriptUI.h"
#include "EC_InstanceReenter.h"
#include "EC_CustomizeMgr.h"

#include "DlgAutoLock.h"
#include "DlgInfo.h"
#include "DlgTrade.h"
#include "DlgEquipRefine.h"
#include "DlgFriendList.h"
#include "DlgChannelChat.h"
#include "DlgMailList.h"
#include "DlgAuctionList.h"
#include "DlgEPEquip.h"
#include "DlgGuildMap.h"
#include "DlgCardSell.h"
#include "DlgELFLearn.h"
#include "DlgELFRefine.h"
#include "DlgELFGenius.h"
#include "DlgELFRetrain.h"
#include "DlgELFProp.h"
#include "DlgProfView.h"
#include "globaldataman.h"
#include "DlgExp.h"
#include "DlgSkillAction.h"
#include "DlgSkillEdit.h"
#include "DlgFortressInfo.h"
#include "DlgFortressExchange.h"
#include "DlgFortressBuild.h"
#include "DlgFortressContrib.h"
#include "DlgFortressExchange.h"
#include "DlgFortressStatus.h"
#include "DlgCongregate.h"
#include "DlgEarthBagRank.h"
#include "DlgInvisibleList.h"
#include "DlgWorldMap.h"
#include "DlgForce.h"
#include "DlgInventoryDetail.h"
#include "DlgMailToFriends.h"
#include "DlgOnlineAward.h"
#include "DlgPKSetting.h"
#include "DlgCountryMap.h"
#include "DlgPreviewPetRebuild.h"
#include "DlgEquipDye.h"
#include "DlgMeridians.h"
#include "DlgMeridiansImpact.h"
#include "DlgStoneChange.h"
#include "DlgTouchShop.h"
#include "DlgTitleList.h"
#include "DlgSignIn.h"
#include "DlgAutoHelp.h"
#include "DlgActivity.h"
#include "DlgReincarnationBook.h"
#include "DlgReincarnationRewrite.h"
#include "DlgReincarnationShow.h"
#include "DlgChariot.h"
#include "DlgQuickBarPet.h"
#include "DlgGeneralCard.h"
#include "DlgStorage.h"
#include "DlgMonsterSpirit.h"
#include "DlgProduce.h"
#include "DlgMatchProfile.h"

#include "EC_ActionSwitcher.h"
#include "auto_delete.h"

#include "gnetdef.h"
#include "TradeStart_Re.hpp"
#include "TradeStartRqst.hrp"
#include "TradeMoveObj_Re.hpp"
#include "TradeDiscard_Re.hpp"
#include "TradeConfirm_Re.hpp"
#include "TradeAddGoods_Re.hpp"
#include "TradeRemoveGoods_Re.hpp"
#include "TradeSubmit_Re.hpp"
#include "TradeEnd.hpp"
#include "PlayerBaseInfo_Re.hpp"
#include "addfriend_re.hpp"
#include "addfriendremarks_re.hpp"
#include "getfriends_re.hpp"
#include "setgroupname_re.hpp"
#include "setfriendgroup_re.hpp"
#include "delfriend_re.hpp"
#include "friendstatus.hpp"
#include "addfriendrqst.hrp"
#include "getsavedmsg_re.hpp"
#include "setcustomdata_re.hpp"
#include "chatroomcreate_re.hpp"
#include "chatroomexpel.hpp"
#include "chatroominvite.hpp"
#include "chatroominvite_re.hpp"
#include "chatroomjoin_re.hpp"
#include "chatroomleave.hpp"
#include "chatroomlist_re.hpp"
#include "chatroomspeak.hpp"
#include "announcenewmail.hpp"
#include "getmail_re.hpp"
#include "getmaillist_re.hpp"
#include "deletemail_re.hpp"
#include "preservemail_re.hpp"
#include "playersendmail_re.hpp"
#include "getmailattachobj_re.hpp"
#include "auctionattendlist_re.hpp"
#include "auctionbid_re.hpp"
#include "auctionclose_re.hpp"
#include "auctionget_re.hpp"
#include "auctionlist_re.hpp"
#include "auctionopen_re.hpp"
#include "auctionexitbid_re.hpp"
#include "auctiongetitem_re.hpp"
#include "auctionlistupdate_re.hpp"

#include "battlechallenge_re.hpp"
#include "battlechallengemap_re.hpp"
#include "battlegetmap_re.hpp"
#include "battleenter_re.hpp"
#include "battlestatus_re.hpp"

#include "sellcancel_re.hpp"
#include "sellpoint_re.hpp"
#include "findsellpointinfo_re.hpp"
#include "buypoint_re.hpp"
#include "getselllist_re.hpp"
#include "announcesellresult.hpp"

#include "webtradelist_re.hpp"
#include "webtradeattendlist_re.hpp"
#include "webtradegetitem_re.hpp"
#include "webtradegetdetail_re.hpp"
#include "webtradeprepost_re.hpp"
#include "webtradeprecancelpost_re.hpp"
#include "webtradeupdate_re.hpp"

#include "sysauctionaccount_re.hpp"
#include "sysauctioncashtransfer_re.hpp"
#include "sysauctiongetitem_re.hpp"
#include "sysauctionlist_re.hpp"
#include "sysauctionbid_re.hpp"
#include "friendextlist.hpp"
#include "sendaumail_re.hpp"

#include "EC_HPWork.h"
#include "EC_HPWorkDead.h"
#include "EC_HPWorkMelee.h"
#include "EC_HPWorkStand.h"
#include "EC_HPWorkTrace.h"
#include "EC_HPWorkFollow.h"
#include "EC_HPWorkMove.h"
#include "EC_HPWorkFly.h"
#include "EC_HPWorkFall.h"
#include "EC_HPWorkSpell.h"
#include "EC_HPWorkSit.h"
#include "EC_HPWorkUse.h"
#include "EC_HostGoblin.h"
#include "EC_IvtrGoblin.h"
#include "EC_IvtrEquipMatter.h"
#include "EC_IvtrArmor.h"
#include "EC_IvtrDecoration.h"
#include "EC_Algorithm.h"
#include "EC_Meridians.h"
#include "EC_CountryConfig.h"
#include "EC_ScreenEffect.h"
#include "EC_UIConfigs.h"
#include "EC_ComboSkillState.h"

#include <A3DTerrain2.h>
#include <A3DLight.h>
#include <A3DCamera.h>
#include "AUICommon.h"
#include "elementdataman.h"
#include <vector>

#include "EC_HostSkillModel.h"
#include "EC_UIHelper.h"
#include "DlgIceThunderBall.h"

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
//	Implement CECHostPlayer
//	
///////////////////////////////////////////////////////////////////////////

//	Process message
bool CECHostPlayer::ProcessMessage(const ECMSG& Msg)
{
	switch (Msg.dwMsg)
	{
	case MSG_GST_MOVE:				OnMsgGstMove(Msg);				break;
	case MSG_GST_PITCH:				OnMsgGstPitch(Msg);				break;
	case MSG_GST_YAW:				OnMsgGstYaw(Msg);				break;
	case MSG_GST_MOVEABSUP:			OnMsgGstMoveAbsUp(Msg);			break;
	case MSG_HST_PUSHMOVE:			OnMsgHstPushMove(Msg);			break;
	case MSG_HST_AUTOMOVE:			OnMsgHstAutoMove(Msg);			break;
	case MSG_HST_PITCH:				OnMsgHstPitch(Msg);				break;
	case MSG_HST_YAW:				OnMsgHstYaw(Msg);				break;
	case MSG_HST_CAMERAMODE:		OnMsgHstCameraMode(Msg);		break;
	case MSG_HST_LBTNCLICK:			OnMsgLBtnClick(Msg);			break;
	case MSG_HST_RBTNCLICK:			OnMsgRBtnClick(Msg);			break;
	case MSG_HST_WHEELCAM:			OnMsgHstWheelCam(Msg);			break;
	case MSG_HST_CAMDEFAULT:		OnMsgHstCamDefault(Msg);		break;
	case MSG_HST_CAMPRESET:			OnMsgHstCamPreset(Msg);			break;
	case MSG_HST_CAMUSERGET:		OnMsgHstCamUserGet(Msg);		break;
	case MSG_HST_CAMUSERSET:		OnMsgHstCamUserSet(Msg);		break;

	case MSG_HST_TURNCAM:
		
		if (Msg.dwParam1)
			m_iTurnCammera |= Msg.dwParam2 ? TURN_LBUTTON : TURN_RBUTTON;
		else
			m_iTurnCammera &= Msg.dwParam2 ? ~TURN_LBUTTON : ~TURN_RBUTTON;

		break;

	case MSG_HST_ATKRESULT:			OnMsgHstAttackResult(Msg);		break;
	case MSG_HST_ATTACKED:			OnMsgHstAttacked(Msg);			break;
	case MSG_HST_DIED:				OnMsgHstDied(Msg);				break;
	case MSG_HST_PICKUPMONEY:		OnMsgHstPickupMoney(Msg);		break;
	case MSG_HST_PICKUPITEM:		OnMsgHstPickupItem(Msg);		break;
	case MSG_HST_FOLLOW:			BeginFollow(Msg.dwParam1);		break;
	case MSG_HST_RECEIVEEXP:		OnMsgHstReceiveExp(Msg);		break;
	case MSG_HST_INFO00:			OnMsgHstInfo00(Msg);			break;
	case MSG_HST_GOTO:				OnMsgHstGoto(Msg);				break;
	case MSG_HST_OWNITEMINFO:		OnMsgHstOwnItemInfo(Msg);		break;
	case MSG_HST_SELTARGET:			OnMsgHstSelTarget(Msg);			break;
	case MSG_HST_FIXCAMERA:			OnMsgHstFixCamera(Msg);			break;
	case MSG_HST_OWNEXTPROP:		OnMsgHstExtProp(Msg);			break;
	case MSG_HST_ADDSTATUSPT:		OnMsgHstAddStatusPt(Msg);		break;
	case MSG_HST_JOINTEAM:			OnMsgHstJoinTeam(Msg);			break;
	case MSG_HST_LEAVETEAM:			OnMsgHstLeaveTeam(Msg);			break;
	case MSG_HST_NEWTEAMMEM:		OnMsgHstNewTeamMem(Msg);		break;
	case MSG_HST_ITEMOPERATION:		OnMsgHstItemOperation(Msg);		break;
	case MSG_HST_TRASHBOXOP:		OnMsgHstTrashBoxOperation(Msg);	break;
	case MSG_HST_TEAMINVITE:		OnMsgHstTeamInvite(Msg);		break;
	case MSG_HST_TEAMREJECT:		OnMsgHstTeamReject(Msg);		break;
	case MSG_HST_TEAMMEMPOS:		OnMsgHstTeamMemPos(Msg);		break;
	case MSG_HST_EQUIPDAMAGED:		OnMsgHstEquipDamaged(Msg);		break;
	case MSG_HST_TEAMMEMPICKUP:		OnMsgHstTeamMemPickup(Msg);		break;
	case MSG_HST_NPCGREETING:		OnMsgHstNPCGreeting(Msg);		break;
	case MSG_HST_TRADESTART:		OnMsgHstTradeStart(Msg);		break;
	case MSG_HST_TRADEREQUEST:		OnMsgHstTradeRequest(Msg);		break;
	case MSG_HST_TRADEMOVEITEM:		OnMsgHstTradeMoveItem(Msg);		break;
	case MSG_HST_TRADECANCEL:		OnMsgHstTradeCancel(Msg);		break;
	case MSG_HST_TRADESUBMIT:		OnMsgHstTradeSubmit(Msg);		break;
	case MSG_HST_TARDECONFIRM:		OnMsgHstTradeConfirm(Msg);		break;
	case MSG_HST_TRADEEND:			OnMsgHstTradeEnd(Msg);			break;
	case MSG_HST_TRADEADDGOODS:		OnMsgHstTradeAddGoods(Msg);		break;
	case MSG_HST_TRADEREMGOODS:		OnMsgHstTradeRemGoods(Msg);		break;
	case MSG_HST_IVTRINFO:			OnMsgHstIvtrInfo(Msg);			break;
	case MSG_HST_STARTATTACK:		OnMsgHstStartAttack(Msg);		break;
	case MSG_HST_GAINITEM:			OnMsgHstGainItem(Msg);			break;
	case MSG_HST_PURCHASEITEMS:		OnMsgHstPurchaseItems(Msg);		break;
	case MSG_HST_SPENDMONEY:		OnMsgHstSpendMoney(Msg);		break;
	case MSG_HST_ITEMTOMONEY:		OnMsgHstItemToMoney(Msg);		break;
	case MSG_HST_REPAIR:			OnMsgHstRepair(Msg);			break;
	case MSG_HST_USEITEM:			OnMsgHstUseItem(Msg);			break;
	case MSG_HST_USEITEMWITHDATA:	OnMsgHstUseItemWithData(Msg);	break;
	case MSG_HST_SKILLDATA:			OnMsgHstSkillData(Msg);			break;
	case MSG_HST_EMBEDITEM:			OnMsgHstEmbedItem(Msg);			break;
	case MSG_HST_CLEARTESSERA:		OnMsgHstClearTessera(Msg);		break;
	case MSG_HST_COSTSKILLPT:		OnMsgHstCostSkillPt(Msg);		break;
	case MSG_HST_LEARNSKILL:		OnMsgHstLearnSkill(Msg);		break;
	case MSG_HST_FLYSWORDTIME:		OnMsgHstFlySwordTime(Msg);		break;
	case MSG_HST_PRODUCEITEM:		OnMsgHstProduceItem(Msg);		break;
	case MSG_HST_BREAKITEM:			OnMsgHstBreakItem(Msg);			break;
	case MSG_HST_TASKDATA:			OnMsgHstTaskData(Msg);			break;
	case MSG_HST_TARGETISFAR:		OnMsgHstTargetIsFar(Msg);		break;
	case MSG_HST_PRESSCANCEL:		OnMsgHstPressCancel(Msg);		break;
	case MSG_HST_ROOTNOTIFY:		OnMsgHstRootNotify(Msg);		break;
	case MSG_HST_STOPATTACK:		OnMsgHstStopAttack(Msg);		break;
	case MSG_HST_JUMP:				OnMsgHstJump(Msg);				break;
	case MSG_HST_HURTRESULT:		OnMsgHstHurtResult(Msg);		break;
	case MSG_HST_ATTACKONCE:		OnMsgHstAttackOnce(Msg);		break;
	case MSG_HST_PLAYTRICK:			OnMsgHstPlayTrick(Msg);			break;
	case MSG_HST_SKILLRESULT:		OnMsgHstSkillResult(Msg);		break;
	case MSG_HST_SKILLATTACKED:		OnMsgHstSkillAttacked(Msg);		break;
	case MSG_HST_ASKTOJOINTEAM:		OnMsgHstAskToJoinTeam(Msg);		break;
	case MSG_HST_FACTION:			OnMsgHstFaction(Msg);			break;	
	case MSG_HST_TASKDELIVER:		OnMsgHstTaskDeliver(Msg);		break;
	case MSG_HST_REPUTATION:		OnMsgHstReputation(Msg);		break;
	case MSG_HST_ITEMIDENTIFY:		OnMsgHstItemIdentify(Msg);		break;
	case MSG_HST_SANCTUARY:			OnMsgHstSanctuary(Msg);			break;
	case MSG_HST_CORRECTPOS:		OnMsgHstCorrectPos(Msg);		break;
	case MSG_HST_FRIENDOPT:			OnMsgHstFriendOpt(Msg);			break;
	case MSG_HST_WAYPOINT:			OnMsgHstWayPoint(Msg);			break;
	case MSG_HST_BREATHDATA:		OnMsgHstBreathData(Msg);		break;
	case MSG_HST_SKILLABILITY:		OnMsgHstSkillAbility(Msg);		break;
	case MSG_HST_COOLTIMEDATA:		OnMsgHstCoolTimeData(Msg);		break;
	case MSG_HST_REVIVALINQUIRE:	OnMsgHstRevivalInquire(Msg);	break;
	case MSG_HST_SETCOOLTIME:		OnMsgHstSetCoolTime(Msg);		break;
	case MSG_HST_CHGTEAMLEADER:		OnMsgHstChangeTeamLeader(Msg);	break;
	case MSG_HST_EXITINSTANCE:		OnMsgHstExitInstance(Msg);		break;
	case MSG_HST_CHANGEFACE:		OnMsgHstChangeFace(Msg);		break;
	case MSG_HST_TEAMMEMBERDATA:	OnMsgHstTeamMemberData(Msg);	break;
	case MSG_HST_SETMOVESTAMP:		OnMsgHstSetMoveStamp(Msg);		break;
	case MSG_HST_CHATROOMOPT:		OnMsgHstChatroomOpt(Msg);		break;
	case MSG_HST_MAILOPT:			OnMsgHstMailOpt(Msg);			break;
	case MSG_HST_VENDUEOPT:			OnMsgHstVendueOpt(Msg);			break;
	case MSG_HST_VIEWOTHEREQUIP:	OnMsgHstViewOtherEquips(Msg);	break;
	case MSG_HST_PARIAHTIME:		OnMsgHstPariahTime(Msg);		break;
	case MSG_HST_PETOPT:			OnMsgHstPetOpt(Msg);			break;
	case MSG_HST_BATTLEOPT:			OnMsgHstBattleOpt(Msg);			break;
	case MSG_HST_ACCOUNTPOINT:		OnMsgHstAccountPoint(Msg);		break;
	case MSG_HST_GMOPT:				OnMsgHstGMOpt(Msg);				break;
	case MSG_HST_REFINEOPT:			OnMsgHstRefineOpt(Msg);			break;
	case MSG_HST_ITEMBOUND:			OnMsgHstItemBound(Msg);			break;
	case MSG_HST_SAFELOCK:			OnMsgHstSafeLock(Msg);			break;
	case MSG_HST_GOBLININFO:		OnMsgHstGoblinInfo(Msg);		break;
	case MSG_HST_BUY_SELL_FAIL:		OnMsgHstBuySellFail(Msg);		break;
	case MSG_HST_HPSTEAL:			OnMsgHstHpSteal(Msg);			break;
	case MSG_HST_MULTI_EXP_INFO:	OnMsgHstMultiExpInfo(Msg);		break;
	case MSG_HST_MULTI_EXP_STATE:	OnMsgHstMultiExpState(Msg);		break;
	case MSG_HST_WEBTRADELIST:		OnMsgHstWebTradeList(Msg);		break;
	case MSG_HST_WEBTRADEATTENDLIST:OnMsgHstWebTradeAttendList(Msg);break;
	case MSG_HST_WEBTRADEGETITEM:	OnMsgHstWebTradeGetItem(Msg);	break;
	case MSG_HST_WEBTRADEGETDETAIL:	OnMsgHstWebTradeGetDetail(Msg);	break;
	case MSG_HST_WEBTRADEPREPOST:	OnMsgHstWebTradePrePost(Msg);	break;
	case MSG_HST_WEBTRADEPRECANCELPOST:OnMsgHstWebTradeCancelPost(Msg);	break;
	case MSG_HST_WEBTRADEUPDATE:	OnMsgHstWebTradeUpdate(Msg);	break;
	case MSG_HST_SYSAUCTIONACCOUNT:	OnMsgHstSysAuctionAccount(Msg);	break;
	case MSG_HST_SYSAUCTIONBID:		OnMsgHstSysAuctionBid(Msg);		break;
	case MSG_HST_SYSAUCTIONCASHTRANSFER:OnMsgHstSysAuctionCashTransfer(Msg);break;
	case MSG_HST_SYSAUCTIONGETITEM:	OnMsgHstSysAuctionGetItem(Msg);	break;
	case MSG_HST_SYSAUCTIONLIST:	OnMsgHstSysAuctionList(Msg);	break;
	case MSG_HST_WEDDINGBOOKLIST:	OnMsgHstWeddingBookList(Msg);	break;
	case MSG_HST_WEDDINGBOOKSUCCESS:OnMsgHstWeddingBookSuccess(Msg);break;
	case MSG_HST_FACTIONCONTRIB:	OnMsgHstFactionContribNotify(Msg);break;
	case MSG_HST_FACTIONFORTRESSINFO:OnMsgHstFactionFortressInfo(Msg);break;
	case MSG_HST_ENTERFACTIONFORTRESS:OnMsgHstEnterFactionFortress(Msg);break;
	case MSG_HST_ENGRAVEITEM:	OnMsgHstEngraveItem(Msg);	break;
	case MSG_HST_DPS_DPH_RANK:		OnMsgHstDPSDPHRank(Msg);	break;
	case MSG_HST_ADDONREGEN:	OnMsgHstAddonRegen(Msg);	break;
	case MSG_HST_INVISIBLEOBJLIST:	OnMsgHstInvisibleObjList(Msg);	break;
	case MSG_HST_SETPLAYERLIMIT:	OnMsgHstSetPlayerLimit(Msg);	break;

	case MSG_PM_PLAYERBASEINFO:		OnMsgPlayerBaseInfo(Msg);		break;
	case MSG_PM_PLAYERCUSTOM:		OnMsgPlayerCustomData(Msg);		break;
	case MSG_PM_PLAYERFLY:			OnMsgPlayerFly(Msg);			break;
	case MSG_PM_PLAYERSITDOWN:		OnMsgPlayerSitDown(Msg);		break;
	case MSG_PM_CASTSKILL:			OnMsgPlayerCastSkill(Msg);		break;
	case MSG_PM_CHANGENAMECOLOR:	OnMsgChangeNameColor(Msg);		break;
	case MSG_PM_PLAYEREXTSTATE:		OnMsgPlayerExtState(Msg);		break;
	case MSG_PM_ENCHANTRESULT:		OnMsgEnchantResult(Msg);		break;
	case MSG_PM_PLAYERADVDATA:		OnMsgPlayerAdvData(Msg);		break;
	case MSG_PM_PLAYERGATHER:		OnMsgPlayerGather(Msg);			break;
	case MSG_PM_PLAYERDOEMOTE:		OnMsgPlayerDoEmote(Msg);		break;
	case MSG_PM_DOCONEMOTE:			OnMsgDoConEmote(Msg);			break;
	case MSG_PM_PICKUPMATTER:		OnMsgPickupMatter(Msg);			break;
	case MSG_PM_PLAYERCHGSHAPE:		OnMsgPlayerChangeShape(Msg);	break;
	case MSG_PM_GOBLINOPT:			OnMsgPlayerGoblinOpt(Msg);		break;
	case MSG_PM_BOOTHOPT:			OnMsgBoothOperation(Msg);		break;
	case MSG_PM_PLAYERTRAVEL:		OnMsgPlayerTravel(Msg);			break;
	case MSG_PM_PLAYERPVP:			OnMsgPlayerPVP(Msg);			break;
	case MSG_PM_FASHIONENABLE:		OnMsgSwitchFashionMode(Msg);	break;
	case MSG_PM_PLAYEREFFECT:		OnMsgPlayerEffect(Msg);			break;
	case MSG_PM_PLAYERUSEITEM:		OnMsgPlayerUseItem(Msg);		break;
	case MSG_PM_CHANGEFACE:			OnMsgPlayerChangeFace(Msg);		break;
	case MSG_PM_PLAYERBINDOPT:		OnMsgPlayerBindOpt(Msg);		break;
	case MSG_PM_PLAYERMOUNT:		OnMsgPlayerMount(Msg);			break;
	case MSG_PM_PLAYERDUELOPT:		OnMsgPlayerDuelOpt(Msg);		break;
	case MSG_PM_PLAYERLEVEL2:		OnMsgPlayerLevel2(Msg);			break;
	case MSG_HST_PLAYERPROPERTY:	OnMsgPlayerProperty(Msg);		break;
	case MSG_HST_PASSWDCHECKED:		OnMsgPlayerPasswdChecked(Msg);	break;
	case MSG_PM_PLAYERKNOCKBACK:	OnMsgPlayerKnockback(Msg);		break;
	case MSG_PM_PLAYEREQUIPDISABLED: OnMsgPlayerEquipDisabled(Msg);	break;
	case MSG_PM_CONGREGATE:			OnMsgCongregate(Msg);			break;
	case MSG_PM_TELEPORT:				OnMsgTeleport(Msg);				break;
	case MSG_HST_FORCE_DATA:		OnMsgForce(Msg);				break;
	case MSG_PM_FORCE_CHANGED:		OnMsgForce(Msg);				break;
	case MSG_HST_INVENTORY_DETAIL:	OnMsgInventoryDetail(Msg);		break;
	case MSG_PM_MULTIOBJ_EFFECT:	OnMsgMultiobject(Msg);			break;	
	case MSG_HST_WEDDINGSCENE_INFO:	OnMsgWeddingSceneInfo(Msg);		break;
	case MSG_HST_ONLINEAWARD:		OnMsgOnlineAward(Msg);			break;
	case MSG_HST_PROFITTIME:		OnMsgProfitTime(Msg);			break;
	case MSG_HST_PVPNOPENALTY:		OnMsgChangePVPNoPenalty(Msg);	break;
	case MSG_HST_CONTINUECOMBOSKILL:OnMsgContinueComboSkill(Msg);	break;
		
	case MSG_HST_COUNTRY_NOTIFY:
	case MSG_PM_COUNTRY_CHANGED:
	case MSG_HST_ENTER_COUNTRYBATTLE:
	case MSG_HST_COUNTRYBATTLE_RESULT:
	case MSG_HST_COUNTRYBATTLE_SCORE:
	case MSG_HST_COUNTRYBATTLE_REVIVE_TIMES:
	case MSG_HST_COUNTRYBATTLE_CARRIER_NOTIFY:
	case MSG_HST_COUNTRYBATTLE_BECOME_CARRIER:
	case MSG_HST_COUNTRYBATTLE_PERSONAL_SCORE:
	case MSG_HST_COUNTRYBATTLE_FLAG_MSG_NOTIFY:
	case MSG_HST_COUNTRYBATTLE_INFO:
	case MSG_HST_COUNTRYBATTLE_STRONGHOLD_STATE:
	case MSG_HST_COUNTRYBATTLE_LIVESHOW:
		OnMsgCountry(Msg); break;
	
	case MSG_HST_TRICKBATTLE:
		OnMsgTrickBattle(Msg);
		break;

	case MSG_HST_DEFENSE_RUNE_ENABLE: OnMsgDefenseRuneEnable(Msg); break;
	case MSG_HST_CASHMONEYRATE:		OnMsgCashMoneyRate(Msg);		break;
	case MSG_HST_MERIDIANS_NOTIFY:		OnMsgMeridiansNotify(Msg);	break;
	case MSG_HST_MERIDIANS_RESULT:		OnMsgMeridiansResult(Msg);	break;
	case MSG_HST_STONECHANGEEND:	OnMsgStoneChangeEnd(Msg);break;
	case MSG_HST_KINGNOTIFY:
	case MSG_PM_KINGCHANGED:
		OnMsgKingChanged(Msg);
		break;
	case MSG_HST_TOUCHPOINT:		OnMsgTouchPoint(Msg);			break;
	case MSG_PM_TITLE:
	case MSG_HST_TITLE:
		OnMsgTitle(Msg);
		break;
	case MSG_HST_SIGNIN:
		OnMsgSignIn(Msg);
		break;
	case MSG_HST_USE_GIFTCARD:		OnMsgUseGiftCard(Msg);			break;
	case MSG_PM_REINCARNATION:
	case MSG_HST_REINCARNATION_TOME_INFO:
	case MSG_HST_REINCARNATION_TOME_ACTIVATE:
		OnMsgReincarnation(Msg);
		break;
	case MSG_HST_REALM_EXP:
	case MSG_PM_REALMLEVEL:
		OnMsgRealm(Msg);
		break;
	case MSG_HST_GENERALCARD:
		OnMsgGeneralCard(Msg);
		break;
	case MSG_HST_MONSTERSPIRIT_LEVEL:
		OnMsgMonsterSpiritLevel(Msg);
		break;
	case MSG_HST_RAND_MALL_SHOPPING_RES:
		OnMsgRandMallShopping(Msg);
		break;
	case MSG_PM_FACTION_PVP_MASK_MODIFY:
		OnMsgFactionPVPMaskModify(Msg);
		break;
	case MSG_HST_WORLD_CONTRIBUTION:
		OnMsgWorldContribution(Msg);
		break;
	case MSG_HST_CANINHERIT_ADDONS:
		OnMsgUpdateInheritableEquipAddons(Msg);
		break;
	case MSG_HST_CLIENT_SCREENEFFECT:
		OnMsgClientScreenEffect(Msg);
		break;
	case MSG_HST_COMBO_SKILL_PREPARE:
		OnMsgComboSkillPrepare(Msg);
		break;
	case MSG_HST_INSTANCE_REENTER_NOTIFY:
		CECInstanceReenter::Instance().OnNotify(*(S2C::cmd_instance_reenter_notify*)Msg.dwParam1);
		break;
	case MSG_HST_PRAY_DISTANCE_CHANGE:
		OnMsgPrayDistanceChange(Msg);
		break;
	}

	return true;
}

void CECHostPlayer::OnMsgPlayerExtState(const ECMSG& Msg)
{
	CECPlayer::OnMsgPlayerExtState(Msg);
	if( !m_aIconStates.empty() ){
		for( S2C::IconStates::const_iterator it = m_aIconStates.begin(); it != m_aIconStates.end(); ++it ){
			if( (*it).id == 287 ){
				CDlgIceThunderBall *pDlgIceThunderBall = static_cast<CDlgIceThunderBall *>( CECUIHelper::GetGameUIMan()->GetDialog("Win_IceThunderBall"));
				bool bHideIceThunderBall = CECUIHelper::GetGame()->GetConfigs()->GetGameSettings().bHideIceThunderBall;
				if( bHideIceThunderBall ){
					pDlgIceThunderBall->SetCurrentState((*it).param[0]);
					pDlgIceThunderBall->Show(false);
				}else{
					pDlgIceThunderBall->UpdateState(*it);
					break;
				}
			} 
		}
	}
}

//	Handle message MSG_GST_MOVE
void CECHostPlayer::OnMsgGstMove(const ECMSG& Msg)
{
	A3DVECTOR3 vDir = m_CameraCoord.GetDir();
	A3DVECTOR3 vRight = m_CameraCoord.GetRight();
	A3DVECTOR3 vMoveDir;

	switch (Msg.dwParam1)
	{
	case 0:	vMoveDir = vDir;	break;
	case 1:	vMoveDir = a3d_Normalize(vDir-vRight);	break;
	case 2:	vMoveDir = -vRight;	break;
	case 3:	vMoveDir = a3d_Normalize(-vDir-vRight);	break;
	case 4:	vMoveDir = -vDir;	break;
	case 5:	vMoveDir = a3d_Normalize(-vDir+vRight);	break;
	case 6:	vMoveDir = vRight;	break;
	case 7:	vMoveDir = a3d_Normalize(vDir+vRight);	break;
	}
	
	float fDist = 5.0f * g_pGame->GetTickTime() * 0.001f;
	vMoveDir *= fDist;
	m_CameraCoord.Move(vMoveDir);
}

//	Handle message MSG_GST_PITCH
void CECHostPlayer::OnMsgGstPitch(const ECMSG& Msg)
{
	CameraPitch(FIX16TOFLOAT((int)Msg.dwParam1));
}

//	Handle message MSG_GST_YAW
void CECHostPlayer::OnMsgGstYaw(const ECMSG& Msg)
{
	CameraYaw(FIX16TOFLOAT((int)Msg.dwParam1));
}

//	Handle message MSG_GST_MOVEABSUP
void CECHostPlayer::OnMsgGstMoveAbsUp(const ECMSG& Msg)
{
	float fDist = 5.0f * g_pGame->GetTickTime() * 0.001f;
	if (!Msg.dwParam1)
		fDist = -fDist;

	m_CameraCoord.Move(g_vAxisY * fDist);
}

void CECHostPlayer::OnMsgHstAutoMove(const ECMSG& Msg)
{
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		AP_ActionEvent(AP_EVENT_CANNOTMOVE);
		return;
	}

	if (!CanDo(CANDO_MOVETO))
	{
		AP_ActionEvent(AP_EVENT_CANNOTMOVE);
		return;
	}

	if(Msg.dwParam1 == 0)			// Start auto move
	{
		bool bMoveOK = false;

		//	Move destination
		bool bShowDlg = Msg.dwParam4 ? true : false;
		A3DVECTOR3 vMoveDest((float)((int)Msg.dwParam2), 0.0f, (float)((int)Msg.dwParam3));
		if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_MOVETOPOS))
		{
			CECHPWorkMove* pWorkMove = dynamic_cast<CECHPWorkMove*>(pWork);
			pWorkMove->SetDestination(CECHPWorkMove::DEST_2D, vMoveDest);
			pWorkMove->SetUseAutoMoveDialog(bShowDlg);
			bMoveOK = true;
		}
		else if (m_pWorkMan->CanStartWork(CECHPWork::WORK_MOVETOPOS))
		{
			//	If destination is too near, ignore it.
			A3DVECTOR3 vDist = vMoveDest - GetPos();
			if (vDist.MagnitudeH() > 0.5f)
			{
				CECHPWorkMove* pWork = (CECHPWorkMove*)m_pWorkMan->CreateWork(CECHPWork::WORK_MOVETOPOS);
				pWork->SetDestination(CECHPWorkMove::DEST_2D, vMoveDest);
				pWork->SetUseAutoMoveDialog(bShowDlg);
				m_pWorkMan->StartWork_p1(pWork);
				bMoveOK = true;
			}
		}

		if( !bMoveOK )
			AP_ActionEvent(AP_EVENT_CANNOTMOVE);
	}
	else if(Msg.dwParam1 == 1)		// Set auto move height & auto land state
	{
		CECHPWorkMove* pWork = dynamic_cast<CECHPWorkMove*>(m_pWorkMan->GetWork(CECHPWork::WORK_MOVETOPOS));
		if(!pWork)
		{
			return;
		}

		pWork->SetAutoHeight((float)((int)Msg.dwParam2));
		pWork->SetAutoLand( (Msg.dwParam3==1) ? true : false);		
	}
	else if(Msg.dwParam1 == 2)		// Stop auto move
	{
		CECHPWorkMove* pWork = dynamic_cast<CECHPWorkMove*>(m_pWorkMan->GetWork(CECHPWork::WORK_MOVETOPOS));
		if(!pWork)
		{
			ASSERT(false);
			return;
		}
	
		pWork->SetUseAutoMoveDialog(false);
		pWork->PressCancel();		
	}
	else if (Msg.dwParam1 == 3)	//	start auto move 2
	{
		const MsgDataAutoMove* pMsgDTId = dynamic_cast<const MsgDataAutoMove*>(Msg.pMsgData);
		
		A3DVECTOR3 vMoveDest(*(float*)&Msg.dwParam2, *(float*)&Msg.dwParam3, *(float*)&Msg.dwParam4);
		int targetId(-1);
		int taskId(-1);
		if (pMsgDTId){
			targetId = pMsgDTId->GetTargetID();
			taskId = pMsgDTId->GetTaskID();
		}

		if (CECHPWork *pWork = m_pWorkMan->GetWork(CECHPWork::WORK_MOVETOPOS))
		{
			CECHPWorkMove* pWorkMove = dynamic_cast<CECHPWorkMove*>(pWork);
			pWorkMove->SetDestination(CECHPWorkMove::DEST_AUTOPF, vMoveDest);
			pWorkMove->SetTaskNPCInfo(targetId,taskId);
			pWorkMove->SetUseAutoMoveDialog(false);
		}
		else if (m_pWorkMan->CanStartWork(CECHPWork::WORK_MOVETOPOS))
		{
			//	If destination is too near, ignore it.
			A3DVECTOR3 vDist = vMoveDest - GetPos();
			if (vDist.MagnitudeH() > 0.5f){
				CECHPWorkMove* pWork = (CECHPWorkMove*)m_pWorkMan->CreateWork(CECHPWork::WORK_MOVETOPOS);
				pWork->SetDestination(CECHPWorkMove::DEST_AUTOPF, vMoveDest);
				pWork->SetTaskNPCInfo(targetId, taskId);
				pWork->SetUseAutoMoveDialog(false);
				m_pWorkMan->StartWork_p1(pWork);
			}else if (targetId){
				CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->FindNPCByTempleID(targetId);
				if (pNPC && SelectTarget(pNPC->GetNPCID())){
					m_pWorkMan->StartWork_p1(m_pWorkMan->CreateNPCTraceWork(pNPC, taskId));
				}
			}
		}
	}
	else
	{
		ASSERT(0);
	}
}
void CECHostPlayer::OnMsgHstPushMove(const ECMSG& Msg)
{
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		return;
	}

	if (!CanDo(CANDO_MOVETO)) return;

	switch (Msg.dwParam1)
	{
	case 0:	m_dwMoveRelDir |= MD_FORWARD;				break;
	case 1:	m_dwMoveRelDir |= MD_FORWARD | MD_LEFT;		break;
	case 2:	m_dwMoveRelDir |= MD_LEFT;					break;
	case 3:	m_dwMoveRelDir |= MD_BACK | MD_LEFT;		break;
	case 4:	m_dwMoveRelDir |= MD_BACK;					break;
	case 5:	m_dwMoveRelDir |= MD_BACK | MD_RIGHT;		break;
	case 6:	m_dwMoveRelDir |= MD_RIGHT;					break;
	case 7:	m_dwMoveRelDir |= MD_FORWARD | MD_RIGHT;	break;
	case 8:	m_dwMoveRelDir |= MD_ABSUP;					break;
	case 9:	m_dwMoveRelDir |= MD_ABSDOWN;				break;
	}

	bool bPushMove = true;
	if (Msg.dwParam1 == 8 || Msg.dwParam1 == 9)
	{
		if (m_iMoveEnv != MOVEENV_AIR && m_iMoveEnv != MOVEENV_WATER)
			bPushMove = false;
	}

	if (bPushMove && !IsAboutToDie() && CanDo(CANDO_MOVETO))
	{
		if (m_pWorkMan->CanStartWork(CECHPWork::WORK_MOVETOPOS)){
			CECHPWorkMove* pNewWork = (CECHPWorkMove*)m_pWorkMan->CreateWork(CECHPWork::WORK_MOVETOPOS);
			pNewWork->SetDestination(CECHPWorkMove::DEST_PUSH, g_vOrigin);
			m_pWorkMan->StartWork_p1(pNewWork);
		}
	}
}

void CECHostPlayer::OnMsgHstPitch(const ECMSG& Msg)
{
	//	Turn camera
	CECCamera::ACTION_DATA ActData;
	memset(&ActData, 0, sizeof(CECCamera::ACTION_DATA));
	ActData.nDeltaX		= 0;
	ActData.nDeltaY		= -(int)Msg.dwParam1;
	ActData.nDeltaZ		= 0;
	ActData.vecDirChar	= GetDir();
	m_CameraCtrl.Action(&ActData);

	// we need to notify help system that the user has turned the camera, to keep the check state uptodate.
	CECScriptMan * pScriptMan = g_pGame->GetGameRun()->GetUIManager()->GetScriptMan();
	if( pScriptMan )
	{
		pScriptMan->GetContext()->GetCheckState()->SetPlayerStatusIsAdjustOrient(true);
	}
}

void CECHostPlayer::OnMsgHstYaw(const ECMSG& Msg)
{
	if (Msg.dwParam2)
	{
		CECConfigs* pConfig = g_pGame->GetConfigs();

		//	Turn player
		float fYaw = -(int)Msg.dwParam1 * pConfig->GetGameSettings().fCamTurnSpeed / 80.0f;

		A3DVECTOR3 vPos = GetPos();
		SetPos(g_vOrigin);
		RotateAxis(g_vAxisY, DEG2RAD(fYaw), false);
		SetPos(vPos);

		ChangeModelMoveDirAndUp(GetDir(), GetUp());
	}
	else
	{
		//	Turn camera
		CECCamera::ACTION_DATA ActData;
		memset(&ActData, 0, sizeof (CECCamera::ACTION_DATA));
		ActData.nDeltaX		= -(int)Msg.dwParam1;
		ActData.nDeltaY		= 0;
		ActData.nDeltaZ		= 0;
		ActData.vecDirChar	= GetDir();
		m_CameraCtrl.Action(&ActData);
	}

	// we need to notify help system that the user has turned the camera, to keep the check state uptodate.
	CECScriptMan * pScriptMan = g_pGame->GetGameRun()->GetUIManager()->GetScriptMan();
	if( pScriptMan )
	{
		pScriptMan->GetContext()->GetCheckState()->SetPlayerStatusIsAdjustOrient(true);
	}
}

//	Message MSG_HST_WHEELCAM handler
void CECHostPlayer::OnMsgHstWheelCam(const ECMSG& Msg)
{
	CECCamera::ACTION_DATA ActData;
	memset(&ActData, 0, sizeof(CECCamera::ACTION_DATA));
	ActData.nDeltaX		= 0;
	ActData.nDeltaY		= 0;
	ActData.nDeltaZ		= (int)Msg.dwParam1;
	ActData.vecDirChar	= GetDir();
	m_CameraCtrl.Action(&ActData);

	// we need to notify help system that the user has zoom in or out of the camera, to keep the check state uptodate.
	CECScriptMan * pScriptMan = g_pGame->GetGameRun()->GetUIManager()->GetScriptMan();
	if( pScriptMan )
	{
		pScriptMan->GetContext()->GetCheckState()->SetPlayerStatusIsAdjustDistance(true);
	}
}

void CECHostPlayer::OnMsgHstCamDefault(const ECMSG& Msg)
{
	CECCamera::ACTION_DATA ActData;
	memset(&ActData, 0, sizeof(CECCamera::ACTION_DATA));
	ActData.nAction = CECCamera::ACTION_DEFAULT;
	ActData.vecDirChar = GetDir();
	m_CameraCtrl.Action(&ActData);
}

void CECHostPlayer::OnMsgHstCamPreset(const ECMSG& Msg)
{
	CECCamera::ACTION_DATA ActData;
	memset(&ActData, 0, sizeof(CECCamera::ACTION_DATA));
	ActData.nAction = CECCamera::ACTION_PRESET;
	m_CameraCtrl.Action(&ActData);
}

void CECHostPlayer::OnMsgHstCamUserGet(const ECMSG& Msg)
{
	m_CameraCtrl.UserView_Retrieve(Msg.dwParam1);
}

void CECHostPlayer::OnMsgHstCamUserSet(const ECMSG& Msg)
{
	m_CameraCtrl.UserView_Define(Msg.dwParam1);
}

//	Message MSG_HST_ATKRESULT handler
void CECHostPlayer::OnMsgHstAttackResult(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_host_attack_result* pCmd = (cmd_host_attack_result*)Msg.dwParam1;
	ASSERT(pCmd);

	//	test code...
//	g_pGame->GetRTDebug()->OutputNotifyMessage(RTDCOL_WARNING, _AL("attack result!"));

	int iAttackTime = 0;
	PlayAttackEffect(pCmd->idTarget, 0, 0, pCmd->iDamage, pCmd->attack_flag, pCmd->attack_speed * 50, &iAttackTime);
	TurnFaceTo(pCmd->idTarget);

	if (iAttackTime)
	{
		//	Let melee work goto idle state so that attack effect can be played
		//	completely.
		if (CECHPWorkMelee* pCurWork = dynamic_cast<CECHPWorkMelee *>(m_pWorkMan->GetRunningWork(CECHPWork::WORK_HACKOBJECT))){
			pCurWork->SetIdleTime(iAttackTime);
		}
	}
}

//	Message MSG_HST_ATTACKED handler
void CECHostPlayer::OnMsgHstAttacked(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_host_attacked* pCmd = (cmd_host_attacked*)Msg.dwParam1;
	ASSERT(pCmd);

	if (pCmd->iDamage && (pCmd->cEquipment & 0x7f) != 0x7f)
	{
		char cEquip = pCmd->cEquipment & 0x7f;
		CECIvtrEquip* pEquip = (CECIvtrEquip*)m_pEquipPack->GetItem(cEquip);
		if (pEquip)
			pEquip->AddCurEndurance(ARMOR_RUIN_SPEED);
	}

	//	The host player is attacked, we should make an effect here
	if( ISPLAYERID(pCmd->idAttacker) )
	{
		CECElsePlayer * pAttacker = m_pPlayerMan->GetElsePlayer(pCmd->idAttacker);
		if (pAttacker)
		{
			if( !pAttacker->IsDead() )
			{
				//	Face to target
				pAttacker->TurnFaceTo(GetPlayerInfo().cid);
			}
			
			pAttacker->PlayAttackEffect(GetCharacterID(), 0, 0, pCmd->iDamage, pCmd->attack_flag, pCmd->speed * 50);
			pAttacker->EnterFightState();
		}
	}
	else if( ISNPCID(pCmd->idAttacker) )
	{
		CECNPC * pAttacker = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(pCmd->idAttacker);
		if (pAttacker)
		{
			pAttacker->OnMsgAttackHostResult(GetCharacterID(), pCmd->iDamage, pCmd->attack_flag, pCmd->speed);
		}
	}

	CECAutoPolicy::GetInstance().SendEvent_BeHurt(pCmd->idAttacker);
}

//	Message MSG_HST_SKILL_ATTACK_RESULT handler
void CECHostPlayer::OnMsgHstSkillResult(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_host_skill_attack_result* pCmd = (cmd_host_skill_attack_result*)Msg.dwParam1;
	ASSERT(pCmd);

	PlayAttackEffect(pCmd->idTarget, pCmd->idSkill, 0, pCmd->iDamage, pCmd->attack_flag, pCmd->attack_speed * 50, NULL, pCmd->section);
}

//	Message MSG_HST_SKILL_ATTACKED handler
void CECHostPlayer::OnMsgHstSkillAttacked(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_host_skill_attacked* pCmd = (cmd_host_skill_attacked*)Msg.dwParam1;
	ASSERT(pCmd);

	if (pCmd->iDamage && (pCmd->cEquipment & 0x7f) != 0x7f)
	{
		char cEquip = pCmd->cEquipment & 0x7f;
		CECIvtrEquip* pEquip = (CECIvtrEquip*)m_pEquipPack->GetItem(cEquip);
		if (pEquip)
			pEquip->AddCurEndurance(ARMOR_RUIN_SPEED);
	}

	//	The host player is attacked, we should make an effect here
	if( ISPLAYERID(pCmd->idAttacker) )
	{
		CECElsePlayer * pAttacker = m_pPlayerMan->GetElsePlayer(pCmd->idAttacker);
		if (pAttacker)
		{
			if( !pAttacker->IsDead() )
			{
				//	Face to target
				pAttacker->TurnFaceTo(GetPlayerInfo().cid);
			}

			pAttacker->PlayAttackEffect(GetCharacterID(), pCmd->idSkill, 0, pCmd->iDamage, pCmd->attack_flag, pCmd->speed * 50,NULL,pCmd->section);
			pAttacker->EnterFightState();
		}
	}
	else if( ISNPCID(pCmd->idAttacker) )
	{
		CECNPC * pAttacker = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(pCmd->idAttacker);
		if (pAttacker)
		{
			if( !pAttacker->IsDead())
			{
				//	Face to target
				pAttacker->NPCTurnFaceTo(GetPlayerInfo().cid);
			}
			pAttacker->PlayAttackEffect(GetCharacterID(), pCmd->idSkill, 0, pCmd->iDamage, pCmd->attack_flag, pCmd->speed,pCmd->section);
		}
	}

	CECAutoPolicy::GetInstance().SendEvent_BeHurt(pCmd->idAttacker);
}

void CECHostPlayer::OnMsgHstAskToJoinTeam(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_team_ask_join* pCmd = (cmd_team_ask_join*)Msg.dwParam1;
	ASSERT(pCmd);

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	ACString strMsg;
	CECPlayer *pPlayer = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(pCmd->idAsker);
	EC_GAME_SETTING m_setting = g_pGame->GetConfigs()->GetGameSettings();

	if( pGameUI->PlayerIsBlack(pCmd->idAsker) || m_setting.bNoTeamRequest )
		return;
	
	if( pPlayer )
	{
		strMsg.Format(pGameUI->GetStringFromTable(686), g_pGame->GetGameRun()->GetPlayerName(pCmd->idAsker, true),
			g_pGame->GetGameRun()->GetProfName(pPlayer->GetProfession()));
	}
	else
	{
		strMsg.Format(pGameUI->GetStringFromTable(686), g_pGame->GetGameRun()->GetPlayerName(pCmd->idAsker, true),
			_AL(""));
	}

	pGameUI->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_AskToJoin", 
		strMsg, 30000, pCmd->idAsker, 0, 0);
}

//	Message MSG_HST_DIED handler
void CECHostPlayer::OnMsgHstDied(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_host_died* pCmd = (cmd_host_died*)Msg.dwParam1;

	if( !m_bAboutToDie )
		Killed(pCmd->idKiller);
}

//	Message MSG_HST_PICKUPMONEY handler
void CECHostPlayer::OnMsgHstPickupMoney(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_pickup_money* pCmd = (cmd_pickup_money*)Msg.dwParam1;
	ASSERT(pCmd);

	//	Add money amount
	AddMoneyAmount(pCmd->iAmount);

	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_PICKUPMONEY, pCmd->iAmount);

	//	Popup notify bubble text
	BubbleText(BUBBLE_MONEY, pCmd->iAmount);
}

//	Message MSG_HST_PICKUPITEM handler
void CECHostPlayer::OnMsgHstPickupItem(const ECMSG& Msg)
{
	using namespace S2C;

	bool bDoOther = false;
	int idItem, iExpireDate=0, iAmount, iCmdLastSlot, iCmdSlotAmount, iPack, iMsg=-1;

	switch (Msg.dwParam2)
	{
	case PICKUP_ITEM:
	{
		cmd_pickup_item* pCmd = (cmd_pickup_item*)Msg.dwParam1;
		ASSERT(pCmd);

		idItem			= pCmd->tid;
		iExpireDate		= pCmd->expire_date;
		iAmount			= pCmd->iAmount;
		iCmdLastSlot	= pCmd->bySlot;
		iCmdSlotAmount	= pCmd->iSlotAmount;
		iPack			= pCmd->byPackage;
		iMsg			= FIXMSG_PICKUPITEM;
		break;
	}
	case HOST_OBTAIN_ITEM:
	{
		cmd_host_obtain_item* pCmd = (cmd_host_obtain_item*)Msg.dwParam1;
		ASSERT(pCmd);

		idItem			= pCmd->type;
		iExpireDate		= pCmd->expire_date;
		iAmount			= pCmd->amount;
		iCmdLastSlot	= pCmd->index;
		iCmdSlotAmount	= pCmd->slot_amount;
		iPack			= pCmd->where;
		iMsg			= FIXMSG_GAINITEM;
		break;
	}
	case PRODUCE_ONCE:
	{
		cmd_produce_once* pCmd = (cmd_produce_once*)Msg.dwParam1;
		ASSERT(pCmd);

		idItem			= pCmd->type;
		iExpireDate		= 0;
		iAmount			= pCmd->amount;
		iCmdLastSlot	= pCmd->index;
		iCmdSlotAmount	= pCmd->slot_amount;
		iPack			= pCmd->where;
		iMsg			= FIXMSG_PRODUCEITEM;

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_PRODUCE_END_ONE, pCmd);
		break;
	}
	case TASK_DELIVER_ITEM:
	{
		cmd_task_deliver_item* pCmd = (cmd_task_deliver_item*)Msg.dwParam1;
		ASSERT(pCmd);

		idItem			= pCmd->type;
		iExpireDate		= pCmd->expire_date;
		iAmount			= pCmd->amount;
		iCmdLastSlot	= pCmd->index;
		iCmdSlotAmount	= pCmd->slot_amount;
		iPack			= pCmd->where;
		iMsg			= FIXMSG_GAINITEM;
		bDoOther = true;
		break;
	}
	default:
	{
		ASSERT(0);
		return;
	}
	}

	CECInventory* pInventory = GetPack(iPack);
	if (!pInventory)
	{
		ASSERT(0);
		return;
	}
	
	int iLastSlot, iSlotNum;
	if (!pInventory->MergeItem(idItem, iExpireDate, iAmount, &iLastSlot, &iSlotNum) ||
		iLastSlot != iCmdLastSlot || iSlotNum != iCmdSlotAmount)
	{
		//	TODO: !! Local inventory data lose synchronization with server,
		//	so we have to re-get all inventory data from server.
		ASSERT(0);
		return;
	}

	if (HOST_OBTAIN_ITEM == Msg.dwParam2 && IVTRTYPE_PACK == iPack){
		CECShoppingManager::Instance().OnObtainItem(iPack, idItem, iAmount);
	}

	//	If item is equipment we have to get it's detail information, 
	//	otherwise it may be shown as "unable to be equipped"
	CECIvtrItem* pItem = pInventory->GetItem(iCmdLastSlot);
	ASSERT(pItem);
	if (pItem)
	{
		if (pItem->IsEquipment() || pItem->GetClassID() == CECIvtrItem::ICID_TASKNMMATTER ||
			pItem->GetClassID() == CECIvtrItem::ICID_TASKDICE || pItem->GetClassID() == CECIvtrItem::ICID_TASKITEM ||
			pItem->GetClassID() == CECIvtrItem::ICID_GOBLIN_EXPPILL ||
			pItem->GetClassID() == CECIvtrItem::ICID_WEDDINGBOOKCARD ||
			pItem->GetClassID() == CECIvtrItem::ICID_WEDDINGINVITECARD ||
			pItem->GetClassID() == CECIvtrItem::ICID_SKILLTOME )
			g_pGame->GetGameSession()->c2s_CmdGetItemInfo(iPack, iCmdLastSlot);

		if (pItem->GetClassID() == CECIvtrItem::ICID_GENERALCARD)
			m_CardsToMove.insert(iCmdLastSlot);

		//	Print a notice
		if (iMsg >= 0)
			g_pGame->GetGameRun()->AddFixedMessage(iMsg, iAmount, pItem->GetName());
	}
	CECUIHelper::OnTaskItemGained(idItem);

	//  对打怪获取物品的任务发起自动组队
	if( bDoOther )
	{
		m_pTaskInterface->DoAutoTeamForTask(idItem);
	}
}

//	Message MSG_HST_RECEIVEEXP handler
void CECHostPlayer::OnMsgHstReceiveExp(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_receive_exp* pCmd = (cmd_receive_exp*)Msg.dwParam1;
	ASSERT(pCmd);

	// 此处不需要加经验和元神，因为服务器在发了RECEIVE_EXP之后会接着发SELF_INFO_00
//	m_BasicProps.iExp	+= pCmd->exp;
//	m_BasicProps.iSP	+= pCmd->sp;
	if (m_ReincarnationTome.tome_active) {
		m_ReincarnationTome.tome_exp += pCmd->exp;
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		CDlgReincarnationBook* pDlgBook = dynamic_cast<CDlgReincarnationBook*>(pGameUI->GetDialog("Win_ReincarnationBook"));
		if (pDlgBook && pDlgBook->IsShow()) pDlgBook->Update();
		CDlgReincarnationRewrite* pDlgRewrite = dynamic_cast<CDlgReincarnationRewrite*>(pGameUI->GetDialog("Win_ReincarnationRewrite"));
		if (pDlgRewrite && pDlgRewrite->IsShow()) pDlgRewrite->Update();
	}

	if (pCmd->exp)
	{
		//	Print notify text
	//	g_pGame->GetGameRun()->AddFixedMessage(pCmd->exp > 0 ? FIXMSG_GOTEXP : FIXMSG_LOSTEXP, (int)pCmd->exp);
		BubbleText(BUBBLE_EXP, pCmd->exp);
	}

	if (pCmd->sp > 0)
	{
		//	Print notify text
	//	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOTSP, (int)pCmd->sp);
		BubbleText(BUBBLE_SP, pCmd->sp);
	}
}

//	Message MSG_HST_Info00 handler
void CECHostPlayer::OnMsgHstInfo00(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_self_info_00* pCmd = (cmd_self_info_00*)Msg.dwParam1;
	ASSERT(pCmd);

	bool bFirstTime = !m_BasicProps.iLevel ? true : false;
	if (!bFirstTime)
	{
		int iLimit = (int)(pCmd->iMaxHP * 0.3f);
		if (pCmd->iHP < m_BasicProps.iCurHP && m_BasicProps.iCurHP >= iLimit && pCmd->iHP < iLimit) {
			BubbleText(BUBBLE_HPWARN, 0);
			if (CECUIHelper::GetGameUIMan()->IsShowLowHP()) {
				// 血量低于临界值则播放特效
				const int GfxLastTime = 10000;		// 持续时间10秒
				CECUIHelper::GetGameUIMan()->GetScreenEffectMan()->StartEffect(CECScreenEffect::EFFECT_REDSPARK, GfxLastTime);
			}
		}

		if (pCmd->iHP >= iLimit || pCmd->iHP <= 0) {
			// 血量高于临界值或死亡，则停止播放特效
			CECUIHelper::GetGameUIMan()->GetScreenEffectMan()->FinishEffect(CECScreenEffect::EFFECT_REDSPARK);
		}

		iLimit = (int)(pCmd->iMaxMP * 0.2f);
		if (pCmd->iMP < m_BasicProps.iCurMP && m_BasicProps.iCurMP >= iLimit && pCmd->iMP < iLimit)
			BubbleText(BUBBLE_MPWARN, 0);

		if (m_ExtProps.max_ap != pCmd->iMaxAP)
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ADDMAXAP, pCmd->iMaxAP - m_ExtProps.max_ap);
	}

	m_BasicProps.iLevel		= pCmd->sLevel;
	SetLevel2(pCmd->Level2, bFirstTime);
	m_BasicProps.iExp		= pCmd->iExp;
	m_BasicProps.iSP		= pCmd->iSP;
	m_BasicProps.iCurHP		= pCmd->iHP;
	m_BasicProps.iCurMP		= pCmd->iMP;
	m_BasicProps.iCurAP		= pCmd->iAP;
	m_ExtProps.bs.max_hp	= pCmd->iMaxHP;
	m_ExtProps.bs.max_mp	= pCmd->iMaxMP;
	m_ExtProps.max_ap		= pCmd->iMaxAP;
	
	if (pCmd->State && m_bFight == false) PlayEnterBattleGfx();
	m_bFight = pCmd->State ? true : false;
	
	UpdateGodEvilSprite();

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgAutoHelp *pDlgHelp = dynamic_cast<CDlgAutoHelp *>(pGameUI->GetDialog("Win_WikiPop"));
	if(pDlgHelp && m_bFight)
		pDlgHelp->SetAutoHelpState(false);
}

//	Message MSG_HST_GOTO handler
void CECHostPlayer::OnMsgHstGoto(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_notify_hostpos* pCmd = (cmd_notify_hostpos*)Msg.dwParam1;
	ASSERT(pCmd);

	int idInst = pCmd->tag;
	A3DVECTOR3 vPos = pCmd->vPos;
	int	iLine = pCmd->line;
	
	if (CECAutoPolicy::GetInstance().IsAutoPolicyEnabled()){
		CECAutoPolicy::GetInstance().StopPolicy();
	}
	
	//	战略图场景(143)内、Win_CountryMap 显示时位置变动不即时生效
	//	可保证 Win_CountryMap 上各领土间流畅移动、
	//	以及避免玩家由于频繁加载场景资源而无法及时响应 CountryBattlePreEnterNotify，从而有机会无法进入国战战场副本(144)
	//	
	CECGameUIMan * pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgCountryMap *pDlgCountryMap = dynamic_cast<CDlgCountryMap *>(pGameUIMan->GetDialog("Win_CountryMap"));
	bool bSaved = pDlgCountryMap->SaveLoadCenter(idInst, vPos, iLine);
	if (!bSaved && !Goto(idInst, vPos, iLine))
		return;

	//	Notify our position to other teammates if we are in a team
	if (m_pTeam)
		g_pGame->GetGameSession()->c2s_CmdTeamNotifyOwnPos();

	// now clear exit instance count down flags
	pGameUIMan->PopupInstanceCountDown(0);
	pGameUIMan->SwitchCountryChannel();
	m_nTimeToExitInstance = 0;	

	// 战车：变形加载模型还没有完成时，跳地图会释放playerMan，不执行SetLoadResult函数，导致角色没有模型。
	if (GetBattleInfo().IsChariotWar())
	{
		QueueLoadDummyModel(m_iShape,false);
	}

	if( m_pAutoTeam )
		m_pAutoTeam->OnWorldChanged();

	if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
	{
		// 响应回城术完成
		CECAutoPolicy::GetInstance().SendEvent_ReturnTown();
	}
}

bool CECHostPlayer::Goto(int idInst, A3DVECTOR3 vPos, int iParallelWorldID)
{
	if (!g_pGame->GetGameRun()->JumpToInstance(idInst, vPos, iParallelWorldID))
	{
		a_LogOutput(1, "CECGameRun::Goto, Failed to jump to instance %d", idInst);
		return false;
	}
	
	//	Stop all current work and goto specified position
	if (m_pWorkMan){
		if (IsAutoMoving())
		{
			CECHPWorkMove* pWorkMove = dynamic_cast<CECHPWorkMove*>(m_pWorkMan->GetRunningWork(CECHPWork::WORK_MOVETOPOS));
			pWorkMove->SetUseAutoMoveDialog(false);
			pWorkMove->PressCancel();
			pWorkMove->Finish();
		}	
		m_pWorkMan->FinishAllWork(true);
	}
	
	//	In some instances flying is forbidden, server should has notified host to fall
	//	down before he enter these instances (before GOTO message is sent). 
	//	The problem is that wing or flysword won't disappear until host touch
	//	ground, so when host fly into these	'no-flying' instances, he may stand on
	//	ground still has wing or flysword !
	if (!IsFlying())
		ShowWing(false);
	
	//	Add a little height to ensure player's AABB won't embed with building
	vPos += g_vAxisY * 0.1f;
	
	//	Ensure we are not under ground
	A3DVECTOR3 vNormal;
	float vTerrainHeight = g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(vPos, &vNormal);
	if (vPos.y < vTerrainHeight)
		vPos.y = vTerrainHeight;
	
	SetPos(vPos);
	
	m_CDRInfo.vTPNormal	= vPos.y <= vTerrainHeight + 0.1f ? vNormal : g_vOrigin;
	m_CDRInfo.fYVel		= 0.0f;
	m_CDRInfo.vAbsVelocity.Clear();
	
	ResetJump();
	
	m_MoveCtrl.SetHostLastPos(vPos);
	m_MoveCtrl.SetLastSevPos(vPos);
	
	//	Update camera
	UpdateFollowCamera(false, 10);

	return true;
}

//	Message MSG_HST_OWNITEMINFO handler
void CECHostPlayer::OnMsgHstOwnItemInfo(const ECMSG& Msg)
{
	using namespace S2C;
	
	if (Msg.dwParam2 == OWN_ITEM_INFO)
	{
		cmd_own_item_info* pCmd = (cmd_own_item_info*)Msg.dwParam1;
		ASSERT(pCmd);

		CECInventory* pInventory = GetPack(pCmd->byPackage);
		CECIvtrItem* pItem = NULL;
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (!pInventory || !(pItem = pInventory->GetItem(pCmd->bySlot)))
		{
			ASSERT(0);
			return;
		}

		pItem->SetExpireDate(pCmd->expire_date);
		pItem->SetProcType(pCmd->state);
		pItem->SetAmount(pCmd->count);
		if( pCmd->content && pCmd->content_length )
			pItem->SetItemInfo((BYTE*) &pCmd->content, pCmd->content_length);
		else 
			((CECIvtrItem *)pItem)->SetItemInfo(NULL, 0);
		
		if (pCmd->byPackage == IVTRTYPE_EQUIPPACK)
		{
			//	更新磨刀石等更新武器等装备带来的光效
			UpdateEquipSkins();
			//  更新卡牌界面
			CDlgGeneralCard* pDlg = dynamic_cast<CDlgGeneralCard*>(pGameUI->GetDialog("Win_GeneralCard"));
			if (pDlg && pDlg->IsShow()) pDlg->Update();
		}
		else if (pCmd->byPackage == IVTRTYPE_PACK)
		{
			if (pItem->IsEquipment())
			{
				
				CDlgEquipDye *pDlgDye = dynamic_cast<CDlgEquipDye *>(pGameUI->GetDialog("Win_EquipDye"));
				if (!pDlgDye->IsShow())
					pDlgDye = dynamic_cast<CDlgEquipDye *>(pGameUI->GetDialog("Win_AllEquipDye"));
				if (pDlgDye->IsShow())
					pDlgDye->OnEquipChange(pItem, pCmd->bySlot);
			}
		}
		
		//  Update goblin
		if( m_pGoblin && pItem->GetClassID() == CECIvtrItem::ICID_GOBLIN )
			UpdateGoblin();

		if (!MoveCardAuto(pItem, pCmd->byPackage, pCmd->bySlot)){
			CECShoppingItemsMover::Instance().MoveItem(pItem, pCmd->byPackage, pCmd->bySlot);
		}
	}
	else if (Msg.dwParam2 == EMPTY_ITEM_SLOT)
	{
		cmd_empty_item_slot* pCmd = (cmd_empty_item_slot*)Msg.dwParam1;
		ASSERT(pCmd);

		CECInventory* pInventory = GetPack(pCmd->byPackage);
		if (pInventory)
		{
			//	Update shortcuts
			CECIvtrItem* pItem = pInventory->GetItem(pCmd->bySlot);
			if (pItem)
				UpdateRemovedItemSC(pItem->GetTemplateID(), pCmd->byPackage, pCmd->bySlot);

			pInventory->SetItem(pCmd->bySlot, NULL);
		}
	}
}

//	Message MSG_HST_SELTARGET handler
void CECHostPlayer::OnMsgHstSelTarget(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == SELECT_TARGET)
	{
		cmd_select_target* pCmd = (cmd_select_target*)Msg.dwParam1;
		ASSERT(pCmd);

		m_idSelTarget	= pCmd->idTarget;
		m_idUCSelTarget	= 0;

		//	Test code ...
		g_pGame->RuntimeDebugInfo(0xffffff00, _AL("Select 0x%x(%d)"), m_idSelTarget, m_idSelTarget);
	}
	else if (Msg.dwParam2 == UNSELECT)
	{
		m_idSelTarget = 0;
		g_pGame->RuntimeDebugInfo(0xffffff00, _AL("UnSelect"));
	}
}

//	Message MSG_HST_FIXCAMERA handler
void CECHostPlayer::OnMsgHstFixCamera(const ECMSG& Msg)
{
	m_CameraCtrl.Fix_Set(!m_CameraCtrl.Fix_Get());
}

void CECHostPlayer::OnMsgHstExtProp(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_own_ext_prop* pCmd = (cmd_own_ext_prop*)Msg.dwParam1;
	ASSERT(pCmd);

	m_ExtProps = pCmd->prop;
	m_BasicProps.iStatusPt = (int)pCmd->status_point;
	m_BasicProps.iAtkDegree = pCmd->attack_degree;
	m_BasicProps.iDefDegree = pCmd->defend_degree;
	m_BasicProps.iCritRate	= pCmd->crit_rate;
	m_BasicProps.iCritDamageBonus = pCmd->crit_damage_bonus;
	m_BasicProps.iInvisibleDegree = pCmd->invisible_degree;
	m_BasicProps.iAntiInvisibleDegree = pCmd->anti_invisible_degree;
	m_BasicProps.iPenetration = pCmd->penetration;
	m_BasicProps.iResilience = pCmd->resilience;
	m_BasicProps.iVigour = pCmd->vigour;
}

void CECHostPlayer::OnMsgHstAddStatusPt(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_add_status_point* pCmd = (cmd_add_status_point*)Msg.dwParam1;
	ASSERT(pCmd);

	m_BasicProps.iStatusPt	= (int)pCmd->remain;
	m_ExtProps.bs.vitality += (int)pCmd->vitality;
	m_ExtProps.bs.energy   += (int)pCmd->energy;
	m_ExtProps.bs.strength += (int)pCmd->strength;
	m_ExtProps.bs.agility  += (int)pCmd->agility;

	//	Get extend properties
	g_pGame->GetGameSession()->c2s_CmdGetExtProps();
}

void CECHostPlayer::OnMsgHstJoinTeam(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_team_join_team* pCmd = (cmd_team_join_team*)Msg.dwParam1;
	ASSERT(pCmd);

	CECTeamMan* pTeamMan = g_pGame->GetGameRun()->GetTeamMan();

	//	Try to get team at first
	CECTeam* pTeam = pTeamMan->GetTeam(pCmd->idLeader);
	if (!pTeam)
	{
		//	Create a new team
		if (!(pTeam = pTeamMan->CreateTeam(pCmd->idLeader)))
		{
			glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECTeamMan::OnMsgHstJoinTeam", __LINE__);
			return;
		}
	}

	//	If host player is leader, add heself into team
	if (pCmd->idLeader == m_PlayerInfo.cid)
		pTeam->AddMember(pCmd->idLeader);
	
	pTeam->SetPickupFlag(pCmd->wPickFlag);

	m_pTeam = pTeam;
}

void CECHostPlayer::OnMsgHstLeaveTeam(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_team_leave_party* pCmd = (cmd_team_leave_party*)Msg.dwParam1;
	ASSERT(pCmd);
	
	if (!m_pTeam)
	{
		ASSERT(m_pTeam);
		return;
	}

	if (pCmd->idLeader != m_pTeam->GetLeaderID())
	{
		// A、B、C三人组队，A为队长，A、B同时下线，导致C收到此协议的idLeader与本地不同
		// 这里暂时注掉，依然执行ReleaseTeam。[8/12/2013 Shizhenhua]
		ASSERT(pCmd->idLeader == m_pTeam->GetLeaderID());
		//return;	
	}

	//	In current game, we don't care those teams which doesn't include host
	//	player, so if only host leave team, delete the team immediately !
	CECTeamMan* pTeamMan = g_pGame->GetGameRun()->GetTeamMan();
	pTeamMan->ReleaseTeam(pCmd->idLeader);
	m_pTeam = NULL;

	if (pCmd->reason != GP_LTR_LEAVE)
	{
		//	Display the reason
		int iMsg = (pCmd->reason == GP_LTR_KICKEDOUT) ? FIXMSG_KICKEDOUTTEAM : FIXMSG_CANCELTEAM;
		g_pGame->GetGameRun()->AddFixedMessage(iMsg);
	}

	//	Update team UI
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->UpdateTeam();
}

void CECHostPlayer::OnMsgHstNewTeamMem(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_team_new_member* pCmd = (cmd_team_new_member*)Msg.dwParam1;
	ASSERT(pCmd);

	if (!m_pTeam)
	{
		ASSERT(m_pTeam);
		return;
	}

	CECGameRun* pGameRun = g_pGame->GetGameRun();
	int cid = pCmd->idMember;

	//	Print notify message
	m_pTeam->AddMember(cid);
	const ACHAR* szName = pGameRun->GetPlayerName(cid, false);
	if (szName)
		pGameRun->AddFixedMessage(FIXMSG_JOINTEAM, szName);
	else
		m_pTeam->AddUnknownID(cid);

	//	If this member isn't near host, get his/her position
	CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(cid);
	if (!pPlayer)
		g_pGame->GetGameSession()->c2s_CmdTeamMemberPos(1, &cid);
	else
	{
		CECTeamMember* pMem = m_pTeam->GetMemberByID(cid);
		if (pMem)
			pMem->SetPos(pGameRun->GetWorld()->GetInstanceID(), pPlayer->GetPos(), true);
	}
	
	//	Update team UI
	CECGameUIMan* pGameUI = pGameRun->GetUIManager()->GetInGameUIMan();
	pGameUI->UpdateTeam();
}

void CECHostPlayer::OnMsgHstItemOperation(const ECMSG& Msg)
{
	using namespace S2C;

	switch (Msg.dwParam2)
	{
	case EXG_IVTR_ITEM:
	{
		cmd_exg_ivtr_item* pCmd = (cmd_exg_ivtr_item*)Msg.dwParam1;
		ASSERT(pCmd);
		m_pPack->ExchangeItem(pCmd->index1, pCmd->index2);

		//	Update shortcuts
		CECIvtrItem* pItem1 = m_pPack->GetItem(pCmd->index1);
		CECIvtrItem* pItem2 = m_pPack->GetItem(pCmd->index2);
		if (pItem1 && pItem2)
			UpdateExchangedItemSC(pItem1->GetTemplateID(), IVTRTYPE_PACK, pCmd->index2, pItem2->GetTemplateID(), IVTRTYPE_PACK, pCmd->index1);
		else if (pItem1)
			UpdateMovedItemSC(pItem1->GetTemplateID(), IVTRTYPE_PACK, pCmd->index2, IVTRTYPE_PACK, pCmd->index1);
		else if (pItem2)
			UpdateMovedItemSC(pItem2->GetTemplateID(), IVTRTYPE_PACK, pCmd->index1, IVTRTYPE_PACK, pCmd->index2);

		break;
	}
	case MOVE_IVTR_ITEM:
	{
		cmd_move_ivtr_item* pCmd = (cmd_move_ivtr_item*)Msg.dwParam1;
		ASSERT(pCmd);
		m_pPack->MoveItem(pCmd->src, pCmd->dest, pCmd->count);

		//	Update shortcuts
		CECIvtrItem* pItem = m_pPack->GetItem(pCmd->dest);
		if (pItem)
			UpdateMovedItemSC(pItem->GetTemplateID(), IVTRTYPE_PACK, pCmd->src, IVTRTYPE_PACK, pCmd->dest);

		break;
	}
	case PLAYER_DROP_ITEM:
	{
		cmd_player_drop_item* pCmd = (cmd_player_drop_item*)Msg.dwParam1;
		ASSERT(pCmd);
		CECInventory* pPack = GetPack(pCmd->byPackage);
		if (!pPack) break;

		//	Keep item information for updating shortcuts later
		CECIvtrItem* pItem = pPack->GetItem(pCmd->bySlot);
		if (pItem && pItem->GetTemplateID() == pCmd->tid)
		{
			//	Print notify message
			switch (pCmd->reason)
			{
			case GP_DROP_GM:
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GM_REMOVESPECITEM, pCmd->count, pItem->GetName());
				break;

			case GP_DROP_DEATH:	
				
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_LOSEITEM, pCmd->count, pItem->GetName());
				break;

			case GP_DROP_TAKEOUT:
			case GP_DROP_TASK:
			case GP_DROP_USED:

				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ITEMDISAPPEAR, pCmd->count, pItem->GetName());
				break;

			case GP_DROP_EXPIRED:
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ITEMEXPIRED, pCmd->count, pItem->GetName());
				break;
			}

			//	Remove item from pack
			pPack->RemoveItem(pCmd->bySlot, pCmd->count);

			//	Update shortcuts if all items have been dropped
			if (!pPack->GetItem(pCmd->bySlot))
				UpdateRemovedItemSC(pCmd->tid, pCmd->byPackage, pCmd->bySlot);
		}
		else if (pCmd->tid >= 0)
		{
			// just get the name of the item and output the messages here.
			pItem = CECIvtrItem::CreateItem(pCmd->tid, 0, 1);
			//	Print notify message
			switch (pCmd->reason)
			{
			case GP_DROP_DEATH:	
				
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_LOSEITEM, pCmd->count, pItem->GetName());
				break;

			case GP_DROP_TAKEOUT:
			case GP_DROP_TASK:
			case GP_DROP_USED:

				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ITEMDISAPPEAR, pCmd->count, pItem->GetName());
				break;

			case GP_DROP_EXPIRED:
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ITEMEXPIRED, pCmd->count, pItem->GetName());
				break;
			}

			delete pItem;
			pItem = NULL;
		}

		if (pCmd->byPackage == IVTRTYPE_EQUIPPACK)
		{
			//	Update equipment skins
			UpdateEquipSkins();

			//	Update player's properties.
			//	TODO: use more cheap updating commands
			g_pGame->GetGameSession()->c2s_CmdGetExtProps();
		}

		if (pCmd->byPackage == IVTRTYPE_PACK)
		{
			//	物品更换可能导致交易相关包裹内物品失效，检查并验证
			ValidatePackItemPointer();
		}

		break;
	}
	case EXG_EQUIP_ITEM:
	{
		cmd_exg_equip_item* pCmd = (cmd_exg_equip_item*)Msg.dwParam1;
		ASSERT(pCmd);
		m_pEquipPack->ExchangeItem(pCmd->index1, pCmd->index2);

		//	Update shortcuts
		CECIvtrItem* pItem1 = m_pEquipPack->GetItem(pCmd->index1);
		CECIvtrItem* pItem2 = m_pEquipPack->GetItem(pCmd->index2);
		if (pItem1 && pItem2)
			UpdateExchangedItemSC(pItem1->GetTemplateID(), IVTRTYPE_EQUIPPACK, pCmd->index2, pItem2->GetTemplateID(), IVTRTYPE_EQUIPPACK, pCmd->index1);
		else if (pItem1)
			UpdateMovedItemSC(pItem1->GetTemplateID(), IVTRTYPE_EQUIPPACK, pCmd->index2, IVTRTYPE_EQUIPPACK, pCmd->index1);
		else if (pItem2)
			UpdateMovedItemSC(pItem2->GetTemplateID(), IVTRTYPE_EQUIPPACK, pCmd->index1, IVTRTYPE_EQUIPPACK, pCmd->index2);

		//	Update equipment skins
		UpdateEquipSkins();
		break;
	}
	case EQUIP_ITEM:
	{
		cmd_equip_item* pCmd = (cmd_equip_item*)Msg.dwParam1;
		ASSERT(pCmd);

		CECIvtrItem* pItem1 = m_pPack->GetItem(pCmd->index_inv, true);
		CECIvtrItem* pItem2;
		
		if( pItem1 && pItem1->GetClassID() == CECIvtrItem::ICID_GOBLIN_EQUIP)
		{
			if( m_pGoblin )
			{
				pItem2 = ((CECHostGoblin*)m_pGoblin)->GetEquipment()->PutItem(pCmd->index_equip, pItem1);
			}
			else
			{
				ASSERT(0);
				return;
			}
		}
		else
			pItem2 = m_pEquipPack->PutItem(pCmd->index_equip, pItem1);

		m_pPack->SetItem(pCmd->index_inv, pItem2);

		if(pCmd->index_equip == EQUIPIVTR_GOBLIN)
		{
			if( m_pGoblin )
			{
				m_pGoblin->Release();
				delete m_pGoblin;
				m_pGoblin = NULL;
			}


			if( pItem1 != NULL)
			{
				CECGameUIMan* pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if(pGameUIMan->GetDialog("Win_Transaction1"))
					pGameUIMan->GetDialog("Win_Transaction1")->Show(false);
		
				if(pGameUIMan->GetDialog("Win_Transaction2"))
					pGameUIMan->GetDialog("Win_Transaction2")->Show(false);
				
				m_pGoblin = new CECHostGoblin();
				CECIvtrGoblin* pIvtrGoblin = (CECIvtrGoblin*)pItem1;
				m_pGoblin->Init(pIvtrGoblin->GetTemplateID(), pIvtrGoblin, this);
			}
			
		}

		//	Synchronize item count
		if (!pCmd->count_equip)
		{
			if (pItem1)
				UpdateRemovedItemSC(pItem1->GetTemplateID(), IVTRTYPE_PACK, pCmd->index_inv);

			m_pEquipPack->SetItem(pCmd->index_equip, NULL);
		}
		else if (pItem1)
			pItem1->SetCount(pCmd->count_equip);

		if (!pCmd->count_inv)
		{
			if (pItem2)
				UpdateRemovedItemSC(pItem2->GetTemplateID(), IVTRTYPE_EQUIPPACK, pCmd->index_equip);

			m_pPack->SetItem(pCmd->index_inv, NULL);
		}
		else if (pItem2)
			pItem2->SetCount(pCmd->count_inv);

		pItem1 = m_pPack->GetItem(pCmd->index_inv);
		pItem2 = m_pEquipPack->GetItem(pCmd->index_equip);

		if (pItem1 && pItem2)
			UpdateExchangedItemSC(pItem1->GetTemplateID(), IVTRTYPE_EQUIPPACK, pCmd->index_equip, pItem2->GetTemplateID(), IVTRTYPE_PACK, pCmd->index_inv);
		else if (pItem1)
			UpdateMovedItemSC(pItem1->GetTemplateID(), IVTRTYPE_EQUIPPACK, pCmd->index_equip, IVTRTYPE_PACK, pCmd->index_inv);
		else if (pItem2)
			UpdateMovedItemSC(pItem2->GetTemplateID(), IVTRTYPE_PACK, pCmd->index_inv, IVTRTYPE_EQUIPPACK, pCmd->index_equip);

		//	Update player's properties.
		//	TODO: use more cheap updating commands
		g_pGame->GetGameSession()->c2s_CmdGetExtProps();

		//	Update equipment skins
		UpdateEquipSkins();

		if( pCmd->index_equip == EQUIPIVTR_WEAPON && pCmd->count_equip )
		{
			// we need to notify help system that the user has just equipped on a weapon, to keep the check state uptodate.
			CECScriptMan * pScriptMan = g_pGame->GetGameRun()->GetUIManager()->GetScriptMan();
			if( pScriptMan )
			{
				pScriptMan->GetContext()->GetCheckState()->SetEquipNewWeapon(true);
			}
		}
		if ( pCmd->index_equip >= EQUIPIVTR_GENERALCARD1 && pCmd->index_equip <= EQUIPIVTR_GENERALCARD6) {
			CDlgGeneralCard* pDlg = dynamic_cast<CDlgGeneralCard*>
				(g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_GeneralCard"));
			if (pDlg && pDlg->IsShow()) pDlg->SetSelectCard(pCmd->index_equip - EQUIPIVTR_GENERALCARD1);
		}
		break;
	}
	case PLAYER_EQUIP_TRASHBOX_ITEM:
	{
		cmd_player_equip_trashbox_item* pCmd = (cmd_player_equip_trashbox_item*)Msg.dwParam1;
		ASSERT(pCmd);

		CECInventory* pTrashBox = NULL;
		int trash_box_type = pCmd->trashbox_index;
		if (trash_box_type == IVTRTYPE_GENERALCARD_BOX)
			pTrashBox = m_pGeneralCardPack;
		else if(trash_box_type == IVTRTYPE_TRASHBOX3) pTrashBox = m_pTrashBoxPack3;
		else {
			ASSERT("!need adding code to deal with this trash box!");
			return;
		}
		CECIvtrItem* pItem1 = pTrashBox->GetItem(pCmd->trash_idx, true);
		CECIvtrItem* pItem2 = m_pEquipPack->PutItem(pCmd->equip_idx, pItem1);
		
		pTrashBox->SetItem(pCmd->trash_idx, pItem2);
		
		
		pItem1 = pTrashBox->GetItem(pCmd->trash_idx);
		pItem2 = m_pEquipPack->GetItem(pCmd->equip_idx);
		
		if (pItem1 && pItem2)
			UpdateExchangedItemSC(pItem1->GetTemplateID(), IVTRTYPE_EQUIPPACK, pCmd->equip_idx, pItem2->GetTemplateID(), trash_box_type, pCmd->trash_idx);
		else if (pItem1)
			UpdateMovedItemSC(pItem1->GetTemplateID(), IVTRTYPE_EQUIPPACK, pCmd->equip_idx, trash_box_type, pCmd->trash_idx);
		else if (pItem2)
			UpdateMovedItemSC(pItem2->GetTemplateID(), trash_box_type, pCmd->trash_idx, IVTRTYPE_EQUIPPACK, pCmd->equip_idx);
		
		//	Update equipment skins
		UpdateEquipSkins();	
		if (trash_box_type == IVTRTYPE_GENERALCARD_BOX) {
			CDlgGeneralCard* pDlg = dynamic_cast<CDlgGeneralCard*>
				(g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_GeneralCard"));
			if (pDlg && pDlg->IsShow()) pDlg->SetSelectCard(pCmd->equip_idx - EQUIPIVTR_GENERALCARD1);
		}
		
		break;
	}
	case MOVE_EQUIP_ITEM:
	{
		cmd_move_equip_item* pCmd = (cmd_move_equip_item*)Msg.dwParam1;
		ASSERT(pCmd);

		CECIvtrItem* pItem1 = m_pPack->GetItem(pCmd->index_inv);
		CECIvtrItem* pItem2 = m_pEquipPack->GetItem(pCmd->index_equip);
		if (pItem2)
		{
			ASSERT(pItem2->GetTemplateID() == pItem1->GetTemplateID());
			pItem2->AddAmount(pCmd->amount);
		}
		else
		{
			pItem2 = CECIvtrItem::CreateItem(pItem1->GetTemplateID(), pItem1->GetExpireDate(), pCmd->amount);
			m_pEquipPack->SetItem(pCmd->index_equip, pItem2);
		}

		//	Keep item information for updating shortcuts later
		ASSERT(pItem1);
		int tid = pItem1 ? pItem1->GetTemplateID() : 0;

		m_pPack->RemoveItem(pCmd->index_inv, pCmd->amount);

		//	Update shortcuts if all items have been moved
		if (tid && !m_pPack->GetItem(pCmd->index_inv))
			UpdateRemovedItemSC(tid, IVTRTYPE_PACK, pCmd->index_inv);

		//	Update player's properties.
		//	TODO: use more cheap updating commands
		g_pGame->GetGameSession()->c2s_CmdGetExtProps();

		break;
	}
	case UNFREEZE_IVTR_SLOT:
	{
		cmd_unfreeze_ivtr_slot* pCmd = (cmd_unfreeze_ivtr_slot*)Msg.dwParam1;
		FreezeItem(pCmd->where, pCmd->index, false, true);
		break;
	}
	}

	//	装备可能有更新，检查并更新装备技能
	if (UpdateEquipSkills(true))
	{
		UpdateEquipSkillCoolDown();
		g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->UpdateSkillRelatedUI();
	}
}

void CECHostPlayer::OnMsgHstTrashBoxOperation(const ECMSG& Msg)
{
	using namespace S2C;
	ASSERT(Msg.dwParam1);

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	switch (Msg.dwParam2)
	{
	case TRASHBOX_OPEN:
	{
		const cmd_trashbox_open* pCmd = (const cmd_trashbox_open*)Msg.dwParam1;

		if(pCmd->is_accountbox)
		{
			m_bUsingAccountBox = true;

			if(m_bFirstAccountBoxOpen)
			{
				m_bFirstAccountBoxOpen = false;

				g_pGame->GetGameSession()->c2s_CmdGetTrashBoxData(true, 1);
			}
			else
			{
				if (m_pAccountBoxPack->GetSize() < pCmd->slot_size)
					m_pAccountBoxPack->Resize(pCmd->slot_size);
			}

			pGameUI->PopupAccountBoxDialog();
		}
		else
		{
			m_bUsingTrashBox = true;

			if (m_bFirstTBOpen)
			{
				//	It's the first time to open trash box, get trash box data
				m_bFirstTBOpen = false;

				if(!m_bFirstFashionOpen)
					m_bFirstFashionOpen = false;
				g_pGame->GetGameSession()->c2s_CmdGetTrashBoxData(true);
			}
			else
			{
				if (m_pTrashBoxPack->GetSize() < pCmd->slot_size)
					m_pTrashBoxPack->Resize(pCmd->slot_size);

				if (m_pTrashBoxPack2->GetSize() < pCmd->slot_size2)
					m_pTrashBoxPack2->Resize(pCmd->slot_size2);

				if (m_pTrashBoxPack3->GetSize() < pCmd->slot_size3)
				{
					m_pTrashBoxPack3->Resize(pCmd->slot_size3);
				}

				pGameUI->PopupStorageDialog();
			}
		}

		break;
	}
	case TRASHBOX_CLOSE:
	{
		const cmd_trashbox_close* pCmd = (const cmd_trashbox_close*)Msg.dwParam1;
		if(pCmd->is_accountbox)
		{
			m_bUsingAccountBox = false;
			pGameUI->PopupAccountBoxDialog(true);
		}
		else
		{
			m_bUsingTrashBox = false;
			pGameUI->PopupStorageDialog(true);
		}
		break;
	}
	case TRASHBOX_WEALTH: 
	{
		const cmd_trashbox_wealth* pCmd = (const cmd_trashbox_wealth*)Msg.dwParam1;
		if(pCmd->is_accountbox)
			m_iAccountBoxMoneyCnt = pCmd->money;
		else
			m_iTrashBoxMoneyCnt = pCmd->money;
		break; 
	}
	case EXG_TRASH_MONEY: 
	{
		const cmd_exg_trash_money* pCmd = (cmd_exg_trash_money*)Msg.dwParam1;
		AddMoneyAmount(pCmd->inv_delta);
		if(pCmd->is_accountbox)
		{
			m_iAccountBoxMoneyCnt += pCmd->tra_delta;
		}
		else
		{
			m_iTrashBoxMoneyCnt += pCmd->tra_delta;
		}
		break;
	}
	case EXG_TRASHBOX_ITEM:	
	{
		const cmd_exg_trashbox_item* pCmd = (const cmd_exg_trashbox_item*)Msg.dwParam1;
		CECInventory* pTrashBox = GetPack(pCmd->where);
		if (pTrashBox)
			pTrashBox->ExchangeItem(pCmd->idx1, pCmd->idx2);

		break;
	}
	case MOVE_TRASHBOX_ITEM:
	{
		const cmd_move_trashbox_item* pCmd = (const cmd_move_trashbox_item*)Msg.dwParam1;
		CECInventory* pTrashBox = GetPack(pCmd->where);
		if (pTrashBox)
			pTrashBox->MoveItem(pCmd->src, pCmd->dest, pCmd->amount);

		break;
	}
	case EXG_TRASHBOX_IVTR:
	{
		const cmd_exg_trashbox_ivtr* pCmd = (const cmd_exg_trashbox_ivtr*)Msg.dwParam1;
		CECInventory* pTrashBox = GetPack(pCmd->where);
		if (pTrashBox)
		{
			CECIvtrItem* pItem1 = m_pPack->GetItem(pCmd->idx_inv, true);
			CECIvtrItem* pItem2 = pTrashBox->GetItem(pCmd->idx_tra, true);
			m_pPack->SetItem(pCmd->idx_inv, pItem2);
			pTrashBox->SetItem(pCmd->idx_tra, pItem1);

			//	Update shortcuts
			if (pItem1)
				UpdateRemovedItemSC(pItem1->GetTemplateID(), IVTRTYPE_PACK, pCmd->idx_inv);
			if (pCmd->where == IVTRTYPE_GENERALCARD_BOX) {
				CDlgStorage* pStorage = dynamic_cast<CDlgStorage*>(pGameUI->GetDialog("Win_Storage4"));
				if (pStorage) {
					pStorage->UpdateStorage();
					pStorage->AutoDice();
					if (pItem1 && pItem1->GetClassID() == CECIvtrItem::ICID_GENERALCARD) 
						pGameUI->AddChatMessage(pGameUI->GetStringFromTable(10981), GP_CHAT_SYSTEM);
				}
			}
			CECShoppingItemsMover::Instance().OnItemExchanged(pCmd->where, pCmd->idx_tra, pItem2, pCmd->idx_inv, pItem1);
		}

		break; 
	}
	case IVTR_ITEM_TO_TRASH:
	{
		const cmd_ivty_item_to_trash* pCmd = (const cmd_ivty_item_to_trash*)Msg.dwParam1;

		CECInventory* pTrashBox = GetPack(pCmd->where);
		if (!pTrashBox)
			return;

		CECIvtrItem* pItem1 = m_pPack->GetItem(pCmd->src);
		CECIvtrItem* pItem2 = pTrashBox->GetItem(pCmd->dest);
		if (!pItem1)
		{
			ASSERT(pItem1);
			break;
		}

		if (pItem2)
		{
			ASSERT(pItem1->GetTemplateID() == pItem2->GetTemplateID());
			pItem2->AddAmount(pCmd->amount);
		}
		else
		{
			pItem2 = CECIvtrItem::CreateItem(pItem1->GetTemplateID(), pItem1->GetExpireDate(), pCmd->amount);
			pTrashBox->SetItem(pCmd->dest, pItem2);
		}

		//	Keep item information for updating shortcuts later
		int tid = pItem1->GetTemplateID();
		
		m_pPack->RemoveItem(pCmd->src, pCmd->amount);

		//	Update shortcuts if all items have been moved
		if (!m_pPack->GetItem(pCmd->src))
			UpdateRemovedItemSC(tid, IVTRTYPE_PACK, pCmd->src);

		break;
	}
	case TRASH_ITEM_TO_IVTR:
	{
		const cmd_trash_item_to_ivtr* pCmd = (const cmd_trash_item_to_ivtr*)Msg.dwParam1;

		CECInventory* pTrashBox = GetPack(pCmd->where);
		if (!pTrashBox)
			return;

		CECIvtrItem* pItem1 = pTrashBox->GetItem(pCmd->src);
		CECIvtrItem* pItem2 = m_pPack->GetItem(pCmd->dest);
		if (!pItem1)
		{
			ASSERT(pItem1);
			break;
		}

		if (pItem2)
		{
			ASSERT(pItem1->GetTemplateID() == pItem2->GetTemplateID());
			pItem2->AddAmount(pCmd->amount);
		}
		else
		{
			pItem2 = CECIvtrItem::CreateItem(pItem1->GetTemplateID(), pItem1->GetExpireDate(), pCmd->amount);
			m_pPack->SetItem(pCmd->dest, pItem2);
		}

		pTrashBox->RemoveItem(pCmd->src, pCmd->amount);
		break;
	}
	case TRASHBOX_PWD_CHANGED:
	{
		const cmd_trashbox_pwd_changed* pCmd = (const cmd_trashbox_pwd_changed*)Msg.dwParam1;
		m_bTrashPsw = pCmd->has_passwd ? true : false;

		if(m_bTrashPsw == false && m_bFirstFashionOpen)
		{
			g_pGame->GetGameSession()->c2s_CmdOpenFashionTrash("");				
		}
		break;
	}
	case TRASHBOX_PWD_STATE:
	{
		const cmd_trashbox_pwd_state* pCmd = (const cmd_trashbox_pwd_state*)Msg.dwParam1;
		m_bTrashPsw = pCmd->has_passwd ? true : false;

		if(m_bTrashPsw == false && m_bFirstFashionOpen)
		{
			g_pGame->GetGameSession()->c2s_CmdOpenFashionTrash("");
		}

		break;
	}
	case TRASHBOX_SIZE:
	{
		const cmd_trashbox_size* pCmd = (const cmd_trashbox_size*)Msg.dwParam1;
		int msg = FIXMSG_TRASHBOX_EXPAND;
		int iSize = pCmd->iNewSize;

		if (pCmd->where == IVTRTYPE_TRASHBOX2)
			msg = FIXMSG_TRASHBOX2_EXPAND;
		else if (pCmd->where == IVTRTYPE_TRASHBOX3)
		{
			msg = FIXMSG_TRASHBOX3_EXPAND;

			if (m_pTrashBoxPack3)
			{
				m_pTrashBoxPack3->Resize(pCmd->iNewSize);
			}
		}
		else if (pCmd->where == IVTRTYPE_ACCOUNT_BOX)
		{
			msg = FIXMSG_ACCBOX_EXPAND;

			if (m_pAccountBoxPack)
			{
				m_pAccountBoxPack->Resize(pCmd->iNewSize);
			}
			iSize = pCmd->iNewSize/2;
		}

		g_pGame->GetGameRun()->AddFixedMessage(msg, iSize);
		break;
	}
	}
}

#ifdef HINT_TOOL_DEBUG
static void ExportHintToolDataToPath(const AString &strPath, S2C::cmd_own_ivtr_detail_info *pCmd)
{
	if (!CreateDirectoryA(strPath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		//	创建包裹子目录
		ASSERT(false);
		return;
	}
	
	{
		//	清除子目录内容		
		WIN32_FIND_DATAA fd;
		AString strPattern = strPath + "\\*.*";
		HANDLE hFind = FindFirstFileA(strPattern, &fd);
		AString strTemp;
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do 
			{
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					continue;
				strTemp = strPath + "\\" + fd.cFileName;
				DeleteFileA(strTemp);
			} while(FindNextFileA(hFind, &fd));
			FindClose(hFind);
		}
	}

	//	导出数据
	CECDataReader dr(pCmd->content, pCmd->content_length);
	int iNumItem = dr.Read_int();
	int iFile(0);
	for (int i=0; i < iNumItem; i++)
	{	
		int iIndex = dr.Read_int();
		if (iIndex < 0)
			continue;
		
		GNET::GRoleInventory foo;
		foo.id =  dr.Read_int();
		foo.pos = iIndex;
		
		foo.expire_date = dr.Read_int();
		foo.proctype = dr.Read_int();
		foo.count	= dr.Read_int();
		WORD crc = dr.Read_WORD();
		WORD wDataLen	= dr.Read_WORD();
		if (wDataLen)
		{
			BYTE* pData = NULL;
			pData = (BYTE*)dr.Read_Data(wDataLen);

			foo.data.replace(pData, wDataLen);
		}
		
		Marshal::OctetsStream os;
		os << foo;

		//	创建文件写入
		AString strFileName;
		strFileName.Format("%s\\__%d", strPath, iFile);
		FILE *f = fopen(strFileName, "wb");
		if (!f)
		{
			ASSERT(false);
			continue;
		}
		size_t len = fwrite(os.begin(), 1, os.size(), f);
		if (len != os.size())
		{
			ASSERT(false);
			fclose(f);
			DeleteFileA(strFileName);
			continue;
		}
		++iFile;
		fflush(f);
		fclose(f);
	}
}
static void ExportForHintTool(S2C::cmd_own_ivtr_detail_info *pCmd)
{
	//	导出普通包裹和装备包裹中的数据，供寻宝网小工具调试使用
	//
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}

	//	创建目录
	AString strHintToolPath = "wmgjHintTool";
	if (!CreateDirectoryA(strHintToolPath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		//	创建顶层目录
		ASSERT(false);
		return;
	}
	if (pCmd->byPackage == IVTRTYPE_EQUIPPACK)
	{
		//	清除并导出装备包裹
		AString	strEquipPackPath = strHintToolPath + "\\equip";
		ExportHintToolDataToPath(strEquipPackPath, pCmd);
	}
	if (pCmd->byPackage == IVTRTYPE_PACK)
	{
		//	清除并导出普通包裹
		AString strPackPath = strHintToolPath + "\\pack";
		ExportHintToolDataToPath(strPackPath, pCmd);
	}

}
#endif

void CECHostPlayer::OnMsgHstIvtrInfo(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	switch (Msg.dwParam2)
	{
	case OWN_IVTR_DATA:
	{
		cmd_own_ivtr_info* pCmd = (cmd_own_ivtr_info*)Msg.dwParam1;

		CECInventory* pPack = GetPack(pCmd->byPackage);
		if (!pPack)
			return;

		pPack->ResetItems(*pCmd);

		//	Update skins
		if (pCmd->byPackage == IVTRTYPE_EQUIPPACK)
		{
			//	Update equipment skins
			UpdateEquipSkins();
		}
		else if (pCmd->byPackage == IVTRTYPE_TRASHBOX)
		{
			if (m_bUsingTrashBox)
				pGameUI->PopupStorageDialog();
		}

		break;
	}
	case OWN_IVTR_DETAIL_DATA:
	{
		cmd_own_ivtr_detail_info* pCmd = (cmd_own_ivtr_detail_info*)Msg.dwParam1;

		CECInventory* pPack = GetPack(pCmd->byPackage);
		if (!pPack)
			return;

		pPack->ResetItems(*pCmd);

#ifdef HINT_TOOL_DEBUG
		ExportForHintTool(pCmd);
#endif

		//	Update skins
		if (pCmd->byPackage == IVTRTYPE_EQUIPPACK)
		{
			//	Update equipment skins
			UpdateEquipSkins();
		}
		else if (pCmd->byPackage == IVTRTYPE_TRASHBOX)
		{
			if (m_bUsingTrashBox)
				pGameUI->PopupStorageDialog();
		}

		break;
	}
	case GET_OWN_MONEY:
	{
		cmd_get_own_money* pCmd = (cmd_get_own_money*)Msg.dwParam1;
		SetMoneyAmount(pCmd->amount);
		m_iMaxMoney = (int)pCmd->max_amount;
		break;
	}
	case CHANGE_IVTR_SIZE:
	{
		cmd_change_ivtr_size* pCmd = (cmd_change_ivtr_size*)Msg.dwParam1;

		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_NEW_INVENTORY_SIZE, pCmd->size);

		if (m_pPack)
			m_pPack->Resize(pCmd->size);

		break;
	}
	}
}

void CECHostPlayer::OnMsgHstTeamInvite(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_team_leader_invite* pCmd = (cmd_team_leader_invite*)Msg.dwParam1;
	ASSERT(pCmd);

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	//	Try to get player's information
	CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(pCmd->idLeader);
	if (pPlayer)
	{
		pGameUI->PopupInviteGroupMenu(pCmd->idLeader, pPlayer->GetName(), pPlayer->GetProfession(), 
							pCmd->seq, pCmd->wPickFlag);
		return;
	}

	CECEPCacheData* pCacheData = m_pPlayerMan->GetPlayerCacheData(pCmd->idLeader);
	if (pCacheData)
	{
		pGameUI->PopupInviteGroupMenu(pCmd->idLeader, pCacheData->m_strName, pCacheData->m_iProfession, 
							pCmd->seq, pCmd->wPickFlag);
		return;
	}

	//	Couldn't get player's information immediately, so we have to got it from
	//	server.
	for (int i=0; i < m_aTeamInvs.GetSize(); i++)
	{
		TEAMINV& Inv = m_aTeamInvs[i];
		if (Inv.idLeader == pCmd->idLeader)
		{
			//	Invitation from this leader has existed, update invitation info.
			Inv.seq			= pCmd->seq;
			Inv.wPickFlag	= pCmd->wPickFlag;
			return;
		}
	}

	//	Save team invitation information
	TEAMINV TeamInv;
	TeamInv.idLeader	= pCmd->idLeader;
	TeamInv.seq			= pCmd->seq;
	TeamInv.wPickFlag	= pCmd->wPickFlag;
	m_aTeamInvs.Add(TeamInv);

	g_pGame->GetGameSession()->GetPlayerBriefInfo(1, &pCmd->idLeader, 3);
}


void CECHostPlayer::OnMsgHstTeamReject(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_team_reject_invite* pCmd = (cmd_team_reject_invite*)Msg.dwParam1;
	ASSERT(pCmd);

	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_REJECTJOINTEAM, g_pGame->GetGameRun()->GetPlayerName(pCmd->idPlayer, true));
}

void CECHostPlayer::OnMsgHstTeamMemPos(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_team_member_pos* pCmd = (cmd_team_member_pos*)Msg.dwParam1;
	ASSERT(pCmd);

	if (!m_pTeam)
	{
		ASSERT(0);
		return;
	}

	CECTeamMember* pMember = m_pTeam->GetMemberByID(pCmd->idMember);
	if (pMember)
		pMember->SetPos(pCmd->idInst, pCmd->pos, pCmd->bSameInstance!=0);
}

void CECHostPlayer::OnMsgHstEquipDamaged(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_equip_damaged* pCmd = (cmd_equip_damaged*)Msg.dwParam1;
	ASSERT(pCmd);

	CECIvtrEquip* pEquip = (CECIvtrEquip*)m_pEquipPack->GetItem(pCmd->index);
	if (!pEquip)
	{
		ASSERT(pEquip);
		return;
	}
	if(!pCmd->reason)
	{
		//	Reduce equipment's endurance to 0
		pEquip->AddCurEndurance(-pEquip->GetCurEndurance());
	}
	else
	{
		int iDropTid = 0;
		int iDropNum = 0;

		int iEquipType = pEquip->GetClassID();
		switch(iEquipType)
		{
		case CECIvtrItem::ICID_WEAPON:
		{
			iDropTid = ((CECIvtrWeapon*)pEquip)->GetDBEssence()->id_drop_after_damaged;
			iDropNum = ((CECIvtrWeapon*)pEquip)->GetDBEssence()->num_drop_after_damaged;
			break;
		}
		case CECIvtrItem::ICID_ARMOR:
		{
			iDropTid = ((CECIvtrArmor*)pEquip)->GetDBEssence()->id_drop_after_damaged;
			iDropNum = ((CECIvtrArmor*)pEquip)->GetDBEssence()->num_drop_after_damaged;
			break;
		}
		case CECIvtrItem::ICID_DECORATION:
		{
			iDropTid = ((CECIvtrDecoration*)pEquip)->GetDBEssence()->id_drop_after_damaged;
			iDropNum = ((CECIvtrDecoration*)pEquip)->GetDBEssence()->num_drop_after_damaged;
			break;
		}
		case CECIvtrItem::ICID_BIBLE:
		{
			iDropTid = ((CECIvtrBible*)pEquip)->GetDBEssence()->id_drop_after_damaged;
			iDropNum = ((CECIvtrBible*)pEquip)->GetDBEssence()->num_drop_after_damaged;
			break;
		}
		case CECIvtrItem::ICID_FLYSWORD:
		{
			iDropTid = ((CECIvtrFlySword*)pEquip)->GetDBEssence()->id_drop_after_damaged;
			iDropNum = ((CECIvtrFlySword*)pEquip)->GetDBEssence()->num_drop_after_damaged;
			break;
		}

		default:

			ASSERT(0);
			break;
		}

		if(iDropNum == 0 || iDropTid == 0)
		{
			ASSERT(0);
			return;
		}

		//	Get database data
		elementdataman* pDB = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		ELEMENT_ESSENCE* pDBEssence	= (ELEMENT_ESSENCE*)pDB->get_data_ptr(iDropTid, ID_SPACE_ESSENCE, DataType);
		
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_EQUIP_DESTROYING, pEquip->GetName(), pDBEssence->name, iDropNum);
	}
}

void CECHostPlayer::OnMsgHstTeamMemPickup(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_team_member_pickup* pCmd = (cmd_team_member_pickup*)Msg.dwParam1;
	ASSERT(pCmd);

	//	Get item name from it's tid
	CECIvtrItem* pItem = CECIvtrItem::CreateItem(pCmd->tid, 0, 1);
	if (pItem)
	{
		ACHAR szName[64];
		AUI_ConvertChatString(g_pGame->GetGameRun()->GetPlayerName(pCmd->idMember, true), szName);
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_TEAMMEMPICKUP, szName, (int)pCmd->count, pItem->GetName());
		delete pItem;
	}
}

void CECHostPlayer::OnMsgHstNPCGreeting(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_npc_greeting* pCmd = (cmd_npc_greeting*)Msg.dwParam1;
	ASSERT(pCmd);

	if (ISNPCID(pCmd->idObject))
	{
		// 专门处理学习技能的隐藏NPC
		if (CECHostSkillModel::Instance().IsSkillLearnNPC(pCmd->idObject)) {
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			//m_idSevNPC = pCmd->idObject;
			//m_bTalkWithNPC	= true;
			//pGameUI->GetDialog("Win_SkillAction")->Show(true);
			//CDlgSkillAction* dlg = dynamic_cast<CDlgSkillAction*>(pGameUI->GetDialog("Win_SkillAction"));
			//dlg->ForceShowDialog();
			CDlgSkillAction* dlg = dynamic_cast<CDlgSkillAction*>(pGameUI->GetDialog("Win_SkillAction"));
			dlg->SetReceivedNPCGreeting(true);
			return;
		}

		CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(pCmd->idObject);
		if (!pNPC || !pNPC->IsServerNPC())
		{
			ASSERT(0);
			return;
		}
	
		//	Check distance again
		if (!CanTouchTarget(pNPC->GetPos(), pNPC->GetTouchRadius(), 3))
			return;

		m_idSevNPC		= pCmd->idObject;
		m_bTalkWithNPC	= true;

		//	Check way point service on NPC
		DWORD dwID = ((CECNPCServer*)pNPC)->GetWayPointID();
		if (dwID && !HasWayPoint((WORD)dwID))
			g_pGame->GetGameSession()->c2s_CmdNPCSevWaypoint();

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->PopupNPCDialog(((CECNPCServer*)pNPC)->GetDBEssence());
	}
	else if (ISPLAYERID(pCmd->idObject))
	{
		CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(pCmd->idObject);

		//	Check distance again
		if (!pPlayer || !CanTouchTarget(pPlayer->GetPos(), 0.0f, 3))
			return;

		m_idSevNPC		= pCmd->idObject;
		m_bTalkWithNPC	= true;
		m_iBoothState	= 3;
		
		g_pGame->GetGameSession()->c2s_CmdNPCSevGetContent(GP_NPCSEV_BOOTHSELL);

		m_pBuyPack->RemoveAllItems();
		m_pSellPack->RemoveAllItems();
		m_pEPBoothBPack->RemoveAllItems();
		m_pEPBoothSPack->RemoveAllItems();

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->PopupBoothDialog(true, false, pCmd->idObject);
	}
	else
	{
		ASSERT(0);
		return;
	}
}

void CECHostPlayer::OnMsgHstTradeStart(const ECMSG& Msg)
{
	using namespace GNET;
	TradeStart_Re* p = (TradeStart_Re*)Msg.dwParam1;
	ASSERT(p->roleid == m_PlayerInfo.cid);

	//	Clear deal packages
	m_pDealPack->RemoveAllItems();
	m_pDealPack->Resize(IVTRSIZE_DEALPACK);
	m_pEPDealPack->RemoveAllItems();
	m_pEPDealPack->Resize(IVTRSIZE_DEALPACK);

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (p->retcode == ERR_SUCCESS)
	{
		m_idTradePlayer = p->partner_roleid;
		pGameUI->PopupTradeResponseMenu(p->partner_roleid, p->tid, true);
	}
	else
		pGameUI->PopupTradeResponseMenu(p->partner_roleid, p->tid, false);
}

void CECHostPlayer::OnMsgHstTradeRequest(const ECMSG& Msg)
{
	using namespace GNET;
	TradeStartRqst* p = (TradeStartRqst*)Msg.dwParam1;
	TradeStartRqstArg* pArg = (TradeStartRqstArg*)p->GetArgument();

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->PopupTradeInviteMenu(pArg->start_roleid, (DWORD)p);
}

void CECHostPlayer::OnMsgHstTradeMoveItem(const ECMSG& Msg)
{
	using namespace GNET;
	TradeMoveObj_Re* p = (TradeMoveObj_Re*)Msg.dwParam1;
	if (p->retcode != ERR_SUCCESS)
	{
		//	Un-freeze items
		FreezeItem(IVTRTYPE_PACK, p->src_pos, false, true);
		FreezeItem(IVTRTYPE_PACK, p->dst_pos, false, true);
		return;
	}

	ASSERT(p->roleid == m_PlayerInfo.cid);

	CECIvtrItem* pSrcItem = m_pPack->GetItem(p->src_pos);
	CECIvtrItem* pDstItem = m_pPack->GetItem(p->dst_pos);

	if (!pSrcItem)
	{
		ASSERT(pSrcItem);
		return;
	}

	if (!pDstItem || (pSrcItem->GetTemplateID() == pDstItem->GetTemplateID() && pSrcItem->GetPileLimit() > 1))
	{
		m_pPack->MoveItem(p->src_pos, p->dst_pos, p->count);

		//	Update shortcuts
		UpdateMovedItemSC(pSrcItem->GetTemplateID(), IVTRTYPE_PACK, p->src_pos, IVTRTYPE_PACK, p->dst_pos);
	}
	else
	{
		m_pPack->ExchangeItem(p->src_pos, p->dst_pos);

		//	Update shortcuts
		if (pSrcItem && pDstItem)
			UpdateExchangedItemSC(pSrcItem->GetTemplateID(), IVTRTYPE_PACK, p->src_pos, pDstItem->GetTemplateID(), IVTRTYPE_PACK, p->dst_pos);
		else if (pSrcItem)
			UpdateMovedItemSC(pSrcItem->GetTemplateID(), IVTRTYPE_PACK, p->src_pos, IVTRTYPE_PACK, p->dst_pos);
		else if (pDstItem)
			UpdateMovedItemSC(pDstItem->GetTemplateID(), IVTRTYPE_PACK, p->dst_pos, IVTRTYPE_PACK, p->src_pos);
	}

	//	Un-freeze items
	FreezeItem(IVTRTYPE_PACK, p->src_pos, false, true);
	FreezeItem(IVTRTYPE_PACK, p->dst_pos, false, true);
}

void CECHostPlayer::OnMsgHstTradeCancel(const ECMSG& Msg)
{
	using namespace GNET;
	TradeDiscard_Re* p = (TradeDiscard_Re*)Msg.dwParam1;

	m_idTradePlayer = 0;

	//	Clear deal packages
	m_pDealPack->RemoveAllItems();
	m_pEPDealPack->RemoveAllItems();

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->TradeAction(p->discard_roleid, p->tid, CDlgTrade::TRADE_ACTION_CANCEL, 0);
}

void CECHostPlayer::OnMsgHstTradeSubmit(const ECMSG& Msg)
{
	using namespace GNET;
	TradeSubmit_Re* p = (TradeSubmit_Re*)Msg.dwParam1;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	if (p->retcode == ERR_TRADE_READY_HALF || p->retcode == ERR_TRADE_READY)
	{
		pGameUI->TradeAction(p->submit_roleid, p->tid, CDlgTrade::TRADE_ACTION_LOCK, p->retcode);
	}
	else if (p->retcode == ERR_TRADE_SPACE)
	{
		//	One of trader doesn't have enough package slot
		pGameUI->TradeAction(p->submit_roleid, p->tid, CDlgTrade::TRADE_ACTION_UNLOCK, p->retcode);
	}
}

void CECHostPlayer::OnMsgHstTradeConfirm(const ECMSG& Msg)
{
	using namespace GNET;
	TradeConfirm_Re* p = (TradeConfirm_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->TradeAction(p->confirm_roleid, p->tid, CDlgTrade::TRADE_ACTION_DEAL, p->retcode);

	//	Trade succeeded, update shortcuts which reference to sold items
	if (p->retcode == ERR_TRADE_DONE && m_pDealPack->GetSize())
	{
		//	Build exclude slots array
		AArray<int, int> aExcSlots;
		int i;

		for (i=0; i < m_pDealPack->GetSize(); i++)
		{
			CECIvtrItem* pItem = m_pDealPack->GetItem(i);
			if (pItem)
			{
				const CECDealInventory::ITEMINFO& ItemInfo = m_pDealPack->GetItemInfo(i);
				if (ItemInfo.iAmount >= pItem->GetCount())
					aExcSlots.Add(ItemInfo.iOrigin);
			}
		}

		for (i=0; i < m_pDealPack->GetSize(); i++)
		{
			CECIvtrItem* pItem = m_pDealPack->GetItem(i);
			if (!pItem)
				continue;

			const CECDealInventory::ITEMINFO& ItemInfo = m_pDealPack->GetItemInfo(i);
			if (ItemInfo.iAmount >= pItem->GetCount())
			{
				UpdateRemovedItemSC(pItem->GetTemplateID(), IVTRTYPE_PACK, ItemInfo.iOrigin, 
					aExcSlots.GetData(), aExcSlots.GetSize());
			}
		}
	}
	else if (p->retcode == ERR_TRADE_SPACE)
	{
		//	Notify player that one of trader doesn't have enough package slot
		if (p->confirm_roleid == m_PlayerInfo.cid)
			pGameUI->ShowErrorMsg(g_pGame->GetFixedMsgTab()->GetWideString(FIXMSG_PACKFULL1));
		else
			pGameUI->ShowErrorMsg(g_pGame->GetFixedMsgTab()->GetWideString(FIXMSG_PACKFULL2));
	}
}

void CECHostPlayer::OnMsgHstTradeEnd(const ECMSG& Msg)
{
	using namespace GNET;
	TradeEnd* p = (TradeEnd*)Msg.dwParam1;

	m_idTradePlayer = 0;

	//	Un-freeze items
	for (int i=0; i < m_pDealPack->GetSize(); i++)
	{
		CECIvtrItem* pItem = m_pDealPack->GetItem(i);
		if (pItem)
			pItem->NetFreeze(false);
	}

	//	Clear deal packages
	m_pDealPack->RemoveAllItems();
	m_pEPDealPack->RemoveAllItems();

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->TradeAction(p->roleid, p->tid, CDlgTrade::TRADE_ACTION_END, p->cause);
}

void CECHostPlayer::OnMsgHstTradeAddGoods(const ECMSG& Msg)
{
	using namespace GNET;
	TradeAddGoods_Re* p = (TradeAddGoods_Re*)Msg.dwParam1;
	ASSERT(p->roleid == m_PlayerInfo.cid);
	
	if (p->owner_roleid == m_PlayerInfo.cid)
	{
		if (p->retcode == ERR_SUCCESS)
		{
			if (p->goods.id)
			{
				CECIvtrItem* pItem = m_pPack->GetItem(p->goods.pos);
				if (!pItem)
				{
					ASSERT(0);
					return;
				}

				m_pDealPack->AddItem(pItem, p->goods.pos, p->goods.count, false);
			}

			m_pDealPack->AddMoney(p->money);
		}
		else
		{
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			pGameUI->ShowErrorMsg(g_pGame->GetFixedMsgTab()->GetWideString(FIXMSG_NOTTRADEITEM));
		}
	}
	else
	{
		ASSERT(ISPLAYERID(p->owner_roleid));

		if (p->goods.id)
		{
			CECIvtrItem* pItem = NULL;
			int iIndex = m_pEPDealPack->GetItemIndex(p->goods.pos);
			if (iIndex >= 0)
			{
				const CECDealInventory::ITEMINFO& Info = m_pEPDealPack->GetItemInfo(iIndex);
				ASSERT(Info.iOrigin == p->goods.pos);

				pItem = m_pEPDealPack->GetItem(iIndex);
				if (pItem)
					pItem->AddAmount(p->goods.count);
			}
			else
			{
				//	Create a new item
				CECIvtrItem* pItem = CECIvtrItem::CreateItem(p->goods.id, p->goods.expire_date, p->goods.count);
				if (pItem)
				{
					if( p->goods.data.begin() && p->goods.data.size() )
						pItem->SetItemInfo((BYTE*)p->goods.data.begin(), p->goods.data.size());
					else
						((CECIvtrItem*)pItem)->SetItemInfo(NULL, 0);

					pItem->SetProcType(p->goods.proctype);
					pItem->SetPriceScale(CECIvtrItem::SCALE_SELL, PLAYER_PRICE_SCALE);

					m_pEPDealPack->AddItem(pItem, p->goods.pos, p->goods.count, true);
				}
			}
		}

		m_pEPDealPack->AddMoney(p->money);
	}

	//	Notify interface other bargainer changed his goods
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->TradeAction(p->owner_roleid, p->tid, CDlgTrade::TRADE_ACTION_ALTER, 0);
}

void CECHostPlayer::OnMsgHstTradeRemGoods(const ECMSG& Msg)
{
	using namespace GNET;
	TradeRemoveGoods_Re* p = (TradeRemoveGoods_Re*)Msg.dwParam1;
	ASSERT(p->roleid == m_PlayerInfo.cid);

	if (p->owner_roleid == m_PlayerInfo.cid)
	{
		if (p->retcode != ERR_SUCCESS)
			return;

		if (p->goods.id)
		{
			CECIvtrItem* pItem = m_pPack->GetItem(p->goods.pos);
			if (!pItem)
			{
				ASSERT(0);
				return;
			}

			m_pDealPack->RemoveItem(p->goods.pos, p->goods.count);
		}

		m_pDealPack->AddMoney(-(int)p->money);
	}
	else
	{
		ASSERT(ISPLAYERID(p->owner_roleid));

		if (p->goods.id)
		{
			int iIndex = m_pEPDealPack->GetItemIndex(p->goods.pos);
			if (iIndex < 0)
			{
				ASSERT(0);
				return;
			}

			m_pEPDealPack->RemoveItem(p->goods.pos, p->goods.count);
		}

		m_pEPDealPack->AddMoney(-(int)p->money);
	}

	//	Notify interface other bargainer changed his goods
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->TradeAction(p->owner_roleid, p->tid, CDlgTrade::TRADE_ACTION_ALTER, 0);
}

void CECHostPlayer::OnMsgHstWebTradeList(const ECMSG& Msg)
{
	using namespace GNET;
	WebTradeList_Re* p = (WebTradeList_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->WebTradeAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstWebTradeAttendList(const ECMSG& Msg)
{
	using namespace GNET;
	WebTradeAttendList_Re* p = (WebTradeAttendList_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->WebTradeAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstWebTradeGetItem(const ECMSG& Msg)
{
	using namespace GNET;
	WebTradeGetItem_Re* p = (WebTradeGetItem_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->WebTradeAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstWebTradeGetDetail(const ECMSG& Msg)
{
	using namespace GNET;
	WebTradeGetDetail_Re* p = (WebTradeGetDetail_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->WebTradeAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstWebTradePrePost(const ECMSG& Msg)
{
	using namespace GNET;
	WebTradePrePost_Re* p = (WebTradePrePost_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->WebTradeAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstWebTradeCancelPost(const ECMSG& Msg)
{
	using namespace GNET;
	WebTradePreCancelPost_Re* p = (WebTradePreCancelPost_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->WebTradeAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstWebTradeUpdate(const ECMSG& Msg)
{
	using namespace GNET;
	WebTradeUpdate_Re* p = (WebTradeUpdate_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->WebTradeAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstSysAuctionAccount(const ECMSG& Msg)
{
	using namespace GNET;
	SysAuctionAccount_Re* p = (SysAuctionAccount_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->SysAuctionAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstSysAuctionBid(const ECMSG& Msg)
{
	using namespace GNET;
	SysAuctionBid_Re* p = (SysAuctionBid_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->SysAuctionAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstSysAuctionCashTransfer(const ECMSG& Msg)
{
	using namespace GNET;
	SysAuctionCashTransfer_Re* p = (SysAuctionCashTransfer_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->SysAuctionAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstSysAuctionGetItem(const ECMSG& Msg)
{
	using namespace GNET;
	SysAuctionGetItem_Re* p = (SysAuctionGetItem_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->SysAuctionAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstSysAuctionList(const ECMSG& Msg)
{
	using namespace GNET;
	SysAuctionList_Re* p = (SysAuctionList_Re*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->SysAuctionAction(p->GetType(), p);
}

void CECHostPlayer::OnMsgHstWeddingBookList(const ECMSG &Msg)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->WeddingBookList((S2C::cmd_wedding_book_list *)Msg.dwParam1);
}

void CECHostPlayer::OnMsgHstWeddingBookSuccess(const ECMSG &Msg)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
		pGameUI->WeddingBookSuccess((S2C::cmd_wedding_book_success *)Msg.dwParam1);
}

void CECHostPlayer::OnMsgHstStartAttack(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_host_start_attack* pCmd = (cmd_host_start_attack*)Msg.dwParam1;
	ASSERT(pCmd);

	//	test code...
//	g_pGame->GetRTDebug()->OutputNotifyMessage(RTDCOL_WARNING, _AL("start attack !"));

	//	Check whether target is the one that we have selected
	if (m_idSelTarget != pCmd->idTarget)
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("Target has changed !"));
	
	//	If target turn to be un-attackable, cancel action
	if (!AttackableJudge(pCmd->idTarget, true))
	{
		g_pGame->GetGameSession()->c2s_CmdCancelAction();
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("Cannel attacking !"));
		return;
	}

	//	Synchronize ammo amount
	CECIvtrItem* pItem = m_pEquipPack->GetItem(EQUIPIVTR_PROJECTILE);
	if (pItem)
	{
		if (!pCmd->ammo_remain)
			m_pEquipPack->SetItem(EQUIPIVTR_PROJECTILE, NULL);
		else
			pItem->SetAmount(pCmd->ammo_remain);
	}

	CECHPWorkMelee* pWork = (CECHPWorkMelee*)m_pWorkMan->CreateWork(CECHPWork::WORK_HACKOBJECT);
	m_pWorkMan->StartWork_p1(pWork);

	m_bMelee = true;
	AP_ActionEvent(AP_EVENT_STARTMELEE);
}

void CECHostPlayer::OnMsgHstGainItem(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	ASSERT(pGameUI);

	switch (Msg.dwParam2)
	{
	case GAIN_MONEY_IN_TRADE:
	case GAIN_ITEM_IN_TRADE:

		//	TODO: print a message to notify player
		break;

	case GAIN_MONEY_AFTER_TRADE:
	{
		cmd_gain_money_after_trade* pCmd = (cmd_gain_money_after_trade*)Msg.dwParam1;
		ASSERT(pCmd);
		AddMoneyAmount(pCmd->amount);
		break;
	}
	case GAIN_ITEM_AFTER_TRADE:
	{
		cmd_gain_item_after_trade* pCmd = (cmd_gain_item_after_trade*)Msg.dwParam1;
		ASSERT(pCmd);

		int iLastSlot, iSlotNum;
		if (!m_pPack->MergeItem(pCmd->tid, pCmd->expire_date, pCmd->amount, &iLastSlot, &iSlotNum) ||
			iLastSlot != (int)pCmd->index || iSlotNum != (int)pCmd->iSlotAmount)
		{
			//	Local inventory data lose synchronization with server,
			//	so we have to re-get all inventory data from server.
			g_pGame->GetGameSession()->c2s_CmdGetIvtrDetailData(IVTRTYPE_PACK);

			//	Print a notice
			CECIvtrItem* pItem = CECIvtrItem::CreateItem(pCmd->tid, pCmd->expire_date, 1);
			if (pItem)
			{
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_PICKUPITEM, pCmd->amount, pItem->GetName());
				delete pItem;
			}
		}
		else
		{
			//	If item is equipment we have to get it's detail information, 
			//	otherwise it may be shown as "unable to be equipped"
			CECIvtrItem* pItem = m_pPack->GetItem(pCmd->index);
			if (!pItem)
				return;

			if (pItem->IsEquipment())
				g_pGame->GetGameSession()->c2s_CmdGetItemInfo(IVTRTYPE_PACK, (BYTE)pCmd->index);

			if (pItem->GetClassID() == CECIvtrItem::ICID_GENERALCARD)
				m_CardsToMove.insert(pCmd->index);

			//	Print a notice
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_PICKUPITEM, pCmd->amount, pItem->GetName());
		}

		break;
	}
	}
}

void CECHostPlayer::OnMsgHstPurchaseItems(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_purchase_item* pCmd = (cmd_purchase_item*)Msg.dwParam1;
	ASSERT(pCmd);

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	AArray<BYTE, BYTE> aEquipments;
	aEquipments.SetSize(pCmd->item_count, 10);

	for (int i=0; i < pCmd->item_count; i++)
	{
		const cmd_purchase_item::ITEM& Item = pCmd->item_list[i];

		//	Put item into pack
		int iLastSlot, iSlotNum;
		if (!m_pPack->MergeItem(Item.item_id, Item.expire_date, Item.count, &iLastSlot, &iSlotNum) ||
			iLastSlot != Item.inv_index)
		{
			ASSERT(0);
			continue;
		}

		//	If item is equipment we have to get it's detail information, 
		//	otherwise it may be shown as "unable to be equipped"
		CECIvtrItem* pItem = m_pPack->GetItem(Item.inv_index);
		if (pItem && pItem->IsEquipment())
			aEquipments.Add((BYTE)Item.inv_index);

		if (pCmd->flag && m_iBoothState == 2)
		{
			int iBoothIndex = m_pBoothBPack->GetItemIndexByFlag(Item.booth_slot);
			CECIvtrItem* pBoothItem = m_pBoothBPack->GetItem(iBoothIndex);
			if (pBoothItem)
			{
				//	Give player a notify message
				struct tm* time = glb_GetFormatLocalTime();
				ACString strMsg;
				strMsg.Format(g_pGame->GetFixedMsgTab()->GetWideString(FIXMSG_BOOTHBUY), 
					time->tm_hour, time->tm_min, Item.count, pBoothItem->GetName(), pBoothItem->GetUnitPrice());
				pGameUI->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_OK", strMsg, 0x0fffffff, 0, 0, 0);
			}

			//	From item from booth buy pack
			m_pBoothBPack->RemoveItemByFlag(Item.booth_slot, Item.count);
		}
	}

	//	Spent money
	AddMoneyAmount(-(int)pCmd->cost);

	if (aEquipments.GetSize())
		g_pGame->GetGameSession()->c2s_CmdGetItemInfoList(IVTRTYPE_PACK, aEquipments.GetSize(), aEquipments.GetData());
}

void CECHostPlayer::OnMsgHstSpendMoney(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_spend_money* pCmd = (cmd_spend_money*)Msg.dwParam1;
	ASSERT(pCmd);

	AddMoneyAmount(-(int)pCmd->cost);
}

void CECHostPlayer::OnMsgHstItemToMoney(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_item_to_money* pCmd = (cmd_item_to_money*)Msg.dwParam1;
	ASSERT(pCmd);

	m_pPack->RemoveItem(pCmd->index, pCmd->count);

	//	Update shortcut
	if (!m_pPack->GetItem(pCmd->index))
		UpdateRemovedItemSC(pCmd->type, IVTRTYPE_PACK, pCmd->index);

	AddMoneyAmount((int)pCmd->money);
}

void CECHostPlayer::OnMsgHstRepair(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == REPAIR_ALL)
	{
		cmd_repair_all* pCmd = (cmd_repair_all*)Msg.dwParam1;
		ASSERT(pCmd);

		m_pEquipPack->RepairAllItems();
	//	m_pPack->RepairAllItems();
	//	m_pTaskPack->RepairAllItems();

		AddMoneyAmount(-(int)pCmd->cost);
	}
	else if (Msg.dwParam2 == REPAIR)
	{
		cmd_repair* pCmd = (cmd_repair*)Msg.dwParam1;
		ASSERT(pCmd);

		CECInventory* pPack = GetPack(pCmd->byPackage);
		if (!pPack) return;
		CECIvtrItem* pItem = (CECIvtrItem*)pPack->GetItem(pCmd->bySlot);
		if (pItem && pItem->IsEquipment() && pItem->IsRepairable())
			((CECIvtrEquip*)pItem)->Repair();

		AddMoneyAmount(-(int)pCmd->cost);
	}
}

void CECHostPlayer::OnMsgHstUseItem(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_host_use_item* pCmd = (cmd_host_use_item*)Msg.dwParam1;
	ASSERT(pCmd);

	CECInventory* pPack = GetPack(pCmd->byPackage);
	if (!pPack) return;

	CECIvtrItem* pItem = (CECIvtrItem*)pPack->GetItem(pCmd->bySlot);
	if (!pItem || pItem->GetTemplateID() != pCmd->item_id)
	{
		ASSERT(0);
		return;
	}

	//	Play some effect or sound ?
	PlayUseItemEffect(pCmd->item_id);

	pItem->Use();

	if (pCmd->use_count)
	{
		pPack->RemoveItem(pCmd->bySlot, pCmd->use_count);

		//	Update shortcut
		if (!pPack->GetItem(pCmd->bySlot))
			UpdateRemovedItemSC(pCmd->item_id, pCmd->byPackage, pCmd->bySlot);			
	}

	//	End using-item work
	if (CECHPWorkUse *pWork = dynamic_cast<CECHPWorkUse *>(m_pWorkMan->GetRunningWork(CECHPWork::WORK_USEITEM))){
		if (pWork->GetItem() == pCmd->item_id){
			m_pWorkMan->FinishRunningWork(CECHPWork::WORK_USEITEM);
		}
	}
}

void CECHostPlayer::OnMsgHstUseItemWithData(const ECMSG& Msg)
{
	using namespace S2C;

	player_use_item_with_arg* pCmd = (player_use_item_with_arg*)Msg.dwParam1;
	ASSERT(pCmd);

	CECInventory* pPack = GetPack(pCmd->where);
	if (!pPack) return;

	CECIvtrItem* pItem = (CECIvtrItem*)pPack->GetItem(pCmd->index);
	if (!pItem || pItem->GetTemplateID() != pCmd->item_id)
	{
		ASSERT(0);
		return;
	}

	//	Play some effect or sound ?
	PlayUseItemEffect(pCmd->item_id, pCmd->arg, pCmd->size);

	pItem->Use();

	if (pCmd->use_count)
	{
		pPack->RemoveItem(pCmd->index, pCmd->use_count);

		//	Update shortcut
		if (!pPack->GetItem(pCmd->index))
			UpdateRemovedItemSC(pCmd->item_id, pCmd->where, pCmd->index);
	}

	//	End using-item work
	if (CECHPWorkUse *pWork = dynamic_cast<CECHPWorkUse *>(m_pWorkMan->GetRunningWork(CECHPWork::WORK_USEITEM))){
		if (pWork->GetItem() == pCmd->item_id){
			m_pWorkMan->FinishRunningWork(CECHPWork::WORK_USEITEM);
		}
	}
}

void CECHostPlayer::OnMsgHstSkillData(const ECMSG& Msg)
{
	//	Release all skills
//	ReleaseSkills();
	if (!HostIsReady())
		ASSERT(!m_aPtSkills.GetSize() && !m_aPsSkills.GetSize());
	
	using namespace S2C;

	cmd_skill_data* pCmd = (cmd_skill_data*)Msg.dwParam1;
	ASSERT(pCmd);

	//建立临时数组存储快捷栏中技能
	
	std::vector<SkillShortCutConfig> skillSCConfigArray1;
	std::vector<SkillShortCutConfig> skillSCConfigArray2;

	std::vector<SkillGrpShortCutConfig> skillGrpSCConfigArray1;
	std::vector<SkillGrpShortCutConfig> skillGrpSCConfigArray2;

	if (HostIsReady())
	{
		// 仙魔转换时、替换原有所有相关技能

		// 1.删除现有对该技能指针的引用
		class CECHPTraceSpellMatcher : public CECHPWorkMatcher{
		public:
			virtual bool operator()(CECHPWork *pWork, int priority, bool isDelayWork)const{
				return pWork != NULL
					&& pWork->GetWorkID() == CECHPWork::WORK_TRACEOBJECT
					&& dynamic_cast<CECHPWorkTrace*>(pWork)->GetTraceReason() == CECHPWorkTrace::TRACE_SPELL;
			}
		};
		m_pWorkMan->FinishWork(CECHPTraceSpellMatcher());
		ASSERT(m_pPrepSkill == NULL);
		m_pPrepSkill = NULL;
		ASSERT(m_pCurSkill == NULL);
		m_pCurSkill = NULL;
		ClearComboSkill();
		
		//备份快捷键技能id,连击技id
		SaveSkillShortcut(skillSCConfigArray1, m_aSCSets1, NUM_HOSTSCSETS1);
		SaveSkillShortcut(skillSCConfigArray2, m_aSCSets2, NUM_HOSTSCSETS2);
		SaveSkillGrpShortcut(skillGrpSCConfigArray1, m_aSCSets1, NUM_HOSTSCSETS1);
		SaveSkillGrpShortcut(skillGrpSCConfigArray2, m_aSCSets2, NUM_HOSTSCSETS2);
							
		// 2.删除所有相关技能

		int i(0);
		for (i=0; i < NUM_HOSTSCSETS1; i++)
		{
			if (m_aSCSets1[i])
				m_aSCSets1[i]->RemoveSkillShortcuts();
		}		
		for (i=0; i < NUM_HOSTSCSETS2; i++)
		{
			if (m_aSCSets2[i])
				m_aSCSets2[i]->RemoveSkillShortcuts();
		}
				
		//	Release positive skills
		for (i=0; i < m_aPtSkills.GetSize(); i++)
			delete m_aPtSkills[i];		
		m_aPtSkills.RemoveAll(false);
		
		//	Release passive skills
		for (i=0; i < m_aPsSkills.GetSize(); i++)
			delete m_aPsSkills[i];		
		m_aPsSkills.RemoveAll(false);
	}

	// 加载新技能数据
	GNET::ElementSkill::LoadSkillData(pCmd);

	for (int i=0; i < (int)pCmd->skill_count; i++)
	{
		const cmd_skill_data::SKILL& Data = pCmd->skill_list[i];
		CECSkill* pSkill = new CECSkill(Data.id_skill, Data.level);

		if (pSkill->GetType() != CECSkill::TYPE_PASSIVE &&
			pSkill->GetType() != CECSkill::TYPE_PRODUCE &&
			pSkill->GetType() != CECSkill::TYPE_LIVE)
			m_aPtSkills.Add(pSkill);
		else
			m_aPsSkills.Add(pSkill);
	}

	//替换快捷栏上技能,包括连击技
	if (HostIsReady())
	{
		ConvertSkillShortcut(skillSCConfigArray1);
		AssignSkillShortcut(skillSCConfigArray1, m_aSCSets1);
		ConvertSkillShortcut(skillSCConfigArray2);
		AssignSkillShortcut(skillSCConfigArray2, m_aSCSets2);
		ConvertComboSkill();
		ValidateSkillGrpShortcut(skillGrpSCConfigArray1);
		AssignSkillGrpShortcut(skillGrpSCConfigArray1, m_aSCSets1);
		ValidateSkillGrpShortcut(skillGrpSCConfigArray2);
		AssignSkillGrpShortcut(skillGrpSCConfigArray2, m_aSCSets2);
	}

	if (HostIsReady())
	{
		// 仙魔转换时、更新现有各种快捷方式及界面，以除去旧有技能的影响（或指针）
		CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUIMan->UpdateSkillRelatedUI();
	}
}

void CECHostPlayer::OnMsgHstEmbedItem(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_embed_item* pCmd = (cmd_embed_item*)Msg.dwParam1;
	ASSERT(pCmd);

	CECIvtrItem* pEquip = m_pPack->GetItem(pCmd->equip_idx);
	CECIvtrItem* pTessera = m_pPack->GetItem(pCmd->chip_idx);

	if (!pEquip || !pTessera)
	{
		ASSERT(0);
		return;
	}

	m_pPack->RemoveItem(pCmd->chip_idx, 1);

	//	Refresh equip's data
	g_pGame->GetGameSession()->c2s_CmdGetItemInfo(IVTRTYPE_PACK, pCmd->equip_idx);
}

void CECHostPlayer::OnMsgHstClearTessera(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_clear_tessera* pCmd = (cmd_clear_tessera*)Msg.dwParam1;
	ASSERT(pCmd);

	CECIvtrItem* pEquip = m_pPack->GetItem(pCmd->equip_idx);
	if (!pEquip)
	{
		ASSERT(0);
		return;
	}

	AddMoneyAmount(-(int)pCmd->cost);

	//	Refresh equip's data
	g_pGame->GetGameSession()->c2s_CmdGetItemInfo(IVTRTYPE_PACK, (BYTE)pCmd->equip_idx);
}

void CECHostPlayer::OnMsgHstCostSkillPt(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_cost_skill_point* pCmd = (cmd_cost_skill_point*)Msg.dwParam1;
	ASSERT(pCmd);

	m_BasicProps.iSP -= pCmd->skill_point;
	a_ClampFloor(m_BasicProps.iSP, 0);
}

void CECHostPlayer::OnMsgHstLearnSkill(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_learn_skill* pCmd = (cmd_learn_skill*)Msg.dwParam1;
	ASSERT(pCmd);

	CECSkill* pSkill = GetNormalSkill(pCmd->skill_id);
	if (pSkill)
	{
		if (pCmd->skill_level)
		{
			pSkill->LevelUp();
			GNET::ElementSkill::SetLevel(pCmd->skill_id, pCmd->skill_level);
		}
		else
		{
			//	Remove this skill
			RemoveNormalSkill(pCmd->skill_id);
		}
	}
	else if (pCmd->skill_level)
	{
		if (!(pSkill = new CECSkill(pCmd->skill_id, pCmd->skill_level)))
		{
			ASSERT(pSkill);
			return;
		}

		if (!pSkill->GetJunior().Empty())
		{
			ReplaceJuniorSkill(pSkill);
		}
		else
		{
			if (pSkill->GetType() != CECSkill::TYPE_PASSIVE &&
				pSkill->GetType() != CECSkill::TYPE_PRODUCE &&
				pSkill->GetType() != CECSkill::TYPE_LIVE)
				m_aPtSkills.Add(pSkill);
			else
				m_aPsSkills.Add(pSkill);
		}

		GNET::ElementSkill::SetLevel(pCmd->skill_id, pCmd->skill_level);
	}

	//	Update UI
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->UpdateTeach();
	CECHostSkillModel::Instance().OnLearnSkill(pCmd->skill_id, pCmd->skill_level);
}

void CECHostPlayer::OnMsgHstFlySwordTime(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_flysword_time* pCmd = (cmd_flysword_time*)Msg.dwParam1;
	ASSERT(pCmd);

	CECInventory* pPack = (CECInventory*)GetPack(pCmd->where);
	if (!pPack) return;

	CECIvtrItem* pItem = pPack->GetItem(pCmd->index);
	if (!pItem || pItem->GetClassID() != CECIvtrItem::ICID_FLYSWORD)
	{
		ASSERT(0);
		return;
	}

	CECIvtrFlySword* pFlySword = (CECIvtrFlySword*)pItem;
	pFlySword->SetCurTime(pCmd->cur_time);
}

void CECHostPlayer::OnMsgHstProduceItem(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	ASSERT(pGameUI);

	if (Msg.dwParam2 == PRODUCE_START)
	{
		cmd_produce_start* pCmd = (cmd_produce_start*)Msg.dwParam1;
		ASSERT(pCmd);
		pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_PRODUCE_START, pCmd);
	}
	else if (Msg.dwParam2 == PRODUCE_END)
	{
		pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_PRODUCE_END, NULL);
	}
	else if (Msg.dwParam2 == PRODUCE_NULL)
	{
		cmd_produce_null* pCmd = (cmd_produce_null*)Msg.dwParam1;
		pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_PRODUCE_NULL, pCmd);
	}
	else if (Msg.dwParam2 == PRODUCE4_ITEM_INFO)
	{
		produce4_item_info* pCmd = (produce4_item_info*)Msg.dwParam1;
		pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_PRODUCE_PREVIEW,pCmd);
	}
}

void CECHostPlayer::OnMsgHstEngraveItem(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	ASSERT(pGameUI);

	if (Msg.dwParam2 == ENGRAVE_START)
	{
		cmd_engrave_start* pCmd = (cmd_engrave_start*)Msg.dwParam1;
		ASSERT(pCmd);
		pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_ENGRAVE_START, pCmd);
	}
	else if (Msg.dwParam2 == ENGRAVE_END)
	{
		pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_ENGRAVE_END, NULL);
	}
	else if (Msg.dwParam2 == ENGRAVE_RESULT)
	{
		cmd_engrave_result* pCmd = (cmd_engrave_result*)Msg.dwParam1;
		ASSERT(pCmd);
		pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_ENGRAVE_RESULT, pCmd);
	}
}

void CECHostPlayer::OnMsgHstAddonRegen(const ECMSG& Msg)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	ASSERT(pGameUI);
	
	pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_ADDONREGEN, (void*)&Msg);
}

void CECHostPlayer::OnMsgHstInvisibleObjList(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_invisible_obj_list *pCmd = (cmd_invisible_obj_list *)Msg.dwParam1;

	// only show the list when the cast player is host player
	if(pCmd->id == GetCharacterID())
	{
		PlayGfx(res_GFXFile(RES_GFX_INVISIBLE_DETECTED), NULL, 1.0, PLAYERMODEL_TYPEALL);

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if(pGameUI)
		{
			CDlgInvisibleList *pDlg = (CDlgInvisibleList *)pGameUI->GetDialog("Win_InvisibleList");
			if (pDlg)
			{
				pDlg->UpdateList(pCmd);
				return;
			}
		}
		// missing resource
		ASSERT(false);
	}
	else if(ISPLAYERID(pCmd->id))
	{
		if (CECElsePlayer* pPlayer = m_pPlayerMan->GetElsePlayer(pCmd->id)){
			pPlayer->InvisibleDetected();
		}
	}
}

void CECHostPlayer::OnMsgHstSetPlayerLimit(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_set_player_limit *pCmd = (cmd_set_player_limit *)Msg.dwParam1;
	if (pCmd->index >= 0 && pCmd->index < PLAYER_LIMIT_MAX)
		m_playerLimits.set(pCmd->index, pCmd->b != 0);
}

void CECHostPlayer::OnMsgHstDPSDPHRank(const ECMSG &Msg)
{
	using namespace S2C;

	cmd_dps_dph_rank *pCmd = (cmd_dps_dph_rank *)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgEarthBagRank *pDlgEarthBag = (CDlgEarthBagRank *)pGameUI->GetDialog("Win_EarthBagRank");
	if (pDlgEarthBag)
		pDlgEarthBag->UpdateRank(pCmd);
}

void CECHostPlayer::OnMsgHstBreakItem(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	ASSERT(pGameUI);

	if (Msg.dwParam2 == DECOMPOSE_START)
	{
		cmd_decompose_start* pCmd = (cmd_decompose_start*)Msg.dwParam1;
		ASSERT(pCmd);
		pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_SPLIT_START, pCmd);
	}
	else if (Msg.dwParam2 == DECOMPOSE_END)
	{
		pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_SPLIT_END, NULL);
	}
}

void CECHostPlayer::OnMsgHstTaskData(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == TASK_DATA)
	{
		cmd_task_data* pCmd = (cmd_task_data*)Msg.dwParam1;
		ASSERT(pCmd);

		int iActiveListSize = (int)pCmd->active_list_size;
		BYTE* pData = (BYTE*)pCmd + sizeof (size_t);
		void* pActiveListbuf = pData;
		pData += iActiveListSize;
		
		int iFinishedListSize = *(int*)pData;
		pData += sizeof (int);
		void* pFinishedListBuf = pData;
		pData += iFinishedListSize;

		int iFinishTimeListSize = *(int*)pData;
		pData += sizeof(int);
		void* pFinishTimeListBuf = pData;
		pData += iFinishTimeListSize;

		int iFinishedCountListSize = *(int*)pData;
		pData += sizeof(int);
		void* pFinishedCountListBuf = pData;
		pData += iFinishedCountListSize;

		int iStorageTasksListSize = *(int*)pData;
		pData += sizeof(int);
		void* pStorageTaskListBuf = pData;
		pData += iStorageTasksListSize;

		A3DRELEASE(m_pTaskInterface);

		if (!(m_pTaskInterface = new CECTaskInterface(this)))
		{
			glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECHostPlayer::OnMsgHstTaskData", __LINE__);
			return;
		}

		if (!m_pTaskInterface->Init(pActiveListbuf, iActiveListSize, pFinishedListBuf, iFinishedListSize, 
									pFinishTimeListBuf, iFinishTimeListSize, pFinishedCountListBuf, iFinishedCountListSize,pStorageTaskListBuf,iStorageTasksListSize))
		{
			a_LogOutput(1, "CECHostPlayer::OnMsgHstTaskData, failed to initialize task interface");
			return;
		}
	
		m_pTaskInterface->CheckPQEnterWorldInit();		
		
		//  check if player has equipped goblin 
		if( m_pEquipPack->GetItem(EQUIPIVTR_GOBLIN) != NULL)
		{
			CECIvtrGoblin* pIvtrGoblin = (CECIvtrGoblin*)m_pEquipPack->GetItem(EQUIPIVTR_GOBLIN);
			m_pGoblin = new CECHostGoblin();
			m_pGoblin->Init(pIvtrGoblin->GetTemplateID(), pIvtrGoblin, this);
		}

		//	Note: this command now is also used as the end flag of responding
		//		for GET_ALL_DATA request
		g_pGame->GetGameSession()->LoadConfigData();

		//	根据装备包裹更新装备技能到技能列表
		if (UpdateEquipSkills())
			UpdateEquipSkillCoolDown();
	}
	else if (Msg.dwParam2 == TASK_VAR_DATA)
	{
		cmd_task_var_data* pCmd = (cmd_task_var_data*)Msg.dwParam1;
		ASSERT(pCmd);
		if(m_pTaskInterface)
			OnServerNotify(m_pTaskInterface, pCmd->data, pCmd->size);
		else
			ASSERT(m_pTaskInterface);
	}
}

void CECHostPlayer::OnMsgHstTargetIsFar(const ECMSG& Msg)
{
	if( CmdNormalAttack(true, m_pComboSkill != NULL) )
		AP_ActionEvent(AP_EVENT_MELEEOUTOFRANGE, 1);
}

void CECHostPlayer::OnMsgHstCameraMode(const ECMSG& Msg)
{
	m_CameraCtrl.SceneryMode_Set(!m_CameraCtrl.SceneryMode_Get());
}

void CECHostPlayer::OnMsgHstPressCancel(const ECMSG& Msg)
{
	CECHPWork *pCurWork = NULL;
	if (pCurWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_TRACEOBJECT)){
		dynamic_cast<CECHPWorkTrace*>(pCurWork)->PressCancel();
		return;
	}
	if (pCurWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_HACKOBJECT)){
		g_pGame->GetGameSession()->c2s_CmdCancelAction();
		return;
	}
	if (pCurWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_USEITEM)){
		g_pGame->GetGameSession()->c2s_CmdCancelAction();
		return;
	}
	if (pCurWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_SPELLOBJECT)){		
		int iState = dynamic_cast<CECHPWorkSpell*>(pCurWork)->GetState();
		if (iState == CECHPWorkSpell::ST_INCANT){
			g_pGame->GetGameSession()->c2s_CmdCancelAction();
		}
		return;
	}
	if (pCurWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_PICKUP)){		
		if (dynamic_cast<CECHPWorkPick*>(pCurWork)->IsGather()){
			g_pGame->GetGameSession()->c2s_CmdCancelAction();
		}
		return;
	}
	if (pCurWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_CONCENTRATE)){		
		if (IsOperatingPet()){
			g_pGame->GetGameSession()->c2s_CmdCancelAction();
		}
		return;
	}
	if (pCurWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_CONGREGATE)){		
		if (IsCongregating()){
			g_pGame->GetGameSession()->c2s_CmdCancelAction();
		}
		return;
	}

	if (m_bUsingTrashBox || DoingSessionPose())
	{
		g_pGame->GetGameSession()->c2s_CmdCancelAction();
		return;
	}

	//	Cancel current selection
	if (m_idSelTarget)
	{
		SelectTarget(0);
		return;
	}

	//	Some work have lower priority
	if (pCurWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_MOVETOPOS)){
		dynamic_cast<CECHPWorkMove*>(pCurWork)->PressCancel();
		return;
	}
}

void CECHostPlayer::OnMsgHstRootNotify(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == HOST_NOTIFY_ROOT)
	{
		cmd_host_notify_root* pCmd = (cmd_host_notify_root*)Msg.dwParam1;
		m_dwLIES |= (1 << pCmd->type);

		if (pCmd->type != 3)
		{
			//	Force pull host to specified position
			SetPos(pCmd->pos);
		}
		
		if (IsRooting())
		{
			if (m_pWorkMan->IsFollowing()){
				m_pWorkMan->FinishRunningWork(CECHPWork::WORK_FOLLOW);
			}
			if (m_pWorkMan->IsMovingToPosition()){
				m_pWorkMan->FinishRunningWork(CECHPWork::WORK_MOVETOPOS);
			}
			if (m_pWorkMan->IsTracing()){
				m_pWorkMan->FinishRunningWork(CECHPWork::WORK_TRACEOBJECT);
			}
		}
	}
	else if (Msg.dwParam2 == HOST_DISPEL_ROOT)
	{
		cmd_host_dispel_root* pCmd = (cmd_host_dispel_root*)Msg.dwParam1;
		m_dwLIES &= ~(1 << pCmd->type);
	}
}

void CECHostPlayer::OnMsgHstStopAttack(const ECMSG& Msg)
{
	using namespace S2C;

	m_bMelee = false;

	// if there is an attack event currently, we should let it fire now.
	ClearComActFlagAllRankNodes(true);

	cmd_host_stop_attack* pCmd = (cmd_host_stop_attack*)Msg.dwParam1;
	ASSERT(pCmd);

	/*	If attack stopped for target is leave too far, trace it and continue
		attacking. Stop reason defined as below:

		0x00: attack is canceled or host want to do some other things.
		0x01: unable to attack anymore (no ammo, weapon is broken etc.)
		0x02: invalid target (target missed or died)
		0x04: target is out of range
	*/
	if (pCmd->iReason & 0x04)
	{
		if (!m_pWorkMan->IsMovingToPosition() &&
			!m_pWorkMan->IsTracing())
		{
			if( CmdNormalAttack(false, m_pComboSkill != NULL) )
				AP_ActionEvent(AP_EVENT_MELEEOUTOFRANGE, 1);
			else
			{
				m_pWorkMan->FinishRunningWork(CECHPWork::WORK_HACKOBJECT);
				AP_ActionEvent(AP_EVENT_STOPMELEE);
			}
		}
		else
		{
			AP_ActionEvent(AP_EVENT_STOPMELEE);
		}
	}
	else
	{
		m_pWorkMan->FinishRunningWork(CECHPWork::WORK_HACKOBJECT);
		AP_ActionEvent(AP_EVENT_STOPMELEE);
	}

#ifdef _SHOW_AUTOPOLICY_DEBUG
	a_LogOutput(1, "Stop Attack, Reason = %d", pCmd->iReason);
#endif
}

void CECHostPlayer::OnMsgHstJump(const ECMSG& Msg)
{
	// first of all see if we need to cancel sitdown work.
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		return;
	}

	if (!CanDo(CANDO_JUMP))
		return;

	float fVertSpeed = 10.0f;

	if (m_iJumpCount == 0)
	{
		if (m_iMoveEnv == MOVEENV_WATER)
		{
			if (!IsUnderWater())
				return;

			fVertSpeed = 7.0f;
			SetJumpInWater(true);
		}
		else if (!m_GndInfo.bOnGround)
			return;
	}

	if (InSlidingState())
		return;

	m_iJumpCount++;

	m_fVertSpeed = fVertSpeed;
	m_CDRInfo.vAbsVelocity.y = fVertSpeed;
	m_CDRInfo.fYVel = 0.0f;
	m_CDRInfo.vTPNormal.Clear();

	if(m_iJumpCount == 1)
	{
		m_MoveCtrl.SendMoveCmd(GetPos(), 2, g_vAxisY, m_CDRInfo.vAbsVelocity, m_iMoveMode, true);
//		BubbleText(BUBBLE_HITMISSED, 0);
	}

	PlayAction(ACT_JUMP_START);

	PlayAction(ACT_JUMP_LOOP, false, 0, true);
}

void CECHostPlayer::OnMsgHstHurtResult(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == BE_HURT)
	{
		cmd_be_hurt* pCmd = (cmd_be_hurt*)Msg.dwParam1;
		if (pCmd->damage)
			Damaged(pCmd->damage);
	}
	else if (Msg.dwParam2 == HURT_RESULT)
	{
		cmd_hurt_result* pCmd = (cmd_hurt_result*)Msg.dwParam1;
		if (pCmd->target_id == m_PlayerInfo.cid)
			return;		//	Host himself will receive BE_HURT, so ignore this.

		if (ISPLAYERID(pCmd->target_id))
		{
			CECElsePlayer* pTarget = m_pPlayerMan->GetElsePlayer(pCmd->target_id);
			if (pTarget)
				pTarget->Damaged(pCmd->damage);
		}
		else if (ISNPCID(pCmd->target_id))
		{
			CECNPC* pTarget = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(pCmd->target_id);
			if (pTarget)
				pTarget->Damaged(pCmd->damage);
		}
	}
}

void CECHostPlayer::OnMsgHstAttackOnce(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_attack_once* pCmd = (cmd_attack_once*)Msg.dwParam1;

	//	Decrease ammo and weapon endurance
	CECIvtrWeapon* pWeapon = (CECIvtrWeapon*)m_pEquipPack->GetItem(EQUIPIVTR_WEAPON);
	if (pWeapon)
	{
		if (pCmd->ammo_num)
		{
			//	Decrease ammo amount if we are using a long-distance weapon
			const IVTR_ESSENCE_WEAPON& Essence = pWeapon->GetEssence();
			if (Essence.weapon_type == WEAPONTYPE_RANGE)
				m_pEquipPack->RemoveItem(EQUIPIVTR_PROJECTILE, pCmd->ammo_num);

			//	If ammo will be used out soon, try to equip it from package automatically
			CECIvtrItem* pItem = m_pEquipPack->GetItem(EQUIPIVTR_PROJECTILE);
			if (pItem && pItem->GetPileLimit() &&
				(float)pItem->GetCount() / pItem->GetPileLimit() < 0.2f)
			{
				int iIndex = m_pPack->FindItem(pItem->GetTemplateID());
				if (iIndex >= 0 && !m_pPack->GetItem(iIndex)->IsFrozen())
					g_pGame->GetGameSession()->c2s_CmdMoveItemToEquip(iIndex, EQUIPIVTR_PROJECTILE);
			}
		}

		//	Reduce weapon endurance
		pWeapon->AddCurEndurance(WEAPON_RUIN_SPEED);
	}
	
	m_bPrepareFight = false;

	if (m_bMelee && m_pComboSkill && !m_pComboSkill->IsStop())
	{
		if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
			g_pGame->GetGameRun()->PostMessage(MSG_HST_CONTINUECOMBOSKILL, MAN_PLAYER, 0, 1, m_pComboSkill->GetGroupIndex());
		else
			m_pComboSkill->Continue(true);
	}
}

void CECHostPlayer::OnMsgHstPlayTrick(const ECMSG& Msg)
{
	// first of all see if we need to cancel sitdown work.
	if (m_pWorkMan->IsSitting())
	{
		g_pGame->GetGameSession()->c2s_CmdStandUp();
		return;
	}

	if (!m_TrickCnt.IsFull())
		return;

	m_TrickCnt.Reset();

	int iTrickAct;
	if (IsJumping())
		iTrickAct = ACT_TRICK_JUMP;
	else if (IsPlayingAction(ACT_RUN))
		iTrickAct = ACT_TRICK_RUN;
	else
		return;
	
	int iCurAction = GetLowerBodyAction();

	PlayAction(iTrickAct);
	g_pGame->GetGameSession()->c2s_CmdTrickAction(iTrickAct);

	if (IsValidAction(iCurAction))
		PlayAction(iCurAction, true, 200, true);

	// we need to notify help system that the user did a trick, to keep the check state uptodate.
	CECScriptMan * pScriptMan = g_pGame->GetGameRun()->GetUIManager()->GetScriptMan();
	if( pScriptMan )
	{
		pScriptMan->GetContext()->GetCheckState()->SetPlayerStatusIsPlayTrick(true);
	}

	GetTaskInterface()->SetEmotion(iTrickAct == ACT_TRICK_JUMP ? TaskInterface::CMD_JUMP_TRICKACTION:TaskInterface::CMD_RUN_TRICKACTION);
}

void CECHostPlayer::OnMsgHstFaction(const ECMSG& Msg)
{
	using namespace S2C;
	const cmd_mafia_info_notify* pNofify;

	switch (Msg.dwParam2)
	{
	case MAFIA_INFO_NOTIFY:

		pNofify = reinterpret_cast<const cmd_mafia_info_notify*>(Msg.dwParam1);

		if (pNofify->idPlayer == m_PlayerInfo.cid)
		{
			SetFactionID(pNofify->idFaction);
			SetFRoleID(pNofify->player_rank);
		}
		else
		{
			CECElsePlayer* pPlayer = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetElsePlayer(pNofify->idPlayer);

			if (pPlayer)
			{
				pPlayer->SetFactionID(pNofify->idFaction);
				pPlayer->SetFRoleID(pNofify->player_rank);
			}
		}

		if (pNofify->idFaction){
			CECFactionMan *pFactionMan = g_pGame->GetFactionMan();
			Faction_Info *pFaction = pFactionMan->GetFaction(pNofify->idFaction, true);
			if (pNofify->idPlayer == m_PlayerInfo.cid && pFaction && !pFactionMan->IsMemInfoReady())
				pFactionMan->RefreshMemList();
		}

		break;

	case MAFIA_TRADE_START:

		break;

	case MAFIA_TRADE_END:

		break;
		
    case PROTOCOL_FACTIONOPREQUEST_RE:
		g_pGame->GetFactionMan()->OnOpRequest(reinterpret_cast<const FactionOPRequest_Re*>(Msg.dwParam1));
		break;

    case PROTOCOL_FACTIONCREATE_RE:
		g_pGame->GetFactionMan()->OnFactionCreate(reinterpret_cast<const FactionCreate_Re*>(Msg.dwParam1));
		break;

    case PROTOCOL_FACTIONBROADCASTNOTICE_RE:
		g_pGame->GetFactionMan()->OnBroadcastNotice(reinterpret_cast<const FactionBroadcastNotice_Re*>(Msg.dwParam1));
		break;

    case PROTOCOL_FACTIONUPGRADE_RE:
		g_pGame->GetFactionMan()->OnUpgrade(reinterpret_cast<const FactionUpgrade_Re*>(Msg.dwParam1));
		break;

    case PROTOCOL_FACTIONDEGRADE_RE:
		g_pGame->GetFactionMan()->OnDegrade(reinterpret_cast<const FactionDegrade_Re*>(Msg.dwParam1));
		break;

	case PROTOCOL_FACTIONLISTMEMBER_RE:
		g_pGame->GetFactionMan()->OnListMember(reinterpret_cast<FactionListMember_Re*>(Msg.dwParam1));
		break;
		
	case PROTOCOL_GETFACTIONBASEINFO_RE:
		g_pGame->GetFactionMan()->OnBaseInfo(reinterpret_cast<GetFactionBaseInfo_Re*>(Msg.dwParam1));
		break;

	case PROTOCOL_FACTIONDISMISS_RE:
		g_pGame->GetFactionMan()->OnDismiss(reinterpret_cast<FactionDismiss_Re*>(Msg.dwParam1));
		break;

	case RPC_FACTIONINVITEJOIN:
		g_pGame->GetFactionMan()->OnInviteJoin(reinterpret_cast<FactionInviteJoin*>(Msg.dwParam1));
		break;

	case PROTOCOL_GETPLAYERFACTIONINFO_RE:
		g_pGame->GetFactionMan()->OnPlayerInfo(reinterpret_cast<GetPlayerFactionInfo_Re*>(Msg.dwParam1));
		break;

	case PROTOCOL_FACTIONCHANGPROCLAIM_RE:
		g_pGame->GetFactionMan()->OnChangeProclaim(reinterpret_cast<FactionChangProclaim_Re*>(Msg.dwParam1));
		break;
	case PROTOCOL_FACTIONRENAMEANNOUNCE:
		g_pGame->GetFactionMan()->OnFactionRename(reinterpret_cast<FactionRenameAnnounce*>(Msg.dwParam1));
		break;
	}

}

void CECHostPlayer::OnMsgHstTaskDeliver(const ECMSG& Msg)
{
	using namespace S2C;

	switch (Msg.dwParam2)
	{
	case TASK_DELIVER_REP:
	{
		cmd_task_deliver_rep* pCmd = (cmd_task_deliver_rep*)Msg.dwParam1;
		SetReputation(pCmd->cur_reputaion);

		//	Print notify text
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOTREP, pCmd->delta);
		break;
	}
	case TASK_DELIVER_EXP:
	{
		cmd_task_deliver_exp* pCmd = (cmd_task_deliver_exp*)Msg.dwParam1;
		m_BasicProps.iExp	+= pCmd->exp;
		m_BasicProps.iSP	+= pCmd->sp;
		
		if (pCmd->exp)
		{
			//	Print notify text
		//	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOTEXP, (int)pCmd->exp);
			BubbleText(BUBBLE_EXP, pCmd->exp);
		}

		if (pCmd->sp > 0)
		{
			//	Print notify text
		//	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOTSP, (int)pCmd->sp);
			BubbleText(BUBBLE_SP, pCmd->sp);
		}

		break;
	}
	case TASK_DELIVER_MONEY:
	{
		cmd_task_deliver_money* pCmd = (cmd_task_deliver_money*)Msg.dwParam1;
		SetMoneyAmount(pCmd->cur_money);

		//	Print notify text
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_PICKUPMONEY, pCmd->amount);
		BubbleText(BUBBLE_MONEY, pCmd->amount);
		break;
	}
	default:
	{
		ASSERT(0);
		return;
	}
	}
}

void CECHostPlayer::OnMsgHstReputation(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_host_reputation* pCmd = (cmd_host_reputation*)Msg.dwParam1;
	SetReputation(pCmd->reputation);
}

void CECHostPlayer::OnMsgHstItemIdentify(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_item_identify* pCmd = (cmd_item_identify*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->ServiceNotify(CECGameUIMan::NOTIFY_IDENTIFY_END, pCmd);
}

void CECHostPlayer::OnMsgHstSanctuary(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == ENTER_SANCTUARY)
	{
		cmd_object_enter_sanctuary* pCmd = (cmd_object_enter_sanctuary*)Msg.dwParam1;

		if( pCmd->id == m_PlayerInfo.cid )
		{
			m_bInSanctuary = true;

			//	Print a notify
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ENTERSHELTER);
		}
		else
		{
			m_bPetInSanctuary = true;

			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_PET_ENTERSHELTER);
		}
	}
	else if (Msg.dwParam2 == LEAVE_SANCTUARY)
	{
		cmd_object_leave_sanctuary* pCmd = (cmd_object_leave_sanctuary*)Msg.dwParam1;

		if( pCmd->id == m_PlayerInfo.cid )
		{
			m_bInSanctuary = false;

			//	Print a notify
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_LEAVESHELTER);
		}
		else
		{
			m_bPetInSanctuary = false;

			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_PET_LEAVESHELTER);
		}
	}
}

void CECHostPlayer::OnMsgHstCorrectPos(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_host_correct_pos* pCmd = (cmd_host_correct_pos*)Msg.dwParam1;

	SetPos(pCmd->pos);
	m_vVelocity.Clear();
	m_CDRInfo.vAbsVelocity.Clear();

	m_MoveCtrl.SetMoveStamp(pCmd->stamp);

	g_pGame->GetRTDebug()->OutputDebugInfo(RTDCOL_WARNING, _AL("correct pos !"));
}

void CECHostPlayer::OnMsgHstFriendOpt(const ECMSG& Msg)
{
	if (!m_pFriendMan)
		return;

	using namespace GNET;

	CECGameRun* pGameRun = g_pGame->GetGameRun();
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	switch (Msg.dwParam2)
	{
	case PROTOCOL_ADDFRIEND_RE:
	{
		AddFriend_Re* p = (AddFriend_Re*)Msg.dwParam1;
		switch (p->retcode)
		{
		case ERR_SUCCESS:
		{
			//	If the same id player has been in our list, notify UI to delete
			//	it at first
			CECFriendMan::FRIEND* pFriend = m_pFriendMan->GetFriendByID(p->info.rid);
			if (pFriend)
				pGameUI->FriendAction(p->info.rid, -1, CDlgFriendList::FRIEND_ACTION_FRIEND_DELETE, 0);
			
			if (m_pFriendMan->AddFriend(p->info, GAME_ONLINE))
				pGameUI->FriendAction(p->info.rid, p->info.gid, CDlgFriendList::FRIEND_ACTION_FRIEND_ADD, 0);

			break;
		}
		case ERR_FS_OFFLINE:	pGameRun->AddFixedMessage(FIXMSG_OFFLINE2);		break;
		case ERR_FS_TIMEOUT:	pGameRun->AddFixedMessage(FIXMSG_FRIEND_OT);	break;
		case ERR_FS_REFUSE:		pGameRun->AddFixedMessage(FIXMSG_REFUSEFRIEND);	break;
		}

		break;
	}
	case PROTOCOL_GETFRIENDS_RE:
	{
		GetFriends_Re* p = (GetFriends_Re*)Msg.dwParam1;
		m_pFriendMan->ResetFriends(*p);
		pGameUI->BuildFriendList();
		break;
	}
	case PROTOCOL_FRIENDEXTLIST:
	{
		FriendExtList* p = (FriendExtList*)Msg.dwParam1;
		m_pFriendMan->ResetFriendsEx(*p);
		break;
	}
	case PROTOCOL_SENDAUMAIL_RE:
	{
		SendAUMail_Re* p = (SendAUMail_Re*)Msg.dwParam1;

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		CDlgMailToFriends* pDlg = dynamic_cast<CDlgMailToFriends*>(pGameUI->GetDialog("Win_MailtoFriend"));
		if (p->result == ERR_SUCCESS)
		{
			pDlg->IncSendMailCount(p->roleid);
		}
		else
		{
			g_pGame->GetGameSession()->OutputLinkSevError(p->result);
			pDlg->SendmailFail();
		}

		break;
	}
	case PROTOCOL_SETGROUPNAME_RE:
	{
		SetGroupName_Re* p = (SetGroupName_Re*)Msg.dwParam1;
		if (p->retcode == ERR_SUCCESS)
		{
			if (!p->name.size())	//	Delete group
			{
				m_pFriendMan->RemoveGroup(p->gid);
				pGameUI->FriendAction(0, p->gid, CDlgFriendList::FRIEND_ACTION_GROUP_DELETE, 0);
			}
			else
			{
				ACString strName((const ACHAR*)p->name.begin(), p->name.size() / sizeof (ACHAR));
				if (m_pFriendMan->GetGroupByID(p->gid))
				{
					//	Change group name
					m_pFriendMan->ChangeGroupName(p->gid, strName);
					pGameUI->FriendAction(0, p->gid, CDlgFriendList::FRIEND_ACTION_GROUP_RENAME, 0);
				}
				else	//	Create new group
				{
					m_pFriendMan->AddGroup(p->gid, strName);
					pGameUI->FriendAction(0, p->gid, CDlgFriendList::FRIEND_ACTION_GROUP_ADD, 0);
				}
			}
		}

		break;
	}
	case PROTOCOL_SETFRIENDGROUP_RE:
	{
		SetFriendGroup_Re* p = (SetFriendGroup_Re*)Msg.dwParam1;
		if (p->retcode == ERR_SUCCESS)
		{
			m_pFriendMan->ChangeFriendGroup(p->friendid, p->gid);
			pGameUI->FriendAction(p->friendid, p->gid, CDlgFriendList::FRIEND_ACTION_FRIEND_UPDATE, 0);
		}

		break;
	}
	case PROTOCOL_DELFRIEND_RE:
	{
		DelFriend_Re* p = (DelFriend_Re*)Msg.dwParam1;
		if (p->retcode == ERR_SUCCESS)
		{
			m_pFriendMan->RemoveFriend(p->friendid);
			pGameUI->FriendAction(p->friendid, -1, CDlgFriendList::FRIEND_ACTION_FRIEND_DELETE, 0);
		}

		break;
	}
	case PROTOCOL_FRIENDSTATUS:
	{
		//	陷阱：我的单向好友从 GT 客户端登录时，也会收到此状态通知
		FriendStatus* p = (FriendStatus*)Msg.dwParam1;
		CECFriendMan::FRIEND *pFriend = m_pFriendMan->GetFriendByID(p->roleid);
		if (pFriend)
		{
			pGameUI->FriendOnlineNotify(p->roleid, pFriend->status, p->status);
			m_pFriendMan->ChangeFriendStatus(p->roleid, p->status);
			pGameUI->FriendAction(p->roleid, -1, CDlgFriendList::FRIEND_ACTION_FRIEND_UPDATE, p->status);
		}
		break;
	}
	case PROTOCOL_GETSAVEDMSG_RE:
	{
		GetSavedMsg_Re* p = (GetSavedMsg_Re*)Msg.dwParam1;
		if (p->retcode == ERR_SUCCESS)
			m_pFriendMan->ResetOfflineMsgs(*p);

		break;
	}
	case PROTOCOL_ADDFRIENDREMARKS_RE:		
		{
			AddFriendRemarks_Re* p = (AddFriendRemarks_Re*)Msg.dwParam1;
			switch (p->retcode)
			{
			case ERR_SUCCESS:
				{
					ACString strRemarks((const ACHAR*)p->friendremarks.begin(), p->friendremarks.size() / sizeof (ACHAR));
					if (m_pFriendMan->SetFriendRemarks(p->friendroleid, strRemarks)){
						pGameUI->FriendAction(p->friendroleid, -1, CDlgFriendList::FRIEND_ACTION_FRIEND_UPDATE_INPLACE, 0);
					}					
					break;
				}
			case ERR_FRIEND_REMARKS_LEN:
				pGameRun->AddFixedMessage(FIXMSG_INVALID_NAME_LEN);
				break;
			}
			break;
	}
	case RPC_ADDFRIENDRQST:
	{
		AddFriendRqst* p = (AddFriendRqst*)Msg.dwParam1;
		AddFriendRqstArg* pArg = (AddFriendRqstArg*)p->GetArgument();

		//	Get asker name and add it to player manager
		ACString strName((const ACHAR*)pArg->srcname.begin(), pArg->srcname.size() / sizeof (ACHAR));
		g_pGame->GetGameRun()->AddPlayerName(pArg->srcroleid, strName);

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		pGameUI->PopupFriendInviteMenu(pArg->srcroleid, strName, (DWORD)p);
		break;
	}
	}
}

void CECHostPlayer::OnMsgHstWayPoint(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameUIMan * pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	if (Msg.dwParam2 == ACTIVATE_WAYPOINT)
	{
		cmd_activate_waypoint* pCmd = (cmd_activate_waypoint*)Msg.dwParam1;
		m_aWayPoints.Add(pCmd->waypoint);

		// add to waypoints array
		pGameUI->GetMapDlgsMgr()->UpdateWayPoints(&pCmd->waypoint, 1, false);

		//	Print a notify message
		const CECMapDlgsMgr::PointMap& aWayPoints = pGameUI->GetMapDlgsMgr()->GetTransPoint();
		CECMapDlgsMgr::PointMap::const_iterator itr = aWayPoints.find(pCmd->waypoint);
		if(itr != aWayPoints.end())
		{
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_NEWWAYPOINT, (itr->second).strName);

			bool bCanPopUITips = true;
			int count = CECUIConfig::Instance().GetGameUI().GetTaskIDDisableWayPointsUITipsCount();
			// 检查身上是否有禁止弹出tips的任务
			for (int i=0;i<count;i++){
				int taskID = CECUIConfig::Instance().GetGameUI().GetTaskIDDisableWayPointsUITips(i);
				if(GetTaskInterface() && GetTaskInterface()->HasTask(taskID)){
					bCanPopUITips = false;
					break;
				}
			}
			// 弹出tips
			CECScriptMan * pScriptMan = g_pGame->GetGameRun()->GetUIManager()->GetScriptMan();
			if( pScriptMan && bCanPopUITips){
				pScriptMan->GetContext()->GetUI()->SetTipDialogTitleAndContent(AC2AS_CP(CP_UTF8,pGameUI->GetStringFromTable(11350)),AC2AS_CP(CP_UTF8,(itr->second).strName));
				pScriptMan->GetContext()->GetUI()->ShowTip(500,500,5000,300);
			}
		}	
	}
	else if (Msg.dwParam2 == WAYPOINT_LIST)
	{
		cmd_waypoint_list* pCmd = (cmd_waypoint_list*)Msg.dwParam1;

		m_aWayPoints.SetSize(pCmd->count, 16);
		for (size_t i=0; i < pCmd->count; i++)
			m_aWayPoints[i] = pCmd->list[i];

		// update the whole list
		pGameUI->GetMapDlgsMgr()->UpdateWayPoints(pCmd->list, pCmd->count, true);
	}
}

void CECHostPlayer::OnMsgHstBreathData(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == BREATH_DATA)
	{
		cmd_breath_data* pCmd = (cmd_breath_data*)Msg.dwParam1;
		m_BreathData.bDiving	= true;
		m_BreathData.iCurBreath	= pCmd->breath;
		m_BreathData.iMaxBreath	= pCmd->breath_capacity;
	}
	else if (Msg.dwParam2 == HOST_STOP_DIVE)
	{
		m_BreathData.bDiving = false;
	}
}

void CECHostPlayer::OnMsgHstSkillAbility(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_skill_ability* pCmd = (cmd_skill_ability*)Msg.dwParam1;
	GNET::ElementSkill::SetAbility(pCmd->skill_id, pCmd->skill_ability);
}

void CECHostPlayer::OnMsgHstCoolTimeData(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_cooltime_data* pCmd = (cmd_cooltime_data*)Msg.dwParam1;

	m_skillCoolTime.clear();
	
	for (int i=0; i < pCmd->count; i++)
	{
		const cmd_cooltime_data::item_t& item = pCmd->list[i];
		if (item.idx > GP_CT_SKILL_START)
		{
			//	Is skill cool time
			int idSkill = item.idx - GP_CT_SKILL_START;

			COOLTIME &ct = m_skillCoolTime[idSkill];
			ct.iCurTime = item.cooldown;
			ct.iMaxTime = item.max_cooltime;
			a_Clamp(ct.iCurTime, 0, ct.iMaxTime);

			CECSkill* pSkill = GetNormalSkill(idSkill);
			if (pSkill)
			{
				pSkill->StartCooling(item.max_cooltime, item.cooldown);
			} else if (pSkill = CECComboSkillState::Instance().GetInherentSkillByID(idSkill))  {
				pSkill->StartCooling(item.max_cooltime, item.cooldown);
			}
			else if (!GetEquipSkillByID(idSkill))
			{		
				//  Add to goblin skill list
				pSkill = new CECSkill(idSkill, 1);				
				pSkill->StartCooling(item.max_cooltime, item.cooldown);
				m_aGoblinSkills.Add(pSkill);
			}
		}
		else if (item.idx >= 0 && item.idx < GP_CT_MAX)
		{
			//	Other cool time
			COOLTIME& ct = m_aCoolTimes[item.idx];
			ct.iCurTime = item.cooldown;
			ct.iMaxTime	= item.max_cooltime;
			a_Clamp(ct.iCurTime, 0, ct.iMaxTime);
			
			if(item.idx >= GP_CT_SKILLCOMMONCOOLDOWN0 && item.idx <= GP_CT_SKILLCOMMONCOOLDOWN4)
			{
				// other player skills should be set public cool down too.
				unsigned int mask = 1 << (item.idx - GP_CT_SKILLCOMMONCOOLDOWN0);
				for(int i=0; i<GetPositiveSkillNum(); i++)
				{
					CECSkill* pSkill = GetPositiveSkillByIndex(i);
					if(pSkill && (pSkill->GetCommonCoolDown() & mask))
						pSkill->StartCooling(item.max_cooltime, item.cooldown);
				}
			}
		}
		else
		{
			ASSERT(item.idx >= 0);
		}
	}

	UpdateEquipSkillCoolDown();
}

void CECHostPlayer::OnMsgHstRevivalInquire(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_revival_inquire* pCmd = (cmd_revival_inquire*)Msg.dwParam1;
	
	m_fReviveExp = pCmd->exp_reduce;
}

void CECHostPlayer::OnMsgHstSetCoolTime(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_set_cooldown* pCmd = (cmd_set_cooldown*)Msg.dwParam1;

	if (pCmd->cooldown_index < 0)
	{
		ASSERT(pCmd->cooldown_index >= 0);
		return;
	}

	if (pCmd->cooldown_index < GP_CT_MAX)
	{
		COOLTIME& ct = m_aCoolTimes[pCmd->cooldown_index];
		ct.iCurTime = pCmd->cooldown_time;
		ct.iMaxTime	= pCmd->cooldown_time;
		a_ClampRoof(ct.iCurTime, ct.iMaxTime);
		
		if(pCmd->cooldown_index == GP_CT_CAST_ELF_SKILL)
		{		
			int i;
			//  other goblin skills should be set public cool down, 1 second
			for(i=0;i<m_aGoblinSkills.GetSize();i++)
			{
				if(m_aGoblinSkills[i] && m_aGoblinSkills[i]->GetCoolingCnt() == 0)
					m_aGoblinSkills[i]->StartCooling(GetCoolTime(GP_CT_CAST_ELF_SKILL), GetCoolTime(GP_CT_CAST_ELF_SKILL));
			}
			
			for(i=0; i<m_aPsSkills.GetSize();i++)
			{
				CECSkill* pSkill = GetPassiveSkillByIndex(i);
				if(pSkill && (pSkill->GetCommonCoolDown() & (1<<(pCmd->cooldown_index - GP_CT_SKILLCOMMONCOOLDOWN0))))
					pSkill->StartCooling(GetCoolTime(pCmd->cooldown_index), GetCoolTime(pCmd->cooldown_index));
			}
		}

		if(pCmd->cooldown_index >= GP_CT_SKILLCOMMONCOOLDOWN0 && pCmd->cooldown_index <= GP_CT_SKILLCOMMONCOOLDOWN4)
		{
			// other player skills should be set public cool down too.
			unsigned int mask = 1<<(pCmd->cooldown_index - GP_CT_SKILLCOMMONCOOLDOWN0);
			for(int i=0; i<GetPositiveSkillNum();i++)
			{
				CECSkill* pSkill = GetPositiveSkillByIndex(i);
				if(pSkill && (pSkill->GetCommonCoolDown() & mask))
					pSkill->StartCooling(GetCoolTime(pCmd->cooldown_index), GetCoolTime(pCmd->cooldown_index));
			}
			const std::map<unsigned int, CECSkill*>& inherentSkillMap = CECComboSkillState::Instance().GetInherentSkillMap();
			std::map<unsigned int, CECSkill*>::const_iterator it;
			for (it = inherentSkillMap.begin(); it != inherentSkillMap.end(); ++it) {
				it->second->StartCooling(GetCoolTime(pCmd->cooldown_index), GetCoolTime(pCmd->cooldown_index));
			}
		}
	}
	else if (pCmd->cooldown_index > GP_CT_SKILL_START)
	{
		int idSkill = pCmd->cooldown_index - GP_CT_SKILL_START;

		COOLTIME &ct = m_skillCoolTime[idSkill];
		ct.iCurTime = pCmd->cooldown_time;
		ct.iMaxTime = pCmd->cooldown_time;
		a_Clamp(ct.iCurTime, 0, ct.iMaxTime);

		CECSkill* pSkill = GetNormalSkill(idSkill);
		if (pSkill)
		{
			pSkill->StartCooling(pCmd->cooldown_time, pCmd->cooldown_time);
		}
		else if (pSkill = CECComboSkillState::Instance().GetInherentSkillByID(idSkill)) 
		{
			pSkill->StartCooling(pCmd->cooldown_time, pCmd->cooldown_time);
		}
		else if (!GetEquipSkillByID(idSkill))
		{
			ASSERT(pSkill);
		}
	}
	else
	{
		// This is a annoying assert and mean nothing, so we ignore it.
//		ASSERT(0);
	}

	UpdateEquipSkillCoolDown(pCmd->cooldown_index);
}

void CECHostPlayer::OnMsgHstChangeTeamLeader(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_change_team_leader* pCmd = (cmd_change_team_leader*)Msg.dwParam1;

	if (!m_pTeam)
	{
		ASSERT(m_pTeam);
		return;
	}

	CECTeamMan* pTeamMan = g_pGame->GetGameRun()->GetTeamMan();
	pTeamMan->ChangeTeamLeader(m_pTeam->GetLeaderID(), pCmd->new_leader);
}

void CECHostPlayer::OnMsgHstExitInstance(const ECMSG& Msg)
{
	CECGameUIMan * pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	using namespace S2C;
	cmd_exit_instance * pCmd = (cmd_exit_instance *) Msg.dwParam1;

	if( !pCmd || pCmd->idInst != g_pGame->GetGameRun()->GetWorld()->GetInstanceID() )
		return;

	pGameUI->PopupInstanceCountDown(pCmd->iLeftTime,pCmd->reason);

	if( pCmd->iLeftTime > 0 )
		m_nTimeToExitInstance = pCmd->iLeftTime * 1000;
	else
		m_nTimeToExitInstance = 0;
}

void CECHostPlayer::OnMsgHstChangeFace(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CECClipboard* pClipboard = g_pGame->GetGameRun()->GetClipboard();

	if (Msg.dwParam2 == CHANGE_FACE_START)
	{
		cmd_change_face_start* pCmd = (cmd_change_face_start*)Msg.dwParam1;
		CECIvtrItem* pItem = m_pPack->GetItem(pCmd->ivtr_idx);
		if (pItem)
			pItem->Freeze(true);

		//	Save host's custom data to clipboard
		pClipboard->SetData(CECClipboard::DATA_CUSTOMDATA, &m_CustomizeData, sizeof (m_CustomizeData));

		StartChangeFace();

		if (m_aEquips[EQUIPIVTR_HEAD] > 0 ||
			m_aEquips[EQUIPIVTR_FASHION_HEAD] > 0)
		{
			UpdateHairModel(true);			
		}

		pGameUI->PopupFaceLiftDialog();

		//	Adjust host's transparency
		AdjustTransparency(5.0f, A3DVECTOR3(1.0f, 0.0f, 0.0f), 10000);
	}
	else if (Msg.dwParam2 == CHANGE_FACE_END)
	{
		cmd_change_face_end* pCmd = (cmd_change_face_end*)Msg.dwParam1;
		CECIvtrItem* pItem = m_pPack->GetItem(pCmd->ivtr_idx);
		if (pItem)
			pItem->Freeze(false);

		StopChangeFace();
		pClipboard->EraseData(CECClipboard::DATA_CUSTOMDATA);

		// now restore player and camera's position
		pGameUI->GetCustomizeMgr()->RestorePlayer(this);

		if(InFashionMode())
		{
			UpdateHairModel(true, m_aEquips[EQUIPIVTR_FASHION_HEAD]);
		}
		else
		{
			UpdateHairModel(true, m_aEquips[EQUIPIVTR_HEAD]);
		}

		UpdateFollowCamera(false, 0);
	}
	else if (Msg.dwParam2 == PROTOCOL_COMMAND)
	{
		if (Msg.dwParam3 == GNET::PROTOCOL_SETCUSTOMDATA_RE)
		{
			GNET::SetCustomData_Re* p = (GNET::SetCustomData_Re*)Msg.dwParam1;
			if (p->result != ERR_SUCCESS)
			{
				//	Restore host's custom data
				int iDataSize = 0;
				void* pData = pClipboard->GetData(CECClipboard::DATA_CUSTOMDATA, &iDataSize);
				if (pData && iDataSize == sizeof (m_CustomizeData))
				{
					ChangeCustomizeData(*(PLAYER_CUSTOMIZEDATA*)pData, true);
				}

				pClipboard->EraseData(CECClipboard::DATA_CUSTOMDATA);
			}
		}
	}
}

void CECHostPlayer::OnMsgHstTeamMemberData(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_team_member_data* pCmd = (cmd_team_member_data*)Msg.dwParam1;
	ASSERT(pCmd);

	if (!m_pTeam || pCmd->idLeader != m_pTeam->GetLeaderID())
	{
		ASSERT(0);
		return;
	}

	//	If m_pTeam only has one member, it means the team is justed
	//	created, force to update team members' position
	if (m_pTeam->GetMemberNum() <= 1)
		m_TMPosCnt.Reset(true);

	//	Update team data
	m_pTeam->UpdateTeamData(*pCmd);

	AArray<int, int> aTemp;

	//	Ensure all member's info is ready
	for (int i=0; i < m_pTeam->GetMemberNum(); i++)
	{
		CECTeamMember* pMember = m_pTeam->GetMemberByIndex(i);
		if (pMember->IsInfoReady())
			continue;

		//	Can we find this player in current player list ?
		CECPlayer* pPlayer = m_pPlayerMan->GetPlayer(pMember->GetCharacterID());
		if (pPlayer)
		{
			//	Ok, we use ready-made player information
			pMember->SetProfession(pPlayer->GetProfession());
			pMember->SetGender(pPlayer->GetGender());
			pMember->SetInfoReadyFlag(true);
		}
		else
		{
			//	Member's information isn't in local place, try to get
			//	from server
			aTemp.Add(pMember->GetCharacterID());
		}
	}

	//	Are there some member's info need updating ?
	if (aTemp.GetSize())
		g_pGame->GetGameSession()->CacheGetPlayerBriefInfo(aTemp.GetSize(), aTemp.GetData(), 0);

	//	Update team UI
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->UpdateTeam();
}

void CECHostPlayer::OnMsgHstSetMoveStamp(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_set_move_stamp* pCmd = (cmd_set_move_stamp*)Msg.dwParam1;
	m_MoveCtrl.SetMoveStamp(pCmd->move_stamp);
}

void CECHostPlayer::OnMsgHstChatroomOpt(const ECMSG& Msg)
{
	using namespace GNET;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	switch (Msg.dwParam2)
	{
	case PROTOCOL_CHATROOMCREATE_RE:
	{
		pGameUI->ChannelAction(CDlgChannelChat::CHANNEL_ACTION_CREATE_RE, (void *)Msg.dwParam1);
		break;
	}
	case PROTOCOL_CHATROOMINVITE:
		pGameUI->ChannelAction(CDlgChannelChat::CHANNEL_ACTION_INVITE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_CHATROOMINVITE_RE:
		pGameUI->ChannelAction(CDlgChannelChat::CHANNEL_ACTION_INVITE_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_CHATROOMJOIN_RE:
		pGameUI->ChannelAction(CDlgChannelChat::CHANNEL_ACTION_JOIN_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_CHATROOMLEAVE:
		pGameUI->ChannelAction(CDlgChannelChat::CHANNEL_ACTION_LEAVE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_CHATROOMEXPEL:
		pGameUI->ChannelAction(CDlgChannelChat::CHANNEL_ACTION_EXPEL, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_CHATROOMSPEAK:
		pGameUI->ChannelAction(CDlgChannelChat::CHANNEL_ACTION_SPEAK, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_CHATROOMLIST_RE:
		pGameUI->UpdateChannelList((void *)Msg.dwParam1);
		break;
	}
}

void CECHostPlayer::OnMsgHstMailOpt(const ECMSG& Msg)
{
	using namespace GNET;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	switch (Msg.dwParam2)
	{
	case PROTOCOL_ANNOUNCENEWMAIL:
		pGameUI->MailAction(CDlgMailList::MAIL_ACTION_ANNOUNCENEWMAIL_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_GETMAILLIST_RE:
		pGameUI->MailAction(CDlgMailList::MAIL_ACTION_GETMAILLIST_RE, (void *)Msg.dwParam1);
		break;
		
	case PROTOCOL_GETMAIL_RE:
		pGameUI->MailAction(CDlgMailList::MAIL_ACTION_GETMAIL_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_GETMAILATTACHOBJ_RE:
		pGameUI->MailAction(CDlgMailList::MAIL_ACTION_GETATTACH_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_DELETEMAIL_RE:
		pGameUI->MailAction(CDlgMailList::MAIL_ACTION_DELETEMAIL_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_PRESERVEMAIL_RE:
		pGameUI->MailAction(CDlgMailList::MAIL_ACTION_PRESERVEMAIL_RE, (void *)Msg.dwParam1);
		break;

	case PROTOCOL_PLAYERSENDMAIL_RE:
	{
		PlayerSendMail_Re* p = (PlayerSendMail_Re*)Msg.dwParam1;
		if (p->attach_obj_pos >= 0 && p->attach_obj_pos < m_pPack->GetSize())
			FreezeItem(IVTRTYPE_PACK, p->attach_obj_pos, false, true);
		pGameUI->MailAction(CDlgMailList::MAIL_ACTION_SENDMAIL_RE, (void *)Msg.dwParam1);

		break;
	}
	}
}

void CECHostPlayer::OnMsgHstVendueOpt(const ECMSG& Msg)
{
	using namespace GNET;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	switch (Msg.dwParam2)
	{
	case PROTOCOL_AUCTIONOPEN_RE:
		pGameUI->AuctionAction(CDlgAuctionList::AUCTION_ACTION_OPEN_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_AUCTIONBID_RE:
		pGameUI->AuctionAction(CDlgAuctionList::AUCTION_ACTION_BID_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_AUCTIONLIST_RE:
		pGameUI->AuctionAction(CDlgAuctionList::AUCTION_ACTION_LIST_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_AUCTIONCLOSE_RE:
		pGameUI->AuctionAction(CDlgAuctionList::AUCTION_ACTION_CLOSE_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_AUCTIONGET_RE:
		pGameUI->AuctionAction(CDlgAuctionList::AUCTION_ACTION_GET_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_AUCTIONATTENDLIST_RE:
		pGameUI->AuctionAction(CDlgAuctionList::AUCTION_ACTION_ATTENDLIST_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_AUCTIONEXITBID_RE:
		pGameUI->AuctionAction(CDlgAuctionList::AUCTION_ACTION_EXITBID_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_AUCTIONGETITEM_RE:
		pGameUI->AuctionAction(CDlgAuctionList::AUCTION_ACTION_GETITEM_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_AUCTIONLISTUPDATE_RE:
		pGameUI->AuctionAction(CDlgAuctionList::AUCTION_ACTION_LISTUPDATE_RE, (void *)Msg.dwParam1);
		break;		
	}
}

void CECHostPlayer::OnMsgHstViewOtherEquips(const ECMSG& Msg)
{
	if (!m_pEPEquipPack)
		return;

	using namespace S2C;
	cmd_player_equip_detail* pCmd = (cmd_player_equip_detail*)Msg.dwParam1;
	m_pEPEquipPack->ResetWithDetailData(0, (void*) &pCmd->content, pCmd->content_length);
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgEPEquip *pDlg = (CDlgEPEquip *)pGameUI->GetDialog("Win_EPEquip");
	pDlg->UpdateEquip(pCmd->id);
}

void CECHostPlayer::OnMsgHstPariahTime(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_pariah_time* pCmd = (cmd_pariah_time*)Msg.dwParam1;
	m_dwPariahTime = pCmd->pariah_time * 1000;
}

void CECHostPlayer::OnMsgHstPetOpt(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameRun* pGameRun = g_pGame->GetGameRun();

	switch (Msg.dwParam2)
	{
	case GAIN_PET:
	{
		const cmd_gain_pet* pCmd = (const cmd_gain_pet*)Msg.dwParam1;
		m_pPetCorral->AddPet(pCmd->slot_index, pCmd->data);

		//	Print a notify
		CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->slot_index);
		if (pPet)
		{
			if (pPet->IsFollowPet() || pPet->IsCombatPet() || pPet->IsEvolutionPet())
				pGameRun->AddFixedMessage(FIXMSG_PET_HATCH, pPet->GetName());
			else if (pPet->IsSummonPet())
				pGameRun->AddFixedMessage(FIXMSG_SUMMON_PET_APPEAR, pPet->GetName());
		}
	
		break;
	}
	case FREE_PET:
	{
		const cmd_free_pet* pCmd = (const cmd_free_pet*)Msg.dwParam1;

		//	Print a notify
		CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->slot_index);
		if (pPet)
		{
			if (pPet->IsFollowPet() || pPet->IsCombatPet() || pPet->IsEvolutionPet())
				pGameRun->AddFixedMessage(FIXMSG_PET_FREE, pPet->GetName());
			else if (pPet->IsSummonPet())
				pGameRun->AddFixedMessage(FIXMSG_SUMMON_PET_DISAPPEAR, pPet->GetName());
		}

		//	Remove pet from corral
		m_pPetCorral->FreePet(pCmd->slot_index, pCmd->pet_id);
		//	Update pet shortcuts
		UpdateFreedPetSC(pCmd->slot_index);

		break;
	}
	case SUMMON_PET:
	{
		const cmd_summon_pet* pCmd = (const cmd_summon_pet*)Msg.dwParam1;
		CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->slot_index);
		ASSERT(pPet && pPet->GetTemplateID() == pCmd->pet_tid);
		m_pPetCorral->SetActivePetIndex(pCmd->slot_index);
		m_pPetCorral->SetActivePetNPCID(pCmd->pet_pid);
		m_pPetCorral->SetActivePetLifetime(pCmd->life_time);

		if( g_pGame->GetConfigs()->GetGameSettings().bPetAutoSkill )
		{
			for( int i=0;i<pPet->GetSkillNum(CECPetData::EM_SKILL_DEFAULT);i++ )
			{
				const CECPetData::PETSKILL* pSkill = pPet->GetSkill(CECPetData::EM_SKILL_DEFAULT, i);
				if( pSkill && g_pGame->IsPetAutoSkill(pSkill->idSkill) )
					pPet->AddAutoSkill(pSkill->idSkill);
			}
		}

		OnPetSays(pCmd->pet_tid, pCmd->pet_pid, CECPetWords::TW_SUMMON);
		if (pPet->IsCombatPet() || pPet->IsSummonPet() || pPet->IsEvolutionPet()) 
			CDlgQuickBarPet::ResetAutoCastSkill();
		break;
	}
	case RECALL_PET:
	{
		const cmd_recall_pet* pCmd = (const cmd_recall_pet*)Msg.dwParam1;
		ASSERT(pCmd->slot_index == m_pPetCorral->GetActivePetIndex());

		int tid = pCmd->pet_id;
		int nid = m_pPetCorral->GetActivePetNPCID();
		
		//	宠物有话说
		switch(pCmd->reason)
		{
		case PET_RECALL_DEFAULT:
			OnPetSays(tid, nid, CECPetWords::TW_RECALL);
			break;

		case PET_RECALL_DEATH:
			OnPetSays(tid, nid, CECPetWords::TW_DEAD);
			break;

		case PET_RECALL_LIFE_EXHAUST:
			OnPetSays(tid, nid, CECPetWords::TW_DISAPPEAR);
			break;

		case PET_RECALL_SACRIFICE:
			OnPetSays(tid, nid, CECPetWords::TW_SACRIFICE);
			break;
		}

		CECPetData* pPet = m_pPetCorral->GetActivePet();
		if( pPet )
			pPet->OnPetDead();

		m_pPetCorral->SetActivePetIndex(-1);
		m_pPetCorral->SetActivePetNPCID(0);
		m_bPetInSanctuary = false;
		break;
	}
	case PLAYER_START_PET_OP:
	{
		const cmd_player_start_pet_op* pCmd = (const cmd_player_start_pet_op*)Msg.dwParam1;
		int iDoWhat;
		if (pCmd->op == 0)
			iDoWhat = CECHPWorkConcentrate::DO_SUMMONPET;
		else if (pCmd->op == 1)
			iDoWhat = CECHPWorkConcentrate::DO_RECALLPET;
		else if (pCmd->op == 2)
			iDoWhat = CECHPWorkConcentrate::DO_BANISHPET;
		else if (pCmd->op == 3)
			iDoWhat = CECHPWorkConcentrate::DO_RESTOREPET;
		else
			iDoWhat = CECHPWorkConcentrate::DO_UNKNOWN;

		m_PetOptCnt.SetPeriod(pCmd->delay * 50);
		m_PetOptCnt.Reset();

		CECHPWorkConcentrate* pWork = (CECHPWorkConcentrate*)m_pWorkMan->CreateWork(CECHPWork::WORK_CONCENTRATE);
		pWork->SetDoWhat(iDoWhat);
		m_pWorkMan->StartWork_p1(pWork);
		break;
	}
	case PLAYER_STOP_PET_OP:
	{
		if (IsOperatingPet())
			m_pWorkMan->FinishRunningWork(CECHPWork::WORK_CONCENTRATE);

		break;
	}
	case PET_RECEIVE_EXP:
	{
		const cmd_pet_receive_exp* pCmd = (const cmd_pet_receive_exp*)Msg.dwParam1;
		CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->slot_index);
		if (pPet)
		{
			ASSERT(pPet->GetTemplateID() == pCmd->pet_id);
			pPet->AddExp(pCmd->exp);
		}

		break;
	}
	case PET_LEVELUP:
	{
		const cmd_pet_levelup* pCmd = (const cmd_pet_levelup*)Msg.dwParam1;
		CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->slot_index);
		if (pPet)
		{
			ASSERT(pPet->GetTemplateID() == pCmd->pet_id);
			pPet->LevelUp(pCmd->level, pCmd->exp);
		}

		pGameRun->AddFixedMessage(FIXMSG_PET_LEVELUP, pCmd->level);
		break;
	}
	case PET_ROOM:
	{
		const cmd_pet_room* pCmd = (const cmd_pet_room*)Msg.dwParam1;
		m_pPetCorral->UpdatePets(*pCmd);
		
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		
		if( pGameUI->GetDialog("Win_Teach")->IsShow() )
			pGameUI->UpdateTeach();
		
		break;
	}
	case PET_ROOM_CAPACITY:
	{
		const cmd_pet_room_capacity* pCmd = (const cmd_pet_room_capacity*)Msg.dwParam1;
		m_pPetCorral->MagnifyPetSlots(pCmd->capacity);

		if( m_pPetCorral->HasInit() )
			pGameRun->AddFixedMessage(FIXMSG_PET_ROOM_SIZE, pCmd->capacity);
		else
			m_pPetCorral->SetHasInit(true);
		break;
	}
	case PET_HONOR_POINT:
	{
		const cmd_pet_honor_point* pCmd = (const cmd_pet_honor_point*)Msg.dwParam1;
		CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->index);
		if (pPet)
			pPet->SetIntimacy(pCmd->cur_honor_point);

		break;
	}
	case PET_HUNGER_GAUGE:
	{
		const cmd_pet_hunger_gauge* pCmd = (const cmd_pet_hunger_gauge*)Msg.dwParam1;
		CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->index);
		if (pPet)
			pPet->SetHunger(pCmd->cur_hunge_gauge);
		
		break;
	}
	case PET_DEAD:
	{
		const cmd_pet_dead* pCmd = (const cmd_pet_dead*)Msg.dwParam1;
		CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->pet_index);
		if (pPet)
		{
			pPet->SetHPFactor(0.0f);
			pPet->SetHP(0);
			pPet->OnPetDead();
		}

		pGameRun->AddFixedMessage(FIXMSG_PET_DEAD);
		break;
	}
	case PET_REVIVE:
	{
		const cmd_pet_revive* pCmd = (const cmd_pet_revive*)Msg.dwParam1;
		CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->pet_index);
		if (pPet)
			pPet->SetHPFactor(pCmd->hp_factor);

		break;
	}
	case PET_HP_NOTIFY:
	{
		const cmd_pet_hp_notify* pCmd = (const cmd_pet_hp_notify*)Msg.dwParam1;
		CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->pet_index);
		if (pPet)
		{
			int lastHP = pPet->GetHP();

			pPet->SetHPFactor(pCmd->hp_factor);
			pPet->SetHP(pCmd->cur_hp);
			
			pPet->SetMPFactor(pCmd->mp_factor);
			pPet->SetMP(pCmd->cur_mp);

			//	If HP is too low, popup a warning on pet's head
			int iLimit = (int)(pPet->CalcMaxHP() * 0.3f);
			if (pCmd->cur_hp < lastHP && lastHP && pCmd->cur_hp < iLimit)
			{
				CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(m_pPetCorral->GetActivePetNPCID());
				if (pNPC && pNPC->GetMasterID() == GetCharacterID())
					pNPC->BubbleText(CECNPC::BUBBLE_HPWARN, 0);
			}
		}

		break;
	}
	case PET_AI_STATE:
	{
		const cmd_pet_ai_state* pCmd = (const cmd_pet_ai_state*)Msg.dwParam1;

		//	宠物有话说
		const CECPetData *pPetData = m_pPetCorral->GetActivePet();
		if (pPetData)
		{
			int tid = pPetData->GetTemplateID();
			int nid = m_pPetCorral->GetActivePetNPCID();
			if (m_pPetCorral->GetMoveMode() != pCmd->move)
			{
				switch(pCmd->move)
				{
				case CECPetCorral::MOVE_FOLLOW:
					OnPetSays(tid, nid, CECPetWords::TW_FOLLOW);
					break;
				case CECPetCorral::MOVE_STAND:
					OnPetSays(tid, nid, CECPetWords::TW_STOP);
					break;
				}
			}
			if (m_pPetCorral->GetAttackMode() != pCmd->attack)
			{
				switch(pCmd->attack)
				{
				case CECPetCorral::ATK_DEFENSE:
					OnPetSays(tid, nid, CECPetWords::TW_DEFENSIVE);
					break;

				case CECPetCorral::ATK_POSITIVE:
					OnPetSays(tid, nid, CECPetWords::TW_OFFENSIVE);
					break;

				case CECPetCorral::ATK_PASSIVE:
					OnPetSays(tid, nid, CECPetWords::TW_PASSIVE);
					break;
				}
			}
		}

		m_pPetCorral->SetMoveMode(pCmd->move);
		m_pPetCorral->SetAttackMode(pCmd->attack);
		break;
	}
	case PET_SET_COOLDOWN:
	{
		const cmd_pet_set_cooldown* pCmd = (const cmd_pet_set_cooldown*)Msg.dwParam1;
		CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->pet_index);
		if (pPet)
			pPet->SetSkillCoolTime(pCmd->cooldown_index, pCmd->cooldown_time);
				
		break;
	}

	case SUMMON_PLANT_PET:
		{
			const cmd_summon_plant_pet *pCmd = (const cmd_summon_plant_pet *)Msg.dwParam1;
			if (pCmd)
				m_pPetCorral->PlantPetEnter(*pCmd);
			break;
		}

	case PLANT_PET_DISAPPEAR:
		{
			const cmd_plant_pet_disapper *pCmd = (const cmd_plant_pet_disapper *)Msg.dwParam1;
			if (pCmd)
				m_pPetCorral->PlantPetDisappear(*pCmd);
			break;
		}

	case PLANT_PET_HP_NOTIFY:
		{
			const cmd_plant_pet_hp_notify *pCmd = (const cmd_plant_pet_hp_notify *)Msg.dwParam1;
			if (pCmd)
				m_pPetCorral->PlantPetInfo(*pCmd);
			break;
		}

	case PET_PROPERTY:
		{
			const cmd_pet_property * pCmd = (const cmd_pet_property *)Msg.dwParam1;		
			CECPetData* pPet = m_pPetCorral->GetPetData(pCmd->pet_index);
			if (pPet)
			{
				pPet->SetExtendProps(pCmd->prop);
			}
			break;
		}
	case PET_REBUILD_INHERIT_START:		
	case PET_REBUILD_INHERIT_INFO:
	case PET_REBUILD_INHERIT_END:
	case PET_EVOLUTION_DONE:
	case PET_REBUILD_NATURE_START:
	case PET_REBUILD_NATURE_INFO:
	case PET_REBUILD_NATURE_END:
		{
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			if(pGameUI)
				((CDlgPreviewPetRebuild*)pGameUI->GetDialog("Win_PreviewPet"))->OnServerNotify(Msg.dwParam2,(void*)Msg.dwParam1);
			break;
		}

	default:
		ASSERT(0);
		break;
	}
}

void CECHostPlayer::OnMsgHstBattleOpt(const ECMSG& Msg)
{
	using namespace GNET;
	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	switch (Msg.dwParam2)
	{
	case PROTOCOL_BATTLECHALLENGE_RE:
		pGameUI->BattleAction(CDlgGuildMap::BATTLE_ACTION_BATTLECHALLENGE_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_BATTLECHALLENGEMAP_RE:
		pGameUI->BattleAction(CDlgGuildMap::BATTLE_ACTION_BATTLECHALLENGEMAP_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_BATTLEGETMAP_RE:
		pGameUI->BattleAction(CDlgGuildMap::BATTLE_ACTION_BATTLEGETMAP_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_BATTLEENTER_RE:
		pGameUI->BattleAction(CDlgGuildMap::BATTLE_ACTION_BATTLEENTER_RE, (void *)Msg.dwParam1);
		break;
	case PROTOCOL_BATTLESTATUS_RE:
		pGameUI->BattleAction(CDlgGuildMap::BATTLE_ACTION_BATTLESTATUS_RE, (void *)Msg.dwParam1);
		break;

	case HOST_ENTER_BATTLE:
	{
		const cmd_host_enter_battle* pCmd = (const cmd_host_enter_battle*)Msg.dwParam1;

		if (pCmd->battle_camp == GP_BATTLE_CAMP_NONE)	//	Leave battle
		{
			m_BattleInfo.nType = BT_NONE;
			m_BattleInfo.idBattle = 0;
			ChangeTankLeader(0, false);
		}
		else
		{
			m_BattleInfo.nType = BT_GUILD;
			m_BattleInfo.idBattle = pCmd->id_battle;
			m_BattleInfo.iEndTime = pCmd->end_time;
		}

		m_iBattleCamp = pCmd->battle_camp;
		m_BattleInfo.iResult = 0;
		m_BattleInfo.iResultCnt = 0;
		break;
	}
	case BATTLE_RESULT:
	{
		const cmd_battle_result* pCmd = (const cmd_battle_result*)Msg.dwParam1;
		m_BattleInfo.iResult = pCmd->result;
		m_BattleInfo.iResultCnt = 5000;

		if (m_BattleInfo.iResult == 1)
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_BAT_INVADER_WIN);
		else if (m_BattleInfo.iResult == 2 || m_BattleInfo.iResult == 3)
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_BAT_DEFENDER_WIN);
		
		break;
	}
	case BATTLE_SCORE:
	{
		const cmd_battle_score* pCmd = (const cmd_battle_score*)Msg.dwParam1;
		m_BattleInfo.iMaxScore_I = pCmd->invader_target;
		m_BattleInfo.iMaxScore_D = pCmd->def_target;
		m_BattleInfo.iScore_I = pCmd->invader_score;
		m_BattleInfo.iScore_D = pCmd->def_score;
		break;
	}
	}
}

void CECHostPlayer::OnMsgHstAccountPoint(const ECMSG& Msg)
{
	using namespace GNET;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	switch (Msg.dwParam2)
	{
	case PROTOCOL_SELLPOINT_RE:
	case PROTOCOL_GETSELLLIST_RE:
	case PROTOCOL_FINDSELLPOINTINFO_RE:
	case PROTOCOL_SELLCANCEL_RE:
	case PROTOCOL_BUYPOINT_RE:
	case PROTOCOL_ANNOUNCESELLRESULT:
		break;
	}
}

void CECHostPlayer::OnMsgHstGMOpt(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == GM_INVISIBLE)
	{
		const gm_cmd_invisible* pCmd = (const gm_cmd_invisible*)Msg.dwParam1;
		if (pCmd->is_visible)
		{
			m_dwGMFlags &= ~GMF_INVISIBLE;
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GM_INVISIBLE);
		}
		else
		{
			m_dwGMFlags |= GMF_INVISIBLE;
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GM_VISIBLE);

			//	进入 GM 隐身状态时，召回已召宠物
			CECPetCorral * pPetCorral = GetPetCorral();
			if (pPetCorral)
			{
				CECPetData * pPet = pPetCorral->GetActivePet();
				if (pPet && pPet->GetClass() >= 0)
					g_pGame->GetGameSession()->c2s_CmdPetRecall();
			}
		}
	}
	else
	{
		const gm_cmd_invincible* pCmd = (const gm_cmd_invincible*)Msg.dwParam1;
		if (pCmd->is_invincible)
		{
			m_dwGMFlags |= GMF_INVINCIBLE;
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GM_VINCIBLE);
		}
		else
		{
			m_dwGMFlags &= ~GMF_INVINCIBLE;
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GM_INVINCIBLE);
		}
	}
}

void CECHostPlayer::OnMsgHstRefineOpt(const ECMSG& Msg)
{
	using namespace S2C;
	const cmd_refine_result* pCmd = (const cmd_refine_result*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	((CDlgEquipRefine*)pGameUI->GetDialog("Win_EquipRefine"))->RefineResult(pCmd->result);
}

void CECHostPlayer::OnMsgHstItemBound(const ECMSG& Msg)
{
	using namespace S2C;
	const cmd_player_bind_success* pCmd = (const cmd_player_bind_success*)Msg.dwParam1;

	CECIvtrItem* pItem = m_pPack->GetItem(pCmd->inv_index);
	ASSERT(pItem && pItem->GetTemplateID() == pCmd->item_id);
	if (pItem)
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ITEMBOUND, pItem->GetName());
	}
}

void CECHostPlayer::OnMsgHstSafeLock(const ECMSG& Msg)
{
	S2C::notify_safe_lock* p= (S2C::notify_safe_lock*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgAutoLock *pDlgAutoLock = ((CDlgAutoLock*)pGameUI->GetDialog("Win_AutoLock"));
	pDlgAutoLock->InitAutoLockInfo(p);
	if (!pDlgAutoLock->IsLocked()){
		CDlgMatchProfileSetting *pDlgMatchSetting = dynamic_cast<CDlgMatchProfileSetting*>(pGameUI->GetDialog("Win_InfoMatch"));
		if (!pDlgMatchSetting->IsServerMaskInitialized()){
			pDlgMatchSetting->GetProfile(false, false);
		}
	}
}

void CECHostPlayer::OnMsgHstBuySellFail(const ECMSG& Msg)
{
	m_pEPBoothSPack->UnfreezeAllItems();
	m_pEPBoothBPack->UnfreezeAllItems();
	m_pPack->UnfreezeAllItems();
}

void CECHostPlayer::OnMsgHstHpSteal(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_player_hp_steal* pCmd = (cmd_player_hp_steal*)Msg.dwParam1;

	//	Popup notify bubble text 
	BubbleText(BUBBLE_ADD, pCmd->hp); 
}

void CECHostPlayer::OnMsgHstMultiExpInfo(const ECMSG &Msg)
{
	using namespace S2C;
	
	cmd_multi_exp_info* pCmd = (cmd_multi_exp_info*)Msg.dwParam1;

	m_multiExpLastTimeStamp = pCmd->last_timestamp;
	m_multiExpFactor = pCmd->enhance_factor;
}

void CECHostPlayer::OnMsgHstMultiExpState(const ECMSG &Msg)
{
	using namespace S2C;
	
	cmd_change_multi_exp_state* pCmd = (cmd_change_multi_exp_state*)Msg.dwParam1;

	bool hasEnhanceStateBeforeChange = !MultiExp_EnhanceStateEmpty();
	bool firstTime = !MultiExp_Inited();

	m_multiExpEnhanceTime = pCmd->enchance_time;
	m_multiExpBufferTime = pCmd->buffer_time;
	m_multiExpImpairTime = pCmd->impair_time;
	
	m_multiExpBeginTimesLeft = pCmd->activate_times_left;

	m_multiExpState = static_cast<MultiExpState>(pCmd->state);
	
	switch (m_multiExpState)
	{
	case MES_ENHANCE:
		m_multiExpExpireMoment = time(NULL) + pCmd->enchance_time;
		break;
	case MES_BUFFER:
		m_multiExpExpireMoment = time(NULL) + pCmd->buffer_time;
		break;
	case MES_IMPAIR:
		m_multiExpExpireMoment = time(NULL) + pCmd->impair_time;
		break;

	default:		
		m_multiExpExpireMoment = 0;
	}

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		CDlgExp *pDlgSetExp = static_cast<CDlgExp *>(pGameUI->GetDialog("Win_SetExp"));
		if (pDlgSetExp)
			pDlgSetExp->OnStateChange(firstTime);
		if (hasEnhanceStateBeforeChange && MultiExp_EnhanceStateEmpty()){
			pGameUI->AddInformation(CDlgInfo::INFO_SYSTEM,"",pGameUI->GetStringFromTable(8509), -1, 0, 0, 0);
		}
	}
}

void CECHostPlayer::OnMsgPlayerSitDown(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == OBJECT_SIT_DOWN)
	{
		if (m_pWorkMan->IsMovingToPosition() || 
			m_pWorkMan->IsTracing() ||
			m_pWorkMan->IsFollowing())
		{
			m_MoveCtrl.SendStopMoveCmd(GetPos(), GetGroundSpeed(), GP_MOVE_RUN);
		}

		m_dwStates |= GP_STATE_SITDOWN;
		CECHPWorkSit* pWork = (CECHPWorkSit*)m_pWorkMan->CreateWork(CECHPWork::WORK_SIT);
		pWork->SetBeSittingFlag(false);
		m_pWorkMan->StartWork_p1(pWork);

		GetTaskInterface()->SetEmotion(CECTaskInterface::CMD_EMOTION_SITDOWN);
	}
	else if (Msg.dwParam2 == OBJECT_STAND_UP)
	{
		m_dwStates &= ~GP_STATE_SITDOWN;
		CECHPWorkStand* pWork = (CECHPWorkStand*)m_pWorkMan->CreateWork(CECHPWork::WORK_STAND);
		m_pWorkMan->StartWork_p1(pWork);
		
		// Set face model render state to normal to avoid flush
		if (GetFaceModel() &&
			(GetRace() == RACE_GHOST || GetRace() == RACE_OBORO))
		{
			GetFaceModel()->GetA3DSkinModel()->SetMaterialMethod(A3DSkinModel::MTL_ORIGINAL);
			GetFaceModel()->GetA3DSkinModel()->SetTransparent(-1.0f);
		}
	}
}

void CECHostPlayer::OnMsgPlayerBaseInfo(const ECMSG& Msg)
{
	//	We have cached host's basic information when login, 
	//	this function is obosolete.
	ASSERT(0);
	return;

/*	using namespace GNET;
	
	PlayerBaseInfo_Re* p = (PlayerBaseInfo_Re*)Msg.dwParam1;
	const GRoleBase& base = p->player;

	ASSERT((int)base.id == m_PlayerInfo.cid);

	m_iProfession	= base.cls;
	m_iGender		= base.gender;

	m_PlayerInfo.crc_c = base.custom_stamp;

	//	Get player name and save into name cache
	m_strName = ACString((const ACHAR*)base.name.begin(), base.name.size() / sizeof (ACHAR));
	m_pPlayerMan->AddPlayerName(m_PlayerInfo.cid, m_strName);
	if (m_pPateName)
		m_pPateName->SetText(m_strName, false);

	//	Calculate player's AABB
	CalcPlayerAABB();

	//	Update upper body radius using new AABB
	m_CDRInfo.fRadius = (m_aabb.Extents.y * 2.0f - m_CDRInfo.fStepHeight) * 0.5f;
	m_AirCDRInfo.fRadius = m_CDRInfo.fRadius;

	//	Load player skeleton
	if (!LoadPlayerSkeleton(false))
	{
		a_LogOutput(1, "CECHostPlayer::OnMsgPlayerCustomData, Failed to load skeleton");
		return;
	}

	//	Load custom data
	if (base.custom_data.size() >= 4)
	{
		if (!ChangeCustomizeData(*(PLAYER_CUSTOMIZEDATA*)base.custom_data.begin()))
		{
			a_LogOutput(1, "CECHostPlayer::OnMsgPlayerCustomData, Failed to load custom data");
		//	return;
		}
	}
	else
		SetResReadyFlag(RESFG_CUSTOM, true);

	//	Get host's necessary data
	g_pGame->GetGameSession()->c2s_CmdGetAllData(true, true, false);
*/
}

void CECHostPlayer::OnMsgPlayerCustomData(const ECMSG& Msg)
{
}

void CECHostPlayer::OnMsgPlayerFly(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == OBJECT_TAKEOFF)
	{
		if(!(m_dwStates & GP_STATE_FLY))
		{
			m_dwStates |= GP_STATE_FLY;
			m_bRushFly	= false;

			CECHPWorkFly* pWork = (CECHPWorkFly*)m_pWorkMan->CreateWork(CECHPWork::WORK_FLYOFF);
			if (m_pWorkMan->IsFreeFalling())
			{
				pWork->m_bContinueFly = true;
				m_pWorkMan->StartWork_p1(pWork);
			}
			else
			{
				pWork->m_bContinueFly = false;
				m_pWorkMan->StartWork_p2(pWork);
			}
		}
	}
	else if (Msg.dwParam2 == OBJECT_LANDING)
	{
		if(m_dwStates & GP_STATE_FLY)
		{
			m_dwStates &= ~GP_STATE_FLY;

			if (IsDead() || m_bCandHangerOn || IsHangerOn())
				ShowWing(false);
			else
			{
				CECHPWorkFall* pWork = (CECHPWorkFall*)m_pWorkMan->CreateWork(CECHPWork::WORK_FREEFALL);
				pWork->SetFallType(CECHPWorkFall::TYPE_FLYFALL);
				m_pWorkMan->StartWork_p1(pWork);
			}

			//	Below two lines will fix the "host stand in air" bug.
			m_iMoveEnv = CECPlayer::MOVEENV_GROUND;
			m_CDRInfo.vTPNormal.Clear();
		}
	}
	else	//	HOST_RUSH_FLY
	{
		cmd_host_rush_fly* pCmd = (cmd_host_rush_fly*)Msg.dwParam1;
		m_bRushFly = pCmd->is_active ? true : false;
	}
}

void CECHostPlayer::OnMsgPlayerCastSkill(const ECMSG& Msg)
{
	using namespace S2C;

	bool bDoOtherThing = false;
	int idTarget = 0;

	bool bActionStartSkill = false;
	int iActionTime = 1000;
	CECPlayerWrapper* pWrapper = CECAutoPolicy::GetInstance().GetPlayerWrapper();

	switch (Msg.dwParam2)
	{
	case OBJECT_CAST_SKILL:
	{
		cmd_object_cast_skill* pCmd = (cmd_object_cast_skill*)Msg.dwParam1;
		ASSERT(pCmd->caster == m_PlayerInfo.cid);

		if (m_pCurSkill)
		{
			m_pCurSkill->EndCharging();
		}

		m_pCurSkill = GetPositiveSkillByID(pCmd->skill);
		if (!m_pCurSkill) m_pCurSkill = GetEquipSkillByID(pCmd->skill);
		if (!m_pCurSkill) m_pCurSkill = CECComboSkillState::Instance().GetInherentSkillByID(pCmd->skill);
		if (!m_pCurSkill)
		{
			ASSERT(m_pCurSkill);
			return;
		}

		if (m_pCurSkill->IsChargeable())
			m_pCurSkill->StartCharging(pCmd->time);

		int iWaitTime = -1;
		if (m_pCurSkill->GetExecuteTime() >= 0)
			iWaitTime = pCmd->time + m_pCurSkill->GetExecuteTime();

		CECHPWorkSpell* pWork = (CECHPWorkSpell*)m_pWorkMan->CreateWork(CECHPWork::WORK_SPELLOBJECT);
		pWork->PrepareCast(pCmd->target, m_pCurSkill, iWaitTime);
		m_pWorkMan->StartWork_p1(pWork);

		//	Start time counter for some type skill
		if (!m_pCurSkill->IsChargeable())
		{
			int iTime = pCmd->time;
			a_ClampFloor(iTime, 10);
			m_IncantCnt.SetPeriod(iTime);
			m_IncantCnt.Reset();
		}
		else
		{
			// make sure the counter is correct shown
			m_IncantCnt.Reset(true);
		}
		
		m_bSpellDSkill = false;

		TurnFaceTo(pCmd->target);

		m_idCurSkillTarget = pCmd->target;
		PlaySkillCastAction(m_pCurSkill->GetSkillID());

		bActionStartSkill = true;
		iActionTime = iWaitTime;
		g_pGame->RuntimeDebugInfo(0xffffffff, _AL("Cast skill(%d): %s"), m_pCurSkill->GetSkillID(), g_pGame->GetSkillDesc()->GetWideString(m_pCurSkill->GetSkillID() * 10));
		break;
	}
	case SKILL_PERFORM:
	{
		m_pPrepSkill = NULL;

		if (m_pCurSkill && m_pCurSkill->IsDurative())
			m_bSpellDSkill = true;

		break;
	}
	case HOST_STOP_SKILL:
	{
		m_pPrepSkill = NULL;

		CECSkill *pSkillToMatch = m_pCurSkill;	//	保存指针值用于后续清除操作
		if (m_pCurSkill)
		{
			ClearComActFlagAllRankNodes(true);
			
			if (((m_pComboSkill && !m_pComboSkill->IsStop()) || 
				m_pCurSkill->ChangeToMelee()) && !m_pWorkMan->HasDelayedWork())
			{
				bDoOtherThing = true;
				idTarget = m_idCurSkillTarget;
			}

			m_pCurSkill->EndCharging();
			m_pCurSkill = NULL;
		}

		AP_ActionEvent(AP_EVENT_STOPSKILL);

		if (pSkillToMatch){
			//	m_pWorkMan中当前运行Work为优先级更高、但 CECHPWorkSpell 排在优先级低队列或被Delay时，
			//	若此处无法准确结束，则当轮到 CECHPWorkSpell 执行时，客户端在此 CECHPWorkSpell 执行期间将无法响应
			//	重现方法：执行起飞后、马上使用回城技能
			m_pWorkMan->FinishWork(CECHPWorkSpellMatcher(pSkillToMatch));
		}
		StopSkillAttackAction();

		m_idCurSkillTarget = 0;
		break;
	}
	case SELF_SKILL_INTERRUPTED:
	{
		int skill_id = 0;
		m_pPrepSkill = NULL;

		CECSkill *pSkillToMatch = m_pCurSkill;	//	保存指针值用于后续清除操作
		if (m_pCurSkill)
		{
			skill_id = m_pCurSkill->GetSkillID();

			ClearComActFlagAllRankNodes(false);
			
			if (((m_pComboSkill && !m_pComboSkill->IsStop()) ||
				m_pCurSkill->ChangeToMelee()) && !m_pWorkMan->HasDelayedWork())
			{
				bDoOtherThing = true;
				idTarget = m_idCurSkillTarget;
			}

			m_pCurSkill->EndCharging();
			m_pCurSkill = NULL;
		}

		m_idCurSkillTarget = 0;
		
		if (pSkillToMatch){
			//	参照 HOST_STOP_SKILL 中描述
			m_pWorkMan->FinishWork(CECHPWorkSpellMatcher(pSkillToMatch));
		}
		StopSkillCastAction();

		//	Print a notify message
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_SKILLINTERRUPT);

		AP_ActionEvent(AP_EVENT_STOPSKILL);

		// 通知玩家技能被中断
		CECAutoPolicy::GetInstance().SendEvent_SkillInterrupt(skill_id);
		break;
	}
	case OBJECT_CAST_INSTANT_SKILL:
	{
	//	m_pPrepSkill = NULL;

		cmd_object_cast_instant_skill* pCmd = (cmd_object_cast_instant_skill*)Msg.dwParam1;
		ASSERT(pCmd->caster == m_PlayerInfo.cid);

		CECSkill* pSkill = GetPositiveSkillByID(pCmd->skill);
		if (!pSkill) pSkill = GetEquipSkillByID(pCmd->skill);
		if (!pSkill)
		{
			ASSERT(pSkill);
			return;
		}

		if (pCmd->target && pCmd->target != m_PlayerInfo.cid)
			TurnFaceTo(pCmd->target);

	//	m_idCurSkillTarget = pCmd->target;
		PlaySkillCastAction(pSkill->GetSkillID());
		bActionStartSkill = true;
		break;
	}
	case OBJECT_CAST_POS_SKILL:
	{
		cmd_object_cast_pos_skill* pCmd = (cmd_object_cast_pos_skill*)Msg.dwParam1;
		ASSERT(pCmd->caster == m_PlayerInfo.cid);

		CECSkill* pSkill = GetNormalSkill(pCmd->skill);
		if (!pSkill) pSkill = GetEquipSkillByID(pCmd->skill);
		if (!pSkill)
		{
			ASSERT(pSkill);
			break;
		}

		TurnFaceTo(pCmd->target);
		
		if(pSkill->GetRangeType() != CECSkill::RANGE_SLEF &&
		   pSkill->GetRangeType() != CECSkill::RANGE_SELFSPHERE &&
		   (pSkill->GetSkillID() == 1095 ||	//	如影随形
		   pSkill->GetSkillID() == 1278 ||	//	真·如影随形
		   pSkill->GetSkillID() == 1279 ||	//	狂·如影随形
		   pSkill->GetSkillID() == 2313))
		{
			A3DVECTOR3 vPos = pCmd->pos;
			if (!IsPosCollideFree(vPos))
			{
				//	Add a little height to ensure player's AABB won't embed with building
				vPos += g_vAxisY * 0.1f;
			}

			//	Ensure we are not under ground
			A3DVECTOR3 vNormal;
			float vTerrainHeight = g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(vPos, &vNormal);
			if (vPos.y < vTerrainHeight)
				vPos.y = vTerrainHeight;

			SetPos(vPos);

			m_CDRInfo.vTPNormal	= vPos.y <= vTerrainHeight + 0.1f ? vNormal : g_vOrigin;
			m_CDRInfo.fYVel		= 0.0f;
			m_CDRInfo.vAbsVelocity.Clear();
			ResetJump();

			m_MoveCtrl.SetHostLastPos(vPos);
			m_MoveCtrl.SetLastSevPos(vPos);

			//	Update camera
			UpdateFollowCamera(false, 10);
		}
		else
		{
 			CECHPWorkFMove* pWork = (CECHPWorkFMove*)m_pWorkMan->CreateWork(CECHPWork::WORK_FLASHMOVE);

			//	检查技能执行时长，避免浮点除0导致出错
			int nExecuteTime = pSkill->GetExecuteTime();
			a_ClampFloor(nExecuteTime, 50);

			if(pSkill->GetSkillID() == 1145 ||	// 刺客的瞬移技能需要播放特效：1145（百步神行决）
				pSkill->GetSkillID()== 1314 ||	//	真·百步神行诀
				pSkill->GetSkillID()== 1315 ||	//	狂·百步神行诀
				pSkill->GetSkillID() == 1362 ||	//	剑灵御心剑
				pSkill->GetSkillID() == 1690 ||	//	剑灵真·御心剑
				pSkill->GetSkillID() == 1691 ||	//	剑灵狂·御心剑
				pSkill->GetSkillID() == 1845 ||	//	右弦之翼，右跳
				pSkill->GetSkillID() == 1844 ||	//	左弦之翼，左跳
				pSkill->GetSkillID() == 1815 ||	//	武侠 冲锋
				pSkill->GetSkillID() == 2272 || //	一骑紫尘
				pSkill->GetSkillID() == 2315 ||	//	百步神行诀
				pSkill->GetSkillID() == 2285 ||	//	御心剑
				pSkill->GetSkillID() == 2340 ||	//	突袭
				pSkill->GetSkillID() == 2341 ||	//	突袭
				pSkill->GetSkillID() == 2342 ||	//	突袭
				pSkill->GetSkillID() == 2553 ||	//	无情游
				pSkill->GetSkillID() == 2740 ||	//	真·无情游
				pSkill->GetSkillID() == 2741 ||	//	狂·无情游
				pSkill->GetSkillID() == 2559 ||	//	刹那
				pSkill->GetSkillID() == 2752 ||	//	真·刹那
				pSkill->GetSkillID() == 2753	//	狂·刹那
				)
			{
				A3DVECTOR3 vPos = pCmd->pos;
				if (!IsPosCollideFree(vPos))
				{
					//	Add a little height to ensure player's AABB won't embed with building
					vPos += g_vAxisY * 0.1f;
				}

				//	Ensure we are not under ground
				A3DVECTOR3 vNormal;
				float vTerrainHeight = g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(vPos, &vNormal);
				if (vPos.y < vTerrainHeight)
					vPos.y = vTerrainHeight;

				m_CDRInfo.vTPNormal	= vPos.y <= vTerrainHeight + 0.1f ? vNormal : g_vOrigin;
				m_CDRInfo.fYVel		= 0.0f;
				m_CDRInfo.vAbsVelocity.Clear();
				ResetJump();

				pWork->PrepareMove(pCmd->pos, nExecuteTime * 0.001f, pSkill->GetSkillID());			
			}
			else{
				if (pSkill->GetRangeType() == CECSkill::RANGE_SLEF ||
					pSkill->GetRangeType() == CECSkill::RANGE_SELFSPHERE){
					m_CDRInfo.vTPNormal = m_MoveCtrl.m_vFlashTPNormal;
				}
				pWork->PrepareMove(pCmd->pos, nExecuteTime * 0.001f, 0);
			}

			m_pWorkMan->StartWork_p2(pWork);
			iActionTime = nExecuteTime;
		}

		bActionStartSkill = true;
		break;
	}
	case PLAYER_CAST_RUNE_SKILL:
	{
		cmd_player_cast_rune_skill* pCmd = (cmd_player_cast_rune_skill*)Msg.dwParam1;
		ASSERT(pCmd->caster == m_PlayerInfo.cid);

		if(m_pTargetItemSkill)
		{
			delete m_pTargetItemSkill;
			m_pTargetItemSkill = NULL;
		}

		if (!(m_pTargetItemSkill = new CECSkill(pCmd->skill, pCmd->level)))
		{
			ASSERT(m_pTargetItemSkill);
			return;
		}

		m_pCurSkill = m_pTargetItemSkill;			

		if (m_pCurSkill->IsChargeable())
			m_pCurSkill->StartCharging(pCmd->time);

		int iWaitTime = -1;
		if (m_pCurSkill->GetExecuteTime() >= 0)
			iWaitTime = pCmd->time + m_pCurSkill->GetExecuteTime();

		CECHPWorkSpell* pWork = (CECHPWorkSpell*)m_pWorkMan->CreateWork(CECHPWork::WORK_SPELLOBJECT);
		pWork->PrepareCast(pCmd->target, m_pCurSkill, iWaitTime);
		m_pWorkMan->StartWork_p1(pWork);

		//	Start time counter for some type skill
		if (!m_pCurSkill->IsChargeable())
		{
			int iTime = pCmd->time;
			a_ClampFloor(iTime, 10);
			m_IncantCnt.SetPeriod(iTime);
			m_IncantCnt.Reset();
		}
		else
		{
			// make sure the counter is correct shown
			m_IncantCnt.Reset(true);
		}
		
		m_bSpellDSkill = false;

		TurnFaceTo(pCmd->target);

		m_idCurSkillTarget = pCmd->target;
		PlaySkillCastAction(m_pCurSkill->GetSkillID());

		bActionStartSkill = true;
		iActionTime = iWaitTime;
		g_pGame->RuntimeDebugInfo(0xffffffff, _AL("Cast skill(%d): %s"), m_pCurSkill->GetSkillID(), g_pGame->GetSkillDesc()->GetWideString(m_pCurSkill->GetSkillID() * 10));
		break;
	}
	case PLAYER_CAST_RUNE_INSTANT_SKILL:
	{
	//	m_pPrepSkill = NULL;

		cmd_player_cast_rune_instant_skill* pCmd = (cmd_player_cast_rune_instant_skill*)Msg.dwParam1;
		ASSERT(pCmd->caster == m_PlayerInfo.cid);

		if(m_pTargetItemSkill)
		{
			delete m_pTargetItemSkill;
			m_pTargetItemSkill = NULL;
		}

		if (!(m_pTargetItemSkill = new CECSkill(pCmd->skill, pCmd->level)))
		{
			ASSERT(m_pTargetItemSkill);
			return;
		}
		
		if (pCmd->target && pCmd->target != m_PlayerInfo.cid)
			TurnFaceTo(pCmd->target);

	//	m_idCurSkillTarget = pCmd->target;
		PlaySkillCastAction(m_pTargetItemSkill->GetSkillID());
		bActionStartSkill = true;
		break;
	}
	case ERROR_MESSAGE:
		bDoOtherThing = true;
	//	m_pPrepSkill = NULL;
		break;
	
	default:
		ASSERT(0);
		break;
	}

	if( bActionStartSkill )
		AP_ActionEvent(AP_EVENT_STARTSKILL, iActionTime);

	if (bDoOtherThing)
	{
		if (m_pComboSkill && !m_pComboSkill->IsStop())
		{
			if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
				g_pGame->GetGameRun()->PostMessage(MSG_HST_CONTINUECOMBOSKILL, MAN_PLAYER, 0, 0, m_pComboSkill->GetGroupIndex());
			else
				m_pComboSkill->Continue(false);
		}
		else
		{
			if( idTarget && idTarget != m_PlayerInfo.cid )
				NormalAttackObject(idTarget, true);
		}
	}
}

void CECHostPlayer::OnMsgContinueComboSkill(const ECMSG &Msg)
{
	bool bMeleeing = (Msg.dwParam1 == 1);
	if( bMeleeing != m_bMelee ) bMeleeing = m_bMelee;
	int iGroupID = (int)Msg.dwParam2;
	if( m_pComboSkill && m_pComboSkill->GetGroupIndex() == iGroupID && !m_pComboSkill->IsStop() )
		m_pComboSkill->Continue(bMeleeing);
}

void CECHostPlayer::OnMsgPlayerKnockback(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_player_knockback* pCmd = (cmd_player_knockback*)Msg.dwParam1;
	ASSERT(pCmd->id == m_PlayerInfo.cid);

	StartPassiveMove(pCmd->pos, pCmd->time, CECHPWorkPassiveMove::PASSIVE_KNOCKBACK);
}

void CECHostPlayer::StartPassiveMove(const A3DVECTOR3& pos, int time, int type)
{
	A3DVECTOR3 vPos = pos;

	//	Add a little height to ensure player's AABB won't embed with building
	vPos += g_vAxisY * 0.1f;

	//	Ensure we are not under ground
	A3DVECTOR3 vNormal;
	float vTerrainHeight = g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(vPos, &vNormal);
	if (vPos.y < vTerrainHeight)
		vPos.y = vTerrainHeight;
	
	m_CDRInfo.vTPNormal	= vPos.y <= vTerrainHeight + 0.1f ? vNormal : g_vOrigin;
	m_CDRInfo.fYVel		= 0.0f;
	m_CDRInfo.vAbsVelocity.Clear();
	ResetJump();

	CECHPWorkPassiveMove* pWork = (CECHPWorkPassiveMove*)m_pWorkMan->CreateWork(CECHPWork::WORK_PASSIVEMOVE);
	pWork->PrepareMove(pos, time);	
	m_pWorkMan->StartWork_p2(pWork);
}

void CECHostPlayer::OnMsgPlayerEquipDisabled(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_player_equip_disabled* pCmd = (cmd_player_equip_disabled*)Msg.dwParam1;
	ASSERT(pCmd->id == m_PlayerInfo.cid);

	ChangeEquipDisableMask(pCmd->mask);
}

void CECHostPlayer::OnMsgPlayerGather(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == PLAYER_GATHER_START)
	{
		cmd_player_gather_start* pCmd = (cmd_player_gather_start*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->pid == m_PlayerInfo.cid);

		CECMatterMan* pMatterMan = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();
		CECMatter* pMatter = pMatterMan->GetMatter(pCmd->mid);
		if (pMatter && pMatter->IsMonsterSpiritMine()) {
			CECHPWorkUse* pWork = (CECHPWorkUse*)m_pWorkMan->CreateWork(CECHPWork::WORK_USEITEM);
			if (pWork) {
				pWork->SetParams(pCmd->mid, pCmd->use_time * 1000, pCmd->mid, true);
				m_pWorkMan->StartWork_p1(pWork);
			}
			StartMonsterSpiritConnectGfx(pCmd->mid, pMatter->GetPos());
		} else {
			CECHPWorkPick* pWork = (CECHPWorkPick*)m_pWorkMan->CreateWork(CECHPWork::WORK_PICKUP);
			if (pWork)
			{
				pWork->SetGather(true, pMatter ? pMatter->GetTemplateID() : 0);
				m_pWorkMan->StartWork_p1(pWork);
			}
		}

		//	Start time counter
		m_GatherCnt.SetPeriod(pCmd->use_time * 1000);
		m_GatherCnt.Reset();
	}
	else if (Msg.dwParam2 == PLAYER_GATHER_STOP)
	{
		cmd_player_gather_stop* pCmd = (cmd_player_gather_stop*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->pid == m_PlayerInfo.cid);
		m_pWorkMan->FinishRunningWork(CECHPWork::WORK_PICKUP);
		m_pWorkMan->FinishRunningWork(CECHPWork::WORK_USEITEM);
		StopMonsterSpiritConnectGfx();
	}
	else if (Msg.dwParam2 == MINE_GATHERED)
	{
		cmd_mine_gathered* pCmd = (cmd_mine_gathered*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->player_id == m_PlayerInfo.cid);
		DATA_TYPE DataType = g_pGame->GetElementDataMan()->get_data_type(pCmd->item_type, ID_SPACE_ESSENCE);
		if (DataType == DT_MONSTER_SPIRIT_ESSENCE) {
			StartMonsterSpiritBallGfx();
			m_CardHolder.gain_times++;
		}
	}
}

void CECHostPlayer::OnMsgPickupMatter(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_matter_pickup* pCmd = (cmd_matter_pickup*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->who == m_PlayerInfo.cid);

	CECHPWorkPick* pWork = (CECHPWorkPick*)m_pWorkMan->CreateWork(CECHPWork::WORK_PICKUP);
	if (pWork)
	{
		pWork->SetGather(false);
		m_pWorkMan->StartWork_p1(pWork);
	}
}

void CECHostPlayer::OnMsgPlayerDoEmote(const ECMSG& Msg)
{
	using namespace S2C;

	if (!m_pWorkMan->IsStanding() && 
		!m_pWorkMan->IsBeingBound())
		return;

	if (Msg.dwParam2 == OBJECT_DO_EMOTE)
	{
		cmd_object_do_emote* pCmd = (cmd_object_do_emote*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->id == m_PlayerInfo.cid);

		DoEmote(pCmd->emotion);

		if( m_iBuddyId )
		{
			CECPlayer* pBuddy = m_pPlayerMan->GetPlayer(m_iBuddyId);
			if (pBuddy)
				pBuddy->DoEmote(pCmd->emotion);
		}

		GetTaskInterface()->SetEmotion(pCmd->emotion);
	}
	else if (Msg.dwParam2 == OBJECT_EMOTE_RESTORE)
	{
		if (CECHPWork *pWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_STAND)){
			dynamic_cast<CECHPWorkStand *>(pWork)->SetPoseAction(ACT_STAND, false);
		}
	}
}

void CECHostPlayer::OnMsgDoConEmote(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == CON_EMOTE_REQUEST)
	{
		cmd_con_emote_request* pCmd = (cmd_con_emote_request*)Msg.dwParam1;
		ASSERT(pCmd);
	}
	else if (Msg.dwParam2 == DO_CONCURRENT_EMOTE)
	{
		cmd_do_concurrent_emote* pCmd = (cmd_do_concurrent_emote*)Msg.dwParam1;
		ASSERT(pCmd);

		if (CECHPWork *pWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_STAND)){
			dynamic_cast<CECHPWorkStand *>(pWork)->SetPoseAction(pCmd->emotion, false);
		}
	}
}

void CECHostPlayer::OnMsgPlayerChangeShape(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_player_chgshape* pCmd = (cmd_player_chgshape*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->idPlayer == m_PlayerInfo.cid);

	TransformShape(pCmd->shape);
}

void CECHostPlayer::OnMsgPlayerGoblinOpt(const ECMSG& Msg)
{
	using namespace S2C;
// 	cmd_elf_refine_activate* pCmd = (cmd_elf_refine_activate*)Msg.dwParam1;
//	ASSERT(pCmd && pCmd->idPlayer == m_PlayerInfo.cid);
	
	switch (Msg.dwParam2)
	{
	case ELF_REFINE_ACTIVATE:
	{
		if( !m_pGoblin )
		{
#ifndef	_PROFILE_MEMORY
			ASSERT(m_pGoblin);
#endif
			return;
		}
		m_pGoblin->ProcessMessage(Msg);
		break;
	}

	case CAST_ELF_SKILL:
	{
		if(!GetRenderGoblin())
			SetRenderGoblin(true);
		SetGoblinRenderCnt(5000);

		cmd_cast_elf_skill* pCmd = (cmd_cast_elf_skill*)Msg.dwParam1;
		ASSERT(pCmd && pCmd->pid == GetCharacterID());

		if(pCmd->target != m_PlayerInfo.cid)
			m_idCurSkillTarget = pCmd->target;

		m_pGoblin->ProcessMessage(Msg);
		break;
	}

	case ELF_CMD_RESULT:
	{
		cmd_elf_result* pCmd = (cmd_elf_result*)Msg.dwParam1;

		//	Get database data
		elementdataman* pDB = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		const void* pDBData = pDB->get_data_ptr(pCmd->result, ID_SPACE_ESSENCE, DataType);
		
		if (!pDBData)
		{
			ASSERT(pDBData);
			return;
		}
		GOBLIN_ESSENCE* pDBEssence = (GOBLIN_ESSENCE*)pDBData;

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

		switch(pCmd->cmd)
		{
		case ELF_LEVELUP:
			{
				int iGenius = (pCmd->param2>>16) & 0x0000ffff;
				int iAttribute = pCmd->param2 & 0x0000ffff;
				const wchar_t* szName = pDBEssence->name;

				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_LEVELUP, pCmd->param1);
	
				if(iGenius != 0)
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_GET_GENIUS, szName, iGenius);

				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_GET_STATUSPT, szName, 1);
			
				if(iAttribute-1 != 0)
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_GET_EXTRA_PT, szName, iAttribute-1);

				break;
			}
		case ELF_LEARN_SKILL:
			{
				const wchar_t* szName = g_pGame->GetSkillDesc()->GetWideString(pCmd->param1 * 10);
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_LEARN_SKILL, szName, pCmd->param2);
				((CDlgELFLearn *)(pGameUI->GetDialog("Win_ELFLearn")))->UpdateTeach(-1);
				break;
			}
		case ELF_FORGET_SKILL:
			{
				const wchar_t* szName = g_pGame->GetSkillDesc()->GetWideString(pCmd->param1 * 10);
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_FORGET_SKILL, szName, pCmd->param2);
				((CDlgELFRetrain*)pGameUI->GetDialog("Win_ELFRetrain"))->RefreshHostDetails();
				break;
			}	
		case ELF_REFINE:
			{
				const wchar_t* szName = pDBEssence->name;
				int iResult = 0;
				if(pCmd->param1 == 0)
				{
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_REFINE_SUCC, szName, pCmd->param2+1);
					iResult = 0;
				}
				else if(pCmd->param1 == 1)
				{
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_REFINE_FAIL, szName, pCmd->param2, szName, pCmd->param2);
					iResult = 1;
				}
				else if(pCmd->param1 == 2)
				{
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_REFINE_FAIL, szName, pCmd->param2,szName, pCmd->param2-1);
					iResult = 2;
				}
				else if(pCmd->param1 == 3)
				{
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_REFINE_FAIL, szName, pCmd->param2,szName, 0);
					iResult = 3;
				}
				
				((CDlgELFRefine*)pGameUI->GetDialog("Win_ELFRefine"))->RefineResult(iResult);
				break;
			}
			
		case ELF_DECOMPOSE:
			{
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_DESTROY, pCmd->param2, pCmd->param1);
				break;
			}			
		case ELF_DEC_ATTRIBUTE:
			{
				int agi = (pCmd->param1>>16) & 0x0000ffff;
				int str = (pCmd->param1) & 0x0000ffff;
				int eng = (pCmd->param2>>16) & 0x0000ffff;
				int vit = (pCmd->param2) & 0x0000ffff;
				
				if(str != 0)
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_WASHPT_STR, str);
				if(agi != 0)
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_WASHPT_AGI, agi);
				if(vit != 0)
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_WASHPT_VIT, vit);
				if(eng != 0)
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_WASHPT_ENG, eng);

				break;
			}	
		case ELF_ADD_GENIUS:
			{				
				((CDlgELFGenius*)pGameUI->GetDialog("Win_ELFGenius"))->RefreshHostDetails();
				break;
			}
		case ELF_EQUIP_ITEM:
			{
				((CDlgELFProp*)pGameUI->GetDialog("Win_CharacterELF"))->UpdateEquip();
				break;
			}

		case ELF_DESTROY_ITEM:
			{
				DATA_TYPE DataType1;
				const void* pDBData1 = pDB->get_data_ptr(pCmd->param1, ID_SPACE_ESSENCE, DataType1);
				
				if (!pDBData1)
				{
					ASSERT(pDBData1);
					return;
				}
				GOBLIN_EQUIP_ESSENCE* pDBEssence1 = (GOBLIN_EQUIP_ESSENCE*)pDBData1;

				const wchar_t* szName1 = pDBEssence1->name;
				if(pCmd->param2 > 0)
				{
					const void* pDBData2 = pDB->get_data_ptr(pCmd->param2, ID_SPACE_ESSENCE, DataType1);
					if (!pDBData1)
					{
						ASSERT(pDBData1);
						return;
					}
					GOBLIN_EQUIP_ESSENCE* pDBEssence2 = (GOBLIN_EQUIP_ESSENCE*)pDBData2;

					const wchar_t* szName2 = pDBEssence2->name;
					
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_EQUIP_REPLACE, szName1, szName2);
				}
				else
				{
					g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_EQUIP_REMOVE, szName1);				
				}

				break;
			}
		case ELF_RECHARGE:
			{
				DATA_TYPE DataType1;
				const void* pDBData1 = pDB->get_data_ptr(pCmd->result, ID_SPACE_ESSENCE, DataType1);
				
				if (!pDBData1)
				{
					ASSERT(pDBData1);
					return;
				}
				ELEMENT_ESSENCE* pDBEssence1 = (ELEMENT_ESSENCE*)pDBData1;
				
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_GET_STAMINA, pCmd->param1, pDBEssence1->name, pCmd->param2);

				break;
			}
		}
		break;
	}
	}
}

void CECHostPlayer::OnMsgBoothOperation(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	switch (Msg.dwParam2)
	{
	case OPEN_BOOTH_TEST:
	{
		//	Re-arrange booth sell and buy package so that items in booth
		//	last can be kept as many as possible.
		RearrangeBoothPacks();
		
		m_iBoothState = 1;
		pGameUI->PopupBoothDialog(true);
		break;
	}
	case SELF_OPEN_BOOTH:
		
		m_iBoothState = 2;
		pGameUI->PopupBoothDialog(false, true);

		// 如果有售卖凭证且没有买卖银票，开启自动转换银票
		if( GetCertificateID() > 0 &&
			m_pBoothSPack->FindItem(21652) == -1 &&
			m_pBoothBPack->FindItem(21652) == -1 )
			m_AutoYinpiao.open = true;
		break;

	case PLAYER_CLOSE_BOOTH:
	{
		cmd_player_close_booth* pCmd = (cmd_player_close_booth*)Msg.dwParam1;
		if (pCmd->pid == m_PlayerInfo.cid)
		{
			m_AutoYinpiao.open = false;
			m_iBoothState = 0;
			pGameUI->PopupBoothDialog(false);
		}
		else
		{
			ASSERT(0);
		}
		
		break;
	}
	case BOOTH_SELL_ITEM:
	{
		cmd_booth_sell_item* pCmd = (cmd_booth_sell_item*)Msg.dwParam1;
		ASSERT(pCmd);

		//	Give player a notify message
		int iBoothIndex = m_pBoothSPack->GetItemIndex(pCmd->inv_index);
		CECIvtrItem* pBoothItem = m_pBoothSPack->GetItem(iBoothIndex);
		if (pBoothItem)
		{
			struct tm* time = glb_GetFormatLocalTime();
			ACString strMsg;
			strMsg.Format(g_pGame->GetFixedMsgTab()->GetWideString(FIXMSG_BOOTHSELL), 
				 time->tm_hour, time->tm_min, pCmd->item_count, pBoothItem->GetName(), pBoothItem->GetUnitPrice());
			pGameUI->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_OK", strMsg, 0x0fffffff, 0, 0, 0);
		}

		//	Remove item from booth package
		m_pBoothSPack->RemoveItem(pCmd->inv_index, pCmd->item_count);

		//	Get item information
		CECIvtrItem* pItem = m_pPack->GetItem(pCmd->inv_index);
		if (pItem)
		{
			//	Keep item's template id
			int tid = pItem->GetTemplateID();
			m_pPack->RemoveItem(pCmd->inv_index, pCmd->item_count);

			//	Update shortcuts if all items have been sold
			if (!m_pPack->GetItem(pCmd->inv_index))
				UpdateRemovedItemSC(tid, IVTRTYPE_PACK, pCmd->inv_index);
		}
		else
		{
			ASSERT(pItem);
		}

		break;
	}
	case PLAYER_BOOTH_INFO:
	{
		cmd_player_booth_info* pCmd = (cmd_player_booth_info*)Msg.dwParam1;
		ASSERT(pCmd);

		if (pCmd->pid == m_PlayerInfo.cid)
		{
			ASSERT(pCmd->pid != m_PlayerInfo.cid);
			return;
		}

		m_pEPBoothSPack->RemoveAllItems();
		m_pEPBoothBPack->RemoveAllItems();

		m_idBoothTrade = pCmd->booth_id;
		BYTE* pData = (BYTE*) &pCmd->goods_list;
		int iCount = 0;

		while (iCount < (int)pCmd->count)
		{
			int idItem = *(int*)pData;
			pData += sizeof (int);

			if (idItem)
			{
				int iAmount = *(int*)pData;
				pData += sizeof (int);
				int iPrice = *(int*)pData;
				pData += sizeof (int);

				if (iAmount > 0)
				{
					int iExpireDate = *(int*)pData;
					pData += sizeof (int);

					WORD wDataLen = *(WORD*)pData;
					pData += sizeof (WORD);
					m_pEPBoothSPack->AddEPBoothItem(idItem, iExpireDate, iCount, iAmount, iPrice, pData, wDataLen);
					pData += wDataLen;
				}
				else
				{
					m_pEPBoothBPack->AddEPBoothItem(idItem, 0, iCount, -iAmount, iPrice, NULL, 0);
				}
			}

			iCount++;
		}

		break;
	}
	case BOOTH_TRADE_SUCCESS:

		if (m_iBoothState == 3)	//	If we are visiting other's booth
			g_pGame->GetGameSession()->c2s_CmdNPCSevGetContent(GP_NPCSEV_BOOTHSELL);

		break;
	}
}

void CECHostPlayer::OnMsgPlayerTravel(const ECMSG& Msg)
{
	using namespace S2C;
}

void CECHostPlayer::OnMsgPlayerPVP(const ECMSG& Msg)
{
	CECPlayer::OnMsgPlayerPVP(Msg);

	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	if (Msg.dwParam2 == PLAYER_ENABLE_PVP && PKLevelCheck())
	{
		cmd_player_enable_pvp* pCmd = (cmd_player_enable_pvp*)Msg.dwParam1;
		if (pCmd->type == PLAYER_PVP_PROTECTED)
			pGameUI->AddChatMessage(pGameUI->GetStringFromTable(9637), GP_CHAT_SYSTEM);
	}
	else if(Msg.dwParam2 == PLAYER_DISABLE_PVP && PKLevelCheck())
	{
		cmd_player_disable_pvp* pCmd = (cmd_player_disable_pvp*)Msg.dwParam1;
		if (pCmd->type == PLAYER_PVP_PROTECTED)
			pGameUI->AddChatMessage(pGameUI->GetStringFromTable(9636), GP_CHAT_SYSTEM);
	}
	else if (Msg.dwParam2 == HOST_PVP_COOLDOWN)
	{
		cmd_host_pvp_cooldown* pCmd = (cmd_host_pvp_cooldown*)Msg.dwParam1;
		m_pvp.dwCoolTime	= pCmd->cool_time * 1000;
		m_pvp.dwMaxCoolTime	= pCmd->max_cool_time * 1000;
	}
	else if (Msg.dwParam2 == HOST_ENABLE_FREEPVP)
	{
		cmd_host_enable_freepvp* pCmd = (cmd_host_enable_freepvp*)Msg.dwParam1;
		m_pvp.bFreePVP = pCmd->is_enable ? true : false;
	}
}

void CECHostPlayer::OnMsgPlayerUseItem(const ECMSG& Msg)
{
	using namespace S2C;

	switch (Msg.dwParam2)
	{
	case OBJECT_START_USE:
	case OBJECT_START_USE_T:
	{
		int idTarget, idItem;
		DWORD dwTime;

		if (Msg.dwParam2 == OBJECT_START_USE)
		{
			cmd_object_start_use* pCmd = (cmd_object_start_use*)Msg.dwParam1;
			idTarget	= 0;
			idItem		= pCmd->item;
			dwTime		= pCmd->time;
		}
		else
		{
			cmd_object_start_use_t* pCmd = (cmd_object_start_use_t*)Msg.dwParam1;
			idTarget	= pCmd->target;
			idItem		= pCmd->item;
			dwTime		= pCmd->time;
		}

		CECHPWorkUse* pNewWork = (CECHPWorkUse*)m_pWorkMan->CreateWork(CECHPWork::WORK_USEITEM);
		pNewWork->SetParams(idItem, dwTime, idTarget);
		m_pWorkMan->StartWork_p1(pNewWork);

		AP_ActionEvent(AP_EVENT_STARTUSEITEM, dwTime);
		break;
	}
	case OBJECT_CANCEL_USE:
	{
		m_pWorkMan->FinishRunningWork(CECHPWork::WORK_USEITEM);
		break;
	}
	case OBJECT_USE_ITEM:

		ASSERT(0);
		return;
	}
}

void CECHostPlayer::OnMsgPlayerChangeFace(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_player_chg_face* pCmd = (cmd_player_chg_face*)Msg.dwParam1;
	m_PlayerInfo.crc_c = pCmd->crc_c;
}

void CECHostPlayer::OnMsgPlayerBindOpt(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	ACString strMsg;
	switch (Msg.dwParam2)
	{
	case PLAYER_BIND_REQUEST:
	{
		cmd_player_bind_request* pCmd = (cmd_player_bind_request*)Msg.dwParam1;
		strMsg.Format(pGameUI->GetStringFromTable(680), g_pGame->GetGameRun()->GetPlayerName(pCmd->who, true));
		pGameUI->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_RequestBind", 
			strMsg, 30000, pCmd->who, 0, 0);
		break;
	}
	case PLAYER_BIND_INVITE:
	{
		cmd_player_bind_invite* pCmd = (cmd_player_bind_invite*)Msg.dwParam1;
		strMsg.Format(pGameUI->GetStringFromTable(681), g_pGame->GetGameRun()->GetPlayerName(pCmd->who, true));
		pGameUI->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_InviteBind", 
			strMsg, 30000, pCmd->who, 0, 0);
		AddRelatedPlayer(pCmd->who);
		break;
	}
	case PLAYER_BIND_REQUEST_REPLY:
	{
		cmd_player_bind_request_reply* pCmd = (cmd_player_bind_request_reply*)Msg.dwParam1;
		strMsg.Format(pGameUI->GetStringFromTable(682), g_pGame->GetGameRun()->GetPlayerName(pCmd->who, true));
		pGameUI->AddChatMessage(strMsg, GP_CHAT_MISC);
		break;
	}
	case PLAYER_BIND_INVITE_REPLY:
	{
		cmd_player_bind_invite_reply* pCmd = (cmd_player_bind_invite_reply*)Msg.dwParam1;
		strMsg.Format(pGameUI->GetStringFromTable(682), g_pGame->GetGameRun()->GetPlayerName(pCmd->who, true));
		pGameUI->AddChatMessage(strMsg, GP_CHAT_MISC);
		break;
	}
	case PLAYER_BIND_STOP:
	{
		//	Restore buddy's transparence
		if (m_iBuddyId)
		{
			CECElsePlayer* pBuddy = m_pPlayerMan->GetElsePlayer(m_iBuddyId);
			pBuddy->SetTransparent(-1.0f);
		}

		//	Detach buddy
		if (!m_bHangerOn)
			DetachBuddy(NULL);

		//	Stop current work and restore to stand state
		if (!m_pWorkMan->IsStanding())
			m_pWorkMan->FinishAllWork(true);

		
		GetTaskInterface()->SetEmotion(CECTaskInterface::CMD_EMOTION_BINDBUDDY);

		break;
	}
	}
}

void CECHostPlayer::OnMsgPlayerDuelOpt(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	ACString strMsg;

	switch (Msg.dwParam2)
	{
	case DUEL_RECV_REQUEST:
	{
		cmd_duel_recv_request* pCmd = (cmd_duel_recv_request*)Msg.dwParam1;
		if( pGameUI->PlayerIsBlack(pCmd->player_id) )
			break;
		
		strMsg.Format(pGameUI->GetStringFromTable(678), g_pGame->GetGameRun()->GetPlayerName(pCmd->player_id, true));
		pGameUI->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_InviteDuel", 
			strMsg, 30000, pCmd->player_id, 0, 0);
		AddRelatedPlayer(pCmd->player_id);
		break;
	}
	case DUEL_REJECT_REQUEST:
	{
		cmd_duel_reject_request* pCmd = (cmd_duel_reject_request*)Msg.dwParam1;
		if( pCmd->reason == 2 )
			strMsg.Format(pGameUI->GetStringFromTable(679), g_pGame->GetGameRun()->GetPlayerName(pCmd->player_id, true));
		else
			strMsg.Format(pGameUI->GetStringFromTable(697), g_pGame->GetGameRun()->GetPlayerName(pCmd->player_id, true));
		pGameUI->AddChatMessage(strMsg, GP_CHAT_MISC);
		break;
	}
	case DUEL_PREPARE:
	{
		cmd_duel_prepare* pCmd = (cmd_duel_prepare*)Msg.dwParam1;
		m_pvp.iDuelState = DUEL_ST_PREPARE;
		m_pvp.iDuelTimeCnt = pCmd->delay * 1000;
		m_pvp.iDuelRlt = 0;
		break;
	}
	case DUEL_CANCEL:

		break;

	case HOST_DUEL_START:
	{
		cmd_host_duel_start* pCmd = (cmd_host_duel_start*)Msg.dwParam1;
		m_pvp.iDuelState = DUEL_ST_INDUEL;
		m_pvp.idDuelOpp = pCmd->idOpponent;
		m_pvp.iDuelTimeCnt = 0;
		break;
	}
	case DUEL_STOP:
	{
		m_pvp.iDuelState = DUEL_ST_STOPPING;
		m_pvp.iDuelTimeCnt = 5000;
		break;
	}
	}
}

void CECHostPlayer::OnMsgPlayerLevel2(const ECMSG& Msg)
{
	CECPlayer::OnMsgPlayerLevel2(Msg);

	using namespace S2C;
	cmd_task_deliver_level2* pCmd = (cmd_task_deliver_level2*)Msg.dwParam1;
		
	//	Print notify text
	int iIndex;
	if (pCmd->level2 <= 8)
		iIndex = FIXMSG_LEVEL2_01 + pCmd->level2;
	else if (pCmd->level2 >= 20 && pCmd->level2 <= 22)
		iIndex = FIXMSG_LEVEL2_20 + pCmd->level2 - 20;
	else if (pCmd->level2 >= 30 && pCmd->level2 <= 32)
		iIndex = FIXMSG_LEVEL2_30 + pCmd->level2 - 30;
	else
	{
		ASSERT(0);
		return;
	}
	
	const ACHAR* szStr = g_pGame->GetFixedMsgTab()->GetWideString(iIndex);
	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_LEVEL2UP, szStr);
}

void CECHostPlayer::OnMsgHstGoblinInfo(const ECMSG& Msg)
{
	using namespace S2C;
	
	if( m_pGoblin == NULL)
		return;
	
	if(Msg.dwParam2 == ELF_EXP)
	{
		cmd_elf_exp* pCmd = (cmd_elf_exp*)Msg.dwParam1;
		ASSERT(pCmd);

		CECIvtrGoblin* pGoblin = (CECIvtrGoblin*)m_pEquipPack->GetItem(EQUIPIVTR_GOBLIN);
		if(!pGoblin)
		{
			ASSERT(pGoblin);
			return;
		}

		pGoblin->SetExp(pCmd->exp);	
	}

	m_pGoblin->ProcessMessage(Msg);
}
void CECHostPlayer::OnMsgPlayerPasswdChecked(const ECMSG& Msg)
{
	if(m_bFirstFashionOpen)
	{
		m_bFirstFashionOpen = false;
		
		//pop up a messagebox
		if(m_bFirstTBOpen && m_bTrashPsw)	// first enter game and has password
		{
			CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			pGameUI->MessageBox("",pGameUI->GetStringFromTable(7791), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));	
		}
	}
}
void CECHostPlayer::OnMsgPlayerProperty(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_player_property* pCmd = (cmd_player_property*)Msg.dwParam1;

	CDlgProfView::ProfView prof;
	CECPlayer* pPlayer = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(pCmd->id);
	if(!pPlayer)
		return;
	
	prof.name = pPlayer->GetName();						// 名字
	prof.level = pPlayer->GetBasicProps().iLevel;		// 等级
	prof.level2 = pPlayer->GetBasicProps().iLevel2;		// 修真
	prof.profession = pPlayer->GetProfession();
	prof.realm_level = pPlayer->GetRealmLevel();
	prof.reincarnation_count = pPlayer->GetReincarnationCount();

	prof.self_damagereduce = pCmd->self_damage_reduce;
	prof.self_prayspeed = pCmd->self_prayspeed;

	prof.hp = pCmd->hp;
	prof.mp = pCmd->mp;

	prof.prof[CDlgProfView::PHYSICAL_DAMAGE_LOW]	= pCmd->damage_low;			// 物理攻击下限
	prof.prof[CDlgProfView::MAGIC_DAMAGE_LOW]		= pCmd->damage_magic_low;	// 法术攻击下限
	prof.prof[CDlgProfView::ACCURATE]				= pCmd->attack;				// 准确度
	prof.prof[CDlgProfView::ATTACK_LEVEL]			= pCmd->attack_degree;		// 攻击等级
	prof.prof[CDlgProfView::PHYSICAL_DEFENCE]		= pCmd->defense;			// 物理防御	

	prof.prof[CDlgProfView::MAGIC_DEFENCE_GOLD]		= pCmd->resistance[0];		// 金防
	prof.prof[CDlgProfView::MAGIC_DEFENCE_WOOD]		= pCmd->resistance[1];		// 木防
	prof.prof[CDlgProfView::MAGIC_DEFENCE_WATER]	= pCmd->resistance[2];		// 水防
	prof.prof[CDlgProfView::MAGIC_DEFENCE_FAIR]		= pCmd->resistance[3];		// 火防
	prof.prof[CDlgProfView::MAGIC_DEFENCE_EARTH]	= pCmd->resistance[4];		// 土防

	prof.prof[CDlgProfView::ARMOR]					= pCmd->armor;				// 躲闪度
	prof.prof[CDlgProfView::DEFENCE_LEVEL]			= pCmd->defend_degree;		// 防御等级
	prof.prof[CDlgProfView::ATTACK_SPEED]			= pCmd->attack_speed;		// 普通频率
	prof.prof[CDlgProfView::RUN_SPEED]				= (int)(pCmd->run_speed*100);		// 移动速度
	prof.prof[CDlgProfView::CRIT_RATE]				= pCmd->crit_rate;			// 致命一击率

	prof.prof[CDlgProfView::DAMAGE_REDUCE]			= pCmd->damage_reduce;		// 装备物理减免百分比
	prof.prof[CDlgProfView::PRAYSPEED]				= pCmd->prayspeed;			// 吟唱速度百分比

	prof.prof[CDlgProfView::CRIT_DAMANGE_BONUS]     = pCmd->crit_damage_bonus;
	prof.prof[CDlgProfView::INVISIBLE_DEGREE]		= pCmd->invisible_degree;
	prof.prof[CDlgProfView::ANTI_INVISIBLE_DEGREE]	= pCmd->anti_invisible_degree;
	prof.prof[CDlgProfView::VIGOUR]					= pCmd->vigour;


	prof.attackmax[0] = pCmd->damage_high;
	prof.attackmax[1] = pCmd->damage_magic_high;


	CECGameUIMan* pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgProfView* pDialog = (CDlgProfView*)pGameUIMan->GetDialog("Win_ProfView");
	if(pDialog)
	{
		pDialog->SetProperty(prof);

		if(pDialog->IsShow())
			pDialog->Show(false);

		pDialog->Show(true);
	}
}

void CECHostPlayer::OnMsgHstFactionContribNotify(const ECMSG &Msg)
{
	using namespace S2C;
	const cmd_faction_contrib_notify *pCmd = (const cmd_faction_contrib_notify *)(Msg.dwParam1);

	if (!m_contribInfoInitialized)
	{
		//	刚登录接收到贡献度通知，无法显示增加量
		m_contribInfoInitialized = true;
	}
	else
	{
		//	收到新的贡献度通知，显示贡献度增加或消耗消息
		int delta(0);
		
		//	可兑换帮派经验的贡献度显示
		delta = pCmd->exp_contrib - m_contribInfo.exp_contrib;
		if (delta > 0)
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FACTION_EXP_CONTRIB_INCREASE, delta);
		else if (delta < 0)
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FACTION_EXP_CONTRIB_DECREASE, -delta);

		//	可消费贡献度显示
		delta = pCmd->consume_contrib - m_contribInfo.consume_contrib;
		if (delta > 0)
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FACTION_CONSUME_CONTRIB_INCREASE, delta);
		else if (delta < 0)
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FACTION_CONSUME_CONTRIB_DECREASE, -delta);

		//	帮派基地状态界面修改
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		CDlgFortressStatus * pDlgStatus = (CDlgFortressStatus *)pGameUI->GetDialog("Win_FortressStatus");
		if (pDlgStatus->IsShow())
			pDlgStatus->UpdateInfo();
	}

	m_contribInfo.consume_contrib = pCmd->consume_contrib;
	m_contribInfo.exp_contrib = pCmd->exp_contrib;
	m_contribInfo.cumulate_contrib = pCmd->cumulate_contrib;
}

class FortressBuildingSortOrder
{
public:
	FortressBuildingSortOrder(CECHostPlayer *pHost)
		: m_pHost(pHost)
	{}

	bool operator () (
		const CECHostPlayer::FACTION_FORTRESS_INFO::building_data &lhs,
		const CECHostPlayer::FACTION_FORTRESS_INFO::building_data &rhs)const
	{
		const FACTION_BUILDING_ESSENCE *pEssence1 = m_pHost->GetBuildingEssence(lhs.id);
		const FACTION_BUILDING_ESSENCE *pEssence2 = m_pHost->GetBuildingEssence(rhs.id);
		if (pEssence1 && pEssence2)
		{
			//	两者都有效，按 sub_type 排序，以在基地设施列表中维持固定顺序
			//
			return pEssence1->id_sub_type < pEssence2->id_sub_type;
		}
		if (pEssence1)
		{
			//	左侧有效，则应排前，无效都排后
			return true;
		}
		if (pEssence2)
		{
			//	右侧有效，则应排前，无效都排后
			return false;
		}
		//	两者均无效，不分先后
		return false;
	}

	CECHostPlayer *m_pHost;
};

void CECHostPlayer::OnMsgHstFactionFortressInfo(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_faction_fortress_info cmd;
	if (!cmd.Initialize((const void *)Msg.dwParam1, Msg.dwParam3))
		return;
	m_fortressInfo.faction_id = cmd.faction_id;
	m_fortressInfo.level = cmd.level;
	m_fortressInfo.exp = cmd.exp;
	m_fortressInfo.exp_today = cmd.exp_today;
	m_fortressInfo.exp_today_time = cmd.exp_today_time;
	m_fortressInfo.tech_point = cmd.tech_point;

	::memcpy(m_fortressInfo.technology, cmd.technology, sizeof(cmd.technology));
	::memcpy(m_fortressInfo.material, cmd.material, sizeof(cmd.material));

	m_fortressInfo.building.clear();
	m_fortressInfo.building.reserve(cmd.building.size());
	FACTION_FORTRESS_INFO::building_data dummy;
	for (size_t i = 0; i < cmd.building.size(); ++ i)
	{
		const cmd_faction_fortress_info::building_data & src = cmd.building[i];
		dummy.id = src.id;
		dummy.finish_time = src.finish_time;
		m_fortressInfo.building.push_back(dummy);
	}
	
	//	按建造时间进行排序显示
	BubbleSort(m_fortressInfo.building.begin(), m_fortressInfo.building.end(), FortressBuildingSortOrder(this));

	m_fortressInfo.health = cmd.health;
	
	//	通知界面更新	
	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	
	CDlgFortressInfo * pDlgInfo = (CDlgFortressInfo *)pGameUI->GetDialog("Win_FortressInfo");
	if (pDlgInfo && pDlgInfo->IsShow())
		pDlgInfo->UpdateInfo();

	CDlgFortressExchange * pDlgExchange = (CDlgFortressExchange *)pGameUI->GetDialog("Win_FortressExchange");
	if (pDlgExchange && pDlgExchange->IsShow())
		pDlgExchange->UpdateInfo();
	
	CDlgFortressBuild * pDlgBuild = (CDlgFortressBuild *)pGameUI->GetDialog("Win_FortressBuild");
	if (pDlgBuild && pDlgBuild->IsShow())
		pDlgBuild->UpdateInfo();	

	CDlgFortressContrib * pDlgContrib = (CDlgFortressContrib *)pGameUI->GetDialog("Win_FortressContrib");
	if (pDlgContrib && pDlgContrib->IsShow())
		pDlgContrib->UpdateInfo();
			
	CDlgFortressStatus * pDlgStatus = (CDlgFortressStatus *)pGameUI->GetDialog("Win_FortressStatus");
	if (pDlgStatus && pDlgStatus->IsShow())
		pDlgStatus->UpdateInfo();
}

void CECHostPlayer::OnMsgHstEnterFactionFortress(const ECMSG &Msg)
{
	using namespace S2C;
	const cmd_enter_factionfortress *pCmd = (const cmd_enter_factionfortress *)(Msg.dwParam1);
	m_fortressEnter.faction_id = pCmd->factionid;
	m_fortressEnter.role_in_war = pCmd->role_in_war;
	m_fortressEnter.end_time = pCmd->offense_endtime;
	
	//	修改阵营信息
	m_iBattleCamp = pCmd->role_in_war;

	//	获取基地帮派信息
	if (pCmd->factionid && pCmd->factionid != GetFactionID())
		g_pGame->GetFactionMan()->GetFaction(pCmd->factionid, true);

	if (pCmd->factionid)
	{
		//	获取我方基地信息，为基地中NPC服务做准备
		g_pGame->GetGameSession()->c2s_CmdGetFactionFortressInfo();
	}

	//	离开基地
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	PAUIDIALOG pDlgMini = pGameUI->GetDialog("Win_FortressMini");
	PAUIDIALOG pDlgStatus = pGameUI->GetDialog("Win_FortressStatus");
	if (IsInMyFortress())
	{
		//	显示基地小图标，随时点击显示我方基地信息
		if (!pDlgMini->IsShow())
			pDlgMini->Show(true);
	}
	else
	{
		if (pDlgMini->IsShow())
			pDlgMini->Show(false);
		if (pDlgStatus->IsShow())
			pDlgStatus->Show(false);
	}
}

void CECHostPlayer::OnMsgCongregate(const ECMSG& Msg)
{
	using namespace S2C;

	int conType = -1; // whether transfer msg to UI

	if(Msg.dwParam2 == CONGREGATE_REQUEST)
	{
		cmd_congregate_request* pCmd = (cmd_congregate_request*)Msg.dwParam1;
		conType = pCmd->type;

		// 迷你客户端忽略集结令
		if( g_pGame->GetConfigs()->IsMiniClient() )
			return;
	}
	else if(Msg.dwParam2 == REJECT_CONGREGATE)
	{
		cmd_reject_congregate* pCmd = (cmd_reject_congregate*)Msg.dwParam1;
		conType = pCmd->type;
	}
	else if(Msg.dwParam2 == CONGREGATE_START)
	{
		cmd_congregate_start* pCmd = (cmd_congregate_start*)Msg.dwParam1;
		conType = pCmd->type;

		CECHPWorkCongregate* pWork = (CECHPWorkCongregate*)m_pWorkMan->CreateWork(CECHPWork::WORK_CONGREGATE);
		pWork->SetTimeout(pCmd->type, pCmd->time);
		m_pWorkMan->StartWork_p1(pWork);		
	}
	else if(Msg.dwParam2 == CANCEL_CONGREGATE)
	{
		cmd_cancel_congregate* pCmd = (cmd_cancel_congregate*)Msg.dwParam1;
		conType = pCmd->type;

		if (CECHPWork * pWork = m_pWorkMan->GetRunningWork(CECHPWork::WORK_CONGREGATE)){
			if(dynamic_cast<CECHPWorkCongregate *>(pWork)->GetConType() == conType){
				m_pWorkMan->FinishRunningWork(CECHPWork::WORK_CONGREGATE);
			}
		}
	}

	if(conType >= 0)
	{
		AString strName;
		strName.Format("Win_Congregate%d", conType);
		
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		CDlgCongregate* pDlg = dynamic_cast<CDlgCongregate*>(pGameUI->GetDialog(strName));
		
		ASSERT(pDlg); // fail if unknown congregate type
		if(pDlg)
		{
			pDlg->OnCongregateAction(Msg.dwParam2, (void*)Msg.dwParam1);
		}
	}

	// ignore the broadcast
}

void CECHostPlayer::OnMsgTeleport(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_player_teleport* pCmd = (cmd_player_teleport*)Msg.dwParam1;

	if(pCmd->mode == CECHPWorkPassiveMove::PASSIVE_DIRECT)
	{
		// exchange pos directly
		SetPos(pCmd->pos);
		m_vVelocity.Clear();
		m_CDRInfo.vAbsVelocity.Clear();
	}
	else if(pCmd->mode == CECHPWorkPassiveMove::PASSIVE_PULL)
	{
		// pull by others
		StartPassiveMove(pCmd->pos, pCmd->use_time, CECHPWorkPassiveMove::PASSIVE_PULL);
	}
	else
	{
		// unknown type
		ASSERT(false);
	}
}

void CECHostPlayer::OnMsgForce(const ECMSG &Msg)
{
	CECGameUIMan* pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgForce* pDlg = dynamic_cast<CDlgForce*>(pGameUIMan->GetDialog("Win_Force"));

	using namespace S2C;
	switch(Msg.dwParam2)
	{
	case PLAYER_FORCE_DATA:
	{
		cmd_player_force_data* pCmd = (cmd_player_force_data*)Msg.dwParam1;
		m_idForce = pCmd->cur_force_id;
		for(size_t i=0;i<pCmd->count;i++)
		{
			CECHostPlayer::FORCE_INFO info;
			info.force_id = pCmd->entry_list[i].force_id;
			info.contribution = pCmd->entry_list[i].contribution;
			info.reputation = pCmd->entry_list[i].reputation;
			SetForceInfo(info.force_id, &info);
		}
		break;
	}
	case PLAYER_FORCE_DATA_UPDATE:
	{
		cmd_player_force_data_update* pCmd = (cmd_player_force_data_update*)Msg.dwParam1;

		const CECHostPlayer::FORCE_INFO* pInfo = GetForceInfo(pCmd->force_id);

		// check contribution delta
		int delta = pCmd->contribution - (!pInfo ? 0 : pInfo->contribution);
		if(delta)
			g_pGame->GetGameRun()->AddFixedChannelMsg(delta > 0 ? FIXMSG_GOTFORCECON : FIXMSG_LOSTFORCECON, GP_CHAT_FIGHT, abs(delta));

		// check reputation delta
		delta = pCmd->reputation - (!pInfo ? 0 : pInfo->reputation);
		if(delta)
			g_pGame->GetGameRun()->AddFixedChannelMsg(delta > 0 ? FIXMSG_GOTFORCEREP : FIXMSG_LOSTFORCEREP, GP_CHAT_FIGHT, abs(delta));

		int limit = m_pForceMgr->CheckShopLimit(pCmd->force_id, pInfo ? pInfo->reputation : 0, pCmd->reputation);
		if(limit)
		{
			ACString strMsg;
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			const FORCE_CONFIG* pConfig = m_pForceMgr->GetForceData(pCmd->force_id);
			ACString strForce = pConfig ? pConfig->name : _AL("");
			pGameUI->AddChatMessage(strMsg.Format(pGameUI->GetStringFromTable(9432), strForce, limit), GP_CHAT_MISC);
		}

		CECHostPlayer::FORCE_INFO info;
		info.force_id = pCmd->force_id;
		info.contribution = pCmd->contribution;
		info.reputation = pCmd->reputation;
		SetForceInfo(pCmd->force_id, &info);

		break;
	}
	case PLAYER_FORCE_CHANGED:
	{
		cmd_player_force_changed* pCmd = (cmd_player_force_changed*)Msg.dwParam1;
		
		// show a message for join or quit
		const FORCE_CONFIG* pConfig = CECForceMgr::GetForceData(m_idForce ? m_idForce : pCmd->cur_force_id);
		if(pConfig && m_idForce != pCmd->cur_force_id)
		{
			g_pGame->GetGameRun()->AddFixedChannelMsg(m_idForce ? FIXMSG_QUITFORCE : FIXMSG_JOINFORCE, GP_CHAT_FIGHT, pConfig->name);
		}

		m_idForce = pCmd->cur_force_id;
		break;
	}
	case FORCE_GLOBAL_DATA:
	{
		cmd_force_global_data* pCmd = (cmd_force_global_data*)Msg.dwParam1;
		if(pCmd->data_ready && m_pForceMgr)
		{
			for(size_t i=0;i<pCmd->count;i++)
			{
				CECForceMgr::FORCE_GLOBAL detail;
				detail.player_count = pCmd->entry_list[i].player_count;
				detail.development = pCmd->entry_list[i].development;
				detail.construction = pCmd->entry_list[i].construction;
				detail.activity = pCmd->entry_list[i].activity;
				detail.activity_level = pCmd->entry_list[i].activity_level;
				
				m_pForceMgr->SetForceGlobal(pCmd->entry_list[i].force_id, &detail);
			}
		}
		break;
	}
	default:
		ASSERT(false);
		break;
	}

	if(pDlg && pDlg->IsShow())
	{
		pDlg->UpdateAll(GetForce());
	}
}

void CECHostPlayer::OnMsgInventoryDetail(const ECMSG &Msg)
{
	using namespace S2C;
	if(PLAYER_INVENTORY_DETAIL == Msg.dwParam2)
	{
		CECGameUIMan* pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		CDlgInventoryDetail* pDlg = 
			dynamic_cast<CDlgInventoryDetail*>(pGameUIMan->GetDialog("Win_InventoryDetail"));
		if(pDlg)
		{
			pDlg->OnInventoryDetail((cmd_player_inventory_detail*)Msg.dwParam1);
		}
	}
	else
	{
		ASSERT(false);
	}
}

void CECHostPlayer::OnMsgMultiobject(const ECMSG &Msg)
{
	using namespace S2C;
	ASSERT(Msg.dwParam2 == ADD_MULTIOBJECT_EFFECT || Msg.dwParam2 == REMOVE_MULTIOBJECT_EFFECT);
	cmd_multiobj_effect *pCmd = (cmd_multiobj_effect *)Msg.dwParam1;

	if (Msg.dwParam2 == ADD_MULTIOBJECT_EFFECT)
	{
		AddMultiObjectEffect(pCmd->target,pCmd->type);
	}
	else
	{
		RemoveMultiObjectEffect(pCmd->target,pCmd->type);
	}
}

void CECHostPlayer::OnMsgWeddingSceneInfo(const ECMSG &Msg)
{
	using namespace S2C;
	ASSERT(Msg.dwParam2 == ENTER_WEDDING_SCENE);
	enter_wedding_scene *pCmd = (enter_wedding_scene*)Msg.dwParam1;
	
	m_weddingSceneInfo.groom = pCmd->groom;
	m_weddingSceneInfo.bride = pCmd->bride; 
}

void CECHostPlayer::OnMsgOnlineAward(const ECMSG& Msg)
{
	using namespace S2C;

	if (Msg.dwParam2 == ONLINE_AWARD_DATA)
	{
		cmd_online_award_data* pCmd = (cmd_online_award_data*)Msg.dwParam1;
		if (m_pOnlineAwardCtrl)
		{
			m_pOnlineAwardCtrl->OnRecvOnlineAwardData(pCmd->total_award_time,pCmd->count,pCmd->entry_list);
		}
	}
	else if (Msg.dwParam2 == TOGGLE_ONLINE_AWARD)
	{
		cmd_toggle_online_award* pCmd = (cmd_toggle_online_award*)Msg.dwParam1;
		if (m_pOnlineAwardCtrl)
		{
			m_pOnlineAwardCtrl->OnToggleOnlineAward(pCmd->type,pCmd->activate);
		}
	}
	else
		ASSERT(FALSE);
}

void CECHostPlayer::OnMsgProfitTime(const ECMSG& Msg)
{
	using namespace S2C;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (Msg.dwParam2 == SET_PROFIT_TIME)
	{
		cmd_set_profit_time* pCmd = (cmd_set_profit_time*)Msg.dwParam1;
		if (0 == pCmd->enable)
			pGameUI->AddChatMessage(pGameUI->GetStringFromTable(9677), GP_CHAT_SYSTEM);
	}
	else if( Msg.dwParam2 == PLAYER_PROFIT_TIME )
	{
		cmd_player_profit_time* pCmd = (cmd_player_profit_time*)Msg.dwParam1;
		ACString strHint;
		ACString strColor = pCmd->profit_time == 0 ? _AL("^ff0000") : _AL("^00ff00");
		switch(pCmd->reason)
		{
		case PLAYER_QUERY:
			break;
		case PLAYER_SWITCH_SCENE:
			if(pCmd->profit_map)
			{
				if (pCmd->profit_level == PROFIT_LEVEL_NONE)
					strHint = pGameUI->GetStringFromTable(9666);
				else if (pCmd->profit_map == PROFIT_MAP_BATTLE)
					strHint = pGameUI->GetStringFromTable(9668);
				else if (pCmd->profit_map == PROFIT_MAP_EXIST)
					strHint = pGameUI->GetStringFromTable(9676);
			}
			break;
		case PLAYER_ONLINE:
			if (pCmd->profit_map == PROFIT_MAP_BATTLE)
				pGameUI->AddChatMessage(pGameUI->GetStringFromTable(9668), GP_CHAT_SYSTEM);
			else if (pCmd->profit_map == PROFIT_MAP_EXIST)
				pGameUI->AddChatMessage(pGameUI->GetStringFromTable(9676), GP_CHAT_SYSTEM);
		case PROFIT_LEVEL_CHANGE:
			if( pCmd->profit_level == PROFIT_LEVEL_NONE )
				strHint = pGameUI->GetStringFromTable(9666);
			else
			{
				strHint.Format(ACString(_AL("%s")) + pGameUI->GetStringFromTable(9665), 
					strColor,
					pGameUI->GetFormatTime(pCmd->profit_time));
			}
			break;
		}
		if (strHint.GetLength() != 0)
		{
			pGameUI->AddChatMessage(strHint, GP_CHAT_SYSTEM);
		}
		
		m_profitInfo.profit_map = pCmd->profit_map;
		m_profitInfo.profit_level = pCmd->profit_level;
		m_profitInfo.profit_time = pCmd->profit_time;
		pGameUI->UpdateProfitUI();
	}
}
void CECHostPlayer::OnMsgChangePVPNoPenalty(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_nonpenalty_pvp_state* pCmd = (cmd_nonpenalty_pvp_state*)Msg.dwParam1;

	bool bEnter = (1 == pCmd->state);
	SetPVPNoPenalty(bEnter);

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		((CDlgPKSetting*)pGameUI->GetDialog("Win_PKSetting"))->OnChangePVPNoPenalty(bEnter);
	}
}
void CECHostPlayer::OnMsgTrickBattle(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	switch(Msg.dwParam2)
	{
	case ENTER_TRICKBATTLE:
		{
			cmd_enter_trickbattle *pCmd = (cmd_enter_trickbattle *)Msg.dwParam1;
			
			if (pCmd->role_in_war == GP_BATTLE_CAMP_NONE)	//	Leave battle
			{
				m_BattleInfo.nType = BT_NONE;
				m_BattleInfo.idBattle = 0;
				m_BattleInfo.iEndTime = 0;

				if (m_pPrepSkill && m_pChariot && m_pChariot->ChariotHasSkill(m_BattleInfo.iChariot,m_pPrepSkill->GetSkillID())){
					m_pPrepSkill = NULL;
				}
				if (m_pCurSkill && m_pChariot && m_pChariot->ChariotHasSkill(m_BattleInfo.iChariot,m_pCurSkill->GetSkillID())){
					m_pCurSkill = NULL;
				}

				UpdatePositiveSkillByChariot(m_BattleInfo.iChariot,0);

				if(m_pChariot)
					m_pChariot->OnChangeUI(pGameUI,false);
			}
			else
			{
				m_BattleInfo.nType = BT_CHARIOT;
				m_BattleInfo.idBattle = pCmd->battle_id;
				m_BattleInfo.iEndTime = pCmd->end_time;

				if(m_pChariot)
					m_pChariot->OnChangeUI(pGameUI,true);
			}
			
			m_iBattleCamp = pCmd->role_in_war;
			m_BattleInfo.iResult = 0;
			m_BattleInfo.iResultCnt = 0;

			m_BattleInfo.iChariot = 0;
			m_BattleInfo.iEnergy = 0;
			m_BattleInfo.iScoreSelf = 0;
			m_BattleInfo.iMultiKill = 0;
		}
		break;
	case TRICKBATTLE_PERSONAL_SCORE:
		{
			cmd_trickbattle_personal_score *pCmd = (cmd_trickbattle_personal_score *)Msg.dwParam1;
			m_BattleInfo.iKillCount = pCmd->kill;
			m_BattleInfo.iDeathCount = pCmd->death;
			m_BattleInfo.iScoreSelf = pCmd->score;
			m_BattleInfo.iMultiKill = pCmd->multi_kill;
		}
		break;
	case TRICKBATTLE_CHARIOT_INFO:
		{
			cmd_trickbattle_chariot_info* pCmd = (cmd_trickbattle_chariot_info*)Msg.dwParam1;
			
			if(m_BattleInfo.iChariot != pCmd->chariot)
			{
				UpdatePositiveSkillByChariot(m_BattleInfo.iChariot,pCmd->chariot);
				if(m_pChariot)
					m_pChariot->UpdateState(m_BattleInfo.iChariot,pCmd->chariot);

				g_pGame->GetGameSession()->c2s_CmdGetExtProps();					
			}

			m_BattleInfo.iChariot = pCmd->chariot;
			m_BattleInfo.iEnergy = pCmd->energy;			
		}
		break;
	}
	
	pGameUI->ChariotWarAction(Msg.dwParam2, (void *)Msg.dwParam1);
}
void CECHostPlayer::OnMsgCountry(const ECMSG &Msg)
{
	using namespace S2C;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	switch (Msg.dwParam2)
	{
	case SELF_COUNTRY_NOTIFY:
		{
			cmd_self_country_notify *pCmd = (cmd_self_country_notify *)Msg.dwParam1;
			SetCountry(pCmd->country_id);
		}
		break;

	case PLAYER_COUNTRY_CHANGED:
		{
			cmd_player_country_changed *pCmd = (cmd_player_country_changed *)Msg.dwParam1;
			SetCountry(pCmd->country_id);
		}
		break;

	case ENTER_COUNTRYBATTLE:
		{
			cmd_enter_countrybattle *pCmd = (cmd_enter_countrybattle *)Msg.dwParam1;
			
			if (pCmd->role_in_war == GP_BATTLE_CAMP_NONE)	//	Leave battle
			{
				m_BattleInfo.nType = BT_NONE;
				m_BattleInfo.idBattle = 0;
				m_BattleInfo.iEndTime = 0;
				m_BattleInfo.iOffenseCountry = 0;
				m_BattleInfo.iDefenceCountry = 0;
			}
			else
			{
				m_BattleInfo.nType = BT_COUNTRY;
				m_BattleInfo.idBattle = pCmd->battle_id;
				m_BattleInfo.iEndTime = pCmd->end_time;
				m_BattleInfo.iOffenseCountry = pCmd->offense_country;
				m_BattleInfo.iDefenceCountry = pCmd->defence_country;
			}
			
			m_iBattleCamp = pCmd->role_in_war;
			m_BattleInfo.iResult = 0;
			m_BattleInfo.iResultCnt = 0;

			//	清除战场相关信息
			m_BattleInfo.bFlagCarrier = false;
			m_BattleInfo.iCarrierID = 0;
			m_BattleInfo.iReviveTimes = 1;
			m_BattleInfo.iStrongHoldCount = 0;
		}
		break;

	case COUNTRYBATTLE_RESULT:
		{
			cmd_countrybattle_result *pCmd = (cmd_countrybattle_result *)Msg.dwParam1;

			m_BattleInfo.iResult = pCmd->result;
			m_BattleInfo.iResultCnt = 5000;
			
			if (m_BattleInfo.iResult == 1)
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_COUNTRY_INVADER_WIN);
			else if (m_BattleInfo.iResult == 2 || m_BattleInfo.iResult == 3)
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_COUNTRY_DEFENDER_WIN);
		}
		break;

	case COUNTRYBATTLE_SCORE:
		{
			cmd_countrybattle_score *pCmd = (cmd_countrybattle_score *)Msg.dwParam1;
			m_BattleInfo.iMaxScore_I = pCmd->offense_goal;
			m_BattleInfo.iMaxScore_D = pCmd->defence_goal;
			m_BattleInfo.iScore_I = pCmd->offense_score;
			m_BattleInfo.iScore_D = pCmd->defence_score;
		}
		break;

	case COUNTRYBATTLE_RESURRECT_REST_TIMES:
		{
			cmd_countrybattle_resurrect_rest_times *pCmd = (cmd_countrybattle_resurrect_rest_times *)Msg.dwParam1;
			m_BattleInfo.iReviveTimes = pCmd->times;
		}
		break;

	case COUNTRYBATTLE_FLAG_CARRIER_NOTIFY:
		{
			cmd_countrybattle_flag_carrier_notify *pCmd = (cmd_countrybattle_flag_carrier_notify *)Msg.dwParam1;
			m_BattleInfo.iCarrierID = pCmd->id;
			m_BattleInfo.posCarrier = pCmd->pos;
			m_BattleInfo.bCarrierInvader = pCmd->offense > 0;
			if (m_BattleInfo.iCarrierID > 0 &&
				!g_pGame->GetGameRun()->GetPlayerName(m_BattleInfo.iCarrierID, false))
				g_pGame->GetGameSession()->CacheGetPlayerBriefInfo(1, &m_BattleInfo.iCarrierID, 2);
		}
		break;

	case COUNTRYBATTLE_BECAME_FLAG_CARRIER:
		{
			cmd_countrybattle_became_flag_carrier *pCmd = (cmd_countrybattle_became_flag_carrier *)Msg.dwParam1;
			m_BattleInfo.bFlagCarrier = pCmd->is_carrier > 0;
		}
		break;

	case COUNTRYBATTLE_PERSONAL_SCORE:
		{
			cmd_countrybattle_personal_score *pCmd = (cmd_countrybattle_personal_score *)Msg.dwParam1;
			m_BattleInfo.iCombatTime = pCmd->combat_time;
			m_BattleInfo.iAttendTime = pCmd->attend_time;
			m_BattleInfo.iKillCount = pCmd->kill_count;
			m_BattleInfo.iDeathCount = pCmd->death_count;
			m_BattleInfo.iCountryKillCount = pCmd->country_kill_count;
			m_BattleInfo.iCountryDeathCount = pCmd->country_death_count;
		}
		break;

	case COUNTRYBATTLE_INFO:
		{
			cmd_countrybattle_info *pCmd = (cmd_countrybattle_info*)Msg.dwParam1;
			m_BattleInfo.iAttackerCount = pCmd->attacker_count;
			m_BattleInfo.iDefenderCount = pCmd->defender_count;
		}
		break;

	case COUNTRYBATTLE_STRONGHOND_STATE_NOTIFY:
		{
			const cmd_stronghold_state_notify *pCmd = (cmd_stronghold_state_notify*)Msg.dwParam1;
			int count = min(pCmd->count, ARRAY_SIZE(m_BattleInfo.iStrongHoldState));

			//	检查当前状态并做出比较，弹出喊话
			if (m_BattleInfo.iStrongHoldCount > 0 && pCmd->count > 0){
				if (count != m_BattleInfo.iStrongHoldCount){
					ASSERT(false);
					a_LogOutput(1, "Invalid stronghold count(%d!=%d)", count, m_BattleInfo.iStrongHoldCount);
				}else{
					bool bAsAttacker = GetBattleCamp() == GP_BATTLE_CAMP_INVADER;
					for (int i(0); i < m_BattleInfo.iStrongHoldCount; ++ i)
					{
						int newState = pCmd->state[i];
						int oldState = m_BattleInfo.iStrongHoldState[i];
						if (newState == oldState)
							continue;
						int idString(0);
						if (newState == CECCountryConfig::SHS_ATTACKER){
							idString = 10420;	//	被攻方占领
						}else if (newState == CECCountryConfig::SHS_DEFENDER){
							idString = 10421;	//	被守方占领
						}else if (newState == CECCountryConfig::SHS_ATTACKER_HALF){
							idString = 10422;	//	攻方正在夺取
						}else if (newState == CECCountryConfig::SHS_DEFENDER_HALF){
							idString = 10423;	//	守方正在夺取
						}
						if (idString > 0){
							pGameUI->AddHeartBeatHint(pGameUI->GetStringFromTable(idString));
						}
					}
				}
			}

			//	替换当前状态
			m_BattleInfo.iStrongHoldCount = count;
			memset(m_BattleInfo.iStrongHoldState, 0, sizeof(m_BattleInfo.iStrongHoldState));
			if (pCmd->count > 0){
				for (int i(0); i < m_BattleInfo.iStrongHoldCount; ++ i)
					m_BattleInfo.iStrongHoldState[i] = pCmd->state[i];
			}
		}
		break;
	case COUNTRYBATTLE_LIVE_SHOW_RESULT:
		{
			cmd_countrybattle_live_show_result cmd;
			if(!cmd.Initialize((const void *)Msg.dwParam1,Msg.dwParam3))
				break;
	
			m_BattleInfo.SetCountryBattleLiveShowInfo(cmd);
		}
		break;
	}	
	pGameUI->CountryAction(Msg.dwParam2, (void *)Msg.dwParam1);
}

void CECHostPlayer::OnMsgDefenseRuneEnable(const ECMSG& Msg)
{	
	using namespace S2C;
	cmd_defense_rune_enable* pCmd = (cmd_defense_rune_enable*)Msg.dwParam1;
	if (pCmd->rune_type >= 0 && pCmd->rune_type < DEFENSE_RUNE_NUM)
		m_bDefenseRuneEnable[pCmd->rune_type] = (pCmd->enable != 0);
}

void CECHostPlayer::OnMsgCashMoneyRate(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_cash_money_exchg_rate* pCmd = (cmd_cash_money_exchg_rate*)Msg.dwParam1;
	m_nCashMoneyRate = pCmd->rate;
	m_bCashMoneyOpen = pCmd->open;
}

void CECHostPlayer::OnMsgMeridiansNotify(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_notify_meridian_data* pCmd = (cmd_notify_meridian_data*)Msg.dwParam1;
	
	MeridiansProp prop;
	CECMeridians::GetSingleton().GetLevelPropBonus(GetProfession(),pCmd->meridian_level,prop.hp,prop.phyDefence,prop.mgiDefence,prop.phyAttack,prop.mgiAttack);
	prop.chargableUpgradeNum	= pCmd->paid_refine_times;
	prop.continuousLoginDays	= pCmd->continu_login_days;
	prop.freeUpgradeNum			= pCmd->free_refine_times;
	prop.eightTrigramsState1	= pCmd->trigrams_map[0];
	prop.eightTrigramsState2	= pCmd->trigrams_map[1];
	prop.eightTrigramsState3	= pCmd->trigrams_map[2];
	prop.level					= pCmd->meridian_level;
	//
	SetMeridiansProp(prop);

	// 
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->MeridiansNotify();
}

void CECHostPlayer::OnMsgMeridiansResult(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_try_refine_meridian_result* pCmd = (cmd_try_refine_meridian_result*)Msg.dwParam1;

	// 通知界面启动动画
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->MeridiansImpactResult(pCmd->index, pCmd->result);
}
void CECHostPlayer::OnMsgStoneChangeEnd(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_equip_addon_update_notify* pCmd = (cmd_equip_addon_update_notify*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgStoneChange* pDlgStone = dynamic_cast<CDlgStoneChange*>(pGameUI->GetDialog("Win_StoneChange"));
	if (pDlgStone)
	{
		pDlgStone->OnStoneChangeEnd(pCmd->update_type,pCmd->equip_idx,pCmd->equip_socket_idx);
	}
}
void CECHostPlayer::OnMsgTouchPoint(const ECMSG& Msg)
{
	using namespace S2C;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgTouchShop* pDlg = dynamic_cast<CDlgTouchShop*>(pGameUI->GetDialog("Win_touchshop"));
	if (pGameUI == NULL || pDlg == NULL) return;
	
	if (Msg.dwParam2 == QUERY_TOUCH_POINT) {
		cmd_query_touch_point* pCmd = (cmd_query_touch_point*)Msg.dwParam1;
		pDlg->OnQueryTouchPointRe(pCmd->income, pCmd->remain, pCmd->retcode);
	} else if (Msg.dwParam2 == SPEND_TOUCH_POINT) {
		cmd_spend_touch_point* pCmd = (cmd_spend_touch_point*)Msg.dwParam1;
		pDlg->OnSpendTouchPointRe(pCmd->income, pCmd->remain, pCmd->cost, pCmd->index, pCmd->lots, pCmd->retcode);
	} else if (Msg.dwParam2 == TOTAL_RECHARGE) {
		cmd_total_recharge* pCmd = (cmd_total_recharge*)Msg.dwParam1;
		pDlg->OnTotalRecharge(pCmd->recharge);
	}
}

void CECHostPlayer::OnMsgTitle(const ECMSG& Msg)
{
	using namespace S2C;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (Msg.dwParam2 == QUERY_TITLE_RE) {
		cmd_query_title_re* pCmd = (cmd_query_title_re*)Msg.dwParam1;
		InitTitle(pCmd->titlescount, pCmd->titles);
		InitTitlePlus(pCmd->expirecount, (void*)((unsigned short*)(pCmd->titles) + pCmd->titlescount));
		CECUIHelper::OnQueryTitleRe();
	} else if (Msg.dwParam2 == CHANGE_CURR_TITLE_RE) {
		cmd_change_curr_title_re* pCmd = (cmd_change_curr_title_re*)Msg.dwParam1;
		CDlgTitleList* pDlg = dynamic_cast<CDlgTitleList*>(pGameUI->GetDialog("Win_TitleList"));
		SetCurrentTitle(pCmd->titleid);
		if (pDlg) pDlg->Update();
	} else if (Msg.dwParam2 == MODIFY_TITLE_NOFIFY) {
		cmd_modify_title_notify* pCmd = (cmd_modify_title_notify*)Msg.dwParam1;
		bool bAdd = (pCmd->flag != 0);
		if (bAdd) {
			CECUIHelper::AddTitle(pCmd->id, pCmd->expiretime);
			if (m_Titles.empty()) g_pGame->GetGameSession()->c2s_CmdChangeTitle(pCmd->id);
		}
		ModifyTitle(pCmd->id, pCmd->expiretime, bAdd);
	}
}

void CECHostPlayer::OnMsgSignIn(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_refresh_signin* pCmd = (cmd_refresh_signin*)Msg.dwParam1;
	m_iSignInMonthCalendar = pCmd->monthcalendar;
	m_iSignInThisYearStat = pCmd->curryearstate;
	m_iSignInLastYearStat = pCmd->lastyearstate;
	m_iSignInAwardedCountThisMonth = pCmd->awardedtimes;
	m_iSignLateCountThisMonth = pCmd->latesignintimes;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgSignIn* pDlg = dynamic_cast<CDlgSignIn*>(pGameUI->GetDialog("Win_Signin"));
	if (pDlg)pDlg->Update(pCmd->localtime);
}

void CECHostPlayer::OnMsgUseGiftCard(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_use_giftcard_result* pCmd = (cmd_use_giftcard_result*)Msg.dwParam1;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI == NULL) return;
	pGameUI->MessageBox("", pGameUI->GetStringFromTable(10821 + pCmd->retcode), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
}

void CECHostPlayer::OnMsgReincarnation(const ECMSG &Msg)
{
	using namespace S2C;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (Msg.dwParam2 == PLAYER_REINCARNATION) {
		cmd_player_reincarnation* pCmd = (cmd_player_reincarnation*) Msg.dwParam1;
		SetReincarnationCount(pCmd->reincarnation_times);
		if (pGameUI) {
			CDlgFullScreenEffectShow* pDlg = dynamic_cast<CDlgFullScreenEffectShow*>(pGameUI->GetDialog("Win_ReincarnationShow"));
			if (pDlg) {
				pDlg->SetEffectToShow(CDlgFullScreenEffectShow::REINCARNATION_EFFECT);
				pDlg->Show(true);	
			}
			pGameUI->AddChatMessage(pGameUI->GetStringFromTable(11162), GP_CHAT_SYSTEM);
		}
	} else if (Msg.dwParam2 == REINCARNATION_TOME_INFO) {
		cmd_reincarnation_tome_info* pCmd = (cmd_reincarnation_tome_info*)Msg.dwParam1;
		m_ReincarnationTome.tome_exp = pCmd->tome_exp;
		m_ReincarnationTome.tome_active = pCmd->tome_active;
		m_ReincarnationTome.reincarnations.clear();
		for (int i = 0; i < pCmd->count; i++) {
			m_ReincarnationTome.reincarnations.push_back(pCmd->records[i]);
			if (pCmd->records[i].level > m_ReincarnationTome.max_level)
				m_ReincarnationTome.max_level = pCmd->records[i].level;
		}
		SetReincarnationCount(pCmd->count);
	} else if (Msg.dwParam2 == ACTIVATE_REINCARNATION_TOME) {
		cmd_activate_reincarnation_tome* pCmd = (cmd_activate_reincarnation_tome*)Msg.dwParam1;
		m_ReincarnationTome.tome_active = pCmd->active;
	}
	if (pGameUI) {
		CDlgReincarnationBook* pDlgBook = dynamic_cast<CDlgReincarnationBook*>(pGameUI->GetDialog("Win_ReincarnationBook"));
		if (pDlgBook && pDlgBook->IsShow()) pDlgBook->Update();
		CDlgReincarnationRewrite* pDlgRewrite = dynamic_cast<CDlgReincarnationRewrite*>(pGameUI->GetDialog("Win_ReincarnationRewrite"));
		if (pDlgRewrite && pDlgRewrite->IsShow()) pDlgRewrite->Update();
	}
}
void CECHostPlayer::OnMsgRealm(const ECMSG &Msg)
{
	using namespace S2C;
	if (Msg.dwParam2 == REALM_EXP) {
		cmd_realm_exp* pCmd = (cmd_realm_exp*)Msg.dwParam1;
		SetRealmExp(pCmd->exp);
		if (pCmd->receive_exp) 
			BubbleText(BUBBLE_REALMEXP, pCmd->receive_exp);
	} else if (Msg.dwParam2 == REALM_LEVEL) {
		cmd_realm_level* pCmd = (cmd_realm_level*)Msg.dwParam1;
		SetRealmLevel(pCmd->level);
		PlayGfx(res_GFXFile(RES_GFX_REALM_LEVELUP), NULL, 1.0, PLAYERMODEL_TYPEALL);
	}
}
void CECHostPlayer::OnMsgGeneralCard(const ECMSG& Msg)
{
	using namespace S2C;
	if (Msg.dwParam2 == PLAYER_LEADERSHIP) {
		cmd_player_leadership* pCmd = (cmd_player_leadership*)Msg.dwParam1;
		if (pCmd->leadership_added) m_GeneralCard.leader_ship += pCmd->leadership_added;
		else m_GeneralCard.leader_ship = pCmd->leadership_total;
	} else if (Msg.dwParam2 == GENERALCARD_COLLECTION_DATA) {
		cmd_generalcard_collection_data* pCmd = (cmd_generalcard_collection_data*)Msg.dwParam1;
		m_GeneralCard.Init(pCmd->state, pCmd->count);
	} else if (Msg.dwParam2 == ADD_GENERALCARD_COLLECTION) {
		cmd_add_generalcard_collection* pCmd = (cmd_add_generalcard_collection*)Msg.dwParam1;
		m_GeneralCard.AddCollection(pCmd->show_order);
	}
}
void CECHostPlayer::OnMsgMonsterSpiritLevel(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_refresh_monsterspirit_level* pCmd = (cmd_refresh_monsterspirit_level*)Msg.dwParam1;
	m_CardHolder.Init(pCmd->entries, pCmd->count, pCmd->gain_times);
	//  更新卡牌界面
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgGeneralCard* pDlg = dynamic_cast<CDlgGeneralCard*>(pGameUI->GetDialog("Win_GeneralCard"));
	if (pDlg && pDlg->IsShow()) pDlg->Update();	
	CDlgMonsterSpirit* pMonster = dynamic_cast<CDlgMonsterSpirit*>(pGameUI->GetDialog("Win_MonsterSpirit"));
	if (pMonster && pMonster->IsShow()) pMonster->Update();
}

void CECHostPlayer::OnMsgRandMallShopping(const ECMSG& Msg)
{
	using namespace S2C;
	const cmd_rand_mall_shopping_res *pCmd = (const cmd_rand_mall_shopping_res *)Msg.dwParam1;	
	a_LogOutput(1, "cmd_rand_mall_shopping_res(config=%d, op=%d, result=%d, item_to_pay=%d, price=%d, firstflag=%d)",
		pCmd->config_id, pCmd->op, pCmd->result, pCmd->item_to_pay, pCmd->price, pCmd->firstflag);
	RandMallShoppingManager::Instance().Process(pCmd);
}

void CECHostPlayer::OnMsgFactionPVPMaskModify(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_faction_pvp_mask_modify *pCmd = (cmd_faction_pvp_mask_modify *)Msg.dwParam1;
	SetFactionPVPMask(pCmd->mask);
	CECFactionPVPModel::Instance().OnJoinFactionPVP(IsInFactionPVP());
}

void CECHostPlayer::OnMsgWorldContribution(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_player_world_contribution* pCmd = (cmd_player_world_contribution*)Msg.dwParam1;
	m_iWorldContribution = pCmd->contribution;
	m_iWorldContributionSpend = pCmd->total_spend;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pCmd->change > 0) {
		g_pGame->GetGameRun()->AddFixedChannelMsg(FIXMSG_GOT_WORLD_CONTRIBUTION, GP_CHAT_FIGHT, pCmd->change);
	}
}
void CECHostPlayer::OnMsgUpdateInheritableEquipAddons(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_equip_can_inherit_addons* pCmd = (cmd_equip_can_inherit_addons*)Msg.dwParam1;
	CDlgProduce* pDlg = static_cast<CDlgProduce*>(g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_Produce"));
	pDlg->OpenAdvancedEquipUp(pCmd->inv_idx, pCmd->equip_id, (int)pCmd->addon_num, pCmd->addon_id_list);
	
}
void CECHostPlayer::OnMsgClientScreenEffect(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_client_screen_effect* pCmd = (cmd_client_screen_effect*)Msg.dwParam1;
	bool bOpen = pCmd->state == 1;
	if (pCmd->type == 0) { // screen effect
		const int FadeInOutTime = 1500;
		const int LongTime = 100000000;
		CECScreenEffectMan * pEffectMan = CECUIHelper::GetGame()->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetScreenEffectMan();
		// 黑屏
		if (pCmd->eid == 1) {
			if (bOpen) {
				pEffectMan->StartEffect(CECScreenEffect::EFFECT_SCREENBLACKFADEIN, FadeInOutTime);
			} else {
				pEffectMan->StartEffect(CECScreenEffect::EFFECT_SCREENBLACKFADEOUT, FadeInOutTime);
			}
		}
		// 屏幕震动
		else if (pCmd->eid == 2) {
			if (bOpen) {
				pEffectMan->StartEffect(CECScreenEffect::EFFECT_SCREENSHAKE, LongTime);
			} else {
				pEffectMan->FinishEffect(CECScreenEffect::EFFECT_SCREENSHAKE);
			}
		}
		// 红色边框闪烁
		else if (pCmd->eid == 3) {
			if (bOpen) {
				pEffectMan->StartEffect(CECScreenEffect::EFFECT_REDSPARK, LongTime);
			} else {
				pEffectMan->FinishEffect(CECScreenEffect::EFFECT_REDSPARK);
			}
		}
		// 屏幕变灰
		else if (pCmd->eid == 4) {
			if (bOpen) {
				pEffectMan->StartEffect(CECScreenEffect::EFFECT_SCREENGRAY, LongTime);
			} else {
				pEffectMan->FinishEffect(CECScreenEffect::EFFECT_SCREENGRAY);
			}
		}
	}
	else if (pCmd->type == 1){  // gfx
	}
}
void CECHostPlayer::OnMsgComboSkillPrepare(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_combo_skill_prepare* cmd = (cmd_combo_skill_prepare*)Msg.dwParam1;
	unsigned int skillID = cmd->skill_id;

	ComboSkillState comboSkillState;
	comboSkillState.skillid = skillID;
	memcpy(comboSkillState.arg, cmd->args, sizeof(cmd->args));

	std::vector<std::pair<unsigned int, int> > comboSkillList;
	GNET::ElementSkill::GetComboSkActivated(comboSkillState, comboSkillList);

	CECComboSkillState::Instance().SetComboSkillState(comboSkillList, comboSkillState);
}

void CECHostPlayer::OnMsgPrayDistanceChange(const ECMSG& Msg){
	using namespace S2C;
	cmd_pray_distance_change* cmd = (cmd_pray_distance_change*)Msg.dwParam1;
	SetPrayDistancePlus(cmd->pray_distance_plus);
}
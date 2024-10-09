 /*
 * FILE: EC_GameUIManDlgs.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Tom Zhou, 2005/8/27
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_GameUIMan2.inl"
#include "EC_Utility.h"
#include "AStringWithWildcard.h"
#include <AUILuaDialog.h>

#define INIT_DIALOG_POINTER(variableName, className, dialogName) \
{ \
	variableName = dynamic_cast<className *>(GetDialog(dialogName)); \
	ASSERT(variableName); \
}

void CECGameUIMan::InitDialogs()
{
	INIT_DIALOG_POINTER(m_pDlgActivity, CDlgActivity, "Win_Activity");
	INIT_DIALOG_POINTER(m_pDlgArrangeTeam, CDlgArrangeTeam, "Win_ArrangeTeam");
	INIT_DIALOG_POINTER(m_pDlgAskHelpToGM, CDlgAskHelpToGM, "Win_InputString6");
	INIT_DIALOG_POINTER(m_pDlgAuctionBuyList, CDlgAuctionBuyList, "Win_AuctionBuyList");
	INIT_DIALOG_POINTER(m_pDlgAuctionMyFavor, CDlgAuctionBuyList, "Win_AuctionMyAttention");
	INIT_DIALOG_POINTER(m_pDlgAuctionList, CDlgAuctionList, "Win_AuctionList");
	INIT_DIALOG_POINTER(m_pDlgAuctionSellList, CDlgAuctionSellList, "Win_AuctionSellList");
	INIT_DIALOG_POINTER(m_pDlgAuctionSearch, CDlgAuctionSearch, "Win_AuctionSearch");
	INIT_DIALOG_POINTER(m_pDlgAutoLock, CDlgAutoLock, "Win_AutoLock");
	INIT_DIALOG_POINTER(m_pDlgAutoMove, CDlgAutoMove, "Win_AutoMove");
	INIT_DIALOG_POINTER(m_pDlgHideMove, CDlgAutoMove, "Win_HideMove");
	INIT_DIALOG_POINTER(m_pDlgBag, CDlgBag, "Win_Bag");
	INIT_DIALOG_POINTER(m_pDlgBBS, CDlgBBS, "Win_BBS");
	INIT_DIALOG_POINTER(m_pDlgBuddyState, CDlgBuddyState, "Win_BuddyState");
	INIT_DIALOG_POINTER(m_pDlgBooth1, CDlgBoothSelf, "Win_PShop1");
	INIT_DIALOG_POINTER(m_pDlgBooth2, CDlgBoothElse, "Win_PShop2");
	INIT_DIALOG_POINTER(m_pDlgCamera, CDlgCamera, "Win_Camera");
	INIT_DIALOG_POINTER(m_pDlgChat, CDlgChat, "Win_Chat");
	INIT_DIALOG_POINTER(m_pDlgChat2, CDlgChat, "Win_ChatSmall");
	INIT_DIALOG_POINTER(m_pDlgChat3, CDlgChat, "Win_ChatBig");
	INIT_DIALOG_POINTER(m_pDlgChatChoose, CDlgChat, "Win_WhisperChoose");
	INIT_DIALOG_POINTER(m_pDlgChatWhisper, CDlgChat, "Win_WhisperChat");
	INIT_DIALOG_POINTER(m_pDlgCharacter, CDlgCharacter, "Win_Character");
	INIT_DIALOG_POINTER(m_pDlgELFProp, CDlgELFProp, "Win_CharacterELF");
	INIT_DIALOG_POINTER(m_pDlgELFGenius, CDlgELFGenius, "Win_ELFGenius");
	INIT_DIALOG_POINTER(m_pDlgELFXiDian, CDlgELFXiDian, "Win_ELFXiDian");
	INIT_DIALOG_POINTER(m_pDlgELFInExp, CDlgELFInExp, "Win_ELFInExp");
	INIT_DIALOG_POINTER(m_pDlgELFRefine, CDlgELFRefine, "Win_ELFRefine");
	INIT_DIALOG_POINTER(m_pDlgELFToPill, CDlgELFToPill, "Win_ELF_ToPill");
	INIT_DIALOG_POINTER(m_pDlgELFRetrain, CDlgELFRetrain, "Win_ELFRetrain");
	INIT_DIALOG_POINTER(m_pDlgELFGeniusReset, CDlgELFGeniusReset, "Win_ELFGeniusReset");
	INIT_DIALOG_POINTER(m_pDlgELFRefineTrans, CDlgELFRefineTrans, "Win_ELFRefineTrans");
	INIT_DIALOG_POINTER(m_pDlgELFEquipRemove, CDlgELFEquipRemove, "Win_ELFEquipRemove");
	INIT_DIALOG_POINTER(m_pDlgELFLearn, CDlgELFLearn, "Win_ELFLearn");
	INIT_DIALOG_POINTER(m_pDlgELFTransaction1, CDlgELFTransaction, "Win_Transaction1");
	INIT_DIALOG_POINTER(m_pDlgELFTransaction2, CDlgELFTransaction, "Win_Transaction2");
	INIT_DIALOG_POINTER(m_pDlgChannelChat, CDlgChannelChat, "Win_ChannelChat");
	INIT_DIALOG_POINTER(m_pDlgChannelCreate, CDlgChannelCreate, "Win_ChannelCreate");
	INIT_DIALOG_POINTER(m_pDlgChannelJoin, CDlgChannelJoin, "Win_ChannelJoin");
	INIT_DIALOG_POINTER(m_pDlgChannelOption, CDlgChannelOption, "Win_ChannelOption");
	INIT_DIALOG_POINTER(m_pDlgChannelPW, CDlgChannelPW, "Win_InputString9");
	INIT_DIALOG_POINTER(m_pDlgDamageRep, CDlgDamageRep, "Win_DamageRep");
	INIT_DIALOG_POINTER(m_pDlgDragDrop, CDlgDragDrop, "DragDrop");
	INIT_DIALOG_POINTER(m_pDlgDoubleExp, CDlgDoubleExp, "Win_DoubleExp");
	INIT_DIALOG_POINTER(m_pDlgDoubleExpS, CDlgDoubleExpS, "Win_DoubleExpS");
	INIT_DIALOG_POINTER(m_pDlgEarthBagRank,	CDlgEarthBagRank,	"Win_EarthBagRank");
	INIT_DIALOG_POINTER(m_pDlgEPEquip, CDlgEPEquip, "Win_EPEquip");
	INIT_DIALOG_POINTER(m_pDlgEquipRefine, CDlgEquipRefine, "Win_EquipRefine");
	INIT_DIALOG_POINTER(m_pDlgEquipBind, CDlgEquipBind, "Win_EquipBind");
	INIT_DIALOG_POINTER(m_pDlgEquipDestroy, CDlgEquipDestroy, "Win_EquipDestroy");
	INIT_DIALOG_POINTER(m_pDlgEquipUndestroy, CDlgEquipUndestroy, "Win_EquipUndestroy");
	INIT_DIALOG_POINTER(m_pDlgEquipDye, CDlgEquipDye, "Win_EquipDye");
	INIT_DIALOG_POINTER(m_pDlgAllEquipDye, CDlgEquipDye, "Win_AllEquipDye");
	INIT_DIALOG_POINTER(m_pDlgEngrave,	CDlgEngrave,		"Win_Engrave");
	INIT_DIALOG_POINTER(m_pDlgEquipMark, CDlgEquipMark, "Win_EquipMark");
	INIT_DIALOG_POINTER(m_pDlgEquipSlot, CDlgEquipSlot, "Win_EquipSlot");
	INIT_DIALOG_POINTER(m_pDlgEquipUp, CDlgEquipUp, "Win_EquipUp");
	INIT_DIALOG_POINTER(m_pDlgEquipUpEnsure, CDlgEquipUpEnsure, "Win_EquipUpEnsure");
	INIT_DIALOG_POINTER(m_pDlgExit, CDlgExit, "Win_Message2");
	INIT_DIALOG_POINTER(m_pDlgExplorer, CDlgExplorer, "Win_Explorer");
	INIT_DIALOG_POINTER(m_pDlgAddExp, CDlgExp, "Win_AddExp");
	INIT_DIALOG_POINTER(m_pDlgSetExp, CDlgExp, "Win_SetExp");
	INIT_DIALOG_POINTER(m_pDlgFaceLift, CDlgFaceLift, "Win_Cosmetic");
	INIT_DIALOG_POINTER(m_pDlgFriendColor, CDlgFriendColor, "Win_FriendColor");
	INIT_DIALOG_POINTER(m_pDlgFriendGroup, CDlgFriendGroup, "Win_FriendGroup");
	INIT_DIALOG_POINTER(m_pDlgFriendHistory, CDlgFriendHistory, "Win_FriendHistory");
	INIT_DIALOG_POINTER(m_pDlgFriendList, CDlgFriendList, "Win_FriendList");
	INIT_DIALOG_POINTER(m_pDlgFriendOptionGroup, CDlgFriendOptionGroup, "Win_FriendOptionGroup");
	INIT_DIALOG_POINTER(m_pDlgFriendOptionName, CDlgFriendOptionName, "Win_FriendOptionName");
	INIT_DIALOG_POINTER(m_pDlgFriendOptionNormal, CDlgFriendOptionNormal, "Win_FriendOptionNormal");
	INIT_DIALOG_POINTER(m_pDlgGMConsole, CDlgGMConsole, "Win_GMConsole");
	INIT_DIALOG_POINTER(m_pDlgGmConsoleForbid,CDlgGMConsoleForbid,"Win_GMConsole2");
	INIT_DIALOG_POINTER(m_pDlgGMDelMsgReason, CDlgGMDelMsgReason, "Win_GMDelMsgReason");
	INIT_DIALOG_POINTER(m_pDlgGMFinishMsg, CDlgGMFinishMsg, "Win_GMFinishMsg");
	INIT_DIALOG_POINTER(m_pDlgGMParam, CDlgGMParam, "Win_GMParam");
	INIT_DIALOG_POINTER(m_pDlgGodEvilConvert, CDlgGodEvilConvert, "Win_SageDemonConvert");
	INIT_DIALOG_POINTER(m_pDlgGoldAccount, CDlgGoldAccount, "Win_GoldAccount");
	INIT_DIALOG_POINTER(m_pDlgGoldInquire, CDlgGoldInquire, "Win_GoldInquire");
	INIT_DIALOG_POINTER(m_pDlgGoldTrade, CDlgGoldTrade, "Win_GoldTrade");
	INIT_DIALOG_POINTER(m_pDlgGoldPwdChange, CDlgGoldPassword, "Win_GoldPwdChange");
	INIT_DIALOG_POINTER(m_pDlgGoldPwdInput, CDlgGoldPassword, "Win_GoldPwdInput");
	INIT_DIALOG_POINTER(m_pDlgGuildCreate, CDlgGuildCreate, "Win_GuildCreate");
	INIT_DIALOG_POINTER(m_pDlgGuildMan, CDlgGuildMan, "Win_GuildManage");
	INIT_DIALOG_POINTER(m_pDlgGuildMap, CDlgGuildMap, "Win_GuildMap");
	INIT_DIALOG_POINTER(m_pDlgGuildDiplomacyApply, CDlgGuildDiplomacyApply, "Win_GuildDiplomacyApply");
	INIT_DIALOG_POINTER(m_pDlgGuildDiplomacyMan, CDlgGuildDiplomacyMan, "Win_GuildDiplomacyMan");
	INIT_DIALOG_POINTER(m_pDlgGMapStatus1, CDlgGuildMapStatus, "Win_GMapStatus1");
	INIT_DIALOG_POINTER(m_pDlgGMapStatus2, CDlgGuildMapStatus, "Win_GMapStatus2");
	INIT_DIALOG_POINTER(m_pDlgGMapChallenge, CDlgGuildMapStatus, "Win_GMapChallenge");
	INIT_DIALOG_POINTER(m_pDlgGMapTravel, CDlgGuildMapTravel, "Win_GMapTravel");
	INIT_DIALOG_POINTER(m_pDlgHelp, CDlgHelp, "Win_Help");
	INIT_DIALOG_POINTER(m_pDlgHost, CDlgHost, "Win_Hpmpxp");
	INIT_DIALOG_POINTER(m_pDlgIdentify, CDlgIdentify, "Win_Identify");
	INIT_DIALOG_POINTER(m_pDlgInfo, CDlgInfo, "Win_Poplist");
	INIT_DIALOG_POINTER(m_pDlgInfoIcon, CDlgInfo, "Win_Pop");
	INIT_DIALOG_POINTER(m_pDlgInputName, CDlgInputName, "Win_InputString5");
	INIT_DIALOG_POINTER(m_pDlgInputNO, CDlgInputNO, "Win_InputNO");
	INIT_DIALOG_POINTER(m_pDlgInstall, CDlgInstall, "Win_Enchase");
	INIT_DIALOG_POINTER(m_pDlgUninstall, CDlgInstall, "Win_Disenchase");
	INIT_DIALOG_POINTER(m_pDlgMailList, CDlgMailList, "Win_MailList");
	INIT_DIALOG_POINTER(m_pDlgMailOption, CDlgMailOption, "Win_MailOption");
	INIT_DIALOG_POINTER(m_pDlgMailRead, CDlgMailRead, "Win_MailRead");
	INIT_DIALOG_POINTER(m_pDlgMailWrite, CDlgMailWrite, "Win_MailWrite");
	INIT_DIALOG_POINTER(m_pDlgMiniMap, CDlgMiniMap, "Win_Map");
	INIT_DIALOG_POINTER(m_pDlgMiniMapMark, CDlgMiniMap, "Win_Mark");
	INIT_DIALOG_POINTER(m_pDlgNPC, CDlgNPC, "Win_NPC");
	INIT_DIALOG_POINTER(m_pDlgPalette, CDlgPalette, "Win_Palette");
	INIT_DIALOG_POINTER(m_pDlgPalette2, CDlgPalette2, "Win_Palette2");
	INIT_DIALOG_POINTER(m_pDlgPetDye, CDlgPetDye, "Win_PetDye");
	INIT_DIALOG_POINTER(m_pDlgPetFittingRoom, CDlgPetFittingRoom, "Win_FittingRoomPet");
	INIT_DIALOG_POINTER(m_pDlgPetList, CDlgPetList, "Win_PetList");
	INIT_DIALOG_POINTER(m_pDlgPetHatch, CDlgPetHatch, "Win_PetHatch");
	INIT_DIALOG_POINTER(m_pDlgPetRec, CDlgPetRec, "Win_PetRec");
	INIT_DIALOG_POINTER(m_pDlgPetRename, CDlgPetRename, "Win_PetRename");
	INIT_DIALOG_POINTER(m_pDlgPetRetrain, CDlgPetRetrain, "Win_PetRetrain");
	INIT_DIALOG_POINTER(m_pDlgPKSetting, CDlgPKSetting, "Win_PKSetting");
	INIT_DIALOG_POINTER(m_pDlgPopMsg, CDlgPopMsg, "Win_Popmsg2");
	INIT_DIALOG_POINTER(m_pDlgPQTitle, CDlgPQ, "Win_PQtitle");
	INIT_DIALOG_POINTER(m_pDlgPQRank, CDlgPQ, "Win_PQrank");
	INIT_DIALOG_POINTER(m_pDlgProduce, CDlgProduce, "Win_Produce");
	INIT_DIALOG_POINTER(m_pDlgPreviewProduce, CDlgPreviewProduce, "Win_PreviewProduce");
	INIT_DIALOG_POINTER(m_pDlgProclaim, CDlgProclaim, "Win_InputString8");
	INIT_DIALOG_POINTER(m_pDlgQuestion, CDlgQuestion, "Win_Question");
	INIT_DIALOG_POINTER(m_pDlgQuickAction, CDlgQuickAction, "Win_QuickAction");
	INIT_DIALOG_POINTER(m_pDlgQuickBar1, CDlgQuickBar, "Win_Quickbar9Va");
	INIT_DIALOG_POINTER(m_pDlgQuickBar2, CDlgQuickBar, "Win_Quickbar8Ha");
	INIT_DIALOG_POINTER(m_pDlgQuickBarPet, CDlgQuickBarPet, "Win_QuickbarPetV");
	INIT_DIALOG_POINTER(m_pDlgQShop, CDlgQShop, "Win_QShop");
	INIT_DIALOG_POINTER(m_pDlgBackShop, CDlgQShop, "Win_BackShop");
	INIT_DIALOG_POINTER(m_pDlgRefineTrans, CDlgRefineTrans, "Win_RefineTrans");
	INIT_DIALOG_POINTER(m_pDlgReportToGM, CDlgReportToGM, "Win_InputString7");
	INIT_DIALOG_POINTER(m_pDlgResetProp, CDlgResetProp, "Win_ResetProp");
	INIT_DIALOG_POINTER(m_pDlgRevive, CDlgRevive, "Win_Message4");
	INIT_DIALOG_POINTER(m_pDlgSettingSystem, CDlgSettingSystem, "Win_SettingSystem");
	INIT_DIALOG_POINTER(m_pDlgSettingGame, CDlgSettingGame, "Win_SettingGame");
	INIT_DIALOG_POINTER(m_pDlgSettingVideo, CDlgSettingVideo, "Win_SettingVideo");
	INIT_DIALOG_POINTER(m_pDlgSettingQuickKey, CDlgSettingQuickKey, "Win_SettingQuickKey");
	INIT_DIALOG_POINTER(m_pDlgBlackList, CDlgBlackList, "Win_Blacklist");
	INIT_DIALOG_POINTER(m_pDlgShop, CDlgShop, "Win_Shop");
	INIT_DIALOG_POINTER(m_pDlgSkillEdit, CDlgSkillEdit, "Win_SkillEdit");
	INIT_DIALOG_POINTER(m_pDlgSplit, CDlgSplit, "Win_Split");
	INIT_DIALOG_POINTER(m_pDlgStorage, CDlgStorage, "Win_Storage");
	INIT_DIALOG_POINTER(m_pDlgStoragePW, CDlgStoragePW, "Win_InputString");
	INIT_DIALOG_POINTER(m_pDlgStorageChangePW, CDlgStorageChangePW, "Win_InputString3");
	INIT_DIALOG_POINTER(m_pDlgAccountBox, CDlgStorage, "Win_Storage3");
	INIT_DIALOG_POINTER(m_pDlgSystem, CDlgSystem, "Win_Main");
	INIT_DIALOG_POINTER(m_pDlgSystem2, CDlgSystem2, "Win_Main2");
	INIT_DIALOG_POINTER(m_pDlgSystem3, CDlgSystem3, "Win_Main3");
	INIT_DIALOG_POINTER(m_pDlgSystem4, CDlgSystem4, "Win_Main4");
	INIT_DIALOG_POINTER(m_pDlgSystem5, CDlgSystem5, "Win_Main5");
	INIT_DIALOG_POINTER(m_pDlgSystemb, CDlgSystem, "Win_Mainb");
	INIT_DIALOG_POINTER(m_pDlgSystem2b, CDlgSystem2, "Win_Main2b");
	INIT_DIALOG_POINTER(m_pDlgSystem3b, CDlgSystem3, "Win_Main3b");
	INIT_DIALOG_POINTER(m_pDlgSystem4b, CDlgSystem4, "Win_Main4b");
	INIT_DIALOG_POINTER(m_pDlgSystem5b, CDlgSystem5, "Win_Main5b");
	INIT_DIALOG_POINTER(m_pDlgTarget, CDlgTarget, "Win_HpOther");
	INIT_DIALOG_POINTER(m_pDlgTask, CDlgTask, "Win_Quest");
	INIT_DIALOG_POINTER(m_pDlgTeach, CDlgTeach, "Win_Teach");
	INIT_DIALOG_POINTER(m_pDlgTeamMe1, CDlgTeamContext, "Win_GroupMe1");
	INIT_DIALOG_POINTER(m_pDlgTeamMe2, CDlgTeamContext, "Win_GroupMe2");
	INIT_DIALOG_POINTER(m_pDlgTeamOther1, CDlgTeamContext, "Win_GroupOther1");
	INIT_DIALOG_POINTER(m_pDlgTeamOther2, CDlgTeamContext, "Win_GroupOther2");
	INIT_DIALOG_POINTER(m_pDlgTeamMain, CDlgTeamMain, "Win_TeamMain");
	INIT_DIALOG_POINTER(m_pDlgTrade, CDlgTrade, "Win_Trade");
	INIT_DIALOG_POINTER(m_pDlgWiki, CDlgWiki, "Win_Wiki");
	INIT_DIALOG_POINTER(m_pDlgWikiMonster, CDlgWikiMonster, "Win_WikiMonsterSearch");
	INIT_DIALOG_POINTER(m_pDlgWikiMonsterDrop, CDlgWikiMonsterDrop, "Win_WikiMonsterDrop");
	INIT_DIALOG_POINTER(m_pDlgWikiMonsterTask, CDlgWikiMonsterTask, "Win_WikiMonsterTask");
	INIT_DIALOG_POINTER(m_pDlgWikiEquipment, CDlgWikiEquipment, "Win_WikiEquipmentSearch");
	INIT_DIALOG_POINTER(m_pDlgWikiRandomProperty, CDlgWikiRandomProperty, "Win_WikiRandomProperty");
	INIT_DIALOG_POINTER(m_pDlgWikiEquipmentDrop, CDlgWikiEquipmentDrop, "Win_WikiEquipmentDrop");
	INIT_DIALOG_POINTER(m_pDlgWikiTask, CDlgWikiTask, "Win_WikiTaskSearch");
	INIT_DIALOG_POINTER(m_pDlgWikiTaskDetail, CDlgWikiTaskDetail, "Win_WikiTaskDetail");
	INIT_DIALOG_POINTER(m_pDlgWorldMap, CDlgWorldMap, "Win_WorldMap");
	INIT_DIALOG_POINTER(m_pDlgWorldMapDetail, CDlgWorldMap, "Win_WorldMapDetail");
	INIT_DIALOG_POINTER(m_pDlgWorldMapTravel, CDlgWorldMap, "Win_WorldMapTravel");
	INIT_DIALOG_POINTER(m_pDlgScriptHelp, CDlgScriptHelp, "Win_ScriptHelp");
	INIT_DIALOG_POINTER(m_pDlgSettingScriptHelp, CDlgSettingScriptHelp, "Win_ScriptHelpSetting");
	INIT_DIALOG_POINTER(m_pDlgScriptTip, CDlgScriptTip, "Win_ScriptTip");
	INIT_DIALOG_POINTER(m_pDlgScriptHelpHistory, CDlgScriptHelpHistory, "Win_HelpHistory");
	INIT_DIALOG_POINTER(m_pDlgBroadCast, CDlgBroadCast, "Win_Broadcast");
	INIT_DIALOG_POINTER(m_pDlgProfView, CDlgProfView, "Win_ProfView");
	INIT_DIALOG_POINTER(m_pDlgPlantPet, CDlgPlantPet,	"Win_PlantPet");

	INIT_DIALOG_POINTER(m_pDlgWebMyShop, CDlgWebMyShop, "Win_Webmyshop");
	INIT_DIALOG_POINTER(m_pDlgWebViewProduct, CDlgWebViewProduct, "Win_Webviewproduct");
	INIT_DIALOG_POINTER(m_pDlgWebTradeOption, CDlgWebTradeOption, "Win_WebTradeOption");
	INIT_DIALOG_POINTER(m_pDlgWedding,	CDlgWedding,	"Win_Wedding");
	INIT_DIALOG_POINTER(m_pDlgWeddingInvite,	CDlgWeddingInvite,	"Win_WeddingInvitation");
	INIT_DIALOG_POINTER(m_pDlgBShop, CDlgBShop, "Win_BShop");
	INIT_DIALOG_POINTER(m_pDlgMailToFriends, CDlgMailToFriends, "Win_MailtoFriend");
	INIT_DIALOG_POINTER(m_pDlgOnlineRemind, CDlgOnlineRemind, "Win_mailtips");

	INIT_DIALOG_POINTER(m_pDlgOnlineAward, CDlgOnlineAward, "Win_AddExp2");
	INIT_DIALOG_POINTER(m_pDlgAddExp2Q, CDlgOnlineAward,"Win_Addexp2Pop");
	INIT_DIALOG_POINTER(m_pDlgCountryMap, CDlgCountryMap, "Win_CountryMap");
	INIT_DIALOG_POINTER(m_pDlgCountryWarMap, CDlgCountryWarMap, "Win_CountryWarMap");
	INIT_DIALOG_POINTER(m_pDlgCountryScore, CDlgCountryScore, "Win_CountryPoint");
	INIT_DIALOG_POINTER(m_pDlgMessageBox, CDlgMessageBox, "Win_MessageBox");
	INIT_DIALOG_POINTER(m_pDlgAutoHPMP, CDlgAutoHPMP, "Win_AutoHPMP");
	INIT_DIALOG_POINTER(m_pDlgCountryReward, CDlgCountryReward, "Win_CountryReward");
	INIT_DIALOG_POINTER(m_pDlgBuyConfirm, CDlgBuyConfirm, "Win_Message6");
	INIT_DIALOG_POINTER(m_pDlgQuickPay, CDlgQuickPay, "Win_QuickPay");

	INIT_DIALOG_POINTER(m_pDlgAskHelpToGM2, CDlgAskHelpToGM2, "Win_InputString11");
	INIT_DIALOG_POINTER(m_pDlgBoothSet, CDlgBoothSet, "Win_PshopSet");
	INIT_DIALOG_POINTER(m_pDlgPetEvolution, CDlgPetEvolution, "Win_PetTrain");
	INIT_DIALOG_POINTER(m_pDlgPetNature, CDlgPetNature, "Win_PetSkillsTree");


	INIT_DIALOG_POINTER(m_pDlgPetPreviewRebuild, CDlgPreviewPetRebuild, "Win_PreviewPet");
	INIT_DIALOG_POINTER(m_pDlgPetEvoAnim, CDlgPetEvoAnim, "Win_PetShow");
	INIT_DIALOG_POINTER(m_pDlgTaskConfirm, CDlgTaskConfirm, "Win_QuestDone");
	INIT_DIALOG_POINTER(m_pDlgTaskHintPop, CDlgTaskHintPop, "Win_Questq");
	
	INIT_DIALOG_POINTER(m_pDlgModifyName, CDlgModifyName, "Win_ModifyName");

	INIT_DIALOG_POINTER(m_pDlgMeridians, CDlgMeridians, "Win_Meridians");
	INIT_DIALOG_POINTER(m_pDlgMeridiansImpact, CDlgMeridiansImpact, "Win_MeridiansImpact");

	INIT_DIALOG_POINTER(m_pDlgStoneChange, CDlgStoneChange, "Win_StoneChange");
	INIT_DIALOG_POINTER(m_pDlgStoneTransfer, CDlgStoneTransfer, "Win_StoneTransfer");
	INIT_DIALOG_POINTER(m_pDlgStoneReplace, CDlgStoneReplace, "Win_StoneReplace");

	INIT_DIALOG_POINTER(m_pDlgKingAuction, CDlgKingAuction, "Win_KingAuctions");
	INIT_DIALOG_POINTER(m_pDlgKingElection, CDlgKingElection, "Win_KingVote");
	INIT_DIALOG_POINTER(m_pDlgKingCommand, CDlgKingCommand, "Win_KingOrder");
	INIT_DIALOG_POINTER(m_pDlgDomainCondition, CDlgDomainCondition, "Win_KingSet");
	INIT_DIALOG_POINTER(m_pDlgKingResult, CDlgKingResult, "Win_KingShow");

	INIT_DIALOG_POINTER(m_pDlgGivingFor, CDlgGivingFor, "Win_GivingFor");
	INIT_DIALOG_POINTER(m_pDlgFashionSplit, CDlgFashionSplit, "Win_FashionSplit");	
	INIT_DIALOG_POINTER(m_pDlgTouchShop, CDlgTouchShop, "Win_touchshop");

	INIT_DIALOG_POINTER(m_pDlgOffShopCreate, CDlgOfflineShopCreate, "Win_ShopCreate");
	INIT_DIALOG_POINTER(m_pDlgOffShopItemList, CDlgOfflineShopItemsList, "Win_ShopItemList");
	INIT_DIALOG_POINTER(m_pDlgOffShopList, CDlgOfflineShopList, "Win_ShopList");
	INIT_DIALOG_POINTER(m_pDlgOffShopSelfSetting, CDlgOfflineShopSelfSetting, "Win_ShopSet");
	INIT_DIALOG_POINTER(m_pDlgOffShopStore, CDlgOfflineShopStore, "Win_ShopBag");
	INIT_DIALOG_POINTER(m_pDlgComplexTitle, CDlgComplexTitle, "Win_ComplexTitle");
	INIT_DIALOG_POINTER(m_pDlgTitleBubble, CDlgTitleBubble, "Win_TitleBubble");
	INIT_DIALOG_POINTER(m_pDlgTitleChallenge, CDlgTitleChallenge, "Win_Event");
	INIT_DIALOG_POINTER(m_pDlgTitleList, CDlgTitleList, "Win_TitleList");

	INIT_DIALOG_POINTER(m_pDlgMatchHabit, CDlgMatchProfileHabit, "Win_Infohabit");
	INIT_DIALOG_POINTER(m_pDlgMatchHobby, CDlgMatchProfileHobby, "Win_Infohobby");
	INIT_DIALOG_POINTER(m_pDlgMatchAge, CDlgMatchProfileAge, "Win_InfoConstellationAge");
	INIT_DIALOG_POINTER(m_pDlgMatchSetting, CDlgMatchProfileSetting, "Win_InfoMatch");
	INIT_DIALOG_POINTER(m_pDlgAutoHelp, CDlgAutoHelp, "Win_WikiPop");
	INIT_DIALOG_POINTER(m_pDlgGiftCard, CDlgGiftCard, "Win_giftcard");
	INIT_DIALOG_POINTER(m_pDlgReincarnation, CDlgReincarnation, "Win_Reincarnation");
	INIT_DIALOG_POINTER(m_pDlgReincarnationBook, CDlgReincarnationBook, "Win_ReincarnationBook");
	INIT_DIALOG_POINTER(m_pDlgReincarnationRewrite, CDlgReincarnationRewrite, "Win_ReincarnationRewrite");
	INIT_DIALOG_POINTER(m_pDlgDownloadSpeed, CDlgDownloadSpeed, "Win_DownloadSpeed");
	INIT_DIALOG_POINTER(m_pDlgExclusiveAward, CDlgExclusiveAward, "Win_ExclusiveAward");
	INIT_DIALOG_POINTER(m_pDlgAutoPolicy, CDlgAutoPolicy, "Win_AutoPolicy");
	
	INIT_DIALOG_POINTER(m_pDlgChariotWarApply, CDlgChariotEnter, "Win_ChariotApply");
	INIT_DIALOG_POINTER(m_pDlgChariotUI, CDlgChariotOpt, "Win_ChariotUI");
	INIT_DIALOG_POINTER(m_pDlgChariotMsgScore, CDlgChariotScore, "Win_BattleMsg01");
	INIT_DIALOG_POINTER(m_pDlgChariotMsgMulti, CDlgChariotMultiKill, "Win_BattleMsg02");
	INIT_DIALOG_POINTER(m_pDlgChariotRankList, CDlgChariotRankList, "Win_battlerank");
	INIT_DIALOG_POINTER(m_pDlgChariotRevive, CDlgChariotRevive, "Win_ChariotRevive");
	INIT_DIALOG_POINTER(m_pDlgChariotAmount, CDlgChariotAmount, "Win_ChariotAmount");
	
	INIT_DIALOG_POINTER(m_pDlgSysModuleQuickBar, CDlgSysModQuickBar, "Win_SysModeQuickBar");
	INIT_DIALOG_POINTER(m_pDlgGeneralCard, CDlgGeneralCard, "Win_GeneralCard");
	INIT_DIALOG_POINTER(m_pDlgGeneralCardBuySetting, CDlgGeneralCardBuySetting, "Win_GeneralCardBuySetting");
	INIT_DIALOG_POINTER(m_pDlgAutoTask, CDlgAutoTask, "Win_AutoTask");

	INIT_DIALOG_POINTER(m_pDlgGenCardRespawn, CDlgGenCardRespawn, "Win_GeneralCardRe");
	INIT_DIALOG_POINTER(m_pDlgFlySwordImprove,CDlgFlySwordImprove,"Win_Aircraft");
	INIT_DIALOG_POINTER(m_pDlgTargetItem, CDlgTargetItem, "Win_ItemPop");
	INIT_DIALOG_POINTER(m_pDlgFactionPVPStatus, CDlgFactionPVPStatus, "Win_GMapStatus3");
	INIT_DIALOG_POINTER(m_pDlgFactionPVPRank, CDlgFactionPVPRank, "Win_PvpReward");
	INIT_DIALOG_POINTER(m_pDlgFashionShop, CDlgFashionShop, "Win_FashionShop");
	
	INIT_DIALOG_POINTER(m_pDlgSkillAction, CDlgSkillAction, "Win_SkillAction");
	INIT_DIALOG_POINTER(m_pDlgSkillSubList, CDlgSkillSubList, "Win_SkillSubList");
	INIT_DIALOG_POINTER(m_pDlgSkillSubTreeEvil, CDlgSkillSubTree, "Win_SkillTreeEvil");
	INIT_DIALOG_POINTER(m_pDlgSkillSubTreeGod, CDlgSkillSubTree, "Win_SkillTreeGod");
	INIT_DIALOG_POINTER(m_pDlgSkillSubTreeBase, CDlgSkillSubTree, "Win_SkillTreeBase");
	INIT_DIALOG_POINTER(m_pDlgSkillSubAction, CDlgSkillSubAction, "Win_SkillSubAction");
	INIT_DIALOG_POINTER(m_pDlgSkillSubOther, CDlgSkillSubOther, "Win_SkillSubOther");
	INIT_DIALOG_POINTER(m_pDlgSkillSubPool, CDlgSkillSubPool, "Win_SkillSubPool");

	INIT_DIALOG_POINTER(m_pDlgRedSpark, CDlgRedSpark, "Win_RedSpark");
	INIT_DIALOG_POINTER(m_pDlgBlackScreen, CDlgBlackScreen, "Win_BlackScreen");
	INIT_DIALOG_POINTER(m_pDlgComboShortCutBar, CDlgComboShortCutBar, "Win_ComboSkill")
	INIT_DIALOG_POINTER(m_pDlgPureColorFashionTip, CDlgPureColorFashionTip, "Win_PureColor")
	INIT_DIALOG_POINTER(m_pDlgIceThunderBall, CDlgIceThunderBall, "Win_IceThunderBall")
	INIT_DIALOG_POINTER(m_pDlgQuestionTask, CDlgQuestionTask, "Win_QuestionTask")
	INIT_DIALOG_POINTER(m_pDlgSimpleCostItemService, CDlgSimpleCostItemService, "Win_SimpleCostItemService")
}

namespace
{	
#define DEFINE_DIALOG_CREATOR(dialogclass) static PAUIDIALOG wmgj_new_##dialogclass(void){ return new dialogclass; }
#define MAP_FILE_TO_CREATOR(file, dialogclass) s_mDialogCreator[file] = &wmgj_new_##dialogclass;  s_mDialogCreator["version01\\"##file] = &wmgj_new_##dialogclass;

	//	定义各对话框创建函数
	DEFINE_DIALOG_CREATOR(CDlgActivity)
	DEFINE_DIALOG_CREATOR(CDlgArrangeTeam)
	DEFINE_DIALOG_CREATOR(CDlgAskHelpToGM)
	DEFINE_DIALOG_CREATOR(CDlgAuctionBuyList)
	DEFINE_DIALOG_CREATOR(CDlgAuctionList)
	DEFINE_DIALOG_CREATOR(CDlgAuctionSearch)
	DEFINE_DIALOG_CREATOR(CDlgAuctionSellList)
	DEFINE_DIALOG_CREATOR(CDlgAutoLock)
	DEFINE_DIALOG_CREATOR(CDlgAutoMove)
	DEFINE_DIALOG_CREATOR(CDlgAutoTask)
	DEFINE_DIALOG_CREATOR(CDlgAward)
	DEFINE_DIALOG_CREATOR(CDlgBBS)
	DEFINE_DIALOG_CREATOR(CDlgBShop)
	DEFINE_DIALOG_CREATOR(CDlgBackHelp)
	DEFINE_DIALOG_CREATOR(CDlgBackOthers)
	DEFINE_DIALOG_CREATOR(CDlgBackSelf)
	DEFINE_DIALOG_CREATOR(CDlgBag)
	DEFINE_DIALOG_CREATOR(CDlgBoothElse)
	DEFINE_DIALOG_CREATOR(CDlgBoothSelf)
	DEFINE_DIALOG_CREATOR(CDlgBroadCast)
	DEFINE_DIALOG_CREATOR(CDlgBuddyState)
	DEFINE_DIALOG_CREATOR(CDlgCWindow)
	DEFINE_DIALOG_CREATOR(CDlgCamera)
	DEFINE_DIALOG_CREATOR(CDlgChangeSkirt)
	DEFINE_DIALOG_CREATOR(CDlgChannelChat)
	DEFINE_DIALOG_CREATOR(CDlgChannelCreate)
	DEFINE_DIALOG_CREATOR(CDlgChannelJoin)
	DEFINE_DIALOG_CREATOR(CDlgChannelOption)
	DEFINE_DIALOG_CREATOR(CDlgChannelPW)
	DEFINE_DIALOG_CREATOR(CDlgCharacter)
	DEFINE_DIALOG_CREATOR(CDlgChat)
	DEFINE_DIALOG_CREATOR(CDlgCongregate)
	DEFINE_DIALOG_CREATOR(CDlgConsole)
	DEFINE_DIALOG_CREATOR(CDlgCoupon)
	DEFINE_DIALOG_CREATOR(CDlgCustomize)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeEye)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeEye2)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeFace)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeFace2)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeHair)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeNoseMouth)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeNoseMouth2)
	DEFINE_DIALOG_CREATOR(CDlgCustomizePaint)
	DEFINE_DIALOG_CREATOR(CDlgCustomizePre)
	DEFINE_DIALOG_CREATOR(CDlgCustomizeStature)
	DEFINE_DIALOG_CREATOR(CDlgDamageRep)
	DEFINE_DIALOG_CREATOR(CDlgDoubleExp)
	DEFINE_DIALOG_CREATOR(CDlgDoubleExpS)
	DEFINE_DIALOG_CREATOR(CDlgDragDrop)
	DEFINE_DIALOG_CREATOR(CDlgELFEquipRemove)
	DEFINE_DIALOG_CREATOR(CDlgELFGenius)
	DEFINE_DIALOG_CREATOR(CDlgELFGeniusReset)
	DEFINE_DIALOG_CREATOR(CDlgELFInExp)
	DEFINE_DIALOG_CREATOR(CDlgELFLearn)
	DEFINE_DIALOG_CREATOR(CDlgELFProp)
	DEFINE_DIALOG_CREATOR(CDlgELFRefine)
	DEFINE_DIALOG_CREATOR(CDlgELFRefineTrans)
	DEFINE_DIALOG_CREATOR(CDlgELFRetrain)
	DEFINE_DIALOG_CREATOR(CDlgELFSkillTree)
	DEFINE_DIALOG_CREATOR(CDlgELFToPill)
	DEFINE_DIALOG_CREATOR(CDlgELFTransaction)
	DEFINE_DIALOG_CREATOR(CDlgELFXiDian)
	DEFINE_DIALOG_CREATOR(CDlgEPEquip)
	DEFINE_DIALOG_CREATOR(CDlgEarthBagRank)
	DEFINE_DIALOG_CREATOR(CDlgEngrave)
	DEFINE_DIALOG_CREATOR(CDlgEquipBind)
	DEFINE_DIALOG_CREATOR(CDlgEquipDestroy)
	DEFINE_DIALOG_CREATOR(CDlgEquipDye)
	DEFINE_DIALOG_CREATOR(CDlgEquipMark)
	DEFINE_DIALOG_CREATOR(CDlgEquipRefine)
	DEFINE_DIALOG_CREATOR(CDlgEquipSlot)
	DEFINE_DIALOG_CREATOR(CDlgEquipUndestroy)
	DEFINE_DIALOG_CREATOR(CDlgEquipUp)
	DEFINE_DIALOG_CREATOR(CDlgEquipUpEnsure)
	DEFINE_DIALOG_CREATOR(CDlgExit)
	DEFINE_DIALOG_CREATOR(CDlgExp)
	DEFINE_DIALOG_CREATOR(CDlgExplorer)
	DEFINE_DIALOG_CREATOR(CDlgFaceLift)
	DEFINE_DIALOG_CREATOR(CDlgFaceName)
	DEFINE_DIALOG_CREATOR(CDlgFindPlayer)
	DEFINE_DIALOG_CREATOR(CDlgFittingRoom)
	DEFINE_DIALOG_CREATOR(CDlgForce)
	DEFINE_DIALOG_CREATOR(CDlgForceActivity)
	DEFINE_DIALOG_CREATOR(CDlgForceActivityIcon)
	DEFINE_DIALOG_CREATOR(CDlgForceNPC)
	DEFINE_DIALOG_CREATOR(CDlgFortressBuild)
	DEFINE_DIALOG_CREATOR(CDlgFortressBuildHelp)
	DEFINE_DIALOG_CREATOR(CDlgFortressBuildSub)
	DEFINE_DIALOG_CREATOR(CDlgFortressBuildSubList)
	DEFINE_DIALOG_CREATOR(CDlgFortressContrib)
	DEFINE_DIALOG_CREATOR(CDlgFortressExchange)
	DEFINE_DIALOG_CREATOR(CDlgFortressInfo)
	DEFINE_DIALOG_CREATOR(CDlgFortressMaterial)
	DEFINE_DIALOG_CREATOR(CDlgFortressMini)
	DEFINE_DIALOG_CREATOR(CDlgFortressStatus)
	DEFINE_DIALOG_CREATOR(CDlgFortressWar)
	DEFINE_DIALOG_CREATOR(CDlgFortressWarList)
	DEFINE_DIALOG_CREATOR(CDlgFriendChat)
	DEFINE_DIALOG_CREATOR(CDlgFriendColor)
	DEFINE_DIALOG_CREATOR(CDlgFriendGroup)
	DEFINE_DIALOG_CREATOR(CDlgFriendHistory)
	DEFINE_DIALOG_CREATOR(CDlgFriendList)
	DEFINE_DIALOG_CREATOR(CDlgFriendOptionGroup)
	DEFINE_DIALOG_CREATOR(CDlgFriendOptionName)
	DEFINE_DIALOG_CREATOR(CDlgFriendOptionNormal)
	DEFINE_DIALOG_CREATOR(CDlgFriendRequest)
	DEFINE_DIALOG_CREATOR(CDlgGMConsole)
	DEFINE_DIALOG_CREATOR(CDlgGMConsoleForbid)
	DEFINE_DIALOG_CREATOR(CDlgGMDelMsgReason)
	DEFINE_DIALOG_CREATOR(CDlgGMFinishMsg)
	DEFINE_DIALOG_CREATOR(CDlgGMParam)
	DEFINE_DIALOG_CREATOR(CDlgGMQueryItem)
	DEFINE_DIALOG_CREATOR(CDlgGMQueryItemSub)
	DEFINE_DIALOG_CREATOR(CDlgGodEvilConvert)
	DEFINE_DIALOG_CREATOR(CDlgGoldAccount)
	DEFINE_DIALOG_CREATOR(CDlgGoldInquire)
	DEFINE_DIALOG_CREATOR(CDlgGoldPassword)
	DEFINE_DIALOG_CREATOR(CDlgGoldTrade)
	DEFINE_DIALOG_CREATOR(CDlgGuildCreate)
	DEFINE_DIALOG_CREATOR(CDlgGuildDiplomacyApply)
	DEFINE_DIALOG_CREATOR(CDlgGuildDiplomacyMan)
	DEFINE_DIALOG_CREATOR(CDlgGuildMan)
	DEFINE_DIALOG_CREATOR(CDlgGuildMap)
	DEFINE_DIALOG_CREATOR(CDlgGuildMapStatus)
	DEFINE_DIALOG_CREATOR(CDlgFactionPVPStatus)
	DEFINE_DIALOG_CREATOR(CDlgFactionPVPRank)
	DEFINE_DIALOG_CREATOR(CDlgGuildMapTravel)
	DEFINE_DIALOG_CREATOR(CDlgHelp)
	DEFINE_DIALOG_CREATOR(CDlgHost)
	DEFINE_DIALOG_CREATOR(CDlgHostELF)
	DEFINE_DIALOG_CREATOR(CDlgHostPet)
	DEFINE_DIALOG_CREATOR(CDlgIdentify)
	DEFINE_DIALOG_CREATOR(CDlgInfo)
	DEFINE_DIALOG_CREATOR(CDlgInputNO)
	DEFINE_DIALOG_CREATOR(CDlgInputNO2)
	DEFINE_DIALOG_CREATOR(CDlgInputName)
	DEFINE_DIALOG_CREATOR(CDlgInstall)
	DEFINE_DIALOG_CREATOR(CDlgInventory)
	DEFINE_DIALOG_CREATOR(CDlgInventoryDetail)
	DEFINE_DIALOG_CREATOR(CDlgInvisibleList)
	DEFINE_DIALOG_CREATOR(CDlgItemDesc)
	DEFINE_DIALOG_CREATOR(CDlgMailList)
	DEFINE_DIALOG_CREATOR(CDlgMailOption)
	DEFINE_DIALOG_CREATOR(CDlgMailRead)
	DEFINE_DIALOG_CREATOR(CDlgMailToFriends)
	DEFINE_DIALOG_CREATOR(CDlgOnlineRemind)
	DEFINE_DIALOG_CREATOR(CDlgMailWrite)
	DEFINE_DIALOG_CREATOR(CDlgMiniMap)
	DEFINE_DIALOG_CREATOR(CDlgMinimizeBar)
	DEFINE_DIALOG_CREATOR(CDlgNPC)
	DEFINE_DIALOG_CREATOR(CDlgPKSetting)
	DEFINE_DIALOG_CREATOR(CDlgPQ)
	DEFINE_DIALOG_CREATOR(CDlgPalette)
	DEFINE_DIALOG_CREATOR(CDlgPalette2)
	DEFINE_DIALOG_CREATOR(CDlgPetDetail)
	DEFINE_DIALOG_CREATOR(CDlgPetDye)
	DEFINE_DIALOG_CREATOR(CDlgPetFittingRoom)
	DEFINE_DIALOG_CREATOR(CDlgPetHatch)
	DEFINE_DIALOG_CREATOR(CDlgPetList)
	DEFINE_DIALOG_CREATOR(CDlgPetRec)
	DEFINE_DIALOG_CREATOR(CDlgPetRename)
	DEFINE_DIALOG_CREATOR(CDlgPetRetrain)
	DEFINE_DIALOG_CREATOR(CDlgPlantPet)
	DEFINE_DIALOG_CREATOR(CDlgPopMsg)
	DEFINE_DIALOG_CREATOR(CDlgProclaim)
	DEFINE_DIALOG_CREATOR(CDlgProduce)
	DEFINE_DIALOG_CREATOR(CDlgProfView)
	DEFINE_DIALOG_CREATOR(CDlgQShop)
	DEFINE_DIALOG_CREATOR(CDlgQShopItem)
	DEFINE_DIALOG_CREATOR(CDlgQuestion)
	DEFINE_DIALOG_CREATOR(CDlgQuickAction)
	DEFINE_DIALOG_CREATOR(CDlgQuickBar)
	DEFINE_DIALOG_CREATOR(CDlgQuickBarPet)
	DEFINE_DIALOG_CREATOR(CDlgRandProp)
	DEFINE_DIALOG_CREATOR(CDlgRefineTrans)
	DEFINE_DIALOG_CREATOR(CDlgReportToGM)
	DEFINE_DIALOG_CREATOR(CDlgResetProp)
	DEFINE_DIALOG_CREATOR(CDlgRevive)
	DEFINE_DIALOG_CREATOR(CDlgScriptHelp)
	DEFINE_DIALOG_CREATOR(CDlgScriptTip)
	DEFINE_DIALOG_CREATOR(CDlgScriptHelpHistory)
	DEFINE_DIALOG_CREATOR(CDlgBlackList)
	DEFINE_DIALOG_CREATOR(CDlgSettingGame)
	DEFINE_DIALOG_CREATOR(CDlgSettingScriptHelp)
	DEFINE_DIALOG_CREATOR(CDlgSettingSystem)
	DEFINE_DIALOG_CREATOR(CDlgSettingVideo)
	DEFINE_DIALOG_CREATOR(CDlgSettingQuickKey)
	DEFINE_DIALOG_CREATOR(CDlgShop)
	DEFINE_DIALOG_CREATOR(CDlgSkillEdit)
	DEFINE_DIALOG_CREATOR(CDlgSkillTree)
	DEFINE_DIALOG_CREATOR(CDlgSplit)
	DEFINE_DIALOG_CREATOR(CDlgStorage)
	DEFINE_DIALOG_CREATOR(CDlgStorageChangePW)
	DEFINE_DIALOG_CREATOR(CDlgStoragePW)
	DEFINE_DIALOG_CREATOR(CDlgSystem)
	DEFINE_DIALOG_CREATOR(CDlgSystem2)
	DEFINE_DIALOG_CREATOR(CDlgSystem3)
	DEFINE_DIALOG_CREATOR(CDlgOnlineAward)
	DEFINE_DIALOG_CREATOR(CDlgTarget)
	DEFINE_DIALOG_CREATOR(CDlgTargetOfTarget)
	DEFINE_DIALOG_CREATOR(CDlgTask)
	DEFINE_DIALOG_CREATOR(CDlgTaskList)
	DEFINE_DIALOG_CREATOR(CDlgTaskTrace)
	DEFINE_DIALOG_CREATOR(CDlgTeach)
	DEFINE_DIALOG_CREATOR(CDlgTeamContext)
	DEFINE_DIALOG_CREATOR(CDlgTeamMain)
	DEFINE_DIALOG_CREATOR(CDlgTeamMate)
	DEFINE_DIALOG_CREATOR(CDlgTextHelp)
	DEFINE_DIALOG_CREATOR(CDlgTrade)
	DEFINE_DIALOG_CREATOR(CDlgTreasureMap)
	DEFINE_DIALOG_CREATOR(CDlgWarTower)
	DEFINE_DIALOG_CREATOR(CDlgWebList)
	DEFINE_DIALOG_CREATOR(CDlgWebMyShop)
	DEFINE_DIALOG_CREATOR(CDlgWebTradeOption)
	DEFINE_DIALOG_CREATOR(CDlgWebViewProduct)
	DEFINE_DIALOG_CREATOR(CDlgWedding)
	DEFINE_DIALOG_CREATOR(CDlgWeddingInvite)
	DEFINE_DIALOG_CREATOR(CDlgWiki)
	DEFINE_DIALOG_CREATOR(CDlgWikiAreaList)
	DEFINE_DIALOG_CREATOR(CDlgWikiEquipment)
	DEFINE_DIALOG_CREATOR(CDlgWikiEquipmentDrop)
	DEFINE_DIALOG_CREATOR(CDlgWikiFeature)
	DEFINE_DIALOG_CREATOR(CDlgWikiGuide)
	DEFINE_DIALOG_CREATOR(CDlgWikiItem)
	DEFINE_DIALOG_CREATOR(CDlgWikiItemConfirm)
	DEFINE_DIALOG_CREATOR(CDlgWikiMonster)
	DEFINE_DIALOG_CREATOR(CDlgWikiMonsterDrop)
	DEFINE_DIALOG_CREATOR(CDlgWikiMonsterList)
	DEFINE_DIALOG_CREATOR(CDlgWikiMonsterTask)
	DEFINE_DIALOG_CREATOR(CDlgWikiNPCList)
	DEFINE_DIALOG_CREATOR(CDlgWikiRandomProperty)
	DEFINE_DIALOG_CREATOR(CDlgWikiRecipe)
	DEFINE_DIALOG_CREATOR(CDlgWikiRecipeDetail)
	DEFINE_DIALOG_CREATOR(CDlgWikiSkill)
	DEFINE_DIALOG_CREATOR(CDlgWikiTask)
	DEFINE_DIALOG_CREATOR(CDlgWikiTaskDetail)
	DEFINE_DIALOG_CREATOR(CDlgWikiTaskList)
	DEFINE_DIALOG_CREATOR(CDlgWorldMap)
	DEFINE_DIALOG_CREATOR(CDlgPreviewProduce)
	DEFINE_DIALOG_CREATOR(CDlgCountryMap)
	DEFINE_DIALOG_CREATOR(CDlgCountryMove)
	DEFINE_DIALOG_CREATOR(CDlgCountryWarMap)
	DEFINE_DIALOG_CREATOR(CDlgCountryScore)
	DEFINE_DIALOG_CREATOR(CDlgMessageBox)
	DEFINE_DIALOG_CREATOR(CDlgAutoHPMP)
	DEFINE_DIALOG_CREATOR(CDlgCountryReward)
	DEFINE_DIALOG_CREATOR(CDlgBuyConfirm)
	DEFINE_DIALOG_CREATOR(CDlgQuickPay)
	DEFINE_DIALOG_CREATOR(CDlgAskHelpToGM2)
	DEFINE_DIALOG_CREATOR(CDlgBoothSet)
	DEFINE_DIALOG_CREATOR(CDlgPetEvolution)
	DEFINE_DIALOG_CREATOR(CDlgPreviewPetRebuild)
	DEFINE_DIALOG_CREATOR(CDlgPetEvoAnim)
	DEFINE_DIALOG_CREATOR(CDlgTaskConfirm)
	DEFINE_DIALOG_CREATOR(CDlgTaskHintPop)
	DEFINE_DIALOG_CREATOR(CDlgModifyName)
	DEFINE_DIALOG_CREATOR(CDlgMeridians)
	DEFINE_DIALOG_CREATOR(CDlgMeridiansImpact)
	DEFINE_DIALOG_CREATOR(CDlgStoneChange)
	DEFINE_DIALOG_CREATOR(CDlgStoneTransfer)
	DEFINE_DIALOG_CREATOR(CDlgStoneReplace)
	DEFINE_DIALOG_CREATOR(CDlgViewModel)
	DEFINE_DIALOG_CREATOR(CDlgKingAuction)
	DEFINE_DIALOG_CREATOR(CDlgKingElection)
	DEFINE_DIALOG_CREATOR(CDlgKingCommand)
	DEFINE_DIALOG_CREATOR(CDlgDomainCondition)
	DEFINE_DIALOG_CREATOR(CDlgKingResult)
	DEFINE_DIALOG_CREATOR(CDlgPetNature)
	DEFINE_DIALOG_CREATOR(CDlgGivingFor)
	DEFINE_DIALOG_CREATOR(CDlgCountryMapSub)
	DEFINE_DIALOG_CREATOR(CDlgFashionSplit)
	DEFINE_DIALOG_CREATOR(CDlgOptimizeGfx)
	DEFINE_DIALOG_CREATOR(CDlgOptimizeMem)
	DEFINE_DIALOG_CREATOR(CDlgOptimizeFunc)
	DEFINE_DIALOG_CREATOR(CDlgOptimizeOS)
	DEFINE_DIALOG_CREATOR(CDlgTouchShop)
	DEFINE_DIALOG_CREATOR(CDlgOfflineShopCreate)
	DEFINE_DIALOG_CREATOR(CDlgOfflineShopItemsList)
	DEFINE_DIALOG_CREATOR(CDlgOfflineShopList)
	DEFINE_DIALOG_CREATOR(CDlgOfflineShopSelfSetting)
	DEFINE_DIALOG_CREATOR(CDlgOfflineShopStore)
	DEFINE_DIALOG_CREATOR(CDlgOfflineShopType)
	DEFINE_DIALOG_CREATOR(CDlgInputNO3)
	DEFINE_DIALOG_CREATOR(CDlgComplexTitle)
	DEFINE_DIALOG_CREATOR(CDlgTitleBubble)
	DEFINE_DIALOG_CREATOR(CDlgTitleChallenge)
	DEFINE_DIALOG_CREATOR(CDlgTitleList)

	DEFINE_DIALOG_CREATOR(CDlgMatchAward)
	DEFINE_DIALOG_CREATOR(CDlgMatchInfoHint)
	DEFINE_DIALOG_CREATOR(CDlgMatchModeSelect)
	DEFINE_DIALOG_CREATOR(CDlgMatchProfileHabit)
	DEFINE_DIALOG_CREATOR(CDlgMatchResult)
	DEFINE_DIALOG_CREATOR(CDlgMatchProfileAge)
	DEFINE_DIALOG_CREATOR(CDlgMatchProfileHobby)
	DEFINE_DIALOG_CREATOR(CDlgMatchProfileSetting)
	DEFINE_DIALOG_CREATOR(CDlgSignIn)

	DEFINE_DIALOG_CREATOR(CDlgAutoHelp)

	DEFINE_DIALOG_CREATOR(CDlgHistoryStage)
	DEFINE_DIALOG_CREATOR(CDlgGiftCard)
	DEFINE_DIALOG_CREATOR(CDlgReincarnation)
	DEFINE_DIALOG_CREATOR(CDlgReincarnationBook)
	DEFINE_DIALOG_CREATOR(CDlgReincarnationRewrite)
	DEFINE_DIALOG_CREATOR(CDlgDownloadSpeed)

	DEFINE_DIALOG_CREATOR(CDlgTaskAction)
	DEFINE_DIALOG_CREATOR(CDlgExclusiveAward)
	DEFINE_DIALOG_CREATOR(CDlgAutoPolicy)

	DEFINE_DIALOG_CREATOR(CDlgChariotEnter)
	DEFINE_DIALOG_CREATOR(CDlgChariotRevive)
	DEFINE_DIALOG_CREATOR(CDlgChariotOpt)
	DEFINE_DIALOG_CREATOR(CDlgChariotRankList)
	DEFINE_DIALOG_CREATOR(CDlgChariotScore)
	DEFINE_DIALOG_CREATOR(CDlgChariotMultiKill)
	DEFINE_DIALOG_CREATOR(CDlgChariotInfo)
	DEFINE_DIALOG_CREATOR(CDlgChariotAmount)

	DEFINE_DIALOG_CREATOR(CDlgSystem4)
	DEFINE_DIALOG_CREATOR(CDlgSystem5)
	DEFINE_DIALOG_CREATOR(CDlgSysModQuickBar)
	DEFINE_DIALOG_CREATOR(CDlgSysModule)
	DEFINE_DIALOG_CREATOR(CDlgGeneralCard)
	DEFINE_DIALOG_CREATOR(CDlgGeneralCardBuy)
	DEFINE_DIALOG_CREATOR(CDlgGeneralCardBuySetting)
	
	DEFINE_DIALOG_CREATOR(CDlgGenCardRespawn)
	DEFINE_DIALOG_CREATOR(CDlgGenCardCollection)
	DEFINE_DIALOG_CREATOR(CDlgTokenShop)
	DEFINE_DIALOG_CREATOR(CDlgMonsterSpirit)
	DEFINE_DIALOG_CREATOR(CDlgFullScreenEffectShow)
	DEFINE_DIALOG_CREATOR(CDlgLevel2UpgradeShow)
	DEFINE_DIALOG_CREATOR(CDlgQShopBuy)
	DEFINE_DIALOG_CREATOR(CDlgFlySwordImprove)	
	DEFINE_DIALOG_CREATOR(CDlgQuickBuyPop)
	DEFINE_DIALOG_CREATOR(CDlgTargetItem)	
	DEFINE_DIALOG_CREATOR(CDlgShopCart)	
	DEFINE_DIALOG_CREATOR(CDlgShopCartSub)
	DEFINE_DIALOG_CREATOR(CDlgShopCartSubList)
	DEFINE_DIALOG_CREATOR(CDlgFashionShop)
	DEFINE_DIALOG_CREATOR(CDlgFashionShopItem)

	DEFINE_DIALOG_CREATOR(CDlgSkillAction)
	DEFINE_DIALOG_CREATOR(CDlgSkillSubList)
	DEFINE_DIALOG_CREATOR(CDlgSkillSubListItem)
	DEFINE_DIALOG_CREATOR(CDlgSkillSubListRankItem)
	DEFINE_DIALOG_CREATOR(CDlgSkillSubTree)
	DEFINE_DIALOG_CREATOR(CDlgSkillSubOther)
	DEFINE_DIALOG_CREATOR(CDlgSkillSubAction)
	DEFINE_DIALOG_CREATOR(CDlgSkillSubPool)

	DEFINE_DIALOG_CREATOR(CDlgRedSpark)
	DEFINE_DIALOG_CREATOR(CDlgBlackScreen)
	DEFINE_DIALOG_CREATOR(CDlgComboShortCutBar)

	DEFINE_DIALOG_CREATOR(CDlgRandomMap)

	DEFINE_DIALOG_CREATOR(CDlgPureColorFashionTip)
	DEFINE_DIALOG_CREATOR(CDlgIceThunderBall)

	DEFINE_DIALOG_CREATOR(CDlgQuestionTask)
	DEFINE_DIALOG_CREATOR(CDlgSimpleCostItemService)

	//	声明从界面文件名创建对话框的函数类型
	typedef	PAUIDIALOG	(*DialogCreatorFunc)(void);

	//	声明从界面文件名到对话框创建函数的映射
	typedef abase::hash_map<ConstChar, DialogCreatorFunc, ConstCharHashFunc>	DialogCreatorMap;

	//	定义映射函数
	static DialogCreatorFunc FindDialogCreator(const AString &strFileName)
	{
		static DialogCreatorMap	s_mDialogCreator;

		static bool	s_init(false);
		if (!s_init)
		{
			s_init = true;

			//	初始化映射函数
			//MAP_FILE_TO_CREATOR("action.xml", CDlgAction)
			MAP_FILE_TO_CREATOR("activity.xml", CDlgActivity)
			MAP_FILE_TO_CREATOR("arrangeteamsip.xml", CDlgArrangeTeam)
			MAP_FILE_TO_CREATOR("inputstring6.xml", CDlgAskHelpToGM)
			MAP_FILE_TO_CREATOR("auctionbuylist.xml", CDlgAuctionBuyList)
			MAP_FILE_TO_CREATOR("auctionmyattention.xml", CDlgAuctionBuyList)
			MAP_FILE_TO_CREATOR("auctionlist.xml", CDlgAuctionList)
			MAP_FILE_TO_CREATOR("auctionsearch.xml", CDlgAuctionSearch)
			MAP_FILE_TO_CREATOR("auctionselllist.xml", CDlgAuctionSellList)
			MAP_FILE_TO_CREATOR("autolock.xml", CDlgAutoLock)
			MAP_FILE_TO_CREATOR("automove.xml", CDlgAutoMove)
			MAP_FILE_TO_CREATOR("hidemove.xml", CDlgAutoMove)
			MAP_FILE_TO_CREATOR("autotask.xml", CDlgAutoTask)
			MAP_FILE_TO_CREATOR("award.xml", CDlgAward)
			MAP_FILE_TO_CREATOR("bbs.xml", CDlgBBS)
			MAP_FILE_TO_CREATOR("bshop.xml", CDlgBShop)
			MAP_FILE_TO_CREATOR("backhelp.xml", CDlgBackHelp)
			MAP_FILE_TO_CREATOR("backothers.xml", CDlgBackOthers)
			MAP_FILE_TO_CREATOR("backself.xml", CDlgBackSelf)
			MAP_FILE_TO_CREATOR("bag.xml", CDlgBag)
			MAP_FILE_TO_CREATOR("pshop2.xml", CDlgBoothElse)
			MAP_FILE_TO_CREATOR("pshop1.xml", CDlgBoothSelf)
			MAP_FILE_TO_CREATOR("broadcast.xml", CDlgBroadCast)
			MAP_FILE_TO_CREATOR("buddystate.xml", CDlgBuddyState)
			MAP_FILE_TO_CREATOR("cwindowmain.xml", CDlgCWindow)
			MAP_FILE_TO_CREATOR("camera.xml", CDlgCamera)
			MAP_FILE_TO_CREATOR("changeskirt.xml", CDlgChangeSkirt)
			MAP_FILE_TO_CREATOR("channelchat.xml", CDlgChannelChat)
			MAP_FILE_TO_CREATOR("channelcreate.xml", CDlgChannelCreate)
			MAP_FILE_TO_CREATOR("channeljoin.xml", CDlgChannelJoin)
			MAP_FILE_TO_CREATOR("channeloption.xml", CDlgChannelOption)
			MAP_FILE_TO_CREATOR("inputstring9.xml", CDlgChannelPW)
			MAP_FILE_TO_CREATOR("character.xml", CDlgCharacter)
			MAP_FILE_TO_CREATOR("chat.xml", CDlgChat)
			MAP_FILE_TO_CREATOR("chatbig.xml", CDlgChat)
			MAP_FILE_TO_CREATOR("chatsmall.xml", CDlgChat)
			MAP_FILE_TO_CREATOR("popface.xml", CDlgChat)
			MAP_FILE_TO_CREATOR("popface01.xml", CDlgChat)
			MAP_FILE_TO_CREATOR("popface02.xml", CDlgChat)
			MAP_FILE_TO_CREATOR("whisperchat.xml", CDlgChat)
			MAP_FILE_TO_CREATOR("whisperchoose.xml", CDlgChat)
			MAP_FILE_TO_CREATOR("congregate0.xml", CDlgCongregate)
			MAP_FILE_TO_CREATOR("congregate1.xml", CDlgCongregate)
			MAP_FILE_TO_CREATOR("congregate2.xml", CDlgCongregate)
			MAP_FILE_TO_CREATOR("console.xml", CDlgConsole)
			MAP_FILE_TO_CREATOR("coupon.xml", CDlgCoupon)
			MAP_FILE_TO_CREATOR("customize.xml", CDlgCustomize)
			MAP_FILE_TO_CREATOR("faces\\chooseeye.xml", CDlgCustomizeEye)
			MAP_FILE_TO_CREATOR("faces\\chooseeye2.xml", CDlgCustomizeEye2)
			MAP_FILE_TO_CREATOR("faces\\chooseface.xml", CDlgCustomizeFace)
			MAP_FILE_TO_CREATOR("faces\\chooseface2.xml", CDlgCustomizeFace2)
			MAP_FILE_TO_CREATOR("faces\\choosehair.xml", CDlgCustomizeHair)
			MAP_FILE_TO_CREATOR("faces\\choosenosemouth.xml", CDlgCustomizeNoseMouth)
			MAP_FILE_TO_CREATOR("faces\\choosenosemouth2.xml", CDlgCustomizeNoseMouth2)
			MAP_FILE_TO_CREATOR("faces\\choosefacepainting.xml", CDlgCustomizePaint)
			MAP_FILE_TO_CREATOR("faces\\chooseprecustomize.xml", CDlgCustomizePre)
			MAP_FILE_TO_CREATOR("faces\\choosestature.xml", CDlgCustomizeStature)
			MAP_FILE_TO_CREATOR("damagerep.xml", CDlgDamageRep)
			MAP_FILE_TO_CREATOR("doubleexp.xml", CDlgDoubleExp)
			MAP_FILE_TO_CREATOR("doubleexps.xml", CDlgDoubleExpS)
			MAP_FILE_TO_CREATOR("dragdrop.xml", CDlgDragDrop)
			MAP_FILE_TO_CREATOR("elf_equipremove.xml", CDlgELFEquipRemove)
			MAP_FILE_TO_CREATOR("character_genius.xml", CDlgELFGenius)
			MAP_FILE_TO_CREATOR("elf_geniusreset.xml", CDlgELFGeniusReset)
			MAP_FILE_TO_CREATOR("elf_inexp.xml", CDlgELFInExp)
			MAP_FILE_TO_CREATOR("elf_learn.xml", CDlgELFLearn)
			MAP_FILE_TO_CREATOR("character_elf.xml", CDlgELFProp)
			MAP_FILE_TO_CREATOR("elf_refine.xml", CDlgELFRefine)
			MAP_FILE_TO_CREATOR("elf_refinetrans.xml", CDlgELFRefineTrans)
			MAP_FILE_TO_CREATOR("elf_retrain.xml", CDlgELFRetrain)
			MAP_FILE_TO_CREATOR("elf_skilltree1.xml", CDlgELFSkillTree)
			MAP_FILE_TO_CREATOR("elf_skilltree2.xml", CDlgELFSkillTree)
			MAP_FILE_TO_CREATOR("elf_topill.xml", CDlgELFToPill)
			MAP_FILE_TO_CREATOR("elf_transaction1.xml", CDlgELFTransaction)
			MAP_FILE_TO_CREATOR("elf_transaction2.xml", CDlgELFTransaction)
			MAP_FILE_TO_CREATOR("elf_xidian.xml", CDlgELFXiDian)
			MAP_FILE_TO_CREATOR("epequip.xml", CDlgEPEquip)
			MAP_FILE_TO_CREATOR("earthbagrank.xml", CDlgEarthBagRank)
			MAP_FILE_TO_CREATOR("engrave.xml", CDlgEngrave)
			MAP_FILE_TO_CREATOR("equipbind.xml", CDlgEquipBind)
			MAP_FILE_TO_CREATOR("equipdestroy.xml", CDlgEquipDestroy)
			MAP_FILE_TO_CREATOR("allequipdye.xml", CDlgEquipDye)
			MAP_FILE_TO_CREATOR("equipdye.xml", CDlgEquipDye)
			MAP_FILE_TO_CREATOR("equipmark.xml", CDlgEquipMark)
			MAP_FILE_TO_CREATOR("equiprefine.xml", CDlgEquipRefine)
			MAP_FILE_TO_CREATOR("equipslot.xml", CDlgEquipSlot)
			MAP_FILE_TO_CREATOR("equipundestroy.xml", CDlgEquipUndestroy)
			MAP_FILE_TO_CREATOR("equipup.xml", CDlgEquipUp)
			MAP_FILE_TO_CREATOR("equipupensure.xml", CDlgEquipUpEnsure)
			MAP_FILE_TO_CREATOR("message2.xml", CDlgExit)
			MAP_FILE_TO_CREATOR("addexp.xml", CDlgExp)
			MAP_FILE_TO_CREATOR("setexp.xml", CDlgExp)
			MAP_FILE_TO_CREATOR("explorer.xml", CDlgExplorer)
			MAP_FILE_TO_CREATOR("explorerradio.xml", CDlgExplorer)
			MAP_FILE_TO_CREATOR("cosmetic.xml", CDlgFaceLift)
			MAP_FILE_TO_CREATOR("faces\\facename.xml", CDlgFaceName)
			MAP_FILE_TO_CREATOR("findplayer.xml", CDlgFindPlayer)
			MAP_FILE_TO_CREATOR("fittingroom_f.xml", CDlgFittingRoom)
			MAP_FILE_TO_CREATOR("fittingroom_m.xml", CDlgFittingRoom)
			MAP_FILE_TO_CREATOR("force.xml", CDlgForce)
			MAP_FILE_TO_CREATOR("force_activity.xml", CDlgForceActivity)
			MAP_FILE_TO_CREATOR("force_activityicon.xml", CDlgForceActivityIcon)
			MAP_FILE_TO_CREATOR("force_join.xml", CDlgForceNPC)
			MAP_FILE_TO_CREATOR("force_quit.xml", CDlgForceNPC)
			MAP_FILE_TO_CREATOR("fortress_build.xml", CDlgFortressBuild)
			MAP_FILE_TO_CREATOR("fortressbuildhelp.xml", CDlgFortressBuildHelp)
			MAP_FILE_TO_CREATOR("fortress_buildsub.xml", CDlgFortressBuildSub)
			MAP_FILE_TO_CREATOR("fortress_buildsublist.xml", CDlgFortressBuildSubList)
			MAP_FILE_TO_CREATOR("fortress_contrib.xml", CDlgFortressContrib)
			MAP_FILE_TO_CREATOR("fortress_exchange.xml", CDlgFortressExchange)
			MAP_FILE_TO_CREATOR("fortress_info.xml", CDlgFortressInfo)
			MAP_FILE_TO_CREATOR("fortress_material.xml", CDlgFortressMaterial)
			MAP_FILE_TO_CREATOR("fortress_mini.xml", CDlgFortressMini)
			MAP_FILE_TO_CREATOR("fortress_status.xml", CDlgFortressStatus)
			MAP_FILE_TO_CREATOR("fortress_war.xml", CDlgFortressWar)
			MAP_FILE_TO_CREATOR("fortress_warlist.xml", CDlgFortressWarList)
			MAP_FILE_TO_CREATOR("friendchat0.xml", CDlgFriendChat)
			MAP_FILE_TO_CREATOR("friendchat1.xml", CDlgFriendChat)
			MAP_FILE_TO_CREATOR("friendchat2.xml", CDlgFriendChat)
			MAP_FILE_TO_CREATOR("friendchat3.xml", CDlgFriendChat)
			MAP_FILE_TO_CREATOR("friendchat4.xml", CDlgFriendChat)
			MAP_FILE_TO_CREATOR("friendchat5.xml", CDlgFriendChat)
			MAP_FILE_TO_CREATOR("friendchat6.xml", CDlgFriendChat)
			MAP_FILE_TO_CREATOR("friendchat7.xml", CDlgFriendChat)
			MAP_FILE_TO_CREATOR("friendchat8.xml", CDlgFriendChat)
			MAP_FILE_TO_CREATOR("friendchat9.xml", CDlgFriendChat)
			MAP_FILE_TO_CREATOR("friendcolor.xml", CDlgFriendColor)
			MAP_FILE_TO_CREATOR("friendgroup.xml", CDlgFriendGroup)
			MAP_FILE_TO_CREATOR("friendhistory.xml", CDlgFriendHistory)
			MAP_FILE_TO_CREATOR("friendlist.xml", CDlgFriendList)
			MAP_FILE_TO_CREATOR("friendoptiongroup.xml", CDlgFriendOptionGroup)
			MAP_FILE_TO_CREATOR("friendoptionname.xml", CDlgFriendOptionName)
			MAP_FILE_TO_CREATOR("friendoptionnormal.xml", CDlgFriendOptionNormal)
			MAP_FILE_TO_CREATOR("message1.xml", CDlgFriendRequest)
			MAP_FILE_TO_CREATOR("gmconsole.xml", CDlgGMConsole)
			MAP_FILE_TO_CREATOR("gmconsole2.xml",CDlgGMConsoleForbid)
			MAP_FILE_TO_CREATOR("gmdelmsgreason.xml", CDlgGMDelMsgReason)
			MAP_FILE_TO_CREATOR("gmfinishmsg.xml", CDlgGMFinishMsg)
			MAP_FILE_TO_CREATOR("gmparam.xml", CDlgGMParam)
			MAP_FILE_TO_CREATOR("gmqueryitem.xml", CDlgGMQueryItem)
			MAP_FILE_TO_CREATOR("gmqueryitemsub.xml", CDlgGMQueryItemSub)
			MAP_FILE_TO_CREATOR("sagedemonconvert.xml", CDlgGodEvilConvert)
			MAP_FILE_TO_CREATOR("goldaccount.xml", CDlgGoldAccount)
			MAP_FILE_TO_CREATOR("goldinquire.xml", CDlgGoldInquire)
			MAP_FILE_TO_CREATOR("goldpwdchange.xml", CDlgGoldPassword)
			MAP_FILE_TO_CREATOR("goldpwdinput.xml", CDlgGoldPassword)
			MAP_FILE_TO_CREATOR("goldtrade.xml", CDlgGoldTrade)
			MAP_FILE_TO_CREATOR("guildcreate.xml", CDlgGuildCreate)
			MAP_FILE_TO_CREATOR("guild_diplomacyapply.xml", CDlgGuildDiplomacyApply)
			MAP_FILE_TO_CREATOR("guild_diplomacyman.xml", CDlgGuildDiplomacyMan)
			MAP_FILE_TO_CREATOR("guildmanage.xml", CDlgGuildMan)
			MAP_FILE_TO_CREATOR("guildmap.xml", CDlgGuildMap)
			MAP_FILE_TO_CREATOR("gmapchallenge.xml", CDlgGuildMapStatus)
			MAP_FILE_TO_CREATOR("gmapstatus1.xml", CDlgGuildMapStatus)
			MAP_FILE_TO_CREATOR("gmapstatus2.xml", CDlgGuildMapStatus)
			MAP_FILE_TO_CREATOR("gmapstatus3.xml", CDlgFactionPVPStatus)
			MAP_FILE_TO_CREATOR("gmaptravel.xml", CDlgGuildMapTravel)
			MAP_FILE_TO_CREATOR("help.xml", CDlgHelp)
			MAP_FILE_TO_CREATOR("hidehp.xml", CDlgHost)
			MAP_FILE_TO_CREATOR("hpmpxp.xml", CDlgHost)
			MAP_FILE_TO_CREATOR("magicprogress1.xml", CDlgHost)
			MAP_FILE_TO_CREATOR("magicprogress2.xml", CDlgHost)
			MAP_FILE_TO_CREATOR("magicprogress3.xml", CDlgHost)
			MAP_FILE_TO_CREATOR("progress.xml", CDlgHost)
			MAP_FILE_TO_CREATOR("progress1.xml", CDlgHost)
			MAP_FILE_TO_CREATOR("progress2.xml", CDlgHost)
			MAP_FILE_TO_CREATOR("progress_hp.xml", CDlgHost)
			MAP_FILE_TO_CREATOR("progress_mp.xml", CDlgHost)
			MAP_FILE_TO_CREATOR("character_elfchat.xml", CDlgHostELF)
			MAP_FILE_TO_CREATOR("character_elfchatzoom.xml", CDlgHostELF)
			MAP_FILE_TO_CREATOR("hpmpxppet0.xml", CDlgHostPet)
			MAP_FILE_TO_CREATOR("hpmpxppet1.xml", CDlgHostPet)
			MAP_FILE_TO_CREATOR("hpmpxppet2.xml", CDlgHostPet)
			MAP_FILE_TO_CREATOR("hpmpxppet3.xml", CDlgHostPet)
			MAP_FILE_TO_CREATOR("hpmpxppet5.xml", CDlgHostPet)
			MAP_FILE_TO_CREATOR("identify.xml", CDlgIdentify)
			MAP_FILE_TO_CREATOR("pop.xml", CDlgInfo)
			MAP_FILE_TO_CREATOR("poplist.xml", CDlgInfo)
			MAP_FILE_TO_CREATOR("inputno.xml", CDlgInputNO)
			MAP_FILE_TO_CREATOR("inputno2.xml", CDlgInputNO2)
			MAP_FILE_TO_CREATOR("inputstring5.xml", CDlgInputName)
			MAP_FILE_TO_CREATOR("disenchase.xml", CDlgInstall)
			MAP_FILE_TO_CREATOR("enchase.xml", CDlgInstall)
			MAP_FILE_TO_CREATOR("inventory_f.xml", CDlgInventory)
			MAP_FILE_TO_CREATOR("inventory_m.xml", CDlgInventory)
			MAP_FILE_TO_CREATOR("inventorydetail.xml", CDlgInventoryDetail)
			MAP_FILE_TO_CREATOR("invisiblelist.xml", CDlgInvisibleList)
			MAP_FILE_TO_CREATOR("itemdesc.xml", CDlgItemDesc)
			MAP_FILE_TO_CREATOR("itemdesc2.xml", CDlgItemDesc)
			MAP_FILE_TO_CREATOR("itemdesc3.xml", CDlgItemDesc)
			MAP_FILE_TO_CREATOR("maillist.xml", CDlgMailList)
			MAP_FILE_TO_CREATOR("mailoption.xml", CDlgMailOption)
			MAP_FILE_TO_CREATOR("mailread.xml", CDlgMailRead)
			MAP_FILE_TO_CREATOR("mailtofriend.xml", CDlgMailToFriends)
			MAP_FILE_TO_CREATOR("mailtips.xml", CDlgOnlineRemind)
			MAP_FILE_TO_CREATOR("mailwrite.xml", CDlgMailWrite)
			MAP_FILE_TO_CREATOR("map.xml", CDlgMiniMap)
			MAP_FILE_TO_CREATOR("mark.xml", CDlgMiniMap)
			MAP_FILE_TO_CREATOR("hidemap.xml", CDlgMiniMap)
			MAP_FILE_TO_CREATOR("minimizebar0.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("minimizebar1.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("minimizebar10.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("minimizebar11.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("minimizebar2.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("minimizebar3.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("minimizebar4.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("minimizebar5.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("minimizebar6.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("minimizebar7.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("minimizebar8.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("minimizebar9.xml", CDlgMinimizeBar)
			MAP_FILE_TO_CREATOR("npc.xml", CDlgNPC)
			MAP_FILE_TO_CREATOR("pksetting.xml", CDlgPKSetting)
			MAP_FILE_TO_CREATOR("pqrank.xml", CDlgPQ)
			MAP_FILE_TO_CREATOR("pqtitle.xml", CDlgPQ)
			MAP_FILE_TO_CREATOR("palette.xml", CDlgPalette)
			MAP_FILE_TO_CREATOR("palette2.xml", CDlgPalette2)
			MAP_FILE_TO_CREATOR("petdetail0.xml", CDlgPetDetail)
			MAP_FILE_TO_CREATOR("petdetail1.xml", CDlgPetDetail)
			MAP_FILE_TO_CREATOR("petdetail2.xml", CDlgPetDetail)
			MAP_FILE_TO_CREATOR("petdetail3.xml", CDlgPetDetail)
			MAP_FILE_TO_CREATOR("petdetail5.xml", CDlgPetDetail)
			MAP_FILE_TO_CREATOR("petdye.xml", CDlgPetDye)
			MAP_FILE_TO_CREATOR("fittingroom_pet.xml", CDlgPetFittingRoom)
			MAP_FILE_TO_CREATOR("pethatch.xml", CDlgPetHatch)
			MAP_FILE_TO_CREATOR("petlist.xml", CDlgPetList)
			MAP_FILE_TO_CREATOR("petrec.xml", CDlgPetRec)
			MAP_FILE_TO_CREATOR("petrename.xml", CDlgPetRename)
			MAP_FILE_TO_CREATOR("petretrain.xml", CDlgPetRetrain)
			MAP_FILE_TO_CREATOR("plantpet.xml", CDlgPlantPet)
			MAP_FILE_TO_CREATOR("popmsg2.xml", CDlgPopMsg)
			MAP_FILE_TO_CREATOR("inputstring8.xml", CDlgProclaim)
			MAP_FILE_TO_CREATOR("produce1.xml", CDlgProduce)
			MAP_FILE_TO_CREATOR("produce_icon.xml", CDlgProduce)
			MAP_FILE_TO_CREATOR("profview.xml", CDlgProfView)
			MAP_FILE_TO_CREATOR("backshop.xml", CDlgQShop)
			MAP_FILE_TO_CREATOR("qshop.xml", CDlgQShop)
			MAP_FILE_TO_CREATOR("qshopitemdefault0.xml", CDlgQShopItem)
			MAP_FILE_TO_CREATOR("qshopitemdefault1.xml", CDlgQShopItem)
			MAP_FILE_TO_CREATOR("qshopitemdefault2.xml", CDlgQShopItem)
			MAP_FILE_TO_CREATOR("qshopitemdefault3.xml", CDlgQShopItem)
			MAP_FILE_TO_CREATOR("qshopitemdefault4.xml", CDlgQShopItem)
			MAP_FILE_TO_CREATOR("qshopitemdefault5.xml", CDlgQShopItem)
			MAP_FILE_TO_CREATOR("qshopitemdefault6.xml", CDlgQShopItem)
			MAP_FILE_TO_CREATOR("qshopitemdefault7.xml", CDlgQShopItem)
			MAP_FILE_TO_CREATOR("question.xml", CDlgQuestion)
			MAP_FILE_TO_CREATOR("quickaction.xml", CDlgQuickAction)
			MAP_FILE_TO_CREATOR("quickbar8ha.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar8hb_1.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar8hb_2.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar8hb_3.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar8va.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar8vb_1.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar8vb_2.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar8vb_3.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9ha.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9hb_1.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9hb_2.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9hb_3.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9hb_4.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9hb_5.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9va.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9vb_1.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9vb_2.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9vb_3.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9vb_4.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbar9vb_5.xml", CDlgQuickBar)
			MAP_FILE_TO_CREATOR("quickbarpeth.xml", CDlgQuickBarPet)
			MAP_FILE_TO_CREATOR("quickbarpetv.xml", CDlgQuickBarPet)
			MAP_FILE_TO_CREATOR("randprop.xml", CDlgRandProp)
			MAP_FILE_TO_CREATOR("refinetrans.xml", CDlgRefineTrans)
			MAP_FILE_TO_CREATOR("inputstring7.xml", CDlgReportToGM)
			MAP_FILE_TO_CREATOR("resetprop.xml", CDlgResetProp)
			MAP_FILE_TO_CREATOR("message4.xml", CDlgRevive)
			MAP_FILE_TO_CREATOR("message6.xml", CDlgBuyConfirm)
			MAP_FILE_TO_CREATOR("scripthelp.xml", CDlgScriptHelp)
			MAP_FILE_TO_CREATOR("scripthelphistory.xml", CDlgScriptHelpHistory)
			MAP_FILE_TO_CREATOR("scripttip.xml", CDlgScriptTip)
			MAP_FILE_TO_CREATOR("friendlist_blacklist.xml", CDlgBlackList)
			MAP_FILE_TO_CREATOR("setting_game.xml", CDlgSettingGame)
			MAP_FILE_TO_CREATOR("setting_scripthelp.xml", CDlgSettingScriptHelp)
			MAP_FILE_TO_CREATOR("setting_system.xml", CDlgSettingSystem)
			MAP_FILE_TO_CREATOR("setting_video.xml", CDlgSettingVideo)
			MAP_FILE_TO_CREATOR("setting_quickkey.xml", CDlgSettingQuickKey)
			MAP_FILE_TO_CREATOR("shop.xml", CDlgShop)
			MAP_FILE_TO_CREATOR("skilledit.xml", CDlgSkillEdit)
			MAP_FILE_TO_CREATOR("skill_pet.xml", CDlgSkillTree)
			MAP_FILE_TO_CREATOR("split.xml", CDlgSplit)
			MAP_FILE_TO_CREATOR("storage.xml", CDlgStorage)
			MAP_FILE_TO_CREATOR("storage1.xml", CDlgStorage)
			MAP_FILE_TO_CREATOR("storage2.xml", CDlgStorage)
			MAP_FILE_TO_CREATOR("storage3.xml", CDlgStorage)
			MAP_FILE_TO_CREATOR("generalcardstorage.xml", CDlgStorage)
			MAP_FILE_TO_CREATOR("inputstring3.xml", CDlgStorageChangePW)
			MAP_FILE_TO_CREATOR("inputstring.xml", CDlgStoragePW)
			MAP_FILE_TO_CREATOR("inputstring10.xml", CDlgStoragePW)
			MAP_FILE_TO_CREATOR("winmain.xml", CDlgSystem)
			MAP_FILE_TO_CREATOR("winmain2.xml", CDlgSystem2)
			MAP_FILE_TO_CREATOR("winmain3.xml", CDlgSystem3)
			MAP_FILE_TO_CREATOR("winmainb.xml", CDlgSystem)
			MAP_FILE_TO_CREATOR("winmain2b.xml", CDlgSystem2)
			MAP_FILE_TO_CREATOR("winmain3b.xml", CDlgSystem3)
			MAP_FILE_TO_CREATOR("addexp2.xml", CDlgOnlineAward)
			MAP_FILE_TO_CREATOR("addexp2q.xml", CDlgOnlineAward)
			MAP_FILE_TO_CREATOR("hpmpother.xml", CDlgTarget)
			MAP_FILE_TO_CREATOR("hpmpotherb.xml", CDlgTarget)
			MAP_FILE_TO_CREATOR("hpother.xml", CDlgTarget)
			MAP_FILE_TO_CREATOR("hpotherb.xml", CDlgTarget)
			MAP_FILE_TO_CREATOR("hpotherplayer.xml", CDlgTargetOfTarget)
			MAP_FILE_TO_CREATOR("hpothermonster.xml", CDlgTargetOfTarget)
			MAP_FILE_TO_CREATOR("quest.xml", CDlgTask)
			MAP_FILE_TO_CREATOR("questlist.xml", CDlgTaskList)
			MAP_FILE_TO_CREATOR("questminion.xml", CDlgTaskTrace)
			MAP_FILE_TO_CREATOR("teach.xml", CDlgTeach)
			MAP_FILE_TO_CREATOR("groupme1.xml", CDlgTeamContext)
			MAP_FILE_TO_CREATOR("groupme2.xml", CDlgTeamContext)
			MAP_FILE_TO_CREATOR("groupother1.xml", CDlgTeamContext)
			MAP_FILE_TO_CREATOR("groupother2.xml", CDlgTeamContext)
			MAP_FILE_TO_CREATOR("teammain.xml", CDlgTeamMain)
			MAP_FILE_TO_CREATOR("teammate1.xml", CDlgTeamMate)
			MAP_FILE_TO_CREATOR("teammate2.xml", CDlgTeamMate)
			MAP_FILE_TO_CREATOR("teammate3.xml", CDlgTeamMate)
			MAP_FILE_TO_CREATOR("teammate4.xml", CDlgTeamMate)
			MAP_FILE_TO_CREATOR("teammate5.xml", CDlgTeamMate)
			MAP_FILE_TO_CREATOR("teammate6.xml", CDlgTeamMate)
			MAP_FILE_TO_CREATOR("teammate7.xml", CDlgTeamMate)
			MAP_FILE_TO_CREATOR("teammate8.xml", CDlgTeamMate)
			MAP_FILE_TO_CREATOR("teammate9.xml", CDlgTeamMate)
			MAP_FILE_TO_CREATOR("addexphelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("addexp2help.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("backselfhelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("bshophelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("fortressinfohelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("fortresswarhelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("guilddiplomacyapplyhelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("questlisthelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("sagedemonconverthelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("web_shophelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("wedding_help.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("wiki\\wiki_guidehelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("wiki\\wikihelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("trade.xml", CDlgTrade)
			MAP_FILE_TO_CREATOR("treasuremap.xml", CDlgTreasureMap)
			MAP_FILE_TO_CREATOR("wartower.xml", CDlgWarTower)
			MAP_FILE_TO_CREATOR("weblist.xml", CDlgWebList)
			MAP_FILE_TO_CREATOR("web_myshop.xml", CDlgWebMyShop)
			MAP_FILE_TO_CREATOR("web_tradeoption.xml", CDlgWebTradeOption)
			MAP_FILE_TO_CREATOR("web_viewproduct.xml", CDlgWebViewProduct)
			MAP_FILE_TO_CREATOR("wedding.xml", CDlgWedding)
			MAP_FILE_TO_CREATOR("wedding_invitation.xml", CDlgWeddingInvite)
			MAP_FILE_TO_CREATOR("wiki\\wiki.xml", CDlgWiki)
			MAP_FILE_TO_CREATOR("wiki\\wiki_areaselect.xml", CDlgWikiAreaList)
			MAP_FILE_TO_CREATOR("wiki\\wiki_equipmentsearch.xml", CDlgWikiEquipment)
			MAP_FILE_TO_CREATOR("wiki\\wiki_equipmentdrop.xml", CDlgWikiEquipmentDrop)
			MAP_FILE_TO_CREATOR("wiki\\wiki_featuresearch.xml", CDlgWikiFeature)
			MAP_FILE_TO_CREATOR("wiki\\wiki_guidesearch.xml", CDlgWikiGuide)
			MAP_FILE_TO_CREATOR("wiki\\wiki_itemsearch.xml", CDlgWikiItem)
			MAP_FILE_TO_CREATOR("wiki\\wiki_itemconfirm.xml", CDlgWikiItemConfirm)
			MAP_FILE_TO_CREATOR("wiki\\wiki_monstersearch.xml", CDlgWikiMonster)
			MAP_FILE_TO_CREATOR("wiki\\wiki_monsterdrop.xml", CDlgWikiMonsterDrop)
			MAP_FILE_TO_CREATOR("wiki\\wiki_itemdrop.xml", CDlgWikiMonsterList)
			MAP_FILE_TO_CREATOR("wiki\\wiki_monstertask.xml", CDlgWikiMonsterTask)
			MAP_FILE_TO_CREATOR("wiki\\wiki_npcselect.xml", CDlgWikiNPCList)
			MAP_FILE_TO_CREATOR("wiki\\wiki_randomproperty.xml", CDlgWikiRandomProperty)
			MAP_FILE_TO_CREATOR("wiki\\wiki_recipesearch.xml", CDlgWikiRecipe)
			MAP_FILE_TO_CREATOR("wiki\\wiki_recipedetail.xml", CDlgWikiRecipeDetail)
			MAP_FILE_TO_CREATOR("wiki\\wiki_skillsearch.xml", CDlgWikiSkill)
			MAP_FILE_TO_CREATOR("wiki\\wiki_tasksearch.xml", CDlgWikiTask)
			MAP_FILE_TO_CREATOR("wiki\\wiki_taskdetail.xml", CDlgWikiTaskDetail)
			MAP_FILE_TO_CREATOR("wiki\\wiki_itemtask.xml", CDlgWikiTaskList)
			MAP_FILE_TO_CREATOR("bigmap.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("bigmapmark.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("flag1.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("flag2.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("flag3.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("flag4.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("flag5.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("sign1.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("sign2.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("sign3.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("sign4.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("sign5.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("worldmap.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("worldmapdetail.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("worldmaptravel.xml", CDlgWorldMap)
			MAP_FILE_TO_CREATOR("previewproduce.xml",CDlgPreviewProduce)
			MAP_FILE_TO_CREATOR("countrymap.xml", CDlgCountryMap)
			MAP_FILE_TO_CREATOR("countrymove.xml", CDlgCountryMove)
			MAP_FILE_TO_CREATOR("countrywarmap.xml", CDlgCountryWarMap)
			MAP_FILE_TO_CREATOR("countrypoint.xml", CDlgCountryScore)
			MAP_FILE_TO_CREATOR("messagebox.xml", CDlgMessageBox)
			MAP_FILE_TO_CREATOR("autohpmp.xml", CDlgAutoHPMP)
			MAP_FILE_TO_CREATOR("countryreward.xml", CDlgCountryReward)
			MAP_FILE_TO_CREATOR("quickpay.xml", CDlgQuickPay);
			MAP_FILE_TO_CREATOR("inputstring11.xml", CDlgAskHelpToGM2)
			MAP_FILE_TO_CREATOR("pshopset.xml", CDlgBoothSet)
			MAP_FILE_TO_CREATOR("pettrain.xml", CDlgPetEvolution)
			MAP_FILE_TO_CREATOR("petskillstree.xml", CDlgPetNature)
			MAP_FILE_TO_CREATOR("previewpet.xml", CDlgPreviewPetRebuild)
			MAP_FILE_TO_CREATOR("petshow.xml", CDlgPetEvoAnim)
			MAP_FILE_TO_CREATOR("questdone.xml", CDlgTaskConfirm)
			MAP_FILE_TO_CREATOR("questq.xml", CDlgTaskHintPop)			
			MAP_FILE_TO_CREATOR("modifyname.xml", CDlgModifyName)
			MAP_FILE_TO_CREATOR("meridians.xml", CDlgMeridians)
			MAP_FILE_TO_CREATOR("meridiansimpact.xml", CDlgMeridiansImpact)
			MAP_FILE_TO_CREATOR("stone_change.xml",CDlgStoneChange)
			MAP_FILE_TO_CREATOR("stone_transfer.xml",CDlgStoneTransfer)
			MAP_FILE_TO_CREATOR("stone_replace.xml",CDlgStoneReplace)
			MAP_FILE_TO_CREATOR("viewmodel.xml", CDlgViewModel)
			MAP_FILE_TO_CREATOR("kingauctions.xml", CDlgKingAuction)
			MAP_FILE_TO_CREATOR("kingvote.xml", CDlgKingElection)
			MAP_FILE_TO_CREATOR("kingorder.xml", CDlgKingCommand)
			MAP_FILE_TO_CREATOR("kingset.xml", CDlgDomainCondition)
			MAP_FILE_TO_CREATOR("kingshow.xml", CDlgKingResult)
			MAP_FILE_TO_CREATOR("givingfor.xml", CDlgGivingFor)
			MAP_FILE_TO_CREATOR("countrymapsub.xml", CDlgCountryMapSub)
			MAP_FILE_TO_CREATOR("fashionsplit.xml", CDlgFashionSplit)
			MAP_FILE_TO_CREATOR("optimize_mem.xml", CDlgOptimizeMem)
			MAP_FILE_TO_CREATOR("optimize_gfx.xml", CDlgOptimizeGfx)
			MAP_FILE_TO_CREATOR("optimize_function.xml", CDlgOptimizeFunc)
			MAP_FILE_TO_CREATOR("optimize_os.xml", CDlgOptimizeOS)
			MAP_FILE_TO_CREATOR("touchshop.xml", CDlgTouchShop)
			MAP_FILE_TO_CREATOR("inputno3.xml", CDlgInputNO3)
			MAP_FILE_TO_CREATOR("offlineshopset.xml", CDlgOfflineShopSelfSetting)
			MAP_FILE_TO_CREATOR("offlineshopitemlist.xml", CDlgOfflineShopItemsList)
			MAP_FILE_TO_CREATOR("offlineshopbag.xml", CDlgOfflineShopStore)
			MAP_FILE_TO_CREATOR("offlineshoplist.xml", CDlgOfflineShopList)
			MAP_FILE_TO_CREATOR("offlineshoptype.xml", CDlgOfflineShopType)
			MAP_FILE_TO_CREATOR("offlineshopcreate.xml", CDlgOfflineShopCreate)
			MAP_FILE_TO_CREATOR("event.xml", CDlgComplexTitle)
			MAP_FILE_TO_CREATOR("event_gettitle.xml", CDlgTitleBubble)
			MAP_FILE_TO_CREATOR("event_list.xml", CDlgTitleChallenge)
			MAP_FILE_TO_CREATOR("event_titlelist.xml", CDlgTitleList)
			MAP_FILE_TO_CREATOR("match_setting1.xml", CDlgMatchProfileHabit)
			MAP_FILE_TO_CREATOR("match_setting2.xml", CDlgMatchProfileHobby)
			MAP_FILE_TO_CREATOR("match_setting3.xml", CDlgMatchProfileAge)
			MAP_FILE_TO_CREATOR("match_setting4.xml", CDlgMatchProfileSetting)
			MAP_FILE_TO_CREATOR("match_modesel.xml", CDlgMatchModeSelect)
			MAP_FILE_TO_CREATOR("match_result.xml", CDlgMatchResult)
			MAP_FILE_TO_CREATOR("match_infohint.xml", CDlgMatchInfoHint)			
			MAP_FILE_TO_CREATOR("offlineshophelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("signin.xml", CDlgSignIn)
			MAP_FILE_TO_CREATOR("activityhelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("wiki\\wikipop.xml", CDlgAutoHelp)
			MAP_FILE_TO_CREATOR("historypro.xml", CDlgHistoryStage)
			MAP_FILE_TO_CREATOR("giftcard.xml", CDlgGiftCard)
			MAP_FILE_TO_CREATOR("reincarnation.xml", CDlgReincarnation)
			MAP_FILE_TO_CREATOR("reincarnationbook.xml", CDlgReincarnationBook)
			MAP_FILE_TO_CREATOR("reincarnationrewrite.xml", CDlgReincarnationRewrite)
			MAP_FILE_TO_CREATOR("downloadspeed.xml", CDlgDownloadSpeed)
			MAP_FILE_TO_CREATOR("actionpop.xml", CDlgTaskAction)
			MAP_FILE_TO_CREATOR("exclusiveaward.xml", CDlgExclusiveAward)
			MAP_FILE_TO_CREATOR("autopolicy.xml", CDlgAutoPolicy)
			MAP_FILE_TO_CREATOR("autopolicyhelp.xml", CDlgTextHelp)
			MAP_FILE_TO_CREATOR("chariotbattle_revive.xml", CDlgChariotRevive)
			MAP_FILE_TO_CREATOR("chariotbattle_apply.xml", CDlgChariotEnter)
			MAP_FILE_TO_CREATOR("chariotui.xml", CDlgChariotOpt)
			MAP_FILE_TO_CREATOR("battlemsg01.xml", CDlgChariotScore)
			MAP_FILE_TO_CREATOR("battlemsg02.xml", CDlgChariotMultiKill)
			MAP_FILE_TO_CREATOR("chariotbattle_rank.xml", CDlgChariotRankList)
			MAP_FILE_TO_CREATOR("chariotinfo.xml", CDlgChariotInfo)
			MAP_FILE_TO_CREATOR("chariotbattle_info.xml",CDlgChariotAmount)
			MAP_FILE_TO_CREATOR("system.xml", CDlgSysModule)
			MAP_FILE_TO_CREATOR("quickbaricon.xml", CDlgSysModQuickBar)
			MAP_FILE_TO_CREATOR("winmain4.xml", CDlgSystem4)
			MAP_FILE_TO_CREATOR("winmain5.xml", CDlgSystem5)
			MAP_FILE_TO_CREATOR("winmain4b.xml", CDlgSystem4)
			MAP_FILE_TO_CREATOR("winmain5b.xml", CDlgSystem5)
			MAP_FILE_TO_CREATOR("generalcard.xml", CDlgGeneralCard)
			MAP_FILE_TO_CREATOR("generalcardre.xml", CDlgGenCardRespawn)
			MAP_FILE_TO_CREATOR("generalcardhint.xml", CDlgGeneralCard)
			MAP_FILE_TO_CREATOR("generalcardhinthover.xml", CDlgGeneralCard)
			MAP_FILE_TO_CREATOR("generalcarddeschover.xml", CDlgItemDesc)
			MAP_FILE_TO_CREATOR("generalcarddeschover2.xml", CDlgItemDesc)
			MAP_FILE_TO_CREATOR("generalcardvolume.xml", CDlgGenCardCollection)
			MAP_FILE_TO_CREATOR("generalcardbuy.xml", CDlgGeneralCardBuy)
			MAP_FILE_TO_CREATOR("generalcardbuysetting.xml", CDlgGeneralCardBuySetting)
			MAP_FILE_TO_CREATOR("tokenshop.xml", CDlgTokenShop)
			MAP_FILE_TO_CREATOR("monsterspirit.xml", CDlgMonsterSpirit)
			MAP_FILE_TO_CREATOR("reincarnationshow.xml", CDlgFullScreenEffectShow)
			MAP_FILE_TO_CREATOR("level2upgradeshow.xml", CDlgLevel2UpgradeShow)
			MAP_FILE_TO_CREATOR("qshopbuy.xml", CDlgQShopBuy)
			MAP_FILE_TO_CREATOR("aircraft.xml",CDlgFlySwordImprove)
			MAP_FILE_TO_CREATOR("quickbuypop1.xml",CDlgQuickBuyPop)
			MAP_FILE_TO_CREATOR("quickbuypop2.xml",CDlgQuickBuyPop)
			MAP_FILE_TO_CREATOR("itempop.xml", CDlgTargetItem)
			MAP_FILE_TO_CREATOR("pvpreward.xml", CDlgFactionPVPRank)
			MAP_FILE_TO_CREATOR("shopcart_sub.xml", CDlgShopCartSub)
			MAP_FILE_TO_CREATOR("shopcart_sublist.xml", CDlgShopCartSubList)
			MAP_FILE_TO_CREATOR("shopcart.xml", CDlgShopCart)			
			MAP_FILE_TO_CREATOR("fashionshop.xml", CDlgFashionShop)
			MAP_FILE_TO_CREATOR("fashionshopitem0.xml", CDlgFashionShopItem)
			MAP_FILE_TO_CREATOR("fashionshopitem1.xml", CDlgFashionShopItem)
			MAP_FILE_TO_CREATOR("fashionshopitem2.xml", CDlgFashionShopItem)
			MAP_FILE_TO_CREATOR("fashionshopitem3.xml", CDlgFashionShopItem)
			MAP_FILE_TO_CREATOR("fashionshopitem4.xml", CDlgFashionShopItem)
			MAP_FILE_TO_CREATOR("fashionshopitem5.xml", CDlgFashionShopItem)
			MAP_FILE_TO_CREATOR("newskill_main.xml", CDlgSkillAction)
			MAP_FILE_TO_CREATOR("newskill_sublist.xml", CDlgSkillSubList)
			MAP_FILE_TO_CREATOR("newskill_sublistskill.xml", CDlgSkillSubListItem)
			MAP_FILE_TO_CREATOR("newskill_sublistrank.xml", CDlgSkillSubListRankItem)
			MAP_FILE_TO_CREATOR("newskill_subtreeevil.xml", CDlgSkillSubTree)
			MAP_FILE_TO_CREATOR("newskill_subtreegod.xml", CDlgSkillSubTree)
			MAP_FILE_TO_CREATOR("newskill_subtreebase.xml", CDlgSkillSubTree)
			MAP_FILE_TO_CREATOR("newskill_subotherskill.xml", CDlgSkillSubOther)
			MAP_FILE_TO_CREATOR("newskill_subaction.xml", CDlgSkillSubAction)
			MAP_FILE_TO_CREATOR("newskill_subpool.xml", CDlgSkillSubPool)
			MAP_FILE_TO_CREATOR("redspark.xml", CDlgRedSpark)
			MAP_FILE_TO_CREATOR("blackscreen.xml", CDlgBlackScreen)
			MAP_FILE_TO_CREATOR("comboskill.xml", CDlgComboShortCutBar)

			MAP_FILE_TO_CREATOR("randommap.xml",CDlgRandomMap)
			
			MAP_FILE_TO_CREATOR("purecolor.xml", CDlgPureColorFashionTip)
			MAP_FILE_TO_CREATOR("icethunderball.xml", CDlgIceThunderBall)

			MAP_FILE_TO_CREATOR("questiontask.xml", CDlgQuestionTask)
			MAP_FILE_TO_CREATOR("simplecostitemservice.xml", CDlgSimpleCostItemService)

		}

		AString strTemp = strFileName;
		strTemp.MakeLower();
		DialogCreatorMap::iterator it = s_mDialogCreator.find((const char *)strTemp);
		return (it != s_mDialogCreator.end()) ? it->second : NULL;
	}
}

PAUIDIALOG CECGameUIMan::CreateDlgInstance(const AString strTemplName)
{
	DialogCreatorFunc f = FindDialogCreator(strTemplName);
	return (f != NULL) ? f() : new CDlgBase;
}

// File		: EC_UIConfig.cpp
// Creator	: Xu Wenbin
// Date		: 2012/5/14

#include "EC_UIConfigs.h"
#include "EC_Split.h"
#include "EC_Time.h"

#include <AWIniFile.h>
#include <AAssist.h>
#include <algorithm>

static bool ParseTimeFromString(const AWString &str, struct tm &t){
	bool result(false);
	while (!str.IsEmpty()){
		CECSplitHelperW splitter(str, L"-");
		if (splitter.Count() != 3){
			break;
		}
		int year = splitter.ToInt(0);
		if (year < 1970 || year > 2050){
			break;
		}
		int month = splitter.ToInt(1);
		if (month < 1 || month > 12){
			break;
		}
		int day = splitter.ToInt(2);
		if (day < 1 || day > 31){
			break;
		}
		t.tm_year = year - 1900;
		t.tm_mon = month - 1;
		t.tm_mday = day;
		result = true;
		break;
	}
	return result;
}

void CECUIConfig::Load(const char *filePath)
{
	AWIniFile ItemFile;
	
	if (ItemFile.Open(filePath))
	{
		//	LoginUI
		{
			const wchar_t *szSec = _AL("LoginUI");
			LoginUI & login = GetLoginUI();
			login.bEnableOtherLogin = ItemFile.GetValueAsInt(szSec, _AL("EnableOtherLogin"), 0) != 0;
			login.bAlwaysTryAutoLogin = ItemFile.GetValueAsInt(szSec, _AL("AlwaysTryAutoLogin"), 0) != 0;
			login.bEnableForceLogin = ItemFile.GetValueAsInt(szSec, _AL("EnableForceLogin"), 0) != 0;
			login.bSkipFirstLoadProgress = ItemFile.GetValueAsInt(szSec, _AL("SkipFirstLoadProgress"), 0) != 0;
			login.bAvoidLoginUI = ItemFile.GetValueAsInt(szSec, _AL("AvoidLoginUI"), 0) != 0;
			login.strHotServerGroupName = ItemFile.GetValueAsString(szSec, _AL("HotServerGroupName"), login.strHotServerGroupName);
			login.nMaxRecordAccount = ItemFile.GetValueAsInt(szSec, _AL("MaxRecordAccount"), login.nMaxRecordAccount);
			login.nMinInputToRemindAccount = ItemFile.GetValueAsInt(szSec, _AL("MinInputToRemindAccount"), login.nMinInputToRemindAccount);
		}

		//	GameUI
		{
			const wchar_t *szSec = _AL("GameUI");

			GetGameUI().bEnableTalkToGM = ItemFile.GetValueAsInt(szSec, _AL("EnableTalkToGM"), 0) != 0;
			GetGameUI().bEnableReportPlayerSpeakToGM = ItemFile.GetValueAsInt(szSec, _AL("EnableReportPlayerSpeakToGM"), 0) != 0;
			GetGameUI().bEnableReportPluginWithFeedback = ItemFile.GetValueAsInt(szSec, _AL("EnableReportPluginWithFeedback"), 0) != 0;

			GetGameUI().bEnableTrashPwdRemind = ItemFile.GetValueAsInt(szSec, _AL("EnableTrashPwdRemind"), 0) != 0;
			GetGameUI().nWallowHintType = static_cast<GameUI::WallowHintType>(ItemFile.GetValueAsInt(szSec, _AL("WallowHintType"), 0));
			GetGameUI().bEnableIE = ItemFile.GetValueAsInt(szSec, _AL("EnableIE"), 0) != 0;
			GetGameUI().bEnableShowIP = ItemFile.GetValueAsInt(szSec, _AL("EnableShowIP"), 0) != 0;
			GetGameUI().bEnableCompleteAccount = ItemFile.GetValueAsInt(szSec, _AL("EnableCompleteAccount"), 0) != 0;
			GetGameUI().bEnableFortressBuildDestroy = ItemFile.GetValueAsInt(szSec, _AL("EnableFortressBuildDestroy"), 0) != 0;
			GetGameUI().bShowNameInCountryWar = ItemFile.GetValueAsInt(szSec, _AL("ShowNameInCountryWar"), 0) != 0;
			GetGameUI().bEnableQuickPay = ItemFile.GetValueAsInt(szSec, _AL("EnableQuickPay"), 0) != 0;
			GetGameUI().nEquipMarkMinInkNum = ItemFile.GetValueAsInt(szSec, _AL("EquipMarkMinInkNum"), GetGameUI().nEquipMarkMinInkNum);
			GetGameUI().nGTLoginCoolDown = ItemFile.GetValueAsInt(szSec, _AL("GTLoginCoolDown"), GetGameUI().nGTLoginCoolDown);
			GetGameUI().bEnableGTOnSpecialServer = ItemFile.GetValueAsInt(szSec, _AL("EnableGTOnSpecialServer"), 0) != 0;
			GetGameUI().bEnableWebTradeSort = ItemFile.GetValueAsInt(szSec, _AL("EnableWebTradeSort"), 0) != 0;
			GetGameUI().bEnablePlayerRename = ItemFile.GetValueAsInt(szSec, _AL("EnablePlayerRename"), 0) != 0;
			GetGameUI().bEnableCheckNewbieGift = ItemFile.GetValueAsInt(szSec, _AL("EnableCheckNewbieGift"), 0) != 0;
			GetGameUI().bEnableQShopFilter = ItemFile.GetValueAsInt(szSec, _AL("EnableQShopFilter"), 0) != 0;
			GetGameUI().bEnableGivingFor = ItemFile.GetValueAsInt(szSec, _AL("EnableGivingFor"), 0) != 0;
			GetGameUI().bEnableGivingForTaskLimitedItem = ItemFile.GetValueAsInt(szSec, _AL("EnableGivingForTaskLimitedItem"), 1) != 0;
			GetGameUI().nCountryWarKingMaxDomainLimit = ItemFile.GetValueAsInt(szSec, _AL("CountryWarKingMaxDomainLimit"), GetGameUI().nCountryWarKingMaxDomainLimit);
			GetGameUI().bEnableOptimize = ItemFile.GetValueAsInt(szSec, _AL("EnableOptimize"), 0) != 0;
			GetGameUI().nFashionSplitCost = ItemFile.GetValueAsInt(szSec, _AL("FashionSplitCost"), GetGameUI().nFashionSplitCost);
			GetGameUI().bEnableTouch = ItemFile.GetValueAsInt(szSec, _AL("EnableTouch"), 0) != 0;
			GetGameUI().bEnableTWRecharge = ItemFile.GetValueAsInt(szSec, _AL("EnableTWRecharge"), 0) != 0;
			GetGameUI().bEnableTitle = ItemFile.GetValueAsInt(szSec, _AL("EnableTitle"), 1) != 0;
			GetGameUI().nAutoSimplifySpeed = ItemFile.GetValueAsInt(szSec, _AL("AutoSimplifySpeed"), GetGameUI().nAutoSimplifySpeed);			

			GetGameUI().nChariotApplyLevel = ItemFile.GetValueAsInt(szSec,_AL("ChariotApplyLevel"),GetGameUI().nChariotApplyLevel);
			GetGameUI().nChariotApplyLevel2 = ItemFile.GetValueAsInt(szSec,_AL("ChariotApplyLevel2"),GetGameUI().nChariotApplyLevel2);
			GetGameUI().nChariotApplyReincarnation = ItemFile.GetValueAsInt(szSec,_AL("ChariotApplyReincarnation"),GetGameUI().nChariotApplyReincarnation);

			GetGameUI().nExitAutoExtractWikiStateTime = ItemFile.GetValueAsInt(szSec,_AL("ExitAutoExtractWikiStateTime"),GetGameUI().nExitAutoExtractWikiStateTime);
			GetGameUI().nCloseWikiPopDlgTime = ItemFile.GetValueAsInt(szSec,_AL("CloseWikiPopDlgTime"),GetGameUI().nCloseWikiPopDlgTime);
			GetGameUI().nCloseWikiMsgInfoTime = ItemFile.GetValueAsInt(szSec,_AL("CloseWikiMsgInfoTime"),GetGameUI().nCloseWikiMsgInfoTime);
			GetGameUI().nOpenWikiPopDlgTime = ItemFile.GetValueAsInt(szSec,_AL("OpenWikiPopDlgTime"),GetGameUI().nOpenWikiPopDlgTime);

			GetGameUI().nChariotReviveTimeout = ItemFile.GetValueAsInt(szSec,_AL("ChariotReviveTimeout"),GetGameUI().nChariotReviveTimeout);

			GetGameUI().nHistoryQueryTimeInterval = ItemFile.GetValueAsInt(szSec,_AL("HistoryQueryTimeInterval"),GetGameUI().nHistoryQueryTimeInterval);
			GetGameUI().bEnableAutoWiki = ItemFile.GetValueAsInt(szSec,_AL("EnableAutoWiki"),0) != 0;
			GetGameUI().nMonsterSpiritGatherTimesPerWeekMax = ItemFile.GetValueAsInt(szSec,_AL("MonsterSpiritGatherTimesPerWeekMax"),GetGameUI().nMonsterSpiritGatherTimesPerWeekMax);
			GetGameUI().bEnableAutoPolicy = ItemFile.GetValueAsBoolean(szSec, _AL("EnableAutoPolicy"), true);
			GetGameUI().bEnablePWService = ItemFile.GetValueAsBoolean(szSec, _AL("EnablePWService"), true);

			GetGameUI().nChariotAmount = ItemFile.GetValueAsInt(szSec,_AL("ChariotAmount"),GetGameUI().nChariotAmount);
			GetGameUI().bEnableActionSwitch = ItemFile.GetValueAsBoolean(szSec,_AL("EnableActionSwitch"),GetGameUI().bEnableActionSwitch);
			GetGameUI().nCountryWarLiveShowUpdateInterval = ItemFile.GetValueAsInt(szSec,_AL("CountryWarLiveShowUpdateInterval"),GetGameUI().nCountryWarLiveShowUpdateInterval);

			GetGameUI().bEnableRecommendQShopItem = ItemFile.GetValueAsBoolean(szSec,_AL("EnableRecommendQShopItem"),GetGameUI().bEnableRecommendQShopItem);

			GetGameUI().bEnableRandShop = ItemFile.GetValueAsBoolean(szSec,_AL("EnableRandShop"),GetGameUI().bEnableRandShop);
			GetGameUI().nPokerShopConfig = ItemFile.GetValueAsInt(szSec,_AL("PokerShopConfig"),GetGameUI().nPokerShopConfig);
			GetGameUI().nPokerShopLevelLimit = ItemFile.GetValueAsInt(szSec,_AL("PokerShopLevelLimit"),GetGameUI().nPokerShopLevelLimit);
			GetGameUI().nContributionTaskLevelLimit = ItemFile.GetValueAsInt(szSec, _AL("ContributionTaskLevelLimit"), GetGameUI().nContributionTaskLevelLimit);
			GetGameUI().bEnableQShopFashionShop = ItemFile.GetValueAsBoolean(szSec, _AL("EnableQShopFashionShop"), GetGameUI().bEnableQShopFashionShop);
			GetGameUI().bEnableBackShopFashionShop = ItemFile.GetValueAsBoolean(szSec, _AL("EnableBackShopFashionShop"), GetGameUI().bEnableBackShopFashionShop);
			GetGameUI().bEnableCeilPriceBeforeDiscountToGold = ItemFile.GetValueAsBoolean(szSec, _AL("EnableCeilPriceBeforeDiscountToGold"), GetGameUI().bEnableCeilPriceBeforeDiscountToGold);
			GetGameUI().nMaxFriendRemarksNameLength = ItemFile.GetValueAsInt(szSec,_AL("MaxFriendRemarksNameLength"),GetGameUI().nMaxFriendRemarksNameLength);
			GetGameUI().bEnableQShopFashionShopFlashSale = ItemFile.GetValueAsBoolean(szSec, _AL("EnableQShopFashionShopFlashSale"), GetGameUI().bEnableQShopFashionShopFlashSale);
			GetGameUI().bEnableBackShopFashionShopFlashSale = ItemFile.GetValueAsBoolean(szSec, _AL("EnableBackShopFashionShopFlashSale"), GetGameUI().bEnableBackShopFashionShopFlashSale);		
			GetGameUI().strQShopFashionShopFlashSaleTitle = ItemFile.GetValueAsString(szSec, _AL("QShopFashionShopFlashSaleTitle"), GetGameUI().strQShopFashionShopFlashSaleTitle);
			GetGameUI().strBackShopFashionShopFlashSaleTitle = ItemFile.GetValueAsString(szSec, _AL("BackShopFashionShopFlashSaleTitle"), GetGameUI().strBackShopFashionShopFlashSaleTitle);
			GetGameUI().bEnablePlayerChangeGender = ItemFile.GetValueAsBoolean(szSec, _AL("EnablePlayerChangeGender"), GetGameUI().bEnablePlayerChangeGender);

			//	CountryWarBonus
			GetGameUI().nCountryWarBonus.clear();
			AWString strValue = ItemFile.GetValueAsString(szSec, _AL("CountryWarBonus"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i(0); i < splitter.Count(); ++ i)
				{
					int bonus = splitter.ToInt(i);
					if (bonus < 0 || i>0 && bonus < GetGameUI().nCountryWarBonus.back())
					{
						ASSERT(false);
						a_LogOutput(1, "Invalid CountryWarBonus as %s within %s", AC2AS(strValue), filePath);
						GetGameUI().nCountryWarBonus.clear();
						break;
					}
					GetGameUI().nCountryWarBonus.push_back(bonus);
				}
			}//	CountryWarBonus
			
			//	CountryWarEnterCondition
			strValue = ItemFile.GetValueAsString(szSec, _AL("CountryWarEnterCondition"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				bool bOK(false);
				while (true)
				{
					if (splitter.Count() < 3) break;
					
					int level = splitter.ToInt(0);
					if (level < 0) break;
					
					int id = splitter.ToInt(1);
					if (id < 0) break;
					
					int count = splitter.ToInt(2);
					if (count < 0) break;
					
					GetGameUI().nCountryWarEnterLevel = level;
					GetGameUI().nCountryWarEnterItem = id;
					GetGameUI().nCountryWarEnterItemCount = count;
					
					bOK = true;
					break;
				}
				if (!bOK)
				{
					ASSERT(false);
					a_LogOutput(1, "Invalid CountryWarEnterCondition as %s within %s", AC2AS(strValue), filePath);
				}
			}//	CountryWarEnterCondition

			//	CrossServerEnterCondition
			strValue = ItemFile.GetValueAsString(szSec, _AL("CrossServerEnterCondition"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				bool bOK(false);
				while (true)
				{
					if (splitter.Count() < 2) break;
					
					int level = splitter.ToInt(0);
					if (level < 0) break;
					
					int level2 = splitter.ToInt(1);
					if (level2 < 0) break;
					
					GetGameUI().nCrossServerEnterLevel = level;
					GetGameUI().nCrossServerEnterLevel2 = level2;
					
					bOK = true;
					break;
				}
				if (!bOK)
				{
					ASSERT(false);
					a_LogOutput(1, "Invalid CrossServerEnterCondition as %s within %s", AC2AS(strValue), filePath);
				}
			}//	CrossServerEnterCondition

			//	CountryWarPlayerLimit
			strValue = ItemFile.GetValueAsString(szSec, _AL("CountryWarPlayerLimit"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					int limit = splitter.ToInt(i);
					if (limit <= 0){
						ASSERT(false);
						break;
					}
					GetGameUI().nCountryWarPlayerLimit.push_back(limit);
				}
				if (splitter.Count() != (int)GetGameUI().nCountryWarPlayerLimit.size()){
					a_LogOutput(1, "Invalid CountryWarPlayerLimit as %s within %s", AC2AS(strValue), filePath);
				}
			}//	CountryWarPlayerLimit

			// MemoryBufferStage
			strValue = ItemFile.GetValueAsString(szSec, _AL("MemoryBufferStage"));
			if( !strValue.IsEmpty() )
			{
				CECSplitHelperW splitter(strValue);
				if( splitter.Count() >= 2 )
				{
					int nVal1 = splitter.ToInt(0);
					int nVal2 = splitter.ToInt(1);

					if( nVal1 < 0 || nVal1 > 2048 || nVal2 < 0 || nVal2 > 2048 || nVal1 > nVal2 )
					{
						ASSERT(0);
						a_LogOutput(1, "Invalid MemoryBufferStage as (%d %d)", nVal1, nVal2);
					}
					else
					{
						GetGameUI().nMemoryUsageLow = nVal1;
						GetGameUI().nMemoryUsageHigh = nVal2;
					}
				}
				else
				{
					ASSERT(0);
					a_LogOutput(1, "Invalid MemoryBufferStage as %s within %s", AC2AS(strValue), filePath);
				}
			}
			
			//	TouchEnabledMap
			strValue = ItemFile.GetValueAsString(szSec, _AL("TouchEnabledMap"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					int limit = splitter.ToInt(i);
					if (limit <= 0){
						ASSERT(false);
						break;
					}
					GetGameUI().nTouchEnabledMap.push_back(limit);
				}
				if (splitter.Count() != (int)GetGameUI().nTouchEnabledMap.size()){
					a_LogOutput(1, "Invalid TouchEnabledMap as %s within %s", AC2AS(strValue), filePath);
				}
			}//	TouchEnabledMap

			//  AutoTeamTransmitEnabledMap
			strValue = ItemFile.GetValueAsString(szSec, _AL("AutoTeamTransmitEnabledMap"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					int limit = splitter.ToInt(i);
					if (limit <= 0){
						ASSERT(false);
						break;
					}
					GetGameUI().nAutoTeamTransmitEnabledMap.push_back(limit);
				}
				if (splitter.Count() != (int)GetGameUI().nAutoTeamTransmitEnabledMap.size()){
					a_LogOutput(1, "Invalid AutoTeamTransmitEnabledMap as %s within %s", AC2AS(strValue), filePath);
				}
			}// AutoTeamTransmitEnabledMap

			//  TaskDisabledInMiniClient
			strValue = ItemFile.GetValueAsString(szSec, _AL("TaskDisabledInMiniClient"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					int limit = splitter.ToInt(i);
					if (limit <= 0){
						ASSERT(false);
						break;
					}
					GetGameUI().nTaskDisabledInMiniClient.push_back(limit);
				}
				if (splitter.Count() != (int)GetGameUI().nTaskDisabledInMiniClient.size()){
					a_LogOutput(1, "Invalid TaskDisabledInMiniClient as %s within %s", AC2AS(strValue), filePath);
				}
			}// TaskDisabledInMiniClient

			//  ItemDisabledInMiniClient
			strValue = ItemFile.GetValueAsString(szSec, _AL("ItemDisabledInMiniClient"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					int limit = splitter.ToInt(i);
					if (limit <= 0){
						ASSERT(false);
						break;
					}
					GetGameUI().nItemDisabledInMiniClient.push_back(limit);
				}
				if (splitter.Count() != (int)GetGameUI().nItemDisabledInMiniClient.size()){
					a_LogOutput(1, "Invalid ItemDisabledInMiniClient as %s within %s", AC2AS(strValue), filePath);
				}
			}// ItemDisabledInMiniClient

			//  MeridianFreeItem
			strValue = ItemFile.GetValueAsString(szSec, _AL("MeridianFreeItem"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					int limit = splitter.ToInt(i);
					if (limit <= 0){
						ASSERT(false);
						break;
					}
					GetGameUI().nMeridianFreeItem.push_back(limit);
				}
				if (splitter.Count() != (int)GetGameUI().nMeridianFreeItem.size()){
					a_LogOutput(1, "Invalid MeridianFreeItem as %s within %s", AC2AS(strValue), filePath);
				}
			}// MeridianFreeItem

			//  MeridianFreeItem
			strValue = ItemFile.GetValueAsString(szSec, _AL("MeridianNotFreeItem"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					int limit = splitter.ToInt(i);
					if (limit <= 0){
						ASSERT(false);
						break;
					}
					GetGameUI().nMeridianNotFreeItem.push_back(limit);
				}
				if (splitter.Count() != (int)GetGameUI().nMeridianNotFreeItem.size()){
					a_LogOutput(1, "Invalid MeridianFreeItem as %s within %s", AC2AS(strValue), filePath);
				}
			}// MeridianFreeItem
			
			//	TWRechargeParameter
			strValue = ItemFile.GetValueAsString(szSec, _AL("TWRechargeParameter"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperA splitter(AC2AS(strValue));
				bool bOK(false);
				while (true)
				{
					if (splitter.Count() < 3) break;
					GetGameUI().strTWRechargeAppID = splitter.ToString(0);
					GetGameUI().strTWRechargeGame = splitter.ToString(1);
					GetGameUI().strTWRechargeKey = splitter.ToString(2);
					bOK = true;
					break;
				}
				if (!bOK)
				{
					ASSERT(false);
					a_LogOutput(1, "Invalid TWRechargeParameter as %s within %s", AC2AS(strValue), filePath);
				}
			}//	TWRechargeParameter

			//	DefaultSystemModuleIndex
			strValue = ItemFile.GetValueAsString(szSec, _AL("DefaultSystemModuleIndex"));
			GetGameUI().nDefaultSystemModuleIndex.clear();
			if (!strValue.IsEmpty())
			{				
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					int index = splitter.ToInt(i);
					if (index < 0){
						ASSERT(false);
						break;
					}
					GetGameUI().nDefaultSystemModuleIndex.push_back(index);
				}
				if (splitter.Count() != (int)GetGameUI().nDefaultSystemModuleIndex.size()){
					a_LogOutput(1, "Invalid DefaultSystemModuleIndex as %s within %s", AC2AS(strValue), filePath);
				}
			}//	DefaultSystemModuleIndex
			
			//	RecommendShopItems
			strValue = ItemFile.GetValueAsString(szSec, _AL("RecommendShopItems"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					int id = splitter.ToInt(i);
					if (id <= 0){
						ASSERT(false);
						break;
					}
					GetGameUI().nRecommendShopItems.push_back(id);
				}
				if (splitter.Count() != (int)GetGameUI().nRecommendShopItems.size()){
					a_LogOutput(1, "Invalid RecommendShopItems as %s within %s", AC2AS(strValue), filePath);
				}
			}//	RecommendShopItems
			
			//	FashionShopAdImage
			strValue = ItemFile.GetValueAsString(szSec, _AL("FashionShopAdImage"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					AString strFileName = AC2AS(splitter.ToString(i));
					strFileName.TrimLeft();
					strFileName.TrimRight();
					if (strFileName.IsEmpty()){
						ASSERT(false);
						break;
					}
					GetGameUI().strFashionShopAdImage.push_back(strFileName);
				}
				if (splitter.Count() != (int)GetGameUI().strFashionShopAdImage.size()){
					a_LogOutput(1, "Invalid FashionShopAdImage as %s within %s", AC2AS(strValue), filePath);
				}
			}//	FashionShopAdImage

			//	FullScreenGfxForeground
			strValue = ItemFile.GetValueAsString(szSec, _AL("FullScreenGfxForeground"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					AString strFileName = AC2AS(splitter.ToString(i));
					strFileName.TrimLeft();
					strFileName.TrimRight();
					if (strFileName.IsEmpty()){
						ASSERT(false);
						break;
					}
					GetGameUI().strFullScreenGfxForeground.push_back(strFileName);
				}
				if (splitter.Count() != (int)GetGameUI().strFullScreenGfxForeground.size()){
					a_LogOutput(1, "Invalid FullScreenGfxForeground as %s within %s", AC2AS(strValue), filePath);
				}
			}//	FullScreenGfxForeground

			//	FullScreenGfxBackground
			strValue = ItemFile.GetValueAsString(szSec, _AL("FullScreenGfxBackground"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					AString strFileName = AC2AS(splitter.ToString(i));
					strFileName.TrimLeft();
					strFileName.TrimRight();
					if (strFileName.IsEmpty()){
						ASSERT(false);
						break;
					}
					GetGameUI().strFullScreenGfxBackground.push_back(strFileName);
				}
				if (splitter.Count() != (int)GetGameUI().strFullScreenGfxBackground.size()){
					a_LogOutput(1, "Invalid FullScreenGfxBackground as %s within %s", AC2AS(strValue), filePath);
				}
			}//	FullScreenGfxBackground
			

			//	关闭传送点的UITips
			strValue = ItemFile.GetValueAsString(szSec, _AL("TaskIDForDisableWayPointUITips"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					int taskid = splitter.ToInt(i);
					if (taskid <= 0){
						ASSERT(false);
						break;
					}
					GetGameUI().nTaskIDForDisableWayPointUITips.push_back(taskid);
				}
				if (splitter.Count() != (int)GetGameUI().nTaskIDForDisableWayPointUITips.size()){
					a_LogOutput(1, "Invalid TaskIDForDisableWayPointsUITips as %s within %s", AC2AS(strValue), filePath);
				}
			}//	关闭传送点的UITips
		}

		// OnlineRemind
		{
			const wchar_t *szSec = _AL("OnlineRemind");
			GetGameUI().bMailToFriendsSwitch = ItemFile.GetValueAsInt(szSec, _AL("MailToFriendsSwitch"), 0) != 0;
			GetGameUI().nMailToFriendsDaysNoLogin = ItemFile.GetValueAsInt(szSec, _AL("MailToFriendsDaysNoLogin"), GetGameUI().nMailToFriendsDaysNoLogin);
			GetGameUI().nMailToFriendsLevel = ItemFile.GetValueAsInt(szSec, _AL("MailToFriendsLevel"), GetGameUI().nMailToFriendsLevel);
			GetGameUI().nMailToFriendsDaysSendMail = ItemFile.GetValueAsInt(szSec, _AL("MailToFriendsDaysSendMail"), GetGameUI().nMailToFriendsDaysSendMail);

			GetGameUI().bActivityReminder = ItemFile.GetValueAsInt(szSec, _AL("ActivityReminder"), 0) != 0;
			GetGameUI().nActivityReminderLevel = ItemFile.GetValueAsInt(szSec, _AL("ActivityReminderLevel"), GetGameUI().nActivityReminderLevel);
			GetGameUI().nActivityReminderMaxLevelSoFar = ItemFile.GetValueAsInt(szSec, _AL("ActivityReminderMaxLevelSoFar"), GetGameUI().nActivityReminderMaxLevelSoFar);
			GetGameUI().nActivityReminderLevel2 = ItemFile.GetValueAsInt(szSec, _AL("ActivityReminderLevel2"), GetGameUI().nActivityReminderLevel2);
			GetGameUI().nActivityReminderReincarnationTimes = ItemFile.GetValueAsInt(szSec, _AL("ActivityReminderReincarnationTimes"), GetGameUI().nActivityReminderReincarnationTimes);
			GetGameUI().nActivityReminderRealmLevel = ItemFile.GetValueAsInt(szSec, _AL("ActivityReminderRealmLevel"), GetGameUI().nActivityReminderRealmLevel);
			GetGameUI().nActivityReminderReputation = ItemFile.GetValueAsInt(szSec, _AL("ActivityReminderReputation"), GetGameUI().nActivityReminderReputation);

			ACString strValue = ItemFile.GetValueAsString(szSec, _AL("ActivityReminderStartTime"));
			if (!ParseTimeFromString(strValue, GetGameUI().tActivityReminderStartTime)){
				a_LogOutput(1, "Invalid ActivityReminderStartTime as %s", AC2AS(strValue));
			}
			strValue = ItemFile.GetValueAsString(szSec, _AL("ActivityReminderEndTime"));
			if (!ParseTimeFromString(strValue, GetGameUI().tActivityReminderEndTime)){
				a_LogOutput(1, "Invalid ActivityReminderEndTime as %s", AC2AS(strValue));
			}
		}
		
		// RandomapInfo
		{
			const wchar_t *szSec = _AL("RandomapInfo");

			//	RandomMaps
			ACString strValue = ItemFile.GetValueAsString(szSec, _AL("RandomMap"));
			if (!strValue.IsEmpty())
			{
				CECSplitHelperW splitter(strValue);
				for (int i = 0; i < splitter.Count(); ++ i)
				{
					int mapid = splitter.ToInt(i);
					if (mapid <= 0){
						ASSERT(false);
						break;
					}
					GetGameUI().nRandomMaps.push_back(mapid);
				}
				if (splitter.Count() != (int)GetGameUI().nRandomMaps.size()){
					a_LogOutput(1, "Invalid RandomMap as %s within %s", AC2AS(strValue), filePath);
				}
			}//	RandomMaps

			strValue = ItemFile.GetValueAsString(szSec, _AL("DefaultItemID"));
			if (!strValue.IsEmpty()){
				CECSplitHelperW splitter(strValue);
				if (splitter.Count() == 2){
					GetGameUI().DefaultRandomMapItem.itemID = splitter.ToInt(0);
					GetGameUI().DefaultRandomMapItem.count = splitter.ToInt(1);
				}
				else{
					a_LogOutput(1, "Invalid DefaultItemID as %s within %s", AC2AS(strValue), filePath);
				}
			}
			int count = ItemFile.GetValueAsInt(szSec,_AL("RandommapItemCnt"),0);
			GetGameUI().SpecialRandomMapItems.clear();
			for (int i=0;i<count;i++){
				ACString keyName;
				keyName.Format(_AL("Map%d"),i);
				strValue = ItemFile.GetValueAsString(szSec, keyName);
				if (!strValue.IsEmpty()){
					CECSplitHelperW splitter(strValue);
					if (splitter.Count() == 3){
						RandomMapItem itemInfo;
						int mapid = splitter.ToInt(0);
						itemInfo.itemID = splitter.ToInt(1);
						itemInfo.count = splitter.ToInt(2);
						GetGameUI().SpecialRandomMapItems[mapid] = itemInfo;
					}
					else{
						a_LogOutput(1, "Invalid Special RandomMapInfo Error, %s within %s", AC2AS(strValue), filePath);
					}
				}
			}
		}

		ItemFile.Close();
	}
	else
	{
		a_LogOutput(1, "CECUIConfig::Init, Failed to Open %s", filePath);
	}
}

CECUIConfig & CECUIConfig::Instance()
{
	static CECUIConfig s_dummy;
	return s_dummy;
}

CECUIConfig::GameUI::GameUI()
: bMailToFriendsSwitch(false)
, nMailToFriendsDaysNoLogin(16)
, nMailToFriendsLevel(90)
, nMailToFriendsDaysSendMail(10)
, bActivityReminder(false)
, nActivityReminderLevel(0)
, nActivityReminderMaxLevelSoFar(0)
, nActivityReminderLevel2(0)
, nActivityReminderReincarnationTimes(0)
, nActivityReminderRealmLevel(0)
, nActivityReminderReputation(0)
, tActivityReminderStartTime(CECTime::GetZeroTime())
, tActivityReminderEndTime(CECTime::GetZeroTime())
, bEnableTalkToGM(false)
, bEnableTrashPwdRemind(false)
, nWallowHintType(WHT_DEFAULT)
, bEnableIE(false)
, bEnableShowIP(false)
, bEnableCompleteAccount(false)
, bEnableFortressBuildDestroy(false)
, bShowNameInCountryWar(false)
, nCountryWarEnterLevel(100)
, nCountryWarEnterItem(36672)
, nCountryWarEnterItemCount(1)
, bEnableQuickPay(false)
, nEquipMarkMinInkNum(1)
, bEnableReportPlayerSpeakToGM(false)
, bEnableReportPluginWithFeedback(false)
, nGTLoginCoolDown(10*1000)
, bEnableGTOnSpecialServer(false)
, nCrossServerEnterLevel(100)
, nCrossServerEnterLevel2(20)
, bEnableWebTradeSort(false)
, bEnablePlayerRename(false)
, bEnableCheckNewbieGift(false)
, bEnableQShopFilter(false)
, bEnableGivingFor(false)
, bEnableGivingForTaskLimitedItem(true)
, nCountryWarKingMaxDomainLimit(7)
, bEnableOptimize(false)
, nMemoryUsageLow(0)
, nMemoryUsageHigh(2048)
, nAutoSimplifySpeed(3000)
, nFashionSplitCost(100000)
, bEnableTouch(false)
, bEnableTWRecharge(false)
, bEnableTitle(true)
, nChariotReviveTimeout(30000)
, nHistoryQueryTimeInterval(10000)
, bEnableAutoWiki(true)
, nChariotApplyLevel(100)
, nChariotApplyLevel2(20)
, nChariotApplyReincarnation(1)	
, nChariotAmount(100)
, nExitAutoExtractWikiStateTime(60000)
, nCloseWikiPopDlgTime(300000)
, nCloseWikiMsgInfoTime(3000)
, nOpenWikiPopDlgTime(300000)
, nMonsterSpiritGatherTimesPerWeekMax(20)
, bEnableAutoPolicy(true)
, bEnablePWService(true)
, bEnableActionSwitch(true)
, nCountryWarLiveShowUpdateInterval(1)
, bEnableRecommendQShopItem(false)
, bEnableRandShop(false)
, nPokerShopConfig(0)
, nPokerShopLevelLimit(1)
, nContributionTaskLevelLimit(100)
, bEnableQShopFashionShop(false)
, bEnableBackShopFashionShop(false)
, bEnableCeilPriceBeforeDiscountToGold(false)
, nMaxFriendRemarksNameLength(12)
, bEnableQShopFashionShopFlashSale(false)
, bEnableBackShopFashionShopFlashSale(false)
, strQShopFashionShopFlashSaleTitle(_AL("FlashSale"))
, strBackShopFashionShopFlashSaleTitle(_AL("FlashSale"))
, bEnablePlayerChangeGender(false)
{
}

int CECUIConfig::GameUI::GetCountryWarBonusLevel(int currentBonus)const
{
	//	获取国战当前奖励级别，级别从0开始，数据无效时返回-1
	int level = -1;
	while (level+1 < (int)nCountryWarBonus.size()
		&& currentBonus > nCountryWarBonus[level+1])
	{
		level ++;
	}
	return level;
}

int CECUIConfig::GameUI::GetCountryWarPlayerLimit(int warType)const
{
	//	根据战场类型、获取单方人数上限
	int playerLimit(0);
	if (warType >= 0 && warType < (int)nCountryWarPlayerLimit.size())
		playerLimit = nCountryWarPlayerLimit[warType];
	return playerLimit;
}

bool CECUIConfig::GameUI::GetCanShowTouchShop(int idInst)const
{
	//	给定地图ID,判断能否显示 TouchShop
	return std::find(nTouchEnabledMap.begin(), nTouchEnabledMap.end(), idInst) != nTouchEnabledMap.end();
}

bool CECUIConfig::GameUI::IsTaskDisabledInMiniClient(int task_id) const
{
	return std::find(nTaskDisabledInMiniClient.begin(), nTaskDisabledInMiniClient.end(), task_id) != nTaskDisabledInMiniClient.end();
}

bool CECUIConfig::GameUI::IsItemDisabledInMiniClient(int item_id) const
{
	return std::find(nItemDisabledInMiniClient.begin(), nItemDisabledInMiniClient.end(), item_id) != nItemDisabledInMiniClient.end();
}
bool CECUIConfig::GameUI::IsMeridianFreeItem(int item_id) const
{
	return std::find(nMeridianFreeItem.begin(), nMeridianFreeItem.end(), item_id) != nMeridianFreeItem.end();
}
bool CECUIConfig::GameUI::IsMeridianNotFreeItem(int item_id) const
{
	return std::find(nMeridianNotFreeItem.begin(), nMeridianNotFreeItem.end(), item_id) != nMeridianNotFreeItem.end();
}
bool CECUIConfig::GameUI::IsRandomMap(int mapid) const
{
	return std::find(nRandomMaps.begin(),nRandomMaps.end(),mapid) != nRandomMaps.end();
}
int CECUIConfig::GameUI::GetRecommendShopItem(int type) const
{
	int id(0);
	if (type >= 0 && type < (int)nRecommendShopItems.size()){
		id = nRecommendShopItems[type];
	}
	return id;
}
bool CECUIConfig::GameUI::GetRandomMapItemInfo(int mapID,RandomMapItem& info)
{
	if (SpecialRandomMapItems.find(mapID) == SpecialRandomMapItems.end())
		 return false;

	info = SpecialRandomMapItems[mapID];

	return true;
}
// CECQShopConfig

CECQShopConfig & CECQShopConfig::Instance()
{
	static CECQShopConfig s_dummy;
	return s_dummy;
}

CECQShopConfig::CECQShopConfig()
{
}

bool CECQShopConfig::Load(const char *filePath)
{
	m_levelFilters.clear();
	m_idFilters.clear();

	AWIniFile ItemFile;
	
	if (ItemFile.Open(filePath))
	{
		//	Level Filter
		{
			const wchar_t *szSec = _AL("Filter");
			int keyNameIndex = 0;
			LevelFilterCondition condition;
			ACString strKey, strValue;
			while (true)
			{
				strKey.Format(_AL("config%d"), keyNameIndex++);
				strValue = ItemFile.GetValueAsString(szSec, strKey);
				if (strValue.IsEmpty()) break;

				bool bSuccess(false);
				while (true)
				{
					CECSplitHelperW splitter(strValue);
					if (splitter.Count() < 2) break;

					//	分析等级
					const ACString &strLevels = splitter.ToString(0);
					CECSplitHelperW levelSplitter(strLevels, _AL("-"));
					if (levelSplitter.Count() != 2) break;
					condition.minLevel = levelSplitter.ToInt(0);
					condition.maxLevel = levelSplitter.ToInt(1);
					if (condition.minLevel <= 0 || condition.maxLevel <= 0 ||
						condition.minLevel > condition.maxLevel)
						break;

					//	分析商品ID
					bool bBadID(false);
					for (int i = 1; i < splitter.Count(); ++ i)
					{
						int id = splitter.ToInt(i);
						if (id <= 0 || m_levelFilters.find(id) != m_levelFilters.end())
						{
							bBadID = true;
							break;
						}
						m_levelFilters[id] = condition;
					}
					if (bBadID) break;

					bSuccess = true;
					break;
				}
				if (!bSuccess)
				{
					a_LogOutput(1, "CECQShopConfig::Init, Invalid Filter %s", AC2AS(strValue));
					m_levelFilters.clear();
					return false;
				}
			}
		}

		//	Task Filter
		{
			const ACHAR *szSec = _AL("TaskLimitedItem");
			int i(0);
			ACString key;
			do {
				key.Format( _AL("ID%d"), i);
				int value = ItemFile.GetValueAsInt(szSec, key, 0);
				if (!value){
					break;
				}
				if (m_idFilters.find(value) == m_idFilters.end()){
					m_idFilters[value] = false;
				}
				++ i;
			} while(true);
		}

		//	Category
		{
			const ACHAR *szSec = _AL("Category");
			int i(0);
			ACString key;
			do {
				key.Format( _AL("c%d"), i);
				AWString strValue = ItemFile.GetValueAsString(szSec, key);
				if (strValue.IsEmpty()){
					break;
				}
				CECSplitHelperW splitter(strValue);
				if (splitter.Count() != 2){
					a_LogOutput(1, "CECQShopConfig::Init, Invalid Category as %s", AC2AS(strValue));
					break;
				}
				m_catMap[i] = CategoryPosition(splitter.ToInt(0), splitter.ToInt(1));
				++ i;
			} while(true);
		}

		ItemFile.Close();
	}
	else
	{
		a_LogOutput(1, "CECQShopConfig::Init, Failed to Open %s", filePath);
	}

	return true;
}

bool CECQShopConfig::IsFilteredByLevel(int id, int myLevel)const
{
	bool bFiltered(false);	
	if (CECUIConfig::Instance().GetGameUI().bEnableQShopFilter){
		LevelFilterMap::const_iterator cit = m_levelFilters.find(id);
		if (cit != m_levelFilters.end())
			bFiltered = !cit->second.Meet(myLevel);
	}
	return bFiltered;
}

bool CECQShopConfig::IsFilteredByID(int id)const
{
	//	此 ID 物品被过滤
	IDFilterMap::const_iterator cit = m_idFilters.find(id);
	return cit != m_idFilters.end() && cit->second;
}

bool CECQShopConfig::CanFilterID(int id)const
{
	//	ID 属于在过滤列表中
	IDFilterMap::const_iterator cit = m_idFilters.find(id);
	return cit != m_idFilters.end();
}

void CECQShopConfig::OnItemBuyed(int id)
{
	IDFilterMap::iterator it = m_idFilters.find(id);
	if (it != m_idFilters.end()){
		it->second = true;
	}
}

void CECQShopConfig::ClearBuyedItem()
{
	for (IDFilterMap::iterator it = m_idFilters.begin(); it != m_idFilters.end(); ++ it)
	{
		it->second = false;
	}
}

bool CECQShopConfig::FindCategory(int idCategory, int &mainType, int &subType)const
{
	bool bFound(false);
	CategoryMap::const_iterator cit = m_catMap.find(idCategory);
	if (cit != m_catMap.end()){
		const CategoryPosition & cat = cit->second;
		mainType = cat.mainType;
		subType = cat.subType;
		bFound = true;
	}
	return bFound;
}

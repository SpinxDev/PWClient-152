/*
 * FILE: EC_SendC2SCmds.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

void c2s_SendDebugCmd(int iCmd, int iNumParams, ...);

//	Assistant functions used to send C2S commands
void c2s_SendCmdPlayerMove(const A3DVECTOR3& vCurPos, const A3DVECTOR3& vDest, int iTime, float fSpeed, int iMoveMode, WORD wStamp);
void c2s_SendCmdStopMove(const A3DVECTOR3& vDest, float fSpeed, int iMoveMode, BYTE byDir, WORD wStamp, int iTime);
void c2s_SendCmdSelectTarget(int id);
void c2s_SendCmdNormalAttack(BYTE byPVPMask);
void c2s_SendCmdReviveVillage(int param=0);
void c2s_SendCmdReviveItem(int param=0);
void c2s_SendCmdGoto(float x, float y, float z);
void c2s_SendCmdGetExtProps();
void c2s_SendCmdGetPartExtProps(int iPropIndex);
void c2s_SendCmdSetStatusPts(int v, int e, int s, int a);
void c2s_SendCmdLogout(int iOutType);
void c2s_SendCmdGetAllData(bool bpd, bool bed, bool btd);
void c2s_SendCmdChargeEquipFlySword(int iEleIdx, int iCount);
void c2s_SendCmdChargeFlySword(int iEleIdx, int iFSIdx, int iCount, int idFlySword);
void c2s_SendCmdCancelAction();
void c2s_SendCmdSitDown();
void c2s_SendCmdStandUp();
void c2s_SendCmdEmoteAction(WORD wPose);
void c2s_SendCmdTaskNotify(const void* pData, DWORD dwDataSize);
void c2s_SendCmdUnselect();
void c2s_SendCmdContinueAction();
void c2s_SendCmdStopFall();
void c2s_SendCmdGatherMaterial(int idMatter, int iToolPack, int iToolIdx, int idTool, int idTask);
void c2s_SendCmdCastSkill(int idSkill, BYTE byPVPMask, int iNumTarget, int* aTargets);
void c2s_SendCmdCastInstantSkill(int idSkill, BYTE byPVPMask, int iNumTarget, int* aTargets);
void c2s_SendCmdCastPosSkill(int idSkill, const A3DVECTOR3& vPos, BYTE byPVPMask, int iNumTarget, int* aTargets);
void c2s_SendCmdTrickAction(int iAction);
void c2s_SendCmdSetAdvData(int iData1, int iData2);
void c2s_SendCmdClearAdvData();
void c2s_SendCmdQueryPlayerInfo1(int iNum, int* aIDs);
void c2s_SendCmdQueryNPCInfo1(int iNum, int* aIDs);
void c2s_SendCmdSessionEmote(int iAction);
void c2s_SendCmdConEmoteRequest(int iAction, int idTarget);
void c2s_SendCmdConEmoteReply(int iResult, int iAction, int idTarget);
void c2s_SendCmdDeadMove(float y, int iTime, float fSpeed, int iMoveMode);
void c2s_SendCmdDeadStopMove(float y, float fSpeed, BYTE byDir, int iMoveMode);
void c2s_SendCmdEnterSanctuary(int id);
void c2s_SendCmdOpenBooth(int iNumEntry, const void* szName, void* aEntries);
void c2s_SendCmdCloseBooth();
void c2s_SendCmdQueryBoothName(int iNum, int* aIDs);
void c2s_SendCmdCompleteTravel();
void c2s_SendCmdEnalbePVP();
void c2s_SendCmdDisablePVP();
void c2s_SendCmdOpenBoothTest();
void c2s_SendCmdFashionSwitch();
void c2s_SendCmdEnterInstance(int iTransIdx, int idInst);
void c2s_SendCmdRevivalAgree(int param = 0);
void c2s_SendCmdActiveRushFly(bool bActive);
void c2s_SendCmdQueryDoubleExp();
void c2s_SendCmdDuelRequest(int idTarget);
void c2s_SendCmdDuelReply(int idWho, int iReply);
void c2s_SendCmdBindPlayerRequest(int idTarget);
void c2s_SendCmdBindPlayerInvite(int idTarget);
void c2s_SendCmdBindPlayerRequestRe(int idWho, int iReply);
void c2s_SendCmdBindPlayerInviteRe(int idWho, int iReply);
void c2s_SendCmdCancelBindPlayer();
void c2s_SendCmdDebugDeliverCmd(WORD type, const char* szCmd);
void c2s_SendCmdDebugGSCmd(const char* szCmd);
void c2s_SendCmdMallShopping(int iCount, const void* pGoods);
void c2s_SendCmdDividendMallShopping(int iCount, const void* pGoods);
void c2s_SendCmdGetWallowInfo();
void c2s_SendCmdTeamDismissParty();
void c2s_SendCmdUseItemWithArg(unsigned char where, unsigned char count, unsigned short index, int item_id, const void* arg, size_t size);
void c2s_SendCmdQueryCashInfo();
void c2s_SendCmdGetMallItemPrice(int start_index, int end_index);
void c2s_SendCmdEquipFashionItem(int iBody, int iLeg, int iFoot, int iWrist, int iHead, int iWeapon);
void c2s_SendCmdNotifyForceAttack(int iForceAttack, BYTE refuseBless);
void c2s_SendCmdGetDividendMallItemPrice(int start_index, int end_index);
void c2s_SendCmdChooseMultiExp(int iIndex);
void c2s_SendCmdToggleMultiExp(bool isActivate);
void c2s_SendCmdMultiExchangeItem(int iPackage, int iCount, const int* aIndexPairs);
void c2s_SendCmdSysAuction(int type, const void* pDataBuf, int iDataSize);
void c2s_SendCmdQueryNetworkDelay(int timestamp);
void c2s_SendCmdGetFactionFortressInfo();
void c2s_SendCmdCongregateReply(int type, bool agree, int sponsor_id);
void c2s_SendCmdGetForceGlobalData();
void c2s_SendCmdPreviewProduceReply(bool bSelectOld);
void c2s_SendCmdQueryProfitTime();
void c2s_SendCmdEnterPKPrecinct();
void c2s_SendCmdCountryGetPersonalScore();
void c2s_SendCmdGetServerTime();
void c2s_SendCmdCountryLeaveWar();
void c2s_SendCmdGetCashMoneyRate();
void c2s_SendCmdExchangeYinpiao(bool is_sell, int count);
void c2s_SendCmdCountryGetStrongHoldState();
void c2s_SendCmdQueryTouchPoint();
void c2s_SendCmdSpendTouchPoint(unsigned int index, unsigned int lots, int id, unsigned int num, unsigned int price, int expire_time);
void c2s_SendCmdQueryTitle(int roleid);
void c2s_SendCmdChangeTitle(unsigned short titleid);
void c2s_SendCmdDaylySignin();
void c2s_SendCmdLateSignin(char type, int item_pos, int dest_time);
void c2s_SendCmdSignInAward(char type, int mon);
void c2s_SendCmdRefreshSignIn();
void c2s_SendCmdSwitchParallelWorld(int line);
void c2s_SendCmdQueryParallelWorld();
void c2s_SendCmdGetReincarnationTome();
void c2s_SendCmdRewriteReincarnationTome(unsigned int record_index, int record_level);
void c2s_SendCmdActivateReincarnationTome(char active);
void c2s_SendCmdSwallowGeneralCard(unsigned char equip_index, unsigned char is_inv, unsigned char swallowed_equip_index, unsigned int count);
void c2s_SendCmdEquipTrashBoxItem(unsigned char trashbox_index, unsigned char item_in_box_index, unsigned char equip_index);
void c2s_SendCmdCountryBattleLiveShow();
void c2s_SendCmdRandMallShopping(int config_id, int op);
void c2s_SendCmdQueryFactionPVPInfo(int faction_id);
void c2s_SendCmdReenterInstance(bool agree);

//	Team commands ...
void c2s_SendCmdTeamInvite(int idPlayer);
void c2s_SendCmdTeamAgreeInvite(int idLeader, int iTeamSeq);
void c2s_SendCmdTeamRejectInvite(int idLeader);
void c2s_SendCmdTeamLeaveParty();
void c2s_SendCmdTeamKickMember(int idMember);
void c2s_SendCmdTeamSetPickupFlag(short sFlag);
void c2s_SendCmdTeamMemberPos(int iNumMem, int* aMemIDs);
void c2s_SendCmdTeamAssistSel(int idTeamMember);
void c2s_SendCmdTeamAskJoin(int idTarget);
void c2s_SendCmdTeamReplyJoinAsk(int idAsker, bool bAgree);
void c2s_SendCmdTeamChangeLeader(int idLeader);
void c2s_SendCmdTeamNotifyOwnPos();

//	Inventory and equipment commands ...
void c2s_SendCmdGetIvtrData(int iPackage);
void c2s_SendCmdGetIvtrDetailData(int iPackage);
void c2s_SendCmdMoveIvtrItem(int iSrc, int iDest, int iAmount);
void c2s_SendCmdExgIvtrItem(int iIndex1, int iIndex2);
void c2s_SendCmdDropIvtrItem(int iIndex, int iAmount);
void c2s_SendCmdDropEquipItem(int iIndex);
void c2s_SendCmdExgEquipItem(int iIndex1, int iIndex2);
void c2s_SendCmdEquipItem(int iIvtrIdx, int iEquipIdx);
void c2s_SendCmdMoveItemToEquip(int iIvtrIdx, int iEquipIdx);
void c2s_SendCmdGetOtherEquip(int iNumID, int* aIDs);
void c2s_SendCmdGetWealth(bool bpd, bool bed, bool btd);
void c2s_SendCmdGetItemInfo(BYTE byPackage, BYTE bySlot);
void c2s_SendCmdPickup(int idItem, int tid);
void c2s_SendCmdUseItem(BYTE byPackage, BYTE bySlot, int tid, BYTE byCount);
void c2s_SendCmdUseItemWithTarget(BYTE byPackage, BYTE bySlot, int tid, BYTE byPVPMask);
void c2s_SendCmdThrowMoney(DWORD dwAmount);
void c2s_SendCmdGetItemInfoList(int iPackage, int iCount, BYTE* aIndices);
void c2s_SendCmdGetTrashBoxData(bool bDetail, int iAccountBox);
void c2s_SendCmdExgTrashBoxItem(int where, int iIndex1, int iIndex2);
void c2s_SendCmdMoveTrashBoxItem(int where, int iSrc, int iDst, int iAmount);
void c2s_SendCmdExgTrashBoxIvtrItem(int where, int iTrashIdx, int iIvtrIdx);
void c2s_SendCmdMoveTrashBoxToIvtr(int where, int iTrashIdx, int iIvtrIdx, int iAmount);
void c2s_SendCmdMoveIvtrToTrashBox(int where, int iTrashIdx, int iIvtrIdx, int iAmount);
void c2s_SendCmdExgTrashBoxMoney(int iTrashMoney, int iIvtrMoney, int iAccountBox);
void c2s_SendCmdGetOtherEquipDetail(int idTarget);
void c2s_SendCmdOpenFashionTrash(const char* szPsw);

//	Pet commands ...
void c2s_SendCmdPetSummon(int iPetIdx);
void c2s_SendCmdPetRecall();
void c2s_SendCmdPetBanish(int iPetIdx);
void c2s_SendCmdPetCtrl(int idTarget, int cmd, void* pParamBuf, int iParamLen);
void c2s_SendCmdPetEvolution(int iPetIdx, int iFormulaIdx);
void c2s_SendCmdPetAddExp(int iPetIdx, int iItemNum);
void c2s_SendCmdPetRebuildNature(int iPetIdx,int iFormula);
void c2s_SendCmdPetRebuildInheritRatio(int iPetIdx,int iFormula);
void c2s_SendCmdPetRebuildInheritChoose(bool bAccpet);
void c2s_SendCmdPetRebuildNatureChoose(bool bAccpet);

//  Goblin comomands...
void c2s_SendCmdGoblinAddAttribute(int str, int agi, int vit, int eng);
void c2s_SendCmdGoblinAddGenius(int g1, int g2, int g3, int g4, int g5);
void c2s_SendCmdGoblinPlayerInsertExp(unsigned int exp, int use_sp);
void c2s_SendCmdGoblinEquipItem(int index_inv);
void c2s_SendCmdGoblinChangeSecureStatus(unsigned char status) ;
void c2s_SendCmdGoblinCastSkill(int idSkill, unsigned char force_attack, int iNumTarget, int* aTargets);
void c2s_SendCmdGoblinChargeEquip(int iEleIdx, int iCount);
void c2s_SendCmdGoblinCharge(int iEleIdx, int iGoblinIdx, int iCount, int idGoblin); 
void c2s_SendCmdGoblinRefineActive(int item_id);

// online award
void c2s_SendCmdRechargeOnlineAward(int type,int item_count,C2S::cmd_recharge_online_award::entry* pEntry);
void c2s_SendCmdToggleOnlineAward(int type, int active);

// unique data
void c2s_SendCmdQueryQuniqueData(const abase::vector<int>& keyvec);

// auto team
void c2s_SendCmdAutoTeamSetGoal(int type, int goal_id, int op);
void c2s_SendCmdAutoTeamJumpToGoal(int goal_id);

//	NPC service commands ...
void c2s_SendCmdNPCSevHello(int nid);
void c2s_SendCmdNPCSevBuy(int iItemNum, C2S::npc_trade_item* aItems);
void c2s_SendCmdNPCSevSell(int iItemNum, C2S::npc_sell_item* aItems);
void c2s_SendCmdNPCSevRepair(BYTE byPackage, BYTE bySlot, int idItem);
void c2s_SendCmdNPCSevRepairAll();
void c2s_SendCmdNPCSevHeal();
void c2s_SendCmdNPCSevTransmit(int iIndex);
void c2s_SendCmdNPCSevEmbed(WORD wStoneIdx, WORD wEquipIdx, int tidStone, int tidEquip);
void c2s_SendCmdNPCSevClearEmbeddedChip(int iEquipIdx, int tidEquip);
void c2s_SendCmdNPCSevLearnSkill(int idSkill);
void c2s_SendCmdNPCSevMakeItem(int idSkill, int idItem, DWORD dwCount);
void c2s_SendCmdNPCSevBreakItem(int idSkill, int iIvtrIdx, int idItem);
void c2s_SendCmdNPCSevAcceptTask(int idTask,int idStorage,int idRefreshItem);
void c2s_SendCmdNPCSevReturnTask(int idTask, int iChoice);
void c2s_SendCmdNPCSevTaskMatter(int idTask);
void c2s_SendCmdNPCSevChgTrashPsw(const char* szOldPsw, const char* szNewPsw);
void c2s_SendCmdNPCSevOpenTrash(const char* szPsw);
void c2s_SendCmdNPCSevViewTrash();
void c2s_SendCmdNPCSevIdentify(int iSlot, int tidItem);
void c2s_SendCmdNPCSevFaction(int iAction, void* pDataBuf, int iDataSize);
void c2s_SendCmdNPCSevTravel(int iIndex, int idLine);
void c2s_SendCmdNPCSevGetContent(int idSevice);
void c2s_SendCmdNPCSevBoothBuy(int idBooth, int iItemNum, C2S::npc_booth_item* aItems, int iYinpiao = 0);
void c2s_SendCmdNPCSevBoothSell(int idBooth, int iItemNum, C2S::npc_booth_item* aItems);
void c2s_SendCmdNPCSevWaypoint();
void c2s_SendCmdNPCSevForgetSkill(int idSkill);
void c2s_SendCmdNPCSevFaceChange(int iIvtrIdx, int idItem);
void c2s_SendCmdNPCSevMail(int iAction, void* pDataBuf, int iDataSize);
void c2s_SendCmdNPCSevVendue(int iAction, void* pDataBuf, int iDataSize);
void c2s_SendCmdNPCSevDblExpTime(int iIndex);
void c2s_SendCmdNPCSevHatchPet(int iIvtrIdx, int idEgg);
void c2s_SendCmdNPCSevRestorePet(int iPetIdx);
void c2s_SendCmdNPCSevBattle(int iAction, void* pDataBuf, int iDataSize);
void c2s_SendCmdNPCSevBuildTower(int iTowerIdx, int idItem);
void c2s_SendCmdNPCSevLeaveBattle();
void c2s_SendCmdNPCSevReturnStatusPt(int iIndex, int idItem);
void c2s_SendCmdNPCSevAccountPoint(int iAction, void* pDataBuf, int iDataSize);
void c2s_SendCmdNPCSevRefine(int iIndex, int idItem, int iComponentIndex);
void c2s_SendCmdNPCSevPetName(int iPetIdx, void* szNameBuf, int iBufLen);
void c2s_SendCmdNPCSevPetSkillDel(int idSkill);
void c2s_SendCmdNPCSevPetSkillLearn(int idSkill);
void c2s_SendCmdNPCSevBindItem(int iIndex, int iItemId, int iItemNeed);
void c2s_SendCmdNPCSevDestroyBind(int iIndex, int iItemId);
void c2s_SendCmdNPCSevCancelDestroy(int iIndex, int iItemId);
void c2s_SendCmdNPCSevStockTransaction(int nWithdraw, int nCash, int nMoney);
void c2s_SendCmdNPCSevStockOperation(int nType, void* pData, int nLen);
void c2s_SendCmdNPCSevDye(int inv_index, int item_type, int dye_index, int dye_type);
void c2s_SendCmdNPCSevRefineTrans(int iSrcIdx, int idSrc, int iDstIdx, int idDst);
void c2s_SendCmdNPCSevCompose(int idSkill, int idRecipe, int aMaterialIDs[16], int aMaterialIdx[16]);
void c2s_SendCmdNPCSevMakeSlot(int src_index, int src_id);
void c2s_SendCmdNPCSevMark(int inv_index, int item_type, int dye_index, int dye_type, void* szNameBuf, int iBufLen);
void c2s_SendCmdNPCSevSplitFashion(int src_index, int src_id);

void c2s_SendCmdNPCSevGoblinReturnStatusPt(int iIndex, int idItem, int str, int agi, int vit, int eng);
void c2s_SendCmdNPCSevGoblinReturnGeniusPt(int iIndex, int idItem);
void c2s_SendCmdNPCSevGoblinLearnSkill(int iIndex, int idSkill);
void c2s_SendCmdNPCSevGoblinForgetSkill(int iIndex, int idSkill,  int forget_level);
void c2s_SendCmdNPCSevGoblinRefine(int iIndex, int idItem, int iTicketCnt = 1);
void c2s_SendCmdNPCSevGoblinTransferRefine(int iIndexSrc, int iIndexDest);
void c2s_SendCmdNPCSevGoblinDestroy(int iIndex); 
void c2s_SendCmdNPCSevGoblinEquipDestroy(int iIndex, int iSlot, int iEquip_index);
void c2s_SendCmdNPCSevDyeBySuit(int iBody, int iLeg, int iFoot, int iWrist, int iDye);
void c2s_SendCmdNPCRepairDestroyingItem(int iIndex);
void c2s_SendCmdNPCSevLevelUpProduce(int idSkill, int idRecipe, int aMaterialIDs[16], int aMaterialIdx[16], int iEquipId, int iEquipInvId, int iInheritType, bool bPreview, bool bAdvanced);
void c2s_SendCmdEquipmentPropData(int iEquipId, int iEquipInvId);
void c2s_SendCmdNPCSevOpenAccountBox();
void c2s_SendCmdNPCSevGodEvilConvert(int iTypeIndex);
void c2s_SendCmdNPCSevWebTrade(int iAction, void* pDataBuf, int iDataSize);
void c2s_SendCmdNPCSevWeddingBook(int start_time, int end_time, int scene, int bookcard_index, int type);
void c2s_SendCmdNPCSevWeddingInvite(int invitecard_index, int invitee);
void c2s_SendCmdNPCSevFactionFortressService1(int iAction, void* pDataBuf, int iDataSize);
void c2s_SendCmdNPCSevFactionFortressService2(int iAction, int param0, int param1, int param2);
void c2s_SendCmdNPCSevFactionFortressService3(int param0, int param1, int param2);
void c2s_SendCmdNPCSevPetDye(int pet_index, int pet_id, int dye_index, int dye_id);
void c2s_SendCmdNPCSevEngrave(int engrave_id, int ivtr_index);
void c2s_SendCmdNPCSevGetDPSDPHRank(int rank_mask);
void c2s_SendCmdNPCSevAddonRegen(int recipe_id, int ivtr_index);
void c2s_SendCmdNPCSevForce(int type, int param);
void c2s_SendCmdNPCSevTransmitDirect(int iNum, int *aWayPoints);
void c2s_SendCmdNPCSevCountryJoinLeave(bool bJoin);
void c2s_SendCmdNPCSevCountryLeaveWar();
void c2s_SendCmdNPCSevCrossServerGetIn();
void c2s_SendCmdNPCSevCrossServerGetOut();
void c2s_SendCmdNPCSevPlayerRename(int item_index,int item_id,unsigned short nameLen,const char*pNewName);
void c2s_SendCmdNPCSevPlayerChangeGender(int item_index,int item_id,unsigned char new_gender,void *customize_data,unsigned short customize_data_len);

void c2s_SendCmdNPCSevStoneTransfer(int equip_idx, int stone_idx, int oldstone_tid, int newstone_tid, int recipe_tid,int aMaterialIDs[4], int aMaterialIdx[4]);
void c2s_SendCmdNPCSevStoneReplace(int equip_idx, int stone_idx, int oldstone_tid, int newstone_idx, int newstone_tid);

void c2s_SendCmdNPCSevKing(int iAction, void* pDataBuf, int iDataSize);

void c2s_SendCmdNPCSevOfflineShop(int iAction, void* pDataBuf, int iDataSize); // 寄卖服务, 离线商店
void c2s_SendCmdNPCSevGiftCard(const char* card_num);
void c2s_SendCmdNPCSevReincarantion();
void c2s_SendCmdNPCSevTrickBattleRequest(int iActor);

void c2s_SendCmdNPCSevRespawn(int majorIdx, int minorIdx); // 卡牌转生

void c2s_SendCmdNPCSevFlySwordImprove(unsigned int idx,int flysword_id); // 飞剑强化
void c2s_SendCmdNPCSevOpenFactionPVP();

void c2s_SendCmdNPCSevMallShopping(int iCount, const void* pGoods);
void c2s_SendCmdNPCSevDividendMallShopping(int iCount, const void* pGoods);

//	GM commands ...
void c2s_SendCmdGMMoveToPlayer(int idPlayer);
void c2s_SendCmdGMCallInPlayer(int idPlayer);
void c2s_SendCmdGMGenerate(int tid);
void c2s_SendCmdGMActiveSpawner(bool bActive, int id_spawner);
void c2s_SendCmdGMGenerateMob(int idMob, int idVis, short sCount, short sLifeTime, const ACHAR* szName);
void c2s_SendCmdGMTriggerInvisible();
void c2s_SendCmdGMTriggerInvincible();
void c2s_SendCmdGMQuerySpecItem(int role_id, int item_id);
void c2s_SendCmdGMRemoveSpecItem(int role_id, int item_id, unsigned char where, unsigned char index, unsigned int count);
void c2s_SendCmdGMOpenActivity(int activity_id,bool bOpen);
void c2s_SendCmdGMChangeDS(bool bToCrossServer);

// Meridians commands
void c2s_SendCmdMerdiansImpact(int index);

// 商城赠送索取
void c2s_SendCmdGivePresent(int roleid, int mail_id, int goods_id, int goods_index, int goods_slot);
void c2s_SendCmdAskForPresent(int roleid, int goods_id, int goods_index, int goods_slot);

// 激活传送点
void c2s_SendCmdActiveTransPoint(int count, int* pData);

// 战车
void c2s_SendCmdUpdateChariot(int id);
void c2s_SendCmdLeaveChariot();
void c2s_SendCmdQueryChariotsAmount();
///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



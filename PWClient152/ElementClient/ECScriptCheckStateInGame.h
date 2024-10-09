/********************************************************************
	created:	2005/09/01
	created:	1:9:2005   19:29
	file name:	ECScriptCheckStateInGame.h
	author:		yaojun
	
	purpose:	implement of IECScriptCheckState for game
*********************************************************************/

#pragma once

#include "ECScriptCheckState.h"
#include "SingletonHolder.h"

class CECGame;
class CECGameUIMan;
class CECWorld;
class CECHostPlayer;

class CECScriptCheckStateInGame : public IECScriptCheckState  
{
private:
	// 0--false 1--true 2--nocheck
	char m_chKillMonster;
	char m_chEquipNewWeapon;
	char m_chDisplayNext;
	char m_chAdjustOrient;
	char m_chAdjustDistance;
	char m_chPlayTrick;
	char m_chInventoryItemMoved;
	char m_chPQTriggered;
	char m_chAccountBoxTriggered;

	// timer param
	DWORD m_dwStartTime;
	DWORD m_dwDuration;

	// framerate list
	std::deque<unsigned short> m_frameRateList;
private:
	CECGame * GetGame() const;
	CECWorld * GetWorld() const;
	CECGameUIMan * GetGameUIMan() const;
	CECHostPlayer * GetHostPlayer() const;
public:
	CECScriptCheckStateInGame();
	virtual ~CECScriptCheckStateInGame();
	
	//////////////////////////////////////////////////////////////////////////
	// 主动状态　
	// 由Getxxx()主动获取状态
	// Setxx()应该为空
	//////////////////////////////////////////////////////////////////////////

	virtual bool GetScriptExecuted(int nScriptID) const;
	virtual void SetScriptExecuted(int nScriptID, bool bExecuted);

	virtual AWString GetStringFromGameUIST(int nID) const;
	virtual void SetStringFromGameUIST(int nID, AWString strString);
	
	virtual int GetPlayerLevel() const;
	virtual void SetPlayerLevel(int nLevel);
	
	virtual int GetPlayerProfessionID() const;
	virtual void SetPlayerProfessionID(int nProfessionID);
	
	virtual int GetCurrentHP() const;
	virtual void SetCurrentHP(int nCurHP);
	virtual int GetMaxHP() const;
	virtual void SetMaxHP(int nMaxHP);
	
	virtual int GetCurrentMP() const;
	virtual void SetCurrentMP(int nCurMP);
	virtual int GetMaxMP() const;
	virtual void SetMaxMP(int nMaxMP);
	
	virtual bool GetDialogVisibleState(AString strDlgName) const;
	virtual void SetDialogVisibleState(AString strDlgName, bool bVisible);
	
	virtual AWString GetDialogItemText(AString strDlgName, AString strItemName) const;
	virtual void SetDialogItemText(AString strDlgName, AString strItemName, AWString strItemText);
	
	virtual bool GetDialogItemIsChecked(AString strDlgName, AString strItemName) const;
	virtual void SetDialogItemIsChecked(AString strDlgName, AString strItemName, bool bCheck);

	virtual bool GetPlayerStatusIsDead() const;
	virtual void SetPlayerStatusIsDead(bool bDead);
	virtual bool GetPlayerStatusIsFlying() const;
	virtual void SetPlayerStatusIsFlying(bool bFlying);
	virtual bool GetPlayerStatusIsSitting() const;
	virtual void SetPlayerStatusIsSitting(bool bSitting);
	virtual bool GetPlayerStatusIsInVader() const;
	virtual void SetPlayerStatusIsInVader(bool bInVader);
	virtual bool GetPlayerStatusIsPariah() const;
	virtual void SetPlayerStatusIsPariah(bool bPariah);
	virtual bool GetPlayerStatusIsDiving() const;
	virtual void SetPlayerStatusIsDiving(bool bDiving);
	virtual bool GetPlayerStatusIsPVPOpen() const;
	virtual void SetPlayerStatusIsPVPOpen(bool bPVPOpen);
	virtual bool GetPlayerStatusIsInFashionMode() const;
	virtual void SetPlayerStatusIsInFashionMode(bool bInFashionMode);
	virtual bool GetPlayerStatusIsFighting() const;
	virtual void SetPlayerStatusIsFighting(bool bFighting);
	virtual bool GetPlayerStatusIsTrading() const;
	virtual void SetPlayerStatusIsTrading(bool bTrading);
	virtual bool GetPlayerStatusIsUsingTrashBox() const;
	virtual void SetPlayerStatusIsUsingTrashBox(bool bUsingTrashBox);
	virtual bool GetPlayerStatusIsRooting() const;
	virtual void SetPlayerStatusIsRooting(bool bRooting);
	virtual bool GetPlayerStatusCannotAttack() const;
	virtual void SetPlayerStatusCannotAttack(bool bCannotAttack);
	virtual bool GetPlayerStatusIsMeleeing() const;
	virtual void SetPlayerStatusIsMeleeing(bool bMeleeing);
	virtual bool GetPlayerStatusIsTalkingWithNPC() const;
	virtual void SetPlayerStatusIsTalkingWithNPC(bool bTalkingWithNPC);
	virtual bool GetPlayerStatusIsSpellingMagic() const;
	virtual void SetPlayerStatusIsSpellingMagic(bool bSpellingMagic);
	virtual bool GetPlayerStatusIsJumping() const;
	virtual void SetPlayerStatusIsJumping(bool bJumping);
	virtual bool GetPlayerStatusIsPicking() const;
	virtual void SetPlayerStatusIsPicking(bool bPicking);
	virtual bool GetPlayerStatusIsGathering() const;
	virtual void SetPlayerStatusIsGathering(bool bGathering);
	virtual bool GetPlayerStatusIsReviving() const;
	virtual void SetPlayerStatusIsReviving(bool bReviving);
	virtual bool GetPlayerStatusIsUsingItem() const;
	virtual void SetPlayerStatusIsUsingItem(bool bUsingItem);
	virtual bool GetPlayerStatusIsChangingFace() const;
	virtual void SetPlayerStatusIsChangingFace(bool bChangingFace);
	virtual bool GetPlayerStatusIsDoingSessionPose() const;
	virtual void SetPlayerStatusIsDoingSessionPose(bool bDoingSessionPose);
	virtual bool GetPlayerStatusTrashBoxHasPsw() const;
	virtual void SetPlayerStatusTrashBoxHasPsw(bool bTrashBoxHasPsw);
	virtual bool GetPlayerStatusIsMoving() const;
	virtual void SetPlayerStatusIsMoving(bool bIsPlayerMoving);
	virtual bool GetPlayerStatusIsUnderWater() const;
	virtual void SetPlayerStatusIsUnderWater(bool bIsUnderWater);

	virtual bool GetPlayerStatusIsOpeningBooth() const;
	virtual void SetPlayerStatusIsOpeningBooth(bool bIsOpeningBooth);
	
	virtual int GetServiceNPCID() const;
	virtual void SetServiceNPCID(int nID);

	virtual int GetSelectedTargetID() const;
	virtual void SetSelectedTargetID(int nID);
	
	virtual bool GetIsNPCID(int nNPCID) const;
	virtual void SetIsNPCID(int nNPCID, bool bIsNPCID);
	virtual bool GetIsNPCExists(int nNPCID) const;
	virtual void SetIsNPCExists(int nNPCID, bool bExists);
	virtual bool GetIsMonsterNPC(int nNPCID) const;
	virtual void SetIsMonsterNPC(int nNPCID, bool bIsMonsterNPC);
	virtual AWString GetNPCName(int nNPCID) const;
	virtual void SetNPCName(int nNPCID, AWString strName);
	virtual int GetNPCRandomProp(int nNPCID) const;
	virtual void SetNPCRandomProp(int nNPCID, int nPropID);
	
	virtual bool GetIsPlayerID(int nPlayerID) const;
	virtual void SetIsPlayerID(int nPlayerID, bool bIsPlayerID);
	virtual bool GetIsPlayerExists(int nPlayerID) const;
	virtual void SetIsPlayerExists(int nPlayerID, bool bExists);
	virtual AWString GetPlayerName(int nPlayerID) const;
	virtual void SetPlayerName(int nPlayerID, AWString strName);
	
	virtual bool GetIsCurrentPrecinctExists() const;
	virtual void SetIsCurrentPrecinctExists(bool bExists);
	virtual AWString GetCurrentPrecinctName() const;
	virtual void SetCurrentPrecinctName(AWString strName);
	
	virtual bool GetIsTaskFinished(int nTaskID, const CIntArray & vecExcludedID) const;
	virtual void SetIsTaskFinished(int nTaskID, bool bFinished);
	virtual bool GetIsTaskTaked(int nTaskID, const CIntArray & vecExcludedID) const;
	virtual void SetIsTaskTaked(int nTaskID, bool bTaked);
	virtual bool GetCanFinishTask(int nTaskID, const CIntArray & vecExcludedID) const;
	virtual void SetCanFinishTask(int nTaskID, bool bCanFinish);
	
	virtual int GetTaskInventoryItemCount(int nItemID) const;
	virtual void SetTaskInventoryItemCount(int nItemID, int nCount);
	virtual int GetInventoryItemCount(int nItemID) const;
	virtual void SetInventoryItemCount(int nItemID, int nCount);
	
	virtual int GetSkillLevel(int nSkillID) const;
	virtual void SetSkillLevel(int nSkillID, int nLevel);
	
	virtual int GetSkillAbility(int nSkillID) const;
	virtual void SetSkillAblility(int nSkillID, int nAblility);

	virtual bool GetAutoMoving() const;
	virtual void SetAutoMoving(bool bAutoMove);

	virtual bool GetHavePet() const;
	virtual void SetHavePet(bool bHavePet);

	virtual int GetReputation() const;
	virtual void SetReputation(int iReputation);

	virtual unsigned char GetReincarnationCount() const;
	virtual void SetReincarnationCount(unsigned char ucReincarnationCount);

	virtual bool GetHaveTitle() const;
	virtual void SetHaveTitle(bool bHaveTitle);

	virtual bool GetHaveActiveScript() const;
	virtual void SetHaveActiveScript(bool bHaveActiveScript);

	//////////////////////////////////////////////////////////////////////////
	// 被动状态　
	// 由外部模块调用Setxxx()设置状态，这个状态将保存在成员中，
	// Getxxx()从成员中获取状态值
	//////////////////////////////////////////////////////////////////////////

	virtual bool GetDisplayNext();
	virtual void SetDisplayNext(bool bDisplayNext);
	
	virtual bool GetKillMonster();
	virtual void SetKillMonster(bool bKillMonster);

	virtual bool GetEquipNewWeapon();
	virtual void SetEquipNewWeapon(bool bEquipNewWeapon);

	virtual bool GetPlayerStatusIsAdjustOrient();
	virtual void SetPlayerStatusIsAdjustOrient(bool bAdjustOrient);

	virtual bool GetPlayerStatusIsAdjustDistance();
	virtual void SetPlayerStatusIsAdjustDistance(bool bAdjustDistance);

	virtual bool GetPlayerStatusIsPlayTrick();
	virtual void SetPlayerStatusIsPlayTrick(bool bPlayTrick);
	
	virtual bool GetInventoryItemMoved();
	virtual void SetInventoryItemMoved(bool bInventoryItemMoved);

	virtual void GetScriptTimerParam(DWORD &dwStartTime, DWORD &dwDuration);
	virtual void SetScriptTimerParam(DWORD dwStartTime, DWORD dwDuration);	
	
	virtual bool GetPQIsTriggered();
	virtual void SetPQIsTriggered(bool rhs);

	virtual bool GetAccountBoxIsTriggered();
	virtual void SetAccountBoxIsTriggered(bool rhs);
	
	virtual int GetMapID() const;
	virtual void SetMapID(int nMapID);

	virtual const std::deque<unsigned short>& GetFrameRate() const;
	virtual void SetFrameRate(unsigned short frameRate);
};




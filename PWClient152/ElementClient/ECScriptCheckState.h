/********************************************************************
	created:	2005/09/01
	created:	1:9:2005   16:46
	file name:	ECScriptCheckState.h
	author:		yaojun
	
	purpose:	IECScriptCheckState define a interface to get/set
				a state. CECScriptCheckBase and its derived class
				use this interface to fetch a state.
*********************************************************************/

#pragma once

#include <AString.h>
#include <AWString.h>
#include <vector.h>
#include <deque>

typedef abase::vector<int> CIntArray;
typedef unsigned long DWORD;

class IECScriptCheckState
{
public:
	virtual ~IECScriptCheckState() {}

	virtual bool GetScriptExecuted(int nScriptID) const = 0;
	virtual void SetScriptExecuted(int nScriptID, bool bExecuted) = 0;

	virtual AWString GetStringFromGameUIST(int nID) const = 0;
	virtual void SetStringFromGameUIST(int nID, AWString strString) = 0;

	virtual int GetPlayerLevel() const = 0;
	virtual void SetPlayerLevel(int nLevel) = 0;
	
	virtual int GetPlayerProfessionID() const = 0;
	virtual void SetPlayerProfessionID(int nProfessionID) = 0;

	virtual int GetCurrentHP() const = 0;
	virtual void SetCurrentHP(int nCurHP) = 0;
	virtual int GetMaxHP() const = 0;
	virtual void SetMaxHP(int nMaxHP) = 0;

	virtual int GetCurrentMP() const = 0;
	virtual void SetCurrentMP(int nCurMP) = 0;
	virtual int GetMaxMP() const = 0;
	virtual void SetMaxMP(int nMaxMP) = 0;
	
	virtual bool GetDialogVisibleState(AString strDlgName) const = 0;
	virtual void SetDialogVisibleState(AString strDlgName, bool bVisible) = 0;
	
	virtual AWString GetDialogItemText(AString strDlgName, AString strItemName) const = 0;
	virtual void SetDialogItemText(AString strDlgName, AString strItemName, AWString strItemText) = 0;

	virtual bool GetDialogItemIsChecked(AString strDlgName, AString strItemName) const = 0;
	virtual void SetDialogItemIsChecked(AString strDlgName, AString strItemName, bool bCheck) = 0;

	virtual bool GetPlayerStatusIsDead() const = 0;
	virtual void SetPlayerStatusIsDead(bool bDead) = 0;
	virtual bool GetPlayerStatusIsFlying() const = 0;
	virtual void SetPlayerStatusIsFlying(bool bFlying) = 0;
	virtual bool GetPlayerStatusIsSitting() const = 0;
	virtual void SetPlayerStatusIsSitting(bool bSitting) = 0;
	virtual bool GetPlayerStatusIsInVader() const = 0;
	virtual void SetPlayerStatusIsInVader(bool bInVader) = 0;
	virtual bool GetPlayerStatusIsPariah() const = 0;
	virtual void SetPlayerStatusIsPariah(bool bPariah) = 0;
	virtual bool GetPlayerStatusIsDiving() const = 0;
	virtual void SetPlayerStatusIsDiving(bool bDiving) = 0;
	virtual bool GetPlayerStatusIsPVPOpen() const = 0;
	virtual void SetPlayerStatusIsPVPOpen(bool bPVPOpen) = 0;
	virtual bool GetPlayerStatusIsInFashionMode() const = 0;
	virtual void SetPlayerStatusIsInFashionMode(bool bInFashionMode) = 0;
	virtual bool GetPlayerStatusIsFighting() const = 0;
	virtual void SetPlayerStatusIsFighting(bool bFighting) = 0;
	virtual bool GetPlayerStatusIsTrading() const = 0;
	virtual void SetPlayerStatusIsTrading(bool bTrading) = 0;
	virtual bool GetPlayerStatusIsUsingTrashBox() const = 0;
	virtual void SetPlayerStatusIsUsingTrashBox(bool bUsingTrashBox) = 0;
	virtual bool GetPlayerStatusIsRooting() const = 0;
	virtual void SetPlayerStatusIsRooting(bool bRooting) = 0;
	virtual bool GetPlayerStatusCannotAttack() const = 0;
	virtual void SetPlayerStatusCannotAttack(bool bCannotAttack) = 0;
	virtual bool GetPlayerStatusIsMeleeing() const = 0;
	virtual void SetPlayerStatusIsMeleeing(bool bMeleeing) = 0;
	virtual bool GetPlayerStatusIsTalkingWithNPC() const = 0;
	virtual void SetPlayerStatusIsTalkingWithNPC(bool bTalkingWithNPC) = 0;
	virtual bool GetPlayerStatusIsSpellingMagic() const = 0;
	virtual void SetPlayerStatusIsSpellingMagic(bool bSpellingMagic) = 0;
	virtual bool GetPlayerStatusIsJumping() const = 0;
	virtual void SetPlayerStatusIsJumping(bool bJumping) = 0;
	virtual bool GetPlayerStatusIsPicking() const = 0;
	virtual void SetPlayerStatusIsPicking(bool bPicking) = 0;
	virtual bool GetPlayerStatusIsGathering() const = 0;
	virtual void SetPlayerStatusIsGathering(bool bGathering) = 0;
	virtual bool GetPlayerStatusIsReviving() const = 0;
	virtual void SetPlayerStatusIsReviving(bool bReviving) = 0;
	virtual bool GetPlayerStatusIsUsingItem() const = 0;
	virtual void SetPlayerStatusIsUsingItem(bool bUsingItem) = 0;
	virtual bool GetPlayerStatusIsChangingFace() const = 0;
	virtual void SetPlayerStatusIsChangingFace(bool bChangingFace) = 0;
	virtual bool GetPlayerStatusIsDoingSessionPose() const = 0;
	virtual void SetPlayerStatusIsDoingSessionPose(bool bDoingSessionPose) = 0;
	virtual bool GetPlayerStatusTrashBoxHasPsw() const = 0;
	virtual void SetPlayerStatusTrashBoxHasPsw(bool bTrashBoxHasPsw) = 0;
	virtual bool GetPlayerStatusIsMoving() const = 0;
	virtual void SetPlayerStatusIsMoving(bool bIsPlayerMoving) = 0;
	virtual bool GetPlayerStatusIsUnderWater() const = 0;
	virtual void SetPlayerStatusIsUnderWater(bool bIsUnderWater) = 0;
	
	virtual bool GetPlayerStatusIsOpeningBooth() const = 0;
	virtual void SetPlayerStatusIsOpeningBooth(bool bIsOpeningBooth) = 0;

	virtual int GetServiceNPCID() const = 0;
	virtual void SetServiceNPCID(int nID) = 0;

	virtual int GetSelectedTargetID() const = 0;
	virtual void SetSelectedTargetID(int nID) = 0;

	virtual bool GetIsNPCID(int nNPCID) const = 0;
	virtual void SetIsNPCID(int nNPCID, bool bIsNPCID) = 0;
	virtual bool GetIsNPCExists(int nNPCID) const = 0;
	virtual void SetIsNPCExists(int nNPCID, bool bExists) = 0;
	virtual bool GetIsMonsterNPC(int nNPCID) const = 0;
	virtual void SetIsMonsterNPC(int nNPCID, bool bIsMonsterNPC) = 0;
	virtual AWString GetNPCName(int nNPCID) const = 0;
	virtual void SetNPCName(int nNPCID, AWString strName) = 0;
	virtual int GetNPCRandomProp(int nNPCID) const = 0;
	virtual void SetNPCRandomProp(int nNPCID, int nPropID) = 0;

	virtual bool GetIsPlayerID(int nPlayerID) const = 0;
	virtual void SetIsPlayerID(int nPlayerID, bool bIsPlayerID) = 0;
	virtual bool GetIsPlayerExists(int nPlayerID) const = 0;
	virtual void SetIsPlayerExists(int nPlayerID, bool bExists) = 0;
	virtual AWString GetPlayerName(int nPlayerID) const = 0;
	virtual void SetPlayerName(int nPlayerID, AWString strName) = 0;

	virtual bool GetIsCurrentPrecinctExists() const = 0;
	virtual void SetIsCurrentPrecinctExists(bool bExists) = 0;
	virtual AWString GetCurrentPrecinctName() const = 0;
	virtual void SetCurrentPrecinctName(AWString strName) = 0;
	
	virtual bool GetIsTaskFinished(int nTaskID, const CIntArray & vecExcludedID) const = 0;
	virtual void SetIsTaskFinished(int nTaskID, bool bFinished) = 0;
	virtual bool GetIsTaskTaked(int nTaskID, const CIntArray & vecExcludedID) const = 0;
	virtual void SetIsTaskTaked(int nTaskID, bool bTaked) = 0;
	virtual bool GetCanFinishTask(int nTaskID, const CIntArray & vecExcludedID) const = 0;
	virtual void SetCanFinishTask(int nTaskID, bool bCanFinish) = 0;
	
	virtual int GetTaskInventoryItemCount(int nItemID) const = 0;
	virtual void SetTaskInventoryItemCount(int nItemID, int nCount) = 0;
	virtual int GetInventoryItemCount(int nItemID) const = 0;
	virtual void SetInventoryItemCount(int nItemID, int nCount) = 0;

	virtual int GetSkillLevel(int nSkillID) const = 0;
	virtual void SetSkillLevel(int nSkillID, int nLevel) = 0;
	
	virtual int GetSkillAbility(int nSkillID) const = 0;
	virtual void SetSkillAblility(int nSkillID, int nAblility) = 0;

	virtual bool GetDisplayNext() = 0;
	virtual void SetDisplayNext(bool bDisplayNext) = 0;

	virtual bool GetKillMonster() = 0;
	virtual void SetKillMonster(bool bKillMonster) = 0;

	virtual bool GetEquipNewWeapon() = 0;
	virtual void SetEquipNewWeapon(bool bEquipNewWeapon) = 0;

	virtual bool GetPlayerStatusIsAdjustOrient() = 0;
	virtual void SetPlayerStatusIsAdjustOrient(bool bAdjustOrient) = 0;

	virtual bool GetPlayerStatusIsAdjustDistance() = 0;
	virtual void SetPlayerStatusIsAdjustDistance(bool bAdjustDistance) = 0;

	virtual bool GetPlayerStatusIsPlayTrick() = 0;
	virtual void SetPlayerStatusIsPlayTrick(bool bPlayTrick) = 0;

	virtual bool GetInventoryItemMoved() = 0;
	virtual void SetInventoryItemMoved(bool bInventoryItemMoved) = 0;

	virtual void GetScriptTimerParam(DWORD &dwStartTime, DWORD &dwDuration) = 0;
	virtual void SetScriptTimerParam(DWORD dwStartTime, DWORD dwDuration) = 0;
	
	virtual bool GetPQIsTriggered()=0;
	virtual void SetPQIsTriggered(bool rhs)=0;

	virtual bool GetAccountBoxIsTriggered()=0;
	virtual void SetAccountBoxIsTriggered(bool rhs)=0;
	
	// 设置地图instanceID 2012/8/11 zhougaomin01305
	virtual int GetMapID() const = 0;
	virtual void SetMapID(int nMapID) = 0;

	// 设置是否自动寻路 2014/7/23
	virtual bool GetAutoMoving() const = 0;
	virtual void SetAutoMoving(bool bAutoMove) = 0;
	
	// 获得较长时间段的帧率
	virtual const std::deque<unsigned short>& GetFrameRate() const = 0;
	virtual void SetFrameRate(unsigned short frameRate) = 0;

	// 是否拥有宠物
	virtual bool GetHavePet() const = 0;
	virtual void SetHavePet(bool bHavePet) = 0;

	// 声望
	virtual int GetReputation() const = 0;
	virtual void SetReputation(int iReputation) = 0;

	// 转生次数
	virtual unsigned char GetReincarnationCount() const = 0;
	virtual void SetReincarnationCount(unsigned char ucReincarnationCount) = 0;

	// 是否有称号
	virtual bool GetHaveTitle() const = 0;
	virtual void SetHaveTitle(bool bHaveTitle) = 0;

	// 当前是否有脚本被激活
	virtual bool GetHaveActiveScript() const = 0;
	virtual void SetHaveActiveScript(bool bHaveActiveScript) = 0;
};


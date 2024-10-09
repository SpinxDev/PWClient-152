// CECScriptCheckStateInGame.cpp: implementation of the CECScriptCheckStateInGame class.
//
//////////////////////////////////////////////////////////////////////

#include "ECScriptCheckStateInGame.h"
#include "ECScriptMan.h"
#include "ECScriptContext.h"
#include "ECScriptOption.h"
#include "ECScriptController.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_Global.h"
#include "EC_UIManager.h"
#include "EC_World.h"
#include "EC_RoleTypes.h"
#include "EC_HostPlayer.h"
#include "EC_ManNPC.h"
#include "EC_ManPlayer.h"
#include "EC_NPC.h"
#include "EC_ManPlayer.h"
#include "EC_TaskInterface.h"
#include "EC_Inventory.h"
#include "EC_Skill.h"
#include "EC_HPWorkMove.h"
#include "EC_PetCorral.h"
#include "EL_Precinct.h"
#include "TaskProcess.h"
#include "TaskTempl.h"
#include "AUIDialog.h"
#include "AUIRadioButton.h"
#include "AUICheckBox.h"


CECScriptCheckStateInGame::CECScriptCheckStateInGame()
{
	m_chKillMonster = 2;
	m_chEquipNewWeapon = 2;
	m_chDisplayNext = 2;
	m_chAdjustDistance = 2;
	m_chAdjustOrient = 2;
	m_chPlayTrick = 2;
	m_chInventoryItemMoved = 2;
	m_chPQTriggered = 2;
	m_chAccountBoxTriggered = 2;

	m_dwStartTime = 0;
	m_dwDuration = 0;
}

CECScriptCheckStateInGame::~CECScriptCheckStateInGame()
{

}

CECGame * CECScriptCheckStateInGame::GetGame() const
{
	return g_pGame;
}

CECGameUIMan * CECScriptCheckStateInGame::GetGameUIMan() const
{
	return GetGame()->GetGameRun()->GetUIManager()->GetInGameUIMan();
}

CECWorld * CECScriptCheckStateInGame::GetWorld() const
{
	return GetGame()->GetGameRun()->GetWorld();
}

CECHostPlayer * CECScriptCheckStateInGame::GetHostPlayer() const
{
	return GetGame()->GetGameRun()->GetHostPlayer();
}

bool CECScriptCheckStateInGame::GetScriptExecuted(int nScriptID) const
{
	CECScriptMan * pScriptMan = GetGame()->GetGameRun()->GetUIManager()->GetScriptMan();
	if( pScriptMan )
		return !pScriptMan->GetContext()->GetOption()->IsScriptActive(nScriptID);
	else
		return false;
}

void CECScriptCheckStateInGame::SetScriptExecuted(int nScriptID, bool bExecuted)
{
}

AWString CECScriptCheckStateInGame::GetStringFromGameUIST(int nID) const
{
	return GetGameUIMan()->GetStringFromTable(nID);
}

void CECScriptCheckStateInGame::SetStringFromGameUIST(int nID, AWString strString)
{
}

int CECScriptCheckStateInGame::GetMapID() const
{
	return GetWorld()->GetInstanceID();
}

void CECScriptCheckStateInGame::SetMapID(int nMapID)
{

}

int CECScriptCheckStateInGame::GetPlayerLevel() const
{
	const ROLEBASICPROP &rbp = GetHostPlayer()->GetBasicProps();
	return rbp.iLevel;
}

void CECScriptCheckStateInGame::SetPlayerLevel(int nLevel)
{

}


int CECScriptCheckStateInGame::GetPlayerProfessionID() const
{
	return GetHostPlayer()->GetProfession();
}

void CECScriptCheckStateInGame::SetPlayerProfessionID(int nProfessionID)
{

}


int CECScriptCheckStateInGame::GetCurrentHP() const
{
	const ROLEBASICPROP &rbp = GetHostPlayer()->GetBasicProps();
	return rbp.iCurHP;
}

void CECScriptCheckStateInGame::SetCurrentHP(int nCurHP)
{

}

int CECScriptCheckStateInGame::GetMaxHP() const
{
	const ROLEEXTPROP &rep = GetHostPlayer()->GetExtendProps();
	return rep.bs.max_hp;
}

void CECScriptCheckStateInGame::SetMaxHP(int nMaxHP)
{

}


int CECScriptCheckStateInGame::GetCurrentMP() const
{
	const ROLEBASICPROP &rbp = GetHostPlayer()->GetBasicProps();
	return rbp.iCurMP;
}

void CECScriptCheckStateInGame::SetCurrentMP(int nCurMP)
{

}

int CECScriptCheckStateInGame::GetMaxMP() const
{
	const ROLEEXTPROP &rep = GetHostPlayer()->GetExtendProps();
	return rep.bs.max_mp;
}

void CECScriptCheckStateInGame::SetMaxMP(int nMaxMP)
{

}


bool CECScriptCheckStateInGame::GetDialogVisibleState(AString strDlgName) const
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog(strDlgName); 
	return ( pDlg && pDlg->IsShow());
}

void CECScriptCheckStateInGame::SetDialogVisibleState(AString strDlgName, bool bVisible)
{

}


AWString CECScriptCheckStateInGame::GetDialogItemText(AString strDlgName, AString strItemName) const
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog(strDlgName); 
	if( pDlg && pDlg->IsShow())
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(strItemName);
		if ( pObj )
			return pObj->GetText();
	}
	return L"";
}

void CECScriptCheckStateInGame::SetDialogItemText(AString strDlgName, AString strItemName, AWString strItemText)
{

}

bool CECScriptCheckStateInGame::GetDialogItemIsChecked(AString strDlgName, AString strItemName) const
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog(strDlgName); 
	if( pDlg )
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(strItemName);
		if ( pObj )
			if( pObj->GetType() == AUIOBJECT_TYPE_RADIOBUTTON )
				return ((PAUIRADIOBUTTON)pObj)->IsChecked();
			else if( pObj->GetType() == AUIOBJECT_TYPE_CHECKBOX )
				return ((PAUICHECKBOX)pObj)->IsChecked();
	}
	return false;
}

void CECScriptCheckStateInGame::SetDialogItemIsChecked(AString strDlgName, AString strItemName, bool bCheck)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsDead() const
{
	return GetHostPlayer()->IsDead();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsDead(bool bDead)
{

}

bool CECScriptCheckStateInGame::GetPlayerStatusIsFlying() const
{
	return GetHostPlayer()->IsFlying();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsFlying(bool bFlying)
{

}

bool CECScriptCheckStateInGame::GetPlayerStatusIsSitting() const
{
	return GetHostPlayer()->IsSitting();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsSitting(bool bSitting)
{

}

bool CECScriptCheckStateInGame::GetPlayerStatusIsInVader() const
{
	return GetHostPlayer()->IsInvader();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsInVader(bool bInVader)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsPariah() const
{
	return GetHostPlayer()->IsPariah();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsPariah(bool bPariah)
{

}

bool CECScriptCheckStateInGame::GetPlayerStatusIsDiving() const
{
	return GetHostPlayer()->GetBreathData().bDiving;
}

void CECScriptCheckStateInGame::SetPlayerStatusIsDiving(bool bDiving)
{

}

bool CECScriptCheckStateInGame::GetPlayerStatusIsPVPOpen() const
{
	return GetHostPlayer()->IsPVPOpen();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsPVPOpen(bool bPVPOpen)
{

}

bool CECScriptCheckStateInGame::GetPlayerStatusIsInFashionMode() const
{
	return GetHostPlayer()->InFashionMode();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsInFashionMode(bool bInFashionMode)
{

}

bool CECScriptCheckStateInGame::GetPlayerStatusIsFighting() const
{
	return GetHostPlayer()->IsFighting();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsFighting(bool bFighting)
{

}

bool CECScriptCheckStateInGame::GetPlayerStatusIsTrading() const
{
	return GetHostPlayer()->IsTrading();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsTrading(bool bTrading)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsUsingTrashBox() const
{
	return GetHostPlayer()->IsUsingTrashBox();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsUsingTrashBox(bool bUsingTrashBox)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsRooting() const
{
	return GetHostPlayer()->IsRooting();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsRooting(bool bRooting)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusCannotAttack() const
{
	return GetHostPlayer()->CannotAttack();
}

void CECScriptCheckStateInGame::SetPlayerStatusCannotAttack(bool bCannotAttack)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsMeleeing() const
{
	return GetHostPlayer()->IsMeleeing();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsMeleeing(bool bMeleeing)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsTalkingWithNPC() const
{
	return GetHostPlayer()->IsTalkingWithNPC();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsTalkingWithNPC(bool bTalkingWithNPC)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsSpellingMagic() const
{
	return GetHostPlayer()->IsSpellingMagic();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsSpellingMagic(bool bSpellingMagic)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsJumping() const
{
	return GetHostPlayer()->IsJumping();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsJumping(bool bJumping)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsPicking() const
{
	return GetHostPlayer()->IsPicking();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsPicking(bool bPicking)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsGathering() const
{
	return GetHostPlayer()->IsGathering();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsGathering(bool bGathering)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsReviving() const
{
	return GetHostPlayer()->IsReviving();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsReviving(bool bReviving)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsUsingItem() const
{
	return GetHostPlayer()->IsUsingItem();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsUsingItem(bool bUsingItem)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsChangingFace() const
{
	return GetHostPlayer()->IsChangingFace();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsChangingFace(bool bChangingFace)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsDoingSessionPose() const
{
	return GetHostPlayer()->DoingSessionPose();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsDoingSessionPose(bool bDoingSessionPose)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusTrashBoxHasPsw() const
{
	return GetHostPlayer()->TrashBoxHasPsw();
}

void CECScriptCheckStateInGame::SetPlayerStatusTrashBoxHasPsw(bool bTrashBoxHasPsw)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsMoving() const
{
	return GetHostPlayer()->IsPlayerMoving();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsMoving(bool bIsPlayerMoving)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsUnderWater() const
{
	return GetHostPlayer()->IsUnderWater();
}

void CECScriptCheckStateInGame::SetPlayerStatusIsUnderWater(bool bIsUnderWater)
{
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsOpeningBooth() const
{
	return (GetHostPlayer()->IsTrading() == 1);
}
void CECScriptCheckStateInGame::SetPlayerStatusIsOpeningBooth(bool bIsOpeningBooth)
{
}

int CECScriptCheckStateInGame::GetSelectedTargetID() const
{
	return GetHostPlayer()->GetSelectedTarget();
}

void CECScriptCheckStateInGame::SetSelectedTargetID(int nID)
{

}

int CECScriptCheckStateInGame::GetServiceNPCID() const
{
	return GetHostPlayer()->GetCurServiceNPC();
}

void CECScriptCheckStateInGame::SetServiceNPCID(int nID)
{

}


bool CECScriptCheckStateInGame::GetIsNPCID(int nNPCID) const
{
	return ISNPCID(nNPCID);
}

void CECScriptCheckStateInGame::SetIsNPCID(int nNPCID, bool bIsNPCID)
{

}

bool CECScriptCheckStateInGame::GetIsNPCExists(int nNPCID) const
{
	return GetWorld()->GetNPCMan()->GetNPC(nNPCID) != NULL;
}

void CECScriptCheckStateInGame::SetIsNPCExists(int nNPCID, bool bExists)
{

}

bool CECScriptCheckStateInGame::GetIsMonsterNPC(int nNPCID) const
{
	if(GetIsNPCExists(nNPCID))
		return GetWorld()->GetNPCMan()->GetNPC(nNPCID)->IsMonsterNPC();
	else
		return false;
}

void CECScriptCheckStateInGame::SetIsMonsterNPC(int , bool bIsMonsterNPC)
{

}

AWString CECScriptCheckStateInGame::GetNPCName(int nNPCID) const
{
	if(GetIsNPCExists(nNPCID))
		return GetWorld()->GetNPCMan()->GetNPC(nNPCID)->GetName();
	else
		return L"";
}

void CECScriptCheckStateInGame::SetNPCName(int nNPCID, AWString strName)
{

}

int CECScriptCheckStateInGame::GetNPCRandomProp(int nNPCID) const
{
	if(GetIsNPCExists(nNPCID))
		return GetWorld()->GetNPCMan()->GetNPC(nNPCID)->GetRandomProp();
	else
		return 0;
}

void CECScriptCheckStateInGame::SetNPCRandomProp(int nNPCID, int )
{

}


bool CECScriptCheckStateInGame::GetIsPlayerID(int nPlayerID) const
{
	return ISPLAYERID(nPlayerID);
}

void CECScriptCheckStateInGame::SetIsPlayerID(int nPlayerID, bool bIsPlayerID)
{

}

bool CECScriptCheckStateInGame::GetIsPlayerExists(int nPlayerID) const
{
	return GetWorld()->GetPlayerMan()->GetPlayer(nPlayerID) != NULL;
}

void CECScriptCheckStateInGame::SetIsPlayerExists(int , bool bExists)
{

}

AWString CECScriptCheckStateInGame::GetPlayerName(int nPlayerID) const
{
	if(GetIsPlayerExists(nPlayerID))
		return GetWorld()->GetPlayerMan()->GetPlayer(nPlayerID)->GetName();
	else
		return L"";
}

void CECScriptCheckStateInGame::SetPlayerName(int nPlayerID, AWString strName)
{

}


bool CECScriptCheckStateInGame::GetIsCurrentPrecinctExists() const
{
	return GetWorld()->GetCurPrecinct() != NULL;
}

void CECScriptCheckStateInGame::SetIsCurrentPrecinctExists(bool bExists)
{
}

AWString CECScriptCheckStateInGame::GetCurrentPrecinctName() const
{
	return GetWorld()->GetCurPrecinct()->GetName();
}

void CECScriptCheckStateInGame::SetCurrentPrecinctName(AWString strName)
{

}


bool CECScriptCheckStateInGame::GetIsTaskFinished(int nTaskID, const CIntArray & vecExcludedID) const
{
	if( nTaskID == -1)
	{
		FinishedTaskList* pTaskList = static_cast<FinishedTaskList*>(GetHostPlayer()->GetTaskInterface()->GetFinishedTaskList());
		unsigned long uExcludedFinsihedTaskCount = 0;
		CIntArray::const_iterator iter = vecExcludedID.begin();
		for (; iter != vecExcludedID.end(); ++iter)
		{
			if (pTaskList->SearchTask(*iter) == 0)
				++uExcludedFinsihedTaskCount;
		}
		return (pTaskList->m_FnshHeader.m_uTaskCount > uExcludedFinsihedTaskCount);
	}
	else
	{
		return ((FinishedTaskList*)GetHostPlayer()->GetTaskInterface()->GetFinishedTaskList())->SearchTask(nTaskID) == 0;
	}
}

void CECScriptCheckStateInGame::SetIsTaskFinished(int nTaskID, bool bFinished)
{

}

bool CECScriptCheckStateInGame::GetIsTaskTaked(int nTaskID, const CIntArray & vecExcludedID) const
{
	if( nTaskID == -1 )
	{
		unsigned char uExcludedTaskedTaskCount = 0;
		CIntArray::const_iterator iter = vecExcludedID.begin();
		for (; iter != vecExcludedID.end(); ++iter)
		{
			if (GetHostPlayer()->GetTaskInterface()->HasTask(*iter))
				++uExcludedTaskedTaskCount;
		}
		ActiveTaskList* pTaskList = static_cast<ActiveTaskList *>(GetHostPlayer()->GetTaskInterface()->GetActiveTaskList());
		return (pTaskList->m_uTaskCount > uExcludedTaskedTaskCount);
	}
	else
	{
		return GetHostPlayer()->GetTaskInterface()->HasTask(nTaskID);
	}
}

void CECScriptCheckStateInGame::SetIsTaskTaked(int nTaskID, bool bTaked)
{

}

bool CECScriptCheckStateInGame::GetCanFinishTask(int nTaskID, const CIntArray & vecExcludedID) const
{
	if( nTaskID == -1 )
	{
		TaskInterface *pTInferface = GetHostPlayer()->GetTaskInterface();
		ActiveTaskList* pLst = (ActiveTaskList*)pTInferface->GetActiveTaskList();
		unsigned long ulCurTime = pTInferface->GetCurTime();
		unsigned char i;

		for (i = 0; i < pLst->m_uTaskCount; i++)
		{
			ActiveTaskEntry& CurEntry = pLst->m_TaskEntries[i];
			
			bool bIsExcluded = false;
			CIntArray::const_iterator iter = vecExcludedID.begin();
			for (; iter != vecExcludedID.end(); ++iter)
			{
				if (CurEntry.m_ID == *iter)
				{
					bIsExcluded = true;
					break;
				}
			}
			if (bIsExcluded)
				continue;

			if (!CurEntry.m_ulTemplAddr) continue;
			if(CurEntry.GetTempl()->CanFinishTask(pTInferface,&CurEntry,ulCurTime))
				return true;
		}
		return false;
	}
	else
	{
		return GetHostPlayer()->GetTaskInterface()->CanFinishTask(nTaskID);
	}
}

void CECScriptCheckStateInGame::SetCanFinishTask(int nTaskID, bool bCanFinish)
{
	
}


int CECScriptCheckStateInGame::GetTaskInventoryItemCount(int nItemID) const
{
	if( nItemID == -1 )
	{
		if( GetHostPlayer()->GetTaskPack()->GetEmptySlotNum() != 
			GetHostPlayer()->GetTaskPack()->GetSize())
			return 99999999;
		else
			return -99999999;
	}
	else
		return GetHostPlayer()->GetTaskPack()->GetItemTotalNum(nItemID);
}

void CECScriptCheckStateInGame::SetTaskInventoryItemCount(int nItemID, int nCount)
{

}

int CECScriptCheckStateInGame::GetInventoryItemCount(int nItemID) const
{
	return GetHostPlayer()->GetPack()->GetItemTotalNum(nItemID);
}

void CECScriptCheckStateInGame::SetInventoryItemCount(int nItemID, int nCount)
{

}


int CECScriptCheckStateInGame::GetSkillLevel(int nSkillID) const
{
	CECSkill* pSkill = GetHostPlayer()->GetNormalSkill(nSkillID);
	if (pSkill) {
		return pSkill->GetSkillLevel();
	} else {
		return 0;
	}
}

void CECScriptCheckStateInGame::SetSkillLevel(int nSkillID, int nLevel)
{

}


int CECScriptCheckStateInGame::GetSkillAbility(int nSkillID) const
{
	return GNET::ElementSkill::GetAbility(nSkillID);
}

void CECScriptCheckStateInGame::SetSkillAblility(int nSkillID, int nAblility)
{

}

bool CECScriptCheckStateInGame::GetAutoMoving() const {
	if (CECHPWork* pCurWork = GetHostPlayer()->GetWorkMan()->GetRunningWork(CECHPWork::WORK_MOVETOPOS)){
		CECHPWorkMove* pWorkMove = static_cast<CECHPWorkMove*>(pCurWork);
		return pWorkMove->GetAutoMove();
	}
	return false;
}

void CECScriptCheckStateInGame::SetAutoMoving(bool bAutoMove) {

}

bool CECScriptCheckStateInGame::GetHavePet() const {
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	int i = 0;
	for (; i < CECPetCorral::MAX_SLOTNUM2; i++) {
		CECPetData * pPet = pPetCorral->GetPetData(i);
		if (pPet) {
			return true;
		}
	}
	return false;
}

void CECScriptCheckStateInGame::SetHavePet(bool bHavePet) {

}

int CECScriptCheckStateInGame::GetReputation() const {
	return GetHostPlayer()->GetReputation();
}

void CECScriptCheckStateInGame::SetReputation(int iReputation) {

}

unsigned char CECScriptCheckStateInGame::GetReincarnationCount() const {
	return GetHostPlayer()->GetReincarnationCount();
}

void CECScriptCheckStateInGame::SetReincarnationCount(unsigned char ucReincarnationCount) {

}


bool CECScriptCheckStateInGame::GetHaveTitle() const {
	return GetHostPlayer()->GetTitles().size() != 0;
}

void CECScriptCheckStateInGame::SetHaveTitle(bool bHaveTitle) {

}

bool CECScriptCheckStateInGame::GetHaveActiveScript() const {
	return GetGame()->GetGameRun()->GetUIManager()->GetScriptMan()->GetContext()->GetController()->GetCurrentScript() != NULL;
}

void CECScriptCheckStateInGame::SetHaveActiveScript(bool bHaveActiveScript) {

}

bool CECScriptCheckStateInGame::GetDisplayNext()
{
	char temp = m_chDisplayNext;
	m_chDisplayNext = 0;
	return temp == 1;
}

void CECScriptCheckStateInGame::SetDisplayNext(bool bDisplayNext)
{
	if( m_chDisplayNext != 2 )
		m_chDisplayNext = bDisplayNext;
}

bool CECScriptCheckStateInGame::GetKillMonster()
{
	char temp = m_chKillMonster;
	m_chKillMonster = 0;
	return temp == 1;
}

void CECScriptCheckStateInGame::SetKillMonster(bool bKillMonster)
{
	if( m_chKillMonster != 2 )
		m_chKillMonster = bKillMonster;
}

bool CECScriptCheckStateInGame::GetEquipNewWeapon()
{
	char temp = m_chEquipNewWeapon;
	m_chEquipNewWeapon = 0;
	return temp == 1;
}

void CECScriptCheckStateInGame::SetEquipNewWeapon(bool bEquipNewWeapon)
{
	if( m_chEquipNewWeapon != 2 )
		m_chEquipNewWeapon = bEquipNewWeapon;
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsAdjustOrient()
{
	char temp = m_chAdjustOrient;
	m_chAdjustOrient = 0;
	return temp == 1;
}

void CECScriptCheckStateInGame::SetPlayerStatusIsAdjustOrient(bool bAdjustOrient)
{
	if( m_chAdjustOrient != 2 )
		m_chAdjustOrient = bAdjustOrient;
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsAdjustDistance()
{
	char temp = m_chAdjustDistance;
	m_chAdjustDistance = 0;
	return temp == 1;
}

void CECScriptCheckStateInGame::SetPlayerStatusIsAdjustDistance(bool bAdjustDistancer)
{
	if( m_chAdjustDistance != 2 )
		m_chAdjustDistance = bAdjustDistancer;
}

bool CECScriptCheckStateInGame::GetPlayerStatusIsPlayTrick()
{
	char temp = m_chPlayTrick;
	m_chPlayTrick = 0;
	return temp == 1;
}

void CECScriptCheckStateInGame::SetPlayerStatusIsPlayTrick(bool bPlayTrick)
{
	if( m_chPlayTrick != 2 )
		m_chPlayTrick = bPlayTrick;
}

bool CECScriptCheckStateInGame::GetInventoryItemMoved()
{
	char temp = m_chInventoryItemMoved;
	m_chInventoryItemMoved = 0;
	return temp == 1;
}

void CECScriptCheckStateInGame::SetInventoryItemMoved(bool bInventoryItemMoved)
{
	if( m_chInventoryItemMoved != 2 )
		m_chInventoryItemMoved = bInventoryItemMoved;
}

void CECScriptCheckStateInGame::GetScriptTimerParam(DWORD &dwStartTime, DWORD &dwDuration)
{
	dwStartTime = m_dwStartTime;
	dwDuration = m_dwDuration;
}

void CECScriptCheckStateInGame::SetScriptTimerParam(DWORD dwStartTime, DWORD dwDuration)
{
	m_dwStartTime = dwStartTime;
	m_dwDuration = dwDuration;
}

bool CECScriptCheckStateInGame::GetPQIsTriggered()
{
	char temp = m_chPQTriggered;
	m_chPQTriggered = 0;
	return temp == 1;
}

void CECScriptCheckStateInGame::SetPQIsTriggered(bool rhs)
{
	if (m_chPQTriggered != 2)
		m_chPQTriggered = rhs;
}

bool CECScriptCheckStateInGame::GetAccountBoxIsTriggered()
{
	char temp = m_chAccountBoxTriggered;
	m_chAccountBoxTriggered = 0;
	return temp == 1;
}

void CECScriptCheckStateInGame::SetAccountBoxIsTriggered(bool rhs)
{
	if (m_chAccountBoxTriggered != 2)
		m_chAccountBoxTriggered = rhs;
}

const std::deque<unsigned short>& CECScriptCheckStateInGame::GetFrameRate() const {
	return m_frameRateList;
}

void CECScriptCheckStateInGame::SetFrameRate(unsigned short frameRate) {
	const int MAX_FRAMERATE_RECORD = 5;
	if (m_frameRateList.size() == MAX_FRAMERATE_RECORD) {
		m_frameRateList.pop_front();
	}
	m_frameRateList.push_back(frameRate);
}
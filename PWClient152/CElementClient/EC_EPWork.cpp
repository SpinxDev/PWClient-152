// Filename	: EC_EPWork.cpp
// Creator	: Xu Wenbin
// Date		: 2014/8/6

#include "EC_EPWork.h"
#include "EC_ElsePlayer.h"
#include "EC_Skill.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_ManAttacks.h"
#include "EC_CastSkillWhenMove.h"
#include "EC_Global.h"
#include "EC_ElementDataHelper.h"

extern A3DVECTOR3	g_vAxisY;

//	class CECEPWork
CECEPWork::CECEPWork(int iWorkID, CECEPWorkMan* pWorkMan)
: m_iWorkID(iWorkID)
, m_pWorkMan(pWorkMan)
, m_bFinished(false)
{
}

const char * CECEPWork::GetWorkName(int iWork){
	static const char *l_WorkName[NUM_WORK] = {
		"WORK_STAND",
		"WORK_MOVE",
		"WORK_HACKOBJECT",
		"WORK_SPELL",
		"WORK_PICKUP",
		"WORK_DEAD",	
		"WORK_USEITEM",		
		"WORK_IDLE",
		"WORK_FLASHMOVE",	
		"WORK_PASSIVEMOVE",	
		"WORK_CONGREGATE",		
		"WORK_SKILLSTATEACT",
	};
	if (iWork >= WORK_STAND && iWork < sizeof(l_WorkName)/sizeof(l_WorkName[0])){
		return l_WorkName[iWork];
	}else{
		return "Invalid Work";
	}
}

const char *CECEPWork::GetWorkName()const{
	return GetWorkName(GetWorkID());
}

int CECEPWork::GetWorkID()const{
	return m_iWorkID;
}

bool CECEPWork::IsFinished()const{
	return m_bFinished;
}

bool CECEPWork::Validate(int iWork){
	return iWork >= WORK_STAND && iWork < NUM_WORK;
}

void CECEPWork::Cancel(){
}

void CECEPWork::Finish(){
	m_bFinished = true;
}

void CECEPWork::Tick(DWORD dwDeltaTime){
}

void CECEPWork::Start(){
}

CECElsePlayer * CECEPWork::GetPlayer()const{
	return m_pWorkMan->GetPlayer();
}

//	class CECEPWorkStand
CECEPWorkStand::CECEPWorkStand(CECEPWorkMan* pWorkMan)
: CECEPWork(WORK_STAND, pWorkMan)
{
}
void CECEPWorkStand::Start(){
	GetPlayer()->m_iMoveMode = CECElsePlayer::MOVE_STAND;
	GetPlayer()->PlayAction(GetStandAction());
}
void CECEPWorkStand::Tick(DWORD dwDeltaTime){
	if (!GetPlayer()->m_FightCnt.IsFull()){
		GetPlayer()->m_FightCnt.IncCounter(dwDeltaTime);
	}
	if (GetPlayer()->GetPlayerModel()){
		if (CECPlayer::IsMoveStandAction(GetPlayer()->GetLowerBodyAction())){
			GetPlayer()->PlayAction(GetStandAction(), false);
		}
	}
}
int	CECEPWorkStand::GetStandAction()const{
	return GetPlayer()->GetMoveStandAction(false, GetPlayer()->IsFighting());
}

//	class CECEPWorkMove
CECEPWorkMove::CECEPWorkMove(CECEPWorkMan* pWorkMan)
: CECEPWork(WORK_MOVE, pWorkMan)
{
}
void CECEPWorkMove::Start(){
	GetPlayer()->PlayAction(GetPlayer()->GetMoveStandAction(true));
}
void CECEPWorkMove::Tick(DWORD dwDeltaTime){		
	if (GetPlayer()->MovingTo(dwDeltaTime)){
		GetPlayer()->m_fLastSpeed = 0.0f;
		GetPlayer()->StartModelMove(GetPlayer()->m_vStopDir, g_vAxisY, 150);
		GetPlayer()->ChangeModelTargetDirAndUp(GetPlayer()->m_vStopDir, g_vAxisY);
		Finish();
	}
}
void CECEPWorkMove::Cancel(){
	GetPlayer()->SetPos(GetPlayer()->m_vServerPos);
	GetPlayer()->StopModelMove(GetPlayer()->m_vStopDir, g_vAxisY, 150);
}

//	class CECEPWorkMelee
CECEPWorkMelee::CECEPWorkMelee(CECEPWorkMan* pWorkMan, int attackTarget)
: CECEPWork(WORK_HACKOBJECT, pWorkMan)
, m_iAttackTarget(attackTarget)
{
}
void CECEPWorkMelee::Start(){
	GetPlayer()->m_iMoveMode = CECElsePlayer::MOVE_STAND;
	GetPlayer()->PlayAction(CECPlayer::ACT_ATTACK_1 + a_Random(0, 3));
	GetPlayer()->EnterFightState();
}
void CECEPWorkMelee::Tick(DWORD dwDeltaTime){
	GetPlayer()->TurnFaceTo(m_iAttackTarget);
	
	//	Slide to server position
	A3DVECTOR3 vDir = GetPlayer()->m_vServerPos - GetPlayer()->GetPos();
	float fDist = vDir.Normalize();
	if (fDist > 0.0001f)
	{
		float fMoveDist = 10.0f * dwDeltaTime * 0.001f;
		if (fMoveDist > fDist)
			fMoveDist = fDist;
		
		GetPlayer()->SetPos(GetPlayer()->GetPos() + vDir * fMoveDist);
	}	
	if (GetPlayer()->m_FightCnt.IncCounter(dwDeltaTime))
	{
		Finish();
	}
}
void CECEPWorkMelee::Cancel(){
	GetPlayer()->TurnFaceTo(0);
}

//	class CECEPWorkSpell
CECEPWorkSpell::CECEPWorkSpell(CECEPWorkMan* pWorkMan, DWORD dwPeriod, CECSkill *pSkill, int target)
: CECEPWork(WORK_SPELL, pWorkMan)
, m_pCurSkill(pSkill)
, m_idCurSkillTarget(target)
{		
	m_SkillCnt.SetPeriod(dwPeriod);
	m_SkillCnt.Reset();
}
CECEPWorkSpell::~CECEPWorkSpell(){
	if (m_pCurSkill){
		delete m_pCurSkill;
		m_pCurSkill = NULL;
	}
}
void CECEPWorkSpell::Start(){
	GetPlayer()->m_iMoveMode = CECElsePlayer::MOVE_STAND;
}
void CECEPWorkSpell::Tick(DWORD dwDeltaTime){		
	GetPlayer()->m_FightCnt.IncCounter(dwDeltaTime);
	int iRealTime = g_pGame->GetRealTickTime();
	if (m_SkillCnt.IncCounter(iRealTime)){
		Finish();
	}else{
		if (m_idCurSkillTarget){
			GetPlayer()->TurnFaceTo(m_idCurSkillTarget);
		}
		if (GetPlayer()->IsPlayingMoveAction() && !GetPlayer()->IsWorkMoveRunning()){	//	从移动施法到停止移动、会一直在持续播放移动动作，此处暂停
			GetPlayer()->PlayAction(CECPlayer::ACT_FIGHTSTAND);
		}
		if (!GetPlayer()->IsPlayingAction()){
			GetPlayer()->PlayAction(CECPlayer::ACT_FIGHTSTAND);	//	技能动作播放完成后、播放战斗站立动作
		}
	}
}
void CECEPWorkSpell::Cancel(){	
	if (m_pCurSkill){
		delete m_pCurSkill;
		m_pCurSkill = NULL;
	}
	m_idCurSkillTarget = 0;
	GetPlayer()->StopSkillAttackAction();
	GetPlayer()->TurnFaceTo(0);
}

CECSkill * CECEPWorkSpell::GetSkill()const{
	return m_pCurSkill;
}

//	class CECEPWorkPickUp
CECEPWorkPickUp::CECEPWorkPickUp(CECEPWorkMan* pWorkMan, DWORD dwPeriod, Type type, int gatherItemID)
: CECEPWork(WORK_PICKUP, pWorkMan)
, m_type(type)
, m_gatherItemID(gatherItemID)
{
	m_PickupCnt.SetPeriod(dwPeriod);
	m_PickupCnt.Reset();
}
void CECEPWorkPickUp::Start(){
	GetPlayer()->m_iMoveMode = CECPlayer::MOVE_STAND;
	m_PickupCnt.Reset();
	switch (m_type){
	case PICKUP_ITEM:
		GetPlayer()->PlayAction(CECPlayer::ACT_PICKUP_MATTER);
		GetPlayer()->PlayAction(CECPlayer::ACT_STAND, true, 200, true);
		break;
	case GATHER_ITEM:
		GetPlayer()->PlayActionWithConfig(CECPlayer::ACT_PICKUP, CECElementDataHelper::GetGatherStartActionConfig(m_gatherItemID));
		GetPlayer()->PlayActionWithConfig(CECPlayer::ACT_PICKUP_LOOP, CECElementDataHelper::GetGatherLoopActionConfig(m_gatherItemID), false, 200, true);
		break;
	case GATHER_MONSTER_SPIRIT:
		GetPlayer()->PlayGatherMonsterSpiritAction();
		break;
	}
}
void CECEPWorkPickUp::Tick(DWORD dwDeltaTime){		
	DWORD dwRealTime = g_pGame->GetRealTickTime();
	switch (m_type){
	case PICKUP_ITEM:
	case GATHER_MONSTER_SPIRIT:
		if (m_PickupCnt.IncCounter(dwRealTime)){
			Finish();
		}
		break;
	}
}

//	class CECEPWorkDead
CECEPWorkDead::CECEPWorkDead(CECEPWorkMan *pWorkMan, DWORD dwParam)
: CECEPWork(WORK_DEAD, pWorkMan)
, m_dwParam(dwParam)
{
}
void CECEPWorkDead::Start(){
	GetPlayer()->m_iMoveMode = CECPlayer::MOVE_STAND;
	
	A3DVECTOR3 vCurPos = GetPlayer()->GetPos();
	A3DVECTOR3 vGndPos, vNormal, vTestPos = vCurPos + g_vAxisY;
	VertRayTrace(vTestPos, vGndPos, vNormal, 10.0f);
	float fWaterHei = g_pGame->GetGameRun()->GetWorld()->GetWaterHeight(vTestPos);
	
	int iFlag = 0;
	//	if (vCurPos.y < fWaterHei - m_MoveConst.fShoreDepth || 
	//		(vCurPos.y < fWaterHei && fWaterHei - vGndPos.y > m_MoveConst.fShoreDepth))
	if (GetPlayer()->CheckWaterMoveEnv(vCurPos, fWaterHei, vGndPos.y))
		iFlag = 1;
	else if (vCurPos.y > vGndPos.y + 0.5f)
		iFlag = 2;
	
	if (m_dwParam)
	{
		if (iFlag == 1)
			GetPlayer()->PlayAction(CECPlayer::ACT_WATERDIE_LOOP);
		else if (iFlag == 2)
			GetPlayer()->PlayAction(CECPlayer::ACT_AIRDIE);
		else
			GetPlayer()->PlayAction(CECPlayer::ACT_GROUNDDIE_LOOP);
	}
	else
	{
		if (iFlag == 1)
		{
			GetPlayer()->PlayAction(CECPlayer::ACT_WATERDIE);
			GetPlayer()->PlayAction(CECPlayer::ACT_WATERDIE_LOOP, true, 200, true);
		}
		else if (iFlag == 2)
		{
			GetPlayer()->PlayAction(CECPlayer::ACT_AIRDIE_ST);
			GetPlayer()->PlayAction(CECPlayer::ACT_AIRDIE, true, 200, true);
		}
		else
		{
			GetPlayer()->PlayAction(CECPlayer::ACT_GROUNDDIE);
			GetPlayer()->PlayAction(CECPlayer::ACT_GROUNDDIE_LOOP, true, 200, true);
		}
	}
}

//	class CECEPWorkUseItem
CECEPWorkUseItem::CECEPWorkUseItem(CECEPWorkMan *pWorkMan, int itemID)
: CECEPWork(WORK_USEITEM, pWorkMan)
, m_itemID(itemID)
{
}
void CECEPWorkUseItem::Start(){		
	GetPlayer()->m_iMoveMode = CECPlayer::MOVE_STAND;
	GetPlayer()->PlayStartUseItemAction(m_itemID);
}

//	class CECEPWorkIdle
CECEPWorkIdle::CECEPWorkIdle(CECEPWorkMan *pWorkMan, int iType, int iOTTime, DWORD dwParam)
: CECEPWork(WORK_IDLE, pWorkMan)
, m_dwParam(dwParam)
{
	m_iType	= iType;
	m_bOTCheck	= iOTTime > 0 ? true : false;
	
	if (m_bOTCheck)
	{
		m_OTCnt.SetPeriod(iOTTime);
		m_OTCnt.Reset();
	}
	
	switch (iType)
	{
	case IDLE_SITDOWN:			
		GetPlayer()->m_iMoveMode = CECPlayer::MOVE_STAND;
		
		if (m_dwParam)
			GetPlayer()->PlayAction(CECPlayer::ACT_SITDOWN_LOOP);
		else
		{
			GetPlayer()->PlayAction(CECPlayer::ACT_SITDOWN);
			GetPlayer()->PlayAction(CECPlayer::ACT_SITDOWN_LOOP, true, 200, true);
		}
		
		break;
		
	case IDLE_REVIVE:
		
		GetPlayer()->PlayAction(CECPlayer::ACT_REVIVE);
		break;
		
	case IDLE_BOOTH:
		
		GetPlayer()->m_iMoveMode = CECPlayer::MOVE_STAND;
		break;
	}
}

void CECEPWorkIdle::Tick(DWORD dwDeltaTime){		
	DWORD dwRealTime = g_pGame->GetRealTickTime();
	if (m_bOTCheck && m_OTCnt.IncCounter(dwRealTime)){
		Finish();
	}
}
int CECEPWorkIdle::GetType()const{
	return m_iType;
}

//	class CECEPWorkFlashMove
CECEPWorkFlashMove::CECEPWorkFlashMove(CECEPWorkMan *pWorkMan, const A3DVECTOR3 &vServerPos, float fMoveSpeed)
: CECEPWork(WORK_FLASHMOVE, pWorkMan)
, m_vServerPos(vServerPos)
, m_fMoveSpeed(fMoveSpeed)
{
}
void CECEPWorkFlashMove::Start(){		
	GetPlayer()->m_iMoveMode = CECPlayer::MOVE_MOVE;		
	GetPlayer()->PlayAction(CECPlayer::ACT_JUMP_START, true, 0);
	GetPlayer()->PlayAction(CECPlayer::ACT_JUMP_LOOP, false, 0, true);
}
void CECEPWorkFlashMove::Tick(DWORD dwDeltaTime){		
	A3DVECTOR3 vCurPos = GetPlayer()->GetPos();
	A3DVECTOR3 vDir = m_vServerPos - vCurPos;
	float fDist = vDir.Normalize();
	float fMoveDelta = m_fMoveSpeed * dwDeltaTime * 0.001f;
	
	if (fMoveDelta >= fDist)
	{
		GetPlayer()->SetPos(m_vServerPos);
		Finish();
	}
	else
	{
		vCurPos = vCurPos + vDir * fMoveDelta;
		GetPlayer()->SetPos(vCurPos);
	}
}
void CECEPWorkFlashMove::Cancel(){
	GetPlayer()->SetPos(m_vServerPos);
}

//	class CECEPWorkPassiveMove
CECEPWorkPassiveMove::CECEPWorkPassiveMove(CECEPWorkMan *pWorkMan, const A3DVECTOR3 &vServerPos, float fMoveSpeed)
: CECEPWork(WORK_PASSIVEMOVE, pWorkMan)
, m_vServerPos(vServerPos)
, m_fMoveSpeed(fMoveSpeed)
{
}
void CECEPWorkPassiveMove::Start(){
	GetPlayer()->m_iMoveMode = CECPlayer::MOVE_MOVE;
	GetPlayer()->PlayAction(CECPlayer::ACT_STRIKEBACK, true, 0);
}
void CECEPWorkPassiveMove::Tick(DWORD dwDeltaTime){
	A3DVECTOR3 vCurPos = GetPlayer()->GetPos();
	A3DVECTOR3 vDir = m_vServerPos - vCurPos;
	float fDist = vDir.Normalize();
	float fMoveDelta = m_fMoveSpeed * dwDeltaTime * 0.001f;
	
	if (fMoveDelta >= fDist)
	{
		GetPlayer()->SetPos(m_vServerPos);
		Finish();
	}
	else
	{
		vCurPos = vCurPos + vDir * fMoveDelta;
		
		//	根据地形调整玩家高度，保证在地形以上
		float terrianHeight = g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(vCurPos);
		a_ClampFloor(vCurPos.y, terrianHeight);
		
		GetPlayer()->SetPos(vCurPos);
	}
}
void CECEPWorkPassiveMove::Cancel(){
	GetPlayer()->SetPos(m_vServerPos);
}

//	class CECEPWorkCongregate
CECEPWorkCongregate::CECEPWorkCongregate(CECEPWorkMan *pWorkMan, int iType)
: CECEPWork(WORK_CONGREGATE, pWorkMan)
, m_iType(iType)
{
}
void CECEPWorkCongregate::Start(){
	GetPlayer()->m_iMoveMode = CECPlayer::MOVE_STAND;
	GetPlayer()->PlayAction(CECPlayer::ACT_SITDOWN);
	GetPlayer()->PlayAction(CECPlayer::ACT_SITDOWN_LOOP, false, 200, true);
}
int CECEPWorkCongregate::GetType()const{
	return m_iType;
}

//	class CECEPWorkSkillStateAct
CECEPWorkSkillStateAct::CECEPWorkSkillStateAct(CECEPWorkMan *pWorkMan, int idSkill)
: CECEPWork(WORK_SKILLSTATEACT, pWorkMan)
, m_skillID(idSkill)
{
}
void CECEPWorkSkillStateAct::Start(){
	int state = GetPlayer()->HasSkillStateForAction();
	if(GetPlayer()->GetPlayerModel() && state && m_skillID)
	{
		CECAttacksMan* pAtkMan = g_pGame->GetGameRun()->GetWorld()->GetAttacksMan();
		AString name1,name2;
		if (pAtkMan && pAtkMan->GetSkillStateActionName(m_skillID,state,name1,name2))
		{
			if (GetPlayer()->PlayNonSkillActionWithName(CECPlayer::ACT_WOUNDED, name1, false, 0)){
				GetPlayer()->QueueNonSkillActionWithName(CECPlayer::ACT_WOUNDED, name2, 0);
			}
		}
	}
}

//	class CECEPWorkMan
CECEPWorkMan::CECEPWorkMan(CECElsePlayer *pElsePlayer)
: m_pElsePlayer(pElsePlayer)
, m_iCurWorkType(-1)
{
}

CECEPWorkMan::~CECEPWorkMan(){
	for (int i(0); i < NUM_WORKTYPE; ++ i){
		WorkList &workList = m_WorkStack[i];
		for (size_t j(0); j < workList.size(); ++ j){
			delete workList[j];
		}
		workList.clear();
	}
}

bool CECEPWorkMan::ValidateWorkType(int iWorkType)const{
	return iWorkType >= 0 && iWorkType < NUM_WORKTYPE;
}

const char * CECEPWorkMan::GetWorkTypeName(int iWorkType){
	static const char *l_WorkTypeName[NUM_WORKTYPE] = {
		"WT_NOTHING",
		"WT_NORMAL",
		"WT_INTERRUPT",
	};
	if (iWorkType >= 0 && iWorkType < sizeof(l_WorkTypeName)/sizeof(l_WorkTypeName[0])){
		return l_WorkTypeName[iWorkType];
	}else{
		return "Invalid work type";
	}
}

CECElsePlayer * CECEPWorkMan::GetPlayer()const{
	return m_pElsePlayer;
}

int CECEPWorkMan::GetCurrentWorkType()const{
	return m_iCurWorkType;
}

void CECEPWorkMan::FinishWork(const CECEPWorkMatcher &matcher){
	bool bCurrentWorkFinished(false);
	for (int i = NUM_WORKTYPE-1; i >= 0; -- i){
		WorkList &workList = m_WorkStack[i];
		if (workList.empty()){
			continue;
		}
		for (size_t j(0); j < workList.size();){
			if (!matcher(workList[j], i)){
				++ j;
				continue;
			}
			KillWork(i, j);
			if (i == m_iCurWorkType && workList.empty()){
				bCurrentWorkFinished = true;
			}
		}
	}
	if (bCurrentWorkFinished){
		StartAwaitingWorks();
	}
}

bool CECEPWorkMan::FinishIdleWork(int iType){
	class CECEPIdleWorkMatcher : public CECEPWorkMatcher{
		int		m_iType;
	public:
		explicit CECEPIdleWorkMatcher(int iType)
			: m_iType(iType)
		{
		}
		virtual bool operator()(CECEPWork *pWork, int iWorkType)const{
			if (pWork != NULL && pWork->GetWorkID() == CECEPWork::WORK_IDLE){
				CECEPWorkIdle *pIdleWork = dynamic_cast<CECEPWorkIdle *>(pWork);
				return pIdleWork->GetType() == m_iType;
			}
			return false;
		}
	};
	FinishWork(CECEPIdleWorkMatcher(iType));
	return true;
}

bool CECEPWorkMan::FinishCongregateWork(int iType){
	class CECEPCongregateWorkMatcher : public CECEPWorkMatcher{
		int		m_iType;
	public:
		explicit CECEPCongregateWorkMatcher(int iType)
			: m_iType(iType)
		{
		}
		virtual bool operator()(CECEPWork *pWork, int iWorkType)const{
			if (pWork != NULL && pWork->GetWorkID() == CECEPWork::WORK_CONGREGATE){
				CECEPWorkCongregate *pCongregateWork = dynamic_cast<CECEPWorkCongregate *>(pWork);
				return pCongregateWork->GetType() == m_iType;
			}
			return false;
		}
	};
	FinishWork(CECEPCongregateWorkMatcher(iType));
	return true;
}

void CECEPWorkMan::FinishWork(int idWork){
	FinishWork(CECEPWorkIDMatcher(idWork));
}

void CECEPWorkMan::FinishRunningWork(int idWork){
	if (!IsWorkRunning(idWork)){
		return;
	}
	WorkList &workList = m_WorkStack[m_iCurWorkType];
	for (size_t i(0); i < workList.size();){
		if (workList[i]->GetWorkID() != idWork){
			++ i;
			continue;
		}		
		LOG_DEBUG_INFO(AString().Format("CECEPWorkMan::FinishRunningWork %s(%s)", workList[i]->GetWorkName(), GetWorkTypeName(m_iCurWorkType)));
		KillWork(m_iCurWorkType, i);
	}
	if (workList.empty()){
		StartAwaitingWorks();
	}
}

CECEPWork* CECEPWorkMan::GetRunningWork(int iWorkID)const{
	CECEPWork *result = NULL;
	if (ValidateWorkType(m_iCurWorkType)){
		const WorkList &workList = m_WorkStack[m_iCurWorkType];
		for (size_t i(0); i < workList.size(); ++ i){
			if (iWorkID == workList[i]->GetWorkID()){
				result = workList[i];
				break;
			}
		}
	}
	return result;
}

bool CECEPWorkMan::IsWorkRunning(int iWork)const{
	return FindWork(m_iCurWorkType, iWork);
}

bool CECEPWorkMan::FindWork(int iWorkType, int iWorkID)const{
	if (!ValidateWorkType(iWorkType)){
		return false;
	}
	const WorkList &workList = m_WorkStack[iWorkType];
	for (size_t i(0); i < workList.size(); ++ i){
		if (iWorkID == workList[i]->GetWorkID()){
			return true;
		}
	}
	return false;
}

bool CECEPWorkMan::IsAnyWorkRunning()const{
	return ValidateWorkType(m_iCurWorkType) && !m_WorkStack[m_iCurWorkType].empty();
}

void CECEPWorkMan::KillWork(int iWorkType, int index){
	if (!ValidateWorkType(iWorkType)){
		ASSERT(false);
		return;
	}
	WorkList &workList = m_WorkStack[iWorkType];
	if (index < 0 || index >= (int)workList.size()){
		ASSERT(false);
		return;
	}
	CECEPWork *pWork = workList[index];
	pWork->Cancel();
	LOG_DEBUG_INFO(AString().Format("CECEPWorkMan::%s(%s) killed", pWork->GetWorkName(), GetWorkTypeName(iWorkType)));
	delete pWork;
	workList.erase(workList.begin() + index);
}

void CECEPWorkMan::Tick(DWORD dwDeltaTime){
	if (!IsAnyWorkRunning()){
		return;
	}
	const WorkList & workList = m_WorkStack[m_iCurWorkType];
	for (size_t i(0); i < workList.size();){
		CECEPWork *pWork = workList[i];
		pWork->Tick(dwDeltaTime);
		if (!pWork->IsFinished()){
			++ i;
			continue;
		}
		KillWork(m_iCurWorkType, i);
	}
	if (workList.empty()){
		StartAwaitingWorks();
	}
}

void CECEPWorkMan::FinishWorkAtWorkType(int iWorkType){
	if (!ValidateWorkType(iWorkType)){
		ASSERT(false);
		return;
	}
	WorkList &workList = m_WorkStack[iWorkType];
	while (!workList.empty()){
		KillWork(iWorkType, 0);
	}
	if (m_iCurWorkType == iWorkType){
		m_iCurWorkType = -1;
	}
}

void CECEPWorkMan::CancelWorkAtWorkType(int iWorkType){
	if (!ValidateWorkType(iWorkType)){
		return;
	}
	WorkList &workList = m_WorkStack[iWorkType];
	for (size_t i(0); i < workList.size(); ++ i){
		CancelWork(workList[i]);
		LOG_DEBUG_INFO(AString().Format("CECEPWorkMan::%s(%s) cancelled", workList[i]->GetWorkName(), GetWorkTypeName(iWorkType)));
	}
}

void CECEPWorkMan::CancelWork(CECEPWork *pWork){
	if (!pWork){
		ASSERT(false);
		return;
	}
	pWork->Cancel();
}

void CECEPWorkMan::StartNormalWork(CECEPWork *pWork){
	StartWork(WT_NORMAL, pWork);
}

void CECEPWorkMan::StartWork(int iWorkType, CECEPWork *pWork)
{
	if (!ValidateWorkType(iWorkType)){
		ASSERT(false);
		delete pWork;
		return;
	}
	if (CanMergeWithCurrentWork(iWorkType, pWork)){
		MergeWork(iWorkType, pWork);
	}else{
		if (pWork->GetWorkID() == CECEPWork::WORK_DEAD)	{
			for (int i = iWorkType; i < NUM_WORKTYPE; i++){
				FinishWorkAtWorkType(i);
			}
			AppendWork(iWorkType, pWork);
			m_iCurWorkType = iWorkType;
		}else{			
			bool shouldStart = (iWorkType >= m_iCurWorkType);
			FinishWorkAtWorkType(iWorkType);
			AppendWork(iWorkType, pWork);
			if (!shouldStart){
				LOG_DEBUG_INFO(AString().Format("CECEPWork::StartWork %s flushed %s", pWork->GetWorkName(), GetWorkTypeName(iWorkType)));
				return;
			}
			if (iWorkType > m_iCurWorkType){
				CancelWorkAtWorkType(m_iCurWorkType);
			}
			m_iCurWorkType = iWorkType;
		}
	}
	pWork->Start();
	LOG_DEBUG_INFO(AString().Format("CECEPWorkMan::StartWork %s(%s) started", pWork->GetWorkName(), GetWorkTypeName(iWorkType)));
}

void CECEPWorkMan::AppendWork(int iWorkType, CECEPWork *pWork){
	if (!ValidateWorkType(iWorkType) || !pWork){
		ASSERT(false);
		return;
	}
	m_WorkStack[iWorkType].push_back(pWork);
}

void CECEPWorkMan::StartAwaitingWorks(){
	if (ValidateWorkType(m_iCurWorkType)){
		for (-- m_iCurWorkType; m_iCurWorkType >= 0; -- m_iCurWorkType){
			WorkList &workList = m_WorkStack[m_iCurWorkType];
			if (workList.empty()){
				continue;
			}
			for (size_t j(0); j < workList.size(); ++ j){
				workList[j]->Start();
				LOG_DEBUG_INFO(AString().Format("CECElsePlayer::StartAwaitingWorks %s(%s) started by decrease priority", workList[j]->GetWorkName(), GetWorkTypeName(m_iCurWorkType)));
			}
			break;
		}
	}
}

bool CECEPWorkMan::CanSpellWhileMoving(const CECEPWorkSpell *pWorkSpell){
	return pWorkSpell->GetSkill()
		&& CECCastSkillWhenMove::Instance().IsSkillSupported(pWorkSpell->GetSkill()->GetSkillID(), GetPlayer());
}

bool CECEPWorkMan::CanRunSimultaneously(const CECEPWork *pWork1, const CECEPWork *pWork2){
	if (pWork1->GetWorkID() == CECEPWork::WORK_MOVE &&
		pWork2->GetWorkID() == CECEPWork::WORK_SPELL){
		return CanSpellWhileMoving(dynamic_cast<const CECEPWorkSpell *>(pWork2));
	}
	if (pWork2->GetWorkID() == CECEPWork::WORK_MOVE &&
		pWork1->GetWorkID() == CECEPWork::WORK_SPELL){
		return CanSpellWhileMoving(dynamic_cast<const CECEPWorkSpell *>(pWork1));
	}
	return false;
}

bool CECEPWorkMan::CanReplace(const CECEPWork *pNewWork, const CECEPWork *pExistWork){
	if (pNewWork->GetWorkID() == CECEPWork::WORK_SPELL &&
		pExistWork->GetWorkID() == CECEPWork::WORK_SPELL){
		return CanSpellWhileMoving(dynamic_cast<const CECEPWorkSpell *>(pNewWork))
			&& CanSpellWhileMoving(dynamic_cast<const CECEPWorkSpell *>(pExistWork));
	}
	return false;
}

bool CECEPWorkMan::CanMergeWithCurrentWork(int iWorkType, const CECEPWork *pWork){
	if (!ValidateWorkType(iWorkType) || iWorkType != m_iCurWorkType){
		return false;
	}
	const WorkList &workList = m_WorkStack[iWorkType];
	if (workList.empty()){
		return false;
	}
	for (size_t i(0); i < workList.size(); ++ i){
		if (!CanRunSimultaneously(pWork, workList[i]) &&	//	要么同时共存
			!CanReplace(pWork, workList[i])){				//	要么可以替换
			return false;
		}
	}
	return true;
}

void CECEPWorkMan::MergeWork(int iWorkType, CECEPWork *pWork){
	if (!CanMergeWithCurrentWork(iWorkType, pWork)){
		ASSERT(false);
		return;
	}
	WorkList &workList = m_WorkStack[iWorkType];
	for (size_t i(0); i < workList.size(); ++ i){
		if (CanReplace(pWork, workList[i])){
			LOG_DEBUG_INFO(AString().Format("CECEPWorkMan::MergeWork %s replacing %s at %s", pWork->GetWorkName(), workList[i]->GetWorkName(), GetWorkTypeName(iWorkType)));
			KillWork(iWorkType, i);
			workList.insert(workList.begin()+i, pWork);
			return;
		}
	}
	workList.push_back(pWork);
	LOG_DEBUG_INFO(AString().Format("CECEPWorkMan::MergeWork %s appended to %s", pWork->GetWorkName(), GetWorkTypeName(iWorkType)));
}

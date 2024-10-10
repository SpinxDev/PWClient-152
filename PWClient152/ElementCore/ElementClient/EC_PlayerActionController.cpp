// Filename	: EC_PlayerActionController.cpp
// Creator	: Xu Wenbin
// Date		: 2014/7/19

#include "EC_PlayerActionController.h"
#include "EC_RoleTypes.h"
#include "EC_CastSkillWhenMove.h"
#include "EC_Player.h"
#include "EC_Global.h"

#include <EC_Model.h>
#include <A3DSkinModelAct.h>
#include <A3DSkeleton.h>

//	CECPlayerActionPlayPolicy
CECPlayerActionPlayPolicy::CECPlayerActionPlayPolicy(CECPlayer *pPlayer, CECModel *pPlayerModel)
: m_pPlayer(pPlayer)
, m_pPlayerModel(pPlayerModel)
{
}

bool CECPlayerActionPlayPolicy::CanCombineWithMoveForSkill(int idSkill)const{
	return false;
}

CECPlayer* CECPlayerActionPlayPolicy::GetPlayer()const{
	return m_pPlayer;
}

CECModel * CECPlayerActionPlayPolicy::GetModel()const{
	return m_pPlayerModel;
}

bool CECPlayerActionPlayPolicy::IsMoving()const{
	return IsPlayingMoveAction() && m_pPlayer->IsWorkMoveRunning();
}

bool CECPlayerActionPlayPolicy::IsMoveAction(DWORD dwUserData){
	return dwUserData == CECPlayer::ACT_RUN
		|| dwUserData == CECPlayer::ACT_WALK
		|| dwUserData == CECPlayer::ACT_FLY
		|| dwUserData == CECPlayer::ACT_FLY_SWORD
		|| dwUserData == CECPlayer::ACT_SWIM
		|| dwUserData == CECPlayer::ACT_SWIM_FOR_MOVESKILL;
}

bool CECPlayerActionPlayPolicy::IsPlayingCastingSkillAction()const{
	return IsPlayingAction(CECPlayer::ACT_CASTSKILL);
}

//	class CECPlayerActionPlayDefaultPolicy
class CECPlayerActionPlayDefaultPolicy : public CECPlayerActionPlayPolicy{
public:
	CECPlayerActionPlayDefaultPolicy(CECPlayer *pPlayer, CECModel *pPlayerModel)
		: CECPlayerActionPlayPolicy(pPlayer, pPlayerModel)
	{
	}

	virtual bool PlayNonSkillActionWithName(int iAction, const char* szActName, bool bRestart, int nTransTime, bool bNoFx, bool *pActFlag, DWORD dwFlagMode){
#ifdef _DEBUG
		if (!IsPlayingAction(iAction) && GetModel() && GetModel()->GetComActByName(szActName)){
			LOG_DEBUG_INFO(AString().Format("PlayNonSkillActionWithName:%s", szActName));
		}
#endif

		//	强行清除 ActFlag 通知，以防被正播放的动作清掉，导致 CECAttackEvent 通知延时及带来的表现延迟
		//	如果需要以不通知的方式调用，即 ClearComActFlagAllRankNodes(false)，请在外部先调用，则此处调用无效
		//	下同
		ClearComActFlagAllRankNodes(true);
		return GetModel()
			&& GetModel()->PlayActionByName(szActName, 1.0f, bRestart, nTransTime, true, iAction, bNoFx, pActFlag, dwFlagMode);
	}
	virtual bool QueueNonSkillActionWithName(int iAction, const char* szActName, int nTransTime, bool bForceStopPrevAct, bool bNoFx, bool bResetSpeed, bool bResetActFlag, bool *pNewActFlag, DWORD dwNewFlagMode){
		LOG_DEBUG_INFO(AString().Format("QueueNonSkillActionWithName:%s", szActName));
		return GetModel()
			&& GetModel()->QueueAction(szActName, nTransTime, iAction, bForceStopPrevAct, false, bNoFx, bResetSpeed, bResetActFlag, pNewActFlag, dwNewFlagMode);
	}
	virtual bool PlaySkillCastActionWithName(int idSkill, const char *szActName, bool bNoFX){
		if (IsMoving()){
			return false;
		}
		LOG_DEBUG_INFO(AString().Format("PlaySkillCastActionWithName:%s", szActName));
		ClearComActFlagAllRankNodes(true);
		return GetModel()
			&& GetModel()->PlayActionByName(szActName, 1.0f, true, 200, true, CECPlayer::ACT_CASTSKILL, bNoFX);
	}
	virtual bool PlaySkillAttackActionWithName(int idSkill, const char *szActName, bool bNoFX, bool *pActFlag, DWORD dwFlagMode){
		if (IsMoving()){
			return false;
		}
		LOG_DEBUG_INFO(AString().Format("PlaySkillAttackActionWithName:%s", szActName));
		ClearComActFlagAllRankNodes(true);
		return GetModel()
			&& GetModel()->PlayActionByName(szActName, 1.0f, true, 200, true, CECPlayer::ACT_CASTSKILL, bNoFX, pActFlag, dwFlagMode);
	}
	virtual bool QueueSkillAttackActionWithName(int idSkill, const char *szActName, int nTransTime, bool bNoFX, bool bResetSpeed, bool bResetActFlag, bool *pNewActFlag, DWORD dwNewFlagMode){
		LOG_DEBUG_INFO(AString().Format("QueueSkillAttackActionWithName:%s", szActName));
		return GetModel()
			&& GetModel()->QueueAction(szActName, nTransTime, CECPlayer::ACT_CASTSKILL, false, false, bNoFX, bResetSpeed, bResetActFlag, pNewActFlag, dwNewFlagMode);
	}
	virtual bool PlayWoundActionWithName(const char* szActName){
		LOG_DEBUG_INFO(AString().Format("PlayWoundActionWithName:%s", szActName));
		return GetModel()
			&& GetModel()->GetA3DSkinModel()->PlayActionByName(szActName, CECModel::ACTCHA_WOUND, 1, 0, false);
	}
	virtual void ClearComActFlagAllRankNodes(bool bSignalCurrent){
		if (GetModel()){
			GetModel()->ClearComActFlagAllRankNodes(bSignalCurrent);
		}
	}
	virtual void StopChannelAction(){
		if (GetModel()){
			ClearComActFlagAllRankNodes(true);
			GetModel()->StopChannelAction(0, true);
		}
	}
	virtual void StopSkillAction(){
		if (GetModel() && IsPlayingCastingSkillAction()){
			StopChannelAction();
		}
	}
	virtual bool IsPlayingAction(int iAction)const{
		if (GetModel()){
			return (int)GetModel()->GetCurActionUserData() == iAction;
		}
		return false;
	}
	virtual bool IsPlayingMoveAction()const{
		if (GetModel()){
			return IsMoveAction(GetModel()->GetCurActionUserData());
		}
		return false;
	}
	virtual int GetLowerBodyAction()const{
		if (GetModel()){
			return (int)GetModel()->GetCurActionUserData();
		}
		return -1;
	}
};

//	class CECPlayerActionPlaySupportCastSkillWhenMovePolicy
class CECPlayerActionPlaySupportCastSkillWhenMovePolicy : public CECPlayerActionPlayPolicy{
	enum{
		ACT_RANK_LOW	= 0,	//	低优先级通道
		ACT_RANK_HIGH	= 1,	//	高优先级通道
	};

	//	查询当前动作 UserData
	DWORD GetUserDataForChannel(int channel)const{
		return GetModel()->GetCurActionUserData(channel);
	}
	DWORD GetUpperBodyChannelUserData()const{
		return GetUserDataForChannel(CECPlayerActionController::ACT_CHANNEL_UPPERBODY);
	}
	DWORD GetLowerBodyChannelUserData()const{
		return GetUserDataForChannel(CECPlayerActionController::ACT_CHANNEL_LOWERBODY);
	}

	//	判断当前动作 UserData
	int  GetPlayingAction(DWORD dwUserData)const{
		if (dwUserData == (DWORD)-1){
			return -1;	//	当前不在播放动作时，统一返回 -1，与 CECPlayerActionPlayDefaultPolicy::GetLowerBodyAction() 保持一致
		}else{
			return (int)(dwUserData & 0x0000ffff);		//	高16位用于存储技能ID
		}
	}
	int  GetUpperBodyPlayingAction()const{
		return GetPlayingAction(GetUpperBodyChannelUserData());
	}
	int  GetLowerBodyPlayingAction()const{
		return GetPlayingAction(GetLowerBodyChannelUserData());
	}

	//	技能相关 UserData	
	int  GetSkillIDFromCastSkillActionUserData(DWORD dwUserData)const{
		return (dwUserData >> 16) & 0x0000ffff;
	}
	bool IsCastingSkillAction(DWORD dwUserData)const{
		return GetPlayingAction(dwUserData) == CECPlayer::ACT_CASTSKILL;
	}
	bool IsCastingSkill()const{
		return IsCastingSkillAction(GetUpperBodyChannelUserData())
			|| IsCastingSkillAction(GetLowerBodyChannelUserData());
	}
	int  GetCastingSkillID()const{
		int result(0);
		if (IsCastingSkillAction(GetUpperBodyChannelUserData())){
			result = GetSkillIDFromCastSkillActionUserData(GetUpperBodyChannelUserData());
		}else if (IsCastingSkillAction(GetLowerBodyChannelUserData())){
			result = GetSkillIDFromCastSkillActionUserData(GetLowerBodyChannelUserData());
		}
		return result;
	}

	//	构造当前动作 UserData
	DWORD MakeCastSkillActionUserData(int idSkill)const{
		return (idSkill << 16) | CECPlayer::ACT_CASTSKILL;
	}

public:
	CECPlayerActionPlaySupportCastSkillWhenMovePolicy(CECPlayer *pPlayer, CECModel *pPlayerModel)
		: CECPlayerActionPlayPolicy(pPlayer, pPlayerModel)
	{
		pPlayerModel->SetMainChannel(CECPlayerActionController::ACT_CHANNEL_UPPERBODY);	//	用于 PlayActionByName 中重置播放速度，避免设置了播放速度的动作影响后续动作
	}

	virtual bool CanCombineWithMoveForSkill(int idSkill=-1)const{
		if (idSkill < 0){
			idSkill = GetCastingSkillID();
		}
		return CECCastSkillWhenMove::Instance().HasActionSupport(idSkill, GetPlayer());
	}

	bool IsHostPlayer()const{
		return GetPlayer()->IsHostPlayer();
	}

	virtual bool PlayNonSkillActionWithName(int iAction, const char* szActName, bool bRestart, int nTransTime, bool bNoFx, bool *pActFlag, DWORD dwFlagMode){
		if (IsCastingSkill()){
			if (!IsMoveAction(iAction) || !CanCombineWithMoveForSkill()){
				StopChannelAction();
			}
		}
#ifdef _DEBUG
		if (IsHostPlayer() && !IsPlayingAction(iAction) && GetModel() && GetModel()->GetComActByName(szActName)){
			LOG_DEBUG_INFO(AString().Format("PlayNonSkillActionWithName:%s", szActName));
		}		
#endif
		return GetModel()->PlayActionByName(CECPlayerActionController::ACT_CHANNEL_UPPERBODY, szActName, 1.0f, bRestart, nTransTime, true, iAction, bNoFx, NULL, dwFlagMode)
			&& GetModel()->PlayActionByName(CECPlayerActionController::ACT_CHANNEL_LOWERBODY, szActName, 1.0f, bRestart, nTransTime, true, iAction, true, pActFlag, dwFlagMode);
	}
	virtual bool QueueNonSkillActionWithName(int iAction, const char* szActName, int nTransTime, bool bForceStopPrevAct, bool bNoFx, bool bResetSpeed, bool bResetActFlag, bool *pNewActFlag, DWORD dwNewFlagMode){
#ifdef _DEBUG
		if (IsHostPlayer()){
			LOG_DEBUG_INFO(AString().Format("QueueNonSkillActionWithName:%s", szActName));
		}
#endif
		return GetModel()->QueueAction(CECPlayerActionController::ACT_CHANNEL_UPPERBODY, szActName, nTransTime, iAction, bForceStopPrevAct, false, bNoFx, bResetSpeed, bResetActFlag, NULL, dwNewFlagMode)
			&& GetModel()->QueueAction(CECPlayerActionController::ACT_CHANNEL_LOWERBODY, szActName, nTransTime, iAction, bForceStopPrevAct, false, true,  bResetSpeed, bResetActFlag, pNewActFlag, dwNewFlagMode);
	}
	virtual bool PlaySkillCastActionWithName(int idSkill, const char *szActName, bool bNoFX){
		if (IsMoving()){
			if (!CanCombineWithMoveForSkill(idSkill)){	//	动作不支持，则直接忽略技能动作播放
				return false;
			}//else 直接播放叠加
		}else{
			StopChannelAction();	//	清除其它动作避免产生叠加效果
		}		
#ifdef _DEBUG
		if (IsHostPlayer()){
			LOG_DEBUG_INFO(AString().Format("PlaySkillCastActionWithName:%s", szActName));
		}
#endif
		return GetModel()->PlayActionByName(CECPlayerActionController::ACT_CHANNEL_UPPERBODY, szActName, 1.0f, true, 200, true, MakeCastSkillActionUserData(idSkill), bNoFX)
			&& GetModel()->PlayActionByName(CECPlayerActionController::ACT_CHANNEL_LOWERBODY, szActName, 1.0f, true, 200, true, MakeCastSkillActionUserData(idSkill), true);
	}
	virtual bool PlaySkillAttackActionWithName(int idSkill, const char *szActName, bool bNoFX, bool *pActFlag, DWORD dwFlagMode){
		if (IsMoving()){
			if (!CanCombineWithMoveForSkill(idSkill)){	//	动作不支持，则直接忽略技能动作播放
				return false;
			}//else 直接播放叠加
		}else{
			StopChannelAction();	//	清除其它动作避免产生叠加效果
		}		
#ifdef _DEBUG
		if (IsHostPlayer()){
			LOG_DEBUG_INFO(AString().Format("PlaySkillAttackActionWithName:%s", szActName));
		}
#endif
		return GetModel()->PlayActionByName(CECPlayerActionController::ACT_CHANNEL_UPPERBODY, szActName, 1.0f, true, 200, true, MakeCastSkillActionUserData(idSkill), bNoFX, pActFlag, dwFlagMode)
			&& GetModel()->PlayActionByName(CECPlayerActionController::ACT_CHANNEL_LOWERBODY, szActName, 1.0f, true, 200, true, MakeCastSkillActionUserData(idSkill), true,  NULL, dwFlagMode);
	}
	virtual bool QueueSkillAttackActionWithName(int idSkill, const char *szActName, int nTransTime, bool bNoFX, bool bResetSpeed, bool bResetActFlag, bool *pNewActFlag, DWORD dwNewFlagMode){
#ifdef _DEBUG
		if (IsHostPlayer()){
			LOG_DEBUG_INFO(AString().Format("QueueSkillAttackActionWithName:%s", szActName));
		}
#endif
		return GetModel()->QueueAction(CECPlayerActionController::ACT_CHANNEL_UPPERBODY, szActName, nTransTime, MakeCastSkillActionUserData(idSkill), false, false, bNoFX, bResetSpeed, bResetActFlag, pNewActFlag, dwNewFlagMode)
			&& GetModel()->QueueAction(CECPlayerActionController::ACT_CHANNEL_LOWERBODY, szActName, nTransTime, MakeCastSkillActionUserData(idSkill), false, false, true,  bResetSpeed, bResetActFlag, NULL, dwNewFlagMode);
	}

	virtual bool PlayWoundActionWithName(const char* szActName){
#ifdef _DEBUG
		if (IsHostPlayer()){
			LOG_DEBUG_INFO(AString().Format("PlayWoundActionWithName:%s", szActName));
		}
#endif
		return GetModel()->GetA3DSkinModel()->PlayActionByName(szActName, CECPlayerActionController::ACT_CHANNEL_WOUND, 1, 0, false);
	}	
	virtual void ClearComActFlagAllRankNodes(bool bSignalCurrent){
		if (GetModel()){
			GetModel()->ClearComActFlagAllRankNodes(CECPlayerActionController::ACT_CHANNEL_UPPERBODY, bSignalCurrent);
			GetModel()->ClearComActFlagAllRankNodes(CECPlayerActionController::ACT_CHANNEL_LOWERBODY, bSignalCurrent);
		}
	}
	virtual void StopChannelAction(){
		if (GetModel()){
			ClearComActFlagAllRankNodes(true);
			GetModel()->StopChannelAction(CECPlayerActionController::ACT_CHANNEL_UPPERBODY, true);
			GetModel()->StopChannelAction(CECPlayerActionController::ACT_CHANNEL_LOWERBODY, true);
		}
	}	
	virtual void StopSkillAction(){
		if (GetModel() && IsPlayingCastingSkillAction()){
			ClearComActFlagAllRankNodes(true);
			if (IsPlayingMoveAction()){
				GetModel()->StopChannelAction(CECPlayerActionController::ACT_CHANNEL_UPPERBODY, ACT_RANK_HIGH, true, false);
				GetModel()->StopChannelAction(CECPlayerActionController::ACT_CHANNEL_LOWERBODY, ACT_RANK_LOW, true, false);
			}else{
				GetModel()->StopChannelAction(CECPlayerActionController::ACT_CHANNEL_UPPERBODY, ACT_RANK_LOW, true, false);
				GetModel()->StopChannelAction(CECPlayerActionController::ACT_CHANNEL_LOWERBODY, ACT_RANK_HIGH, true, false);
			}
		}
	}
	virtual bool IsPlayingAction(int iAction)const{
		return GetUpperBodyPlayingAction() == iAction
			|| GetLowerBodyPlayingAction() == iAction;
	}
	virtual bool IsPlayingMoveAction()const{
		return IsMoveAction(GetLowerBodyPlayingAction())
			|| IsMoveAction(GetUpperBodyPlayingAction());
	}
	virtual int GetLowerBodyAction()const{
		return GetLowerBodyPlayingAction();
	}
};

//	CECPlayerActionController

CECPlayerActionController::CECPlayerActionController()
: m_pPlayer(NULL)
, m_pPlayerModel(NULL)
, m_bSupportCastSkillWhenMove(false)
, m_actionPlayPolicy(NULL)
, m_bSkillAttackActionPlayed(false)
{
}

CECPlayerActionController::~CECPlayerActionController(){
	ReleaseActionPlayPolicy();
}

void CECPlayerActionController::Bind(CECPlayer *player, CECModel *playerModel){
	if (!player){
		return;
	}
	if (player == m_pPlayer && playerModel == m_pPlayerModel){
		return;
	}
	ReleaseActionPlayPolicy();
	m_pPlayer = player;
	m_pPlayerModel = playerModel;
	if (CECCastSkillWhenMove::Instance().HasModelSupport(player)){
		if (BuildChannelForCastSkillWhenMove()){
			m_bSupportCastSkillWhenMove = true;
		}else{
			m_bSupportCastSkillWhenMove = false;
		}
	}else{
		m_bSupportCastSkillWhenMove = false;
	}
	if (!m_bSupportCastSkillWhenMove){
		if (m_pPlayerModel){
			m_pPlayerModel->BuildWoundActionChannel();
		}
	}
	InitializeActionPlayPolicy();
}

bool CECPlayerActionController::SupportCastSkillWhenMove()const{
	return m_bSupportCastSkillWhenMove;
}

bool CECPlayerActionController::CanCombineWithMoveForSkill(int idSkill)const{
	return m_actionPlayPolicy != NULL && m_actionPlayPolicy->CanCombineWithMoveForSkill(idSkill);
}

void CECPlayerActionController::InitializeActionPlayPolicy(){
	if (m_bSupportCastSkillWhenMove){
		m_actionPlayPolicy = new CECPlayerActionPlaySupportCastSkillWhenMovePolicy(m_pPlayer, m_pPlayerModel);
	}else{
		m_actionPlayPolicy = new CECPlayerActionPlayDefaultPolicy(m_pPlayer, m_pPlayerModel);
	}
}

void CECPlayerActionController::ReleaseActionPlayPolicy(){
	if (m_actionPlayPolicy){
		delete m_actionPlayPolicy;
		m_actionPlayPolicy = NULL;
	}
}

bool CECPlayerActionController::BuildChannelForCastSkillWhenMove(){
	//	构建 CastSkillWhenMove 所需 Channel	
	if (m_pPlayerModel == NULL){
		return false;
	}
	Bones upperBodyBones = GetUpperBodyBones();
	if (upperBodyBones.empty()){
		return false;
	}
	Bones lowerBodyBones = GetLowerBodyBones();
	if (lowerBodyBones.empty()){
		return false;
	}
	Bones woundChannelBones = GetWoundChannelBones();
	if (woundChannelBones.empty()){
		return false;
	}
	if (upperBodyBones.size() + lowerBodyBones.size() != woundChannelBones.size()){
		a_LogOutput(1, "CECPlayerActionController::BuildChannelForCastSkillWhenMove, upperbody bones and lower bones do not compose whole bone");
		return false;
	}
	if (!m_pPlayerModel->BuildActionChannel(ACT_CHANNEL_UPPERBODY, upperBodyBones.size(), upperBodyBones.begin()) ||
		!m_pPlayerModel->BuildActionChannel(ACT_CHANNEL_LOWERBODY, lowerBodyBones.size(), lowerBodyBones.begin()) ||
		!m_pPlayerModel->BuildActionChannel(ACT_CHANNEL_WOUND, woundChannelBones.size(), woundChannelBones.begin())){
		return false;
	}
	m_pPlayerModel->GetActionChannel(ACT_CHANNEL_WOUND)->SetPlayMode(A3DSMActionChannel::PLAY_COMBINE);
	return true;
}

CECPlayerActionController::Bones CECPlayerActionController::GetUpperBodyBones()const{	
	const char ** szBonesName = NULL;
	int	bonesNameCount(0);
	CECCastSkillWhenMove::Instance().GetUpperBodyBonesName(m_pPlayer, bonesNameCount, szBonesName);
	return GetBonesIDFromName(bonesNameCount, szBonesName);
}

CECPlayerActionController::Bones CECPlayerActionController::GetLowerBodyBones()const{
	const char ** szBonesName = NULL;
	int	bonesNameCount(0);
	CECCastSkillWhenMove::Instance().GetLowerBodyBonesName(m_pPlayer, bonesNameCount, szBonesName);
	return GetBonesIDFromName(bonesNameCount, szBonesName);
}

CECPlayerActionController::Bones CECPlayerActionController::GetWoundChannelBones()const{
	std::vector<int> result;
	for (int i(0); i < m_pPlayerModel->GetA3DSkinModel()->GetSkeleton()->GetBoneNum(); ++ i){
		result.push_back(i);
	}
	return result;
}

bool CECPlayerActionController::PlayNonSkillActionWithName(int iAction, const char* szActName, bool bRestart/* =true */, int nTransTime/* =200 */, bool bNoFx/* =false */, bool *pActFlag/* =NULL */, DWORD dwFlagMode/* =COMACT_FLAG_MODE_NONE */){
	return m_actionPlayPolicy
		&& m_actionPlayPolicy->PlayNonSkillActionWithName(iAction, szActName, bRestart, nTransTime, bNoFx, pActFlag, dwFlagMode);
}

bool CECPlayerActionController::QueueNonSkillActionWithName(int iAction, const char* szActName, int nTransTime/* =200 */, bool bForceStopPrevAct/* =false */, bool bNoFx/* =false */, bool bResetSpeed/* =false */, bool bResetActFlag/* =false */, bool *pNewActFlag/* =NULL */, DWORD dwNewFlagMode/* =COMACT_FLAG_MODE_NONE */){
	return m_actionPlayPolicy
		&& m_actionPlayPolicy->QueueNonSkillActionWithName(iAction, szActName, nTransTime, bForceStopPrevAct, bNoFx, bResetSpeed, bResetActFlag, pNewActFlag, dwNewFlagMode);
}

bool CECPlayerActionController::PlaySkillCastActionWithName(int idSkill, const char *szActName, bool bNoFX/* =false */){
	if (m_actionPlayPolicy &&
		m_actionPlayPolicy->PlaySkillCastActionWithName(idSkill, szActName, bNoFX)){
		m_bSkillAttackActionPlayed = false;
		return true;
	}
	return false;
}

bool CECPlayerActionController::PlaySkillAttackActionWithName(int idSkill, const char *szActName, bool bNoFX/* =false */, bool *pActFlag/* =NULL */, DWORD dwFlagMode/* =COMACT_FLAG_MODE_NONE */){
	if (m_actionPlayPolicy &&
		m_actionPlayPolicy->PlaySkillAttackActionWithName(idSkill, szActName, bNoFX, pActFlag, dwFlagMode)){
		m_bSkillAttackActionPlayed = true;
		return true;
	}
	return false;
}

bool CECPlayerActionController::QueueSkillAttackActionWithName(int idSkill, const char *szActName, int nTransTime/* =200 */, bool bNoFX/* =false */, bool bResetSpeed/* =false */, bool bResetActFlag/* =false */, bool *pNewActFlag/* =NULL */, DWORD dwNewFlagMode/* =COMACT_FLAG_MODE_NONE */){
	return m_actionPlayPolicy
		&& m_actionPlayPolicy->QueueSkillAttackActionWithName(idSkill, szActName, nTransTime, bNoFX, bResetSpeed, bResetActFlag, pNewActFlag, dwNewFlagMode);
}

bool CECPlayerActionController::PlayWoundActionWithName(const char* szActName){
	return m_actionPlayPolicy
		&& m_actionPlayPolicy->PlayWoundActionWithName(szActName);
}

void CECPlayerActionController::ClearComActFlagAllRankNodes(bool bSignalCurrent){
	if (m_actionPlayPolicy){
		m_actionPlayPolicy->ClearComActFlagAllRankNodes(bSignalCurrent);
	}
}

void CECPlayerActionController::StopChannelAction(){
	if (m_actionPlayPolicy){
		m_actionPlayPolicy->StopChannelAction();
	}
}

void CECPlayerActionController::StopSkillCastAction(){
	if (m_actionPlayPolicy && m_actionPlayPolicy->IsPlayingCastingSkillAction() && !m_bSkillAttackActionPlayed){
		m_actionPlayPolicy->StopSkillAction();
		LOG_DEBUG_INFO(AString().Format("CECPlayerActionController::StopSkillCastAction(%s)", m_pPlayer->IsHostPlayer() ? "HostPlayer" : "ElsePlayer"));
	}
}

void CECPlayerActionController::StopSkillAttackAction(){
	if (m_actionPlayPolicy && m_actionPlayPolicy->IsPlayingCastingSkillAction() && m_bSkillAttackActionPlayed){
		m_actionPlayPolicy->StopSkillAction();
		LOG_DEBUG_INFO(AString().Format("CECPlayerActionController::StopSkillAttackAction(%s)", m_pPlayer->IsHostPlayer() ? "HostPlayer" : "ElsePlayer"));
	}
}

bool CECPlayerActionController::IsPlayingAction(int iAction)const{
	return m_actionPlayPolicy ? m_actionPlayPolicy->IsPlayingAction(iAction) : false;
}

bool CECPlayerActionController::IsPlayingCastingSkillAction()const{
	return m_actionPlayPolicy ? m_actionPlayPolicy->IsPlayingCastingSkillAction() : false;
}

bool CECPlayerActionController::IsPlayingMoveAction()const{
	return m_actionPlayPolicy ? m_actionPlayPolicy->IsPlayingMoveAction() : false;
}

int CECPlayerActionController::GetLowerBodyAction()const{
	return m_actionPlayPolicy ? m_actionPlayPolicy->GetLowerBodyAction() : -1;
}

CECPlayerActionController::Bones CECPlayerActionController::GetBonesIDFromName( int bonesNameCount, const char ** szBonesName) const
{
	CECPlayerActionController::Bones result;
	A3DSkeleton *pSkeleton = m_pPlayerModel->GetA3DSkinModel()->GetSkeleton();
	for (int i(0); i < bonesNameCount; ++ i){
		int bone(-1);
		if (!pSkeleton->GetBone(szBonesName[i], &bone)){
			a_LogOutput(1,
				"CECPlayerActionController::GetBonesIDFromName, Failed to get bone %s for %s",
				szBonesName[i],
				pSkeleton->GetA3DSkinModel()->GetFileName());
			return false;
		}else{
			result.push_back(bone);
		}
	}
	return result;
}
// File		: EC_Reconnect.cpp
// Creator	: Xu Wenbin
// Date		: 2014/10/8

#include "EC_Reconnect.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_ServerList.h"
#include "EC_LoginSwitch.h"
#include "EC_CommandLine.h"
#include "Arc/arc_overlay.h"

#include "EC_UIManager.h"
#include "EC_LoginUIMan.h"

//	class CECReconnectPolicy
ACString CECReconnectPolicy::GetCurrentUser()const{
	return AS2AC(g_pGame->GetGameSession()->GetUserName());
}
bool CECReconnectPolicy::MatchCurrentUser(const ACString &user)const{
	return user == GetCurrentUser();
}
bool CECReconnectPolicy::MatchCurrentUser()const{
	return MatchCurrentUser(GetUser());
}

//	class CECReconnectSimpleUserPasswordPolicy
CECReconnectSimpleUserPasswordPolicy::CECReconnectSimpleUserPasswordPolicy(const ACString &user, const ACString &password)
: m_user(user)
, m_password(password)
{
}
ACString CECReconnectSimpleUserPasswordPolicy::GetUser()const{
	return m_user;
}
bool CECReconnectSimpleUserPasswordPolicy::CanRequestReconnect()const{
	return MatchCurrentUser();
}
bool CECReconnectSimpleUserPasswordPolicy::RequestReconnect(){
	return MatchCurrentUser();
}
bool CECReconnectSimpleUserPasswordPolicy::CanReconnectNow()const{
	return MatchCurrentUser();
}
void CECReconnectSimpleUserPasswordPolicy::UseLoginParameter(){
	CECLoginSwitch::Instance().PasswordLogin(m_user, m_password);
}

//	class CECReconnectUserTokenPolicy
CECReconnectUserTokenPolicy::CECReconnectUserTokenPolicy(const ACString &user)
: m_user(user)
{
}
ACString CECReconnectUserTokenPolicy::GetUser()const{
	return m_user;
}
bool CECReconnectUserTokenPolicy::SetUserToken(const ACString &user, const ACString &token){
	if (user != GetUser()){
		a_LogOutput(1, "CECReconnectUserTokenPolicy::SetUserToken, refused cause user not match(given user=%s, user should be %s)", AC2AS(user), AC2AS(GetUser()));
		return false;
	}
	m_token = token;
	a_LogOutput(1, "CECReconnectUserTokenPolicy::SetUserToken, token=%s", AC2AS(token));
	return true;
}
bool CECReconnectUserTokenPolicy::CanReconnectNow()const{
	return MatchCurrentUser() && !m_token.IsEmpty();
}
void CECReconnectUserTokenPolicy::UseLoginParameter(){
	CECLoginSwitch::Instance().TokenLogin(m_user, m_token);
}

//	class CECReconnectForeignArcUserTokenPolicy
CECReconnectForeignArcUserTokenPolicy::CECReconnectForeignArcUserTokenPolicy(const ACString &user)
: CECReconnectUserTokenPolicy(user)
{
}

bool CECReconnectForeignArcUserTokenPolicy::CanRequestReconnect()const{
	bool result(false);
	if (!MatchCurrentUser()){
		a_LogOutput(1, "CECReconnectForeignArcUserTokenPolicy::CanRequestReconnect, local user(%s) not match current user(%s)", AC2AS(GetUser()), AC2AS(GetCurrentUser()));
	}else{
		ACString strArcUser;
		if (CC_SDK::ArcOverlay::Instance().GetAccountName(strArcUser)){
			if (!MatchCurrentUser(strArcUser)){
				a_LogOutput(1, "CECReconnectForeignArcUserTokenPolicy::CanRequestReconnect, arc user (%s) not match current user (%s)", AC2AS(strArcUser), AC2AS(GetCurrentUser()));
			}else{
				result = true;
			}
		}
	}
	return result;
}
bool CECReconnectForeignArcUserTokenPolicy::RequestReconnect(){
	bool result(false);
	if (CanRequestReconnect()){
		ACString strToken;
		if (!CC_SDK::ArcOverlay::Instance().GetToken(GetUser(), strToken)){
			a_LogOutput(1, "CECReconnectForeignArcUserTokenPolicy::RequestReconnect, failed to get token");
		}else{
			result = SetUserToken(GetUser(), strToken);
		}
	}
	return result;
}

//	class CECReconnectState
void CECReconnectState::SetServerIndex(int serverIndex){
	a_LogOutput(1, "CECReconnectState::SetServerIndex ignored at state %s", GetName());
}
void CECReconnectState::SetRoleID(int roleID){	
	a_LogOutput(1, "CECReconnectState::SetRoleID ignored at state %s", GetName());
}
void CECReconnectState::SetReconnectPolicyCandidate(CECReconnectPolicy *policy){
	delete policy;
	a_LogOutput(1, "CECReconnectState::SetReconnectPolicyCandidate ignored at state %s", GetName());
}
void CECReconnectState::AcceptReconnectPolicyCandidate(){
	a_LogOutput(1, "CECReconnectState::AcceptReconnectPolicyCandidate ignored at state %s", GetName());
}
bool CECReconnectState::RequestReconnect(){
	a_LogOutput(1, "CECReconnectState::RequestReconnect ignored at state %s", GetName());
	return false;
}
bool CECReconnectState::ApplyServerSetting(){
	a_LogOutput(1, "CECReconnectState::ApplyServerSetting ignored at state %s", GetName());
	return false;
}
void CECReconnectState::UseLoginParameter(){
	a_LogOutput(1, "CECReconnectState::UseLoginParameter ignored at state %s", GetName());
}
void CECReconnectState::OnReconnectSuccess(){
	a_LogOutput(1, "CECReconnectState::OnReconnectSuccess ignored at state %s", GetName());
}
void CECReconnectState::OnReconnectFail(){
	a_LogOutput(1, "CECReconnectState::OnReconnectFail ignored at state %s", GetName());
}

//	class CECReconnectStateNone
void CECReconnectStateNone::Enter(CECReconnectState *prev){
	GetParent()->ResetStateData();
}
void CECReconnectStateNone::SetServerIndex(int serverIndex){
	GetParent()->SetServerIndexImpl(serverIndex);
	TestAndGotoStateCanRequest();
}
void CECReconnectStateNone::SetRoleID(int roleID){
	GetParent()->SetRoleIDImpl(roleID);
	TestAndGotoStateCanRequest();
}
void CECReconnectStateNone::SetReconnectPolicyCandidate(CECReconnectPolicy *policy){
	GetParent()->SetReconnectPolicyCandidateImpl(policy);
}
void CECReconnectStateNone::AcceptReconnectPolicyCandidate(){
	GetParent()->AcceptReconnectPolicyCandidateImpl();
	TestAndGotoStateCanRequest();
}
void CECReconnectStateNone::TestAndGotoStateCanRequest(){
	if (CanRequest()){
		GetParent()->ChangeState(GetParent()->GetStateCanRequest());
	}
}
bool CECReconnectStateNone::CanRequest()const{
	return GetParent()->GetServerIndex() >= 0 && GetParent()->GetServerIndex() < CECServerList::Instance().GetServerCount()
		&& (g_pGame->GetGameRun()->GetGameState() != CECGameRun::GS_GAME || GetParent()->GetRoleIDToSelect() != 0)
		&& GetParent()->m_reconnectPolicy != NULL;
}

//	class CECReconnectStateCanRequest
void CECReconnectStateCanRequest::SetRoleID(int roleID){
	GetParent()->SetRoleIDImpl(roleID);
}
bool CECReconnectStateCanRequest::CanRequestReconnect()const{
	return GetParent()->CanRequestReconnectImpl();
}
bool CECReconnectStateCanRequest::RequestReconnect(){
	if (GetParent()->RequestReconnectImpl()){
		GetParent()->ChangeState(GetParent()->GetStateRequested());
		return true;
	}
	return false;
}

//	class CECReconnectStateRequested
void CECReconnectStateRequested::Tick(){
	if (GetParent()->CanReconnectNowImpl()){
		GetParent()->ChangeState(GetParent()->GetStateReconnecting());
		GetParent()->ReconnectImpl();
	}
}

//	class CECReconnectStateReconnecting
bool CECReconnectStateReconnecting::CanRequestReconnect()const{	//	重连状态下也允许重连，以允许更多合理重连发生：如从游戏中退出到登录界面后登录时失败、可再重连
	return GetParent()->CanRequestReconnectImpl();
}
bool CECReconnectStateReconnecting::RequestReconnect(){
	if (GetParent()->RequestReconnectImpl()){
		GetParent()->ChangeState(GetParent()->GetStateRequested());
		return true;
	}
	return false;
}
bool CECReconnectStateReconnecting::ApplyServerSetting(){
	return GetParent()->ApplyServerSettingImpl();
}
bool CECReconnectStateReconnecting::IsReconnectFromGame()const{
	return GetParent()->IsReconnectFromGameImpl();
}
void CECReconnectStateReconnecting::UseLoginParameter(){
	GetParent()->UseLoginParameterImpl();
}
void CECReconnectStateReconnecting::OnReconnectFail(){
	a_LogOutput(1, "CECReconnectStateReconnecting::OnReconnectFail");
	GetParent()->ResetState();
}
void CECReconnectStateReconnecting::OnReconnectSuccess(){
	a_LogOutput(1, "CECReconnectStateReconnecting::OnReconnectSuccess");
	GetParent()->ChangeState(GetParent()->GetStateCanRequest());
}

//	class CECReconnect

ELEMENTCLIENT_DEFINE_SINGLETON_NO_CTOR(CECReconnect)

CECReconnect::CECReconnect()
: m_serverIndex(-1)
, m_curState(NULL)
, m_roleID(0)
, m_reconnectPolicyCandidate(NULL)
, m_reconnectPolicy(NULL)
, m_reconnectFromGame(false)
{
	GetStateNone()->Init(this);
	GetStateCanRequest()->Init(this);
	GetStateRequested()->Init(this);
	GetStateReconnecting()->Init(this);
	
	ChangeState(GetStateNone());
}

CECReconnect::~CECReconnect(){
	if (m_reconnectPolicyCandidate){
		delete m_reconnectPolicyCandidate;
		m_reconnectPolicyCandidate = NULL;
	}
	if (m_reconnectPolicy){
		delete m_reconnectPolicy;
		m_reconnectPolicy = NULL;
	}
}

void CECReconnect::SetServerIndex(int serverIndex){
	m_curState->SetServerIndex(serverIndex);
}
void CECReconnect::SetRoleID(int roleID){
	m_curState->SetRoleID(roleID);
}
void CECReconnect::SetReconnectPolicyCandidate(CECReconnectPolicy *policy){
	m_curState->SetReconnectPolicyCandidate(policy);
}
void CECReconnect::AcceptReconnectPolicyCandidate(){
	m_curState->AcceptReconnectPolicyCandidate();
}
bool CECReconnect::CanRequestReconnect()const{
	return m_curState->CanRequestReconnect();
}
bool CECReconnect::RequestReconnect(){
	return m_curState->RequestReconnect();
}
void CECReconnect::Tick(){
	m_curState->Tick();
}
bool CECReconnect::ApplyServerSetting(){
	return m_curState->ApplyServerSetting();
}
void CECReconnect::UseLoginParameter(){
	m_curState->UseLoginParameter();
}
bool CECReconnect::IsReconnecting()const{
	return m_curState->IsReconnecting();
}
bool CECReconnect::IsReconnectFromGame()const{
	return m_curState->IsReconnectFromGame();
}
void CECReconnect::OnReconnectSuccess(){
	m_curState->OnReconnectSuccess();
}
void CECReconnect::OnReconnectFail(){
	m_curState->OnReconnectFail();
}
void CECReconnect::ResetState(){
	a_LogOutput(1, "CECReconnect::ResetState");
	ChangeState(GetStateNone());
	ResetStateData();			//	如果当前正在准备重连参数，则当前状态还是 StateNone，则 ResetStateData 不一定能够调用
}
void CECReconnect::ChangeState(CECReconnectState *state){
	if (state == m_curState){
		return;
	}
	CECReconnectState *prev = m_curState;
	m_curState = state;
	m_curState->Enter(prev);
	LOG_DEBUG_INFO(AString().Format("CECReconnect::ChangeState(%s)", state->GetName()));
}

void CECReconnect::SetServerIndexImpl(int serverIndex){
	m_serverIndex = serverIndex;
}
void CECReconnect::SetRoleIDImpl(int roleID){
	m_roleID = roleID;
}
void CECReconnect::SetReconnectPolicyCandidateImpl(CECReconnectPolicy *policy){
	if (m_reconnectPolicyCandidate != policy){
		delete m_reconnectPolicyCandidate;
		m_reconnectPolicyCandidate = policy;
	}
}
void CECReconnect::AcceptReconnectPolicyCandidateImpl(){
	if (m_reconnectPolicyCandidate != m_reconnectPolicy){
		delete m_reconnectPolicy;
		m_reconnectPolicy = m_reconnectPolicyCandidate;
		m_reconnectPolicyCandidate = NULL;
	}
}
bool CECReconnect::CanRequestReconnectImpl()const{
	return !g_pGame->GetGameSession()->IsConnected()
		&& m_reconnectPolicy != NULL && m_reconnectPolicy->CanRequestReconnect();
}
bool CECReconnect::RequestReconnectImpl(){
	return m_reconnectPolicy != NULL && m_reconnectPolicy->RequestReconnect();
}
bool CECReconnect::CanReconnectNowImpl()const{
	return m_reconnectPolicy != NULL && m_reconnectPolicy->CanReconnectNow();
}
void CECReconnect::ReconnectImpl(){
	m_reconnectFromGame = g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME;
	switch (g_pGame->GetGameRun()->GetGameState()){
	case CECGameRun::GS_GAME:
		g_pGame->GetGameRun()->SetLogoutFlag(2);
		break;
	case CECGameRun::GS_LOGIN:
		g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan()->CancelCustomize();
		g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan()->ReclickLoginButton();
		break;
	}
}
bool CECReconnect::ApplyServerSettingImpl(){
	CECServerList::Instance().SelectServer(GetServerIndex());
	if (!CECServerList::Instance().ApplyServerSetting()){
		a_LogOutput(1, "CECReconnect::ApplyServerSettingImpl, Invalid server index:%d", CECServerList::Instance().GetSelected());
		return false;
	}
	return true;
}
void CECReconnect::UseLoginParameterImpl(){
	if (m_reconnectPolicy != NULL){
		m_reconnectPolicy->UseLoginParameter();
	}
}
bool CECReconnect::IsReconnectFromGameImpl()const{
	return m_reconnectFromGame;
}
void CECReconnect::ResetStateData(){
	SetServerIndexImpl(-1);
	SetRoleIDImpl(0);
	SetReconnectPolicyCandidateImpl(NULL);
	AcceptReconnectPolicyCandidateImpl();
	m_reconnectFromGame = false;
}

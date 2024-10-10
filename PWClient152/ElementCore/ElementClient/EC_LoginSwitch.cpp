// File		: EC_LoginSwitch.cpp
// Creator	: Xu Wenbin
// Date		: 2012/5/17

#include "EC_LoginSwitch.h"
#include "EC_CommandLine.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_LoginUIMan.h"
#include "EC_GameSession.h"
#include "EC_CrossServer.h"
#include "Arc/Asia/EC_ArcAsia.h"
#include "EC_Configs.h"
#include "EC_Reconnect.h"
#include "Arc/arc_overlay.h"

#include "DlgLogin.h"

static CDlgLogin * GetLoginDialog()
{
	CDlgLogin *pDlgLogin = NULL;

	while (true)
	{
		if (!g_pGame)	break;
		
		CECGameRun *pGameRun = g_pGame->GetGameRun();
		if (!pGameRun)	break;
		
		CECUIManager *pUIMan = pGameRun->GetUIManager();
		if (!pUIMan) break;
		
		CECLoginUIMan *pLoginUIMan = pUIMan->GetLoginUIMan();
		if (!pLoginUIMan) break;
		
		pDlgLogin = dynamic_cast<CDlgLogin *>(pLoginUIMan->GetDialog("Win_Login"));
		break;
	}

	return pDlgLogin;
}

CECLoginSwitch::CECLoginSwitch()
: m_loginType(LT_DEFAULT)
{
}

CECLoginSwitch & CECLoginSwitch::Instance()
{
	static CECLoginSwitch s_dummy;
	return s_dummy;
}

void CECLoginSwitch::CheckAutoLogin()
{
	if (g_pGame->GetConfigs()->GetEnableArcAsia() && CECArcAsia::GetSingleton().IsArcInit()) {
		ArcAsiaLogin();
	}else{
		ParseCommandLine();
	}
}

void CECLoginSwitch::DefaultLogin()
{
	m_loginType = LT_DEFAULT;
	
	m_strUser.Empty();
	m_strPassword.Empty();
	m_strToken.Empty();
	m_strAgent.Empty();
}

void CECLoginSwitch::PasswordLogin(const ACString &user, const ACString &pwd)
{
	DefaultLogin();
	m_loginType = LT_PWD;
	m_strUser = user;
	m_strPassword = pwd;
}

void CECLoginSwitch::TokenLogin(const ACString &user, const ACString &token)
{
	DefaultLogin();
	m_loginType = LT_TOKEN;
	m_strUser = user;
	m_strToken = token;
}

void CECLoginSwitch::Token2Login(const ACString &user, const ACString &token2)
{
	DefaultLogin();
	m_loginType = LT_TOKEN2;
	m_strUser = user;
	m_strToken = token2;
}

void CECLoginSwitch::SSOLogin(const ACString &user, const ACString &pwd, const ACString &agent)
{
	DefaultLogin();
	m_loginType = LT_SSO;
	m_strUser = user;
	m_strPassword = pwd;
	m_strAgent = agent;
}

void CECLoginSwitch::CrossServerLogin(const ACString &user)
{
	DefaultLogin();
	m_loginType = LT_CROSSSERVER;
	m_strUser = user;
}

void CECLoginSwitch::ArcAsiaLogin()
{
	DefaultLogin();
	m_loginType = LT_ARC;
}

void CECLoginSwitch::ArcAsiaGetToken()
{
	CECArcAsia& arc = CECArcAsia::GetSingleton();	
	if (arc.IsArcInit() && IsUseArc()) {
		ACString strUser, strPassword, strAgent;
		arc.GetToken(strUser, strPassword, strAgent);		
		if (!strUser.IsEmpty() && !strPassword.IsEmpty() && !strAgent.IsEmpty()) {
			m_strUser = strUser;
			m_strPassword = strPassword;
			m_strAgent = strAgent;
		}
	}
}

void CECLoginSwitch::ParseCommandLine()
{
	//	从命令中自动分析登录方式并设置参数

	//	先处理成默认登录方式
	DefaultLogin();
	
	//	从命令行获取各参数
	ACString strUser = CECCommandLine::GetUser();
	ACString strPassword = CECCommandLine::GetPassword();
	ACString strAgent = CECCommandLine::GetAgent();
	ACString strToken = CECCommandLine::GetToken();
	char tokenType = CECCommandLine::GetTokenType();

	//	检查 SSO 登录
	if (!strUser.IsEmpty() && !strPassword.IsEmpty() && !strAgent.IsEmpty())
	{
		a_LogOutput(1, "CECLoginSwitch::ParseCommandLine, using sso");
		SSOLogin(strUser, strPassword, strAgent);
		return;
	}

	//	检查 token 登录
	if (!strUser.IsEmpty() && !strToken.IsEmpty() && (tokenType == 1 || tokenType == 2))
	{
		switch (tokenType)
		{
		case 1:
			a_LogOutput(1, "CECLoginSwitch::ParseCommandLine, using token");
			TokenLogin(strUser, strToken);
			break;

		case 2:
			a_LogOutput(1, "CECLoginSwitch::ParseCommandLine, using token2");
			Token2Login(strUser, strToken);
			break;
		}
		return;
	}

	//	检查 pwd 登录
	if (!strUser.IsEmpty() && !strPassword.IsEmpty())
	{
		a_LogOutput(1, "CECLoginSwitch::ParseCommandLine, using pwd");
		PasswordLogin(strUser, strPassword);
		return;
	}

	//	不是合法的默认登录方式
}

ACString CECLoginSwitch::GetUser()const
{
	//	获取账号名称
	if (GetLoginType() == LT_DEFAULT)
	{
		CDlgLogin *pDlgLogin = GetLoginDialog();
		return pDlgLogin ? pDlgLogin->GetUser() : ACString();
	}
	return m_strUser;
}

ACString CECLoginSwitch::GetPassword()const
{
	//	获取密码
	switch (GetLoginType())
	{
	case LT_DEFAULT:
		{
			CDlgLogin *pDlgLogin = GetLoginDialog();
			if (pDlgLogin) return pDlgLogin->GetPassword();
		}
		break;

	case LT_PWD:
	case LT_SSO:
	case LT_ARC:
		return m_strPassword;
	}
	
	return ACString();
}

bool CECLoginSwitch::GetKickoutUser()const
{
	if (CECReconnect::Instance().IsReconnecting()){
		return true;
	}
	CDlgLogin *pDlgLogin = GetLoginDialog();
	return pDlgLogin ? pDlgLogin->GetKickoutUser() : false;
}

ACString CECLoginSwitch::GetToken()const
{
	return IsUseToken() ? m_strToken : ACString();
 }

char CECLoginSwitch::GetTokenType()const
{
	char type(0);
	switch (GetLoginType())
	{
	case LT_TOKEN: type=1; break;
	case LT_TOKEN2: type=2; break;
	}
	return type;
}

ACString CECLoginSwitch::GetAgent()const
{
	LoginType type = GetLoginType();
	return (type == LT_SSO || type == LT_ARC) ? m_strAgent : ACString();
}

bool CECLoginSwitch::Validate()const
{
	ACString strUser = GetUser();
	ACString strPassword = GetPassword();
	ACString strToken = GetToken();
	ACString strAgent = GetAgent();

	//	参数非空验证
	if (strUser.IsEmpty())
	{
		a_LogOutput(1, "CECLoginSwitch::Login, user empty");
		return false;	//	账号不可为空
	}
	
	switch (GetLoginType())
	{
	case LT_DEFAULT:
	case LT_PWD:
		if (strPassword.IsEmpty())
		{
			a_LogOutput(1, "CECLoginSwitch::Login, password empty");
			return false;	//	密码登录时 pwd 不可为空
		}
		break;
		
	case LT_TOKEN:
	case LT_TOKEN2:
		if (strToken.IsEmpty())
		{
			a_LogOutput(1, "CECLoginSwitch::Login, token empty");
			return false;	//	token 登录时 token 不可为空
		}
		break;
		
	case LT_SSO:
	case LT_ARC:
		if (strPassword.IsEmpty() || strAgent.IsEmpty())
		{
			AString strError;
			if (strPassword.IsEmpty()) strError += "password";
			if (strAgent.IsEmpty()) strError += "agent";
			a_LogOutput(1, "CECLoginSwitch::Login, %s empty", strError);
			return false;	//	使用 SOO 登录时 pwd 及 agent 都不可为空
		}
		break;
	}

	return true;
}

bool CECLoginSwitch::Login()
{
	glb_RepairExeInMemory();

	if (!Validate())
	{
		DefaultLogin();
		return false;
	}
	
	CECGameSession *pSession = g_pGame->GetGameSession();
	
	ACString strUser = GetUser();

	//	保存账号名供转服用
	CECCrossServer::Instance().SetUser(strUser);

	AString strAgentAccount = pSession->GetAgentAccount();
	if (strAgentAccount.IsEmpty())
	{
		//	第三方登录时账号名称区分大小写，非第三方登录时
		strUser.MakeLower();
	}

	char utf8UserName[256];
	WideCharToMultiByte(CP_UTF8, 0, strUser, -1, utf8UserName, 256, NULL, NULL);
	pSession->SetUserName(utf8UserName, strUser);
	
	ACString strPassword = GetPassword();
	pSession->SetUserPassword(AC2AS(strPassword));
	
	ACString strToken = GetToken();
	pSession->SetLoginToken(AC2AS(strToken), GetTokenType());

	pSession->SetAgentName(AC2AS(GetAgent()));
	pSession->SetKickUserFlag(GetKickoutUser());

	glb_RepairExeInMemory();

	pSession->Open();
	
	if (!CECReconnect::Instance().IsReconnecting()){
		UpdateReconnectPolicy();
	}

	//	无论使用何种方式登录，登录后即恢复默认登录方式
	DefaultLogin();

	glb_RepairExeInMemory();

	return true;
}

void CECLoginSwitch::UpdateReconnectPolicy(){
	if (IsUseCrossServer()){
		return;	//	跨服登录不影响，否则跨服上掉线无法重连
	}
	CECReconnectPolicy *pReconnectPolicyCandidate = NULL;
	if (IsUsePassword()){
		pReconnectPolicyCandidate = new CECReconnectSimpleUserPasswordPolicy(GetUser(), GetPassword());
	}else if (IsUseToken()){
		if (CC_SDK::ArcOverlay::Instance().IsLoaded()){
			pReconnectPolicyCandidate = new CECReconnectForeignArcUserTokenPolicy(GetUser());
		}
	}else if (!CanAutoLogin()){
		if (GetUser() == CECCommandLine::GetUser()){
			pReconnectPolicyCandidate = new CECReconnectSimpleUserPasswordPolicy(GetUser(), GetPassword());
		}
	}
	if (pReconnectPolicyCandidate != NULL){
		CECReconnect::Instance().SetReconnectPolicyCandidate(pReconnectPolicyCandidate);
	}else{
		CECReconnect::Instance().ResetState();
	}
}

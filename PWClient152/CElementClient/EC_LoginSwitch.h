// File		: EC_LoginSwitch.h
// Creator	: Xu Wenbin
// Date		: 2012/5/17

#pragma once

#include <AAssist.h>

class CECLoginSwitch
{
	CECLoginSwitch();

	CECLoginSwitch(const CECLoginSwitch &);
	CECLoginSwitch & operator=(const CECLoginSwitch&);

public:
	static CECLoginSwitch & Instance();

	enum LoginType {
		LT_DEFAULT,			//	普通登录方式（从 CDlgLogin 获取玩家输入的账号密码）
		LT_PWD,				//	password 登录（从命令行读取账号、pwd）
		LT_TOKEN,			//	token 登录（从命令行获取账号、token）
		LT_TOKEN2,			//	token2 登录（从命令行获取账号、token2）
		LT_SSO,				//	sso 登录（从命令行或账号联入网页获取账号、pwd、agent）
		LT_CROSSSERVER,		//	转服登录
		LT_ARC,				//	arc平台自动登陆
	};

	//	设置登录参数
	void CheckAutoLogin();
	void DefaultLogin();
	void PasswordLogin(const ACString &user, const ACString &pwd);
	void TokenLogin(const ACString &user, const ACString &token);
	void Token2Login(const ACString &user, const ACString &token2);
	void SSOLogin(const ACString &user, const ACString &pwd, const ACString &agent);
	void CrossServerLogin(const ACString &user);
	void ArcAsiaLogin();

	void ArcAsiaGetToken();
	void ParseCommandLine();

	//	获取登录参数
	ACString GetUser()const;
	ACString GetPassword()const;
	ACString GetToken()const;
	ACString GetAgent()const;

	LoginType	GetLoginType()const{ return m_loginType; }
	bool		GetKickoutUser()const;

	bool IsUsePassword()const{ return GetLoginType() == LT_PWD; }
	bool IsUseToken()const { return GetLoginType() == LT_TOKEN || GetLoginType() == LT_TOKEN2; }
	bool IsUseSSO()const { return GetLoginType() == LT_SSO; }
	bool IsUseCrossServer()const { return GetLoginType() == LT_CROSSSERVER; }
	bool IsUseArc() const { return GetLoginType() == LT_ARC; }
	bool CanAutoLogin()const { return GetLoginType() != LT_DEFAULT; }
	char GetTokenType()const;

	//	使用已设置参数登录
	bool Login();
	bool Validate()const;

private:
	void UpdateReconnectPolicy();

private:

	LoginType	m_loginType;
	bool		m_bKickoutUser;

	ACString	m_strUser;
	ACString	m_strPassword;
	ACString	m_strToken;
	ACString	m_strAgent;
};
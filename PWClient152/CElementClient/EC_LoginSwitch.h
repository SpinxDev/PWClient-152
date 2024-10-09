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
		LT_DEFAULT,			//	��ͨ��¼��ʽ���� CDlgLogin ��ȡ���������˺����룩
		LT_PWD,				//	password ��¼���������ж�ȡ�˺š�pwd��
		LT_TOKEN,			//	token ��¼���������л�ȡ�˺š�token��
		LT_TOKEN2,			//	token2 ��¼���������л�ȡ�˺š�token2��
		LT_SSO,				//	sso ��¼���������л��˺�������ҳ��ȡ�˺š�pwd��agent��
		LT_CROSSSERVER,		//	ת����¼
		LT_ARC,				//	arcƽ̨�Զ���½
	};

	//	���õ�¼����
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

	//	��ȡ��¼����
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

	//	ʹ�������ò�����¼
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
// Filename	: EC_ArcAsia.h
// Creator	: Han Guanghui
// Date		: 2014/01/22

#pragma once

#include "ArcAsia_API.h"
#include <AAssist.h>

namespace GNET{
	class Octets;
	class SSOGetTicket_Re;
}

class CECArcAsia 
{
public:
	CECArcAsia();
	~CECArcAsia();

	static CECArcAsia& GetSingleton();

	bool IsArcInit(){ return m_hArcSDK != NULL; }
	void Init();
	void UnInit(); //游戏客户端退出时调用。通过Arc起动游戏并自动登录失败时也要调用。
	void GetToken(ACString& user, ACString& password, ACString& agent);
	bool NavigateURL(const ACString& url);
	
	enum {
		SSO_TOAID = 109,
	};
	bool CheckUser(const ACString& user); // 检查当前登录的账号与Arc账号是否一致，不一致的话卸载Arc
	GNET::Octets GetArcAsiaContext();
	bool OnSSOGetTicket_Re(const wchar_t* ticket);
protected:
	
	ARC_ASIA_SDK::CC_HANDLE m_hArcSDK;
	ACString m_strUser;
	ACString m_strUserChanging;
	DWORD	 m_dwGetTokenTime; // 记录调用CC_GetTokenEx的时间，防止两次调用间隔过短，引起登陆失败
};
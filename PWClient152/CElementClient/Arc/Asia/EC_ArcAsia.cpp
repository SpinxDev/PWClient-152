// Filename	: EC_ArcAsia.cpp
// Creator	: Han Guanghui
// Date		: 2014/01/22

#include "EC_ArcAsia.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_TimeSafeChecker.h"
#include "gnproto.h"
#include <ALog.h>
#include "AWString.h"

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

static const DWORD s_MinTimeBetweenGetToken = 500;

CECArcAsia::CECArcAsia():
m_hArcSDK(NULL),
m_dwGetTokenTime(0)
{

}

CECArcAsia::~CECArcAsia()
{
	UnInit();
}

CECArcAsia& CECArcAsia::GetSingleton()
{
	static CECArcAsia arc;
	return arc;
}

void CECArcAsia::Init()
{
	if (m_hArcSDK == NULL) {
		m_hArcSDK = ARC_ASIA_SDK::CC_Init(0, 0);
		m_strUser = _AL("");
		if (m_hArcSDK != NULL) a_LogOutput(1, "ARC_ASIA_SDK::CC_Init, Succeed.");
		else a_LogOutput(1, "ARC_ASIA_SDK::CC_Init, Failed!");
	}
}

void CECArcAsia::UnInit()
{
	if (m_hArcSDK != NULL) {
		ARC_ASIA_SDK::CC_UnInit(m_hArcSDK);
		m_hArcSDK = NULL;
		m_strUser = _AL("");
	}
}
void CECArcAsia::GetToken(ACString& user, ACString& password, ACString& agent)
{
	DWORD dwTick = GetTickCount();
	if( CECTimeSafeChecker::ElapsedTime(dwTick, m_dwGetTokenTime) > s_MinTimeBetweenGetToken) {
		m_dwGetTokenTime = dwTick;
		if (m_hArcSDK != NULL) {
			HRESULT hRes = S_OK ;
			wchar_t szAccount[MAX_PATH] = {0};
			wchar_t szToken[MAX_PATH] = {0};
			wchar_t szAgent[MAX_PATH] = {0};
			hRes = ARC_ASIA_SDK::CC_GetTokenEx(m_hArcSDK, 
				szAccount, 
				_countof(szAccount), 
				szToken, 
				_countof(szToken), 
				szAgent, 
				_countof(szAgent), 
				10*1000);
			if (hRes == S_OK) {
				user = szAccount;
				password = szToken;
				agent = szAgent;
				m_strUser = szAccount;
				if (m_strUser.Right(4) == _AL("@sso")) m_strUser.CutRight(4);
				a_LogOutput(1, "ARC_ASIA_SDK::CC_GetTokenEx, Succeed.");
			} else {
				a_LogOutput(1, "ARC_ASIA_SDK::CC_GetTokenEx, Failed! ret code = %d", hRes);
			}
		}
	} else {
		a_LogOutput(1, "Another CECArcAsia::GetToken called in too short time!");
	}
}

bool CECArcAsia::NavigateURL(const ACString& url)
{
	if (m_hArcSDK != NULL) {
		ACString strUrl(url);
		HRESULT hRes = ARC_ASIA_SDK::CC_GotoUrlInOverlay(m_hArcSDK, strUrl.GetBuffer(0));
		return hRes == S_OK;
	}
	return false;
}

bool CECArcAsia::CheckUser(const ACString& user)
{
	bool user_same_as_arc(true);

	if (m_hArcSDK != NULL) {
		ACString user_no_sso(user);
		if (user_no_sso.Right(4) == _AL("@sso")) user_no_sso.CutRight(4);
		if (m_strUser != user_no_sso) {
			m_strUserChanging = user_no_sso;
			//	ÉêÇë ticket
			GNET::Marshal::OctetsStream info;
			g_pGame->GetGameSession()->sso_GetTicket(GetArcAsiaContext(), info, SSO_TOAID);
			user_same_as_arc = false;
			a_LogOutput(1, "CECArcAsia::CheckUser, sso_GetTicket called.");
		}
	} else user_same_as_arc = false;

	return user_same_as_arc;
}

GNET::Octets CECArcAsia::GetArcAsiaContext()
{
	const char *szContext = "ticket_arc";
	return GNET::Octets(szContext, strlen(szContext));
}

bool CECArcAsia::OnSSOGetTicket_Re(const wchar_t* ticket)
{
	HRESULT hRes = ARC_ASIA_SDK::CC_UserChanged(m_hArcSDK, const_cast<wchar_t*>((const wchar_t*)m_strUserChanging), const_cast<wchar_t*>(ticket));
	if (hRes == S_OK) {
		m_strUser = m_strUserChanging;
		a_LogOutput(1, "ARC_ASIA_SDK::CC_UserChanged, Succeed.");
		return true;
	} else {
		a_LogOutput(1, "ARC_ASIA_SDK::CC_UserChanged, Failed! ret code = %d", hRes);
		return false;
	}
}
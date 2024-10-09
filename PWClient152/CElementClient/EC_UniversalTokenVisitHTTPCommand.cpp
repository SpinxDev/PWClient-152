// File		: EC_UniversalTokenVisitHTTPCommand.cpp
// Creator	: Xu Wenbin
// Date		: 2014/4/11

#include "EC_UniversalTokenVisitHTTPCommand.h"

#include "EC_Global.h"
#include "Network/IOLib/gnoctets.h"
#include "Network/ssogetticket_re.hpp"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_UIManager.h"
#include "EC_BaseUIMan.h"
#include "ExpTypes.h"

//	外部引用变量
extern CECGame * g_pGame;

//	局部函数
static GNET::Octets GetUniversalTokenVisitHTTPTicketContext()
{
	AString strContext;
	int s_ticketContextCounter = 0;
	strContext.Format("ticket_universal_token_%d", s_ticketContextCounter++);
	return GNET::Octets(strContext, strContext.GetLength());
}

//	class CECUniversalTokenHTTPOSNavigator
#include "EC_URLOSNavigator.h"
#include "EC_UIHelper.h"

class CECUniversalTokenHTTPOSNavigator : public CECURLOSNavigator{
	AString	m_strHTTPLinkFormat;
	bool	m_addTicketInfo;
	int		m_ticketInfoType;
	GNET::Octets m_ticketContext;
public:
	CECUniversalTokenHTTPOSNavigator(const AString &strHTTPLinkFormat, bool addTicketInfo, int ticketInfoType)
		: m_strHTTPLinkFormat(strHTTPLinkFormat)
		, m_addTicketInfo(addTicketInfo)
		, m_ticketInfoType(ticketInfoType)
	{
		CECUniversalTokenHTTPOSNavigatorTicketHandler::Instance().RegisterHandler(this);
	}
	virtual ~CECUniversalTokenHTTPOSNavigator(){
		CECUniversalTokenHTTPOSNavigatorTicketHandler::Instance().UnregisterHandler(this);
	}
	//	override CECURLOSNavigator
	virtual bool CanNavigateNow()const{
		return g_pGame->GetGameSession()->IsConnected();
	}
	virtual void OnClickNavigate(){
		if (!CanNavigateNow()){
			return;
		}
		GNET::Marshal::OctetsStream info;
		if (m_addTicketInfo){
			info << static_cast<CECGameSession::SSOTicketInfoType>(m_ticketInfoType);
		}
		m_ticketContext = AppendNavigateInOSSuffix(GetUniversalTokenVisitHTTPTicketContext());
		g_pGame->GetGameSession()->sso_GetTicket(m_ticketContext, info);
	}
	virtual bool AutoRelease()const{
		return true;
	}
	//	override CECSSOTicketHandler
	virtual bool HandleRequest(const CECSSOTicketHandler::Request *p){
		bool result(false);
		if (p->retcode == 0 &&
			p->local_context == m_ticketContext){
			AString strTicket((const char *)p->ticket.begin(), p->ticket.size());
			if (strTicket.IsEmpty()){
				a_LogOutput(1, "CECUniversalTokenHTTPOSNavigator::HandleRequest, ticket is empty.");
			}else{
				AString strURL;
				strURL.Format(m_strHTTPLinkFormat, strTicket, g_pGame->GetGameSession()->GetTicketAccount());
				if (HasNavigateInOSSuffix(m_ticketContext)){
					CECUIHelper::GetBaseUIMan()->NavigateURL(strURL, NULL);
				}else{
					CECUIHelper::GetBaseUIMan()->NavigateURL(strURL, "Win_Explorer", false, this);
				}
			}
			result = true;
		}
		return result;
	}
};

//	class CECUniversalTokenHTTPOSNavigatorTicketHandler
ELEMENTCLIENT_DEFINE_SINGLETON(CECUniversalTokenHTTPOSNavigatorTicketHandler);

void CECUniversalTokenHTTPOSNavigatorTicketHandler::Clear(){
	m_handlers.clear();
}

void CECUniversalTokenHTTPOSNavigatorTicketHandler::RegisterHandler(CECUniversalTokenHTTPOSNavigator *p){
	if (std::find(m_handlers.begin(), m_handlers.end(), p) == m_handlers.end()){
		m_handlers.push_back(p);
	}
}

void CECUniversalTokenHTTPOSNavigatorTicketHandler::UnregisterHandler(CECUniversalTokenHTTPOSNavigator *p){
	m_handlers.erase(std::remove(m_handlers.begin(), m_handlers.end(), p), m_handlers.end());
}

void CECUniversalTokenHTTPOSNavigatorTicketHandler::HandleRequest(const CECSSOTicketHandler::Request *p){
	for (TicketHandlers::iterator it = m_handlers.begin(); it != m_handlers.end(); ++ it){
		if ((*it)->HandleRequest(p)){
			return;
		}
	}
	CECSSOTicketHandler::HandleRequest(p);
}

//	class CECUniversalTokenVisitHTTPCommand
CECUniversalTokenVisitHTTPCommand::CECUniversalTokenVisitHTTPCommand(const UNIVERSAL_TOKEN_ESSENCE *pDBEssence, int usageIndex)
: CECUseUniversalTokenCommand(pDBEssence, usageIndex)
, m_useIngameIE(true)
, m_addTicketInfo(false)
, m_ticketInfoType(0)
{
	if (pDBEssence && usageIndex >= 0 && usageIndex < ELEMENTDATA_MAX_UNIVERSAL_TOKEN_USAGE){
		int configID = pDBEssence->config_id[usageIndex];
		const char *szSec = "ItemLink";
		AString strKey;
		strKey.Format("URL%d", configID);
		CECBaseUIMan *pBaseUIMan = g_pGame->GetGameRun()->GetUIManager()->GetBaseUIMan();
		m_strHTTPLinkFormat = pBaseUIMan->GetURL(szSec, strKey);

		strKey.Format("INGAMEIE%d", configID);
		AString strUseInGameIE = pBaseUIMan->GetURL(szSec, strKey);
		if (!strUseInGameIE.IsEmpty()){
			m_useIngameIE = strUseInGameIE.ToInt() > 0;
		}

		strKey.Format("INFOTYPE%d", configID);
		AString strTicketInfoType = pBaseUIMan->GetURL(szSec, strKey);
		if (!strTicketInfoType.IsEmpty()){
			int ticketInfoType = strTicketInfoType.ToInt();
			if (ticketInfoType > 0){
				m_ticketInfoType = ticketInfoType;
				m_addTicketInfo = true;
			}
		}
	}else{
		ASSERT(false);
	}
}

bool CECUniversalTokenVisitHTTPCommand::UseAndNeedFeedBack()
{
	bool usedAndNeedFeedBack(false);
	if (!m_strHTTPLinkFormat.IsEmpty() &&
		g_pGame->GetGameSession()->IsConnected()){
		GNET::Marshal::OctetsStream info;
		if (m_addTicketInfo){
			info << static_cast<CECGameSession::SSOTicketInfoType>(m_ticketInfoType);
		}
		m_ticketContext = GetUniversalTokenVisitHTTPTicketContext();
		g_pGame->GetGameSession()->sso_GetTicket(m_ticketContext, info);
		usedAndNeedFeedBack = true;
	}
	return usedAndNeedFeedBack;
}

bool CECUniversalTokenVisitHTTPCommand::FeedBack(int usageIndex, const void *p)
{
	bool bProcessed(false);
	if (usageIndex == GetUsageIndex()){
		const CECSSOTicketHandler::Request *pRequest = (const CECSSOTicketHandler::Request *)p;
		if (pRequest->retcode == 0 && pRequest->local_context == m_ticketContext){
			AString strTicket((const char *)pRequest->ticket.begin(), pRequest->ticket.size());
			if (strTicket.IsEmpty()){
				a_LogOutput(1, "CECUniversalTokenVisitHTTPCommand::FeedBack, ticket is empty.");
			}else if (!m_strHTTPLinkFormat.IsEmpty()){
				AString strAccount = g_pGame->GetGameSession()->GetTicketAccount();
				AString strURL;
				strURL.Format(m_strHTTPLinkFormat, strTicket, strAccount);
				CECBaseUIMan *pBaseUIMan = g_pGame->GetGameRun()->GetUIManager()->GetBaseUIMan();
				if (m_useIngameIE){
					pBaseUIMan->NavigateURL(strURL, "Win_Explorer", false,
						new CECUniversalTokenHTTPOSNavigator(m_strHTTPLinkFormat, m_addTicketInfo, m_ticketInfoType));
				}else{
					pBaseUIMan->NavigateURL(strURL, NULL);
				}
			}
			bProcessed = true;
		}
	}
	return bProcessed;
}
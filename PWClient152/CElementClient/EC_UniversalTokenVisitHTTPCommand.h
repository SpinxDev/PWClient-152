// File		: EC_UniversalTokenVisitHTTPCommand.h
// Creator	: Xu Wenbin
// Date		: 2014/4/11

#ifndef _ELEMENTCLIENT_EC_UNIVERSALTOKENVISITHTTPCOMMAND_H_
#define _ELEMENTCLIENT_EC_UNIVERSALTOKENVISITHTTPCOMMAND_H_

#include "EC_UseUniversalToken.h"
#include "gnoctets.h"
#include "EC_Handler.h"
#include "EC_Global.h"

#include <AString.h>
#include <vector.h>

//	class CECUniversalTokenVisitHTTPCommand
class CECUniversalTokenVisitHTTPCommand : public CECUseUniversalTokenCommand
{
	AString	m_strHTTPLinkFormat;
	bool	m_useIngameIE;
	bool	m_addTicketInfo;
	int		m_ticketInfoType;
	GNET::Octets m_ticketContext;
public:
	CECUniversalTokenVisitHTTPCommand(const UNIVERSAL_TOKEN_ESSENCE *pDBEssence, int usageIndex);
	virtual bool UseAndNeedFeedBack();
	virtual bool FeedBack(int usageIndex, const void *p);
};

class CECUniversalTokenHTTPOSNavigator;
class CECUniversalTokenHTTPOSNavigatorTicketHandler : public CECSSOTicketHandler{
	typedef abase::vector<CECUniversalTokenHTTPOSNavigator *> TicketHandlers;
	TicketHandlers	m_handlers;

	ELEMENTCLIENT_DECLARE_SINGLETON(CECUniversalTokenHTTPOSNavigatorTicketHandler);

public:
	void Clear();
	void RegisterHandler(CECUniversalTokenHTTPOSNavigator *);
	void UnregisterHandler(CECUniversalTokenHTTPOSNavigator *);
	virtual void HandleRequest(const CECSSOTicketHandler::Request *p);
};

#endif	//	_ELEMENTCLIENT_EC_UNIVERSALTOKENVISITHTTPCOMMAND_H_
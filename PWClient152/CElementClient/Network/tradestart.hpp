
#ifndef __GNET_TRADESTART_HPP
#define __GNET_TRADESTART_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "tradestart_re.hpp"
namespace GNET
{

class TradeStart : public GNET::Protocol
{
	#include "tradestart"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
		//manager->Send(sid,TradeStart_Re(ERR_SUCCESS,_TRADE_ID_INVALID,partner_roleid,roleid,localsid));
		//manager->Send(sid,TradeStart_Re(ERR_TRADE_REFUSE,_TRADE_ID_INVALID,partner_roleid,roleid,localsid));
	}
};

};

#endif

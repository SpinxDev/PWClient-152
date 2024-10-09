
#ifndef __GNET_EXCHANGECONSUMEPOINTS_HPP
#define __GNET_EXCHANGECONSUMEPOINTS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class ExchangeConsumePoints : public GNET::Protocol
{
	#include "exchangeconsumepoints"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

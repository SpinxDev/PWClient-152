
#ifndef __GNET_EXCHANGECONSUMEPOINTS_RE_HPP
#define __GNET_EXCHANGECONSUMEPOINTS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class ExchangeConsumePoints_Re : public GNET::Protocol
{
	#include "exchangeconsumepoints_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

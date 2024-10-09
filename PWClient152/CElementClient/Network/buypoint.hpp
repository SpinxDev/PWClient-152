
#ifndef __GNET_BUYPOINT_HPP
#define __GNET_BUYPOINT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class BuyPoint : public GNET::Protocol
{
	#include "buypoint"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_KEEPALIVE_HPP
#define __GNET_KEEPALIVE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class KeepAlive : public GNET::Protocol
{
	#include "keepalive"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_GMSHUTUPROLE_HPP
#define __GNET_GMSHUTUPROLE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMShutupRole : public GNET::Protocol
{
	#include "gmshutuprole"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_SELECTROLE_HPP
#define __GNET_SELECTROLE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SelectRole : public GNET::Protocol
{
	#include "selectrole"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_GMKICKOUTUSER_HPP
#define __GNET_GMKICKOUTUSER_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMKickoutUser : public GNET::Protocol
{
	#include "gmkickoutuser"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

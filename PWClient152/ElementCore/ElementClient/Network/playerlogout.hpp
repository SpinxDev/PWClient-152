
#ifndef __GNET_PLAYERLOGOUT_HPP
#define __GNET_PLAYERLOGOUT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class PlayerLogout : public GNET::Protocol
{
	#include "playerlogout"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

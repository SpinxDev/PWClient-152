
#ifndef __GNET_PLAYERACCUSE_HPP
#define __GNET_PLAYERACCUSE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PlayerAccuse : public GNET::Protocol
{
	#include "playeraccuse"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

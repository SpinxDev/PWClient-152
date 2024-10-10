
#ifndef __GNET_GMLISTONLINEUSER_HPP
#define __GNET_GMLISTONLINEUSER_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMListOnlineUser : public GNET::Protocol
{
	#include "gmlistonlineuser"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

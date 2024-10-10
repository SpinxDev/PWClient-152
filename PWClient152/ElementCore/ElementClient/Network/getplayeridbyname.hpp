
#ifndef __GNET_GETPLAYERIDBYNAME_HPP
#define __GNET_GETPLAYERIDBYNAME_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetPlayerIDByName : public GNET::Protocol
{
	#include "getplayeridbyname"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_ACREMOTECODE_HPP
#define __GNET_ACREMOTECODE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ACRemoteCode : public GNET::Protocol
{
	#include "acremotecode"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

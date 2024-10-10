
#ifndef __GNET_DELETEROLE_HPP
#define __GNET_DELETEROLE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class DeleteRole : public GNET::Protocol
{
	#include "deleterole"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

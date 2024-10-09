
#ifndef __GNET_GMKICKOUTROLE_HPP
#define __GNET_GMKICKOUTROLE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMKickoutRole : public GNET::Protocol
{
	#include "gmkickoutrole"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_GETHELPSTATES_HPP
#define __GNET_GETHELPSTATES_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetHelpStates : public GNET::Protocol
{
	#include "gethelpstates"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

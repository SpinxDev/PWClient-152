
#ifndef __GNET_SETHELPSTATES_HPP
#define __GNET_SETHELPSTATES_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SetHelpStates : public GNET::Protocol
{
	#include "sethelpstates"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

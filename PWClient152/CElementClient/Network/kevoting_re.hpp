
#ifndef __GNET_KEVOTING_RE_HPP
#define __GNET_KEVOTING_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class KEVoting_Re : public GNET::Protocol
{
	#include "kevoting_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

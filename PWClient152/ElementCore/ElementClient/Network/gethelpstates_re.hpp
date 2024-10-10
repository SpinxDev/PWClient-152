
#ifndef __GNET_GETHELPSTATES_RE_HPP
#define __GNET_GETHELPSTATES_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetHelpStates_Re : public GNET::Protocol
{
	#include "gethelpstates_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

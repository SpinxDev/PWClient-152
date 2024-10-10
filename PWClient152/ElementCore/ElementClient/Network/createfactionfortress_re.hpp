
#ifndef __GNET_CREATEFACTIONFORTRESS_RE_HPP
#define __GNET_CREATEFACTIONFORTRESS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gfactionfortressbriefinfo"

namespace GNET
{

class CreateFactionFortress_Re : public GNET::Protocol
{
	#include "createfactionfortress_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

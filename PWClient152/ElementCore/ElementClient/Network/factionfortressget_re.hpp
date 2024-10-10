
#ifndef __GNET_FACTIONFORTRESSGET_RE_HPP
#define __GNET_FACTIONFORTRESSGET_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gfactionfortressbriefinfo"

namespace GNET
{

class FactionFortressGet_Re : public GNET::Protocol
{
	#include "factionfortressget_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

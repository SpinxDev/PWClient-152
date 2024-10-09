
#ifndef __GNET_FACTIONLEAVE_RE_HPP
#define __GNET_FACTIONLEAVE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionLeave_Re : public GNET::Protocol
{
	#include "factionleave_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

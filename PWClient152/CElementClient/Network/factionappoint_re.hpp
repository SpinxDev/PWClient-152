
#ifndef __GNET_FACTIONAPPOINT_RE_HPP
#define __GNET_FACTIONAPPOINT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionAppoint_Re : public GNET::Protocol
{
	#include "factionappoint_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

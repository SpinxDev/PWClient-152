
#ifndef __GNET_FACTIONCREATE_RE_HPP
#define __GNET_FACTIONCREATE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionCreate_Re : public GNET::Protocol
{
	#include "factioncreate_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO

	}
};

};

#endif

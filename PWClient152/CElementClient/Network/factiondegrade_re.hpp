
#ifndef __GNET_FACTIONDEGRADE_RE_HPP
#define __GNET_FACTIONDEGRADE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionDegrade_Re : public GNET::Protocol
{
	#include "factiondegrade_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

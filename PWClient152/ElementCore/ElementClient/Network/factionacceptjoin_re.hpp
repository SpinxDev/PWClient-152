
#ifndef __GNET_FACTIONACCEPTJOIN_RE_HPP
#define __GNET_FACTIONACCEPTJOIN_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionAcceptJoin_Re : public GNET::Protocol
{
	#include "factionacceptjoin_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

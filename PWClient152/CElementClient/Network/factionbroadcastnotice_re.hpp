
#ifndef __GNET_FACTIONBROADCASTNOTICE_RE_HPP
#define __GNET_FACTIONBROADCASTNOTICE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionBroadcastNotice_Re : public GNET::Protocol
{
	#include "factionbroadcastnotice_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

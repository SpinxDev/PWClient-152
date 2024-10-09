
#ifndef __GNET_FACTIONRESIGN_RE_HPP
#define __GNET_FACTIONRESIGN_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionResign_Re : public GNET::Protocol
{
	#include "factionresign_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

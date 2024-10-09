
#ifndef __GNET_FACTIONCHANGPROCLAIM_RE_HPP
#define __GNET_FACTIONCHANGPROCLAIM_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionChangProclaim_Re : public GNET::Protocol
{
	#include "factionchangproclaim_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

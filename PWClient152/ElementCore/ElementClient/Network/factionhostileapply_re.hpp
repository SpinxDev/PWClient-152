
#ifndef __GNET_FACTIONHOSTILEAPPLY_RE_HPP
#define __GNET_FACTIONHOSTILEAPPLY_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionHostileApply_Re : public GNET::Protocol
{
	#include "factionhostileapply_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_FACTIONHOSTILEREPLY_RE_HPP
#define __GNET_FACTIONHOSTILEREPLY_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionHostileReply_Re : public GNET::Protocol
{
	#include "factionhostilereply_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

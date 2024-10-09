
#ifndef __GNET_FACTIONDELAYEXPELANNOUNCE_HPP
#define __GNET_FACTIONDELAYEXPELANNOUNCE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionDelayExpelAnnounce : public GNET::Protocol
{
	#include "factiondelayexpelannounce"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_FACTIONLISTONLINE_HPP
#define __GNET_FACTIONLISTONLINE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionListOnline : public GNET::Protocol
{
	#include "factionlistonline"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

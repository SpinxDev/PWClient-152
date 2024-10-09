
#ifndef __GNET_FACTIONFORTRESSLIST_HPP
#define __GNET_FACTIONFORTRESSLIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionFortressList : public GNET::Protocol
{
	#include "factionfortresslist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

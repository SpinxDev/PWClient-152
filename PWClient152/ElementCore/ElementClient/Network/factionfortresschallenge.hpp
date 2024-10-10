
#ifndef __GNET_FACTIONFORTRESSCHALLENGE_HPP
#define __GNET_FACTIONFORTRESSCHALLENGE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionFortressChallenge : public GNET::Protocol
{
	#include "factionfortresschallenge"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

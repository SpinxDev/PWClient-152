
#ifndef __GNET_FACTIONFORTRESSCHALLENGE_RE_HPP
#define __GNET_FACTIONFORTRESSCHALLENGE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionFortressChallenge_Re : public GNET::Protocol
{
	#include "factionfortresschallenge_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

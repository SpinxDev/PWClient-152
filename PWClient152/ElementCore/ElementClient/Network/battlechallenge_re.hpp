
#ifndef __GNET_BATTLECHALLENGE_RE_HPP
#define __GNET_BATTLECHALLENGE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class BattleChallenge_Re : public GNET::Protocol
{
	#include "battlechallenge_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

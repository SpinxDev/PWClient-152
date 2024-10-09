
#ifndef __GNET_COUNTRYBATTLEGETKINGCOMMANDPOINT_HPP
#define __GNET_COUNTRYBATTLEGETKINGCOMMANDPOINT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleGetKingCommandPoint : public GNET::Protocol
{
	#include "countrybattlegetkingcommandpoint"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

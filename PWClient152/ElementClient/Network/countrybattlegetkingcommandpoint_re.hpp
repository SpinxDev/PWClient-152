
#ifndef __GNET_COUNTRYBATTLEGETKINGCOMMANDPOINT_RE_HPP
#define __GNET_COUNTRYBATTLEGETKINGCOMMANDPOINT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleGetKingCommandPoint_Re : public GNET::Protocol
{
	#include "countrybattlegetkingcommandpoint_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_COUNTRYBATTLEKINGASSIGNASSAULT_RE_HPP
#define __GNET_COUNTRYBATTLEKINGASSIGNASSAULT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleKingAssignAssault_Re : public GNET::Protocol
{
	#include "countrybattlekingassignassault_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

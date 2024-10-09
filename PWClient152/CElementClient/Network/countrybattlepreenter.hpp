
#ifndef __GNET_COUNTRYBATTLEPREENTER_HPP
#define __GNET_COUNTRYBATTLEPREENTER_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattlePreEnter : public GNET::Protocol
{
	#include "countrybattlepreenter"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

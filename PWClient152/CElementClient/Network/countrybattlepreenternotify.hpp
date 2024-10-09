
#ifndef __GNET_COUNTRYBATTLEPREENTERNOTIFY_HPP
#define __GNET_COUNTRYBATTLEPREENTERNOTIFY_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattlePreEnterNotify : public GNET::Protocol
{
	#include "countrybattlepreenternotify"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

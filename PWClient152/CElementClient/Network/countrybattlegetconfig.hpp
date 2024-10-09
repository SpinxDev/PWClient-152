
#ifndef __GNET_COUNTRYBATTLEGETCONFIG_HPP
#define __GNET_COUNTRYBATTLEGETCONFIG_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleGetConfig : public GNET::Protocol
{
	#include "countrybattlegetconfig"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

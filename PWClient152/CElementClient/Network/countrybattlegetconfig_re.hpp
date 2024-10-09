
#ifndef __GNET_COUNTRYBATTLEGETCONFIG_RE_HPP
#define __GNET_COUNTRYBATTLEGETCONFIG_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CountryBattleGetConfig_Re : public GNET::Protocol
{
	#include "countrybattlegetconfig_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

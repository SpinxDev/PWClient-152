
#ifndef __GNET_COUNTRYBATTLEGETBATTLELIMIT_HPP
#define __GNET_COUNTRYBATTLEGETBATTLELIMIT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gcountrybattlelimit"

namespace GNET
{

class CountryBattleGetBattleLimit : public GNET::Protocol
{
	#include "countrybattlegetbattlelimit"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

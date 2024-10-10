
#ifndef __GNET_COUNTRYBATTLEGETBATTLELIMIT_RE_HPP
#define __GNET_COUNTRYBATTLEGETBATTLELIMIT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gcountrybattlelimit"

namespace GNET
{

class CountryBattleGetBattleLimit_Re : public GNET::Protocol
{
	#include "countrybattlegetbattlelimit_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

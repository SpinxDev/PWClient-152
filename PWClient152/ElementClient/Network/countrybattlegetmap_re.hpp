
#ifndef __GNET_COUNTRYBATTLEGETMAP_RE_HPP
#define __GNET_COUNTRYBATTLEGETMAP_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gcountrybattledomain"

namespace GNET
{

class CountryBattleGetMap_Re : public GNET::Protocol
{
	#include "countrybattlegetmap_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

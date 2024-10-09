
#ifndef __GNET_BATTLEGETMAP_RE_HPP
#define __GNET_BATTLEGETMAP_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "gterritory"

namespace GNET
{

class BattleGetMap_Re : public GNET::Protocol
{
	#include "battlegetmap_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

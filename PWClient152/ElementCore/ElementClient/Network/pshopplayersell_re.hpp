
#ifndef __GNET_PSHOPPLAYERSELL_RE_HPP
#define __GNET_PSHOPPLAYERSELL_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "pshopitem"
#include "groleinventory"

namespace GNET
{

class PShopPlayerSell_Re : public GNET::Protocol
{
	#include "pshopplayersell_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

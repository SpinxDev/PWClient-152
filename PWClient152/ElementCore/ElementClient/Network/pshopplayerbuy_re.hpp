
#ifndef __GNET_PSHOPPLAYERBUY_RE_HPP
#define __GNET_PSHOPPLAYERBUY_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "pshopitem"

namespace GNET
{

class PShopPlayerBuy_Re : public GNET::Protocol
{
	#include "pshopplayerbuy_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

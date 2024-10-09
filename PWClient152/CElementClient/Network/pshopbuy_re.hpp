
#ifndef __GNET_PSHOPBUY_RE_HPP
#define __GNET_PSHOPBUY_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "pshopitem"

namespace GNET
{

class PShopBuy_Re : public GNET::Protocol
{
	#include "pshopbuy_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

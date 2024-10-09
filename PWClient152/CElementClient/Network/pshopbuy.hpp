
#ifndef __GNET_PSHOPBUY_HPP
#define __GNET_PSHOPBUY_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopBuy : public GNET::Protocol
{
	#include "pshopbuy"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

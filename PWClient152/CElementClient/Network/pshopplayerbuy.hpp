
#ifndef __GNET_PSHOPPLAYERBUY_HPP
#define __GNET_PSHOPPLAYERBUY_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopPlayerBuy : public GNET::Protocol
{
	#include "pshopplayerbuy"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

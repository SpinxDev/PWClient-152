
#ifndef __GNET_PSHOPPLAYERSELL_HPP
#define __GNET_PSHOPPLAYERSELL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopPlayerSell : public GNET::Protocol
{
	#include "pshopplayersell"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

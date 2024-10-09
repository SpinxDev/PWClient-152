
#ifndef __GNET_PSHOPLIST_HPP
#define __GNET_PSHOPLIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopList : public GNET::Protocol
{
	#include "pshoplist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

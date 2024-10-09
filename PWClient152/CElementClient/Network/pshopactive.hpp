
#ifndef __GNET_PSHOPACTIVE_HPP
#define __GNET_PSHOPACTIVE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopActive : public GNET::Protocol
{
	#include "pshopactive"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

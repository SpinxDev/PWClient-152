
#ifndef __GNET_PSHOPLISTITEM_HPP
#define __GNET_PSHOPLISTITEM_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopListItem : public GNET::Protocol
{
	#include "pshoplistitem"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

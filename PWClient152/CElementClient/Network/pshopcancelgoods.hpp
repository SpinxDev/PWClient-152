
#ifndef __GNET_PSHOPCANCELGOODS_HPP
#define __GNET_PSHOPCANCELGOODS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopCancelGoods : public GNET::Protocol
{
	#include "pshopcancelgoods"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_PSHOPPLAYERGET_HPP
#define __GNET_PSHOPPLAYERGET_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopPlayerGet : public GNET::Protocol
{
	#include "pshopplayerget"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

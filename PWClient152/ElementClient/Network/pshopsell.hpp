
#ifndef __GNET_PSHOPSELL_HPP
#define __GNET_PSHOPSELL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopSell : public GNET::Protocol
{
	#include "pshopsell"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

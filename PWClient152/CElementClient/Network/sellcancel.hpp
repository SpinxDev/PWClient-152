
#ifndef __GNET_SELLCANCEL_HPP
#define __GNET_SELLCANCEL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SellCancel : public GNET::Protocol
{
	#include "sellcancel"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_SELLPOINT_HPP
#define __GNET_SELLPOINT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SellPoint : public GNET::Protocol
{
	#include "sellpoint"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif


#ifndef __GNET_PSHOPSELFGET_HPP
#define __GNET_PSHOPSELFGET_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopSelfGet : public GNET::Protocol
{
	#include "pshopselfget"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

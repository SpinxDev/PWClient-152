
#ifndef __GNET_PSHOPACTIVE_RE_HPP
#define __GNET_PSHOPACTIVE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class PShopActive_Re : public GNET::Protocol
{
	#include "pshopactive_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif

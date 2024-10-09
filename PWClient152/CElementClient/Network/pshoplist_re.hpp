
#ifndef __GNET_PSHOPLIST_RE_HPP
#define __GNET_PSHOPLIST_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "pshopentry"

namespace GNET
{

class PShopList_Re : public GNET::Protocol
{
	#include "pshoplist_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
